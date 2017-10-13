// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

/*
 * All the APIs declared here are internal only APIs, it can only be used by 
 * espressif internal modules, such as SSC, LWIP, TCPIP adapter etc, espressif 
 * customers are not recommended to use them.
 *
 * If someone really want to use specified APIs declared in here, please contact
 * espressif AE/developer to make sure you know the limitations or risk of 
 * the API, otherwise you may get unexpected behavior!!!
 *
 */


#ifndef __ESP_WIFI_INTERNAL_H__
#define __ESP_WIFI_INTERNAL_H__

#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "rom/queue.h"
#include "esp_err.h"
#include "esp_wifi_types.h"
#include "esp_event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize Wi-Fi Driver
 *     Alloc resource for WiFi driver, such as WiFi control structure, RX/TX buffer,
 *     WiFi NVS structure among others.
 *
 * For the most part, you need not call this function directly. It gets called
 * from esp_wifi_init().
 *
 * This function may be called, if you only need to initialize the Wi-Fi driver
 * without having to use the network stack on top.
 *
 * @param  config provide WiFi init configuration
 *
 * @return
 *    - ESP_OK: succeed
 *    - ESP_ERR_WIFI_NO_MEM: out of memory
 *    - others: refer to error code esp_err.h
 */
esp_err_t esp_wifi_init_internal(const wifi_init_config_t *config);

/**
  * @brief  get whether the wifi driver is allowed to transmit data or not
  *
  * @return
  *     - true  : upper layer should stop to transmit data to wifi driver
  *     - false : upper layer can transmit data to wifi driver
  */
bool esp_wifi_internal_tx_is_stop(void);

/**
  * @brief  free the rx buffer which allocated by wifi driver
  *
  * @param  void* buffer: rx buffer pointer
  */
void esp_wifi_internal_free_rx_buffer(void* buffer);

/**
  * @brief  transmit the buffer via wifi driver
  *
  * @param  wifi_interface_t wifi_if : wifi interface id
  * @param  void *buffer : the buffer to be tansmit
  * @param  u16_t len : the length of buffer
  *
  * @return
  *    - ERR_OK  : Successfully transmit the buffer to wifi driver
  *    - ERR_MEM : Out of memory
  *    - ERR_IF : WiFi driver error
  *    - ERR_ARG : Invalid argument
  */
int esp_wifi_internal_tx(wifi_interface_t wifi_if, void *buffer, u16_t len);

/**
  * @brief     The WiFi RX callback function
  *
  *            Each time the WiFi need to forward the packets to high layer, the callback function will be called
  */
typedef esp_err_t (*wifi_rxcb_t)(void *buffer, uint16_t len, void *eb);

/**
  * @brief     Set the WiFi RX callback
  *
  * @attention 1. Currently we support only one RX callback for each interface
  *
  * @param     wifi_interface_t ifx : interface
  * @param     wifi_rxcb_t fn : WiFi RX callback
  *
  * @return
  *     - ESP_OK : succeed
  *     - others : fail
  */
esp_err_t esp_wifi_internal_reg_rxcb(wifi_interface_t ifx, wifi_rxcb_t fn);

/**
  * @brief     Notify WIFI driver that the station got ip successfully
  *
  * @return
  *     - ESP_OK : succeed
  *     - others : fail
  */
esp_err_t esp_wifi_internal_set_sta_ip(void);

/**
  * @brief     Allocate a chunk of memory for WiFi driver
  *
  * @attention This API is not used for DMA memory allocation.
  *
  * @param     size_t size : Size, in bytes, of the amount of memory to allocate
  *
  * @return    A pointer to the memory allocated on success, NULL on failure
  */
void *wifi_malloc( size_t size );

/**
  * @brief     Reallocate a chunk of memory for WiFi driver
  *
  * @attention This API is not used for DMA memory allocation.
  *
  * @param     void * ptr  : Pointer to previously allocated memory, or NULL for a new allocation.
  * @param     size_t size : Size, in bytes, of the amount of memory to allocate
  *
  * @return    A pointer to the memory allocated on success, NULL on failure
  */
void *wifi_realloc( void *ptr, size_t size );

/**
  * @brief     Callocate memory for WiFi driver
  *
  * @attention This API is not used for DMA memory allocation.
  *
  * @param     size_t n    : Number of continuing chunks of memory to allocate
  * @param     size_t size : Size, in bytes, of the amount of memory to allocate
  *
  * @return    A pointer to the memory allocated on success, NULL on failure
  */
void *wifi_calloc( size_t n, size_t size );

#ifdef __cplusplus
}
#endif

#endif /* __ESP_WIFI_H__ */
