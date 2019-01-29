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

#ifndef __ESP_ETH_H__
#define __ESP_ETH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_types.h"
#include "esp_err.h"

/**
 * @brief Ethernet interface mode
 *
 */
typedef enum {
    ETH_MODE_RMII = 0, /*!< RMII mode */
    ETH_MODE_MII,      /*!< MII mode */
} eth_mode_t;

/**
 * @brief Ethernet clock mode
 *
 */
typedef enum {
    ETH_CLOCK_GPIO0_IN = 0,   /*!< RMII clock input to GPIO0 */
    ETH_CLOCK_GPIO0_OUT = 1,  /*!< RMII clock output from GPIO0 */
    ETH_CLOCK_GPIO16_OUT = 2, /*!< RMII clock output from GPIO16 */
    ETH_CLOCK_GPIO17_OUT = 3  /*!< RMII clock output from GPIO17 */
} eth_clock_mode_t;

/**
 * @brief Ethernet Speed
 *
 */
typedef enum {
    ETH_SPEED_MODE_10M = 0, /*!< Ethernet speed: 10Mbps */
    ETH_SPEED_MODE_100M,    /*!< Ethernet speed: 100Mbps */
} eth_speed_mode_t;

/**
 * @brief Ethernet Duplex
 *
 */
typedef enum {
    ETH_MODE_HALFDUPLEX = 0, /*!< Ethernet half duplex */
    ETH_MODE_FULLDUPLEX,     /*!< Ethernet full duplex */
} eth_duplex_mode_t;

/**
 * @brief Ethernet PHY address
 *
 */
typedef enum {
    PHY0 = 0, /*!< PHY address 0 */
    PHY1,     /*!< PHY address 1 */
    PHY2,     /*!< PHY address 2 */
    PHY3,     /*!< PHY address 3 */
    PHY4,     /*!< PHY address 4 */
    PHY5,     /*!< PHY address 5 */
    PHY6,     /*!< PHY address 6 */
    PHY7,     /*!< PHY address 7 */
    PHY8,     /*!< PHY address 8 */
    PHY9,     /*!< PHY address 9 */
    PHY10,    /*!< PHY address 10 */
    PHY11,    /*!< PHY address 11 */
    PHY12,    /*!< PHY address 12 */
    PHY13,    /*!< PHY address 13 */
    PHY14,    /*!< PHY address 14 */
    PHY15,    /*!< PHY address 15 */
    PHY16,    /*!< PHY address 16 */
    PHY17,    /*!< PHY address 17 */
    PHY18,    /*!< PHY address 18 */
    PHY19,    /*!< PHY address 19 */
    PHY20,    /*!< PHY address 20 */
    PHY21,    /*!< PHY address 21 */
    PHY22,    /*!< PHY address 22 */
    PHY23,    /*!< PHY address 23 */
    PHY24,    /*!< PHY address 24 */
    PHY25,    /*!< PHY address 25 */
    PHY26,    /*!< PHY address 26 */
    PHY27,    /*!< PHY address 27 */
    PHY28,    /*!< PHY address 28 */
    PHY29,    /*!< PHY address 29 */
    PHY30,    /*!< PHY address 30 */
    PHY31     /*!< PHY address 31 */
} eth_phy_base_t;

typedef bool (*eth_phy_check_link_func)(void);
typedef void (*eth_phy_check_init_func)(void);
typedef eth_speed_mode_t (*eth_phy_get_speed_mode_func)(void);
typedef eth_duplex_mode_t (*eth_phy_get_duplex_mode_func)(void);
typedef esp_err_t (*eth_phy_func)(void);
typedef esp_err_t (*eth_tcpip_input_func)(void *buffer, uint16_t len, void *eb);
typedef void (*eth_gpio_config_func)(void);
typedef bool (*eth_phy_get_partner_pause_enable_func)(void);
typedef void (*eth_phy_power_enable_func)(bool enable);

/**
 * @brief ethernet configuration
 *
 */
typedef struct {
    eth_phy_base_t phy_addr;                                            /*!< PHY address (0~31) */
    eth_mode_t mac_mode;                                                /*!< MAC mode: only support RMII now */
    eth_clock_mode_t clock_mode;                                        /*!< external/internal clock mode selection */
    eth_tcpip_input_func tcpip_input;                                   /*!< tcpip input func */
    eth_phy_func phy_init;                                              /*!< phy init func */
    eth_phy_check_link_func phy_check_link;                             /*!< phy check link func */
    eth_phy_check_init_func phy_check_init;                             /*!< phy check init func */
    eth_phy_get_speed_mode_func phy_get_speed_mode;                     /*!< phy check init func */
    eth_phy_get_duplex_mode_func phy_get_duplex_mode;                   /*!< phy check init func */
    eth_gpio_config_func gpio_config;                                   /*!< gpio config func  */
    bool flow_ctrl_enable;                                              /*!< flag of flow ctrl enable */
    eth_phy_get_partner_pause_enable_func phy_get_partner_pause_enable; /*!< get partner pause enable */
    eth_phy_power_enable_func phy_power_enable;                         /*!< enable or disable phy power */
    uint32_t reset_timeout_ms;                                          /*!< timeout value for reset emac */
} eth_config_t;

/**
 * @brief  Init ethernet mac
 *
 * @note   config can not be NULL, and phy chip must be suitable to phy init func.
 *
 * @param[in] config  mac init data.
 *
 * @return
 *      - ESP_OK
 *      - ESP_FAIL
 */
esp_err_t esp_eth_init(eth_config_t *config);

/**
 * @brief  Deinit ethernet mac
 *
 * @return
 *      - ESP_OK
 *      - ESP_FAIL
 *      - ESP_ERR_INVALID_STATE
 */
esp_err_t esp_eth_deinit(void);

/**
 * @brief  Init Ethernet mac driver only
 *
 * For the most part, you need not call this function directly. It gets called
 * from esp_eth_init().
 *
 * This function may be called, if you only need to initialize the Ethernet
 * driver without having to use the network stack on top.
 *
 * @note   config can not be NULL, and phy chip must be suitable to phy init func.
 * @param[in] config  mac init data.
 *
 * @return
 *      - ESP_OK
 *      - ESP_FAIL
 */
esp_err_t esp_eth_init_internal(eth_config_t *config);

/**
 * @brief  Send packet from tcp/ip to mac
 *
 * @note   buf can not be NULL, size must be less than 1580
 *
 * @param[in] buf:  start address of packet data.
 *
 * @param[in] size:  size (byte) of packet data.
 *
 * @return
 *      - ESP_OK
 *      - ESP_FAIL
 */
esp_err_t esp_eth_tx(uint8_t *buf, uint16_t size);

/**
 * @brief  Enable ethernet interface
 *
 * @note   Should be called after esp_eth_init
 *
 * @return
 *      - ESP_OK
 *      - ESP_FAIL
 */
esp_err_t esp_eth_enable(void);

/**
 * @brief  Disable ethernet interface
 *
 * @note   Should be called after esp_eth_init
 *
 * @return
 *      - ESP_OK
 *      - ESP_FAIL
 */
esp_err_t esp_eth_disable(void);

/**
 * @brief  Get mac addr
 *
 * @note   mac addr must be a valid unicast address
 *
 * @param[out] mac:  start address of mac address.
 */
void esp_eth_get_mac(uint8_t mac[6]);

/**
 * @brief  Write PHY reg with SMI interface.
 *
 * @note  PHY base addr must be right.
 *
 * @param[in] reg_num:  PHY reg num.
 *
 * @param[in] value: value which is written to PHY reg.
 */
void esp_eth_smi_write(uint32_t reg_num, uint16_t value);

/**
 * @brief  Read PHY reg with SMI interface.
 *
 * @note  PHY base addr must be right.
 *
 * @param[in] reg_num:  PHY reg num.
 *
 * @return value that is read from PHY reg
 */
uint16_t esp_eth_smi_read(uint32_t reg_num);

/**
 * @brief Continuously read a PHY register over SMI interface, wait until the register has the desired value.
 *
 * @note PHY base address must be right.
 *
 * @param reg_num: PHY register number
 * @param value: Value to wait for (masked with value_mask)
 * @param value_mask: Mask of bits to match in the register.
 * @param timeout_ms: Timeout to wait for this value (milliseconds). 0 means never timeout.
 *
 * @return ESP_OK if desired value matches, ESP_ERR_TIMEOUT if timed out.
 */
esp_err_t esp_eth_smi_wait_value(uint32_t reg_num, uint16_t value, uint16_t value_mask, int timeout_ms);

/**
 * @brief Continuously read a PHY register over SMI interface, wait until the register has all bits in a mask set.
 *
 * @note PHY base address must be right.
 *
 * @param reg_num: PHY register number
 * @param value_mask: Value mask to wait for (all bits in this mask must be set)
 * @param timeout_ms: Timeout to wait for this value (milliseconds). 0 means never timeout.
 *
 * @return ESP_OK if desired value matches, ESP_ERR_TIMEOUT if timed out.
 */
static inline esp_err_t esp_eth_smi_wait_set(uint32_t reg_num, uint16_t value_mask, int timeout_ms)
{
    return esp_eth_smi_wait_value(reg_num, value_mask, value_mask, timeout_ms);
}

/**
 * @brief  Free emac rx buf.
 *
 * @note  buf can not be null, and it is tcpip input buf.
 *
 * @param[in] buf: start address of received packet data.
 *
 */
void esp_eth_free_rx_buf(void *buf);

/**
 * @brief  Set mac of ethernet interface.
 *
 * @note user can call this function after emac_init, and the new mac address will be enabled after emac_enable.
 *
 * @param[in] mac: the Mac address.
 *
 * @return
 *    - ESP_OK: succeed
 *    - ESP_ERR_INVALID_MAC: invalid mac address
 */
esp_err_t esp_eth_set_mac(const uint8_t mac[6]);

/**
 * @brief Get Ethernet link speed
 *
 * @return eth_speed_mode_t ETH_SPEED_MODE_10M when link speed is 10Mbps
 *                          ETH_SPEED_MODE_100M when link speed is 100Mbps
 */
eth_speed_mode_t esp_eth_get_speed(void);

#ifdef __cplusplus
}
#endif

#endif
