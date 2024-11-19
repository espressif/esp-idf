/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_BLUETOOTH_MESH_DFU_H__
#define ZEPHYR_INCLUDE_BLUETOOTH_MESH_DFU_H__

#include <sys/types.h>

#include <zephyr/kernel.h>
#include <zephyr/bluetooth/mesh/blob.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup bt_mesh_dfu Bluetooth Mesh Device Firmware Update
 * @ingroup bt_mesh
 * @{
 */

#ifndef CONFIG_BT_MESH_DFU_FWID_MAXLEN
#define CONFIG_BT_MESH_DFU_FWID_MAXLEN 0
#endif

#ifndef CONFIG_BT_MESH_DFU_METADATA_MAXLEN
#define CONFIG_BT_MESH_DFU_METADATA_MAXLEN 0
#endif

#ifndef CONFIG_BT_MESH_DFU_URI_MAXLEN
#define CONFIG_BT_MESH_DFU_URI_MAXLEN 0
#endif

#ifndef CONFIG_BT_MESH_DFU_SLOT_CNT
#define CONFIG_BT_MESH_DFU_SLOT_CNT 0
#endif

/** DFU transfer phase. */
enum bt_mesh_dfu_phase {
	/** Ready to start a Receive Firmware procedure. */
	BT_MESH_DFU_PHASE_IDLE,

	/** The Transfer BLOB procedure failed. */
	BT_MESH_DFU_PHASE_TRANSFER_ERR,

	/** The Receive Firmware procedure is being executed.  */
	BT_MESH_DFU_PHASE_TRANSFER_ACTIVE,

	/** The Verify Firmware procedure is being executed. */
	BT_MESH_DFU_PHASE_VERIFY,

	/** The Verify Firmware procedure completed successfully. */
	BT_MESH_DFU_PHASE_VERIFY_OK,

	/** The Verify Firmware procedure failed. */
	BT_MESH_DFU_PHASE_VERIFY_FAIL,

	/** The Apply New Firmware procedure is being executed. */
	BT_MESH_DFU_PHASE_APPLYING,

	/** Firmware transfer has been canceled. */
	BT_MESH_DFU_PHASE_TRANSFER_CANCELED,

	/** Firmware applying succeeded. */
	BT_MESH_DFU_PHASE_APPLY_SUCCESS,

	/** Firmware applying failed. */
	BT_MESH_DFU_PHASE_APPLY_FAIL,

	/** Phase of a node was not yet retrieved. */
	BT_MESH_DFU_PHASE_UNKNOWN,
};


/** DFU status. */
enum bt_mesh_dfu_status {
	/** The message was processed successfully. */
	BT_MESH_DFU_SUCCESS,

	/** Insufficient resources on the node */
	BT_MESH_DFU_ERR_RESOURCES,

	/** The operation cannot be performed while the Server is in the current
	 *  phase.
	 */
	BT_MESH_DFU_ERR_WRONG_PHASE,

	/** An internal error occurred on the node. */
	BT_MESH_DFU_ERR_INTERNAL,

	/** The message contains a firmware index value that is not expected. */
	BT_MESH_DFU_ERR_FW_IDX,

	/** The metadata check failed. */
	BT_MESH_DFU_ERR_METADATA,

	/** The Server cannot start a firmware update. */
	BT_MESH_DFU_ERR_TEMPORARILY_UNAVAILABLE,

	/** Another BLOB transfer is in progress. */
	BT_MESH_DFU_ERR_BLOB_XFER_BUSY,
};

/** Expected effect of a DFU transfer. */
enum bt_mesh_dfu_effect {
	/** No changes to node Composition Data. */
	BT_MESH_DFU_EFFECT_NONE,

	/** Node Composition Data changed and the node does not support remote
	 *  provisioning.
	 */
	BT_MESH_DFU_EFFECT_COMP_CHANGE_NO_RPR,

	/** Node Composition Data changed, and remote provisioning is supported.
	 *  The node supports remote provisioning and Composition Data Page
	 *  0x80. Page 0x80 contains different Composition Data than Page 0x0.
	 */
	BT_MESH_DFU_EFFECT_COMP_CHANGE,

	/** Node will be unprovisioned after the update. */
	BT_MESH_DFU_EFFECT_UNPROV,
};

/** Action for DFU iteration callbacks. */
enum bt_mesh_dfu_iter {
	/** Stop iterating. */
	BT_MESH_DFU_ITER_STOP,

	/** Continue iterating. */
	BT_MESH_DFU_ITER_CONTINUE,
};

/** DFU image instance.
 *
 *  Each DFU image represents a single updatable firmware image.
 */
struct bt_mesh_dfu_img {
	/** Firmware ID. */
	const void *fwid;

	/** Length of the firmware ID. */
	size_t fwid_len;

	/** Update URI, or NULL. */
	const char *uri;
};

/** DFU image slot for DFU distribution. */
struct bt_mesh_dfu_slot {
	/** Size of the firmware in bytes. */
	size_t size;
	/** Length of the firmware ID. */
	size_t fwid_len;
	/** Length of the metadata. */
	size_t metadata_len;
	/** Firmware ID. */
	uint8_t fwid[CONFIG_BT_MESH_DFU_FWID_MAXLEN];
	/** Metadata. */
	uint8_t metadata[CONFIG_BT_MESH_DFU_METADATA_MAXLEN];
};

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_MESH_DFU_H__ */
