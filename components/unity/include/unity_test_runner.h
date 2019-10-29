// Copyright 2016-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#include <stdint.h>
#include <stdbool.h>

// This file gets included from unity.h via unity_internals.h via unity_config.h
// It is inside #ifdef __cplusplus / extern "C" block, so we can
// only use C features here

// Define helpers to register test cases from multiple files
#define UNITY_EXPAND2(a, b) a ## b
#define UNITY_EXPAND(a, b) UNITY_EXPAND2(a, b)
#define UNITY_TEST_UID(what) UNITY_EXPAND(what, __LINE__)

#define UNITY_TEST_REG_HELPER reg_helper ## UNITY_TEST_UID
#define UNITY_TEST_DESC_UID desc ## UNITY_TEST_UID


// get count of __VA_ARGS__
#define PP_NARG(...) \
         PP_NARG_(__VA_ARGS__,PP_RSEQ_N())
#define PP_NARG_(...) \
         PP_ARG_N(__VA_ARGS__)
#define PP_ARG_N( \
          _1, _2, _3, _4, _5, _6, _7, _8, _9, N, ...) N
#define PP_RSEQ_N() 9,8,7,6,5,4,3,2,1,0

// support max 5 test func now
#define FN_NAME_SET_1(a)                {#a}
#define FN_NAME_SET_2(a, b)             {#a, #b}
#define FN_NAME_SET_3(a, b, c)          {#a, #b, #c}
#define FN_NAME_SET_4(a, b, c, d)       {#a, #b, #c, #d}
#define FN_NAME_SET_5(a, b, c, d, e)    {#a, #b, #c, #d, #e}

#define FN_NAME_SET2(n) FN_NAME_SET_##n
#define FN_NAME_SET(n, ...) FN_NAME_SET2(n)(__VA_ARGS__)

#define UNITY_TEST_FN_SET(...)  \
    static test_func UNITY_TEST_UID(test_functions)[] = {__VA_ARGS__}; \
    static const char* UNITY_TEST_UID(test_fn_name)[] = FN_NAME_SET(PP_NARG(__VA_ARGS__), __VA_ARGS__)


typedef void (* test_func)(void);

typedef struct test_desc_t
{
    const char* name;
    const char* desc;
    test_func* fn;
    const char* file;
    int line;
    uint8_t test_fn_count;
    const char ** test_fn_name;
    struct test_desc_t* next;
} test_desc_t;

void unity_testcase_register(test_desc_t* desc);


/*  Test case macro, a-la CATCH framework.
    First argument is a free-form description,
    second argument is (by convention) a list of identifiers, each one in square brackets.
    Identifiers are used to group related tests, or tests with specific properties.
    Use like:

    TEST_CASE("Frobnicator forbnicates", "[frobnicator][rom]")
    {
        // test goes here
    }
*/

#define TEST_CASE(name_, desc_) \
    static void UNITY_TEST_UID(test_func_) (void); \
    static void __attribute__((constructor)) UNITY_TEST_UID(test_reg_helper_) (void) \
    { \
        static test_func test_fn_[] = {&UNITY_TEST_UID(test_func_)}; \
        static test_desc_t UNITY_TEST_UID(test_desc_) = { \
            .name = name_, \
            .desc = desc_, \
            .fn = test_fn_, \
            .file = __FILE__, \
            .line = __LINE__, \
            .test_fn_count = 1, \
            .test_fn_name = NULL, \
            .next = NULL \
        }; \
        unity_testcase_register( & UNITY_TEST_UID(test_desc_) ); \
    }\
    static void UNITY_TEST_UID(test_func_) (void)


/*
 * Multiple stages test cases will handle the case that test steps are separated by DUT reset.
 * e.g: we want to verify some function after SW reset, WDT reset or deep sleep reset.
 *
 * First argument is a free-form description,
 * second argument is (by convention) a list of identifiers, each one in square brackets.
 * subsequent arguments are names test functions separated by reset.
 * e.g:
 * TEST_CASE_MULTIPLE_STAGES("run light sleep after deep sleep","[sleep]", goto_deepsleep, light_sleep_after_deep_sleep_wakeup);
 * */

#define TEST_CASE_MULTIPLE_STAGES(name_, desc_, ...) \
    UNITY_TEST_FN_SET(__VA_ARGS__); \
    static void __attribute__((constructor)) UNITY_TEST_UID(test_reg_helper_) (void) \
    { \
        static test_desc_t UNITY_TEST_UID(test_desc_) = { \
            .name = name_, \
            .desc = desc_"[multi_stage]", \
            .fn = UNITY_TEST_UID(test_functions), \
            .file = __FILE__, \
            .line = __LINE__, \
            .test_fn_count = PP_NARG(__VA_ARGS__), \
            .test_fn_name = UNITY_TEST_UID(test_fn_name), \
            .next = NULL \
        }; \
        unity_testcase_register( & UNITY_TEST_UID(test_desc_) ); \
    }



/*
 * First argument is a free-form description,
 * second argument is (by convention) a list of identifiers, each one in square brackets.
 * subsequent arguments are names of test functions for different DUTs
 * e.g:
 * TEST_CASE_MULTIPLE_DEVICES("master and slave spi","[spi][test_env=UT_T2_1]", master_test, slave_test);
 * */

#define TEST_CASE_MULTIPLE_DEVICES(name_, desc_, ...) \
    UNITY_TEST_FN_SET(__VA_ARGS__); \
    static void __attribute__((constructor)) UNITY_TEST_UID(test_reg_helper_) (void) \
    { \
        static test_desc_t UNITY_TEST_UID(test_desc_) = { \
            .name = name_, \
            .desc = desc_"[multi_device]", \
            .fn = UNITY_TEST_UID(test_functions), \
            .file = __FILE__, \
            .line = __LINE__, \
            .test_fn_count = PP_NARG(__VA_ARGS__), \
            .test_fn_name = UNITY_TEST_UID(test_fn_name), \
            .next = NULL \
        }; \
        unity_testcase_register( & UNITY_TEST_UID(test_desc_) ); \
    }

/**
 * Note: initialization of test_desc_t fields above has to be done exactly
 * in the same order as the fields are declared in the structure.
 * Otherwise the initializer will not be valid in C++ (which doesn't
 * support designated initializers). G++ can parse the syntax, but
 * field names are treated as annotations and don't affect initialization
 * order. Also make sure all the fields are initialized.
 */

void unity_run_test_by_name(const char *name);

void unity_run_tests_by_tag(const char *tag, bool invert);

void unity_run_all_tests(void);

void unity_run_menu(void);

#include "sdkconfig.h"
#if CONFIG_IDF_TARGET_ESP32
#define TEST_CASE_ESP32(...) TEST_CASE(__VA_ARGS__)
#define TEST_CASE_MULTIPLE_STAGES_ESP32(...) TEST_CASE_MULTIPLE_STAGES(__VA_ARGS__)
#define TEST_CASE_MULTIPLE_DEVICES_ESP32(...) TEST_CASE_MULTIPLE_DEVICES(__VA_ARGS__)
#else
#define TEST_CASE_ESP32(...) __attribute__((unused)) static void UNITY_TEST_UID(test_func_) (void)
#define TEST_CASE_MULTIPLE_STAGES_ESP32(_, __, ...) __attribute__((unused)) static test_func UNITY_TEST_UID(test_functions)[] = {__VA_ARGS__};
#define TEST_CASE_MULTIPLE_DEVICES_ESP32(_, __, ...) __attribute__((unused)) static test_func UNITY_TEST_UID(test_functions)[] = {__VA_ARGS__};

#endif
