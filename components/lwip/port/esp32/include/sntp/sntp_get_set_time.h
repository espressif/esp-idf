// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
