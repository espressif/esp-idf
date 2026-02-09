/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <esp_private/wifi.h>
#include "esp_err.h"
#include "esp_wifi_remote.h"
#include "esp_log.h"

#define CHANNELS 2
#define WEAK __attribute__((weak))

static esp_remote_channel_tx_fn_t s_tx_cb[CHANNELS];
static esp_remote_channel_t s_channel[CHANNELS];
static wifi_rxcb_t s_rx_fn[CHANNELS];

WEAK esp_err_t esp_wifi_remote_channel_rx(void *h, void *buffer, void *buff_to_free, size_t len)
{
    assert(h);
    if (h == s_channel[0] && s_rx_fn[0]) {
        return s_rx_fn[0](buffer, len, buff_to_free);
    }
    if (h == s_channel[1] && s_rx_fn[1]) {
        return s_rx_fn[1](buffer, len, buff_to_free);
    }
    esp_wifi_internal_free_rx_buffer(buff_to_free);
    return ESP_FAIL;
}

WEAK esp_err_t esp_wifi_remote_channel_set(wifi_interface_t ifx, void *h, esp_remote_channel_tx_fn_t tx_cb)
{
    if (ifx == WIFI_IF_STA) {
        s_channel[0] = h;
        s_tx_cb[0] = tx_cb;
        return ESP_OK;
    }
    if (ifx == WIFI_IF_AP) {
        s_channel[1] = h;
        s_tx_cb[1] = tx_cb;
        return ESP_OK;
    }
    return ESP_FAIL;
}

WEAK esp_err_t esp_wifi_internal_set_sta_ip(void)
{
    // TODO: Pass this information to the slave target
    // Note that this function is called from the default event loop, so we shouldn't block here
    return ESP_OK;
}

WEAK esp_err_t esp_wifi_internal_reg_netstack_buf_cb(wifi_netstack_buf_ref_cb_t ref, wifi_netstack_buf_free_cb_t free)
{
    return ESP_OK;
}

WEAK void esp_wifi_internal_free_rx_buffer(void *buffer)
{
    free(buffer);
}

WEAK esp_err_t esp_wifi_internal_tx_by_ref(wifi_interface_t ifx, void *buffer, size_t len, void *netstack_buf)
{
    return esp_wifi_internal_tx(ifx, buffer, (uint16_t)len);
}

WEAK int esp_wifi_internal_tx(wifi_interface_t ifx, void *buffer, uint16_t len)
{
    if (ifx == WIFI_IF_STA && s_tx_cb[0]) {

        /* TODO: If not needed, remove arg3 */
        return s_tx_cb[0](s_channel[0], buffer, len);
    }
    if (ifx == WIFI_IF_AP && s_tx_cb[1]) {
        return s_tx_cb[1](s_channel[1], buffer, len);
    }

    return -1;
}

WEAK esp_err_t esp_wifi_internal_reg_rxcb(wifi_interface_t ifx, wifi_rxcb_t fn)
{
    if (ifx == WIFI_IF_STA) {
        ESP_LOGI("esp_wifi_remote", "%s: sta: %p", __func__, fn);
        s_rx_fn[0] = fn;
        return ESP_OK;
    }
    if (ifx == WIFI_IF_AP) {
        s_rx_fn[1] = fn;
        return ESP_OK;
    }

    return ESP_FAIL;
}
