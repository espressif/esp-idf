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
        .irq = ETS_GSPI2_INTR_SOURCE,
        .irq_dma = -1,
        .hw = &GPSPI2,
        .func = SPI2_FUNC_NUM,
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
            .owner = ENTRY(0) | ENTRY(2) }, \
    /* Additional interrupt setting is required by idf SPI drivers after register recovered */ \
    [1] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_GPSPI_LINK(1), \
                                            SPI_DMA_INT_SET_REG(num), \
                                            SPI_TRANS_DONE_INT_SET | SPI_DMA_SEG_TRANS_DONE_INT_SET , \
                                            UINT32_MAX, 1, 0), \
            .owner = ENTRY(0) | ENTRY(2) }, \
}

static const regdma_entries_config_t spi2_regs_retention[] = SPI_REG_RETENTION_ENTRIES(2);   // '2' for GPSPI2

const spi_reg_retention_info_t spi_reg_retention_info[SOC_SPI_PERIPH_NUM - 1] = {   // '-1' to except mspi
    {
        .module_id = SLEEP_RETENTION_MODULE_GPSPI2,
        .entry_array = spi2_regs_retention,
        .array_size = ARRAY_SIZE(spi2_regs_retention),
    },
};
