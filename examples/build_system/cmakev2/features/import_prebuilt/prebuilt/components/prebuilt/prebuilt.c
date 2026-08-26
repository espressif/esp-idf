/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "esp_ota_ops.h"
#include "esp_partition.h"
#include "esp_log.h"

const char *TAG = "prebuilt";

void prebuilt_func(void)
{
    const esp_partition_t* running_partition = esp_ota_get_running_partition();
    ESP_LOGI(TAG, "The running partition is '%s'!", running_partition->label);
}
