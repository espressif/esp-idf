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
            .tx_irq_id = ETS_PARL_IO_INTR_SOURCE,
            .rx_irq_id = ETS_PARL_IO_INTR_SOURCE,
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
                        PARL_TX_DATA8_IDX,
                        PARL_TX_DATA9_IDX,
                        PARL_TX_DATA10_IDX,
                        PARL_TX_DATA11_IDX,
                        PARL_TX_DATA12_IDX,
                        PARL_TX_DATA13_IDX,
                        PARL_TX_DATA14_IDX,
                        PARL_TX_DATA15_IDX,
                    },
                    .clk_out_sig = PARL_TX_CLK_OUT_IDX,
                    .clk_in_sig = PARL_TX_CLK_IN_IDX,
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
                        PARL_RX_DATA8_IDX,
                        PARL_RX_DATA9_IDX,
                        PARL_RX_DATA10_IDX,
                        PARL_RX_DATA11_IDX,
                        PARL_RX_DATA12_IDX,
                        PARL_RX_DATA13_IDX,
                        PARL_RX_DATA14_IDX,
                        PARL_RX_DATA15_IDX,
                    },
                    .clk_out_sig = -1,
                    .clk_in_sig = PARL_RX_CLK_IN_IDX,
                }
            }
        },
    },
};
