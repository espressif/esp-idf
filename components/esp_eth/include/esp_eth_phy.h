// Copyright 2019-2021 Espressif Systems (Shanghai) PTE LTD
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

#include <stdbool.h>
#include "esp_eth_com.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_ETH_PHY_ADDR_AUTO (-1)

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
    * @param[in] phyL Ethernet PHY instance
    *
    * @return
    *      - ESP_OK: deinitialize Ethernet PHY successfully
    *      - ESP_FAIL: deinitialize Ethernet PHY failed because some error occurred
    *
    */
    esp_err_t (*deinit)(esp_eth_phy_t *phy);

    /**
    * @brief Start auto negotiation
    *
    * @param[in] phy: Ethernet PHY instance
    *
    * @return
    *      - ESP_OK: restart auto negotiation successfully
    *      - ESP_FAIL: restart auto negotiation failed because some error occurred
    *
    */
    esp_err_t (*negotiate)(esp_eth_phy_t *phy);

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
    * @brief
    *
    * @param[in] phy: Ethernet PHY instance
    * @param[in] enable: enables or disables PHY loopback
    *
    * @return
    *      - ESP_OK: configures PHY instance loopback function successfully
    *      - ESP_FAIL: PHY instance loopback configuration failed because some error occurred
    *
    */
    esp_err_t (*loopback)(esp_eth_phy_t *phy, bool enable);

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
* @brief Create a PHY instance of LAN8720
*
* @note For ESP-IDF backwards compatibility reasons. In all other cases, use esp_eth_phy_new_lan87xx instead.
*
* @param[in] config: configuration of PHY
*
* @return
*      - instance: create PHY instance successfully
*      - NULL: create PHY instance failed because some error occurred
*/
static inline esp_eth_phy_t *esp_eth_phy_new_lan8720(const eth_phy_config_t *config)
{
    return esp_eth_phy_new_lan87xx(config);
}

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
* @brief Create a PHY instance of KSZ8041
*
* @param[in] config: configuration of PHY
*
* @return
*      - instance: create PHY instance successfully
*      - NULL: create PHY instance failed because some error occurred
*/
esp_eth_phy_t *esp_eth_phy_new_ksz8041(const eth_phy_config_t *config);

/**
* @brief Create a PHY instance of KSZ8081
*
* @param[in] config: configuration of PHY
*
* @return
*      - instance: create PHY instance successfully
*      - NULL: create PHY instance failed because some error occurred
*/
esp_eth_phy_t *esp_eth_phy_new_ksz8081(const eth_phy_config_t *config);

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
