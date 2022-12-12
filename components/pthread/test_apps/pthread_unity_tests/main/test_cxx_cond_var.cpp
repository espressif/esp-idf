/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <iostream>
#include <thread>
#include <condition_variable>
#include <chrono>
#include <mutex>
#include <atomic>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "unity.h"

#if __GTHREADS && __GTHREADS_CXX0X

static std::condition_variable cv;
static std::mutex cv_m;
static std::atomic<int> i{0};

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


TEST_CASE("cxx: condition_variable can timeout", "[cxx]")
{
    std::condition_variable cv;
    std::mutex mtx;
    std::unique_lock<std::mutex> lck(mtx);
    srand(99);
    for (int i = 0; i < 10; ++i) {
        usleep(rand() % 1000);
        auto status = cv.wait_for(lck, std::chrono::milliseconds(200));
        TEST_ASSERT_EQUAL(std::cv_status::timeout, status);
    }
}

TEST_CASE("cxx: condition_variable timeout never before deadline", "[cxx]")
{
    using SysClock = std::chrono::system_clock;

    std::mutex mutex;
    std::condition_variable cond;
    std::unique_lock<std::mutex> lock(mutex);

    for (int i = 0; i < 25; ++i) {
        auto timeout = std::chrono::milliseconds(portTICK_PERIOD_MS * (i+1));
        auto deadline = SysClock::now() + timeout;

        auto secs = std::chrono::time_point_cast<std::chrono::seconds>(deadline);
        auto nsecs = std::chrono::duration_cast<std::chrono::nanoseconds>
                (deadline - secs);
        struct timespec ts = {
                .tv_sec = static_cast<time_t>(secs.time_since_epoch().count()),
                .tv_nsec = static_cast<long>(nsecs.count())};
        int rc = ::pthread_cond_timedwait(cond.native_handle(),
                                          lock.mutex()->native_handle(), &ts);
        auto status = (rc == ETIMEDOUT) ? std::cv_status::timeout :
                                          std::cv_status::no_timeout;
        auto end = SysClock::now();
        auto extra = end - deadline;
        auto extra_us = extra / std::chrono::microseconds(1);
        printf("timeout %lldms Extra time: %lldus, status: %s\n", timeout.count(), extra_us,
               (status == std::cv_status::timeout) ? "timeout" : "no timeout");

        // The timed wait should always return at least 1us after the timeout deadline
        TEST_ASSERT_GREATER_THAN(0, extra_us);
    }
}

#endif // __GTHREADS && __GTHREADS_CXX0X
