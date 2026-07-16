/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_attr.h"

/* Static (.bss) placement markers for the esp_ble_iso component.
 *
 * These sources compile into libbt.a, where object names collide
 * (common/iso.c vs adapter/bluedroid/iso.c vs host/iso/iso.c, etc.), so a
 * linker fragment cannot target them cleanly - placement is done per-variable
 * via these attributes.
 *
 *  - BT_ISO_EXT_RAM_BSS_ATTR: eligible for external SPIRAM (cold or
 *    control-plane, task-context, non-DMA). Moves to PSRAM when
 *    BT_ISO_BSS_SEG_EXTERNAL_MEMORY is set, else expands to nothing. Safe
 *    because esp_ble_iso registers no IRAM ISR (nothing here is touched with
 *    the flash cache disabled).
 *
 *  - BT_ISO_CTRL_BSS_ATTR: must stay in internal DRAM - currently cannot go
 *    to PSRAM. Marker only (expands to nothing = default internal placement).
 *    Used for the real-time ISO data path and anything DMA-reachable or
 *    otherwise unsafe/uncertain in PSRAM. Kernel-object wrappers (k_sem/k_mutex)
 *    are marked CTRL too - synchronization primitives taken on hot/host paths
 *    (incl. the ISO dispatch), kept internal for determinism (and tiny anyway).
 *    net_buf pools and initialized (.data) statistics also stay internal but need
 *    no marker (auto-named .bss cannot take a per-var attribute; .data cannot
 *    use EXT_RAM_BSS).
 *
 * esp_ble_audio has its own equivalent (common/audio_attr.h, BT_AUDIO_* macros),
 * kept separate for the audio layer. The distinct name and include prefix
 * (utils/ here vs common/ there) avoid any collision on the bt component's shared
 * flat include path.
 */
#if CONFIG_BT_ISO_BSS_SEG_EXTERNAL_MEMORY
#define BT_ISO_EXT_RAM_BSS_ATTR EXT_RAM_BSS_ATTR
#else
#define BT_ISO_EXT_RAM_BSS_ATTR
#endif

#define BT_ISO_CTRL_BSS_ATTR
