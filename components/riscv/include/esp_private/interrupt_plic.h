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

#if SOC_INT_PLIC_SUPPORTED


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Set global masking level. On the PLIC, all interrupt priority levels strictly less than the threshold
 * level are masked.
 */
#define RVHAL_INTR_ENABLE_THRESH    1

/**
 * @brief Bitmask to enable the vector mode when writing MTVEC CSR.
 * Setting mode field to 1 treats `MTVEC` as a vector base address.
 */
#define MTVEC_MODE_CSR          1

/**
 * In the case of PLIC, all the interrupt lines are dedicated to external peripherals, so the offset is 0
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


#ifdef __cplusplus
}
#endif

#endif /* SOC_INT_PLIC_SUPPORTED */
