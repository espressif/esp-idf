/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "esp_err.h"

typedef const char*  esp_event_base_t; /**< unique pointer to a subsystem that exposes events */
typedef unsigned long TickType_t;

#define ESP_EVENT_DECLARE_BASE(id) extern esp_event_base_t id
#define ESP_EVENT_DEFINE_BASE(id) esp_event_base_t id = #id

esp_err_t esp_event_loop_create_default(void);

esp_err_t esp_event_post(esp_event_base_t event_base, int32_t event_id,
        const void* event_data, size_t event_data_size, TickType_t ticks_to_wait);
