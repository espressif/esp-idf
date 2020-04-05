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

#include <string.h>
#include <stdbool.h>
#include "hal/wdt_types.h"
#include "hal/wdt_hal.h"

/* ---------------------------- Init and Config ----------------------------- */

void wdt_hal_init(wdt_hal_context_t *hal, wdt_inst_t wdt_inst, uint32_t prescaler, bool enable_intr)
{
    //Initialize HAL context
    memset(hal, 0, sizeof(wdt_hal_context_t));
    if (wdt_inst == WDT_MWDT0) {
        hal->mwdt_dev = &TIMERG0;
    } else if (wdt_inst == WDT_MWDT1) {
        hal->mwdt_dev = &TIMERG1;
    } else {
        hal->rwdt_dev = &RTCCNTL;
    }
    hal->inst = wdt_inst;

    if (hal->inst == WDT_RWDT) {
        //Unlock RTC WDT
        rwdt_ll_write_protect_disable(hal->rwdt_dev);
        //Disable RTC WDT, all stages, and all interrupts.
        rwdt_ll_disable(hal->rwdt_dev);
        rwdt_ll_disable_stage(hal->rwdt_dev, WDT_STAGE0);
        rwdt_ll_disable_stage(hal->rwdt_dev, WDT_STAGE1);
        rwdt_ll_disable_stage(hal->rwdt_dev, WDT_STAGE2);
        rwdt_ll_disable_stage(hal->rwdt_dev, WDT_STAGE3);
#ifdef CONFIG_IDF_TARGET_ESP32
        //Enable or disable level interrupt. Edge interrupt is always disabled.
        rwdt_ll_set_edge_intr(hal->rwdt_dev, false);
        rwdt_ll_set_level_intr(hal->rwdt_dev, enable_intr);
#else   //CONFIG_IDF_TARGET_ESP32S2BETA
        //Enable or disable chip reset on timeout, and length of chip reset signal
        rwdt_ll_set_chip_reset_width(hal->rwdt_dev, 0);
        rwdt_ll_set_chip_reset_en(hal->rwdt_dev, false);
#endif
        rwdt_ll_clear_intr_status(hal->rwdt_dev);
        rwdt_ll_set_intr_enable(hal->rwdt_dev, enable_intr);
        //Set default values
        rwdt_ll_set_appcpu_reset_en(hal->rwdt_dev, true);
        rwdt_ll_set_procpu_reset_en(hal->rwdt_dev, true);
        rwdt_ll_set_pause_in_sleep_en(hal->rwdt_dev, true);
        rwdt_ll_set_cpu_reset_length(hal->rwdt_dev, WDT_RESET_SIG_LENGTH_3_2us);
        rwdt_ll_set_sys_reset_length(hal->rwdt_dev, WDT_RESET_SIG_LENGTH_3_2us);
        //Lock RTC WDT
        rwdt_ll_write_protect_enable(hal->rwdt_dev);
    } else {
        //Unlock WDT
        mwdt_ll_write_protect_disable(hal->mwdt_dev);
        //Disable WDT and stages.
        mwdt_ll_disable(hal->mwdt_dev);
        mwdt_ll_disable_stage(hal->mwdt_dev, 0);
        mwdt_ll_disable_stage(hal->mwdt_dev, 1);
        mwdt_ll_disable_stage(hal->mwdt_dev, 2);
        mwdt_ll_disable_stage(hal->mwdt_dev, 3);
        //Enable or disable level interrupt. Edge interrupt is always disabled.
        mwdt_ll_set_edge_intr(hal->mwdt_dev, false);
        mwdt_ll_set_level_intr(hal->mwdt_dev, enable_intr);
        mwdt_ll_clear_intr_status(hal->mwdt_dev);
        mwdt_ll_set_intr_enable(hal->mwdt_dev, enable_intr);
        //Set default values
        mwdt_ll_set_cpu_reset_length(hal->mwdt_dev, WDT_RESET_SIG_LENGTH_3_2us);
        mwdt_ll_set_sys_reset_length(hal->mwdt_dev, WDT_RESET_SIG_LENGTH_3_2us);
        //Set tick period
        mwdt_ll_set_prescaler(hal->mwdt_dev, prescaler);
        //Lock WDT
        mwdt_ll_write_protect_enable(hal->mwdt_dev);
    }
}

void wdt_hal_deinit(wdt_hal_context_t *hal)
{
    if (hal->inst == WDT_RWDT) {
        //Unlock WDT
        rwdt_ll_write_protect_disable(hal->rwdt_dev);
        //Disable WDT and clear any interrupts
        rwdt_ll_feed(hal->rwdt_dev);
        rwdt_ll_disable(hal->rwdt_dev);
        rwdt_ll_clear_intr_status(hal->rwdt_dev);
        rwdt_ll_set_intr_enable(hal->rwdt_dev, false);
        //Lock WDT
        rwdt_ll_write_protect_enable(hal->rwdt_dev);
    } else {
        //Unlock WDT
        mwdt_ll_write_protect_disable(hal->mwdt_dev);
        //Disable WDT and clear/disable any interrupts
        mwdt_ll_feed(hal->mwdt_dev);
        mwdt_ll_disable(hal->mwdt_dev);
        mwdt_ll_clear_intr_status(hal->mwdt_dev);
        mwdt_ll_set_intr_enable(hal->mwdt_dev, false);
        //Lock WDT
        mwdt_ll_write_protect_enable(hal->mwdt_dev);
    }
    //Deinit HAL context
    hal->mwdt_dev = NULL;
}

void wdt_hal_config_stage(wdt_hal_context_t *hal, wdt_stage_t stage, uint32_t timeout_ticks, wdt_stage_action_t behavior)
{
    if (hal->inst == WDT_RWDT) {
        rwdt_ll_config_stage(hal->rwdt_dev, stage, timeout_ticks, behavior);
    } else {
        mwdt_ll_config_stage(hal->mwdt_dev, stage, timeout_ticks, behavior);
    }
}

/* -------------------------------- Runtime --------------------------------- */

void wdt_hal_write_protect_disable(wdt_hal_context_t *hal)
{
    if (hal->inst == WDT_RWDT) {
        rwdt_ll_write_protect_disable(hal->rwdt_dev);
    } else {
        mwdt_ll_write_protect_disable(hal->mwdt_dev);
    }
}

void wdt_hal_write_protect_enable(wdt_hal_context_t *hal)
{
    if (hal->inst == WDT_RWDT) {
        rwdt_ll_write_protect_enable(hal->rwdt_dev);
    } else {
        mwdt_ll_write_protect_enable(hal->mwdt_dev);
    }
}

void wdt_hal_enable(wdt_hal_context_t *hal)
{
    if (hal->inst == WDT_RWDT) {
        rwdt_ll_feed(hal->rwdt_dev);
        rwdt_ll_enable(hal->rwdt_dev);
    } else {
        mwdt_ll_feed(hal->mwdt_dev);
        mwdt_ll_enable(hal->mwdt_dev);
    }
}

void wdt_hal_disable(wdt_hal_context_t *hal)
{
    if (hal->inst == WDT_RWDT) {
        rwdt_ll_disable(hal->rwdt_dev);
    } else {
        mwdt_ll_disable(hal->mwdt_dev);
    }
}

void wdt_hal_handle_intr(wdt_hal_context_t *hal)
{
    if (hal->inst == WDT_RWDT) {
        rwdt_ll_feed(hal->rwdt_dev);
        rwdt_ll_clear_intr_status(hal->rwdt_dev);
    } else {
        mwdt_ll_feed(hal->mwdt_dev);
        mwdt_ll_clear_intr_status(hal->mwdt_dev);
    }
}

void wdt_hal_feed(wdt_hal_context_t *hal)
{
    if (hal->inst == WDT_RWDT) {
        rwdt_ll_feed(hal->rwdt_dev);
    } else {
        mwdt_ll_feed(hal->mwdt_dev);
    }
}

void wdt_hal_set_flashboot_en(wdt_hal_context_t *hal, bool enable)
{
    if (hal->inst == WDT_RWDT) {
        rwdt_ll_set_flashboot_en(hal->rwdt_dev, enable);
    } else {
        mwdt_ll_set_flashboot_en(hal->mwdt_dev, enable);
    }
}

bool wdt_hal_is_enabled(wdt_hal_context_t *hal)
{
    if (hal->inst == WDT_RWDT) {
        return rwdt_ll_check_if_enabled(hal->rwdt_dev);
    } else {
        return mwdt_ll_check_if_enabled(hal->mwdt_dev);
    }
}

