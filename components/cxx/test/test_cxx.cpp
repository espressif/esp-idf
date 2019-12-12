#include <vector>
#include <numeric>
#include <stdexcept>
#include <string>
#include "unity.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "soc/soc.h"

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

TEST_CASE("static initialization guards work as expected", "[cxx]")
{
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

/* Note: When first exception (in system) is thrown this test produces memory leaks report (~500 bytes):
   - 392 bytes (can vary) as libunwind allocates memory to keep stack frames info to handle exceptions.
     This info is kept until global destructors are called by __do_global_dtors_aux()
   - 8 bytes are allocated by __cxa_get_globals() to keep __cxa_eh_globals
   - 16 bytes are allocated by pthread_setspecific() which is called by __cxa_get_globals() to init TLS var for __cxa_eh_globals
   - 88 bytes are allocated by pthread_setspecific() to init internal lock
   */
#ifdef CONFIG_COMPILER_CXX_EXCEPTIONS

TEST_CASE("c++ exceptions work", "[cxx] [exceptions] [leaks=800]")
{
    int thrown_value;
    try {
        throw 20;
    } catch (int e) {
        thrown_value = e;
    }
    TEST_ASSERT_EQUAL(20, thrown_value);
    printf("OK?\n");
}

TEST_CASE("c++ bool exception", "[cxx] [exceptions] [leaks=800]")
{
    bool thrown_value = false;
    try {
        throw true;
    } catch (bool e) {
        thrown_value = e;
    }
    TEST_ASSERT_EQUAL(true, thrown_value);
    printf("OK?\n");
}

TEST_CASE("c++ void exception", "[cxx] [exceptions] [leaks=800]")
{
    void* thrown_value = 0;
    try {
        throw (void*) 47;
    } catch (void* e) {
        thrown_value = e;
    }
    TEST_ASSERT_EQUAL(47, thrown_value);
    printf("OK?\n");
}

TEST_CASE("c++ uint64_t exception", "[cxx] [exceptions] [leaks=800]")
{
    uint64_t thrown_value = 0;
    try {
        throw (uint64_t) 47;
    } catch (uint64_t e) {
        thrown_value = e;
    }
    TEST_ASSERT_EQUAL(47, thrown_value);
    printf("OK?\n");
}

TEST_CASE("c++ char exception", "[cxx] [exceptions] [leaks=800]")
{
    char thrown_value = '0';
    try {
        throw '/';
    } catch (char e) {
        thrown_value = e;
    }
    TEST_ASSERT_EQUAL('/', thrown_value);
    printf("OK?\n");
}

TEST_CASE("c++ wchar exception", "[cxx] [exceptions] [leaks=800]")
{
    wchar_t thrown_value = 0;
    try {
        throw (wchar_t) 47;
    } catch (wchar_t e) {
        thrown_value = e;
    }
    TEST_ASSERT_EQUAL((wchar_t) 47, thrown_value);
    printf("OK?\n");
}

TEST_CASE("c++ float exception", "[cxx] [exceptions] [leaks=800]")
{
    float thrown_value = 0;
    try {
        throw 23.5f;
    } catch (float e) {
        thrown_value = e;
    }
    TEST_ASSERT_EQUAL(23.5, thrown_value);
    printf("OK?\n");
}

TEST_CASE("c++ double exception", "[cxx] [exceptions] [leaks=800]")
{
    double thrown_value = 0;
    try {
        throw 23.5d;
    } catch (double e) {
        thrown_value = e;
    }
    TEST_ASSERT_EQUAL(23.5d, thrown_value);
    printf("OK?\n");
}

TEST_CASE("c++ const char* exception", "[cxx] [exceptions] [leaks=800]")
{
    const char *thrown_value = 0;
    try {
        throw "Hi :)";
    } catch (const char *e) {
        thrown_value = e;
    }
    TEST_ASSERT_EQUAL_STRING("Hi :)", thrown_value);
    printf("OK?\n");
}

struct NonExcTypeThrowee {
    int value;
public:
    NonExcTypeThrowee(int value) : value(value) { }
};

TEST_CASE("c++ any class exception", "[cxx] [exceptions] [leaks=800]")
{
    int thrown_value = 0;
    try {
        throw NonExcTypeThrowee(47);
    } catch (NonExcTypeThrowee &e) {
        thrown_value = e.value;
    }
    TEST_ASSERT_EQUAL(47, thrown_value);
    printf("OK?\n");
}

struct ExcTypeThrowee : public std::exception {
    int value;
public:
    ExcTypeThrowee(int value) : value(value) { }
};

TEST_CASE("c++ std::exception child", "[cxx] [exceptions] [leaks=800]")
{
    int thrown_value = 0;
    try {
        throw ExcTypeThrowee(47);
    } catch (ExcTypeThrowee &e) {
        thrown_value = e.value;
    }
    TEST_ASSERT_EQUAL(47, thrown_value);
    printf("OK?\n");
}

TEST_CASE("c++ exceptions emergency pool", "[cxx] [exceptions] [ignore]")
{
    void **p, **pprev = NULL;
    int thrown_value = 0;
    // throw first exception to ensure that all initial allocations are made
    try
    {
        throw 33;
    }
    catch (int e)
    {
        thrown_value = e;
    }
    TEST_ASSERT_EQUAL(33, thrown_value);
    // consume all dynamic memory
    while ((p = (void **)malloc(sizeof(void *)))) {
        if (pprev) {
            *p = pprev;
        } else {
            *p = NULL;
        }
        pprev = p;
    }
    try
    {
        throw 20;
    }
    catch (int e)
    {
        thrown_value = e;
        printf("Got exception %d\n", thrown_value);
    }
#if CONFIG_COMPILER_CXX_EXCEPTIONS_EMG_POOL_SIZE > 0
    // free all memory
    while (pprev) {
        p = (void **)(*pprev);
        free(pprev);
        pprev = p;
    }
    TEST_ASSERT_EQUAL(20, thrown_value);
#else
    // if emergency pool is disabled we should never get here,
    // expect abort() due to lack of memory for new exception
    TEST_ASSERT_TRUE(0 == 1);
#endif
}

#else // !CONFIG_COMPILER_CXX_EXCEPTIONS

TEST_CASE_ESP32("std::out_of_range exception when -fno-exceptions", "[cxx][reset=abort,SW_CPU_RESET]")
{
    std::vector<int> v(10);
    v.at(20) = 42;
    TEST_FAIL_MESSAGE("Unreachable because we are aborted on the line above");
}

TEST_CASE_ESP32("std::bad_alloc exception when -fno-exceptions", "[cxx][reset=abort,SW_CPU_RESET]")
{
    std::string s = std::string(2000000000, 'a');
    (void)s;
    TEST_FAIL_MESSAGE("Unreachable because we are aborted on the line above");
}

#endif

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
#include "hal/timer_types.h"
template void test_binary_operators<timer_intr_t>();




