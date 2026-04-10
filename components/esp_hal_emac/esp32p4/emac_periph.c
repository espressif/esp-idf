/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "hal/emac_periph.h"
#include "hal/config.h"
#include "soc/io_mux_reg.h"
#if __has_include("soc/emac_reg.h")
#include "soc/emac_reg.h"
#endif

/**
 * In emac_periph terms, `SIG_GPIO_OUT_IDX` indicates that the EMAC signal cannot be connected
 * via the GPIO Matrix (i.e. such connection doesn't exist for the function), so it should
 * stay in the default "unconnected state".
 *
 * Note: `SIG_GPIO_OUT_IDX` is defined for all targets and is usually used to signify "disconnect
 * from peripheral signal" (a default unconnected peripheral state).
 */

const emac_io_info_t emac_io_idx = {
    .mdc_idx = MII_MDC_PAD_OUT_IDX,
    .mdo_idx = MII_MDO_PAD_OUT_IDX,
    .mdi_idx = MII_MDI_PAD_IN_IDX,
    .mii_tx_clk_i_idx = EMAC_TX_CLK_PAD_IN_IDX,
    .mii_tx_en_o_idx = EMAC_PHY_TXEN_PAD_OUT_IDX,
    .mii_txd0_o_idx = EMAC_PHY_TXD0_PAD_OUT_IDX,
    .mii_txd1_o_idx = EMAC_PHY_TXD1_PAD_OUT_IDX,
    .mii_txd2_o_idx = EMAC_PHY_TXD2_PAD_OUT_IDX,
    .mii_txd3_o_idx = EMAC_PHY_TXD3_PAD_OUT_IDX,
    .mii_rx_clk_i_idx = EMAC_RX_CLK_PAD_IN_IDX,
    .mii_rx_dv_i_idx = EMAC_PHY_RXDV_PAD_IN_IDX,
    .mii_rxd0_i_idx = EMAC_PHY_RXD0_PAD_IN_IDX,
    .mii_rxd1_i_idx = EMAC_PHY_RXD1_PAD_IN_IDX,
    .mii_rxd2_i_idx = EMAC_PHY_RXD2_PAD_IN_IDX,
    .mii_rxd3_i_idx = EMAC_PHY_RXD3_PAD_IN_IDX,
    .mii_col_i_idx = EMAC_PHY_COL_PAD_IN_IDX,
    .mii_crs_i_idx = EMAC_PHY_CRS_PAD_IN_IDX,
    .mii_tx_er_o_idx = EMAC_PHY_TXER_PAD_OUT_IDX,
    .mii_rx_er_i_idx = EMAC_PHY_RXER_PAD_IN_IDX,
    .rmii_refclk_i_idx = SIG_GPIO_OUT_IDX,
    .rmii_refclk_o_idx = SIG_GPIO_OUT_IDX,
#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300
    .ptp_pps_idx = EMAC_PTP_PPS_PAD_OUT_IDX,
#else
    .ptp_pps_idx = SIG_GPIO_OUT_IDX,
#endif
};

static const emac_iomux_info_t emac_rmii_iomux_clki[] = {
    [0] = {
        .gpio_num = 32,
        .func = FUNC_GPIO32_EMAC_RMII_CLK_PAD,
    },
    [1] = {
        .gpio_num = 44,
        .func = FUNC_GPIO44_EMAC_RMII_CLK_PAD,
    },
    [2] = {
        .gpio_num = 50,
        .func = FUNC_GPIO50_EMAC_RMII_CLK_PAD,
    },
    [3] = {
        .gpio_num = GPIO_NUM_MAX, // indicates end of list
    }
};

static const emac_iomux_info_t emac_rmii_iomux_clko[] = {
    [0] = {
        .gpio_num = 23,
        .func = FUNC_GPIO23_REF_50M_CLK_PAD,
    },
    [1] = {
        .gpio_num = 39,
        .func = FUNC_GPIO39_REF_50M_CLK_PAD,
    },
    [2] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rmii_iomux_tx_en[] = {
    [0] = {
        .gpio_num = 33,
        .func = FUNC_GPIO33_EMAC_PHY_TXEN_PAD,
    },
    [1] = {
        .gpio_num = 40,
        .func = FUNC_GPIO40_EMAC_PHY_TXEN_PAD,
    },
    [2] = {
        .gpio_num = 49,
        .func = FUNC_GPIO40_EMAC_PHY_TXEN_PAD,
    },
    [3] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rmii_iomux_txd0[] = {
    [0] = {
        .gpio_num = 34,
        .func = FUNC_GPIO34_EMAC_PHY_TXD0_PAD,
    },
    [1] = {
        .gpio_num = 41,
        .func = FUNC_GPIO41_EMAC_PHY_TXD0_PAD,
    },
    [2] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rmii_iomux_txd1[] = {
    [0] = {
        .gpio_num = 35,
        .func = FUNC_GPIO35_EMAC_PHY_TXD1_PAD,
    },
    [1] = {
        .gpio_num = 42,
        .func = FUNC_GPIO42_EMAC_PHY_TXD1_PAD,
    },
    [2] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rmii_iomux_crs_dv[] = {
    [0] = {
        .gpio_num = 28,
        .func = FUNC_GPIO28_EMAC_PHY_RXDV_PAD,
    },
    [1] = {
        .gpio_num = 45,
        .func = FUNC_GPIO45_EMAC_PHY_RXDV_PAD,
    },
    [2] = {
        .gpio_num = 51,
        .func = FUNC_GPIO51_EMAC_PHY_RXDV_PAD,
    },
    [3] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rmii_iomux_rxd0[] = {
    [0] = {
        .gpio_num = 29,
        .func = FUNC_GPIO29_EMAC_PHY_RXD0_PAD,
    },
    [1] = {
        .gpio_num = 46,
        .func = FUNC_GPIO46_EMAC_PHY_RXD0_PAD,
    },
    [2] = {
        .gpio_num = 52,
        .func = FUNC_GPIO52_EMAC_PHY_RXD0_PAD,
    },
    [3] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rmii_iomux_rxd1[] = {
    [0] = {
        .gpio_num = 30,
        .func = FUNC_GPIO30_EMAC_PHY_RXD1_PAD,
    },
    [1] = {
        .gpio_num = 47,
        .func = FUNC_GPIO47_EMAC_PHY_RXD1_PAD,
    },
    [2] = {
        .gpio_num = 53,
        .func = FUNC_GPIO53_EMAC_PHY_RXD1_PAD,
    },
    [3] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rmii_iomux_tx_er[] = {
    [0] = {
        .gpio_num = 36,
        .func = FUNC_GPIO36_EMAC_PHY_TXER_PAD,
    },
    [1] = {
        .gpio_num = 43,
        .func = FUNC_GPIO43_EMAC_PHY_TXER_PAD,
    },
    [2] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rmii_iomux_rx_er[] = {
    [0] = {
        .gpio_num = 31,
        .func = FUNC_GPIO31_EMAC_PHY_RXER_PAD,
    },
    [1] = {
        .gpio_num = 48,
        .func = FUNC_GPIO48_EMAC_PHY_RXER_PAD,
    },
    [2] = {
        .gpio_num = 54,
        .func = FUNC_GPIO54_EMAC_PHY_RXER_PAD,
    },
    [3] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

const emac_rmii_iomux_info_t emac_rmii_iomux_pins = {
    .clki = emac_rmii_iomux_clki,
    .clko = emac_rmii_iomux_clko,
    .tx_en = emac_rmii_iomux_tx_en,
    .txd0 = emac_rmii_iomux_txd0,
    .txd1 = emac_rmii_iomux_txd1,
    .crs_dv = emac_rmii_iomux_crs_dv,
    .rxd0 = emac_rmii_iomux_rxd0,
    .rxd1 = emac_rmii_iomux_rxd1,
    .tx_er = emac_rmii_iomux_tx_er,
    .rx_er = emac_rmii_iomux_rx_er,
};

const emac_mii_iomux_info_t emac_mii_iomux_pins = { 0 };

/* Registers in retention context:
    DMA:
       0 Bus Mode Register
       3 Receive Descriptor List Address Register (can write to this register only when Rx DMA has stopped)
       4 Transmit Descriptor List Address Register - same as above
       6 Operation Mode Register
       7 Interrupt Enable Register
       18 Current Host Transmit Descriptor Register (RO!) => save to  Transmit Descriptor Reg 4
       19 Current Host Receive Descriptor Register (RO) => save to  Receive Descriptor Reg 3

    MAC:
       0 MAC Configuration Register
       1 MAC Frame Filter
       4 GMII Address Register
       6 Flow Control Register
       15 Interrupt Mask Register
       16 MAC Address0 High Register
       17 MAC Address0 Low Register
       18 - (18 + n*2) MAC Address1-n Registers

    IEEE1588:
        no need to save/restore since such use case does not make sense (system cannot sleep when wants to maintain nsec sync precision)
*/
#if SOC_EMAC_SUPPORT_SLEEP_RETENTION

#define EMAC_MAC_RETENTION_REGS_CNT 4
#define EMAC_MAC_RETENTION_REGS_BASE (DR_REG_EMAC_BASE + 0x0)
static const uint32_t emac_mac_regs_map[4] = {0x53, 0x0, 0x0, 0x0};

#define EMAC_DMA_RETENTION_REGS_CNT 3
#define EMAC_DMA_RETENTION_REGS_BASE (DR_REG_EMAC_BASE + 0x1000)
static const uint32_t emac_dma_regs_map[4] = {0xc1, 0x0, 0x0, 0x0};

const regdma_entries_config_t emac_regdma_entries[] = {
    // backup stage: stop TX/RX DMA
    [0] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_EMAC_LINK(0x00),
                                         EMAC_OPERATIONMODE_REG, 0x0, EMAC_SR_M | EMAC_ST_M, 0, 1),
        .owner = ENTRY(0)
    },
    // backup stage: wait for the TX done (debug reg);
    // IDF-13600
    [1] = {
        .config = REGDMA_LINK_WAIT_INIT(REGDMA_EMAC_LINK(0x01),
                                        EMAC_DEBUG_REG, 0x0, EMAC_TFCSTS_M, 0, 1),
        .owner = ENTRY(0)
    },

    // backup stage: stop TX/RX in MAC layer
    [2] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_EMAC_LINK(0x02),
                                         EMAC_MACCONFIGURATION_REG, 0x0, EMAC_RE_M | EMAC_RE_M, 0, 1),
        .owner = ENTRY(0)
    },

    // restore stage: EMAC SW reset
    [3] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_EMAC_LINK(0x03),
                                         EMAC_BUSMODE_REG, EMAC_SWR, EMAC_SWR_M, 1, 0),
        .owner = ENTRY(0)
    },
    // restore stage: wait for the SW reset done
    [4] = {
        .config = REGDMA_LINK_WAIT_INIT(REGDMA_EMAC_LINK(0x04),
                                        EMAC_BUSMODE_REG, 0x0, EMAC_SWR_M, 1, 0),
        .owner = ENTRY(0)
    },

    // backup stage: save Current Host Tx/Rx Descriptor Register
    // restore stage: restore to Rx/Tx Descriptor List Address Register
    [5] = {
        .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_EMAC_LINK(0x05),
                                              EMAC_CURRENTHOSTRECEIVEDESCRIPTOR_REG, EMAC_RECEIVEDESCRIPTORLISTADDRESS_REG, 1, 0, 0),
        .owner = ENTRY(0)
    },
    [6] = {
        .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_EMAC_LINK(0x06),
                                              EMAC_CURRENTHOSTTRANSMITDESCRIPTOR_REG, EMAC_TRANSMITDESCRIPTORLISTADDRESS_REG, 1, 0, 0),
        .owner = ENTRY(0)
    },

    // backup stage: save MAC Configuration Register (0), MAC Frame Filter (1), GMII Address Register (4) and Flow Control Register (6)
    // restore stage: restore at the same positions
    [7] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_EMAC_LINK(0x07),
                                            EMAC_MAC_RETENTION_REGS_BASE, EMAC_MAC_RETENTION_REGS_BASE,
                                            EMAC_MAC_RETENTION_REGS_CNT, 0, 0,
                                            emac_mac_regs_map[0], emac_mac_regs_map[1],
                                            emac_mac_regs_map[2], emac_mac_regs_map[3]),
        .owner = ENTRY(0)
    },

    // backup stage: save DMA Bus Mode Register (0), Operation Mode Register (6) and Interrupt Enable Register (7)
    // restore stage: restore at the same positions
    [8] = {
        .config = REGDMA_LINK_ADDR_MAP_INIT(REGDMA_EMAC_LINK(0x08),
                                            EMAC_DMA_RETENTION_REGS_BASE, EMAC_DMA_RETENTION_REGS_BASE,
                                            EMAC_DMA_RETENTION_REGS_CNT, 0, 0,
                                            emac_dma_regs_map[0], emac_dma_regs_map[1],
                                            emac_dma_regs_map[2], emac_dma_regs_map[3]),
        .owner = ENTRY(0)
    },

    // backup stage: save Interrupt Mask Register (15) and MAC Address Registers (16-...)
    // restore stage: restore at the same positions
    [9] = {
        // 1 word for int. reg, 2 words for MAC Addr0, 8*2 words for MAC Addr1-8
        .config =  REGDMA_LINK_CONTINUOUS_INIT(REGDMA_EMAC_LINK(0x09),
                                               EMAC_INTERRUPTMASK_REG, EMAC_INTERRUPTMASK_REG, 1 + 2 + 8 * 2, 0, 0),
        .owner = ENTRY(0)
    },

    //** ------------------------------------------------------------------------------------------------
    //  Below steps are to be performed only when link is up and EMAC is started
    //** ------------------------------------------------------------------------------------------------
    // restore stage: start TX in MAC layer
    [EMAC_REGDMA_LINK_EMAC_START_BEGIN] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_EMAC_LINK(EMAC_REGDMA_LINK_EMAC_START_BEGIN),
                                         EMAC_MACCONFIGURATION_REG, EMAC_TE, EMAC_TE_M, 1, 1),
        .owner = ENTRY(0)
    },

    // restore stage: start DMA TX and RX
    [EMAC_REGDMA_LINK_EMAC_START_BEGIN + 1] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_EMAC_LINK((EMAC_REGDMA_LINK_EMAC_START_BEGIN + 1)),
                                         EMAC_OPERATIONMODE_REG, EMAC_ST | EMAC_SR, EMAC_ST_M | EMAC_SR_M, 1, 1),
        .owner = ENTRY(0)
    },

    // restore stage: start RX in MAC layer
    [EMAC_REGDMA_LINK_EMAC_START_BEGIN + 2] = {
        .config = REGDMA_LINK_WRITE_INIT(REGDMA_EMAC_LINK((EMAC_REGDMA_LINK_EMAC_START_BEGIN + 2)),
                                         EMAC_MACCONFIGURATION_REG, EMAC_RE, EMAC_RE_M, 1, 1),
        .owner = ENTRY(0)
    },
};

const emac_reg_retention_info_t emac_reg_retention_info = {
    .module_id = SLEEP_RETENTION_MODULE_EMAC,
    .entry_array = emac_regdma_entries,
    .array_size = ARRAY_SIZE(emac_regdma_entries)
};

#endif // SOC_EMAC_SUPPORT_SLEEP_RETENTION
