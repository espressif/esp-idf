// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _SETTINGS_UID_H_
#define _SETTINGS_UID_H_

#include "mesh_types.h"
#include "settings_nvs.h"

#ifdef __cplusplus
extern "C" {
#endif

int settings_uid_init(void);
int settings_uid_load(void);
int settings_uid_deinit(void);
int settings_uid_erase(void);

int bt_mesh_provisioner_open_settings_with_index(uint8_t index);
int bt_mesh_provisioner_open_settings_with_uid(const char *id, uint8_t *index);
int bt_mesh_provisioner_close_settings_with_index(uint8_t index, bool erase);
int bt_mesh_provisioner_close_settings_with_uid(const char *id, bool erase, uint8_t *index);
int bt_mesh_provisioner_delete_settings_with_index(uint8_t index);
int bt_mesh_provisioner_delete_settings_with_uid(const char *id, uint8_t *index);

const char *bt_mesh_provisioner_get_settings_uid(uint8_t index);
uint8_t bt_mesh_provisioner_get_settings_index(const char *id);
uint8_t bt_mesh_provisioner_get_free_settings_count(void);

int bt_mesh_provisioner_direct_erase_settings(void);

#ifdef __cplusplus
}
#endif

#endif /* _SETTINGS_UID_H_ */
