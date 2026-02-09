/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include "soc/soc_caps.h"
#include "esp_rom_caps.h"
#include "hal/mwdt_periph.h"
#include "hal/wdt_hal.h"

#if ESP_ROM_WDT_INIT_PATCH
void wdt_hal_init(wdt_hal_context_t *hal, wdt_inst_t wdt_inst, uint32_t prescaler, bool enable_intr)
{
    //Initialize HAL context
    memset(hal, 0, sizeof(wdt_hal_context_t));
    if (wdt_inst == WDT_MWDT0) {
        hal->mwdt_dev = &TIMERG0;
    }
#if TIMG_LL_GET(INST_NUM) >= 2
    else if (wdt_inst == WDT_MWDT1) {
        hal->mwdt_dev = &TIMERG1;
    }
#endif
    else {
        hal->rwdt_dev = RWDT_DEV_GET();
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
#if SOC_IS(ESP32)
        //Enable or disable level interrupt. Edge interrupt is always disabled.
        rwdt_ll_set_edge_intr(hal->rwdt_dev, false);
        rwdt_ll_set_level_intr(hal->rwdt_dev, enable_intr);
#else
        //Enable or disable chip reset on timeout, and length of chip reset signal
        rwdt_ll_set_chip_reset_width(hal->rwdt_dev, 0);
        rwdt_ll_set_chip_reset_en(hal->rwdt_dev, false);
#endif
        rwdt_ll_clear_intr_status(hal->rwdt_dev);
        rwdt_ll_set_intr_enable(hal->rwdt_dev, enable_intr);
        //Set default values
#if SOC_CPU_CORES_NUM > 1
        rwdt_ll_set_appcpu_reset_en(hal->rwdt_dev, true);
#endif
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
#if SOC_IS(ESP32) || SOC_IS(ESP32S2) || SOC_IS(ESP32S3)
        //Enable or disable level interrupt. Edge interrupt is always disabled.
        mwdt_ll_set_edge_intr(hal->mwdt_dev, false);
        mwdt_ll_set_level_intr(hal->mwdt_dev, enable_intr);
#endif
        mwdt_ll_clear_intr_status(hal->mwdt_dev);
        mwdt_ll_set_intr_enable(hal->mwdt_dev, enable_intr);
        //Set default values
        mwdt_ll_set_cpu_reset_length(hal->mwdt_dev, WDT_RESET_SIG_LENGTH_3_2us);
        mwdt_ll_set_sys_reset_length(hal->mwdt_dev, WDT_RESET_SIG_LENGTH_3_2us);
        mwdt_ll_set_clock_source(hal->mwdt_dev, MWDT_CLK_SRC_DEFAULT);
        mwdt_ll_enable_clock(hal->mwdt_dev, true);
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
        mwdt_ll_enable_clock(hal->mwdt_dev, false);
        //Lock WDT
        mwdt_ll_write_protect_enable(hal->mwdt_dev);
    }
    //Deinit HAL context
    hal->mwdt_dev = NULL;
}

#if SOC_IS(ESP32P4)
extern void rom_wdt_hal_config_stage(wdt_hal_context_t *hal, wdt_stage_t stage, uint32_t timeout, wdt_stage_action_t behavior);

/* rwdt_ll_config_stage is implemented erroneously in ESP32P4 rom code, TODO: PM-654*/
void wdt_hal_config_stage(wdt_hal_context_t *hal, wdt_stage_t stage, uint32_t timeout_ticks, wdt_stage_action_t behavior)
{
    if (hal->inst == WDT_RWDT && stage == WDT_STAGE0) {
        timeout_ticks = timeout_ticks >> (1 + REG_GET_FIELD(EFUSE_RD_REPEAT_DATA1_REG, EFUSE_WDT_DELAY_SEL));
    }
    rom_wdt_hal_config_stage(hal, stage, timeout_ticks, behavior);
}
#endif // SOC_IS(ESP32P4)
#endif // ESP_ROM_WDT_INIT_PATCH
