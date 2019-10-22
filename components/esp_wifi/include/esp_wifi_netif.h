// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _ESP_WIFI_NETIF_H
#define _ESP_WIFI_NETIF_H

/**
 * @brief Number of WiFi interfaces used by wifi-netif abstraction
 */
#define MAX_WIFI_IFS (2)

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
 *      - true if ready after intertace started (typically Access Point type)
 *      - false if ready once intertace connected (typically for Station type)
 */
bool esp_wifi_is_if_ready_when_started(wifi_netif_driver_t ifx);

/**
 * @brief Register interface receive callback function with argument
 *
 * @param[in] ifx pointer to wifi interface handle
 * @param[in] fn  funtion to be registered (typically esp_netif_receive)
 * @param[in] arg argument to be supplied to registered function (typically esp_netif ptr)
 *
 * @return ESP_OK on success
 *
 */
esp_err_t esp_wifi_register_if_rxcb(wifi_netif_driver_t ifx, esp_netif_receive_t fn, void * arg);


#endif //_ESP_WIFI_NETIF_H