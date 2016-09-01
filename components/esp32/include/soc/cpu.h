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

#include "xtensa/corebits.h"

/* C macros for xtensa special register read/write/exchange */

#define RSR(reg, curval)  asm volatile ("rsr %0, " #reg : "=r" (curval));
#define WSR(reg, newval)  asm volatile ("wsr %0, " #reg : : "r" (newval));
#define XSR(reg, swapval) asm volatile ("xsr %0, " #reg : "+r" (swapval));

/* Return true if the CPU is in an interrupt context
   (PS.UM == 0)
*/
static inline bool cpu_in_interrupt_context(void)
{
    uint32_t ps;
    RSR(PS, ps);
    return (ps & PS_UM) == 0;
}

#endif
