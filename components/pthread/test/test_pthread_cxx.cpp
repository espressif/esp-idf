#include <iostream>
#include <sstream>
#include <thread>
#include <mutex>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"

#if __GTHREADS && __GTHREADS_CXX0X

#define LOG_LOCAL_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include "esp_log.h"
const static char *TAG = "pthread_test";

static std::shared_ptr<int> global_sp;
static std::mutex           mtx;
static std::recursive_mutex recur_mtx;

static void thread_do_nothing() {}

static void thread_main()
{
    int i = 0;
    std::cout << "thread_main CXX " << std::hex <<  std::this_thread::get_id() << std::endl;

    while (i < 3) {
        int old_val, new_val;

        // mux test
        mtx.lock();
        old_val = *global_sp;
        std::this_thread::yield();
        (*global_sp)++;
        std::this_thread::yield();
        new_val = *global_sp;
        mtx.unlock();
        std::cout << "thread " << std::hex << std::this_thread::get_id() << ": " << i++ << " val= " << *global_sp << std::endl;
        TEST_ASSERT_TRUE(new_val == old_val + 1);

        // sleep_for test
        std::chrono::milliseconds dur(300);
        std::this_thread::sleep_for(dur);

        // recursive mux test
        recur_mtx.lock();
        recur_mtx.lock();
        old_val = *global_sp;
        std::this_thread::yield();
        (*global_sp)++;
        std::this_thread::yield();
        new_val = *global_sp;
        recur_mtx.unlock();
        recur_mtx.unlock();
        std::cout << "thread " << std::hex << std::this_thread::get_id() << ": " << i++ << " val= " << *global_sp << std::endl;
        TEST_ASSERT_TRUE(new_val == old_val + 1);

        // sleep_until test
        using std::chrono::system_clock;
        std::time_t tt = system_clock::to_time_t(system_clock::now());
        struct std::tm *ptm = std::localtime(&tt);
        ptm->tm_sec++;
        std::this_thread::sleep_until(system_clock::from_time_t (mktime(ptm)));
    }
}

TEST_CASE("pthread C++", "[pthread]")
{
    global_sp.reset(new int(1));

    std::thread t1(thread_do_nothing);
    t1.join();

    std::thread t2(thread_main);
    std::cout << "Detach thread " << std::hex << t2.get_id() << std::endl;
    t2.detach();
    TEST_ASSERT_FALSE(t2.joinable());

    std::thread t3(thread_main);
    std::thread t4(thread_main);
    if (t3.joinable()) {
        std::cout << "Join thread " << std::hex << t3.get_id() << std::endl;
        t3.join();
    }
    if (t4.joinable()) {
        std::cout << "Join thread " << std::hex << t4.get_id() << std::endl;
        t4.join();
    }
}

static void task_test_sandbox(void *arg)
{
    bool *running = (bool *)arg;

    ESP_LOGI(TAG, "About to create a string stream");
    std::stringstream ss;
    ESP_LOGI(TAG, "About to write to string stream");
    ss << "Hello World!";
    ESP_LOGI(TAG, "About to extract from stringstream");
    ESP_LOGI(TAG, "Text: %s", ss.str().c_str());

    if (running) {
        *running = false;
        vTaskDelete(NULL);
    }
}

TEST_CASE("pthread mix C/C++", "[pthread]")
{
    bool running = true;

    std::thread t1(task_test_sandbox, (void *)NULL);
    xTaskCreatePinnedToCore((TaskFunction_t)&task_test_sandbox, "task_test_sandbox", 2048, &running, 5, NULL, 0);
    while (running) {
        vTaskDelay(1);
    }
    if (t1.joinable()) {
        std::cout << "Join thread " << std::hex << t1.get_id() << std::endl;
        t1.join();
    }
}

#endif
