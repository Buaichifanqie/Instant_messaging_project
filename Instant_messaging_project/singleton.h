#ifndef SINGLETON_H
#define SINGLETON_H
#include "global.h"

template <typename T>
class Singleton
{
    //为了基类可以构造
protected:
    Singleton()=default;
    Singleton(const Singleton<T>&)=delete;
    Singleton& operator = (const Singleton<T>&)=delete;
    static std::shared_ptr<T> m_instance;
public:
    static std::shared_ptr<T> GetInstance() {
        // 定义一个静态的 std::once_flag 变量 s_flag
        // std::once_flag 用于确保某个操作只执行一次
        static std::once_flag s_flag;

        // 使用 std::call_once 调用 s_flag，确保 lambda 表达式中的代码只执行一次
        std::call_once(s_flag, [&]() {
            // 在 lambda 表达式中，创建 T 类型的实例，并用 std::shared_ptr 管理
            m_instance = std::shared_ptr<T>(new T);
        });
        return m_instance;
    }

    void PrintAddres(){
        std::cout<<m_instance.get()<<std::endl;
    }

    ~Singleton(){
        std::cout<<"this is singleton destruct..."<<std::endl;
    }
};

template <typename T>
std::shared_ptr<T> Singleton<T>::m_instance=nullptr;

#endif // SINGLETON_H
