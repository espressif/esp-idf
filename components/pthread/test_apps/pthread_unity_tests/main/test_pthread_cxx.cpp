/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <iostream>
#include <sstream>
#include <thread>
#include <mutex>
#include <memory>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "test_utils.h"

#if __GTHREADS && __GTHREADS_CXX0X

#include "esp_log.h"
const static __attribute__((unused)) char *TAG = "pthread_test";

static std::mutex           mtx;
static std::shared_ptr<int> global_sp_mtx; // protected by mux

static std::recursive_mutex recur_mtx;
static std::shared_ptr<int> global_sp_recur_mtx; // protected by recursive mux

static void thread_do_nothing() {}

static void thread_main()
{
    std::cout << "thread_main CXX " << std::hex <<  std::this_thread::get_id() << std::endl;
    std::chrono::milliseconds dur = std::chrono::milliseconds(10);

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            int old_val, new_val;

            // mux test
            mtx.lock();
            old_val = *global_sp_mtx;
            std::this_thread::yield();
            (*global_sp_mtx)++;
            std::this_thread::yield();
            new_val = *global_sp_mtx;
            mtx.unlock();
            std::cout << "thread " << std::hex << std::this_thread::get_id() << ": nrec " << i << " val= " << *global_sp_mtx << std::endl;
            TEST_ASSERT_EQUAL(old_val + 1, new_val);

            // sleep_for test
            std::this_thread::sleep_for(dur);

            // recursive mux test
            recur_mtx.lock();
            recur_mtx.lock();
            old_val = *global_sp_recur_mtx;
            std::this_thread::yield();
            (*global_sp_recur_mtx)++;
            std::this_thread::yield();
            new_val = *global_sp_recur_mtx;
            recur_mtx.unlock();
            recur_mtx.unlock();
            std::cout << "thread " << std::hex << std::this_thread::get_id() << ": rec " << i << " val= " << *global_sp_recur_mtx << std::endl;
            TEST_ASSERT_EQUAL(old_val + 1, new_val);
        }

        // sleep_until test
        using std::chrono::system_clock;
        std::time_t tt = system_clock::to_time_t(system_clock::now());
        struct std::tm *ptm = std::localtime(&tt);
        ptm->tm_sec++;
        std::this_thread::sleep_until(system_clock::from_time_t(mktime(ptm)));
    }
}

// IDF-6423 - assert and crash when running this testcase on QEMU
TEST_CASE("pthread C++", "[pthread][qemu-ignore]")
{
    global_sp_mtx.reset(new int(1));
    global_sp_recur_mtx.reset(new int(-1000));

    std::thread t1(thread_do_nothing);
    t1.join();

    std::thread t2(thread_main);
    std::cout << "Detach thread " << std::hex << t2.get_id() << std::endl;
    t2.detach();
    TEST_ASSERT_FALSE(t2.joinable());

    std::thread t3(thread_main);
    std::thread t4(thread_main);
    TEST_ASSERT(t3.joinable());
    TEST_ASSERT(t4.joinable());
    std::cout << "Join thread " << std::hex << t3.get_id() << std::endl;
    t3.join();
    std::cout << "Join thread " << std::hex << t4.get_id() << std::endl;
    t4.join();

    // we don't know if/when t2 has finished, so delay another 2s before
    // deleting the common mutexes
    std::this_thread::sleep_for(std::chrono::seconds(2));

    global_sp_mtx.reset(); // avoid reported leak
    global_sp_recur_mtx.reset();
}

static void task_test_sandbox()
{
    std::stringstream ss;

    ESP_LOGI(TAG, "About to create a string stream");
    ESP_LOGI(TAG, "About to write to string stream");
    ss << "Hello World!";
    ESP_LOGI(TAG, "About to extract from stringstream");
    ESP_LOGI(TAG, "Text: %s", ss.str().c_str());
}

static void task_test_sandbox_c(void *arg)
{
    bool *running = (bool *)arg;

    // wrap thread func to ensure that all C++ stack objects are cleaned up by their destructors
    task_test_sandbox();

    ESP_LOGI(TAG, "Task stk_wm = %d", uxTaskGetStackHighWaterMark(NULL));
    if (running) {
        *running = false;
        vTaskDelete(NULL);
    }
}

TEST_CASE("pthread mix C/C++", "[pthread]")
{
    bool c_running = true;

    std::thread t1(task_test_sandbox);
    xTaskCreatePinnedToCore((TaskFunction_t)&task_test_sandbox_c, "task_test_sandbox", 3072, &c_running, 5, NULL, 0);
    while (c_running) {
        vTaskDelay(1);
    }
    if (t1.joinable()) {
        std::cout << "Join thread " << std::hex << t1.get_id() << std::endl;
        t1.join();
    }

    /* Short delay to allow cleanup, avoid leaks */
    vTaskDelay(10);
}
#endif
