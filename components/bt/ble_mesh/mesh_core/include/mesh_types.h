/*
 * Copyright (c) 2017 Linaro Limited
 * Additional Copyright (c) 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __Z_TYPES_H__
#define __Z_TYPES_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef signed char         s8_t;
typedef signed short        s16_t;
typedef signed int          s32_t;
typedef signed long long    s64_t;

typedef unsigned char       u8_t;
typedef unsigned short      u16_t;
typedef unsigned int        u32_t;
typedef unsigned long long  u64_t;

typedef int atomic_t;

#ifndef bool
#define bool        int8_t
#define false       0        ///< XOS definition of 'false'
#define true        1        ///< XOS definition of 'true'
#endif

#ifdef __cplusplus
}
#endif

#endif /* __Z_TYPES_H__ */

