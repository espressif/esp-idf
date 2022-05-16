/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BLE_MESH_SETTINGS_NVS_H_
#define _BLE_MESH_SETTINGS_NVS_H_

#include "nvs_flash.h"
#include "mesh_buf.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef nvs_handle_t    bt_mesh_nvs_handle_t;

#define SETTINGS_ITEM_SIZE              sizeof(uint16_t)

#define BLE_MESH_GET_ELEM_IDX(x)        ((uint8_t)((x) >> 8))
#define BLE_MESH_GET_MODEL_IDX(x)       ((uint8_t)(x))
#define BLE_MESH_GET_MODEL_KEY(a, b)    ((uint16_t)(((uint16_t)((a) << 8)) | (b)))

int bt_mesh_settings_nvs_open(const char* name, bt_mesh_nvs_handle_t *handle);
void bt_mesh_settings_nvs_close(bt_mesh_nvs_handle_t handle);

void bt_mesh_settings_init_foreach(void);
void bt_mesh_settings_deinit_foreach(bool erase);

int bt_mesh_settings_direct_open(bt_mesh_nvs_handle_t *handle);
void bt_mesh_settings_direct_close(void);

int bt_mesh_save_settings(bt_mesh_nvs_handle_t handle, const char *key,
                          const uint8_t *val, size_t len);
int bt_mesh_save_core_settings(const char *key, const uint8_t *val, size_t len);
int bt_mesh_save_uid_settings(const char *key, const uint8_t *val, size_t len);

int bt_mesh_erase_settings(bt_mesh_nvs_handle_t handle, const char *key);
int bt_mesh_erase_core_settings(const char *key);
int bt_mesh_erase_uid_settings(const char *name);

int bt_mesh_load_settings(bt_mesh_nvs_handle_t handle, const char *key,
                          uint8_t *buf, size_t buf_len, bool *exist);
int bt_mesh_load_core_settings(const char *key, uint8_t *buf, size_t buf_len, bool *exist);
int bt_mesh_load_uid_settings(const char *key, uint8_t *buf, size_t buf_len, bool *exist);

struct net_buf_simple *bt_mesh_get_settings_item(bt_mesh_nvs_handle_t handle, const char *key);
struct net_buf_simple *bt_mesh_get_core_settings_item(const char *key);
struct net_buf_simple *bt_mesh_get_uid_settings_item(const char *key);

int bt_mesh_add_settings_item(bt_mesh_nvs_handle_t handle, const char *key, const uint16_t val);
int bt_mesh_add_core_settings_item(const char *key, const uint16_t val);
int bt_mesh_add_uid_settings_item(const char *key, const uint16_t val);

int bt_mesh_remove_settings_item(bt_mesh_nvs_handle_t handle, const char *key, const uint16_t val);
int bt_mesh_remove_core_settings_item(const char *key, const uint16_t val);
int bt_mesh_remove_uid_settings_item(const char *key, const uint16_t val);

int bt_mesh_settings_erase_key(bt_mesh_nvs_handle_t handle, const char *key);
int bt_mesh_settings_erase_all(bt_mesh_nvs_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MESH_SETTINGS_NVS_H_ */
