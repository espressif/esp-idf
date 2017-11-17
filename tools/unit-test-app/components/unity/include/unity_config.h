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
struct test_desc_t
{
	const char* name;
	const char* desc;
	void (*fn)(void);
	const char* file;
	int line;
	struct test_desc_t* next;
};

void unity_testcase_register(struct test_desc_t* desc);

void unity_run_menu();

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
		static struct test_desc_t UNITY_TEST_UID(test_desc_) = { \
			.name = name_, \
			.desc = desc_, \
			.fn = &UNITY_TEST_UID(test_func_), \
			.file = __FILE__, \
			.line = __LINE__, \
			.next = NULL \
		}; \
		unity_testcase_register( & UNITY_TEST_UID(test_desc_) ); \
	}\
	static void UNITY_TEST_UID(test_func_) (void)
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
