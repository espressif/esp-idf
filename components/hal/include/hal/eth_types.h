/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

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

/**
* @brief Internal ethernet EMAC's DMA available burst sizes
*/
typedef enum {
    ETH_DMA_BURST_LEN_32,
    ETH_DMA_BURST_LEN_16,
    ETH_DMA_BURST_LEN_8,
    ETH_DMA_BURST_LEN_4,
    ETH_DMA_BURST_LEN_2,
    ETH_DMA_BURST_LEN_1,
} eth_mac_dma_burst_len_t;
