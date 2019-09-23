#ifndef UNITY_CONFIG_H
#define UNITY_CONFIG_H

// This file gets included from unity.h via unity_internals.h
// It is inside #ifdef __cplusplus / extern "C" block, so we can
// only use C features here

#include <esp_err.h>
#include <stddef.h>
#include "sdkconfig.h"

#ifdef CONFIG_UNITY_ENABLE_FLOAT
#define UNITY_INCLUDE_FLOAT
#else
#define UNITY_EXCLUDE_FLOAT
#endif //CONFIG_UNITY_ENABLE_FLOAT

#ifdef CONFIG_UNITY_ENABLE_DOUBLE
#define UNITY_INCLUDE_DOUBLE
#else
#define UNITY_EXCLUDE_DOUBLE
#endif //CONFIG_UNITY_ENABLE_DOUBLE

#ifdef CONFIG_UNITY_ENABLE_COLOR
#define UNITY_OUTPUT_COLOR
#endif

#define UNITY_EXCLUDE_TIME_H

void unity_flush(void);
void unity_putc(int c);
void unity_gets(char* dst, size_t len);
void unity_exec_time_start(void);
void unity_exec_time_stop(void);
uint32_t unity_exec_time_get_ms(void);

#define UNITY_OUTPUT_CHAR(a)    unity_putc(a)
#define UNITY_OUTPUT_FLUSH()    unity_flush()
#define UNITY_EXEC_TIME_START() unity_exec_time_start()
#define UNITY_EXEC_TIME_STOP()  unity_exec_time_stop()
#define UNITY_EXEC_TIME_MS()    unity_exec_time_get_ms()

#ifdef CONFIG_UNITY_ENABLE_IDF_TEST_RUNNER

#include "unity_test_runner.h"

#endif //CONFIG_UNITY_ENABLE_IDF_TEST_RUNNER

// shorthand to check esp_err_t return code
#define TEST_ESP_OK(rc) TEST_ASSERT_EQUAL_HEX32(ESP_OK, rc)
#define TEST_ESP_ERR(err, rc) TEST_ASSERT_EQUAL_HEX32(err, rc)

#endif //UNITY_CONFIG_H
