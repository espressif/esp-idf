/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "esp_log.h"
#include "esp_avrc_api.h"
#include "avrcp_common_utils.h"
#include "avrcp_psth_cmd_service.h"

#define RC_PSTH_SRV_TAG    "RC_PSTH_SRV"

esp_err_t avrc_psth_cmd_srv_send_cmd(esp_a2d_conn_hdl_t conn_hdl, esp_avrc_pt_cmd_t key_code,
                                     esp_avrc_pt_cmd_state_t key_state)
{
    esp_err_t err = ESP_OK;
    uint8_t tl = bt_avrc_common_alloc_tl();

    err = esp_avrc_ct_send_passthrough_cmd_by_handle(conn_hdl, tl, key_code, key_state);
    if (err != ESP_OK) {
        ESP_LOGE(RC_PSTH_SRV_TAG, "send passthrough command failed, key_code 0x%02x, key_state %d, err %s", key_code, key_state, esp_err_to_name(err));
    }

    return err;
}
