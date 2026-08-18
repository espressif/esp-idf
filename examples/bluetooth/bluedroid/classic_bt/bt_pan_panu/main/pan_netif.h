/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "esp_netif.h"
#include "esp_pan_api.h"

esp_err_t pan_netif_init(const uint8_t *mac);
esp_netif_t *pan_netif_get(void);
void pan_netif_on_connected(uint16_t handle);
void pan_netif_on_disconnected(void);
esp_err_t pan_netif_input(const uint8_t *dst, const uint8_t *src, uint16_t protocol,
                          const uint8_t *payload, uint16_t len);
