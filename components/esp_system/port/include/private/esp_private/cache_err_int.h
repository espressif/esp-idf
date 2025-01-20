/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Cache error information
 */
typedef struct {
    const char* err_str;    /*!< Error message for the current panic */
    uint32_t vaddr;     /*!< Virtual address that caused the error */
    uint32_t size;      /*!< Size of the access which caused the error */
} esp_cache_err_info_t;

/**
 * @brief initialize cache invalid access interrupt
 *
 * This function enables cache invalid access interrupt source and connects it
 * to interrupt input number. It is called from the startup code.
 *
 * On ESP32, the interrupt input number is ETS_MEMACCESS_ERR_INUM. On other targets
 * it is ETS_CACHEERR_INUM. See soc/soc.h for more information.
 */
void esp_cache_err_int_init(void);

/**
 * @brief get the CPU which caused cache invalid access interrupt. Helper function in
 * panic handling.
 * @return
 *  - PRO_CPU_NUM, if PRO_CPU has caused cache IA interrupt
 *  - APP_CPU_NUM, if APP_CPU has caused cache IA interrupt
 *  - (-1) otherwise
 */
int esp_cache_err_get_cpuid(void);

/**
 * @brief Get error info for the current cache exception
 *
 * @err_info struct containing the information of the current error
 */
void esp_cache_err_get_panic_info(esp_cache_err_info_t *err_info);

/**
 * @brief Checks if any cache errors are active
 *
 * @return true
 * @return false
 */
bool esp_cache_err_has_active_err(void);

#if SOC_CACHE_ACS_INVALID_STATE_ON_PANIC
/**
 * @brief Saves and clears active access errors
 *
 * @note Some access errors needs to be cleared to allow the cache to operate again,
 *       otherwise accessing e.g rodata from flash might give invalid data
 *
 */
void esp_cache_err_acs_save_and_clr(void);

#endif //SOC_CACHE_ACS_INVALID_STATE_ON_PANIC

#ifdef __cplusplus
}
#endif
