/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "esp_bt_main.h"
#include "btc/btc_manage.h"
#include "stack/sdpdefs.h"

#include "btc_sdp.h"
#include "esp_sdp_api.h"
#include "common/bt_target.h"

#if (defined BTC_SDP_COMMON_INCLUDED && BTC_SDP_COMMON_INCLUDED == TRUE)

static bool esp_sdp_record_integrity_check(esp_bluetooth_sdp_record_t *record)
{
    bool ret = true;

    if (record != NULL) {
        switch (record->hdr.type) {
        case ESP_SDP_TYPE_DIP_SERVER:
            if (record->dip.vendor_id_source != ESP_SDP_VENDOR_ID_SRC_BT &&
                record->dip.vendor_id_source != ESP_SDP_VENDOR_ID_SRC_USB) {
                LOG_ERROR("Invalid vendor_id_source!\n");
                ret = false;
            }
            break;

        default:
            if (record->hdr.service_name_length > ESP_SDP_SERVER_NAME_MAX ||
                strlen(record->hdr.service_name) + 1 != record->hdr.service_name_length) {
                LOG_ERROR("Invalid server name!\n");
                ret = false;
            }
            break;
        }
    } else {
        LOG_ERROR("record is NULL!\n");
        ret = false;
    }

    return ret;
}

esp_err_t esp_sdp_register_callback(esp_sdp_cb_t callback)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (callback == NULL) {
        return ESP_FAIL;
    }

    btc_profile_cb_set(BTC_PID_SDP, callback);
    return ESP_OK;
}

esp_err_t esp_sdp_init(void)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    btc_msg_t msg;
    bt_status_t stat;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_SDP;
    msg.act = BTC_SDP_ACT_INIT;

    /* Switch to BTC context */
    stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_sdp_deinit(void)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    btc_msg_t msg;
    bt_status_t stat;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_SDP;
    msg.act = BTC_SDP_ACT_DEINIT;

    /* Switch to BTC context */
    stat = btc_transfer_context(&msg, NULL, 0, NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_sdp_search_record(esp_bd_addr_t bd_addr, esp_bt_uuid_t uuid)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    btc_msg_t msg;
    bt_status_t stat;
    btc_sdp_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_SDP;
    msg.act = BTC_SDP_ACT_SEARCH;

    memset(&arg, 0, sizeof(btc_sdp_args_t));
    memcpy(arg.search.bd_addr, bd_addr, ESP_BD_ADDR_LEN);
    arg.search.sdp_uuid.len = uuid.len;
    memcpy(&arg.search.sdp_uuid.uu, &uuid.uuid, sizeof(uuid.uuid));

    /* Switch to BTC context */
    stat = btc_transfer_context(&msg, &arg, sizeof(btc_sdp_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_sdp_create_record(esp_bluetooth_sdp_record_t *record)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (!esp_sdp_record_integrity_check(record)) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg;
    bt_status_t stat;
    btc_sdp_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_SDP;
    msg.act = BTC_SDP_ACT_CREATE_RECORD;

    memset(&arg, 0, sizeof(btc_sdp_args_t));
    arg.create_record.record = (bluetooth_sdp_record *)record;

    /* Switch to BTC context */
    stat = btc_transfer_context(&msg, &arg, sizeof(btc_sdp_args_t),
                                    btc_sdp_arg_deep_copy, btc_sdp_arg_deep_free);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_sdp_remove_record(int record_handle)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    btc_msg_t msg;
    bt_status_t stat;
    btc_sdp_args_t arg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_SDP;
    msg.act = BTC_SDP_ACT_REMOVE_RECORD;

    arg.remove_record.record_handle = record_handle;

    /* Switch to BTC context */
    stat = btc_transfer_context(&msg, &arg, sizeof(btc_sdp_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}

#endif ///defined BTC_SDP_COMMON_INCLUDED && BTC_SDP_COMMON_INCLUDED == TRUE
