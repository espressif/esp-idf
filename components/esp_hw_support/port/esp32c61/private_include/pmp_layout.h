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
 * ESP32-C61 PMP entry layout: a bootloader<->application ABI. An entry the
 * (non-updatable) bootloader locks cannot be reconfigured until CPU reset, so
 * the index of any entry a shipped bootloader locks is frozen.
 *
 * Entries locked by shipped bootloader generations (MP baseline v5.5):
 *   v5.5        : 0, 1-3 (ROM), 12 (peripherals)
 *   v6.0 / v6.1 : 0, 12 (peripherals)
 *   >= v6.2     : none
 */
#define PMP_ENTRY_CPU_SUBSYSTEM     0   /* NAPOT RWX */
#define PMP_ENTRY_ROM_LOW           1   /* TOR base, or NAPOT RX (unaligned branch) */
#define PMP_ENTRY_ROM_TEXT_HIGH     2   /* TOR RX (aligned branch) */
#define PMP_ENTRY_ROM_HIGH          3   /* TOR R (aligned branch) */
/* 4..11: application-owned SRAM/flash split, programmed by plain index in
 * cpu_region_protect.c; not part of the ABI, not frozen here. */
#define PMP_ENTRY_PERIPHERAL        12  /* NAPOT RW */

ESP_STATIC_ASSERT(PMP_ENTRY_CPU_SUBSYSTEM == 0
               && PMP_ENTRY_ROM_LOW == 1 && PMP_ENTRY_ROM_TEXT_HIGH == 2
               && PMP_ENTRY_ROM_HIGH == 3
               && PMP_ENTRY_PERIPHERAL == 12,
               "Entries locked by shipped bootloaders are a frozen ABI and must never move");

#ifdef __cplusplus
}
#endif
