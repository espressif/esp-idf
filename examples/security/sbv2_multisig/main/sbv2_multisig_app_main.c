/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* Secure Boot V2 Multisig Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "esp_secure_boot.h"

static const char* TAG = "sbv2_multisig_app";

void app_main(void)
{
    bool restart_required = false;
    bool sb_release_mode = esp_secure_boot_cfg_verify_release_mode();
    if (!sb_release_mode) {
        restart_required = true;
        ESP_LOGI(TAG, "Secure Boot V2 not in Release mode\nSetting Release mode...");
        esp_efuse_batch_write_begin();
        if (esp_secure_boot_enable_secure_features() == ESP_OK) {
            esp_efuse_batch_write_commit();
            sb_release_mode = esp_secure_boot_cfg_verify_release_mode();
        } else {
            esp_efuse_batch_write_cancel();
        }

        if (!sb_release_mode) {
            ESP_LOGE(TAG, "Failed to set Secure Boot V2 to Release mode");
        }
    }

    if (restart_required) {
        ESP_LOGI(TAG, "Restarting now (5 seconds)...");
        vTaskDelay(pdMS_TO_TICKS(5000));
        esp_restart();
    }

    if (sb_release_mode) {
        ESP_LOGI(TAG, "Secure Boot is enabled in Release Mode");
    } else {
        ESP_LOGE(TAG, "Secure Boot is not enabled in Release mode");
    }
}
