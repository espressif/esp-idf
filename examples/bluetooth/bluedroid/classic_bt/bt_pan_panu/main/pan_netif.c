/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_netif_types.h"
#include "esp_pan_api.h"
#include "lwip/inet.h"
#include "ping/ping_sock.h"

#include "pan_netif.h"

#define PAN_NETIF_TAG           "PAN_NETIF"
#define ETH_HEADER_LEN          14

typedef struct {
    esp_netif_driver_base_t base;
    uint16_t pan_handle;
    uint8_t mac[6];
} pan_netif_glue_t;

static esp_netif_t *s_pan_netif = NULL;
static pan_netif_glue_t *s_pan_glue = NULL;
static esp_ping_handle_t s_ping = NULL;
static bool s_ping_started = false;

static esp_err_t pan_transmit(void *handle, void *data, size_t len)
{
    pan_netif_glue_t *glue = (pan_netif_glue_t *)handle;

    if (glue == NULL || glue->pan_handle == ESP_PAN_INVALID_HANDLE || data == NULL || len < ETH_HEADER_LEN) {
        return ESP_ERR_INVALID_STATE;
    }

    uint8_t *frame = (uint8_t *)data;
    uint16_t protocol = ((uint16_t)frame[12] << 8) | frame[13];
    uint16_t payload_len = (uint16_t)(len - ETH_HEADER_LEN);

    esp_err_t ret = esp_pan_write(glue->pan_handle, frame, frame + 6, protocol,
                                  payload_len, frame + ETH_HEADER_LEN, false);
    return (ret == ESP_OK) ? ESP_OK : ESP_FAIL;
}

static void pan_free_rx_buffer(void *handle, void *buffer)
{
    (void)handle;
    free(buffer);
}

static esp_err_t pan_post_attach(esp_netif_t *esp_netif, void *args)
{
    pan_netif_glue_t *glue = (pan_netif_glue_t *)args;

    glue->base.netif = esp_netif;

    esp_netif_driver_ifconfig_t driver_cfg = {
        .handle = glue,
        .transmit = pan_transmit,
        .driver_free_rx_buffer = pan_free_rx_buffer,
    };

    ESP_ERROR_CHECK(esp_netif_set_driver_config(esp_netif, &driver_cfg));
    ESP_ERROR_CHECK(esp_netif_set_mac(esp_netif, glue->mac));
    ESP_LOGI(PAN_NETIF_TAG, "PAN netif attached, MAC %02x:%02x:%02x:%02x:%02x:%02x",
             glue->mac[0], glue->mac[1], glue->mac[2], glue->mac[3], glue->mac[4], glue->mac[5]);
    return ESP_OK;
}

static void pan_ping_on_success(esp_ping_handle_t hdl, void *args)
{
    uint8_t ttl;
    uint16_t seqno;
    uint32_t elapsed_time, recv_len;
    ip_addr_t target_addr;

    esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
    esp_ping_get_profile(hdl, ESP_PING_PROF_TTL, &ttl, sizeof(ttl));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    esp_ping_get_profile(hdl, ESP_PING_PROF_SIZE, &recv_len, sizeof(recv_len));
    esp_ping_get_profile(hdl, ESP_PING_PROF_TIMEGAP, &elapsed_time, sizeof(elapsed_time));
    ESP_LOGI(PAN_NETIF_TAG, "%" PRIu32 " bytes from %s icmp_seq=%u ttl=%u time=%" PRIu32 " ms",
             recv_len, ipaddr_ntoa(&target_addr), seqno, ttl, elapsed_time);
}

static void pan_ping_on_timeout(esp_ping_handle_t hdl, void *args)
{
    uint16_t seqno;
    ip_addr_t target_addr;

    esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    ESP_LOGW(PAN_NETIF_TAG, "From %s icmp_seq=%u timeout", ipaddr_ntoa(&target_addr), seqno);
}

static void pan_ping_on_end(esp_ping_handle_t hdl, void *args)
{
    ip_addr_t target_addr;
    uint32_t transmitted;
    uint32_t received;
    uint32_t total_time_ms;

    esp_ping_get_profile(hdl, ESP_PING_PROF_REQUEST, &transmitted, sizeof(transmitted));
    esp_ping_get_profile(hdl, ESP_PING_PROF_REPLY, &received, sizeof(received));
    esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
    esp_ping_get_profile(hdl, ESP_PING_PROF_DURATION, &total_time_ms, sizeof(total_time_ms));

    uint32_t loss = transmitted ? (uint32_t)((1.0f - ((float)received / transmitted)) * 100) : 100;
    ESP_LOGI(PAN_NETIF_TAG, "--- %s ping statistics ---", ipaddr_ntoa(&target_addr));
    ESP_LOGI(PAN_NETIF_TAG, "%" PRIu32 " packets transmitted, %" PRIu32 " received, %" PRIu32 "%% packet loss, time %" PRIu32 " ms",
             transmitted, received, loss, total_time_ms);

    if (s_ping == hdl) {
        s_ping = NULL;
        s_ping_started = false;
    }
    esp_ping_delete_session(hdl);
}

static void pan_stop_ping(void)
{
    if (s_ping == NULL) {
        s_ping_started = false;
        return;
    }

    esp_ping_handle_t hdl = s_ping;
    s_ping = NULL;
    s_ping_started = false;
    esp_ping_stop(hdl);
    esp_ping_delete_session(hdl);
}

static void pan_start_ping(void)
{
    if (s_ping_started || s_pan_netif == NULL) {
        return;
    }

    ip_addr_t target_addr = {0};
    if (!ipaddr_aton(CONFIG_EXAMPLE_PING_TARGET, &target_addr)) {
        ESP_LOGE(PAN_NETIF_TAG, "Invalid ping target: %s", CONFIG_EXAMPLE_PING_TARGET);
        return;
    }

    esp_ping_config_t config = ESP_PING_DEFAULT_CONFIG();
    config.target_addr = target_addr;
    config.count = CONFIG_EXAMPLE_PING_COUNT;
    config.timeout_ms = CONFIG_EXAMPLE_PING_TIMEOUT_MS;
    config.interface = esp_netif_get_netif_impl_index(s_pan_netif);

    esp_ping_callbacks_t cbs = {
        .on_ping_success = pan_ping_on_success,
        .on_ping_timeout = pan_ping_on_timeout,
        .on_ping_end = pan_ping_on_end,
    };

    esp_ping_handle_t ping = NULL;
    if (esp_ping_new_session(&config, &cbs, &ping) != ESP_OK) {
        ESP_LOGE(PAN_NETIF_TAG, "Failed to create ping session");
        return;
    }

    s_ping = ping;
    s_ping_started = true;
    ESP_LOGI(PAN_NETIF_TAG, "Start ping %s (%u packets)", CONFIG_EXAMPLE_PING_TARGET, config.count);
    esp_ping_start(ping);
}

static void pan_on_got_ip(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(PAN_NETIF_TAG, "Got IP: " IPSTR ", mask: " IPSTR ", gw: " IPSTR,
             IP2STR(&event->ip_info.ip), IP2STR(&event->ip_info.netmask), IP2STR(&event->ip_info.gw));
    pan_start_ping();
}

static void pan_on_lost_ip(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ESP_LOGW(PAN_NETIF_TAG, "IP lost");
    pan_stop_ping();
}

esp_err_t pan_netif_init(const uint8_t *mac)
{
    if (s_pan_netif != NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    s_pan_glue = calloc(1, sizeof(pan_netif_glue_t));
    if (s_pan_glue == NULL) {
        return ESP_ERR_NO_MEM;
    }

    s_pan_glue->base.post_attach = pan_post_attach;
    s_pan_glue->pan_handle = ESP_PAN_INVALID_HANDLE;
    memcpy(s_pan_glue->mac, mac, 6);

    esp_netif_inherent_config_t base_cfg = ESP_NETIF_INHERENT_DEFAULT_ETH();
    base_cfg.if_key = "PAN_DEF";
    base_cfg.if_desc = "pan";
    base_cfg.route_prio = 60;

    esp_netif_config_t cfg = {
        .base = &base_cfg,
        .stack = ESP_NETIF_NETSTACK_DEFAULT_ETH,
    };

    s_pan_netif = esp_netif_new(&cfg);
    if (s_pan_netif == NULL) {
        free(s_pan_glue);
        s_pan_glue = NULL;
        return ESP_ERR_NO_MEM;
    }

    ESP_ERROR_CHECK(esp_netif_attach(s_pan_netif, s_pan_glue));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, pan_on_got_ip, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_LOST_IP, pan_on_lost_ip, NULL));

    return ESP_OK;
}

esp_netif_t *pan_netif_get(void)
{
    return s_pan_netif;
}

void pan_netif_on_connected(uint16_t handle)
{
    if (s_pan_glue == NULL || s_pan_netif == NULL) {
        return;
    }

    s_pan_glue->pan_handle = handle;
    pan_stop_ping();
    esp_netif_set_default_netif(s_pan_netif);
    esp_netif_action_start(s_pan_netif, 0, 0, 0);
    esp_netif_action_connected(s_pan_netif, 0, 0, 0);
    ESP_LOGI(PAN_NETIF_TAG, "PAN link up, DHCP client started");
}

void pan_netif_on_disconnected(void)
{
    if (s_pan_glue == NULL || s_pan_netif == NULL) {
        return;
    }

    s_pan_glue->pan_handle = ESP_PAN_INVALID_HANDLE;
    pan_stop_ping();
    esp_netif_action_disconnected(s_pan_netif, 0, 0, 0);
    esp_netif_action_stop(s_pan_netif, 0, 0, 0);
    ESP_LOGI(PAN_NETIF_TAG, "PAN link down");
}

esp_err_t pan_netif_input(const uint8_t *dst, const uint8_t *src, uint16_t protocol,
                          const uint8_t *payload, uint16_t len)
{
    if (s_pan_netif == NULL || payload == NULL || len == 0) {
        return ESP_ERR_INVALID_STATE;
    }

    size_t frame_len = ETH_HEADER_LEN + len;
    uint8_t *frame = malloc(frame_len);
    if (frame == NULL) {
        return ESP_ERR_NO_MEM;
    }

    memcpy(frame, dst, 6);
    memcpy(frame + 6, src, 6);
    frame[12] = (protocol >> 8) & 0xff;
    frame[13] = protocol & 0xff;
    memcpy(frame + ETH_HEADER_LEN, payload, len);

    /* esp_netif_receive takes ownership of frame and frees it via
     * pan_free_rx_buffer on both success and failure paths. */
    return esp_netif_receive(s_pan_netif, frame, frame_len, NULL);
}
