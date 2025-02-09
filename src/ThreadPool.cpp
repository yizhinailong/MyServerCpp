#include "ThreadPool.hpp"

ThreadPool::ThreadPool(int size)
    : _stop(false) {
    for (int i = 0; i < size; i++) {
        _threads.emplace_back(std::thread([this]() {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(_tasks_mtx);
                    _cv.wait(lock, [this]() {
                        return _stop || !_tasks.empty();
                    });
                    if (_stop || _tasks.empty()) {
                        return;
                    }
                    task = _tasks.front();
                    _tasks.pop();
                }
                task();
            }
        }));
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(_tasks_mtx);
        _stop = true;
    }
    _cv.notify_all();
    for (std::thread& thread : _threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}
