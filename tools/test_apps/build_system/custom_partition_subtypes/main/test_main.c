/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <esp_partition.h>
#include <esp_log.h>

static const char *TAG = "test_main";

void app_main(void)
{
    const esp_partition_t *my_app1 = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_MY_APP1, NULL);
    if (my_app1 == NULL) {
        ESP_LOGE(TAG, "Failed to find custom my_app1");
    } else {
        ESP_LOGI(TAG, "Found custom partition @ 0x%x (0x%x)", my_app1->address, my_app1->size);
    }

    const esp_partition_t *my_app2 = esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_MY_APP2, NULL);
    if (my_app2 == NULL) {
        ESP_LOGE(TAG, "Failed to find custom my_app1");
    } else {
        ESP_LOGI(TAG, "Found custom partition @ 0x%x (0x%x)", my_app2->address, my_app2->size);
    }
}
