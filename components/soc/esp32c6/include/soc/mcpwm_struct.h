/**
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Prescaler configuration */
/** Type of clk_cfg register
 *  PWM clock prescaler register.
 */
typedef union {
    struct {
        /** clk_prescale : R/W; bitpos: [7:0]; default: 0;
         *  Period of PWM_clk = 6.25ns * (PWM_CLK_PRESCALE + 1)
         */
        uint32_t clk_prescale:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} mcpwm_clk_cfg_reg_t;


/** Group: MCPWM Timer Configuration and status */
/** Type of timer_cfg0 register
 *  PWM timer period and update method configuration register.
 */
typedef union {
    struct {
        /** timer_prescale : R/W; bitpos: [7:0]; default: 0;
         *  period of PT0_clk = Period of PWM_clk * (PWM_TIMER_PRESCALE + 1)
         */
        uint32_t timer_prescale:8;
        /** timer_period : R/W; bitpos: [23:8]; default: 255;
         *  period shadow register of PWM timer
         */
        uint32_t timer_period:16;
        /** timer_period_upmethod : R/W; bitpos: [25:24]; default: 0;
         *  Update method for active register of PWM timer period, 0: immediate, 1: TEZ, 2:
         *  sync, 3: TEZ | sync. TEZ here and below means timer equal zero event
         */
        uint32_t timer_period_upmethod:2;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} mcpwm_timer_cfg0_reg_t;

/** Type of timer_cfg1 register
 *  PWM timer0 working mode and start/stop control configuration register.
 */
typedef union {
    struct {
        /** timer_start : R/W/SC; bitpos: [2:0]; default: 0;
         *  PWM timer start and stop control. 0: if PWM timer starts, then stops at TEZ, 1:
         *  if timer0 starts, then stops at TEP, 2: PWM timer starts and runs on, 3: timer
         *  starts and stops at the next TEZ, 4: timer starts and stops at the next TEP. TEP
         *  here and below means the event that happens when the timer equals to period
         */
        uint32_t timer_start:3;
        /** timer_mod : R/W; bitpos: [4:3]; default: 0;
         *  PWM timer working mode, 0: freeze, 1: increase mode, 2: decrease mode, 3: up-down
         *  mode
         */
        uint32_t timer_mod:2;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} mcpwm_timer_cfg1_reg_t;

/** Type of timer_sync register
 *  PWM timer sync function configuration register.
 */
typedef union {
    struct {
        /** timer_synci_en : R/W; bitpos: [0]; default: 0;
         *  When set, timer reloading with phase on sync input event is enabled.
         */
        uint32_t timer_synci_en:1;
        /** timer_sync_sw : R/W; bitpos: [1]; default: 0;
         *  Toggling this bit will trigger a software sync.
         */
        uint32_t timer_sync_sw:1;
        /** timer_synco_sel : R/W; bitpos: [3:2]; default: 0;
         *  PWM timer sync_out selection, 0: sync_in, 1: TEZ, 2: TEP, and sync out will always
         *  generate when toggling the reg_timer0_sync_sw bit
         */
        uint32_t timer_synco_sel:2;
        /** timer_phase : R/W; bitpos: [19:4]; default: 0;
         *  phase for timer reload on sync event
         */
        uint32_t timer_phase:16;
        /** timer_phase_direction : R/W; bitpos: [20]; default: 0;
         *  Configure the PWM timer0's direction when timer mode is up-down mode:
         *  0-increase,1-decrease
         */
        uint32_t timer_phase_direction:1;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} mcpwm_timer_sync_reg_t;

/** Type of timer_status register
 *  PWM timer status register.
 */
typedef union {
    struct {
        /** timer_value : RO; bitpos: [15:0]; default: 0;
         *  current PWM timer counter value
         */
        uint32_t timer_value:16;
        /** timer_direction : RO; bitpos: [16]; default: 0;
         *  current PWM timer counter direction, 0: increment 1: decrement
         */
        uint32_t timer_direction:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} mcpwm_timer_status_reg_t;


/** Group: Common configuration for MCPWM timers */
/** Type of timer_synci_cfg register
 *  Synchronization input selection for three PWM timers.
 */
typedef union {
    struct {
        /** timer0_syncisel : R/W; bitpos: [2:0]; default: 0;
         *  select sync input for PWM timer0, 1: PWM timer0 sync_out, 2: PWM timer1 sync_out,
         *  3: PWM timer2 sync_out, 4: SYNC0 from GPIO matrix, 5: SYNC1 from GPIO matrix, 6:
         *  SYNC2 from GPIO matrix, other values: no sync input selected
         */
        uint32_t timer0_syncisel:3;
        /** timer1_syncisel : R/W; bitpos: [5:3]; default: 0;
         *  select sync input for PWM timer1, 1: PWM timer0 sync_out, 2: PWM timer1 sync_out,
         *  3: PWM timer2 sync_out, 4: SYNC0 from GPIO matrix, 5: SYNC1 from GPIO matrix, 6:
         *  SYNC2 from GPIO matrix, other values: no sync input selected
         */
        uint32_t timer1_syncisel:3;
        /** timer2_syncisel : R/W; bitpos: [8:6]; default: 0;
         *  select sync input for PWM timer2, 1: PWM timer0 sync_out, 2: PWM timer1 sync_out,
         *  3: PWM timer2 sync_out, 4: SYNC0 from GPIO matrix, 5: SYNC1 from GPIO matrix, 6:
         *  SYNC2 from GPIO matrix, other values: no sync input selected
         */
        uint32_t timer2_syncisel:3;
        /** external_synci0_invert : R/W; bitpos: [9]; default: 0;
         *  invert SYNC0 from GPIO matrix
         */
        uint32_t external_synci0_invert:1;
        /** external_synci1_invert : R/W; bitpos: [10]; default: 0;
         *  invert SYNC1 from GPIO matrix
         */
        uint32_t external_synci1_invert:1;
        /** external_synci2_invert : R/W; bitpos: [11]; default: 0;
         *  invert SYNC2 from GPIO matrix
         */
        uint32_t external_synci2_invert:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} mcpwm_timer_synci_cfg_reg_t;

/** Type of operator_timersel register
 *  Select specific timer for PWM operators.
 */
typedef union {
    struct {
        /** operator0_timersel : R/W; bitpos: [1:0]; default: 0;
         *  Select which PWM timer's is the timing reference for PWM operator0, 0: timer0, 1:
         *  timer1, 2: timer2
         */
        uint32_t operator0_timersel:2;
        /** operator1_timersel : R/W; bitpos: [3:2]; default: 0;
         *  Select which PWM timer's is the timing reference for PWM operator1, 0: timer0, 1:
         *  timer1, 2: timer2
         */
        uint32_t operator1_timersel:2;
        /** operator2_timersel : R/W; bitpos: [5:4]; default: 0;
         *  Select which PWM timer's is the timing reference for PWM operator2, 0: timer0, 1:
         *  timer1, 2: timer2
         */
        uint32_t operator2_timersel:2;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} mcpwm_operator_timersel_reg_t;


/** Group: MCPWM Operator Configuration and Status */
/** Type of gen_stmp_cfg register
 *  Transfer status and update method for time stamp registers A and B
 */
typedef union {
    struct {
        /** cmpr_a_upmethod : R/W; bitpos: [3:0]; default: 0;
         *  Update method for PWM generator time stamp A's active register. When all bits are
         *  set to 0: immediately, when bit0 is set to 1: TEZ, when bit1 is set to 1: TEP,when
         *  bit2 is set to 1: sync, when bit3 is set to 1: disable the update.
         */
        uint32_t cmpr_a_upmethod:4;
        /** cmpr_b_upmethod : R/W; bitpos: [7:4]; default: 0;
         *  Update method for PWM generator time stamp B's active register. When all bits are
         *  set to 0: immediately, when bit0 is set to 1: TEZ, when bit1 is set to 1: TEP,when
         *  bit2 is set to 1: sync, when bit3 is set to 1: disable the update.
         */
        uint32_t cmpr_b_upmethod:4;
        /** cmpr_a_shdw_full : R/W/WTC/SC; bitpos: [8]; default: 0;
         *  Set and reset by hardware. If set, PWM generator time stamp A's shadow reg is
         *  filled and waiting to be transferred to A's active reg. If cleared, A's active reg
         *  has been updated with shadow register latest value
         */
        uint32_t cmpr_a_shdw_full:1;
        /** cmpr_b_shdw_full : R/W/WTC/SC; bitpos: [9]; default: 0;
         *  Set and reset by hardware. If set, PWM generator time stamp B's shadow reg is
         *  filled and waiting to be transferred to B's active reg. If cleared, B's active reg
         *  has been updated with shadow register latest value
         */
        uint32_t cmpr_b_shdw_full:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} mcpwm_gen_stmp_cfg_reg_t;

/** Type of gen_tstmp register
 *  Shadow register for register B.
 */
typedef union {
    struct {
        /** cmpr : R/W; bitpos: [15:0]; default: 0;
         *  PWM generator time stamp's shadow register
         */
        uint32_t cmpr:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} mcpwm_gen_tstmp_reg_t;

/** Type of gen_cfg0 register
 *  Fault event T0 and T1 handling
 */
typedef union {
    struct {
        /** gen_cfg_upmethod : R/W; bitpos: [3:0]; default: 0;
         *  Update method for PWM generator's active register of configuration. When all bits
         *  are set to 0: immediately, when bit0 is set to 1: TEZ, when bit1 is set to
         *  1:TEP,when bit2 is set to 1:sync,when bit3 is set to 1:disable the update
         */
        uint32_t gen_cfg_upmethod:4;
        /** gen_t0_sel : R/W; bitpos: [6:4]; default: 0;
         *  Source selection for PWM generator event_t0, take effect immediately, 0:
         *  fault_event0, 1: fault_event1, 2: fault_event2, 3: sync_taken, 4: none
         */
        uint32_t gen_t0_sel:3;
        /** gen_t1_sel : R/W; bitpos: [9:7]; default: 0;
         *  Source selection for PWM generator event_t1, take effect immediately, 0:
         *  fault_event0, 1: fault_event1, 2: fault_event2, 3: sync_taken, 4: none
         */
        uint32_t gen_t1_sel:3;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} mcpwm_gen_cfg0_reg_t;

/** Type of gen_force register
 *  Permissives to force PWM0A and PWM0B outputs by software
 */
typedef union {
    struct {
        /** gen_cntuforce_upmethod : R/W; bitpos: [5:0]; default: 32;
         *  Updating method for continuous software force of PWM generator. When all bits are
         *  set to 0: immediately, when bit0 is set to 1: TEZ,,when bit1 is set to 1: TEP, when
         *  bit2 is set to 1: TEA, when bit3 is set to 1: TEB, when bit4 is set to 1: sync,
         *  when bit5 is set to 1: disable update. (TEA/B here and below means an event
         *  generated when the timer's value equals to that of register A/B.)
         */
        uint32_t gen_cntuforce_upmethod:6;
        /** gen_a_cntuforce_mode : R/W; bitpos: [7:6]; default: 0;
         *  Continuous software force mode for PWMnA. 0: disabled, 1: low, 2: high, 3: disabled
         */
        uint32_t gen_a_cntuforce_mode:2;
        /** gen_b_cntuforce_mode : R/W; bitpos: [9:8]; default: 0;
         *  Continuous software force mode for PWMnB. 0: disabled, 1: low, 2: high, 3: disabled
         */
        uint32_t gen_b_cntuforce_mode:2;
        /** gen_a_nciforce : R/W; bitpos: [10]; default: 0;
         *  Trigger of non-continuous immediate software-force event for PWMnA, a toggle will
         *  trigger a force event.
         */
        uint32_t gen_a_nciforce:1;
        /** gen_a_nciforce_mode : R/W; bitpos: [12:11]; default: 0;
         *  non-continuous immediate software force mode for PWMnA, 0: disabled, 1: low, 2:
         *  high, 3: disabled
         */
        uint32_t gen_a_nciforce_mode:2;
        /** gen_b_nciforce : R/W; bitpos: [13]; default: 0;
         *  Trigger of non-continuous immediate software-force event for PWMnB, a toggle will
         *  trigger a force event.
         */
        uint32_t gen_b_nciforce:1;
        /** gen_b_nciforce_mode : R/W; bitpos: [15:14]; default: 0;
         *  non-continuous immediate software force mode for PWMnB, 0: disabled, 1: low, 2:
         *  high, 3: disabled
         */
        uint32_t gen_b_nciforce_mode:2;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} mcpwm_gen_force_reg_t;

/** Type of gen register
 *  Actions triggered by events on PWM0B
 */
typedef union {
    struct {
        /** gen_utez : R/W; bitpos: [1:0]; default: 0;
         *  Action on PWMnA/B triggered by event TEZ when timer increasing
         */
        uint32_t gen_utez:2;
        /** gen_utep : R/W; bitpos: [3:2]; default: 0;
         *  Action on PWMnA/B triggered by event TEP when timer increasing
         */
        uint32_t gen_utep:2;
        /** gen_utea : R/W; bitpos: [5:4]; default: 0;
         *  Action on PWMnA/B triggered by event TEA when timer increasing
         */
        uint32_t gen_utea:2;
        /** gen_uteb : R/W; bitpos: [7:6]; default: 0;
         *  Action on PWMnA/B triggered by event TEB when timer increasing
         */
        uint32_t gen_uteb:2;
        /** gen_ut0 : R/W; bitpos: [9:8]; default: 0;
         *  Action on PWMnA/B triggered by event_t0 when timer increasing
         */
        uint32_t gen_ut0:2;
        /** gen_ut1 : R/W; bitpos: [11:10]; default: 0;
         *  Action on PWMnA/B triggered by event_t1 when timer increasing
         */
        uint32_t gen_ut1:2;
        /** gen_dtez : R/W; bitpos: [13:12]; default: 0;
         *  Action on PWMnA/B triggered by event TEZ when timer decreasing
         */
        uint32_t gen_dtez:2;
        /** gen_dtep : R/W; bitpos: [15:14]; default: 0;
         *  Action on PWMnA/B triggered by event TEP when timer decreasing
         */
        uint32_t gen_dtep:2;
        /** gen_dtea : R/W; bitpos: [17:16]; default: 0;
         *  Action on PWMnA/B triggered by event TEA when timer decreasing
         */
        uint32_t gen_dtea:2;
        /** gen_dteb : R/W; bitpos: [19:18]; default: 0;
         *  Action on PWMnA/B triggered by event TEB when timer decreasing
         */
        uint32_t gen_dteb:2;
        /** gen_dt0 : R/W; bitpos: [21:20]; default: 0;
         *  Action on PWMnA/B triggered by event_t0 when timer decreasing
         */
        uint32_t gen_dt0:2;
        /** gen_dt1 : R/W; bitpos: [23:22]; default: 0;
         *  Action on PWMnA/B triggered by event_t1 when timer decreasing. 0: no change, 1: low,
         *  2: high, 3: toggle
         */
        uint32_t gen_dt1:2;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} mcpwm_gen_reg_t;

/** Type of dt_cfg register
 *  dead time type selection and configuration
 */
typedef union {
    struct {
        /** db_fed_upmethod : R/W; bitpos: [3:0]; default: 0;
         *  Update method for FED (rising edge delay) active register. 0: immediate, when bit0
         *  is set to 1: tez, when bit1 is set to 1:tep,  when bit2 is set to 1: sync, when
         *  bit3 is set to 1: disable the update
         */
        uint32_t db_fed_upmethod:4;
        /** db_red_upmethod : R/W; bitpos: [7:4]; default: 0;
         *  Update method for RED (rising edge delay) active register. 0: immediate, when bit0
         *  is set to 1: tez, when bit1 is set to 1:tep,  when bit2 is set to 1: sync, when
         *  bit3 is set to 1: disable the update
         */
        uint32_t db_red_upmethod:4;
        /** db_deb_mode : R/W; bitpos: [8]; default: 0;
         *  S8 in table, dual-edge B mode, 0: fed/red take effect on different path separately,
         *  1: fed/red take effect on B path, A out is in bypass or dulpB mode
         */
        uint32_t db_deb_mode:1;
        /** db_a_outswap : R/W; bitpos: [9]; default: 0;
         *  S6 in table
         */
        uint32_t db_a_outswap:1;
        /** db_b_outswap : R/W; bitpos: [10]; default: 0;
         *  S7 in table
         */
        uint32_t db_b_outswap:1;
        /** db_red_insel : R/W; bitpos: [11]; default: 0;
         *  S4 in table
         */
        uint32_t db_red_insel:1;
        /** db_fed_insel : R/W; bitpos: [12]; default: 0;
         *  S5 in table
         */
        uint32_t db_fed_insel:1;
        /** db_red_outinvert : R/W; bitpos: [13]; default: 0;
         *  S2 in table
         */
        uint32_t db_red_outinvert:1;
        /** db_fed_outinvert : R/W; bitpos: [14]; default: 0;
         *  S3 in table
         */
        uint32_t db_fed_outinvert:1;
        /** db_a_outbypass : R/W; bitpos: [15]; default: 1;
         *  S1 in table
         */
        uint32_t db_a_outbypass:1;
        /** db_b_outbypass : R/W; bitpos: [16]; default: 1;
         *  S0 in table
         */
        uint32_t db_b_outbypass:1;
        /** db_clk_sel : R/W; bitpos: [17]; default: 0;
         *  Dead time generator n clock selection. 0: PWM_clk, 1: PT_clk
         */
        uint32_t db_clk_sel:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} mcpwm_dt_cfg_reg_t;

/** Type of dt_fed_cfg register
 *  Shadow register for falling edge delay (FED).
 */
typedef union {
    struct {
        /** db_fed : R/W; bitpos: [15:0]; default: 0;
         *  Shadow register for FED
         */
        uint32_t db_fed:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} mcpwm_dt_fed_cfg_reg_t;

/** Type of dt_red_cfg register
 *  Shadow register for rising edge delay (RED).
 */
typedef union {
    struct {
        /** db_red : R/W; bitpos: [15:0]; default: 0;
         *  Shadow register for RED
         */
        uint32_t db_red:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} mcpwm_dt_red_cfg_reg_t;

/** Type of carrier_cfg register
 *  Carrier enable and configuratoin
 */
typedef union {
    struct {
        /** chopper_en : R/W; bitpos: [0]; default: 0;
         *  When set, carrier0 function is enabled. When cleared, carrier0 is bypassed
         */
        uint32_t chopper_en:1;
        /** chopper_prescale : R/W; bitpos: [4:1]; default: 0;
         *  PWM carrier0 clock (PC_clk) prescale value. Period of PC_clk = period of PWM_clk *
         *  (PWM_CARRIER0_PRESCALE + 1)
         */
        uint32_t chopper_prescale:4;
        /** chopper_duty : R/W; bitpos: [7:5]; default: 0;
         *  carrier duty selection. Duty = PWM_CARRIER0_DUTY / 8
         */
        uint32_t chopper_duty:3;
        /** chopper_oshtwth : R/W; bitpos: [11:8]; default: 0;
         *  width of the first pulse in number of periods of the carrier
         */
        uint32_t chopper_oshtwth:4;
        /** chopper_out_invert : R/W; bitpos: [12]; default: 0;
         *  when set, invert the output of PWM0A and PWM0B for this submodule
         */
        uint32_t chopper_out_invert:1;
        /** chopper_in_invert : R/W; bitpos: [13]; default: 0;
         *  when set, invert the input of PWM0A and PWM0B for this submodule
         */
        uint32_t chopper_in_invert:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} mcpwm_carrier_cfg_reg_t;

/** Type of fh_cfg0 register
 *  Actions on PWM0A and PWM0B trip events
 */
typedef union {
    struct {
        /** tz_sw_cbc : R/W; bitpos: [0]; default: 0;
         *  Enable register for software force cycle-by-cycle mode action. 0: disable, 1: enable
         */
        uint32_t tz_sw_cbc:1;
        /** tz_f2_cbc : R/W; bitpos: [1]; default: 0;
         *  event_f2 will trigger cycle-by-cycle mode action. 0: disable, 1: enable
         */
        uint32_t tz_f2_cbc:1;
        /** tz_f1_cbc : R/W; bitpos: [2]; default: 0;
         *  event_f1 will trigger cycle-by-cycle mode action. 0: disable, 1: enable
         */
        uint32_t tz_f1_cbc:1;
        /** tz_f0_cbc : R/W; bitpos: [3]; default: 0;
         *  event_f0 will trigger cycle-by-cycle mode action. 0: disable, 1: enable
         */
        uint32_t tz_f0_cbc:1;
        /** tz_sw_ost : R/W; bitpos: [4]; default: 0;
         *  Enable register for software force one-shot mode action. 0: disable, 1: enable
         */
        uint32_t tz_sw_ost:1;
        /** tz_f2_ost : R/W; bitpos: [5]; default: 0;
         *  event_f2 will trigger one-shot mode action. 0: disable, 1: enable
         */
        uint32_t tz_f2_ost:1;
        /** tz_f1_ost : R/W; bitpos: [6]; default: 0;
         *  event_f1 will trigger one-shot mode action. 0: disable, 1: enable
         */
        uint32_t tz_f1_ost:1;
        /** tz_f0_ost : R/W; bitpos: [7]; default: 0;
         *  event_f0 will trigger one-shot mode action. 0: disable, 1: enable
         */
        uint32_t tz_f0_ost:1;
        /** tz_a_cbc_d : R/W; bitpos: [9:8]; default: 0;
         *  Cycle-by-cycle mode action on PWM0A when fault event occurs and timer is
         *  decreasing. 0: do nothing, 1: force low, 2: force high, 3: toggle
         */
        uint32_t tz_a_cbc_d:2;
        /** tz_a_cbc_u : R/W; bitpos: [11:10]; default: 0;
         *  Cycle-by-cycle mode action on PWM0A when fault event occurs and timer is
         *  increasing. 0: do nothing, 1: force low, 2: force high, 3: toggle
         */
        uint32_t tz_a_cbc_u:2;
        /** tz_a_ost_d : R/W; bitpos: [13:12]; default: 0;
         *  One-shot mode action on PWM0A when fault event occurs and timer is decreasing. 0:
         *  do nothing, 1: force low, 2: force high, 3: toggle
         */
        uint32_t tz_a_ost_d:2;
        /** tz_a_ost_u : R/W; bitpos: [15:14]; default: 0;
         *  One-shot mode action on PWM0A when fault event occurs and timer is increasing. 0:
         *  do nothing, 1: force low, 2: force high, 3: toggle
         */
        uint32_t tz_a_ost_u:2;
        /** tz_b_cbc_d : R/W; bitpos: [17:16]; default: 0;
         *  Cycle-by-cycle mode action on PWM0B when fault event occurs and timer is
         *  decreasing. 0: do nothing, 1: force low, 2: force high, 3: toggle
         */
        uint32_t tz_b_cbc_d:2;
        /** tz_b_cbc_u : R/W; bitpos: [19:18]; default: 0;
         *  Cycle-by-cycle mode action on PWM0B when fault event occurs and timer is
         *  increasing. 0: do nothing,1: force low, 2: force high, 3: toggle
         */
        uint32_t tz_b_cbc_u:2;
        /** tz_b_ost_d : R/W; bitpos: [21:20]; default: 0;
         *  One-shot mode action on PWM0B when fault event occurs and timer is decreasing. 0:
         *  do nothing, 1: force low, 2: force high, 3: toggle
         */
        uint32_t tz_b_ost_d:2;
        /** tz_b_ost_u : R/W; bitpos: [23:22]; default: 0;
         *  One-shot mode action on PWM0B when fault event occurs and timer is increasing. 0:
         *  do nothing, 1: force low, 2: force high, 3: toggle
         */
        uint32_t tz_b_ost_u:2;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} mcpwm_fh_cfg0_reg_t;

/** Type of fh_cfg1 register
 *  Software triggers for fault handler actions
 */
typedef union {
    struct {
        /** tz_clr_ost : R/W; bitpos: [0]; default: 0;
         *  a rising edge will clear on going one-shot mode action
         */
        uint32_t tz_clr_ost:1;
        /** tz_cbcpulse : R/W; bitpos: [2:1]; default: 0;
         *  cycle-by-cycle mode action refresh moment selection. When bit0 is set to 1: TEZ,
         *  when bit1 is set to 1:TEP
         */
        uint32_t tz_cbcpulse:2;
        /** tz_force_cbc : R/W; bitpos: [3]; default: 0;
         *  a toggle trigger a cycle-by-cycle mode action
         */
        uint32_t tz_force_cbc:1;
        /** tz_force_ost : R/W; bitpos: [4]; default: 0;
         *  a toggle (software negate its value) triggers a one-shot mode action
         */
        uint32_t tz_force_ost:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} mcpwm_fh_cfg1_reg_t;

/** Type of fh_status register
 *  Status of fault events.
 */
typedef union {
    struct {
        /** tz_cbc_on : RO; bitpos: [0]; default: 0;
         *  Set and reset by hardware. If set, a cycle-by-cycle mode action is on going
         */
        uint32_t tz_cbc_on:1;
        /** tz_ost_on : RO; bitpos: [1]; default: 0;
         *  Set and reset by hardware. If set, an one-shot mode action is on going
         */
        uint32_t tz_ost_on:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} mcpwm_fh_status_reg_t;


/** Group: Fault Detection Configuration and Status */
/** Type of fault_detect register
 *  Fault detection configuration and status
 */
typedef union {
    struct {
        /** f0_en : R/W; bitpos: [0]; default: 0;
         *  When set, event_f0 generation is enabled
         */
        uint32_t f0_en:1;
        /** f1_en : R/W; bitpos: [1]; default: 0;
         *  When set, event_f1 generation is enabled
         */
        uint32_t f1_en:1;
        /** f2_en : R/W; bitpos: [2]; default: 0;
         *  When set, event_f2 generation is enabled
         */
        uint32_t f2_en:1;
        /** f0_pole : R/W; bitpos: [3]; default: 0;
         *  Set event_f0 trigger polarity on FAULT2 source from GPIO matrix. 0: level low, 1:
         *  level high
         */
        uint32_t f0_pole:1;
        /** f1_pole : R/W; bitpos: [4]; default: 0;
         *  Set event_f1 trigger polarity on FAULT2 source from GPIO matrix. 0: level low, 1:
         *  level high
         */
        uint32_t f1_pole:1;
        /** f2_pole : R/W; bitpos: [5]; default: 0;
         *  Set event_f2 trigger polarity on FAULT2 source from GPIO matrix. 0: level low, 1:
         *  level high
         */
        uint32_t f2_pole:1;
        /** event_f0 : RO; bitpos: [6]; default: 0;
         *  Set and reset by hardware. If set, event_f0 is on going
         */
        uint32_t event_f0:1;
        /** event_f1 : RO; bitpos: [7]; default: 0;
         *  Set and reset by hardware. If set, event_f1 is on going
         */
        uint32_t event_f1:1;
        /** event_f2 : RO; bitpos: [8]; default: 0;
         *  Set and reset by hardware. If set, event_f2 is on going
         */
        uint32_t event_f2:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} mcpwm_fault_detect_reg_t;


/** Group: Capture Configuration and Status */
/** Type of cap_timer_cfg register
 *  Configure capture timer
 */
typedef union {
    struct {
        /** cap_timer_en : R/W; bitpos: [0]; default: 0;
         *  When set, capture timer incrementing under APB_clk is enabled.
         */
        uint32_t cap_timer_en:1;
        /** cap_synci_en : R/W; bitpos: [1]; default: 0;
         *  When set, capture timer sync is enabled.
         */
        uint32_t cap_synci_en:1;
        /** cap_synci_sel : R/W; bitpos: [4:2]; default: 0;
         *  capture module sync input selection. 0: none, 1: timer0 sync_out, 2: timer1
         *  sync_out, 3: timer2 sync_out, 4: SYNC0 from GPIO matrix, 5: SYNC1 from GPIO matrix,
         *  6: SYNC2 from GPIO matrix
         */
        uint32_t cap_synci_sel:3;
        /** cap_sync_sw : WT; bitpos: [5]; default: 0;
         *  When reg_cap_synci_en is 1,  write 1 will trigger a capture timer sync, capture
         *  timer is loaded with value in phase register.
         */
        uint32_t cap_sync_sw:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} mcpwm_cap_timer_cfg_reg_t;

/** Type of cap_timer_phase register
 *  Phase for capture timer sync
 */
typedef union {
    struct {
        /** cap_phase : R/W; bitpos: [31:0]; default: 0;
         *  Phase value for capture timer sync operation.
         */
        uint32_t cap_phase:32;
    };
    uint32_t val;
} mcpwm_cap_timer_phase_reg_t;

/** Type of cap_chn_cfg register
 *  Capture channel n configuration and enable
 */
typedef union {
    struct {
        /** capn_en : R/W; bitpos: [0]; default: 0;
         *  When set, capture on channel 0 is enabled
         */
        uint32_t capn_en:1;
        /** capn_mode : R/W; bitpos: [2:1]; default: 0;
         *  Edge of capture on channel n after prescaling. When bit0 is set to 1: enable
         *  capture on the negative edge, When bit1 is set to 1: enable capture on the positive
         *  edge.
         */
        uint32_t capn_mode:2;
        /** capn_prescale : R/W; bitpos: [10:3]; default: 0;
         *  Value of prescaling on possitive edge of CAPn. Prescale value = PWM_CAP0_PRESCALE +
         *  1
         */
        uint32_t capn_prescale:8;
        /** capn_in_invert : R/W; bitpos: [11]; default: 0;
         *  when set, CAPn form GPIO matrix is inverted before prescale
         */
        uint32_t capn_in_invert:1;
        /** capn_sw : WT; bitpos: [12]; default: 0;
         *  Write 1 will trigger a software forced capture on channel n
         */
        uint32_t capn_sw:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} mcpwm_cap_chn_cfg_reg_t;

/** Type of cap_chn register
 *  chn capture value status register
 */
typedef union {
    struct {
        /** capn_value : RO; bitpos: [31:0]; default: 0;
         *  Value of last capture on channel n
         */
        uint32_t capn_value:32;
    };
    uint32_t val;
} mcpwm_cap_chn_reg_t;

/** Type of cap_status register
 *  Edge of last capture trigger
 */
typedef union {
    struct {
        /** cap0_edge : RO; bitpos: [0]; default: 0;
         *  Edge of last capture trigger on channel 0, 0: posedge, 1: negedge
         */
        uint32_t cap0_edge:1;
        /** cap1_edge : RO; bitpos: [1]; default: 0;
         *  Edge of last capture trigger on channel 1, 0: posedge, 1: negedge
         */
        uint32_t cap1_edge:1;
        /** cap2_edge : RO; bitpos: [2]; default: 0;
         *  Edge of last capture trigger on channel 2, 0: posedge, 1: negedge
         */
        uint32_t cap2_edge:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} mcpwm_cap_status_reg_t;


/** Group: Enable update of active registers */
/** Type of update_cfg register
 *  Enable update.
 */
typedef union {
    struct {
        /** global_up_en : R/W; bitpos: [0]; default: 1;
         *  The global enable of update of all active registers in MCPWM module
         */
        uint32_t global_up_en:1;
        /** global_force_up : R/W; bitpos: [1]; default: 0;
         *  a toggle (software invert its value) will trigger a forced update of all active
         *  registers in MCPWM module
         */
        uint32_t global_force_up:1;
        /** op0_up_en : R/W; bitpos: [2]; default: 1;
         *  When set and PWM_GLOBAL_UP_EN is set, update of active registers in PWM operator 0
         *  are enabled
         */
        uint32_t op0_up_en:1;
        /** op0_force_up : R/W; bitpos: [3]; default: 0;
         *  a toggle (software invert its value) will trigger a forced update of active
         *  registers in PWM operator 0
         */
        uint32_t op0_force_up:1;
        /** op1_up_en : R/W; bitpos: [4]; default: 1;
         *  When set and PWM_GLOBAL_UP_EN is set, update of active registers in PWM operator 1
         *  are enabled
         */
        uint32_t op1_up_en:1;
        /** op1_force_up : R/W; bitpos: [5]; default: 0;
         *  a toggle (software invert its value) will trigger a forced update of active
         *  registers in PWM operator 1
         */
        uint32_t op1_force_up:1;
        /** op2_up_en : R/W; bitpos: [6]; default: 1;
         *  When set and PWM_GLOBAL_UP_EN is set, update of active registers in PWM operator 2
         *  are enabled
         */
        uint32_t op2_up_en:1;
        /** op2_force_up : R/W; bitpos: [7]; default: 0;
         *  a toggle (software invert its value) will trigger a forced update of active
         *  registers in PWM operator 2
         */
        uint32_t op2_force_up:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} mcpwm_update_cfg_reg_t;


/** Group: Manage Interrupts */
/** Type of int_ena register
 *  Interrupt enable bits
 */
typedef union {
    struct {
        /** timer0_stop_int_ena : R/W; bitpos: [0]; default: 0;
         *  The enable bit for the interrupt triggered when the timer 0 stops.
         */
        uint32_t timer0_stop_int_ena:1;
        /** timer1_stop_int_ena : R/W; bitpos: [1]; default: 0;
         *  The enable bit for the interrupt triggered when the timer 1 stops.
         */
        uint32_t timer1_stop_int_ena:1;
        /** timer2_stop_int_ena : R/W; bitpos: [2]; default: 0;
         *  The enable bit for the interrupt triggered when the timer 2 stops.
         */
        uint32_t timer2_stop_int_ena:1;
        /** timer0_tez_int_ena : R/W; bitpos: [3]; default: 0;
         *  The enable bit for the interrupt triggered by a PWM timer 0 TEZ event.
         */
        uint32_t timer0_tez_int_ena:1;
        /** timer1_tez_int_ena : R/W; bitpos: [4]; default: 0;
         *  The enable bit for the interrupt triggered by a PWM timer 1 TEZ event.
         */
        uint32_t timer1_tez_int_ena:1;
        /** timer2_tez_int_ena : R/W; bitpos: [5]; default: 0;
         *  The enable bit for the interrupt triggered by a PWM timer 2 TEZ event.
         */
        uint32_t timer2_tez_int_ena:1;
        /** timer0_tep_int_ena : R/W; bitpos: [6]; default: 0;
         *  The enable bit for the interrupt triggered by a PWM timer 0 TEP event.
         */
        uint32_t timer0_tep_int_ena:1;
        /** timer1_tep_int_ena : R/W; bitpos: [7]; default: 0;
         *  The enable bit for the interrupt triggered by a PWM timer 1 TEP event.
         */
        uint32_t timer1_tep_int_ena:1;
        /** timer2_tep_int_ena : R/W; bitpos: [8]; default: 0;
         *  The enable bit for the interrupt triggered by a PWM timer 2 TEP event.
         */
        uint32_t timer2_tep_int_ena:1;
        /** fault0_int_ena : R/W; bitpos: [9]; default: 0;
         *  The enable bit for the interrupt triggered when event_f0 starts.
         */
        uint32_t fault0_int_ena:1;
        /** fault1_int_ena : R/W; bitpos: [10]; default: 0;
         *  The enable bit for the interrupt triggered when event_f1 starts.
         */
        uint32_t fault1_int_ena:1;
        /** fault2_int_ena : R/W; bitpos: [11]; default: 0;
         *  The enable bit for the interrupt triggered when event_f2 starts.
         */
        uint32_t fault2_int_ena:1;
        /** fault0_clr_int_ena : R/W; bitpos: [12]; default: 0;
         *  The enable bit for the interrupt triggered when event_f0 ends.
         */
        uint32_t fault0_clr_int_ena:1;
        /** fault1_clr_int_ena : R/W; bitpos: [13]; default: 0;
         *  The enable bit for the interrupt triggered when event_f1 ends.
         */
        uint32_t fault1_clr_int_ena:1;
        /** fault2_clr_int_ena : R/W; bitpos: [14]; default: 0;
         *  The enable bit for the interrupt triggered when event_f2 ends.
         */
        uint32_t fault2_clr_int_ena:1;
        /** cmpr0_tea_int_ena : R/W; bitpos: [15]; default: 0;
         *  The enable bit for the interrupt triggered by a PWM operator 0 TEA event
         */
        uint32_t cmpr0_tea_int_ena:1;
        /** cmpr1_tea_int_ena : R/W; bitpos: [16]; default: 0;
         *  The enable bit for the interrupt triggered by a PWM operator 1 TEA event
         */
        uint32_t cmpr1_tea_int_ena:1;
        /** cmpr2_tea_int_ena : R/W; bitpos: [17]; default: 0;
         *  The enable bit for the interrupt triggered by a PWM operator 2 TEA event
         */
        uint32_t cmpr2_tea_int_ena:1;
        /** cmpr0_teb_int_ena : R/W; bitpos: [18]; default: 0;
         *  The enable bit for the interrupt triggered by a PWM operator 0 TEB event
         */
        uint32_t cmpr0_teb_int_ena:1;
        /** cmpr1_teb_int_ena : R/W; bitpos: [19]; default: 0;
         *  The enable bit for the interrupt triggered by a PWM operator 1 TEB event
         */
        uint32_t cmpr1_teb_int_ena:1;
        /** cmpr2_teb_int_ena : R/W; bitpos: [20]; default: 0;
         *  The enable bit for the interrupt triggered by a PWM operator 2 TEB event
         */
        uint32_t cmpr2_teb_int_ena:1;
        /** tz0_cbc_int_ena : R/W; bitpos: [21]; default: 0;
         *  The enable bit for the interrupt triggered by a cycle-by-cycle mode action on PWM0.
         */
        uint32_t tz0_cbc_int_ena:1;
        /** tz1_cbc_int_ena : R/W; bitpos: [22]; default: 0;
         *  The enable bit for the interrupt triggered by a cycle-by-cycle mode action on PWM1.
         */
        uint32_t tz1_cbc_int_ena:1;
        /** tz2_cbc_int_ena : R/W; bitpos: [23]; default: 0;
         *  The enable bit for the interrupt triggered by a cycle-by-cycle mode action on PWM2.
         */
        uint32_t tz2_cbc_int_ena:1;
        /** tz0_ost_int_ena : R/W; bitpos: [24]; default: 0;
         *  The enable bit for the interrupt triggered by a one-shot mode action on PWM0.
         */
        uint32_t tz0_ost_int_ena:1;
        /** tz1_ost_int_ena : R/W; bitpos: [25]; default: 0;
         *  The enable bit for the interrupt triggered by a one-shot mode action on PWM1.
         */
        uint32_t tz1_ost_int_ena:1;
        /** tz2_ost_int_ena : R/W; bitpos: [26]; default: 0;
         *  The enable bit for the interrupt triggered by a one-shot mode action on PWM2.
         */
        uint32_t tz2_ost_int_ena:1;
        /** cap0_int_ena : R/W; bitpos: [27]; default: 0;
         *  The enable bit for the interrupt triggered by capture on channel 0.
         */
        uint32_t cap0_int_ena:1;
        /** cap1_int_ena : R/W; bitpos: [28]; default: 0;
         *  The enable bit for the interrupt triggered by capture on channel 1.
         */
        uint32_t cap1_int_ena:1;
        /** cap2_int_ena : R/W; bitpos: [29]; default: 0;
         *  The enable bit for the interrupt triggered by capture on channel 2.
         */
        uint32_t cap2_int_ena:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} mcpwm_int_ena_reg_t;

/** Type of int_raw register
 *  Raw interrupt status
 */
typedef union {
    struct {
        /** timer0_stop_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw status bit for the interrupt triggered when the timer 0 stops.
         */
        uint32_t timer0_stop_int_raw:1;
        /** timer1_stop_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw status bit for the interrupt triggered when the timer 1 stops.
         */
        uint32_t timer1_stop_int_raw:1;
        /** timer2_stop_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw status bit for the interrupt triggered when the timer 2 stops.
         */
        uint32_t timer2_stop_int_raw:1;
        /** timer0_tez_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw status bit for the interrupt triggered by a PWM timer 0 TEZ event.
         */
        uint32_t timer0_tez_int_raw:1;
        /** timer1_tez_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw status bit for the interrupt triggered by a PWM timer 1 TEZ event.
         */
        uint32_t timer1_tez_int_raw:1;
        /** timer2_tez_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw status bit for the interrupt triggered by a PWM timer 2 TEZ event.
         */
        uint32_t timer2_tez_int_raw:1;
        /** timer0_tep_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The raw status bit for the interrupt triggered by a PWM timer 0 TEP event.
         */
        uint32_t timer0_tep_int_raw:1;
        /** timer1_tep_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  The raw status bit for the interrupt triggered by a PWM timer 1 TEP event.
         */
        uint32_t timer1_tep_int_raw:1;
        /** timer2_tep_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  The raw status bit for the interrupt triggered by a PWM timer 2 TEP event.
         */
        uint32_t timer2_tep_int_raw:1;
        /** fault0_int_raw : R/WTC/SS; bitpos: [9]; default: 0;
         *  The raw status bit for the interrupt triggered when event_f0 starts.
         */
        uint32_t fault0_int_raw:1;
        /** fault1_int_raw : R/WTC/SS; bitpos: [10]; default: 0;
         *  The raw status bit for the interrupt triggered when event_f1 starts.
         */
        uint32_t fault1_int_raw:1;
        /** fault2_int_raw : R/WTC/SS; bitpos: [11]; default: 0;
         *  The raw status bit for the interrupt triggered when event_f2 starts.
         */
        uint32_t fault2_int_raw:1;
        /** fault0_clr_int_raw : R/WTC/SS; bitpos: [12]; default: 0;
         *  The raw status bit for the interrupt triggered when event_f0 ends.
         */
        uint32_t fault0_clr_int_raw:1;
        /** fault1_clr_int_raw : R/WTC/SS; bitpos: [13]; default: 0;
         *  The raw status bit for the interrupt triggered when event_f1 ends.
         */
        uint32_t fault1_clr_int_raw:1;
        /** fault2_clr_int_raw : R/WTC/SS; bitpos: [14]; default: 0;
         *  The raw status bit for the interrupt triggered when event_f2 ends.
         */
        uint32_t fault2_clr_int_raw:1;
        /** cmpr0_tea_int_raw : R/WTC/SS; bitpos: [15]; default: 0;
         *  The raw status bit for the interrupt triggered by a PWM operator 0 TEA event
         */
        uint32_t cmpr0_tea_int_raw:1;
        /** cmpr1_tea_int_raw : R/WTC/SS; bitpos: [16]; default: 0;
         *  The raw status bit for the interrupt triggered by a PWM operator 1 TEA event
         */
        uint32_t cmpr1_tea_int_raw:1;
        /** cmpr2_tea_int_raw : R/WTC/SS; bitpos: [17]; default: 0;
         *  The raw status bit for the interrupt triggered by a PWM operator 2 TEA event
         */
        uint32_t cmpr2_tea_int_raw:1;
        /** cmpr0_teb_int_raw : R/WTC/SS; bitpos: [18]; default: 0;
         *  The raw status bit for the interrupt triggered by a PWM operator 0 TEB event
         */
        uint32_t cmpr0_teb_int_raw:1;
        /** cmpr1_teb_int_raw : R/WTC/SS; bitpos: [19]; default: 0;
         *  The raw status bit for the interrupt triggered by a PWM operator 1 TEB event
         */
        uint32_t cmpr1_teb_int_raw:1;
        /** cmpr2_teb_int_raw : R/WTC/SS; bitpos: [20]; default: 0;
         *  The raw status bit for the interrupt triggered by a PWM operator 2 TEB event
         */
        uint32_t cmpr2_teb_int_raw:1;
        /** tz0_cbc_int_raw : R/WTC/SS; bitpos: [21]; default: 0;
         *  The raw status bit for the interrupt triggered by a cycle-by-cycle mode action on
         *  PWM0.
         */
        uint32_t tz0_cbc_int_raw:1;
        /** tz1_cbc_int_raw : R/WTC/SS; bitpos: [22]; default: 0;
         *  The raw status bit for the interrupt triggered by a cycle-by-cycle mode action on
         *  PWM1.
         */
        uint32_t tz1_cbc_int_raw:1;
        /** tz2_cbc_int_raw : R/WTC/SS; bitpos: [23]; default: 0;
         *  The raw status bit for the interrupt triggered by a cycle-by-cycle mode action on
         *  PWM2.
         */
        uint32_t tz2_cbc_int_raw:1;
        /** tz0_ost_int_raw : R/WTC/SS; bitpos: [24]; default: 0;
         *  The raw status bit for the interrupt triggered by a one-shot mode action on PWM0.
         */
        uint32_t tz0_ost_int_raw:1;
        /** tz1_ost_int_raw : R/WTC/SS; bitpos: [25]; default: 0;
         *  The raw status bit for the interrupt triggered by a one-shot mode action on PWM1.
         */
        uint32_t tz1_ost_int_raw:1;
        /** tz2_ost_int_raw : R/WTC/SS; bitpos: [26]; default: 0;
         *  The raw status bit for the interrupt triggered by a one-shot mode action on PWM2.
         */
        uint32_t tz2_ost_int_raw:1;
        /** cap0_int_raw : R/WTC/SS; bitpos: [27]; default: 0;
         *  The raw status bit for the interrupt triggered by capture on channel 0.
         */
        uint32_t cap0_int_raw:1;
        /** cap1_int_raw : R/WTC/SS; bitpos: [28]; default: 0;
         *  The raw status bit for the interrupt triggered by capture on channel 1.
         */
        uint32_t cap1_int_raw:1;
        /** cap2_int_raw : R/WTC/SS; bitpos: [29]; default: 0;
         *  The raw status bit for the interrupt triggered by capture on channel 2.
         */
        uint32_t cap2_int_raw:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} mcpwm_int_raw_reg_t;

/** Type of int_st register
 *  Masked interrupt status
 */
typedef union {
    struct {
        /** timer0_stop_int_st : RO; bitpos: [0]; default: 0;
         *  The masked status bit for the interrupt triggered when the timer 0 stops.
         */
        uint32_t timer0_stop_int_st:1;
        /** timer1_stop_int_st : RO; bitpos: [1]; default: 0;
         *  The masked status bit for the interrupt triggered when the timer 1 stops.
         */
        uint32_t timer1_stop_int_st:1;
        /** timer2_stop_int_st : RO; bitpos: [2]; default: 0;
         *  The masked status bit for the interrupt triggered when the timer 2 stops.
         */
        uint32_t timer2_stop_int_st:1;
        /** timer0_tez_int_st : RO; bitpos: [3]; default: 0;
         *  The masked status bit for the interrupt triggered by a PWM timer 0 TEZ event.
         */
        uint32_t timer0_tez_int_st:1;
        /** timer1_tez_int_st : RO; bitpos: [4]; default: 0;
         *  The masked status bit for the interrupt triggered by a PWM timer 1 TEZ event.
         */
        uint32_t timer1_tez_int_st:1;
        /** timer2_tez_int_st : RO; bitpos: [5]; default: 0;
         *  The masked status bit for the interrupt triggered by a PWM timer 2 TEZ event.
         */
        uint32_t timer2_tez_int_st:1;
        /** timer0_tep_int_st : RO; bitpos: [6]; default: 0;
         *  The masked status bit for the interrupt triggered by a PWM timer 0 TEP event.
         */
        uint32_t timer0_tep_int_st:1;
        /** timer1_tep_int_st : RO; bitpos: [7]; default: 0;
         *  The masked status bit for the interrupt triggered by a PWM timer 1 TEP event.
         */
        uint32_t timer1_tep_int_st:1;
        /** timer2_tep_int_st : RO; bitpos: [8]; default: 0;
         *  The masked status bit for the interrupt triggered by a PWM timer 2 TEP event.
         */
        uint32_t timer2_tep_int_st:1;
        /** fault0_int_st : RO; bitpos: [9]; default: 0;
         *  The masked status bit for the interrupt triggered when event_f0 starts.
         */
        uint32_t fault0_int_st:1;
        /** fault1_int_st : RO; bitpos: [10]; default: 0;
         *  The masked status bit for the interrupt triggered when event_f1 starts.
         */
        uint32_t fault1_int_st:1;
        /** fault2_int_st : RO; bitpos: [11]; default: 0;
         *  The masked status bit for the interrupt triggered when event_f2 starts.
         */
        uint32_t fault2_int_st:1;
        /** fault0_clr_int_st : RO; bitpos: [12]; default: 0;
         *  The masked status bit for the interrupt triggered when event_f0 ends.
         */
        uint32_t fault0_clr_int_st:1;
        /** fault1_clr_int_st : RO; bitpos: [13]; default: 0;
         *  The masked status bit for the interrupt triggered when event_f1 ends.
         */
        uint32_t fault1_clr_int_st:1;
        /** fault2_clr_int_st : RO; bitpos: [14]; default: 0;
         *  The masked status bit for the interrupt triggered when event_f2 ends.
         */
        uint32_t fault2_clr_int_st:1;
        /** cmpr0_tea_int_st : RO; bitpos: [15]; default: 0;
         *  The masked status bit for the interrupt triggered by a PWM operator 0 TEA event
         */
        uint32_t cmpr0_tea_int_st:1;
        /** cmpr1_tea_int_st : RO; bitpos: [16]; default: 0;
         *  The masked status bit for the interrupt triggered by a PWM operator 1 TEA event
         */
        uint32_t cmpr1_tea_int_st:1;
        /** cmpr2_tea_int_st : RO; bitpos: [17]; default: 0;
         *  The masked status bit for the interrupt triggered by a PWM operator 2 TEA event
         */
        uint32_t cmpr2_tea_int_st:1;
        /** cmpr0_teb_int_st : RO; bitpos: [18]; default: 0;
         *  The masked status bit for the interrupt triggered by a PWM operator 0 TEB event
         */
        uint32_t cmpr0_teb_int_st:1;
        /** cmpr1_teb_int_st : RO; bitpos: [19]; default: 0;
         *  The masked status bit for the interrupt triggered by a PWM operator 1 TEB event
         */
        uint32_t cmpr1_teb_int_st:1;
        /** cmpr2_teb_int_st : RO; bitpos: [20]; default: 0;
         *  The masked status bit for the interrupt triggered by a PWM operator 2 TEB event
         */
        uint32_t cmpr2_teb_int_st:1;
        /** tz0_cbc_int_st : RO; bitpos: [21]; default: 0;
         *  The masked status bit for the interrupt triggered by a cycle-by-cycle mode action
         *  on PWM0.
         */
        uint32_t tz0_cbc_int_st:1;
        /** tz1_cbc_int_st : RO; bitpos: [22]; default: 0;
         *  The masked status bit for the interrupt triggered by a cycle-by-cycle mode action
         *  on PWM1.
         */
        uint32_t tz1_cbc_int_st:1;
        /** tz2_cbc_int_st : RO; bitpos: [23]; default: 0;
         *  The masked status bit for the interrupt triggered by a cycle-by-cycle mode action
         *  on PWM2.
         */
        uint32_t tz2_cbc_int_st:1;
        /** tz0_ost_int_st : RO; bitpos: [24]; default: 0;
         *  The masked status bit for the interrupt triggered by a one-shot mode action on PWM0.
         */
        uint32_t tz0_ost_int_st:1;
        /** tz1_ost_int_st : RO; bitpos: [25]; default: 0;
         *  The masked status bit for the interrupt triggered by a one-shot mode action on PWM1.
         */
        uint32_t tz1_ost_int_st:1;
        /** tz2_ost_int_st : RO; bitpos: [26]; default: 0;
         *  The masked status bit for the interrupt triggered by a one-shot mode action on PWM2.
         */
        uint32_t tz2_ost_int_st:1;
        /** cap0_int_st : RO; bitpos: [27]; default: 0;
         *  The masked status bit for the interrupt triggered by capture on channel 0.
         */
        uint32_t cap0_int_st:1;
        /** cap1_int_st : RO; bitpos: [28]; default: 0;
         *  The masked status bit for the interrupt triggered by capture on channel 1.
         */
        uint32_t cap1_int_st:1;
        /** cap2_int_st : RO; bitpos: [29]; default: 0;
         *  The masked status bit for the interrupt triggered by capture on channel 2.
         */
        uint32_t cap2_int_st:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} mcpwm_int_st_reg_t;

/** Type of int_clr register
 *  Interrupt clear bits
 */
typedef union {
    struct {
        /** timer0_stop_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the interrupt triggered when the timer 0 stops.
         */
        uint32_t timer0_stop_int_clr:1;
        /** timer1_stop_int_clr : WT; bitpos: [1]; default: 0;
         *  Set this bit to clear the interrupt triggered when the timer 1 stops.
         */
        uint32_t timer1_stop_int_clr:1;
        /** timer2_stop_int_clr : WT; bitpos: [2]; default: 0;
         *  Set this bit to clear the interrupt triggered when the timer 2 stops.
         */
        uint32_t timer2_stop_int_clr:1;
        /** timer0_tez_int_clr : WT; bitpos: [3]; default: 0;
         *  Set this bit to clear the interrupt triggered by a PWM timer 0 TEZ event.
         */
        uint32_t timer0_tez_int_clr:1;
        /** timer1_tez_int_clr : WT; bitpos: [4]; default: 0;
         *  Set this bit to clear the interrupt triggered by a PWM timer 1 TEZ event.
         */
        uint32_t timer1_tez_int_clr:1;
        /** timer2_tez_int_clr : WT; bitpos: [5]; default: 0;
         *  Set this bit to clear the interrupt triggered by a PWM timer 2 TEZ event.
         */
        uint32_t timer2_tez_int_clr:1;
        /** timer0_tep_int_clr : WT; bitpos: [6]; default: 0;
         *  Set this bit to clear the interrupt triggered by a PWM timer 0 TEP event.
         */
        uint32_t timer0_tep_int_clr:1;
        /** timer1_tep_int_clr : WT; bitpos: [7]; default: 0;
         *  Set this bit to clear the interrupt triggered by a PWM timer 1 TEP event.
         */
        uint32_t timer1_tep_int_clr:1;
        /** timer2_tep_int_clr : WT; bitpos: [8]; default: 0;
         *  Set this bit to clear the interrupt triggered by a PWM timer 2 TEP event.
         */
        uint32_t timer2_tep_int_clr:1;
        /** fault0_int_clr : WT; bitpos: [9]; default: 0;
         *  Set this bit to clear the interrupt triggered when event_f0 starts.
         */
        uint32_t fault0_int_clr:1;
        /** fault1_int_clr : WT; bitpos: [10]; default: 0;
         *  Set this bit to clear the interrupt triggered when event_f1 starts.
         */
        uint32_t fault1_int_clr:1;
        /** fault2_int_clr : WT; bitpos: [11]; default: 0;
         *  Set this bit to clear the interrupt triggered when event_f2 starts.
         */
        uint32_t fault2_int_clr:1;
        /** fault0_clr_int_clr : WT; bitpos: [12]; default: 0;
         *  Set this bit to clear the interrupt triggered when event_f0 ends.
         */
        uint32_t fault0_clr_int_clr:1;
        /** fault1_clr_int_clr : WT; bitpos: [13]; default: 0;
         *  Set this bit to clear the interrupt triggered when event_f1 ends.
         */
        uint32_t fault1_clr_int_clr:1;
        /** fault2_clr_int_clr : WT; bitpos: [14]; default: 0;
         *  Set this bit to clear the interrupt triggered when event_f2 ends.
         */
        uint32_t fault2_clr_int_clr:1;
        /** cmpr0_tea_int_clr : WT; bitpos: [15]; default: 0;
         *  Set this bit to clear the interrupt triggered by a PWM operator 0 TEA event
         */
        uint32_t cmpr0_tea_int_clr:1;
        /** cmpr1_tea_int_clr : WT; bitpos: [16]; default: 0;
         *  Set this bit to clear the interrupt triggered by a PWM operator 1 TEA event
         */
        uint32_t cmpr1_tea_int_clr:1;
        /** cmpr2_tea_int_clr : WT; bitpos: [17]; default: 0;
         *  Set this bit to clear the interrupt triggered by a PWM operator 2 TEA event
         */
        uint32_t cmpr2_tea_int_clr:1;
        /** cmpr0_teb_int_clr : WT; bitpos: [18]; default: 0;
         *  Set this bit to clear the interrupt triggered by a PWM operator 0 TEB event
         */
        uint32_t cmpr0_teb_int_clr:1;
        /** cmpr1_teb_int_clr : WT; bitpos: [19]; default: 0;
         *  Set this bit to clear the interrupt triggered by a PWM operator 1 TEB event
         */
        uint32_t cmpr1_teb_int_clr:1;
        /** cmpr2_teb_int_clr : WT; bitpos: [20]; default: 0;
         *  Set this bit to clear the interrupt triggered by a PWM operator 2 TEB event
         */
        uint32_t cmpr2_teb_int_clr:1;
        /** tz0_cbc_int_clr : WT; bitpos: [21]; default: 0;
         *  Set this bit to clear the interrupt triggered by a cycle-by-cycle mode action on
         *  PWM0.
         */
        uint32_t tz0_cbc_int_clr:1;
        /** tz1_cbc_int_clr : WT; bitpos: [22]; default: 0;
         *  Set this bit to clear the interrupt triggered by a cycle-by-cycle mode action on
         *  PWM1.
         */
        uint32_t tz1_cbc_int_clr:1;
        /** tz2_cbc_int_clr : WT; bitpos: [23]; default: 0;
         *  Set this bit to clear the interrupt triggered by a cycle-by-cycle mode action on
         *  PWM2.
         */
        uint32_t tz2_cbc_int_clr:1;
        /** tz0_ost_int_clr : WT; bitpos: [24]; default: 0;
         *  Set this bit to clear the interrupt triggered by a one-shot mode action on PWM0.
         */
        uint32_t tz0_ost_int_clr:1;
        /** tz1_ost_int_clr : WT; bitpos: [25]; default: 0;
         *  Set this bit to clear the interrupt triggered by a one-shot mode action on PWM1.
         */
        uint32_t tz1_ost_int_clr:1;
        /** tz2_ost_int_clr : WT; bitpos: [26]; default: 0;
         *  Set this bit to clear the interrupt triggered by a one-shot mode action on PWM2.
         */
        uint32_t tz2_ost_int_clr:1;
        /** cap0_int_clr : WT; bitpos: [27]; default: 0;
         *  Set this bit to clear the interrupt triggered by capture on channel 0.
         */
        uint32_t cap0_int_clr:1;
        /** cap1_int_clr : WT; bitpos: [28]; default: 0;
         *  Set this bit to clear the interrupt triggered by capture on channel 1.
         */
        uint32_t cap1_int_clr:1;
        /** cap2_int_clr : WT; bitpos: [29]; default: 0;
         *  Set this bit to clear the interrupt triggered by capture on channel 2.
         */
        uint32_t cap2_int_clr:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} mcpwm_int_clr_reg_t;


/** Group: MCPWM event enable register */
/** Type of evt_en register
 *  MCPWM event enable register
 */
typedef union {
    struct {
        /** evt_timer0_stop_en : R/W; bitpos: [0]; default: 0;
         *  set this bit high to enable timer0 stop event generate
         */
        uint32_t evt_timer0_stop_en:1;
        /** evt_timer1_stop_en : R/W; bitpos: [1]; default: 0;
         *  set this bit high to enable timer1 stop event generate
         */
        uint32_t evt_timer1_stop_en:1;
        /** evt_timer2_stop_en : R/W; bitpos: [2]; default: 0;
         *  set this bit high to enable timer2 stop event generate
         */
        uint32_t evt_timer2_stop_en:1;
        /** evt_timer0_tez_en : R/W; bitpos: [3]; default: 0;
         *  set this bit high to enable timer0 equal zero event generate
         */
        uint32_t evt_timer0_tez_en:1;
        /** evt_timer1_tez_en : R/W; bitpos: [4]; default: 0;
         *  set this bit high to enable timer1 equal zero event generate
         */
        uint32_t evt_timer1_tez_en:1;
        /** evt_timer2_tez_en : R/W; bitpos: [5]; default: 0;
         *  set this bit high to enable timer2 equal zero event generate
         */
        uint32_t evt_timer2_tez_en:1;
        /** evt_timer0_tep_en : R/W; bitpos: [6]; default: 0;
         *  set this bit high to enable timer0 equal period event generate
         */
        uint32_t evt_timer0_tep_en:1;
        /** evt_timer1_tep_en : R/W; bitpos: [7]; default: 0;
         *  set this bit high to enable timer1 equal period event generate
         */
        uint32_t evt_timer1_tep_en:1;
        /** evt_timer2_tep_en : R/W; bitpos: [8]; default: 0;
         *  set this bit high to enable timer2 equal period event generate
         */
        uint32_t evt_timer2_tep_en:1;
        /** evt_op0_tea_en : R/W; bitpos: [9]; default: 0;
         *  set this bit high to enable PWM generator0 timer equal a event generate
         */
        uint32_t evt_op0_tea_en:1;
        /** evt_op1_tea_en : R/W; bitpos: [10]; default: 0;
         *  set this bit high to enable PWM generator1 timer equal a event generate
         */
        uint32_t evt_op1_tea_en:1;
        /** evt_op2_tea_en : R/W; bitpos: [11]; default: 0;
         *  set this bit high to enable PWM generator2 timer equal a event generate
         */
        uint32_t evt_op2_tea_en:1;
        /** evt_op0_teb_en : R/W; bitpos: [12]; default: 0;
         *  set this bit high to enable PWM generator0 timer equal b event generate
         */
        uint32_t evt_op0_teb_en:1;
        /** evt_op1_teb_en : R/W; bitpos: [13]; default: 0;
         *  set this bit high to enable PWM generator1 timer equal b event generate
         */
        uint32_t evt_op1_teb_en:1;
        /** evt_op2_teb_en : R/W; bitpos: [14]; default: 0;
         *  set this bit high to enable PWM generator2 timer equal b event generate
         */
        uint32_t evt_op2_teb_en:1;
        /** evt_f0_en : R/W; bitpos: [15]; default: 0;
         *  set this bit high to enable fault0 event generate
         */
        uint32_t evt_f0_en:1;
        /** evt_f1_en : R/W; bitpos: [16]; default: 0;
         *  set this bit high to enable fault1 event generate
         */
        uint32_t evt_f1_en:1;
        /** evt_f2_en : R/W; bitpos: [17]; default: 0;
         *  set this bit high to enable fault2 event generate
         */
        uint32_t evt_f2_en:1;
        /** evt_f0_clr_en : R/W; bitpos: [18]; default: 0;
         *  set this bit high to enable fault0 clear event generate
         */
        uint32_t evt_f0_clr_en:1;
        /** evt_f1_clr_en : R/W; bitpos: [19]; default: 0;
         *  set this bit high to enable fault1 clear event generate
         */
        uint32_t evt_f1_clr_en:1;
        /** evt_f2_clr_en : R/W; bitpos: [20]; default: 0;
         *  set this bit high to enable fault2 clear event generate
         */
        uint32_t evt_f2_clr_en:1;
        /** evt_tz0_cbc_en : R/W; bitpos: [21]; default: 0;
         *  set this bit high to enable cycle by cycle trip0 event generate
         */
        uint32_t evt_tz0_cbc_en:1;
        /** evt_tz1_cbc_en : R/W; bitpos: [22]; default: 0;
         *  set this bit high to enable cycle by cycle trip1 event generate
         */
        uint32_t evt_tz1_cbc_en:1;
        /** evt_tz2_cbc_en : R/W; bitpos: [23]; default: 0;
         *  set this bit high to enable cycle by cycle trip2 event generate
         */
        uint32_t evt_tz2_cbc_en:1;
        /** evt_tz0_ost_en : R/W; bitpos: [24]; default: 0;
         *  set this bit high to enable one shot trip0 event generate
         */
        uint32_t evt_tz0_ost_en:1;
        /** evt_tz1_ost_en : R/W; bitpos: [25]; default: 0;
         *  set this bit high to enable one shot trip1 event generate
         */
        uint32_t evt_tz1_ost_en:1;
        /** evt_tz2_ost_en : R/W; bitpos: [26]; default: 0;
         *  set this bit high to enable one shot trip2 event generate
         */
        uint32_t evt_tz2_ost_en:1;
        /** evt_cap0_en : R/W; bitpos: [27]; default: 0;
         *  set this bit high to enable capture0 event generate
         */
        uint32_t evt_cap0_en:1;
        /** evt_cap1_en : R/W; bitpos: [28]; default: 0;
         *  set this bit high to enable capture1 event generate
         */
        uint32_t evt_cap1_en:1;
        /** evt_cap2_en : R/W; bitpos: [29]; default: 0;
         *  set this bit high to enable capture2 event generate
         */
        uint32_t evt_cap2_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} mcpwm_evt_en_reg_t;


/** Group: MCPWM task enable register */
/** Type of task_en register
 *  MCPWM task enable register
 */
typedef union {
    struct {
        /** task_cmpr0_a_up_en : R/W; bitpos: [0]; default: 0;
         *  set this bit high to enable PWM generator0 timer stamp A's shadow register update
         *  task receive
         */
        uint32_t task_cmpr0_a_up_en:1;
        /** task_cmpr1_a_up_en : R/W; bitpos: [1]; default: 0;
         *  set this bit high to enable PWM generator1 timer stamp A's shadow register update
         *  task receive
         */
        uint32_t task_cmpr1_a_up_en:1;
        /** task_cmpr2_a_up_en : R/W; bitpos: [2]; default: 0;
         *  set this bit high to enable PWM generator2 timer stamp A's shadow register update
         *  task receive
         */
        uint32_t task_cmpr2_a_up_en:1;
        /** task_cmpr0_b_up_en : R/W; bitpos: [3]; default: 0;
         *  set this bit high to enable PWM generator0 timer stamp B's shadow register update
         *  task receive
         */
        uint32_t task_cmpr0_b_up_en:1;
        /** task_cmpr1_b_up_en : R/W; bitpos: [4]; default: 0;
         *  set this bit high to enable PWM generator1 timer stamp B's shadow register update
         *  task receive
         */
        uint32_t task_cmpr1_b_up_en:1;
        /** task_cmpr2_b_up_en : R/W; bitpos: [5]; default: 0;
         *  set this bit high to enable PWM generator2 timer stamp B's shadow register update
         *  task receive
         */
        uint32_t task_cmpr2_b_up_en:1;
        /** task_gen_stop_en : R/W; bitpos: [6]; default: 0;
         *  set this bit high to enable all PWM generate stop task receive
         */
        uint32_t task_gen_stop_en:1;
        /** task_timer0_sync_en : R/W; bitpos: [7]; default: 0;
         *  set this bit high to enable timer0 sync task receive
         */
        uint32_t task_timer0_sync_en:1;
        /** task_timer1_sync_en : R/W; bitpos: [8]; default: 0;
         *  set this bit high to enable timer1 sync task receive
         */
        uint32_t task_timer1_sync_en:1;
        /** task_timer2_sync_en : R/W; bitpos: [9]; default: 0;
         *  set this bit high to enable timer2 sync task receive
         */
        uint32_t task_timer2_sync_en:1;
        /** task_timer0_period_up_en : R/W; bitpos: [10]; default: 0;
         *  set this bit high to enable timer0 period update task receive
         */
        uint32_t task_timer0_period_up_en:1;
        /** task_timer1_period_up_en : R/W; bitpos: [11]; default: 0;
         *  set this bit high to enable timer1 period update task receive
         */
        uint32_t task_timer1_period_up_en:1;
        /** task_timer2_period_up_en : R/W; bitpos: [12]; default: 0;
         *  set this bit high to enable timer2 period update task receive
         */
        uint32_t task_timer2_period_up_en:1;
        /** task_tz0_ost_en : R/W; bitpos: [13]; default: 0;
         *  set this bit high to enable one shot trip0 task receive
         */
        uint32_t task_tz0_ost_en:1;
        /** task_tz1_ost_en : R/W; bitpos: [14]; default: 0;
         *  set this bit high to enable one shot trip1 task receive
         */
        uint32_t task_tz1_ost_en:1;
        /** task_tz2_ost_en : R/W; bitpos: [15]; default: 0;
         *  set this bit high to enable one shot trip2 task receive
         */
        uint32_t task_tz2_ost_en:1;
        /** task_clr0_ost_en : R/W; bitpos: [16]; default: 0;
         *  set this bit high to enable one shot trip0 clear task receive
         */
        uint32_t task_clr0_ost_en:1;
        /** task_clr1_ost_en : R/W; bitpos: [17]; default: 0;
         *  set this bit high to enable one shot trip1 clear task receive
         */
        uint32_t task_clr1_ost_en:1;
        /** task_clr2_ost_en : R/W; bitpos: [18]; default: 0;
         *  set this bit high to enable one shot trip2 clear task receive
         */
        uint32_t task_clr2_ost_en:1;
        /** task_cap0_en : R/W; bitpos: [19]; default: 0;
         *  set this bit high to enable capture0 task receive
         */
        uint32_t task_cap0_en:1;
        /** task_cap1_en : R/W; bitpos: [20]; default: 0;
         *  set this bit high to enable capture1 task receive
         */
        uint32_t task_cap1_en:1;
        /** task_cap2_en : R/W; bitpos: [21]; default: 0;
         *  set this bit high to enable capture2 task receive
         */
        uint32_t task_cap2_en:1;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} mcpwm_task_en_reg_t;


/** Group: MCPWM APB configuration register */
/** Type of clk register
 *  MCPWM APB configuration register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 0;
         *  Force clock on for this register file
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} mcpwm_clk_reg_t;


/** Group: Version Register */
/** Type of version register
 *  Version register.
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 35656256;
         *  Version of this register file
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} mcpwm_version_reg_t;

typedef struct {
    mcpwm_timer_cfg0_reg_t timer_cfg0;
    mcpwm_timer_cfg1_reg_t timer_cfg1;
    mcpwm_timer_sync_reg_t timer_sync;
    mcpwm_timer_status_reg_t timer_status;
} mcpwm_timer_regs_t;

typedef struct {
    mcpwm_gen_stmp_cfg_reg_t gen_stmp_cfg;
    mcpwm_gen_tstmp_reg_t timestamp[2];
    mcpwm_gen_cfg0_reg_t gen_cfg0;
    mcpwm_gen_force_reg_t gen_force;
    mcpwm_gen_reg_t generator[2];
    mcpwm_dt_cfg_reg_t dt_cfg;
    mcpwm_dt_fed_cfg_reg_t dt_fed_cfg;
    mcpwm_dt_red_cfg_reg_t dt_red_cfg;
    mcpwm_carrier_cfg_reg_t carrier_cfg;
    mcpwm_fh_cfg0_reg_t fh_cfg0;
    mcpwm_fh_cfg1_reg_t fh_cfg1;
    mcpwm_fh_status_reg_t fh_status;
} mcpwm_operator_reg_t;

typedef struct mcpwm_dev_t {
    volatile mcpwm_clk_cfg_reg_t clk_cfg;
    volatile mcpwm_timer_regs_t timer[3];
    volatile mcpwm_timer_synci_cfg_reg_t timer_synci_cfg;
    volatile mcpwm_operator_timersel_reg_t operator_timersel;
    volatile mcpwm_operator_reg_t operators[3];
    volatile mcpwm_fault_detect_reg_t fault_detect;
    volatile mcpwm_cap_timer_cfg_reg_t cap_timer_cfg;
    volatile mcpwm_cap_timer_phase_reg_t cap_timer_phase;
    volatile mcpwm_cap_chn_cfg_reg_t cap_chn_cfg[3];
    volatile mcpwm_cap_chn_reg_t cap_chn[3];
    volatile mcpwm_cap_status_reg_t cap_status;
    volatile mcpwm_update_cfg_reg_t update_cfg;
    volatile mcpwm_int_ena_reg_t int_ena;
    volatile mcpwm_int_raw_reg_t int_raw;
    volatile mcpwm_int_st_reg_t int_st;
    volatile mcpwm_int_clr_reg_t int_clr;
    volatile mcpwm_evt_en_reg_t evt_en;
    volatile mcpwm_task_en_reg_t task_en;
    volatile mcpwm_clk_reg_t clk;
    volatile mcpwm_version_reg_t version;
} mcpwm_dev_t;

extern mcpwm_dev_t MCPWM0;

#ifndef __cplusplus
_Static_assert(sizeof(mcpwm_dev_t) == 0x130, "Invalid size of mcpwm_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
