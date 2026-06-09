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

#define NAN_PAIRING_PINCODE_MIN 000000
#define NAN_PAIRING_PINCODE_MAX 999999

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
    union pairing_cred_t cred;
} wifi_nan_pairing_config_t;

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
  *             WIFI_EVENT_NAN_BOOTSTRAP_INDICATION event.
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

#endif /* CONFIG_ESP_WIFI_NAN_PAIRING */

#ifdef __cplusplus
}
#endif
