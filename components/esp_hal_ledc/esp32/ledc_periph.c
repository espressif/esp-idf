/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/ledc_periph.h"
#include "soc/gpio_sig_map.h"

/*
 Bunch of constants for every LEDC peripheral: GPIO signals
*/
const ledc_signal_conn_t ledc_periph_signal[1] = {
    {
        .irq_id = ETS_LEDC_INTR_SOURCE,
        .speed_mode = {
            [0] = {
                .sig_out_idx = {
                    LEDC_HS_SIG_OUT0_IDX,
                    LEDC_HS_SIG_OUT1_IDX,
                    LEDC_HS_SIG_OUT2_IDX,
                    LEDC_HS_SIG_OUT3_IDX,
                    LEDC_HS_SIG_OUT4_IDX,
                    LEDC_HS_SIG_OUT5_IDX,
                    LEDC_HS_SIG_OUT6_IDX,
                    LEDC_HS_SIG_OUT7_IDX,
                }
            },
            [1] = {
                .sig_out_idx = {
                    LEDC_LS_SIG_OUT0_IDX,
                    LEDC_LS_SIG_OUT1_IDX,
                    LEDC_LS_SIG_OUT2_IDX,
                    LEDC_LS_SIG_OUT3_IDX,
                    LEDC_LS_SIG_OUT4_IDX,
                    LEDC_LS_SIG_OUT5_IDX,
                    LEDC_LS_SIG_OUT6_IDX,
                    LEDC_LS_SIG_OUT7_IDX,
                }
            },
        },
    }
};
