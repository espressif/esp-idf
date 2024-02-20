/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once


#include <stdint.h>
#include <assert.h>
#include "esp_attr.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * In the case of INTC, all the interrupt lines are dedicated to external peripherals, so the offset is 0
 */
#define RV_EXTERNAL_INT_COUNT   32
#define RV_EXTERNAL_INT_OFFSET  0


FORCE_INLINE_ATTR void assert_valid_rv_int_num(int rv_int_num)
{
    assert(rv_int_num != 0 && "Invalid CPU interrupt number");
}


#ifdef __cplusplus
}
#endif
