/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <inttypes.h>

#include "esp_log.h"
#include "esp_avrc_api.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "avrcp_psth_cmd_utils.h"
#include "avrcp_psth_cmd_service.h"

#define RC_PSTH_SRV_TAG "RC_PSTH_UTILS"

static volatile bool s_is_connected = false;
static TaskHandle_t s_mcs_task_hdl = NULL;    /* handle for music control simulation task */

static void music_control_simulation(void *arg)
{
    (void)arg;
    ESP_LOGI(RC_PSTH_SRV_TAG, "start music control simulation");

    for (;;) {
        if (s_is_connected) {
            ESP_LOGI(RC_PSTH_SRV_TAG, "PLAY");
            avrc_psth_cmd_srv_send_cmd(ESP_AVRC_PT_CMD_PLAY, ESP_AVRC_PT_CMD_STATE_PRESSED);
            avrc_psth_cmd_srv_send_cmd(ESP_AVRC_PT_CMD_PLAY, ESP_AVRC_PT_CMD_STATE_RELEASED);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));

        if (s_is_connected) {
            ESP_LOGI(RC_PSTH_SRV_TAG, "PAUSE");
            avrc_psth_cmd_srv_send_cmd(ESP_AVRC_PT_CMD_PAUSE, ESP_AVRC_PT_CMD_STATE_PRESSED);
            avrc_psth_cmd_srv_send_cmd(ESP_AVRC_PT_CMD_PAUSE, ESP_AVRC_PT_CMD_STATE_RELEASED);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));

        if (s_is_connected) {
            ESP_LOGI(RC_PSTH_SRV_TAG, "NEXT TRACK");
            avrc_psth_cmd_srv_send_cmd(ESP_AVRC_PT_CMD_FORWARD, ESP_AVRC_PT_CMD_STATE_PRESSED);
            avrc_psth_cmd_srv_send_cmd(ESP_AVRC_PT_CMD_FORWARD, ESP_AVRC_PT_CMD_STATE_RELEASED);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void bt_avrc_psth_cmd_ct_evt_hdl(esp_avrc_ct_cb_event_t event, esp_avrc_ct_cb_param_t *param)
{
    ESP_LOGD(RC_PSTH_SRV_TAG, "%s event: %d", __func__, event);

    switch (event) {
    case ESP_AVRC_CT_CONNECTION_STATE_EVT: {
        uint8_t *bda = param->conn_stat.remote_bda;
        ESP_LOGI(RC_PSTH_SRV_TAG, "AVRC conn_state event: state %d, [%02x:%02x:%02x:%02x:%02x:%02x]",
                 param->conn_stat.connected, bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
        if (param->conn_stat.connected) {
            /* create task to simulate music control */
            s_is_connected = true;
            if (!s_mcs_task_hdl) {
                xTaskCreate(music_control_simulation, "vcsTask", 4 * 1024, NULL, 5, &s_mcs_task_hdl);
            }
        } else {
            s_is_connected = false;
            ESP_LOGI(RC_PSTH_SRV_TAG, "stop music control simulation");
        }
        break;
    }
    case ESP_AVRC_CT_PASSTHROUGH_RSP_EVT: {
        ESP_LOGI(RC_PSTH_SRV_TAG, "AVRC passthrough rsp: key_code 0x%x, key_state %d, rsp_code %d", param->psth_rsp.key_code,
                 param->psth_rsp.key_state, param->psth_rsp.rsp_code);
        break;
    }
    case ESP_AVRC_CT_REMOTE_FEATURES_EVT: {
        ESP_LOGI(RC_PSTH_SRV_TAG, "AVRC remote features %" PRIx32 ", TG features %x",
                 param->rmt_feats.feat_mask, param->rmt_feats.tg_feat_flag);
        break;
    }
    case ESP_AVRC_CT_PROF_STATE_EVT: {
        if (ESP_AVRC_INIT_SUCCESS == param->avrc_ct_init_stat.state) {
            ESP_LOGI(RC_PSTH_SRV_TAG, "AVRCP CT STATE: Init Complete");
        } else if (ESP_AVRC_DEINIT_SUCCESS == param->avrc_ct_init_stat.state) {
            ESP_LOGI(RC_PSTH_SRV_TAG, "AVRCP CT STATE: Deinit Complete");
        } else {
            ESP_LOGE(RC_PSTH_SRV_TAG, "AVRCP CT STATE error: %d", param->avrc_ct_init_stat.state);
        }
        break;
    }
    default: {
        ESP_LOGI(RC_PSTH_SRV_TAG, "%s unhandled event: %d", __func__, event);
        break;
    }
    }
}
