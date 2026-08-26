/** @file
 *  @brief Internal APIs for Bluetooth ISO handling.
 */

/*
 * SPDX-FileCopyrightText: 2020 Intel Corporation
 * SPDX-FileCopyrightText: 2021-2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/buf.h>
#include <zephyr/bluetooth/iso.h>
#include <zephyr/kernel.h>
#include <zephyr/net_buf.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/sys/slist.h>

enum bt_iso_cig_state {
    BT_ISO_CIG_STATE_IDLE,
    BT_ISO_CIG_STATE_CONFIGURED,
    BT_ISO_CIG_STATE_ACTIVE,
    BT_ISO_CIG_STATE_INACTIVE
};

struct bt_iso_cig {
    /** List of ISO channels to setup as CIS (the CIG). */
    sys_slist_t cis_channels;

    /** Total number of CISes in the CIG. */
    uint8_t num_cis;

    /** The CIG ID */
    uint8_t id;

    /** The CIG state
     *
     * Refer to BT Core Spec 5.3, Vol 6, Part 6, Figure 4.63
     */
    enum bt_iso_cig_state state;
};

enum {
    BT_BIG_INITIALIZED,

    /* Creating a BIG as a broadcaster */
    BT_BIG_PENDING,
    /* Creating a BIG as a receiver */
    BT_BIG_SYNCING,
    /* BIG is busy handling an HCI event.
     *
     * Use this to prevent API calls from modifying the BIG while the event is being processed.
     */
    BT_BIG_BUSY,

    BT_BIG_NUM_FLAGS,
};

struct bt_iso_big {
    /** List of ISO channels to setup as BIS (the BIG). */
    sys_slist_t bis_channels;

    /** Total number of BISes in the BIG. */
    uint8_t num_bis;

    /** The BIG handle */
    uint8_t handle;

    ATOMIC_DEFINE(flags, BT_BIG_NUM_FLAGS);
};

#define iso(buf) ((struct bt_conn_rx *)net_buf_user_data(buf))

/* Process ISO buffer */
void hci_iso(struct net_buf *buf);

/* Process CIS Established event */
void hci_le_cis_established(struct net_buf *buf);
void hci_le_cis_established_v2(struct net_buf *buf);

/* Process CIS Request event */
void hci_le_cis_req(struct net_buf *buf);

/** Process BIG complete event */
void hci_le_big_complete(struct net_buf *buf);

/** Process BIG terminate event */
void hci_le_big_terminate(struct net_buf *buf);

/** Process BIG sync established event */
void hci_le_big_sync_established(struct net_buf *buf);

/** Process BIG sync lost event */
void hci_le_big_sync_lost(struct net_buf *buf);

/* Notify ISO channels of a new connection */
void bt_iso_connected(struct bt_conn *iso);

/* Notify ISO channels of a disconnect event */
void bt_iso_disconnected(struct bt_conn *iso);

#if defined(CONFIG_BT_ISO_LOG_LEVEL_DBG)
void bt_iso_chan_set_state_debug(struct bt_iso_chan *chan,
                                 enum bt_iso_state state,
                                 const char *func, int line);
#define bt_iso_chan_set_state(_chan, _state) \
    bt_iso_chan_set_state_debug(_chan, _state, __func__, __LINE__)
#else
void bt_iso_chan_set_state(struct bt_iso_chan *chan, enum bt_iso_state state);
#endif /* CONFIG_BT_ISO_LOG_LEVEL_DBG */

/* Process incoming data for a connection */
void bt_iso_recv(struct bt_conn *iso, struct net_buf *buf, uint8_t flags);

/* Whether the HCI ISO data packet contains a timestamp or not.
 * Per spec, the TS flag can only be set for the first fragment.
 */
enum bt_iso_timestamp {
    BT_ISO_TS_ABSENT = 0,
    BT_ISO_TS_PRESENT,
};
