/*
 * SPDX-FileCopyrightText: 2019-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/dac_periph.h"

/*
 Bunch of constants for DAC peripheral: GPIO number
*/
const dac_signal_conn_t dac_periph_signal = {
    .dac_channel_io_num[0] = 25,
    .dac_channel_io_num[1] = 26,
};
