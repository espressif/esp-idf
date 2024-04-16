/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_attr.h"
#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "soc/reg_base.h"

/* Do not use INTC on targets that have hardware CLIC */
#if SOC_CPU_HAS_FLEXIBLE_INTC && !SOC_INT_CLIC_SUPPORTED

#include "soc/interrupt_reg.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Route any interrupt source to any CPU interrupt, including internal ones
 *
 * @param intr_src Interrupt source to map
 * @param rv_int_num Destination CPU interrupt source, which can be an internal or an external interrupt.
 */
FORCE_INLINE_ATTR void interrupt_intc_ll_route(int intr_src, int intr_num)
{
    REG_WRITE(DR_REG_INTERRUPT_BASE + 4 * intr_src, intr_num);
}


/**
 * @brief Get interrupt enable mask
 *
 * @return interrupt enable mask, bit i is 1 if interrupt i is enabled, 0 if interrupt i is disabled
 */
FORCE_INLINE_ATTR uint32_t interrupt_intc_ll_get_unmask(void)
{
    return REG_READ(INTERRUPT_CORE0_CPU_INT_ENABLE_REG);
}


/**
 * @brief Get the type for the given interrupt
 *
 * @param rv_int_num Interrupt number to get the type of. It can be an internal or an external interrupt.
 *
 * @return 0 for level-triggered type, 1 for edge-triggered interrupt type
 */
FORCE_INLINE_ATTR int interrupt_intc_ll_get_type(int rv_int_num)
{
    uint32_t intr_type_reg = REG_READ(INTERRUPT_CORE0_CPU_INT_TYPE_REG);
    return (intr_type_reg & (1 << rv_int_num));
}


/**
 * @brief Get the priority for the given interrupt
 *
 * @param rv_int_num Interrupt number to get the priority of. It can be an internal or an external interrupt.
 *
 * @return interrupt priority, between 0 and 7
 */
FORCE_INLINE_ATTR int interrupt_intc_ll_get_priority(int rv_int_num)
{
    return REG_READ(INTERRUPT_PRIO_REG(rv_int_num));
}


#ifdef __cplusplus
}
#endif

#endif // SOC_CPU_HAS_FLEXIBLE_INTC
