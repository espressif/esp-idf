/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
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
#include "esp_bit_defs.h"
#include "soc/lp_analog_peri_struct.h"
#include "hal/assert.h"
#include "hal/regi2c_ctrl.h"
#include "soc/regi2c_brownout.h"


typedef enum {
    VBAT_LL_CHARGER_UPVOLTAGE_INTR = BIT(27),
    VBAT_LL_CHARGER_UNDERVOLTAGE_INTR = BIT(28),
    VBAT_LL_BROWNOUT_INTR = BIT(30),
} vbat_ll_intr_t;

#define VBAT_LL_CHARGER_MASK   (BIT(27)|BIT(28))
#define VBAT_LL_DETECT_MASK    (BIT(30))

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set vbat brownout threshold voltage
 *
 * @param threshold vbat brownout threshold
 */
static inline void vbat_ll_set_brownout_threshold(uint8_t threshold)
{
    // Give same value
    REGI2C_WRITE_MASK(I2C_BOD, I2C_BIAS_OR_DREF_VBAT_L, threshold);
    REGI2C_WRITE_MASK(I2C_BOD, I2C_BIAS_OR_DREF_VBAT_H, threshold);
}

/**
 * @brief Set vbat charge threshold voltage
 *
 * @param threshold vbat charge threshold
 */
static inline void vbat_ll_set_charger_threshold(uint8_t threshold_l, uint8_t threshold_h)
{
    REGI2C_WRITE_MASK(I2C_BOD, I2C_BIAS_OR_DREF_VBAT_CHARGER_L, threshold_l);
    REGI2C_WRITE_MASK(I2C_BOD, I2C_BIAS_OR_DREF_VBAT_CHARGER_H, threshold_h);
}

/**
 * @brief Enable or disable the VBAT charger comparator
 *
 * @param enable Set to `true` to enable the comparator, or `false` to disable it.
 */
static inline void vbat_ll_enable_charger_comparator(bool enable)
{
    REGI2C_WRITE_MASK(I2C_BOD, I2C_BIAS_OR_FORCE_PU_VBAT_CHARGER, enable);
}

/**
 * @brief Set the under voltage filter time for the charger detector
 *
 * @param time_tick The filter time in ticks (unit depends on the hardware implementation).
 */
static inline void vbat_ll_set_undervoltage_filter_time(uint32_t time_tick)
{
    HAL_ASSERT(time_tick < (2<<10));
    LP_ANA_PERI.vddbat_charge_cntl.vddbat_charge_undervoltage_target = time_tick;
}

/**
 * @brief Set the upvoltage filter time for the charger detector
 *
 * @param time_tick The filter time in ticks (unit depends on the hardware implementation).
 */
static inline void vbat_ll_set_upvoltage_filter_time(uint32_t time_tick)
{
    HAL_ASSERT(time_tick < (2<<10));
    LP_ANA_PERI.vddbat_charge_cntl.vddbat_charge_upvoltage_target = time_tick;
}

/**
 * @brief Set the charger resistor value for VBAT charging
 *
 * @param resistor Resistor value to be set (unit depends on the hardware implementation).
 */
static inline void vbat_ll_set_charger_resistor(uint32_t resistor)
{
    REGI2C_WRITE_MASK(I2C_BOD, I2C_BIAS_OR_DRES_CHARGER, resistor);
}

/*
 * @brief Start or stop the VBAT battery charging process
 *
 * @param start Set to true to start charging, or false to stop charging.
 */
static inline void vbat_ll_start_battery_charge(bool start)
{
    LP_ANA_PERI.vddbat_charge_cntl.vddbat_charge_charger = start;
    LP_ANA_PERI.vddbat_bod_cntl.vddbat_charger = start;
}

/**
 * @brief Enable the interrupt mask for vbat usage
 *
 * @param mask A bitmask representing the interrupts to enable.
 *             Each bit corresponds to a specific interrupt source.
 * @param enable true for enabling the interrupt, otherwise false.
 *
 * @note Avoid concurrency risky with brownout_ll_intr_enable
 */
static inline void vbat_ll_enable_intr_mask(uint32_t mask, bool enable)
{
    if (enable) {
        LP_ANA_PERI.int_ena.val |= mask;
    } else {
        LP_ANA_PERI.int_ena.val &= ~mask;
    }
}

/**
 * @brief Clear the interrupt mask for vbat usage
 *
 * @param mask A bitmask representing the interrupts to clear.
 *             Each bit corresponds to a specific interrupt source.
 */
static inline void vbat_ll_clear_intr_mask(uint32_t mask)
{
    LP_ANA_PERI.int_clr.val = mask;
}

/**
 * @brief Get the current interrupt mask for vbat usage
 *
 * @param intr_status Pointer to a variable where the interrupt status mask will be stored.
 *                    The function will write the current interrupt status to this variable.
 */
static inline void vbat_ll_get_interrupt_status(uint32_t *intr_status)
{
    *intr_status = LP_ANA_PERI.int_st.val;
}

/**
 * @brief Clear the VBAT count for charge detection
 *
 * This function clears the internal counter that tracks the number of charge events detected
 * related to the VBAT power supply. It is typically used to reset the count for monitoring purposes.
 */
static inline void vbat_ll_clear_count(void)
{
    LP_ANA_PERI.vddbat_charge_cntl.vddbat_charge_cnt_clr = 1;
    LP_ANA_PERI.vddbat_charge_cntl.vddbat_charge_cnt_clr = 0;
}

#ifdef __cplusplus
}
#endif
