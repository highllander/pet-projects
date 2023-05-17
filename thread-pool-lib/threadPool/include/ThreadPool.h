#pragma once

#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

namespace ThreadPoolNS {

class IThreadPool {

public:
	using task_t = std::function<void()>;

	virtual ~IThreadPool() = default;

	// API to post simple tasks in the pool
	virtual bool Post(const task_t& task) = 0;

	// API to post tasks with arguments in the pool
	template<typename Fn, typename ...Args>
	auto PostTask(Fn&& fn, Args&&... args)->std::future<typename std::result_of<Fn(Args...)>::type>;

	// API to force stop the pool
	virtual void Shutdown() = 0;
};

template<typename Fn, typename ...Args>
auto IThreadPool::PostTask(Fn&& fn, Args&&... args) -> std::future<typename std::result_of<Fn(Args...)>::type> {
	using FnReturn_t = decltype(fn(args...));

	auto task = std::make_shared<std::packaged_task<FnReturn_t()>>(std::bind(std::forward<Fn>(fn), std::forward<Args>(args)...));

	std::future<FnReturn_t> retFuture = task->get_future();

	auto postLambda = [task = std::move(task)](){
		(*task)();
	};
	Post(postLambda);
	return retFuture;
}


class ThreadPool : public IThreadPool {
	std::queue<task_t> m_taskQueue;
	std::vector<std::thread> m_workers;
	std::mutex m_queueMutex;
	std::condition_variable m_cond;
	bool m_stop = false;

public:
	ThreadPool(std::size_t size_t);

	void Shutdown() override;
	bool Post(const task_t& task) override;

	~ThreadPool();
};

// Factory
std::shared_ptr<IThreadPool> CreateThreadPool(size_t poolSize = std::thread::hardware_concurrency());

}