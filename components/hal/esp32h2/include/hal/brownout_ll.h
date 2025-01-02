/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
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
#include "soc/lp_analog_peri_struct.h"
#include "hal/regi2c_ctrl.h"
#include "soc/regi2c_brownout.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief power down the flash when a brown out happens.
 *
 * @param enable true: power down flash. false: not power down
 */
static inline void brownout_ll_enable_flash_power_down(bool enable)
{
    LP_ANA_PERI.bod_mode0_cntl.bod_mode0_close_flash_ena = enable;
}

/**
 * @brief power down the RF circuits when a brown out happens
 *
 * @param enable true: power down. false: not power done.
 */
static inline void brownout_ll_enable_rf_power_down(bool enable)
{
    LP_ANA_PERI.bod_mode0_cntl.bod_mode0_pd_rf_ena = enable;
}

/**
 * @brief Enable this to reset brown out
 *
 * @note: If brown out interrupt is used, this should be disabled.
 *
 * @param reset_ena true: enable reset. false: disable reset.
 * @param reset_wait brown out reset wait cycles
 * @param select 1: chip reset, 0: system reset
 */
static inline void brownout_ll_reset_config(bool reset_ena, uint32_t reset_wait, uint8_t select)
{
    LP_ANA_PERI.bod_mode0_cntl.bod_mode0_reset_wait = reset_wait;
    LP_ANA_PERI.bod_mode0_cntl.bod_mode0_reset_ena = reset_ena;
    LP_ANA_PERI.bod_mode0_cntl.bod_mode0_reset_sel = select;
}

/**
 * @brief Set brown out threshold
 *
 * @param threshold brownout threshold
 */
static inline void brownout_ll_set_threshold(uint8_t threshold)
{
    REGI2C_WRITE_MASK(I2C_BOD, I2C_BOD_THRESHOLD_L, threshold);
    REGI2C_WRITE_MASK(I2C_BOD, I2C_BOD_THRESHOLD_H, threshold);

}

/**
 * @brief Set this bit to enable the brown out detection
 *
 * @param bod_enable true: enable, false: disable
 */
static inline void brownout_ll_bod_enable(bool bod_enable)
{
    LP_ANA_PERI.bod_mode0_cntl.bod_mode0_intr_ena = bod_enable;
}

/**
 * @brief configure the waiting cycles before sending an interrupt
 *
 * @param cycle waiting cycles.
 */
static inline void brownout_ll_set_intr_wait_cycles(uint8_t cycle)
{
    LP_ANA_PERI.bod_mode0_cntl.bod_mode0_intr_wait = cycle;
}

/**
 * @brief Enable brown out interrupt
 *
 * @param enable true: enable, false: disable
 */
static inline void brownout_ll_intr_enable(bool enable)
{
    LP_ANA_PERI.int_ena.bod_mode0_int_ena = enable;
}

/**
 * @brief Enable brownout hardware reset
 *
 * @param enable
 */
static inline void brownout_ll_ana_reset_enable(bool enable)
{
    LP_ANA_PERI.bod_mode1_cntl.bod_mode1_reset_ena = enable;
}

/**
 * @brief Clear interrupt bits.
 */
__attribute__((always_inline))
static inline void brownout_ll_intr_clear(void)
{
    LP_ANA_PERI.int_clr.bod_mode0_int_clr = 1;
}

/**
 * @brief Clear BOD internal count.
 */
static inline void brownout_ll_clear_count(void)
{
    LP_ANA_PERI.bod_mode0_cntl.bod_mode0_cnt_clr = 1;
    LP_ANA_PERI.bod_mode0_cntl.bod_mode0_cnt_clr = 0;
}

#ifdef __cplusplus
}
#endif
