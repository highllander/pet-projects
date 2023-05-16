#include "threadPool.h"

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
									return m_stop || !m_queue.empty();
								});

							// when stop commands came, stops worker if there is no any more task
							if (m_stop && m_queue.empty())
								return;

							task = std::move(m_queue.front());
							m_queue.pop();

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


	ThreadPool::~ThreadPool() {
		Shutdown();
	}

}