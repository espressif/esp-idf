/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct analog_cmpr_dev_t *analog_cmpr_handle_t; // Analog comparator SOC layer handle

/**
 * HAL context type of analog comparator driver
 */
typedef struct {
    analog_cmpr_handle_t dev;
} analog_cmpr_hal_context_t;

/**
 * @brief Initialize Analog comparator hal driver
 *
 * @param hal Context of the HAL layer
 */
void analog_cmpr_hal_init(analog_cmpr_hal_context_t *hal);

#ifdef __cplusplus
}
#endif
