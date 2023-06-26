/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __SNTP_GET_SET_TIME_H__
#define __SNTP_GET_SET_TIME_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/*
 * Declarations of functions used in lwipopts.h to redefine
 * default sntp macros, such as:
 * - SNTP_UPDATE_DELAY()
 * - SNTP_SET_SYSTEM_TIME_US()
 * - SNTP_GET_SYSTEM_TIME()
 */

/*
 * @brief Get the sync interval of SNTP operation
 * Full definition is provided in IDF's layer in esp_sntp.c
 */
uint32_t sntp_get_sync_interval(void);

/**
 * @brief system time setter used in the sntp module
 * @note The lwip sntp uses u32_t types for sec and us arguments
 */
void sntp_set_system_time(uint32_t sec, uint32_t us);

/**
 * @brief system time getter used in the sntp module
 * @note The lwip sntp uses u32_t types for sec and us arguments
 */
void sntp_get_system_time(uint32_t* sec, uint32_t* us);

#ifdef __cplusplus
}
#endif

#endif //__SNTP_GET_SET_TIME_H__
