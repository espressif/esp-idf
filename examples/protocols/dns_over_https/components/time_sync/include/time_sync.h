/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Update system time from NVS or SNTP if not available.
 *
 * This function retrieves the stored system time from NVS. If the time is not found,
 * it synchronizes with the SNTP server and updates the time in NVS.
 *
 * @return esp_err_t ESP_OK on success, ESP_FAIL on failure.
 */
esp_err_t update_time_from_nvs(void);

/**
 * @brief Fetch the current system time and store it in NVS.
 *
 * This function initializes SNTP, retrieves the current system time, and stores
 * it in Non-Volatile Storage (NVS). In case of failure, an error message is logged.
 *
 * @param[in] args Unused argument placeholder.
 * @return esp_err_t ESP_OK on success, ESP_FAIL on failure.
 */
esp_err_t fetch_and_store_time_in_nvs(void*);

/**
 * @brief Sets up periodic time updates.
 *
 * If the reset reason is power-on, this function updates the system time from NVS.
 * It also creates a periodic timer to regularly fetch and store the time in NVS
 * using `fetch_and_store_time_in_nvs` as the callback.
 *
 * @return void
 */
void setup_periodic_time_updates(void);

#ifdef __cplusplus
}
#endif
