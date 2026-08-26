/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "esp_log.h"

void bootloader_extra_dir_function(void)
{
    ESP_LOGI("EXTRA", "This function is called from an extra component");
}
