// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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

/**
 * @brief Ethernet frame CRC length
 *
 */
#define ETH_CRC_LEN (4)

/**
* @brief Ethernet interface
*
*/
typedef enum {
    EMAC_DATA_INTERFACE_RMII,    /*!< Reduced Media Independent Interface */
    EMAC_DATA_INTERFACE_MII,     /*!< Media Independent Interface */
} eth_data_interface_t;

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
    ETH_SPEED_10M,  /*!< Ethernet speed is 10Mbps */
    ETH_SPEED_100M, /*!< Ethernet speed is 100Mbps */
    ETH_SPEED_MAX   /*!< Max speed mode (for checking purpose) */
} eth_speed_t;

/**
* @brief Ethernet duplex mode
*
*/
typedef enum {
    ETH_DUPLEX_HALF,    /*!< Ethernet is in half duplex */
    ETH_DUPLEX_FULL,    /*!< Ethernet is in full duplex */
} eth_duplex_t;

/**
* @brief Ethernet Checksum
*/
typedef enum {
    ETH_CHECKSUM_SW, /*!< Ethernet checksum calculate by software */
    ETH_CHECKSUM_HW  /*!< Ethernet checksum calculate by hardware */
} eth_checksum_t;
