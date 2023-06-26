/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief      Convert user input colon separated MAC Address into 6 byte MAC Address
  *
  * @param[in]      str   User input colon separated MAC Address.
  * @param[out]     dest  Output 6 byte MAC Address.
  *
  * @return
  *    - ESP_OK: Succeed
  *    - ESP_FAIL: Invalid input format
  */
esp_err_t esp_supplicant_str_to_mac(const char *str, uint8_t dest[6]);

#ifdef __cplusplus
}
#endif
