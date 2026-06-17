/*
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 * SPDX-FileCopyrightText: 2023 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_COMMON_GATT_H_
#define HOST_COMMON_GATT_H_

#include <stdint.h>

#include <zephyr/autoconf.h>
#include <zephyr/bluetooth/gatt.h>

#if CONFIG_BT_BLUEDROID_ENABLED
#include "bluedroid/gatt.h"
#else
#include "nimble/gatt.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct bt_le_gatt_mtu_change_event {
    uint16_t conn_handle;
    uint16_t mtu;
};

enum {
    GATTC_DISC_TYPE_DUMMY,
    GATTC_DISC_TYPE_SVC_BY_UUID,
    GATTC_DISC_TYPE_INC_SVCS,
    GATTC_DISC_TYPE_CHRS_BY_UUID,
    GATTC_DISC_TYPE_ALL_CHRS,
    GATTC_DISC_TYPE_ALL_DSCS,   /* Not used for now */

    GATTC_DISC_TYPE_MAX,
};

struct bt_le_gattc_discover_event {
    uint8_t type;
    uint16_t conn_handle;
    uint16_t uuid;
    struct bt_gatt_discover_params *params;
};

struct bt_le_gattc_disc_cmpl_event {
    uint8_t  status;
    uint16_t conn_handle;
};

struct bt_le_gatts_subscribe_event {
    uint16_t conn_handle;
    uint16_t attr_handle;
    uint8_t  prev_notify : 1;
    uint8_t  cur_notify : 1;
    uint8_t  prev_indicate : 1;
    uint8_t  cur_indicate : 1;
    uint8_t  reason;
};

struct bt_le_gattc_notify_rx_event {
    bool     is_notify;
    uint16_t conn_handle;
    uint16_t attr_handle;
    uint16_t len;
    uint8_t *value;
};

struct bt_le_gatts_notify_tx_event {
    bool     is_notify;
    uint16_t conn_handle;
    uint16_t attr_handle;
    uint8_t  status;
};

/* Bluedroid-side adapter events. NimBLE produces ACL connect/disconnect via
 * the GAP event path (BT_LE_GAP_APP_PARAM_ACL_*), but Bluedroid's BTA
 * surfaces these through GATTS/GATTC callbacks. The bluedroid adapter posts
 * these typed events into the iso task and translates them into ACL/MTU/
 * discovery events for the LE Audio profile dispatcher. */

struct bt_le_addr_simple {
    uint8_t type;
    uint8_t val[6];
};

struct bt_le_gattc_connect_event {
    uint16_t conn_handle;
    uint8_t  role;
    struct bt_le_addr_simple peer;
};

struct bt_le_gattc_disconnect_event {
    uint16_t conn_handle;
    uint8_t  reason;
};

struct bt_le_gattc_open_event {
    uint8_t  status;
    uint16_t conn_handle;
};

struct bt_le_gattc_mtu_event {
    uint8_t  status;
    uint16_t conn_handle;
    uint16_t mtu;
};

struct bt_le_gattc_read_chrc_event {
    uint8_t  status;
    uint16_t conn_handle;
    uint16_t attr_handle;
    uint16_t len;
    uint8_t  *value;
};

struct bt_le_gattc_write_chrc_event {
    uint8_t  status;
    uint16_t conn_handle;
    uint16_t attr_handle;
    uint16_t offset;
};

struct bt_le_gatts_connect_event {
    uint16_t conn_handle;
    uint8_t  role;
    struct bt_le_addr_simple peer;
};

struct bt_le_gatts_disconnect_event {
    uint16_t conn_handle;
    uint8_t  reason;
};

struct bt_le_gatts_mtu_event {
    uint16_t conn_handle;
    uint16_t mtu;
};

struct bt_le_gatts_read_event {
    uint16_t conn_handle;
    uint32_t trans_id;
    uint8_t  peer[6];
    uint16_t attr_handle;
    uint16_t offset;
    bool     is_long;
    bool     need_rsp;
};

struct bt_le_gatts_write_event {
    uint16_t conn_handle;
    uint32_t trans_id;
    uint8_t  peer[6];
    uint16_t attr_handle;
    uint16_t offset;
    bool     is_prep;
    bool     need_rsp;
    uint16_t len;
    uint8_t  *value;
};

struct bt_le_gatt_event_param {
    uint8_t type;

    union {
        struct bt_le_gatt_mtu_change_event  gatt_mtu_change;
        struct bt_le_gattc_discover_event   gattc_discover;
        struct bt_le_gattc_disc_cmpl_event  gattc_disc_cmpl;
        struct bt_le_gatts_subscribe_event  gatts_subscribe;
        struct bt_le_gattc_notify_rx_event  gattc_notify_rx;
        struct bt_le_gatts_notify_tx_event  gatts_notify_tx;

        /* Bluedroid-only paths (see comment above). */
        struct bt_le_gattc_connect_event    gattc_connect;
        struct bt_le_gattc_disconnect_event gattc_disconnect;
        struct bt_le_gattc_open_event       gattc_open;
        struct bt_le_gattc_mtu_event        gattc_mtu;
        struct bt_le_gattc_read_chrc_event  gattc_read_chrc;
        struct bt_le_gattc_write_chrc_event gattc_write_chrc;
        struct bt_le_gatts_connect_event    gatts_connect;
        struct bt_le_gatts_disconnect_event gatts_disconnect;
        struct bt_le_gatts_mtu_event        gatts_mtu;
        struct bt_le_gatts_read_event       gatts_read;
        struct bt_le_gatts_write_event      gatts_write;
    };
};

enum {
    BT_LE_GATT_MTU_CHANGE_EVENT,
    BT_LE_GATTC_DISCOVER_EVENT,
    BT_LE_GATTC_DISC_CMPL_EVENT,
    BT_LE_GATTS_SUBSCRIBE_EVENT,
    BT_LE_GATTC_NOTIFY_RX_EVENT,
    BT_LE_GATTS_NOTIFY_TX_EVENT,

    /* Bluedroid-only paths (see comment above). */
    BT_LE_GATTC_CONNECT_EVENT,
    BT_LE_GATTC_DISCONNECT_EVENT,
    BT_LE_GATTC_OPEN_EVENT,
    BT_LE_GATTC_MTU_EVENT,
    BT_LE_GATTC_READ_CHRC_EVENT,
    BT_LE_GATTC_WRITE_CHRC_EVENT,
    BT_LE_GATTS_CONNECT_EVENT,
    BT_LE_GATTS_DISCONNECT_EVENT,
    BT_LE_GATTS_MTU_EVENT,
    BT_LE_GATTS_READ_EVENT,
    BT_LE_GATTS_WRITE_EVENT,

    BT_LE_GATT_EVENT_MAX,
};

struct gattc_sub {
    uint8_t id;
    bt_addr_le_t peer;
    sys_slist_t list;
};

struct gatt_incl {
    uint16_t start_handle;
    uint16_t end_handle;
    uint16_t uuid16;
} __attribute__((packed));

struct gatt_chrc {
    uint8_t  properties;
    uint16_t value_handle;
    union {
        uint16_t uuid16;
        uint8_t  uuid[16];
    };
} __attribute__((packed));

struct bt_att_write_req {
    uint16_t handle;
    uint8_t  value[0];
} __attribute__((packed));

struct notify_data {
    const struct bt_gatt_attr *attr;
    uint16_t handle;
    int err;
    uint16_t type;
    union {
        struct bt_gatt_notify_params *nfy_params;
        struct bt_gatt_indicate_params *ind_params;
    };
};

struct bt_gatt_attr *bt_gatts_find_attr_by_handle(uint16_t handle);

bool bt_gatts_find_attr_by_uuid(struct notify_data *found,
                                   const struct bt_uuid *uuid);

int bt_gatts_sub_changed(uint16_t conn_handle,
                         uint16_t ccc_handle,
                         uint8_t cur_notify,
                         uint8_t cur_indicate,
                         uint8_t reason);

int bt_gattc_disc_start_safe(uint16_t conn_handle);

struct gattc_sub *bt_gattc_sub_find(struct bt_conn *conn);

void bt_le_acl_conn_disconnected_gatt_listener(uint16_t conn_handle);

void bt_le_gatt_handle_event(uint8_t *data, size_t data_len);

#ifdef __cplusplus
}
#endif

#endif /* HOST_COMMON_GATT_H_ */
