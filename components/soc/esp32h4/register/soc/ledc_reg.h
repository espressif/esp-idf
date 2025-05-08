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

/** LEDC_CH0_CONF0_REG register
 *  Configuration register 0 for channel 0
 */
#define LEDC_CH0_CONF0_REG (DR_REG_LEDC_BASE + 0x0)
/** LEDC_TIMER_SEL_CH0 : R/W; bitpos: [1:0]; default: 0;
 *  Configures which timer is channel 0 selected.
 *  0: Select timer0
 *  1: Select timer1
 *  2: Select timer2
 *  3: Select timer3
 */
#define LEDC_TIMER_SEL_CH0    0x00000003U
#define LEDC_TIMER_SEL_CH0_M  (LEDC_TIMER_SEL_CH0_V << LEDC_TIMER_SEL_CH0_S)
#define LEDC_TIMER_SEL_CH0_V  0x00000003U
#define LEDC_TIMER_SEL_CH0_S  0
/** LEDC_SIG_OUT_EN_CH0 : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable signal output on channel 0.
 *  0: Signal output disable
 *  1: Signal output enable
 */
#define LEDC_SIG_OUT_EN_CH0    (BIT(2))
#define LEDC_SIG_OUT_EN_CH0_M  (LEDC_SIG_OUT_EN_CH0_V << LEDC_SIG_OUT_EN_CH0_S)
#define LEDC_SIG_OUT_EN_CH0_V  0x00000001U
#define LEDC_SIG_OUT_EN_CH0_S  2
/** LEDC_IDLE_LV_CH0 : R/W; bitpos: [3]; default: 0;
 *  Configures the output value when channel 0 is inactive. Valid only when
 *  LEDC_SIG_OUT_EN_CH0 is 0.
 *  0: Output level is low
 *  1: Output level is high
 */
#define LEDC_IDLE_LV_CH0    (BIT(3))
#define LEDC_IDLE_LV_CH0_M  (LEDC_IDLE_LV_CH0_V << LEDC_IDLE_LV_CH0_S)
#define LEDC_IDLE_LV_CH0_V  0x00000001U
#define LEDC_IDLE_LV_CH0_S  3
/** LEDC_PARA_UP_CH0 : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to update LEDC_HPOINT_CH0, LEDC_DUTY_START_CH0,
 *  LEDC_SIG_OUT_EN_CH0, LEDC_TIMER_SEL_CH0, LEDC_OVF_CNT_EN_CH0 fields and duty cycle
 *  range configuration for channel 0, and will be automatically cleared by hardware.
 *  0: Invalid. No effect
 *  1: Update
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
 *  Configures whether or not to enable the ovf_cnt of channel 0.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_OVF_CNT_EN_CH0    (BIT(15))
#define LEDC_OVF_CNT_EN_CH0_M  (LEDC_OVF_CNT_EN_CH0_V << LEDC_OVF_CNT_EN_CH0_S)
#define LEDC_OVF_CNT_EN_CH0_V  0x00000001U
#define LEDC_OVF_CNT_EN_CH0_S  15
/** LEDC_OVF_CNT_RESET_CH0 : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to reset the  ovf_cnt of channel 0.
 *  0: Invalid. No effect
 *  1: Reset the ovf_cnt
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
 *  Configures whether the duty cycle fading configurations take effect.
 *  0: Not take effect
 *  1: Take effect
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
 *  Configures which timer is channel 1 selected.
 *  0: Select timer0
 *  1: Select timer1
 *  2: Select timer2
 *  3: Select timer3
 */
#define LEDC_TIMER_SEL_CH1    0x00000003U
#define LEDC_TIMER_SEL_CH1_M  (LEDC_TIMER_SEL_CH1_V << LEDC_TIMER_SEL_CH1_S)
#define LEDC_TIMER_SEL_CH1_V  0x00000003U
#define LEDC_TIMER_SEL_CH1_S  0
/** LEDC_SIG_OUT_EN_CH1 : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable signal output on channel 1.
 *  0: Signal output disable
 *  1: Signal output enable
 */
#define LEDC_SIG_OUT_EN_CH1    (BIT(2))
#define LEDC_SIG_OUT_EN_CH1_M  (LEDC_SIG_OUT_EN_CH1_V << LEDC_SIG_OUT_EN_CH1_S)
#define LEDC_SIG_OUT_EN_CH1_V  0x00000001U
#define LEDC_SIG_OUT_EN_CH1_S  2
/** LEDC_IDLE_LV_CH1 : R/W; bitpos: [3]; default: 0;
 *  Configures the output value when channel 1 is inactive. Valid only when
 *  LEDC_SIG_OUT_EN_CH1 is 0.
 *  0: Output level is low
 *  1: Output level is high
 */
#define LEDC_IDLE_LV_CH1    (BIT(3))
#define LEDC_IDLE_LV_CH1_M  (LEDC_IDLE_LV_CH1_V << LEDC_IDLE_LV_CH1_S)
#define LEDC_IDLE_LV_CH1_V  0x00000001U
#define LEDC_IDLE_LV_CH1_S  3
/** LEDC_PARA_UP_CH1 : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to update LEDC_HPOINT_CH1, LEDC_DUTY_START_CH1,
 *  LEDC_SIG_OUT_EN_CH1, LEDC_TIMER_SEL_CH1, LEDC_OVF_CNT_EN_CH1 fields and duty cycle
 *  range configuration for channel 1, and will be automatically cleared by hardware.
 *  0: Invalid. No effect
 *  1: Update
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
 *  Configures whether or not to enable the ovf_cnt of channel 1.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_OVF_CNT_EN_CH1    (BIT(15))
#define LEDC_OVF_CNT_EN_CH1_M  (LEDC_OVF_CNT_EN_CH1_V << LEDC_OVF_CNT_EN_CH1_S)
#define LEDC_OVF_CNT_EN_CH1_V  0x00000001U
#define LEDC_OVF_CNT_EN_CH1_S  15
/** LEDC_OVF_CNT_RESET_CH1 : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to reset the  ovf_cnt of channel 1.
 *  0: Invalid. No effect
 *  1: Reset the ovf_cnt
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
 *  Configures whether the duty cycle fading configurations take effect.
 *  0: Not take effect
 *  1: Take effect
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
 *  Configures which timer is channel 2 selected.
 *  0: Select timer0
 *  1: Select timer1
 *  2: Select timer2
 *  3: Select timer3
 */
#define LEDC_TIMER_SEL_CH2    0x00000003U
#define LEDC_TIMER_SEL_CH2_M  (LEDC_TIMER_SEL_CH2_V << LEDC_TIMER_SEL_CH2_S)
#define LEDC_TIMER_SEL_CH2_V  0x00000003U
#define LEDC_TIMER_SEL_CH2_S  0
/** LEDC_SIG_OUT_EN_CH2 : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable signal output on channel 2.
 *  0: Signal output disable
 *  1: Signal output enable
 */
#define LEDC_SIG_OUT_EN_CH2    (BIT(2))
#define LEDC_SIG_OUT_EN_CH2_M  (LEDC_SIG_OUT_EN_CH2_V << LEDC_SIG_OUT_EN_CH2_S)
#define LEDC_SIG_OUT_EN_CH2_V  0x00000001U
#define LEDC_SIG_OUT_EN_CH2_S  2
/** LEDC_IDLE_LV_CH2 : R/W; bitpos: [3]; default: 0;
 *  Configures the output value when channel 2 is inactive. Valid only when
 *  LEDC_SIG_OUT_EN_CH2 is 0.
 *  0: Output level is low
 *  1: Output level is high
 */
#define LEDC_IDLE_LV_CH2    (BIT(3))
#define LEDC_IDLE_LV_CH2_M  (LEDC_IDLE_LV_CH2_V << LEDC_IDLE_LV_CH2_S)
#define LEDC_IDLE_LV_CH2_V  0x00000001U
#define LEDC_IDLE_LV_CH2_S  3
/** LEDC_PARA_UP_CH2 : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to update LEDC_HPOINT_CH2, LEDC_DUTY_START_CH2,
 *  LEDC_SIG_OUT_EN_CH2, LEDC_TIMER_SEL_CH2, LEDC_OVF_CNT_EN_CH2 fields and duty cycle
 *  range configuration for channel 2, and will be automatically cleared by hardware.
 *  0: Invalid. No effect
 *  1: Update
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
 *  Configures whether or not to enable the ovf_cnt of channel 2.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_OVF_CNT_EN_CH2    (BIT(15))
#define LEDC_OVF_CNT_EN_CH2_M  (LEDC_OVF_CNT_EN_CH2_V << LEDC_OVF_CNT_EN_CH2_S)
#define LEDC_OVF_CNT_EN_CH2_V  0x00000001U
#define LEDC_OVF_CNT_EN_CH2_S  15
/** LEDC_OVF_CNT_RESET_CH2 : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to reset the  ovf_cnt of channel 2.
 *  0: Invalid. No effect
 *  1: Reset the ovf_cnt
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
 *  Configures whether the duty cycle fading configurations take effect.
 *  0: Not take effect
 *  1: Take effect
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
 *  Configures which timer is channel 3 selected.
 *  0: Select timer0
 *  1: Select timer1
 *  2: Select timer2
 *  3: Select timer3
 */
#define LEDC_TIMER_SEL_CH3    0x00000003U
#define LEDC_TIMER_SEL_CH3_M  (LEDC_TIMER_SEL_CH3_V << LEDC_TIMER_SEL_CH3_S)
#define LEDC_TIMER_SEL_CH3_V  0x00000003U
#define LEDC_TIMER_SEL_CH3_S  0
/** LEDC_SIG_OUT_EN_CH3 : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable signal output on channel 3.
 *  0: Signal output disable
 *  1: Signal output enable
 */
#define LEDC_SIG_OUT_EN_CH3    (BIT(2))
#define LEDC_SIG_OUT_EN_CH3_M  (LEDC_SIG_OUT_EN_CH3_V << LEDC_SIG_OUT_EN_CH3_S)
#define LEDC_SIG_OUT_EN_CH3_V  0x00000001U
#define LEDC_SIG_OUT_EN_CH3_S  2
/** LEDC_IDLE_LV_CH3 : R/W; bitpos: [3]; default: 0;
 *  Configures the output value when channel 3 is inactive. Valid only when
 *  LEDC_SIG_OUT_EN_CH3 is 0.
 *  0: Output level is low
 *  1: Output level is high
 */
#define LEDC_IDLE_LV_CH3    (BIT(3))
#define LEDC_IDLE_LV_CH3_M  (LEDC_IDLE_LV_CH3_V << LEDC_IDLE_LV_CH3_S)
#define LEDC_IDLE_LV_CH3_V  0x00000001U
#define LEDC_IDLE_LV_CH3_S  3
/** LEDC_PARA_UP_CH3 : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to update LEDC_HPOINT_CH3, LEDC_DUTY_START_CH3,
 *  LEDC_SIG_OUT_EN_CH3, LEDC_TIMER_SEL_CH3, LEDC_OVF_CNT_EN_CH3 fields and duty cycle
 *  range configuration for channel 3, and will be automatically cleared by hardware.
 *  0: Invalid. No effect
 *  1: Update
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
 *  Configures whether or not to enable the ovf_cnt of channel 3.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_OVF_CNT_EN_CH3    (BIT(15))
#define LEDC_OVF_CNT_EN_CH3_M  (LEDC_OVF_CNT_EN_CH3_V << LEDC_OVF_CNT_EN_CH3_S)
#define LEDC_OVF_CNT_EN_CH3_V  0x00000001U
#define LEDC_OVF_CNT_EN_CH3_S  15
/** LEDC_OVF_CNT_RESET_CH3 : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to reset the  ovf_cnt of channel 3.
 *  0: Invalid. No effect
 *  1: Reset the ovf_cnt
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
 *  Configures whether the duty cycle fading configurations take effect.
 *  0: Not take effect
 *  1: Take effect
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
 *  Configures which timer is channel 4 selected.
 *  0: Select timer0
 *  1: Select timer1
 *  2: Select timer2
 *  3: Select timer3
 */
#define LEDC_TIMER_SEL_CH4    0x00000003U
#define LEDC_TIMER_SEL_CH4_M  (LEDC_TIMER_SEL_CH4_V << LEDC_TIMER_SEL_CH4_S)
#define LEDC_TIMER_SEL_CH4_V  0x00000003U
#define LEDC_TIMER_SEL_CH4_S  0
/** LEDC_SIG_OUT_EN_CH4 : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable signal output on channel 4.
 *  0: Signal output disable
 *  1: Signal output enable
 */
#define LEDC_SIG_OUT_EN_CH4    (BIT(2))
#define LEDC_SIG_OUT_EN_CH4_M  (LEDC_SIG_OUT_EN_CH4_V << LEDC_SIG_OUT_EN_CH4_S)
#define LEDC_SIG_OUT_EN_CH4_V  0x00000001U
#define LEDC_SIG_OUT_EN_CH4_S  2
/** LEDC_IDLE_LV_CH4 : R/W; bitpos: [3]; default: 0;
 *  Configures the output value when channel 4 is inactive. Valid only when
 *  LEDC_SIG_OUT_EN_CH4 is 0.
 *  0: Output level is low
 *  1: Output level is high
 */
#define LEDC_IDLE_LV_CH4    (BIT(3))
#define LEDC_IDLE_LV_CH4_M  (LEDC_IDLE_LV_CH4_V << LEDC_IDLE_LV_CH4_S)
#define LEDC_IDLE_LV_CH4_V  0x00000001U
#define LEDC_IDLE_LV_CH4_S  3
/** LEDC_PARA_UP_CH4 : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to update LEDC_HPOINT_CH4, LEDC_DUTY_START_CH4,
 *  LEDC_SIG_OUT_EN_CH4, LEDC_TIMER_SEL_CH4, LEDC_OVF_CNT_EN_CH4 fields and duty cycle
 *  range configuration for channel 4, and will be automatically cleared by hardware.
 *  0: Invalid. No effect
 *  1: Update
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
 *  Configures whether or not to enable the ovf_cnt of channel 4.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_OVF_CNT_EN_CH4    (BIT(15))
#define LEDC_OVF_CNT_EN_CH4_M  (LEDC_OVF_CNT_EN_CH4_V << LEDC_OVF_CNT_EN_CH4_S)
#define LEDC_OVF_CNT_EN_CH4_V  0x00000001U
#define LEDC_OVF_CNT_EN_CH4_S  15
/** LEDC_OVF_CNT_RESET_CH4 : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to reset the  ovf_cnt of channel 4.
 *  0: Invalid. No effect
 *  1: Reset the ovf_cnt
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
 *  Configures whether the duty cycle fading configurations take effect.
 *  0: Not take effect
 *  1: Take effect
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
 *  Configures which timer is channel 5 selected.
 *  0: Select timer0
 *  1: Select timer1
 *  2: Select timer2
 *  3: Select timer3
 */
#define LEDC_TIMER_SEL_CH5    0x00000003U
#define LEDC_TIMER_SEL_CH5_M  (LEDC_TIMER_SEL_CH5_V << LEDC_TIMER_SEL_CH5_S)
#define LEDC_TIMER_SEL_CH5_V  0x00000003U
#define LEDC_TIMER_SEL_CH5_S  0
/** LEDC_SIG_OUT_EN_CH5 : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable signal output on channel 5.
 *  0: Signal output disable
 *  1: Signal output enable
 */
#define LEDC_SIG_OUT_EN_CH5    (BIT(2))
#define LEDC_SIG_OUT_EN_CH5_M  (LEDC_SIG_OUT_EN_CH5_V << LEDC_SIG_OUT_EN_CH5_S)
#define LEDC_SIG_OUT_EN_CH5_V  0x00000001U
#define LEDC_SIG_OUT_EN_CH5_S  2
/** LEDC_IDLE_LV_CH5 : R/W; bitpos: [3]; default: 0;
 *  Configures the output value when channel 5 is inactive. Valid only when
 *  LEDC_SIG_OUT_EN_CH5 is 0.
 *  0: Output level is low
 *  1: Output level is high
 */
#define LEDC_IDLE_LV_CH5    (BIT(3))
#define LEDC_IDLE_LV_CH5_M  (LEDC_IDLE_LV_CH5_V << LEDC_IDLE_LV_CH5_S)
#define LEDC_IDLE_LV_CH5_V  0x00000001U
#define LEDC_IDLE_LV_CH5_S  3
/** LEDC_PARA_UP_CH5 : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to update LEDC_HPOINT_CH5, LEDC_DUTY_START_CH5,
 *  LEDC_SIG_OUT_EN_CH5, LEDC_TIMER_SEL_CH5, LEDC_OVF_CNT_EN_CH5 fields and duty cycle
 *  range configuration for channel 5, and will be automatically cleared by hardware.
 *  0: Invalid. No effect
 *  1: Update
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
 *  Configures whether or not to enable the ovf_cnt of channel 5.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_OVF_CNT_EN_CH5    (BIT(15))
#define LEDC_OVF_CNT_EN_CH5_M  (LEDC_OVF_CNT_EN_CH5_V << LEDC_OVF_CNT_EN_CH5_S)
#define LEDC_OVF_CNT_EN_CH5_V  0x00000001U
#define LEDC_OVF_CNT_EN_CH5_S  15
/** LEDC_OVF_CNT_RESET_CH5 : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to reset the  ovf_cnt of channel 5.
 *  0: Invalid. No effect
 *  1: Reset the ovf_cnt
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
 *  Configures whether the duty cycle fading configurations take effect.
 *  0: Not take effect
 *  1: Take effect
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

/** LEDC_CH6_CONF0_REG register
 *  Configuration register 0 for channel 6
 */
#define LEDC_CH6_CONF0_REG (DR_REG_LEDC_BASE + 0x78)
/** LEDC_TIMER_SEL_CH6 : R/W; bitpos: [1:0]; default: 0;
 *  Configures which timer is channel 6 selected.
 *  0: Select timer0
 *  1: Select timer1
 *  2: Select timer2
 *  3: Select timer3
 */
#define LEDC_TIMER_SEL_CH6    0x00000003U
#define LEDC_TIMER_SEL_CH6_M  (LEDC_TIMER_SEL_CH6_V << LEDC_TIMER_SEL_CH6_S)
#define LEDC_TIMER_SEL_CH6_V  0x00000003U
#define LEDC_TIMER_SEL_CH6_S  0
/** LEDC_SIG_OUT_EN_CH6 : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable signal output on channel 6.
 *  0: Signal output disable
 *  1: Signal output enable
 */
#define LEDC_SIG_OUT_EN_CH6    (BIT(2))
#define LEDC_SIG_OUT_EN_CH6_M  (LEDC_SIG_OUT_EN_CH6_V << LEDC_SIG_OUT_EN_CH6_S)
#define LEDC_SIG_OUT_EN_CH6_V  0x00000001U
#define LEDC_SIG_OUT_EN_CH6_S  2
/** LEDC_IDLE_LV_CH6 : R/W; bitpos: [3]; default: 0;
 *  Configures the output value when channel 6 is inactive. Valid only when
 *  LEDC_SIG_OUT_EN_CH6 is 0.
 *  0: Output level is low
 *  1: Output level is high
 */
#define LEDC_IDLE_LV_CH6    (BIT(3))
#define LEDC_IDLE_LV_CH6_M  (LEDC_IDLE_LV_CH6_V << LEDC_IDLE_LV_CH6_S)
#define LEDC_IDLE_LV_CH6_V  0x00000001U
#define LEDC_IDLE_LV_CH6_S  3
/** LEDC_PARA_UP_CH6 : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to update LEDC_HPOINT_CH6, LEDC_DUTY_START_CH6,
 *  LEDC_SIG_OUT_EN_CH6, LEDC_TIMER_SEL_CH6, LEDC_OVF_CNT_EN_CH6 fields and duty cycle
 *  range configuration for channel 6, and will be automatically cleared by hardware.
 *  0: Invalid. No effect
 *  1: Update
 */
#define LEDC_PARA_UP_CH6    (BIT(4))
#define LEDC_PARA_UP_CH6_M  (LEDC_PARA_UP_CH6_V << LEDC_PARA_UP_CH6_S)
#define LEDC_PARA_UP_CH6_V  0x00000001U
#define LEDC_PARA_UP_CH6_S  4
/** LEDC_OVF_NUM_CH6 : R/W; bitpos: [14:5]; default: 0;
 *  Configures the maximum times of overflow minus 1.The LEDC_OVF_CNT_CH6_INT interrupt
 *  will be triggered when channel 6 overflows for (LEDC_OVF_NUM_CH6 + 1) times.
 */
#define LEDC_OVF_NUM_CH6    0x000003FFU
#define LEDC_OVF_NUM_CH6_M  (LEDC_OVF_NUM_CH6_V << LEDC_OVF_NUM_CH6_S)
#define LEDC_OVF_NUM_CH6_V  0x000003FFU
#define LEDC_OVF_NUM_CH6_S  5
/** LEDC_OVF_CNT_EN_CH6 : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable the ovf_cnt of channel 6.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_OVF_CNT_EN_CH6    (BIT(15))
#define LEDC_OVF_CNT_EN_CH6_M  (LEDC_OVF_CNT_EN_CH6_V << LEDC_OVF_CNT_EN_CH6_S)
#define LEDC_OVF_CNT_EN_CH6_V  0x00000001U
#define LEDC_OVF_CNT_EN_CH6_S  15
/** LEDC_OVF_CNT_RESET_CH6 : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to reset the  ovf_cnt of channel 6.
 *  0: Invalid. No effect
 *  1: Reset the ovf_cnt
 */
#define LEDC_OVF_CNT_RESET_CH6    (BIT(16))
#define LEDC_OVF_CNT_RESET_CH6_M  (LEDC_OVF_CNT_RESET_CH6_V << LEDC_OVF_CNT_RESET_CH6_S)
#define LEDC_OVF_CNT_RESET_CH6_V  0x00000001U
#define LEDC_OVF_CNT_RESET_CH6_S  16

/** LEDC_CH6_HPOINT_REG register
 *  High point register for channel 6
 */
#define LEDC_CH6_HPOINT_REG (DR_REG_LEDC_BASE + 0x7c)
/** LEDC_HPOINT_CH6 : R/W; bitpos: [19:0]; default: 0;
 *  Configures high point of signal output on channel 6. The output value changes to
 *  high when the selected timers has reached the value specified by this register.
 */
#define LEDC_HPOINT_CH6    0x000FFFFFU
#define LEDC_HPOINT_CH6_M  (LEDC_HPOINT_CH6_V << LEDC_HPOINT_CH6_S)
#define LEDC_HPOINT_CH6_V  0x000FFFFFU
#define LEDC_HPOINT_CH6_S  0

/** LEDC_CH6_DUTY_REG register
 *  Initial duty cycle register for channel 6
 */
#define LEDC_CH6_DUTY_REG (DR_REG_LEDC_BASE + 0x80)
/** LEDC_DUTY_CH6 : R/W; bitpos: [24:0]; default: 0;
 *  Configures the duty of signal output on channel 6.
 */
#define LEDC_DUTY_CH6    0x01FFFFFFU
#define LEDC_DUTY_CH6_M  (LEDC_DUTY_CH6_V << LEDC_DUTY_CH6_S)
#define LEDC_DUTY_CH6_V  0x01FFFFFFU
#define LEDC_DUTY_CH6_S  0

/** LEDC_CH6_CONF1_REG register
 *  Configuration register 1 for channel 6
 */
#define LEDC_CH6_CONF1_REG (DR_REG_LEDC_BASE + 0x84)
/** LEDC_DUTY_START_CH6 : R/W/SC; bitpos: [31]; default: 0;
 *  Configures whether the duty cycle fading configurations take effect.
 *  0: Not take effect
 *  1: Take effect
 */
#define LEDC_DUTY_START_CH6    (BIT(31))
#define LEDC_DUTY_START_CH6_M  (LEDC_DUTY_START_CH6_V << LEDC_DUTY_START_CH6_S)
#define LEDC_DUTY_START_CH6_V  0x00000001U
#define LEDC_DUTY_START_CH6_S  31

/** LEDC_CH6_DUTY_R_REG register
 *  Current duty cycle register for channel 6
 */
#define LEDC_CH6_DUTY_R_REG (DR_REG_LEDC_BASE + 0x88)
/** LEDC_DUTY_CH6_R : RO; bitpos: [24:0]; default: 0;
 *  Represents the current duty of output signal on channel 6.
 */
#define LEDC_DUTY_CH6_R    0x01FFFFFFU
#define LEDC_DUTY_CH6_R_M  (LEDC_DUTY_CH6_R_V << LEDC_DUTY_CH6_R_S)
#define LEDC_DUTY_CH6_R_V  0x01FFFFFFU
#define LEDC_DUTY_CH6_R_S  0

/** LEDC_CH7_CONF0_REG register
 *  Configuration register 0 for channel 7
 */
#define LEDC_CH7_CONF0_REG (DR_REG_LEDC_BASE + 0x8c)
/** LEDC_TIMER_SEL_CH7 : R/W; bitpos: [1:0]; default: 0;
 *  Configures which timer is channel 7 selected.
 *  0: Select timer0
 *  1: Select timer1
 *  2: Select timer2
 *  3: Select timer3
 */
#define LEDC_TIMER_SEL_CH7    0x00000003U
#define LEDC_TIMER_SEL_CH7_M  (LEDC_TIMER_SEL_CH7_V << LEDC_TIMER_SEL_CH7_S)
#define LEDC_TIMER_SEL_CH7_V  0x00000003U
#define LEDC_TIMER_SEL_CH7_S  0
/** LEDC_SIG_OUT_EN_CH7 : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable signal output on channel 7.
 *  0: Signal output disable
 *  1: Signal output enable
 */
#define LEDC_SIG_OUT_EN_CH7    (BIT(2))
#define LEDC_SIG_OUT_EN_CH7_M  (LEDC_SIG_OUT_EN_CH7_V << LEDC_SIG_OUT_EN_CH7_S)
#define LEDC_SIG_OUT_EN_CH7_V  0x00000001U
#define LEDC_SIG_OUT_EN_CH7_S  2
/** LEDC_IDLE_LV_CH7 : R/W; bitpos: [3]; default: 0;
 *  Configures the output value when channel 7 is inactive. Valid only when
 *  LEDC_SIG_OUT_EN_CH7 is 0.
 *  0: Output level is low
 *  1: Output level is high
 */
#define LEDC_IDLE_LV_CH7    (BIT(3))
#define LEDC_IDLE_LV_CH7_M  (LEDC_IDLE_LV_CH7_V << LEDC_IDLE_LV_CH7_S)
#define LEDC_IDLE_LV_CH7_V  0x00000001U
#define LEDC_IDLE_LV_CH7_S  3
/** LEDC_PARA_UP_CH7 : WT; bitpos: [4]; default: 0;
 *  Configures whether or not to update LEDC_HPOINT_CH7, LEDC_DUTY_START_CH7,
 *  LEDC_SIG_OUT_EN_CH7, LEDC_TIMER_SEL_CH7, LEDC_OVF_CNT_EN_CH7 fields and duty cycle
 *  range configuration for channel 7, and will be automatically cleared by hardware.
 *  0: Invalid. No effect
 *  1: Update
 */
#define LEDC_PARA_UP_CH7    (BIT(4))
#define LEDC_PARA_UP_CH7_M  (LEDC_PARA_UP_CH7_V << LEDC_PARA_UP_CH7_S)
#define LEDC_PARA_UP_CH7_V  0x00000001U
#define LEDC_PARA_UP_CH7_S  4
/** LEDC_OVF_NUM_CH7 : R/W; bitpos: [14:5]; default: 0;
 *  Configures the maximum times of overflow minus 1.The LEDC_OVF_CNT_CH7_INT interrupt
 *  will be triggered when channel 7 overflows for (LEDC_OVF_NUM_CH7 + 1) times.
 */
#define LEDC_OVF_NUM_CH7    0x000003FFU
#define LEDC_OVF_NUM_CH7_M  (LEDC_OVF_NUM_CH7_V << LEDC_OVF_NUM_CH7_S)
#define LEDC_OVF_NUM_CH7_V  0x000003FFU
#define LEDC_OVF_NUM_CH7_S  5
/** LEDC_OVF_CNT_EN_CH7 : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable the ovf_cnt of channel 7.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_OVF_CNT_EN_CH7    (BIT(15))
#define LEDC_OVF_CNT_EN_CH7_M  (LEDC_OVF_CNT_EN_CH7_V << LEDC_OVF_CNT_EN_CH7_S)
#define LEDC_OVF_CNT_EN_CH7_V  0x00000001U
#define LEDC_OVF_CNT_EN_CH7_S  15
/** LEDC_OVF_CNT_RESET_CH7 : WT; bitpos: [16]; default: 0;
 *  Configures whether or not to reset the  ovf_cnt of channel 7.
 *  0: Invalid. No effect
 *  1: Reset the ovf_cnt
 */
#define LEDC_OVF_CNT_RESET_CH7    (BIT(16))
#define LEDC_OVF_CNT_RESET_CH7_M  (LEDC_OVF_CNT_RESET_CH7_V << LEDC_OVF_CNT_RESET_CH7_S)
#define LEDC_OVF_CNT_RESET_CH7_V  0x00000001U
#define LEDC_OVF_CNT_RESET_CH7_S  16

/** LEDC_CH7_HPOINT_REG register
 *  High point register for channel 7
 */
#define LEDC_CH7_HPOINT_REG (DR_REG_LEDC_BASE + 0x90)
/** LEDC_HPOINT_CH7 : R/W; bitpos: [19:0]; default: 0;
 *  Configures high point of signal output on channel 7. The output value changes to
 *  high when the selected timers has reached the value specified by this register.
 */
#define LEDC_HPOINT_CH7    0x000FFFFFU
#define LEDC_HPOINT_CH7_M  (LEDC_HPOINT_CH7_V << LEDC_HPOINT_CH7_S)
#define LEDC_HPOINT_CH7_V  0x000FFFFFU
#define LEDC_HPOINT_CH7_S  0

/** LEDC_CH7_DUTY_REG register
 *  Initial duty cycle register for channel 7
 */
#define LEDC_CH7_DUTY_REG (DR_REG_LEDC_BASE + 0x94)
/** LEDC_DUTY_CH7 : R/W; bitpos: [24:0]; default: 0;
 *  Configures the duty of signal output on channel 7.
 */
#define LEDC_DUTY_CH7    0x01FFFFFFU
#define LEDC_DUTY_CH7_M  (LEDC_DUTY_CH7_V << LEDC_DUTY_CH7_S)
#define LEDC_DUTY_CH7_V  0x01FFFFFFU
#define LEDC_DUTY_CH7_S  0

/** LEDC_CH7_CONF1_REG register
 *  Configuration register 1 for channel 7
 */
#define LEDC_CH7_CONF1_REG (DR_REG_LEDC_BASE + 0x98)
/** LEDC_DUTY_START_CH7 : R/W/SC; bitpos: [31]; default: 0;
 *  Configures whether the duty cycle fading configurations take effect.
 *  0: Not take effect
 *  1: Take effect
 */
#define LEDC_DUTY_START_CH7    (BIT(31))
#define LEDC_DUTY_START_CH7_M  (LEDC_DUTY_START_CH7_V << LEDC_DUTY_START_CH7_S)
#define LEDC_DUTY_START_CH7_V  0x00000001U
#define LEDC_DUTY_START_CH7_S  31

/** LEDC_CH7_DUTY_R_REG register
 *  Current duty cycle register for channel 7
 */
#define LEDC_CH7_DUTY_R_REG (DR_REG_LEDC_BASE + 0x9c)
/** LEDC_DUTY_CH7_R : RO; bitpos: [24:0]; default: 0;
 *  Represents the current duty of output signal on channel 7.
 */
#define LEDC_DUTY_CH7_R    0x01FFFFFFU
#define LEDC_DUTY_CH7_R_M  (LEDC_DUTY_CH7_R_V << LEDC_DUTY_CH7_R_S)
#define LEDC_DUTY_CH7_R_V  0x01FFFFFFU
#define LEDC_DUTY_CH7_R_S  0

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
 *  Configures whether or not to pause the counter in timer 0.
 *  0: Normal
 *  1: Pause
 */
#define LEDC_TIMER0_PAUSE    (BIT(23))
#define LEDC_TIMER0_PAUSE_M  (LEDC_TIMER0_PAUSE_V << LEDC_TIMER0_PAUSE_S)
#define LEDC_TIMER0_PAUSE_V  0x00000001U
#define LEDC_TIMER0_PAUSE_S  23
/** LEDC_TIMER0_RST : R/W; bitpos: [24]; default: 1;
 *  Configures whether or not to reset timer 0. The counter will show 0 after reset.
 *  0: Not reset
 *  1: Reset
 */
#define LEDC_TIMER0_RST    (BIT(24))
#define LEDC_TIMER0_RST_M  (LEDC_TIMER0_RST_V << LEDC_TIMER0_RST_S)
#define LEDC_TIMER0_RST_V  0x00000001U
#define LEDC_TIMER0_RST_S  24
/** LEDC_TIMER0_PARA_UP : WT; bitpos: [26]; default: 0;
 *  Configures whether or not to update LEDC_CLK_DIV_TIMER0 and LEDC_TIMER0_DUTY_RES.
 *  0: Invalid. No effect
 *  1: Update
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
 *  Configures whether or not to pause the counter in timer 1.
 *  0: Normal
 *  1: Pause
 */
#define LEDC_TIMER1_PAUSE    (BIT(23))
#define LEDC_TIMER1_PAUSE_M  (LEDC_TIMER1_PAUSE_V << LEDC_TIMER1_PAUSE_S)
#define LEDC_TIMER1_PAUSE_V  0x00000001U
#define LEDC_TIMER1_PAUSE_S  23
/** LEDC_TIMER1_RST : R/W; bitpos: [24]; default: 1;
 *  Configures whether or not to reset timer 1. The counter will show 0 after reset.
 *  0: Not reset
 *  1: Reset
 */
#define LEDC_TIMER1_RST    (BIT(24))
#define LEDC_TIMER1_RST_M  (LEDC_TIMER1_RST_V << LEDC_TIMER1_RST_S)
#define LEDC_TIMER1_RST_V  0x00000001U
#define LEDC_TIMER1_RST_S  24
/** LEDC_TIMER1_PARA_UP : WT; bitpos: [26]; default: 0;
 *  Configures whether or not to update LEDC_CLK_DIV_TIMER1 and LEDC_TIMER1_DUTY_RES.
 *  0: Invalid. No effect
 *  1: Update
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
 *  Configures whether or not to pause the counter in timer 2.
 *  0: Normal
 *  1: Pause
 */
#define LEDC_TIMER2_PAUSE    (BIT(23))
#define LEDC_TIMER2_PAUSE_M  (LEDC_TIMER2_PAUSE_V << LEDC_TIMER2_PAUSE_S)
#define LEDC_TIMER2_PAUSE_V  0x00000001U
#define LEDC_TIMER2_PAUSE_S  23
/** LEDC_TIMER2_RST : R/W; bitpos: [24]; default: 1;
 *  Configures whether or not to reset timer 2. The counter will show 0 after reset.
 *  0: Not reset
 *  1: Reset
 */
#define LEDC_TIMER2_RST    (BIT(24))
#define LEDC_TIMER2_RST_M  (LEDC_TIMER2_RST_V << LEDC_TIMER2_RST_S)
#define LEDC_TIMER2_RST_V  0x00000001U
#define LEDC_TIMER2_RST_S  24
/** LEDC_TIMER2_PARA_UP : WT; bitpos: [26]; default: 0;
 *  Configures whether or not to update LEDC_CLK_DIV_TIMER2 and LEDC_TIMER2_DUTY_RES.
 *  0: Invalid. No effect
 *  1: Update
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
 *  Configures whether or not to pause the counter in timer 3.
 *  0: Normal
 *  1: Pause
 */
#define LEDC_TIMER3_PAUSE    (BIT(23))
#define LEDC_TIMER3_PAUSE_M  (LEDC_TIMER3_PAUSE_V << LEDC_TIMER3_PAUSE_S)
#define LEDC_TIMER3_PAUSE_V  0x00000001U
#define LEDC_TIMER3_PAUSE_S  23
/** LEDC_TIMER3_RST : R/W; bitpos: [24]; default: 1;
 *  Configures whether or not to reset timer 3. The counter will show 0 after reset.
 *  0: Not reset
 *  1: Reset
 */
#define LEDC_TIMER3_RST    (BIT(24))
#define LEDC_TIMER3_RST_M  (LEDC_TIMER3_RST_V << LEDC_TIMER3_RST_S)
#define LEDC_TIMER3_RST_V  0x00000001U
#define LEDC_TIMER3_RST_S  24
/** LEDC_TIMER3_PARA_UP : WT; bitpos: [26]; default: 0;
 *  Configures whether or not to update LEDC_CLK_DIV_TIMER3 and LEDC_TIMER3_DUTY_RES.
 *  0: Invalid. No effect
 *  1: Update
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
/** LEDC_DUTY_CHNG_END_CH6_INT_RAW : R/WTC/SS; bitpos: [10]; default: 0;
 *  Raw status bit: The raw interrupt status of LEDC_DUTY_CHNG_END_CH6_INT. Triggered
 *  when the fading of duty has finished.
 */
#define LEDC_DUTY_CHNG_END_CH6_INT_RAW    (BIT(10))
#define LEDC_DUTY_CHNG_END_CH6_INT_RAW_M  (LEDC_DUTY_CHNG_END_CH6_INT_RAW_V << LEDC_DUTY_CHNG_END_CH6_INT_RAW_S)
#define LEDC_DUTY_CHNG_END_CH6_INT_RAW_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH6_INT_RAW_S  10
/** LEDC_DUTY_CHNG_END_CH7_INT_RAW : R/WTC/SS; bitpos: [11]; default: 0;
 *  Raw status bit: The raw interrupt status of LEDC_DUTY_CHNG_END_CH7_INT. Triggered
 *  when the fading of duty has finished.
 */
#define LEDC_DUTY_CHNG_END_CH7_INT_RAW    (BIT(11))
#define LEDC_DUTY_CHNG_END_CH7_INT_RAW_M  (LEDC_DUTY_CHNG_END_CH7_INT_RAW_V << LEDC_DUTY_CHNG_END_CH7_INT_RAW_S)
#define LEDC_DUTY_CHNG_END_CH7_INT_RAW_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH7_INT_RAW_S  11
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
/** LEDC_OVF_CNT_CH6_INT_RAW : R/WTC/SS; bitpos: [18]; default: 0;
 *  Raw status bit: The raw interrupt status of LEDC_OVF_CNT_CH6_INT. Triggered when
 *  the ovf_cnt has reached the value specified by LEDC_OVF_NUM_CH6.
 */
#define LEDC_OVF_CNT_CH6_INT_RAW    (BIT(18))
#define LEDC_OVF_CNT_CH6_INT_RAW_M  (LEDC_OVF_CNT_CH6_INT_RAW_V << LEDC_OVF_CNT_CH6_INT_RAW_S)
#define LEDC_OVF_CNT_CH6_INT_RAW_V  0x00000001U
#define LEDC_OVF_CNT_CH6_INT_RAW_S  18
/** LEDC_OVF_CNT_CH7_INT_RAW : R/WTC/SS; bitpos: [19]; default: 0;
 *  Raw status bit: The raw interrupt status of LEDC_OVF_CNT_CH7_INT. Triggered when
 *  the ovf_cnt has reached the value specified by LEDC_OVF_NUM_CH7.
 */
#define LEDC_OVF_CNT_CH7_INT_RAW    (BIT(19))
#define LEDC_OVF_CNT_CH7_INT_RAW_M  (LEDC_OVF_CNT_CH7_INT_RAW_V << LEDC_OVF_CNT_CH7_INT_RAW_S)
#define LEDC_OVF_CNT_CH7_INT_RAW_V  0x00000001U
#define LEDC_OVF_CNT_CH7_INT_RAW_S  19

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
/** LEDC_DUTY_CHNG_END_CH6_INT_ST : RO; bitpos: [10]; default: 0;
 *  Masked status bit: The masked interrupt status of LEDC_DUTY_CHNG_END_CH6_INT. Valid
 *  only when LEDC_DUTY_CHNG_END_CH6_INT_ENA is set to 1.
 */
#define LEDC_DUTY_CHNG_END_CH6_INT_ST    (BIT(10))
#define LEDC_DUTY_CHNG_END_CH6_INT_ST_M  (LEDC_DUTY_CHNG_END_CH6_INT_ST_V << LEDC_DUTY_CHNG_END_CH6_INT_ST_S)
#define LEDC_DUTY_CHNG_END_CH6_INT_ST_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH6_INT_ST_S  10
/** LEDC_DUTY_CHNG_END_CH7_INT_ST : RO; bitpos: [11]; default: 0;
 *  Masked status bit: The masked interrupt status of LEDC_DUTY_CHNG_END_CH7_INT. Valid
 *  only when LEDC_DUTY_CHNG_END_CH7_INT_ENA is set to 1.
 */
#define LEDC_DUTY_CHNG_END_CH7_INT_ST    (BIT(11))
#define LEDC_DUTY_CHNG_END_CH7_INT_ST_M  (LEDC_DUTY_CHNG_END_CH7_INT_ST_V << LEDC_DUTY_CHNG_END_CH7_INT_ST_S)
#define LEDC_DUTY_CHNG_END_CH7_INT_ST_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH7_INT_ST_S  11
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
/** LEDC_OVF_CNT_CH6_INT_ST : RO; bitpos: [18]; default: 0;
 *  Masked status bit: The masked interrupt status of LEDC_OVF_CNT_CH6_INT. Valid only
 *  when LEDC_OVF_CNT_CH6_INT_ENA is set to 1.
 */
#define LEDC_OVF_CNT_CH6_INT_ST    (BIT(18))
#define LEDC_OVF_CNT_CH6_INT_ST_M  (LEDC_OVF_CNT_CH6_INT_ST_V << LEDC_OVF_CNT_CH6_INT_ST_S)
#define LEDC_OVF_CNT_CH6_INT_ST_V  0x00000001U
#define LEDC_OVF_CNT_CH6_INT_ST_S  18
/** LEDC_OVF_CNT_CH7_INT_ST : RO; bitpos: [19]; default: 0;
 *  Masked status bit: The masked interrupt status of LEDC_OVF_CNT_CH7_INT. Valid only
 *  when LEDC_OVF_CNT_CH7_INT_ENA is set to 1.
 */
#define LEDC_OVF_CNT_CH7_INT_ST    (BIT(19))
#define LEDC_OVF_CNT_CH7_INT_ST_M  (LEDC_OVF_CNT_CH7_INT_ST_V << LEDC_OVF_CNT_CH7_INT_ST_S)
#define LEDC_OVF_CNT_CH7_INT_ST_V  0x00000001U
#define LEDC_OVF_CNT_CH7_INT_ST_S  19

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
/** LEDC_DUTY_CHNG_END_CH6_INT_ENA : R/W; bitpos: [10]; default: 0;
 *  Enable bit: Write 1 to enable LEDC_DUTY_CHNG_END_CH6_INT.
 */
#define LEDC_DUTY_CHNG_END_CH6_INT_ENA    (BIT(10))
#define LEDC_DUTY_CHNG_END_CH6_INT_ENA_M  (LEDC_DUTY_CHNG_END_CH6_INT_ENA_V << LEDC_DUTY_CHNG_END_CH6_INT_ENA_S)
#define LEDC_DUTY_CHNG_END_CH6_INT_ENA_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH6_INT_ENA_S  10
/** LEDC_DUTY_CHNG_END_CH7_INT_ENA : R/W; bitpos: [11]; default: 0;
 *  Enable bit: Write 1 to enable LEDC_DUTY_CHNG_END_CH7_INT.
 */
#define LEDC_DUTY_CHNG_END_CH7_INT_ENA    (BIT(11))
#define LEDC_DUTY_CHNG_END_CH7_INT_ENA_M  (LEDC_DUTY_CHNG_END_CH7_INT_ENA_V << LEDC_DUTY_CHNG_END_CH7_INT_ENA_S)
#define LEDC_DUTY_CHNG_END_CH7_INT_ENA_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH7_INT_ENA_S  11
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
/** LEDC_OVF_CNT_CH6_INT_ENA : R/W; bitpos: [18]; default: 0;
 *  Enable bit: Write 1 to enable LEDC_OVF_CNT_CH6_INT.
 */
#define LEDC_OVF_CNT_CH6_INT_ENA    (BIT(18))
#define LEDC_OVF_CNT_CH6_INT_ENA_M  (LEDC_OVF_CNT_CH6_INT_ENA_V << LEDC_OVF_CNT_CH6_INT_ENA_S)
#define LEDC_OVF_CNT_CH6_INT_ENA_V  0x00000001U
#define LEDC_OVF_CNT_CH6_INT_ENA_S  18
/** LEDC_OVF_CNT_CH7_INT_ENA : R/W; bitpos: [19]; default: 0;
 *  Enable bit: Write 1 to enable LEDC_OVF_CNT_CH7_INT.
 */
#define LEDC_OVF_CNT_CH7_INT_ENA    (BIT(19))
#define LEDC_OVF_CNT_CH7_INT_ENA_M  (LEDC_OVF_CNT_CH7_INT_ENA_V << LEDC_OVF_CNT_CH7_INT_ENA_S)
#define LEDC_OVF_CNT_CH7_INT_ENA_V  0x00000001U
#define LEDC_OVF_CNT_CH7_INT_ENA_S  19

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
/** LEDC_DUTY_CHNG_END_CH6_INT_CLR : WT; bitpos: [10]; default: 0;
 *  Clear bit: Write 1 to clear LEDC_DUTY_CHNG_END_CH6_INT.
 */
#define LEDC_DUTY_CHNG_END_CH6_INT_CLR    (BIT(10))
#define LEDC_DUTY_CHNG_END_CH6_INT_CLR_M  (LEDC_DUTY_CHNG_END_CH6_INT_CLR_V << LEDC_DUTY_CHNG_END_CH6_INT_CLR_S)
#define LEDC_DUTY_CHNG_END_CH6_INT_CLR_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH6_INT_CLR_S  10
/** LEDC_DUTY_CHNG_END_CH7_INT_CLR : WT; bitpos: [11]; default: 0;
 *  Clear bit: Write 1 to clear LEDC_DUTY_CHNG_END_CH7_INT.
 */
#define LEDC_DUTY_CHNG_END_CH7_INT_CLR    (BIT(11))
#define LEDC_DUTY_CHNG_END_CH7_INT_CLR_M  (LEDC_DUTY_CHNG_END_CH7_INT_CLR_V << LEDC_DUTY_CHNG_END_CH7_INT_CLR_S)
#define LEDC_DUTY_CHNG_END_CH7_INT_CLR_V  0x00000001U
#define LEDC_DUTY_CHNG_END_CH7_INT_CLR_S  11
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
/** LEDC_OVF_CNT_CH6_INT_CLR : WT; bitpos: [18]; default: 0;
 *  Clear bit: Write 1 to clear LEDC_OVF_CNT_CH6_INT.
 */
#define LEDC_OVF_CNT_CH6_INT_CLR    (BIT(18))
#define LEDC_OVF_CNT_CH6_INT_CLR_M  (LEDC_OVF_CNT_CH6_INT_CLR_V << LEDC_OVF_CNT_CH6_INT_CLR_S)
#define LEDC_OVF_CNT_CH6_INT_CLR_V  0x00000001U
#define LEDC_OVF_CNT_CH6_INT_CLR_S  18
/** LEDC_OVF_CNT_CH7_INT_CLR : WT; bitpos: [19]; default: 0;
 *  Clear bit: Write 1 to clear LEDC_OVF_CNT_CH7_INT.
 */
#define LEDC_OVF_CNT_CH7_INT_CLR    (BIT(19))
#define LEDC_OVF_CNT_CH7_INT_CLR_M  (LEDC_OVF_CNT_CH7_INT_CLR_V << LEDC_OVF_CNT_CH7_INT_CLR_S)
#define LEDC_OVF_CNT_CH7_INT_CLR_V  0x00000001U
#define LEDC_OVF_CNT_CH7_INT_CLR_S  19

/** LEDC_CH0_FADE_CONF_REG register
 *  Ledc ch0 fade config register.
 */
#define LEDC_CH0_FADE_CONF_REG (DR_REG_LEDC_BASE + 0x100)
/** LEDC_CH0_FADE_PAUSE : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to pause duty cycle fading of LEDC ch0.
 *  0: Invalid. No effect
 *  1: Pause
 */
#define LEDC_CH0_FADE_PAUSE    (BIT(5))
#define LEDC_CH0_FADE_PAUSE_M  (LEDC_CH0_FADE_PAUSE_V << LEDC_CH0_FADE_PAUSE_S)
#define LEDC_CH0_FADE_PAUSE_V  0x00000001U
#define LEDC_CH0_FADE_PAUSE_S  5
/** LEDC_CH0_FADE_RESUME : WT; bitpos: [6]; default: 0;
 *  Configures whether or nor to resume duty cycle fading of LEDC ch0.
 *  0: Invalid. No effect
 *  1: Resume
 */
#define LEDC_CH0_FADE_RESUME    (BIT(6))
#define LEDC_CH0_FADE_RESUME_M  (LEDC_CH0_FADE_RESUME_V << LEDC_CH0_FADE_RESUME_S)
#define LEDC_CH0_FADE_RESUME_V  0x00000001U
#define LEDC_CH0_FADE_RESUME_S  6

/** LEDC_CH1_FADE_CONF_REG register
 *  Ledc ch1 fade config register.
 */
#define LEDC_CH1_FADE_CONF_REG (DR_REG_LEDC_BASE + 0x104)
/** LEDC_CH1_FADE_PAUSE : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to pause duty cycle fading of LEDC ch1.
 *  0: Invalid. No effect
 *  1: Pause
 */
#define LEDC_CH1_FADE_PAUSE    (BIT(5))
#define LEDC_CH1_FADE_PAUSE_M  (LEDC_CH1_FADE_PAUSE_V << LEDC_CH1_FADE_PAUSE_S)
#define LEDC_CH1_FADE_PAUSE_V  0x00000001U
#define LEDC_CH1_FADE_PAUSE_S  5
/** LEDC_CH1_FADE_RESUME : WT; bitpos: [6]; default: 0;
 *  Configures whether or nor to resume duty cycle fading of LEDC ch1.
 *  0: Invalid. No effect
 *  1: Resume
 */
#define LEDC_CH1_FADE_RESUME    (BIT(6))
#define LEDC_CH1_FADE_RESUME_M  (LEDC_CH1_FADE_RESUME_V << LEDC_CH1_FADE_RESUME_S)
#define LEDC_CH1_FADE_RESUME_V  0x00000001U
#define LEDC_CH1_FADE_RESUME_S  6

/** LEDC_CH2_FADE_CONF_REG register
 *  Ledc ch2 fade config register.
 */
#define LEDC_CH2_FADE_CONF_REG (DR_REG_LEDC_BASE + 0x108)
/** LEDC_CH2_FADE_PAUSE : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to pause duty cycle fading of LEDC ch2.
 *  0: Invalid. No effect
 *  1: Pause
 */
#define LEDC_CH2_FADE_PAUSE    (BIT(5))
#define LEDC_CH2_FADE_PAUSE_M  (LEDC_CH2_FADE_PAUSE_V << LEDC_CH2_FADE_PAUSE_S)
#define LEDC_CH2_FADE_PAUSE_V  0x00000001U
#define LEDC_CH2_FADE_PAUSE_S  5
/** LEDC_CH2_FADE_RESUME : WT; bitpos: [6]; default: 0;
 *  Configures whether or nor to resume duty cycle fading of LEDC ch2.
 *  0: Invalid. No effect
 *  1: Resume
 */
#define LEDC_CH2_FADE_RESUME    (BIT(6))
#define LEDC_CH2_FADE_RESUME_M  (LEDC_CH2_FADE_RESUME_V << LEDC_CH2_FADE_RESUME_S)
#define LEDC_CH2_FADE_RESUME_V  0x00000001U
#define LEDC_CH2_FADE_RESUME_S  6

/** LEDC_CH3_FADE_CONF_REG register
 *  Ledc ch3 fade config register.
 */
#define LEDC_CH3_FADE_CONF_REG (DR_REG_LEDC_BASE + 0x10c)
/** LEDC_CH3_FADE_PAUSE : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to pause duty cycle fading of LEDC ch3.
 *  0: Invalid. No effect
 *  1: Pause
 */
#define LEDC_CH3_FADE_PAUSE    (BIT(5))
#define LEDC_CH3_FADE_PAUSE_M  (LEDC_CH3_FADE_PAUSE_V << LEDC_CH3_FADE_PAUSE_S)
#define LEDC_CH3_FADE_PAUSE_V  0x00000001U
#define LEDC_CH3_FADE_PAUSE_S  5
/** LEDC_CH3_FADE_RESUME : WT; bitpos: [6]; default: 0;
 *  Configures whether or nor to resume duty cycle fading of LEDC ch3.
 *  0: Invalid. No effect
 *  1: Resume
 */
#define LEDC_CH3_FADE_RESUME    (BIT(6))
#define LEDC_CH3_FADE_RESUME_M  (LEDC_CH3_FADE_RESUME_V << LEDC_CH3_FADE_RESUME_S)
#define LEDC_CH3_FADE_RESUME_V  0x00000001U
#define LEDC_CH3_FADE_RESUME_S  6

/** LEDC_CH4_FADE_CONF_REG register
 *  Ledc ch4 fade config register.
 */
#define LEDC_CH4_FADE_CONF_REG (DR_REG_LEDC_BASE + 0x110)
/** LEDC_CH4_FADE_PAUSE : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to pause duty cycle fading of LEDC ch4.
 *  0: Invalid. No effect
 *  1: Pause
 */
#define LEDC_CH4_FADE_PAUSE    (BIT(5))
#define LEDC_CH4_FADE_PAUSE_M  (LEDC_CH4_FADE_PAUSE_V << LEDC_CH4_FADE_PAUSE_S)
#define LEDC_CH4_FADE_PAUSE_V  0x00000001U
#define LEDC_CH4_FADE_PAUSE_S  5
/** LEDC_CH4_FADE_RESUME : WT; bitpos: [6]; default: 0;
 *  Configures whether or nor to resume duty cycle fading of LEDC ch4.
 *  0: Invalid. No effect
 *  1: Resume
 */
#define LEDC_CH4_FADE_RESUME    (BIT(6))
#define LEDC_CH4_FADE_RESUME_M  (LEDC_CH4_FADE_RESUME_V << LEDC_CH4_FADE_RESUME_S)
#define LEDC_CH4_FADE_RESUME_V  0x00000001U
#define LEDC_CH4_FADE_RESUME_S  6

/** LEDC_CH5_FADE_CONF_REG register
 *  Ledc ch5 fade config register.
 */
#define LEDC_CH5_FADE_CONF_REG (DR_REG_LEDC_BASE + 0x114)
/** LEDC_CH5_FADE_PAUSE : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to pause duty cycle fading of LEDC ch5.
 *  0: Invalid. No effect
 *  1: Pause
 */
#define LEDC_CH5_FADE_PAUSE    (BIT(5))
#define LEDC_CH5_FADE_PAUSE_M  (LEDC_CH5_FADE_PAUSE_V << LEDC_CH5_FADE_PAUSE_S)
#define LEDC_CH5_FADE_PAUSE_V  0x00000001U
#define LEDC_CH5_FADE_PAUSE_S  5
/** LEDC_CH5_FADE_RESUME : WT; bitpos: [6]; default: 0;
 *  Configures whether or nor to resume duty cycle fading of LEDC ch5.
 *  0: Invalid. No effect
 *  1: Resume
 */
#define LEDC_CH5_FADE_RESUME    (BIT(6))
#define LEDC_CH5_FADE_RESUME_M  (LEDC_CH5_FADE_RESUME_V << LEDC_CH5_FADE_RESUME_S)
#define LEDC_CH5_FADE_RESUME_V  0x00000001U
#define LEDC_CH5_FADE_RESUME_S  6

/** LEDC_CH6_FADE_CONF_REG register
 *  Ledc ch6 fade config register.
 */
#define LEDC_CH6_FADE_CONF_REG (DR_REG_LEDC_BASE + 0x118)
/** LEDC_CH6_FADE_PAUSE : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to pause duty cycle fading of LEDC ch6.
 *  0: Invalid. No effect
 *  1: Pause
 */
#define LEDC_CH6_FADE_PAUSE    (BIT(5))
#define LEDC_CH6_FADE_PAUSE_M  (LEDC_CH6_FADE_PAUSE_V << LEDC_CH6_FADE_PAUSE_S)
#define LEDC_CH6_FADE_PAUSE_V  0x00000001U
#define LEDC_CH6_FADE_PAUSE_S  5
/** LEDC_CH6_FADE_RESUME : WT; bitpos: [6]; default: 0;
 *  Configures whether or nor to resume duty cycle fading of LEDC ch6.
 *  0: Invalid. No effect
 *  1: Resume
 */
#define LEDC_CH6_FADE_RESUME    (BIT(6))
#define LEDC_CH6_FADE_RESUME_M  (LEDC_CH6_FADE_RESUME_V << LEDC_CH6_FADE_RESUME_S)
#define LEDC_CH6_FADE_RESUME_V  0x00000001U
#define LEDC_CH6_FADE_RESUME_S  6

/** LEDC_CH7_FADE_CONF_REG register
 *  Ledc ch7 fade config register.
 */
#define LEDC_CH7_FADE_CONF_REG (DR_REG_LEDC_BASE + 0x11c)
/** LEDC_CH7_FADE_PAUSE : WT; bitpos: [5]; default: 0;
 *  Configures whether or not to pause duty cycle fading of LEDC ch7.
 *  0: Invalid. No effect
 *  1: Pause
 */
#define LEDC_CH7_FADE_PAUSE    (BIT(5))
#define LEDC_CH7_FADE_PAUSE_M  (LEDC_CH7_FADE_PAUSE_V << LEDC_CH7_FADE_PAUSE_S)
#define LEDC_CH7_FADE_PAUSE_V  0x00000001U
#define LEDC_CH7_FADE_PAUSE_S  5
/** LEDC_CH7_FADE_RESUME : WT; bitpos: [6]; default: 0;
 *  Configures whether or nor to resume duty cycle fading of LEDC ch7.
 *  0: Invalid. No effect
 *  1: Resume
 */
#define LEDC_CH7_FADE_RESUME    (BIT(6))
#define LEDC_CH7_FADE_RESUME_M  (LEDC_CH7_FADE_RESUME_V << LEDC_CH7_FADE_RESUME_S)
#define LEDC_CH7_FADE_RESUME_V  0x00000001U
#define LEDC_CH7_FADE_RESUME_S  6

/** LEDC_EVT_TASK_EN0_REG register
 *  Ledc event task enable bit register0.
 */
#define LEDC_EVT_TASK_EN0_REG (DR_REG_LEDC_BASE + 0x120)
/** LEDC_EVT_DUTY_CHNG_END_CH0_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_DUTY_CHNG_END_CH0 event.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_EVT_DUTY_CHNG_END_CH0_EN    (BIT(0))
#define LEDC_EVT_DUTY_CHNG_END_CH0_EN_M  (LEDC_EVT_DUTY_CHNG_END_CH0_EN_V << LEDC_EVT_DUTY_CHNG_END_CH0_EN_S)
#define LEDC_EVT_DUTY_CHNG_END_CH0_EN_V  0x00000001U
#define LEDC_EVT_DUTY_CHNG_END_CH0_EN_S  0
/** LEDC_EVT_DUTY_CHNG_END_CH1_EN : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_DUTY_CHNG_END_CH1 event.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_EVT_DUTY_CHNG_END_CH1_EN    (BIT(1))
#define LEDC_EVT_DUTY_CHNG_END_CH1_EN_M  (LEDC_EVT_DUTY_CHNG_END_CH1_EN_V << LEDC_EVT_DUTY_CHNG_END_CH1_EN_S)
#define LEDC_EVT_DUTY_CHNG_END_CH1_EN_V  0x00000001U
#define LEDC_EVT_DUTY_CHNG_END_CH1_EN_S  1
/** LEDC_EVT_DUTY_CHNG_END_CH2_EN : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_DUTY_CHNG_END_CH2 event.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_EVT_DUTY_CHNG_END_CH2_EN    (BIT(2))
#define LEDC_EVT_DUTY_CHNG_END_CH2_EN_M  (LEDC_EVT_DUTY_CHNG_END_CH2_EN_V << LEDC_EVT_DUTY_CHNG_END_CH2_EN_S)
#define LEDC_EVT_DUTY_CHNG_END_CH2_EN_V  0x00000001U
#define LEDC_EVT_DUTY_CHNG_END_CH2_EN_S  2
/** LEDC_EVT_DUTY_CHNG_END_CH3_EN : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_DUTY_CHNG_END_CH3 event.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_EVT_DUTY_CHNG_END_CH3_EN    (BIT(3))
#define LEDC_EVT_DUTY_CHNG_END_CH3_EN_M  (LEDC_EVT_DUTY_CHNG_END_CH3_EN_V << LEDC_EVT_DUTY_CHNG_END_CH3_EN_S)
#define LEDC_EVT_DUTY_CHNG_END_CH3_EN_V  0x00000001U
#define LEDC_EVT_DUTY_CHNG_END_CH3_EN_S  3
/** LEDC_EVT_DUTY_CHNG_END_CH4_EN : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_DUTY_CHNG_END_CH4 event.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_EVT_DUTY_CHNG_END_CH4_EN    (BIT(4))
#define LEDC_EVT_DUTY_CHNG_END_CH4_EN_M  (LEDC_EVT_DUTY_CHNG_END_CH4_EN_V << LEDC_EVT_DUTY_CHNG_END_CH4_EN_S)
#define LEDC_EVT_DUTY_CHNG_END_CH4_EN_V  0x00000001U
#define LEDC_EVT_DUTY_CHNG_END_CH4_EN_S  4
/** LEDC_EVT_DUTY_CHNG_END_CH5_EN : R/W; bitpos: [5]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_DUTY_CHNG_END_CH5 event.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_EVT_DUTY_CHNG_END_CH5_EN    (BIT(5))
#define LEDC_EVT_DUTY_CHNG_END_CH5_EN_M  (LEDC_EVT_DUTY_CHNG_END_CH5_EN_V << LEDC_EVT_DUTY_CHNG_END_CH5_EN_S)
#define LEDC_EVT_DUTY_CHNG_END_CH5_EN_V  0x00000001U
#define LEDC_EVT_DUTY_CHNG_END_CH5_EN_S  5
/** LEDC_EVT_DUTY_CHNG_END_CH6_EN : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_DUTY_CHNG_END_CH6 event.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_EVT_DUTY_CHNG_END_CH6_EN    (BIT(6))
#define LEDC_EVT_DUTY_CHNG_END_CH6_EN_M  (LEDC_EVT_DUTY_CHNG_END_CH6_EN_V << LEDC_EVT_DUTY_CHNG_END_CH6_EN_S)
#define LEDC_EVT_DUTY_CHNG_END_CH6_EN_V  0x00000001U
#define LEDC_EVT_DUTY_CHNG_END_CH6_EN_S  6
/** LEDC_EVT_DUTY_CHNG_END_CH7_EN : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_DUTY_CHNG_END_CH7 event.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_EVT_DUTY_CHNG_END_CH7_EN    (BIT(7))
#define LEDC_EVT_DUTY_CHNG_END_CH7_EN_M  (LEDC_EVT_DUTY_CHNG_END_CH7_EN_V << LEDC_EVT_DUTY_CHNG_END_CH7_EN_S)
#define LEDC_EVT_DUTY_CHNG_END_CH7_EN_V  0x00000001U
#define LEDC_EVT_DUTY_CHNG_END_CH7_EN_S  7
/** LEDC_EVT_OVF_CNT_PLS_CH0_EN : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_OVF_CNT_PLS_CH0 event.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_EVT_OVF_CNT_PLS_CH0_EN    (BIT(8))
#define LEDC_EVT_OVF_CNT_PLS_CH0_EN_M  (LEDC_EVT_OVF_CNT_PLS_CH0_EN_V << LEDC_EVT_OVF_CNT_PLS_CH0_EN_S)
#define LEDC_EVT_OVF_CNT_PLS_CH0_EN_V  0x00000001U
#define LEDC_EVT_OVF_CNT_PLS_CH0_EN_S  8
/** LEDC_EVT_OVF_CNT_PLS_CH1_EN : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_OVF_CNT_PLS_CH1 event.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_EVT_OVF_CNT_PLS_CH1_EN    (BIT(9))
#define LEDC_EVT_OVF_CNT_PLS_CH1_EN_M  (LEDC_EVT_OVF_CNT_PLS_CH1_EN_V << LEDC_EVT_OVF_CNT_PLS_CH1_EN_S)
#define LEDC_EVT_OVF_CNT_PLS_CH1_EN_V  0x00000001U
#define LEDC_EVT_OVF_CNT_PLS_CH1_EN_S  9
/** LEDC_EVT_OVF_CNT_PLS_CH2_EN : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_OVF_CNT_PLS_CH2 event.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_EVT_OVF_CNT_PLS_CH2_EN    (BIT(10))
#define LEDC_EVT_OVF_CNT_PLS_CH2_EN_M  (LEDC_EVT_OVF_CNT_PLS_CH2_EN_V << LEDC_EVT_OVF_CNT_PLS_CH2_EN_S)
#define LEDC_EVT_OVF_CNT_PLS_CH2_EN_V  0x00000001U
#define LEDC_EVT_OVF_CNT_PLS_CH2_EN_S  10
/** LEDC_EVT_OVF_CNT_PLS_CH3_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_OVF_CNT_PLS_CH3 event.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_EVT_OVF_CNT_PLS_CH3_EN    (BIT(11))
#define LEDC_EVT_OVF_CNT_PLS_CH3_EN_M  (LEDC_EVT_OVF_CNT_PLS_CH3_EN_V << LEDC_EVT_OVF_CNT_PLS_CH3_EN_S)
#define LEDC_EVT_OVF_CNT_PLS_CH3_EN_V  0x00000001U
#define LEDC_EVT_OVF_CNT_PLS_CH3_EN_S  11
/** LEDC_EVT_OVF_CNT_PLS_CH4_EN : R/W; bitpos: [12]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_OVF_CNT_PLS_CH4 event.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_EVT_OVF_CNT_PLS_CH4_EN    (BIT(12))
#define LEDC_EVT_OVF_CNT_PLS_CH4_EN_M  (LEDC_EVT_OVF_CNT_PLS_CH4_EN_V << LEDC_EVT_OVF_CNT_PLS_CH4_EN_S)
#define LEDC_EVT_OVF_CNT_PLS_CH4_EN_V  0x00000001U
#define LEDC_EVT_OVF_CNT_PLS_CH4_EN_S  12
/** LEDC_EVT_OVF_CNT_PLS_CH5_EN : R/W; bitpos: [13]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_OVF_CNT_PLS_CH5 event.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_EVT_OVF_CNT_PLS_CH5_EN    (BIT(13))
#define LEDC_EVT_OVF_CNT_PLS_CH5_EN_M  (LEDC_EVT_OVF_CNT_PLS_CH5_EN_V << LEDC_EVT_OVF_CNT_PLS_CH5_EN_S)
#define LEDC_EVT_OVF_CNT_PLS_CH5_EN_V  0x00000001U
#define LEDC_EVT_OVF_CNT_PLS_CH5_EN_S  13
/** LEDC_EVT_OVF_CNT_PLS_CH6_EN : R/W; bitpos: [14]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_OVF_CNT_PLS_CH6 event.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_EVT_OVF_CNT_PLS_CH6_EN    (BIT(14))
#define LEDC_EVT_OVF_CNT_PLS_CH6_EN_M  (LEDC_EVT_OVF_CNT_PLS_CH6_EN_V << LEDC_EVT_OVF_CNT_PLS_CH6_EN_S)
#define LEDC_EVT_OVF_CNT_PLS_CH6_EN_V  0x00000001U
#define LEDC_EVT_OVF_CNT_PLS_CH6_EN_S  14
/** LEDC_EVT_OVF_CNT_PLS_CH7_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_OVF_CNT_PLS_CH7 event.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_EVT_OVF_CNT_PLS_CH7_EN    (BIT(15))
#define LEDC_EVT_OVF_CNT_PLS_CH7_EN_M  (LEDC_EVT_OVF_CNT_PLS_CH7_EN_V << LEDC_EVT_OVF_CNT_PLS_CH7_EN_S)
#define LEDC_EVT_OVF_CNT_PLS_CH7_EN_V  0x00000001U
#define LEDC_EVT_OVF_CNT_PLS_CH7_EN_S  15
/** LEDC_EVT_TIME_OVF_TIMER0_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_TIME_OVF_TIMER0 event.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_EVT_TIME_OVF_TIMER0_EN    (BIT(16))
#define LEDC_EVT_TIME_OVF_TIMER0_EN_M  (LEDC_EVT_TIME_OVF_TIMER0_EN_V << LEDC_EVT_TIME_OVF_TIMER0_EN_S)
#define LEDC_EVT_TIME_OVF_TIMER0_EN_V  0x00000001U
#define LEDC_EVT_TIME_OVF_TIMER0_EN_S  16
/** LEDC_EVT_TIME_OVF_TIMER1_EN : R/W; bitpos: [17]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_TIME_OVF_TIMER1 event.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_EVT_TIME_OVF_TIMER1_EN    (BIT(17))
#define LEDC_EVT_TIME_OVF_TIMER1_EN_M  (LEDC_EVT_TIME_OVF_TIMER1_EN_V << LEDC_EVT_TIME_OVF_TIMER1_EN_S)
#define LEDC_EVT_TIME_OVF_TIMER1_EN_V  0x00000001U
#define LEDC_EVT_TIME_OVF_TIMER1_EN_S  17
/** LEDC_EVT_TIME_OVF_TIMER2_EN : R/W; bitpos: [18]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_TIME_OVF_TIMER2 event.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_EVT_TIME_OVF_TIMER2_EN    (BIT(18))
#define LEDC_EVT_TIME_OVF_TIMER2_EN_M  (LEDC_EVT_TIME_OVF_TIMER2_EN_V << LEDC_EVT_TIME_OVF_TIMER2_EN_S)
#define LEDC_EVT_TIME_OVF_TIMER2_EN_V  0x00000001U
#define LEDC_EVT_TIME_OVF_TIMER2_EN_S  18
/** LEDC_EVT_TIME_OVF_TIMER3_EN : R/W; bitpos: [19]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_TIME_OVF_TIMER3 event.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_EVT_TIME_OVF_TIMER3_EN    (BIT(19))
#define LEDC_EVT_TIME_OVF_TIMER3_EN_M  (LEDC_EVT_TIME_OVF_TIMER3_EN_V << LEDC_EVT_TIME_OVF_TIMER3_EN_S)
#define LEDC_EVT_TIME_OVF_TIMER3_EN_V  0x00000001U
#define LEDC_EVT_TIME_OVF_TIMER3_EN_S  19
/** LEDC_EVT_TIMER0_CMP_EN : R/W; bitpos: [20]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_TIMER0_CMP event.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_EVT_TIMER0_CMP_EN    (BIT(20))
#define LEDC_EVT_TIMER0_CMP_EN_M  (LEDC_EVT_TIMER0_CMP_EN_V << LEDC_EVT_TIMER0_CMP_EN_S)
#define LEDC_EVT_TIMER0_CMP_EN_V  0x00000001U
#define LEDC_EVT_TIMER0_CMP_EN_S  20
/** LEDC_EVT_TIMER1_CMP_EN : R/W; bitpos: [21]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_TIMER1_CMP event.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_EVT_TIMER1_CMP_EN    (BIT(21))
#define LEDC_EVT_TIMER1_CMP_EN_M  (LEDC_EVT_TIMER1_CMP_EN_V << LEDC_EVT_TIMER1_CMP_EN_S)
#define LEDC_EVT_TIMER1_CMP_EN_V  0x00000001U
#define LEDC_EVT_TIMER1_CMP_EN_S  21
/** LEDC_EVT_TIMER2_CMP_EN : R/W; bitpos: [22]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_TIMER2_CMP event.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_EVT_TIMER2_CMP_EN    (BIT(22))
#define LEDC_EVT_TIMER2_CMP_EN_M  (LEDC_EVT_TIMER2_CMP_EN_V << LEDC_EVT_TIMER2_CMP_EN_S)
#define LEDC_EVT_TIMER2_CMP_EN_V  0x00000001U
#define LEDC_EVT_TIMER2_CMP_EN_S  22
/** LEDC_EVT_TIMER3_CMP_EN : R/W; bitpos: [23]; default: 0;
 *  Configures whether or not to enable the LEDC_EVT_TIMER3_CMP event.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_EVT_TIMER3_CMP_EN    (BIT(23))
#define LEDC_EVT_TIMER3_CMP_EN_M  (LEDC_EVT_TIMER3_CMP_EN_V << LEDC_EVT_TIMER3_CMP_EN_S)
#define LEDC_EVT_TIMER3_CMP_EN_V  0x00000001U
#define LEDC_EVT_TIMER3_CMP_EN_S  23
/** LEDC_TASK_DUTY_SCALE_UPDATE_CH0_EN : R/W; bitpos: [24]; default: 0;
 *  Configures whether or not to enable the LEDC_TASK_DUTY_SCALE_UPDATE_CH0 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH0_EN    (BIT(24))
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH0_EN_M  (LEDC_TASK_DUTY_SCALE_UPDATE_CH0_EN_V << LEDC_TASK_DUTY_SCALE_UPDATE_CH0_EN_S)
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH0_EN_V  0x00000001U
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH0_EN_S  24
/** LEDC_TASK_DUTY_SCALE_UPDATE_CH1_EN : R/W; bitpos: [25]; default: 0;
 *  Configures whether or not to enable the LEDC_TASK_DUTY_SCALE_UPDATE_CH1 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH1_EN    (BIT(25))
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH1_EN_M  (LEDC_TASK_DUTY_SCALE_UPDATE_CH1_EN_V << LEDC_TASK_DUTY_SCALE_UPDATE_CH1_EN_S)
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH1_EN_V  0x00000001U
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH1_EN_S  25
/** LEDC_TASK_DUTY_SCALE_UPDATE_CH2_EN : R/W; bitpos: [26]; default: 0;
 *  Configures whether or not to enable the LEDC_TASK_DUTY_SCALE_UPDATE_CH2 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH2_EN    (BIT(26))
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH2_EN_M  (LEDC_TASK_DUTY_SCALE_UPDATE_CH2_EN_V << LEDC_TASK_DUTY_SCALE_UPDATE_CH2_EN_S)
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH2_EN_V  0x00000001U
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH2_EN_S  26
/** LEDC_TASK_DUTY_SCALE_UPDATE_CH3_EN : R/W; bitpos: [27]; default: 0;
 *  Configures whether or not to enable the LEDC_TASK_DUTY_SCALE_UPDATE_CH3 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH3_EN    (BIT(27))
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH3_EN_M  (LEDC_TASK_DUTY_SCALE_UPDATE_CH3_EN_V << LEDC_TASK_DUTY_SCALE_UPDATE_CH3_EN_S)
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH3_EN_V  0x00000001U
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH3_EN_S  27
/** LEDC_TASK_DUTY_SCALE_UPDATE_CH4_EN : R/W; bitpos: [28]; default: 0;
 *  Configures whether or not to enable the LEDC_TASK_DUTY_SCALE_UPDATE_CH4 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH4_EN    (BIT(28))
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH4_EN_M  (LEDC_TASK_DUTY_SCALE_UPDATE_CH4_EN_V << LEDC_TASK_DUTY_SCALE_UPDATE_CH4_EN_S)
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH4_EN_V  0x00000001U
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH4_EN_S  28
/** LEDC_TASK_DUTY_SCALE_UPDATE_CH5_EN : R/W; bitpos: [29]; default: 0;
 *  Configures whether or not to enable the LEDC_TASK_DUTY_SCALE_UPDATE_CH5 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH5_EN    (BIT(29))
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH5_EN_M  (LEDC_TASK_DUTY_SCALE_UPDATE_CH5_EN_V << LEDC_TASK_DUTY_SCALE_UPDATE_CH5_EN_S)
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH5_EN_V  0x00000001U
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH5_EN_S  29
/** LEDC_TASK_DUTY_SCALE_UPDATE_CH6_EN : R/W; bitpos: [30]; default: 0;
 *  Configures whether or not to enable the LEDC_TASK_DUTY_SCALE_UPDATE_CH6 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH6_EN    (BIT(30))
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH6_EN_M  (LEDC_TASK_DUTY_SCALE_UPDATE_CH6_EN_V << LEDC_TASK_DUTY_SCALE_UPDATE_CH6_EN_S)
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH6_EN_V  0x00000001U
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH6_EN_S  30
/** LEDC_TASK_DUTY_SCALE_UPDATE_CH7_EN : R/W; bitpos: [31]; default: 0;
 *  Configures whether or not to enable the LEDC_TASK_DUTY_SCALE_UPDATE_CH7 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH7_EN    (BIT(31))
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH7_EN_M  (LEDC_TASK_DUTY_SCALE_UPDATE_CH7_EN_V << LEDC_TASK_DUTY_SCALE_UPDATE_CH7_EN_S)
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH7_EN_V  0x00000001U
#define LEDC_TASK_DUTY_SCALE_UPDATE_CH7_EN_S  31

/** LEDC_EVT_TASK_EN1_REG register
 *  Ledc event task enable bit register1.
 */
#define LEDC_EVT_TASK_EN1_REG (DR_REG_LEDC_BASE + 0x124)
/** LEDC_TASK_TIMER0_RES_UPDATE_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER0_RES_UPDATE task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_TIMER0_RES_UPDATE_EN    (BIT(0))
#define LEDC_TASK_TIMER0_RES_UPDATE_EN_M  (LEDC_TASK_TIMER0_RES_UPDATE_EN_V << LEDC_TASK_TIMER0_RES_UPDATE_EN_S)
#define LEDC_TASK_TIMER0_RES_UPDATE_EN_V  0x00000001U
#define LEDC_TASK_TIMER0_RES_UPDATE_EN_S  0
/** LEDC_TASK_TIMER1_RES_UPDATE_EN : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER1_RES_UPDATE task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_TIMER1_RES_UPDATE_EN    (BIT(1))
#define LEDC_TASK_TIMER1_RES_UPDATE_EN_M  (LEDC_TASK_TIMER1_RES_UPDATE_EN_V << LEDC_TASK_TIMER1_RES_UPDATE_EN_S)
#define LEDC_TASK_TIMER1_RES_UPDATE_EN_V  0x00000001U
#define LEDC_TASK_TIMER1_RES_UPDATE_EN_S  1
/** LEDC_TASK_TIMER2_RES_UPDATE_EN : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER2_RES_UPDATE task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_TIMER2_RES_UPDATE_EN    (BIT(2))
#define LEDC_TASK_TIMER2_RES_UPDATE_EN_M  (LEDC_TASK_TIMER2_RES_UPDATE_EN_V << LEDC_TASK_TIMER2_RES_UPDATE_EN_S)
#define LEDC_TASK_TIMER2_RES_UPDATE_EN_V  0x00000001U
#define LEDC_TASK_TIMER2_RES_UPDATE_EN_S  2
/** LEDC_TASK_TIMER3_RES_UPDATE_EN : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER3_RES_UPDATE task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_TIMER3_RES_UPDATE_EN    (BIT(3))
#define LEDC_TASK_TIMER3_RES_UPDATE_EN_M  (LEDC_TASK_TIMER3_RES_UPDATE_EN_V << LEDC_TASK_TIMER3_RES_UPDATE_EN_S)
#define LEDC_TASK_TIMER3_RES_UPDATE_EN_V  0x00000001U
#define LEDC_TASK_TIMER3_RES_UPDATE_EN_S  3
/** LEDC_TASK_TIMER0_CAP_EN : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER0_CAP task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_TIMER0_CAP_EN    (BIT(4))
#define LEDC_TASK_TIMER0_CAP_EN_M  (LEDC_TASK_TIMER0_CAP_EN_V << LEDC_TASK_TIMER0_CAP_EN_S)
#define LEDC_TASK_TIMER0_CAP_EN_V  0x00000001U
#define LEDC_TASK_TIMER0_CAP_EN_S  4
/** LEDC_TASK_TIMER1_CAP_EN : R/W; bitpos: [5]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER1_CAP task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_TIMER1_CAP_EN    (BIT(5))
#define LEDC_TASK_TIMER1_CAP_EN_M  (LEDC_TASK_TIMER1_CAP_EN_V << LEDC_TASK_TIMER1_CAP_EN_S)
#define LEDC_TASK_TIMER1_CAP_EN_V  0x00000001U
#define LEDC_TASK_TIMER1_CAP_EN_S  5
/** LEDC_TASK_TIMER2_CAP_EN : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER2_CAP task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_TIMER2_CAP_EN    (BIT(6))
#define LEDC_TASK_TIMER2_CAP_EN_M  (LEDC_TASK_TIMER2_CAP_EN_V << LEDC_TASK_TIMER2_CAP_EN_S)
#define LEDC_TASK_TIMER2_CAP_EN_V  0x00000001U
#define LEDC_TASK_TIMER2_CAP_EN_S  6
/** LEDC_TASK_TIMER3_CAP_EN : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER3_CAP task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_TIMER3_CAP_EN    (BIT(7))
#define LEDC_TASK_TIMER3_CAP_EN_M  (LEDC_TASK_TIMER3_CAP_EN_V << LEDC_TASK_TIMER3_CAP_EN_S)
#define LEDC_TASK_TIMER3_CAP_EN_V  0x00000001U
#define LEDC_TASK_TIMER3_CAP_EN_S  7
/** LEDC_TASK_SIG_OUT_DIS_CH0_EN : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_SIG_OUT_DIS_CH0 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_SIG_OUT_DIS_CH0_EN    (BIT(8))
#define LEDC_TASK_SIG_OUT_DIS_CH0_EN_M  (LEDC_TASK_SIG_OUT_DIS_CH0_EN_V << LEDC_TASK_SIG_OUT_DIS_CH0_EN_S)
#define LEDC_TASK_SIG_OUT_DIS_CH0_EN_V  0x00000001U
#define LEDC_TASK_SIG_OUT_DIS_CH0_EN_S  8
/** LEDC_TASK_SIG_OUT_DIS_CH1_EN : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_SIG_OUT_DIS_CH1 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_SIG_OUT_DIS_CH1_EN    (BIT(9))
#define LEDC_TASK_SIG_OUT_DIS_CH1_EN_M  (LEDC_TASK_SIG_OUT_DIS_CH1_EN_V << LEDC_TASK_SIG_OUT_DIS_CH1_EN_S)
#define LEDC_TASK_SIG_OUT_DIS_CH1_EN_V  0x00000001U
#define LEDC_TASK_SIG_OUT_DIS_CH1_EN_S  9
/** LEDC_TASK_SIG_OUT_DIS_CH2_EN : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_SIG_OUT_DIS_CH2 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_SIG_OUT_DIS_CH2_EN    (BIT(10))
#define LEDC_TASK_SIG_OUT_DIS_CH2_EN_M  (LEDC_TASK_SIG_OUT_DIS_CH2_EN_V << LEDC_TASK_SIG_OUT_DIS_CH2_EN_S)
#define LEDC_TASK_SIG_OUT_DIS_CH2_EN_V  0x00000001U
#define LEDC_TASK_SIG_OUT_DIS_CH2_EN_S  10
/** LEDC_TASK_SIG_OUT_DIS_CH3_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_SIG_OUT_DIS_CH3 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_SIG_OUT_DIS_CH3_EN    (BIT(11))
#define LEDC_TASK_SIG_OUT_DIS_CH3_EN_M  (LEDC_TASK_SIG_OUT_DIS_CH3_EN_V << LEDC_TASK_SIG_OUT_DIS_CH3_EN_S)
#define LEDC_TASK_SIG_OUT_DIS_CH3_EN_V  0x00000001U
#define LEDC_TASK_SIG_OUT_DIS_CH3_EN_S  11
/** LEDC_TASK_SIG_OUT_DIS_CH4_EN : R/W; bitpos: [12]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_SIG_OUT_DIS_CH4 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_SIG_OUT_DIS_CH4_EN    (BIT(12))
#define LEDC_TASK_SIG_OUT_DIS_CH4_EN_M  (LEDC_TASK_SIG_OUT_DIS_CH4_EN_V << LEDC_TASK_SIG_OUT_DIS_CH4_EN_S)
#define LEDC_TASK_SIG_OUT_DIS_CH4_EN_V  0x00000001U
#define LEDC_TASK_SIG_OUT_DIS_CH4_EN_S  12
/** LEDC_TASK_SIG_OUT_DIS_CH5_EN : R/W; bitpos: [13]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_SIG_OUT_DIS_CH5 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_SIG_OUT_DIS_CH5_EN    (BIT(13))
#define LEDC_TASK_SIG_OUT_DIS_CH5_EN_M  (LEDC_TASK_SIG_OUT_DIS_CH5_EN_V << LEDC_TASK_SIG_OUT_DIS_CH5_EN_S)
#define LEDC_TASK_SIG_OUT_DIS_CH5_EN_V  0x00000001U
#define LEDC_TASK_SIG_OUT_DIS_CH5_EN_S  13
/** LEDC_TASK_SIG_OUT_DIS_CH6_EN : R/W; bitpos: [14]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_SIG_OUT_DIS_CH6 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_SIG_OUT_DIS_CH6_EN    (BIT(14))
#define LEDC_TASK_SIG_OUT_DIS_CH6_EN_M  (LEDC_TASK_SIG_OUT_DIS_CH6_EN_V << LEDC_TASK_SIG_OUT_DIS_CH6_EN_S)
#define LEDC_TASK_SIG_OUT_DIS_CH6_EN_V  0x00000001U
#define LEDC_TASK_SIG_OUT_DIS_CH6_EN_S  14
/** LEDC_TASK_SIG_OUT_DIS_CH7_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_SIG_OUT_DIS_CH7 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_SIG_OUT_DIS_CH7_EN    (BIT(15))
#define LEDC_TASK_SIG_OUT_DIS_CH7_EN_M  (LEDC_TASK_SIG_OUT_DIS_CH7_EN_V << LEDC_TASK_SIG_OUT_DIS_CH7_EN_S)
#define LEDC_TASK_SIG_OUT_DIS_CH7_EN_V  0x00000001U
#define LEDC_TASK_SIG_OUT_DIS_CH7_EN_S  15
/** LEDC_TASK_OVF_CNT_RST_CH0_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_OVF_CNT_RST_CH0 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_OVF_CNT_RST_CH0_EN    (BIT(16))
#define LEDC_TASK_OVF_CNT_RST_CH0_EN_M  (LEDC_TASK_OVF_CNT_RST_CH0_EN_V << LEDC_TASK_OVF_CNT_RST_CH0_EN_S)
#define LEDC_TASK_OVF_CNT_RST_CH0_EN_V  0x00000001U
#define LEDC_TASK_OVF_CNT_RST_CH0_EN_S  16
/** LEDC_TASK_OVF_CNT_RST_CH1_EN : R/W; bitpos: [17]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_OVF_CNT_RST_CH1 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_OVF_CNT_RST_CH1_EN    (BIT(17))
#define LEDC_TASK_OVF_CNT_RST_CH1_EN_M  (LEDC_TASK_OVF_CNT_RST_CH1_EN_V << LEDC_TASK_OVF_CNT_RST_CH1_EN_S)
#define LEDC_TASK_OVF_CNT_RST_CH1_EN_V  0x00000001U
#define LEDC_TASK_OVF_CNT_RST_CH1_EN_S  17
/** LEDC_TASK_OVF_CNT_RST_CH2_EN : R/W; bitpos: [18]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_OVF_CNT_RST_CH2 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_OVF_CNT_RST_CH2_EN    (BIT(18))
#define LEDC_TASK_OVF_CNT_RST_CH2_EN_M  (LEDC_TASK_OVF_CNT_RST_CH2_EN_V << LEDC_TASK_OVF_CNT_RST_CH2_EN_S)
#define LEDC_TASK_OVF_CNT_RST_CH2_EN_V  0x00000001U
#define LEDC_TASK_OVF_CNT_RST_CH2_EN_S  18
/** LEDC_TASK_OVF_CNT_RST_CH3_EN : R/W; bitpos: [19]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_OVF_CNT_RST_CH3 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_OVF_CNT_RST_CH3_EN    (BIT(19))
#define LEDC_TASK_OVF_CNT_RST_CH3_EN_M  (LEDC_TASK_OVF_CNT_RST_CH3_EN_V << LEDC_TASK_OVF_CNT_RST_CH3_EN_S)
#define LEDC_TASK_OVF_CNT_RST_CH3_EN_V  0x00000001U
#define LEDC_TASK_OVF_CNT_RST_CH3_EN_S  19
/** LEDC_TASK_OVF_CNT_RST_CH4_EN : R/W; bitpos: [20]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_OVF_CNT_RST_CH4 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_OVF_CNT_RST_CH4_EN    (BIT(20))
#define LEDC_TASK_OVF_CNT_RST_CH4_EN_M  (LEDC_TASK_OVF_CNT_RST_CH4_EN_V << LEDC_TASK_OVF_CNT_RST_CH4_EN_S)
#define LEDC_TASK_OVF_CNT_RST_CH4_EN_V  0x00000001U
#define LEDC_TASK_OVF_CNT_RST_CH4_EN_S  20
/** LEDC_TASK_OVF_CNT_RST_CH5_EN : R/W; bitpos: [21]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_OVF_CNT_RST_CH5 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_OVF_CNT_RST_CH5_EN    (BIT(21))
#define LEDC_TASK_OVF_CNT_RST_CH5_EN_M  (LEDC_TASK_OVF_CNT_RST_CH5_EN_V << LEDC_TASK_OVF_CNT_RST_CH5_EN_S)
#define LEDC_TASK_OVF_CNT_RST_CH5_EN_V  0x00000001U
#define LEDC_TASK_OVF_CNT_RST_CH5_EN_S  21
/** LEDC_TASK_OVF_CNT_RST_CH6_EN : R/W; bitpos: [22]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_OVF_CNT_RST_CH6 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_OVF_CNT_RST_CH6_EN    (BIT(22))
#define LEDC_TASK_OVF_CNT_RST_CH6_EN_M  (LEDC_TASK_OVF_CNT_RST_CH6_EN_V << LEDC_TASK_OVF_CNT_RST_CH6_EN_S)
#define LEDC_TASK_OVF_CNT_RST_CH6_EN_V  0x00000001U
#define LEDC_TASK_OVF_CNT_RST_CH6_EN_S  22
/** LEDC_TASK_OVF_CNT_RST_CH7_EN : R/W; bitpos: [23]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_OVF_CNT_RST_CH7 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_OVF_CNT_RST_CH7_EN    (BIT(23))
#define LEDC_TASK_OVF_CNT_RST_CH7_EN_M  (LEDC_TASK_OVF_CNT_RST_CH7_EN_V << LEDC_TASK_OVF_CNT_RST_CH7_EN_S)
#define LEDC_TASK_OVF_CNT_RST_CH7_EN_V  0x00000001U
#define LEDC_TASK_OVF_CNT_RST_CH7_EN_S  23
/** LEDC_TASK_TIMER0_RST_EN : R/W; bitpos: [24]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER0_RST task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_TIMER0_RST_EN    (BIT(24))
#define LEDC_TASK_TIMER0_RST_EN_M  (LEDC_TASK_TIMER0_RST_EN_V << LEDC_TASK_TIMER0_RST_EN_S)
#define LEDC_TASK_TIMER0_RST_EN_V  0x00000001U
#define LEDC_TASK_TIMER0_RST_EN_S  24
/** LEDC_TASK_TIMER1_RST_EN : R/W; bitpos: [25]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER1_RST task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_TIMER1_RST_EN    (BIT(25))
#define LEDC_TASK_TIMER1_RST_EN_M  (LEDC_TASK_TIMER1_RST_EN_V << LEDC_TASK_TIMER1_RST_EN_S)
#define LEDC_TASK_TIMER1_RST_EN_V  0x00000001U
#define LEDC_TASK_TIMER1_RST_EN_S  25
/** LEDC_TASK_TIMER2_RST_EN : R/W; bitpos: [26]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER2_RST task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_TIMER2_RST_EN    (BIT(26))
#define LEDC_TASK_TIMER2_RST_EN_M  (LEDC_TASK_TIMER2_RST_EN_V << LEDC_TASK_TIMER2_RST_EN_S)
#define LEDC_TASK_TIMER2_RST_EN_V  0x00000001U
#define LEDC_TASK_TIMER2_RST_EN_S  26
/** LEDC_TASK_TIMER3_RST_EN : R/W; bitpos: [27]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER3_RST task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_TIMER3_RST_EN    (BIT(27))
#define LEDC_TASK_TIMER3_RST_EN_M  (LEDC_TASK_TIMER3_RST_EN_V << LEDC_TASK_TIMER3_RST_EN_S)
#define LEDC_TASK_TIMER3_RST_EN_V  0x00000001U
#define LEDC_TASK_TIMER3_RST_EN_S  27
/** LEDC_TASK_TIMER0_PAUSE_RESUME_EN : R/W; bitpos: [28]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER0_PAUSE and LEDC_TASK_TIMER0
 *  _RESUME task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_TIMER0_PAUSE_RESUME_EN    (BIT(28))
#define LEDC_TASK_TIMER0_PAUSE_RESUME_EN_M  (LEDC_TASK_TIMER0_PAUSE_RESUME_EN_V << LEDC_TASK_TIMER0_PAUSE_RESUME_EN_S)
#define LEDC_TASK_TIMER0_PAUSE_RESUME_EN_V  0x00000001U
#define LEDC_TASK_TIMER0_PAUSE_RESUME_EN_S  28
/** LEDC_TASK_TIMER1_PAUSE_RESUME_EN : R/W; bitpos: [29]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER1_PAUSE and LEDC_TASK_TIMER1
 *  _RESUME task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_TIMER1_PAUSE_RESUME_EN    (BIT(29))
#define LEDC_TASK_TIMER1_PAUSE_RESUME_EN_M  (LEDC_TASK_TIMER1_PAUSE_RESUME_EN_V << LEDC_TASK_TIMER1_PAUSE_RESUME_EN_S)
#define LEDC_TASK_TIMER1_PAUSE_RESUME_EN_V  0x00000001U
#define LEDC_TASK_TIMER1_PAUSE_RESUME_EN_S  29
/** LEDC_TASK_TIMER2_PAUSE_RESUME_EN : R/W; bitpos: [30]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER2_PAUSE and LEDC_TASK_TIMER2
 *  _RESUME task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_TIMER2_PAUSE_RESUME_EN    (BIT(30))
#define LEDC_TASK_TIMER2_PAUSE_RESUME_EN_M  (LEDC_TASK_TIMER2_PAUSE_RESUME_EN_V << LEDC_TASK_TIMER2_PAUSE_RESUME_EN_S)
#define LEDC_TASK_TIMER2_PAUSE_RESUME_EN_V  0x00000001U
#define LEDC_TASK_TIMER2_PAUSE_RESUME_EN_S  30
/** LEDC_TASK_TIMER3_PAUSE_RESUME_EN : R/W; bitpos: [31]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_TIMER3_PAUSE and LEDC_TASK_TIMER3
 *  _RESUME task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_TIMER3_PAUSE_RESUME_EN    (BIT(31))
#define LEDC_TASK_TIMER3_PAUSE_RESUME_EN_M  (LEDC_TASK_TIMER3_PAUSE_RESUME_EN_V << LEDC_TASK_TIMER3_PAUSE_RESUME_EN_S)
#define LEDC_TASK_TIMER3_PAUSE_RESUME_EN_V  0x00000001U
#define LEDC_TASK_TIMER3_PAUSE_RESUME_EN_S  31

/** LEDC_EVT_TASK_EN2_REG register
 *  Ledc event task enable bit register2.
 */
#define LEDC_EVT_TASK_EN2_REG (DR_REG_LEDC_BASE + 0x128)
/** LEDC_TASK_FADE_RESTART_CH0_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_FADE_RESTART_CH0 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_FADE_RESTART_CH0_EN    (BIT(0))
#define LEDC_TASK_FADE_RESTART_CH0_EN_M  (LEDC_TASK_FADE_RESTART_CH0_EN_V << LEDC_TASK_FADE_RESTART_CH0_EN_S)
#define LEDC_TASK_FADE_RESTART_CH0_EN_V  0x00000001U
#define LEDC_TASK_FADE_RESTART_CH0_EN_S  0
/** LEDC_TASK_FADE_RESTART_CH1_EN : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_FADE_RESTART_CH1 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_FADE_RESTART_CH1_EN    (BIT(1))
#define LEDC_TASK_FADE_RESTART_CH1_EN_M  (LEDC_TASK_FADE_RESTART_CH1_EN_V << LEDC_TASK_FADE_RESTART_CH1_EN_S)
#define LEDC_TASK_FADE_RESTART_CH1_EN_V  0x00000001U
#define LEDC_TASK_FADE_RESTART_CH1_EN_S  1
/** LEDC_TASK_FADE_RESTART_CH2_EN : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_FADE_RESTART_CH2 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_FADE_RESTART_CH2_EN    (BIT(2))
#define LEDC_TASK_FADE_RESTART_CH2_EN_M  (LEDC_TASK_FADE_RESTART_CH2_EN_V << LEDC_TASK_FADE_RESTART_CH2_EN_S)
#define LEDC_TASK_FADE_RESTART_CH2_EN_V  0x00000001U
#define LEDC_TASK_FADE_RESTART_CH2_EN_S  2
/** LEDC_TASK_FADE_RESTART_CH3_EN : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_FADE_RESTART_CH3 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_FADE_RESTART_CH3_EN    (BIT(3))
#define LEDC_TASK_FADE_RESTART_CH3_EN_M  (LEDC_TASK_FADE_RESTART_CH3_EN_V << LEDC_TASK_FADE_RESTART_CH3_EN_S)
#define LEDC_TASK_FADE_RESTART_CH3_EN_V  0x00000001U
#define LEDC_TASK_FADE_RESTART_CH3_EN_S  3
/** LEDC_TASK_FADE_RESTART_CH4_EN : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_FADE_RESTART_CH4 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_FADE_RESTART_CH4_EN    (BIT(4))
#define LEDC_TASK_FADE_RESTART_CH4_EN_M  (LEDC_TASK_FADE_RESTART_CH4_EN_V << LEDC_TASK_FADE_RESTART_CH4_EN_S)
#define LEDC_TASK_FADE_RESTART_CH4_EN_V  0x00000001U
#define LEDC_TASK_FADE_RESTART_CH4_EN_S  4
/** LEDC_TASK_FADE_RESTART_CH5_EN : R/W; bitpos: [5]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_FADE_RESTART_CH5 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_FADE_RESTART_CH5_EN    (BIT(5))
#define LEDC_TASK_FADE_RESTART_CH5_EN_M  (LEDC_TASK_FADE_RESTART_CH5_EN_V << LEDC_TASK_FADE_RESTART_CH5_EN_S)
#define LEDC_TASK_FADE_RESTART_CH5_EN_V  0x00000001U
#define LEDC_TASK_FADE_RESTART_CH5_EN_S  5
/** LEDC_TASK_FADE_RESTART_CH6_EN : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_FADE_RESTART_CH6 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_FADE_RESTART_CH6_EN    (BIT(6))
#define LEDC_TASK_FADE_RESTART_CH6_EN_M  (LEDC_TASK_FADE_RESTART_CH6_EN_V << LEDC_TASK_FADE_RESTART_CH6_EN_S)
#define LEDC_TASK_FADE_RESTART_CH6_EN_V  0x00000001U
#define LEDC_TASK_FADE_RESTART_CH6_EN_S  6
/** LEDC_TASK_FADE_RESTART_CH7_EN : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_FADE_RESTART_CH7 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_FADE_RESTART_CH7_EN    (BIT(7))
#define LEDC_TASK_FADE_RESTART_CH7_EN_M  (LEDC_TASK_FADE_RESTART_CH7_EN_V << LEDC_TASK_FADE_RESTART_CH7_EN_S)
#define LEDC_TASK_FADE_RESTART_CH7_EN_V  0x00000001U
#define LEDC_TASK_FADE_RESTART_CH7_EN_S  7
/** LEDC_TASK_FADE_PAUSE_CH0_EN : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_FADE_PAUSE_CH0 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_FADE_PAUSE_CH0_EN    (BIT(8))
#define LEDC_TASK_FADE_PAUSE_CH0_EN_M  (LEDC_TASK_FADE_PAUSE_CH0_EN_V << LEDC_TASK_FADE_PAUSE_CH0_EN_S)
#define LEDC_TASK_FADE_PAUSE_CH0_EN_V  0x00000001U
#define LEDC_TASK_FADE_PAUSE_CH0_EN_S  8
/** LEDC_TASK_FADE_PAUSE_CH1_EN : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_FADE_PAUSE_CH1 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_FADE_PAUSE_CH1_EN    (BIT(9))
#define LEDC_TASK_FADE_PAUSE_CH1_EN_M  (LEDC_TASK_FADE_PAUSE_CH1_EN_V << LEDC_TASK_FADE_PAUSE_CH1_EN_S)
#define LEDC_TASK_FADE_PAUSE_CH1_EN_V  0x00000001U
#define LEDC_TASK_FADE_PAUSE_CH1_EN_S  9
/** LEDC_TASK_FADE_PAUSE_CH2_EN : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_FADE_PAUSE_CH2 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_FADE_PAUSE_CH2_EN    (BIT(10))
#define LEDC_TASK_FADE_PAUSE_CH2_EN_M  (LEDC_TASK_FADE_PAUSE_CH2_EN_V << LEDC_TASK_FADE_PAUSE_CH2_EN_S)
#define LEDC_TASK_FADE_PAUSE_CH2_EN_V  0x00000001U
#define LEDC_TASK_FADE_PAUSE_CH2_EN_S  10
/** LEDC_TASK_FADE_PAUSE_CH3_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_FADE_PAUSE_CH3 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_FADE_PAUSE_CH3_EN    (BIT(11))
#define LEDC_TASK_FADE_PAUSE_CH3_EN_M  (LEDC_TASK_FADE_PAUSE_CH3_EN_V << LEDC_TASK_FADE_PAUSE_CH3_EN_S)
#define LEDC_TASK_FADE_PAUSE_CH3_EN_V  0x00000001U
#define LEDC_TASK_FADE_PAUSE_CH3_EN_S  11
/** LEDC_TASK_FADE_PAUSE_CH4_EN : R/W; bitpos: [12]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_FADE_PAUSE_CH4 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_FADE_PAUSE_CH4_EN    (BIT(12))
#define LEDC_TASK_FADE_PAUSE_CH4_EN_M  (LEDC_TASK_FADE_PAUSE_CH4_EN_V << LEDC_TASK_FADE_PAUSE_CH4_EN_S)
#define LEDC_TASK_FADE_PAUSE_CH4_EN_V  0x00000001U
#define LEDC_TASK_FADE_PAUSE_CH4_EN_S  12
/** LEDC_TASK_FADE_PAUSE_CH5_EN : R/W; bitpos: [13]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_FADE_PAUSE_CH5 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_FADE_PAUSE_CH5_EN    (BIT(13))
#define LEDC_TASK_FADE_PAUSE_CH5_EN_M  (LEDC_TASK_FADE_PAUSE_CH5_EN_V << LEDC_TASK_FADE_PAUSE_CH5_EN_S)
#define LEDC_TASK_FADE_PAUSE_CH5_EN_V  0x00000001U
#define LEDC_TASK_FADE_PAUSE_CH5_EN_S  13
/** LEDC_TASK_FADE_PAUSE_CH6_EN : R/W; bitpos: [14]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_FADE_PAUSE_CH6 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_FADE_PAUSE_CH6_EN    (BIT(14))
#define LEDC_TASK_FADE_PAUSE_CH6_EN_M  (LEDC_TASK_FADE_PAUSE_CH6_EN_V << LEDC_TASK_FADE_PAUSE_CH6_EN_S)
#define LEDC_TASK_FADE_PAUSE_CH6_EN_V  0x00000001U
#define LEDC_TASK_FADE_PAUSE_CH6_EN_S  14
/** LEDC_TASK_FADE_PAUSE_CH7_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_FADE_PAUSE_CH7 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_FADE_PAUSE_CH7_EN    (BIT(15))
#define LEDC_TASK_FADE_PAUSE_CH7_EN_M  (LEDC_TASK_FADE_PAUSE_CH7_EN_V << LEDC_TASK_FADE_PAUSE_CH7_EN_S)
#define LEDC_TASK_FADE_PAUSE_CH7_EN_V  0x00000001U
#define LEDC_TASK_FADE_PAUSE_CH7_EN_S  15
/** LEDC_TASK_FADE_RESUME_CH0_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_FADE_RESUME_CH0 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_FADE_RESUME_CH0_EN    (BIT(16))
#define LEDC_TASK_FADE_RESUME_CH0_EN_M  (LEDC_TASK_FADE_RESUME_CH0_EN_V << LEDC_TASK_FADE_RESUME_CH0_EN_S)
#define LEDC_TASK_FADE_RESUME_CH0_EN_V  0x00000001U
#define LEDC_TASK_FADE_RESUME_CH0_EN_S  16
/** LEDC_TASK_FADE_RESUME_CH1_EN : R/W; bitpos: [17]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_FADE_RESUME_CH1 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_FADE_RESUME_CH1_EN    (BIT(17))
#define LEDC_TASK_FADE_RESUME_CH1_EN_M  (LEDC_TASK_FADE_RESUME_CH1_EN_V << LEDC_TASK_FADE_RESUME_CH1_EN_S)
#define LEDC_TASK_FADE_RESUME_CH1_EN_V  0x00000001U
#define LEDC_TASK_FADE_RESUME_CH1_EN_S  17
/** LEDC_TASK_FADE_RESUME_CH2_EN : R/W; bitpos: [18]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_FADE_RESUME_CH2 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_FADE_RESUME_CH2_EN    (BIT(18))
#define LEDC_TASK_FADE_RESUME_CH2_EN_M  (LEDC_TASK_FADE_RESUME_CH2_EN_V << LEDC_TASK_FADE_RESUME_CH2_EN_S)
#define LEDC_TASK_FADE_RESUME_CH2_EN_V  0x00000001U
#define LEDC_TASK_FADE_RESUME_CH2_EN_S  18
/** LEDC_TASK_FADE_RESUME_CH3_EN : R/W; bitpos: [19]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_FADE_RESUME_CH3 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_FADE_RESUME_CH3_EN    (BIT(19))
#define LEDC_TASK_FADE_RESUME_CH3_EN_M  (LEDC_TASK_FADE_RESUME_CH3_EN_V << LEDC_TASK_FADE_RESUME_CH3_EN_S)
#define LEDC_TASK_FADE_RESUME_CH3_EN_V  0x00000001U
#define LEDC_TASK_FADE_RESUME_CH3_EN_S  19
/** LEDC_TASK_FADE_RESUME_CH4_EN : R/W; bitpos: [20]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_FADE_RESUME_CH4 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_FADE_RESUME_CH4_EN    (BIT(20))
#define LEDC_TASK_FADE_RESUME_CH4_EN_M  (LEDC_TASK_FADE_RESUME_CH4_EN_V << LEDC_TASK_FADE_RESUME_CH4_EN_S)
#define LEDC_TASK_FADE_RESUME_CH4_EN_V  0x00000001U
#define LEDC_TASK_FADE_RESUME_CH4_EN_S  20
/** LEDC_TASK_FADE_RESUME_CH5_EN : R/W; bitpos: [21]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_FADE_RESUME_CH5 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_FADE_RESUME_CH5_EN    (BIT(21))
#define LEDC_TASK_FADE_RESUME_CH5_EN_M  (LEDC_TASK_FADE_RESUME_CH5_EN_V << LEDC_TASK_FADE_RESUME_CH5_EN_S)
#define LEDC_TASK_FADE_RESUME_CH5_EN_V  0x00000001U
#define LEDC_TASK_FADE_RESUME_CH5_EN_S  21
/** LEDC_TASK_FADE_RESUME_CH6_EN : R/W; bitpos: [22]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_FADE_RESUME_CH6 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_FADE_RESUME_CH6_EN    (BIT(22))
#define LEDC_TASK_FADE_RESUME_CH6_EN_M  (LEDC_TASK_FADE_RESUME_CH6_EN_V << LEDC_TASK_FADE_RESUME_CH6_EN_S)
#define LEDC_TASK_FADE_RESUME_CH6_EN_V  0x00000001U
#define LEDC_TASK_FADE_RESUME_CH6_EN_S  22
/** LEDC_TASK_FADE_RESUME_CH7_EN : R/W; bitpos: [23]; default: 0;
 *  Configures whether or not to enable LEDC_TASK_FADE_RESUME_CH7 task.
 *  0: Disable
 *  1: Enable
 */
#define LEDC_TASK_FADE_RESUME_CH7_EN    (BIT(23))
#define LEDC_TASK_FADE_RESUME_CH7_EN_M  (LEDC_TASK_FADE_RESUME_CH7_EN_V << LEDC_TASK_FADE_RESUME_CH7_EN_S)
#define LEDC_TASK_FADE_RESUME_CH7_EN_V  0x00000001U
#define LEDC_TASK_FADE_RESUME_CH7_EN_S  23

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
/** LEDC_CLK_EN : R/W; bitpos: [31]; default: 0;
 *  Configures whether or not to open register clock gate.
 *  0: Open the clock gate only when application writes registers
 *  1: Force open the clock gate for register
 */
#define LEDC_CLK_EN    (BIT(31))
#define LEDC_CLK_EN_M  (LEDC_CLK_EN_V << LEDC_CLK_EN_S)
#define LEDC_CLK_EN_V  0x00000001U
#define LEDC_CLK_EN_S  31

/** LEDC_CH_POWER_UP_CONF_REG register
 *  LEDC channel power up configuration register
 */
#define LEDC_CH_POWER_UP_CONF_REG (DR_REG_LEDC_BASE + 0x174)
/** LEDC_CH0_POWER_UP : R/W; bitpos: [0]; default: 1;
 *  Configures whether or not to power up ch0.
 *  0: power down
 *  1: power up
 */
#define LEDC_CH0_POWER_UP    (BIT(0))
#define LEDC_CH0_POWER_UP_M  (LEDC_CH0_POWER_UP_V << LEDC_CH0_POWER_UP_S)
#define LEDC_CH0_POWER_UP_V  0x00000001U
#define LEDC_CH0_POWER_UP_S  0
/** LEDC_CH1_POWER_UP : R/W; bitpos: [1]; default: 1;
 *  Configures whether or not to power up ch1.
 *  0: power down
 *  1: power up
 */
#define LEDC_CH1_POWER_UP    (BIT(1))
#define LEDC_CH1_POWER_UP_M  (LEDC_CH1_POWER_UP_V << LEDC_CH1_POWER_UP_S)
#define LEDC_CH1_POWER_UP_V  0x00000001U
#define LEDC_CH1_POWER_UP_S  1
/** LEDC_CH2_POWER_UP : R/W; bitpos: [2]; default: 1;
 *  Configures whether or not to power up ch2.
 *  0: power down
 *  1: power up
 */
#define LEDC_CH2_POWER_UP    (BIT(2))
#define LEDC_CH2_POWER_UP_M  (LEDC_CH2_POWER_UP_V << LEDC_CH2_POWER_UP_S)
#define LEDC_CH2_POWER_UP_V  0x00000001U
#define LEDC_CH2_POWER_UP_S  2
/** LEDC_CH3_POWER_UP : R/W; bitpos: [3]; default: 1;
 *  Configures whether or not to power up ch3.
 *  0: power down
 *  1: power up
 */
#define LEDC_CH3_POWER_UP    (BIT(3))
#define LEDC_CH3_POWER_UP_M  (LEDC_CH3_POWER_UP_V << LEDC_CH3_POWER_UP_S)
#define LEDC_CH3_POWER_UP_V  0x00000001U
#define LEDC_CH3_POWER_UP_S  3
/** LEDC_CH4_POWER_UP : R/W; bitpos: [4]; default: 1;
 *  Configures whether or not to power up ch4.
 *  0: power down
 *  1: power up
 */
#define LEDC_CH4_POWER_UP    (BIT(4))
#define LEDC_CH4_POWER_UP_M  (LEDC_CH4_POWER_UP_V << LEDC_CH4_POWER_UP_S)
#define LEDC_CH4_POWER_UP_V  0x00000001U
#define LEDC_CH4_POWER_UP_S  4
/** LEDC_CH5_POWER_UP : R/W; bitpos: [5]; default: 1;
 *  Configures whether or not to power up ch5.
 *  0: power down
 *  1: power up
 */
#define LEDC_CH5_POWER_UP    (BIT(5))
#define LEDC_CH5_POWER_UP_M  (LEDC_CH5_POWER_UP_V << LEDC_CH5_POWER_UP_S)
#define LEDC_CH5_POWER_UP_V  0x00000001U
#define LEDC_CH5_POWER_UP_S  5
/** LEDC_CH6_POWER_UP : R/W; bitpos: [6]; default: 1;
 *  Configures whether or not to power up ch6.
 *  0: power down
 *  1: power up
 */
#define LEDC_CH6_POWER_UP    (BIT(6))
#define LEDC_CH6_POWER_UP_M  (LEDC_CH6_POWER_UP_V << LEDC_CH6_POWER_UP_S)
#define LEDC_CH6_POWER_UP_V  0x00000001U
#define LEDC_CH6_POWER_UP_S  6
/** LEDC_CH7_POWER_UP : R/W; bitpos: [7]; default: 1;
 *  Configures whether or not to power up ch7.
 *  0: power down
 *  1: power up
 */
#define LEDC_CH7_POWER_UP    (BIT(7))
#define LEDC_CH7_POWER_UP_M  (LEDC_CH7_POWER_UP_V << LEDC_CH7_POWER_UP_S)
#define LEDC_CH7_POWER_UP_V  0x00000001U
#define LEDC_CH7_POWER_UP_S  7
/** LEDC_CH0_CLK_GATE_FORCE_ON : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to open ch0 clock gate.
 *  0: Open the clock gate only when ch0 is power up
 *  1: Force open the clock gate for ch0
 */
#define LEDC_CH0_CLK_GATE_FORCE_ON    (BIT(8))
#define LEDC_CH0_CLK_GATE_FORCE_ON_M  (LEDC_CH0_CLK_GATE_FORCE_ON_V << LEDC_CH0_CLK_GATE_FORCE_ON_S)
#define LEDC_CH0_CLK_GATE_FORCE_ON_V  0x00000001U
#define LEDC_CH0_CLK_GATE_FORCE_ON_S  8
/** LEDC_CH1_CLK_GATE_FORCE_ON : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to open ch1 clock gate.
 *  0: Open the clock gate only when ch1 is power up
 *  1: Force open the clock gate for ch1
 */
#define LEDC_CH1_CLK_GATE_FORCE_ON    (BIT(9))
#define LEDC_CH1_CLK_GATE_FORCE_ON_M  (LEDC_CH1_CLK_GATE_FORCE_ON_V << LEDC_CH1_CLK_GATE_FORCE_ON_S)
#define LEDC_CH1_CLK_GATE_FORCE_ON_V  0x00000001U
#define LEDC_CH1_CLK_GATE_FORCE_ON_S  9
/** LEDC_CH2_CLK_GATE_FORCE_ON : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to open ch2 clock gate.
 *  0: Open the clock gate only when ch2 is power up
 *  1: Force open the clock gate for ch2
 */
#define LEDC_CH2_CLK_GATE_FORCE_ON    (BIT(10))
#define LEDC_CH2_CLK_GATE_FORCE_ON_M  (LEDC_CH2_CLK_GATE_FORCE_ON_V << LEDC_CH2_CLK_GATE_FORCE_ON_S)
#define LEDC_CH2_CLK_GATE_FORCE_ON_V  0x00000001U
#define LEDC_CH2_CLK_GATE_FORCE_ON_S  10
/** LEDC_CH3_CLK_GATE_FORCE_ON : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to open ch3 clock gate.
 *  0: Open the clock gate only when ch3 is power up
 *  1: Force open the clock gate for ch3
 */
#define LEDC_CH3_CLK_GATE_FORCE_ON    (BIT(11))
#define LEDC_CH3_CLK_GATE_FORCE_ON_M  (LEDC_CH3_CLK_GATE_FORCE_ON_V << LEDC_CH3_CLK_GATE_FORCE_ON_S)
#define LEDC_CH3_CLK_GATE_FORCE_ON_V  0x00000001U
#define LEDC_CH3_CLK_GATE_FORCE_ON_S  11
/** LEDC_CH4_CLK_GATE_FORCE_ON : R/W; bitpos: [12]; default: 0;
 *  Configures whether or not to open ch4 clock gate.
 *  0: Open the clock gate only when ch4 is power up
 *  1: Force open the clock gate for ch4
 */
#define LEDC_CH4_CLK_GATE_FORCE_ON    (BIT(12))
#define LEDC_CH4_CLK_GATE_FORCE_ON_M  (LEDC_CH4_CLK_GATE_FORCE_ON_V << LEDC_CH4_CLK_GATE_FORCE_ON_S)
#define LEDC_CH4_CLK_GATE_FORCE_ON_V  0x00000001U
#define LEDC_CH4_CLK_GATE_FORCE_ON_S  12
/** LEDC_CH5_CLK_GATE_FORCE_ON : R/W; bitpos: [13]; default: 0;
 *  Configures whether or not to open ch5 clock gate.
 *  0: Open the clock gate only when ch5 is power up
 *  1: Force open the clock gate for ch5
 */
#define LEDC_CH5_CLK_GATE_FORCE_ON    (BIT(13))
#define LEDC_CH5_CLK_GATE_FORCE_ON_M  (LEDC_CH5_CLK_GATE_FORCE_ON_V << LEDC_CH5_CLK_GATE_FORCE_ON_S)
#define LEDC_CH5_CLK_GATE_FORCE_ON_V  0x00000001U
#define LEDC_CH5_CLK_GATE_FORCE_ON_S  13
/** LEDC_CH6_CLK_GATE_FORCE_ON : R/W; bitpos: [14]; default: 0;
 *  Configures whether or not to open ch6 clock gate.
 *  0: Open the clock gate only when ch6 is power up
 *  1: Force open the clock gate for ch6
 */
#define LEDC_CH6_CLK_GATE_FORCE_ON    (BIT(14))
#define LEDC_CH6_CLK_GATE_FORCE_ON_M  (LEDC_CH6_CLK_GATE_FORCE_ON_V << LEDC_CH6_CLK_GATE_FORCE_ON_S)
#define LEDC_CH6_CLK_GATE_FORCE_ON_V  0x00000001U
#define LEDC_CH6_CLK_GATE_FORCE_ON_S  14
/** LEDC_CH7_CLK_GATE_FORCE_ON : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to open ch7 clock gate.
 *  0: Open the clock gate only when ch7 is power up
 *  1: Force open the clock gate for ch7
 */
#define LEDC_CH7_CLK_GATE_FORCE_ON    (BIT(15))
#define LEDC_CH7_CLK_GATE_FORCE_ON_M  (LEDC_CH7_CLK_GATE_FORCE_ON_V << LEDC_CH7_CLK_GATE_FORCE_ON_S)
#define LEDC_CH7_CLK_GATE_FORCE_ON_V  0x00000001U
#define LEDC_CH7_CLK_GATE_FORCE_ON_S  15

/** LEDC_TIMER_POWER_UP_CONF_REG register
 *  LEDC timer power up configuration register
 */
#define LEDC_TIMER_POWER_UP_CONF_REG (DR_REG_LEDC_BASE + 0x178)
/** LEDC_TIMER0_POWER_UP : R/W; bitpos: [0]; default: 1;
 *  Configures whether or not to power up timer0.
 *  0: power down
 *  1: power up
 */
#define LEDC_TIMER0_POWER_UP    (BIT(0))
#define LEDC_TIMER0_POWER_UP_M  (LEDC_TIMER0_POWER_UP_V << LEDC_TIMER0_POWER_UP_S)
#define LEDC_TIMER0_POWER_UP_V  0x00000001U
#define LEDC_TIMER0_POWER_UP_S  0
/** LEDC_TIMER1_POWER_UP : R/W; bitpos: [1]; default: 1;
 *  Configures whether or not to power up timer1.
 *  0: power down
 *  1: power up
 */
#define LEDC_TIMER1_POWER_UP    (BIT(1))
#define LEDC_TIMER1_POWER_UP_M  (LEDC_TIMER1_POWER_UP_V << LEDC_TIMER1_POWER_UP_S)
#define LEDC_TIMER1_POWER_UP_V  0x00000001U
#define LEDC_TIMER1_POWER_UP_S  1
/** LEDC_TIMER2_POWER_UP : R/W; bitpos: [2]; default: 1;
 *  Configures whether or not to power up timer2.
 *  0: power down
 *  1: power up
 */
#define LEDC_TIMER2_POWER_UP    (BIT(2))
#define LEDC_TIMER2_POWER_UP_M  (LEDC_TIMER2_POWER_UP_V << LEDC_TIMER2_POWER_UP_S)
#define LEDC_TIMER2_POWER_UP_V  0x00000001U
#define LEDC_TIMER2_POWER_UP_S  2
/** LEDC_TIMER3_POWER_UP : R/W; bitpos: [3]; default: 1;
 *  Configures whether or not to power up timer3.
 *  0: power down
 *  1: power up
 */
#define LEDC_TIMER3_POWER_UP    (BIT(3))
#define LEDC_TIMER3_POWER_UP_M  (LEDC_TIMER3_POWER_UP_V << LEDC_TIMER3_POWER_UP_S)
#define LEDC_TIMER3_POWER_UP_V  0x00000001U
#define LEDC_TIMER3_POWER_UP_S  3
/** LEDC_TIMER0_CLK_GATE_FORCE_ON : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to open timer0 clock gate.
 *  0: Open the clock gate only when timer0 is power up
 *  1: Force open the clock gate for timer0
 */
#define LEDC_TIMER0_CLK_GATE_FORCE_ON    (BIT(4))
#define LEDC_TIMER0_CLK_GATE_FORCE_ON_M  (LEDC_TIMER0_CLK_GATE_FORCE_ON_V << LEDC_TIMER0_CLK_GATE_FORCE_ON_S)
#define LEDC_TIMER0_CLK_GATE_FORCE_ON_V  0x00000001U
#define LEDC_TIMER0_CLK_GATE_FORCE_ON_S  4
/** LEDC_TIMER1_CLK_GATE_FORCE_ON : R/W; bitpos: [5]; default: 0;
 *  Configures whether or not to open timer1 clock gate.
 *  0: Open the clock gate only when timer1 is power up
 *  1: Force open the clock gate for timer1
 */
#define LEDC_TIMER1_CLK_GATE_FORCE_ON    (BIT(5))
#define LEDC_TIMER1_CLK_GATE_FORCE_ON_M  (LEDC_TIMER1_CLK_GATE_FORCE_ON_V << LEDC_TIMER1_CLK_GATE_FORCE_ON_S)
#define LEDC_TIMER1_CLK_GATE_FORCE_ON_V  0x00000001U
#define LEDC_TIMER1_CLK_GATE_FORCE_ON_S  5
/** LEDC_TIMER2_CLK_GATE_FORCE_ON : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to open timer2 clock gate.
 *  0: Open the clock gate only when timer2 is power up
 *  1: Force open the clock gate for timer2
 */
#define LEDC_TIMER2_CLK_GATE_FORCE_ON    (BIT(6))
#define LEDC_TIMER2_CLK_GATE_FORCE_ON_M  (LEDC_TIMER2_CLK_GATE_FORCE_ON_V << LEDC_TIMER2_CLK_GATE_FORCE_ON_S)
#define LEDC_TIMER2_CLK_GATE_FORCE_ON_V  0x00000001U
#define LEDC_TIMER2_CLK_GATE_FORCE_ON_S  6
/** LEDC_TIMER3_CLK_GATE_FORCE_ON : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to open timer3 clock gate.
 *  0: Open the clock gate only when timer3 is power up
 *  1: Force open the clock gate for timer3
 */
#define LEDC_TIMER3_CLK_GATE_FORCE_ON    (BIT(7))
#define LEDC_TIMER3_CLK_GATE_FORCE_ON_M  (LEDC_TIMER3_CLK_GATE_FORCE_ON_V << LEDC_TIMER3_CLK_GATE_FORCE_ON_S)
#define LEDC_TIMER3_CLK_GATE_FORCE_ON_V  0x00000001U
#define LEDC_TIMER3_CLK_GATE_FORCE_ON_S  7

/** LEDC_DATE_REG register
 *  Version control register
 */
#define LEDC_DATE_REG (DR_REG_LEDC_BASE + 0x17c)
/** LEDC_LEDC_DATE : R/W; bitpos: [27:0]; default: 37765488;
 *  Configures the version.
 */
#define LEDC_LEDC_DATE    0x0FFFFFFFU
#define LEDC_LEDC_DATE_M  (LEDC_LEDC_DATE_V << LEDC_LEDC_DATE_S)
#define LEDC_LEDC_DATE_V  0x0FFFFFFFU
#define LEDC_LEDC_DATE_S  0

#ifdef __cplusplus
}
#endif
