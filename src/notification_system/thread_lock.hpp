#pragma once
#include <mutex>

namespace notification_system {
    template <typename T>
    class Lock{
    public:
        Lock(std::mutex& mutex, T value) : value_mutex_(mutex), value_(value){
            value_mutex_.lock();
        }
        ~Lock() {
            value_mutex_.unlock();
        }
        T operator*(){
            return value_;
        }
        T operator->(){
            return value_;
        }
    private:
        std::mutex& value_mutex_;
        T value_;
    };
}