/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/twai_periph.h"
#include "soc/gpio_sig_map.h"

const twai_controller_signal_conn_t twai_controller_periph_signals = {
    .controllers = {
        [0] = {
            .module = PERIPH_TWAI0_MODULE,
            .irq_id = ETS_TWAI0_INTR_SOURCE,
            .tx_sig = TWAI0_TX_IDX,
            .rx_sig = TWAI0_RX_IDX,
            .bus_off_sig = TWAI0_BUS_OFF_ON_IDX,
            .clk_out_sig = TWAI0_CLKOUT_IDX,
            .stand_by_sig = TWAI0_STANDBY_IDX,
        },
        [1] = {
            .module = PERIPH_TWAI1_MODULE,
            .irq_id = ETS_TWAI1_INTR_SOURCE,
            .tx_sig = TWAI1_TX_IDX,
            .rx_sig = TWAI1_RX_IDX,
            .bus_off_sig = TWAI1_BUS_OFF_ON_IDX,
            .clk_out_sig = TWAI1_CLKOUT_IDX,
            .stand_by_sig = TWAI1_STANDBY_IDX,
        }
    }
};
