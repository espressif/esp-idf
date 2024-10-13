/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdexcept>
#include "unity.h"
#include "unity_test_utils.h"

/* Note: When first exception (in system) is thrown this test produces memory leaks report (~300 bytes):
   - 8 bytes are allocated by __cxa_get_globals() to keep __cxa_eh_globals
   - 16 bytes are allocated by pthread_setspecific() which is called by __cxa_get_globals() to init TLS var for __cxa_eh_globals
   - 88 bytes are allocated by pthread_setspecific() to init internal lock
   - some more memory...
   */
#if CONFIG_IDF_TARGET_ESP32
#define LEAKS (300)
#elif CONFIG_IDF_TARGET_ESP32S2
#define LEAKS (800)
#elif CONFIG_IDF_TARGET_ESP32C3
#define LEAKS (700)
#else
#error "unknown target in CXX tests, can't set leaks threshold"
#endif

extern "C" void setUp()
{
    unity_utils_record_free_mem();
}

extern "C" void tearDown()
{
    unity_utils_evaluate_leaks_direct(LEAKS);
}

TEST_CASE("c++ exceptions work", "[cxx] [exceptions]")
{
    int thrown_value;
    try {
        throw 20;
    } catch (int e) {
        thrown_value = e;
    }
    TEST_ASSERT_EQUAL(20, thrown_value);
}

TEST_CASE("c++ bool exception", "[cxx] [exceptions]")
{
    bool thrown_value = false;
    try {
        throw true;
    } catch (bool e) {
        thrown_value = e;
    }
    TEST_ASSERT_EQUAL(true, thrown_value);
}

TEST_CASE("c++ void exception", "[cxx] [exceptions]")
{
    void* thrown_value = 0;
    try {
        throw (void*) 47;
    } catch (void* e) {
        thrown_value = e;
    }
    TEST_ASSERT_EQUAL(47, thrown_value);
}

TEST_CASE("c++ uint64_t exception", "[cxx] [exceptions]")
{
    uint64_t thrown_value = 0;
    try {
        throw (uint64_t) 47;
    } catch (uint64_t e) {
        thrown_value = e;
    }
    TEST_ASSERT_EQUAL(47, thrown_value);
}

TEST_CASE("c++ char exception", "[cxx] [exceptions]")
{
    char thrown_value = '0';
    try {
        throw '/';
    } catch (char e) {
        thrown_value = e;
    }
    TEST_ASSERT_EQUAL('/', thrown_value);
}

TEST_CASE("c++ wchar exception", "[cxx] [exceptions]")
{
    wchar_t thrown_value = 0;
    try {
        throw (wchar_t) 47;
    } catch (wchar_t e) {
        thrown_value = e;
    }
    TEST_ASSERT_EQUAL((wchar_t) 47, thrown_value);
}

TEST_CASE("c++ float exception", "[cxx] [exceptions]")
{
    float thrown_value = 0;
    try {
        throw 23.5f;
    } catch (float e) {
        thrown_value = e;
    }
    TEST_ASSERT_EQUAL(23.5, thrown_value);
}

TEST_CASE("c++ double exception", "[cxx] [exceptions]")
{
    double thrown_value = 0;
    try {
        throw 23.5d;
    } catch (double e) {
        thrown_value = e;
    }
    TEST_ASSERT_EQUAL(23.5d, thrown_value);
}

TEST_CASE("c++ const char* exception", "[cxx] [exceptions]")
{
    const char *thrown_value = 0;
    try {
        throw "Hi :)";
    } catch (const char *e) {
        thrown_value = e;
    }
    TEST_ASSERT_EQUAL_STRING("Hi :)", thrown_value);
}

struct NonExcTypeThrowee {
    int value;
public:
    NonExcTypeThrowee(int value) : value(value) { }
};

TEST_CASE("c++ any class exception", "[cxx] [exceptions]")
{
    int thrown_value = 0;
    try {
        throw NonExcTypeThrowee(47);
    } catch (NonExcTypeThrowee &e) {
        thrown_value = e.value;
    }
    TEST_ASSERT_EQUAL(47, thrown_value);
}

struct ExcTypeThrowee : public std::exception {
    int value;
public:
    ExcTypeThrowee(int value) : value(value) { }
};

TEST_CASE("c++ std::exception child", "[cxx] [exceptions]")
{
    int thrown_value = 0;
    try {
        throw ExcTypeThrowee(47);
    } catch (ExcTypeThrowee &e) {
        thrown_value = e.value;
    }
    TEST_ASSERT_EQUAL(47, thrown_value);
}

TEST_CASE("c++ exceptions emergency pool", "[cxx] [exceptions]")
{
    void **p, **pprev = NULL;
    int thrown_value = 0;
    // throw first exception to ensure that all initial allocations are made
    try {
        throw 33;
    } catch (int e) {
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
    try {
        throw 20;
    } catch (int e) {
        thrown_value = e;
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

extern "C" void app_main(void)
{
    printf("CXX EXCEPTION TEST\n");
    unity_run_menu();
}
