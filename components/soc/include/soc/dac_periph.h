/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/sens_reg.h"
#include "soc/sens_struct.h"
#include "soc/rtc_io_reg.h"
#include "soc/rtc_io_struct.h"
#include "soc/rtc.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
    const uint8_t dac_channel_io_num[SOC_DAC_CHAN_NUM];
} dac_signal_conn_t;

extern const dac_signal_conn_t dac_periph_signal;

#ifdef __cplusplus
}
#endif
