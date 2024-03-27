/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdexcept>
#include <typeinfo>
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
    unity_utils_set_leak_level(0);
    unity_utils_record_free_mem();
}

extern "C" void tearDown()
{
    unity_utils_evaluate_leaks();
}

using namespace std;

class Base {
public:
    virtual ~Base() {} ;
    virtual char name() = 0;
};

class DerivedA : public Base {
public:
    char name() override
    {
        return 'A';
    }
};

class DerivedB : public Base {
public:
    char name() override
    {
        return 'B';
    }
};

TEST_CASE("unsuccessful dynamic cast on pointer returns nullptr", "[cxx]")
{
    Base *base = new DerivedA();
    DerivedB *derived = dynamic_cast<DerivedB*>(base);
    TEST_ASSERT_EQUAL(derived, nullptr);
    delete base;
    derived = nullptr;
}

TEST_CASE("dynamic cast works", "[cxx]")
{
    Base *base = new DerivedA();
    DerivedA *derived = dynamic_cast<DerivedA*>(base);
    TEST_ASSERT_EQUAL(derived, base);
    delete base;
}

TEST_CASE("typeid of dynamic objects works", "[cxx]")
{
    Base *base = new DerivedA();
    DerivedA *derived = dynamic_cast<DerivedA*>(base);
    TEST_ASSERT_EQUAL(typeid(*derived).hash_code(), typeid(*base).hash_code());
    TEST_ASSERT_EQUAL(typeid(*derived).hash_code(), typeid(DerivedA).hash_code());
    delete base;
    derived = nullptr;
}

int dummy_function1(int arg1, double arg2);
int dummy_function2(int arg1, double arg2);

TEST_CASE("typeid of function works", "[cxx]")
{
    TEST_ASSERT_EQUAL(typeid(dummy_function1).hash_code(), typeid(dummy_function2).hash_code());
}

TEST_CASE("unsuccessful dynamic cast on reference throws exception", "[cxx]")
{
    unity_utils_set_leak_level(LEAKS);
    bool thrown = false;
    DerivedA derived_a;
    Base &base = derived_a;
    try {
        DerivedB &derived_b = dynamic_cast<DerivedB &>(base);
        derived_b.name(); // suppress warning
    } catch (bad_cast &e) {
        thrown = true;
    }
    TEST_ASSERT(thrown);
}

TEST_CASE("typeid on nullptr throws bad_typeid", "[cxx]")
{
    Base *base = nullptr;
    size_t hash = 0;
    bool thrown = false;
    try {
        hash = typeid(*base).hash_code();
    } catch (bad_typeid &e) {
        thrown = true;
    }
    TEST_ASSERT_EQUAL(0, hash);
    TEST_ASSERT(thrown);
}

extern "C" void app_main(void)
{
    printf("CXX RTTI TEST\n");
    unity_run_menu();
}
