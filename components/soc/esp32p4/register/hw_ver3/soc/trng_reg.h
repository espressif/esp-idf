/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** RNG_CFG_REG register
 *  configure rng register
 */
#define RNG_CFG_REG (DR_REG_RNG_BASE + 0x0)
/** RNG_SAMPLE_ENABLE : R/W; bitpos: [0]; default: 0;
 *  enable rng RO
 *  1: enable RO
 *  0: disable RO
 */
#define RNG_SAMPLE_ENABLE    (BIT(0))
#define RNG_SAMPLE_ENABLE_M  (RNG_SAMPLE_ENABLE_V << RNG_SAMPLE_ENABLE_S)
#define RNG_SAMPLE_ENABLE_V  0x00000001U
#define RNG_SAMPLE_ENABLE_S  0
/** RNG_TIMER_PSCALE : R/W; bitpos: [8:1]; default: 255;
 *  configure rng timer clk div
 */
#define RNG_TIMER_PSCALE    0x000000FFU
#define RNG_TIMER_PSCALE_M  (RNG_TIMER_PSCALE_V << RNG_TIMER_PSCALE_S)
#define RNG_TIMER_PSCALE_V  0x000000FFU
#define RNG_TIMER_PSCALE_S  1
/** RNG_TIMER_EN : R/W; bitpos: [9]; default: 1;
 *  enable rng xor async rng timer
 */
#define RNG_TIMER_EN    (BIT(9))
#define RNG_TIMER_EN_M  (RNG_TIMER_EN_V << RNG_TIMER_EN_S)
#define RNG_TIMER_EN_V  0x00000001U
#define RNG_TIMER_EN_S  9
/** RNG_SAMPLE_CNT : RO; bitpos: [31:24]; default: 0;
 *  get rng RO sample cnt
 */
#define RNG_SAMPLE_CNT    0x000000FFU
#define RNG_SAMPLE_CNT_M  (RNG_SAMPLE_CNT_V << RNG_SAMPLE_CNT_S)
#define RNG_SAMPLE_CNT_V  0x000000FFU
#define RNG_SAMPLE_CNT_S  24

/** RNG_DATA_REG register
 *  RNG result register
 */
#define RNG_DATA_REG (DR_REG_RNG_BASE + 0x4)
/** RNG_DATA : RO; bitpos: [31:0]; default: 0;
 *  get rng data
 */
#define RNG_DATA    0xFFFFFFFFU
#define RNG_DATA_M  (RNG_DATA_V << RNG_DATA_S)
#define RNG_DATA_V  0xFFFFFFFFU
#define RNG_DATA_S  0

/** RNG_RSTN_REG register
 *  rng rstn register
 */
#define RNG_RSTN_REG (DR_REG_RNG_BASE + 0x8)
/** RNG_RSTN : R/W; bitpos: [0]; default: 1;
 *  enable rng system reset: 1: not reset, 0: reset
 */
#define RNG_RSTN    (BIT(0))
#define RNG_RSTN_M  (RNG_RSTN_V << RNG_RSTN_S)
#define RNG_RSTN_V  0x00000001U
#define RNG_RSTN_S  0

/** RNG_DATE_REG register
 *  need_des
 */
#define RNG_DATE_REG (DR_REG_RNG_BASE + 0xc)
/** RNG_DATE : R/W; bitpos: [30:0]; default: 2425091;
 *  need_des
 */
#define RNG_DATE    0x7FFFFFFFU
#define RNG_DATE_M  (RNG_DATE_V << RNG_DATE_S)
#define RNG_DATE_V  0x7FFFFFFFU
#define RNG_DATE_S  0
/** RNG_CLK_EN : R/W; bitpos: [31]; default: 0;
 *  need_des
 */
#define RNG_CLK_EN    (BIT(31))
#define RNG_CLK_EN_M  (RNG_CLK_EN_V << RNG_CLK_EN_S)
#define RNG_CLK_EN_V  0x00000001U
#define RNG_CLK_EN_S  31

#ifdef __cplusplus
}
#endif
