#pragma once
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
public:
    ThreadPool(int size = 10);
    ~ThreadPool();

    void add(std::function<void()> thread);

private:
    std::vector<std::thread> _threads;
    std::queue<std::function<void()>> _tasks;
    std::mutex _tasks_mtx;
    std::condition_variable _cv;
    bool _stop;
};
