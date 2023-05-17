#define BOOST_TEST_MAIN

#include "ThreadPool.h"

#include <chrono>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(ThreadPool)

BOOST_AUTO_TEST_CASE(PostDummyLambdas)
{
    auto pool = ThreadPoolNS::CreateThreadPool(4);
    const size_t TASK_NUMBER = 10;
    auto dummyTask = []() {};

    for(int i = 0; i < TASK_NUMBER; ++i)
        BOOST_REQUIRE(pool->Post(dummyTask));
     
    pool->Shutdown();
}

int getSum(int a, int b)
{
    return a + b;
}
BOOST_AUTO_TEST_CASE(PostTasks)
{
    auto pool = ThreadPoolNS::CreateThreadPool();

    auto value = pool->PostTask(getSum, 15, 10);
    BOOST_REQUIRE_EQUAL(value.get(), 25);

    auto value2 = pool->PostTask(getSum, 10, 10);
    BOOST_REQUIRE_EQUAL(value2.get(), 20);

    pool->Shutdown();
}

BOOST_AUTO_TEST_CASE(PoolAndWaitResults)
{
    auto pool = ThreadPoolNS::CreateThreadPool();
    auto dummy = [](int number)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            auto log = "task N" + std::to_string(number) + " is done";
            BOOST_TEST_MESSAGE(log);
        };
       
    std::vector<std::future<void>> results;

    const size_t TASK_NUMBER = 10;
    for (int i = 0; i < TASK_NUMBER; ++i) {
        results.push_back(pool->PostTask(dummy, i));
    }

    for (auto& r : results)
        r.get();

    BOOST_CHECK_NO_THROW(pool->Shutdown());
}


BOOST_AUTO_TEST_CASE(ShutdownWhileActive)
{
    auto pool = ThreadPoolNS::CreateThreadPool();
    const size_t TASK_NUMBER = 100;

    for (int i = 0; i < TASK_NUMBER; ++i) {
        pool->PostTask([]() {});
    }

    BOOST_CHECK_NO_THROW(pool->Shutdown());
}

BOOST_AUTO_TEST_SUITE_END()