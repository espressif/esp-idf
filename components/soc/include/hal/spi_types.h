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

#include "soc/spi_caps.h"
#include "sdkconfig.h"

/**
 * @brief Enum with the three SPI peripherals that are software-accessible in it
 */
typedef enum {
    SPI1_HOST=0,    ///< SPI1
    SPI2_HOST=1,    ///< SPI2
    SPI3_HOST=2,    ///< SPI3
#if SOC_SPI_PERIPH_NUM > 3
    SPI4_HOST=3,    ///< SPI4
#endif
} spi_host_device_t;

//alias for different chips
#ifdef CONFIG_IDF_TARGET_ESP32
#define SPI_HOST    SPI1_HOST
#define HSPI_HOST   SPI2_HOST
#define VSPI_HOST   SPI3_HOST
#elif CONFIG_IDF_TARGET_ESP32S2BETA
#define SPI_HOST    SPI1_HOST
#define FSPI_HOST   SPI2_HOST
#define HSPI_HOST   SPI3_HOST
#define VSPI_HOST   SPI4_HOST
#endif
