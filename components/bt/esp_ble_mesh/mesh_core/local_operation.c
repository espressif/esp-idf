/*  Bluetooth Mesh */

/*
 * Copyright (c) 2017 Intel Corporation
 * Additional Copyright (c) 2020 Espressif Systems (Shanghai) PTE LTD
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
#include "mesh_main.h"
#include "settings.h"

static struct bt_mesh_model *find_model(u16_t elem_addr, u16_t cid, u16_t mod_id)
{
    struct bt_mesh_elem *elem = NULL;

    if (!BLE_MESH_ADDR_IS_UNICAST(elem_addr)) {
        BT_ERR("%s, Not a unicast address 0x%04x", __func__, elem_addr);
        return NULL;
    }

    elem = bt_mesh_elem_find(elem_addr);
    if (elem == NULL) {
        BT_ERR("%s, No element found, addr 0x%04x", __func__, elem_addr);
        return NULL;
    }

    if (cid == BLE_MESH_CID_NVAL) {
        return bt_mesh_model_find(elem, mod_id);
    } else {
        return bt_mesh_model_find_vnd(elem, cid, mod_id);
    }
}

int bt_mesh_model_subscribe_group_addr(u16_t elem_addr, u16_t cid,
                                       u16_t mod_id, u16_t group_addr)
{
    struct bt_mesh_model *model = NULL;
    int i;

    model = find_model(elem_addr, cid, mod_id);
    if (model == NULL) {
        BT_ERR("Subscribe, model not found, cid 0x%04x, mod_id 0x%04x", cid, mod_id);
        return -ENODEV;
    }

    if (!BLE_MESH_ADDR_IS_GROUP(group_addr)) {
        BT_ERR("Subscribe, not a group address 0x%04x", group_addr);
        return -EINVAL;
    }

    if (bt_mesh_model_find_group(model, group_addr)) {
        BT_INFO("Group address 0x%04x already exists", group_addr);
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

            BT_INFO("Subscribe group address 0x%04x", group_addr);
            return 0;
        }
    }

    BT_ERR("Subscribe, model sub is full!");
    return -ENOMEM;
}

int bt_mesh_model_unsubscribe_group_addr(u16_t elem_addr, u16_t cid,
                                         u16_t mod_id, u16_t group_addr)
{
    struct bt_mesh_model *model = NULL;
    u16_t *match = NULL;

    model = find_model(elem_addr, cid, mod_id);
    if (model == NULL) {
        BT_ERR("Unsubscribe, model not found, cid 0x%04x, mod_id 0x%04x", cid, mod_id);
        return -ENODEV;
    }

    if (!BLE_MESH_ADDR_IS_GROUP(group_addr)) {
        BT_ERR("Unsubscribe, not a group address 0x%04x", group_addr);
        return -EINVAL;
    }

    match = bt_mesh_model_find_group(model, group_addr);
    if (match == NULL) {
        BT_WARN("Group address 0x%04x not exists", group_addr);
        return -EEXIST;
    }

    *match = BLE_MESH_ADDR_UNASSIGNED;

    if (IS_ENABLED(CONFIG_BLE_MESH_SETTINGS)) {
        bt_mesh_store_mod_sub(model);
    }

    if (IS_ENABLED(CONFIG_BLE_MESH_LOW_POWER)) {
        bt_mesh_lpn_group_del(&group_addr, 1);
    }

    BT_INFO("Unsubscribe group address 0x%04x", group_addr);
    return 0;
}
