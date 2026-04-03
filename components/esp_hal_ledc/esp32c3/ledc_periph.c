/*
 * SPDX-FileCopyrightText: 2020-2026 Espressif Systems (Shanghai) CO LTD
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
            [0] = { .sig_out0_idx = LEDC_LS_SIG_OUT0_IDX },
        },
    }
};
