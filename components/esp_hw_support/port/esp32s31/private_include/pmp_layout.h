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
 * ESP32-S31 PMP entry layout: a bootloader <-> application ABI.
 *
 * A PMP entry that the (non-updatable) second stage bootloader locks cannot be reconfigured until
 * the next CPU reset, so the index of any entry a shipped bootloader locks would be frozen for the
 * lifetime of that device. To avoid such drift, the ESP-IDF bootloader for ESP32-S31 configures no
 * PMP entry at all (it configures only PMA); the application programs and locks the entire PMP
 * layout during early startup (see cpu_region_protect.c).
 *
 * No bootloader that locks a PMP entry has ever shipped for this target, so today no index is
 * constrained by a deployed device. The boundary entries below are nonetheless frozen as a
 * forward-compatibility contract and pinned with ESP_STATIC_ASSERTs, so a locked entry a future
 * bootloader or a custom (non-ESP-IDF) application might rely on can never be silently renumbered.
 *
 * Frozen entries:
 *   0        : CPU subsystem (NAPOT RWX)
 *   1 - 2    : I/D-ROM (ROM-Cache) TOR pair
 *   14 - 15  : peripherals TOR pair (on-chip peripherals, CPU peripheral, cache-data memory,
 *              LP peripherals and the debug address space)
 *
 * Entries 3 - 13 are application-owned (the SRAM IRAM/DRAM split, the flash I/D-cache split, the
 * external PSRAM region and the LP-TCM split). Their count varies with the build configuration,
 * they are programmed by plain index in cpu_region_protect.c, and they are NOT part of the ABI -
 * they may change between versions.
 */
#define PMP_ENTRY_CPU_SUBSYSTEM     0
#define PMP_ENTRY_ROM_LOW           1
#define PMP_ENTRY_ROM_HIGH          2
#define PMP_ENTRY_PERIPHERAL_LOW    14
#define PMP_ENTRY_PERIPHERAL_HIGH   15

ESP_STATIC_ASSERT(PMP_ENTRY_CPU_SUBSYSTEM == 0
                  && PMP_ENTRY_ROM_LOW == 1
                  && PMP_ENTRY_ROM_HIGH == 2
                  && PMP_ENTRY_PERIPHERAL_LOW == 14
                  && PMP_ENTRY_PERIPHERAL_HIGH == 15,
                  "ESP32-S31 PMP entries that a bootloader may lock are a frozen ABI and must never move");

#ifdef __cplusplus
}
#endif
