/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "soc/soc.h"
#ifdef __cplusplus
extern "C" {
#endif

/** MCPWM_CLK_CFG_REG register
 *  PWM clock prescaler register.
 */
#define MCPWM_CLK_CFG_REG (DR_REG_MCPWM_BASE + 0x0)
/** MCPWM_CLK_PRESCALE : R/W; bitpos: [7:0]; default: 0;
 *  Configures the prescaler value of clock, so that the period of PWM_clk = 6.25ns *
 *  (PWM_CLK_PRESCALE + 1).
 */
#define MCPWM_CLK_PRESCALE    0x000000FFU
#define MCPWM_CLK_PRESCALE_M  (MCPWM_CLK_PRESCALE_V << MCPWM_CLK_PRESCALE_S)
#define MCPWM_CLK_PRESCALE_V  0x000000FFU
#define MCPWM_CLK_PRESCALE_S  0

/** MCPWM_TIMER0_CFG0_REG register
 *  PWM timer0 period and update method configuration register.
 */
#define MCPWM_TIMER0_CFG0_REG (DR_REG_MCPWM_BASE + 0x4)
/** MCPWM_TIMER0_PRESCALE : R/W; bitpos: [7:0]; default: 0;
 *  Configures the prescaler value of timer0, so that the period of PT0_clk = Period of
 *  PWM_clk * (PWM_TIMER0_PRESCALE + 1)
 */
#define MCPWM_TIMER0_PRESCALE    0x000000FFU
#define MCPWM_TIMER0_PRESCALE_M  (MCPWM_TIMER0_PRESCALE_V << MCPWM_TIMER0_PRESCALE_S)
#define MCPWM_TIMER0_PRESCALE_V  0x000000FFU
#define MCPWM_TIMER0_PRESCALE_S  0
/** MCPWM_TIMER0_PERIOD : R/W; bitpos: [23:8]; default: 255;
 *  Configures the period shadow of PWM timer0
 */
#define MCPWM_TIMER0_PERIOD    0x0000FFFFU
#define MCPWM_TIMER0_PERIOD_M  (MCPWM_TIMER0_PERIOD_V << MCPWM_TIMER0_PERIOD_S)
#define MCPWM_TIMER0_PERIOD_V  0x0000FFFFU
#define MCPWM_TIMER0_PERIOD_S  8
/** MCPWM_TIMER0_PERIOD_UPMETHOD : R/W; bitpos: [25:24]; default: 0;
 *  Configures the update method for active register of PWM timer0 period.\\0:
 *  Immediate\\1: TEZ\\2: Sync\\3: TEZ or sync\\TEZ here and below means timer equal
 *  zero event
 */
#define MCPWM_TIMER0_PERIOD_UPMETHOD    0x00000003U
#define MCPWM_TIMER0_PERIOD_UPMETHOD_M  (MCPWM_TIMER0_PERIOD_UPMETHOD_V << MCPWM_TIMER0_PERIOD_UPMETHOD_S)
#define MCPWM_TIMER0_PERIOD_UPMETHOD_V  0x00000003U
#define MCPWM_TIMER0_PERIOD_UPMETHOD_S  24

/** MCPWM_TIMER0_CFG1_REG register
 *  PWM timer$n working mode and start/stop control register.
 */
#define MCPWM_TIMER0_CFG1_REG (DR_REG_MCPWM_BASE + 0x8)
/** MCPWM_TIMER0_START : R/W/SC; bitpos: [2:0]; default: 0;
 *  Configures whether or not to start/stop PWM timer$n.\\0: If PWM timer$n starts,
 *  then stops at TEZ\\1: If timer$n starts, then stops at TEP\\2: PWM timer$n starts
 *  and runs on\\3: Timer$n starts and stops at the next TEZ\\4: Timer0 starts and
 *  stops at the next TEP.\\TEP here and below means the event that happens when the
 *  timer equals to period
 */
#define MCPWM_TIMER0_START    0x00000007U
#define MCPWM_TIMER0_START_M  (MCPWM_TIMER0_START_V << MCPWM_TIMER0_START_S)
#define MCPWM_TIMER0_START_V  0x00000007U
#define MCPWM_TIMER0_START_S  0
/** MCPWM_TIMER0_MOD : R/W; bitpos: [4:3]; default: 0;
 *  Configures the working mode of PWM timer$n.\\0: Freeze\\1: Increase mode\\2:
 *  Decrease mode\\3: Up-down mode
 */
#define MCPWM_TIMER0_MOD    0x00000003U
#define MCPWM_TIMER0_MOD_M  (MCPWM_TIMER0_MOD_V << MCPWM_TIMER0_MOD_S)
#define MCPWM_TIMER0_MOD_V  0x00000003U
#define MCPWM_TIMER0_MOD_S  3

/** MCPWM_TIMER0_SYNC_REG register
 *  PWM timer$n sync function configuration register.
 */
#define MCPWM_TIMER0_SYNC_REG (DR_REG_MCPWM_BASE + 0xc)
/** MCPWM_TIMER0_SYNCI_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable timer$n reloading with phase on sync input
 *  event is enabled.\\0: Disable\\1: Enable
 */
#define MCPWM_TIMER0_SYNCI_EN    (BIT(0))
#define MCPWM_TIMER0_SYNCI_EN_M  (MCPWM_TIMER0_SYNCI_EN_V << MCPWM_TIMER0_SYNCI_EN_S)
#define MCPWM_TIMER0_SYNCI_EN_V  0x00000001U
#define MCPWM_TIMER0_SYNCI_EN_S  0
/** MCPWM_TIMER0_SYNC_SW : R/W; bitpos: [1]; default: 0;
 *  Configures the generation of software sync. Toggling this bit will trigger a
 *  software sync.
 */
#define MCPWM_TIMER0_SYNC_SW    (BIT(1))
#define MCPWM_TIMER0_SYNC_SW_M  (MCPWM_TIMER0_SYNC_SW_V << MCPWM_TIMER0_SYNC_SW_S)
#define MCPWM_TIMER0_SYNC_SW_V  0x00000001U
#define MCPWM_TIMER0_SYNC_SW_S  1
/** MCPWM_TIMER0_SYNCO_SEL : R/W; bitpos: [3:2]; default: 0;
 *  Configures the selection of PWM timer$n sync_out.\\0: Sync_in\\1: TEZ\\2: TEP\\3:
 *  Invalid, sync_out selects noting
 */
#define MCPWM_TIMER0_SYNCO_SEL    0x00000003U
#define MCPWM_TIMER0_SYNCO_SEL_M  (MCPWM_TIMER0_SYNCO_SEL_V << MCPWM_TIMER0_SYNCO_SEL_S)
#define MCPWM_TIMER0_SYNCO_SEL_V  0x00000003U
#define MCPWM_TIMER0_SYNCO_SEL_S  2
/** MCPWM_TIMER0_PHASE : R/W; bitpos: [19:4]; default: 0;
 *  Configures the phase for timer$n reload on sync event.
 */
#define MCPWM_TIMER0_PHASE    0x0000FFFFU
#define MCPWM_TIMER0_PHASE_M  (MCPWM_TIMER0_PHASE_V << MCPWM_TIMER0_PHASE_S)
#define MCPWM_TIMER0_PHASE_V  0x0000FFFFU
#define MCPWM_TIMER0_PHASE_S  4
/** MCPWM_TIMER0_PHASE_DIRECTION : R/W; bitpos: [20]; default: 0;
 *  Configures the PWM timer$n's direction when timer$n mode is up-down mode.\\0:
 *  Increase\\1: Decrease
 */
#define MCPWM_TIMER0_PHASE_DIRECTION    (BIT(20))
#define MCPWM_TIMER0_PHASE_DIRECTION_M  (MCPWM_TIMER0_PHASE_DIRECTION_V << MCPWM_TIMER0_PHASE_DIRECTION_S)
#define MCPWM_TIMER0_PHASE_DIRECTION_V  0x00000001U
#define MCPWM_TIMER0_PHASE_DIRECTION_S  20

/** MCPWM_TIMER0_STATUS_REG register
 *  PWM timer$n status register.
 */
#define MCPWM_TIMER0_STATUS_REG (DR_REG_MCPWM_BASE + 0x10)
/** MCPWM_TIMER0_VALUE : RO; bitpos: [15:0]; default: 0;
 *  Represents current PWM timer$n counter value.
 */
#define MCPWM_TIMER0_VALUE    0x0000FFFFU
#define MCPWM_TIMER0_VALUE_M  (MCPWM_TIMER0_VALUE_V << MCPWM_TIMER0_VALUE_S)
#define MCPWM_TIMER0_VALUE_V  0x0000FFFFU
#define MCPWM_TIMER0_VALUE_S  0
/** MCPWM_TIMER0_DIRECTION : RO; bitpos: [16]; default: 0;
 *  Represents current PWM timer$n counter direction.\\0: Increment\\1: Decrement
 */
#define MCPWM_TIMER0_DIRECTION    (BIT(16))
#define MCPWM_TIMER0_DIRECTION_M  (MCPWM_TIMER0_DIRECTION_V << MCPWM_TIMER0_DIRECTION_S)
#define MCPWM_TIMER0_DIRECTION_V  0x00000001U
#define MCPWM_TIMER0_DIRECTION_S  16

/** MCPWM_TIMER1_CFG0_REG register
 *  PWM timer1 period and update method configuration register.
 */
#define MCPWM_TIMER1_CFG0_REG (DR_REG_MCPWM_BASE + 0x14)
/** MCPWM_TIMER0_PRESCALE : R/W; bitpos: [7:0]; default: 0;
 *  Configures the prescaler value of timer1, so that the period of PT0_clk = Period of
 *  PWM_clk * (PWM_TIMER1_PRESCALE + 1)
 */
#define MCPWM_TIMER0_PRESCALE    0x000000FFU
#define MCPWM_TIMER0_PRESCALE_M  (MCPWM_TIMER0_PRESCALE_V << MCPWM_TIMER0_PRESCALE_S)
#define MCPWM_TIMER0_PRESCALE_V  0x000000FFU
#define MCPWM_TIMER0_PRESCALE_S  0
/** MCPWM_TIMER0_PERIOD : R/W; bitpos: [23:8]; default: 255;
 *  Configures the period shadow of PWM timer1
 */
#define MCPWM_TIMER0_PERIOD    0x0000FFFFU
#define MCPWM_TIMER0_PERIOD_M  (MCPWM_TIMER0_PERIOD_V << MCPWM_TIMER0_PERIOD_S)
#define MCPWM_TIMER0_PERIOD_V  0x0000FFFFU
#define MCPWM_TIMER0_PERIOD_S  8
/** MCPWM_TIMER0_PERIOD_UPMETHOD : R/W; bitpos: [25:24]; default: 0;
 *  Configures the update method for active register of PWM timer1 period.\\0:
 *  Immediate\\1: TEZ\\2: Sync\\3: TEZ or sync\\TEZ here and below means timer equal
 *  zero event
 */
#define MCPWM_TIMER0_PERIOD_UPMETHOD    0x00000003U
#define MCPWM_TIMER0_PERIOD_UPMETHOD_M  (MCPWM_TIMER0_PERIOD_UPMETHOD_V << MCPWM_TIMER0_PERIOD_UPMETHOD_S)
#define MCPWM_TIMER0_PERIOD_UPMETHOD_V  0x00000003U
#define MCPWM_TIMER0_PERIOD_UPMETHOD_S  24

/** MCPWM_TIMER1_CFG1_REG register
 *  PWM timer$n working mode and start/stop control register.
 */
#define MCPWM_TIMER1_CFG1_REG (DR_REG_MCPWM_BASE + 0x18)
/** MCPWM_TIMER1_START : R/W/SC; bitpos: [2:0]; default: 0;
 *  Configures whether or not to start/stop PWM timer$n.\\0: If PWM timer$n starts,
 *  then stops at TEZ\\1: If timer$n starts, then stops at TEP\\2: PWM timer$n starts
 *  and runs on\\3: Timer$n starts and stops at the next TEZ\\4: Timer0 starts and
 *  stops at the next TEP.\\TEP here and below means the event that happens when the
 *  timer equals to period
 */
#define MCPWM_TIMER1_START    0x00000007U
#define MCPWM_TIMER1_START_M  (MCPWM_TIMER1_START_V << MCPWM_TIMER1_START_S)
#define MCPWM_TIMER1_START_V  0x00000007U
#define MCPWM_TIMER1_START_S  0
/** MCPWM_TIMER1_MOD : R/W; bitpos: [4:3]; default: 0;
 *  Configures the working mode of PWM timer$n.\\0: Freeze\\1: Increase mode\\2:
 *  Decrease mode\\3: Up-down mode
 */
#define MCPWM_TIMER1_MOD    0x00000003U
#define MCPWM_TIMER1_MOD_M  (MCPWM_TIMER1_MOD_V << MCPWM_TIMER1_MOD_S)
#define MCPWM_TIMER1_MOD_V  0x00000003U
#define MCPWM_TIMER1_MOD_S  3

/** MCPWM_TIMER1_SYNC_REG register
 *  PWM timer$n sync function configuration register.
 */
#define MCPWM_TIMER1_SYNC_REG (DR_REG_MCPWM_BASE + 0x1c)
/** MCPWM_TIMER1_SYNCI_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable timer$n reloading with phase on sync input
 *  event is enabled.\\0: Disable\\1: Enable
 */
#define MCPWM_TIMER1_SYNCI_EN    (BIT(0))
#define MCPWM_TIMER1_SYNCI_EN_M  (MCPWM_TIMER1_SYNCI_EN_V << MCPWM_TIMER1_SYNCI_EN_S)
#define MCPWM_TIMER1_SYNCI_EN_V  0x00000001U
#define MCPWM_TIMER1_SYNCI_EN_S  0
/** MCPWM_TIMER1_SYNC_SW : R/W; bitpos: [1]; default: 0;
 *  Configures the generation of software sync. Toggling this bit will trigger a
 *  software sync.
 */
#define MCPWM_TIMER1_SYNC_SW    (BIT(1))
#define MCPWM_TIMER1_SYNC_SW_M  (MCPWM_TIMER1_SYNC_SW_V << MCPWM_TIMER1_SYNC_SW_S)
#define MCPWM_TIMER1_SYNC_SW_V  0x00000001U
#define MCPWM_TIMER1_SYNC_SW_S  1
/** MCPWM_TIMER1_SYNCO_SEL : R/W; bitpos: [3:2]; default: 0;
 *  Configures the selection of PWM timer$n sync_out.\\0: Sync_in\\1: TEZ\\2: TEP\\3:
 *  Invalid, sync_out selects noting
 */
#define MCPWM_TIMER1_SYNCO_SEL    0x00000003U
#define MCPWM_TIMER1_SYNCO_SEL_M  (MCPWM_TIMER1_SYNCO_SEL_V << MCPWM_TIMER1_SYNCO_SEL_S)
#define MCPWM_TIMER1_SYNCO_SEL_V  0x00000003U
#define MCPWM_TIMER1_SYNCO_SEL_S  2
/** MCPWM_TIMER1_PHASE : R/W; bitpos: [19:4]; default: 0;
 *  Configures the phase for timer$n reload on sync event.
 */
#define MCPWM_TIMER1_PHASE    0x0000FFFFU
#define MCPWM_TIMER1_PHASE_M  (MCPWM_TIMER1_PHASE_V << MCPWM_TIMER1_PHASE_S)
#define MCPWM_TIMER1_PHASE_V  0x0000FFFFU
#define MCPWM_TIMER1_PHASE_S  4
/** MCPWM_TIMER1_PHASE_DIRECTION : R/W; bitpos: [20]; default: 0;
 *  Configures the PWM timer$n's direction when timer$n mode is up-down mode.\\0:
 *  Increase\\1: Decrease
 */
#define MCPWM_TIMER1_PHASE_DIRECTION    (BIT(20))
#define MCPWM_TIMER1_PHASE_DIRECTION_M  (MCPWM_TIMER1_PHASE_DIRECTION_V << MCPWM_TIMER1_PHASE_DIRECTION_S)
#define MCPWM_TIMER1_PHASE_DIRECTION_V  0x00000001U
#define MCPWM_TIMER1_PHASE_DIRECTION_S  20

/** MCPWM_TIMER1_STATUS_REG register
 *  PWM timer$n status register.
 */
#define MCPWM_TIMER1_STATUS_REG (DR_REG_MCPWM_BASE + 0x20)
/** MCPWM_TIMER1_VALUE : RO; bitpos: [15:0]; default: 0;
 *  Represents current PWM timer$n counter value.
 */
#define MCPWM_TIMER1_VALUE    0x0000FFFFU
#define MCPWM_TIMER1_VALUE_M  (MCPWM_TIMER1_VALUE_V << MCPWM_TIMER1_VALUE_S)
#define MCPWM_TIMER1_VALUE_V  0x0000FFFFU
#define MCPWM_TIMER1_VALUE_S  0
/** MCPWM_TIMER1_DIRECTION : RO; bitpos: [16]; default: 0;
 *  Represents current PWM timer$n counter direction.\\0: Increment\\1: Decrement
 */
#define MCPWM_TIMER1_DIRECTION    (BIT(16))
#define MCPWM_TIMER1_DIRECTION_M  (MCPWM_TIMER1_DIRECTION_V << MCPWM_TIMER1_DIRECTION_S)
#define MCPWM_TIMER1_DIRECTION_V  0x00000001U
#define MCPWM_TIMER1_DIRECTION_S  16

/** MCPWM_TIMER2_CFG0_REG register
 *  PWM timer2 period and update method configuration register.
 */
#define MCPWM_TIMER2_CFG0_REG (DR_REG_MCPWM_BASE + 0x24)
/** MCPWM_TIMER0_PRESCALE : R/W; bitpos: [7:0]; default: 0;
 *  Configures the prescaler value of timer2, so that the period of PT0_clk = Period of
 *  PWM_clk * (PWM_TIMER2_PRESCALE + 1)
 */
#define MCPWM_TIMER0_PRESCALE    0x000000FFU
#define MCPWM_TIMER0_PRESCALE_M  (MCPWM_TIMER0_PRESCALE_V << MCPWM_TIMER0_PRESCALE_S)
#define MCPWM_TIMER0_PRESCALE_V  0x000000FFU
#define MCPWM_TIMER0_PRESCALE_S  0
/** MCPWM_TIMER0_PERIOD : R/W; bitpos: [23:8]; default: 255;
 *  Configures the period shadow of PWM timer2
 */
#define MCPWM_TIMER0_PERIOD    0x0000FFFFU
#define MCPWM_TIMER0_PERIOD_M  (MCPWM_TIMER0_PERIOD_V << MCPWM_TIMER0_PERIOD_S)
#define MCPWM_TIMER0_PERIOD_V  0x0000FFFFU
#define MCPWM_TIMER0_PERIOD_S  8
/** MCPWM_TIMER0_PERIOD_UPMETHOD : R/W; bitpos: [25:24]; default: 0;
 *  Configures the update method for active register of PWM timer2 period.\\0:
 *  Immediate\\1: TEZ\\2: Sync\\3: TEZ or sync\\TEZ here and below means timer equal
 *  zero event
 */
#define MCPWM_TIMER0_PERIOD_UPMETHOD    0x00000003U
#define MCPWM_TIMER0_PERIOD_UPMETHOD_M  (MCPWM_TIMER0_PERIOD_UPMETHOD_V << MCPWM_TIMER0_PERIOD_UPMETHOD_S)
#define MCPWM_TIMER0_PERIOD_UPMETHOD_V  0x00000003U
#define MCPWM_TIMER0_PERIOD_UPMETHOD_S  24

/** MCPWM_TIMER2_CFG1_REG register
 *  PWM timer$n working mode and start/stop control register.
 */
#define MCPWM_TIMER2_CFG1_REG (DR_REG_MCPWM_BASE + 0x28)
/** MCPWM_TIMER2_START : R/W/SC; bitpos: [2:0]; default: 0;
 *  Configures whether or not to start/stop PWM timer$n.\\0: If PWM timer$n starts,
 *  then stops at TEZ\\1: If timer$n starts, then stops at TEP\\2: PWM timer$n starts
 *  and runs on\\3: Timer$n starts and stops at the next TEZ\\4: Timer0 starts and
 *  stops at the next TEP.\\TEP here and below means the event that happens when the
 *  timer equals to period
 */
#define MCPWM_TIMER2_START    0x00000007U
#define MCPWM_TIMER2_START_M  (MCPWM_TIMER2_START_V << MCPWM_TIMER2_START_S)
#define MCPWM_TIMER2_START_V  0x00000007U
#define MCPWM_TIMER2_START_S  0
/** MCPWM_TIMER2_MOD : R/W; bitpos: [4:3]; default: 0;
 *  Configures the working mode of PWM timer$n.\\0: Freeze\\1: Increase mode\\2:
 *  Decrease mode\\3: Up-down mode
 */
#define MCPWM_TIMER2_MOD    0x00000003U
#define MCPWM_TIMER2_MOD_M  (MCPWM_TIMER2_MOD_V << MCPWM_TIMER2_MOD_S)
#define MCPWM_TIMER2_MOD_V  0x00000003U
#define MCPWM_TIMER2_MOD_S  3

/** MCPWM_TIMER2_SYNC_REG register
 *  PWM timer$n sync function configuration register.
 */
#define MCPWM_TIMER2_SYNC_REG (DR_REG_MCPWM_BASE + 0x2c)
/** MCPWM_TIMER2_SYNCI_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable timer$n reloading with phase on sync input
 *  event is enabled.\\0: Disable\\1: Enable
 */
#define MCPWM_TIMER2_SYNCI_EN    (BIT(0))
#define MCPWM_TIMER2_SYNCI_EN_M  (MCPWM_TIMER2_SYNCI_EN_V << MCPWM_TIMER2_SYNCI_EN_S)
#define MCPWM_TIMER2_SYNCI_EN_V  0x00000001U
#define MCPWM_TIMER2_SYNCI_EN_S  0
/** MCPWM_TIMER2_SYNC_SW : R/W; bitpos: [1]; default: 0;
 *  Configures the generation of software sync. Toggling this bit will trigger a
 *  software sync.
 */
#define MCPWM_TIMER2_SYNC_SW    (BIT(1))
#define MCPWM_TIMER2_SYNC_SW_M  (MCPWM_TIMER2_SYNC_SW_V << MCPWM_TIMER2_SYNC_SW_S)
#define MCPWM_TIMER2_SYNC_SW_V  0x00000001U
#define MCPWM_TIMER2_SYNC_SW_S  1
/** MCPWM_TIMER2_SYNCO_SEL : R/W; bitpos: [3:2]; default: 0;
 *  Configures the selection of PWM timer$n sync_out.\\0: Sync_in\\1: TEZ\\2: TEP\\3:
 *  Invalid, sync_out selects noting
 */
#define MCPWM_TIMER2_SYNCO_SEL    0x00000003U
#define MCPWM_TIMER2_SYNCO_SEL_M  (MCPWM_TIMER2_SYNCO_SEL_V << MCPWM_TIMER2_SYNCO_SEL_S)
#define MCPWM_TIMER2_SYNCO_SEL_V  0x00000003U
#define MCPWM_TIMER2_SYNCO_SEL_S  2
/** MCPWM_TIMER2_PHASE : R/W; bitpos: [19:4]; default: 0;
 *  Configures the phase for timer$n reload on sync event.
 */
#define MCPWM_TIMER2_PHASE    0x0000FFFFU
#define MCPWM_TIMER2_PHASE_M  (MCPWM_TIMER2_PHASE_V << MCPWM_TIMER2_PHASE_S)
#define MCPWM_TIMER2_PHASE_V  0x0000FFFFU
#define MCPWM_TIMER2_PHASE_S  4
/** MCPWM_TIMER2_PHASE_DIRECTION : R/W; bitpos: [20]; default: 0;
 *  Configures the PWM timer$n's direction when timer$n mode is up-down mode.\\0:
 *  Increase\\1: Decrease
 */
#define MCPWM_TIMER2_PHASE_DIRECTION    (BIT(20))
#define MCPWM_TIMER2_PHASE_DIRECTION_M  (MCPWM_TIMER2_PHASE_DIRECTION_V << MCPWM_TIMER2_PHASE_DIRECTION_S)
#define MCPWM_TIMER2_PHASE_DIRECTION_V  0x00000001U
#define MCPWM_TIMER2_PHASE_DIRECTION_S  20

/** MCPWM_TIMER2_STATUS_REG register
 *  PWM timer$n status register.
 */
#define MCPWM_TIMER2_STATUS_REG (DR_REG_MCPWM_BASE + 0x30)
/** MCPWM_TIMER2_VALUE : RO; bitpos: [15:0]; default: 0;
 *  Represents current PWM timer$n counter value.
 */
#define MCPWM_TIMER2_VALUE    0x0000FFFFU
#define MCPWM_TIMER2_VALUE_M  (MCPWM_TIMER2_VALUE_V << MCPWM_TIMER2_VALUE_S)
#define MCPWM_TIMER2_VALUE_V  0x0000FFFFU
#define MCPWM_TIMER2_VALUE_S  0
/** MCPWM_TIMER2_DIRECTION : RO; bitpos: [16]; default: 0;
 *  Represents current PWM timer$n counter direction.\\0: Increment\\1: Decrement
 */
#define MCPWM_TIMER2_DIRECTION    (BIT(16))
#define MCPWM_TIMER2_DIRECTION_M  (MCPWM_TIMER2_DIRECTION_V << MCPWM_TIMER2_DIRECTION_S)
#define MCPWM_TIMER2_DIRECTION_V  0x00000001U
#define MCPWM_TIMER2_DIRECTION_S  16

/** MCPWM_TIMER_SYNCI_CFG_REG register
 *  Synchronization input selection register for PWM timers.
 */
#define MCPWM_TIMER_SYNCI_CFG_REG (DR_REG_MCPWM_BASE + 0x34)
/** MCPWM_TIMER0_SYNCISEL : R/W; bitpos: [2:0]; default: 0;
 *  Configures the selection of sync input for PWM timer0.\\1: PWM timer0 sync_out\\2:
 *  PWM timer1 sync_out\\3: PWM timer2 sync_out\\4: SYNC0 from GPIO matrix\\5: SYNC1
 *  from GPIO matrix\\6: SYNC2 from GPIO matrix\\Other values: No sync input selected
 */
#define MCPWM_TIMER0_SYNCISEL    0x00000007U
#define MCPWM_TIMER0_SYNCISEL_M  (MCPWM_TIMER0_SYNCISEL_V << MCPWM_TIMER0_SYNCISEL_S)
#define MCPWM_TIMER0_SYNCISEL_V  0x00000007U
#define MCPWM_TIMER0_SYNCISEL_S  0
/** MCPWM_TIMER1_SYNCISEL : R/W; bitpos: [5:3]; default: 0;
 *  Configures the selection of sync input for PWM timer1.\\1: PWM timer0 sync_out\\2:
 *  PWM timer1 sync_out\\3: PWM timer2 sync_out\\4: SYNC0 from GPIO matrix\\5: SYNC1
 *  from GPIO matrix\\6: SYNC2 from GPIO matrix\\Other values: No sync input selected
 */
#define MCPWM_TIMER1_SYNCISEL    0x00000007U
#define MCPWM_TIMER1_SYNCISEL_M  (MCPWM_TIMER1_SYNCISEL_V << MCPWM_TIMER1_SYNCISEL_S)
#define MCPWM_TIMER1_SYNCISEL_V  0x00000007U
#define MCPWM_TIMER1_SYNCISEL_S  3
/** MCPWM_TIMER2_SYNCISEL : R/W; bitpos: [8:6]; default: 0;
 *  Configures the selection of sync input for PWM timer2.\\1: PWM timer0 sync_out\\2:
 *  PWM timer1 sync_out\\3: PWM timer2 sync_out\\4: SYNC0 from GPIO matrix\\5: SYNC1
 *  from GPIO matrix\\6: SYNC2 from GPIO matrix\\Other values: No sync input selected
 */
#define MCPWM_TIMER2_SYNCISEL    0x00000007U
#define MCPWM_TIMER2_SYNCISEL_M  (MCPWM_TIMER2_SYNCISEL_V << MCPWM_TIMER2_SYNCISEL_S)
#define MCPWM_TIMER2_SYNCISEL_V  0x00000007U
#define MCPWM_TIMER2_SYNCISEL_S  6
/** MCPWM_EXTERNAL_SYNCI0_INVERT : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to invert SYNC0 from GPIO matrix.\\0: Not invert\\1:
 *  Invert
 */
#define MCPWM_EXTERNAL_SYNCI0_INVERT    (BIT(9))
#define MCPWM_EXTERNAL_SYNCI0_INVERT_M  (MCPWM_EXTERNAL_SYNCI0_INVERT_V << MCPWM_EXTERNAL_SYNCI0_INVERT_S)
#define MCPWM_EXTERNAL_SYNCI0_INVERT_V  0x00000001U
#define MCPWM_EXTERNAL_SYNCI0_INVERT_S  9
/** MCPWM_EXTERNAL_SYNCI1_INVERT : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to invert SYNC1 from GPIO matrix.\\0: Not invert\\1:
 *  Invert
 */
#define MCPWM_EXTERNAL_SYNCI1_INVERT    (BIT(10))
#define MCPWM_EXTERNAL_SYNCI1_INVERT_M  (MCPWM_EXTERNAL_SYNCI1_INVERT_V << MCPWM_EXTERNAL_SYNCI1_INVERT_S)
#define MCPWM_EXTERNAL_SYNCI1_INVERT_V  0x00000001U
#define MCPWM_EXTERNAL_SYNCI1_INVERT_S  10
/** MCPWM_EXTERNAL_SYNCI2_INVERT : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert SYNC2 from GPIO matrix.\\0: Not invert\\1:
 *  Invert
 */
#define MCPWM_EXTERNAL_SYNCI2_INVERT    (BIT(11))
#define MCPWM_EXTERNAL_SYNCI2_INVERT_M  (MCPWM_EXTERNAL_SYNCI2_INVERT_V << MCPWM_EXTERNAL_SYNCI2_INVERT_S)
#define MCPWM_EXTERNAL_SYNCI2_INVERT_V  0x00000001U
#define MCPWM_EXTERNAL_SYNCI2_INVERT_S  11

/** MCPWM_OPERATOR_TIMERSEL_REG register
 *  PWM operator's timer select register
 */
#define MCPWM_OPERATOR_TIMERSEL_REG (DR_REG_MCPWM_BASE + 0x38)
/** MCPWM_OPERATOR0_TIMERSEL : R/W; bitpos: [1:0]; default: 0;
 *  Configures which PWM timer will be the timing reference for PWM operator0.\\0:
 *  Timer0\\1: Timer1\\2: Timer2\\3: Invalid, will select timer2
 */
#define MCPWM_OPERATOR0_TIMERSEL    0x00000003U
#define MCPWM_OPERATOR0_TIMERSEL_M  (MCPWM_OPERATOR0_TIMERSEL_V << MCPWM_OPERATOR0_TIMERSEL_S)
#define MCPWM_OPERATOR0_TIMERSEL_V  0x00000003U
#define MCPWM_OPERATOR0_TIMERSEL_S  0
/** MCPWM_OPERATOR1_TIMERSEL : R/W; bitpos: [3:2]; default: 0;
 *  Configures which PWM timer will be the timing reference for PWM operator1.\\0:
 *  Timer0\\1: Timer1\\2: Timer2\\3: Invalid, will select timer2
 */
#define MCPWM_OPERATOR1_TIMERSEL    0x00000003U
#define MCPWM_OPERATOR1_TIMERSEL_M  (MCPWM_OPERATOR1_TIMERSEL_V << MCPWM_OPERATOR1_TIMERSEL_S)
#define MCPWM_OPERATOR1_TIMERSEL_V  0x00000003U
#define MCPWM_OPERATOR1_TIMERSEL_S  2
/** MCPWM_OPERATOR2_TIMERSEL : R/W; bitpos: [5:4]; default: 0;
 *  Configures which PWM timer will be the timing reference for PWM operator2.\\0:
 *  Timer0\\1: Timer1\\2: Timer2\\3: Invalid, will select timer2
 */
#define MCPWM_OPERATOR2_TIMERSEL    0x00000003U
#define MCPWM_OPERATOR2_TIMERSEL_M  (MCPWM_OPERATOR2_TIMERSEL_V << MCPWM_OPERATOR2_TIMERSEL_S)
#define MCPWM_OPERATOR2_TIMERSEL_V  0x00000003U
#define MCPWM_OPERATOR2_TIMERSEL_S  4

/** MCPWM_GEN0_STMP_CFG_REG register
 *  Generator0 time stamp registers A and B transfer status and update method register
 */
#define MCPWM_GEN0_STMP_CFG_REG (DR_REG_MCPWM_BASE + 0x3c)
/** MCPWM_CMPR0_A_UPMETHOD : R/W; bitpos: [3:0]; default: 0;
 *  Configures the update method for PWM generator 0 time stamp A's active
 *  register.\\0: Immediately\\Bit0 is set to 1: TEZ\\Bit1 is set to 1: TEP\\Bit2 is
 *  set to 1: Sync\\Bit3 is set to 1: Disable the update
 */
#define MCPWM_CMPR0_A_UPMETHOD    0x0000000FU
#define MCPWM_CMPR0_A_UPMETHOD_M  (MCPWM_CMPR0_A_UPMETHOD_V << MCPWM_CMPR0_A_UPMETHOD_S)
#define MCPWM_CMPR0_A_UPMETHOD_V  0x0000000FU
#define MCPWM_CMPR0_A_UPMETHOD_S  0
/** MCPWM_CMPR0_B_UPMETHOD : R/W; bitpos: [7:4]; default: 0;
 *  Configures the update method for PWM generator 0 time stamp B's active
 *  register.\\0: Immediately\\Bit0 is set to 1: TEZ\\Bit1 is set to 1: TEP\\Bit2 is
 *  set to 1: Sync\\Bit3 is set to 1: Disable the update
 */
#define MCPWM_CMPR0_B_UPMETHOD    0x0000000FU
#define MCPWM_CMPR0_B_UPMETHOD_M  (MCPWM_CMPR0_B_UPMETHOD_V << MCPWM_CMPR0_B_UPMETHOD_S)
#define MCPWM_CMPR0_B_UPMETHOD_V  0x0000000FU
#define MCPWM_CMPR0_B_UPMETHOD_S  4
/** MCPWM_CMPR0_A_SHDW_FULL : R/W/WTC/SC; bitpos: [8]; default: 0;
 *  Represents whether or not generator0 time stamp A's shadow reg is transferred.\\0:
 *  A's active reg has been updated with shadow register latest value.\\1: A's shadow
 *  reg is filled and waiting to be transferred to A's active reg
 */
#define MCPWM_CMPR0_A_SHDW_FULL    (BIT(8))
#define MCPWM_CMPR0_A_SHDW_FULL_M  (MCPWM_CMPR0_A_SHDW_FULL_V << MCPWM_CMPR0_A_SHDW_FULL_S)
#define MCPWM_CMPR0_A_SHDW_FULL_V  0x00000001U
#define MCPWM_CMPR0_A_SHDW_FULL_S  8
/** MCPWM_CMPR0_B_SHDW_FULL : R/W/WTC/SC; bitpos: [9]; default: 0;
 *  Represents whether or not generator0 time stamp B's shadow reg is transferred.\\0:
 *  B's active reg has been updated with shadow register latest value.\\1: B's shadow
 *  reg is filled and waiting to be transferred to B's active reg
 */
#define MCPWM_CMPR0_B_SHDW_FULL    (BIT(9))
#define MCPWM_CMPR0_B_SHDW_FULL_M  (MCPWM_CMPR0_B_SHDW_FULL_V << MCPWM_CMPR0_B_SHDW_FULL_S)
#define MCPWM_CMPR0_B_SHDW_FULL_V  0x00000001U
#define MCPWM_CMPR0_B_SHDW_FULL_S  9

/** MCPWM_GEN0_TSTMP_A_REG register
 *  Generator$n time stamp A's shadow register
 */
#define MCPWM_GEN0_TSTMP_A_REG (DR_REG_MCPWM_BASE + 0x40)
/** MCPWM_CMPR0_A : R/W; bitpos: [15:0]; default: 0;
 *  Configures the value of PWM generator $n time stamp A's shadow register.
 */
#define MCPWM_CMPR0_A    0x0000FFFFU
#define MCPWM_CMPR0_A_M  (MCPWM_CMPR0_A_V << MCPWM_CMPR0_A_S)
#define MCPWM_CMPR0_A_V  0x0000FFFFU
#define MCPWM_CMPR0_A_S  0

/** MCPWM_GEN0_TSTMP_B_REG register
 *  Generator$n time stamp B's shadow register
 */
#define MCPWM_GEN0_TSTMP_B_REG (DR_REG_MCPWM_BASE + 0x44)
/** MCPWM_CMPR0_B : R/W; bitpos: [15:0]; default: 0;
 *  Configures the value of PWM generator $n time stamp B's shadow register.
 */
#define MCPWM_CMPR0_B    0x0000FFFFU
#define MCPWM_CMPR0_B_M  (MCPWM_CMPR0_B_V << MCPWM_CMPR0_B_S)
#define MCPWM_CMPR0_B_V  0x0000FFFFU
#define MCPWM_CMPR0_B_S  0

/** MCPWM_GEN0_CFG0_REG register
 *  Generator$n fault event T0 and T1 configuration register
 */
#define MCPWM_GEN0_CFG0_REG (DR_REG_MCPWM_BASE + 0x48)
/** MCPWM_GEN0_CFG_UPMETHOD : R/W; bitpos: [3:0]; default: 0;
 *  Configures update method for PWM generator $n's active register.\\0:
 *  Immediately\\Bit0 is set to 1: TEZ\\Bit1 is set to 1: TEP\\Bit2 is set to 1:
 *  Sync\\Bit3 is set to 1: Disable the update
 */
#define MCPWM_GEN0_CFG_UPMETHOD    0x0000000FU
#define MCPWM_GEN0_CFG_UPMETHOD_M  (MCPWM_GEN0_CFG_UPMETHOD_V << MCPWM_GEN0_CFG_UPMETHOD_S)
#define MCPWM_GEN0_CFG_UPMETHOD_V  0x0000000FU
#define MCPWM_GEN0_CFG_UPMETHOD_S  0
/** MCPWM_GEN0_T0_SEL : R/W; bitpos: [6:4]; default: 0;
 *  Configures source selection for PWM generator $n event_t0, take effect
 *  immediately.\\0: fault_event0\\1: fault_event1\\2: fault_event2\\3: sync_taken\\4:
 *  Invalid, Select nothing
 */
#define MCPWM_GEN0_T0_SEL    0x00000007U
#define MCPWM_GEN0_T0_SEL_M  (MCPWM_GEN0_T0_SEL_V << MCPWM_GEN0_T0_SEL_S)
#define MCPWM_GEN0_T0_SEL_V  0x00000007U
#define MCPWM_GEN0_T0_SEL_S  4
/** MCPWM_GEN0_T1_SEL : R/W; bitpos: [9:7]; default: 0;
 *  Configures source selection for PWM generator $n event_t1, take effect
 *  immediately.\\0: fault_event0\\1: fault_event1\\2: fault_event2\\3: sync_taken\\4:
 *  Invalid, Select nothing
 */
#define MCPWM_GEN0_T1_SEL    0x00000007U
#define MCPWM_GEN0_T1_SEL_M  (MCPWM_GEN0_T1_SEL_V << MCPWM_GEN0_T1_SEL_S)
#define MCPWM_GEN0_T1_SEL_V  0x00000007U
#define MCPWM_GEN0_T1_SEL_S  7

/** MCPWM_GEN0_FORCE_REG register
 *  Generator$n output signal force mode register.
 */
#define MCPWM_GEN0_FORCE_REG (DR_REG_MCPWM_BASE + 0x4c)
/** MCPWM_GEN0_CNTUFORCE_UPMETHOD : R/W; bitpos: [5:0]; default: 32;
 *  Configures update method for continuous software force of PWM generator$n.\\0:
 *  Immediately\\Bit0 is set to 1: TEZ\\Bit1 is set to 1: TEP\\Bit2 is set to 1:
 *  TEA\\Bit3 is set to 1: TEB\\Bit4 is set to 1: Sync\\Bit5 is set to 1: Disable
 *  update. TEA/B here and below means an event generated when the timer's value equals
 *  to that of register A/B.
 */
#define MCPWM_GEN0_CNTUFORCE_UPMETHOD    0x0000003FU
#define MCPWM_GEN0_CNTUFORCE_UPMETHOD_M  (MCPWM_GEN0_CNTUFORCE_UPMETHOD_V << MCPWM_GEN0_CNTUFORCE_UPMETHOD_S)
#define MCPWM_GEN0_CNTUFORCE_UPMETHOD_V  0x0000003FU
#define MCPWM_GEN0_CNTUFORCE_UPMETHOD_S  0
/** MCPWM_GEN0_A_CNTUFORCE_MODE : R/W; bitpos: [7:6]; default: 0;
 *  Configures continuous software force mode for PWM$n A.\\0: Disabled\\1: Low\\2:
 *  High\\3: Disabled
 */
#define MCPWM_GEN0_A_CNTUFORCE_MODE    0x00000003U
#define MCPWM_GEN0_A_CNTUFORCE_MODE_M  (MCPWM_GEN0_A_CNTUFORCE_MODE_V << MCPWM_GEN0_A_CNTUFORCE_MODE_S)
#define MCPWM_GEN0_A_CNTUFORCE_MODE_V  0x00000003U
#define MCPWM_GEN0_A_CNTUFORCE_MODE_S  6
/** MCPWM_GEN0_B_CNTUFORCE_MODE : R/W; bitpos: [9:8]; default: 0;
 *  Configures continuous software force mode for PWM$n B.\\0: Disabled\\1: Low\\2:
 *  High\\3: Disabled
 */
#define MCPWM_GEN0_B_CNTUFORCE_MODE    0x00000003U
#define MCPWM_GEN0_B_CNTUFORCE_MODE_M  (MCPWM_GEN0_B_CNTUFORCE_MODE_V << MCPWM_GEN0_B_CNTUFORCE_MODE_S)
#define MCPWM_GEN0_B_CNTUFORCE_MODE_V  0x00000003U
#define MCPWM_GEN0_B_CNTUFORCE_MODE_S  8
/** MCPWM_GEN0_A_NCIFORCE : R/W; bitpos: [10]; default: 0;
 *  Configures the generation of non-continuous immediate software-force event for
 *  PWM$n A, a toggle will trigger a force event.
 */
#define MCPWM_GEN0_A_NCIFORCE    (BIT(10))
#define MCPWM_GEN0_A_NCIFORCE_M  (MCPWM_GEN0_A_NCIFORCE_V << MCPWM_GEN0_A_NCIFORCE_S)
#define MCPWM_GEN0_A_NCIFORCE_V  0x00000001U
#define MCPWM_GEN0_A_NCIFORCE_S  10
/** MCPWM_GEN0_A_NCIFORCE_MODE : R/W; bitpos: [12:11]; default: 0;
 *  Configures non-continuous immediate software force mode for PWM$n A.\\0:
 *  Disabled\\1: Low\\2: High\\3: Disabled
 */
#define MCPWM_GEN0_A_NCIFORCE_MODE    0x00000003U
#define MCPWM_GEN0_A_NCIFORCE_MODE_M  (MCPWM_GEN0_A_NCIFORCE_MODE_V << MCPWM_GEN0_A_NCIFORCE_MODE_S)
#define MCPWM_GEN0_A_NCIFORCE_MODE_V  0x00000003U
#define MCPWM_GEN0_A_NCIFORCE_MODE_S  11
/** MCPWM_GEN0_B_NCIFORCE : R/W; bitpos: [13]; default: 0;
 *  Configures the generation of non-continuous immediate software-force event for
 *  PWM$n B, a toggle will trigger a force event.
 */
#define MCPWM_GEN0_B_NCIFORCE    (BIT(13))
#define MCPWM_GEN0_B_NCIFORCE_M  (MCPWM_GEN0_B_NCIFORCE_V << MCPWM_GEN0_B_NCIFORCE_S)
#define MCPWM_GEN0_B_NCIFORCE_V  0x00000001U
#define MCPWM_GEN0_B_NCIFORCE_S  13
/** MCPWM_GEN0_B_NCIFORCE_MODE : R/W; bitpos: [15:14]; default: 0;
 *  Configures non-continuous immediate software force mode for PWM$n B.\\0:
 *  Disabled\\1: Low\\2: High\\3: Disabled
 */
#define MCPWM_GEN0_B_NCIFORCE_MODE    0x00000003U
#define MCPWM_GEN0_B_NCIFORCE_MODE_M  (MCPWM_GEN0_B_NCIFORCE_MODE_V << MCPWM_GEN0_B_NCIFORCE_MODE_S)
#define MCPWM_GEN0_B_NCIFORCE_MODE_V  0x00000003U
#define MCPWM_GEN0_B_NCIFORCE_MODE_S  14

/** MCPWM_GEN0_A_REG register
 *  PWM$n output signal A actions configuration register
 */
#define MCPWM_GEN0_A_REG (DR_REG_MCPWM_BASE + 0x50)
/** MCPWM_GEN0_A_UTEZ : R/W; bitpos: [1:0]; default: 0;
 *  Configures action on PWM$n A triggered by event TEZ when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN0_A_UTEZ    0x00000003U
#define MCPWM_GEN0_A_UTEZ_M  (MCPWM_GEN0_A_UTEZ_V << MCPWM_GEN0_A_UTEZ_S)
#define MCPWM_GEN0_A_UTEZ_V  0x00000003U
#define MCPWM_GEN0_A_UTEZ_S  0
/** MCPWM_GEN0_A_UTEP : R/W; bitpos: [3:2]; default: 0;
 *  Configures action on PWM$n A triggered by event TEP when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN0_A_UTEP    0x00000003U
#define MCPWM_GEN0_A_UTEP_M  (MCPWM_GEN0_A_UTEP_V << MCPWM_GEN0_A_UTEP_S)
#define MCPWM_GEN0_A_UTEP_V  0x00000003U
#define MCPWM_GEN0_A_UTEP_S  2
/** MCPWM_GEN0_A_UTEA : R/W; bitpos: [5:4]; default: 0;
 *  Configures action on PWM$n A triggered by event TEA when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN0_A_UTEA    0x00000003U
#define MCPWM_GEN0_A_UTEA_M  (MCPWM_GEN0_A_UTEA_V << MCPWM_GEN0_A_UTEA_S)
#define MCPWM_GEN0_A_UTEA_V  0x00000003U
#define MCPWM_GEN0_A_UTEA_S  4
/** MCPWM_GEN0_A_UTEB : R/W; bitpos: [7:6]; default: 0;
 *  Configures action on PWM$n A triggered by event TEB when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN0_A_UTEB    0x00000003U
#define MCPWM_GEN0_A_UTEB_M  (MCPWM_GEN0_A_UTEB_V << MCPWM_GEN0_A_UTEB_S)
#define MCPWM_GEN0_A_UTEB_V  0x00000003U
#define MCPWM_GEN0_A_UTEB_S  6
/** MCPWM_GEN0_A_UT0 : R/W; bitpos: [9:8]; default: 0;
 *  Configures action on PWM$n A triggered by event_t0 when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN0_A_UT0    0x00000003U
#define MCPWM_GEN0_A_UT0_M  (MCPWM_GEN0_A_UT0_V << MCPWM_GEN0_A_UT0_S)
#define MCPWM_GEN0_A_UT0_V  0x00000003U
#define MCPWM_GEN0_A_UT0_S  8
/** MCPWM_GEN0_A_UT1 : R/W; bitpos: [11:10]; default: 0;
 *  Configures action on PWM$n A triggered by event_t1 when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN0_A_UT1    0x00000003U
#define MCPWM_GEN0_A_UT1_M  (MCPWM_GEN0_A_UT1_V << MCPWM_GEN0_A_UT1_S)
#define MCPWM_GEN0_A_UT1_V  0x00000003U
#define MCPWM_GEN0_A_UT1_S  10
/** MCPWM_GEN0_A_DTEZ : R/W; bitpos: [13:12]; default: 0;
 *  Configures action on PWM$n A triggered by event TEZ when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN0_A_DTEZ    0x00000003U
#define MCPWM_GEN0_A_DTEZ_M  (MCPWM_GEN0_A_DTEZ_V << MCPWM_GEN0_A_DTEZ_S)
#define MCPWM_GEN0_A_DTEZ_V  0x00000003U
#define MCPWM_GEN0_A_DTEZ_S  12
/** MCPWM_GEN0_A_DTEP : R/W; bitpos: [15:14]; default: 0;
 *  Configures action on PWM$n A triggered by event TEP when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN0_A_DTEP    0x00000003U
#define MCPWM_GEN0_A_DTEP_M  (MCPWM_GEN0_A_DTEP_V << MCPWM_GEN0_A_DTEP_S)
#define MCPWM_GEN0_A_DTEP_V  0x00000003U
#define MCPWM_GEN0_A_DTEP_S  14
/** MCPWM_GEN0_A_DTEA : R/W; bitpos: [17:16]; default: 0;
 *  Configures action on PWM$n A triggered by event TEA when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN0_A_DTEA    0x00000003U
#define MCPWM_GEN0_A_DTEA_M  (MCPWM_GEN0_A_DTEA_V << MCPWM_GEN0_A_DTEA_S)
#define MCPWM_GEN0_A_DTEA_V  0x00000003U
#define MCPWM_GEN0_A_DTEA_S  16
/** MCPWM_GEN0_A_DTEB : R/W; bitpos: [19:18]; default: 0;
 *  Configures action on PWM$n A triggered by event TEB when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN0_A_DTEB    0x00000003U
#define MCPWM_GEN0_A_DTEB_M  (MCPWM_GEN0_A_DTEB_V << MCPWM_GEN0_A_DTEB_S)
#define MCPWM_GEN0_A_DTEB_V  0x00000003U
#define MCPWM_GEN0_A_DTEB_S  18
/** MCPWM_GEN0_A_DT0 : R/W; bitpos: [21:20]; default: 0;
 *  Configures action on PWM$n A triggered by event_t0 when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN0_A_DT0    0x00000003U
#define MCPWM_GEN0_A_DT0_M  (MCPWM_GEN0_A_DT0_V << MCPWM_GEN0_A_DT0_S)
#define MCPWM_GEN0_A_DT0_V  0x00000003U
#define MCPWM_GEN0_A_DT0_S  20
/** MCPWM_GEN0_A_DT1 : R/W; bitpos: [23:22]; default: 0;
 *  Configures action on PWM$n A triggered by event_t1 when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN0_A_DT1    0x00000003U
#define MCPWM_GEN0_A_DT1_M  (MCPWM_GEN0_A_DT1_V << MCPWM_GEN0_A_DT1_S)
#define MCPWM_GEN0_A_DT1_V  0x00000003U
#define MCPWM_GEN0_A_DT1_S  22

/** MCPWM_GEN0_B_REG register
 *  PWM$n output signal B actions configuration register
 */
#define MCPWM_GEN0_B_REG (DR_REG_MCPWM_BASE + 0x54)
/** MCPWM_GEN0_B_UTEZ : R/W; bitpos: [1:0]; default: 0;
 *  Configures action on PWM$n B triggered by event TEZ when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN0_B_UTEZ    0x00000003U
#define MCPWM_GEN0_B_UTEZ_M  (MCPWM_GEN0_B_UTEZ_V << MCPWM_GEN0_B_UTEZ_S)
#define MCPWM_GEN0_B_UTEZ_V  0x00000003U
#define MCPWM_GEN0_B_UTEZ_S  0
/** MCPWM_GEN0_B_UTEP : R/W; bitpos: [3:2]; default: 0;
 *  Configures action on PWM$n B triggered by event TEP when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN0_B_UTEP    0x00000003U
#define MCPWM_GEN0_B_UTEP_M  (MCPWM_GEN0_B_UTEP_V << MCPWM_GEN0_B_UTEP_S)
#define MCPWM_GEN0_B_UTEP_V  0x00000003U
#define MCPWM_GEN0_B_UTEP_S  2
/** MCPWM_GEN0_B_UTEA : R/W; bitpos: [5:4]; default: 0;
 *  Configures action on PWM$n B triggered by event TEA when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN0_B_UTEA    0x00000003U
#define MCPWM_GEN0_B_UTEA_M  (MCPWM_GEN0_B_UTEA_V << MCPWM_GEN0_B_UTEA_S)
#define MCPWM_GEN0_B_UTEA_V  0x00000003U
#define MCPWM_GEN0_B_UTEA_S  4
/** MCPWM_GEN0_B_UTEB : R/W; bitpos: [7:6]; default: 0;
 *  Configures action on PWM$n B triggered by event TEB when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN0_B_UTEB    0x00000003U
#define MCPWM_GEN0_B_UTEB_M  (MCPWM_GEN0_B_UTEB_V << MCPWM_GEN0_B_UTEB_S)
#define MCPWM_GEN0_B_UTEB_V  0x00000003U
#define MCPWM_GEN0_B_UTEB_S  6
/** MCPWM_GEN0_B_UT0 : R/W; bitpos: [9:8]; default: 0;
 *  Configures action on PWM$n B triggered by event_t0 when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN0_B_UT0    0x00000003U
#define MCPWM_GEN0_B_UT0_M  (MCPWM_GEN0_B_UT0_V << MCPWM_GEN0_B_UT0_S)
#define MCPWM_GEN0_B_UT0_V  0x00000003U
#define MCPWM_GEN0_B_UT0_S  8
/** MCPWM_GEN0_B_UT1 : R/W; bitpos: [11:10]; default: 0;
 *  Configures action on PWM$n B triggered by event_t1 when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN0_B_UT1    0x00000003U
#define MCPWM_GEN0_B_UT1_M  (MCPWM_GEN0_B_UT1_V << MCPWM_GEN0_B_UT1_S)
#define MCPWM_GEN0_B_UT1_V  0x00000003U
#define MCPWM_GEN0_B_UT1_S  10
/** MCPWM_GEN0_B_DTEZ : R/W; bitpos: [13:12]; default: 0;
 *  Configures action on PWM$n B triggered by event TEZ when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN0_B_DTEZ    0x00000003U
#define MCPWM_GEN0_B_DTEZ_M  (MCPWM_GEN0_B_DTEZ_V << MCPWM_GEN0_B_DTEZ_S)
#define MCPWM_GEN0_B_DTEZ_V  0x00000003U
#define MCPWM_GEN0_B_DTEZ_S  12
/** MCPWM_GEN0_B_DTEP : R/W; bitpos: [15:14]; default: 0;
 *  Configures action on PWM$n B triggered by event TEP when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN0_B_DTEP    0x00000003U
#define MCPWM_GEN0_B_DTEP_M  (MCPWM_GEN0_B_DTEP_V << MCPWM_GEN0_B_DTEP_S)
#define MCPWM_GEN0_B_DTEP_V  0x00000003U
#define MCPWM_GEN0_B_DTEP_S  14
/** MCPWM_GEN0_B_DTEA : R/W; bitpos: [17:16]; default: 0;
 *  Configures action on PWM$n B triggered by event TEA when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN0_B_DTEA    0x00000003U
#define MCPWM_GEN0_B_DTEA_M  (MCPWM_GEN0_B_DTEA_V << MCPWM_GEN0_B_DTEA_S)
#define MCPWM_GEN0_B_DTEA_V  0x00000003U
#define MCPWM_GEN0_B_DTEA_S  16
/** MCPWM_GEN0_B_DTEB : R/W; bitpos: [19:18]; default: 0;
 *  Configures action on PWM$n B triggered by event TEB when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN0_B_DTEB    0x00000003U
#define MCPWM_GEN0_B_DTEB_M  (MCPWM_GEN0_B_DTEB_V << MCPWM_GEN0_B_DTEB_S)
#define MCPWM_GEN0_B_DTEB_V  0x00000003U
#define MCPWM_GEN0_B_DTEB_S  18
/** MCPWM_GEN0_B_DT0 : R/W; bitpos: [21:20]; default: 0;
 *  Configures action on PWM$n B triggered by event_t0 when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN0_B_DT0    0x00000003U
#define MCPWM_GEN0_B_DT0_M  (MCPWM_GEN0_B_DT0_V << MCPWM_GEN0_B_DT0_S)
#define MCPWM_GEN0_B_DT0_V  0x00000003U
#define MCPWM_GEN0_B_DT0_S  20
/** MCPWM_GEN0_B_DT1 : R/W; bitpos: [23:22]; default: 0;
 *  Configures action on PWM$n B triggered by event_t1 when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN0_B_DT1    0x00000003U
#define MCPWM_GEN0_B_DT1_M  (MCPWM_GEN0_B_DT1_V << MCPWM_GEN0_B_DT1_S)
#define MCPWM_GEN0_B_DT1_V  0x00000003U
#define MCPWM_GEN0_B_DT1_S  22

/** MCPWM_DT0_CFG_REG register
 *  Dead time configuration register
 */
#define MCPWM_DT0_CFG_REG (DR_REG_MCPWM_BASE + 0x58)
/** MCPWM_DB0_FED_UPMETHOD : R/W; bitpos: [3:0]; default: 0;
 *  Configures update method for FED (Falling edge delay) active register.\\0:
 *  Immediate\\Bit0 is set to 1: TEZ\\Bit1 is set to 1: TEP\\Bit2 is set to 1:
 *  Sync\\Bit3 is set to 1: Disable the update
 */
#define MCPWM_DB0_FED_UPMETHOD    0x0000000FU
#define MCPWM_DB0_FED_UPMETHOD_M  (MCPWM_DB0_FED_UPMETHOD_V << MCPWM_DB0_FED_UPMETHOD_S)
#define MCPWM_DB0_FED_UPMETHOD_V  0x0000000FU
#define MCPWM_DB0_FED_UPMETHOD_S  0
/** MCPWM_DB0_RED_UPMETHOD : R/W; bitpos: [7:4]; default: 0;
 *  Configures update method for RED (rising edge delay) active register.\\0:
 *  Immediate\\Bit0 is set to 1: TEZ\\Bit1 is set to 1: TEP\\Bit2 is set to 1:
 *  Sync\\Bit3 is set to 1: Disable the update
 */
#define MCPWM_DB0_RED_UPMETHOD    0x0000000FU
#define MCPWM_DB0_RED_UPMETHOD_M  (MCPWM_DB0_RED_UPMETHOD_V << MCPWM_DB0_RED_UPMETHOD_S)
#define MCPWM_DB0_RED_UPMETHOD_V  0x0000000FU
#define MCPWM_DB0_RED_UPMETHOD_S  4
/** MCPWM_DB0_DEB_MODE : R/W; bitpos: [8]; default: 0;
 *  Configures S8 in table, dual-edge B mode.\\0: fed/red take effect on different path
 *  separately\\1: fed/red take effect on B path, A out is in bypass or dulpB mode
 */
#define MCPWM_DB0_DEB_MODE    (BIT(8))
#define MCPWM_DB0_DEB_MODE_M  (MCPWM_DB0_DEB_MODE_V << MCPWM_DB0_DEB_MODE_S)
#define MCPWM_DB0_DEB_MODE_V  0x00000001U
#define MCPWM_DB0_DEB_MODE_S  8
/** MCPWM_DB0_A_OUTSWAP : R/W; bitpos: [9]; default: 0;
 *  Configures S6 in table.
 */
#define MCPWM_DB0_A_OUTSWAP    (BIT(9))
#define MCPWM_DB0_A_OUTSWAP_M  (MCPWM_DB0_A_OUTSWAP_V << MCPWM_DB0_A_OUTSWAP_S)
#define MCPWM_DB0_A_OUTSWAP_V  0x00000001U
#define MCPWM_DB0_A_OUTSWAP_S  9
/** MCPWM_DB0_B_OUTSWAP : R/W; bitpos: [10]; default: 0;
 *  Configures S7 in table.
 */
#define MCPWM_DB0_B_OUTSWAP    (BIT(10))
#define MCPWM_DB0_B_OUTSWAP_M  (MCPWM_DB0_B_OUTSWAP_V << MCPWM_DB0_B_OUTSWAP_S)
#define MCPWM_DB0_B_OUTSWAP_V  0x00000001U
#define MCPWM_DB0_B_OUTSWAP_S  10
/** MCPWM_DB0_RED_INSEL : R/W; bitpos: [11]; default: 0;
 *  Configures S4 in table.
 */
#define MCPWM_DB0_RED_INSEL    (BIT(11))
#define MCPWM_DB0_RED_INSEL_M  (MCPWM_DB0_RED_INSEL_V << MCPWM_DB0_RED_INSEL_S)
#define MCPWM_DB0_RED_INSEL_V  0x00000001U
#define MCPWM_DB0_RED_INSEL_S  11
/** MCPWM_DB0_FED_INSEL : R/W; bitpos: [12]; default: 0;
 *  Configures S5 in table.
 */
#define MCPWM_DB0_FED_INSEL    (BIT(12))
#define MCPWM_DB0_FED_INSEL_M  (MCPWM_DB0_FED_INSEL_V << MCPWM_DB0_FED_INSEL_S)
#define MCPWM_DB0_FED_INSEL_V  0x00000001U
#define MCPWM_DB0_FED_INSEL_S  12
/** MCPWM_DB0_RED_OUTINVERT : R/W; bitpos: [13]; default: 0;
 *  Configures S2 in table.
 */
#define MCPWM_DB0_RED_OUTINVERT    (BIT(13))
#define MCPWM_DB0_RED_OUTINVERT_M  (MCPWM_DB0_RED_OUTINVERT_V << MCPWM_DB0_RED_OUTINVERT_S)
#define MCPWM_DB0_RED_OUTINVERT_V  0x00000001U
#define MCPWM_DB0_RED_OUTINVERT_S  13
/** MCPWM_DB0_FED_OUTINVERT : R/W; bitpos: [14]; default: 0;
 *  Configures S3 in table.
 */
#define MCPWM_DB0_FED_OUTINVERT    (BIT(14))
#define MCPWM_DB0_FED_OUTINVERT_M  (MCPWM_DB0_FED_OUTINVERT_V << MCPWM_DB0_FED_OUTINVERT_S)
#define MCPWM_DB0_FED_OUTINVERT_V  0x00000001U
#define MCPWM_DB0_FED_OUTINVERT_S  14
/** MCPWM_DB0_A_OUTBYPASS : R/W; bitpos: [15]; default: 1;
 *  Configures S1 in table.
 */
#define MCPWM_DB0_A_OUTBYPASS    (BIT(15))
#define MCPWM_DB0_A_OUTBYPASS_M  (MCPWM_DB0_A_OUTBYPASS_V << MCPWM_DB0_A_OUTBYPASS_S)
#define MCPWM_DB0_A_OUTBYPASS_V  0x00000001U
#define MCPWM_DB0_A_OUTBYPASS_S  15
/** MCPWM_DB0_B_OUTBYPASS : R/W; bitpos: [16]; default: 1;
 *  Configures S0 in table.
 */
#define MCPWM_DB0_B_OUTBYPASS    (BIT(16))
#define MCPWM_DB0_B_OUTBYPASS_M  (MCPWM_DB0_B_OUTBYPASS_V << MCPWM_DB0_B_OUTBYPASS_S)
#define MCPWM_DB0_B_OUTBYPASS_V  0x00000001U
#define MCPWM_DB0_B_OUTBYPASS_S  16
/** MCPWM_DB0_CLK_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures dead time generator $n clock selection.\\0: PWM_clk\\1: PT_clk
 */
#define MCPWM_DB0_CLK_SEL    (BIT(17))
#define MCPWM_DB0_CLK_SEL_M  (MCPWM_DB0_CLK_SEL_V << MCPWM_DB0_CLK_SEL_S)
#define MCPWM_DB0_CLK_SEL_V  0x00000001U
#define MCPWM_DB0_CLK_SEL_S  17

/** MCPWM_DT0_FED_CFG_REG register
 *  Falling edge delay (FED) shadow register
 */
#define MCPWM_DT0_FED_CFG_REG (DR_REG_MCPWM_BASE + 0x5c)
/** MCPWM_DB0_FED : R/W; bitpos: [15:0]; default: 0;
 *  Configures shadow register for FED.
 */
#define MCPWM_DB0_FED    0x0000FFFFU
#define MCPWM_DB0_FED_M  (MCPWM_DB0_FED_V << MCPWM_DB0_FED_S)
#define MCPWM_DB0_FED_V  0x0000FFFFU
#define MCPWM_DB0_FED_S  0

/** MCPWM_DT0_RED_CFG_REG register
 *  Rising edge delay (RED) shadow register
 */
#define MCPWM_DT0_RED_CFG_REG (DR_REG_MCPWM_BASE + 0x60)
/** MCPWM_DB0_RED : R/W; bitpos: [15:0]; default: 0;
 *  Configures shadow register for RED.
 */
#define MCPWM_DB0_RED    0x0000FFFFU
#define MCPWM_DB0_RED_M  (MCPWM_DB0_RED_V << MCPWM_DB0_RED_S)
#define MCPWM_DB0_RED_V  0x0000FFFFU
#define MCPWM_DB0_RED_S  0

/** MCPWM_CARRIER0_CFG_REG register
 *  Carrier$n configuration register
 */
#define MCPWM_CARRIER0_CFG_REG (DR_REG_MCPWM_BASE + 0x64)
/** MCPWM_CHOPPER0_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable carrier$n.\\0: Bypassed\\1: Enabled
 */
#define MCPWM_CHOPPER0_EN    (BIT(0))
#define MCPWM_CHOPPER0_EN_M  (MCPWM_CHOPPER0_EN_V << MCPWM_CHOPPER0_EN_S)
#define MCPWM_CHOPPER0_EN_V  0x00000001U
#define MCPWM_CHOPPER0_EN_S  0
/** MCPWM_CHOPPER0_PRESCALE : R/W; bitpos: [4:1]; default: 0;
 *  Configures the prescale value of PWM carrier$n clock (PC_clk), so that period of
 *  PC_clk = period of PWM_clk * (PWM_CARRIER$n_PRESCALE + 1)
 */
#define MCPWM_CHOPPER0_PRESCALE    0x0000000FU
#define MCPWM_CHOPPER0_PRESCALE_M  (MCPWM_CHOPPER0_PRESCALE_V << MCPWM_CHOPPER0_PRESCALE_S)
#define MCPWM_CHOPPER0_PRESCALE_V  0x0000000FU
#define MCPWM_CHOPPER0_PRESCALE_S  1
/** MCPWM_CHOPPER0_DUTY : R/W; bitpos: [7:5]; default: 0;
 *  Configures carrier duty. Duty = PWM_CARRIER$n_DUTY / 8
 */
#define MCPWM_CHOPPER0_DUTY    0x00000007U
#define MCPWM_CHOPPER0_DUTY_M  (MCPWM_CHOPPER0_DUTY_V << MCPWM_CHOPPER0_DUTY_S)
#define MCPWM_CHOPPER0_DUTY_V  0x00000007U
#define MCPWM_CHOPPER0_DUTY_S  5
/** MCPWM_CHOPPER0_OSHTWTH : R/W; bitpos: [11:8]; default: 0;
 *  Configures width of the first pulse. Measurement unit: Periods of the carrier.
 */
#define MCPWM_CHOPPER0_OSHTWTH    0x0000000FU
#define MCPWM_CHOPPER0_OSHTWTH_M  (MCPWM_CHOPPER0_OSHTWTH_V << MCPWM_CHOPPER0_OSHTWTH_S)
#define MCPWM_CHOPPER0_OSHTWTH_V  0x0000000FU
#define MCPWM_CHOPPER0_OSHTWTH_S  8
/** MCPWM_CHOPPER0_OUT_INVERT : R/W; bitpos: [12]; default: 0;
 *  Configures whether or not to invert the output of PWM$n A and PWM$n B for this
 *  submodule.\\0: Normal\\1: Invert
 */
#define MCPWM_CHOPPER0_OUT_INVERT    (BIT(12))
#define MCPWM_CHOPPER0_OUT_INVERT_M  (MCPWM_CHOPPER0_OUT_INVERT_V << MCPWM_CHOPPER0_OUT_INVERT_S)
#define MCPWM_CHOPPER0_OUT_INVERT_V  0x00000001U
#define MCPWM_CHOPPER0_OUT_INVERT_S  12
/** MCPWM_CHOPPER0_IN_INVERT : R/W; bitpos: [13]; default: 0;
 *  Configures whether or not to invert the input of PWM$n A and PWM$n B for this
 *  submodule.\\0: Normal\\1: Invert
 */
#define MCPWM_CHOPPER0_IN_INVERT    (BIT(13))
#define MCPWM_CHOPPER0_IN_INVERT_M  (MCPWM_CHOPPER0_IN_INVERT_V << MCPWM_CHOPPER0_IN_INVERT_S)
#define MCPWM_CHOPPER0_IN_INVERT_V  0x00000001U
#define MCPWM_CHOPPER0_IN_INVERT_S  13

/** MCPWM_FH0_CFG0_REG register
 *  PWM$n A and PWM$n B trip events actions configuration register
 */
#define MCPWM_FH0_CFG0_REG (DR_REG_MCPWM_BASE + 0x68)
/** MCPWM_TZ0_SW_CBC : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable software force cycle-by-cycle mode action.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_TZ0_SW_CBC    (BIT(0))
#define MCPWM_TZ0_SW_CBC_M  (MCPWM_TZ0_SW_CBC_V << MCPWM_TZ0_SW_CBC_S)
#define MCPWM_TZ0_SW_CBC_V  0x00000001U
#define MCPWM_TZ0_SW_CBC_S  0
/** MCPWM_TZ0_F2_CBC : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not event_f2 will trigger cycle-by-cycle mode action.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_TZ0_F2_CBC    (BIT(1))
#define MCPWM_TZ0_F2_CBC_M  (MCPWM_TZ0_F2_CBC_V << MCPWM_TZ0_F2_CBC_S)
#define MCPWM_TZ0_F2_CBC_V  0x00000001U
#define MCPWM_TZ0_F2_CBC_S  1
/** MCPWM_TZ0_F1_CBC : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not event_f1 will trigger cycle-by-cycle mode action.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_TZ0_F1_CBC    (BIT(2))
#define MCPWM_TZ0_F1_CBC_M  (MCPWM_TZ0_F1_CBC_V << MCPWM_TZ0_F1_CBC_S)
#define MCPWM_TZ0_F1_CBC_V  0x00000001U
#define MCPWM_TZ0_F1_CBC_S  2
/** MCPWM_TZ0_F0_CBC : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not event_f0 will trigger cycle-by-cycle mode action.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_TZ0_F0_CBC    (BIT(3))
#define MCPWM_TZ0_F0_CBC_M  (MCPWM_TZ0_F0_CBC_V << MCPWM_TZ0_F0_CBC_S)
#define MCPWM_TZ0_F0_CBC_V  0x00000001U
#define MCPWM_TZ0_F0_CBC_S  3
/** MCPWM_TZ0_SW_OST : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable software force one-shot mode action.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_TZ0_SW_OST    (BIT(4))
#define MCPWM_TZ0_SW_OST_M  (MCPWM_TZ0_SW_OST_V << MCPWM_TZ0_SW_OST_S)
#define MCPWM_TZ0_SW_OST_V  0x00000001U
#define MCPWM_TZ0_SW_OST_S  4
/** MCPWM_TZ0_F2_OST : R/W; bitpos: [5]; default: 0;
 *  Configures whether or not event_f2 will trigger one-shot mode action.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_TZ0_F2_OST    (BIT(5))
#define MCPWM_TZ0_F2_OST_M  (MCPWM_TZ0_F2_OST_V << MCPWM_TZ0_F2_OST_S)
#define MCPWM_TZ0_F2_OST_V  0x00000001U
#define MCPWM_TZ0_F2_OST_S  5
/** MCPWM_TZ0_F1_OST : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not event_f1 will trigger one-shot mode action.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_TZ0_F1_OST    (BIT(6))
#define MCPWM_TZ0_F1_OST_M  (MCPWM_TZ0_F1_OST_V << MCPWM_TZ0_F1_OST_S)
#define MCPWM_TZ0_F1_OST_V  0x00000001U
#define MCPWM_TZ0_F1_OST_S  6
/** MCPWM_TZ0_F0_OST : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not event_f0 will trigger one-shot mode action.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_TZ0_F0_OST    (BIT(7))
#define MCPWM_TZ0_F0_OST_M  (MCPWM_TZ0_F0_OST_V << MCPWM_TZ0_F0_OST_S)
#define MCPWM_TZ0_F0_OST_V  0x00000001U
#define MCPWM_TZ0_F0_OST_S  7
/** MCPWM_TZ0_A_CBC_D : R/W; bitpos: [9:8]; default: 0;
 *  Configures cycle-by-cycle mode action on PWM$n A when fault event occurs and timer
 *  is decreasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
 */
#define MCPWM_TZ0_A_CBC_D    0x00000003U
#define MCPWM_TZ0_A_CBC_D_M  (MCPWM_TZ0_A_CBC_D_V << MCPWM_TZ0_A_CBC_D_S)
#define MCPWM_TZ0_A_CBC_D_V  0x00000003U
#define MCPWM_TZ0_A_CBC_D_S  8
/** MCPWM_TZ0_A_CBC_U : R/W; bitpos: [11:10]; default: 0;
 *  Configures cycle-by-cycle mode action on PWM$n A when fault event occurs and timer
 *  is increasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
 */
#define MCPWM_TZ0_A_CBC_U    0x00000003U
#define MCPWM_TZ0_A_CBC_U_M  (MCPWM_TZ0_A_CBC_U_V << MCPWM_TZ0_A_CBC_U_S)
#define MCPWM_TZ0_A_CBC_U_V  0x00000003U
#define MCPWM_TZ0_A_CBC_U_S  10
/** MCPWM_TZ0_A_OST_D : R/W; bitpos: [13:12]; default: 0;
 *  Configures one-shot mode action on PWM$n A when fault event occurs and timer is
 *  decreasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
 */
#define MCPWM_TZ0_A_OST_D    0x00000003U
#define MCPWM_TZ0_A_OST_D_M  (MCPWM_TZ0_A_OST_D_V << MCPWM_TZ0_A_OST_D_S)
#define MCPWM_TZ0_A_OST_D_V  0x00000003U
#define MCPWM_TZ0_A_OST_D_S  12
/** MCPWM_TZ0_A_OST_U : R/W; bitpos: [15:14]; default: 0;
 *  Configures one-shot mode action on PWM$n A when fault event occurs and timer is
 *  increasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
 */
#define MCPWM_TZ0_A_OST_U    0x00000003U
#define MCPWM_TZ0_A_OST_U_M  (MCPWM_TZ0_A_OST_U_V << MCPWM_TZ0_A_OST_U_S)
#define MCPWM_TZ0_A_OST_U_V  0x00000003U
#define MCPWM_TZ0_A_OST_U_S  14
/** MCPWM_TZ0_B_CBC_D : R/W; bitpos: [17:16]; default: 0;
 *  Configures cycle-by-cycle mode action on PWM$n B when fault event occurs and timer
 *  is decreasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
 */
#define MCPWM_TZ0_B_CBC_D    0x00000003U
#define MCPWM_TZ0_B_CBC_D_M  (MCPWM_TZ0_B_CBC_D_V << MCPWM_TZ0_B_CBC_D_S)
#define MCPWM_TZ0_B_CBC_D_V  0x00000003U
#define MCPWM_TZ0_B_CBC_D_S  16
/** MCPWM_TZ0_B_CBC_U : R/W; bitpos: [19:18]; default: 0;
 *  Configures cycle-by-cycle mode action on PWM$n B when fault event occurs and timer
 *  is increasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
 */
#define MCPWM_TZ0_B_CBC_U    0x00000003U
#define MCPWM_TZ0_B_CBC_U_M  (MCPWM_TZ0_B_CBC_U_V << MCPWM_TZ0_B_CBC_U_S)
#define MCPWM_TZ0_B_CBC_U_V  0x00000003U
#define MCPWM_TZ0_B_CBC_U_S  18
/** MCPWM_TZ0_B_OST_D : R/W; bitpos: [21:20]; default: 0;
 *  Configures one-shot mode action on PWM$n B when fault event occurs and timer is
 *  decreasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
 */
#define MCPWM_TZ0_B_OST_D    0x00000003U
#define MCPWM_TZ0_B_OST_D_M  (MCPWM_TZ0_B_OST_D_V << MCPWM_TZ0_B_OST_D_S)
#define MCPWM_TZ0_B_OST_D_V  0x00000003U
#define MCPWM_TZ0_B_OST_D_S  20
/** MCPWM_TZ0_B_OST_U : R/W; bitpos: [23:22]; default: 0;
 *  Configures one-shot mode action on PWM$n B when fault event occurs and timer is
 *  increasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
 */
#define MCPWM_TZ0_B_OST_U    0x00000003U
#define MCPWM_TZ0_B_OST_U_M  (MCPWM_TZ0_B_OST_U_V << MCPWM_TZ0_B_OST_U_S)
#define MCPWM_TZ0_B_OST_U_V  0x00000003U
#define MCPWM_TZ0_B_OST_U_S  22

/** MCPWM_FH0_CFG1_REG register
 *  Software triggers for fault handler actions configuration register
 */
#define MCPWM_FH0_CFG1_REG (DR_REG_MCPWM_BASE + 0x6c)
/** MCPWM_TZ0_CLR_OST : R/W; bitpos: [0]; default: 0;
 *  Configures the generation of software one-shot mode action clear. A toggle
 *  (software negate its value) triggers a clear for on going one-shot mode action.
 */
#define MCPWM_TZ0_CLR_OST    (BIT(0))
#define MCPWM_TZ0_CLR_OST_M  (MCPWM_TZ0_CLR_OST_V << MCPWM_TZ0_CLR_OST_S)
#define MCPWM_TZ0_CLR_OST_V  0x00000001U
#define MCPWM_TZ0_CLR_OST_S  0
/** MCPWM_TZ0_CBCPULSE : R/W; bitpos: [2:1]; default: 0;
 *  Configures the refresh moment selection of cycle-by-cycle mode action.\\0: Select
 *  nothing, will not refresh\\Bit0 is set to 1: TEZ\\Bit1 is set to 1: TEP
 */
#define MCPWM_TZ0_CBCPULSE    0x00000003U
#define MCPWM_TZ0_CBCPULSE_M  (MCPWM_TZ0_CBCPULSE_V << MCPWM_TZ0_CBCPULSE_S)
#define MCPWM_TZ0_CBCPULSE_V  0x00000003U
#define MCPWM_TZ0_CBCPULSE_S  1
/** MCPWM_TZ0_FORCE_CBC : R/W; bitpos: [3]; default: 0;
 *  Configures the generation of software cycle-by-cycle mode action. A toggle
 *  (software negate its value) triggers a cycle-by-cycle mode action.
 */
#define MCPWM_TZ0_FORCE_CBC    (BIT(3))
#define MCPWM_TZ0_FORCE_CBC_M  (MCPWM_TZ0_FORCE_CBC_V << MCPWM_TZ0_FORCE_CBC_S)
#define MCPWM_TZ0_FORCE_CBC_V  0x00000001U
#define MCPWM_TZ0_FORCE_CBC_S  3
/** MCPWM_TZ0_FORCE_OST : R/W; bitpos: [4]; default: 0;
 *  Configures the generation of software one-shot mode action. A toggle (software
 *  negate its value) triggers a one-shot mode action.
 */
#define MCPWM_TZ0_FORCE_OST    (BIT(4))
#define MCPWM_TZ0_FORCE_OST_M  (MCPWM_TZ0_FORCE_OST_V << MCPWM_TZ0_FORCE_OST_S)
#define MCPWM_TZ0_FORCE_OST_V  0x00000001U
#define MCPWM_TZ0_FORCE_OST_S  4

/** MCPWM_FH0_STATUS_REG register
 *  Fault events status register
 */
#define MCPWM_FH0_STATUS_REG (DR_REG_MCPWM_BASE + 0x70)
/** MCPWM_TZ0_CBC_ON : RO; bitpos: [0]; default: 0;
 *  Represents whether or not an cycle-by-cycle mode action is on going.\\0:No
 *  action\\1: On going
 */
#define MCPWM_TZ0_CBC_ON    (BIT(0))
#define MCPWM_TZ0_CBC_ON_M  (MCPWM_TZ0_CBC_ON_V << MCPWM_TZ0_CBC_ON_S)
#define MCPWM_TZ0_CBC_ON_V  0x00000001U
#define MCPWM_TZ0_CBC_ON_S  0
/** MCPWM_TZ0_OST_ON : RO; bitpos: [1]; default: 0;
 *  Represents whether or not an one-shot mode action is on going.\\0:No action\\1: On
 *  going
 */
#define MCPWM_TZ0_OST_ON    (BIT(1))
#define MCPWM_TZ0_OST_ON_M  (MCPWM_TZ0_OST_ON_V << MCPWM_TZ0_OST_ON_S)
#define MCPWM_TZ0_OST_ON_V  0x00000001U
#define MCPWM_TZ0_OST_ON_S  1

/** MCPWM_GEN1_STMP_CFG_REG register
 *  Generator1 time stamp registers A and B transfer status and update method register
 */
#define MCPWM_GEN1_STMP_CFG_REG (DR_REG_MCPWM_BASE + 0x74)
/** MCPWM_CMPR0_A_UPMETHOD : R/W; bitpos: [3:0]; default: 0;
 *  Configures the update method for PWM generator 1 time stamp A's active
 *  register.\\0: Immediately\\Bit0 is set to 1: TEZ\\Bit1 is set to 1: TEP\\Bit2 is
 *  set to 1: Sync\\Bit3 is set to 1: Disable the update
 */
#define MCPWM_CMPR0_A_UPMETHOD    0x0000000FU
#define MCPWM_CMPR0_A_UPMETHOD_M  (MCPWM_CMPR0_A_UPMETHOD_V << MCPWM_CMPR0_A_UPMETHOD_S)
#define MCPWM_CMPR0_A_UPMETHOD_V  0x0000000FU
#define MCPWM_CMPR0_A_UPMETHOD_S  0
/** MCPWM_CMPR0_B_UPMETHOD : R/W; bitpos: [7:4]; default: 0;
 *  Configures the update method for PWM generator 1 time stamp B's active
 *  register.\\0: Immediately\\Bit0 is set to 1: TEZ\\Bit1 is set to 1: TEP\\Bit2 is
 *  set to 1: Sync\\Bit3 is set to 1: Disable the update
 */
#define MCPWM_CMPR0_B_UPMETHOD    0x0000000FU
#define MCPWM_CMPR0_B_UPMETHOD_M  (MCPWM_CMPR0_B_UPMETHOD_V << MCPWM_CMPR0_B_UPMETHOD_S)
#define MCPWM_CMPR0_B_UPMETHOD_V  0x0000000FU
#define MCPWM_CMPR0_B_UPMETHOD_S  4
/** MCPWM_CMPR0_A_SHDW_FULL : R/W/WTC/SC; bitpos: [8]; default: 0;
 *  Represents whether or not generator1 time stamp A's shadow reg is transferred.\\0:
 *  A's active reg has been updated with shadow register latest value.\\1: A's shadow
 *  reg is filled and waiting to be transferred to A's active reg
 */
#define MCPWM_CMPR0_A_SHDW_FULL    (BIT(8))
#define MCPWM_CMPR0_A_SHDW_FULL_M  (MCPWM_CMPR0_A_SHDW_FULL_V << MCPWM_CMPR0_A_SHDW_FULL_S)
#define MCPWM_CMPR0_A_SHDW_FULL_V  0x00000001U
#define MCPWM_CMPR0_A_SHDW_FULL_S  8
/** MCPWM_CMPR0_B_SHDW_FULL : R/W/WTC/SC; bitpos: [9]; default: 0;
 *  Represents whether or not generator1 time stamp B's shadow reg is transferred.\\0:
 *  B's active reg has been updated with shadow register latest value.\\1: B's shadow
 *  reg is filled and waiting to be transferred to B's active reg
 */
#define MCPWM_CMPR0_B_SHDW_FULL    (BIT(9))
#define MCPWM_CMPR0_B_SHDW_FULL_M  (MCPWM_CMPR0_B_SHDW_FULL_V << MCPWM_CMPR0_B_SHDW_FULL_S)
#define MCPWM_CMPR0_B_SHDW_FULL_V  0x00000001U
#define MCPWM_CMPR0_B_SHDW_FULL_S  9

/** MCPWM_GEN1_TSTMP_A_REG register
 *  Generator$n time stamp A's shadow register
 */
#define MCPWM_GEN1_TSTMP_A_REG (DR_REG_MCPWM_BASE + 0x78)
/** MCPWM_CMPR1_A : R/W; bitpos: [15:0]; default: 0;
 *  Configures the value of PWM generator $n time stamp A's shadow register.
 */
#define MCPWM_CMPR1_A    0x0000FFFFU
#define MCPWM_CMPR1_A_M  (MCPWM_CMPR1_A_V << MCPWM_CMPR1_A_S)
#define MCPWM_CMPR1_A_V  0x0000FFFFU
#define MCPWM_CMPR1_A_S  0

/** MCPWM_GEN1_TSTMP_B_REG register
 *  Generator$n time stamp B's shadow register
 */
#define MCPWM_GEN1_TSTMP_B_REG (DR_REG_MCPWM_BASE + 0x7c)
/** MCPWM_CMPR1_B : R/W; bitpos: [15:0]; default: 0;
 *  Configures the value of PWM generator $n time stamp B's shadow register.
 */
#define MCPWM_CMPR1_B    0x0000FFFFU
#define MCPWM_CMPR1_B_M  (MCPWM_CMPR1_B_V << MCPWM_CMPR1_B_S)
#define MCPWM_CMPR1_B_V  0x0000FFFFU
#define MCPWM_CMPR1_B_S  0

/** MCPWM_GEN1_CFG0_REG register
 *  Generator$n fault event T0 and T1 configuration register
 */
#define MCPWM_GEN1_CFG0_REG (DR_REG_MCPWM_BASE + 0x80)
/** MCPWM_GEN1_CFG_UPMETHOD : R/W; bitpos: [3:0]; default: 0;
 *  Configures update method for PWM generator $n's active register.\\0:
 *  Immediately\\Bit0 is set to 1: TEZ\\Bit1 is set to 1: TEP\\Bit2 is set to 1:
 *  Sync\\Bit3 is set to 1: Disable the update
 */
#define MCPWM_GEN1_CFG_UPMETHOD    0x0000000FU
#define MCPWM_GEN1_CFG_UPMETHOD_M  (MCPWM_GEN1_CFG_UPMETHOD_V << MCPWM_GEN1_CFG_UPMETHOD_S)
#define MCPWM_GEN1_CFG_UPMETHOD_V  0x0000000FU
#define MCPWM_GEN1_CFG_UPMETHOD_S  0
/** MCPWM_GEN1_T0_SEL : R/W; bitpos: [6:4]; default: 0;
 *  Configures source selection for PWM generator $n event_t0, take effect
 *  immediately.\\0: fault_event0\\1: fault_event1\\2: fault_event2\\3: sync_taken\\4:
 *  Invalid, Select nothing
 */
#define MCPWM_GEN1_T0_SEL    0x00000007U
#define MCPWM_GEN1_T0_SEL_M  (MCPWM_GEN1_T0_SEL_V << MCPWM_GEN1_T0_SEL_S)
#define MCPWM_GEN1_T0_SEL_V  0x00000007U
#define MCPWM_GEN1_T0_SEL_S  4
/** MCPWM_GEN1_T1_SEL : R/W; bitpos: [9:7]; default: 0;
 *  Configures source selection for PWM generator $n event_t1, take effect
 *  immediately.\\0: fault_event0\\1: fault_event1\\2: fault_event2\\3: sync_taken\\4:
 *  Invalid, Select nothing
 */
#define MCPWM_GEN1_T1_SEL    0x00000007U
#define MCPWM_GEN1_T1_SEL_M  (MCPWM_GEN1_T1_SEL_V << MCPWM_GEN1_T1_SEL_S)
#define MCPWM_GEN1_T1_SEL_V  0x00000007U
#define MCPWM_GEN1_T1_SEL_S  7

/** MCPWM_GEN1_FORCE_REG register
 *  Generator$n output signal force mode register.
 */
#define MCPWM_GEN1_FORCE_REG (DR_REG_MCPWM_BASE + 0x84)
/** MCPWM_GEN1_CNTUFORCE_UPMETHOD : R/W; bitpos: [5:0]; default: 32;
 *  Configures update method for continuous software force of PWM generator$n.\\0:
 *  Immediately\\Bit0 is set to 1: TEZ\\Bit1 is set to 1: TEP\\Bit2 is set to 1:
 *  TEA\\Bit3 is set to 1: TEB\\Bit4 is set to 1: Sync\\Bit5 is set to 1: Disable
 *  update. TEA/B here and below means an event generated when the timer's value equals
 *  to that of register A/B.
 */
#define MCPWM_GEN1_CNTUFORCE_UPMETHOD    0x0000003FU
#define MCPWM_GEN1_CNTUFORCE_UPMETHOD_M  (MCPWM_GEN1_CNTUFORCE_UPMETHOD_V << MCPWM_GEN1_CNTUFORCE_UPMETHOD_S)
#define MCPWM_GEN1_CNTUFORCE_UPMETHOD_V  0x0000003FU
#define MCPWM_GEN1_CNTUFORCE_UPMETHOD_S  0
/** MCPWM_GEN1_A_CNTUFORCE_MODE : R/W; bitpos: [7:6]; default: 0;
 *  Configures continuous software force mode for PWM$n A.\\0: Disabled\\1: Low\\2:
 *  High\\3: Disabled
 */
#define MCPWM_GEN1_A_CNTUFORCE_MODE    0x00000003U
#define MCPWM_GEN1_A_CNTUFORCE_MODE_M  (MCPWM_GEN1_A_CNTUFORCE_MODE_V << MCPWM_GEN1_A_CNTUFORCE_MODE_S)
#define MCPWM_GEN1_A_CNTUFORCE_MODE_V  0x00000003U
#define MCPWM_GEN1_A_CNTUFORCE_MODE_S  6
/** MCPWM_GEN1_B_CNTUFORCE_MODE : R/W; bitpos: [9:8]; default: 0;
 *  Configures continuous software force mode for PWM$n B.\\0: Disabled\\1: Low\\2:
 *  High\\3: Disabled
 */
#define MCPWM_GEN1_B_CNTUFORCE_MODE    0x00000003U
#define MCPWM_GEN1_B_CNTUFORCE_MODE_M  (MCPWM_GEN1_B_CNTUFORCE_MODE_V << MCPWM_GEN1_B_CNTUFORCE_MODE_S)
#define MCPWM_GEN1_B_CNTUFORCE_MODE_V  0x00000003U
#define MCPWM_GEN1_B_CNTUFORCE_MODE_S  8
/** MCPWM_GEN1_A_NCIFORCE : R/W; bitpos: [10]; default: 0;
 *  Configures the generation of non-continuous immediate software-force event for
 *  PWM$n A, a toggle will trigger a force event.
 */
#define MCPWM_GEN1_A_NCIFORCE    (BIT(10))
#define MCPWM_GEN1_A_NCIFORCE_M  (MCPWM_GEN1_A_NCIFORCE_V << MCPWM_GEN1_A_NCIFORCE_S)
#define MCPWM_GEN1_A_NCIFORCE_V  0x00000001U
#define MCPWM_GEN1_A_NCIFORCE_S  10
/** MCPWM_GEN1_A_NCIFORCE_MODE : R/W; bitpos: [12:11]; default: 0;
 *  Configures non-continuous immediate software force mode for PWM$n A.\\0:
 *  Disabled\\1: Low\\2: High\\3: Disabled
 */
#define MCPWM_GEN1_A_NCIFORCE_MODE    0x00000003U
#define MCPWM_GEN1_A_NCIFORCE_MODE_M  (MCPWM_GEN1_A_NCIFORCE_MODE_V << MCPWM_GEN1_A_NCIFORCE_MODE_S)
#define MCPWM_GEN1_A_NCIFORCE_MODE_V  0x00000003U
#define MCPWM_GEN1_A_NCIFORCE_MODE_S  11
/** MCPWM_GEN1_B_NCIFORCE : R/W; bitpos: [13]; default: 0;
 *  Configures the generation of non-continuous immediate software-force event for
 *  PWM$n B, a toggle will trigger a force event.
 */
#define MCPWM_GEN1_B_NCIFORCE    (BIT(13))
#define MCPWM_GEN1_B_NCIFORCE_M  (MCPWM_GEN1_B_NCIFORCE_V << MCPWM_GEN1_B_NCIFORCE_S)
#define MCPWM_GEN1_B_NCIFORCE_V  0x00000001U
#define MCPWM_GEN1_B_NCIFORCE_S  13
/** MCPWM_GEN1_B_NCIFORCE_MODE : R/W; bitpos: [15:14]; default: 0;
 *  Configures non-continuous immediate software force mode for PWM$n B.\\0:
 *  Disabled\\1: Low\\2: High\\3: Disabled
 */
#define MCPWM_GEN1_B_NCIFORCE_MODE    0x00000003U
#define MCPWM_GEN1_B_NCIFORCE_MODE_M  (MCPWM_GEN1_B_NCIFORCE_MODE_V << MCPWM_GEN1_B_NCIFORCE_MODE_S)
#define MCPWM_GEN1_B_NCIFORCE_MODE_V  0x00000003U
#define MCPWM_GEN1_B_NCIFORCE_MODE_S  14

/** MCPWM_GEN1_A_REG register
 *  PWM$n output signal A actions configuration register
 */
#define MCPWM_GEN1_A_REG (DR_REG_MCPWM_BASE + 0x88)
/** MCPWM_GEN1_A_UTEZ : R/W; bitpos: [1:0]; default: 0;
 *  Configures action on PWM$n A triggered by event TEZ when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN1_A_UTEZ    0x00000003U
#define MCPWM_GEN1_A_UTEZ_M  (MCPWM_GEN1_A_UTEZ_V << MCPWM_GEN1_A_UTEZ_S)
#define MCPWM_GEN1_A_UTEZ_V  0x00000003U
#define MCPWM_GEN1_A_UTEZ_S  0
/** MCPWM_GEN1_A_UTEP : R/W; bitpos: [3:2]; default: 0;
 *  Configures action on PWM$n A triggered by event TEP when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN1_A_UTEP    0x00000003U
#define MCPWM_GEN1_A_UTEP_M  (MCPWM_GEN1_A_UTEP_V << MCPWM_GEN1_A_UTEP_S)
#define MCPWM_GEN1_A_UTEP_V  0x00000003U
#define MCPWM_GEN1_A_UTEP_S  2
/** MCPWM_GEN1_A_UTEA : R/W; bitpos: [5:4]; default: 0;
 *  Configures action on PWM$n A triggered by event TEA when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN1_A_UTEA    0x00000003U
#define MCPWM_GEN1_A_UTEA_M  (MCPWM_GEN1_A_UTEA_V << MCPWM_GEN1_A_UTEA_S)
#define MCPWM_GEN1_A_UTEA_V  0x00000003U
#define MCPWM_GEN1_A_UTEA_S  4
/** MCPWM_GEN1_A_UTEB : R/W; bitpos: [7:6]; default: 0;
 *  Configures action on PWM$n A triggered by event TEB when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN1_A_UTEB    0x00000003U
#define MCPWM_GEN1_A_UTEB_M  (MCPWM_GEN1_A_UTEB_V << MCPWM_GEN1_A_UTEB_S)
#define MCPWM_GEN1_A_UTEB_V  0x00000003U
#define MCPWM_GEN1_A_UTEB_S  6
/** MCPWM_GEN1_A_UT0 : R/W; bitpos: [9:8]; default: 0;
 *  Configures action on PWM$n A triggered by event_t0 when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN1_A_UT0    0x00000003U
#define MCPWM_GEN1_A_UT0_M  (MCPWM_GEN1_A_UT0_V << MCPWM_GEN1_A_UT0_S)
#define MCPWM_GEN1_A_UT0_V  0x00000003U
#define MCPWM_GEN1_A_UT0_S  8
/** MCPWM_GEN1_A_UT1 : R/W; bitpos: [11:10]; default: 0;
 *  Configures action on PWM$n A triggered by event_t1 when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN1_A_UT1    0x00000003U
#define MCPWM_GEN1_A_UT1_M  (MCPWM_GEN1_A_UT1_V << MCPWM_GEN1_A_UT1_S)
#define MCPWM_GEN1_A_UT1_V  0x00000003U
#define MCPWM_GEN1_A_UT1_S  10
/** MCPWM_GEN1_A_DTEZ : R/W; bitpos: [13:12]; default: 0;
 *  Configures action on PWM$n A triggered by event TEZ when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN1_A_DTEZ    0x00000003U
#define MCPWM_GEN1_A_DTEZ_M  (MCPWM_GEN1_A_DTEZ_V << MCPWM_GEN1_A_DTEZ_S)
#define MCPWM_GEN1_A_DTEZ_V  0x00000003U
#define MCPWM_GEN1_A_DTEZ_S  12
/** MCPWM_GEN1_A_DTEP : R/W; bitpos: [15:14]; default: 0;
 *  Configures action on PWM$n A triggered by event TEP when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN1_A_DTEP    0x00000003U
#define MCPWM_GEN1_A_DTEP_M  (MCPWM_GEN1_A_DTEP_V << MCPWM_GEN1_A_DTEP_S)
#define MCPWM_GEN1_A_DTEP_V  0x00000003U
#define MCPWM_GEN1_A_DTEP_S  14
/** MCPWM_GEN1_A_DTEA : R/W; bitpos: [17:16]; default: 0;
 *  Configures action on PWM$n A triggered by event TEA when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN1_A_DTEA    0x00000003U
#define MCPWM_GEN1_A_DTEA_M  (MCPWM_GEN1_A_DTEA_V << MCPWM_GEN1_A_DTEA_S)
#define MCPWM_GEN1_A_DTEA_V  0x00000003U
#define MCPWM_GEN1_A_DTEA_S  16
/** MCPWM_GEN1_A_DTEB : R/W; bitpos: [19:18]; default: 0;
 *  Configures action on PWM$n A triggered by event TEB when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN1_A_DTEB    0x00000003U
#define MCPWM_GEN1_A_DTEB_M  (MCPWM_GEN1_A_DTEB_V << MCPWM_GEN1_A_DTEB_S)
#define MCPWM_GEN1_A_DTEB_V  0x00000003U
#define MCPWM_GEN1_A_DTEB_S  18
/** MCPWM_GEN1_A_DT0 : R/W; bitpos: [21:20]; default: 0;
 *  Configures action on PWM$n A triggered by event_t0 when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN1_A_DT0    0x00000003U
#define MCPWM_GEN1_A_DT0_M  (MCPWM_GEN1_A_DT0_V << MCPWM_GEN1_A_DT0_S)
#define MCPWM_GEN1_A_DT0_V  0x00000003U
#define MCPWM_GEN1_A_DT0_S  20
/** MCPWM_GEN1_A_DT1 : R/W; bitpos: [23:22]; default: 0;
 *  Configures action on PWM$n A triggered by event_t1 when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN1_A_DT1    0x00000003U
#define MCPWM_GEN1_A_DT1_M  (MCPWM_GEN1_A_DT1_V << MCPWM_GEN1_A_DT1_S)
#define MCPWM_GEN1_A_DT1_V  0x00000003U
#define MCPWM_GEN1_A_DT1_S  22

/** MCPWM_GEN1_B_REG register
 *  PWM$n output signal B actions configuration register
 */
#define MCPWM_GEN1_B_REG (DR_REG_MCPWM_BASE + 0x8c)
/** MCPWM_GEN1_B_UTEZ : R/W; bitpos: [1:0]; default: 0;
 *  Configures action on PWM$n B triggered by event TEZ when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN1_B_UTEZ    0x00000003U
#define MCPWM_GEN1_B_UTEZ_M  (MCPWM_GEN1_B_UTEZ_V << MCPWM_GEN1_B_UTEZ_S)
#define MCPWM_GEN1_B_UTEZ_V  0x00000003U
#define MCPWM_GEN1_B_UTEZ_S  0
/** MCPWM_GEN1_B_UTEP : R/W; bitpos: [3:2]; default: 0;
 *  Configures action on PWM$n B triggered by event TEP when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN1_B_UTEP    0x00000003U
#define MCPWM_GEN1_B_UTEP_M  (MCPWM_GEN1_B_UTEP_V << MCPWM_GEN1_B_UTEP_S)
#define MCPWM_GEN1_B_UTEP_V  0x00000003U
#define MCPWM_GEN1_B_UTEP_S  2
/** MCPWM_GEN1_B_UTEA : R/W; bitpos: [5:4]; default: 0;
 *  Configures action on PWM$n B triggered by event TEA when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN1_B_UTEA    0x00000003U
#define MCPWM_GEN1_B_UTEA_M  (MCPWM_GEN1_B_UTEA_V << MCPWM_GEN1_B_UTEA_S)
#define MCPWM_GEN1_B_UTEA_V  0x00000003U
#define MCPWM_GEN1_B_UTEA_S  4
/** MCPWM_GEN1_B_UTEB : R/W; bitpos: [7:6]; default: 0;
 *  Configures action on PWM$n B triggered by event TEB when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN1_B_UTEB    0x00000003U
#define MCPWM_GEN1_B_UTEB_M  (MCPWM_GEN1_B_UTEB_V << MCPWM_GEN1_B_UTEB_S)
#define MCPWM_GEN1_B_UTEB_V  0x00000003U
#define MCPWM_GEN1_B_UTEB_S  6
/** MCPWM_GEN1_B_UT0 : R/W; bitpos: [9:8]; default: 0;
 *  Configures action on PWM$n B triggered by event_t0 when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN1_B_UT0    0x00000003U
#define MCPWM_GEN1_B_UT0_M  (MCPWM_GEN1_B_UT0_V << MCPWM_GEN1_B_UT0_S)
#define MCPWM_GEN1_B_UT0_V  0x00000003U
#define MCPWM_GEN1_B_UT0_S  8
/** MCPWM_GEN1_B_UT1 : R/W; bitpos: [11:10]; default: 0;
 *  Configures action on PWM$n B triggered by event_t1 when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN1_B_UT1    0x00000003U
#define MCPWM_GEN1_B_UT1_M  (MCPWM_GEN1_B_UT1_V << MCPWM_GEN1_B_UT1_S)
#define MCPWM_GEN1_B_UT1_V  0x00000003U
#define MCPWM_GEN1_B_UT1_S  10
/** MCPWM_GEN1_B_DTEZ : R/W; bitpos: [13:12]; default: 0;
 *  Configures action on PWM$n B triggered by event TEZ when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN1_B_DTEZ    0x00000003U
#define MCPWM_GEN1_B_DTEZ_M  (MCPWM_GEN1_B_DTEZ_V << MCPWM_GEN1_B_DTEZ_S)
#define MCPWM_GEN1_B_DTEZ_V  0x00000003U
#define MCPWM_GEN1_B_DTEZ_S  12
/** MCPWM_GEN1_B_DTEP : R/W; bitpos: [15:14]; default: 0;
 *  Configures action on PWM$n B triggered by event TEP when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN1_B_DTEP    0x00000003U
#define MCPWM_GEN1_B_DTEP_M  (MCPWM_GEN1_B_DTEP_V << MCPWM_GEN1_B_DTEP_S)
#define MCPWM_GEN1_B_DTEP_V  0x00000003U
#define MCPWM_GEN1_B_DTEP_S  14
/** MCPWM_GEN1_B_DTEA : R/W; bitpos: [17:16]; default: 0;
 *  Configures action on PWM$n B triggered by event TEA when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN1_B_DTEA    0x00000003U
#define MCPWM_GEN1_B_DTEA_M  (MCPWM_GEN1_B_DTEA_V << MCPWM_GEN1_B_DTEA_S)
#define MCPWM_GEN1_B_DTEA_V  0x00000003U
#define MCPWM_GEN1_B_DTEA_S  16
/** MCPWM_GEN1_B_DTEB : R/W; bitpos: [19:18]; default: 0;
 *  Configures action on PWM$n B triggered by event TEB when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN1_B_DTEB    0x00000003U
#define MCPWM_GEN1_B_DTEB_M  (MCPWM_GEN1_B_DTEB_V << MCPWM_GEN1_B_DTEB_S)
#define MCPWM_GEN1_B_DTEB_V  0x00000003U
#define MCPWM_GEN1_B_DTEB_S  18
/** MCPWM_GEN1_B_DT0 : R/W; bitpos: [21:20]; default: 0;
 *  Configures action on PWM$n B triggered by event_t0 when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN1_B_DT0    0x00000003U
#define MCPWM_GEN1_B_DT0_M  (MCPWM_GEN1_B_DT0_V << MCPWM_GEN1_B_DT0_S)
#define MCPWM_GEN1_B_DT0_V  0x00000003U
#define MCPWM_GEN1_B_DT0_S  20
/** MCPWM_GEN1_B_DT1 : R/W; bitpos: [23:22]; default: 0;
 *  Configures action on PWM$n B triggered by event_t1 when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN1_B_DT1    0x00000003U
#define MCPWM_GEN1_B_DT1_M  (MCPWM_GEN1_B_DT1_V << MCPWM_GEN1_B_DT1_S)
#define MCPWM_GEN1_B_DT1_V  0x00000003U
#define MCPWM_GEN1_B_DT1_S  22

/** MCPWM_DT1_CFG_REG register
 *  Dead time configuration register
 */
#define MCPWM_DT1_CFG_REG (DR_REG_MCPWM_BASE + 0x90)
/** MCPWM_DB1_FED_UPMETHOD : R/W; bitpos: [3:0]; default: 0;
 *  Configures update method for FED (Falling edge delay) active register.\\0:
 *  Immediate\\Bit0 is set to 1: TEZ\\Bit1 is set to 1: TEP\\Bit2 is set to 1:
 *  Sync\\Bit3 is set to 1: Disable the update
 */
#define MCPWM_DB1_FED_UPMETHOD    0x0000000FU
#define MCPWM_DB1_FED_UPMETHOD_M  (MCPWM_DB1_FED_UPMETHOD_V << MCPWM_DB1_FED_UPMETHOD_S)
#define MCPWM_DB1_FED_UPMETHOD_V  0x0000000FU
#define MCPWM_DB1_FED_UPMETHOD_S  0
/** MCPWM_DB1_RED_UPMETHOD : R/W; bitpos: [7:4]; default: 0;
 *  Configures update method for RED (rising edge delay) active register.\\0:
 *  Immediate\\Bit0 is set to 1: TEZ\\Bit1 is set to 1: TEP\\Bit2 is set to 1:
 *  Sync\\Bit3 is set to 1: Disable the update
 */
#define MCPWM_DB1_RED_UPMETHOD    0x0000000FU
#define MCPWM_DB1_RED_UPMETHOD_M  (MCPWM_DB1_RED_UPMETHOD_V << MCPWM_DB1_RED_UPMETHOD_S)
#define MCPWM_DB1_RED_UPMETHOD_V  0x0000000FU
#define MCPWM_DB1_RED_UPMETHOD_S  4
/** MCPWM_DB1_DEB_MODE : R/W; bitpos: [8]; default: 0;
 *  Configures S8 in table, dual-edge B mode.\\0: fed/red take effect on different path
 *  separately\\1: fed/red take effect on B path, A out is in bypass or dulpB mode
 */
#define MCPWM_DB1_DEB_MODE    (BIT(8))
#define MCPWM_DB1_DEB_MODE_M  (MCPWM_DB1_DEB_MODE_V << MCPWM_DB1_DEB_MODE_S)
#define MCPWM_DB1_DEB_MODE_V  0x00000001U
#define MCPWM_DB1_DEB_MODE_S  8
/** MCPWM_DB1_A_OUTSWAP : R/W; bitpos: [9]; default: 0;
 *  Configures S6 in table.
 */
#define MCPWM_DB1_A_OUTSWAP    (BIT(9))
#define MCPWM_DB1_A_OUTSWAP_M  (MCPWM_DB1_A_OUTSWAP_V << MCPWM_DB1_A_OUTSWAP_S)
#define MCPWM_DB1_A_OUTSWAP_V  0x00000001U
#define MCPWM_DB1_A_OUTSWAP_S  9
/** MCPWM_DB1_B_OUTSWAP : R/W; bitpos: [10]; default: 0;
 *  Configures S7 in table.
 */
#define MCPWM_DB1_B_OUTSWAP    (BIT(10))
#define MCPWM_DB1_B_OUTSWAP_M  (MCPWM_DB1_B_OUTSWAP_V << MCPWM_DB1_B_OUTSWAP_S)
#define MCPWM_DB1_B_OUTSWAP_V  0x00000001U
#define MCPWM_DB1_B_OUTSWAP_S  10
/** MCPWM_DB1_RED_INSEL : R/W; bitpos: [11]; default: 0;
 *  Configures S4 in table.
 */
#define MCPWM_DB1_RED_INSEL    (BIT(11))
#define MCPWM_DB1_RED_INSEL_M  (MCPWM_DB1_RED_INSEL_V << MCPWM_DB1_RED_INSEL_S)
#define MCPWM_DB1_RED_INSEL_V  0x00000001U
#define MCPWM_DB1_RED_INSEL_S  11
/** MCPWM_DB1_FED_INSEL : R/W; bitpos: [12]; default: 0;
 *  Configures S5 in table.
 */
#define MCPWM_DB1_FED_INSEL    (BIT(12))
#define MCPWM_DB1_FED_INSEL_M  (MCPWM_DB1_FED_INSEL_V << MCPWM_DB1_FED_INSEL_S)
#define MCPWM_DB1_FED_INSEL_V  0x00000001U
#define MCPWM_DB1_FED_INSEL_S  12
/** MCPWM_DB1_RED_OUTINVERT : R/W; bitpos: [13]; default: 0;
 *  Configures S2 in table.
 */
#define MCPWM_DB1_RED_OUTINVERT    (BIT(13))
#define MCPWM_DB1_RED_OUTINVERT_M  (MCPWM_DB1_RED_OUTINVERT_V << MCPWM_DB1_RED_OUTINVERT_S)
#define MCPWM_DB1_RED_OUTINVERT_V  0x00000001U
#define MCPWM_DB1_RED_OUTINVERT_S  13
/** MCPWM_DB1_FED_OUTINVERT : R/W; bitpos: [14]; default: 0;
 *  Configures S3 in table.
 */
#define MCPWM_DB1_FED_OUTINVERT    (BIT(14))
#define MCPWM_DB1_FED_OUTINVERT_M  (MCPWM_DB1_FED_OUTINVERT_V << MCPWM_DB1_FED_OUTINVERT_S)
#define MCPWM_DB1_FED_OUTINVERT_V  0x00000001U
#define MCPWM_DB1_FED_OUTINVERT_S  14
/** MCPWM_DB1_A_OUTBYPASS : R/W; bitpos: [15]; default: 1;
 *  Configures S1 in table.
 */
#define MCPWM_DB1_A_OUTBYPASS    (BIT(15))
#define MCPWM_DB1_A_OUTBYPASS_M  (MCPWM_DB1_A_OUTBYPASS_V << MCPWM_DB1_A_OUTBYPASS_S)
#define MCPWM_DB1_A_OUTBYPASS_V  0x00000001U
#define MCPWM_DB1_A_OUTBYPASS_S  15
/** MCPWM_DB1_B_OUTBYPASS : R/W; bitpos: [16]; default: 1;
 *  Configures S0 in table.
 */
#define MCPWM_DB1_B_OUTBYPASS    (BIT(16))
#define MCPWM_DB1_B_OUTBYPASS_M  (MCPWM_DB1_B_OUTBYPASS_V << MCPWM_DB1_B_OUTBYPASS_S)
#define MCPWM_DB1_B_OUTBYPASS_V  0x00000001U
#define MCPWM_DB1_B_OUTBYPASS_S  16
/** MCPWM_DB1_CLK_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures dead time generator $n clock selection.\\0: PWM_clk\\1: PT_clk
 */
#define MCPWM_DB1_CLK_SEL    (BIT(17))
#define MCPWM_DB1_CLK_SEL_M  (MCPWM_DB1_CLK_SEL_V << MCPWM_DB1_CLK_SEL_S)
#define MCPWM_DB1_CLK_SEL_V  0x00000001U
#define MCPWM_DB1_CLK_SEL_S  17

/** MCPWM_DT1_FED_CFG_REG register
 *  Falling edge delay (FED) shadow register
 */
#define MCPWM_DT1_FED_CFG_REG (DR_REG_MCPWM_BASE + 0x94)
/** MCPWM_DB1_FED : R/W; bitpos: [15:0]; default: 0;
 *  Configures shadow register for FED.
 */
#define MCPWM_DB1_FED    0x0000FFFFU
#define MCPWM_DB1_FED_M  (MCPWM_DB1_FED_V << MCPWM_DB1_FED_S)
#define MCPWM_DB1_FED_V  0x0000FFFFU
#define MCPWM_DB1_FED_S  0

/** MCPWM_DT1_RED_CFG_REG register
 *  Rising edge delay (RED) shadow register
 */
#define MCPWM_DT1_RED_CFG_REG (DR_REG_MCPWM_BASE + 0x98)
/** MCPWM_DB1_RED : R/W; bitpos: [15:0]; default: 0;
 *  Configures shadow register for RED.
 */
#define MCPWM_DB1_RED    0x0000FFFFU
#define MCPWM_DB1_RED_M  (MCPWM_DB1_RED_V << MCPWM_DB1_RED_S)
#define MCPWM_DB1_RED_V  0x0000FFFFU
#define MCPWM_DB1_RED_S  0

/** MCPWM_CARRIER1_CFG_REG register
 *  Carrier$n configuration register
 */
#define MCPWM_CARRIER1_CFG_REG (DR_REG_MCPWM_BASE + 0x9c)
/** MCPWM_CHOPPER1_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable carrier$n.\\0: Bypassed\\1: Enabled
 */
#define MCPWM_CHOPPER1_EN    (BIT(0))
#define MCPWM_CHOPPER1_EN_M  (MCPWM_CHOPPER1_EN_V << MCPWM_CHOPPER1_EN_S)
#define MCPWM_CHOPPER1_EN_V  0x00000001U
#define MCPWM_CHOPPER1_EN_S  0
/** MCPWM_CHOPPER1_PRESCALE : R/W; bitpos: [4:1]; default: 0;
 *  Configures the prescale value of PWM carrier$n clock (PC_clk), so that period of
 *  PC_clk = period of PWM_clk * (PWM_CARRIER$n_PRESCALE + 1)
 */
#define MCPWM_CHOPPER1_PRESCALE    0x0000000FU
#define MCPWM_CHOPPER1_PRESCALE_M  (MCPWM_CHOPPER1_PRESCALE_V << MCPWM_CHOPPER1_PRESCALE_S)
#define MCPWM_CHOPPER1_PRESCALE_V  0x0000000FU
#define MCPWM_CHOPPER1_PRESCALE_S  1
/** MCPWM_CHOPPER1_DUTY : R/W; bitpos: [7:5]; default: 0;
 *  Configures carrier duty. Duty = PWM_CARRIER$n_DUTY / 8
 */
#define MCPWM_CHOPPER1_DUTY    0x00000007U
#define MCPWM_CHOPPER1_DUTY_M  (MCPWM_CHOPPER1_DUTY_V << MCPWM_CHOPPER1_DUTY_S)
#define MCPWM_CHOPPER1_DUTY_V  0x00000007U
#define MCPWM_CHOPPER1_DUTY_S  5
/** MCPWM_CHOPPER1_OSHTWTH : R/W; bitpos: [11:8]; default: 0;
 *  Configures width of the first pulse. Measurement unit: Periods of the carrier.
 */
#define MCPWM_CHOPPER1_OSHTWTH    0x0000000FU
#define MCPWM_CHOPPER1_OSHTWTH_M  (MCPWM_CHOPPER1_OSHTWTH_V << MCPWM_CHOPPER1_OSHTWTH_S)
#define MCPWM_CHOPPER1_OSHTWTH_V  0x0000000FU
#define MCPWM_CHOPPER1_OSHTWTH_S  8
/** MCPWM_CHOPPER1_OUT_INVERT : R/W; bitpos: [12]; default: 0;
 *  Configures whether or not to invert the output of PWM$n A and PWM$n B for this
 *  submodule.\\0: Normal\\1: Invert
 */
#define MCPWM_CHOPPER1_OUT_INVERT    (BIT(12))
#define MCPWM_CHOPPER1_OUT_INVERT_M  (MCPWM_CHOPPER1_OUT_INVERT_V << MCPWM_CHOPPER1_OUT_INVERT_S)
#define MCPWM_CHOPPER1_OUT_INVERT_V  0x00000001U
#define MCPWM_CHOPPER1_OUT_INVERT_S  12
/** MCPWM_CHOPPER1_IN_INVERT : R/W; bitpos: [13]; default: 0;
 *  Configures whether or not to invert the input of PWM$n A and PWM$n B for this
 *  submodule.\\0: Normal\\1: Invert
 */
#define MCPWM_CHOPPER1_IN_INVERT    (BIT(13))
#define MCPWM_CHOPPER1_IN_INVERT_M  (MCPWM_CHOPPER1_IN_INVERT_V << MCPWM_CHOPPER1_IN_INVERT_S)
#define MCPWM_CHOPPER1_IN_INVERT_V  0x00000001U
#define MCPWM_CHOPPER1_IN_INVERT_S  13

/** MCPWM_FH1_CFG0_REG register
 *  PWM$n A and PWM$n B trip events actions configuration register
 */
#define MCPWM_FH1_CFG0_REG (DR_REG_MCPWM_BASE + 0xa0)
/** MCPWM_TZ1_SW_CBC : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable software force cycle-by-cycle mode action.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_TZ1_SW_CBC    (BIT(0))
#define MCPWM_TZ1_SW_CBC_M  (MCPWM_TZ1_SW_CBC_V << MCPWM_TZ1_SW_CBC_S)
#define MCPWM_TZ1_SW_CBC_V  0x00000001U
#define MCPWM_TZ1_SW_CBC_S  0
/** MCPWM_TZ1_F2_CBC : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not event_f2 will trigger cycle-by-cycle mode action.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_TZ1_F2_CBC    (BIT(1))
#define MCPWM_TZ1_F2_CBC_M  (MCPWM_TZ1_F2_CBC_V << MCPWM_TZ1_F2_CBC_S)
#define MCPWM_TZ1_F2_CBC_V  0x00000001U
#define MCPWM_TZ1_F2_CBC_S  1
/** MCPWM_TZ1_F1_CBC : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not event_f1 will trigger cycle-by-cycle mode action.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_TZ1_F1_CBC    (BIT(2))
#define MCPWM_TZ1_F1_CBC_M  (MCPWM_TZ1_F1_CBC_V << MCPWM_TZ1_F1_CBC_S)
#define MCPWM_TZ1_F1_CBC_V  0x00000001U
#define MCPWM_TZ1_F1_CBC_S  2
/** MCPWM_TZ1_F0_CBC : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not event_f0 will trigger cycle-by-cycle mode action.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_TZ1_F0_CBC    (BIT(3))
#define MCPWM_TZ1_F0_CBC_M  (MCPWM_TZ1_F0_CBC_V << MCPWM_TZ1_F0_CBC_S)
#define MCPWM_TZ1_F0_CBC_V  0x00000001U
#define MCPWM_TZ1_F0_CBC_S  3
/** MCPWM_TZ1_SW_OST : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable software force one-shot mode action.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_TZ1_SW_OST    (BIT(4))
#define MCPWM_TZ1_SW_OST_M  (MCPWM_TZ1_SW_OST_V << MCPWM_TZ1_SW_OST_S)
#define MCPWM_TZ1_SW_OST_V  0x00000001U
#define MCPWM_TZ1_SW_OST_S  4
/** MCPWM_TZ1_F2_OST : R/W; bitpos: [5]; default: 0;
 *  Configures whether or not event_f2 will trigger one-shot mode action.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_TZ1_F2_OST    (BIT(5))
#define MCPWM_TZ1_F2_OST_M  (MCPWM_TZ1_F2_OST_V << MCPWM_TZ1_F2_OST_S)
#define MCPWM_TZ1_F2_OST_V  0x00000001U
#define MCPWM_TZ1_F2_OST_S  5
/** MCPWM_TZ1_F1_OST : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not event_f1 will trigger one-shot mode action.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_TZ1_F1_OST    (BIT(6))
#define MCPWM_TZ1_F1_OST_M  (MCPWM_TZ1_F1_OST_V << MCPWM_TZ1_F1_OST_S)
#define MCPWM_TZ1_F1_OST_V  0x00000001U
#define MCPWM_TZ1_F1_OST_S  6
/** MCPWM_TZ1_F0_OST : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not event_f0 will trigger one-shot mode action.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_TZ1_F0_OST    (BIT(7))
#define MCPWM_TZ1_F0_OST_M  (MCPWM_TZ1_F0_OST_V << MCPWM_TZ1_F0_OST_S)
#define MCPWM_TZ1_F0_OST_V  0x00000001U
#define MCPWM_TZ1_F0_OST_S  7
/** MCPWM_TZ1_A_CBC_D : R/W; bitpos: [9:8]; default: 0;
 *  Configures cycle-by-cycle mode action on PWM$n A when fault event occurs and timer
 *  is decreasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
 */
#define MCPWM_TZ1_A_CBC_D    0x00000003U
#define MCPWM_TZ1_A_CBC_D_M  (MCPWM_TZ1_A_CBC_D_V << MCPWM_TZ1_A_CBC_D_S)
#define MCPWM_TZ1_A_CBC_D_V  0x00000003U
#define MCPWM_TZ1_A_CBC_D_S  8
/** MCPWM_TZ1_A_CBC_U : R/W; bitpos: [11:10]; default: 0;
 *  Configures cycle-by-cycle mode action on PWM$n A when fault event occurs and timer
 *  is increasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
 */
#define MCPWM_TZ1_A_CBC_U    0x00000003U
#define MCPWM_TZ1_A_CBC_U_M  (MCPWM_TZ1_A_CBC_U_V << MCPWM_TZ1_A_CBC_U_S)
#define MCPWM_TZ1_A_CBC_U_V  0x00000003U
#define MCPWM_TZ1_A_CBC_U_S  10
/** MCPWM_TZ1_A_OST_D : R/W; bitpos: [13:12]; default: 0;
 *  Configures one-shot mode action on PWM$n A when fault event occurs and timer is
 *  decreasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
 */
#define MCPWM_TZ1_A_OST_D    0x00000003U
#define MCPWM_TZ1_A_OST_D_M  (MCPWM_TZ1_A_OST_D_V << MCPWM_TZ1_A_OST_D_S)
#define MCPWM_TZ1_A_OST_D_V  0x00000003U
#define MCPWM_TZ1_A_OST_D_S  12
/** MCPWM_TZ1_A_OST_U : R/W; bitpos: [15:14]; default: 0;
 *  Configures one-shot mode action on PWM$n A when fault event occurs and timer is
 *  increasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
 */
#define MCPWM_TZ1_A_OST_U    0x00000003U
#define MCPWM_TZ1_A_OST_U_M  (MCPWM_TZ1_A_OST_U_V << MCPWM_TZ1_A_OST_U_S)
#define MCPWM_TZ1_A_OST_U_V  0x00000003U
#define MCPWM_TZ1_A_OST_U_S  14
/** MCPWM_TZ1_B_CBC_D : R/W; bitpos: [17:16]; default: 0;
 *  Configures cycle-by-cycle mode action on PWM$n B when fault event occurs and timer
 *  is decreasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
 */
#define MCPWM_TZ1_B_CBC_D    0x00000003U
#define MCPWM_TZ1_B_CBC_D_M  (MCPWM_TZ1_B_CBC_D_V << MCPWM_TZ1_B_CBC_D_S)
#define MCPWM_TZ1_B_CBC_D_V  0x00000003U
#define MCPWM_TZ1_B_CBC_D_S  16
/** MCPWM_TZ1_B_CBC_U : R/W; bitpos: [19:18]; default: 0;
 *  Configures cycle-by-cycle mode action on PWM$n B when fault event occurs and timer
 *  is increasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
 */
#define MCPWM_TZ1_B_CBC_U    0x00000003U
#define MCPWM_TZ1_B_CBC_U_M  (MCPWM_TZ1_B_CBC_U_V << MCPWM_TZ1_B_CBC_U_S)
#define MCPWM_TZ1_B_CBC_U_V  0x00000003U
#define MCPWM_TZ1_B_CBC_U_S  18
/** MCPWM_TZ1_B_OST_D : R/W; bitpos: [21:20]; default: 0;
 *  Configures one-shot mode action on PWM$n B when fault event occurs and timer is
 *  decreasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
 */
#define MCPWM_TZ1_B_OST_D    0x00000003U
#define MCPWM_TZ1_B_OST_D_M  (MCPWM_TZ1_B_OST_D_V << MCPWM_TZ1_B_OST_D_S)
#define MCPWM_TZ1_B_OST_D_V  0x00000003U
#define MCPWM_TZ1_B_OST_D_S  20
/** MCPWM_TZ1_B_OST_U : R/W; bitpos: [23:22]; default: 0;
 *  Configures one-shot mode action on PWM$n B when fault event occurs and timer is
 *  increasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
 */
#define MCPWM_TZ1_B_OST_U    0x00000003U
#define MCPWM_TZ1_B_OST_U_M  (MCPWM_TZ1_B_OST_U_V << MCPWM_TZ1_B_OST_U_S)
#define MCPWM_TZ1_B_OST_U_V  0x00000003U
#define MCPWM_TZ1_B_OST_U_S  22

/** MCPWM_FH1_CFG1_REG register
 *  Software triggers for fault handler actions configuration register
 */
#define MCPWM_FH1_CFG1_REG (DR_REG_MCPWM_BASE + 0xa4)
/** MCPWM_TZ1_CLR_OST : R/W; bitpos: [0]; default: 0;
 *  Configures the generation of software one-shot mode action clear. A toggle
 *  (software negate its value) triggers a clear for on going one-shot mode action.
 */
#define MCPWM_TZ1_CLR_OST    (BIT(0))
#define MCPWM_TZ1_CLR_OST_M  (MCPWM_TZ1_CLR_OST_V << MCPWM_TZ1_CLR_OST_S)
#define MCPWM_TZ1_CLR_OST_V  0x00000001U
#define MCPWM_TZ1_CLR_OST_S  0
/** MCPWM_TZ1_CBCPULSE : R/W; bitpos: [2:1]; default: 0;
 *  Configures the refresh moment selection of cycle-by-cycle mode action.\\0: Select
 *  nothing, will not refresh\\Bit0 is set to 1: TEZ\\Bit1 is set to 1: TEP
 */
#define MCPWM_TZ1_CBCPULSE    0x00000003U
#define MCPWM_TZ1_CBCPULSE_M  (MCPWM_TZ1_CBCPULSE_V << MCPWM_TZ1_CBCPULSE_S)
#define MCPWM_TZ1_CBCPULSE_V  0x00000003U
#define MCPWM_TZ1_CBCPULSE_S  1
/** MCPWM_TZ1_FORCE_CBC : R/W; bitpos: [3]; default: 0;
 *  Configures the generation of software cycle-by-cycle mode action. A toggle
 *  (software negate its value) triggers a cycle-by-cycle mode action.
 */
#define MCPWM_TZ1_FORCE_CBC    (BIT(3))
#define MCPWM_TZ1_FORCE_CBC_M  (MCPWM_TZ1_FORCE_CBC_V << MCPWM_TZ1_FORCE_CBC_S)
#define MCPWM_TZ1_FORCE_CBC_V  0x00000001U
#define MCPWM_TZ1_FORCE_CBC_S  3
/** MCPWM_TZ1_FORCE_OST : R/W; bitpos: [4]; default: 0;
 *  Configures the generation of software one-shot mode action. A toggle (software
 *  negate its value) triggers a one-shot mode action.
 */
#define MCPWM_TZ1_FORCE_OST    (BIT(4))
#define MCPWM_TZ1_FORCE_OST_M  (MCPWM_TZ1_FORCE_OST_V << MCPWM_TZ1_FORCE_OST_S)
#define MCPWM_TZ1_FORCE_OST_V  0x00000001U
#define MCPWM_TZ1_FORCE_OST_S  4

/** MCPWM_FH1_STATUS_REG register
 *  Fault events status register
 */
#define MCPWM_FH1_STATUS_REG (DR_REG_MCPWM_BASE + 0xa8)
/** MCPWM_TZ1_CBC_ON : RO; bitpos: [0]; default: 0;
 *  Represents whether or not an cycle-by-cycle mode action is on going.\\0:No
 *  action\\1: On going
 */
#define MCPWM_TZ1_CBC_ON    (BIT(0))
#define MCPWM_TZ1_CBC_ON_M  (MCPWM_TZ1_CBC_ON_V << MCPWM_TZ1_CBC_ON_S)
#define MCPWM_TZ1_CBC_ON_V  0x00000001U
#define MCPWM_TZ1_CBC_ON_S  0
/** MCPWM_TZ1_OST_ON : RO; bitpos: [1]; default: 0;
 *  Represents whether or not an one-shot mode action is on going.\\0:No action\\1: On
 *  going
 */
#define MCPWM_TZ1_OST_ON    (BIT(1))
#define MCPWM_TZ1_OST_ON_M  (MCPWM_TZ1_OST_ON_V << MCPWM_TZ1_OST_ON_S)
#define MCPWM_TZ1_OST_ON_V  0x00000001U
#define MCPWM_TZ1_OST_ON_S  1

/** MCPWM_GEN2_STMP_CFG_REG register
 *  Generator2 time stamp registers A and B transfer status and update method register
 */
#define MCPWM_GEN2_STMP_CFG_REG (DR_REG_MCPWM_BASE + 0xac)
/** MCPWM_CMPR0_A_UPMETHOD : R/W; bitpos: [3:0]; default: 0;
 *  Configures the update method for PWM generator 2 time stamp A's active
 *  register.\\0: Immediately\\Bit0 is set to 1: TEZ\\Bit1 is set to 1: TEP\\Bit2 is
 *  set to 1: Sync\\Bit3 is set to 1: Disable the update
 */
#define MCPWM_CMPR0_A_UPMETHOD    0x0000000FU
#define MCPWM_CMPR0_A_UPMETHOD_M  (MCPWM_CMPR0_A_UPMETHOD_V << MCPWM_CMPR0_A_UPMETHOD_S)
#define MCPWM_CMPR0_A_UPMETHOD_V  0x0000000FU
#define MCPWM_CMPR0_A_UPMETHOD_S  0
/** MCPWM_CMPR0_B_UPMETHOD : R/W; bitpos: [7:4]; default: 0;
 *  Configures the update method for PWM generator 2 time stamp B's active
 *  register.\\0: Immediately\\Bit0 is set to 1: TEZ\\Bit1 is set to 1: TEP\\Bit2 is
 *  set to 1: Sync\\Bit3 is set to 1: Disable the update
 */
#define MCPWM_CMPR0_B_UPMETHOD    0x0000000FU
#define MCPWM_CMPR0_B_UPMETHOD_M  (MCPWM_CMPR0_B_UPMETHOD_V << MCPWM_CMPR0_B_UPMETHOD_S)
#define MCPWM_CMPR0_B_UPMETHOD_V  0x0000000FU
#define MCPWM_CMPR0_B_UPMETHOD_S  4
/** MCPWM_CMPR0_A_SHDW_FULL : R/W/WTC/SC; bitpos: [8]; default: 0;
 *  Represents whether or not generator2 time stamp A's shadow reg is transferred.\\0:
 *  A's active reg has been updated with shadow register latest value.\\1: A's shadow
 *  reg is filled and waiting to be transferred to A's active reg
 */
#define MCPWM_CMPR0_A_SHDW_FULL    (BIT(8))
#define MCPWM_CMPR0_A_SHDW_FULL_M  (MCPWM_CMPR0_A_SHDW_FULL_V << MCPWM_CMPR0_A_SHDW_FULL_S)
#define MCPWM_CMPR0_A_SHDW_FULL_V  0x00000001U
#define MCPWM_CMPR0_A_SHDW_FULL_S  8
/** MCPWM_CMPR0_B_SHDW_FULL : R/W/WTC/SC; bitpos: [9]; default: 0;
 *  Represents whether or not generator2 time stamp B's shadow reg is transferred.\\0:
 *  B's active reg has been updated with shadow register latest value.\\1: B's shadow
 *  reg is filled and waiting to be transferred to B's active reg
 */
#define MCPWM_CMPR0_B_SHDW_FULL    (BIT(9))
#define MCPWM_CMPR0_B_SHDW_FULL_M  (MCPWM_CMPR0_B_SHDW_FULL_V << MCPWM_CMPR0_B_SHDW_FULL_S)
#define MCPWM_CMPR0_B_SHDW_FULL_V  0x00000001U
#define MCPWM_CMPR0_B_SHDW_FULL_S  9

/** MCPWM_GEN2_TSTMP_A_REG register
 *  Generator$n time stamp A's shadow register
 */
#define MCPWM_GEN2_TSTMP_A_REG (DR_REG_MCPWM_BASE + 0xb0)
/** MCPWM_CMPR2_A : R/W; bitpos: [15:0]; default: 0;
 *  Configures the value of PWM generator $n time stamp A's shadow register.
 */
#define MCPWM_CMPR2_A    0x0000FFFFU
#define MCPWM_CMPR2_A_M  (MCPWM_CMPR2_A_V << MCPWM_CMPR2_A_S)
#define MCPWM_CMPR2_A_V  0x0000FFFFU
#define MCPWM_CMPR2_A_S  0

/** MCPWM_GEN2_TSTMP_B_REG register
 *  Generator$n time stamp B's shadow register
 */
#define MCPWM_GEN2_TSTMP_B_REG (DR_REG_MCPWM_BASE + 0xb4)
/** MCPWM_CMPR2_B : R/W; bitpos: [15:0]; default: 0;
 *  Configures the value of PWM generator $n time stamp B's shadow register.
 */
#define MCPWM_CMPR2_B    0x0000FFFFU
#define MCPWM_CMPR2_B_M  (MCPWM_CMPR2_B_V << MCPWM_CMPR2_B_S)
#define MCPWM_CMPR2_B_V  0x0000FFFFU
#define MCPWM_CMPR2_B_S  0

/** MCPWM_GEN2_CFG0_REG register
 *  Generator$n fault event T0 and T1 configuration register
 */
#define MCPWM_GEN2_CFG0_REG (DR_REG_MCPWM_BASE + 0xb8)
/** MCPWM_GEN2_CFG_UPMETHOD : R/W; bitpos: [3:0]; default: 0;
 *  Configures update method for PWM generator $n's active register.\\0:
 *  Immediately\\Bit0 is set to 1: TEZ\\Bit1 is set to 1: TEP\\Bit2 is set to 1:
 *  Sync\\Bit3 is set to 1: Disable the update
 */
#define MCPWM_GEN2_CFG_UPMETHOD    0x0000000FU
#define MCPWM_GEN2_CFG_UPMETHOD_M  (MCPWM_GEN2_CFG_UPMETHOD_V << MCPWM_GEN2_CFG_UPMETHOD_S)
#define MCPWM_GEN2_CFG_UPMETHOD_V  0x0000000FU
#define MCPWM_GEN2_CFG_UPMETHOD_S  0
/** MCPWM_GEN2_T0_SEL : R/W; bitpos: [6:4]; default: 0;
 *  Configures source selection for PWM generator $n event_t0, take effect
 *  immediately.\\0: fault_event0\\1: fault_event1\\2: fault_event2\\3: sync_taken\\4:
 *  Invalid, Select nothing
 */
#define MCPWM_GEN2_T0_SEL    0x00000007U
#define MCPWM_GEN2_T0_SEL_M  (MCPWM_GEN2_T0_SEL_V << MCPWM_GEN2_T0_SEL_S)
#define MCPWM_GEN2_T0_SEL_V  0x00000007U
#define MCPWM_GEN2_T0_SEL_S  4
/** MCPWM_GEN2_T1_SEL : R/W; bitpos: [9:7]; default: 0;
 *  Configures source selection for PWM generator $n event_t1, take effect
 *  immediately.\\0: fault_event0\\1: fault_event1\\2: fault_event2\\3: sync_taken\\4:
 *  Invalid, Select nothing
 */
#define MCPWM_GEN2_T1_SEL    0x00000007U
#define MCPWM_GEN2_T1_SEL_M  (MCPWM_GEN2_T1_SEL_V << MCPWM_GEN2_T1_SEL_S)
#define MCPWM_GEN2_T1_SEL_V  0x00000007U
#define MCPWM_GEN2_T1_SEL_S  7

/** MCPWM_GEN2_FORCE_REG register
 *  Generator$n output signal force mode register.
 */
#define MCPWM_GEN2_FORCE_REG (DR_REG_MCPWM_BASE + 0xbc)
/** MCPWM_GEN2_CNTUFORCE_UPMETHOD : R/W; bitpos: [5:0]; default: 32;
 *  Configures update method for continuous software force of PWM generator$n.\\0:
 *  Immediately\\Bit0 is set to 1: TEZ\\Bit1 is set to 1: TEP\\Bit2 is set to 1:
 *  TEA\\Bit3 is set to 1: TEB\\Bit4 is set to 1: Sync\\Bit5 is set to 1: Disable
 *  update. TEA/B here and below means an event generated when the timer's value equals
 *  to that of register A/B.
 */
#define MCPWM_GEN2_CNTUFORCE_UPMETHOD    0x0000003FU
#define MCPWM_GEN2_CNTUFORCE_UPMETHOD_M  (MCPWM_GEN2_CNTUFORCE_UPMETHOD_V << MCPWM_GEN2_CNTUFORCE_UPMETHOD_S)
#define MCPWM_GEN2_CNTUFORCE_UPMETHOD_V  0x0000003FU
#define MCPWM_GEN2_CNTUFORCE_UPMETHOD_S  0
/** MCPWM_GEN2_A_CNTUFORCE_MODE : R/W; bitpos: [7:6]; default: 0;
 *  Configures continuous software force mode for PWM$n A.\\0: Disabled\\1: Low\\2:
 *  High\\3: Disabled
 */
#define MCPWM_GEN2_A_CNTUFORCE_MODE    0x00000003U
#define MCPWM_GEN2_A_CNTUFORCE_MODE_M  (MCPWM_GEN2_A_CNTUFORCE_MODE_V << MCPWM_GEN2_A_CNTUFORCE_MODE_S)
#define MCPWM_GEN2_A_CNTUFORCE_MODE_V  0x00000003U
#define MCPWM_GEN2_A_CNTUFORCE_MODE_S  6
/** MCPWM_GEN2_B_CNTUFORCE_MODE : R/W; bitpos: [9:8]; default: 0;
 *  Configures continuous software force mode for PWM$n B.\\0: Disabled\\1: Low\\2:
 *  High\\3: Disabled
 */
#define MCPWM_GEN2_B_CNTUFORCE_MODE    0x00000003U
#define MCPWM_GEN2_B_CNTUFORCE_MODE_M  (MCPWM_GEN2_B_CNTUFORCE_MODE_V << MCPWM_GEN2_B_CNTUFORCE_MODE_S)
#define MCPWM_GEN2_B_CNTUFORCE_MODE_V  0x00000003U
#define MCPWM_GEN2_B_CNTUFORCE_MODE_S  8
/** MCPWM_GEN2_A_NCIFORCE : R/W; bitpos: [10]; default: 0;
 *  Configures the generation of non-continuous immediate software-force event for
 *  PWM$n A, a toggle will trigger a force event.
 */
#define MCPWM_GEN2_A_NCIFORCE    (BIT(10))
#define MCPWM_GEN2_A_NCIFORCE_M  (MCPWM_GEN2_A_NCIFORCE_V << MCPWM_GEN2_A_NCIFORCE_S)
#define MCPWM_GEN2_A_NCIFORCE_V  0x00000001U
#define MCPWM_GEN2_A_NCIFORCE_S  10
/** MCPWM_GEN2_A_NCIFORCE_MODE : R/W; bitpos: [12:11]; default: 0;
 *  Configures non-continuous immediate software force mode for PWM$n A.\\0:
 *  Disabled\\1: Low\\2: High\\3: Disabled
 */
#define MCPWM_GEN2_A_NCIFORCE_MODE    0x00000003U
#define MCPWM_GEN2_A_NCIFORCE_MODE_M  (MCPWM_GEN2_A_NCIFORCE_MODE_V << MCPWM_GEN2_A_NCIFORCE_MODE_S)
#define MCPWM_GEN2_A_NCIFORCE_MODE_V  0x00000003U
#define MCPWM_GEN2_A_NCIFORCE_MODE_S  11
/** MCPWM_GEN2_B_NCIFORCE : R/W; bitpos: [13]; default: 0;
 *  Configures the generation of non-continuous immediate software-force event for
 *  PWM$n B, a toggle will trigger a force event.
 */
#define MCPWM_GEN2_B_NCIFORCE    (BIT(13))
#define MCPWM_GEN2_B_NCIFORCE_M  (MCPWM_GEN2_B_NCIFORCE_V << MCPWM_GEN2_B_NCIFORCE_S)
#define MCPWM_GEN2_B_NCIFORCE_V  0x00000001U
#define MCPWM_GEN2_B_NCIFORCE_S  13
/** MCPWM_GEN2_B_NCIFORCE_MODE : R/W; bitpos: [15:14]; default: 0;
 *  Configures non-continuous immediate software force mode for PWM$n B.\\0:
 *  Disabled\\1: Low\\2: High\\3: Disabled
 */
#define MCPWM_GEN2_B_NCIFORCE_MODE    0x00000003U
#define MCPWM_GEN2_B_NCIFORCE_MODE_M  (MCPWM_GEN2_B_NCIFORCE_MODE_V << MCPWM_GEN2_B_NCIFORCE_MODE_S)
#define MCPWM_GEN2_B_NCIFORCE_MODE_V  0x00000003U
#define MCPWM_GEN2_B_NCIFORCE_MODE_S  14

/** MCPWM_GEN2_A_REG register
 *  PWM$n output signal A actions configuration register
 */
#define MCPWM_GEN2_A_REG (DR_REG_MCPWM_BASE + 0xc0)
/** MCPWM_GEN2_A_UTEZ : R/W; bitpos: [1:0]; default: 0;
 *  Configures action on PWM$n A triggered by event TEZ when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN2_A_UTEZ    0x00000003U
#define MCPWM_GEN2_A_UTEZ_M  (MCPWM_GEN2_A_UTEZ_V << MCPWM_GEN2_A_UTEZ_S)
#define MCPWM_GEN2_A_UTEZ_V  0x00000003U
#define MCPWM_GEN2_A_UTEZ_S  0
/** MCPWM_GEN2_A_UTEP : R/W; bitpos: [3:2]; default: 0;
 *  Configures action on PWM$n A triggered by event TEP when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN2_A_UTEP    0x00000003U
#define MCPWM_GEN2_A_UTEP_M  (MCPWM_GEN2_A_UTEP_V << MCPWM_GEN2_A_UTEP_S)
#define MCPWM_GEN2_A_UTEP_V  0x00000003U
#define MCPWM_GEN2_A_UTEP_S  2
/** MCPWM_GEN2_A_UTEA : R/W; bitpos: [5:4]; default: 0;
 *  Configures action on PWM$n A triggered by event TEA when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN2_A_UTEA    0x00000003U
#define MCPWM_GEN2_A_UTEA_M  (MCPWM_GEN2_A_UTEA_V << MCPWM_GEN2_A_UTEA_S)
#define MCPWM_GEN2_A_UTEA_V  0x00000003U
#define MCPWM_GEN2_A_UTEA_S  4
/** MCPWM_GEN2_A_UTEB : R/W; bitpos: [7:6]; default: 0;
 *  Configures action on PWM$n A triggered by event TEB when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN2_A_UTEB    0x00000003U
#define MCPWM_GEN2_A_UTEB_M  (MCPWM_GEN2_A_UTEB_V << MCPWM_GEN2_A_UTEB_S)
#define MCPWM_GEN2_A_UTEB_V  0x00000003U
#define MCPWM_GEN2_A_UTEB_S  6
/** MCPWM_GEN2_A_UT0 : R/W; bitpos: [9:8]; default: 0;
 *  Configures action on PWM$n A triggered by event_t0 when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN2_A_UT0    0x00000003U
#define MCPWM_GEN2_A_UT0_M  (MCPWM_GEN2_A_UT0_V << MCPWM_GEN2_A_UT0_S)
#define MCPWM_GEN2_A_UT0_V  0x00000003U
#define MCPWM_GEN2_A_UT0_S  8
/** MCPWM_GEN2_A_UT1 : R/W; bitpos: [11:10]; default: 0;
 *  Configures action on PWM$n A triggered by event_t1 when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN2_A_UT1    0x00000003U
#define MCPWM_GEN2_A_UT1_M  (MCPWM_GEN2_A_UT1_V << MCPWM_GEN2_A_UT1_S)
#define MCPWM_GEN2_A_UT1_V  0x00000003U
#define MCPWM_GEN2_A_UT1_S  10
/** MCPWM_GEN2_A_DTEZ : R/W; bitpos: [13:12]; default: 0;
 *  Configures action on PWM$n A triggered by event TEZ when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN2_A_DTEZ    0x00000003U
#define MCPWM_GEN2_A_DTEZ_M  (MCPWM_GEN2_A_DTEZ_V << MCPWM_GEN2_A_DTEZ_S)
#define MCPWM_GEN2_A_DTEZ_V  0x00000003U
#define MCPWM_GEN2_A_DTEZ_S  12
/** MCPWM_GEN2_A_DTEP : R/W; bitpos: [15:14]; default: 0;
 *  Configures action on PWM$n A triggered by event TEP when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN2_A_DTEP    0x00000003U
#define MCPWM_GEN2_A_DTEP_M  (MCPWM_GEN2_A_DTEP_V << MCPWM_GEN2_A_DTEP_S)
#define MCPWM_GEN2_A_DTEP_V  0x00000003U
#define MCPWM_GEN2_A_DTEP_S  14
/** MCPWM_GEN2_A_DTEA : R/W; bitpos: [17:16]; default: 0;
 *  Configures action on PWM$n A triggered by event TEA when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN2_A_DTEA    0x00000003U
#define MCPWM_GEN2_A_DTEA_M  (MCPWM_GEN2_A_DTEA_V << MCPWM_GEN2_A_DTEA_S)
#define MCPWM_GEN2_A_DTEA_V  0x00000003U
#define MCPWM_GEN2_A_DTEA_S  16
/** MCPWM_GEN2_A_DTEB : R/W; bitpos: [19:18]; default: 0;
 *  Configures action on PWM$n A triggered by event TEB when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN2_A_DTEB    0x00000003U
#define MCPWM_GEN2_A_DTEB_M  (MCPWM_GEN2_A_DTEB_V << MCPWM_GEN2_A_DTEB_S)
#define MCPWM_GEN2_A_DTEB_V  0x00000003U
#define MCPWM_GEN2_A_DTEB_S  18
/** MCPWM_GEN2_A_DT0 : R/W; bitpos: [21:20]; default: 0;
 *  Configures action on PWM$n A triggered by event_t0 when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN2_A_DT0    0x00000003U
#define MCPWM_GEN2_A_DT0_M  (MCPWM_GEN2_A_DT0_V << MCPWM_GEN2_A_DT0_S)
#define MCPWM_GEN2_A_DT0_V  0x00000003U
#define MCPWM_GEN2_A_DT0_S  20
/** MCPWM_GEN2_A_DT1 : R/W; bitpos: [23:22]; default: 0;
 *  Configures action on PWM$n A triggered by event_t1 when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN2_A_DT1    0x00000003U
#define MCPWM_GEN2_A_DT1_M  (MCPWM_GEN2_A_DT1_V << MCPWM_GEN2_A_DT1_S)
#define MCPWM_GEN2_A_DT1_V  0x00000003U
#define MCPWM_GEN2_A_DT1_S  22

/** MCPWM_GEN2_B_REG register
 *  PWM$n output signal B actions configuration register
 */
#define MCPWM_GEN2_B_REG (DR_REG_MCPWM_BASE + 0xc4)
/** MCPWM_GEN2_B_UTEZ : R/W; bitpos: [1:0]; default: 0;
 *  Configures action on PWM$n B triggered by event TEZ when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN2_B_UTEZ    0x00000003U
#define MCPWM_GEN2_B_UTEZ_M  (MCPWM_GEN2_B_UTEZ_V << MCPWM_GEN2_B_UTEZ_S)
#define MCPWM_GEN2_B_UTEZ_V  0x00000003U
#define MCPWM_GEN2_B_UTEZ_S  0
/** MCPWM_GEN2_B_UTEP : R/W; bitpos: [3:2]; default: 0;
 *  Configures action on PWM$n B triggered by event TEP when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN2_B_UTEP    0x00000003U
#define MCPWM_GEN2_B_UTEP_M  (MCPWM_GEN2_B_UTEP_V << MCPWM_GEN2_B_UTEP_S)
#define MCPWM_GEN2_B_UTEP_V  0x00000003U
#define MCPWM_GEN2_B_UTEP_S  2
/** MCPWM_GEN2_B_UTEA : R/W; bitpos: [5:4]; default: 0;
 *  Configures action on PWM$n B triggered by event TEA when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN2_B_UTEA    0x00000003U
#define MCPWM_GEN2_B_UTEA_M  (MCPWM_GEN2_B_UTEA_V << MCPWM_GEN2_B_UTEA_S)
#define MCPWM_GEN2_B_UTEA_V  0x00000003U
#define MCPWM_GEN2_B_UTEA_S  4
/** MCPWM_GEN2_B_UTEB : R/W; bitpos: [7:6]; default: 0;
 *  Configures action on PWM$n B triggered by event TEB when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN2_B_UTEB    0x00000003U
#define MCPWM_GEN2_B_UTEB_M  (MCPWM_GEN2_B_UTEB_V << MCPWM_GEN2_B_UTEB_S)
#define MCPWM_GEN2_B_UTEB_V  0x00000003U
#define MCPWM_GEN2_B_UTEB_S  6
/** MCPWM_GEN2_B_UT0 : R/W; bitpos: [9:8]; default: 0;
 *  Configures action on PWM$n B triggered by event_t0 when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN2_B_UT0    0x00000003U
#define MCPWM_GEN2_B_UT0_M  (MCPWM_GEN2_B_UT0_V << MCPWM_GEN2_B_UT0_S)
#define MCPWM_GEN2_B_UT0_V  0x00000003U
#define MCPWM_GEN2_B_UT0_S  8
/** MCPWM_GEN2_B_UT1 : R/W; bitpos: [11:10]; default: 0;
 *  Configures action on PWM$n B triggered by event_t1 when timer increasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN2_B_UT1    0x00000003U
#define MCPWM_GEN2_B_UT1_M  (MCPWM_GEN2_B_UT1_V << MCPWM_GEN2_B_UT1_S)
#define MCPWM_GEN2_B_UT1_V  0x00000003U
#define MCPWM_GEN2_B_UT1_S  10
/** MCPWM_GEN2_B_DTEZ : R/W; bitpos: [13:12]; default: 0;
 *  Configures action on PWM$n B triggered by event TEZ when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN2_B_DTEZ    0x00000003U
#define MCPWM_GEN2_B_DTEZ_M  (MCPWM_GEN2_B_DTEZ_V << MCPWM_GEN2_B_DTEZ_S)
#define MCPWM_GEN2_B_DTEZ_V  0x00000003U
#define MCPWM_GEN2_B_DTEZ_S  12
/** MCPWM_GEN2_B_DTEP : R/W; bitpos: [15:14]; default: 0;
 *  Configures action on PWM$n B triggered by event TEP when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN2_B_DTEP    0x00000003U
#define MCPWM_GEN2_B_DTEP_M  (MCPWM_GEN2_B_DTEP_V << MCPWM_GEN2_B_DTEP_S)
#define MCPWM_GEN2_B_DTEP_V  0x00000003U
#define MCPWM_GEN2_B_DTEP_S  14
/** MCPWM_GEN2_B_DTEA : R/W; bitpos: [17:16]; default: 0;
 *  Configures action on PWM$n B triggered by event TEA when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN2_B_DTEA    0x00000003U
#define MCPWM_GEN2_B_DTEA_M  (MCPWM_GEN2_B_DTEA_V << MCPWM_GEN2_B_DTEA_S)
#define MCPWM_GEN2_B_DTEA_V  0x00000003U
#define MCPWM_GEN2_B_DTEA_S  16
/** MCPWM_GEN2_B_DTEB : R/W; bitpos: [19:18]; default: 0;
 *  Configures action on PWM$n B triggered by event TEB when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN2_B_DTEB    0x00000003U
#define MCPWM_GEN2_B_DTEB_M  (MCPWM_GEN2_B_DTEB_V << MCPWM_GEN2_B_DTEB_S)
#define MCPWM_GEN2_B_DTEB_V  0x00000003U
#define MCPWM_GEN2_B_DTEB_S  18
/** MCPWM_GEN2_B_DT0 : R/W; bitpos: [21:20]; default: 0;
 *  Configures action on PWM$n B triggered by event_t0 when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN2_B_DT0    0x00000003U
#define MCPWM_GEN2_B_DT0_M  (MCPWM_GEN2_B_DT0_V << MCPWM_GEN2_B_DT0_S)
#define MCPWM_GEN2_B_DT0_V  0x00000003U
#define MCPWM_GEN2_B_DT0_S  20
/** MCPWM_GEN2_B_DT1 : R/W; bitpos: [23:22]; default: 0;
 *  Configures action on PWM$n B triggered by event_t1 when timer decreasing.\\0: No
 *  change\\1: Low\\2: High\\3: Toggle
 */
#define MCPWM_GEN2_B_DT1    0x00000003U
#define MCPWM_GEN2_B_DT1_M  (MCPWM_GEN2_B_DT1_V << MCPWM_GEN2_B_DT1_S)
#define MCPWM_GEN2_B_DT1_V  0x00000003U
#define MCPWM_GEN2_B_DT1_S  22

/** MCPWM_DT2_CFG_REG register
 *  Dead time configuration register
 */
#define MCPWM_DT2_CFG_REG (DR_REG_MCPWM_BASE + 0xc8)
/** MCPWM_DB2_FED_UPMETHOD : R/W; bitpos: [3:0]; default: 0;
 *  Configures update method for FED (Falling edge delay) active register.\\0:
 *  Immediate\\Bit0 is set to 1: TEZ\\Bit1 is set to 1: TEP\\Bit2 is set to 1:
 *  Sync\\Bit3 is set to 1: Disable the update
 */
#define MCPWM_DB2_FED_UPMETHOD    0x0000000FU
#define MCPWM_DB2_FED_UPMETHOD_M  (MCPWM_DB2_FED_UPMETHOD_V << MCPWM_DB2_FED_UPMETHOD_S)
#define MCPWM_DB2_FED_UPMETHOD_V  0x0000000FU
#define MCPWM_DB2_FED_UPMETHOD_S  0
/** MCPWM_DB2_RED_UPMETHOD : R/W; bitpos: [7:4]; default: 0;
 *  Configures update method for RED (rising edge delay) active register.\\0:
 *  Immediate\\Bit0 is set to 1: TEZ\\Bit1 is set to 1: TEP\\Bit2 is set to 1:
 *  Sync\\Bit3 is set to 1: Disable the update
 */
#define MCPWM_DB2_RED_UPMETHOD    0x0000000FU
#define MCPWM_DB2_RED_UPMETHOD_M  (MCPWM_DB2_RED_UPMETHOD_V << MCPWM_DB2_RED_UPMETHOD_S)
#define MCPWM_DB2_RED_UPMETHOD_V  0x0000000FU
#define MCPWM_DB2_RED_UPMETHOD_S  4
/** MCPWM_DB2_DEB_MODE : R/W; bitpos: [8]; default: 0;
 *  Configures S8 in table, dual-edge B mode.\\0: fed/red take effect on different path
 *  separately\\1: fed/red take effect on B path, A out is in bypass or dulpB mode
 */
#define MCPWM_DB2_DEB_MODE    (BIT(8))
#define MCPWM_DB2_DEB_MODE_M  (MCPWM_DB2_DEB_MODE_V << MCPWM_DB2_DEB_MODE_S)
#define MCPWM_DB2_DEB_MODE_V  0x00000001U
#define MCPWM_DB2_DEB_MODE_S  8
/** MCPWM_DB2_A_OUTSWAP : R/W; bitpos: [9]; default: 0;
 *  Configures S6 in table.
 */
#define MCPWM_DB2_A_OUTSWAP    (BIT(9))
#define MCPWM_DB2_A_OUTSWAP_M  (MCPWM_DB2_A_OUTSWAP_V << MCPWM_DB2_A_OUTSWAP_S)
#define MCPWM_DB2_A_OUTSWAP_V  0x00000001U
#define MCPWM_DB2_A_OUTSWAP_S  9
/** MCPWM_DB2_B_OUTSWAP : R/W; bitpos: [10]; default: 0;
 *  Configures S7 in table.
 */
#define MCPWM_DB2_B_OUTSWAP    (BIT(10))
#define MCPWM_DB2_B_OUTSWAP_M  (MCPWM_DB2_B_OUTSWAP_V << MCPWM_DB2_B_OUTSWAP_S)
#define MCPWM_DB2_B_OUTSWAP_V  0x00000001U
#define MCPWM_DB2_B_OUTSWAP_S  10
/** MCPWM_DB2_RED_INSEL : R/W; bitpos: [11]; default: 0;
 *  Configures S4 in table.
 */
#define MCPWM_DB2_RED_INSEL    (BIT(11))
#define MCPWM_DB2_RED_INSEL_M  (MCPWM_DB2_RED_INSEL_V << MCPWM_DB2_RED_INSEL_S)
#define MCPWM_DB2_RED_INSEL_V  0x00000001U
#define MCPWM_DB2_RED_INSEL_S  11
/** MCPWM_DB2_FED_INSEL : R/W; bitpos: [12]; default: 0;
 *  Configures S5 in table.
 */
#define MCPWM_DB2_FED_INSEL    (BIT(12))
#define MCPWM_DB2_FED_INSEL_M  (MCPWM_DB2_FED_INSEL_V << MCPWM_DB2_FED_INSEL_S)
#define MCPWM_DB2_FED_INSEL_V  0x00000001U
#define MCPWM_DB2_FED_INSEL_S  12
/** MCPWM_DB2_RED_OUTINVERT : R/W; bitpos: [13]; default: 0;
 *  Configures S2 in table.
 */
#define MCPWM_DB2_RED_OUTINVERT    (BIT(13))
#define MCPWM_DB2_RED_OUTINVERT_M  (MCPWM_DB2_RED_OUTINVERT_V << MCPWM_DB2_RED_OUTINVERT_S)
#define MCPWM_DB2_RED_OUTINVERT_V  0x00000001U
#define MCPWM_DB2_RED_OUTINVERT_S  13
/** MCPWM_DB2_FED_OUTINVERT : R/W; bitpos: [14]; default: 0;
 *  Configures S3 in table.
 */
#define MCPWM_DB2_FED_OUTINVERT    (BIT(14))
#define MCPWM_DB2_FED_OUTINVERT_M  (MCPWM_DB2_FED_OUTINVERT_V << MCPWM_DB2_FED_OUTINVERT_S)
#define MCPWM_DB2_FED_OUTINVERT_V  0x00000001U
#define MCPWM_DB2_FED_OUTINVERT_S  14
/** MCPWM_DB2_A_OUTBYPASS : R/W; bitpos: [15]; default: 1;
 *  Configures S1 in table.
 */
#define MCPWM_DB2_A_OUTBYPASS    (BIT(15))
#define MCPWM_DB2_A_OUTBYPASS_M  (MCPWM_DB2_A_OUTBYPASS_V << MCPWM_DB2_A_OUTBYPASS_S)
#define MCPWM_DB2_A_OUTBYPASS_V  0x00000001U
#define MCPWM_DB2_A_OUTBYPASS_S  15
/** MCPWM_DB2_B_OUTBYPASS : R/W; bitpos: [16]; default: 1;
 *  Configures S0 in table.
 */
#define MCPWM_DB2_B_OUTBYPASS    (BIT(16))
#define MCPWM_DB2_B_OUTBYPASS_M  (MCPWM_DB2_B_OUTBYPASS_V << MCPWM_DB2_B_OUTBYPASS_S)
#define MCPWM_DB2_B_OUTBYPASS_V  0x00000001U
#define MCPWM_DB2_B_OUTBYPASS_S  16
/** MCPWM_DB2_CLK_SEL : R/W; bitpos: [17]; default: 0;
 *  Configures dead time generator $n clock selection.\\0: PWM_clk\\1: PT_clk
 */
#define MCPWM_DB2_CLK_SEL    (BIT(17))
#define MCPWM_DB2_CLK_SEL_M  (MCPWM_DB2_CLK_SEL_V << MCPWM_DB2_CLK_SEL_S)
#define MCPWM_DB2_CLK_SEL_V  0x00000001U
#define MCPWM_DB2_CLK_SEL_S  17

/** MCPWM_DT2_FED_CFG_REG register
 *  Falling edge delay (FED) shadow register
 */
#define MCPWM_DT2_FED_CFG_REG (DR_REG_MCPWM_BASE + 0xcc)
/** MCPWM_DB2_FED : R/W; bitpos: [15:0]; default: 0;
 *  Configures shadow register for FED.
 */
#define MCPWM_DB2_FED    0x0000FFFFU
#define MCPWM_DB2_FED_M  (MCPWM_DB2_FED_V << MCPWM_DB2_FED_S)
#define MCPWM_DB2_FED_V  0x0000FFFFU
#define MCPWM_DB2_FED_S  0

/** MCPWM_DT2_RED_CFG_REG register
 *  Rising edge delay (RED) shadow register
 */
#define MCPWM_DT2_RED_CFG_REG (DR_REG_MCPWM_BASE + 0xd0)
/** MCPWM_DB2_RED : R/W; bitpos: [15:0]; default: 0;
 *  Configures shadow register for RED.
 */
#define MCPWM_DB2_RED    0x0000FFFFU
#define MCPWM_DB2_RED_M  (MCPWM_DB2_RED_V << MCPWM_DB2_RED_S)
#define MCPWM_DB2_RED_V  0x0000FFFFU
#define MCPWM_DB2_RED_S  0

/** MCPWM_CARRIER2_CFG_REG register
 *  Carrier$n configuration register
 */
#define MCPWM_CARRIER2_CFG_REG (DR_REG_MCPWM_BASE + 0xd4)
/** MCPWM_CHOPPER2_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable carrier$n.\\0: Bypassed\\1: Enabled
 */
#define MCPWM_CHOPPER2_EN    (BIT(0))
#define MCPWM_CHOPPER2_EN_M  (MCPWM_CHOPPER2_EN_V << MCPWM_CHOPPER2_EN_S)
#define MCPWM_CHOPPER2_EN_V  0x00000001U
#define MCPWM_CHOPPER2_EN_S  0
/** MCPWM_CHOPPER2_PRESCALE : R/W; bitpos: [4:1]; default: 0;
 *  Configures the prescale value of PWM carrier$n clock (PC_clk), so that period of
 *  PC_clk = period of PWM_clk * (PWM_CARRIER$n_PRESCALE + 1)
 */
#define MCPWM_CHOPPER2_PRESCALE    0x0000000FU
#define MCPWM_CHOPPER2_PRESCALE_M  (MCPWM_CHOPPER2_PRESCALE_V << MCPWM_CHOPPER2_PRESCALE_S)
#define MCPWM_CHOPPER2_PRESCALE_V  0x0000000FU
#define MCPWM_CHOPPER2_PRESCALE_S  1
/** MCPWM_CHOPPER2_DUTY : R/W; bitpos: [7:5]; default: 0;
 *  Configures carrier duty. Duty = PWM_CARRIER$n_DUTY / 8
 */
#define MCPWM_CHOPPER2_DUTY    0x00000007U
#define MCPWM_CHOPPER2_DUTY_M  (MCPWM_CHOPPER2_DUTY_V << MCPWM_CHOPPER2_DUTY_S)
#define MCPWM_CHOPPER2_DUTY_V  0x00000007U
#define MCPWM_CHOPPER2_DUTY_S  5
/** MCPWM_CHOPPER2_OSHTWTH : R/W; bitpos: [11:8]; default: 0;
 *  Configures width of the first pulse. Measurement unit: Periods of the carrier.
 */
#define MCPWM_CHOPPER2_OSHTWTH    0x0000000FU
#define MCPWM_CHOPPER2_OSHTWTH_M  (MCPWM_CHOPPER2_OSHTWTH_V << MCPWM_CHOPPER2_OSHTWTH_S)
#define MCPWM_CHOPPER2_OSHTWTH_V  0x0000000FU
#define MCPWM_CHOPPER2_OSHTWTH_S  8
/** MCPWM_CHOPPER2_OUT_INVERT : R/W; bitpos: [12]; default: 0;
 *  Configures whether or not to invert the output of PWM$n A and PWM$n B for this
 *  submodule.\\0: Normal\\1: Invert
 */
#define MCPWM_CHOPPER2_OUT_INVERT    (BIT(12))
#define MCPWM_CHOPPER2_OUT_INVERT_M  (MCPWM_CHOPPER2_OUT_INVERT_V << MCPWM_CHOPPER2_OUT_INVERT_S)
#define MCPWM_CHOPPER2_OUT_INVERT_V  0x00000001U
#define MCPWM_CHOPPER2_OUT_INVERT_S  12
/** MCPWM_CHOPPER2_IN_INVERT : R/W; bitpos: [13]; default: 0;
 *  Configures whether or not to invert the input of PWM$n A and PWM$n B for this
 *  submodule.\\0: Normal\\1: Invert
 */
#define MCPWM_CHOPPER2_IN_INVERT    (BIT(13))
#define MCPWM_CHOPPER2_IN_INVERT_M  (MCPWM_CHOPPER2_IN_INVERT_V << MCPWM_CHOPPER2_IN_INVERT_S)
#define MCPWM_CHOPPER2_IN_INVERT_V  0x00000001U
#define MCPWM_CHOPPER2_IN_INVERT_S  13

/** MCPWM_FH2_CFG0_REG register
 *  PWM$n A and PWM$n B trip events actions configuration register
 */
#define MCPWM_FH2_CFG0_REG (DR_REG_MCPWM_BASE + 0xd8)
/** MCPWM_TZ2_SW_CBC : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable software force cycle-by-cycle mode action.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_TZ2_SW_CBC    (BIT(0))
#define MCPWM_TZ2_SW_CBC_M  (MCPWM_TZ2_SW_CBC_V << MCPWM_TZ2_SW_CBC_S)
#define MCPWM_TZ2_SW_CBC_V  0x00000001U
#define MCPWM_TZ2_SW_CBC_S  0
/** MCPWM_TZ2_F2_CBC : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not event_f2 will trigger cycle-by-cycle mode action.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_TZ2_F2_CBC    (BIT(1))
#define MCPWM_TZ2_F2_CBC_M  (MCPWM_TZ2_F2_CBC_V << MCPWM_TZ2_F2_CBC_S)
#define MCPWM_TZ2_F2_CBC_V  0x00000001U
#define MCPWM_TZ2_F2_CBC_S  1
/** MCPWM_TZ2_F1_CBC : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not event_f1 will trigger cycle-by-cycle mode action.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_TZ2_F1_CBC    (BIT(2))
#define MCPWM_TZ2_F1_CBC_M  (MCPWM_TZ2_F1_CBC_V << MCPWM_TZ2_F1_CBC_S)
#define MCPWM_TZ2_F1_CBC_V  0x00000001U
#define MCPWM_TZ2_F1_CBC_S  2
/** MCPWM_TZ2_F0_CBC : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not event_f0 will trigger cycle-by-cycle mode action.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_TZ2_F0_CBC    (BIT(3))
#define MCPWM_TZ2_F0_CBC_M  (MCPWM_TZ2_F0_CBC_V << MCPWM_TZ2_F0_CBC_S)
#define MCPWM_TZ2_F0_CBC_V  0x00000001U
#define MCPWM_TZ2_F0_CBC_S  3
/** MCPWM_TZ2_SW_OST : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable software force one-shot mode action.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_TZ2_SW_OST    (BIT(4))
#define MCPWM_TZ2_SW_OST_M  (MCPWM_TZ2_SW_OST_V << MCPWM_TZ2_SW_OST_S)
#define MCPWM_TZ2_SW_OST_V  0x00000001U
#define MCPWM_TZ2_SW_OST_S  4
/** MCPWM_TZ2_F2_OST : R/W; bitpos: [5]; default: 0;
 *  Configures whether or not event_f2 will trigger one-shot mode action.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_TZ2_F2_OST    (BIT(5))
#define MCPWM_TZ2_F2_OST_M  (MCPWM_TZ2_F2_OST_V << MCPWM_TZ2_F2_OST_S)
#define MCPWM_TZ2_F2_OST_V  0x00000001U
#define MCPWM_TZ2_F2_OST_S  5
/** MCPWM_TZ2_F1_OST : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not event_f1 will trigger one-shot mode action.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_TZ2_F1_OST    (BIT(6))
#define MCPWM_TZ2_F1_OST_M  (MCPWM_TZ2_F1_OST_V << MCPWM_TZ2_F1_OST_S)
#define MCPWM_TZ2_F1_OST_V  0x00000001U
#define MCPWM_TZ2_F1_OST_S  6
/** MCPWM_TZ2_F0_OST : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not event_f0 will trigger one-shot mode action.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_TZ2_F0_OST    (BIT(7))
#define MCPWM_TZ2_F0_OST_M  (MCPWM_TZ2_F0_OST_V << MCPWM_TZ2_F0_OST_S)
#define MCPWM_TZ2_F0_OST_V  0x00000001U
#define MCPWM_TZ2_F0_OST_S  7
/** MCPWM_TZ2_A_CBC_D : R/W; bitpos: [9:8]; default: 0;
 *  Configures cycle-by-cycle mode action on PWM$n A when fault event occurs and timer
 *  is decreasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
 */
#define MCPWM_TZ2_A_CBC_D    0x00000003U
#define MCPWM_TZ2_A_CBC_D_M  (MCPWM_TZ2_A_CBC_D_V << MCPWM_TZ2_A_CBC_D_S)
#define MCPWM_TZ2_A_CBC_D_V  0x00000003U
#define MCPWM_TZ2_A_CBC_D_S  8
/** MCPWM_TZ2_A_CBC_U : R/W; bitpos: [11:10]; default: 0;
 *  Configures cycle-by-cycle mode action on PWM$n A when fault event occurs and timer
 *  is increasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
 */
#define MCPWM_TZ2_A_CBC_U    0x00000003U
#define MCPWM_TZ2_A_CBC_U_M  (MCPWM_TZ2_A_CBC_U_V << MCPWM_TZ2_A_CBC_U_S)
#define MCPWM_TZ2_A_CBC_U_V  0x00000003U
#define MCPWM_TZ2_A_CBC_U_S  10
/** MCPWM_TZ2_A_OST_D : R/W; bitpos: [13:12]; default: 0;
 *  Configures one-shot mode action on PWM$n A when fault event occurs and timer is
 *  decreasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
 */
#define MCPWM_TZ2_A_OST_D    0x00000003U
#define MCPWM_TZ2_A_OST_D_M  (MCPWM_TZ2_A_OST_D_V << MCPWM_TZ2_A_OST_D_S)
#define MCPWM_TZ2_A_OST_D_V  0x00000003U
#define MCPWM_TZ2_A_OST_D_S  12
/** MCPWM_TZ2_A_OST_U : R/W; bitpos: [15:14]; default: 0;
 *  Configures one-shot mode action on PWM$n A when fault event occurs and timer is
 *  increasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
 */
#define MCPWM_TZ2_A_OST_U    0x00000003U
#define MCPWM_TZ2_A_OST_U_M  (MCPWM_TZ2_A_OST_U_V << MCPWM_TZ2_A_OST_U_S)
#define MCPWM_TZ2_A_OST_U_V  0x00000003U
#define MCPWM_TZ2_A_OST_U_S  14
/** MCPWM_TZ2_B_CBC_D : R/W; bitpos: [17:16]; default: 0;
 *  Configures cycle-by-cycle mode action on PWM$n B when fault event occurs and timer
 *  is decreasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
 */
#define MCPWM_TZ2_B_CBC_D    0x00000003U
#define MCPWM_TZ2_B_CBC_D_M  (MCPWM_TZ2_B_CBC_D_V << MCPWM_TZ2_B_CBC_D_S)
#define MCPWM_TZ2_B_CBC_D_V  0x00000003U
#define MCPWM_TZ2_B_CBC_D_S  16
/** MCPWM_TZ2_B_CBC_U : R/W; bitpos: [19:18]; default: 0;
 *  Configures cycle-by-cycle mode action on PWM$n B when fault event occurs and timer
 *  is increasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
 */
#define MCPWM_TZ2_B_CBC_U    0x00000003U
#define MCPWM_TZ2_B_CBC_U_M  (MCPWM_TZ2_B_CBC_U_V << MCPWM_TZ2_B_CBC_U_S)
#define MCPWM_TZ2_B_CBC_U_V  0x00000003U
#define MCPWM_TZ2_B_CBC_U_S  18
/** MCPWM_TZ2_B_OST_D : R/W; bitpos: [21:20]; default: 0;
 *  Configures one-shot mode action on PWM$n B when fault event occurs and timer is
 *  decreasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
 */
#define MCPWM_TZ2_B_OST_D    0x00000003U
#define MCPWM_TZ2_B_OST_D_M  (MCPWM_TZ2_B_OST_D_V << MCPWM_TZ2_B_OST_D_S)
#define MCPWM_TZ2_B_OST_D_V  0x00000003U
#define MCPWM_TZ2_B_OST_D_S  20
/** MCPWM_TZ2_B_OST_U : R/W; bitpos: [23:22]; default: 0;
 *  Configures one-shot mode action on PWM$n B when fault event occurs and timer is
 *  increasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
 */
#define MCPWM_TZ2_B_OST_U    0x00000003U
#define MCPWM_TZ2_B_OST_U_M  (MCPWM_TZ2_B_OST_U_V << MCPWM_TZ2_B_OST_U_S)
#define MCPWM_TZ2_B_OST_U_V  0x00000003U
#define MCPWM_TZ2_B_OST_U_S  22

/** MCPWM_FH2_CFG1_REG register
 *  Software triggers for fault handler actions configuration register
 */
#define MCPWM_FH2_CFG1_REG (DR_REG_MCPWM_BASE + 0xdc)
/** MCPWM_TZ2_CLR_OST : R/W; bitpos: [0]; default: 0;
 *  Configures the generation of software one-shot mode action clear. A toggle
 *  (software negate its value) triggers a clear for on going one-shot mode action.
 */
#define MCPWM_TZ2_CLR_OST    (BIT(0))
#define MCPWM_TZ2_CLR_OST_M  (MCPWM_TZ2_CLR_OST_V << MCPWM_TZ2_CLR_OST_S)
#define MCPWM_TZ2_CLR_OST_V  0x00000001U
#define MCPWM_TZ2_CLR_OST_S  0
/** MCPWM_TZ2_CBCPULSE : R/W; bitpos: [2:1]; default: 0;
 *  Configures the refresh moment selection of cycle-by-cycle mode action.\\0: Select
 *  nothing, will not refresh\\Bit0 is set to 1: TEZ\\Bit1 is set to 1: TEP
 */
#define MCPWM_TZ2_CBCPULSE    0x00000003U
#define MCPWM_TZ2_CBCPULSE_M  (MCPWM_TZ2_CBCPULSE_V << MCPWM_TZ2_CBCPULSE_S)
#define MCPWM_TZ2_CBCPULSE_V  0x00000003U
#define MCPWM_TZ2_CBCPULSE_S  1
/** MCPWM_TZ2_FORCE_CBC : R/W; bitpos: [3]; default: 0;
 *  Configures the generation of software cycle-by-cycle mode action. A toggle
 *  (software negate its value) triggers a cycle-by-cycle mode action.
 */
#define MCPWM_TZ2_FORCE_CBC    (BIT(3))
#define MCPWM_TZ2_FORCE_CBC_M  (MCPWM_TZ2_FORCE_CBC_V << MCPWM_TZ2_FORCE_CBC_S)
#define MCPWM_TZ2_FORCE_CBC_V  0x00000001U
#define MCPWM_TZ2_FORCE_CBC_S  3
/** MCPWM_TZ2_FORCE_OST : R/W; bitpos: [4]; default: 0;
 *  Configures the generation of software one-shot mode action. A toggle (software
 *  negate its value) triggers a one-shot mode action.
 */
#define MCPWM_TZ2_FORCE_OST    (BIT(4))
#define MCPWM_TZ2_FORCE_OST_M  (MCPWM_TZ2_FORCE_OST_V << MCPWM_TZ2_FORCE_OST_S)
#define MCPWM_TZ2_FORCE_OST_V  0x00000001U
#define MCPWM_TZ2_FORCE_OST_S  4

/** MCPWM_FH2_STATUS_REG register
 *  Fault events status register
 */
#define MCPWM_FH2_STATUS_REG (DR_REG_MCPWM_BASE + 0xe0)
/** MCPWM_TZ2_CBC_ON : RO; bitpos: [0]; default: 0;
 *  Represents whether or not an cycle-by-cycle mode action is on going.\\0:No
 *  action\\1: On going
 */
#define MCPWM_TZ2_CBC_ON    (BIT(0))
#define MCPWM_TZ2_CBC_ON_M  (MCPWM_TZ2_CBC_ON_V << MCPWM_TZ2_CBC_ON_S)
#define MCPWM_TZ2_CBC_ON_V  0x00000001U
#define MCPWM_TZ2_CBC_ON_S  0
/** MCPWM_TZ2_OST_ON : RO; bitpos: [1]; default: 0;
 *  Represents whether or not an one-shot mode action is on going.\\0:No action\\1: On
 *  going
 */
#define MCPWM_TZ2_OST_ON    (BIT(1))
#define MCPWM_TZ2_OST_ON_M  (MCPWM_TZ2_OST_ON_V << MCPWM_TZ2_OST_ON_S)
#define MCPWM_TZ2_OST_ON_V  0x00000001U
#define MCPWM_TZ2_OST_ON_S  1

/** MCPWM_FAULT_DETECT_REG register
 *  Fault detection configuration and status register
 */
#define MCPWM_FAULT_DETECT_REG (DR_REG_MCPWM_BASE + 0xe4)
/** MCPWM_F0_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable event_f0 generation.\\0: Disable\\1: Enable
 */
#define MCPWM_F0_EN    (BIT(0))
#define MCPWM_F0_EN_M  (MCPWM_F0_EN_V << MCPWM_F0_EN_S)
#define MCPWM_F0_EN_V  0x00000001U
#define MCPWM_F0_EN_S  0
/** MCPWM_F1_EN : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enable event_f1 generation.\\0: Disable\\1: Enable
 */
#define MCPWM_F1_EN    (BIT(1))
#define MCPWM_F1_EN_M  (MCPWM_F1_EN_V << MCPWM_F1_EN_S)
#define MCPWM_F1_EN_V  0x00000001U
#define MCPWM_F1_EN_S  1
/** MCPWM_F2_EN : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable event_f2 generation.\\0: Disable\\1: Enable
 */
#define MCPWM_F2_EN    (BIT(2))
#define MCPWM_F2_EN_M  (MCPWM_F2_EN_V << MCPWM_F2_EN_S)
#define MCPWM_F2_EN_V  0x00000001U
#define MCPWM_F2_EN_S  2
/** MCPWM_F0_POLE : R/W; bitpos: [3]; default: 0;
 *  Configures event_f0 trigger polarity on FAULT0 source from GPIO matrix.\\0: Level
 *  low\\1: Level high
 */
#define MCPWM_F0_POLE    (BIT(3))
#define MCPWM_F0_POLE_M  (MCPWM_F0_POLE_V << MCPWM_F0_POLE_S)
#define MCPWM_F0_POLE_V  0x00000001U
#define MCPWM_F0_POLE_S  3
/** MCPWM_F1_POLE : R/W; bitpos: [4]; default: 0;
 *  Configures event_f1 trigger polarity on FAULT1 source from GPIO matrix.\\0: Level
 *  low\\1: Level high
 */
#define MCPWM_F1_POLE    (BIT(4))
#define MCPWM_F1_POLE_M  (MCPWM_F1_POLE_V << MCPWM_F1_POLE_S)
#define MCPWM_F1_POLE_V  0x00000001U
#define MCPWM_F1_POLE_S  4
/** MCPWM_F2_POLE : R/W; bitpos: [5]; default: 0;
 *  Configures event_f2 trigger polarity on FAULT2 source from GPIO matrix.\\0: Level
 *  low\\1: Level high
 */
#define MCPWM_F2_POLE    (BIT(5))
#define MCPWM_F2_POLE_M  (MCPWM_F2_POLE_V << MCPWM_F2_POLE_S)
#define MCPWM_F2_POLE_V  0x00000001U
#define MCPWM_F2_POLE_S  5
/** MCPWM_EVENT_F0 : RO; bitpos: [6]; default: 0;
 *  Represents whether or not an event_f0 is on going.\\0: No action\\1: On going
 */
#define MCPWM_EVENT_F0    (BIT(6))
#define MCPWM_EVENT_F0_M  (MCPWM_EVENT_F0_V << MCPWM_EVENT_F0_S)
#define MCPWM_EVENT_F0_V  0x00000001U
#define MCPWM_EVENT_F0_S  6
/** MCPWM_EVENT_F1 : RO; bitpos: [7]; default: 0;
 *  Represents whether or not an event_f1 is on going.\\0: No action\\1: On going
 */
#define MCPWM_EVENT_F1    (BIT(7))
#define MCPWM_EVENT_F1_M  (MCPWM_EVENT_F1_V << MCPWM_EVENT_F1_S)
#define MCPWM_EVENT_F1_V  0x00000001U
#define MCPWM_EVENT_F1_S  7
/** MCPWM_EVENT_F2 : RO; bitpos: [8]; default: 0;
 *  Represents whether or not an event_f2 is on going.\\0: No action\\1: On going
 */
#define MCPWM_EVENT_F2    (BIT(8))
#define MCPWM_EVENT_F2_M  (MCPWM_EVENT_F2_V << MCPWM_EVENT_F2_S)
#define MCPWM_EVENT_F2_V  0x00000001U
#define MCPWM_EVENT_F2_S  8

/** MCPWM_CAP_TIMER_CFG_REG register
 *  Capture timer configuration register
 */
#define MCPWM_CAP_TIMER_CFG_REG (DR_REG_MCPWM_BASE + 0xe8)
/** MCPWM_CAP_TIMER_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable capture timer increment.\\0: Disable\\1: Enable
 */
#define MCPWM_CAP_TIMER_EN    (BIT(0))
#define MCPWM_CAP_TIMER_EN_M  (MCPWM_CAP_TIMER_EN_V << MCPWM_CAP_TIMER_EN_S)
#define MCPWM_CAP_TIMER_EN_V  0x00000001U
#define MCPWM_CAP_TIMER_EN_S  0
/** MCPWM_CAP_SYNCI_EN : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enable capture timer sync.\\0: Disable\\1: Enable
 */
#define MCPWM_CAP_SYNCI_EN    (BIT(1))
#define MCPWM_CAP_SYNCI_EN_M  (MCPWM_CAP_SYNCI_EN_V << MCPWM_CAP_SYNCI_EN_S)
#define MCPWM_CAP_SYNCI_EN_V  0x00000001U
#define MCPWM_CAP_SYNCI_EN_S  1
/** MCPWM_CAP_SYNCI_SEL : R/W; bitpos: [4:2]; default: 0;
 *  Configures the selection of capture module sync input.\\0: None\\1: Timer0
 *  sync_out\\2: Timer1 sync_out\\3: Timer2 sync_out\\4: SYNC0 from GPIO matrix\\5:
 *  SYNC1 from GPIO matrix\\6: SYNC2 from GPIO matrix\\7: None
 */
#define MCPWM_CAP_SYNCI_SEL    0x00000007U
#define MCPWM_CAP_SYNCI_SEL_M  (MCPWM_CAP_SYNCI_SEL_V << MCPWM_CAP_SYNCI_SEL_S)
#define MCPWM_CAP_SYNCI_SEL_V  0x00000007U
#define MCPWM_CAP_SYNCI_SEL_S  2
/** MCPWM_CAP_SYNC_SW : WT; bitpos: [5]; default: 0;
 *  Configures the generation of a capture timer sync when reg_cap_synci_en is 1.\\0:
 *  Invalid, No effect\\1: Trigger a capture timer sync, capture timer is loaded with
 *  value in phase register
 */
#define MCPWM_CAP_SYNC_SW    (BIT(5))
#define MCPWM_CAP_SYNC_SW_M  (MCPWM_CAP_SYNC_SW_V << MCPWM_CAP_SYNC_SW_S)
#define MCPWM_CAP_SYNC_SW_V  0x00000001U
#define MCPWM_CAP_SYNC_SW_S  5

/** MCPWM_CAP_TIMER_PHASE_REG register
 *  Capture timer sync phase register
 */
#define MCPWM_CAP_TIMER_PHASE_REG (DR_REG_MCPWM_BASE + 0xec)
/** MCPWM_CAP_PHASE : R/W; bitpos: [31:0]; default: 0;
 *  Configures phase value for capture timer sync operation.
 */
#define MCPWM_CAP_PHASE    0xFFFFFFFFU
#define MCPWM_CAP_PHASE_M  (MCPWM_CAP_PHASE_V << MCPWM_CAP_PHASE_S)
#define MCPWM_CAP_PHASE_V  0xFFFFFFFFU
#define MCPWM_CAP_PHASE_S  0

/** MCPWM_CAP_CH0_CFG_REG register
 *  Capture channel 0 configuration register
 */
#define MCPWM_CAP_CH0_CFG_REG (DR_REG_MCPWM_BASE + 0xf0)
/** MCPWM_CAP0_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable capture on channel 0.\\0: Disable\\1: Enable
 */
#define MCPWM_CAP0_EN    (BIT(0))
#define MCPWM_CAP0_EN_M  (MCPWM_CAP0_EN_V << MCPWM_CAP0_EN_S)
#define MCPWM_CAP0_EN_V  0x00000001U
#define MCPWM_CAP0_EN_S  0
/** MCPWM_CAP0_MODE : R/W; bitpos: [2:1]; default: 0;
 *  Configures which edge of capture on channel 0 after prescaling is used.\\0:
 *  None\\Bit0 is set to 1: Rnable capture on the negative edge\\Bit1 is set to 1:
 *  Enable capture on the positive edge
 */
#define MCPWM_CAP0_MODE    0x00000003U
#define MCPWM_CAP0_MODE_M  (MCPWM_CAP0_MODE_V << MCPWM_CAP0_MODE_S)
#define MCPWM_CAP0_MODE_V  0x00000003U
#define MCPWM_CAP0_MODE_S  1
/** MCPWM_CAP0_PRESCALE : R/W; bitpos: [10:3]; default: 0;
 *  Configures prescale value on possitive edge of CAP0. Prescale value =
 *  PWM_CAP0_PRESCALE + 1
 */
#define MCPWM_CAP0_PRESCALE    0x000000FFU
#define MCPWM_CAP0_PRESCALE_M  (MCPWM_CAP0_PRESCALE_V << MCPWM_CAP0_PRESCALE_S)
#define MCPWM_CAP0_PRESCALE_V  0x000000FFU
#define MCPWM_CAP0_PRESCALE_S  3
/** MCPWM_CAP0_IN_INVERT : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert CAP0 from GPIO matrix before prescale.\\0:
 *  Normal\\1: Invert
 */
#define MCPWM_CAP0_IN_INVERT    (BIT(11))
#define MCPWM_CAP0_IN_INVERT_M  (MCPWM_CAP0_IN_INVERT_V << MCPWM_CAP0_IN_INVERT_S)
#define MCPWM_CAP0_IN_INVERT_V  0x00000001U
#define MCPWM_CAP0_IN_INVERT_S  11
/** MCPWM_CAP0_SW : WT; bitpos: [12]; default: 0;
 *  Configures the generation of software capture.\\0: Invalid, No effect\\1: Trigger a
 *  software forced capture on channel 0
 */
#define MCPWM_CAP0_SW    (BIT(12))
#define MCPWM_CAP0_SW_M  (MCPWM_CAP0_SW_V << MCPWM_CAP0_SW_S)
#define MCPWM_CAP0_SW_V  0x00000001U
#define MCPWM_CAP0_SW_S  12

/** MCPWM_CAP_CH1_CFG_REG register
 *  Capture channel 1 configuration register
 */
#define MCPWM_CAP_CH1_CFG_REG (DR_REG_MCPWM_BASE + 0xf4)
/** MCPWM_CAP0_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable capture on channel 1.\\0: Disable\\1: Enable
 */
#define MCPWM_CAP0_EN    (BIT(0))
#define MCPWM_CAP0_EN_M  (MCPWM_CAP0_EN_V << MCPWM_CAP0_EN_S)
#define MCPWM_CAP0_EN_V  0x00000001U
#define MCPWM_CAP0_EN_S  0
/** MCPWM_CAP0_MODE : R/W; bitpos: [2:1]; default: 0;
 *  Configures which edge of capture on channel 1 after prescaling is used.\\0:
 *  None\\Bit0 is set to 1: Rnable capture on the negative edge\\Bit1 is set to 1:
 *  Enable capture on the positive edge
 */
#define MCPWM_CAP0_MODE    0x00000003U
#define MCPWM_CAP0_MODE_M  (MCPWM_CAP0_MODE_V << MCPWM_CAP0_MODE_S)
#define MCPWM_CAP0_MODE_V  0x00000003U
#define MCPWM_CAP0_MODE_S  1
/** MCPWM_CAP0_PRESCALE : R/W; bitpos: [10:3]; default: 0;
 *  Configures prescale value on possitive edge of CAP1. Prescale value =
 *  PWM_CAP1_PRESCALE + 1
 */
#define MCPWM_CAP0_PRESCALE    0x000000FFU
#define MCPWM_CAP0_PRESCALE_M  (MCPWM_CAP0_PRESCALE_V << MCPWM_CAP0_PRESCALE_S)
#define MCPWM_CAP0_PRESCALE_V  0x000000FFU
#define MCPWM_CAP0_PRESCALE_S  3
/** MCPWM_CAP0_IN_INVERT : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert CAP1 from GPIO matrix before prescale.\\0:
 *  Normal\\1: Invert
 */
#define MCPWM_CAP0_IN_INVERT    (BIT(11))
#define MCPWM_CAP0_IN_INVERT_M  (MCPWM_CAP0_IN_INVERT_V << MCPWM_CAP0_IN_INVERT_S)
#define MCPWM_CAP0_IN_INVERT_V  0x00000001U
#define MCPWM_CAP0_IN_INVERT_S  11
/** MCPWM_CAP0_SW : WT; bitpos: [12]; default: 0;
 *  Configures the generation of software capture.\\0: Invalid, No effect\\1: Trigger a
 *  software forced capture on channel 1
 */
#define MCPWM_CAP0_SW    (BIT(12))
#define MCPWM_CAP0_SW_M  (MCPWM_CAP0_SW_V << MCPWM_CAP0_SW_S)
#define MCPWM_CAP0_SW_V  0x00000001U
#define MCPWM_CAP0_SW_S  12

/** MCPWM_CAP_CH2_CFG_REG register
 *  Capture channel 2 configuration register
 */
#define MCPWM_CAP_CH2_CFG_REG (DR_REG_MCPWM_BASE + 0xf8)
/** MCPWM_CAP0_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable capture on channel 2.\\0: Disable\\1: Enable
 */
#define MCPWM_CAP0_EN    (BIT(0))
#define MCPWM_CAP0_EN_M  (MCPWM_CAP0_EN_V << MCPWM_CAP0_EN_S)
#define MCPWM_CAP0_EN_V  0x00000001U
#define MCPWM_CAP0_EN_S  0
/** MCPWM_CAP0_MODE : R/W; bitpos: [2:1]; default: 0;
 *  Configures which edge of capture on channel 2 after prescaling is used.\\0:
 *  None\\Bit0 is set to 1: Rnable capture on the negative edge\\Bit1 is set to 1:
 *  Enable capture on the positive edge
 */
#define MCPWM_CAP0_MODE    0x00000003U
#define MCPWM_CAP0_MODE_M  (MCPWM_CAP0_MODE_V << MCPWM_CAP0_MODE_S)
#define MCPWM_CAP0_MODE_V  0x00000003U
#define MCPWM_CAP0_MODE_S  1
/** MCPWM_CAP0_PRESCALE : R/W; bitpos: [10:3]; default: 0;
 *  Configures prescale value on possitive edge of CAP2. Prescale value =
 *  PWM_CAP2_PRESCALE + 1
 */
#define MCPWM_CAP0_PRESCALE    0x000000FFU
#define MCPWM_CAP0_PRESCALE_M  (MCPWM_CAP0_PRESCALE_V << MCPWM_CAP0_PRESCALE_S)
#define MCPWM_CAP0_PRESCALE_V  0x000000FFU
#define MCPWM_CAP0_PRESCALE_S  3
/** MCPWM_CAP0_IN_INVERT : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to invert CAP2 from GPIO matrix before prescale.\\0:
 *  Normal\\1: Invert
 */
#define MCPWM_CAP0_IN_INVERT    (BIT(11))
#define MCPWM_CAP0_IN_INVERT_M  (MCPWM_CAP0_IN_INVERT_V << MCPWM_CAP0_IN_INVERT_S)
#define MCPWM_CAP0_IN_INVERT_V  0x00000001U
#define MCPWM_CAP0_IN_INVERT_S  11
/** MCPWM_CAP0_SW : WT; bitpos: [12]; default: 0;
 *  Configures the generation of software capture.\\0: Invalid, No effect\\1: Trigger a
 *  software forced capture on channel 2
 */
#define MCPWM_CAP0_SW    (BIT(12))
#define MCPWM_CAP0_SW_M  (MCPWM_CAP0_SW_V << MCPWM_CAP0_SW_S)
#define MCPWM_CAP0_SW_V  0x00000001U
#define MCPWM_CAP0_SW_S  12

/** MCPWM_CAP_CH0_REG register
 *  CAP0 capture value register
 */
#define MCPWM_CAP_CH0_REG (DR_REG_MCPWM_BASE + 0xfc)
/** MCPWM_CAP0_VALUE : RO; bitpos: [31:0]; default: 0;
 *  Represents value of last capture on CAP0
 */
#define MCPWM_CAP0_VALUE    0xFFFFFFFFU
#define MCPWM_CAP0_VALUE_M  (MCPWM_CAP0_VALUE_V << MCPWM_CAP0_VALUE_S)
#define MCPWM_CAP0_VALUE_V  0xFFFFFFFFU
#define MCPWM_CAP0_VALUE_S  0

/** MCPWM_CAP_CH1_REG register
 *  CAP1 capture value register
 */
#define MCPWM_CAP_CH1_REG (DR_REG_MCPWM_BASE + 0x100)
/** MCPWM_CAP0_VALUE : RO; bitpos: [31:0]; default: 0;
 *  Represents value of last capture on CAP1
 */
#define MCPWM_CAP0_VALUE    0xFFFFFFFFU
#define MCPWM_CAP0_VALUE_M  (MCPWM_CAP0_VALUE_V << MCPWM_CAP0_VALUE_S)
#define MCPWM_CAP0_VALUE_V  0xFFFFFFFFU
#define MCPWM_CAP0_VALUE_S  0

/** MCPWM_CAP_CH2_REG register
 *  CAP2 capture value register
 */
#define MCPWM_CAP_CH2_REG (DR_REG_MCPWM_BASE + 0x104)
/** MCPWM_CAP0_VALUE : RO; bitpos: [31:0]; default: 0;
 *  Represents value of last capture on CAP2
 */
#define MCPWM_CAP0_VALUE    0xFFFFFFFFU
#define MCPWM_CAP0_VALUE_M  (MCPWM_CAP0_VALUE_V << MCPWM_CAP0_VALUE_S)
#define MCPWM_CAP0_VALUE_V  0xFFFFFFFFU
#define MCPWM_CAP0_VALUE_S  0

/** MCPWM_CAP_STATUS_REG register
 *  Last capture trigger edge information register
 */
#define MCPWM_CAP_STATUS_REG (DR_REG_MCPWM_BASE + 0x108)
/** MCPWM_CAP0_EDGE : RO; bitpos: [0]; default: 0;
 *  Represents edge of last capture trigger on channel0.\\0: Posedge\\1: Negedge
 */
#define MCPWM_CAP0_EDGE    (BIT(0))
#define MCPWM_CAP0_EDGE_M  (MCPWM_CAP0_EDGE_V << MCPWM_CAP0_EDGE_S)
#define MCPWM_CAP0_EDGE_V  0x00000001U
#define MCPWM_CAP0_EDGE_S  0
/** MCPWM_CAP1_EDGE : RO; bitpos: [1]; default: 0;
 *  Represents edge of last capture trigger on channel1.\\0: Posedge\\1: Negedge
 */
#define MCPWM_CAP1_EDGE    (BIT(1))
#define MCPWM_CAP1_EDGE_M  (MCPWM_CAP1_EDGE_V << MCPWM_CAP1_EDGE_S)
#define MCPWM_CAP1_EDGE_V  0x00000001U
#define MCPWM_CAP1_EDGE_S  1
/** MCPWM_CAP2_EDGE : RO; bitpos: [2]; default: 0;
 *  Represents edge of last capture trigger on channel2.\\0: Posedge\\1: Negedge
 */
#define MCPWM_CAP2_EDGE    (BIT(2))
#define MCPWM_CAP2_EDGE_M  (MCPWM_CAP2_EDGE_V << MCPWM_CAP2_EDGE_S)
#define MCPWM_CAP2_EDGE_V  0x00000001U
#define MCPWM_CAP2_EDGE_S  2

/** MCPWM_UPDATE_CFG_REG register
 *  Generator Update configuration register
 */
#define MCPWM_UPDATE_CFG_REG (DR_REG_MCPWM_BASE + 0x10c)
/** MCPWM_GLOBAL_UP_EN : R/W; bitpos: [0]; default: 1;
 *  Configures whether or not to enable global update for all active registers in MCPWM
 *  module.\\0: Disable\\1: Enable
 */
#define MCPWM_GLOBAL_UP_EN    (BIT(0))
#define MCPWM_GLOBAL_UP_EN_M  (MCPWM_GLOBAL_UP_EN_V << MCPWM_GLOBAL_UP_EN_S)
#define MCPWM_GLOBAL_UP_EN_V  0x00000001U
#define MCPWM_GLOBAL_UP_EN_S  0
/** MCPWM_GLOBAL_FORCE_UP : R/W; bitpos: [1]; default: 0;
 *  Configures the generation of global forced update for all active registers in MCPWM
 *  module. A toggle (software invert its value) will trigger a global forced update.
 *  Valid only when MCPWM_GLOBAL_UP_EN and MCPWM_OP0/1/2_UP_EN are both set to 1.
 */
#define MCPWM_GLOBAL_FORCE_UP    (BIT(1))
#define MCPWM_GLOBAL_FORCE_UP_M  (MCPWM_GLOBAL_FORCE_UP_V << MCPWM_GLOBAL_FORCE_UP_S)
#define MCPWM_GLOBAL_FORCE_UP_V  0x00000001U
#define MCPWM_GLOBAL_FORCE_UP_S  1
/** MCPWM_OP0_UP_EN : R/W; bitpos: [2]; default: 1;
 *  Configures whether or not to enable update of active registers in PWM operator$n.
 *  Valid only when PWM_GLOBAL_UP_EN is set to 1.\\0: Disable\\1: Enable
 */
#define MCPWM_OP0_UP_EN    (BIT(2))
#define MCPWM_OP0_UP_EN_M  (MCPWM_OP0_UP_EN_V << MCPWM_OP0_UP_EN_S)
#define MCPWM_OP0_UP_EN_V  0x00000001U
#define MCPWM_OP0_UP_EN_S  2
/** MCPWM_OP0_FORCE_UP : R/W; bitpos: [3]; default: 0;
 *  Configures the generation of forced update for active registers in PWM operator0. A
 *  toggle (software invert its value) will trigger a forced update. Valid only when
 *  MCPWM_GLOBAL_UP_EN and MCPWM_OP0_UP_EN are both set to 1.
 */
#define MCPWM_OP0_FORCE_UP    (BIT(3))
#define MCPWM_OP0_FORCE_UP_M  (MCPWM_OP0_FORCE_UP_V << MCPWM_OP0_FORCE_UP_S)
#define MCPWM_OP0_FORCE_UP_V  0x00000001U
#define MCPWM_OP0_FORCE_UP_S  3
/** MCPWM_OP1_UP_EN : R/W; bitpos: [4]; default: 1;
 *  Configures whether or not to enable update of active registers in PWM operator$n.
 *  Valid only when PWM_GLOBAL_UP_EN is set to 1.\\0: Disable\\1: Enable
 */
#define MCPWM_OP1_UP_EN    (BIT(4))
#define MCPWM_OP1_UP_EN_M  (MCPWM_OP1_UP_EN_V << MCPWM_OP1_UP_EN_S)
#define MCPWM_OP1_UP_EN_V  0x00000001U
#define MCPWM_OP1_UP_EN_S  4
/** MCPWM_OP1_FORCE_UP : R/W; bitpos: [5]; default: 0;
 *  Configures the generation of forced update for active registers in PWM operator1. A
 *  toggle (software invert its value) will trigger a forced update. Valid only when
 *  MCPWM_GLOBAL_UP_EN and MCPWM_OP1_UP_EN are both set to 1.
 */
#define MCPWM_OP1_FORCE_UP    (BIT(5))
#define MCPWM_OP1_FORCE_UP_M  (MCPWM_OP1_FORCE_UP_V << MCPWM_OP1_FORCE_UP_S)
#define MCPWM_OP1_FORCE_UP_V  0x00000001U
#define MCPWM_OP1_FORCE_UP_S  5
/** MCPWM_OP2_UP_EN : R/W; bitpos: [6]; default: 1;
 *  Configures whether or not to enable update of active registers in PWM operator$n.
 *  Valid only when PWM_GLOBAL_UP_EN is set to 1.\\0: Disable\\1: Enable
 */
#define MCPWM_OP2_UP_EN    (BIT(6))
#define MCPWM_OP2_UP_EN_M  (MCPWM_OP2_UP_EN_V << MCPWM_OP2_UP_EN_S)
#define MCPWM_OP2_UP_EN_V  0x00000001U
#define MCPWM_OP2_UP_EN_S  6
/** MCPWM_OP2_FORCE_UP : R/W; bitpos: [7]; default: 0;
 *  Configures the generation of forced update for active registers in PWM operator2. A
 *  toggle (software invert its value) will trigger a forced update. Valid only when
 *  MCPWM_GLOBAL_UP_EN and MCPWM_OP2_UP_EN are both set to 1.
 */
#define MCPWM_OP2_FORCE_UP    (BIT(7))
#define MCPWM_OP2_FORCE_UP_M  (MCPWM_OP2_FORCE_UP_V << MCPWM_OP2_FORCE_UP_S)
#define MCPWM_OP2_FORCE_UP_V  0x00000001U
#define MCPWM_OP2_FORCE_UP_S  7

/** MCPWM_INT_ENA_REG register
 *  Interrupt enable register
 */
#define MCPWM_INT_ENA_REG (DR_REG_MCPWM_BASE + 0x110)
/** MCPWM_TIMER0_STOP_INT_ENA : R/W; bitpos: [0]; default: 0;
 *  Enable bit: Write 1 to enable the interrupt triggered when the timer 0 stops.
 */
#define MCPWM_TIMER0_STOP_INT_ENA    (BIT(0))
#define MCPWM_TIMER0_STOP_INT_ENA_M  (MCPWM_TIMER0_STOP_INT_ENA_V << MCPWM_TIMER0_STOP_INT_ENA_S)
#define MCPWM_TIMER0_STOP_INT_ENA_V  0x00000001U
#define MCPWM_TIMER0_STOP_INT_ENA_S  0
/** MCPWM_TIMER1_STOP_INT_ENA : R/W; bitpos: [1]; default: 0;
 *  Enable bit: Write 1 to enable the interrupt triggered when the timer 1 stops.
 */
#define MCPWM_TIMER1_STOP_INT_ENA    (BIT(1))
#define MCPWM_TIMER1_STOP_INT_ENA_M  (MCPWM_TIMER1_STOP_INT_ENA_V << MCPWM_TIMER1_STOP_INT_ENA_S)
#define MCPWM_TIMER1_STOP_INT_ENA_V  0x00000001U
#define MCPWM_TIMER1_STOP_INT_ENA_S  1
/** MCPWM_TIMER2_STOP_INT_ENA : R/W; bitpos: [2]; default: 0;
 *  Enable bit: Write 1 to enable the interrupt triggered when the timer 2 stops.
 */
#define MCPWM_TIMER2_STOP_INT_ENA    (BIT(2))
#define MCPWM_TIMER2_STOP_INT_ENA_M  (MCPWM_TIMER2_STOP_INT_ENA_V << MCPWM_TIMER2_STOP_INT_ENA_S)
#define MCPWM_TIMER2_STOP_INT_ENA_V  0x00000001U
#define MCPWM_TIMER2_STOP_INT_ENA_S  2
/** MCPWM_TIMER0_TEZ_INT_ENA : R/W; bitpos: [3]; default: 0;
 *  Enable bit: Write 1 to enable the interrupt triggered by a PWM timer 0 TEZ event.
 */
#define MCPWM_TIMER0_TEZ_INT_ENA    (BIT(3))
#define MCPWM_TIMER0_TEZ_INT_ENA_M  (MCPWM_TIMER0_TEZ_INT_ENA_V << MCPWM_TIMER0_TEZ_INT_ENA_S)
#define MCPWM_TIMER0_TEZ_INT_ENA_V  0x00000001U
#define MCPWM_TIMER0_TEZ_INT_ENA_S  3
/** MCPWM_TIMER1_TEZ_INT_ENA : R/W; bitpos: [4]; default: 0;
 *  Enable bit: Write 1 to enable the interrupt triggered by a PWM timer 1 TEZ event.
 */
#define MCPWM_TIMER1_TEZ_INT_ENA    (BIT(4))
#define MCPWM_TIMER1_TEZ_INT_ENA_M  (MCPWM_TIMER1_TEZ_INT_ENA_V << MCPWM_TIMER1_TEZ_INT_ENA_S)
#define MCPWM_TIMER1_TEZ_INT_ENA_V  0x00000001U
#define MCPWM_TIMER1_TEZ_INT_ENA_S  4
/** MCPWM_TIMER2_TEZ_INT_ENA : R/W; bitpos: [5]; default: 0;
 *  Enable bit: Write 1 to enable the interrupt triggered by a PWM timer 2 TEZ event.
 */
#define MCPWM_TIMER2_TEZ_INT_ENA    (BIT(5))
#define MCPWM_TIMER2_TEZ_INT_ENA_M  (MCPWM_TIMER2_TEZ_INT_ENA_V << MCPWM_TIMER2_TEZ_INT_ENA_S)
#define MCPWM_TIMER2_TEZ_INT_ENA_V  0x00000001U
#define MCPWM_TIMER2_TEZ_INT_ENA_S  5
/** MCPWM_TIMER0_TEP_INT_ENA : R/W; bitpos: [6]; default: 0;
 *  Enable bit: Write 1 to enable the interrupt triggered by a PWM timer 0 TEP event.
 */
#define MCPWM_TIMER0_TEP_INT_ENA    (BIT(6))
#define MCPWM_TIMER0_TEP_INT_ENA_M  (MCPWM_TIMER0_TEP_INT_ENA_V << MCPWM_TIMER0_TEP_INT_ENA_S)
#define MCPWM_TIMER0_TEP_INT_ENA_V  0x00000001U
#define MCPWM_TIMER0_TEP_INT_ENA_S  6
/** MCPWM_TIMER1_TEP_INT_ENA : R/W; bitpos: [7]; default: 0;
 *  Enable bit: Write 1 to enable the interrupt triggered by a PWM timer 1 TEP event.
 */
#define MCPWM_TIMER1_TEP_INT_ENA    (BIT(7))
#define MCPWM_TIMER1_TEP_INT_ENA_M  (MCPWM_TIMER1_TEP_INT_ENA_V << MCPWM_TIMER1_TEP_INT_ENA_S)
#define MCPWM_TIMER1_TEP_INT_ENA_V  0x00000001U
#define MCPWM_TIMER1_TEP_INT_ENA_S  7
/** MCPWM_TIMER2_TEP_INT_ENA : R/W; bitpos: [8]; default: 0;
 *  Enable bit: Write 1 to enable the interrupt triggered by a PWM timer 2 TEP event.
 */
#define MCPWM_TIMER2_TEP_INT_ENA    (BIT(8))
#define MCPWM_TIMER2_TEP_INT_ENA_M  (MCPWM_TIMER2_TEP_INT_ENA_V << MCPWM_TIMER2_TEP_INT_ENA_S)
#define MCPWM_TIMER2_TEP_INT_ENA_V  0x00000001U
#define MCPWM_TIMER2_TEP_INT_ENA_S  8
/** MCPWM_FAULT0_INT_ENA : R/W; bitpos: [9]; default: 0;
 *  Enable bit: Write 1 to enable the interrupt triggered when event_f0 starts.
 */
#define MCPWM_FAULT0_INT_ENA    (BIT(9))
#define MCPWM_FAULT0_INT_ENA_M  (MCPWM_FAULT0_INT_ENA_V << MCPWM_FAULT0_INT_ENA_S)
#define MCPWM_FAULT0_INT_ENA_V  0x00000001U
#define MCPWM_FAULT0_INT_ENA_S  9
/** MCPWM_FAULT1_INT_ENA : R/W; bitpos: [10]; default: 0;
 *  Enable bit: Write 1 to enable the interrupt triggered when event_f1 starts.
 */
#define MCPWM_FAULT1_INT_ENA    (BIT(10))
#define MCPWM_FAULT1_INT_ENA_M  (MCPWM_FAULT1_INT_ENA_V << MCPWM_FAULT1_INT_ENA_S)
#define MCPWM_FAULT1_INT_ENA_V  0x00000001U
#define MCPWM_FAULT1_INT_ENA_S  10
/** MCPWM_FAULT2_INT_ENA : R/W; bitpos: [11]; default: 0;
 *  Enable bit: Write 1 to enable the interrupt triggered when event_f2 starts.
 */
#define MCPWM_FAULT2_INT_ENA    (BIT(11))
#define MCPWM_FAULT2_INT_ENA_M  (MCPWM_FAULT2_INT_ENA_V << MCPWM_FAULT2_INT_ENA_S)
#define MCPWM_FAULT2_INT_ENA_V  0x00000001U
#define MCPWM_FAULT2_INT_ENA_S  11
/** MCPWM_FAULT0_CLR_INT_ENA : R/W; bitpos: [12]; default: 0;
 *  Enable bit: Write 1 to enable the interrupt triggered when event_f0 clears.
 */
#define MCPWM_FAULT0_CLR_INT_ENA    (BIT(12))
#define MCPWM_FAULT0_CLR_INT_ENA_M  (MCPWM_FAULT0_CLR_INT_ENA_V << MCPWM_FAULT0_CLR_INT_ENA_S)
#define MCPWM_FAULT0_CLR_INT_ENA_V  0x00000001U
#define MCPWM_FAULT0_CLR_INT_ENA_S  12
/** MCPWM_FAULT1_CLR_INT_ENA : R/W; bitpos: [13]; default: 0;
 *  Enable bit: Write 1 to enable the interrupt triggered when event_f1 clears.
 */
#define MCPWM_FAULT1_CLR_INT_ENA    (BIT(13))
#define MCPWM_FAULT1_CLR_INT_ENA_M  (MCPWM_FAULT1_CLR_INT_ENA_V << MCPWM_FAULT1_CLR_INT_ENA_S)
#define MCPWM_FAULT1_CLR_INT_ENA_V  0x00000001U
#define MCPWM_FAULT1_CLR_INT_ENA_S  13
/** MCPWM_FAULT2_CLR_INT_ENA : R/W; bitpos: [14]; default: 0;
 *  Enable bit: Write 1 to enable the interrupt triggered when event_f2 clears.
 */
#define MCPWM_FAULT2_CLR_INT_ENA    (BIT(14))
#define MCPWM_FAULT2_CLR_INT_ENA_M  (MCPWM_FAULT2_CLR_INT_ENA_V << MCPWM_FAULT2_CLR_INT_ENA_S)
#define MCPWM_FAULT2_CLR_INT_ENA_V  0x00000001U
#define MCPWM_FAULT2_CLR_INT_ENA_S  14
/** MCPWM_CMPR0_TEA_INT_ENA : R/W; bitpos: [15]; default: 0;
 *  Enable bit: Write 1 to enable the interrupt triggered by a PWM operator 0 TEA event.
 */
#define MCPWM_CMPR0_TEA_INT_ENA    (BIT(15))
#define MCPWM_CMPR0_TEA_INT_ENA_M  (MCPWM_CMPR0_TEA_INT_ENA_V << MCPWM_CMPR0_TEA_INT_ENA_S)
#define MCPWM_CMPR0_TEA_INT_ENA_V  0x00000001U
#define MCPWM_CMPR0_TEA_INT_ENA_S  15
/** MCPWM_CMPR1_TEA_INT_ENA : R/W; bitpos: [16]; default: 0;
 *  Enable bit: Write 1 to enable the interrupt triggered by a PWM operator 1 TEA event.
 */
#define MCPWM_CMPR1_TEA_INT_ENA    (BIT(16))
#define MCPWM_CMPR1_TEA_INT_ENA_M  (MCPWM_CMPR1_TEA_INT_ENA_V << MCPWM_CMPR1_TEA_INT_ENA_S)
#define MCPWM_CMPR1_TEA_INT_ENA_V  0x00000001U
#define MCPWM_CMPR1_TEA_INT_ENA_S  16
/** MCPWM_CMPR2_TEA_INT_ENA : R/W; bitpos: [17]; default: 0;
 *  Enable bit: Write 1 to enable the interrupt triggered by a PWM operator 2 TEA event.
 */
#define MCPWM_CMPR2_TEA_INT_ENA    (BIT(17))
#define MCPWM_CMPR2_TEA_INT_ENA_M  (MCPWM_CMPR2_TEA_INT_ENA_V << MCPWM_CMPR2_TEA_INT_ENA_S)
#define MCPWM_CMPR2_TEA_INT_ENA_V  0x00000001U
#define MCPWM_CMPR2_TEA_INT_ENA_S  17
/** MCPWM_CMPR0_TEB_INT_ENA : R/W; bitpos: [18]; default: 0;
 *  Enable bit: Write 1 to enable the interrupt triggered by a PWM operator 0 TEB event.
 */
#define MCPWM_CMPR0_TEB_INT_ENA    (BIT(18))
#define MCPWM_CMPR0_TEB_INT_ENA_M  (MCPWM_CMPR0_TEB_INT_ENA_V << MCPWM_CMPR0_TEB_INT_ENA_S)
#define MCPWM_CMPR0_TEB_INT_ENA_V  0x00000001U
#define MCPWM_CMPR0_TEB_INT_ENA_S  18
/** MCPWM_CMPR1_TEB_INT_ENA : R/W; bitpos: [19]; default: 0;
 *  Enable bit: Write 1 to enable the interrupt triggered by a PWM operator 1 TEB event.
 */
#define MCPWM_CMPR1_TEB_INT_ENA    (BIT(19))
#define MCPWM_CMPR1_TEB_INT_ENA_M  (MCPWM_CMPR1_TEB_INT_ENA_V << MCPWM_CMPR1_TEB_INT_ENA_S)
#define MCPWM_CMPR1_TEB_INT_ENA_V  0x00000001U
#define MCPWM_CMPR1_TEB_INT_ENA_S  19
/** MCPWM_CMPR2_TEB_INT_ENA : R/W; bitpos: [20]; default: 0;
 *  Enable bit: Write 1 to enable the interrupt triggered by a PWM operator 2 TEB event.
 */
#define MCPWM_CMPR2_TEB_INT_ENA    (BIT(20))
#define MCPWM_CMPR2_TEB_INT_ENA_M  (MCPWM_CMPR2_TEB_INT_ENA_V << MCPWM_CMPR2_TEB_INT_ENA_S)
#define MCPWM_CMPR2_TEB_INT_ENA_V  0x00000001U
#define MCPWM_CMPR2_TEB_INT_ENA_S  20
/** MCPWM_TZ0_CBC_INT_ENA : R/W; bitpos: [21]; default: 0;
 *  Enable bit: Write 1 to enable the interrupt triggered by a cycle-by-cycle mode
 *  action on PWM0.
 */
#define MCPWM_TZ0_CBC_INT_ENA    (BIT(21))
#define MCPWM_TZ0_CBC_INT_ENA_M  (MCPWM_TZ0_CBC_INT_ENA_V << MCPWM_TZ0_CBC_INT_ENA_S)
#define MCPWM_TZ0_CBC_INT_ENA_V  0x00000001U
#define MCPWM_TZ0_CBC_INT_ENA_S  21
/** MCPWM_TZ1_CBC_INT_ENA : R/W; bitpos: [22]; default: 0;
 *  Enable bit: Write 1 to enable the interrupt triggered by a cycle-by-cycle mode
 *  action on PWM1.
 */
#define MCPWM_TZ1_CBC_INT_ENA    (BIT(22))
#define MCPWM_TZ1_CBC_INT_ENA_M  (MCPWM_TZ1_CBC_INT_ENA_V << MCPWM_TZ1_CBC_INT_ENA_S)
#define MCPWM_TZ1_CBC_INT_ENA_V  0x00000001U
#define MCPWM_TZ1_CBC_INT_ENA_S  22
/** MCPWM_TZ2_CBC_INT_ENA : R/W; bitpos: [23]; default: 0;
 *  Enable bit: Write 1 to enable the interrupt triggered by a cycle-by-cycle mode
 *  action on PWM2.
 */
#define MCPWM_TZ2_CBC_INT_ENA    (BIT(23))
#define MCPWM_TZ2_CBC_INT_ENA_M  (MCPWM_TZ2_CBC_INT_ENA_V << MCPWM_TZ2_CBC_INT_ENA_S)
#define MCPWM_TZ2_CBC_INT_ENA_V  0x00000001U
#define MCPWM_TZ2_CBC_INT_ENA_S  23
/** MCPWM_TZ0_OST_INT_ENA : R/W; bitpos: [24]; default: 0;
 *  Enable bit: Write 1 to enable the interrupt triggered by a one-shot mode action on
 *  PWM0.
 */
#define MCPWM_TZ0_OST_INT_ENA    (BIT(24))
#define MCPWM_TZ0_OST_INT_ENA_M  (MCPWM_TZ0_OST_INT_ENA_V << MCPWM_TZ0_OST_INT_ENA_S)
#define MCPWM_TZ0_OST_INT_ENA_V  0x00000001U
#define MCPWM_TZ0_OST_INT_ENA_S  24
/** MCPWM_TZ1_OST_INT_ENA : R/W; bitpos: [25]; default: 0;
 *  Enable bit: Write 1 to enable the interrupt triggered by a one-shot mode action on
 *  PWM1.
 */
#define MCPWM_TZ1_OST_INT_ENA    (BIT(25))
#define MCPWM_TZ1_OST_INT_ENA_M  (MCPWM_TZ1_OST_INT_ENA_V << MCPWM_TZ1_OST_INT_ENA_S)
#define MCPWM_TZ1_OST_INT_ENA_V  0x00000001U
#define MCPWM_TZ1_OST_INT_ENA_S  25
/** MCPWM_TZ2_OST_INT_ENA : R/W; bitpos: [26]; default: 0;
 *  Enable bit: Write 1 to enable the interrupt triggered by a one-shot mode action on
 *  PWM2.
 */
#define MCPWM_TZ2_OST_INT_ENA    (BIT(26))
#define MCPWM_TZ2_OST_INT_ENA_M  (MCPWM_TZ2_OST_INT_ENA_V << MCPWM_TZ2_OST_INT_ENA_S)
#define MCPWM_TZ2_OST_INT_ENA_V  0x00000001U
#define MCPWM_TZ2_OST_INT_ENA_S  26
/** MCPWM_CAP0_INT_ENA : R/W; bitpos: [27]; default: 0;
 *  Enable bit: Write 1 to enable the interrupt triggered by capture on CAP0.
 */
#define MCPWM_CAP0_INT_ENA    (BIT(27))
#define MCPWM_CAP0_INT_ENA_M  (MCPWM_CAP0_INT_ENA_V << MCPWM_CAP0_INT_ENA_S)
#define MCPWM_CAP0_INT_ENA_V  0x00000001U
#define MCPWM_CAP0_INT_ENA_S  27
/** MCPWM_CAP1_INT_ENA : R/W; bitpos: [28]; default: 0;
 *  Enable bit: Write 1 to enable the interrupt triggered by capture on CAP1.
 */
#define MCPWM_CAP1_INT_ENA    (BIT(28))
#define MCPWM_CAP1_INT_ENA_M  (MCPWM_CAP1_INT_ENA_V << MCPWM_CAP1_INT_ENA_S)
#define MCPWM_CAP1_INT_ENA_V  0x00000001U
#define MCPWM_CAP1_INT_ENA_S  28
/** MCPWM_CAP2_INT_ENA : R/W; bitpos: [29]; default: 0;
 *  Enable bit: Write 1 to enable the interrupt triggered by capture on CAP2.
 */
#define MCPWM_CAP2_INT_ENA    (BIT(29))
#define MCPWM_CAP2_INT_ENA_M  (MCPWM_CAP2_INT_ENA_V << MCPWM_CAP2_INT_ENA_S)
#define MCPWM_CAP2_INT_ENA_V  0x00000001U
#define MCPWM_CAP2_INT_ENA_S  29

/** MCPWM_INT_RAW_REG register
 *  Interrupt raw status register
 */
#define MCPWM_INT_RAW_REG (DR_REG_MCPWM_BASE + 0x114)
/** MCPWM_TIMER0_STOP_INT_RAW : R/WTC/SS; bitpos: [0]; default: 0;
 *  Raw status bit: The raw interrupt status of the interrupt triggered when the timer
 *  0 stops.
 */
#define MCPWM_TIMER0_STOP_INT_RAW    (BIT(0))
#define MCPWM_TIMER0_STOP_INT_RAW_M  (MCPWM_TIMER0_STOP_INT_RAW_V << MCPWM_TIMER0_STOP_INT_RAW_S)
#define MCPWM_TIMER0_STOP_INT_RAW_V  0x00000001U
#define MCPWM_TIMER0_STOP_INT_RAW_S  0
/** MCPWM_TIMER1_STOP_INT_RAW : R/WTC/SS; bitpos: [1]; default: 0;
 *  Raw status bit: The raw interrupt status of the interrupt triggered when the timer
 *  1 stops.
 */
#define MCPWM_TIMER1_STOP_INT_RAW    (BIT(1))
#define MCPWM_TIMER1_STOP_INT_RAW_M  (MCPWM_TIMER1_STOP_INT_RAW_V << MCPWM_TIMER1_STOP_INT_RAW_S)
#define MCPWM_TIMER1_STOP_INT_RAW_V  0x00000001U
#define MCPWM_TIMER1_STOP_INT_RAW_S  1
/** MCPWM_TIMER2_STOP_INT_RAW : R/WTC/SS; bitpos: [2]; default: 0;
 *  Raw status bit: The raw interrupt status of the interrupt triggered when the timer
 *  2 stops.
 */
#define MCPWM_TIMER2_STOP_INT_RAW    (BIT(2))
#define MCPWM_TIMER2_STOP_INT_RAW_M  (MCPWM_TIMER2_STOP_INT_RAW_V << MCPWM_TIMER2_STOP_INT_RAW_S)
#define MCPWM_TIMER2_STOP_INT_RAW_V  0x00000001U
#define MCPWM_TIMER2_STOP_INT_RAW_S  2
/** MCPWM_TIMER0_TEZ_INT_RAW : R/WTC/SS; bitpos: [3]; default: 0;
 *  Raw status bit: The raw interrupt status of the interrupt triggered by a PWM timer
 *  0 TEZ event.
 */
#define MCPWM_TIMER0_TEZ_INT_RAW    (BIT(3))
#define MCPWM_TIMER0_TEZ_INT_RAW_M  (MCPWM_TIMER0_TEZ_INT_RAW_V << MCPWM_TIMER0_TEZ_INT_RAW_S)
#define MCPWM_TIMER0_TEZ_INT_RAW_V  0x00000001U
#define MCPWM_TIMER0_TEZ_INT_RAW_S  3
/** MCPWM_TIMER1_TEZ_INT_RAW : R/WTC/SS; bitpos: [4]; default: 0;
 *  Raw status bit: The raw interrupt status of the interrupt triggered by a PWM timer
 *  1 TEZ event.
 */
#define MCPWM_TIMER1_TEZ_INT_RAW    (BIT(4))
#define MCPWM_TIMER1_TEZ_INT_RAW_M  (MCPWM_TIMER1_TEZ_INT_RAW_V << MCPWM_TIMER1_TEZ_INT_RAW_S)
#define MCPWM_TIMER1_TEZ_INT_RAW_V  0x00000001U
#define MCPWM_TIMER1_TEZ_INT_RAW_S  4
/** MCPWM_TIMER2_TEZ_INT_RAW : R/WTC/SS; bitpos: [5]; default: 0;
 *  Raw status bit: The raw interrupt status of the interrupt triggered by a PWM timer
 *  2 TEZ event.
 */
#define MCPWM_TIMER2_TEZ_INT_RAW    (BIT(5))
#define MCPWM_TIMER2_TEZ_INT_RAW_M  (MCPWM_TIMER2_TEZ_INT_RAW_V << MCPWM_TIMER2_TEZ_INT_RAW_S)
#define MCPWM_TIMER2_TEZ_INT_RAW_V  0x00000001U
#define MCPWM_TIMER2_TEZ_INT_RAW_S  5
/** MCPWM_TIMER0_TEP_INT_RAW : R/WTC/SS; bitpos: [6]; default: 0;
 *  Raw status bit: The raw interrupt status of the interrupt triggered by a PWM timer
 *  0 TEP event.
 */
#define MCPWM_TIMER0_TEP_INT_RAW    (BIT(6))
#define MCPWM_TIMER0_TEP_INT_RAW_M  (MCPWM_TIMER0_TEP_INT_RAW_V << MCPWM_TIMER0_TEP_INT_RAW_S)
#define MCPWM_TIMER0_TEP_INT_RAW_V  0x00000001U
#define MCPWM_TIMER0_TEP_INT_RAW_S  6
/** MCPWM_TIMER1_TEP_INT_RAW : R/WTC/SS; bitpos: [7]; default: 0;
 *  Raw status bit: The raw interrupt status of the interrupt triggered by a PWM timer
 *  1 TEP event.
 */
#define MCPWM_TIMER1_TEP_INT_RAW    (BIT(7))
#define MCPWM_TIMER1_TEP_INT_RAW_M  (MCPWM_TIMER1_TEP_INT_RAW_V << MCPWM_TIMER1_TEP_INT_RAW_S)
#define MCPWM_TIMER1_TEP_INT_RAW_V  0x00000001U
#define MCPWM_TIMER1_TEP_INT_RAW_S  7
/** MCPWM_TIMER2_TEP_INT_RAW : R/WTC/SS; bitpos: [8]; default: 0;
 *  Raw status bit: The raw interrupt status of the interrupt triggered by a PWM timer
 *  2 TEP event.
 */
#define MCPWM_TIMER2_TEP_INT_RAW    (BIT(8))
#define MCPWM_TIMER2_TEP_INT_RAW_M  (MCPWM_TIMER2_TEP_INT_RAW_V << MCPWM_TIMER2_TEP_INT_RAW_S)
#define MCPWM_TIMER2_TEP_INT_RAW_V  0x00000001U
#define MCPWM_TIMER2_TEP_INT_RAW_S  8
/** MCPWM_FAULT0_INT_RAW : R/WTC/SS; bitpos: [9]; default: 0;
 *  Raw status bit: The raw interrupt status of the interrupt triggered when event_f0
 *  starts.
 */
#define MCPWM_FAULT0_INT_RAW    (BIT(9))
#define MCPWM_FAULT0_INT_RAW_M  (MCPWM_FAULT0_INT_RAW_V << MCPWM_FAULT0_INT_RAW_S)
#define MCPWM_FAULT0_INT_RAW_V  0x00000001U
#define MCPWM_FAULT0_INT_RAW_S  9
/** MCPWM_FAULT1_INT_RAW : R/WTC/SS; bitpos: [10]; default: 0;
 *  Raw status bit: The raw interrupt status of the interrupt triggered when event_f1
 *  starts.
 */
#define MCPWM_FAULT1_INT_RAW    (BIT(10))
#define MCPWM_FAULT1_INT_RAW_M  (MCPWM_FAULT1_INT_RAW_V << MCPWM_FAULT1_INT_RAW_S)
#define MCPWM_FAULT1_INT_RAW_V  0x00000001U
#define MCPWM_FAULT1_INT_RAW_S  10
/** MCPWM_FAULT2_INT_RAW : R/WTC/SS; bitpos: [11]; default: 0;
 *  Raw status bit: The raw interrupt status of the interrupt triggered when event_f2
 *  starts.
 */
#define MCPWM_FAULT2_INT_RAW    (BIT(11))
#define MCPWM_FAULT2_INT_RAW_M  (MCPWM_FAULT2_INT_RAW_V << MCPWM_FAULT2_INT_RAW_S)
#define MCPWM_FAULT2_INT_RAW_V  0x00000001U
#define MCPWM_FAULT2_INT_RAW_S  11
/** MCPWM_FAULT0_CLR_INT_RAW : R/WTC/SS; bitpos: [12]; default: 0;
 *  Raw status bit: The raw interrupt status of the interrupt triggered when event_f0
 *  clears.
 */
#define MCPWM_FAULT0_CLR_INT_RAW    (BIT(12))
#define MCPWM_FAULT0_CLR_INT_RAW_M  (MCPWM_FAULT0_CLR_INT_RAW_V << MCPWM_FAULT0_CLR_INT_RAW_S)
#define MCPWM_FAULT0_CLR_INT_RAW_V  0x00000001U
#define MCPWM_FAULT0_CLR_INT_RAW_S  12
/** MCPWM_FAULT1_CLR_INT_RAW : R/WTC/SS; bitpos: [13]; default: 0;
 *  Raw status bit: The raw interrupt status of the interrupt triggered when event_f1
 *  clears.
 */
#define MCPWM_FAULT1_CLR_INT_RAW    (BIT(13))
#define MCPWM_FAULT1_CLR_INT_RAW_M  (MCPWM_FAULT1_CLR_INT_RAW_V << MCPWM_FAULT1_CLR_INT_RAW_S)
#define MCPWM_FAULT1_CLR_INT_RAW_V  0x00000001U
#define MCPWM_FAULT1_CLR_INT_RAW_S  13
/** MCPWM_FAULT2_CLR_INT_RAW : R/WTC/SS; bitpos: [14]; default: 0;
 *  Raw status bit: The raw interrupt status of the interrupt triggered when event_f2
 *  clears.
 */
#define MCPWM_FAULT2_CLR_INT_RAW    (BIT(14))
#define MCPWM_FAULT2_CLR_INT_RAW_M  (MCPWM_FAULT2_CLR_INT_RAW_V << MCPWM_FAULT2_CLR_INT_RAW_S)
#define MCPWM_FAULT2_CLR_INT_RAW_V  0x00000001U
#define MCPWM_FAULT2_CLR_INT_RAW_S  14
/** MCPWM_CMPR0_TEA_INT_RAW : R/WTC/SS; bitpos: [15]; default: 0;
 *  Raw status bit: The raw interrupt status of the interrupt triggered by a PWM
 *  operator 0 TEA event
 */
#define MCPWM_CMPR0_TEA_INT_RAW    (BIT(15))
#define MCPWM_CMPR0_TEA_INT_RAW_M  (MCPWM_CMPR0_TEA_INT_RAW_V << MCPWM_CMPR0_TEA_INT_RAW_S)
#define MCPWM_CMPR0_TEA_INT_RAW_V  0x00000001U
#define MCPWM_CMPR0_TEA_INT_RAW_S  15
/** MCPWM_CMPR1_TEA_INT_RAW : R/WTC/SS; bitpos: [16]; default: 0;
 *  Raw status bit: The raw interrupt status of the interrupt triggered by a PWM
 *  operator 1 TEA event
 */
#define MCPWM_CMPR1_TEA_INT_RAW    (BIT(16))
#define MCPWM_CMPR1_TEA_INT_RAW_M  (MCPWM_CMPR1_TEA_INT_RAW_V << MCPWM_CMPR1_TEA_INT_RAW_S)
#define MCPWM_CMPR1_TEA_INT_RAW_V  0x00000001U
#define MCPWM_CMPR1_TEA_INT_RAW_S  16
/** MCPWM_CMPR2_TEA_INT_RAW : R/WTC/SS; bitpos: [17]; default: 0;
 *  Raw status bit: The raw interrupt status of the interrupt triggered by a PWM
 *  operator 2 TEA event
 */
#define MCPWM_CMPR2_TEA_INT_RAW    (BIT(17))
#define MCPWM_CMPR2_TEA_INT_RAW_M  (MCPWM_CMPR2_TEA_INT_RAW_V << MCPWM_CMPR2_TEA_INT_RAW_S)
#define MCPWM_CMPR2_TEA_INT_RAW_V  0x00000001U
#define MCPWM_CMPR2_TEA_INT_RAW_S  17
/** MCPWM_CMPR0_TEB_INT_RAW : R/WTC/SS; bitpos: [18]; default: 0;
 *  Raw status bit: The raw interrupt status of the interrupt triggered by a PWM
 *  operator 0 TEB event
 */
#define MCPWM_CMPR0_TEB_INT_RAW    (BIT(18))
#define MCPWM_CMPR0_TEB_INT_RAW_M  (MCPWM_CMPR0_TEB_INT_RAW_V << MCPWM_CMPR0_TEB_INT_RAW_S)
#define MCPWM_CMPR0_TEB_INT_RAW_V  0x00000001U
#define MCPWM_CMPR0_TEB_INT_RAW_S  18
/** MCPWM_CMPR1_TEB_INT_RAW : R/WTC/SS; bitpos: [19]; default: 0;
 *  Raw status bit: The raw interrupt status of the interrupt triggered by a PWM
 *  operator 1 TEB event
 */
#define MCPWM_CMPR1_TEB_INT_RAW    (BIT(19))
#define MCPWM_CMPR1_TEB_INT_RAW_M  (MCPWM_CMPR1_TEB_INT_RAW_V << MCPWM_CMPR1_TEB_INT_RAW_S)
#define MCPWM_CMPR1_TEB_INT_RAW_V  0x00000001U
#define MCPWM_CMPR1_TEB_INT_RAW_S  19
/** MCPWM_CMPR2_TEB_INT_RAW : R/WTC/SS; bitpos: [20]; default: 0;
 *  Raw status bit: The raw interrupt status of the interrupt triggered by a PWM
 *  operator 2 TEB event
 */
#define MCPWM_CMPR2_TEB_INT_RAW    (BIT(20))
#define MCPWM_CMPR2_TEB_INT_RAW_M  (MCPWM_CMPR2_TEB_INT_RAW_V << MCPWM_CMPR2_TEB_INT_RAW_S)
#define MCPWM_CMPR2_TEB_INT_RAW_V  0x00000001U
#define MCPWM_CMPR2_TEB_INT_RAW_S  20
/** MCPWM_TZ0_CBC_INT_RAW : R/WTC/SS; bitpos: [21]; default: 0;
 *  Raw status bit: The raw interrupt status of the interrupt triggered by a
 *  cycle-by-cycle mode action on PWM0.
 */
#define MCPWM_TZ0_CBC_INT_RAW    (BIT(21))
#define MCPWM_TZ0_CBC_INT_RAW_M  (MCPWM_TZ0_CBC_INT_RAW_V << MCPWM_TZ0_CBC_INT_RAW_S)
#define MCPWM_TZ0_CBC_INT_RAW_V  0x00000001U
#define MCPWM_TZ0_CBC_INT_RAW_S  21
/** MCPWM_TZ1_CBC_INT_RAW : R/WTC/SS; bitpos: [22]; default: 0;
 *  Raw status bit: The raw interrupt status of the interrupt triggered by a
 *  cycle-by-cycle mode action on PWM1.
 */
#define MCPWM_TZ1_CBC_INT_RAW    (BIT(22))
#define MCPWM_TZ1_CBC_INT_RAW_M  (MCPWM_TZ1_CBC_INT_RAW_V << MCPWM_TZ1_CBC_INT_RAW_S)
#define MCPWM_TZ1_CBC_INT_RAW_V  0x00000001U
#define MCPWM_TZ1_CBC_INT_RAW_S  22
/** MCPWM_TZ2_CBC_INT_RAW : R/WTC/SS; bitpos: [23]; default: 0;
 *  Raw status bit: The raw interrupt status of the interrupt triggered by a
 *  cycle-by-cycle mode action on PWM2.
 */
#define MCPWM_TZ2_CBC_INT_RAW    (BIT(23))
#define MCPWM_TZ2_CBC_INT_RAW_M  (MCPWM_TZ2_CBC_INT_RAW_V << MCPWM_TZ2_CBC_INT_RAW_S)
#define MCPWM_TZ2_CBC_INT_RAW_V  0x00000001U
#define MCPWM_TZ2_CBC_INT_RAW_S  23
/** MCPWM_TZ0_OST_INT_RAW : R/WTC/SS; bitpos: [24]; default: 0;
 *  Raw status bit: The raw interrupt status of the interrupt triggered by a one-shot
 *  mode action on PWM0.
 */
#define MCPWM_TZ0_OST_INT_RAW    (BIT(24))
#define MCPWM_TZ0_OST_INT_RAW_M  (MCPWM_TZ0_OST_INT_RAW_V << MCPWM_TZ0_OST_INT_RAW_S)
#define MCPWM_TZ0_OST_INT_RAW_V  0x00000001U
#define MCPWM_TZ0_OST_INT_RAW_S  24
/** MCPWM_TZ1_OST_INT_RAW : R/WTC/SS; bitpos: [25]; default: 0;
 *  Raw status bit: The raw interrupt status of the interrupt triggered by a one-shot
 *  mode action on PWM1.
 */
#define MCPWM_TZ1_OST_INT_RAW    (BIT(25))
#define MCPWM_TZ1_OST_INT_RAW_M  (MCPWM_TZ1_OST_INT_RAW_V << MCPWM_TZ1_OST_INT_RAW_S)
#define MCPWM_TZ1_OST_INT_RAW_V  0x00000001U
#define MCPWM_TZ1_OST_INT_RAW_S  25
/** MCPWM_TZ2_OST_INT_RAW : R/WTC/SS; bitpos: [26]; default: 0;
 *  Raw status bit: The raw interrupt status of the interrupt triggered by a one-shot
 *  mode action on PWM2.
 */
#define MCPWM_TZ2_OST_INT_RAW    (BIT(26))
#define MCPWM_TZ2_OST_INT_RAW_M  (MCPWM_TZ2_OST_INT_RAW_V << MCPWM_TZ2_OST_INT_RAW_S)
#define MCPWM_TZ2_OST_INT_RAW_V  0x00000001U
#define MCPWM_TZ2_OST_INT_RAW_S  26
/** MCPWM_CAP0_INT_RAW : R/WTC/SS; bitpos: [27]; default: 0;
 *  Raw status bit: The raw interrupt status of the interrupt triggered by capture on
 *  CAP0.
 */
#define MCPWM_CAP0_INT_RAW    (BIT(27))
#define MCPWM_CAP0_INT_RAW_M  (MCPWM_CAP0_INT_RAW_V << MCPWM_CAP0_INT_RAW_S)
#define MCPWM_CAP0_INT_RAW_V  0x00000001U
#define MCPWM_CAP0_INT_RAW_S  27
/** MCPWM_CAP1_INT_RAW : R/WTC/SS; bitpos: [28]; default: 0;
 *  Raw status bit: The raw interrupt status of the interrupt triggered by capture on
 *  CAP1.
 */
#define MCPWM_CAP1_INT_RAW    (BIT(28))
#define MCPWM_CAP1_INT_RAW_M  (MCPWM_CAP1_INT_RAW_V << MCPWM_CAP1_INT_RAW_S)
#define MCPWM_CAP1_INT_RAW_V  0x00000001U
#define MCPWM_CAP1_INT_RAW_S  28
/** MCPWM_CAP2_INT_RAW : R/WTC/SS; bitpos: [29]; default: 0;
 *  Raw status bit: The raw interrupt status of the interrupt triggered by capture on
 *  CAP2.
 */
#define MCPWM_CAP2_INT_RAW    (BIT(29))
#define MCPWM_CAP2_INT_RAW_M  (MCPWM_CAP2_INT_RAW_V << MCPWM_CAP2_INT_RAW_S)
#define MCPWM_CAP2_INT_RAW_V  0x00000001U
#define MCPWM_CAP2_INT_RAW_S  29

/** MCPWM_INT_ST_REG register
 *  Interrupt masked status register
 */
#define MCPWM_INT_ST_REG (DR_REG_MCPWM_BASE + 0x118)
/** MCPWM_TIMER0_STOP_INT_ST : RO; bitpos: [0]; default: 0;
 *  Masked status bit: The masked interrupt status of the interrupt triggered when the
 *  timer 0 stops.
 */
#define MCPWM_TIMER0_STOP_INT_ST    (BIT(0))
#define MCPWM_TIMER0_STOP_INT_ST_M  (MCPWM_TIMER0_STOP_INT_ST_V << MCPWM_TIMER0_STOP_INT_ST_S)
#define MCPWM_TIMER0_STOP_INT_ST_V  0x00000001U
#define MCPWM_TIMER0_STOP_INT_ST_S  0
/** MCPWM_TIMER1_STOP_INT_ST : RO; bitpos: [1]; default: 0;
 *  Masked status bit: The masked interrupt status of the interrupt triggered when the
 *  timer 1 stops.
 */
#define MCPWM_TIMER1_STOP_INT_ST    (BIT(1))
#define MCPWM_TIMER1_STOP_INT_ST_M  (MCPWM_TIMER1_STOP_INT_ST_V << MCPWM_TIMER1_STOP_INT_ST_S)
#define MCPWM_TIMER1_STOP_INT_ST_V  0x00000001U
#define MCPWM_TIMER1_STOP_INT_ST_S  1
/** MCPWM_TIMER2_STOP_INT_ST : RO; bitpos: [2]; default: 0;
 *  Masked status bit: The masked interrupt status of the interrupt triggered when the
 *  timer 2 stops.
 */
#define MCPWM_TIMER2_STOP_INT_ST    (BIT(2))
#define MCPWM_TIMER2_STOP_INT_ST_M  (MCPWM_TIMER2_STOP_INT_ST_V << MCPWM_TIMER2_STOP_INT_ST_S)
#define MCPWM_TIMER2_STOP_INT_ST_V  0x00000001U
#define MCPWM_TIMER2_STOP_INT_ST_S  2
/** MCPWM_TIMER0_TEZ_INT_ST : RO; bitpos: [3]; default: 0;
 *  Masked status bit: The masked interrupt status of the interrupt triggered by a PWM
 *  timer 0 TEZ event.
 */
#define MCPWM_TIMER0_TEZ_INT_ST    (BIT(3))
#define MCPWM_TIMER0_TEZ_INT_ST_M  (MCPWM_TIMER0_TEZ_INT_ST_V << MCPWM_TIMER0_TEZ_INT_ST_S)
#define MCPWM_TIMER0_TEZ_INT_ST_V  0x00000001U
#define MCPWM_TIMER0_TEZ_INT_ST_S  3
/** MCPWM_TIMER1_TEZ_INT_ST : RO; bitpos: [4]; default: 0;
 *  Masked status bit: The masked interrupt status of the interrupt triggered by a PWM
 *  timer 1 TEZ event.
 */
#define MCPWM_TIMER1_TEZ_INT_ST    (BIT(4))
#define MCPWM_TIMER1_TEZ_INT_ST_M  (MCPWM_TIMER1_TEZ_INT_ST_V << MCPWM_TIMER1_TEZ_INT_ST_S)
#define MCPWM_TIMER1_TEZ_INT_ST_V  0x00000001U
#define MCPWM_TIMER1_TEZ_INT_ST_S  4
/** MCPWM_TIMER2_TEZ_INT_ST : RO; bitpos: [5]; default: 0;
 *  Masked status bit: The masked interrupt status of the interrupt triggered by a PWM
 *  timer 2 TEZ event.
 */
#define MCPWM_TIMER2_TEZ_INT_ST    (BIT(5))
#define MCPWM_TIMER2_TEZ_INT_ST_M  (MCPWM_TIMER2_TEZ_INT_ST_V << MCPWM_TIMER2_TEZ_INT_ST_S)
#define MCPWM_TIMER2_TEZ_INT_ST_V  0x00000001U
#define MCPWM_TIMER2_TEZ_INT_ST_S  5
/** MCPWM_TIMER0_TEP_INT_ST : RO; bitpos: [6]; default: 0;
 *  Masked status bit: The masked interrupt status of the interrupt triggered by a PWM
 *  timer 0 TEP event.
 */
#define MCPWM_TIMER0_TEP_INT_ST    (BIT(6))
#define MCPWM_TIMER0_TEP_INT_ST_M  (MCPWM_TIMER0_TEP_INT_ST_V << MCPWM_TIMER0_TEP_INT_ST_S)
#define MCPWM_TIMER0_TEP_INT_ST_V  0x00000001U
#define MCPWM_TIMER0_TEP_INT_ST_S  6
/** MCPWM_TIMER1_TEP_INT_ST : RO; bitpos: [7]; default: 0;
 *  Masked status bit: The masked interrupt status of the interrupt triggered by a PWM
 *  timer 1 TEP event.
 */
#define MCPWM_TIMER1_TEP_INT_ST    (BIT(7))
#define MCPWM_TIMER1_TEP_INT_ST_M  (MCPWM_TIMER1_TEP_INT_ST_V << MCPWM_TIMER1_TEP_INT_ST_S)
#define MCPWM_TIMER1_TEP_INT_ST_V  0x00000001U
#define MCPWM_TIMER1_TEP_INT_ST_S  7
/** MCPWM_TIMER2_TEP_INT_ST : RO; bitpos: [8]; default: 0;
 *  Masked status bit: The masked interrupt status of the interrupt triggered by a PWM
 *  timer 2 TEP event.
 */
#define MCPWM_TIMER2_TEP_INT_ST    (BIT(8))
#define MCPWM_TIMER2_TEP_INT_ST_M  (MCPWM_TIMER2_TEP_INT_ST_V << MCPWM_TIMER2_TEP_INT_ST_S)
#define MCPWM_TIMER2_TEP_INT_ST_V  0x00000001U
#define MCPWM_TIMER2_TEP_INT_ST_S  8
/** MCPWM_FAULT0_INT_ST : RO; bitpos: [9]; default: 0;
 *  Masked status bit: The masked interrupt status of the interrupt triggered when
 *  event_f0 starts.
 */
#define MCPWM_FAULT0_INT_ST    (BIT(9))
#define MCPWM_FAULT0_INT_ST_M  (MCPWM_FAULT0_INT_ST_V << MCPWM_FAULT0_INT_ST_S)
#define MCPWM_FAULT0_INT_ST_V  0x00000001U
#define MCPWM_FAULT0_INT_ST_S  9
/** MCPWM_FAULT1_INT_ST : RO; bitpos: [10]; default: 0;
 *  Masked status bit: The masked interrupt status of the interrupt triggered when
 *  event_f1 starts.
 */
#define MCPWM_FAULT1_INT_ST    (BIT(10))
#define MCPWM_FAULT1_INT_ST_M  (MCPWM_FAULT1_INT_ST_V << MCPWM_FAULT1_INT_ST_S)
#define MCPWM_FAULT1_INT_ST_V  0x00000001U
#define MCPWM_FAULT1_INT_ST_S  10
/** MCPWM_FAULT2_INT_ST : RO; bitpos: [11]; default: 0;
 *  Masked status bit: The masked interrupt status of the interrupt triggered when
 *  event_f2 starts.
 */
#define MCPWM_FAULT2_INT_ST    (BIT(11))
#define MCPWM_FAULT2_INT_ST_M  (MCPWM_FAULT2_INT_ST_V << MCPWM_FAULT2_INT_ST_S)
#define MCPWM_FAULT2_INT_ST_V  0x00000001U
#define MCPWM_FAULT2_INT_ST_S  11
/** MCPWM_FAULT0_CLR_INT_ST : RO; bitpos: [12]; default: 0;
 *  Masked status bit: The masked interrupt status of the interrupt triggered when
 *  event_f0 clears.
 */
#define MCPWM_FAULT0_CLR_INT_ST    (BIT(12))
#define MCPWM_FAULT0_CLR_INT_ST_M  (MCPWM_FAULT0_CLR_INT_ST_V << MCPWM_FAULT0_CLR_INT_ST_S)
#define MCPWM_FAULT0_CLR_INT_ST_V  0x00000001U
#define MCPWM_FAULT0_CLR_INT_ST_S  12
/** MCPWM_FAULT1_CLR_INT_ST : RO; bitpos: [13]; default: 0;
 *  Masked status bit: The masked interrupt status of the interrupt triggered when
 *  event_f1 clears.
 */
#define MCPWM_FAULT1_CLR_INT_ST    (BIT(13))
#define MCPWM_FAULT1_CLR_INT_ST_M  (MCPWM_FAULT1_CLR_INT_ST_V << MCPWM_FAULT1_CLR_INT_ST_S)
#define MCPWM_FAULT1_CLR_INT_ST_V  0x00000001U
#define MCPWM_FAULT1_CLR_INT_ST_S  13
/** MCPWM_FAULT2_CLR_INT_ST : RO; bitpos: [14]; default: 0;
 *  Masked status bit: The masked interrupt status of the interrupt triggered when
 *  event_f2 clears.
 */
#define MCPWM_FAULT2_CLR_INT_ST    (BIT(14))
#define MCPWM_FAULT2_CLR_INT_ST_M  (MCPWM_FAULT2_CLR_INT_ST_V << MCPWM_FAULT2_CLR_INT_ST_S)
#define MCPWM_FAULT2_CLR_INT_ST_V  0x00000001U
#define MCPWM_FAULT2_CLR_INT_ST_S  14
/** MCPWM_CMPR0_TEA_INT_ST : RO; bitpos: [15]; default: 0;
 *  Masked status bit: The masked interrupt status of the interrupt triggered by a PWM
 *  operator 0 TEA event
 */
#define MCPWM_CMPR0_TEA_INT_ST    (BIT(15))
#define MCPWM_CMPR0_TEA_INT_ST_M  (MCPWM_CMPR0_TEA_INT_ST_V << MCPWM_CMPR0_TEA_INT_ST_S)
#define MCPWM_CMPR0_TEA_INT_ST_V  0x00000001U
#define MCPWM_CMPR0_TEA_INT_ST_S  15
/** MCPWM_CMPR1_TEA_INT_ST : RO; bitpos: [16]; default: 0;
 *  Masked status bit: The masked interrupt status of the interrupt triggered by a PWM
 *  operator 1 TEA event
 */
#define MCPWM_CMPR1_TEA_INT_ST    (BIT(16))
#define MCPWM_CMPR1_TEA_INT_ST_M  (MCPWM_CMPR1_TEA_INT_ST_V << MCPWM_CMPR1_TEA_INT_ST_S)
#define MCPWM_CMPR1_TEA_INT_ST_V  0x00000001U
#define MCPWM_CMPR1_TEA_INT_ST_S  16
/** MCPWM_CMPR2_TEA_INT_ST : RO; bitpos: [17]; default: 0;
 *  Masked status bit: The masked interrupt status of the interrupt triggered by a PWM
 *  operator 2 TEA event
 */
#define MCPWM_CMPR2_TEA_INT_ST    (BIT(17))
#define MCPWM_CMPR2_TEA_INT_ST_M  (MCPWM_CMPR2_TEA_INT_ST_V << MCPWM_CMPR2_TEA_INT_ST_S)
#define MCPWM_CMPR2_TEA_INT_ST_V  0x00000001U
#define MCPWM_CMPR2_TEA_INT_ST_S  17
/** MCPWM_CMPR0_TEB_INT_ST : RO; bitpos: [18]; default: 0;
 *  Masked status bit: The masked interrupt status of the interrupt triggered by a PWM
 *  operator 0 TEB event
 */
#define MCPWM_CMPR0_TEB_INT_ST    (BIT(18))
#define MCPWM_CMPR0_TEB_INT_ST_M  (MCPWM_CMPR0_TEB_INT_ST_V << MCPWM_CMPR0_TEB_INT_ST_S)
#define MCPWM_CMPR0_TEB_INT_ST_V  0x00000001U
#define MCPWM_CMPR0_TEB_INT_ST_S  18
/** MCPWM_CMPR1_TEB_INT_ST : RO; bitpos: [19]; default: 0;
 *  Masked status bit: The masked interrupt status of the interrupt triggered by a PWM
 *  operator 1 TEB event
 */
#define MCPWM_CMPR1_TEB_INT_ST    (BIT(19))
#define MCPWM_CMPR1_TEB_INT_ST_M  (MCPWM_CMPR1_TEB_INT_ST_V << MCPWM_CMPR1_TEB_INT_ST_S)
#define MCPWM_CMPR1_TEB_INT_ST_V  0x00000001U
#define MCPWM_CMPR1_TEB_INT_ST_S  19
/** MCPWM_CMPR2_TEB_INT_ST : RO; bitpos: [20]; default: 0;
 *  Masked status bit: The masked interrupt status of the interrupt triggered by a PWM
 *  operator 2 TEB event
 */
#define MCPWM_CMPR2_TEB_INT_ST    (BIT(20))
#define MCPWM_CMPR2_TEB_INT_ST_M  (MCPWM_CMPR2_TEB_INT_ST_V << MCPWM_CMPR2_TEB_INT_ST_S)
#define MCPWM_CMPR2_TEB_INT_ST_V  0x00000001U
#define MCPWM_CMPR2_TEB_INT_ST_S  20
/** MCPWM_TZ0_CBC_INT_ST : RO; bitpos: [21]; default: 0;
 *  Masked status bit: The masked interrupt status of the interrupt triggered by a
 *  cycle-by-cycle mode action on PWM0.
 */
#define MCPWM_TZ0_CBC_INT_ST    (BIT(21))
#define MCPWM_TZ0_CBC_INT_ST_M  (MCPWM_TZ0_CBC_INT_ST_V << MCPWM_TZ0_CBC_INT_ST_S)
#define MCPWM_TZ0_CBC_INT_ST_V  0x00000001U
#define MCPWM_TZ0_CBC_INT_ST_S  21
/** MCPWM_TZ1_CBC_INT_ST : RO; bitpos: [22]; default: 0;
 *  Masked status bit: The masked interrupt status of the interrupt triggered by a
 *  cycle-by-cycle mode action on PWM1.
 */
#define MCPWM_TZ1_CBC_INT_ST    (BIT(22))
#define MCPWM_TZ1_CBC_INT_ST_M  (MCPWM_TZ1_CBC_INT_ST_V << MCPWM_TZ1_CBC_INT_ST_S)
#define MCPWM_TZ1_CBC_INT_ST_V  0x00000001U
#define MCPWM_TZ1_CBC_INT_ST_S  22
/** MCPWM_TZ2_CBC_INT_ST : RO; bitpos: [23]; default: 0;
 *  Masked status bit: The masked interrupt status of the interrupt triggered by a
 *  cycle-by-cycle mode action on PWM2.
 */
#define MCPWM_TZ2_CBC_INT_ST    (BIT(23))
#define MCPWM_TZ2_CBC_INT_ST_M  (MCPWM_TZ2_CBC_INT_ST_V << MCPWM_TZ2_CBC_INT_ST_S)
#define MCPWM_TZ2_CBC_INT_ST_V  0x00000001U
#define MCPWM_TZ2_CBC_INT_ST_S  23
/** MCPWM_TZ0_OST_INT_ST : RO; bitpos: [24]; default: 0;
 *  Masked status bit: The masked interrupt status of the interrupt triggered by a
 *  one-shot mode action on PWM0.
 */
#define MCPWM_TZ0_OST_INT_ST    (BIT(24))
#define MCPWM_TZ0_OST_INT_ST_M  (MCPWM_TZ0_OST_INT_ST_V << MCPWM_TZ0_OST_INT_ST_S)
#define MCPWM_TZ0_OST_INT_ST_V  0x00000001U
#define MCPWM_TZ0_OST_INT_ST_S  24
/** MCPWM_TZ1_OST_INT_ST : RO; bitpos: [25]; default: 0;
 *  Masked status bit: The masked interrupt status of the interrupt triggered by a
 *  one-shot mode action on PWM1.
 */
#define MCPWM_TZ1_OST_INT_ST    (BIT(25))
#define MCPWM_TZ1_OST_INT_ST_M  (MCPWM_TZ1_OST_INT_ST_V << MCPWM_TZ1_OST_INT_ST_S)
#define MCPWM_TZ1_OST_INT_ST_V  0x00000001U
#define MCPWM_TZ1_OST_INT_ST_S  25
/** MCPWM_TZ2_OST_INT_ST : RO; bitpos: [26]; default: 0;
 *  Masked status bit: The masked interrupt status of the interrupt triggered by a
 *  one-shot mode action on PWM2.
 */
#define MCPWM_TZ2_OST_INT_ST    (BIT(26))
#define MCPWM_TZ2_OST_INT_ST_M  (MCPWM_TZ2_OST_INT_ST_V << MCPWM_TZ2_OST_INT_ST_S)
#define MCPWM_TZ2_OST_INT_ST_V  0x00000001U
#define MCPWM_TZ2_OST_INT_ST_S  26
/** MCPWM_CAP0_INT_ST : RO; bitpos: [27]; default: 0;
 *  Masked status bit: The masked interrupt status of the interrupt triggered by
 *  capture on CAP0.
 */
#define MCPWM_CAP0_INT_ST    (BIT(27))
#define MCPWM_CAP0_INT_ST_M  (MCPWM_CAP0_INT_ST_V << MCPWM_CAP0_INT_ST_S)
#define MCPWM_CAP0_INT_ST_V  0x00000001U
#define MCPWM_CAP0_INT_ST_S  27
/** MCPWM_CAP1_INT_ST : RO; bitpos: [28]; default: 0;
 *  Masked status bit: The masked interrupt status of the interrupt triggered by
 *  capture on CAP1.
 */
#define MCPWM_CAP1_INT_ST    (BIT(28))
#define MCPWM_CAP1_INT_ST_M  (MCPWM_CAP1_INT_ST_V << MCPWM_CAP1_INT_ST_S)
#define MCPWM_CAP1_INT_ST_V  0x00000001U
#define MCPWM_CAP1_INT_ST_S  28
/** MCPWM_CAP2_INT_ST : RO; bitpos: [29]; default: 0;
 *  Masked status bit: The masked interrupt status of the interrupt triggered by
 *  capture on CAP2.
 */
#define MCPWM_CAP2_INT_ST    (BIT(29))
#define MCPWM_CAP2_INT_ST_M  (MCPWM_CAP2_INT_ST_V << MCPWM_CAP2_INT_ST_S)
#define MCPWM_CAP2_INT_ST_V  0x00000001U
#define MCPWM_CAP2_INT_ST_S  29

/** MCPWM_INT_CLR_REG register
 *  Interrupt clear register
 */
#define MCPWM_INT_CLR_REG (DR_REG_MCPWM_BASE + 0x11c)
/** MCPWM_TIMER0_STOP_INT_CLR : WT; bitpos: [0]; default: 0;
 *  Clear bit: Write 1 to clear the interrupt triggered when the timer 0 stops.
 */
#define MCPWM_TIMER0_STOP_INT_CLR    (BIT(0))
#define MCPWM_TIMER0_STOP_INT_CLR_M  (MCPWM_TIMER0_STOP_INT_CLR_V << MCPWM_TIMER0_STOP_INT_CLR_S)
#define MCPWM_TIMER0_STOP_INT_CLR_V  0x00000001U
#define MCPWM_TIMER0_STOP_INT_CLR_S  0
/** MCPWM_TIMER1_STOP_INT_CLR : WT; bitpos: [1]; default: 0;
 *  Clear bit: Write 1 to clear the interrupt triggered when the timer 1 stops.
 */
#define MCPWM_TIMER1_STOP_INT_CLR    (BIT(1))
#define MCPWM_TIMER1_STOP_INT_CLR_M  (MCPWM_TIMER1_STOP_INT_CLR_V << MCPWM_TIMER1_STOP_INT_CLR_S)
#define MCPWM_TIMER1_STOP_INT_CLR_V  0x00000001U
#define MCPWM_TIMER1_STOP_INT_CLR_S  1
/** MCPWM_TIMER2_STOP_INT_CLR : WT; bitpos: [2]; default: 0;
 *  Clear bit: Write 1 to clear the interrupt triggered when the timer 2 stops.
 */
#define MCPWM_TIMER2_STOP_INT_CLR    (BIT(2))
#define MCPWM_TIMER2_STOP_INT_CLR_M  (MCPWM_TIMER2_STOP_INT_CLR_V << MCPWM_TIMER2_STOP_INT_CLR_S)
#define MCPWM_TIMER2_STOP_INT_CLR_V  0x00000001U
#define MCPWM_TIMER2_STOP_INT_CLR_S  2
/** MCPWM_TIMER0_TEZ_INT_CLR : WT; bitpos: [3]; default: 0;
 *  Clear bit: Write 1 to clear the interrupt triggered by a PWM timer 0 TEZ event.
 */
#define MCPWM_TIMER0_TEZ_INT_CLR    (BIT(3))
#define MCPWM_TIMER0_TEZ_INT_CLR_M  (MCPWM_TIMER0_TEZ_INT_CLR_V << MCPWM_TIMER0_TEZ_INT_CLR_S)
#define MCPWM_TIMER0_TEZ_INT_CLR_V  0x00000001U
#define MCPWM_TIMER0_TEZ_INT_CLR_S  3
/** MCPWM_TIMER1_TEZ_INT_CLR : WT; bitpos: [4]; default: 0;
 *  Clear bit: Write 1 to clear the interrupt triggered by a PWM timer 1 TEZ event.
 */
#define MCPWM_TIMER1_TEZ_INT_CLR    (BIT(4))
#define MCPWM_TIMER1_TEZ_INT_CLR_M  (MCPWM_TIMER1_TEZ_INT_CLR_V << MCPWM_TIMER1_TEZ_INT_CLR_S)
#define MCPWM_TIMER1_TEZ_INT_CLR_V  0x00000001U
#define MCPWM_TIMER1_TEZ_INT_CLR_S  4
/** MCPWM_TIMER2_TEZ_INT_CLR : WT; bitpos: [5]; default: 0;
 *  Clear bit: Write 1 to clear the interrupt triggered by a PWM timer 2 TEZ event.
 */
#define MCPWM_TIMER2_TEZ_INT_CLR    (BIT(5))
#define MCPWM_TIMER2_TEZ_INT_CLR_M  (MCPWM_TIMER2_TEZ_INT_CLR_V << MCPWM_TIMER2_TEZ_INT_CLR_S)
#define MCPWM_TIMER2_TEZ_INT_CLR_V  0x00000001U
#define MCPWM_TIMER2_TEZ_INT_CLR_S  5
/** MCPWM_TIMER0_TEP_INT_CLR : WT; bitpos: [6]; default: 0;
 *  Clear bit: Write 1 to clear the interrupt triggered by a PWM timer 0 TEP event.
 */
#define MCPWM_TIMER0_TEP_INT_CLR    (BIT(6))
#define MCPWM_TIMER0_TEP_INT_CLR_M  (MCPWM_TIMER0_TEP_INT_CLR_V << MCPWM_TIMER0_TEP_INT_CLR_S)
#define MCPWM_TIMER0_TEP_INT_CLR_V  0x00000001U
#define MCPWM_TIMER0_TEP_INT_CLR_S  6
/** MCPWM_TIMER1_TEP_INT_CLR : WT; bitpos: [7]; default: 0;
 *  Clear bit: Write 1 to clear the interrupt triggered by a PWM timer 1 TEP event.
 */
#define MCPWM_TIMER1_TEP_INT_CLR    (BIT(7))
#define MCPWM_TIMER1_TEP_INT_CLR_M  (MCPWM_TIMER1_TEP_INT_CLR_V << MCPWM_TIMER1_TEP_INT_CLR_S)
#define MCPWM_TIMER1_TEP_INT_CLR_V  0x00000001U
#define MCPWM_TIMER1_TEP_INT_CLR_S  7
/** MCPWM_TIMER2_TEP_INT_CLR : WT; bitpos: [8]; default: 0;
 *  Clear bit: Write 1 to clear the interrupt triggered by a PWM timer 2 TEP event.
 */
#define MCPWM_TIMER2_TEP_INT_CLR    (BIT(8))
#define MCPWM_TIMER2_TEP_INT_CLR_M  (MCPWM_TIMER2_TEP_INT_CLR_V << MCPWM_TIMER2_TEP_INT_CLR_S)
#define MCPWM_TIMER2_TEP_INT_CLR_V  0x00000001U
#define MCPWM_TIMER2_TEP_INT_CLR_S  8
/** MCPWM_FAULT0_INT_CLR : WT; bitpos: [9]; default: 0;
 *  Clear bit: Write 1 to clear the interrupt triggered when event_f0 starts.
 */
#define MCPWM_FAULT0_INT_CLR    (BIT(9))
#define MCPWM_FAULT0_INT_CLR_M  (MCPWM_FAULT0_INT_CLR_V << MCPWM_FAULT0_INT_CLR_S)
#define MCPWM_FAULT0_INT_CLR_V  0x00000001U
#define MCPWM_FAULT0_INT_CLR_S  9
/** MCPWM_FAULT1_INT_CLR : WT; bitpos: [10]; default: 0;
 *  Clear bit: Write 1 to clear the interrupt triggered when event_f1 starts.
 */
#define MCPWM_FAULT1_INT_CLR    (BIT(10))
#define MCPWM_FAULT1_INT_CLR_M  (MCPWM_FAULT1_INT_CLR_V << MCPWM_FAULT1_INT_CLR_S)
#define MCPWM_FAULT1_INT_CLR_V  0x00000001U
#define MCPWM_FAULT1_INT_CLR_S  10
/** MCPWM_FAULT2_INT_CLR : WT; bitpos: [11]; default: 0;
 *  Clear bit: Write 1 to clear the interrupt triggered when event_f2 starts.
 */
#define MCPWM_FAULT2_INT_CLR    (BIT(11))
#define MCPWM_FAULT2_INT_CLR_M  (MCPWM_FAULT2_INT_CLR_V << MCPWM_FAULT2_INT_CLR_S)
#define MCPWM_FAULT2_INT_CLR_V  0x00000001U
#define MCPWM_FAULT2_INT_CLR_S  11
/** MCPWM_FAULT0_CLR_INT_CLR : WT; bitpos: [12]; default: 0;
 *  Clear bit: Write 1 to clear the interrupt triggered when event_f0 clears.
 */
#define MCPWM_FAULT0_CLR_INT_CLR    (BIT(12))
#define MCPWM_FAULT0_CLR_INT_CLR_M  (MCPWM_FAULT0_CLR_INT_CLR_V << MCPWM_FAULT0_CLR_INT_CLR_S)
#define MCPWM_FAULT0_CLR_INT_CLR_V  0x00000001U
#define MCPWM_FAULT0_CLR_INT_CLR_S  12
/** MCPWM_FAULT1_CLR_INT_CLR : WT; bitpos: [13]; default: 0;
 *  Clear bit: Write 1 to clear the interrupt triggered when event_f1 clears.
 */
#define MCPWM_FAULT1_CLR_INT_CLR    (BIT(13))
#define MCPWM_FAULT1_CLR_INT_CLR_M  (MCPWM_FAULT1_CLR_INT_CLR_V << MCPWM_FAULT1_CLR_INT_CLR_S)
#define MCPWM_FAULT1_CLR_INT_CLR_V  0x00000001U
#define MCPWM_FAULT1_CLR_INT_CLR_S  13
/** MCPWM_FAULT2_CLR_INT_CLR : WT; bitpos: [14]; default: 0;
 *  Clear bit: Write 1 to clear the interrupt triggered when event_f2 clears.
 */
#define MCPWM_FAULT2_CLR_INT_CLR    (BIT(14))
#define MCPWM_FAULT2_CLR_INT_CLR_M  (MCPWM_FAULT2_CLR_INT_CLR_V << MCPWM_FAULT2_CLR_INT_CLR_S)
#define MCPWM_FAULT2_CLR_INT_CLR_V  0x00000001U
#define MCPWM_FAULT2_CLR_INT_CLR_S  14
/** MCPWM_CMPR0_TEA_INT_CLR : WT; bitpos: [15]; default: 0;
 *  Clear bit: Write 1 to clear the interrupt triggered by a PWM operator 0 TEA event
 */
#define MCPWM_CMPR0_TEA_INT_CLR    (BIT(15))
#define MCPWM_CMPR0_TEA_INT_CLR_M  (MCPWM_CMPR0_TEA_INT_CLR_V << MCPWM_CMPR0_TEA_INT_CLR_S)
#define MCPWM_CMPR0_TEA_INT_CLR_V  0x00000001U
#define MCPWM_CMPR0_TEA_INT_CLR_S  15
/** MCPWM_CMPR1_TEA_INT_CLR : WT; bitpos: [16]; default: 0;
 *  Clear bit: Write 1 to clear the interrupt triggered by a PWM operator 1 TEA event
 */
#define MCPWM_CMPR1_TEA_INT_CLR    (BIT(16))
#define MCPWM_CMPR1_TEA_INT_CLR_M  (MCPWM_CMPR1_TEA_INT_CLR_V << MCPWM_CMPR1_TEA_INT_CLR_S)
#define MCPWM_CMPR1_TEA_INT_CLR_V  0x00000001U
#define MCPWM_CMPR1_TEA_INT_CLR_S  16
/** MCPWM_CMPR2_TEA_INT_CLR : WT; bitpos: [17]; default: 0;
 *  Clear bit: Write 1 to clear the interrupt triggered by a PWM operator 2 TEA event
 */
#define MCPWM_CMPR2_TEA_INT_CLR    (BIT(17))
#define MCPWM_CMPR2_TEA_INT_CLR_M  (MCPWM_CMPR2_TEA_INT_CLR_V << MCPWM_CMPR2_TEA_INT_CLR_S)
#define MCPWM_CMPR2_TEA_INT_CLR_V  0x00000001U
#define MCPWM_CMPR2_TEA_INT_CLR_S  17
/** MCPWM_CMPR0_TEB_INT_CLR : WT; bitpos: [18]; default: 0;
 *  Clear bit: Write 1 to clear the interrupt triggered by a PWM operator 0 TEB event
 */
#define MCPWM_CMPR0_TEB_INT_CLR    (BIT(18))
#define MCPWM_CMPR0_TEB_INT_CLR_M  (MCPWM_CMPR0_TEB_INT_CLR_V << MCPWM_CMPR0_TEB_INT_CLR_S)
#define MCPWM_CMPR0_TEB_INT_CLR_V  0x00000001U
#define MCPWM_CMPR0_TEB_INT_CLR_S  18
/** MCPWM_CMPR1_TEB_INT_CLR : WT; bitpos: [19]; default: 0;
 *  Clear bit: Write 1 to clear the interrupt triggered by a PWM operator 1 TEB event
 */
#define MCPWM_CMPR1_TEB_INT_CLR    (BIT(19))
#define MCPWM_CMPR1_TEB_INT_CLR_M  (MCPWM_CMPR1_TEB_INT_CLR_V << MCPWM_CMPR1_TEB_INT_CLR_S)
#define MCPWM_CMPR1_TEB_INT_CLR_V  0x00000001U
#define MCPWM_CMPR1_TEB_INT_CLR_S  19
/** MCPWM_CMPR2_TEB_INT_CLR : WT; bitpos: [20]; default: 0;
 *  Clear bit: Write 1 to clear the interrupt triggered by a PWM operator 2 TEB event
 */
#define MCPWM_CMPR2_TEB_INT_CLR    (BIT(20))
#define MCPWM_CMPR2_TEB_INT_CLR_M  (MCPWM_CMPR2_TEB_INT_CLR_V << MCPWM_CMPR2_TEB_INT_CLR_S)
#define MCPWM_CMPR2_TEB_INT_CLR_V  0x00000001U
#define MCPWM_CMPR2_TEB_INT_CLR_S  20
/** MCPWM_TZ0_CBC_INT_CLR : WT; bitpos: [21]; default: 0;
 *  Clear bit: Write 1 to clear the interrupt triggered by a cycle-by-cycle mode action
 *  on PWM0.
 */
#define MCPWM_TZ0_CBC_INT_CLR    (BIT(21))
#define MCPWM_TZ0_CBC_INT_CLR_M  (MCPWM_TZ0_CBC_INT_CLR_V << MCPWM_TZ0_CBC_INT_CLR_S)
#define MCPWM_TZ0_CBC_INT_CLR_V  0x00000001U
#define MCPWM_TZ0_CBC_INT_CLR_S  21
/** MCPWM_TZ1_CBC_INT_CLR : WT; bitpos: [22]; default: 0;
 *  Clear bit: Write 1 to clear the interrupt triggered by a cycle-by-cycle mode action
 *  on PWM1.
 */
#define MCPWM_TZ1_CBC_INT_CLR    (BIT(22))
#define MCPWM_TZ1_CBC_INT_CLR_M  (MCPWM_TZ1_CBC_INT_CLR_V << MCPWM_TZ1_CBC_INT_CLR_S)
#define MCPWM_TZ1_CBC_INT_CLR_V  0x00000001U
#define MCPWM_TZ1_CBC_INT_CLR_S  22
/** MCPWM_TZ2_CBC_INT_CLR : WT; bitpos: [23]; default: 0;
 *  Clear bit: Write 1 to clear the interrupt triggered by a cycle-by-cycle mode action
 *  on PWM2.
 */
#define MCPWM_TZ2_CBC_INT_CLR    (BIT(23))
#define MCPWM_TZ2_CBC_INT_CLR_M  (MCPWM_TZ2_CBC_INT_CLR_V << MCPWM_TZ2_CBC_INT_CLR_S)
#define MCPWM_TZ2_CBC_INT_CLR_V  0x00000001U
#define MCPWM_TZ2_CBC_INT_CLR_S  23
/** MCPWM_TZ0_OST_INT_CLR : WT; bitpos: [24]; default: 0;
 *  Clear bit: Write 1 to clear the interrupt triggered by a one-shot mode action on
 *  PWM0.
 */
#define MCPWM_TZ0_OST_INT_CLR    (BIT(24))
#define MCPWM_TZ0_OST_INT_CLR_M  (MCPWM_TZ0_OST_INT_CLR_V << MCPWM_TZ0_OST_INT_CLR_S)
#define MCPWM_TZ0_OST_INT_CLR_V  0x00000001U
#define MCPWM_TZ0_OST_INT_CLR_S  24
/** MCPWM_TZ1_OST_INT_CLR : WT; bitpos: [25]; default: 0;
 *  Clear bit: Write 1 to clear the interrupt triggered by a one-shot mode action on
 *  PWM1.
 */
#define MCPWM_TZ1_OST_INT_CLR    (BIT(25))
#define MCPWM_TZ1_OST_INT_CLR_M  (MCPWM_TZ1_OST_INT_CLR_V << MCPWM_TZ1_OST_INT_CLR_S)
#define MCPWM_TZ1_OST_INT_CLR_V  0x00000001U
#define MCPWM_TZ1_OST_INT_CLR_S  25
/** MCPWM_TZ2_OST_INT_CLR : WT; bitpos: [26]; default: 0;
 *  Clear bit: Write 1 to clear the interrupt triggered by a one-shot mode action on
 *  PWM2.
 */
#define MCPWM_TZ2_OST_INT_CLR    (BIT(26))
#define MCPWM_TZ2_OST_INT_CLR_M  (MCPWM_TZ2_OST_INT_CLR_V << MCPWM_TZ2_OST_INT_CLR_S)
#define MCPWM_TZ2_OST_INT_CLR_V  0x00000001U
#define MCPWM_TZ2_OST_INT_CLR_S  26
/** MCPWM_CAP0_INT_CLR : WT; bitpos: [27]; default: 0;
 *  Clear bit: Write 1 to clear the interrupt triggered by capture on CAP0.
 */
#define MCPWM_CAP0_INT_CLR    (BIT(27))
#define MCPWM_CAP0_INT_CLR_M  (MCPWM_CAP0_INT_CLR_V << MCPWM_CAP0_INT_CLR_S)
#define MCPWM_CAP0_INT_CLR_V  0x00000001U
#define MCPWM_CAP0_INT_CLR_S  27
/** MCPWM_CAP1_INT_CLR : WT; bitpos: [28]; default: 0;
 *  Clear bit: Write 1 to clear the interrupt triggered by capture on CAP1.
 */
#define MCPWM_CAP1_INT_CLR    (BIT(28))
#define MCPWM_CAP1_INT_CLR_M  (MCPWM_CAP1_INT_CLR_V << MCPWM_CAP1_INT_CLR_S)
#define MCPWM_CAP1_INT_CLR_V  0x00000001U
#define MCPWM_CAP1_INT_CLR_S  28
/** MCPWM_CAP2_INT_CLR : WT; bitpos: [29]; default: 0;
 *  Clear bit: Write 1 to clear the interrupt triggered by capture on CAP2.
 */
#define MCPWM_CAP2_INT_CLR    (BIT(29))
#define MCPWM_CAP2_INT_CLR_M  (MCPWM_CAP2_INT_CLR_V << MCPWM_CAP2_INT_CLR_S)
#define MCPWM_CAP2_INT_CLR_V  0x00000001U
#define MCPWM_CAP2_INT_CLR_S  29

/** MCPWM_EVT_EN_REG register
 *  Event enable register
 */
#define MCPWM_EVT_EN_REG (DR_REG_MCPWM_BASE + 0x120)
/** MCPWM_EVT_TIMER0_STOP_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable timer0 stop event generate.\\0: Disable\\1:
 *  Enable
 */
#define MCPWM_EVT_TIMER0_STOP_EN    (BIT(0))
#define MCPWM_EVT_TIMER0_STOP_EN_M  (MCPWM_EVT_TIMER0_STOP_EN_V << MCPWM_EVT_TIMER0_STOP_EN_S)
#define MCPWM_EVT_TIMER0_STOP_EN_V  0x00000001U
#define MCPWM_EVT_TIMER0_STOP_EN_S  0
/** MCPWM_EVT_TIMER1_STOP_EN : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enable timer1 stop event generate.\\0: Disable\\1:
 *  Enable
 */
#define MCPWM_EVT_TIMER1_STOP_EN    (BIT(1))
#define MCPWM_EVT_TIMER1_STOP_EN_M  (MCPWM_EVT_TIMER1_STOP_EN_V << MCPWM_EVT_TIMER1_STOP_EN_S)
#define MCPWM_EVT_TIMER1_STOP_EN_V  0x00000001U
#define MCPWM_EVT_TIMER1_STOP_EN_S  1
/** MCPWM_EVT_TIMER2_STOP_EN : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable timer2 stop event generate.\\0: Disable\\1:
 *  Enable
 */
#define MCPWM_EVT_TIMER2_STOP_EN    (BIT(2))
#define MCPWM_EVT_TIMER2_STOP_EN_M  (MCPWM_EVT_TIMER2_STOP_EN_V << MCPWM_EVT_TIMER2_STOP_EN_S)
#define MCPWM_EVT_TIMER2_STOP_EN_V  0x00000001U
#define MCPWM_EVT_TIMER2_STOP_EN_S  2
/** MCPWM_EVT_TIMER0_TEZ_EN : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable timer0 equal zero event generate.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_EVT_TIMER0_TEZ_EN    (BIT(3))
#define MCPWM_EVT_TIMER0_TEZ_EN_M  (MCPWM_EVT_TIMER0_TEZ_EN_V << MCPWM_EVT_TIMER0_TEZ_EN_S)
#define MCPWM_EVT_TIMER0_TEZ_EN_V  0x00000001U
#define MCPWM_EVT_TIMER0_TEZ_EN_S  3
/** MCPWM_EVT_TIMER1_TEZ_EN : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable timer1 equal zero event generate.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_EVT_TIMER1_TEZ_EN    (BIT(4))
#define MCPWM_EVT_TIMER1_TEZ_EN_M  (MCPWM_EVT_TIMER1_TEZ_EN_V << MCPWM_EVT_TIMER1_TEZ_EN_S)
#define MCPWM_EVT_TIMER1_TEZ_EN_V  0x00000001U
#define MCPWM_EVT_TIMER1_TEZ_EN_S  4
/** MCPWM_EVT_TIMER2_TEZ_EN : R/W; bitpos: [5]; default: 0;
 *  Configures whether or not to enable timer2 equal zero event generate.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_EVT_TIMER2_TEZ_EN    (BIT(5))
#define MCPWM_EVT_TIMER2_TEZ_EN_M  (MCPWM_EVT_TIMER2_TEZ_EN_V << MCPWM_EVT_TIMER2_TEZ_EN_S)
#define MCPWM_EVT_TIMER2_TEZ_EN_V  0x00000001U
#define MCPWM_EVT_TIMER2_TEZ_EN_S  5
/** MCPWM_EVT_TIMER0_TEP_EN : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to enable timer0 equal period event generate.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_EVT_TIMER0_TEP_EN    (BIT(6))
#define MCPWM_EVT_TIMER0_TEP_EN_M  (MCPWM_EVT_TIMER0_TEP_EN_V << MCPWM_EVT_TIMER0_TEP_EN_S)
#define MCPWM_EVT_TIMER0_TEP_EN_V  0x00000001U
#define MCPWM_EVT_TIMER0_TEP_EN_S  6
/** MCPWM_EVT_TIMER1_TEP_EN : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable timer1 equal period event generate.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_EVT_TIMER1_TEP_EN    (BIT(7))
#define MCPWM_EVT_TIMER1_TEP_EN_M  (MCPWM_EVT_TIMER1_TEP_EN_V << MCPWM_EVT_TIMER1_TEP_EN_S)
#define MCPWM_EVT_TIMER1_TEP_EN_V  0x00000001U
#define MCPWM_EVT_TIMER1_TEP_EN_S  7
/** MCPWM_EVT_TIMER2_TEP_EN : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to enable timer2 equal period event generate.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_EVT_TIMER2_TEP_EN    (BIT(8))
#define MCPWM_EVT_TIMER2_TEP_EN_M  (MCPWM_EVT_TIMER2_TEP_EN_V << MCPWM_EVT_TIMER2_TEP_EN_S)
#define MCPWM_EVT_TIMER2_TEP_EN_V  0x00000001U
#define MCPWM_EVT_TIMER2_TEP_EN_S  8
/** MCPWM_EVT_OP0_TEA_EN : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable PWM generator0 timer equal a event
 *  generate.\\0: Disable\\1: Enable
 */
#define MCPWM_EVT_OP0_TEA_EN    (BIT(9))
#define MCPWM_EVT_OP0_TEA_EN_M  (MCPWM_EVT_OP0_TEA_EN_V << MCPWM_EVT_OP0_TEA_EN_S)
#define MCPWM_EVT_OP0_TEA_EN_V  0x00000001U
#define MCPWM_EVT_OP0_TEA_EN_S  9
/** MCPWM_EVT_OP1_TEA_EN : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable PWM generator1 timer equal a event
 *  generate.\\0: Disable\\1: Enable
 */
#define MCPWM_EVT_OP1_TEA_EN    (BIT(10))
#define MCPWM_EVT_OP1_TEA_EN_M  (MCPWM_EVT_OP1_TEA_EN_V << MCPWM_EVT_OP1_TEA_EN_S)
#define MCPWM_EVT_OP1_TEA_EN_V  0x00000001U
#define MCPWM_EVT_OP1_TEA_EN_S  10
/** MCPWM_EVT_OP2_TEA_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable PWM generator2 timer equal a event
 *  generate.\\0: Disable\\1: Enable
 */
#define MCPWM_EVT_OP2_TEA_EN    (BIT(11))
#define MCPWM_EVT_OP2_TEA_EN_M  (MCPWM_EVT_OP2_TEA_EN_V << MCPWM_EVT_OP2_TEA_EN_S)
#define MCPWM_EVT_OP2_TEA_EN_V  0x00000001U
#define MCPWM_EVT_OP2_TEA_EN_S  11
/** MCPWM_EVT_OP0_TEB_EN : R/W; bitpos: [12]; default: 0;
 *  Configures whether or not to enable PWM generator0 timer equal b event
 *  generate.\\0: Disable\\1: Enable
 */
#define MCPWM_EVT_OP0_TEB_EN    (BIT(12))
#define MCPWM_EVT_OP0_TEB_EN_M  (MCPWM_EVT_OP0_TEB_EN_V << MCPWM_EVT_OP0_TEB_EN_S)
#define MCPWM_EVT_OP0_TEB_EN_V  0x00000001U
#define MCPWM_EVT_OP0_TEB_EN_S  12
/** MCPWM_EVT_OP1_TEB_EN : R/W; bitpos: [13]; default: 0;
 *  Configures whether or not to enable PWM generator1 timer equal b event
 *  generate.\\0: Disable\\1: Enable
 */
#define MCPWM_EVT_OP1_TEB_EN    (BIT(13))
#define MCPWM_EVT_OP1_TEB_EN_M  (MCPWM_EVT_OP1_TEB_EN_V << MCPWM_EVT_OP1_TEB_EN_S)
#define MCPWM_EVT_OP1_TEB_EN_V  0x00000001U
#define MCPWM_EVT_OP1_TEB_EN_S  13
/** MCPWM_EVT_OP2_TEB_EN : R/W; bitpos: [14]; default: 0;
 *  Configures whether or not to enable PWM generator2 timer equal b event
 *  generate.\\0: Disable\\1: Enable
 */
#define MCPWM_EVT_OP2_TEB_EN    (BIT(14))
#define MCPWM_EVT_OP2_TEB_EN_M  (MCPWM_EVT_OP2_TEB_EN_V << MCPWM_EVT_OP2_TEB_EN_S)
#define MCPWM_EVT_OP2_TEB_EN_V  0x00000001U
#define MCPWM_EVT_OP2_TEB_EN_S  14
/** MCPWM_EVT_F0_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable fault0 event generate.\\0: Disable\\1: Enable
 */
#define MCPWM_EVT_F0_EN    (BIT(15))
#define MCPWM_EVT_F0_EN_M  (MCPWM_EVT_F0_EN_V << MCPWM_EVT_F0_EN_S)
#define MCPWM_EVT_F0_EN_V  0x00000001U
#define MCPWM_EVT_F0_EN_S  15
/** MCPWM_EVT_F1_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable fault1 event generate.\\0: Disable\\1: Enable
 */
#define MCPWM_EVT_F1_EN    (BIT(16))
#define MCPWM_EVT_F1_EN_M  (MCPWM_EVT_F1_EN_V << MCPWM_EVT_F1_EN_S)
#define MCPWM_EVT_F1_EN_V  0x00000001U
#define MCPWM_EVT_F1_EN_S  16
/** MCPWM_EVT_F2_EN : R/W; bitpos: [17]; default: 0;
 *  Configures whether or not to enable fault2 event generate.\\0: Disable\\1: Enable
 */
#define MCPWM_EVT_F2_EN    (BIT(17))
#define MCPWM_EVT_F2_EN_M  (MCPWM_EVT_F2_EN_V << MCPWM_EVT_F2_EN_S)
#define MCPWM_EVT_F2_EN_V  0x00000001U
#define MCPWM_EVT_F2_EN_S  17
/** MCPWM_EVT_F0_CLR_EN : R/W; bitpos: [18]; default: 0;
 *  Configures whether or not to enable fault0 clear event generate.\\0: Disable\\1:
 *  Enable
 */
#define MCPWM_EVT_F0_CLR_EN    (BIT(18))
#define MCPWM_EVT_F0_CLR_EN_M  (MCPWM_EVT_F0_CLR_EN_V << MCPWM_EVT_F0_CLR_EN_S)
#define MCPWM_EVT_F0_CLR_EN_V  0x00000001U
#define MCPWM_EVT_F0_CLR_EN_S  18
/** MCPWM_EVT_F1_CLR_EN : R/W; bitpos: [19]; default: 0;
 *  Configures whether or not to enable fault1 clear event generate.\\0: Disable\\1:
 *  Enable
 */
#define MCPWM_EVT_F1_CLR_EN    (BIT(19))
#define MCPWM_EVT_F1_CLR_EN_M  (MCPWM_EVT_F1_CLR_EN_V << MCPWM_EVT_F1_CLR_EN_S)
#define MCPWM_EVT_F1_CLR_EN_V  0x00000001U
#define MCPWM_EVT_F1_CLR_EN_S  19
/** MCPWM_EVT_F2_CLR_EN : R/W; bitpos: [20]; default: 0;
 *  Configures whether or not to enable fault2 clear event generate.\\0: Disable\\1:
 *  Enable
 */
#define MCPWM_EVT_F2_CLR_EN    (BIT(20))
#define MCPWM_EVT_F2_CLR_EN_M  (MCPWM_EVT_F2_CLR_EN_V << MCPWM_EVT_F2_CLR_EN_S)
#define MCPWM_EVT_F2_CLR_EN_V  0x00000001U
#define MCPWM_EVT_F2_CLR_EN_S  20
/** MCPWM_EVT_TZ0_CBC_EN : R/W; bitpos: [21]; default: 0;
 *  Configures whether or not to enable cycle-by-cycle trip0 event generate.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_EVT_TZ0_CBC_EN    (BIT(21))
#define MCPWM_EVT_TZ0_CBC_EN_M  (MCPWM_EVT_TZ0_CBC_EN_V << MCPWM_EVT_TZ0_CBC_EN_S)
#define MCPWM_EVT_TZ0_CBC_EN_V  0x00000001U
#define MCPWM_EVT_TZ0_CBC_EN_S  21
/** MCPWM_EVT_TZ1_CBC_EN : R/W; bitpos: [22]; default: 0;
 *  Configures whether or not to enable cycle-by-cycle trip1 event generate.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_EVT_TZ1_CBC_EN    (BIT(22))
#define MCPWM_EVT_TZ1_CBC_EN_M  (MCPWM_EVT_TZ1_CBC_EN_V << MCPWM_EVT_TZ1_CBC_EN_S)
#define MCPWM_EVT_TZ1_CBC_EN_V  0x00000001U
#define MCPWM_EVT_TZ1_CBC_EN_S  22
/** MCPWM_EVT_TZ2_CBC_EN : R/W; bitpos: [23]; default: 0;
 *  Configures whether or not to enable cycle-by-cycle trip2 event generate.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_EVT_TZ2_CBC_EN    (BIT(23))
#define MCPWM_EVT_TZ2_CBC_EN_M  (MCPWM_EVT_TZ2_CBC_EN_V << MCPWM_EVT_TZ2_CBC_EN_S)
#define MCPWM_EVT_TZ2_CBC_EN_V  0x00000001U
#define MCPWM_EVT_TZ2_CBC_EN_S  23
/** MCPWM_EVT_TZ0_OST_EN : R/W; bitpos: [24]; default: 0;
 *  Configures whether or not to enable one-shot trip0 event generate.\\0: Disable\\1:
 *  Enable
 */
#define MCPWM_EVT_TZ0_OST_EN    (BIT(24))
#define MCPWM_EVT_TZ0_OST_EN_M  (MCPWM_EVT_TZ0_OST_EN_V << MCPWM_EVT_TZ0_OST_EN_S)
#define MCPWM_EVT_TZ0_OST_EN_V  0x00000001U
#define MCPWM_EVT_TZ0_OST_EN_S  24
/** MCPWM_EVT_TZ1_OST_EN : R/W; bitpos: [25]; default: 0;
 *  Configures whether or not to enable one-shot trip1 event generate.\\0: Disable\\1:
 *  Enable
 */
#define MCPWM_EVT_TZ1_OST_EN    (BIT(25))
#define MCPWM_EVT_TZ1_OST_EN_M  (MCPWM_EVT_TZ1_OST_EN_V << MCPWM_EVT_TZ1_OST_EN_S)
#define MCPWM_EVT_TZ1_OST_EN_V  0x00000001U
#define MCPWM_EVT_TZ1_OST_EN_S  25
/** MCPWM_EVT_TZ2_OST_EN : R/W; bitpos: [26]; default: 0;
 *  Configures whether or not to enable one-shot trip2 event generate.\\0: Disable\\1:
 *  Enable
 */
#define MCPWM_EVT_TZ2_OST_EN    (BIT(26))
#define MCPWM_EVT_TZ2_OST_EN_M  (MCPWM_EVT_TZ2_OST_EN_V << MCPWM_EVT_TZ2_OST_EN_S)
#define MCPWM_EVT_TZ2_OST_EN_V  0x00000001U
#define MCPWM_EVT_TZ2_OST_EN_S  26
/** MCPWM_EVT_CAP0_EN : R/W; bitpos: [27]; default: 0;
 *  Configures whether or not to enable capture0 event generate.\\0: Disable\\1: Enable
 */
#define MCPWM_EVT_CAP0_EN    (BIT(27))
#define MCPWM_EVT_CAP0_EN_M  (MCPWM_EVT_CAP0_EN_V << MCPWM_EVT_CAP0_EN_S)
#define MCPWM_EVT_CAP0_EN_V  0x00000001U
#define MCPWM_EVT_CAP0_EN_S  27
/** MCPWM_EVT_CAP1_EN : R/W; bitpos: [28]; default: 0;
 *  Configures whether or not to enable capture1 event generate.\\0: Disable\\1: Enable
 */
#define MCPWM_EVT_CAP1_EN    (BIT(28))
#define MCPWM_EVT_CAP1_EN_M  (MCPWM_EVT_CAP1_EN_V << MCPWM_EVT_CAP1_EN_S)
#define MCPWM_EVT_CAP1_EN_V  0x00000001U
#define MCPWM_EVT_CAP1_EN_S  28
/** MCPWM_EVT_CAP2_EN : R/W; bitpos: [29]; default: 0;
 *  Configures whether or not to enable capture2 event generate.\\0: Disable\\1: Enable
 */
#define MCPWM_EVT_CAP2_EN    (BIT(29))
#define MCPWM_EVT_CAP2_EN_M  (MCPWM_EVT_CAP2_EN_V << MCPWM_EVT_CAP2_EN_S)
#define MCPWM_EVT_CAP2_EN_V  0x00000001U
#define MCPWM_EVT_CAP2_EN_S  29

/** MCPWM_TASK_EN_REG register
 *  Task enable register
 */
#define MCPWM_TASK_EN_REG (DR_REG_MCPWM_BASE + 0x124)
/** MCPWM_TASK_CMPR0_A_UP_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable PWM generator0 timer stamp A's shadow register
 *  update task receive.\\0: Disable\\1: Enable
 */
#define MCPWM_TASK_CMPR0_A_UP_EN    (BIT(0))
#define MCPWM_TASK_CMPR0_A_UP_EN_M  (MCPWM_TASK_CMPR0_A_UP_EN_V << MCPWM_TASK_CMPR0_A_UP_EN_S)
#define MCPWM_TASK_CMPR0_A_UP_EN_V  0x00000001U
#define MCPWM_TASK_CMPR0_A_UP_EN_S  0
/** MCPWM_TASK_CMPR1_A_UP_EN : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enable PWM generator1 timer stamp A's shadow register
 *  update task receive.\\0: Disable\\1: Enable
 */
#define MCPWM_TASK_CMPR1_A_UP_EN    (BIT(1))
#define MCPWM_TASK_CMPR1_A_UP_EN_M  (MCPWM_TASK_CMPR1_A_UP_EN_V << MCPWM_TASK_CMPR1_A_UP_EN_S)
#define MCPWM_TASK_CMPR1_A_UP_EN_V  0x00000001U
#define MCPWM_TASK_CMPR1_A_UP_EN_S  1
/** MCPWM_TASK_CMPR2_A_UP_EN : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable PWM generator2 timer stamp A's shadow register
 *  update task receive.\\0: Disable\\1: Enable
 */
#define MCPWM_TASK_CMPR2_A_UP_EN    (BIT(2))
#define MCPWM_TASK_CMPR2_A_UP_EN_M  (MCPWM_TASK_CMPR2_A_UP_EN_V << MCPWM_TASK_CMPR2_A_UP_EN_S)
#define MCPWM_TASK_CMPR2_A_UP_EN_V  0x00000001U
#define MCPWM_TASK_CMPR2_A_UP_EN_S  2
/** MCPWM_TASK_CMPR0_B_UP_EN : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable PWM generator0 timer stamp B's shadow register
 *  update task receive.\\0: Disable\\1: Enable
 */
#define MCPWM_TASK_CMPR0_B_UP_EN    (BIT(3))
#define MCPWM_TASK_CMPR0_B_UP_EN_M  (MCPWM_TASK_CMPR0_B_UP_EN_V << MCPWM_TASK_CMPR0_B_UP_EN_S)
#define MCPWM_TASK_CMPR0_B_UP_EN_V  0x00000001U
#define MCPWM_TASK_CMPR0_B_UP_EN_S  3
/** MCPWM_TASK_CMPR1_B_UP_EN : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable PWM generator1 timer stamp B's shadow register
 *  update task receive.\\0: Disable\\1: Enable
 */
#define MCPWM_TASK_CMPR1_B_UP_EN    (BIT(4))
#define MCPWM_TASK_CMPR1_B_UP_EN_M  (MCPWM_TASK_CMPR1_B_UP_EN_V << MCPWM_TASK_CMPR1_B_UP_EN_S)
#define MCPWM_TASK_CMPR1_B_UP_EN_V  0x00000001U
#define MCPWM_TASK_CMPR1_B_UP_EN_S  4
/** MCPWM_TASK_CMPR2_B_UP_EN : R/W; bitpos: [5]; default: 0;
 *  Configures whether or not to enable PWM generator2 timer stamp B's shadow register
 *  update task receive.\\0: Disable\\1: Enable
 */
#define MCPWM_TASK_CMPR2_B_UP_EN    (BIT(5))
#define MCPWM_TASK_CMPR2_B_UP_EN_M  (MCPWM_TASK_CMPR2_B_UP_EN_V << MCPWM_TASK_CMPR2_B_UP_EN_S)
#define MCPWM_TASK_CMPR2_B_UP_EN_V  0x00000001U
#define MCPWM_TASK_CMPR2_B_UP_EN_S  5
/** MCPWM_TASK_GEN_STOP_EN : R/W; bitpos: [6]; default: 0;
 *  Configures whether or not to enable all PWM generate stop task receive.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_TASK_GEN_STOP_EN    (BIT(6))
#define MCPWM_TASK_GEN_STOP_EN_M  (MCPWM_TASK_GEN_STOP_EN_V << MCPWM_TASK_GEN_STOP_EN_S)
#define MCPWM_TASK_GEN_STOP_EN_V  0x00000001U
#define MCPWM_TASK_GEN_STOP_EN_S  6
/** MCPWM_TASK_TIMER0_SYNC_EN : R/W; bitpos: [7]; default: 0;
 *  Configures whether or not to enable timer0 sync task receive.\\0: Disable\\1: Enable
 */
#define MCPWM_TASK_TIMER0_SYNC_EN    (BIT(7))
#define MCPWM_TASK_TIMER0_SYNC_EN_M  (MCPWM_TASK_TIMER0_SYNC_EN_V << MCPWM_TASK_TIMER0_SYNC_EN_S)
#define MCPWM_TASK_TIMER0_SYNC_EN_V  0x00000001U
#define MCPWM_TASK_TIMER0_SYNC_EN_S  7
/** MCPWM_TASK_TIMER1_SYNC_EN : R/W; bitpos: [8]; default: 0;
 *  Configures whether or not to enable timer1 sync task receive.\\0: Disable\\1: Enable
 */
#define MCPWM_TASK_TIMER1_SYNC_EN    (BIT(8))
#define MCPWM_TASK_TIMER1_SYNC_EN_M  (MCPWM_TASK_TIMER1_SYNC_EN_V << MCPWM_TASK_TIMER1_SYNC_EN_S)
#define MCPWM_TASK_TIMER1_SYNC_EN_V  0x00000001U
#define MCPWM_TASK_TIMER1_SYNC_EN_S  8
/** MCPWM_TASK_TIMER2_SYNC_EN : R/W; bitpos: [9]; default: 0;
 *  Configures whether or not to enable timer2 sync task receive.\\0: Disable\\1: Enable
 */
#define MCPWM_TASK_TIMER2_SYNC_EN    (BIT(9))
#define MCPWM_TASK_TIMER2_SYNC_EN_M  (MCPWM_TASK_TIMER2_SYNC_EN_V << MCPWM_TASK_TIMER2_SYNC_EN_S)
#define MCPWM_TASK_TIMER2_SYNC_EN_V  0x00000001U
#define MCPWM_TASK_TIMER2_SYNC_EN_S  9
/** MCPWM_TASK_TIMER0_PERIOD_UP_EN : R/W; bitpos: [10]; default: 0;
 *  Configures whether or not to enable timer0 period update task receive.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_TASK_TIMER0_PERIOD_UP_EN    (BIT(10))
#define MCPWM_TASK_TIMER0_PERIOD_UP_EN_M  (MCPWM_TASK_TIMER0_PERIOD_UP_EN_V << MCPWM_TASK_TIMER0_PERIOD_UP_EN_S)
#define MCPWM_TASK_TIMER0_PERIOD_UP_EN_V  0x00000001U
#define MCPWM_TASK_TIMER0_PERIOD_UP_EN_S  10
/** MCPWM_TASK_TIMER1_PERIOD_UP_EN : R/W; bitpos: [11]; default: 0;
 *  Configures whether or not to enable timer1 period update task receive.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_TASK_TIMER1_PERIOD_UP_EN    (BIT(11))
#define MCPWM_TASK_TIMER1_PERIOD_UP_EN_M  (MCPWM_TASK_TIMER1_PERIOD_UP_EN_V << MCPWM_TASK_TIMER1_PERIOD_UP_EN_S)
#define MCPWM_TASK_TIMER1_PERIOD_UP_EN_V  0x00000001U
#define MCPWM_TASK_TIMER1_PERIOD_UP_EN_S  11
/** MCPWM_TASK_TIMER2_PERIOD_UP_EN : R/W; bitpos: [12]; default: 0;
 *  Configures whether or not to enable timer2 period update task receive.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_TASK_TIMER2_PERIOD_UP_EN    (BIT(12))
#define MCPWM_TASK_TIMER2_PERIOD_UP_EN_M  (MCPWM_TASK_TIMER2_PERIOD_UP_EN_V << MCPWM_TASK_TIMER2_PERIOD_UP_EN_S)
#define MCPWM_TASK_TIMER2_PERIOD_UP_EN_V  0x00000001U
#define MCPWM_TASK_TIMER2_PERIOD_UP_EN_S  12
/** MCPWM_TASK_TZ0_OST_EN : R/W; bitpos: [13]; default: 0;
 *  Configures whether or not to enable one shot trip0 task receive.\\0: Disable\\1:
 *  Enable
 */
#define MCPWM_TASK_TZ0_OST_EN    (BIT(13))
#define MCPWM_TASK_TZ0_OST_EN_M  (MCPWM_TASK_TZ0_OST_EN_V << MCPWM_TASK_TZ0_OST_EN_S)
#define MCPWM_TASK_TZ0_OST_EN_V  0x00000001U
#define MCPWM_TASK_TZ0_OST_EN_S  13
/** MCPWM_TASK_TZ1_OST_EN : R/W; bitpos: [14]; default: 0;
 *  Configures whether or not to enable one shot trip1 task receive.\\0: Disable\\1:
 *  Enable
 */
#define MCPWM_TASK_TZ1_OST_EN    (BIT(14))
#define MCPWM_TASK_TZ1_OST_EN_M  (MCPWM_TASK_TZ1_OST_EN_V << MCPWM_TASK_TZ1_OST_EN_S)
#define MCPWM_TASK_TZ1_OST_EN_V  0x00000001U
#define MCPWM_TASK_TZ1_OST_EN_S  14
/** MCPWM_TASK_TZ2_OST_EN : R/W; bitpos: [15]; default: 0;
 *  Configures whether or not to enable one shot trip2 task receive.\\0: Disable\\1:
 *  Enable
 */
#define MCPWM_TASK_TZ2_OST_EN    (BIT(15))
#define MCPWM_TASK_TZ2_OST_EN_M  (MCPWM_TASK_TZ2_OST_EN_V << MCPWM_TASK_TZ2_OST_EN_S)
#define MCPWM_TASK_TZ2_OST_EN_V  0x00000001U
#define MCPWM_TASK_TZ2_OST_EN_S  15
/** MCPWM_TASK_CLR0_OST_EN : R/W; bitpos: [16]; default: 0;
 *  Configures whether or not to enable one shot trip0 clear task receive.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_TASK_CLR0_OST_EN    (BIT(16))
#define MCPWM_TASK_CLR0_OST_EN_M  (MCPWM_TASK_CLR0_OST_EN_V << MCPWM_TASK_CLR0_OST_EN_S)
#define MCPWM_TASK_CLR0_OST_EN_V  0x00000001U
#define MCPWM_TASK_CLR0_OST_EN_S  16
/** MCPWM_TASK_CLR1_OST_EN : R/W; bitpos: [17]; default: 0;
 *  Configures whether or not to enable one shot trip1 clear task receive.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_TASK_CLR1_OST_EN    (BIT(17))
#define MCPWM_TASK_CLR1_OST_EN_M  (MCPWM_TASK_CLR1_OST_EN_V << MCPWM_TASK_CLR1_OST_EN_S)
#define MCPWM_TASK_CLR1_OST_EN_V  0x00000001U
#define MCPWM_TASK_CLR1_OST_EN_S  17
/** MCPWM_TASK_CLR2_OST_EN : R/W; bitpos: [18]; default: 0;
 *  Configures whether or not to enable one shot trip2 clear task receive.\\0:
 *  Disable\\1: Enable
 */
#define MCPWM_TASK_CLR2_OST_EN    (BIT(18))
#define MCPWM_TASK_CLR2_OST_EN_M  (MCPWM_TASK_CLR2_OST_EN_V << MCPWM_TASK_CLR2_OST_EN_S)
#define MCPWM_TASK_CLR2_OST_EN_V  0x00000001U
#define MCPWM_TASK_CLR2_OST_EN_S  18
/** MCPWM_TASK_CAP0_EN : R/W; bitpos: [19]; default: 0;
 *  Configures whether or not to enable capture0 task receive.\\0: Disable\\1: Enable
 */
#define MCPWM_TASK_CAP0_EN    (BIT(19))
#define MCPWM_TASK_CAP0_EN_M  (MCPWM_TASK_CAP0_EN_V << MCPWM_TASK_CAP0_EN_S)
#define MCPWM_TASK_CAP0_EN_V  0x00000001U
#define MCPWM_TASK_CAP0_EN_S  19
/** MCPWM_TASK_CAP1_EN : R/W; bitpos: [20]; default: 0;
 *  Configures whether or not to enable capture1 task receive.\\0: Disable\\1: Enable
 */
#define MCPWM_TASK_CAP1_EN    (BIT(20))
#define MCPWM_TASK_CAP1_EN_M  (MCPWM_TASK_CAP1_EN_V << MCPWM_TASK_CAP1_EN_S)
#define MCPWM_TASK_CAP1_EN_V  0x00000001U
#define MCPWM_TASK_CAP1_EN_S  20
/** MCPWM_TASK_CAP2_EN : R/W; bitpos: [21]; default: 0;
 *  Configures whether or not to enable capture2 task receive.\\0: Disable\\1: Enable
 */
#define MCPWM_TASK_CAP2_EN    (BIT(21))
#define MCPWM_TASK_CAP2_EN_M  (MCPWM_TASK_CAP2_EN_V << MCPWM_TASK_CAP2_EN_S)
#define MCPWM_TASK_CAP2_EN_V  0x00000001U
#define MCPWM_TASK_CAP2_EN_S  21

/** MCPWM_EVT_EN2_REG register
 *  Event enable register2
 */
#define MCPWM_EVT_EN2_REG (DR_REG_MCPWM_BASE + 0x128)
/** MCPWM_EVT_OP0_TEE1_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to enable PWM generator0 timer equal OP0_TSTMP_E1_REG
 *  event generate.\\0: Disable\\1: Enable
 */
#define MCPWM_EVT_OP0_TEE1_EN    (BIT(0))
#define MCPWM_EVT_OP0_TEE1_EN_M  (MCPWM_EVT_OP0_TEE1_EN_V << MCPWM_EVT_OP0_TEE1_EN_S)
#define MCPWM_EVT_OP0_TEE1_EN_V  0x00000001U
#define MCPWM_EVT_OP0_TEE1_EN_S  0
/** MCPWM_EVT_OP1_TEE1_EN : R/W; bitpos: [1]; default: 0;
 *  Configures whether or not to enable PWM generator1 timer equal OP1_TSTMP_E1_REG
 *  event generate.\\0: Disable\\1: Enable
 */
#define MCPWM_EVT_OP1_TEE1_EN    (BIT(1))
#define MCPWM_EVT_OP1_TEE1_EN_M  (MCPWM_EVT_OP1_TEE1_EN_V << MCPWM_EVT_OP1_TEE1_EN_S)
#define MCPWM_EVT_OP1_TEE1_EN_V  0x00000001U
#define MCPWM_EVT_OP1_TEE1_EN_S  1
/** MCPWM_EVT_OP2_TEE1_EN : R/W; bitpos: [2]; default: 0;
 *  Configures whether or not to enable PWM generator2 timer equal OP2_TSTMP_E1_REG
 *  event generate.\\0: Disable\\1: Enable
 */
#define MCPWM_EVT_OP2_TEE1_EN    (BIT(2))
#define MCPWM_EVT_OP2_TEE1_EN_M  (MCPWM_EVT_OP2_TEE1_EN_V << MCPWM_EVT_OP2_TEE1_EN_S)
#define MCPWM_EVT_OP2_TEE1_EN_V  0x00000001U
#define MCPWM_EVT_OP2_TEE1_EN_S  2
/** MCPWM_EVT_OP0_TEE2_EN : R/W; bitpos: [3]; default: 0;
 *  Configures whether or not to enable PWM generator0 timer equal OP0_TSTMP_E2_REG
 *  event generate.\\0: Disable\\1: Enable
 */
#define MCPWM_EVT_OP0_TEE2_EN    (BIT(3))
#define MCPWM_EVT_OP0_TEE2_EN_M  (MCPWM_EVT_OP0_TEE2_EN_V << MCPWM_EVT_OP0_TEE2_EN_S)
#define MCPWM_EVT_OP0_TEE2_EN_V  0x00000001U
#define MCPWM_EVT_OP0_TEE2_EN_S  3
/** MCPWM_EVT_OP1_TEE2_EN : R/W; bitpos: [4]; default: 0;
 *  Configures whether or not to enable PWM generator1 timer equal OP1_TSTMP_E2_REG
 *  event generate.\\0: Disable\\1: Enable
 */
#define MCPWM_EVT_OP1_TEE2_EN    (BIT(4))
#define MCPWM_EVT_OP1_TEE2_EN_M  (MCPWM_EVT_OP1_TEE2_EN_V << MCPWM_EVT_OP1_TEE2_EN_S)
#define MCPWM_EVT_OP1_TEE2_EN_V  0x00000001U
#define MCPWM_EVT_OP1_TEE2_EN_S  4
/** MCPWM_EVT_OP2_TEE2_EN : R/W; bitpos: [5]; default: 0;
 *  Configures whether or not to enable PWM generator2 timer equal OP2_TSTMP_E2_REG
 *  event generate.\\0: Disable\\1: Enable
 */
#define MCPWM_EVT_OP2_TEE2_EN    (BIT(5))
#define MCPWM_EVT_OP2_TEE2_EN_M  (MCPWM_EVT_OP2_TEE2_EN_V << MCPWM_EVT_OP2_TEE2_EN_S)
#define MCPWM_EVT_OP2_TEE2_EN_V  0x00000001U
#define MCPWM_EVT_OP2_TEE2_EN_S  5

/** MCPWM_OP0_TSTMP_E1_REG register
 *  Generator0 timer stamp E1 value register
 */
#define MCPWM_OP0_TSTMP_E1_REG (DR_REG_MCPWM_BASE + 0x12c)
/** MCPWM_OP0_TSTMP_E1 : R/W; bitpos: [15:0]; default: 0;
 *  Configures generator0 timer stamp E1 value register
 */
#define MCPWM_OP0_TSTMP_E1    0x0000FFFFU
#define MCPWM_OP0_TSTMP_E1_M  (MCPWM_OP0_TSTMP_E1_V << MCPWM_OP0_TSTMP_E1_S)
#define MCPWM_OP0_TSTMP_E1_V  0x0000FFFFU
#define MCPWM_OP0_TSTMP_E1_S  0

/** MCPWM_OP0_TSTMP_E2_REG register
 *  Generator$n timer stamp E2 value register
 */
#define MCPWM_OP0_TSTMP_E2_REG (DR_REG_MCPWM_BASE + 0x130)
/** MCPWM_OP0_TSTMP_E2 : R/W; bitpos: [15:0]; default: 0;
 *  Configures generator$n timer stamp E2 value register
 */
#define MCPWM_OP0_TSTMP_E2    0x0000FFFFU
#define MCPWM_OP0_TSTMP_E2_M  (MCPWM_OP0_TSTMP_E2_V << MCPWM_OP0_TSTMP_E2_S)
#define MCPWM_OP0_TSTMP_E2_V  0x0000FFFFU
#define MCPWM_OP0_TSTMP_E2_S  0

/** MCPWM_OP1_TSTMP_E1_REG register
 *  Generator1 timer stamp E1 value register
 */
#define MCPWM_OP1_TSTMP_E1_REG (DR_REG_MCPWM_BASE + 0x134)
/** MCPWM_OP0_TSTMP_E1 : R/W; bitpos: [15:0]; default: 0;
 *  Configures generator1 timer stamp E1 value register
 */
#define MCPWM_OP0_TSTMP_E1    0x0000FFFFU
#define MCPWM_OP0_TSTMP_E1_M  (MCPWM_OP0_TSTMP_E1_V << MCPWM_OP0_TSTMP_E1_S)
#define MCPWM_OP0_TSTMP_E1_V  0x0000FFFFU
#define MCPWM_OP0_TSTMP_E1_S  0

/** MCPWM_OP1_TSTMP_E2_REG register
 *  Generator$n timer stamp E2 value register
 */
#define MCPWM_OP1_TSTMP_E2_REG (DR_REG_MCPWM_BASE + 0x138)
/** MCPWM_OP1_TSTMP_E2 : R/W; bitpos: [15:0]; default: 0;
 *  Configures generator$n timer stamp E2 value register
 */
#define MCPWM_OP1_TSTMP_E2    0x0000FFFFU
#define MCPWM_OP1_TSTMP_E2_M  (MCPWM_OP1_TSTMP_E2_V << MCPWM_OP1_TSTMP_E2_S)
#define MCPWM_OP1_TSTMP_E2_V  0x0000FFFFU
#define MCPWM_OP1_TSTMP_E2_S  0

/** MCPWM_OP2_TSTMP_E1_REG register
 *  Generator2 timer stamp E1 value register
 */
#define MCPWM_OP2_TSTMP_E1_REG (DR_REG_MCPWM_BASE + 0x13c)
/** MCPWM_OP0_TSTMP_E1 : R/W; bitpos: [15:0]; default: 0;
 *  Configures generator2 timer stamp E1 value register
 */
#define MCPWM_OP0_TSTMP_E1    0x0000FFFFU
#define MCPWM_OP0_TSTMP_E1_M  (MCPWM_OP0_TSTMP_E1_V << MCPWM_OP0_TSTMP_E1_S)
#define MCPWM_OP0_TSTMP_E1_V  0x0000FFFFU
#define MCPWM_OP0_TSTMP_E1_S  0

/** MCPWM_OP2_TSTMP_E2_REG register
 *  Generator$n timer stamp E2 value register
 */
#define MCPWM_OP2_TSTMP_E2_REG (DR_REG_MCPWM_BASE + 0x140)
/** MCPWM_OP2_TSTMP_E2 : R/W; bitpos: [15:0]; default: 0;
 *  Configures generator$n timer stamp E2 value register
 */
#define MCPWM_OP2_TSTMP_E2    0x0000FFFFU
#define MCPWM_OP2_TSTMP_E2_M  (MCPWM_OP2_TSTMP_E2_V << MCPWM_OP2_TSTMP_E2_S)
#define MCPWM_OP2_TSTMP_E2_V  0x0000FFFFU
#define MCPWM_OP2_TSTMP_E2_S  0

/** MCPWM_CLK_REG register
 *  Global configuration register
 */
#define MCPWM_CLK_REG (DR_REG_MCPWM_BASE + 0x144)
/** MCPWM_CLK_EN : R/W; bitpos: [0]; default: 0;
 *  Configures whether or not to open register clock gate.\\0: Open the clock gate only
 *  when application writes registers\\1: Force open the clock gate for register
 */
#define MCPWM_CLK_EN    (BIT(0))
#define MCPWM_CLK_EN_M  (MCPWM_CLK_EN_V << MCPWM_CLK_EN_S)
#define MCPWM_CLK_EN_V  0x00000001U
#define MCPWM_CLK_EN_S  0

/** MCPWM_VERSION_REG register
 *  Version register.
 */
#define MCPWM_VERSION_REG (DR_REG_MCPWM_BASE + 0x148)
/** MCPWM_DATE : R/W; bitpos: [27:0]; default: 35725968;
 *  Configures the version.
 */
#define MCPWM_DATE    0x0FFFFFFFU
#define MCPWM_DATE_M  (MCPWM_DATE_V << MCPWM_DATE_S)
#define MCPWM_DATE_V  0x0FFFFFFFU
#define MCPWM_DATE_S  0

#ifdef __cplusplus
}
#endif
