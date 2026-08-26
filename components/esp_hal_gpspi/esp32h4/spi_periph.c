/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
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
        .irq = ETS_GPSPI2_INTR_SOURCE,
        .irq_dma = -1,
        .hw = &GPSPI2,
        .func = SPI2_FUNC_NUM_QUAD,
    }, {
        .spiclk_out = FSPI3CLK_OUT_IDX,
        .spiclk_in = FSPI3CLK_IN_IDX,
        .spid_out = FSPI3D_OUT_IDX,
        .spiq_out = FSPI3Q_OUT_IDX,
        .spiwp_out = FSPI3WP_OUT_IDX,
        .spihd_out = FSPI3HD_OUT_IDX,
        .spid_in = FSPI3D_IN_IDX,
        .spiq_in = FSPI3Q_IN_IDX,
        .spiwp_in = FSPI3WP_IN_IDX,
        .spihd_in = FSPI3HD_IN_IDX,
        .spics_out = {FSPI3CS0_OUT_IDX, FSPI3CS1_OUT_IDX, FSPI3CS2_OUT_IDX},
        .spics_in = FSPI3CS0_IN_IDX,
        //SPI3 doesn't have iomux pins
        .spiclk_iomux_pin = -1,
        .spid_iomux_pin = -1,
        .spiq_iomux_pin = -1,
        .spiwp_iomux_pin = -1,
        .spihd_iomux_pin = -1,
        .spics0_iomux_pin = -1,
        .irq = ETS_GPSPI3_INTR_SOURCE,
        .irq_dma = -1,
        .hw = &GPSPI3,
        .func = -1,
    }
};
