/*
 * SPDX-FileCopyrightText: 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/sys/dlist.h>
#include <zephyr/sys/byteorder.h>

#include <zephyr/bluetooth/services/ots.h>
#include "ots_internal.h"

#include <zephyr/logging/log.h>

struct bt_gatt_ots_pool_item {
    sys_dnode_t dnode;
    struct bt_gatt_ots_object val;
    bool is_allocated;
};

struct bt_gatt_ots_obj_manager {
    sys_dlist_t list;
    struct bt_gatt_ots_pool_item pool[CONFIG_BT_OTS_MAX_OBJ_CNT];
    bool is_assigned;
};

static uint64_t obj_id_to_index(uint64_t id)
{
    if (IS_ENABLED(CONFIG_BT_OTS_DIR_LIST_OBJ)) {
        if (id == OTS_OBJ_ID_DIR_LIST) {
            return id;
        } else {
            return id - BT_OTS_OBJ_ID_MIN + 1;
        }
    } else {
        return id - BT_OTS_OBJ_ID_MIN;
    }
}

static uint64_t obj_index_to_id(uint64_t index)
{
    if (IS_ENABLED(CONFIG_BT_OTS_DIR_LIST_OBJ)) {
        if (index == 0) {
            return OTS_OBJ_ID_DIR_LIST;
        } else {
            return BT_OTS_OBJ_ID_MIN + index - 1;
        }
    } else {
        return BT_OTS_OBJ_ID_MIN + index;
    }
}

int bt_gatt_ots_obj_manager_first_obj_get(
    struct bt_gatt_ots_obj_manager *obj_manager,
    struct bt_gatt_ots_object **obj)
{
    sys_dnode_t *obj_dnode;
    struct bt_gatt_ots_pool_item *first_item;

    if (sys_dlist_is_empty(&obj_manager->list)) {
        return -ENOENT;
    }

    obj_dnode = sys_dlist_peek_head_not_empty(&obj_manager->list);
    first_item = CONTAINER_OF(obj_dnode, struct bt_gatt_ots_pool_item,
                              dnode);
    *obj = &first_item->val;

    return 0;
}

int bt_gatt_ots_obj_manager_last_obj_get(
    struct bt_gatt_ots_obj_manager *obj_manager,
    struct bt_gatt_ots_object **obj)
{
    sys_dnode_t *obj_dnode;
    struct bt_gatt_ots_pool_item *last_item;

    if (sys_dlist_is_empty(&obj_manager->list)) {
        return -ENOENT;
    }

    obj_dnode = sys_dlist_peek_tail(&obj_manager->list);
    last_item = CONTAINER_OF(obj_dnode, struct bt_gatt_ots_pool_item,
                             dnode);
    *obj = &last_item->val;

    return 0;
}

int bt_gatt_ots_obj_manager_prev_obj_get(
    struct bt_gatt_ots_obj_manager *obj_manager,
    const struct bt_gatt_ots_object *cur_obj,
    struct bt_gatt_ots_object **prev_obj)
{
    sys_dnode_t *prev_obj_dnode;
    struct bt_gatt_ots_pool_item *cur_item, *prev_item;

    if (sys_dlist_is_empty(&obj_manager->list)) {
        return -ENOENT;
    }

    cur_item = CONTAINER_OF(cur_obj, struct bt_gatt_ots_pool_item, val);
    prev_obj_dnode = sys_dlist_peek_prev_no_check(&obj_manager->list,
                                                  &cur_item->dnode);
    if (!prev_obj_dnode) {
        return -ENFILE;
    }

    prev_item = CONTAINER_OF(prev_obj_dnode,
                             struct bt_gatt_ots_pool_item,
                             dnode);
    *prev_obj = &prev_item->val;

    return 0;
}

int bt_gatt_ots_obj_manager_next_obj_get(
    struct bt_gatt_ots_obj_manager *obj_manager,
    const struct bt_gatt_ots_object *cur_obj,
    struct bt_gatt_ots_object **next_obj)
{
    sys_dnode_t *next_obj_dnode;
    struct bt_gatt_ots_pool_item *cur_item, *next_item;

    if (sys_dlist_is_empty(&obj_manager->list)) {
        return -ENOENT;
    }

    cur_item = CONTAINER_OF(cur_obj, struct bt_gatt_ots_pool_item, val);
    next_obj_dnode = sys_dlist_peek_next_no_check(&obj_manager->list,
                                                  &cur_item->dnode);
    if (!next_obj_dnode) {
        return -ENFILE;
    }

    next_item = CONTAINER_OF(next_obj_dnode,
                             struct bt_gatt_ots_pool_item,
                             dnode);
    *next_obj = &next_item->val;

    return 0;
}

int bt_gatt_ots_obj_manager_obj_get(
    struct bt_gatt_ots_obj_manager *obj_manager, uint64_t id,
    struct bt_gatt_ots_object **object)
{
    uint64_t index;

    if (sys_dlist_is_empty(&obj_manager->list)) {
        return -ENOENT;
    }

    if (id == OTS_OBJ_ID_DIR_LIST && !IS_ENABLED(CONFIG_BT_OTS_DIR_LIST_OBJ)) {
        return -EINVAL;
    }

    index = obj_id_to_index(id);

    if (index >= ARRAY_SIZE(obj_manager->pool)) {
        return -EINVAL;
    }

    if (!obj_manager->pool[index].is_allocated) {
        return -EINVAL;
    }

    *object = &obj_manager->pool[index].val;

    return 0;
}

int bt_gatt_ots_obj_manager_obj_add(
    struct bt_gatt_ots_obj_manager *obj_manager,
    struct bt_gatt_ots_object **object)
{
    for (uint64_t i = 0; i < ARRAY_SIZE(obj_manager->pool); i++) {
        struct bt_gatt_ots_pool_item *cur_obj =
                &obj_manager->pool[i];

        if (!cur_obj->is_allocated) {
            cur_obj->is_allocated = true;
            /* TODO: do we need to reset cur_obj->val to 0 here? */
            cur_obj->val.id = obj_index_to_id(i);
            sys_dlist_append(&obj_manager->list, &cur_obj->dnode);

            *object = &cur_obj->val;
            return 0;
        }
    }

    return -ENOMEM;
}

int bt_gatt_ots_obj_manager_obj_delete(struct bt_gatt_ots_object *obj)
{
    struct bt_gatt_ots_pool_item *item;

    item = CONTAINER_OF(obj, struct bt_gatt_ots_pool_item, val);

    if (!item->is_allocated) {
        return -EINVAL;
    }

    if (IS_ENABLED(CONFIG_BT_OTS_DIR_LIST_OBJ) &&
            obj->id == OTS_OBJ_ID_DIR_LIST) {
        return -EINVAL;
    }

    item->is_allocated = false;
    sys_dlist_remove(&item->dnode);

    return 0;
}

bool bt_gatt_ots_obj_manager_obj_contains(struct bt_gatt_ots_obj_manager *obj_manager,
                                          struct bt_gatt_ots_object *obj)
{
    struct bt_gatt_ots_pool_item *item;

    item = CONTAINER_OF(obj, struct bt_gatt_ots_pool_item, val);

    return PART_OF_ARRAY(obj_manager->pool, item);
}

void *bt_gatt_ots_obj_manager_assign(void)
{
    static struct bt_gatt_ots_obj_manager
        obj_manager[CONFIG_BT_OTS_MAX_INST_CNT];
    struct bt_gatt_ots_obj_manager *cur_manager;

    for (cur_manager = obj_manager;
            cur_manager != obj_manager + CONFIG_BT_OTS_MAX_INST_CNT;
            cur_manager++) {
        if (!cur_manager->is_assigned) {
            break;
        }
    }

    if (cur_manager == obj_manager + CONFIG_BT_OTS_MAX_INST_CNT) {
        return NULL;
    }

    if (cur_manager->is_assigned) {
        return NULL;
    }

    cur_manager->is_assigned = true;
    sys_dlist_init(&cur_manager->list);

    return cur_manager;
}
