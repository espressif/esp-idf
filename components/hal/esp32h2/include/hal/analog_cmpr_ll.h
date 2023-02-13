/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "hal/misc.h"
#include "hal/assert.h"
#include "soc/gpio_ext_struct.h"

#define ANALOG_CMPR_LL_GET_HW()    (&ANALOG_CMPR)
#define ANALOG_CMPR_LL_EVENT_CROSS (1 << 0)

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
    hw->pad_comp_config.xpd_comp = en;
}

/**
 * @brief Set the voltage of the internal reference
 *
 * @param hw Analog comparator register base address
 * @param voltage The voltage of the internal reference, range [0.0V, 0.7VDD], step 0.1VDD
 */
static inline void analog_cmpr_ll_set_internal_ref_voltage(analog_cmpr_dev_t *hw, float voltage)
{
    hw->pad_comp_config.mode_comp = 0;
    uint32_t volt_reg_val = (uint32_t)((voltage + 0.05F) / 0.1F);
    HAL_ASSERT(volt_reg_val <= 7);
    hw->pad_comp_config.dref_comp = volt_reg_val;
}

/**
 * @brief Get the voltage of the internal reference
 *
 * @param hw Analog comparator register base address
 * @return The voltage of the internal reference
 */
static inline float analog_cmpr_ll_get_internal_ref_voltage(analog_cmpr_dev_t *hw)
{
    return hw->pad_comp_config.dref_comp * 0.1F;
}

/**
 * @brief The reference voltage comes from GPIO pad (GPIO10)
 *
 * @note Also see `analog_cmpr_ll_set_internal_ref_voltage` to use the internal reference voltage
 *
 * @param hw Analog comparator register base address
 */
static inline void analog_cmpr_ll_ref_voltage_from_external(analog_cmpr_dev_t *hw)
{
    hw->pad_comp_config.mode_comp = 1;
}

/**
 * @brief Disable the cross detection
 *
 * @param hw Analog comparator register base address
 */
static inline void analog_cmpr_ll_disable_cross_detection(analog_cmpr_dev_t *hw)
{
    hw->pad_comp_config.zero_det_mode = 0;
}

/**
 * @brief Enable to detect the positive cross (input analog goes from low to high and across the reference voltage)
 *
 * @param hw Analog comparator register base address
 * @param enable True to enable, False to disable
 */
static inline void analog_cmpr_ll_enable_pos_cross_detection(analog_cmpr_dev_t *hw, bool enable)
{
    if (enable) {
        hw->pad_comp_config.zero_det_mode |= (1 << 0);
    } else {
        hw->pad_comp_config.zero_det_mode &= ~(1 << 0);
    }
}

/**
 * @brief Enable to detect the negative cross (input analog goes from high to low and across the reference voltage)
 *
 * @param hw Analog comparator register base address
 * @param enable True to enable, False to disable
 */
static inline void analog_cmpr_ll_enable_neg_cross_detection(analog_cmpr_dev_t *hw, bool enable)
{
    if (enable) {
        hw->pad_comp_config.zero_det_mode |= (1 << 1);
    } else {
        hw->pad_comp_config.zero_det_mode &= ~(1 << 1);
    }
}

/**
 * @brief Set the debounce cycle for the cross detection
 *
 * @note When the comparator detects a cross, it will wait for the debounce cycle to make sure the cross is stable.
 *
 * @param hw Analog comparator register base address
 * @param cycle The debounce cycle
 */
static inline void analog_cmpr_ll_set_debounce_cycle(analog_cmpr_dev_t *hw, uint32_t cycle)
{
    hw->pad_comp_filter.zero_det_filter_cnt = cycle;
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
    if (enable) {
        hw->int_ena.val |= mask;
    } else {
        hw->int_ena.val &= ~mask;
    }
}

/**
 * @brief Get comparator interrupt status
 *
 * @param hw Analog comparator register base address
 */
static inline void analog_cmpr_ll_get_intr_status(analog_cmpr_dev_t *hw)
{
    hw->int_st.val;
}

/**
 * @brief Clear comparator interrupt status
 *
 * @param hw Analog comparator register base address
 * @param mask Interrupt status word
 */
static inline void analog_cmpr_ll_clear_intr(analog_cmpr_dev_t *hw, uint32_t mask)
{
    hw->int_clr.val = mask;
}

/**
 * @brief Get the interrupt status register address
 *
 * @param hw Analog comparator register base address
 * @return The interrupt status register address
 */
static inline volatile void *analog_cmpr_ll_get_intr_status_reg(analog_cmpr_dev_t *hw)
{
    return &hw->int_st;
}

#ifdef __cplusplus
}
#endif
