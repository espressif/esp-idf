/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
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

    sdmmc_ll_mem_power_by_pmu(hal->dev);
    sdmmc_ll_mem_set_low_power_mode(hal->dev, SDMMC_LL_MEM_LP_MODE_SHUT_DOWN); // power down memory during low power stage
}

void sdmmc_hal_deinit(sdmmc_hal_context_t *hal)
{
    sdmmc_ll_mem_force_low_power(hal->dev);  // power off SDMMC power domain forcefully
    hal->dev = NULL;
}
