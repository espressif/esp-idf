/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>

/* Heap allocation policy for LE Audio. Every runtime allocation should use one
 * of these two so intent is explicit (a bare malloc/calloc reads as "not yet
 * classified" and risks being rerouted by a later PSRAM sweep). Both pin the
 * allocation to a memory type with heap_caps, independent of the platform's
 * default malloc policy (which, under CONFIG_SPIRAM_USE_MALLOC, may itself hand
 * out PSRAM).
 *
 *  - bt_le_ext_*: control-plane data (GATT/GAP event marshalling, profile setup,
 *    per-conn state, ...). Placed in external SPIRAM (MALLOC_CAP_SPIRAM) when
 *    BT_ISO_HEAP_EXTERNAL_MEMORY is set, else the internal heap. Returns NULL on
 *    SPIRAM exhaustion (no internal fallback).
 *
 *  - bt_le_int_*: the real-time ISO TX/RX data path (SDU packet buffers handed to
 *    the controller, per-SDU nodes/events). Forced to internal DRAM
 *    (MALLOC_CAP_INTERNAL): controller/DMA-reachable and latency-critical, so it
 *    must never land in PSRAM. Plain malloc/calloc would NOT guarantee this -
 *    under CONFIG_SPIRAM_USE_MALLOC they can return PSRAM.
 *
 * Both are freed with plain free() (valid for heap_caps allocations in ESP-IDF).
 */
void *bt_le_ext_malloc(size_t size);
void *bt_le_ext_calloc(size_t n, size_t size);
void *bt_le_int_malloc(size_t size);
void *bt_le_int_calloc(size_t n, size_t size);
