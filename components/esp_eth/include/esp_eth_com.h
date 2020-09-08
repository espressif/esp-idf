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

#include "esp_err.h"
#include "esp_event_base.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Maximum Ethernet payload size
 *
 */
#define ETH_MAX_PAYLOAD_LEN (1500)

/**
 * @brief Minimum Ethernet payload size
 *
 */
#define ETH_MIN_PAYLOAD_LEN (46)

/**
 * @brief Ethernet frame header size: Dest addr(6 Bytes) + Src addr(6 Bytes) + length/type(2 Bytes)
 *
 */
#define ETH_HEADER_LEN (14)

/**
 * @brief Ethernet frame CRC length
 *
 */
#define ETH_CRC_LEN (4)

/**
 * @brief Optional 802.1q VLAN Tag length
 *
 */
#define ETH_VLAN_TAG_LEN (4)

/**
 * @brief Jumbo frame payload size
 *
 */
#define ETH_JUMBO_FRAME_PAYLOAD_LEN (9000)

/**
 * @brief Maximum frame size (1522 Bytes)
 *
 */
#define ETH_MAX_PACKET_SIZE (ETH_HEADER_LEN + ETH_VLAN_TAG_LEN + ETH_MAX_PAYLOAD_LEN + ETH_CRC_LEN)

/**
 * @brief Minimum frame size (64 Bytes)
 *
 */
#define ETH_MIN_PACKET_SIZE (ETH_HEADER_LEN + ETH_MIN_PAYLOAD_LEN + ETH_CRC_LEN)

/**
* @brief Ethernet driver state
*
*/
typedef enum {
    ETH_STATE_LLINIT, /*!< Lowlevel init done */
    ETH_STATE_DEINIT, /*!< Deinit done */
    ETH_STATE_LINK,   /*!< Link status changed */
    ETH_STATE_SPEED,  /*!< Speed updated */
    ETH_STATE_DUPLEX, /*!< Duplex updated */
    ETH_STATE_PAUSE,  /*!< Pause ability updated */
} esp_eth_state_t;

/**
* @brief Command list for ioctl API
*
*/
typedef enum {
    ETH_CMD_G_MAC_ADDR,    /*!< Get MAC address */
    ETH_CMD_S_MAC_ADDR,    /*!< Set MAC address */
    ETH_CMD_G_PHY_ADDR,    /*!< Get PHY address */
    ETH_CMD_S_PHY_ADDR,    /*!< Set PHY address */
    ETH_CMD_G_SPEED,       /*!< Get Speed */
    ETH_CMD_S_PROMISCUOUS, /*!< Set promiscuous mode */
    ETH_CMD_S_FLOW_CTRL,   /*!< Set flow control */
} esp_eth_io_cmd_t;

/**
* @brief Ethernet link status
*
*/
typedef enum {
    ETH_LINK_UP,  /*!< Ethernet link is up */
    ETH_LINK_DOWN /*!< Ethernet link is down */
} eth_link_t;

/**
* @brief Ethernet speed
*
*/
typedef enum {
    ETH_SPEED_10M, /*!< Ethernet speed is 10Mbps */
    ETH_SPEED_100M /*!< Ethernet speed is 100Mbps */
} eth_speed_t;

/**
* @brief Ethernet duplex mode
*
*/
typedef enum {
    ETH_DUPLEX_HALF, /*!< Ethernet is in half duplex */
    ETH_DUPLEX_FULL  /*!< Ethernet is in full duplex */
} eth_duplex_t;

/**
* @brief Ethernet mediator
*
*/
typedef struct esp_eth_mediator_s esp_eth_mediator_t;

/**
* @brief Ethernet mediator
*
*/
struct esp_eth_mediator_s {
    /**
    * @brief Read PHY register
    *
    * @param[in] eth: mediator of Ethernet driver
    * @param[in] phy_addr: PHY Chip address (0~31)
    * @param[in] phy_reg: PHY register index code
    * @param[out] reg_value: PHY register value
    *
    * @return
    *       - ESP_OK: read PHY register successfully
    *       - ESP_FAIL: read PHY register failed because some error occurred
    *
    */
    esp_err_t (*phy_reg_read)(esp_eth_mediator_t *eth, uint32_t phy_addr, uint32_t phy_reg, uint32_t *reg_value);

    /**
    * @brief Write PHY register
    *
    * @param[in] eth: mediator of Ethernet driver
    * @param[in] phy_addr: PHY Chip address (0~31)
    * @param[in] phy_reg: PHY register index code
    * @param[in] reg_value: PHY register value
    *
    * @return
    *       - ESP_OK: write PHY register successfully
    *       - ESP_FAIL: write PHY register failed because some error occurred
    */
    esp_err_t (*phy_reg_write)(esp_eth_mediator_t *eth, uint32_t phy_addr, uint32_t phy_reg, uint32_t reg_value);

    /**
    * @brief Deliver packet to upper stack
    *
    * @param[in] eth: mediator of Ethernet driver
    * @param[in] buffer: packet buffer
    * @param[in] length: length of the packet
    *
    * @return
    *       - ESP_OK: deliver packet to upper stack successfully
    *       - ESP_FAIL: deliver packet failed because some error occurred
    *
    */
    esp_err_t (*stack_input)(esp_eth_mediator_t *eth, uint8_t *buffer, uint32_t length);

    /**
    * @brief Callback on Ethernet state changed
    *
    * @param[in] eth: mediator of Ethernet driver
    * @param[in] state: new state
    * @param[in] args: optional argument for the new state
    *
    * @return
    *       - ESP_OK: process the new state successfully
    *       - ESP_FAIL: process the new state failed because some error occurred
    *
    */
    esp_err_t (*on_state_changed)(esp_eth_mediator_t *eth, esp_eth_state_t state, void *args);
};

/**
* @brief Ethernet event declarations
*
*/
typedef enum {
    ETHERNET_EVENT_START,        /*!< Ethernet driver start */
    ETHERNET_EVENT_STOP,         /*!< Ethernet driver stop */
    ETHERNET_EVENT_CONNECTED,    /*!< Ethernet got a valid link */
    ETHERNET_EVENT_DISCONNECTED, /*!< Ethernet lost a valid link */
} eth_event_t;

/**
* @brief Ethernet event base declaration
*
*/
ESP_EVENT_DECLARE_BASE(ETH_EVENT);

/**
* @brief Detect PHY address
*
* @param[in] eth: mediator of Ethernet driver
* @param[out] detected_addr: a valid address after detection
* @return
*       - ESP_OK: detect phy address successfully
*       - ESP_ERR_INVALID_ARG: invalid parameter
*       - ESP_ERR_NOT_FOUND: can't detect any PHY device
*       - ESP_FAIL: detect phy address failed because some error occurred
*/
esp_err_t esp_eth_detect_phy_addr(esp_eth_mediator_t *eth, uint32_t *detected_addr);

#ifdef __cplusplus
}
#endif
