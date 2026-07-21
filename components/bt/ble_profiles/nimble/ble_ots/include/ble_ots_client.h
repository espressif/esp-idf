/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef BLE_OTS_CLIENT_H
#define BLE_OTS_CLIENT_H

#include <stdint.h>
#include <stdbool.h>
#include "ble_ots_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Kconfig Defaults
 *****************************************************************************/
#ifndef CONFIG_BLE_OTS_CLIENT_OACP_TIMEOUT_MS
#define CONFIG_BLE_OTS_CLIENT_OACP_TIMEOUT_MS       30000
#endif

#ifndef CONFIG_BLE_OTS_CLIENT_OLCP_TIMEOUT_MS
#define CONFIG_BLE_OTS_CLIENT_OLCP_TIMEOUT_MS       30000
#endif

#ifndef CONFIG_BLE_OTS_CLIENT_TRANSFER_TIMEOUT_MS
#define CONFIG_BLE_OTS_CLIENT_TRANSFER_TIMEOUT_MS   30000
#endif

#ifndef CONFIG_BLE_OTS_CLIENT_MAX_NAME_LEN
#define CONFIG_BLE_OTS_CLIENT_MAX_NAME_LEN          120
#endif

/*****************************************************************************
 * Event Codes
 *****************************************************************************/

/**
 * @brief OTS client event codes delivered through the application callback.
 */
typedef enum {
    /* Discovery module events */
    BLE_OTS_CLIENT_EVT_DISCOVER_COMPLETE = 0,  /*!< Service/char/desc discovery finished */
    BLE_OTS_CLIENT_EVT_FEATURE_READ,           /*!< OTS Feature read completed */
    BLE_OTS_CLIENT_EVT_OBJECT_NAME_READ,       /*!< Object Name read completed */
    BLE_OTS_CLIENT_EVT_OBJECT_TYPE_READ,       /*!< Object Type UUID read completed */
    BLE_OTS_CLIENT_EVT_OBJECT_SIZE_READ,       /*!< Object Size read completed */
    BLE_OTS_CLIENT_EVT_FIRST_CREATED_READ,     /*!< First-Created date-time read completed */
    BLE_OTS_CLIENT_EVT_LAST_MODIFIED_READ,     /*!< Last-Modified date-time read completed */
    BLE_OTS_CLIENT_EVT_OBJECT_ID_READ,         /*!< Object ID read completed */
    BLE_OTS_CLIENT_EVT_PROPERTIES_READ,        /*!< Object Properties read completed */
    BLE_OTS_CLIENT_EVT_METADATA_WRITTEN,       /*!< Metadata write completed */

    /* Object navigation events */
    BLE_OTS_CLIENT_EVT_OLCP_RESPONSE,          /*!< OLCP indication received */
    BLE_OTS_CLIENT_EVT_FILTER_SET,             /*!< Object List Filter write completed */
    BLE_OTS_CLIENT_EVT_FILTER_READ,            /*!< Object List Filter read completed */
    BLE_OTS_CLIENT_EVT_OBJECT_CHANGED,         /*!< Object Changed indication received */

    /* Transfer events */
    BLE_OTS_CLIENT_EVT_OACP_RESPONSE,          /*!< OACP indication received */
    BLE_OTS_CLIENT_EVT_CHANNEL_OPEN,           /*!< Object Transfer Channel opened */
    BLE_OTS_CLIENT_EVT_CHANNEL_CLOSED,         /*!< Object Transfer Channel closed */
    BLE_OTS_CLIENT_EVT_DATA_RECEIVED,          /*!< Object data chunk received */
    BLE_OTS_CLIENT_EVT_DATA_SENT,              /*!< Object data chunk sent */
    BLE_OTS_CLIENT_EVT_CP_TIMEOUT,             /*!< Control-point response timer expired */
    BLE_OTS_CLIENT_EVT_TRANSFER_TIMEOUT,       /*!< Data transfer inactivity timeout */
} ble_ots_client_event_t;

/*****************************************************************************
 * Event Data Structures
 *****************************************************************************/

/**
 * @brief Discovered characteristic handle set for a single OTS instance.
 *
 * A handle value of 0x0000 means the characteristic was not found.
 */
typedef struct {
    uint16_t ots_feature_handle;                /*!< OTS Feature (0x2ABD) */
    uint16_t object_name_handle;                /*!< Object Name (0x2ABE) */
    uint16_t object_type_handle;                /*!< Object Type (0x2ABF) */
    uint16_t object_size_handle;                /*!< Object Size (0x2AC0) */
    uint16_t first_created_handle;              /*!< First-Created (0x2AC1), 0 if absent */
    uint16_t last_modified_handle;              /*!< Last-Modified (0x2AC2), 0 if absent */
    uint16_t object_id_handle;                  /*!< Object ID (0x2AC3), 0 if absent */
    uint16_t object_properties_handle;          /*!< Object Properties (0x2AC4) */
    uint16_t oacp_handle;                       /*!< OACP (0x2AC5) */
    uint16_t oacp_cccd_handle;                  /*!< OACP CCCD */
    uint16_t olcp_handle;                       /*!< OLCP (0x2AC6), 0 if absent */
    uint16_t olcp_cccd_handle;                  /*!< OLCP CCCD, 0 if absent */
    uint16_t object_list_filter_handle[3];      /*!< Object List Filter (0x2AC7); per spec there are
                                                     either three instances or none — all 0 if absent */
    uint16_t object_changed_handle;             /*!< Object Changed (0x2AC8), 0 if absent */
    uint16_t object_changed_cccd_handle;        /*!< Object Changed CCCD, 0 if absent */
} ble_ots_client_char_handles_t;

/**
 * @brief Discovery complete event data.
 */
typedef struct {
    int status;                                 /*!< 0 on success, ATT/GATT error on failure */
    ble_ots_client_char_handles_t handles;      /*!< Copy of discovered handles */
    bool multi_object_server;                   /*!< true if OLCP is exposed */
} ble_ots_client_discover_complete_t;

/**
 * @brief Feature read event data.
 */
typedef struct {
    int status;                                 /*!< 0 on success, ATT error on failure */
    ble_ots_feature_t feature;                  /*!< OACP + OLCP feature bit-fields */
} ble_ots_client_feature_read_t;

/**
 * @brief Object Name read event data.
 */
typedef struct {
    int status;                                 /*!< 0 on success, ATT error on failure */
    const char *name;                           /*!< UTF-8 name buffer, not NUL-terminated */
    uint16_t name_len;                          /*!< Name length in octets (0–120) */
} ble_ots_client_object_name_read_t;

/**
 * @brief Object Type read event data.
 */
typedef struct {
    int status;                                 /*!< 0 on success, ATT error on failure */
    uint8_t uuid[16];                           /*!< UUID value (2 or 16 octets, LE) */
    uint8_t uuid_len;                           /*!< 2 for 16-bit, 16 for 128-bit UUID */
} ble_ots_client_object_type_read_t;

/**
 * @brief Object Size read event data.
 */
typedef struct {
    int status;                                 /*!< 0 on success, ATT error on failure */
    uint32_t current_size;                      /*!< Actual object size in octets */
    uint32_t allocated_size;                    /*!< Allocated size in octets */
} ble_ots_client_object_size_read_t;

/**
 * @brief Date-time read event data (First-Created / Last-Modified).
 */
typedef struct {
    int status;                                 /*!< 0 on success, ATT error on failure */
    ble_ots_date_time_t datetime;               /*!< Parsed date-time value */
} ble_ots_client_datetime_read_t;

/**
 * @brief Object ID read event data.
 */
typedef struct {
    int status;                                 /*!< 0 on success, ATT error on failure */
    uint64_t object_id;                         /*!< UINT48 Object ID (lower 48 bits) */
} ble_ots_client_object_id_read_t;

/**
 * @brief Object Properties read event data.
 */
typedef struct {
    int status;                                 /*!< 0 on success, ATT error on failure */
    uint32_t properties;                        /*!< ble_ots_obj_property_t bit-field */
} ble_ots_client_properties_read_t;

/**
 * @brief Metadata identifier for write events.
 */
typedef enum {
    BLE_OTS_CLIENT_METADATA_OBJECT_NAME = 0,   /*!< Object Name */
    BLE_OTS_CLIENT_METADATA_FIRST_CREATED,     /*!< Object First-Created */
    BLE_OTS_CLIENT_METADATA_LAST_MODIFIED,     /*!< Object Last-Modified */
    BLE_OTS_CLIENT_METADATA_PROPERTIES,        /*!< Object Properties */
} ble_ots_client_metadata_type_t;

/**
 * @brief Metadata written event data.
 */
typedef struct {
    int status;                                 /*!< 0 on success, ATT error on failure */
    ble_ots_client_metadata_type_t metadata_type; /*!< Which metadata was written */
} ble_ots_client_metadata_written_t;

/**
 * @brief OLCP response event data.
 */
typedef struct {
    uint8_t request_opcode;                     /*!< Original OLCP Op Code (0x01–0x08) */
    uint8_t result_code;                        /*!< OLCP result code */
    uint32_t num_objects;                       /*!< Valid only for Request Number of Objects + Success */
} ble_ots_client_olcp_response_t;

/**
 * @brief Filter set event data.
 */
typedef struct {
    uint8_t instance;                           /*!< Filter instance index (0, 1, 2) */
    int status;                                 /*!< 0 on success, ATT error on failure */
} ble_ots_client_filter_set_t;

/**
 * @brief Filter read event data.
 */
typedef struct {
    uint8_t instance;                           /*!< Filter instance index (0, 1, 2) */
    int status;                                 /*!< 0 on success, ATT error on failure */
    uint8_t filter_type;                        /*!< One of ble_ots_list_filter_type_t */
    const uint8_t *param;                       /*!< Filter parameter bytes (valid during callback), NULL if none */
    uint16_t param_len;                         /*!< Length of param in octets (0 if none) */
} ble_ots_client_filter_read_t;

/**
 * @brief Object Changed event data.
 */
typedef struct {
    uint8_t source_of_change;                   /*!< 0 = Server, 1 = another Client */
    bool contents_changed;                      /*!< true if object contents changed */
    bool metadata_changed;                      /*!< true if object metadata changed */
    bool object_created;                        /*!< true if a new object was created */
    bool object_deleted;                        /*!< true if an object was deleted */
    uint64_t object_id;                         /*!< UINT48 Object ID (lower 48 bits) */
} ble_ots_client_object_changed_t;

/**
 * @brief OACP response event data.
 */
typedef struct {
    uint8_t request_opcode;                     /*!< Original OACP Op Code (0x01–0x07) */
    uint8_t result_code;                        /*!< OACP result code */
    uint32_t checksum;                          /*!< CRC-32 (valid only for Calculate Checksum + Success) */
    bool has_checksum;                          /*!< true if checksum field is valid */
} ble_ots_client_oacp_response_t;

/**
 * @brief Channel open event data.
 */
typedef struct {
    uint16_t conn_id;                           /*!< Connection identifier */
    uint16_t channel_id;                        /*!< L2CAP channel identifier */
    uint16_t mtu;                               /*!< Negotiated L2CAP MTU */
    uint16_t mps;                               /*!< Negotiated MPS (LE Credit Based only) */
} ble_ots_client_channel_info_t;

/**
 * @brief Channel closed event data.
 */
typedef struct {
    uint16_t conn_id;                           /*!< Connection identifier */
    uint8_t reason;                             /*!< 0 = local close, nonzero = remote/link loss */
} ble_ots_client_channel_closed_t;

/**
 * @brief Data received event data.
 */
typedef struct {
    const uint8_t *data;                        /*!< Pointer to received data chunk */
    uint16_t data_len;                          /*!< Length of received data chunk */
    uint32_t offset;                            /*!< Logical byte offset in the object */
} ble_ots_client_data_received_t;

/**
 * @brief Data sent event data.
 */
typedef struct {
    uint16_t data_len;                          /*!< Octets acknowledged as sent */
    uint32_t remaining;                         /*!< Remaining octets to send */
} ble_ots_client_data_sent_t;

/**
 * @brief Transfer timeout event data.
 */
typedef struct {
    uint16_t conn_id;                           /*!< Connection identifier */
    uint8_t opcode;                             /*!< OACP Op Code (0 = data-transfer inactivity) */
} ble_ots_client_transfer_timeout_t;

/*****************************************************************************
 * Callback Type
 *****************************************************************************/

/**
 * @brief Application event callback type.
 *
 * All OTS client events from all modules are delivered through this callback.
 * The callback is invoked from the BLE host task context; the application must
 * not block.
 *
 * @param conn_id Connection identifier
 * @param event   One of ble_ots_client_event_t values
 * @param param   Event-specific parameter structure (valid during callback only)
 */
typedef void (*ble_ots_client_event_cb_t)(uint16_t conn_id,
                                          ble_ots_client_event_t event,
                                          const void *param);

/*****************************************************************************
 * Public APIs — Discovery & Metadata (ble_ots_client_discovery.c / ble_ots_client_metadata.c)
 *****************************************************************************/

/**
 * @brief Initialize the OTS client module.
 *
 * Allocates internal resources and registers the application event callback.
 * Must be called before any other OTS client API. Call ble_ots_client_deinit()
 * first to re-initialize.
 *
 * @param callback Application event callback function
 * @return 0 on success, error code on failure
 */
int ble_ots_client_init(ble_ots_client_event_cb_t callback);

/**
 * @brief Deinitialize the OTS client module.
 *
 * Releases all resources (per-connection contexts, timers), unregisters the
 * callback, and resets internal state.
 *
 * @return 0 on success, error code on failure
 */
int ble_ots_client_deinit(void);

/**
 * @brief Discover the OTS service on a connected server.
 *
 * Discovers all service characteristics and descriptors, and auto-configures
 * OACP/OLCP CCCDs for indications. Result reported via
 * BLE_OTS_CLIENT_EVT_DISCOVER_COMPLETE.
 *
 * @param conn_id BLE connection identifier
 * @return 0 on success (discovery started), error code on failure
 */
int ble_ots_client_discover_service(uint16_t conn_id);

/**
 * @brief Read the OTS Feature characteristic (Feature Discovery).
 *
 * The result is cached and reported via BLE_OTS_CLIENT_EVT_FEATURE_READ.
 *
 * @param conn_id BLE connection identifier
 * @return 0 on success (read initiated), error code on failure
 */
int ble_ots_client_read_feature(uint16_t conn_id);

/**
 * @brief Read the Object Name of the Current Object.
 *
 * Uses Read Long if needed. Result via BLE_OTS_CLIENT_EVT_OBJECT_NAME_READ.
 *
 * @param conn_id BLE connection identifier
 * @return 0 on success (read initiated), error code on failure
 */
int ble_ots_client_read_object_name(uint16_t conn_id);

/**
 * @brief Read the Object Type UUID of the Current Object.
 *
 * Result via BLE_OTS_CLIENT_EVT_OBJECT_TYPE_READ.
 *
 * @param conn_id BLE connection identifier
 * @return 0 on success (read initiated), error code on failure
 */
int ble_ots_client_read_object_type(uint16_t conn_id);

/**
 * @brief Read the Object Size of the Current Object.
 *
 * Result via BLE_OTS_CLIENT_EVT_OBJECT_SIZE_READ.
 *
 * @param conn_id BLE connection identifier
 * @return 0 on success (read initiated), error code on failure
 */
int ble_ots_client_read_object_size(uint16_t conn_id);

/**
 * @brief Read the First-Created date-time of the Current Object.
 *
 * Result via BLE_OTS_CLIENT_EVT_FIRST_CREATED_READ.
 *
 * @param conn_id BLE connection identifier
 * @return 0 on success (read initiated), error code on failure
 */
int ble_ots_client_read_object_first_created(uint16_t conn_id);

/**
 * @brief Read the Last-Modified date-time of the Current Object.
 *
 * Result via BLE_OTS_CLIENT_EVT_LAST_MODIFIED_READ.
 *
 * @param conn_id BLE connection identifier
 * @return 0 on success (read initiated), error code on failure
 */
int ble_ots_client_read_object_last_modified(uint16_t conn_id);

/**
 * @brief Read the Object ID of the Current Object.
 *
 * Result via BLE_OTS_CLIENT_EVT_OBJECT_ID_READ.
 *
 * @param conn_id BLE connection identifier
 * @return 0 on success (read initiated), error code on failure
 */
int ble_ots_client_read_object_id(uint16_t conn_id);

/**
 * @brief Read the Object Properties of the Current Object.
 *
 * Result via BLE_OTS_CLIENT_EVT_PROPERTIES_READ.
 *
 * @param conn_id BLE connection identifier
 * @return 0 on success (read initiated), error code on failure
 */
int ble_ots_client_read_object_properties(uint16_t conn_id);

/**
 * @brief Write a new name for the Current Object.
 *
 * Uses Write Long if name exceeds ATT_MTU - 3. Result via
 * BLE_OTS_CLIENT_EVT_METADATA_WRITTEN with METADATA_OBJECT_NAME.
 *
 * @param conn_id  BLE connection identifier
 * @param name     Pointer to UTF-8 encoded name buffer, need not be NUL-terminated
 * @param name_len Length of name in octets (1–120)
 * @return 0 on success (write initiated), error code on failure
 */
int ble_ots_client_write_object_name(uint16_t conn_id,
                                     const char *name,
                                     uint16_t name_len);

/**
 * @brief Write the First-Created date-time of the Current Object.
 *
 * Result via BLE_OTS_CLIENT_EVT_METADATA_WRITTEN with METADATA_FIRST_CREATED.
 *
 * @param conn_id  BLE connection identifier
 * @param datetime Pointer to date-time value to write
 * @return 0 on success (write initiated), error code on failure
 */
int ble_ots_client_write_object_first_created(uint16_t conn_id,
                                              const ble_ots_date_time_t *datetime);

/**
 * @brief Write the Last-Modified date-time of the Current Object.
 *
 * Result via BLE_OTS_CLIENT_EVT_METADATA_WRITTEN with METADATA_LAST_MODIFIED.
 *
 * @param conn_id  BLE connection identifier
 * @param datetime Pointer to date-time value to write
 * @return 0 on success (write initiated), error code on failure
 */
int ble_ots_client_write_object_last_modified(uint16_t conn_id,
                                              const ble_ots_date_time_t *datetime);

/**
 * @brief Write the Object Properties of the Current Object.
 *
 * Result via BLE_OTS_CLIENT_EVT_METADATA_WRITTEN with METADATA_PROPERTIES.
 *
 * @param conn_id    BLE connection identifier
 * @param properties ble_ots_obj_property_t bit-field value
 * @return 0 on success (write initiated), error code on failure
 */
int ble_ots_client_write_object_properties(uint16_t conn_id,
                                           uint32_t properties);

/*****************************************************************************
 * Public APIs — Object Navigation (ble_ots_client_object_nav.c)
 *****************************************************************************/

/**
 * @brief Navigate to the first object (OLCP First, 0x01).
 *
 * Result via BLE_OTS_CLIENT_EVT_OLCP_RESPONSE.
 *
 * @param conn_id Connection identifier
 * @return 0 on success, error code on failure
 */
int ble_ots_client_first_object(uint16_t conn_id);

/**
 * @brief Navigate to the last object (OLCP Last, 0x02).
 *
 * Result via BLE_OTS_CLIENT_EVT_OLCP_RESPONSE.
 *
 * @param conn_id Connection identifier
 * @return 0 on success, error code on failure
 */
int ble_ots_client_last_object(uint16_t conn_id);

/**
 * @brief Navigate to the previous object (OLCP Previous, 0x03).
 *
 * Result via BLE_OTS_CLIENT_EVT_OLCP_RESPONSE.
 *
 * @param conn_id Connection identifier
 * @return 0 on success, error code on failure
 */
int ble_ots_client_prev_object(uint16_t conn_id);

/**
 * @brief Navigate to the next object (OLCP Next, 0x04).
 *
 * Result via BLE_OTS_CLIENT_EVT_OLCP_RESPONSE.
 *
 * @param conn_id Connection identifier
 * @return 0 on success, error code on failure
 */
int ble_ots_client_next_object(uint16_t conn_id);

/**
 * @brief Select an object by Object ID (OLCP Go To, 0x05).
 *
 * Result via BLE_OTS_CLIENT_EVT_OLCP_RESPONSE.
 *
 * @param conn_id   Connection identifier
 * @param object_id UINT48 Object ID (lower 48 bits used)
 * @return 0 on success, error code on failure
 */
int ble_ots_client_goto_object(uint16_t conn_id, uint64_t object_id);

/**
 * @brief Re-order the object list (OLCP Order, 0x06).
 *
 * Result via BLE_OTS_CLIENT_EVT_OLCP_RESPONSE.
 *
 * @param conn_id    Connection identifier
 * @param sort_order One of ble_ots_list_sort_order_t values
 * @return 0 on success, error code on failure
 */
int ble_ots_client_order_objects(uint16_t conn_id, uint8_t sort_order);

/**
 * @brief Request number of objects matching current filters (OLCP 0x07).
 *
 * Result via BLE_OTS_CLIENT_EVT_OLCP_RESPONSE with num_objects.
 *
 * @param conn_id Connection identifier
 * @return 0 on success, error code on failure
 */
int ble_ots_client_request_num_objects(uint16_t conn_id);

/**
 * @brief Clear marking on all filtered objects (OLCP 0x08).
 *
 * Result via BLE_OTS_CLIENT_EVT_OLCP_RESPONSE.
 *
 * @param conn_id Connection identifier
 * @return 0 on success, error code on failure
 */
int ble_ots_client_clear_marking(uint16_t conn_id);

/**
 * @brief Write an Object List Filter value to a filter instance.
 *
 * Result via BLE_OTS_CLIENT_EVT_FILTER_SET.
 *
 * @param conn_id     Connection identifier
 * @param instance    Filter instance index (0, 1, or 2)
 * @param filter_type One of ble_ots_list_filter_type_t values
 * @param param       Filter parameter data (NULL for No Filter / Marked Objects)
 * @param param_len   Length of param in octets
 * @return 0 on success, error code on failure
 */
int ble_ots_client_set_filter(uint16_t conn_id,
                              uint8_t instance,
                              uint8_t filter_type,
                              const uint8_t *param,
                              uint16_t param_len);

/**
 * @brief Read the Object List Filter value of a filter instance.
 *
 * Reads back the filter type and parameter currently configured on the given
 * filter instance. Uses Read Long automatically when the value exceeds
 * ATT_MTU - 1. Result via BLE_OTS_CLIENT_EVT_FILTER_READ.
 *
 * @param conn_id  Connection identifier
 * @param instance Filter instance index (0, 1, or 2)
 * @return 0 on success (read initiated), error code on failure
 */
int ble_ots_client_read_filter(uint16_t conn_id, uint8_t instance);

/**
 * @brief Enable or disable Object Changed indications.
 *
 * When enabled, BLE_OTS_CLIENT_EVT_OBJECT_CHANGED events are received.
 *
 * @param conn_id Connection identifier
 * @param enable  true to enable, false to disable
 * @return 0 on success, error code on failure
 */
int ble_ots_client_subscribe_object_changed(uint16_t conn_id, bool enable);

/*****************************************************************************
 * Public APIs — Transfer (ble_ots_client_transfer.c)
 *****************************************************************************/

/**
 * @brief Create a new object on the server (OACP Create, 0x01).
 *
 * Result via BLE_OTS_CLIENT_EVT_OACP_RESPONSE.
 *
 * @param conn_id       Connection identifier
 * @param size          Allocated size in octets
 * @param type_uuid     Pointer to Object Type UUID (2 or 16 octets)
 * @param type_uuid_len Length of the type UUID: 2 or 16
 * @return 0 on success, error code on failure
 */
int ble_ots_client_create_object(uint16_t conn_id,
                                 uint32_t size,
                                 const uint8_t *type_uuid,
                                 uint8_t type_uuid_len);

/**
 * @brief Delete the Current Object (OACP Delete, 0x02).
 *
 * Result via BLE_OTS_CLIENT_EVT_OACP_RESPONSE.
 *
 * @param conn_id Connection identifier
 * @return 0 on success, error code on failure
 */
int ble_ots_client_delete_object(uint16_t conn_id);

/**
 * @brief Execute the Current Object (OACP Execute, 0x04).
 *
 * Result via BLE_OTS_CLIENT_EVT_OACP_RESPONSE.
 *
 * @param conn_id Connection identifier
 * @return 0 on success, error code on failure
 */
int ble_ots_client_execute_object(uint16_t conn_id);

/**
 * @brief Initiate reading object contents (OACP Read, 0x05).
 *
 * Data arrives via BLE_OTS_CLIENT_EVT_DATA_RECEIVED. The Object Transfer
 * Channel must already be open.
 *
 * @param conn_id Connection identifier
 * @param offset  Zero-based byte offset to start reading
 * @param length  Number of octets to read; MUST be non-zero. The transfer
 *                channel has no end-of-data marker, so the client tracks
 *                completion by counting octets against this value.
 * @return 0 on success, BLE_HS_EINVAL if length is 0, error code on failure
 */
int ble_ots_client_read_object_content(uint16_t conn_id,
                                       uint32_t offset,
                                       uint32_t length);

/**
 * @brief Initiate writing object contents (OACP Write, 0x06).
 *
 * After OACP Success, send data via ble_ots_client_send_data(). The Object
 * Transfer Channel must already be open.
 *
 * @param conn_id Connection identifier
 * @param offset  Zero-based byte offset to start writing
 * @param length  Number of octets to write; MUST be non-zero
 * @param mode    Write mode bit-field (bit 1 = Truncate)
 * @return 0 on success, BLE_HS_EINVAL if length is 0, error code on failure
 */
int ble_ots_client_write_object_content(uint16_t conn_id,
                                        uint32_t offset,
                                        uint32_t length,
                                        uint8_t mode);

/**
 * @brief Send a chunk of object data over the Object Transfer Channel.
 *
 * Called repeatedly during OACP Write until all declared octets are sent.
 *
 * @param conn_id  Connection identifier
 * @param data     Pointer to the object data to send
 * @param data_len Length of the data chunk in octets
 * @return 0 on success, error code on failure
 */
int ble_ots_client_send_data(uint16_t conn_id,
                             const uint8_t *data,
                             uint16_t data_len);

/**
 * @brief Request a CRC-32 checksum (OACP Calculate Checksum, 0x03).
 *
 * Result via BLE_OTS_CLIENT_EVT_OACP_RESPONSE with checksum.
 *
 * @param conn_id Connection identifier
 * @param offset  Zero-based byte offset
 * @param length  Number of octets to checksum
 * @return 0 on success, error code on failure
 */
int ble_ots_client_calculate_checksum(uint16_t conn_id,
                                      uint32_t offset,
                                      uint32_t length);

/**
 * @brief Abort an in-progress OACP Read transfer (OACP Abort, 0x07).
 *
 * Result via BLE_OTS_CLIENT_EVT_OACP_RESPONSE. Per OTS v1.0 section 4.4 this
 * may be issued while the server is still processing another OACP procedure,
 * so it is not rejected with BLE_HS_EBUSY the way the other OACP commands are.
 *
 * @param conn_id Connection identifier
 * @return 0 on success, error code on failure
 */
int ble_ots_client_abort_transfer(uint16_t conn_id);

/**
 * @brief Open an L2CAP Object Transfer Channel (PSM_OTS 0x0025).
 *
 * Result via BLE_OTS_CLIENT_EVT_CHANNEL_OPEN or BLE_OTS_CLIENT_EVT_CHANNEL_CLOSED.
 *
 * @param conn_id Connection identifier
 * @return 0 on success, error code on failure
 */
int ble_ots_client_open_channel(uint16_t conn_id);

/**
 * @brief Close the Object Transfer Channel.
 *
 * Result via BLE_OTS_CLIENT_EVT_CHANNEL_CLOSED.
 *
 * @param conn_id Connection identifier
 * @return 0 on success, error code on failure
 */
int ble_ots_client_close_channel(uint16_t conn_id);

#ifdef __cplusplus
}
#endif

#endif /* BLE_OTS_CLIENT_H */
