/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "soc/emac_periph.h"
#include "soc/io_mux_reg.h"

const emac_io_info_t emac_io_idx = {
    .mdc_idx = MII_MDC_PAD_OUT_IDX,
    .mdo_idx = MII_MDO_PAD_OUT_IDX,
    .mdi_idx = MII_MDI_PAD_IN_IDX,
    .mii_tx_clk_i_idx = EMAC_TX_CLK_PAD_IN_IDX,
    .mii_tx_en_o_idx = EMAC_PHY_TXEN_PAD_OUT_IDX,
    .mii_txd0_o_idx = EMAC_PHY_TXD0_PAD_OUT_IDX,
    .mii_txd1_o_idx = EMAC_PHY_TXD0_PAD_OUT_IDX,
    .mii_txd2_o_idx = EMAC_PHY_TXD0_PAD_OUT_IDX,
    .mii_txd3_o_idx = EMAC_PHY_TXD0_PAD_OUT_IDX,
    .mii_rx_clk_i_idx = EMAC_RX_CLK_PAD_IN_IDX,
    .mii_rx_dv_i_idx = EMAC_PHY_RXDV_PAD_IN_IDX,
    .mii_rxd0_i_idx = EMAC_PHY_RXD0_PAD_IN_IDX,
    .mii_rxd1_i_idx = EMAC_PHY_RXD1_PAD_IN_IDX,
    .mii_rxd2_i_idx = EMAC_PHY_RXD2_PAD_IN_IDX,
    .mii_rxd3_i_idx = EMAC_PHY_RXD3_PAD_IN_IDX,
    .mii_col_i_idx = EMAC_PHY_COL_PAD_IN_IDX,
    .mii_crs_i_idx = EMAC_PHY_CRS_PAD_IN_IDX,
    .mii_tx_er_o_idx = EMAC_PHY_TXER_PAD_OUT_IDX,
    .mii_rx_er_i_idx = EMAC_PHY_RXER_PAD_IN_IDX
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
