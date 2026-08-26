/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/parlio_periph.h"
#include "soc/gpio_sig_map.h"
#include "soc/interrupts.h"

const soc_parlio_signal_desc_t soc_parlio_signals[1] = {
    [0] = {
        .module_name = "PARLIO0",
        .tx_irq_id = ETS_PARL_IO_TX_INTR_SOURCE,
        .rx_irq_id = ETS_PARL_IO_RX_INTR_SOURCE,
        .tx_units = {
            [0] = {
                .data_sigs = {
                    PARL_TX_DATA0_IDX,
                    PARL_TX_DATA1_IDX,
                    PARL_TX_DATA2_IDX,
                    PARL_TX_DATA3_IDX,
                    PARL_TX_DATA4_IDX,
                    PARL_TX_DATA5_IDX,
                    PARL_TX_DATA6_IDX,
                    PARL_TX_DATA7_IDX,
                },
                .clk_out_sig = PARL_TX_CLK_OUT_IDX,
                .clk_in_sig = PARL_TX_CLK_IN_IDX,
                .cs_sig = -1,
            }
        },
        .rx_units = {
            [0] = {
                .data_sigs = {
                    PARL_RX_DATA0_IDX,
                    PARL_RX_DATA1_IDX,
                    PARL_RX_DATA2_IDX,
                    PARL_RX_DATA3_IDX,
                    PARL_RX_DATA4_IDX,
                    PARL_RX_DATA5_IDX,
                    PARL_RX_DATA6_IDX,
                    PARL_RX_DATA7_IDX,
                },
                .clk_out_sig = PARL_RX_CLK_OUT_IDX,
                .clk_in_sig = PARL_RX_CLK_IN_IDX,
            }
        }
    }
};
