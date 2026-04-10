/*
 * SPDX-FileCopyrightText: 2020 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @defgroup bt_mesh_dfu_cli Firmware Uppdate Client model
 * @ingroup bt_mesh_dfu
 * @{
 * @brief API for the Bluetooth Mesh Firmware Update Client model
 */

#ifndef _BLE_MESH_v11_DFU_CLI_H__
#define _BLE_MESH_v11_DFU_CLI_H__

#include "freertos/semphr.h"

#include "mesh/access.h"
#include "mesh_v1.1/mbt/blob_cli.h"
#include "mesh_v1.1/dfu/dfu.h"

#ifdef __cplusplus
extern "C" {
#endif

struct bt_mesh_dfu_cli;

/**
 *
 *  @brief Initialization parameters for the @ref bt_mesh_dfu_cli.
 *
 *  @sa bt_mesh_dfu_cli_cb.
 *
 *  @param _handlers Handler callback structure.
 */
#define BLE_MESH_DFU_CLI_INIT(_handlers)                               \
    {                                                                  \
        .cb = _handlers,                                               \
        .blob = { .cb = &_bt_mesh_dfu_cli_blob_handlers },             \
    }

/**
 *
 *  @brief Firmware Update Client model Composition Data entry.
 *
 *  @param _cli Pointer to a @ref bt_mesh_dfu_cli instance.
 */
#define BT_MESH_MODEL_DFU_CLI(_cli)                                        \
    BT_MESH_MODEL_BLOB_CLI(&(_cli)->blob),                                 \
    BT_MESH_MODEL_CB(BLE_MESH_MODEL_ID_DFU_CLI, _bt_mesh_dfu_cli_op, NULL, \
             _cli, &_bt_mesh_dfu_cli_cb)

/** DFU Target node. */
struct bt_mesh_dfu_target {
    /** BLOB Target node */
    struct bt_mesh_blob_target blob;
    /** Image index on the Target node */
    uint8_t img_idx;
    /** Expected DFU effect, see @ref bt_mesh_dfu_effect. */
    uint8_t effect;
    /** Current DFU status, see @ref bt_mesh_dfu_status. */
    uint8_t status;
    /** Current DFU phase, see @ref bt_mesh_dfu_phase. */
    uint8_t phase;
};

/** Metadata status response. */
struct bt_mesh_dfu_metadata_status {
    /** Image index. */
    uint8_t idx;
    /** Status code. */
    enum bt_mesh_dfu_status status;
    /** Effect of transfer. */
    enum bt_mesh_dfu_effect effect;
};

/** DFU Target node status parameters.
 * The structure should be same as
 * @ref esp_ble_mesh_device_firmware_update_status_t
*/
struct bt_mesh_dfu_target_status {
    /** Status of the previous operation. */
    enum bt_mesh_dfu_status status;
    /** Phase of the current DFU transfer. */
    enum bt_mesh_dfu_phase phase;
    /** The effect the update will have on the Target device's state. */
    enum bt_mesh_dfu_effect effect;
    /** BLOB ID used in the transfer. */
    uint64_t blob_id;
    /** Image index to transfer. */
    uint8_t img_idx;
    /** TTL used in the transfer. */
    uint8_t ttl;

    /** Additional response time for the Target nodes, in 10-second increments.
     *
     *  The extra time can be used to give the Target nodes more time to respond
     *  to messages from the Client. The actual timeout will be calculated
     *  according to the following formula:
     *
     *  @verbatim
     *  timeout = 20 seconds + (10 seconds * timeout_base) + (100 ms * TTL)
     *  @endverbatim
     *
     *  If a Target node fails to respond to a message from the Client within the
     *  configured transfer timeout, the Target node is dropped.
     */
    uint16_t timeout_base;
};

/** @brief DFU image callback.
 *
 *  The image callback is called for every DFU image on the Target node when
 *  calling @ref bt_mesh_dfu_cli_imgs_get.
 *
 *  @param cli     Firmware Update Client model instance.
 *  @param ctx     Message context of the received message.
 *  @param idx     Image index.
 *  @param total   Total number of images on the Target node.
 *  @param img     Image information for the given image index.
 *  @param cb_data Callback data.
 *
 *  @retval BLE_MESH_DFU_ITER_STOP     Stop iterating through the image list and
 *                                    return from @ref bt_mesh_dfu_cli_imgs_get.
 *  @retval BLE_MESH_DFU_ITER_CONTINUE Continue iterating through the image list
 *                                    if any images remain.
 */
typedef enum bt_mesh_dfu_iter(*bt_mesh_dfu_img_cb_t)(struct bt_mesh_dfu_cli *cli,
                                                     struct bt_mesh_msg_ctx *ctx,
                                                     uint8_t idx, uint8_t total,
                                                     const struct bt_mesh_dfu_img *img,
                                                     void *cb_data);

/** Firmware Update Client event callbacks. */
struct bt_mesh_dfu_cli_cb {
    /** @brief BLOB transfer is suspended.
     *
     * Called when the BLOB transfer is suspended due to response timeout from all Target nodes.
     *
     * @param cli    Firmware Update Client model instance.
     */
    void (*suspended)(struct bt_mesh_dfu_cli *cli);

    /** @brief DFU ended.
     *
     *  Called when the DFU transfer ends, either because all Target nodes were
     *  lost or because the transfer was completed successfully.
     *
     *  @param cli    Firmware Update Client model instance.
     *  @param reason Reason for ending.
     */
    void (*ended)(struct bt_mesh_dfu_cli *cli,
                  enum bt_mesh_dfu_status reason);

    /** @brief DFU transfer applied on all active Target nodes.
     *
     *  Called at the end of the apply procedure started by @ref
     *  bt_mesh_dfu_cli_apply.
     *
     *  @param cli Firmware Update Client model instance.
     */
    void (*applied)(struct bt_mesh_dfu_cli *cli);

    /** @brief DFU transfer confirmed on all active Target nodes.
     *
     *  Called at the end of the apply procedure started by @ref
     *  bt_mesh_dfu_cli_confirm.
     *
     *  @param cli Firmware Update Client model instance.
     */
    void (*confirmed)(struct bt_mesh_dfu_cli *cli);

    /** @brief DFU Target node was lost.
     *
     *  A DFU Target node was dropped from the receivers list. The Target node's
     *  @c status is set to reflect the reason for the failure.
     *
     *  @param cli    Firmware Update Client model instance.
     *  @param target DFU Target node that was lost.
     */
    void (*lost_target)(struct bt_mesh_dfu_cli *cli,
                        struct bt_mesh_dfu_target *target);
};

enum {
    STATE_IDLE,
    STATE_TRANSFER,
    STATE_REFRESH,
    STATE_VERIFIED,
    STATE_APPLY,
    STATE_APPLIED,
    STATE_CONFIRM,
    STATE_CANCEL,
    STATE_SUSPENDED,
};

typedef struct {
    sys_snode_t node;
    uint8_t ttl;
    uint8_t type;
    uint8_t img_cnt;
    uint32_t opcode;
    void *params;
    bt_mesh_dfu_img_cb_t img_cb;
    struct bt_mesh_dfu_cli *dfu_cli;
    struct bt_mesh_msg_ctx ctx;
    int32_t  timeout;
    struct k_delayed_work timer;    /* Time used to get response. Only for internal use. */
} bt_mesh_dfu_cli_req_t;

/** Firmware Update Client model instance.
 *
 *  Should be initialized with @ref BLE_MESH_DFU_CLI_INIT.
 */
struct bt_mesh_dfu_cli {
    /** Callback structure. */
    const struct bt_mesh_dfu_cli_cb *cb;
    /** Underlying BLOB Transfer Client. */
    struct bt_mesh_blob_cli blob;

    /* runtime state */

    uint32_t op;
    struct bt_mesh_model *mod;

    struct {
        const struct bt_mesh_dfu_slot *slot;
        const struct bt_mesh_blob_io *io;
        struct bt_mesh_blob_xfer blob;
        uint8_t state;
        uint8_t flags;
    } xfer;

    bt_mesh_dfu_cli_req_t *req;
};

/** BLOB parameters for Firmware Update Client transfer: */
struct bt_mesh_dfu_cli_xfer_blob_params {
    /* Logarithmic representation of the block size. */
    uint8_t block_size_log;
    /** Base chunk size. May be smaller for the last chunk. */
    uint16_t chunk_size;
    /* Enhanced parameters for message context in blob chunks  */
    bt_mesh_msg_enh_params_t chunk_enh_params;
};

/** Firmware Update Client transfer parameters: */
struct bt_mesh_dfu_cli_xfer {
    /** BLOB ID to use for this transfer, or 0 to set it randomly. */
    uint64_t blob_id;
    /** DFU image slot to transfer. */
    const struct bt_mesh_dfu_slot *slot;
    /**  Transfer mode (Push (Push BLOB Transfer Mode) or Pull (Pull BLOB Transfer Mode)) */
    enum bt_mesh_blob_xfer_mode mode;
    /** BLOB parameters to be used for the transfer, or NULL to retrieve Target nodes'
     *  capabilities before sending a firmware.
     */
    const struct bt_mesh_dfu_cli_xfer_blob_params *blob_params;
};

void dfu_targets_free(void);
struct bt_mesh_blob_cli_inputs *dfu_targets_alloc(struct bt_mesh_blob_cli_inputs *src);

/** @brief Start distributing a DFU.
 *
 *  Starts distribution of the firmware in the given slot to the list of DFU
 *  Target nodes in @c ctx. The transfer runs in the background, and its end is
 *  signalled through the @ref bt_mesh_dfu_cli_cb::ended callback.
 *
 *  @note The BLOB Transfer Client transfer inputs @c targets list must point to a list of @ref
 *  bt_mesh_dfu_target nodes.
 *
 *  @param cli    Firmware Update Client model instance.
 *  @param inputs BLOB Transfer Client transfer inputs.
 *  @param io     BLOB stream to read BLOB from.
 *  @param xfer   Firmware Update Client transfer parameters.
 *
 *  @return 0 on success, or (negative) error code otherwise.
 */
int bt_mesh_dfu_cli_send(struct bt_mesh_dfu_cli *cli,
                         const struct bt_mesh_blob_cli_inputs *inputs,
                         const struct bt_mesh_blob_io *io,
                         const struct bt_mesh_dfu_cli_xfer *xfer);

/** @brief Suspend a DFU transfer.
 *
 *  @param cli Firmware Update Client instance.
 *
 *  @return 0 on success, or (negative) error code otherwise.
 */
int bt_mesh_dfu_cli_suspend(struct bt_mesh_dfu_cli *cli);

/** @brief Resume the suspended transfer.
 *
 *  @param cli Firmware Update Client instance.
 *
 *  @return 0 on success, or (negative) error code otherwise.
 */
int bt_mesh_dfu_cli_resume(struct bt_mesh_dfu_cli *cli);

/** @brief Cancel a DFU transfer.
 *
 *  Will cancel the ongoing DFU transfer, or the transfer on a specific Target
 *  node if @c ctx is valid.
 *
 *  @param cli Firmware Update Client model instance.
 *  @param ctx Message context, or NULL to cancel the ongoing DFU transfer.
 *
 *  @return 0 on success, or (negative) error code otherwise.
 */
int bt_mesh_dfu_cli_cancel(struct bt_mesh_dfu_cli *cli,
                           struct bt_mesh_msg_ctx *ctx);

/** @brief Apply the completed DFU transfer.
 *
 *  A transfer can only be applied after it has ended successfully. The Firmware
 *  Update Client's @c applied callback is called at the end of the apply procedure.
 *
 *  @param cli Firmware Update Client model instance.
 *
 *  @return 0 on success, or (negative) error code otherwise.
 */
int bt_mesh_dfu_cli_apply(struct bt_mesh_dfu_cli *cli);

/** @brief Confirm that the active transfer has been applied on the Target nodes.
 *
 *  A transfer can only be confirmed after it has been applied. The Firmware Update
 *  Client's @c confirmed callback is called at the end of the confirm
 *  procedure.
 *
 *  Target nodes that have reported the effect as @ref BLE_MESH_DFU_EFFECT_UNPROV
 *  are expected to not respond to the query, and will fail if they do.
 *
 *  @param cli Firmware Update Client model instance.
 *
 *  @return 0 on success, or (negative) error code otherwise.
 */
int bt_mesh_dfu_cli_confirm(struct bt_mesh_dfu_cli *cli);

/** @brief Get progress as a percentage of completion.
 *
 *  @param cli Firmware Update Client model instance.
 *
 *  @return The progress of the current transfer in percent, or 0 if no
 *          transfer is active.
 */
uint8_t bt_mesh_dfu_cli_progress(struct bt_mesh_dfu_cli *cli);

/** @brief Check whether a DFU transfer is in progress.
 *
 *  @param cli Firmware Update Client model instance.
 *
 *  @return true if the BLOB Transfer Client is currently participating in a transfer,
 *          false otherwise.
 */
bool bt_mesh_dfu_cli_is_busy(struct bt_mesh_dfu_cli *cli);

/** @brief Perform a DFU image list request.
 *
 *  Requests the full list of DFU images on a Target node, and iterates through
 *  them, calling the @c cb for every image.
 *
 *  The DFU image list request can be used to determine which image index the
 *  Target node holds its different firmwares in.
 *
 *  Waits for a response until the procedure timeout expires.
 *
 *  @param cli       Firmware Update Client model instance.
 *  @param ctx       Message context.
 *  @param cb        Callback to call for each image index.
 *  @param cb_data   Callback data to pass to @c cb.
 *  @param first_idx Index of the first requested imgs entry.
 *  @param max_count Max number of images to return.
 *
 *  @return 0 on success, or (negative) error code otherwise.
 */
int bt_mesh_dfu_cli_imgs_get(struct bt_mesh_dfu_cli *cli,
                             struct bt_mesh_msg_ctx *ctx,
                             bt_mesh_dfu_img_cb_t cb, void *cb_data,
                             uint8_t first_idx, uint8_t max_count);

/** @brief Perform a metadata check for the given DFU image slot.
 *
 *  The metadata check procedure allows the Firmware Update Client to check if a Target
 *  node will accept a transfer of this DFU image slot, and what the effect would be.
 *
 *  Waits for a response until the procedure timeout expires.
 *
 *  @param cli      Firmware Update Client model instance.
 *  @param ctx      Message context.
 *  @param img_idx  Target node's image index to check.
 *  @param metadata DFU image metadata to check for.
 *
 *  @return 0 on success, or (negative) error code otherwise.
 */
int bt_mesh_dfu_cli_metadata_check(struct bt_mesh_dfu_cli *cli,
                                   struct bt_mesh_msg_ctx *ctx, uint8_t img_idx,
                                   struct net_buf_simple *metadata);

/** @brief Get the status of a Target node.
 *
 *  @param cli Firmware Update Client model instance.
 *  @param ctx Message context.
 *
 *  @return 0 on success, or (negative) error code otherwise.
 */
int bt_mesh_dfu_cli_status_get(struct bt_mesh_dfu_cli *cli,
                               struct bt_mesh_msg_ctx *ctx);

/** @brief Get the current procedure timeout value.
 *
 *  @return The configured procedure timeout.
 */
int32_t bt_mesh_dfu_cli_timeout_get(void);

/** @brief Set the procedure timeout value.
 *
 *  @param timeout The new procedure timeout.
 */
void bt_mesh_dfu_cli_timeout_set(int32_t timeout);

/** @cond INTERNAL_HIDDEN */
extern const struct bt_mesh_blob_cli_cb _bt_mesh_dfu_cli_blob_handlers;
extern const struct bt_mesh_model_cb _bt_mesh_dfu_cli_cb;
extern const struct bt_mesh_model_op _bt_mesh_dfu_cli_op[];
/** @endcond */

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MESH_v11_DFU_CLI_H__ */
