/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// For newer chips, the highest 2 bits of GPIO_FUNCn_IN_SEL determine the behavior:
//   - If the highest bit is 1 and the second highest bit is 0, it means const 1.
//   - If the highest bit is 1 and the second highest bit is 1, it means const 0.
#define GPIO_MATRIX_CONST_ONE_INPUT   (0x80)
#define GPIO_MATRIX_CONST_ZERO_INPUT  (0xC0)

#ifdef __cplusplus
}
#endif
