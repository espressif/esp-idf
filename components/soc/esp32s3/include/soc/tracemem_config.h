/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define TRACEMEM_MUX_BLK0_NUM                   22
#define TRACEMEM_MUX_BLK1_NUM                   23

#if (TRACEMEM_MUX_BLK0_NUM < 6) || (TRACEMEM_MUX_BLK0_NUM > 29)
#error Invalid TRAX block 0 num!
#endif
#if (TRACEMEM_MUX_BLK1_NUM < 6) || (TRACEMEM_MUX_BLK1_NUM > 29)
#error Invalid TRAX block 1 num!
#endif

#if TRACEMEM_MUX_BLK0_NUM < 6
#define TRACEMEM_BLK0_ADDR                      (0x3FC88000UL + 0x2000UL*(TRACEMEM_MUX_BLK0_NUM-2UL))
#elif TRACEMEM_MUX_BLK0_NUM < 30
#define TRACEMEM_BLK0_ADDR                      (0x3FC90000UL + 0x4000UL*(TRACEMEM_MUX_BLK0_NUM-6UL))
#endif
#if TRACEMEM_MUX_BLK1_NUM < 6
#define TRACEMEM_BLK1_ADDR                      (0x3FC88000UL + 0x2000UL*(TRACEMEM_MUX_BLK1_NUM-2UL))
#elif TRACEMEM_MUX_BLK1_NUM < 30
#define TRACEMEM_BLK1_ADDR                      (0x3FC90000UL + 0x4000UL*(TRACEMEM_MUX_BLK1_NUM-6UL))
#endif

#define TRACEMEM_MUX_BLK_ALLOC(_n_)             (((_n_)-2UL)%4UL)
#define TRACEMEM_CORE0_MUX_BLK_BITS(_n_)         (BIT(((_n_)-2UL)/4UL) | (TRACEMEM_MUX_BLK_ALLOC(_n_) << 14))
#define TRACEMEM_CORE1_MUX_BLK_BITS(_n_)         (BIT(7UL+(((_n_)-2UL)/4UL)) | (TRACEMEM_MUX_BLK_ALLOC(_n_) << 16))

#ifdef __cplusplus
}
#endif
