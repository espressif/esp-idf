/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/gdma_periph.h"

const gdma_signal_conn_t gdma_periph_signals = {
    .groups = {
        [0] = {
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
                },
                [3] = {
                    .rx_irq_id = ETS_AHB_PDMA_IN_CH3_INTR_SOURCE,
                    .tx_irq_id = ETS_AHB_PDMA_OUT_CH3_INTR_SOURCE,
                },
                [4] = {
                    .rx_irq_id = ETS_AHB_PDMA_IN_CH4_INTR_SOURCE,
                    .tx_irq_id = ETS_AHB_PDMA_OUT_CH4_INTR_SOURCE,
                }
            }
        },
        [1] = {
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

#if SOC_PAU_SUPPORTED && SOC_GDMA_SUPPORT_SLEEP_RETENTION
/* AHB_DMA Channel (Group0, Pair0) Registers Context
   Include: AHB_DMA_MISC_CONF_REG
            AHB_DMA_IN_INT_ENA_CH0_REG / AHB_DMA_OUT_INT_ENA_CH0_REG
            AHB_DMA_IN_CONF0_CH0_REG / AHB_DMA_IN_CONF1_CH0_REG
            AHB_DMA_IN_LINK_CH0_REG / AHB_DMA_IN_LINK_ADDR_CH0_REG
            AHB_DMA_IN_PRI_CH0_REG / AHB_DMA_IN_PERI_SEL_CH0_REG
            AHB_DMA_RX_CH_ARB_WEIGH_CH0_REG / AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH0_REG
            AHB_DMA_OUT_CONF0_CH0_REG / AHB_DMA_OUT_CONF1_CH0_REG
            AHB_DMA_OUT_LINK_CH0_REG / AHB_DMA_OUT_LINK_ADDR_CH0_REG
            AHB_DMA_OUT_PRI_CH0_REG / AHB_DMA_OUT_PERI_SEL_CH0_REG
            AHB_DMA_TX_CH_ARB_WEIGH_CH0_REG / AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH0_REG

            AHB_DMA_INTR_MEM_START_ADDR_REG / AHB_DMA_INTR_MEM_END_ADDR_REG
            AHB_DMA_ARB_TIMEOUT_REG / AHB_DMA_WEIGHT_EN_REG
*/
#define G0P0_RETENTION_REGS_CNT_0 19
#define G0P0_RETENTION_MAP_BASE_0 (DR_REG_AHB_DMA_BASE + 0x8)
#define G0P0_RETENTION_REGS_CNT_1 4
#define G0P0_RETENTION_MAP_BASE_1 (DR_REG_AHB_DMA_BASE + 0x600)
static const uint32_t g0p0_regs_map0[4] = {0x100001, 0xc0000080, 0xc000780c, 0x780c};
static const uint32_t g0p0_regs_map1[4] = {0x17, 0x0, 0x0, 0x0};
static const regdma_entries_config_t ahb_dma_g0p0_regs_retention[] = {
    [0] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_AHB_DMA_LINK(0x00), \
                                            G0P0_RETENTION_MAP_BASE_0, G0P0_RETENTION_MAP_BASE_0, \
                                            G0P0_RETENTION_REGS_CNT_0, 0, 0, \
                                            g0p0_regs_map0[0], g0p0_regs_map0[1], \
                                            g0p0_regs_map0[2], g0p0_regs_map0[3]), \
        .owner = GDMA_RETENTION_ENTRY
    }, \
    [1] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_AHB_DMA_LINK(0x00), \
                                            G0P0_RETENTION_MAP_BASE_1, G0P0_RETENTION_MAP_BASE_1, \
                                            G0P0_RETENTION_REGS_CNT_1, 0, 0, \
                                            g0p0_regs_map1[0], g0p0_regs_map1[1], \
                                            g0p0_regs_map1[2], g0p0_regs_map1[3]), \
        .owner = GDMA_RETENTION_ENTRY
    }, \
};

/* AHB_DMA Channel (Group0, Pair1) Registers Context
   Include: AHB_DMA_MISC_CONF_REG
            AHB_DMA_IN_INT_ENA_CH1_REG / AHB_DMA_OUT_INT_ENA_CH1_REG

            AHB_DMA_IN_CONF0_CH1_REG / AHB_DMA_IN_CONF1_CH1_REG
            AHB_DMA_IN_LINK_CH1_REG / AHB_DMA_IN_LINK_ADDR_CH1_REG
            AHB_DMA_IN_PRI_CH1_REG / AHB_DMA_IN_PERI_SEL_CH1_REG
            AHB_DMA_RX_CH_ARB_WEIGH_CH1_REG / AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH1_REG
            AHB_DMA_OUT_CONF0_CH1_REG / AHB_DMA_OUT_CONF1_CH1_REG
            AHB_DMA_OUT_LINK_CH1_REG / AHB_DMA_OUT_LINK_ADDR_CH1_REG
            AHB_DMA_OUT_PRI_CH1_REG / AHB_DMA_OUT_PERI_SEL_CH1_REG
            AHB_DMA_TX_CH_ARB_WEIGH_CH1_REG / AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH1_REG

            AHB_DMA_INTR_MEM_START_ADDR_REG / AHB_DMA_INTR_MEM_END_ADDR_REG
            AHB_DMA_ARB_TIMEOUT_REG / AHB_DMA_WEIGHT_EN_REG
*/

#define G0P1_RETENTION_REGS_CNT_0 3
#define G0P1_RETENTION_MAP_BASE_0 (DR_REG_AHB_DMA_BASE + 0x18)
#define G0P1_RETENTION_REGS_CNT_1 16
#define G0P1_RETENTION_MAP_BASE_1 (DR_REG_AHB_DMA_BASE + 0x200)
#define G0P1_RETENTION_REGS_CNT_2 4
#define G0P1_RETENTION_MAP_BASE_2 (DR_REG_AHB_DMA_BASE + 0x600)
static const uint32_t g0p1_regs_map0[4] = {0x100001, 0x8, 0x0, 0x0};
static const uint32_t g0p1_regs_map1[4] = {0x1e033, 0x1e033, 0x0, 0x0};
static const uint32_t g0p1_regs_map2[4] = {0x17, 0x0, 0x0, 0x0};
static const regdma_entries_config_t ahb_dma_g0p1_regs_retention[] = {
    [0] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_AHB_DMA_LINK(0x00), \
                                            G0P1_RETENTION_MAP_BASE_0, G0P1_RETENTION_MAP_BASE_0, \
                                            G0P1_RETENTION_REGS_CNT_0, 0, 0, \
                                            g0p1_regs_map0[0], g0p1_regs_map0[1], \
                                            g0p1_regs_map0[2], g0p1_regs_map0[3]), \
        .owner = GDMA_RETENTION_ENTRY
    }, \
    [1] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_AHB_DMA_LINK(0x00), \
                                            G0P1_RETENTION_MAP_BASE_1, G0P1_RETENTION_MAP_BASE_1, \
                                            G0P1_RETENTION_REGS_CNT_1, 0, 0, \
                                            g0p1_regs_map1[0], g0p1_regs_map1[1], \
                                            g0p1_regs_map1[2], g0p1_regs_map1[3]), \
        .owner = GDMA_RETENTION_ENTRY
    }, \
    [2] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_AHB_DMA_LINK(0x00), \
                                            G0P1_RETENTION_MAP_BASE_2, G0P1_RETENTION_MAP_BASE_2, \
                                            G0P1_RETENTION_REGS_CNT_2, 0, 0, \
                                            g0p1_regs_map2[0], g0p1_regs_map2[1], \
                                            g0p1_regs_map2[2], g0p1_regs_map2[3]), \
        .owner = GDMA_RETENTION_ENTRY
    }, \
};

/* AHB_DMA Channel (Group0, Pair2) Registers Context
   Include: AHB_DMA_MISC_CONF_REG
            AHB_DMA_IN_INT_ENA_CH2_REG / AHB_DMA_OUT_INT_ENA_CH2_REG

            AHB_DMA_IN_CONF0_CH2_REG / AHB_DMA_IN_CONF1_CH2_REG
            AHB_DMA_IN_LINK_CH2_REG / AHB_DMA_IN_LINK_ADDR_CH2_REG
            AHB_DMA_IN_PRI_CH2_REG / AHB_DMA_IN_PERI_SEL_CH2_REG
            AHB_DMA_RX_CH_ARB_WEIGH_CH2_REG / AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH2_REG
            AHB_DMA_OUT_CONF0_CH2_REG / AHB_DMA_OUT_CONF1_CH2_REG
            AHB_DMA_OUT_LINK_CH2_REG / AHB_DMA_OUT_LINK_ADDR_CH2_REG
            AHB_DMA_OUT_PRI_CH2_REG / AHB_DMA_OUT_PERI_SEL_CH2_REG
            AHB_DMA_TX_CH_ARB_WEIGH_CH2_REG / AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH2_REG

            AHB_DMA_INTR_MEM_START_ADDR_REG / AHB_DMA_INTR_MEM_END_ADDR_REG
            AHB_DMA_ARB_TIMEOUT_REG / AHB_DMA_WEIGHT_EN_REG
*/
#define G0P2_RETENTION_REGS_CNT_0 3
#define G0P2_RETENTION_MAP_BASE_0 (DR_REG_AHB_DMA_BASE + 0x28)
#define G0P2_RETENTION_REGS_CNT_1 16
#define G0P2_RETENTION_MAP_BASE_1 (DR_REG_AHB_DMA_BASE + 0x300)
#define G0P2_RETENTION_REGS_CNT_2 4
#define G0P2_RETENTION_MAP_BASE_2 (DR_REG_AHB_DMA_BASE + 0x600)
static const uint32_t g0p2_regs_map0[4] = {0x80100001, 0x8, 0x0, 0x0};
static const uint32_t g0p2_regs_map1[4] = {0x1e033, 0x1e033, 0x0, 0x0};
static const uint32_t g0p2_regs_map2[4] = {0x17, 0x0, 0x0, 0x0};
static const regdma_entries_config_t ahb_dma_g0p2_regs_retention[] = {
    [0] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_AHB_DMA_LINK(0x00), \
                                            G0P2_RETENTION_MAP_BASE_0, G0P2_RETENTION_MAP_BASE_0, \
                                            G0P2_RETENTION_REGS_CNT_0, 0, 0, \
                                            g0p2_regs_map0[0], g0p2_regs_map0[1], \
                                            g0p2_regs_map0[2], g0p2_regs_map0[3]), \
        .owner = GDMA_RETENTION_ENTRY
    }, \
    [1] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_AHB_DMA_LINK(0x00), \
                                            G0P2_RETENTION_MAP_BASE_1, G0P2_RETENTION_MAP_BASE_1, \
                                            G0P2_RETENTION_REGS_CNT_1, 0, 0, \
                                            g0p2_regs_map1[0], g0p2_regs_map1[1], \
                                            g0p2_regs_map1[2], g0p2_regs_map1[3]), \
        .owner = GDMA_RETENTION_ENTRY
    }, \
    [2] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_AHB_DMA_LINK(0x00), \
                                            G0P2_RETENTION_MAP_BASE_2, G0P2_RETENTION_MAP_BASE_2, \
                                            G0P2_RETENTION_REGS_CNT_2, 0, 0, \
                                            g0p2_regs_map2[0], g0p2_regs_map2[1], \
                                            g0p2_regs_map2[2], g0p2_regs_map2[3]), \
        .owner = GDMA_RETENTION_ENTRY
    }, \
};

/* AHB_DMA Channel (Group0, Pair3) Registers Context
   Include: AHB_DMA_MISC_CONF_REG
            AHB_DMA_IN_INT_ENA_CH3_REG / AHB_DMA_OUT_INT_ENA_CH3_REG

            AHB_DMA_IN_CONF0_CH3_REG / AHB_DMA_IN_CONF1_CH3_REG
            AHB_DMA_IN_LINK_CH3_REG / AHB_DMA_IN_LINK_ADDR_CH3_REG
            AHB_DMA_IN_PRI_CH3_REG / AHB_DMA_IN_PERI_SEL_CH3_REG
            AHB_DMA_RX_CH_ARB_WEIGH_CH3_REG / AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH3_REG
            AHB_DMA_OUT_CONF0_CH3_REG / AHB_DMA_OUT_CONF1_CH3_REG
            AHB_DMA_OUT_LINK_CH3_REG / AHB_DMA_OUT_LINK_ADDR_CH3_REG
            AHB_DMA_OUT_PRI_CH3_REG / AHB_DMA_OUT_PERI_SEL_CH3_REG
            AHB_DMA_TX_CH_ARB_WEIGH_CH3_REG / AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH3_REG

            AHB_DMA_INTR_MEM_START_ADDR_REG / AHB_DMA_INTR_MEM_END_ADDR_REG
            AHB_DMA_ARB_TIMEOUT_REG / AHB_DMA_WEIGHT_EN_REG
*/
#define G0P3_RETENTION_REGS_CNT_0 3
#define G0P3_RETENTION_MAP_BASE_0 (DR_REG_AHB_DMA_BASE + 0x38)
#define G0P3_RETENTION_REGS_CNT_1 16
#define G0P3_RETENTION_MAP_BASE_1 (DR_REG_AHB_DMA_BASE + 0x400)
#define G0P3_RETENTION_REGS_CNT_2 4
#define G0P3_RETENTION_MAP_BASE_2 (DR_REG_AHB_DMA_BASE + 0x600)
static const uint32_t g0p3_regs_map0[4] = {0x8100001, 0x8, 0x0, 0x0};
static const uint32_t g0p3_regs_map1[4] = {0x1e033, 0x1e033, 0x0, 0x0};
static const uint32_t g0p3_regs_map2[4] = {0x17, 0x0, 0x0, 0x0};
static const regdma_entries_config_t ahb_dma_g0p3_regs_retention[] = {
    [0] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_AHB_DMA_LINK(0x00), \
                                            G0P3_RETENTION_MAP_BASE_0, G0P3_RETENTION_MAP_BASE_0, \
                                            G0P3_RETENTION_REGS_CNT_0, 0, 0, \
                                            g0p3_regs_map0[0], g0p3_regs_map0[1], \
                                            g0p3_regs_map0[2], g0p3_regs_map0[3]), \
        .owner = GDMA_RETENTION_ENTRY
    }, \
    [1] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_AHB_DMA_LINK(0x00), \
                                            G0P3_RETENTION_MAP_BASE_1, G0P3_RETENTION_MAP_BASE_1, \
                                            G0P3_RETENTION_REGS_CNT_1, 0, 0, \
                                            g0p3_regs_map1[0], g0p3_regs_map1[1], \
                                            g0p3_regs_map1[2], g0p3_regs_map1[3]), \
        .owner = GDMA_RETENTION_ENTRY
    }, \
    [2] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_AHB_DMA_LINK(0x00), \
                                            G0P3_RETENTION_MAP_BASE_2, G0P3_RETENTION_MAP_BASE_2, \
                                            G0P3_RETENTION_REGS_CNT_2, 0, 0, \
                                            g0p3_regs_map2[0], g0p3_regs_map2[1], \
                                            g0p3_regs_map2[2], g0p3_regs_map2[3]), \
        .owner = GDMA_RETENTION_ENTRY
    }, \
};

/* AHB_DMA Channel (Group0, Pair4) Registers Context
   Include: AHB_DMA_MISC_CONF_REG
            AHB_DMA_IN_INT_ENA_CH4_REG / AHB_DMA_OUT_INT_ENA_CH4_REG

            AHB_DMA_IN_CONF0_CH4_REG / AHB_DMA_IN_CONF1_CH4_REG
            AHB_DMA_IN_LINK_CH4_REG / AHB_DMA_IN_LINK_ADDR_CH4_REG
            AHB_DMA_IN_PRI_CH4_REG / AHB_DMA_IN_PERI_SEL_CH4_REG
            AHB_DMA_RX_CH_ARB_WEIGH_CH4_REG / AHB_DMA_RX_ARB_WEIGH_OPT_DIR_CH4_REG
            AHB_DMA_OUT_CONF0_CH4_REG / AHB_DMA_OUT_CONF1_CH4_REG
            AHB_DMA_OUT_LINK_CH4_REG / AHB_DMA_OUT_LINK_ADDR_CH4_REG
            AHB_DMA_OUT_PRI_CH4_REG / AHB_DMA_OUT_PERI_SEL_CH4_REG
            AHB_DMA_TX_CH_ARB_WEIGH_CH4_REG / AHB_DMA_TX_ARB_WEIGH_OPT_DIR_CH4_REG
            AHB_DMA_INTR_MEM_START_ADDR_REG / AHB_DMA_INTR_MEM_END_ADDR_REG
            AHB_DMA_ARB_TIMEOUT_REG / AHB_DMA_WEIGHT_EN_REG
*/
#define G0P4_RETENTION_REGS_CNT_0 3
#define G0P4_RETENTION_MAP_BASE_0 (DR_REG_AHB_DMA_BASE + 0x48)
#define G0P4_RETENTION_REGS_CNT_1 20
#define G0P4_RETENTION_MAP_BASE_1 (DR_REG_AHB_DMA_BASE + 0x500)
static const uint32_t g0p4_regs_map0[4] = {0x900001, 0x8, 0x0, 0x0};
static const uint32_t g0p4_regs_map1[4] = {0x1e033, 0x1e033, 0x17, 0x0};
static const regdma_entries_config_t ahb_dma_g0p4_regs_retention[] = {
    [0] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_AHB_DMA_LINK(0x00), \
                                            G0P4_RETENTION_MAP_BASE_0, G0P4_RETENTION_MAP_BASE_0, \
                                            G0P4_RETENTION_REGS_CNT_0, 0, 0, \
                                            g0p4_regs_map0[0], g0p4_regs_map0[1], \
                                            g0p4_regs_map0[2], g0p4_regs_map0[3]), \
        .owner = GDMA_RETENTION_ENTRY
    }, \
    [1] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_AHB_DMA_LINK(0x00), \
                                            G0P4_RETENTION_MAP_BASE_1, G0P4_RETENTION_MAP_BASE_1, \
                                            G0P4_RETENTION_REGS_CNT_1, 0, 0, \
                                            g0p4_regs_map1[0], g0p4_regs_map1[1], \
                                            g0p4_regs_map1[2], g0p4_regs_map1[3]), \
        .owner = GDMA_RETENTION_ENTRY
    }, \
};

/* AXI_DMA Channel (Group1, Pair0) Registers Context
   Include: AXI_DMA_IN_INT_ENA_CH0_REG / AXI_DMA_IN_CONF0_CH0_REG / AXI_DMA_IN_CONF1_CH0_REG / AXI_DMA_IN_LINK1_CH0_REG / AXI_DMA_IN_LINK2_CH0_REG
            AXI_DMA_IN_PRI_CH0_REG / AXI_DMA_IN_PERI_SEL_CH0_REG
            AXI_DMA_OUT_INT_ENA_CH0_REG / AXI_DMA_OUT_CONF0_CH0_REG / AXI_DMA_OUT_CONF1_CH0_REG / AXI_DMA_OUT_LINK1_CH0_REG / AXI_DMA_OUT_LINK2_CH0_REG
            AXI_DMA_OUT_PRI_CH0_REG / AXI_DMA_OUT_PERI_SEL_CH0_REG
            AXI_DMA_ARB_TIMEOUT_REG / AXI_DMA_WEIGHT_EN_REG / AXI_DMA_IN_MEM_CONF_REG
            AXI_DMA_INTR_MEM_START_ADDR_REG / AXI_DMA_INTR_MEM_END_ADDR_REG / AXI_DMA_EXTR_MEM_START_ADDR_REG / AXI_DMA_EXTR_MEM_END_ADDR_REG
            AXI_DMA_MISC_CONF_REG
*/
#define AXI_DMA_G1P0_RETENTION_REGS_CNT_0  14
#define AXI_DMA_G1P0_RETENTION_MAP_BASE_0  AXI_DMA_IN_INT_ENA_CH0_REG
#define AXI_DMA_G1P0_RETENTION_REGS_CNT_1  8
#define AXI_DMA_G1P0_RETENTION_MAP_BASE_1  AXI_DMA_ARB_TIMEOUT_REG
static const uint32_t axi_dma_g1p0_regs_map0[4] = {0xc0cd, 0x0, 0x30334000, 0x0};
static const uint32_t axi_dma_g1p0_regs_map1[4] = {0x407f, 0x0, 0x0, 0x0};
static const regdma_entries_config_t axi_dma_g1p0_regs_retention[] = {
    [0]  = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_GDMA_LINK(0x00), \
                                            AXI_DMA_G1P0_RETENTION_MAP_BASE_0, AXI_DMA_G1P0_RETENTION_MAP_BASE_0, \
                                            AXI_DMA_G1P0_RETENTION_REGS_CNT_0, 0, 0, \
                                            axi_dma_g1p0_regs_map0[0], axi_dma_g1p0_regs_map0[1],   \
                                            axi_dma_g1p0_regs_map0[2], axi_dma_g1p0_regs_map0[3]),  \
        .owner = GDMA_RETENTION_ENTRY
    },
    [1]  = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_GDMA_LINK(0x01), \
                                            AXI_DMA_G1P0_RETENTION_MAP_BASE_1, AXI_DMA_G1P0_RETENTION_MAP_BASE_1, \
                                            AXI_DMA_G1P0_RETENTION_REGS_CNT_1, 0, 0, \
                                            axi_dma_g1p0_regs_map1[0], axi_dma_g1p0_regs_map1[1],   \
                                            axi_dma_g1p0_regs_map1[2], axi_dma_g1p0_regs_map1[3]),  \
        .owner = GDMA_RETENTION_ENTRY
    },
};

/* AXI_DMA Channel (Group1, Pair1) Registers Context
   Include: AXI_DMA_IN_INT_ENA_CH1_REG / AXI_DMA_IN_CONF0_CH1_REG / AXI_DMA_IN_CONF1_CH1_REG / AXI_DMA_IN_LINK1_CH1_REG / AXI_DMA_IN_LINK2_CH1_REG
            AXI_DMA_IN_PRI_CH1_REG / AXI_DMA_IN_PERI_SEL_CH1_REG
            AXI_DMA_OUT_INT_ENA_CH1_REG / AXI_DMA_OUT_CONF0_CH1_REG / AXI_DMA_OUT_CONF1_CH1_REG / AXI_DMA_OUT_LINK1_CH1_REG / AXI_DMA_OUT_LINK2_CH1_REG
            AXI_DMA_OUT_PRI_CH1_REG / AXI_DMA_OUT_PERI_SEL_CH1_REG
            AXI_DMA_ARB_TIMEOUT_REG / AXI_DMA_WEIGHT_EN_REG / AXI_DMA_IN_MEM_CONF_REG
            AXI_DMA_INTR_MEM_START_ADDR_REG / AXI_DMA_INTR_MEM_END_ADDR_REG / AXI_DMA_EXTR_MEM_START_ADDR_REG / AXI_DMA_EXTR_MEM_END_ADDR_REG
            AXI_DMA_MISC_CONF_REG
*/
#define AXI_DMA_G1P1_RETENTION_REGS_CNT_0  14
#define AXI_DMA_G1P1_RETENTION_MAP_BASE_0  AXI_DMA_IN_INT_ENA_CH1_REG
#define AXI_DMA_G1P1_RETENTION_REGS_CNT_1  8
#define AXI_DMA_G1P1_RETENTION_MAP_BASE_1  AXI_DMA_ARB_TIMEOUT_REG
static const uint32_t axi_dma_g1p1_regs_map0[4] = {0xc0cd, 0x0, 0x30334000, 0x0};
static const uint32_t axi_dma_g1p1_regs_map1[4] = {0x407f, 0x0, 0x0, 0x0};
static const regdma_entries_config_t axi_dma_g1p1_regs_retention[] = {
    [0]  = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_GDMA_LINK(0x00), \
                                            AXI_DMA_G1P1_RETENTION_MAP_BASE_0, AXI_DMA_G1P1_RETENTION_MAP_BASE_0, \
                                            AXI_DMA_G1P1_RETENTION_REGS_CNT_0, 0, 0, \
                                            axi_dma_g1p1_regs_map0[0], axi_dma_g1p1_regs_map0[1],   \
                                            axi_dma_g1p1_regs_map0[2], axi_dma_g1p1_regs_map0[3]),  \
        .owner = GDMA_RETENTION_ENTRY
    },
    [1]  = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_GDMA_LINK(0x01), \
                                            AXI_DMA_G1P1_RETENTION_MAP_BASE_1, AXI_DMA_G1P1_RETENTION_MAP_BASE_1, \
                                            AXI_DMA_G1P1_RETENTION_REGS_CNT_1, 0, 0, \
                                            axi_dma_g1p1_regs_map1[0], axi_dma_g1p1_regs_map1[1],   \
                                            axi_dma_g1p1_regs_map1[2], axi_dma_g1p1_regs_map1[3]),  \
        .owner = GDMA_RETENTION_ENTRY
    },
};

/* AXI_DMA Channel (Group1, Pair2) Registers Context
   Include: AXI_DMA_IN_INT_ENA_CH2_REG / AXI_DMA_IN_CONF0_CH2_REG / AXI_DMA_IN_CONF1_CH2_REG / AXI_DMA_IN_LINK1_CH2_REG / AXI_DMA_IN_LINK2_CH2_REG
            AXI_DMA_IN_PRI_CH2_REG / AXI_DMA_IN_PERI_SEL_CH2_REG
            AXI_DMA_OUT_INT_ENA_CH2_REG / AXI_DMA_OUT_CONF0_CH2_REG / AXI_DMA_OUT_CONF1_CH2_REG / AXI_DMA_OUT_LINK1_CH2_REG / AXI_DMA_OUT_LINK2_CH2_REG
            AXI_DMA_OUT_PRI_CH2_REG / AXI_DMA_OUT_PERI_SEL_CH2_REG
            AXI_DMA_ARB_TIMEOUT_REG / AXI_DMA_WEIGHT_EN_REG / AXI_DMA_IN_MEM_CONF_REG
            AXI_DMA_INTR_MEM_START_ADDR_REG / AXI_DMA_INTR_MEM_END_ADDR_REG / AXI_DMA_EXTR_MEM_START_ADDR_REG / AXI_DMA_EXTR_MEM_END_ADDR_REG
            AXI_DMA_MISC_CONF_REG
*/
#define AXI_DMA_G1P2_RETENTION_REGS_CNT_0  14
#define AXI_DMA_G1P2_RETENTION_MAP_BASE_0  AXI_DMA_IN_INT_ENA_CH2_REG
#define AXI_DMA_G1P2_RETENTION_REGS_CNT_1  8
#define AXI_DMA_G1P2_RETENTION_MAP_BASE_1  AXI_DMA_ARB_TIMEOUT_REG
static const uint32_t axi_dma_g1p2_regs_map0[4] = {0xc0cd, 0x0, 0x30334000, 0x0};
static const uint32_t axi_dma_g1p2_regs_map1[4] = {0x407f, 0x0, 0x0, 0x0};
static const regdma_entries_config_t axi_dma_g1p2_regs_retention[] = {
    [0]  = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_GDMA_LINK(0x00), \
                                            AXI_DMA_G1P2_RETENTION_MAP_BASE_0, AXI_DMA_G1P2_RETENTION_MAP_BASE_0, \
                                            AXI_DMA_G1P2_RETENTION_REGS_CNT_0, 0, 0, \
                                            axi_dma_g1p2_regs_map0[0], axi_dma_g1p2_regs_map0[1],   \
                                            axi_dma_g1p2_regs_map0[2], axi_dma_g1p2_regs_map0[3]),  \
        .owner = GDMA_RETENTION_ENTRY
    },
    [1]  = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_GDMA_LINK(0x01), \
                                            AXI_DMA_G1P2_RETENTION_MAP_BASE_1, AXI_DMA_G1P2_RETENTION_MAP_BASE_1, \
                                            AXI_DMA_G1P2_RETENTION_REGS_CNT_1, 0, 0, \
                                            axi_dma_g1p2_regs_map1[0], axi_dma_g1p2_regs_map1[1],   \
                                            axi_dma_g1p2_regs_map1[2], axi_dma_g1p2_regs_map1[3]),  \
        .owner = GDMA_RETENTION_ENTRY
    },
};

const gdma_chx_reg_ctx_link_t gdma_chx_regs_retention[2][5] = {
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
        [3] = {
            ahb_dma_g0p3_regs_retention,
            ARRAY_SIZE(ahb_dma_g0p3_regs_retention),
            SLEEP_RETENTION_MODULE_AHB_DMA_CH3,
        },
        [4] = {
            ahb_dma_g0p4_regs_retention,
            ARRAY_SIZE(ahb_dma_g0p4_regs_retention),
            SLEEP_RETENTION_MODULE_AHB_DMA_CH4,
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
#endif // SOC_PAU_SUPPORTED && SOC_GDMA_SUPPORT_SLEEP_RETENTION
