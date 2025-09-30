/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Check that freeRTOS headers can be included without the 'freertos/' directory path prefix */
#include "FreeRTOS.h"
#include "task.h"

void app_main(void)
{
}
