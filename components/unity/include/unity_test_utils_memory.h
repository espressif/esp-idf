/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Adjust the allowed memory leak thresholds for unit tests.
 *
 * Usually, unit tests will check if memory is leaked. Some functionality used by unit tests may unavoidably
 * leak memory. This function allows to adjust that memory leak threshold.
 *
 * @param leak_level Maximum allowed memory leak which will not trigger a unit test failure.
 */
void unity_utils_set_leak_level(size_t leak_level);

/**
 * @brief Start/Restart memory leak checking.
 *
 * Records the current free memory values at time of calling. After the test case, it may be checked with
 * \c unity_utils_finish_and_evaluate_leaks.
 *
 * If this function is called repeatedly, only the free memory values at the last time of calling will prevail
 * as reference.
 */
void unity_utils_record_free_mem(void);

/**
 * @brief Calculate leaks and check they are below against a threshold
 *
 * This function is for internal use, users shouldn't have a reason to call this.
 *
 * Calculates the leak from \c before_free and \c after_free and checks that the difference does not exceed
 * \c threshold. It uses a unity assert to to the check and report in case of failure.
 * A summary of the leaked data will be printed in all cases.
 */
void unity_utils_check_leak(unsigned int before_free,
        unsigned int after_free,
        const char *type,
        unsigned int threshold);

/**
 * @brief Evaluate memory leak checking according to the provided thresholds.
 *
 * If the current memory leak level (counted from the last time calling \c unity_utils_record_free_mem() ) exceeds
 * \c threshold, a unit test failure will be triggered.
 */
void unity_utils_evaluate_leaks_direct(size_t threshold);

/**
 * @brief Evaluate memory leaks.
 *
 * If the current memory leak level (counted from the last time calling \c unity_utils_record_free_mem() ) exceeds
 * the threshold set before via \c unity_utils_set_leak_level(), a unit test failure will be triggered.
 *
 * @note The user MUST set the allowed leak threshold before via \c unity_utils_set_leak_level(), otherwise the
 *       allowed leak threshold is undefined.
 */
void unity_utils_evaluate_leaks(void);

/**
 * @brief Helper function to setup and initialize heap tracing.
 *
 * @param num_heap_records the size of the heap record buffer,
 *        counted in number of heap record elements (heap_trace_record_t).
 *        Use a default value of 80 if no special requirements need to be met.
 */
void unity_utils_setup_heap_record(size_t num_heap_records);

#ifdef __cplusplus
}
#endif
