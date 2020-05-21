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


#include "lwip/dns.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_netif_net_stack.h"
#include "esp_event.h"
#include "esp_netif_slip.h"

#include "esp_netif_lwip_internal.h"

#include "lwip/opt.h"
#include "lwip/sio.h"
#include "lwip/ip.h"
#include "lwip/ip6.h"
#include "lwip/ip6_addr.h"
#include "lwip/netif.h"
#include "netif/slipif.h"

#include <string.h>


ESP_EVENT_DEFINE_BASE(SLIP_EVENT);

static const char *TAG = "esp-netif_lwip-slip";

/**
 * @brief LWIP SLIP context object
 */
typedef struct lwip_slip_ctx {
    //! ESP netif object
    esp_netif_t *esp_netif;

    //! SLIP interface IP6 address
    ip6_addr_t addr;


    //! UART device for underling SIO
    uart_port_t uart_dev;

} lwip_slip_ctx_t;

/**
 * @brief Create a new lwip slip interface
 */
lwip_slip_ctx_t *esp_netif_new_slip(esp_netif_t *esp_netif, const esp_netif_netstack_config_t *esp_netif_stack_config)
{
    ESP_LOGD(TAG, "%s", __func__);

    // Fetch netif and create context
    struct netif *netif_impl = esp_netif->lwip_netif;

    lwip_slip_ctx_t *slip_ctx = calloc(1, sizeof(lwip_slip_ctx_t));
    if (slip_ctx == NULL) {
        ESP_LOGE(TAG, "%s: cannot allocate lwip_slip_ctx_t", __func__);
        return NULL;
    }

    ESP_LOGD(TAG, "%s: Initialising SLIP (esp_netif %p, lwip_netif %p, device: %d)", __func__, esp_netif, netif_impl, slip_ctx->uart_dev);

    // Load config
    const esp_netif_slip_config_t *slip_config = &esp_netif_stack_config->lwip_slip.slip_config;

    // Attache network interface and uart device
    slip_ctx->uart_dev = slip_config->uart_dev;
    slip_ctx->esp_netif = esp_netif;

    ESP_LOGI(TAG, "%s: Created SLIP interface (netif %p, slip_ctx: %p)", __func__, esp_netif, slip_ctx);

    return slip_ctx;
}

/**
 * @brief Creates new SLIP related structure
 */
esp_err_t esp_netif_start_slip(lwip_slip_ctx_t *slip_ctx)
{
    ESP_LOGI(TAG, "%s: Starting SLIP connection (slip_ctx: %p, addr: %s)", __func__, slip_ctx, ip6addr_ntoa(&slip_ctx->addr));

    struct netif *netif_impl = slip_ctx->esp_netif->lwip_netif;

    // Store serial port number in net interface for SIO open command
    netif_impl->state = (void *)slip_ctx->uart_dev;

    // Attach interface
    netif_add_noaddr(slip_ctx->esp_netif->lwip_netif, (void *)slip_ctx->uart_dev, &slipif_init, &ip_input);

    // Bind address
    int8_t addr_index = 0;

    // Note that addr_set is used here as the address is statically configured
    // rather than negotiated over the link
    netif_ip6_addr_set(slip_ctx->esp_netif->lwip_netif, addr_index, &slip_ctx->addr);
    netif_ip6_addr_set_state(slip_ctx->esp_netif->lwip_netif, addr_index, IP6_ADDR_VALID);

    // Setup interface
    netif_set_link_up(slip_ctx->esp_netif->lwip_netif);
    netif_set_up(slip_ctx->esp_netif->lwip_netif);

    return ESP_OK;
}

/**
 * @brief Stops the SLIP interface
 */
esp_err_t esp_netif_stop_slip(lwip_slip_ctx_t *slip_ctx)
{
    ESP_LOGI(TAG, "%s: Stopped SLIP connection: %p", __func__, slip_ctx);

    // Stop interface
    netif_set_link_down(slip_ctx->esp_netif->lwip_netif);

    return ESP_OK;
}


/**
 * @brief Sets paramaters for the supplied netif
 */
esp_err_t esp_netif_slip_set_params(esp_netif_t *netif, const esp_netif_slip_config_t *slip_config)
{

    lwip_slip_ctx_t *slip_ctx = netif->lwip_slip_ctx;

    ESP_LOGD(TAG, "%s (slip_ctx: %p)", __func__, slip_ctx);

    if (netif_is_link_up(slip_ctx->esp_netif->lwip_netif)) {
        ESP_LOGE(TAG, "Cannot set parameters while SLIP interface is running");
        return ESP_ERR_INVALID_STATE;
    }

    memcpy(&slip_ctx->addr, &slip_config->addr, sizeof(ip6_addr_t));
    slip_ctx->uart_dev = slip_config->uart_dev;

    return ESP_OK;
}

/**
 * @brief Write incoming serial data to the SLIP interface
 */
void esp_netif_lwip_slip_input(void *ctx, void *buffer, size_t len, void *eb)
{
    lwip_slip_ctx_t *slip_ctx = (lwip_slip_ctx_t *)ctx;

    ESP_LOGD(TAG, "%s", __func__);
    ESP_LOG_BUFFER_HEXDUMP(TAG, buffer, len, ESP_LOG_DEBUG);

    // Update slip netif with data
    slipif_received_bytes(slip_ctx->esp_netif->lwip_netif, buffer, len);

    // Process incoming bytes
    for (int i = 0; i < len; i++) {
        slipif_process_rxqueue(slip_ctx->esp_netif->lwip_netif);
    }
}

/**
 * @brief Write raw data out the SLIP interface
 */
void esp_netif_lwip_slip_output(lwip_slip_ctx_t *slip_ctx, void *buffer, size_t len)
{
    struct netif *lwip_netif = slip_ctx->esp_netif->lwip_netif;

    ESP_LOGD(TAG, "%s", __func__);
    ESP_LOG_BUFFER_HEXDUMP(TAG, buffer, len, ESP_LOG_DEBUG);

    struct pbuf p = {
        .next = NULL,
        .payload = buffer,
        .tot_len = len,
        .len = len,
    };

    // Call slip if output function to feed data out slip interface
    lwip_netif->output_ip6(lwip_netif, &p, NULL);
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
    lwip_netif->output_ip6(lwip_netif, &p, NULL);
}

/**
 * @brief Fetch pointer to internal slip_lwip_context
 *
 * This is required to support the wiring of esp_netif objects outside
 * of this component.
 *
 * @return
 *        - lwip slip context
 */
lwip_slip_ctx_t *esp_netif_lwip_slip_get_ctx(esp_netif_t *slip_netif)
{
    return slip_netif->lwip_slip_ctx;
}

/**
 * @brief Destroys the SLIP context object
 */
void esp_netif_destroy_slip(lwip_slip_ctx_t *slip_ctx)
{
    ESP_LOGD(TAG, "%s", __func__);

    // Free base object
    free(slip_ctx);
}

void esp_netif_slip_set_default_netif(lwip_slip_ctx_t *slip_ctx)
{
    netif_set_default(slip_ctx->esp_netif->lwip_netif);
}

const ip6_addr_t *esp_slip_get_ip6(esp_netif_t *slip_netif)
{
    return &slip_netif->lwip_slip_ctx->addr;
}

