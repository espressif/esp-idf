// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// The LL layer for Timer Group register operations.
// Note that most of the register operations in this layer are non-atomic operations.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdbool.h>
#include "hal/wdt_types.h"
#include "soc/rtc_cntl_periph.h"
#include "soc/rtc_cntl_struct.h"
#include "esp_attr.h"

//Type check wdt_stage_action_t
_Static_assert(WDT_STAGE_ACTION_OFF == RTC_WDT_STG_SEL_OFF, "Add mapping to LL watchdog timeout behavior, since it's no longer naturally compatible with wdt_stage_action_t");
_Static_assert(WDT_STAGE_ACTION_INT == RTC_WDT_STG_SEL_INT, "Add mapping to LL watchdog timeout behavior, since it's no longer naturally compatible with wdt_stage_action_t");
_Static_assert(WDT_STAGE_ACTION_RESET_CPU == RTC_WDT_STG_SEL_RESET_CPU, "Add mapping to LL watchdog timeout behavior, since it's no longer naturally compatible with wdt_stage_action_t");
_Static_assert(WDT_STAGE_ACTION_RESET_SYSTEM == RTC_WDT_STG_SEL_RESET_SYSTEM, "Add mapping to LL watchdog timeout behavior, since it's no longer naturally compatible with wdt_stage_action_t");
_Static_assert(WDT_STAGE_ACTION_RESET_RTC == RTC_WDT_STG_SEL_RESET_RTC, "Add mapping to LL watchdog timeout behavior, since it's no longer naturally compatible with wdt_stage_action_t");
//Type check wdt_reset_sig_length_t
_Static_assert(WDT_RESET_SIG_LENGTH_100ns == RTC_WDT_RESET_LENGTH_100_NS, "Add mapping to LL watchdog timeout behavior, since it's no longer naturally compatible with wdt_reset_sig_length_t");
_Static_assert(WDT_RESET_SIG_LENGTH_200ns == RTC_WDT_RESET_LENGTH_200_NS, "Add mapping to LL watchdog timeout behavior, since it's no longer naturally compatible with wdt_reset_sig_length_t");
_Static_assert(WDT_RESET_SIG_LENGTH_300ns == RTC_WDT_RESET_LENGTH_300_NS, "Add mapping to LL watchdog timeout behavior, since it's no longer naturally compatible with wdt_reset_sig_length_t");
_Static_assert(WDT_RESET_SIG_LENGTH_400ns == RTC_WDT_RESET_LENGTH_400_NS, "Add mapping to LL watchdog timeout behavior, since it's no longer naturally compatible with wdt_reset_sig_length_t");
_Static_assert(WDT_RESET_SIG_LENGTH_500ns == RTC_WDT_RESET_LENGTH_500_NS, "Add mapping to LL watchdog timeout behavior, since it's no longer naturally compatible with wdt_reset_sig_length_t");
_Static_assert(WDT_RESET_SIG_LENGTH_800ns == RTC_WDT_RESET_LENGTH_800_NS, "Add mapping to LL watchdog timeout behavior, since it's no longer naturally compatible with wdt_reset_sig_length_t");
_Static_assert(WDT_RESET_SIG_LENGTH_1_6us == RTC_WDT_RESET_LENGTH_1600_NS, "Add mapping to LL watchdog timeout behavior, since it's no longer naturally compatible with wdt_reset_sig_length_t");
_Static_assert(WDT_RESET_SIG_LENGTH_3_2us == RTC_WDT_RESET_LENGTH_3200_NS, "Add mapping to LL watchdog timeout behavior, since it's no longer naturally compatible with wdt_reset_sig_length_t");


/**
 * @brief Enable the RWDT
 *
 * @param hw Start address of the peripheral registers.
 */
FORCE_INLINE_ATTR void rwdt_ll_enable(rtc_cntl_dev_t *hw)
{
    hw->wdt_config0.en = 1;
}

/**
 * @brief Disable the RWDT
 *
 * @param hw Start address of the peripheral registers.
 * @note This function does not disable the flashboot mode. Therefore, given that
 *       the MWDT is disabled using this function, a timeout can still occur
 *       if the flashboot mode is simultaneously enabled.
 */
FORCE_INLINE_ATTR void rwdt_ll_disable(rtc_cntl_dev_t *hw)
{
    hw->wdt_config0.en = 0;
}

/**
 * @brief Check if the RWDT is enabled
 *
 * @param hw Start address of the peripheral registers.
 * @return True if RTC WDT is enabled
 */
FORCE_INLINE_ATTR bool rwdt_ll_check_if_enabled(rtc_cntl_dev_t *hw)
{
    return (hw->wdt_config0.en) ? true : false;
}

/**
 * @brief Configure a particular stage of the RWDT
 *
 * @param hw Start address of the peripheral registers.
 * @param stage Which stage to configure
 * @param timeout Number of timer ticks for the stage to timeout
 * @param behavior What action to take when the stage times out
 */
FORCE_INLINE_ATTR void rwdt_ll_config_stage(rtc_cntl_dev_t *hw, wdt_stage_t stage, uint32_t timeout_ticks, wdt_stage_action_t behavior)
{
    switch (stage) {
        case WDT_STAGE0:
            hw->wdt_config0.stg0 = behavior;
            hw->wdt_config1 = timeout_ticks;
            break;
        case WDT_STAGE1:
            hw->wdt_config0.stg1 = behavior;
            hw->wdt_config2 = timeout_ticks;
            break;
        case WDT_STAGE2:
            hw->wdt_config0.stg2 = behavior;
            hw->wdt_config3 = timeout_ticks;
            break;
        case WDT_STAGE3:
            hw->wdt_config0.stg3 = behavior;
            hw->wdt_config4 = timeout_ticks;
            break;
        default:
            abort();
    }
}

/**
 * @brief Disable a particular stage of the RWDT
 *
 * @param hw Start address of the peripheral registers.
 * @param stage Which stage to disable
 */
FORCE_INLINE_ATTR void rwdt_ll_disable_stage(rtc_cntl_dev_t *hw, wdt_stage_t stage)
{
    switch (stage) {
        case WDT_STAGE0:
            hw->wdt_config0.stg0 = WDT_STAGE_ACTION_OFF;
            break;
        case WDT_STAGE1:
            hw->wdt_config0.stg1 = WDT_STAGE_ACTION_OFF;
            break;
        case WDT_STAGE2:
            hw->wdt_config0.stg2 = WDT_STAGE_ACTION_OFF;
            break;
        case WDT_STAGE3:
            hw->wdt_config0.stg3 = WDT_STAGE_ACTION_OFF;
            break;
        default:
            abort();
    }
}

/**
 * @brief Enable or disable RWDT edge interrupt
 *
 * @param hw Start address of the peripheral registers.
 * @param enable Whether to enable edge interrupt
 */
FORCE_INLINE_ATTR void rwdt_ll_set_edge_intr(rtc_cntl_dev_t *hw, bool enable)
{
    hw->wdt_config0.edge_int_en = (enable) ? 1 : 0;
}

/**
 * @brief Enable or disable RWDT level interrupt
 *
 * @param hw Start address of the peripheral registers.
 * @param enable Whether to enable level interrupt
 */
FORCE_INLINE_ATTR void rwdt_ll_set_level_intr(rtc_cntl_dev_t *hw, bool enable)
{
    hw->wdt_config0.level_int_en = (enable) ? 1 : 0;
}

/**
 * @brief Set the length of the CPU reset action
 *
 * @param hw Start address of the peripheral registers.
 * @param length Length of CPU reset signal
 */
FORCE_INLINE_ATTR void rwdt_ll_set_cpu_reset_length(rtc_cntl_dev_t *hw, wdt_reset_sig_length_t length)
{
    hw->wdt_config0.cpu_reset_length = length;
}

/**
 * @brief Set the length of the system reset action
 *
 * @param hw Start address of the peripheral registers.
 * @param length Length of system reset signal
 */
FORCE_INLINE_ATTR void rwdt_ll_set_sys_reset_length(rtc_cntl_dev_t *hw, wdt_reset_sig_length_t length)
{
    hw->wdt_config0.sys_reset_length = length;
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
FORCE_INLINE_ATTR void rwdt_ll_set_flashboot_en(rtc_cntl_dev_t* hw, bool enable)
{
    hw->wdt_config0.flashboot_mod_en = (enable) ? 1 : 0;
}

/**
 * @brief Enable/Disable the CPU0 to be reset on WDT_STAGE_ACTION_RESET_CPU
 *
 * @param hw Start address of the peripheral registers.
 * @param enable True to enable CPU0 to be reset, false to disable.
 */
FORCE_INLINE_ATTR void rwdt_ll_set_procpu_reset_en(rtc_cntl_dev_t* hw, bool enable)
{
    hw->wdt_config0.procpu_reset_en = (enable) ? 1 : 0;
}

/**
 * @brief Enable/Disable the CPU1 to be reset on WDT_STAGE_ACTION_RESET_CPU
 *
 * @param hw Start address of the peripheral registers.
 * @param enable True to enable CPU1 to be reset, false to disable.
 */
FORCE_INLINE_ATTR void rwdt_ll_set_appcpu_reset_en(rtc_cntl_dev_t* hw, bool enable)
{
    hw->wdt_config0.appcpu_reset_en = (enable) ? 1 : 0;
}

/**
 * @brief Enable/Disable the RWDT pause during sleep functionality
 *
 * @param hw Start address of the peripheral registers.
 * @param enable True to enable, false to disable.
 */
FORCE_INLINE_ATTR void rwdt_ll_set_pause_in_sleep_en(rtc_cntl_dev_t* hw, bool enable)
{
    hw->wdt_config0.pause_in_slp = (enable) ? 1 : 0;
}

/**
 * @brief Feed the RWDT
 *
 * Resets the current timer count and current stage.
 *
 * @param hw Start address of the peripheral registers.
 */
FORCE_INLINE_ATTR void rwdt_ll_feed(rtc_cntl_dev_t *hw)
{
    hw->wdt_feed.feed = 1;
}

/**
 * @brief Enable write protection of the RWDT registers
 *
 * @param hw Start address of the peripheral registers.
 */
FORCE_INLINE_ATTR void rwdt_ll_write_protect_enable(rtc_cntl_dev_t *hw)
{
    hw->wdt_wprotect = 0;
}

/**
 * @brief Disable write protection of the RWDT registers
 *
 * @param hw Start address of the peripheral registers.
 */
FORCE_INLINE_ATTR void rwdt_ll_write_protect_disable(rtc_cntl_dev_t *hw)
{
    hw->wdt_wprotect = RTC_CNTL_WDT_WKEY_VALUE;
}

/**
 * @brief Enable the RWDT interrupt.
 *
 * @param hw Start address of the peripheral registers.
 * @param enable True to enable RWDT interrupt, false to disable.
 */
FORCE_INLINE_ATTR void rwdt_ll_set_intr_enable(rtc_cntl_dev_t* hw, bool enable)
{
    hw->int_ena.rtc_wdt = (enable) ? 1 : 0;
}

/**
 * @brief Check if the RWDT interrupt has been triggered
 *
 * @param hw Start address of the peripheral registers.
 * @return True if the RWDT interrupt was triggered
 */
FORCE_INLINE_ATTR bool rwdt_ll_check_intr_status(rtc_cntl_dev_t *hw)
{
    return (hw->int_st.rtc_wdt) ? true : false;
}

/**
 * @brief Clear the RWDT interrupt status.
 *
 * @param hw Start address of the peripheral registers.
 */
FORCE_INLINE_ATTR void rwdt_ll_clear_intr_status(rtc_cntl_dev_t* hw)
{
    hw->int_clr.rtc_wdt = 1;
}

#ifdef __cplusplus
}
#endif
