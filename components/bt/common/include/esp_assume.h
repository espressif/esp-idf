/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <assert.h>

#if !defined(ESP_ASSUME_NONNULL)
#if defined(CONFIG_COMPILER_OPTIMIZATION_DEBUG) || defined(CONFIG_BT_NIMBLE_DEBUG) || !defined(NDEBUG)
/* Debug: fail fast if upstream contract is broken */
#define ESP_ASSUME_NONNULL(p) do { assert((p) != NULL); } while (0)
#else
/* Release: contract only (no extra code size) */
#define ESP_ASSUME_NONNULL(p) do { (void)(p); } while (0)
#endif
#endif
