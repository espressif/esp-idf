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

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ETH_MODE_RMII = 0,
    ETH_MDOE_MII,
} eth_mode_t;

typedef enum {
    ETH_SPEED_MODE_10M = 0,
    ETH_SPEED_MODE_100M,
} eth_speed_mode_t;

typedef enum {
    ETH_MODE_HALFDUPLEX = 0,
    ETH_MDOE_FULLDUPLEX,
} eth_duplex_mode_t;

typedef enum {
    PHY0 = 0,
    PHY1,
    PHY2,
    PHY3,
    PHY4,
    PHY5,
    PHY6,
    PHY7,
    PHY8,
    PHY9,
    PHY10,
    PHY11,
    PHY12,
    PHY13,
    PHY14,
    PHY15,
    PHY16,
    PHY17,
    PHY18,
    PHY19,
    PHY20,
    PHY21,
    PHY22,
    PHY23,
    PHY24,
    PHY25,
    PHY26,
    PHY27,
    PHY28,
    PHY29,
    PHY30,
    PHY31,
} eth_phy_base_t;

typedef bool (*eth_phy_check_link_func)(void);
typedef void (*eth_phy_check_init_func)(void);
typedef eth_speed_mode_t (*eth_phy_get_speed_mode_func)(void);
typedef eth_duplex_mode_t (*eth_phy_get_duplex_mode_func)(void);
typedef void (*eth_phy_func)(void);
typedef esp_err_t (*eth_tcpip_input_func)(void *buffer, uint16_t len, void *eb);
typedef void (*eth_gpio_config_func)(void);
typedef bool (*eth_phy_get_partner_pause_enable_func)(void);


/**
 * @brief ethernet configuration
 *
 */
typedef struct {
    eth_phy_base_t  phy_addr;                   /*!< phy base addr (0~31) */
    eth_mode_t mac_mode;                        /*!< mac mode only support RMII now */
    eth_tcpip_input_func tcpip_input;            /*!< tcpip input func  */
    eth_phy_func phy_init;                       /*!< phy init func  */
    eth_phy_check_link_func phy_check_link;     /*!< phy check link func  */
    eth_phy_check_init_func phy_check_init;     /*!< phy check init func  */
    eth_phy_get_speed_mode_func phy_get_speed_mode;     /*!< phy check init func  */
    eth_phy_get_duplex_mode_func phy_get_duplex_mode;     /*!< phy check init func  */
    eth_gpio_config_func gpio_config;           /*!< gpio config func  */
    bool flow_ctrl_enable;                      /*!< flag of flow ctrl enable */
    eth_phy_get_partner_pause_enable_func  phy_get_partner_pause_enable; /*!< get partner pause enable */
    
} eth_config_t;

/**
 * @brief  Init ethernet mac
 *
 * @note   config can not be NULL,and phy chip must be suitable to phy init func.
 *
 * @param[in] config  mac init data.
 *
 * @return
 *      - ESP_OK
 *      - ESP_FAIL
 */
esp_err_t esp_eth_init(eth_config_t *config);

/**
 * @brief  Send packet from tcp/ip to mac
 *
 * @note   buf can not be NULL,size must be less than 1580
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
 * @note   Shout be called after esp_eth_init
 *
 * @return
 *      - ESP_OK
 *      - ESP_FAIL
 */
esp_err_t esp_eth_enable(void);

/**
 * @brief  Disable ethernet interface
 *
 * @note   Shout be called after esp_eth_init
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
 * @brief  Read phy reg with smi interface.
 *
 * @note  phy base addr must be right.
 *
 * @param[in] reg_num:  phy reg num.
 *
 * @param[in] value: value which write to phy reg.
 */
void esp_eth_smi_write(uint32_t reg_num, uint16_t value);

/**
 * @brief  Write phy reg with smi interface.
 *
 * @note  phy base addr must be right.
 *
 * @param[in] reg_num:  phy reg num.
 *
 * @return value what read from phy reg
 */
uint16_t esp_eth_smi_read(uint32_t reg_num);

/**
 * @brief  Free emac rx buf.
 *
 * @note  buf can not be null,and it is tcpip input buf.
 *
 * @param[in] buf: start address of recevie packet data.
 *
 */
void esp_eth_free_rx_buf(void *buf);

#ifdef __cplusplus
}
#endif

#endif

