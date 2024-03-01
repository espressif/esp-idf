/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
PROVIDE( systimer_hal_get_counter_value = 0x40000e40 );
PROVIDE( systimer_hal_get_time = 0x40000e4c );
PROVIDE( systimer_hal_set_alarm_target = 0x40000e58 );
PROVIDE( systimer_hal_set_alarm_period = 0x40000e64 );
PROVIDE( systimer_hal_get_alarm_value = 0x40000e70 );
PROVIDE( systimer_hal_enable_alarm_int = 0x40000e7c );
PROVIDE( systimer_hal_on_apb_freq_update = 0x40000e88 );
PROVIDE( systimer_hal_counter_value_advance = 0x40000e94 );
PROVIDE( systimer_hal_enable_counter = 0x40000ea0 );
PROVIDE( systimer_hal_init = 0x40000eac );
PROVIDE( systimer_hal_select_alarm_mode = 0x40000eb8 );
PROVIDE( systimer_hal_connect_alarm_counter = 0x40000ec4 );
