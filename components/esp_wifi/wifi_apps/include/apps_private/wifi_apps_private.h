/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_log.h"
#include "esp_err.h"
#include "esp_wifi_types.h"
#include "esp_netif.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIG_ESP_WIFI_NAN_SYNC_ENABLE

/**
  * @brief      Initialize the NAN App and required Data Structures
  *
  * @attention  This API should be called in esp_wifi_init()
  */
void esp_nan_app_init(void);

/**
  * @brief      De-initialize the NAN App and complete the cleanup
  *
  * @attention  This API should be called in esp_wifi_deinit()
  */
void esp_nan_app_deinit(void);

/**
  * @brief      NAN App action handler for NAN Started event. Sets up other event handlers and
  *             initializes NAN App context
  *
  * @attention  This API should be called in WIFI_EVENT_NAN_SYNC_STARTED event handler
  *
  * @param      nan_netif  Netif handle corresponding to NAN interface.
  */
void esp_nan_action_start(esp_netif_t *nan_netif);

/**
  * @brief      NAN App action handler for NAN Stopped event. Clears other event handlers and
  *             resets NAN App context
  *
  * @attention  This API should be called in WIFI_EVENT_NAN_SYNC_STOPPED event handler
  */
void esp_nan_action_stop(void);

#endif /* CONFIG_ESP_WIFI_NAN_SYNC_ENABLE */

#ifdef CONFIG_ESP_WIFI_NAN_PAIRING

#include "esp_private/wifi_types.h"

#define WIFI_NAN_PAIRING_REASON_NIK_FUP_TIMEOUT  1  /**< Local reason: peer NIK follow-up not received within timeout.
                                                         See Wi-Fi Aware v4.0 §7.6.4.2 for the NIK-exchange procedure. */

#ifndef NAN_PAIRING_PINCODE_MIN
#define NAN_PAIRING_PINCODE_MIN 0
#endif
#ifndef NAN_PAIRING_PINCODE_MAX
#define NAN_PAIRING_PINCODE_MAX 999999
#endif

union pairing_cred_t {
    uint32_t pincode;       /**< 6-digit PIN in range of NAN_PAIRING_PINCODE_MIN to NAN_PAIRING_PINCODE_MAX */
};

enum nan_pairing_role {
    NAN_PAIRING_ROLE_INITIATOR,
    NAN_PAIRING_ROLE_RESPONDER,
};

typedef struct {
    uint8_t peer_svc_id;
    uint8_t peer_nmi[6];
    enum nan_pairing_role self_role;
    uint8_t pairing_verification: 1; /**< 1 - PASN verify (re-pair), 0 - PASN auth (bootstrap pairing) */
    uint8_t reserved: 7;
    union pairing_cred_t cred;
} wifi_nan_pairing_config_t;

/**
  * @brief NAN Pairing Bootstrap frame event (request or response).
  *
  * @p type is WIFI_NAN_NPBA_TYPE_REQUEST (1) for a bootstrapping request from a peer,
  * or WIFI_NAN_NPBA_TYPE_RESPONSE (2) for a bootstrapping response.
  * For requests, @p methods is the selected bootstrapping method.
  * For responses, @p methods is the matched method, @p status and @p reason_code are valid.
  */
typedef struct {
    uint8_t type;           /**< WIFI_NAN_NPBA_TYPE_REQUEST or WIFI_NAN_NPBA_TYPE_RESPONSE */
    uint8_t peer_svc_id;    /**< Peer's service instance id */
    uint8_t own_svc_id;     /**< Own service instance id */
    uint8_t peer_nmi[6];    /**< Peer's NAN Management Interface MAC */
    uint16_t methods;       /**< selected_method (request) or matched_method (response) */
    uint8_t status;         /**< wifi_nan_pairing_status_t; valid for response */
    uint8_t reason_code;    /**< Rejection reason; valid for response when rejected */
} wifi_nan_bootstrap_event_t;

/**
  * @brief Callback invoked when a NAN Pairing Bootstrap request or response is received.
  *
  * @param evt  Bootstrap frame event data.
  */
typedef void (*esp_nan_app_bootstrap_cb_t)(const wifi_nan_bootstrap_event_t *evt);

/**
  * @brief Set the callback for NAN bootstrap request/response frames.
  *
  * @param cb   Bootstrap callback, or NULL to clear.
  *
  * @return
  *    - ESP_OK: succeed
  */
esp_err_t esp_nan_app_set_bootstrap_cb(esp_nan_app_bootstrap_cb_t cb);

/**
  * @brief NAN Pairing Bootstrapping status values
  */
typedef enum {
    WIFI_NAN_PAIRING_STATUS_ACCEPTED = 0,   /**< Bootstrapping request accepted */
    WIFI_NAN_PAIRING_STATUS_REJECTED = 1,   /**< Bootstrapping request rejected */
    WIFI_NAN_PAIRING_STATUS_COMEBACK = 2,   /**< Comeback - responder needs more time */
} wifi_nan_pairing_status_t;

/**
  * @brief NAN Pairing Bootstrapping Request parameters (initiator -> responder)
  *
  * Used by a subscriber (bootstrapping initiator) to send a bootstrapping
  * request follow-up message with NPBA attribute (Type=Request) to a publisher.
  */
typedef struct {
    uint8_t inst_id;                    /**< Own service instance id */
    uint8_t peer_inst_id;               /**< Peer's service instance id */
    uint8_t peer_mac[6];                /**< Peer's NAN Management Interface MAC */
    uint16_t selected_method;           /**< One selected WIFI_NAN_BOOTSTRAP_* method bit */
    wifi_nan_pairing_status_t status;   /**< Set to COMEBACK when resending with cookie */
    uint16_t comeback_after;            /**< Comeback deferral time in TUs (only when status=COMEBACK) */
    uint32_t cookie;                    /**< Opaque cookie from responder (only when status=COMEBACK) */
} wifi_nan_pairing_bootstrap_req_t;

/**
  * @brief NAN Pairing Bootstrapping Response parameters (responder -> initiator)
  *
  * Used by a publisher (bootstrapping responder) to respond to a bootstrapping
  * request follow-up message with NPBA attribute (Type=Response).
  */
typedef struct {
    uint8_t inst_id;                    /**< Own service instance id */
    uint8_t peer_inst_id;               /**< Peer's service instance id */
    uint8_t peer_mac[6];                /**< Peer's NAN Management Interface MAC */
    wifi_nan_pairing_status_t status;   /**< Accepted, Rejected, or Comeback */
    uint16_t matched_method;            /**< Matched bootstrapping method, one WIFI_NAN_BOOTSTRAP_* bit (valid if accepted) */
    uint8_t reason_code;                /**< Rejection reason code (valid if rejected) */
    uint16_t comeback_after;            /**< Comeback deferral time in TUs (only when status=COMEBACK) */
    uint32_t cookie;                    /**< Opaque cookie for comeback (only when status=COMEBACK) */
} wifi_nan_pairing_bootstrapping_resp_t;

/**
  * @brief      Send a NAN Pairing Bootstrapping request to a matched publisher
  *
  * @attention  This API should be called by the Subscriber after a service match
  *             (WIFI_EVENT_NAN_SVC_MATCH) with a Publisher that has pairing enabled.
  *             The selected_method must match one of the methods advertised by the publisher.
  *
  * @param      req  Pairing bootstrapping request parameters.
  *
  * @return
  *    - ESP_OK: Bootstrapping request follow-up sent successfully
  *    - ESP_ERR_INVALID_ARG: Invalid parameters
  *    - ESP_FAIL: Failed to send
  */
esp_err_t esp_wifi_nan_bootstrap_request(wifi_nan_pairing_bootstrap_req_t *req);

/**
  * @brief      Respond to a NAN Pairing Bootstrapping request from a peer
  *
  * @attention  This API should be called by the Publisher after receiving a
  *             bootstrap indication via the registered NAN bootstrap callback.
  *
  * @param      resp  Pairing bootstrapping response parameters.
  *
  * @return
  *    - ESP_OK: Bootstrapping response follow-up sent successfully
  *    - ESP_ERR_INVALID_ARG: Invalid parameters
  *    - ESP_FAIL: Failed to send
  */
esp_err_t esp_wifi_nan_bootstrap_response(wifi_nan_pairing_bootstrapping_resp_t *resp);

/**
  * @brief      Start NAN Pairing process after credentials are shared Out-of-band
  *
  * @attention  This API should be called after Bootstrapping is completed
  *
  * @param      req  Pairing setup parameters.
  *
  * @return
  *    - ESP_OK: Bootstrapping request follow-up sent successfully
  *    - ESP_ERR_INVALID_ARG: Invalid parameters
  *    - ESP_FAIL: Failed to send
  */
esp_err_t esp_wifi_nan_pairing_start(wifi_nan_pairing_config_t *cfg);

struct nan_pasn_data;

/**
  * @brief      Get the NAN PASN context held by the NAN App.
  *
  * @return     Pointer to the current NAN PASN data, or NULL if none is set.
  */
struct nan_pasn_data *esp_nan_app_get_pasn_data(void);

/**
  * @brief      Store the NAN PASN context in the NAN App.
  *
  * @attention  Ownership is not transferred; the caller remains responsible for
  *             the lifetime of @p pd. Pass NULL to clear the stored context.
  *
  * @param      pd  Pointer to the NAN PASN data to store, or NULL to clear it.
  */
void esp_nan_app_set_pasn_data(struct nan_pasn_data *pd);

/**
  * @brief      Clear NM-TK and ND-TK in firmware and host state.
  *
  * @param      peer_nmi    Peer NMI (6 octets), or NULL to clear all peers on
  *                         @p service_id.
  * @param      service_id  Own service id; used only when @p peer_nmi is NULL.
  */
void esp_nan_app_clear_peer_tks(const uint8_t *peer_nmi, uint8_t service_id);

/**
  * @brief      Terminate all active NDPs with a peer (PASN verify prep).
  *
  * @param      peer_nmi  Peer NMI (6 octets).
  *
  * @return     ESP_OK if all ends succeeded or none were active.
  */
esp_err_t esp_nan_app_end_peer_datapaths(const uint8_t *peer_nmi);

#endif /* CONFIG_ESP_WIFI_NAN_PAIRING */

#ifdef __cplusplus
}
#endif
