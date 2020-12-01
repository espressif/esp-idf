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

#include "soc/mpu_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/* This LL is currently unused for ESP32-C3 - cleanup is TODO ESP32-C3 IDF-2375 */

static inline uint32_t mpu_ll_id_to_addr(int id)
{
    abort();
}

static inline void mpu_ll_set_region_rw(uint32_t addr)
{
    abort();
}

static inline void mpu_ll_set_region_rwx(uint32_t addr)
{
    abort();
}

static inline void mpu_ll_set_region_x(uint32_t addr)
{
    abort();
}


static inline void mpu_ll_set_region_illegal(uint32_t addr)
{
    abort();
}

#ifdef __cplusplus
}
#endif
