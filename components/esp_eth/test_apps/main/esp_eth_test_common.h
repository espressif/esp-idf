/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

#include "unity.h"
#include "test_utils.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_eth.h"

#define ETH_START_BIT BIT(0)
#define ETH_STOP_BIT BIT(1)
#define ETH_CONNECT_BIT BIT(2)
#define ETH_GOT_IP_BIT BIT(3)

#define ETH_START_TIMEOUT_MS (10000)
#define ETH_CONNECT_TIMEOUT_MS (40000)
#define ETH_STOP_TIMEOUT_MS (10000)
#define ETH_GET_IP_TIMEOUT_MS (60000)
#define ETH_DOWNLOAD_END_TIMEOUT_MS (240000)

typedef struct {
    uint8_t dest[ETH_ADDR_LEN];
    uint8_t src[ETH_ADDR_LEN];
    uint16_t proto;
    uint8_t data[];
} __attribute__((__packed__)) emac_frame_t;

esp_eth_mac_t *mac_init(void *vendor_emac_config, eth_mac_config_t *mac_config);
esp_eth_phy_t *phy_init(eth_phy_config_t *phy_config);
void extra_eth_config(esp_eth_handle_t eth_handle);
void extra_cleanup(void);

/** Event handler for Ethernet events */
void eth_event_handler(void *arg, esp_event_base_t event_base,
                                int32_t event_id, void *event_data);
/** Event handler for IP_EVENT_ETH_GOT_IP */
void got_ip_event_handler(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data);
