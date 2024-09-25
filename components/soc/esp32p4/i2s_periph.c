/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/i2s_periph.h"
#include "soc/i2s_reg.h"
#include "soc/gpio_sig_map.h"
#include "soc/lp_gpio_sig_map.h"

/*
 Bunch of constants for every I2S peripheral: GPIO signals, irqs, hw addr of registers etc
*/
const i2s_signal_conn_t i2s_periph_signal[SOC_I2S_NUM] = {
    [0] = {
        .mck_out_sig  = I2S0_MCLK_PAD_OUT_IDX,
        .mck_in_sig   = I2S0_MCLK_PAD_IN_IDX,

        .m_tx_bck_sig = I2S0_O_BCK_PAD_OUT_IDX,
        .m_rx_bck_sig = I2S0_I_BCK_PAD_OUT_IDX,
        .m_tx_ws_sig  = I2S0_O_WS_PAD_OUT_IDX,
        .m_rx_ws_sig  = I2S0_I_WS_PAD_OUT_IDX,

        .s_tx_bck_sig = I2S0_O_BCK_PAD_IN_IDX,
        .s_rx_bck_sig = I2S0_I_BCK_PAD_IN_IDX,
        .s_tx_ws_sig  = I2S0_O_WS_PAD_IN_IDX,
        .s_rx_ws_sig  = I2S0_I_WS_PAD_IN_IDX,

        .data_out_sigs[0] = I2S0_O_SD_PAD_OUT_IDX,
        .data_out_sigs[1] = I2S0_O_SD1_PAD_OUT_IDX,
        .data_in_sigs[0]  = I2S0_I_SD_PAD_IN_IDX,
        .data_in_sigs[1]  = I2S0_I_SD1_PAD_IN_IDX,
        .data_in_sigs[2]  = I2S0_I_SD2_PAD_IN_IDX,
        .data_in_sigs[3]  = I2S0_I_SD3_PAD_IN_IDX,

        .irq          = ETS_I2S0_INTR_SOURCE,
    },
    [1] = {
        .mck_out_sig  = I2S1_MCLK_PAD_OUT_IDX,
        .mck_in_sig   = I2S1_MCLK_PAD_IN_IDX,

        .m_tx_bck_sig = I2S1_O_BCK_PAD_OUT_IDX,
        .m_rx_bck_sig = I2S1_I_BCK_PAD_OUT_IDX,
        .m_tx_ws_sig  = I2S1_O_WS_PAD_OUT_IDX,
        .m_rx_ws_sig  = I2S1_I_WS_PAD_OUT_IDX,

        .s_tx_bck_sig = I2S1_O_BCK_PAD_IN_IDX,
        .s_rx_bck_sig = I2S1_I_BCK_PAD_IN_IDX,
        .s_tx_ws_sig  = I2S1_O_WS_PAD_IN_IDX,
        .s_rx_ws_sig  = I2S1_I_WS_PAD_IN_IDX,

        .data_out_sigs[0] = I2S1_O_SD_PAD_OUT_IDX,
        .data_out_sigs[1] = -1,
        .data_in_sigs[0]  = I2S1_I_SD_PAD_IN_IDX,
        .data_in_sigs[1]  = -1,
        .data_in_sigs[2]  = -1,
        .data_in_sigs[3]  = -1,

        .irq          = ETS_I2S1_INTR_SOURCE,
    },
    [2] = {
        .mck_out_sig  = I2S2_MCLK_PAD_OUT_IDX,
        .mck_in_sig   = I2S2_MCLK_PAD_IN_IDX,

        .m_tx_bck_sig = I2S2_O_BCK_PAD_OUT_IDX,
        .m_rx_bck_sig = I2S2_I_BCK_PAD_OUT_IDX,
        .m_tx_ws_sig  = I2S2_O_WS_PAD_OUT_IDX,
        .m_rx_ws_sig  = I2S2_I_WS_PAD_OUT_IDX,

        .s_tx_bck_sig = I2S2_O_BCK_PAD_IN_IDX,
        .s_rx_bck_sig = I2S2_I_BCK_PAD_IN_IDX,
        .s_tx_ws_sig  = I2S2_O_WS_PAD_IN_IDX,
        .s_rx_ws_sig  = I2S2_I_WS_PAD_IN_IDX,

        .data_out_sigs[0] = I2S2_O_SD_PAD_OUT_IDX,
        .data_out_sigs[1] = -1,
        .data_in_sigs[0]  = I2S2_I_SD_PAD_IN_IDX,
        .data_in_sigs[1]  = -1,
        .data_in_sigs[2]  = -1,
        .data_in_sigs[3]  = -1,

        .irq          = ETS_I2S2_INTR_SOURCE,
    },
};

const i2s_signal_conn_t lp_i2s_periph_signal[SOC_LP_I2S_NUM] = {
   [0] = {
        .mck_out_sig  = -1,
        .mck_in_sig   = -1,

        .m_tx_bck_sig = LP_I2S_O_BCK_PAD_OUT_IDX,
        .m_rx_bck_sig = LP_I2S_I_BCK_PAD_OUT_IDX,
        .m_tx_ws_sig  = LP_I2S_O_WS_PAD_OUT_IDX,
        .m_rx_ws_sig  = LP_I2S_I_WS_PAD_OUT_IDX,

        .s_tx_bck_sig = LP_I2S_O_BCK_PAD_IN_IDX,
        .s_rx_bck_sig = LP_I2S_I_BCK_PAD_IN_IDX,
        .s_tx_ws_sig  = LP_I2S_O_WS_PAD_IN_IDX,
        .s_rx_ws_sig  = LP_I2S_I_WS_PAD_IN_IDX,

        .data_out_sigs[0] = LP_I2S_O_SD_PAD_OUT_IDX,
        .data_out_sigs[1] = -1,
        .data_in_sigs[0]  = LP_I2S_I_SD_PAD_IN_IDX,
        .data_in_sigs[1]  = -1,
        .data_in_sigs[2]  = -1,
        .data_in_sigs[3]  = -1,

        .irq          = ETS_LP_I2S_INTR_SOURCE,
    },
};

/**
 * I2S Registers to be saved during sleep retention
 * - I2S_RX_CONF_REG
 * - I2S_TX_CONF_REG
 * - I2S_RX_CONF1_REG
 * - I2S_TX_CONF1_REG
 * - I2S_TX_PCM2PDM_CONF_REG
 * - I2S_TX_PCM2PDM_CONF1_REG
 * - I2S_RX_PDM2PCM_CONF_REG
 * - I2S_RX_TDM_CTRL_REG
 * - I2S_TX_TDM_CTRL_REG
 * - I2S_RXEOF_NUM_REG
 * - I2S_ETM_CONF_REG
*/
#define I2S_RETENTION_REGS_CNT 11
#define I2S_RETENTION_REGS_BASE(i) I2S_RX_CONF_REG(i)
static const uint32_t i2s_regs_map[4] = {0x12370f, 0x0, 0x0, 0x0};
#define I2S_SLEEP_RETENTION_ENTRIES(i2s_port) {  \
    /* Save/restore the register values */  \
    [0] = { .config = REGDMA_LINK_ADDR_MAP_INIT(  \
                REGDMA_I2S_LINK(0x00), \
                I2S_RETENTION_REGS_BASE(i2s_port),  \
                I2S_RETENTION_REGS_BASE(i2s_port), \
                I2S_RETENTION_REGS_CNT, 0, 0, \
                i2s_regs_map[0], i2s_regs_map[1], \
                i2s_regs_map[2], i2s_regs_map[3]), \
            .owner = ENTRY(0)}, \
};

static const regdma_entries_config_t i2s0_regs_retention[] = I2S_SLEEP_RETENTION_ENTRIES(0);
static const regdma_entries_config_t i2s1_regs_retention[] = I2S_SLEEP_RETENTION_ENTRIES(1);
static const regdma_entries_config_t i2s2_regs_retention[] = I2S_SLEEP_RETENTION_ENTRIES(2);

const i2s_reg_retention_info_t i2s_reg_retention_info[SOC_I2S_NUM] = {
    [0] = {
        .retention_module = SLEEP_RETENTION_MODULE_I2S0,
        .entry_array = i2s0_regs_retention,
        .array_size = ARRAY_SIZE(i2s0_regs_retention)
    },
    [1] = {
        .retention_module = SLEEP_RETENTION_MODULE_I2S1,
        .entry_array = i2s1_regs_retention,
        .array_size = ARRAY_SIZE(i2s1_regs_retention)
    },
    [2] = {
        .retention_module = SLEEP_RETENTION_MODULE_I2S2,
        .entry_array = i2s2_regs_retention,
        .array_size = ARRAY_SIZE(i2s2_regs_retention)
    },
};
