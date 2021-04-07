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

#include "esp_netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Configuration structure for SLIP network interface
 *
 */
typedef struct esp_netif_slip_config {
    esp_ip6_addr_t ip6_addr;        /* Local IP6 address */

} esp_netif_slip_config_t;


/** @brief Sets common parameters for the supplied esp-netif.
 *
 * @param[in]  esp_netif handle to slip esp-netif instance
 * @param[in]  config Pointer to SLIP netif configuration structure
 *
 * @return     ESP_OK on success, ESP_ERR_ESP_NETIF_INVALID_PARAMS if netif null or not SLIP
 */
esp_err_t esp_netif_slip_set_params(esp_netif_t *netif, const esp_netif_slip_config_t *config);

#if CONFIG_LWIP_IPV6
/** @brief Sets IPV6 address for the supplied esp-netif.
 *
 * @param[in]  netif handle to slip esp-netif instance
 * @param[in]  ipv6 IPv6 address of the SLIP interface
 *
 * @return     ESP_OK on success, ESP_ERR_ESP_NETIF_INVALID_PARAMS if netif null or not SLIP
 */
esp_err_t esp_netif_slip_set_ipv6(esp_netif_t *netif, const esp_ip6_addr_t *ipv6);
#endif

/**
 * @brief  Data path API to write raw packet ous the SLIP interface
 *
 * This API is typically used when implementing user defined methods
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
const esp_ip6_addr_t *esp_slip_get_ip6(esp_netif_t *slip_netif);

#endif
