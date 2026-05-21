/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * EMAC peripheral GPIO pin mappings for ESP32S31.
 *
 * GPIO signal names use GMAC_ prefix in S31 (vs EMAC_ in P4).
 *
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
    .mdc_idx = GMII_MDC_PAD_OUT_IDX,
    .mdo_idx = GMII_MDO_PAD_OUT_IDX,
    .mdi_idx = GMII_MDI_PAD_IN_IDX,
    .mii_tx_clk_i_idx = GMAC_TX_CLK_PAD_IN_IDX,
    .mii_tx_en_o_idx = GMAC_PHY_TXEN_PAD_OUT_IDX,
    .mii_txd0_o_idx = GMAC_PHY_TXD0_PAD_OUT_IDX,
    .mii_txd1_o_idx = GMAC_PHY_TXD1_PAD_OUT_IDX,
    .mii_txd2_o_idx = GMAC_PHY_TXD2_PAD_OUT_IDX,
    .mii_txd3_o_idx = GMAC_PHY_TXD3_PAD_OUT_IDX,
    .mii_rx_clk_i_idx = GMAC_RX_CLK_PAD_IN_IDX,
    .mii_rx_dv_i_idx = GMAC_PHY_RXDV_PAD_IN_IDX,
    .mii_rxd0_i_idx = GMAC_PHY_RXD0_PAD_IN_IDX,
    .mii_rxd1_i_idx = GMAC_PHY_RXD1_PAD_IN_IDX,
    .mii_rxd2_i_idx = GMAC_PHY_RXD2_PAD_IN_IDX,
    .mii_rxd3_i_idx = GMAC_PHY_RXD3_PAD_IN_IDX,
    .mii_col_i_idx = GMAC_PHY_COL_PAD_IN_IDX,
    .mii_crs_i_idx = GMAC_PHY_CRS_PAD_IN_IDX,
    .mii_tx_er_o_idx = GMAC_PHY_TXER_PAD_OUT_IDX,
    .mii_rx_er_i_idx = GMAC_PHY_RXER_PAD_IN_IDX,
    .rmii_refclk_i_idx = SIG_GPIO_OUT_IDX,
    .rmii_refclk_o_idx = SIG_GPIO_OUT_IDX,
    .phy_ref_clk_o_idx = SIG_GPIO_OUT_IDX,
    .ptp_pps_idx = EMAC_PTP_PPS_PAD_OUT_IDX,
};

/*
 * ESP32S31 has two sets of GMAC I/O pads (as shown in the GMAC I/O table):
 * - Set 1: GPIO8-19 (first set, some in CNNT domain for wake-up)
 * - Set 2: GPIO36-47 (second set)
 *
 * CNNT domain pads (GPIO13-19): These 7 IOs remain operational when the TOP domain
 * is powered down, supporting GMAC wake-up functionality. To use them, set
 * CNNT_IO_MUX_CTRL_REG.gmac_pad_pin_ctrl_ded_sel = 1.
 */

static const emac_iomux_info_t emac_rmii_iomux_clki[] = {
    [0] = {
        .gpio_num = 13,
        .func = FUNC_GPIO13_GMAC_RMII_CLK_PAD,
    },
    [1] = {
        .gpio_num = 43,
        .func = FUNC_GPIO43_GMAC_RMII_CLK_PAD,
    },
    [2] = {
        .gpio_num = GPIO_NUM_MAX, // indicates end of list
    }
};

static const emac_iomux_info_t emac_rmii_iomux_clko[] = {
    [0] = {
        .gpio_num = 13,
        .func = FUNC_GPIO13_GMAC_RMII_CLK_PAD,
    },
    [1] = {
        .gpio_num = 43,
        .func = FUNC_GPIO43_GMAC_RMII_CLK_PAD,
    },
    [2] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rmii_iomux_tx_en[] = {
    [0] = {
        .gpio_num = 12,
        .func = FUNC_GPIO12_GMAC_PHY_TXEN_PAD,
    },
    [1] = {
        .gpio_num = 37,
        .func = FUNC_GPIO37_GMAC_PHY_TXEN_PAD,
    },
    [2] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rmii_iomux_txd0[] = {
    [0] = {
        .gpio_num = 8,
        .func = FUNC_GPIO8_GMAC_PHY_TXD0_PAD,
    },
    [1] = {
        .gpio_num = 44,
        .func = FUNC_GPIO44_GMAC_PHY_TXD0_PAD,
    },
    [2] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rmii_iomux_txd1[] = {
    [0] = {
        .gpio_num = 9,
        .func = FUNC_GPIO9_GMAC_PHY_TXD1_PAD,
    },
    [1] = {
        .gpio_num = 45,
        .func = FUNC_GPIO45_GMAC_PHY_TXD1_PAD,
    },
    [2] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rmii_iomux_crs_dv[] = {
    [0] = {
        .gpio_num = 15,
        .func = FUNC_GPIO15_GMAC_PHY_RXDV_PAD,
    },
    [1] = {
        .gpio_num = 36,
        .func = FUNC_GPIO36_GMAC_PHY_RXDV_PAD,
    },
    [2] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rmii_iomux_rxd0[] = {
    [0] = {
        .gpio_num = 19,
        .func = FUNC_GPIO19_GMAC_PHY_RXD0_PAD,
    },
    [1] = {
        .gpio_num = 41,
        .func = FUNC_GPIO41_GMAC_PHY_RXD0_PAD,
    },
    [2] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rmii_iomux_rxd1[] = {
    [0] = {
        .gpio_num = 18,
        .func = FUNC_GPIO18_GMAC_PHY_RXD1_PAD,
    },
    [1] = {
        .gpio_num = 40,
        .func = FUNC_GPIO40_GMAC_PHY_RXD1_PAD,
    },
    [2] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

/* tx_er and rx_er - no dedicated IOMUX functions. They can be routed via GPIO matrix instead. */
static const emac_iomux_info_t emac_rmii_iomux_tx_er[] = {
    [0] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rmii_iomux_rx_er[] = {
    [0] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};
/* ------------------end of rmii iomux info ------------------*/

/* ------------------start of rgmii iomux info ------------------*/
static const emac_iomux_info_t emac_rgmii_iomux_clki[] = {
    [0] = {
        .gpio_num = 14,
        .func = FUNC_GPIO14_GMAC_RX_CLK_PAD,
    },
    [1] = {
        .gpio_num = 42,
        .func = FUNC_GPIO42_GMAC_RX_CLK_PAD,
    },
    [2] = {
        .gpio_num = GPIO_NUM_MAX, // indicates end of list
    }
};

static const emac_iomux_info_t emac_rgmii_iomux_clko[] = {
    [0] = {
        .gpio_num = 13,
        .func = FUNC_GPIO13_GMAC_RMII_CLK_PAD,
    },

    [1] = {
        .gpio_num = 43,
        .func = FUNC_GPIO43_GMAC_RMII_CLK_PAD,
    },
    [2] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rgmii_iomux_tx_ctl[] = {
    [0] = {
        .gpio_num = 12,
        .func = FUNC_GPIO12_GMAC_PHY_TXEN_PAD,
    },
    [1] = {
        .gpio_num = 37,
        .func = FUNC_GPIO37_GMAC_PHY_TXEN_PAD,
    },
    [2] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rgmii_iomux_txd0[] = {
    [0] = {
        .gpio_num = 8,
        .func = FUNC_GPIO8_GMAC_PHY_TXD0_PAD,
    },
    [1] = {
        .gpio_num = 44,
        .func = FUNC_GPIO44_GMAC_PHY_TXD0_PAD,
    },
    [2] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rgmii_iomux_txd1[] = {
    [0] = {
        .gpio_num = 9,
        .func = FUNC_GPIO9_GMAC_PHY_TXD1_PAD,
    },
    [1] = {
        .gpio_num = 45,
        .func = FUNC_GPIO45_GMAC_PHY_TXD1_PAD,
    },
    [2] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rgmii_iomux_txd2[] = {
    [0] = {
        .gpio_num = 10,
        .func = FUNC_GPIO10_GMAC_PHY_TXD2_PAD,
    },
    [1] = {
        .gpio_num = 46,
        .func = FUNC_GPIO46_GMAC_PHY_TXD2_PAD,
    },
    [2] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rgmii_iomux_txd3[] = {
    [0] = {
        .gpio_num = 11,
        .func = FUNC_GPIO11_GMAC_PHY_TXD3_PAD,
    },
    [1] = {
        .gpio_num = 47,
        .func = FUNC_GPIO47_GMAC_PHY_TXD3_PAD,
    },
    [2] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rgmii_iomux_rx_ctl[] = {
    [0] = {
        .gpio_num = 15,
        .func = FUNC_GPIO15_GMAC_PHY_RXDV_PAD,
    },
    [1] = {
        .gpio_num = 36,
        .func = FUNC_GPIO36_GMAC_PHY_RXDV_PAD,
    },
    [2] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rgmii_iomux_rxd0[] = {
    [0] = {
        .gpio_num = 19,
        .func = FUNC_GPIO19_GMAC_PHY_RXD0_PAD,
    },
    [1] = {
        .gpio_num = 41,
        .func = FUNC_GPIO41_GMAC_PHY_RXD0_PAD,
    },
    [2] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rgmii_iomux_rxd1[] = {
    [0] = {
        .gpio_num = 18,
        .func = FUNC_GPIO18_GMAC_PHY_RXD1_PAD,
    },
    [1] = {
        .gpio_num = 40,
        .func = FUNC_GPIO40_GMAC_PHY_RXD1_PAD,
    },
    [2] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rgmii_iomux_rxd2[] = {
    [0] = {
        .gpio_num = 17,
        .func = FUNC_GPIO17_GMAC_PHY_RXD2_PAD,
    },
    [1] = {
        .gpio_num = 39,
        .func = FUNC_GPIO39_GMAC_PHY_RXD2_PAD,
    },
    [2] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rgmii_iomux_rxd3[] = {
    [0] = {
        .gpio_num = 16,
        .func = FUNC_GPIO16_GMAC_PHY_RXD3_PAD,
    },
    [1] = {
        .gpio_num = 38,
        .func = FUNC_GPIO38_GMAC_PHY_RXD3_PAD,
    },
    [2] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

/* ---end of rgmii iomux info ------------------*/

/* ---start of ref clk iomux info ------------------*/
static const emac_iomux_info_t emac_ref_clk_iomux_phy_ref_clk[] = {
    [0] = {
        .gpio_num = 35,
        .func = FUNC_GPIO35_REF_GMAC_CLK_PAD,
    },
    [1] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

/* ---end of ref clk iomux info ------------------*/

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

const emac_rgmii_iomux_info_t emac_rgmii_iomux_pins = {
    .clk_tx = emac_rgmii_iomux_clko,
    .tx_ctl = emac_rgmii_iomux_tx_ctl,
    .txd0 = emac_rgmii_iomux_txd0,
    .txd1 = emac_rgmii_iomux_txd1,
    .txd2 = emac_rgmii_iomux_txd2,
    .txd3 = emac_rgmii_iomux_txd3,
    .clk_rx = emac_rgmii_iomux_clki,
    .rx_ctl = emac_rgmii_iomux_rx_ctl,
    .rxd0 = emac_rgmii_iomux_rxd0,
    .rxd1 = emac_rgmii_iomux_rxd1,
    .rxd2 = emac_rgmii_iomux_rxd2,
    .rxd3 = emac_rgmii_iomux_rxd3,
};

const emac_ref_clk_iomux_info_t emac_ref_clk_iomux_pins = {
    .phy_ref_clk = emac_ref_clk_iomux_phy_ref_clk,
};

const emac_mii_iomux_info_t emac_mii_iomux_pins = { 0 };
