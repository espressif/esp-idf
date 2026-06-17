/*
 * SPDX-FileCopyrightText: 2023 NXP
 * SPDX-FileCopyrightText: 2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_ble_audio_pbp_api.h"

#if CONFIG_BT_PBP
esp_err_t esp_ble_audio_pbp_get_announcement(const uint8_t meta[], size_t meta_len,
                                             esp_ble_audio_pbp_announcement_feature_t features,
                                             struct net_buf_simple *pba_data_buf)
{
    int err;

    if (pba_data_buf == NULL ||
            pba_data_buf->size < (meta_len + ESP_BLE_AUDIO_PBP_MIN_PBA_SIZE)) {
        return ESP_ERR_INVALID_ARG;
    }

    if ((meta == NULL && meta_len != 0) || (meta != NULL && meta_len == 0)) {
        return ESP_ERR_INVALID_ARG;
    }

    err = bt_pbp_get_announcement(meta, meta_len, features, pba_data_buf);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t esp_ble_audio_pbp_parse_announcement(uint8_t data_type, const uint8_t *data, uint8_t data_len,
                                               esp_ble_audio_pbp_announcement_feature_t *features,
                                               uint8_t **meta, uint8_t *meta_len)
{
    struct bt_data ad = {
        .type = data_type,
        .data = data,
        .data_len = data_len,
    };
    int ret;

    if (data == NULL || data_len == 0 || features == NULL ||
            meta == NULL || meta_len == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (data_type != BT_DATA_SVC_DATA16 ||
            data_len < ESP_BLE_AUDIO_PBP_MIN_PBA_SIZE) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_pbp_parse_announcement(&ad, features, meta);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *meta_len = ret;

    return ESP_OK;
}
#endif /* CONFIG_BT_PBP */
