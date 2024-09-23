/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
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
        // MSPI on P4 has dedicated iomux pins
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
        .hw = &GPSPI2,
        .func = SPI2_FUNC_NUM,
    }, {
        .spiclk_out = SPI3_CK_PAD_OUT_IDX,
        .spiclk_in = SPI3_CK_PAD_IN_IDX,
        .spid_out = SPI3_D_PAD_OUT_IDX,
        .spiq_out = SPI3_QO_PAD_OUT_IDX,
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
        .hw = &GPSPI3,
        .func = -1,
    }
};

/**
 * Backup registers in Light sleep: (total cnt 12)
 *
 * cmd
 * addr
 * ctrl
 * clock
 * user
 * user1
 * user2
 * ms_dlen
 * misc
 * dma_conf
 * dma_int_ena
 * slave
 */
#define SPI_RETENTION_REGS_CNT 12
static const uint32_t spi_regs_map[4] = {0x31ff, 0x1000000, 0x0, 0x0};
#define SPI_REG_RETENTION_ENTRIES(num) { \
    [0] = { .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_GPSPI_LINK(0), \
                                               REG_SPI_BASE(num), REG_SPI_BASE(num), \
                                               SPI_RETENTION_REGS_CNT, 0, 0, \
                                               spi_regs_map[0], spi_regs_map[1], \
                                               spi_regs_map[2], spi_regs_map[3]), \
            .owner = ENTRY(0) }, \
    /* Additional interrupt setting is required by idf SPI drivers after register recovered */ \
    [1] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_GPSPI_LINK(1), \
                                            SPI_DMA_INT_SET_REG(num), \
                                            SPI_TRANS_DONE_INT_SET | SPI_DMA_SEG_TRANS_DONE_INT_SET , \
                                            UINT32_MAX, 1, 0), \
            .owner = ENTRY(0) }, \
}

static const regdma_entries_config_t spi2_regs_retention[] = SPI_REG_RETENTION_ENTRIES(2);   // '2' for GPSPI2
static const regdma_entries_config_t spi3_regs_retention[] = SPI_REG_RETENTION_ENTRIES(3);

const spi_reg_retention_info_t spi_reg_retention_info[SOC_SPI_PERIPH_NUM - 1] = {   // '-1' to except mspi
    {
        .module_id = SLEEP_RETENTION_MODULE_GPSPI2,
        .entry_array = spi2_regs_retention,
        .array_size = ARRAY_SIZE(spi2_regs_retention),
    },
    {
        .module_id = SLEEP_RETENTION_MODULE_GPSPI3,
        .entry_array = spi3_regs_retention,
        .array_size = ARRAY_SIZE(spi3_regs_retention),
    },
};
