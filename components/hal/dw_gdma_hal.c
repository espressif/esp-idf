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
}
