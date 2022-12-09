/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#ifndef _BLE_MESH_EXAMPLE_NVS_H_
#define _BLE_MESH_EXAMPLE_NVS_H_

#include <stdint.h>
#include "esp_err.h"
#include "nvs_flash.h"

esp_err_t ble_mesh_nvs_open(nvs_handle_t *handle);

esp_err_t ble_mesh_nvs_store(nvs_handle_t handle, const char *key, const void *data, size_t length);

esp_err_t ble_mesh_nvs_get_length(nvs_handle_t handle, const char *key, size_t *length);

esp_err_t ble_mesh_nvs_restore(nvs_handle_t handle, const char *key, void *data, size_t length, bool *exist);

esp_err_t ble_mesh_nvs_erase(nvs_handle_t handle, const char *key);

#endif /* _BLE_MESH_EXAMPLE_NVS_H_ */
