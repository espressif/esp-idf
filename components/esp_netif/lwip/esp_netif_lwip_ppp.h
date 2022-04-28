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

#ifndef _ESP_NETIF_LWIP_PPP_H_
#define _ESP_NETIF_LWIP_PPP_H_

#if CONFIG_ESP_NETIF_TCPIP_LWIP

/**
 * @brief  Creates new PPP related structure
 *
 * @param[in]     esp_netif pointer esp-netif instance
 * @param[in]     stack_config TCP/IP stack configuration structure
 *
 * @return
 *         - pointer to ppp-netif object on success
 *         - NULL otherwise
 */
netif_related_data_t * esp_netif_new_ppp(esp_netif_t *esp_netif, const esp_netif_netstack_config_t *esp_netif_stack_config);

/**
 * @brief  Creates new PPP related structure
 *
 * @param[in]     esp_netif pointer esp-netif instance
 *
 * @return
 *         - ESP_OK on success
 */
esp_err_t esp_netif_start_ppp(esp_netif_t *esp_netif);

/**
 * @brief  Data path API to input incoming packets to PPP
 *
 * @param[in]    ppp pointer to internal ppp context instance
 * @param[in]    buffer pointer to the incoming data
 * @param[in]    len length of the data
 * @param[in]    eb external buffer ptr not used here (to be inline with input function prototypes)
 *
 * @return
 *         - ESP_OK on success
 */
void esp_netif_lwip_ppp_input(void *ppp, void *buffer, size_t len, void *eb);

/**
 * @brief   Destroys the ppp netif object
 *
 * @param[in]    netif_related pointer to internal ppp context instance
 */
void esp_netif_destroy_ppp(netif_related_data_t *netif_related);

/**
 * @brief  Stops the PPP interface
 *
 * @param[in]    netif_related pointer to internal ppp context instance
 *
 * @return
 *         - ESP_OK on success
 */
esp_err_t esp_netif_stop_ppp(netif_related_data_t *netif_related);

/**
 * @brief  Sets default netif for routing priority config
 *
 * @note: This function must be called from lwip thread
 *
 */
void esp_netif_ppp_set_default_netif(netif_related_data_t *netif_related);

#endif /* CONFIG_ESP_NETIF_TCPIP_LWIP */


#endif // _ESP_NETIF_LWIP_PPP_H_
