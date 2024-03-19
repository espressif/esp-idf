/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "soc/soc.h"
#include "soc/pcr_reg.h"
#include "soc/pmu_reg.h"
#include "hal/regi2c_ctrl.h"
#include "esp_log.h"

void bootloader_random_enable(void)
{
// TODO: [ESP32C5] IDF-8626, IDF-9197
    ESP_EARLY_LOGW("bootloader_random", "bootloader_random_enable() has not been implemented on C5 yet");
}

void bootloader_random_disable(void)
{
// TODO: [ESP32C5] IDF-8626, IDF-9197
    ESP_EARLY_LOGW("bootloader_random", "bootloader_random_disable() has not been implemented on C5 yet");
}
