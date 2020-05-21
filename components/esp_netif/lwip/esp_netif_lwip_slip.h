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

#ifndef _ESP_NETIF_LWIP_SLIP_H_
#define _ESP_NETIF_LWIP_SLIP_H_

typedef struct lwip_slip_ctx lwip_slip_ctx_t;

/**
 * @brief  Creates new SLIP related structure
 *
 * @param[in]     esp_netif pointer esp-netif instance
 * @param[in]     stack_config TCP/IP stack configuration structure
 *
 * @return
 *         - pointer to slip-netif object on success
 *         - NULL otherwise
 */
lwip_slip_ctx_t *esp_netif_new_slip(esp_netif_t *esp_netif, const esp_netif_netstack_config_t *esp_netif_stack_config);

/**
 * @brief  Creates new SLIP related structure
 *
 * @param[in]    slip pointer to internal slip context instance
 *
 * @return
 *         - ESP_OK on success
 */
esp_err_t esp_netif_start_slip(lwip_slip_ctx_t *slip);



/**
 * @brief   Destroys the slip netif object
 *
 * @param[in]    slip pointer to internal slip context instance
 */
void esp_netif_destroy_slip(lwip_slip_ctx_t *slip);

/**
 * @brief  Stops the SLIP interface
 *
 * @param[in]    slip pointer to internal slip context instance
 *
 * @return
 *         - ESP_OK on success
 */
esp_err_t esp_netif_stop_slip(lwip_slip_ctx_t *slip);

/**
 * @brief  Sets default netif for routing priority config
 *
 * @note: This function must be called from lwip thread
 *
 */
void esp_netif_slip_set_default_netif(lwip_slip_ctx_t *slip_ctx);



#endif // _ESP_NETIF_LWIP_SLIP_H_