/*
 * SPDX-FileCopyrightText: 2020 Intel Corporation
 * SPDX-FileCopyrightText: 2020-2024 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "esp_ble_audio_common_api.h"

esp_err_t esp_ble_audio_data_parse(const uint8_t ltv[], size_t size,
                                   bool (*func)(uint8_t type,
                                                const uint8_t *data,
                                                uint8_t data_len,
                                                void *user_data),
                                   void *user_data)
{
    return esp_ble_iso_data_parse(ltv, size, func, user_data);
}

esp_err_t esp_ble_audio_data_get_val(const uint8_t ltv_data[],
                                     size_t size, uint8_t type,
                                     const uint8_t **data,
                                     uint8_t *data_len)
{
    int ret;

    if (ltv_data == NULL || data == NULL || data_len == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ret = bt_audio_data_get_val(ltv_data, size, type, data);
    if (ret < 0) {
        return ESP_FAIL;
    }

    *data_len = ret;

    return ESP_OK;
}

uint8_t esp_ble_audio_get_chan_count(esp_ble_audio_location_t chan_allocation)
{
    return bt_audio_get_chan_count(chan_allocation);
}

esp_err_t esp_ble_audio_gattc_disc_start(uint16_t conn_handle)
{
    int err;

    err = bt_gattc_disc_start_safe(conn_handle);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

void esp_ble_audio_gap_app_post_event(uint8_t type, void *param)
{
    bt_le_gap_app_post_event(type, param);
}

void esp_ble_audio_gatt_app_post_event(uint8_t type, void *param)
{
    bt_le_gatt_app_post_event(type, param);
}

esp_err_t esp_ble_audio_common_init(esp_ble_audio_init_info_t *info)
{
    bool gap_cb_registered = false;
    bool gatt_cb_registered = false;
    int err;

    if (info) {
        if (info->gap_cb) {
            err = bt_le_gap_app_cb_register(info->gap_cb);
            if (err) {
                return ESP_FAIL;
            }

            gap_cb_registered = true;
        }

        if (info->gatt_cb) {
            err = bt_le_gatt_app_cb_register(info->gatt_cb);
            if (err) {
                goto unregister_gap;
            }

            gatt_cb_registered = true;
        }
    }

    err = bt_le_host_init();
    if (err) {
        goto unregister_gatt;
    }

    err = bt_le_audio_init();
    if (err) {
        bt_le_host_deinit();
        goto unregister_gatt;
    }

    return ESP_OK;

unregister_gatt:
    if (gatt_cb_registered) {
        bt_le_gatt_app_cb_unregister();
    }
unregister_gap:
    if (gap_cb_registered) {
        bt_le_gap_app_cb_unregister();
    }
    return ESP_FAIL;
}

esp_err_t esp_ble_audio_common_start(esp_ble_audio_start_info_t *info)
{
    int err;

    if (info) {
#if CONFIG_BT_CSIP_SET_MEMBER
        uint8_t cas_included = 0;

        for (size_t i = 0; i < ARRAY_SIZE(info->csis_insts); i++) {
            if (info->csis_insts[i].svc_inst &&
                    info->csis_insts[i].included_by_cas) {
                cas_included++;
            }
        }

        /* From CAS Spec:
         * The CAS shall include no more than one instance of CSIS.
         */
        if (cas_included > 1) {
            return ESP_ERR_INVALID_ARG;
        }
#endif /* CONFIG_BT_CSIP_SET_MEMBER */
    }

    err = bt_le_audio_start(info);
    if (err) {
        return ESP_FAIL;
    }

    return ESP_OK;
}
