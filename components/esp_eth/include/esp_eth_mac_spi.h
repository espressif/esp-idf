/*
 * SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include "soc/soc_caps.h"
#include "esp_eth_com.h"
#include "esp_eth_mac.h"
#include "driver/spi_master.h"
#include "sdkconfig.h"

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
#ifdef __cplusplus
}
#endif
