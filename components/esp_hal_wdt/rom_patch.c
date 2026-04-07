/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include "hal/config.h"
#include "soc/soc_caps.h"
#include "soc/chip_revision.h"
#include "esp_rom_caps.h"
#include "hal/efuse_hal.h"
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

#if SOC_IS(ESP32P4) && (HAL_CONFIG(CHIP_SUPPORT_MIN_REV) <= 301)
extern void rom_wdt_hal_config_stage(wdt_hal_context_t *hal, wdt_stage_t stage, uint32_t timeout, wdt_stage_action_t behavior);

/* rwdt_ll_config_stage is implemented erroneously in ESP32P4 rom code, TODO: PM-654*/
void wdt_hal_config_stage(wdt_hal_context_t *hal, wdt_stage_t stage, uint32_t timeout_ticks, wdt_stage_action_t behavior)
{
    if ((hal->inst == WDT_RWDT && stage == WDT_STAGE0) && !ESP_CHIP_REV_ABOVE(efuse_hal_chip_revision(), 302)) {
        timeout_ticks = timeout_ticks >> (1 + REG_GET_FIELD(EFUSE_RD_REPEAT_DATA1_REG, EFUSE_WDT_DELAY_SEL));
    }
    rom_wdt_hal_config_stage(hal, stage, timeout_ticks, behavior);
}
#endif // SOC_IS(ESP32P4)
#endif // ESP_ROM_WDT_INIT_PATCH
