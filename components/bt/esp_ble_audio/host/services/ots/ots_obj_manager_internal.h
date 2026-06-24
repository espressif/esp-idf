/*
 * SPDX-FileCopyrightText: 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef BT_GATT_OTS_OBJ_MANAGER_H_
#define BT_GATT_OTS_OBJ_MANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr/types.h>

int bt_gatt_ots_obj_manager_first_obj_get(void *obj_manager,
                                          struct bt_gatt_ots_object **obj);

int bt_gatt_ots_obj_manager_last_obj_get(void *obj_manager,
                                         struct bt_gatt_ots_object **obj);

int bt_gatt_ots_obj_manager_prev_obj_get(
    void *obj_manager,
    const struct bt_gatt_ots_object *cur_obj,
    struct bt_gatt_ots_object **prev_obj);

int bt_gatt_ots_obj_manager_next_obj_get(
    void *obj_manager,
    const struct bt_gatt_ots_object *cur_obj,
    struct bt_gatt_ots_object **next_obj);

int bt_gatt_ots_obj_manager_obj_get(void *obj_manager,
                                    uint64_t id,
                                    struct bt_gatt_ots_object **obj);

int bt_gatt_ots_obj_manager_obj_add(void *obj_manager,
                                    struct bt_gatt_ots_object **obj);

int bt_gatt_ots_obj_manager_obj_delete(struct bt_gatt_ots_object *obj);

bool bt_gatt_ots_obj_manager_obj_contains(void *obj_manager, struct bt_gatt_ots_object *obj);

void *bt_gatt_ots_obj_manager_assign(void);

#ifdef __cplusplus
}
#endif

#endif /* BT_GATT_OTS_OBJ_MANAGER_H_ */
