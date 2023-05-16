#define BOOST_TEST_MAIN

#include "ThreadPool.h"

#include <chrono>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(ThreadPool)

BOOST_AUTO_TEST_CASE(RunThreadPool)
{
    ThreadPoolNS::ThreadPool pool(4);
    auto task_getInt = pool.PostTask([]() 
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            return 42;
        });
    
    BOOST_REQUIRE_EQUAL(task_getInt.get(), 42);
}

BOOST_AUTO_TEST_CASE(PoolingTasks)
{
    ThreadPoolNS::ThreadPool pool;
    auto dummy = [](int number) 
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200)); 
            std::cout << "I am {" << number << "} done!\n";
        };
       
    std::vector<std::future<void>> results;

    for (int i = 20; i >= 0; --i) {
        results.push_back(pool.PostTask(dummy, i));
    }
    for (auto& r : results)
        r.get();

    BOOST_CHECK_NO_THROW(pool.Shutdown());
}
 

BOOST_AUTO_TEST_SUITE_END()