/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/gdma_periph.h"
#include "soc/ahb_dma_reg.h"
#include "soc/axi_dma_reg.h"

const gdma_signal_conn_t gdma_periph_signals = {
    .groups = {
        [0] = {
            .module = PERIPH_AHB_PDMA_MODULE,
            .pairs = {
                [0] = {
                    .rx_irq_id = ETS_AHB_PDMA_IN_CH0_INTR_SOURCE,
                    .tx_irq_id = ETS_AHB_PDMA_OUT_CH0_INTR_SOURCE,
                },
                [1] = {
                    .rx_irq_id = ETS_AHB_PDMA_IN_CH1_INTR_SOURCE,
                    .tx_irq_id = ETS_AHB_PDMA_OUT_CH1_INTR_SOURCE,
                },
                [2] = {
                    .rx_irq_id = ETS_AHB_PDMA_IN_CH2_INTR_SOURCE,
                    .tx_irq_id = ETS_AHB_PDMA_OUT_CH2_INTR_SOURCE,
                }
            }
        },
        [1] = {
            .module = PERIPH_AXI_PDMA_MODULE,
            .pairs = {
                [0] = {
                    .rx_irq_id = ETS_AXI_PDMA_IN_CH0_INTR_SOURCE,
                    .tx_irq_id = ETS_AXI_PDMA_OUT_CH0_INTR_SOURCE,
                },
                [1] = {
                    .rx_irq_id = ETS_AXI_PDMA_IN_CH1_INTR_SOURCE,
                    .tx_irq_id = ETS_AXI_PDMA_OUT_CH1_INTR_SOURCE,
                },
                [2] = {
                    .rx_irq_id = ETS_AXI_PDMA_IN_CH2_INTR_SOURCE,
                    .tx_irq_id = ETS_AXI_PDMA_OUT_CH2_INTR_SOURCE,
                }
            }
        }
    }
};

/* AHB_DMA Channel (Group0, Pair0) Registers Context
   Include: AHB_DMA_INTR_MEM_START_ADDR_REG / AHB_DMA_INTR_MEM_END_ADDR_REG
            AHB_DMA_ARB_TIMEOUT_TX_REG / AHB_DMA_ARB_TIMEOUT_RX_REG / AHB_DMA_WEIGHT_EN_TX_REG / AHB_DMA_WEIGHT_EN_RX_REG
            AHB_DMA_MISC_CONF_REG /
            AHB_DMA_IN_INT_ENA_CH0_REG / AHB_DMA_OUT_INT_ENA_CH0_REG / AHB_DMA_IN_PERI_SEL_CH0_REG / AHB_DMA_OUT_PERI_SEL_CH0_REG
            AHB_DMA_IN_CONF0_CH0_REG / AHB_DMA_IN_CONF1_CH0_REG / AHB_DMA_IN_LINK_CH0_REG / AHB_DMA_IN_LINK_ADDR_CH0_REG / AHB_DMA_IN_PRI_CH0_REG
            AHB_DMA_OUT_CONF0_CH0_REG / AHB_DMA_OUT_CONF1_CH0_REG / AHB_DMA_OUT_LINK_CH0_REG / AHB_DMA_OUT_LINK_ADDR_CH0_REG / AHB_DMA_OUT_PRI_CH0_REG
            AHB_DMA_TX_CH_ARB_WEIGH_CH0_REG / AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH0_REG
            AHB_DMA_RX_CH_ARB_WEIGH_CH0_REG / AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH0_REG

   Note: CRC functionality is hard to do hardware retention, will consider to use software to do the backup and restore.
*/
#define AHB_DMA_G0P0_RETENTION_REGS_CNT_0  13
#define AHB_DMA_G0P0_RETENTION_MAP_BASE_0  AHB_DMA_IN_INT_ENA_CH0_REG
#define AHB_DMA_G0P0_RETENTION_REGS_CNT_1  12
#define AHB_DMA_G0P0_RETENTION_MAP_BASE_1  AHB_DMA_TX_CH_ARB_WEIGH_CH0_REG
static const uint32_t ahb_dma_g0p0_regs_map0[4] = {0x4c801001, 0x604c0060, 0x0, 0x0};
static const uint32_t ahb_dma_g0p0_regs_map1[4] = {0xc0000003, 0xfc900000, 0x0, 0x0};
static const regdma_entries_config_t ahb_dma_g0p0_regs_retention[] = {
    [0]  = { .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_GDMA_LINK(0x00), \
                                                AHB_DMA_G0P0_RETENTION_MAP_BASE_0, AHB_DMA_G0P0_RETENTION_MAP_BASE_0, \
                                                AHB_DMA_G0P0_RETENTION_REGS_CNT_0, 0, 0, \
                                                ahb_dma_g0p0_regs_map0[0], ahb_dma_g0p0_regs_map0[1],   \
                                                ahb_dma_g0p0_regs_map0[2], ahb_dma_g0p0_regs_map0[3]),  \
             .owner = GDMA_RETENTION_ENTRY },
    [1]  = { .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_GDMA_LINK(0x01), \
                                                AHB_DMA_G0P0_RETENTION_MAP_BASE_1, AHB_DMA_G0P0_RETENTION_MAP_BASE_1, \
                                                AHB_DMA_G0P0_RETENTION_REGS_CNT_1, 0, 0, \
                                                ahb_dma_g0p0_regs_map1[0], ahb_dma_g0p0_regs_map1[1],   \
                                                ahb_dma_g0p0_regs_map1[2], ahb_dma_g0p0_regs_map1[3]),  \
             .owner = GDMA_RETENTION_ENTRY },
};

/* AHB_DMA Channel (Group0, Pair1) Registers Context
   Include: AHB_DMA_INTR_MEM_START_ADDR_REG / AHB_DMA_INTR_MEM_END_ADDR_REG
            AHB_DMA_ARB_TIMEOUT_TX_REG / AHB_DMA_ARB_TIMEOUT_RX_REG / AHB_DMA_WEIGHT_EN_TX_REG / AHB_DMA_WEIGHT_EN_RX_REG
            AHB_DMA_MISC_CONF_REG /
            AHB_DMA_IN_INT_ENA_CH1_REG / AHB_DMA_OUT_INT_ENA_CH1_REG / AHB_DMA_IN_PERI_SEL_CH1_REG / AHB_DMA_OUT_PERI_SEL_CH1_REG
            AHB_DMA_IN_CONF0_CH1_REG / AHB_DMA_IN_CONF1_CH1_REG / AHB_DMA_IN_LINK_CH1_REG / AHB_DMA_IN_LINK_ADDR_CH1_REG / AHB_DMA_IN_PRI_CH1_REG
            AHB_DMA_OUT_CONF0_CH1_REG / AHB_DMA_OUT_CONF1_CH1_REG / AHB_DMA_OUT_LINK_CH1_REG / AHB_DMA_OUT_LINK_ADDR_CH1_REG / AHB_DMA_OUT_PRI_CH1_REG
            AHB_DMA_TX_CH_ARB_WEIGH_CH1_REG / AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH1_REG
            AHB_DMA_RX_CH_ARB_WEIGH_CH1_REG / AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH1_REG

   Note: CRC functionality is hard to do hardware retention, will consider to use software to do the backup and restore.
*/
#define AHB_DMA_G0P1_RETENTION_REGS_CNT_0  13
#define AHB_DMA_G0P1_RETENTION_MAP_BASE_0  AHB_DMA_IN_INT_ENA_CH1_REG
#define AHB_DMA_G0P1_RETENTION_REGS_CNT_1  12
#define AHB_DMA_G0P1_RETENTION_MAP_BASE_1  AHB_DMA_TX_CH_ARB_WEIGH_CH1_REG
static const uint32_t ahb_dma_g0p1_regs_map0[4] = {0x81001, 0, 0xC00604C0, 0x604};
static const uint32_t ahb_dma_g0p1_regs_map1[4] = {0xc0000003, 0x3f4800, 0x0, 0x0};
static const regdma_entries_config_t ahb_dma_g0p1_regs_retention[] = {
    [0]  = { .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_GDMA_LINK(0x00), \
                                                AHB_DMA_G0P1_RETENTION_MAP_BASE_0, AHB_DMA_G0P1_RETENTION_MAP_BASE_0, \
                                                AHB_DMA_G0P1_RETENTION_REGS_CNT_0, 0, 0, \
                                                ahb_dma_g0p1_regs_map0[0], ahb_dma_g0p1_regs_map0[1],   \
                                                ahb_dma_g0p1_regs_map0[2], ahb_dma_g0p1_regs_map0[3]),  \
             .owner = GDMA_RETENTION_ENTRY },
    [1]  = { .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_GDMA_LINK(0x01), \
                                                AHB_DMA_G0P1_RETENTION_MAP_BASE_1, AHB_DMA_G0P1_RETENTION_MAP_BASE_1, \
                                                AHB_DMA_G0P1_RETENTION_REGS_CNT_1, 0, 0, \
                                                ahb_dma_g0p1_regs_map1[0], ahb_dma_g0p1_regs_map1[1],   \
                                                ahb_dma_g0p1_regs_map1[2], ahb_dma_g0p1_regs_map1[3]),  \
             .owner = GDMA_RETENTION_ENTRY },
};

/* AHB_DMA Channel (Group0, Pair2) Registers Context
   Include: AHB_DMA_INTR_MEM_START_ADDR_REG / AHB_DMA_INTR_MEM_END_ADDR_REG
            AHB_DMA_ARB_TIMEOUT_TX_REG / AHB_DMA_ARB_TIMEOUT_RX_REG / AHB_DMA_WEIGHT_EN_TX_REG / AHB_DMA_WEIGHT_EN_RX_REG
            AHB_DMA_MISC_CONF_REG /
            AHB_DMA_IN_INT_ENA_CH2_REG / AHB_DMA_OUT_INT_ENA_CH2_REG / AHB_DMA_IN_PERI_SEL_CH2_REG / AHB_DMA_OUT_PERI_SEL_CH2_REG
            AHB_DMA_IN_CONF0_CH2_REG / AHB_DMA_IN_CONF1_CH2_REG / AHB_DMA_IN_LINK_CH2_REG / AHB_DMA_IN_LINK_ADDR_CH2_REG / AHB_DMA_IN_PRI_CH2_REG
            AHB_DMA_OUT_CONF0_CH2_REG / AHB_DMA_OUT_CONF1_CH2_REG / AHB_DMA_OUT_LINK_CH2_REG / AHB_DMA_OUT_LINK_ADDR_CH2_REG / AHB_DMA_OUT_PRI_CH2_REG
            AHB_DMA_TX_CH_ARB_WEIGH_CH2_REG / AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH2_REG
            AHB_DMA_RX_CH_ARB_WEIGH_CH2_REG / AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH2_REG

   Note: CRC functionality is hard to do hardware retention, will consider to use software to do the backup and restore.
*/
#define AHB_DMA_G0P2_RETENTION_REGS_CNT_0  6
#define AHB_DMA_G0P2_RETENTION_MAP_BASE_0  AHB_DMA_IN_INT_ENA_CH2_REG
#define AHB_DMA_G0P2_RETENTION_REGS_CNT_1  19
#define AHB_DMA_G0P2_RETENTION_MAP_BASE_1  AHB_DMA_IN_PRI_CH2_REG
static const uint32_t ahb_dma_g0p2_regs_map0[4] = {0x9001, 0, 0, 0x4C0000};
static const uint32_t ahb_dma_g0p2_regs_map1[4] = {0x3026003, 0x0, 0x30, 0xfe4c};
static const regdma_entries_config_t ahb_dma_g0p2_regs_retention[] = {
    [0]  = { .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_GDMA_LINK(0x00), \
                                                AHB_DMA_G0P2_RETENTION_MAP_BASE_0, AHB_DMA_G0P2_RETENTION_MAP_BASE_0, \
                                                AHB_DMA_G0P2_RETENTION_REGS_CNT_0, 0, 0, \
                                                ahb_dma_g0p2_regs_map0[0], ahb_dma_g0p2_regs_map0[1],   \
                                                ahb_dma_g0p2_regs_map0[2], ahb_dma_g0p2_regs_map0[3]),  \
             .owner = GDMA_RETENTION_ENTRY },
    [1]  = { .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_GDMA_LINK(0x01), \
                                                AHB_DMA_G0P2_RETENTION_MAP_BASE_1, AHB_DMA_G0P2_RETENTION_MAP_BASE_1, \
                                                AHB_DMA_G0P2_RETENTION_REGS_CNT_1, 0, 0, \
                                                ahb_dma_g0p2_regs_map1[0], ahb_dma_g0p2_regs_map1[1],   \
                                                ahb_dma_g0p2_regs_map1[2], ahb_dma_g0p2_regs_map1[3]),  \
             .owner = GDMA_RETENTION_ENTRY },
};

/* AXI_DMA Channel (Group1, Pair0) Registers Context
   Include: AXI_DMA_IN_INT_ENA_CH0_REG / AXI_DMA_IN_CONF0_CH0_REG / AXI_DMA_IN_CONF1_CH0_REG / AXI_DMA_IN_LINK1_CH0_REG / AXI_DMA_IN_LINK2_CH0_REG
            AXI_DMA_IN_PRI_CH0_REG / AXI_DMA_IN_PERI_SEL_CH0_REG
            AXI_DMA_OUT_INT_ENA_CH0_REG / AXI_DMA_OUT_CONF0_CH0_REG / AXI_DMA_OUT_CONF1_CH0_REG / AXI_DMA_OUT_LINK1_CH0_REG / AXI_DMA_OUT_LINK2_CH0_REG
            AXI_DMA_OUT_PRI_CH0_REG / AXI_DMA_OUT_PERI_SEL_CH0_REG
            AXI_DMA_ARB_TIMEOUT_REG / AXI_DMA_WEIGHT_EN_REG / AXI_DMA_IN_MEM_CONF_REG
            AXI_DMA_INTR_MEM_START_ADDR_REG / AXI_DMA_INTR_MEM_END_ADDR_REG / AXI_DMA_EXTR_MEM_START_ADDR_REG / AXI_DMA_EXTR_MEM_END_ADDR_REG
            AXI_DMA_MISC_CONF_REG

   Note: CRC functionality is hard to do hardware retention, will consider to use software to do the backup and restore.
*/
#define AXI_DMA_G1P0_RETENTION_REGS_CNT_0  14
#define AXI_DMA_G1P0_RETENTION_MAP_BASE_0  AXI_DMA_IN_INT_ENA_CH0_REG
#define AXI_DMA_G1P0_RETENTION_REGS_CNT_1  8
#define AXI_DMA_G1P0_RETENTION_MAP_BASE_1  AXI_DMA_ARB_TIMEOUT_REG
static const uint32_t axi_dma_g1p0_regs_map0[4] = {0xc0cd, 0x0, 0x30334000, 0x0};
static const uint32_t axi_dma_g1p0_regs_map1[4] = {0x407f, 0x0, 0x0, 0x0};
static const regdma_entries_config_t axi_dma_g1p0_regs_retention[] = {
    [0]  = { .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_GDMA_LINK(0x00), \
                                                AXI_DMA_G1P0_RETENTION_MAP_BASE_0, AXI_DMA_G1P0_RETENTION_MAP_BASE_0, \
                                                AXI_DMA_G1P0_RETENTION_REGS_CNT_0, 0, 0, \
                                                axi_dma_g1p0_regs_map0[0], axi_dma_g1p0_regs_map0[1],   \
                                                axi_dma_g1p0_regs_map0[2], axi_dma_g1p0_regs_map0[3]),  \
             .owner = GDMA_RETENTION_ENTRY },
    [1]  = { .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_GDMA_LINK(0x01), \
                                                AXI_DMA_G1P0_RETENTION_MAP_BASE_1, AXI_DMA_G1P0_RETENTION_MAP_BASE_1, \
                                                AXI_DMA_G1P0_RETENTION_REGS_CNT_1, 0, 0, \
                                                axi_dma_g1p0_regs_map1[0], axi_dma_g1p0_regs_map1[1],   \
                                                axi_dma_g1p0_regs_map1[2], axi_dma_g1p0_regs_map1[3]),  \
             .owner = GDMA_RETENTION_ENTRY},
};

/* AXI_DMA Channel (Group1, Pair1) Registers Context
   Include: AXI_DMA_IN_INT_ENA_CH1_REG / AXI_DMA_IN_CONF0_CH1_REG / AXI_DMA_IN_CONF1_CH1_REG / AXI_DMA_IN_LINK1_CH1_REG / AXI_DMA_IN_LINK2_CH1_REG
            AXI_DMA_IN_PRI_CH1_REG / AXI_DMA_IN_PERI_SEL_CH1_REG
            AXI_DMA_OUT_INT_ENA_CH1_REG / AXI_DMA_OUT_CONF0_CH1_REG / AXI_DMA_OUT_CONF1_CH1_REG / AXI_DMA_OUT_LINK1_CH1_REG / AXI_DMA_OUT_LINK2_CH1_REG
            AXI_DMA_OUT_PRI_CH1_REG / AXI_DMA_OUT_PERI_SEL_CH1_REG
            AXI_DMA_ARB_TIMEOUT_REG / AXI_DMA_WEIGHT_EN_REG / AXI_DMA_IN_MEM_CONF_REG
            AXI_DMA_INTR_MEM_START_ADDR_REG / AXI_DMA_INTR_MEM_END_ADDR_REG / AXI_DMA_EXTR_MEM_START_ADDR_REG / AXI_DMA_EXTR_MEM_END_ADDR_REG
            AXI_DMA_MISC_CONF_REG

   Note: CRC functionality is hard to do hardware retention, will consider to use software to do the backup and restore.
*/
#define AXI_DMA_G1P1_RETENTION_REGS_CNT_0  14
#define AXI_DMA_G1P1_RETENTION_MAP_BASE_0  AXI_DMA_IN_INT_ENA_CH1_REG
#define AXI_DMA_G1P1_RETENTION_REGS_CNT_1  8
#define AXI_DMA_G1P1_RETENTION_MAP_BASE_1  AXI_DMA_ARB_TIMEOUT_REG
static const uint32_t axi_dma_g1p1_regs_map0[4] = {0xc0cd, 0x0, 0x30334000, 0x0};
static const uint32_t axi_dma_g1p1_regs_map1[4] = {0x407f, 0x0, 0x0, 0x0};
static const regdma_entries_config_t axi_dma_g1p1_regs_retention[] = {
    [0]  = { .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_GDMA_LINK(0x00), \
                                                AXI_DMA_G1P1_RETENTION_MAP_BASE_0, AXI_DMA_G1P1_RETENTION_MAP_BASE_0, \
                                                AXI_DMA_G1P1_RETENTION_REGS_CNT_0, 0, 0, \
                                                axi_dma_g1p1_regs_map0[0], axi_dma_g1p1_regs_map0[1],   \
                                                axi_dma_g1p1_regs_map0[2], axi_dma_g1p1_regs_map0[3]),  \
             .owner = GDMA_RETENTION_ENTRY },
    [1]  = { .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_GDMA_LINK(0x01), \
                                                AXI_DMA_G1P1_RETENTION_MAP_BASE_1, AXI_DMA_G1P1_RETENTION_MAP_BASE_1, \
                                                AXI_DMA_G1P1_RETENTION_REGS_CNT_1, 0, 0, \
                                                axi_dma_g1p1_regs_map1[0], axi_dma_g1p1_regs_map1[1],   \
                                                axi_dma_g1p1_regs_map1[2], axi_dma_g1p1_regs_map1[3]),  \
             .owner = GDMA_RETENTION_ENTRY },
};

/* AXI_DMA Channel (Group1, Pair2) Registers Context
   Include: AXI_DMA_IN_INT_ENA_CH2_REG / AXI_DMA_IN_CONF0_CH2_REG / AXI_DMA_IN_CONF1_CH2_REG / AXI_DMA_IN_LINK1_CH2_REG / AXI_DMA_IN_LINK2_CH2_REG
            AXI_DMA_IN_PRI_CH2_REG / AXI_DMA_IN_PERI_SEL_CH2_REG
            AXI_DMA_OUT_INT_ENA_CH2_REG / AXI_DMA_OUT_CONF0_CH2_REG / AXI_DMA_OUT_CONF1_CH2_REG / AXI_DMA_OUT_LINK1_CH2_REG / AXI_DMA_OUT_LINK2_CH2_REG
            AXI_DMA_OUT_PRI_CH2_REG / AXI_DMA_OUT_PERI_SEL_CH2_REG
            AXI_DMA_ARB_TIMEOUT_REG / AXI_DMA_WEIGHT_EN_REG / AXI_DMA_IN_MEM_CONF_REG
            AXI_DMA_INTR_MEM_START_ADDR_REG / AXI_DMA_INTR_MEM_END_ADDR_REG / AXI_DMA_EXTR_MEM_START_ADDR_REG / AXI_DMA_EXTR_MEM_END_ADDR_REG
            AXI_DMA_MISC_CONF_REG

   Note: CRC functionality is hard to do hardware retention, will consider to use software to do the backup and restore.
*/
#define AXI_DMA_G1P2_RETENTION_REGS_CNT_0  14
#define AXI_DMA_G1P2_RETENTION_MAP_BASE_0  AXI_DMA_IN_INT_ENA_CH2_REG
#define AXI_DMA_G1P2_RETENTION_REGS_CNT_1  8
#define AXI_DMA_G1P2_RETENTION_MAP_BASE_1  AXI_DMA_ARB_TIMEOUT_REG
static const uint32_t axi_dma_g1p2_regs_map0[4] = {0xc0cd, 0x0, 0x30334000, 0x0};
static const uint32_t axi_dma_g1p2_regs_map1[4] = {0x407f, 0x0, 0x0, 0x0};
static const regdma_entries_config_t axi_dma_g1p2_regs_retention[] = {
    [0]  = { .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_GDMA_LINK(0x00), \
                                                AXI_DMA_G1P2_RETENTION_MAP_BASE_0, AXI_DMA_G1P2_RETENTION_MAP_BASE_0, \
                                                AXI_DMA_G1P2_RETENTION_REGS_CNT_0, 0, 0, \
                                                axi_dma_g1p2_regs_map0[0], axi_dma_g1p2_regs_map0[1],   \
                                                axi_dma_g1p2_regs_map0[2], axi_dma_g1p2_regs_map0[3]),  \
             .owner = GDMA_RETENTION_ENTRY },
    [1]  = { .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_GDMA_LINK(0x01), \
                                                AXI_DMA_G1P2_RETENTION_MAP_BASE_1, AXI_DMA_G1P2_RETENTION_MAP_BASE_1, \
                                                AXI_DMA_G1P2_RETENTION_REGS_CNT_1, 0, 0, \
                                                axi_dma_g1p2_regs_map1[0], axi_dma_g1p2_regs_map1[1],   \
                                                axi_dma_g1p2_regs_map1[2], axi_dma_g1p2_regs_map1[3]),  \
             .owner = GDMA_RETENTION_ENTRY },
};

const gdma_chx_reg_ctx_link_t gdma_chx_regs_retention[SOC_GDMA_NUM_GROUPS_MAX][SOC_GDMA_PAIRS_PER_GROUP_MAX] = {
    [0] = {
        [0] = {
                ahb_dma_g0p0_regs_retention,
                ARRAY_SIZE(ahb_dma_g0p0_regs_retention),
                SLEEP_RETENTION_MODULE_AHB_DMA_CH0,
              },
        [1] = {
                ahb_dma_g0p1_regs_retention,
                ARRAY_SIZE(ahb_dma_g0p1_regs_retention),
                SLEEP_RETENTION_MODULE_AHB_DMA_CH1,
               },
        [2] = {
                ahb_dma_g0p2_regs_retention,
                ARRAY_SIZE(ahb_dma_g0p2_regs_retention),
                SLEEP_RETENTION_MODULE_AHB_DMA_CH2,
               },
    },
    [1] = {
        [0] = {
                axi_dma_g1p0_regs_retention,
                ARRAY_SIZE(axi_dma_g1p0_regs_retention),
                SLEEP_RETENTION_MODULE_AXI_DMA_CH0,
              },
        [1] = {
                axi_dma_g1p1_regs_retention,
                ARRAY_SIZE(axi_dma_g1p1_regs_retention),
                SLEEP_RETENTION_MODULE_AXI_DMA_CH1,
              },
        [2] = {
                axi_dma_g1p2_regs_retention,
                ARRAY_SIZE(axi_dma_g1p2_regs_retention),
                SLEEP_RETENTION_MODULE_AXI_DMA_CH2,
              },
    }
};
