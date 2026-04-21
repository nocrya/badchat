#ifndef SINGLETON_H
#define SINGLETON_H
#include <global.h>

template <typename T>
class Singleton{
protected:
    Singleton() = default;
    Singleton(const Singleton<T>&) = delete;
    Singleton& operator= (const Singleton<T>& st) = delete;
    static std::shared_ptr<T> _instance;
public:
    static std::shared_ptr<T> GetInstance(){
        static std::once_flag s_flag;
        std::call_once(s_flag, [&](){
            _instance = std::shared_ptr<T>(new T);
        });
        return _instance;
    }

    // static std::shared_ptr<T> GetInstance() {
    //     // 局部静态变量，无须全局 _instance 成员
    //     static std::shared_ptr<T> _instance = std::make_shared<T>();
    //     // C++11 起，局部静态变量初始化是线程安全的
    //     return _instance;
    // }

    void PrintAddress(){
        std::cout <<  _instance.get() << std::endl;
    }

    ~Singleton() = default;

};

template <typename T>
std::shared_ptr<T> Singleton<T>::_instance = nullptr;


#endif // SINGLETON_H
