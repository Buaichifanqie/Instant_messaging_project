#pragma once
#include<atomic>
#include<memory>
#include <cassert>

// 无锁队列模板类
template<typename T>
class MyQueue
{
private:

	// 节点引用计数结构体，使用位域存储内部计数和外部计数器数目
	struct node_counter
	{
		unsigned internal_count : 30; // 内部计数，占用30位
		unsigned external_counters : 2; // 外部计数器数目，占用2位
	};

	// 队列节点结构体
	struct node;

	// 保存节点指针和外部引用计数的结构体
	struct counted_node_ptr
	{
		counted_node_ptr() :external_count(0), ptr(nullptr) {} // 默认构造函数
		int external_count; // 外部引用计数
		node* ptr; // 指向节点的指针
	};

	// 队列节点结构体
	struct node
	{
		std::atomic<T*> data; // 原子性地存储数据指针
		std::atomic<node_counter> count; // 原子性地存储引用计数信息
		std::atomic<counted_node_ptr> next; // 原子性地存储下一个节点的信息

		// 构造函数，初始化节点
		node(int external_count = 2)
		{
			node_counter new_count;
			new_count.internal_count = 0; // 初始化内部计数为0
			new_count.external_counters = external_count; // 设置外部计数器数目
			count.store(new_count); // 存储引用计数信息

			counted_node_ptr node_ptr;
			node_ptr.ptr = nullptr; // 初始化下一个节点指针为空
			node_ptr.external_count = 0;
			next.store(node_ptr);
		}

		// 释放节点引用，减少内部计数，当计数为0时删除节点
		void release_ref()
		{
			node_counter old_counter = count.load(std::memory_order_relaxed);
			node_counter new_counter;
			do
			{
				new_counter = old_counter;
				--new_counter.internal_count; // 减少内部计数
			} while (!count.compare_exchange_strong(
				old_counter, new_counter,
				std::memory_order_acquire, std::memory_order_relaxed));

			// 如果内部和外部计数都为0，删除节点
			if (!new_counter.internal_count && !new_counter.external_counters)
			{
				delete this;
				destruct_count.fetch_add(1); // 增加销毁计数（用于调试）
			}
		}
	};

	// 原子性地存储头节点信息
	std::atomic<counted_node_ptr> head;
	// 原子性地存储尾节点信息
	std::atomic<counted_node_ptr> tail;

	// 设置新尾节点，处理并发更新
	void set_new_tail(counted_node_ptr& old_tail, counted_node_ptr const& new_tail)
	{
		node* const current_tail_ptr = old_tail.ptr; // 保存旧尾节点指针
		// CAS循环直到成功或tail被其他线程更新
		while (!tail.compare_exchange_weak(old_tail, new_tail) &&
			old_tail.ptr == current_tail_ptr);
		// 如果尾节点未被其他线程修改，释放旧尾节点
		if (old_tail.ptr == current_tail_ptr)
			free_external_counter(old_tail);
		else
			// 如果尾节点已被其他线程修改，减少旧尾节点的引用计数
			current_tail_ptr->release_ref();
	}

	// 释放外部计数器并可能删除节点
	static void free_external_counter(counted_node_ptr& old_node_ptr)
	{
		node* const ptr = old_node_ptr.ptr; // 获取节点指针
		int const count_increase = old_node_ptr.external_count - 2; // 计算引用差
		node_counter old_counter = ptr->count.load(std::memory_order_relaxed); // 获取当前引用计数信息
		node_counter new_counter;
		do
		{
			new_counter = old_counter;
			--new_counter.external_counters; // 减少外部计数器
			new_counter.internal_count += count_increase; // 调整内部计数
		} while (!ptr->count.compare_exchange_strong(
			old_counter, new_counter,
			std::memory_order_acquire, std::memory_order_relaxed));

		// 如果内部计数和外部计数都为0，删除节点
		if (!new_counter.internal_count && !new_counter.external_counters)
		{
			delete ptr;
			destruct_count.fetch_add(1); // 增加销毁计数（用于调试）
		}
	}

	// 增加外部引用计数
	static void increase_external_count(
		std::atomic<counted_node_ptr>& counter,
		counted_node_ptr& old_counter)
	{
		counted_node_ptr new_counter;
		do
		{
			new_counter = old_counter;
			++new_counter.external_count; // 增加外部计数
		} while (!counter.compare_exchange_strong(
			old_counter, new_counter,
			std::memory_order_acquire, std::memory_order_relaxed));
		old_counter.external_count = new_counter.external_count;
	}

public:
	// 构造函数，初始化队列
	MyQueue()
	{
		counted_node_ptr new_next;
		new_next.ptr = new node(); // 创建一个空的头节点
		new_next.external_count = 1;
		tail.store(new_next); // 初始化尾节点
		head.store(new_next); // 初始化头节点
	}

	// 析构函数，清空队列并删除头节点
	~MyQueue()
	{
		while (pop()); // 清空队列
		auto head_counted_node = head.load();
		delete head_counted_node.ptr; // 删除头节点
	}

	// 入队操作
	void push(T new_value)
	{
		std::unique_ptr<T> new_data(new T(new_value)); // 创建新数据
		counted_node_ptr new_next;
		new_next.ptr = new node; // 创建新节点
		new_next.external_count = 1;
		counted_node_ptr old_tail = tail.load(); // 获取当前尾节点
		for (;;)
		{
			increase_external_count(tail, old_tail); // 增加尾节点的外部计数
			T* old_data = nullptr;
			// 尝试将新数据存入尾节点
			if (old_tail.ptr->data.compare_exchange_strong(
				old_data, new_data.get()))
			{
				counted_node_ptr old_next;
				counted_node_ptr now_next = old_tail.ptr->next.load();
				// 链接新的节点
				if (!old_tail.ptr->next.compare_exchange_strong(
					old_next, new_next))
				{
					delete new_next.ptr; // 如果链接失败，删除新节点
					new_next = old_next; // 使用旧的下一个节点
				}
				set_new_tail(old_tail, new_next); // 更新尾节点
				new_data.release();
				element_count.fetch_add(1, std::memory_order_relaxed);  // 增加计数
				break;
			}
			else
			{
				counted_node_ptr old_next;
				// 如果尾节点的数据已被占用，尝试链接新节点
				if (old_tail.ptr->next.compare_exchange_strong(
					old_next, new_next))
				{
					old_next = new_next; // 更新旧的下一个节点
					new_next.ptr = new node; // 创建新的下一个节点
				}
				set_new_tail(old_tail, old_next); // 更新尾节点
			}
		}
		construct_count++; // 增加构造计数（用于调试）
	}

	std::unique_ptr<T>front() {
		counted_node_ptr old_head = head.load(std::memory_order_relaxed);
		for (;;) {
			increase_external_count(head, old_head); // 增加头节点的外部引用计数
			node* const ptr = old_head.ptr;
			if (ptr == tail.load().ptr) {
				// 如果头尾相同，说明队列为空
				ptr->release_ref(); // 释放头节点的引用
				return std::unique_ptr<T>(); // 返回空指针
			}

			// 获取下一个节点
			counted_node_ptr next = ptr->next.load();
			if (head.compare_exchange_strong(old_head, next)) {
				// 如果成功更新头节点，返回当前节点的数据
				T* const res = ptr->data.load(std::memory_order_acquire);
				free_external_counter(old_head);
				return std::unique_ptr<T>(res);
			}

			// 如果更新头节点失败，释放头节点的引用
			ptr->release_ref();
		}
	}

	// 出队操作
	std::unique_ptr<T> pop()
	{
		counted_node_ptr old_head = head.load(std::memory_order_relaxed);
		for (;;)
		{
			increase_external_count(head, old_head); // 增加头节点的外部计数
			node* const ptr = old_head.ptr;
			if (ptr == tail.load().ptr)
			{
				// 如果头尾相同，说明队列为空
				ptr->release_ref(); // 释放头节点的引用
				return std::unique_ptr<T>();
			}

			counted_node_ptr next = ptr->next.load(); // 获取下一个节点
			if (head.compare_exchange_strong(old_head, next)) {
				T* const res = ptr->data.exchange(nullptr);
				free_external_counter(old_head);
				if (res) {  // 只有当实际获取到数据时才减少计数
					element_count.fetch_sub(1, std::memory_order_relaxed);
				}
				return std::unique_ptr<T>(res);
			}

			ptr->release_ref(); // 如果更新头节点失败，释放头节点的引用
		}
	}

	// 在MyQueue中添加线程安全的try_pop方法
	bool try_pop(T& value) {
		auto res = pop();
		if (res) {
			value = *res;
			return true;
		}
		return false;
	}

	int size() const noexcept {
		return element_count.load(std::memory_order_relaxed);
	}

	// 判断队列是否为空
	bool empty() const noexcept {
		return size() == 0;
	}

	// 用于调试的静态计数器，记录构造和销毁的节点数目
	static std::atomic<int> destruct_count;
	static std::atomic<int> construct_count;
	std::atomic<int> element_count{ 0 };  // 新增元素计数器
};

// 初始化静态计数器
template<typename T>
std::atomic<int> MyQueue<T>::destruct_count = 0;

template<typename T>
std::atomic<int> MyQueue<T>::construct_count = 0;