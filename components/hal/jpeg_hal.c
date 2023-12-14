/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include "hal/jpeg_hal.h"
#include "hal/jpeg_ll.h"

void jpeg_hal_init(jpeg_hal_context_t *hal)
{
    hal->dev = JPEG_LL_GET_HW();
}

void jpeg_hal_deinit(jpeg_hal_context_t *hal)
{
    hal->dev = NULL;
}
