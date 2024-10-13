/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_eth_com.h"
#if CONFIG_ETH_USE_SPI_ETHERNET
#include "esp_eth_mac_spi.h"
#endif // CONFIG_ETH_USE_SPI_ETHERNET
#if CONFIG_ETH_USE_ESP32_EMAC
#include "esp_eth_mac_esp.h"
#endif // CONFIG_ETH_USE_ESP32_EMAC
#if  CONFIG_ETH_USE_OPENETH
#include "esp_eth_mac_openeth.h"
#endif // CONFIG_ETH_USE_OPENETH
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

    /**
    * @brief Read PHY register
    *
    * @note Usually the PHY register read/write function is provided by MAC (SMI interface),
    *       but if the PHY device is managed by other interface (e.g. I2C), then user needs to
    *       implement the corresponding read/write.
    *       Setting this to NULL means your PHY device is managed by MAC's SMI interface.
    *
    * @param[in] eth_handle: handle of Ethernet driver
    * @param[in] phy_addr: PHY chip address (0~31)
    * @param[in] phy_reg: PHY register index code
    * @param[out] reg_value: PHY register value
    *
    * @return
    *      - ESP_OK: read PHY register successfully
    *      - ESP_ERR_INVALID_ARG: read PHY register failed because of invalid argument
    *      - ESP_ERR_TIMEOUT: read PHY register failed because of timeout
    *      - ESP_FAIL: read PHY register failed because some other error occurred
    */
    esp_err_t (*read_phy_reg)(esp_eth_handle_t eth_handle, uint32_t phy_addr, uint32_t phy_reg, uint32_t *reg_value);

    /**
    * @brief Write PHY register
    *
    * @note Usually the PHY register read/write function is provided by MAC (SMI interface),
    *       but if the PHY device is managed by other interface (e.g. I2C), then user needs to
    *       implement the corresponding read/write.
    *       Setting this to NULL means your PHY device is managed by MAC's SMI interface.
    *
    * @param[in] eth_handle: handle of Ethernet driver
    * @param[in] phy_addr: PHY chip address (0~31)
    * @param[in] phy_reg: PHY register index code
    * @param[in] reg_value: PHY register value
    *
    * @return
    *      - ESP_OK: write PHY register successfully
    *      - ESP_ERR_INVALID_ARG: read PHY register failed because of invalid argument
    *      - ESP_ERR_TIMEOUT: write PHY register failed because of timeout
    *      - ESP_FAIL: write PHY register failed because some other error occurred
    */
    esp_err_t (*write_phy_reg)(esp_eth_handle_t eth_handle, uint32_t phy_addr, uint32_t phy_reg, uint32_t reg_value);
} esp_eth_config_t;

/**
 * @brief Data structure to Read/Write PHY register via ioctl API
 *
 */
typedef struct {
    uint32_t reg_addr;              /*!< PHY register address */
    uint32_t *reg_value_p;          /*!< Pointer to a memory where the register value is read/written */
} esp_eth_phy_reg_rw_data_t;

/**
* @brief Command list for ioctl API
*
*/
typedef enum {
    ETH_CMD_G_MAC_ADDR,               /*!< Get MAC address */
    ETH_CMD_S_MAC_ADDR,               /*!< Set MAC address */
    ETH_CMD_G_PHY_ADDR,               /*!< Get PHY address */
    ETH_CMD_S_PHY_ADDR,               /*!< Set PHY address */
    ETH_CMD_G_AUTONEGO,               /*!< Get PHY Auto Negotiation */
    ETH_CMD_S_AUTONEGO,               /*!< Set PHY Auto Negotiation */
    ETH_CMD_G_SPEED,                  /*!< Get Speed */
    ETH_CMD_S_SPEED,                  /*!< Set Speed */
    ETH_CMD_S_PROMISCUOUS,            /*!< Set promiscuous mode */
    ETH_CMD_S_FLOW_CTRL,              /*!< Set flow control */
    ETH_CMD_G_DUPLEX_MODE,            /*!< Get Duplex mode */
    ETH_CMD_S_DUPLEX_MODE,            /*!< Set Duplex mode */
    ETH_CMD_S_PHY_LOOPBACK,           /*!< Set PHY loopback */
    ETH_CMD_READ_PHY_REG,             /*!< Read PHY register */
    ETH_CMD_WRITE_PHY_REG,            /*!< Write PHY register */

    ETH_CMD_CUSTOM_MAC_CMDS = ETH_CMD_CUSTOM_MAC_CMDS_OFFSET, // Offset for start of MAC custom commands
    ETH_CMD_CUSTOM_PHY_CMDS = ETH_CMD_CUSTOM_PHY_CMDS_OFFSET, // Offset for start of PHY custom commands
} esp_eth_io_cmd_t;

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
        .read_phy_reg = NULL,            \
        .write_phy_reg = NULL,           \
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
*       - ESP_ERR_INVALID_STATE: invalid driver state (e.i. driver is not started)
*       - ESP_ERR_TIMEOUT: transmit frame buffer failed because HW was not get available in predefined period
*       - ESP_FAIL: transmit frame buffer failed because some other error occurred
*/
esp_err_t esp_eth_transmit(esp_eth_handle_t hdl, void *buf, size_t length);

/**
* @brief Special Transmit with variable number of arguments
*
* @param[in] hdl handle of Ethernet driver
* @param[in] argc number variable arguments
* @param ... variable arguments
* @return
*       - ESP_OK: transmit successful
*       - ESP_ERR_INVALID_STATE: invalid driver state (e.i. driver is not started)
*       - ESP_ERR_TIMEOUT: transmit frame buffer failed because HW was not get available in predefined period
*       - ESP_FAIL: transmit frame buffer failed because some other error occurred
*/
esp_err_t esp_eth_transmit_vargs(esp_eth_handle_t hdl, uint32_t argc, ...);

/**
* @brief Misc IO function of Ethernet driver
*
* @param[in] hdl: handle of Ethernet driver
* @param[in] cmd: IO control command
* @param[in, out] data: address of data for `set` command or address where to store the data when used with `get` command
*
* @return
*       - ESP_OK: process io command successfully
*       - ESP_ERR_INVALID_ARG: process io command failed because of some invalid argument
*       - ESP_FAIL: process io command failed because some other error occurred
*       - ESP_ERR_NOT_SUPPORTED: requested feature is not supported
*
* The following common IO control commands are supported:
* @li @c ETH_CMD_S_MAC_ADDR sets Ethernet interface MAC address. @c data argument is pointer to MAC address buffer with expected size of 6 bytes.
* @li @c ETH_CMD_G_MAC_ADDR gets Ethernet interface MAC address. @c data argument is pointer to a buffer to which MAC address is to be copied. The buffer size must be at least 6 bytes.
* @li @c ETH_CMD_S_PHY_ADDR sets PHY address in range of <0-31>. @c data argument is pointer to memory of uint32_t datatype from where the configuration option is read.
* @li @c ETH_CMD_G_PHY_ADDR gets PHY address. @c data argument is pointer to memory of uint32_t datatype to which the PHY address is to be stored.
* @li @c ETH_CMD_S_AUTONEGO enables or disables Ethernet link speed and duplex mode autonegotiation. @c data argument is pointer to memory of bool datatype from which the configuration option is read.
*                           Preconditions: Ethernet driver needs to be stopped.
* @li @c ETH_CMD_G_AUTONEGO gets current configuration of the Ethernet link speed and duplex mode autonegotiation. @c data argument is pointer to memory of bool datatype to which the current configuration is to be stored.
* @li @c ETH_CMD_S_SPEED sets the Ethernet link speed. @c data argument is pointer to memory of eth_speed_t datatype from which the configuration option is read.
*                           Preconditions: Ethernet driver needs to be stopped and auto-negotiation disabled.
* @li @c ETH_CMD_G_SPEED gets current Ethernet link speed. @c data argument is pointer to memory of eth_speed_t datatype to which the speed is to be stored.
* @li @c ETH_CMD_S_PROMISCUOUS sets/resets Ethernet interface promiscuous mode. @c data argument is pointer to memory of bool datatype from which the configuration option is read.
* @li @c ETH_CMD_S_FLOW_CTRL sets/resets Ethernet interface flow control. @c data argument is pointer to memory of bool datatype from which the configuration option is read.
* @li @c ETH_CMD_S_DUPLEX_MODE sets the Ethernet duplex mode. @c data argument is pointer to memory of eth_duplex_t datatype from which the configuration option is read.
*                            Preconditions: Ethernet driver needs to be stopped and auto-negotiation disabled.
* @li @c ETH_CMD_G_DUPLEX_MODE gets current Ethernet link duplex mode.  @c data argument is pointer to memory of eth_duplex_t datatype to which the duplex mode is to be stored.
* @li @c ETH_CMD_S_PHY_LOOPBACK sets/resets PHY to/from loopback mode. @c data argument is pointer to memory of bool datatype from which the configuration option is read.
*
* @li Note that additional control commands may be available for specific MAC or PHY chips. Please consult specific MAC or PHY documentation or driver code.
*/
esp_err_t esp_eth_ioctl(esp_eth_handle_t hdl, esp_eth_io_cmd_t cmd, void *data);

/**
* @brief Get PHY instance memory address
*
* @param[in] hdl handle of Ethernet driver
* @param[out] phy pointer to memory to store the instance
* @return esp_err_t
*       - ESP_OK: success
*       - ESP_ERR_INVALID_ARG: failed because of some invalid argument
*/
esp_err_t esp_eth_get_phy_instance(esp_eth_handle_t hdl, esp_eth_phy_t **phy);

/**
* @brief Get MAC instance memory address
*
* @param[in] hdl handle of Ethernet driver
* @param[out] mac pointer to memory to store the instance
* @return esp_err_t
*       - ESP_OK: success
*       - ESP_ERR_INVALID_ARG: failed because of some invalid argument
*/
esp_err_t esp_eth_get_mac_instance(esp_eth_handle_t hdl, esp_eth_mac_t **mac);

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
