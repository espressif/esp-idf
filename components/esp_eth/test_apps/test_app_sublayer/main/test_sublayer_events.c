/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/*
 * Target tests for the Ethernet sublayer event forwarding logic.
 *
 * Tested behaviour:
 *  - Without a connect-confirm event the sublayer forwards
 *    ETHERNET_EVENT_CONNECTED straight to esp_netif_action_connected(),
 *    bringing the netif up immediately.
 *  - With a connect-confirm event the netif stays down after
 *    ETHERNET_EVENT_CONNECTED and only comes up when the custom
 *    confirm event is posted.
 *  - ETHERNET_EVENT_DISCONNECTED brings the netif down.
 *  - A custom disconnect-trigger event also brings the netif down.
 *
 * PHY loopback mode is enabled so the tests are self-contained and do
 * not require a physical cable.
 */

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "esp_log.h"
#include "unity.h"
#include "ethernet_init.h"
#include "test_sublayer_common.h"

static const char *TAG = "sublayer_events_test";

#define EVENT_PROPAGATION_MS 200

ESP_EVENT_DEFINE_BASE(TEST_SUBLAYER_EVENT);
enum {
    TEST_CONFIRM_EVENT_ID    = 0,
    TEST_DISCONNECT_EVENT_ID = 1,
};

/* ------------------------------------------------------------------ */
/*  Test context and helpers                                          */
/* ------------------------------------------------------------------ */

typedef struct {
    sublayer_test_ctx_t base;
    esp_netif_t *netif;
    esp_eth_sublayer_vlan_handle_t vlan_hdl;
} events_test_ctx_t;

static void test_setup(events_test_ctx_t *ctx, const esp_eth_sublayer_config_t *sub_cfg_template)
{
    memset(ctx, 0, sizeof(*ctx));

    sublayer_test_create_event_loop(&ctx->base);
    sublayer_test_init_ethernet(&ctx->base);
    sublayer_test_register_ctx(&ctx->base);

    esp_eth_sublayer_config_t cfg = *sub_cfg_template;
    cfg.eth_handle = ctx->base.eth_handles[0];
    TEST_ESP_OK(esp_eth_sublayer_new(&cfg, &ctx->base.sub));

    TEST_ESP_OK(esp_eth_sublayer_vlan_add(ctx->base.sub, ESP_ETH_SUBLAYER_UNTAGGED_VID, &ctx->vlan_hdl));

    ctx->netif = sublayer_test_create_eth_netif(&ctx->base, "ETH_DEF", "eth");
    TEST_ESP_OK(esp_netif_attach(ctx->netif, ctx->vlan_hdl));
}

static void test_start_and_wait_connect(events_test_ctx_t *ctx)
{
    sublayer_test_start_and_wait_connect(&ctx->base);
}

static void test_teardown(events_test_ctx_t *ctx)
{
    sublayer_test_teardown(&ctx->base);
    sublayer_test_unregister_ctx();
}

/* ------------------------------------------------------------------ */
/*  Test cases                                                        */
/* ------------------------------------------------------------------ */

TEST_CASE("connect without confirm event sets link up", "[sublayer_events]")
{
    ESP_LOGI(TAG, "--- no confirm event: CONNECTED should bring netif up immediately ---");

    esp_eth_sublayer_config_t sub_cfg = ESP_ETH_SUBLAYER_CONFIG_DEFAULT();

    events_test_ctx_t ctx;
    test_setup(&ctx, &sub_cfg);
    test_start_and_wait_connect(&ctx);

    TEST_ASSERT_TRUE_MESSAGE(esp_netif_is_netif_up(ctx.netif),
                             "netif must be up after CONNECTED (no confirm event)");

    test_teardown(&ctx);
}

TEST_CASE("connect with confirm event defers link up", "[sublayer_events]")
{
    ESP_LOGI(TAG, "--- with confirm event: CONNECTED alone must NOT bring netif up ---");

    esp_eth_sublayer_config_t sub_cfg = ESP_ETH_SUBLAYER_CONFIG_DEFAULT();
    sub_cfg.connect_confirm_event = (esp_eth_sublayer_event_t){
        .base = TEST_SUBLAYER_EVENT,
        .event_id = TEST_CONFIRM_EVENT_ID,
    };

    events_test_ctx_t ctx;
    test_setup(&ctx, &sub_cfg);
    test_start_and_wait_connect(&ctx);

    TEST_ASSERT_FALSE_MESSAGE(esp_netif_is_netif_up(ctx.netif),
                              "netif must stay down after CONNECTED when confirm event is configured");

    ESP_LOGI(TAG, "--- posting confirm event ---");
    TEST_ESP_OK(esp_event_post(TEST_SUBLAYER_EVENT, TEST_CONFIRM_EVENT_ID,
                               NULL, 0, portMAX_DELAY));
    vTaskDelay(pdMS_TO_TICKS(EVENT_PROPAGATION_MS));

    TEST_ASSERT_TRUE_MESSAGE(esp_netif_is_netif_up(ctx.netif),
                             "netif must be up after confirm event");

    test_teardown(&ctx);
}

TEST_CASE("confirm event prior connect sets link up", "[sublayer_events]")
{
    ESP_LOGI(TAG, "--- confirm event posted before CONNECTED: netif must come up on CONNECTED ---");

    esp_eth_sublayer_config_t sub_cfg = ESP_ETH_SUBLAYER_CONFIG_DEFAULT();
    sub_cfg.connect_confirm_event = (esp_eth_sublayer_event_t){
        .base = TEST_SUBLAYER_EVENT,
        .event_id = TEST_CONFIRM_EVENT_ID,
    };

    events_test_ctx_t ctx;
    test_setup(&ctx, &sub_cfg);

    ESP_LOGI(TAG, "--- posting confirm event while the physical link is down ---");
    TEST_ESP_OK(esp_event_post(TEST_SUBLAYER_EVENT, TEST_CONFIRM_EVENT_ID,
                               NULL, 0, portMAX_DELAY));
    vTaskDelay(pdMS_TO_TICKS(EVENT_PROPAGATION_MS));

    TEST_ASSERT_FALSE_MESSAGE(esp_netif_is_netif_up(ctx.netif),
                              "netif must stay down when only the confirm event was received");

    test_start_and_wait_connect(&ctx);

    TEST_ASSERT_TRUE_MESSAGE(esp_netif_is_netif_up(ctx.netif),
                             "netif must be up after CONNECTED when the confirm event was received before");

    test_teardown(&ctx);
}

TEST_CASE("eth disconnect event sets link down", "[sublayer_events]")
{
    ESP_LOGI(TAG, "--- ETHERNET_EVENT_DISCONNECTED should bring netif down ---");

    esp_eth_sublayer_config_t sub_cfg = ESP_ETH_SUBLAYER_CONFIG_DEFAULT();

    events_test_ctx_t ctx;
    test_setup(&ctx, &sub_cfg);
    test_start_and_wait_connect(&ctx);

    TEST_ASSERT_TRUE_MESSAGE(esp_netif_is_netif_up(ctx.netif),
                             "precondition: netif must be up before disconnect test");

    esp_eth_handle_t hdl = ctx.base.eth_handles[0];
    TEST_ESP_OK(esp_event_post(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED,
                               &hdl, sizeof(hdl), portMAX_DELAY));
    vTaskDelay(pdMS_TO_TICKS(EVENT_PROPAGATION_MS));

    TEST_ASSERT_FALSE_MESSAGE(esp_netif_is_netif_up(ctx.netif),
                              "netif must be down after ETHERNET_EVENT_DISCONNECTED");

    test_teardown(&ctx);
}

TEST_CASE("custom disconnect trigger sets link down", "[sublayer_events]")
{
    ESP_LOGI(TAG, "--- custom disconnect trigger should bring netif down ---");

    esp_eth_sublayer_config_t sub_cfg = ESP_ETH_SUBLAYER_CONFIG_DEFAULT();
    sub_cfg.disconnect_trigger_event = (esp_eth_sublayer_event_t){
        .base = TEST_SUBLAYER_EVENT,
        .event_id = TEST_DISCONNECT_EVENT_ID,
    };

    events_test_ctx_t ctx;
    test_setup(&ctx, &sub_cfg);
    test_start_and_wait_connect(&ctx);

    TEST_ASSERT_TRUE_MESSAGE(esp_netif_is_netif_up(ctx.netif),
                             "precondition: netif must be up before disconnect trigger test");

    TEST_ESP_OK(esp_event_post(TEST_SUBLAYER_EVENT, TEST_DISCONNECT_EVENT_ID,
                               NULL, 0, portMAX_DELAY));
    vTaskDelay(pdMS_TO_TICKS(EVENT_PROPAGATION_MS));

    TEST_ASSERT_FALSE_MESSAGE(esp_netif_is_netif_up(ctx.netif),
                              "netif must be down after custom disconnect trigger");

    test_teardown(&ctx);
}
