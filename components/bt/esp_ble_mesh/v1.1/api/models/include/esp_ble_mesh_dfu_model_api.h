/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_BLE_MESH_DFU_MODEL_API_H_
#define _ESP_BLE_MESH_DFU_MODEL_API_H_

#include "esp_ble_mesh_defs.h"
#include "esp_ble_mesh_blob_model_api.h"
#include "esp_ble_mesh_dfu_slot_api.h"

#if CONFIG_BLE_MESH_DFU_CLI || CONFIG_BLE_MESH_DFU_SRV
#ifndef _BLE_MESH_BLOB_DEPRECATE_WARN
#define _BLE_MESH_BLOB_DEPRECATE_WARN
#warning Please note: All APIs published in this document are in Preview version and may undergo significant changes in the future.
#endif
#endif /* CONFIG_BLE_MESH_DFU_CLI || CONFIG_BLE_MESH_DFU_SRV */

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_BLE_MESH_DFU_OP_UPDATE_INFO_GET             ESP_BLE_MESH_MODEL_OP_2(0x83, 0x08)
#define ESP_BLE_MESH_DFU_OP_UPDATE_INFO_STATUS          ESP_BLE_MESH_MODEL_OP_2(0x83, 0x09)

#define ESP_BLE_MESH_DFU_OP_UPDATE_METADATA_CHECK       ESP_BLE_MESH_MODEL_OP_2(0x83, 0x0a)
#define ESP_BLE_MESH_DFU_OP_UPDATE_METADATA_STATUS      ESP_BLE_MESH_MODEL_OP_2(0x83, 0x0b)

#define ESP_BLE_MESH_DFU_OP_UPDATE_GET                  ESP_BLE_MESH_MODEL_OP_2(0x83, 0x0c)
#define ESP_BLE_MESH_DFU_OP_UPDATE_START                ESP_BLE_MESH_MODEL_OP_2(0x83, 0x0d)
#define ESP_BLE_MESH_DFU_OP_UPDATE_CANCEL               ESP_BLE_MESH_MODEL_OP_2(0x83, 0x0e)
#define ESP_BLE_MESH_DFU_OP_UPDATE_APPLY                ESP_BLE_MESH_MODEL_OP_2(0x83, 0x0f)
#define ESP_BLE_MESH_DFU_OP_UPDATE_STATUS               ESP_BLE_MESH_MODEL_OP_2(0x83, 0x10)

#if CONFIG_BLE_MESH_DFU_CLI
/** @def    ESP_BLE_MESH_MODEL_DFU_CLI
 *
 *  @brief  Define a new DFU Client model.
 *
 *  @note   This API needs to be called for each element on which
 *          the application needs to have a DFU Client model.
 *
 *  @param  cli_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  cli_data Pointer to the unique struct esp_ble_mesh_client_t.
 *
 *  @return New DFU Client model instance.
 */
#define ESP_BLE_MESH_MODEL_DFU_CLI(cli_pub, cli_data)         \
        ESP_BLE_MESH_MODEL_BLOB_CLI(NULL, &(cli_data)->blob), \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_DFU_CLI, \
                               NULL, cli_pub, cli_data)


/** @def    ESP_BLE_MESH_DFU_CLI_INIT
 *
 *  @brief  Initialize a DFU Client model instance.
 *
 *  @note   This macro is used to initialize the DFU Client model.
 *          It sets the callback handlers for the DFU Client.
 *
 *  @param  _handlers Pointer to the callback handler structure (esp_ble_mesh_dfu_cli_cb_t).
 *
 *  @return An initialized DFU Client model instance.
 */
#define ESP_BLE_MESH_DFU_CLI_INIT(_handlers)                           \
{                                                                  \
    .cb = _handlers,                                               \
}
#endif /* CONFIG_BLE_MESH_DFU_CLI */

#if CONFIG_BLE_MESH_DFU_SRV
/** @def    ESP_BLE_MESH_MODEL_DFU_SRV
 *
 *  @brief  Define a new DFU Server model.
 *
 *  @note   This API needs to be called for each element on which
 *          the application needs to have a DFU Server model.
 *
 *  @param  srv_pub  Pointer to the unique struct esp_ble_mesh_model_pub_t.
 *  @param  srv_data Pointer to the unique struct esp_ble_mesh_dfu_trans_srv_t.
 *
 *  @return New DFU Server model instance.
 */
#define ESP_BLE_MESH_MODEL_DFU_SRV(srv_pub, srv_data)         \
        ESP_BLE_MESH_MODEL_BLOB_SRV(NULL, &(srv_data)->blob), \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_DFU_SRV, \
                               NULL, srv_pub, srv_data)


/** @def    ESP_BLE_MESH_DFU_SRV_INIT
 *
 *  @brief  Initialize a DFU Server model instance.
 *
 *  @note   This macro is used to initialize the DFU Server model.
 *          It sets the callback handlers and image information for the DFU Server.
 *
 *  @param  _handlers  Pointer to the callback handler structure (esp_ble_mesh_dfu_srv_cb_t).
 *  @param  _imgs      Pointer to the array of DFU images (esp_ble_mesh_dfu_img_t).
 *  @param  _img_count Number of DFU images in the array.
 *
 *  @return An initialized DFU Server model instance.
 */
#define ESP_BLE_MESH_DFU_SRV_INIT(_handlers, _imgs, _img_count)            \
    {                                                                      \
        .cb = _handlers,                                                   \
        .imgs = _imgs, .img_count = _img_count,                            \
    }
#endif /* CONFIG_BLE_MESH_DFU_SRV */

/** @brief DFU image structure. **/
typedef struct esp_ble_mesh_dfu_img esp_ble_mesh_dfu_img_t;

#if CONFIG_BLE_MESH_DFU_CLI
/** @brief DFU Client model instance structure.
 *
 *  This structure represents a DFU (Device Firmware Update) Client model instance.
 *  It contains the necessary fields for managing DFU operations as a client.
 *  Should be initialized with ESP_BLE_MESH_DFU_CLI_INIT.
 *
 *  @note Preview version, the contents of this struct may change in the future.
 */
typedef struct esp_ble_mesh_dfu_cli esp_ble_mesh_dfu_cli_t;

/** @brief DFU iteration action enumeration.
 *
 *  This enumeration defines the possible actions that can be taken
 *  during DFU iteration callbacks. It controls whether to continue
 *  or stop iterating through DFU images.
 */
typedef enum esp_ble_mesh_dfu_iter esp_ble_mesh_dfu_iter_t;

/** @brief DFU image callback.
 *
 *  The image callback is called for every DFU image on the Target node when
 *  calling bt_mesh_dfu_cli_imgs_get.
 *
 *  @param cli     Firmware Update Client model instance.
 *  @param ctx     Message context of the received message.
 *  @param idx     Image index.
 *  @param total   Total number of images on the Target node.
 *  @param img     Image information for the given image index.
 *  @param cb_data Callback data.
 *
 *  @retval ESP_BLE_MESH_DFU_ITER_STOP     Stop iterating through the image list and
 *                                         return from bt_mesh_dfu_cli_imgs_get.
 *  @retval ESP_BLE_MESH_DFU_ITER_CONTINUE Continue iterating through the image list
 *                                         if any images remain.
 */
typedef esp_ble_mesh_dfu_iter_t (*esp_ble_mesh_dfu_img_cb_t)(esp_ble_mesh_dfu_cli_t *cli,
                                                             esp_ble_mesh_msg_ctx_t *ctx, uint8_t idx,
                                                             uint8_t total, const esp_ble_mesh_dfu_img_t *img,
                                                             void *cb_data);
/** @brief DFU image slot structure.
 *
 *  This structure represents a DFU image slot used for DFU distribution.
 *  It contains information about the firmware image including size,
 *  firmware ID, metadata, and their respective lengths.
 */
typedef struct esp_ble_mesh_dfu_slot esp_ble_mesh_dfu_slot_t;

/** @brief DFU Target node structure.
 *
 *  This structure represents a DFU Target node in the mesh network.
 *  It contains information about the target's BLOB transfer status,
 *  image index, expected DFU effect, current status, and phase.
 */
typedef struct esp_ble_mesh_dfu_target esp_ble_mesh_dfu_target_t;

/** @brief DFU Client callback structure.
 *
 *  This structure contains callback function pointers for DFU Client events
 *  such as transfer suspension, completion, application, confirmation,
 *  and target node loss.
 */
typedef struct esp_ble_mesh_dfu_cli_cb esp_ble_mesh_dfu_cli_cb_t;

/** @brief DFU Client model instance structure.
 *
 *  This structure represents a DFU (Device Firmware Update) Client model instance.
 *  It contains the necessary fields for managing DFU operations as a client.
 *  Should be initialized with ESP_BLE_MESH_DFU_CLI_INIT.
 *
 *  @note Preview version, the contents of this struct may change in the future.
 */
typedef struct esp_ble_mesh_dfu_cli esp_ble_mesh_dfu_cli_t;

/** @brief DFU Client BLOB transfer parameters structure.
 *
 *  This structure contains parameters for BLOB transfer during DFU operations,
 *  including block size logarithmic representation and base chunk size.
 */
typedef struct esp_ble_mesh_dfu_cli_xfer_blob_params esp_ble_mesh_dfu_cli_xfer_blob_params_t;

/** @brief DFU Client transfer parameters structure.
 *
 *  This structure contains parameters for DFU transfer operations,
 *  including BLOB ID, image slot, transfer mode, and BLOB parameters.
 */
typedef struct esp_ble_mesh_dfu_cli_xfer esp_ble_mesh_dfu_cli_xfer_t;

/** @brief Device Firmware Update Info Get parameters structure.
 *
 *  This structure contains parameters for the Firmware Update Information Get message,
 *  including the first index and entries limit for requesting firmware information.
 */
typedef struct esp_ble_mesh_device_firmware_update_info_get esp_ble_mesh_device_firmware_update_info_get_t;

/** @brief Device Firmware Update Metadata Check parameters structure.
 *
 *  This structure contains parameters for the Firmware Update Metadata Check message,
 *  including the update firmware index and vendor-specific metadata.
 */
typedef struct esp_ble_mesh_device_firmware_update_metadata_check esp_ble_mesh_device_firmware_update_metadata_check_t;

/** @brief DFU Client get message union.
 *
 *  This union contains different parameter structures for various DFU Client
 *  get messages, allowing type-safe access to message-specific parameters.
 */
typedef union esp_ble_mesh_dfu_client_get esp_ble_mesh_dfu_client_get_t;

/** @brief DFU Client send arguments structure.
 *
 *  This structure contains arguments for sending firmware via DFU Client,
 *  including the client instance, transfer inputs, BLOB stream, and transfer parameters.
 */
typedef struct esp_ble_mesh_dfu_cli_send_arg esp_ble_mesh_dfu_cli_send_arg_t;

/** @brief Firmware information structure.
 *
 *  This structure contains information about firmware including firmware ID
 *  and update URI for retrieving new firmware.
 */
typedef struct esp_ble_mesh_firmware_info esp_ble_mesh_firmware_info_t;

/** @brief Device Firmware Update Info Status response structure.
 *
 *  This structure contains the response parameters for Firmware Update Information
 *  Status message, including firmware information list count and details.
 */
typedef struct esp_ble_mesh_device_firmware_update_info_status esp_ble_mesh_device_firmware_update_info_status_t;

/** @brief Device Firmware Update Metadata Check Status response structure.
 *
 *  This structure contains the response parameters for Firmware Update Metadata
 *  Check Status message, including status code and additional information.
 */
typedef struct esp_ble_mesh_device_firmware_metadata_check_status esp_ble_mesh_device_firmware_metadata_check_status_t;

/** @brief Device Firmware Update Status response structure.
 *
 *  This structure contains the response parameters for various Firmware Update
 *  Status messages (Get, Start, Cancel, Apply), including status, phase,
 *  TTL, timeout base, BLOB ID, and firmware image index.
 */
typedef struct esp_ble_mesh_device_firmware_update_status esp_ble_mesh_device_firmware_update_status_t;

/** @brief DFU Client send callback structure.
 *
 *  This structure contains the result of sending DFU Client messages,
 *  including error code indicating success or failure.
 */
typedef struct esp_ble_mesh_dfu_client_send_cb esp_ble_mesh_dfu_client_send_cb_t;

/** @brief DFU Client receive callback union.
 *
 *  This union contains different response structures for various DFU Client
 *  received status messages, allowing type-safe access to message-specific responses.
 */
typedef union esp_ble_mesh_dfu_client_recv_cb esp_ble_mesh_dfu_client_recv_cb_t;

/** @brief DFU Client callback parameters structure.
 *
 *  This structure contains parameters for DFU Client callbacks, including
 *  common client parameters and a union of send/receive callback data.
 */
typedef struct esp_ble_mesh_dfu_client_cb_param esp_ble_mesh_dfu_client_cb_param_t;
#endif /* CONFIG_BLE_MESH_DFU_CLI */

#if CONFIG_BLE_MESH_DFU_SRV
/** @brief Firmware Update Server instance structure.
 *
 *  This structure represents a DFU (Device Firmware Update) Server model instance.
 *  It contains the necessary fields for managing DFU operations as a server.
 *  Should be initialized with ESP_BLE_MESH_DFU_SRV_INIT.
 *
 *  @note Preview version, the contents of this struct may change in the future.
 */
typedef struct esp_ble_mesh_dfu_srv esp_ble_mesh_dfu_srv_t;

/** @brief Firmware Update Server callback structure.
 *
 *  This structure contains callback function pointers for DFU Server events
 *  such as transfer check, start, end, recovery, and apply.
 */
typedef struct esp_ble_mesh_dfu_srv_cb esp_ble_mesh_dfu_srv_cb_t;
#endif /* CONFIG_BLE_MESH_DFU_SRV */

/** DFU transfer phase. */
typedef enum esp_ble_mesh_dfu_phase {
    /** Ready to start a Receive Firmware procedure. */
    ESP_BLE_MESH_DFU_PHASE_IDLE,

    /** The Transfer BLOB procedure failed. */
    ESP_BLE_MESH_DFU_PHASE_TRANSFER_ERR,

    /** The Receive Firmware procedure is being executed.  */
    ESP_BLE_MESH_DFU_PHASE_TRANSFER_ACTIVE,

    /** The Verify Firmware procedure is being executed. */
    ESP_BLE_MESH_DFU_PHASE_VERIFY,

    /** The Verify Firmware procedure completed successfully. */
    ESP_BLE_MESH_DFU_PHASE_VERIFY_OK,

    /** The Verify Firmware procedure failed. */
    ESP_BLE_MESH_DFU_PHASE_VERIFY_FAIL,

    /** The Apply New Firmware procedure is being executed. */
    ESP_BLE_MESH_DFU_PHASE_APPLYING,

    /** Firmware transfer has been canceled. */
    ESP_BLE_MESH_DFU_PHASE_TRANSFER_CANCELED,

    /** Firmware applying succeeded. */
    ESP_BLE_MESH_DFU_PHASE_APPLY_SUCCESS,

    /** Firmware applying failed. */
    ESP_BLE_MESH_DFU_PHASE_APPLY_FAIL,

    /** Phase of a node was not yet retrieved. */
    ESP_BLE_MESH_DFU_PHASE_UNKNOWN,
} esp_ble_mesh_dfu_phase_t;

/** Expected effect of a DFU transfer. */
typedef enum esp_ble_mesh_dfu_effect {
    /** No changes to node Composition Data. */
    ESP_BLE_MESH_DFU_EFFECT_NONE,

    /** Node Composition Data changed and the node does not support remote
     *  provisioning.
     */
    ESP_BLE_MESH_DFU_EFFECT_COMP_CHANGE_NO_RPR,

    /** Node Composition Data changed, and remote provisioning is supported.
     *  The node supports remote provisioning and Composition Data Page
     *  0x80. Page 0x80 contains different Composition Data than Page 0x0.
     */
    ESP_BLE_MESH_DFU_EFFECT_COMP_CHANGE,

    /** Node will be unprovisioned after the update. */
    ESP_BLE_MESH_DFU_EFFECT_UNPROV,
} esp_ble_mesh_dfu_effect_t;

/** DFU status. */
typedef enum esp_ble_mesh_dfu_status {
    /** The message was processed successfully. */
    ESP_BLE_MESH_DFU_SUCCESS,

    /** Insufficient resources on the node */
    ESP_BLE_MESH_DFU_ERR_RESOURCES,

    /** The operation cannot be performed while the Server is in the current
     *  phase.
     */
    ESP_BLE_MESH_DFU_ERR_WRONG_PHASE,

    /** An internal error occurred on the node. */
    ESP_BLE_MESH_DFU_ERR_INTERNAL,

    /** The message contains a firmware index value that is not expected. */
    ESP_BLE_MESH_DFU_ERR_FW_IDX,

    /** The metadata check failed. */
    ESP_BLE_MESH_DFU_ERR_METADATA,

    /** The Server cannot start a firmware update. */
    ESP_BLE_MESH_DFU_ERR_TEMPORARILY_UNAVAILABLE,

    /** Another BLOB transfer is in progress. */
    ESP_BLE_MESH_DFU_ERR_BLOB_XFER_BUSY,
} esp_ble_mesh_dfu_status_t;

#if CONFIG_BLE_MESH_DFU_CLI
/** Action for DFU iteration callbacks. */
enum esp_ble_mesh_dfu_iter {
    /** Stop iterating. */
    ESP_BLE_MESH_DFU_ITER_STOP,

    /** Continue iterating. */
    ESP_BLE_MESH_DFU_ITER_CONTINUE,
};

/** @brief DFU transfer states
 *
 *  This enumeration defines the various states of a DFU (Device Firmware Update) transfer.
 *  These states track the progress of firmware distribution from idle state through
 *  transfer, verification, application, and confirmation phases.
 */
typedef enum esp_ble_mesh_dfu_xfer_state {
    /** Idle state - No DFU transfer is in progress. */
    ESP_BLE_MESH_DFU_XFER_IDLE,

    /** Transfer state - Firmware data is being transferred to target nodes. */
    ESP_BLE_MESH_DFU_XFER_TRANSFER,

    /** Refresh state - Transfer is paused or being refreshed. */
    ESP_BLE_MESH_DFU_XFER_REFRESH,

    /** Verified state - Firmware has been successfully verified on target nodes. */
    ESP_BLE_MESH_DFU_XFER_VERIFIED,

    /** Apply state - Firmware is being applied on target nodes. */
    ESP_BLE_MESH_DFU_XFER_APPLY,

    /** Applied state - Firmware has been successfully applied on target nodes. */
    ESP_BLE_MESH_DFU_XFER_APPLIED,

    /** Confirm state - Waiting for confirmation from target nodes. */
    ESP_BLE_MESH_DFU_XFER_CONFIRM,

    /** Cancel state - DFU transfer has been cancelled. */
    ESP_BLE_MESH_DFU_XFER_CANCEL,

    /** Suspended state - DFU transfer has been suspended. */
    ESP_BLE_MESH_DFU_XFER_SUSPENDED,
} esp_ble_mesh_dfu_xfer_state_t;

/** @brief Callback events of Device Firmware Update Client Model
 *
 *  This enumeration defines the various callback events that can be generated
 *  by the DFU (Device Firmware Update) Client model. These events are used
 *  to notify the application about different stages and states of the DFU process,
 *  including message sending, timeouts, response reception, and image transfer completion.
 */
typedef enum {
    /** DFU Client message sending completed event.
     *  This event is triggered when a DFU Client message has been successfully sent.
     */
    ESP_BLE_MESH_DFU_CLIENT_SEND_COMP_EVT,

    /** DFU Client timeout event.
     *  This event is triggered when a DFU Client operation times out waiting for a response.
     */
    ESP_BLE_MESH_DFU_CLIENT_TIMEOUT_EVT,

    /** DFU Client received Get response event.
     *  This event is triggered when the DFU Client receives a response to a Get message.
     */
    ESP_BLE_MESH_DFU_CLIENT_RECV_GET_RSP_EVT,

    /** DFU Client image send completed event.
     *  This event is triggered when the DFU Client has completed sending a firmware image.
     */
    ESP_BLE_MESH_DFU_CLIENT_IMG_SEND_CMP_EVT,

    /** Maximum DFU Client event value (used for bounds checking). */
    ESP_BLE_MESH_DFU_CLIENT_EVT_MAX,
} esp_ble_mesh_dfu_client_cb_event_t;

#endif /* CONFIG_BLE_MESH_DFU_CLI */

/** DFU image instance.
 *
 *  Each DFU image represents a single updatable firmware image.
 */
struct esp_ble_mesh_dfu_img {
    /** Firmware ID. */
    const void *fwid;

    /** Length of the firmware ID. */
    size_t fwid_len;

    /** Update URI, or NULL. */
    const char *uri;
};

#if CONFIG_BLE_MESH_DFU_CLI
/** DFU image slot for DFU distribution. */
struct esp_ble_mesh_dfu_slot {
    /** Size of the firmware in bytes. */
    size_t size;
    /** Length of the firmware ID. */
    size_t fwid_len;
    /** Length of the metadata. */
    size_t metadata_len;
    /** Firmware ID. */
    uint8_t fwid[CONFIG_BLE_MESH_DFU_FWID_MAXLEN];
    /** Metadata. */
    uint8_t metadata[CONFIG_BLE_MESH_DFU_METADATA_MAXLEN];
};

/** DFU Target node. */
struct esp_ble_mesh_dfu_target {
    /** BLOB Target node */
    esp_ble_mesh_blob_target_t blob;
    /** Image index on the Target node */
    uint8_t img_idx;
    /** Expected DFU effect, see bt_mesh_dfu_effect. */
    uint8_t effect;
    /** Current DFU status, see bt_mesh_dfu_status. */
    uint8_t status;
    /** Current DFU phase, see bt_mesh_dfu_phase. */
    uint8_t phase;
};

/** Firmware Update Client event callbacks. */
struct esp_ble_mesh_dfu_cli_cb {
    /** @brief BLOB transfer is suspended.
     *
     * Called when the BLOB transfer is suspended due to response timeout from all Target nodes.
     *
     * @param cli    Firmware Update Client model instance.
     */
    void (*suspended)(esp_ble_mesh_dfu_cli_t *cli);

    /** @brief DFU ended.
     *
     *  Called when the DFU transfer ends, either because all Target nodes were
     *  lost or because the transfer was completed successfully.
     *
     *  @param cli    Firmware Update Client model instance.
     *  @param reason Reason for ending.
     */
    void (*ended)(esp_ble_mesh_dfu_cli_t *cli,
                  esp_ble_mesh_dfu_status_t reason);

    /** @brief DFU transfer applied on all active Target nodes.
     *
     *  Called at the end of the apply procedure.
     *
     *  @param cli Firmware Update Client model instance.
     */
    void (*applied)(esp_ble_mesh_dfu_cli_t *cli);

    /** @brief DFU transfer confirmed on all active Target nodes.
     *
     *  Called at the end of the apply procedure.
     *
     *  @param cli Firmware Update Client model instance.
     */
    void (*confirmed)(esp_ble_mesh_dfu_cli_t *cli);

    /** @brief DFU Target node was lost.
     *
     *  A DFU Target node was dropped from the receivers list. The Target node's
     *  @c status is set to reflect the reason for the failure.
     *
     *  @param cli    Firmware Update Client model instance.
     *  @param target DFU Target node that was lost.
     */
    void (*lost_target)(esp_ble_mesh_dfu_cli_t *cli,
                        esp_ble_mesh_dfu_target_t *target);
};

/** Firmware Update Client model instance.
 *  Should be initialized with ESP_BLE_MESH_DFU_CLI_INIT.
 *
 *  @note Preview version, the contents of this struct may change in the future.
 */
struct esp_ble_mesh_dfu_cli {
    /** Callback structure. */
    const esp_ble_mesh_dfu_cli_cb_t *cb;
    /** Underlying BLOB Transfer Client. */
    esp_ble_mesh_blob_cli_t blob;

    /** runtime state */
    uint32_t op;

    /** ble mesh model instance. */
    esp_ble_mesh_model_t *mod;

    /** BLOB Transfer information */
    struct {
        const esp_ble_mesh_dfu_slot_t *slot;
        const esp_ble_mesh_blob_io_t *io;
        esp_ble_mesh_blob_xfer_t blob;
        uint8_t state;
        uint8_t flags;
    } xfer;

    /** Current request */
    void *req;
};

/** BLOB parameters for Firmware Update Client transfer */
struct esp_ble_mesh_dfu_cli_xfer_blob_params {
    /** Logarithmic representation of the block size. */
    uint8_t block_size_log;
    /** Base chunk size. May be smaller for the last chunk. */
    uint16_t chunk_size;
};

/** Firmware Update Client transfer parameters */
struct esp_ble_mesh_dfu_cli_xfer {
    /** BLOB ID to use for this transfer, or 0 to set it randomly. */
    uint64_t blob_id;
    /** DFU image slot to transfer. */
    const esp_ble_mesh_dfu_slot_t *slot;
    /**  Transfer mode (Push (Push BLOB Transfer Mode) or Pull (Pull BLOB Transfer Mode)) */
    enum esp_ble_mesh_blob_xfer_mode mode;
    /** BLOB parameters to be used for the transfer, or NULL to retrieve Target nodes'
     *  capabilities before sending a firmware.
     */
    const esp_ble_mesh_dfu_cli_xfer_blob_params_t *blob_params;
};

/**
 * @brief Parameters of Device Firmware Update Info Get.
 */
struct esp_ble_mesh_device_firmware_update_info_get {
    uint8_t first_index;                                                                /*!< Index of the first requested entry from the Firmware Information List state */
    uint8_t entries_limit;                                                              /*!< Maximum number of entries that the server includes in a Firmware Update Information Status message */
    esp_ble_mesh_dfu_img_cb_t img_cb;                                                   /*!< Added by Espressif, callback function used to walkthrough recceived images */
};

/**
 * @brief Parameters of Device Firmware Update Firmware Metadata Check.
 */
struct esp_ble_mesh_device_firmware_update_metadata_check {
    uint8_t update_firmware_index;                                                      /*!< Index of the firmware image in the Firmware Information List state to check  */
    struct net_buf_simple *metadata;                                                    /*!< Vendor-specific metadata */
};

/**
 * @brief Device Firmware Update Client model get message union.
 *        For ESP_BLE_MESH_DFU_OP_UPDATE_INFO_GET
 *            ESP_BLE_MESH_DFU_OP_UPDATE_METADATA_CHECK
 *            ESP_BLE_MESH_DFU_OP_UPDATE_START
 * the get_state parameter in the `esp_ble_mesh_dfu_client_get_state` function should not be set to NULL.
 */
union esp_ble_mesh_dfu_client_get {
    esp_ble_mesh_device_firmware_update_info_get_t              dfu_update_info_get;    /*!< For ESP_BLE_MESH_DFU_OP_UPDATE_INFO_GET */
    esp_ble_mesh_device_firmware_update_metadata_check_t        dfu_metadata_check;     /*!< For ESP_BLE_MESH_DFU_OP_UPDATE_METADATA_CHECK */
};

/**
 * @brief Parameters of Device Firmware Update Client
 * model to send Firmware.
 */
struct esp_ble_mesh_dfu_cli_send_arg {
    esp_ble_mesh_dfu_cli_t *cli;               /*!< Device Firmware Update Client model instance */
    esp_ble_mesh_blob_cli_inputs_t *inputs;    /*!< BLOB Transfer Client transfer inputs. */
    esp_ble_mesh_blob_io_t *io;                /*!< BLOB stream. */
    esp_ble_mesh_dfu_cli_xfer_t *xfer;         /*!< Firmware Update Client transfer parameters */
};

/**
 * @brief Struct of firmware information.
 */
struct esp_ble_mesh_firmware_info {
    struct net_buf_simple *fw_id;       /*!< Firmware ID */
    struct net_buf_simple *update_uri;  /*!< URI used to retrieve a new firmware */
};

/**
 * @brief Response of Firmware Update Information Get Message
 */
struct esp_ble_mesh_device_firmware_update_info_status {
    uint8_t fw_info_list_cnt;                           /*!< The number of entries in the Firmware Information List */
    uint8_t first_index;                                /*!< Index of the first requested entry from the Firmware Information List */
    esp_ble_mesh_firmware_info_t *fw_info_list;         /*!< List of firmware information entries */
};

/**
 * @brief Response of Firmware Update Firmware Metadata Check Message
 */
struct esp_ble_mesh_device_firmware_metadata_check_status {
    uint8_t status : 3;                                 /*!< Status Code from the firmware metadata check */
    uint8_t additional_info : 5;                        /*!< The Firmware Update Additional Information state from the Firmware Update Server (see Section 4.1.3)*/
    uint8_t update_firmware_image_index;                /*!< Index of the firmware image in the Firmware Information List state that was checked*/
};

/**
 * @brief Response of Firmware Update Get Message,
 *                    Firmware Update Start Message,
 *                    Firmware Update Cancel Message,
 *                    Firmware Update Apply Message.
 */
struct esp_ble_mesh_device_firmware_update_status {
    uint8_t status       : 3;                           /*!< Status Code for the requesting message */
    uint8_t update_phase : 3;                           /*!< The Update Phase state of the Firmware Update Server */
    uint8_t update_ttl;                                 /*!< TTL value to use during firmware image transfer */
    uint8_t additional_info : 5;                        /*!< The Firmware Update Additional Information state from the Firmware Update Server (see Section 4.1.3)*/
    uint16_t update_timeout_base;                       /*!< Used to compute the timeout of the firmware image transfer */
    uint64_t update_blob_id;                            /*!< BLOB identifier for the firmware image */
    uint8_t update_fw_img_idx;                          /*!< The index of the firmware image in the Firmware Information List state being updated*/
};

/** Result of sending Device Firmware Update Client messages */
struct esp_ble_mesh_dfu_client_send_cb {
    int err_code;                                       /*!< Result of sending a message */
};

/**
 * @brief Device Firmware Update Client model received message union
 */
union esp_ble_mesh_dfu_client_recv_cb {
    esp_ble_mesh_device_firmware_update_info_status_t              dfu_update_info_status;              /*!< ESP_BLE_MESH_MODEL_OP_DIRECTED_CONTROL_STATUS */
    esp_ble_mesh_device_firmware_metadata_check_status_t           dfu_metadata_check_status;           /*!< ESP_BLE_MESH_MODEL_OP_PATH_METRIC_STATUS */
    esp_ble_mesh_device_firmware_update_status_t                   dfu_firmware_update_status;          /*!< ESP_BLE_MESH_MODEL_OP_DISCOVERY_TABLE_CAPS_STATUS */
};

/** Device Firmware Update Client model callback parameters */
struct esp_ble_mesh_dfu_client_cb_param {
    esp_ble_mesh_client_common_param_t *params;         /*!< Client common parameters, used by all events. */
    /** Union of DF Client callback */
    union {
        esp_ble_mesh_dfu_client_send_cb_t send;          /*!< Result of sending a message */
        esp_ble_mesh_dfu_client_recv_cb_t recv;          /*!< Parameters of received status message */
    };
};

/**
 *  @brief Bluetooth Mesh Device Firmware Update client and server model functions.
 */

/**
 * @brief   Device Firmware Update Client model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* esp_ble_mesh_dfu_client_cb_t)(esp_ble_mesh_dfu_client_cb_event_t event,
                                              esp_ble_mesh_dfu_client_cb_param_t *param);

/**
 * @brief   Device Firmware Update Server model callback function type
 * @param   event: Event type
 * @param   param: Pointer to callback parameter
 */
typedef void (* esp_ble_mesh_dfu_server_cb_t)(esp_ble_mesh_dfu_client_cb_event_t event,
                                              esp_ble_mesh_dfu_client_cb_param_t *param);

/**
 * @brief       Register BLE Mesh Device Firmware Update Client model callback.
 *
 * @param   callback: Pointer to the callback function.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_dfu_client_callback(esp_ble_mesh_dfu_client_cb_t callback);

/**
 * @brief       Get the value of Device Firmware Server model state with the corresponding get message.
 *
 * @param   params: Pointer to BLE Mesh common client parameters.
 * @param   get:    Pointer to a union, each kind of opcode corresponds to one structure inside.
 *
 * @return      ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_dfu_client_get_state(esp_ble_mesh_client_common_param_t *params,
                                            esp_ble_mesh_dfu_client_get_t *get);

/** @brief Start distributing a DFU.
 *
 *  Starts distribution of the firmware in the given slot to the list of DFU
 *  Target nodes in @c ctx. The transfer runs in the background, and its end is
 *  signalled through the bt_mesh_dfu_cli_cb::ended callback.
 *
 *  @note The BLOB Transfer Client transfer inputs @c targets list must point to a list of
 *  bt_mesh_dfu_target nodes. The contents of `inputs`, `io`, `xfer` must not be changed or
 *  released from the beginning to the end of the transmission.
 *
 *  @param cli    Firmware Update Client model instance.
 *  @param inputs BLOB Transfer Client transfer inputs.
 *  @param io     BLOB stream to read BLOB from.
 *  @param xfer   Firmware Update Client transfer parameters.
 *
 *  @return 0 on success, or (negative) error code otherwise.
 */
esp_err_t esp_ble_mesh_dfu_cli_img_send(esp_ble_mesh_dfu_cli_t *cli,
                                        esp_ble_mesh_blob_cli_inputs_t *inputs,
                                        esp_ble_mesh_blob_io_t *io,
                                        esp_ble_mesh_dfu_cli_xfer_t *xfer);

/** @brief Get progress as a percentage of completion.
 *
 *  @param cli Firmware Update Client model instance.
 *
 *  @return The progress of the current transfer in percent, or 0 if no
 *          transfer is active.
 */
uint8_t esp_ble_mesh_dfu_cli_progress(esp_ble_mesh_dfu_cli_t *cli);
#endif /* CONFIG_BLE_MESH_DFU_CLI */

#if CONFIG_BLE_MESH_DFU_SRV
/** @brief Firmware Update Server event callbacks. */
struct esp_ble_mesh_dfu_srv_cb {
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
    int (*check)(esp_ble_mesh_dfu_srv_t *srv,
                 const esp_ble_mesh_dfu_img_t *img,
                 struct net_buf_simple *metadata,
                 esp_ble_mesh_dfu_effect_t *effect);

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
    int (*start)(esp_ble_mesh_dfu_srv_t *srv,
                 const esp_ble_mesh_dfu_img_t *img,
                 struct net_buf_simple *metadata,
                 const esp_ble_mesh_blob_io_t **io);

    /** @brief Transfer end callback.
     *
     *  This handler is optional.
     *
     *  If the transfer is successful, the application should verify the
     *  firmware image, and call either bt_mesh_dfu_srv_verified or
     *  bt_mesh_dfu_srv_rejected depending on the outcome.
     *
     *  If the transfer fails, the Firmware Update Server will be available for new
     *  transfers immediately after this function returns.
     *
     *  @param srv     Firmware Update Server instance.
     *  @param img     DFU image that failed the update.
     *  @param success Whether the DFU transfer was successful.
     */
    void (*end)(esp_ble_mesh_dfu_srv_t *srv,
                const esp_ble_mesh_dfu_img_t *img, bool success);

    /** @brief Transfer recovery callback.
     *
     *  If the device reboots in the middle of a transfer, the Firmware Update Server
     *  calls this function when the Bluetooth mesh subsystem is started.
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
    int (*recover)(esp_ble_mesh_dfu_srv_t *srv,
                   const esp_ble_mesh_dfu_img_t *img,
                   const esp_ble_mesh_blob_io_t **io);

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
    int (*apply)(esp_ble_mesh_dfu_srv_t *srv,
                 const esp_ble_mesh_dfu_img_t *img);
};

/** @brief Firmware Update Server instance.
 *
 *  Should be initialized with ESP_BLE_MESH_DFU_SRV_INIT.
 *  @note Preview version, the contents of this struct may change in the future.
 */
struct esp_ble_mesh_dfu_srv {
    /** Underlying BLOB Transfer Server. */
    esp_ble_mesh_blob_srv_t blob;
    /** Callback structure. */
    const esp_ble_mesh_dfu_srv_cb_t *cb;
    /** List of updatable images. */
    const esp_ble_mesh_dfu_img_t *imgs;
    /** Number of updatable images. */
    size_t img_count;

    /** Runtime state */
    esp_ble_mesh_model_t *mod;
    /** Update information */
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
void esp_ble_mesh_dfu_srv_verified(esp_ble_mesh_dfu_srv_t *srv);

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
void esp_ble_mesh_dfu_srv_rejected(esp_ble_mesh_dfu_srv_t *srv);

/** @brief Cancel the ongoing DFU transfer.
 *
 *  @param srv Firmware Update Server instance.
 */
void esp_ble_mesh_dfu_srv_cancel(esp_ble_mesh_dfu_srv_t *srv);

/** @brief Confirm that the received DFU transfer was applied.
 *
 *  Should be called as a result of the `bt_mesh_dfu_srv_cb.apply` callback.
 *
 *  @param srv Firmware Update Server instance.
 */
void esp_ble_mesh_dfu_srv_applied(esp_ble_mesh_dfu_srv_t *srv);

/** @brief Check if the Firmware Update Server is busy processing a transfer.
 *
 *  @param srv Firmware Update Server instance.
 *
 *  @return true if a DFU procedure is in progress, false otherwise.
 */
bool esp_ble_mesh_dfu_srv_is_busy(const esp_ble_mesh_dfu_srv_t *srv);

/** @brief Get the progress of the current DFU procedure, in percent.
 *
 *  @param srv Firmware Update Server instance.
 *
 *  @return The current transfer progress in percent.
 */
uint8_t esp_ble_mesh_dfu_srv_progress(const esp_ble_mesh_dfu_srv_t *srv);
#endif /* CONFIG_BLE_MESH_DFU_SRV */

#ifdef __cplusplus
}
#endif

#endif /* _ESP_BLE_MESH_DFU_MODEL_API_H_ */
