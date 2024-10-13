/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/soc_caps.h"
#include "xtensa/config/core-isa.h"
#include "xtensa/config/core.h"
#include "xtensa/config/extreg.h"
#include "xtensa/config/specreg.h"
#include "xtensa/xtruntime.h"
#include "xt_instr_macros.h"
#include "esp_bit_defs.h"
#include "esp_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------- CPU Registers ----------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

FORCE_INLINE_ATTR __attribute__((pure)) uint32_t xt_utils_get_core_id(void)
{
    /*
    Note: We depend on SOC_CPU_CORES_NUM instead of XCHAL_HAVE_PRID as some single Xtensa targets (such as ESP32-S2) have
    the PRID register even though they are single core.
    */
#if SOC_CPU_CORES_NUM > 1
    // Read and extract bit 13 of special register PRID
    uint32_t id;
    asm volatile (
        "rsr.prid %0\n"
        "extui %0,%0,13,1"
        :"=r"(id));
    return id;
#else
    return 0;
#endif // SOC_CPU_CORES_NUM > 1
}

FORCE_INLINE_ATTR __attribute__((pure)) uint32_t xt_utils_get_raw_core_id(void)
{
#if XCHAL_HAVE_PRID
    // Read the raw value of special register PRID
    uint32_t id;
    asm volatile (
        "rsr.prid %0\n"
        :"=r"(id));
    return id;
#else
    return 0;
#endif // XCHAL_HAVE_PRID
}

FORCE_INLINE_ATTR void *xt_utils_get_sp(void)
{
    void *sp;
    asm volatile ("mov %0, sp;" : "=r" (sp));
    return sp;
}

FORCE_INLINE_ATTR uint32_t xt_utils_get_cycle_count(void)
{
    uint32_t ccount;
    RSR(CCOUNT, ccount);
    return ccount;
}

static inline void xt_utils_set_cycle_count(uint32_t ccount)
{
    WSR(CCOUNT, ccount);
}

FORCE_INLINE_ATTR void xt_utils_wait_for_intr(void)
{
    asm volatile ("waiti 0\n");
}

/* ------------------------------------------------- CPU Interrupts ----------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

// ---------------- Interrupt Descriptors ------------------

// --------------- Interrupt Configuration -----------------

FORCE_INLINE_ATTR void xt_utils_set_vecbase(uint32_t vecbase)
{
    asm volatile ("wsr %0, vecbase" :: "r" (vecbase));
}

// ------------------ Interrupt Control --------------------

FORCE_INLINE_ATTR uint32_t xt_utils_intr_get_enabled_mask(void)
{
    uint32_t intr_mask;
    RSR(INTENABLE, intr_mask);
    return intr_mask;
}

/* -------------------------------------------------- Memory Ports -----------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

/* ---------------------------------------------------- Debugging ------------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

// --------------- Breakpoints/Watchpoints -----------------

FORCE_INLINE_ATTR void xt_utils_set_breakpoint(int bp_num, uint32_t bp_addr)
{
    //Set the breakpoint's address
    if (bp_num == 1) {
        WSR(IBREAKA_1, bp_addr);
    } else {
        WSR(IBREAKA_0, bp_addr);
    }
    //Enable the breakpoint
    uint32_t brk_ena_reg;
    RSR(IBREAKENABLE, brk_ena_reg);
    brk_ena_reg |= BIT(bp_num);
    WSR(IBREAKENABLE, brk_ena_reg);
}

FORCE_INLINE_ATTR void xt_utils_clear_breakpoint(int bp_num)
{
    // Disable the breakpoint using the break enable register
    uint32_t bp_en = 0;
    RSR(IBREAKENABLE, bp_en);
    bp_en &= ~BIT(bp_num);
    WSR(IBREAKENABLE, bp_en);
    // Zero the break address register
    uint32_t bp_addr = 0;
    if (bp_num == 1) {
        WSR(IBREAKA_1, bp_addr);
    } else {
        WSR(IBREAKA_0, bp_addr);
    }
}

FORCE_INLINE_ATTR void xt_utils_set_watchpoint(int wp_num,
                                               uint32_t wp_addr,
                                               size_t size,
                                               bool on_read,
                                               bool on_write)
{
    // Initialize DBREAKC bits (see Table 4–143 or isa_rm.pdf)
    uint32_t dbreakc_reg = 0x3F;
    dbreakc_reg = dbreakc_reg << (__builtin_ffsll(size) - 1);
    dbreakc_reg = dbreakc_reg & 0x3F;
    if (on_read) {
        dbreakc_reg |= BIT(30);
    }
    if (on_write) {
        dbreakc_reg |= BIT(31);
    }
    // Enable break address and break control register
    if (wp_num == 1) {
        WSR(DBREAKA_1, (uint32_t) wp_addr);
        WSR(DBREAKC_1, dbreakc_reg);
    } else {
        WSR(DBREAKA_0, (uint32_t) wp_addr);
        WSR(DBREAKC_0, dbreakc_reg);
    }
}

FORCE_INLINE_ATTR void xt_utils_clear_watchpoint(int wp_num)
{
    // Clear both break control and break address register
    if (wp_num == 1) {
        WSR(DBREAKC_1, 0);
        WSR(DBREAKA_1, 0);
    } else {
        WSR(DBREAKC_0, 0);
        WSR(DBREAKA_0, 0);
    }
}

// ---------------------- Debugger -------------------------

FORCE_INLINE_ATTR bool xt_utils_dbgr_is_attached(void)
{
    uint32_t dcr = 0;
    uint32_t reg = DSRSET;
    RER(reg, dcr);
    return (bool)(dcr & 0x1);
}

FORCE_INLINE_ATTR void xt_utils_dbgr_break(void)
{
    __asm__ ("break 1,15");
}

/* ------------------------------------------------------ Misc ---------------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

FORCE_INLINE_ATTR bool xt_utils_compare_and_set(volatile uint32_t *addr, uint32_t compare_value, uint32_t new_value)
{
#if XCHAL_HAVE_S32C1I
#ifdef __clang_analyzer__
    //Teach clang-tidy that "addr" cannot be const as it can be updated by S32C1I instruction
    volatile uint32_t temp;
    temp = *addr;
    *addr = temp;
#endif
    // Atomic compare and set using S32C1I instruction
    uint32_t old_value = new_value;
    __asm__ __volatile__ (
        "WSR    %2, SCOMPARE1 \n"
        "S32C1I %0, %1, 0 \n"
        :"=r"(old_value)
        :"r"(addr), "r"(compare_value), "0"(old_value)
    );

    return (old_value == compare_value);
#else // XCHAL_HAVE_S32C1I
    // Single core target has no atomic CAS instruction. We can achieve atomicity by disabling interrupts
    uint32_t intr_level;
    __asm__ __volatile__ ("rsil %0, " XTSTR(XCHAL_EXCM_LEVEL) "\n"
                          : "=r"(intr_level));
    // Compare and set
    uint32_t old_value;
    old_value = *addr;
    if (old_value == compare_value) {
        *addr = new_value;
    }
    // Restore interrupts
    __asm__ __volatile__ ("memw \n"
                          "wsr %0, ps\n"
                          :: "r"(intr_level));

    return (old_value == compare_value);
#endif // XCHAL_HAVE_S32C1I
}

#ifdef __cplusplus
}
#endif
