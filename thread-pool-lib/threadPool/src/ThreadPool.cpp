#include "threadPool.h"

#include <iostream>

namespace ThreadPoolNS {


	ThreadPool::ThreadPool(std::size_t poolSize) {

		for (int i = 0; i < poolSize; i++) {
			m_workers.emplace_back(std::thread([this]()
				{
					ThreadPool::task_t task;
					while (true) {
						{
							std::unique_lock<std::mutex> lock(m_queueMutex);
							m_cond.wait(lock, [this]()
								{
									return m_stop || !m_taskQueue.empty();
								});

							// when stop commands came, stops worker if there is no any more task
							if (m_stop && m_taskQueue.empty())
								return;

							task = std::move(m_taskQueue.front());
							m_taskQueue.pop();

							task();
						}
					}
				}));
		}

	}

	void ThreadPool::Shutdown() {
		{
			std::unique_lock<std::mutex> lock(m_queueMutex);
			m_stop = true;
		}

		m_cond.notify_all();

		for (std::thread& w : m_workers) {
			if (w.joinable())
				w.join();
		}
	}

	bool ThreadPool::Post(const task_t& task)
	{
		bool isOk = false;
		{
			try {
				std::unique_lock<std::mutex> lock(m_queueMutex);
				m_taskQueue.emplace(task);
				isOk = true;
				m_cond.notify_one();
			}
			catch (const std::exception& ex) {
				std::cout << "Error: ex=" << ex.what();
			}
		}
		return true;
	}

	ThreadPool::~ThreadPool() {
		Shutdown();
	}

	std::shared_ptr<IThreadPool> CreateThreadPool(size_t poolSize)
	{
		return std::make_shared<ThreadPool>(poolSize);
	}

}