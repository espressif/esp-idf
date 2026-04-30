/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/buf.h>
#include <zephyr/bluetooth/hci.h>

#include <../host/hci_core.h>
#include <../host/iso_internal.h>

#include "common/host.h"
#include "common/app/gatt.h"

static bt_le_gatt_app_cb gatt_app_cb;

int bt_le_gatt_app_cb_register(bt_le_gatt_app_cb cb)
{
    LOG_DBG("GattAppCbReg");

    if (cb == NULL) {
        LOG_ERR("GattAppCbNull");
        return -EINVAL;
    }

    if (gatt_app_cb) {
        LOG_WRN("GattAppCbExist");
        return -EALREADY;
    }

    gatt_app_cb = cb;

    return 0;
}

void bt_le_gatt_app_cb_unregister(void)
{
    if (gatt_app_cb) {
        LOG_DBG("GattAppCbUnreg");
        gatt_app_cb = NULL;
    }
}

static inline void bt_le_gatt_app_cb_evt(struct bt_le_gatt_app_event *event)
{
    if (gatt_app_cb) {
        gatt_app_cb(event);
    }
}

void bt_le_gatt_app_mtu_change_event(uint16_t conn_handle, uint16_t mtu)
{
    struct bt_le_gatt_app_event event = {
        .type = BT_LE_GATT_APP_EVENT_GATT_MTU_CHANGE,
    };

    LOG_DBG("GattMtuChangeAppEvt[%u][%u]", conn_handle, mtu);

    event.gatt_mtu_change.conn_handle = conn_handle;
    event.gatt_mtu_change.mtu = mtu;

    bt_le_gatt_app_cb_evt(&event);
}

void bt_le_gattc_app_disc_cmpl_event(struct bt_le_gattc_disc_cmpl_event *param)
{
    struct bt_le_gatt_app_event event = {
        .type = BT_LE_GATT_APP_EVENT_GATTC_DISC_CMPL,
    };

    LOG_DBG("GattcDiscCmplAppEvt[%u][%02x]", param->conn_handle, param->status);

    event.gattc_disc_cmpl.status = param->status;
    event.gattc_disc_cmpl.conn_handle = param->conn_handle;

    bt_le_gatt_app_cb_evt(&event);
}

void bt_le_gatts_app_subscribe_event(struct bt_le_gatts_subscribe_event *param)
{
    struct bt_le_gatt_app_event event = {
        .type = BT_LE_GATT_APP_EVENT_GATTS_SUBSCRIBE,
    };

    /* LOG_DBG("GattsSubAppEvt[%d][%u][%u][%u][%u][%u][%u][%02x]", */
    /*         err, param->conn_handle, param->attr_handle, param->prev_notify, */
    /*         param->cur_notify, param->prev_indicate, param->cur_indicate, */
    /*         param->reason); */

    event.gatts_subscribe.conn_handle = param->conn_handle;
    event.gatts_subscribe.attr_handle = param->attr_handle;
    event.gatts_subscribe.prev_notify = param->prev_notify;
    event.gatts_subscribe.cur_notify = param->cur_notify;
    event.gatts_subscribe.prev_indicate = param->prev_indicate;
    event.gatts_subscribe.cur_indicate = param->cur_indicate;
    event.gatts_subscribe.reason = param->reason;

    bt_le_gatt_app_cb_evt(&event);
}

void bt_le_gatt_app_post_event(uint8_t type, void *param)
{
    /* Currently type is not used */
    ARG_UNUSED(type);

    bt_le_nimble_gatt_post_event(param);
}
