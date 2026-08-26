/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include "esp_private/spi_common_internal.h"
#include "soc/spi_periph.h"

/**
 * Backup registers in Light sleep: (total cnt 29)
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
 * data_buf[0-15]   // slave driver only
 * slave
 * slave1
 */
#define SPI_RETENTION_REGS_CNT 29
static const uint32_t spi_regs_map[4] = {0x31ff, 0x33fffc0, 0x0, 0x0};
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
                                            SPI_TRANS_DONE_INT_SET | SPI_DMA_SEG_TRANS_DONE_INT_SET | SPI_SLV_CMD7_INT_SET | SPI_SLV_CMD8_INT_SET , \
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
