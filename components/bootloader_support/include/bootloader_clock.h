/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Configure clocks for early boot
 *
 * Called by bootloader, or by the app if the bootloader version is old (pre v2.1).
 */
void bootloader_clock_configure(void);

/** @brief Return the rated maximum frequency of this chip
 */
int bootloader_clock_get_rated_freq_mhz(void);

#ifdef __cplusplus
}
#endif
