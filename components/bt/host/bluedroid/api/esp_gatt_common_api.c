/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_gatt_common_api.h"
#include "esp_bt_main.h"
#include "esp_gatt_defs.h"
#include "btc_gatt_common.h"

/**
 * @brief           This function is called to set local MTU,
 *                  the function is called before BLE connection.
 *
 * @param[in]       mtu: the size of MTU.
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 *
 */
esp_err_t esp_ble_gatt_set_local_mtu (uint16_t mtu)
{
    btc_msg_t msg = {0};
    btc_ble_gatt_com_args_t arg;

    ESP_BLUEDROID_STATUS_CHECK(ESP_BLUEDROID_STATUS_ENABLED);

    if ((mtu < ESP_GATT_DEF_BLE_MTU_SIZE) || (mtu > ESP_GATT_MAX_MTU_SIZE)) {
        return ESP_ERR_INVALID_SIZE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GATT_COMMON;
    msg.act = BTC_GATT_ACT_SET_LOCAL_MTU;
    arg.set_mtu.mtu = mtu;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gatt_com_args_t), NULL, NULL) == BT_STATUS_SUCCESS ? ESP_OK : ESP_FAIL);
}

#if (BLE_INCLUDED == TRUE)
extern UINT16 L2CA_GetFreePktBufferNum_LE(void);

/**
 * @brief           This function is called to get currently sendable packets number on controller,
 *                  the function is called only in BLE running core and single connection now.
 *
 * @return
 *                  sendable packets number on controller
 *
 */

uint16_t esp_ble_get_sendable_packets_num (void)
{
    return L2CA_GetFreePktBufferNum_LE();
}

/**
 * @brief           This function is used to query the number of available buffers for the current connection.
 *                  When you need to query the current available buffer number, it is recommended to use this API.
 * @param[in]       conn_id: current connection id.
 *
 * @return
 *                  Number of available buffers for the current connection
 *
 */

extern UINT16 L2CA_GetCurFreePktBufferNum_LE(UINT16 conn_id);
uint16_t esp_ble_get_cur_sendable_packets_num (uint16_t connid)
{
    return L2CA_GetCurFreePktBufferNum_LE(connid);
}
#endif
