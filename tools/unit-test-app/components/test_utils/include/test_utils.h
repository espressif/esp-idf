// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
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

// Utilities for esp-idf unit tests

#include <stdint.h>
#include <esp_partition.h>
#include "sdkconfig.h"

/* include performance pass standards header file */
#include "idf_performance.h"

/* For performance check with unity test on IDF */
/* These macros should only be used with ESP-IDF.
 * To use performance check, we need to first define pass standard in idf_performance.h.
 */
#define TEST_PERFORMANCE_LESS_THAN(name, value_fmt, value)  do { \
    printf("[Performance]["#name"]: "value_fmt"\n", value); \
    TEST_ASSERT(value < IDF_PERFORMANCE_MAX_##name); \
} while(0)

#define TEST_PERFORMANCE_GREATER_THAN(name, value_fmt, value)  do { \
    printf("[Performance]["#name"]: "value_fmt"\n", value); \
    TEST_ASSERT(value > IDF_PERFORMANCE_MIN_##name); \
} while(0)


/* @brief macro to print IDF performance
 * @param mode :        performance item name. a string pointer.
 * @param value_fmt:    print format and unit of the value, for example: "%02fms", "%dKB"
 * @param value :       the performance value.
*/
#define IDF_LOG_PERFORMANCE(item, value_fmt, value) \
    printf("[Performance][%s]: "value_fmt"\n", item, value)


/* Some definitions applicable to Unity running in FreeRTOS */
#define UNITY_FREERTOS_PRIORITY CONFIG_UNITY_FREERTOS_PRIORITY
#define UNITY_FREERTOS_CPU CONFIG_UNITY_FREERTOS_CPU
#define UNITY_FREERTOS_STACK_SIZE CONFIG_UNITY_FREERTOS_STACK_SIZE

/* Return the 'flash_test' custom data partition (type 0x55)
   defined in the custom partition table.
*/
const esp_partition_t *get_test_data_partition();

/**
 * @brief Initialize reference clock
 *
 * Reference clock provides timestamps at constant 1 MHz frequency, even when
 * the APB frequency is changing.
 */
void ref_clock_init();

/**
 * @brief Deinitialize reference clock
 */
void ref_clock_deinit();


/**
 * @brief Get reference clock timestamp
 * @return number of microseconds since the reference clock was initialized
 */
uint64_t ref_clock_get();

/**
 * @brief Entry point of the test application
 *
 * Starts Unity test runner in a separate task and returns.
 */
void test_main();

/**
 * @brief Reset automatic leak checking which happens in unit tests.
 *
 * Updates recorded "before" free memory values to the free memory values
 * at time of calling. Resets leak checker if tracing is enabled in
 * config.
 *
 * This can be called if a test case does something which allocates
 * memory on first use, for example.
 *
 * @note Use with care as this can mask real memory leak problems.
 */
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

/**
 * @brief Leak for components
 */
typedef enum {
    COMP_LEAK_GENERAL = 0,  /**< Leak by default */
    COMP_LEAK_LWIP,         /**< Leak for LWIP */
    COMP_LEAK_NVS,          /**< Leak for NVS */
    COMP_LEAK_ALL,          /**< Use for getting the summary leak level */
} esp_comp_leak_t;

/**
 * @brief Type of leak
 */
typedef enum {
    TYPE_LEAK_WARNING = 0,  /**< Warning level of leak */
    TYPE_LEAK_CRITICAL,     /**< Critical level of leak */
    TYPE_LEAK_MAX,          /**< Max number of leak levels */
} esp_type_leak_t;

/**
 * @brief Set a leak level for the required type and component.
 *
 * @param[in] leak_level Level of leak
 * @param[in] type       Type of leak
 * @param[in] component  Name of component
 *
 * return ESP_OK: Successful.
 *        ESP_ERR_INVALID_ARG: Invalid argument.
 */
esp_err_t test_utils_set_leak_level(size_t leak_level, esp_type_leak_t type, esp_comp_leak_t component);

/**
 * @brief Get a leak level for the required type and component.
 *
 * @param[in] type       Type of leak.
 * @param[in] component  Name of component. If COMP_LEAK_ALL, then the level will be summarized for all components.
 * return Leak level
 */
size_t test_utils_get_leak_level(esp_type_leak_t type, esp_comp_leak_t component);
