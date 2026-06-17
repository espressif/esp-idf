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

#if defined(CONFIG_ESP_WIFI_PASN_SUPPORT)
struct nan_pasn_data;
struct nan_pasn_data *esp_nan_app_get_pasn_data(void);
void esp_nan_app_set_pasn_data(struct nan_pasn_data *pd);
//void esp_nan_app_post_pasn_pairing_indication(const wifi_event_nan_pasn_pairing_indication_t *evt);
//void esp_nan_app_post_pasn_pairing_confirm(const wifi_event_nan_pasn_pairing_confirm_t *evt);
#endif /* CONFIG_ESP_WIFI_PASN_SUPPORT */

#ifdef __cplusplus
}
#endif
