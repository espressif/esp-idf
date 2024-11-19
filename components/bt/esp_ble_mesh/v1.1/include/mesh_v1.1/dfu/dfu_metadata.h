/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @defgroup bt_mesh_dfu_metadata Bluetooth Mesh Device Firmware Update (DFU) metadata
 * @ingroup bt_mesh_dfu
 * @{
 * @brief Common types and functions for the Bluetooth Mesh DFU metadata.
 */

#ifndef ZEPHYR_INCLUDE_BLUETOOTH_MESH_DFU_METADATA_H__
#define ZEPHYR_INCLUDE_BLUETOOTH_MESH_DFU_METADATA_H__

#include <stdint.h>

#include <sys/types.h>

#include <zephyr/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Firmware version. */
struct bt_mesh_dfu_metadata_fw_ver {
	/** Firmware major version. */
	uint8_t major;
	/** Firmware minor version. */
	uint8_t minor;
	/** Firmware revision. */
	uint16_t revision;
	/** Firmware build number. */
	uint32_t build_num;
};

/** Firmware core type. */
enum bt_mesh_dfu_metadata_fw_core_type {
	/** Application core. */
	BT_MESH_DFU_FW_CORE_TYPE_APP = BIT(0),
	/** Network core. */
	BT_MESH_DFU_FW_CORE_TYPE_NETWORK = BIT(1),
	/** Application-specific BLOB. */
	BT_MESH_DFU_FW_CORE_TYPE_APP_SPECIFIC_BLOB = BIT(2),
};

/** Firmware metadata. */
struct bt_mesh_dfu_metadata {
	/** New firmware version. */
	struct bt_mesh_dfu_metadata_fw_ver fw_ver;
	/** New firmware size. */
	uint32_t fw_size;
	/** New firmware core type. */
	enum bt_mesh_dfu_metadata_fw_core_type fw_core_type;
	/** Hash of incoming Composition Data. */
	uint32_t comp_hash;
	/** New number of node elements. */
	uint16_t elems;
	/** Application-specific data for new firmware. This field is optional. */
	uint8_t *user_data;
	/** Length of the application-specific field. */
	uint32_t user_data_len;
};

/** @brief Decode a firmware metadata from a network buffer.
 *
 *  @param buf Buffer containing a raw metadata to be decoded.
 *  @param metadata Pointer to a metadata structure to be filled.
 *
 *  @return 0 on success, or (negative) error code otherwise.
 */
int bt_mesh_dfu_metadata_decode(struct net_buf_simple *buf,
				struct bt_mesh_dfu_metadata *metadata);

/** @brief Encode a firmware metadata into a network buffer.
 *
 *  @param metadata Firmware metadata to be encoded.
 *  @param buf Buffer to store the encoded metadata.
 *
 *  @return 0 on success, or (negative) error code otherwise.
 */
int bt_mesh_dfu_metadata_encode(const struct bt_mesh_dfu_metadata *metadata,
				struct net_buf_simple *buf);

/** @brief Compute hash of the Composition Data state.
 *
 *  The format of the Composition Data is defined in MshPRTv1.1: 4.2.2.1.
 *
 *  @param buf Pointer to buffer holding Composition Data.
 *  @param key 128-bit key to be used in the hash computation.
 *  @param hash Pointer to a memory location to which the hash will be stored.
 *
 *  @return 0 on success, or (negative) error code otherwise.
 */
int bt_mesh_dfu_metadata_comp_hash_get(struct net_buf_simple *buf, uint8_t *key, uint32_t *hash);

/** @brief Compute hash of the Composition Data Page 0 of this device.
 *
 *  @param key 128-bit key to be used in the hash computation.
 *  @param hash Pointer to a memory location to which the hash will be stored.
 *
 *  @return 0 on success, or (negative) error code otherwise.
 */
int bt_mesh_dfu_metadata_comp_hash_local_get(uint8_t *key, uint32_t *hash);

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_MESH_DFU_METADATA_H__ */

/** @} */
