/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_BLE_L2CAP_API_H__
#define __ESP_BLE_L2CAP_API_H__

#include <stdint.h>
#include <stdbool.h>

#include "esp_err.h"
#include "esp_bt_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief LE L2CAP connection-oriented channel (CoC) callback events
 */
typedef enum {
    ESP_BLE_L2CAP_COC_CONNECTED_EVT = 0,      /*!< When an LE CoC channel is connected or the connection attempt fails, the event comes */
    ESP_BLE_L2CAP_COC_DISCONNECTED_EVT,         /*!< When an LE CoC channel is disconnected, the event comes */
    ESP_BLE_L2CAP_COC_ACCEPT_EVT,               /*!< When a remote device requests a new LE CoC connection to a local server, the event comes */
    ESP_BLE_L2CAP_COC_DATA_RECEIVED_EVT,        /*!< When a complete SDU is received on an LE CoC channel, the event comes */
    ESP_BLE_L2CAP_COC_TX_UNSTALLED_EVT,         /*!< When TX credits are restored and more data may be sent, the event comes */
    ESP_BLE_L2CAP_COC_RECONFIG_COMPLETED_EVT,   /*!< When a local channel reconfiguration request completes, the event comes */
    ESP_BLE_L2CAP_COC_PEER_RECONFIGURED_EVT,    /*!< When the peer completes a channel reconfiguration, the event comes */
    ESP_BLE_L2CAP_COC_EVT_MAX,
} esp_ble_l2cap_evt_t;

/**
 * @brief LE CoC channel information
 *
 * Delivered in `ESP_BLE_L2CAP_COC_CONNECTED_EVT` and reconfiguration events when the
 * operation succeeds.
 */
typedef struct {
    uint16_t scid;      /*!< Local channel identifier (CID) */
    uint16_t dcid;      /*!< Remote channel identifier (CID) */
    uint16_t psm;       /*!< Protocol/Service Multiplexer */
    uint16_t our_mtu;   /*!< Local maximum SDU size (MTU) */
    uint16_t peer_mtu;  /*!< Peer maximum SDU size (MTU) */
    uint16_t our_mps;   /*!< Local maximum PDU payload size (MPS) */
    uint16_t peer_mps;  /*!< Peer maximum PDU payload size (MPS) */
} esp_ble_l2cap_chan_info_t;

/**
 * @brief LE L2CAP CoC callback parameters union
 */
typedef union {
    /**
     * @brief ESP_BLE_L2CAP_COC_CONNECTED_EVT
     */
    struct {
        uint16_t conn_id;                       /*!< GATT connection id of the underlying ACL link. May be 0 if not yet bound */
        uint16_t chan_handle;                   /*!< Local L2CAP channel identifier (CID) of the CoC */
        uint16_t status;                        /*!< Connection result. 0 (`L2CAP_CONN_OK`) means success; other values are L2CAP connection result codes */
        esp_ble_l2cap_chan_info_t chan_info;    /*!< Channel information. Valid only when `status` is 0 (`L2CAP_CONN_OK`) */
    } coc_connected;                            /*!< LE L2CAP callback param of ESP_BLE_L2CAP_COC_CONNECTED_EVT */

    /**
     * @brief ESP_BLE_L2CAP_COC_DISCONNECTED_EVT
     */
    struct {
        uint16_t conn_id;                       /*!< Reserved. Currently not populated by the stack (0) */
        uint16_t chan_handle;                   /*!< Local L2CAP channel identifier (CID) of the disconnected CoC */
    } coc_disconnected;                         /*!< LE L2CAP callback param of ESP_BLE_L2CAP_COC_DISCONNECTED_EVT */

    /**
     * @brief ESP_BLE_L2CAP_COC_ACCEPT_EVT
     */
    struct {
        uint16_t conn_id;                       /*!< GATT connection id of the underlying ACL link. May be 0 if not yet bound */
        uint16_t chan_handle;                   /*!< Proposed local L2CAP channel identifier (CID) */
        uint8_t l2cap_id;                       /*!< L2CAP signaling identifier of the connection request */
        uint16_t psm;                           /*!< Protocol/Service Multiplexer requested by the peer */
    } coc_accept;                               /*!< LE L2CAP callback param of ESP_BLE_L2CAP_COC_ACCEPT_EVT */

    /**
     * @brief ESP_BLE_L2CAP_COC_DATA_RECEIVED_EVT
     */
    struct {
        uint16_t chan_handle;                   /*!< Local L2CAP channel identifier (CID) that received the SDU */
        uint16_t len;                           /*!< SDU length in bytes */
        uint8_t *data;                          /*!< Pointer to the received SDU payload. Valid only during the callback */
    } data_received;                            /*!< LE L2CAP callback param of ESP_BLE_L2CAP_COC_DATA_RECEIVED_EVT */

    /**
     * @brief ESP_BLE_L2CAP_COC_TX_UNSTALLED_EVT
     */
    struct {
        uint16_t chan_handle;                   /*!< Local L2CAP channel identifier (CID) whose TX path is no longer congested */
    } tx_unstalled;                             /*!< LE L2CAP callback param of ESP_BLE_L2CAP_COC_TX_UNSTALLED_EVT */

    /**
     * @brief ESP_BLE_L2CAP_COC_RECONFIG_COMPLETED_EVT
     */
    struct {
        uint16_t chan_handle;                   /*!< Local L2CAP channel identifier (CID) that was reconfigured */
        uint16_t status;                        /*!< Reconfiguration result. 0 (`L2CAP_LE_RECONFIG_OK`) means success */
        esp_ble_l2cap_chan_info_t chan_info;    /*!< Updated channel information. Valid only when `status` is 0 (`L2CAP_LE_RECONFIG_OK`) */
    } reconfig_completed;                       /*!< LE L2CAP callback param of ESP_BLE_L2CAP_COC_RECONFIG_COMPLETED_EVT */

    /**
     * @brief ESP_BLE_L2CAP_COC_PEER_RECONFIGURED_EVT
     */
    struct {
        uint16_t chan_handle;                   /*!< Local L2CAP channel identifier (CID) reconfigured by the peer */
        uint16_t status;                        /*!< Reconfiguration result. 0 (`L2CAP_LE_RECONFIG_OK`) means success */
        esp_ble_l2cap_chan_info_t chan_info;    /*!< Updated channel information. Valid only when `status` is 0 (`L2CAP_LE_RECONFIG_OK`) */
    } peer_reconfigured;                        /*!< LE L2CAP callback param of ESP_BLE_L2CAP_COC_PEER_RECONFIGURED_EVT */
} esp_ble_l2cap_cb_param_t;

/**
 * @brief       LE L2CAP CoC callback function type
 *
 * @param[in]   event:  Event type
 * @param[in]   param:  Pointer to callback parameter, currently is union type
 */
typedef void (*esp_ble_l2cap_cb_t)(esp_ble_l2cap_evt_t event, esp_ble_l2cap_cb_param_t *param);

/**
 * @brief       Register the LE L2CAP CoC callback function
 *
 * @param[in]   callback:  Pointer to the callback function
 *
 * @return
 *              - ESP_OK: success
 *              - ESP_ERR_INVALID_ARG: callback is NULL
 *              - ESP_FAIL: other error
 */
esp_err_t esp_ble_l2cap_register_callback(esp_ble_l2cap_cb_t callback);

/**
 * @brief       Initialize the LE L2CAP CoC module
 *
 *              Requires `CONFIG_BT_BLE_L2CAP_COC_ENABLED`.
 *              This function should be called after `esp_bluedroid_enable()` completes successfully.
 *
 * @return
 *              - ESP_OK: success
 *              - ESP_FAIL: other error
 */
esp_err_t esp_ble_l2cap_init(void);

/**
 * @brief       Deinitialize the LE L2CAP CoC module
 *
 *              Deregisters all local CoC servers created by this module.
 *              This function should be called after `esp_ble_l2cap_init()` completes successfully.
 *
 * @return
 *              - ESP_OK: success
 *              - ESP_FAIL: other error
 */
esp_err_t esp_ble_l2cap_deinit(void);

/**
 * @brief       Register a local LE CoC server on the given PSM
 *
 *              When a remote device requests a connection to this PSM, the callback receives
 *              `ESP_BLE_L2CAP_COC_ACCEPT_EVT`.
 *
 * @param[in]   psm:  LE Protocol/Service Multiplexer. Valid range is 0x0001 to 0x00FF
 * @param[in]   mtu:  Local maximum SDU size (MTU) for channels accepted on this PSM
 *
 * @return
 *              - ESP_OK: success
 *              - ESP_ERR_INVALID_ARG: invalid `psm` or `mtu`
 *              - ESP_FAIL: other error
 */
esp_err_t esp_ble_l2cap_create_server(uint16_t psm, uint16_t mtu);

/**
 * @brief       Deregister a local LE CoC server
 *
 * @param[in]   psm:  LE Protocol/Service Multiplexer previously registered with `esp_ble_l2cap_create_server()`
 *
 * @return
 *              - ESP_OK: success
 *              - ESP_ERR_INVALID_ARG: `psm` is 0
 *              - ESP_FAIL: other error
 */
esp_err_t esp_ble_l2cap_delete_server(uint16_t psm);

/**
 * @brief       Connect to a remote LE CoC server (client role)
 *
 *              When the connection attempt completes, the callback receives
 *              `ESP_BLE_L2CAP_COC_CONNECTED_EVT`.
 *
 * @param[in]   conn_id:  GATT connection id of the underlying ACL link
 * @param[in]   psm:      Remote LE Protocol/Service Multiplexer. Valid range is 0x0001 to 0x00FF
 * @param[in]   mtu:      Local maximum SDU size (MTU) to propose for the channel
 *
 * @return
 *              - ESP_OK: success
 *              - ESP_ERR_INVALID_ARG: invalid `psm` or `mtu`
 *              - ESP_FAIL: other error
 */
esp_err_t esp_ble_l2cap_connect(uint16_t conn_id, uint16_t psm, uint16_t mtu);

/**
 * @brief       Accept or reject an inbound LE CoC connection request (server role)
 *
 *              Call this function in response to `ESP_BLE_L2CAP_COC_ACCEPT_EVT`.
 *              When accepted, the callback receives `ESP_BLE_L2CAP_COC_CONNECTED_EVT`.
 *              When rejected, no `ESP_BLE_L2CAP_COC_CONNECTED_EVT` is reported to the local server.
 *
 * @param[in]   conn_id:      GATT connection id from `ESP_BLE_L2CAP_COC_ACCEPT_EVT`
 * @param[in]   l2cap_id:     L2CAP signaling identifier from `ESP_BLE_L2CAP_COC_ACCEPT_EVT`
 * @param[in]   chan_handle:  Proposed local channel identifier from `ESP_BLE_L2CAP_COC_ACCEPT_EVT`
 * @param[in]   accept:       True to accept the connection; false to reject it
 * @param[in]   mtu:          Local maximum SDU size (MTU) to use when accepting. Ignored when rejecting
 *
 * @return
 *              - ESP_OK: success
 *              - ESP_ERR_INVALID_ARG: `chan_handle` is 0
 *              - ESP_FAIL: other error
 */
esp_err_t esp_ble_l2cap_accept(uint16_t conn_id, uint8_t l2cap_id,
                               uint16_t chan_handle, bool accept, uint16_t mtu);

/**
 * @brief       Disconnect an LE CoC channel
 *
 *              When the channel is closed, the callback receives `ESP_BLE_L2CAP_COC_DISCONNECTED_EVT`.
 *
 * @param[in]   chan_handle:  Local L2CAP channel identifier (CID) of the CoC to disconnect
 *
 * @return
 *              - ESP_OK: success
 *              - ESP_ERR_INVALID_ARG: `chan_handle` is 0
 *              - ESP_FAIL: other error
 */
esp_err_t esp_ble_l2cap_disconnect(uint16_t chan_handle);

/**
 * @brief       Send an SDU on an LE CoC channel
 *
 *              Transmission is credit-based. The host accepts at most one SDU per
 *              channel in its TX queue; further calls return `ESP_OK` but the SDU
 *              may be dropped if the channel is busy. Retry on
 *              `ESP_BLE_L2CAP_COC_TX_UNSTALLED_EVT` or after the pipeline drains.
 *
 *              This function returns `ESP_OK` when the send request is queued to the host stack.
 *              It does not indicate that the SDU has already been transmitted.
 *
 * @param[in]   chan_handle:  Local L2CAP channel identifier (CID)
 * @param[in]   data:         Pointer to the SDU payload to send
 * @param[in]   len:          SDU length in bytes
 *
 * @return
 *              - ESP_OK: success
 *              - ESP_ERR_INVALID_ARG: invalid argument
 *              - ESP_FAIL: other error
 */
esp_err_t esp_ble_l2cap_send(uint16_t chan_handle, uint8_t *data, uint16_t len);

/**
 * @brief       Return RX credits after processing a received SDU (manual credit mode)
 *
 *              Call this function once after the application has finished handling the SDU delivered
 *              in `ESP_BLE_L2CAP_COC_DATA_RECEIVED_EVT`. The stack returns the exact number of RX
 *              credits that SDU consumed (a multi-frame SDU consumes more than one), so no credits
 *              are leaked regardless of how the SDU was fragmented.
 *
 *              This call only has an effect when the channel is in manual credit mode
 *              (`esp_ble_l2cap_set_auto_credit(chan_handle, false)`). In the default automatic mode
 *              the stack returns credits itself and this call is a harmless no-op. See
 *              `esp_ble_l2cap_set_auto_credit()` for the trade-offs between the two modes.
 *
 * @param[in]   chan_handle:  Local L2CAP channel identifier (CID)
 *
 * @return
 *              - ESP_OK: success
 *              - ESP_ERR_INVALID_ARG: `chan_handle` is 0
 *              - ESP_FAIL: other error
 */
esp_err_t esp_ble_l2cap_recv_ready(uint16_t chan_handle);

/**
 * @brief       Connect multiple LE CoC channels in one Enhanced Credit Flow Control request (client role)
 *
 *              Requires `CONFIG_BT_BLE_L2CAP_ENHANCED_COC`. The corresponding API symbols are
 *              available only when this option is enabled at build time.
 *              One `ESP_BLE_L2CAP_COC_CONNECTED_EVT` is reported per channel.
 *
 * @param[in]   conn_id:   GATT connection id of the underlying ACL link
 * @param[in]   psm:       Remote LE Protocol/Service Multiplexer. Valid range is 0x0001 to 0x00FF
 * @param[in]   mtu:       Local maximum SDU size (MTU) to propose for each channel
 * @param[in]   num_chan:  Number of CoC channels to open in a single request
 *
 * @return
 *              - ESP_OK: success
 *              - ESP_ERR_INVALID_ARG: invalid argument
 *              - ESP_FAIL: other error
 */
esp_err_t esp_ble_l2cap_connect_ecoc(uint16_t conn_id, uint16_t psm, uint16_t mtu, uint8_t num_chan);

/**
 * @brief       Reconfigure MTU and/or MPS on one or more LE CoC channels
 *
 *              Requires `CONFIG_BT_BLE_L2CAP_ENHANCED_COC`. The corresponding API symbols are
 *              available only when this option is enabled at build time.
 *              When the local request completes, the callback receives
 *              `ESP_BLE_L2CAP_COC_RECONFIG_COMPLETED_EVT` per channel.
 *
 * @param[in]   chan_handles:  Array of local L2CAP channel identifiers (CIDs) to reconfigure
 * @param[in]   num_chan:      Number of entries in `chan_handles`
 * @param[in]   mtu:           New local maximum SDU size (MTU)
 * @param[in]   mps:           New local maximum PDU payload size (MPS)
 *
 * @return
 *              - ESP_OK: success
 *              - ESP_ERR_INVALID_ARG: invalid argument
 *              - ESP_FAIL: other error
 */
esp_err_t esp_ble_l2cap_reconfig(uint16_t *chan_handles, uint8_t num_chan, uint16_t mtu, uint16_t mps);

/**
 * @brief       Select the RX credit return policy for an LE CoC channel
 *
 *              LE CoC flow control is credit based: one credit == one K-frame (an L2CAP PDU of
 *              up to MPS bytes). A single application SDU (up to MTU bytes) may be fragmented into
 *              several K-frames, so it consumes several RX credits. This function chooses how those
 *              consumed credits are returned to the peer.
 *
 *              Automatic mode (enable = true, the default):
 *              - Behaviour: the stack returns credits itself as each K-frame is consumed (returns
 *                are batched for efficiency and flushed as the window drains). In this mode
 *                `esp_ble_l2cap_recv_ready()` is a no-op and does not need to be called.
 *              - Pros: highest sustained RX throughput (credits are replenished on the Bluetooth
 *                task with no application round trip); no per-SDU bookkeeping for the application;
 *                works for any MTU/MPS, including SDUs larger than the credit window (credits are
 *                returned mid-SDU so reassembly can always complete).
 *              - Cons: no application-level backpressure. The peer keeps sending as fast as the
 *                credit window allows, regardless of how quickly the application drains the data.
 *                Recommended for throughput-oriented use and as the general default.
 *
 *              Manual mode (enable = false):
 *              - Behaviour: the stack withholds the consumed credits; the application returns them
 *                by calling `esp_ble_l2cap_recv_ready()` once after it has finished processing each
 *                SDU delivered in `ESP_BLE_L2CAP_COC_DATA_RECEIVED_EVT`. The stack tracks the exact
 *                number of K-frames each SDU consumed and returns that many credits per call, so a
 *                multi-frame SDU does not leak credits.
 *              - Pros: application-level backpressure. The peer's flow is gated by the application's
 *                processing pace (if `recv_ready()` is not called, the peer stalls once its credits
 *                run out), which is useful when the receiver has limited buffering.
 *              - Cons: lower sustained throughput than automatic mode, because each replenishment
 *                incurs an application-to-stack round trip.
 *
 *              Possible problem in manual mode (large SDUs):
 *              - Because credits are returned only after a complete SDU is delivered, a single SDU
 *                whose K-frame count exceeds the whole RX credit window (roughly when
 *                ceil((MTU + 2) / MPS) > window) can stall: the peer exhausts its credits before the
 *                SDU is complete, so the application never receives the event and never calls
 *                `recv_ready()`. The stack contains a deadlock breaker that returns the withheld
 *                credits mid-SDU in this situation so the transfer still completes (at the cost of
 *                weaker backpressure for that oversized SDU), and it logs a warning when manual mode
 *                is enabled on a channel where this can happen. For large MTUs prefer automatic mode
 *                or negotiate a larger MPS so a single SDU fits within the credit window.
 *
 * @param[in]   chan_handle:  Local L2CAP channel identifier (CID)
 * @param[in]   enable:       True to enable automatic credit return (default); false for manual
 *                            return via `esp_ble_l2cap_recv_ready()`
 *
 * @return
 *              - ESP_OK: success
 *              - ESP_ERR_INVALID_ARG: `chan_handle` is 0
 *              - ESP_FAIL: other error
 */
esp_err_t esp_ble_l2cap_set_auto_credit(uint16_t chan_handle, bool enable);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_BLE_L2CAP_API_H__ */
