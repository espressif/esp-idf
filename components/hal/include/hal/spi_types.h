// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <stdint.h>
#include "esp_attr.h"
#include "esp_bit_defs.h"
#include "soc/soc_caps.h"
#include "sdkconfig.h"

/**
 * @brief Enum with the three SPI peripherals that are software-accessible in it
 */
typedef enum {
//SPI1 can be used as GPSPI only on ESP32
    SPI1_HOST=0,    ///< SPI1
    SPI2_HOST=1,    ///< SPI2
    SPI3_HOST=2,    ///< SPI3
} spi_host_device_t;

/// SPI Events
typedef enum {
    /* Slave HD Only */
    SPI_EV_BUF_TX         = BIT(0), ///< The buffer has sent data to master.
    SPI_EV_BUF_RX         = BIT(1), ///< The buffer has received data from master.
    SPI_EV_SEND_DMA_READY = BIT(2), ///< Slave has loaded its TX data buffer to the hardware (DMA).
    SPI_EV_SEND           = BIT(3), ///< Master has received certain number of the data, the number is determined by Master.
    SPI_EV_RECV_DMA_READY = BIT(4), ///< Slave has loaded its RX data buffer to the hardware (DMA).
    SPI_EV_RECV           = BIT(5), ///< Slave has received certain number of data from master, the number is determined by Master.
    SPI_EV_CMD9           = BIT(6), ///< Received CMD9 from master.
    SPI_EV_CMDA           = BIT(7), ///< Received CMDA from master.
    /* Common Event */
    SPI_EV_TRANS          = BIT(8), ///< A transaction has done
} spi_event_t;
FLAG_ATTR(spi_event_t)

/**
 * @brief Line mode of SPI transaction phases: CMD, ADDR, DOUT/DIN.
 */
typedef struct {
    uint8_t cmd_lines;    ///< The line width of command phase, e.g. 2-line-cmd-phase.
    uint8_t addr_lines;   ///< The line width of address phase, e.g. 1-line-addr-phase.
    uint8_t data_lines;   ///< The line width of data phase, e.g. 4-line-data-phase.
} spi_line_mode_t;


/** @cond */    //Doxy command to hide preprocessor definitions from docs */

//alias for different chips, deprecated for the chips after esp32s2
#ifdef CONFIG_IDF_TARGET_ESP32
#define SPI_HOST    SPI1_HOST
#define HSPI_HOST   SPI2_HOST
#define VSPI_HOST   SPI3_HOST
#elif CONFIG_IDF_TARGET_ESP32S2
// SPI_HOST (SPI1_HOST) is not supported by the SPI Master and SPI Slave driver on ESP32-S2 and later
#define SPI_HOST    SPI1_HOST
#define FSPI_HOST   SPI2_HOST
#define HSPI_HOST   SPI3_HOST
#endif

/** @endcond */
