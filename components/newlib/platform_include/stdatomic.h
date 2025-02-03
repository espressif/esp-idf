/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#include_next "stdatomic.h"
#ifndef __clang__
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline bool __atomic_test_and_set(volatile void *ptr, int memorder)
{
    return __atomic_exchange_1((bool *)ptr, true, memorder);
}

#ifdef __cplusplus
}
#endif

#endif
