/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/dac_periph.h"

/*
 Bunch of constants for DAC peripheral: GPIO number
*/
const dac_signal_conn_t dac_periph_signal = {
    .dac_channel_io_num[0] = DAC_CHAN_0_GPIO_NUM,
    .dac_channel_io_num[1] = DAC_CHAN_1_GPIO_NUM,
};
