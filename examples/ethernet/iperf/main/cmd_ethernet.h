/*
 * SPDX-FileCopyrightText: 2018-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_netif.h"
#include "esp_eth.h"

// Register Ethernet functions
void register_ethernet_commands(void);

#ifdef __cplusplus
}
#endif
