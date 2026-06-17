/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include <errno.h>

#include "mesh.h"
#include "lpn.h"
#include "crypto.h"
#include "access.h"
#include "foundation.h"
#include "transport.h"
#include "mesh/main.h"
#include "settings.h"

static struct bt_mesh_model *find_model(uint16_t elem_addr, uint16_t cid, uint16_t mod_id)
{
    struct bt_mesh_elem *elem = NULL;

    BT_DBG("FindModelLocal");
    BT_DBG("ElemAddr 0x%04x CID 0x%04x ID 0x%04x", elem_addr, cid, mod_id);

    if (!BLE_MESH_ADDR_IS_UNICAST(elem_addr)) {
        BT_ERR("InvalidUnicastAddr 0x%04x", elem_addr);
        return NULL;
    }

    elem = bt_mesh_elem_find(elem_addr);
    if (elem == NULL) {
        BT_ERR("NoElemFound 0x%04x", elem_addr);
        return NULL;
    }

    if (cid == BLE_MESH_CID_NVAL) {
        return bt_mesh_model_find(elem, mod_id);
    }

    return bt_mesh_model_find_vnd(elem, cid, mod_id);
}

int bt_mesh_model_subscribe_group_addr(uint16_t elem_addr, uint16_t cid,
                                       uint16_t mod_id, uint16_t group_addr)
{
    struct bt_mesh_model *model = NULL;
    int i;

    BT_DBG("ModelSubGroupAddrLocal");
    BT_DBG("ElemAddr 0x%04x CID 0x%04x ID 0x%04x GroupAddr 0x%04x",
           elem_addr, cid, mod_id, group_addr);

    model = find_model(elem_addr, cid, mod_id);
    if (model == NULL) {
        BT_ERR("ModelNotFound, CID 0x%04x ID 0x%04x", cid, mod_id);
        return -ENODEV;
    }

    if (!BLE_MESH_ADDR_IS_GROUP(group_addr)) {
        BT_ERR("NotGroupAddr 0x%04x", group_addr);
        return -EINVAL;
    }

    if (bt_mesh_model_find_group(model, group_addr)) {
        BT_INFO("GroupAddrExist 0x%04x", group_addr);
        return 0;
    }

    for (i = 0; i < ARRAY_SIZE(model->groups); i++) {
        if (model->groups[i] == BLE_MESH_ADDR_UNASSIGNED) {
            model->groups[i] = group_addr;

            if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
                bt_mesh_store_mod_sub(model);
            }

            if (IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER)) {
                bt_mesh_lpn_group_add(group_addr);
            }

            return 0;
        }
    }

    BT_ERR("ModelSubFull");
    return -ENOMEM;
}

int bt_mesh_model_unsubscribe_group_addr(uint16_t elem_addr, uint16_t cid,
                                         uint16_t mod_id, uint16_t group_addr)
{
    struct bt_mesh_model *model = NULL;
    uint16_t *match = NULL;

    BT_DBG("ModelUnsubGroupAddrLocal");
    BT_DBG("ElemAddr 0x%04x CID 0x%04x ID 0x%04x GroupAddr 0x%04x",
           elem_addr, cid, mod_id, group_addr);

    model = find_model(elem_addr, cid, mod_id);
    if (model == NULL) {
        BT_ERR("ModelNotFound, CID 0x%04x ID 0x%04x", cid, mod_id);
        return -ENODEV;
    }

    if (!BLE_MESH_ADDR_IS_GROUP(group_addr)) {
        BT_ERR("NotGroupAddr 0x%04x", group_addr);
        return -EINVAL;
    }

    match = bt_mesh_model_find_group(model, group_addr);
    if (match == NULL) {
        BT_WARN("GroupAddrNotExist 0x%04x", group_addr);
        return -ENOENT;
    }

    *match = BLE_MESH_ADDR_UNASSIGNED;

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_mod_sub(model);
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER)) {
        bt_mesh_lpn_group_del(&group_addr, 1);
    }

    return 0;
}

#if CONFIG_BLE_MESH_DF_SRV
int bt_mesh_enable_directed_forwarding(uint16_t net_idx, bool directed_forwarding,
                                       bool directed_forwarding_relay)
{
    struct bt_mesh_subnet *sub = NULL;

    BT_DBG("EnableDFLocal, NetIdx 0x%04x DF %u DFRelay %u",
           net_idx, directed_forwarding, directed_forwarding_relay);

    if (net_idx > 0xFFF) {
        BT_ERR("InvalidNetIdx 0x%04x", net_idx);
        return -EINVAL;
    }

    sub = bt_mesh_subnet_get(net_idx);
    if (!sub) {
        BT_ERR("NetIdxNotExist 0x%04x", net_idx);
        return -EINVAL;
    }

    if (directed_forwarding == BLE_MESH_DIRECTED_FORWARDING_DISABLED &&
        directed_forwarding_relay == BLE_MESH_DIRECTED_RELAY_ENABLED) {
        BT_ERR("InvalidDFConfig %u/%u",
               directed_forwarding, directed_forwarding_relay);
        return -EINVAL;
    }

    sub->directed_forwarding = directed_forwarding;
    sub->directed_relay = directed_forwarding_relay;

    return 0;
}
#endif /* CONFIG_BLE_MESH_DF_SRV */

#if CONFIG_BLE_MESH_NODE
const uint8_t *bt_mesh_node_get_local_net_key(uint16_t net_idx)
{
    struct bt_mesh_subnet *sub = NULL;

    BT_DBG("NodeGetNetKeyLocal, NetIdx 0x%04x", net_idx);

    if (net_idx > 0xFFF) {
        BT_ERR("InvalidNetIdx 0x%04x", net_idx);
        return NULL;
    }

    sub = bt_mesh_subnet_get(net_idx);
    if (!sub) {
        BT_ERR("NetIdxNotExist 0x%04x", net_idx);
        return NULL;
    }

    return sub->kr_flag ? sub->keys[1].net : sub->keys[0].net;
}

const uint8_t *bt_mesh_node_get_local_app_key(uint16_t app_idx)
{
    struct bt_mesh_app_key *key = NULL;

    BT_DBG("NodeGetAppKeyLocal, AppIdx 0x%04x", app_idx);

    if (app_idx > 0xFFF) {
        BT_ERR("InvalidAppIdx 0x%04x", app_idx);
        return NULL;
    }

    key = bt_mesh_app_key_get(app_idx);
    if (!key) {
        BT_ERR("AppIdxNotExist 0x%04x", app_idx);
        return NULL;
    }

    return key->updated ? key->keys[1].val : key->keys[0].val;
}

int bt_mesh_node_local_net_key_add(uint16_t net_idx, const uint8_t net_key[16])
{
    struct bt_mesh_subnet *sub = NULL;
    int err = 0;
    int i;

    BT_DBG("NodeAddNetKeyLocal, NetIdx 0x%04x", net_idx);

    if (net_idx > 0xFFF || net_key == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    if (!bt_mesh_is_provisioned()) {
        BT_ERR("NotProvisioned");
        return -EIO;
    }

    sub = bt_mesh_subnet_get(net_idx);
    if (sub) {
        BT_WARN("NetIdxExist 0x%04x", net_idx);
        return -EEXIST;
    }

    for (i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
        if (bt_mesh.sub[i].net_idx != BLE_MESH_KEY_UNUSED) {
            if ((bt_mesh.sub[i].kr_flag == false &&
                memcmp(bt_mesh.sub[i].keys[0].net, net_key, 16) == 0) ||
                (bt_mesh.sub[i].kr_flag == true &&
                memcmp(bt_mesh.sub[i].keys[1].net, net_key, 16) == 0)) {
                BT_WARN("NetKeyValExist %s", bt_hex(net_key, 16));
                return -EEXIST;
            }
        }
    }

    for (i = 0; i < ARRAY_SIZE(bt_mesh.sub); i++) {
        if (bt_mesh.sub[i].net_idx == BLE_MESH_KEY_UNUSED) {
            sub = &bt_mesh.sub[i];
            break;
        }
    }

    if (sub == NULL) {
        BT_ERR("NetKeyFull");
        return -ENOMEM;
    }

    err = bt_mesh_net_keys_create(&sub->keys[0], net_key);
    if (err) {
        BT_ERR("NetKeyCreateFail 0x%04x", net_idx);
        return -EIO;
    }

    sub->net_idx = net_idx;
    sub->kr_flag = false;
    sub->kr_phase = BLE_MESH_KR_NORMAL;
    if (IS_ENABLED(CONFIG_BLE_MESH_GATT_PROXY_SERVER)) {
        sub->node_id = BLE_MESH_NODE_IDENTITY_STOPPED;
    } else {
        sub->node_id = BLE_MESH_NODE_IDENTITY_NOT_SUPPORTED;
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_subnet(sub);
    }

    /* Make sure we have valid beacon data to be sent */
    bt_mesh_net_secure_beacon_update(sub);

    return 0;
}

int bt_mesh_node_local_app_key_add(uint16_t net_idx, uint16_t app_idx,
                                   const uint8_t app_key[16])
{
    struct bt_mesh_app_key *key = NULL;

    BT_DBG("NodeAddAppKeyLocal, NetIdx 0x%04x AppIdx 0x%04x", net_idx, app_idx);

    if (net_idx > 0xFFF || app_idx > 0xFFF || app_key == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    if (!bt_mesh_is_provisioned()) {
        BT_ERR("NotProvisioned");
        return -EIO;
    }

    if (bt_mesh_subnet_get(net_idx) == NULL) {
        BT_ERR("NetIdxNotExist 0x%04x", net_idx);
        return -EIO;
    }

    key = bt_mesh_app_key_get(app_idx);
    if (key) {
        BT_WARN("AppIdxExist 0x%04x", app_idx);
        return -EEXIST;
    }

    for (int i = 0; i < ARRAY_SIZE(bt_mesh.app_keys); i++) {
        if (bt_mesh.app_keys[i].net_idx != BLE_MESH_KEY_UNUSED) {
            if ((bt_mesh.app_keys[i].updated == false &&
                memcmp(bt_mesh.app_keys[i].keys[0].val, app_key, 16) == 0) ||
                (bt_mesh.app_keys[i].updated == true &&
                memcmp(bt_mesh.app_keys[i].keys[1].val, app_key, 16) == 0)) {
                BT_WARN("AppKeyValExist %s", bt_hex(app_key, 16));
                return -EEXIST;
            }
        }
    }

    key = bt_mesh_app_key_alloc(app_idx);
    if (key) {
        struct bt_mesh_app_keys *keys = &key->keys[0];

        if (bt_mesh_app_id(app_key, &keys->id)) {
            BT_ERR("GenAIDFail");
            return -EIO;
        }

        key->net_idx = net_idx;
        key->app_idx = app_idx;
        key->updated = false;
        memcpy(keys->val, app_key, 16);

        if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
            bt_mesh_store_app_key(key);
        }

        return 0;
    }

    return -ENOMEM;
}

int bt_mesh_node_bind_app_key_to_model(uint16_t elem_addr, uint16_t mod_id,
                                       uint16_t cid, uint16_t app_idx)
{
    struct bt_mesh_model *model = NULL;
    int i;

    BT_DBG("NodeBindAppKeyLocal");
    BT_DBG("ElemAddr 0x%04x CID 0x%04x ID 0x%04x AppIdx 0x%04x",
           elem_addr, cid, mod_id, app_idx);

    if (!bt_mesh_is_provisioned()) {
        BT_ERR("NotProvisioned");
        return -EIO;
    }

    model = find_model(elem_addr, cid, mod_id);
    if (model == NULL) {
        BT_ERR("ModelNotFound, ID 0x%04x CID 0x%04x", mod_id, cid);
        return -ENODEV;
    }

    if (bt_mesh_app_key_get(app_idx) == NULL) {
        BT_ERR("AppIdxNotExist 0x%04x", app_idx);
        return -ENODEV;
    }

    for (i = 0; i < ARRAY_SIZE(model->keys); i++) {
        if (model->keys[i] == app_idx) {
            BT_WARN("AppIdxBound 0x%04x", app_idx);
            return -EALREADY;
        }
    }

    for (i = 0; i < ARRAY_SIZE(model->keys); i++) {
        if (model->keys[i] == BLE_MESH_KEY_UNUSED) {
            model->keys[i] = app_idx;

            if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
                bt_mesh_store_mod_bind(model);
            }

            return 0;
        }
    }

    BT_ERR("ModelBindFull");
    return -ENOMEM;
}
#endif /* CONFIG_BLE_MESH_NODE */
