/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32h4.rom.systimer.ld for esp32h4
 *
 *
 * Generated from ../../rom/target/esp32h4/interface-esp32h4.yml md5sum c9db9928804fe99070d282b058ff3c65
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group hal_systimer
 ***************************************/

/* Functions */
systimer_hal_init = 0x4000036c;
systimer_hal_deinit = 0x40000370;
systimer_hal_set_tick_rate_ops = 0x40000374;
systimer_hal_get_counter_value = 0x40000378;
systimer_hal_get_time = 0x4000037c;
systimer_hal_set_alarm_target = 0x40000380;
systimer_hal_set_alarm_period = 0x40000384;
systimer_hal_get_alarm_value = 0x40000388;
systimer_hal_enable_alarm_int = 0x4000038c;
systimer_hal_on_apb_freq_update = 0x40000390;
systimer_hal_counter_value_advance = 0x40000394;
systimer_hal_enable_counter = 0x40000398;
systimer_hal_select_alarm_mode = 0x4000039c;
systimer_hal_connect_alarm_counter = 0x400003a0;
systimer_hal_counter_can_stall_by_cpu = 0x400003a4;
