#ifndef WEBSERVER_THREADPOOL_H
#define WEBSERVER_THREADPOOL_H

#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include <functional>
#include <cassert>

class ThreadPool {
private:
	struct Pool {
		std::mutex mtx;
		std::condition_variable cond;
		bool is_closed;
		std::queue<std::function<void()>> tasks;
	};
	std::shared_ptr<Pool> pool_ptr;
public:
	explicit ThreadPool(size_t thread_count = 0)
			: pool_ptr(std::make_shared<Pool>()) {
		assert(thread_count > 0);
		for (size_t i = 0; i < thread_count; ++i) {
			std::thread([pool = pool_ptr] {
				std::unique_lock<std::mutex> locker(pool->mtx);
				while (true) {
					if (!pool->tasks.empty()) {
						auto task = std::move(pool->tasks.front());
						pool->tasks.pop();
						locker.unlock();
						task();
						locker.lock();
					} else if (pool->is_closed) {
						break;
					} else {
						pool->cond.wait(locker);
					}
				}
			}).detach();
		}
	}

	ThreadPool() = default;
	ThreadPool(ThreadPool &&) = default;

	~ThreadPool() {
		if (static_cast<bool>(pool_ptr)) {
			{
				// 这个大括号是一个作用域不可省略,
				// 原因是 lock_guard 在作用域结束后会自动调用析构函数解锁
				std::lock_guard<std::mutex> locker(pool_ptr->mtx);
				pool_ptr->is_closed = true;
			}
			pool_ptr->cond.notify_all();
		}
	}

	template<class FUNCTION>
	void add_task(FUNCTION &&task) {
		{
			std::lock_guard<std::mutex> locker(pool_ptr->mtx);
			pool_ptr->tasks.emplace(std::forward<FUNCTION>(task));
		}
		pool_ptr->cond.notify_one();
	}
};

#endif //WEBSERVER_THREADPOOL_H
