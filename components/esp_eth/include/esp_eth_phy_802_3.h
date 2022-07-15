/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include "esp_eth.h"
#include "sdkconfig.h"
#include "eth_phy_802_3_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief IEEE 802.3 PHY object infostructure
 *
 */
typedef struct {
    esp_eth_phy_t parent;           /*!< Parent Ethernet PHY instance */
    esp_eth_mediator_t *eth;        /*!< Mediator of Ethernet driver */
    int addr;                       /*!< PHY address */
    uint32_t reset_timeout_ms;      /*!< Reset timeout value (Unit: ms) */
    uint32_t autonego_timeout_ms;   /*!< Auto-negotiation timeout value (Unit: ms) */
    eth_link_t link_status;         /*!< Current Link status */
    int reset_gpio_num;             /*!< Reset GPIO number, -1 means no hardware reset */
} phy_802_3_t;

/**
 * @brief Performs hardware reset with specific reset pin assertion time
 *
 * @param phy_802_3 IEEE 802.3 PHY object infostructure
 * @param reset_assert_us Hardware reset pin assertion time
 * @return
 *      - ESP_OK: reset Ethernet PHY successfully
 */
esp_err_t esp_eth_phy_802_3_reset_hw(phy_802_3_t *phy_802_3, uint32_t reset_assert_us);

/**
 * @brief Detect PHY address
 *
 * @param eth Mediator of Ethernet driver
 * @param[out] detected_addr: a valid address after detection
 * @return
 *       - ESP_OK: detect phy address successfully
 *       - ESP_ERR_INVALID_ARG: invalid parameter
 *       - ESP_ERR_NOT_FOUND: can't detect any PHY device
 *       - ESP_FAIL: detect phy address failed because some error occurred
 */
esp_err_t esp_eth_phy_802_3_detect_phy_addr(esp_eth_mediator_t *eth, int *detected_addr);

/**
 * @brief Performs basic PHY chip initialization
 *
 * @note It should be called as the first function in PHY specific driver instance
 *
 * @param phy_802_3 IEEE 802.3 PHY object infostructure
 * @return
 *      - ESP_OK: initialized Ethernet PHY successfully
 *      - ESP_FAIL: initialization of Ethernet PHY failed because some error occurred
 *      - ESP_ERR_INVALID_ARG: invalid argument
 *      - ESP_ERR_NOT_FOUND: PHY device not detected
 *      - ESP_ERR_TIMEOUT: MII Management read/write operation timeout
 *      - ESP_ERR_INVALID_STATE: PHY is in invalid state to perform requested operation
 */
esp_err_t esp_eth_phy_802_3_basic_phy_init(phy_802_3_t *phy_802_3);

/**
 * @brief Performs basic PHY chip de-initialization
 *
 * @note It should be called as the last function in PHY specific driver instance
 *
 * @param phy_802_3 IEEE 802.3 PHY object infostructure
 * @return
 *      - ESP_OK: de-initialized Ethernet PHY successfully
 *      - ESP_FAIL: de-initialization of Ethernet PHY failed because some error occurred
 *      - ESP_ERR_TIMEOUT: MII Management read/write operation timeout
 *      - ESP_ERR_INVALID_STATE: PHY is in invalid state to perform requested operation
 */
esp_err_t esp_eth_phy_802_3_basic_phy_deinit(phy_802_3_t *phy_802_3);

/**
 * @brief Reads raw content of OUI field
 *
 * @param phy_802_3 IEEE 802.3 PHY object infostructure
 * @param[out] oui OUI value
 * @return
 *      - ESP_OK: OUI field read successfully
 *      - ESP_FAIL: OUI field read failed because some error occurred
 *      - ESP_ERR_INVALID_ARG: invalid @c oui argument
 *      - ESP_ERR_TIMEOUT: MII Management read/write operation timeout
 *      - ESP_ERR_INVALID_STATE: PHY is in invalid state to perform requested operation
 */
esp_err_t esp_eth_phy_802_3_read_oui(phy_802_3_t *phy_802_3, uint32_t *oui);

/**
 * @brief Reads manufacturer’s model and revision number
 *
 * @param phy_802_3 IEEE 802.3 PHY object infostructure
 * @param[out] model Manufacturer’s model number (can be NULL when not required)
 * @param[out] rev Manufacturer’s revision number (can be NULL when not required)
 * @return
 *      - ESP_OK: Manufacturer’s info read successfully
 *      - ESP_FAIL: Manufacturer’s info read failed because some error occurred
 *      - ESP_ERR_TIMEOUT: MII Management read/write operation timeout
 *      - ESP_ERR_INVALID_STATE: PHY is in invalid state to perform requested operation
 */
esp_err_t esp_eth_phy_802_3_read_manufac_info(phy_802_3_t *phy_802_3, uint8_t *model, uint8_t *rev);

/**
 * @brief Returns address to parent IEEE 802.3 PHY object infostructure
 *
 * @param phy Ethernet PHY instance
 * @return phy_802_3_t*
 *      - address to parent IEEE 802.3 PHY object infostructure
 */
phy_802_3_t *esp_eth_phy_into_phy_802_3(esp_eth_phy_t *phy);

/**
 * @brief Initializes configuration of parent IEEE 802.3 PHY object infostructure
 *
 * @param phy_802_3 Address to IEEE 802.3 PHY object infostructure
 * @param config Configuration of the IEEE 802.3 PHY object
 * @return
 *      - ESP_OK: configuration initialized successfully
 *      - ESP_ERR_INVALID_ARG: invalid @c config argument
 */
esp_err_t esp_eth_phy_802_3_obj_config_init(phy_802_3_t *phy_802_3, const eth_phy_config_t *config);

#ifdef __cplusplus
}
#endif
