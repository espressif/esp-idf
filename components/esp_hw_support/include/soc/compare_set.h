// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/cpu.h"
#include "soc/soc_memory_layout.h"

#if __XTENSA__
#include "xtensa/xtruntime.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

static inline void __attribute__((always_inline)) compare_and_set_native(volatile uint32_t *addr, uint32_t compare, uint32_t *set)
{
#if (XCHAL_HAVE_S32C1I > 0)
    __asm__ __volatile__ (
        "WSR 	    %2,SCOMPARE1 \n"
        "S32C1I     %0, %1, 0	 \n"
        :"=r"(*set)
        :"r"(addr), "r"(compare), "0"(*set)
    );
#else
    uint32_t old_value;

#ifdef __XTENSA__
    // No S32C1I, so do this by disabling and re-enabling interrupts (slower)
    uint32_t intlevel;
    __asm__ __volatile__ ("rsil %0, " XTSTR(XCHAL_EXCM_LEVEL) "\n"
                          : "=r"(intlevel));
#else
    unsigned old_mstatus = RV_CLEAR_CSR(mstatus, MSTATUS_MIE);
#endif

    old_value = *addr;
    if (old_value == compare) {
        *addr = *set;
    }

#ifdef __XTENSA__
    __asm__ __volatile__ ("memw \n"
                          "wsr %0, ps\n"
                          :: "r"(intlevel));

#else
    RV_SET_CSR(mstatus, old_mstatus & MSTATUS_MIE);
#endif

    *set = old_value;
#endif
}


void compare_and_set_extram(volatile uint32_t *addr, uint32_t compare, uint32_t *set);

#ifdef __cplusplus
}
#endif
