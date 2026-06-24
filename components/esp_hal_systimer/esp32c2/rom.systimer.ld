/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
PROVIDE( systimer_hal_get_counter_value = 0x400002ac );
PROVIDE( systimer_hal_get_alarm_value = 0x400002bc );
PROVIDE( systimer_hal_enable_alarm_int = 0x400002c0 );
PROVIDE( systimer_hal_enable_counter = 0x400002cc );
PROVIDE( systimer_hal_select_alarm_mode = 0x400002d0 );
PROVIDE( systimer_hal_connect_alarm_counter = 0x400002d4 );
PROVIDE( systimer_hal_counter_can_stall_by_cpu = 0x400002d8 );

/* The following ROM functions are commented out because they're patched in the rom_patch.c */
/* PROVIDE( systimer_hal_init = 0x400002a8 ); */
/* PROVIDE( systimer_hal_get_time = 0x400002b0 ); */
/* PROVIDE( systimer_hal_set_alarm_target = 0x400002b4 ); */
/* PROVIDE( systimer_hal_set_alarm_period = 0x400002b8 ); */
/* PROVIDE( systimer_hal_counter_value_advance = 0x400002c8 ); */
