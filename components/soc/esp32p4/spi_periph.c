/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/spi_periph.h"
#include "stddef.h"

/*
 Bunch of constants for every SPI peripheral: GPIO signals, irqs, hw addr of registers etc
*/
const spi_signal_conn_t spi_periph_signal[SOC_SPI_PERIPH_NUM] = {
    {
        // MSPI on P4 has dedicated iomux pins
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
        .spiclk_out = SPI2_CK_PAD_OUT_IDX,
        .spiclk_in = SPI2_CK_PAD_IN_IDX,
        .spid_out = SPI2_D_PAD_OUT_IDX,
        .spiq_out = SPI2_Q_PAD_OUT_IDX,
        .spiwp_out = SPI2_WP_PAD_OUT_IDX,
        .spihd_out = SPI2_HOLD_PAD_OUT_IDX,
        .spid4_out = SPI2_IO4_PAD_OUT_IDX,
        .spid5_out = SPI2_IO5_PAD_OUT_IDX,
        .spid6_out = SPI2_IO6_PAD_OUT_IDX,
        .spid7_out = SPI2_IO7_PAD_OUT_IDX,
        .spid_in = SPI2_D_PAD_IN_IDX,
        .spiq_in = SPI2_Q_PAD_IN_IDX,
        .spiwp_in = SPI2_WP_PAD_IN_IDX,
        .spihd_in = SPI2_HOLD_PAD_IN_IDX,
        .spid4_in = SPI2_IO4_PAD_IN_IDX,
        .spid5_in = SPI2_IO5_PAD_IN_IDX,
        .spid6_in = SPI2_IO6_PAD_IN_IDX,
        .spid7_in = SPI2_IO7_PAD_IN_IDX,
        .spics_out = {SPI2_CS_PAD_OUT_IDX, SPI2_CS1_PAD_OUT_IDX, SPI2_CS2_PAD_OUT_IDX, SPI2_CS3_PAD_OUT_IDX, SPI2_CS4_PAD_OUT_IDX, SPI2_CS5_PAD_OUT_IDX},
        .spics_in = SPI2_CS_PAD_IN_IDX,
        .spiclk_iomux_pin = SPI2_IOMUX_PIN_NUM_CLK,
        .spid_iomux_pin = SPI2_IOMUX_PIN_NUM_MOSI,
        .spiq_iomux_pin = SPI2_IOMUX_PIN_NUM_MISO,
        .spiwp_iomux_pin = SPI2_IOMUX_PIN_NUM_WP,
        .spihd_iomux_pin = SPI2_IOMUX_PIN_NUM_HD,
        .spics0_iomux_pin = SPI2_IOMUX_PIN_NUM_CS,
        .irq = ETS_SPI2_INTR_SOURCE,
        .irq_dma = -1,
        .module = PERIPH_GPSPI2_MODULE,
        .hw = &GPSPI2,
        .func = SPI2_FUNC_NUM,
    }, {
        .spiclk_out = SPI3_CK_PAD_OUT_IDX,
        .spiclk_in = SPI3_CK_PAD_IN_IDX,
        .spid_out = SPI3_D_PAD_OUT_IDX,
        .spiq_out = SPI3_QO_PAD_OUT_IDX,
        //SPI3 doesn't have wp and hd signals
        .spiwp_out = SPI3_WP_PAD_OUT_IDX,
        .spihd_out = SPI3_HOLD_PAD_OUT_IDX,
        .spid_in = SPI3_D_PAD_IN_IDX,
        .spiq_in = SPI3_Q_PAD_IN_IDX,
        .spiwp_in = SPI3_WP_PAD_IN_IDX,
        .spihd_in = SPI3_HOLD_PAD_IN_IDX,
        .spics_out = {SPI3_CS_PAD_OUT_IDX, SPI3_CS1_PAD_OUT_IDX, SPI3_CS2_PAD_OUT_IDX},
        .spics_in = SPI3_CS_PAD_IN_IDX,
        //SPI3 doesn't have iomux pins
        .spiclk_iomux_pin = -1,
        .spid_iomux_pin = -1,
        .spiq_iomux_pin = -1,
        .spiwp_iomux_pin = -1,
        .spihd_iomux_pin = -1,
        .spics0_iomux_pin = -1,
        .irq = ETS_SPI3_INTR_SOURCE,
        .irq_dma = -1,
        .module = PERIPH_GPSPI3_MODULE,
        .hw = &GPSPI3,
        .func = -1,
    }
};
