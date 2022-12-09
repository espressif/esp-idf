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

#ifndef __COMPARE_SET_H
#define __COMPARE_SET_H

#include <stdint.h>
#include <stdbool.h>
#include "soc/cpu.h"
#include "soc/soc_memory_layout.h"
#include "xtensa/xtruntime.h"


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
    // No S32C1I, so do this by disabling and re-enabling interrupts (slower)
    uint32_t intlevel, old_value;
    __asm__ __volatile__ ("rsil %0, " XTSTR(XCHAL_EXCM_LEVEL) "\n"
                          : "=r"(intlevel));

    old_value = *addr;
    if (old_value == compare) {
        *addr = *set;
    }

    __asm__ __volatile__ ("memw \n"
                          "wsr %0, ps\n"
                          :: "r"(intlevel));

    *set = old_value;
#endif
}


void compare_and_set_extram(volatile uint32_t *addr, uint32_t compare, uint32_t *set);

#endif