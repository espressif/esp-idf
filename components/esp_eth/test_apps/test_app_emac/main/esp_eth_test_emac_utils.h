/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_err.h"
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
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

#define MAX_HEAP_ALLOCATION_POINTERS 20

typedef struct {
    uint8_t dest[ETH_ADDR_LEN];
    uint8_t src[ETH_ADDR_LEN];
    uint16_t proto;
    uint8_t data[];
} __attribute__((__packed__)) emac_frame_t;

/** @brief Get the Ethernet handle initialized by setUp() */
esp_eth_handle_t eth_test_get_eth_handle(void);

/** @brief Get the Event Group handle initialized by setUp() */
EventGroupHandle_t eth_test_get_default_event_group(void);

/** @brief Get the ESP-NETIF handle initialized by setUp()
 *
 * @note Valid only when the test case uses the [esp-netif] identifier
 */
esp_netif_t *eth_test_get_netif(void);

/** @brief Default ETH_EVENT handler for tests that use [skip_setup_teardown] */
void eth_test_default_event_handler(void *arg, esp_event_base_t event_base,
                                    int32_t event_id, void *event_data);

/** @brief Default IP_EVENT_ETH_GOT_IP handler for tests that use [skip_setup_teardown] */
void eth_test_got_ip_event_handler(void *arg, esp_event_base_t event_base,
                                   int32_t event_id, void *event_data);

/** @brief Allocate memory tracked by tearDown() */
void *eth_test_alloc(size_t size);

/** @brief Free a specific allocation tracked by the test framework */
void eth_test_free(void *ptr);

/** @brief Free all allocations tracked by the test framework */
void eth_test_free_all(void);
