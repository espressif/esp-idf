/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include "soc/soc_caps.h"
#include "esp_eth_com.h"
#include "esp_eth_mac.h"
#include "sdkconfig.h"
#include "driver/spi_master.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_ETH_USE_SPI_ETHERNET

/**
 * @brief Custom SPI Driver Configuration.
 * This structure declares configuration and callback functions to access Ethernet SPI module via
 * user's custom SPI driver.
 *
 */
typedef struct
{
    /**
     * @brief Custom driver specific configuration data used by `init()` function.
     *
     * @note Type and its content is fully under user's control
     *
     */
    void *config;

    /**
     * @brief Custom driver SPI Initialization
     *
     * @param[in] spi_config: Custom driver specific configuration
     *
     * @return
     *      - spi_ctx: when initialization is successful, a pointer to context structure holding all variables
     *          needed for subsequent SPI access operations (e.g. SPI bus identification, mutexes, etc.)
     *      - NULL: driver initialization failed
     *
     * @note return type and its content is fully under user's control
     */
    void *(*init)(const void *spi_config);

    /**
     * @brief Custom driver De-initialization
     *
     * @param[in] spi_ctx: a pointer to driver specific context structure
     *
     * @return
     *      - ESP_OK: driver de-initialization was successful
     *      - ESP_FAIL: driver de-initialization failed
     *      - any other failure codes are allowed to be used to provide failure isolation
     */
    esp_err_t (*deinit)(void *spi_ctx);

    /**
     * @brief Custom driver SPI read
     *
     * @note The read function is responsible to construct command, address and data fields
     * of the SPI frame in format expected by particular SPI Ethernet module
     *
     * @param[in] spi_ctx: a pointer to driver specific context structure
     * @param[in] cmd: command
     * @param[in] addr: register address
     * @param[out] data: read data
     * @param[in] data_len: read data length in bytes
     *
     * @return
     *      - ESP_OK: read was successful
     *      - ESP_FAIL: read failed
     *      - any other failure codes are allowed to be used to provide failure isolation
     */
    esp_err_t (*read)(void *spi_ctx, uint32_t cmd, uint32_t addr, void *data, uint32_t data_len);

    /**
     * @brief Custom driver SPI write
     *
     * @note The write function is responsible to construct command, address and data fields
     * of the SPI frame in format expected by particular SPI Ethernet module
     *
     * @param[in] spi_ctx: a pointer to driver specific context structure
     * @param[in] cmd: command
     * @param[in] addr: register address
     * @param[in] data: data to write
     * @param[in] data_len: length of data to write in bytes
     *
     * @return
     *      - ESP_OK: write was successful
     *      - ESP_FAIL: write failed
     *      - any other failure codes are allowed to be used to provide failure isolation
     */
    esp_err_t (*write)(void *spi_ctx, uint32_t cmd, uint32_t addr, const void *data, uint32_t data_len);
} eth_spi_custom_driver_config_t;

/**
 * @brief Default configuration of the custom SPI driver.
 * Internal ESP-IDF SPI Master driver is used by default.
 *
 */
#define ETH_DEFAULT_SPI  \
    {                    \
        .config = NULL,  \
        .init = NULL,    \
        .deinit = NULL,  \
        .read = NULL,    \
        .write = NULL    \
    }
#endif // CONFIG_ETH_USE_SPI_ETHERNET

#if CONFIG_ETH_SPI_ETHERNET_DM9051
/**
 * @brief DM9051 specific configuration
 *
 */
typedef struct {
    int int_gpio_num;                                   /*!< Interrupt GPIO number, set -1 to not use interrupt and to poll rx status periodically */
    uint32_t poll_period_ms;                            /*!< Period in ms to poll rx status when interrupt mode is not used */
    spi_host_device_t spi_host_id;                      /*!< SPI peripheral (this field is invalid when custom SPI driver is defined) */
    spi_device_interface_config_t *spi_devcfg;          /*!< SPI device configuration (this field is invalid when custom SPI driver is defined) */
    eth_spi_custom_driver_config_t custom_spi_driver;   /*!< Custom SPI driver definitions */
} eth_dm9051_config_t;

/**
 * @brief Default DM9051 specific configuration
 *
 */
#define ETH_DM9051_DEFAULT_CONFIG(spi_host, spi_devcfg_p) \
    {                                           \
        .int_gpio_num = 4,                      \
        .poll_period_ms = 0,                    \
        .spi_host_id = spi_host,                \
        .spi_devcfg = spi_devcfg_p,             \
        .custom_spi_driver = ETH_DEFAULT_SPI,   \
    }

/**
* @brief Create DM9051 Ethernet MAC instance
*
* @param dm9051_config: DM9051 specific configuration
* @param mac_config: Ethernet MAC configuration
*
* @return
*      - instance: create MAC instance successfully
*      - NULL: create MAC instance failed because some error occurred
*/
esp_eth_mac_t *esp_eth_mac_new_dm9051(const eth_dm9051_config_t *dm9051_config, const eth_mac_config_t *mac_config);
#endif // CONFIG_ETH_SPI_ETHERNET_DM9051

#if CONFIG_ETH_SPI_ETHERNET_W5500
/**
 * @brief W5500 specific configuration
 *
 */
typedef struct {
    int int_gpio_num;                                   /*!< Interrupt GPIO number, set -1 to not use interrupt and to poll rx status periodically */
    uint32_t poll_period_ms;                            /*!< Period in ms to poll rx status when interrupt mode is not used */
    spi_host_device_t spi_host_id;                      /*!< SPI peripheral (this field is invalid when custom SPI driver is defined)*/
    spi_device_interface_config_t *spi_devcfg;          /*!< SPI device configuration (this field is invalid when custom SPI driver is defined)*/
    eth_spi_custom_driver_config_t custom_spi_driver;   /*!< Custom SPI driver definitions */
} eth_w5500_config_t;

/**
 * @brief Default W5500 specific configuration
 *
 */
#define ETH_W5500_DEFAULT_CONFIG(spi_host, spi_devcfg_p) \
    {                                           \
        .int_gpio_num = 4,                      \
        .poll_period_ms = 0,                    \
        .spi_host_id = spi_host,                \
        .spi_devcfg = spi_devcfg_p,             \
        .custom_spi_driver = ETH_DEFAULT_SPI,   \
    }

/**
* @brief Create W5500 Ethernet MAC instance
*
* @param w5500_config: W5500 specific configuration
* @param mac_config: Ethernet MAC configuration
*
* @return
*      - instance: create MAC instance successfully
*      - NULL: create MAC instance failed because some error occurred
*/
esp_eth_mac_t *esp_eth_mac_new_w5500(const eth_w5500_config_t *w5500_config, const eth_mac_config_t *mac_config);
#endif // CONFIG_ETH_SPI_ETHERNET_W5500

#if CONFIG_ETH_SPI_ETHERNET_KSZ8851SNL
/**
 * @brief KSZ8851SNL specific configuration
 *
 */
typedef struct {
    int int_gpio_num;                                   /*!< Interrupt GPIO number, set -1 to not use interrupt and to poll rx status periodically */
    uint32_t poll_period_ms;                            /*!< Period in ms to poll rx status when interrupt mode is not used */
    spi_host_device_t spi_host_id;                      /*!< SPI peripheral (this field is invalid when custom SPI driver is defined) */
    spi_device_interface_config_t *spi_devcfg;          /*!< SPI device configuration (this field is invalid when custom SPI driver is defined) */
    eth_spi_custom_driver_config_t custom_spi_driver;   /*!< Custom SPI driver definitions */
} eth_ksz8851snl_config_t;

/**
 * @brief Default KSZ8851SNL specific configuration
 *
 */
#define ETH_KSZ8851SNL_DEFAULT_CONFIG(spi_host, spi_devcfg_p) \
    {                                               \
        .int_gpio_num = 4,                          \
        .poll_period_ms = 0,                        \
        .spi_host_id = spi_host,                    \
        .spi_devcfg = spi_devcfg_p,                 \
        .custom_spi_driver = ETH_DEFAULT_SPI,       \
    }

/**
* @brief Create KSZ8851SNL Ethernet MAC instance
*
* @param ksz8851snl_config: KSZ8851SNL specific configuration
* @param mac_config: Ethernet MAC configuration
*
* @return
*      - instance: create MAC instance successfully
*      - NULL: create MAC instance failed because some error occurred
*/
esp_eth_mac_t *esp_eth_mac_new_ksz8851snl(const eth_ksz8851snl_config_t *ksz8851snl_config, const eth_mac_config_t *mac_config);
#endif // CONFIG_ETH_SPI_ETHERNET_KSZ8851

#ifdef __cplusplus
}
#endif
