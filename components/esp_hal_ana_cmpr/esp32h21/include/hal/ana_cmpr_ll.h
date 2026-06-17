/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "hal/misc.h"
#include "hal/assert.h"
#include "hal/ana_cmpr_types.h"
#include "soc/ana_cmpr_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ANALOG_CMPR_LL_GET(_attr) ANALOG_CMPR_LL_ ## _attr
#define ANALOG_CMPR_LL_SUPPORT(_feat) ANALOG_CMPR_LL_SUPPORT_ ## _feat

// Number of Analog Comparator instances
#define ANALOG_CMPR_LL_INST_NUM 1

// ANA_CMPR IP version
#define ANALOG_CMPR_LL_IP_VERSION 1

// Number of pads for each Analog Comparator instance
#define ANALOG_CMPR_LL_PAD_NUM  2

// Number of source channels for the comparator
#define ANALOG_CMPR_LL_SRC_CHANNEL_NUM 1

// Can detect positive/negative/any cross type
#define ANALOG_CMPR_LL_SUPPORT_EDGE_SPECIFIC_INTR_MASK  1

#define ANALOG_CMPR_LL_GET_HW(unit)     (&ANALOG_CMPR[unit])

#define ANALOG_CMPR_LL_NEG_CROSS_INTR_MASK(unit, src_chan)   0x01
#define ANALOG_CMPR_LL_POS_CROSS_INTR_MASK(unit, src_chan)   0x02
#define ANALOG_CMPR_LL_ANY_CROSS_INTR_MASK(unit, src_chan)   (ANALOG_CMPR_LL_NEG_CROSS_INTR_MASK(unit, src_chan) | ANALOG_CMPR_LL_POS_CROSS_INTR_MASK(unit, src_chan))
#define ANALOG_CMPR_LL_ALL_INTR_MASK(unit)                   0x07

/**
 * @brief Get the interrupt mask from the given cross type
 *
 * @param unit_id Unit ID
 * @param src_chan Source channel ID
 * @param type Cross type
 * @return Interrupt mask value
 */
__attribute__((always_inline))
static inline uint32_t analog_cmpr_ll_get_intr_mask_by_type(uint32_t unit_id, uint32_t src_chan, ana_cmpr_cross_type_t type)
{
    uint32_t mask = 0;
    switch (type) {
    case ANA_CMPR_CROSS_POS:
        mask |= ANALOG_CMPR_LL_POS_CROSS_INTR_MASK(unit_id, src_chan);
        break;
    case ANA_CMPR_CROSS_NEG:
        mask |= ANALOG_CMPR_LL_NEG_CROSS_INTR_MASK(unit_id, src_chan);
        break;
    case ANA_CMPR_CROSS_ANY:
        mask |= ANALOG_CMPR_LL_ANY_CROSS_INTR_MASK(unit_id, src_chan);
        break;
    default:
        break;
    }
    return mask;
}

/**
 * @brief Enable analog comparator
 *
 * @param hw Analog comparator register base address
 * @param en True to enable, False to disable
 */
static inline void analog_cmpr_ll_enable(analog_cmpr_dev_t *hw, bool en)
{
    hw->pad_comp_config->ext_xpd_comp_0 = en;
}

/**
 * @brief Set the voltage of the internal reference
 *
 * @param hw Analog comparator register base address
 * @param volt_level The voltage level of the internal reference, range [0.0V, 0.7VDD], step 0.1VDD
 */
__attribute__((always_inline))
static inline void analog_cmpr_ll_set_internal_ref_voltage(analog_cmpr_dev_t *hw, ana_cmpr_ref_voltage_t volt_level)
{
    hw->pad_comp_config->ext_dref_comp_0 = volt_level;
}

/**
 * @brief Get the voltage of the internal reference
 *
 * @param hw Analog comparator register base address
 * @return The voltage of the internal reference
 */
static inline float analog_cmpr_ll_get_internal_ref_voltage(analog_cmpr_dev_t *hw)
{
    return hw->pad_comp_config->ext_dref_comp_0 * 0.1F;
}

/**
 * @brief The reference voltage comes from internal or external
 *
 * @note Also see `analog_cmpr_ll_set_internal_ref_voltage` to use the internal reference voltage
 *
 * @param hw Analog comparator register base address
 * @param ref_src reference source, 0 for internal, 1 for external GPIO pad
 */
static inline void analog_cmpr_ll_set_ref_source(analog_cmpr_dev_t *hw, ana_cmpr_ref_source_t ref_src)
{
    hw->pad_comp_config->ext_mode_comp_0 = ref_src;
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
static inline void analog_cmpr_ll_set_cross_debounce_cycle(analog_cmpr_dev_t *hw, uint32_t cycle)
{
    hw->pad_comp_filter->ext_zero_det_filter_cnt_0 = cycle;
}

/**
 * @brief Enable comparator interrupt
 *
 * @param hw Analog comparator register base address
 * @param mask Interrupt mask
 * @param enable True to enable, False to disable
 */
__attribute__((always_inline))
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
    return hw->int_st->val & ANALOG_CMPR_LL_ALL_INTR_MASK(0);
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

/**
 * @brief Enable the bus clock for Analog Comparator module
 *
 * @param unit_id Unit ID
 * @param enable true to enable, false to disable
 */
static inline void analog_cmpr_ll_enable_bus_clock(int unit_id, bool enable)
{
    (void)unit_id;
    (void)enable;
}

/**
 * @brief Reset the Analog Comparator module
 *
 * @param unit_id Unit ID
 */
static inline void analog_cmpr_ll_reset_register(int unit_id)
{
    (void)unit_id;
}

/**
 * @brief Reset the core logic of Analog Comparator module
 *
 * @param unit_id Unit ID
 */
static inline void analog_cmpr_ll_reset_core(int unit_id)
{
    (void)unit_id;
}

/**
 * @brief Set the clock source for analog comparator PAD_COMP_CLK
 *
 * @param unit_id Unit ID
 * @param clk_src Clock source, see `ana_cmpr_clk_src_t`
 */
static inline void analog_cmpr_ll_set_clk_src(int unit_id, ana_cmpr_clk_src_t clk_src)
{
    (void)unit_id;
    (void)clk_src;
}

/**
 * @brief Enable the function clock for Analog Comparator module
 *
 * @param unit_id Unit ID
 * @param enable true to enable, false to disable
 */
static inline void analog_cmpr_ll_enable_function_clock(int unit_id, bool enable)
{
    (void)unit_id;
    (void)enable;
}

/**
 * @brief Set the clock divider for analog comparator PAD_COMP_CLK
 *
 * @param unit_id Unit ID
 * @param div Clock divider value, the output clock frequency is input clock frequency divided by this value.
 *            Must be greater than 0.
 */
static inline void analog_cmpr_ll_set_clk_div(int unit_id, uint32_t div)
{
    (void)unit_id;
    (void)div;
}

#ifdef __cplusplus
}
#endif
