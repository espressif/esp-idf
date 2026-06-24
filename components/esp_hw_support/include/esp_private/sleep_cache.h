/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Nestable suspend: disable external-mem cache on the first call (refcounted). Critical section; pair with @ref sleep_cache_resume.
 */
void sleep_cache_suspend(void);

/**
 * @brief Nestable resume: restore cache when the refcount returns to zero. Must match @ref sleep_cache_suspend.
 */
void sleep_cache_resume(void);

#if !CONFIG_SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE && CONFIG_SPIRAM
/**
 * @brief Cache writeback before sleep when CPU/cache may power down (SPIRAM / PAU paths; Kconfig-driven).
 * @param sleep_flags Same flags as light sleep, e.g. @c PMU_SLEEP_PD_CPU when relevant.
 */
void sleep_cache_safe_writeback(uint32_t sleep_flags);
#endif

#ifdef __cplusplus
}
#endif
