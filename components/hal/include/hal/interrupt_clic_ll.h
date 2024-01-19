/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdbool.h>
#include "hal/assert.h"
#include "esp_attr.h"
#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "soc/reg_base.h"


#if SOC_INT_CLIC_SUPPORTED

#include "soc/clic_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RV_TOTAL_INT_COUNT   48

/**
 * @brief Route any interrupt source to any CPU interrupt, including internal ones
 *
 * @param core_id CPU core to map the interrupt in (ignored on single-core targets)
 * @param intr_src Interrupt source to map
 * @param rv_int_num Destination CPU interrupt source, which can be an internal or an external interrupt.
 */
FORCE_INLINE_ATTR void interrupt_clic_ll_route(uint32_t core_id, int intr_src, int intr_num)
{
    HAL_ASSERT(intr_num < RV_TOTAL_INT_COUNT);

#if SOC_CPU_CORES_NUM > 1
    if (core_id == 0) {
        REG_WRITE(DR_REG_INTERRUPT_CORE0_BASE + 4 * intr_src, intr_num);
    } else {
        REG_WRITE(DR_REG_INTERRUPT_CORE1_BASE + 4 * intr_src, intr_num);
    }
#else
    REG_WRITE(DR_REG_INTERRUPT_CORE0_BASE + 4 * intr_src, intr_num);
#endif // SOC_CPU_CORES_NUM > 1
}


/**
 * @brief Get the type for the given interrupt
 *
 * @param rv_int_num Interrupt number to get the type of. It can be an internal or an external interrupt.
 *
 * @return 0 for level-triggered type, 1 for edge-triggered interrupt type
 */
FORCE_INLINE_ATTR int interrupt_clic_ll_get_type(int rv_int_num)
{
    return REG_GET_FIELD(CLIC_INT_CTRL_REG(rv_int_num), CLIC_INT_ATTR_TRIG) & 1;
}


/**
 * @brief Get the priority for the given interrupt
 *
 * @param rv_int_num Interrupt number to get the priority of. It can be an internal or an external interrupt.
 *
 * @return interrupt priority, between 0 and 7
 */
FORCE_INLINE_ATTR int interrupt_clic_ll_get_priority(int rv_int_num)
{
    const uint32_t priority = REG_GET_FIELD(CLIC_INT_CTRL_REG(rv_int_num), CLIC_INT_CTL);
    return (priority >> (8 - NLBITS));
}


/**
 * @brief Check if an interrupt is vectored
 *
 * @param rv_int_num Interrupt number to check. It can be an internal or an external interrupt.
 *
 * @return true if the interrupt is vectored, false else
 */
FORCE_INLINE_ATTR bool interrupt_clic_ll_is_vectored(int rv_int_num)
{
    const uint32_t shv = REG_GET_FIELD(CLIC_INT_CTRL_REG(rv_int_num), CLIC_INT_ATTR_SHV);
    return shv != 0;
}


/**
 * @brief Set an interrupt to vectored or non-vectored
 *
 * @param rv_int_num Interrupt number to modify. It can be an internal or an external interrupt.
 * @param vectored True to set the interrupt to vectored, false to set it to non vectored.
 *
 */
FORCE_INLINE_ATTR void interrupt_clic_ll_set_vectored(int rv_int_num, bool vectored)
{
    REG_SET_FIELD(CLIC_INT_CTRL_REG(rv_int_num), CLIC_INT_ATTR_SHV, vectored ? 1 : 0);
}

#ifdef __cplusplus
}
#endif

#endif // SOC_INT_PLIC_SUPPORTED
