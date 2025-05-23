/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc.h"
#include "soc/timer_periph.h"

const timer_group_signal_conn_t timer_group_periph_signals = {
    .groups = {
        [0] = {
            .module_name = {
                [0] = "TIMG0T0",
                [1] = "TIMG0T1",
            },
            .module = PERIPH_TIMG0_MODULE,
            .timer_irq_id = {
                [0] = ETS_TG0_T0_LEVEL_INTR_SOURCE,
                [1] = ETS_TG0_T1_LEVEL_INTR_SOURCE,
            }
        },
        [1] = {
            .module_name = {
                [0] = "TIMG1T0",
                [1] = "TIMG1T1",
            },
            .module = PERIPH_TIMG1_MODULE,
            .timer_irq_id = {
                [0] = ETS_TG1_T0_LEVEL_INTR_SOURCE,
                [1] = ETS_TG1_T1_LEVEL_INTR_SOURCE,
            }
        }
    }
};
