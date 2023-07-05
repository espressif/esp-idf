/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <stdint.h>

#include "esp_err.h"

#include "hal/mpu_hal.h"
#include "hal/mpu_ll.h"
#include "hal/mpu_types.h"

#include "soc/soc_caps.h"

void mpu_hal_set_region_access(int id, mpu_access_t access)
{
    uint32_t addr = mpu_ll_id_to_addr(id);

    switch (access)
    {
#if SOC_MPU_REGION_RO_SUPPORTED
        case MPU_REGION_RO:
            mpu_ll_set_region_ro(addr);
            break;
#endif
#if SOC_MPU_REGION_WO_SUPPORTED
        case MPU_REGION_WO:
            mpu_ll_set_region_wo(addr);
            break;
#endif
        case MPU_REGION_RW:
            mpu_ll_set_region_rw(addr);
            break;
        case MPU_REGION_X:
            mpu_ll_set_region_x(addr);
            break;
        case MPU_REGION_RWX:
            mpu_ll_set_region_rwx(addr);
            break;
        case MPU_REGION_ILLEGAL:
            mpu_ll_set_region_illegal(addr);
            break;
        default:
            break;
    }
}
