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
//

#ifndef _ESP_NETIF_SLIP_H_
#define _ESP_NETIF_SLIP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "driver/uart.h"

/** @brief SLIP event base */
ESP_EVENT_DECLARE_BASE(SLIP_EVENT);

typedef enum esp_netif_slip_event {
    SLIP_EVENT_START = 1,
    SLIP_EVENT_STOP = 2,

} esp_netif_slip_event_e;

/** @brief Forward declaration of lwip_slip_ctx for external use */
typedef struct lwip_slip_ctx lwip_slip_ctx_t;

/** @brief Configuration structure for SLIP network interface
 *
 */
typedef struct esp_netif_slip_config {
    ip6_addr_t addr;        /* Local IP6 address */

    uart_port_t uart_dev;   /* UART device for reading and writing SLIP information, this must be initialised externally */

} esp_netif_slip_config_t;


/** @brief Sets common parameters for the supplied esp-netif.
 *
 * @param[in]  esp_netif handle to slip esp-netif instance
 * @param[in]  config Pointer to SLIP netif configuration structure
 *
 * @return     ESP_OK on success, ESP_ERR_ESP_NETIF_INVALID_PARAMS if netif null or not SLIP
 */
esp_err_t esp_netif_slip_set_params(esp_netif_t *netif, const esp_netif_slip_config_t *config);

/**
 * @brief  Data path API to input incoming packets to SLIP
 *
 * @param[in]    esp_netif handle to slip esp-netif instance
 * @param[in]    buffer pointer to the incoming data
 * @param[in]    len length of the data
 *
 * @return
 *         - ESP_OK on success
 */
void esp_netif_lwip_slip_raw_input(esp_netif_t *netif, void *buffer, size_t len);

/**
 * @brief  Data path API to write raw packet ous the SLIP interface
 *
 * @param[in]    esp_netif handle to slip esp-netif instance
 * @param[in]    buffer pointer to the outgoing data
 * @param[in]    len length of the data
 *
 * @return
 *         - ESP_OK on success
 */
void esp_netif_lwip_slip_raw_output(esp_netif_t *netif, void *buffer, size_t len);

/**
 * @brief  Fetch IP6 address attached to the SLIP interface
 *
 * @param[in]    esp_netif handle to slip esp-netif instance
 * @param[in]    address index (unused)
 *
 * @return
 *         - pointer to the internal ip6 address object
 */
const ip6_addr_t *esp_slip_get_ip6(esp_netif_t *slip_netif);

/**
 * @brief  Fetch lwip_slip_ctx_t for esp_netif_t object
 *
 * This is required to support the wiring of esp_netif objects outside
 * of this component.
 *
 * @return
 *         - lwip slip context
 */
lwip_slip_ctx_t *esp_netif_lwip_slip_get_ctx(esp_netif_t *slip_netif);

/**
 * @brief Start the esp slip netif
 *
 * @param[in]    esp_netif handle to slip esp-netif instance
 *
 * @return
 *          - ESP_OK on success
 */
esp_err_t esp_netif_start_slip(lwip_slip_ctx_t *slip_ctx);

/**
 * @brief Stop the esp slip netif
 *
 * @param[in]    esp_netif handle to slip esp-netif instance
 *
 * @return
 *         - ESP_OK on success
 */
esp_err_t esp_netif_stop_slip(lwip_slip_ctx_t *slip_ctx);

#endif
