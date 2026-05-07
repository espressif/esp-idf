/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_NIMBLE_GATT_H_
#define HOST_NIMBLE_GATT_H_

#include <stdint.h>
#include <assert.h>

#include <zephyr/bluetooth/uuid.h>

#include "host/ble_uuid.h"
#include "host/ble_gatt.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bt_le_nimble_gatt_read_cb {
    int (*read_cb)(void *arg, uint16_t offset, const void *data, uint16_t len);
    void *read_arg;
};

static inline uint8_t BT_LE_NIMBLE_GATT_UUID_TO_Z(uint8_t type)
{
    /* Used to convert NimBLE UUID to Zephyr UUID */

    if (type == BLE_UUID_TYPE_16) {
        return BT_UUID_TYPE_16;
    } else if (type == BLE_UUID_TYPE_32) {
        return BT_UUID_TYPE_32;
    } else if (type == BLE_UUID_TYPE_128) {
        return BT_UUID_TYPE_128;
    } else {
        assert(0);
        return BT_UUID_TYPE_16;
    }
}

void bt_le_nimble_gatt_post_event(void *event);

int bt_le_nimble_gatt_post_disc_event(uint16_t conn_handle, ble_uuid16_t *uuid,
                                      struct bt_gatt_discover_params *params,
                                      uint8_t type);

int bt_le_nimble_gatt_post_disc_cmpl_event(uint16_t conn_handle, uint8_t status);

void bt_le_nimble_gatt_handle_event(uint8_t *data, size_t data_len);

uint16_t bt_le_nimble_gatt_get_mtu(struct bt_conn *conn);

ssize_t bt_le_nimble_gatts_attr_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                     void *buf, uint16_t buf_len, uint16_t offset,
                                     const void *value, uint16_t value_len);

int bt_le_nimble_gatts_notify(struct bt_conn *conn, struct bt_gatt_notify_params *params);

int bt_le_nimble_gatts_indicate(struct bt_conn *conn, struct bt_gatt_indicate_params *params);

int bt_le_nimble_gattc_disc_start(uint16_t conn_handle);

int bt_le_nimble_gattc_discover(struct bt_conn *conn, struct bt_gatt_discover_params *params);

int bt_le_nimble_gattc_read(struct bt_conn *conn, struct bt_gatt_read_params *params);

int bt_le_nimble_gattc_write(struct bt_conn *conn, struct bt_gatt_write_params *params);

int bt_le_nimble_gattc_write_without_rsp(struct bt_conn *conn, uint16_t handle,
                                         const void *data, uint16_t length);

int bt_le_nimble_gattc_write_ccc(struct bt_conn *conn, struct bt_gatt_subscribe_params *params);

struct bt_le_gattc_discover_event;

void handle_gattc_db_disc_event_safe(struct bt_le_gattc_discover_event *event);

int bt_le_nimble_gattc_db_disc_svc_by_uuid(struct bt_conn *conn, ble_uuid16_t *uuid,
                                           struct bt_gatt_discover_params *params);

int bt_le_nimble_gattc_db_find_inc_svcs(struct bt_conn *conn,
                                        struct bt_gatt_discover_params *params);

int bt_le_nimble_gattc_db_disc_chrs_by_uuid(struct bt_conn *conn, ble_uuid16_t *uuid,
                                            struct bt_gatt_discover_params *params);

int bt_le_nimble_gattc_db_disc_all_chrs(struct bt_conn *conn,
                                        struct bt_gatt_discover_params *params);

int bt_le_nimble_gattc_db_disc_all_dscs(struct bt_conn *conn,
                                        struct bt_gatt_discover_params *params);

int bt_le_nimble_gattc_db_auto_disc(uint16_t conn_handle);

void bt_le_nimble_gattc_db_remove(uint16_t conn_handle);

enum {
    GATTC_NRP_READ_BY_UUID,
    GATTC_NRP_READ_LONG,
    GATTC_NRP_READ_SINGLE,
    GATTC_NRP_WRITE_REQ,
    GATTC_NRP_SUBSCRIBE,
    GATTS_NRP_INDICATE,

    GATT_NRP_MAX_NUM,
};

void bt_le_nimble_gatts_nrp_indicate_cb(uint16_t conn_handle,
                                        int16_t attr_handle,
                                        uint8_t status);

int bt_le_nimble_gatt_nrp_insert(struct bt_conn *conn, uint8_t type, void *params);

int bt_le_nimble_gatt_nrp_remove(struct bt_conn *conn, uint8_t type, void *params);

void bt_le_nimble_gatt_nrp_clear(uint16_t conn_handle);

#ifdef __cplusplus
}
#endif

#endif /* HOST_NIMBLE_GATT_H_ */
