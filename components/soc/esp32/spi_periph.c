// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "soc/spi_periph.h"

/*
 Bunch of constants for every SPI peripheral: GPIO signals, irqs, hw addr of registers etc
*/
const spi_signal_conn_t spi_periph_signal[3] = {
    {
        .spiclk_out = SPICLK_OUT_IDX,
        .spiclk_in = SPICLK_IN_IDX,
        .spid_out = SPID_OUT_IDX,
        .spiq_out = SPIQ_OUT_IDX,
        .spiwp_out = SPIWP_OUT_IDX,
        .spihd_out = SPIHD_OUT_IDX,
        .spid_in = SPID_IN_IDX,
        .spiq_in = SPIQ_IN_IDX,
        .spiwp_in = SPIWP_IN_IDX,
        .spihd_in = SPIHD_IN_IDX,
        .spics_out = {SPICS0_OUT_IDX, SPICS1_OUT_IDX, SPICS2_OUT_IDX},
        .spics_in = SPICS0_IN_IDX,
        .spiclk_iomux_pin = SPI_IOMUX_PIN_NUM_CLK,
        .spid_iomux_pin = SPI_IOMUX_PIN_NUM_MOSI,
        .spiq_iomux_pin = SPI_IOMUX_PIN_NUM_MISO,
        .spiwp_iomux_pin = SPI_IOMUX_PIN_NUM_WP,
        .spihd_iomux_pin = SPI_IOMUX_PIN_NUM_HD,
        .spics0_iomux_pin = SPI_IOMUX_PIN_NUM_CS,
        .irq = ETS_SPI1_INTR_SOURCE,
        .irq_dma = ETS_SPI1_DMA_INTR_SOURCE,
        .module = PERIPH_SPI_MODULE,
        .hw = &SPI1
    }, {
        .spiclk_out = HSPICLK_OUT_IDX,
        .spiclk_in = HSPICLK_IN_IDX,
        .spid_out = HSPID_OUT_IDX,
        .spiq_out = HSPIQ_OUT_IDX,
        .spiwp_out = HSPIWP_OUT_IDX,
        .spihd_out = HSPIHD_OUT_IDX,
        .spid_in = HSPID_IN_IDX,
        .spiq_in = HSPIQ_IN_IDX,
        .spiwp_in = HSPIWP_IN_IDX,
        .spihd_in = HSPIHD_IN_IDX,
        .spics_out = {HSPICS0_OUT_IDX, HSPICS1_OUT_IDX, HSPICS2_OUT_IDX},
        .spics_in = HSPICS0_IN_IDX,
        .spiclk_iomux_pin = HSPI_IOMUX_PIN_NUM_CLK,
        .spid_iomux_pin = HSPI_IOMUX_PIN_NUM_MOSI,
        .spiq_iomux_pin = HSPI_IOMUX_PIN_NUM_MISO,
        .spiwp_iomux_pin = HSPI_IOMUX_PIN_NUM_WP,
        .spihd_iomux_pin = HSPI_IOMUX_PIN_NUM_HD,
        .spics0_iomux_pin = HSPI_IOMUX_PIN_NUM_CS,
        .irq = ETS_SPI2_INTR_SOURCE,
        .irq_dma = ETS_SPI2_DMA_INTR_SOURCE,
        .module = PERIPH_HSPI_MODULE,
        .hw = &SPI2
    }, {
        .spiclk_out = VSPICLK_OUT_IDX,
        .spiclk_in = VSPICLK_IN_IDX,
        .spid_out = VSPID_OUT_IDX,
        .spiq_out = VSPIQ_OUT_IDX,
        .spiwp_out = VSPIWP_OUT_IDX,
        .spihd_out = VSPIHD_OUT_IDX,
        .spid_in = VSPID_IN_IDX,
        .spiq_in = VSPIQ_IN_IDX,
        .spiwp_in = VSPIWP_IN_IDX,
        .spihd_in = VSPIHD_IN_IDX,
        .spics_out = {VSPICS0_OUT_IDX, VSPICS1_OUT_IDX, VSPICS2_OUT_IDX},
        .spics_in = VSPICS0_IN_IDX,
        .spiclk_iomux_pin = VSPI_IOMUX_PIN_NUM_CLK,
        .spid_iomux_pin = VSPI_IOMUX_PIN_NUM_MOSI,
        .spiq_iomux_pin = VSPI_IOMUX_PIN_NUM_MISO,
        .spiwp_iomux_pin = VSPI_IOMUX_PIN_NUM_WP,
        .spihd_iomux_pin = VSPI_IOMUX_PIN_NUM_HD,
        .spics0_iomux_pin = VSPI_IOMUX_PIN_NUM_CS,
        .irq = ETS_SPI3_INTR_SOURCE,
        .irq_dma = ETS_SPI3_DMA_INTR_SOURCE,
        .module = PERIPH_VSPI_MODULE,
        .hw = &SPI3
    }
};