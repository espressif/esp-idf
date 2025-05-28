/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/gdma_periph.h"
#include "soc/ahb_dma_reg.h"

const gdma_signal_conn_t gdma_periph_signals = {
    .groups = {
        [0] = {
            .pairs = {
                [0]  = {
                    .rx_irq_id = ETS_DMA_IN_CH0_INTR_SOURCE,
                    .tx_irq_id = ETS_DMA_OUT_CH0_INTR_SOURCE,
                },
                [1]  = {
                    .rx_irq_id = ETS_DMA_IN_CH1_INTR_SOURCE,
                    .tx_irq_id = ETS_DMA_OUT_CH1_INTR_SOURCE,
                }
            }
        }
    }
};

/* AHB_DMA Channel (Group0, Pair0) Registers Context
   Include: AHB_DMA_MISC_CONF_REG
            AHB_DMA_IN_INT_ENA_CH0_REG / AHB_DMA_OUT_INT_ENA_CH0_REG / AHB_DMA_IN_PERI_SEL_CH0_REG / AHB_DMA_OUT_PERI_SEL_CH0_REG
            AHB_DMA_IN_CONF0_CH0_REG / AHB_DMA_IN_CONF1_CH0_REG / AHB_DMA_IN_LINK_CH0_REG / AHB_DMA_IN_PRI_CH0_REG
            AHB_DMA_OUT_CONF0_CH0_REG / AHB_DMA_OUT_CONF1_CH0_REG / AHB_DMA_OUT_LINK_CH0_REG / AHB_DMA_OUT_PRI_CH0_REG

            AHB_DMA_TX_CH_ARB_WEIGHT_CH0_REG / AHB_DMA_TX_ARB_WEIGHT_OPT_DIR_CH0_REG
            AHB_DMA_RX_CH_ARB_WEIGHT_CH0_REG / AHB_DMA_RX_ARB_WEIGHT_OPT_DIR_CH0_REG
            AHB_DMA_IN_LINK_ADDR_CH0_REG / AHB_DMA_OUT_LINK_ADDR_CH0_REG
            AHB_DMA_INTR_MEM_START_ADDR_REG / AHB_DMA_INTR_MEM_END_ADDR_REG
            AHB_DMA_ARB_TIMEOUT_TX_REG / AHB_DMA_ARB_TIMEOUT_RX_REG
            AHB_DMA_WEIGHT_EN_TX_REG / AHB_DMA_WEIGHT_EN_RX_REG
*/
#define G0P0_RETENTION_REGS_CNT_0 13
#define G0P0_RETENTION_MAP_BASE_0 AHB_DMA_IN_INT_ENA_CH0_REG
#define G0P0_RETENTION_REGS_CNT_1 12
#define G0P0_RETENTION_MAP_BASE_1 AHB_DMA_TX_CH_ARB_WEIGHT_CH0_REG
static const uint32_t g0p0_regs_map0[4] = {0x4c801001, 0x604c0060, 0x0, 0x0};
static const uint32_t g0p0_regs_map1[4] = {0xc0000003, 0xfc900000, 0x0, 0x0};
static const regdma_entries_config_t gdma_g0p0_regs_retention[] = {
    [0] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_GDMA_LINK(0x00), \
                                            G0P0_RETENTION_MAP_BASE_0, G0P0_RETENTION_MAP_BASE_0, \
                                            G0P0_RETENTION_REGS_CNT_0, 0, 0, \
                                            g0p0_regs_map0[0], g0p0_regs_map0[1], \
                                            g0p0_regs_map0[2], g0p0_regs_map0[3]), \
        .owner = GDMA_RETENTION_ENTRY
    }, \
    [1] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_GDMA_LINK(0x01), \
                                            G0P0_RETENTION_MAP_BASE_1, G0P0_RETENTION_MAP_BASE_1, \
                                            G0P0_RETENTION_REGS_CNT_1, 0, 0, \
                                            g0p0_regs_map1[0], g0p0_regs_map1[1],   \
                                            g0p0_regs_map1[2], g0p0_regs_map1[3]),  \
        .owner = GDMA_RETENTION_ENTRY
    },
};

/* AHB_DMA Channel (Group0, Pair1) Registers Context
   Include: AHB_DMA_MISC_CONF_REG
            AHB_DMA_IN_INT_ENA_CH1_REG / AHB_DMA_OUT_INT_ENA_CH1_REG / AHB_DMA_IN_PERI_SEL_CH1_REG / AHB_DMA_OUT_PERI_SEL_CH1_REG
            AHB_DMA_IN_CONF0_CH1_REG / AHB_DMA_IN_CONF1_CH1_REG / AHB_DMA_IN_LINK_CH1_REG / AHB_DMA_IN_PRI_CH1_REG
            AHB_DMA_OUT_CONF0_CH1_REG / AHB_DMA_OUT_CONF1_CH1_REG / AHB_DMA_OUT_LINK_CH1_REG / AHB_DMA_OUT_PRI_CH1_REG

            AHB_DMA_TX_CH_ARB_WEIGHT_CH1_REG / AHB_DMA_TX_ARB_WEIGHT_OPT_DIR_CH1_REG
            AHB_DMA_RX_CH_ARB_WEIGHT_CH1_REG / AHB_DMA_RX_ARB_WEIGHT_OPT_DIR_CH1_REG
            AHB_DMA_IN_LINK_ADDR_CH1_REG / AHB_DMA_OUT_LINK_ADDR_CH1_REG
            AHB_DMA_INTR_MEM_START_ADDR_REG / AHB_DMA_INTR_MEM_END_ADDR_REG
            AHB_DMA_ARB_TIMEOUT_TX_REG / AHB_DMA_ARB_TIMEOUT_RX_REG
            AHB_DMA_WEIGHT_EN_TX_REG / AHB_DMA_WEIGHT_EN_RX_REG
*/
#define G0P1_RETENTION_REGS_CNT_0  13
#define G0P1_RETENTION_MAP_BASE_0  AHB_DMA_IN_INT_ENA_CH1_REG
#define G0P1_RETENTION_REGS_CNT_1  12
#define G0P1_RETENTION_MAP_BASE_1  AHB_DMA_TX_CH_ARB_WEIGHT_CH1_REG
static const uint32_t g0p1_regs_map0[4] = {0x81001, 0x0, 0xc00604c0, 0x604};
static const uint32_t g0p1_regs_map1[4] = {0xc0000003, 0x3f4800, 0x0, 0x0};
static const regdma_entries_config_t gdma_g0p1_regs_retention[] = {
    [0] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_GDMA_LINK(0x00), \
                                            G0P1_RETENTION_MAP_BASE_0, G0P1_RETENTION_MAP_BASE_0, \
                                            G0P1_RETENTION_REGS_CNT_0, 0, 0, \
                                            g0p1_regs_map0[0], g0p1_regs_map0[1],   \
                                            g0p1_regs_map0[2], g0p1_regs_map0[3]),  \
        .owner = GDMA_RETENTION_ENTRY
    },
    [1] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_GDMA_LINK(0x01), \
                                            G0P1_RETENTION_MAP_BASE_1, G0P1_RETENTION_MAP_BASE_1, \
                                            G0P1_RETENTION_REGS_CNT_1, 0, 0, \
                                            g0p1_regs_map1[0], g0p1_regs_map1[1],   \
                                            g0p1_regs_map1[2], g0p1_regs_map1[3]),  \
        .owner = GDMA_RETENTION_ENTRY
    },
};

const gdma_chx_reg_ctx_link_t gdma_chx_regs_retention[SOC_GDMA_NUM_GROUPS_MAX][SOC_GDMA_PAIRS_PER_GROUP_MAX] = {
    [0] = {
        [0] = {
            gdma_g0p0_regs_retention,
            ARRAY_SIZE(gdma_g0p0_regs_retention),
            SLEEP_RETENTION_MODULE_GDMA_CH0,
        },
        [1] = {
            gdma_g0p1_regs_retention,
            ARRAY_SIZE(gdma_g0p1_regs_retention),
            SLEEP_RETENTION_MODULE_GDMA_CH1,
        },
    }
};
