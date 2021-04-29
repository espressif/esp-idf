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

#define SPI_FUNC_NUM            1
#define SPI_IOMUX_PIN_NUM_MISO  7
#define SPI_IOMUX_PIN_NUM_MOSI  8
#define SPI_IOMUX_PIN_NUM_CLK   6
#define SPI_IOMUX_PIN_NUM_CS    11
#define SPI_IOMUX_PIN_NUM_WP    10
#define SPI_IOMUX_PIN_NUM_HD    9

//For D2WD and PICO-D4 chip
#define SPI_D2WD_PIN_NUM_MISO  17
#define SPI_D2WD_PIN_NUM_MOSI  8
#define SPI_D2WD_PIN_NUM_CLK   6
#define SPI_D2WD_PIN_NUM_CS    16
#define SPI_D2WD_PIN_NUM_WP    7
#define SPI_D2WD_PIN_NUM_HD    11

#define SPI2_FUNC_NUM           HSPI_FUNC_NUM
#define SPI2_IOMUX_PIN_NUM_MISO HSPI_IOMUX_PIN_NUM_MISO
#define SPI2_IOMUX_PIN_NUM_MOSI HSPI_IOMUX_PIN_NUM_MOSI
#define SPI2_IOMUX_PIN_NUM_CLK  HSPI_IOMUX_PIN_NUM_CLK
#define SPI2_IOMUX_PIN_NUM_CS   HSPI_IOMUX_PIN_NUM_CS
#define SPI2_IOMUX_PIN_NUM_WP   HSPI_IOMUX_PIN_NUM_WP
#define SPI2_IOMUX_PIN_NUM_HD   HSPI_IOMUX_PIN_NUM_HD

#define SPI3_FUNC_NUM           VSPI_FUNC_NUM
#define SPI3_IOMUX_PIN_NUM_MISO VSPI_IOMUX_PIN_NUM_MISO
#define SPI3_IOMUX_PIN_NUM_MOSI VSPI_IOMUX_PIN_NUM_MOSI
#define SPI3_IOMUX_PIN_NUM_CLK  VSPI_IOMUX_PIN_NUM_CLK
#define SPI3_IOMUX_PIN_NUM_CS   VSPI_IOMUX_PIN_NUM_CS
#define SPI3_IOMUX_PIN_NUM_WP   VSPI_IOMUX_PIN_NUM_WP
#define SPI3_IOMUX_PIN_NUM_HD   VSPI_IOMUX_PIN_NUM_HD

//Following Macros are deprecated. Please use the Macros above
#define HSPI_FUNC_NUM           1
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
