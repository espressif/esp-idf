/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <vector>
#include <numeric>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "unity.h"
#include "unity_test_utils.h"
#include "soc/soc.h"

extern "C" void setUp()
{
    unity_utils_set_leak_level(0);
    unity_utils_record_free_mem();
}

extern "C" void tearDown()
{
    unity_utils_evaluate_leaks();
}

static const char* TAG = "cxx";

class NonPOD
{
public:
    NonPOD(int a_) : a(a_) { }
    int a;
};

static int non_pod_test_helper(int new_val)
{
    static NonPOD non_pod(42);
    int ret = non_pod.a;
    non_pod.a = new_val;
    return ret;
}

// Will fail if run twice
TEST_CASE("can use static initializers for non-POD types", "[restart_init]")
{
    unity_utils_set_leak_level(300);
    TEST_ASSERT_EQUAL(42, non_pod_test_helper(1));
    TEST_ASSERT_EQUAL(1, non_pod_test_helper(0));
}

/*
 * This test exercises static initialization guards for two objects.
 * For each object, 4 tasks are created which attempt to perform static initialization.
 * We check that constructor runs only once for each object.
 */

static SemaphoreHandle_t s_slow_init_sem = NULL;

template<int obj>
class SlowInit
{
public:
    SlowInit(int arg) {
        ESP_LOGD(TAG, "init obj=%d start, arg=%d", obj, arg);
        vTaskDelay(300/portTICK_PERIOD_MS);
        TEST_ASSERT_EQUAL(-1, mInitBy);
        TEST_ASSERT_EQUAL(0, mInitCount);
        mInitBy = arg;
        ++mInitCount;
        ESP_LOGD(TAG, "init obj=%d done", obj);
    }

    static void task(void* arg) {
        int taskId = reinterpret_cast<int>(arg);
        ESP_LOGD(TAG, "obj=%d before static init, task=%d", obj, taskId);
        static SlowInit slowinit(taskId);
        ESP_LOGD(TAG, "obj=%d after static init, task=%d", obj, taskId);
        xSemaphoreGive(s_slow_init_sem);
        vTaskDelete(NULL);
    }
private:
    static int mInitBy;
    static int mInitCount;
};

template<> int SlowInit<1>::mInitBy = -1;
template<> int SlowInit<1>::mInitCount = 0;
template<> int SlowInit<2>::mInitBy = -1;
template<> int SlowInit<2>::mInitCount = 0;

template<int obj>
static int start_slow_init_task(int id, int affinity)
{
    return xTaskCreatePinnedToCore(&SlowInit<obj>::task, "slow_init", 2048,
            reinterpret_cast<void*>(id), 3, NULL, affinity) ? 1 : 0;
}

TEST_CASE("static initialization guards work as expected", "[misc]")
{
    unity_utils_set_leak_level(300);
    s_slow_init_sem = xSemaphoreCreateCounting(10, 0);
    TEST_ASSERT_NOT_NULL(s_slow_init_sem);
    int task_count = 0;
    // four tasks competing for static initialization of one object
    task_count += start_slow_init_task<1>(0, PRO_CPU_NUM);
#if portNUM_PROCESSORS == 2
    task_count += start_slow_init_task<1>(1, APP_CPU_NUM);
#endif
    task_count += start_slow_init_task<1>(2, PRO_CPU_NUM);
    task_count += start_slow_init_task<1>(3, tskNO_AFFINITY);

    // four tasks competing for static initialization of another object
    task_count += start_slow_init_task<2>(0, PRO_CPU_NUM);
#if portNUM_PROCESSORS == 2
    task_count += start_slow_init_task<2>(1, APP_CPU_NUM);
#endif
    task_count += start_slow_init_task<2>(2, PRO_CPU_NUM);
    task_count += start_slow_init_task<2>(3, tskNO_AFFINITY);

    // All tasks should
    for (int i = 0; i < task_count; ++i) {
        TEST_ASSERT_TRUE(xSemaphoreTake(s_slow_init_sem, 500/portTICK_PERIOD_MS));
    }
    vSemaphoreDelete(s_slow_init_sem);

    vTaskDelay(10); // Allow tasks to clean up, avoids race with leak detector
}

struct GlobalInitTest
{
    GlobalInitTest() : index(order++) {
    }
    int index;
    static int order;
};

int GlobalInitTest::order = 0;

GlobalInitTest g_init_test1;
GlobalInitTest g_init_test2;
GlobalInitTest g_init_test3;

TEST_CASE("global initializers run in the correct order", "[misc]")
{
    TEST_ASSERT_EQUAL(0, g_init_test1.index);
    TEST_ASSERT_EQUAL(1, g_init_test2.index);
    TEST_ASSERT_EQUAL(2, g_init_test3.index);
}

struct StaticInitTestBeforeScheduler
{
    StaticInitTestBeforeScheduler()
    {
        static int first_init_order = getOrder();
        index = first_init_order;
    }

    int getOrder()
    {
        return order++;
    }

    int index;
    static int order;
};

int StaticInitTestBeforeScheduler::order = 1;

StaticInitTestBeforeScheduler g_static_init_test1;
StaticInitTestBeforeScheduler g_static_init_test2;
StaticInitTestBeforeScheduler g_static_init_test3;

TEST_CASE("before scheduler has started, static initializers work correctly", "[misc]")
{
    TEST_ASSERT_EQUAL(1, g_static_init_test1.index);
    TEST_ASSERT_EQUAL(1, g_static_init_test2.index);
    TEST_ASSERT_EQUAL(1, g_static_init_test3.index);
    TEST_ASSERT_EQUAL(2, StaticInitTestBeforeScheduler::order);
}

struct PriorityInitTest
{
    PriorityInitTest()
    {
        index = getOrder();
    }

    int getOrder()
    {
        return order++;
    }

    int index;
    static int order;
};

int PriorityInitTest::order = 0;

// init_priority objects are initialized from the lowest to the heighest priority number
// Default init_priority is always the lowest (highest priority number)
PriorityInitTest g_static_init_priority_test2;
PriorityInitTest g_static_init_priority_test1 __attribute__((init_priority(1000)));
PriorityInitTest g_static_init_priority_test0 __attribute__((init_priority(999)));

TEST_CASE("init_priority extension works", "[misc]")
{
    TEST_ASSERT_EQUAL(0, g_static_init_priority_test0.index);
    TEST_ASSERT_EQUAL(1, g_static_init_priority_test1.index);
    TEST_ASSERT_EQUAL(2, g_static_init_priority_test2.index);
}

TEST_CASE("can use new and delete", "[misc]")
{
    int* int_p = new int(10);
    delete int_p;
    int* int_array = new int[10];
    delete[] int_array;
}

class Base
{
public:
    virtual ~Base() = default;
    virtual void foo() = 0;
};

class Derived : public Base
{
public:
    virtual void foo() { }
};

TEST_CASE("can call virtual functions", "[misc]")
{
    Derived d;
    Base& b = static_cast<Base&>(d);
    b.foo();
}

TEST_CASE("can use std::vector", "[misc]")
{
    std::vector<int> v(10, 1);
    v[0] = 42;
    TEST_ASSERT_EQUAL(51, std::accumulate(std::begin(v), std::end(v), 0));
}

/* These test cases pull a lot of code from libstdc++ and are disabled for now
 */
#if 0
#include <iostream>
#include <functional>

TEST_CASE("can use iostreams", "[misc]")
{
    std::cout << "hello world";
}

TEST_CASE("can call std::function and bind", "[misc]")
{
    int outer = 1;
    std::function<int(int)> fn = [&outer](int x) -> int {
        return x + outer;
    };
    outer = 5;
    TEST_ASSERT_EQUAL(6, fn(1));

    auto bound = std::bind(fn, outer);
    outer = 10;
    TEST_ASSERT_EQUAL(15, bound());
}

#endif

static void recur_and_smash_cxx()
{
    static int cnt;
    volatile uint8_t buf[50];
    volatile int num = sizeof(buf)+10;

    if (cnt++ < 1) {
        recur_and_smash_cxx();
    }
    for (int i = 0; i < num; i++) {
        buf[i] = 0;
    }
}

TEST_CASE("stack smashing protection CXX", "[stack_smash]")
{
    recur_and_smash_cxx();
}

extern "C" void app_main(void)
{
    printf("CXX GENERAL TEST\n");
    unity_run_menu();
}

/* Tests below are done in the compile time, don't actually get run. */
/* Check whether a enumerator flag can be used in C++ */
template<typename T> __attribute__((unused)) static void test_binary_operators()
{
    T flag1 = (T)0;
    T flag2 = (T)0;
    flag1 = ~flag1;
    flag1 = flag1 | flag2;
    flag1 = flag1 & flag2;
    flag1 = flag1 ^ flag2;
    flag1 = flag1 >> 2;
    flag1 = flag1 << 2;
    flag1 |= flag2;
    flag1 &= flag2;
    flag1 ^= flag2;
    flag1 >>= 2;
    flag1 <<= 2;
}

//Add more types here. If any flags cannot pass the build, use FLAG_ATTR in esp_attr.h
