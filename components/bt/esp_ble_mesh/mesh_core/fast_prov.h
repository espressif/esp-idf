// Copyright 2017-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
