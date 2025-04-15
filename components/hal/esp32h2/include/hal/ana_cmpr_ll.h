/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "hal/misc.h"
#include "hal/assert.h"
#include "soc/ana_cmpr_struct.h"

#define ANALOG_CMPR_LL_GET_HW(unit)     (&ANALOG_CMPR[unit])
#define ANALOG_CMPR_LL_EVENT_CROSS      (1 << 0)

#define ANALOG_CMPR_LL_ALL_INTR_MASK(unit) (ANALOG_CMPR_LL_EVENT_CROSS)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable analog comparator
 *
 * @param hw Analog comparator register base address
 * @param en True to enable, False to disable
 */
static inline void analog_cmpr_ll_enable(analog_cmpr_dev_t *hw, bool en)
{
    hw->pad_comp_config->xpd_comp = en;
}

/**
 * @brief Set the voltage of the internal reference
 *
 * @param hw Analog comparator register base address
 * @param volt_level The voltage level of the internal reference, range [0.0V, 0.7VDD], step 0.1VDD
 */
__attribute__((always_inline))
static inline void analog_cmpr_ll_set_internal_ref_voltage(analog_cmpr_dev_t *hw, uint32_t volt_level)
{
    hw->pad_comp_config->dref_comp = volt_level;
}

/**
 * @brief Get the voltage of the internal reference
 *
 * @param hw Analog comparator register base address
 * @return The voltage of the internal reference times 10
 */
static inline uint32_t analog_cmpr_ll_get_internal_ref_voltage(analog_cmpr_dev_t *hw)
{
    return hw->pad_comp_config->dref_comp;
}

/**
 * @brief The reference voltage comes from internal or external
 *
 * @note Also see `analog_cmpr_ll_set_internal_ref_voltage` to use the internal reference voltage
 *
 * @param hw Analog comparator register base address
 * @param ref_src reference source, 0 for internal, 1 for external GPIO pad (GPIO10)
 */
static inline void analog_cmpr_ll_set_ref_source(analog_cmpr_dev_t *hw, uint32_t ref_src)
{
    hw->pad_comp_config->mode_comp = ref_src;
}

/**
 * @brief Set cross interrupt trigger type
 *
 * @param hw Analog comparator register base address
 * @param type The type of cross interrupt
 *              - 0: disable interrupt
 *              - 1: enable positive cross interrupt (input analog goes from low to high and across the reference voltage)
 *              - 2: enable negative cross interrupt (input analog goes from high to low and across the reference voltage)
 *              - 3: enable any positive or negative cross interrupt
 */
__attribute__((always_inline))
static inline void analog_cmpr_ll_set_cross_type(analog_cmpr_dev_t *hw, uint8_t type)
{
    hw->pad_comp_config->zero_det_mode = type;
}

/**
 * @brief Get the interrupt mask by trigger type
 * @note  Only one interrupt on H2
 *
 * @param hw Analog comparator register base address
 * @param type Not used on H2, because H2 can't distinguish the edge type
 *             The parameter here only to be compatible with other targets
 * @return interrupt mask
 */
__attribute__((always_inline))
static inline uint32_t analog_cmpr_ll_get_intr_mask_by_type(analog_cmpr_dev_t *hw, uint8_t type)
{
    (void)type;
    return ANALOG_CMPR_LL_EVENT_CROSS;
}

/**
 * @brief Set the debounce cycle for the cross detection
 *
 * @note When the comparator detects a cross, it will wait for the debounce cycle to make sure the cross is stable.
 *
 * @param hw Analog comparator register base address
 * @param cycle The debounce cycle
 */
__attribute__((always_inline))
static inline void analog_cmpr_ll_set_debounce_cycle(analog_cmpr_dev_t *hw, uint32_t cycle)
{
    hw->pad_comp_filter->zero_det_filter_cnt = cycle;
}

/**
 * @brief Enable comparator interrupt
 *
 * @param hw Analog comparator register base address
 * @param mask Interrupt mask
 * @param enable True to enable, False to disable
 */
static inline void analog_cmpr_ll_enable_intr(analog_cmpr_dev_t *hw, uint32_t mask, bool enable)
{
    uint32_t val = hw->int_ena->val;
    if (enable) {
        val |= mask;
    } else {
        val &= ~mask;
    }
    hw->int_ena->val = val;
}

/**
 * @brief Get comparator interrupt status
 *
 * @param hw Analog comparator register base address
 */
__attribute__((always_inline))
static inline uint32_t analog_cmpr_ll_get_intr_status(analog_cmpr_dev_t *hw)
{
    return hw->int_st->val;
}

/**
 * @brief Clear comparator interrupt status
 *
 * @param hw Analog comparator register base address
 * @param mask Interrupt status word
 */
__attribute__((always_inline))
static inline void analog_cmpr_ll_clear_intr(analog_cmpr_dev_t *hw, uint32_t mask)
{
    hw->int_clr->val = mask;
}

/**
 * @brief Get the interrupt status register address
 *
 * @param hw Analog comparator register base address
 * @return The interrupt status register address
 */
static inline volatile void *analog_cmpr_ll_get_intr_status_reg(analog_cmpr_dev_t *hw)
{
    return hw->int_st;
}

#ifdef __cplusplus
}
#endif
