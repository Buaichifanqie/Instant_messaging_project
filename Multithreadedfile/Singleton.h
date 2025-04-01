#pragma once
#include <memory>
#include <mutex>
#include <iostream>
using namespace std;
template<typename T>
class Singleton
{
public:
	static std::shared_ptr<T> GetInstance()
	{
		static std::once_flag s_flag;
		std::call_once(s_flag, [&]() {
			m_instance = std::shared_ptr<T>(new T);
		});
		return m_instance;
	}
	~Singleton() {
		std::cout << "this is singleton destruct" << std::endl;
	}
	void PrintAddress() {
		std::cout << m_instance.get() << endl;
	}
protected://可以让子类也使用
	Singleton() = default;
	Singleton(const Singleton<T>&) = delete;
	Singleton& operator=(const Singleton<T>&) = delete;
	static std::shared_ptr<T> m_instance;
};

//静态变量要在类外初始化
template<typename T>
std::shared_ptr<T> Singleton<T>::m_instance = nullptr;
