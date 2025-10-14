/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/timer_periph.h"

const soc_timg_gptimer_signal_desc_t soc_timg_gptimer_signals[2][1] = {
    [0] = {
        [0] = {
            .module_name = "TIMG0T0",
            .parent_module = PERIPH_TIMG0_MODULE,
            .irq_id = ETS_TG0_T0_LEVEL_INTR_SOURCE,
        },
    },
    [1] = {
        [0] = {
            .module_name = "TIMG1T0",
            .parent_module = PERIPH_TIMG1_MODULE,
            .irq_id = ETS_TG1_T0_LEVEL_INTR_SOURCE,
        },
    }
};
