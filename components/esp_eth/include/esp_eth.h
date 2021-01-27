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

#include "esp_eth_com.h"
#include "esp_eth_mac.h"
#include "esp_eth_phy.h"

#ifdef __cplusplus
extern "C" {
#endif

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
    esp_err_t (*stack_input)(esp_eth_handle_t eth_handle, uint8_t *buffer, uint32_t length, void *priv);

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
* @brief Uninstall Ethernet driver
* @note It's not recommended to uninstall Ethernet driver unless it won't get used any more in application code.
*       To uninstall Ethernet driver, you have to make sure, all references to the driver are released.
*       Ethernet driver can only be uninstalled successfully when reference counter equals to one.
*
* @param[in] hdl: handle of Ethernet driver
*
* @return
*       - ESP_OK: uninstall esp_eth driver successfully
*       - ESP_ERR_INVALID_ARG: uninstall esp_eth driver failed because of some invalid argument
*       - ESP_ERR_INVALID_STATE: uninstall esp_eth driver failed because it has more than one reference
*       - ESP_FAIL: uninstall esp_eth driver failed because some other error occurred
*/
esp_err_t esp_eth_driver_uninstall(esp_eth_handle_t hdl);

/**
* @brief Start Ethernet driver **ONLY** in standalone mode (i.e. without TCP/IP stack)
*
* @note This API will start driver state machine and internal software timer (for checking link status).
*
* @param[in] hdl handle of Ethernet driver
*
* @return
*       - ESP_OK: start esp_eth driver successfully
*       - ESP_ERR_INVALID_ARG: start esp_eth driver failed because of some invalid argument
*       - ESP_ERR_INVALID_STATE: start esp_eth driver failed because driver has started already
*       - ESP_FAIL: start esp_eth driver failed because some other error occurred
*/
esp_err_t esp_eth_start(esp_eth_handle_t hdl);

/**
* @brief Stop Ethernet driver
*
* @note This function does the oppsite operation of `esp_eth_start`.
*
* @param[in] hdl handle of Ethernet driver
* @return
*       - ESP_OK: stop esp_eth driver successfully
*       - ESP_ERR_INVALID_ARG: stop esp_eth driver failed because of some invalid argument
*       - ESP_ERR_INVALID_STATE: stop esp_eth driver failed because driver has not started yet
*       - ESP_FAIL: stop esp_eth driver failed because some other error occurred
*/
esp_err_t esp_eth_stop(esp_eth_handle_t hdl);

/**
* @brief Update Ethernet data input path (i.e. specify where to pass the input buffer)
*
* @note After install driver, Ethernet still don't know where to deliver the input buffer.
*       In fact, this API registers a callback function which get invoked when Ethernet received new packets.
*
* @param[in] hdl handle of Ethernet driver
* @param[in] stack_input function pointer, which does the actual process on incoming packets
* @param[in] priv private resource, which gets passed to `stack_input` callback without any modification
* @return
*       - ESP_OK: update input path successfully
*       - ESP_ERR_INVALID_ARG: update input path failed because of some invalid argument
*       - ESP_FAIL: update input path failed because some other error occurred
*/
esp_err_t esp_eth_update_input_path(
    esp_eth_handle_t hdl,
    esp_err_t (*stack_input)(esp_eth_handle_t hdl, uint8_t *buffer, uint32_t length, void *priv),
    void *priv);

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
esp_err_t esp_eth_transmit(esp_eth_handle_t hdl, void *buf, size_t length);

/**
* @brief General Receive is deprecated and shall not be accessed from app code,
*        as polling is not supported by Ethernet.
*
* @param[in] hdl: handle of Ethernet driver
* @param[out] buf: buffer to preserve the received packet
* @param[out] length: length of the received packet
*
* @note Before this function got invoked, the value of "length" should set by user, equals the size of buffer.
*       After the function returned, the value of "length" means the real length of received data.
* @note This API was exposed by accident, users should not use this API in their applications.
*       Ethernet driver is interrupt driven, and doesn't support polling mode.
*       Instead, users should register input callback with ``esp_eth_update_input_path``.
*
* @return
*       - ESP_OK: receive frame buffer successfully
*       - ESP_ERR_INVALID_ARG: receive frame buffer failed because of some invalid argument
*       - ESP_ERR_INVALID_SIZE: input buffer size is not enough to hold the incoming data.
*                               in this case, value of returned "length" indicates the real size of incoming data.
*       - ESP_FAIL: receive frame buffer failed because some other error occurred
*/
esp_err_t esp_eth_receive(esp_eth_handle_t hdl, uint8_t *buf, uint32_t *length) __attribute__((deprecated("Ethernet driver is interrupt driven only, please register input callback with esp_eth_update_input_path")));

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
* @brief Increase Ethernet driver reference
* @note Ethernet driver handle can be obtained by os timer, netif, etc.
*       It's dangerous when thread A is using Ethernet but thread B uninstall the driver.
*       Using reference counter can prevent such risk, but care should be taken, when you obtain Ethernet driver,
*       this API must be invoked so that the driver won't be uninstalled during your using time.
*
*
* @param[in] hdl: handle of Ethernet driver
* @return
*       - ESP_OK: increase reference successfully
*       - ESP_ERR_INVALID_ARG: increase reference failed because of some invalid argument
*/
esp_err_t esp_eth_increase_reference(esp_eth_handle_t hdl);

/**
* @brief Decrease Ethernet driver reference
*
* @param[in] hdl: handle of Ethernet driver
* @return
*       - ESP_OK: increase reference successfully
*       - ESP_ERR_INVALID_ARG: increase reference failed because of some invalid argument
*/
esp_err_t esp_eth_decrease_reference(esp_eth_handle_t hdl);

#ifdef __cplusplus
}
#endif
