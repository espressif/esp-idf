/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/interrupts.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_HAS(DEDICATED_GPIO)
#include "hal/dedic_gpio_caps.h"

typedef struct {
    const int irq;                // Interrupt resource (-1 means no interrupt supported)
    struct {
        const int in_sig_per_channel[DEDIC_GPIO_CAPS_GET(IN_CHANS_PER_CPU)];
        const int out_sig_per_channel[DEDIC_GPIO_CAPS_GET(OUT_CHANS_PER_CPU)];
    } cores[SOC_CPU_CORES_NUM]; // Signals routed to/from GPIO matrix
} dedic_gpio_signal_conn_t;

extern const dedic_gpio_signal_conn_t dedic_gpio_periph_signals;
#endif // SOC_HAS(DEDICATED_GPIO)

#ifdef __cplusplus
}
#endif
