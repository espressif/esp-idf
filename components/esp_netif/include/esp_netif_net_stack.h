// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _ESP_NETIF_NET_STACK_H_
#define _ESP_NETIF_NET_STACK_H_

//
// Network stack API: This ESP-NETIF API are supposed to be called only from internals of TCP/IP stack
//

/**
 * @brief Returns esp-netif handle
 *
 * @param[in] dev opaque ptr to network interface of specific TCP/IP stack
 *
 * @return    handle to related esp-netif instance
 */
esp_netif_t* esp_netif_get_handle_from_netif_impl(void *dev);


/**
 * @brief Returns network stack specific implementation handle
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 *
 * @return    handle to related network stack netif handle
 */
void* esp_netif_get_netif_impl(esp_netif_t *esp_netif);

#endif //_ESP_NETIF_NET_STACK_H_
