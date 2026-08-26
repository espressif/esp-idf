/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "esp_netif.h"

#define PAN_NETIF_MAX_LINKS     4

typedef struct {
    const uint8_t *mac;
    char ip_str[16];
    char netmask_str[16];
    char gw_str[16];
} pan_netif_cfg_t;

esp_err_t pan_netif_init(const pan_netif_cfg_t *cfg);
esp_netif_t *pan_netif_get(void);
void pan_netif_set_uplink(esp_netif_t *wifi_sta_netif);
void pan_netif_on_connected(uint16_t handle, const uint8_t *peer_bda);
void pan_netif_on_disconnected(uint16_t handle);
void pan_netif_on_cong(uint16_t handle, bool congested);
esp_err_t pan_netif_input(uint16_t handle, const uint8_t *dst, const uint8_t *src,
                          uint16_t protocol, const uint8_t *payload, uint16_t len);
