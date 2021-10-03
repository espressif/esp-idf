/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * NOTE: this is not the original header file from the hal component. It is a stripped-down copy to support mocking.
 */

#pragma once

/**
 * @brief Enum with the three SPI peripherals that are software-accessible in it
 */
typedef enum {
// SPI_HOST (SPI1_HOST) is not supported by the SPI Master and SPI Slave driver on ESP32-S2
    SPI1_HOST=0,    ///< SPI1
    SPI2_HOST=1,    ///< SPI2
    SPI3_HOST=2,    ///< SPI3
    SPI_HOST_MAX=3,   ///< invalid host value
} spi_host_device_t;
