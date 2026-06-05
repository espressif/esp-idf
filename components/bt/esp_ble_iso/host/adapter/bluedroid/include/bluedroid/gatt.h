/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HOST_BLUEDROID_GATT_H_
#define HOST_BLUEDROID_GATT_H_

#include <stdint.h>
#include <assert.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/slist.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>

#ifdef __cplusplus
extern "C" {
#endif

struct gatt_conn {
    uint8_t  used : 1;
    uint8_t  conn_create : 1;
    uint8_t  gattc_open : 1;
    uint8_t  cfg_mtu : 1;
    uint8_t  mtu_posted : 1;

    uint8_t  status;
    uint8_t  gatt_if;
    uint16_t conn_handle;
    uint8_t  role;
    struct {
        uint8_t type;
        uint8_t val[6];
    } peer;

    uint16_t mtu;

    /* FIFO of in-flight GATTC read/write ops; mirrors BTA p_cmd_list ordering.
     * BTA EVTs deliver attr_handle/status only — we need this to recover the
     * caller's params on each cmpl. See struct gattc_list_node in gatt.c. */
    sys_slist_t gattc_list;

    /* FIFO of pending GATTS indications. GATT spec restricts per-conn to a
     * single outstanding indication; this queue absorbs that limit so callers
     * never see -EBUSY. See struct gatts_list_node in gatt.c. Mirrors
     * NimBLE adapter NRP indicate behavior. */
    sys_slist_t gatts_list;

    /* FIFO of in-flight GATTS notify markers. BTA fires CONF_EVT for notify
     * too (immediately after send) — tBTA_GATTS_REQ carries no notify/indicate
     * flag so the handler pops this list first to disambiguate from the
     * indication acks tracked in gatts_list. See struct gatts_notify_node. */
    sys_slist_t gatts_notify_list;
};

uint8_t bt_le_bluedroid_gattc_get_if(void);

uint8_t bt_le_bluedroid_gatts_get_if(void);

void bt_le_bluedroid_gatts_sem_reset(void);

int bt_le_bluedroid_gatts_sem_take(void);

void bt_le_bluedroid_gatts_sem_give(int result);

struct gatt_conn *bt_le_bluedroid_find_gatt_conn_with_addr(uint8_t addr_type,
                                                           const uint8_t addr[6],
                                                           bool ignore_type);

struct gatt_conn *bt_le_bluedroid_find_gatt_conn_with_handle(uint16_t conn_handle);

struct gatt_conn *bt_le_bluedroid_find_free_gatt_conn(void);

void bt_le_bluedroid_gatt_handle_event(uint8_t *data, size_t data_len);

void bt_le_bluedroid_gatt_uuid_convert(const struct bt_uuid *uuid_in, void *uuid_out);

uint16_t bt_le_bluedroid_gatt_perm_convert(uint16_t perm_in);

uint16_t bt_le_bluedroid_gatt_get_mtu(struct bt_conn *conn);

ssize_t bt_le_bluedroid_gatts_attr_read(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                                        void *buf, uint16_t buf_len, uint16_t offset,
                                        const void *value, uint16_t value_len);

int bt_le_bluedroid_gatts_notify(struct bt_conn *conn, struct bt_gatt_notify_params *params);

int bt_le_bluedroid_gatts_indicate(struct bt_conn *conn, struct bt_gatt_indicate_params *params);

int bt_le_bluedroid_gattc_disc_start(uint16_t conn_handle);

int bt_le_bluedroid_gattc_discover(struct bt_conn *conn, struct bt_gatt_discover_params *params);

int bt_le_bluedroid_gattc_read(struct bt_conn *conn, struct bt_gatt_read_params *params);

int bt_le_bluedroid_gattc_write(struct bt_conn *conn, struct bt_gatt_write_params *params);

int bt_le_bluedroid_gattc_write_without_rsp(struct bt_conn *conn, uint16_t handle,
                                            const void *data, uint16_t length);

int bt_le_bluedroid_gattc_write_ccc(struct bt_conn *conn, struct bt_gatt_subscribe_params *params);

int bt_le_bluedroid_gatt_init(void);

void bt_le_bluedroid_gatt_deinit(void);

struct inc_svc_inst {
    struct bt_gatt_service *svc_p;
    bool included;
};

struct gatts_svc_cb {
    void (*svc_create_cb)(uint16_t service_id, uint16_t svc_instance,
                          bool is_primary, uint8_t status, void *uuid);

    void (*inc_svc_add_cb)(uint16_t service_id, uint16_t attr_id, uint8_t status);

    void (*chrc_add_cb)(uint16_t service_id, uint16_t attr_id,
                        uint8_t status, void *uuid);

    void (*svc_start_cb)(uint16_t service_id, uint8_t status);
};

void bt_le_bluedroid_gatts_svc_cb_register(struct gatts_svc_cb *cb);

#ifdef __cplusplus
}
#endif

#endif /* HOST_BLUEDROID_GATT_H_ */
