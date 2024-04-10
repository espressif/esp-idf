/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include "esp_bt_device.h"
#include "esp_bt_main.h"
#include "device/controller.h"
#include "btc/btc_task.h"
#include "btc/btc_dev.h"
#include "btc/btc_config.h"
#include "btc/btc_manage.h"

esp_err_t esp_bt_dev_register_callback(esp_bt_dev_cb_t callback)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    if (callback == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_profile_cb_set(BTC_PID_DEV, callback);
    return ESP_OK;
}

const uint8_t *esp_bt_dev_get_address(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
	return NULL;
    }
    return controller_get_interface()->get_address()->address;
}

esp_err_t esp_bt_dev_set_device_name(const char *name)
{
    btc_msg_t msg = {0};
    btc_dev_args_t arg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }
    if (!name){
        return ESP_ERR_INVALID_ARG;
    }
    if (strlen(name) > BTC_MAX_LOC_BD_NAME_LEN) {
        return ESP_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_DEV;
    msg.act = BTC_DEV_ACT_SET_DEVICE_NAME;
    arg.set_dev_name.device_name = (char *)name;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_dev_args_t), btc_dev_call_arg_deep_copy, btc_dev_call_arg_deep_free) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

esp_err_t esp_bt_dev_get_device_name(void)
{
    btc_msg_t msg = {0};

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_DEV;
    msg.act = BTC_DEV_ACT_GET_DEVICE_NAME;

    return (btc_transfer_context(&msg, NULL, 0, NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

#if (ESP_COEX_VSC_INCLUDED == TRUE)
esp_err_t esp_bt_dev_coex_status_config(esp_bt_dev_coex_type_t type, esp_bt_dev_coex_op_t op, uint8_t status)
{
    btc_msg_t msg = {0};
    btc_dev_args_t arg;

    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return ESP_ERR_INVALID_STATE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_DEV;
    msg.act = BTC_DEV_ACT_CFG_COEX_STATUS;
    arg.cfg_coex_status.type = type;
    arg.cfg_coex_status.op = op;
    arg.cfg_coex_status.status = status;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_dev_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}
#endif

esp_err_t esp_bt_config_file_path_update(const char *file_path)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_UNINITIALIZED);

    return btc_config_file_path_update(file_path);
}
