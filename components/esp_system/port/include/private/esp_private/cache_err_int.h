/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

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

#ifdef __cplusplus
}
#endif
