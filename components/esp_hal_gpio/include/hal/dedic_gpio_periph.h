/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/interrupts.h"
#include "soc/soc_caps_full.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_HAS(DEDICATED_GPIO)
// helper macros to access module attributes
#define SOC_DEDIC_GPIO_ATTR(_attr)    SOC_MODULE_ATTR(DEDIC_GPIO, _attr)

typedef struct {
    const int irq;                // Interrupt resource (-1 means no interrupt supported)
    struct {
        const int in_sig_per_channel[SOC_DEDIC_GPIO_ATTR(IN_CHANS_PER_CPU)];
        const int out_sig_per_channel[SOC_DEDIC_GPIO_ATTR(OUT_CHANS_PER_CPU)];
    } cores[SOC_CPU_CORES_NUM]; // Signals routed to/from GPIO matrix
} dedic_gpio_signal_conn_t;

extern const dedic_gpio_signal_conn_t dedic_gpio_periph_signals;
#endif // SOC_HAS(DEDICATED_GPIO)

#ifdef __cplusplus
}
#endif
