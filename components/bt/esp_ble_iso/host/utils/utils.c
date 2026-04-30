/*
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include <zephyr/net_buf.h>
#include <zephyr/bluetooth/hci_types.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/conn.h>

#include <../host/keys.h>
#include <../host/hci_core.h>

struct bt_dev bt_dev;

uint8_t bt_get_phy(uint8_t hci_phy)
{
    switch (hci_phy) {
    case BT_HCI_LE_PHY_1M:
        return BT_GAP_LE_PHY_1M;
    case BT_HCI_LE_PHY_2M:
        return BT_GAP_LE_PHY_2M;
    case BT_HCI_LE_PHY_CODED:
        return BT_GAP_LE_PHY_CODED;
    default:
        return 0;
    }
}

static struct bt_keys key_pool[CONFIG_BT_MAX_PAIRED];

void bt_foreach_bond(uint8_t id, void (*func)(const struct bt_bond_info *info,
                                              void *user_data),
                     void *user_data)
{
    assert(func);

    for (size_t i = 0; i < ARRAY_SIZE(key_pool); i++) {
        struct bt_keys *keys = &key_pool[i];

        if (keys->keys && keys->id == id) {
            struct bt_bond_info info;

            bt_addr_le_copy(&info.addr, &keys->addr);
            func(&info, user_data);
        }
    }
}

bool bt_le_bond_exists(uint8_t id, const bt_addr_le_t *addr)
{
    if (addr == NULL) {
        return false;
    }

    for (size_t i = 0; i < ARRAY_SIZE(key_pool); i++) {
        struct bt_keys *keys = &key_pool[i];

        if (keys->keys && keys->id == id &&
            bt_addr_le_eq(&keys->addr, addr)) {
            return true;
        }
    }

    return false;
}
