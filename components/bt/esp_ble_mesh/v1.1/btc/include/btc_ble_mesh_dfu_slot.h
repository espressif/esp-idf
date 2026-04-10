/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BTC_BLE_MESH_DFU_SLOT_H_
#define _BTC_BLE_MESH_DFU_SLOT_H_

int btc_ble_mesh_dfu_slot_count(void);

struct esp_ble_mesh_dfu_slot *btc_ble_mesh_dfu_slot_reserve(void);

int btc_ble_mesh_dfu_slot_info_set(struct esp_ble_mesh_dfu_slot *dfu_slot, size_t size,
                                   const uint8_t *metadata, size_t metadata_len);

int btc_ble_mesh_dfu_slot_fwid_set(struct esp_ble_mesh_dfu_slot *dfu_slot,
                                   const uint8_t *fwid, size_t fwid_len);

int btc_ble_mesh_dfu_slot_commit(struct esp_ble_mesh_dfu_slot *dfu_slot);

void btc_ble_mesh_dfu_slot_release(const struct esp_ble_mesh_dfu_slot *dfu_slot);

int btc_ble_mesh_dfu_slot_del(const struct esp_ble_mesh_dfu_slot *slot);

void btc_ble_mesh_dfu_slot_del_all(void);

const struct esp_ble_mesh_dfu_slot *btc_ble_mesh_dfu_slot_at(uint16_t img_idx);

int btc_ble_mesh_dfu_slot_get(const uint8_t *fwid, size_t fwid_len, struct esp_ble_mesh_dfu_slot **slot);

int btc_ble_mesh_dfu_slot_img_idx_get(const struct esp_ble_mesh_dfu_slot *slot);

size_t btc_ble_mesh_dfu_slot_foreach(esp_ble_mesh_dfu_slot_cb_t cb, void *user_data);

#endif /* _BTC_BLE_MESH_DFU_SLOT_H_ */
