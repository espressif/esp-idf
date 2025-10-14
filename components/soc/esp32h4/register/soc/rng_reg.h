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

#define DR_REG_RNG_BASE   DR_REG_TRNG_BASE

/** RNG_DATA_REG register
 *  RNG result register
 */
#define RNG_DATA_REG (DR_REG_RNG_BASE + 0x0)
/** RNG_RND_DATA : RO; bitpos: [31:0]; default: 0;
 *  get rng data
 */
#define RNG_RND_DATA    0xFFFFFFFFU
#define RNG_RND_DATA_M  (RNG_RND_DATA_V << RNG_RND_DATA_S)
#define RNG_RND_DATA_V  0xFFFFFFFFU
#define RNG_RND_DATA_S  0

/** RNG_CFG_REG register
 *  configure rng register
 */
#define RNG_CFG_REG (DR_REG_RNG_BASE + 0x4)
/** RNG_SAMPLE_ENABLE : R/W; bitpos: [0]; default: 0;
 *  reserved
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
/** RNG_RTC_TIMER_EN : R/W; bitpos: [11:10]; default: 3;
 *  reserved
 */
#define RNG_RTC_TIMER_EN    0x00000003U
#define RNG_RTC_TIMER_EN_M  (RNG_RTC_TIMER_EN_V << RNG_RTC_TIMER_EN_S)
#define RNG_RTC_TIMER_EN_V  0x00000003U
#define RNG_RTC_TIMER_EN_S  10
/** RNG_SAMPLE_CNT : RO; bitpos: [31:24]; default: 0;
 *  reserved
 */
#define RNG_SAMPLE_CNT    0x000000FFU
#define RNG_SAMPLE_CNT_M  (RNG_SAMPLE_CNT_V << RNG_SAMPLE_CNT_S)
#define RNG_SAMPLE_CNT_V  0x000000FFU
#define RNG_SAMPLE_CNT_S  24

/** RNG_DATA_SYNC_REG register
 *  rng result sync register
 */
#define RNG_DATA_SYNC_REG (DR_REG_RNG_BASE + 0x8)
/** RNG_RND_SYNC_DATA : RO; bitpos: [31:0]; default: 0;
 *  get rnd sync result
 */
#define RNG_RND_SYNC_DATA    0xFFFFFFFFU
#define RNG_RND_SYNC_DATA_M  (RNG_RND_SYNC_DATA_V << RNG_RND_SYNC_DATA_S)
#define RNG_RND_SYNC_DATA_V  0xFFFFFFFFU
#define RNG_RND_SYNC_DATA_S  0

/** RNG_DATE_REG register
 *  Date register.
 */
#define RNG_DATE_REG (DR_REG_RNG_BASE + 0x3fc)
/** RNG_DATE : R/W; bitpos: [30:0]; default: 36708608;
 *  RNG date information/ RNG version information.
 */
#define RNG_DATE    0x7FFFFFFFU
#define RNG_DATE_M  (RNG_DATE_V << RNG_DATE_S)
#define RNG_DATE_V  0x7FFFFFFFU
#define RNG_DATE_S  0
/** RNG_CLK_EN : R/W; bitpos: [31]; default: 0;
 *  force on reg clk
 */
#define RNG_CLK_EN    (BIT(31))
#define RNG_CLK_EN_M  (RNG_CLK_EN_V << RNG_CLK_EN_S)
#define RNG_CLK_EN_V  0x00000001U
#define RNG_CLK_EN_S  31

#ifdef __cplusplus
}
#endif
