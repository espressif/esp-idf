/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The HAL is not public api, don't use in application code.
 * See readme.md in soc/README.md
 ******************************************************************************/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "soc/gdma_struct.h"

typedef struct {
    gdma_dev_t *dev;
} gdma_hal_context_t;

void gdma_hal_init(gdma_hal_context_t *hal, int group_id);

#ifdef __cplusplus
}
#endif
