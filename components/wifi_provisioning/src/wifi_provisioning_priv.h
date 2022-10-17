/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <protocomm.h>
#include <protocomm_security.h>

#include "wifi_provisioning/manager.h"
#include "wifi_provisioning/wifi_config.h"
#include "wifi_provisioning/wifi_scan.h"
#include "wifi_ctrl.h"

/**
 * @brief   Notify manager that provisioning is done
 *
 * Stops the provisioning. This is called by the get_status_handler()
 * when the status is connected. This has no effect if main application
 * has disabled auto stop on completion by calling
 * wifi_prov_mgr_disable_auto_stop()
 *
 * @return
 *  - ESP_OK      : Provisioning will be stopped
 *  - ESP_FAIL    : Failed to stop provisioning
 */
esp_err_t wifi_prov_mgr_done(void);

/**
 * @brief   Start Wi-Fi AP Scan
 *
 * @param[in] blocking        Set true to return only after scanning is complete
 * @param[in] passive         Set true to perform passive scan instead of default active scan
 * @param[in] group_channels  Number of channels to scan in one go
 *                            (set to 0 for scanning all channels in one go)
 * @param[in] period_ms       Scan time (in milli-seconds) on each channel
 *
 * @return
 *  - ESP_OK    : Successfully started Wi-Fi scanning
 *  - ESP_FAIL  : Provisioning app not running
 */
esp_err_t wifi_prov_mgr_wifi_scan_start(bool blocking, bool passive,
                                        uint8_t group_channels,
                                        uint32_t period_ms);

/**
 * @brief   Use to query the state of Wi-Fi scan
 *
 * @return
 *  - true   : Scan finished
 *  - false  : Scan running
 */
bool wifi_prov_mgr_wifi_scan_finished(void);

/**
 * @brief   Get the count of results in the scan list
 *
 * @return
 *  - count  : Number of Wi-Fi Access Points detected while scanning
 */
uint16_t wifi_prov_mgr_wifi_scan_result_count(void);

/**
 * @brief   Get AP record for a particular index in the scan list result
 *
 * @param[out] index  Index of the result to fetch
 *
 * @return
 *  - result : Pointer to Access Point record
 */
const wifi_ap_record_t *wifi_prov_mgr_wifi_scan_result(uint16_t index);

/**
 * @brief   Get protocomm handlers for wifi_config provisioning endpoint
 *
 * @param[out] ptr   pointer to structure to be set
 *
 * @return
 *  - ESP_OK   : success
 *  - ESP_ERR_INVALID_ARG : null argument
 */
esp_err_t get_wifi_prov_handlers(wifi_prov_config_handlers_t *ptr);

/**
 * @brief   Get protocomm handlers for wifi_scan provisioning endpoint
 *
 * @param[out] ptr   pointer to structure to be set
 *
 * @return
 *  - ESP_OK   : success
 *  - ESP_ERR_INVALID_ARG : null argument
 */
esp_err_t get_wifi_scan_handlers(wifi_prov_scan_handlers_t *ptr);

/**
 * @brief   Get protocomm handlers for wifi_ctrl provisioning endpoint
 *
 * @param[in] ptr   pointer to structure to be set
 *
 * @return
 *  - ESP_OK   : success
 *  - ESP_ERR_INVALID_ARG : null argument
 */
esp_err_t get_wifi_ctrl_handlers(wifi_ctrl_handlers_t *ptr);
