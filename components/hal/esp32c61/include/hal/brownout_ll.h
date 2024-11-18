/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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
#include "hal/regi2c_ctrl.h"
#include "hal/psdet_types.h"
#include "soc/regi2c_brownout.h"

#define BROWNOUT_DETECTOR_LL_INTERRUPT_MASK   (BIT(31))
#define BROWNOUT_DETECTOR_LL_FIB_ENABLE       (BIT(1))

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief suspend the flash when a brown out happens.
 *
 * @param enable true: suspend flash. false: not suspend
 */
static inline void brownout_ll_enable_flash_suspend(bool enable)
{
    LP_ANA.bod_mode0_cntl.bod_mode0_close_flash_ena = enable;
}

/**
 * @brief power down the RF circuits when a brown out happens
 *
 * @param enable true: power down. false: not power down.
 */
static inline void brownout_ll_enable_rf_power_down(bool enable)
{
    LP_ANA.bod_mode0_cntl.bod_mode0_pd_rf_ena = enable;
}

/**
 * @brief Configure the brown out detector to do a hardware reset
 *
 * @note: If brown out interrupt is also used, the hardware reset can be disabled,
 *        because we can call software reset in the interrupt handler.
 *
 * @param reset_ena true: enable reset. false: disable reset.
 * @param reset_wait brown out reset wait cycles
 * @param reset_level reset level
 */
static inline void brownout_ll_reset_config(bool reset_ena, uint32_t reset_wait, brownout_reset_level_t reset_level)
{
    LP_ANA.bod_mode0_cntl.bod_mode0_reset_wait = reset_wait;
    LP_ANA.bod_mode0_cntl.bod_mode0_reset_ena = reset_ena;
    LP_ANA.bod_mode0_cntl.bod_mode0_reset_sel = reset_level;
}

/**
 * @brief Set brown out threshold voltage
 *
 * @param threshold brownout threshold
 */
static inline void brownout_ll_set_threshold(uint8_t threshold)
{
    REGI2C_WRITE_MASK(I2C_BOD, I2C_BOD_THRESHOLD, threshold);
}

/**
 * @brief Set this bit to enable the brown out detection
 *
 * @param bod_enable true: enable, false: disable
 */
static inline void brownout_ll_bod_enable(bool bod_enable)
{
    LP_ANA.bod_mode0_cntl.bod_mode0_intr_ena = bod_enable;
}

/**
 * @brief configure the waiting cycles before sending an interrupt
 *
 * @param cycle waiting cycles.
 */
static inline void brownout_ll_set_intr_wait_cycles(uint8_t cycle)
{
    LP_ANA.bod_mode0_cntl.bod_mode0_intr_wait = cycle;
}

/**
 * @brief Enable brown out interrupt
 *
 * @param enable true: enable, false: disable
 */
static inline void brownout_ll_intr_enable(bool enable)
{
    LP_ANA.int_ena.bod_mode0_int_ena = enable;
}

/**
 * @brief Enable brownout hardware reset (mode1)
 *
 * @param enable true: enable, false: disable
 */
static inline void brownout_ll_ana_reset_enable(bool enable)
{
    // give BOD mode1 control permission to the software
    LP_ANA.fib_enable.val &= ~BROWNOUT_DETECTOR_LL_FIB_ENABLE;
    // then we can enable or disable if we want the BOD mode1 to reset the system
    LP_ANA.bod_mode1_cntl.bod_mode1_reset_ena = enable;
}

/**
 * @brief Clear interrupt bits.
 */
__attribute__((always_inline))
static inline void brownout_ll_intr_clear(void)
{
    LP_ANA.int_clr.bod_mode0_int_clr = 1;
}

/**
 * @brief Clear BOD internal count.
 */
static inline void brownout_ll_clear_count(void)
{
    LP_ANA.bod_mode0_cntl.bod_mode0_cnt_clr = 1;
    LP_ANA.bod_mode0_cntl.bod_mode0_cnt_clr = 0;
}

/**
 * @brief Get interrupt status register address
 *
 * @return Register address
 */
static inline volatile void *brownout_ll_intr_get_status_reg(void)
{
    return &LP_ANA.int_st;
}

#ifdef __cplusplus
}
#endif
