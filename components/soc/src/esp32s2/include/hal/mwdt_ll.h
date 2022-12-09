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

#include <stdint.h>
#include <stdbool.h>
#include "soc/timer_periph.h"
#include "hal/wdt_types.h"
#include "esp_attr.h"

//Type check wdt_stage_action_t
_Static_assert(WDT_STAGE_ACTION_OFF == TIMG_WDT_STG_SEL_OFF, "Add mapping to LL watchdog timeout behavior, since it's no longer naturally compatible with wdt_stage_action_t");
_Static_assert(WDT_STAGE_ACTION_INT == TIMG_WDT_STG_SEL_INT, "Add mapping to LL watchdog timeout behavior, since it's no longer naturally compatible with wdt_stage_action_t");
_Static_assert(WDT_STAGE_ACTION_RESET_CPU == TIMG_WDT_STG_SEL_RESET_CPU, "Add mapping to LL watchdog timeout behavior, since it's no longer naturally compatible with wdt_stage_action_t");
_Static_assert(WDT_STAGE_ACTION_RESET_SYSTEM == TIMG_WDT_STG_SEL_RESET_SYSTEM, "Add mapping to LL watchdog timeout behavior, since it's no longer naturally compatible with wdt_stage_action_t");
//Type check wdt_reset_sig_length_t
_Static_assert(WDT_RESET_SIG_LENGTH_100ns == TIMG_WDT_RESET_LENGTH_100_NS, "Add mapping to LL watchdog timeout behavior, since it's no longer naturally compatible with wdt_reset_sig_length_t");
_Static_assert(WDT_RESET_SIG_LENGTH_200ns == TIMG_WDT_RESET_LENGTH_200_NS, "Add mapping to LL watchdog timeout behavior, since it's no longer naturally compatible with wdt_reset_sig_length_t");
_Static_assert(WDT_RESET_SIG_LENGTH_300ns == TIMG_WDT_RESET_LENGTH_300_NS, "Add mapping to LL watchdog timeout behavior, since it's no longer naturally compatible with wdt_reset_sig_length_t");
_Static_assert(WDT_RESET_SIG_LENGTH_400ns == TIMG_WDT_RESET_LENGTH_400_NS, "Add mapping to LL watchdog timeout behavior, since it's no longer naturally compatible with wdt_reset_sig_length_t");
_Static_assert(WDT_RESET_SIG_LENGTH_500ns == TIMG_WDT_RESET_LENGTH_500_NS, "Add mapping to LL watchdog timeout behavior, since it's no longer naturally compatible with wdt_reset_sig_length_t");
_Static_assert(WDT_RESET_SIG_LENGTH_800ns == TIMG_WDT_RESET_LENGTH_800_NS, "Add mapping to LL watchdog timeout behavior, since it's no longer naturally compatible with wdt_reset_sig_length_t");
_Static_assert(WDT_RESET_SIG_LENGTH_1_6us == TIMG_WDT_RESET_LENGTH_1600_NS, "Add mapping to LL watchdog timeout behavior, since it's no longer naturally compatible with wdt_reset_sig_length_t");
_Static_assert(WDT_RESET_SIG_LENGTH_3_2us == TIMG_WDT_RESET_LENGTH_3200_NS, "Add mapping to LL watchdog timeout behavior, since it's no longer naturally compatible with wdt_reset_sig_length_t");

/**
 * @brief Enable the MWDT
 *
 * @param hw Start address of the peripheral registers.
 */
FORCE_INLINE_ATTR void mwdt_ll_enable(timg_dev_t *hw)
{
    hw->wdt_config0.en = 1;
}

/**
 * @brief Disable the MWDT
 *
 * @param hw Start address of the peripheral registers.
 * @note This function does not disable the flashboot mode. Therefore, given that
 *       the MWDT is disabled using this function, a timeout can still occur
 *       if the flashboot mode is simultaneously enabled.
 */
FORCE_INLINE_ATTR void mwdt_ll_disable(timg_dev_t *hw)
{
    hw->wdt_config0.en = 0;
}

/**
 * Check if the MWDT is enabled
 *
 * @param hw Start address of the peripheral registers.
 * @return True if the MWDT is enabled, false otherwise
 */
FORCE_INLINE_ATTR bool mwdt_ll_check_if_enabled(timg_dev_t *hw)
{
    return (hw->wdt_config0.en) ? true : false;
}

/**
 * @brief Configure a particular stage of the MWDT
 *
 * @param hw Start address of the peripheral registers.
 * @param stage Which stage to configure
 * @param timeout Number of timer ticks for the stage to timeout
 * @param behavior What action to take when the stage times out
 */
FORCE_INLINE_ATTR void mwdt_ll_config_stage(timg_dev_t *hw, wdt_stage_t stage, uint32_t timeout, wdt_stage_action_t behavior)
{
    switch (stage) {
        case WDT_STAGE0:
            hw->wdt_config0.stg0 = behavior;
            hw->wdt_config2 = timeout;
            break;
        case WDT_STAGE1:
            hw->wdt_config0.stg1 = behavior;
            hw->wdt_config3 = timeout;
            break;
        case WDT_STAGE2:
            hw->wdt_config0.stg2 = behavior;
            hw->wdt_config4 = timeout;
            break;
        case WDT_STAGE3:
            hw->wdt_config0.stg3 = behavior;
            hw->wdt_config5 = timeout;
            break;
        default:
            break;
    }
}

/**
 * @brief Disable a particular stage of the MWDT
 *
 * @param hw Start address of the peripheral registers.
 * @param stage Which stage to disable
 */
FORCE_INLINE_ATTR void mwdt_ll_disable_stage(timg_dev_t *hw, uint32_t stage)
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
            break;
    }
}

/**
 * @brief Enable or disable MWDT edge interrupt
 *
 * @param hw Start address of the peripheral registers.
 * @param enable Whether to enable edge interrupt
 */
FORCE_INLINE_ATTR void mwdt_ll_set_edge_intr(timg_dev_t *hw, bool enable)
{
    hw->wdt_config0.edge_int_en = (enable) ? 1 : 0;
}

/**
 * @brief Enable or disable MWDT level interrupt
 *
 * @param hw Start address of the peripheral registers.
 * @param enable Whether to enable level interrupt
 */
FORCE_INLINE_ATTR void mwdt_ll_set_level_intr(timg_dev_t *hw, bool enable)
{
    hw->wdt_config0.level_int_en = (enable) ? 1 : 0;
}

/**
 * @brief Set the length of the CPU reset action
 *
 * @param hw Start address of the peripheral registers.
 * @param length Length of CPU reset signal
 */
FORCE_INLINE_ATTR void mwdt_ll_set_cpu_reset_length(timg_dev_t *hw, wdt_reset_sig_length_t length)
{
    hw->wdt_config0.cpu_reset_length = length;
}

/**
 * @brief Set the length of the system reset action
 *
 * @param hw Start address of the peripheral registers.
 * @param length Length of system reset signal
 */
FORCE_INLINE_ATTR void mwdt_ll_set_sys_reset_length(timg_dev_t *hw, wdt_reset_sig_length_t length)
{
    hw->wdt_config0.sys_reset_length = length;
}

/**
 * @brief Enable/Disable the MWDT flashboot mode.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param enable True to enable WDT flashboot mode, false to disable WDT flashboot mode.
 *
 * @note Flashboot mode is independent and can trigger a WDT timeout event if the
 *       WDT's enable bit is set to 0. Flashboot mode for TG0 is automatically enabled
 *       on flashboot, and should be disabled by software when flashbooting completes.
 */
FORCE_INLINE_ATTR void mwdt_ll_set_flashboot_en(timg_dev_t* hw, bool enable)
{
    hw->wdt_config0.flashboot_mod_en = (enable) ? 1 : 0;
}

/**
 * @brief Set the clock prescaler of the MWDT
 *
 * @param hw Start address of the peripheral registers.
 * @param prescaler Prescaler value between 1 to 65535
 */
FORCE_INLINE_ATTR void mwdt_ll_set_prescaler(timg_dev_t *hw, uint32_t prescaler)
{
    hw->wdt_config1.clk_prescale = prescaler;
}

/**
 * @brief Feed the MWDT
 *
 * Resets the current timer count and current stage.
 *
 * @param hw Start address of the peripheral registers.
 */
FORCE_INLINE_ATTR void mwdt_ll_feed(timg_dev_t *hw)
{
    hw->wdt_feed = 1;
}

/**
 * @brief Enable write protection of the MWDT registers
 *
 * Locking the MWDT will prevent any of the MWDT's registers from being modified
 *
 * @param hw Start address of the peripheral registers.
 */
FORCE_INLINE_ATTR void mwdt_ll_write_protect_enable(timg_dev_t *hw)
{
    hw->wdt_wprotect = 0;
}

/**
 * @brief Disable write protection of the MWDT registers
 *
 * @param hw Start address of the peripheral registers.
 */
FORCE_INLINE_ATTR void mwdt_ll_write_protect_disable(timg_dev_t *hw)
{
    hw->wdt_wprotect = TIMG_WDT_WKEY_VALUE;
}

/**
 * @brief Clear the MWDT interrupt status.
 *
 * @param hw Start address of the peripheral registers.
 */
FORCE_INLINE_ATTR void mwdt_ll_clear_intr_status(timg_dev_t* hw)
{
    hw->int_clr.wdt = 1;
}

/**
 * @brief Set the interrupt enable bit for the MWDT interrupt.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param enable Whether to enable the MWDT interrupt
 */
FORCE_INLINE_ATTR void mwdt_ll_set_intr_enable(timg_dev_t* hw, bool enable)
{
    hw->int_ena.wdt = (enable) ? 1 : 0;
}

#ifdef __cplusplus
}
#endif
