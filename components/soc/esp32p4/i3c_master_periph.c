/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/i3c_master_periph.h"
#include "soc/gpio_sig_map.h"
#include "soc/interrupts.h"

/*
 Bunch of constants for every I3C peripheral: GPIO signals, irqs, hw addr of registers etc
*/
const i3c_master_signal_conn_t i3c_master_periph_signal[SOC_I3C_MASTER_PERIPH_NUM] = {
    {
        .sda_out_sig = I3C_MST_SDA_PAD_OUT_IDX,
        .sda_in_sig = I3C_MST_SDA_PAD_IN_IDX,
        .scl_out_sig = I3C_MST_SCL_PAD_OUT_IDX,
        .scl_in_sig = I3C_MST_SCL_PAD_IN_IDX,
        .irq = ETS_I3C_MST_INTR_SOURCE,
    },
};
