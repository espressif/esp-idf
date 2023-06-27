/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_SDM_SUPPORTED

typedef struct {
    struct {
        const int sd_sig;
    } channels[SOC_SDM_CHANNELS_PER_GROUP];
} sigma_delta_signal_conn_t;

extern const sigma_delta_signal_conn_t sigma_delta_periph_signals;

#endif // SOC_SDM_SUPPORTED

#ifdef __cplusplus
}
#endif
