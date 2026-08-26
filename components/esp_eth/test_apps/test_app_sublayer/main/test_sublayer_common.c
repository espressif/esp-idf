/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <string.h>
#include "test_sublayer_common.h"
#include "esp_vfs_l2tap.h"
#include "esp_log.h"
#include "unity.h"
#include "ethernet_init.h"

static const char *TAG = "sublayer_test_common";

static sublayer_test_ctx_t *s_active_ctx;

/* ------------------------------------------------------------------ */
/*  ETH event handler                                                 */
/* ------------------------------------------------------------------ */

static void eth_event_handler(void *arg, esp_event_base_t base,
                              int32_t event_id, void *event_data)
{
    EventGroupHandle_t eg = (EventGroupHandle_t)arg;
    (void)base;
    (void)event_data;
    switch (event_id) {
    case ETHERNET_EVENT_START:
        xEventGroupSetBits(eg, SUBLAYER_TEST_ETH_START_BIT);
        break;
    case ETHERNET_EVENT_CONNECTED:
        xEventGroupSetBits(eg, SUBLAYER_TEST_ETH_CONNECT_BIT);
        break;
    case ETHERNET_EVENT_STOP:
        xEventGroupSetBits(eg, SUBLAYER_TEST_ETH_STOP_BIT);
        break;
    default:
        break;
    }
}

/* ------------------------------------------------------------------ */
/*  Granular init helpers                                              */
/* ------------------------------------------------------------------ */

void sublayer_test_create_event_loop(sublayer_test_ctx_t *ctx)
{
    TEST_ESP_OK(esp_netif_init());

    if (esp_event_loop_create_default() == ESP_ERR_INVALID_STATE) {
        esp_event_loop_delete_default();
        TEST_ESP_OK(esp_event_loop_create_default());
    }

    ctx->eg = xEventGroupCreate();
    TEST_ASSERT_NOT_NULL(ctx->eg);

    TEST_ESP_OK(esp_event_handler_instance_register(ETH_EVENT, ESP_EVENT_ANY_ID,
                                                    eth_event_handler, ctx->eg,
                                                    &ctx->eth_evt_inst));
}

void sublayer_test_init_ethernet(sublayer_test_ctx_t *ctx)
{
    TEST_ESP_OK(ethernet_init_all(&ctx->eth_handles, &ctx->eth_cnt));
    TEST_ASSERT_GREATER_THAN(0, ctx->eth_cnt);
}

void sublayer_test_register_l2tap(sublayer_test_ctx_t *ctx)
{
    esp_err_t ret = esp_vfs_l2tap_intf_register(NULL);
    if (ret == ESP_ERR_INVALID_STATE) {
        esp_vfs_l2tap_intf_unregister(NULL);
        TEST_ESP_OK(esp_vfs_l2tap_intf_register(NULL));
    } else {
        TEST_ESP_OK(ret);
    }
    ctx->l2tap_registered = true;
}

esp_netif_t *sublayer_test_create_eth_netif(sublayer_test_ctx_t *ctx,
                                            const char *if_key,
                                            const char *if_desc)
{
    esp_netif_inherent_config_t base_cfg = ESP_NETIF_INHERENT_DEFAULT_ETH();
    base_cfg.if_key = if_key;
    base_cfg.if_desc = if_desc;
    esp_netif_config_t netif_cfg = {
        .base = &base_cfg,
        .driver = NULL,
        .stack = ESP_NETIF_NETSTACK_DEFAULT_ETH,
    };
    esp_netif_t *netif = esp_netif_new(&netif_cfg);
    TEST_ASSERT_NOT_NULL(netif);

    TEST_ASSERT_LESS_THAN(SUBLAYER_TEST_MAX_NETIFS, ctx->netif_count);
    ctx->netifs[ctx->netif_count++] = netif;
    return netif;
}

void sublayer_test_start_and_wait_connect(sublayer_test_ctx_t *ctx)
{
    bool loopback_en = true;
    esp_eth_ioctl(ctx->eth_handles[0], ETH_CMD_S_PHY_LOOPBACK, &loopback_en);

    TEST_ESP_OK(esp_eth_start(ctx->eth_handles[0]));

    EventBits_t bits;
    bits = xEventGroupWaitBits(ctx->eg, SUBLAYER_TEST_ETH_START_BIT, pdTRUE, pdTRUE,
                               pdMS_TO_TICKS(SUBLAYER_TEST_ETH_START_TIMEOUT_MS));
    TEST_ASSERT_BITS(SUBLAYER_TEST_ETH_START_BIT, SUBLAYER_TEST_ETH_START_BIT, bits);

    bits = xEventGroupWaitBits(ctx->eg, SUBLAYER_TEST_ETH_CONNECT_BIT, pdTRUE, pdTRUE,
                               pdMS_TO_TICKS(SUBLAYER_TEST_ETH_CONNECT_TIMEOUT_MS));
    TEST_ASSERT_BITS(SUBLAYER_TEST_ETH_CONNECT_BIT, SUBLAYER_TEST_ETH_CONNECT_BIT, bits);

    vTaskDelay(pdMS_TO_TICKS(SUBLAYER_TEST_EVENT_PROPAGATION_MS));
}

/* ------------------------------------------------------------------ */
/*  Teardown                                                          */
/* ------------------------------------------------------------------ */

void sublayer_test_teardown(sublayer_test_ctx_t *ctx)
{
    if (ctx->eth_handles != NULL && ctx->eth_cnt > 0) {
        esp_eth_stop(ctx->eth_handles[0]);
        if (ctx->eg != NULL) {
            xEventGroupWaitBits(ctx->eg, SUBLAYER_TEST_ETH_STOP_BIT, pdTRUE, pdTRUE,
                                pdMS_TO_TICKS(SUBLAYER_TEST_ETH_STOP_TIMEOUT_MS));
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    for (int i = 0; i < ctx->netif_count; i++) {
        if (ctx->netifs[i] != NULL) {
            esp_netif_destroy(ctx->netifs[i]);
            ctx->netifs[i] = NULL;
        }
    }
    ctx->netif_count = 0;

    if (ctx->sub != NULL) {
        esp_eth_sublayer_del(ctx->sub);
        ctx->sub = NULL;
    }

    if (ctx->eth_evt_inst != NULL) {
        esp_event_handler_instance_unregister(ETH_EVENT, ESP_EVENT_ANY_ID, ctx->eth_evt_inst);
        ctx->eth_evt_inst = NULL;
    }

    if (ctx->eth_handles != NULL) {
        ethernet_deinit_all(ctx->eth_handles);
        ctx->eth_handles = NULL;
        ctx->eth_cnt = 0;
    }

    if (ctx->l2tap_registered) {
        esp_vfs_l2tap_intf_unregister(NULL);
        ctx->l2tap_registered = false;
    }

    if (ctx->eg != NULL) {
        vEventGroupDelete(ctx->eg);
        ctx->eg = NULL;
    }

    esp_event_loop_delete_default();
}

/* ------------------------------------------------------------------ */
/*  Force-teardown safety net                                         */
/* ------------------------------------------------------------------ */

void sublayer_test_register_ctx(sublayer_test_ctx_t *ctx)
{
    s_active_ctx = ctx;
}

void sublayer_test_unregister_ctx(void)
{
    s_active_ctx = NULL;
}

void sublayer_test_force_teardown(void)
{
    if (s_active_ctx == NULL) {
        return;
    }

    ESP_LOGW(TAG, "Force teardown triggered — cleaning up after test failure");
    sublayer_test_teardown(s_active_ctx);
    s_active_ctx = NULL;
}
