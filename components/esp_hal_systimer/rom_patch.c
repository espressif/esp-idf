/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief ROM patch for systimer HAL
 *
 * Some chips have systimer HAL implementations in ROM that require patches.
 * This file provides the necessary patches when ROM implementation is used.
 */

#include <stddef.h>
#include "esp_rom_caps.h"
#include "hal/systimer_hal.h"
#include "hal/systimer_ll.h"

#if !CONFIG_IDF_TARGET_ESP32C2 // esp32c2 has dedicated ROM patch

extern void rom_systimer_hal_init(systimer_hal_context_t *hal);
extern void rom_systimer_hal_deinit(systimer_hal_context_t *hal);

void systimer_hal_init(systimer_hal_context_t *hal)
{
    // For chips with ROM systimer that does not enable ETM, the ROM functions are
    // exposed with "rom_" prefix in rom.systimer.ld. We wrap them here to add
    // the missing systimer_ll_enable_etm() call.
    rom_systimer_hal_init(hal);
    systimer_ll_enable_etm(&SYSTIMER, true);
}

void systimer_hal_deinit(systimer_hal_context_t *hal)
{
    systimer_ll_enable_etm(&SYSTIMER, false);
    rom_systimer_hal_deinit(hal);
}
#endif // !CONFIG_IDF_TARGET_ESP32C2
