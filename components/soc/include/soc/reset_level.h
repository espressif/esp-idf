/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#pragma once

/**
 * Shared reset-level helpers for soc/reset_reasons.h.
 *
 * Prefer including soc/reset_reasons.h. That header defines
 * SOC_RESET_LEVEL_{CPU,CORE,SYSTEM}_MASK and soc_reset_reason_t, then includes
 * this file so soc_reset_get_level() is available.
 *
 * Including this header alone only exposes soc_reset_level_t.
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reset level. Values increase from CPU to chip so callers can compare
 *        with >= (Core and above, System and above).
 *
 * Unknown codes are reported as chip reset so "this level and above" paths
 * take a full re-initialization.
 */
typedef enum {
    SOC_RESET_LEVEL_CPU = 1,
    SOC_RESET_LEVEL_CORE,
    SOC_RESET_LEVEL_SYSTEM,
    SOC_RESET_LEVEL_CHIP,
} soc_reset_level_t;

#if defined(SOC_RESET_LEVEL_CPU_MASK) && defined(SOC_RESET_LEVEL_CORE_MASK) && defined(SOC_RESET_LEVEL_SYSTEM_MASK)

/**
 * @brief Return the reset level encoded in @p reason.
 *
 * Classification uses per-level bitmasks of hardware reset codes (same-value
 * aliases share one bit). Unlisted / out-of-range codes are treated as chip reset.
 */
static inline soc_reset_level_t soc_reset_get_level(soc_reset_reason_t reason)
{
    const unsigned code = (unsigned)reason;
    if (code < 32) {
        const unsigned bit = 1U << code;
        if (bit & SOC_RESET_LEVEL_CPU_MASK) {
            return SOC_RESET_LEVEL_CPU;
        }
        if (bit & SOC_RESET_LEVEL_CORE_MASK) {
            return SOC_RESET_LEVEL_CORE;
        }
        if (bit & SOC_RESET_LEVEL_SYSTEM_MASK) {
            return SOC_RESET_LEVEL_SYSTEM;
        }
    }
    return SOC_RESET_LEVEL_CHIP;
}

#endif /* SOC_RESET_LEVEL_*_MASK */

#ifdef __cplusplus
}
#endif
