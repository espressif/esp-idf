/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for xtal32k WDT register operations.
// Note that most of the register operations in this layer are non-atomic operations.

#pragma once

#include <stdbool.h>
#include "soc/rtc_cntl_periph.h"

#ifdef __cplusplus
extern "C" {
#endif

#define XT_WDT_LL_XTAL32_DEAD_INTR_MASK RTC_CNTL_XTAL32K_DEAD_INT_ST_M

/**
 * @brief Enable the XT_WDT
 *
 * @param hw Start address of the peripheral registers.
 */
inline void xt_wdt_ll_enable(rtc_cntl_dev_t *hw, bool enable)
{
    hw->ext_xtl_conf.xtal32k_wdt_en = enable;
}

/**
 * @brief Check if the XT_WDT is enabled
 *
 * @param hw Start address of the peripheral registers.
 * @return True if XT WDT is enabled
 */
inline bool xt_wdt_ll_check_if_enabled(rtc_cntl_dev_t *hw)
{
    return (hw->ext_xtl_conf.xtal32k_wdt_en) ? true : false;
}

/**
 * @brief Set the watchdog timeout value
 *
 * @param hw Start address of the peripheral registers.
 * @param timeout timeout value in RTC_CLK cycles
 */
inline void xt_wdt_ll_set_timeout(rtc_cntl_dev_t *hw, uint8_t timeout)
{
    hw->xtal32k_conf.xtal32k_wdt_timeout = timeout;
}


/**
 * @brief Reset the XT_WDT
 *
 * @param hw Start address of the peripheral registers.
 */
inline void xt_wdt_ll_reset(rtc_cntl_dev_t *hw)
{
    hw->ext_xtl_conf.xtal32k_wdt_reset = 1;
    hw->ext_xtl_conf.xtal32k_wdt_reset = 0;
}


/**
 * @brief Set the backup clock value
 *
 * @param hw Start address of the peripheral registers.
 * @param backup_clk_val Backup clock value, see TRM for definition
 */
inline void xt_wdt_ll_set_backup_clk_factor(rtc_cntl_dev_t *hw, uint32_t backup_clk_val)
{
    hw->xtal32k_clk_factor = backup_clk_val;
}

/**
 * @brief Enable the auto-backup clock feature
 *
 * @param hw Start address of the peripheral registers.
 * @param enable True - enable, False - disable
 */
inline void xt_wdt_ll_auto_backup_enable(rtc_cntl_dev_t *hw, bool enable)
{
    hw->ext_xtl_conf.xtal32k_auto_backup = enable;
}

/**
 * @brief Enable the timeout interrupt
 *
 * @param hw Start address of the peripheral registers.
 * @param enable True - enable, False - disable
 */
inline void xt_wdt_ll_intr_enable(rtc_cntl_dev_t *hw, bool enable)
{
    hw->int_ena.rtc_xtal32k_dead = enable;
}

#ifdef __cplusplus
}
#endif
