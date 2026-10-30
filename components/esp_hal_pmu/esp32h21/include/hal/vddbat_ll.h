/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The ll is not public api, don't use in application code.
 * See readme.md in hal/readme.md
 ******************************************************************************/

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "esp_bit_defs.h"
#include "soc/lp_analog_peri_struct.h"
#include "hal/misc.h"
#include "hal/regi2c_ctrl.h"
#include "soc/regi2c_brownout.h"

/* LP_ANA interrupt bits driven by the VDDBAT charge filter channel */
#define VDDBAT_LL_UP_VOLT_INTR      BIT(27)
#define VDDBAT_LL_UNDER_VOLT_INTR   BIT(28)
#define VDDBAT_LL_INTR_MASK         (VDDBAT_LL_UP_VOLT_INTR | VDDBAT_LL_UNDER_VOLT_INTR)

/* The filter is a 10-bit up/down counter clocked by RC_FAST */
#define VDDBAT_LL_FILTER_TICK_MAX   1023

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set the VDDBAT_VGOOD comparator threshold
 *
 * @note This comparator is shared with the brownout detector, both features see the same threshold.
 *
 * @param threshold voltage threshold level
 */
static inline void vddbat_ll_set_vgood_threshold(uint8_t threshold)
{
    REGI2C_WRITE_MASK(I2C_BOD, I2C_BOD_THRESHOLD, threshold);
}

/**
 * @brief Route or unroute VDDBAT_VGOOD to the analog brownout reset path
 *
 * @param enable true: a VDDBAT_VGOOD low resets the chip. false: the reset path ignores VDDBAT_VGOOD.
 */
static inline void vddbat_ll_enable_vgood_reset_source(bool enable)
{
    uint32_t source_ena = HAL_FORCE_READ_U32_REG_FIELD(LP_ANA_PERI.ana_vdd_source_cntl, ana_bod_source_ena);
    if (enable) {
        source_ena |= BIT(0);
    } else {
        source_ena &= ~BIT(0);
    }
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_ANA_PERI.ana_vdd_source_cntl, ana_bod_source_ena, source_ena);
}

/**
 * @brief Set how many net low ticks confirm an under voltage
 *
 * @param ticks filter counter target, up to VDDBAT_LL_FILTER_TICK_MAX
 */
static inline void vddbat_ll_set_under_volt_filter_ticks(uint32_t ticks)
{
    LP_ANA_PERI.ana_vddbat_charge_cntl.ana_vddbat_charge_undervoltage_target = ticks;
}

/**
 * @brief Set the filter counter value at which a recovered voltage is confirmed
 *
 * @note The difference to the under voltage target is the hysteresis of the filter, in ticks.
 *
 * @param ticks filter counter target, must be lower than the under voltage target
 */
static inline void vddbat_ll_set_up_volt_filter_ticks(uint32_t ticks)
{
    LP_ANA_PERI.ana_vddbat_charge_cntl.ana_vddbat_charge_upvoltage_target = ticks;
}

/**
 * @brief Clear the filter counter
 */
static inline void vddbat_ll_clear_count(void)
{
    LP_ANA_PERI.ana_vddbat_charge_cntl.ana_vddbat_charge_cnt_clr = 1;
    LP_ANA_PERI.ana_vddbat_charge_cntl.ana_vddbat_charge_cnt_clr = 0;
}

/**
 * @brief Get the current under voltage flag of the filter channel
 *
 * @return true if the filter is holding an unconfirmed-recovery under voltage state
 */
static inline bool vddbat_ll_is_under_volt(void)
{
    return LP_ANA_PERI.ana_vddbat_charge_cntl.ana_vddbat_charge_undervoltage_flag;
}

/**
 * @brief Enable or disable interrupts of the filter channel
 *
 * @param mask   bitmask of VDDBAT_LL_*_INTR
 * @param enable true for enabling the interrupt, otherwise false
 *
 */
__attribute__((always_inline))
static inline void vddbat_ll_enable_intr_mask(uint32_t mask, bool enable)
{
    if (enable) {
        LP_ANA_PERI.ana_int_ena.val |= mask;
    } else {
        LP_ANA_PERI.ana_int_ena.val &= ~mask;
    }
}

/**
 * @brief Clear raw interrupt bits of the filter channel
 *
 * @note The raw bits are sticky and the PMU samples them regardless of the interrupt enable.
 *
 * @param mask bitmask of VDDBAT_LL_*_INTR
 */
__attribute__((always_inline))
static inline void vddbat_ll_clear_intr_mask(uint32_t mask)
{
    LP_ANA_PERI.ana_int_clr.val = mask;
}

/**
 * @brief Get the masked interrupt status of the filter channel
 *
 * @return status bitmask of VDDBAT_LL_*_INTR
 */
__attribute__((always_inline))
static inline uint32_t vddbat_ll_get_intr_status(void)
{
    return LP_ANA_PERI.ana_int_st.val & VDDBAT_LL_INTR_MASK;
}

/**
 * @brief Get the raw interrupt status of the filter channel
 *
 * @return raw bitmask of VDDBAT_LL_*_INTR
 */
static inline uint32_t vddbat_ll_get_intr_raw(void)
{
    return LP_ANA_PERI.ana_int_raw.val & VDDBAT_LL_INTR_MASK;
}

/**
 * @brief Get interrupt status register address
 *
 * @return Register address
 */
static inline volatile void *vddbat_ll_intr_get_status_reg(void)
{
    return &LP_ANA_PERI.ana_int_st;
}

#ifdef __cplusplus
}
#endif
