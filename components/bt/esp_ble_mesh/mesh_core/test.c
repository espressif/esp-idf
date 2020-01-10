/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

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
#endif /* #if defined(CONFIG_BLE_MESH_SELF_TEST) */

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

    /* The device becomes a node and enters the network */
    err = bt_mesh_provision(info->net_key, info->net_idx, info->flags, info->iv_index,
                            info->unicast_addr, info->dev_key);
    if (err) {
        BT_ERR("%s, bt_mesh_provision() failed (err %d)", __func__, err);
        return err;
    }

    /* Adds application key to device */
    sub = bt_mesh_subnet_get(info->net_idx);
    if (!sub) {
        BT_ERR("%s, Failed to find subnet 0x%04x", __func__, info->net_idx);
        return -ENODEV;
    }

    for (i = 0; i < ARRAY_SIZE(bt_mesh.app_keys); i++) {
        key = &bt_mesh.app_keys[i];
        if (key->net_idx == BLE_MESH_KEY_UNUSED) {
            break;
        }
    }
    if (i == ARRAY_SIZE(bt_mesh.app_keys)) {
        BT_ERR("%s, Failed to allocate memory, AppKeyIndex 0x%04x", __func__, info->app_idx);
        return -ENOMEM;
    }

    keys = sub->kr_flag ? &key->keys[1] : &key->keys[0];

    if (bt_mesh_app_id(info->app_key, &keys->id)) {
        BT_ERR("%s, Failed to calculate AID, AppKeyIndex 0x%04x", __func__, info->app_idx);
        return -EIO;
    }

    key->net_idx = info->net_idx;
    key->app_idx = info->app_idx;
    memcpy(keys->val, info->app_key, 16);

    /* Binds AppKey with all non-config models, adds group address to all these models */
    comp = bt_mesh_comp_get();
    if (!comp) {
        BT_ERR("%s, Composition data is NULL", __func__);
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
