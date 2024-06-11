/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include "soc/soc_caps.h"
#include "esp_eth_com.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
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
    * @brief Start Ethernet MAC
    *
    * @param[in] mac: Ethernet MAC instance
    *
    * @return
    *      - ESP_OK: start Ethernet MAC successfully
    *      - ESP_FAIL: start Ethernet MAC failed because some other error occurred
    *
    */
    esp_err_t (*start)(esp_eth_mac_t *mac);

    /**
    * @brief Stop Ethernet MAC
    *
    * @param[in] mac: Ethernet MAC instance
    *
    * @return
    *      - ESP_OK: stop Ethernet MAC successfully
    *      - ESP_FAIL: stop Ethernet MAC failed because some error occurred
    *
    */
    esp_err_t (*stop)(esp_eth_mac_t *mac);

    /**
    * @brief Transmit packet from Ethernet MAC
    *
    * @param[in] mac: Ethernet MAC instance
    * @param[in] buf: packet buffer to transmit
    * @param[in] length: length of packet
    *
    * @return
    *      - ESP_OK: transmit packet successfully
    *      - ESP_ERR_INVALID_SIZE: number of actually sent bytes differs to expected
    *      - ESP_FAIL: transmit packet failed because some other error occurred
    *
    * @note Returned error codes may differ for each specific MAC chip.
    *
    */
    esp_err_t (*transmit)(esp_eth_mac_t *mac, uint8_t *buf, uint32_t length);

    /**
    * @brief Transmit packet from Ethernet MAC constructed with special parameters at Layer2.
    *
    * @param[in] mac: Ethernet MAC instance
    * @param[in] argc: number variable arguments
    * @param[in] args: variable arguments
    *
    * @note Typical intended use case is to make possible to construct a frame from multiple higher layer
    *       buffers without a need of buffer reallocations. However, other use cases are not limited.
    *
    * @return
    *      - ESP_OK: transmit packet successfully
    *      - ESP_ERR_INVALID_SIZE: number of actually sent bytes differs to expected
    *      - ESP_FAIL: transmit packet failed because some other error occurred
    *
    * @note Returned error codes may differ for each specific MAC chip.
    *
    */
    esp_err_t (*transmit_vargs)(esp_eth_mac_t *mac, uint32_t argc, va_list args);

    /**
    * @brief Receive packet from Ethernet MAC
    *
    * @param[in] mac: Ethernet MAC instance
    * @param[out] buf: packet buffer which will preserve the received frame
    * @param[out] length: length of the received packet
    *
    * @note Memory of buf is allocated in the Layer2, make sure it get free after process.
    * @note Before this function got invoked, the value of "length" should set by user, equals the size of buffer.
    *       After the function returned, the value of "length" means the real length of received data.
    *
    * @return
    *      - ESP_OK: receive packet successfully
    *      - ESP_ERR_INVALID_ARG: receive packet failed because of invalid argument
    *      - ESP_ERR_INVALID_SIZE: input buffer size is not enough to hold the incoming data.
    *                              in this case, value of returned "length" indicates the real size of incoming data.
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
    * @brief Enable flow control on MAC layer or not
    *
    * @param[in] mac: Ethernet MAC instance
    * @param[in] enable: set true to enable flow control; set false to disable flow control
    *
    * @return
    *      - ESP_OK: set flow control successfully
    *      - ESP_FAIL: set flow control failed because some error occurred
    *
    */
    esp_err_t (*enable_flow_ctrl)(esp_eth_mac_t *mac, bool enable);

    /**
    * @brief Set the PAUSE ability of peer node
    *
    * @param[in] mac: Ethernet MAC instance
    * @param[in] ability: zero indicates that pause function is supported by link partner; non-zero indicates that pause function is not supported by link partner
    *
    * @return
    *      - ESP_OK: set peer pause ability successfully
    *      - ESP_FAIL: set peer pause ability failed because some error occurred
    */
    esp_err_t (*set_peer_pause_ability)(esp_eth_mac_t *mac, uint32_t ability);

    /**
    * @brief Custom IO function of MAC driver. This function is intended to extend common options of esp_eth_ioctl to cover specifics of MAC chip.
    *
    * @note This function may not be assigned when the MAC chip supports only most common set of configuration options.
    *
    * @param[in] mac: Ethernet MAC instance
    * @param[in] cmd: IO control command
    * @param[in, out] data: address of data for `set` command or address where to store the data when used with `get` command
    *
    * @return
    *       - ESP_OK: process io command successfully
    *       - ESP_ERR_INVALID_ARG: process io command failed because of some invalid argument
    *       - ESP_FAIL: process io command failed because some other error occurred
    *       - ESP_ERR_NOT_SUPPORTED: requested feature is not supported
    */
    esp_err_t (*custom_ioctl)(esp_eth_mac_t *mac, int cmd, void *data);

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
    uint32_t sw_reset_timeout_ms;        /*!< Software reset timeout value (Unit: ms) */
    uint32_t rx_task_stack_size;         /*!< Stack size of the receive task */
    uint32_t rx_task_prio;               /*!< Priority of the receive task */
    uint32_t flags;                      /*!< Flags that specify extra capability for mac driver */
} eth_mac_config_t;

#define ETH_MAC_FLAG_WORK_WITH_CACHE_DISABLE (1 << 0) /*!< MAC driver can work when cache is disabled */
#define ETH_MAC_FLAG_PIN_TO_CORE (1 << 1)             /*!< Pin MAC task to the CPU core where driver installation happened */

/**
 * @brief Default configuration for Ethernet MAC object
 *
 */
#define ETH_MAC_DEFAULT_CONFIG()                          \
    {                                                     \
        .sw_reset_timeout_ms = 100,                       \
        .rx_task_stack_size = 4096,                       \
        .rx_task_prio = 15,                               \
        .flags = 0,                                       \
    }

#ifdef __cplusplus
}
#endif
