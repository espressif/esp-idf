/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdatomic.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "cmd_twai.h"
#include "esp_log.h"
#include "esp_console.h"
#include "cmd_twai_internal.h"

static const char *TAG = "cmd_twai";

twai_controller_ctx_t g_twai_controller_ctx[SOC_TWAI_CONTROLLER_NUM];

/* =============================================================================
 * COMMAND REGISTRATION
 * =============================================================================*/

twai_controller_ctx_t* get_controller_by_id(int controller_id)
{
    if (controller_id < 0 || controller_id >= SOC_TWAI_CONTROLLER_NUM) {
        ESP_LOGE(TAG, "Invalid controller ID: %d (valid range: 0-%d)",
                 controller_id, SOC_TWAI_CONTROLLER_NUM - 1);
        return NULL;
    }
    return &g_twai_controller_ctx[controller_id];
}

void register_twai_commands(void)
{
    register_twai_core_commands();
    register_twai_send_commands();
    register_twai_dump_commands();
    ESP_LOGI(TAG, "TWAI commands registered successfully");
}

void unregister_twai_commands(void)
{
    unregister_twai_dump_commands();
    unregister_twai_send_commands();
    unregister_twai_core_commands();
    ESP_LOGI(TAG, "TWAI commands unregistered successfully");
}
