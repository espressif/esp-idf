/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "mesh.h"
#include "mesh/common.h"
#include "mesh/main.h"
#include "access.h"
#include "beacon.h"
#include "foundation.h"
#include "proxy_client.h"
#include "prov_pvnr.h"
#include "pvnr_mgmt.h"

#if CONFIG_BLE_MESH_FAST_PROV

#define ACTION_ENTER    0x01
#define ACTION_SUSPEND  0x02
#define ACTION_EXIT     0x03

const uint8_t *bt_mesh_fast_prov_dev_key_get(uint16_t dst)
{
    const uint8_t *key = NULL;

    if (!BLE_MESH_ADDR_IS_UNICAST(dst)) {
        BT_ERR("Invalid unicast address 0x%04x", dst);
        return NULL;
    }

    if (bt_mesh_is_provisioner_en() == false) {
        return bt_mesh.dev_key;
    }

    /* For fast provisioning, try to find the DevKey from
     * the database firstly. If the dst is not in the DB,
     * then we will directly use the DevKey.
     */
    key = bt_mesh_provisioner_dev_key_get(dst);
    if (key) {
        return key;
    }

    return bt_mesh.dev_key;
}

struct bt_mesh_subnet *bt_mesh_fast_prov_subnet_get(uint16_t net_idx)
{
    struct bt_mesh_subnet *sub = NULL;
    int i;

    for (i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
        sub = &bt_mesh.sub[i];
        if (sub->net_idx == net_idx) {
            return sub;
        }
    }

    for (i = 0; i < ARRAY_SIZE(bt_mesh.p_sub); i++) {
        sub = bt_mesh.p_sub[i];
        if (sub && sub->net_idx == net_idx) {
            return sub;
        }
    }

    return NULL;
}

struct bt_mesh_app_key *bt_mesh_fast_prov_app_key_find(uint16_t app_idx)
{
    struct bt_mesh_app_key *key = NULL;
    int i;

    for (i = 0; i < ARRAY_SIZE(bt_mesh.app_keys); i++) {
        key = &bt_mesh.app_keys[i];
        if (key->net_idx != BLE_MESH_KEY_UNUSED &&
            key->app_idx == app_idx) {
            return key;
        }
    }

    for (i = 0; i < ARRAY_SIZE(bt_mesh.p_app_keys); i++) {
        key = bt_mesh.p_app_keys[i];
        if (key && key->net_idx != BLE_MESH_KEY_UNUSED &&
            key->app_idx == app_idx) {
            return key;
        }
    }

    return NULL;
}

uint8_t bt_mesh_set_fast_prov_net_idx(uint16_t net_idx)
{
    /* Set net_idx for fast provisioning */
    bt_mesh_provisioner_set_fast_prov_net_idx(net_idx);

    if (bt_mesh_fast_prov_subnet_get(net_idx) == NULL) {
        /* If NetKey is not found, wait for NetKey to be added. */
        BT_WARN("Wait for NetKey for fast provisioning");
        return 0x01; /*status: Wait for NetKey */
    }

    return 0x0; /* status: Succeed */
}

uint8_t bt_mesh_fast_prov_net_key_add(const uint8_t net_key[16])
{
    uint16_t net_idx = 0U;
    int err = 0;

    net_idx = bt_mesh_provisioner_get_fast_prov_net_idx();
    bt_mesh.p_net_idx_next = net_idx;

    err = bt_mesh_provisioner_local_net_key_add(net_key, &net_idx);
    if (err) {
        BT_ERR("Invalid NetKeyIndex 0x%04x", net_idx);
        return 0x01; /* status: Add NetKey failed */
    };

    bt_mesh_provisioner_set_fast_prov_net_idx(net_idx);
    return 0x0; /* status: Succeed */
}

const uint8_t *bt_mesh_fast_prov_net_key_get(uint16_t net_idx)
{
    struct bt_mesh_subnet *sub = NULL;

    sub = bt_mesh_fast_prov_subnet_get(net_idx);
    if (!sub) {
        BT_ERR("Invalid NetKeyIndex 0x%04x", net_idx);
        return NULL;
    }

    return (sub->kr_flag ? sub->keys[1].net : sub->keys[0].net);
}

const uint8_t *bt_mesh_get_fast_prov_app_key(uint16_t net_idx, uint16_t app_idx)
{
    struct bt_mesh_app_key *key = NULL;

    key = bt_mesh_fast_prov_app_key_find(app_idx);
    if (!key) {
        BT_ERR("Invalid AppKeyIndex 0x%04x", app_idx);
        return NULL;
    }

    return (key->updated ? key->keys[1].val : key->keys[0].val);
}

uint8_t bt_mesh_set_fast_prov_action(uint8_t action)
{
    if (!action || action > ACTION_EXIT) {
        return 0x01;
    }

    if ((!bt_mesh_is_provisioner_en() && (action == ACTION_SUSPEND || action == ACTION_EXIT)) ||
        (bt_mesh_is_provisioner_en() && (action == ACTION_ENTER))) {
        BT_WARN("%s, Already", __func__);
        return 0x0;
    }

    if (action == ACTION_ENTER) {
        if (bt_mesh_secure_beacon_get() == BLE_MESH_SECURE_BEACON_ENABLED) {
            bt_mesh_secure_beacon_disable();
        }
        if (IS_ENABLED(CONFIG_BLE_MESH_PB_GATT)) {
            bt_mesh_proxy_client_prov_enable();
        }
        bt_mesh_provisioner_set_primary_elem_addr(bt_mesh_primary_addr());
        bt_mesh_provisioner_set_prov_bearer(BLE_MESH_PROV_ADV, false);
        bt_mesh_provisioner_fast_prov_enable(true);
        bt_mesh_atomic_or(bt_mesh.flags, BIT(BLE_MESH_PROVISIONER) | BIT(BLE_MESH_VALID_PROV));
    } else {
        if (IS_ENABLED(CONFIG_BLE_MESH_PB_GATT)) {
            bt_mesh_proxy_client_prov_disable();
        }
        if (bt_mesh_secure_beacon_get() == BLE_MESH_SECURE_BEACON_ENABLED) {
            bt_mesh_secure_beacon_enable();
        }
        bt_mesh_atomic_and(bt_mesh.flags, ~(BIT(BLE_MESH_PROVISIONER) | BIT(BLE_MESH_VALID_PROV)));
        bt_mesh_provisioner_fast_prov_enable(false);
        if (action == ACTION_EXIT) {
            bt_mesh_provisioner_remove_node(NULL);
        }
    }

    return 0x0;
}
#endif /* CONFIG_BLE_MESH_FAST_PROV */
