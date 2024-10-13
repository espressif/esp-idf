/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once


#include <stdint.h>
#include <assert.h>
#include "esp_attr.h"
#include "soc/interrupt_reg.h"
#include "soc/soc_caps.h"
#include "riscv/csr.h"

#if !SOC_INT_CLIC_SUPPORTED && !SOC_INT_PLIC_SUPPORTED

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Set global masking level. On the legacy INTC, all interrupt priority levels strictly less than the threshold
 * level are masked.
 */
#define RVHAL_INTR_ENABLE_THRESH    1

/**
 * @brief Bitmask to enable the vector mode when writing MTVEC CSR.
 * Setting mode field to 1 treats `MTVEC` as a vector base address.
 */
#define MTVEC_MODE_CSR          1

/**
 * In the case of INTC, all the interrupt lines are dedicated to external peripherals, so the offset is 0
 */
#define RV_EXTERNAL_INT_COUNT   32
#define RV_EXTERNAL_INT_OFFSET  0


FORCE_INLINE_ATTR void assert_valid_rv_int_num(int rv_int_num)
{
    assert(rv_int_num != 0 && "Invalid CPU interrupt number");
}

/**
 * @brief Get the enabled interrupts on the current CPU.
 *
 * @return Bit mask of the enabled interrupts
 */
FORCE_INLINE_ATTR uint32_t rv_utils_intr_get_enabled_mask(void)
{
    return REG_READ(INTERRUPT_CORE0_CPU_INT_ENABLE_REG);
}

/**
 * @brief Acknowledge an edge interrupt
 *
 * @param intr_num Interrupt number (from 0 to 31)
 */
FORCE_INLINE_ATTR void rv_utils_intr_edge_ack(uint32_t intr_num)
{
    REG_SET_BIT(INTERRUPT_CORE0_CPU_INT_CLEAR_REG, intr_num);
}

/**
 * @brief Restore the CPU interrupt level to the value returned by `rv_utils_set_intlevel_regval`.
 *
 * @param restoreval Former raw interrupt level, it is NOT necessarily a value between 0 and 7, this is hardware and configuration dependent.
 */
FORCE_INLINE_ATTR void rv_utils_restore_intlevel_regval(uint32_t restoreval)
{
    REG_WRITE(INTERRUPT_CURRENT_CORE_INT_THRESH_REG, restoreval);
}

/**
 * @brief Set the interrupt threshold to `intlevel` while getting the current level.
 *
 * @param intlevel New raw interrupt level, it is NOT necessarily a value between 0 and 7, this is hardware and configuration dependent.
 *
 * @return Current raw interrupt level, can be restored by calling `rv_utils_restore_intlevel_regval`.
 */
FORCE_INLINE_ATTR uint32_t rv_utils_set_intlevel_regval(uint32_t intlevel)
{
    uint32_t old_mstatus = RV_CLEAR_CSR(mstatus, MSTATUS_MIE);
    uint32_t old_thresh = REG_READ(INTERRUPT_CURRENT_CORE_INT_THRESH_REG);
    rv_utils_restore_intlevel_regval(intlevel);
    RV_SET_CSR(mstatus, old_mstatus & MSTATUS_MIE);

    return old_thresh;
}


#ifdef __cplusplus
}
#endif

#endif /* !SOC_INT_CLIC_SUPPORTED && !SOC_INT_PLIC_SUPPORTED */
