/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "logging_util.h"
#include "esp_log.h"

static const char *TAG = "logging_util";

void log_info(const char *message)
{
    ESP_LOGI(TAG, "%s", message);
}

void log_warn(const char *message)
{
    ESP_LOGW(TAG, "%s", message);
}

void log_error(const char *message)
{
    ESP_LOGE(TAG, "%s", message);
}
