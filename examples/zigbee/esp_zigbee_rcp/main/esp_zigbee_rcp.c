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
#include "zb_scheduler.h"
#include "esp_zigbee_rcp.h"

#if (defined ZB_MACSPLIT_HOST && !defined ZB_MACSPLIT_DEVICE)
#error Only Zigbee rcp device should be defined
#endif
static const char *TAG = "ESP_ZB_RCP";

void zboss_signal_handler(zb_uint8_t param)
{
    zb_zdo_app_signal_hdr_t *sg_p = NULL;
    /* get application signal from the buffer */
    zb_zdo_app_signal_type_t sig =  zb_get_app_signal(param, &sg_p);

    if (ZB_GET_APP_SIGNAL_STATUS(param) == 0) {
        switch (sig) {
        case ZB_COMMON_SIGNAL_CAN_SLEEP:
#if defined(ZB_USE_SLEEP)
            zb_sleep_now();
#endif
            break;
        default: break;
        }
    } else if (sig == ZB_ZDO_SIGNAL_PRODUCTION_CONFIG_READY) {
        ESP_LOGI(TAG, "Production config is not present or invalid");
    } else {
        ESP_LOGI(TAG, "Device started FAILED status %d", ZB_GET_APP_SIGNAL_STATUS(param));
    }

    if (param) {
        zb_buf_free(param);
    }
}

void zboss_task()
{
    ZB_INIT("esp_zigbee_rcp");
    while (1) {
        zb_sched_loop_iteration();
    }
}

void app_main()
{
    zb_esp_platform_config_t config = {
        .radio_config = ZB_ESP_DEFAULT_RADIO_CONFIG(),
        .host_config = ZB_ESP_DEFAULT_HOST_CONFIG(),
    };
    /* load Zigbee rcp platform config to initialization */
    ESP_ERROR_CHECK(zb_esp_platform_config(&config));
    xTaskCreate(zboss_task, "zboss_main", 4096, xTaskGetCurrentTaskHandle(), 5, NULL);
}
