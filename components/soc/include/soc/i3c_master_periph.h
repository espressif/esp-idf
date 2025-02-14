/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stdlib.h>
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_I3C_MASTER_SUPPORTED

typedef struct {
    const uint8_t sda_out_sig;
    const uint8_t sda_in_sig;
    const uint8_t scl_out_sig;
    const uint8_t scl_in_sig;
    const uint8_t irq;
} i3c_master_signal_conn_t;

extern const i3c_master_signal_conn_t i3c_master_periph_signal[SOC_I3C_MASTER_PERIPH_NUM];

#endif

#ifdef __cplusplus
}
#endif
