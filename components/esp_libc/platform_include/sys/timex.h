/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Minimal subset of <sys/timex.h> for clock_adjtime() support.
 * Original struct timex and ADJ_* constants by David L. Mills (1993).
 *
 * Only the modes, offset, and freq fields are implemented.
 * freq uses ppb (parts per billion) units instead of Linux's scaled ppm.
 */

#define ADJ_OFFSET              0x0001 /* time offset; microseconds by default, nanoseconds with ADJ_NANO */
#define ADJ_FREQUENCY           0x0002 /* frequency offset in ppb (parts per billion) */
#define ADJ_MICRO               0x1000 /* select microsecond resolution */
#define ADJ_NANO                0x2000 /* select nanosecond resolution */
#define ADJ_OFFSET_SINGLESHOT   0x8001 /* old-fashioned adjtime (offset in microseconds)*/
#define ADJ_OFFSET_SS_READ      0xa001 /* read-only adjtime (remaining offset in microseconds) */

struct timex {
    int  modes;     /*!< Mode selector (ADJ_OFFSET, ADJ_FREQUENCY, etc.) */
    long offset;    /*!< Time offset: microseconds by default, nanoseconds when ADJ_NANO is set */
    long freq;      /*!< Frequency offset in ppb (parts per billion) */
};

/**
 * @brief Adjust a clock identified by clk_id
 *
 * Uses struct timex.modes to select which fields to apply:
 *   - ADJ_OFFSET:           gradually slew by buf->offset
 *   - ADJ_FREQUENCY:        set frequency trim to buf->freq ppb
 *   - ADJ_OFFSET_SINGLESHOT: old-style adjtime(), buf->offset
 *   - ADJ_OFFSET_SS_READ:   read remaining adjtime() correction into buf->offset
 *
 * buf->offset uses microseconds by default. Set ADJ_NANO in modes to use
 * nanoseconds instead. ADJ_MICRO may be used to explicitly select the default.
 *
 * ADJ_OFFSET and ADJ_FREQUENCY are mutually exclusive; do not OR them together.
 *
 * @param clk_id  Clock identifier (CLOCK_REALTIME, or a custom registered clock)
 * @param buf     Pointer to struct timex with modes and values to apply.
 *                On return, buf->offset may be updated (e.g. ADJ_OFFSET_SS_READ).
 *
 * @return
 *     - 0: Success
 *     - -1: Failure (errno is set)
 */
int clock_adjtime(clockid_t clk_id, struct timex *buf);

#ifdef __cplusplus
}
#endif
