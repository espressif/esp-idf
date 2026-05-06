/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_COMMON_APP_GATT_H_
#define HOST_COMMON_APP_GATT_H_

#include <stdint.h>

#include "sdkconfig.h"

#include "nimble/gatt.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bt_le_gatt_app_event_gatt_mtu_change {
    uint16_t conn_handle;
    uint16_t mtu;
};

struct bt_le_gatt_app_event_gattc_disc_cmpl {
    uint8_t  status;
    uint16_t conn_handle;
};

struct bt_le_gatt_app_event_gatts_subscribe {
    uint16_t conn_handle;
    uint16_t attr_handle;
    uint8_t  prev_notify : 1;
    uint8_t  cur_notify : 1;
    uint8_t  prev_indicate : 1;
    uint8_t  cur_indicate : 1;
    uint8_t  reason;
};

struct bt_le_gatt_app_event {
    uint8_t type;

    union {
        struct bt_le_gatt_app_event_gatt_mtu_change gatt_mtu_change;
        struct bt_le_gatt_app_event_gattc_disc_cmpl gattc_disc_cmpl;
        struct bt_le_gatt_app_event_gatts_subscribe gatts_subscribe;
    };
};

enum bt_le_gatt_app_event_type {
    BT_LE_GATT_APP_EVENT_GATT_MTU_CHANGE,
    BT_LE_GATT_APP_EVENT_GATTC_DISC_CMPL,
    BT_LE_GATT_APP_EVENT_GATTS_SUBSCRIBE,

    BT_LE_GATT_APP_EVENT_MAX,
};

typedef void (*bt_le_gatt_app_cb)(struct bt_le_gatt_app_event *event);

int bt_le_gatt_app_cb_register(bt_le_gatt_app_cb cb);

void bt_le_gatt_app_cb_unregister(void);

void bt_le_gatt_app_mtu_change_event(uint16_t conn_handle, uint16_t mtu);

void bt_le_gattc_app_disc_cmpl_event(struct bt_le_gattc_disc_cmpl_event *param);

void bt_le_gatts_app_subscribe_event(struct bt_le_gatts_subscribe_event *param);

void bt_le_gatt_app_post_event(uint8_t type, void *param);

#ifdef __cplusplus
}
#endif

#endif /* HOST_COMMON_APP_GATT_H_ */
