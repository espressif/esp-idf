/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#include_next <stdatomic.h>

/* The ATOMIC_VAR_INIT macro was deprecated in:
 *  - C17
 *  - C++20
 * and removed in subsequent standards.
 * Since users may change the standard version for their projects,
 * IDF should remain compatible across different standards.
 */
#if __STDC_VERSION__ > 201710L || __cplusplus > 202002L
# ifndef ATOMIC_VAR_INIT
#   define ATOMIC_VAR_INIT(val) (val)
# endif
#endif
