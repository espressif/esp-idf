/* hci_core.h - Bluetooth HCI core access */

/*
 * SPDX-FileCopyrightText: 2021 Nordic Semiconductor ASA
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/* bt_dev flags: the flags defined here represent BT controller state */
enum {
    /** The application either explicitly or implicitly instructed the stack to scan
     * for advertisers.
     *
     * Examples of such cases
     *  - Explicit scanning, @ref BT_LE_SCAN_USER_EXPLICIT_SCAN.
     *  - The application instructed the stack to automatically connect if a given device
     *    is detected.
     *  - The application wants to connect to a peer device using private addresses, but
     *    the controller resolving list is too small. The host will fallback to using
     *    host-based privacy and first scan for the device before it initiates a connection.
     *  - The application wants to synchronize to a periodic advertiser.
     *    The host will implicitly start scanning if it is not already doing so.
     *
     * The host needs to keep track of this state to ensure it can restart scanning
     * when a connection is established/lost, explicit scanning is started or stopped etc.
     * Also, when the scanner and advertiser share the same identity, the scanner may need
     * to be restarted upon RPA refresh.
     */
    BT_DEV_SCANNING,

    /* Total number of flags - must be at the end of the enum */
    BT_DEV_NUM_FLAGS,
};

enum {
    /* Periodic Advertising parameters has been set in the controller. */
    BT_PER_ADV_PARAMS_SET,

    BT_ADV_NUM_FLAGS,
};

struct bt_le_ext_adv {
    /* Advertising handle */
    uint8_t handle;

    ATOMIC_DEFINE(flags, BT_ADV_NUM_FLAGS);
};

enum {
    /** Periodic Advertising Sync has been created in the host. */
    BT_PER_ADV_SYNC_CREATED,

    /** Periodic Advertising Sync is established and can be terminated */
    BT_PER_ADV_SYNC_SYNCED,

    /** Periodic Advertising Sync is attempting to create sync */
    BT_PER_ADV_SYNC_SYNCING,

    BT_PER_ADV_SYNC_NUM_FLAGS,
};

struct bt_le_per_adv_sync {
    /** Periodic Advertiser Address */
    bt_addr_le_t addr;

    /** Advertiser SID */
    uint8_t sid;

    /** Sync handle */
    uint16_t handle;

    /** Periodic advertising interval (N * 1.25 ms) */
    uint16_t interval;

    /** Advertiser PHY */
    uint8_t phy;

    /** Flags */
    ATOMIC_DEFINE(flags, BT_PER_ADV_SYNC_NUM_FLAGS);
};

struct bt_dev_le {
    /* LE features */
    uint8_t features[8];
};

/* State tracking for the local Bluetooth controller */
struct bt_dev {
    ATOMIC_DEFINE(flags, BT_DEV_NUM_FLAGS);

    /* LE controller specific features */
    struct bt_dev_le le;
};

extern struct bt_dev bt_dev;

/* Data type to store state related with command to be updated
 * when command completes successfully.
 */
struct bt_hci_cmd_state_set {
    /* Dummy */
};

uint8_t bt_get_phy(uint8_t hci_phy);

bool bt_le_bond_exists(uint8_t id, const bt_addr_le_t *addr);
