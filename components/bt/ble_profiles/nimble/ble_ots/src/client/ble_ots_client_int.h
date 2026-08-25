/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef BLE_OTS_CLIENT_INT_H
#define BLE_OTS_CLIENT_INT_H

#include <stdint.h>
#include <stdbool.h>
#include "host/ble_gap.h"
#include "host/ble_l2cap.h"
#include "os/os_mbuf.h"
#include "nimble/nimble_npl.h"
#include "ble_ots_client.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Constants
 *****************************************************************************/
#define BLE_OTS_CLIENT_MAX_CONNECTIONS      3  /**< Max simultaneous OTS client connections */

/*****************************************************************************
 * Per-Connection Client Context
 *****************************************************************************/

/**
 * @brief Per-connection OTS client context.
 *
 * One instance per active connection. Created on successful discovery,
 * destroyed on disconnect or deinit.
 */
typedef struct {
    uint16_t conn_id;                           /*!< BLE connection identifier */
    bool     in_use;                            /*!< true if this context slot is allocated */
    ble_ots_client_char_handles_t handles;      /*!< Discovered characteristic handle set */
    ble_ots_feature_t feature;                  /*!< Cached OTS Feature value */
    bool     feature_valid;                     /*!< true after Feature has been read */
    bool     server_supports_read_long;         /*!< Initially true; false if server returns Not Supported */
    bool     server_supports_write_long;        /*!< Initially true; false if server returns Not Supported */
    bool     multi_object_server;               /*!< true if OLCP was discovered */

    /* Control-point timeout management */
    struct ble_npl_callout cp_timer;            /*!< Control-point response timer (host-task callout) */
    bool     cp_timer_inited;                   /*!< true once cp_timer callout is initialised */
    bool     cp_timer_active;                   /*!< true while CP timer is running */
    bool     cp_timed_out;                      /*!< true if last CP operation timed out */
    uint8_t  cp_pending_opcode;                 /*!< Opcode of pending CP operation; also a synchronous
                                                     busy flag that serialises control-point procedures */

    /* Discovery state */
    uint16_t disc_svc_start_handle;             /*!< OTS service start handle */
    uint16_t disc_svc_end_handle;               /*!< OTS service end handle */
    uint8_t  disc_state;                        /*!< Internal discovery state machine step */

    /* L2CAP Object Transfer Channel state (used by transfer module) */
    struct ble_l2cap_chan *otc_chan;             /*!< L2CAP channel pointer, NULL if not open */
    bool     otc_open;                          /*!< true if OTC is established */
    uint8_t  otc_retry_count;                   /*!< OTC connect retries attempted (ENOTSUP backoff) */
    struct ble_npl_callout otc_retry_timer;     /*!< One-shot backoff callout for OTC connect retry */
    bool     otc_retry_timer_inited;            /*!< true once otc_retry_timer callout is initialised */

    /* Transfer tracking (used by transfer module) */
    struct ble_npl_callout transfer_timer;      /*!< Data transfer inactivity callout (host-task) */
    bool     transfer_timer_inited;             /*!< true once transfer_timer callout is initialised */
    bool     transfer_timer_active;             /*!< true while transfer timer is running */
    uint32_t transfer_offset;                   /*!< Read/write current byte offset */
    uint32_t transfer_length;                   /*!< Total read/write length */
    uint32_t transfer_received;                 /*!< Bytes received so far (read) */
    uint32_t transfer_sent;                     /*!< Bytes sent so far (write) */
    bool     transfer_in_progress;              /*!< true during active data transfer */
    uint8_t  transfer_opcode;                   /*!< OACP opcode for active transfer */

    /* Deferred OACP-response dispatch for Write success */
    struct ble_npl_event oacp_dispatch_ev;      /*!< Host-task event to defer OACP_RESPONSE dispatch */
    bool     oacp_dispatch_ev_ready;            /*!< true once oacp_dispatch_ev has been initialised */
    ble_ots_client_oacp_response_t pending_oacp_resp; /*!< Response payload for the deferred dispatch */

    /* Name read buffer (used by metadata module for Read Long) */
    char     name_buf[CONFIG_BLE_OTS_CLIENT_MAX_NAME_LEN]; /*!< Object name (UTF-8, not NUL-terminated) */
    uint16_t name_buf_len;                      /*!< Current bytes accumulated in name_buf */
} ble_ots_client_conn_ctx_t;

/*****************************************************************************
 * Module State
 *****************************************************************************/

/**
 * @brief Global OTS client module state.
 *
 * Allocated dynamically by ble_ots_client_init(), freed by ble_ots_client_deinit().
 */
typedef struct {
    ble_ots_client_event_cb_t app_cb;           /*!< Registered application callback */
    struct ble_gap_event_listener gap_listener; /*!< GAP listener for connect/disconnect events */
    ble_ots_client_conn_ctx_t conns[BLE_OTS_CLIENT_MAX_CONNECTIONS]; /*!< Per-connection contexts */
    bool initialized;                           /*!< true after successful init */
} ble_ots_client_state_t;

/**
 * @brief Global module state pointer (owned by ble_ots_client_discovery.c).
 */
extern ble_ots_client_state_t *g_ots_client;

/* ---- ble_ots_client_discovery.c ---- */

/**
 * @brief Retrieve the per-connection client context.
 *
 * @param conn_id BLE connection identifier
 * @return Pointer to context, or NULL if not found
 */
ble_ots_client_conn_ctx_t *ble_ots_client_get_conn_ctx(uint16_t conn_id);

/**
 * @brief Dispatch an event to the application callback.
 *
 * @param conn_id Connection identifier (for logging)
 * @param event   Event code
 * @param param   Event-specific parameter structure
 */
void ble_ots_client_dispatch_event(uint16_t conn_id,
                                   ble_ots_client_event_t event,
                                   const void *param);

/**
 * @brief Start a control-point response timer.
 *
 * If the timer expires, dispatches BLE_OTS_CLIENT_EVT_CP_TIMEOUT or
 * BLE_OTS_CLIENT_EVT_TRANSFER_TIMEOUT and sets cp_timed_out = true.
 *
 * @param conn_id    BLE connection identifier
 * @param timeout_ms Timeout period in milliseconds
 * @return 0 on success, error code on failure
 */
int ble_ots_client_start_cp_timer(uint16_t conn_id, uint32_t timeout_ms);

/**
 * @brief Stop a running control-point response timer.
 *
 * Safe to call when no timer is running (no-op).
 *
 * @param conn_id BLE connection identifier
 */
void ble_ots_client_stop_cp_timer(uint16_t conn_id);

/**
 * @brief Remove and free a per-connection context (called on disconnect).
 *
 * @param conn_id BLE connection identifier
 */
void ble_ots_client_remove_conn_ctx(uint16_t conn_id);

/* ---- ble_ots_client_object_nav.c ---- */

/**
 * @brief GAP event handler for OLCP and Object Changed indications.
 *
 * Routes BLE_GAP_EVENT_NOTIFY_RX events for OLCP and Object Changed
 * characteristic handles to the appropriate indication parsers.
 *
 * @param event GAP event
 * @param arg   Unused
 * @return 0
 */
int ble_ots_client_object_nav_gap_event(struct ble_gap_event *event, void *arg);

/* ---- ble_ots_client_transfer.c ---- */

/**
 * @brief Handle an OACP indication received from the server.
 *
 * Parses the OACP Response Code indication, stops the CP timer, and
 * dispatches BLE_OTS_CLIENT_EVT_OACP_RESPONSE. For Read/Write Success,
 * sets up transfer tracking state.
 *
 * @param conn_handle Connection handle
 * @param attr_handle Attribute handle of the OACP characteristic
 * @param om          Indication payload
 */
void ble_ots_client_handle_oacp_indication(uint16_t conn_handle,
                                           uint16_t attr_handle,
                                           struct os_mbuf *om);

#ifdef __cplusplus
}
#endif

#endif /* BLE_OTS_CLIENT_INT_H */
