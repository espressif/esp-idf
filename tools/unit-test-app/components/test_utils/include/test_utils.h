/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

// Utilities for esp-idf unit tests

#include <stdint.h>
#include <esp_partition.h>
#include "sdkconfig.h"
#include "unity.h"
#include "soc/soc_caps.h"
/* include performance pass standards header file */
#include "idf_performance.h"
#include "idf_performance_target.h"

#ifdef __cplusplus
extern "C" {
#endif

/* For performance check with unity test on IDF */
/* These macros should only be used with ESP-IDF.
 * To use performance check, we need to first define pass standard in idf_performance.h.
 */

//macros call this to expand an argument instead of directly converting into str
#define PERFORMANCE_STR(s)   #s
//macros call this to contact strings after expanding them
#define PERFORMANCE_CON(a, b) _PERFORMANCE_CON(a, b)
#define _PERFORMANCE_CON(a, b) a##b

#if !CONFIG_UNITY_IGNORE_PERFORMANCE_TESTS
#define _TEST_PERFORMANCE_ASSERT TEST_ASSERT
#else
#define _TEST_PERFORMANCE_ASSERT(ARG) printf("Ignoring performance test [%s]\n", PERFORMANCE_STR(ARG))
#endif

#define TEST_PERFORMANCE_LESS_THAN(name, value_fmt, value, ...)  do { \
    IDF_LOG_PERFORMANCE(#name, value_fmt, value, ##__VA_ARGS__); \
    _TEST_PERFORMANCE_ASSERT(value < PERFORMANCE_CON(IDF_PERFORMANCE_MAX_, name)); \
} while(0)

#define TEST_PERFORMANCE_GREATER_THAN(name, value_fmt, value, ...)  do { \
    IDF_LOG_PERFORMANCE(#name, value_fmt, value, ##__VA_ARGS__); \
    _TEST_PERFORMANCE_ASSERT(value > PERFORMANCE_CON(IDF_PERFORMANCE_MIN_, name)); \
} while(0)

/* Macros to be used when performance is calculated using the cache compensated timer
   will not assert if ccomp not supported */
#if SOC_CCOMP_TIMER_SUPPORTED
#define TEST_PERFORMANCE_CCOMP_GREATER_THAN(name, value_fmt, value, ...) \
    TEST_PERFORMANCE_GREATER_THAN(name, value_fmt, value, ##__VA_ARGS__)
#define TEST_PERFORMANCE_CCOMP_LESS_THAN(name, value_fmt, value, ...) \
    TEST_PERFORMANCE_LESS_THAN(name, value_fmt, value, ##__VA_ARGS__)
#else
#define TEST_PERFORMANCE_CCOMP_GREATER_THAN(name, value_fmt, value, ...) \
    IDF_LOG_PERFORMANCE(#name, value_fmt, value, ##__VA_ARGS__)
#define TEST_PERFORMANCE_CCOMP_LESS_THAN(name, value_fmt, value, ...) \
    IDF_LOG_PERFORMANCE(#name, value_fmt, value, ##__VA_ARGS__)
#endif //SOC_CCOMP_TIMER_SUPPORTED


/* @brief macro to print IDF performance
 * @param item :        performance item name. a string pointer.
 * @param value_fmt:    print format and unit of the value, for example: "%02fms", "%dKB"
 * @param value :       the performance value.
*/
#define IDF_LOG_PERFORMANCE(item, value_fmt, value, ...) \
    printf("[Performance][%s]: " value_fmt "\n", item, value, ##__VA_ARGS__)


/* Some definitions applicable to Unity running in FreeRTOS */
#define UNITY_FREERTOS_PRIORITY CONFIG_UNITY_FREERTOS_PRIORITY
#define UNITY_FREERTOS_CPU CONFIG_UNITY_FREERTOS_CPU
#define UNITY_FREERTOS_STACK_SIZE CONFIG_UNITY_FREERTOS_STACK_SIZE

/* Return the 'flash_test' custom data partition (type 0x55)
   defined in the custom partition table.
*/
const esp_partition_t *get_test_data_partition(void);

/**
 * @brief Initialize reference clock
 *
 * Reference clock provides timestamps at constant 1 MHz frequency, even when
 * the APB frequency is changing.
 */
void ref_clock_init(void);

/**
 * @brief Deinitialize reference clock
 */
void ref_clock_deinit(void);


/**
 * @brief Get reference clock timestamp
 * @return number of microseconds since the reference clock was initialized
 */
uint64_t ref_clock_get(void);

/**
 * @brief Entry point of the test application
 *
 * Starts Unity test runner in a separate task and returns.
 */
void test_main(void);

void unity_reset_leak_checks(void);


/**
 * @brief Call this function from a test case which requires TCP/IP or
 * LWIP functionality.
 *
 * @note This should be the first function the test case calls, as it will
 * allocate memory on first use (and also reset the test case leak checker).
 */
void test_case_uses_tcpip(void);

/**
 * @brief wait for signals with parameters.
 *
 * for multiple devices test cases, DUT might need to wait for other DUTs before continue testing.
 * As all DUTs are independent, need user (or test script) interaction to make test synchronized.
 *
 * Here we provide signal functions for this.
 * For example, we're testing GPIO, DUT1 has one pin connect to with DUT2.
 * DUT2 will output high level and then DUT1 will read input.
 * DUT1 should call `unity_wait_for_signal("output high level");` before it reads input.
 * DUT2 should call `unity_send_signal("output high level");` after it finished setting output high level.
 * According to the console logs:
 *
 * DUT1 console:
 *
 * ```
 *     Waiting for signal: [output high level]!
 *     Please press "Enter" key to once any board send this signal.
 * ```
 *
 * DUT2 console:
 *
 * ```
 *     Send signal: [output high level]!
 * ```
 *
 * Then we press Enter key on DUT1's console, DUT1 starts to read input and then test success.
 *
 * Another example, we have 2 DUTs in multiple devices test, and DUT1 need to get DUT2's mac address to perform BT connection.
 * DUT1 should call `unity_wait_for_signal_param("dut2 mac address", mac, 19);` to wait for DUT2's mac address.
 * DUT2 should call `unity_send_signal_param("dut2 mac address", "10:20:30:40:50:60");` to send to DUT1 its mac address.
 * According to the console logs:
 *
 * DUT1 console:
 *
 * ```
 *     Waiting for signal: [dut2 mac address]!
 *     Please input parameter value from any board send this signal and press "Enter" key.
 * ```
 *
 * DUT2 console:
 *
 * ```
 *     Send signal: [dut2 mac address][10:20:30:40:50:60]!
 * ```
 *
 * @param signal_name signal name which DUT expected to wait before proceed testing
 * @param parameter_buf buffer to receive parameter
 * @param buf_len length of parameter_buf.
 *                Currently we have a limitation that it will write 1 extra byte at the end of string.
 *                We need to use a buffer with 2 bytes longer than actual string length.
 */
void unity_wait_for_signal_param(const char* signal_name, char *parameter_buf, uint8_t buf_len);

/**
 * @brief wait for signals.
 *
 * @param signal_name signal name which DUT expected to wait before proceed testing
 */
static inline void unity_wait_for_signal(const char* signal_name)
{
    unity_wait_for_signal_param(signal_name, NULL, 0);
}

/**
 * @brief DUT send signal and pass parameter to other devices.
 *
 * @param signal_name signal name which DUT send once it finished preparing.
 * @param parameter a string to let remote device to receive.
 */
void unity_send_signal_param(const char* signal_name, const char *parameter);

/**
 * @brief DUT send signal with parameter.
 *
 * @param signal_name signal name which DUT send once it finished preparing.
 */
static inline void unity_send_signal(const char* signal_name)
{
    unity_send_signal_param(signal_name, NULL);
}

/**
 * @brief convert mac string to mac address
 *
 * @param mac_str MAC address string with format "xx:xx:xx:xx:xx:xx"
 * @param[out] mac_addr store converted MAC address
 */
bool unity_util_convert_mac_from_string(const char* mac_str, uint8_t *mac_addr);

typedef struct test_utils_exhaust_memory_record_s *test_utils_exhaust_memory_rec;

/**
 * Limit the largest free block of memory with a particular capability set to
 * 'limit' bytes (meaning an allocation of 'limit' should succeed at least once,
 * but any allocation of more bytes will fail.)
 *
 * Returns a record pointer which needs to be passed back in to test_utils_free_exhausted_memory
 * before the test completes, to avoid a major memory leak.
 *
 * @param caps Capabilities of memory to exhause
 * @param limit The size to limit largest free block to
 * @return Record pointer to pass to test_utils_free_exhausted_memory() once done
 */
test_utils_exhaust_memory_rec test_utils_exhaust_memory(uint32_t caps, size_t limit);


/**
 * Call to free memory which was taken up by test_utils_exhaust_memory() call
 *
 * @param rec Result previously returned from test_utils_exhaust_memory()
 */
void test_utils_free_exhausted_memory(test_utils_exhaust_memory_rec rec);

#ifdef __cplusplus
}
#endif
