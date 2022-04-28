/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Update the system time from time stored in NVS.
 *
 */

esp_err_t update_time_from_nvs(void);

/**
 * @brief Fetch the current time from SNTP and stores it in NVS.
 *
 */
void fetch_and_store_time_in_nvs(void*);

#ifdef __cplusplus
}
#endif
