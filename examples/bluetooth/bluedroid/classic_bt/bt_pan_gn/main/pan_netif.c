/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdlib.h>
#include <string.h>

#include "esp_log.h"
#include "esp_netif.h"
#include "esp_netif_types.h"
#include "esp_pan_api.h"
#include "freertos/FreeRTOS.h"
#include "lwip/inet.h"

#include "pan_netif.h"

#define PAN_NETIF_TAG           "PAN_NETIF"
#define ETH_HEADER_LEN          14

typedef struct {
    uint16_t handle;
    uint8_t peer_mac[6];
} pan_link_t;

typedef struct {
    esp_netif_driver_base_t base;
    pan_link_t links[PAN_NETIF_MAX_LINKS];
    uint8_t num_handles;
    uint8_t mac[6];
    portMUX_TYPE lock;
} pan_netif_glue_t;

static esp_netif_t *s_pan_netif = NULL;
static pan_netif_glue_t *s_pan_glue = NULL;
static bool s_netif_up = false;

/* Match example main.c: Ethernet MAC is the peer BD_ADDR (PAN/BNEP). */
static void bdaddr_to_eth_mac(const uint8_t *bda, uint8_t *mac)
{
    memcpy(mac, bda, 6);
}

static bool pan_link_add(pan_netif_glue_t *glue, uint16_t handle, const uint8_t *peer_mac)
{
    bool ok = true;

    portENTER_CRITICAL(&glue->lock);
    for (int i = 0; i < glue->num_handles; i++) {
        if (glue->links[i].handle == handle) {
            if (peer_mac) {
                memcpy(glue->links[i].peer_mac, peer_mac, 6);
            }
            portEXIT_CRITICAL(&glue->lock);
            return true;
        }
    }
    if (glue->num_handles >= PAN_NETIF_MAX_LINKS) {
        ok = false;
    } else {
        pan_link_t *link = &glue->links[glue->num_handles++];
        link->handle = handle;
        if (peer_mac) {
            memcpy(link->peer_mac, peer_mac, 6);
        } else {
            memset(link->peer_mac, 0, 6);
        }
    }
    portEXIT_CRITICAL(&glue->lock);

    if (!ok) {
        ESP_LOGW(PAN_NETIF_TAG, "Too many PAN links (max %d)", PAN_NETIF_MAX_LINKS);
    }
    return ok;
}

static void pan_link_remove(pan_netif_glue_t *glue, uint16_t handle)
{
    portENTER_CRITICAL(&glue->lock);
    for (int i = 0; i < glue->num_handles; i++) {
        if (glue->links[i].handle == handle) {
            for (int j = i; j < glue->num_handles - 1; j++) {
                glue->links[j] = glue->links[j + 1];
            }
            glue->num_handles--;
            break;
        }
    }
    portEXIT_CRITICAL(&glue->lock);
}

static void pan_link_learn_mac(pan_netif_glue_t *glue, uint16_t handle, const uint8_t *peer_mac)
{
    if (peer_mac == NULL) {
        return;
    }
    portENTER_CRITICAL(&glue->lock);
    for (int i = 0; i < glue->num_handles; i++) {
        if (glue->links[i].handle == handle) {
            memcpy(glue->links[i].peer_mac, peer_mac, 6);
            break;
        }
    }
    portEXIT_CRITICAL(&glue->lock);
}

static uint16_t pan_link_lookup_by_mac(const pan_link_t *links, uint8_t num_handles, const uint8_t *dst_mac)
{
    for (uint8_t i = 0; i < num_handles; i++) {
        if (memcmp(links[i].peer_mac, dst_mac, 6) == 0) {
            return links[i].handle;
        }
    }
    return ESP_PAN_INVALID_HANDLE;
}

static esp_err_t pan_send_frame(uint16_t handle,
                                uint8_t *dst, uint8_t *src, uint16_t protocol,
                                uint8_t *payload, uint16_t payload_len)
{
    if (handle == ESP_PAN_INVALID_HANDLE) {
        return ESP_ERR_INVALID_STATE;
    }
    esp_err_t ret = esp_pan_write(handle, dst, src, protocol, payload_len, payload, false);
    return (ret == ESP_OK) ? ESP_OK : ESP_FAIL;
}

static esp_err_t pan_transmit(void *handle, void *data, size_t len)
{
    pan_netif_glue_t *glue = (pan_netif_glue_t *)handle;
    pan_link_t links_snapshot[PAN_NETIF_MAX_LINKS];
    uint8_t num_handles;

    if (glue == NULL || data == NULL || len < ETH_HEADER_LEN) {
        return ESP_ERR_INVALID_STATE;
    }

    uint8_t *frame = (uint8_t *)data;
    uint16_t protocol = ((uint16_t)frame[12] << 8) | frame[13];
    uint16_t payload_len = (uint16_t)(len - ETH_HEADER_LEN);
    uint8_t *payload = frame + ETH_HEADER_LEN;
    bool is_bcast = (frame[0] & 0x01) != 0;

    portENTER_CRITICAL(&glue->lock);
    num_handles = glue->num_handles;
    if (num_handles > 0) {
        memcpy(links_snapshot, glue->links, num_handles * sizeof(links_snapshot[0]));
    }
    portEXIT_CRITICAL(&glue->lock);

    if (num_handles == 0) {
        return ESP_ERR_INVALID_STATE;
    }

    if (is_bcast) {
        esp_err_t last = ESP_OK;
        for (int i = 0; i < num_handles; i++) {
            esp_err_t ret = pan_send_frame(links_snapshot[i].handle, frame, frame + 6,
                                           protocol, payload, payload_len);
            if (ret != ESP_OK) {
                last = ret;
            }
        }
        return last;
    }

    uint16_t out_handle = pan_link_lookup_by_mac(links_snapshot, num_handles, frame);
    if (out_handle == ESP_PAN_INVALID_HANDLE) {
        if (num_handles == 1) {
            out_handle = links_snapshot[0].handle;
        } else {
            ESP_LOGW(PAN_NETIF_TAG, "No link for dst MAC %02x:%02x:%02x:%02x:%02x:%02x",
                     frame[0], frame[1], frame[2], frame[3], frame[4], frame[5]);
            return ESP_ERR_NOT_FOUND;
        }
    }
    return pan_send_frame(out_handle, frame, frame + 6, protocol, payload, payload_len);
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
    ESP_LOGI(PAN_NETIF_TAG, "PAN netif MAC %02x:%02x:%02x:%02x:%02x:%02x",
             glue->mac[0], glue->mac[1], glue->mac[2], glue->mac[3], glue->mac[4], glue->mac[5]);
    return ESP_OK;
}

static void pan_apply_static_ip(esp_netif_t *netif, const char *ip, const char *mask, const char *gw)
{
    esp_netif_ip_info_t ip_info = {0};

    ip_info.ip.addr = ipaddr_addr(ip);
    ip_info.netmask.addr = ipaddr_addr(mask);
    ip_info.gw.addr = ipaddr_addr(gw);
    esp_netif_dhcpc_stop(netif);
    esp_netif_set_ip_info(netif, &ip_info);
    ESP_LOGI(PAN_NETIF_TAG, "Static IP %s, mask %s, gw %s", ip, mask, gw);
}

esp_err_t pan_netif_init(const pan_netif_cfg_t *cfg)
{
    if (s_pan_netif != NULL || cfg == NULL || cfg->mac == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    s_pan_glue = calloc(1, sizeof(pan_netif_glue_t));
    if (s_pan_glue == NULL) {
        return ESP_ERR_NO_MEM;
    }

    s_pan_glue->base.post_attach = pan_post_attach;
    s_pan_glue->lock = (portMUX_TYPE)portMUX_INITIALIZER_UNLOCKED;
    memcpy(s_pan_glue->mac, cfg->mac, 6);

    esp_netif_inherent_config_t base_cfg = ESP_NETIF_INHERENT_DEFAULT_ETH();
    base_cfg.if_key = "PAN_GN";
    base_cfg.if_desc = "pan_gn";
    base_cfg.route_prio = 60;

    esp_netif_config_t netif_cfg = {
        .base = &base_cfg,
        .stack = ESP_NETIF_NETSTACK_DEFAULT_ETH,
    };

    s_pan_netif = esp_netif_new(&netif_cfg);
    if (s_pan_netif == NULL) {
        free(s_pan_glue);
        s_pan_glue = NULL;
        return ESP_ERR_NO_MEM;
    }

    ESP_ERROR_CHECK(esp_netif_attach(s_pan_netif, s_pan_glue));
    pan_apply_static_ip(s_pan_netif, cfg->ip_str, cfg->netmask_str, cfg->gw_str);
    return ESP_OK;
}

esp_netif_t *pan_netif_get(void)
{
    return s_pan_netif;
}

void pan_netif_on_connected(uint16_t handle, const uint8_t *peer_bda)
{
    uint8_t peer_mac[6] = {0};

    if (s_pan_glue == NULL || s_pan_netif == NULL) {
        return;
    }

    if (peer_bda) {
        bdaddr_to_eth_mac(peer_bda, peer_mac);
    }

    if (!pan_link_add(s_pan_glue, handle, peer_mac)) {
        return;
    }

    esp_netif_set_default_netif(s_pan_netif);
    if (!s_netif_up) {
        esp_netif_action_start(s_pan_netif, 0, 0, 0);
        esp_netif_action_connected(s_pan_netif, 0, 0, 0);
        s_netif_up = true;
    }

    uint8_t num_handles;
    portENTER_CRITICAL(&s_pan_glue->lock);
    num_handles = s_pan_glue->num_handles;
    portEXIT_CRITICAL(&s_pan_glue->lock);
    ESP_LOGI(PAN_NETIF_TAG, "PAN link up, handle=%u, links=%u", handle, num_handles);
}

void pan_netif_on_disconnected(uint16_t handle)
{
    if (s_pan_glue == NULL || s_pan_netif == NULL) {
        return;
    }

    pan_link_remove(s_pan_glue, handle);

    uint8_t num_handles;
    portENTER_CRITICAL(&s_pan_glue->lock);
    num_handles = s_pan_glue->num_handles;
    portEXIT_CRITICAL(&s_pan_glue->lock);
    ESP_LOGI(PAN_NETIF_TAG, "PAN link down, handle=%u, links=%u", handle, num_handles);

    if (num_handles == 0 && s_netif_up) {
        esp_netif_action_disconnected(s_pan_netif, 0, 0, 0);
        esp_netif_action_stop(s_pan_netif, 0, 0, 0);
        s_netif_up = false;
    }
}

esp_err_t pan_netif_input(uint16_t handle, const uint8_t *dst, const uint8_t *src, uint16_t protocol,
                          const uint8_t *payload, uint16_t len)
{
    if (s_pan_netif == NULL || s_pan_glue == NULL || payload == NULL || len == 0) {
        return ESP_ERR_INVALID_STATE;
    }

    /* Learn the peer Ethernet MAC from the frame source for unicast TX routing. */
    pan_link_learn_mac(s_pan_glue, handle, src);

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
