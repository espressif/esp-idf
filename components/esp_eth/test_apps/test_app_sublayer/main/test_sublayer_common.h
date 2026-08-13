/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

#include "sdkconfig.h"

#ifndef CONFIG_ETH_SUBLAYER_SUPPORT
#error "This test app requires CONFIG_ETH_SUBLAYER_SUPPORT"
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_eth.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SUBLAYER_TEST_ETH_START_BIT    BIT0
#define SUBLAYER_TEST_ETH_CONNECT_BIT  BIT1
#define SUBLAYER_TEST_ETH_STOP_BIT     BIT2

#define SUBLAYER_TEST_ETH_START_TIMEOUT_MS   10000
#define SUBLAYER_TEST_ETH_CONNECT_TIMEOUT_MS 30000
#define SUBLAYER_TEST_ETH_STOP_TIMEOUT_MS    10000
#define SUBLAYER_TEST_EVENT_PROPAGATION_MS   500

#define SUBLAYER_TEST_MAX_NETIFS 4

/**
 * Base test context shared by all sublayer test modules.
 * Embed as the FIRST member of test-specific context structs.
 */
typedef struct {
    esp_eth_handle_t *eth_handles;
    uint8_t eth_cnt;
    EventGroupHandle_t eg;
    esp_eth_sublayer_handle_t sub;
    esp_event_handler_instance_t eth_evt_inst;
    esp_netif_t *netifs[SUBLAYER_TEST_MAX_NETIFS];
    uint8_t netif_count;
    bool l2tap_registered;
} sublayer_test_ctx_t;

/**
 * Create the default event loop and register an ETH event handler
 * that signals START/CONNECT/STOP bits on ctx->eg.
 */
void sublayer_test_create_event_loop(sublayer_test_ctx_t *ctx);

/**
 * Initialize all available Ethernet drivers (PHY + MAC).
 * Populates ctx->eth_handles and ctx->eth_cnt.
 */
void sublayer_test_init_ethernet(sublayer_test_ctx_t *ctx);

/**
 * Register L2TAP VFS interface (/dev/net/tap).
 * Sets ctx->l2tap_registered so teardown knows to unregister.
 */
void sublayer_test_register_l2tap(sublayer_test_ctx_t *ctx);

/**
 * Create an esp_netif with custom if_key/if_desc and track it in
 * ctx->netifs[] for automatic cleanup during teardown.
 */
esp_netif_t *sublayer_test_create_eth_netif(sublayer_test_ctx_t *ctx,
                                            const char *if_key,
                                            const char *if_desc);

/**
 * Enable PHY loopback, start ETH driver, and wait for START + CONNECT events.
 */
void sublayer_test_start_and_wait_connect(sublayer_test_ctx_t *ctx);

/**
 * Graceful teardown: stop ETH, destroy tracked netifs, delete sublayer,
 * unregister events, deinit ETH drivers, unregister L2TAP, delete event loop.
 */
void sublayer_test_teardown(sublayer_test_ctx_t *ctx);

/**
 * Register the active test context for the force-teardown safety net.
 */
void sublayer_test_register_ctx(sublayer_test_ctx_t *ctx);

/**
 * Clear the registered active context (call at end of normal teardown).
 */
void sublayer_test_unregister_ctx(void);

/**
 * Force-teardown safety net called from Unity tearDown() — cleans up
 * after a test failure regardless of which test module was active.
 */
void sublayer_test_force_teardown(void);

#ifdef __cplusplus
}
#endif
