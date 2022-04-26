/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_DPP_H
#define ESP_DPP_H

#include <stdbool.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_ERR_DPP_FAILURE         (ESP_ERR_WIFI_BASE + 151)   /*!< Generic failure during DPP Operation */
#define ESP_ERR_DPP_TX_FAILURE      (ESP_ERR_WIFI_BASE + 152)   /*!< DPP Frame Tx failed OR not Acked */
#define ESP_ERR_DPP_INVALID_ATTR    (ESP_ERR_WIFI_BASE + 153)   /*!< Encountered invalid DPP Attribute */

/** @brief Types of Bootstrap Methods for DPP. */
typedef enum dpp_bootstrap_type {
    DPP_BOOTSTRAP_QR_CODE,  /**< QR Code Method */
    DPP_BOOTSTRAP_PKEX,     /**< Proof of Knowledge Method */
    DPP_BOOTSTRAP_NFC_URI,  /**< NFC URI record Method */
} esp_supp_dpp_bootstrap_t;

/** @brief Types of Callback Events received from DPP Supplicant. */
typedef enum {
    ESP_SUPP_DPP_URI_READY,     /**< URI is ready through Bootstrapping */
    ESP_SUPP_DPP_CFG_RECVD,     /**< Config received via DPP Authentication */
    ESP_SUPP_DPP_FAIL,          /**< DPP Authentication failure */
} esp_supp_dpp_event_t;

/**
  * @brief Callback function for receiving DPP Events from Supplicant.
  *
  *        Callback function will be called with DPP related information.
  *
  * @param evt DPP event ID
  * @param data Event data payload
  */
typedef void (*esp_supp_dpp_event_cb_t)(esp_supp_dpp_event_t evt, void *data);

/**
  * @brief Initialize DPP Supplicant
  *
  *        Starts DPP Supplicant and initializes related Data Structures.
  *
  * @param evt_cb Callback function to receive DPP related events
  *
  * return
  *    - ESP_OK: Success
  *    - ESP_FAIL: Failure
  */
esp_err_t esp_supp_dpp_init(esp_supp_dpp_event_cb_t evt_cb);

/**
  * @brief De-initalize DPP Supplicant
  *
  *        Frees memory from DPP Supplicant Data Structures.
  */
void esp_supp_dpp_deinit(void);

/**
  * @brief Generates Bootstrap Information as an Enrollee.
  *
  *        Generates Out Of Band Bootstrap information as an Enrollee which can be
  *        used by a DPP Configurator to provision the Enrollee.
  *
  * @param chan_list List of channels device will be available on for listening
  * @param type Bootstrap method type, only QR Code method is supported for now.
  * @param key (Optional) Private Key used to generate a Bootstrapping Public Key
  * @param info (Optional) Ancilliary Device Information like Serial Number
  *
  * @return
  *    - ESP_OK: Success
  *    - ESP_FAIL: Failure
  */
esp_err_t
esp_supp_dpp_bootstrap_gen(const char *chan_list, esp_supp_dpp_bootstrap_t type,
                           const char *key, const char *info);

/**
  * @brief Start listening on Channels provided during esp_supp_dpp_bootstrap_gen.
  *
  *        Listens on every Channel from Channel List for a pre-defined wait time.
  *
  * @return
  *    - ESP_OK: Success
  *    - ESP_FAIL: Generic Failure
  *    - ESP_ERR_INVALID_STATE: ROC attempted before WiFi is started
  *    - ESP_ERR_NO_MEM: Memory allocation failed while posting ROC request
  */
esp_err_t esp_supp_dpp_start_listen(void);

/**
  * @brief Stop listening on Channels.
  *
  *        Stops listening on Channels and cancels ongoing listen operation.
  */
void esp_supp_dpp_stop_listen(void);

#ifdef __cplusplus
}
#endif
#endif /* ESP_DPP_H */
