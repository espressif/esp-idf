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
#define SOC_SPI_DMA_CHAN_NUM        3
#define SOC_SPI_PERIPH_CS_NUM(i)    (((i)==0)? 2: (((i)==1)? 6: 3))
#define SOC_SPI_MAX_CS_NUM          6

#define SPI_FUNC_NUM            0
#define SPI_IOMUX_PIN_NUM_HD    27
#define SPI_IOMUX_PIN_NUM_CS    29
#define SPI_IOMUX_PIN_NUM_MOSI  32
#define SPI_IOMUX_PIN_NUM_CLK   30
#define SPI_IOMUX_PIN_NUM_MISO  31
#define SPI_IOMUX_PIN_NUM_WP    28
//TODO: add the next slot

#define FSPI_FUNC_NUM           4
#define FSPI_IOMUX_PIN_NUM_HD   9
#define FSPI_IOMUX_PIN_NUM_CS   10
#define FSPI_IOMUX_PIN_NUM_MOSI 11
#define FSPI_IOMUX_PIN_NUM_CLK  12
#define FSPI_IOMUX_PIN_NUM_MISO 13
#define FSPI_IOMUX_PIN_NUM_WP   14
//TODO: add the next slot

//HSPI has no iomux pins

#define SOC_SPI_MAXIMUM_BUFFER_SIZE     72

//#define SOC_SPI_SUPPORT_AS_CS         //don't support to toggle the CS while the clock toggles
#define SOC_SPI_SUPPORT_DDRCLK              1
#define SOC_SPI_SLAVE_SUPPORT_SEG_TRANS     1
#define SOC_SPI_SUPPORT_CD_SIG              1
#define SOC_SPI_SUPPORT_CONTINUOUS_TRANS    1


// Peripheral supports DIO, DOUT, QIO, or QOUT
// VSPI (SPI3) only support 1-bit mode
#define SOC_SPI_PERIPH_SUPPORT_MULTILINE_MODE(host_id)          ((host_id) != 2)

// Peripheral supports output given level during its "dummy phase"
// Only SPI1 supports this feature
#define SOC_SPI_PERIPH_SUPPORT_CONTROL_DUMMY_OUTPUT(host_id)    ((host_id) == 0)
