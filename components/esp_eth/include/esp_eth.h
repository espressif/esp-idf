// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_eth_com.h"
#include "esp_eth_mac.h"
#include "esp_eth_phy.h"

/**
* @brief Handle of Ethernet driver
*
*/
typedef void *esp_eth_handle_t;

/**
* @brief Configuration of Ethernet driver
*
*/
typedef struct {
    /**
    * @brief Ethernet MAC object
    *
    */
    esp_eth_mac_t *mac;

    /**
    * @brief Ethernet PHY object
    *
    */
    esp_eth_phy_t *phy;

    /**
    * @brief Period time of checking Ethernet link status
    *
    */
    uint32_t check_link_period_ms;

    /**
    * @brief Input frame buffer to user's stack
    *
    * @param[in] eth_handle: handle of Ethernet driver
    * @param[in] buffer: frame buffer that will get input to upper stack
    * @param[in] length: length of the frame buffer
    *
    * @return
    *      - ESP_OK: input frame buffer to upper stack successfully
    *      - ESP_FAIL: error occurred when inputting buffer to upper stack
    *
    */
    esp_err_t (*stack_input)(esp_eth_handle_t eth_handle, uint8_t *buffer, uint32_t length);

    /**
    * @brief Callback function invoked when lowlevel initialization is finished
    *
    * @param[in] eth_handle: handle of Ethernet driver
    *
    * @return
    *       - ESP_OK: process extra lowlevel initialization successfully
    *       - ESP_FAIL: error occurred when processing extra lowlevel initialization
    */
    esp_err_t (*on_lowlevel_init_done)(esp_eth_handle_t eth_handle);

    /**
    * @brief Callback function invoked when lowlevel deinitialization is finished
    *
    * @param[in] eth_handle: handle of Ethernet driver
    *
    * @return
    *       - ESP_OK: process extra lowlevel deinitialization successfully
    *       - ESP_FAIL: error occurred when processing extra lowlevel deinitialization
    */
    esp_err_t (*on_lowlevel_deinit_done)(esp_eth_handle_t eth_handle);

} esp_eth_config_t;

/**
 * @brief Default configuration for Ethernet driver
 *
 */
#define ETH_DEFAULT_CONFIG(emac, ephy)   \
    {                                    \
        .mac = emac,                     \
        .phy = ephy,                     \
        .check_link_period_ms = 2000,    \
        .stack_input = NULL,             \
        .on_lowlevel_init_done = NULL,   \
        .on_lowlevel_deinit_done = NULL, \
    }

/**
* @brief Install Ethernet driver
*
* @param[in]  config: configuration of the Ethernet driver
* @param[out] out_hdl: handle of Ethernet driver
*
* @return
*       - ESP_OK: install esp_eth driver successfully
*       - ESP_ERR_INVALID_ARG: install esp_eth driver failed because of some invalid argument
*       - ESP_ERR_NO_MEM: install esp_eth driver failed because there's no memory for driver
*       - ESP_FAIL: install esp_eth driver failed because some other error occurred
*/
esp_err_t esp_eth_driver_install(const esp_eth_config_t *config, esp_eth_handle_t *out_hdl);

/**
* @brief Start ethernet driver **ONLY** in standalone mode, i.e. without TCP/IP stack
*
* Note that ethernet driver is typically started as soon as it is attached to esp-netif.
* This API should only be called if ethernet is used separately without esp-netif, for example
* when esp_eth_config_t.stack_input is not NULL.
*
* @param[in] eth_handle handle of Ethernet driver
*
* @return
*       - ESP_OK: starts ethernet driver
*       - ESP_ERR_INVALID_STATE: if event loop hasn't been initialized
*/
esp_err_t esp_eth_driver_start(esp_eth_handle_t eth_handle);

/**
* @brief Uninstall Ethernet driver
*
* @param[in] hdl: handle of Ethernet driver
*
* @return
*       - ESP_OK: uninstall esp_eth driver successfully
*       - ESP_ERR_INVALID_ARG: uninstall esp_eth driver failed because of some invalid argument
*       - ESP_FAIL: uninstall esp_eth driver failed because some other error occurred
*/
esp_err_t esp_eth_driver_uninstall(esp_eth_handle_t hdl);

/**
* @brief General Transmit
*
* @param[in] hdl: handle of Ethernet driver
* @param[in] buf: buffer of the packet to transfer
* @param[in] length: length of the buffer to transfer
*
* @return
*       - ESP_OK: transmit frame buffer successfully
*       - ESP_ERR_INVALID_ARG: transmit frame buffer failed because of some invalid argument
*       - ESP_FAIL: transmit frame buffer failed because some other error occurred
*/
esp_err_t esp_eth_transmit(void* hdl, void *buf, uint32_t length);

/**
* @brief General Receive
*
* @param[in] hdl: handle of Ethernet driver
* @param[out] buf: buffer to preserve the received packet
* @param[out] length: length of the received packet
*
* @note Before this function got invoked, the value of "length" should set by user, equals the size of buffer.
*       After the function returned, the value of "length" means the real length of received data.
*
* @return
*       - ESP_OK: receive frame buffer successfully
*       - ESP_ERR_INVALID_ARG: receive frame buffer failed because of some invalid argument
*       - ESP_ERR_INVALID_SIZE: input buffer size is not enough to hold the incoming data.
*                               in this case, value of returned "length" indicates the real size of incoming data.
*       - ESP_FAIL: receive frame buffer failed because some other error occurred
*/
esp_err_t esp_eth_receive(esp_eth_handle_t hdl, uint8_t *buf, uint32_t *length);

/**
* @brief Misc IO function of Etherent driver
*
* @param[in] hdl: handle of Ethernet driver
* @param[in] cmd: IO control command
* @param[in] data: specificed data for command
*
* @return
*       - ESP_OK: process io command successfully
*       - ESP_ERR_INVALID_ARG: process io command failed because of some invalid argument
*       - ESP_FAIL: process io command failed because some other error occurred
*/
esp_err_t esp_eth_ioctl(esp_eth_handle_t hdl, esp_eth_io_cmd_t cmd, void *data);

/**
 * @brief Register default ethernet handlers
 *
 * @param[in] esp_netif esp network interface handle created for this driver
 * (note: appropriate ethernet handle not yet properly initialized when setting up
 * default handlers)
 */
esp_err_t esp_eth_set_default_handlers(void* esp_netif);

/**
 * @brief Unregister default ethernet handlers
 *
 * @param[in] esp_netif esp network interface handle created for this driver
 */
esp_err_t esp_eth_clear_default_handlers(void* esp_netif);

#ifdef __cplusplus
}
#endif
