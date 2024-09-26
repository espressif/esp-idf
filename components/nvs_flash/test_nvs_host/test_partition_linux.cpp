/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_private/partition_linux.h"
#include "esp_partition.h"

uint32_t esp_partition_get_main_flash_sector_size(void)
{
    return ESP_PARTITION_EMULATED_SECTOR_SIZE;
}
