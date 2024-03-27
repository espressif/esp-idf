/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Spinlock performance on esp32p4 is slower. */
#define IDF_PERFORMANCE_MAX_FREERTOS_SPINLOCK_CYCLES_PER_OP                     400
#define IDF_PERFORMANCE_MAX_FREERTOS_SPINLOCK_CYCLES_PER_OP_UNICORE             150

/* Solicited yields (portYIELD() or taskYIELD()) take longer on esp32p4. TODO: IDF-2809 */
#define IDF_PERFORMANCE_MAX_SCHEDULING_TIME                                     3200
