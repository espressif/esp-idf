/*
 * SPDX-FileCopyrightText: 2010-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "sdkconfig.h"
#include "esp_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

/** The content of this file is to be kept in sync with the common section of esp_memory_utils.h **/

/**
 * @brief Check if the pointer is in iram
 *
 * @param p pointer
 *
 * @return true: is in iram; false: not in iram
 */
__attribute__((always_inline))
inline static bool esp_ptr_in_iram(const void *p) {
#if CONFIG_IDF_TARGET_ESP32 && CONFIG_FREERTOS_UNICORE
    return ((intptr_t)p >= SOC_CACHE_APP_LOW && (intptr_t)p < SOC_IRAM_HIGH);
#else
    return ((intptr_t)p >= SOC_IRAM_LOW && (intptr_t)p < SOC_IRAM_HIGH);
#endif
}

/**
 * @brief Check if the pointer is in dram
 *
 * @param p pointer
 *
 * @return true: is in dram; false: not in dram
 */
__attribute__((always_inline))
inline static bool esp_ptr_in_dram(const void *p) {
    return ((intptr_t)p >= SOC_DRAM_LOW && (intptr_t)p < SOC_DRAM_HIGH);
}

/**
 * @brief Check if the pointer is in diram_dram
 *
 * @param p pointer
 *
 * @return true: is in diram_dram; false: not in diram_dram
 */
__attribute__((always_inline))
inline static bool esp_ptr_in_diram_dram(const void *p) {
    return ((intptr_t)p >= SOC_DIRAM_DRAM_LOW && (intptr_t)p < SOC_DIRAM_DRAM_HIGH);
}

/**
 * @brief Check if the pointer is in diram_iram
 *
 * @param p pointer
 *
 * @return true: is in diram_iram; false: not in diram_iram
 */
__attribute__((always_inline))
inline static bool esp_ptr_in_diram_iram(const void *p) {
    return ((intptr_t)p >= SOC_DIRAM_IRAM_LOW && (intptr_t)p < SOC_DIRAM_IRAM_HIGH);
}

/**
 * @brief Check if the pointer is in rtc_iram_fast
 *
 * @param p pointer
 *
 * @return true: is in rtc_iram_fast; false: not in rtc_iram_fast
 */
__attribute__((always_inline))
inline static bool esp_ptr_in_rtc_iram_fast(const void *p) {
#if SOC_RTC_FAST_MEM_SUPPORTED
    return ((intptr_t)p >= SOC_RTC_IRAM_LOW && (intptr_t)p < SOC_RTC_IRAM_HIGH);
#else
    return false;
#endif
}

/**
 * @brief Check if the pointer is in rtc_dram_fast
 *
 * @param p pointer
 *
 * @return true: is in rtc_dram_fast; false: not in rtc_dram_fast
 */
__attribute__((always_inline))
inline static bool esp_ptr_in_rtc_dram_fast(const void *p) {
#if SOC_RTC_FAST_MEM_SUPPORTED
    return ((intptr_t)p >= SOC_RTC_DRAM_LOW && (intptr_t)p < SOC_RTC_DRAM_HIGH);
#else
    return false;
#endif
}

/**
 * @brief Check if the pointer is in rtc_slow
 *
 * @param p pointer
 *
 * @return true: is in rtc_slow; false: not in rtc_slow
 */
__attribute__((always_inline))
inline static bool esp_ptr_in_rtc_slow(const void *p) {
#if SOC_RTC_SLOW_MEM_SUPPORTED
    return ((intptr_t)p >= SOC_RTC_DATA_LOW && (intptr_t)p < SOC_RTC_DATA_HIGH);
#else
    return false;
#endif
}


/* Convert a D/IRAM DRAM pointer to equivalent word address in IRAM

   - Address must be word aligned
   - Address must pass esp_ptr_in_diram_dram() test, or result will be invalid pointer
*/
__attribute__((always_inline))
inline static void * esp_ptr_diram_dram_to_iram(const void *p) {
#if SOC_DIRAM_INVERTED
    return (void *) ( SOC_DIRAM_IRAM_LOW + (SOC_DIRAM_DRAM_HIGH - (intptr_t)p) - 4);
#else
    return (void *) ( SOC_DIRAM_IRAM_LOW + ((intptr_t)p - SOC_DIRAM_DRAM_LOW) );
#endif
}

/* Convert a D/IRAM IRAM pointer to equivalent word address in DRAM

   - Address must be word aligned
   - Address must pass esp_ptr_in_diram_iram() test, or result will be invalid pointer
*/
__attribute__((always_inline))
inline static void * esp_ptr_diram_iram_to_dram(const void *p) {
#if SOC_DIRAM_INVERTED
    return (void *) ( SOC_DIRAM_DRAM_LOW + (SOC_DIRAM_IRAM_HIGH - (intptr_t)p) - 4);
#else
    return (void *) ( SOC_DIRAM_DRAM_LOW + ((intptr_t)p - SOC_DIRAM_IRAM_LOW) );
#endif
}

/** End of the common section that has to be in sync with esp_memory_utils.h **/
/** Don't add new functions below **/

#ifdef __cplusplus
}
#endif
