/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_err.h"
#include "esp_event_base.h"
#include "hal/eth_types.h"
#include "esp_eth_spec.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Offset for start of MAC custom ioctl commands
 *
 */
#define ETH_CMD_CUSTOM_MAC_CMDS_OFFSET      0x0FFF
/**
 * @brief Offset for start of PHY custom ioctl commands
 *
 */
#define ETH_CMD_CUSTOM_PHY_CMDS_OFFSET      0x1FFF

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

#ifdef __cplusplus
}
#endif
