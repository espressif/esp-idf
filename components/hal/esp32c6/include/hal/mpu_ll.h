/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Copied from C3, and this LL is currently unused for ESP32-C3 */

static inline uint32_t mpu_ll_id_to_addr(unsigned id)
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
