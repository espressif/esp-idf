/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
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
 * @brief IEEE 802.3 MMD modes enumeration
 *
 */
typedef enum {
    MMD_FUNC_ADDRESS = 0,
    MMD_FUNC_DATA_NOINCR = 1,
    MMD_FUNC_DATA_RWINCR = 2,
    MMD_FUNC_DATA_WINCR = 3
} esp_eth_phy_802_3_mmd_func_t;

/**
 * @brief Set Ethernet mediator
 *
 * @param phy_802_3 IEEE 802.3 PHY object infostructure
 * @param eth Ethernet mediator pointer
 * @return
 *      - ESP_OK: Ethermet mediator set successfuly
 *      - ESP_ERR_INVALID_ARG: if @c eth is @c NULL
 */
esp_err_t esp_eth_phy_802_3_set_mediator(phy_802_3_t *phy_802_3, esp_eth_mediator_t *eth);

/**
 * @brief Reset PHY
 *
 * @param phy_802_3 IEEE 802.3 PHY object infostructure
 * @return
 *      - ESP_OK: Ethernet PHY reset successfuly
 *      - ESP_FAIL: reset Ethernet PHY failed because some error occured
 */
esp_err_t esp_eth_phy_802_3_reset(phy_802_3_t *phy_802_3);

/**
 * @brief Control autonegotiation mode of Ethernet PHY
 *
 * @param phy_802_3 IEEE 802.3 PHY object infostructure
 * @param cmd autonegotiation command enumeration
 * @param[out] autonego_en_stat autonegotiation enabled flag
 * @return
 *      - ESP_OK: Ethernet PHY autonegotiation configured successfuly
 *      - ESP_FAIL: Ethernet PHY autonegotiation configuration fail because some error occured
 *      - ESP_ERR_INVALID_ARG: invalid value of @c cmd
 */
esp_err_t esp_eth_phy_802_3_autonego_ctrl(phy_802_3_t *phy_802_3, eth_phy_autoneg_cmd_t cmd, bool *autonego_en_stat);

/**
 * @brief Power control of Ethernet PHY
 *
 * @param phy_802_3 IEEE 802.3 PHY object infostructure
 * @param enable set true to power ON Ethernet PHY; set false to power OFF Ethernet PHY
 * @return
 *      - ESP_OK: Ethernet PHY power down mode set successfuly
 *      - ESP_FAIL: Ethernet PHY power up or power down failed because some error occured
 */
esp_err_t esp_eth_phy_802_3_pwrctl(phy_802_3_t *phy_802_3, bool enable);

/**
 * @brief Set Ethernet PHY address
 *
 * @param phy_802_3 IEEE 802.3 PHY object infostructure
 * @param addr new PHY address
 * @return
 *      - ESP_OK: Ethernet PHY address set
 */
esp_err_t esp_eth_phy_802_3_set_addr(phy_802_3_t *phy_802_3, uint32_t addr);

/**
 * @brief Get Ethernet PHY address
 *
 * @param phy_802_3 IEEE 802.3 PHY object infostructure
 * @param[out] addr Ethernet PHY address
 * @return
 *      - ESP_OK: Ethernet PHY address read successfuly
 *      - ESP_ERR_INVALID_ARG: @c addr pointer is @c NULL
 */
esp_err_t esp_eth_phy_802_3_get_addr(phy_802_3_t *phy_802_3, uint32_t *addr);

/**
 * @brief Advertise pause function ability
 *
 * @param phy_802_3 IEEE 802.3 PHY object infostructure
 * @param ability enable or disable pause ability
 * @return
 *      - ESP_OK: pause ability set successfuly
 *      - ESP_FAIL: Advertise pause function ability failed because some error occured
 */
esp_err_t esp_eth_phy_802_3_advertise_pause_ability(phy_802_3_t *phy_802_3, uint32_t ability);

/**
 * @brief Set Ethernet PHY loopback mode
 *
 * @param phy_802_3 IEEE 802.3 PHY object infostructure
 * @param enable set true to enable loopback; set false to disable loopback
 * @return
 *      - ESP_OK: Ethernet PHY loopback mode set successfuly
 *      - ESP_FAIL: Ethernet PHY loopback configuration failed because some error occured
 */
esp_err_t esp_eth_phy_802_3_loopback(phy_802_3_t *phy_802_3, bool enable);

/**
 * @brief Set Ethernet PHY speed
 *
 * @param phy_802_3 IEEE 802.3 PHY object infostructure
 * @param speed new speed of Ethernet PHY link
 * @return
 *      - ESP_OK: Ethernet PHY speed set successfuly
 *      - ESP_FAIL: Set Ethernet PHY speed failed because some error occured
 */
esp_err_t esp_eth_phy_802_3_set_speed(phy_802_3_t *phy_802_3, eth_speed_t speed);

/**
 * @brief Set Ethernet PHY duplex mode
 *
 * @param phy_802_3 IEEE 802.3 PHY object infostructure
 * @param duplex new duplex mode for Ethernet PHY link
 * @return
 *      - ESP_OK: Ethernet PHY duplex mode set successfuly
 *      - ESP_ERR_INVALID_STATE: unable to set duplex mode to Half if loopback is enabled
 *      - ESP_FAIL: Set Ethernet PHY duplex mode failed because some error occured
 */
esp_err_t esp_eth_phy_802_3_set_duplex(phy_802_3_t *phy_802_3, eth_duplex_t duplex);

/**
 * @brief Set Ethernet PHY link status
 *
 * @param phy_802_3 IEEE 802.3 PHY object infostructure
 * @param link new link status
 * @return
 *      - ESP_OK: Ethernet PHY link set successfuly
 */
esp_err_t esp_eth_phy_802_3_set_link(phy_802_3_t *phy_802_3, eth_link_t link);

/**
 * @brief Initialize Ethernet PHY
 *
 * @param phy_802_3 IEEE 802.3 PHY object infostructure
 * @return
 *      - ESP_OK: Ethernet PHY initialized successfuly
 */
esp_err_t esp_eth_phy_802_3_init(phy_802_3_t *phy_802_3);

/**
 * @brief Power off Eternet PHY
 *
 * @param phy_802_3 IEEE 802.3 PHY object infostructure
 * @return
 *      - ESP_OK: Ethernet PHY powered off successfuly
 */
esp_err_t esp_eth_phy_802_3_deinit(phy_802_3_t *phy_802_3);

/**
 * @brief Delete Ethernet PHY infostructure
 *
 * @param phy_802_3 IEEE 802.3 PHY object infostructure
 * @return
 *      - ESP_OK: Ethrnet PHY infostructure deleted
 */
esp_err_t esp_eth_phy_802_3_del(phy_802_3_t *phy_802_3);

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
 * @brief Reads MDIO device's internal address register
 *
 * @param phy_802_3 IEEE 802.3 PHY object infostructure
 * @param devaddr Address of MDIO device
 * @param[out] mmd_addr Current address stored in device's register
 * @return
 *      - ESP_OK: Address register read successfuly
 *      - ESP_FAIL: Address register read failed because of some error occured
 *      - ESP_ERR_INVALID_ARG: Device address provided is out of range (hardware limits device address to 5 bits)
 */
esp_err_t esp_eth_phy_802_3_get_mmd_addr(phy_802_3_t *phy_802_3, uint8_t devaddr, uint16_t *mmd_addr);

/**
 * @brief Write to DIO device's internal address register
 *
 * @param phy_802_3 IEEE 802.3 PHY object infostructure
 * @param devaddr Address of MDIO device
 * @param[out] mmd_addr New value of MDIO device's address register value
 * @return
 *      - ESP_OK: Address register written to successfuly
 *      - ESP_FAIL: Address register write failed because of some error occured
 *      - ESP_ERR_INVALID_ARG: Device address provided is out of range (hardware limits device address to 5 bits)
 */
esp_err_t esp_eth_phy_802_3_set_mmd_addr(phy_802_3_t *phy_802_3, uint8_t devaddr, uint16_t mmd_addr);

/**
 * @brief Read data of MDIO device's memory at address register
 *
 * @param phy_802_3 IEEE 802.3 PHY object infostructure
 * @param devaddr Address of MDIO device
 * @param function MMD function
 * @param[out] data Data read from the device's memory
 * @return
 *      - ESP_OK: Memory read successfuly
 *      - ESP_FAIL: Memory read failed because of some error occured
 *      - ESP_ERR_INVALID_ARG: Device address provided is out of range (hardware limits device address to 5 bits) or MMD access function is invalid
 */
esp_err_t esp_eth_phy_802_3_read_mmd_data(phy_802_3_t *phy_802_3, uint8_t devaddr, esp_eth_phy_802_3_mmd_func_t function, uint32_t *data);

/**
 * @brief Write data to MDIO device's memory at address register
 *
 * @param phy_802_3 IEEE 802.3 PHY object infostructure
 * @param devaddr Address of MDIO device
 * @param function MMD function
 * @param[out] data Data to write to the device's memory
 * @return
 *      - ESP_OK: Memory written successfuly
 *      - ESP_FAIL: Memory write failed because of some error occured
 *      - ESP_ERR_INVALID_ARG: Device address provided is out of range (hardware limits device address to 5 bits) or MMD access function is invalid
 */
esp_err_t esp_eth_phy_802_3_write_mmd_data(phy_802_3_t *phy_802_3, uint8_t devaddr, esp_eth_phy_802_3_mmd_func_t function, uint32_t data);

/**
 * @brief Set MMD address to mmd_addr with function MMD_FUNC_NOINCR and read contents to *data
 *
 * @param phy_802_3 IEEE 802.3 PHY object infostructure
 * @param devaddr Address of MDIO device
 * @param mmd_addr Address of MDIO device register
 * @param[out] data Data read from the device's memory
 * @return
 *      - ESP_OK: Memory read successfuly
 *      - ESP_FAIL: Memory read failed because of some error occured
 *      - ESP_ERR_INVALID_ARG: Device address provided is out of range (hardware limits device address to 5 bits)
 */
esp_err_t esp_eth_phy_802_3_read_mmd_register(phy_802_3_t *phy_802_3, uint8_t devaddr, uint16_t mmd_addr, uint32_t *data);

/**
 * @brief Set MMD address to mmd_addr with function MMD_FUNC_NOINCR and write data
 *
 * @param phy_802_3 IEEE 802.3 PHY object infostructure
 * @param devaddr Address of MDIO device
 * @param mmd_addr Address of MDIO device register
 * @param[out] data Data to write to the device's memory
 * @return
 *      - ESP_OK: Memory written to successfuly
 *      - ESP_FAIL: Memory write failed because of some error occured
 *      - ESP_ERR_INVALID_ARG: Device address provided is out of range (hardware limits device address to 5 bits)
 */
esp_err_t esp_eth_phy_802_3_write_mmd_register(phy_802_3_t *phy_802_3, uint8_t devaddr, uint16_t mmd_addr, uint32_t data);

/**
 * @brief Returns address to parent IEEE 802.3 PHY object infostructure
 *
 * @param phy Ethernet PHY instance
 * @return phy_802_3_t*
 *      - address to parent IEEE 802.3 PHY object infostructure
 */
inline __attribute__((always_inline)) phy_802_3_t *esp_eth_phy_into_phy_802_3(esp_eth_phy_t *phy)
{
    return __containerof(phy, phy_802_3_t, parent);
}

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
