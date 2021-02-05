/*
 * Copyright (c) 2016 Intel Corporation
 * Copyright (c) 2016 Wind River Systems, Inc.
 * Additional Copyright (c) 2020 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mesh_kernel.h"

void k_sleep(int32_t duration)
{
    vTaskDelay(duration / portTICK_PERIOD_MS);
}
