/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include "esp_ble_mesh_dfu_slot_api.h"
#include "btc_ble_mesh_dfu_slot.h"

int esp_ble_mesh_dfu_slot_count(void)
{
    return btc_ble_mesh_dfu_slot_count();
}

esp_ble_mesh_dfu_slot_t *esp_ble_mesh_dfu_slot_reserve(void)
{
    return btc_ble_mesh_dfu_slot_reserve();
}

int esp_ble_mesh_dfu_slot_info_set(esp_ble_mesh_dfu_slot_t *dfu_slot, size_t size,
                                   const uint8_t *metadata, size_t metadata_len)
{
    if (!dfu_slot || !metadata ||
        metadata_len == 0 ||
        metadata_len > CONFIG_BLE_MESH_DFU_METADATA_MAXLEN) {
        return ESP_ERR_INVALID_ARG;
    }

    return btc_ble_mesh_dfu_slot_info_set(dfu_slot, size,
                                          metadata, metadata_len);
}

int esp_ble_mesh_dfu_slot_fwid_set(esp_ble_mesh_dfu_slot_t *dfu_slot,
                                   const uint8_t *fwid, size_t fwid_len)
{
    if (!dfu_slot || !fwid || fwid_len == 0 ||
        fwid_len > CONFIG_BLE_MESH_DFU_FWID_MAXLEN) {
        return ESP_ERR_INVALID_ARG;
    }

    return btc_ble_mesh_dfu_slot_fwid_set(dfu_slot, fwid,
                                          fwid_len);
}

int esp_ble_mesh_dfu_slot_commit(esp_ble_mesh_dfu_slot_t *dfu_slot)
{
    if (!dfu_slot) {
        return ESP_ERR_INVALID_ARG;
    }

    return btc_ble_mesh_dfu_slot_commit(dfu_slot);
}

void esp_ble_mesh_dfu_slot_release(const esp_ble_mesh_dfu_slot_t *dfu_slot)
{
    if (!dfu_slot) {
        return;
    }

    btc_ble_mesh_dfu_slot_release(dfu_slot);
}

int esp_ble_mesh_dfu_slot_del(const esp_ble_mesh_dfu_slot_t *slot)
{
    if (!slot) {
        return ESP_ERR_INVALID_ARG;
    }

    return btc_ble_mesh_dfu_slot_del(slot);
}

void esp_ble_mesh_dfu_slot_del_all(void)
{
    btc_ble_mesh_dfu_slot_del_all();
}

const esp_ble_mesh_dfu_slot_t *esp_ble_mesh_dfu_slot_at(uint16_t img_idx)
{
    return btc_ble_mesh_dfu_slot_at(img_idx);
}

int esp_ble_mesh_dfu_slot_get(const uint8_t *fwid, size_t fwid_len, esp_ble_mesh_dfu_slot_t **slot)
{
    if (!fwid || fwid_len == 0 ||
        fwid_len > CONFIG_BLE_MESH_DFU_FWID_MAXLEN ||
        !slot || !*slot) {
        return ESP_ERR_INVALID_ARG;
    }

    return btc_ble_mesh_dfu_slot_get(fwid, fwid_len, slot);
}

int esp_ble_mesh_dfu_slot_img_idx_get(const esp_ble_mesh_dfu_slot_t *slot)
{
    if (!slot) {
        return ESP_ERR_INVALID_ARG;
    }

    return btc_ble_mesh_dfu_slot_img_idx_get(slot);
}
