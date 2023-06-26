/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for PMU

#pragma once

#include "soc/soc_caps.h"
#include "hal/pmu_ll.h"
#include "hal/pmu_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    pmu_dev_t *dev;
} pmu_hal_context_t;

#ifdef __cplusplus
}
#endif
