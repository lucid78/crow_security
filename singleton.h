#ifndef SINGLETON_H
#define SINGLETON_H

#pragma once

#include <thread>
#include <mutex>

template <class T>

class Singleton {
protected:
    Singleton() {}
    ~Singleton() {}

private:
    static std::once_flag chkSingle;
    static std::shared_ptr<T> pInstance;

    static void destroy() {
        if (pInstance != nullptr) {
            pInstance.reset();
            pInstance = nullptr;
        }
    }

public:
    static T& get() {
        std::call_once(chkSingle, []()
        {
            pInstance.reset(new T());
            std::atexit(destroy);
        });
        return *(pInstance.get());
    }
};

template <class T> std::shared_ptr<T> Singleton <T>::pInstance = nullptr;
template <class T> std::once_flag Singleton <T>::chkSingle;


#endif // SINGLETON_H
