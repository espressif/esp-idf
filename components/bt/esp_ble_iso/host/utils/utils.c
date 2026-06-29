/*
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <zephyr/net_buf.h>
#include <zephyr/logging/log.h>
#include <zephyr/bluetooth/hci_types.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/conn.h>

#include <../host/hci_core.h>

#if CONFIG_BT_BLUEDROID_ENABLED
#include "esp_gap_ble_api.h"
#else
#include "host/ble_gap.h"
#include "host/ble_store.h"
#endif

LOG_MODULE_REGISTER(ISO_UTILS, CONFIG_BT_ISO_LOG_LEVEL);

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
        LOG_WRN("UnknownHciPhy[%u]", hci_phy);
        return 0;
    }
}

/* Query the active host's persistent bond store (the old local key_pool was never
 * populated, so bt_le_bond_exists() was always false). `id` ignored (single identity);
 * addresses stay in native host byte order. */
#if CONFIG_BT_BLUEDROID_ENABLED

void bt_foreach_bond(uint8_t id, void (*func)(const struct bt_bond_info *info,
                                              void *user_data),
                     void *user_data)
{
    int num = esp_ble_get_bond_device_num();
    esp_ble_bond_dev_t *list;

    assert(func);
    (void)id;

    LOG_DBG("[B]ForeachBond[%d]", num);

    if (num <= 0) {
        return;
    }

    list = calloc(num, sizeof(*list));
    if (list == NULL) {
        LOG_ERR("[B]BondAllocFail[%d]", num);
        return;
    }

    if (esp_ble_get_bond_device_list(&num, list) == ESP_OK) {
        for (int i = 0; i < num; i++) {
            struct bt_bond_info info = {0};

            info.addr.type = list[i].bd_addr_type;
            memcpy(info.addr.a.val, list[i].bd_addr, BT_ADDR_SIZE);
            func(&info, user_data);
        }
    } else {
        LOG_WRN("[B]BondReadFail");
    }

    free(list);
}

bool bt_le_bond_exists(uint8_t id, const bt_addr_le_t *addr)
{
    int num = esp_ble_get_bond_device_num();
    esp_ble_bond_dev_t *list;
    bool found = false;

    (void)id;

    if (addr == NULL) {
        LOG_WRN("[B]BondNullAddr");
        return false;
    }

    if (num == 0) {
        LOG_DBG("[B]BondNone");   /* genuinely no bonds stored */
        return false;
    }

    /* A query failure (num<0, OOM, read error) doesn't mean unbonded — the record is
     * intact, we just can't read it. Callers treat false as "not bonded" and drop the
     * peer's CCC/keys, so fail safe to true rather than destroy a real bond's state. */
    if (num < 0) {
        LOG_WRN("[B]BondNumFail[%d]", num);
        return true;
    }

    list = calloc(num, sizeof(*list));
    if (list == NULL) {
        LOG_ERR("[B]BondAllocFail[%d]", num);
        return true;
    }

    if (esp_ble_get_bond_device_list(&num, list) != ESP_OK) {
        LOG_WRN("[B]BondReadFail");
        free(list);
        return true;
    }

    for (int i = 0; i < num; i++) {
        /* Match the 6-byte address only: bd_addr uniquely identifies a bond,
         * and the type encodings (public/random vs *_ID/RPA) do not map 1:1
         * across the host boundary. */
        if (memcmp(list[i].bd_addr, addr->a.val, BT_ADDR_SIZE) == 0) {
            found = true;
            break;
        }
    }

    LOG_INF("[B]BondExists[%d][%02x:%02x:%02x:%02x:%02x:%02x]", found,
            addr->a.val[0], addr->a.val[1], addr->a.val[2],
            addr->a.val[3], addr->a.val[4], addr->a.val[5]);
    free(list);

    return found;
}

#else /* NimBLE host */

void bt_foreach_bond(uint8_t id, void (*func)(const struct bt_bond_info *info,
                                              void *user_data),
                     void *user_data)
{
    ble_addr_t peers[CONFIG_BT_MAX_PAIRED];
    int num = 0;

    assert(func);
    (void)id;

    if (ble_store_util_bonded_peers(peers, &num, ARRAY_SIZE(peers)) != 0) {
        LOG_WRN("[N]BondReadFail");
        return;
    }

    LOG_DBG("[N]ForeachBond[%d]", num);

    for (int i = 0; i < num; i++) {
        struct bt_bond_info info = {0};

        info.addr.type = peers[i].type;
        memcpy(info.addr.a.val, peers[i].val, BT_ADDR_SIZE);
        func(&info, user_data);
    }
}

bool bt_le_bond_exists(uint8_t id, const bt_addr_le_t *addr)
{
    struct ble_store_value_sec value = {0};
    struct ble_store_key_sec key = {0};

    (void)id;

    if (addr == NULL) {
        LOG_WRN("[N]BondNullAddr");
        return false;
    }

    key.peer_addr.type = addr->type;
    memcpy(key.peer_addr.val, addr->a.val, BT_ADDR_SIZE);

    bool found = ble_store_read_peer_sec(&key, &value) == 0;

    LOG_INF("[N]BondExists[%d][%02x:%02x:%02x:%02x:%02x:%02x]", found,
            addr->a.val[0], addr->a.val[1], addr->a.val[2],
            addr->a.val[3], addr->a.val[4], addr->a.val[5]);

    return found;
}

#endif /* CONFIG_BT_BLUEDROID_ENABLED */
