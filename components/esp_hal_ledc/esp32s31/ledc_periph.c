/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/ledc_periph.h"
#include "soc/gpio_sig_map.h"
#include "soc/ledc_reg.h"

/*
 Bunch of constants for every LEDC peripheral: GPIO signals
*/
const ledc_signal_conn_t ledc_periph_signal[2] = {
    {
        .irq_id = ETS_LEDC0_INTR_SOURCE,
        .speed_mode = {
            [0] = {
                .sig_out_idx = {
                    LEDC0_LS_SIG_OUT_PAD_OUT0_IDX,
                    LEDC0_LS_SIG_OUT_PAD_OUT1_IDX,
                    LEDC0_LS_SIG_OUT_PAD_OUT2_IDX,
                    LEDC0_LS_SIG_OUT_PAD_OUT3_IDX,
                    LEDC0_LS_SIG_OUT_PAD_OUT4_IDX,
                    LEDC0_LS_SIG_OUT_PAD_OUT5_IDX,
                    LEDC0_LS_SIG_OUT_PAD_OUT6_IDX,
                    LEDC0_LS_SIG_OUT_PAD_OUT7_IDX,
                }
            },
        },
    },
    {
        .irq_id = ETS_LEDC1_INTR_SOURCE,
        .speed_mode = {
            [0] = {
                .sig_out_idx = {
                    LEDC1_LS_SIG_OUT_PAD_OUT0_IDX,
                    LEDC1_LS_SIG_OUT_PAD_OUT1_IDX,
                    LEDC1_LS_SIG_OUT_PAD_OUT2_IDX,
                    LEDC1_LS_SIG_OUT_PAD_OUT3_IDX,
                    LEDC1_LS_SIG_OUT_PAD_OUT4_IDX,
                    LEDC1_LS_SIG_OUT_PAD_OUT5_IDX,
                    LEDC1_LS_SIG_OUT_PAD_OUT6_IDX,
                    LEDC1_LS_SIG_OUT_PAD_OUT7_IDX,
                }
            },
        },
    },
};
