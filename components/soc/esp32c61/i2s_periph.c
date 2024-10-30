/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/i2s_periph.h"
#include "soc/i2s_reg.h"
#include "soc/gpio_sig_map.h"

/*
 Bunch of constants for every I2S peripheral: GPIO signals, irqs, hw addr of registers etc
*/
const i2s_signal_conn_t i2s_periph_signal[SOC_I2S_NUM] = {
    {
        .mck_out_sig  = I2S_MCLK_OUT_IDX,
        .mck_in_sig   = I2S_MCLK_IN_IDX,

        .m_tx_bck_sig = I2SO_BCK_OUT_IDX,
        .m_rx_bck_sig = I2SI_BCK_OUT_IDX,
        .m_tx_ws_sig  = I2SO_WS_OUT_IDX,
        .m_rx_ws_sig  = I2SI_WS_OUT_IDX,

        .s_tx_bck_sig = I2SO_BCK_IN_IDX,
        .s_rx_bck_sig = I2SI_BCK_IN_IDX,
        .s_tx_ws_sig  = I2SO_WS_IN_IDX,
        .s_rx_ws_sig  = I2SI_WS_IN_IDX,

        .data_out_sigs[0] = I2SO_SD_OUT_IDX,
        .data_out_sigs[1] = I2SO_SD1_OUT_IDX,
        .data_in_sig  = I2SI_SD_IN_IDX,

        .irq          = ETS_I2S0_INTR_SOURCE,
    }
};

/**
 * I2S Registers to be saved during sleep retention
 * - I2S_RX_CONF_REG
 * - I2S_TX_CONF_REG
 * - I2S_RX_CONF1_REG
 * - I2S_TX_CONF1_REG
 * - I2S_TX_PCM2PDM_CONF_REG
 * - I2S_TX_PCM2PDM_CONF1_REG
 * - I2S_RX_TDM_CTRL_REG
 * - I2S_TX_TDM_CTRL_REG
 * - I2S_RXEOF_NUM_REG
 * - I2S_ETM_CONF_REG
*/
#define I2S_RETENTION_REGS_CNT 10
#define I2S_RETENTION_REGS_BASE(i) I2S_RX_CONF_REG
static const uint32_t i2s_regs_map[4] = {0x12360f, 0x0, 0x0, 0x0};
#define I2S_SLEEP_RETENTION_ENTRIES(i2s_port) {  \
    /* Save/restore the register values */  \
    [0] = { .config = REGDMA_LINK_ADDR_MAP_INIT(  \
                REGDMA_I2S_LINK(0x00), \
                I2S_RETENTION_REGS_BASE(i2s_port),  \
                I2S_RETENTION_REGS_BASE(i2s_port), \
                I2S_RETENTION_REGS_CNT, 0, 0, \
                i2s_regs_map[0], i2s_regs_map[1], \
                i2s_regs_map[2], i2s_regs_map[3]), \
            .owner = ENTRY(0) | ENTRY(2)}, \
};

static const regdma_entries_config_t i2s0_regs_retention[] = I2S_SLEEP_RETENTION_ENTRIES(0);

const i2s_reg_retention_info_t i2s_reg_retention_info[SOC_I2S_NUM] = {
    [0] = {
        .retention_module = SLEEP_RETENTION_MODULE_I2S0,
        .entry_array = i2s0_regs_retention,
        .array_size = ARRAY_SIZE(i2s0_regs_retention)
    },
};
