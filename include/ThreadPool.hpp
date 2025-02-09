#pragma once
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool
{
public:
    ThreadPool(int size = 10);
    ~ThreadPool();

    template <typename F, typename... Args>
    auto add(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;

private:
    std::vector<std::thread> _threads;
    std::queue<std::function<void()>> _tasks;
    std::mutex _tasks_mtx;
    std::condition_variable _cv;
    bool _stop;
};

template <typename F, typename... Args>
inline auto ThreadPool::add(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
    using result_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<result_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<result_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(_tasks_mtx);
        if (_stop) {
            throw std::runtime_error("enqueue on stoped ThreadPool");
        }
        _tasks.emplace([task]() { (*task)(); });
    }
    _cv.notify_one();

    return res;
}
