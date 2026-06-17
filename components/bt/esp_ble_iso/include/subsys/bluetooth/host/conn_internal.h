/** @file
 *  @brief Internal APIs for Bluetooth connection handling.
 */

/*
 * SPDX-FileCopyrightText: 2015 Intel Corporation
 * SPDX-FileCopyrightText: 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#include <zephyr/bluetooth/iso.h>

typedef enum __packed {
    BT_CONN_DISCONNECTED,         /* Disconnected, conn is completely down */
    BT_CONN_DISCONNECT_COMPLETE,  /* Received disconn comp event, transition to DISCONNECTED */

    BT_CONN_INITIATING,           /* Central connection establishment */

    BT_CONN_CONNECTED,            /* Peripheral or Central connected */
    BT_CONN_DISCONNECTING,        /* Peripheral or Central issued disconnection command */
} bt_conn_state_t;

struct bt_conn_le {
    bt_addr_le_t dst;

    uint16_t interval;

    /** @brief Remote LE features
     *
     * Available after `atomic_test_bit(conn->flags, BT_CONN_LE_FEATURES_EXCHANGED)`.
     * Signaled by bt_conn_cb.remote_info_available().
     */
    uint8_t features[8];

    struct bt_keys *keys;
};

struct bt_conn_iso {
    /* Reference to ACL Connection */
    struct bt_conn *acl;

    /* Reference to the struct bt_iso_chan */
    struct bt_iso_chan *chan;

    /** Stored information about the ISO stream */
    struct bt_iso_info info;
};

typedef void (*bt_conn_tx_cb_t)(struct bt_conn *conn, void *user_data, int err);

struct bt_conn_rx {
    /* Index into the bt_conn storage array */
    uint8_t  index;

    /** Connection handle */
    uint16_t handle;
};

struct bt_conn {
    uint16_t handle;
    enum bt_conn_type type;
    uint8_t role;

    /* Which local identity address this connection uses */
    uint8_t id;

    bt_security_t sec_level;
    uint8_t encrypt;

    /* Connection error or reason for disconnect */
    uint8_t err;

    bt_conn_state_t state;
    uint16_t rx_len;
    struct net_buf *rx;

    /* Active L2CAP channels */
    sys_slist_t channels;

    union {
        struct bt_conn_le le;
        struct bt_conn_iso iso;
    };

    /* Get (and clears for ACL conns) callback and user-data for `buf`. */
    void (*get_and_clear_cb)(struct bt_conn *conn, struct net_buf *buf,
                             bt_conn_tx_cb_t *cb, void **ud);
};

/* Cleanup ISO references */
void bt_iso_cleanup_acl(struct bt_conn *iso_conn);

void bt_iso_reset(void);

/* Allocate new connection object */
struct bt_conn *bt_conn_new(struct bt_conn *conns, size_t size);

/* Look up an existing connection */
struct bt_conn *bt_conn_lookup_handle(uint16_t handle, enum bt_conn_type type);

/* Check if the connection is with the given peer. */
bool bt_conn_is_peer_addr_le(const struct bt_conn *conn, uint8_t id,
                             const bt_addr_le_t *peer);

/* Helpers for identifying & looking up connections based on the index to
 * the connection list. This is useful for O(1) lookups, but can't be used
 * e.g. as the handle since that's assigned to us by the controller.
 */
#define BT_CONN_INDEX_INVALID 0xff

/* Set connection object in certain state and perform action related to state */
void bt_conn_set_state(struct bt_conn *conn, bt_conn_state_t state);
