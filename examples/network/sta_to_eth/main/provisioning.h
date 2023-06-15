/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

/**
 * @brief Checks if the device has been provisioned
 * @return true if WiFi is provisioned
 */
bool is_provisioned(void);

/**
 * @brief Initiate provisioning
 * @param flags Event flags to indicate status of provisioning
 * @param success_bit bits set in the event flags on success
 * @param fail_bit bits set in the event flags on failure
 * @return ESP_OK if provisioning started
 */
esp_err_t start_provisioning(EventGroupHandle_t *flags, int success_bit, int fail_bit);
