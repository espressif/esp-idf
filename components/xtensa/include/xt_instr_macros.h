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

#define RSR(reg, at)         asm volatile ("rsr %0, %1" : "=r" (at) : "i" (reg))
#define WSR(reg, at)         asm volatile ("wsr %0, %1" : : "r" (at), "i" (reg))
#define XSR(reg, at)         asm volatile ("xsr %0, %1" : "+r" (at) : "i" (reg))

#define RER(reg, at)         asm volatile ("rer %0, %1" : "=r" (at) : "r" (reg))

#define WITLB(at, as)        asm volatile ("witlb  %0, %1; \n isync \n " : : "r" (at), "r" (as))
#define WDTLB(at, as)        asm volatile ("wdtlb  %0, %1; \n dsync \n " : : "r" (at), "r" (as))

/* The SET_STACK implements a setting a new stack pointer (sp or a1).
 * to do this the need reset PS_WOE, reset WINDOWSTART, update SP, and return PS_WOE.
 *
 * Note: It has 2 implementations one for using in assembler files (*.S) and one for using in C.
 *
 * C code prototype for SET_STACK:
 *   uint32_t ps_reg;
 *   uint32_t w_base;
 *   RSR(PS, ps_reg);
 *   ps_reg &= ~(PS_WOE_MASK | PS_OWB_MASK | PS_CALLINC_MASK);
 *   WSR(PS, ps_reg);
 *
 *   RSR(WINDOWBASE, w_base);
 *   WSR(WINDOWSTART, (1 << w_base));
 *
 *   asm volatile ( "movi sp, "XTSTR( (SOC_DRAM_LOW + (SOC_DRAM_HIGH - SOC_DRAM_LOW) / 2) )"");
 *
 *   RSR(PS, ps_reg);
 *   ps_reg |= (PS_WOE_MASK);
 *   WSR(PS, ps_reg);
*/
#ifdef __ASSEMBLER__
    .macro SET_STACK  new_sp tmp1 tmp2
    rsr.ps \tmp1
    movi \tmp2, ~(PS_WOE_MASK | PS_OWB_MASK | PS_CALLINC_MASK)
    and \tmp1, \tmp1, \tmp2
    wsr.ps \tmp1
    rsync

    rsr.windowbase \tmp1
    ssl	\tmp1
    movi \tmp1, 1
    sll	\tmp1, \tmp1
    wsr.windowstart \tmp1
    rsync

    mov sp, \new_sp

    rsr.ps \tmp1
    movi \tmp2, (PS_WOE)
    or \tmp1, \tmp1, \tmp2
    wsr.ps \tmp1
    rsync
    .endm
#else
#define SET_STACK(new_sp) \
    do { \
        uint32_t tmp1 = 0, tmp2 = 0; \
        asm volatile ( \
          "rsr.ps %1 \n"\
          "movi %2, ~" XTSTR( PS_WOE_MASK | PS_OWB_MASK | PS_CALLINC_MASK ) " \n"\
          "and %1, %1, %2 \n"\
          "wsr.ps %1 \n"\
          "rsync \n"\
          " \n"\
          "rsr.windowbase %1 \n"\
          "ssl	%1 \n"\
          "movi %1, 1 \n"\
          "sll	%1, %1 \n"\
          "wsr.windowstart %1 \n"\
          "rsync \n"\
          " \n"\
          "mov sp, %0 \n"\
          "rsr.ps %1 \n"\
          " \n"\
          "movi %2, " XTSTR( PS_WOE_MASK ) "\n"\
          " \n"\
          "or %1, %1, %2 \n"\
          "wsr.ps %1 \n"\
          "rsync \n"\
          : "+r"(new_sp), "+r"(tmp1), "+r"(tmp2)); \
    } while (0);
#endif // __ASSEMBLER__
