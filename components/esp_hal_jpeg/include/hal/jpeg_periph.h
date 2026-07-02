/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char *module_name;   // Module name
    const int irq_id;          // interrupt source ID
} soc_jpeg_signal_desc_t;

extern const soc_jpeg_signal_desc_t soc_jpeg_signals[1];

#ifdef __cplusplus
}
#endif
