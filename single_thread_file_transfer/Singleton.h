#ifndef SINGLETON_H
#define SINGLETON_H
#include <memory>
#include <mutex>

template<typename T>
class Singleton
{
public:
    static std::shared_ptr<T> Inst(){
        static std::once_flag flag;
        std::call_once(flag,[](){
            _inst = std::shared_ptr<T>(new T);
        });

        return _inst;
    }
    ~Singleton(){}
protected:
    Singleton()=default;
    //拷贝构造删除
    Singleton(const Singleton& st) = delete ;
    //拷贝赋值删除
    Singleton& operator=(const Singleton& st) = delete;
private:
    static std::shared_ptr<T> _inst;
};

template <typename T>
std::shared_ptr<T> Singleton<T>::_inst = nullptr;

#endif // SINGLETON_H
