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

#ifndef _SOC_CPU_H
#define _SOC_CPU_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "xtensa/corebits.h"
#include "xtensa/config/core.h"

#include "xtensa/config/specreg.h"
#include "xt_instr_macros.h"

#include "hal/cpu_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Read current stack pointer address
 *
 */
static inline void *get_sp(void)
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

/**
 * @brief Convert the PC register value to its true address
 *
 * The address of the current instruction is not stored as an exact uint32_t
 * representation in PC register. This function will convert the value stored in
 * the PC register to a uint32_t address.
 *
 * @param pc_raw The PC as stored in register format.
 *
 * @return Address in uint32_t format
 */
static inline uint32_t esp_cpu_process_stack_pc(uint32_t pc)
{
    if (pc & 0x80000000) {
        //Top two bits of a0 (return address) specify window increment. Overwrite to map to address space.
        pc = (pc & 0x3fffffff) | 0x40000000;
    }
    //Minus 3 to get PC of previous instruction (i.e. instruction executed before return address)
    return pc - 3;
}

typedef uint32_t esp_cpu_ccount_t;

static inline esp_cpu_ccount_t esp_cpu_get_ccount(void)
{
    uint32_t result; 
    RSR(CCOUNT, result);
    return result;
}

#ifdef __cplusplus
}
#endif

#endif
