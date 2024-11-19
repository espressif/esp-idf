/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/bluetooth/mesh.h>

/** @brief Slot iteration callback.
 *
 *  @param slot      A valid DFU image slot.
 *  @param user_data User data passed to @ref bt_mesh_dfu_slot_foreach.
 *
 *  @return Iteration action determining next step.
 */
typedef enum bt_mesh_dfu_iter (*bt_mesh_dfu_slot_cb_t)(
	const struct bt_mesh_dfu_slot *slot, void *user_data);

/** @brief Get the number of slots committed to the firmware list.
 *
 *  @return Number of committed slots.
 */
int bt_mesh_dfu_slot_count(void);

/** @brief Reserve a new DFU image slot for a distributable image.
 *
 *  A DFU image slot represents a single distributable DFU image with all its
 *  metadata. The slot data must be set using @ref bt_mesh_dfu_slot_info_set and
 *  @ref bt_mesh_dfu_slot_fwid_set, and the slot committed using
 *  @ref bt_mesh_dfu_slot_commit for the slot to be considered part of the slot
 *  list.
 *
 *  @return A pointer to the reserved slot, or NULL if allocation failed.
 */
struct bt_mesh_dfu_slot *bt_mesh_dfu_slot_reserve(void);

/** @brief Set the size and metadata for a reserved slot.
 *
 *  @param dfu_slot     Pointer to the reserved slot for which to set the
 *                      metadata.
 *  @param size         The size of the image.
 *  @param metadata     Metadata or NULL.
 *  @param metadata_len Length of the metadata, at most @c
 *                      CONFIG_BT_MESH_DFU_METADATA_MAXLEN.
 *
 *  @return 0 on success, (negative) error code otherwise.
 */
int bt_mesh_dfu_slot_info_set(struct bt_mesh_dfu_slot *dfu_slot, size_t size,
			      const uint8_t *metadata, size_t metadata_len);

/** @brief Set the new fwid for the incoming image for a reserved slot.
 *
 *  @param dfu_slot Pointer to the reserved slot for which to set the fwid.
 *  @param fwid     Fwid to set.
 *  @param fwid_len Length of the fwid, at most @c
 *                  CONFIG_BT_MESH_DFU_FWID_MAXLEN.
 *
 *  @return 0 on success, (negative) error code otherwise.
 */
int bt_mesh_dfu_slot_fwid_set(struct bt_mesh_dfu_slot *dfu_slot,
			      const uint8_t *fwid, size_t fwid_len);

/** @brief Commit the reserved slot to the list of slots, and store it
 *         persistently.
 *
 *  If the commit fails for any reason, the slot will still be in the reserved
 *  state after this call.
 *
 *  @param dfu_slot Pointer to the reserved slot.
 *
 *  @return 0 on success, (negative) error code otherwise.
 */
int bt_mesh_dfu_slot_commit(struct bt_mesh_dfu_slot *dfu_slot);

/** @brief Release a reserved slot so that it can be reserved again.
 *
 *  @param dfu_slot Pointer to the reserved slot.
 */
void bt_mesh_dfu_slot_release(const struct bt_mesh_dfu_slot *dfu_slot);

/** @brief Delete a committed DFU image slot.
 *
 *  @param slot Slot to delete. Must be a valid pointer acquired from this
 *              module.
 *
 *  @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_dfu_slot_del(const struct bt_mesh_dfu_slot *slot);

/** @brief Delete all DFU image slots.
 *
 *  @return 0 on success, or (negative) error code on failure.
 */
void bt_mesh_dfu_slot_del_all(void);

/** @brief Get the DFU image slot at the given firmware image list index.
 *
 *  @param idx DFU image slot index.
 *
 *  @return The DFU image slot at the given index, or NULL if no slot exists with the
 *          given index.
 */
const struct bt_mesh_dfu_slot *bt_mesh_dfu_slot_at(uint16_t img_idx);

/** @brief Get the committed DFU image slot for the image with the given
 *         firmware ID.
 *
 *  @param fwid     Firmware ID.
 *  @param fwid_len Firmware ID length.
 *  @param slot     Slot pointer to fill.
 *
 *  @return Slot index on success, or negative error code on failure.
 */
int bt_mesh_dfu_slot_get(const uint8_t *fwid, size_t fwid_len, struct bt_mesh_dfu_slot **slot);

/** @brief Get the index in the firmware image list for the given slot.
 *
 *  @param slot Slot to find.
 *
 *  @return Slot index on success, or negative error code on failure.
 */
int bt_mesh_dfu_slot_img_idx_get(const struct bt_mesh_dfu_slot *slot);

/** @brief Iterate through all DFU image slots.
 *
 *  Calls the callback for every DFU image slot or until the callback returns
 *  something other than @ref BT_MESH_DFU_ITER_CONTINUE.
 *
 *  @param cb        Callback to call for each slot, or NULL to just count the
 *                   number of slots.
 *  @param user_data User data to pass to the callback.
 *
 *  @return The number of slots iterated over.
 */
size_t bt_mesh_dfu_slot_foreach(bt_mesh_dfu_slot_cb_t cb, void *user_data);
