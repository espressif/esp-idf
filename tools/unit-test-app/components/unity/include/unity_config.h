#ifndef UNITY_CONFIG_H
#define UNITY_CONFIG_H

// This file gets included from unity.h via unity_internals.h
// It is inside #ifdef __cplusplus / extern "C" block, so we can
// only use C features here

// Adapt Unity to our environment, disable FP support

#include <esp_err.h>

/* Some definitions applicable to Unity running in FreeRTOS */
#define UNITY_FREERTOS_PRIORITY 5
#define UNITY_FREERTOS_CPU 0

#define UNITY_EXCLUDE_FLOAT
#define UNITY_EXCLUDE_DOUBLE

#define UNITY_OUTPUT_CHAR unity_putc
#define UNITY_OUTPUT_FLUSH unity_flush

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
    static char* UNITY_TEST_UID(test_fn_name)[] = FN_NAME_SET(PP_NARG(__VA_ARGS__), __VA_ARGS__)


typedef void (* test_func)(void);

struct test_desc_t
{
    const char* name;
    const char* desc;
    test_func* fn;
    const char* file;
    int line;
    uint8_t test_fn_count;
    char ** test_fn_name;
    struct test_desc_t* next;
};

void unity_testcase_register(struct test_desc_t* desc);

void unity_run_menu() __attribute__((noreturn));

void unity_run_tests_with_filter(const char* filter);

void unity_run_all_tests();

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
    static void __attribute__((constructor)) UNITY_TEST_UID(test_reg_helper_) () \
    { \
        static test_func test_fn_[] = {&UNITY_TEST_UID(test_func_)}; \
        static struct test_desc_t UNITY_TEST_UID(test_desc_) = { \
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
 * First argument is a free-form description,
 * second argument is (by convention) a list of identifiers, each one in square brackets.
 * subsequent arguments are names of test functions for different DUTs
 * e.g:
 * TEST_CASE_MULTIPLE_DEVICES("master and slave spi","[spi][test_env=UT_T2_1]", master_test, slave_test);
 * */

#define TEST_CASE_MULTIPLE_DEVICES(name_, desc_, ...) \
    UNITY_TEST_FN_SET(__VA_ARGS__); \
    static void __attribute__((constructor)) UNITY_TEST_UID(test_reg_helper_) () \
    { \
        static struct test_desc_t UNITY_TEST_UID(test_desc_) = { \
            .name = name_, \
            .desc = desc_, \
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

// shorthand to check esp_err_t return code
#define TEST_ESP_OK(rc)	TEST_ASSERT_EQUAL_HEX32(ESP_OK, rc)
#define TEST_ESP_ERR(err, rc) TEST_ASSERT_EQUAL_HEX32(err, rc)


#endif //UNITY_CONFIG_H
