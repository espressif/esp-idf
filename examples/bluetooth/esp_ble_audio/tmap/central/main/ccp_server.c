/*
 * SPDX-FileCopyrightText: 2023 NXP
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include "tmap_central.h"

static bool tbs_originate_call_cb(esp_ble_conn_t *conn,
                                  uint8_t call_index,
                                  const char *caller_id)
{
    ESP_LOGI(TAG, "CCP: Placing call to remote with id %u to %s",
             call_index, caller_id);

    return true;
}

static void tbs_terminate_call_cb(esp_ble_conn_t *conn,
                                  uint8_t call_index,
                                  uint8_t reason)
{
    ESP_LOGI(TAG, "CCP: Call terminated for id %u with reason %u",
             call_index, reason);
}

static esp_ble_audio_tbs_cb_t tbs_cbs = {
    .originate_call = tbs_originate_call_cb,
    .terminate_call = tbs_terminate_call_cb,
};

int ccp_server_init(void)
{
    const esp_ble_audio_tbs_register_param_t gtbs_param = {
        .provider_name = "Generic TBS",
        .uci = "un000",
        .uri_schemes_supported = "tel,wechat",
        .gtbs = true,
        .authorization_required = false,
        .technology = ESP_BLE_AUDIO_TBS_TECHNOLOGY_5G,
        .supported_features = CONFIG_BT_TBS_SUPPORTED_FEATURES,
    };
    uint8_t bearer_index;
    int err;

    esp_ble_audio_tbs_register_cb(&tbs_cbs);

    err = esp_ble_audio_tbs_register_bearer(&gtbs_param, &bearer_index);
    if (err) {
        ESP_LOGE(TAG, "Failed to register gtbs bearer (err %d)", err);
        return err;
    }

    ESP_LOGI(TAG, "Registered gtbs bearer %u", bearer_index);

    return 0;
}
