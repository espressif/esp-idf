/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include "soc/spi_periph.h"

/*
 Bunch of constants for every SPI peripheral: GPIO signals, irqs, hw addr of registers etc
*/
const spi_signal_conn_t spi_periph_signal[SOC_SPI_PERIPH_NUM] = {
    {
        // MSPI has dedicated iomux pins
        .spiclk_out = -1,
        .spiclk_in = -1,
        .spid_out = -1,
        .spiq_out = -1,
        .spiwp_out = -1,
        .spihd_out = -1,
        .spid_in = -1,
        .spiq_in = -1,
        .spiwp_in = -1,
        .spihd_in = -1,
        .spics_out = {-1},
        .spics_in = -1,
        .spiclk_iomux_pin = -1,
        .spid_iomux_pin = -1,
        .spiq_iomux_pin = -1,
        .spiwp_iomux_pin = -1,
        .spihd_iomux_pin = -1,
        .spics0_iomux_pin = -1,
        .irq = -1,
        .irq_dma = -1,
        .module = -1,
        .hw = NULL,
        .func = -1,
    }, {
        .spiclk_out = FSPICLK_OUT_IDX,
        .spiclk_in = FSPICLK_IN_IDX,
        .spid_out = FSPID_OUT_IDX,
        .spiq_out = FSPIQ_OUT_IDX,
        .spiwp_out = FSPIWP_OUT_IDX,
        .spihd_out = FSPIHD_OUT_IDX,
        .spid_in = FSPID_IN_IDX,
        .spiq_in = FSPIQ_IN_IDX,
        .spiwp_in = FSPIWP_IN_IDX,
        .spihd_in = FSPIHD_IN_IDX,
        .spics_out = {FSPICS0_OUT_IDX, FSPICS1_OUT_IDX, FSPICS2_OUT_IDX, FSPICS3_OUT_IDX, FSPICS4_OUT_IDX, FSPICS5_OUT_IDX},
        .spics_in = FSPICS0_IN_IDX,
        .spiclk_iomux_pin = SPI2_IOMUX_PIN_NUM_CLK,
        .spid_iomux_pin = SPI2_IOMUX_PIN_NUM_MOSI,
        .spiq_iomux_pin = SPI2_IOMUX_PIN_NUM_MISO,
        .spiwp_iomux_pin = SPI2_IOMUX_PIN_NUM_WP,
        .spihd_iomux_pin = SPI2_IOMUX_PIN_NUM_HD,
        .spics0_iomux_pin = SPI2_IOMUX_PIN_NUM_CS,
        .irq = ETS_GSPI2_INTR_SOURCE,
        .irq_dma = -1,
        .module = PERIPH_SPI2_MODULE,
        .hw = &GPSPI2,
        .func = SPI2_FUNC_NUM,
    }
};
