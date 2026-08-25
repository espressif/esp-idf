/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef BLE_OTS_SERVER_INT_H
#define BLE_OTS_SERVER_INT_H

#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "host/ble_hs.h"
#include "nimble/nimble_npl.h"
#include "ble_ots_common.h"
#include "ble_ots_server.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Internal Constants
 *****************************************************************************/

/** @brief Invalid connection handle sentinel */
#define BLE_OTS_CONN_ID_NONE    0xFFFF

/** @brief Maximum number of filter instances per connection */
#define BLE_OTS_FILTER_INSTANCE_COUNT   3

/** @brief Maximum DLO content buffer size (estimated worst-case) */
#define BLE_OTS_DLO_MAX_BUF_SIZE        4096

/** @brief Maximum serialized length of an Object List Filter characteristic value. */
#define BLE_OTS_FILTER_VALUE_MAX_LEN    (1 + BLE_OTS_OBJECT_NAME_MAX_LEN)

/* ATT error codes defined by the Supplement to the Bluetooth Core
 * Specification (CSS), Part B "Common Profile and Service Error Codes".
 * NimBLE's ble_att.h only covers the core ATT codes (up to 0x13). */

/** @brief Client Characteristic Configuration Descriptor Improperly Configured */
#define ATT_ERR_CCCD_IMPROPERLY_CONFIGURED  0xFD

/** @brief Procedure Already in Progress */
#define ATT_ERR_PROC_ALREADY_IN_PROGRESS    0xFE

/*****************************************************************************
 * Internal Data Structures — ots_server_init.c
 *****************************************************************************/

/**
 * @brief Object database entry — a single object in the object store.
 */
typedef struct {
    bool in_use;                                 /*!< Whether this slot is occupied */
    ble_ots_obj_id_t object_id;                  /*!< 48-bit Object ID */
    char name[BLE_OTS_OBJECT_NAME_MAX_LEN];      /*!< Object name (UTF-8, not NUL-terminated) */
    uint8_t name_len;                            /*!< Length of name in octets */
    uint8_t type_uuid_len;                       /*!< 2 or 16 */
    uint8_t type_uuid[16];                       /*!< Object type UUID (little-endian) */
    uint32_t current_size;                       /*!< Actual content size in octets */
    uint32_t allocated_size;                     /*!< Allocated storage size */
    ble_ots_date_time_t first_created;           /*!< First-Created timestamp */
    ble_ots_date_time_t last_modified;           /*!< Last-Modified timestamp */
    uint32_t properties;                         /*!< Object properties bit-field */
    uint8_t *data;                               /*!< Object content data buffer */
    uint16_t locked_by;                          /*!< Connection holding lock, or BLE_OTS_CONN_ID_NONE */
    uint32_t marked;                             /*!< Per-bond marking bitmask */
} ble_ots_server_obj_t;

/** @brief Deferred indication request (opaque, defined in ots_server_init.c) */
struct ots_indicate_ctx;

/**
 * @brief Per-connection state managed by the concurrency pool.
 */
typedef struct {
    bool in_pool;                                /*!< Whether connection is in concurrency pool */
    uint16_t conn_id;                            /*!< BLE connection identifier */
    ble_ots_obj_id_t current_object_id;          /*!< Current Object ID for this connection */
    bool current_obj_valid;                      /*!< false = Invalid Object state */
    ble_ots_list_filter_t filter[BLE_OTS_FILTER_INSTANCE_COUNT]; /*!< Per-connection filters */
    struct ble_l2cap_chan *otc_chan;              /*!< L2CAP OTC channel, or NULL */
    bool transfer_active;                        /*!< Transfer (read or write) in progress */
    bool transfer_is_read;                       /*!< true = read transfer, false = write */
    ble_ots_obj_id_t transfer_object_id;         /*!< Object ID being transferred */
    uint32_t transfer_offset;                    /*!< Transfer start offset */
    uint32_t transfer_length;                    /*!< Total transfer length requested */
    uint32_t transfer_bytes_done;                /*!< Bytes transferred so far */
    bool transfer_truncated;                     /*!< Write transfer applied an OACP truncation */
    struct ble_npl_callout transfer_timer;       /*!< Inactivity timeout callout (NimBLE task context) */
    bool transfer_timer_inited;                  /*!< Whether transfer_timer has been initialized */
    struct ble_npl_callout transfer_retry_timer; /*!< Read-transfer back-off callout (NimBLE task context) */
    bool transfer_retry_timer_inited;            /*!< Whether transfer_retry_timer has been initialized */
    uint16_t transfer_retry_count;               /*!< Consecutive back-off retries of the current chunk */
    struct ble_npl_event transfer_data_ev;       /*!< Deferred read-data send event (NimBLE task context) */
    bool transfer_data_ev_inited;                /*!< Whether transfer_data_ev has been initialized */
    ble_ots_obj_id_t created_obj_ids[CONFIG_BLE_OTS_SERVER_MAX_OBJECTS]; /*!< Objects created this session */
    uint8_t created_obj_count;                   /*!< Number of objects in created_obj_ids */
    ble_ots_list_sort_order_t sort_order;        /*!< Current sort order for this connection */

    bool olcp_indicate_subscribed;               /*!< OLCP CCCD indication subscription state */
    bool oacp_subscribed;                        /*!< OACP CCCD indication subscription state */
    bool obj_changed_subscribed;                 /*!< Object Changed CCCD indication subscription state */

    /* ATT allows only one outstanding indication per connection, so queued
     * indications are drained one at a time as confirmations arrive. */
    struct ots_indicate_ctx *indicate_head;      /*!< Oldest queued indication, or NULL */
    struct ots_indicate_ctx *indicate_tail;      /*!< Newest queued indication, or NULL */
    bool indicate_in_flight;                     /*!< An indication is awaiting confirmation */
} ble_ots_server_conn_state_t;

/**
 * @brief OTS server global control block — aggregated internal state.
 */
typedef struct {
    bool initialized;                                        /*!< Server initialized */
    bool started;                                            /*!< Server started */
    ble_ots_server_config_t config;                          /*!< Copy of configuration */
    ble_ots_server_cb_t app_cb;                              /*!< Application callback */
    ble_ots_feature_t ots_feature;                           /*!< OTS Feature value */
    ble_ots_server_obj_t obj_db[CONFIG_BLE_OTS_SERVER_MAX_OBJECTS]; /*!< Object database */
    ble_ots_obj_id_t next_obj_id;                            /*!< Next Object ID to allocate */
    ble_ots_server_conn_state_t conn_pool[CONFIG_BLE_OTS_SERVER_MAX_CONCURRENCY]; /*!< Concurrency pool */
    uint8_t *dlo_content;                                    /*!< Directory Listing Object content buffer */
    uint32_t dlo_content_size;                               /*!< Current DLO content size */
} ble_ots_server_cb_env_t;

/**
 * @brief Global OTS server control block (defined in ots_server_init.c).
 */
extern ble_ots_server_cb_env_t *p_ble_ots_server_env;

/** @brief Control block accessor — only valid when BLE_OTS_SERVER_ENV_OK() */
#define ble_ots_server_env (*p_ble_ots_server_env)

/** @brief Whether the control block is currently allocated */
#define BLE_OTS_SERVER_ENV_OK() (p_ble_ots_server_env != NULL)

/**
 * @brief Acquire the OTS server mutex.
 *
 * Guards the object database, the object content buffers and the connection
 * pool, all of which are reached from both the NimBLE host task and the
 * application task. The mutex is recursive, so nesting is safe. Composite
 * read-modify-write sequences over that state must hold it for their whole
 * duration, not just around the individual accessors they call.
 */
void ble_ots_server_lock(void);

/**
 * @brief Release the OTS server mutex acquired with ble_ots_server_lock().
 */
void ble_ots_server_unlock(void);

/*****************************************************************************
 * Internal Interfaces — ots_server_init.c
 *****************************************************************************/

/**
 * @brief Create a new empty object in the database.
 *
 * @param type Object type UUID entry
 * @param size Initial allocated size
 * @return Newly allocated Object ID, or 0 on failure (DB full)
 */
ble_ots_obj_id_t ble_ots_server_obj_db_create(const ble_ots_obj_type_entry_t *type,
                                                uint32_t size);

/**
 * @brief Delete an object from the database by Object ID.
 *
 * @param object_id Object ID to delete (DLO cannot be deleted)
 * @return 0 on success, non-zero on failure
 */
int ble_ots_server_obj_db_delete(ble_ots_obj_id_t object_id);

/**
 * @brief Look up an object by Object ID.
 *
 * @param object_id Object ID to look up
 * @return Pointer to the object structure, or NULL if not found
 */
ble_ots_server_obj_t *ble_ots_server_obj_db_lookup(ble_ots_obj_id_t object_id);

/**
 * @brief Read object content data into a buffer.
 *
 * @param object_id Target object ID
 * @param offset    Byte offset within the object
 * @param length    Number of octets to read
 * @param buf       Destination buffer (must be >= length octets)
 * @return 0 on success, non-zero on failure
 */
int ble_ots_server_obj_data_read(ble_ots_obj_id_t object_id, uint32_t offset,
                                  uint32_t length, uint8_t *buf);

/**
 * @brief Write object content data into the database.
 *
 * @param object_id Target object ID
 * @param offset    Byte offset within the object
 * @param data      Source data buffer
 * @param length    Number of octets to write
 * @return 0 on success, non-zero on failure
 */
int ble_ots_server_obj_data_write(ble_ots_obj_id_t object_id, uint32_t offset,
                                   const uint8_t *data, uint32_t length);

/**
 * @brief Get the Current Object ID for a connection.
 *
 * @param conn_id BLE connection identifier
 * @return Current Object ID, or BLE_OTS_OBJ_ID_INVALID if invalid
 */
ble_ots_obj_id_t ble_ots_server_current_obj_get(uint16_t conn_id);

/**
 * @brief Set the Current Object for a connection.
 *
 * @param conn_id   BLE connection identifier
 * @param object_id Object ID to set (BLE_OTS_OBJ_ID_INVALID to invalidate)
 * @return 0 on success, non-zero on failure
 */
int ble_ots_server_current_obj_set(uint16_t conn_id, ble_ots_obj_id_t object_id);

/**
 * @brief Check if a connection is within the concurrency pool.
 *
 * @param conn_id BLE connection identifier
 * @return true if within pool, false if exceeded
 */
bool ble_ots_server_concurrency_check(uint16_t conn_id);

/**
 * @brief Lock an object for exclusive write transfer.
 *
 * @param object_id Object ID to lock
 * @param conn_id   Connection requesting the lock
 * @return 0 on success, non-zero if already locked by another connection
 */
int ble_ots_server_obj_lock(ble_ots_obj_id_t object_id, uint16_t conn_id);

/**
 * @brief Unlock an object after transfer completion.
 *
 * @param object_id Object ID to unlock
 * @return 0 on success, non-zero if not locked
 */
int ble_ots_server_obj_unlock(ble_ots_obj_id_t object_id);

/**
 * @brief Get the L2CAP OTC channel handle for a connection.
 *
 * @param conn_id BLE connection identifier
 * @return L2CAP channel pointer, or NULL if none open
 */
struct ble_l2cap_chan *ble_ots_server_otc_get(uint16_t conn_id);

/**
 * @brief Close the L2CAP OTC channel for a connection.
 *
 * @param conn_id BLE connection identifier
 * @return 0 on success, non-zero on failure
 */
int ble_ots_server_otc_close(uint16_t conn_id);

/**
 * @brief Send a GATT indication on the specified characteristic.
 *
 * The indication mbuf is built synchronously (so @p data may be on the
 * caller's stack), but the actual transmission is deferred to a NimBLE
 * host-task event. This keeps indications from being sent inside a GATT
 * access callback. A return of 0 therefore means "successfully queued".
 *
 * @param conn_id   BLE connection identifier
 * @param char_uuid Characteristic UUID (OACP/OLCP/Object Changed)
 * @param data      Indication payload
 * @param len       Length of data in octets
 * @return 0 if queued successfully, non-zero on failure
 */
int ble_ots_server_indicate_response(uint16_t conn_id, uint16_t char_uuid,
                                      const uint8_t *data, uint16_t len);

/**
 * @brief Get the per-connection state for a given connection ID.
 *
 * @param conn_id BLE connection identifier
 * @return Pointer to connection state, or NULL if not found
 */
ble_ots_server_conn_state_t *ble_ots_server_conn_state_get(uint16_t conn_id);

/**
 * @brief Get the OACP characteristic value handle.
 *
 * @return OACP value handle, or 0 if the service has never been registered
 */
uint16_t ble_ots_server_oacp_handle_get(void);

/**
 * @brief Get the OLCP characteristic value handle.
 *
 * @return OLCP value handle, or 0 if the service has never been registered
 */
uint16_t ble_ots_server_olcp_handle_get(void);

/**
 * @brief Dispatch an event to the application callback.
 *
 * @param event Event type
 * @param param Event parameter union
 */
void ble_ots_server_dispatch_event(ble_ots_server_event_t event,
                                    ble_ots_server_cb_param_t *param);

/**
 * @brief Start the transfer inactivity timer for a connection.
 *
 * @param conn_id BLE connection identifier
 */
void ble_ots_server_transfer_timer_start(uint16_t conn_id);

/**
 * @brief Stop the transfer inactivity timer for a connection.
 *
 * @param conn_id BLE connection identifier
 */
void ble_ots_server_transfer_timer_stop(uint16_t conn_id);

/**
 * @brief Reset the transfer inactivity timer for a connection.
 *
 * @param conn_id BLE connection identifier
 */
void ble_ots_server_transfer_timer_reset(uint16_t conn_id);

/**
 * @brief Check whether the given object name is unique across the object DB.
 *
 * @param name     Name to check
 * @param name_len Length of name
 * @param exclude_id Object ID to exclude from the check (or BLE_OTS_OBJ_ID_INVALID)
 * @return true if name is unique, false if duplicate exists
 */
bool ble_ots_server_name_is_unique(const char *name, uint8_t name_len,
                                    ble_ots_obj_id_t exclude_id);

/*****************************************************************************
 * Internal Interfaces — ots_server_filter_changed.c
 *****************************************************************************/

/**
 * @brief Filtered list result for OLCP navigation.
 */
typedef struct {
    uint32_t count;                                          /*!< Number of matching objects */
    ble_ots_obj_id_t object_ids[CONFIG_BLE_OTS_SERVER_MAX_OBJECTS]; /*!< Object IDs passing filters */
} ble_ots_server_filtered_list_t;

/**
 * @brief Allocate the filter/changed sub-module context.
 *
 * Must be called once on the OTS server init path before any other
 * filter/changed API is used.  Idempotent.
 *
 * @return 0 on success, BLE_HS_ENOMEM on allocation failure
 */
int ble_ots_server_filter_changed_init(void);

/**
 * @brief Free the filter/changed sub-module context.
 *
 * Called on the OTS server deinit path.  Safe to call when never allocated.
 */
void ble_ots_server_filter_changed_deinit(void);

/**
 * @brief Get the filtered and sorted object list for a connection.
 *
 * Evaluates all three filter instances with AND logic against the object DB.
 *
 * @param conn_id Connection identifier
 * @return Pointer to the reusable filtered list, or NULL on error
 */
ble_ots_server_filtered_list_t *ble_ots_server_filter_get_list(uint16_t conn_id);

/**
 * @brief Dispatch Object Changed indication to subscribed clients.
 *
 * @param object_id     Object ID that changed (must not be DLO)
 * @param flags         Change flags (BLE_OTS_OBJ_CHANGED_FLAG_*)
 * @param source_conn_id Connection that caused the change, or BLE_OTS_CONN_ID_NONE for server
 */
void ble_ots_server_dispatch_obj_changed(ble_ots_obj_id_t object_id, uint8_t flags,
                                          uint16_t source_conn_id);

/**
 * @brief Reset all three filter instances for a connection to No Filter.
 *
 * @param conn_id Connection identifier
 */
void ble_ots_server_filter_reset(uint16_t conn_id);

/**
 * @brief Handle a client write to an Object List Filter instance.
 *
 * @param conn_id      Connection identifier
 * @param instance_idx Filter instance index (0, 1, or 2)
 * @param data         Raw write data (filter type + parameters)
 * @param length       Length of write data
 * @return 0 on success, ATT error code on failure
 */
int ble_ots_server_filter_write(uint16_t conn_id, uint8_t instance_idx,
                                 const uint8_t *data, uint16_t length);

/**
 * @brief Handle a client read of an Object List Filter instance.
 *
 * @param conn_id      Connection identifier
 * @param instance_idx Filter instance index (0, 1, or 2)
 * @param buf          Output buffer
 * @param buf_len      In: buffer capacity; Out: actual length written
 * @return 0 on success, non-zero on failure
 */
int ble_ots_server_filter_read(uint16_t conn_id, uint8_t instance_idx,
                                uint8_t *buf, uint16_t *buf_len);

/**
 * @brief Rebuild the Directory Listing Object content.
 *
 * Iterates over all objects in the database and serializes them into DLO records.
 */
void ble_ots_server_dlo_rebuild(void);

/**
 * @brief Set or clear the Mark property for an object on a per-bond basis.
 *
 * @param conn_id   Connection identifier (for bond determination)
 * @param object_id Object ID to mark/unmark
 * @param mark      true to mark, false to unmark
 * @return 0 on success, non-zero on failure
 */
int ble_ots_server_mark_object(uint16_t conn_id, ble_ots_obj_id_t object_id, bool mark);

/**
 * @brief Clean up marked-object state on disconnection.
 *
 * For non-bonded peers, clears the marked bits from all objects and frees the
 * bond-index slot.  For bonded peers, the slot and marked bits are preserved
 * so that marked state persists across reconnections.
 *
 * Takes the peer identity rather than a connection handle: NimBLE deletes the
 * connection object before delivering BLE_GAP_EVENT_DISCONNECT, so
 * ble_gap_conn_find() can no longer resolve the peer at this point.  Pass the
 * fields of the connection descriptor carried by the disconnect event
 * (event->disconnect.conn).
 *
 * @param peer_id_addr Peer identity address of the disconnecting peer
 * @param bonded       Whether the peer was bonded at disconnection time
 */
void ble_ots_server_mark_cleanup_on_disconnect(const ble_addr_t *peer_id_addr, bool bonded);

/*****************************************************************************
 * Internal Interfaces — ots_server_oacp_transfer.c
 *****************************************************************************/

/**
 * @brief Handle OACP Read procedure.
 *
 * @param conn_id Connection identifier
 * @param offset  Byte offset within the Current Object
 * @param length  Number of octets to read and send
 * @return 0 if accepted, negative error code otherwise
 */
int ble_ots_server_oacp_read_proc(uint16_t conn_id, uint32_t offset, uint32_t length);

/**
 * @brief Handle OACP Write procedure.
 *
 * @param conn_id Connection identifier
 * @param offset  Byte offset within the Current Object
 * @param length  Total octets the client intends to send
 * @param mode    Write mode bit-field
 * @return 0 if accepted, negative error code otherwise
 */
int ble_ots_server_oacp_write_proc(uint16_t conn_id, uint32_t offset,
                                    uint32_t length, uint8_t mode);

/**
 * @brief Handle OACP Abort procedure.
 *
 * @param conn_id Connection identifier
 * @return 0 if accepted, negative error code otherwise
 */
int ble_ots_server_oacp_abort_proc(uint16_t conn_id);

/*****************************************************************************
 * Internal Interfaces — ots_server_olcp.c
 *****************************************************************************/

/**
 * @brief Handle a BLE_GAP_EVENT_SUBSCRIBE for OLCP CCCD tracking.
 *
 * Should be called from the GAP event callback when a subscribe event is
 * received. Updates the per-connection OLCP indication subscription state.
 *
 * @param conn_handle  BLE connection handle
 * @param attr_handle  Attribute handle from the subscribe event
 * @param cur_indicate Current indication subscription state (1 = subscribed)
 */
void ble_ots_server_olcp_handle_subscribe(uint16_t conn_handle,
                                           uint16_t attr_handle,
                                           bool cur_indicate);

/**
 * @brief Check if the object database contains any objects.
 *
 * @return true if at least one object slot is in use, false otherwise
 */
bool ble_ots_server_obj_db_has_objects(void);

#ifdef __cplusplus
}
#endif

#endif /* BLE_OTS_SERVER_INT_H */
