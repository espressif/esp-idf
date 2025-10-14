/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/twai_periph.h"
#include "soc/gpio_sig_map.h"

const twai_signal_conn_t twai_periph_signals[SOC_TWAI_CONTROLLER_NUM] = {
    [0] = {
        .module_name = "TWAI0",
        .irq_id = ETS_TWAI_INTR_SOURCE,
        .tx_sig = TWAI_TX_IDX,
        .rx_sig = TWAI_RX_IDX,
        .bus_off_sig = TWAI_BUS_OFF_ON_IDX,
        .clk_out_sig = TWAI_CLKOUT_IDX,
        .stand_by_sig = -1,
    },
};
