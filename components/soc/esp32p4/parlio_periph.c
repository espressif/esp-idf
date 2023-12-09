/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/parlio_periph.h"
#include "soc/gpio_sig_map.h"

const parlio_signal_conn_t parlio_periph_signals = {
    .groups = {
        [0] = {
            .module = PERIPH_PARLIO_MODULE,
            .tx_irq_id = ETS_HP_PARLIO_TX_INTR_SOURCE,
            .rx_irq_id = ETS_HP_PARLIO_RX_INTR_SOURCE,
            .tx_units = {
                [0] = {
                    .data_sigs = {
                        PARLIO_TX_DATA0_PAD_OUT_IDX,
                        PARLIO_TX_DATA1_PAD_OUT_IDX,
                        PARLIO_TX_DATA2_PAD_OUT_IDX,
                        PARLIO_TX_DATA3_PAD_OUT_IDX,
                        PARLIO_TX_DATA4_PAD_OUT_IDX,
                        PARLIO_TX_DATA5_PAD_OUT_IDX,
                        PARLIO_TX_DATA6_PAD_OUT_IDX,
                        PARLIO_TX_DATA7_PAD_OUT_IDX,
                        PARLIO_TX_DATA8_PAD_OUT_IDX,
                        PARLIO_TX_DATA9_PAD_OUT_IDX,
                        PARLIO_TX_DATA10_PAD_OUT_IDX,
                        PARLIO_TX_DATA11_PAD_OUT_IDX,
                        PARLIO_TX_DATA12_PAD_OUT_IDX,
                        PARLIO_TX_DATA13_PAD_OUT_IDX,
                        PARLIO_TX_DATA14_PAD_OUT_IDX,
                        PARLIO_TX_DATA15_PAD_OUT_IDX,
                    },
                    .clk_out_sig = PARLIO_TX_CLK_PAD_OUT_IDX,
                    .clk_in_sig = PARLIO_TX_CLK_PAD_IN_IDX,
                }
            },
            .rx_units = {
                [0] = {
                    .data_sigs = {
                        PARLIO_RX_DATA0_PAD_IN_IDX,
                        PARLIO_RX_DATA1_PAD_IN_IDX,
                        PARLIO_RX_DATA2_PAD_IN_IDX,
                        PARLIO_RX_DATA3_PAD_IN_IDX,
                        PARLIO_RX_DATA4_PAD_IN_IDX,
                        PARLIO_RX_DATA5_PAD_IN_IDX,
                        PARLIO_RX_DATA6_PAD_IN_IDX,
                        PARLIO_RX_DATA7_PAD_IN_IDX,
                        PARLIO_RX_DATA8_PAD_IN_IDX,
                        PARLIO_RX_DATA9_PAD_IN_IDX,
                        PARLIO_RX_DATA10_PAD_IN_IDX,
                        PARLIO_RX_DATA11_PAD_IN_IDX,
                        PARLIO_RX_DATA12_PAD_IN_IDX,
                        PARLIO_RX_DATA13_PAD_IN_IDX,
                        PARLIO_RX_DATA14_PAD_IN_IDX,
                        PARLIO_RX_DATA15_PAD_IN_IDX,
                    },
                    .clk_out_sig = PARLIO_RX_CLK_PAD_OUT_IDX,
                    .clk_in_sig = PARLIO_RX_CLK_PAD_IN_IDX,
                }
            }
        },
    },
};
