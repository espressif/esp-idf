/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// `LP_GPIO_MATRIX_CONST_ONE_INPUT` means connect logic 1 to the LP peripheral signal
#define LP_GPIO_MATRIX_CONST_ONE_INPUT   (0x30)
// `LP_GPIO_MATRIX_CONST_ZERO_INPUT` means connect logic 0 to the LP peripheral signal
#define LP_GPIO_MATRIX_CONST_ZERO_INPUT  (0x20)

#ifdef __cplusplus
}
#endif
