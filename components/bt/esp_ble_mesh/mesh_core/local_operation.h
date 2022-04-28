/*  Bluetooth Mesh */

/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileContributor: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _LOCAL_OPERATION_H_
#define _LOCAL_OPERATION_H_

#include "mesh_types.h"

#ifdef __cplusplus
extern "C" {
#endif

int bt_mesh_model_subscribe_group_addr(uint16_t elem_addr, uint16_t mod_id,
                                       uint16_t cid, uint16_t group_addr);

int bt_mesh_model_unsubscribe_group_addr(uint16_t elem_addr, uint16_t cid,
                                         uint16_t mod_id, uint16_t group_addr);

const uint8_t *bt_mesh_node_get_local_net_key(uint16_t net_idx);

const uint8_t *bt_mesh_node_get_local_app_key(uint16_t app_idx);

int bt_mesh_node_local_net_key_add(uint16_t net_idx, const uint8_t net_key[16]);

int bt_mesh_node_local_app_key_add(uint16_t net_idx, uint16_t app_idx,
                                   const uint8_t app_key[16]);

int bt_mesh_node_bind_app_key_to_model(uint16_t elem_addr, uint16_t mod_id,
                                       uint16_t cid, uint16_t app_idx);

#ifdef __cplusplus
}
#endif

#endif /* _LOCAL_OPERATION_H_ */
