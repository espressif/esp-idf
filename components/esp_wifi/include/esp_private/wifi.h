/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * All the APIs declared here are internal only APIs, it can only be used by
 * espressif internal modules, such as SSC, LWIP, esp-netif etc, espressif
 * customers are not recommended to use them.
 *
 * If someone really want to use specified APIs declared in here, please contact
 * espressif AE/developer to make sure you know the limitations or risk of
 * the API, otherwise you may get unexpected behavior!!!
 *
 */

#ifndef __ESP_WIFI_INTERNAL_H__
#define __ESP_WIFI_INTERNAL_H__

#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "sys/queue.h"
#include "esp_err.h"
#include "esp_wifi_types.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_smartconfig.h"
#include "wifi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WIFI_MAC_ADDR_LEN  6   /**< Length of an 802.11 MAC address in octets */
#ifndef NAN_IPV6_IDENTIFIER_LEN
#define NAN_IPV6_IDENTIFIER_LEN  8   /**< Length of a NAN IPv6 interface identifier in octets */
#endif

typedef struct {
    QueueHandle_t handle; /**< FreeRTOS queue handler */
    void *storage;        /**< storage for FreeRTOS queue */
} wifi_static_queue_t;

/**
  * @brief NAN Datapath Security Type (Wi-Fi Aware v4.0 §9.5.16.1 Table 85, "Security Present" bit)
  */
typedef enum {
    WIFI_NAN_SECURITY_OPEN      = 0,    /**< NDP does not require security */
    WIFI_NAN_SECURITY_ENCRYPTED = 1,    /**< NDP requires security */
} wifi_nan_security_type_t;

/**
  * @brief NAN single-PMKID security parameters
  *
  * @note Holds one derived ND-PMK + ND-PMKID per spec §7.1.3.5. Used both for
  *       the blob's per-service derived cache (own-side Publish SDF SCIA TX)
  *       and for the host's per-NDL handshake state (M1-M4). Single PMKID is
  *       sufficient because every spec key derivation step yields exactly one
  *       PMKID for a given (PMK, peer NMIs, Service ID) tuple.
  *       Shared between WiFi libraries and NAN app layer.
  */
typedef struct {
    wifi_nan_security_type_t type;     /**< Security Type (Open/Encrypted) */
    uint16_t csid_bitmap;              /**< Selected Cipher Suite ID bit (WIFI_NAN_CSID_BIT_*) */
    uint8_t nd_pmk[ESP_WIFI_NAN_NDP_PMK_LEN];     /**< ND-PMK */
    uint8_t nd_pmkid[ESP_WIFI_NAN_NDP_PMKID_LEN]; /**< ND-PMKID */
    uint8_t group_data_prot: 1;        /**< Group addressed data frame protection. Reserved: not supported right now. */
    uint8_t group_mgmt_prot: 1;        /**< Group addressed management frame protection. Reserved: not supported right now. */
    uint8_t reserved: 6;               /**< Reserved */
} wifi_nan_security_params_t;

/**
  * @brief NAN peer security and pairing material parsed from Publish/Subscribe SDF
  *
  * @note Per Wi-Fi Aware v4.0 §7.1.3.5, a publisher SDF SCIA may advertise
  *       multiple ND-PMKIDs (one per cached PMK). The receiver stores them
  *       and matches against locally-derived PMKID at NDP-initiation time.
  *       Pairing flags mirror wifi_event_nan_svc_match_t and are filled by the
  *       NAN blob when parsing CSIA/SCIA/DCEA.
  *       Internal only — not part of the public API. Shared between WiFi
  *       libraries and NAN app layer.
  */
#define NAN_PEER_MAX_PMKIDS  2  /**< Internal cap; can grow without API impact */
typedef struct {
    uint16_t csid_bitmap;              /**< Peer's advertised Cipher Suite ID bitmap (WIFI_NAN_CSID_BIT_*) */
    uint8_t num_pmkids;                /**< Number of parsed PMKIDs */
    uint8_t pmkids[NAN_PEER_MAX_PMKIDS][ESP_WIFI_NAN_NDP_PMKID_LEN]; /**< Parsed ND-PMKIDs */
    uint8_t group_data_prot: 1;        /**< Peer advertises group data frame protection */
    uint8_t group_mgmt_prot: 1;        /**< Peer advertises group mgmt frame protection */
    uint8_t pairing_setup: 1;          /**< Pairing setup: 0 - disabled, 1 - enabled */
    uint8_t npk_nik_caching: 1;        /**< NPK/NIK caching: 0 - disabled, 1 - enabled (valid if pairing_setup) */
    uint8_t reserved: 4;               /**< Reserved */
} wifi_nan_peer_sdf_security_t;

/* NAN Peer info parsed from SDF */
struct nan_cb_peer_info {
    uint8_t peer_mac[WIFI_MAC_ADDR_LEN];                    /**< Peer NMI / interface MAC */
    uint8_t peer_svc_id;                                    /**< Peer's service instance ID */
    uint16_t sdea;                                          /**< SDEA control field bits */
    uint32_t device_caps;                                   /**< NAN device capabilities */
    uint8_t ssi_ver;                                        /**< SSI version (service_match) */
    uint8_t *ssi;                                           /**< Service-specific information */
    uint16_t ssi_len;                                       /**< SSI length in bytes */
    wifi_nan_peer_sdf_security_t *peer_security_params;     /**< Peer's discovery security params parsed from SDF */
    nan_vendor_ie_t *vendor_ie;                             /**< Vendor-specific IE, if any */
};

/* NDP Peer info parsed from NAF. */
struct ndp_cb_peer_info {
    uint8_t ndp_id;
    uint8_t peer_nmi[WIFI_MAC_ADDR_LEN];
    uint8_t peer_ndi[WIFI_MAC_ADDR_LEN];
    uint8_t *ssi;
    uint16_t ssi_len;
};

/* NAN Pairing Bootstrapping parameters parsed from SDF */
struct nan_cb_npba_t {
    uint8_t type;
    uint8_t status;
    uint16_t methods;
};

/* Host-side callbacks the closed-source NAN blob fires up into nan_app.
 * Registered once at nan_app init; all callbacks run in blob/WiFi-task
 * context, so handlers must be non-blocking and avoid heavy work. */
struct nan_sync_callbacks {
    /* Subscriber side: a Publish SDF matching the local subscribe was
     * received from a peer.
     *   sub_id    -- local subscribe service instance ID
     *   peer_info -- publisher details (see struct nan_cb_peer_info)
     *   npba      -- NPBA parsed from Publish SDF (Advertise), or NULL */
    void (* service_match)(uint8_t sub_id, struct nan_cb_peer_info *peer_info,
                           struct nan_cb_npba_t *npba);

    /* Publisher side: a Solicited Publish SDF was just transmitted in
     * response to a Subscribe match. Fires once per recipient.
     *   pub_id    -- local publish service instance ID
     *   peer_info -- subscriber MAC and its service instance ID */
    void (* replied)(uint8_t pub_id, struct nan_cb_peer_info *peer_info);

    /* Either side: a Follow-up SDF was received for an existing service
     * match (post-discovery messaging).
     *   svc_id    -- local service instance ID this Follow-up targets
     *   peer_info -- sender MAC and SSI payload
     *   shared_key_attr -- Shared key descriptor attribute, if any
     *   shared_key_attr_buf_len -- total attribute length for pointer
     *                          shared_key_attr (0 if shared_key_attr is NULL)
     *   npba -- NAN Pairing Bootstrapping parameters, if any */
    void (* receive)(uint8_t svc_id, struct nan_cb_peer_info *peer_info,
                     uint8_t *shared_key_attr, uint16_t shared_key_attr_len,
                     struct nan_cb_npba_t *npba);

    /* Publisher side (NDP Responder): an NDP Request (M1) was received.
     * Host either auto-accepts or waits for esp_wifi_nan_datapath_resp()
     * depending on the publish-time ndp_resp_needed flag.
     *   pub_id      -- local publish instance bound to this request
     *   peer_info   -- initiator's ndp_id, NMI, NDI, SSI
     *   device_caps -- initiator's NAN device capabilities */
    void (* ndp_indication)(uint8_t pub_id, struct ndp_cb_peer_info *peer_info, uint32_t device_caps);

    /* Either side: NDP setup completed (success or rejection).
     *   status          -- NDP_STATUS_ACCEPTED / NDP_STATUS_REJECTED
     *   peer_info       -- peer ndp_id, NMI, NDI
     *   own_ndi         -- locally-assigned NDI for this NDP
     *   ipv6_identifier -- 8-byte IPv6 interface identifier learnt from
     *                      the peer; all-zero if the peer did not
     *                      advertise one (host then derives it from the
     *                      peer NDI). */
    void (* ndp_confirm)(uint8_t status, struct ndp_cb_peer_info *peer_info,
                         uint8_t own_ndi[WIFI_MAC_ADDR_LEN],
                         uint8_t ipv6_identifier[NAN_IPV6_IDENTIFIER_LEN]);

    /* Either side: an established NDP was torn down.
     *   reason   -- termination reason code (peer-initiated, timeout, ...)
     *   ndp_id   -- NDP identifier of the terminated path
     *   init_ndi -- initiator's NDI for this NDP */
    void (* ndp_terminated)(uint8_t reason, uint8_t ndp_id,
                            uint8_t init_ndi[WIFI_MAC_ADDR_LEN]);

    /* TX completion for a host-queued NAN action frame (Follow-up, etc.).
     *   context   -- opaque tag matching the original TX request
     *   tx_status -- true on transmit success, false on failure */
    void (* action_txdone)(uint32_t context, bool tx_status);

    /* Initiator-side M2 RX indication. Blob fires this after parsing the
     * Responder NDI from the M2 NDP attribute (Wi-Fi Aware v4.0 §9.5.16.1
     * Table 82) and before invoking esp_nan_verify_ndp_resp_mic, so the
     * host can populate ndl->peer_ndi for spec-correct PTK derivation
     * (§7.1.3.5: PTK uses Data Interface addresses). */
    void (* ndp_response_indication)(struct ndp_cb_peer_info *peer_info);
    void (* receive_pasn)(uint8_t *buf, size_t len, uint16_t trans_seq, uint16_t status);
    uint32_t (* get_nira_len)(void);
    int (* construct_nira)(uint8_t *frm);
};

/* Host helpers for NAN encrypted-datapath, registered via
 * esp_nan_internal_register_secure_dp_funcs() at nan_app init.
 * Security-gated fields are NULL when CONFIG_ESP_WIFI_NAN_SECURITY=n. */
struct nan_secure_dp_funcs {
    /* === Always-present helpers === */

    /* Fired by the blob after a host-built NDP setup frame (M2/M4) is
     * transmitted, so the host can advance its NDP setup state machine.
     *   msg_type  -- 0x01 = M2 (NDP Response), 0x02 = M4 (Security Install)
     *   tx_status -- true on TX success, false on failure */
    void (*ndp_tx_done_cb)(uint8_t ndp_id, const uint8_t *peer_nmi,
                           uint8_t msg_type, bool tx_status);

    /* === Security-gated helpers (NULL when SECURITY=n) === */

    /* --- Length getters: callers use these to size TX buffers before
     *     invoking the matching construct_*() / get_*_key_desc helper. --- */

    /* Byte length of the CSIA emitted for the union of the two cipher
     * suite bitmaps (Wi-Fi Aware v4.0 §9.5.21.2). */
    uint32_t (*get_csia_len)(uint16_t own_csid_bitmap, uint16_t peer_csid_bitmap);

    /* Byte length of a SCIA carrying @c num_pmkids 16-octet ND-PMKIDs
     * (§9.5.21.4). */
    uint32_t (*get_scia_len)(uint8_t num_pmkids);

    /* Byte length of a NAN Shared Key Descriptor attribute (§9.5.21.5)
     * with the given Key Data field length. */
    uint32_t (*get_shared_key_desc_attr_len)(uint16_t key_data_len);

    /* Byte length of the M4 Shared Key Descriptor including any
     * encrypted KDE payload (GTK/IGTK/BIGTK). */
    int (*ndp_security_install_get_shared_desc_len)(void);

    /* --- CSIA / SCIA construction. Each writes a complete NAN
     *     attribute (header + body) at @c frm and returns bytes
     *     written, or -1 on error. --- */

    /* Build CSIA from the negotiated own/peer cipher suite bitmaps. */
    int (*construct_csia)(uint8_t *frm, uint8_t pub_id,
                          uint16_t own_csid_bitmap, uint16_t peer_csid_bitmap);

    /* Build SCIA for a Publish SDF: emits one SCID per provisioned
     * ND-PMKID so subscribers can recognise themselves (§9.5.21.4). */
    int (*construct_scia_publish)(uint8_t *frm, uint8_t pub_id,
                                  uint8_t num_pmkids,
                                  const uint8_t pmkids[][ESP_WIFI_NAN_NDP_PMKID_LEN]);

    /* Build SCIA for an NDP Request (M1): single SCID matching the
     * (ndp_id, peer_nmi) credential the initiator selected. */
    int (*construct_scia_ndp_req)(uint8_t *frm, uint8_t ndp_id,
                                  const uint8_t *peer_nmi);

    /* Build SCIA for an NDP Response (M2): echoes the SCID accepted
     * by the responder. */
    int (*construct_scia_ndp_resp)(uint8_t *frm, uint8_t ndp_id,
                                   const uint8_t *peer_nmi);

    /* --- Shared Key Descriptor builders (M1 / M2 / M3 / M4).
     *     Each writes the full NAN Shared Key Descriptor attribute
     *     (§9.5.21.5) into @c buf; the MIC field is left zeroed and
     *     must be populated by the matching update_*_mic helper after
     *     the rest of the frame body is in place. Return bytes
     *     written, -1 on error. --- */

    /* M1 (NDP Request): Nonce = INonce, no MIC required. */
    int (*get_ndp_req_shared_key_desc)(uint8_t *buf, size_t buf_len,
                                       uint8_t ndp_id, const uint8_t *peer_nmi);

    /* M2 (NDP Response): Nonce = RNonce, MIC over M2 body. */
    int (*get_ndp_resp_shared_key_desc)(uint8_t *buf, size_t buf_len,
                                        uint8_t ndp_id, const uint8_t *peer_nmi);

    /* M3 (NDP Confirm): MIC over (Auth_Token || M3 body). */
    int (*get_ndp_confirm_shared_key_desc)(uint8_t *buf, size_t buf_len,
                                           uint8_t ndp_id, const uint8_t *peer_nmi);

    /* M4 (Security Install): install bit set; carries any GTK / IGTK /
     * BIGTK KDEs encrypted under ND-KEK. */
    int (*get_ndp_security_install_key_desc)(uint8_t *buf, size_t buf_len,
                                             uint8_t ndp_id, const uint8_t *peer_nmi);

    /* Compute and cache SHA-256(M1_body)[0:16] in the (ndp_id, peer_nmi)
     * NDL slot. The cached Authentication Token is later prepended when
     * computing/verifying the M3 MIC (Wi-Fi Aware v4.0 §7.1.3.5). */
    int (*capture_m1_auth_token)(const uint8_t *m1_body, size_t body_len,
                                 uint8_t ndp_id, const uint8_t *peer_nmi);

    /* --- MIC compute (TX path). Fill the Key MIC field in
     *     @c key_desc_attr (which sits inside @c m*_body) using
     *     ND-KCK derived for this NDP. Return 0 on success. --- */

    /* M2 MIC: HMAC over the entire M2 body. */
    int (*update_ndp_resp_mic)(uint8_t *m2_body, size_t body_len,
                               uint8_t *key_desc_attr,
                               uint8_t ndp_id, const uint8_t *peer_nmi);

    /* M3 MIC: HMAC over (cached M1 Auth_Token || M3 body). */
    int (*update_ndp_confirm_mic)(uint8_t *m3_body, size_t body_len,
                                  uint8_t *key_desc_attr,
                                  uint8_t ndp_id, const uint8_t *peer_nmi);

    /* M4 MIC: HMAC over the entire M4 body. */
    int (*update_ndp_security_install_mic)(uint8_t *m4_body, size_t body_len,
                                           uint8_t *key_desc_attr,
                                           uint8_t ndp_id, const uint8_t *peer_nmi);

    /* --- MIC verify (RX path). Recompute the expected Key MIC over
     *     the same input as the matching update_* helper and compare
     *     against the value in @c key_desc_attr. Return 0 on pass,
     *     -1 on mismatch -- caller tears down the NDP on -1. --- */

    /* M2 MIC verify (initiator). */
    int (*verify_ndp_resp_mic)(uint8_t *m2_body, size_t body_len,
                               uint8_t *key_desc_attr,
                               uint8_t ndp_id, const uint8_t *peer_nmi);

    /* M3 MIC verify (responder; uses cached M1 Auth_Token). */
    int (*verify_ndp_confirm_mic)(uint8_t *m3_body, size_t body_len,
                                  uint8_t *key_desc_attr,
                                  uint8_t ndp_id, const uint8_t *peer_nmi);

    /* M4 MIC verify (initiator). */
    int (*verify_ndp_security_install_mic)(uint8_t *m4_body, size_t body_len,
                                           uint8_t *key_desc_attr,
                                           uint8_t ndp_id, const uint8_t *peer_nmi);

    /* --- RX-path attribute parsers. --- */

    /* Parse CSIA from a received NDP setup frame and populate the
     * negotiated cipher-suite bitmap in @c param. */
    void (*parse_ndp_csia)(void *frm, size_t buf_len, wifi_nan_security_params_t *param);

    /* Parse SCIA from a received NDP setup frame and populate the
     * selected ND-PMKID + Publish ID in @c param. */
    void (*parse_ndp_scia)(void *frm, size_t buf_len, wifi_nan_security_params_t *param);

    /* Parse the NAN Shared Key Descriptor attribute from a received
     * M2/M3/M4 frame: advances the NDL replay counter, captures the
     * peer Nonce, decrypts KDE payloads, and installs GTK/IGTK/BIGTK
     * as applicable. */
    void (*parse_ndp_key_desc)(void *frm, size_t buf_len, uint8_t ndp_id, const uint8_t *peer_nmi);

    /* Parse security-related attributes from an inbound Publish SDF
     * (CSIA + SCIA list) and populate @c security so the subscriber
     * state machine can pick a compatible credential. */
    esp_err_t (*parse_publish_security)(const uint8_t *attrs, size_t attrs_len,
                                        wifi_nan_peer_sdf_security_t *security);

    /* Derives ND-PMK + ND-PMKID for each credential in sec_cfg->creds[] and
     * writes one entry per credential into out_derived[0..num_credentials).
     * Caller passes service_name + the original publish/subscribe security
     * cfg. out_derived must point at an array of at least
     * ESP_WIFI_NAN_MAX_CREDS_PER_SVC entries. All inputs are treated as
     * read-only. Returns ESP_FAIL or NULL when CONFIG_ESP_WIFI_NAN_SECURITY=n. */
    esp_err_t (*derive_security_params)(const char *service_name,
                                        const wifi_nan_discovery_security_params_t *sec_cfg,
                                        wifi_nan_security_params_t *out_derived);

    /* Returns the cipher-suite bitmap negotiated for an in-flight NDP,
     * or 0 if the NDP is open. Used by RX/TX paths to decide whether
     * to apply CCMP/GCMP and which key length to use. */
    uint16_t (*get_ndp_security_csid)(uint8_t ndp_id, const uint8_t *peer_nmi);
};

/**
  * @brief WiFi log level
  *
  */
typedef enum {
    WIFI_LOG_NONE = 0,
    WIFI_LOG_ERROR,       /*enabled by default*/
    WIFI_LOG_WARNING,     /*enabled by default*/
    WIFI_LOG_INFO,        /*enabled by default*/
    WIFI_LOG_DEBUG,       /*can be set in menuconfig*/
    WIFI_LOG_VERBOSE,     /*can be set in menuconfig*/
} wifi_log_level_t;

/**
  * @brief WiFi log module definition
  *
  */
typedef enum {
    WIFI_LOG_MODULE_ALL  = 0, /*all log modules */
    WIFI_LOG_MODULE_WIFI, /*logs related to WiFi*/
    WIFI_LOG_MODULE_COEX, /*logs related to WiFi and BT(or BLE) coexist*/
    WIFI_LOG_MODULE_MESH, /*logs related to Mesh*/
} wifi_log_module_t;

/**
  * @brief WiFi log submodule definition
  *
  */
#define WIFI_LOG_SUBMODULE_ALL   (0)    /*all log submodules*/
#define WIFI_LOG_SUBMODULE_INIT  (1)    /*logs related to initialization*/
#define WIFI_LOG_SUBMODULE_IOCTL (1<<1) /*logs related to API calling*/
#define WIFI_LOG_SUBMODULE_CONN  (1<<2) /*logs related to connecting*/
#define WIFI_LOG_SUBMODULE_SCAN  (1<<3) /*logs related to scanning*/

/**
 * @brief Initialize Wi-Fi Driver
 *     Alloc resource for WiFi driver, such as WiFi control structure, RX/TX buffer,
 *     WiFi NVS structure among others.
 *
 * For the most part, you need not call this function directly. It gets called
 * from esp_wifi_init().
 *
 * This function may be called, if you only need to initialize the Wi-Fi driver
 * without having to use the network stack on top.
 *
 * @param  config provide WiFi init configuration
 *
 * @return
 *    - ESP_OK: succeed
 *    - ESP_ERR_NO_MEM: out of memory
 *    - others: refer to error code esp_err.h
 */
esp_err_t esp_wifi_init_internal(const wifi_init_config_t *config);

/**
 * @brief Deinitialize Wi-Fi Driver
 *     Free resource for WiFi driver, such as WiFi control structure, RX/TX buffer,
 *     WiFi NVS structure among others.
 *
 * For the most part, you need not call this function directly. It gets called
 * from esp_wifi_deinit().
 *
 * This function may be called, if you call esp_wifi_init_internal to initialize
 * WiFi driver.
 *
 * @return
 *    - ESP_OK: succeed
 *    - others: refer to error code esp_err.h
 */
esp_err_t esp_wifi_deinit_internal(void);

/**
  * @brief  free the rx buffer which allocated by wifi driver
  *
  * @param  void* buffer: rx buffer pointer
  */
void esp_wifi_internal_free_rx_buffer(void* buffer);

/**
  * @brief  transmit the buffer via wifi driver
  *
  * This API makes a copy of the input buffer and then forwards the buffer
  * copy to WiFi driver.
  *
  * @param  wifi_interface_t wifi_if : wifi interface id
  * @param  void *buffer : the buffer to be transmit
  * @param  uint16_t len : the length of buffer
  *
  * @return
  *    - ESP_OK  : Successfully transmit the buffer to wifi driver
  *    - ESP_ERR_NO_MEM: out of memory
  *    - ESP_ERR_INVALID_ARG: invalid argument
  *    - ESP_ERR_WIFI_IF : WiFi interface is invalid
  *    - ESP_ERR_WIFI_CONN : WiFi interface is not created, e.g. send the data to STA while WiFi mode is AP mode
  *    - ESP_ERR_WIFI_NOT_STARTED : WiFi is not started
  *    - ESP_ERR_WIFI_STATE : WiFi internal state is not ready, e.g. WiFi is not started
  *    - ESP_ERR_WIFI_NOT_ASSOC : WiFi is not associated
  *    - ESP_ERR_WIFI_TX_DISALLOW : WiFi TX is disallowed, e.g. WiFi hasn't pass the authentication
  *    - ESP_ERR_WIFI_POST : caller fails to post event to WiFi task
  */
int esp_wifi_internal_tx(wifi_interface_t wifi_if, void *buffer, uint16_t len);

/**
  * @brief     The net stack buffer reference counter callback function
  *
  */
typedef void (*wifi_netstack_buf_ref_cb_t)(void *netstack_buf);

/**
  * @brief     The net stack buffer free callback function
  *
  */
typedef void (*wifi_netstack_buf_free_cb_t)(void *netstack_buf);

/**
  * @brief  transmit the buffer by reference via wifi driver
  *
  * This API firstly increases the reference counter of the input buffer and
  * then forwards the buffer to WiFi driver. The WiFi driver will free the buffer
  * after processing it. Use esp_wifi_internal_tx() if the uplayer buffer doesn't
  * supports reference counter.
  *
  * @param  wifi_if : wifi interface id
  * @param  buffer : the buffer to be transmit
  * @param  len : the length of buffer
  * @param  netstack_buf : the netstack buffer related to buffer
  *
  * @return
  *    - ESP_OK  : Successfully transmit the buffer to wifi driver
  *    - ESP_ERR_NO_MEM: out of memory
  *    - ESP_ERR_INVALID_ARG: invalid argument
  *    - ESP_ERR_WIFI_IF : WiFi interface is invalid
  *    - ESP_ERR_WIFI_CONN : WiFi interface is not created, e.g. send the data to STA while WiFi mode is AP mode
  *    - ESP_ERR_WIFI_NOT_STARTED : WiFi is not started
  *    - ESP_ERR_WIFI_STATE : WiFi internal state is not ready, e.g. WiFi is not started
  *    - ESP_ERR_WIFI_NOT_ASSOC : WiFi is not associated
  *    - ESP_ERR_WIFI_TX_DISALLOW : WiFi TX is disallowed, e.g. WiFi hasn't pass the authentication
  *    - ESP_ERR_WIFI_POST : caller fails to post event to WiFi task
  */
esp_err_t esp_wifi_internal_tx_by_ref(wifi_interface_t ifx, void *buffer, size_t len, void *netstack_buf);

/**
  * @brief     Initialize WAPI function when wpa_supplicant initialize.
  *
  * This API is privately used, be careful not open to external applicantion.
  *
  * @return
  *          - ESP_OK : succeed
  *          - ESP_ERR_WAPI_INTERNAL : Internal error
  */
esp_err_t esp_wifi_internal_wapi_init(void);

/**
  * @brief     De-initialize WAPI function when wpa_supplicant de-initialize.
  *
  * This API is privately used, be careful not open to external applicantion.
  *
  * @return
  *          - ESP_OK : succeed
  */
esp_err_t esp_wifi_internal_wapi_deinit(void);

/**
  * @brief  register the net stack buffer reference increasing and free callback
  *
  * @param  ref : net stack buffer reference callback
  * @param  free: net stack buffer free callback
  *
  * @return
  *    - ESP_OK  : Successfully transmit the buffer to wifi driver
  *    - others  : failed to register the callback
  */
esp_err_t esp_wifi_internal_reg_netstack_buf_cb(wifi_netstack_buf_ref_cb_t ref, wifi_netstack_buf_free_cb_t free);

/**
  * @brief     The WiFi RX callback function
  *
  *            Each time the WiFi need to forward the packets to high layer, the callback function will be called
  */
typedef esp_err_t (*wifi_rxcb_t)(void *buffer, uint16_t len, void *eb);

/**
  * @brief     Set the WiFi RX callback
  *
  * @attention 1. Currently we support only one RX callback for each interface
  *
  * @param     wifi_interface_t ifx : interface
  * @param     wifi_rxcb_t fn : WiFi RX callback
  *
  * @return
  *     - ESP_OK : succeed
  *     - others : fail
  */
esp_err_t esp_wifi_internal_reg_rxcb(wifi_interface_t ifx, wifi_rxcb_t fn);

/**
  * @brief     Notify WIFI driver that the station got ip successfully
  *
  * @return
  *     - ESP_OK : succeed
  *     - others : fail
  */
esp_err_t esp_wifi_internal_set_sta_ip(void);

/**
  * @brief  enable or disable transmitting WiFi MAC frame with fixed rate
  *
  * @attention 1. If fixed rate is enabled, both management and data frame are transmitted with fixed rate
  * @attention 2. Make sure that the receiver is able to receive the frame with the fixed rate if you want the frame to be received
  * @attention 3. Not support to set fix rate for espnow and 80211_tx
  *
  * @param  ifx : wifi interface
  * @param  en : false - disable, true - enable
  * @param  rate : PHY rate
  *
  * @return
  *    - ERR_OK  : succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi was not started by esp_wifi_start
  *    - ESP_ERR_WIFI_IF : invalid WiFi interface
  *    - ESP_ERR_INVALID_ARG : invalid rate
  *    - ESP_ERR_NOT_SUPPORTED : do not support to set fixed rate if TX AMPDU is enabled
  */
esp_err_t esp_wifi_internal_set_fix_rate(wifi_interface_t ifx, bool en, wifi_phy_rate_t rate);

/**
  * @brief     Start SmartConfig, config ESP device to connect AP. You need to broadcast information by phone APP.
  *            Device sniffer special packets from the air that containing SSID and password of target AP.
  *
  * @attention 1. This API can be called in station or softAP-station mode.
  * @attention 2. Can not call esp_smartconfig_start twice before it finish, please call
  *               esp_smartconfig_stop first.
  *
  * @param     config pointer to smartconfig start configure structure
  *
  * @return
  *     - ESP_OK: succeed
  *     - others: fail
  */
esp_err_t esp_smartconfig_internal_start(const smartconfig_start_config_t *config);

/**
  * @brief     Stop SmartConfig, free the buffer taken by esp_smartconfig_start.
  *
  * @attention Whether connect to AP succeed or not, this API should be called to free
  *            memory taken by smartconfig_start.
  *
  * @return
  *     - ESP_OK: succeed
  *     - others: fail
  */
esp_err_t esp_smartconfig_internal_stop(void);

/**
  * @brief     Check the MD5 values of the OS adapter header files in IDF and WiFi library
  *
  * @attention 1. It is used for internal CI version check
  *
  * @return
  *     - ESP_OK : succeed
  *     - ESP_WIFI_INVALID_ARG : MD5 check fail
  */
esp_err_t esp_wifi_internal_osi_funcs_md5_check(const char *md5);

/**
  * @brief     Check the MD5 values of the crypto types header files in IDF and WiFi library
  *
  * @attention 1. It is used for internal CI version check
  *
  * @return
  *     - ESP_OK : succeed
  *     - ESP_WIFI_INVALID_ARG : MD5 check fail
  */
esp_err_t esp_wifi_internal_crypto_funcs_md5_check(const char *md5);

/**
  * @brief     Check the MD5 values of the esp_wifi_types.h in IDF and WiFi library
  *
  * @attention 1. It is used for internal CI version check
  *
  * @return
  *     - ESP_OK : succeed
  *     - ESP_WIFI_INVALID_ARG : MD5 check fail
  */
esp_err_t esp_wifi_internal_wifi_type_md5_check(const char *md5);

/**
  * @brief     Check the MD5 values of the esp_wifi_he_types.h in IDF and WiFi library
  *
  * @attention 1. It is used for internal CI version check
  *
  * @return
  *     - ESP_OK : succeed
  *     - ESP_WIFI_INVALID_ARG : MD5 check fail
  */
esp_err_t esp_wifi_internal_wifi_he_type_md5_check(const char *md5);

/**
  * @brief     Check the MD5 values of the esp_wifi.h in IDF and WiFi library
  *
  * @attention 1. It is used for internal CI version check
  *
  * @return
  *     - ESP_OK : succeed
  *     - ESP_WIFI_INVALID_ARG : MD5 check fail
  */
esp_err_t esp_wifi_internal_esp_wifi_md5_check(const char *md5);

/**
  * @brief     Check the MD5 values of the esp_wifi_he.h in IDF and WiFi library
  *
  * @attention 1. It is used for internal CI version check
  *
  * @return
  *     - ESP_OK : succeed
  *     - ESP_WIFI_INVALID_ARG : MD5 check fail
  */
esp_err_t esp_wifi_internal_esp_wifi_he_md5_check(const char *md5);

/**
  * @brief     Allocate a chunk of memory for WiFi driver
  *
  * @attention This API is not used for DMA memory allocation.
  *
  * @param     size_t size : Size, in bytes, of the amount of memory to allocate
  *
  * @return    A pointer to the memory allocated on success, NULL on failure
  */
void *wifi_malloc(size_t size);

/**
  * @brief     Reallocate a chunk of memory for WiFi driver
  *
  * @attention This API is not used for DMA memory allocation.
  *
  * @param     void * ptr  : Pointer to previously allocated memory, or NULL for a new allocation.
  * @param     size_t size : Size, in bytes, of the amount of memory to allocate
  *
  * @return    A pointer to the memory allocated on success, NULL on failure
  */
void *wifi_realloc(void *ptr, size_t size);

/**
  * @brief     Callocate memory for WiFi driver
  *
  * @attention This API is not used for DMA memory allocation.
  *
  * @param     size_t n    : Number of continuing chunks of memory to allocate
  * @param     size_t size : Size, in bytes, of the amount of memory to allocate
  *
  * @return    A pointer to the memory allocated on success, NULL on failure
  */
void *wifi_calloc(size_t n, size_t size);

/**
  * @brief     Update WiFi MAC time
  *
  * @param     uint32_t time_delta : time duration since the WiFi/BT common clock is disabled
  *
  * @return    Always returns ESP_OK
  */
typedef esp_err_t (* wifi_mac_time_update_cb_t)(uint32_t time_delta);

/**
  * @brief     Update WiFi MAC time
  *
  * @param     uint32_t time_delta : time duration since the WiFi/BT common clock is disabled
  *
  * @return    Always returns ESP_OK
  */
esp_err_t esp_wifi_internal_update_mac_time(uint32_t time_delta);

/**
  * @brief     Set current WiFi log level
  *
  * @param     level   Log level.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_FAIL: level is invalid
  */
esp_err_t esp_wifi_internal_set_log_level(wifi_log_level_t level);

/**
  * @brief     Set current log module and submodule
  *
  * @param     module      Log module
  * @param     submodule   Log submodule
  * @param     enable      enable or disable
  *            If module == 0 && enable == 0, all log modules are disabled.
  *            If module == 0 && enable == 1, all log modules are enabled.
  *            If submodule == 0 && enable == 0, all log submodules are disabled.
  *            If submodule == 0 && enable == 1, all log submodules are enabled.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_INVALID_ARG: invalid argument
  */
esp_err_t esp_wifi_internal_set_log_mod(wifi_log_module_t module, uint32_t submodule, bool enable);

/**
  * @brief     Get current WiFi log info
  *
  * @param     log_level  the return log level.
  * @param     log_mod    the return log module and submodule
  *
  * @return
  *    - ESP_OK: succeed
  */
esp_err_t esp_wifi_internal_get_log(wifi_log_level_t *log_level, uint32_t *log_mod);

/**
  * @brief     A general API to set/get WiFi internal configuration, it's for debug only
  *
  * @param     cmd : ioctl command type
  * @param     cfg : configuration for the command
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_wifi_internal_ioctl(int cmd, wifi_ioctl_config_t *cfg);

/**
  * @brief     Get the user-configured channel info
  *
  * @param     ifx : WiFi interface
  * @param     primary : store the configured primary channel
  * @param     second : store the configured second channel
  *
  * @return
  *    - ESP_OK: succeed
  */
esp_err_t esp_wifi_internal_get_config_channel(wifi_interface_t ifx, uint8_t *primary, uint8_t *second);

/**
  * @brief     Get the negotiated channel info after WiFi connection established
  *
  * @param     ifx : WiFi interface
  * @param     aid : the connection number when a STA connects to the softAP
  * @param     primary : store the negotiated primary channel
  * @param     second : store the negotiated second channel
  * @attention the aid param is only works when the device in softAP/softAP+STA mode
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_INVALID_ARG : invalid argument
  *    - ESP_ERR_WIFI_NOT_STARTED : WiFi is not started by esp_wifi_start
  *    - ESP_ERR_WIFI_NOT_CONNECT : No connection between STA and AP
  */
esp_err_t esp_wifi_internal_get_negotiated_channel(wifi_interface_t ifx, uint8_t aid, uint8_t *primary, uint8_t *second);

/**
  * @brief     Get the negotiated bandwidth info after WiFi connection established
  *
  * @param     ifx : WiFi interface
  * @param     bw : store the negotiated bandwidth
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_INVALID_ARG : invalid argument
  *    - ESP_ERR_WIFI_NOT_STARTED : WiFi is not started by esp_wifi_start
  *    - ESP_ERR_WIFI_NOT_CONNECT : No connection between STA and AP
  */
esp_err_t esp_wifi_internal_get_negotiated_bandwidth(wifi_interface_t ifx, uint8_t aid, uint8_t *bw);

#if SOC_WIFI_HW_TSF
/**
  * @brief     Check if WiFi TSF is active
  *
  * @return
  *    - true: Active
  *    - false: Not active
  */
bool esp_wifi_internal_is_tsf_active(void);

/**
  * @brief     Update WIFI light sleep wake ahead time
  *
  */
void esp_wifi_internal_update_light_sleep_wake_ahead_time(uint32_t);

/**
  * @brief     Update WiFi TSF tick interval
  *
  * @return
  *    - true: Active
  *    - false: Not active
  */
esp_err_t esp_wifi_update_tsf_tick_interval(void);
#endif

/**
 * @brief Wifi power domain power on
 */
void esp_wifi_power_domain_on(void);

/**
 * @brief Wifi power domain power off
 */
void esp_wifi_power_domain_off(void);

#if (CONFIG_FREERTOS_USE_TICKLESS_IDLE && SOC_PM_MODEM_RETENTION_BY_REGDMA)
/**
  * @brief     Get wifi mac sleep retention hardware context configuration and size
  *
  * @param     config_size: the wifi mac hardware context configuration size
  *
  * @return    A pointer that point to wifi mac sleep renteiton hardware context configuration table
  */
void * esp_wifi_internal_mac_retention_context_get(int *config_size);
#endif

#if CONFIG_MAC_BB_PD && SOC_PM_MODEM_RETENTION_BY_REGDMA
/**
 * @brief Attach Wi-Fi MAC sleep retention (REGDMA) linked list after allocation.
 *
 * @return
 *    - ESP_OK on success
 *    - error code from sleep retention otherwise
 */
esp_err_t esp_wifi_internal_mac_sleep_retention_attach(void);

/**
 * @brief Detach Wi-Fi MAC sleep retention (REGDMA) linked list before free.
 *
 * @return
 *    - ESP_OK on success
 *    - error code from sleep retention otherwise
 */
esp_err_t esp_wifi_internal_mac_sleep_retention_detach(void);
#endif

#if CONFIG_MAC_BB_PD
/**
  * @brief     Enable or disable powering down MAC and baseband when Wi-Fi is sleeping.
  *
  * @param     enable : enable or disable
  *
  * @return
  *    - ESP_OK: succeed
  */
esp_err_t esp_wifi_internal_set_mac_sleep(bool enable);

/**
 * @brief mac bb sleep.
 */
void pm_mac_sleep(void);

/**
 * @brief mac bb wakeup.
 */
void pm_mac_wakeup(void);
#endif

/**
  * @brief    TxDone callback function type. Should be registered using esp_wifi_set_tx_done_cb()
  *
  * @param    ifidx The interface id that the tx callback has been triggered from
  * @param    data Pointer to the data transmitted
  * @param    data_len Length of the data transmitted
  * @param    txStatus True:if the data was transmitted successfully False: if data transmission failed
  */
typedef void (* wifi_tx_done_cb_t)(uint8_t ifidx, uint8_t *data, uint16_t *data_len, bool txStatus);

/**
  * @brief    Register the txDone callback function of type wifi_tx_done_cb_t
  *
  * @param    cb The callback function
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start
  */
esp_err_t esp_wifi_set_tx_done_cb(wifi_tx_done_cb_t cb);

/**
 * @brief     Set device spp amsdu attributes
 *
 * @param     ifx: WiFi interface
 * @param     spp_cap: spp amsdu capable
 * @param     spp_req: spp amsdu require
 *
 * @return
 *     - ESP_OK: succeed
 *     - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
 *     - ESP_ERR_WIFI_IF : invalid WiFi interface
 */
esp_err_t esp_wifi_internal_set_spp_amsdu(wifi_interface_t ifidx, bool spp_cap, bool spp_req);

/**
 * @brief   Update WIFI light sleep default parameters
 *
 * @param   min_freq_mhz: minimum frequency of DFS
 * @param   max_freq_mhz: maximum frequency of DFS
 */
void esp_wifi_internal_update_light_sleep_default_params(int min_freq_mhz, int max_freq_mhz);

/**
 * @brief   Update WIFI modem sleep default parameters
 */
void esp_wifi_internal_update_modem_sleep_default_params(void);

/**
 * @brief   Set the min active time for wifi to enter the sleep state when light sleep
 *
 * @param   min_active_time: minimum timeout time  for waiting to receive
 *                      data, when no data is received during the timeout period,
 *                      the wifi enters the sleep process.
 */
void esp_wifi_set_sleep_min_active_time(uint32_t min_active_time);

/**
 * @brief   Set wifi keep alive time
 *
 * @param   keep_alive_time: keep alive time
 */
void esp_wifi_set_keep_alive_time(uint32_t keep_alive_time);

/**
  * @brief      Set the min broadcast data wait time for wifi to enter the sleep state
  *
  * @attention  Default sleep wait broadcast data time is 15000, Uint µs.
  *
  * @param      time: When the station knows through the beacon that the AP
  *                   will send broadcast packet, it will wait for a minimum of
  *                   wait_broadcast_data_time before entering the sleep process.
  */
void esp_wifi_set_sleep_wait_broadcast_data_time(uint32_t time);

/**
 * @brief   Configure wifi beacon montior default parameters
 *
 * @param   config: the configuration parameters for wifi beacon monitor
 */
void esp_wifi_beacon_monitor_configure(wifi_beacon_monitor_config_t *config);

/**
 * @brief   Set modem state mode to require WiFi to enable or disable Advanced DTIM sleep function
 *
 * @param   require_modem_state: true for require WiFi to enable Advanced DTIM sleep function,
 *                              false for require WiFi to disable Advanced DTIM sleep function.
 * @return
 *    - ESP_OK: succeed
 */
void esp_wifi_internal_modem_state_configure(bool require_modem_state);

/**
 * @brief   Set light sleep mode to require WiFi to enable or disable Advanced DTIM sleep function
 *
 * @param   light_sleep_enable: true for light sleep mode is enabled, false for light sleep mode is disabled.
 * @return
 *    - ESP_OK: succeed
 */
void esp_wifi_internal_light_sleep_configure(bool light_sleep_enable);

/**
  * @brief      Start Publishing a service in the NAN cluster
  *
  * @attention  This API should be called after esp_wifi_start() in NAN Mode.
  *
  * @param      publish_cfg  Configuration parameters for publishing a service.
  * @param      id  Identifier for the Publish service.
  * @param      cancel  Cancel the service identified by the id.
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_nan_internal_publish_service(const wifi_nan_publish_cfg_t *publish_cfg,
                                           uint8_t *id, bool cancel);

/**
  * @brief      Subscribe for a service within the NAN cluster
  *
  * @attention  This API should be called after esp_wifi_start() in NAN Mode.
  *
  * @param      subscribe_cfg  Configuration parameters for subscribing for a service.
  * @param      id  Identifier for the Subscribe service.
  * @param      cancel  Cancel the service identified by the id.
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_nan_internal_subscribe_service(const wifi_nan_subscribe_cfg_t *subscribe_cfg,
                                             uint8_t *id, bool cancel);

/**
  * @brief      Send Follow-up to the Publisher with matching service
  *
  * @attention  This API should be called after WIFI_EVENT_NAN_SVC_MATCH event is received.
  *
  * @param[in]  fup_params  Configuration parameters for sending a Follow-up to the Peer.
  * @param[out] context Context returned for Follow-up frame to be matched in Tx done.
  * @param[in]  params_i Optional internal-only extras (NPBA / wrapped Shared Key Descriptor).
  *                      If NULL, follow-up is sent with provided `fup_params->ssi`.
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_nan_internal_send_followup(wifi_nan_followup_params_t *fup_params, uint32_t *context,
                                         extra_params_internal_t *params_i);

/**
  * @brief      Send Datapath Request to the Publisher with matching service
  *
  * @attention  This API should be called after WIFI_EVENT_NAN_SVC_MATCH event is received.
  *
  * @param[in]      req  NAN Datapath Request parameters.
  * @param[out]     ndp_id  Pointer to the NDP ID in the NAN Datapath Request.
  * @param[in]      ipv6_identifier IPV6 Address Identifier to be sent in the NDPE attribute.
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_nan_internal_datapath_req(wifi_nan_datapath_req_t *req, uint8_t *ndp_id, uint8_t ipv6_identifier[8]);

/**
  * @brief      Send Datapath Response to accept or reject the received request
  *
  * @attention  This API should be called on the Publisher after receiving WIFI_EVENT_NDP_INDICATION event.
  *
  * @param      resp  NAN Datapath Response parameters.
  * @param      ipv6_identifier IPV6 Address Identifier to be sent in the NDPE attribute.
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_nan_internal_datapath_resp(wifi_nan_datapath_resp_t *resp, uint8_t ipv6_identifier[8]);

/**
  * @brief      End NAN Datapath that is active
  *
  * @attention  This API should be called after receiving WIFI_EVENT_NDP_CONFIRM event.
  *
  * @param      req  NAN Datapath end request parameters.
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_nan_internal_datapath_end(wifi_nan_datapath_end_req_t *req);

/**
  * @brief      End NAN Datapath that is active
  *
  * @attention  This API should be called after receiving WIFI_EVENT_NDP_CONFIRM event.
  *
  * @param      req  NAN Datapath end request parameters.
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_nan_internal_register_callbacks(struct nan_sync_callbacks *cb);

/**
  * @brief Register the NAN secure-datapath helper table with the WiFi libraries.
  *
  * Pass a pointer to a static struct populated by the host; pass NULL to
  * deregister at deinit time.
  *
  * @param fns Pointer to populated nan_secure_dp_funcs (or NULL to deregister)
  * @return ESP_OK on success
  */
esp_err_t esp_nan_internal_register_secure_dp_funcs(struct nan_secure_dp_funcs *fns);

/**
 * @brief      Install NAN pairwise/group key into Wi-Fi firmware key table
 *

 * @param[in]  alg       Cipher algorithm identifier (for CCMP use 3)
 * @param[in]  addr      Peer address used for key lookup (NDI for NAN data path)
 * @param[in]  key_idx   Key index (use 0 for pairwise key)
 * @param[in]  set_tx    Set key as TX key when non-zero
 * @param[in]  seq       Initial sequence/RSC value (typically 8 bytes)
 * @param[in]  seq_len   Length of seq in bytes
 * @param[in]  key       Key material
 * @param[in]  key_len   Key length in bytes
 * @param[in]  key_flag  Key usage flags bitmask
 *
 * @return     0 on success, negative value on failure
 */
int esp_wifi_set_nan_key_internal(int alg, uint8_t *addr, int key_idx, int set_tx,
                                  uint8_t *seq, size_t seq_len, uint8_t *key, size_t key_len, int key_flag);

/**
  * @brief     Connect WiFi station to the AP.
  *
  * @attention 1. This API only impact WIFI_MODE_STA or WIFI_MODE_APSTA mode
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi is not started by esp_wifi_start
  *    - ESP_ERR_WIFI_MODE: WiFi mode error
  *    - ESP_ERR_WIFI_CONN: WiFi internal error, station or soft-AP control block wrong
  *    - ESP_ERR_WIFI_SSID: SSID of AP which station connects is invalid
  */
esp_err_t esp_wifi_connect_internal(void);

/**
  * @brief     Disconnect WiFi station from the AP.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi was not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi was not started by esp_wifi_start
  *    - ESP_FAIL: other WiFi internal errors
  *
  */
esp_err_t esp_wifi_disconnect_internal(void);

/**
 * @brief      Get length of NAN Identity Resolution Attribute (NIRA)
 *
 * @return     Length in bytes
 */
uint32_t esp_nan_get_nira_len(void);

/**
 * @brief      Construct dummy NAN Identity Resolution Attribute (NIRA)
 *
 * @param[out] frm     Buffer to write the attribute to
 *
 * @return     Number of bytes written
 */
int esp_nan_construct_nira(uint8_t *frm);

/**
  * @brief Get the time information from the MAC clock. The time is precise only if modem sleep or light sleep is not enabled.
  *
  * @return 32-bit time value, unit: microsecond. This time is only 32 bits, which means it will overflow and reset to 0 after approximately 71 minutes.
  *         Therefore, when using it, the time difference between two consecutive readings should not be too long.
  */
uint32_t esp_wifi_internal_get_mac_clock_time(void);

#if CONFIG_ESP_WIFI_SLP_SAMPLE_BEACON_FEATURE
/**
 * @brief   Configure wifi beacon offset default parameters
 *
 * @param   config: the configuration parameters for wifi beacon offset
  *
  * @return
  *    - ESP_OK: succeed
  *    - others: failed
  */
esp_err_t esp_wifi_beacon_offset_configure(wifi_beacon_offset_config_t *config);
#endif

/**
 * @brief empty init pm_beacon_offset.
 */
void pm_beacon_offset_funcs_empty_init(void);

/**
 * @brief This API is not context safe and enable easy fragment just for internal test only.
 */
void esp_wifi_enable_easy_fragment(bool enable);

/**
  * @brief     Function signature for received Vendor-Specific Information Element callback with return value.
  * @param     ctx Context argument, as passed to esp_wifi_set_vendor_ie_cb() when registering callback.
  * @param     type Information element type, based on frame type received.
  * @param     sa Source 802.11 address.
  * @param     vnd_ie Pointer to the vendor specific element data received.
  * @param     rssi Received signal strength indication.
  */
typedef esp_err_t (*esp_vendor_ie_cb_with_ret_t)(void *ctx, wifi_vendor_ie_type_t type, const uint8_t sa[6], const vendor_ie_data_t *vnd_ie, int rssi);

/**
  * @brief     Register Vendor-Specific Information Element monitoring callback.
  *
  * @param     cb   Callback function with return value
  * @param     ctx  Context argument, passed to callback function.
  *
  * @return
  *    - ESP_OK: succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  */
esp_err_t esp_wifi_set_vendor_ie_with_ret_cb(esp_vendor_ie_cb_with_ret_t cb, void *ctx);

/**
 * @brief Weak Wi-Fi assert hook.
 *
 * The Wi-Fi library provides a weak default implementation. Applications may
 * define this symbol to override the default behavior for debugging.
 *
 * @note This is intended for diagnostics only and is not part of normal Wi-Fi
 * operation.
 */
void wifi_assert(bool expr, const char *file, const char *func, int line);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_WIFI_H__ */
