#pragma once

#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

namespace ThreadPoolNS{

class ThreadPool{

	using task_t = std::function<void()>;

	std::queue<task_t> m_queue;
	std::vector<std::thread> m_workers;
	std::mutex m_queueMutex;
	std::condition_variable m_cond;
	bool m_stop = false;

public:

	ThreadPool(std::size_t poolSize = std::thread::hardware_concurrency());
	
	void Shutdown();

	template<typename Fn, typename ...Args>
	auto PostTask(Fn&& fn, Args&&... args)->std::future<typename std::result_of<Fn(Args...)>::type>;

	~ThreadPool();

};

template<typename Fn, typename ...Args>
auto ThreadPool::PostTask(Fn&& fn, Args&&... args) -> std::future<typename std::result_of<Fn(Args...)>::type> {
	using FnReturn_t = decltype(fn(args...));

	auto task = std::make_shared<std::packaged_task<FnReturn_t()>>(std::bind(std::forward<Fn>(fn), std::forward<Args>(args)...));

	std::future<FnReturn_t> retFuture = task->get_future();
	{
		std::unique_lock<std::mutex> lock(m_queueMutex);
		m_queue.push([task]()
			{
				(*task)();
			});
	}
	m_cond.notify_one();
	return retFuture;
}

}