/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SETTINGS_UID_H_
#define _SETTINGS_UID_H_

#include "mesh/types.h"
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
