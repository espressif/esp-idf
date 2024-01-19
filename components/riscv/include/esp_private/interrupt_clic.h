/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
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
 * If the target is using the CLIC as the interrupt controller, we have 32 external interrupt lines and 16 internal
 * lines. Let's consider the internal ones reserved and not mappable to any handler.
 */
#define RV_EXTERNAL_INT_COUNT   32
#define RV_EXTERNAL_INT_OFFSET  16


FORCE_INLINE_ATTR void assert_valid_rv_int_num(int rv_int_num)
{
    assert(rv_int_num < RV_EXTERNAL_INT_COUNT && "Invalid CPU interrupt number");
}


#ifdef __cplusplus
}
#endif
