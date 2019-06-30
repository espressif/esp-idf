// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _BLE_MESH_SETTINGS_NVS_H_
#define _BLE_MESH_SETTINGS_NVS_H_

#include <stdint.h>
#include "mesh_types.h"
#include "mesh_buf.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SETTINGS_ITEM_SIZE  sizeof(u16_t)

void bt_mesh_settings_foreach(void);

int bt_mesh_save_core_settings(const char *key, const u8_t *val, size_t len);

int bt_mesh_load_core_settings(const char *key, u8_t *buf, size_t buf_len, bool *exist);

struct net_buf_simple *bt_mesh_get_core_settings_item(const char *key);

int bt_mesh_add_core_settings_item(const char *key, const u16_t val);

int bt_mesh_remove_core_settings_item(const char *key, const u16_t val);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MESH_SETTINGS_NVS_H_ */
