/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
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
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (callback == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    int ret = btc_profile_cb_set(BTC_PID_DEV, callback);
    return (ret == 0) ? ESP_OK : ESP_FAIL;
}

const uint8_t *esp_bt_dev_get_address(void)
{
    if (esp_bluedroid_get_status() != ESP_BLUEDROID_STATUS_ENABLED) {
        return NULL;
    }
    return controller_get_interface()->get_address()->address;
}

#if (ESP_COEX_VSC_INCLUDED == TRUE)
esp_err_t esp_bt_dev_coex_status_config(esp_bt_dev_coex_type_t type, esp_bt_dev_coex_op_t op, uint8_t status)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (type < ESP_BT_DEV_COEX_TYPE_BLE || type >= ESP_BT_DEV_COEX_TYPE_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    btc_msg_t msg = {0};
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_DEV;
    msg.act = BTC_DEV_ACT_CFG_COEX_STATUS;

    btc_dev_args_t arg = {0};
    arg.cfg_coex_status.type = type;
    arg.cfg_coex_status.op = op;
    arg.cfg_coex_status.status = status;

    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_dev_args_t), NULL, NULL);
    return (stat == BT_STATUS_SUCCESS) ? ESP_OK : ESP_FAIL;
}
#endif

esp_err_t esp_bt_config_file_path_get(char *file_path)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if (file_path == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    int ret = btc_config_file_path_get(file_path);
    return (ret == 0) ? ESP_OK : ESP_FAIL;
}

esp_err_t esp_bt_config_file_path_update(const char *file_path)
{
    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_UNINITIALIZED);

    if (file_path == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    int ret = btc_config_file_path_update(file_path);
    return (ret == 0) ? ESP_OK : ESP_FAIL;
}
