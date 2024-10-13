/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "soc/periph_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_DEDICATED_GPIO_SUPPORTED
typedef struct {
    const int irq;                // Interrupt resource (-1 means no interrupt supported)
    struct {
        const int in_sig_per_channel[SOC_DEDIC_GPIO_IN_CHANNELS_NUM];
        const int out_sig_per_channel[SOC_DEDIC_GPIO_OUT_CHANNELS_NUM];
    } cores[SOC_CPU_CORES_NUM]; // Signals routed to/from GPIO matrix
} dedic_gpio_signal_conn_t;

extern const dedic_gpio_signal_conn_t dedic_gpio_periph_signals;
#endif // SOC_DEDICATED_GPIO_SUPPORTED

#ifdef __cplusplus
}
#endif
