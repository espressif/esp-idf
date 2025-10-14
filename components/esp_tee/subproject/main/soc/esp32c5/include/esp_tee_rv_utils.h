/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <assert.h>

#include "riscv/csr.h"
#include "riscv/interrupt.h"

#include "hal/interrupt_clic_ll.h"
#include "soc/interrupt_reg.h"
#include "soc/clic_reg.h"

#include "esp_attr.h"
#include "esp_tee.h"

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MCOUNTEREN_CSR    0x306
#define MTVT_CSR          0x307
#define MINTSTATUS_CSR    0xFB1
#define MINTTHRESH_CSR    0x347
#define UINTTHRESH_CSR    0x047

FORCE_INLINE_ATTR void rv_utils_tee_intr_global_enable(void)
{
    /*
     * Set the the U-mode previous enable global interrupts state
     *
     * NOTE: The TICK interrupt is setup before this service call and thus,
     * it occurs in the return path of this call.
     *
     * Before entering the U-mode interrupt handler routine, USTATUS:UIE is
     * cleared to disable U-mode interrupts temporarily.
     *
     * While exiting the above routine, URET is executed, setting USTATUS:UIE
     * to the value of USTATUS:UPIE. However, since no interrupts were enabled
     * previously, USTATUS:UPIE and thus, USTATUS:UIE is cleared.
     *
     * The service call completes and returns to U-mode with USTATUS:UIE disabled,
     * preventing any further interrupts in U-mode.
     *
     */
    RV_SET_CSR(ustatus, USTATUS_UPIE);
    /* Enabling the global M-mode and U-mode interrupts */
    RV_SET_CSR(ustatus, USTATUS_UIE);
    RV_SET_CSR(mstatus, MSTATUS_UIE);
    RV_SET_CSR(mstatus, MSTATUS_MIE);
}

FORCE_INLINE_ATTR void rv_utils_tee_intr_global_disable(void)
{
    RV_CLEAR_CSR(ustatus, USTATUS_UIE);
    RV_CLEAR_CSR(mstatus, MSTATUS_UIE);
    RV_CLEAR_CSR(mstatus, MSTATUS_MIE);
}

FORCE_INLINE_ATTR void rv_utils_tee_intr_enable(uint32_t intr_mask)
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

FORCE_INLINE_ATTR void rv_utils_tee_intr_disable(uint32_t intr_mask)
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

FORCE_INLINE_ATTR void rv_utils_tee_intr_set_type(int intr_num, enum intr_type type)
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

FORCE_INLINE_ATTR void rv_utils_tee_intr_set_priority(int rv_int_num, int priority)
{
    assert(rv_int_num >= 0 && rv_int_num < SOC_CPU_INTR_NUM);
    *(uint8_t volatile *)(BYTE_CLIC_INT_CTL_REG(rv_int_num + CLIC_EXT_INTR_NUM_OFFSET)) = (priority << BYTE_CLIC_INT_CTL_S);
}

FORCE_INLINE_ATTR void rv_utils_tee_intr_set_threshold(int priority_threshold)
{
    uint32_t adj_threshold = ((priority_threshold << (8 - NLBITS)) | 0x1f);
    REG_SET_FIELD(CLIC_INT_THRESH_REG, CLIC_CPU_INT_THRESH, adj_threshold);
    RV_WRITE_CSR(MINTTHRESH_CSR, adj_threshold);
    RV_WRITE_CSR(UINTTHRESH_CSR, adj_threshold);
}

FORCE_INLINE_ATTR void rv_utils_tee_intr_set_mode(int rv_int_num, int mode)
{
    assert(rv_int_num >= 0 && rv_int_num < SOC_CPU_INTR_NUM);
    REG_SET_FIELD(CLIC_INT_CTRL_REG(rv_int_num + CLIC_EXT_INTR_NUM_OFFSET), CLIC_INT_ATTR_MODE, mode);
}

#ifdef __cplusplus
}
#endif
