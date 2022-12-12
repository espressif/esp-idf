/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/timer_periph.h"

const timer_group_signal_conn_t timer_group_periph_signals = {
    .groups = {
        [0] = {
            .module = PERIPH_TIMG0_MODULE,
            .timer_irq_id = {
                [0] = ETS_TG0_T0_LEVEL_INTR_SOURCE,
            }
        },
        [1] = {
            .module = PERIPH_TIMG1_MODULE,
            .timer_irq_id = {
                [0] = ETS_TG1_T0_LEVEL_INTR_SOURCE,
            }
        }
    }
};
