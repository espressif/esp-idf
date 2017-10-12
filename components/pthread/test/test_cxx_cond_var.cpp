#include <iostream>
#include <thread>
#include <condition_variable>
#include <chrono>
#include <mutex>
#include <atomic>
#include "unity.h"

#if __GTHREADS && __GTHREADS_CXX0X

std::condition_variable cv;
std::mutex cv_m;
std::atomic<int> i{0};

static void waits(int idx, int timeout_ms)
{
    std::unique_lock<std::mutex> lk(cv_m);
    auto now = std::chrono::system_clock::now();

    if(cv.wait_until(lk, now + std::chrono::milliseconds(timeout_ms), [](){return i == 1;}))
        std::cout << "Thread " << idx << " finished waiting. i == " << i << '\n';
    else
        std::cout << "Thread " << idx << " timed out. i == " << i << '\n';
}

static void signals(int signal_ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(signal_ms));
    std::cout << "Notifying...\n";
    cv.notify_all();
    std::this_thread::sleep_for(std::chrono::milliseconds(signal_ms));
    i = 1;
    std::cout << "Notifying again...\n";
    cv.notify_all();
}

TEST_CASE("C++ condition_variable", "[std::condition_variable]")
{
    i = 0;
    std::thread t1(waits, 1, 100), t2(waits, 2, 800), t3(signals, 200);

    t1.join();
    t2.join();
    t3.join();

    std::cout << "All threads joined\n";
}
#endif
