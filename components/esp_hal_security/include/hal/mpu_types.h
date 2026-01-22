/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_MPU_CONFIGURABLE_REGIONS_SUPPORTED
typedef void** mpu_region_table_t;
#endif

typedef enum {
    MPU_REGION_ILLEGAL,
#if SOC_MPU_REGION_RO_SUPPORTED
    MPU_REGION_RO,      // read-only
#endif
#if SOC_MPU_REGION_WO_SUPPORTED
    MPU_REGION_WO,      // write-only
#endif
    MPU_REGION_RW,      // read-write
    MPU_REGION_X,       // executable
    MPU_REGION_RWX      // read-write-executable
} mpu_access_t;

#ifdef __cplusplus
}
#endif
