/*
 * SPDX-FileCopyrightText: 2020-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ESP_DPP_H
#define ESP_DPP_H

#include <stdbool.h>

#include "esp_err.h"
#include "esp_wifi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_DPP_MAX_CHAN_COUNT 5

#ifndef ESP_DPP_MAX_CONFIG_COUNT
#ifdef CONFIG_ESP_WIFI_DPP_MAX_CONF_OBJ
#define ESP_DPP_MAX_CONFIG_COUNT CONFIG_ESP_WIFI_DPP_MAX_CONF_OBJ
#else
#define ESP_DPP_MAX_CONFIG_COUNT 3
#endif
#endif

#define ESP_ERR_DPP_FAILURE         (ESP_ERR_WIFI_BASE + 151)   /*!< Generic failure during DPP Operation */
#define ESP_ERR_DPP_TX_FAILURE      (ESP_ERR_WIFI_BASE + 152)   /*!< DPP Frame Tx failed OR not Acked */
#define ESP_ERR_DPP_INVALID_ATTR    (ESP_ERR_WIFI_BASE + 153)   /*!< Encountered invalid DPP Attribute */
#define ESP_ERR_DPP_AUTH_TIMEOUT    (ESP_ERR_WIFI_BASE + 154)   /*!< DPP Auth response was not received in time */
#define ESP_ERR_DPP_INVALID_LIST    (ESP_ERR_WIFI_BASE + 155)   /*!< Channel list given in esp_supp_dpp_bootstrap_gen() is not valid or too big */
#define ESP_ERR_DPP_CONF_TIMEOUT    (ESP_ERR_WIFI_BASE + 156)   /*!< DPP Configuration was not received in time */

/**
 * @brief AKM values for one DPP configuration row (which credentials apply and how to connect).
 *
 *        Use this with fields in esp_dpp_config_data_t: legacy modes use password,
 *        DPP modes use connector and network access key via esp_supp_dpp_set_config().
 */
typedef enum {
    ESP_DPP_AKM_UNKNOWN = 0,       /**< Not set or unrecognized */
    ESP_DPP_AKM_DPP = 1,           /**< DPP-only: connector and network access key */
    ESP_DPP_AKM_PSK = 2,           /**< WPA2-PSK: passphrase in password */
    ESP_DPP_AKM_SAE = 3,           /**< WPA3-SAE: passphrase in password */
    ESP_DPP_AKM_PSK_SAE = 4,       /**< WPA2/WPA3 transition: passphrase in password */
    ESP_DPP_AKM_SAE_DPP = 5,       /**< SAE plus DPP: passphrase and DPP credentials */
    ESP_DPP_AKM_PSK_SAE_DPP = 6,   /**< WPA2/WPA3 plus DPP: passphrase and DPP credentials */
} esp_dpp_akm_t;

/** @brief Types of Bootstrap Methods for DPP. */
typedef enum dpp_bootstrap_type {
    DPP_BOOTSTRAP_QR_CODE,  /**< QR Code Method */
    DPP_BOOTSTRAP_PKEX,     /**< Proof of Knowledge Method */
    DPP_BOOTSTRAP_NFC_URI,  /**< NFC URI record Method */
} esp_supp_dpp_bootstrap_t;

/**
  * @brief Initialize DPP Supplicant
  *
  *        Starts DPP Supplicant and initializes related Data Structures.
  *
  * return
  *    - ESP_OK: Success
  *    - ESP_FAIL: Failure
  */
esp_err_t esp_supp_dpp_init(void);

/**
  * @brief De-initialize DPP Supplicant
  *
  *        Frees memory from DPP Supplicant Data Structures.
  *
  * @return
  *    - ESP_OK: Success
  */
esp_err_t esp_supp_dpp_deinit(void);

/**
  * @brief Generates Bootstrap Information as an Enrollee.
  *
  *        Generates Out Of Band Bootstrap information as an Enrollee which can be
  *        used by a DPP Configurator to provision the Enrollee.
  *
  * @param chan_list List of channels device will be available on for listening (must not be NULL)
  * @param type Bootstrap method type, only QR Code method is supported for now.
  * @param key (Optional) 32 byte Raw Private Key for generating a Bootstrapping Public Key
  * @param info (Optional) Ancillary Device Information like Serial Number
  *
  * @return
  *    - ESP_OK: Success
  *    - ESP_ERR_INVALID_ARG: chan_list is NULL
  *    - ESP_ERR_DPP_INVALID_LIST: Channel list not valid
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
  *
  * @return
  *    - ESP_OK: Success
  *    - ESP_FAIL: Failure
  */
esp_err_t esp_supp_dpp_stop_listen(void);

/**
  * @brief Install or clear DPP AKM connector material in the supplicant.
  *
  *        This function copies the given DPP AKM row (connector and related keys) into the
  *        supplicant; only one such row is retained, and a new row replaces the previous one.
  *        Pass NULL to clear the stored row. (If the first configuration object in the
  *        Configuration Response uses a DPP AKM with a connector, the stack may load it into the
  *        same store before the event is delivered; NULL clears that selection too.)
  *
  *        Connection retry/fallback policy is application-owned. Typical sequence per selected
  *        row is: pick one row from WIFI_EVENT_DPP_CFG_RECVD, call esp_wifi_set_config(),
  *        call esp_supp_dpp_set_config() using connector values received from
  *        WIFI_EVENT_DPP_CFG_RECVD, call esp_wifi_connect().
  *
  * @param config Pointer to one DPP AKM row from WIFI_EVENT_DPP_CFG_RECVD, or NULL to clear.
  *
  * @return
  *    - ESP_OK: Success
  *    - ESP_ERR_INVALID_ARG: @a config is not a DPP AKM row, or key lengths are invalid
  *    - ESP_ERR_INVALID_STATE: DPP supplicant not initialized
  *    - ESP_ERR_NO_MEM: Allocation failed while storing the row
  */
esp_err_t esp_supp_dpp_set_config(const esp_dpp_config_data_t *config);

#ifdef __cplusplus
}
#endif
#endif /* ESP_DPP_H */
