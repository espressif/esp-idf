/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include "hal/sdmmc_hal.h"
#include "hal/sdmmc_ll.h"
#include "soc/soc_caps.h"

void sdmmc_hal_init(sdmmc_hal_context_t *hal)
{
    hal->dev = SDMMC_LL_GET_HW(0);
}
