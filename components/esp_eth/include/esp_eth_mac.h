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

#include <stdbool.h>
#include "esp_eth_com.h"
#include "sdkconfig.h"
#if CONFIG_ETH_USE_SPI_ETHERNET
#include "driver/gpio.h"
#include "driver/spi_master.h"
#endif


/**
* @brief Ethernet MAC
*
*/
typedef struct esp_eth_mac_s esp_eth_mac_t;

/**
* @brief Ethernet MAC
*
*/
struct esp_eth_mac_s {
    /**
    * @brief Set mediator for Ethernet MAC
    *
    * @param[in] mac: Ethernet MAC instance
    * @param[in] eth: Ethernet mediator
    *
    * @return
    *      - ESP_OK: set mediator for Ethernet MAC successfully
    *      - ESP_ERR_INVALID_ARG: set mediator for Ethernet MAC failed because of invalid argument
    *
    */
    esp_err_t (*set_mediator)(esp_eth_mac_t *mac, esp_eth_mediator_t *eth);

    /**
    * @brief Initialize Ethernet MAC
    *
    * @param[in] mac: Ethernet MAC instance
    *
    * @return
    *      - ESP_OK: initialize Ethernet MAC successfully
    *      - ESP_ERR_TIMEOUT: initialize Ethernet MAC failed because of timeout
    *      - ESP_FAIL: initialize Ethernet MAC failed because some other error occurred
    *
    */
    esp_err_t (*init)(esp_eth_mac_t *mac);

    /**
    * @brief Deinitialize Ethernet MAC
    *
    * @param[in] mac: Ethernet MAC instance
    *
    * @return
    *      - ESP_OK: deinitialize Ethernet MAC successfully
    *      - ESP_FAIL: deinitialize Ethernet MAC failed because some error occurred
    *
    */
    esp_err_t (*deinit)(esp_eth_mac_t *mac);

    /**
    * @brief Transmit packet from Ethernet MAC
    *
    * @param[in] mac: Ethernet MAC instance
    * @param[in] buf: packet buffer to transmit
    * @param[in] length: length of packet
    *
    * @return
    *      - ESP_OK: transmit packet successfully
    *      - ESP_ERR_INVALID_ARG: transmit packet failed because of invalid argument
    *      - ESP_ERR_INVALID_STATE: transmit packet failed because of wrong state of MAC
    *      - ESP_FAIL: transmit packet failed because some other error occurred
    *
    */
    esp_err_t (*transmit)(esp_eth_mac_t *mac, uint8_t *buf, uint32_t length);

    /**
    * @brief Receive packet from Ethernet MAC
    *
    * @param[in] mac: Ethernet MAC instance
    * @param[out] buf: packet buffer which will preserve the received frame
    * @param[out] length: length of the received packet
    *
    * @note Memory of buf is allocated in the Layer2, make sure it get free after process.
    *
    * @return
    *      - ESP_OK: receive packet successfully
    *      - ESP_ERR_INVALID_ARG: receive packet failed because of invalid argument
    *      - ESP_FAIL: receive packet failed because some other error occurred
    *
    */
    esp_err_t (*receive)(esp_eth_mac_t *mac, uint8_t *buf, uint32_t *length);

    /**
    * @brief Read PHY register
    *
    * @param[in] mac: Ethernet MAC instance
    * @param[in] phy_addr: PHY chip address (0~31)
    * @param[in] phy_reg: PHY register index code
    * @param[out] reg_value: PHY register value
    *
    * @return
    *      - ESP_OK: read PHY register successfully
    *      - ESP_ERR_INVALID_ARG: read PHY register failed because of invalid argument
    *      - ESP_ERR_INVALID_STATE: read PHY register failed because of wrong state of MAC
    *      - ESP_ERR_TIMEOUT: read PHY register failed because of timeout
    *      - ESP_FAIL: read PHY register failed because some other error occurred
    *
    */
    esp_err_t (*read_phy_reg)(esp_eth_mac_t *mac, uint32_t phy_addr, uint32_t phy_reg, uint32_t *reg_value);

    /**
    * @brief Write PHY register
    *
    * @param[in] mac: Ethernet MAC instance
    * @param[in] phy_addr: PHY chip address (0~31)
    * @param[in] phy_reg: PHY register index code
    * @param[in] reg_value: PHY register value
    *
    * @return
    *      - ESP_OK: write PHY register successfully
    *      - ESP_ERR_INVALID_STATE: write PHY register failed because of wrong state of MAC
    *      - ESP_ERR_TIMEOUT: write PHY register failed because of timeout
    *      - ESP_FAIL: write PHY register failed because some other error occurred
    *
    */
    esp_err_t (*write_phy_reg)(esp_eth_mac_t *mac, uint32_t phy_addr, uint32_t phy_reg, uint32_t reg_value);

    /**
    * @brief Set MAC address
    *
    * @param[in] mac: Ethernet MAC instance
    * @param[in] addr: MAC address
    *
    * @return
    *      - ESP_OK: set MAC address successfully
    *      - ESP_ERR_INVALID_ARG: set MAC address failed because of invalid argument
    *      - ESP_FAIL: set MAC address failed because some other error occurred
    *
    */
    esp_err_t (*set_addr)(esp_eth_mac_t *mac, uint8_t *addr);

    /**
    * @brief Get MAC address
    *
    * @param[in] mac: Ethernet MAC instance
    * @param[out] addr: MAC address
    *
    * @return
    *      - ESP_OK: get MAC address successfully
    *      - ESP_ERR_INVALID_ARG: get MAC address failed because of invalid argument
    *      - ESP_FAIL: get MAC address failed because some other error occurred
    *
    */
    esp_err_t (*get_addr)(esp_eth_mac_t *mac, uint8_t *addr);

    /**
    * @brief Set speed of MAC
    *
    * @param[in] ma:c Ethernet MAC instance
    * @param[in] speed: MAC speed
    *
    * @return
    *      - ESP_OK: set MAC speed successfully
    *      - ESP_ERR_INVALID_ARG: set MAC speed failed because of invalid argument
    *      - ESP_FAIL: set MAC speed failed because some other error occurred
    *
    */
    esp_err_t (*set_speed)(esp_eth_mac_t *mac, eth_speed_t speed);

    /**
    * @brief Set duplex mode of MAC
    *
    * @param[in] mac: Ethernet MAC instance
    * @param[in] duplex: MAC duplex
    *
    * @return
    *      - ESP_OK: set MAC duplex mode successfully
    *      - ESP_ERR_INVALID_ARG: set MAC duplex failed because of invalid argument
    *      - ESP_FAIL: set MAC duplex failed because some other error occurred
    *
    */
    esp_err_t (*set_duplex)(esp_eth_mac_t *mac, eth_duplex_t duplex);

    /**
    * @brief Set link status of MAC
    *
    * @param[in] mac: Ethernet MAC instance
    * @param[in] link: Link status
    *
    * @return
    *      - ESP_OK: set link status successfully
    *      - ESP_ERR_INVALID_ARG: set link status failed because of invalid argument
    *      - ESP_FAIL: set link status failed because some other error occurred
    *
    */
    esp_err_t (*set_link)(esp_eth_mac_t *mac, eth_link_t link);

    /**
    * @brief Set promiscuous of MAC
    *
    * @param[in] mac: Ethernet MAC instance
    * @param[in] enable: set true to enable promiscuous mode; set false to disable promiscuous mode
    *
    * @return
    *      - ESP_OK: set promiscuous mode successfully
    *      - ESP_FAIL: set promiscuous mode failed because some error occurred
    *
    */
    esp_err_t (*set_promiscuous)(esp_eth_mac_t *mac, bool enable);

    /**
    * @brief Free memory of Ethernet MAC
    *
    * @param[in] mac: Ethernet MAC instance
    *
    * @return
    *      - ESP_OK: free Ethernet MAC instance successfully
    *      - ESP_FAIL: free Ethernet MAC instance failed because some error occurred
    *
    */
    esp_err_t (*del)(esp_eth_mac_t *mac);
};

/**
* @brief Configuration of Ethernet MAC object
*
*/
typedef struct {
    uint32_t sw_reset_timeout_ms; /*!< Software reset timeout value (Unit: ms) */
    uint32_t rx_task_stack_size;  /*!< Stack size of the receive task */
    uint32_t rx_task_prio;        /*!< Priority of the receive task */
} eth_mac_config_t;

/**
 * @brief Default configuration for Ethernet MAC object
 *
 */
#define ETH_MAC_DEFAULT_CONFIG()    \
    {                               \
        .sw_reset_timeout_ms = 100, \
        .rx_task_stack_size = 4096, \
        .rx_task_prio = 15,         \
    }

#if CONFIG_ETH_USE_ESP32_EMAC
/**
* @brief Create ESP32 Ethernet MAC instance
*
* @param config: Ethernet MAC configuration
*
* @return
*      - instance: create MAC instance successfully
*      - NULL: create MAC instance failed because some error occurred
*/
esp_eth_mac_t *esp_eth_mac_new_esp32(const eth_mac_config_t *config);
#endif

#if CONFIG_ETH_SPI_ETHERNET_DM9051
/**
 * @brief DM9051 specific configuration
 *
 */
typedef struct {
    spi_device_handle_t spi_hdl; /*!< Handle of SPI device driver */
} eth_dm9051_config_t;

/**
 * @brief Default DM9051 specific configuration
 *
 */
#define ETH_DM9051_DEFAULT_CONFIG(spi_device) \
    {                                         \
        .spi_hdl = spi_device,                \
    }

/**
* @brief Create DM9051 Ethernet MAC instance
*
* @param dm9051_config: DM9051 specific configuration
* @param mac_config: Ethernet MAC configuration
*
* @return
*      - instance: create MAC instance successfully
*      - NULL: create MAC instance failed because some error occurred
*/
esp_eth_mac_t *esp_eth_mac_new_dm9051(const eth_dm9051_config_t *dm9051_config, const eth_mac_config_t *mac_config);
#endif
#ifdef __cplusplus
}
#endif
