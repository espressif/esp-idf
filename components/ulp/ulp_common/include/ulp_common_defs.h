/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __ULP_COMMON_DEFS_H__
#define __ULP_COMMON_DEFS_H__

#include "soc/soc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RTC_SLOW_MEM ((uint32_t*) SOC_RTC_DATA_LOW)

#ifdef __cplusplus
}
#endif

#endif // __ULP_COMMON_DEFS_H__
