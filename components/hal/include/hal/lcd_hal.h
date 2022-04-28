/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lcd_cam_dev_t *lcd_soc_handle_t;

typedef struct {
    lcd_soc_handle_t dev;
} lcd_hal_context_t;

void lcd_hal_init(lcd_hal_context_t *hal, int id);

#ifdef __cplusplus
}
#endif
