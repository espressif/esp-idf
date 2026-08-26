/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "sdkconfig.h"

/*
 * Shared KASAN heap redzone layout helpers for heap_caps_base.c, heap_caps.c,
 * and heap_kasan.c.
 *
 * Allocation layout (with CONFIG_HEAP_TASK_TRACKING=y):
 *
 *   [ block-owner word ][ left redzone ][ user bytes ][ right redzone ]
 *
 * The user-visible pointer points at the start of the user region.
 * KASAN_USER_TO_PTR / KASAN_USER_TO_RAW move back to the left redzone start;
 * KASAN_PTR_TO_USER moves a block-owner-relative pointer to the user region.
 */

#if CONFIG_COMPILER_KASAN && CONFIG_HEAP_USE_HOOKS && (CONFIG_KASAN_HEAP_REDZONE_SIZE > 0)
#define HEAP_KASAN_RZ_ENABLED  1
#else
#define HEAP_KASAN_RZ_ENABLED  0
#endif

#if HEAP_KASAN_RZ_ENABLED
#define KASAN_RZ              CONFIG_KASAN_HEAP_REDZONE_SIZE
#define KASAN_ADD_RZ(sz)      ((sz) + 2 * KASAN_RZ)
#define KASAN_PTR_TO_USER(p)  ((void *)((uint8_t *)(p) + KASAN_RZ))
#define KASAN_USER_TO_PTR(p)  ((void *)((uint8_t *)(p) - KASAN_RZ))
#define KASAN_USER_TO_RAW(p)  KASAN_USER_TO_PTR(p)
#else
#define KASAN_RZ              0
#define KASAN_ADD_RZ(sz)      (sz)
#define KASAN_PTR_TO_USER(p)  (p)
#define KASAN_USER_TO_PTR(p)  (p)
#define KASAN_USER_TO_RAW(p)  (p)
#endif
