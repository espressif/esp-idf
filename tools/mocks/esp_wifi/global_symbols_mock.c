/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_wifi.h"
#include "esp_private/wifi_os_adapter.h"

/**
 * The following global objects are defined when WiFi is mocked.
 */

const wpa_crypto_funcs_t g_wifi_default_wpa_crypto_funcs = { };

wifi_osi_funcs_t g_wifi_osi_funcs = { };
