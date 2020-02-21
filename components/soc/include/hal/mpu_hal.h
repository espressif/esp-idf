// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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