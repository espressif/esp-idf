/*
 * SPDX-FileCopyrightText: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Check if half-open intervals overlap
 *
 * @param start1  interval 1 start
 * @param end1    interval 1 end
 * @param start2  interval 2 start
 * @param end2    interval 2 end
 * @return true iff [start1; end1) overlaps [start2; end2)
 */
static inline bool bootloader_util_regions_overlap(
        const intptr_t start1, const intptr_t end1,
        const intptr_t start2, const intptr_t end2)
{
    assert(end1>start1);
    assert(end2>start2);
    return (end1 > start2 && end2 > start1);
}

#ifdef __cplusplus
}
#endif
