/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Spinlock performance on esp32p4 is slower. May need to adjust these values once IDF-7898 is fixed */
#define IDF_PERFORMANCE_MAX_FREERTOS_SPINLOCK_CYCLES_PER_OP                     380
#define IDF_PERFORMANCE_MAX_FREERTOS_SPINLOCK_CYCLES_PER_OP_UNICORE             135

/* Solicited yields (portYIELD() or taskYIELD()) take longer on esp32p4. TODO: IDF-2809 */
#define IDF_PERFORMANCE_MAX_SCHEDULING_TIME                                     2900
