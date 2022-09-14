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

#define SOC_SPI_PERIPH_NUM          3
#define SOC_SPI_DMA_CHAN_NUM        2
#define SOC_SPI_PERIPH_CS_NUM(i)    3
#define SOC_SPI_MAX_CS_NUM          3

#define SPI_FUNC_NUM            1
#define SPI_IOMUX_PIN_NUM_MISO  7
#define SPI_IOMUX_PIN_NUM_MOSI  8
#define SPI_IOMUX_PIN_NUM_CLK   6
#define SPI_IOMUX_PIN_NUM_CS    11
#define SPI_IOMUX_PIN_NUM_WP    10
#define SPI_IOMUX_PIN_NUM_HD    9

#define HSPI_FUNC_NUM           1

//For D2WD and PICO-D4 chip
#define SPI_D2WD_PIN_NUM_MISO  17
#define SPI_D2WD_PIN_NUM_MOSI  8
#define SPI_D2WD_PIN_NUM_CLK   6
#define SPI_D2WD_PIN_NUM_CS    16
#define SPI_D2WD_PIN_NUM_WP    7
#define SPI_D2WD_PIN_NUM_HD    11

#define HSPI_IOMUX_PIN_NUM_MISO 12
#define HSPI_IOMUX_PIN_NUM_MOSI 13
#define HSPI_IOMUX_PIN_NUM_CLK  14
#define HSPI_IOMUX_PIN_NUM_CS   15
#define HSPI_IOMUX_PIN_NUM_WP   2
#define HSPI_IOMUX_PIN_NUM_HD   4

#define VSPI_FUNC_NUM           1
#define VSPI_IOMUX_PIN_NUM_MISO 19
#define VSPI_IOMUX_PIN_NUM_MOSI 23
#define VSPI_IOMUX_PIN_NUM_CLK  18
#define VSPI_IOMUX_PIN_NUM_CS   5
#define VSPI_IOMUX_PIN_NUM_WP   22
#define VSPI_IOMUX_PIN_NUM_HD   21

#define SOC_SPI_MAXIMUM_BUFFER_SIZE     64

#define SOC_SPI_SUPPORT_AS_CS   1   //Support to toggle the CS while the clock toggles

//#define SOC_SPI_SUPPORT_DDRCLK
//#define SOC_SPI_SLAVE_SUPPORT_SEG_TRANS
//#define SOC_SPI_SUPPORT_CD_SIG

// Peripheral supports DIO, DOUT, QIO, or QOUT
#define SOC_SPI_PERIPH_SUPPORT_MULTILINE_MODE(spi_host)          ({(void)spi_host; 1;})

// Peripheral doesn't support output given level during its "dummy phase"
#define SOC_SPI_PERIPH_SUPPORT_CONTROL_DUMMY_OUTPUT(spi_host)    ({(void)spi_host; 0;})
