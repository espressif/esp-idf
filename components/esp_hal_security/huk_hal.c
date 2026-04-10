/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
// The HAL layer for Hardware Unique Key(HUK) Generator

#include "hal/huk_hal.h"
#include "hal/huk_ll.h"
#include "hal/huk_types.h"
#include "hal/assert.h"
#include "hal/log.h"
#include "rom/km.h"
#include "esp_err.h"
#include "soc/soc_caps.h"

#if SOC_KEY_MANAGER_SUPPORTED
esp_huk_state_t huk_hal_get_state(void)
{
    return huk_ll_get_state();
}

static void inline huk_hal_wait_for_state(esp_huk_state_t state)
{
    while (huk_ll_get_state() != state) {
        ;
    }
}

esp_err_t huk_hal_configure(const esp_huk_mode_t huk_mode, uint8_t *huk_info_buf)
{
    huk_ll_power_up();

    if (esp_rom_km_huk_conf(huk_mode, huk_info_buf) != ETS_OK) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

uint8_t huk_hal_get_risk_level(void)
{
    return (uint8_t) esp_rom_km_huk_risk();
}

uint32_t huk_hal_get_date_info(void)
{
    return huk_ll_get_date_info();
}

#if SOC_HUK_MEM_NEEDS_RECHARGE
void huk_hal_recharge_huk_memory(void)
{
    huk_ll_recharge_huk_memory();
}
#endif
#endif
