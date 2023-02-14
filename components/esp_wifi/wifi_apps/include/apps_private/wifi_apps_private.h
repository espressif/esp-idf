/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
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

#ifdef CONFIG_ESP_WIFI_NAN_ENABLE

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
  * @attention  This API should be called in WIFI_EVENT_NAN_STARTED event handler
  *
  * @param      nan_netif  Netif handle corresponding to NAN interface.
  */
void esp_nan_action_start(esp_netif_t *nan_netif);

/**
  * @brief      NAN App action handler for NAN Stopped event. Clears other event handlers and
  *             resets NAN App context
  *
  * @attention  This API should be called in WIFI_EVENT_NAN_STOPPED event handler
  */
void esp_nan_action_stop(void);

#endif /* CONFIG_ESP_WIFI_NAN_ENABLE */

#ifdef __cplusplus
}
#endif
