/**
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** PCNT_U0_CONF0_REG register
 *  Configuration register 0 for unit 0
 */
#define PCNT_U0_CONF0_REG (DR_REG_PCNT_BASE + 0x0)
/** PCNT_FILTER_THRES_U0 : R/W; bitpos: [9:0]; default: 16;
 *  This sets the maximum threshold, in APB_CLK cycles, for the filter.
 *
 *  Any pulses with width less than this will be ignored when the filter is enabled.
 */
#define PCNT_FILTER_THRES_U0    0x000003FFU
#define PCNT_FILTER_THRES_U0_M  (PCNT_FILTER_THRES_U0_V << PCNT_FILTER_THRES_U0_S)
#define PCNT_FILTER_THRES_U0_V  0x000003FFU
#define PCNT_FILTER_THRES_U0_S  0
/** PCNT_FILTER_EN_U0 : R/W; bitpos: [10]; default: 1;
 *  This is the enable bit for unit 0's input filter.
 */
#define PCNT_FILTER_EN_U0    (BIT(10))
#define PCNT_FILTER_EN_U0_M  (PCNT_FILTER_EN_U0_V << PCNT_FILTER_EN_U0_S)
#define PCNT_FILTER_EN_U0_V  0x00000001U
#define PCNT_FILTER_EN_U0_S  10
/** PCNT_THR_ZERO_EN_U0 : R/W; bitpos: [11]; default: 1;
 *  This is the enable bit for unit 0's zero comparator.
 */
#define PCNT_THR_ZERO_EN_U0    (BIT(11))
#define PCNT_THR_ZERO_EN_U0_M  (PCNT_THR_ZERO_EN_U0_V << PCNT_THR_ZERO_EN_U0_S)
#define PCNT_THR_ZERO_EN_U0_V  0x00000001U
#define PCNT_THR_ZERO_EN_U0_S  11
/** PCNT_THR_H_LIM_EN_U0 : R/W; bitpos: [12]; default: 1;
 *  This is the enable bit for unit 0's thr_h_lim comparator. Configures it to enable
 *  the high limit interrupt.
 */
#define PCNT_THR_H_LIM_EN_U0    (BIT(12))
#define PCNT_THR_H_LIM_EN_U0_M  (PCNT_THR_H_LIM_EN_U0_V << PCNT_THR_H_LIM_EN_U0_S)
#define PCNT_THR_H_LIM_EN_U0_V  0x00000001U
#define PCNT_THR_H_LIM_EN_U0_S  12
/** PCNT_THR_L_LIM_EN_U0 : R/W; bitpos: [13]; default: 1;
 *  This is the enable bit for unit 0's thr_l_lim comparator. Configures it to enable
 *  the low limit interrupt.
 */
#define PCNT_THR_L_LIM_EN_U0    (BIT(13))
#define PCNT_THR_L_LIM_EN_U0_M  (PCNT_THR_L_LIM_EN_U0_V << PCNT_THR_L_LIM_EN_U0_S)
#define PCNT_THR_L_LIM_EN_U0_V  0x00000001U
#define PCNT_THR_L_LIM_EN_U0_S  13
/** PCNT_THR_THRES0_EN_U0 : R/W; bitpos: [14]; default: 0;
 *  This is the enable bit for unit 0's thres0 comparator.
 */
#define PCNT_THR_THRES0_EN_U0    (BIT(14))
#define PCNT_THR_THRES0_EN_U0_M  (PCNT_THR_THRES0_EN_U0_V << PCNT_THR_THRES0_EN_U0_S)
#define PCNT_THR_THRES0_EN_U0_V  0x00000001U
#define PCNT_THR_THRES0_EN_U0_S  14
/** PCNT_THR_THRES1_EN_U0 : R/W; bitpos: [15]; default: 0;
 *  This is the enable bit for unit 0's thres1 comparator.
 */
#define PCNT_THR_THRES1_EN_U0    (BIT(15))
#define PCNT_THR_THRES1_EN_U0_M  (PCNT_THR_THRES1_EN_U0_V << PCNT_THR_THRES1_EN_U0_S)
#define PCNT_THR_THRES1_EN_U0_V  0x00000001U
#define PCNT_THR_THRES1_EN_U0_S  15
/** PCNT_CH0_NEG_MODE_U0 : R/W; bitpos: [17:16]; default: 0;
 *  This register sets the behavior when the signal input of channel 0 detects a
 *  negative edge.
 *
 *  1: Increase the counter.2: Decrease the counter.0, 3: No effect on counter
 */
#define PCNT_CH0_NEG_MODE_U0    0x00000003U
#define PCNT_CH0_NEG_MODE_U0_M  (PCNT_CH0_NEG_MODE_U0_V << PCNT_CH0_NEG_MODE_U0_S)
#define PCNT_CH0_NEG_MODE_U0_V  0x00000003U
#define PCNT_CH0_NEG_MODE_U0_S  16
/** PCNT_CH0_POS_MODE_U0 : R/W; bitpos: [19:18]; default: 0;
 *  This register sets the behavior when the signal input of channel 0 detects a
 *  positive edge.
 *
 *  1: Increase the counter.2: Decrease the counter.0, 3: No effect on counter
 */
#define PCNT_CH0_POS_MODE_U0    0x00000003U
#define PCNT_CH0_POS_MODE_U0_M  (PCNT_CH0_POS_MODE_U0_V << PCNT_CH0_POS_MODE_U0_S)
#define PCNT_CH0_POS_MODE_U0_V  0x00000003U
#define PCNT_CH0_POS_MODE_U0_S  18
/** PCNT_CH0_HCTRL_MODE_U0 : R/W; bitpos: [21:20]; default: 0;
 *  This register configures how the CH0_POS_MODE/CH0_NEG_MODE settings will be
 *  modified when the control signal is high.
 *
 *  0: No modification.1: Invert behavior (increase -> decrease, decrease ->
 *  increase).2, 3: Inhibit counter modification
 */
#define PCNT_CH0_HCTRL_MODE_U0    0x00000003U
#define PCNT_CH0_HCTRL_MODE_U0_M  (PCNT_CH0_HCTRL_MODE_U0_V << PCNT_CH0_HCTRL_MODE_U0_S)
#define PCNT_CH0_HCTRL_MODE_U0_V  0x00000003U
#define PCNT_CH0_HCTRL_MODE_U0_S  20
/** PCNT_CH0_LCTRL_MODE_U0 : R/W; bitpos: [23:22]; default: 0;
 *  This register configures how the CH0_POS_MODE/CH0_NEG_MODE settings will be
 *  modified when the control signal is low.
 *
 *  0: No modification.1: Invert behavior (increase -> decrease, decrease ->
 *  increase).2, 3: Inhibit counter modification
 */
#define PCNT_CH0_LCTRL_MODE_U0    0x00000003U
#define PCNT_CH0_LCTRL_MODE_U0_M  (PCNT_CH0_LCTRL_MODE_U0_V << PCNT_CH0_LCTRL_MODE_U0_S)
#define PCNT_CH0_LCTRL_MODE_U0_V  0x00000003U
#define PCNT_CH0_LCTRL_MODE_U0_S  22
/** PCNT_CH1_NEG_MODE_U0 : R/W; bitpos: [25:24]; default: 0;
 *  This register sets the behavior when the signal input of channel 1 detects a
 *  negative edge.
 *
 *  1: Increment the counter.2: Decrement the counter.0, 3: No effect on counter
 */
#define PCNT_CH1_NEG_MODE_U0    0x00000003U
#define PCNT_CH1_NEG_MODE_U0_M  (PCNT_CH1_NEG_MODE_U0_V << PCNT_CH1_NEG_MODE_U0_S)
#define PCNT_CH1_NEG_MODE_U0_V  0x00000003U
#define PCNT_CH1_NEG_MODE_U0_S  24
/** PCNT_CH1_POS_MODE_U0 : R/W; bitpos: [27:26]; default: 0;
 *  This register sets the behavior when the signal input of channel 1 detects a
 *  positive edge.
 *
 *  1: Increment the counter.2: Decrement the counter.0, 3: No effect on counter
 */
#define PCNT_CH1_POS_MODE_U0    0x00000003U
#define PCNT_CH1_POS_MODE_U0_M  (PCNT_CH1_POS_MODE_U0_V << PCNT_CH1_POS_MODE_U0_S)
#define PCNT_CH1_POS_MODE_U0_V  0x00000003U
#define PCNT_CH1_POS_MODE_U0_S  26
/** PCNT_CH1_HCTRL_MODE_U0 : R/W; bitpos: [29:28]; default: 0;
 *  This register configures how the CH0_POS_MODE/CH0_NEG_MODE settings will be
 *  modified when the control signal is high.
 *
 *  0: No modification.1: Invert behavior (increase -> decrease, decrease ->
 *  increase).2, 3: Inhibit counter modification
 */
#define PCNT_CH1_HCTRL_MODE_U0    0x00000003U
#define PCNT_CH1_HCTRL_MODE_U0_M  (PCNT_CH1_HCTRL_MODE_U0_V << PCNT_CH1_HCTRL_MODE_U0_S)
#define PCNT_CH1_HCTRL_MODE_U0_V  0x00000003U
#define PCNT_CH1_HCTRL_MODE_U0_S  28
/** PCNT_CH1_LCTRL_MODE_U0 : R/W; bitpos: [31:30]; default: 0;
 *  This register configures how the CH0_POS_MODE/CH0_NEG_MODE settings will be
 *  modified when the control signal is low.
 *
 *  0: No modification.1: Invert behavior (increase -> decrease, decrease ->
 *  increase).2, 3: Inhibit counter modification
 */
#define PCNT_CH1_LCTRL_MODE_U0    0x00000003U
#define PCNT_CH1_LCTRL_MODE_U0_M  (PCNT_CH1_LCTRL_MODE_U0_V << PCNT_CH1_LCTRL_MODE_U0_S)
#define PCNT_CH1_LCTRL_MODE_U0_V  0x00000003U
#define PCNT_CH1_LCTRL_MODE_U0_S  30

/** PCNT_U0_CONF1_REG register
 *  Configuration register 1 for unit 0
 */
#define PCNT_U0_CONF1_REG (DR_REG_PCNT_BASE + 0x4)
/** PCNT_CNT_THRES0_U0 : R/W; bitpos: [15:0]; default: 0;
 *  This register is used to configure the thres0 value for unit 0.
 */
#define PCNT_CNT_THRES0_U0    0x0000FFFFU
#define PCNT_CNT_THRES0_U0_M  (PCNT_CNT_THRES0_U0_V << PCNT_CNT_THRES0_U0_S)
#define PCNT_CNT_THRES0_U0_V  0x0000FFFFU
#define PCNT_CNT_THRES0_U0_S  0
/** PCNT_CNT_THRES1_U0 : R/W; bitpos: [31:16]; default: 0;
 *  This register is used to configure the thres1 value for unit 0.
 */
#define PCNT_CNT_THRES1_U0    0x0000FFFFU
#define PCNT_CNT_THRES1_U0_M  (PCNT_CNT_THRES1_U0_V << PCNT_CNT_THRES1_U0_S)
#define PCNT_CNT_THRES1_U0_V  0x0000FFFFU
#define PCNT_CNT_THRES1_U0_S  16

/** PCNT_U0_CONF2_REG register
 *  Configuration register 2 for unit 0
 */
#define PCNT_U0_CONF2_REG (DR_REG_PCNT_BASE + 0x8)
/** PCNT_CNT_H_LIM_U0 : R/W; bitpos: [15:0]; default: 0;
 *  This register is used to configure the thr_h_lim value for unit 0. When pluse_cnt
 *  reaches this value, the counter will be cleared to 0.
 */
#define PCNT_CNT_H_LIM_U0    0x0000FFFFU
#define PCNT_CNT_H_LIM_U0_M  (PCNT_CNT_H_LIM_U0_V << PCNT_CNT_H_LIM_U0_S)
#define PCNT_CNT_H_LIM_U0_V  0x0000FFFFU
#define PCNT_CNT_H_LIM_U0_S  0
/** PCNT_CNT_L_LIM_U0 : R/W; bitpos: [31:16]; default: 0;
 *  This register is used to configure the thr_l_lim value for unit 0. When pluse_cnt
 *  reaches this value, the counter will be cleared to 0.
 */
#define PCNT_CNT_L_LIM_U0    0x0000FFFFU
#define PCNT_CNT_L_LIM_U0_M  (PCNT_CNT_L_LIM_U0_V << PCNT_CNT_L_LIM_U0_S)
#define PCNT_CNT_L_LIM_U0_V  0x0000FFFFU
#define PCNT_CNT_L_LIM_U0_S  16

/** PCNT_U1_CONF0_REG register
 *  Configuration register 0 for unit 1
 */
#define PCNT_U1_CONF0_REG (DR_REG_PCNT_BASE + 0xc)
/** PCNT_FILTER_THRES_U1 : R/W; bitpos: [9:0]; default: 16;
 *  This sets the maximum threshold, in APB_CLK cycles, for the filter.
 *
 *  Any pulses with width less than this will be ignored when the filter is enabled.
 */
#define PCNT_FILTER_THRES_U1    0x000003FFU
#define PCNT_FILTER_THRES_U1_M  (PCNT_FILTER_THRES_U1_V << PCNT_FILTER_THRES_U1_S)
#define PCNT_FILTER_THRES_U1_V  0x000003FFU
#define PCNT_FILTER_THRES_U1_S  0
/** PCNT_FILTER_EN_U1 : R/W; bitpos: [10]; default: 1;
 *  This is the enable bit for unit 1's input filter.
 */
#define PCNT_FILTER_EN_U1    (BIT(10))
#define PCNT_FILTER_EN_U1_M  (PCNT_FILTER_EN_U1_V << PCNT_FILTER_EN_U1_S)
#define PCNT_FILTER_EN_U1_V  0x00000001U
#define PCNT_FILTER_EN_U1_S  10
/** PCNT_THR_ZERO_EN_U1 : R/W; bitpos: [11]; default: 1;
 *  This is the enable bit for unit 1's zero comparator.
 */
#define PCNT_THR_ZERO_EN_U1    (BIT(11))
#define PCNT_THR_ZERO_EN_U1_M  (PCNT_THR_ZERO_EN_U1_V << PCNT_THR_ZERO_EN_U1_S)
#define PCNT_THR_ZERO_EN_U1_V  0x00000001U
#define PCNT_THR_ZERO_EN_U1_S  11
/** PCNT_THR_H_LIM_EN_U1 : R/W; bitpos: [12]; default: 1;
 *  This is the enable bit for unit 1's thr_h_lim comparator. Configures it to enable
 *  the high limit interrupt.
 */
#define PCNT_THR_H_LIM_EN_U1    (BIT(12))
#define PCNT_THR_H_LIM_EN_U1_M  (PCNT_THR_H_LIM_EN_U1_V << PCNT_THR_H_LIM_EN_U1_S)
#define PCNT_THR_H_LIM_EN_U1_V  0x00000001U
#define PCNT_THR_H_LIM_EN_U1_S  12
/** PCNT_THR_L_LIM_EN_U1 : R/W; bitpos: [13]; default: 1;
 *  This is the enable bit for unit 1's thr_l_lim comparator. Configures it to enable
 *  the low limit interrupt.
 */
#define PCNT_THR_L_LIM_EN_U1    (BIT(13))
#define PCNT_THR_L_LIM_EN_U1_M  (PCNT_THR_L_LIM_EN_U1_V << PCNT_THR_L_LIM_EN_U1_S)
#define PCNT_THR_L_LIM_EN_U1_V  0x00000001U
#define PCNT_THR_L_LIM_EN_U1_S  13
/** PCNT_THR_THRES0_EN_U1 : R/W; bitpos: [14]; default: 0;
 *  This is the enable bit for unit 1's thres0 comparator.
 */
#define PCNT_THR_THRES0_EN_U1    (BIT(14))
#define PCNT_THR_THRES0_EN_U1_M  (PCNT_THR_THRES0_EN_U1_V << PCNT_THR_THRES0_EN_U1_S)
#define PCNT_THR_THRES0_EN_U1_V  0x00000001U
#define PCNT_THR_THRES0_EN_U1_S  14
/** PCNT_THR_THRES1_EN_U1 : R/W; bitpos: [15]; default: 0;
 *  This is the enable bit for unit 1's thres1 comparator.
 */
#define PCNT_THR_THRES1_EN_U1    (BIT(15))
#define PCNT_THR_THRES1_EN_U1_M  (PCNT_THR_THRES1_EN_U1_V << PCNT_THR_THRES1_EN_U1_S)
#define PCNT_THR_THRES1_EN_U1_V  0x00000001U
#define PCNT_THR_THRES1_EN_U1_S  15
/** PCNT_CH0_NEG_MODE_U1 : R/W; bitpos: [17:16]; default: 0;
 *  This register sets the behavior when the signal input of channel 0 detects a
 *  negative edge.
 *
 *  1: Increase the counter.2: Decrease the counter.0, 3: No effect on counter
 */
#define PCNT_CH0_NEG_MODE_U1    0x00000003U
#define PCNT_CH0_NEG_MODE_U1_M  (PCNT_CH0_NEG_MODE_U1_V << PCNT_CH0_NEG_MODE_U1_S)
#define PCNT_CH0_NEG_MODE_U1_V  0x00000003U
#define PCNT_CH0_NEG_MODE_U1_S  16
/** PCNT_CH0_POS_MODE_U1 : R/W; bitpos: [19:18]; default: 0;
 *  This register sets the behavior when the signal input of channel 0 detects a
 *  positive edge.
 *
 *  1: Increase the counter.2: Decrease the counter.0, 3: No effect on counter
 */
#define PCNT_CH0_POS_MODE_U1    0x00000003U
#define PCNT_CH0_POS_MODE_U1_M  (PCNT_CH0_POS_MODE_U1_V << PCNT_CH0_POS_MODE_U1_S)
#define PCNT_CH0_POS_MODE_U1_V  0x00000003U
#define PCNT_CH0_POS_MODE_U1_S  18
/** PCNT_CH0_HCTRL_MODE_U1 : R/W; bitpos: [21:20]; default: 0;
 *  This register configures how the CH1_POS_MODE/CH1_NEG_MODE settings will be
 *  modified when the control signal is high.
 *
 *  0: No modification.1: Invert behavior (increase -> decrease, decrease ->
 *  increase).2, 3: Inhibit counter modification
 */
#define PCNT_CH0_HCTRL_MODE_U1    0x00000003U
#define PCNT_CH0_HCTRL_MODE_U1_M  (PCNT_CH0_HCTRL_MODE_U1_V << PCNT_CH0_HCTRL_MODE_U1_S)
#define PCNT_CH0_HCTRL_MODE_U1_V  0x00000003U
#define PCNT_CH0_HCTRL_MODE_U1_S  20
/** PCNT_CH0_LCTRL_MODE_U1 : R/W; bitpos: [23:22]; default: 0;
 *  This register configures how the CH1_POS_MODE/CH1_NEG_MODE settings will be
 *  modified when the control signal is low.
 *
 *  0: No modification.1: Invert behavior (increase -> decrease, decrease ->
 *  increase).2, 3: Inhibit counter modification
 */
#define PCNT_CH0_LCTRL_MODE_U1    0x00000003U
#define PCNT_CH0_LCTRL_MODE_U1_M  (PCNT_CH0_LCTRL_MODE_U1_V << PCNT_CH0_LCTRL_MODE_U1_S)
#define PCNT_CH0_LCTRL_MODE_U1_V  0x00000003U
#define PCNT_CH0_LCTRL_MODE_U1_S  22
/** PCNT_CH1_NEG_MODE_U1 : R/W; bitpos: [25:24]; default: 0;
 *  This register sets the behavior when the signal input of channel 1 detects a
 *  negative edge.
 *
 *  1: Increment the counter.2: Decrement the counter.0, 3: No effect on counter
 */
#define PCNT_CH1_NEG_MODE_U1    0x00000003U
#define PCNT_CH1_NEG_MODE_U1_M  (PCNT_CH1_NEG_MODE_U1_V << PCNT_CH1_NEG_MODE_U1_S)
#define PCNT_CH1_NEG_MODE_U1_V  0x00000003U
#define PCNT_CH1_NEG_MODE_U1_S  24
/** PCNT_CH1_POS_MODE_U1 : R/W; bitpos: [27:26]; default: 0;
 *  This register sets the behavior when the signal input of channel 1 detects a
 *  positive edge.
 *
 *  1: Increment the counter.2: Decrement the counter.0, 3: No effect on counter
 */
#define PCNT_CH1_POS_MODE_U1    0x00000003U
#define PCNT_CH1_POS_MODE_U1_M  (PCNT_CH1_POS_MODE_U1_V << PCNT_CH1_POS_MODE_U1_S)
#define PCNT_CH1_POS_MODE_U1_V  0x00000003U
#define PCNT_CH1_POS_MODE_U1_S  26
/** PCNT_CH1_HCTRL_MODE_U1 : R/W; bitpos: [29:28]; default: 0;
 *  This register configures how the CH1_POS_MODE/CH1_NEG_MODE settings will be
 *  modified when the control signal is high.
 *
 *  0: No modification.1: Invert behavior (increase -> decrease, decrease ->
 *  increase).2, 3: Inhibit counter modification
 */
#define PCNT_CH1_HCTRL_MODE_U1    0x00000003U
#define PCNT_CH1_HCTRL_MODE_U1_M  (PCNT_CH1_HCTRL_MODE_U1_V << PCNT_CH1_HCTRL_MODE_U1_S)
#define PCNT_CH1_HCTRL_MODE_U1_V  0x00000003U
#define PCNT_CH1_HCTRL_MODE_U1_S  28
/** PCNT_CH1_LCTRL_MODE_U1 : R/W; bitpos: [31:30]; default: 0;
 *  This register configures how the CH1_POS_MODE/CH1_NEG_MODE settings will be
 *  modified when the control signal is low.
 *
 *  0: No modification.1: Invert behavior (increase -> decrease, decrease ->
 *  increase).2, 3: Inhibit counter modification
 */
#define PCNT_CH1_LCTRL_MODE_U1    0x00000003U
#define PCNT_CH1_LCTRL_MODE_U1_M  (PCNT_CH1_LCTRL_MODE_U1_V << PCNT_CH1_LCTRL_MODE_U1_S)
#define PCNT_CH1_LCTRL_MODE_U1_V  0x00000003U
#define PCNT_CH1_LCTRL_MODE_U1_S  30

/** PCNT_U1_CONF1_REG register
 *  Configuration register 1 for unit 1
 */
#define PCNT_U1_CONF1_REG (DR_REG_PCNT_BASE + 0x10)
/** PCNT_CNT_THRES0_U1 : R/W; bitpos: [15:0]; default: 0;
 *  This register is used to configure the thres0 value for unit 1.
 */
#define PCNT_CNT_THRES0_U1    0x0000FFFFU
#define PCNT_CNT_THRES0_U1_M  (PCNT_CNT_THRES0_U1_V << PCNT_CNT_THRES0_U1_S)
#define PCNT_CNT_THRES0_U1_V  0x0000FFFFU
#define PCNT_CNT_THRES0_U1_S  0
/** PCNT_CNT_THRES1_U1 : R/W; bitpos: [31:16]; default: 0;
 *  This register is used to configure the thres1 value for unit 1.
 */
#define PCNT_CNT_THRES1_U1    0x0000FFFFU
#define PCNT_CNT_THRES1_U1_M  (PCNT_CNT_THRES1_U1_V << PCNT_CNT_THRES1_U1_S)
#define PCNT_CNT_THRES1_U1_V  0x0000FFFFU
#define PCNT_CNT_THRES1_U1_S  16

/** PCNT_U1_CONF2_REG register
 *  Configuration register 2 for unit 1
 */
#define PCNT_U1_CONF2_REG (DR_REG_PCNT_BASE + 0x14)
/** PCNT_CNT_H_LIM_U1 : R/W; bitpos: [15:0]; default: 0;
 *  This register is used to configure the thr_h_lim value for unit 1. When pluse_cnt
 *  reaches this value, the counter will be cleared to 0.
 */
#define PCNT_CNT_H_LIM_U1    0x0000FFFFU
#define PCNT_CNT_H_LIM_U1_M  (PCNT_CNT_H_LIM_U1_V << PCNT_CNT_H_LIM_U1_S)
#define PCNT_CNT_H_LIM_U1_V  0x0000FFFFU
#define PCNT_CNT_H_LIM_U1_S  0
/** PCNT_CNT_L_LIM_U1 : R/W; bitpos: [31:16]; default: 0;
 *  This register is used to configure the thr_l_lim value for unit 1. When pluse_cnt
 *  reaches this value, the counter will be cleared to 0.
 */
#define PCNT_CNT_L_LIM_U1    0x0000FFFFU
#define PCNT_CNT_L_LIM_U1_M  (PCNT_CNT_L_LIM_U1_V << PCNT_CNT_L_LIM_U1_S)
#define PCNT_CNT_L_LIM_U1_V  0x0000FFFFU
#define PCNT_CNT_L_LIM_U1_S  16

/** PCNT_U2_CONF0_REG register
 *  Configuration register 0 for unit 2
 */
#define PCNT_U2_CONF0_REG (DR_REG_PCNT_BASE + 0x18)
/** PCNT_FILTER_THRES_U2 : R/W; bitpos: [9:0]; default: 16;
 *  This sets the maximum threshold, in APB_CLK cycles, for the filter.
 *
 *  Any pulses with width less than this will be ignored when the filter is enabled.
 */
#define PCNT_FILTER_THRES_U2    0x000003FFU
#define PCNT_FILTER_THRES_U2_M  (PCNT_FILTER_THRES_U2_V << PCNT_FILTER_THRES_U2_S)
#define PCNT_FILTER_THRES_U2_V  0x000003FFU
#define PCNT_FILTER_THRES_U2_S  0
/** PCNT_FILTER_EN_U2 : R/W; bitpos: [10]; default: 1;
 *  This is the enable bit for unit 2's input filter.
 */
#define PCNT_FILTER_EN_U2    (BIT(10))
#define PCNT_FILTER_EN_U2_M  (PCNT_FILTER_EN_U2_V << PCNT_FILTER_EN_U2_S)
#define PCNT_FILTER_EN_U2_V  0x00000001U
#define PCNT_FILTER_EN_U2_S  10
/** PCNT_THR_ZERO_EN_U2 : R/W; bitpos: [11]; default: 1;
 *  This is the enable bit for unit 2's zero comparator.
 */
#define PCNT_THR_ZERO_EN_U2    (BIT(11))
#define PCNT_THR_ZERO_EN_U2_M  (PCNT_THR_ZERO_EN_U2_V << PCNT_THR_ZERO_EN_U2_S)
#define PCNT_THR_ZERO_EN_U2_V  0x00000001U
#define PCNT_THR_ZERO_EN_U2_S  11
/** PCNT_THR_H_LIM_EN_U2 : R/W; bitpos: [12]; default: 1;
 *  This is the enable bit for unit 2's thr_h_lim comparator. Configures it to enable
 *  the high limit interrupt.
 */
#define PCNT_THR_H_LIM_EN_U2    (BIT(12))
#define PCNT_THR_H_LIM_EN_U2_M  (PCNT_THR_H_LIM_EN_U2_V << PCNT_THR_H_LIM_EN_U2_S)
#define PCNT_THR_H_LIM_EN_U2_V  0x00000001U
#define PCNT_THR_H_LIM_EN_U2_S  12
/** PCNT_THR_L_LIM_EN_U2 : R/W; bitpos: [13]; default: 1;
 *  This is the enable bit for unit 2's thr_l_lim comparator. Configures it to enable
 *  the low limit interrupt.
 */
#define PCNT_THR_L_LIM_EN_U2    (BIT(13))
#define PCNT_THR_L_LIM_EN_U2_M  (PCNT_THR_L_LIM_EN_U2_V << PCNT_THR_L_LIM_EN_U2_S)
#define PCNT_THR_L_LIM_EN_U2_V  0x00000001U
#define PCNT_THR_L_LIM_EN_U2_S  13
/** PCNT_THR_THRES0_EN_U2 : R/W; bitpos: [14]; default: 0;
 *  This is the enable bit for unit 2's thres0 comparator.
 */
#define PCNT_THR_THRES0_EN_U2    (BIT(14))
#define PCNT_THR_THRES0_EN_U2_M  (PCNT_THR_THRES0_EN_U2_V << PCNT_THR_THRES0_EN_U2_S)
#define PCNT_THR_THRES0_EN_U2_V  0x00000001U
#define PCNT_THR_THRES0_EN_U2_S  14
/** PCNT_THR_THRES1_EN_U2 : R/W; bitpos: [15]; default: 0;
 *  This is the enable bit for unit 2's thres1 comparator.
 */
#define PCNT_THR_THRES1_EN_U2    (BIT(15))
#define PCNT_THR_THRES1_EN_U2_M  (PCNT_THR_THRES1_EN_U2_V << PCNT_THR_THRES1_EN_U2_S)
#define PCNT_THR_THRES1_EN_U2_V  0x00000001U
#define PCNT_THR_THRES1_EN_U2_S  15
/** PCNT_CH0_NEG_MODE_U2 : R/W; bitpos: [17:16]; default: 0;
 *  This register sets the behavior when the signal input of channel 0 detects a
 *  negative edge.
 *
 *  1: Increase the counter.2: Decrease the counter.0, 3: No effect on counter
 */
#define PCNT_CH0_NEG_MODE_U2    0x00000003U
#define PCNT_CH0_NEG_MODE_U2_M  (PCNT_CH0_NEG_MODE_U2_V << PCNT_CH0_NEG_MODE_U2_S)
#define PCNT_CH0_NEG_MODE_U2_V  0x00000003U
#define PCNT_CH0_NEG_MODE_U2_S  16
/** PCNT_CH0_POS_MODE_U2 : R/W; bitpos: [19:18]; default: 0;
 *  This register sets the behavior when the signal input of channel 0 detects a
 *  positive edge.
 *
 *  1: Increase the counter.2: Decrease the counter.0, 3: No effect on counter
 */
#define PCNT_CH0_POS_MODE_U2    0x00000003U
#define PCNT_CH0_POS_MODE_U2_M  (PCNT_CH0_POS_MODE_U2_V << PCNT_CH0_POS_MODE_U2_S)
#define PCNT_CH0_POS_MODE_U2_V  0x00000003U
#define PCNT_CH0_POS_MODE_U2_S  18
/** PCNT_CH0_HCTRL_MODE_U2 : R/W; bitpos: [21:20]; default: 0;
 *  This register configures how the CH2_POS_MODE/CH2_NEG_MODE settings will be
 *  modified when the control signal is high.
 *
 *  0: No modification.1: Invert behavior (increase -> decrease, decrease ->
 *  increase).2, 3: Inhibit counter modification
 */
#define PCNT_CH0_HCTRL_MODE_U2    0x00000003U
#define PCNT_CH0_HCTRL_MODE_U2_M  (PCNT_CH0_HCTRL_MODE_U2_V << PCNT_CH0_HCTRL_MODE_U2_S)
#define PCNT_CH0_HCTRL_MODE_U2_V  0x00000003U
#define PCNT_CH0_HCTRL_MODE_U2_S  20
/** PCNT_CH0_LCTRL_MODE_U2 : R/W; bitpos: [23:22]; default: 0;
 *  This register configures how the CH2_POS_MODE/CH2_NEG_MODE settings will be
 *  modified when the control signal is low.
 *
 *  0: No modification.1: Invert behavior (increase -> decrease, decrease ->
 *  increase).2, 3: Inhibit counter modification
 */
#define PCNT_CH0_LCTRL_MODE_U2    0x00000003U
#define PCNT_CH0_LCTRL_MODE_U2_M  (PCNT_CH0_LCTRL_MODE_U2_V << PCNT_CH0_LCTRL_MODE_U2_S)
#define PCNT_CH0_LCTRL_MODE_U2_V  0x00000003U
#define PCNT_CH0_LCTRL_MODE_U2_S  22
/** PCNT_CH1_NEG_MODE_U2 : R/W; bitpos: [25:24]; default: 0;
 *  This register sets the behavior when the signal input of channel 1 detects a
 *  negative edge.
 *
 *  1: Increment the counter.2: Decrement the counter.0, 3: No effect on counter
 */
#define PCNT_CH1_NEG_MODE_U2    0x00000003U
#define PCNT_CH1_NEG_MODE_U2_M  (PCNT_CH1_NEG_MODE_U2_V << PCNT_CH1_NEG_MODE_U2_S)
#define PCNT_CH1_NEG_MODE_U2_V  0x00000003U
#define PCNT_CH1_NEG_MODE_U2_S  24
/** PCNT_CH1_POS_MODE_U2 : R/W; bitpos: [27:26]; default: 0;
 *  This register sets the behavior when the signal input of channel 1 detects a
 *  positive edge.
 *
 *  1: Increment the counter.2: Decrement the counter.0, 3: No effect on counter
 */
#define PCNT_CH1_POS_MODE_U2    0x00000003U
#define PCNT_CH1_POS_MODE_U2_M  (PCNT_CH1_POS_MODE_U2_V << PCNT_CH1_POS_MODE_U2_S)
#define PCNT_CH1_POS_MODE_U2_V  0x00000003U
#define PCNT_CH1_POS_MODE_U2_S  26
/** PCNT_CH1_HCTRL_MODE_U2 : R/W; bitpos: [29:28]; default: 0;
 *  This register configures how the CH2_POS_MODE/CH2_NEG_MODE settings will be
 *  modified when the control signal is high.
 *
 *  0: No modification.1: Invert behavior (increase -> decrease, decrease ->
 *  increase).2, 3: Inhibit counter modification
 */
#define PCNT_CH1_HCTRL_MODE_U2    0x00000003U
#define PCNT_CH1_HCTRL_MODE_U2_M  (PCNT_CH1_HCTRL_MODE_U2_V << PCNT_CH1_HCTRL_MODE_U2_S)
#define PCNT_CH1_HCTRL_MODE_U2_V  0x00000003U
#define PCNT_CH1_HCTRL_MODE_U2_S  28
/** PCNT_CH1_LCTRL_MODE_U2 : R/W; bitpos: [31:30]; default: 0;
 *  This register configures how the CH2_POS_MODE/CH2_NEG_MODE settings will be
 *  modified when the control signal is low.
 *
 *  0: No modification.1: Invert behavior (increase -> decrease, decrease ->
 *  increase).2, 3: Inhibit counter modification
 */
#define PCNT_CH1_LCTRL_MODE_U2    0x00000003U
#define PCNT_CH1_LCTRL_MODE_U2_M  (PCNT_CH1_LCTRL_MODE_U2_V << PCNT_CH1_LCTRL_MODE_U2_S)
#define PCNT_CH1_LCTRL_MODE_U2_V  0x00000003U
#define PCNT_CH1_LCTRL_MODE_U2_S  30

/** PCNT_U2_CONF1_REG register
 *  Configuration register 1 for unit 2
 */
#define PCNT_U2_CONF1_REG (DR_REG_PCNT_BASE + 0x1c)
/** PCNT_CNT_THRES0_U2 : R/W; bitpos: [15:0]; default: 0;
 *  This register is used to configure the thres0 value for unit 2.
 */
#define PCNT_CNT_THRES0_U2    0x0000FFFFU
#define PCNT_CNT_THRES0_U2_M  (PCNT_CNT_THRES0_U2_V << PCNT_CNT_THRES0_U2_S)
#define PCNT_CNT_THRES0_U2_V  0x0000FFFFU
#define PCNT_CNT_THRES0_U2_S  0
/** PCNT_CNT_THRES1_U2 : R/W; bitpos: [31:16]; default: 0;
 *  This register is used to configure the thres1 value for unit 2.
 */
#define PCNT_CNT_THRES1_U2    0x0000FFFFU
#define PCNT_CNT_THRES1_U2_M  (PCNT_CNT_THRES1_U2_V << PCNT_CNT_THRES1_U2_S)
#define PCNT_CNT_THRES1_U2_V  0x0000FFFFU
#define PCNT_CNT_THRES1_U2_S  16

/** PCNT_U2_CONF2_REG register
 *  Configuration register 2 for unit 2
 */
#define PCNT_U2_CONF2_REG (DR_REG_PCNT_BASE + 0x20)
/** PCNT_CNT_H_LIM_U2 : R/W; bitpos: [15:0]; default: 0;
 *  This register is used to configure the thr_h_lim value for unit 2. When pluse_cnt
 *  reaches this value, the counter will be cleared to 0.
 */
#define PCNT_CNT_H_LIM_U2    0x0000FFFFU
#define PCNT_CNT_H_LIM_U2_M  (PCNT_CNT_H_LIM_U2_V << PCNT_CNT_H_LIM_U2_S)
#define PCNT_CNT_H_LIM_U2_V  0x0000FFFFU
#define PCNT_CNT_H_LIM_U2_S  0
/** PCNT_CNT_L_LIM_U2 : R/W; bitpos: [31:16]; default: 0;
 *  This register is used to configure the thr_l_lim value for unit 2. When pluse_cnt
 *  reaches this value, the counter will be cleared to 0.
 */
#define PCNT_CNT_L_LIM_U2    0x0000FFFFU
#define PCNT_CNT_L_LIM_U2_M  (PCNT_CNT_L_LIM_U2_V << PCNT_CNT_L_LIM_U2_S)
#define PCNT_CNT_L_LIM_U2_V  0x0000FFFFU
#define PCNT_CNT_L_LIM_U2_S  16

/** PCNT_U3_CONF0_REG register
 *  Configuration register 0 for unit 3
 */
#define PCNT_U3_CONF0_REG (DR_REG_PCNT_BASE + 0x24)
/** PCNT_FILTER_THRES_U3 : R/W; bitpos: [9:0]; default: 16;
 *  This sets the maximum threshold, in APB_CLK cycles, for the filter.
 *
 *  Any pulses with width less than this will be ignored when the filter is enabled.
 */
#define PCNT_FILTER_THRES_U3    0x000003FFU
#define PCNT_FILTER_THRES_U3_M  (PCNT_FILTER_THRES_U3_V << PCNT_FILTER_THRES_U3_S)
#define PCNT_FILTER_THRES_U3_V  0x000003FFU
#define PCNT_FILTER_THRES_U3_S  0
/** PCNT_FILTER_EN_U3 : R/W; bitpos: [10]; default: 1;
 *  This is the enable bit for unit 3's input filter.
 */
#define PCNT_FILTER_EN_U3    (BIT(10))
#define PCNT_FILTER_EN_U3_M  (PCNT_FILTER_EN_U3_V << PCNT_FILTER_EN_U3_S)
#define PCNT_FILTER_EN_U3_V  0x00000001U
#define PCNT_FILTER_EN_U3_S  10
/** PCNT_THR_ZERO_EN_U3 : R/W; bitpos: [11]; default: 1;
 *  This is the enable bit for unit 3's zero comparator.
 */
#define PCNT_THR_ZERO_EN_U3    (BIT(11))
#define PCNT_THR_ZERO_EN_U3_M  (PCNT_THR_ZERO_EN_U3_V << PCNT_THR_ZERO_EN_U3_S)
#define PCNT_THR_ZERO_EN_U3_V  0x00000001U
#define PCNT_THR_ZERO_EN_U3_S  11
/** PCNT_THR_H_LIM_EN_U3 : R/W; bitpos: [12]; default: 1;
 *  This is the enable bit for unit 3's thr_h_lim comparator. Configures it to enable
 *  the high limit interrupt.
 */
#define PCNT_THR_H_LIM_EN_U3    (BIT(12))
#define PCNT_THR_H_LIM_EN_U3_M  (PCNT_THR_H_LIM_EN_U3_V << PCNT_THR_H_LIM_EN_U3_S)
#define PCNT_THR_H_LIM_EN_U3_V  0x00000001U
#define PCNT_THR_H_LIM_EN_U3_S  12
/** PCNT_THR_L_LIM_EN_U3 : R/W; bitpos: [13]; default: 1;
 *  This is the enable bit for unit 3's thr_l_lim comparator. Configures it to enable
 *  the low limit interrupt.
 */
#define PCNT_THR_L_LIM_EN_U3    (BIT(13))
#define PCNT_THR_L_LIM_EN_U3_M  (PCNT_THR_L_LIM_EN_U3_V << PCNT_THR_L_LIM_EN_U3_S)
#define PCNT_THR_L_LIM_EN_U3_V  0x00000001U
#define PCNT_THR_L_LIM_EN_U3_S  13
/** PCNT_THR_THRES0_EN_U3 : R/W; bitpos: [14]; default: 0;
 *  This is the enable bit for unit 3's thres0 comparator.
 */
#define PCNT_THR_THRES0_EN_U3    (BIT(14))
#define PCNT_THR_THRES0_EN_U3_M  (PCNT_THR_THRES0_EN_U3_V << PCNT_THR_THRES0_EN_U3_S)
#define PCNT_THR_THRES0_EN_U3_V  0x00000001U
#define PCNT_THR_THRES0_EN_U3_S  14
/** PCNT_THR_THRES1_EN_U3 : R/W; bitpos: [15]; default: 0;
 *  This is the enable bit for unit 3's thres1 comparator.
 */
#define PCNT_THR_THRES1_EN_U3    (BIT(15))
#define PCNT_THR_THRES1_EN_U3_M  (PCNT_THR_THRES1_EN_U3_V << PCNT_THR_THRES1_EN_U3_S)
#define PCNT_THR_THRES1_EN_U3_V  0x00000001U
#define PCNT_THR_THRES1_EN_U3_S  15
/** PCNT_CH0_NEG_MODE_U3 : R/W; bitpos: [17:16]; default: 0;
 *  This register sets the behavior when the signal input of channel 0 detects a
 *  negative edge.
 *
 *  1: Increase the counter.2: Decrease the counter.0, 3: No effect on counter
 */
#define PCNT_CH0_NEG_MODE_U3    0x00000003U
#define PCNT_CH0_NEG_MODE_U3_M  (PCNT_CH0_NEG_MODE_U3_V << PCNT_CH0_NEG_MODE_U3_S)
#define PCNT_CH0_NEG_MODE_U3_V  0x00000003U
#define PCNT_CH0_NEG_MODE_U3_S  16
/** PCNT_CH0_POS_MODE_U3 : R/W; bitpos: [19:18]; default: 0;
 *  This register sets the behavior when the signal input of channel 0 detects a
 *  positive edge.
 *
 *  1: Increase the counter.2: Decrease the counter.0, 3: No effect on counter
 */
#define PCNT_CH0_POS_MODE_U3    0x00000003U
#define PCNT_CH0_POS_MODE_U3_M  (PCNT_CH0_POS_MODE_U3_V << PCNT_CH0_POS_MODE_U3_S)
#define PCNT_CH0_POS_MODE_U3_V  0x00000003U
#define PCNT_CH0_POS_MODE_U3_S  18
/** PCNT_CH0_HCTRL_MODE_U3 : R/W; bitpos: [21:20]; default: 0;
 *  This register configures how the CH3_POS_MODE/CH3_NEG_MODE settings will be
 *  modified when the control signal is high.
 *
 *  0: No modification.1: Invert behavior (increase -> decrease, decrease ->
 *  increase).2, 3: Inhibit counter modification
 */
#define PCNT_CH0_HCTRL_MODE_U3    0x00000003U
#define PCNT_CH0_HCTRL_MODE_U3_M  (PCNT_CH0_HCTRL_MODE_U3_V << PCNT_CH0_HCTRL_MODE_U3_S)
#define PCNT_CH0_HCTRL_MODE_U3_V  0x00000003U
#define PCNT_CH0_HCTRL_MODE_U3_S  20
/** PCNT_CH0_LCTRL_MODE_U3 : R/W; bitpos: [23:22]; default: 0;
 *  This register configures how the CH3_POS_MODE/CH3_NEG_MODE settings will be
 *  modified when the control signal is low.
 *
 *  0: No modification.1: Invert behavior (increase -> decrease, decrease ->
 *  increase).2, 3: Inhibit counter modification
 */
#define PCNT_CH0_LCTRL_MODE_U3    0x00000003U
#define PCNT_CH0_LCTRL_MODE_U3_M  (PCNT_CH0_LCTRL_MODE_U3_V << PCNT_CH0_LCTRL_MODE_U3_S)
#define PCNT_CH0_LCTRL_MODE_U3_V  0x00000003U
#define PCNT_CH0_LCTRL_MODE_U3_S  22
/** PCNT_CH1_NEG_MODE_U3 : R/W; bitpos: [25:24]; default: 0;
 *  This register sets the behavior when the signal input of channel 1 detects a
 *  negative edge.
 *
 *  1: Increment the counter.2: Decrement the counter.0, 3: No effect on counter
 */
#define PCNT_CH1_NEG_MODE_U3    0x00000003U
#define PCNT_CH1_NEG_MODE_U3_M  (PCNT_CH1_NEG_MODE_U3_V << PCNT_CH1_NEG_MODE_U3_S)
#define PCNT_CH1_NEG_MODE_U3_V  0x00000003U
#define PCNT_CH1_NEG_MODE_U3_S  24
/** PCNT_CH1_POS_MODE_U3 : R/W; bitpos: [27:26]; default: 0;
 *  This register sets the behavior when the signal input of channel 1 detects a
 *  positive edge.
 *
 *  1: Increment the counter.2: Decrement the counter.0, 3: No effect on counter
 */
#define PCNT_CH1_POS_MODE_U3    0x00000003U
#define PCNT_CH1_POS_MODE_U3_M  (PCNT_CH1_POS_MODE_U3_V << PCNT_CH1_POS_MODE_U3_S)
#define PCNT_CH1_POS_MODE_U3_V  0x00000003U
#define PCNT_CH1_POS_MODE_U3_S  26
/** PCNT_CH1_HCTRL_MODE_U3 : R/W; bitpos: [29:28]; default: 0;
 *  This register configures how the CH3_POS_MODE/CH3_NEG_MODE settings will be
 *  modified when the control signal is high.
 *
 *  0: No modification.1: Invert behavior (increase -> decrease, decrease ->
 *  increase).2, 3: Inhibit counter modification
 */
#define PCNT_CH1_HCTRL_MODE_U3    0x00000003U
#define PCNT_CH1_HCTRL_MODE_U3_M  (PCNT_CH1_HCTRL_MODE_U3_V << PCNT_CH1_HCTRL_MODE_U3_S)
#define PCNT_CH1_HCTRL_MODE_U3_V  0x00000003U
#define PCNT_CH1_HCTRL_MODE_U3_S  28
/** PCNT_CH1_LCTRL_MODE_U3 : R/W; bitpos: [31:30]; default: 0;
 *  This register configures how the CH3_POS_MODE/CH3_NEG_MODE settings will be
 *  modified when the control signal is low.
 *
 *  0: No modification.1: Invert behavior (increase -> decrease, decrease ->
 *  increase).2, 3: Inhibit counter modification
 */
#define PCNT_CH1_LCTRL_MODE_U3    0x00000003U
#define PCNT_CH1_LCTRL_MODE_U3_M  (PCNT_CH1_LCTRL_MODE_U3_V << PCNT_CH1_LCTRL_MODE_U3_S)
#define PCNT_CH1_LCTRL_MODE_U3_V  0x00000003U
#define PCNT_CH1_LCTRL_MODE_U3_S  30

/** PCNT_U3_CONF1_REG register
 *  Configuration register 1 for unit 3
 */
#define PCNT_U3_CONF1_REG (DR_REG_PCNT_BASE + 0x28)
/** PCNT_CNT_THRES0_U3 : R/W; bitpos: [15:0]; default: 0;
 *  This register is used to configure the thres0 value for unit 3.
 */
#define PCNT_CNT_THRES0_U3    0x0000FFFFU
#define PCNT_CNT_THRES0_U3_M  (PCNT_CNT_THRES0_U3_V << PCNT_CNT_THRES0_U3_S)
#define PCNT_CNT_THRES0_U3_V  0x0000FFFFU
#define PCNT_CNT_THRES0_U3_S  0
/** PCNT_CNT_THRES1_U3 : R/W; bitpos: [31:16]; default: 0;
 *  This register is used to configure the thres1 value for unit 3.
 */
#define PCNT_CNT_THRES1_U3    0x0000FFFFU
#define PCNT_CNT_THRES1_U3_M  (PCNT_CNT_THRES1_U3_V << PCNT_CNT_THRES1_U3_S)
#define PCNT_CNT_THRES1_U3_V  0x0000FFFFU
#define PCNT_CNT_THRES1_U3_S  16

/** PCNT_U3_CONF2_REG register
 *  Configuration register 2 for unit 3
 */
#define PCNT_U3_CONF2_REG (DR_REG_PCNT_BASE + 0x2c)
/** PCNT_CNT_H_LIM_U3 : R/W; bitpos: [15:0]; default: 0;
 *  This register is used to configure the thr_h_lim value for unit 3. When pluse_cnt
 *  reaches this value, the counter will be cleared to 0.
 */
#define PCNT_CNT_H_LIM_U3    0x0000FFFFU
#define PCNT_CNT_H_LIM_U3_M  (PCNT_CNT_H_LIM_U3_V << PCNT_CNT_H_LIM_U3_S)
#define PCNT_CNT_H_LIM_U3_V  0x0000FFFFU
#define PCNT_CNT_H_LIM_U3_S  0
/** PCNT_CNT_L_LIM_U3 : R/W; bitpos: [31:16]; default: 0;
 *  This register is used to configure the thr_l_lim value for unit 3. When pluse_cnt
 *  reaches this value, the counter will be cleared to 0.
 */
#define PCNT_CNT_L_LIM_U3    0x0000FFFFU
#define PCNT_CNT_L_LIM_U3_M  (PCNT_CNT_L_LIM_U3_V << PCNT_CNT_L_LIM_U3_S)
#define PCNT_CNT_L_LIM_U3_V  0x0000FFFFU
#define PCNT_CNT_L_LIM_U3_S  16

/** PCNT_U0_CNT_REG register
 *  Counter value for unit 0
 */
#define PCNT_U0_CNT_REG (DR_REG_PCNT_BASE + 0x30)
/** PCNT_PULSE_CNT_U0 : RO; bitpos: [15:0]; default: 0;
 *  This register stores the current pulse count value for unit 0.
 */
#define PCNT_PULSE_CNT_U0    0x0000FFFFU
#define PCNT_PULSE_CNT_U0_M  (PCNT_PULSE_CNT_U0_V << PCNT_PULSE_CNT_U0_S)
#define PCNT_PULSE_CNT_U0_V  0x0000FFFFU
#define PCNT_PULSE_CNT_U0_S  0

/** PCNT_U1_CNT_REG register
 *  Counter value for unit 1
 */
#define PCNT_U1_CNT_REG (DR_REG_PCNT_BASE + 0x34)
/** PCNT_PULSE_CNT_U1 : RO; bitpos: [15:0]; default: 0;
 *  This register stores the current pulse count value for unit 1.
 */
#define PCNT_PULSE_CNT_U1    0x0000FFFFU
#define PCNT_PULSE_CNT_U1_M  (PCNT_PULSE_CNT_U1_V << PCNT_PULSE_CNT_U1_S)
#define PCNT_PULSE_CNT_U1_V  0x0000FFFFU
#define PCNT_PULSE_CNT_U1_S  0

/** PCNT_U2_CNT_REG register
 *  Counter value for unit 2
 */
#define PCNT_U2_CNT_REG (DR_REG_PCNT_BASE + 0x38)
/** PCNT_PULSE_CNT_U2 : RO; bitpos: [15:0]; default: 0;
 *  This register stores the current pulse count value for unit 2.
 */
#define PCNT_PULSE_CNT_U2    0x0000FFFFU
#define PCNT_PULSE_CNT_U2_M  (PCNT_PULSE_CNT_U2_V << PCNT_PULSE_CNT_U2_S)
#define PCNT_PULSE_CNT_U2_V  0x0000FFFFU
#define PCNT_PULSE_CNT_U2_S  0

/** PCNT_U3_CNT_REG register
 *  Counter value for unit 3
 */
#define PCNT_U3_CNT_REG (DR_REG_PCNT_BASE + 0x3c)
/** PCNT_PULSE_CNT_U3 : RO; bitpos: [15:0]; default: 0;
 *  This register stores the current pulse count value for unit 3.
 */
#define PCNT_PULSE_CNT_U3    0x0000FFFFU
#define PCNT_PULSE_CNT_U3_M  (PCNT_PULSE_CNT_U3_V << PCNT_PULSE_CNT_U3_S)
#define PCNT_PULSE_CNT_U3_V  0x0000FFFFU
#define PCNT_PULSE_CNT_U3_S  0

/** PCNT_INT_RAW_REG register
 *  Interrupt raw status register
 */
#define PCNT_INT_RAW_REG (DR_REG_PCNT_BASE + 0x40)
/** PCNT_CNT_THR_EVENT_U0_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  The raw interrupt status bit for the PCNT_CNT_THR_EVENT_U0_INT interrupt.
 */
#define PCNT_CNT_THR_EVENT_U0_INT_RAW    (BIT(0))
#define PCNT_CNT_THR_EVENT_U0_INT_RAW_M  (PCNT_CNT_THR_EVENT_U0_INT_RAW_V << PCNT_CNT_THR_EVENT_U0_INT_RAW_S)
#define PCNT_CNT_THR_EVENT_U0_INT_RAW_V  0x00000001U
#define PCNT_CNT_THR_EVENT_U0_INT_RAW_S  0
/** PCNT_CNT_THR_EVENT_U1_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  The raw interrupt status bit for the PCNT_CNT_THR_EVENT_U1_INT interrupt.
 */
#define PCNT_CNT_THR_EVENT_U1_INT_RAW    (BIT(1))
#define PCNT_CNT_THR_EVENT_U1_INT_RAW_M  (PCNT_CNT_THR_EVENT_U1_INT_RAW_V << PCNT_CNT_THR_EVENT_U1_INT_RAW_S)
#define PCNT_CNT_THR_EVENT_U1_INT_RAW_V  0x00000001U
#define PCNT_CNT_THR_EVENT_U1_INT_RAW_S  1
/** PCNT_CNT_THR_EVENT_U2_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  The raw interrupt status bit for the PCNT_CNT_THR_EVENT_U2_INT interrupt.
 */
#define PCNT_CNT_THR_EVENT_U2_INT_RAW    (BIT(2))
#define PCNT_CNT_THR_EVENT_U2_INT_RAW_M  (PCNT_CNT_THR_EVENT_U2_INT_RAW_V << PCNT_CNT_THR_EVENT_U2_INT_RAW_S)
#define PCNT_CNT_THR_EVENT_U2_INT_RAW_V  0x00000001U
#define PCNT_CNT_THR_EVENT_U2_INT_RAW_S  2
/** PCNT_CNT_THR_EVENT_U3_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  The raw interrupt status bit for the PCNT_CNT_THR_EVENT_U3_INT interrupt.
 */
#define PCNT_CNT_THR_EVENT_U3_INT_RAW    (BIT(3))
#define PCNT_CNT_THR_EVENT_U3_INT_RAW_M  (PCNT_CNT_THR_EVENT_U3_INT_RAW_V << PCNT_CNT_THR_EVENT_U3_INT_RAW_S)
#define PCNT_CNT_THR_EVENT_U3_INT_RAW_V  0x00000001U
#define PCNT_CNT_THR_EVENT_U3_INT_RAW_S  3

/** PCNT_INT_ST_REG register
 *  Interrupt status register
 */
#define PCNT_INT_ST_REG (DR_REG_PCNT_BASE + 0x44)
/** PCNT_CNT_THR_EVENT_U0_INT_ST : RO; bitpos: [0]; default: 0;
 *  The masked interrupt status bit for the PCNT_CNT_THR_EVENT_U0_INT interrupt.
 */
#define PCNT_CNT_THR_EVENT_U0_INT_ST    (BIT(0))
#define PCNT_CNT_THR_EVENT_U0_INT_ST_M  (PCNT_CNT_THR_EVENT_U0_INT_ST_V << PCNT_CNT_THR_EVENT_U0_INT_ST_S)
#define PCNT_CNT_THR_EVENT_U0_INT_ST_V  0x00000001U
#define PCNT_CNT_THR_EVENT_U0_INT_ST_S  0
/** PCNT_CNT_THR_EVENT_U1_INT_ST : RO; bitpos: [1]; default: 0;
 *  The masked interrupt status bit for the PCNT_CNT_THR_EVENT_U1_INT interrupt.
 */
#define PCNT_CNT_THR_EVENT_U1_INT_ST    (BIT(1))
#define PCNT_CNT_THR_EVENT_U1_INT_ST_M  (PCNT_CNT_THR_EVENT_U1_INT_ST_V << PCNT_CNT_THR_EVENT_U1_INT_ST_S)
#define PCNT_CNT_THR_EVENT_U1_INT_ST_V  0x00000001U
#define PCNT_CNT_THR_EVENT_U1_INT_ST_S  1
/** PCNT_CNT_THR_EVENT_U2_INT_ST : RO; bitpos: [2]; default: 0;
 *  The masked interrupt status bit for the PCNT_CNT_THR_EVENT_U2_INT interrupt.
 */
#define PCNT_CNT_THR_EVENT_U2_INT_ST    (BIT(2))
#define PCNT_CNT_THR_EVENT_U2_INT_ST_M  (PCNT_CNT_THR_EVENT_U2_INT_ST_V << PCNT_CNT_THR_EVENT_U2_INT_ST_S)
#define PCNT_CNT_THR_EVENT_U2_INT_ST_V  0x00000001U
#define PCNT_CNT_THR_EVENT_U2_INT_ST_S  2
/** PCNT_CNT_THR_EVENT_U3_INT_ST : RO; bitpos: [3]; default: 0;
 *  The masked interrupt status bit for the PCNT_CNT_THR_EVENT_U3_INT interrupt.
 */
#define PCNT_CNT_THR_EVENT_U3_INT_ST    (BIT(3))
#define PCNT_CNT_THR_EVENT_U3_INT_ST_M  (PCNT_CNT_THR_EVENT_U3_INT_ST_V << PCNT_CNT_THR_EVENT_U3_INT_ST_S)
#define PCNT_CNT_THR_EVENT_U3_INT_ST_V  0x00000001U
#define PCNT_CNT_THR_EVENT_U3_INT_ST_S  3

/** PCNT_INT_ENA_REG register
 *  Interrupt enable register
 */
#define PCNT_INT_ENA_REG (DR_REG_PCNT_BASE + 0x48)
/** PCNT_CNT_THR_EVENT_U0_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  The interrupt enable bit for the PCNT_CNT_THR_EVENT_U0_INT interrupt.
 */
#define PCNT_CNT_THR_EVENT_U0_INT_ENA    (BIT(0))
#define PCNT_CNT_THR_EVENT_U0_INT_ENA_M  (PCNT_CNT_THR_EVENT_U0_INT_ENA_V << PCNT_CNT_THR_EVENT_U0_INT_ENA_S)
#define PCNT_CNT_THR_EVENT_U0_INT_ENA_V  0x00000001U
#define PCNT_CNT_THR_EVENT_U0_INT_ENA_S  0
/** PCNT_CNT_THR_EVENT_U1_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  The interrupt enable bit for the PCNT_CNT_THR_EVENT_U1_INT interrupt.
 */
#define PCNT_CNT_THR_EVENT_U1_INT_ENA    (BIT(1))
#define PCNT_CNT_THR_EVENT_U1_INT_ENA_M  (PCNT_CNT_THR_EVENT_U1_INT_ENA_V << PCNT_CNT_THR_EVENT_U1_INT_ENA_S)
#define PCNT_CNT_THR_EVENT_U1_INT_ENA_V  0x00000001U
#define PCNT_CNT_THR_EVENT_U1_INT_ENA_S  1
/** PCNT_CNT_THR_EVENT_U2_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  The interrupt enable bit for the PCNT_CNT_THR_EVENT_U2_INT interrupt.
 */
#define PCNT_CNT_THR_EVENT_U2_INT_ENA    (BIT(2))
#define PCNT_CNT_THR_EVENT_U2_INT_ENA_M  (PCNT_CNT_THR_EVENT_U2_INT_ENA_V << PCNT_CNT_THR_EVENT_U2_INT_ENA_S)
#define PCNT_CNT_THR_EVENT_U2_INT_ENA_V  0x00000001U
#define PCNT_CNT_THR_EVENT_U2_INT_ENA_S  2
/** PCNT_CNT_THR_EVENT_U3_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  The interrupt enable bit for the PCNT_CNT_THR_EVENT_U3_INT interrupt.
 */
#define PCNT_CNT_THR_EVENT_U3_INT_ENA    (BIT(3))
#define PCNT_CNT_THR_EVENT_U3_INT_ENA_M  (PCNT_CNT_THR_EVENT_U3_INT_ENA_V << PCNT_CNT_THR_EVENT_U3_INT_ENA_S)
#define PCNT_CNT_THR_EVENT_U3_INT_ENA_V  0x00000001U
#define PCNT_CNT_THR_EVENT_U3_INT_ENA_S  3

/** PCNT_INT_CLR_REG register
 *  Interrupt clear register
 */
#define PCNT_INT_CLR_REG (DR_REG_PCNT_BASE + 0x4c)
/** PCNT_CNT_THR_EVENT_U0_INT_CLR : WO; bitpos: [0]; default: 0;
 *  Set this bit to clear the PCNT_CNT_THR_EVENT_U0_INT interrupt.
 */
#define PCNT_CNT_THR_EVENT_U0_INT_CLR    (BIT(0))
#define PCNT_CNT_THR_EVENT_U0_INT_CLR_M  (PCNT_CNT_THR_EVENT_U0_INT_CLR_V << PCNT_CNT_THR_EVENT_U0_INT_CLR_S)
#define PCNT_CNT_THR_EVENT_U0_INT_CLR_V  0x00000001U
#define PCNT_CNT_THR_EVENT_U0_INT_CLR_S  0
/** PCNT_CNT_THR_EVENT_U1_INT_CLR : WO; bitpos: [1]; default: 0;
 *  Set this bit to clear the PCNT_CNT_THR_EVENT_U1_INT interrupt.
 */
#define PCNT_CNT_THR_EVENT_U1_INT_CLR    (BIT(1))
#define PCNT_CNT_THR_EVENT_U1_INT_CLR_M  (PCNT_CNT_THR_EVENT_U1_INT_CLR_V << PCNT_CNT_THR_EVENT_U1_INT_CLR_S)
#define PCNT_CNT_THR_EVENT_U1_INT_CLR_V  0x00000001U
#define PCNT_CNT_THR_EVENT_U1_INT_CLR_S  1
/** PCNT_CNT_THR_EVENT_U2_INT_CLR : WO; bitpos: [2]; default: 0;
 *  Set this bit to clear the PCNT_CNT_THR_EVENT_U2_INT interrupt.
 */
#define PCNT_CNT_THR_EVENT_U2_INT_CLR    (BIT(2))
#define PCNT_CNT_THR_EVENT_U2_INT_CLR_M  (PCNT_CNT_THR_EVENT_U2_INT_CLR_V << PCNT_CNT_THR_EVENT_U2_INT_CLR_S)
#define PCNT_CNT_THR_EVENT_U2_INT_CLR_V  0x00000001U
#define PCNT_CNT_THR_EVENT_U2_INT_CLR_S  2
/** PCNT_CNT_THR_EVENT_U3_INT_CLR : WO; bitpos: [3]; default: 0;
 *  Set this bit to clear the PCNT_CNT_THR_EVENT_U3_INT interrupt.
 */
#define PCNT_CNT_THR_EVENT_U3_INT_CLR    (BIT(3))
#define PCNT_CNT_THR_EVENT_U3_INT_CLR_M  (PCNT_CNT_THR_EVENT_U3_INT_CLR_V << PCNT_CNT_THR_EVENT_U3_INT_CLR_S)
#define PCNT_CNT_THR_EVENT_U3_INT_CLR_V  0x00000001U
#define PCNT_CNT_THR_EVENT_U3_INT_CLR_S  3

/** PCNT_U0_STATUS_REG register
 *  PNCT UNIT0 status register
 */
#define PCNT_U0_STATUS_REG (DR_REG_PCNT_BASE + 0x50)
/** PCNT_CNT_THR_ZERO_MODE_U0 : RO; bitpos: [1:0]; default: 0;
 *  The pulse counter status of PCNT_U0 corresponding to 0. 0: pulse counter decreases
 *  from positive to 0. 1: pulse counter increases from negative to 0. 2: pulse counter
 *  is negative. 3: pulse counter is positive.
 */
#define PCNT_CNT_THR_ZERO_MODE_U0    0x00000003U
#define PCNT_CNT_THR_ZERO_MODE_U0_M  (PCNT_CNT_THR_ZERO_MODE_U0_V << PCNT_CNT_THR_ZERO_MODE_U0_S)
#define PCNT_CNT_THR_ZERO_MODE_U0_V  0x00000003U
#define PCNT_CNT_THR_ZERO_MODE_U0_S  0
/** PCNT_CNT_THR_THRES1_LAT_U0 : RO; bitpos: [2]; default: 0;
 *  The latched value of thres1 event of PCNT_U0 when threshold event interrupt is
 *  valid. 1: the current pulse counter equals to thres1 and thres1 event is valid. 0:
 *  others
 */
#define PCNT_CNT_THR_THRES1_LAT_U0    (BIT(2))
#define PCNT_CNT_THR_THRES1_LAT_U0_M  (PCNT_CNT_THR_THRES1_LAT_U0_V << PCNT_CNT_THR_THRES1_LAT_U0_S)
#define PCNT_CNT_THR_THRES1_LAT_U0_V  0x00000001U
#define PCNT_CNT_THR_THRES1_LAT_U0_S  2
/** PCNT_CNT_THR_THRES0_LAT_U0 : RO; bitpos: [3]; default: 0;
 *  The latched value of thres0 event of PCNT_U0 when threshold event interrupt is
 *  valid. 1: the current pulse counter equals to thres0 and thres0 event is valid. 0:
 *  others
 */
#define PCNT_CNT_THR_THRES0_LAT_U0    (BIT(3))
#define PCNT_CNT_THR_THRES0_LAT_U0_M  (PCNT_CNT_THR_THRES0_LAT_U0_V << PCNT_CNT_THR_THRES0_LAT_U0_S)
#define PCNT_CNT_THR_THRES0_LAT_U0_V  0x00000001U
#define PCNT_CNT_THR_THRES0_LAT_U0_S  3
/** PCNT_CNT_THR_L_LIM_LAT_U0 : RO; bitpos: [4]; default: 0;
 *  The latched value of low limit event of PCNT_U0 when threshold event interrupt is
 *  valid. 1: the current pulse counter equals to thr_l_lim and low limit event is
 *  valid. 0: others
 */
#define PCNT_CNT_THR_L_LIM_LAT_U0    (BIT(4))
#define PCNT_CNT_THR_L_LIM_LAT_U0_M  (PCNT_CNT_THR_L_LIM_LAT_U0_V << PCNT_CNT_THR_L_LIM_LAT_U0_S)
#define PCNT_CNT_THR_L_LIM_LAT_U0_V  0x00000001U
#define PCNT_CNT_THR_L_LIM_LAT_U0_S  4
/** PCNT_CNT_THR_H_LIM_LAT_U0 : RO; bitpos: [5]; default: 0;
 *  The latched value of high limit event of PCNT_U0 when threshold event interrupt is
 *  valid. 1: the current pulse counter equals to thr_h_lim and high limit event is
 *  valid. 0: others
 */
#define PCNT_CNT_THR_H_LIM_LAT_U0    (BIT(5))
#define PCNT_CNT_THR_H_LIM_LAT_U0_M  (PCNT_CNT_THR_H_LIM_LAT_U0_V << PCNT_CNT_THR_H_LIM_LAT_U0_S)
#define PCNT_CNT_THR_H_LIM_LAT_U0_V  0x00000001U
#define PCNT_CNT_THR_H_LIM_LAT_U0_S  5
/** PCNT_CNT_THR_ZERO_LAT_U0 : RO; bitpos: [6]; default: 0;
 *  The latched value of zero threshold event of PCNT_U0 when threshold event interrupt
 *  is valid. 1: the current pulse counter equals to 0 and zero threshold event is
 *  valid. 0: others
 */
#define PCNT_CNT_THR_ZERO_LAT_U0    (BIT(6))
#define PCNT_CNT_THR_ZERO_LAT_U0_M  (PCNT_CNT_THR_ZERO_LAT_U0_V << PCNT_CNT_THR_ZERO_LAT_U0_S)
#define PCNT_CNT_THR_ZERO_LAT_U0_V  0x00000001U
#define PCNT_CNT_THR_ZERO_LAT_U0_S  6
/** PCNT_CNT_THR_STEP_LIM_LAT_U0 : RO; bitpos: [7]; default: 0;
 *  The latched value of step counter limit event of PCNT_U0 when step counter event
 *  interrupt is valid. 1: the current pulse counter equals to reg_cnt_step_lim and
 *  step counter event is valid. 0: others
 */
#define PCNT_CNT_THR_STEP_LIM_LAT_U0    (BIT(7))
#define PCNT_CNT_THR_STEP_LIM_LAT_U0_M  (PCNT_CNT_THR_STEP_LIM_LAT_U0_V << PCNT_CNT_THR_STEP_LIM_LAT_U0_S)
#define PCNT_CNT_THR_STEP_LIM_LAT_U0_V  0x00000001U
#define PCNT_CNT_THR_STEP_LIM_LAT_U0_S  7
/** PCNT_CNT_THR_STEP_LAT_U0 : RO; bitpos: [8]; default: 0;
 *  The latched value of step counter event of PCNT_U0 when step counter event
 *  interrupt is valid. 1: the current pulse counter increment equals to reg_cnt_step
 *  and step counter event is valid. 0: others
 */
#define PCNT_CNT_THR_STEP_LAT_U0    (BIT(8))
#define PCNT_CNT_THR_STEP_LAT_U0_M  (PCNT_CNT_THR_STEP_LAT_U0_V << PCNT_CNT_THR_STEP_LAT_U0_S)
#define PCNT_CNT_THR_STEP_LAT_U0_V  0x00000001U
#define PCNT_CNT_THR_STEP_LAT_U0_S  8

/** PCNT_U1_STATUS_REG register
 *  PNCT UNIT1 status register
 */
#define PCNT_U1_STATUS_REG (DR_REG_PCNT_BASE + 0x54)
/** PCNT_CNT_THR_ZERO_MODE_U1 : RO; bitpos: [1:0]; default: 0;
 *  The pulse counter status of PCNT_U1 corresponding to 0. 0: pulse counter decreases
 *  from positive to 0. 1: pulse counter increases from negative to 0. 2: pulse counter
 *  is negative. 3: pulse counter is positive.
 */
#define PCNT_CNT_THR_ZERO_MODE_U1    0x00000003U
#define PCNT_CNT_THR_ZERO_MODE_U1_M  (PCNT_CNT_THR_ZERO_MODE_U1_V << PCNT_CNT_THR_ZERO_MODE_U1_S)
#define PCNT_CNT_THR_ZERO_MODE_U1_V  0x00000003U
#define PCNT_CNT_THR_ZERO_MODE_U1_S  0
/** PCNT_CNT_THR_THRES1_LAT_U1 : RO; bitpos: [2]; default: 0;
 *  The latched value of thres1 event of PCNT_U1 when threshold event interrupt is
 *  valid. 1: the current pulse counter equals to thres1 and thres1 event is valid. 0:
 *  others
 */
#define PCNT_CNT_THR_THRES1_LAT_U1    (BIT(2))
#define PCNT_CNT_THR_THRES1_LAT_U1_M  (PCNT_CNT_THR_THRES1_LAT_U1_V << PCNT_CNT_THR_THRES1_LAT_U1_S)
#define PCNT_CNT_THR_THRES1_LAT_U1_V  0x00000001U
#define PCNT_CNT_THR_THRES1_LAT_U1_S  2
/** PCNT_CNT_THR_THRES0_LAT_U1 : RO; bitpos: [3]; default: 0;
 *  The latched value of thres0 event of PCNT_U1 when threshold event interrupt is
 *  valid. 1: the current pulse counter equals to thres0 and thres0 event is valid. 0:
 *  others
 */
#define PCNT_CNT_THR_THRES0_LAT_U1    (BIT(3))
#define PCNT_CNT_THR_THRES0_LAT_U1_M  (PCNT_CNT_THR_THRES0_LAT_U1_V << PCNT_CNT_THR_THRES0_LAT_U1_S)
#define PCNT_CNT_THR_THRES0_LAT_U1_V  0x00000001U
#define PCNT_CNT_THR_THRES0_LAT_U1_S  3
/** PCNT_CNT_THR_L_LIM_LAT_U1 : RO; bitpos: [4]; default: 0;
 *  The latched value of low limit event of PCNT_U1 when threshold event interrupt is
 *  valid. 1: the current pulse counter equals to thr_l_lim and low limit event is
 *  valid. 0: others
 */
#define PCNT_CNT_THR_L_LIM_LAT_U1    (BIT(4))
#define PCNT_CNT_THR_L_LIM_LAT_U1_M  (PCNT_CNT_THR_L_LIM_LAT_U1_V << PCNT_CNT_THR_L_LIM_LAT_U1_S)
#define PCNT_CNT_THR_L_LIM_LAT_U1_V  0x00000001U
#define PCNT_CNT_THR_L_LIM_LAT_U1_S  4
/** PCNT_CNT_THR_H_LIM_LAT_U1 : RO; bitpos: [5]; default: 0;
 *  The latched value of high limit event of PCNT_U1 when threshold event interrupt is
 *  valid. 1: the current pulse counter equals to thr_h_lim and high limit event is
 *  valid. 0: others
 */
#define PCNT_CNT_THR_H_LIM_LAT_U1    (BIT(5))
#define PCNT_CNT_THR_H_LIM_LAT_U1_M  (PCNT_CNT_THR_H_LIM_LAT_U1_V << PCNT_CNT_THR_H_LIM_LAT_U1_S)
#define PCNT_CNT_THR_H_LIM_LAT_U1_V  0x00000001U
#define PCNT_CNT_THR_H_LIM_LAT_U1_S  5
/** PCNT_CNT_THR_ZERO_LAT_U1 : RO; bitpos: [6]; default: 0;
 *  The latched value of zero threshold event of PCNT_U1 when threshold event interrupt
 *  is valid. 1: the current pulse counter equals to 0 and zero threshold event is
 *  valid. 0: others
 */
#define PCNT_CNT_THR_ZERO_LAT_U1    (BIT(6))
#define PCNT_CNT_THR_ZERO_LAT_U1_M  (PCNT_CNT_THR_ZERO_LAT_U1_V << PCNT_CNT_THR_ZERO_LAT_U1_S)
#define PCNT_CNT_THR_ZERO_LAT_U1_V  0x00000001U
#define PCNT_CNT_THR_ZERO_LAT_U1_S  6

/** PCNT_U2_STATUS_REG register
 *  PNCT UNIT2 status register
 */
#define PCNT_U2_STATUS_REG (DR_REG_PCNT_BASE + 0x58)
/** PCNT_CNT_THR_ZERO_MODE_U2 : RO; bitpos: [1:0]; default: 0;
 *  The pulse counter status of PCNT_U2 corresponding to 0. 0: pulse counter decreases
 *  from positive to 0. 1: pulse counter increases from negative to 0. 2: pulse counter
 *  is negative. 3: pulse counter is positive.
 */
#define PCNT_CNT_THR_ZERO_MODE_U2    0x00000003U
#define PCNT_CNT_THR_ZERO_MODE_U2_M  (PCNT_CNT_THR_ZERO_MODE_U2_V << PCNT_CNT_THR_ZERO_MODE_U2_S)
#define PCNT_CNT_THR_ZERO_MODE_U2_V  0x00000003U
#define PCNT_CNT_THR_ZERO_MODE_U2_S  0
/** PCNT_CNT_THR_THRES1_LAT_U2 : RO; bitpos: [2]; default: 0;
 *  The latched value of thres1 event of PCNT_U2 when threshold event interrupt is
 *  valid. 1: the current pulse counter equals to thres1 and thres1 event is valid. 0:
 *  others
 */
#define PCNT_CNT_THR_THRES1_LAT_U2    (BIT(2))
#define PCNT_CNT_THR_THRES1_LAT_U2_M  (PCNT_CNT_THR_THRES1_LAT_U2_V << PCNT_CNT_THR_THRES1_LAT_U2_S)
#define PCNT_CNT_THR_THRES1_LAT_U2_V  0x00000001U
#define PCNT_CNT_THR_THRES1_LAT_U2_S  2
/** PCNT_CNT_THR_THRES0_LAT_U2 : RO; bitpos: [3]; default: 0;
 *  The latched value of thres0 event of PCNT_U2 when threshold event interrupt is
 *  valid. 1: the current pulse counter equals to thres0 and thres0 event is valid. 0:
 *  others
 */
#define PCNT_CNT_THR_THRES0_LAT_U2    (BIT(3))
#define PCNT_CNT_THR_THRES0_LAT_U2_M  (PCNT_CNT_THR_THRES0_LAT_U2_V << PCNT_CNT_THR_THRES0_LAT_U2_S)
#define PCNT_CNT_THR_THRES0_LAT_U2_V  0x00000001U
#define PCNT_CNT_THR_THRES0_LAT_U2_S  3
/** PCNT_CNT_THR_L_LIM_LAT_U2 : RO; bitpos: [4]; default: 0;
 *  The latched value of low limit event of PCNT_U2 when threshold event interrupt is
 *  valid. 1: the current pulse counter equals to thr_l_lim and low limit event is
 *  valid. 0: others
 */
#define PCNT_CNT_THR_L_LIM_LAT_U2    (BIT(4))
#define PCNT_CNT_THR_L_LIM_LAT_U2_M  (PCNT_CNT_THR_L_LIM_LAT_U2_V << PCNT_CNT_THR_L_LIM_LAT_U2_S)
#define PCNT_CNT_THR_L_LIM_LAT_U2_V  0x00000001U
#define PCNT_CNT_THR_L_LIM_LAT_U2_S  4
/** PCNT_CNT_THR_H_LIM_LAT_U2 : RO; bitpos: [5]; default: 0;
 *  The latched value of high limit event of PCNT_U2 when threshold event interrupt is
 *  valid. 1: the current pulse counter equals to thr_h_lim and high limit event is
 *  valid. 0: others
 */
#define PCNT_CNT_THR_H_LIM_LAT_U2    (BIT(5))
#define PCNT_CNT_THR_H_LIM_LAT_U2_M  (PCNT_CNT_THR_H_LIM_LAT_U2_V << PCNT_CNT_THR_H_LIM_LAT_U2_S)
#define PCNT_CNT_THR_H_LIM_LAT_U2_V  0x00000001U
#define PCNT_CNT_THR_H_LIM_LAT_U2_S  5
/** PCNT_CNT_THR_ZERO_LAT_U2 : RO; bitpos: [6]; default: 0;
 *  The latched value of zero threshold event of PCNT_U2 when threshold event interrupt
 *  is valid. 1: the current pulse counter equals to 0 and zero threshold event is
 *  valid. 0: others
 */
#define PCNT_CNT_THR_ZERO_LAT_U2    (BIT(6))
#define PCNT_CNT_THR_ZERO_LAT_U2_M  (PCNT_CNT_THR_ZERO_LAT_U2_V << PCNT_CNT_THR_ZERO_LAT_U2_S)
#define PCNT_CNT_THR_ZERO_LAT_U2_V  0x00000001U
#define PCNT_CNT_THR_ZERO_LAT_U2_S  6

/** PCNT_U3_STATUS_REG register
 *  PNCT UNIT3 status register
 */
#define PCNT_U3_STATUS_REG (DR_REG_PCNT_BASE + 0x5c)
/** PCNT_CNT_THR_ZERO_MODE_U3 : RO; bitpos: [1:0]; default: 0;
 *  The pulse counter status of PCNT_U3 corresponding to 0. 0: pulse counter decreases
 *  from positive to 0. 1: pulse counter increases from negative to 0. 2: pulse counter
 *  is negative. 3: pulse counter is positive.
 */
#define PCNT_CNT_THR_ZERO_MODE_U3    0x00000003U
#define PCNT_CNT_THR_ZERO_MODE_U3_M  (PCNT_CNT_THR_ZERO_MODE_U3_V << PCNT_CNT_THR_ZERO_MODE_U3_S)
#define PCNT_CNT_THR_ZERO_MODE_U3_V  0x00000003U
#define PCNT_CNT_THR_ZERO_MODE_U3_S  0
/** PCNT_CNT_THR_THRES1_LAT_U3 : RO; bitpos: [2]; default: 0;
 *  The latched value of thres1 event of PCNT_U3 when threshold event interrupt is
 *  valid. 1: the current pulse counter equals to thres1 and thres1 event is valid. 0:
 *  others
 */
#define PCNT_CNT_THR_THRES1_LAT_U3    (BIT(2))
#define PCNT_CNT_THR_THRES1_LAT_U3_M  (PCNT_CNT_THR_THRES1_LAT_U3_V << PCNT_CNT_THR_THRES1_LAT_U3_S)
#define PCNT_CNT_THR_THRES1_LAT_U3_V  0x00000001U
#define PCNT_CNT_THR_THRES1_LAT_U3_S  2
/** PCNT_CNT_THR_THRES0_LAT_U3 : RO; bitpos: [3]; default: 0;
 *  The latched value of thres0 event of PCNT_U3 when threshold event interrupt is
 *  valid. 1: the current pulse counter equals to thres0 and thres0 event is valid. 0:
 *  others
 */
#define PCNT_CNT_THR_THRES0_LAT_U3    (BIT(3))
#define PCNT_CNT_THR_THRES0_LAT_U3_M  (PCNT_CNT_THR_THRES0_LAT_U3_V << PCNT_CNT_THR_THRES0_LAT_U3_S)
#define PCNT_CNT_THR_THRES0_LAT_U3_V  0x00000001U
#define PCNT_CNT_THR_THRES0_LAT_U3_S  3
/** PCNT_CNT_THR_L_LIM_LAT_U3 : RO; bitpos: [4]; default: 0;
 *  The latched value of low limit event of PCNT_U3 when threshold event interrupt is
 *  valid. 1: the current pulse counter equals to thr_l_lim and low limit event is
 *  valid. 0: others
 */
#define PCNT_CNT_THR_L_LIM_LAT_U3    (BIT(4))
#define PCNT_CNT_THR_L_LIM_LAT_U3_M  (PCNT_CNT_THR_L_LIM_LAT_U3_V << PCNT_CNT_THR_L_LIM_LAT_U3_S)
#define PCNT_CNT_THR_L_LIM_LAT_U3_V  0x00000001U
#define PCNT_CNT_THR_L_LIM_LAT_U3_S  4
/** PCNT_CNT_THR_H_LIM_LAT_U3 : RO; bitpos: [5]; default: 0;
 *  The latched value of high limit event of PCNT_U3 when threshold event interrupt is
 *  valid. 1: the current pulse counter equals to thr_h_lim and high limit event is
 *  valid. 0: others
 */
#define PCNT_CNT_THR_H_LIM_LAT_U3    (BIT(5))
#define PCNT_CNT_THR_H_LIM_LAT_U3_M  (PCNT_CNT_THR_H_LIM_LAT_U3_V << PCNT_CNT_THR_H_LIM_LAT_U3_S)
#define PCNT_CNT_THR_H_LIM_LAT_U3_V  0x00000001U
#define PCNT_CNT_THR_H_LIM_LAT_U3_S  5
/** PCNT_CNT_THR_ZERO_LAT_U3 : RO; bitpos: [6]; default: 0;
 *  The latched value of zero threshold event of PCNT_U3 when threshold event interrupt
 *  is valid. 1: the current pulse counter equals to 0 and zero threshold event is
 *  valid. 0: others
 */
#define PCNT_CNT_THR_ZERO_LAT_U3    (BIT(6))
#define PCNT_CNT_THR_ZERO_LAT_U3_M  (PCNT_CNT_THR_ZERO_LAT_U3_V << PCNT_CNT_THR_ZERO_LAT_U3_S)
#define PCNT_CNT_THR_ZERO_LAT_U3_V  0x00000001U
#define PCNT_CNT_THR_ZERO_LAT_U3_S  6

/** PCNT_CTRL_REG register
 *  Control register for all counters
 */
#define PCNT_CTRL_REG (DR_REG_PCNT_BASE + 0x60)
/** PCNT_PULSE_CNT_RST_U0 : R/W; bitpos: [0]; default: 1;
 *  Set this bit to clear unit 0's counter.
 */
#define PCNT_PULSE_CNT_RST_U0    (BIT(0))
#define PCNT_PULSE_CNT_RST_U0_M  (PCNT_PULSE_CNT_RST_U0_V << PCNT_PULSE_CNT_RST_U0_S)
#define PCNT_PULSE_CNT_RST_U0_V  0x00000001U
#define PCNT_PULSE_CNT_RST_U0_S  0
/** PCNT_CNT_PAUSE_U0 : R/W; bitpos: [1]; default: 0;
 *  Set this bit to freeze unit 0's counter.
 */
#define PCNT_CNT_PAUSE_U0    (BIT(1))
#define PCNT_CNT_PAUSE_U0_M  (PCNT_CNT_PAUSE_U0_V << PCNT_CNT_PAUSE_U0_S)
#define PCNT_CNT_PAUSE_U0_V  0x00000001U
#define PCNT_CNT_PAUSE_U0_S  1
/** PCNT_PULSE_CNT_RST_U1 : R/W; bitpos: [2]; default: 1;
 *  Set this bit to clear unit 1's counter.
 */
#define PCNT_PULSE_CNT_RST_U1    (BIT(2))
#define PCNT_PULSE_CNT_RST_U1_M  (PCNT_PULSE_CNT_RST_U1_V << PCNT_PULSE_CNT_RST_U1_S)
#define PCNT_PULSE_CNT_RST_U1_V  0x00000001U
#define PCNT_PULSE_CNT_RST_U1_S  2
/** PCNT_CNT_PAUSE_U1 : R/W; bitpos: [3]; default: 0;
 *  Set this bit to freeze unit 1's counter.
 */
#define PCNT_CNT_PAUSE_U1    (BIT(3))
#define PCNT_CNT_PAUSE_U1_M  (PCNT_CNT_PAUSE_U1_V << PCNT_CNT_PAUSE_U1_S)
#define PCNT_CNT_PAUSE_U1_V  0x00000001U
#define PCNT_CNT_PAUSE_U1_S  3
/** PCNT_PULSE_CNT_RST_U2 : R/W; bitpos: [4]; default: 1;
 *  Set this bit to clear unit 2's counter.
 */
#define PCNT_PULSE_CNT_RST_U2    (BIT(4))
#define PCNT_PULSE_CNT_RST_U2_M  (PCNT_PULSE_CNT_RST_U2_V << PCNT_PULSE_CNT_RST_U2_S)
#define PCNT_PULSE_CNT_RST_U2_V  0x00000001U
#define PCNT_PULSE_CNT_RST_U2_S  4
/** PCNT_CNT_PAUSE_U2 : R/W; bitpos: [5]; default: 0;
 *  Set this bit to freeze unit 2's counter.
 */
#define PCNT_CNT_PAUSE_U2    (BIT(5))
#define PCNT_CNT_PAUSE_U2_M  (PCNT_CNT_PAUSE_U2_V << PCNT_CNT_PAUSE_U2_S)
#define PCNT_CNT_PAUSE_U2_V  0x00000001U
#define PCNT_CNT_PAUSE_U2_S  5
/** PCNT_PULSE_CNT_RST_U3 : R/W; bitpos: [6]; default: 1;
 *  Set this bit to clear unit 3's counter.
 */
#define PCNT_PULSE_CNT_RST_U3    (BIT(6))
#define PCNT_PULSE_CNT_RST_U3_M  (PCNT_PULSE_CNT_RST_U3_V << PCNT_PULSE_CNT_RST_U3_S)
#define PCNT_PULSE_CNT_RST_U3_V  0x00000001U
#define PCNT_PULSE_CNT_RST_U3_S  6
/** PCNT_CNT_PAUSE_U3 : R/W; bitpos: [7]; default: 0;
 *  Set this bit to freeze unit 3's counter.
 */
#define PCNT_CNT_PAUSE_U3    (BIT(7))
#define PCNT_CNT_PAUSE_U3_M  (PCNT_CNT_PAUSE_U3_V << PCNT_CNT_PAUSE_U3_S)
#define PCNT_CNT_PAUSE_U3_V  0x00000001U
#define PCNT_CNT_PAUSE_U3_S  7
/** PCNT_DALTA_CHANGE_EN_U0 : R/W; bitpos: [8]; default: 0;
 *  Configures this bit to enable unit 0's step comparator.
 */
#define PCNT_DALTA_CHANGE_EN_U0    (BIT(8))
#define PCNT_DALTA_CHANGE_EN_U0_M  (PCNT_DALTA_CHANGE_EN_U0_V << PCNT_DALTA_CHANGE_EN_U0_S)
#define PCNT_DALTA_CHANGE_EN_U0_V  0x00000001U
#define PCNT_DALTA_CHANGE_EN_U0_S  8
/** PCNT_DALTA_CHANGE_EN_U1 : R/W; bitpos: [9]; default: 0;
 *  Configures this bit to enable unit 1's step comparator.
 */
#define PCNT_DALTA_CHANGE_EN_U1    (BIT(9))
#define PCNT_DALTA_CHANGE_EN_U1_M  (PCNT_DALTA_CHANGE_EN_U1_V << PCNT_DALTA_CHANGE_EN_U1_S)
#define PCNT_DALTA_CHANGE_EN_U1_V  0x00000001U
#define PCNT_DALTA_CHANGE_EN_U1_S  9
/** PCNT_DALTA_CHANGE_EN_U2 : R/W; bitpos: [10]; default: 0;
 *  Configures this bit to enable unit 2's step comparator.
 */
#define PCNT_DALTA_CHANGE_EN_U2    (BIT(10))
#define PCNT_DALTA_CHANGE_EN_U2_M  (PCNT_DALTA_CHANGE_EN_U2_V << PCNT_DALTA_CHANGE_EN_U2_S)
#define PCNT_DALTA_CHANGE_EN_U2_V  0x00000001U
#define PCNT_DALTA_CHANGE_EN_U2_S  10
/** PCNT_DALTA_CHANGE_EN_U3 : R/W; bitpos: [11]; default: 0;
 *  Configures this bit to enable unit 3's step comparator.
 */
#define PCNT_DALTA_CHANGE_EN_U3    (BIT(11))
#define PCNT_DALTA_CHANGE_EN_U3_M  (PCNT_DALTA_CHANGE_EN_U3_V << PCNT_DALTA_CHANGE_EN_U3_S)
#define PCNT_DALTA_CHANGE_EN_U3_V  0x00000001U
#define PCNT_DALTA_CHANGE_EN_U3_S  11
/** PCNT_CLK_EN : R/W; bitpos: [16]; default: 0;
 *  The registers clock gate enable signal of PCNT module. 1: the registers can be read
 *  and written by application. 0: the registers can not be read or written by
 *  application
 */
#define PCNT_CLK_EN    (BIT(16))
#define PCNT_CLK_EN_M  (PCNT_CLK_EN_V << PCNT_CLK_EN_S)
#define PCNT_CLK_EN_V  0x00000001U
#define PCNT_CLK_EN_S  16

/** PCNT_U3_CHANGE_CONF_REG register
 *  Configuration register for unit $n's step value.
 */
#define PCNT_U3_CHANGE_CONF_REG (DR_REG_PCNT_BASE + 0x64)
/** PCNT_CNT_STEP_U3 : R/W; bitpos: [15:0]; default: 0;
 *  Configures the step value for unit 3.
 */
#define PCNT_CNT_STEP_U3    0x0000FFFFU
#define PCNT_CNT_STEP_U3_M  (PCNT_CNT_STEP_U3_V << PCNT_CNT_STEP_U3_S)
#define PCNT_CNT_STEP_U3_V  0x0000FFFFU
#define PCNT_CNT_STEP_U3_S  0
/** PCNT_CNT_STEP_LIM_U3 : R/W; bitpos: [31:16]; default: 0;
 *  Configures the step limit value for unit 3.
 */
#define PCNT_CNT_STEP_LIM_U3    0x0000FFFFU
#define PCNT_CNT_STEP_LIM_U3_M  (PCNT_CNT_STEP_LIM_U3_V << PCNT_CNT_STEP_LIM_U3_S)
#define PCNT_CNT_STEP_LIM_U3_V  0x0000FFFFU
#define PCNT_CNT_STEP_LIM_U3_S  16

/** PCNT_U2_CHANGE_CONF_REG register
 *  Configuration register for unit $n's step value.
 */
#define PCNT_U2_CHANGE_CONF_REG (DR_REG_PCNT_BASE + 0x68)
/** PCNT_CNT_STEP_U2 : R/W; bitpos: [15:0]; default: 0;
 *  Configures the step value for unit 2.
 */
#define PCNT_CNT_STEP_U2    0x0000FFFFU
#define PCNT_CNT_STEP_U2_M  (PCNT_CNT_STEP_U2_V << PCNT_CNT_STEP_U2_S)
#define PCNT_CNT_STEP_U2_V  0x0000FFFFU
#define PCNT_CNT_STEP_U2_S  0
/** PCNT_CNT_STEP_LIM_U2 : R/W; bitpos: [31:16]; default: 0;
 *  Configures the step limit value for unit 2.
 */
#define PCNT_CNT_STEP_LIM_U2    0x0000FFFFU
#define PCNT_CNT_STEP_LIM_U2_M  (PCNT_CNT_STEP_LIM_U2_V << PCNT_CNT_STEP_LIM_U2_S)
#define PCNT_CNT_STEP_LIM_U2_V  0x0000FFFFU
#define PCNT_CNT_STEP_LIM_U2_S  16

/** PCNT_U1_CHANGE_CONF_REG register
 *  Configuration register for unit $n's step value.
 */
#define PCNT_U1_CHANGE_CONF_REG (DR_REG_PCNT_BASE + 0x6c)
/** PCNT_CNT_STEP_U1 : R/W; bitpos: [15:0]; default: 0;
 *  Configures the step value for unit 1.
 */
#define PCNT_CNT_STEP_U1    0x0000FFFFU
#define PCNT_CNT_STEP_U1_M  (PCNT_CNT_STEP_U1_V << PCNT_CNT_STEP_U1_S)
#define PCNT_CNT_STEP_U1_V  0x0000FFFFU
#define PCNT_CNT_STEP_U1_S  0
/** PCNT_CNT_STEP_LIM_U1 : R/W; bitpos: [31:16]; default: 0;
 *  Configures the step limit value for unit 1.
 */
#define PCNT_CNT_STEP_LIM_U1    0x0000FFFFU
#define PCNT_CNT_STEP_LIM_U1_M  (PCNT_CNT_STEP_LIM_U1_V << PCNT_CNT_STEP_LIM_U1_S)
#define PCNT_CNT_STEP_LIM_U1_V  0x0000FFFFU
#define PCNT_CNT_STEP_LIM_U1_S  16

/** PCNT_U0_CHANGE_CONF_REG register
 *  Configuration register for unit $n's step value.
 */
#define PCNT_U0_CHANGE_CONF_REG (DR_REG_PCNT_BASE + 0x70)
/** PCNT_CNT_STEP_U0 : R/W; bitpos: [15:0]; default: 0;
 *  Configures the step value for unit 0.
 */
#define PCNT_CNT_STEP_U0    0x0000FFFFU
#define PCNT_CNT_STEP_U0_M  (PCNT_CNT_STEP_U0_V << PCNT_CNT_STEP_U0_S)
#define PCNT_CNT_STEP_U0_V  0x0000FFFFU
#define PCNT_CNT_STEP_U0_S  0
/** PCNT_CNT_STEP_LIM_U0 : R/W; bitpos: [31:16]; default: 0;
 *  Configures the step limit value for unit 0.
 */
#define PCNT_CNT_STEP_LIM_U0    0x0000FFFFU
#define PCNT_CNT_STEP_LIM_U0_M  (PCNT_CNT_STEP_LIM_U0_V << PCNT_CNT_STEP_LIM_U0_S)
#define PCNT_CNT_STEP_LIM_U0_V  0x0000FFFFU
#define PCNT_CNT_STEP_LIM_U0_S  16

/** PCNT_DATE_REG register
 *  PCNT version control register
 */
#define PCNT_DATE_REG (DR_REG_PCNT_BASE + 0xfc)
/** PCNT_DATE : R/W; bitpos: [31:0]; default: 403121664;
 *  This is the PCNT version control register.
 */
#define PCNT_DATE    0xFFFFFFFFU
#define PCNT_DATE_M  (PCNT_DATE_V << PCNT_DATE_S)
#define PCNT_DATE_V  0xFFFFFFFFU
#define PCNT_DATE_S  0

#ifdef __cplusplus
}
#endif
