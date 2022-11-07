/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc.h"
#include "soc/twai_periph.h"
#include "soc/gpio_sig_map.h"

const twai_controller_signal_conn_t twai_controller_periph_signals = {
    .controllers = {
        [0] = {
            .module = PERIPH_TWAI_MODULE,
            .irq_id = ETS_TWAI_INTR_SOURCE,
            .tx_sig = TWAI_TX_IDX,
            .rx_sig = TWAI_RX_IDX,
            .bus_off_sig = TWAI_BUS_OFF_ON_IDX,
            .clk_out_sig = TWAI_CLKOUT_IDX,
            .stand_by_sig = -1,
        },
    }
};
