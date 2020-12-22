/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "adv.h"
#include "scan.h"
#include "mesh.h"
#include "test.h"
#include "crypto.h"
#include "access.h"
#include "foundation.h"
#include "mesh_main.h"

#if defined(CONFIG_BLE_MESH_SELF_TEST)

int bt_mesh_test(void)
{
    return 0;
}

#if CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_TEST_AUTO_ENTER_NETWORK
int bt_mesh_device_auto_enter_network(struct bt_mesh_device_network_info *info)
{
    const struct bt_mesh_comp *comp = NULL;
    struct bt_mesh_model *model = NULL;
    struct bt_mesh_elem *elem = NULL;
    struct bt_mesh_app_keys *keys = NULL;
    struct bt_mesh_app_key *key = NULL;
    struct bt_mesh_subnet *sub = NULL;
    int i, j, k;
    int err = 0;

    if (info == NULL || !BLE_MESH_ADDR_IS_UNICAST(info->unicast_addr) ||
            !BLE_MESH_ADDR_IS_GROUP(info->group_addr)) {
        return -EINVAL;
    }

    bt_mesh_atomic_set_bit(bt_mesh.flags, BLE_MESH_NODE);

    /* The device becomes a node and enters the network */
    err = bt_mesh_provision(info->net_key, info->net_idx, info->flags, info->iv_index,
                            info->unicast_addr, info->dev_key);
    if (err) {
        BT_ERR("bt_mesh_provision() failed (err %d)", err);
        return err;
    }

    /* Adds application key to device */
    sub = bt_mesh_subnet_get(info->net_idx);
    if (!sub) {
        BT_ERR("Invalid NetKeyIndex 0x%04x", info->net_idx);
        return -ENODEV;
    }

    for (i = 0; i < ARRAY_SIZE(bt_mesh.app_keys); i++) {
        key = &bt_mesh.app_keys[i];
        if (key->net_idx == BLE_MESH_KEY_UNUSED) {
            break;
        }
    }
    if (i == ARRAY_SIZE(bt_mesh.app_keys)) {
        BT_ERR("Failed to allocate AppKey, 0x%04x", info->app_idx);
        return -ENOMEM;
    }

    keys = sub->kr_flag ? &key->keys[1] : &key->keys[0];

    if (bt_mesh_app_id(info->app_key, &keys->id)) {
        BT_ERR("Failed to calculate AID, 0x%04x", info->app_idx);
        return -EIO;
    }

    key->net_idx = info->net_idx;
    key->app_idx = info->app_idx;
    memcpy(keys->val, info->app_key, 16);

    /* Binds AppKey with all non-config models, adds group address to all these models */
    comp = bt_mesh_comp_get();
    if (!comp) {
        BT_ERR("Invalid composition data");
        return -ENODEV;
    }

    for (i = 0; i < comp->elem_count; i++) {
        elem = &comp->elem[i];
        for (j = 0; j < elem->model_count; j++) {
            model = &elem->models[j];
            if (model->id == BLE_MESH_MODEL_ID_CFG_SRV ||
                    model->id == BLE_MESH_MODEL_ID_CFG_CLI) {
                continue;
            }
            for (k = 0; k < ARRAY_SIZE(model->keys); k++) {
                if (model->keys[k] == BLE_MESH_KEY_UNUSED) {
                    model->keys[k] = info->app_idx;
                    break;
                }
            }
            for (k = 0; k < ARRAY_SIZE(model->groups); k++) {
                if (model->groups[k] == BLE_MESH_ADDR_UNASSIGNED) {
                    model->groups[k] = info->group_addr;
                    break;
                }
            }
        }
        for (j = 0; j < elem->vnd_model_count; j++) {
            model = &elem->vnd_models[j];
            for (k = 0; k < ARRAY_SIZE(model->keys); k++) {
                if (model->keys[k] == BLE_MESH_KEY_UNUSED) {
                    model->keys[k] = info->app_idx;
                    break;
                }
            }
            for (k = 0; k < ARRAY_SIZE(model->groups); k++) {
                if (model->groups[k] == BLE_MESH_ADDR_UNASSIGNED) {
                    model->groups[k] = info->group_addr;
                    break;
                }
            }
        }
    }

    return 0;
}
#endif /* CONFIG_BLE_MESH_NODE && CONFIG_BLE_MESH_TEST_AUTO_ENTER_NETWORK */

#if CONFIG_BLE_MESH_TEST_USE_WHITE_LIST
int bt_mesh_test_update_white_list(struct bt_mesh_white_list *wl)
{
    int err = 0;

    if (wl == NULL) {
        BT_ERR("%s, Invalid parameter", __func__);
        return -EINVAL;
    }

    BT_INFO("%s, addr %s, addr_type 0x%02x", wl->add_remove ? "Add" : "Remove",
        bt_hex(wl->remote_bda, BLE_MESH_ADDR_LEN), wl->addr_type);

    err = bt_le_update_white_list(wl);
    if (err) {
        BT_ERR("Failed to update white list");
    }

    return err;
}

int bt_mesh_test_start_scanning(bool wl_en)
{
    BT_INFO("Scan with filter policy %s", wl_en ? "enabled" : "disabled");

    if (wl_en) {
        return bt_mesh_scan_with_wl_enable();
    } else {
        return bt_mesh_scan_enable();
    }
}

int bt_mesh_test_stop_scanning(void)
{
    return bt_mesh_scan_disable();
}
#endif /* CONFIG_BLE_MESH_TEST_USE_WHITE_LIST */

#endif /* CONFIG_BLE_MESH_SELF_TEST */
