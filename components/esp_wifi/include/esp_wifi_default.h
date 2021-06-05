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

#ifndef _ESP_WIFI_DEFAULT_H
#define _ESP_WIFI_DEFAULT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Attaches wifi station interface to supplied netif
 *
 * @param esp_netif instance to attach the wifi station to
 *
 * @return
 *  - ESP_OK on success
 *  - ESP_FAIL if attach failed
 */
esp_err_t esp_netif_attach_wifi_station(esp_netif_t *esp_netif);

/**
 * @brief Attaches wifi soft AP interface to supplied netif
 *
 * @param esp_netif instance to attach the wifi AP to
 *
 * @return
 *  - ESP_OK on success
 *  - ESP_FAIL if attach failed
 */
esp_err_t esp_netif_attach_wifi_ap(esp_netif_t *esp_netif);

/**
 * @brief Sets default wifi event handlers for STA interface
 *
 * @return
 *  - ESP_OK on success, error returned from esp_event_handler_register if failed
 */
esp_err_t esp_wifi_set_default_wifi_sta_handlers(void);

/**
 * @brief Sets default wifi event handlers for STA interface
 *
 * @return
 *  - ESP_OK on success, error returned from esp_event_handler_register if failed
 */
esp_err_t esp_wifi_set_default_wifi_ap_handlers(void);

/**
 * @brief Clears default wifi event handlers for supplied network interface
 *
 * @param esp_netif instance of corresponding if object
 *
 * @return
 *  - ESP_OK on success, error returned from esp_event_handler_register if failed
 */
esp_err_t esp_wifi_clear_default_wifi_driver_and_handlers(void *esp_netif);

/**
 * @brief Creates default WIFI AP. In case of any init error this API aborts.
 *
 * @note The API creates esp_netif object with default WiFi access point config,
 * attaches the netif to wifi and registers default wifi handlers.
 *
 * @return pointer to esp-netif instance
 */
esp_netif_t* esp_netif_create_default_wifi_ap(void);

/**
 * @brief Creates default WIFI STA. In case of any init error this API aborts.
 *
 * @note The API creates esp_netif object with default WiFi station config,
 * attaches the netif to wifi and registers default wifi handlers.
 *
 * @return pointer to esp-netif instance
 */
esp_netif_t* esp_netif_create_default_wifi_sta(void);

/**
 * @brief Destroys default WIFI netif created with esp_netif_create_default_wifi_...() API.
 *
 * @param[in] esp_netif object to detach from WiFi and destroy
 *
 * @note This API unregisters wifi handlers and detaches the created object from the wifi.
 * (this function is a no-operation if esp_netif is NULL)
 */
void esp_netif_destroy_default_wifi(void *esp_netif);

/**
 * @brief Creates esp_netif WiFi object based on the custom configuration.
 *
 * @attention This API DOES NOT register default handlers!
 *
 * @param[in] wifi_if type of wifi interface
 * @param[in] esp_netif_config inherent esp-netif configuration pointer
 *
 * @return pointer to esp-netif instance
 */
esp_netif_t* esp_netif_create_wifi(wifi_interface_t wifi_if, esp_netif_inherent_config_t *esp_netif_config);

/**
 * @brief Creates default STA and AP network interfaces for esp-mesh.
 *
 * Both netifs are almost identical to the default station and softAP, but with
 * DHCP client and server disabled. Please note that the DHCP client is typically
 * enabled only if the device is promoted to a root node.
 *
 * Returns created interfaces which could be ignored setting parameters to NULL
 * if an application code does not need to save the interface instances
 * for further processing.
 *
 * @param[out] p_netif_sta pointer where the resultant STA interface is saved (if non NULL)
 * @param[out] p_netif_ap pointer where the resultant AP interface is saved (if non NULL)
 *
 * @return ESP_OK on success
 */
esp_err_t esp_netif_create_default_wifi_mesh_netifs(esp_netif_t **p_netif_sta, esp_netif_t **p_netif_ap);

#ifdef __cplusplus
}
#endif

#endif //_ESP_WIFI_DEFAULT_H
