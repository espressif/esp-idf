/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for Timer Group register operations.
// Note that most of the register operations in this layer are non-atomic operations.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdbool.h>
#include "hal/misc.h"
#include "hal/wdt_types.h"
#include "soc/rtc_cntl_periph.h"
#include "soc/efuse_reg.h"
#include "esp_attr.h"

#include "esp32h2/rom/ets_sys.h"

/**
 * @brief Enable the RWDT
 *
 * @param hw Start address of the peripheral registers.
 */
FORCE_INLINE_ATTR void lpwdt_ll_enable(lp_wdt_dev_t *hw)
{
    // hw->config0.en = 1;
}

/**
 * @brief Disable the RWDT
 *
 * @param hw Start address of the peripheral registers.
 * @note This function does not disable the flashboot mode. Therefore, given that
 *       the MWDT is disabled using this function, a timeout can still occur
 *       if the flashboot mode is simultaneously enabled.
 */
FORCE_INLINE_ATTR void lpwdt_ll_disable(lp_wdt_dev_t *hw)
{
    // hw->config0.en = 0;
}

/**
 * @brief Check if the RWDT is enabled
 *
 * @param hw Start address of the peripheral registers.
 * @return True if RTC WDT is enabled
 */
FORCE_INLINE_ATTR bool lpwdt_ll_check_if_enabled(lp_wdt_dev_t *hw)
{
    return false;//(hw->config0.en) ? true : false;
}

/**
 * @brief Configure a particular stage of the RWDT
 *
 * @param hw Start address of the peripheral registers.
 * @param stage Which stage to configure
 * @param timeout Number of timer ticks for the stage to timeout (see note).
 * @param behavior What action to take when the stage times out
 *
 * @note The value of of RWDT stage 0 timeout register is special, in
 *       that an implicit multiplier is applied to that value to produce
 *       and effective timeout tick value. The multiplier is dependent
 *       on an EFuse value. Therefore, when configuring stage 0, the valid
 *       values for the timeout argument are:
 *       - If Efuse value is 0, any even number between [2,2*UINT32_MAX]
 *       - If Efuse value is 1, any multiple of 4 between [4,4*UINT32_MAX]
 *       - If Efuse value is 2, any multiple of 8 between [8,8*UINT32_MAX]
 *       - If Efuse value is 3, any multiple of 16 between [16,16*UINT32_MAX]
 */
FORCE_INLINE_ATTR void lpwdt_ll_config_stage(lp_wdt_dev_t *hw, wdt_stage_t stage, uint32_t timeout_ticks, wdt_stage_action_t behavior)
{
    // switch (stage) {
    // case WDT_STAGE0:
    //     hw->config0.stg0 = behavior;
    //     //Account of implicty multiplier applied to stage 0 timeout tick config value
    //     hw->config1 = timeout_ticks >> (1 + REG_GET_FIELD(EFUSE_RD_REPEAT_DATA1_REG, EFUSE_WDT_DELAY_SEL));
    //     break;
    // case WDT_STAGE1:
    //     hw->config0.stg1 = behavior;
    //     hw->config2 = timeout_ticks;
    //     break;
    // case WDT_STAGE2:
    //     hw->config0.stg2 = behavior;
    //     hw->config3 = timeout_ticks;
    //     break;
    // case WDT_STAGE3:
    //     hw->config0.stg3 = behavior;
    //     hw->config4 = timeout_ticks;
    //     break;
    // default:
    //     abort();
    // }
}

/**
 * @brief Disable a particular stage of the RWDT
 *
 * @param hw Start address of the peripheral registers.
 * @param stage Which stage to disable
 */
FORCE_INLINE_ATTR void lpwdt_ll_disable_stage(lp_wdt_dev_t *hw, wdt_stage_t stage)
{
    // switch (stage) {
    // case WDT_STAGE0:
    //     hw->config0.stg0 = WDT_STAGE_ACTION_OFF;
    //     break;
    // case WDT_STAGE1:
    //     hw->config0.stg1 = WDT_STAGE_ACTION_OFF;
    //     break;
    // case WDT_STAGE2:
    //     hw->config0.stg2 = WDT_STAGE_ACTION_OFF;
    //     break;
    // case WDT_STAGE3:
    //     hw->config0.stg3 = WDT_STAGE_ACTION_OFF;
    //     break;
    // default:
    //     abort();
    // }
}

/**
 * @brief Set the length of the CPU reset action
 *
 * @param hw Start address of the peripheral registers.
 * @param length Length of CPU reset signal
 */
FORCE_INLINE_ATTR void lpwdt_ll_set_cpu_reset_length(lp_wdt_dev_t *hw, wdt_reset_sig_length_t length)
{
    // hw->config0.cpu_reset_length = length;
}

/**
 * @brief Set the length of the system reset action
 *
 * @param hw Start address of the peripheral registers.
 * @param length Length of system reset signal
 */
FORCE_INLINE_ATTR void lpwdt_ll_set_sys_reset_length(lp_wdt_dev_t *hw, wdt_reset_sig_length_t length)
{
    // hw->config0.sys_reset_length = length;
}

/**
 * @brief Enable/Disable the RWDT flashboot mode.
 *
 * @param hw Start address of the peripheral registers.
 * @param enable True to enable RWDT flashboot mode, false to disable RWDT flashboot mode.
 *
 * @note Flashboot mode is independent and can trigger a WDT timeout event if the
 *       WDT's enable bit is set to 0. Flashboot mode for RWDT is automatically enabled
 *       on flashboot, and should be disabled by software when flashbooting completes.
 */
FORCE_INLINE_ATTR void lpwdt_ll_set_flashboot_en(lp_wdt_dev_t *hw, bool enable)
{
    // hw->config0.flashboot_mod_en = (enable) ? 1 : 0;
}

/**
 * @brief Enable/Disable the CPU0 to be reset on WDT_STAGE_ACTION_RESET_CPU
 *
 * @param hw Start address of the peripheral registers.
 * @param enable True to enable CPU0 to be reset, false to disable.
 */
FORCE_INLINE_ATTR void lpwdt_ll_set_procpu_reset_en(lp_wdt_dev_t *hw, bool enable)
{
    // hw->config0.procpu_reset_en = (enable) ? 1 : 0;
}

/**
 * @brief Enable/Disable the CPU1 to be reset on WDT_STAGE_ACTION_RESET_CPU
 *
 * @param hw Start address of the peripheral registers.
 * @param enable True to enable CPU1 to be reset, false to disable.
 */
FORCE_INLINE_ATTR void lpwdt_ll_set_appcpu_reset_en(lp_wdt_dev_t *hw, bool enable)
{
    // hw->config0.appcpu_reset_en = (enable) ? 1 : 0;
}

/**
 * @brief Enable/Disable the RWDT pause during sleep functionality
 *
 * @param hw Start address of the peripheral registers.
 * @param enable True to enable, false to disable.
 */
FORCE_INLINE_ATTR void lpwdt_ll_set_pause_in_sleep_en(lp_wdt_dev_t *hw, bool enable)
{
    // hw->config0.pause_in_slp = (enable) ? 1 : 0;
}

/**
 * @brief Enable/Disable chip reset on RWDT timeout.
 *
 * A chip reset also resets the analog portion of the chip. It will appear as a
 * POWERON reset rather than an RTC reset.
 *
 * @param hw Start address of the peripheral registers.
 * @param enable True to enable, false to disable.
 */
FORCE_INLINE_ATTR void lpwdt_ll_set_chip_reset_en(lp_wdt_dev_t *hw, bool enable)
{
    // hw->config5.chip_reset_en = (enable) ? 1 : 0;
}

/**
 * @brief Set width of chip reset signal
 *
 * @param hw Start address of the peripheral registers.
 * @param width Width of chip reset signal in terms of number of RTC_SLOW_CLK cycles
 */
FORCE_INLINE_ATTR void lpwdt_ll_set_chip_reset_width(lp_wdt_dev_t *hw, uint32_t width)
{
    // ESP32H2-TODO: IDF-6402
    // HAL_FORCE_MODIFY_U32_REG_FIELD(hw->config0, chip_reset_width, width);
}

/**
 * @brief Feed the RWDT
 *
 * Resets the current timer count and current stage.
 *
 * @param hw Start address of the peripheral registers.
 */
FORCE_INLINE_ATTR void lpwdt_ll_feed(lp_wdt_dev_t *hw)
{
    // ESP32H2-TODO: IDF-6402
    // hw->feed.rtc_wdt_feed = 1;
}

/**
 * @brief Enable write protection of the RWDT registers
 *
 * @param hw Start address of the peripheral registers.
 */
FORCE_INLINE_ATTR void lpwdt_ll_write_protect_enable(lp_wdt_dev_t *hw)
{
    // ESP32H2-TODO: IDF-6402
    // hw->wprotect = 0;
}

/**
 * @brief Disable write protection of the RWDT registers
 *
 * @param hw Start address of the peripheral registers.
 */
FORCE_INLINE_ATTR void lpwdt_ll_write_protect_disable(lp_wdt_dev_t *hw)
{
    // ESP32H2-TODO: IDF-6402
    // hw->wprotect = RTC_CNTL_WDT_WKEY_VALUE;
}

/**
 * @brief Enable the RWDT interrupt.
 *
 * @param hw Start address of the peripheral registers.
 * @param enable True to enable RWDT interrupt, false to disable.
 */
FORCE_INLINE_ATTR void lpwdt_ll_set_intr_enable(lp_wdt_dev_t *hw, bool enable)
{
    // ESP32H2-TODO: IDF-6402
    // hw->int_ena.lp_wdt_int_ena = (enable) ? 1 : 0;
}

/**
 * @brief Check if the RWDT interrupt has been triggered
 *
 * @param hw Start address of the peripheral registers.
 * @return True if the RWDT interrupt was triggered
 */
FORCE_INLINE_ATTR bool lpwdt_ll_check_intr_status(lp_wdt_dev_t *hw)
{
    return false;//(hw->int_st.lp_wdt_int_st) ? true : false; // ESP32H2-TODO: IDF-6402
}

/**
 * @brief Clear the RWDT interrupt status.
 *
 * @param hw Start address of the peripheral registers.
 */
FORCE_INLINE_ATTR void lpwdt_ll_clear_intr_status(lp_wdt_dev_t *hw)
{
    // ESP32H2-TODO: IDF-6402
    // hw->int_clr.lp_wdt_int_clr = 1;
}

#ifdef __cplusplus
}
#endif
