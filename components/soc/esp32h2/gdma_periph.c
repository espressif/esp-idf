/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/gdma_periph.h"
#include "soc/gdma_reg.h"

const gdma_signal_conn_t gdma_periph_signals = {
    .groups = {
        [0] = {
            .module = PERIPH_GDMA_MODULE,
            .pairs = {
                [0]  = {
                    .rx_irq_id = ETS_DMA_IN_CH0_INTR_SOURCE,
                    .tx_irq_id = ETS_DMA_OUT_CH0_INTR_SOURCE,
                },
                [1]  = {
                    .rx_irq_id = ETS_DMA_IN_CH1_INTR_SOURCE,
                    .tx_irq_id = ETS_DMA_OUT_CH1_INTR_SOURCE,
                },
                [2]  = {
                    .rx_irq_id = ETS_DMA_IN_CH2_INTR_SOURCE,
                    .tx_irq_id = ETS_DMA_OUT_CH2_INTR_SOURCE,
                }
            }
        }
    }
};

/* GDMA Channel (Group0, Pair0) Registers Context
   Include: GDMA_MISC_CONF_REG /
            GDMA_IN_INT_ENA_CH0_REG / GDMA_OUT_INT_ENA_CH0_REG / GDMA_IN_PERI_SEL_CH0_REG / GDMA_OUT_PERI_SEL_CH0_REG
            GDMA_IN_CONF0_CH0_REG / GDMA_IN_CONF1_CH0_REG / GDMA_IN_LINK_CH0_REG / GDMA_IN_PRI_CH0_REG
            GDMA_OUT_CONF0_CH0_REG / GDMA_OUT_CONF1_CH0_REG / GDMA_OUT_LINK_CH0_REG /GDMA_OUT_PRI_CH0_REG
*/
#define G0P0_RETENTION_REGS_CNT  13
#define G0P0_RETENTION_MAP_BASE  GDMA_IN_INT_ENA_CH0_REG
static const uint32_t g0p0_regs_map[4] = {0x4C801001, 0x604C0060, 0, 0};
static const regdma_entries_config_t gdma_g0p0_regs_retention[] = {
    [0]  = { .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_GDMA_LINK(0x00), \
                                                G0P0_RETENTION_MAP_BASE, G0P0_RETENTION_MAP_BASE, \
                                                G0P0_RETENTION_REGS_CNT, 0, 0, \
                                                g0p0_regs_map[0], g0p0_regs_map[1],   \
                                                g0p0_regs_map[2], g0p0_regs_map[3]),  \
                                                .owner = GDMA_RETENTION_ENTRY },
};

/* GDMA Channel (Group0, Pair1) Registers Context
   Include: GDMA_MISC_CONF_REG /
            GDMA_IN_INT_ENA_CH1_REG / GDMA_OUT_INT_ENA_CH1_REG / GDMA_IN_PERI_SEL_CH1_REG / GDMA_OUT_PERI_SEL_CH1_REG
            GDMA_IN_CONF0_CH1_REG / GDMA_IN_CONF1_CH1_REG / GDMA_IN_LINK_CH1_REG / GDMA_IN_PRI_CH1_REG
            GDMA_OUT_CONF0_CH1_REG / GDMA_OUT_CONF1_CH1_REG / GDMA_OUT_LINK_CH1_REG /GDMA_OUT_PRI_CH1_REG
*/
#define G0P1_RETENTION_REGS_CNT  13
#define G0P1_RETENTION_MAP_BASE  GDMA_IN_INT_ENA_CH1_REG
static const uint32_t g0p1_regs_map[4] = {0x81001, 0, 0xC00604C0, 0x604};
static const regdma_entries_config_t gdma_g0p1_regs_retention[] = {
    [0]  = { .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_GDMA_LINK(0x00), \
                                                G0P1_RETENTION_MAP_BASE, G0P1_RETENTION_MAP_BASE, \
                                                G0P1_RETENTION_REGS_CNT, 0, 0, \
                                                g0p1_regs_map[0], g0p1_regs_map[1],   \
                                                g0p1_regs_map[2], g0p1_regs_map[3]),  \
                                                .owner = GDMA_RETENTION_ENTRY },
};

/* GDMA Channel (Group0, Pair2) Registers Context
   Include: GDMA_MISC_CONF_REG /
            GDMA_IN_INT_ENA_CH2_REG / GDMA_OUT_INT_ENA_CH2_REG / GDMA_IN_PERI_SEL_CH2_REG / GDMA_OUT_PERI_SEL_CH2_REG
            GDMA_IN_CONF0_CH2_REG / GDMA_IN_CONF1_CH2_REG / GDMA_IN_LINK_CH2_REG / GDMA_IN_PRI_CH2_REG
            GDMA_OUT_CONF0_CH2_REG / GDMA_OUT_CONF1_CH2_REG / GDMA_OUT_LINK_CH2_REG /GDMA_OUT_PRI_CH2_REG
*/
#define G0P2_RETENTION_REGS_CNT_0  6
#define G0P2_RETENTION_MAP_BASE_0  GDMA_IN_INT_ENA_CH2_REG
#define G0P2_RETENTION_REGS_CNT_1  7
#define G0P2_RETENTION_MAP_BASE_1  GDMA_IN_PRI_CH2_REG
static const uint32_t g0p2_regs_map0[4] = {0x9001, 0, 0, 0x4C0000};
static const uint32_t g0p2_regs_map1[4] = {0x3026003, 0, 0, 0};
static const regdma_entries_config_t gdma_g0p2_regs_retention[] = {
    [0]  = { .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_GDMA_LINK(0x00), \
                                                G0P2_RETENTION_MAP_BASE_0, G0P2_RETENTION_MAP_BASE_0, \
                                                G0P2_RETENTION_REGS_CNT_0, 0, 0, \
                                                g0p2_regs_map0[0], g0p2_regs_map0[1],   \
                                                g0p2_regs_map0[2], g0p2_regs_map0[3]),  \
                                                .owner = GDMA_RETENTION_ENTRY },
    [1]  = { .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_GDMA_LINK(0x01), \
                                                G0P2_RETENTION_MAP_BASE_1, G0P2_RETENTION_MAP_BASE_1, \
                                                G0P2_RETENTION_REGS_CNT_1, 0, 0, \
                                                g0p2_regs_map1[0], g0p2_regs_map1[1],   \
                                                g0p2_regs_map1[2], g0p2_regs_map1[3]),  \
                                                .owner = GDMA_RETENTION_ENTRY },
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
        [2] = {
                gdma_g0p2_regs_retention,
                ARRAY_SIZE(gdma_g0p2_regs_retention),
                SLEEP_RETENTION_MODULE_GDMA_CH2,
              }
    }
};
