/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_err.h"
#include "esp_bt_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Invalid PAN connection handle.
 *
 * Returned when a connection is not established. This value must not be passed
 * to PAN API functions that require a valid handle.
 */
#define ESP_PAN_INVALID_HANDLE          0xFFFF

/**
 * @brief Maximum Ethernet payload length accepted by esp_pan_write().
 *
 * Matches the fixed PAN write buffer capacity after BT_HDR and protocol header
 * overhead: PAN_BUF_SIZE(4112) - sizeof(BT_HDR)(8) - PAN_MINIMUM_OFFSET(28).
 */
#define ESP_PAN_MAX_WRITE_LEN           4076

/**
 * @brief Maximum number of protocol filter ranges accepted by esp_pan_set_protocol_filters().
 *
 * Must match BNEP_MAX_PROT_FILTERS.
 */
#define ESP_PAN_MAX_PROTOCOL_FILTERS    5

/**
 * @brief Maximum number of multicast filter ranges accepted by esp_pan_set_multicast_filters().
 *
 * Must match BNEP_MAX_MULTI_FILTERS.
 */
#define ESP_PAN_MAX_MULTICAST_FILTERS   5

/**
 * @brief Ethernet protocol type for IPv4 over PAN/BNEP.
 */
#define ESP_PAN_PROTO_IP                0x0800

/**
 * @brief Ethernet protocol type for ARP over PAN/BNEP.
 */
#define ESP_PAN_PROTO_ARP               0x0806

/**
 * @brief PAN role type.
 *
 * Each role is a single bit. Combine values with bitwise OR into an
 * esp_pan_role_mask_t when registering multiple local roles at initialization.
 *
 * Typical connection role pairs:
 * - PANU (client)  -> NAP (server)
 * - PANU (client)  -> GN  (server)
 * - GN   (client)  -> GN  (server)
 * - NAP  (server)  <- PANU (client, incoming connection)
 */
typedef enum {
    ESP_PAN_ROLE_PANU = 0x01,           /*!< PAN User role. Connects to NAP or GN for network access. */
    ESP_PAN_ROLE_GN   = 0x02,           /*!< Group Network role. Bridges multiple PAN devices in a subnet. */
    ESP_PAN_ROLE_NAP  = 0x04,           /*!< Network Access Point role. Provides network access to PANU peers. */
} esp_pan_role_t;

/**
 * @brief Bitmap of esp_pan_role_t values for local role registration.
 */
typedef uint8_t esp_pan_role_mask_t;

/* Security Setting Mask
 *
 * Use these combinations on both sides:
 * 1. ESP_PAN_SEC_NONE
 * 2. ESP_PAN_SEC_AUTHENTICATE
 * 3. (ESP_PAN_SEC_AUTHENTICATE | ESP_PAN_SEC_ENCRYPT)
 */
#define ESP_PAN_SEC_NONE            0x0000    /*!< No security. */
#define ESP_PAN_SEC_AUTHENTICATE    0x0012    /*!< Authentication required. */
#define ESP_PAN_SEC_ENCRYPT         0x0024    /*!< Encryption required. */
typedef uint16_t esp_pan_sec_t;               /*!< PAN security type. */

/**
 * @brief PAN status type.
 */
typedef enum {
    ESP_PAN_SUCCESS = 0,                  /*!< Successful operation. */
    ESP_PAN_FAILURE,                      /*!< Generic failure. */
    ESP_PAN_BUSY,                         /*!< Temporarily cannot handle this request. */
    ESP_PAN_NO_RESOURCE,                  /*!< No more resources. */
    ESP_PAN_NEED_INIT,                    /*!< PAN module shall init first. */
    ESP_PAN_NEED_DEINIT,                  /*!< PAN module shall deinit first. */
    ESP_PAN_NO_CONNECTION,                /*!< Connection may have been closed. */
} esp_pan_status_t;

/**
 * @brief PAN initialization configuration parameters.
 */
typedef struct {
    esp_pan_role_mask_t role;             /*!< Local PAN roles to register. Bitmap of esp_pan_role_t values. */
    esp_pan_sec_t panu_sec;               /*!< Security mask for PANU role registration. */
    esp_pan_sec_t gn_sec;                 /*!< Security mask for GN role registration. */
    esp_pan_sec_t nap_sec;                /*!< Security mask for NAP role registration. */
    const char *panu_service_name;        /*!< SDP service name for PANU. Set NULL to use the default name. */
    const char *gn_service_name;          /*!< SDP service name for GN. Set NULL to use the default name. */
    const char *nap_service_name;         /*!< SDP service name for NAP. Set NULL to use the default name. */
} esp_pan_cfg_t;

/**
 * @brief PAN default configuration.
 *
 * Registers PANU role only with authentication and encryption enabled.
 */
#define ESP_PAN_DEFAULT_CONFIG() { \
    .role = ESP_PAN_ROLE_PANU, \
    .panu_sec = ESP_PAN_SEC_AUTHENTICATE | ESP_PAN_SEC_ENCRYPT, \
    .gn_sec = ESP_PAN_SEC_AUTHENTICATE | ESP_PAN_SEC_ENCRYPT, \
    .nap_sec = ESP_PAN_SEC_AUTHENTICATE | ESP_PAN_SEC_ENCRYPT, \
    .panu_service_name = NULL, \
    .gn_service_name = NULL, \
    .nap_service_name = NULL, \
}

/**
 * @brief PAN callback function events.
 */
typedef enum {
    ESP_PAN_INIT_EVT = 0,                 /*!< When PAN is initialized, the event comes. */
    ESP_PAN_DEINIT_EVT,                   /*!< When PAN is deinitialized, the event comes. */
    ESP_PAN_SET_ROLE_EVT,                 /*!< When PAN role registration completes, the event comes. */
    ESP_PAN_OPENING_EVT,                  /*!< When an incoming PAN connection is being established, the event comes. */
    ESP_PAN_OPEN_EVT,                     /*!< When a PAN connection is opened, the event comes. */
    ESP_PAN_CLOSE_EVT,                    /*!< When a PAN connection is closed, the event comes. */
    ESP_PAN_DATA_IND_EVT,                 /*!< When PAN connection received data, the event comes. */
    ESP_PAN_CONG_EVT,                     /*!< When PAN connection congestion status changed, the event comes. */
    ESP_PAN_PFILTER_EVT,                  /*!< When protocol filter indication/result is available, the event comes. */
    ESP_PAN_MFILTER_EVT,                  /*!< When multicast filter indication/result is available, the event comes. */
    ESP_PAN_WRITE_EVT,                    /*!< When PAN write operation completes, the event comes. */
} esp_pan_cb_event_t;

/**
 * @brief PAN callback parameters union.
 */
typedef union {
    /**
     * @brief ESP_PAN_INIT_EVT
     */
    struct pan_init_evt_param {
        esp_pan_status_t status;          /*!< Initialization status. */
    } init;                               /*!< PAN callback param of ESP_PAN_INIT_EVT */

    /**
     * @brief ESP_PAN_DEINIT_EVT
     */
    struct pan_deinit_evt_param {
        esp_pan_status_t status;          /*!< Deinitialization status. */
    } deinit;                             /*!< PAN callback param of ESP_PAN_DEINIT_EVT */

    /**
     * @brief ESP_PAN_SET_ROLE_EVT
     */
    struct pan_set_role_evt_param {
        esp_pan_status_t status;          /*!< Role registration status. */
        esp_pan_role_mask_t role;         /*!< Registered local role bitmap. */
    } set_role;                           /*!< PAN callback param of ESP_PAN_SET_ROLE_EVT */

    /**
     * @brief ESP_PAN_OPENING_EVT
     */
    struct pan_opening_evt_param {
        esp_bd_addr_t remote_bda;         /*!< Remote Bluetooth device address. */
        uint16_t handle;                  /*!< Connection handle assigned by PAN stack. */
    } opening;                            /*!< PAN callback param of ESP_PAN_OPENING_EVT */

    /**
     * @brief ESP_PAN_OPEN_EVT
     */
    struct pan_open_evt_param {
        esp_pan_status_t status;          /*!< Connection open status. */
        esp_bd_addr_t remote_bda;         /*!< Remote Bluetooth device address. */
        uint16_t handle;                  /*!< Connection handle assigned by PAN stack. */
        esp_pan_role_t local_role;        /*!< Local role used for this connection. */
        esp_pan_role_t peer_role;         /*!< Peer role used for this connection. */
    } open;                               /*!< PAN callback param of ESP_PAN_OPEN_EVT */

    /**
     * @brief ESP_PAN_CLOSE_EVT
     */
    struct pan_close_evt_param {
        uint16_t handle;                  /*!< Connection handle of the closed link. */
    } close;                              /*!< PAN callback param of ESP_PAN_CLOSE_EVT */

    /**
     * @brief ESP_PAN_DATA_IND_EVT
     */
    struct pan_data_ind_evt_param {
        uint16_t handle;                  /*!< Connection handle on which data was received. */
        uint8_t src[ESP_BD_ADDR_LEN];     /*!< Source Ethernet MAC address of the received frame. */
        uint8_t dst[ESP_BD_ADDR_LEN];     /*!< Destination Ethernet MAC address of the received frame. */
        uint16_t protocol;                /*!< Ethernet protocol type, e.g. ESP_PAN_PROTO_IP or ESP_PAN_PROTO_ARP. */
        uint16_t len;                     /*!< Length of the payload pointed to by data. */
        uint8_t *data;                    /*!< Pointer to L3 payload. Ethernet header is not included. */
        bool ext;                         /*!< TRUE if BNEP extension headers are present. */
        bool forward;                     /*!< TRUE if the frame is forwarded by GN/NAP. */
    } data_ind;                           /*!< PAN callback param of ESP_PAN_DATA_IND_EVT */

    /**
     * @brief ESP_PAN_CONG_EVT
     */
    struct pan_cong_evt_param {
        uint16_t handle;                  /*!< Connection handle whose congestion status changed. */
        bool cong;                        /*!< TRUE, congested. FALSE, uncongested. Pause esp_pan_write() when TRUE. */
    } cong;                               /*!< PAN callback param of ESP_PAN_CONG_EVT */

    /**
     * @brief ESP_PAN_PFILTER_EVT
     */
    struct pan_pfilter_evt_param {
        uint16_t handle;                  /*!< Connection handle related to the filter event. */
        bool indication;                  /*!< TRUE if this is an indication from peer. FALSE if this is a local operation result. */
        esp_pan_status_t status;          /*!< Operation status. */
        uint16_t len;                     /*!< Length of the filters buffer in bytes. */
        uint8_t *filters;                 /*!< Pointer to protocol filter data. Valid only in callback context. */
    } pfilter;                            /*!< PAN callback param of ESP_PAN_PFILTER_EVT */

    /**
     * @brief ESP_PAN_MFILTER_EVT
     */
    struct pan_mfilter_evt_param {
        uint16_t handle;                  /*!< Connection handle related to the filter event. */
        bool indication;                  /*!< TRUE if this is an indication from peer. FALSE if this is a local operation result. */
        esp_pan_status_t status;          /*!< Operation status. */
        uint16_t len;                     /*!< Length of the filters buffer in bytes. */
        uint8_t *filters;                 /*!< Pointer to multicast filter data. Valid only in callback context. */
    } mfilter;                            /*!< PAN callback param of ESP_PAN_MFILTER_EVT */

    /**
     * @brief ESP_PAN_WRITE_EVT
     */
    struct pan_write_evt_param {
        esp_pan_status_t status;          /*!< Write operation status. */
        uint16_t handle;                  /*!< Connection handle on which data was written. */
    } write;                              /*!< PAN callback param of ESP_PAN_WRITE_EVT */
} esp_pan_cb_param_t;                     /*!< PAN callback parameter union type */

/**
 * @brief       PAN callback function type.
 *
 *              When handling ESP_PAN_DATA_IND_EVT, it is strongly recommended to cache incoming data and
 *              process it in another lower priority application task rather than performing heavy work directly
 *              in this callback.
 *
 * @param       event:      Event type.
 * @param       param:      Pointer to callback parameter, currently union type.
 */
typedef void (*esp_pan_cb_t)(esp_pan_cb_event_t event, esp_pan_cb_param_t *param);

/**
 * @brief       This function is called to register callbacks with the PAN module.
 *
 * @param[in]   callback:   Pointer to the callback function.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_pan_register_callback(esp_pan_cb_t callback);

/**
 * @brief       This function is called to initialize the PAN module.
 *
 *              When the operation is completed, the callback function will be called with ESP_PAN_INIT_EVT.
 *              After initialization succeeds, ESP_PAN_SET_ROLE_EVT is reported for the registered roles.
 *              This function should be called after esp_bluedroid_enable() completes successfully.
 *
 * @param[in]   cfg:        PAN initialization configuration.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_pan_init(const esp_pan_cfg_t *cfg);

/**
 * @brief       This function is called to deinitialize the PAN module.
 *
 *              Active PAN connections are closed first. For each closed connection, ESP_PAN_CLOSE_EVT is reported.
 *              When deinitialization completes, ESP_PAN_DEINIT_EVT is reported.
 *              This function must be called after esp_pan_init() succeeds and before calling esp_pan_init() again.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_pan_deinit(void);

/**
 * @brief       This function makes a PAN connection to a remote Bluetooth device.
 *
 *              When the connection is established or fails, the callback function will be called with ESP_PAN_OPEN_EVT.
 *              For an incoming connection, ESP_PAN_OPENING_EVT is reported before ESP_PAN_OPEN_EVT.
 *              This function must be called after esp_pan_init() succeeds and before esp_pan_deinit().
 *
 * @param[in]   remote_bda: Remote Bluetooth device address.
 * @param[in]   local_role: Local PAN role for this connection. One of ESP_PAN_ROLE_PANU, ESP_PAN_ROLE_GN, ESP_PAN_ROLE_NAP.
 * @param[in]   peer_role:  Expected peer PAN role for this connection.
 *
 * @note        Valid outgoing role pairs include PANU->NAP, PANU->GN, and GN->GN.
 *              A PANU device can only maintain one PANU-role connection at a time.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_pan_connect(esp_bd_addr_t remote_bda, esp_pan_role_t local_role, esp_pan_role_t peer_role);

/**
 * @brief       This function closes a PAN connection.
 *
 *              When the operation is completed, the callback function will be called with ESP_PAN_CLOSE_EVT.
 *              This function must be called after a valid connection handle is obtained from ESP_PAN_OPEN_EVT.
 *
 * @param[in]   handle:     Connection handle obtained from ESP_PAN_OPEN_EVT.
 *
 * @return
 *              - ESP_OK: success
 *              - other: failed
 */
esp_err_t esp_pan_disconnect(uint16_t handle);

/**
 * @brief       This function sends Ethernet payload data over a PAN connection.
 *
 *              The data is encapsulated by BNEP and transmitted to the peer device.
 *              When the write request is accepted or rejected, ESP_PAN_WRITE_EVT is reported.
 *              This function must be called after a valid connection handle is obtained from ESP_PAN_OPEN_EVT.
 *
 * @param[in]   handle:     Connection handle obtained from ESP_PAN_OPEN_EVT.
 * @param[in]   dst:        Destination Ethernet MAC address. Use broadcast address for ARP requests.
 * @param[in]   src:        Source Ethernet MAC address. Usually the local PAN netif MAC address.
 * @param[in]   protocol:   Ethernet protocol type, e.g. ESP_PAN_PROTO_IP or ESP_PAN_PROTO_ARP.
 * @param[in]   len:        Length of the payload pointed to by data.
 *                          Must be in the range [1, ESP_PAN_MAX_WRITE_LEN].
 * @param[in]   data:       Pointer to L3 payload. Ethernet header is not included.
 * @param[in]   ext:        TRUE if forwarded BNEP extension headers are present.
 *
 * @note        dst and src are 6-byte Ethernet MAC addresses, not Bluetooth device addresses.
 *              When integrated with esp_netif, use the Ethernet frame header generated by the TCP/IP stack.
 *              Pause transmission when ESP_PAN_CONG_EVT reports congestion.
 *              len greater than ESP_PAN_MAX_WRITE_LEN is rejected with ESP_ERR_INVALID_ARG.
 *
 * @return
 *              - ESP_OK: success
 *              - ESP_ERR_INVALID_ARG: invalid handle, NULL pointer, zero or oversized len
 *              - other: failed
 */
esp_err_t esp_pan_write(uint16_t handle, const uint8_t *dst, const uint8_t *src, uint16_t protocol,
                        uint16_t len, const uint8_t *data, bool ext);

/**
 * @brief       This function sets protocol filters on the peer device.
 *
 *              Only Ethernet frames whose protocol type falls into one of the configured ranges are accepted.
 *              When the operation completes or a peer indication is received, ESP_PAN_PFILTER_EVT is reported.
 *
 * @param[in]   handle:         Connection handle obtained from ESP_PAN_OPEN_EVT.
 * @param[in]   num_filters:    Number of protocol filter ranges (0 to ESP_PAN_MAX_PROTOCOL_FILTERS).
 *                              Pass 0 with NULL arrays to clear filters.
 * @param[in]   start_array:    Array of range start protocol values. Ignored when num_filters is 0.
 * @param[in]   end_array:      Array of range end protocol values. Ignored when num_filters is 0.
 *
 * @return
 *              - ESP_OK: success
 *              - ESP_ERR_INVALID_ARG: invalid handle, NULL arrays when num_filters > 0, or too many filters
 *              - other: failed
 */
esp_err_t esp_pan_set_protocol_filters(uint16_t handle, uint16_t num_filters,
                                       const uint16_t *start_array, const uint16_t *end_array);

/**
 * @brief       This function sets multicast filters on the peer device.
 *
 *              Only multicast Ethernet frames whose destination MAC falls into one of the configured ranges
 *              are accepted. When the operation completes or a peer indication is received,
 *              ESP_PAN_MFILTER_EVT is reported.
 *
 * @param[in]   handle:         Connection handle obtained from ESP_PAN_OPEN_EVT.
 * @param[in]   num_filters:    Number of multicast filter ranges (0 to ESP_PAN_MAX_MULTICAST_FILTERS).
 *                              Pass 0 with NULL arrays to clear filters.
 * @param[in]   start_array:    Array of range start MAC addresses (esp_bd_addr_t elements). Ignored when num_filters is 0.
 * @param[in]   end_array:      Array of range end MAC addresses (esp_bd_addr_t elements). Ignored when num_filters is 0.
 *
 * @return
 *              - ESP_OK: success
 *              - ESP_ERR_INVALID_ARG: invalid handle, NULL arrays when num_filters > 0, or too many filters
 *              - other: failed
 */
esp_err_t esp_pan_set_multicast_filters(uint16_t handle, uint16_t num_filters,
                                        const esp_bd_addr_t *start_array, const esp_bd_addr_t *end_array);

#ifdef __cplusplus
}
#endif
