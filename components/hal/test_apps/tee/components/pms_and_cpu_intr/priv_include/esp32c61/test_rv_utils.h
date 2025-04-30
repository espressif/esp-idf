/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <assert.h>

#include "esp_attr.h"

#include "soc/soc_caps.h"
#include "soc/clic_reg.h"
#include "hal/interrupt_clic_ll.h"

#include "riscv/csr.h"
#include "riscv/encoding.h"
#include "riscv/interrupt.h"
#include "esp_private/interrupt_clic.h"

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

FORCE_INLINE_ATTR void test_rv_utils_set_xtvec(uint32_t xtvec_val, int mode)
{
    xtvec_val |= MTVEC_MODE_CSR;
    if (mode == PRV_M) {
        RV_WRITE_CSR(mtvec, xtvec_val);
    } else {
        RV_WRITE_CSR(utvec, xtvec_val);
    }
}

FORCE_INLINE_ATTR void test_rv_utils_set_xtvt(uint32_t xtvt_val, int mode)
{
    if (mode == PRV_M) {
        RV_WRITE_CSR(MTVT_CSR, xtvt_val);
    } else {
        RV_WRITE_CSR(UTVT_CSR, xtvt_val);
    }
}

FORCE_INLINE_ATTR void test_rv_utils_intr_global_enable(int mode)
{
    if (mode == PRV_M) {
        RV_SET_CSR(mstatus, MSTATUS_MIE);
        RV_SET_CSR(mstatus, MSTATUS_UIE);
    } else {
        RV_SET_CSR(ustatus, USTATUS_UIE);
    }
}

FORCE_INLINE_ATTR void test_rv_utils_intr_global_disable(int mode)
{
    if (mode == PRV_M) {
        RV_CLEAR_CSR(mstatus, MSTATUS_MIE);
        RV_CLEAR_CSR(mstatus, MSTATUS_UIE);
    } else {
        RV_CLEAR_CSR(ustatus, USTATUS_UIE);
    }
}

FORCE_INLINE_ATTR void test_rv_utils_intr_enable(uint32_t intr_mask)
{
    // Machine mode
    // Disable all interrupts to make updating of the interrupt mask atomic.
    unsigned old_xstatus = RV_CLEAR_CSR(mstatus, MSTATUS_MIE);
    while (intr_mask != 0) {
        // __builtin_ffs returns one plus the index of the lsb 1-bit of x. If x is zero, returns zero
        uint32_t intr_num = __builtin_ffs(intr_mask) - 1;
        *(uint8_t volatile *)(BYTE_CLIC_INT_IE_REG(intr_num + CLIC_EXT_INTR_NUM_OFFSET)) = BYTE_CLIC_INT_IE;
        intr_mask &= (intr_mask - 1);
    }
    RV_SET_CSR(mstatus, old_xstatus & MSTATUS_MIE);
}

FORCE_INLINE_ATTR void test_rv_utils_intr_disable(uint32_t intr_mask)
{
    // Machine mode
    // Disable all interrupts to make updating of the interrupt mask atomic.
    unsigned old_xstatus = RV_CLEAR_CSR(mstatus, MSTATUS_MIE);
    while (intr_mask != 0) {
        // __builtin_ffs returns one plus the index of the lsb 1-bit of x. If x is zero, returns zero
        uint32_t intr_num = __builtin_ffs(intr_mask) - 1;
        *(uint8_t volatile *)(BYTE_CLIC_INT_IE_REG(intr_num + CLIC_EXT_INTR_NUM_OFFSET)) = 0;
        intr_mask &= (intr_mask - 1);
    }
    RV_SET_CSR(mstatus, old_xstatus & MSTATUS_MIE);
}

FORCE_INLINE_ATTR void test_rv_utils_intr_set_type(int intr_num, enum intr_type type)
{
    assert(intr_num >= 0 && intr_num < SOC_CPU_INTR_NUM);
    /* TODO: CLIC supports both rising and falling edge triggered interrupts.
     * Currently only rising edge is implemented.
     */
    volatile uint8_t *attr_reg = (volatile uint8_t *)BYTE_CLIC_INT_ATTR_REG(intr_num + CLIC_EXT_INTR_NUM_OFFSET);
    uint8_t attr = *attr_reg;
    attr &= ~BYTE_CLIC_INT_ATTR_TRIG_M;
    if (type == INTR_TYPE_EDGE) {
        attr |= (INTR_TYPE_EDGE << BYTE_CLIC_INT_ATTR_TRIG_S);
    }
    *attr_reg = attr;
}

FORCE_INLINE_ATTR void test_rv_utils_intr_set_priority(int intr_num, int priority)
{
    assert(intr_num >= 0 && intr_num < SOC_CPU_INTR_NUM);
    *(uint8_t volatile *)(BYTE_CLIC_INT_CTL_REG(intr_num + CLIC_EXT_INTR_NUM_OFFSET)) = (priority << BYTE_CLIC_INT_CTL_S);
}

FORCE_INLINE_ATTR void test_rv_utils_intr_set_threshold(int priority_threshold, int mode)
{
    uint32_t adj_threshold = ((priority_threshold << (8 - NLBITS)) | 0x1f);
    if (mode == PRV_M) {
        REG_SET_FIELD(CLIC_INT_THRESH_REG, CLIC_CPU_INT_THRESH, adj_threshold);
        RV_WRITE_CSR(MINTTHRESH_CSR, adj_threshold);
    } else {
        RV_WRITE_CSR(UINTTHRESH_CSR, adj_threshold);
    }
}

FORCE_INLINE_ATTR void test_rv_utils_intr_set_mode(int intr_num, int mode)
{
    assert(intr_num >= 0 && intr_num < SOC_CPU_INTR_NUM);
    REG_SET_FIELD(CLIC_INT_CTRL_REG(intr_num + CLIC_EXT_INTR_NUM_OFFSET), CLIC_INT_ATTR_MODE, mode);
}

FORCE_INLINE_ATTR void test_rv_utils_intr_set_vectored(int intr_num, bool vectored)
{
    REG_SET_FIELD(CLIC_INT_CTRL_REG(intr_num + CLIC_EXT_INTR_NUM_OFFSET), CLIC_INT_ATTR_SHV, vectored ? 1 : 0);
}

FORCE_INLINE_ATTR void test_rv_utils_intr_enable_u_mode(bool enable)
{
    REG_SET_FIELD(CLIC_INT_CONFIG_REG, CLIC_INT_CONFIG_NMBITS, enable ? 0x01 : 0x00);
    REG_SET_FIELD(CLIC_INT_CONFIG_REG, CLIC_INT_CONFIG_UNLBITS, NLBITS);
    REG_SET_FIELD(CLIC_INT_CONFIG_REG, CLIC_INT_CONFIG_MNLBITS, NLBITS);
}

#ifdef __cplusplus
}
#endif
