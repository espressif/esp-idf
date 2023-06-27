/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "test_flash_utils.h"

const esp_partition_t *get_test_flash_partition(void)
{
    /* This finds "flash_test" partition defined in custom partitions.csv */
    const esp_partition_t *result = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
            ESP_PARTITION_SUBTYPE_ANY, "flash_test");
    assert(result != NULL); /* means partition table set wrong */
    return result;
}
