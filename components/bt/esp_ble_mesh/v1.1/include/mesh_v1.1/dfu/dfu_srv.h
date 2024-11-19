/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @defgroup bt_mesh_dfu_srv Firmware Update Server model
 * @ingroup bt_mesh_dfu
 * @{
 * @brief API for the Bluetooth Mesh Firmware Update Server model
 */

#ifndef ZEPHYR_INCLUDE_BLUETOOTH_MESH_DFU_SRV_H__
#define ZEPHYR_INCLUDE_BLUETOOTH_MESH_DFU_SRV_H__

#include <zephyr/bluetooth/mesh/dfu.h>
#include <zephyr/bluetooth/mesh/blob_srv.h>
#include <zephyr/bluetooth/mesh/access.h>

#ifdef __cplusplus
extern "C" {
#endif

struct bt_mesh_dfu_srv;

/**
 *
 * @brief Initialization parameters for @ref bt_mesh_dfu_srv.
 *
 * @param _handlers   DFU handler function structure.
 * @param _imgs       List of @ref bt_mesh_dfu_img managed by this Server.
 * @param _img_count  Number of DFU images managed by this Server.
 */
#define BT_MESH_DFU_SRV_INIT(_handlers, _imgs, _img_count)                     \
	{                                                                      \
		.blob = { .cb = &_bt_mesh_dfu_srv_blob_cb }, .cb = _handlers,  \
		.imgs = _imgs, .img_count = _img_count,                        \
	}

/**
 *
 *  @brief Firmware Update Server model entry.
 *
 *  @param _srv Pointer to a @ref bt_mesh_dfu_srv instance.
 */
#define BT_MESH_MODEL_DFU_SRV(_srv)                                            \
	BT_MESH_MODEL_BLOB_SRV(&(_srv)->blob),                                  \
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_DFU_SRV, _bt_mesh_dfu_srv_op, NULL,  \
			 _srv, &_bt_mesh_dfu_srv_cb)

/** @brief Firmware Update Server event callbacks. */
struct bt_mesh_dfu_srv_cb {
	/** @brief Transfer check callback.
	 *
	 *  The transfer check can be used to validate the incoming transfer
	 *  before it starts. The contents of the metadata is implementation
	 *  specific, and should contain all the information the application
	 *  needs to determine whether this image should be accepted, and what
	 *  the effect of the transfer would be.
	 *
	 *  If applying the image will have an effect on the provisioning state
	 *  of the mesh stack, this can be communicated through the @c effect
	 *  return parameter.
	 *
	 *  The metadata check can be performed both as part of starting a new
	 *  transfer and as a separate procedure.
	 *
	 *  This handler is optional.
	 *
	 *  @param srv          Firmware Update Server instance.
	 *  @param img          DFU image the metadata check is performed on.
	 *  @param metadata     Image metadata.
	 *  @param effect       Return parameter for the image effect on the
	 *                      provisioning state of the mesh stack.
	 *
	 *  @return 0 on success, or (negative) error code otherwise.
	 */
	int (*check)(struct bt_mesh_dfu_srv *srv,
		     const struct bt_mesh_dfu_img *img,
		     struct net_buf_simple *metadata,
		     enum bt_mesh_dfu_effect *effect);

	/** @brief Transfer start callback.
	 *
	 *  Called when the Firmware Update Server is ready to start a new DFU transfer.
	 *  The application must provide an initialized BLOB stream to be used
	 *  during the DFU transfer.
	 *
	 *  The following error codes are treated specially, and should be used
	 *  to communicate these issues:
	 *  - @c -ENOMEM: The device cannot fit this image.
	 *  - @c -EBUSY: The application is temporarily unable to accept the
	 *    transfer.
	 *  - @c -EALREADY: The device has already received and verified this
	 *    image, and there's no need to transfer it again. The Firmware Update model
	 *    will skip the transfer phase, and mark the image as verified.
	 *
	 *  This handler is mandatory.
	 *
	 *  @param srv          Firmware Update Server instance.
	 *  @param img          DFU image being updated.
	 *  @param metadata     Image metadata.
	 *  @param io           BLOB stream return parameter. Must be set to a
	 *                      valid BLOB stream by the callback.
	 *
	 *  @return 0 on success, or (negative) error code otherwise. Return
	 *          codes @c -ENOMEM, @c -EBUSY @c -EALREADY will be passed to
	 *          the updater, other error codes are reported as internal
	 *          errors.
	 */
	int (*start)(struct bt_mesh_dfu_srv *srv,
		     const struct bt_mesh_dfu_img *img,
		     struct net_buf_simple *metadata,
		     const struct bt_mesh_blob_io **io);

	/** @brief Transfer end callback.
	 *
	 *  This handler is optional.
	 *
	 *  If the transfer is successful, the application should verify the
	 *  firmware image, and call either @ref bt_mesh_dfu_srv_verified or
	 *  @ref bt_mesh_dfu_srv_rejected depending on the outcome.
	 *
	 *  If the transfer fails, the Firmware Update Server will be available for new
	 *  transfers immediately after this function returns.
	 *
	 *  @param srv     Firmware Update Server instance.
	 *  @param img     DFU image that failed the update.
	 *  @param success Whether the DFU transfer was successful.
	 */
	void (*end)(struct bt_mesh_dfu_srv *srv,
		    const struct bt_mesh_dfu_img *img, bool success);

	/** @brief Transfer recovery callback.
	 *
	 *  If the device reboots in the middle of a transfer, the Firmware Update Server
	 *  calls this function when the Bluetooth Mesh subsystem is started.
	 *
	 *  This callback is optional, but transfers will not be recovered after
	 *  a reboot without it.
	 *
	 *  @param srv Firmware Update Server instance.
	 *  @param img DFU image being updated.
	 *  @param io  BLOB stream return parameter. Must be set to a valid BLOB
	 *             stream by the callback.
	 *
	 *  @return 0 on success, or (negative) error code to abandon the
	 *          transfer.
	 */
	int (*recover)(struct bt_mesh_dfu_srv *srv,
		       const struct bt_mesh_dfu_img *img,
		       const struct bt_mesh_blob_io **io);

	/** @brief Transfer apply callback.
	 *
	 *  Called after a transfer has been validated, and the updater sends an
	 *  apply message to the Target nodes.
	 *
	 *  This handler is optional.
	 *
	 *  @param srv Firmware Update Server instance.
	 *  @param img DFU image that should be applied.
	 *
	 *  @return 0 on success, or (negative) error code otherwise.
	 */
	int (*apply)(struct bt_mesh_dfu_srv *srv,
		     const struct bt_mesh_dfu_img *img);
};

/** @brief Firmware Update Server instance.
 *
 *  Should be initialized with @ref BT_MESH_DFU_SRV_INIT.
 */
struct bt_mesh_dfu_srv {
	/** Underlying BLOB Transfer Server. */
	struct bt_mesh_blob_srv blob;
	/** Callback structure. */
	const struct bt_mesh_dfu_srv_cb *cb;
	/** List of updatable images. */
	const struct bt_mesh_dfu_img *imgs;
	/** Number of updatable images. */
	size_t img_count;

	/* Runtime state */
	const struct bt_mesh_model *mod;
	struct {
		/* Effect of transfer, @see bt_mesh_dfu_effect. */
		uint8_t effect;
		/* Current phase, @see bt_mesh_dfu_phase. */
		uint8_t phase;
		uint8_t ttl;
		uint8_t idx;
		uint16_t timeout_base;
		uint16_t meta;
	} update;
};

/** @brief Accept the received DFU transfer.
 *
 *  Should be called at the end of a successful DFU transfer.
 *
 *  If the DFU transfer completes successfully, the application should verify
 *  the image validity (including any image authentication or integrity checks),
 *  and call this function if the image is ready to be applied.
 *
 *  @param srv Firmware Update Server instance.
 */
void bt_mesh_dfu_srv_verified(struct bt_mesh_dfu_srv *srv);

/** @brief Reject the received DFU transfer.
 *
 *  Should be called at the end of a successful DFU transfer.
 *
 *  If the DFU transfer completes successfully, the application should verify
 *  the image validity (including any image authentication or integrity checks),
 *  and call this function if one of the checks fail.
 *
 *  @param srv Firmware Update Server instance.
 */
void bt_mesh_dfu_srv_rejected(struct bt_mesh_dfu_srv *srv);

/** @brief Cancel the ongoing DFU transfer.
 *
 *  @param srv Firmware Update Server instance.
 */
void bt_mesh_dfu_srv_cancel(struct bt_mesh_dfu_srv *srv);

/** @brief Confirm that the received DFU transfer was applied.
 *
 *  Should be called as a result of the @ref bt_mesh_dfu_srv_cb.apply callback.
 *
 *  @param srv Firmware Update Server instance.
 */
void bt_mesh_dfu_srv_applied(struct bt_mesh_dfu_srv *srv);

/** @brief Check if the Firmware Update	Server is busy processing a transfer.
 *
 *  @param srv Firmware Update Server instance.
 *
 *  @return true if a DFU procedure is in progress, false otherwise.
 */
bool bt_mesh_dfu_srv_is_busy(const struct bt_mesh_dfu_srv *srv);

/** @brief Get the progress of the current DFU procedure, in percent.
 *
 *  @param srv Firmware Update Server instance.
 *
 *  @return The current transfer progress in percent.
 */
uint8_t bt_mesh_dfu_srv_progress(const struct bt_mesh_dfu_srv *srv);

/** @cond INTERNAL_HIDDEN */
extern const struct bt_mesh_model_op _bt_mesh_dfu_srv_op[];
extern const struct bt_mesh_model_cb _bt_mesh_dfu_srv_cb;
extern const struct bt_mesh_blob_srv_cb _bt_mesh_dfu_srv_blob_cb;
/** @endcond */

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_MESH_DFU_SRV_H__ */

/** @} */
