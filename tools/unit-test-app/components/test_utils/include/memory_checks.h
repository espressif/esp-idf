/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Leak for components
 */
typedef enum {
    ESP_COMP_LEAK_GENERAL = 0,  /**< Leak by default */
    ESP_COMP_LEAK_LWIP,         /**< Leak for LWIP */
    ESP_COMP_LEAK_NVS,          /**< Leak for NVS */
    ESP_COMP_LEAK_ALL,          /**< Use for getting the summary leak level */
} esp_comp_leak_t;

/**
 * @brief Type of a leak threshold
 */
typedef enum {
    ESP_LEAK_TYPE_WARNING = 0,  /**< Warning level of leak */
    ESP_LEAK_TYPE_CRITICAL,     /**< Critical level of leak */
    ESP_LEAK_TYPE_MAX,          /**< Max number of leak levels for all components/levels */
} esp_type_leak_t;

/**
 * @brief Adjust the memory leak thresholds for unit tests.
 *
 * Usually, unit tests will check if memory is leaked. Some functionality used by unit tests may unavoidably
 * leak memory. This is why there is a default threshold for memory leaks (currently 1200 bytes).
 * Within this range, the number of bytes leaked will be visually reported on the terminal, but no test failure will
 * be triggered. Any memory leak above the default threshold will trigger a unit test failure.
 * This function allows to adjust that memory leak threshold.
 *
 * @param leak_level Maximum allowed memory leak which will not trigger a unit test failure.
 * @param type_of_leak There are two types of leak thresholds: critical and warning. Only the
 *                      critical threshold will trigger a unit test failure if exceeded.
 * @param component Thresholds can be set in general or for specific components. Note that this argument
 *                      is not checked.
 *
 * @return ESP_OK on success, ESP_INVALID_ARG if type_of_leak is invalid. \c component is unchecked.
 */
esp_err_t test_utils_set_leak_level(size_t leak_level, esp_type_leak_t type_of_leak, esp_comp_leak_t component);

/**
 * @brief Return the memory leak thresholds for unit tests for a leak type and component.
 *
 * For more information, see \c test_utils_set_leak_level above.
 *
 * @param type_of_leak Warning or Critical
 * @param component The component for which to return the leak threshold.
 */
size_t test_utils_get_leak_level(esp_type_leak_t type_of_leak, esp_comp_leak_t component);

/**
 * @brief Start/Restart memory leak checking.
 *
 * Records the current free memory values at time of calling. After the test case, it may be checked with
 * \c test_utils_finish_and_evaluate_leaks.
 *
 * If this function is called repeatedly, only the free memory values at the last time of calling will prevail
 * as reference.
 */
void test_utils_record_free_mem(void);

/**
 * @brief Evaluate memory leak checking according to the provided thresholds.
 *
 * If the current memory leak level (counted from the last time calling \c test_utils_record_free_mem() ) exceeds
 * \c critical_threshold, a unit test failure will be triggered. If it exceeds only the warning,
 * a warning message will be issued.
 */
void test_utils_finish_and_evaluate_leaks(size_t warn_threshold, size_t critical_threshold);

/**
 * @brief Helper function to setup and initialize heap tracing.
 */
void setup_heap_record(void);

#ifdef __cplusplus
}
#endif
