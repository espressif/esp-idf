/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "sys/lock.h"

#include "esp_log.h"
#include "esp_avrc_api.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "avrcp_abs_vol_utils.h"
#include "avrcp_utils_tags.h"
#include "avrcp_abs_vol_service.h"

/*******************************
 * STATIC FUNCTION DECLARATIONS
 ******************************/

/* simulation volume change */
static void volume_change_simulation(void *arg);

/*******************************
 * STATIC VARIABLE DEFINITIONS
 ******************************/

static TaskHandle_t s_vcs_task_hdl = NULL;    /* handle for volume change simulation task */

/********************************
 * STATIC FUNCTION DEFINITIONS
 *******************************/

static void volume_change_simulation(void *arg)
{
    ESP_LOGI(BT_RC_TG_TAG, "start volume change simulation");

    for (;;) {
        /* volume up locally every 10 seconds */
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        uint8_t volume = (avrc_abs_vol_srv_get_volume() + 5) & 0x7f;
        avrc_abs_vol_srv_set_volume(VOLUME_SET_BY_LOCAL_HOST, volume);
    }
}

/********************************
 * EXTERNAL FUNCTION DEFINITIONS
 *******************************/

void bt_avrc_avc_tg_evt_hdl(uint16_t event, void *param)
{
    ESP_LOGD(BT_RC_TG_TAG, "%s event: %d", __func__, event);

    esp_avrc_tg_cb_param_t *rc = (esp_avrc_tg_cb_param_t *)(param);

    switch (event) {
    /* when connection state changed, this event comes */
    case ESP_AVRC_TG_CONNECTION_STATE_EVT: {
        uint8_t *bda = rc->conn_stat.remote_bda;
        ESP_LOGI(BT_RC_TG_TAG, "AVRC conn_state evt: state %d, [%02x:%02x:%02x:%02x:%02x:%02x]",
                 rc->conn_stat.connected, bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
        if (rc->conn_stat.connected) {
            /* create task to simulate volume change */
            avrc_abs_vol_srv_open();
            xTaskCreate(volume_change_simulation, "vcsTask", 4 * 1024, NULL, 5, &s_vcs_task_hdl);
        } else {
            avrc_abs_vol_srv_close();
            if (s_vcs_task_hdl) {
                vTaskDelete(s_vcs_task_hdl);
                s_vcs_task_hdl = NULL;
            }
            ESP_LOGI(BT_RC_TG_TAG, "Stop volume change simulation");
        }
        break;
    }
    /* when absolute volume command from remote device set, this event comes */
    case ESP_AVRC_TG_SET_ABSOLUTE_VOLUME_CMD_EVT: {
        ESP_LOGI(BT_RC_TG_TAG, "AVRC set absolute volume: %d%%", (int)rc->set_abs_vol.volume * 100 / 0x7f);
        avrc_abs_vol_srv_set_volume(VOLUME_SET_BY_CONTROLLER, rc->set_abs_vol.volume);
        break;
    }
    /* when notification registered, this event comes */
    case ESP_AVRC_TG_REGISTER_NOTIFICATION_EVT: {
        ESP_LOGI(BT_RC_TG_TAG, "AVRC register event notification: %d, param: 0x%"PRIx32, rc->reg_ntf.event_id, rc->reg_ntf.event_parameter);
        if (rc->reg_ntf.event_id == ESP_AVRC_RN_VOLUME_CHANGE) {
            avrc_abs_vol_srv_rn_volume_change();
        }
        break;
    }
    /* others */
    default:
        ESP_LOGE(BT_RC_TG_TAG, "%s unhandled event: %d", __func__, event);
        break;
    }
}
