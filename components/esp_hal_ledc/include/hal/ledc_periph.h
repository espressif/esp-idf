/*
 * SPDX-FileCopyrightText: 2019-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/interrupts.h"
#if SOC_LEDC_SUPPORTED
#include "hal/ledc_ll.h"
#include "hal/ledc_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_LEDC_SUPPORTED
/*
 Stores a bunch of per-ledc-peripheral data.
*/
typedef struct {
    const int irq_id;
    struct {
        const int sig_out_idx[SOC_LEDC_CHANNEL_NUM];
    } speed_mode[LEDC_SPEED_MODE_MAX];
} ledc_signal_conn_t;

extern const ledc_signal_conn_t ledc_periph_signal[LEDC_LL_GET(GROUP_NUM)];

#endif // SOC_LEDC_SUPPORTED

#ifdef __cplusplus
}
#endif
