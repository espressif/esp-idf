/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_attr.h"

/* Static (.bss) placement markers for esp_ble_audio source files. Host-neutral:
 * used by the bluedroid adapter profiles AND the host-agnostic OTS service, so
 * it lives in the audio component's common include (not under bluedroid/).
 *
 * Distinct from esp_ble_iso's utils/iso_attr.h in both name and include prefix, so
 * neither collides on the bt component's shared flat include path.
 *
 *  - BT_AUDIO_EXT_RAM_BSS_ATTR: eligible for external SPIRAM (cold, task-context,
 *    non-DMA). Moves to PSRAM when BT_AUDIO_BSS_SEG_EXTERNAL_MEMORY is set, else
 *    expands to nothing.
 *  - BT_AUDIO_CTRL_BSS_ATTR: must stay in internal DRAM (marker only).
 *
 * net_buf pools (NET_BUF_POOL_FIXED_DEFINE / NET_BUF_SIMPLE_DEFINE_STATIC) hide
 * their .bss backing behind auto-generated names and cannot take a per-variable
 * attribute; those objects (e.g. ots_l2cap) are moved via the linker fragment in
 * linker_common.lf instead.
 */
#if CONFIG_BT_AUDIO_BSS_SEG_EXTERNAL_MEMORY
#define BT_AUDIO_EXT_RAM_BSS_ATTR EXT_RAM_BSS_ATTR
#else
#define BT_AUDIO_EXT_RAM_BSS_ATTR
#endif

#define BT_AUDIO_CTRL_BSS_ATTR
