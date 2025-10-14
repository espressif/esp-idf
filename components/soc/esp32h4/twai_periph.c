/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/twai_periph.h"
#include "soc/gpio_sig_map.h"

const twai_signal_conn_t twai_periph_signals[1] = {
    [0] = {
        .module_name = "TWAI0",
        .irq_id = ETS_TWAI0_INTR_SOURCE,
        .timer_irq_id = ETS_TWAI0_TIMER_INTR_SOURCE,
        .tx_sig = TWAI0_TX_IDX,
        .rx_sig = TWAI0_RX_IDX,
        .bus_off_sig = -1,
        .clk_out_sig = -1,
        .stand_by_sig = -1,
    }
};
