/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "esp_err.h"
#include "esp_log.h"
#include "esp_gap_bt_api.h"
#include "esp_a2dp_api.h"
#include "esp_avrc_api.h"

#include "bt_app_core_utils.h"
#include "bredr_app_common_utils.h"
#include "avrcp_psth_cmd_utils.h"

/* device name */
static const char local_device_name[] = CONFIG_EXAMPLE_LOCAL_DEVICE_NAME;
/* tag */
static const char *TAG = "AVRCP_PSTH";

/* event for stack up */
enum {
    BT_APP_EVT_STACK_UP = 0,
};

/*******************************
 * STATIC FUNCTION DEFINITIONS
 ******************************/

static void bt_av_hdl_stack_evt(uint16_t event, void *p_param)
{
    ESP_LOGD(TAG, "%s event: %d", __func__, event);

    switch (event) {
    /* when do the stack up, this event comes */
    case BT_APP_EVT_STACK_UP: {
        esp_bt_gap_set_device_name(local_device_name);

        esp_bt_gap_register_callback(bredr_app_gap_evt_def_hdl);
        esp_avrc_ct_register_callback(bt_avrc_psth_cmd_ct_evt_hdl);
        ESP_ERROR_CHECK(esp_avrc_ct_init());
        ESP_ERROR_CHECK(esp_a2d_sink_init());

        /* set discoverable and connectable mode, wait to be connected */
        esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
        break;
    }
    /* others */
    default:
        ESP_LOGE(TAG, "%s unhandled event: %d", __func__, event);
        break;
    }
}

/*******************************
 * MAIN ENTRY POINT
 ******************************/

void app_main(void)
{
    ESP_ERROR_CHECK(bredr_app_common_init());

    bt_app_task_start_up();
    /* bluetooth device name, connection mode and profile set up */
    bt_app_work_dispatch(bt_av_hdl_stack_evt, BT_APP_EVT_STACK_UP, NULL, 0, NULL, NULL);
}
