/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdbool.h>
#include <assert.h>
#include "soc/soc.h"
#include "soc/lp_analog_peri_reg.h"
// TODO: [ESP32C5] IDF-8667 remove esp_log.h
#include "esp_log.h"

void bootloader_ana_super_wdt_reset_config(bool enable)
{
    // TODO: [ESP32C5] IDF-8667
    ESP_EARLY_LOGW("bootloader", "bootloader_ana_super_wdt_reset_config() has not been implemented on C5 yet");
}

void bootloader_ana_bod_reset_config(bool enable)
{
     // TODO: [ESP32C5] IDF-8667
    ESP_EARLY_LOGW("bootloader", "bootloader_ana_bod_reset_config() has not been implemented on C5 yet");
}

//Not supported but common bootloader calls the function. Do nothing
void bootloader_ana_clock_glitch_reset_config(bool enable)
{
     // TODO: [ESP32C5] IDF-8667
    (void)enable;
}
