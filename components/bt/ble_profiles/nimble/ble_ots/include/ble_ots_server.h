/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef BLE_OTS_SERVER_H
#define BLE_OTS_SERVER_H

#include <stdint.h>
#include <stdbool.h>
#include "ble_ots_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Kconfig Defaults
 *****************************************************************************/
#ifndef CONFIG_BLE_OTS_SERVER_MAX_OBJECTS
#define CONFIG_BLE_OTS_SERVER_MAX_OBJECTS            10
#endif

#ifndef CONFIG_BLE_OTS_SERVER_MAX_CONCURRENCY
#define CONFIG_BLE_OTS_SERVER_MAX_CONCURRENCY        1
#endif

#ifndef CONFIG_BLE_OTS_SERVER_TRANSFER_TIMEOUT_SEC
#define CONFIG_BLE_OTS_SERVER_TRANSFER_TIMEOUT_SEC   30
#endif

/*****************************************************************************
 * Constants
 *****************************************************************************/

/** @brief Invalid Object ID sentinel (outside the valid 48-bit range) */
#define BLE_OTS_OBJ_ID_INVALID      0xFFFFFFFFFFFFFFFFULL

/*****************************************************************************
 * Transfer Completion Status
 *****************************************************************************/

/**
 * @brief Status codes for object transfer completion.
 */
typedef enum {
    BLE_OTS_TRANSFER_SUCCESS         = 0x00, /*!< All expected octets transferred */
    BLE_OTS_TRANSFER_ABORTED         = 0x01, /*!< Aborted via OACP Abort */
    BLE_OTS_TRANSFER_TIMEOUT         = 0x02, /*!< Inactivity timeout */
    BLE_OTS_TRANSFER_CHANNEL_CLOSED  = 0x03, /*!< OTC closed unexpectedly */
    BLE_OTS_TRANSFER_EXCESS_DATA     = 0x04, /*!< More data received than expected */
} ble_ots_server_transfer_status_t;

/*****************************************************************************
 * Server Configuration
 *****************************************************************************/

/**
 * @brief Configuration for OTS server initialization.
 */
typedef struct {
    uint32_t oacp_features;                          /*!< OACP feature bit-field */
    uint32_t olcp_features;                          /*!< OLCP feature bit-field */
    const ble_ots_obj_type_entry_t *supported_types; /*!< Supported type UUIDs for OACP Create */
    uint8_t num_supported_types;                     /*!< Number of entries in supported_types */
    bool include_obj_first_created;                  /*!< Include Object First-Created char */
    bool include_obj_last_modified;                  /*!< Include Object Last-Modified char */
    bool include_obj_changed;                        /*!< Include Object Changed char */
    bool include_obj_list_filter;                    /*!< Include 3x Object List Filter chars */
    bool has_realtime_clock;                         /*!< Server has a real-time clock */
    bool obj_name_writable;                          /*!< Object Name supports Write */
    bool obj_properties_writable;                    /*!< Object Properties supports Write */
} ble_ots_server_config_t;

/*****************************************************************************
 * Object Parameters (for ble_ots_server_add_object)
 *****************************************************************************/

/**
 * @brief Parameters for adding a pre-populated object to the server.
 */
typedef struct {
    const char *name;                    /*!< UTF-8 object name (0–120 octets), may be NULL */
    uint8_t name_len;                    /*!< Length of name in octets */
    ble_ots_obj_type_entry_t type;       /*!< Object type UUID */
    uint32_t properties;                 /*!< Initial object properties bit-field */
    ble_ots_date_time_t first_created;   /*!< First-created timestamp */
    ble_ots_date_time_t last_modified;   /*!< Last-modified timestamp */
    const uint8_t *data;                 /*!< Initial content data, may be NULL */
    uint32_t data_len;                   /*!< Length of initial data in octets */
    uint32_t allocated_size;             /*!< Allocated size (>= data_len) */
} ble_ots_server_obj_params_t;

/*****************************************************************************
 * Event Types
 *****************************************************************************/

/**
 * @brief OTS server event types dispatched through the application callback.
 */
typedef enum {
    BLE_OTS_SERVER_EVT_OBJECT_CREATED,   /*!< Object created via OACP Create */
    BLE_OTS_SERVER_EVT_OBJECT_DELETED,   /*!< Object deleted via OACP Delete */
    BLE_OTS_SERVER_EVT_EXECUTE,          /*!< OACP Execute on current object */
    BLE_OTS_SERVER_EVT_CHECKSUM_REQUEST, /*!< OACP Calculate Checksum completed */
    BLE_OTS_SERVER_EVT_READ_COMPLETE,    /*!< Read transfer completed */
    BLE_OTS_SERVER_EVT_WRITE_COMPLETE,   /*!< Write transfer completed */
    BLE_OTS_SERVER_EVT_METADATA_WRITTEN, /*!< Client wrote a metadata characteristic */
} ble_ots_server_event_t;

/*****************************************************************************
 * Event Data Structures
 *****************************************************************************/

/**
 * @brief Event data for BLE_OTS_SERVER_EVT_OBJECT_CREATED.
 */
typedef struct {
    ble_ots_obj_id_t object_id;          /*!< Allocated Object ID */
    ble_ots_obj_type_entry_t type;       /*!< Object Type UUID */
    uint32_t allocated_size;             /*!< Allocated size in octets */
} ble_ots_server_oacp_create_evt_t;

/**
 * @brief Event data for BLE_OTS_SERVER_EVT_OBJECT_DELETED.
 */
typedef struct {
    ble_ots_obj_id_t object_id;          /*!< Deleted Object ID */
} ble_ots_server_oacp_delete_evt_t;

/**
 * @brief Event data for BLE_OTS_SERVER_EVT_EXECUTE.
 *
 * @note The callback carries out the execute action and reports its outcome in
 *       @p result, which the server then indicates to the client. Work that
 *       cannot complete inside the callback must be acknowledged with
 *       BLE_OTS_OACP_RESULT_SUCCESS once accepted, since the OACP indication is
 *       sent as soon as the callback returns.
 */
typedef struct {
    ble_ots_obj_id_t object_id;          /*!< Current Object ID */
    const uint8_t *param;                /*!< Optional parameter data (NULL if none), valid
                                              only for the duration of the callback */
    uint16_t param_len;                  /*!< Parameter data length (0 if none) */
    uint8_t result;                      /*!< [out] OACP Result Code sent to the client, one of
                                              ble_ots_oacp_result_code_t. Pre-set to
                                              BLE_OTS_OACP_RESULT_SUCCESS; overwrite it with e.g.
                                              BLE_OTS_OACP_RESULT_INVALID_PARAMETER or
                                              BLE_OTS_OACP_RESULT_OPERATION_FAILED to report a
                                              failure. Out-of-range values are reported as
                                              BLE_OTS_OACP_RESULT_OPERATION_FAILED. */
} ble_ots_server_oacp_execute_evt_t;

/**
 * @brief Event data for BLE_OTS_SERVER_EVT_CHECKSUM_REQUEST.
 */
typedef struct {
    ble_ots_obj_id_t object_id;          /*!< Object ID */
    uint32_t offset;                     /*!< Byte offset of checksum range */
    uint32_t length;                     /*!< Length of checksum range */
    uint32_t checksum;                   /*!< Computed CRC-32 value */
} ble_ots_server_oacp_checksum_evt_t;

/**
 * @brief Event data for BLE_OTS_SERVER_EVT_READ_COMPLETE.
 */
typedef struct {
    ble_ots_obj_id_t object_id;          /*!< Object ID that was read */
    uint32_t offset;                     /*!< Starting offset */
    uint32_t length;                     /*!< Requested length in octets */
    uint32_t bytes_sent;                 /*!< Actual octets sent */
    ble_ots_server_transfer_status_t status; /*!< Completion status */
} ble_ots_server_evt_read_complete_t;

/**
 * @brief Event data for BLE_OTS_SERVER_EVT_WRITE_COMPLETE.
 */
typedef struct {
    ble_ots_obj_id_t object_id;          /*!< Object ID that was written */
    uint32_t offset;                     /*!< Starting offset */
    uint32_t bytes_received;             /*!< Actual octets received */
    ble_ots_server_transfer_status_t status; /*!< Completion status */
} ble_ots_server_evt_write_complete_t;

/**
 * @brief Event data for BLE_OTS_SERVER_EVT_METADATA_WRITTEN.
 */
typedef struct {
    uint16_t conn_id;                    /*!< Connection ID of the writing client */
    ble_ots_obj_id_t object_id;          /*!< Object ID of affected object */
    uint16_t char_uuid;                  /*!< UUID of written characteristic */
    const char *name;                    /*!< New name (valid when char_uuid == 0x2ABE) */
    uint8_t name_len;                    /*!< Name length (valid when char_uuid == 0x2ABE) */
    ble_ots_date_time_t date_time;       /*!< New timestamp (valid for 0x2AC1/0x2AC2) */
    uint32_t properties;                 /*!< New properties (valid for 0x2AC4) */
} ble_ots_server_metadata_evt_t;

/*****************************************************************************
 * Callback Parameter Union
 *****************************************************************************/

/**
 * @brief Union of all OTS server event parameter structures.
 */
typedef union {
    ble_ots_server_oacp_create_evt_t    object_created;   /*!< BLE_OTS_SERVER_EVT_OBJECT_CREATED */
    ble_ots_server_oacp_delete_evt_t    object_deleted;   /*!< BLE_OTS_SERVER_EVT_OBJECT_DELETED */
    ble_ots_server_oacp_execute_evt_t   execute;          /*!< BLE_OTS_SERVER_EVT_EXECUTE */
    ble_ots_server_oacp_checksum_evt_t  checksum;         /*!< BLE_OTS_SERVER_EVT_CHECKSUM_REQUEST */
    ble_ots_server_evt_read_complete_t  read_complete;    /*!< BLE_OTS_SERVER_EVT_READ_COMPLETE */
    ble_ots_server_evt_write_complete_t write_complete;   /*!< BLE_OTS_SERVER_EVT_WRITE_COMPLETE */
    ble_ots_server_metadata_evt_t       metadata_written; /*!< BLE_OTS_SERVER_EVT_METADATA_WRITTEN */
} ble_ots_server_cb_param_t;

/*****************************************************************************
 * Callback Type
 *****************************************************************************/

/**
 * @brief Application callback for all OTS server events.
 *
 * Runs in the NimBLE host task with the OTS server's internal lock held, so it
 * must not block: no delays, no waiting on semaphores or queues, no blocking
 * I/O. Post the work to your own task instead. Calling ble_ots_server_* APIs
 * from within the callback is safe.
 *
 * @param event Event type
 * @param param Event-specific parameter data
 */
typedef void (*ble_ots_server_cb_t)(ble_ots_server_event_t event,
                                     ble_ots_server_cb_param_t *param);

/*****************************************************************************
 * Public APIs
 *****************************************************************************/

/**
 * @brief Initialize the OTS server.
 *
 * Validates configuration, allocates object database and concurrency pool,
 * registers the GATT service with all OTS characteristics and CCCDs,
 * registers the L2CAP server on PSM_OTS, and creates the Directory Listing
 * Object if the server supports multiple objects.
 *
 * @note The GATT service and the L2CAP PSM listener are registered only on the
 *       first call after boot, because NimBLE offers no way to unregister
 *       them. A later call following ble_ots_server_deinit() re-creates the
 *       object database and the concurrency pool but keeps the characteristic
 *       layout established by the first initialization — @p config fields that
 *       affect that layout (obj_name_writable, include_obj_changed,
 *       include_obj_list_filter, ...) are ignored from then on.
 *
 * @param config Pointer to server configuration
 * @return 0 on success, non-zero error code on failure
 */
int ble_ots_server_init(const ble_ots_server_config_t *config);

/**
 * @brief Deinitialize the OTS server.
 *
 * Stops serving OTS, closes open OTC channels, unregisters the GAP event
 * listener and frees object storage and the concurrency pool. Further requests
 * from peers are rejected until the server is initialized again.
 *
 * @note The GATT service and the L2CAP PSM listener stay registered with the
 *       NimBLE host: it provides no API to remove them. Their callbacks remain
 *       live and reject every request while the server is deinitialized.
 *
 * @return 0 on success, non-zero error code on failure
 */
int ble_ots_server_deinit(void);

/**
 * @brief Register the application event callback.
 *
 * Only one callback may be registered; calling again replaces the previous one.
 *
 * @param callback Application event callback function
 * @return 0 on success, non-zero error code on failure
 */
int ble_ots_server_register_cb(ble_ots_server_cb_t callback);

/**
 * @brief Add a pre-populated object to the server object database.
 *
 * Server-initiated creation (not via OACP). Allocates a unique Object ID,
 * stores metadata and optional initial content. Triggers Object Changed
 * indication with Creation flag.
 *
 * @param params     Pointer to object parameters
 * @param out_obj_id Output: allocated Object ID on success (may be NULL)
 * @return 0 on success, negative error code on failure
 */
int ble_ots_server_add_object(const ble_ots_server_obj_params_t *params,
                               ble_ots_obj_id_t *out_obj_id);

/**
 * @brief Remove an object from the server object database.
 *
 * Server-initiated deletion. The DLO (Object ID 0) cannot be removed.
 * Triggers Object Changed indication with Deletion flag.
 *
 * @param object_id Object ID to remove
 * @return 0 on success, negative error code on failure
 */
int ble_ots_server_remove_object(ble_ots_obj_id_t object_id);

/**
 * @brief Set or update object content data from the server side.
 *
 * Updates Current Size and may increase Allocated Size. Triggers Object
 * Changed indication with Content Changed flag.
 *
 * @param object_id Object ID of the target object
 * @param data      Pointer to data buffer
 * @param offset    Byte offset within the object
 * @param length    Number of octets to write
 * @return 0 on success, negative error code on failure
 */
int ble_ots_server_set_object_data(ble_ots_obj_id_t object_id,
                                    const uint8_t *data,
                                    uint32_t offset,
                                    uint32_t length);

/**
 * @brief Trigger Object Changed indication for a server-initiated change.
 *
 * Dispatches indication to all subscribed clients. Source of Change (bit 0)
 * is forced to 0 (Server). The DLO (ID 0) cannot be the target.
 *
 * @param object_id Object ID of the changed object
 * @param flags     Change flags (BLE_OTS_OBJ_CHANGED_FLAG_*)
 * @return 0 on success, non-zero error code on failure
 */
int ble_ots_server_object_changed(ble_ots_obj_id_t object_id, uint8_t flags);

#ifdef __cplusplus
}
#endif

#endif /* BLE_OTS_SERVER_H */
