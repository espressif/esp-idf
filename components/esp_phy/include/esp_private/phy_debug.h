/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "esp_phy_init.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_ESP_PHY_RECORD_USED_TIME
void phy_record_time(bool enabled, esp_phy_modem_t modem);
#endif

#ifdef __cplusplus
}
#endif
