/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include "esp_eth_com.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_ETH_PHY_ADDR_AUTO (-1)

/**
 * @brief Auto-negotiation controll commands
 *
 */
typedef enum {
    ESP_ETH_PHY_AUTONEGO_RESTART,
    ESP_ETH_PHY_AUTONEGO_EN,
    ESP_ETH_PHY_AUTONEGO_DIS,
    ESP_ETH_PHY_AUTONEGO_G_STAT,
} eth_phy_autoneg_cmd_t;

/**
* @brief Ethernet PHY
*
*/
typedef struct esp_eth_phy_s esp_eth_phy_t;

/**
* @brief Ethernet PHY
*
*/
struct esp_eth_phy_s {
    /**
    * @brief Set mediator for PHY
    *
    * @param[in] phy: Ethernet PHY instance
    * @param[in] mediator: mediator of Ethernet driver
    *
    * @return
    *      - ESP_OK: set mediator for Ethernet PHY instance successfully
    *      - ESP_ERR_INVALID_ARG: set mediator for Ethernet PHY instance failed because of some invalid arguments
    *
    */
    esp_err_t (*set_mediator)(esp_eth_phy_t *phy, esp_eth_mediator_t *mediator);

    /**
    * @brief Software Reset Ethernet PHY
    *
    * @param[in] phy: Ethernet PHY instance
    *
    * @return
    *      - ESP_OK: reset Ethernet PHY successfully
    *      - ESP_FAIL: reset Ethernet PHY failed because some error occurred
    *
    */
    esp_err_t (*reset)(esp_eth_phy_t *phy);

    /**
    * @brief Hardware Reset Ethernet PHY
    *
    * @note Hardware reset is mostly done by pull down and up PHY's nRST pin
    *
    * @param[in] phy: Ethernet PHY instance
    *
    * @return
    *      - ESP_OK: reset Ethernet PHY successfully
    *      - ESP_FAIL: reset Ethernet PHY failed because some error occurred
    *
    */
    esp_err_t (*reset_hw)(esp_eth_phy_t *phy);

    /**
    * @brief Initialize Ethernet PHY
    *
    * @param[in] phy: Ethernet PHY instance
    *
    * @return
    *      - ESP_OK: initialize Ethernet PHY successfully
    *      - ESP_FAIL: initialize Ethernet PHY failed because some error occurred
    *
    */
    esp_err_t (*init)(esp_eth_phy_t *phy);

    /**
    * @brief Deinitialize Ethernet PHY
    *
    * @param[in] phy: Ethernet PHY instance
    *
    * @return
    *      - ESP_OK: deinitialize Ethernet PHY successfully
    *      - ESP_FAIL: deinitialize Ethernet PHY failed because some error occurred
    *
    */
    esp_err_t (*deinit)(esp_eth_phy_t *phy);

    /**
    * @brief Configure auto negotiation
    *
    * @param[in] phy: Ethernet PHY instance
    * @param[in] cmd: Configuration command, it is possible to Enable (restart), Disable or get current status
    *                   of PHY auto negotiation
    * @param[out] autonego_en_stat: Address where to store current status of auto negotiation configuration
    *
    * @return
    *      - ESP_OK: restart auto negotiation successfully
    *      - ESP_FAIL: restart auto negotiation failed because some error occurred
    *      - ESP_ERR_INVALID_ARG: invalid command
    *
    */
    esp_err_t (*autonego_ctrl)(esp_eth_phy_t *phy, eth_phy_autoneg_cmd_t cmd, bool *autonego_en_stat);

    /**
    * @brief Get Ethernet PHY link status
    *
    * @param[in] phy: Ethernet PHY instance
    *
    * @return
    *      - ESP_OK: get Ethernet PHY link status successfully
    *      - ESP_FAIL: get Ethernet PHY link status failed because some error occurred
    *
    */
    esp_err_t (*get_link)(esp_eth_phy_t *phy);

    /**
    * @brief Set Ethernet PHY link status
    *
    * @param[in] phy: Ethernet PHY instance
    * @param[in] link new link status
    *
    * @return
    *      - ESP_OK: set Ethernet PHY link status successfully
    *      - ESP_FAIL: set Ethernet PHY link status failed because some error occurred
    *
    */
    esp_err_t (*set_link)(esp_eth_phy_t *phy, eth_link_t link);

    /**
    * @brief Power control of Ethernet PHY
    *
    * @param[in] phy: Ethernet PHY instance
    * @param[in] enable: set true to power on Ethernet PHY; ser false to power off Ethernet PHY
    *
    * @return
    *      - ESP_OK: control Ethernet PHY power successfully
    *      - ESP_FAIL: control Ethernet PHY power failed because some error occurred
    *
    */
    esp_err_t (*pwrctl)(esp_eth_phy_t *phy, bool enable);

    /**
    * @brief Set PHY chip address
    *
    * @param[in] phy: Ethernet PHY instance
    * @param[in] addr: PHY chip address
    *
    * @return
    *      - ESP_OK: set Ethernet PHY address successfully
    *      - ESP_FAIL: set Ethernet PHY address failed because some error occurred
    *
    */
    esp_err_t (*set_addr)(esp_eth_phy_t *phy, uint32_t addr);

    /**
    * @brief Get PHY chip address
    *
    * @param[in] phy: Ethernet PHY instance
    * @param[out] addr: PHY chip address
    *
    * @return
    *      - ESP_OK: get Ethernet PHY address successfully
    *      - ESP_ERR_INVALID_ARG: get Ethernet PHY address failed because of invalid argument
    *
    */
    esp_err_t (*get_addr)(esp_eth_phy_t *phy, uint32_t *addr);

    /**
    * @brief Advertise pause function supported by MAC layer
    *
    * @param[in] phy: Ethernet PHY instance
    * @param[out] addr: Pause ability
    *
    * @return
    *      - ESP_OK: Advertise pause ability successfully
    *      - ESP_ERR_INVALID_ARG: Advertise pause ability failed because of invalid argument
    *
    */
    esp_err_t (*advertise_pause_ability)(esp_eth_phy_t *phy, uint32_t ability);

    /**
    * @brief Sets the PHY to loopback mode
    *
    * @param[in] phy: Ethernet PHY instance
    * @param[in] enable: enables or disables PHY loopback
    *
    * @return
    *      - ESP_OK: PHY instance loopback mode has been configured successfully
    *      - ESP_FAIL: PHY instance loopback configuration failed because some error occurred
    *
    */
    esp_err_t (*loopback)(esp_eth_phy_t *phy, bool enable);

    /**
    * @brief Sets PHY speed mode
    *
    * @note Autonegotiation feature needs to be disabled prior to calling this function for the new
    *       setting to be applied
    *
    * @param[in] phy: Ethernet PHY instance
    * @param[in] speed: Speed mode to be set
    *
    * @return
    *      - ESP_OK: PHY instance speed mode has been configured successfully
    *      - ESP_FAIL: PHY instance speed mode configuration failed because some error occurred
    *
    */
    esp_err_t (*set_speed)(esp_eth_phy_t *phy, eth_speed_t speed);

    /**
    * @brief Sets PHY duplex mode
    *
    * @note Autonegotiation feature needs to be disabled prior to calling this function for the new
    *       setting to be applied
    *
    * @param[in] phy: Ethernet PHY instance
    * @param[in] duplex: Duplex mode to be set
    *
    * @return
    *      - ESP_OK: PHY instance duplex mode has been configured successfully
    *      - ESP_FAIL: PHY instance duplex mode configuration failed because some error occurred
    *
    */
    esp_err_t (*set_duplex)(esp_eth_phy_t *phy, eth_duplex_t duplex);

    /**
    * @brief Custom IO function of PHY driver. This function is intended to extend common options of esp_eth_ioctl to cover specifics of PHY chip.
    *
    * @note This function may not be assigned when the PHY chip supports only most common set of configuration options.
    *
    * @param[in] phy: Ethernet PHY instance
    * @param[in] cmd: IO control command
    * @param[in, out] data: address of data for `set` command or address where to store the data when used with `get` command
    *
    * @return
    *       - ESP_OK: process io command successfully
    *       - ESP_ERR_INVALID_ARG: process io command failed because of some invalid argument
    *       - ESP_FAIL: process io command failed because some other error occurred
    *       - ESP_ERR_NOT_SUPPORTED: requested feature is not supported
    */
    esp_err_t (*custom_ioctl)(esp_eth_phy_t *phy, uint32_t cmd, void *data);

    /**
    * @brief Free memory of Ethernet PHY instance
    *
    * @param[in] phy: Ethernet PHY instance
    *
    * @return
    *      - ESP_OK: free PHY instance successfully
    *      - ESP_FAIL: free PHY instance failed because some error occurred
    *
    */
    esp_err_t (*del)(esp_eth_phy_t *phy);
};

/**
* @brief Ethernet PHY configuration
*
*/
typedef struct {
    int32_t phy_addr;             /*!< PHY address, set -1 to enable PHY address detection at initialization stage */
    uint32_t reset_timeout_ms;    /*!< Reset timeout value (Unit: ms) */
    uint32_t autonego_timeout_ms; /*!< Auto-negotiation timeout value (Unit: ms) */
    int reset_gpio_num;           /*!< Reset GPIO number, -1 means no hardware reset */
} eth_phy_config_t;

/**
 * @brief Default configuration for Ethernet PHY object
 *
 */
#define ETH_PHY_DEFAULT_CONFIG()           \
    {                                      \
        .phy_addr = ESP_ETH_PHY_ADDR_AUTO, \
        .reset_timeout_ms = 100,           \
        .autonego_timeout_ms = 4000,       \
        .reset_gpio_num = 5,               \
    }

/**
* @brief Create a PHY instance of IP101
*
* @param[in] config: configuration of PHY
*
* @return
*      - instance: create PHY instance successfully
*      - NULL: create PHY instance failed because some error occurred
*/
esp_eth_phy_t *esp_eth_phy_new_ip101(const eth_phy_config_t *config);

/**
* @brief Create a PHY instance of RTL8201
*
* @param[in] config: configuration of PHY
*
* @return
*      - instance: create PHY instance successfully
*      - NULL: create PHY instance failed because some error occurred
*/
esp_eth_phy_t *esp_eth_phy_new_rtl8201(const eth_phy_config_t *config);

/**
* @brief Create a PHY instance of LAN87xx
*
* @param[in] config: configuration of PHY
*
* @return
*      - instance: create PHY instance successfully
*      - NULL: create PHY instance failed because some error occurred
*/
esp_eth_phy_t *esp_eth_phy_new_lan87xx(const eth_phy_config_t *config);

/**
* @brief Create a PHY instance of DP83848
*
* @param[in] config: configuration of PHY
*
* @return
*      - instance: create PHY instance successfully
*      - NULL: create PHY instance failed because some error occurred
*/
esp_eth_phy_t *esp_eth_phy_new_dp83848(const eth_phy_config_t *config);

/**
* @brief Create a PHY instance of KSZ80xx
*
* The phy model from the KSZ80xx series is detected automatically. If the driver
* is unable to detect a supported model, \c NULL is returned.
*
* Currently, the following models are supported:
* KSZ8001, KSZ8021, KSZ8031, KSZ8041, KSZ8051, KSZ8061, KSZ8081, KSZ8091
*
* @param[in] config: configuration of PHY
*
* @return
*      - instance: create PHY instance successfully
*      - NULL: create PHY instance failed because some error occurred
*/
esp_eth_phy_t *esp_eth_phy_new_ksz80xx(const eth_phy_config_t *config);

#if CONFIG_ETH_SPI_ETHERNET_DM9051
/**
* @brief Create a PHY instance of DM9051
*
* @param[in] config: configuration of PHY
*
* @return
*      - instance: create PHY instance successfully
*      - NULL: create PHY instance failed because some error occurred
*/
esp_eth_phy_t *esp_eth_phy_new_dm9051(const eth_phy_config_t *config);
#endif

#if CONFIG_ETH_SPI_ETHERNET_W5500
/**
* @brief Create a PHY instance of W5500
*
* @param[in] config: configuration of PHY
*
* @return
*      - instance: create PHY instance successfully
*      - NULL: create PHY instance failed because some error occurred
*/
esp_eth_phy_t *esp_eth_phy_new_w5500(const eth_phy_config_t *config);
#endif

#if CONFIG_ETH_SPI_ETHERNET_KSZ8851SNL
/**
* @brief Create a PHY instance of KSZ8851SNL
*
* @param[in] config: configuration of PHY
*
* @return
*      - instance: create PHY instance successfully
*      - NULL: create PHY instance failed because some error occurred
*/
esp_eth_phy_t *esp_eth_phy_new_ksz8851snl(const eth_phy_config_t *config);
#endif
#ifdef __cplusplus
}
#endif
