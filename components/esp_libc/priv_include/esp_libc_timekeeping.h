/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get current realtime in microseconds (adjusted boot time + time_since_boot).
 */
uint64_t esp_libc_timekeeping_get_realtime_us(void);

/**
 * @brief Set realtime to the given epoch in microseconds (stops any slew, sets boot time).
 */
void esp_libc_timekeeping_set_realtime_us(uint64_t us);

/**
 * @brief Get remaining adjtime correction in microseconds.
 *
 * @return Remaining slew in microseconds.
 */
int64_t esp_libc_timekeeping_adjtime_get_remaining_us(void);

/**
 * @brief Apply an adjtime offset (slew) in microseconds.
 *
 * @param offset_us         Offset to apply in microseconds (positive or negative).
 * @param prev_remaining_us On success, set to previous remaining correction in microseconds.
 * @return 0 on success, -1 on error (e.g. invalid offset magnitude).
 */
int esp_libc_timekeeping_adjtime_apply(int64_t offset_us, int64_t *prev_remaining_us);

#ifdef __cplusplus
}
#endif
