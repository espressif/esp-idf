/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "soc/emac_periph.h"
#include "soc/io_mux_reg.h"

const emac_io_info_t emac_io_idx = {
    .mdc_idx = EMAC_MDC_O_IDX,
    .mdo_idx = EMAC_MDO_O_IDX,
    .mdi_idx = EMAC_MDI_I_IDX,
    .mii_tx_clk_i_idx = SIG_GPIO_OUT_IDX, // indicates EMAC signal cannot be connected via GPIO Matrix on the target
    .mii_tx_en_o_idx = SIG_GPIO_OUT_IDX,
    .mii_txd0_o_idx = SIG_GPIO_OUT_IDX,
    .mii_txd1_o_idx = SIG_GPIO_OUT_IDX,
    .mii_txd2_o_idx = SIG_GPIO_OUT_IDX,
    .mii_txd3_o_idx = SIG_GPIO_OUT_IDX,
    .mii_rx_clk_i_idx = SIG_GPIO_OUT_IDX,
    .mii_rx_dv_i_idx = SIG_GPIO_OUT_IDX,
    .mii_rxd0_i_idx = SIG_GPIO_OUT_IDX,
    .mii_rxd1_i_idx = SIG_GPIO_OUT_IDX,
    .mii_rxd2_i_idx = SIG_GPIO_OUT_IDX,
    .mii_rxd3_i_idx = SIG_GPIO_OUT_IDX,
    .mii_col_i_idx = SIG_GPIO_OUT_IDX,
    .mii_crs_i_idx = SIG_GPIO_OUT_IDX,
    .mii_tx_er_o_idx = SIG_GPIO_OUT_IDX,
    .mii_rx_er_i_idx = SIG_GPIO_OUT_IDX
};

static const emac_iomux_info_t emac_rmii_iomux_clki[] = {
    [0] = {
        .gpio_num = 0,
        .func = FUNC_GPIO0_EMAC_TX_CLK,
    },
    [1] = {
        .gpio_num = GPIO_NUM_MAX, // indicates end of list
    }
};

static const emac_iomux_info_t emac_rmii_iomux_clko[] = {
    [0] = {
        .gpio_num = 16,
        .func = FUNC_GPIO16_EMAC_CLK_OUT,
    },
    [1] = {
        .gpio_num = 17,
        .func = FUNC_GPIO17_EMAC_CLK_OUT_180,
    },
    [2] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rmii_mii_iomux_tx_en[] = {
    [0] = {
        .gpio_num = 21,
        .func = FUNC_GPIO21_EMAC_TX_EN,
    },
    [1] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rmii_mii_iomux_txd0[] = {
    [0] = {
        .gpio_num = 19,
        .func = FUNC_GPIO19_EMAC_TXD0,
    },
    [1] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rmii_mii_iomux_txd1[] = {
    [0] = {
        .gpio_num = 22,
        .func = FUNC_GPIO22_EMAC_TXD1,
    },
    [1] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rmii_mii_iomux_crs_dv[] = {
    [0] = {
        .gpio_num = 27,
        .func = FUNC_GPIO27_EMAC_RX_DV,
    },
    [1] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rmii_mii_iomux_rxd0[] = {
    [0] = {
        .gpio_num = 25,
        .func = FUNC_GPIO25_EMAC_RXD0,
    },
    [1] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rmii_mii_iomux_rxd1[] = {
    [0] = {
        .gpio_num = 26,
        .func = FUNC_GPIO26_EMAC_RXD1,
    },
    [1] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rmii_mii_iomux_tx_er[] = {
    [0] = {
        .gpio_num = 4,
        .func = FUNC_GPIO4_EMAC_TX_ER,
    },
    [1] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_rmii_mii_iomux_rx_er[] = {
    [0] = {
        .gpio_num = 13,
        .func = FUNC_MTCK_EMAC_RX_ER,
    },
    [1] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

// MII Specific
static const emac_iomux_info_t emac_mii_iomux_clk_tx[] = {
    [0] = {
        .gpio_num = 0,
        .func = FUNC_GPIO0_EMAC_TX_CLK,
    },
    [1] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_mii_iomux_txd2[] = {
    [0] = {
        .gpio_num = 14,
        .func = FUNC_MTMS_EMAC_TXD2,
    },
    [1] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_mii_iomux_txd3[] = {
    [0] = {
        .gpio_num = 12,
        .func = FUNC_MTDI_EMAC_TXD3,
    },
    [1] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_mii_iomux_clk_rx[] = {
    [0] = {
        .gpio_num = 5,
        .func = FUNC_GPIO5_EMAC_RX_CLK,
    },
    [1] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_mii_iomux_rxd2[] = {
    [0] = {
        .gpio_num = 1,
        .func = FUNC_U0TXD_EMAC_RXD2,
    },
    [1] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

static const emac_iomux_info_t emac_mii_iomux_rxd3[] = {
    [0] = {
        .gpio_num = 15,
        .func = FUNC_MTDO_EMAC_RXD3,
    },
    [1] = {
        .gpio_num = GPIO_NUM_MAX,
    }
};

const emac_rmii_iomux_info_t emac_rmii_iomux_pins = {
    .clki = emac_rmii_iomux_clki,
    .clko = emac_rmii_iomux_clko,
    .tx_en = emac_rmii_mii_iomux_tx_en,
    .txd0 = emac_rmii_mii_iomux_txd0,
    .txd1 = emac_rmii_mii_iomux_txd1,
    .crs_dv = emac_rmii_mii_iomux_crs_dv,
    .rxd0 = emac_rmii_mii_iomux_rxd0,
    .rxd1 = emac_rmii_mii_iomux_rxd1,
    .tx_er = emac_rmii_mii_iomux_tx_er,
    .rx_er = emac_rmii_mii_iomux_rx_er,
};

const emac_mii_iomux_info_t emac_mii_iomux_pins = {
    .clk_tx = emac_mii_iomux_clk_tx,
    .tx_en = emac_rmii_mii_iomux_tx_en,
    .txd0 = emac_rmii_mii_iomux_txd0,
    .txd1 = emac_rmii_mii_iomux_txd1,
    .txd2 = emac_mii_iomux_txd2,
    .txd3 = emac_mii_iomux_txd3,
    .clk_rx = emac_mii_iomux_clk_rx,
    .rx_dv = emac_rmii_mii_iomux_crs_dv,
    .rxd0 = emac_rmii_mii_iomux_rxd0,
    .rxd1 = emac_rmii_mii_iomux_rxd1,
    .rxd2 = emac_mii_iomux_rxd2,
    .rxd3 = emac_mii_iomux_rxd3,
    .tx_er = emac_rmii_mii_iomux_tx_er,
    .rx_er = emac_rmii_mii_iomux_rx_er,
};
