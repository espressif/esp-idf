/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc.h"
#include "esp_assert.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ESP32-C5 PMP entry layout: a bootloader<->application ABI. An entry the
 * (non-updatable) bootloader locks cannot be reconfigured until CPU reset, so
 * the index of any entry a shipped bootloader locks is frozen.
 *
 * Entries locked by shipped bootloader generations (MP baseline v5.5):
 *   v5.5        : 0, 1-2 (ROM), 15 (peripherals)
 *   v6.0 / v6.1 : 0, 1 (ROM), 15 (peripherals)
 *   >= v6.2     : none
 */
#define PMP_ENTRY_CPU_SUBSYSTEM     0   /* NAPOT RWX */
#define PMP_ENTRY_ROM_LOW           1   /* TOR base */
#define PMP_ENTRY_ROM_HIGH          2   /* TOR R or RX */
/* 3..14: application-owned SRAM/flash/LP-RAM split, programmed by plain index
 * in cpu_region_protect.c; not part of the ABI, not frozen here. */
#define PMP_ENTRY_PERIPHERAL        15  /* NAPOT RW */

ESP_STATIC_ASSERT(PMP_ENTRY_CPU_SUBSYSTEM == 0
               && PMP_ENTRY_ROM_LOW == 1 && PMP_ENTRY_ROM_HIGH == 2
               && PMP_ENTRY_PERIPHERAL == 15,
               "Entries locked by shipped bootloaders are a frozen ABI and must never move");

#ifdef __cplusplus
}
#endif
