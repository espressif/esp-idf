/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

#include "esp_err.h"
#include "unity.h"
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

#define MAX_HEAP_ALLOCATION_POINTERS 20

typedef struct {
    uint8_t dest[ETH_ADDR_LEN];
    uint8_t src[ETH_ADDR_LEN];
    uint16_t proto;
    uint8_t data[];
} __attribute__((__packed__)) emac_frame_t;

/** @brief Get the Ethernet handle initialized by setUp()
 *
 * @return esp_eth_handle_t The Ethernet handle, if initialized successfully, otherwise NULL
 */
esp_eth_handle_t eth_test_get_eth_handle(void);

/** @brief Get the Event Group handle initialized by setUp()
 *
 * @return EventGroupHandle_t The Event Group handle, if initialized successfully, otherwise NULL
 */
EventGroupHandle_t eth_test_get_default_event_group(void);

/** @brief Get the ESP-NETIF handle initialized by setUp()
 *
 * @note Valid output only available if the test case is using [esp-netif] identifier
 *
 * @return esp_netif_t The Network Interface handle if initialized successfully, otherwise NULL
 */
esp_netif_t *eth_test_get_netif(void);

/** @brief Set the PHY register bits
 *
 * @param eth_handle The Ethernet handle
 * @param reg_addr The PHY register address
 * @param bitmask The bits to set
 * @param max_attempts The maximum number of set attempts
 * @return esp_err_t The error code
 */
esp_err_t eth_test_set_phy_reg_bits(esp_eth_handle_t eth_handle, uint32_t reg_addr, uint32_t bitmask, uint32_t max_attempts);

/** @brief Clear the PHY register bits
 *
 * @param eth_handle The Ethernet handle
 * @param reg_addr The PHY register address
 * @param bitmask The bits to clear
 * @param max_attempts The maximum number of clear attempts
 * @return esp_err_t The error code
 */
esp_err_t eth_test_clear_phy_reg_bits(esp_eth_handle_t eth_handle, uint32_t reg_addr, uint32_t bitmask, uint32_t max_attempts);

/** @brief Initialize Ethernet driver (default implementation uses ethernet_init component).
 *
 * @param[out] eth_handle Initialized Ethernet driver handle
 * @return esp_err_t
 */
esp_err_t esp_eth_test_eth_init(esp_eth_handle_t *eth_handle);

/** @brief Deinitialize Ethernet driver (default implementation uses ethernet_init component).
 *
 * @param eth_handle Ethernet driver handle to deinitialize
 * @return esp_err_t
 */
esp_err_t esp_eth_test_eth_deinit(esp_eth_handle_t eth_handle);

/** @brief Default event handler function header for Ethernet events to be registered by `esp_event_handler_register()` in the test
 *         with event base `ETH_EVENT` and event ID `ESP_EVENT_ANY_ID`
 *
 *  @note This function is intended to be registered as a callback function only when [skip_setup_teardown] identifier is used. Otherwise,
 *        it's registered automatically by the Common test app during setUp() phase.
 */
void eth_test_default_event_handler(void *arg, esp_event_base_t event_base,
                                int32_t event_id, void *event_data);

/** @brief Default event handler function header for IP_EVENT_ETH_GOT_IP events to be registered by `esp_event_handler_register()` in the test
 *         with event base `IP_EVENT` and event ID `IP_EVENT_ETH_GOT_IP`
 *
 *  @note This function is intended to be registered as a callback function only when [skip_setup_teardown] identifier is used. Otherwise,
 *        it's registered automatically by the Common test app during setUp() phase.
 */
void eth_test_got_ip_event_handler(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data);

/** @brief Allocate memory from the heap
 *
 *  @note The the Common test app keeps track of all allocated memory and frees it automatically during tearDown() phase.
 *        The maximum number of allocations is limited to `MAX_HEAP_ALLOCATION_POINTERS`.
 *
 * @param size The size of the memory to allocate
 * @return void* The pointer to the allocated memory, NULL if allocation failed
 */
void* eth_test_alloc(size_t size);

/** @brief Free memory from the heap
 *
 *  @note All allocated memory is freed automatically during tearDown() phase. Use this function only when required by the test
 *        procedure.
 *
 * @param ptr The pointer to the memory to free
 * @return void
 */
void eth_test_free(void *ptr);

/** @brief Free all memory from the heap
 *
 *  @note All allocated memory is freed automatically during tearDown() phase. Use this function only when required by the test
 *        procedure.
 *
 * @return void
 */
void eth_test_free_all(void);
