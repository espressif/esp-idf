/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @defgroup bt_mesh_dfd_cli Device Firmware Distribution Client model
 * @ingroup bt_mesh_dfd
 * @{
 * @brief API for the Device Firmware Distribution Client model
 */

#ifndef _BLE_MESH_v11_DFD_CLI_H__
#define _BLE_MESH_v11_DFD_CLI_H__

#include "mesh/access.h"
#include "mesh/client_common.h"
#include "mesh_v1.1/dfu/dfd.h"
#include "mesh_v1.1/dfu/dfu_cli.h"
#include "mesh_v1.1/mbt/blob_cli.h"
#include "mesh_v1.1/mbt/blob_srv.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Device Firmware Distribution Client model instance. */
typedef bt_mesh_client_user_data_t bt_mesh_dfd_client_t;

/** @brief DFD Client internal data structure. */
typedef bt_mesh_client_internal_data_t dfd_internal_data_t;

/*!< Maximum value of upload progress indicating progress is unset. */
#define UPLOAD_PROGRESS_UNSET 101

/*!< In-band upload type. */
#define UPLOAD_IN_BAND        0x00

/*!< Out-of-band upload type. */
#define UPLOAD_OOB            0x01

/*!< Upload type is not set. */
#define UPLOAD_TYPE_UNSET     0xFF

/*!< Extract target address from target info bitfield. */
#define TARGET_ADDR(target_info) (target_info >> 17)

/*!< Extract target update phase from target info bitfield. */
#define TARGET_UPDATE_PHASE(target_info) ((target_info >> 13) & 0x0f)

/*!< Extract target update status from target info bitfield. */
#define TARGET_UPDATE_STATUS(target_info) ((target_info >> 10) & 0x07)

/*!< Extract target transfer status from target info bitfield. */
#define TARGET_TRANSFER_STATUS(target_info) ((target_info >> 6) & 0x0f)

/*!< Extract target transfer progress from target info bitfield. */
#define TARGET_TRANSFER_PROGRESS(target_info) (target_info & 0x3f)

/**
 * @brief DFD Client receiver entry structure.
 *
 * This structure represents a single receiver node that will receive firmware
 * distribution. Each receiver is identified by its unicast address and the
 * firmware image index that should be distributed to it.
 */
typedef struct {
    uint16_t addr;      /*!< Unicast address of the receiver node. */
    uint8_t fw_idx;     /*!< Index of the firmware image to distribute. */
} dfd_cli_receiver_entry_t;

/**
 * @brief Target node entry information structure.
 *
 * This structure contains detailed information about a target node's
 * firmware distribution status, including address, update phase, transfer
 * progress, and other related information.
 */
typedef struct {
    uint32_t addr:15;                     /*!< Unicast address of the target node. */
    uint32_t retrieved_update_phase:4;    /*!< Current update phase of the target. */
    uint32_t update_status:3;             /*!< Update status of the target node. */
    uint32_t transfer_status:4;           /*!< Transfer status of the target. */
    uint32_t transfer_progress:6;         /*!< Transfer progress percentage. */
    uint8_t update_fw_idx;                /*!< Index of firmware being updated. */
} target_node_entry_t;

/**
 * @brief DFD receiver status response structure.
 *
 * This structure contains the status response for receiver add/delete
 * operations, including the operation result and current receiver list size.
 */
typedef struct {
    uint8_t status;                 /*!< Status code of the operation (see @ref bt_mesh_dfd_status). */
    uint16_t receiver_list_cnt;     /*!< Current number of receivers in the distribution list. */
} dfd_cli_receiver_status_t;

/**
 * @brief DFD receiver list response structure.
 *
 * This structure contains the response to a receivers get request,
 * providing a list of target nodes with their current status information.
 */
typedef struct {
    uint16_t entries_cnt;                /*!< Number of receiver entries in the list. */
    uint16_t first_index;                /*!< Index of the first entry in the list. */
    target_node_entry_t *entries;        /*!< Pointer to array of target node entries. */
} dfd_cli_receiver_list_t;

/**
 * @brief DFD distribution capabilities structure.
 *
 * This structure contains the capabilities of the DFD Server, including
 * storage limits, receiver limits, and supported features.
 */
typedef struct {
    uint16_t max_receiver_list_sz;                     /*!< Maximum number of receivers supported. */
    uint16_t max_fw_list_sz;                          /*!< Maximum number of firmware images supported. */
    uint32_t max_fw_sz;                               /*!< Maximum firmware image size supported. */
    uint32_t max_upload_space;                        /*!< Total upload storage space available. */
    uint32_t remaining_upload_space;                  /*!< Remaining upload storage space. */
    uint8_t oob_retrieval_supported;                  /*!< OOB retrieval support flag. */
    struct net_buf_simple *supported_url_scheme_names; /*!< Supported URL scheme names. */
} dfd_cli_dist_caps_t;

/**
 * @brief DFD distribution status structure.
 *
 * This structure contains the current status of an ongoing or completed
 * firmware distribution operation, including phase, progress, and configuration.
 */
typedef struct {
    uint8_t status;                 /*!< Distribution status code (see @ref bt_mesh_dfd_status). */
    uint8_t dist_phase;             /*!< Current distribution phase (see @ref bt_mesh_dfd_phase). */
    uint16_t multicast_address;     /*!< Multicast address used for distribution. */
    uint16_t appkey_idx;            /*!< Application key index used for distribution. */
    uint8_t ttl;                    /*!< TTL value used for distribution messages. */
    uint16_t timeout_base;          /*!< Base timeout value for distribution. */
    uint8_t trans_mode:2;           /*!< Transfer mode (push or pull). */
    uint8_t update_policy:1;        /*!< Update policy (single or all). */
    uint8_t RFU:5;                  /*!< Reserved for future use. */
    uint16_t fw_idx;                /*!< Index of firmware being distributed. */
} dfd_cli_dist_status_t;

/**
 * @brief DFD upload status structure.
 *
 * This structure contains the current status of a firmware upload operation,
 * including phase, progress, and firmware identification.
 */
typedef struct {
    uint8_t status;                 /*!< Upload operation status code (see @ref bt_mesh_dfd_status). */
    uint8_t upload_phase;           /*!< Current upload phase (see @ref bt_mesh_dfd_upload_phase). */
    uint8_t upload_progress:7;      /*!< Upload progress percentage (0-100). */
    uint8_t upload_type:1;          /*!< Upload type (in-band or OOB). */
    union {
        struct net_buf_simple *fwid;        /*!< Firmware ID for in-band uploads. */
        struct net_buf_simple *oob_fwid;    /*!< Firmware ID for OOB uploads. */
    };
} dfd_cli_upload_status_t;

/**
 * @brief DFD firmware status structure.
 *
 * This structure contains the status response for firmware management
 * operations, including information about firmware images.
 */
typedef struct {
    uint8_t status;                     /*!< Firmware operation status code (see @ref bt_mesh_dfd_status). */
    uint16_t entry_cnt;                 /*!< Number of firmware entries. */
    uint16_t firmware_image_index;      /*!< Index of the firmware image. */
    struct net_buf_simple *fwid;        /*!< Firmware ID buffer. */
} dfd_cli_firmware_status_t;

/**
 * @brief DFD distribution start parameters structure.
 *
 * This structure contains parameters for starting a firmware distribution
 * operation, including target configuration and distribution settings.
 */
typedef struct {
    uint8_t ttl;                    /*!< TTL value for distribution messages. */
    uint8_t trans_mode:2;           /*!< Transfer mode (push or pull). */
    uint8_t update_policy:1;        /*!< Update policy (single or all). */
    uint8_t RFU:5;                  /*!< Reserved for future use. */
    uint16_t app_idx;               /*!< Application key index for distribution. */
    uint16_t timeout_base;          /*!< Base timeout value for distribution. */
    uint16_t fw_idx;                /*!< Index of firmware to distribute. */
    bool is_va;                     /*!< True if using virtual address, false for group address. */
    union {
        uint16_t group_addr;        /*!< Group address for distribution (if is_va is false). */
        uint8_t label_uuid[16];     /*!< Virtual label UUID for distribution (if is_va is true). */
    };
} dfd_cli_dist_start_t;

/**
 * @brief DFD distribution upload start parameters structure.
 *
 * This structure contains parameters for starting an in-band firmware
 * upload operation to the DFD Server.
 */
typedef struct {
    uint8_t ttl;                            /*!< TTL value for upload messages. */
    uint16_t timeout_base;                  /*!< Base timeout value for upload. */
    uint32_t fw_size;                       /*!< Size of the firmware image in bytes. */
    uint64_t blob_id;                       /*!< BLOB ID for the firmware transfer. */
    struct net_buf_simple *fwid;            /*!< Firmware ID buffer. */
    struct net_buf_simple *fw_metadata;     /*!< Firmware metadata buffer. */
} dfd_cli_dist_upload_start_t;

/**
 * @brief DFD distribution OOB upload start parameters structure.
 *
 * This structure contains parameters for starting an out-of-band firmware
 * upload operation to the DFD Server.
 */
typedef struct {
    struct net_buf_simple *fwid;    /*!< Firmware ID buffer. */
    struct net_buf_simple *url;     /*!< URL for retrieving the firmware. */
} dfd_cli_dist_upload_oob_start_t;

/**
 * @brief DFD status response union.
 *
 * This union contains different status response structures for various DFD
 * Client operations, allowing type-safe access to operation-specific responses.
 */
typedef union {
    dfd_cli_receiver_status_t receiver_status;     /*!< Receiver add/delete status. */
    dfd_cli_receiver_list_t receiver_list;         /*!< Receiver list response. */
    dfd_cli_dist_caps_t dist_caps;                 /*!< Distribution capabilities. */
    dfd_cli_dist_status_t dist_status;             /*!< Distribution status. */
    dfd_cli_upload_status_t upload_status;         /*!< Upload status. */
    dfd_cli_firmware_status_t firmware_status;     /*!< Firmware management status. */
} dfd_status_t;

/*!< DFD Client model operation handlers. */
extern const struct bt_mesh_model_op _bt_mesh_dfd_cli_op[];

/*!< DFD Client model callbacks. */
extern const struct bt_mesh_model_cb _bt_mesh_dfd_cli_cb;

/**
 * @brief Add receivers to the distribution list.
 *
 * Add multiple target nodes to the DFD Server's distribution receiver list.
 * These receivers will be included in subsequent firmware distribution operations.
 *
 * @param[in]  param          Common client parameters for the operation.
 * @param[in]  receivers      Pointer to array of receiver entries to add.
 * @param[in]  receivers_cnt  Number of receiver entries in the array.
 *
 * @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_dfd_cli_receivers_add(bt_mesh_client_common_param_t *param,
                                 dfd_cli_receiver_entry_t *receivers,
                                 uint16_t receivers_cnt);

/**
 * @brief Delete all receivers from the distribution list.
 *
 * Remove all target nodes from the DFD Server's distribution receiver list.
 * This operation will clear the entire receiver list.
 *
 * @param[in]  param  Common client parameters for the operation.
 *
 * @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_dfd_cli_receivers_delete_all(bt_mesh_client_common_param_t *param);

/**
 * @brief Get the distribution receiver list.
 *
 * Retrieve a portion of the DFD Server's distribution receiver list.
 * Supports pagination to retrieve large receiver lists in chunks.
 *
 * @param[in]  param        Common client parameters for the operation.
 * @param[in]  first_index  Index of the first receiver entry to retrieve.
 * @param[in]  entries_limit Maximum number of entries to retrieve.
 *
 * @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_dfd_cli_receivers_get(bt_mesh_client_common_param_t *param,
                                 uint16_t first_index,
                                 uint16_t entries_limit);

/**
 * @brief Get distribution capabilities from the DFD Server.
 *
 * Query the DFD Server's capabilities, including storage limits,
 * supported features, and maximum receiver count.
 *
 * @param[in]  param  Common client parameters for the operation.
 *
 * @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_dfd_cli_distribution_capabilities_get(bt_mesh_client_common_param_t *param);
/**
 * @brief Get current distribution status from the DFD Server.
 *
 * Query the current status of firmware distribution operations,
 * including phase, progress, and configuration information.
 *
 * @param[in]  param  Common client parameters for the operation.
 *
 * @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_dfd_cli_distribution_get(bt_mesh_client_common_param_t *param);

/**
 * @brief Start firmware distribution to receivers.
 *
 * Initiate firmware distribution to all target nodes in the distribution
 * receiver list using the specified configuration parameters.
 *
 * @param[in]  param  Common client parameters for the operation.
 * @param[in]  start  Distribution start parameters and configuration.
 *
 * @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_dfd_cli_distribution_start(bt_mesh_client_common_param_t *param,
                                       dfd_cli_dist_start_t *start);

/**
 * @brief Suspend ongoing firmware distribution.
 *
 * Suspend a currently active firmware distribution operation.
 * The distribution can be resumed later if needed.
 *
 * @param[in]  param  Common client parameters for the operation.
 *
 * @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_dfd_cli_distribution_suspend(bt_mesh_client_common_param_t *param);

/**
 * @brief Cancel firmware distribution.
 *
 * Cancel an ongoing or suspended firmware distribution operation.
 * This will terminate the distribution process.
 *
 * @param[in]  param  Common client parameters for the operation.
 *
 * @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_dfd_cli_distribution_cancel(bt_mesh_client_common_param_t *param);

/**
 * @brief Apply distributed firmware on target nodes.
 *
 * Apply the distributed firmware image on target nodes that have
 * successfully received the firmware transfer.
 *
 * @param[in]  param  Common client parameters for the operation.
 *
 * @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_dfd_cli_distribution_apply(bt_mesh_client_common_param_t *param);
/**
 * @brief Get firmware upload status from the DFD Server.
 *
 * Query the current status of firmware upload operations, including
 * phase, progress, and transfer information.
 *
 * @param[in]  param  Common client parameters for the operation.
 *
 * @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_dfd_cli_distribution_upload_get(bt_mesh_client_common_param_t *param);

/**
 * @brief Start in-band firmware upload to the DFD Server.
 *
 * Initiate an in-band firmware upload using BLOB transfer protocol.
 * The firmware image will be transferred directly through the mesh network.
 *
 * @param[in]  param  Common client parameters for the operation.
 * @param[in]  start  Upload start parameters including firmware details.
 *
 * @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_dfd_cli_distribution_upload_start(bt_mesh_client_common_param_t *param,
                                              dfd_cli_dist_upload_start_t *start);

/**
 * @brief Start out-of-band firmware upload to the DFD Server.
 *
 * Initiate an out-of-band firmware upload where the DFD Server
 * retrieves the firmware from the specified URL.
 *
 * @param[in]  param  Common client parameters for the operation.
 * @param[in]  start  OOB upload start parameters including URL and firmware ID.
 *
 * @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_dfd_cli_distribution_upload_oob_start(bt_mesh_client_common_param_t *param,
                                                  dfd_cli_dist_upload_oob_start_t *start);

/**
 * @brief Cancel out-of-band firmware upload.
 *
 * Cancel an ongoing out-of-band firmware upload operation.
 *
 * @param[in]  param  Common client parameters for the operation.
 *
 * @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_dfd_cli_distribution_upload_oob_cancel(bt_mesh_client_common_param_t *param);
/**
 * @brief Get firmware information by firmware ID.
 *
 * Query detailed information about a specific firmware image
 * stored on the DFD Server using its firmware ID.
 *
 * @param[in]  param  Common client parameters for the operation.
 * @param[in]  fwid   Firmware ID to query information for.
 *
 * @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_dfd_cli_firmware_get(bt_mesh_client_common_param_t *param,
                                struct net_buf_simple *fwid);

/**
 * @brief Get firmware information by index.
 *
 * Query detailed information about a firmware image stored on the
 * DFD Server using its index in the firmware list.
 *
 * @param[in]  param        Common client parameters for the operation.
 * @param[in]  fw_id_index  Index of the firmware image to query.
 *
 * @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_dfd_cli_firmware_get_by_index(bt_mesh_client_common_param_t *param,
                                          uint16_t fw_id_index);

/**
 * @brief Delete specific firmware image by firmware ID.
 *
 * Delete a specific firmware image from the DFD Server's storage
 * using its firmware ID.
 *
 * @param[in]  param  Common client parameters for the operation.
 * @param[in]  fwid   Firmware ID of the image to delete.
 *
 * @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_dfd_cli_firmware_get_delete(bt_mesh_client_common_param_t *param,
                                       struct net_buf_simple *fwid);

/**
 * @brief Delete all firmware images from the DFD Server.
 *
 * Delete all firmware images stored on the DFD Server.
 * This operation will clear the entire firmware storage.
 *
 * @param[in]  param  Common client parameters for the operation.
 *
 * @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_dfd_cli_firmware_delete_all(bt_mesh_client_common_param_t *param);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* _BLE_MESH_v11_DFD_CLI_H__ */
