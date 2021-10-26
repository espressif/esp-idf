/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _FAST_PROV_H_
#define _FAST_PROV_H_

#include "net.h"

#ifdef __cplusplus
extern "C" {
#endif

const uint8_t *bt_mesh_fast_prov_dev_key_get(uint16_t dst);

struct bt_mesh_subnet *bt_mesh_fast_prov_subnet_get(uint16_t net_idx);

struct bt_mesh_app_key *bt_mesh_fast_prov_app_key_find(uint16_t app_idx);

uint8_t bt_mesh_set_fast_prov_net_idx(uint16_t net_idx);

uint8_t bt_mesh_fast_prov_net_key_add(const uint8_t net_key[16]);

const uint8_t *bt_mesh_fast_prov_net_key_get(uint16_t net_idx);

const uint8_t *bt_mesh_get_fast_prov_app_key(uint16_t net_idx, uint16_t app_idx);

uint8_t bt_mesh_set_fast_prov_action(uint8_t action);

#ifdef __cplusplus
}
#endif

#endif /* _FAST_PROV_H_ */
