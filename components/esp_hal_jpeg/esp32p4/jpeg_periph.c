/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/jpeg_periph.h"
#include "soc/interrupts.h"

const soc_jpeg_signal_desc_t soc_jpeg_signals[1] = {
    [0] = {
        .module_name = "JPEG",
        .irq_id = ETS_JPEG_INTR_SOURCE,
    }
};
