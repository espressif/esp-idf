/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 *
 * Zigbee RCP Example
 *
 * This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "zb_scheduler.h"
#include "esp_zigbee_rcp.h"

#if (defined ZB_MACSPLIT_HOST && !defined ZB_MACSPLIT_DEVICE)
#error Only Zigbee rcp device should be defined
#endif
static const char *TAG = "ESP_ZB_RCP";

void esp_zb_app_signal_handler(esp_zb_app_signal_t *signal_struct)
{
    uint32_t *p_sg_p       = signal_struct->p_app_signal;
    esp_err_t err_status = signal_struct->esp_err_status;
    esp_zb_app_signal_type_t sig_type = *p_sg_p;
    if (err_status == ESP_OK) {
    } else if (sig_type == ESP_ZB_ZDO_SIGNAL_PRODUCTION_CONFIG_READY) {
        ESP_LOGI(TAG, "Production config is not present or invalid");
    } else {
        ESP_LOGI(TAG, "Device started FAILED status %d", err_status);
    }
}

static void esp_zb_task(void *pvParameters)
{
    esp_zb_rcp_init();
    esp_zb_rcp_main_loop_iteration();
}

void app_main(void)
{
    esp_zb_platform_config_t config = {
        .radio_config = ESP_ZB_DEFAULT_RADIO_CONFIG(),
        .host_config = ESP_ZB_DEFAULT_HOST_CONFIG(),
    };
    ESP_ERROR_CHECK(nvs_flash_init());
    /* load Zigbee rcp platform config to initialization */
    esp_zb_macsplit_set_version(RCP_COMPILE_DEFINE);
    ESP_ERROR_CHECK(esp_zb_platform_config(&config));
    xTaskCreate(esp_zb_task, "Zigbee_main", 4096, NULL, 5, NULL);
}
