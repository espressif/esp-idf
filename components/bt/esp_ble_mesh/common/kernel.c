/*
 * SPDX-FileCopyrightText: 2016 Intel Corporation
 * SPDX-FileCopyrightText: 2016 Wind River Systems, Inc.
 * SPDX-FileContributor: 2020-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mesh/timer.h"
#include "mesh/kernel.h"

void k_sleep(int32_t duration)
{
    if (duration < 0 && duration != K_FOREVER) {
        duration = 0;
    }
    vTaskDelay((duration == K_FOREVER) ? portMAX_DELAY : (duration / portTICK_PERIOD_MS));
}
