/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// btbb sleep retention reg
#define BB_PART_CNT    4
#define BB_PART_0_SIZE 128
#define BB_PART_1_SIZE 68
#define BB_PART_2_SIZE 19
#define BB_PART_3_SIZE 64

#define BB_PART_0_ADDR 0x20102000
#define BB_PART_1_ADDR 0x20102800
#define BB_PART_2_ADDR 0x20102C00
#define BB_PART_3_ADDR 0x20102400



#ifdef __cplusplus
}
#endif
