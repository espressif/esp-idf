/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_BLE_MESH_DFU_SLOT_API_H_
#define _ESP_BLE_MESH_DFU_SLOT_API_H_

#include "esp_ble_mesh_dfu_model_api.h"

#if CONFIG_BLE_MESH_DFU_SLOTS
#ifndef _BLE_MESH_BLOB_DEPRECATE_WARN
#define _BLE_MESH_BLOB_DEPRECATE_WARN
#warning Please note: All APIs published in this document are in Preview version and may undergo significant changes in the future.
#endif
#endif /* CONFIG_BLE_MESH_DFU_SLOTS */

typedef struct esp_ble_mesh_dfu_slot esp_ble_mesh_dfu_slot_t;

typedef enum esp_ble_mesh_dfu_iter
        (*esp_ble_mesh_dfu_slot_cb_t) (const esp_ble_mesh_dfu_slot_t *slot, void *user_data);

/** @brief Get the number of slots committed to the firmware list.
 *
 *  @return Number of committed slots.
 */
int esp_ble_mesh_dfu_slot_count(void);

/** @brief Reserve a new DFU image slot for a distributable image.
 *
 *  A DFU image slot represents a single distributable DFU image with all its
 *  metadata. The slot data must be set using esp_ble_mesh_dfu_slot_info_set and
 *  esp_ble_mesh_dfu_slot_fwid_set, and the slot committed using
 *  esp_ble_mesh_dfu_slot_commit for the slot to be considered part of the slot
 *  list.
 *
 *  @return
 *              - slot : A pointer to the reserved slot
 *              - @c NULL : Slot allocation failed
 */
esp_ble_mesh_dfu_slot_t *esp_ble_mesh_dfu_slot_reserve(void);

/** @brief Set the size and metadata for a reserved slot.
 *
 *  @param dfu_slot     Pointer to the reserved slot for which to set the
 *                      metadata.
 *  @param size         The size of the image.
 *  @param metadata     Metadata or NULL.
 *  @param metadata_len Length of the metadata, at most @c
 *                      CONFIG_BLE_MESH_DFU_METADATA_MAXLEN.
 *
 *  @return
 *                      - 0          : success
 *                      - @c ESP_ERR_INVALID_ARG : invalid slot parameter or invalid metadata parameter
 *                      - @c ESP_ERR_INVALID_SIZE : metadata length is zero or exceeds @c CONFIG_BLE_MESH_DFU_METADATA_MAXLEN
 */
int esp_ble_mesh_dfu_slot_info_set(esp_ble_mesh_dfu_slot_t *dfu_slot, size_t size,
                                   const uint8_t *metadata, size_t metadata_len);

/** @brief Set the new fwid for the incoming image for a reserved slot.
 *
 *  @param dfu_slot Pointer to the reserved slot for which to set the fwid.
 *  @param fwid     Fwid to set.
 *  @param fwid_len Length of the fwid, at most @c
 *                  CONFIG_BLE_MESH_DFU_FWID_MAXLEN.
 *
 *  @return
 *                  - 0     : success
 *                  - @c ESP_ERR_INVALID_ARG : invalid slot parameter or invalid fwid parameter
 *                  - @c ESP_ERR_INVALID_SIZE : fwid length is zero or exceeds @c CONFIG_BLE_MESH_DFU_FWID_MAXLEN
 */
int esp_ble_mesh_dfu_slot_fwid_set(esp_ble_mesh_dfu_slot_t *dfu_slot,
                                   const uint8_t *fwid, size_t fwid_len);

/** @brief Commit the reserved slot to the list of slots, and store it
 *         persistently.
 *
 *  If the commit fails for any reason, the slot will still be in the reserved
 *  state after this call.
 *
 *  @param dfu_slot Pointer to the reserved slot.
 *
 *  @return
 *                  - 0 : success
 *                  - @c ESP_ERR_INVALID_ARG : invalid slot parameter or slot has been committed
 *                  - @c ESP_ERR_INVALID_SIZE : slot size is zero
 */
int esp_ble_mesh_dfu_slot_commit(esp_ble_mesh_dfu_slot_t *dfu_slot);

/** @brief Release a reserved slot so that it can be reserved again.
 *
 *  @param dfu_slot Pointer to the reserved slot.
 */
void esp_ble_mesh_dfu_slot_release(const esp_ble_mesh_dfu_slot_t *dfu_slot);

/** @brief Delete a committed DFU image slot.
 *
 *  @param slot Slot to delete. Must be a valid pointer acquired from this
 *              module.
 *
 *  @return
 *              - 0 : success
 *              - @c ESP_ERR_INVALID_ARG : invalid slot parameter
 */
int esp_ble_mesh_dfu_slot_del(const esp_ble_mesh_dfu_slot_t *slot);

/** @brief Delete all DFU image slots.
 */
void esp_ble_mesh_dfu_slot_del_all(void);

/** @brief Get the DFU image slot at the given firmware image list index.
 *
 *  @param img_idx DFU image slot index.
 *
 *  @return
 *              - slot : A pointer to the reserved slot
 *              - @c NULL : No slot exists with the given index
 */
const esp_ble_mesh_dfu_slot_t *esp_ble_mesh_dfu_slot_at(uint16_t img_idx);

/** @brief Get the committed DFU image slot for the image with the given
 *         firmware ID.
 *
 *  @param fwid     Firmware ID.
 *  @param fwid_len Firmware ID length.
 *  @param slot     Slot pointer to fill.
 *
 *  @return
 *                - index : Slot index
 *                - @c ESP_ERR_INVALID_ARG : invalid fwid parameter or invalid slot parameter
 *                - @c ESP_ERR_INVALID_SIZE : fwid length is zero
 *                - @c ESP_ERR_NOT_FOUND : no slot exists with the given firmware id
 */
int esp_ble_mesh_dfu_slot_get(const uint8_t *fwid, size_t fwid_len, esp_ble_mesh_dfu_slot_t **slot);

/** @brief Get the index in the firmware image list for the given slot.
 *
 *  @param slot Slot to find.
 *
 *  @return
 *                - index : Slot index
 *                - @c ESP_ERR_INVALID_ARG : invalid slot parameter
 *                - @c ESP_ERR_NOT_FOUND : The slot does not exist
 */
int esp_ble_mesh_dfu_slot_img_idx_get(const esp_ble_mesh_dfu_slot_t *slot);

#endif /* _ESP_BLE_MESH_FW_SLOT_API_H_ */
