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

#ifdef __cplusplus
extern "C" {
#endif

//
// Network stack API: This ESP-NETIF API are supposed to be called only from internals of TCP/IP stack
//

/** @addtogroup ESP_NETIF_CONVERT
 * @{
 */

/**
 * @brief Returns esp-netif handle
 *
 * @param[in] dev opaque ptr to network interface of specific TCP/IP stack
 *
 * @return    handle to related esp-netif instance
 */
esp_netif_t* esp_netif_get_handle_from_netif_impl(void *dev);

/**
 * @brief Returns network stack specific implementation handle (if supported)
 *
 * Note that it is not supported to acquire PPP netif impl pointer and
 * this function will return NULL for esp_netif instances configured to PPP mode
 *
 * @param[in]  esp_netif Handle to esp-netif instance
 *
 * @return    handle to related network stack netif handle
 */
void* esp_netif_get_netif_impl(esp_netif_t *esp_netif);

/**
 * @}
 */

/** @addtogroup ESP_NETIF_DATA_IO_API
 * @{
 */

/**
  * @brief  Outputs packets from the TCP/IP stack to the media to be transmitted
  *
  * This function gets called from network stack to output packets to IO driver.
  *
  * @param[in]  esp_netif Handle to esp-netif instance
  * @param[in]  data Data to be transmitted
  * @param[in]  len Length of the data frame
  *
  * @return   ESP_OK on success, an error passed from the I/O driver otherwise
  */
esp_err_t esp_netif_transmit(esp_netif_t *esp_netif, void* data, size_t len);

/**
  * @brief  Outputs packets from the TCP/IP stack to the media to be transmitted
  *
  * This function gets called from network stack to output packets to IO driver.
  *
  * @param[in]  esp_netif Handle to esp-netif instance
  * @param[in]  data Data to be transmitted
  * @param[in]  len Length of the data frame
  * @param[in]  netstack_buf net stack buffer
  *
  * @return   ESP_OK on success, an error passed from the I/O driver otherwise
  */
esp_err_t esp_netif_transmit_wrap(esp_netif_t *esp_netif, void *data, size_t len, void *netstack_buf);

/**
  * @brief  Free the rx buffer allocated by the media driver
  *
  * This function gets called from network stack when the rx buffer to be freed in IO driver context,
  * i.e. to deallocate a buffer owned by io driver (when data packets were passed to higher levels
  * to avoid copying)
  *
  * @param[in]  esp_netif Handle to esp-netif instance
  * @param[in]  buffer Rx buffer pointer
  */
void esp_netif_free_rx_buffer(void *esp_netif, void* buffer);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif //_ESP_NETIF_NET_STACK_H_
