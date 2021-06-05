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

#define SPI_FUNC_NUM            0
#define SPI_IOMUX_PIN_NUM_HD    27
#define SPI_IOMUX_PIN_NUM_CS    29
#define SPI_IOMUX_PIN_NUM_MOSI  32
#define SPI_IOMUX_PIN_NUM_CLK   30
#define SPI_IOMUX_PIN_NUM_MISO  31
#define SPI_IOMUX_PIN_NUM_WP    28

#define SPI2_FUNC_NUM           FSPI_FUNC_NUM
#define SPI2_IOMUX_PIN_NUM_HD   FSPI_IOMUX_PIN_NUM_HD
#define SPI2_IOMUX_PIN_NUM_CS   FSPI_IOMUX_PIN_NUM_CS
#define SPI2_IOMUX_PIN_NUM_MOSI FSPI_IOMUX_PIN_NUM_MOSI
#define SPI2_IOMUX_PIN_NUM_CLK  FSPI_IOMUX_PIN_NUM_CLK
#define SPI2_IOMUX_PIN_NUM_MISO FSPI_IOMUX_PIN_NUM_MISO
#define SPI2_IOMUX_PIN_NUM_WP   FSPI_IOMUX_PIN_NUM_WP

//SPI3 has no iomux pins

//Following Macros are deprecated. Please use the Macros above
#define FSPI_FUNC_NUM           4
#define FSPI_IOMUX_PIN_NUM_HD   9
#define FSPI_IOMUX_PIN_NUM_CS   10
#define FSPI_IOMUX_PIN_NUM_MOSI 11
#define FSPI_IOMUX_PIN_NUM_CLK  12
#define FSPI_IOMUX_PIN_NUM_MISO 13
#define FSPI_IOMUX_PIN_NUM_WP   14
