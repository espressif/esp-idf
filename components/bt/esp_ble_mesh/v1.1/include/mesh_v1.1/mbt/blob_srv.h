/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_BLUETOOTH_MESH_BLOB_SRV_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_MESH_BLOB_SRV_H_

#include <zephyr/bluetooth/mesh/access.h>
#include <zephyr/bluetooth/mesh/blob.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup bt_mesh_blob_srv Bluetooth Mesh BLOB Transfer Server model API
 * @ingroup bt_mesh
 * @{
 */

struct bt_mesh_blob_srv;

/**
 *
 *  @brief Max number of blocks in a single transfer.
 */
#if defined(CONFIG_BT_MESH_BLOB_SRV)
#define BT_MESH_BLOB_BLOCKS_MAX                                                \
	(DIV_ROUND_UP(CONFIG_BT_MESH_BLOB_SIZE_MAX,                        \
			  CONFIG_BT_MESH_BLOB_BLOCK_SIZE_MIN))
#else
#define BT_MESH_BLOB_BLOCKS_MAX 1
#endif

/**
 *
 *  @brief BLOB Transfer Server model composition data entry.
 *
 *  @param _srv Pointer to a @ref bt_mesh_blob_srv instance.
 */
#define BT_MESH_MODEL_BLOB_SRV(_srv)                                           \
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_BLOB_SRV, _bt_mesh_blob_srv_op,      \
			 NULL, _srv, &_bt_mesh_blob_srv_cb)

/** @brief BLOB Transfer Server model event handlers.
 *
 *  All callbacks are optional.
 */
struct bt_mesh_blob_srv_cb {
	/** @brief Transfer start callback.
	 *
	 *  Called when the transfer has started with the prepared BLOB ID.
	 *
	 *  @param srv  BLOB Transfer Server instance.
	 *  @param ctx  Message context for the incoming start message. The
	 *              entire transfer will be sent from the same source
	 *              address.
	 *  @param xfer Transfer parameters.
	 *
	 *  @return 0 on success, or (negative) error code to reject the
	 *          transfer.
	 */
	int (*start)(struct bt_mesh_blob_srv *srv, struct bt_mesh_msg_ctx *ctx,
		     struct bt_mesh_blob_xfer *xfer);

	/** @brief Transfer end callback.
	 *
	 *  Called when the transfer ends, either because it was cancelled, or
	 *  because it finished successfully. A new transfer may be prepared.
	 *
	 *  @note The transfer may end before it's started if the start
	 *        parameters are invalid.
	 *
	 *  @param srv     BLOB Transfer Server instance.
	 *  @param id      BLOB ID of the cancelled transfer.
	 *  @param success Whether the transfer was successful.
	 */
	void (*end)(struct bt_mesh_blob_srv *srv, uint64_t id, bool success);

	/** @brief Transfer suspended callback.
	 *
	 *  Called if the Server timed out while waiting for a transfer packet.
	 *  A suspended transfer may resume later from the start of the current
	 *  block. Any received chunks in the current block should be discarded,
	 *  they will be received again if the transfer resumes.
	 *
	 *  The transfer will call @c resumed again when resuming.
	 *
	 *  @note The BLOB Transfer Server does not run a timer in the suspended state,
	 *        and it's up to the application to determine whether the
	 *        transfer should be permanently cancelled. Without interaction,
	 *        the transfer will be suspended indefinitely, and the BLOB Transfer
	 *        Server will not accept any new transfers.
	 *
	 *  @param srv BLOB Transfer Server instance.
	 */
	void (*suspended)(struct bt_mesh_blob_srv *srv);

	/** @brief Transfer resume callback.
	 *
	 *  Called if the transfer is resumed after being suspended.
	 *
	 *  @param srv BLOB Transfer Server instance.
	 */
	void (*resume)(struct bt_mesh_blob_srv *srv);

	/** @brief Transfer recovery callback.
	 *
	 *  Called when the Bluetooth Mesh subsystem is started if the device is rebooted
	 *  in the middle of a transfer.
	 *
	 *  Transfers will not be resumed after a reboot if this callback is not
	 *  defined.
	 *
	 *  @param srv  BLOB Transfer Server instance.
	 *  @param xfer Transfer to resume.
	 *  @param io   BLOB stream return parameter. Must be set to a valid
	 *              BLOB stream by the callback.
	 *
	 *  @return 0 on success, or (negative) error code to abandon the
	 *          transfer.
	 */
	int (*recover)(struct bt_mesh_blob_srv *srv,
		       struct bt_mesh_blob_xfer *xfer,
		       const struct bt_mesh_blob_io **io);
};

/** @brief BLOB Transfer Server model instance. */
struct bt_mesh_blob_srv {
	/** Event handler callbacks. */
	const struct bt_mesh_blob_srv_cb *cb;

	/* Runtime state: */
	const struct bt_mesh_blob_io *io;
	struct k_work_delayable rx_timeout;
	struct bt_mesh_blob_block block;
	const struct bt_mesh_model *mod;
	enum bt_mesh_blob_xfer_phase phase;

	struct bt_mesh_blob_srv_state {
		struct bt_mesh_blob_xfer xfer;
		uint16_t cli;
		uint16_t app_idx;
		uint16_t timeout_base;
		uint16_t mtu_size;
		uint8_t ttl;

		/* Bitfield of pending blocks. */
		ATOMIC_DEFINE(blocks, BT_MESH_BLOB_BLOCKS_MAX);
	} state;

	/* Pull mode (Pull BLOB Transfer Mode) behavior. */
	struct {
		uint16_t chunk_idx;
		struct k_work_delayable report;
	} pull;
};

/** @brief Prepare BLOB Transfer Server for an incoming transfer.
 *
 *  Before a BLOB Transfer Server can receive a transfer, the transfer must be prepared
 *  through some application level mechanism. The BLOB Transfer Server will only accept
 *  incoming transfers with a matching BLOB ID.
 *
 *  @param srv          BLOB Transfer Server instance.
 *  @param id           BLOB ID to accept.
 *  @param io           BLOB stream to write the incoming BLOB to.
 *  @param ttl          Time to live value to use in responses to the BLOB Transfer Client.
 *  @param timeout_base Extra time for the Client to respond in addition to the
 *                      base 10 seconds, in 10-second increments.
 *
 *  @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_blob_srv_recv(struct bt_mesh_blob_srv *srv, uint64_t id,
			  const struct bt_mesh_blob_io *io, uint8_t ttl,
			  uint16_t timeout_base);

/** @brief Cancel the current BLOB transfer.
 *
 *  Tells the BLOB Transfer Client to drop this device from the list of Targets for the
 *  current transfer. Note that the client may continue sending the transfer to
 *  other Targets.
 *
 *  @param srv BLOB Transfer Server instance.
 *
 *  @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_blob_srv_cancel(struct bt_mesh_blob_srv *srv);

/** @brief Get the current state of the BLOB Transfer Server.
 *
 *  @param srv BLOB Transfer Server instance.
 *
 *  @return true if the BLOB Transfer Server is currently participating in a transfer,
 *          false otherwise.
 */
bool bt_mesh_blob_srv_is_busy(const struct bt_mesh_blob_srv *srv);

/** @brief Get the current progress of the active transfer in percent.
 *
 *  @param srv BLOB Transfer Server instance.
 *
 *  @return The current transfer progress, or 0 if no transfer is active.
 */
uint8_t bt_mesh_blob_srv_progress(const struct bt_mesh_blob_srv *srv);

/** @cond INTERNAL_HIDDEN */
extern const struct bt_mesh_model_op _bt_mesh_blob_srv_op[];
extern const struct bt_mesh_model_cb _bt_mesh_blob_srv_cb;
/** @endcond */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_MESH_BLOB_SRV_H_ */
