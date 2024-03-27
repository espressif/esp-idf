/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <sys/param.h>
#include "hal/dw_gdma_hal.h"
#include "hal/dw_gdma_ll.h"

void dw_gdma_hal_init(dw_gdma_hal_context_t *hal, const dw_gdma_hal_config_t *config)
{
    hal->dev = DW_GDMA_LL_GET_HW();
    dw_gdma_ll_reset(hal->dev);
    dw_gdma_ll_enable_controller(hal->dev, true);
    dw_gdma_ll_enable_intr_global(hal->dev, true);
}

void dw_gdma_hal_deinit(dw_gdma_hal_context_t *hal)
{
    dw_gdma_ll_enable_intr_global(hal->dev, false);
    dw_gdma_ll_enable_controller(hal->dev, false);
    hal->dev = NULL;
}
