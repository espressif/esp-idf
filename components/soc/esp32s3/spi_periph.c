/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
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
        .spid4_out = FSPIIO4_OUT_IDX,
        .spid5_out = FSPIIO5_OUT_IDX,
        .spid6_out = FSPIIO6_OUT_IDX,
        .spid7_out = FSPIIO7_OUT_IDX,
        .spid_in = FSPID_IN_IDX,
        .spiq_in = FSPIQ_IN_IDX,
        .spiwp_in = FSPIWP_IN_IDX,
        .spihd_in = FSPIHD_IN_IDX,
        .spid4_in = FSPIIO4_IN_IDX,
        .spid5_in = FSPIIO5_IN_IDX,
        .spid6_in = FSPIIO6_IN_IDX,
        .spid7_in = FSPIIO7_IN_IDX,
        .spics_out = {FSPICS0_OUT_IDX, FSPICS1_OUT_IDX, FSPICS2_OUT_IDX, FSPICS3_OUT_IDX, FSPICS4_OUT_IDX, FSPICS5_OUT_IDX},
        .spics_in = FSPICS0_IN_IDX,
        .spiclk_iomux_pin = SPI2_IOMUX_PIN_NUM_CLK,
        .spid_iomux_pin = SPI2_IOMUX_PIN_NUM_MOSI,
        .spiq_iomux_pin = SPI2_IOMUX_PIN_NUM_MISO,
        .spiwp_iomux_pin = SPI2_IOMUX_PIN_NUM_WP,
        .spihd_iomux_pin = SPI2_IOMUX_PIN_NUM_HD,
        .spics0_iomux_pin = SPI2_IOMUX_PIN_NUM_CS,
        .irq = ETS_SPI2_INTR_SOURCE,
        .irq_dma = -1,
        .module = PERIPH_SPI2_MODULE,
        .hw = &GPSPI2,
        .func = SPI2_FUNC_NUM,
    }, {
        .spiclk_out = SPI3_CLK_OUT_IDX,
        .spiclk_in = SPI3_CLK_IN_IDX,
        .spid_out = SPI3_D_OUT_IDX,
        .spiq_out = SPI3_Q_OUT_IDX,
        //SPI3 doesn't have wp and hd signals
        .spiwp_out = SPI3_WP_OUT_IDX,
        .spihd_out = SPI3_HD_OUT_IDX,
        .spid_in = SPI3_D_IN_IDX,
        .spiq_in = SPI3_Q_IN_IDX,
        .spiwp_in = SPI3_WP_IN_IDX,
        .spihd_in = SPI3_HD_IN_IDX,
        .spics_out = {SPI3_CS0_OUT_IDX, SPI3_CS1_OUT_IDX, SPI3_CS2_OUT_IDX},
        .spics_in = SPI3_CS0_IN_IDX,
        //SPI3 doesn't have iomux pins
        .spiclk_iomux_pin = -1,
        .spid_iomux_pin = -1,
        .spiq_iomux_pin = -1,
        .spiwp_iomux_pin = -1,
        .spihd_iomux_pin = -1,
        .spics0_iomux_pin = -1,
        .irq = ETS_SPI3_INTR_SOURCE,
        .irq_dma = -1,
        .module = PERIPH_SPI3_MODULE,
        .hw = &GPSPI3,
        .func = -1,
    }
};
