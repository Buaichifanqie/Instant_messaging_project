#ifndef SINGLETON_H
#define SINGLETON_H
#include <mutex>
#include <memory>


template <typename T>
class Singleton
{
public:
    static std::shared_ptr<T> Inst()
    {
        static std::once_flag flag;
        std::call_once(flag,[](){
            m_inst=std::shared_ptr<T> (new T);
        });
        return m_inst;
    }

    ~Singleton(){}
protected:
    Singleton()=default;
    //拷贝构造删除
    Singleton(const Singleton& st)=delete;
    //拷贝赋值删除
    Singleton& operator=(const Singleton& st)=delete;

private:
    static std::shared_ptr<T> m_inst;
};

template <typename T>
std::shared_ptr<T> Singleton<T>::m_inst=nullptr;

#endif // SINGLETON_H
