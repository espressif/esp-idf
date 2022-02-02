/*
 * Copyright (c) 2021 Espressif Systems (Shanghai) CO LTD
 * All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Espressif Systems
 *    integrated circuit in a product or a software update for such product,
 *    must reproduce the above copyright notice, this list of conditions and
 *    the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * 4. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_zigbee_gateway.h"

#if (!defined ZB_MACSPLIT_HOST && defined ZB_MACSPLIT_DEVICE)
#error Only Zigbee gateway hostdevice should be defined
#endif

static const char *TAG = "ESP_ZB_GATEWAY";

/********************* Define functions **************************/
static void bdb_start_top_level_commissioning_cb(zb_uint8_t mode_mask)
{
    if (!bdb_start_top_level_commissioning(mode_mask)) {
        ESP_LOGE(TAG, "In BDB commissioning, an error occurred (for example: the device has already been running)");
    }
}

void zboss_signal_handler(zb_bufid_t bufid)
{
    zb_zdo_app_signal_hdr_t *p_sg_p       = NULL;
    zb_zdo_app_signal_type_t  sig         = zb_get_app_signal(bufid, &p_sg_p);
    zb_ret_t                  status      = ZB_GET_APP_SIGNAL_STATUS(bufid);
    zb_zdo_signal_device_annce_params_t *dev_annce_params = NULL;
    zb_zdo_signal_macsplit_dev_boot_params_t *rcp_version = NULL;
    zb_uint32_t    gateway_version;

    switch (sig) {
    case ZB_ZDO_SIGNAL_SKIP_STARTUP:
        ESP_LOGI(TAG, "Zigbee stack initialized");
        bdb_start_top_level_commissioning(ZB_BDB_INITIALIZATION);
        break;

    case ZB_MACSPLIT_DEVICE_BOOT:
        ESP_LOGI(TAG, "Zigbee rcp device booted");
        gateway_version = zb_esp_macsplit_get_version();
        rcp_version = ZB_ZDO_SIGNAL_GET_PARAMS(p_sg_p, zb_zdo_signal_macsplit_dev_boot_params_t);
        ESP_LOGI(TAG, "Zigbee rcp device version: %d.%d.%d", (rcp_version->dev_version >> 24 & 0x000000FF), (rcp_version->dev_version >> 16 & 0x000000FF), (rcp_version->dev_version & 0x000000FF));
        ESP_LOGI(TAG, "Zigbee gateway version: %d.%d.%d", (gateway_version >> 24 & 0x000000FF), (gateway_version >> 16 & 0x000000FF), (gateway_version & 0x000000FF));
        if (gateway_version != rcp_version->dev_version) {
            ESP_LOGE(TAG, "rcp has different Zigbee stack version with Zigbee gateway! Please check the rcp software or other issues");
        }
        break;

    case ZB_BDB_SIGNAL_DEVICE_FIRST_START:
        if (status == RET_OK) {
            ESP_LOGI(TAG, "Start network formation");
            bdb_start_top_level_commissioning(ZB_BDB_NETWORK_FORMATION);
        } else {
            ESP_LOGE(TAG, "Failed to initialize Zigbee stack (status: %d)", status);
        }
        break;

    case ZB_BDB_SIGNAL_FORMATION:
        if (status == RET_OK) {
            zb_ieee_addr_t ieee_address;
            zb_get_long_address(ieee_address);
            ESP_LOGI(TAG, "Formed network successfully (ieee extended address: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x, PAN ID: 0x%04hx)",
                     ieee_address[7], ieee_address[6], ieee_address[5], ieee_address[4],
                     ieee_address[3], ieee_address[2], ieee_address[1], ieee_address[0],
                     ZB_PIBCACHE_PAN_ID());
            bdb_start_top_level_commissioning(ZB_BDB_NETWORK_STEERING);
        } else {
            ESP_LOGI(TAG, "Restart network formation (status: %d)", status);
            ZB_SCHEDULE_APP_ALARM((zb_callback_t)bdb_start_top_level_commissioning_cb, ZB_BDB_NETWORK_FORMATION, ZB_TIME_ONE_SECOND);
        }
        break;

    case ZB_BDB_SIGNAL_STEERING:
        if (status == RET_OK) {
            ESP_LOGI(TAG, "Network steering started");
        }
        break;

    case ZB_ZDO_SIGNAL_DEVICE_ANNCE:
        dev_annce_params = ZB_ZDO_SIGNAL_GET_PARAMS(p_sg_p, zb_zdo_signal_device_annce_params_t);
        ESP_LOGI(TAG, "New device commissioned or rejoined (short: 0x%04hx)", dev_annce_params->device_short_addr);
        break;

    default:
        ESP_LOGI(TAG, "status: %d", status);
        break;
    }
    /* All callbacks should either reuse or free passed buffers. If bufid == 0, the buffer is invalid (not passed) */
    if (bufid) {
        zb_buf_free(bufid);
    }
}

static void zboss_task(void *pvParameters)
{
    ZB_INIT("zigbee gateway");
    zb_set_network_coordinator_role(IEEE_CHANNEL_MASK);
    zb_set_nvram_erase_at_start(ERASE_PERSISTENT_CONFIG);
    zb_set_max_children(MAX_CHILDREN);
    /* initiate Zigbee Stack start without zb_send_no_autostart_signal auto-start */
    ESP_ERROR_CHECK(zboss_start_no_autostart());
    while (1) {
        zboss_main_loop_iteration();
    }
}

void app_main(void)
{
    zb_esp_platform_config_t config = {
        .radio_config = ZB_ESP_DEFAULT_RADIO_CONFIG(),
        .host_config = ZB_ESP_DEFAULT_HOST_CONFIG(),
    };
    /* load Zigbee gateway platform config to initialization */
    ESP_ERROR_CHECK(zb_esp_platform_config(&config));
    xTaskCreate(zboss_task, "zboss_main", 4096, NULL, 5, NULL);
}
