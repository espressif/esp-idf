/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief LP SPI peripheral
 * Since we have just one LP SPI peripheral, we can define it as a uint32_t type for now, instead of an enum.
 */
typedef uint32_t lp_spi_host_t;

/**
 * @brief LP SPI device configuration flags
 */
#define LP_SPI_DEVICE_TXBIT_LSBFIRST    (1<<0)  /*!< Transmit command/address/data LSB first instead of the default MSB first */
#define LP_SPI_DEVICE_RXBIT_LSBFIRST    (1<<1)  /*!< Receive data LSB first instead of the default MSB first */
#define LP_SPI_DEVICE_BIT_LSBFIRST      (LP_SPI_DEVICE_TXBIT_LSBFIRST|LP_SPI_DEVICE_RXBIT_LSBFIRST) /*!< Transmit and receive LSB first */
#define LP_SPI_DEVICE_3WIRE             (1<<2)  /*!< Use MOSI (=spid) for both sending and receiving data */
#define LP_SPI_DEVICE_CS_ACTIVE_HIGH    (1<<3)  /*!< Make CS line active-high during a transanction instead of the default active-low state. Only available in SPI master mode. */
#define LP_SPI_DEVICE_HALF_DUPLEX       (1<<4)  /*!< Transmit data before receiving it, instead of simultaneously. Only available in SPI master mode. */

/**
 * @brief LP SPI bus configuration parameters
 */
typedef struct {
    int mosi_io_num;        /*!< GPIO pin for Master out, Slave In signal, a.k.a, SPI_D. */
    int miso_io_num;        /*!< GPIO pin for Master in, Slave Out signal, a.k.a, SPI_Q. */
    int sclk_io_num;        /*!< GPIO pin for LP SPI Clock signal. */
} lp_spi_bus_config_t;

/**
 * @brief LP SPI device configuration parameters
 */
typedef struct {
    int cs_io_num;          /*!< GPIO pin for the device Chip Select (CS) signal. */
    int clock_speed_hz;     /*!< SPI clock speed in Hz. */
    int spi_mode;           /*!< SPI mode, representing a pair of Clock Polarity (CPOL) and Clock Phase (CPHA) configuration:
                                - SPI Mode 0: (0, 0)
                                - SPI Mode 1: (0, 1)
                                - SPI Mode 2: (1, 0)
                                - SPI Mode 3: (1, 1)
                            */
    int duty_cycle;         /*!< Duty cycle of positive SPI clock, in 1/256th increments (128 = 50% duty cycle). Setting this to 0 (=not setting it) is equivalent to setting this to 128. */
    int flags;              /*!< Bitwise OR of LP_SPI_DEVICE_* flags */
    int cs_ena_pretrans;    /*!< Amount of SPI bit-cycles the CS should be active for, before the transmission (0-16). This only works on half-duplex transactions. */
    int cs_ena_posttrans;   /*!< Amount of SPI bit-cycles the CS should stay active for, after the transmission (0-16). This only works on half-duplex transactions. */
} lp_spi_device_config_t;

/**
 * @brief LP SPI slave configuration parameters
 */
typedef struct {
    int cs_io_num;          /*!< GPIO pin for the device Chip Select (CS) signal. */
    int spi_mode;           /*!< SPI mode, representing a pair of Clock Polarity (CPOL) and Clock Phase (CPHA) configuration:
                                - SPI Mode 0: (0, 0)
                                - SPI Mode 1: (0, 1)
                                - SPI Mode 2: (1, 0)
                                - SPI Mode 3: (1, 1)
                            */
    int flags;              /*!< Bitwise OR of LP_SPI_DEVICE_* flags */
} lp_spi_slave_config_t;

/**
 * @brief Initialize the LP SPI bus for use by the LP core
 *
 * @param host_id       LP SPI host number
 * @param bus_config    LP SPI bus configuration parameters
 *
 * @return esp_err_t    ESP_OK when successful
 *                      ESP_ERR_INVALID_ARG if the configuration is invalid
 */
esp_err_t lp_core_lp_spi_bus_initialize(lp_spi_host_t host_id, const lp_spi_bus_config_t *bus_config);

/**
 * @brief Initialize the LP SPI controller in master mode and add an SPI device to the LP SPI bus.
 *
 * @param host_id       LP SPI host number
 * @param dev_config    LP SPI device configuration parameters
 *
 * @return esp_err_t    ESP_OK when successful
 *                      ESP_ERR_INVALID_ARG if the configuration is invalid
 *                      ESP_FAIL if the device could not be added
 */
esp_err_t lp_core_lp_spi_bus_add_device(lp_spi_host_t host_id, const lp_spi_device_config_t *dev_config);

/**
 * @brief Initialize the LP SPI controller in slave mode
 *
 * @param host_id       LP SPI host number
 * @param slave_config  LP SPI slave configuration parameters
 *
 * @return esp_err_t    ESP_OK when successful
 *                      ESP_FAIL if the SPI controller could not be initialized in slave mode
 */
esp_err_t lp_core_lp_spi_slave_initialize(lp_spi_host_t host_id, const lp_spi_slave_config_t *slave_config);

#ifdef __cplusplus
}
#endif
