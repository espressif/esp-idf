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
 * ESP32-C6 PMP entry layout: a bootloader<->application ABI. An entry the
 * (non-updatable) bootloader locks cannot be reconfigured until CPU reset, so
 * the index of any entry a shipped bootloader locks is frozen.
 * The layout below matches v5.2.2..v5.5; older generations that locked a
 * different value on a shared index cannot be helped (their lock wins).
 *
 * Entries locked by shipped bootloader generations:
 *   <= v5.2.1      : 0, 1-2 (I-ROM), 3-4 (D-ROM), 8-9 (cache), 14 (periph)
 *   v5.2.2 .. v5.5 : 0, 1-2 (I-ROM), 15 (peripherals)
 *   v6.0 / v6.1    : 0, 1-2 (I-ROM), 13 (peripherals)
 *   >= v6.2        : none
 */
#define PMP_ENTRY_CPU_SUBSYSTEM     0   /* NAPOT RWX */
#define PMP_ENTRY_IROM_LOW          1   /* TOR base */
#define PMP_ENTRY_IROM_HIGH         2   /* TOR RX */
#define PMP_ENTRY_DROM_LOW          3   /* TOR base (locked by <= v5.2.1) */
#define PMP_ENTRY_DROM_HIGH         4   /* TOR R (locked by <= v5.2.1) */
/* 5..14: application-owned SRAM/flash/LP-RAM split, programmed by plain index
 * in cpu_region_protect.c; not part of the ABI, not frozen here. */
#define PMP_ENTRY_PERIPHERAL        15  /* NAPOT RW */

ESP_STATIC_ASSERT(PMP_ENTRY_CPU_SUBSYSTEM == 0
               && PMP_ENTRY_IROM_LOW == 1 && PMP_ENTRY_IROM_HIGH == 2
               && PMP_ENTRY_DROM_LOW == 3 && PMP_ENTRY_DROM_HIGH == 4
               && PMP_ENTRY_PERIPHERAL == 15,
               "Entries locked by shipped bootloaders are a frozen ABI and must never move");

#ifdef __cplusplus
}
#endif
