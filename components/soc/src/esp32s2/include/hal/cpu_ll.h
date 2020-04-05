// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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
#pragma once
#include <stdint.h>

#include "esp_attr.h"

#include "soc/cpu_caps.h"

#include "xt_instr_macros.h"
#include "xtensa/config/specreg.h"
#include "xtensa/config/extreg.h"
#include "esp_bit_defs.h"
#include "xtensa/config/core.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline int IRAM_ATTR cpu_ll_get_core_id(void)
{
    return 0;
}

static inline uint32_t cpu_ll_get_cycle_count(void)
{
    uint32_t result;
    RSR(CCOUNT, result);
    return result;
}

static inline void* cpu_ll_get_sp(void)
{
    void *sp;
    asm volatile ("mov %0, sp;" : "=r" (sp));
    return sp;
}

static inline void cpu_ll_init_hwloop(void)
{
#if XCHAL_ERRATUM_572
    uint32_t memctl = XCHAL_CACHE_MEMCTL_DEFAULT;
    WSR(MEMCTL, memctl);
#endif // XCHAL_ERRATUM_572
}

static inline void cpu_ll_set_breakpoint(int id, uint32_t pc)
{
    uint32_t en;

    // Set the break address register to the appropriate PC
    if (id) {
        WSR(IBREAKA_1, pc);
    } else {
        WSR(IBREAKA_0, pc);
    }

    // Enable the breakpoint using the break enable register
    RSR(IBREAKENABLE, en);
    en |= BIT(id);
    WSR(IBREAKENABLE, en);
}

static inline void cpu_ll_clear_breakpoint(int id)
{
    uint32_t en = 0;
    uint32_t pc = 0;

    // Set the break address register to the appropriate PC
    if (id) {
        WSR(IBREAKA_1, pc);
    } else {
        WSR(IBREAKA_0, pc);
    }

    // Enable the breakpoint using the break enable register
    RSR(IBREAKENABLE, en);
    en &= ~BIT(id);
    WSR(IBREAKENABLE, en);
}

static inline uint32_t cpu_ll_ptr_to_pc(const void* addr)
{
    return ((uint32_t) addr);
}

static inline void* cpu_ll_pc_to_ptr(uint32_t pc)
{
    return (void*) ((pc & 0x3fffffff) | 0x40000000);
}

static inline void cpu_ll_set_watchpoint(int id,
                                        const void* addr,
                                        size_t size,
                                        bool on_read,
                                        bool on_write)
{
    uint32_t dbreakc = 0x3F;

    //We support watching 2^n byte values, from 1 to 64. Calculate the mask for that.
    for (int x = 0; x < 7; x++) {
        if (size == (1 << x)) {
            break;
        }
        dbreakc <<= 1;
    }

    dbreakc = (dbreakc & 0x3F);

    if (on_read) {
        dbreakc |= BIT(30);
    }

    if (on_write) {
        dbreakc |= BIT(31);
    }

    // Write the break address register and the size to control
    // register.
    if (id) {
        WSR(DBREAKA_1, (uint32_t) addr);
        WSR(DBREAKC_1, dbreakc);
    } else {
        WSR(DBREAKA_0, (uint32_t) addr);
        WSR(DBREAKC_0, dbreakc);
    }
}

static inline void cpu_ll_clear_watchpoint(int id)
{
    // Clear both break address register and control register
    if (id) {
        WSR(DBREAKA_1, 0);
        WSR(DBREAKC_1, 0);
    } else {
        WSR(DBREAKA_0, 0);
        WSR(DBREAKC_0, 0);
    }
}

static inline bool cpu_ll_is_debugger_attached(void)
{
    uint32_t dcr = 0;
    uint32_t reg = DSRSET;
    RER(reg, dcr);
    return (dcr&0x1);
}


static inline void cpu_ll_break(void)
{
    __asm__ ("break 0,0");
}

static inline void cpu_ll_set_vecbase(const void* vecbase)
{
    asm volatile ("wsr %0, vecbase" :: "r" (vecbase));
}

#ifdef __cplusplus
}
#endif
