/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "esp_wifi_types.h"
#include "esp_netif_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Number of WiFi interfaces used by wifi-netif abstraction
 */
#define MAX_WIFI_IFS WIFI_IF_MAX

/**
 * @brief Forward declaration of WiFi interface handle
 */
typedef struct wifi_netif_driver* wifi_netif_driver_t;

/**
 * @brief Creates wifi driver instance to be used with esp-netif
 *
 * @param wifi_if wifi interface type (station, softAP)
 *
 * @return
 *  - pointer to wifi interface handle on success
 *  - NULL otherwise
 */
wifi_netif_driver_t esp_wifi_create_if_driver(wifi_interface_t wifi_if);

/**
 * @brief Destroys wifi driver instance
 *
 * @param h pointer to wifi interface handle
 *
 */
void esp_wifi_destroy_if_driver(wifi_netif_driver_t h);

/**
 * @brief Return mac of specified wifi driver instance
 *
 * @param[in] ifx pointer to wifi interface handle
 * @param[out] mac output mac address
 *
 * @return ESP_OK on success
 *
 */
esp_err_t esp_wifi_get_if_mac(wifi_netif_driver_t ifx, uint8_t mac[6]);

/**
 * @brief Return true if the supplied interface instance is ready after start.
 * Typically used when registering on receive callback, which ought to be
 * installed as soon as AP started, but once STA gets connected.
 *
 * @param[in] ifx pointer to wifi interface handle
 *
 * @return
 *      - true if ready after interface started (typically Access Point type)
 *      - false if ready once interface connected (typically for Station type)
 */
bool esp_wifi_is_if_ready_when_started(wifi_netif_driver_t ifx);

/**
 * @brief Register interface receive callback function with argument
 *
 * @param[in] ifx pointer to wifi interface handle
 * @param[in] fn  function to be registered (typically esp_netif_receive)
 * @param[in] arg argument to be supplied to registered function (typically esp_netif ptr)
 *
 * @return ESP_OK on success
 *
 */
esp_err_t esp_wifi_register_if_rxcb(wifi_netif_driver_t ifx, esp_netif_receive_t fn, void * arg);

#ifdef __cplusplus
}
#endif
