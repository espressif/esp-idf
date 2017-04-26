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

/* C macros for xtensa special register read/write/exchange */

#define RSR(reg, curval)  asm volatile ("rsr %0, " #reg : "=r" (curval));
#define WSR(reg, newval)  asm volatile ("wsr %0, " #reg : : "r" (newval));
#define XSR(reg, swapval) asm volatile ("xsr %0, " #reg : "+r" (swapval));

/** @brief Read current stack pointer address
 *
 */
static inline void *get_sp()
{
    void *sp;
    asm volatile ("mov %0, sp;" : "=r" (sp));
    return sp;
}

/* Functions to set page attributes for Region Protection option in the CPU.
 * See Xtensa ISA Reference manual for explanation of arguments (section 4.6.3.2).
 */

static inline void cpu_write_dtlb(uint32_t vpn, unsigned attr)
{
    asm volatile ("wdtlb  %1, %0; dsync\n" :: "r" (vpn), "r" (attr));
}


static inline void cpu_write_itlb(unsigned vpn, unsigned attr)
{
    asm volatile ("witlb  %1, %0; isync\n" :: "r" (vpn), "r" (attr));
}

/**
 * @brief Configure memory region protection
 *
 * Make page 0 access raise an exception.
 * Also protect some other unused pages so we can catch weirdness.
 * Useful attribute values:
 * 0 — cached, RW
 * 2 — bypass cache, RWX (default value after CPU reset)
 * 15 — no access, raise exception
 */

static inline void cpu_configure_region_protection()
{
    const uint32_t pages_to_protect[] = {0x00000000, 0x80000000, 0xa0000000, 0xc0000000, 0xe0000000};
    for (int i = 0; i < sizeof(pages_to_protect)/sizeof(pages_to_protect[0]); ++i) {
        cpu_write_dtlb(pages_to_protect[i], 0xf);
        cpu_write_itlb(pages_to_protect[i], 0xf);
    }
    cpu_write_dtlb(0x20000000, 0);
    cpu_write_itlb(0x20000000, 0);
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
 * @brief Returns true if a JTAG debugger is attached to CPU
 * OCD (on chip debug) port.
 *
 * @note If "Make exception and panic handlers JTAG/OCD aware"
 * is disabled, this function always returns false.
 */
bool esp_cpu_in_ocd_debug_mode();

#endif
