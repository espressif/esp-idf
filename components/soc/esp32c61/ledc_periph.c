/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/ledc_periph.h"
#include "soc/gpio_sig_map.h"

/*
 Bunch of constants for every LEDC peripheral: GPIO signals
*/
const ledc_signal_conn_t ledc_periph_signal[1] = {
    {
        .sig_out0_idx = LEDC_LS_SIG_OUT0_IDX,
    }
};
