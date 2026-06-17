/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include "esp_rom_caps.h"
#include "hal/mwdt_periph.h"
#include "hal/wdt_hal.h"

#if ESP_ROM_WDT_INIT_PATCH
extern void rom_wdt_hal_init(wdt_hal_context_t *hal, wdt_inst_t wdt_inst, uint32_t prescaler, bool enable_intr);
extern void rom_wdt_hal_deinit(wdt_hal_context_t *hal);

void wdt_hal_init(wdt_hal_context_t *hal, wdt_inst_t wdt_inst, uint32_t prescaler, bool enable_intr)
{
    // ROM version omits clock source config for MWDT — delegate to ROM then fix up
    rom_wdt_hal_init(hal, wdt_inst, prescaler, enable_intr);

    if (hal->inst != WDT_RWDT) {
        mwdt_ll_write_protect_disable(hal->mwdt_dev);
        mwdt_ll_set_clock_source(hal->mwdt_dev, MWDT_CLK_SRC_DEFAULT);
        mwdt_ll_enable_clock(hal->mwdt_dev, true);
        mwdt_ll_write_protect_enable(hal->mwdt_dev);
    }
}

void wdt_hal_deinit(wdt_hal_context_t *hal)
{
    // ROM version omits mwdt_ll_enable_clock(false) — delegate to ROM then fix up
    if (hal->inst != WDT_RWDT) {
        mwdt_ll_write_protect_disable(hal->mwdt_dev);
        mwdt_ll_enable_clock(hal->mwdt_dev, false);
        mwdt_ll_write_protect_enable(hal->mwdt_dev);
    }
    rom_wdt_hal_deinit(hal);
}

#if ESP_ROM_WDT_CONFIG_STAGE_PATCH
extern void rom_wdt_hal_config_stage(wdt_hal_context_t *hal, wdt_stage_t stage, uint32_t timeout, wdt_stage_action_t behavior);

void wdt_hal_config_stage(wdt_hal_context_t *hal, wdt_stage_t stage, uint32_t timeout_ticks, wdt_stage_action_t behavior)
{
    rom_wdt_hal_config_stage(hal, stage, timeout_ticks, behavior);
    if (hal->inst == WDT_RWDT) {
        rwdt_ll_config_stage(hal->rwdt_dev, stage, timeout_ticks, behavior);
    }
}
#endif // ESP_ROM_WDT_CONFIG_STAGE_PATCH
#endif // ESP_ROM_WDT_INIT_PATCH
