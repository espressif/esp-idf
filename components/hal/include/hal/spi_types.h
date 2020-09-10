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

#include "esp_attr.h"
#include <esp_bit_defs.h>
#include "soc/soc_caps.h"
#include "sdkconfig.h"

/**
 * @brief Enum with the three SPI peripherals that are software-accessible in it
 */
typedef enum {
// SPI_HOST (SPI1_HOST) is not supported by the SPI Master and SPI Slave driver on ESP32-S2
    SPI1_HOST=0,    ///< SPI1
    SPI2_HOST=1,    ///< SPI2
    SPI3_HOST=2,    ///< SPI3
} spi_host_device_t;

/// SPI Events
typedef enum {
    SPI_EV_BUF_TX = BIT(0), ///< The buffer has sent data to master, Slave HD only
    SPI_EV_BUF_RX = BIT(1), ///< The buffer has received data from master, Slave HD only
    SPI_EV_SEND =   BIT(2), ///< Has sent data to master through RDDMA, Slave HD only
    SPI_EV_RECV =   BIT(3), ///< Has received data from master through WRDMA, Slave HD only
    SPI_EV_CMD9 =   BIT(4), ///< Received CMD9 from master, Slave HD only
    SPI_EV_CMDA =   BIT(5), ///< Received CMDA from master, Slave HD only
    SPI_EV_TRANS =  BIT(6), ///< A transaction has done
} spi_event_t;
FLAG_ATTR(spi_event_t)


/** @cond */    //Doxy command to hide preprocessor definitions from docs */

//alias for different chips
#ifdef CONFIG_IDF_TARGET_ESP32
#define SPI_HOST    SPI1_HOST
#define HSPI_HOST   SPI2_HOST
#define VSPI_HOST   SPI3_HOST
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
// SPI_HOST (SPI1_HOST) is not supported by the SPI Master and SPI Slave driver on ESP32-S2
#define SPI_HOST    SPI1_HOST
#define FSPI_HOST   SPI2_HOST
#define HSPI_HOST   SPI3_HOST
#endif

/** @endcond */
