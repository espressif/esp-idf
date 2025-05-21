/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <assert.h>

#include "riscv/csr.h"
#include "riscv/interrupt.h"

#include "soc/interrupt_reg.h"
#include "soc/plic_reg.h"

#include "esp_attr.h"
#include "esp_tee.h"

#ifdef __cplusplus
extern "C" {
#endif

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
    unsigned old_xstatus;

    // Machine mode
    // Disable all interrupts to make updating of the interrupt mask atomic.
    old_xstatus = RV_CLEAR_CSR(mstatus, MSTATUS_MIE);
    REG_SET_BIT(PLIC_MXINT_ENABLE_REG, intr_mask);
    RV_SET_CSR(mie, intr_mask);
    RV_SET_CSR(mstatus, old_xstatus & MSTATUS_MIE);

    // User mode
    // Disable all interrupts to make updating of the interrupt mask atomic.
    old_xstatus = RV_CLEAR_CSR(ustatus, USTATUS_UIE);
    REG_SET_BIT(PLIC_UXINT_ENABLE_REG, intr_mask);
    RV_SET_CSR(uie, intr_mask);
    RV_SET_CSR(ustatus, old_xstatus & USTATUS_UIE);
}

FORCE_INLINE_ATTR void rv_utils_tee_intr_disable(uint32_t intr_mask)
{
    unsigned old_xstatus;

    // Machine mode
    // Disable all interrupts to make updating of the interrupt mask atomic.
    old_xstatus = RV_CLEAR_CSR(mstatus, MSTATUS_MIE);
    REG_CLR_BIT(PLIC_MXINT_ENABLE_REG, intr_mask);
    RV_CLEAR_CSR(mie, intr_mask);
    RV_SET_CSR(mstatus, old_xstatus & MSTATUS_MIE);

    // User mode
    // Disable all interrupts to make updating of the interrupt mask atomic.
    old_xstatus = RV_CLEAR_CSR(ustatus, USTATUS_UIE);
    REG_CLR_BIT(PLIC_UXINT_ENABLE_REG, intr_mask);
    RV_CLEAR_CSR(uie, intr_mask);
    RV_SET_CSR(ustatus, old_xstatus & USTATUS_UIE);
}

FORCE_INLINE_ATTR void rv_utils_tee_intr_set_type(int intr_num, enum intr_type type)
{
    assert(intr_num >= 0 && intr_num < SOC_CPU_INTR_NUM);

    if (type == INTR_TYPE_LEVEL) {
        REG_CLR_BIT(PLIC_MXINT_TYPE_REG, BIT(intr_num));
        REG_CLR_BIT(PLIC_UXINT_TYPE_REG, BIT(intr_num));
    } else {
        REG_SET_BIT(PLIC_MXINT_TYPE_REG, BIT(intr_num));
        REG_SET_BIT(PLIC_UXINT_TYPE_REG, BIT(intr_num));
    }
}

FORCE_INLINE_ATTR void rv_utils_tee_intr_set_priority(int rv_int_num, int priority)
{
    assert(rv_int_num >= 0 && rv_int_num < SOC_CPU_INTR_NUM);

    REG_WRITE(PLIC_MXINT_PRI_REG(rv_int_num), priority);
    REG_WRITE(PLIC_UXINT_PRI_REG(rv_int_num), priority);
}

FORCE_INLINE_ATTR void rv_utils_tee_intr_set_threshold(int priority_threshold)
{
    REG_WRITE(PLIC_MXINT_THRESH_REG, priority_threshold);
    REG_WRITE(PLIC_UXINT_THRESH_REG, priority_threshold);
}

FORCE_INLINE_ATTR void rv_utils_tee_intr_edge_ack(int intr_num)
{
    assert(intr_num >= 0 && intr_num < SOC_CPU_INTR_NUM);

    REG_SET_BIT(PLIC_MXINT_CLEAR_REG, intr_num);
    REG_SET_BIT(PLIC_UXINT_CLEAR_REG, intr_num);
}

#ifdef __cplusplus
}
#endif
