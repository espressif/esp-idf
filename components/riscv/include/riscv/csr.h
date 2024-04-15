// Copyright (c) 2013, The Regents of the University of California (Regents).
// Copyright (c) 2018-2019, The libfemto authors
// Copyright (c) 2020 Espressif Systems (Shanghai) PTE LTD
// All Rights Reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. Neither the name of the Regents nor the
//    names of its contributors may be used to endorse or promote products
//    derived from this software without specific prior written permission.

// IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
// SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
// OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF REGENTS HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
// HEREUNDER IS PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE
// MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/param.h>
#include "encoding.h"
#include "esp_assert.h"

/********************************************************
 Physical Memory Attributes (PMA) register fields
 (privileged spec)
 ********************************************************/

/********************************************************
   PMA CSR and TOR & NAPOT macros
 ********************************************************/
#define CSR_PMACFG0  0xBC0
#define CSR_PMAADDR0 0xBD0

#define CSR_PMACFG(i)  (CSR_PMACFG0 + (i))
#define CSR_PMAADDR(i)  (CSR_PMAADDR0 + (i))

#define PMA_EN    BIT(0)
#define PMA_R     BIT(4)
#define PMA_W     BIT(3)
#define PMA_X     BIT(2)
#define PMA_L     BIT(29)
#define PMA_SHIFT 2

#define PMA_TOR   0x40000000
#define PMA_NA4   0x80000000
#define PMA_NAPOT 0xC0000000

#define PMA_NONCACHEABLE     BIT(27)
#define PMA_WRITETHROUGH     BIT(26)
#define PMA_WRITEMISSNOALLOC BIT(25)
#define PMA_READMISSNOALLOC  BIT(24)

#define PMA_ENTRY_SET_TOR(ENTRY, ADDR, CFG)                            \
    do {                                                               \
        RV_WRITE_CSR((CSR_PMAADDR0) + (ENTRY), (ADDR) >> (PMA_SHIFT)); \
        RV_WRITE_CSR((CSR_PMACFG0) + (ENTRY), CFG);                    \
    } while (0)

#define PMA_ENTRY_SET_NAPOT(ENTRY, ADDR, SIZE, CFG)                                \
    do {                                                                           \
        ESP_STATIC_ASSERT(__builtin_popcount((SIZE)) == 1, "Size must be a power of 2"); \
        ESP_STATIC_ASSERT((ADDR) % ((SIZE)) == 0, "Addr must be aligned to size"); \
        RV_WRITE_CSR((CSR_PMAADDR0) + (ENTRY), ((ADDR) | (((SIZE) >> 1) - 1)) >> 2); \
        RV_WRITE_CSR((CSR_PMACFG0) + (ENTRY), CFG);                                \
    } while (0)

/********************************************************
 Physical Memory Protection (PMP) register fields
 (privileged spec)
 ********************************************************/

/* Value of pmpcfg0 CSR. Note this macro is only needed for calculations like (CSR_PMPCFG0 + 1), which must
   still be constant at compile time. Use the assembler name pmpcfg0, pmpcfg1, pmpcfg2, etc. in other cases */
#define CSR_PMPCFG0 0x3A0

/* Value of pmpaddr0 CSR. Note, as above, this macro is only needed for calculations and the assembler names
   pmpaddr0, pmpaddr1, pmpaddr2, etc should be used otherwise. */
#define CSR_PMPADDR0 0x3B0

/* Generate the PMP address field value when PMPCFG.A == NAPOT

   START & END should be calculated at compile time. The size of the region
   (END-START) must be a power of 2 size, and START must be aligned to this
   size.

   Note: this value must be right-shifted PMP_SHIFT when written to the PMPADDR
   register. The PMP_ENTRY_SET macro will do this.
 */
#define PMPADDR_NAPOT(START, END) ({                                    \
            ESP_STATIC_ASSERT(__builtin_popcount((END)-(START)) == 1, "Size must be a power of 2"); \
            ESP_STATIC_ASSERT((START) % ((END)-(START)) == 0, "Start must be aligned to size"); \
            (((START)) | (((END)-(START)-1)>>1));                       \
        })

#define PMPADDR_ALL 0xFFFFFFFF


/* Set a PMP entry.

   - ENTRY is number of the PMP entry to set. This must be a compile-time constant because it's used to
     generate specific assembly instructions.
   - ADDR is the address to write to the PMPADDRx register. Note this is the unshifted address.
   - CFG is the configuration value to write to the correct CFG entry register. Note that
     the macro only sets bits in the CFG register, so it should be zeroed already.
*/
#define PMP_ENTRY_SET(ENTRY, ADDR, CFG) do {  \
    RV_WRITE_CSR((CSR_PMPADDR0) + (ENTRY), (ADDR) >> (PMP_SHIFT));    \
    RV_SET_CSR((CSR_PMPCFG0) + (ENTRY)/4, ((CFG)&0xFF) << (ENTRY%4)*8); \
    } while(0)

/*Only set PMPCFG entries*/
#define PMP_ENTRY_CFG_SET(ENTRY, CFG) do {\
    RV_SET_CSR((CSR_PMPCFG0) + (ENTRY)/4, ((CFG)&0xFF) << (ENTRY%4)*8); \
    } while(0)

/*Reset all permissions of a particular PMPCFG entry*/
#define PMP_ENTRY_CFG_RESET(ENTRY) do {\
    RV_CLEAR_CSR((CSR_PMPCFG0) + (ENTRY)/4, (0xFF) << (ENTRY%4)*8); \
    } while(0)

/********************************************************
   Trigger Module register fields (Debug specification)
 ********************************************************/

#define TCONTROL_MTE     (1<<3)    /*R/W, Current M mode trigger enable bit*/
#define TCONTROL_MPTE    (1<<7)    /*R/W, Previous M mode trigger enable bit*/

#define TDATA1_LOAD      (1<<0)  /*R/W,Fire trigger on load address match*/
#define TDATA1_STORE     (1<<1)  /*R/W,Fire trigger on store address mat*/
#define TDATA1_EXECUTE   (1<<2)  /*R/W,Fire trigger on instruction fetch address match*/
#define TDATA1_USER      (1<<3)  /*R/W,allow trigger to be fired in user mode*/
#define TDATA1_MACHINE   (1<<6)  /*R/W,Allow trigger to be fired while hart is executing in machine mode*/
#define TDATA1_MATCH_EXACT  (0)
#define TDATA1_MATCH_NAPOT  (1<<7)
#define TDATA1_MATCH_V   (0xF)   /*R/W,Address match type :0 : Exact byte match  1 : NAPOT range match */
#define TDATA1_MATCH_S   (7)
#define TDATA1_HIT_S     (20)


/********************************************************
   Espressif's bus error exceptions registers and fields
 ********************************************************/

#define MEXSTATUS   0x7E1
#define MHINT       0x7C5

#define LDPC0       0xBE0
#define LDPC1       0xBE1

#define STPC0       0xBF0
#define STPC1       0xBF1
#define STPC2       0xBF2

/* RISC-V CSR macros
 * Adapted from https://github.com/michaeljclark/riscv-probe/blob/master/libfemto/include/arch/riscv/machine.h
 */

#define RV_READ_CONST_CSR(reg) ({ unsigned long __tmp; \
  asm ("csrr %0, " _CSR_STRINGIFY(reg) : "=r"(__tmp)); __tmp; })

#define RV_READ_CSR(reg) ({ unsigned long __tmp; \
  asm volatile ("csrr %0, " _CSR_STRINGIFY(reg) : "=r"(__tmp)); __tmp; })

#define RV_WRITE_CSR(reg, val) ({ \
  asm volatile ("csrw " _CSR_STRINGIFY(reg) ", %0" :: "rK"(val)); })

#define RV_SWAP_CSR(reg, val) ({ unsigned long __tmp; \
  asm volatile ("csrrw %0, " _CSR_STRINGIFY(reg) ", %1" : "=r"(__tmp) : "rK"(val)); __tmp; })

/* Note: this uses the atomic read-and-set instruction so possible to read the old CSR value as a result */
#define RV_SET_CSR(reg, bit) ({ unsigned long __tmp; \
  asm volatile ("csrrs %0, " _CSR_STRINGIFY(reg) ", %1" : "=r"(__tmp) : "rK"(bit)); __tmp; })

/* Note: this uses the atomic read-and-clear instruction so possible to read the old CSR value as a result */
#define RV_CLEAR_CSR(reg, bit) ({ unsigned long __tmp; \
  asm volatile ("csrrc %0, " _CSR_STRINGIFY(reg) ", %1" : "=r"(__tmp) : "rK"(bit)); __tmp; })

#define RV_SET_CSR_FIELD(_r, _f, _v) ({ (RV_WRITE_CSR((_r),((RV_READ_CSR(_r) & ~((_f##_V) << (_f##_S)))|(((_v) & (_f##_V))<<(_f##_S)))));})
#define RV_CLEAR_CSR_FIELD(_r, _f) ({ (RV_WRITE_CSR((_r),(RV_READ_CSR(_r) & ~((_f##_V) << (_f##_S)))));})

#define RV_READ_MSTATUS_AND_DISABLE_INTR() ({ unsigned long __tmp; \
  asm volatile ("csrrci %0, mstatus, 0x8"  : "=r"(__tmp)); __tmp; })

#define _CSR_STRINGIFY(REG) #REG /* needed so the 'reg' argument can be a macro or a register name */

#ifdef __cplusplus
}
#endif
