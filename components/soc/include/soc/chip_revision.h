/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Convenient macros to check current wafer version against a version where some changes are introduced.
 * Use `ESP_CHIP_REV_ABOVE` for a change introduced before any major versions.
 * Use `ESP_CHIP_REV_MAJOR_AND_ABOVE` for changes introduced after a major version is added.
 * For example, on ESP32 we have wafer versions:
 *
 * 0.0 -> 1.0 -> 2.0 -> 3.0 -> 3.1 -> N.A.
 *            |->1.1
 *
 * - If we are adding code for a change on 1.1, we should use `ESP_CHIP_REV_MAJOR_AND_ABOVE`
 *   because there is already major version 2 existing. The condition will be met from 1.1 to 1.99,
 *   while not inherited by 2.0 and above.
 *
 * - If we are adding code for a change on 3.1, we should use `ESP_CHIP_REV_ABOVE`
 *   because there is no major version 4. The condition will be met from 3.1 to 3.99 and 4.0 and above.
 *   Even if we add revision 4.0 on this version, the logic will be inherited.
 */

#define ESP_CHIP_REV_ABOVE(rev, min_rev) ((min_rev) <= (rev))
#define ESP_CHIP_REV_MAJOR_AND_ABOVE(rev, min_rev) (((rev) / 100 == (min_rev) / 100) && ((rev) >= (min_rev)))

#ifdef __cplusplus
}
#endif
