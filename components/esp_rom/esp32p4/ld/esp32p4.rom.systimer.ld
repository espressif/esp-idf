/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


/***************************************
 Group hal_systimer
 ***************************************/

/* Functions */
/* The following ROM functions are commented out because they're patched in the esp_rom_systimer.c */
/* systimer_hal_init = 0x4fc00228; */
/* systimer_hal_deinit = 0x4fc0022c; */
systimer_hal_set_tick_rate_ops = 0x4fc00230;
systimer_hal_get_counter_value = 0x4fc00234;
systimer_hal_get_time = 0x4fc00238;
systimer_hal_set_alarm_target = 0x4fc0023c;
systimer_hal_set_alarm_period = 0x4fc00240;
systimer_hal_get_alarm_value = 0x4fc00244;
systimer_hal_enable_alarm_int = 0x4fc00248;
systimer_hal_on_apb_freq_update = 0x4fc0024c;
systimer_hal_counter_value_advance = 0x4fc00250;
systimer_hal_enable_counter = 0x4fc00254;
systimer_hal_select_alarm_mode = 0x4fc00258;
systimer_hal_connect_alarm_counter = 0x4fc0025c;
systimer_hal_counter_can_stall_by_cpu = 0x4fc00260;
