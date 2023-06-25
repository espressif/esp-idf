/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if SOC_DAC_SUPPORTED

typedef struct {
    const uint8_t dac_channel_io_num[SOC_DAC_CHAN_NUM];
} dac_signal_conn_t;

extern const dac_signal_conn_t dac_periph_signal;

#endif // SOC_DAC_SUPPORTED

#ifdef __cplusplus
}
#endif
