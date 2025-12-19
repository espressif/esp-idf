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

/* Distribution Receivers Management Operations (Section 4.3.1) */
#define ESP_BLE_MESH_DFD_OP_RECEIVERS_ADD               ESP_BLE_MESH_MODEL_OP_2(0x83, 0x11) /*!< Add receivers to the distribution list. */
#define ESP_BLE_MESH_DFD_OP_RECEIVERS_DELETE_ALL        ESP_BLE_MESH_MODEL_OP_2(0x83, 0x12) /*!< Delete all receivers from the distribution list. */
#define ESP_BLE_MESH_DFD_OP_RECEIVERS_STATUS            ESP_BLE_MESH_MODEL_OP_2(0x83, 0x13) /*!< Status of receivers add/delete operations. */
#define ESP_BLE_MESH_DFD_OP_RECEIVERS_GET               ESP_BLE_MESH_MODEL_OP_2(0x83, 0x14) /*!< Get the distribution receivers list. */
#define ESP_BLE_MESH_DFD_OP_RECEIVERS_LIST              ESP_BLE_MESH_MODEL_OP_2(0x83, 0x15) /*!< List of distribution receivers. */

/* Distribution Capabilities Operations (Section 4.3.2) */
#define ESP_BLE_MESH_DFD_OP_CAPABILITIES_GET            ESP_BLE_MESH_MODEL_OP_2(0x83, 0x16) /*!< Get distribution capabilities of the node. */
#define ESP_BLE_MESH_DFD_OP_CAPABILITIES_STATUS         ESP_BLE_MESH_MODEL_OP_2(0x83, 0x17) /*!< Distribution capabilities status. */

/* Firmware Distribution Control Operations (Section 4.3.3) */
#define ESP_BLE_MESH_DFD_OP_GET                         ESP_BLE_MESH_MODEL_OP_2(0x83, 0x18) /*!< Get current firmware distribution status. */
#define ESP_BLE_MESH_DFD_OP_START                       ESP_BLE_MESH_MODEL_OP_2(0x83, 0x19) /*!< Start firmware distribution to receivers. */
#define ESP_BLE_MESH_DFD_OP_SUSPEND                     ESP_BLE_MESH_MODEL_OP_2(0x83, 0x1a) /*!< Suspend ongoing firmware distribution. */
#define ESP_BLE_MESH_DFD_OP_CANCEL                      ESP_BLE_MESH_MODEL_OP_2(0x83, 0x1b) /*!< Cancel firmware distribution. */
#define ESP_BLE_MESH_DFD_OP_APPLY                       ESP_BLE_MESH_MODEL_OP_2(0x83, 0x1c) /*!< Apply distributed firmware on target nodes. */
#define ESP_BLE_MESH_DFD_OP_STATUS                      ESP_BLE_MESH_MODEL_OP_2(0x83, 0x1d) /*!< Firmware distribution status. */

/* Firmware Upload Management Operations (Section 4.3.4) */
#define ESP_BLE_MESH_DFD_OP_UPLOAD_GET                  ESP_BLE_MESH_MODEL_OP_2(0x83, 0x1e) /*!< Get firmware upload status and progress. */
#define ESP_BLE_MESH_DFD_OP_UPLOAD_START                ESP_BLE_MESH_MODEL_OP_2(0x83, 0x1f) /*!< Start firmware upload (in-band transfer). */
#define ESP_BLE_MESH_DFD_OP_UPLOAD_START_OOB            ESP_BLE_MESH_MODEL_OP_2(0x83, 0x20) /*!< Start firmware upload (out-of-band transfer). */
#define ESP_BLE_MESH_DFD_OP_UPLOAD_CANCEL               ESP_BLE_MESH_MODEL_OP_2(0x83, 0x21) /*!< Cancel firmware upload. */
#define ESP_BLE_MESH_DFD_OP_UPLOAD_STATUS               ESP_BLE_MESH_MODEL_OP_2(0x83, 0x22) /*!< Firmware upload status and progress. */

/* Firmware Image Management Operations (Section 4.3.5) */
#define ESP_BLE_MESH_DFD_OP_FW_GET                      ESP_BLE_MESH_MODEL_OP_2(0x83, 0x23) /*!< Get firmware image information by FW ID. */
#define ESP_BLE_MESH_DFD_OP_FW_GET_BY_INDEX             ESP_BLE_MESH_MODEL_OP_2(0x83, 0x24) /*!< Get firmware image information by index. */
#define ESP_BLE_MESH_DFD_OP_FW_DELETE                   ESP_BLE_MESH_MODEL_OP_2(0x83, 0x25) /*!< Delete specific firmware image. */
#define ESP_BLE_MESH_DFD_OP_FW_DELETE_ALL               ESP_BLE_MESH_MODEL_OP_2(0x83, 0x26) /*!< Delete all firmware images. */
#define ESP_BLE_MESH_DFD_OP_FW_STATUS                   ESP_BLE_MESH_MODEL_OP_2(0x83, 0x27) /*!< Firmware image management status. */

/* Device Firmware Distribution Constants and Status Codes */
#define ESP_BLE_MESH_DFD_UPLOAD_PROGRESS_UNSET         101   /*!< Upload progress is not set/available. */
#define ESP_BLE_MESH_DFD_UPLOAD_TYPE_INBAND            0     /*!< In-band firmware upload type. */
#define ESP_BLE_MESH_DFD_UPLOAD_TYPE_OOB               BIT0  /*!< Out-of-band firmware upload type. */

#if CONFIG_BLE_MESH_DFU_CLI
/**
 * @brief Define a new DFU Client model.
 *
 * @note This API needs to be called for each element on which
 *       the application needs to have a DFU Client model.
 *
 * @param cli_pub  Pointer to the unique `esp_ble_mesh_model_pub_t`.
 * @param cli_data Pointer to the unique `esp_ble_mesh_client_t`.
 *
 * @return New DFU Client model instance.
 */
#define ESP_BLE_MESH_MODEL_DFU_CLI(cli_pub, cli_data)         \
        ESP_BLE_MESH_MODEL_BLOB_CLI(NULL, &(cli_data)->blob), \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_DFU_CLI, \
                               NULL, cli_pub, cli_data)


/**
 * @brief Initialize a DFU Client model instance.
 *
 * @note This macro initializes the DFU Client model and sets callback handlers.
 *
 * @param _handlers Pointer to the callback handler structure `esp_ble_mesh_dfu_cli_cb_t`.
 *
 * @return An initialized DFU Client model instance.
 */
#define ESP_BLE_MESH_DFU_CLI_INIT(_handlers)                           \
{                                                                  \
    .cb = _handlers,                                               \
}
#endif /* CONFIG_BLE_MESH_DFU_CLI */

#if CONFIG_BLE_MESH_DFU_SRV
/**
 * @brief Define a new DFU Server model.
 *
 * @note This API needs to be called for each element on which
 *       the application needs to have a DFU Server model.
 *
 * @param srv_pub  Pointer to the unique `esp_ble_mesh_model_pub_t`.
 * @param srv_data Pointer to the unique `esp_ble_mesh_dfu_trans_srv_t`.
 *
 * @return New DFU Server model instance.
 */
#define ESP_BLE_MESH_MODEL_DFU_SRV(srv_pub, srv_data)         \
        ESP_BLE_MESH_MODEL_BLOB_SRV(NULL, &(srv_data)->blob), \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_DFU_SRV, \
                               NULL, srv_pub, srv_data)


/**
 * @brief Initialize a DFU Server model instance.
 *
 * @note This macro initializes the DFU Server model and sets callback handlers
 *       and image information.
 *
 * @param _handlers  Pointer to the callback handler structure `esp_ble_mesh_dfu_srv_cb_t`.
 * @param _imgs      Pointer to the array of DFU images `esp_ble_mesh_dfu_img_t`.
 * @param _img_count Number of DFU images in the array.
 *
 * @return An initialized DFU Server model instance.
 */
#define ESP_BLE_MESH_DFU_SRV_INIT(_handlers, _imgs, _img_count)            \
    {                                                                      \
        .cb = _handlers,                                                   \
        .imgs = _imgs, .img_count = _img_count,                            \
    }
#endif /* CONFIG_BLE_MESH_DFU_SRV */

#if CONFIG_BLE_MESH_DFD_CLI
/**
 * @def ESP_BLE_MESH_MODEL_DFD_CLI
 *
 * @brief Define a new Device Firmware Distribution Client model.
 *
 * Define a DFD Client model for managing firmware distribution operations,
 * including receiver lists, upload control, progress monitoring, and
 * firmware transfers.
 *
 * @param cli_pub  Pointer to the unique `esp_ble_mesh_model_pub_t` for publish messages.
 * @param cli_data Pointer to the unique `esp_ble_mesh_client_t` containing client data.
 *
 * @return New DFD Client model instance.
 */
#define ESP_BLE_MESH_MODEL_DFD_CLI(cli_pub, cli_data)         \
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_DFD_CLI, \
                               NULL, cli_pub, cli_data)
#endif /* CONFIG_BLE_MESH_DFD_CLI */

#if CONFIG_BLE_MESH_DFD_SRV
/**
 * @def ESP_BLE_MESH_MODEL_DFD_SRV
 *
 * @brief Define a new Device Firmware Distribution Server model.
 *
 * Define a DFD Server model that stores firmware, distributes to targets,
 * handles uploads, manages receiver lists, and controls distribution phases.
 * Contains internal DFU Client for updates and BLOB Server for transfers.
 *
 * @param srv_pub  Pointer to the unique `esp_ble_mesh_model_pub_t` for publish messages.
 * @param srv_data Pointer to the unique `esp_ble_dfd_trans_srv_t` containing server data.
 *
 * @return New DFD Server model instance.
 */
#define ESP_BLE_MESH_MODEL_DFD_SRV(srv_pub, srv_data)                 \
        ESP_BLE_MESH_MODEL_DFU_CLI(NULL, &(srv_data)->dfu),           \
        ESP_BLE_MESH_MODEL_BLOB_SRV(NULL, &((srv_data)->upload.blob)),\
        ESP_BLE_MESH_SIG_MODEL(ESP_BLE_MESH_MODEL_ID_DFD_SRV,         \
                               NULL, srv_pub, srv_data)

/* Internal callback structures for DFD Server implementation */
extern const struct bt_mesh_dfu_cli_cb _bt_mesh_dfd_srv_dfu_cb;      /*!< Internal DFU Client callbacks for DFD Server. */
extern const struct bt_mesh_blob_srv_cb _bt_mesh_dfd_srv_blob_cb;    /*!< Internal BLOB Server callbacks for DFD Server. */

/**
 * @def ESP_BLE_MESH_DFD_SRV_INIT
 *
 * @brief Initialize a Device Firmware Distribution Server.
 *
 * Initialize a DFD Server instance with the provided callback structure
 * and set up the internal DFU Client and BLOB Server components.
 *
 * @param _cb Pointer to a `bt_mesh_dfd_srv_cb` instance containing the server callbacks.
 *
 * @return Initialized DFD Server structure with internal components configured.
 */
#define ESP_BLE_MESH_DFD_SRV_INIT(_cb)                                               \
    {                                                                                \
        .cb = _cb,                                                                   \
        .dfu = ESP_BLE_MESH_DFU_CLI_INIT((struct esp_ble_mesh_dfu_cli_cb *)          \
                                         &_bt_mesh_dfd_srv_dfu_cb),                  \
        .upload = {                                                                  \
            .blob = { .cb = (esp_ble_mesh_blob_srv_cb_t *)                           \
                            &_bt_mesh_dfd_srv_blob_cb },                             \
        },                                                                           \
    }
#endif /* CONFIG_BLE_MESH_DFD_SRV */

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

#if CONFIG_BLE_MESH_DFD_CLI
/* DFD Client Forward Declarations */
typedef enum esp_ble_mesh_dfd_client_act esp_ble_mesh_dfd_client_act_t;                                /*!< DFD Client action enumeration. */
typedef enum esp_ble_mesh_dfd_client_cb_evt esp_ble_mesh_dfd_client_cb_evt_t;                          /*!< DFD Client callback event enumeration. */
typedef struct esp_ble_mesh_dfd_cli_receiver_entry esp_ble_mesh_dfd_cli_receiver_entry_t;              /*!< DFD Client receiver entry structure. */

/* DFD Client Receiver Management Structures */
typedef struct esp_ble_mesh_dfd_cli_receivers_add esp_ble_mesh_dfd_cli_receivers_add_t;                /*!< Parameters for adding receivers to distribution list. */
typedef struct esp_ble_mesh_dfd_receiver_status esp_ble_mesh_dfd_receiver_status_t;                    /*!< Status response for receiver operations. */

typedef struct esp_ble_mesh_dfd_receivers_get esp_ble_mesh_dfd_receivers_get_t;                        /*!< Parameters for getting distribution receivers list. */
typedef struct esp_ble_mesh_dfd_target_node_entry esp_ble_mesh_dfd_target_node_entry_t;                /*!< Target node entry information. */
typedef struct esp_ble_mesh_dfd_receiver_list esp_ble_mesh_dfd_receiver_list_t;                        /*!< Distribution receivers list response. */

/* DFD Client Distribution Control Structures */
typedef struct esp_ble_mesh_dfd_dist_caps esp_ble_mesh_dfd_dist_caps_t;                                /*!< Distribution capabilities of the DFD Server. */

typedef struct esp_ble_mesh_dfd_cli_dist_start esp_ble_mesh_dfd_cli_dist_start_t;                      /*!< Parameters for starting firmware distribution. */
typedef struct esp_ble_mesh_dfd_dist_status esp_ble_mesh_dfd_dist_status_t;                            /*!< Firmware distribution status response. */

/* DFD Client Upload Management Structures */
typedef struct esp_ble_mesh_dfd_cli_dist_upload_start esp_ble_mesh_dfd_cli_dist_upload_start_t;         /*!< Parameters for starting in-band firmware upload. */
typedef struct esp_ble_mesh_dfd_cli_dist_upload_oob_start esp_ble_mesh_dfd_cli_dist_upload_oob_start_t; /*!< Parameters for starting out-of-band firmware upload. */
typedef struct esp_ble_mesh_dfd_upload_status esp_ble_mesh_dfd_upload_status_t;                         /*!< Firmware upload status and progress. */

/* DFD Client Firmware Management Structures */
typedef struct esp_ble_mesh_dfd_cli_dist_fw_get esp_ble_mesh_dfd_cli_dist_fw_get_t;                     /*!< Parameters for getting firmware by FW ID. */
typedef struct esp_ble_mesh_dfd_cli_dist_fw_get_by_idx esp_ble_mesh_dfd_cli_dist_fw_get_by_idx_t;       /*!< Parameters for getting firmware by index. */
typedef struct esp_ble_mesh_dfd_cli_dist_fw_del esp_ble_mesh_dfd_cli_dist_fw_del_t;                     /*!< Parameters for deleting firmware. */
typedef struct esp_ble_mesh_dfd_firmware_status esp_ble_mesh_dfd_firmware_status_t;                     /*!< Firmware management status response. */

/* DFD Client Callback Structures */
typedef struct esp_ble_mesh_dfd_client_cb_param esp_ble_mesh_dfd_client_cb_param_t;                     /*!< DFD Client callback parameters. */
typedef union esp_ble_mesh_dfd_client_common_cb_param esp_ble_mesh_dfd_client_common_cb_param_t;        /*!< Common callback parameter union. */

/* DFD Client Callback Function Type */
typedef void (*esp_ble_mesh_dfd_client_cb_t)(esp_ble_mesh_dfd_client_cb_evt_t event,                  /*!< DFD Client callback function type. */
                                             esp_ble_mesh_dfd_client_cb_param_t *param);
#endif /* CONFIG_BLE_MESH_DFD_CLI */

#if CONFIG_BLE_MESH_DFD_SRV
/**
 * @brief Firmware Distribution status enumeration.
 *
 * This enumeration defines the status codes used in DFD operations
 * according to the Bluetooth Mesh Model Specification v1.0. These
 * status codes indicate success, failure, and error conditions
 * for various firmware distribution operations.
 */
typedef enum esp_ble_mesh_dfd_status esp_ble_mesh_dfd_status_t;

/**
 * @brief Firmware distribution phases enumeration.
 *
 * This enumeration defines the different phases of a firmware distribution
 * operation as specified in the Bluetooth Mesh Model Specification v1.0.
 * The phases track the state of firmware distribution from idle through
 * transfer, verification, application, and completion states.
 */
typedef enum esp_ble_mesh_dfd_phase esp_ble_mesh_dfd_phase_t;

/**
 * @brief Firmware upload phases enumeration.
 *
 * This enumeration defines the phases of firmware upload operations
 * on the DFD Server. It tracks the state of firmware storage procedures
 * including in-band and out-of-band upload methods.
 */
typedef enum esp_ble_mesh_dfd_upload_phase esp_ble_mesh_dfd_upload_phase_t;

/**
 * @brief Firmware Distribution Server callback structure.
 *
 * This structure contains callback function pointers that allow the
 * application to handle various DFD Server events including firmware
 * reception, distribution control, and phase changes.
 */
typedef struct esp_ble_mesh_dfd_srv_cb esp_ble_mesh_dfd_srv_cb_t;

/**
 * @brief Firmware Distribution Server instance structure.
 *
 * This structure represents a DFD Server model instance that manages
 * firmware distribution operations. It contains state information for
 * ongoing distributions, firmware storage, and target node management.
 */
typedef struct esp_ble_mesh_dfd_srv esp_ble_mesh_dfd_srv_t;
#endif /* CONFIG_BLE_MESH_DFD_SRV */

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
    esp_ble_mesh_dfu_img_cb_t img_cb;                                                   /*!< Added by Espressif, callback function used to walkthrough received images */
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

#if CONFIG_BLE_MESH_DFD_CLI
/**
 * @brief DFD Client receiver entry structure.
 *
 * This structure represents a single receiver node that will be added
 * to the distribution list for firmware updates. Each entry contains
 * the target node's address and the specific firmware image index
 * that should be distributed to that node.
 *
 * @note This is used with the DFD Receivers Add message to build
 *       the distribution receiver list.
 */
struct esp_ble_mesh_dfd_cli_receiver_entry {
    /** The unicast address of the target node to receive firmware updates */
    uint16_t addr;
    /** Index of the firmware image that should be distributed to this target node */
    uint8_t fw_idx;
};

/**
 * @brief DFD Client receivers add parameters.
 *
 * This structure contains the parameters for the DFD Receivers Add
 * message. It is used to add multiple target nodes to the distribution
 * receiver list.
 *
 * @note The receivers array should contain valid unicast addresses
 *       and corresponding firmware image indices.
 */
struct esp_ble_mesh_dfd_cli_receivers_add {
    /** Pointer to array of receiver entries to add to the distribution list */
    esp_ble_mesh_dfd_cli_receiver_entry_t *receivers;
    /** Number of receiver entries in the receivers array */
    uint16_t receivers_cnt;
};

/**
 * @brief DFD receiver status response structure.
 *
 * This structure contains the response parameters for the DFD
 * Receivers Add/Delete operations.
 *
 * The status indicates whether the receiver list modification
 * was successful, and the receiver_list_cnt provides the current
 * size of the distribution receiver list.
 */
struct esp_ble_mesh_dfd_receiver_status {
    /** Status code for the receiver add/delete operation (see esp_ble_mesh_dfd_status) */
    uint8_t status;
    /** Current number of entries in the Distribution Receivers List state */
    uint16_t receiver_list_cnt;
};

/**
 * @brief DFD receivers get parameters.
 *
 * This structure contains the parameters for the DFD Receivers Get
 * message. It is used to retrieve a portion of the distribution
 * receiver list.
 *
 * @note This supports pagination of large receiver lists by allowing
 *       the client to request specific ranges of entries.
 */
struct esp_ble_mesh_dfd_receivers_get {
    /** Index of the first entry in the Distribution Receivers List to return */
    uint16_t first_index;
    /** Maximum number of entries to include in the response */
    uint16_t entries_limit;
};

/**
 * @brief DFD target node entry structure.
 *
 * This structure represents the status of a single target node
 * in the distribution receiver list. It contains detailed information
 * about the target's current update phase, transfer status, and progress.
 *
 * This is returned in the DFD Receivers List message.
 */
struct esp_ble_mesh_dfd_target_node_entry {
    /** Unicast address of the target node (15 bits) */
    uint32_t addr:15,
    /** Retrieved update phase of the target node (4 bits, see esp_ble_mesh_dfu_phase) */
             retrieved_update_phase:4,
    /** Update status of the target node (3 bits, see esp_ble_mesh_dfu_status) */
             update_status:3,
    /** Transfer status of the target node (4 bits) */
             transfer_status:4,
    /** Transfer progress percentage of the target node (6 bits, in 2 percent increments) */
             transfer_progress:6;
    /** Index of the firmware image being updated on the target node */
    uint8_t update_fw_idx;
};

/**
 * @brief DFD receiver list response structure.
 *
 * This structure contains the response parameters for the DFD Receivers Get
 * message. It provides a list of target nodes in the distribution receiver
 * list along with their current status.
 */
struct esp_ble_mesh_dfd_receiver_list {
    /** Number of entries included in this response */
    uint16_t entries_cnt;
    /** Index of the first entry in the Distribution Receivers List returned */
    uint16_t first_index;
    /** Pointer to array of target node entries containing receiver status information */
    esp_ble_mesh_dfd_target_node_entry_t *entries;
};

/**
 * @brief DFD distribution capabilities structure.
 *
 * This structure contains the capabilities of a DFD Server as
 * returned by the DFD Capabilities Get message.
 *
 * It provides information about the server's capacity for handling
 * firmware distribution operations, storage limits, and supported
 * out-of-band retrieval methods.
 */
struct esp_ble_mesh_dfd_dist_caps {
    /** Maximum number of receivers that can be stored in the Distribution Receivers List */
    uint16_t max_receiver_list_sz;
    /** Maximum number of firmware images that can be stored in the Firmware Information List */
    uint16_t max_fw_list_sz;
    /** Maximum size of a single firmware image that can be stored (in bytes) */
    uint32_t max_fw_sz;
    /** Maximum upload space available for storing firmware images (in bytes) */
    uint32_t max_upload_space;
    /** Remaining upload space available after current stored firmware (in bytes) */
    uint32_t remaining_upload_space;
    /** Bit field indicating supported out-of-band retrieval methods (0 = none supported) */
    uint8_t oob_retrieval_supported;
    /** Pointer to buffer containing supported URL scheme names (UTF-8, null-terminated) */
    struct net_buf_simple *supported_url_scheme_names;
};

/**
 * @brief DFD Client distribution start parameters.
 *
 * This structure contains the parameters for the DFD Start message.
 * It initiates the firmware distribution process to all target nodes
 * in the Distribution Receivers List.
 */
struct esp_ble_mesh_dfd_cli_dist_start {
    /** TTL value to be used for all messages sent during firmware distribution */
    uint8_t ttl;
    /** Transfer mode: 0x00 = Push BLOB Transfer Mode, 0x01 = Pull BLOB Transfer Mode (2 bits) */
    uint8_t trans_mode : 2,
    /** Update policy: 0x00 = Single Target Node, 0x01 = All Target Nodes (1 bit) */
            update_policy : 1,
    /** Reserved for Future Use (5 bits) */
            RFU : 5;
    /** Application Key Index to be used for securing firmware distribution messages */
    uint16_t app_idx;
    /** Base time value used to calculate transfer timeout (in 100ms units) */
    uint16_t timeout_base;
    /** Index of the firmware image in the Firmware Information List to distribute */
    uint16_t fw_idx;
    /** Flag indicating whether the target address is a virtual address (true) or group address (false) */
    bool is_va;
    /** Target address for firmware distribution (group address or virtual label UUID) */
    union {
        /** Group address for firmware distribution (used when is_va is false) */
        uint16_t group_addr;
        /** Virtual Label UUID for firmware distribution (used when is_va is true) */
        uint8_t label_uuid[16];
    };
};

/**
 * @brief DFD distribution status structure.
 *
 * This structure contains the response parameters for the DFD Get and
 * DFD Start messages. It provides the current status and configuration
 * of the firmware distribution operation.
 */
struct esp_ble_mesh_dfd_dist_status {
    /** Status code for the distribution operation (see esp_ble_mesh_dfd_status) */
    uint8_t status;
    /** Current firmware distribution phase (see esp_ble_mesh_dfd_phase) */
    uint8_t dist_phase;
    /** Multicast address used for the firmware distribution (group address or virtual label) */
    uint16_t multicast_address;
    /** Application Key Index being used for securing firmware distribution messages */
    uint16_t appkey_idx;
    /** TTL value being used for firmware distribution messages */
    uint8_t ttl;
    /** Base time value used to calculate transfer timeout (in 100ms units) */
    uint16_t timeout_base;
    /** Transfer mode: 0x00 = Push BLOB Transfer Mode, 0x01 = Pull BLOB Transfer Mode (2 bits) */
    uint8_t trans_mode:2,
    /** Update policy: 0x00 = Single Target Node, 0x01 = All Target Nodes (1 bit) */
            update_policy:1,
    /** Reserved for Future Use (5 bits) */
            RFU:5;
    /** Index of the firmware image currently being distributed */
    uint16_t firmware_image_index;
};

/**
 * @brief DFD Client upload start parameters.
 *
 * This structure contains the parameters for the DFD Upload Start
 * message. It initiates an in-band firmware
 * upload to the DFD Server using BLOB Transfer.
 *
 * @note This is used for in-band firmware transfers where the firmware
 *       data is transferred through the mesh network.
 */
struct esp_ble_mesh_dfd_cli_dist_upload_start {
    /** TTL value to be used for all messages during the upload */
    uint8_t ttl;
    /** Base time value used to calculate upload timeout (in 100ms units) */
    uint16_t timeout_base;
    /** Total size of the firmware image to be uploaded (in bytes) */
    uint32_t fw_size;
    /** BLOB identifier that will be used for the firmware transfer */
    uint64_t blob_id;
    /** Pointer to buffer containing the firmware ID of the image to upload */
    struct net_buf_simple *fwid;
    /** Pointer to buffer containing the firmware metadata for the upload */
    struct net_buf_simple *fw_metadata;
};

/**
 * @brief DFD Client out-of-band upload start parameters.
 *
 * This structure contains the parameters for the DFD Upload Start OOB
 * message. It initiates an out-of-band firmware
 * upload where the firmware is retrieved from an external source.
 *
 * @note This is used for out-of-band firmware transfers where the firmware
 *       is obtained from a URI or external source.
 */
struct esp_ble_mesh_dfd_cli_dist_upload_oob_start {
    /** Pointer to buffer containing the current firmware ID to check against */
    struct net_buf_simple *fwid;
    /** Pointer to buffer containing the URI for firmware retrieval (UTF-8 string) */
    struct net_buf_simple *url;
};

/**
 * @brief DFD upload status structure.
 *
 * This structure contains the response parameters for the DFD Upload Get,
 * DFD Upload Start, and DFD Upload Start OOB messages.
 *
 * It provides the current status and progress of an ongoing or completed
 * firmware upload operation on the DFD Server.
 */
struct esp_ble_mesh_dfd_upload_status {
    /** Status code for the upload operation (see esp_ble_mesh_dfd_status) */
    uint8_t status;
    /** Current upload phase (see esp_ble_mesh_dfd_upload_phase) */
    uint8_t upload_phase;
    /** Upload progress percentage (0-100%, only valid when progress != 101) */
    uint8_t upload_progress:7,
    /** Upload type: 0x00 = In-band upload, 0x01 = Out-of-band upload */
            upload_type:1;
    /** Union containing firmware ID information based on upload type */
    union {
        /** Firmware ID for in-band upload operations */
        struct net_buf_simple *fwid;
        /** Firmware ID for out-of-band upload operations */
        struct net_buf_simple *oob_fwid;
    };
};

/**
 * @brief DFD Client firmware get parameters.
 *
 * This structure contains the parameters for the DFD Firmware Get
 * message. It requests information about a specific
 * firmware image identified by its firmware ID.
 */
struct esp_ble_mesh_dfd_cli_dist_fw_get {
    /** Pointer to buffer containing the firmware ID to query */
    struct net_buf_simple *fwid;
};

/**
 * @brief DFD Client firmware get by index parameters.
 *
 * This structure contains the parameters for the DFD Firmware Get By Index
 * message. It requests information about a specific firmware image identified
 * by its index in the Firmware Information List.
 */
struct esp_ble_mesh_dfd_cli_dist_fw_get_by_idx {
    /** Index of the firmware image in the Firmware Information List */
    uint16_t dist_fw_idx;
};

/**
 * @brief DFD Client firmware delete parameters.
 *
 * This structure contains the parameters for the DFD Firmware Delete
 * message. It requests deletion of a specific firmware image from the
 * DFD Server.
 *
 * @note Deleting a firmware image that is currently being distributed will
 *       cause the distribution to fail.
 */
struct esp_ble_mesh_dfd_cli_dist_fw_del {
    /** Pointer to buffer containing the firmware ID of the image to delete */
    struct net_buf_simple *fwid;
};

/**
 * @brief DFD firmware status structure.
 *
 * This structure contains the response parameters for the DFD Firmware Get
 * and DFD Firmware Get By Index messages.
 *
 * It provides detailed information about a specific firmware image stored
 * on the DFD Server.
 */
struct esp_ble_mesh_dfd_firmware_status {
    /** Status code for the firmware query operation (see esp_ble_mesh_dfd_status) */
    uint8_t status;
    /** Total number of entries in the Firmware Information List */
    uint16_t entry_cnt;
    /** Index of the firmware image in the Firmware Information List */
    uint16_t fw_idx;
    /** Pointer to buffer containing the firmware ID of the queried image */
    struct net_buf_simple *fwid;
};

/**
 * @brief DFD Client action enumeration.
 *
 * This enumeration defines the different types of actions that can be performed
 * by the DFD Client model. These actions are used internally to
 * distinguish between different DFD message types when processing
 * client requests.
 */
enum esp_ble_mesh_dfd_client_act {
    /** DFD Client Get action - retrieving information from DFD Server */
    ESP_BLE_MESH_ACT_DFD_CLIENT_GET,
    /** DFD Client Set action - sending commands/configuration to DFD Server */
    ESP_BLE_MESH_ACT_DFD_CLIENT_SET,
    /** Maximum value for DFD Client action enumeration (bounds checking) */
    ESP_BLE_MESH_ACT_DFD_CLIENT_MAX,
};

/**
 * @brief DFD Client callback event enumeration.
 *
 * This enumeration defines the different callback events that can be generated
 * by the DFD Client model. These events notify the application
 * about the status of DFD operations, message responses, timeouts,
 * and completion of various procedures.
 *
 * Application should handle these events to properly manage firmware
 * distribution operations and provide user feedback.
 */
enum esp_ble_mesh_dfd_client_cb_evt {
    /** DFD Client timeout event - operation timed out waiting for response */
    ESP_BLE_MESH_EVT_DFD_CLIENT_TIMEOUT,
    /** DFD Client receive response event - received a response from DFD Server */
    ESP_BLE_MESH_EVT_DFD_CLIENT_RECV_RSP,
    /** DFD Client send complete event - message transmission completed */
    ESP_BLE_MESH_ACT_DFD_CLIEND_SEND_COMP,
    /** Maximum value for DFD Client callback event enumeration (bounds checking) */
    ESP_BLE_MESH_EVT_DFD_CLIENT_MAX,
};

/**
 * @brief DFD Client get parameters union.
 *
 * This union contains different parameter structures for various DFD Client
 * Get messages. The specific structure used depends on the type of
 * get operation being performed.
 *
 * This allows type-safe access to message-specific parameters while
 * using a single parameter type in the API.
 */
typedef union {
    /** Parameters for DFD Receivers Get operation */
    esp_ble_mesh_dfd_receivers_get_t receivers_get;
    /** Parameters for DFD Firmware Get operation */
    esp_ble_mesh_dfd_cli_dist_fw_get_t dist_fw_get;
    /** Parameters for DFD Firmware Get By Index operation */
    esp_ble_mesh_dfd_cli_dist_fw_get_by_idx_t dist_fw_get_by_idx;
} esp_ble_mesh_dfd_client_get_param_t;

/**
 * @brief DFD Client set parameters union.
 *
 * This union contains different parameter structures for various DFD Client
 * Set messages. The specific structure used depends on the type of
 * set operation being performed.
 *
 * This allows type-safe access to message-specific parameters while
 * using a single parameter type in the API.
 */
typedef union {
    /** Parameters for DFD Receivers Add operation */
    esp_ble_mesh_dfd_cli_receivers_add_t receivers_add;
    /** Parameters for DFD Start operation */
    esp_ble_mesh_dfd_cli_dist_start_t dist_start;
    /** Parameters for DFD Upload Start operation */
    esp_ble_mesh_dfd_cli_dist_upload_start_t dist_upload_start;
    /** Parameters for DFD Upload Start OOB operation */
    esp_ble_mesh_dfd_cli_dist_upload_oob_start_t dist_upload_oob_start;
    /** Parameters for DFD Firmware Get operation */
    esp_ble_mesh_dfd_cli_dist_fw_get_t dist_fw_get;
    /** Parameters for DFD Firmware Delete operation */
    esp_ble_mesh_dfd_cli_dist_fw_del_t dist_fw_del;
} esp_ble_mesh_dfd_client_set_param_t;

/**
 * @brief DFD Client common callback parameters union.
 *
 * This union contains different response structures for various DFD Client
 * received messages. The specific structure used depends on the type of
 * message that triggered the callback.
 *
 * This allows type-safe access to message-specific response data
 * while using a single callback parameter type.
 */
union esp_ble_mesh_dfd_client_common_cb_param {
    /** Response parameters for DFD Receivers Add/Delete operations */
    esp_ble_mesh_dfd_receiver_status_t      receiver_status;
    /** Response parameters for DFD Receivers Get operation */
    esp_ble_mesh_dfd_receiver_list_t        receiver_list;
    /** Response parameters for DFD Capabilities Get operation */
    esp_ble_mesh_dfd_dist_caps_t            dist_caps;
    /** Response parameters for DFD Get/Start operations */
    esp_ble_mesh_dfd_dist_status_t          dist_status;
    /** Response parameters for DFD Upload Get/Start operations */
    esp_ble_mesh_dfd_upload_status_t        upload_status;
    /** Response parameters for DFD Firmware Get operations */
    esp_ble_mesh_dfd_firmware_status_t      firmware_status;
};

/**
 * @brief DFD Client callback parameters structure.
 *
 * This structure contains the parameters passed to the DFD Client
 * callback function for various events. It provides information about
 * the operation status, error codes, and event-specific data.
 *
 * Applications should examine the err_code field first to determine
 * whether the operation was successful, then check the appropriate
 * field in the status_cb union for operation-specific data.
 */
struct esp_ble_mesh_dfd_client_cb_param {
    /** Error code indicating success (0) or failure (negative) of the operation */
    int err_code;
    /** Common client parameters including source address, destination, etc. */
    esp_ble_mesh_client_common_param_t *params;
    /** Union containing event-specific response data */
    esp_ble_mesh_dfd_client_common_cb_param_t status_cb;
};

/**
 * @brief Register DFD Client callback function.
 *
 * This function registers a callback function that will be called when
 * DFD Client events occur, such as message timeouts, responses,
 * or operation completions.
 *
 * @param callback Pointer to the callback function to register.
 *
 * @return ESP_OK on success, or error code otherwise.
 *
 * @note Only one callback can be registered at a time. Calling this
 *       function with a NULL callback will deregister the current callback.
 */
esp_err_t esp_ble_mesh_register_dfd_cli_callback(esp_ble_mesh_dfd_client_cb_t callback);

/**
 * @brief Send DFD Client Get message.
 *
 * This function sends a DFD Get message to retrieve information
 * from the DFD Server. The specific type of information requested
 * depends on the get_param structure provided.
 *
 * @param params    Common client parameters including destination address, TTL, etc.
 * @param get_param Union containing the specific get message parameters.
 *
 * @return ESP_OK on success, or error code otherwise.
 *
 * @note The response will be delivered through the registered callback
 *       function with the ESP_BLE_MESH_EVT_DFD_CLIENT_RECV_RSP event.
 */
esp_err_t esp_ble_mesh_dfd_cli_get(esp_ble_mesh_client_common_param_t *params,
                                   esp_ble_mesh_dfd_client_get_param_t *get_param);

/**
 * @brief Send DFD Client Set message.
 *
 * This function sends a DFD Set message to perform various operations
 * on the DFD Server, such as starting firmware distribution,
 * uploading firmware, or managing the receiver list.
 *
 * @param params    Common client parameters including destination address, TTL, etc.
 * @param set_param Union containing the specific set message parameters.
 *
 * @return ESP_OK on success, or error code otherwise.
 *
 * @note The response will be delivered through the registered callback
 *       function with the ESP_BLE_MESH_EVT_DFD_CLIENT_RECV_RSP event.
 */
esp_err_t esp_ble_mesh_dfd_cli_set(esp_ble_mesh_client_common_param_t *params,
                                   esp_ble_mesh_dfd_client_set_param_t *set_param);

#endif /* CONFIG_BLE_MESH_DFD_CLI */

#if CONFIG_BLE_MESH_DFD_SRV
/**
 * @brief Firmware Distribution status enumeration.
 *
 * This enumeration defines the status codes used in DFD operations.
 * These status codes indicate success, failure, and error conditions
 * for various firmware distribution operations.
 */
enum esp_ble_mesh_dfd_status {
    /** The message was processed successfully. */
    ESP_BLE_MESH_DFD_SUCCESS,

    /** Insufficient resources on the node. */
    ESP_BLE_MESH_DFD_ERR_INSUFFICIENT_RESOURCES,

    /** The operation cannot be performed while the Server is in the current phase. */
    ESP_BLE_MESH_DFD_ERR_WRONG_PHASE,

    /** An internal error occurred on the node. */
    ESP_BLE_MESH_DFD_ERR_INTERNAL,

    /** The requested firmware image is not stored on the Distributor. */
    ESP_BLE_MESH_DFD_ERR_FW_NOT_FOUND,

    /** The AppKey identified by the AppKey Index is not known to the node. */
    ESP_BLE_MESH_DFD_ERR_INVALID_APPKEY_INDEX,

    /** There are no Target nodes in the Distribution Receivers List state. */
    ESP_BLE_MESH_DFD_ERR_RECEIVERS_LIST_EMPTY,

    /** Another firmware image distribution is in progress. */
    ESP_BLE_MESH_DFD_ERR_BUSY_WITH_DISTRIBUTION,

    /** Another upload is in progress. */
    ESP_BLE_MESH_DFD_ERR_BUSY_WITH_UPLOAD,

    /** The URI scheme name indicated by the Update URI is not supported. */
    ESP_BLE_MESH_DFD_ERR_URI_NOT_SUPPORTED,

    /** The format of the Update URI is invalid. */
    ESP_BLE_MESH_DFD_ERR_URI_MALFORMED,

    /** The URI is currently unreachable. */
    ESP_BLE_MESH_DFD_ERR_URI_UNREACHABLE,

    /** The Check Firmware OOB procedure did not find any new firmware. */
    ESP_BLE_MESH_DFD_ERR_NEW_FW_NOT_AVAILABLE,

    /** The suspension of the Distribute Firmware procedure failed. */
    ESP_BLE_MESH_DFD_ERR_SUSPEND_FAILED,
};

/**
 * @brief Firmware distribution phases enumeration.
 *
 * This enumeration defines the different phases of a firmware distribution
 * operation. The phases track the state of firmware distribution from
 * idle through transfer, verification, application, and completion states.
 */
enum esp_ble_mesh_dfd_phase {
    /** No firmware distribution is in progress. */
    ESP_BLE_MESH_DFD_PHASE_IDLE,

    /** Firmware distribution is in progress. */
    ESP_BLE_MESH_DFD_PHASE_TRANSFER_ACTIVE,

    /** The Transfer BLOB procedure has completed successfully. */
    ESP_BLE_MESH_DFD_PHASE_TRANSFER_SUCCESS,

    /** The Apply Firmware on Target Nodes procedure is being executed. */
    ESP_BLE_MESH_DFD_PHASE_APPLYING_UPDATE,

    /** The Distribute Firmware procedure has completed successfully. */
    ESP_BLE_MESH_DFD_PHASE_COMPLETED,

    /** The Distribute Firmware procedure has failed. */
    ESP_BLE_MESH_DFD_PHASE_FAILED,

    /** The Cancel Firmware Update procedure is being executed. */
    ESP_BLE_MESH_DFD_PHASE_CANCELING_UPDATE,

    /** The Transfer BLOB procedure is suspended. */
    ESP_BLE_MESH_DFD_PHASE_TRANSFER_SUSPENDED,
};

/**
 * @brief Firmware upload phases enumeration.
 *
 * This enumeration defines the phases of firmware upload operations
 * on the DFD Server. It tracks the state of firmware storage procedures
 * including both in-band and out-of-band upload methods.
 */
enum esp_ble_mesh_dfd_upload_phase {
    /** No firmware upload is in progress. */
    ESP_BLE_MESH_DFD_UPLOAD_PHASE_IDLE,

    /** The Store Firmware procedure is being executed. */
    ESP_BLE_MESH_DFD_UPLOAD_PHASE_TRANSFER_ACTIVE,

    /** The Store Firmware procedure or Store Firmware OOB procedure failed. */
    ESP_BLE_MESH_DFD_UPLOAD_PHASE_TRANSFER_ERROR,

    /** The Store Firmware procedure or the Store Firmware OOB procedure completed successfully. */
    ESP_BLE_MESH_DFD_UPLOAD_PHASE_TRANSFER_SUCCESS,
};

#if 0
/**
 * @brief Flash partition
 *
 * This structure represents a fixed-size partition on a flash device.
 * Each partition contains one or more flash sectors.
 */
struct flash_area {
    /** ID number */
    uint8_t fa_id;
    uint16_t pad16;
    /** Start offset from the beginning of the flash device */
    off_t fa_off;
    /** Total size */
    size_t fa_size;
    /** Backing flash device */
    const struct device *fa_dev;
#if CONFIG_FLASH_MAP_LABELS
    /** Partition label if defined in DTS. Otherwise nullptr; */
    const char *fa_label;
#endif
};
#endif

/** Firmware Distribution Server callbacks: */
struct esp_ble_mesh_dfd_srv_cb {

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
    int (*recv)(esp_ble_mesh_dfd_srv_t *srv,
                const esp_ble_mesh_dfu_slot_t *slot,
                const esp_ble_mesh_blob_io_t **io);

#ifdef CONFIG_BLE_MESH_DFD_SRV_OOB_UPLOAD
    /** @brief Firmware upload OOB start callback.
     *
     *  Called at the start of an OOB firmware upload. The application must
     *  start a firmware check using an OOB mechanism, and then call
     *  bt_mesh_dfd_srv_oob_check_complete. Depending on the return
     *  value of this function, the application must then start storing the
     *  firmware image using an OOB mechanism, and call
     *  bt_mesh_dfd_srv_oob_store_complete. This callback is mandatory
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
     *  @return ESP_BLE_MESH_DFD_SUCCESS on success, or error code otherwise.
     */
    int (*start_oob_upload)(esp_ble_mesh_dfd_srv_t *srv,
                            const esp_ble_mesh_dfu_slot_t *slot,
                            const uint8_t *uri, uint8_t uri_len,
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
    void (*cancel_oob_upload)(esp_ble_mesh_dfd_srv_t *srv,
                              const esp_ble_mesh_dfu_slot_t *slot);

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
    uint8_t (*oob_progress_get)(esp_ble_mesh_dfd_srv_t *srv,
                                const esp_ble_mesh_dfu_slot_t *slot);
#endif /* CONFIG_BLE_MESH_DFD_SRV_OOB_UPLOAD */

    /** @brief Slot delete callback.
     *
     *  Called when the Firmware Distribution Server is about to delete a DFU image slot.
     *  All allocated data associated with the firmware slot should be
     *  deleted.
     *
     *  @param srv  Firmware Update Server instance.
     *  @param slot DFU image slot being deleted.
     */
    void (*del)(esp_ble_mesh_dfd_srv_t *srv,
                const esp_ble_mesh_dfu_slot_t *slot);

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
    int (*send)(esp_ble_mesh_dfd_srv_t *srv,
                const esp_ble_mesh_dfu_slot_t *slot,
                const esp_ble_mesh_blob_io_t **io);

    /** @brief Phase change callback (Optional).
     *
     *  Called whenever the phase of the Firmware Distribution Server changes.
     *
     *  @param srv  Firmware Distribution Server model instance.
     *  @param phase  New Firmware Distribution phase.
     */
    void (*phase)(esp_ble_mesh_dfd_srv_t *srv, esp_ble_mesh_dfd_phase_t phase);
};

/** Firmware Distribution Server instance. */
struct esp_ble_mesh_dfd_srv {
    const esp_ble_mesh_dfd_srv_cb_t *cb;
    esp_ble_mesh_model_t *mod;
    esp_ble_mesh_dfu_cli_t dfu;
    esp_ble_mesh_dfu_target_t targets[CONFIG_BLE_MESH_DFD_SRV_TARGETS_MAX];
    esp_ble_mesh_blob_target_pull_t pull_ctxs[CONFIG_BLE_MESH_DFD_SRV_TARGETS_MAX];
    const esp_ble_mesh_blob_io_t *io;
    uint16_t target_cnt;
    uint16_t slot_idx;
    bool apply;
    esp_ble_mesh_dfd_phase_t phase;
    esp_ble_mesh_blob_cli_inputs_t inputs;

    struct {
        esp_ble_mesh_dfd_upload_phase_t phase;
        esp_ble_mesh_dfu_slot_t *slot;
#if 0
        const struct flash_area *area;
#endif
        esp_ble_mesh_blob_srv_t blob;
#ifdef CONFIG_BLE_MESH_DFD_SRV_OOB_UPLOAD
        bool is_oob;
        bool is_pending_oob_check;
        struct {
            uint8_t uri_len;
            uint8_t uri[CONFIG_BLE_MESH_DFU_URI_MAXLEN];
            uint16_t current_fwid_len;
            uint8_t current_fwid[CONFIG_BLE_MESH_DFU_FWID_MAXLEN];
            esp_ble_mesh_msg_ctx_t ctx;
        } oob;
#endif /* CONFIG_BLE_MESH_DFD_SRV_OOB_UPLOAD */
    } upload;

#ifdef CONFIG_BLE_MESH_DFD_SRV_OOB_UPLOAD
    struct {
        const uint8_t *schemes;
        const uint8_t count;
    } oob_schemes;
#endif /* CONFIG_BLE_MESH_DFD_SRV_OOB_UPLOAD */
};
#endif /* CONFIG_BLE_MESH_DFD_SRV */

#ifdef __cplusplus
}
#endif

#endif /* _ESP_BLE_MESH_DFU_MODEL_API_H_ */
