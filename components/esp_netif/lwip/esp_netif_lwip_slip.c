// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.



#include "esp_netif.h"
#include "esp_log.h"
#include "esp_netif_slip.h"
#include "esp_netif_lwip_internal.h"
#include "esp_netif_net_stack.h"

#ifdef CONFIG_ESP_NETIF_TCPIP_LWIP

#include "lwip/dns.h"
#include "lwip/opt.h"
#include "lwip/ip6_addr.h"
#include "lwip/netif.h"
#include "netif/slipif.h"
#include "lwip/sio.h"

#include <string.h>

static const char *TAG = "esp-netif_lwip-slip";

/**
 * @brief LWIP SLIP context object extends esp-netif related data
 */
typedef struct lwip_slip_ctx {
    //! Generic esp-netif related data
    netif_related_data_t base;

    //! SLIP interface IP6 address
    esp_ip6_addr_t addr;

} lwip_slip_ctx_t;

/**
 * @brief Create a new lwip slip interface
 */
netif_related_data_t * esp_netif_new_slip(esp_netif_t *esp_netif, const esp_netif_netstack_config_t *esp_netif_stack_config)
{
    ESP_LOGD(TAG, "%s", __func__);

    // Fetch netif and create context
    struct netif *netif_impl = esp_netif->lwip_netif;

    lwip_slip_ctx_t *slip_ctx = calloc(1, sizeof(lwip_slip_ctx_t));
    if (slip_ctx == NULL) {
        ESP_LOGE(TAG, "%s: cannot allocate lwip_slip_ctx_t", __func__);
        return NULL;
    }
    // Setup the generic esp-netif fields
    slip_ctx->base.is_point2point = true;
    slip_ctx->base.netif_type = SLIP_LWIP_NETIF;

    ESP_LOGD(TAG, "%s: Initialising SLIP (esp_netif %p, lwip_netif %p)", __func__, esp_netif, netif_impl);

    ESP_LOGI(TAG, "%s: Created SLIP interface (netif %p, slip_ctx: %p)", __func__, esp_netif, slip_ctx);

    return (netif_related_data_t *)slip_ctx;
}

/**
 * @brief Stops the SLIP interface
 */
esp_err_t esp_netif_stop_slip(esp_netif_t *esp_netif)
{
    lwip_slip_ctx_t *slip_ctx = (lwip_slip_ctx_t *)esp_netif->related_data;
    assert(slip_ctx->base.netif_type == SLIP_LWIP_NETIF);

    ESP_LOGI(TAG, "%s: Stopped SLIP connection: %p", __func__, slip_ctx);

    // Stop interface
    netif_set_link_down(esp_netif->lwip_netif);

    return ESP_OK;
}

/**
 * @brief Starts the SLIP interface
 */
esp_err_t esp_netif_start_slip(esp_netif_t *esp_netif)
{
    lwip_slip_ctx_t *slip_ctx = (lwip_slip_ctx_t *)esp_netif->related_data;
    assert(slip_ctx->base.netif_type == SLIP_LWIP_NETIF);

    ESP_LOGI(TAG, "%s: Starting SLIP interface: %p", __func__, slip_ctx);

    // Set the netif up
    netif_set_up(esp_netif->lwip_netif);
    netif_set_link_up(esp_netif->lwip_netif);
#if CONFIG_LWIP_IPV6
    int8_t addr_index = 0;

    netif_ip6_addr_set(esp_netif->lwip_netif, addr_index, (ip6_addr_t *)&slip_ctx->addr);
    netif_ip6_addr_set_state(esp_netif->lwip_netif, addr_index, IP6_ADDR_VALID);
#endif
    return ESP_OK;
}


/**
 * @brief Sets paramaters for the supplied netif
 */
esp_err_t esp_netif_slip_set_params(esp_netif_t *netif, const esp_netif_slip_config_t *slip_config)
{

    lwip_slip_ctx_t *slip_ctx = (lwip_slip_ctx_t *)netif->related_data;
    assert(slip_ctx->base.netif_type == SLIP_LWIP_NETIF);

    ESP_LOGD(TAG, "%s (slip_ctx: %p)", __func__, slip_ctx);

    if (netif_is_link_up(netif->lwip_netif)) {
        ESP_LOGE(TAG, "Cannot set parameters while SLIP interface is running");
        return ESP_ERR_INVALID_STATE;
    }

    memcpy(&slip_ctx->addr, &slip_config->ip6_addr, sizeof(esp_ip6_addr_t));


    return ESP_OK;
}

#if CONFIG_LWIP_IPV6
esp_err_t esp_netif_slip_set_ipv6(esp_netif_t *netif, const esp_ip6_addr_t *ipv6)
{
    lwip_slip_ctx_t *slip_ctx = (lwip_slip_ctx_t *)netif->related_data;
    assert(slip_ctx->base.netif_type == SLIP_LWIP_NETIF);

    ESP_LOGV(TAG, "%s (slip_ctx: %p)", __func__, slip_ctx);

    if (netif_is_link_up(netif->lwip_netif)) {
        ESP_LOGE(TAG, "Cannot set parameters while SLIP interface is running");
        return ESP_ERR_INVALID_STATE;
    }
    memcpy(&slip_ctx->addr, ipv6, sizeof(ip6_addr_t));
    int8_t addr_index = 0;

    netif_ip6_addr_set(netif->lwip_netif, addr_index, (ip6_addr_t *)&slip_ctx->addr);
    netif_ip6_addr_set_state(netif->lwip_netif, addr_index, IP6_ADDR_VALID);

    return ESP_OK;
}
#endif

/**
 * @brief Write incoming serial data to the SLIP interface
 */
void esp_netif_lwip_slip_input(void *h, void *buffer, unsigned int len, void *eb)
{
    esp_netif_t *netif = h;
    lwip_slip_ctx_t *slip_ctx = (lwip_slip_ctx_t *)netif->related_data;
    assert(slip_ctx->base.netif_type == SLIP_LWIP_NETIF);

    ESP_LOGD(TAG, "%s", __func__);
    ESP_LOG_BUFFER_HEXDUMP(TAG, buffer, len, ESP_LOG_DEBUG);

    // Update slip netif with data
    const int max_batch = 255;
    int sent = 0;
    while(sent < len) {
        int batch = (len - sent) > max_batch ? max_batch : (len - sent);
        slipif_received_bytes(netif->lwip_netif, buffer+sent, batch);
        sent += batch;
    }

    // Process incoming bytes
    for (int i = 0; i < len; i++) {
        slipif_process_rxqueue(netif->lwip_netif);
    }
}

/**
 * @brief Write raw data out the SLIP interface
 */
void esp_netif_lwip_slip_raw_output(esp_netif_t *slip_netif, void *buffer, size_t len)
{
    struct netif *lwip_netif = slip_netif->lwip_netif;

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

/**
 * @brief Destroys the SLIP context object
 */
void esp_netif_destroy_slip(netif_related_data_t *slip)
{
    ESP_LOGD(TAG, "%s", __func__);

    // Free base object
    free(slip);
}

const esp_ip6_addr_t *esp_slip_get_ip6(esp_netif_t *slip_netif)
{
    lwip_slip_ctx_t *slip_ctx = (lwip_slip_ctx_t *)slip_netif->related_data;
    assert(slip_ctx->base.netif_type == SLIP_LWIP_NETIF);
    return &slip_ctx->addr;
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

static const struct esp_netif_netstack_config s_netif_config_slip = {
        .lwip = {
                .init_fn = esp_slipif_init,
                .input_fn = esp_netif_lwip_slip_input,
        }
};

const esp_netif_netstack_config_t *_g_esp_netif_netstack_default_slip = &s_netif_config_slip;


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

#endif /* CONFIG_ESP_NETIF_TCPIP_LWIP */
