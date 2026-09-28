/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_OPP_API_H__
#define __ESP_OPP_API_H__

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
#include "esp_bt_defs.h"
#include "esp_gap_bt_api.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_OPP_INVALID_HANDLE              0           /*!< Invalid OPP connection handle */
#define ESP_OPP_MAX_NAME_LEN                255         /*!< Maximum object name length (ASCII bytes) */
#define ESP_OPP_MAX_TYPE_LEN                64          /*!< Maximum MIME type length */

#define ESP_OPP_MTU_MIN                     255         /*!< Minimal MTU can be used in OPP connection */
#define ESP_OPP_MTU_MAX                     1691        /*!< Maximum MTU can be used in OPP connection */

/**
 * @brief OPP supported formats for SDP (GOEP Supported Formats List).
 */
#define ESP_OPP_FORMAT_VCARD_2_1            0x01        /*!< vCard 2.1 */
#define ESP_OPP_FORMAT_VCARD_3_0            0x02        /*!< vCard 3.0 */
#define ESP_OPP_FORMAT_VCAL_1_0             0x03        /*!< vCal 1.0 */
#define ESP_OPP_FORMAT_ICAL_2_0             0x04        /*!< iCal 2.0 */
#define ESP_OPP_FORMAT_VNOTE                0x05        /*!< vNote */
#define ESP_OPP_FORMAT_VMESSAGE             0x06        /*!< vMessage */
#define ESP_OPP_FORMAT_ANY                  0xFF        /*!< Any type of object */

/**
 * @brief OPP profile error code.
 */
#define ESP_BT_STATUS_OPP_NOT_FOUND     (ESP_BT_STATUS_BASE_FOR_OPP_ERR + 0)  /*!< OPP: requested object/connection not found */
#define ESP_BT_STATUS_OPP_ABORTED       (ESP_BT_STATUS_BASE_FOR_OPP_ERR + 1)  /*!< OPP: transfer aborted (local cancel or response timeout) */
#define ESP_BT_STATUS_OPP_SDP_FAIL      (ESP_BT_STATUS_BASE_FOR_OPP_ERR + 2)  /*!< OPP: operation failed during SDP */
#define ESP_BT_STATUS_OPP_OBEX_FAIL     (ESP_BT_STATUS_BASE_FOR_OPP_ERR + 3)  /*!< OPP: operation failed during OBEX/GOEP */
#define ESP_BT_STATUS_OPP_FORBIDDEN     (ESP_BT_STATUS_BASE_FOR_OPP_ERR + 4)  /*!< OPP: operation refused (OBEX Forbidden) */

typedef uint16_t esp_opp_conn_hdl_t;                     /*!< OPP connection handle */

/**
 * @brief OPP server callback events
 */
typedef enum {
    ESP_OPP_SERVER_INIT_EVT = 0,            /*!< When OPP server is initialized, the event comes */
    ESP_OPP_SERVER_DEINIT_EVT,              /*!< When OPP server is deinitialized, the event comes */
    ESP_OPP_SERVER_START_EVT,               /*!< When OPP server starts listening, the event comes */
    ESP_OPP_SERVER_STOP_EVT,                /*!< When OPP server stops listening, the event comes */
    ESP_OPP_SERVER_CONNECTION_STATE_EVT,    /*!< When inbound OPP connection state changes, the event comes */
    ESP_OPP_SERVER_INCOMING_OBJECT_EVT,     /*!< When a peer starts pushing an object, the event comes */
    ESP_OPP_SERVER_ACCEPT_EVT,              /*!< When esp_opp_server_accept completes, the event comes */
    ESP_OPP_SERVER_PROGRESS_EVT,            /*!< When incoming object transfer progress updates, the event comes */
    ESP_OPP_SERVER_TRANSFER_COMPLETE_EVT,   /*!< When an incoming object transfer finishes, the event comes */
} esp_opp_server_cb_event_t;

/**
 * @brief OPP client callback events
 */
typedef enum {
    ESP_OPP_CLIENT_INIT_EVT = 0,            /*!< When OPP client is initialized, the event comes */
    ESP_OPP_CLIENT_DEINIT_EVT,              /*!< When OPP client is deinitialized, the event comes */
    ESP_OPP_CLIENT_CONNECTION_STATE_EVT,    /*!< When outbound OPP connection state changes, the event comes */
    ESP_OPP_CLIENT_OPEN_EVT,                /*!< When esp_opp_client_open_object completes, the event comes */
    ESP_OPP_CLIENT_PROGRESS_EVT,            /*!< When outgoing object transfer progress updates, the event comes */
    ESP_OPP_CLIENT_TRANSFER_COMPLETE_EVT,   /*!< When an outgoing object transfer finishes, the event comes */
} esp_opp_client_cb_event_t;

/**
 * @brief OPP connection state
 */
typedef enum {
    ESP_OPP_DISCONNECTED,                   /*!< Connection closed */
    ESP_OPP_CONNECTED,                      /*!< Connection established */
} esp_opp_connection_state_t;

/**
 * @brief OPP profile status parameters
 */
typedef struct {
    bool opp_server_inited;                 /*!< OPP server initialization */
    bool opp_client_inited;                 /*!< OPP client initialization */
    uint8_t conn_num;                       /*!< Number of connections */
} esp_opp_profile_status_t;

/**
 * @brief OPP server start configuration parameters
 */
typedef struct {
    esp_bt_sec_t sec_mask;                  /*!< Security setting mask.
                                                 @note Suggest using one of:
                                                       - ESP_BT_SEC_NONE
                                                       - ESP_BT_SEC_AUTHENTICATE
                                                       - (ESP_BT_SEC_AUTHENTICATE | ESP_BT_SEC_ENCRYPT) */
    uint16_t mtu;                           /*!< Preferred OBEX packet length, range: ESP_OPP_MTU_MIN ～ ESP_OPP_MTU_MAX; 0 means transport default */
    const char *service_name;               /*!< SDP service name; NULL uses stack default */
    const uint8_t *supported_formats;       /*!< Supported formats list for SDP; NULL means any type */
    uint8_t supported_formats_len;          /*!< Number of bytes in supported_formats */
    bool auto_accept;                       /*!< true: accept every object automatically.
                                             *  false: accept once per OBEX connection (first object
                                             *  needs esp_opp_server_accept; later objects on the same
                                             *  connection are auto-accepted) */
} esp_opp_server_cfg_t;

/**
 * @brief OPP client connect parameters
 */
typedef struct {
    esp_bd_addr_t bd_addr;                  /*!< Remote device Bluetooth address */
    esp_bt_sec_t sec_mask;                  /*!< Security setting mask.
                                                 @note Suggest using one of:
                                                       - ESP_BT_SEC_NONE
                                                       - ESP_BT_SEC_AUTHENTICATE
                                                       - (ESP_BT_SEC_AUTHENTICATE | ESP_BT_SEC_ENCRYPT) */
    uint16_t mtu;                           /*!< Preferred OBEX packet length, range: ESP_OPP_MTU_MIN ～ ESP_OPP_MTU_MAX; 0 means transport default */
} esp_opp_client_connect_param_t;

/**
 * @brief OPP client object open parameters
 *
 * After ESP_OPP_CLIENT_OPEN_EVT reports success, write object body bytes with
 * write(fd), then close(fd) to finish the object (End-of-Body). The number of
 * bytes written must equal len.
 */
typedef struct {
    esp_opp_conn_hdl_t handle;              /*!< Connection handle from CONNECTION_STATE_EVT */
    const char *name;                       /*!< Object name; ASCII is encoded as OBEX UTF-16BE */
    const char *type;                       /*!< MIME type, for example text/x-vcard; may be NULL */
    uint32_t len;                           /*!< Declared object length (OBEX Length header) */
} esp_opp_client_object_cfg_t;

/**
 * @brief OPP server callback parameters
 */
typedef union {
    /**
     * @brief ESP_OPP_SERVER_INIT_EVT
     */
    struct opp_server_init_evt_param {
        esp_bt_status_t status;            /*!< status */
    } init;                                 /*!< OPP server callback param of ESP_OPP_SERVER_INIT_EVT */

    /**
     * @brief ESP_OPP_SERVER_DEINIT_EVT
     */
    struct opp_server_deinit_evt_param {
        esp_bt_status_t status;            /*!< status */
    } deinit;                               /*!< OPP server callback param of ESP_OPP_SERVER_DEINIT_EVT */

    /**
     * @brief ESP_OPP_SERVER_START_EVT
     */
    struct opp_server_start_evt_param {
        esp_bt_status_t status;            /*!< status */
        uint8_t scn;                        /*!< RFCOMM server channel; 0 if start failed */
    } start;                                /*!< OPP server callback param of ESP_OPP_SERVER_START_EVT */

    /**
     * @brief ESP_OPP_SERVER_STOP_EVT
     */
    struct opp_server_stop_evt_param {
        esp_bt_status_t status;            /*!< status */
    } stop;                                 /*!< OPP server callback param of ESP_OPP_SERVER_STOP_EVT */

    /**
     * @brief ESP_OPP_SERVER_CONNECTION_STATE_EVT
     */
    struct opp_server_conn_evt_param {
        esp_opp_conn_hdl_t handle;          /*!< Connection handle */
        esp_opp_connection_state_t state;   /*!< Connection state */
        esp_bt_status_t status;            /*!< status */
        esp_bd_addr_t bd_addr;              /*!< Peer Bluetooth device address */
    } conn;                                 /*!< OPP server callback param of ESP_OPP_SERVER_CONNECTION_STATE_EVT */

    /**
     * @brief ESP_OPP_SERVER_INCOMING_OBJECT_EVT
     */
    struct opp_server_incoming_object_evt_param {
        esp_opp_conn_hdl_t handle;          /*!< Connection handle */
        const char *name;                   /*!< Object name; valid only during the callback */
        const char *type;                   /*!< Object MIME type; may be NULL */
        uint32_t len;                       /*!< Declared object length; 0 if unknown */
        int fd;                             /*!< VFS fd for reading the object body; -1 if needs_accept
                                             *  (fd is delivered in ESP_OPP_SERVER_ACCEPT_EVT) */
        bool needs_accept;                  /*!< true: call esp_opp_server_accept/reject;
                                             *  false: already authorized, read(fd) directly */
    } incoming;                             /*!< OPP server callback param of ESP_OPP_SERVER_INCOMING_OBJECT_EVT */

    /**
     * @brief ESP_OPP_SERVER_ACCEPT_EVT
     */
    struct opp_server_accept_evt_param {
        esp_bt_status_t status;            /*!< status */
        esp_opp_conn_hdl_t handle;          /*!< Connection handle */
        int fd;                             /*!< VFS read file descriptor; -1 on failure */
    } accept;                               /*!< OPP server callback param of ESP_OPP_SERVER_ACCEPT_EVT */

    /**
     * @brief ESP_OPP_SERVER_PROGRESS_EVT
     */
    struct opp_server_progress_evt_param {
        esp_opp_conn_hdl_t handle;          /*!< Connection handle */
        uint32_t transferred;               /*!< Bytes received so far */
        uint32_t total;                     /*!< Declared total length; 0 if unknown */
    } progress;                             /*!< OPP server callback param of ESP_OPP_SERVER_PROGRESS_EVT */

    /**
     * @brief ESP_OPP_SERVER_TRANSFER_COMPLETE_EVT
     */
    struct opp_server_complete_evt_param {
        esp_opp_conn_hdl_t handle;          /*!< Connection handle */
        esp_bt_status_t status;            /*!< Transfer result */
        uint32_t transferred;               /*!< Bytes received when transfer ended */
    } complete;                             /*!< OPP server callback param of ESP_OPP_SERVER_TRANSFER_COMPLETE_EVT */
} esp_opp_server_param_t;

/**
 * @brief OPP client callback parameters
 */
typedef union {
    /**
     * @brief ESP_OPP_CLIENT_INIT_EVT
     */
    struct opp_client_init_evt_param {
        esp_bt_status_t status;            /*!< status */
    } init;                                 /*!< OPP client callback param of ESP_OPP_CLIENT_INIT_EVT */

    /**
     * @brief ESP_OPP_CLIENT_DEINIT_EVT
     */
    struct opp_client_deinit_evt_param {
        esp_bt_status_t status;            /*!< status */
    } deinit;                               /*!< OPP client callback param of ESP_OPP_CLIENT_DEINIT_EVT */

    /**
     * @brief ESP_OPP_CLIENT_CONNECTION_STATE_EVT
     */
    struct opp_client_conn_evt_param {
        esp_opp_conn_hdl_t handle;          /*!< Connection handle */
        esp_opp_connection_state_t state;   /*!< Connection state */
        esp_bt_status_t status;            /*!< status */
        esp_bd_addr_t bd_addr;              /*!< Peer Bluetooth device address */
    } conn;                                 /*!< OPP client callback param of ESP_OPP_CLIENT_CONNECTION_STATE_EVT */

    /**
     * @brief ESP_OPP_CLIENT_OPEN_EVT
     */
    struct opp_client_open_evt_param {
        esp_bt_status_t status;            /*!< status */
        esp_opp_conn_hdl_t handle;          /*!< Connection handle */
        int fd;                             /*!< VFS write file descriptor; -1 on failure */
    } open;                                 /*!< OPP client callback param of ESP_OPP_CLIENT_OPEN_EVT */

    /**
     * @brief ESP_OPP_CLIENT_PROGRESS_EVT
     */
    struct opp_client_progress_evt_param {
        esp_opp_conn_hdl_t handle;          /*!< Connection handle */
        uint32_t transferred;               /*!< Bytes sent so far */
        uint32_t total;                     /*!< Declared total length */
    } progress;                             /*!< OPP client callback param of ESP_OPP_CLIENT_PROGRESS_EVT */

    /**
     * @brief ESP_OPP_CLIENT_TRANSFER_COMPLETE_EVT
     */
    struct opp_client_complete_evt_param {
        esp_opp_conn_hdl_t handle;          /*!< Connection handle */
        esp_bt_status_t status;            /*!< Transfer result */
        uint32_t transferred;               /*!< Bytes sent when transfer ended */
    } complete;                             /*!< OPP client callback param of ESP_OPP_CLIENT_TRANSFER_COMPLETE_EVT */
} esp_opp_client_param_t;

/**
 * @brief OPP server callback function type
 *
 * @param[in] event Event type
 * @param[in] param Pointer to callback parameter
 */
typedef void (*esp_opp_server_callback_t)(esp_opp_server_cb_event_t event, esp_opp_server_param_t *param);

/**
 * @brief OPP client callback function type
 *
 * @param[in] event Event type
 * @param[in] param Pointer to callback parameter
 */
typedef void (*esp_opp_client_callback_t)(esp_opp_client_cb_event_t event, esp_opp_client_param_t *param);

/**
 * @brief This function is called to register a user callback for OPP server.
 *
 * @param[in] callback: pointer to the user callback function.
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_opp_server_register_callback(esp_opp_server_callback_t callback);

/**
 * @brief This function is called to register a user callback for OPP client.
 *
 * @param[in] callback: pointer to the user callback function.
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_opp_client_register_callback(esp_opp_client_callback_t callback);

/**
 * @brief This function is called to initialize OPP server.
 *
 * Acquires the shared OPP VFS (read side). When the operation is completed, the
 * callback function will be called with ESP_OPP_SERVER_INIT_EVT.
 * This function should be called after esp_bluedroid_enable() and
 * esp_opp_server_register_callback() complete successfully.
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_opp_server_init(void);

/**
 * @brief This function is called to deinitialize OPP server.
 *
 * Active server connections are closed first. When the operation is completed,
 * the callback function will be called with ESP_OPP_SERVER_DEINIT_EVT.
 * This function should be called after esp_opp_server_init() completes successfully.
 *
 * Finish all OPP VFS read/write and close() every OPP fd before calling this
 * function.
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_opp_server_deinit(void);

/**
 * @brief This function creates an OPP server and starts listening for inbound connections.
 *
 * When the server is started successfully, the callback is called with ESP_OPP_SERVER_START_EVT.
 * When an inbound connection is established or released, the callback is called with
 * ESP_OPP_SERVER_CONNECTION_STATE_EVT.
 * This function should be called after esp_opp_server_init() completes successfully.
 *
 * @param[in] cfg: OPP server configuration.
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_opp_server_start(const esp_opp_server_cfg_t *cfg);

/**
 * @brief This function stops the OPP server.
 *
 * When the operation is completed, the callback function will be called with ESP_OPP_SERVER_STOP_EVT.
 * This function should be called after esp_opp_server_start() completes successfully.
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_opp_server_stop(void);

/**
 * @brief Accept a pending incoming object.
 *
 * Call this when ESP_OPP_SERVER_INCOMING_OBJECT_EVT reports needs_accept == true
 * (auto_accept is false and this is the first object on the connection).
 * The first PUT is held without an OBEX response until accept, reject, or the
 * stack accept timeout (~30s). Accept also authorizes further objects on the
 * same connection and unblocks body delivery from the peer.
 * When the operation is completed, the callback is called with ESP_OPP_SERVER_ACCEPT_EVT
 * (status, handle, fd). After a successful accept, read(fd) until EOF (0), then close(fd).
 *
 * @param[in] handle: Connection handle from ESP_OPP_SERVER_INCOMING_OBJECT_EVT.
 *
 * @return
 *                  - ESP_OK: request posted; result is in ESP_OPP_SERVER_ACCEPT_EVT
 *                  - other: failed
 */
esp_err_t esp_opp_server_accept(esp_opp_conn_hdl_t handle);

/**
 * @brief Reject a pending incoming object and close the connection.
 *
 * Sends OBEX Forbidden and cancels the whole batch on that connection
 * (subsequent objects are not received).
 *
 * @param[in] handle: Connection handle from ESP_OPP_SERVER_INCOMING_OBJECT_EVT.
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_opp_server_reject(esp_opp_conn_hdl_t handle);

/**
 * @brief Cancel the current incoming object transfer on the server connection.
 *
 * @param[in] handle: Connection handle.
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_opp_server_cancel(esp_opp_conn_hdl_t handle);

/**
 * @brief This function is called to initialize OPP client.
 *
 * Acquires the shared OPP VFS (write side). When the operation is completed, the
 * callback function will be called with ESP_OPP_CLIENT_INIT_EVT.
 * This function should be called after esp_bluedroid_enable() and
 * esp_opp_client_register_callback() complete successfully.
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_opp_client_init(void);

/**
 * @brief This function is called to deinitialize OPP client.
 *
 * Active client connections are closed first. When the operation is completed,
 * the callback function will be called with ESP_OPP_CLIENT_DEINIT_EVT.
 * This function should be called after esp_opp_client_init() completes successfully.
 *
 * Finish all OPP VFS read/write and close() every OPP fd before calling this
 * function.
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_opp_client_deinit(void);

/**
 * @brief This function connects OPP client to a remote OPP server.
 *
 * When the connection is established or failed, the callback is called with
 * ESP_OPP_CLIENT_CONNECTION_STATE_EVT.
 * This function should be called after esp_opp_client_init() completes successfully.
 *
 * @param[in] param: Connect parameters including peer address and security.
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_opp_client_connect(const esp_opp_client_connect_param_t *param);

/**
 * @brief This function disconnects the OPP client from the remote OPP server.
 *
 * When the operation is completed, the callback function will be called with
 * ESP_OPP_CLIENT_CONNECTION_STATE_EVT.
 * This function should be called after a successful connection.
 *
 * @param[in] handle: Connection handle from ESP_OPP_CLIENT_CONNECTION_STATE_EVT.
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_opp_client_disconnect(esp_opp_conn_hdl_t handle);

/**
 * @brief Open a VFS write fd for pushing one object on an OPP client connection.
 *
 * When the operation is completed, the callback is called with ESP_OPP_CLIENT_OPEN_EVT
 * (status, handle, fd). On success, write the object body with write(fd), then
 * close(fd) to send End-of-Body. The total bytes written must equal cfg->len.
 * Transfer completion is reported by ESP_OPP_CLIENT_TRANSFER_COMPLETE_EVT.
 *
 * After each OBEX CONNECT/PUT/DISCONNECT request, the stack waits up to ~30s for
 * the peer response (Continue/OK). No response aborts the transfer (if any) and
 * closes the connection. Waiting for the application to write(fd) does not
 * consume that timeout.
 *
 * @param[in] cfg: Object metadata and connection handle.
 *
 * @return
 *                  - ESP_OK: request posted; result is in ESP_OPP_CLIENT_OPEN_EVT
 *                  - other: failed
 */
esp_err_t esp_opp_client_open_object(const esp_opp_client_object_cfg_t *cfg);

/**
 * @brief Cancel the current outgoing object transfer on the client connection.
 *
 * @param[in] handle: Connection handle.
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_opp_client_cancel(esp_opp_conn_hdl_t handle);

/**
 * @brief This function is used to get the status of OPP
 *
 * @param[out] profile_status: OPP status
 *
 * @return
 *                  - ESP_OK: success
 *                  - other: failed
 */
esp_err_t esp_opp_get_profile_status(esp_opp_profile_status_t *profile_status);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_OPP_API_H__ */
