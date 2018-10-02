/* BLE based Provisioning Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <esp_event_loop.h>

#include <protocomm_security.h>
#include <wifi_provisioning/wifi_config.h>

/**
 * @brief   Get state of WiFi Station during provisioning
 *
 * @note    WiFi is initially configured as AP, when
 *          provisioning starts. After provisioning data
 *          is provided by user, the WiFi is reconfigured
 *          to run as both AP and Station.
 *
 * @param[out] state    Pointer to wifi_prov_sta_state_t variable to be filled
 *
 * @return
 *  - ESP_OK    : Successfully retrieved wifi state
 *  - ESP_FAIL  : Provisioning app not running
 */
esp_err_t app_prov_get_wifi_state(wifi_prov_sta_state_t* state);

/**
 * @brief   Get reason code in case of WiFi station
 *          disconnection during provisioning
 *
* @param[out] reason    Pointer to wifi_prov_sta_fail_reason_t variable to be filled
 *
 * @return
 *  - ESP_OK    : Successfully retrieved wifi disconnect reason
 *  - ESP_FAIL  : Provisioning app not running
 */
esp_err_t app_prov_get_wifi_disconnect_reason(wifi_prov_sta_fail_reason_t* reason);

/**
 * @brief   Event handler for provisioning app
 *
 * This is called from the main event handler and controls the
 * provisioning application, depeding on WiFi events
 *
 * @param[in] ctx   Event context data
 * @param[in] event Event info
 *
 * @return
 *  - ESP_OK      : Event handled successfully
 *  - ESP_FAIL    : Failed to start server on event AP start
 */
esp_err_t app_prov_event_handler(void *ctx, system_event_t *event);

/**
 * @brief   Checks if device is provisioned
 * *
 * @param[out] provisioned  True if provisioned, else false
 *
 * @return
 *  - ESP_OK      : Retrieved provision state successfully
 *  - ESP_FAIL    : Failed to retrieve provision state
 */
esp_err_t app_prov_is_provisioned(bool *provisioned);

/**
 * @brief   Runs WiFi as Station
 *
 * Configures the WiFi station mode to connect to the
 * SSID and password specified in config structure,
 * and starts WiFi to run as station
 *
 * @param[in] wifi_cfg  Pointer to WiFi cofiguration structure
 *
 * @return
 *  - ESP_OK      : WiFi configured and started successfully
 *  - ESP_FAIL    : Failed to set configuration
 */
esp_err_t app_prov_configure_sta(wifi_config_t *wifi_cfg);

/**
 * @brief   Start provisioning via Bluetooth
 *
 * @param[in] security  Security mode
 * @param[in] pop       Pointer to proof of possession (NULL if not present)
 *
 * @return
 *  - ESP_OK      : Provisioning started successfully
 *  - ESP_FAIL    : Failed to start
 */
esp_err_t app_prov_start_ble_provisioning(int security, const protocomm_security_pop_t *pop);
