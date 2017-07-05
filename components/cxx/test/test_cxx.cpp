#include <vector>
#include <algorithm>
#include "unity.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

static const char* TAG = "cxx";

TEST_CASE("can use new and delete", "[cxx]")
{
    int* int_p = new int(10);
    delete int_p;
    int* int_array = new int[10];
    delete[] int_array;
}

class Base
{
public:
    virtual ~Base() {}
    virtual void foo() = 0; 
};

class Derived : public Base
{
public:
    virtual void foo() { }
};

TEST_CASE("can call virtual functions", "[cxx]")
{
    Derived d;
    Base& b = static_cast<Base&>(d);
    b.foo();
}

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

TEST_CASE("can use static initializers for non-POD types", "[cxx]")
{
    TEST_ASSERT_EQUAL(42, non_pod_test_helper(1));
    TEST_ASSERT_EQUAL(1, non_pod_test_helper(0));
}

TEST_CASE("can use std::vector", "[cxx]")
{
    std::vector<int> v(10, 1);
    v[0] = 42;
    TEST_ASSERT_EQUAL(51, std::accumulate(std::begin(v), std::end(v), 0));
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
        ESP_LOGD(TAG, "init obj=%d start, arg=%d\n", obj, arg);
        vTaskDelay(300/portTICK_PERIOD_MS);
        TEST_ASSERT_EQUAL(-1, mInitBy);
        TEST_ASSERT_EQUAL(0, mInitCount);
        mInitBy = arg;
        ++mInitCount;
        ESP_LOGD(TAG, "init obj=%d done\n", obj);
    }

    static void task(void* arg) {
        int taskId = reinterpret_cast<int>(arg);
        ESP_LOGD(TAG, "obj=%d before static init, task=%d\n", obj, taskId);
        static SlowInit slowinit(taskId);
        ESP_LOGD(TAG, "obj=%d after static init, task=%d\n", obj, taskId);
        xSemaphoreGive(s_slow_init_sem);
        vTaskDelay(10);
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
static void start_slow_init_task(int id, int affinity)
{
    xTaskCreatePinnedToCore(&SlowInit<obj>::task, "slow_init", 2048,
            reinterpret_cast<void*>(id), 3, NULL, affinity);
}

TEST_CASE("static initialization guards work as expected", "[cxx]")
{
    s_slow_init_sem = xSemaphoreCreateCounting(10, 0);
    TEST_ASSERT_NOT_NULL(s_slow_init_sem);
    // four tasks competing for static initialization of one object
    start_slow_init_task<1>(0, PRO_CPU_NUM);
    start_slow_init_task<1>(1, APP_CPU_NUM);
    start_slow_init_task<1>(2, PRO_CPU_NUM);
    start_slow_init_task<1>(3, tskNO_AFFINITY);

    // four tasks competing for static initialization of another object
    start_slow_init_task<2>(0, PRO_CPU_NUM);
    start_slow_init_task<2>(1, APP_CPU_NUM);
    start_slow_init_task<2>(2, PRO_CPU_NUM);
    start_slow_init_task<2>(3, tskNO_AFFINITY);

    // All tasks should
    for (int i = 0; i < 8; ++i) {
        TEST_ASSERT_TRUE(xSemaphoreTake(s_slow_init_sem, 500/portTICK_PERIOD_MS));
    }
    vSemaphoreDelete(s_slow_init_sem);
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

TEST_CASE("global initializers run in the correct order", "[cxx]")
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

TEST_CASE("before scheduler has started, static initializers work correctly", "[cxx]")
{
    TEST_ASSERT_EQUAL(1, g_static_init_test1.index);
    TEST_ASSERT_EQUAL(1, g_static_init_test2.index);
    TEST_ASSERT_EQUAL(1, g_static_init_test3.index);
    TEST_ASSERT_EQUAL(2, StaticInitTestBeforeScheduler::order);
}

/* These test cases pull a lot of code from libstdc++ and are disabled for now
 */
#if 0
#include <iostream>
#include <functional>

TEST_CASE("can use iostreams", "[cxx]")
{
    std::cout << "hello world";
}

TEST_CASE("can call std::function and bind", "[cxx]")
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

