/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set the leak threshold value for the specific test case
 *
 * @param threshold  Maximum allowed memory leak in bytes
 */
void set_leak_threshold(int threshold);

#ifdef __cplusplus
}
#endif
