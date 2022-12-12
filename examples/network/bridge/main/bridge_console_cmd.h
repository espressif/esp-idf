/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

#include <stdint.h>
#include "esp_netif.h"

#ifdef __cplusplus
extern "C" {
#endif

// Register Bridge configuration commands
void example_register_br_config_commands(esp_netif_t *br_netif, uint8_t br_port_cnt);

#ifdef __cplusplus
}
#endif
