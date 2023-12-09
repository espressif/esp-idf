/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
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
    if (esp_rom_km_huk_conf(huk_mode, huk_info_buf) == ETS_OK) {
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }
}

uint8_t huk_hal_get_risk_level(void)
{
    return (uint8_t) esp_rom_km_huk_risk();
}

uint32_t huk_hal_get_date_info(void)
{
    return huk_ll_get_date_info();
}
