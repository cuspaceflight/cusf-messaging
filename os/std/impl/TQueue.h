#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

template <class T> 
class TQueue {
public:
    TQueue() : running(true) {

    }

    void enqueue(T val) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!running)
            return;
        queue_.push(val);
        condition_.notify_one();
    }

    bool dequeue(T& ret) {
        std::unique_lock<std::mutex> lock(mutex_);
        while (queue_.empty()) {
            if (!running)
                return false;
            condition_.wait(lock);
        }
        ret = queue_.front();
        queue_.pop();
        return true;
    }

    bool isEmpty() {
        std::unique_lock<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    void close() {
        std::unique_lock<std::mutex> lock(mutex_);
        running = false;
        condition_.notify_all();
    }

    void open() {
        std::unique_lock<std::mutex> lock(mutex_);
        running = true;
    }

private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    bool running;
};