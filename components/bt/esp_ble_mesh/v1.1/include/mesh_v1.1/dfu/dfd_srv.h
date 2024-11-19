/*
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @defgroup bt_mesh_dfd_srv Firmware Distribution Server model
 * @ingroup bt_mesh_dfd
 * @{
 * @brief API for the Firmware Distribution Server model
 */

#ifndef ZEPHYR_INCLUDE_BLUETOOTH_MESH_DFD_SRV_H__
#define ZEPHYR_INCLUDE_BLUETOOTH_MESH_DFD_SRV_H__

#include <zephyr/bluetooth/mesh/access.h>
#include <zephyr/bluetooth/mesh/dfd.h>
#include <zephyr/bluetooth/mesh/blob_srv.h>
#include <zephyr/bluetooth/mesh/blob_cli.h>
#include <zephyr/bluetooth/mesh/dfu_cli.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_BT_MESH_DFD_SRV_TARGETS_MAX
#define CONFIG_BT_MESH_DFD_SRV_TARGETS_MAX 0
#endif

#ifndef CONFIG_BT_MESH_DFD_SRV_SLOT_MAX_SIZE
#define CONFIG_BT_MESH_DFD_SRV_SLOT_MAX_SIZE 0
#endif

#ifndef CONFIG_BT_MESH_DFD_SRV_SLOT_SPACE
#define CONFIG_BT_MESH_DFD_SRV_SLOT_SPACE 0
#endif

struct bt_mesh_dfd_srv;

#ifdef CONFIG_BT_MESH_DFD_SRV_OOB_UPLOAD
/**
 *
 *  @brief Initialization parameters for the @ref bt_mesh_dfd_srv with OOB
 *         upload support.
 *
 *  @param[in] _cb                Pointer to a @ref bt_mesh_dfd_srv_cb instance.
 *  @param[in] _oob_schemes       Array of OOB schemes supported by the server,
 *                                each scheme being a code point from the
 *                                Bluetooth SIG Assigned Numbers document.
 *  @param[in] _oob_schemes_count Number of schemes in @c _oob_schemes.
 */
#define BT_MESH_DFD_SRV_OOB_INIT(_cb, _oob_schemes, _oob_schemes_count)        \
	{                                                                      \
		.cb = _cb,                                                     \
		.dfu = BT_MESH_DFU_CLI_INIT(&_bt_mesh_dfd_srv_dfu_cb),         \
		.upload = {                                                    \
			.blob = { .cb = &_bt_mesh_dfd_srv_blob_cb },           \
		},                                                             \
		.oob_schemes = {                                               \
			.schemes = _oob_schemes,                               \
			.count = _oob_schemes_count,                           \
		},                                                             \
	}
#endif /* CONFIG_BT_MESH_DFD_SRV_OOB_UPLOAD */

/**
 *
 *  @brief Initialization parameters for the @ref bt_mesh_dfd_srv.
 *
 *  @param[in] _cb Pointer to a @ref bt_mesh_dfd_srv_cb instance.
 */
#define BT_MESH_DFD_SRV_INIT(_cb)                                              \
	{                                                                      \
		.cb = _cb,                                                     \
		.dfu = BT_MESH_DFU_CLI_INIT(&_bt_mesh_dfd_srv_dfu_cb),         \
		.upload = {                                                    \
			.blob = { .cb = &_bt_mesh_dfd_srv_blob_cb },           \
		},                                                             \
	}

/**
 *
 *  @brief Firmware Distribution Server model Composition Data entry.
 *
 *  @param _srv Pointer to a @ref bt_mesh_dfd_srv instance.
 */
#define BT_MESH_MODEL_DFD_SRV(_srv)                                            \
	BT_MESH_MODEL_DFU_CLI(&(_srv)->dfu),                                   \
	BT_MESH_MODEL_BLOB_SRV(&(_srv)->upload.blob),                          \
	BT_MESH_MODEL_CB(BT_MESH_MODEL_ID_DFD_SRV, _bt_mesh_dfd_srv_op, NULL,  \
			 _srv, &_bt_mesh_dfd_srv_cb)

/** Firmware Distribution Server callbacks: */
struct bt_mesh_dfd_srv_cb {

	/** @brief Slot receive callback.
	 *
	 *  Called at the start of an upload procedure. The callback must fill
	 *  @c io with a pointer to a writable BLOB stream for the Firmware Distribution
	 *  Server to write the firmware image to.
	 *
	 *  @param srv  Firmware Distribution Server model instance.
	 *  @param slot DFU image slot being received.
	 *  @param io   BLOB stream response pointer.
	 *
	 *  @return 0 on success, or (negative) error code otherwise.
	 */
	int (*recv)(struct bt_mesh_dfd_srv *srv,
		    const struct bt_mesh_dfu_slot *slot,
		    const struct bt_mesh_blob_io **io);

#ifdef CONFIG_BT_MESH_DFD_SRV_OOB_UPLOAD
	/** @brief Firmware upload OOB start callback.
	 *
	 *  Called at the start of an OOB firmware upload. The application must
	 *  start a firmware check using an OOB mechanism, and then call
	 *  @ref bt_mesh_dfd_srv_oob_check_complete. Depending on the return
	 *  value of this function, the application must then start storing the
	 *  firmware image using an OOB mechanism, and call
	 *  @ref bt_mesh_dfd_srv_oob_store_complete. This callback is mandatory
	 *  to support OOB uploads.
	 *
	 *  @param srv          Firmware Distribution Server model instance.
	 *  @param slot         Slot to be used for the upload.
	 *  @param uri          Pointer to buffer containing the URI used to
	 *                      check for new firmware.
	 *  @param uri_len      Length of the URI buffer.
	 *  @param fwid         Pointer to buffer containing the current
	 *                      firmware ID to be used when checking for
	 *                      availability of new firmware.
	 *  @param fwid_len     Length of the current firmware ID. Must be set
	 *                      to the length of the new firmware ID if it is
	 *                      available, or to 0 if new firmware is not
	 *                      available.
	 *
	 *  @return BT_MESH_DFD_SUCCESS on success, or error code otherwise.
	 */
	int (*start_oob_upload)(struct bt_mesh_dfd_srv *srv,
				const struct bt_mesh_dfu_slot *slot,
				const char *uri, uint8_t uri_len,
				const uint8_t *fwid, uint16_t fwid_len);

	/** @brief Cancel store OOB callback
	 *
	 *  Called when an OOB store is cancelled. The application must stop
	 *  any ongoing OOB image transfer. This callback is mandatory to
	 *  support OOB uploads.
	 *
	 *  @param srv  Firmware Distribution Server model instance.
	 *  @param slot DFU image slot to cancel
	 */
	void (*cancel_oob_upload)(struct bt_mesh_dfd_srv *srv,
				  const struct bt_mesh_dfu_slot *slot);

	/** @brief Get the progress of an ongoing OOB store
	 *
	 *  Called by the Firmware Distribution Server model when it needs to
	 *  get the current progress of an ongoing OOB store from the
	 *  application. This callback is mandatory to support OOB uploads.
	 *
	 *  @param srv  Firmware Distribution Server model instance.
	 *  @param slot DFU image slot to get progress for.
	 *
	 *  @return The current progress of the ongoing OOB store, in percent.
	 */
	uint8_t (*oob_progress_get)(struct bt_mesh_dfd_srv *srv,
				    const struct bt_mesh_dfu_slot *slot);
#endif /* CONFIG_BT_MESH_DFD_SRV_OOB_UPLOAD */

	/** @brief Slot delete callback.
	 *
	 *  Called when the Firmware Distribution Server is about to delete a DFU image slot.
	 *  All allocated data associated with the firmware slot should be
	 *  deleted.
	 *
	 *  @param srv  Firmware Update Server instance.
	 *  @param slot DFU image slot being deleted.
	 */
	void (*del)(struct bt_mesh_dfd_srv *srv,
		    const struct bt_mesh_dfu_slot *slot);

	/** @brief Slot send callback.
	 *
	 *  Called at the start of a distribution procedure. The callback must
	 *  fill @c io with a pointer to a readable BLOB stream for the Firmware
	 *  Distribution Server to read the firmware image from.
	 *
	 *  @param srv  Firmware Distribution Server model instance.
	 *  @param slot DFU image slot being sent.
	 *  @param io   BLOB stream response pointer.
	 *
	 *  @return 0 on success, or (negative) error code otherwise.
	 */
	int (*send)(struct bt_mesh_dfd_srv *srv,
		    const struct bt_mesh_dfu_slot *slot,
		    const struct bt_mesh_blob_io **io);

	/** @brief Phase change callback (Optional).
	 *
	 *  Called whenever the phase of the Firmware Distribution Server changes.
	 *
	 *  @param srv  Firmware Distribution Server model instance.
	 *  @param phase  New Firmware Distribution phase.
	 */
	void (*phase)(struct bt_mesh_dfd_srv *srv, enum bt_mesh_dfd_phase phase);
};

/** Firmware Distribution Server instance. */
struct bt_mesh_dfd_srv {
	const struct bt_mesh_dfd_srv_cb *cb;
	const struct bt_mesh_model *mod;
	struct bt_mesh_dfu_cli dfu;
	struct bt_mesh_dfu_target targets[CONFIG_BT_MESH_DFD_SRV_TARGETS_MAX];
	struct bt_mesh_blob_target_pull pull_ctxs[CONFIG_BT_MESH_DFD_SRV_TARGETS_MAX];
	const struct bt_mesh_blob_io *io;
	uint16_t target_cnt;
	uint16_t slot_idx;
	bool apply;
	enum bt_mesh_dfd_phase phase;
	struct bt_mesh_blob_cli_inputs inputs;

	struct {
		enum bt_mesh_dfd_upload_phase phase;
		struct bt_mesh_dfu_slot *slot;
		const struct flash_area *area;
		struct bt_mesh_blob_srv blob;
#ifdef CONFIG_BT_MESH_DFD_SRV_OOB_UPLOAD
		bool is_oob;
		bool is_pending_oob_check;
		struct {
			uint8_t uri_len;
			uint8_t uri[CONFIG_BT_MESH_DFU_URI_MAXLEN];
			uint16_t current_fwid_len;
			uint8_t current_fwid[CONFIG_BT_MESH_DFU_FWID_MAXLEN];
			struct bt_mesh_msg_ctx ctx;
		} oob;
#endif /* CONFIG_BT_MESH_DFD_SRV_OOB_UPLOAD */
	} upload;

#ifdef CONFIG_BT_MESH_DFD_SRV_OOB_UPLOAD
	struct {
		const uint8_t *schemes;
		const uint8_t count;
	} oob_schemes;
#endif /* CONFIG_BT_MESH_DFD_SRV_OOB_UPLOAD */
};

#ifdef CONFIG_BT_MESH_DFD_SRV_OOB_UPLOAD
/** @brief Call when an OOB check has completed or failed
 *
 *  This should be called by the application after an OOB check started by the @c start_oob_upload
 *  callback has completed or failed. The @p status param should be set to one of the following
 *  values:
 *
 *  * @c BT_MESH_DFD_SUCCESS if the check was successful and a new firmware ID was found.
 *  * @c BT_MESH_DFD_ERR_URI_MALFORMED if the URI is not formatted correctly.
 *  * @c BT_MESH_DFD_ERR_URI_NOT_SUPPORTED if the URI scheme is not supported by the node.
 *  * @c BT_MESH_DFD_ERR_URI_UNREACHABLE if the URI can't be reached.
 *  * @c BT_MESH_DFD_ERR_NEW_FW_NOT_AVAILABLE if the check completes successfully but no new
 *    firmware is available.
 *
 *  If this function returns 0, the application should then download the firmware to the
 *  slot. If an error code is returned, the application should abort the OOB upload.
 *
 *  @param srv      Firmware Distribution Server model instance.
 *  @param slot     The slot used in the OOB upload.
 *  @param status   The result of the firmware check.
 *  @param fwid     If the check was successful and new firmware found, this should point to a
 *                  buffer containing the new firmware ID to store.
 *  @param fwid_len The length of the firmware ID pointed to by @p fwid.
 *
 *  @return 0 on success, (negative) error code otherwise.
 */
int bt_mesh_dfd_srv_oob_check_complete(struct bt_mesh_dfd_srv *srv,
				       const struct bt_mesh_dfu_slot *slot, int status,
				       uint8_t *fwid, size_t fwid_len);

/** @brief Call when an OOB store has completed or failed
 *
 *  This should be called by the application after an OOB store started after a successful call to
 *  @c bt_mesh_dfd_srv_oob_check_complete has completed successfully or failed.
 *
 *  @param srv          Firmware Distribution Server model instance.
 *  @param slot         The slot used when storing the firmware image.
 *  @param success      @c true if the OOB store completed successfully, @c false otherwise.
 *  @param size         The size of the stored firmware image, in bytes.
 *  @param metadata     Pointer to the metadata received OOB, or @c NULL if no metadata was
 *                      received.
 *  @param metadata_len Size of the metadata pointed to by @p metadata.
 *
 *  @return 0 on success, (negative) error code otherwise.
 */
int bt_mesh_dfd_srv_oob_store_complete(struct bt_mesh_dfd_srv *srv,
				       const struct bt_mesh_dfu_slot *slot, bool success,
				       size_t size, const uint8_t *metadata, size_t metadata_len);
#endif /* CONFIG_BT_MESH_DFD_SRV_OOB_UPLOAD */

/** @cond INTERNAL_HIDDEN */
extern const struct bt_mesh_model_op _bt_mesh_dfd_srv_op[];
extern const struct bt_mesh_model_cb _bt_mesh_dfd_srv_cb;
extern const struct bt_mesh_dfu_cli_cb _bt_mesh_dfd_srv_dfu_cb;
extern const struct bt_mesh_blob_srv_cb _bt_mesh_dfd_srv_blob_cb;
/** @endcond */

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_MESH_DFD_SRV_H__ */

/** @} */
