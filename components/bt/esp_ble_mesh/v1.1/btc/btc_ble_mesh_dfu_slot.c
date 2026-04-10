/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <errno.h>

#include "dfu_slot.h"
#include "esp_ble_mesh_dfu_model_api.h"
#include "esp_ble_mesh_dfu_slot_api.h"
#include "btc_ble_mesh_dfu_slot.h"

#if CONFIG_BLE_MESH_DFU_SLOTS
int btc_ble_mesh_dfu_slot_count(void)
{
    return bt_mesh_dfu_slot_count();
}

struct esp_ble_mesh_dfu_slot *btc_ble_mesh_dfu_slot_reserve(void)
{
    return (struct esp_ble_mesh_dfu_slot *)bt_mesh_dfu_slot_reserve();
}

int btc_ble_mesh_dfu_slot_info_set(struct esp_ble_mesh_dfu_slot *dfu_slot, size_t size,
                                   const uint8_t *metadata, size_t metadata_len)
{
    return bt_mesh_dfu_slot_info_set((struct bt_mesh_dfu_slot *) dfu_slot, size,
                                     metadata, metadata_len);
}

int btc_ble_mesh_dfu_slot_fwid_set(struct esp_ble_mesh_dfu_slot *dfu_slot,
                                   const uint8_t *fwid, size_t fwid_len)
{
    return bt_mesh_dfu_slot_fwid_set((struct bt_mesh_dfu_slot *) dfu_slot, fwid,
                                     fwid_len);
}

int btc_ble_mesh_dfu_slot_commit(struct esp_ble_mesh_dfu_slot *dfu_slot)
{
    return bt_mesh_dfu_slot_commit((struct bt_mesh_dfu_slot *) dfu_slot);
}

void btc_ble_mesh_dfu_slot_release(const struct esp_ble_mesh_dfu_slot *dfu_slot)
{
    bt_mesh_dfu_slot_release((const struct bt_mesh_dfu_slot *)dfu_slot);
}

int btc_ble_mesh_dfu_slot_del(const struct esp_ble_mesh_dfu_slot *slot)
{
    return bt_mesh_dfu_slot_del((const struct bt_mesh_dfu_slot *)slot);
}

void btc_ble_mesh_dfu_slot_del_all(void)
{
    bt_mesh_dfu_slot_del_all();
}

const struct esp_ble_mesh_dfu_slot *btc_ble_mesh_dfu_slot_at(uint16_t img_idx)
{
    return (const struct esp_ble_mesh_dfu_slot *)bt_mesh_dfu_slot_at(img_idx);
}

int btc_ble_mesh_dfu_slot_get(const uint8_t *fwid, size_t fwid_len, struct esp_ble_mesh_dfu_slot **slot)
{
    return bt_mesh_dfu_slot_get(fwid, fwid_len, (struct bt_mesh_dfu_slot **)slot);
}

int btc_ble_mesh_dfu_slot_img_idx_get(const struct esp_ble_mesh_dfu_slot *slot)
{
    return bt_mesh_dfu_slot_img_idx_get((const struct bt_mesh_dfu_slot *)slot);
}

size_t btc_ble_mesh_dfu_slot_foreach(esp_ble_mesh_dfu_slot_cb_t cb, void *user_data)
{
    return bt_mesh_dfu_slot_foreach((bt_mesh_dfu_slot_cb_t)cb, user_data);
}
#endif /* CONFIG_BLE_MESH_DFU_SLOTS */
