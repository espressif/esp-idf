/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "sdkconfig.h"

/* Private interface file */

#if CONFIG_ESP_CRYPTO_DPA_PROTECTION_AT_STARTUP
void esp_crypto_dpa_protection_startup(void);
#endif
