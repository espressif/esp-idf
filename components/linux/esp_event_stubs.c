/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_err.h"
#include "esp_event.h"

esp_err_t esp_event_loop_create_default(void)
{
    return ESP_OK;
}

esp_err_t esp_event_post(esp_event_base_t event_base, int32_t event_id,
        const void* event_data, size_t event_data_size, TickType_t ticks_to_wait)
{
    return ESP_OK;
}
