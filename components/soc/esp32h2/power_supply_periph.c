/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/power_supply_periph.h"

const power_supply_signal_conn_t power_supply_periph_signal = {
    .irq = ETS_LP_RTC_TIMER_INTR_SOURCE,
};
