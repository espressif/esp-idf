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
#include "esp_wifi.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    QueueHandle_t handle; /**< FreeRTOS queue handler */
    void *storage;        /**< storage for FreeRTOS queue */
} wifi_static_queue_t;

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
 *    - ESP_ERR_NO_MEM: out of memory
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
  * @param  uint16_t len : the length of buffer
  *
  * @return
  *    - ERR_OK  : Successfully transmit the buffer to wifi driver
  *    - ERR_MEM : Out of memory
  *    - ERR_IF : WiFi driver error
  *    - ERR_ARG : Invalid argument
  */
int esp_wifi_internal_tx(wifi_interface_t wifi_if, void *buffer, uint16_t len);

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
  * @brief  enable or disable transmitting WiFi MAC frame with fixed rate
  *
  * @attention 1. If fixed rate is enabled, both management and data frame are transmitted with fixed rate
  * @attention 2. Make sure that the receiver is able to receive the frame with the fixed rate if you want the frame to be received
  *
  * @param  ifx : wifi interface
  * @param  en : false - disable, true - enable
  * @param  rate : PHY rate
  *
  * @return
  *    - ERR_OK  : succeed
  *    - ESP_ERR_WIFI_NOT_INIT: WiFi is not initialized by esp_wifi_init
  *    - ESP_ERR_WIFI_NOT_STARTED: WiFi was not started by esp_wifi_start
  *    - ESP_ERR_WIFI_IF : invalid WiFi interface
  *    - ESP_ERR_INVALID_ARG : invalid rate
  *    - ESP_ERR_NOT_SUPPORTED : do not support to set fixed rate if TX AMPDU is enabled
  */
esp_err_t esp_wifi_internal_set_fix_rate(wifi_interface_t ifx, bool en, wifi_phy_rate_t rate);

/**
  * @brief     Check the MD5 values of the OS adapter header files in IDF and WiFi library
  *
  * @attention 1. It is used for internal CI version check
  *
  * @return
  *     - ESP_OK : succeed
  *     - ESP_WIFI_INVALID_ARG : MD5 check fail
  */
esp_err_t esp_wifi_internal_osi_funcs_md5_check(const char *md5);

/**
  * @brief     Check the MD5 values of the crypto types header files in IDF and WiFi library
  *
  * @attention 1. It is used for internal CI version check
  *
  * @return
  *     - ESP_OK : succeed
  *     - ESP_WIFI_INVALID_ARG : MD5 check fail
  */
esp_err_t esp_wifi_internal_crypto_funcs_md5_check(const char *md5);

/**
  * @brief     Check the MD5 values of the esp_wifi_types.h in IDF and WiFi library
  *
  * @attention 1. It is used for internal CI version check
  *
  * @return
  *     - ESP_OK : succeed
  *     - ESP_WIFI_INVALID_ARG : MD5 check fail
  */
esp_err_t esp_wifi_internal_wifi_type_md5_check(const char *md5);

/**
  * @brief     Check the MD5 values of the esp_wifi.h in IDF and WiFi library
  *
  * @attention 1. It is used for internal CI version check
  *
  * @return
  *     - ESP_OK : succeed
  *     - ESP_WIFI_INVALID_ARG : MD5 check fail
  */
esp_err_t esp_wifi_internal_esp_wifi_md5_check(const char *md5);

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

/**
  * @brief     Update WiFi MAC time
  *
  * @param     uint32_t time_delta : time duration since the WiFi/BT common clock is disabled
  *
  * @return    Always returns ESP_OK
  */
typedef esp_err_t (* wifi_mac_time_update_cb_t)( uint32_t time_delta );

/**
  * @brief     Update WiFi MAC time
  *
  * @param     uint32_t time_delta : time duration since the WiFi/BT common clock is disabled
  *
  * @return    Always returns ESP_OK
  */
esp_err_t esp_wifi_internal_update_mac_time( uint32_t time_delta );

/**
 * @brief     A general API to set/get WiFi internal configuration, it's for debug only
 *
 * @param     cmd : ioctl command type
 * @param     cfg : configuration for the command
 *
 * @return
 *    - ESP_OK: succeed
 *    - others: failed
 */
esp_err_t esp_wifi_internal_ioctl(int cmd, wifi_ioctl_config_t *cfg);

#ifdef __cplusplus
}
#endif

#endif /* __ESP_WIFI_H__ */
