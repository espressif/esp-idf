/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include "esp_err.h"

#include "hal/mpu_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Specify the type of access allowed on a memory region.
 *
 * @param id index to the region table; on targets not SOC_MPU_CONFIGURABLE_REGIONS_SUPPORTED,
 * the region divisions is predefined in hardware which is likely reflected in LL implementation.
 * @param access type of access allowed
 */
void mpu_hal_set_region_access(int id, mpu_access_t access);

#ifdef __cplusplus
}
#endif
