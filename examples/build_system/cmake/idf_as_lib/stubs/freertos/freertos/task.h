/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define portTICK_PERIOD_MS  1000

void vTaskDelay( const uint32_t xTicksToDelay );

#ifdef __cplusplus
}
#endif
