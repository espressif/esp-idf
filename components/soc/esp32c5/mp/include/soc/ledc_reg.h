/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** LEDC_CH0_CONF0_REG register
 *  Configuration register 0 for channel 0
 */
#define LEDC_CH0_CONF0_REG (DR_REG_LEDC_BASE + 0x0)
/** LEDC_TIMER_SEL_CH0 : R/W; bitpos: [1:0]; default: 0;
 *  Configures which timer is channel 0 selected.\\0: Select timer0\\1: Select
 *  timer1\\2: Select timer2\\3: Select timer3
 */
#define LEDC_TIMER_SEL_CH0    0x00000003U
#define LEDC_TIMER_SEL_CH0_M  (LEDC_TIMER_SEL_CH0_V << LEDC_TIMER_SEL_CH0_S)
#define LEDC_TIMER_SEL_CH0_V  0x00000003U
#define LEDC_TIMER_SEL_CH0_S  0
/** LEDC_SIG_OUT_EN_CH0 : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable signal output on channel 0.\\0: Signal output
 *  disable\\1: Signal output enable
 */
#define LEDC_SIG_OUT_EN_CH0    (BIT(2))
#define LEDC_SIG_OUT_EN_CH0_M  (LEDC_SIG_OUT_EN_CH0_V << LEDC_SIG_OUT_EN_CH0_S)
#define LEDC_SIG_OUT_EN_CH0_V  0x00000001U
#define LEDC_SIG_OUT_EN_CH0_S  2
/** LEDC_IDLE_LV_CH0 : R/W; bitpos: [3]; default: 0;
 *  Configures the output value when channel 0 is inactive. Valid only when
 *  LEDC_SIG_OUT_EN_CH0 is 0.\\0: Output level is low\\1: Output level is high
 */
#define LEDC_IDLE_LV_CH0    (BIT(3))
#define LEDC_IDLE_LV_CH0_M  (LEDC_IDLE_LV_CH0_V << LEDC_IDLE_LV_CH0_S)
#define LEDC_IDLE_LV_CH0_V  0x00000001U
#define LEDC_IDLE_LV_CH0_S  3
/** LEDC_PARA_UP_CH0 : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to update LEDC_HPOINT_CH0, LEDC_DUTY_START_CH0,
 *  LEDC_SIG_OUT_EN_CH0, LEDC_TIMER_SEL_CH0, LEDC_OVF_CNT_EN_CH0 fields and duty cycle
 *  range configuration for channel 0, and will be automatically cleared by
 *  hardware.\\0: Invalid. No effect\\1: Update
 */
#define LEDC_PARA_UP_CH0    (BIT(4))
#define LEDC_PARA_UP_CH0_M  (LEDC_PARA_UP_CH0_V << LEDC_PARA_UP_CH0_S)
#define LEDC_PARA_UP_CH0_V  0x00000001U
#define LEDC_PARA_UP_CH0_S  4
/** LEDC_OVF_NUM_CH0 : R/W; bitpos: [14:5]; default: 0;
 *  Configures the maximum times of overflow minus 1.The LEDC_OVF_CNT_CH0_INT interrupt
 *  will be triggered when channel 0 overflows for (LEDC_OVF_NUM_CH0 + 1) times.
 */
#define LEDC_OVF_NUM_CH0    0x000003FFU
#define LEDC_OVF_NUM_CH0_M  (LEDC_OVF_NUM_CH0_V << LEDC_OVF_NUM_CH0_S)
#define LEDC_OVF_NUM_CH0_V  0x000003FFU
#define LEDC_OVF_NUM_CH0_S  5
/** LEDC_OVF_CNT_EN_CH0 : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable the ovf_cnt of channel 0.\\0: Disable\\1: Enable
 */
#define LEDC_OVF_CNT_EN_CH0    (BIT(15))
#define LEDC_OVF_CNT_EN_CH0_M  (LEDC_OVF_CNT_EN_CH0_V << LEDC_OVF_CNT_EN_CH0_S)
#define LEDC_OVF_CNT_EN_CH0_V  0x00000001U
#define LEDC_OVF_CNT_EN_CH0_S  15
/** LEDC_OVF_CNT_RESET_CH0 : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to reset the  ovf_cnt of channel 0.\\0: Invalid. No
 *  effect\\1: Reset the ovf_cnt
 */
#define LEDC_OVF_CNT_RESET_CH0    (BIT(16))
#define LEDC_OVF_CNT_RESET_CH0_M  (LEDC_OVF_CNT_RESET_CH0_V << LEDC_OVF_CNT_RESET_CH0_S)
#define LEDC_OVF_CNT_RESET_CH0_V  0x00000001U
#define LEDC_OVF_CNT_RESET_CH0_S  16

/** LEDC_CH0_HPOINT_REG register
 *  High point register for channel 0
 */
#define LEDC_CH0_HPOINT_REG (DR_REG_LEDC_BASE + 0x4)
/** LEDC_HPOINT_CH0 : R/W; bitpos: [19:0]; default: 0;
 *  Configures high point of signal output on channel 0. The output value changes to
 *  high when the selected timers has reached the value specified by this register.
 */
#define LEDC_HPOINT_CH0    0x000FFFFFU
#define LEDC_HPOINT_CH0_M  (LEDC_HPOINT_CH0_V << LEDC_HPOINT_CH0_S)
#define LEDC_HPOINT_CH0_V  0x000FFFFFU
#define LEDC_HPOINT_CH0_S  0

/** LEDC_CH0_DUTY_REG register
 *  Initial duty cycle register for channel 0
 */
#define LEDC_CH0_DUTY_REG (DR_REG_LEDC_BASE + 0x8)
/** LEDC_DUTY_CH0 : R/W; bitpos: [24:0]; default: 0;
 *  Configures the duty of signal output on channel 0.
 */
#define LEDC_DUTY_CH0    0x01FFFFFFU
#define LEDC_DUTY_CH0_M  (LEDC_DUTY_CH0_V << LEDC_DUTY_CH0_S)
#define LEDC_DUTY_CH0_V  0x01FFFFFFU
#define LEDC_DUTY_CH0_S  0

/** LEDC_CH0_CONF1_REG register
 *  Configuration register 1 for channel 0
 */
#define LEDC_CH0_CONF1_REG (DR_REG_LEDC_BASE + 0xc)
/** LEDC_DUTY_START_CH0 : R/W/SC; bitpos: [31]; default: 0;
 *  Configures whether the duty cycle fading configurations take effect.\\0: Not take
 *  effect\\1: Take effect
 */
#define LEDC_DUTY_START_CH0    (BIT(31))
#define LEDC_DUTY_START_CH0_M  (LEDC_DUTY_START_CH0_V << LEDC_DUTY_START_CH0_S)
#define LEDC_DUTY_START_CH0_V  0x00000001U
#define LEDC_DUTY_START_CH0_S  31

/** LEDC_CH0_DUTY_R_REG register
 *  Current duty cycle register for channel 0
 */
#define LEDC_CH0_DUTY_R_REG (DR_REG_LEDC_BASE + 0x10)
/** LEDC_DUTY_CH0_R : RO; bitpos: [24:0]; default: 0;
 *  Represents the current duty of output signal on channel 0.
 */
#define LEDC_DUTY_CH0_R    0x01FFFFFFU
#define LEDC_DUTY_CH0_R_M  (LEDC_DUTY_CH0_R_V << LEDC_DUTY_CH0_R_S)
#define LEDC_DUTY_CH0_R_V  0x01FFFFFFU
#define LEDC_DUTY_CH0_R_S  0

/** LEDC_CH1_CONF0_REG register
 *  Configuration register 0 for channel 1
 */
#define LEDC_CH1_CONF0_REG (DR_REG_LEDC_BASE + 0x14)
/** LEDC_TIMER_SEL_CH1 : R/W; bitpos: [1:0]; default: 0;
 *  Configures which timer is channel 1 selected.\\0: Select timer0\\1: Select
 *  timer1\\2: Select timer2\\3: Select timer3
 */
#define LEDC_TIMER_SEL_CH1    0x00000003U
#define LEDC_TIMER_SEL_CH1_M  (LEDC_TIMER_SEL_CH1_V << LEDC_TIMER_SEL_CH1_S)
#define LEDC_TIMER_SEL_CH1_V  0x00000003U
#define LEDC_TIMER_SEL_CH1_S  0
/** LEDC_SIG_OUT_EN_CH1 : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable signal output on channel 1.\\0: Signal output
 *  disable\\1: Signal output enable
 */
#define LEDC_SIG_OUT_EN_CH1    (BIT(2))
#define LEDC_SIG_OUT_EN_CH1_M  (LEDC_SIG_OUT_EN_CH1_V << LEDC_SIG_OUT_EN_CH1_S)
#define LEDC_SIG_OUT_EN_CH1_V  0x00000001U
#define LEDC_SIG_OUT_EN_CH1_S  2
/** LEDC_IDLE_LV_CH1 : R/W; bitpos: [3]; default: 0;
 *  Configures the output value when channel 1 is inactive. Valid only when
 *  LEDC_SIG_OUT_EN_CH1 is 0.\\0: Output level is low\\1: Output level is high
 */
#define LEDC_IDLE_LV_CH1    (BIT(3))
#define LEDC_IDLE_LV_CH1_M  (LEDC_IDLE_LV_CH1_V << LEDC_IDLE_LV_CH1_S)
#define LEDC_IDLE_LV_CH1_V  0x00000001U
#define LEDC_IDLE_LV_CH1_S  3
/** LEDC_PARA_UP_CH1 : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to update LEDC_HPOINT_CH1, LEDC_DUTY_START_CH1,
 *  LEDC_SIG_OUT_EN_CH1, LEDC_TIMER_SEL_CH1, LEDC_OVF_CNT_EN_CH1 fields and duty cycle
 *  range configuration for channel 1, and will be automatically cleared by
 *  hardware.\\0: Invalid. No effect\\1: Update
 */
#define LEDC_PARA_UP_CH1    (BIT(4))
#define LEDC_PARA_UP_CH1_M  (LEDC_PARA_UP_CH1_V << LEDC_PARA_UP_CH1_S)
#define LEDC_PARA_UP_CH1_V  0x00000001U
#define LEDC_PARA_UP_CH1_S  4
/** LEDC_OVF_NUM_CH1 : R/W; bitpos: [14:5]; default: 0;
 *  Configures the maximum times of overflow minus 1.The LEDC_OVF_CNT_CH1_INT interrupt
 *  will be triggered when channel 1 overflows for (LEDC_OVF_NUM_CH1 + 1) times.
 */
#define LEDC_OVF_NUM_CH1    0x000003FFU
#define LEDC_OVF_NUM_CH1_M  (LEDC_OVF_NUM_CH1_V << LEDC_OVF_NUM_CH1_S)
#define LEDC_OVF_NUM_CH1_V  0x000003FFU
#define LEDC_OVF_NUM_CH1_S  5
/** LEDC_OVF_CNT_EN_CH1 : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable the ovf_cnt of channel 1.\\0: Disable\\1: Enable
 */
#define LEDC_OVF_CNT_EN_CH1    (BIT(15))
#define LEDC_OVF_CNT_EN_CH1_M  (LEDC_OVF_CNT_EN_CH1_V << LEDC_OVF_CNT_EN_CH1_S)
#define LEDC_OVF_CNT_EN_CH1_V  0x00000001U
#define LEDC_OVF_CNT_EN_CH1_S  15
/** LEDC_OVF_CNT_RESET_CH1 : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to reset the  ovf_cnt of channel 1.\\0: Invalid. No
 *  effect\\1: Reset the ovf_cnt
 */
#define LEDC_OVF_CNT_RESET_CH1    (BIT(16))
#define LEDC_OVF_CNT_RESET_CH1_M  (LEDC_OVF_CNT_RESET_CH1_V << LEDC_OVF_CNT_RESET_CH1_S)
#define LEDC_OVF_CNT_RESET_CH1_V  0x00000001U
#define LEDC_OVF_CNT_RESET_CH1_S  16

/** LEDC_CH1_HPOINT_REG register
 *  High point register for channel 1
 */
#define LEDC_CH1_HPOINT_REG (DR_REG_LEDC_BASE + 0x18)
/** LEDC_HPOINT_CH1 : R/W; bitpos: [19:0]; default: 0;
 *  Configures high point of signal output on channel 1. The output value changes to
 *  high when the selected timers has reached the value specified by this register.
 */
#define LEDC_HPOINT_CH1    0x000FFFFFU
#define LEDC_HPOINT_CH1_M  (LEDC_HPOINT_CH1_V << LEDC_HPOINT_CH1_S)
#define LEDC_HPOINT_CH1_V  0x000FFFFFU
#define LEDC_HPOINT_CH1_S  0

/** LEDC_CH1_DUTY_REG register
 *  Initial duty cycle register for channel 1
 */
#define LEDC_CH1_DUTY_REG (DR_REG_LEDC_BASE + 0x1c)
/** LEDC_DUTY_CH1 : R/W; bitpos: [24:0]; default: 0;
 *  Configures the duty of signal output on channel 1.
 */
#define LEDC_DUTY_CH1    0x01FFFFFFU
#define LEDC_DUTY_CH1_M  (LEDC_DUTY_CH1_V << LEDC_DUTY_CH1_S)
#define LEDC_DUTY_CH1_V  0x01FFFFFFU
#define LEDC_DUTY_CH1_S  0

/** LEDC_CH1_CONF1_REG register
 *  Configuration register 1 for channel 1
 */
#define LEDC_CH1_CONF1_REG (DR_REG_LEDC_BASE + 0x20)
/** LEDC_DUTY_START_CH1 : R/W/SC; bitpos: [31]; default: 0;
 *  Configures whether the duty cycle fading configurations take effect.\\0: Not take
 *  effect\\1: Take effect
 */
#define LEDC_DUTY_START_CH1    (BIT(31))
#define LEDC_DUTY_START_CH1_M  (LEDC_DUTY_START_CH1_V << LEDC_DUTY_START_CH1_S)
#define LEDC_DUTY_START_CH1_V  0x00000001U
#define LEDC_DUTY_START_CH1_S  31

/** LEDC_CH1_DUTY_R_REG register
 *  Current duty cycle register for channel 1
 */
#define LEDC_CH1_DUTY_R_REG (DR_REG_LEDC_BASE + 0x24)
/** LEDC_DUTY_CH1_R : RO; bitpos: [24:0]; default: 0;
 *  Represents the current duty of output signal on channel 1.
 */
#define LEDC_DUTY_CH1_R    0x01FFFFFFU
#define LEDC_DUTY_CH1_R_M  (LEDC_DUTY_CH1_R_V << LEDC_DUTY_CH1_R_S)
#define LEDC_DUTY_CH1_R_V  0x01FFFFFFU
#define LEDC_DUTY_CH1_R_S  0

/** LEDC_CH2_CONF0_REG register
 *  Configuration register 0 for channel 2
 */
#define LEDC_CH2_CONF0_REG (DR_REG_LEDC_BASE + 0x28)
/** LEDC_TIMER_SEL_CH2 : R/W; bitpos: [1:0]; default: 0;
 *  Configures which timer is channel 2 selected.\\0: Select timer0\\1: Select
 *  timer1\\2: Select timer2\\3: Select timer3
 */
#define LEDC_TIMER_SEL_CH2    0x00000003U
#define LEDC_TIMER_SEL_CH2_M  (LEDC_TIMER_SEL_CH2_V << LEDC_TIMER_SEL_CH2_S)
#define LEDC_TIMER_SEL_CH2_V  0x00000003U
#define LEDC_TIMER_SEL_CH2_S  0
/** LEDC_SIG_OUT_EN_CH2 : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable signal output on channel 2.\\0: Signal output
 *  disable\\1: Signal output enable
 */
#define LEDC_SIG_OUT_EN_CH2    (BIT(2))
#define LEDC_SIG_OUT_EN_CH2_M  (LEDC_SIG_OUT_EN_CH2_V << LEDC_SIG_OUT_EN_CH2_S)
#define LEDC_SIG_OUT_EN_CH2_V  0x00000001U
#define LEDC_SIG_OUT_EN_CH2_S  2
/** LEDC_IDLE_LV_CH2 : R/W; bitpos: [3]; default: 0;
 *  Configures the output value when channel 2 is inactive. Valid only when
 *  LEDC_SIG_OUT_EN_CH2 is 0.\\0: Output level is low\\1: Output level is high
 */
#define LEDC_IDLE_LV_CH2    (BIT(3))
#define LEDC_IDLE_LV_CH2_M  (LEDC_IDLE_LV_CH2_V << LEDC_IDLE_LV_CH2_S)
#define LEDC_IDLE_LV_CH2_V  0x00000001U
#define LEDC_IDLE_LV_CH2_S  3
/** LEDC_PARA_UP_CH2 : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to update LEDC_HPOINT_CH2, LEDC_DUTY_START_CH2,
 *  LEDC_SIG_OUT_EN_CH2, LEDC_TIMER_SEL_CH2, LEDC_OVF_CNT_EN_CH2 fields and duty cycle
 *  range configuration for channel 2, and will be automatically cleared by
 *  hardware.\\0: Invalid. No effect\\1: Update
 */
#define LEDC_PARA_UP_CH2    (BIT(4))
#define LEDC_PARA_UP_CH2_M  (LEDC_PARA_UP_CH2_V << LEDC_PARA_UP_CH2_S)
#define LEDC_PARA_UP_CH2_V  0x00000001U
#define LEDC_PARA_UP_CH2_S  4
/** LEDC_OVF_NUM_CH2 : R/W; bitpos: [14:5]; default: 0;
 *  Configures the maximum times of overflow minus 1.The LEDC_OVF_CNT_CH2_INT interrupt
 *  will be triggered when channel 2 overflows for (LEDC_OVF_NUM_CH2 + 1) times.
 */
#define LEDC_OVF_NUM_CH2    0x000003FFU
#define LEDC_OVF_NUM_CH2_M  (LEDC_OVF_NUM_CH2_V << LEDC_OVF_NUM_CH2_S)
#define LEDC_OVF_NUM_CH2_V  0x000003FFU
#define LEDC_OVF_NUM_CH2_S  5
/** LEDC_OVF_CNT_EN_CH2 : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable the ovf_cnt of channel 2.\\0: Disable\\1: Enable
 */
#define LEDC_OVF_CNT_EN_CH2    (BIT(15))
#define LEDC_OVF_CNT_EN_CH2_M  (LEDC_OVF_CNT_EN_CH2_V << LEDC_OVF_CNT_EN_CH2_S)
#define LEDC_OVF_CNT_EN_CH2_V  0x00000001U
#define LEDC_OVF_CNT_EN_CH2_S  15
/** LEDC_OVF_CNT_RESET_CH2 : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to reset the  ovf_cnt of channel 2.\\0: Invalid. No
 *  effect\\1: Reset the ovf_cnt
 */
#define LEDC_OVF_CNT_RESET_CH2    (BIT(16))
#define LEDC_OVF_CNT_RESET_CH2_M  (LEDC_OVF_CNT_RESET_CH2_V << LEDC_OVF_CNT_RESET_CH2_S)
#define LEDC_OVF_CNT_RESET_CH2_V  0x00000001U
#define LEDC_OVF_CNT_RESET_CH2_S  16

/** LEDC_CH2_HPOINT_REG register
 *  High point register for channel 2
 */
#define LEDC_CH2_HPOINT_REG (DR_REG_LEDC_BASE + 0x2c)
/** LEDC_HPOINT_CH2 : R/W; bitpos: [19:0]; default: 0;
 *  Configures high point of signal output on channel 2. The output value changes to
 *  high when the selected timers has reached the value specified by this register.
 */
#define LEDC_HPOINT_CH2    0x000FFFFFU
#define LEDC_HPOINT_CH2_M  (LEDC_HPOINT_CH2_V << LEDC_HPOINT_CH2_S)
#define LEDC_HPOINT_CH2_V  0x000FFFFFU
#define LEDC_HPOINT_CH2_S  0

/** LEDC_CH2_DUTY_REG register
 *  Initial duty cycle register for channel 2
 */
#define LEDC_CH2_DUTY_REG (DR_REG_LEDC_BASE + 0x30)
/** LEDC_DUTY_CH2 : R/W; bitpos: [24:0]; default: 0;
 *  Configures the duty of signal output on channel 2.
 */
#define LEDC_DUTY_CH2    0x01FFFFFFU
#define LEDC_DUTY_CH2_M  (LEDC_DUTY_CH2_V << LEDC_DUTY_CH2_S)
#define LEDC_DUTY_CH2_V  0x01FFFFFFU
#define LEDC_DUTY_CH2_S  0

/** LEDC_CH2_CONF1_REG register
 *  Configuration register 1 for channel 2
 */
#define LEDC_CH2_CONF1_REG (DR_REG_LEDC_BASE + 0x34)
/** LEDC_DUTY_START_CH2 : R/W/SC; bitpos: [31]; default: 0;
 *  Configures whether the duty cycle fading configurations take effect.\\0: Not take
 *  effect\\1: Take effect
 */
#define LEDC_DUTY_START_CH2    (BIT(31))
#define LEDC_DUTY_START_CH2_M  (LEDC_DUTY_START_CH2_V << LEDC_DUTY_START_CH2_S)
#define LEDC_DUTY_START_CH2_V  0x00000001U
#define LEDC_DUTY_START_CH2_S  31

/** LEDC_CH2_DUTY_R_REG register
 *  Current duty cycle register for channel 2
 */
#define LEDC_CH2_DUTY_R_REG (DR_REG_LEDC_BASE + 0x38)
/** LEDC_DUTY_CH2_R : RO; bitpos: [24:0]; default: 0;
 *  Represents the current duty of output signal on channel 2.
 */
#define LEDC_DUTY_CH2_R    0x01FFFFFFU
#define LEDC_DUTY_CH2_R_M  (LEDC_DUTY_CH2_R_V << LEDC_DUTY_CH2_R_S)
#define LEDC_DUTY_CH2_R_V  0x01FFFFFFU
#define LEDC_DUTY_CH2_R_S  0

/** LEDC_CH3_CONF0_REG register
 *  Configuration register 0 for channel 3
 */
#define LEDC_CH3_CONF0_REG (DR_REG_LEDC_BASE + 0x3c)
/** LEDC_TIMER_SEL_CH3 : R/W; bitpos: [1:0]; default: 0;
 *  Configures which timer is channel 3 selected.\\0: Select timer0\\1: Select
 *  timer1\\2: Select timer2\\3: Select timer3
 */
#define LEDC_TIMER_SEL_CH3    0x00000003U
#define LEDC_TIMER_SEL_CH3_M  (LEDC_TIMER_SEL_CH3_V << LEDC_TIMER_SEL_CH3_S)
#define LEDC_TIMER_SEL_CH3_V  0x00000003U
#define LEDC_TIMER_SEL_CH3_S  0
/** LEDC_SIG_OUT_EN_CH3 : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable signal output on channel 3.\\0: Signal output
 *  disable\\1: Signal output enable
 */
#define LEDC_SIG_OUT_EN_CH3    (BIT(2))
#define LEDC_SIG_OUT_EN_CH3_M  (LEDC_SIG_OUT_EN_CH3_V << LEDC_SIG_OUT_EN_CH3_S)
#define LEDC_SIG_OUT_EN_CH3_V  0x00000001U
#define LEDC_SIG_OUT_EN_CH3_S  2
/** LEDC_IDLE_LV_CH3 : R/W; bitpos: [3]; default: 0;
 *  Configures the output value when channel 3 is inactive. Valid only when
 *  LEDC_SIG_OUT_EN_CH3 is 0.\\0: Output level is low\\1: Output level is high
 */
#define LEDC_IDLE_LV_CH3    (BIT(3))
#define LEDC_IDLE_LV_CH3_M  (LEDC_IDLE_LV_CH3_V << LEDC_IDLE_LV_CH3_S)
#define LEDC_IDLE_LV_CH3_V  0x00000001U
#define LEDC_IDLE_LV_CH3_S  3
/** LEDC_PARA_UP_CH3 : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to update LEDC_HPOINT_CH3, LEDC_DUTY_START_CH3,
 *  LEDC_SIG_OUT_EN_CH3, LEDC_TIMER_SEL_CH3, LEDC_OVF_CNT_EN_CH3 fields and duty cycle
 *  range configuration for channel 3, and will be automatically cleared by
 *  hardware.\\0: Invalid. No effect\\1: Update
 */
#define LEDC_PARA_UP_CH3    (BIT(4))
#define LEDC_PARA_UP_CH3_M  (LEDC_PARA_UP_CH3_V << LEDC_PARA_UP_CH3_S)
#define LEDC_PARA_UP_CH3_V  0x00000001U
#define LEDC_PARA_UP_CH3_S  4
/** LEDC_OVF_NUM_CH3 : R/W; bitpos: [14:5]; default: 0;
 *  Configures the maximum times of overflow minus 1.The LEDC_OVF_CNT_CH3_INT interrupt
 *  will be triggered when channel 3 overflows for (LEDC_OVF_NUM_CH3 + 1) times.
 */
#define LEDC_OVF_NUM_CH3    0x000003FFU
#define LEDC_OVF_NUM_CH3_M  (LEDC_OVF_NUM_CH3_V << LEDC_OVF_NUM_CH3_S)
#define LEDC_OVF_NUM_CH3_V  0x000003FFU
#define LEDC_OVF_NUM_CH3_S  5
/** LEDC_OVF_CNT_EN_CH3 : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable the ovf_cnt of channel 3.\\0: Disable\\1: Enable
 */
#define LEDC_OVF_CNT_EN_CH3    (BIT(15))
#define LEDC_OVF_CNT_EN_CH3_M  (LEDC_OVF_CNT_EN_CH3_V << LEDC_OVF_CNT_EN_CH3_S)
#define LEDC_OVF_CNT_EN_CH3_V  0x00000001U
#define LEDC_OVF_CNT_EN_CH3_S  15
/** LEDC_OVF_CNT_RESET_CH3 : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to reset the  ovf_cnt of channel 3.\\0: Invalid. No
 *  effect\\1: Reset the ovf_cnt
 */
#define LEDC_OVF_CNT_RESET_CH3    (BIT(16))
#define LEDC_OVF_CNT_RESET_CH3_M  (LEDC_OVF_CNT_RESET_CH3_V << LEDC_OVF_CNT_RESET_CH3_S)
#define LEDC_OVF_CNT_RESET_CH3_V  0x00000001U
#define LEDC_OVF_CNT_RESET_CH3_S  16

/** LEDC_CH3_HPOINT_REG register
 *  High point register for channel 3
 */
#define LEDC_CH3_HPOINT_REG (DR_REG_LEDC_BASE + 0x40)
/** LEDC_HPOINT_CH3 : R/W; bitpos: [19:0]; default: 0;
 *  Configures high point of signal output on channel 3. The output value changes to
 *  high when the selected timers has reached the value specified by this register.
 */
#define LEDC_HPOINT_CH3    0x000FFFFFU
#define LEDC_HPOINT_CH3_M  (LEDC_HPOINT_CH3_V << LEDC_HPOINT_CH3_S)
#define LEDC_HPOINT_CH3_V  0x000FFFFFU
#define LEDC_HPOINT_CH3_S  0

/** LEDC_CH3_DUTY_REG register
 *  Initial duty cycle register for channel 3
 */
#define LEDC_CH3_DUTY_REG (DR_REG_LEDC_BASE + 0x44)
/** LEDC_DUTY_CH3 : R/W; bitpos: [24:0]; default: 0;
 *  Configures the duty of signal output on channel 3.
 */
#define LEDC_DUTY_CH3    0x01FFFFFFU
#define LEDC_DUTY_CH3_M  (LEDC_DUTY_CH3_V << LEDC_DUTY_CH3_S)
#define LEDC_DUTY_CH3_V  0x01FFFFFFU
#define LEDC_DUTY_CH3_S  0

/** LEDC_CH3_CONF1_REG register
 *  Configuration register 1 for channel 3
 */
#define LEDC_CH3_CONF1_REG (DR_REG_LEDC_BASE + 0x48)
/** LEDC_DUTY_START_CH3 : R/W/SC; bitpos: [31]; default: 0;
 *  Configures whether the duty cycle fading configurations take effect.\\0: Not take
 *  effect\\1: Take effect
 */
#define LEDC_DUTY_START_CH3    (BIT(31))
#define LEDC_DUTY_START_CH3_M  (LEDC_DUTY_START_CH3_V << LEDC_DUTY_START_CH3_S)
#define LEDC_DUTY_START_CH3_V  0x00000001U
#define LEDC_DUTY_START_CH3_S  31

/** LEDC_CH3_DUTY_R_REG register
 *  Current duty cycle register for channel 3
 */
#define LEDC_CH3_DUTY_R_REG (DR_REG_LEDC_BASE + 0x4c)
/** LEDC_DUTY_CH3_R : RO; bitpos: [24:0]; default: 0;
 *  Represents the current duty of output signal on channel 3.
 */
#define LEDC_DUTY_CH3_R    0x01FFFFFFU
#define LEDC_DUTY_CH3_R_M  (LEDC_DUTY_CH3_R_V << LEDC_DUTY_CH3_R_S)
#define LEDC_DUTY_CH3_R_V  0x01FFFFFFU
#define LEDC_DUTY_CH3_R_S  0

/** LEDC_CH4_CONF0_REG register
 *  Configuration register 0 for channel 4
 */
#define LEDC_CH4_CONF0_REG (DR_REG_LEDC_BASE + 0x50)
/** LEDC_TIMER_SEL_CH4 : R/W; bitpos: [1:0]; default: 0;
 *  Configures which timer is channel 4 selected.\\0: Select timer0\\1: Select
 *  timer1\\2: Select timer2\\3: Select timer3
 */
#define LEDC_TIMER_SEL_CH4    0x00000003U
#define LEDC_TIMER_SEL_CH4_M  (LEDC_TIMER_SEL_CH4_V << LEDC_TIMER_SEL_CH4_S)
#define LEDC_TIMER_SEL_CH4_V  0x00000003U
#define LEDC_TIMER_SEL_CH4_S  0
/** LEDC_SIG_OUT_EN_CH4 : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable signal output on channel 4.\\0: Signal output
 *  disable\\1: Signal output enable
 */
#define LEDC_SIG_OUT_EN_CH4    (BIT(2))
#define LEDC_SIG_OUT_EN_CH4_M  (LEDC_SIG_OUT_EN_CH4_V << LEDC_SIG_OUT_EN_CH4_S)
#define LEDC_SIG_OUT_EN_CH4_V  0x00000001U
#define LEDC_SIG_OUT_EN_CH4_S  2
/** LEDC_IDLE_LV_CH4 : R/W; bitpos: [3]; default: 0;
 *  Configures the output value when channel 4 is inactive. Valid only when
 *  LEDC_SIG_OUT_EN_CH4 is 0.\\0: Output level is low\\1: Output level is high
 */
#define LEDC_IDLE_LV_CH4    (BIT(3))
#define LEDC_IDLE_LV_CH4_M  (LEDC_IDLE_LV_CH4_V << LEDC_IDLE_LV_CH4_S)
#define LEDC_IDLE_LV_CH4_V  0x00000001U
#define LEDC_IDLE_LV_CH4_S  3
/** LEDC_PARA_UP_CH4 : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to update LEDC_HPOINT_CH4, LEDC_DUTY_START_CH4,
 *  LEDC_SIG_OUT_EN_CH4, LEDC_TIMER_SEL_CH4, LEDC_OVF_CNT_EN_CH4 fields and duty cycle
 *  range configuration for channel 4, and will be automatically cleared by
 *  hardware.\\0: Invalid. No effect\\1: Update
 */
#define LEDC_PARA_UP_CH4    (BIT(4))
#define LEDC_PARA_UP_CH4_M  (LEDC_PARA_UP_CH4_V << LEDC_PARA_UP_CH4_S)
#define LEDC_PARA_UP_CH4_V  0x00000001U
#define LEDC_PARA_UP_CH4_S  4
/** LEDC_OVF_NUM_CH4 : R/W; bitpos: [14:5]; default: 0;
 *  Configures the maximum times of overflow minus 1.The LEDC_OVF_CNT_CH4_INT interrupt
 *  will be triggered when channel 4 overflows for (LEDC_OVF_NUM_CH4 + 1) times.
 */
#define LEDC_OVF_NUM_CH4    0x000003FFU
#define LEDC_OVF_NUM_CH4_M  (LEDC_OVF_NUM_CH4_V << LEDC_OVF_NUM_CH4_S)
#define LEDC_OVF_NUM_CH4_V  0x000003FFU
#define LEDC_OVF_NUM_CH4_S  5
/** LEDC_OVF_CNT_EN_CH4 : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable the ovf_cnt of channel 4.\\0: Disable\\1: Enable
 */
#define LEDC_OVF_CNT_EN_CH4    (BIT(15))
#define LEDC_OVF_CNT_EN_CH4_M  (LEDC_OVF_CNT_EN_CH4_V << LEDC_OVF_CNT_EN_CH4_S)
#define LEDC_OVF_CNT_EN_CH4_V  0x00000001U
#define LEDC_OVF_CNT_EN_CH4_S  15
/** LEDC_OVF_CNT_RESET_CH4 : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to reset the  ovf_cnt of channel 4.\\0: Invalid. No
 *  effect\\1: Reset the ovf_cnt
 */
#define LEDC_OVF_CNT_RESET_CH4    (BIT(16))
#define LEDC_OVF_CNT_RESET_CH4_M  (LEDC_OVF_CNT_RESET_CH4_V << LEDC_OVF_CNT_RESET_CH4_S)
#define LEDC_OVF_CNT_RESET_CH4_V  0x00000001U
#define LEDC_OVF_CNT_RESET_CH4_S  16

/** LEDC_CH4_HPOINT_REG register
 *  High point register for channel 4
 */
#define LEDC_CH4_HPOINT_REG (DR_REG_LEDC_BASE + 0x54)
/** LEDC_HPOINT_CH4 : R/W; bitpos: [19:0]; default: 0;
 *  Configures high point of signal output on channel 4. The output value changes to
 *  high when the selected timers has reached the value specified by this register.
 */
#define LEDC_HPOINT_CH4    0x000FFFFFU
#define LEDC_HPOINT_CH4_M  (LEDC_HPOINT_CH4_V << LEDC_HPOINT_CH4_S)
#define LEDC_HPOINT_CH4_V  0x000FFFFFU
#define LEDC_HPOINT_CH4_S  0

/** LEDC_CH4_DUTY_REG register
 *  Initial duty cycle register for channel 4
 */
#define LEDC_CH4_DUTY_REG (DR_REG_LEDC_BASE + 0x58)
/** LEDC_DUTY_CH4 : R/W; bitpos: [24:0]; default: 0;
 *  Configures the duty of signal output on channel 4.
 */
#define LEDC_DUTY_CH4    0x01FFFFFFU
#define LEDC_DUTY_CH4_M  (LEDC_DUTY_CH4_V << LEDC_DUTY_CH4_S)
#define LEDC_DUTY_CH4_V  0x01FFFFFFU
#define LEDC_DUTY_CH4_S  0

/** LEDC_CH4_CONF1_REG register
 *  Configuration register 1 for channel 4
 */
#define LEDC_CH4_CONF1_REG (DR_REG_LEDC_BASE + 0x5c)
/** LEDC_DUTY_START_CH4 : R/W/SC; bitpos: [31]; default: 0;
 *  Configures whether the duty cycle fading configurations take effect.\\0: Not take
 *  effect\\1: Take effect
 */
#define LEDC_DUTY_START_CH4    (BIT(31))
#define LEDC_DUTY_START_CH4_M  (LEDC_DUTY_START_CH4_V << LEDC_DUTY_START_CH4_S)
#define LEDC_DUTY_START_CH4_V  0x00000001U
#define LEDC_DUTY_START_CH4_S  31

/** LEDC_CH4_DUTY_R_REG register
 *  Current duty cycle register for channel 4
 */
#define LEDC_CH4_DUTY_R_REG (DR_REG_LEDC_BASE + 0x60)
/** LEDC_DUTY_CH4_R : RO; bitpos: [24:0]; default: 0;
 *  Represents the current duty of output signal on channel 4.
 */
#define LEDC_DUTY_CH4_R    0x01FFFFFFU
#define LEDC_DUTY_CH4_R_M  (LEDC_DUTY_CH4_R_V << LEDC_DUTY_CH4_R_S)
#define LEDC_DUTY_CH4_R_V  0x01FFFFFFU
#define LEDC_DUTY_CH4_R_S  0

/** LEDC_CH5_CONF0_REG register
 *  Configuration register 0 for channel 5
 */
#define LEDC_CH5_CONF0_REG (DR_REG_LEDC_BASE + 0x64)
/** LEDC_TIMER_SEL_CH5 : R/W; bitpos: [1:0]; default: 0;
 *  Configures which timer is channel 5 selected.\\0: Select timer0\\1: Select
 *  timer1\\2: Select timer2\\3: Select timer3
 */
#define LEDC_TIMER_SEL_CH5    0x00000003U
#define LEDC_TIMER_SEL_CH5_M  (LEDC_TIMER_SEL_CH5_V << LEDC_TIMER_SEL_CH5_S)
#define LEDC_TIMER_SEL_CH5_V  0x00000003U
#define LEDC_TIMER_SEL_CH5_S  0
/** LEDC_SIG_OUT_EN_CH5 : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable signal output on channel 5.\\0: Signal output
 *  disable\\1: Signal output enable
 */
#define LEDC_SIG_OUT_EN_CH5    (BIT(2))
#define LEDC_SIG_OUT_EN_CH5_M  (LEDC_SIG_OUT_EN_CH5_V << LEDC_SIG_OUT_EN_CH5_S)
#define LEDC_SIG_OUT_EN_CH5_V  0x00000001U
#define LEDC_SIG_OUT_EN_CH5_S  2
/** LEDC_IDLE_LV_CH5 : R/W; bitpos: [3]; default: 0;
 *  Configures the output value when channel 5 is inactive. Valid only when
 *  LEDC_SIG_OUT_EN_CH5 is 0.\\0: Output level is low\\1: Output level is high
 */
#define LEDC_IDLE_LV_CH5    (BIT(3))
#define LEDC_IDLE_LV_CH5_M  (LEDC_IDLE_LV_CH5_V << LEDC_IDLE_LV_CH5_S)
#define LEDC_IDLE_LV_CH5_V  0x00000001U
#define LEDC_IDLE_LV_CH5_S  3
/** LEDC_PARA_UP_CH5 : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to update LEDC_HPOINT_CH5, LEDC_DUTY_START_CH5,
 *  LEDC_SIG_OUT_EN_CH5, LEDC_TIMER_SEL_CH5, LEDC_OVF_CNT_EN_CH5 fields and duty cycle
 *  range configuration for channel 5, and will be automatically cleared by
 *  hardware.\\0: Invalid. No effect\\1: Update
 */
#define LEDC_PARA_UP_CH5    (BIT(4))
#define LEDC_PARA_UP_CH5_M  (LEDC_PARA_UP_CH5_V << LEDC_PARA_UP_CH5_S)
#define LEDC_PARA_UP_CH5_V  0x00000001U
#define LEDC_PARA_UP_CH5_S  4
/** LEDC_OVF_NUM_CH5 : R/W; bitpos: [14:5]; default: 0;
 *  Configures the maximum times of overflow minus 1.The LEDC_OVF_CNT_CH5_INT interrupt
 *  will be triggered when channel 5 overflows for (LEDC_OVF_NUM_CH5 + 1) times.
 */
#define LEDC_OVF_NUM_CH5    0x000003FFU
#define LEDC_OVF_NUM_CH5_M  (LEDC_OVF_NUM_CH5_V << LEDC_OVF_NUM_CH5_S)
#define LEDC_OVF_NUM_CH5_V  0x000003FFU
#define LEDC_OVF_NUM_CH5_S  5
/** LEDC_OVF_CNT_EN_CH5 : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable the ovf_cnt of channel 5.\\0: Disable\\1: Enable
 */
#define LEDC_OVF_CNT_EN_CH5    (BIT(15))
#define LEDC_OVF_CNT_EN_CH5_M  (LEDC_OVF_CNT_EN_CH5_V << LEDC_OVF_CNT_EN_CH5_S)
#define LEDC_OVF_CNT_EN_CH5_V  0x00000001U
#define LEDC_OVF_CNT_EN_CH5_S  15
/** LEDC_OVF_CNT_RESET_CH5 : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to reset the  ovf_cnt of channel 5.\\0: Invalid. No
 *  effect\\1: Reset the ovf_cnt
 */
#define LEDC_OVF_CNT_RESET_CH5    (BIT(16))
#define LEDC_OVF_CNT_RESET_CH5_M  (LEDC_OVF_CNT_RESET_CH5_V << LEDC_OVF_CNT_RESET_CH5_S)
#define LEDC_OVF_CNT_RESET_CH5_V  0x00000001U
#define LEDC_OVF_CNT_RESET_CH5_S  16

/** LEDC_CH5_HPOINT_REG register
 *  High point register for channel 5
 */
#define LEDC_CH5_HPOINT_REG (DR_REG_LEDC_BASE + 0x68)
/** LEDC_HPOINT_CH5 : R/W; bitpos: [19:0]; default: 0;
 *  Configures high point of signal output on channel 5. The output value changes to
 *  high when the selected timers has reached the value specified by this register.
 */
#define LEDC_HPOINT_CH5    0x000FFFFFU
#define LEDC_HPOINT_CH5_M  (LEDC_HPOINT_CH5_V << LEDC_HPOINT_CH5_S)
#define LEDC_HPOINT_CH5_V  0x000FFFFFU
#define LEDC_HPOINT_CH5_S  0

/** LEDC_CH5_DUTY_REG register
 *  Initial duty cycle register for channel 5
 */
#define LEDC_CH5_DUTY_REG (DR_REG_LEDC_BASE + 0x6c)
/** LEDC_DUTY_CH5 : R/W; bitpos: [24:0]; default: 0;
 *  Configures the duty of signal output on channel 5.
 */
#define LEDC_DUTY_CH5    0x01FFFFFFU
#define LEDC_DUTY_CH5_M  (LEDC_DUTY_CH5_V << LEDC_DUTY_CH5_S)
#define LEDC_DUTY_CH5_V  0x01FFFFFFU
#define LEDC_DUTY_CH5_S  0

/** LEDC_CH5_CONF1_REG register
 *  Configuration register 1 for channel 5
 */
#define LEDC_CH5_CONF1_REG (DR_REG_LEDC_BASE + 0x70)
/** LEDC_DUTY_START_CH5 : R/W/SC; bitpos: [31]; default: 0;
 *  Configures whether the duty cycle fading configurations take effect.\\0: Not take
 *  effect\\1: Take effect
 */
#define LEDC_DUTY_START_CH5    (BIT(31))
#define LEDC_DUTY_START_CH5_M  (LEDC_DUTY_START_CH5_V << LEDC_DUTY_START_CH5_S)
#define LEDC_DUTY_START_CH5_V  0x00000001U
#define LEDC_DUTY_START_CH5_S  31

/** LEDC_CH5_DUTY_R_REG register
 *  Current duty cycle register for channel 5
 */
#define LEDC_CH5_DUTY_R_REG (DR_REG_LEDC_BASE + 0x74)
/** LEDC_DUTY_CH5_R : RO; bitpos: [24:0]; default: 0;
 *  Represents the current duty of output signal on channel 5.
 */
#define LEDC_DUTY_CH5_R    0x01FFFFFFU
#define LEDC_DUTY_CH5_R_M  (LEDC_DUTY_CH5_R_V << LEDC_DUTY_CH5_R_S)
#define LEDC_DUTY_CH5_R_V  0x01FFFFFFU
#define LEDC_DUTY_CH5_R_S  0

/** LEDC_TIMER0_CONF_REG register
 *  Timer 0 configuration register
 */
#define LEDC_TIMER0_CONF_REG (DR_REG_LEDC_BASE + 0xa0)
/** LEDC_TIMER0_DUTY_RES : R/W; bitpos: [4:0]; default: 0;
 *  Configures the bit width of the counter in timer 0. Valid values are 1 to 20.
 */
#define LEDC_TIMER0_DUTY_RES    0x0000001FU
#define LEDC_TIMER0_DUTY_RES_M  (LEDC_TIMER0_DUTY_RES_V << LEDC_TIMER0_DUTY_RES_S)
#define LEDC_TIMER0_DUTY_RES_V  0x0000001FU
#define LEDC_TIMER0_DUTY_RES_S  0
/** LEDC_CLK_DIV_TIMER0 : R/W; bitpos: [22:5]; default: 0;
 *  Configures the divisor for the divider in timer 0.The least significant eight bits
 *  represent the fractional part.
 */
#define LEDC_CLK_DIV_TIMER0    0x0003FFFFU
#define LEDC_CLK_DIV_TIMER0_M  (LEDC_CLK_DIV_TIMER0_V << LEDC_CLK_DIV_TIMER0_S)
#define LEDC_CLK_DIV_TIMER0_V  0x0003FFFFU
#define LEDC_CLK_DIV_TIMER0_S  5
/** LEDC_TIMER0_PAUSE : R/W; bitpos: [23]; default: 0;
 *  Configures whether or not to pause the counter in timer 0.\\0: Normal\\1: Pause
 */
#define LEDC_TIMER0_PAUSE    (BIT(23))
#define LEDC_TIMER0_PAUSE_M  (LEDC_TIMER0_PAUSE_V << LEDC_TIMER0_PAUSE_S)
#define LEDC_TIMER0_PAUSE_V  0x00000001U
#define LEDC_TIMER0_PAUSE_S  23
/** LEDC_TIMER0_RST : R/W; bitpos: [24]; default: 1;
 *  Configures whether or not to reset timer 0. The counter will show 0 after
 *  reset.\\0: Not reset\\1: Reset
 */
#define LEDC_TIMER0_RST    (BIT(24))
#define LEDC_TIMER0_RST_M  (LEDC_TIMER0_RST_V << LEDC_TIMER0_RST_S)
#define LEDC_TIMER0_RST_V  0x00000001U
#define LEDC_TIMER0_RST_S  24
/** LEDC_TICK_SEL_TIMER0 : R/W; bitpos: [25]; default: 0;
 *  Configures which clock is timer 0 selected. Unused.
 */
#define LEDC_TICK_SEL_TIMER0    (BIT(25))
#define LEDC_TICK_SEL_TIMER0_M  (LEDC_TICK_SEL_TIMER0_V << LEDC_TICK_SEL_TIMER0_S)
#define LEDC_TICK_SEL_TIMER0_V  0x00000001U
#define LEDC_TICK_SEL_TIMER0_S  25
/** LEDC_TIMER0_PARA_UP : WT; bitpos: [26]; default: 0;
 *  Configures whether or not to update LEDC_CLK_DIV_TIMER0 and
 *  LEDC_TIMER0_DUTY_RES.\\0: Invalid. No effect\\1: Update
 */
#define LEDC_TIMER0_PARA_UP    (BIT(26))
#define LEDC_TIMER0_PARA_UP_M  (LEDC_TIMER0_PARA_UP_V << LEDC_TIMER0_PARA_UP_S)
#define LEDC_TIMER0_PARA_UP_V  0x00000001U
#define LEDC_TIMER0_PARA_UP_S  26

/** LEDC_TIMER0_VALUE_REG register
 *  Timer 0 current counter value register
 */
#define LEDC_TIMER0_VALUE_REG (DR_REG_LEDC_BASE + 0xa4)
/** LEDC_TIMER0_CNT : RO; bitpos: [19:0]; default: 0;
 *  Represents the current counter value of timer 0.
 */
#define LEDC_TIMER0_CNT    0x000FFFFFU
#define LEDC_TIMER0_CNT_M  (LEDC_TIMER0_CNT_V << LEDC_TIMER0_CNT_S)
#define LEDC_TIMER0_CNT_V  0x000FFFFFU
#define LEDC_TIMER0_CNT_S  0

/** LEDC_TIMER1_CONF_REG register
 *  Timer 1 configuration register
 */
#define LEDC_TIMER1_CONF_REG (DR_REG_LEDC_BASE + 0xa8)
/** LEDC_TIMER1_DUTY_RES : R/W; bitpos: [4:0]; default: 0;
 *  Configures the bit width of the counter in timer 1. Valid values are 1 to 20.
 */
#define LEDC_TIMER1_DUTY_RES    0x0000001FU
#define LEDC_TIMER1_DUTY_RES_M  (LEDC_TIMER1_DUTY_RES_V << LEDC_TIMER1_DUTY_RES_S)
#define LEDC_TIMER1_DUTY_RES_V  0x0000001FU
#define LEDC_TIMER1_DUTY_RES_S  0
/** LEDC_CLK_DIV_TIMER1 : R/W; bitpos: [22:5]; default: 0;
 *  Configures the divisor for the divider in timer 1.The least significant eight bits
 *  represent the fractional part.
 */
#define LEDC_CLK_DIV_TIMER1    0x0003FFFFU
#define LEDC_CLK_DIV_TIMER1_M  (LEDC_CLK_DIV_TIMER1_V << LEDC_CLK_DIV_TIMER1_S)
#define LEDC_CLK_DIV_TIMER1_V  0x0003FFFFU
#define LEDC_CLK_DIV_TIMER1_S  5
/** LEDC_TIMER1_PAUSE : R/W; bitpos: [23]; default: 0;
 *  Configures whether or not to pause the counter in timer 1.\\0: Normal\\1: Pause
 */
#define LEDC_TIMER1_PAUSE    (BIT(23))
#define LEDC_TIMER1_PAUSE_M  (LEDC_TIMER1_PAUSE_V << LEDC_TIMER1_PAUSE_S)
#define LEDC_TIMER1_PAUSE_V  0x00000001U
#define LEDC_TIMER1_PAUSE_S  23
/** LEDC_TIMER1_RST : R/W; bitpos: [24]; default: 1;
 *  Configures whether or not to reset timer 1. The counter will show 0 after
 *  reset.\\0: Not reset\\1: Reset
 */
#define LEDC_TIMER1_RST    (BIT(24))
#define LEDC_TIMER1_RST_M  (LEDC_TIMER1_RST_V << LEDC_TIMER1_RST_S)
#define LEDC_TIMER1_RST_V  0x00000001U
#define LEDC_TIMER1_RST_S  24
/** LEDC_TICK_SEL_TIMER1 : R/W; bitpos: [25]; default: 0;
 *  Configures which clock is timer 1 selected. Unused.
 */
#define LEDC_TICK_SEL_TIMER1    (BIT(25))
#define LEDC_TICK_SEL_TIMER1_M  (LEDC_TICK_SEL_TIMER1_V << LEDC_TICK_SEL_TIMER1_S)
#define LEDC_TICK_SEL_TIMER1_V  0x00000001U
#define LEDC_TICK_SEL_TIMER1_S  25
/** LEDC_TIMER1_PARA_UP : WT; bitpos: [26]; default: 0;
 *  Configures whether or not to update LEDC_CLK_DIV_TIMER1 and
 *  LEDC_TIMER1_DUTY_RES.\\0: Invalid. No effect\\1: Update
 */
#define LEDC_TIMER1_PARA_UP    (BIT(26))
#define LEDC_TIMER1_PARA_UP_M  (LEDC_TIMER1_PARA_UP_V << LEDC_TIMER1_PARA_UP_S)
#define LEDC_TIMER1_PARA_UP_V  0x00000001U
#define LEDC_TIMER1_PARA_UP_S  26

/** LEDC_TIMER1_VALUE_REG register
 *  Timer 1 current counter value register
 */
#define LEDC_TIMER1_VALUE_REG (DR_REG_LEDC_BASE + 0xac)
/** LEDC_TIMER1_CNT : RO; bitpos: [19:0]; default: 0;
 *  Represents the current counter value of timer 1.
 */
#define LEDC_TIMER1_CNT    0x000FFFFFU
#define LEDC_TIMER1_CNT_M  (LEDC_TIMER1_CNT_V << LEDC_TIMER1_CNT_S)
#define LEDC_TIMER1_CNT_V  0x000FFFFFU
#define LEDC_TIMER1_CNT_S  0

/** LEDC_TIMER2_CONF_REG register
 *  Timer 2 configuration register
 */
#define LEDC_TIMER2_CONF_REG (DR_REG_LEDC_BASE + 0xb0)
/** LEDC_TIMER2_DUTY_RES : R/W; bitpos: [4:0]; default: 0;
 *  Configures the bit width of the counter in timer 2. Valid values are 1 to 20.
 */
#define LEDC_TIMER2_DUTY_RES    0x0000001FU
#define LEDC_TIMER2_DUTY_RES_M  (LEDC_TIMER2_DUTY_RES_V << LEDC_TIMER2_DUTY_RES_S)
#define LEDC_TIMER2_DUTY_RES_V  0x0000001FU
#define LEDC_TIMER2_DUTY_RES_S  0
/** LEDC_CLK_DIV_TIMER2 : R/W; bitpos: [22:5]; default: 0;
 *  Configures the divisor for the divider in timer 2.The least significant eight bits
 *  represent the fractional part.
 */
#define LEDC_CLK_DIV_TIMER2    0x0003FFFFU
#define LEDC_CLK_DIV_TIMER2_M  (LEDC_CLK_DIV_TIMER2_V << LEDC_CLK_DIV_TIMER2_S)
#define LEDC_CLK_DIV_TIMER2_V  0x0003FFFFU
#define LEDC_CLK_DIV_TIMER2_S  5
/** LEDC_TIMER2_PAUSE : R/W; bitpos: [23]; default: 0;
 *  Configures whether or not to pause the counter in timer 2.\\0: Normal\\1: Pause
 */
#define LEDC_TIMER2_PAUSE    (BIT(23))
#define LEDC_TIMER2_PAUSE_M  (LEDC_TIMER2_PAUSE_V << LEDC_TIMER2_PAUSE_S)
#define LEDC_TIMER2_PAUSE_V  0x00000001U
#define LEDC_TIMER2_PAUSE_S  23
/** LEDC_TIMER2_RST : R/W; bitpos: [24]; default: 1;
 *  Configures whether or not to reset timer 2. The counter will show 0 after
 *  reset.\\0: Not reset\\1: Reset
 */
#define LEDC_TIMER2_RST    (BIT(24))
#define LEDC_TIMER2_RST_M  (LEDC_TIMER2_RST_V << LEDC_TIMER2_RST_S)
#define LEDC_TIMER2_RST_V  0x00000001U
#define LEDC_TIMER2_RST_S  24
/** LEDC_TICK_SEL_TIMER2 : R/W; bitpos: [25]; default: 0;
 *  Configures which clock is timer 2 selected. Unused.
 */
#define LEDC_TICK_SEL_TIMER2    (BIT(25))
#define LEDC_TICK_SEL_TIMER2_M  (LEDC_TICK_SEL_TIMER2_V << LEDC_TICK_SEL_TIMER2_S)
#define LEDC_TICK_SEL_TIMER2_V  0x00000001U
#define LEDC_TICK_SEL_TIMER2_S  25
/** LEDC_TIMER2_PARA_UP : WT; bitpos: [26]; default: 0;
 *  Configures whether or not to update LEDC_CLK_DIV_TIMER2 and
 *  LEDC_TIMER2_DUTY_RES.\\0: Invalid. No effect\\1: Update
 */
#define LEDC_TIMER2_PARA_UP    (BIT(26))
#define LEDC_TIMER2_PARA_UP_M  (LEDC_TIMER2_PARA_UP_V << LEDC_TIMER2_PARA_UP_S)
#define LEDC_TIMER2_PARA_UP_V  0x00000001U
#define LEDC_TIMER2_PARA_UP_S  26

/** LEDC_TIMER2_VALUE_REG register
 *  Timer 2 current counter value register
 */
#define LEDC_TIMER2_VALUE_REG (DR_REG_LEDC_BASE + 0xb4)
/** LEDC_TIMER2_CNT : RO; bitpos: [19:0]; default: 0;
 *  Represents the current counter value of timer 2.
 */
#define LEDC_TIMER2_CNT    0x000FFFFFU
#define LEDC_TIMER2_CNT_M  (LEDC_TIMER2_CNT_V << LEDC_TIMER2_CNT_S)
#define LEDC_TIMER2_CNT_V  0x000FFFFFU
#define LEDC_TIMER2_CNT_S  0

/** LEDC_TIMER3_CONF_REG register
 *  Timer 3 configuration register
 */
#define LEDC_TIMER3_CONF_REG (DR_REG_LEDC_BASE + 0xb8)
/** LEDC_TIMER3_DUTY_RES : R/W; bitpos: [4:0]; default: 0;
 *  Configures the bit width of the counter in timer 3. Valid values are 1 to 20.
 */
#define LEDC_TIMER3_DUTY_RES    0x0000001FU
#define LEDC_TIMER3_DUTY_RES_M  (LEDC_TIMER3_DUTY_RES_V << LEDC_TIMER3_DUTY_RES_S)
#define LEDC_TIMER3_DUTY_RES_V  0x0000001FU
#define LEDC_TIMER3_DUTY_RES_S  0
/** LEDC_CLK_DIV_TIMER3 : R/W; bitpos: [22:5]; default: 0;
 *  Configures the divisor for the divider in timer 3.The least significant eight bits
 *  represent the fractional part.
 */
#define LEDC_CLK_DIV_TIMER3    0x0003FFFFU
#define LEDC_CLK_DIV_TIMER3_M  (LEDC_CLK_DIV_TIMER3_V << LEDC_CLK_DIV_TIMER3_S)
#define LEDC_CLK_DIV_TIMER3_V  0x0003FFFFU
#define LEDC_CLK_DIV_TIMER3_S  5
/** LEDC_TIMER3_PAUSE : R/W; bitpos: [23]; default: 0;
 *  Configures whether or not to pause the counter in timer 3.\\0: Normal\\1: Pause
 */
#define LEDC_TIMER3_PAUSE    (BIT(23))
#define LEDC_TIMER3_PAUSE_M  (LEDC_TIMER3_PAUSE_V << LEDC_TIMER3_PAUSE_S)
#define LEDC_TIMER3_PAUSE_V  0x00000001U
#define LEDC_TIMER3_PAUSE_S  23
/** LEDC_TIMER3_RST : R/W; bitpos: [24]; default: 1;
 *  Configures whether or not to reset timer 3. The counter will show 0 after
 *  reset.\\0: Not reset\\1: Reset
 */
#define LEDC_TIMER3_RST    (BIT(24))
#define LEDC_TIMER3_RST_M  (LEDC_TIMER3_RST_V << LEDC_TIMER3_RST_S)
#define LEDC_TIMER3_RST_V  0x00000001U
#define LEDC_TIMER3_RST_S  24
/** LEDC_TICK_SEL_TIMER3 : R/W; bitpos: [25]; default: 0;
 *  Configures which clock is timer 3 selected. Unused.
 */
#define LEDC_TICK_SEL_TIMER3    (BIT(25))
#define LEDC_TICK_SEL_TIMER3_M  (LEDC_TICK_SEL_TIMER3_V << LEDC_TICK_SEL_TIMER3_S)
#define LEDC_TICK_SEL_TIMER3_V  0x00000001U
#define LEDC_TICK_SEL_TIMER3_S  25
/** LEDC_TIMER3_PARA_UP : WT; bitpos: [26]; default: 0;
 *  Configures whether or not to update LEDC_CLK_DIV_TIMER3 and
 *  LEDC_TIMER3_DUTY_RES.\\0: Invalid. No effect\\1: Update
 */
#define LEDC_TIMER3_PARA_UP    (BIT(26))
#define LEDC_TIMER3_PARA_UP_M  (LEDC_TIMER3_PARA_UP_V << LEDC_TIMER3_PARA_UP_S)
#define LEDC_TIMER3_PARA_UP_V  0x00000001U
#define LEDC_TIMER3_PARA_UP_S  26

/** LEDC_TIMER3_VALUE_REG register
 *  Timer 3 current counter value register
 */
#define LEDC_TIMER3_VALUE_REG (DR_REG_LEDC_BASE + 0xbc)
/** LEDC_TIMER3_CNT : RO; bitpos: [19:0]; default: 0;
 *  Represents the current counter value of timer 3.
 */
#define LEDC_TIMER3_CNT    0x000FFFFFU
#define LEDC_TIMER3_CNT_M  (LEDC_TIMER3_CNT_V << LEDC_TIMER3_CNT_S)
#define LEDC_TIMER3_CNT_V  0x000FFFFFU
#define LEDC_TIMER3_CNT_S  0

/** LEDC_INT_RAW_REG register
 *  Interrupt raw status register
 */
#define LEDC_INT_RAW_REG (DR_REG_LEDC_BASE + 0xc0)
/** LEDC_TIMER0_OVF_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  Raw status bit: The raw interrupt status of LEDC_TIMER0_OVF_INT. Triggered when the
 *  timer0 has reached its maximum counter value.
 */
#define LEDC_TIMER0_OVF_INT_RAW    (BIT(0))
#define LEDC_TIMER0_OVF_INT_RAW_M  (LEDC_TIMER0_OVF_INT_RAW_V << LEDC_TIMER0_OVF_INT_RAW_S)
#define LEDC_TIMER0_OVF_INT_RAW_V  0x00000001U
#define LEDC_TIMER0_OVF_INT_RAW_S  0
/** LEDC_TIMER1_OVF_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  Raw status bit: The raw interrupt status of LEDC_TIMER1_OVF_INT. Triggered when the
 *  timer1 has reached its maximum counter value.
 */
#define LEDC_TIMER1_OVF_INT_RAW    (BIT(1))
#define LEDC_TIMER1_OVF_INT_RAW_M  (LEDC_TIMER1_OVF_INT_RAW_V << LEDC_TIMER1_OVF_INT_RAW_S)
#define LEDC_TIMER1_OVF_INT_RAW_V  0x00000001U
#define LEDC_TIMER1_OVF_INT_RAW_S  1
/** LEDC_TIMER2_OVF_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  Raw status bit: The raw interrupt status of LEDC_TIMER2_OVF_INT. Triggered when the
 *  timer2 has reached its maximum counter value.
 */
#define LEDC_TIMER2_OVF_INT_RAW    (BIT(2))
#define LEDC_TIMER2_OVF_INT_RAW_M  (LEDC_TIMER2_OVF_INT_RAW_V << LEDC_TIMER2_OVF_INT_RAW_S)
#define LEDC_TIMER2_OVF_INT_RAW_V  0x00000001U
#define LEDC_TIMER2_OVF_INT_RAW_S  2
/** LEDC_TIMER3_OVF_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  Raw status bit: The raw interrupt status of LEDC_TIMER3_OVF_INT. Triggered when the
 *  timer3 has reached its maximum counter value.
 */
#define LEDC_TIMER3_OVF_INT_RAW    (BIT(3))
#define LEDC_TIMER3_OVF_INT_RAW_M  (LEDC_TIMER3_OVF_INT_RAW_V << LEDC_TIMER3_OVF_INT_RAW_S)
#define LEDC_TIMER3_OVF_INT_RAW_V  0x00000001U
#define LEDC_TIMER3_OVF_INT_RAW_S  3
/** LEDC_DUTY_CHNG_END_CH0_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  Raw status bit: The raw interrupt status of LEDC_DUTY_CHNG_END_CH0_INT. Triggered
 *  when the fading of duty has finished.
 */
#define LEDC_DUTY_CHNG_END_CH0_INT_RAW    (BIT(4))
#define LEDC_DUTY_CHNG_END_CH0_INT_RAW_M  (LEDC_DUTY_CHNG_END_CH0_INT_RAW_V << LEDC_DUTY_CHNG_END_CH0_INT_RAW_S)
#define LEDC_DUTY_CHNG_END_CH0_INT_RAW_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH0_INT_RAW_S  4
/** LEDC_DUTY_CHNG_END_CH1_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  Raw status bit: The raw interrupt status of LEDC_DUTY_CHNG_END_CH1_INT. Triggered
 *  when the fading of duty has finished.
 */
#define LEDC_DUTY_CHNG_END_CH1_INT_RAW    (BIT(5))
#define LEDC_DUTY_CHNG_END_CH1_INT_RAW_M  (LEDC_DUTY_CHNG_END_CH1_INT_RAW_V << LEDC_DUTY_CHNG_END_CH1_INT_RAW_S)
#define LEDC_DUTY_CHNG_END_CH1_INT_RAW_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH1_INT_RAW_S  5
/** LEDC_DUTY_CHNG_END_CH2_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  Raw status bit: The raw interrupt status of LEDC_DUTY_CHNG_END_CH2_INT. Triggered
 *  when the fading of duty has finished.
 */
#define LEDC_DUTY_CHNG_END_CH2_INT_RAW    (BIT(6))
#define LEDC_DUTY_CHNG_END_CH2_INT_RAW_M  (LEDC_DUTY_CHNG_END_CH2_INT_RAW_V << LEDC_DUTY_CHNG_END_CH2_INT_RAW_S)
#define LEDC_DUTY_CHNG_END_CH2_INT_RAW_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH2_INT_RAW_S  6
/** LEDC_DUTY_CHNG_END_CH3_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  Raw status bit: The raw interrupt status of LEDC_DUTY_CHNG_END_CH3_INT. Triggered
 *  when the fading of duty has finished.
 */
#define LEDC_DUTY_CHNG_END_CH3_INT_RAW    (BIT(7))
#define LEDC_DUTY_CHNG_END_CH3_INT_RAW_M  (LEDC_DUTY_CHNG_END_CH3_INT_RAW_V << LEDC_DUTY_CHNG_END_CH3_INT_RAW_S)
#define LEDC_DUTY_CHNG_END_CH3_INT_RAW_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH3_INT_RAW_S  7
/** LEDC_DUTY_CHNG_END_CH4_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  Raw status bit: The raw interrupt status of LEDC_DUTY_CHNG_END_CH4_INT. Triggered
 *  when the fading of duty has finished.
 */
#define LEDC_DUTY_CHNG_END_CH4_INT_RAW    (BIT(8))
#define LEDC_DUTY_CHNG_END_CH4_INT_RAW_M  (LEDC_DUTY_CHNG_END_CH4_INT_RAW_V << LEDC_DUTY_CHNG_END_CH4_INT_RAW_S)
#define LEDC_DUTY_CHNG_END_CH4_INT_RAW_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH4_INT_RAW_S  8
/** LEDC_DUTY_CHNG_END_CH5_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  Raw status bit: The raw interrupt status of LEDC_DUTY_CHNG_END_CH5_INT. Triggered
 *  when the fading of duty has finished.
 */
#define LEDC_DUTY_CHNG_END_CH5_INT_RAW    (BIT(9))
#define LEDC_DUTY_CHNG_END_CH5_INT_RAW_M  (LEDC_DUTY_CHNG_END_CH5_INT_RAW_V << LEDC_DUTY_CHNG_END_CH5_INT_RAW_S)
#define LEDC_DUTY_CHNG_END_CH5_INT_RAW_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH5_INT_RAW_S  9
/** LEDC_OVF_CNT_CH0_INT_RAW : R/WTC/SS; bitpos: [12]; default: 0;
 *  Raw status bit: The raw interrupt status of LEDC_OVF_CNT_CH0_INT. Triggered when
 *  the ovf_cnt has reached the value specified by LEDC_OVF_NUM_CH0.
 */
#define LEDC_OVF_CNT_CH0_INT_RAW    (BIT(12))
#define LEDC_OVF_CNT_CH0_INT_RAW_M  (LEDC_OVF_CNT_CH0_INT_RAW_V << LEDC_OVF_CNT_CH0_INT_RAW_S)
#define LEDC_OVF_CNT_CH0_INT_RAW_V  0x00000001U
#define LEDC_OVF_CNT_CH0_INT_RAW_S  12
/** LEDC_OVF_CNT_CH1_INT_RAW : R/WTC/SS; bitpos: [13]; default: 0;
 *  Raw status bit: The raw interrupt status of LEDC_OVF_CNT_CH1_INT. Triggered when
 *  the ovf_cnt has reached the value specified by LEDC_OVF_NUM_CH1.
 */
#define LEDC_OVF_CNT_CH1_INT_RAW    (BIT(13))
#define LEDC_OVF_CNT_CH1_INT_RAW_M  (LEDC_OVF_CNT_CH1_INT_RAW_V << LEDC_OVF_CNT_CH1_INT_RAW_S)
#define LEDC_OVF_CNT_CH1_INT_RAW_V  0x00000001U
#define LEDC_OVF_CNT_CH1_INT_RAW_S  13
/** LEDC_OVF_CNT_CH2_INT_RAW : R/WTC/SS; bitpos: [14]; default: 0;
 *  Raw status bit: The raw interrupt status of LEDC_OVF_CNT_CH2_INT. Triggered when
 *  the ovf_cnt has reached the value specified by LEDC_OVF_NUM_CH2.
 */
#define LEDC_OVF_CNT_CH2_INT_RAW    (BIT(14))
#define LEDC_OVF_CNT_CH2_INT_RAW_M  (LEDC_OVF_CNT_CH2_INT_RAW_V << LEDC_OVF_CNT_CH2_INT_RAW_S)
#define LEDC_OVF_CNT_CH2_INT_RAW_V  0x00000001U
#define LEDC_OVF_CNT_CH2_INT_RAW_S  14
/** LEDC_OVF_CNT_CH3_INT_RAW : R/WTC/SS; bitpos: [15]; default: 0;
 *  Raw status bit: The raw interrupt status of LEDC_OVF_CNT_CH3_INT. Triggered when
 *  the ovf_cnt has reached the value specified by LEDC_OVF_NUM_CH3.
 */
#define LEDC_OVF_CNT_CH3_INT_RAW    (BIT(15))
#define LEDC_OVF_CNT_CH3_INT_RAW_M  (LEDC_OVF_CNT_CH3_INT_RAW_V << LEDC_OVF_CNT_CH3_INT_RAW_S)
#define LEDC_OVF_CNT_CH3_INT_RAW_V  0x00000001U
#define LEDC_OVF_CNT_CH3_INT_RAW_S  15
/** LEDC_OVF_CNT_CH4_INT_RAW : R/WTC/SS; bitpos: [16]; default: 0;
 *  Raw status bit: The raw interrupt status of LEDC_OVF_CNT_CH4_INT. Triggered when
 *  the ovf_cnt has reached the value specified by LEDC_OVF_NUM_CH4.
 */
#define LEDC_OVF_CNT_CH4_INT_RAW    (BIT(16))
#define LEDC_OVF_CNT_CH4_INT_RAW_M  (LEDC_OVF_CNT_CH4_INT_RAW_V << LEDC_OVF_CNT_CH4_INT_RAW_S)
#define LEDC_OVF_CNT_CH4_INT_RAW_V  0x00000001U
#define LEDC_OVF_CNT_CH4_INT_RAW_S  16
/** LEDC_OVF_CNT_CH5_INT_RAW : R/WTC/SS; bitpos: [17]; default: 0;
 *  Raw status bit: The raw interrupt status of LEDC_OVF_CNT_CH5_INT. Triggered when
 *  the ovf_cnt has reached the value specified by LEDC_OVF_NUM_CH5.
 */
#define LEDC_OVF_CNT_CH5_INT_RAW    (BIT(17))
#define LEDC_OVF_CNT_CH5_INT_RAW_M  (LEDC_OVF_CNT_CH5_INT_RAW_V << LEDC_OVF_CNT_CH5_INT_RAW_S)
#define LEDC_OVF_CNT_CH5_INT_RAW_V  0x00000001U
#define LEDC_OVF_CNT_CH5_INT_RAW_S  17

/** LEDC_INT_ST_REG register
 *  Interrupt masked status register
 */
#define LEDC_INT_ST_REG (DR_REG_LEDC_BASE + 0xc4)
/** LEDC_TIMER0_OVF_INT_ST : RO; bitpos: [0]; default: 0;
 *  Masked status bit: The masked interrupt status of LEDC_TIMER0_OVF_INT. Valid only
 *  when LEDC_TIMER0_OVF_INT_ENA is set to 1.
 */
#define LEDC_TIMER0_OVF_INT_ST    (BIT(0))
#define LEDC_TIMER0_OVF_INT_ST_M  (LEDC_TIMER0_OVF_INT_ST_V << LEDC_TIMER0_OVF_INT_ST_S)
#define LEDC_TIMER0_OVF_INT_ST_V  0x00000001U
#define LEDC_TIMER0_OVF_INT_ST_S  0
/** LEDC_TIMER1_OVF_INT_ST : RO; bitpos: [1]; default: 0;
 *  Masked status bit: The masked interrupt status of LEDC_TIMER1_OVF_INT. Valid only
 *  when LEDC_TIMER1_OVF_INT_ENA is set to 1.
 */
#define LEDC_TIMER1_OVF_INT_ST    (BIT(1))
#define LEDC_TIMER1_OVF_INT_ST_M  (LEDC_TIMER1_OVF_INT_ST_V << LEDC_TIMER1_OVF_INT_ST_S)
#define LEDC_TIMER1_OVF_INT_ST_V  0x00000001U
#define LEDC_TIMER1_OVF_INT_ST_S  1
/** LEDC_TIMER2_OVF_INT_ST : RO; bitpos: [2]; default: 0;
 *  Masked status bit: The masked interrupt status of LEDC_TIMER2_OVF_INT. Valid only
 *  when LEDC_TIMER2_OVF_INT_ENA is set to 1.
 */
#define LEDC_TIMER2_OVF_INT_ST    (BIT(2))
#define LEDC_TIMER2_OVF_INT_ST_M  (LEDC_TIMER2_OVF_INT_ST_V << LEDC_TIMER2_OVF_INT_ST_S)
#define LEDC_TIMER2_OVF_INT_ST_V  0x00000001U
#define LEDC_TIMER2_OVF_INT_ST_S  2
/** LEDC_TIMER3_OVF_INT_ST : RO; bitpos: [3]; default: 0;
 *  Masked status bit: The masked interrupt status of LEDC_TIMER3_OVF_INT. Valid only
 *  when LEDC_TIMER3_OVF_INT_ENA is set to 1.
 */
#define LEDC_TIMER3_OVF_INT_ST    (BIT(3))
#define LEDC_TIMER3_OVF_INT_ST_M  (LEDC_TIMER3_OVF_INT_ST_V << LEDC_TIMER3_OVF_INT_ST_S)
#define LEDC_TIMER3_OVF_INT_ST_V  0x00000001U
#define LEDC_TIMER3_OVF_INT_ST_S  3
/** LEDC_DUTY_CHNG_END_CH0_INT_ST : RO; bitpos: [4]; default: 0;
 *  Masked status bit: The masked interrupt status of LEDC_DUTY_CHNG_END_CH0_INT. Valid
 *  only when LEDC_DUTY_CHNG_END_CH0_INT_ENA is set to 1.
 */
#define LEDC_DUTY_CHNG_END_CH0_INT_ST    (BIT(4))
#define LEDC_DUTY_CHNG_END_CH0_INT_ST_M  (LEDC_DUTY_CHNG_END_CH0_INT_ST_V << LEDC_DUTY_CHNG_END_CH0_INT_ST_S)
#define LEDC_DUTY_CHNG_END_CH0_INT_ST_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH0_INT_ST_S  4
/** LEDC_DUTY_CHNG_END_CH1_INT_ST : RO; bitpos: [5]; default: 0;
 *  Masked status bit: The masked interrupt status of LEDC_DUTY_CHNG_END_CH1_INT. Valid
 *  only when LEDC_DUTY_CHNG_END_CH1_INT_ENA is set to 1.
 */
#define LEDC_DUTY_CHNG_END_CH1_INT_ST    (BIT(5))
#define LEDC_DUTY_CHNG_END_CH1_INT_ST_M  (LEDC_DUTY_CHNG_END_CH1_INT_ST_V << LEDC_DUTY_CHNG_END_CH1_INT_ST_S)
#define LEDC_DUTY_CHNG_END_CH1_INT_ST_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH1_INT_ST_S  5
/** LEDC_DUTY_CHNG_END_CH2_INT_ST : RO; bitpos: [6]; default: 0;
 *  Masked status bit: The masked interrupt status of LEDC_DUTY_CHNG_END_CH2_INT. Valid
 *  only when LEDC_DUTY_CHNG_END_CH2_INT_ENA is set to 1.
 */
#define LEDC_DUTY_CHNG_END_CH2_INT_ST    (BIT(6))
#define LEDC_DUTY_CHNG_END_CH2_INT_ST_M  (LEDC_DUTY_CHNG_END_CH2_INT_ST_V << LEDC_DUTY_CHNG_END_CH2_INT_ST_S)
#define LEDC_DUTY_CHNG_END_CH2_INT_ST_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH2_INT_ST_S  6
/** LEDC_DUTY_CHNG_END_CH3_INT_ST : RO; bitpos: [7]; default: 0;
 *  Masked status bit: The masked interrupt status of LEDC_DUTY_CHNG_END_CH3_INT. Valid
 *  only when LEDC_DUTY_CHNG_END_CH3_INT_ENA is set to 1.
 */
#define LEDC_DUTY_CHNG_END_CH3_INT_ST    (BIT(7))
#define LEDC_DUTY_CHNG_END_CH3_INT_ST_M  (LEDC_DUTY_CHNG_END_CH3_INT_ST_V << LEDC_DUTY_CHNG_END_CH3_INT_ST_S)
#define LEDC_DUTY_CHNG_END_CH3_INT_ST_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH3_INT_ST_S  7
/** LEDC_DUTY_CHNG_END_CH4_INT_ST : RO; bitpos: [8]; default: 0;
 *  Masked status bit: The masked interrupt status of LEDC_DUTY_CHNG_END_CH4_INT. Valid
 *  only when LEDC_DUTY_CHNG_END_CH4_INT_ENA is set to 1.
 */
#define LEDC_DUTY_CHNG_END_CH4_INT_ST    (BIT(8))
#define LEDC_DUTY_CHNG_END_CH4_INT_ST_M  (LEDC_DUTY_CHNG_END_CH4_INT_ST_V << LEDC_DUTY_CHNG_END_CH4_INT_ST_S)
#define LEDC_DUTY_CHNG_END_CH4_INT_ST_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH4_INT_ST_S  8
/** LEDC_DUTY_CHNG_END_CH5_INT_ST : RO; bitpos: [9]; default: 0;
 *  Masked status bit: The masked interrupt status of LEDC_DUTY_CHNG_END_CH5_INT. Valid
 *  only when LEDC_DUTY_CHNG_END_CH5_INT_ENA is set to 1.
 */
#define LEDC_DUTY_CHNG_END_CH5_INT_ST    (BIT(9))
#define LEDC_DUTY_CHNG_END_CH5_INT_ST_M  (LEDC_DUTY_CHNG_END_CH5_INT_ST_V << LEDC_DUTY_CHNG_END_CH5_INT_ST_S)
#define LEDC_DUTY_CHNG_END_CH5_INT_ST_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH5_INT_ST_S  9
/** LEDC_OVF_CNT_CH0_INT_ST : RO; bitpos: [12]; default: 0;
 *  Masked status bit: The masked interrupt status of LEDC_OVF_CNT_CH0_INT. Valid only
 *  when LEDC_OVF_CNT_CH0_INT_ENA is set to 1.
 */
#define LEDC_OVF_CNT_CH0_INT_ST    (BIT(12))
#define LEDC_OVF_CNT_CH0_INT_ST_M  (LEDC_OVF_CNT_CH0_INT_ST_V << LEDC_OVF_CNT_CH0_INT_ST_S)
#define LEDC_OVF_CNT_CH0_INT_ST_V  0x00000001U
#define LEDC_OVF_CNT_CH0_INT_ST_S  12
/** LEDC_OVF_CNT_CH1_INT_ST : RO; bitpos: [13]; default: 0;
 *  Masked status bit: The masked interrupt status of LEDC_OVF_CNT_CH1_INT. Valid only
 *  when LEDC_OVF_CNT_CH1_INT_ENA is set to 1.
 */
#define LEDC_OVF_CNT_CH1_INT_ST    (BIT(13))
#define LEDC_OVF_CNT_CH1_INT_ST_M  (LEDC_OVF_CNT_CH1_INT_ST_V << LEDC_OVF_CNT_CH1_INT_ST_S)
#define LEDC_OVF_CNT_CH1_INT_ST_V  0x00000001U
#define LEDC_OVF_CNT_CH1_INT_ST_S  13
/** LEDC_OVF_CNT_CH2_INT_ST : RO; bitpos: [14]; default: 0;
 *  Masked status bit: The masked interrupt status of LEDC_OVF_CNT_CH2_INT. Valid only
 *  when LEDC_OVF_CNT_CH2_INT_ENA is set to 1.
 */
#define LEDC_OVF_CNT_CH2_INT_ST    (BIT(14))
#define LEDC_OVF_CNT_CH2_INT_ST_M  (LEDC_OVF_CNT_CH2_INT_ST_V << LEDC_OVF_CNT_CH2_INT_ST_S)
#define LEDC_OVF_CNT_CH2_INT_ST_V  0x00000001U
#define LEDC_OVF_CNT_CH2_INT_ST_S  14
/** LEDC_OVF_CNT_CH3_INT_ST : RO; bitpos: [15]; default: 0;
 *  Masked status bit: The masked interrupt status of LEDC_OVF_CNT_CH3_INT. Valid only
 *  when LEDC_OVF_CNT_CH3_INT_ENA is set to 1.
 */
#define LEDC_OVF_CNT_CH3_INT_ST    (BIT(15))
#define LEDC_OVF_CNT_CH3_INT_ST_M  (LEDC_OVF_CNT_CH3_INT_ST_V << LEDC_OVF_CNT_CH3_INT_ST_S)
#define LEDC_OVF_CNT_CH3_INT_ST_V  0x00000001U
#define LEDC_OVF_CNT_CH3_INT_ST_S  15
/** LEDC_OVF_CNT_CH4_INT_ST : RO; bitpos: [16]; default: 0;
 *  Masked status bit: The masked interrupt status of LEDC_OVF_CNT_CH4_INT. Valid only
 *  when LEDC_OVF_CNT_CH4_INT_ENA is set to 1.
 */
#define LEDC_OVF_CNT_CH4_INT_ST    (BIT(16))
#define LEDC_OVF_CNT_CH4_INT_ST_M  (LEDC_OVF_CNT_CH4_INT_ST_V << LEDC_OVF_CNT_CH4_INT_ST_S)
#define LEDC_OVF_CNT_CH4_INT_ST_V  0x00000001U
#define LEDC_OVF_CNT_CH4_INT_ST_S  16
/** LEDC_OVF_CNT_CH5_INT_ST : RO; bitpos: [17]; default: 0;
 *  Masked status bit: The masked interrupt status of LEDC_OVF_CNT_CH5_INT. Valid only
 *  when LEDC_OVF_CNT_CH5_INT_ENA is set to 1.
 */
#define LEDC_OVF_CNT_CH5_INT_ST    (BIT(17))
#define LEDC_OVF_CNT_CH5_INT_ST_M  (LEDC_OVF_CNT_CH5_INT_ST_V << LEDC_OVF_CNT_CH5_INT_ST_S)
#define LEDC_OVF_CNT_CH5_INT_ST_V  0x00000001U
#define LEDC_OVF_CNT_CH5_INT_ST_S  17

/** LEDC_INT_ENA_REG register
 *  Interrupt enable register
 */
#define LEDC_INT_ENA_REG (DR_REG_LEDC_BASE + 0xc8)
/** LEDC_TIMER0_OVF_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  Enable bit: Write 1 to enable LEDC_TIMER0_OVF_INT.
 */
#define LEDC_TIMER0_OVF_INT_ENA    (BIT(0))
#define LEDC_TIMER0_OVF_INT_ENA_M  (LEDC_TIMER0_OVF_INT_ENA_V << LEDC_TIMER0_OVF_INT_ENA_S)
#define LEDC_TIMER0_OVF_INT_ENA_V  0x00000001U
#define LEDC_TIMER0_OVF_INT_ENA_S  0
/** LEDC_TIMER1_OVF_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  Enable bit: Write 1 to enable LEDC_TIMER1_OVF_INT.
 */
#define LEDC_TIMER1_OVF_INT_ENA    (BIT(1))
#define LEDC_TIMER1_OVF_INT_ENA_M  (LEDC_TIMER1_OVF_INT_ENA_V << LEDC_TIMER1_OVF_INT_ENA_S)
#define LEDC_TIMER1_OVF_INT_ENA_V  0x00000001U
#define LEDC_TIMER1_OVF_INT_ENA_S  1
/** LEDC_TIMER2_OVF_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  Enable bit: Write 1 to enable LEDC_TIMER2_OVF_INT.
 */
#define LEDC_TIMER2_OVF_INT_ENA    (BIT(2))
#define LEDC_TIMER2_OVF_INT_ENA_M  (LEDC_TIMER2_OVF_INT_ENA_V << LEDC_TIMER2_OVF_INT_ENA_S)
#define LEDC_TIMER2_OVF_INT_ENA_V  0x00000001U
#define LEDC_TIMER2_OVF_INT_ENA_S  2
/** LEDC_TIMER3_OVF_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  Enable bit: Write 1 to enable LEDC_TIMER3_OVF_INT.
 */
#define LEDC_TIMER3_OVF_INT_ENA    (BIT(3))
#define LEDC_TIMER3_OVF_INT_ENA_M  (LEDC_TIMER3_OVF_INT_ENA_V << LEDC_TIMER3_OVF_INT_ENA_S)
#define LEDC_TIMER3_OVF_INT_ENA_V  0x00000001U
#define LEDC_TIMER3_OVF_INT_ENA_S  3
/** LEDC_DUTY_CHNG_END_CH0_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  Enable bit: Write 1 to enable LEDC_DUTY_CHNG_END_CH0_INT.
 */
#define LEDC_DUTY_CHNG_END_CH0_INT_ENA    (BIT(4))
#define LEDC_DUTY_CHNG_END_CH0_INT_ENA_M  (LEDC_DUTY_CHNG_END_CH0_INT_ENA_V << LEDC_DUTY_CHNG_END_CH0_INT_ENA_S)
#define LEDC_DUTY_CHNG_END_CH0_INT_ENA_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH0_INT_ENA_S  4
/** LEDC_DUTY_CHNG_END_CH1_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  Enable bit: Write 1 to enable LEDC_DUTY_CHNG_END_CH1_INT.
 */
#define LEDC_DUTY_CHNG_END_CH1_INT_ENA    (BIT(5))
#define LEDC_DUTY_CHNG_END_CH1_INT_ENA_M  (LEDC_DUTY_CHNG_END_CH1_INT_ENA_V << LEDC_DUTY_CHNG_END_CH1_INT_ENA_S)
#define LEDC_DUTY_CHNG_END_CH1_INT_ENA_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH1_INT_ENA_S  5
/** LEDC_DUTY_CHNG_END_CH2_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  Enable bit: Write 1 to enable LEDC_DUTY_CHNG_END_CH2_INT.
 */
#define LEDC_DUTY_CHNG_END_CH2_INT_ENA    (BIT(6))
#define LEDC_DUTY_CHNG_END_CH2_INT_ENA_M  (LEDC_DUTY_CHNG_END_CH2_INT_ENA_V << LEDC_DUTY_CHNG_END_CH2_INT_ENA_S)
#define LEDC_DUTY_CHNG_END_CH2_INT_ENA_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH2_INT_ENA_S  6
/** LEDC_DUTY_CHNG_END_CH3_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  Enable bit: Write 1 to enable LEDC_DUTY_CHNG_END_CH3_INT.
 */
#define LEDC_DUTY_CHNG_END_CH3_INT_ENA    (BIT(7))
#define LEDC_DUTY_CHNG_END_CH3_INT_ENA_M  (LEDC_DUTY_CHNG_END_CH3_INT_ENA_V << LEDC_DUTY_CHNG_END_CH3_INT_ENA_S)
#define LEDC_DUTY_CHNG_END_CH3_INT_ENA_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH3_INT_ENA_S  7
/** LEDC_DUTY_CHNG_END_CH4_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  Enable bit: Write 1 to enable LEDC_DUTY_CHNG_END_CH4_INT.
 */
#define LEDC_DUTY_CHNG_END_CH4_INT_ENA    (BIT(8))
#define LEDC_DUTY_CHNG_END_CH4_INT_ENA_M  (LEDC_DUTY_CHNG_END_CH4_INT_ENA_V << LEDC_DUTY_CHNG_END_CH4_INT_ENA_S)
#define LEDC_DUTY_CHNG_END_CH4_INT_ENA_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH4_INT_ENA_S  8
/** LEDC_DUTY_CHNG_END_CH5_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  Enable bit: Write 1 to enable LEDC_DUTY_CHNG_END_CH5_INT.
 */
#define LEDC_DUTY_CHNG_END_CH5_INT_ENA    (BIT(9))
#define LEDC_DUTY_CHNG_END_CH5_INT_ENA_M  (LEDC_DUTY_CHNG_END_CH5_INT_ENA_V << LEDC_DUTY_CHNG_END_CH5_INT_ENA_S)
#define LEDC_DUTY_CHNG_END_CH5_INT_ENA_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH5_INT_ENA_S  9
/** LEDC_OVF_CNT_CH0_INT_ENA : R/W; bitpos: [12]; default: 0;
 *  Enable bit: Write 1 to enable LEDC_OVF_CNT_CH0_INT.
 */
#define LEDC_OVF_CNT_CH0_INT_ENA    (BIT(12))
#define LEDC_OVF_CNT_CH0_INT_ENA_M  (LEDC_OVF_CNT_CH0_INT_ENA_V << LEDC_OVF_CNT_CH0_INT_ENA_S)
#define LEDC_OVF_CNT_CH0_INT_ENA_V  0x00000001U
#define LEDC_OVF_CNT_CH0_INT_ENA_S  12
/** LEDC_OVF_CNT_CH1_INT_ENA : R/W; bitpos: [13]; default: 0;
 *  Enable bit: Write 1 to enable LEDC_OVF_CNT_CH1_INT.
 */
#define LEDC_OVF_CNT_CH1_INT_ENA    (BIT(13))
#define LEDC_OVF_CNT_CH1_INT_ENA_M  (LEDC_OVF_CNT_CH1_INT_ENA_V << LEDC_OVF_CNT_CH1_INT_ENA_S)
#define LEDC_OVF_CNT_CH1_INT_ENA_V  0x00000001U
#define LEDC_OVF_CNT_CH1_INT_ENA_S  13
/** LEDC_OVF_CNT_CH2_INT_ENA : R/W; bitpos: [14]; default: 0;
 *  Enable bit: Write 1 to enable LEDC_OVF_CNT_CH2_INT.
 */
#define LEDC_OVF_CNT_CH2_INT_ENA    (BIT(14))
#define LEDC_OVF_CNT_CH2_INT_ENA_M  (LEDC_OVF_CNT_CH2_INT_ENA_V << LEDC_OVF_CNT_CH2_INT_ENA_S)
#define LEDC_OVF_CNT_CH2_INT_ENA_V  0x00000001U
#define LEDC_OVF_CNT_CH2_INT_ENA_S  14
/** LEDC_OVF_CNT_CH3_INT_ENA : R/W; bitpos: [15]; default: 0;
 *  Enable bit: Write 1 to enable LEDC_OVF_CNT_CH3_INT.
 */
#define LEDC_OVF_CNT_CH3_INT_ENA    (BIT(15))
#define LEDC_OVF_CNT_CH3_INT_ENA_M  (LEDC_OVF_CNT_CH3_INT_ENA_V << LEDC_OVF_CNT_CH3_INT_ENA_S)
#define LEDC_OVF_CNT_CH3_INT_ENA_V  0x00000001U
#define LEDC_OVF_CNT_CH3_INT_ENA_S  15
/** LEDC_OVF_CNT_CH4_INT_ENA : R/W; bitpos: [16]; default: 0;
 *  Enable bit: Write 1 to enable LEDC_OVF_CNT_CH4_INT.
 */
#define LEDC_OVF_CNT_CH4_INT_ENA    (BIT(16))
#define LEDC_OVF_CNT_CH4_INT_ENA_M  (LEDC_OVF_CNT_CH4_INT_ENA_V << LEDC_OVF_CNT_CH4_INT_ENA_S)
#define LEDC_OVF_CNT_CH4_INT_ENA_V  0x00000001U
#define LEDC_OVF_CNT_CH4_INT_ENA_S  16
/** LEDC_OVF_CNT_CH5_INT_ENA : R/W; bitpos: [17]; default: 0;
 *  Enable bit: Write 1 to enable LEDC_OVF_CNT_CH5_INT.
 */
#define LEDC_OVF_CNT_CH5_INT_ENA    (BIT(17))
#define LEDC_OVF_CNT_CH5_INT_ENA_M  (LEDC_OVF_CNT_CH5_INT_ENA_V << LEDC_OVF_CNT_CH5_INT_ENA_S)
#define LEDC_OVF_CNT_CH5_INT_ENA_V  0x00000001U
#define LEDC_OVF_CNT_CH5_INT_ENA_S  17

/** LEDC_INT_CLR_REG register
 *  Interrupt clear register
 */
#define LEDC_INT_CLR_REG (DR_REG_LEDC_BASE + 0xcc)
/** LEDC_TIMER0_OVF_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Clear bit: Write 1 to clear LEDC_TIMER0_OVF_INT.
 */
#define LEDC_TIMER0_OVF_INT_CLR    (BIT(0))
#define LEDC_TIMER0_OVF_INT_CLR_M  (LEDC_TIMER0_OVF_INT_CLR_V << LEDC_TIMER0_OVF_INT_CLR_S)
#define LEDC_TIMER0_OVF_INT_CLR_V  0x00000001U
#define LEDC_TIMER0_OVF_INT_CLR_S  0
/** LEDC_TIMER1_OVF_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Clear bit: Write 1 to clear LEDC_TIMER1_OVF_INT.
 */
#define LEDC_TIMER1_OVF_INT_CLR    (BIT(1))
#define LEDC_TIMER1_OVF_INT_CLR_M  (LEDC_TIMER1_OVF_INT_CLR_V << LEDC_TIMER1_OVF_INT_CLR_S)
#define LEDC_TIMER1_OVF_INT_CLR_V  0x00000001U
#define LEDC_TIMER1_OVF_INT_CLR_S  1
/** LEDC_TIMER2_OVF_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Clear bit: Write 1 to clear LEDC_TIMER2_OVF_INT.
 */
#define LEDC_TIMER2_OVF_INT_CLR    (BIT(2))
#define LEDC_TIMER2_OVF_INT_CLR_M  (LEDC_TIMER2_OVF_INT_CLR_V << LEDC_TIMER2_OVF_INT_CLR_S)
#define LEDC_TIMER2_OVF_INT_CLR_V  0x00000001U
#define LEDC_TIMER2_OVF_INT_CLR_S  2
/** LEDC_TIMER3_OVF_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Clear bit: Write 1 to clear LEDC_TIMER3_OVF_INT.
 */
#define LEDC_TIMER3_OVF_INT_CLR    (BIT(3))
#define LEDC_TIMER3_OVF_INT_CLR_M  (LEDC_TIMER3_OVF_INT_CLR_V << LEDC_TIMER3_OVF_INT_CLR_S)
#define LEDC_TIMER3_OVF_INT_CLR_V  0x00000001U
#define LEDC_TIMER3_OVF_INT_CLR_S  3
/** LEDC_DUTY_CHNG_END_CH0_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Clear bit: Write 1 to clear LEDC_DUTY_CHNG_END_CH0_INT.
 */
#define LEDC_DUTY_CHNG_END_CH0_INT_CLR    (BIT(4))
#define LEDC_DUTY_CHNG_END_CH0_INT_CLR_M  (LEDC_DUTY_CHNG_END_CH0_INT_CLR_V << LEDC_DUTY_CHNG_END_CH0_INT_CLR_S)
#define LEDC_DUTY_CHNG_END_CH0_INT_CLR_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH0_INT_CLR_S  4
/** LEDC_DUTY_CHNG_END_CH1_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Clear bit: Write 1 to clear LEDC_DUTY_CHNG_END_CH1_INT.
 */
#define LEDC_DUTY_CHNG_END_CH1_INT_CLR    (BIT(5))
#define LEDC_DUTY_CHNG_END_CH1_INT_CLR_M  (LEDC_DUTY_CHNG_END_CH1_INT_CLR_V << LEDC_DUTY_CHNG_END_CH1_INT_CLR_S)
#define LEDC_DUTY_CHNG_END_CH1_INT_CLR_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH1_INT_CLR_S  5
/** LEDC_DUTY_CHNG_END_CH2_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Clear bit: Write 1 to clear LEDC_DUTY_CHNG_END_CH2_INT.
 */
#define LEDC_DUTY_CHNG_END_CH2_INT_CLR    (BIT(6))
#define LEDC_DUTY_CHNG_END_CH2_INT_CLR_M  (LEDC_DUTY_CHNG_END_CH2_INT_CLR_V << LEDC_DUTY_CHNG_END_CH2_INT_CLR_S)
#define LEDC_DUTY_CHNG_END_CH2_INT_CLR_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH2_INT_CLR_S  6
/** LEDC_DUTY_CHNG_END_CH3_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Clear bit: Write 1 to clear LEDC_DUTY_CHNG_END_CH3_INT.
 */
#define LEDC_DUTY_CHNG_END_CH3_INT_CLR    (BIT(7))
#define LEDC_DUTY_CHNG_END_CH3_INT_CLR_M  (LEDC_DUTY_CHNG_END_CH3_INT_CLR_V << LEDC_DUTY_CHNG_END_CH3_INT_CLR_S)
#define LEDC_DUTY_CHNG_END_CH3_INT_CLR_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH3_INT_CLR_S  7
/** LEDC_DUTY_CHNG_END_CH4_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Clear bit: Write 1 to clear LEDC_DUTY_CHNG_END_CH4_INT.
 */
#define LEDC_DUTY_CHNG_END_CH4_INT_CLR    (BIT(8))
#define LEDC_DUTY_CHNG_END_CH4_INT_CLR_M  (LEDC_DUTY_CHNG_END_CH4_INT_CLR_V << LEDC_DUTY_CHNG_END_CH4_INT_CLR_S)
#define LEDC_DUTY_CHNG_END_CH4_INT_CLR_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH4_INT_CLR_S  8
/** LEDC_DUTY_CHNG_END_CH5_INT_CLR : WT; bitpos: [9]; default: 0;
 *  Clear bit: Write 1 to clear LEDC_DUTY_CHNG_END_CH5_INT.
 */
#define LEDC_DUTY_CHNG_END_CH5_INT_CLR    (BIT(9))
#define LEDC_DUTY_CHNG_END_CH5_INT_CLR_M  (LEDC_DUTY_CHNG_END_CH5_INT_CLR_V << LEDC_DUTY_CHNG_END_CH5_INT_CLR_S)
#define LEDC_DUTY_CHNG_END_CH5_INT_CLR_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH5_INT_CLR_S  9
/** LEDC_OVF_CNT_CH0_INT_CLR : WT; bitpos: [12]; default: 0;
 *  Clear bit: Write 1 to clear LEDC_OVF_CNT_CH0_INT.
 */
#define LEDC_OVF_CNT_CH0_INT_CLR    (BIT(12))
#define LEDC_OVF_CNT_CH0_INT_CLR_M  (LEDC_OVF_CNT_CH0_INT_CLR_V << LEDC_OVF_CNT_CH0_INT_CLR_S)
#define LEDC_OVF_CNT_CH0_INT_CLR_V  0x00000001U
#define LEDC_OVF_CNT_CH0_INT_CLR_S  12
/** LEDC_OVF_CNT_CH1_INT_CLR : WT; bitpos: [13]; default: 0;
 *  Clear bit: Write 1 to clear LEDC_OVF_CNT_CH1_INT.
 */
#define LEDC_OVF_CNT_CH1_INT_CLR    (BIT(13))
#define LEDC_OVF_CNT_CH1_INT_CLR_M  (LEDC_OVF_CNT_CH1_INT_CLR_V << LEDC_OVF_CNT_CH1_INT_CLR_S)
#define LEDC_OVF_CNT_CH1_INT_CLR_V  0x00000001U
#define LEDC_OVF_CNT_CH1_INT_CLR_S  13
/** LEDC_OVF_CNT_CH2_INT_CLR : WT; bitpos: [14]; default: 0;
 *  Clear bit: Write 1 to clear LEDC_OVF_CNT_CH2_INT.
 */
#define LEDC_OVF_CNT_CH2_INT_CLR    (BIT(14))
#define LEDC_OVF_CNT_CH2_INT_CLR_M  (LEDC_OVF_CNT_CH2_INT_CLR_V << LEDC_OVF_CNT_CH2_INT_CLR_S)
#define LEDC_OVF_CNT_CH2_INT_CLR_V  0x00000001U
#define LEDC_OVF_CNT_CH2_INT_CLR_S  14
/** LEDC_OVF_CNT_CH3_INT_CLR : WT; bitpos: [15]; default: 0;
 *  Clear bit: Write 1 to clear LEDC_OVF_CNT_CH3_INT.
 */
#define LEDC_OVF_CNT_CH3_INT_CLR    (BIT(15))
#define LEDC_OVF_CNT_CH3_INT_CLR_M  (LEDC_OVF_CNT_CH3_INT_CLR_V << LEDC_OVF_CNT_CH3_INT_CLR_S)
#define LEDC_OVF_CNT_CH3_INT_CLR_V  0x00000001U
#define LEDC_OVF_CNT_CH3_INT_CLR_S  15
/** LEDC_OVF_CNT_CH4_INT_CLR : WT; bitpos: [16]; default: 0;
 *  Clear bit: Write 1 to clear LEDC_OVF_CNT_CH4_INT.
 */
#define LEDC_OVF_CNT_CH4_INT_CLR    (BIT(16))
#define LEDC_OVF_CNT_CH4_INT_CLR_M  (LEDC_OVF_CNT_CH4_INT_CLR_V << LEDC_OVF_CNT_CH4_INT_CLR_S)
#define LEDC_OVF_CNT_CH4_INT_CLR_V  0x00000001U
#define LEDC_OVF_CNT_CH4_INT_CLR_S  16
/** LEDC_OVF_CNT_CH5_INT_CLR : WT; bitpos: [17]; default: 0;
 *  Clear bit: Write 1 to clear LEDC_OVF_CNT_CH5_INT.
 */
#define LEDC_OVF_CNT_CH5_INT_CLR    (BIT(17))
#define LEDC_OVF_CNT_CH5_INT_CLR_M  (LEDC_OVF_CNT_CH5_INT_CLR_V << LEDC_OVF_CNT_CH5_INT_CLR_S)
#define LEDC_OVF_CNT_CH5_INT_CLR_V  0x00000001U
#define LEDC_OVF_CNT_CH5_INT_CLR_S  17

/** LEDC_CH0_GAMMA_CONF_REG register
 *  Ledc ch0 gamma config register.
 */
#define LEDC_CH0_GAMMA_CONF_REG (DR_REG_LEDC_BASE + 0x100)
/** LEDC_CH0_GAMMA_ENTRY_NUM : R/W; bitpos: [4:0]; default: 0;
 *  Configures the number of duty cycle fading rages for LEDC ch0.
 */
#define LEDC_CH0_GAMMA_ENTRY_NUM    0x0000001FU
#define LEDC_CH0_GAMMA_ENTRY_NUM_M  (LEDC_CH0_GAMMA_ENTRY_NUM_V << LEDC_CH0_GAMMA_ENTRY_NUM_S)
#define LEDC_CH0_GAMMA_ENTRY_NUM_V  0x0000001FU
#define LEDC_CH0_GAMMA_ENTRY_NUM_S  0
/** LEDC_CH0_GAMMA_PAUSE : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to pause duty cycle fading of LEDC ch0.\\0: Invalid. No
 *  effect\\1: Pause
 */
#define LEDC_CH0_GAMMA_PAUSE    (BIT(5))
#define LEDC_CH0_GAMMA_PAUSE_M  (LEDC_CH0_GAMMA_PAUSE_V << LEDC_CH0_GAMMA_PAUSE_S)
#define LEDC_CH0_GAMMA_PAUSE_V  0x00000001U
#define LEDC_CH0_GAMMA_PAUSE_S  5
/** LEDC_CH0_GAMMA_RESUME : WT; bitpos: [6]; default: 0;
 *  Configures whether or nor to resume duty cycle fading of LEDC ch0.\\0: Invalid. No
 *  effect\\1: Resume
 */
#define LEDC_CH0_GAMMA_RESUME    (BIT(6))
#define LEDC_CH0_GAMMA_RESUME_M  (LEDC_CH0_GAMMA_RESUME_V << LEDC_CH0_GAMMA_RESUME_S)
#define LEDC_CH0_GAMMA_RESUME_V  0x00000001U
#define LEDC_CH0_GAMMA_RESUME_S  6

/** LEDC_CH1_GAMMA_CONF_REG register
 *  Ledc ch1 gamma config register.
 */
#define LEDC_CH1_GAMMA_CONF_REG (DR_REG_LEDC_BASE + 0x104)
/** LEDC_CH1_GAMMA_ENTRY_NUM : R/W; bitpos: [4:0]; default: 0;
 *  Configures the number of duty cycle fading rages for LEDC ch1.
 */
#define LEDC_CH1_GAMMA_ENTRY_NUM    0x0000001FU
#define LEDC_CH1_GAMMA_ENTRY_NUM_M  (LEDC_CH1_GAMMA_ENTRY_NUM_V << LEDC_CH1_GAMMA_ENTRY_NUM_S)
#define LEDC_CH1_GAMMA_ENTRY_NUM_V  0x0000001FU
#define LEDC_CH1_GAMMA_ENTRY_NUM_S  0
/** LEDC_CH1_GAMMA_PAUSE : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to pause duty cycle fading of LEDC ch1.\\0: Invalid. No
 *  effect\\1: Pause
 */
#define LEDC_CH1_GAMMA_PAUSE    (BIT(5))
#define LEDC_CH1_GAMMA_PAUSE_M  (LEDC_CH1_GAMMA_PAUSE_V << LEDC_CH1_GAMMA_PAUSE_S)
#define LEDC_CH1_GAMMA_PAUSE_V  0x00000001U
#define LEDC_CH1_GAMMA_PAUSE_S  5
/** LEDC_CH1_GAMMA_RESUME : WT; bitpos: [6]; default: 0;
 *  Configures whether or nor to resume duty cycle fading of LEDC ch1.\\0: Invalid. No
 *  effect\\1: Resume
 */
#define LEDC_CH1_GAMMA_RESUME    (BIT(6))
#define LEDC_CH1_GAMMA_RESUME_M  (LEDC_CH1_GAMMA_RESUME_V << LEDC_CH1_GAMMA_RESUME_S)
#define LEDC_CH1_GAMMA_RESUME_V  0x00000001U
#define LEDC_CH1_GAMMA_RESUME_S  6

/** LEDC_CH2_GAMMA_CONF_REG register
 *  Ledc ch2 gamma config register.
 */
#define LEDC_CH2_GAMMA_CONF_REG (DR_REG_LEDC_BASE + 0x108)
/** LEDC_CH2_GAMMA_ENTRY_NUM : R/W; bitpos: [4:0]; default: 0;
 *  Configures the number of duty cycle fading rages for LEDC ch2.
 */
#define LEDC_CH2_GAMMA_ENTRY_NUM    0x0000001FU
#define LEDC_CH2_GAMMA_ENTRY_NUM_M  (LEDC_CH2_GAMMA_ENTRY_NUM_V << LEDC_CH2_GAMMA_ENTRY_NUM_S)
#define LEDC_CH2_GAMMA_ENTRY_NUM_V  0x0000001FU
#define LEDC_CH2_GAMMA_ENTRY_NUM_S  0
/** LEDC_CH2_GAMMA_PAUSE : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to pause duty cycle fading of LEDC ch2.\\0: Invalid. No
 *  effect\\1: Pause
 */
#define LEDC_CH2_GAMMA_PAUSE    (BIT(5))
#define LEDC_CH2_GAMMA_PAUSE_M  (LEDC_CH2_GAMMA_PAUSE_V << LEDC_CH2_GAMMA_PAUSE_S)
#define LEDC_CH2_GAMMA_PAUSE_V  0x00000001U
#define LEDC_CH2_GAMMA_PAUSE_S  5
/** LEDC_CH2_GAMMA_RESUME : WT; bitpos: [6]; default: 0;
 *  Configures whether or nor to resume duty cycle fading of LEDC ch2.\\0: Invalid. No
 *  effect\\1: Resume
 */
#define LEDC_CH2_GAMMA_RESUME    (BIT(6))
#define LEDC_CH2_GAMMA_RESUME_M  (LEDC_CH2_GAMMA_RESUME_V << LEDC_CH2_GAMMA_RESUME_S)
#define LEDC_CH2_GAMMA_RESUME_V  0x00000001U
#define LEDC_CH2_GAMMA_RESUME_S  6

/** LEDC_CH3_GAMMA_CONF_REG register
 *  Ledc ch3 gamma config register.
 */
#define LEDC_CH3_GAMMA_CONF_REG (DR_REG_LEDC_BASE + 0x10c)
/** LEDC_CH3_GAMMA_ENTRY_NUM : R/W; bitpos: [4:0]; default: 0;
 *  Configures the number of duty cycle fading rages for LEDC ch3.
 */
#define LEDC_CH3_GAMMA_ENTRY_NUM    0x0000001FU
#define LEDC_CH3_GAMMA_ENTRY_NUM_M  (LEDC_CH3_GAMMA_ENTRY_NUM_V << LEDC_CH3_GAMMA_ENTRY_NUM_S)
#define LEDC_CH3_GAMMA_ENTRY_NUM_V  0x0000001FU
#define LEDC_CH3_GAMMA_ENTRY_NUM_S  0
/** LEDC_CH3_GAMMA_PAUSE : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to pause duty cycle fading of LEDC ch3.\\0: Invalid. No
 *  effect\\1: Pause
 */
#define LEDC_CH3_GAMMA_PAUSE    (BIT(5))
#define LEDC_CH3_GAMMA_PAUSE_M  (LEDC_CH3_GAMMA_PAUSE_V << LEDC_CH3_GAMMA_PAUSE_S)
#define LEDC_CH3_GAMMA_PAUSE_V  0x00000001U
#define LEDC_CH3_GAMMA_PAUSE_S  5
/** LEDC_CH3_GAMMA_RESUME : WT; bitpos: [6]; default: 0;
 *  Configures whether or nor to resume duty cycle fading of LEDC ch3.\\0: Invalid. No
 *  effect\\1: Resume
 */
#define LEDC_CH3_GAMMA_RESUME    (BIT(6))
#define LEDC_CH3_GAMMA_RESUME_M  (LEDC_CH3_GAMMA_RESUME_V << LEDC_CH3_GAMMA_RESUME_S)
#define LEDC_CH3_GAMMA_RESUME_V  0x00000001U
#define LEDC_CH3_GAMMA_RESUME_S  6

/** LEDC_CH4_GAMMA_CONF_REG register
 *  Ledc ch4 gamma config register.
 */
#define LEDC_CH4_GAMMA_CONF_REG (DR_REG_LEDC_BASE + 0x110)
/** LEDC_CH4_GAMMA_ENTRY_NUM : R/W; bitpos: [4:0]; default: 0;
 *  Configures the number of duty cycle fading rages for LEDC ch4.
 */
#define LEDC_CH4_GAMMA_ENTRY_NUM    0x0000001FU
#define LEDC_CH4_GAMMA_ENTRY_NUM_M  (LEDC_CH4_GAMMA_ENTRY_NUM_V << LEDC_CH4_GAMMA_ENTRY_NUM_S)
#define LEDC_CH4_GAMMA_ENTRY_NUM_V  0x0000001FU
#define LEDC_CH4_GAMMA_ENTRY_NUM_S  0
/** LEDC_CH4_GAMMA_PAUSE : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to pause duty cycle fading of LEDC ch4.\\0: Invalid. No
 *  effect\\1: Pause
 */
#define LEDC_CH4_GAMMA_PAUSE    (BIT(5))
#define LEDC_CH4_GAMMA_PAUSE_M  (LEDC_CH4_GAMMA_PAUSE_V << LEDC_CH4_GAMMA_PAUSE_S)
#define LEDC_CH4_GAMMA_PAUSE_V  0x00000001U
#define LEDC_CH4_GAMMA_PAUSE_S  5
/** LEDC_CH4_GAMMA_RESUME : WT; bitpos: [6]; default: 0;
 *  Configures whether or nor to resume duty cycle fading of LEDC ch4.\\0: Invalid. No
 *  effect\\1: Resume
 */
#define LEDC_CH4_GAMMA_RESUME    (BIT(6))
#define LEDC_CH4_GAMMA_RESUME_M  (LEDC_CH4_GAMMA_RESUME_V << LEDC_CH4_GAMMA_RESUME_S)
#define LEDC_CH4_GAMMA_RESUME_V  0x00000001U
#define LEDC_CH4_GAMMA_RESUME_S  6

/** LEDC_CH5_GAMMA_CONF_REG register
 *  Ledc ch5 gamma config register.
 */
#define LEDC_CH5_GAMMA_CONF_REG (DR_REG_LEDC_BASE + 0x114)
/** LEDC_CH5_GAMMA_ENTRY_NUM : R/W; bitpos: [4:0]; default: 0;
 *  Configures the number of duty cycle fading rages for LEDC ch5.
 */
#define LEDC_CH5_GAMMA_ENTRY_NUM    0x0000001FU
#define LEDC_CH5_GAMMA_ENTRY_NUM_M  (LEDC_CH5_GAMMA_ENTRY_NUM_V << LEDC_CH5_GAMMA_ENTRY_NUM_S)
#define LEDC_CH5_GAMMA_ENTRY_NUM_V  0x0000001FU
#define LEDC_CH5_GAMMA_ENTRY_NUM_S  0
/** LEDC_CH5_GAMMA_PAUSE : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to pause duty cycle fading of LEDC ch5.\\0: Invalid. No
 *  effect\\1: Pause
 */
#define LEDC_CH5_GAMMA_PAUSE    (BIT(5))
#define LEDC_CH5_GAMMA_PAUSE_M  (LEDC_CH5_GAMMA_PAUSE_V << LEDC_CH5_GAMMA_PAUSE_S)
#define LEDC_CH5_GAMMA_PAUSE_V  0x00000001U
#define LEDC_CH5_GAMMA_PAUSE_S  5
/** LEDC_CH5_GAMMA_RESUME : WT; bitpos: [6]; default: 0;
 *  Configures whether or nor to resume duty cycle fading of LEDC ch5.\\0: Invalid. No
 *  effect\\1: Resume
 */
#define LEDC_CH5_GAMMA_RESUME    (BIT(6))
#define LEDC_CH5_GAMMA_RESUME_M  (LEDC_CH5_GAMMA_RESUME_V << LEDC_CH5_GAMMA_RESUME_S)
#define LEDC_CH5_GAMMA_RESUME_V  0x00000001U
#define LEDC_CH5_GAMMA_RESUME_S  6

/** LEDC_EVT_TASK_EN0_REG register
 *  Ledc event task enable bit register0.
 */
#define LEDC_EVT_TASK_EN0_REG (DR_REG_LEDC_BASE + 0x120)
/** LEDC_EVT_DUTY_CHNG_END_CH0_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_DUTY_CHNG_END_CH0 event.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_EVT_DUTY_CHNG_END_CH0_EN    (BIT(0))
#define LEDC_EVT_DUTY_CHNG_END_CH0_EN_M  (LEDC_EVT_DUTY_CHNG_END_CH0_EN_V << LEDC_EVT_DUTY_CHNG_END_CH0_EN_S)
#define LEDC_EVT_DUTY_CHNG_END_CH0_EN_V  0x00000001U
#define LEDC_EVT_DUTY_CHNG_END_CH0_EN_S  0
/** LEDC_EVT_DUTY_CHNG_END_CH1_EN : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_DUTY_CHNG_END_CH1 event.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_EVT_DUTY_CHNG_END_CH1_EN    (BIT(1))
#define LEDC_EVT_DUTY_CHNG_END_CH1_EN_M  (LEDC_EVT_DUTY_CHNG_END_CH1_EN_V << LEDC_EVT_DUTY_CHNG_END_CH1_EN_S)
#define LEDC_EVT_DUTY_CHNG_END_CH1_EN_V  0x00000001U
#define LEDC_EVT_DUTY_CHNG_END_CH1_EN_S  1
/** LEDC_EVT_DUTY_CHNG_END_CH2_EN : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_DUTY_CHNG_END_CH2 event.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_EVT_DUTY_CHNG_END_CH2_EN    (BIT(2))
#define LEDC_EVT_DUTY_CHNG_END_CH2_EN_M  (LEDC_EVT_DUTY_CHNG_END_CH2_EN_V << LEDC_EVT_DUTY_CHNG_END_CH2_EN_S)
#define LEDC_EVT_DUTY_CHNG_END_CH2_EN_V  0x00000001U
#define LEDC_EVT_DUTY_CHNG_END_CH2_EN_S  2
/** LEDC_EVT_DUTY_CHNG_END_CH3_EN : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_DUTY_CHNG_END_CH3 event.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_EVT_DUTY_CHNG_END_CH3_EN    (BIT(3))
#define LEDC_EVT_DUTY_CHNG_END_CH3_EN_M  (LEDC_EVT_DUTY_CHNG_END_CH3_EN_V << LEDC_EVT_DUTY_CHNG_END_CH3_EN_S)
#define LEDC_EVT_DUTY_CHNG_END_CH3_EN_V  0x00000001U
#define LEDC_EVT_DUTY_CHNG_END_CH3_EN_S  3
/** LEDC_EVT_DUTY_CHNG_END_CH4_EN : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_DUTY_CHNG_END_CH4 event.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_EVT_DUTY_CHNG_END_CH4_EN    (BIT(4))
#define LEDC_EVT_DUTY_CHNG_END_CH4_EN_M  (LEDC_EVT_DUTY_CHNG_END_CH4_EN_V << LEDC_EVT_DUTY_CHNG_END_CH4_EN_S)
#define LEDC_EVT_DUTY_CHNG_END_CH4_EN_V  0x00000001U
#define LEDC_EVT_DUTY_CHNG_END_CH4_EN_S  4
/** LEDC_EVT_DUTY_CHNG_END_CH5_EN : R/W; bitpos: [5]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_DUTY_CHNG_END_CH5 event.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_EVT_DUTY_CHNG_END_CH5_EN    (BIT(5))
#define LEDC_EVT_DUTY_CHNG_END_CH5_EN_M  (LEDC_EVT_DUTY_CHNG_END_CH5_EN_V << LEDC_EVT_DUTY_CHNG_END_CH5_EN_S)
#define LEDC_EVT_DUTY_CHNG_END_CH5_EN_V  0x00000001U
#define LEDC_EVT_DUTY_CHNG_END_CH5_EN_S  5
/** LEDC_EVT_OVF_CNT_PLS_CH0_EN : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_OVF_CNT_PLS_CH0 event.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_EVT_OVF_CNT_PLS_CH0_EN    (BIT(8))
#define LEDC_EVT_OVF_CNT_PLS_CH0_EN_M  (LEDC_EVT_OVF_CNT_PLS_CH0_EN_V << LEDC_EVT_OVF_CNT_PLS_CH0_EN_S)
#define LEDC_EVT_OVF_CNT_PLS_CH0_EN_V  0x00000001U
#define LEDC_EVT_OVF_CNT_PLS_CH0_EN_S  8
/** LEDC_EVT_OVF_CNT_PLS_CH1_EN : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_OVF_CNT_PLS_CH1 event.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_EVT_OVF_CNT_PLS_CH1_EN    (BIT(9))
#define LEDC_EVT_OVF_CNT_PLS_CH1_EN_M  (LEDC_EVT_OVF_CNT_PLS_CH1_EN_V << LEDC_EVT_OVF_CNT_PLS_CH1_EN_S)
#define LEDC_EVT_OVF_CNT_PLS_CH1_EN_V  0x00000001U
#define LEDC_EVT_OVF_CNT_PLS_CH1_EN_S  9
/** LEDC_EVT_OVF_CNT_PLS_CH2_EN : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_OVF_CNT_PLS_CH2 event.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_EVT_OVF_CNT_PLS_CH2_EN    (BIT(10))
#define LEDC_EVT_OVF_CNT_PLS_CH2_EN_M  (LEDC_EVT_OVF_CNT_PLS_CH2_EN_V << LEDC_EVT_OVF_CNT_PLS_CH2_EN_S)
#define LEDC_EVT_OVF_CNT_PLS_CH2_EN_V  0x00000001U
#define LEDC_EVT_OVF_CNT_PLS_CH2_EN_S  10
/** LEDC_EVT_OVF_CNT_PLS_CH3_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_OVF_CNT_PLS_CH3 event.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_EVT_OVF_CNT_PLS_CH3_EN    (BIT(11))
#define LEDC_EVT_OVF_CNT_PLS_CH3_EN_M  (LEDC_EVT_OVF_CNT_PLS_CH3_EN_V << LEDC_EVT_OVF_CNT_PLS_CH3_EN_S)
#define LEDC_EVT_OVF_CNT_PLS_CH3_EN_V  0x00000001U
#define LEDC_EVT_OVF_CNT_PLS_CH3_EN_S  11
/** LEDC_EVT_OVF_CNT_PLS_CH4_EN : R/W; bitpos: [12]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_OVF_CNT_PLS_CH4 event.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_EVT_OVF_CNT_PLS_CH4_EN    (BIT(12))
#define LEDC_EVT_OVF_CNT_PLS_CH4_EN_M  (LEDC_EVT_OVF_CNT_PLS_CH4_EN_V << LEDC_EVT_OVF_CNT_PLS_CH4_EN_S)
#define LEDC_EVT_OVF_CNT_PLS_CH4_EN_V  0x00000001U
#define LEDC_EVT_OVF_CNT_PLS_CH4_EN_S  12
/** LEDC_EVT_OVF_CNT_PLS_CH5_EN : R/W; bitpos: [13]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_OVF_CNT_PLS_CH5 event.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_EVT_OVF_CNT_PLS_CH5_EN    (BIT(13))
#define LEDC_EVT_OVF_CNT_PLS_CH5_EN_M  (LEDC_EVT_OVF_CNT_PLS_CH5_EN_V << LEDC_EVT_OVF_CNT_PLS_CH5_EN_S)
#define LEDC_EVT_OVF_CNT_PLS_CH5_EN_V  0x00000001U
#define LEDC_EVT_OVF_CNT_PLS_CH5_EN_S  13
/** LEDC_EVT_TIME_OVF_TIMER0_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_TIME_OVF_TIMER0 event.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_EVT_TIME_OVF_TIMER0_EN    (BIT(16))
#define LEDC_EVT_TIME_OVF_TIMER0_EN_M  (LEDC_EVT_TIME_OVF_TIMER0_EN_V << LEDC_EVT_TIME_OVF_TIMER0_EN_S)
#define LEDC_EVT_TIME_OVF_TIMER0_EN_V  0x00000001U
#define LEDC_EVT_TIME_OVF_TIMER0_EN_S  16
/** LEDC_EVT_TIME_OVF_TIMER1_EN : R/W; bitpos: [17]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_TIME_OVF_TIMER1 event.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_EVT_TIME_OVF_TIMER1_EN    (BIT(17))
#define LEDC_EVT_TIME_OVF_TIMER1_EN_M  (LEDC_EVT_TIME_OVF_TIMER1_EN_V << LEDC_EVT_TIME_OVF_TIMER1_EN_S)
#define LEDC_EVT_TIME_OVF_TIMER1_EN_V  0x00000001U
#define LEDC_EVT_TIME_OVF_TIMER1_EN_S  17
/** LEDC_EVT_TIME_OVF_TIMER2_EN : R/W; bitpos: [18]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_TIME_OVF_TIMER2 event.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_EVT_TIME_OVF_TIMER2_EN    (BIT(18))
#define LEDC_EVT_TIME_OVF_TIMER2_EN_M  (LEDC_EVT_TIME_OVF_TIMER2_EN_V << LEDC_EVT_TIME_OVF_TIMER2_EN_S)
#define LEDC_EVT_TIME_OVF_TIMER2_EN_V  0x00000001U
#define LEDC_EVT_TIME_OVF_TIMER2_EN_S  18
/** LEDC_EVT_TIME_OVF_TIMER3_EN : R/W; bitpos: [19]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_TIME_OVF_TIMER3 event.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_EVT_TIME_OVF_TIMER3_EN    (BIT(19))
#define LEDC_EVT_TIME_OVF_TIMER3_EN_M  (LEDC_EVT_TIME_OVF_TIMER3_EN_V << LEDC_EVT_TIME_OVF_TIMER3_EN_S)
#define LEDC_EVT_TIME_OVF_TIMER3_EN_V  0x00000001U
#define LEDC_EVT_TIME_OVF_TIMER3_EN_S  19
/** LEDC_EVT_TIME0_CMP_EN : R/W; bitpos: [20]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_TIMER0_CMP event.\\0: Disable\\1:
 *  Enable
 */
#define LEDC_EVT_TIME0_CMP_EN    (BIT(20))
#define LEDC_EVT_TIME0_CMP_EN_M  (LEDC_EVT_TIME0_CMP_EN_V << LEDC_EVT_TIME0_CMP_EN_S)
#define LEDC_EVT_TIME0_CMP_EN_V  0x00000001U
#define LEDC_EVT_TIME0_CMP_EN_S  20
/** LEDC_EVT_TIME1_CMP_EN : R/W; bitpos: [21]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_TIMER1_CMP event.\\0: Disable\\1:
 *  Enable
 */
#define LEDC_EVT_TIME1_CMP_EN    (BIT(21))
#define LEDC_EVT_TIME1_CMP_EN_M  (LEDC_EVT_TIME1_CMP_EN_V << LEDC_EVT_TIME1_CMP_EN_S)
#define LEDC_EVT_TIME1_CMP_EN_V  0x00000001U
#define LEDC_EVT_TIME1_CMP_EN_S  21
/** LEDC_EVT_TIME2_CMP_EN : R/W; bitpos: [22]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_TIMER2_CMP event.\\0: Disable\\1:
 *  Enable
 */
#define LEDC_EVT_TIME2_CMP_EN    (BIT(22))
#define LEDC_EVT_TIME2_CMP_EN_M  (LEDC_EVT_TIME2_CMP_EN_V << LEDC_EVT_TIME2_CMP_EN_S)
#define LEDC_EVT_TIME2_CMP_EN_V  0x00000001U
#define LEDC_EVT_TIME2_CMP_EN_S  22
/** LEDC_EVT_TIME3_CMP_EN : R/W; bitpos: [23]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_TIMER3_CMP event.\\0: Disable\\1:
 *  Enable
 */
#define LEDC_EVT_TIME3_CMP_EN    (BIT(23))
#define LEDC_EVT_TIME3_CMP_EN_M  (LEDC_EVT_TIME3_CMP_EN_V << LEDC_EVT_TIME3_CMP_EN_S)
#define LEDC_EVT_TIME3_CMP_EN_V  0x00000001U
#define LEDC_EVT_TIME3_CMP_EN_S  23
/** LEDC_TASK_DUTY_SCALE_UPDATE_CH0_EN : R/W; bitpos: [24]; default: 0;
 *  Configures whether or not to enable the LEDC_TASK_DUTY_SCALE_UPDATE_CH0 task.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH0_EN    (BIT(24))
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH0_EN_M  (LEDC_TASK_DUTY_SCALE_UPDATE_CH0_EN_V << LEDC_TASK_DUTY_SCALE_UPDATE_CH0_EN_S)
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH0_EN_V  0x00000001U
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH0_EN_S  24
/** LEDC_TASK_DUTY_SCALE_UPDATE_CH1_EN : R/W; bitpos: [25]; default: 0;
 *  Configures whether or not to enable the LEDC_TASK_DUTY_SCALE_UPDATE_CH1 task.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH1_EN    (BIT(25))
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH1_EN_M  (LEDC_TASK_DUTY_SCALE_UPDATE_CH1_EN_V << LEDC_TASK_DUTY_SCALE_UPDATE_CH1_EN_S)
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH1_EN_V  0x00000001U
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH1_EN_S  25
/** LEDC_TASK_DUTY_SCALE_UPDATE_CH2_EN : R/W; bitpos: [26]; default: 0;
 *  Configures whether or not to enable the LEDC_TASK_DUTY_SCALE_UPDATE_CH2 task.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH2_EN    (BIT(26))
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH2_EN_M  (LEDC_TASK_DUTY_SCALE_UPDATE_CH2_EN_V << LEDC_TASK_DUTY_SCALE_UPDATE_CH2_EN_S)
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH2_EN_V  0x00000001U
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH2_EN_S  26
/** LEDC_TASK_DUTY_SCALE_UPDATE_CH3_EN : R/W; bitpos: [27]; default: 0;
 *  Configures whether or not to enable the LEDC_TASK_DUTY_SCALE_UPDATE_CH3 task.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH3_EN    (BIT(27))
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH3_EN_M  (LEDC_TASK_DUTY_SCALE_UPDATE_CH3_EN_V << LEDC_TASK_DUTY_SCALE_UPDATE_CH3_EN_S)
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH3_EN_V  0x00000001U
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH3_EN_S  27
/** LEDC_TASK_DUTY_SCALE_UPDATE_CH4_EN : R/W; bitpos: [28]; default: 0;
 *  Configures whether or not to enable the LEDC_TASK_DUTY_SCALE_UPDATE_CH4 task.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH4_EN    (BIT(28))
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH4_EN_M  (LEDC_TASK_DUTY_SCALE_UPDATE_CH4_EN_V << LEDC_TASK_DUTY_SCALE_UPDATE_CH4_EN_S)
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH4_EN_V  0x00000001U
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH4_EN_S  28
/** LEDC_TASK_DUTY_SCALE_UPDATE_CH5_EN : R/W; bitpos: [29]; default: 0;
 *  Configures whether or not to enable the LEDC_TASK_DUTY_SCALE_UPDATE_CH5 task.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH5_EN    (BIT(29))
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH5_EN_M  (LEDC_TASK_DUTY_SCALE_UPDATE_CH5_EN_V << LEDC_TASK_DUTY_SCALE_UPDATE_CH5_EN_S)
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH5_EN_V  0x00000001U
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH5_EN_S  29

/** LEDC_EVT_TASK_EN1_REG register
 *  Ledc event task enable bit register1.
 */
#define LEDC_EVT_TASK_EN1_REG (DR_REG_LEDC_BASE + 0x124)
/** LEDC_TASK_TIMER0_RES_UPDATE_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER0_RES_UPDATE task.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_TASK_TIMER0_RES_UPDATE_EN    (BIT(0))
#define LEDC_TASK_TIMER0_RES_UPDATE_EN_M  (LEDC_TASK_TIMER0_RES_UPDATE_EN_V << LEDC_TASK_TIMER0_RES_UPDATE_EN_S)
#define LEDC_TASK_TIMER0_RES_UPDATE_EN_V  0x00000001U
#define LEDC_TASK_TIMER0_RES_UPDATE_EN_S  0
/** LEDC_TASK_TIMER1_RES_UPDATE_EN : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER1_RES_UPDATE task.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_TASK_TIMER1_RES_UPDATE_EN    (BIT(1))
#define LEDC_TASK_TIMER1_RES_UPDATE_EN_M  (LEDC_TASK_TIMER1_RES_UPDATE_EN_V << LEDC_TASK_TIMER1_RES_UPDATE_EN_S)
#define LEDC_TASK_TIMER1_RES_UPDATE_EN_V  0x00000001U
#define LEDC_TASK_TIMER1_RES_UPDATE_EN_S  1
/** LEDC_TASK_TIMER2_RES_UPDATE_EN : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER2_RES_UPDATE task.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_TASK_TIMER2_RES_UPDATE_EN    (BIT(2))
#define LEDC_TASK_TIMER2_RES_UPDATE_EN_M  (LEDC_TASK_TIMER2_RES_UPDATE_EN_V << LEDC_TASK_TIMER2_RES_UPDATE_EN_S)
#define LEDC_TASK_TIMER2_RES_UPDATE_EN_V  0x00000001U
#define LEDC_TASK_TIMER2_RES_UPDATE_EN_S  2
/** LEDC_TASK_TIMER3_RES_UPDATE_EN : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER3_RES_UPDATE task.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_TASK_TIMER3_RES_UPDATE_EN    (BIT(3))
#define LEDC_TASK_TIMER3_RES_UPDATE_EN_M  (LEDC_TASK_TIMER3_RES_UPDATE_EN_V << LEDC_TASK_TIMER3_RES_UPDATE_EN_S)
#define LEDC_TASK_TIMER3_RES_UPDATE_EN_V  0x00000001U
#define LEDC_TASK_TIMER3_RES_UPDATE_EN_S  3
/** LEDC_TASK_TIMER0_CAP_EN : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER0_CAP task.\\0: Disable\\1:
 *  Enable
 */
#define LEDC_TASK_TIMER0_CAP_EN    (BIT(4))
#define LEDC_TASK_TIMER0_CAP_EN_M  (LEDC_TASK_TIMER0_CAP_EN_V << LEDC_TASK_TIMER0_CAP_EN_S)
#define LEDC_TASK_TIMER0_CAP_EN_V  0x00000001U
#define LEDC_TASK_TIMER0_CAP_EN_S  4
/** LEDC_TASK_TIMER1_CAP_EN : R/W; bitpos: [5]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER1_CAP task.\\0: Disable\\1:
 *  Enable
 */
#define LEDC_TASK_TIMER1_CAP_EN    (BIT(5))
#define LEDC_TASK_TIMER1_CAP_EN_M  (LEDC_TASK_TIMER1_CAP_EN_V << LEDC_TASK_TIMER1_CAP_EN_S)
#define LEDC_TASK_TIMER1_CAP_EN_V  0x00000001U
#define LEDC_TASK_TIMER1_CAP_EN_S  5
/** LEDC_TASK_TIMER2_CAP_EN : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER2_CAP task.\\0: Disable\\1:
 *  Enable
 */
#define LEDC_TASK_TIMER2_CAP_EN    (BIT(6))
#define LEDC_TASK_TIMER2_CAP_EN_M  (LEDC_TASK_TIMER2_CAP_EN_V << LEDC_TASK_TIMER2_CAP_EN_S)
#define LEDC_TASK_TIMER2_CAP_EN_V  0x00000001U
#define LEDC_TASK_TIMER2_CAP_EN_S  6
/** LEDC_TASK_TIMER3_CAP_EN : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER3_CAP task.\\0: Disable\\1:
 *  Enable
 */
#define LEDC_TASK_TIMER3_CAP_EN    (BIT(7))
#define LEDC_TASK_TIMER3_CAP_EN_M  (LEDC_TASK_TIMER3_CAP_EN_V << LEDC_TASK_TIMER3_CAP_EN_S)
#define LEDC_TASK_TIMER3_CAP_EN_V  0x00000001U
#define LEDC_TASK_TIMER3_CAP_EN_S  7
/** LEDC_TASK_SIG_OUT_DIS_CH0_EN : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_SIG_OUT_DIS_CH0 task.\\0: Disable\\1:
 *  Enable
 */
#define LEDC_TASK_SIG_OUT_DIS_CH0_EN    (BIT(8))
#define LEDC_TASK_SIG_OUT_DIS_CH0_EN_M  (LEDC_TASK_SIG_OUT_DIS_CH0_EN_V << LEDC_TASK_SIG_OUT_DIS_CH0_EN_S)
#define LEDC_TASK_SIG_OUT_DIS_CH0_EN_V  0x00000001U
#define LEDC_TASK_SIG_OUT_DIS_CH0_EN_S  8
/** LEDC_TASK_SIG_OUT_DIS_CH1_EN : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_SIG_OUT_DIS_CH1 task.\\0: Disable\\1:
 *  Enable
 */
#define LEDC_TASK_SIG_OUT_DIS_CH1_EN    (BIT(9))
#define LEDC_TASK_SIG_OUT_DIS_CH1_EN_M  (LEDC_TASK_SIG_OUT_DIS_CH1_EN_V << LEDC_TASK_SIG_OUT_DIS_CH1_EN_S)
#define LEDC_TASK_SIG_OUT_DIS_CH1_EN_V  0x00000001U
#define LEDC_TASK_SIG_OUT_DIS_CH1_EN_S  9
/** LEDC_TASK_SIG_OUT_DIS_CH2_EN : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_SIG_OUT_DIS_CH2 task.\\0: Disable\\1:
 *  Enable
 */
#define LEDC_TASK_SIG_OUT_DIS_CH2_EN    (BIT(10))
#define LEDC_TASK_SIG_OUT_DIS_CH2_EN_M  (LEDC_TASK_SIG_OUT_DIS_CH2_EN_V << LEDC_TASK_SIG_OUT_DIS_CH2_EN_S)
#define LEDC_TASK_SIG_OUT_DIS_CH2_EN_V  0x00000001U
#define LEDC_TASK_SIG_OUT_DIS_CH2_EN_S  10
/** LEDC_TASK_SIG_OUT_DIS_CH3_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_SIG_OUT_DIS_CH3 task.\\0: Disable\\1:
 *  Enable
 */
#define LEDC_TASK_SIG_OUT_DIS_CH3_EN    (BIT(11))
#define LEDC_TASK_SIG_OUT_DIS_CH3_EN_M  (LEDC_TASK_SIG_OUT_DIS_CH3_EN_V << LEDC_TASK_SIG_OUT_DIS_CH3_EN_S)
#define LEDC_TASK_SIG_OUT_DIS_CH3_EN_V  0x00000001U
#define LEDC_TASK_SIG_OUT_DIS_CH3_EN_S  11
/** LEDC_TASK_SIG_OUT_DIS_CH4_EN : R/W; bitpos: [12]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_SIG_OUT_DIS_CH4 task.\\0: Disable\\1:
 *  Enable
 */
#define LEDC_TASK_SIG_OUT_DIS_CH4_EN    (BIT(12))
#define LEDC_TASK_SIG_OUT_DIS_CH4_EN_M  (LEDC_TASK_SIG_OUT_DIS_CH4_EN_V << LEDC_TASK_SIG_OUT_DIS_CH4_EN_S)
#define LEDC_TASK_SIG_OUT_DIS_CH4_EN_V  0x00000001U
#define LEDC_TASK_SIG_OUT_DIS_CH4_EN_S  12
/** LEDC_TASK_SIG_OUT_DIS_CH5_EN : R/W; bitpos: [13]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_SIG_OUT_DIS_CH5 task.\\0: Disable\\1:
 *  Enable
 */
#define LEDC_TASK_SIG_OUT_DIS_CH5_EN    (BIT(13))
#define LEDC_TASK_SIG_OUT_DIS_CH5_EN_M  (LEDC_TASK_SIG_OUT_DIS_CH5_EN_V << LEDC_TASK_SIG_OUT_DIS_CH5_EN_S)
#define LEDC_TASK_SIG_OUT_DIS_CH5_EN_V  0x00000001U
#define LEDC_TASK_SIG_OUT_DIS_CH5_EN_S  13
/** LEDC_TASK_OVF_CNT_RST_CH0_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_OVF_CNT_RST_CH0 task.\\0: Disable\\1:
 *  Enable
 */
#define LEDC_TASK_OVF_CNT_RST_CH0_EN    (BIT(16))
#define LEDC_TASK_OVF_CNT_RST_CH0_EN_M  (LEDC_TASK_OVF_CNT_RST_CH0_EN_V << LEDC_TASK_OVF_CNT_RST_CH0_EN_S)
#define LEDC_TASK_OVF_CNT_RST_CH0_EN_V  0x00000001U
#define LEDC_TASK_OVF_CNT_RST_CH0_EN_S  16
/** LEDC_TASK_OVF_CNT_RST_CH1_EN : R/W; bitpos: [17]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_OVF_CNT_RST_CH1 task.\\0: Disable\\1:
 *  Enable
 */
#define LEDC_TASK_OVF_CNT_RST_CH1_EN    (BIT(17))
#define LEDC_TASK_OVF_CNT_RST_CH1_EN_M  (LEDC_TASK_OVF_CNT_RST_CH1_EN_V << LEDC_TASK_OVF_CNT_RST_CH1_EN_S)
#define LEDC_TASK_OVF_CNT_RST_CH1_EN_V  0x00000001U
#define LEDC_TASK_OVF_CNT_RST_CH1_EN_S  17
/** LEDC_TASK_OVF_CNT_RST_CH2_EN : R/W; bitpos: [18]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_OVF_CNT_RST_CH2 task.\\0: Disable\\1:
 *  Enable
 */
#define LEDC_TASK_OVF_CNT_RST_CH2_EN    (BIT(18))
#define LEDC_TASK_OVF_CNT_RST_CH2_EN_M  (LEDC_TASK_OVF_CNT_RST_CH2_EN_V << LEDC_TASK_OVF_CNT_RST_CH2_EN_S)
#define LEDC_TASK_OVF_CNT_RST_CH2_EN_V  0x00000001U
#define LEDC_TASK_OVF_CNT_RST_CH2_EN_S  18
/** LEDC_TASK_OVF_CNT_RST_CH3_EN : R/W; bitpos: [19]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_OVF_CNT_RST_CH3 task.\\0: Disable\\1:
 *  Enable
 */
#define LEDC_TASK_OVF_CNT_RST_CH3_EN    (BIT(19))
#define LEDC_TASK_OVF_CNT_RST_CH3_EN_M  (LEDC_TASK_OVF_CNT_RST_CH3_EN_V << LEDC_TASK_OVF_CNT_RST_CH3_EN_S)
#define LEDC_TASK_OVF_CNT_RST_CH3_EN_V  0x00000001U
#define LEDC_TASK_OVF_CNT_RST_CH3_EN_S  19
/** LEDC_TASK_OVF_CNT_RST_CH4_EN : R/W; bitpos: [20]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_OVF_CNT_RST_CH4 task.\\0: Disable\\1:
 *  Enable
 */
#define LEDC_TASK_OVF_CNT_RST_CH4_EN    (BIT(20))
#define LEDC_TASK_OVF_CNT_RST_CH4_EN_M  (LEDC_TASK_OVF_CNT_RST_CH4_EN_V << LEDC_TASK_OVF_CNT_RST_CH4_EN_S)
#define LEDC_TASK_OVF_CNT_RST_CH4_EN_V  0x00000001U
#define LEDC_TASK_OVF_CNT_RST_CH4_EN_S  20
/** LEDC_TASK_OVF_CNT_RST_CH5_EN : R/W; bitpos: [21]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_OVF_CNT_RST_CH5 task.\\0: Disable\\1:
 *  Enable
 */
#define LEDC_TASK_OVF_CNT_RST_CH5_EN    (BIT(21))
#define LEDC_TASK_OVF_CNT_RST_CH5_EN_M  (LEDC_TASK_OVF_CNT_RST_CH5_EN_V << LEDC_TASK_OVF_CNT_RST_CH5_EN_S)
#define LEDC_TASK_OVF_CNT_RST_CH5_EN_V  0x00000001U
#define LEDC_TASK_OVF_CNT_RST_CH5_EN_S  21
/** LEDC_TASK_TIMER0_RST_EN : R/W; bitpos: [24]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER0_RST task.\\0: Disable\\1:
 *  Enable
 */
#define LEDC_TASK_TIMER0_RST_EN    (BIT(24))
#define LEDC_TASK_TIMER0_RST_EN_M  (LEDC_TASK_TIMER0_RST_EN_V << LEDC_TASK_TIMER0_RST_EN_S)
#define LEDC_TASK_TIMER0_RST_EN_V  0x00000001U
#define LEDC_TASK_TIMER0_RST_EN_S  24
/** LEDC_TASK_TIMER1_RST_EN : R/W; bitpos: [25]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER1_RST task.\\0: Disable\\1:
 *  Enable
 */
#define LEDC_TASK_TIMER1_RST_EN    (BIT(25))
#define LEDC_TASK_TIMER1_RST_EN_M  (LEDC_TASK_TIMER1_RST_EN_V << LEDC_TASK_TIMER1_RST_EN_S)
#define LEDC_TASK_TIMER1_RST_EN_V  0x00000001U
#define LEDC_TASK_TIMER1_RST_EN_S  25
/** LEDC_TASK_TIMER2_RST_EN : R/W; bitpos: [26]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER2_RST task.\\0: Disable\\1:
 *  Enable
 */
#define LEDC_TASK_TIMER2_RST_EN    (BIT(26))
#define LEDC_TASK_TIMER2_RST_EN_M  (LEDC_TASK_TIMER2_RST_EN_V << LEDC_TASK_TIMER2_RST_EN_S)
#define LEDC_TASK_TIMER2_RST_EN_V  0x00000001U
#define LEDC_TASK_TIMER2_RST_EN_S  26
/** LEDC_TASK_TIMER3_RST_EN : R/W; bitpos: [27]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER3_RST task.\\0: Disable\\1:
 *  Enable
 */
#define LEDC_TASK_TIMER3_RST_EN    (BIT(27))
#define LEDC_TASK_TIMER3_RST_EN_M  (LEDC_TASK_TIMER3_RST_EN_V << LEDC_TASK_TIMER3_RST_EN_S)
#define LEDC_TASK_TIMER3_RST_EN_V  0x00000001U
#define LEDC_TASK_TIMER3_RST_EN_S  27
/** LEDC_TASK_TIMER0_PAUSE_RESUME_EN : R/W; bitpos: [28]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER0_PAUSE and LEDC_TASK_TIMER0
 *  _RESUME task.\\0: Disable\\1: Enable
 */
#define LEDC_TASK_TIMER0_PAUSE_RESUME_EN    (BIT(28))
#define LEDC_TASK_TIMER0_PAUSE_RESUME_EN_M  (LEDC_TASK_TIMER0_PAUSE_RESUME_EN_V << LEDC_TASK_TIMER0_PAUSE_RESUME_EN_S)
#define LEDC_TASK_TIMER0_PAUSE_RESUME_EN_V  0x00000001U
#define LEDC_TASK_TIMER0_PAUSE_RESUME_EN_S  28
/** LEDC_TASK_TIMER1_PAUSE_RESUME_EN : R/W; bitpos: [29]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER1_PAUSE and LEDC_TASK_TIMER1
 *  _RESUME task.\\0: Disable\\1: Enable
 */
#define LEDC_TASK_TIMER1_PAUSE_RESUME_EN    (BIT(29))
#define LEDC_TASK_TIMER1_PAUSE_RESUME_EN_M  (LEDC_TASK_TIMER1_PAUSE_RESUME_EN_V << LEDC_TASK_TIMER1_PAUSE_RESUME_EN_S)
#define LEDC_TASK_TIMER1_PAUSE_RESUME_EN_V  0x00000001U
#define LEDC_TASK_TIMER1_PAUSE_RESUME_EN_S  29
/** LEDC_TASK_TIMER2_PAUSE_RESUME_EN : R/W; bitpos: [30]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER2_PAUSE and LEDC_TASK_TIMER2
 *  _RESUME task.\\0: Disable\\1: Enable
 */
#define LEDC_TASK_TIMER2_PAUSE_RESUME_EN    (BIT(30))
#define LEDC_TASK_TIMER2_PAUSE_RESUME_EN_M  (LEDC_TASK_TIMER2_PAUSE_RESUME_EN_V << LEDC_TASK_TIMER2_PAUSE_RESUME_EN_S)
#define LEDC_TASK_TIMER2_PAUSE_RESUME_EN_V  0x00000001U
#define LEDC_TASK_TIMER2_PAUSE_RESUME_EN_S  30
/** LEDC_TASK_TIMER3_PAUSE_RESUME_EN : R/W; bitpos: [31]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER3_PAUSE and LEDC_TASK_TIMER3
 *  _RESUME task.\\0: Disable\\1: Enable
 */
#define LEDC_TASK_TIMER3_PAUSE_RESUME_EN    (BIT(31))
#define LEDC_TASK_TIMER3_PAUSE_RESUME_EN_M  (LEDC_TASK_TIMER3_PAUSE_RESUME_EN_V << LEDC_TASK_TIMER3_PAUSE_RESUME_EN_S)
#define LEDC_TASK_TIMER3_PAUSE_RESUME_EN_V  0x00000001U
#define LEDC_TASK_TIMER3_PAUSE_RESUME_EN_S  31

/** LEDC_EVT_TASK_EN2_REG register
 *  Ledc event task enable bit register2.
 */
#define LEDC_EVT_TASK_EN2_REG (DR_REG_LEDC_BASE + 0x128)
/** LEDC_TASK_GAMMA_RESTART_CH0_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_GAMMA_RESTART_CH0 task.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_TASK_GAMMA_RESTART_CH0_EN    (BIT(0))
#define LEDC_TASK_GAMMA_RESTART_CH0_EN_M  (LEDC_TASK_GAMMA_RESTART_CH0_EN_V << LEDC_TASK_GAMMA_RESTART_CH0_EN_S)
#define LEDC_TASK_GAMMA_RESTART_CH0_EN_V  0x00000001U
#define LEDC_TASK_GAMMA_RESTART_CH0_EN_S  0
/** LEDC_TASK_GAMMA_RESTART_CH1_EN : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_GAMMA_RESTART_CH1 task.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_TASK_GAMMA_RESTART_CH1_EN    (BIT(1))
#define LEDC_TASK_GAMMA_RESTART_CH1_EN_M  (LEDC_TASK_GAMMA_RESTART_CH1_EN_V << LEDC_TASK_GAMMA_RESTART_CH1_EN_S)
#define LEDC_TASK_GAMMA_RESTART_CH1_EN_V  0x00000001U
#define LEDC_TASK_GAMMA_RESTART_CH1_EN_S  1
/** LEDC_TASK_GAMMA_RESTART_CH2_EN : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_GAMMA_RESTART_CH2 task.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_TASK_GAMMA_RESTART_CH2_EN    (BIT(2))
#define LEDC_TASK_GAMMA_RESTART_CH2_EN_M  (LEDC_TASK_GAMMA_RESTART_CH2_EN_V << LEDC_TASK_GAMMA_RESTART_CH2_EN_S)
#define LEDC_TASK_GAMMA_RESTART_CH2_EN_V  0x00000001U
#define LEDC_TASK_GAMMA_RESTART_CH2_EN_S  2
/** LEDC_TASK_GAMMA_RESTART_CH3_EN : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_GAMMA_RESTART_CH3 task.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_TASK_GAMMA_RESTART_CH3_EN    (BIT(3))
#define LEDC_TASK_GAMMA_RESTART_CH3_EN_M  (LEDC_TASK_GAMMA_RESTART_CH3_EN_V << LEDC_TASK_GAMMA_RESTART_CH3_EN_S)
#define LEDC_TASK_GAMMA_RESTART_CH3_EN_V  0x00000001U
#define LEDC_TASK_GAMMA_RESTART_CH3_EN_S  3
/** LEDC_TASK_GAMMA_RESTART_CH4_EN : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_GAMMA_RESTART_CH4 task.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_TASK_GAMMA_RESTART_CH4_EN    (BIT(4))
#define LEDC_TASK_GAMMA_RESTART_CH4_EN_M  (LEDC_TASK_GAMMA_RESTART_CH4_EN_V << LEDC_TASK_GAMMA_RESTART_CH4_EN_S)
#define LEDC_TASK_GAMMA_RESTART_CH4_EN_V  0x00000001U
#define LEDC_TASK_GAMMA_RESTART_CH4_EN_S  4
/** LEDC_TASK_GAMMA_RESTART_CH5_EN : R/W; bitpos: [5]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_GAMMA_RESTART_CH5 task.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_TASK_GAMMA_RESTART_CH5_EN    (BIT(5))
#define LEDC_TASK_GAMMA_RESTART_CH5_EN_M  (LEDC_TASK_GAMMA_RESTART_CH5_EN_V << LEDC_TASK_GAMMA_RESTART_CH5_EN_S)
#define LEDC_TASK_GAMMA_RESTART_CH5_EN_V  0x00000001U
#define LEDC_TASK_GAMMA_RESTART_CH5_EN_S  5
/** LEDC_TASK_GAMMA_PAUSE_CH0_EN : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_GAMMA_PAUSE_CH0 task.\\0: Disable\\1:
 *  Enable
 */
#define LEDC_TASK_GAMMA_PAUSE_CH0_EN    (BIT(8))
#define LEDC_TASK_GAMMA_PAUSE_CH0_EN_M  (LEDC_TASK_GAMMA_PAUSE_CH0_EN_V << LEDC_TASK_GAMMA_PAUSE_CH0_EN_S)
#define LEDC_TASK_GAMMA_PAUSE_CH0_EN_V  0x00000001U
#define LEDC_TASK_GAMMA_PAUSE_CH0_EN_S  8
/** LEDC_TASK_GAMMA_PAUSE_CH1_EN : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_GAMMA_PAUSE_CH1 task.\\0: Disable\\1:
 *  Enable
 */
#define LEDC_TASK_GAMMA_PAUSE_CH1_EN    (BIT(9))
#define LEDC_TASK_GAMMA_PAUSE_CH1_EN_M  (LEDC_TASK_GAMMA_PAUSE_CH1_EN_V << LEDC_TASK_GAMMA_PAUSE_CH1_EN_S)
#define LEDC_TASK_GAMMA_PAUSE_CH1_EN_V  0x00000001U
#define LEDC_TASK_GAMMA_PAUSE_CH1_EN_S  9
/** LEDC_TASK_GAMMA_PAUSE_CH2_EN : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_GAMMA_PAUSE_CH2 task.\\0: Disable\\1:
 *  Enable
 */
#define LEDC_TASK_GAMMA_PAUSE_CH2_EN    (BIT(10))
#define LEDC_TASK_GAMMA_PAUSE_CH2_EN_M  (LEDC_TASK_GAMMA_PAUSE_CH2_EN_V << LEDC_TASK_GAMMA_PAUSE_CH2_EN_S)
#define LEDC_TASK_GAMMA_PAUSE_CH2_EN_V  0x00000001U
#define LEDC_TASK_GAMMA_PAUSE_CH2_EN_S  10
/** LEDC_TASK_GAMMA_PAUSE_CH3_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_GAMMA_PAUSE_CH3 task.\\0: Disable\\1:
 *  Enable
 */
#define LEDC_TASK_GAMMA_PAUSE_CH3_EN    (BIT(11))
#define LEDC_TASK_GAMMA_PAUSE_CH3_EN_M  (LEDC_TASK_GAMMA_PAUSE_CH3_EN_V << LEDC_TASK_GAMMA_PAUSE_CH3_EN_S)
#define LEDC_TASK_GAMMA_PAUSE_CH3_EN_V  0x00000001U
#define LEDC_TASK_GAMMA_PAUSE_CH3_EN_S  11
/** LEDC_TASK_GAMMA_PAUSE_CH4_EN : R/W; bitpos: [12]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_GAMMA_PAUSE_CH4 task.\\0: Disable\\1:
 *  Enable
 */
#define LEDC_TASK_GAMMA_PAUSE_CH4_EN    (BIT(12))
#define LEDC_TASK_GAMMA_PAUSE_CH4_EN_M  (LEDC_TASK_GAMMA_PAUSE_CH4_EN_V << LEDC_TASK_GAMMA_PAUSE_CH4_EN_S)
#define LEDC_TASK_GAMMA_PAUSE_CH4_EN_V  0x00000001U
#define LEDC_TASK_GAMMA_PAUSE_CH4_EN_S  12
/** LEDC_TASK_GAMMA_PAUSE_CH5_EN : R/W; bitpos: [13]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_GAMMA_PAUSE_CH5 task.\\0: Disable\\1:
 *  Enable
 */
#define LEDC_TASK_GAMMA_PAUSE_CH5_EN    (BIT(13))
#define LEDC_TASK_GAMMA_PAUSE_CH5_EN_M  (LEDC_TASK_GAMMA_PAUSE_CH5_EN_V << LEDC_TASK_GAMMA_PAUSE_CH5_EN_S)
#define LEDC_TASK_GAMMA_PAUSE_CH5_EN_V  0x00000001U
#define LEDC_TASK_GAMMA_PAUSE_CH5_EN_S  13
/** LEDC_TASK_GAMMA_RESUME_CH0_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_GAMMA_RESUME_CH0 task.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_TASK_GAMMA_RESUME_CH0_EN    (BIT(16))
#define LEDC_TASK_GAMMA_RESUME_CH0_EN_M  (LEDC_TASK_GAMMA_RESUME_CH0_EN_V << LEDC_TASK_GAMMA_RESUME_CH0_EN_S)
#define LEDC_TASK_GAMMA_RESUME_CH0_EN_V  0x00000001U
#define LEDC_TASK_GAMMA_RESUME_CH0_EN_S  16
/** LEDC_TASK_GAMMA_RESUME_CH1_EN : R/W; bitpos: [17]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_GAMMA_RESUME_CH1 task.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_TASK_GAMMA_RESUME_CH1_EN    (BIT(17))
#define LEDC_TASK_GAMMA_RESUME_CH1_EN_M  (LEDC_TASK_GAMMA_RESUME_CH1_EN_V << LEDC_TASK_GAMMA_RESUME_CH1_EN_S)
#define LEDC_TASK_GAMMA_RESUME_CH1_EN_V  0x00000001U
#define LEDC_TASK_GAMMA_RESUME_CH1_EN_S  17
/** LEDC_TASK_GAMMA_RESUME_CH2_EN : R/W; bitpos: [18]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_GAMMA_RESUME_CH2 task.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_TASK_GAMMA_RESUME_CH2_EN    (BIT(18))
#define LEDC_TASK_GAMMA_RESUME_CH2_EN_M  (LEDC_TASK_GAMMA_RESUME_CH2_EN_V << LEDC_TASK_GAMMA_RESUME_CH2_EN_S)
#define LEDC_TASK_GAMMA_RESUME_CH2_EN_V  0x00000001U
#define LEDC_TASK_GAMMA_RESUME_CH2_EN_S  18
/** LEDC_TASK_GAMMA_RESUME_CH3_EN : R/W; bitpos: [19]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_GAMMA_RESUME_CH3 task.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_TASK_GAMMA_RESUME_CH3_EN    (BIT(19))
#define LEDC_TASK_GAMMA_RESUME_CH3_EN_M  (LEDC_TASK_GAMMA_RESUME_CH3_EN_V << LEDC_TASK_GAMMA_RESUME_CH3_EN_S)
#define LEDC_TASK_GAMMA_RESUME_CH3_EN_V  0x00000001U
#define LEDC_TASK_GAMMA_RESUME_CH3_EN_S  19
/** LEDC_TASK_GAMMA_RESUME_CH4_EN : R/W; bitpos: [20]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_GAMMA_RESUME_CH4 task.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_TASK_GAMMA_RESUME_CH4_EN    (BIT(20))
#define LEDC_TASK_GAMMA_RESUME_CH4_EN_M  (LEDC_TASK_GAMMA_RESUME_CH4_EN_V << LEDC_TASK_GAMMA_RESUME_CH4_EN_S)
#define LEDC_TASK_GAMMA_RESUME_CH4_EN_V  0x00000001U
#define LEDC_TASK_GAMMA_RESUME_CH4_EN_S  20
/** LEDC_TASK_GAMMA_RESUME_CH5_EN : R/W; bitpos: [21]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_GAMMA_RESUME_CH5 task.\\0:
 *  Disable\\1: Enable
 */
#define LEDC_TASK_GAMMA_RESUME_CH5_EN    (BIT(21))
#define LEDC_TASK_GAMMA_RESUME_CH5_EN_M  (LEDC_TASK_GAMMA_RESUME_CH5_EN_V << LEDC_TASK_GAMMA_RESUME_CH5_EN_S)
#define LEDC_TASK_GAMMA_RESUME_CH5_EN_V  0x00000001U
#define LEDC_TASK_GAMMA_RESUME_CH5_EN_S  21

/** LEDC_TIMER0_CMP_REG register
 *  Ledc timer0 compare value register.
 */
#define LEDC_TIMER0_CMP_REG (DR_REG_LEDC_BASE + 0x140)
/** LEDC_TIMER0_CMP : R/W; bitpos: [19:0]; default: 0;
 *  Configures the comparison value for LEDC timer0.
 */
#define LEDC_TIMER0_CMP    0x000FFFFFU
#define LEDC_TIMER0_CMP_M  (LEDC_TIMER0_CMP_V << LEDC_TIMER0_CMP_S)
#define LEDC_TIMER0_CMP_V  0x000FFFFFU
#define LEDC_TIMER0_CMP_S  0

/** LEDC_TIMER1_CMP_REG register
 *  Ledc timer1 compare value register.
 */
#define LEDC_TIMER1_CMP_REG (DR_REG_LEDC_BASE + 0x144)
/** LEDC_TIMER1_CMP : R/W; bitpos: [19:0]; default: 0;
 *  Configures the comparison value for LEDC timer1.
 */
#define LEDC_TIMER1_CMP    0x000FFFFFU
#define LEDC_TIMER1_CMP_M  (LEDC_TIMER1_CMP_V << LEDC_TIMER1_CMP_S)
#define LEDC_TIMER1_CMP_V  0x000FFFFFU
#define LEDC_TIMER1_CMP_S  0

/** LEDC_TIMER2_CMP_REG register
 *  Ledc timer2 compare value register.
 */
#define LEDC_TIMER2_CMP_REG (DR_REG_LEDC_BASE + 0x148)
/** LEDC_TIMER2_CMP : R/W; bitpos: [19:0]; default: 0;
 *  Configures the comparison value for LEDC timer2.
 */
#define LEDC_TIMER2_CMP    0x000FFFFFU
#define LEDC_TIMER2_CMP_M  (LEDC_TIMER2_CMP_V << LEDC_TIMER2_CMP_S)
#define LEDC_TIMER2_CMP_V  0x000FFFFFU
#define LEDC_TIMER2_CMP_S  0

/** LEDC_TIMER3_CMP_REG register
 *  Ledc timer3 compare value register.
 */
#define LEDC_TIMER3_CMP_REG (DR_REG_LEDC_BASE + 0x14c)
/** LEDC_TIMER3_CMP : R/W; bitpos: [19:0]; default: 0;
 *  Configures the comparison value for LEDC timer3.
 */
#define LEDC_TIMER3_CMP    0x000FFFFFU
#define LEDC_TIMER3_CMP_M  (LEDC_TIMER3_CMP_V << LEDC_TIMER3_CMP_S)
#define LEDC_TIMER3_CMP_V  0x000FFFFFU
#define LEDC_TIMER3_CMP_S  0

/** LEDC_TIMER0_CNT_CAP_REG register
 *  Ledc timer0 captured count value register.
 */
#define LEDC_TIMER0_CNT_CAP_REG (DR_REG_LEDC_BASE + 0x150)
/** LEDC_TIMER0_CNT_CAP : RO; bitpos: [19:0]; default: 0;
 *  Represents the captured LEDC timer0 count value.
 */
#define LEDC_TIMER0_CNT_CAP    0x000FFFFFU
#define LEDC_TIMER0_CNT_CAP_M  (LEDC_TIMER0_CNT_CAP_V << LEDC_TIMER0_CNT_CAP_S)
#define LEDC_TIMER0_CNT_CAP_V  0x000FFFFFU
#define LEDC_TIMER0_CNT_CAP_S  0

/** LEDC_TIMER1_CNT_CAP_REG register
 *  Ledc timer1 captured count value register.
 */
#define LEDC_TIMER1_CNT_CAP_REG (DR_REG_LEDC_BASE + 0x154)
/** LEDC_TIMER1_CNT_CAP : RO; bitpos: [19:0]; default: 0;
 *  Represents the captured LEDC timer1 count value.
 */
#define LEDC_TIMER1_CNT_CAP    0x000FFFFFU
#define LEDC_TIMER1_CNT_CAP_M  (LEDC_TIMER1_CNT_CAP_V << LEDC_TIMER1_CNT_CAP_S)
#define LEDC_TIMER1_CNT_CAP_V  0x000FFFFFU
#define LEDC_TIMER1_CNT_CAP_S  0

/** LEDC_TIMER2_CNT_CAP_REG register
 *  Ledc timer2 captured count value register.
 */
#define LEDC_TIMER2_CNT_CAP_REG (DR_REG_LEDC_BASE + 0x158)
/** LEDC_TIMER2_CNT_CAP : RO; bitpos: [19:0]; default: 0;
 *  Represents the captured LEDC timer2 count value.
 */
#define LEDC_TIMER2_CNT_CAP    0x000FFFFFU
#define LEDC_TIMER2_CNT_CAP_M  (LEDC_TIMER2_CNT_CAP_V << LEDC_TIMER2_CNT_CAP_S)
#define LEDC_TIMER2_CNT_CAP_V  0x000FFFFFU
#define LEDC_TIMER2_CNT_CAP_S  0

/** LEDC_TIMER3_CNT_CAP_REG register
 *  Ledc timer3 captured count value register.
 */
#define LEDC_TIMER3_CNT_CAP_REG (DR_REG_LEDC_BASE + 0x15c)
/** LEDC_TIMER3_CNT_CAP : RO; bitpos: [19:0]; default: 0;
 *  Represents the captured LEDC timer3 count value.
 */
#define LEDC_TIMER3_CNT_CAP    0x000FFFFFU
#define LEDC_TIMER3_CNT_CAP_M  (LEDC_TIMER3_CNT_CAP_V << LEDC_TIMER3_CNT_CAP_S)
#define LEDC_TIMER3_CNT_CAP_V  0x000FFFFFU
#define LEDC_TIMER3_CNT_CAP_S  0

/** LEDC_CONF_REG register
 *  LEDC global configuration register
 */
#define LEDC_CONF_REG (DR_REG_LEDC_BASE + 0x170)
/** LEDC_GAMMA_RAM_CLK_EN_CH0 : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to open LEDC ch0 gamma ram clock gate.\\0: Open the clock
 *  gate only when application writes or reads LEDC ch0 gamma ram\\1: Force open the
 *  clock gate for LEDC ch0 gamma ram
 */
#define LEDC_GAMMA_RAM_CLK_EN_CH0    (BIT(2))
#define LEDC_GAMMA_RAM_CLK_EN_CH0_M  (LEDC_GAMMA_RAM_CLK_EN_CH0_V << LEDC_GAMMA_RAM_CLK_EN_CH0_S)
#define LEDC_GAMMA_RAM_CLK_EN_CH0_V  0x00000001U
#define LEDC_GAMMA_RAM_CLK_EN_CH0_S  2
/** LEDC_GAMMA_RAM_CLK_EN_CH1 : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to open LEDC ch1 gamma ram clock gate.\\0: Open the clock
 *  gate only when application writes or reads LEDC ch1 gamma ram\\1: Force open the
 *  clock gate for LEDC ch1 gamma ram
 */
#define LEDC_GAMMA_RAM_CLK_EN_CH1    (BIT(3))
#define LEDC_GAMMA_RAM_CLK_EN_CH1_M  (LEDC_GAMMA_RAM_CLK_EN_CH1_V << LEDC_GAMMA_RAM_CLK_EN_CH1_S)
#define LEDC_GAMMA_RAM_CLK_EN_CH1_V  0x00000001U
#define LEDC_GAMMA_RAM_CLK_EN_CH1_S  3
/** LEDC_GAMMA_RAM_CLK_EN_CH2 : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to open LEDC ch2 gamma ram clock gate.\\0: Open the clock
 *  gate only when application writes or reads LEDC ch2 gamma ram\\1: Force open the
 *  clock gate for LEDC ch2 gamma ram
 */
#define LEDC_GAMMA_RAM_CLK_EN_CH2    (BIT(4))
#define LEDC_GAMMA_RAM_CLK_EN_CH2_M  (LEDC_GAMMA_RAM_CLK_EN_CH2_V << LEDC_GAMMA_RAM_CLK_EN_CH2_S)
#define LEDC_GAMMA_RAM_CLK_EN_CH2_V  0x00000001U
#define LEDC_GAMMA_RAM_CLK_EN_CH2_S  4
/** LEDC_GAMMA_RAM_CLK_EN_CH3 : R/W; bitpos: [5]; default: 0;
 *  Configures whether or not to open LEDC ch3 gamma ram clock gate.\\0: Open the clock
 *  gate only when application writes or reads LEDC ch3 gamma ram\\1: Force open the
 *  clock gate for LEDC ch3 gamma ram
 */
#define LEDC_GAMMA_RAM_CLK_EN_CH3    (BIT(5))
#define LEDC_GAMMA_RAM_CLK_EN_CH3_M  (LEDC_GAMMA_RAM_CLK_EN_CH3_V << LEDC_GAMMA_RAM_CLK_EN_CH3_S)
#define LEDC_GAMMA_RAM_CLK_EN_CH3_V  0x00000001U
#define LEDC_GAMMA_RAM_CLK_EN_CH3_S  5
/** LEDC_GAMMA_RAM_CLK_EN_CH4 : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to open LEDC ch4 gamma ram clock gate.\\0: Open the clock
 *  gate only when application writes or reads LEDC ch4 gamma ram\\1: Force open the
 *  clock gate for LEDC ch4 gamma ram
 */
#define LEDC_GAMMA_RAM_CLK_EN_CH4    (BIT(6))
#define LEDC_GAMMA_RAM_CLK_EN_CH4_M  (LEDC_GAMMA_RAM_CLK_EN_CH4_V << LEDC_GAMMA_RAM_CLK_EN_CH4_S)
#define LEDC_GAMMA_RAM_CLK_EN_CH4_V  0x00000001U
#define LEDC_GAMMA_RAM_CLK_EN_CH4_S  6
/** LEDC_GAMMA_RAM_CLK_EN_CH5 : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to open LEDC ch5 gamma ram clock gate.\\0: Open the clock
 *  gate only when application writes or reads LEDC ch5 gamma ram\\1: Force open the
 *  clock gate for LEDC ch5 gamma ram
 */
#define LEDC_GAMMA_RAM_CLK_EN_CH5    (BIT(7))
#define LEDC_GAMMA_RAM_CLK_EN_CH5_M  (LEDC_GAMMA_RAM_CLK_EN_CH5_V << LEDC_GAMMA_RAM_CLK_EN_CH5_S)
#define LEDC_GAMMA_RAM_CLK_EN_CH5_V  0x00000001U
#define LEDC_GAMMA_RAM_CLK_EN_CH5_S  7
/** LEDC_CLK_EN : R/W; bitpos: [31]; default: 0;
 *  Configures whether or not to open register clock gate.\\0: Open the clock gate only
 *  when application writes registers\\1: Force open the clock gate for register
 */
#define LEDC_CLK_EN    (BIT(31))
#define LEDC_CLK_EN_M  (LEDC_CLK_EN_V << LEDC_CLK_EN_S)
#define LEDC_CLK_EN_V  0x00000001U
#define LEDC_CLK_EN_S  31

/** LEDC_DATE_REG register
 *  Version control register
 */
#define LEDC_DATE_REG (DR_REG_LEDC_BASE + 0x174)
/** LEDC_LEDC_DATE : R/W; bitpos: [27:0]; default: 36770336;
 *  Configures the version.
 */
#define LEDC_LEDC_DATE    0x0FFFFFFFU
#define LEDC_LEDC_DATE_M  (LEDC_LEDC_DATE_V << LEDC_LEDC_DATE_S)
#define LEDC_LEDC_DATE_V  0x0FFFFFFFU
#define LEDC_LEDC_DATE_S  0

/** LEDC gamma fade config ram registers
 *  16 words (32bit) per channel * 6 channels
 */
#define LEDC_CH0_GAMMA_RANGE0_REG  (DR_REG_LEDC_BASE + 0x400)
/* LEDC_CH0_GAMMA_RANGE0_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE0_DUTY_NUM    0x000003FF
#define LEDC_CH0_GAMMA_RANGE0_DUTY_NUM_M  ((LEDC_CH0_GAMMA_RANGE0_DUTY_NUM_V)<<(LEDC_CH0_GAMMA_RANGE0_DUTY_NUM_S))
#define LEDC_CH0_GAMMA_RANGE0_DUTY_NUM_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE0_DUTY_NUM_S  21
/* LEDC_CH0_GAMMA_RANGE0_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE0_SCALE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE0_SCALE_M  ((LEDC_CH0_GAMMA_RANGE0_SCALE_V)<<(LEDC_CH0_GAMMA_RANGE0_SCALE_S))
#define LEDC_CH0_GAMMA_RANGE0_SCALE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE0_SCALE_S  11
/* LEDC_CH0_GAMMA_RANGE0_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE0_DUTY_CYCLE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE0_DUTY_CYCLE_M  ((LEDC_CH0_GAMMA_RANGE0_DUTY_CYCLE_V)<<(LEDC_CH0_GAMMA_RANGE0_DUTY_CYCLE_S))
#define LEDC_CH0_GAMMA_RANGE0_DUTY_CYCLE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE0_DUTY_CYCLE_S  1
/* LEDC_CH0_GAMMA_RANGE0_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH0_GAMMA_RANGE0_DUTY_INC    (BIT(0))
#define LEDC_CH0_GAMMA_RANGE0_DUTY_INC_M  (BIT(0))
#define LEDC_CH0_GAMMA_RANGE0_DUTY_INC_V  0x1
#define LEDC_CH0_GAMMA_RANGE0_DUTY_INC_S  0

#define LEDC_CH0_GAMMA_RANGE1_REG  (DR_REG_LEDC_BASE + 0x404)
/* LEDC_CH0_GAMMA_RANGE1_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE1_DUTY_NUM    0x000003FF
#define LEDC_CH0_GAMMA_RANGE1_DUTY_NUM_M  ((LEDC_CH0_GAMMA_RANGE1_DUTY_NUM_V)<<(LEDC_CH0_GAMMA_RANGE1_DUTY_NUM_S))
#define LEDC_CH0_GAMMA_RANGE1_DUTY_NUM_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE1_DUTY_NUM_S  21
/* LEDC_CH0_GAMMA_RANGE1_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE1_SCALE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE1_SCALE_M  ((LEDC_CH0_GAMMA_RANGE1_SCALE_V)<<(LEDC_CH0_GAMMA_RANGE1_SCALE_S))
#define LEDC_CH0_GAMMA_RANGE1_SCALE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE1_SCALE_S  11
/* LEDC_CH0_GAMMA_RANGE1_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE1_DUTY_CYCLE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE1_DUTY_CYCLE_M  ((LEDC_CH0_GAMMA_RANGE1_DUTY_CYCLE_V)<<(LEDC_CH0_GAMMA_RANGE1_DUTY_CYCLE_S))
#define LEDC_CH0_GAMMA_RANGE1_DUTY_CYCLE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE1_DUTY_CYCLE_S  1
/* LEDC_CH0_GAMMA_RANGE1_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH0_GAMMA_RANGE1_DUTY_INC    (BIT(0))
#define LEDC_CH0_GAMMA_RANGE1_DUTY_INC_M  (BIT(0))
#define LEDC_CH0_GAMMA_RANGE1_DUTY_INC_V  0x1
#define LEDC_CH0_GAMMA_RANGE1_DUTY_INC_S  0

#define LEDC_CH0_GAMMA_RANGE2_REG  (DR_REG_LEDC_BASE + 0x408)
/* LEDC_CH0_GAMMA_RANGE2_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE2_DUTY_NUM    0x000003FF
#define LEDC_CH0_GAMMA_RANGE2_DUTY_NUM_M  ((LEDC_CH0_GAMMA_RANGE2_DUTY_NUM_V)<<(LEDC_CH0_GAMMA_RANGE2_DUTY_NUM_S))
#define LEDC_CH0_GAMMA_RANGE2_DUTY_NUM_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE2_DUTY_NUM_S  21
/* LEDC_CH0_GAMMA_RANGE2_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE2_SCALE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE2_SCALE_M  ((LEDC_CH0_GAMMA_RANGE2_SCALE_V)<<(LEDC_CH0_GAMMA_RANGE2_SCALE_S))
#define LEDC_CH0_GAMMA_RANGE2_SCALE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE2_SCALE_S  11
/* LEDC_CH0_GAMMA_RANGE2_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE2_DUTY_CYCLE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE2_DUTY_CYCLE_M  ((LEDC_CH0_GAMMA_RANGE2_DUTY_CYCLE_V)<<(LEDC_CH0_GAMMA_RANGE2_DUTY_CYCLE_S))
#define LEDC_CH0_GAMMA_RANGE2_DUTY_CYCLE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE2_DUTY_CYCLE_S  1
/* LEDC_CH0_GAMMA_RANGE2_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH0_GAMMA_RANGE2_DUTY_INC    (BIT(0))
#define LEDC_CH0_GAMMA_RANGE2_DUTY_INC_M  (BIT(0))
#define LEDC_CH0_GAMMA_RANGE2_DUTY_INC_V  0x1
#define LEDC_CH0_GAMMA_RANGE2_DUTY_INC_S  0

#define LEDC_CH0_GAMMA_RANGE3_REG  (DR_REG_LEDC_BASE + 0x40c)
/* LEDC_CH0_GAMMA_RANGE3_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE3_DUTY_NUM    0x000003FF
#define LEDC_CH0_GAMMA_RANGE3_DUTY_NUM_M  ((LEDC_CH0_GAMMA_RANGE3_DUTY_NUM_V)<<(LEDC_CH0_GAMMA_RANGE3_DUTY_NUM_S))
#define LEDC_CH0_GAMMA_RANGE3_DUTY_NUM_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE3_DUTY_NUM_S  21
/* LEDC_CH0_GAMMA_RANGE3_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE3_SCALE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE3_SCALE_M  ((LEDC_CH0_GAMMA_RANGE3_SCALE_V)<<(LEDC_CH0_GAMMA_RANGE3_SCALE_S))
#define LEDC_CH0_GAMMA_RANGE3_SCALE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE3_SCALE_S  11
/* LEDC_CH0_GAMMA_RANGE3_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE3_DUTY_CYCLE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE3_DUTY_CYCLE_M  ((LEDC_CH0_GAMMA_RANGE3_DUTY_CYCLE_V)<<(LEDC_CH0_GAMMA_RANGE3_DUTY_CYCLE_S))
#define LEDC_CH0_GAMMA_RANGE3_DUTY_CYCLE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE3_DUTY_CYCLE_S  1
/* LEDC_CH0_GAMMA_RANGE3_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH0_GAMMA_RANGE3_DUTY_INC    (BIT(0))
#define LEDC_CH0_GAMMA_RANGE3_DUTY_INC_M  (BIT(0))
#define LEDC_CH0_GAMMA_RANGE3_DUTY_INC_V  0x1
#define LEDC_CH0_GAMMA_RANGE3_DUTY_INC_S  0

#define LEDC_CH0_GAMMA_RANGE4_REG  (DR_REG_LEDC_BASE + 0x410)
/* LEDC_CH0_GAMMA_RANGE4_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE4_DUTY_NUM    0x000003FF
#define LEDC_CH0_GAMMA_RANGE4_DUTY_NUM_M  ((LEDC_CH0_GAMMA_RANGE4_DUTY_NUM_V)<<(LEDC_CH0_GAMMA_RANGE4_DUTY_NUM_S))
#define LEDC_CH0_GAMMA_RANGE4_DUTY_NUM_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE4_DUTY_NUM_S  21
/* LEDC_CH0_GAMMA_RANGE4_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE4_SCALE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE4_SCALE_M  ((LEDC_CH0_GAMMA_RANGE4_SCALE_V)<<(LEDC_CH0_GAMMA_RANGE4_SCALE_S))
#define LEDC_CH0_GAMMA_RANGE4_SCALE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE4_SCALE_S  11
/* LEDC_CH0_GAMMA_RANGE4_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE4_DUTY_CYCLE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE4_DUTY_CYCLE_M  ((LEDC_CH0_GAMMA_RANGE4_DUTY_CYCLE_V)<<(LEDC_CH0_GAMMA_RANGE4_DUTY_CYCLE_S))
#define LEDC_CH0_GAMMA_RANGE4_DUTY_CYCLE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE4_DUTY_CYCLE_S  1
/* LEDC_CH0_GAMMA_RANGE4_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH0_GAMMA_RANGE4_DUTY_INC    (BIT(0))
#define LEDC_CH0_GAMMA_RANGE4_DUTY_INC_M  (BIT(0))
#define LEDC_CH0_GAMMA_RANGE4_DUTY_INC_V  0x1
#define LEDC_CH0_GAMMA_RANGE4_DUTY_INC_S  0

#define LEDC_CH0_GAMMA_RANGE5_REG  (DR_REG_LEDC_BASE + 0x414)
/* LEDC_CH0_GAMMA_RANGE5_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE5_DUTY_NUM    0x000003FF
#define LEDC_CH0_GAMMA_RANGE5_DUTY_NUM_M  ((LEDC_CH0_GAMMA_RANGE5_DUTY_NUM_V)<<(LEDC_CH0_GAMMA_RANGE5_DUTY_NUM_S))
#define LEDC_CH0_GAMMA_RANGE5_DUTY_NUM_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE5_DUTY_NUM_S  21
/* LEDC_CH0_GAMMA_RANGE5_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE5_SCALE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE5_SCALE_M  ((LEDC_CH0_GAMMA_RANGE5_SCALE_V)<<(LEDC_CH0_GAMMA_RANGE5_SCALE_S))
#define LEDC_CH0_GAMMA_RANGE5_SCALE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE5_SCALE_S  11
/* LEDC_CH0_GAMMA_RANGE5_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE5_DUTY_CYCLE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE5_DUTY_CYCLE_M  ((LEDC_CH0_GAMMA_RANGE5_DUTY_CYCLE_V)<<(LEDC_CH0_GAMMA_RANGE5_DUTY_CYCLE_S))
#define LEDC_CH0_GAMMA_RANGE5_DUTY_CYCLE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE5_DUTY_CYCLE_S  1
/* LEDC_CH0_GAMMA_RANGE5_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH0_GAMMA_RANGE5_DUTY_INC    (BIT(0))
#define LEDC_CH0_GAMMA_RANGE5_DUTY_INC_M  (BIT(0))
#define LEDC_CH0_GAMMA_RANGE5_DUTY_INC_V  0x1
#define LEDC_CH0_GAMMA_RANGE5_DUTY_INC_S  0

#define LEDC_CH0_GAMMA_RANGE6_REG  (DR_REG_LEDC_BASE + 0x418)
/* LEDC_CH0_GAMMA_RANGE6_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE6_DUTY_NUM    0x000003FF
#define LEDC_CH0_GAMMA_RANGE6_DUTY_NUM_M  ((LEDC_CH0_GAMMA_RANGE6_DUTY_NUM_V)<<(LEDC_CH0_GAMMA_RANGE6_DUTY_NUM_S))
#define LEDC_CH0_GAMMA_RANGE6_DUTY_NUM_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE6_DUTY_NUM_S  21
/* LEDC_CH0_GAMMA_RANGE6_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE6_SCALE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE6_SCALE_M  ((LEDC_CH0_GAMMA_RANGE6_SCALE_V)<<(LEDC_CH0_GAMMA_RANGE6_SCALE_S))
#define LEDC_CH0_GAMMA_RANGE6_SCALE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE6_SCALE_S  11
/* LEDC_CH0_GAMMA_RANGE6_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE6_DUTY_CYCLE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE6_DUTY_CYCLE_M  ((LEDC_CH0_GAMMA_RANGE6_DUTY_CYCLE_V)<<(LEDC_CH0_GAMMA_RANGE6_DUTY_CYCLE_S))
#define LEDC_CH0_GAMMA_RANGE6_DUTY_CYCLE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE6_DUTY_CYCLE_S  1
/* LEDC_CH0_GAMMA_RANGE6_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH0_GAMMA_RANGE6_DUTY_INC    (BIT(0))
#define LEDC_CH0_GAMMA_RANGE6_DUTY_INC_M  (BIT(0))
#define LEDC_CH0_GAMMA_RANGE6_DUTY_INC_V  0x1
#define LEDC_CH0_GAMMA_RANGE6_DUTY_INC_S  0

#define LEDC_CH0_GAMMA_RANGE7_REG  (DR_REG_LEDC_BASE + 0x41c)
/* LEDC_CH0_GAMMA_RANGE7_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE7_DUTY_NUM    0x000003FF
#define LEDC_CH0_GAMMA_RANGE7_DUTY_NUM_M  ((LEDC_CH0_GAMMA_RANGE7_DUTY_NUM_V)<<(LEDC_CH0_GAMMA_RANGE7_DUTY_NUM_S))
#define LEDC_CH0_GAMMA_RANGE7_DUTY_NUM_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE7_DUTY_NUM_S  21
/* LEDC_CH0_GAMMA_RANGE7_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE7_SCALE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE7_SCALE_M  ((LEDC_CH0_GAMMA_RANGE7_SCALE_V)<<(LEDC_CH0_GAMMA_RANGE7_SCALE_S))
#define LEDC_CH0_GAMMA_RANGE7_SCALE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE7_SCALE_S  11
/* LEDC_CH0_GAMMA_RANGE7_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE7_DUTY_CYCLE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE7_DUTY_CYCLE_M  ((LEDC_CH0_GAMMA_RANGE7_DUTY_CYCLE_V)<<(LEDC_CH0_GAMMA_RANGE7_DUTY_CYCLE_S))
#define LEDC_CH0_GAMMA_RANGE7_DUTY_CYCLE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE7_DUTY_CYCLE_S  1
/* LEDC_CH0_GAMMA_RANGE7_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH0_GAMMA_RANGE7_DUTY_INC    (BIT(0))
#define LEDC_CH0_GAMMA_RANGE7_DUTY_INC_M  (BIT(0))
#define LEDC_CH0_GAMMA_RANGE7_DUTY_INC_V  0x1
#define LEDC_CH0_GAMMA_RANGE7_DUTY_INC_S  0

#define LEDC_CH0_GAMMA_RANGE8_REG  (DR_REG_LEDC_BASE + 0x420)
/* LEDC_CH0_GAMMA_RANGE8_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE8_DUTY_NUM    0x000003FF
#define LEDC_CH0_GAMMA_RANGE8_DUTY_NUM_M  ((LEDC_CH0_GAMMA_RANGE8_DUTY_NUM_V)<<(LEDC_CH0_GAMMA_RANGE8_DUTY_NUM_S))
#define LEDC_CH0_GAMMA_RANGE8_DUTY_NUM_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE8_DUTY_NUM_S  21
/* LEDC_CH0_GAMMA_RANGE8_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE8_SCALE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE8_SCALE_M  ((LEDC_CH0_GAMMA_RANGE8_SCALE_V)<<(LEDC_CH0_GAMMA_RANGE8_SCALE_S))
#define LEDC_CH0_GAMMA_RANGE8_SCALE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE8_SCALE_S  11
/* LEDC_CH0_GAMMA_RANGE8_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE8_DUTY_CYCLE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE8_DUTY_CYCLE_M  ((LEDC_CH0_GAMMA_RANGE8_DUTY_CYCLE_V)<<(LEDC_CH0_GAMMA_RANGE8_DUTY_CYCLE_S))
#define LEDC_CH0_GAMMA_RANGE8_DUTY_CYCLE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE8_DUTY_CYCLE_S  1
/* LEDC_CH0_GAMMA_RANGE8_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH0_GAMMA_RANGE8_DUTY_INC    (BIT(0))
#define LEDC_CH0_GAMMA_RANGE8_DUTY_INC_M  (BIT(0))
#define LEDC_CH0_GAMMA_RANGE8_DUTY_INC_V  0x1
#define LEDC_CH0_GAMMA_RANGE8_DUTY_INC_S  0

#define LEDC_CH0_GAMMA_RANGE9_REG  (DR_REG_LEDC_BASE + 0x424)
/* LEDC_CH0_GAMMA_RANGE9_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE9_DUTY_NUM    0x000003FF
#define LEDC_CH0_GAMMA_RANGE9_DUTY_NUM_M  ((LEDC_CH0_GAMMA_RANGE9_DUTY_NUM_V)<<(LEDC_CH0_GAMMA_RANGE9_DUTY_NUM_S))
#define LEDC_CH0_GAMMA_RANGE9_DUTY_NUM_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE9_DUTY_NUM_S  21
/* LEDC_CH0_GAMMA_RANGE9_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE9_SCALE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE9_SCALE_M  ((LEDC_CH0_GAMMA_RANGE9_SCALE_V)<<(LEDC_CH0_GAMMA_RANGE9_SCALE_S))
#define LEDC_CH0_GAMMA_RANGE9_SCALE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE9_SCALE_S  11
/* LEDC_CH0_GAMMA_RANGE9_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE9_DUTY_CYCLE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE9_DUTY_CYCLE_M  ((LEDC_CH0_GAMMA_RANGE9_DUTY_CYCLE_V)<<(LEDC_CH0_GAMMA_RANGE9_DUTY_CYCLE_S))
#define LEDC_CH0_GAMMA_RANGE9_DUTY_CYCLE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE9_DUTY_CYCLE_S  1
/* LEDC_CH0_GAMMA_RANGE9_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH0_GAMMA_RANGE9_DUTY_INC    (BIT(0))
#define LEDC_CH0_GAMMA_RANGE9_DUTY_INC_M  (BIT(0))
#define LEDC_CH0_GAMMA_RANGE9_DUTY_INC_V  0x1
#define LEDC_CH0_GAMMA_RANGE9_DUTY_INC_S  0

#define LEDC_CH0_GAMMA_RANGE10_REG  (DR_REG_LEDC_BASE + 0x428)
/* LEDC_CH0_GAMMA_RANGE10_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE10_DUTY_NUM    0x000003FF
#define LEDC_CH0_GAMMA_RANGE10_DUTY_NUM_M  ((LEDC_CH0_GAMMA_RANGE10_DUTY_NUM_V)<<(LEDC_CH0_GAMMA_RANGE10_DUTY_NUM_S))
#define LEDC_CH0_GAMMA_RANGE10_DUTY_NUM_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE10_DUTY_NUM_S  21
/* LEDC_CH0_GAMMA_RANGE10_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE10_SCALE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE10_SCALE_M  ((LEDC_CH0_GAMMA_RANGE10_SCALE_V)<<(LEDC_CH0_GAMMA_RANGE10_SCALE_S))
#define LEDC_CH0_GAMMA_RANGE10_SCALE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE10_SCALE_S  11
/* LEDC_CH0_GAMMA_RANGE10_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE10_DUTY_CYCLE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE10_DUTY_CYCLE_M  ((LEDC_CH0_GAMMA_RANGE10_DUTY_CYCLE_V)<<(LEDC_CH0_GAMMA_RANGE10_DUTY_CYCLE_S))
#define LEDC_CH0_GAMMA_RANGE10_DUTY_CYCLE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE10_DUTY_CYCLE_S  1
/* LEDC_CH0_GAMMA_RANGE10_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH0_GAMMA_RANGE10_DUTY_INC    (BIT(0))
#define LEDC_CH0_GAMMA_RANGE10_DUTY_INC_M  (BIT(0))
#define LEDC_CH0_GAMMA_RANGE10_DUTY_INC_V  0x1
#define LEDC_CH0_GAMMA_RANGE10_DUTY_INC_S  0

#define LEDC_CH0_GAMMA_RANGE11_REG  (DR_REG_LEDC_BASE + 0x42c)
/* LEDC_CH0_GAMMA_RANGE11_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE11_DUTY_NUM    0x000003FF
#define LEDC_CH0_GAMMA_RANGE11_DUTY_NUM_M  ((LEDC_CH0_GAMMA_RANGE11_DUTY_NUM_V)<<(LEDC_CH0_GAMMA_RANGE11_DUTY_NUM_S))
#define LEDC_CH0_GAMMA_RANGE11_DUTY_NUM_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE11_DUTY_NUM_S  21
/* LEDC_CH0_GAMMA_RANGE11_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE11_SCALE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE11_SCALE_M  ((LEDC_CH0_GAMMA_RANGE11_SCALE_V)<<(LEDC_CH0_GAMMA_RANGE11_SCALE_S))
#define LEDC_CH0_GAMMA_RANGE11_SCALE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE11_SCALE_S  11
/* LEDC_CH0_GAMMA_RANGE11_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE11_DUTY_CYCLE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE11_DUTY_CYCLE_M  ((LEDC_CH0_GAMMA_RANGE11_DUTY_CYCLE_V)<<(LEDC_CH0_GAMMA_RANGE11_DUTY_CYCLE_S))
#define LEDC_CH0_GAMMA_RANGE11_DUTY_CYCLE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE11_DUTY_CYCLE_S  1
/* LEDC_CH0_GAMMA_RANGE11_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH0_GAMMA_RANGE11_DUTY_INC    (BIT(0))
#define LEDC_CH0_GAMMA_RANGE11_DUTY_INC_M  (BIT(0))
#define LEDC_CH0_GAMMA_RANGE11_DUTY_INC_V  0x1
#define LEDC_CH0_GAMMA_RANGE11_DUTY_INC_S  0

#define LEDC_CH0_GAMMA_RANGE12_REG  (DR_REG_LEDC_BASE + 0x430)
/* LEDC_CH0_GAMMA_RANGE12_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE12_DUTY_NUM    0x000003FF
#define LEDC_CH0_GAMMA_RANGE12_DUTY_NUM_M  ((LEDC_CH0_GAMMA_RANGE12_DUTY_NUM_V)<<(LEDC_CH0_GAMMA_RANGE12_DUTY_NUM_S))
#define LEDC_CH0_GAMMA_RANGE12_DUTY_NUM_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE12_DUTY_NUM_S  21
/* LEDC_CH0_GAMMA_RANGE12_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE12_SCALE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE12_SCALE_M  ((LEDC_CH0_GAMMA_RANGE12_SCALE_V)<<(LEDC_CH0_GAMMA_RANGE12_SCALE_S))
#define LEDC_CH0_GAMMA_RANGE12_SCALE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE12_SCALE_S  11
/* LEDC_CH0_GAMMA_RANGE12_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE12_DUTY_CYCLE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE12_DUTY_CYCLE_M  ((LEDC_CH0_GAMMA_RANGE12_DUTY_CYCLE_V)<<(LEDC_CH0_GAMMA_RANGE12_DUTY_CYCLE_S))
#define LEDC_CH0_GAMMA_RANGE12_DUTY_CYCLE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE12_DUTY_CYCLE_S  1
/* LEDC_CH0_GAMMA_RANGE12_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH0_GAMMA_RANGE12_DUTY_INC    (BIT(0))
#define LEDC_CH0_GAMMA_RANGE12_DUTY_INC_M  (BIT(0))
#define LEDC_CH0_GAMMA_RANGE12_DUTY_INC_V  0x1
#define LEDC_CH0_GAMMA_RANGE12_DUTY_INC_S  0

#define LEDC_CH0_GAMMA_RANGE13_REG  (DR_REG_LEDC_BASE + 0x434)
/* LEDC_CH0_GAMMA_RANGE13_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE13_DUTY_NUM    0x000003FF
#define LEDC_CH0_GAMMA_RANGE13_DUTY_NUM_M  ((LEDC_CH0_GAMMA_RANGE13_DUTY_NUM_V)<<(LEDC_CH0_GAMMA_RANGE13_DUTY_NUM_S))
#define LEDC_CH0_GAMMA_RANGE13_DUTY_NUM_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE13_DUTY_NUM_S  21
/* LEDC_CH0_GAMMA_RANGE13_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE13_SCALE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE13_SCALE_M  ((LEDC_CH0_GAMMA_RANGE13_SCALE_V)<<(LEDC_CH0_GAMMA_RANGE13_SCALE_S))
#define LEDC_CH0_GAMMA_RANGE13_SCALE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE13_SCALE_S  11
/* LEDC_CH0_GAMMA_RANGE13_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE13_DUTY_CYCLE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE13_DUTY_CYCLE_M  ((LEDC_CH0_GAMMA_RANGE13_DUTY_CYCLE_V)<<(LEDC_CH0_GAMMA_RANGE13_DUTY_CYCLE_S))
#define LEDC_CH0_GAMMA_RANGE13_DUTY_CYCLE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE13_DUTY_CYCLE_S  1
/* LEDC_CH0_GAMMA_RANGE13_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH0_GAMMA_RANGE13_DUTY_INC    (BIT(0))
#define LEDC_CH0_GAMMA_RANGE13_DUTY_INC_M  (BIT(0))
#define LEDC_CH0_GAMMA_RANGE13_DUTY_INC_V  0x1
#define LEDC_CH0_GAMMA_RANGE13_DUTY_INC_S  0

#define LEDC_CH0_GAMMA_RANGE14_REG  (DR_REG_LEDC_BASE + 0x438)
/* LEDC_CH0_GAMMA_RANGE14_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE14_DUTY_NUM    0x000003FF
#define LEDC_CH0_GAMMA_RANGE14_DUTY_NUM_M  ((LEDC_CH0_GAMMA_RANGE14_DUTY_NUM_V)<<(LEDC_CH0_GAMMA_RANGE14_DUTY_NUM_S))
#define LEDC_CH0_GAMMA_RANGE14_DUTY_NUM_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE14_DUTY_NUM_S  21
/* LEDC_CH0_GAMMA_RANGE14_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE14_SCALE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE14_SCALE_M  ((LEDC_CH0_GAMMA_RANGE14_SCALE_V)<<(LEDC_CH0_GAMMA_RANGE14_SCALE_S))
#define LEDC_CH0_GAMMA_RANGE14_SCALE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE14_SCALE_S  11
/* LEDC_CH0_GAMMA_RANGE14_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE14_DUTY_CYCLE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE14_DUTY_CYCLE_M  ((LEDC_CH0_GAMMA_RANGE14_DUTY_CYCLE_V)<<(LEDC_CH0_GAMMA_RANGE14_DUTY_CYCLE_S))
#define LEDC_CH0_GAMMA_RANGE14_DUTY_CYCLE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE14_DUTY_CYCLE_S  1
/* LEDC_CH0_GAMMA_RANGE14_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH0_GAMMA_RANGE14_DUTY_INC    (BIT(0))
#define LEDC_CH0_GAMMA_RANGE14_DUTY_INC_M  (BIT(0))
#define LEDC_CH0_GAMMA_RANGE14_DUTY_INC_V  0x1
#define LEDC_CH0_GAMMA_RANGE14_DUTY_INC_S  0

#define LEDC_CH0_GAMMA_RANGE15_REG  (DR_REG_LEDC_BASE + 0x43c)
/* LEDC_CH0_GAMMA_RANGE15_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE15_DUTY_NUM    0x000003FF
#define LEDC_CH0_GAMMA_RANGE15_DUTY_NUM_M  ((LEDC_CH0_GAMMA_RANGE15_DUTY_NUM_V)<<(LEDC_CH0_GAMMA_RANGE15_DUTY_NUM_S))
#define LEDC_CH0_GAMMA_RANGE15_DUTY_NUM_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE15_DUTY_NUM_S  21
/* LEDC_CH0_GAMMA_RANGE15_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE15_SCALE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE15_SCALE_M  ((LEDC_CH0_GAMMA_RANGE15_SCALE_V)<<(LEDC_CH0_GAMMA_RANGE15_SCALE_S))
#define LEDC_CH0_GAMMA_RANGE15_SCALE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE15_SCALE_S  11
/* LEDC_CH0_GAMMA_RANGE15_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH0_GAMMA_RANGE15_DUTY_CYCLE    0x000003FF
#define LEDC_CH0_GAMMA_RANGE15_DUTY_CYCLE_M  ((LEDC_CH0_GAMMA_RANGE15_DUTY_CYCLE_V)<<(LEDC_CH0_GAMMA_RANGE15_DUTY_CYCLE_S))
#define LEDC_CH0_GAMMA_RANGE15_DUTY_CYCLE_V  0x3FF
#define LEDC_CH0_GAMMA_RANGE15_DUTY_CYCLE_S  1
/* LEDC_CH0_GAMMA_RANGE15_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH0_GAMMA_RANGE15_DUTY_INC    (BIT(0))
#define LEDC_CH0_GAMMA_RANGE15_DUTY_INC_M  (BIT(0))
#define LEDC_CH0_GAMMA_RANGE15_DUTY_INC_V  0x1
#define LEDC_CH0_GAMMA_RANGE15_DUTY_INC_S  0


#define LEDC_CH1_GAMMA_RANGE0_REG  (DR_REG_LEDC_BASE + 0x440)
/* LEDC_CH1_GAMMA_RANGE0_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE0_DUTY_NUM    0x000003FF
#define LEDC_CH1_GAMMA_RANGE0_DUTY_NUM_M  ((LEDC_CH1_GAMMA_RANGE0_DUTY_NUM_V)<<(LEDC_CH1_GAMMA_RANGE0_DUTY_NUM_S))
#define LEDC_CH1_GAMMA_RANGE0_DUTY_NUM_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE0_DUTY_NUM_S  21
/* LEDC_CH1_GAMMA_RANGE0_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE0_SCALE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE0_SCALE_M  ((LEDC_CH1_GAMMA_RANGE0_SCALE_V)<<(LEDC_CH1_GAMMA_RANGE0_SCALE_S))
#define LEDC_CH1_GAMMA_RANGE0_SCALE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE0_SCALE_S  11
/* LEDC_CH1_GAMMA_RANGE0_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE0_DUTY_CYCLE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE0_DUTY_CYCLE_M  ((LEDC_CH1_GAMMA_RANGE0_DUTY_CYCLE_V)<<(LEDC_CH1_GAMMA_RANGE0_DUTY_CYCLE_S))
#define LEDC_CH1_GAMMA_RANGE0_DUTY_CYCLE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE0_DUTY_CYCLE_S  1
/* LEDC_CH1_GAMMA_RANGE0_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH1_GAMMA_RANGE0_DUTY_INC    (BIT(0))
#define LEDC_CH1_GAMMA_RANGE0_DUTY_INC_M  (BIT(0))
#define LEDC_CH1_GAMMA_RANGE0_DUTY_INC_V  0x1
#define LEDC_CH1_GAMMA_RANGE0_DUTY_INC_S  0

#define LEDC_CH1_GAMMA_RANGE1_REG  (DR_REG_LEDC_BASE + 0x444)
/* LEDC_CH1_GAMMA_RANGE1_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE1_DUTY_NUM    0x000003FF
#define LEDC_CH1_GAMMA_RANGE1_DUTY_NUM_M  ((LEDC_CH1_GAMMA_RANGE1_DUTY_NUM_V)<<(LEDC_CH1_GAMMA_RANGE1_DUTY_NUM_S))
#define LEDC_CH1_GAMMA_RANGE1_DUTY_NUM_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE1_DUTY_NUM_S  21
/* LEDC_CH1_GAMMA_RANGE1_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE1_SCALE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE1_SCALE_M  ((LEDC_CH1_GAMMA_RANGE1_SCALE_V)<<(LEDC_CH1_GAMMA_RANGE1_SCALE_S))
#define LEDC_CH1_GAMMA_RANGE1_SCALE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE1_SCALE_S  11
/* LEDC_CH1_GAMMA_RANGE1_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE1_DUTY_CYCLE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE1_DUTY_CYCLE_M  ((LEDC_CH1_GAMMA_RANGE1_DUTY_CYCLE_V)<<(LEDC_CH1_GAMMA_RANGE1_DUTY_CYCLE_S))
#define LEDC_CH1_GAMMA_RANGE1_DUTY_CYCLE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE1_DUTY_CYCLE_S  1
/* LEDC_CH1_GAMMA_RANGE1_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH1_GAMMA_RANGE1_DUTY_INC    (BIT(0))
#define LEDC_CH1_GAMMA_RANGE1_DUTY_INC_M  (BIT(0))
#define LEDC_CH1_GAMMA_RANGE1_DUTY_INC_V  0x1
#define LEDC_CH1_GAMMA_RANGE1_DUTY_INC_S  0

#define LEDC_CH1_GAMMA_RANGE2_REG  (DR_REG_LEDC_BASE + 0x448)
/* LEDC_CH1_GAMMA_RANGE2_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE2_DUTY_NUM    0x000003FF
#define LEDC_CH1_GAMMA_RANGE2_DUTY_NUM_M  ((LEDC_CH1_GAMMA_RANGE2_DUTY_NUM_V)<<(LEDC_CH1_GAMMA_RANGE2_DUTY_NUM_S))
#define LEDC_CH1_GAMMA_RANGE2_DUTY_NUM_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE2_DUTY_NUM_S  21
/* LEDC_CH1_GAMMA_RANGE2_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE2_SCALE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE2_SCALE_M  ((LEDC_CH1_GAMMA_RANGE2_SCALE_V)<<(LEDC_CH1_GAMMA_RANGE2_SCALE_S))
#define LEDC_CH1_GAMMA_RANGE2_SCALE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE2_SCALE_S  11
/* LEDC_CH1_GAMMA_RANGE2_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE2_DUTY_CYCLE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE2_DUTY_CYCLE_M  ((LEDC_CH1_GAMMA_RANGE2_DUTY_CYCLE_V)<<(LEDC_CH1_GAMMA_RANGE2_DUTY_CYCLE_S))
#define LEDC_CH1_GAMMA_RANGE2_DUTY_CYCLE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE2_DUTY_CYCLE_S  1
/* LEDC_CH1_GAMMA_RANGE2_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH1_GAMMA_RANGE2_DUTY_INC    (BIT(0))
#define LEDC_CH1_GAMMA_RANGE2_DUTY_INC_M  (BIT(0))
#define LEDC_CH1_GAMMA_RANGE2_DUTY_INC_V  0x1
#define LEDC_CH1_GAMMA_RANGE2_DUTY_INC_S  0

#define LEDC_CH1_GAMMA_RANGE3_REG  (DR_REG_LEDC_BASE + 0x44c)
/* LEDC_CH1_GAMMA_RANGE3_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE3_DUTY_NUM    0x000003FF
#define LEDC_CH1_GAMMA_RANGE3_DUTY_NUM_M  ((LEDC_CH1_GAMMA_RANGE3_DUTY_NUM_V)<<(LEDC_CH1_GAMMA_RANGE3_DUTY_NUM_S))
#define LEDC_CH1_GAMMA_RANGE3_DUTY_NUM_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE3_DUTY_NUM_S  21
/* LEDC_CH1_GAMMA_RANGE3_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE3_SCALE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE3_SCALE_M  ((LEDC_CH1_GAMMA_RANGE3_SCALE_V)<<(LEDC_CH1_GAMMA_RANGE3_SCALE_S))
#define LEDC_CH1_GAMMA_RANGE3_SCALE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE3_SCALE_S  11
/* LEDC_CH1_GAMMA_RANGE3_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE3_DUTY_CYCLE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE3_DUTY_CYCLE_M  ((LEDC_CH1_GAMMA_RANGE3_DUTY_CYCLE_V)<<(LEDC_CH1_GAMMA_RANGE3_DUTY_CYCLE_S))
#define LEDC_CH1_GAMMA_RANGE3_DUTY_CYCLE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE3_DUTY_CYCLE_S  1
/* LEDC_CH1_GAMMA_RANGE3_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH1_GAMMA_RANGE3_DUTY_INC    (BIT(0))
#define LEDC_CH1_GAMMA_RANGE3_DUTY_INC_M  (BIT(0))
#define LEDC_CH1_GAMMA_RANGE3_DUTY_INC_V  0x1
#define LEDC_CH1_GAMMA_RANGE3_DUTY_INC_S  0

#define LEDC_CH1_GAMMA_RANGE4_REG  (DR_REG_LEDC_BASE + 0x450)
/* LEDC_CH1_GAMMA_RANGE4_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE4_DUTY_NUM    0x000003FF
#define LEDC_CH1_GAMMA_RANGE4_DUTY_NUM_M  ((LEDC_CH1_GAMMA_RANGE4_DUTY_NUM_V)<<(LEDC_CH1_GAMMA_RANGE4_DUTY_NUM_S))
#define LEDC_CH1_GAMMA_RANGE4_DUTY_NUM_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE4_DUTY_NUM_S  21
/* LEDC_CH1_GAMMA_RANGE4_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE4_SCALE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE4_SCALE_M  ((LEDC_CH1_GAMMA_RANGE4_SCALE_V)<<(LEDC_CH1_GAMMA_RANGE4_SCALE_S))
#define LEDC_CH1_GAMMA_RANGE4_SCALE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE4_SCALE_S  11
/* LEDC_CH1_GAMMA_RANGE4_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE4_DUTY_CYCLE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE4_DUTY_CYCLE_M  ((LEDC_CH1_GAMMA_RANGE4_DUTY_CYCLE_V)<<(LEDC_CH1_GAMMA_RANGE4_DUTY_CYCLE_S))
#define LEDC_CH1_GAMMA_RANGE4_DUTY_CYCLE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE4_DUTY_CYCLE_S  1
/* LEDC_CH1_GAMMA_RANGE4_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH1_GAMMA_RANGE4_DUTY_INC    (BIT(0))
#define LEDC_CH1_GAMMA_RANGE4_DUTY_INC_M  (BIT(0))
#define LEDC_CH1_GAMMA_RANGE4_DUTY_INC_V  0x1
#define LEDC_CH1_GAMMA_RANGE4_DUTY_INC_S  0

#define LEDC_CH1_GAMMA_RANGE5_REG  (DR_REG_LEDC_BASE + 0x454)
/* LEDC_CH1_GAMMA_RANGE5_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE5_DUTY_NUM    0x000003FF
#define LEDC_CH1_GAMMA_RANGE5_DUTY_NUM_M  ((LEDC_CH1_GAMMA_RANGE5_DUTY_NUM_V)<<(LEDC_CH1_GAMMA_RANGE5_DUTY_NUM_S))
#define LEDC_CH1_GAMMA_RANGE5_DUTY_NUM_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE5_DUTY_NUM_S  21
/* LEDC_CH1_GAMMA_RANGE5_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE5_SCALE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE5_SCALE_M  ((LEDC_CH1_GAMMA_RANGE5_SCALE_V)<<(LEDC_CH1_GAMMA_RANGE5_SCALE_S))
#define LEDC_CH1_GAMMA_RANGE5_SCALE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE5_SCALE_S  11
/* LEDC_CH1_GAMMA_RANGE5_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE5_DUTY_CYCLE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE5_DUTY_CYCLE_M  ((LEDC_CH1_GAMMA_RANGE5_DUTY_CYCLE_V)<<(LEDC_CH1_GAMMA_RANGE5_DUTY_CYCLE_S))
#define LEDC_CH1_GAMMA_RANGE5_DUTY_CYCLE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE5_DUTY_CYCLE_S  1
/* LEDC_CH1_GAMMA_RANGE5_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH1_GAMMA_RANGE5_DUTY_INC    (BIT(0))
#define LEDC_CH1_GAMMA_RANGE5_DUTY_INC_M  (BIT(0))
#define LEDC_CH1_GAMMA_RANGE5_DUTY_INC_V  0x1
#define LEDC_CH1_GAMMA_RANGE5_DUTY_INC_S  0

#define LEDC_CH1_GAMMA_RANGE6_REG  (DR_REG_LEDC_BASE + 0x458)
/* LEDC_CH1_GAMMA_RANGE6_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE6_DUTY_NUM    0x000003FF
#define LEDC_CH1_GAMMA_RANGE6_DUTY_NUM_M  ((LEDC_CH1_GAMMA_RANGE6_DUTY_NUM_V)<<(LEDC_CH1_GAMMA_RANGE6_DUTY_NUM_S))
#define LEDC_CH1_GAMMA_RANGE6_DUTY_NUM_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE6_DUTY_NUM_S  21
/* LEDC_CH1_GAMMA_RANGE6_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE6_SCALE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE6_SCALE_M  ((LEDC_CH1_GAMMA_RANGE6_SCALE_V)<<(LEDC_CH1_GAMMA_RANGE6_SCALE_S))
#define LEDC_CH1_GAMMA_RANGE6_SCALE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE6_SCALE_S  11
/* LEDC_CH1_GAMMA_RANGE6_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE6_DUTY_CYCLE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE6_DUTY_CYCLE_M  ((LEDC_CH1_GAMMA_RANGE6_DUTY_CYCLE_V)<<(LEDC_CH1_GAMMA_RANGE6_DUTY_CYCLE_S))
#define LEDC_CH1_GAMMA_RANGE6_DUTY_CYCLE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE6_DUTY_CYCLE_S  1
/* LEDC_CH1_GAMMA_RANGE6_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH1_GAMMA_RANGE6_DUTY_INC    (BIT(0))
#define LEDC_CH1_GAMMA_RANGE6_DUTY_INC_M  (BIT(0))
#define LEDC_CH1_GAMMA_RANGE6_DUTY_INC_V  0x1
#define LEDC_CH1_GAMMA_RANGE6_DUTY_INC_S  0

#define LEDC_CH1_GAMMA_RANGE7_REG  (DR_REG_LEDC_BASE + 0x45c)
/* LEDC_CH1_GAMMA_RANGE7_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE7_DUTY_NUM    0x000003FF
#define LEDC_CH1_GAMMA_RANGE7_DUTY_NUM_M  ((LEDC_CH1_GAMMA_RANGE7_DUTY_NUM_V)<<(LEDC_CH1_GAMMA_RANGE7_DUTY_NUM_S))
#define LEDC_CH1_GAMMA_RANGE7_DUTY_NUM_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE7_DUTY_NUM_S  21
/* LEDC_CH1_GAMMA_RANGE7_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE7_SCALE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE7_SCALE_M  ((LEDC_CH1_GAMMA_RANGE7_SCALE_V)<<(LEDC_CH1_GAMMA_RANGE7_SCALE_S))
#define LEDC_CH1_GAMMA_RANGE7_SCALE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE7_SCALE_S  11
/* LEDC_CH1_GAMMA_RANGE7_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE7_DUTY_CYCLE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE7_DUTY_CYCLE_M  ((LEDC_CH1_GAMMA_RANGE7_DUTY_CYCLE_V)<<(LEDC_CH1_GAMMA_RANGE7_DUTY_CYCLE_S))
#define LEDC_CH1_GAMMA_RANGE7_DUTY_CYCLE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE7_DUTY_CYCLE_S  1
/* LEDC_CH1_GAMMA_RANGE7_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH1_GAMMA_RANGE7_DUTY_INC    (BIT(0))
#define LEDC_CH1_GAMMA_RANGE7_DUTY_INC_M  (BIT(0))
#define LEDC_CH1_GAMMA_RANGE7_DUTY_INC_V  0x1
#define LEDC_CH1_GAMMA_RANGE7_DUTY_INC_S  0

#define LEDC_CH1_GAMMA_RANGE8_REG  (DR_REG_LEDC_BASE + 0x460)
/* LEDC_CH1_GAMMA_RANGE8_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE8_DUTY_NUM    0x000003FF
#define LEDC_CH1_GAMMA_RANGE8_DUTY_NUM_M  ((LEDC_CH1_GAMMA_RANGE8_DUTY_NUM_V)<<(LEDC_CH1_GAMMA_RANGE8_DUTY_NUM_S))
#define LEDC_CH1_GAMMA_RANGE8_DUTY_NUM_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE8_DUTY_NUM_S  21
/* LEDC_CH1_GAMMA_RANGE8_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE8_SCALE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE8_SCALE_M  ((LEDC_CH1_GAMMA_RANGE8_SCALE_V)<<(LEDC_CH1_GAMMA_RANGE8_SCALE_S))
#define LEDC_CH1_GAMMA_RANGE8_SCALE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE8_SCALE_S  11
/* LEDC_CH1_GAMMA_RANGE8_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE8_DUTY_CYCLE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE8_DUTY_CYCLE_M  ((LEDC_CH1_GAMMA_RANGE8_DUTY_CYCLE_V)<<(LEDC_CH1_GAMMA_RANGE8_DUTY_CYCLE_S))
#define LEDC_CH1_GAMMA_RANGE8_DUTY_CYCLE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE8_DUTY_CYCLE_S  1
/* LEDC_CH1_GAMMA_RANGE8_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH1_GAMMA_RANGE8_DUTY_INC    (BIT(0))
#define LEDC_CH1_GAMMA_RANGE8_DUTY_INC_M  (BIT(0))
#define LEDC_CH1_GAMMA_RANGE8_DUTY_INC_V  0x1
#define LEDC_CH1_GAMMA_RANGE8_DUTY_INC_S  0

#define LEDC_CH1_GAMMA_RANGE9_REG  (DR_REG_LEDC_BASE + 0x464)
/* LEDC_CH1_GAMMA_RANGE9_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE9_DUTY_NUM    0x000003FF
#define LEDC_CH1_GAMMA_RANGE9_DUTY_NUM_M  ((LEDC_CH1_GAMMA_RANGE9_DUTY_NUM_V)<<(LEDC_CH1_GAMMA_RANGE9_DUTY_NUM_S))
#define LEDC_CH1_GAMMA_RANGE9_DUTY_NUM_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE9_DUTY_NUM_S  21
/* LEDC_CH1_GAMMA_RANGE9_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE9_SCALE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE9_SCALE_M  ((LEDC_CH1_GAMMA_RANGE9_SCALE_V)<<(LEDC_CH1_GAMMA_RANGE9_SCALE_S))
#define LEDC_CH1_GAMMA_RANGE9_SCALE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE9_SCALE_S  11
/* LEDC_CH1_GAMMA_RANGE9_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE9_DUTY_CYCLE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE9_DUTY_CYCLE_M  ((LEDC_CH1_GAMMA_RANGE9_DUTY_CYCLE_V)<<(LEDC_CH1_GAMMA_RANGE9_DUTY_CYCLE_S))
#define LEDC_CH1_GAMMA_RANGE9_DUTY_CYCLE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE9_DUTY_CYCLE_S  1
/* LEDC_CH1_GAMMA_RANGE9_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH1_GAMMA_RANGE9_DUTY_INC    (BIT(0))
#define LEDC_CH1_GAMMA_RANGE9_DUTY_INC_M  (BIT(0))
#define LEDC_CH1_GAMMA_RANGE9_DUTY_INC_V  0x1
#define LEDC_CH1_GAMMA_RANGE9_DUTY_INC_S  0

#define LEDC_CH1_GAMMA_RANGE10_REG  (DR_REG_LEDC_BASE + 0x468)
/* LEDC_CH1_GAMMA_RANGE10_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE10_DUTY_NUM    0x000003FF
#define LEDC_CH1_GAMMA_RANGE10_DUTY_NUM_M  ((LEDC_CH1_GAMMA_RANGE10_DUTY_NUM_V)<<(LEDC_CH1_GAMMA_RANGE10_DUTY_NUM_S))
#define LEDC_CH1_GAMMA_RANGE10_DUTY_NUM_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE10_DUTY_NUM_S  21
/* LEDC_CH1_GAMMA_RANGE10_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE10_SCALE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE10_SCALE_M  ((LEDC_CH1_GAMMA_RANGE10_SCALE_V)<<(LEDC_CH1_GAMMA_RANGE10_SCALE_S))
#define LEDC_CH1_GAMMA_RANGE10_SCALE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE10_SCALE_S  11
/* LEDC_CH1_GAMMA_RANGE10_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE10_DUTY_CYCLE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE10_DUTY_CYCLE_M  ((LEDC_CH1_GAMMA_RANGE10_DUTY_CYCLE_V)<<(LEDC_CH1_GAMMA_RANGE10_DUTY_CYCLE_S))
#define LEDC_CH1_GAMMA_RANGE10_DUTY_CYCLE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE10_DUTY_CYCLE_S  1
/* LEDC_CH1_GAMMA_RANGE10_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH1_GAMMA_RANGE10_DUTY_INC    (BIT(0))
#define LEDC_CH1_GAMMA_RANGE10_DUTY_INC_M  (BIT(0))
#define LEDC_CH1_GAMMA_RANGE10_DUTY_INC_V  0x1
#define LEDC_CH1_GAMMA_RANGE10_DUTY_INC_S  0

#define LEDC_CH1_GAMMA_RANGE11_REG  (DR_REG_LEDC_BASE + 0x46c)
/* LEDC_CH1_GAMMA_RANGE11_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE11_DUTY_NUM    0x000003FF
#define LEDC_CH1_GAMMA_RANGE11_DUTY_NUM_M  ((LEDC_CH1_GAMMA_RANGE11_DUTY_NUM_V)<<(LEDC_CH1_GAMMA_RANGE11_DUTY_NUM_S))
#define LEDC_CH1_GAMMA_RANGE11_DUTY_NUM_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE11_DUTY_NUM_S  21
/* LEDC_CH1_GAMMA_RANGE11_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE11_SCALE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE11_SCALE_M  ((LEDC_CH1_GAMMA_RANGE11_SCALE_V)<<(LEDC_CH1_GAMMA_RANGE11_SCALE_S))
#define LEDC_CH1_GAMMA_RANGE11_SCALE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE11_SCALE_S  11
/* LEDC_CH1_GAMMA_RANGE11_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE11_DUTY_CYCLE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE11_DUTY_CYCLE_M  ((LEDC_CH1_GAMMA_RANGE11_DUTY_CYCLE_V)<<(LEDC_CH1_GAMMA_RANGE11_DUTY_CYCLE_S))
#define LEDC_CH1_GAMMA_RANGE11_DUTY_CYCLE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE11_DUTY_CYCLE_S  1
/* LEDC_CH1_GAMMA_RANGE11_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH1_GAMMA_RANGE11_DUTY_INC    (BIT(0))
#define LEDC_CH1_GAMMA_RANGE11_DUTY_INC_M  (BIT(0))
#define LEDC_CH1_GAMMA_RANGE11_DUTY_INC_V  0x1
#define LEDC_CH1_GAMMA_RANGE11_DUTY_INC_S  0

#define LEDC_CH1_GAMMA_RANGE12_REG  (DR_REG_LEDC_BASE + 0x470)
/* LEDC_CH1_GAMMA_RANGE12_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE12_DUTY_NUM    0x000003FF
#define LEDC_CH1_GAMMA_RANGE12_DUTY_NUM_M  ((LEDC_CH1_GAMMA_RANGE12_DUTY_NUM_V)<<(LEDC_CH1_GAMMA_RANGE12_DUTY_NUM_S))
#define LEDC_CH1_GAMMA_RANGE12_DUTY_NUM_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE12_DUTY_NUM_S  21
/* LEDC_CH1_GAMMA_RANGE12_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE12_SCALE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE12_SCALE_M  ((LEDC_CH1_GAMMA_RANGE12_SCALE_V)<<(LEDC_CH1_GAMMA_RANGE12_SCALE_S))
#define LEDC_CH1_GAMMA_RANGE12_SCALE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE12_SCALE_S  11
/* LEDC_CH1_GAMMA_RANGE12_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE12_DUTY_CYCLE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE12_DUTY_CYCLE_M  ((LEDC_CH1_GAMMA_RANGE12_DUTY_CYCLE_V)<<(LEDC_CH1_GAMMA_RANGE12_DUTY_CYCLE_S))
#define LEDC_CH1_GAMMA_RANGE12_DUTY_CYCLE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE12_DUTY_CYCLE_S  1
/* LEDC_CH1_GAMMA_RANGE12_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH1_GAMMA_RANGE12_DUTY_INC    (BIT(0))
#define LEDC_CH1_GAMMA_RANGE12_DUTY_INC_M  (BIT(0))
#define LEDC_CH1_GAMMA_RANGE12_DUTY_INC_V  0x1
#define LEDC_CH1_GAMMA_RANGE12_DUTY_INC_S  0

#define LEDC_CH1_GAMMA_RANGE13_REG  (DR_REG_LEDC_BASE + 0x474)
/* LEDC_CH1_GAMMA_RANGE13_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE13_DUTY_NUM    0x000003FF
#define LEDC_CH1_GAMMA_RANGE13_DUTY_NUM_M  ((LEDC_CH1_GAMMA_RANGE13_DUTY_NUM_V)<<(LEDC_CH1_GAMMA_RANGE13_DUTY_NUM_S))
#define LEDC_CH1_GAMMA_RANGE13_DUTY_NUM_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE13_DUTY_NUM_S  21
/* LEDC_CH1_GAMMA_RANGE13_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE13_SCALE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE13_SCALE_M  ((LEDC_CH1_GAMMA_RANGE13_SCALE_V)<<(LEDC_CH1_GAMMA_RANGE13_SCALE_S))
#define LEDC_CH1_GAMMA_RANGE13_SCALE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE13_SCALE_S  11
/* LEDC_CH1_GAMMA_RANGE13_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE13_DUTY_CYCLE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE13_DUTY_CYCLE_M  ((LEDC_CH1_GAMMA_RANGE13_DUTY_CYCLE_V)<<(LEDC_CH1_GAMMA_RANGE13_DUTY_CYCLE_S))
#define LEDC_CH1_GAMMA_RANGE13_DUTY_CYCLE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE13_DUTY_CYCLE_S  1
/* LEDC_CH1_GAMMA_RANGE13_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH1_GAMMA_RANGE13_DUTY_INC    (BIT(0))
#define LEDC_CH1_GAMMA_RANGE13_DUTY_INC_M  (BIT(0))
#define LEDC_CH1_GAMMA_RANGE13_DUTY_INC_V  0x1
#define LEDC_CH1_GAMMA_RANGE13_DUTY_INC_S  0

#define LEDC_CH1_GAMMA_RANGE14_REG  (DR_REG_LEDC_BASE + 0x478)
/* LEDC_CH1_GAMMA_RANGE14_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE14_DUTY_NUM    0x000003FF
#define LEDC_CH1_GAMMA_RANGE14_DUTY_NUM_M  ((LEDC_CH1_GAMMA_RANGE14_DUTY_NUM_V)<<(LEDC_CH1_GAMMA_RANGE14_DUTY_NUM_S))
#define LEDC_CH1_GAMMA_RANGE14_DUTY_NUM_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE14_DUTY_NUM_S  21
/* LEDC_CH1_GAMMA_RANGE14_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE14_SCALE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE14_SCALE_M  ((LEDC_CH1_GAMMA_RANGE14_SCALE_V)<<(LEDC_CH1_GAMMA_RANGE14_SCALE_S))
#define LEDC_CH1_GAMMA_RANGE14_SCALE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE14_SCALE_S  11
/* LEDC_CH1_GAMMA_RANGE14_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE14_DUTY_CYCLE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE14_DUTY_CYCLE_M  ((LEDC_CH1_GAMMA_RANGE14_DUTY_CYCLE_V)<<(LEDC_CH1_GAMMA_RANGE14_DUTY_CYCLE_S))
#define LEDC_CH1_GAMMA_RANGE14_DUTY_CYCLE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE14_DUTY_CYCLE_S  1
/* LEDC_CH1_GAMMA_RANGE14_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH1_GAMMA_RANGE14_DUTY_INC    (BIT(0))
#define LEDC_CH1_GAMMA_RANGE14_DUTY_INC_M  (BIT(0))
#define LEDC_CH1_GAMMA_RANGE14_DUTY_INC_V  0x1
#define LEDC_CH1_GAMMA_RANGE14_DUTY_INC_S  0

#define LEDC_CH1_GAMMA_RANGE15_REG  (DR_REG_LEDC_BASE + 0x47c)
/* LEDC_CH1_GAMMA_RANGE15_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE15_DUTY_NUM    0x000003FF
#define LEDC_CH1_GAMMA_RANGE15_DUTY_NUM_M  ((LEDC_CH1_GAMMA_RANGE15_DUTY_NUM_V)<<(LEDC_CH1_GAMMA_RANGE15_DUTY_NUM_S))
#define LEDC_CH1_GAMMA_RANGE15_DUTY_NUM_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE15_DUTY_NUM_S  21
/* LEDC_CH1_GAMMA_RANGE15_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE15_SCALE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE15_SCALE_M  ((LEDC_CH1_GAMMA_RANGE15_SCALE_V)<<(LEDC_CH1_GAMMA_RANGE15_SCALE_S))
#define LEDC_CH1_GAMMA_RANGE15_SCALE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE15_SCALE_S  11
/* LEDC_CH1_GAMMA_RANGE15_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH1_GAMMA_RANGE15_DUTY_CYCLE    0x000003FF
#define LEDC_CH1_GAMMA_RANGE15_DUTY_CYCLE_M  ((LEDC_CH1_GAMMA_RANGE15_DUTY_CYCLE_V)<<(LEDC_CH1_GAMMA_RANGE15_DUTY_CYCLE_S))
#define LEDC_CH1_GAMMA_RANGE15_DUTY_CYCLE_V  0x3FF
#define LEDC_CH1_GAMMA_RANGE15_DUTY_CYCLE_S  1
/* LEDC_CH1_GAMMA_RANGE15_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH1_GAMMA_RANGE15_DUTY_INC    (BIT(0))
#define LEDC_CH1_GAMMA_RANGE15_DUTY_INC_M  (BIT(0))
#define LEDC_CH1_GAMMA_RANGE15_DUTY_INC_V  0x1
#define LEDC_CH1_GAMMA_RANGE15_DUTY_INC_S  0


#define LEDC_CH2_GAMMA_RANGE0_REG  (DR_REG_LEDC_BASE + 0x480)
/* LEDC_CH2_GAMMA_RANGE0_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE0_DUTY_NUM    0x000003FF
#define LEDC_CH2_GAMMA_RANGE0_DUTY_NUM_M  ((LEDC_CH2_GAMMA_RANGE0_DUTY_NUM_V)<<(LEDC_CH2_GAMMA_RANGE0_DUTY_NUM_S))
#define LEDC_CH2_GAMMA_RANGE0_DUTY_NUM_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE0_DUTY_NUM_S  21
/* LEDC_CH2_GAMMA_RANGE0_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE0_SCALE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE0_SCALE_M  ((LEDC_CH2_GAMMA_RANGE0_SCALE_V)<<(LEDC_CH2_GAMMA_RANGE0_SCALE_S))
#define LEDC_CH2_GAMMA_RANGE0_SCALE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE0_SCALE_S  11
/* LEDC_CH2_GAMMA_RANGE0_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE0_DUTY_CYCLE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE0_DUTY_CYCLE_M  ((LEDC_CH2_GAMMA_RANGE0_DUTY_CYCLE_V)<<(LEDC_CH2_GAMMA_RANGE0_DUTY_CYCLE_S))
#define LEDC_CH2_GAMMA_RANGE0_DUTY_CYCLE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE0_DUTY_CYCLE_S  1
/* LEDC_CH2_GAMMA_RANGE0_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH2_GAMMA_RANGE0_DUTY_INC    (BIT(0))
#define LEDC_CH2_GAMMA_RANGE0_DUTY_INC_M  (BIT(0))
#define LEDC_CH2_GAMMA_RANGE0_DUTY_INC_V  0x1
#define LEDC_CH2_GAMMA_RANGE0_DUTY_INC_S  0

#define LEDC_CH2_GAMMA_RANGE1_REG  (DR_REG_LEDC_BASE + 0x484)
/* LEDC_CH2_GAMMA_RANGE1_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE1_DUTY_NUM    0x000003FF
#define LEDC_CH2_GAMMA_RANGE1_DUTY_NUM_M  ((LEDC_CH2_GAMMA_RANGE1_DUTY_NUM_V)<<(LEDC_CH2_GAMMA_RANGE1_DUTY_NUM_S))
#define LEDC_CH2_GAMMA_RANGE1_DUTY_NUM_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE1_DUTY_NUM_S  21
/* LEDC_CH2_GAMMA_RANGE1_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE1_SCALE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE1_SCALE_M  ((LEDC_CH2_GAMMA_RANGE1_SCALE_V)<<(LEDC_CH2_GAMMA_RANGE1_SCALE_S))
#define LEDC_CH2_GAMMA_RANGE1_SCALE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE1_SCALE_S  11
/* LEDC_CH2_GAMMA_RANGE1_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE1_DUTY_CYCLE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE1_DUTY_CYCLE_M  ((LEDC_CH2_GAMMA_RANGE1_DUTY_CYCLE_V)<<(LEDC_CH2_GAMMA_RANGE1_DUTY_CYCLE_S))
#define LEDC_CH2_GAMMA_RANGE1_DUTY_CYCLE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE1_DUTY_CYCLE_S  1
/* LEDC_CH2_GAMMA_RANGE1_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH2_GAMMA_RANGE1_DUTY_INC    (BIT(0))
#define LEDC_CH2_GAMMA_RANGE1_DUTY_INC_M  (BIT(0))
#define LEDC_CH2_GAMMA_RANGE1_DUTY_INC_V  0x1
#define LEDC_CH2_GAMMA_RANGE1_DUTY_INC_S  0

#define LEDC_CH2_GAMMA_RANGE2_REG  (DR_REG_LEDC_BASE + 0x488)
/* LEDC_CH2_GAMMA_RANGE2_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE2_DUTY_NUM    0x000003FF
#define LEDC_CH2_GAMMA_RANGE2_DUTY_NUM_M  ((LEDC_CH2_GAMMA_RANGE2_DUTY_NUM_V)<<(LEDC_CH2_GAMMA_RANGE2_DUTY_NUM_S))
#define LEDC_CH2_GAMMA_RANGE2_DUTY_NUM_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE2_DUTY_NUM_S  21
/* LEDC_CH2_GAMMA_RANGE2_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE2_SCALE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE2_SCALE_M  ((LEDC_CH2_GAMMA_RANGE2_SCALE_V)<<(LEDC_CH2_GAMMA_RANGE2_SCALE_S))
#define LEDC_CH2_GAMMA_RANGE2_SCALE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE2_SCALE_S  11
/* LEDC_CH2_GAMMA_RANGE2_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE2_DUTY_CYCLE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE2_DUTY_CYCLE_M  ((LEDC_CH2_GAMMA_RANGE2_DUTY_CYCLE_V)<<(LEDC_CH2_GAMMA_RANGE2_DUTY_CYCLE_S))
#define LEDC_CH2_GAMMA_RANGE2_DUTY_CYCLE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE2_DUTY_CYCLE_S  1
/* LEDC_CH2_GAMMA_RANGE2_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH2_GAMMA_RANGE2_DUTY_INC    (BIT(0))
#define LEDC_CH2_GAMMA_RANGE2_DUTY_INC_M  (BIT(0))
#define LEDC_CH2_GAMMA_RANGE2_DUTY_INC_V  0x1
#define LEDC_CH2_GAMMA_RANGE2_DUTY_INC_S  0

#define LEDC_CH2_GAMMA_RANGE3_REG  (DR_REG_LEDC_BASE + 0x48c)
/* LEDC_CH2_GAMMA_RANGE3_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE3_DUTY_NUM    0x000003FF
#define LEDC_CH2_GAMMA_RANGE3_DUTY_NUM_M  ((LEDC_CH2_GAMMA_RANGE3_DUTY_NUM_V)<<(LEDC_CH2_GAMMA_RANGE3_DUTY_NUM_S))
#define LEDC_CH2_GAMMA_RANGE3_DUTY_NUM_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE3_DUTY_NUM_S  21
/* LEDC_CH2_GAMMA_RANGE3_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE3_SCALE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE3_SCALE_M  ((LEDC_CH2_GAMMA_RANGE3_SCALE_V)<<(LEDC_CH2_GAMMA_RANGE3_SCALE_S))
#define LEDC_CH2_GAMMA_RANGE3_SCALE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE3_SCALE_S  11
/* LEDC_CH2_GAMMA_RANGE3_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE3_DUTY_CYCLE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE3_DUTY_CYCLE_M  ((LEDC_CH2_GAMMA_RANGE3_DUTY_CYCLE_V)<<(LEDC_CH2_GAMMA_RANGE3_DUTY_CYCLE_S))
#define LEDC_CH2_GAMMA_RANGE3_DUTY_CYCLE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE3_DUTY_CYCLE_S  1
/* LEDC_CH2_GAMMA_RANGE3_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH2_GAMMA_RANGE3_DUTY_INC    (BIT(0))
#define LEDC_CH2_GAMMA_RANGE3_DUTY_INC_M  (BIT(0))
#define LEDC_CH2_GAMMA_RANGE3_DUTY_INC_V  0x1
#define LEDC_CH2_GAMMA_RANGE3_DUTY_INC_S  0

#define LEDC_CH2_GAMMA_RANGE4_REG  (DR_REG_LEDC_BASE + 0x490)
/* LEDC_CH2_GAMMA_RANGE4_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE4_DUTY_NUM    0x000003FF
#define LEDC_CH2_GAMMA_RANGE4_DUTY_NUM_M  ((LEDC_CH2_GAMMA_RANGE4_DUTY_NUM_V)<<(LEDC_CH2_GAMMA_RANGE4_DUTY_NUM_S))
#define LEDC_CH2_GAMMA_RANGE4_DUTY_NUM_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE4_DUTY_NUM_S  21
/* LEDC_CH2_GAMMA_RANGE4_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE4_SCALE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE4_SCALE_M  ((LEDC_CH2_GAMMA_RANGE4_SCALE_V)<<(LEDC_CH2_GAMMA_RANGE4_SCALE_S))
#define LEDC_CH2_GAMMA_RANGE4_SCALE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE4_SCALE_S  11
/* LEDC_CH2_GAMMA_RANGE4_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE4_DUTY_CYCLE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE4_DUTY_CYCLE_M  ((LEDC_CH2_GAMMA_RANGE4_DUTY_CYCLE_V)<<(LEDC_CH2_GAMMA_RANGE4_DUTY_CYCLE_S))
#define LEDC_CH2_GAMMA_RANGE4_DUTY_CYCLE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE4_DUTY_CYCLE_S  1
/* LEDC_CH2_GAMMA_RANGE4_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH2_GAMMA_RANGE4_DUTY_INC    (BIT(0))
#define LEDC_CH2_GAMMA_RANGE4_DUTY_INC_M  (BIT(0))
#define LEDC_CH2_GAMMA_RANGE4_DUTY_INC_V  0x1
#define LEDC_CH2_GAMMA_RANGE4_DUTY_INC_S  0

#define LEDC_CH2_GAMMA_RANGE5_REG  (DR_REG_LEDC_BASE + 0x494)
/* LEDC_CH2_GAMMA_RANGE5_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE5_DUTY_NUM    0x000003FF
#define LEDC_CH2_GAMMA_RANGE5_DUTY_NUM_M  ((LEDC_CH2_GAMMA_RANGE5_DUTY_NUM_V)<<(LEDC_CH2_GAMMA_RANGE5_DUTY_NUM_S))
#define LEDC_CH2_GAMMA_RANGE5_DUTY_NUM_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE5_DUTY_NUM_S  21
/* LEDC_CH2_GAMMA_RANGE5_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE5_SCALE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE5_SCALE_M  ((LEDC_CH2_GAMMA_RANGE5_SCALE_V)<<(LEDC_CH2_GAMMA_RANGE5_SCALE_S))
#define LEDC_CH2_GAMMA_RANGE5_SCALE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE5_SCALE_S  11
/* LEDC_CH2_GAMMA_RANGE5_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE5_DUTY_CYCLE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE5_DUTY_CYCLE_M  ((LEDC_CH2_GAMMA_RANGE5_DUTY_CYCLE_V)<<(LEDC_CH2_GAMMA_RANGE5_DUTY_CYCLE_S))
#define LEDC_CH2_GAMMA_RANGE5_DUTY_CYCLE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE5_DUTY_CYCLE_S  1
/* LEDC_CH2_GAMMA_RANGE5_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH2_GAMMA_RANGE5_DUTY_INC    (BIT(0))
#define LEDC_CH2_GAMMA_RANGE5_DUTY_INC_M  (BIT(0))
#define LEDC_CH2_GAMMA_RANGE5_DUTY_INC_V  0x1
#define LEDC_CH2_GAMMA_RANGE5_DUTY_INC_S  0

#define LEDC_CH2_GAMMA_RANGE6_REG  (DR_REG_LEDC_BASE + 0x498)
/* LEDC_CH2_GAMMA_RANGE6_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE6_DUTY_NUM    0x000003FF
#define LEDC_CH2_GAMMA_RANGE6_DUTY_NUM_M  ((LEDC_CH2_GAMMA_RANGE6_DUTY_NUM_V)<<(LEDC_CH2_GAMMA_RANGE6_DUTY_NUM_S))
#define LEDC_CH2_GAMMA_RANGE6_DUTY_NUM_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE6_DUTY_NUM_S  21
/* LEDC_CH2_GAMMA_RANGE6_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE6_SCALE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE6_SCALE_M  ((LEDC_CH2_GAMMA_RANGE6_SCALE_V)<<(LEDC_CH2_GAMMA_RANGE6_SCALE_S))
#define LEDC_CH2_GAMMA_RANGE6_SCALE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE6_SCALE_S  11
/* LEDC_CH2_GAMMA_RANGE6_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE6_DUTY_CYCLE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE6_DUTY_CYCLE_M  ((LEDC_CH2_GAMMA_RANGE6_DUTY_CYCLE_V)<<(LEDC_CH2_GAMMA_RANGE6_DUTY_CYCLE_S))
#define LEDC_CH2_GAMMA_RANGE6_DUTY_CYCLE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE6_DUTY_CYCLE_S  1
/* LEDC_CH2_GAMMA_RANGE6_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH2_GAMMA_RANGE6_DUTY_INC    (BIT(0))
#define LEDC_CH2_GAMMA_RANGE6_DUTY_INC_M  (BIT(0))
#define LEDC_CH2_GAMMA_RANGE6_DUTY_INC_V  0x1
#define LEDC_CH2_GAMMA_RANGE6_DUTY_INC_S  0

#define LEDC_CH2_GAMMA_RANGE7_REG  (DR_REG_LEDC_BASE + 0x49c)
/* LEDC_CH2_GAMMA_RANGE7_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE7_DUTY_NUM    0x000003FF
#define LEDC_CH2_GAMMA_RANGE7_DUTY_NUM_M  ((LEDC_CH2_GAMMA_RANGE7_DUTY_NUM_V)<<(LEDC_CH2_GAMMA_RANGE7_DUTY_NUM_S))
#define LEDC_CH2_GAMMA_RANGE7_DUTY_NUM_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE7_DUTY_NUM_S  21
/* LEDC_CH2_GAMMA_RANGE7_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE7_SCALE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE7_SCALE_M  ((LEDC_CH2_GAMMA_RANGE7_SCALE_V)<<(LEDC_CH2_GAMMA_RANGE7_SCALE_S))
#define LEDC_CH2_GAMMA_RANGE7_SCALE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE7_SCALE_S  11
/* LEDC_CH2_GAMMA_RANGE7_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE7_DUTY_CYCLE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE7_DUTY_CYCLE_M  ((LEDC_CH2_GAMMA_RANGE7_DUTY_CYCLE_V)<<(LEDC_CH2_GAMMA_RANGE7_DUTY_CYCLE_S))
#define LEDC_CH2_GAMMA_RANGE7_DUTY_CYCLE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE7_DUTY_CYCLE_S  1
/* LEDC_CH2_GAMMA_RANGE7_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH2_GAMMA_RANGE7_DUTY_INC    (BIT(0))
#define LEDC_CH2_GAMMA_RANGE7_DUTY_INC_M  (BIT(0))
#define LEDC_CH2_GAMMA_RANGE7_DUTY_INC_V  0x1
#define LEDC_CH2_GAMMA_RANGE7_DUTY_INC_S  0

#define LEDC_CH2_GAMMA_RANGE8_REG  (DR_REG_LEDC_BASE + 0x4a0)
/* LEDC_CH2_GAMMA_RANGE8_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE8_DUTY_NUM    0x000003FF
#define LEDC_CH2_GAMMA_RANGE8_DUTY_NUM_M  ((LEDC_CH2_GAMMA_RANGE8_DUTY_NUM_V)<<(LEDC_CH2_GAMMA_RANGE8_DUTY_NUM_S))
#define LEDC_CH2_GAMMA_RANGE8_DUTY_NUM_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE8_DUTY_NUM_S  21
/* LEDC_CH2_GAMMA_RANGE8_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE8_SCALE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE8_SCALE_M  ((LEDC_CH2_GAMMA_RANGE8_SCALE_V)<<(LEDC_CH2_GAMMA_RANGE8_SCALE_S))
#define LEDC_CH2_GAMMA_RANGE8_SCALE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE8_SCALE_S  11
/* LEDC_CH2_GAMMA_RANGE8_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE8_DUTY_CYCLE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE8_DUTY_CYCLE_M  ((LEDC_CH2_GAMMA_RANGE8_DUTY_CYCLE_V)<<(LEDC_CH2_GAMMA_RANGE8_DUTY_CYCLE_S))
#define LEDC_CH2_GAMMA_RANGE8_DUTY_CYCLE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE8_DUTY_CYCLE_S  1
/* LEDC_CH2_GAMMA_RANGE8_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH2_GAMMA_RANGE8_DUTY_INC    (BIT(0))
#define LEDC_CH2_GAMMA_RANGE8_DUTY_INC_M  (BIT(0))
#define LEDC_CH2_GAMMA_RANGE8_DUTY_INC_V  0x1
#define LEDC_CH2_GAMMA_RANGE8_DUTY_INC_S  0

#define LEDC_CH2_GAMMA_RANGE9_REG  (DR_REG_LEDC_BASE + 0x4a4)
/* LEDC_CH2_GAMMA_RANGE9_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE9_DUTY_NUM    0x000003FF
#define LEDC_CH2_GAMMA_RANGE9_DUTY_NUM_M  ((LEDC_CH2_GAMMA_RANGE9_DUTY_NUM_V)<<(LEDC_CH2_GAMMA_RANGE9_DUTY_NUM_S))
#define LEDC_CH2_GAMMA_RANGE9_DUTY_NUM_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE9_DUTY_NUM_S  21
/* LEDC_CH2_GAMMA_RANGE9_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE9_SCALE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE9_SCALE_M  ((LEDC_CH2_GAMMA_RANGE9_SCALE_V)<<(LEDC_CH2_GAMMA_RANGE9_SCALE_S))
#define LEDC_CH2_GAMMA_RANGE9_SCALE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE9_SCALE_S  11
/* LEDC_CH2_GAMMA_RANGE9_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE9_DUTY_CYCLE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE9_DUTY_CYCLE_M  ((LEDC_CH2_GAMMA_RANGE9_DUTY_CYCLE_V)<<(LEDC_CH2_GAMMA_RANGE9_DUTY_CYCLE_S))
#define LEDC_CH2_GAMMA_RANGE9_DUTY_CYCLE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE9_DUTY_CYCLE_S  1
/* LEDC_CH2_GAMMA_RANGE9_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH2_GAMMA_RANGE9_DUTY_INC    (BIT(0))
#define LEDC_CH2_GAMMA_RANGE9_DUTY_INC_M  (BIT(0))
#define LEDC_CH2_GAMMA_RANGE9_DUTY_INC_V  0x1
#define LEDC_CH2_GAMMA_RANGE9_DUTY_INC_S  0

#define LEDC_CH2_GAMMA_RANGE10_REG  (DR_REG_LEDC_BASE + 0x4a8)
/* LEDC_CH2_GAMMA_RANGE10_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE10_DUTY_NUM    0x000003FF
#define LEDC_CH2_GAMMA_RANGE10_DUTY_NUM_M  ((LEDC_CH2_GAMMA_RANGE10_DUTY_NUM_V)<<(LEDC_CH2_GAMMA_RANGE10_DUTY_NUM_S))
#define LEDC_CH2_GAMMA_RANGE10_DUTY_NUM_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE10_DUTY_NUM_S  21
/* LEDC_CH2_GAMMA_RANGE10_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE10_SCALE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE10_SCALE_M  ((LEDC_CH2_GAMMA_RANGE10_SCALE_V)<<(LEDC_CH2_GAMMA_RANGE10_SCALE_S))
#define LEDC_CH2_GAMMA_RANGE10_SCALE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE10_SCALE_S  11
/* LEDC_CH2_GAMMA_RANGE10_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE10_DUTY_CYCLE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE10_DUTY_CYCLE_M  ((LEDC_CH2_GAMMA_RANGE10_DUTY_CYCLE_V)<<(LEDC_CH2_GAMMA_RANGE10_DUTY_CYCLE_S))
#define LEDC_CH2_GAMMA_RANGE10_DUTY_CYCLE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE10_DUTY_CYCLE_S  1
/* LEDC_CH2_GAMMA_RANGE10_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH2_GAMMA_RANGE10_DUTY_INC    (BIT(0))
#define LEDC_CH2_GAMMA_RANGE10_DUTY_INC_M  (BIT(0))
#define LEDC_CH2_GAMMA_RANGE10_DUTY_INC_V  0x1
#define LEDC_CH2_GAMMA_RANGE10_DUTY_INC_S  0

#define LEDC_CH2_GAMMA_RANGE11_REG  (DR_REG_LEDC_BASE + 0x4ac)
/* LEDC_CH2_GAMMA_RANGE11_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE11_DUTY_NUM    0x000003FF
#define LEDC_CH2_GAMMA_RANGE11_DUTY_NUM_M  ((LEDC_CH2_GAMMA_RANGE11_DUTY_NUM_V)<<(LEDC_CH2_GAMMA_RANGE11_DUTY_NUM_S))
#define LEDC_CH2_GAMMA_RANGE11_DUTY_NUM_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE11_DUTY_NUM_S  21
/* LEDC_CH2_GAMMA_RANGE11_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE11_SCALE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE11_SCALE_M  ((LEDC_CH2_GAMMA_RANGE11_SCALE_V)<<(LEDC_CH2_GAMMA_RANGE11_SCALE_S))
#define LEDC_CH2_GAMMA_RANGE11_SCALE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE11_SCALE_S  11
/* LEDC_CH2_GAMMA_RANGE11_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE11_DUTY_CYCLE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE11_DUTY_CYCLE_M  ((LEDC_CH2_GAMMA_RANGE11_DUTY_CYCLE_V)<<(LEDC_CH2_GAMMA_RANGE11_DUTY_CYCLE_S))
#define LEDC_CH2_GAMMA_RANGE11_DUTY_CYCLE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE11_DUTY_CYCLE_S  1
/* LEDC_CH2_GAMMA_RANGE11_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH2_GAMMA_RANGE11_DUTY_INC    (BIT(0))
#define LEDC_CH2_GAMMA_RANGE11_DUTY_INC_M  (BIT(0))
#define LEDC_CH2_GAMMA_RANGE11_DUTY_INC_V  0x1
#define LEDC_CH2_GAMMA_RANGE11_DUTY_INC_S  0

#define LEDC_CH2_GAMMA_RANGE12_REG  (DR_REG_LEDC_BASE + 0x4b0)
/* LEDC_CH2_GAMMA_RANGE12_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE12_DUTY_NUM    0x000003FF
#define LEDC_CH2_GAMMA_RANGE12_DUTY_NUM_M  ((LEDC_CH2_GAMMA_RANGE12_DUTY_NUM_V)<<(LEDC_CH2_GAMMA_RANGE12_DUTY_NUM_S))
#define LEDC_CH2_GAMMA_RANGE12_DUTY_NUM_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE12_DUTY_NUM_S  21
/* LEDC_CH2_GAMMA_RANGE12_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE12_SCALE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE12_SCALE_M  ((LEDC_CH2_GAMMA_RANGE12_SCALE_V)<<(LEDC_CH2_GAMMA_RANGE12_SCALE_S))
#define LEDC_CH2_GAMMA_RANGE12_SCALE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE12_SCALE_S  11
/* LEDC_CH2_GAMMA_RANGE12_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE12_DUTY_CYCLE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE12_DUTY_CYCLE_M  ((LEDC_CH2_GAMMA_RANGE12_DUTY_CYCLE_V)<<(LEDC_CH2_GAMMA_RANGE12_DUTY_CYCLE_S))
#define LEDC_CH2_GAMMA_RANGE12_DUTY_CYCLE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE12_DUTY_CYCLE_S  1
/* LEDC_CH2_GAMMA_RANGE12_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH2_GAMMA_RANGE12_DUTY_INC    (BIT(0))
#define LEDC_CH2_GAMMA_RANGE12_DUTY_INC_M  (BIT(0))
#define LEDC_CH2_GAMMA_RANGE12_DUTY_INC_V  0x1
#define LEDC_CH2_GAMMA_RANGE12_DUTY_INC_S  0

#define LEDC_CH2_GAMMA_RANGE13_REG  (DR_REG_LEDC_BASE + 0x4b4)
/* LEDC_CH2_GAMMA_RANGE13_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE13_DUTY_NUM    0x000003FF
#define LEDC_CH2_GAMMA_RANGE13_DUTY_NUM_M  ((LEDC_CH2_GAMMA_RANGE13_DUTY_NUM_V)<<(LEDC_CH2_GAMMA_RANGE13_DUTY_NUM_S))
#define LEDC_CH2_GAMMA_RANGE13_DUTY_NUM_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE13_DUTY_NUM_S  21
/* LEDC_CH2_GAMMA_RANGE13_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE13_SCALE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE13_SCALE_M  ((LEDC_CH2_GAMMA_RANGE13_SCALE_V)<<(LEDC_CH2_GAMMA_RANGE13_SCALE_S))
#define LEDC_CH2_GAMMA_RANGE13_SCALE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE13_SCALE_S  11
/* LEDC_CH2_GAMMA_RANGE13_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE13_DUTY_CYCLE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE13_DUTY_CYCLE_M  ((LEDC_CH2_GAMMA_RANGE13_DUTY_CYCLE_V)<<(LEDC_CH2_GAMMA_RANGE13_DUTY_CYCLE_S))
#define LEDC_CH2_GAMMA_RANGE13_DUTY_CYCLE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE13_DUTY_CYCLE_S  1
/* LEDC_CH2_GAMMA_RANGE13_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH2_GAMMA_RANGE13_DUTY_INC    (BIT(0))
#define LEDC_CH2_GAMMA_RANGE13_DUTY_INC_M  (BIT(0))
#define LEDC_CH2_GAMMA_RANGE13_DUTY_INC_V  0x1
#define LEDC_CH2_GAMMA_RANGE13_DUTY_INC_S  0

#define LEDC_CH2_GAMMA_RANGE14_REG  (DR_REG_LEDC_BASE + 0x4b8)
/* LEDC_CH2_GAMMA_RANGE14_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE14_DUTY_NUM    0x000003FF
#define LEDC_CH2_GAMMA_RANGE14_DUTY_NUM_M  ((LEDC_CH2_GAMMA_RANGE14_DUTY_NUM_V)<<(LEDC_CH2_GAMMA_RANGE14_DUTY_NUM_S))
#define LEDC_CH2_GAMMA_RANGE14_DUTY_NUM_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE14_DUTY_NUM_S  21
/* LEDC_CH2_GAMMA_RANGE14_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE14_SCALE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE14_SCALE_M  ((LEDC_CH2_GAMMA_RANGE14_SCALE_V)<<(LEDC_CH2_GAMMA_RANGE14_SCALE_S))
#define LEDC_CH2_GAMMA_RANGE14_SCALE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE14_SCALE_S  11
/* LEDC_CH2_GAMMA_RANGE14_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE14_DUTY_CYCLE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE14_DUTY_CYCLE_M  ((LEDC_CH2_GAMMA_RANGE14_DUTY_CYCLE_V)<<(LEDC_CH2_GAMMA_RANGE14_DUTY_CYCLE_S))
#define LEDC_CH2_GAMMA_RANGE14_DUTY_CYCLE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE14_DUTY_CYCLE_S  1
/* LEDC_CH2_GAMMA_RANGE14_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH2_GAMMA_RANGE14_DUTY_INC    (BIT(0))
#define LEDC_CH2_GAMMA_RANGE14_DUTY_INC_M  (BIT(0))
#define LEDC_CH2_GAMMA_RANGE14_DUTY_INC_V  0x1
#define LEDC_CH2_GAMMA_RANGE14_DUTY_INC_S  0

#define LEDC_CH2_GAMMA_RANGE15_REG  (DR_REG_LEDC_BASE + 0x4bc)
/* LEDC_CH2_GAMMA_RANGE15_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE15_DUTY_NUM    0x000003FF
#define LEDC_CH2_GAMMA_RANGE15_DUTY_NUM_M  ((LEDC_CH2_GAMMA_RANGE15_DUTY_NUM_V)<<(LEDC_CH2_GAMMA_RANGE15_DUTY_NUM_S))
#define LEDC_CH2_GAMMA_RANGE15_DUTY_NUM_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE15_DUTY_NUM_S  21
/* LEDC_CH2_GAMMA_RANGE15_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE15_SCALE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE15_SCALE_M  ((LEDC_CH2_GAMMA_RANGE15_SCALE_V)<<(LEDC_CH2_GAMMA_RANGE15_SCALE_S))
#define LEDC_CH2_GAMMA_RANGE15_SCALE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE15_SCALE_S  11
/* LEDC_CH2_GAMMA_RANGE15_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH2_GAMMA_RANGE15_DUTY_CYCLE    0x000003FF
#define LEDC_CH2_GAMMA_RANGE15_DUTY_CYCLE_M  ((LEDC_CH2_GAMMA_RANGE15_DUTY_CYCLE_V)<<(LEDC_CH2_GAMMA_RANGE15_DUTY_CYCLE_S))
#define LEDC_CH2_GAMMA_RANGE15_DUTY_CYCLE_V  0x3FF
#define LEDC_CH2_GAMMA_RANGE15_DUTY_CYCLE_S  1
/* LEDC_CH2_GAMMA_RANGE15_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH2_GAMMA_RANGE15_DUTY_INC    (BIT(0))
#define LEDC_CH2_GAMMA_RANGE15_DUTY_INC_M  (BIT(0))
#define LEDC_CH2_GAMMA_RANGE15_DUTY_INC_V  0x1
#define LEDC_CH2_GAMMA_RANGE15_DUTY_INC_S  0


#define LEDC_CH3_GAMMA_RANGE0_REG  (DR_REG_LEDC_BASE + 0x4c0)
/* LEDC_CH3_GAMMA_RANGE0_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE0_DUTY_NUM    0x000003FF
#define LEDC_CH3_GAMMA_RANGE0_DUTY_NUM_M  ((LEDC_CH3_GAMMA_RANGE0_DUTY_NUM_V)<<(LEDC_CH3_GAMMA_RANGE0_DUTY_NUM_S))
#define LEDC_CH3_GAMMA_RANGE0_DUTY_NUM_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE0_DUTY_NUM_S  21
/* LEDC_CH3_GAMMA_RANGE0_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE0_SCALE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE0_SCALE_M  ((LEDC_CH3_GAMMA_RANGE0_SCALE_V)<<(LEDC_CH3_GAMMA_RANGE0_SCALE_S))
#define LEDC_CH3_GAMMA_RANGE0_SCALE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE0_SCALE_S  11
/* LEDC_CH3_GAMMA_RANGE0_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE0_DUTY_CYCLE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE0_DUTY_CYCLE_M  ((LEDC_CH3_GAMMA_RANGE0_DUTY_CYCLE_V)<<(LEDC_CH3_GAMMA_RANGE0_DUTY_CYCLE_S))
#define LEDC_CH3_GAMMA_RANGE0_DUTY_CYCLE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE0_DUTY_CYCLE_S  1
/* LEDC_CH3_GAMMA_RANGE0_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH3_GAMMA_RANGE0_DUTY_INC    (BIT(0))
#define LEDC_CH3_GAMMA_RANGE0_DUTY_INC_M  (BIT(0))
#define LEDC_CH3_GAMMA_RANGE0_DUTY_INC_V  0x1
#define LEDC_CH3_GAMMA_RANGE0_DUTY_INC_S  0

#define LEDC_CH3_GAMMA_RANGE1_REG  (DR_REG_LEDC_BASE + 0x4c4)
/* LEDC_CH3_GAMMA_RANGE1_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE1_DUTY_NUM    0x000003FF
#define LEDC_CH3_GAMMA_RANGE1_DUTY_NUM_M  ((LEDC_CH3_GAMMA_RANGE1_DUTY_NUM_V)<<(LEDC_CH3_GAMMA_RANGE1_DUTY_NUM_S))
#define LEDC_CH3_GAMMA_RANGE1_DUTY_NUM_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE1_DUTY_NUM_S  21
/* LEDC_CH3_GAMMA_RANGE1_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE1_SCALE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE1_SCALE_M  ((LEDC_CH3_GAMMA_RANGE1_SCALE_V)<<(LEDC_CH3_GAMMA_RANGE1_SCALE_S))
#define LEDC_CH3_GAMMA_RANGE1_SCALE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE1_SCALE_S  11
/* LEDC_CH3_GAMMA_RANGE1_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE1_DUTY_CYCLE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE1_DUTY_CYCLE_M  ((LEDC_CH3_GAMMA_RANGE1_DUTY_CYCLE_V)<<(LEDC_CH3_GAMMA_RANGE1_DUTY_CYCLE_S))
#define LEDC_CH3_GAMMA_RANGE1_DUTY_CYCLE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE1_DUTY_CYCLE_S  1
/* LEDC_CH3_GAMMA_RANGE1_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH3_GAMMA_RANGE1_DUTY_INC    (BIT(0))
#define LEDC_CH3_GAMMA_RANGE1_DUTY_INC_M  (BIT(0))
#define LEDC_CH3_GAMMA_RANGE1_DUTY_INC_V  0x1
#define LEDC_CH3_GAMMA_RANGE1_DUTY_INC_S  0

#define LEDC_CH3_GAMMA_RANGE2_REG  (DR_REG_LEDC_BASE + 0x4c8)
/* LEDC_CH3_GAMMA_RANGE2_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE2_DUTY_NUM    0x000003FF
#define LEDC_CH3_GAMMA_RANGE2_DUTY_NUM_M  ((LEDC_CH3_GAMMA_RANGE2_DUTY_NUM_V)<<(LEDC_CH3_GAMMA_RANGE2_DUTY_NUM_S))
#define LEDC_CH3_GAMMA_RANGE2_DUTY_NUM_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE2_DUTY_NUM_S  21
/* LEDC_CH3_GAMMA_RANGE2_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE2_SCALE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE2_SCALE_M  ((LEDC_CH3_GAMMA_RANGE2_SCALE_V)<<(LEDC_CH3_GAMMA_RANGE2_SCALE_S))
#define LEDC_CH3_GAMMA_RANGE2_SCALE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE2_SCALE_S  11
/* LEDC_CH3_GAMMA_RANGE2_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE2_DUTY_CYCLE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE2_DUTY_CYCLE_M  ((LEDC_CH3_GAMMA_RANGE2_DUTY_CYCLE_V)<<(LEDC_CH3_GAMMA_RANGE2_DUTY_CYCLE_S))
#define LEDC_CH3_GAMMA_RANGE2_DUTY_CYCLE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE2_DUTY_CYCLE_S  1
/* LEDC_CH3_GAMMA_RANGE2_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH3_GAMMA_RANGE2_DUTY_INC    (BIT(0))
#define LEDC_CH3_GAMMA_RANGE2_DUTY_INC_M  (BIT(0))
#define LEDC_CH3_GAMMA_RANGE2_DUTY_INC_V  0x1
#define LEDC_CH3_GAMMA_RANGE2_DUTY_INC_S  0

#define LEDC_CH3_GAMMA_RANGE3_REG  (DR_REG_LEDC_BASE + 0x4cc)
/* LEDC_CH3_GAMMA_RANGE3_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE3_DUTY_NUM    0x000003FF
#define LEDC_CH3_GAMMA_RANGE3_DUTY_NUM_M  ((LEDC_CH3_GAMMA_RANGE3_DUTY_NUM_V)<<(LEDC_CH3_GAMMA_RANGE3_DUTY_NUM_S))
#define LEDC_CH3_GAMMA_RANGE3_DUTY_NUM_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE3_DUTY_NUM_S  21
/* LEDC_CH3_GAMMA_RANGE3_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE3_SCALE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE3_SCALE_M  ((LEDC_CH3_GAMMA_RANGE3_SCALE_V)<<(LEDC_CH3_GAMMA_RANGE3_SCALE_S))
#define LEDC_CH3_GAMMA_RANGE3_SCALE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE3_SCALE_S  11
/* LEDC_CH3_GAMMA_RANGE3_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE3_DUTY_CYCLE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE3_DUTY_CYCLE_M  ((LEDC_CH3_GAMMA_RANGE3_DUTY_CYCLE_V)<<(LEDC_CH3_GAMMA_RANGE3_DUTY_CYCLE_S))
#define LEDC_CH3_GAMMA_RANGE3_DUTY_CYCLE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE3_DUTY_CYCLE_S  1
/* LEDC_CH3_GAMMA_RANGE3_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH3_GAMMA_RANGE3_DUTY_INC    (BIT(0))
#define LEDC_CH3_GAMMA_RANGE3_DUTY_INC_M  (BIT(0))
#define LEDC_CH3_GAMMA_RANGE3_DUTY_INC_V  0x1
#define LEDC_CH3_GAMMA_RANGE3_DUTY_INC_S  0

#define LEDC_CH3_GAMMA_RANGE4_REG  (DR_REG_LEDC_BASE + 0x4d0)
/* LEDC_CH3_GAMMA_RANGE4_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE4_DUTY_NUM    0x000003FF
#define LEDC_CH3_GAMMA_RANGE4_DUTY_NUM_M  ((LEDC_CH3_GAMMA_RANGE4_DUTY_NUM_V)<<(LEDC_CH3_GAMMA_RANGE4_DUTY_NUM_S))
#define LEDC_CH3_GAMMA_RANGE4_DUTY_NUM_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE4_DUTY_NUM_S  21
/* LEDC_CH3_GAMMA_RANGE4_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE4_SCALE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE4_SCALE_M  ((LEDC_CH3_GAMMA_RANGE4_SCALE_V)<<(LEDC_CH3_GAMMA_RANGE4_SCALE_S))
#define LEDC_CH3_GAMMA_RANGE4_SCALE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE4_SCALE_S  11
/* LEDC_CH3_GAMMA_RANGE4_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE4_DUTY_CYCLE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE4_DUTY_CYCLE_M  ((LEDC_CH3_GAMMA_RANGE4_DUTY_CYCLE_V)<<(LEDC_CH3_GAMMA_RANGE4_DUTY_CYCLE_S))
#define LEDC_CH3_GAMMA_RANGE4_DUTY_CYCLE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE4_DUTY_CYCLE_S  1
/* LEDC_CH3_GAMMA_RANGE4_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH3_GAMMA_RANGE4_DUTY_INC    (BIT(0))
#define LEDC_CH3_GAMMA_RANGE4_DUTY_INC_M  (BIT(0))
#define LEDC_CH3_GAMMA_RANGE4_DUTY_INC_V  0x1
#define LEDC_CH3_GAMMA_RANGE4_DUTY_INC_S  0

#define LEDC_CH3_GAMMA_RANGE5_REG  (DR_REG_LEDC_BASE + 0x4d4)
/* LEDC_CH3_GAMMA_RANGE5_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE5_DUTY_NUM    0x000003FF
#define LEDC_CH3_GAMMA_RANGE5_DUTY_NUM_M  ((LEDC_CH3_GAMMA_RANGE5_DUTY_NUM_V)<<(LEDC_CH3_GAMMA_RANGE5_DUTY_NUM_S))
#define LEDC_CH3_GAMMA_RANGE5_DUTY_NUM_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE5_DUTY_NUM_S  21
/* LEDC_CH3_GAMMA_RANGE5_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE5_SCALE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE5_SCALE_M  ((LEDC_CH3_GAMMA_RANGE5_SCALE_V)<<(LEDC_CH3_GAMMA_RANGE5_SCALE_S))
#define LEDC_CH3_GAMMA_RANGE5_SCALE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE5_SCALE_S  11
/* LEDC_CH3_GAMMA_RANGE5_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE5_DUTY_CYCLE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE5_DUTY_CYCLE_M  ((LEDC_CH3_GAMMA_RANGE5_DUTY_CYCLE_V)<<(LEDC_CH3_GAMMA_RANGE5_DUTY_CYCLE_S))
#define LEDC_CH3_GAMMA_RANGE5_DUTY_CYCLE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE5_DUTY_CYCLE_S  1
/* LEDC_CH3_GAMMA_RANGE5_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH3_GAMMA_RANGE5_DUTY_INC    (BIT(0))
#define LEDC_CH3_GAMMA_RANGE5_DUTY_INC_M  (BIT(0))
#define LEDC_CH3_GAMMA_RANGE5_DUTY_INC_V  0x1
#define LEDC_CH3_GAMMA_RANGE5_DUTY_INC_S  0

#define LEDC_CH3_GAMMA_RANGE6_REG  (DR_REG_LEDC_BASE + 0x4d8)
/* LEDC_CH3_GAMMA_RANGE6_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE6_DUTY_NUM    0x000003FF
#define LEDC_CH3_GAMMA_RANGE6_DUTY_NUM_M  ((LEDC_CH3_GAMMA_RANGE6_DUTY_NUM_V)<<(LEDC_CH3_GAMMA_RANGE6_DUTY_NUM_S))
#define LEDC_CH3_GAMMA_RANGE6_DUTY_NUM_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE6_DUTY_NUM_S  21
/* LEDC_CH3_GAMMA_RANGE6_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE6_SCALE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE6_SCALE_M  ((LEDC_CH3_GAMMA_RANGE6_SCALE_V)<<(LEDC_CH3_GAMMA_RANGE6_SCALE_S))
#define LEDC_CH3_GAMMA_RANGE6_SCALE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE6_SCALE_S  11
/* LEDC_CH3_GAMMA_RANGE6_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE6_DUTY_CYCLE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE6_DUTY_CYCLE_M  ((LEDC_CH3_GAMMA_RANGE6_DUTY_CYCLE_V)<<(LEDC_CH3_GAMMA_RANGE6_DUTY_CYCLE_S))
#define LEDC_CH3_GAMMA_RANGE6_DUTY_CYCLE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE6_DUTY_CYCLE_S  1
/* LEDC_CH3_GAMMA_RANGE6_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH3_GAMMA_RANGE6_DUTY_INC    (BIT(0))
#define LEDC_CH3_GAMMA_RANGE6_DUTY_INC_M  (BIT(0))
#define LEDC_CH3_GAMMA_RANGE6_DUTY_INC_V  0x1
#define LEDC_CH3_GAMMA_RANGE6_DUTY_INC_S  0

#define LEDC_CH3_GAMMA_RANGE7_REG  (DR_REG_LEDC_BASE + 0x4dc)
/* LEDC_CH3_GAMMA_RANGE7_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE7_DUTY_NUM    0x000003FF
#define LEDC_CH3_GAMMA_RANGE7_DUTY_NUM_M  ((LEDC_CH3_GAMMA_RANGE7_DUTY_NUM_V)<<(LEDC_CH3_GAMMA_RANGE7_DUTY_NUM_S))
#define LEDC_CH3_GAMMA_RANGE7_DUTY_NUM_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE7_DUTY_NUM_S  21
/* LEDC_CH3_GAMMA_RANGE7_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE7_SCALE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE7_SCALE_M  ((LEDC_CH3_GAMMA_RANGE7_SCALE_V)<<(LEDC_CH3_GAMMA_RANGE7_SCALE_S))
#define LEDC_CH3_GAMMA_RANGE7_SCALE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE7_SCALE_S  11
/* LEDC_CH3_GAMMA_RANGE7_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE7_DUTY_CYCLE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE7_DUTY_CYCLE_M  ((LEDC_CH3_GAMMA_RANGE7_DUTY_CYCLE_V)<<(LEDC_CH3_GAMMA_RANGE7_DUTY_CYCLE_S))
#define LEDC_CH3_GAMMA_RANGE7_DUTY_CYCLE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE7_DUTY_CYCLE_S  1
/* LEDC_CH3_GAMMA_RANGE7_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH3_GAMMA_RANGE7_DUTY_INC    (BIT(0))
#define LEDC_CH3_GAMMA_RANGE7_DUTY_INC_M  (BIT(0))
#define LEDC_CH3_GAMMA_RANGE7_DUTY_INC_V  0x1
#define LEDC_CH3_GAMMA_RANGE7_DUTY_INC_S  0

#define LEDC_CH3_GAMMA_RANGE8_REG  (DR_REG_LEDC_BASE + 0x4e0)
/* LEDC_CH3_GAMMA_RANGE8_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE8_DUTY_NUM    0x000003FF
#define LEDC_CH3_GAMMA_RANGE8_DUTY_NUM_M  ((LEDC_CH3_GAMMA_RANGE8_DUTY_NUM_V)<<(LEDC_CH3_GAMMA_RANGE8_DUTY_NUM_S))
#define LEDC_CH3_GAMMA_RANGE8_DUTY_NUM_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE8_DUTY_NUM_S  21
/* LEDC_CH3_GAMMA_RANGE8_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE8_SCALE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE8_SCALE_M  ((LEDC_CH3_GAMMA_RANGE8_SCALE_V)<<(LEDC_CH3_GAMMA_RANGE8_SCALE_S))
#define LEDC_CH3_GAMMA_RANGE8_SCALE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE8_SCALE_S  11
/* LEDC_CH3_GAMMA_RANGE8_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE8_DUTY_CYCLE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE8_DUTY_CYCLE_M  ((LEDC_CH3_GAMMA_RANGE8_DUTY_CYCLE_V)<<(LEDC_CH3_GAMMA_RANGE8_DUTY_CYCLE_S))
#define LEDC_CH3_GAMMA_RANGE8_DUTY_CYCLE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE8_DUTY_CYCLE_S  1
/* LEDC_CH3_GAMMA_RANGE8_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH3_GAMMA_RANGE8_DUTY_INC    (BIT(0))
#define LEDC_CH3_GAMMA_RANGE8_DUTY_INC_M  (BIT(0))
#define LEDC_CH3_GAMMA_RANGE8_DUTY_INC_V  0x1
#define LEDC_CH3_GAMMA_RANGE8_DUTY_INC_S  0

#define LEDC_CH3_GAMMA_RANGE9_REG  (DR_REG_LEDC_BASE + 0x4e4)
/* LEDC_CH3_GAMMA_RANGE9_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE9_DUTY_NUM    0x000003FF
#define LEDC_CH3_GAMMA_RANGE9_DUTY_NUM_M  ((LEDC_CH3_GAMMA_RANGE9_DUTY_NUM_V)<<(LEDC_CH3_GAMMA_RANGE9_DUTY_NUM_S))
#define LEDC_CH3_GAMMA_RANGE9_DUTY_NUM_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE9_DUTY_NUM_S  21
/* LEDC_CH3_GAMMA_RANGE9_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE9_SCALE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE9_SCALE_M  ((LEDC_CH3_GAMMA_RANGE9_SCALE_V)<<(LEDC_CH3_GAMMA_RANGE9_SCALE_S))
#define LEDC_CH3_GAMMA_RANGE9_SCALE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE9_SCALE_S  11
/* LEDC_CH3_GAMMA_RANGE9_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE9_DUTY_CYCLE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE9_DUTY_CYCLE_M  ((LEDC_CH3_GAMMA_RANGE9_DUTY_CYCLE_V)<<(LEDC_CH3_GAMMA_RANGE9_DUTY_CYCLE_S))
#define LEDC_CH3_GAMMA_RANGE9_DUTY_CYCLE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE9_DUTY_CYCLE_S  1
/* LEDC_CH3_GAMMA_RANGE9_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH3_GAMMA_RANGE9_DUTY_INC    (BIT(0))
#define LEDC_CH3_GAMMA_RANGE9_DUTY_INC_M  (BIT(0))
#define LEDC_CH3_GAMMA_RANGE9_DUTY_INC_V  0x1
#define LEDC_CH3_GAMMA_RANGE9_DUTY_INC_S  0

#define LEDC_CH3_GAMMA_RANGE10_REG  (DR_REG_LEDC_BASE + 0x4e8)
/* LEDC_CH3_GAMMA_RANGE10_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE10_DUTY_NUM    0x000003FF
#define LEDC_CH3_GAMMA_RANGE10_DUTY_NUM_M  ((LEDC_CH3_GAMMA_RANGE10_DUTY_NUM_V)<<(LEDC_CH3_GAMMA_RANGE10_DUTY_NUM_S))
#define LEDC_CH3_GAMMA_RANGE10_DUTY_NUM_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE10_DUTY_NUM_S  21
/* LEDC_CH3_GAMMA_RANGE10_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE10_SCALE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE10_SCALE_M  ((LEDC_CH3_GAMMA_RANGE10_SCALE_V)<<(LEDC_CH3_GAMMA_RANGE10_SCALE_S))
#define LEDC_CH3_GAMMA_RANGE10_SCALE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE10_SCALE_S  11
/* LEDC_CH3_GAMMA_RANGE10_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE10_DUTY_CYCLE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE10_DUTY_CYCLE_M  ((LEDC_CH3_GAMMA_RANGE10_DUTY_CYCLE_V)<<(LEDC_CH3_GAMMA_RANGE10_DUTY_CYCLE_S))
#define LEDC_CH3_GAMMA_RANGE10_DUTY_CYCLE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE10_DUTY_CYCLE_S  1
/* LEDC_CH3_GAMMA_RANGE10_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH3_GAMMA_RANGE10_DUTY_INC    (BIT(0))
#define LEDC_CH3_GAMMA_RANGE10_DUTY_INC_M  (BIT(0))
#define LEDC_CH3_GAMMA_RANGE10_DUTY_INC_V  0x1
#define LEDC_CH3_GAMMA_RANGE10_DUTY_INC_S  0

#define LEDC_CH3_GAMMA_RANGE11_REG  (DR_REG_LEDC_BASE + 0x4ec)
/* LEDC_CH3_GAMMA_RANGE11_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE11_DUTY_NUM    0x000003FF
#define LEDC_CH3_GAMMA_RANGE11_DUTY_NUM_M  ((LEDC_CH3_GAMMA_RANGE11_DUTY_NUM_V)<<(LEDC_CH3_GAMMA_RANGE11_DUTY_NUM_S))
#define LEDC_CH3_GAMMA_RANGE11_DUTY_NUM_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE11_DUTY_NUM_S  21
/* LEDC_CH3_GAMMA_RANGE11_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE11_SCALE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE11_SCALE_M  ((LEDC_CH3_GAMMA_RANGE11_SCALE_V)<<(LEDC_CH3_GAMMA_RANGE11_SCALE_S))
#define LEDC_CH3_GAMMA_RANGE11_SCALE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE11_SCALE_S  11
/* LEDC_CH3_GAMMA_RANGE11_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE11_DUTY_CYCLE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE11_DUTY_CYCLE_M  ((LEDC_CH3_GAMMA_RANGE11_DUTY_CYCLE_V)<<(LEDC_CH3_GAMMA_RANGE11_DUTY_CYCLE_S))
#define LEDC_CH3_GAMMA_RANGE11_DUTY_CYCLE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE11_DUTY_CYCLE_S  1
/* LEDC_CH3_GAMMA_RANGE11_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH3_GAMMA_RANGE11_DUTY_INC    (BIT(0))
#define LEDC_CH3_GAMMA_RANGE11_DUTY_INC_M  (BIT(0))
#define LEDC_CH3_GAMMA_RANGE11_DUTY_INC_V  0x1
#define LEDC_CH3_GAMMA_RANGE11_DUTY_INC_S  0

#define LEDC_CH3_GAMMA_RANGE12_REG  (DR_REG_LEDC_BASE + 0x4f0)
/* LEDC_CH3_GAMMA_RANGE12_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE12_DUTY_NUM    0x000003FF
#define LEDC_CH3_GAMMA_RANGE12_DUTY_NUM_M  ((LEDC_CH3_GAMMA_RANGE12_DUTY_NUM_V)<<(LEDC_CH3_GAMMA_RANGE12_DUTY_NUM_S))
#define LEDC_CH3_GAMMA_RANGE12_DUTY_NUM_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE12_DUTY_NUM_S  21
/* LEDC_CH3_GAMMA_RANGE12_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE12_SCALE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE12_SCALE_M  ((LEDC_CH3_GAMMA_RANGE12_SCALE_V)<<(LEDC_CH3_GAMMA_RANGE12_SCALE_S))
#define LEDC_CH3_GAMMA_RANGE12_SCALE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE12_SCALE_S  11
/* LEDC_CH3_GAMMA_RANGE12_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE12_DUTY_CYCLE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE12_DUTY_CYCLE_M  ((LEDC_CH3_GAMMA_RANGE12_DUTY_CYCLE_V)<<(LEDC_CH3_GAMMA_RANGE12_DUTY_CYCLE_S))
#define LEDC_CH3_GAMMA_RANGE12_DUTY_CYCLE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE12_DUTY_CYCLE_S  1
/* LEDC_CH3_GAMMA_RANGE12_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH3_GAMMA_RANGE12_DUTY_INC    (BIT(0))
#define LEDC_CH3_GAMMA_RANGE12_DUTY_INC_M  (BIT(0))
#define LEDC_CH3_GAMMA_RANGE12_DUTY_INC_V  0x1
#define LEDC_CH3_GAMMA_RANGE12_DUTY_INC_S  0

#define LEDC_CH3_GAMMA_RANGE13_REG  (DR_REG_LEDC_BASE + 0x4f4)
/* LEDC_CH3_GAMMA_RANGE13_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE13_DUTY_NUM    0x000003FF
#define LEDC_CH3_GAMMA_RANGE13_DUTY_NUM_M  ((LEDC_CH3_GAMMA_RANGE13_DUTY_NUM_V)<<(LEDC_CH3_GAMMA_RANGE13_DUTY_NUM_S))
#define LEDC_CH3_GAMMA_RANGE13_DUTY_NUM_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE13_DUTY_NUM_S  21
/* LEDC_CH3_GAMMA_RANGE13_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE13_SCALE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE13_SCALE_M  ((LEDC_CH3_GAMMA_RANGE13_SCALE_V)<<(LEDC_CH3_GAMMA_RANGE13_SCALE_S))
#define LEDC_CH3_GAMMA_RANGE13_SCALE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE13_SCALE_S  11
/* LEDC_CH3_GAMMA_RANGE13_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE13_DUTY_CYCLE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE13_DUTY_CYCLE_M  ((LEDC_CH3_GAMMA_RANGE13_DUTY_CYCLE_V)<<(LEDC_CH3_GAMMA_RANGE13_DUTY_CYCLE_S))
#define LEDC_CH3_GAMMA_RANGE13_DUTY_CYCLE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE13_DUTY_CYCLE_S  1
/* LEDC_CH3_GAMMA_RANGE13_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH3_GAMMA_RANGE13_DUTY_INC    (BIT(0))
#define LEDC_CH3_GAMMA_RANGE13_DUTY_INC_M  (BIT(0))
#define LEDC_CH3_GAMMA_RANGE13_DUTY_INC_V  0x1
#define LEDC_CH3_GAMMA_RANGE13_DUTY_INC_S  0

#define LEDC_CH3_GAMMA_RANGE14_REG  (DR_REG_LEDC_BASE + 0x4f8)
/* LEDC_CH3_GAMMA_RANGE14_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE14_DUTY_NUM    0x000003FF
#define LEDC_CH3_GAMMA_RANGE14_DUTY_NUM_M  ((LEDC_CH3_GAMMA_RANGE14_DUTY_NUM_V)<<(LEDC_CH3_GAMMA_RANGE14_DUTY_NUM_S))
#define LEDC_CH3_GAMMA_RANGE14_DUTY_NUM_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE14_DUTY_NUM_S  21
/* LEDC_CH3_GAMMA_RANGE14_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE14_SCALE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE14_SCALE_M  ((LEDC_CH3_GAMMA_RANGE14_SCALE_V)<<(LEDC_CH3_GAMMA_RANGE14_SCALE_S))
#define LEDC_CH3_GAMMA_RANGE14_SCALE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE14_SCALE_S  11
/* LEDC_CH3_GAMMA_RANGE14_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE14_DUTY_CYCLE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE14_DUTY_CYCLE_M  ((LEDC_CH3_GAMMA_RANGE14_DUTY_CYCLE_V)<<(LEDC_CH3_GAMMA_RANGE14_DUTY_CYCLE_S))
#define LEDC_CH3_GAMMA_RANGE14_DUTY_CYCLE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE14_DUTY_CYCLE_S  1
/* LEDC_CH3_GAMMA_RANGE14_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH3_GAMMA_RANGE14_DUTY_INC    (BIT(0))
#define LEDC_CH3_GAMMA_RANGE14_DUTY_INC_M  (BIT(0))
#define LEDC_CH3_GAMMA_RANGE14_DUTY_INC_V  0x1
#define LEDC_CH3_GAMMA_RANGE14_DUTY_INC_S  0

#define LEDC_CH3_GAMMA_RANGE15_REG  (DR_REG_LEDC_BASE + 0x4fc)
/* LEDC_CH3_GAMMA_RANGE15_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE15_DUTY_NUM    0x000003FF
#define LEDC_CH3_GAMMA_RANGE15_DUTY_NUM_M  ((LEDC_CH3_GAMMA_RANGE15_DUTY_NUM_V)<<(LEDC_CH3_GAMMA_RANGE15_DUTY_NUM_S))
#define LEDC_CH3_GAMMA_RANGE15_DUTY_NUM_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE15_DUTY_NUM_S  21
/* LEDC_CH3_GAMMA_RANGE15_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE15_SCALE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE15_SCALE_M  ((LEDC_CH3_GAMMA_RANGE15_SCALE_V)<<(LEDC_CH3_GAMMA_RANGE15_SCALE_S))
#define LEDC_CH3_GAMMA_RANGE15_SCALE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE15_SCALE_S  11
/* LEDC_CH3_GAMMA_RANGE15_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH3_GAMMA_RANGE15_DUTY_CYCLE    0x000003FF
#define LEDC_CH3_GAMMA_RANGE15_DUTY_CYCLE_M  ((LEDC_CH3_GAMMA_RANGE15_DUTY_CYCLE_V)<<(LEDC_CH3_GAMMA_RANGE15_DUTY_CYCLE_S))
#define LEDC_CH3_GAMMA_RANGE15_DUTY_CYCLE_V  0x3FF
#define LEDC_CH3_GAMMA_RANGE15_DUTY_CYCLE_S  1
/* LEDC_CH3_GAMMA_RANGE15_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH3_GAMMA_RANGE15_DUTY_INC    (BIT(0))
#define LEDC_CH3_GAMMA_RANGE15_DUTY_INC_M  (BIT(0))
#define LEDC_CH3_GAMMA_RANGE15_DUTY_INC_V  0x1
#define LEDC_CH3_GAMMA_RANGE15_DUTY_INC_S  0


#define LEDC_CH4_GAMMA_RANGE0_REG  (DR_REG_LEDC_BASE + 0x500)
/* LEDC_CH4_GAMMA_RANGE0_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE0_DUTY_NUM    0x000003FF
#define LEDC_CH4_GAMMA_RANGE0_DUTY_NUM_M  ((LEDC_CH4_GAMMA_RANGE0_DUTY_NUM_V)<<(LEDC_CH4_GAMMA_RANGE0_DUTY_NUM_S))
#define LEDC_CH4_GAMMA_RANGE0_DUTY_NUM_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE0_DUTY_NUM_S  21
/* LEDC_CH4_GAMMA_RANGE0_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE0_SCALE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE0_SCALE_M  ((LEDC_CH4_GAMMA_RANGE0_SCALE_V)<<(LEDC_CH4_GAMMA_RANGE0_SCALE_S))
#define LEDC_CH4_GAMMA_RANGE0_SCALE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE0_SCALE_S  11
/* LEDC_CH4_GAMMA_RANGE0_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE0_DUTY_CYCLE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE0_DUTY_CYCLE_M  ((LEDC_CH4_GAMMA_RANGE0_DUTY_CYCLE_V)<<(LEDC_CH4_GAMMA_RANGE0_DUTY_CYCLE_S))
#define LEDC_CH4_GAMMA_RANGE0_DUTY_CYCLE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE0_DUTY_CYCLE_S  1
/* LEDC_CH4_GAMMA_RANGE0_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH4_GAMMA_RANGE0_DUTY_INC    (BIT(0))
#define LEDC_CH4_GAMMA_RANGE0_DUTY_INC_M  (BIT(0))
#define LEDC_CH4_GAMMA_RANGE0_DUTY_INC_V  0x1
#define LEDC_CH4_GAMMA_RANGE0_DUTY_INC_S  0

#define LEDC_CH4_GAMMA_RANGE1_REG  (DR_REG_LEDC_BASE + 0x504)
/* LEDC_CH4_GAMMA_RANGE1_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE1_DUTY_NUM    0x000003FF
#define LEDC_CH4_GAMMA_RANGE1_DUTY_NUM_M  ((LEDC_CH4_GAMMA_RANGE1_DUTY_NUM_V)<<(LEDC_CH4_GAMMA_RANGE1_DUTY_NUM_S))
#define LEDC_CH4_GAMMA_RANGE1_DUTY_NUM_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE1_DUTY_NUM_S  21
/* LEDC_CH4_GAMMA_RANGE1_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE1_SCALE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE1_SCALE_M  ((LEDC_CH4_GAMMA_RANGE1_SCALE_V)<<(LEDC_CH4_GAMMA_RANGE1_SCALE_S))
#define LEDC_CH4_GAMMA_RANGE1_SCALE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE1_SCALE_S  11
/* LEDC_CH4_GAMMA_RANGE1_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE1_DUTY_CYCLE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE1_DUTY_CYCLE_M  ((LEDC_CH4_GAMMA_RANGE1_DUTY_CYCLE_V)<<(LEDC_CH4_GAMMA_RANGE1_DUTY_CYCLE_S))
#define LEDC_CH4_GAMMA_RANGE1_DUTY_CYCLE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE1_DUTY_CYCLE_S  1
/* LEDC_CH4_GAMMA_RANGE1_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH4_GAMMA_RANGE1_DUTY_INC    (BIT(0))
#define LEDC_CH4_GAMMA_RANGE1_DUTY_INC_M  (BIT(0))
#define LEDC_CH4_GAMMA_RANGE1_DUTY_INC_V  0x1
#define LEDC_CH4_GAMMA_RANGE1_DUTY_INC_S  0

#define LEDC_CH4_GAMMA_RANGE2_REG  (DR_REG_LEDC_BASE + 0x508)
/* LEDC_CH4_GAMMA_RANGE2_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE2_DUTY_NUM    0x000003FF
#define LEDC_CH4_GAMMA_RANGE2_DUTY_NUM_M  ((LEDC_CH4_GAMMA_RANGE2_DUTY_NUM_V)<<(LEDC_CH4_GAMMA_RANGE2_DUTY_NUM_S))
#define LEDC_CH4_GAMMA_RANGE2_DUTY_NUM_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE2_DUTY_NUM_S  21
/* LEDC_CH4_GAMMA_RANGE2_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE2_SCALE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE2_SCALE_M  ((LEDC_CH4_GAMMA_RANGE2_SCALE_V)<<(LEDC_CH4_GAMMA_RANGE2_SCALE_S))
#define LEDC_CH4_GAMMA_RANGE2_SCALE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE2_SCALE_S  11
/* LEDC_CH4_GAMMA_RANGE2_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE2_DUTY_CYCLE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE2_DUTY_CYCLE_M  ((LEDC_CH4_GAMMA_RANGE2_DUTY_CYCLE_V)<<(LEDC_CH4_GAMMA_RANGE2_DUTY_CYCLE_S))
#define LEDC_CH4_GAMMA_RANGE2_DUTY_CYCLE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE2_DUTY_CYCLE_S  1
/* LEDC_CH4_GAMMA_RANGE2_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH4_GAMMA_RANGE2_DUTY_INC    (BIT(0))
#define LEDC_CH4_GAMMA_RANGE2_DUTY_INC_M  (BIT(0))
#define LEDC_CH4_GAMMA_RANGE2_DUTY_INC_V  0x1
#define LEDC_CH4_GAMMA_RANGE2_DUTY_INC_S  0

#define LEDC_CH4_GAMMA_RANGE3_REG  (DR_REG_LEDC_BASE + 0x50c)
/* LEDC_CH4_GAMMA_RANGE3_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE3_DUTY_NUM    0x000003FF
#define LEDC_CH4_GAMMA_RANGE3_DUTY_NUM_M  ((LEDC_CH4_GAMMA_RANGE3_DUTY_NUM_V)<<(LEDC_CH4_GAMMA_RANGE3_DUTY_NUM_S))
#define LEDC_CH4_GAMMA_RANGE3_DUTY_NUM_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE3_DUTY_NUM_S  21
/* LEDC_CH4_GAMMA_RANGE3_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE3_SCALE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE3_SCALE_M  ((LEDC_CH4_GAMMA_RANGE3_SCALE_V)<<(LEDC_CH4_GAMMA_RANGE3_SCALE_S))
#define LEDC_CH4_GAMMA_RANGE3_SCALE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE3_SCALE_S  11
/* LEDC_CH4_GAMMA_RANGE3_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE3_DUTY_CYCLE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE3_DUTY_CYCLE_M  ((LEDC_CH4_GAMMA_RANGE3_DUTY_CYCLE_V)<<(LEDC_CH4_GAMMA_RANGE3_DUTY_CYCLE_S))
#define LEDC_CH4_GAMMA_RANGE3_DUTY_CYCLE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE3_DUTY_CYCLE_S  1
/* LEDC_CH4_GAMMA_RANGE3_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH4_GAMMA_RANGE3_DUTY_INC    (BIT(0))
#define LEDC_CH4_GAMMA_RANGE3_DUTY_INC_M  (BIT(0))
#define LEDC_CH4_GAMMA_RANGE3_DUTY_INC_V  0x1
#define LEDC_CH4_GAMMA_RANGE3_DUTY_INC_S  0

#define LEDC_CH4_GAMMA_RANGE4_REG  (DR_REG_LEDC_BASE + 0x510)
/* LEDC_CH4_GAMMA_RANGE4_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE4_DUTY_NUM    0x000003FF
#define LEDC_CH4_GAMMA_RANGE4_DUTY_NUM_M  ((LEDC_CH4_GAMMA_RANGE4_DUTY_NUM_V)<<(LEDC_CH4_GAMMA_RANGE4_DUTY_NUM_S))
#define LEDC_CH4_GAMMA_RANGE4_DUTY_NUM_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE4_DUTY_NUM_S  21
/* LEDC_CH4_GAMMA_RANGE4_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE4_SCALE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE4_SCALE_M  ((LEDC_CH4_GAMMA_RANGE4_SCALE_V)<<(LEDC_CH4_GAMMA_RANGE4_SCALE_S))
#define LEDC_CH4_GAMMA_RANGE4_SCALE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE4_SCALE_S  11
/* LEDC_CH4_GAMMA_RANGE4_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE4_DUTY_CYCLE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE4_DUTY_CYCLE_M  ((LEDC_CH4_GAMMA_RANGE4_DUTY_CYCLE_V)<<(LEDC_CH4_GAMMA_RANGE4_DUTY_CYCLE_S))
#define LEDC_CH4_GAMMA_RANGE4_DUTY_CYCLE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE4_DUTY_CYCLE_S  1
/* LEDC_CH4_GAMMA_RANGE4_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH4_GAMMA_RANGE4_DUTY_INC    (BIT(0))
#define LEDC_CH4_GAMMA_RANGE4_DUTY_INC_M  (BIT(0))
#define LEDC_CH4_GAMMA_RANGE4_DUTY_INC_V  0x1
#define LEDC_CH4_GAMMA_RANGE4_DUTY_INC_S  0

#define LEDC_CH4_GAMMA_RANGE5_REG  (DR_REG_LEDC_BASE + 0x514)
/* LEDC_CH4_GAMMA_RANGE5_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE5_DUTY_NUM    0x000003FF
#define LEDC_CH4_GAMMA_RANGE5_DUTY_NUM_M  ((LEDC_CH4_GAMMA_RANGE5_DUTY_NUM_V)<<(LEDC_CH4_GAMMA_RANGE5_DUTY_NUM_S))
#define LEDC_CH4_GAMMA_RANGE5_DUTY_NUM_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE5_DUTY_NUM_S  21
/* LEDC_CH4_GAMMA_RANGE5_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE5_SCALE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE5_SCALE_M  ((LEDC_CH4_GAMMA_RANGE5_SCALE_V)<<(LEDC_CH4_GAMMA_RANGE5_SCALE_S))
#define LEDC_CH4_GAMMA_RANGE5_SCALE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE5_SCALE_S  11
/* LEDC_CH4_GAMMA_RANGE5_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE5_DUTY_CYCLE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE5_DUTY_CYCLE_M  ((LEDC_CH4_GAMMA_RANGE5_DUTY_CYCLE_V)<<(LEDC_CH4_GAMMA_RANGE5_DUTY_CYCLE_S))
#define LEDC_CH4_GAMMA_RANGE5_DUTY_CYCLE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE5_DUTY_CYCLE_S  1
/* LEDC_CH4_GAMMA_RANGE5_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH4_GAMMA_RANGE5_DUTY_INC    (BIT(0))
#define LEDC_CH4_GAMMA_RANGE5_DUTY_INC_M  (BIT(0))
#define LEDC_CH4_GAMMA_RANGE5_DUTY_INC_V  0x1
#define LEDC_CH4_GAMMA_RANGE5_DUTY_INC_S  0

#define LEDC_CH4_GAMMA_RANGE6_REG  (DR_REG_LEDC_BASE + 0x518)
/* LEDC_CH4_GAMMA_RANGE6_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE6_DUTY_NUM    0x000003FF
#define LEDC_CH4_GAMMA_RANGE6_DUTY_NUM_M  ((LEDC_CH4_GAMMA_RANGE6_DUTY_NUM_V)<<(LEDC_CH4_GAMMA_RANGE6_DUTY_NUM_S))
#define LEDC_CH4_GAMMA_RANGE6_DUTY_NUM_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE6_DUTY_NUM_S  21
/* LEDC_CH4_GAMMA_RANGE6_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE6_SCALE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE6_SCALE_M  ((LEDC_CH4_GAMMA_RANGE6_SCALE_V)<<(LEDC_CH4_GAMMA_RANGE6_SCALE_S))
#define LEDC_CH4_GAMMA_RANGE6_SCALE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE6_SCALE_S  11
/* LEDC_CH4_GAMMA_RANGE6_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE6_DUTY_CYCLE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE6_DUTY_CYCLE_M  ((LEDC_CH4_GAMMA_RANGE6_DUTY_CYCLE_V)<<(LEDC_CH4_GAMMA_RANGE6_DUTY_CYCLE_S))
#define LEDC_CH4_GAMMA_RANGE6_DUTY_CYCLE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE6_DUTY_CYCLE_S  1
/* LEDC_CH4_GAMMA_RANGE6_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH4_GAMMA_RANGE6_DUTY_INC    (BIT(0))
#define LEDC_CH4_GAMMA_RANGE6_DUTY_INC_M  (BIT(0))
#define LEDC_CH4_GAMMA_RANGE6_DUTY_INC_V  0x1
#define LEDC_CH4_GAMMA_RANGE6_DUTY_INC_S  0

#define LEDC_CH4_GAMMA_RANGE7_REG  (DR_REG_LEDC_BASE + 0x51c)
/* LEDC_CH4_GAMMA_RANGE7_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE7_DUTY_NUM    0x000003FF
#define LEDC_CH4_GAMMA_RANGE7_DUTY_NUM_M  ((LEDC_CH4_GAMMA_RANGE7_DUTY_NUM_V)<<(LEDC_CH4_GAMMA_RANGE7_DUTY_NUM_S))
#define LEDC_CH4_GAMMA_RANGE7_DUTY_NUM_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE7_DUTY_NUM_S  21
/* LEDC_CH4_GAMMA_RANGE7_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE7_SCALE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE7_SCALE_M  ((LEDC_CH4_GAMMA_RANGE7_SCALE_V)<<(LEDC_CH4_GAMMA_RANGE7_SCALE_S))
#define LEDC_CH4_GAMMA_RANGE7_SCALE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE7_SCALE_S  11
/* LEDC_CH4_GAMMA_RANGE7_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE7_DUTY_CYCLE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE7_DUTY_CYCLE_M  ((LEDC_CH4_GAMMA_RANGE7_DUTY_CYCLE_V)<<(LEDC_CH4_GAMMA_RANGE7_DUTY_CYCLE_S))
#define LEDC_CH4_GAMMA_RANGE7_DUTY_CYCLE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE7_DUTY_CYCLE_S  1
/* LEDC_CH4_GAMMA_RANGE7_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH4_GAMMA_RANGE7_DUTY_INC    (BIT(0))
#define LEDC_CH4_GAMMA_RANGE7_DUTY_INC_M  (BIT(0))
#define LEDC_CH4_GAMMA_RANGE7_DUTY_INC_V  0x1
#define LEDC_CH4_GAMMA_RANGE7_DUTY_INC_S  0

#define LEDC_CH4_GAMMA_RANGE8_REG  (DR_REG_LEDC_BASE + 0x520)
/* LEDC_CH4_GAMMA_RANGE8_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE8_DUTY_NUM    0x000003FF
#define LEDC_CH4_GAMMA_RANGE8_DUTY_NUM_M  ((LEDC_CH4_GAMMA_RANGE8_DUTY_NUM_V)<<(LEDC_CH4_GAMMA_RANGE8_DUTY_NUM_S))
#define LEDC_CH4_GAMMA_RANGE8_DUTY_NUM_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE8_DUTY_NUM_S  21
/* LEDC_CH4_GAMMA_RANGE8_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE8_SCALE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE8_SCALE_M  ((LEDC_CH4_GAMMA_RANGE8_SCALE_V)<<(LEDC_CH4_GAMMA_RANGE8_SCALE_S))
#define LEDC_CH4_GAMMA_RANGE8_SCALE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE8_SCALE_S  11
/* LEDC_CH4_GAMMA_RANGE8_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE8_DUTY_CYCLE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE8_DUTY_CYCLE_M  ((LEDC_CH4_GAMMA_RANGE8_DUTY_CYCLE_V)<<(LEDC_CH4_GAMMA_RANGE8_DUTY_CYCLE_S))
#define LEDC_CH4_GAMMA_RANGE8_DUTY_CYCLE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE8_DUTY_CYCLE_S  1
/* LEDC_CH4_GAMMA_RANGE8_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH4_GAMMA_RANGE8_DUTY_INC    (BIT(0))
#define LEDC_CH4_GAMMA_RANGE8_DUTY_INC_M  (BIT(0))
#define LEDC_CH4_GAMMA_RANGE8_DUTY_INC_V  0x1
#define LEDC_CH4_GAMMA_RANGE8_DUTY_INC_S  0

#define LEDC_CH4_GAMMA_RANGE9_REG  (DR_REG_LEDC_BASE + 0x524)
/* LEDC_CH4_GAMMA_RANGE9_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE9_DUTY_NUM    0x000003FF
#define LEDC_CH4_GAMMA_RANGE9_DUTY_NUM_M  ((LEDC_CH4_GAMMA_RANGE9_DUTY_NUM_V)<<(LEDC_CH4_GAMMA_RANGE9_DUTY_NUM_S))
#define LEDC_CH4_GAMMA_RANGE9_DUTY_NUM_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE9_DUTY_NUM_S  21
/* LEDC_CH4_GAMMA_RANGE9_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE9_SCALE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE9_SCALE_M  ((LEDC_CH4_GAMMA_RANGE9_SCALE_V)<<(LEDC_CH4_GAMMA_RANGE9_SCALE_S))
#define LEDC_CH4_GAMMA_RANGE9_SCALE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE9_SCALE_S  11
/* LEDC_CH4_GAMMA_RANGE9_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE9_DUTY_CYCLE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE9_DUTY_CYCLE_M  ((LEDC_CH4_GAMMA_RANGE9_DUTY_CYCLE_V)<<(LEDC_CH4_GAMMA_RANGE9_DUTY_CYCLE_S))
#define LEDC_CH4_GAMMA_RANGE9_DUTY_CYCLE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE9_DUTY_CYCLE_S  1
/* LEDC_CH4_GAMMA_RANGE9_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH4_GAMMA_RANGE9_DUTY_INC    (BIT(0))
#define LEDC_CH4_GAMMA_RANGE9_DUTY_INC_M  (BIT(0))
#define LEDC_CH4_GAMMA_RANGE9_DUTY_INC_V  0x1
#define LEDC_CH4_GAMMA_RANGE9_DUTY_INC_S  0

#define LEDC_CH4_GAMMA_RANGE10_REG  (DR_REG_LEDC_BASE + 0x528)
/* LEDC_CH4_GAMMA_RANGE10_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE10_DUTY_NUM    0x000003FF
#define LEDC_CH4_GAMMA_RANGE10_DUTY_NUM_M  ((LEDC_CH4_GAMMA_RANGE10_DUTY_NUM_V)<<(LEDC_CH4_GAMMA_RANGE10_DUTY_NUM_S))
#define LEDC_CH4_GAMMA_RANGE10_DUTY_NUM_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE10_DUTY_NUM_S  21
/* LEDC_CH4_GAMMA_RANGE10_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE10_SCALE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE10_SCALE_M  ((LEDC_CH4_GAMMA_RANGE10_SCALE_V)<<(LEDC_CH4_GAMMA_RANGE10_SCALE_S))
#define LEDC_CH4_GAMMA_RANGE10_SCALE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE10_SCALE_S  11
/* LEDC_CH4_GAMMA_RANGE10_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE10_DUTY_CYCLE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE10_DUTY_CYCLE_M  ((LEDC_CH4_GAMMA_RANGE10_DUTY_CYCLE_V)<<(LEDC_CH4_GAMMA_RANGE10_DUTY_CYCLE_S))
#define LEDC_CH4_GAMMA_RANGE10_DUTY_CYCLE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE10_DUTY_CYCLE_S  1
/* LEDC_CH4_GAMMA_RANGE10_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH4_GAMMA_RANGE10_DUTY_INC    (BIT(0))
#define LEDC_CH4_GAMMA_RANGE10_DUTY_INC_M  (BIT(0))
#define LEDC_CH4_GAMMA_RANGE10_DUTY_INC_V  0x1
#define LEDC_CH4_GAMMA_RANGE10_DUTY_INC_S  0

#define LEDC_CH4_GAMMA_RANGE11_REG  (DR_REG_LEDC_BASE + 0x52c)
/* LEDC_CH4_GAMMA_RANGE11_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE11_DUTY_NUM    0x000003FF
#define LEDC_CH4_GAMMA_RANGE11_DUTY_NUM_M  ((LEDC_CH4_GAMMA_RANGE11_DUTY_NUM_V)<<(LEDC_CH4_GAMMA_RANGE11_DUTY_NUM_S))
#define LEDC_CH4_GAMMA_RANGE11_DUTY_NUM_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE11_DUTY_NUM_S  21
/* LEDC_CH4_GAMMA_RANGE11_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE11_SCALE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE11_SCALE_M  ((LEDC_CH4_GAMMA_RANGE11_SCALE_V)<<(LEDC_CH4_GAMMA_RANGE11_SCALE_S))
#define LEDC_CH4_GAMMA_RANGE11_SCALE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE11_SCALE_S  11
/* LEDC_CH4_GAMMA_RANGE11_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE11_DUTY_CYCLE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE11_DUTY_CYCLE_M  ((LEDC_CH4_GAMMA_RANGE11_DUTY_CYCLE_V)<<(LEDC_CH4_GAMMA_RANGE11_DUTY_CYCLE_S))
#define LEDC_CH4_GAMMA_RANGE11_DUTY_CYCLE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE11_DUTY_CYCLE_S  1
/* LEDC_CH4_GAMMA_RANGE11_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH4_GAMMA_RANGE11_DUTY_INC    (BIT(0))
#define LEDC_CH4_GAMMA_RANGE11_DUTY_INC_M  (BIT(0))
#define LEDC_CH4_GAMMA_RANGE11_DUTY_INC_V  0x1
#define LEDC_CH4_GAMMA_RANGE11_DUTY_INC_S  0

#define LEDC_CH4_GAMMA_RANGE12_REG  (DR_REG_LEDC_BASE + 0x530)
/* LEDC_CH4_GAMMA_RANGE12_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE12_DUTY_NUM    0x000003FF
#define LEDC_CH4_GAMMA_RANGE12_DUTY_NUM_M  ((LEDC_CH4_GAMMA_RANGE12_DUTY_NUM_V)<<(LEDC_CH4_GAMMA_RANGE12_DUTY_NUM_S))
#define LEDC_CH4_GAMMA_RANGE12_DUTY_NUM_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE12_DUTY_NUM_S  21
/* LEDC_CH4_GAMMA_RANGE12_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE12_SCALE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE12_SCALE_M  ((LEDC_CH4_GAMMA_RANGE12_SCALE_V)<<(LEDC_CH4_GAMMA_RANGE12_SCALE_S))
#define LEDC_CH4_GAMMA_RANGE12_SCALE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE12_SCALE_S  11
/* LEDC_CH4_GAMMA_RANGE12_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE12_DUTY_CYCLE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE12_DUTY_CYCLE_M  ((LEDC_CH4_GAMMA_RANGE12_DUTY_CYCLE_V)<<(LEDC_CH4_GAMMA_RANGE12_DUTY_CYCLE_S))
#define LEDC_CH4_GAMMA_RANGE12_DUTY_CYCLE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE12_DUTY_CYCLE_S  1
/* LEDC_CH4_GAMMA_RANGE12_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH4_GAMMA_RANGE12_DUTY_INC    (BIT(0))
#define LEDC_CH4_GAMMA_RANGE12_DUTY_INC_M  (BIT(0))
#define LEDC_CH4_GAMMA_RANGE12_DUTY_INC_V  0x1
#define LEDC_CH4_GAMMA_RANGE12_DUTY_INC_S  0

#define LEDC_CH4_GAMMA_RANGE13_REG  (DR_REG_LEDC_BASE + 0x534)
/* LEDC_CH4_GAMMA_RANGE13_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE13_DUTY_NUM    0x000003FF
#define LEDC_CH4_GAMMA_RANGE13_DUTY_NUM_M  ((LEDC_CH4_GAMMA_RANGE13_DUTY_NUM_V)<<(LEDC_CH4_GAMMA_RANGE13_DUTY_NUM_S))
#define LEDC_CH4_GAMMA_RANGE13_DUTY_NUM_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE13_DUTY_NUM_S  21
/* LEDC_CH4_GAMMA_RANGE13_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE13_SCALE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE13_SCALE_M  ((LEDC_CH4_GAMMA_RANGE13_SCALE_V)<<(LEDC_CH4_GAMMA_RANGE13_SCALE_S))
#define LEDC_CH4_GAMMA_RANGE13_SCALE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE13_SCALE_S  11
/* LEDC_CH4_GAMMA_RANGE13_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE13_DUTY_CYCLE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE13_DUTY_CYCLE_M  ((LEDC_CH4_GAMMA_RANGE13_DUTY_CYCLE_V)<<(LEDC_CH4_GAMMA_RANGE13_DUTY_CYCLE_S))
#define LEDC_CH4_GAMMA_RANGE13_DUTY_CYCLE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE13_DUTY_CYCLE_S  1
/* LEDC_CH4_GAMMA_RANGE13_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH4_GAMMA_RANGE13_DUTY_INC    (BIT(0))
#define LEDC_CH4_GAMMA_RANGE13_DUTY_INC_M  (BIT(0))
#define LEDC_CH4_GAMMA_RANGE13_DUTY_INC_V  0x1
#define LEDC_CH4_GAMMA_RANGE13_DUTY_INC_S  0

#define LEDC_CH4_GAMMA_RANGE14_REG  (DR_REG_LEDC_BASE + 0x538)
/* LEDC_CH4_GAMMA_RANGE14_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE14_DUTY_NUM    0x000003FF
#define LEDC_CH4_GAMMA_RANGE14_DUTY_NUM_M  ((LEDC_CH4_GAMMA_RANGE14_DUTY_NUM_V)<<(LEDC_CH4_GAMMA_RANGE14_DUTY_NUM_S))
#define LEDC_CH4_GAMMA_RANGE14_DUTY_NUM_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE14_DUTY_NUM_S  21
/* LEDC_CH4_GAMMA_RANGE14_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE14_SCALE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE14_SCALE_M  ((LEDC_CH4_GAMMA_RANGE14_SCALE_V)<<(LEDC_CH4_GAMMA_RANGE14_SCALE_S))
#define LEDC_CH4_GAMMA_RANGE14_SCALE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE14_SCALE_S  11
/* LEDC_CH4_GAMMA_RANGE14_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE14_DUTY_CYCLE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE14_DUTY_CYCLE_M  ((LEDC_CH4_GAMMA_RANGE14_DUTY_CYCLE_V)<<(LEDC_CH4_GAMMA_RANGE14_DUTY_CYCLE_S))
#define LEDC_CH4_GAMMA_RANGE14_DUTY_CYCLE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE14_DUTY_CYCLE_S  1
/* LEDC_CH4_GAMMA_RANGE14_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH4_GAMMA_RANGE14_DUTY_INC    (BIT(0))
#define LEDC_CH4_GAMMA_RANGE14_DUTY_INC_M  (BIT(0))
#define LEDC_CH4_GAMMA_RANGE14_DUTY_INC_V  0x1
#define LEDC_CH4_GAMMA_RANGE14_DUTY_INC_S  0

#define LEDC_CH4_GAMMA_RANGE15_REG  (DR_REG_LEDC_BASE + 0x53c)
/* LEDC_CH4_GAMMA_RANGE15_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE15_DUTY_NUM    0x000003FF
#define LEDC_CH4_GAMMA_RANGE15_DUTY_NUM_M  ((LEDC_CH4_GAMMA_RANGE15_DUTY_NUM_V)<<(LEDC_CH4_GAMMA_RANGE15_DUTY_NUM_S))
#define LEDC_CH4_GAMMA_RANGE15_DUTY_NUM_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE15_DUTY_NUM_S  21
/* LEDC_CH4_GAMMA_RANGE15_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE15_SCALE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE15_SCALE_M  ((LEDC_CH4_GAMMA_RANGE15_SCALE_V)<<(LEDC_CH4_GAMMA_RANGE15_SCALE_S))
#define LEDC_CH4_GAMMA_RANGE15_SCALE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE15_SCALE_S  11
/* LEDC_CH4_GAMMA_RANGE15_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH4_GAMMA_RANGE15_DUTY_CYCLE    0x000003FF
#define LEDC_CH4_GAMMA_RANGE15_DUTY_CYCLE_M  ((LEDC_CH4_GAMMA_RANGE15_DUTY_CYCLE_V)<<(LEDC_CH4_GAMMA_RANGE15_DUTY_CYCLE_S))
#define LEDC_CH4_GAMMA_RANGE15_DUTY_CYCLE_V  0x3FF
#define LEDC_CH4_GAMMA_RANGE15_DUTY_CYCLE_S  1
/* LEDC_CH4_GAMMA_RANGE15_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH4_GAMMA_RANGE15_DUTY_INC    (BIT(0))
#define LEDC_CH4_GAMMA_RANGE15_DUTY_INC_M  (BIT(0))
#define LEDC_CH4_GAMMA_RANGE15_DUTY_INC_V  0x1
#define LEDC_CH4_GAMMA_RANGE15_DUTY_INC_S  0


#define LEDC_CH5_GAMMA_RANGE0_REG  (DR_REG_LEDC_BASE + 0x540)
/* LEDC_CH5_GAMMA_RANGE0_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE0_DUTY_NUM    0x000003FF
#define LEDC_CH5_GAMMA_RANGE0_DUTY_NUM_M  ((LEDC_CH5_GAMMA_RANGE0_DUTY_NUM_V)<<(LEDC_CH5_GAMMA_RANGE0_DUTY_NUM_S))
#define LEDC_CH5_GAMMA_RANGE0_DUTY_NUM_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE0_DUTY_NUM_S  21
/* LEDC_CH5_GAMMA_RANGE0_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE0_SCALE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE0_SCALE_M  ((LEDC_CH5_GAMMA_RANGE0_SCALE_V)<<(LEDC_CH5_GAMMA_RANGE0_SCALE_S))
#define LEDC_CH5_GAMMA_RANGE0_SCALE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE0_SCALE_S  11
/* LEDC_CH5_GAMMA_RANGE0_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE0_DUTY_CYCLE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE0_DUTY_CYCLE_M  ((LEDC_CH5_GAMMA_RANGE0_DUTY_CYCLE_V)<<(LEDC_CH5_GAMMA_RANGE0_DUTY_CYCLE_S))
#define LEDC_CH5_GAMMA_RANGE0_DUTY_CYCLE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE0_DUTY_CYCLE_S  1
/* LEDC_CH5_GAMMA_RANGE0_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH5_GAMMA_RANGE0_DUTY_INC    (BIT(0))
#define LEDC_CH5_GAMMA_RANGE0_DUTY_INC_M  (BIT(0))
#define LEDC_CH5_GAMMA_RANGE0_DUTY_INC_V  0x1
#define LEDC_CH5_GAMMA_RANGE0_DUTY_INC_S  0

#define LEDC_CH5_GAMMA_RANGE1_REG  (DR_REG_LEDC_BASE + 0x544)
/* LEDC_CH5_GAMMA_RANGE1_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE1_DUTY_NUM    0x000003FF
#define LEDC_CH5_GAMMA_RANGE1_DUTY_NUM_M  ((LEDC_CH5_GAMMA_RANGE1_DUTY_NUM_V)<<(LEDC_CH5_GAMMA_RANGE1_DUTY_NUM_S))
#define LEDC_CH5_GAMMA_RANGE1_DUTY_NUM_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE1_DUTY_NUM_S  21
/* LEDC_CH5_GAMMA_RANGE1_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE1_SCALE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE1_SCALE_M  ((LEDC_CH5_GAMMA_RANGE1_SCALE_V)<<(LEDC_CH5_GAMMA_RANGE1_SCALE_S))
#define LEDC_CH5_GAMMA_RANGE1_SCALE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE1_SCALE_S  11
/* LEDC_CH5_GAMMA_RANGE1_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE1_DUTY_CYCLE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE1_DUTY_CYCLE_M  ((LEDC_CH5_GAMMA_RANGE1_DUTY_CYCLE_V)<<(LEDC_CH5_GAMMA_RANGE1_DUTY_CYCLE_S))
#define LEDC_CH5_GAMMA_RANGE1_DUTY_CYCLE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE1_DUTY_CYCLE_S  1
/* LEDC_CH5_GAMMA_RANGE1_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH5_GAMMA_RANGE1_DUTY_INC    (BIT(0))
#define LEDC_CH5_GAMMA_RANGE1_DUTY_INC_M  (BIT(0))
#define LEDC_CH5_GAMMA_RANGE1_DUTY_INC_V  0x1
#define LEDC_CH5_GAMMA_RANGE1_DUTY_INC_S  0

#define LEDC_CH5_GAMMA_RANGE2_REG  (DR_REG_LEDC_BASE + 0x548)
/* LEDC_CH5_GAMMA_RANGE2_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE2_DUTY_NUM    0x000003FF
#define LEDC_CH5_GAMMA_RANGE2_DUTY_NUM_M  ((LEDC_CH5_GAMMA_RANGE2_DUTY_NUM_V)<<(LEDC_CH5_GAMMA_RANGE2_DUTY_NUM_S))
#define LEDC_CH5_GAMMA_RANGE2_DUTY_NUM_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE2_DUTY_NUM_S  21
/* LEDC_CH5_GAMMA_RANGE2_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE2_SCALE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE2_SCALE_M  ((LEDC_CH5_GAMMA_RANGE2_SCALE_V)<<(LEDC_CH5_GAMMA_RANGE2_SCALE_S))
#define LEDC_CH5_GAMMA_RANGE2_SCALE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE2_SCALE_S  11
/* LEDC_CH5_GAMMA_RANGE2_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE2_DUTY_CYCLE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE2_DUTY_CYCLE_M  ((LEDC_CH5_GAMMA_RANGE2_DUTY_CYCLE_V)<<(LEDC_CH5_GAMMA_RANGE2_DUTY_CYCLE_S))
#define LEDC_CH5_GAMMA_RANGE2_DUTY_CYCLE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE2_DUTY_CYCLE_S  1
/* LEDC_CH5_GAMMA_RANGE2_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH5_GAMMA_RANGE2_DUTY_INC    (BIT(0))
#define LEDC_CH5_GAMMA_RANGE2_DUTY_INC_M  (BIT(0))
#define LEDC_CH5_GAMMA_RANGE2_DUTY_INC_V  0x1
#define LEDC_CH5_GAMMA_RANGE2_DUTY_INC_S  0

#define LEDC_CH5_GAMMA_RANGE3_REG  (DR_REG_LEDC_BASE + 0x54c)
/* LEDC_CH5_GAMMA_RANGE3_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE3_DUTY_NUM    0x000003FF
#define LEDC_CH5_GAMMA_RANGE3_DUTY_NUM_M  ((LEDC_CH5_GAMMA_RANGE3_DUTY_NUM_V)<<(LEDC_CH5_GAMMA_RANGE3_DUTY_NUM_S))
#define LEDC_CH5_GAMMA_RANGE3_DUTY_NUM_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE3_DUTY_NUM_S  21
/* LEDC_CH5_GAMMA_RANGE3_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE3_SCALE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE3_SCALE_M  ((LEDC_CH5_GAMMA_RANGE3_SCALE_V)<<(LEDC_CH5_GAMMA_RANGE3_SCALE_S))
#define LEDC_CH5_GAMMA_RANGE3_SCALE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE3_SCALE_S  11
/* LEDC_CH5_GAMMA_RANGE3_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE3_DUTY_CYCLE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE3_DUTY_CYCLE_M  ((LEDC_CH5_GAMMA_RANGE3_DUTY_CYCLE_V)<<(LEDC_CH5_GAMMA_RANGE3_DUTY_CYCLE_S))
#define LEDC_CH5_GAMMA_RANGE3_DUTY_CYCLE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE3_DUTY_CYCLE_S  1
/* LEDC_CH5_GAMMA_RANGE3_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH5_GAMMA_RANGE3_DUTY_INC    (BIT(0))
#define LEDC_CH5_GAMMA_RANGE3_DUTY_INC_M  (BIT(0))
#define LEDC_CH5_GAMMA_RANGE3_DUTY_INC_V  0x1
#define LEDC_CH5_GAMMA_RANGE3_DUTY_INC_S  0

#define LEDC_CH5_GAMMA_RANGE4_REG  (DR_REG_LEDC_BASE + 0x550)
/* LEDC_CH5_GAMMA_RANGE4_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE4_DUTY_NUM    0x000003FF
#define LEDC_CH5_GAMMA_RANGE4_DUTY_NUM_M  ((LEDC_CH5_GAMMA_RANGE4_DUTY_NUM_V)<<(LEDC_CH5_GAMMA_RANGE4_DUTY_NUM_S))
#define LEDC_CH5_GAMMA_RANGE4_DUTY_NUM_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE4_DUTY_NUM_S  21
/* LEDC_CH5_GAMMA_RANGE4_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE4_SCALE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE4_SCALE_M  ((LEDC_CH5_GAMMA_RANGE4_SCALE_V)<<(LEDC_CH5_GAMMA_RANGE4_SCALE_S))
#define LEDC_CH5_GAMMA_RANGE4_SCALE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE4_SCALE_S  11
/* LEDC_CH5_GAMMA_RANGE4_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE4_DUTY_CYCLE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE4_DUTY_CYCLE_M  ((LEDC_CH5_GAMMA_RANGE4_DUTY_CYCLE_V)<<(LEDC_CH5_GAMMA_RANGE4_DUTY_CYCLE_S))
#define LEDC_CH5_GAMMA_RANGE4_DUTY_CYCLE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE4_DUTY_CYCLE_S  1
/* LEDC_CH5_GAMMA_RANGE4_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH5_GAMMA_RANGE4_DUTY_INC    (BIT(0))
#define LEDC_CH5_GAMMA_RANGE4_DUTY_INC_M  (BIT(0))
#define LEDC_CH5_GAMMA_RANGE4_DUTY_INC_V  0x1
#define LEDC_CH5_GAMMA_RANGE4_DUTY_INC_S  0

#define LEDC_CH5_GAMMA_RANGE5_REG  (DR_REG_LEDC_BASE + 0x554)
/* LEDC_CH5_GAMMA_RANGE5_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE5_DUTY_NUM    0x000003FF
#define LEDC_CH5_GAMMA_RANGE5_DUTY_NUM_M  ((LEDC_CH5_GAMMA_RANGE5_DUTY_NUM_V)<<(LEDC_CH5_GAMMA_RANGE5_DUTY_NUM_S))
#define LEDC_CH5_GAMMA_RANGE5_DUTY_NUM_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE5_DUTY_NUM_S  21
/* LEDC_CH5_GAMMA_RANGE5_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE5_SCALE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE5_SCALE_M  ((LEDC_CH5_GAMMA_RANGE5_SCALE_V)<<(LEDC_CH5_GAMMA_RANGE5_SCALE_S))
#define LEDC_CH5_GAMMA_RANGE5_SCALE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE5_SCALE_S  11
/* LEDC_CH5_GAMMA_RANGE5_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE5_DUTY_CYCLE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE5_DUTY_CYCLE_M  ((LEDC_CH5_GAMMA_RANGE5_DUTY_CYCLE_V)<<(LEDC_CH5_GAMMA_RANGE5_DUTY_CYCLE_S))
#define LEDC_CH5_GAMMA_RANGE5_DUTY_CYCLE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE5_DUTY_CYCLE_S  1
/* LEDC_CH5_GAMMA_RANGE5_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH5_GAMMA_RANGE5_DUTY_INC    (BIT(0))
#define LEDC_CH5_GAMMA_RANGE5_DUTY_INC_M  (BIT(0))
#define LEDC_CH5_GAMMA_RANGE5_DUTY_INC_V  0x1
#define LEDC_CH5_GAMMA_RANGE5_DUTY_INC_S  0

#define LEDC_CH5_GAMMA_RANGE6_REG  (DR_REG_LEDC_BASE + 0x558)
/* LEDC_CH5_GAMMA_RANGE6_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE6_DUTY_NUM    0x000003FF
#define LEDC_CH5_GAMMA_RANGE6_DUTY_NUM_M  ((LEDC_CH5_GAMMA_RANGE6_DUTY_NUM_V)<<(LEDC_CH5_GAMMA_RANGE6_DUTY_NUM_S))
#define LEDC_CH5_GAMMA_RANGE6_DUTY_NUM_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE6_DUTY_NUM_S  21
/* LEDC_CH5_GAMMA_RANGE6_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE6_SCALE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE6_SCALE_M  ((LEDC_CH5_GAMMA_RANGE6_SCALE_V)<<(LEDC_CH5_GAMMA_RANGE6_SCALE_S))
#define LEDC_CH5_GAMMA_RANGE6_SCALE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE6_SCALE_S  11
/* LEDC_CH5_GAMMA_RANGE6_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE6_DUTY_CYCLE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE6_DUTY_CYCLE_M  ((LEDC_CH5_GAMMA_RANGE6_DUTY_CYCLE_V)<<(LEDC_CH5_GAMMA_RANGE6_DUTY_CYCLE_S))
#define LEDC_CH5_GAMMA_RANGE6_DUTY_CYCLE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE6_DUTY_CYCLE_S  1
/* LEDC_CH5_GAMMA_RANGE6_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH5_GAMMA_RANGE6_DUTY_INC    (BIT(0))
#define LEDC_CH5_GAMMA_RANGE6_DUTY_INC_M  (BIT(0))
#define LEDC_CH5_GAMMA_RANGE6_DUTY_INC_V  0x1
#define LEDC_CH5_GAMMA_RANGE6_DUTY_INC_S  0

#define LEDC_CH5_GAMMA_RANGE7_REG  (DR_REG_LEDC_BASE + 0x55c)
/* LEDC_CH5_GAMMA_RANGE7_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE7_DUTY_NUM    0x000003FF
#define LEDC_CH5_GAMMA_RANGE7_DUTY_NUM_M  ((LEDC_CH5_GAMMA_RANGE7_DUTY_NUM_V)<<(LEDC_CH5_GAMMA_RANGE7_DUTY_NUM_S))
#define LEDC_CH5_GAMMA_RANGE7_DUTY_NUM_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE7_DUTY_NUM_S  21
/* LEDC_CH5_GAMMA_RANGE7_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE7_SCALE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE7_SCALE_M  ((LEDC_CH5_GAMMA_RANGE7_SCALE_V)<<(LEDC_CH5_GAMMA_RANGE7_SCALE_S))
#define LEDC_CH5_GAMMA_RANGE7_SCALE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE7_SCALE_S  11
/* LEDC_CH5_GAMMA_RANGE7_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE7_DUTY_CYCLE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE7_DUTY_CYCLE_M  ((LEDC_CH5_GAMMA_RANGE7_DUTY_CYCLE_V)<<(LEDC_CH5_GAMMA_RANGE7_DUTY_CYCLE_S))
#define LEDC_CH5_GAMMA_RANGE7_DUTY_CYCLE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE7_DUTY_CYCLE_S  1
/* LEDC_CH5_GAMMA_RANGE7_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH5_GAMMA_RANGE7_DUTY_INC    (BIT(0))
#define LEDC_CH5_GAMMA_RANGE7_DUTY_INC_M  (BIT(0))
#define LEDC_CH5_GAMMA_RANGE7_DUTY_INC_V  0x1
#define LEDC_CH5_GAMMA_RANGE7_DUTY_INC_S  0

#define LEDC_CH5_GAMMA_RANGE8_REG  (DR_REG_LEDC_BASE + 0x560)
/* LEDC_CH5_GAMMA_RANGE8_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE8_DUTY_NUM    0x000003FF
#define LEDC_CH5_GAMMA_RANGE8_DUTY_NUM_M  ((LEDC_CH5_GAMMA_RANGE8_DUTY_NUM_V)<<(LEDC_CH5_GAMMA_RANGE8_DUTY_NUM_S))
#define LEDC_CH5_GAMMA_RANGE8_DUTY_NUM_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE8_DUTY_NUM_S  21
/* LEDC_CH5_GAMMA_RANGE8_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE8_SCALE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE8_SCALE_M  ((LEDC_CH5_GAMMA_RANGE8_SCALE_V)<<(LEDC_CH5_GAMMA_RANGE8_SCALE_S))
#define LEDC_CH5_GAMMA_RANGE8_SCALE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE8_SCALE_S  11
/* LEDC_CH5_GAMMA_RANGE8_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE8_DUTY_CYCLE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE8_DUTY_CYCLE_M  ((LEDC_CH5_GAMMA_RANGE8_DUTY_CYCLE_V)<<(LEDC_CH5_GAMMA_RANGE8_DUTY_CYCLE_S))
#define LEDC_CH5_GAMMA_RANGE8_DUTY_CYCLE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE8_DUTY_CYCLE_S  1
/* LEDC_CH5_GAMMA_RANGE8_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH5_GAMMA_RANGE8_DUTY_INC    (BIT(0))
#define LEDC_CH5_GAMMA_RANGE8_DUTY_INC_M  (BIT(0))
#define LEDC_CH5_GAMMA_RANGE8_DUTY_INC_V  0x1
#define LEDC_CH5_GAMMA_RANGE8_DUTY_INC_S  0

#define LEDC_CH5_GAMMA_RANGE9_REG  (DR_REG_LEDC_BASE + 0x564)
/* LEDC_CH5_GAMMA_RANGE9_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE9_DUTY_NUM    0x000003FF
#define LEDC_CH5_GAMMA_RANGE9_DUTY_NUM_M  ((LEDC_CH5_GAMMA_RANGE9_DUTY_NUM_V)<<(LEDC_CH5_GAMMA_RANGE9_DUTY_NUM_S))
#define LEDC_CH5_GAMMA_RANGE9_DUTY_NUM_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE9_DUTY_NUM_S  21
/* LEDC_CH5_GAMMA_RANGE9_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE9_SCALE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE9_SCALE_M  ((LEDC_CH5_GAMMA_RANGE9_SCALE_V)<<(LEDC_CH5_GAMMA_RANGE9_SCALE_S))
#define LEDC_CH5_GAMMA_RANGE9_SCALE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE9_SCALE_S  11
/* LEDC_CH5_GAMMA_RANGE9_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE9_DUTY_CYCLE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE9_DUTY_CYCLE_M  ((LEDC_CH5_GAMMA_RANGE9_DUTY_CYCLE_V)<<(LEDC_CH5_GAMMA_RANGE9_DUTY_CYCLE_S))
#define LEDC_CH5_GAMMA_RANGE9_DUTY_CYCLE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE9_DUTY_CYCLE_S  1
/* LEDC_CH5_GAMMA_RANGE9_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH5_GAMMA_RANGE9_DUTY_INC    (BIT(0))
#define LEDC_CH5_GAMMA_RANGE9_DUTY_INC_M  (BIT(0))
#define LEDC_CH5_GAMMA_RANGE9_DUTY_INC_V  0x1
#define LEDC_CH5_GAMMA_RANGE9_DUTY_INC_S  0

#define LEDC_CH5_GAMMA_RANGE10_REG  (DR_REG_LEDC_BASE + 0x568)
/* LEDC_CH5_GAMMA_RANGE10_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE10_DUTY_NUM    0x000003FF
#define LEDC_CH5_GAMMA_RANGE10_DUTY_NUM_M  ((LEDC_CH5_GAMMA_RANGE10_DUTY_NUM_V)<<(LEDC_CH5_GAMMA_RANGE10_DUTY_NUM_S))
#define LEDC_CH5_GAMMA_RANGE10_DUTY_NUM_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE10_DUTY_NUM_S  21
/* LEDC_CH5_GAMMA_RANGE10_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE10_SCALE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE10_SCALE_M  ((LEDC_CH5_GAMMA_RANGE10_SCALE_V)<<(LEDC_CH5_GAMMA_RANGE10_SCALE_S))
#define LEDC_CH5_GAMMA_RANGE10_SCALE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE10_SCALE_S  11
/* LEDC_CH5_GAMMA_RANGE10_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE10_DUTY_CYCLE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE10_DUTY_CYCLE_M  ((LEDC_CH5_GAMMA_RANGE10_DUTY_CYCLE_V)<<(LEDC_CH5_GAMMA_RANGE10_DUTY_CYCLE_S))
#define LEDC_CH5_GAMMA_RANGE10_DUTY_CYCLE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE10_DUTY_CYCLE_S  1
/* LEDC_CH5_GAMMA_RANGE10_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH5_GAMMA_RANGE10_DUTY_INC    (BIT(0))
#define LEDC_CH5_GAMMA_RANGE10_DUTY_INC_M  (BIT(0))
#define LEDC_CH5_GAMMA_RANGE10_DUTY_INC_V  0x1
#define LEDC_CH5_GAMMA_RANGE10_DUTY_INC_S  0

#define LEDC_CH5_GAMMA_RANGE11_REG  (DR_REG_LEDC_BASE + 0x56c)
/* LEDC_CH5_GAMMA_RANGE11_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE11_DUTY_NUM    0x000003FF
#define LEDC_CH5_GAMMA_RANGE11_DUTY_NUM_M  ((LEDC_CH5_GAMMA_RANGE11_DUTY_NUM_V)<<(LEDC_CH5_GAMMA_RANGE11_DUTY_NUM_S))
#define LEDC_CH5_GAMMA_RANGE11_DUTY_NUM_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE11_DUTY_NUM_S  21
/* LEDC_CH5_GAMMA_RANGE11_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE11_SCALE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE11_SCALE_M  ((LEDC_CH5_GAMMA_RANGE11_SCALE_V)<<(LEDC_CH5_GAMMA_RANGE11_SCALE_S))
#define LEDC_CH5_GAMMA_RANGE11_SCALE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE11_SCALE_S  11
/* LEDC_CH5_GAMMA_RANGE11_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE11_DUTY_CYCLE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE11_DUTY_CYCLE_M  ((LEDC_CH5_GAMMA_RANGE11_DUTY_CYCLE_V)<<(LEDC_CH5_GAMMA_RANGE11_DUTY_CYCLE_S))
#define LEDC_CH5_GAMMA_RANGE11_DUTY_CYCLE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE11_DUTY_CYCLE_S  1
/* LEDC_CH5_GAMMA_RANGE11_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH5_GAMMA_RANGE11_DUTY_INC    (BIT(0))
#define LEDC_CH5_GAMMA_RANGE11_DUTY_INC_M  (BIT(0))
#define LEDC_CH5_GAMMA_RANGE11_DUTY_INC_V  0x1
#define LEDC_CH5_GAMMA_RANGE11_DUTY_INC_S  0

#define LEDC_CH5_GAMMA_RANGE12_REG  (DR_REG_LEDC_BASE + 0x570)
/* LEDC_CH5_GAMMA_RANGE12_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE12_DUTY_NUM    0x000003FF
#define LEDC_CH5_GAMMA_RANGE12_DUTY_NUM_M  ((LEDC_CH5_GAMMA_RANGE12_DUTY_NUM_V)<<(LEDC_CH5_GAMMA_RANGE12_DUTY_NUM_S))
#define LEDC_CH5_GAMMA_RANGE12_DUTY_NUM_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE12_DUTY_NUM_S  21
/* LEDC_CH5_GAMMA_RANGE12_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE12_SCALE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE12_SCALE_M  ((LEDC_CH5_GAMMA_RANGE12_SCALE_V)<<(LEDC_CH5_GAMMA_RANGE12_SCALE_S))
#define LEDC_CH5_GAMMA_RANGE12_SCALE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE12_SCALE_S  11
/* LEDC_CH5_GAMMA_RANGE12_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE12_DUTY_CYCLE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE12_DUTY_CYCLE_M  ((LEDC_CH5_GAMMA_RANGE12_DUTY_CYCLE_V)<<(LEDC_CH5_GAMMA_RANGE12_DUTY_CYCLE_S))
#define LEDC_CH5_GAMMA_RANGE12_DUTY_CYCLE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE12_DUTY_CYCLE_S  1
/* LEDC_CH5_GAMMA_RANGE12_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH5_GAMMA_RANGE12_DUTY_INC    (BIT(0))
#define LEDC_CH5_GAMMA_RANGE12_DUTY_INC_M  (BIT(0))
#define LEDC_CH5_GAMMA_RANGE12_DUTY_INC_V  0x1
#define LEDC_CH5_GAMMA_RANGE12_DUTY_INC_S  0

#define LEDC_CH5_GAMMA_RANGE13_REG  (DR_REG_LEDC_BASE + 0x574)
/* LEDC_CH5_GAMMA_RANGE13_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE13_DUTY_NUM    0x000003FF
#define LEDC_CH5_GAMMA_RANGE13_DUTY_NUM_M  ((LEDC_CH5_GAMMA_RANGE13_DUTY_NUM_V)<<(LEDC_CH5_GAMMA_RANGE13_DUTY_NUM_S))
#define LEDC_CH5_GAMMA_RANGE13_DUTY_NUM_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE13_DUTY_NUM_S  21
/* LEDC_CH5_GAMMA_RANGE13_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE13_SCALE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE13_SCALE_M  ((LEDC_CH5_GAMMA_RANGE13_SCALE_V)<<(LEDC_CH5_GAMMA_RANGE13_SCALE_S))
#define LEDC_CH5_GAMMA_RANGE13_SCALE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE13_SCALE_S  11
/* LEDC_CH5_GAMMA_RANGE13_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE13_DUTY_CYCLE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE13_DUTY_CYCLE_M  ((LEDC_CH5_GAMMA_RANGE13_DUTY_CYCLE_V)<<(LEDC_CH5_GAMMA_RANGE13_DUTY_CYCLE_S))
#define LEDC_CH5_GAMMA_RANGE13_DUTY_CYCLE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE13_DUTY_CYCLE_S  1
/* LEDC_CH5_GAMMA_RANGE13_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH5_GAMMA_RANGE13_DUTY_INC    (BIT(0))
#define LEDC_CH5_GAMMA_RANGE13_DUTY_INC_M  (BIT(0))
#define LEDC_CH5_GAMMA_RANGE13_DUTY_INC_V  0x1
#define LEDC_CH5_GAMMA_RANGE13_DUTY_INC_S  0

#define LEDC_CH5_GAMMA_RANGE14_REG  (DR_REG_LEDC_BASE + 0x578)
/* LEDC_CH5_GAMMA_RANGE14_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE14_DUTY_NUM    0x000003FF
#define LEDC_CH5_GAMMA_RANGE14_DUTY_NUM_M  ((LEDC_CH5_GAMMA_RANGE14_DUTY_NUM_V)<<(LEDC_CH5_GAMMA_RANGE14_DUTY_NUM_S))
#define LEDC_CH5_GAMMA_RANGE14_DUTY_NUM_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE14_DUTY_NUM_S  21
/* LEDC_CH5_GAMMA_RANGE14_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE14_SCALE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE14_SCALE_M  ((LEDC_CH5_GAMMA_RANGE14_SCALE_V)<<(LEDC_CH5_GAMMA_RANGE14_SCALE_S))
#define LEDC_CH5_GAMMA_RANGE14_SCALE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE14_SCALE_S  11
/* LEDC_CH5_GAMMA_RANGE14_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE14_DUTY_CYCLE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE14_DUTY_CYCLE_M  ((LEDC_CH5_GAMMA_RANGE14_DUTY_CYCLE_V)<<(LEDC_CH5_GAMMA_RANGE14_DUTY_CYCLE_S))
#define LEDC_CH5_GAMMA_RANGE14_DUTY_CYCLE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE14_DUTY_CYCLE_S  1
/* LEDC_CH5_GAMMA_RANGE14_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH5_GAMMA_RANGE14_DUTY_INC    (BIT(0))
#define LEDC_CH5_GAMMA_RANGE14_DUTY_INC_M  (BIT(0))
#define LEDC_CH5_GAMMA_RANGE14_DUTY_INC_V  0x1
#define LEDC_CH5_GAMMA_RANGE14_DUTY_INC_S  0

#define LEDC_CH5_GAMMA_RANGE15_REG  (DR_REG_LEDC_BASE + 0x57c)
/* LEDC_CH5_GAMMA_RANGE15_DUTY_NUM : R/W ;bitpos:[30:21] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE15_DUTY_NUM    0x000003FF
#define LEDC_CH5_GAMMA_RANGE15_DUTY_NUM_M  ((LEDC_CH5_GAMMA_RANGE15_DUTY_NUM_V)<<(LEDC_CH5_GAMMA_RANGE15_DUTY_NUM_S))
#define LEDC_CH5_GAMMA_RANGE15_DUTY_NUM_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE15_DUTY_NUM_S  21
/* LEDC_CH5_GAMMA_RANGE15_SCALE : R/W ;bitpos:[20:11] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE15_SCALE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE15_SCALE_M  ((LEDC_CH5_GAMMA_RANGE15_SCALE_V)<<(LEDC_CH5_GAMMA_RANGE15_SCALE_S))
#define LEDC_CH5_GAMMA_RANGE15_SCALE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE15_SCALE_S  11
/* LEDC_CH5_GAMMA_RANGE15_DUTY_CYCLE : R/W ;bitpos:[10:1] ;default: 10'h0 ; */
#define LEDC_CH5_GAMMA_RANGE15_DUTY_CYCLE    0x000003FF
#define LEDC_CH5_GAMMA_RANGE15_DUTY_CYCLE_M  ((LEDC_CH5_GAMMA_RANGE15_DUTY_CYCLE_V)<<(LEDC_CH5_GAMMA_RANGE15_DUTY_CYCLE_S))
#define LEDC_CH5_GAMMA_RANGE15_DUTY_CYCLE_V  0x3FF
#define LEDC_CH5_GAMMA_RANGE15_DUTY_CYCLE_S  1
/* LEDC_CH5_GAMMA_RANGE15_DUTY_INC : R/W ;bitpos:[0] ;default: 1'h0 ; */
#define LEDC_CH5_GAMMA_RANGE15_DUTY_INC    (BIT(0))
#define LEDC_CH5_GAMMA_RANGE15_DUTY_INC_M  (BIT(0))
#define LEDC_CH5_GAMMA_RANGE15_DUTY_INC_V  0x1
#define LEDC_CH5_GAMMA_RANGE15_DUTY_INC_S  0

#ifdef __cplusplus
}
#endif
