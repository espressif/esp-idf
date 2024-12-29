/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

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

/**
 * @brief EMAC System timestamp update update method
 *
 */
typedef enum {
    ETH_PTP_UPDATE_METHOD_COARSE,   /*!< EMAC System timestamp update using the Coarse method */
    ETH_PTP_UPDATE_METHOD_FINE      /*!< EMAC System timestamp update using the Fine method */
} eth_mac_ptp_update_method_t;

/**
 * @brief EMAC System Timestamp Rollover
 *
 */
typedef enum {
    ETH_PTP_DIGITAL_ROLLOVER,       /*!< Digital - subseconds register rolls over after 999999999 value (1 nanosecond accuracy) */
    ETH_PTP_BINARY_ROLLOVER         /*!< Binary - subseconds register rolls over after 0x7FFFFFFF value  */
} eth_mac_ptp_roll_type_t;

#ifdef __cplusplus
}
#endif
