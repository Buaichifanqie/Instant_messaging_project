#pragma once
#include<atomic>
#include<memory>
#include <cassert>

// ��������ģ����
template<typename T>
class MyQueue
{
private:

	// �ڵ����ü����ṹ�壬ʹ��λ��洢�ڲ��������ⲿ��������Ŀ
	struct node_counter
	{
		unsigned internal_count : 30; // �ڲ�������ռ��30λ
		unsigned external_counters : 2; // �ⲿ��������Ŀ��ռ��2λ
	};

	// ���нڵ�ṹ��
	struct node;

	// ����ڵ�ָ����ⲿ���ü����Ľṹ��
	struct counted_node_ptr
	{
		counted_node_ptr() :external_count(0), ptr(nullptr) {} // Ĭ�Ϲ��캯��
		int external_count; // �ⲿ���ü���
		node* ptr; // ָ��ڵ��ָ��
	};

	// ���нڵ�ṹ��
	struct node
	{
		std::atomic<T*> data; // ԭ���Եش洢����ָ��
		std::atomic<node_counter> count; // ԭ���Եش洢���ü�����Ϣ
		std::atomic<counted_node_ptr> next; // ԭ���Եش洢��һ���ڵ����Ϣ

		// ���캯������ʼ���ڵ�
		node(int external_count = 2)
		{
			node_counter new_count;
			new_count.internal_count = 0; // ��ʼ���ڲ�����Ϊ0
			new_count.external_counters = external_count; // �����ⲿ��������Ŀ
			count.store(new_count); // �洢���ü�����Ϣ

			counted_node_ptr node_ptr;
			node_ptr.ptr = nullptr; // ��ʼ����һ���ڵ�ָ��Ϊ��
			node_ptr.external_count = 0;
			next.store(node_ptr);
		}

		// �ͷŽڵ����ã������ڲ�������������Ϊ0ʱɾ���ڵ�
		void release_ref()
		{
			node_counter old_counter = count.load(std::memory_order_relaxed);
			node_counter new_counter;
			do
			{
				new_counter = old_counter;
				--new_counter.internal_count; // �����ڲ�����
			} while (!count.compare_exchange_strong(
				old_counter, new_counter,
				std::memory_order_acquire, std::memory_order_relaxed));

			// ����ڲ����ⲿ������Ϊ0��ɾ���ڵ�
			if (!new_counter.internal_count && !new_counter.external_counters)
			{
				delete this;
				destruct_count.fetch_add(1); // �������ټ��������ڵ��ԣ�
			}
		}
	};

	// ԭ���Եش洢ͷ�ڵ���Ϣ
	std::atomic<counted_node_ptr> head;
	// ԭ���Եش洢β�ڵ���Ϣ
	std::atomic<counted_node_ptr> tail;

	// ������β�ڵ㣬����������
	void set_new_tail(counted_node_ptr& old_tail, counted_node_ptr const& new_tail)
	{
		node* const current_tail_ptr = old_tail.ptr; // �����β�ڵ�ָ��
		// CASѭ��ֱ���ɹ���tail�������̸߳���
		while (!tail.compare_exchange_weak(old_tail, new_tail) &&
			old_tail.ptr == current_tail_ptr);
		// ���β�ڵ�δ�������߳��޸ģ��ͷž�β�ڵ�
		if (old_tail.ptr == current_tail_ptr)
			free_external_counter(old_tail);
		else
			// ���β�ڵ��ѱ������߳��޸ģ����پ�β�ڵ�����ü���
			current_tail_ptr->release_ref();
	}

	// �ͷ��ⲿ������������ɾ���ڵ�
	static void free_external_counter(counted_node_ptr& old_node_ptr)
	{
		node* const ptr = old_node_ptr.ptr; // ��ȡ�ڵ�ָ��
		int const count_increase = old_node_ptr.external_count - 2; // �������ò�
		node_counter old_counter = ptr->count.load(std::memory_order_relaxed); // ��ȡ��ǰ���ü�����Ϣ
		node_counter new_counter;
		do
		{
			new_counter = old_counter;
			--new_counter.external_counters; // �����ⲿ������
			new_counter.internal_count += count_increase; // �����ڲ�����
		} while (!ptr->count.compare_exchange_strong(
			old_counter, new_counter,
			std::memory_order_acquire, std::memory_order_relaxed));

		// ����ڲ��������ⲿ������Ϊ0��ɾ���ڵ�
		if (!new_counter.internal_count && !new_counter.external_counters)
		{
			delete ptr;
			destruct_count.fetch_add(1); // �������ټ��������ڵ��ԣ�
		}
	}

	// �����ⲿ���ü���
	static void increase_external_count(
		std::atomic<counted_node_ptr>& counter,
		counted_node_ptr& old_counter)
	{
		counted_node_ptr new_counter;
		do
		{
			new_counter = old_counter;
			++new_counter.external_count; // �����ⲿ����
		} while (!counter.compare_exchange_strong(
			old_counter, new_counter,
			std::memory_order_acquire, std::memory_order_relaxed));
		old_counter.external_count = new_counter.external_count;
	}

public:
	// ���캯������ʼ������
	MyQueue()
	{
		counted_node_ptr new_next;
		new_next.ptr = new node(); // ����һ���յ�ͷ�ڵ�
		new_next.external_count = 1;
		tail.store(new_next); // ��ʼ��β�ڵ�
		head.store(new_next); // ��ʼ��ͷ�ڵ�
	}

	// ������������ն��в�ɾ��ͷ�ڵ�
	~MyQueue()
	{
		while (pop()); // ��ն���
		auto head_counted_node = head.load();
		delete head_counted_node.ptr; // ɾ��ͷ�ڵ�
	}

	// ��Ӳ���
	void push(T new_value)
	{
		std::unique_ptr<T> new_data(new T(new_value)); // ����������
		counted_node_ptr new_next;
		new_next.ptr = new node; // �����½ڵ�
		new_next.external_count = 1;
		counted_node_ptr old_tail = tail.load(); // ��ȡ��ǰβ�ڵ�
		for (;;)
		{
			increase_external_count(tail, old_tail); // ����β�ڵ���ⲿ����
			T* old_data = nullptr;
			// ���Խ������ݴ���β�ڵ�
			if (old_tail.ptr->data.compare_exchange_strong(
				old_data, new_data.get()))
			{
				counted_node_ptr old_next;
				counted_node_ptr now_next = old_tail.ptr->next.load();
				// �����µĽڵ�
				if (!old_tail.ptr->next.compare_exchange_strong(
					old_next, new_next))
				{
					delete new_next.ptr; // �������ʧ�ܣ�ɾ���½ڵ�
					new_next = old_next; // ʹ�þɵ���һ���ڵ�
				}
				set_new_tail(old_tail, new_next); // ����β�ڵ�
				new_data.release();
				element_count.fetch_add(1, std::memory_order_relaxed);  // ���Ӽ���
				break;
			}
			else
			{
				counted_node_ptr old_next;
				// ���β�ڵ�������ѱ�ռ�ã����������½ڵ�
				if (old_tail.ptr->next.compare_exchange_strong(
					old_next, new_next))
				{
					old_next = new_next; // ���¾ɵ���һ���ڵ�
					new_next.ptr = new node; // �����µ���һ���ڵ�
				}
				set_new_tail(old_tail, old_next); // ����β�ڵ�
			}
		}
		construct_count++; // ���ӹ�����������ڵ��ԣ�
	}

	std::unique_ptr<T>front() {
		counted_node_ptr old_head = head.load(std::memory_order_relaxed);
		for (;;) {
			increase_external_count(head, old_head); // ����ͷ�ڵ���ⲿ���ü���
			node* const ptr = old_head.ptr;
			if (ptr == tail.load().ptr) {
				// ���ͷβ��ͬ��˵������Ϊ��
				ptr->release_ref(); // �ͷ�ͷ�ڵ������
				return std::unique_ptr<T>(); // ���ؿ�ָ��
			}

			// ��ȡ��һ���ڵ�
			counted_node_ptr next = ptr->next.load();
			if (head.compare_exchange_strong(old_head, next)) {
				// ����ɹ�����ͷ�ڵ㣬���ص�ǰ�ڵ������
				T* const res = ptr->data.load(std::memory_order_acquire);
				free_external_counter(old_head);
				return std::unique_ptr<T>(res);
			}

			// �������ͷ�ڵ�ʧ�ܣ��ͷ�ͷ�ڵ������
			ptr->release_ref();
		}
	}

	// ���Ӳ���
	std::unique_ptr<T> pop()
	{
		counted_node_ptr old_head = head.load(std::memory_order_relaxed);
		for (;;)
		{
			increase_external_count(head, old_head); // ����ͷ�ڵ���ⲿ����
			node* const ptr = old_head.ptr;
			if (ptr == tail.load().ptr)
			{
				// ���ͷβ��ͬ��˵������Ϊ��
				ptr->release_ref(); // �ͷ�ͷ�ڵ������
				return std::unique_ptr<T>();
			}

			counted_node_ptr next = ptr->next.load(); // ��ȡ��һ���ڵ�
			if (head.compare_exchange_strong(old_head, next)) {
				T* const res = ptr->data.exchange(nullptr);
				free_external_counter(old_head);
				if (res) {  // ֻ�е�ʵ�ʻ�ȡ������ʱ�ż��ټ���
					element_count.fetch_sub(1, std::memory_order_relaxed);
				}
				return std::unique_ptr<T>(res);
			}

			ptr->release_ref(); // �������ͷ�ڵ�ʧ�ܣ��ͷ�ͷ�ڵ������
		}
	}

	// ��MyQueue������̰߳�ȫ��try_pop����
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

	// �ж϶����Ƿ�Ϊ��
	bool empty() const noexcept {
		return size() == 0;
	}

	// ���ڵ��Եľ�̬����������¼��������ٵĽڵ���Ŀ
	static std::atomic<int> destruct_count;
	static std::atomic<int> construct_count;
	std::atomic<int> element_count{ 0 };  // ����Ԫ�ؼ�����
};

// ��ʼ����̬������
template<typename T>
std::atomic<int> MyQueue<T>::destruct_count = 0;

template<typename T>
std::atomic<int> MyQueue<T>::construct_count = 0;