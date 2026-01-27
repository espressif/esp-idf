/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32s31.rom.systimer.ld for esp32s31
 *
 *
 * Generated from ../../rom/target/esp32s31/interface-esp32s31.yml md5sum 0cb775308c66eebf0757e406021ad5be
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group hal_systimer
 ***************************************/

/* Functions */
systimer_hal_init = 0x2f800394;
systimer_hal_deinit = 0x2f800398;
systimer_hal_set_tick_rate_ops = 0x2f80039c;
systimer_hal_get_counter_value = 0x2f8003a0;
systimer_hal_get_time = 0x2f8003a4;
systimer_hal_set_alarm_target = 0x2f8003a8;
systimer_hal_set_alarm_period = 0x2f8003ac;
systimer_hal_get_alarm_value = 0x2f8003b0;
systimer_hal_enable_alarm_int = 0x2f8003b4;
systimer_hal_on_apb_freq_update = 0x2f8003b8;
systimer_hal_counter_value_advance = 0x2f8003bc;
systimer_hal_enable_counter = 0x2f8003c0;
systimer_hal_select_alarm_mode = 0x2f8003c4;
systimer_hal_connect_alarm_counter = 0x2f8003c8;
systimer_hal_counter_can_stall_by_cpu = 0x2f8003cc;
