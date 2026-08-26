/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* Path MTU probe example using ICMP ping payload search
 */

#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_check.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "protocol_examples_common.h"
#include "esp_netif.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "ping/ping_sock.h"

static const char *TAG = "pmtu_probe";

typedef struct {
    SemaphoreHandle_t done;
    bool got_reply;
} probe_ctx_t;

static void on_ping_success(esp_ping_handle_t hdl, void *args)
{
    probe_ctx_t *ctx = (probe_ctx_t *)args;
    ctx->got_reply = true;
}

static void on_ping_timeout(esp_ping_handle_t hdl, void *args)
{
    (void)hdl; (void)args;
}

static void on_ping_end(esp_ping_handle_t hdl, void *args)
{
    probe_ctx_t *ctx = (probe_ctx_t *)args;
    if (ctx && ctx->done) {
        xSemaphoreGive(ctx->done);
    }
}

static bool try_payload_once(const ip_addr_t *target, uint32_t payload_size)
{
    ESP_LOGI(TAG, "Trying payload once with size %u", payload_size);
    probe_ctx_t ctx = { .done = xSemaphoreCreateBinary(), .got_reply = false };
    if (!ctx.done) {
        return false;
    }
    esp_ping_config_t cfg = ESP_PING_DEFAULT_CONFIG();
    cfg.target_addr = *target;
    cfg.count = 1;
    cfg.timeout_ms = 1000;   // 1s timeout
    cfg.interval_ms = 100;   // not used for count=1
    cfg.data_size = payload_size;
    esp_ping_callbacks_t cbs = {
        .cb_args = &ctx,
        .on_ping_success = on_ping_success,
        .on_ping_timeout = on_ping_timeout,
        .on_ping_end = on_ping_end,
    };
    esp_ping_handle_t ping = NULL;
    if (esp_ping_new_session(&cfg, &cbs, &ping) != ESP_OK) {
        vSemaphoreDelete(ctx.done);
        return false;
    }
    esp_ping_start(ping);
    (void)xSemaphoreTake(ctx.done, pdMS_TO_TICKS(1500));
    esp_ping_delete_session(ping);
    vSemaphoreDelete(ctx.done);
    return ctx.got_reply;
}

static bool resolve_host(const char *host, ip_addr_t *out)
{
    memset(out, 0, sizeof(*out));
    struct sockaddr_in6 s6;
    if (inet_pton(AF_INET6, host, &s6.sin6_addr) == 1) {
        ipaddr_aton(host, out);
        return true;
    }
    struct addrinfo hint = { 0 }, *res = NULL;
    if (getaddrinfo(host, NULL, &hint, &res) != 0 || !res) {
        return false;
    }
#if CONFIG_LWIP_IPV4
    if (res->ai_family == AF_INET) {
        struct in_addr a4 = ((struct sockaddr_in *)res->ai_addr)->sin_addr;
        inet_addr_to_ip4addr(ip_2_ip4(out), &a4);
        freeaddrinfo(res);
        return true;
    }
#endif
    // Prefer IPv4 for simple MTU formula; fall back to first result
    ipaddr_aton(host, out);
    freeaddrinfo(res);
    return true;
}

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_LOGI(TAG, "Connecting network...");
    ESP_ERROR_CHECK(example_connect());

    const char *host = CONFIG_EXAMPLE_PROBE_HOST;
    ip_addr_t target;
    if (!resolve_host(host, &target)) {
        ESP_LOGE(TAG, "Failed to resolve host: %s", host);
        return;
    }

    esp_netif_t *netif = get_example_netif();
    if (!netif) {
        ESP_LOGE(TAG, "No default netif available");
        return;
    }

    ESP_LOGI(TAG, "Probing PMTU to host %s (IPv4)", host);

    uint16_t if_mtu = 0;
    uint32_t lo = 0;
    uint32_t hi = 1472; // fallback max payload (1500 - 28)
    if (esp_netif_get_mtu(netif, &if_mtu) == ESP_OK && if_mtu >= 28) {
        hi = (if_mtu >= 28) ? (if_mtu - 28) : 0;
    }
    ESP_LOGI(TAG, "Search range payload=[%u, %u]", (unsigned)lo, (unsigned)hi);

    uint32_t best = 0;
    while (lo <= hi) {
        uint32_t mid = lo + ((hi - lo) / 2);
        bool ok = try_payload_once(&target, mid);
        if (ok) {
            best = mid;
            lo = mid + 1;
        } else {
            if (mid == 0) {
                break; // can't go lower
            }
            hi = mid - 1;
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    uint32_t new_mtu = best + 28; // IPv4 header (20) + ICMP (8)
    if (new_mtu < 576) {
        // Be conservative; typical minimum IPv4 MTU recommendation
        new_mtu = 576;
    }
    ESP_LOGI(TAG, "Best payload=%u -> MTU=%u", (unsigned)best, (unsigned)new_mtu);
    ESP_LOGI(TAG, "Applying MTU %u to default netif", (unsigned)new_mtu);
    esp_err_t err = esp_netif_set_mtu(netif, (uint16_t)new_mtu);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "Failed to set MTU: %s", esp_err_to_name(err));
    }
}
