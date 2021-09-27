/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "hal/lcd_hal.h"
#include "hal/lcd_ll.h"

void lcd_hal_init(lcd_hal_context_t *hal, int id)
{
    hal->dev = LCD_LL_GET_HW(id);
}
