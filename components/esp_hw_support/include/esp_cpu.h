// Copyright 2010-2016 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _ESP_CPU_H
#define _ESP_CPU_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "hal/cpu_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_WATCHPOINT_LOAD 0x40000000
#define ESP_WATCHPOINT_STORE 0x80000000
#define ESP_WATCHPOINT_ACCESS 0xC0000000

typedef uint32_t esp_cpu_ccount_t;

/** @brief Read current stack pointer address
 *
 */
static inline void *esp_cpu_get_sp(void)
{
    return cpu_hal_get_sp();
}

/**
 * @brief Stall CPU using RTC controller
 * @param cpu_id ID of the CPU to stall (0 = PRO, 1 = APP)
 */
void esp_cpu_stall(int cpu_id);

/**
 * @brief Un-stall CPU using RTC controller
 * @param cpu_id ID of the CPU to un-stall (0 = PRO, 1 = APP)
 */
void esp_cpu_unstall(int cpu_id);

/**
 * @brief Reset CPU using RTC controller
 * @param cpu_id ID of the CPU to reset (0 = PRO, 1 = APP)
 */
void esp_cpu_reset(int cpu_id);

/**
 * @brief Returns true if a JTAG debugger is attached to CPU
 * OCD (on chip debug) port.
 *
 * @note If "Make exception and panic handlers JTAG/OCD aware"
 * is disabled, this function always returns false.
 */
bool esp_cpu_in_ocd_debug_mode(void);

static inline esp_cpu_ccount_t esp_cpu_get_ccount(void)
{
    return cpu_hal_get_cycle_count();
}

static inline void esp_cpu_set_ccount(esp_cpu_ccount_t val)
{
    cpu_hal_set_cycle_count(val);
}

/**
 * @brief Set a watchpoint to break/panic when a certain memory range is accessed.
 *
 * @param no Watchpoint number. On the ESP32, this can be 0 or 1.
 * @param adr Base address to watch
 * @param size Size of the region, starting at the base address, to watch. Must
 *             be one of 2^n, with n in [0..6].
 * @param flags One of ESP_WATCHPOINT_* flags
 *
 * @return ESP_ERR_INVALID_ARG on invalid arg, ESP_OK otherwise
 *
 * @warning The ESP32 watchpoint hardware watches a region of bytes by effectively
 *          masking away the lower n bits for a region with size 2^n. If adr does
 *          not have zero for these lower n bits, you may not be watching the
 *          region you intended.
 */
esp_err_t esp_cpu_set_watchpoint(int no, void *adr, int size, int flags);

/**
 * @brief Clear a watchpoint
 *
 * @param no Watchpoint to clear
 *
 */
void esp_cpu_clear_watchpoint(int no);

#ifdef __cplusplus
}
#endif

#endif // _ESP_CPU_H
