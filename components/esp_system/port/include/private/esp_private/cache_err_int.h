/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

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
 * @brief Returns a pointer to the cache error message
 *
 * @return const char* Pointer to the error message
 */
const char *esp_cache_err_panic_string(void);

/**
 * @brief Checks if any cache errors are active
 *
 * @return true
 * @return false
 */
bool esp_cache_err_has_active_err(void);

#ifdef __cplusplus
}
#endif
