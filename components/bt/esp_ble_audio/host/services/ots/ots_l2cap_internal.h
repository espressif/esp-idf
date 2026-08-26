/*
 * SPDX-FileCopyrightText: 2020-2022 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef BT_GATT_OTS_L2CAP_H_
#define BT_GATT_OTS_L2CAP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr/types.h>
#include <zephyr/sys/slist.h>
#include <sys/types.h>

#include <zephyr/bluetooth/l2cap.h>

struct bt_gatt_ots_l2cap_tx {
    uint8_t *data;
    uint32_t len;
    uint32_t len_sent;
};

struct bt_gatt_ots_l2cap {
    sys_snode_t node;
    struct bt_l2cap_le_chan ot_chan;
    struct bt_gatt_ots_l2cap_tx tx;
    void (*tx_done)(struct bt_gatt_ots_l2cap *l2cap_ctx,
                    struct bt_conn *conn);
    ssize_t (*rx_done)(struct bt_gatt_ots_l2cap *l2cap_ctx,
                       struct bt_conn *conn, struct net_buf *buf);
    void (*closed)(struct bt_gatt_ots_l2cap *l2cap_ctx,
                   struct bt_conn *conn);
};

bool bt_gatt_ots_l2cap_is_open(struct bt_gatt_ots_l2cap *l2cap_ctx,
                               struct bt_conn *conn);

int bt_gatt_ots_l2cap_send(struct bt_gatt_ots_l2cap *l2cap_ctx,
                           uint8_t *data,
                           uint32_t len);

int bt_gatt_ots_l2cap_register(struct bt_gatt_ots_l2cap *l2cap_ctx);

int bt_gatt_ots_l2cap_unregister(struct bt_gatt_ots_l2cap *l2cap_ctx);

/** @brief Connect OTS L2CAP channel
 *
 *  This function is for the OTS client to make an L2CAP connection to
 *  the OTS server.
 *
 * @param[in]     conn       Connection pointer
 * @param[in,out] l2cap_ctx  On entry, the caller's context to connect, or NULL
 *                           to pick any free registered one. On success, set
 *                           to the connected context; NULL on failure.
 *
 * @return     0 in case of success or negative value in case of error
 */
int bt_gatt_ots_l2cap_connect(struct bt_conn *conn,
                              struct bt_gatt_ots_l2cap **l2cap_ctx);

int bt_gatt_ots_l2cap_disconnect(struct bt_gatt_ots_l2cap *l2cap_ctx);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* BT_GATT_OTS_L2CAP_H_ */
