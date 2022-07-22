/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_netif_net_stack.h"
#include "lwip/esp_netif_net_stack.h"
#include "lwip/dns.h"
#include "lwip/ip6_addr.h"
#include "lwip/netif.h"
#include "netif/slipif.h"
#include "lwip/sio.h"

static const char *TAG = "slip-modem-netif";

/**
 * @brief Stops the SLIP interface
 */
esp_err_t slip_modem_netif_stop(esp_netif_t *esp_netif)
{
    struct netif *netif = esp_netif_get_netif_impl(esp_netif);

    ESP_LOGI(TAG, "%s: Stopped SLIP connection: lwip netif:%p", __func__, netif);

    // Stop interface
    netif_set_link_down(netif);

    return ESP_OK;
}

/**
 * @brief Starts the SLIP interface
 */
esp_err_t slip_modem_netif_start(esp_netif_t *esp_netif, esp_ip6_addr_t *addr)
{
    struct netif *netif = esp_netif_get_netif_impl(esp_netif);

    ESP_LOGI(TAG, "%s: Starting SLIP interface: lwip netif:%p", __func__, netif);

    // Set the netif up
    netif_set_up(netif);
    netif_set_link_up(netif);
#if CONFIG_LWIP_IPV6
    int8_t addr_index = 0;

    netif_ip6_addr_set(netif, addr_index, (ip6_addr_t *)addr);
    netif_ip6_addr_set_state(netif, addr_index, IP6_ADDR_VALID);
#endif
    return ESP_OK;
}

/**
 * @brief Write incoming serial data to the SLIP interface
 */
void esp_netif_lwip_slip_input(void *h, void *buffer, unsigned int len, void *eb)
{
    struct netif *netif = h;

    ESP_LOGD(TAG, "%s", __func__);
    ESP_LOG_BUFFER_HEXDUMP(TAG, buffer, len, ESP_LOG_DEBUG);

    // Update slip netif with data
    const int max_batch = 255;
    int sent = 0;
    while(sent < len) {
        int batch = (len - sent) > max_batch ? max_batch : (len - sent);
        slipif_received_bytes(netif, buffer+sent, batch);
        sent += batch;
    }

    // Process incoming bytes
    for (int i = 0; i < len; i++) {
        slipif_process_rxqueue(netif);
    }
}

/**
 * @brief Write raw data out the SLIP interface
 */
void slip_modem_netif_raw_output(esp_netif_t *netif, void *buffer, size_t len)
{
    struct netif *lwip_netif = esp_netif_get_netif_impl(netif);

    ESP_LOGD(TAG, "%s", __func__);

    struct pbuf p = {
        .next = NULL,
        .payload = buffer,
        .tot_len = len,
        .len = len,
    };

    // Call slip if output function to feed data out slip interface
#if CONFIG_LWIP_IPV6
    lwip_netif->output_ip6(lwip_netif, &p, NULL);
#else
    lwip_netif->output(lwip_netif, &p, NULL);
#endif
}


/** @brief Get esp-netif object corresponding to registration index
 */
static esp_netif_t * get_netif_with_esp_index(int index)
{
    esp_netif_t *netif = NULL;
    int counter = 0;
    while ((netif = esp_netif_next(netif)) != NULL) {
        if (counter == index) {
            return netif;
        }
        counter++;
    }
    return NULL;
}

/** @brief Return list registration index of the supplied netif ptr
 */
static int get_esp_netif_index(esp_netif_t * esp_netif)
{
    esp_netif_t *netif = NULL;
    int counter = 0;
    while ((netif = esp_netif_next(netif)) != NULL) {
        if (esp_netif == netif) {
            return counter;
        }
        counter++;
    }
    return -1;
}

err_t esp_slipif_init(struct netif *netif)
{
    esp_netif_t *esp_netif = netif->state;
    int esp_index = get_esp_netif_index(esp_netif);
    if (esp_index < 0) {
        return ERR_IF;
    }

    // Store netif index in net interface for SIO open command to abstract the dev
    netif->state = (void *)esp_index;

    return slipif_init(netif);
}

const struct esp_netif_netstack_config s_netif_config_slip = {
        .lwip = {
                .init_fn = esp_slipif_init,
                .input_fn = esp_netif_lwip_slip_input,
        }
};

const esp_netif_netstack_config_t *netstack_default_slip = &s_netif_config_slip;


/***
 * @brief Open a serial device for communication
 */
sio_fd_t sio_open(uint8_t devnum)
{
    ESP_LOGD(TAG, "Opening device: %d\r\n", devnum);

    esp_netif_t *esp_netif = get_netif_with_esp_index(devnum);
    if (!esp_netif) {
        ESP_LOGE(TAG, "didn't find esp-netif with index=%d\n", devnum);
        return NULL;
    }

    // Return SIO handle
    return esp_netif;
}

/***
 * @brief Send a single character to the serial device (blocking)
 */
void sio_send(uint8_t c, sio_fd_t fd)
{
    esp_netif_t *esp_netif = fd;

    ESP_LOGD(TAG, "%s", __func__);
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, &c, 1, ESP_LOG_DEBUG);

    esp_err_t ret = esp_netif_transmit(esp_netif, &c, 1);
    if (ret != ESP_OK) {
        // Handle errors
        ESP_LOGD(TAG, "%s: uart_write_bytes error %i", __func__, ret);
    }
}
