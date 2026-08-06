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
 * ESP32-P4 PMP entry layout: a bootloader<->application ABI. An entry the
 * (non-updatable) bootloader locks cannot be reconfigured until CPU reset, so
 * the index of any entry a shipped bootloader locks is frozen.
 * Two incompatible layouts selected by chip revision (>= 3.0); binaries do not
 * mix, so each has its own index namespace. Entries 0-2 are shared.
 *
 * Locked by shipped bootloader generations:
 *   rev < 3.0  v5.3/v5.4 : 0-2 (ROM), 13 (periph);  v5.5..v6.1 : 0-2, 15
 *   rev >= 3.0 v6.0/v6.1 : 0-3 (ROM cached+uncached), 27 (periph)
 *   >= v6.2              : none
 */
#define PMP_ENTRY_CPU_SUBSYSTEM         0   /* NAPOT RW, both layouts */
#define PMP_ENTRY_CPU_PERIPHERAL        1   /* NAPOT RW, both layouts */
#define PMP_ENTRY_ROM                   2   /* NAPOT RX, both layouts */

/* rev < 3.0 has two application layouts, chosen at runtime by probing which
 * peripheral entry the bootloader locked (see cpu_region_protect.c):
 *   - default         : SRAM/flash 3-8, LP-RAM 9-12, peripheral 13; 14-15 free.
 *   - PSRAM-protected : SRAM/flash/ext-RAM 3-10, LP-RAM 11-14, peripheral 15.
 * The PSRAM layout is taken only on v5.5+ bootloaders (which leave 13 free and
 * lock the peripheral at 15); a v5.3/v5.4 bootloader locks the peripheral at 13,
 * forcing the default layout. Entries 13 and 15 are both peripheral homes locked
 * by shipped bootloaders and must never be repurposed for a non-peripheral region. */
#define PMP_ENTRY_PERIPHERAL_REV_LESS_THAN_V3               13  /* NAPOT RW, default layout */
#define PMP_ENTRY_PERIPHERAL_REV_LESS_THAN_V3_SPIRAM_MEMPROT_EN  15  /* NAPOT RW, PSRAM layout */

/* rev >= 3.0: 4..26 (SRAM/ext-RAM/flash cached+uncached aliases) and 28..31
 * (LP) are application-owned; their indices shift with the memprot config, so
 * they are programmed by plain index in cpu_region_protect.c, not named here. */
#define PMP_ENTRY_ROM_UNCACHED_REV_V3       3   /* NAPOT RX */
#define PMP_ENTRY_PERIPHERAL_REV_V3     27  /* NAPOT RW */

ESP_STATIC_ASSERT(PMP_ENTRY_CPU_SUBSYSTEM == 0 && PMP_ENTRY_CPU_PERIPHERAL == 1
               && PMP_ENTRY_ROM == 2,
               "Shared entries are a frozen ABI and must never move");
ESP_STATIC_ASSERT(PMP_ENTRY_PERIPHERAL_REV_LESS_THAN_V3 == 13
               && PMP_ENTRY_PERIPHERAL_REV_LESS_THAN_V3_SPIRAM_MEMPROT_EN == 15,
               "rev < 3.0 peripheral entries are locked by shipped bootloaders and must never move");
ESP_STATIC_ASSERT(PMP_ENTRY_ROM_UNCACHED_REV_V3 == 3 && PMP_ENTRY_PERIPHERAL_REV_V3 == 27,
               "rev >= 3.0 entries locked by shipped bootloaders are a frozen ABI and must never move");

#ifdef __cplusplus
}
#endif
