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

TEST_CASE("can use std::vector", "[cxx]")
{
    std::vector<int> v(10, 1);
    v[0] = 42;
    TEST_ASSERT_EQUAL(51, std::accumulate(std::begin(v), std::end(v), 0));
}

#ifdef CONFIG_COMPILER_CXX_EXCEPTIONS

TEST_CASE("c++ exceptions work", "[cxx]")
{
    /* Note: When first exception (in system) is thrown this test produces memory leaks report (~500 bytes):
       - 392 bytes (can vary) as libunwind allocates memory to keep stack frames info to handle exceptions.
         This info is kept until global destructors are called by __do_global_dtors_aux()
       - 8 bytes are allocated by __cxa_get_globals() to keep __cxa_eh_globals
       - 16 bytes are allocated by pthread_setspecific() which is called by __cxa_get_globals() to init TLS var for __cxa_eh_globals
       - 88 bytes are allocated by pthread_setspecific() to init internal lock
       */
    int thrown_value;
    try
    {
        throw 20;
    }
    catch (int e)
    {
        thrown_value = e;
    }
    TEST_ASSERT_EQUAL(20, thrown_value);
    printf("OK?\n");
}

TEST_CASE("c++ exceptions emergency pool", "[cxx] [ignore]")
{
    /* Note: When first exception (in system) is thrown this test produces memory leaks report (~500 bytes):
       - 392 bytes (can vary) as libunwind allocates memory to keep stack frames info to handle exceptions.
         This info is kept until global destructors are called by __do_global_dtors_aux()
       - 8 bytes are allocated by __cxa_get_globals() to keep __cxa_eh_globals
       - 16 bytes are allocated by pthread_setspecific() which is called by __cxa_get_globals() to init TLS var for __cxa_eh_globals
       - 88 bytes are allocated by pthread_setspecific() to init internal lock
       */
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

TEST_CASE("std::out_of_range exception when -fno-exceptions", "[cxx][reset=abort,SW_CPU_RESET]")
{
    std::vector<int> v(10);
    v.at(20) = 42;
    TEST_FAIL_MESSAGE("Unreachable because we are aborted on the line above");
}

TEST_CASE("std::bad_alloc exception when -fno-exceptions", "[cxx][reset=abort,SW_CPU_RESET]")
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

