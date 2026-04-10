/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#include "esp_wifi.h"
#include "esp_wifi_remote_api.h"
#include "esp_wifi_remote_version.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CONFIG_ESP_WIFI_ENABLED)
ESP_EVENT_DECLARE_BASE(WIFI_REMOTE_EVENT);
#else
#define WIFI_REMOTE_EVENT WIFI_EVENT
#endif

/**
 * @brief Remote channel Rx function pointer
 */
typedef esp_err_t (*esp_remote_channel_rx_fn_t)(void *h, void *buffer, void *buff_to_free, size_t len);

/**
 * @brief Remote channel Tx function pointer
 */
typedef esp_err_t (*esp_remote_channel_tx_fn_t)(void *h, void *buffer, size_t len);

/**
 * @brief Remote channel handle
 */
typedef struct esp_remote_channel *esp_remote_channel_t;

/**
 * @brief Remote channel configuration
 */
typedef struct esp_remote_channel_config *esp_remote_channel_config_t;

// handling channels
/**
 * @brief Receive packet to the esp_wifi network layers
 * @param h Channel handle
 * @param buffer Packet buffer ptr
 * @param buff_to_free Packet ptr to free
 * @param len Packet len
 * @return ESP_OK on success
 */
esp_err_t esp_wifi_remote_channel_rx(void *h, void *buffer, void *buff_to_free, size_t len);

/**
 * @brief Sets Tx callback for the remote channel
 * @param ifx Wifi interface
 * @param h Channel handle
 * @param tx_cb Callback type
 * @return ESP_OK on success
 */
esp_err_t esp_wifi_remote_channel_set(wifi_interface_t ifx, void *h, esp_remote_channel_tx_fn_t tx_cb);

/**
 * @brief Create default STA interface
 * @return ESP_OK on success
 */
esp_netif_t* esp_wifi_remote_create_default_sta(void);

/**
 * @brief Create default AP interface
 * @return ESP_OK on success
 */
esp_netif_t* esp_wifi_remote_create_default_ap(void);

#ifdef __cplusplus
}
#endif
