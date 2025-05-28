/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/***************************************
 Group hal_systimer
 ***************************************/

/* Functions */
systimer_hal_init = 0x400003d0;
systimer_hal_deinit = 0x400003d4;
systimer_hal_set_tick_rate_ops = 0x400003d8;
systimer_hal_get_counter_value = 0x400003dc;
systimer_hal_get_time = 0x400003e0;
systimer_hal_set_alarm_target = 0x400003e4;
systimer_hal_set_alarm_period = 0x400003e8;
systimer_hal_get_alarm_value = 0x400003ec;
systimer_hal_enable_alarm_int = 0x400003f0;
systimer_hal_on_apb_freq_update = 0x400003f4;
systimer_hal_counter_value_advance = 0x400003f8;
systimer_hal_enable_counter = 0x400003fc;
systimer_hal_select_alarm_mode = 0x40000400;
systimer_hal_connect_alarm_counter = 0x40000404;
systimer_hal_counter_can_stall_by_cpu = 0x40000408;
