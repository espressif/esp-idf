/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Configuration register */
/** Type of clk_cfg register
 *  PWM clock prescaler register.
 */
typedef union {
    struct {
        /** clk_prescale : R/W; bitpos: [7:0]; default: 0;
         *  Configures the prescaler value of clock, so that the period of PWM_clk = 6.25ns *
         *  (PWM_CLK_PRESCALE + 1).
         */
        uint32_t clk_prescale:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} mcpwm_clk_cfg_reg_t;

/** Type of timern_cfg0 register
 *  PWM timern period and update method configuration register.
 */
typedef union {
    struct {
        /** timern_prescale : R/W; bitpos: [7:0]; default: 0;
         *  Configures the prescaler value of timern, so that the period of PT0_clk = Period of
         *  PWM_clk * (PWM_TIMERn_PRESCALE + 1)
         */
        uint32_t timern_prescale:8;
        /** timern_period : R/W; bitpos: [23:8]; default: 255;
         *  Configures the period shadow of PWM timern
         */
        uint32_t timern_period:16;
        /** timern_period_upmethod : R/W; bitpos: [25:24]; default: 0;
         *  Configures the update method for active register of PWM timern period.\\0:
         *  Immediate\\1: TEZ\\2: Sync\\3: TEZ or sync\\TEZ here and below means timer equal
         *  zero event
         */
        uint32_t timern_period_upmethod:2;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} mcpwm_timern_cfg0_reg_t;

/** Type of timern_cfg1 register
 *  PWM timern working mode and start/stop control register.
 */
typedef union {
    struct {
        /** timern_start : R/W/SC; bitpos: [2:0]; default: 0;
         *  Configures whether or not to start/stop PWM timern.\\0: If PWM timern starts, then
         *  stops at TEZ\\1: If timern starts, then stops at TEP\\2: PWM timern starts and runs
         *  on\\3: Timern starts and stops at the next TEZ\\4: Timer0 starts and stops at the
         *  next TEP.\\TEP here and below means the event that happens when the timer equals to
         *  period
         */
        uint32_t timern_start:3;
        /** timern_mod : R/W; bitpos: [4:3]; default: 0;
         *  Configures the working mode of PWM timern.\\0: Freeze\\1: Increase mode\\2:
         *  Decrease mode\\3: Up-down mode
         */
        uint32_t timern_mod:2;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} mcpwm_timern_cfg1_reg_t;

/** Type of timern_sync register
 *  PWM timern sync function configuration register.
 */
typedef union {
    struct {
        /** timern_synci_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enable timern reloading with phase on sync input event
         *  is enabled.\\0: Disable\\1: Enable
         */
        uint32_t timern_synci_en:1;
        /** timern_sync_sw : R/W; bitpos: [1]; default: 0;
         *  Configures the generation of software sync. Toggling this bit will trigger a
         *  software sync.
         */
        uint32_t timern_sync_sw:1;
        /** timern_synco_sel : R/W; bitpos: [3:2]; default: 0;
         *  Configures the selection of PWM timern sync_out.\\0: Sync_in\\1: TEZ\\2: TEP\\3:
         *  Invalid, sync_out selects noting
         */
        uint32_t timern_synco_sel:2;
        /** timern_phase : R/W; bitpos: [19:4]; default: 0;
         *  Configures the phase for timern reload on sync event.
         */
        uint32_t timern_phase:16;
        /** timern_phase_direction : R/W; bitpos: [20]; default: 0;
         *  Configures the PWM timern's direction when timern mode is up-down mode.\\0:
         *  Increase\\1: Decrease
         */
        uint32_t timern_phase_direction:1;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} mcpwm_timern_sync_reg_t;

/** Type of timern_status register
 *  PWM timern status register.
 */
typedef union {
    struct {
        /** timern_value : RO; bitpos: [15:0]; default: 0;
         *  Represents current PWM timern counter value.
         */
        uint32_t timern_value:16;
        /** timern_direction : RO; bitpos: [16]; default: 0;
         *  Represents current PWM timern counter direction.\\0: Increment\\1: Decrement
         */
        uint32_t timern_direction:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} mcpwm_timern_status_reg_t;

/** Type of timer_synci_cfg register
 *  Synchronization input selection register for PWM timers.
 */
typedef union {
    struct {
        /** timer0_syncisel : R/W; bitpos: [2:0]; default: 0;
         *  Configures the selection of sync input for PWM timer0.\\1: PWM timer0 sync_out\\2:
         *  PWM timer1 sync_out\\3: PWM timer2 sync_out\\4: SYNC0 from GPIO matrix\\5: SYNC1
         *  from GPIO matrix\\6: SYNC2 from GPIO matrix\\Other values: No sync input selected
         */
        uint32_t timer0_syncisel:3;
        /** timer1_syncisel : R/W; bitpos: [5:3]; default: 0;
         *  Configures the selection of sync input for PWM timer1.\\1: PWM timer0 sync_out\\2:
         *  PWM timer1 sync_out\\3: PWM timer2 sync_out\\4: SYNC0 from GPIO matrix\\5: SYNC1
         *  from GPIO matrix\\6: SYNC2 from GPIO matrix\\Other values: No sync input selected
         */
        uint32_t timer1_syncisel:3;
        /** timer2_syncisel : R/W; bitpos: [8:6]; default: 0;
         *  Configures the selection of sync input for PWM timer2.\\1: PWM timer0 sync_out\\2:
         *  PWM timer1 sync_out\\3: PWM timer2 sync_out\\4: SYNC0 from GPIO matrix\\5: SYNC1
         *  from GPIO matrix\\6: SYNC2 from GPIO matrix\\Other values: No sync input selected
         */
        uint32_t timer2_syncisel:3;
        /** external_synci0_invert : R/W; bitpos: [9]; default: 0;
         *  Configures whether or not to invert SYNC0 from GPIO matrix.\\0: Not invert\\1:
         *  Invert
         */
        uint32_t external_synci0_invert:1;
        /** external_synci1_invert : R/W; bitpos: [10]; default: 0;
         *  Configures whether or not to invert SYNC1 from GPIO matrix.\\0: Not invert\\1:
         *  Invert
         */
        uint32_t external_synci1_invert:1;
        /** external_synci2_invert : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to invert SYNC2 from GPIO matrix.\\0: Not invert\\1:
         *  Invert
         */
        uint32_t external_synci2_invert:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} mcpwm_timer_synci_cfg_reg_t;

/** Type of operator_timersel register
 *  PWM operator's timer select register
 */
typedef union {
    struct {
        /** operator0_timersel : R/W; bitpos: [1:0]; default: 0;
         *  Configures which PWM timer will be the timing reference for PWM operator0.\\0:
         *  Timer0\\1: Timer1\\2: Timer2\\3: Invalid, will select timer2
         */
        uint32_t operator0_timersel:2;
        /** operator1_timersel : R/W; bitpos: [3:2]; default: 0;
         *  Configures which PWM timer will be the timing reference for PWM operator1.\\0:
         *  Timer0\\1: Timer1\\2: Timer2\\3: Invalid, will select timer2
         */
        uint32_t operator1_timersel:2;
        /** operator2_timersel : R/W; bitpos: [5:4]; default: 0;
         *  Configures which PWM timer will be the timing reference for PWM operator2.\\0:
         *  Timer0\\1: Timer1\\2: Timer2\\3: Invalid, will select timer2
         */
        uint32_t operator2_timersel:2;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} mcpwm_operator_timersel_reg_t;


/** Group: cfgrep */
/** Type of genn_stmp_cfg register
 *  Generatorn time stamp registers A and B transfer status and update method register
 */
typedef union {
    struct {
        /** genn_a_upmethod : R/W; bitpos: [3:0]; default: 0;
         *  Configures the update method for PWM generator n time stamp A's active
         *  register.\\0: Immediately\\Bit0 is set to 1: TEZ\\Bit1 is set to 1: TEP\\Bit2 is
         *  set to 1: Sync\\Bit3 is set to 1: Disable the update
         */
        uint32_t genn_a_upmethod:4;
        /** genn_b_upmethod : R/W; bitpos: [7:4]; default: 0;
         *  Configures the update method for PWM generator n time stamp B's active
         *  register.\\0: Immediately\\Bit0 is set to 1: TEZ\\Bit1 is set to 1: TEP\\Bit2 is
         *  set to 1: Sync\\Bit3 is set to 1: Disable the update
         */
        uint32_t genn_b_upmethod:4;
        /** genn_a_shdw_full : R/W/WTC/SC; bitpos: [8]; default: 0;
         *  Represents whether or not generatorn time stamp A's shadow reg is transferred.\\0:
         *  A's active reg has been updated with shadow register latest value.\\1: A's shadow
         *  reg is filled and waiting to be transferred to A's active reg
         */
        uint32_t genn_a_shdw_full:1;
        /** genn_b_shdw_full : R/W/WTC/SC; bitpos: [9]; default: 0;
         *  Represents whether or not generatorn time stamp B's shadow reg is transferred.\\0:
         *  B's active reg has been updated with shadow register latest value.\\1: B's shadow
         *  reg is filled and waiting to be transferred to B's active reg
         */
        uint32_t genn_b_shdw_full:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} mcpwm_genn_stmp_cfg_reg_t;

/** Type of genn_tstmp_a register
 *  Generatorn time stamp A's shadow register
 */
typedef union {
    struct {
        /** genn_a : R/W; bitpos: [15:0]; default: 0;
         *  Configures the value of PWM generator n time stamp A's shadow register.
         */
        uint32_t genn_a:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} mcpwm_genn_tstmp_a_reg_t;

/** Type of genn_tstmp_b register
 *  Generatorn time stamp B's shadow register
 */
typedef union {
    struct {
        /** genn_b : R/W; bitpos: [15:0]; default: 0;
         *  Configures the value of PWM generator n time stamp B's shadow register.
         */
        uint32_t genn_b:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} mcpwm_genn_tstmp_b_reg_t;

/** Type of genn_cfg0 register
 *  Generatorn fault event T0 and T1 configuration register
 */
typedef union {
    struct {
        /** genn_cfg_upmethod : R/W; bitpos: [3:0]; default: 0;
         *  Configures update method for PWM generator n's active register.\\0:
         *  Immediately\\Bit0 is set to 1: TEZ\\Bit1 is set to 1: TEP\\Bit2 is set to 1:
         *  Sync\\Bit3 is set to 1: Disable the update
         */
        uint32_t genn_cfg_upmethod:4;
        /** genn_t0_sel : R/W; bitpos: [6:4]; default: 0;
         *  Configures source selection for PWM generator n event_t0, take effect
         *  immediately.\\0: fault_event0\\1: fault_event1\\2: fault_event2\\3: sync_taken\\4:
         *  Invalid, Select nothing
         */
        uint32_t genn_t0_sel:3;
        /** genn_t1_sel : R/W; bitpos: [9:7]; default: 0;
         *  Configures source selection for PWM generator n event_t1, take effect
         *  immediately.\\0: fault_event0\\1: fault_event1\\2: fault_event2\\3: sync_taken\\4:
         *  Invalid, Select nothing
         */
        uint32_t genn_t1_sel:3;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} mcpwm_genn_cfg0_reg_t;

/** Type of genn_force register
 *  Generatorn output signal force mode register.
 */
typedef union {
    struct {
        /** genn_cntuforce_upmethod : R/W; bitpos: [5:0]; default: 32;
         *  Configures update method for continuous software force of PWM generatorn.\\0:
         *  Immediately\\Bit0 is set to 1: TEZ\\Bit1 is set to 1: TEP\\Bit2 is set to 1:
         *  TEA\\Bit3 is set to 1: TEB\\Bit4 is set to 1: Sync\\Bit5 is set to 1: Disable
         *  update. TEA/B here and below means an event generated when the timer's value equals
         *  to that of register A/B.
         */
        uint32_t genn_cntuforce_upmethod:6;
        /** genn_a_cntuforce_mode : R/W; bitpos: [7:6]; default: 0;
         *  Configures continuous software force mode for PWMn A.\\0: Disabled\\1: Low\\2:
         *  High\\3: Disabled
         */
        uint32_t genn_a_cntuforce_mode:2;
        /** genn_b_cntuforce_mode : R/W; bitpos: [9:8]; default: 0;
         *  Configures continuous software force mode for PWMn B.\\0: Disabled\\1: Low\\2:
         *  High\\3: Disabled
         */
        uint32_t genn_b_cntuforce_mode:2;
        /** genn_a_nciforce : R/W; bitpos: [10]; default: 0;
         *  Configures the generation of non-continuous immediate software-force event for PWMn
         *  A, a toggle will trigger a force event.
         */
        uint32_t genn_a_nciforce:1;
        /** genn_a_nciforce_mode : R/W; bitpos: [12:11]; default: 0;
         *  Configures non-continuous immediate software force mode for PWMn A.\\0:
         *  Disabled\\1: Low\\2: High\\3: Disabled
         */
        uint32_t genn_a_nciforce_mode:2;
        /** genn_b_nciforce : R/W; bitpos: [13]; default: 0;
         *  Configures the generation of non-continuous immediate software-force event for PWMn
         *  B, a toggle will trigger a force event.
         */
        uint32_t genn_b_nciforce:1;
        /** genn_b_nciforce_mode : R/W; bitpos: [15:14]; default: 0;
         *  Configures non-continuous immediate software force mode for PWMn B.\\0:
         *  Disabled\\1: Low\\2: High\\3: Disabled
         */
        uint32_t genn_b_nciforce_mode:2;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} mcpwm_genn_force_reg_t;

/** Type of genn_a register
 *  PWMn output signal A actions configuration register
 */
typedef union {
    struct {
        /** genn_a_utez : R/W; bitpos: [1:0]; default: 0;
         *  Configures action on PWMn A triggered by event TEZ when timer increasing.\\0: No
         *  change\\1: Low\\2: High\\3: Toggle
         */
        uint32_t genn_a_utez:2;
        /** genn_a_utep : R/W; bitpos: [3:2]; default: 0;
         *  Configures action on PWMn A triggered by event TEP when timer increasing.\\0: No
         *  change\\1: Low\\2: High\\3: Toggle
         */
        uint32_t genn_a_utep:2;
        /** genn_a_utea : R/W; bitpos: [5:4]; default: 0;
         *  Configures action on PWMn A triggered by event TEA when timer increasing.\\0: No
         *  change\\1: Low\\2: High\\3: Toggle
         */
        uint32_t genn_a_utea:2;
        /** genn_a_uteb : R/W; bitpos: [7:6]; default: 0;
         *  Configures action on PWMn A triggered by event TEB when timer increasing.\\0: No
         *  change\\1: Low\\2: High\\3: Toggle
         */
        uint32_t genn_a_uteb:2;
        /** genn_a_ut0 : R/W; bitpos: [9:8]; default: 0;
         *  Configures action on PWMn A triggered by event_t0 when timer increasing.\\0: No
         *  change\\1: Low\\2: High\\3: Toggle
         */
        uint32_t genn_a_ut0:2;
        /** genn_a_ut1 : R/W; bitpos: [11:10]; default: 0;
         *  Configures action on PWMn A triggered by event_t1 when timer increasing.\\0: No
         *  change\\1: Low\\2: High\\3: Toggle
         */
        uint32_t genn_a_ut1:2;
        /** genn_a_dtez : R/W; bitpos: [13:12]; default: 0;
         *  Configures action on PWMn A triggered by event TEZ when timer decreasing.\\0: No
         *  change\\1: Low\\2: High\\3: Toggle
         */
        uint32_t genn_a_dtez:2;
        /** genn_a_dtep : R/W; bitpos: [15:14]; default: 0;
         *  Configures action on PWMn A triggered by event TEP when timer decreasing.\\0: No
         *  change\\1: Low\\2: High\\3: Toggle
         */
        uint32_t genn_a_dtep:2;
        /** genn_a_dtea : R/W; bitpos: [17:16]; default: 0;
         *  Configures action on PWMn A triggered by event TEA when timer decreasing.\\0: No
         *  change\\1: Low\\2: High\\3: Toggle
         */
        uint32_t genn_a_dtea:2;
        /** genn_a_dteb : R/W; bitpos: [19:18]; default: 0;
         *  Configures action on PWMn A triggered by event TEB when timer decreasing.\\0: No
         *  change\\1: Low\\2: High\\3: Toggle
         */
        uint32_t genn_a_dteb:2;
        /** genn_a_dt0 : R/W; bitpos: [21:20]; default: 0;
         *  Configures action on PWMn A triggered by event_t0 when timer decreasing.\\0: No
         *  change\\1: Low\\2: High\\3: Toggle
         */
        uint32_t genn_a_dt0:2;
        /** genn_a_dt1 : R/W; bitpos: [23:22]; default: 0;
         *  Configures action on PWMn A triggered by event_t1 when timer decreasing.\\0: No
         *  change\\1: Low\\2: High\\3: Toggle
         */
        uint32_t genn_a_dt1:2;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} mcpwm_genn_a_reg_t;

/** Type of genn_b register
 *  PWMn output signal B actions configuration register
 */
typedef union {
    struct {
        /** genn_b_utez : R/W; bitpos: [1:0]; default: 0;
         *  Configures action on PWMn B triggered by event TEZ when timer increasing.\\0: No
         *  change\\1: Low\\2: High\\3: Toggle
         */
        uint32_t genn_b_utez:2;
        /** genn_b_utep : R/W; bitpos: [3:2]; default: 0;
         *  Configures action on PWMn B triggered by event TEP when timer increasing.\\0: No
         *  change\\1: Low\\2: High\\3: Toggle
         */
        uint32_t genn_b_utep:2;
        /** genn_b_utea : R/W; bitpos: [5:4]; default: 0;
         *  Configures action on PWMn B triggered by event TEA when timer increasing.\\0: No
         *  change\\1: Low\\2: High\\3: Toggle
         */
        uint32_t genn_b_utea:2;
        /** genn_b_uteb : R/W; bitpos: [7:6]; default: 0;
         *  Configures action on PWMn B triggered by event TEB when timer increasing.\\0: No
         *  change\\1: Low\\2: High\\3: Toggle
         */
        uint32_t genn_b_uteb:2;
        /** genn_b_ut0 : R/W; bitpos: [9:8]; default: 0;
         *  Configures action on PWMn B triggered by event_t0 when timer increasing.\\0: No
         *  change\\1: Low\\2: High\\3: Toggle
         */
        uint32_t genn_b_ut0:2;
        /** genn_b_ut1 : R/W; bitpos: [11:10]; default: 0;
         *  Configures action on PWMn B triggered by event_t1 when timer increasing.\\0: No
         *  change\\1: Low\\2: High\\3: Toggle
         */
        uint32_t genn_b_ut1:2;
        /** genn_b_dtez : R/W; bitpos: [13:12]; default: 0;
         *  Configures action on PWMn B triggered by event TEZ when timer decreasing.\\0: No
         *  change\\1: Low\\2: High\\3: Toggle
         */
        uint32_t genn_b_dtez:2;
        /** genn_b_dtep : R/W; bitpos: [15:14]; default: 0;
         *  Configures action on PWMn B triggered by event TEP when timer decreasing.\\0: No
         *  change\\1: Low\\2: High\\3: Toggle
         */
        uint32_t genn_b_dtep:2;
        /** genn_b_dtea : R/W; bitpos: [17:16]; default: 0;
         *  Configures action on PWMn B triggered by event TEA when timer decreasing.\\0: No
         *  change\\1: Low\\2: High\\3: Toggle
         */
        uint32_t genn_b_dtea:2;
        /** genn_b_dteb : R/W; bitpos: [19:18]; default: 0;
         *  Configures action on PWMn B triggered by event TEB when timer decreasing.\\0: No
         *  change\\1: Low\\2: High\\3: Toggle
         */
        uint32_t genn_b_dteb:2;
        /** genn_b_dt0 : R/W; bitpos: [21:20]; default: 0;
         *  Configures action on PWMn B triggered by event_t0 when timer decreasing.\\0: No
         *  change\\1: Low\\2: High\\3: Toggle
         */
        uint32_t genn_b_dt0:2;
        /** genn_b_dt1 : R/W; bitpos: [23:22]; default: 0;
         *  Configures action on PWMn B triggered by event_t1 when timer decreasing.\\0: No
         *  change\\1: Low\\2: High\\3: Toggle
         */
        uint32_t genn_b_dt1:2;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} mcpwm_genn_b_reg_t;

/** Type of dtn_cfg register
 *  Dead time configuration register
 */
typedef union {
    struct {
        /** dtn_fed_upmethod : R/W; bitpos: [3:0]; default: 0;
         *  Configures update method for FED (Falling edge delay) active register.\\0:
         *  Immediate\\Bit0 is set to 1: TEZ\\Bit1 is set to 1: TEP\\Bit2 is set to 1:
         *  Sync\\Bit3 is set to 1: Disable the update
         */
        uint32_t dtn_fed_upmethod:4;
        /** dtn_red_upmethod : R/W; bitpos: [7:4]; default: 0;
         *  Configures update method for RED (rising edge delay) active register.\\0:
         *  Immediate\\Bit0 is set to 1: TEZ\\Bit1 is set to 1: TEP\\Bit2 is set to 1:
         *  Sync\\Bit3 is set to 1: Disable the update
         */
        uint32_t dtn_red_upmethod:4;
        /** dtn_deb_mode : R/W; bitpos: [8]; default: 0;
         *  Configures S8 in table, dual-edge B mode.\\0: fed/red take effect on different path
         *  separately\\1: fed/red take effect on B path, A out is in bypass or dulpB mode
         */
        uint32_t dtn_deb_mode:1;
        /** dtn_a_outswap : R/W; bitpos: [9]; default: 0;
         *  Configures S6 in table.
         */
        uint32_t dtn_a_outswap:1;
        /** dtn_b_outswap : R/W; bitpos: [10]; default: 0;
         *  Configures S7 in table.
         */
        uint32_t dtn_b_outswap:1;
        /** dtn_red_insel : R/W; bitpos: [11]; default: 0;
         *  Configures S4 in table.
         */
        uint32_t dtn_red_insel:1;
        /** dtn_fed_insel : R/W; bitpos: [12]; default: 0;
         *  Configures S5 in table.
         */
        uint32_t dtn_fed_insel:1;
        /** dtn_red_outinvert : R/W; bitpos: [13]; default: 0;
         *  Configures S2 in table.
         */
        uint32_t dtn_red_outinvert:1;
        /** dtn_fed_outinvert : R/W; bitpos: [14]; default: 0;
         *  Configures S3 in table.
         */
        uint32_t dtn_fed_outinvert:1;
        /** dtn_a_outbypass : R/W; bitpos: [15]; default: 1;
         *  Configures S1 in table.
         */
        uint32_t dtn_a_outbypass:1;
        /** dtn_b_outbypass : R/W; bitpos: [16]; default: 1;
         *  Configures S0 in table.
         */
        uint32_t dtn_b_outbypass:1;
        /** dtn_clk_sel : R/W; bitpos: [17]; default: 0;
         *  Configures dead time generator n clock selection.\\0: PWM_clk\\1: PT_clk
         */
        uint32_t dtn_clk_sel:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} mcpwm_dtn_cfg_reg_t;

/** Type of dtn_fed_cfg register
 *  Falling edge delay (FED) shadow register
 */
typedef union {
    struct {
        /** dtn_fed : R/W; bitpos: [15:0]; default: 0;
         *  Configures shadow register for FED.
         */
        uint32_t dtn_fed:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} mcpwm_dtn_fed_cfg_reg_t;

/** Type of dtn_red_cfg register
 *  Rising edge delay (RED) shadow register
 */
typedef union {
    struct {
        /** dtn_red : R/W; bitpos: [15:0]; default: 0;
         *  Configures shadow register for RED.
         */
        uint32_t dtn_red:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} mcpwm_dtn_red_cfg_reg_t;

/** Type of carriern_cfg register
 *  Carriern configuration register
 */
typedef union {
    struct {
        /** carriern_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enable carriern.\\0: Bypassed\\1: Enabled
         */
        uint32_t carriern_en:1;
        /** carriern_prescale : R/W; bitpos: [4:1]; default: 0;
         *  Configures the prescale value of PWM carriern clock (PC_clk), so that period of
         *  PC_clk = period of PWM_clk * (PWM_CARRIERn_PRESCALE + 1)
         */
        uint32_t carriern_prescale:4;
        /** carriern_duty : R/W; bitpos: [7:5]; default: 0;
         *  Configures carrier duty. Duty = PWM_CARRIERn_DUTY / 8
         */
        uint32_t carriern_duty:3;
        /** carriern_oshtwth : R/W; bitpos: [11:8]; default: 0;
         *  Configures width of the first pulse. Measurement unit: Periods of the carrier.
         */
        uint32_t carriern_oshtwth:4;
        /** carriern_out_invert : R/W; bitpos: [12]; default: 0;
         *  Configures whether or not to invert the output of PWMn A and PWMn B for this
         *  submodule.\\0: Normal\\1: Invert
         */
        uint32_t carriern_out_invert:1;
        /** carriern_in_invert : R/W; bitpos: [13]; default: 0;
         *  Configures whether or not to invert the input of PWMn A and PWMn B for this
         *  submodule.\\0: Normal\\1: Invert
         */
        uint32_t carriern_in_invert:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} mcpwm_carriern_cfg_reg_t;

/** Type of fhn_cfg0 register
 *  PWMn A and PWMn B trip events actions configuration register
 */
typedef union {
    struct {
        /** fhn_sw_cbc : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enable software force cycle-by-cycle mode action.\\0:
         *  Disable\\1: Enable
         */
        uint32_t fhn_sw_cbc:1;
        /** fhn_f2_cbc : R/W; bitpos: [1]; default: 0;
         *  Configures whether or not event_f2 will trigger cycle-by-cycle mode action.\\0:
         *  Disable\\1: Enable
         */
        uint32_t fhn_f2_cbc:1;
        /** fhn_f1_cbc : R/W; bitpos: [2]; default: 0;
         *  Configures whether or not event_f1 will trigger cycle-by-cycle mode action.\\0:
         *  Disable\\1: Enable
         */
        uint32_t fhn_f1_cbc:1;
        /** fhn_f0_cbc : R/W; bitpos: [3]; default: 0;
         *  Configures whether or not event_f0 will trigger cycle-by-cycle mode action.\\0:
         *  Disable\\1: Enable
         */
        uint32_t fhn_f0_cbc:1;
        /** fhn_sw_ost : R/W; bitpos: [4]; default: 0;
         *  Configures whether or not to enable software force one-shot mode action.\\0:
         *  Disable\\1: Enable
         */
        uint32_t fhn_sw_ost:1;
        /** fhn_f2_ost : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not event_f2 will trigger one-shot mode action.\\0:
         *  Disable\\1: Enable
         */
        uint32_t fhn_f2_ost:1;
        /** fhn_f1_ost : R/W; bitpos: [6]; default: 0;
         *  Configures whether or not event_f1 will trigger one-shot mode action.\\0:
         *  Disable\\1: Enable
         */
        uint32_t fhn_f1_ost:1;
        /** fhn_f0_ost : R/W; bitpos: [7]; default: 0;
         *  Configures whether or not event_f0 will trigger one-shot mode action.\\0:
         *  Disable\\1: Enable
         */
        uint32_t fhn_f0_ost:1;
        /** fhn_a_cbc_d : R/W; bitpos: [9:8]; default: 0;
         *  Configures cycle-by-cycle mode action on PWMn A when fault event occurs and timer
         *  is decreasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
         */
        uint32_t fhn_a_cbc_d:2;
        /** fhn_a_cbc_u : R/W; bitpos: [11:10]; default: 0;
         *  Configures cycle-by-cycle mode action on PWMn A when fault event occurs and timer
         *  is increasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
         */
        uint32_t fhn_a_cbc_u:2;
        /** fhn_a_ost_d : R/W; bitpos: [13:12]; default: 0;
         *  Configures one-shot mode action on PWMn A when fault event occurs and timer is
         *  decreasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
         */
        uint32_t fhn_a_ost_d:2;
        /** fhn_a_ost_u : R/W; bitpos: [15:14]; default: 0;
         *  Configures one-shot mode action on PWMn A when fault event occurs and timer is
         *  increasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
         */
        uint32_t fhn_a_ost_u:2;
        /** fhn_b_cbc_d : R/W; bitpos: [17:16]; default: 0;
         *  Configures cycle-by-cycle mode action on PWMn B when fault event occurs and timer
         *  is decreasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
         */
        uint32_t fhn_b_cbc_d:2;
        /** fhn_b_cbc_u : R/W; bitpos: [19:18]; default: 0;
         *  Configures cycle-by-cycle mode action on PWMn B when fault event occurs and timer
         *  is increasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
         */
        uint32_t fhn_b_cbc_u:2;
        /** fhn_b_ost_d : R/W; bitpos: [21:20]; default: 0;
         *  Configures one-shot mode action on PWMn B when fault event occurs and timer is
         *  decreasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
         */
        uint32_t fhn_b_ost_d:2;
        /** fhn_b_ost_u : R/W; bitpos: [23:22]; default: 0;
         *  Configures one-shot mode action on PWMn B when fault event occurs and timer is
         *  increasing.\\0: Do nothing\\1: Force low\\2: Force high\\3: Toggle
         */
        uint32_t fhn_b_ost_u:2;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} mcpwm_fhn_cfg0_reg_t;

/** Type of fhn_cfg1 register
 *  Software triggers for fault handler actions configuration register
 */
typedef union {
    struct {
        /** fhn_clr_ost : R/W; bitpos: [0]; default: 0;
         *  Configures the generation of software one-shot mode action clear. A toggle
         *  (software negate its value) triggers a clear for on going one-shot mode action.
         */
        uint32_t fhn_clr_ost:1;
        /** fhn_cbcpulse : R/W; bitpos: [2:1]; default: 0;
         *  Configures the refresh moment selection of cycle-by-cycle mode action.\\0: Select
         *  nothing, will not refresh\\Bit0 is set to 1: TEZ\\Bit1 is set to 1: TEP
         */
        uint32_t fhn_cbcpulse:2;
        /** fhn_force_cbc : R/W; bitpos: [3]; default: 0;
         *  Configures the generation of software cycle-by-cycle mode action. A toggle
         *  (software negate its value) triggers a cycle-by-cycle mode action.
         */
        uint32_t fhn_force_cbc:1;
        /** fhn_force_ost : R/W; bitpos: [4]; default: 0;
         *  Configures the generation of software one-shot mode action. A toggle (software
         *  negate its value) triggers a one-shot mode action.
         */
        uint32_t fhn_force_ost:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} mcpwm_fhn_cfg1_reg_t;

/** Type of fhn_status register
 *  Fault events status register
 */
typedef union {
    struct {
        /** fhn_cbc_on : RO; bitpos: [0]; default: 0;
         *  Represents whether or not an cycle-by-cycle mode action is on going.\\0:No
         *  action\\1: On going
         */
        uint32_t fhn_cbc_on:1;
        /** fhn_ost_on : RO; bitpos: [1]; default: 0;
         *  Represents whether or not an one-shot mode action is on going.\\0:No action\\1: On
         *  going
         */
        uint32_t fhn_ost_on:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} mcpwm_fhn_status_reg_t;


/** Group: fault det */
/** Type of fault_detect register
 *  Fault detection configuration and status register
 */
typedef union {
    struct {
        /** f0_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enable event_f0 generation.\\0: Disable\\1: Enable
         */
        uint32_t f0_en:1;
        /** f1_en : R/W; bitpos: [1]; default: 0;
         *  Configures whether or not to enable event_f1 generation.\\0: Disable\\1: Enable
         */
        uint32_t f1_en:1;
        /** f2_en : R/W; bitpos: [2]; default: 0;
         *  Configures whether or not to enable event_f2 generation.\\0: Disable\\1: Enable
         */
        uint32_t f2_en:1;
        /** f0_pole : R/W; bitpos: [3]; default: 0;
         *  Configures event_f0 trigger polarity on FAULT0 source from GPIO matrix.\\0: Level
         *  low\\1: Level high
         */
        uint32_t f0_pole:1;
        /** f1_pole : R/W; bitpos: [4]; default: 0;
         *  Configures event_f1 trigger polarity on FAULT1 source from GPIO matrix.\\0: Level
         *  low\\1: Level high
         */
        uint32_t f1_pole:1;
        /** f2_pole : R/W; bitpos: [5]; default: 0;
         *  Configures event_f2 trigger polarity on FAULT2 source from GPIO matrix.\\0: Level
         *  low\\1: Level high
         */
        uint32_t f2_pole:1;
        /** event_f0 : RO; bitpos: [6]; default: 0;
         *  Represents whether or not an event_f0 is on going.\\0: No action\\1: On going
         */
        uint32_t event_f0:1;
        /** event_f1 : RO; bitpos: [7]; default: 0;
         *  Represents whether or not an event_f1 is on going.\\0: No action\\1: On going
         */
        uint32_t event_f1:1;
        /** event_f2 : RO; bitpos: [8]; default: 0;
         *  Represents whether or not an event_f2 is on going.\\0: No action\\1: On going
         */
        uint32_t event_f2:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} mcpwm_fault_detect_reg_t;


/** Group: sync */
/** Type of cap_timer_cfg register
 *  Capture timer configuration register
 */
typedef union {
    struct {
        /** cap_timer_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enable capture timer increment.\\0: Disable\\1: Enable
         */
        uint32_t cap_timer_en:1;
        /** cap_synci_en : R/W; bitpos: [1]; default: 0;
         *  Configures whether or not to enable capture timer sync.\\0: Disable\\1: Enable
         */
        uint32_t cap_synci_en:1;
        /** cap_synci_sel : R/W; bitpos: [4:2]; default: 0;
         *  Configures the selection of capture module sync input.\\0: None\\1: Timer0
         *  sync_out\\2: Timer1 sync_out\\3: Timer2 sync_out\\4: SYNC0 from GPIO matrix\\5:
         *  SYNC1 from GPIO matrix\\6: SYNC2 from GPIO matrix\\7: None
         */
        uint32_t cap_synci_sel:3;
        /** cap_sync_sw : WT; bitpos: [5]; default: 0;
         *  Configures the generation of a capture timer sync when reg_cap_synci_en is 1.\\0:
         *  Invalid, No effect\\1: Trigger a capture timer sync, capture timer is loaded with
         *  value in phase register
         */
        uint32_t cap_sync_sw:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} mcpwm_cap_timer_cfg_reg_t;


/** Group: phase */
/** Type of cap_timer_phase register
 *  Capture timer sync phase register
 */
typedef union {
    struct {
        /** cap_phase : R/W; bitpos: [31:0]; default: 0;
         *  Configures phase value for capture timer sync operation.
         */
        uint32_t cap_phase:32;
    };
    uint32_t val;
} mcpwm_cap_timer_phase_reg_t;


/** Group: cap_cfg */
/** Type of cap_chn_cfg register
 *  Capture channel n configuration register
 */
typedef union {
    struct {
        /** capn_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enable capture on channel n.\\0: Disable\\1: Enable
         */
        uint32_t capn_en:1;
        /** capn_mode : R/W; bitpos: [2:1]; default: 0;
         *  Configures which edge of capture on channel n after prescaling is used.\\0:
         *  None\\Bit0 is set to 1: Rnable capture on the negative edge\\Bit1 is set to 1:
         *  Enable capture on the positive edge
         */
        uint32_t capn_mode:2;
        /** capn_prescale : R/W; bitpos: [10:3]; default: 0;
         *  Configures prescale value on possitive edge of CAPn. Prescale value =
         *  PWM_CAPn_PRESCALE + 1
         */
        uint32_t capn_prescale:8;
        /** capn_in_invert : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to invert CAPn from GPIO matrix before prescale.\\0:
         *  Normal\\1: Invert
         */
        uint32_t capn_in_invert:1;
        /** capn_sw : WT; bitpos: [12]; default: 0;
         *  Configures the generation of software capture.\\0: Invalid, No effect\\1: Trigger a
         *  software forced capture on channel n
         */
        uint32_t capn_sw:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} mcpwm_cap_chn_cfg_reg_t;


/** Group: cap_ch */
/** Type of cap_chn register
 *  CAPn capture value register
 */
typedef union {
    struct {
        /** capn_value : RO; bitpos: [31:0]; default: 0;
         *  Represents value of last capture on CAPn
         */
        uint32_t capn_value:32;
    };
    uint32_t val;
} mcpwm_cap_chn_reg_t;


/** Group: edge */
/** Type of cap_status register
 *  Last capture trigger edge information register
 */
typedef union {
    struct {
        /** cap0_edge : RO; bitpos: [0]; default: 0;
         *  Represents edge of last capture trigger on channel0.\\0: Posedge\\1: Negedge
         */
        uint32_t cap0_edge:1;
        /** cap1_edge : RO; bitpos: [1]; default: 0;
         *  Represents edge of last capture trigger on channel1.\\0: Posedge\\1: Negedge
         */
        uint32_t cap1_edge:1;
        /** cap2_edge : RO; bitpos: [2]; default: 0;
         *  Represents edge of last capture trigger on channel2.\\0: Posedge\\1: Negedge
         */
        uint32_t cap2_edge:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} mcpwm_cap_status_reg_t;


/** Group: update */
/** Type of update_cfg register
 *  Generator Update configuration register
 */
typedef union {
    struct {
        /** global_up_en : R/W; bitpos: [0]; default: 1;
         *  Configures whether or not to enable global update for all active registers in MCPWM
         *  module.\\0: Disable\\1: Enable
         */
        uint32_t global_up_en:1;
        /** global_force_up : R/W; bitpos: [1]; default: 0;
         *  Configures the generation of global forced update for all active registers in MCPWM
         *  module. A toggle (software invert its value) will trigger a global forced update.
         *  Valid only when MCPWM_GLOBAL_UP_EN and MCPWM_OP0/1/2_UP_EN are both set to 1.
         */
        uint32_t global_force_up:1;
        /** op0_up_en : R/W; bitpos: [2]; default: 1;
         *  Configures whether or not to enable update of active registers in PWM operator$n.
         *  Valid only when PWM_GLOBAL_UP_EN is set to 1.\\0: Disable\\1: Enable
         */
        uint32_t op0_up_en:1;
        /** op0_force_up : R/W; bitpos: [3]; default: 0;
         *  Configures the generation of forced update for active registers in PWM operator0. A
         *  toggle (software invert its value) will trigger a forced update. Valid only when
         *  MCPWM_GLOBAL_UP_EN and MCPWM_OP0_UP_EN are both set to 1.
         */
        uint32_t op0_force_up:1;
        /** op1_up_en : R/W; bitpos: [4]; default: 1;
         *  Configures whether or not to enable update of active registers in PWM operator$n.
         *  Valid only when PWM_GLOBAL_UP_EN is set to 1.\\0: Disable\\1: Enable
         */
        uint32_t op1_up_en:1;
        /** op1_force_up : R/W; bitpos: [5]; default: 0;
         *  Configures the generation of forced update for active registers in PWM operator1. A
         *  toggle (software invert its value) will trigger a forced update. Valid only when
         *  MCPWM_GLOBAL_UP_EN and MCPWM_OP1_UP_EN are both set to 1.
         */
        uint32_t op1_force_up:1;
        /** op2_up_en : R/W; bitpos: [6]; default: 1;
         *  Configures whether or not to enable update of active registers in PWM operator$n.
         *  Valid only when PWM_GLOBAL_UP_EN is set to 1.\\0: Disable\\1: Enable
         */
        uint32_t op2_up_en:1;
        /** op2_force_up : R/W; bitpos: [7]; default: 0;
         *  Configures the generation of forced update for active registers in PWM operator2. A
         *  toggle (software invert its value) will trigger a forced update. Valid only when
         *  MCPWM_GLOBAL_UP_EN and MCPWM_OP2_UP_EN are both set to 1.
         */
        uint32_t op2_force_up:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} mcpwm_update_cfg_reg_t;


/** Group: Interrupt register */
/** Type of int_ena register
 *  Interrupt enable register
 */
typedef union {
    struct {
        /** timer0_stop_int_ena : R/W; bitpos: [0]; default: 0;
         *  Enable bit: Write 1 to enable the interrupt triggered when the timer 0 stops.
         */
        uint32_t timer0_stop_int_ena:1;
        /** timer1_stop_int_ena : R/W; bitpos: [1]; default: 0;
         *  Enable bit: Write 1 to enable the interrupt triggered when the timer 1 stops.
         */
        uint32_t timer1_stop_int_ena:1;
        /** timer2_stop_int_ena : R/W; bitpos: [2]; default: 0;
         *  Enable bit: Write 1 to enable the interrupt triggered when the timer 2 stops.
         */
        uint32_t timer2_stop_int_ena:1;
        /** timer0_tez_int_ena : R/W; bitpos: [3]; default: 0;
         *  Enable bit: Write 1 to enable the interrupt triggered by a PWM timer 0 TEZ event.
         */
        uint32_t timer0_tez_int_ena:1;
        /** timer1_tez_int_ena : R/W; bitpos: [4]; default: 0;
         *  Enable bit: Write 1 to enable the interrupt triggered by a PWM timer 1 TEZ event.
         */
        uint32_t timer1_tez_int_ena:1;
        /** timer2_tez_int_ena : R/W; bitpos: [5]; default: 0;
         *  Enable bit: Write 1 to enable the interrupt triggered by a PWM timer 2 TEZ event.
         */
        uint32_t timer2_tez_int_ena:1;
        /** timer0_tep_int_ena : R/W; bitpos: [6]; default: 0;
         *  Enable bit: Write 1 to enable the interrupt triggered by a PWM timer 0 TEP event.
         */
        uint32_t timer0_tep_int_ena:1;
        /** timer1_tep_int_ena : R/W; bitpos: [7]; default: 0;
         *  Enable bit: Write 1 to enable the interrupt triggered by a PWM timer 1 TEP event.
         */
        uint32_t timer1_tep_int_ena:1;
        /** timer2_tep_int_ena : R/W; bitpos: [8]; default: 0;
         *  Enable bit: Write 1 to enable the interrupt triggered by a PWM timer 2 TEP event.
         */
        uint32_t timer2_tep_int_ena:1;
        /** fault0_int_ena : R/W; bitpos: [9]; default: 0;
         *  Enable bit: Write 1 to enable the interrupt triggered when event_f0 starts.
         */
        uint32_t fault0_int_ena:1;
        /** fault1_int_ena : R/W; bitpos: [10]; default: 0;
         *  Enable bit: Write 1 to enable the interrupt triggered when event_f1 starts.
         */
        uint32_t fault1_int_ena:1;
        /** fault2_int_ena : R/W; bitpos: [11]; default: 0;
         *  Enable bit: Write 1 to enable the interrupt triggered when event_f2 starts.
         */
        uint32_t fault2_int_ena:1;
        /** fault0_clr_int_ena : R/W; bitpos: [12]; default: 0;
         *  Enable bit: Write 1 to enable the interrupt triggered when event_f0 clears.
         */
        uint32_t fault0_clr_int_ena:1;
        /** fault1_clr_int_ena : R/W; bitpos: [13]; default: 0;
         *  Enable bit: Write 1 to enable the interrupt triggered when event_f1 clears.
         */
        uint32_t fault1_clr_int_ena:1;
        /** fault2_clr_int_ena : R/W; bitpos: [14]; default: 0;
         *  Enable bit: Write 1 to enable the interrupt triggered when event_f2 clears.
         */
        uint32_t fault2_clr_int_ena:1;
        /** cmpr0_tea_int_ena : R/W; bitpos: [15]; default: 0;
         *  Enable bit: Write 1 to enable the interrupt triggered by a PWM operator 0 TEA event.
         */
        uint32_t cmpr0_tea_int_ena:1;
        /** cmpr1_tea_int_ena : R/W; bitpos: [16]; default: 0;
         *  Enable bit: Write 1 to enable the interrupt triggered by a PWM operator 1 TEA event.
         */
        uint32_t cmpr1_tea_int_ena:1;
        /** cmpr2_tea_int_ena : R/W; bitpos: [17]; default: 0;
         *  Enable bit: Write 1 to enable the interrupt triggered by a PWM operator 2 TEA event.
         */
        uint32_t cmpr2_tea_int_ena:1;
        /** cmpr0_teb_int_ena : R/W; bitpos: [18]; default: 0;
         *  Enable bit: Write 1 to enable the interrupt triggered by a PWM operator 0 TEB event.
         */
        uint32_t cmpr0_teb_int_ena:1;
        /** cmpr1_teb_int_ena : R/W; bitpos: [19]; default: 0;
         *  Enable bit: Write 1 to enable the interrupt triggered by a PWM operator 1 TEB event.
         */
        uint32_t cmpr1_teb_int_ena:1;
        /** cmpr2_teb_int_ena : R/W; bitpos: [20]; default: 0;
         *  Enable bit: Write 1 to enable the interrupt triggered by a PWM operator 2 TEB event.
         */
        uint32_t cmpr2_teb_int_ena:1;
        /** tz0_cbc_int_ena : R/W; bitpos: [21]; default: 0;
         *  Enable bit: Write 1 to enable the interrupt triggered by a cycle-by-cycle mode
         *  action on PWM0.
         */
        uint32_t tz0_cbc_int_ena:1;
        /** tz1_cbc_int_ena : R/W; bitpos: [22]; default: 0;
         *  Enable bit: Write 1 to enable the interrupt triggered by a cycle-by-cycle mode
         *  action on PWM1.
         */
        uint32_t tz1_cbc_int_ena:1;
        /** tz2_cbc_int_ena : R/W; bitpos: [23]; default: 0;
         *  Enable bit: Write 1 to enable the interrupt triggered by a cycle-by-cycle mode
         *  action on PWM2.
         */
        uint32_t tz2_cbc_int_ena:1;
        /** tz0_ost_int_ena : R/W; bitpos: [24]; default: 0;
         *  Enable bit: Write 1 to enable the interrupt triggered by a one-shot mode action on
         *  PWM0.
         */
        uint32_t tz0_ost_int_ena:1;
        /** tz1_ost_int_ena : R/W; bitpos: [25]; default: 0;
         *  Enable bit: Write 1 to enable the interrupt triggered by a one-shot mode action on
         *  PWM1.
         */
        uint32_t tz1_ost_int_ena:1;
        /** tz2_ost_int_ena : R/W; bitpos: [26]; default: 0;
         *  Enable bit: Write 1 to enable the interrupt triggered by a one-shot mode action on
         *  PWM2.
         */
        uint32_t tz2_ost_int_ena:1;
        /** cap0_int_ena : R/W; bitpos: [27]; default: 0;
         *  Enable bit: Write 1 to enable the interrupt triggered by capture on CAP0.
         */
        uint32_t cap0_int_ena:1;
        /** cap1_int_ena : R/W; bitpos: [28]; default: 0;
         *  Enable bit: Write 1 to enable the interrupt triggered by capture on CAP1.
         */
        uint32_t cap1_int_ena:1;
        /** cap2_int_ena : R/W; bitpos: [29]; default: 0;
         *  Enable bit: Write 1 to enable the interrupt triggered by capture on CAP2.
         */
        uint32_t cap2_int_ena:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} mcpwm_int_ena_reg_t;

/** Type of int_raw register
 *  Interrupt raw status register
 */
typedef union {
    struct {
        /** timer0_stop_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  Raw status bit: The raw interrupt status of the interrupt triggered when the timer
         *  0 stops.
         */
        uint32_t timer0_stop_int_raw:1;
        /** timer1_stop_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  Raw status bit: The raw interrupt status of the interrupt triggered when the timer
         *  1 stops.
         */
        uint32_t timer1_stop_int_raw:1;
        /** timer2_stop_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  Raw status bit: The raw interrupt status of the interrupt triggered when the timer
         *  2 stops.
         */
        uint32_t timer2_stop_int_raw:1;
        /** timer0_tez_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  Raw status bit: The raw interrupt status of the interrupt triggered by a PWM timer
         *  0 TEZ event.
         */
        uint32_t timer0_tez_int_raw:1;
        /** timer1_tez_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  Raw status bit: The raw interrupt status of the interrupt triggered by a PWM timer
         *  1 TEZ event.
         */
        uint32_t timer1_tez_int_raw:1;
        /** timer2_tez_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  Raw status bit: The raw interrupt status of the interrupt triggered by a PWM timer
         *  2 TEZ event.
         */
        uint32_t timer2_tez_int_raw:1;
        /** timer0_tep_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  Raw status bit: The raw interrupt status of the interrupt triggered by a PWM timer
         *  0 TEP event.
         */
        uint32_t timer0_tep_int_raw:1;
        /** timer1_tep_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  Raw status bit: The raw interrupt status of the interrupt triggered by a PWM timer
         *  1 TEP event.
         */
        uint32_t timer1_tep_int_raw:1;
        /** timer2_tep_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  Raw status bit: The raw interrupt status of the interrupt triggered by a PWM timer
         *  2 TEP event.
         */
        uint32_t timer2_tep_int_raw:1;
        /** fault0_int_raw : R/WTC/SS; bitpos: [9]; default: 0;
         *  Raw status bit: The raw interrupt status of the interrupt triggered when event_f0
         *  starts.
         */
        uint32_t fault0_int_raw:1;
        /** fault1_int_raw : R/WTC/SS; bitpos: [10]; default: 0;
         *  Raw status bit: The raw interrupt status of the interrupt triggered when event_f1
         *  starts.
         */
        uint32_t fault1_int_raw:1;
        /** fault2_int_raw : R/WTC/SS; bitpos: [11]; default: 0;
         *  Raw status bit: The raw interrupt status of the interrupt triggered when event_f2
         *  starts.
         */
        uint32_t fault2_int_raw:1;
        /** fault0_clr_int_raw : R/WTC/SS; bitpos: [12]; default: 0;
         *  Raw status bit: The raw interrupt status of the interrupt triggered when event_f0
         *  clears.
         */
        uint32_t fault0_clr_int_raw:1;
        /** fault1_clr_int_raw : R/WTC/SS; bitpos: [13]; default: 0;
         *  Raw status bit: The raw interrupt status of the interrupt triggered when event_f1
         *  clears.
         */
        uint32_t fault1_clr_int_raw:1;
        /** fault2_clr_int_raw : R/WTC/SS; bitpos: [14]; default: 0;
         *  Raw status bit: The raw interrupt status of the interrupt triggered when event_f2
         *  clears.
         */
        uint32_t fault2_clr_int_raw:1;
        /** cmpr0_tea_int_raw : R/WTC/SS; bitpos: [15]; default: 0;
         *  Raw status bit: The raw interrupt status of the interrupt triggered by a PWM
         *  operator 0 TEA event
         */
        uint32_t cmpr0_tea_int_raw:1;
        /** cmpr1_tea_int_raw : R/WTC/SS; bitpos: [16]; default: 0;
         *  Raw status bit: The raw interrupt status of the interrupt triggered by a PWM
         *  operator 1 TEA event
         */
        uint32_t cmpr1_tea_int_raw:1;
        /** cmpr2_tea_int_raw : R/WTC/SS; bitpos: [17]; default: 0;
         *  Raw status bit: The raw interrupt status of the interrupt triggered by a PWM
         *  operator 2 TEA event
         */
        uint32_t cmpr2_tea_int_raw:1;
        /** cmpr0_teb_int_raw : R/WTC/SS; bitpos: [18]; default: 0;
         *  Raw status bit: The raw interrupt status of the interrupt triggered by a PWM
         *  operator 0 TEB event
         */
        uint32_t cmpr0_teb_int_raw:1;
        /** cmpr1_teb_int_raw : R/WTC/SS; bitpos: [19]; default: 0;
         *  Raw status bit: The raw interrupt status of the interrupt triggered by a PWM
         *  operator 1 TEB event
         */
        uint32_t cmpr1_teb_int_raw:1;
        /** cmpr2_teb_int_raw : R/WTC/SS; bitpos: [20]; default: 0;
         *  Raw status bit: The raw interrupt status of the interrupt triggered by a PWM
         *  operator 2 TEB event
         */
        uint32_t cmpr2_teb_int_raw:1;
        /** tz0_cbc_int_raw : R/WTC/SS; bitpos: [21]; default: 0;
         *  Raw status bit: The raw interrupt status of the interrupt triggered by a
         *  cycle-by-cycle mode action on PWM0.
         */
        uint32_t tz0_cbc_int_raw:1;
        /** tz1_cbc_int_raw : R/WTC/SS; bitpos: [22]; default: 0;
         *  Raw status bit: The raw interrupt status of the interrupt triggered by a
         *  cycle-by-cycle mode action on PWM1.
         */
        uint32_t tz1_cbc_int_raw:1;
        /** tz2_cbc_int_raw : R/WTC/SS; bitpos: [23]; default: 0;
         *  Raw status bit: The raw interrupt status of the interrupt triggered by a
         *  cycle-by-cycle mode action on PWM2.
         */
        uint32_t tz2_cbc_int_raw:1;
        /** tz0_ost_int_raw : R/WTC/SS; bitpos: [24]; default: 0;
         *  Raw status bit: The raw interrupt status of the interrupt triggered by a one-shot
         *  mode action on PWM0.
         */
        uint32_t tz0_ost_int_raw:1;
        /** tz1_ost_int_raw : R/WTC/SS; bitpos: [25]; default: 0;
         *  Raw status bit: The raw interrupt status of the interrupt triggered by a one-shot
         *  mode action on PWM1.
         */
        uint32_t tz1_ost_int_raw:1;
        /** tz2_ost_int_raw : R/WTC/SS; bitpos: [26]; default: 0;
         *  Raw status bit: The raw interrupt status of the interrupt triggered by a one-shot
         *  mode action on PWM2.
         */
        uint32_t tz2_ost_int_raw:1;
        /** cap0_int_raw : R/WTC/SS; bitpos: [27]; default: 0;
         *  Raw status bit: The raw interrupt status of the interrupt triggered by capture on
         *  CAP0.
         */
        uint32_t cap0_int_raw:1;
        /** cap1_int_raw : R/WTC/SS; bitpos: [28]; default: 0;
         *  Raw status bit: The raw interrupt status of the interrupt triggered by capture on
         *  CAP1.
         */
        uint32_t cap1_int_raw:1;
        /** cap2_int_raw : R/WTC/SS; bitpos: [29]; default: 0;
         *  Raw status bit: The raw interrupt status of the interrupt triggered by capture on
         *  CAP2.
         */
        uint32_t cap2_int_raw:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} mcpwm_int_raw_reg_t;

/** Type of int_st register
 *  Interrupt masked status register
 */
typedef union {
    struct {
        /** timer0_stop_int_st : RO; bitpos: [0]; default: 0;
         *  Masked status bit: The masked interrupt status of the interrupt triggered when the
         *  timer 0 stops.
         */
        uint32_t timer0_stop_int_st:1;
        /** timer1_stop_int_st : RO; bitpos: [1]; default: 0;
         *  Masked status bit: The masked interrupt status of the interrupt triggered when the
         *  timer 1 stops.
         */
        uint32_t timer1_stop_int_st:1;
        /** timer2_stop_int_st : RO; bitpos: [2]; default: 0;
         *  Masked status bit: The masked interrupt status of the interrupt triggered when the
         *  timer 2 stops.
         */
        uint32_t timer2_stop_int_st:1;
        /** timer0_tez_int_st : RO; bitpos: [3]; default: 0;
         *  Masked status bit: The masked interrupt status of the interrupt triggered by a PWM
         *  timer 0 TEZ event.
         */
        uint32_t timer0_tez_int_st:1;
        /** timer1_tez_int_st : RO; bitpos: [4]; default: 0;
         *  Masked status bit: The masked interrupt status of the interrupt triggered by a PWM
         *  timer 1 TEZ event.
         */
        uint32_t timer1_tez_int_st:1;
        /** timer2_tez_int_st : RO; bitpos: [5]; default: 0;
         *  Masked status bit: The masked interrupt status of the interrupt triggered by a PWM
         *  timer 2 TEZ event.
         */
        uint32_t timer2_tez_int_st:1;
        /** timer0_tep_int_st : RO; bitpos: [6]; default: 0;
         *  Masked status bit: The masked interrupt status of the interrupt triggered by a PWM
         *  timer 0 TEP event.
         */
        uint32_t timer0_tep_int_st:1;
        /** timer1_tep_int_st : RO; bitpos: [7]; default: 0;
         *  Masked status bit: The masked interrupt status of the interrupt triggered by a PWM
         *  timer 1 TEP event.
         */
        uint32_t timer1_tep_int_st:1;
        /** timer2_tep_int_st : RO; bitpos: [8]; default: 0;
         *  Masked status bit: The masked interrupt status of the interrupt triggered by a PWM
         *  timer 2 TEP event.
         */
        uint32_t timer2_tep_int_st:1;
        /** fault0_int_st : RO; bitpos: [9]; default: 0;
         *  Masked status bit: The masked interrupt status of the interrupt triggered when
         *  event_f0 starts.
         */
        uint32_t fault0_int_st:1;
        /** fault1_int_st : RO; bitpos: [10]; default: 0;
         *  Masked status bit: The masked interrupt status of the interrupt triggered when
         *  event_f1 starts.
         */
        uint32_t fault1_int_st:1;
        /** fault2_int_st : RO; bitpos: [11]; default: 0;
         *  Masked status bit: The masked interrupt status of the interrupt triggered when
         *  event_f2 starts.
         */
        uint32_t fault2_int_st:1;
        /** fault0_clr_int_st : RO; bitpos: [12]; default: 0;
         *  Masked status bit: The masked interrupt status of the interrupt triggered when
         *  event_f0 clears.
         */
        uint32_t fault0_clr_int_st:1;
        /** fault1_clr_int_st : RO; bitpos: [13]; default: 0;
         *  Masked status bit: The masked interrupt status of the interrupt triggered when
         *  event_f1 clears.
         */
        uint32_t fault1_clr_int_st:1;
        /** fault2_clr_int_st : RO; bitpos: [14]; default: 0;
         *  Masked status bit: The masked interrupt status of the interrupt triggered when
         *  event_f2 clears.
         */
        uint32_t fault2_clr_int_st:1;
        /** cmpr0_tea_int_st : RO; bitpos: [15]; default: 0;
         *  Masked status bit: The masked interrupt status of the interrupt triggered by a PWM
         *  operator 0 TEA event
         */
        uint32_t cmpr0_tea_int_st:1;
        /** cmpr1_tea_int_st : RO; bitpos: [16]; default: 0;
         *  Masked status bit: The masked interrupt status of the interrupt triggered by a PWM
         *  operator 1 TEA event
         */
        uint32_t cmpr1_tea_int_st:1;
        /** cmpr2_tea_int_st : RO; bitpos: [17]; default: 0;
         *  Masked status bit: The masked interrupt status of the interrupt triggered by a PWM
         *  operator 2 TEA event
         */
        uint32_t cmpr2_tea_int_st:1;
        /** cmpr0_teb_int_st : RO; bitpos: [18]; default: 0;
         *  Masked status bit: The masked interrupt status of the interrupt triggered by a PWM
         *  operator 0 TEB event
         */
        uint32_t cmpr0_teb_int_st:1;
        /** cmpr1_teb_int_st : RO; bitpos: [19]; default: 0;
         *  Masked status bit: The masked interrupt status of the interrupt triggered by a PWM
         *  operator 1 TEB event
         */
        uint32_t cmpr1_teb_int_st:1;
        /** cmpr2_teb_int_st : RO; bitpos: [20]; default: 0;
         *  Masked status bit: The masked interrupt status of the interrupt triggered by a PWM
         *  operator 2 TEB event
         */
        uint32_t cmpr2_teb_int_st:1;
        /** tz0_cbc_int_st : RO; bitpos: [21]; default: 0;
         *  Masked status bit: The masked interrupt status of the interrupt triggered by a
         *  cycle-by-cycle mode action on PWM0.
         */
        uint32_t tz0_cbc_int_st:1;
        /** tz1_cbc_int_st : RO; bitpos: [22]; default: 0;
         *  Masked status bit: The masked interrupt status of the interrupt triggered by a
         *  cycle-by-cycle mode action on PWM1.
         */
        uint32_t tz1_cbc_int_st:1;
        /** tz2_cbc_int_st : RO; bitpos: [23]; default: 0;
         *  Masked status bit: The masked interrupt status of the interrupt triggered by a
         *  cycle-by-cycle mode action on PWM2.
         */
        uint32_t tz2_cbc_int_st:1;
        /** tz0_ost_int_st : RO; bitpos: [24]; default: 0;
         *  Masked status bit: The masked interrupt status of the interrupt triggered by a
         *  one-shot mode action on PWM0.
         */
        uint32_t tz0_ost_int_st:1;
        /** tz1_ost_int_st : RO; bitpos: [25]; default: 0;
         *  Masked status bit: The masked interrupt status of the interrupt triggered by a
         *  one-shot mode action on PWM1.
         */
        uint32_t tz1_ost_int_st:1;
        /** tz2_ost_int_st : RO; bitpos: [26]; default: 0;
         *  Masked status bit: The masked interrupt status of the interrupt triggered by a
         *  one-shot mode action on PWM2.
         */
        uint32_t tz2_ost_int_st:1;
        /** cap0_int_st : RO; bitpos: [27]; default: 0;
         *  Masked status bit: The masked interrupt status of the interrupt triggered by
         *  capture on CAP0.
         */
        uint32_t cap0_int_st:1;
        /** cap1_int_st : RO; bitpos: [28]; default: 0;
         *  Masked status bit: The masked interrupt status of the interrupt triggered by
         *  capture on CAP1.
         */
        uint32_t cap1_int_st:1;
        /** cap2_int_st : RO; bitpos: [29]; default: 0;
         *  Masked status bit: The masked interrupt status of the interrupt triggered by
         *  capture on CAP2.
         */
        uint32_t cap2_int_st:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} mcpwm_int_st_reg_t;

/** Type of int_clr register
 *  Interrupt clear register
 */
typedef union {
    struct {
        /** timer0_stop_int_clr : WT; bitpos: [0]; default: 0;
         *  Clear bit: Write 1 to clear the interrupt triggered when the timer 0 stops.
         */
        uint32_t timer0_stop_int_clr:1;
        /** timer1_stop_int_clr : WT; bitpos: [1]; default: 0;
         *  Clear bit: Write 1 to clear the interrupt triggered when the timer 1 stops.
         */
        uint32_t timer1_stop_int_clr:1;
        /** timer2_stop_int_clr : WT; bitpos: [2]; default: 0;
         *  Clear bit: Write 1 to clear the interrupt triggered when the timer 2 stops.
         */
        uint32_t timer2_stop_int_clr:1;
        /** timer0_tez_int_clr : WT; bitpos: [3]; default: 0;
         *  Clear bit: Write 1 to clear the interrupt triggered by a PWM timer 0 TEZ event.
         */
        uint32_t timer0_tez_int_clr:1;
        /** timer1_tez_int_clr : WT; bitpos: [4]; default: 0;
         *  Clear bit: Write 1 to clear the interrupt triggered by a PWM timer 1 TEZ event.
         */
        uint32_t timer1_tez_int_clr:1;
        /** timer2_tez_int_clr : WT; bitpos: [5]; default: 0;
         *  Clear bit: Write 1 to clear the interrupt triggered by a PWM timer 2 TEZ event.
         */
        uint32_t timer2_tez_int_clr:1;
        /** timer0_tep_int_clr : WT; bitpos: [6]; default: 0;
         *  Clear bit: Write 1 to clear the interrupt triggered by a PWM timer 0 TEP event.
         */
        uint32_t timer0_tep_int_clr:1;
        /** timer1_tep_int_clr : WT; bitpos: [7]; default: 0;
         *  Clear bit: Write 1 to clear the interrupt triggered by a PWM timer 1 TEP event.
         */
        uint32_t timer1_tep_int_clr:1;
        /** timer2_tep_int_clr : WT; bitpos: [8]; default: 0;
         *  Clear bit: Write 1 to clear the interrupt triggered by a PWM timer 2 TEP event.
         */
        uint32_t timer2_tep_int_clr:1;
        /** fault0_int_clr : WT; bitpos: [9]; default: 0;
         *  Clear bit: Write 1 to clear the interrupt triggered when event_f0 starts.
         */
        uint32_t fault0_int_clr:1;
        /** fault1_int_clr : WT; bitpos: [10]; default: 0;
         *  Clear bit: Write 1 to clear the interrupt triggered when event_f1 starts.
         */
        uint32_t fault1_int_clr:1;
        /** fault2_int_clr : WT; bitpos: [11]; default: 0;
         *  Clear bit: Write 1 to clear the interrupt triggered when event_f2 starts.
         */
        uint32_t fault2_int_clr:1;
        /** fault0_clr_int_clr : WT; bitpos: [12]; default: 0;
         *  Clear bit: Write 1 to clear the interrupt triggered when event_f0 clears.
         */
        uint32_t fault0_clr_int_clr:1;
        /** fault1_clr_int_clr : WT; bitpos: [13]; default: 0;
         *  Clear bit: Write 1 to clear the interrupt triggered when event_f1 clears.
         */
        uint32_t fault1_clr_int_clr:1;
        /** fault2_clr_int_clr : WT; bitpos: [14]; default: 0;
         *  Clear bit: Write 1 to clear the interrupt triggered when event_f2 clears.
         */
        uint32_t fault2_clr_int_clr:1;
        /** cmpr0_tea_int_clr : WT; bitpos: [15]; default: 0;
         *  Clear bit: Write 1 to clear the interrupt triggered by a PWM operator 0 TEA event
         */
        uint32_t cmpr0_tea_int_clr:1;
        /** cmpr1_tea_int_clr : WT; bitpos: [16]; default: 0;
         *  Clear bit: Write 1 to clear the interrupt triggered by a PWM operator 1 TEA event
         */
        uint32_t cmpr1_tea_int_clr:1;
        /** cmpr2_tea_int_clr : WT; bitpos: [17]; default: 0;
         *  Clear bit: Write 1 to clear the interrupt triggered by a PWM operator 2 TEA event
         */
        uint32_t cmpr2_tea_int_clr:1;
        /** cmpr0_teb_int_clr : WT; bitpos: [18]; default: 0;
         *  Clear bit: Write 1 to clear the interrupt triggered by a PWM operator 0 TEB event
         */
        uint32_t cmpr0_teb_int_clr:1;
        /** cmpr1_teb_int_clr : WT; bitpos: [19]; default: 0;
         *  Clear bit: Write 1 to clear the interrupt triggered by a PWM operator 1 TEB event
         */
        uint32_t cmpr1_teb_int_clr:1;
        /** cmpr2_teb_int_clr : WT; bitpos: [20]; default: 0;
         *  Clear bit: Write 1 to clear the interrupt triggered by a PWM operator 2 TEB event
         */
        uint32_t cmpr2_teb_int_clr:1;
        /** tz0_cbc_int_clr : WT; bitpos: [21]; default: 0;
         *  Clear bit: Write 1 to clear the interrupt triggered by a cycle-by-cycle mode action
         *  on PWM0.
         */
        uint32_t tz0_cbc_int_clr:1;
        /** tz1_cbc_int_clr : WT; bitpos: [22]; default: 0;
         *  Clear bit: Write 1 to clear the interrupt triggered by a cycle-by-cycle mode action
         *  on PWM1.
         */
        uint32_t tz1_cbc_int_clr:1;
        /** tz2_cbc_int_clr : WT; bitpos: [23]; default: 0;
         *  Clear bit: Write 1 to clear the interrupt triggered by a cycle-by-cycle mode action
         *  on PWM2.
         */
        uint32_t tz2_cbc_int_clr:1;
        /** tz0_ost_int_clr : WT; bitpos: [24]; default: 0;
         *  Clear bit: Write 1 to clear the interrupt triggered by a one-shot mode action on
         *  PWM0.
         */
        uint32_t tz0_ost_int_clr:1;
        /** tz1_ost_int_clr : WT; bitpos: [25]; default: 0;
         *  Clear bit: Write 1 to clear the interrupt triggered by a one-shot mode action on
         *  PWM1.
         */
        uint32_t tz1_ost_int_clr:1;
        /** tz2_ost_int_clr : WT; bitpos: [26]; default: 0;
         *  Clear bit: Write 1 to clear the interrupt triggered by a one-shot mode action on
         *  PWM2.
         */
        uint32_t tz2_ost_int_clr:1;
        /** cap0_int_clr : WT; bitpos: [27]; default: 0;
         *  Clear bit: Write 1 to clear the interrupt triggered by capture on CAP0.
         */
        uint32_t cap0_int_clr:1;
        /** cap1_int_clr : WT; bitpos: [28]; default: 0;
         *  Clear bit: Write 1 to clear the interrupt triggered by capture on CAP1.
         */
        uint32_t cap1_int_clr:1;
        /** cap2_int_clr : WT; bitpos: [29]; default: 0;
         *  Clear bit: Write 1 to clear the interrupt triggered by capture on CAP2.
         */
        uint32_t cap2_int_clr:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} mcpwm_int_clr_reg_t;


/** Group: etm */
/** Type of evt_en register
 *  Event enable register
 */
typedef union {
    struct {
        /** evt_timer0_stop_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enable timer0 stop event generate.\\0: Disable\\1:
         *  Enable
         */
        uint32_t evt_timer0_stop_en:1;
        /** evt_timer1_stop_en : R/W; bitpos: [1]; default: 0;
         *  Configures whether or not to enable timer1 stop event generate.\\0: Disable\\1:
         *  Enable
         */
        uint32_t evt_timer1_stop_en:1;
        /** evt_timer2_stop_en : R/W; bitpos: [2]; default: 0;
         *  Configures whether or not to enable timer2 stop event generate.\\0: Disable\\1:
         *  Enable
         */
        uint32_t evt_timer2_stop_en:1;
        /** evt_timer0_tez_en : R/W; bitpos: [3]; default: 0;
         *  Configures whether or not to enable timer0 equal zero event generate.\\0:
         *  Disable\\1: Enable
         */
        uint32_t evt_timer0_tez_en:1;
        /** evt_timer1_tez_en : R/W; bitpos: [4]; default: 0;
         *  Configures whether or not to enable timer1 equal zero event generate.\\0:
         *  Disable\\1: Enable
         */
        uint32_t evt_timer1_tez_en:1;
        /** evt_timer2_tez_en : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to enable timer2 equal zero event generate.\\0:
         *  Disable\\1: Enable
         */
        uint32_t evt_timer2_tez_en:1;
        /** evt_timer0_tep_en : R/W; bitpos: [6]; default: 0;
         *  Configures whether or not to enable timer0 equal period event generate.\\0:
         *  Disable\\1: Enable
         */
        uint32_t evt_timer0_tep_en:1;
        /** evt_timer1_tep_en : R/W; bitpos: [7]; default: 0;
         *  Configures whether or not to enable timer1 equal period event generate.\\0:
         *  Disable\\1: Enable
         */
        uint32_t evt_timer1_tep_en:1;
        /** evt_timer2_tep_en : R/W; bitpos: [8]; default: 0;
         *  Configures whether or not to enable timer2 equal period event generate.\\0:
         *  Disable\\1: Enable
         */
        uint32_t evt_timer2_tep_en:1;
        /** evt_op0_tea_en : R/W; bitpos: [9]; default: 0;
         *  Configures whether or not to enable PWM generator0 timer equal a event
         *  generate.\\0: Disable\\1: Enable
         */
        uint32_t evt_op0_tea_en:1;
        /** evt_op1_tea_en : R/W; bitpos: [10]; default: 0;
         *  Configures whether or not to enable PWM generator1 timer equal a event
         *  generate.\\0: Disable\\1: Enable
         */
        uint32_t evt_op1_tea_en:1;
        /** evt_op2_tea_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable PWM generator2 timer equal a event
         *  generate.\\0: Disable\\1: Enable
         */
        uint32_t evt_op2_tea_en:1;
        /** evt_op0_teb_en : R/W; bitpos: [12]; default: 0;
         *  Configures whether or not to enable PWM generator0 timer equal b event
         *  generate.\\0: Disable\\1: Enable
         */
        uint32_t evt_op0_teb_en:1;
        /** evt_op1_teb_en : R/W; bitpos: [13]; default: 0;
         *  Configures whether or not to enable PWM generator1 timer equal b event
         *  generate.\\0: Disable\\1: Enable
         */
        uint32_t evt_op1_teb_en:1;
        /** evt_op2_teb_en : R/W; bitpos: [14]; default: 0;
         *  Configures whether or not to enable PWM generator2 timer equal b event
         *  generate.\\0: Disable\\1: Enable
         */
        uint32_t evt_op2_teb_en:1;
        /** evt_f0_en : R/W; bitpos: [15]; default: 0;
         *  Configures whether or not to enable fault0 event generate.\\0: Disable\\1: Enable
         */
        uint32_t evt_f0_en:1;
        /** evt_f1_en : R/W; bitpos: [16]; default: 0;
         *  Configures whether or not to enable fault1 event generate.\\0: Disable\\1: Enable
         */
        uint32_t evt_f1_en:1;
        /** evt_f2_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to enable fault2 event generate.\\0: Disable\\1: Enable
         */
        uint32_t evt_f2_en:1;
        /** evt_f0_clr_en : R/W; bitpos: [18]; default: 0;
         *  Configures whether or not to enable fault0 clear event generate.\\0: Disable\\1:
         *  Enable
         */
        uint32_t evt_f0_clr_en:1;
        /** evt_f1_clr_en : R/W; bitpos: [19]; default: 0;
         *  Configures whether or not to enable fault1 clear event generate.\\0: Disable\\1:
         *  Enable
         */
        uint32_t evt_f1_clr_en:1;
        /** evt_f2_clr_en : R/W; bitpos: [20]; default: 0;
         *  Configures whether or not to enable fault2 clear event generate.\\0: Disable\\1:
         *  Enable
         */
        uint32_t evt_f2_clr_en:1;
        /** evt_tz0_cbc_en : R/W; bitpos: [21]; default: 0;
         *  Configures whether or not to enable cycle-by-cycle trip0 event generate.\\0:
         *  Disable\\1: Enable
         */
        uint32_t evt_tz0_cbc_en:1;
        /** evt_tz1_cbc_en : R/W; bitpos: [22]; default: 0;
         *  Configures whether or not to enable cycle-by-cycle trip1 event generate.\\0:
         *  Disable\\1: Enable
         */
        uint32_t evt_tz1_cbc_en:1;
        /** evt_tz2_cbc_en : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to enable cycle-by-cycle trip2 event generate.\\0:
         *  Disable\\1: Enable
         */
        uint32_t evt_tz2_cbc_en:1;
        /** evt_tz0_ost_en : R/W; bitpos: [24]; default: 0;
         *  Configures whether or not to enable one-shot trip0 event generate.\\0: Disable\\1:
         *  Enable
         */
        uint32_t evt_tz0_ost_en:1;
        /** evt_tz1_ost_en : R/W; bitpos: [25]; default: 0;
         *  Configures whether or not to enable one-shot trip1 event generate.\\0: Disable\\1:
         *  Enable
         */
        uint32_t evt_tz1_ost_en:1;
        /** evt_tz2_ost_en : R/W; bitpos: [26]; default: 0;
         *  Configures whether or not to enable one-shot trip2 event generate.\\0: Disable\\1:
         *  Enable
         */
        uint32_t evt_tz2_ost_en:1;
        /** evt_cap0_en : R/W; bitpos: [27]; default: 0;
         *  Configures whether or not to enable capture0 event generate.\\0: Disable\\1: Enable
         */
        uint32_t evt_cap0_en:1;
        /** evt_cap1_en : R/W; bitpos: [28]; default: 0;
         *  Configures whether or not to enable capture1 event generate.\\0: Disable\\1: Enable
         */
        uint32_t evt_cap1_en:1;
        /** evt_cap2_en : R/W; bitpos: [29]; default: 0;
         *  Configures whether or not to enable capture2 event generate.\\0: Disable\\1: Enable
         */
        uint32_t evt_cap2_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} mcpwm_evt_en_reg_t;

/** Type of task_en register
 *  Task enable register
 */
typedef union {
    struct {
        /** task_cmpr0_a_up_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enable PWM generator0 timer stamp A's shadow register
         *  update task receive.\\0: Disable\\1: Enable
         */
        uint32_t task_cmpr0_a_up_en:1;
        /** task_cmpr1_a_up_en : R/W; bitpos: [1]; default: 0;
         *  Configures whether or not to enable PWM generator1 timer stamp A's shadow register
         *  update task receive.\\0: Disable\\1: Enable
         */
        uint32_t task_cmpr1_a_up_en:1;
        /** task_cmpr2_a_up_en : R/W; bitpos: [2]; default: 0;
         *  Configures whether or not to enable PWM generator2 timer stamp A's shadow register
         *  update task receive.\\0: Disable\\1: Enable
         */
        uint32_t task_cmpr2_a_up_en:1;
        /** task_cmpr0_b_up_en : R/W; bitpos: [3]; default: 0;
         *  Configures whether or not to enable PWM generator0 timer stamp B's shadow register
         *  update task receive.\\0: Disable\\1: Enable
         */
        uint32_t task_cmpr0_b_up_en:1;
        /** task_cmpr1_b_up_en : R/W; bitpos: [4]; default: 0;
         *  Configures whether or not to enable PWM generator1 timer stamp B's shadow register
         *  update task receive.\\0: Disable\\1: Enable
         */
        uint32_t task_cmpr1_b_up_en:1;
        /** task_cmpr2_b_up_en : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to enable PWM generator2 timer stamp B's shadow register
         *  update task receive.\\0: Disable\\1: Enable
         */
        uint32_t task_cmpr2_b_up_en:1;
        /** task_gen_stop_en : R/W; bitpos: [6]; default: 0;
         *  Configures whether or not to enable all PWM generate stop task receive.\\0:
         *  Disable\\1: Enable
         */
        uint32_t task_gen_stop_en:1;
        /** task_timer0_sync_en : R/W; bitpos: [7]; default: 0;
         *  Configures whether or not to enable timer0 sync task receive.\\0: Disable\\1: Enable
         */
        uint32_t task_timer0_sync_en:1;
        /** task_timer1_sync_en : R/W; bitpos: [8]; default: 0;
         *  Configures whether or not to enable timer1 sync task receive.\\0: Disable\\1: Enable
         */
        uint32_t task_timer1_sync_en:1;
        /** task_timer2_sync_en : R/W; bitpos: [9]; default: 0;
         *  Configures whether or not to enable timer2 sync task receive.\\0: Disable\\1: Enable
         */
        uint32_t task_timer2_sync_en:1;
        /** task_timer0_period_up_en : R/W; bitpos: [10]; default: 0;
         *  Configures whether or not to enable timer0 period update task receive.\\0:
         *  Disable\\1: Enable
         */
        uint32_t task_timer0_period_up_en:1;
        /** task_timer1_period_up_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable timer1 period update task receive.\\0:
         *  Disable\\1: Enable
         */
        uint32_t task_timer1_period_up_en:1;
        /** task_timer2_period_up_en : R/W; bitpos: [12]; default: 0;
         *  Configures whether or not to enable timer2 period update task receive.\\0:
         *  Disable\\1: Enable
         */
        uint32_t task_timer2_period_up_en:1;
        /** task_tz0_ost_en : R/W; bitpos: [13]; default: 0;
         *  Configures whether or not to enable one shot trip0 task receive.\\0: Disable\\1:
         *  Enable
         */
        uint32_t task_tz0_ost_en:1;
        /** task_tz1_ost_en : R/W; bitpos: [14]; default: 0;
         *  Configures whether or not to enable one shot trip1 task receive.\\0: Disable\\1:
         *  Enable
         */
        uint32_t task_tz1_ost_en:1;
        /** task_tz2_ost_en : R/W; bitpos: [15]; default: 0;
         *  Configures whether or not to enable one shot trip2 task receive.\\0: Disable\\1:
         *  Enable
         */
        uint32_t task_tz2_ost_en:1;
        /** task_clr0_ost_en : R/W; bitpos: [16]; default: 0;
         *  Configures whether or not to enable one shot trip0 clear task receive.\\0:
         *  Disable\\1: Enable
         */
        uint32_t task_clr0_ost_en:1;
        /** task_clr1_ost_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to enable one shot trip1 clear task receive.\\0:
         *  Disable\\1: Enable
         */
        uint32_t task_clr1_ost_en:1;
        /** task_clr2_ost_en : R/W; bitpos: [18]; default: 0;
         *  Configures whether or not to enable one shot trip2 clear task receive.\\0:
         *  Disable\\1: Enable
         */
        uint32_t task_clr2_ost_en:1;
        /** task_cap0_en : R/W; bitpos: [19]; default: 0;
         *  Configures whether or not to enable capture0 task receive.\\0: Disable\\1: Enable
         */
        uint32_t task_cap0_en:1;
        /** task_cap1_en : R/W; bitpos: [20]; default: 0;
         *  Configures whether or not to enable capture1 task receive.\\0: Disable\\1: Enable
         */
        uint32_t task_cap1_en:1;
        /** task_cap2_en : R/W; bitpos: [21]; default: 0;
         *  Configures whether or not to enable capture2 task receive.\\0: Disable\\1: Enable
         */
        uint32_t task_cap2_en:1;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} mcpwm_task_en_reg_t;

/** Type of evt_en2 register
 *  Event enable register2
 */
typedef union {
    struct {
        /** evt_op0_tee1_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enable PWM generator0 timer equal OP0_TSTMP_E1_REG
         *  event generate.\\0: Disable\\1: Enable
         */
        uint32_t evt_op0_tee1_en:1;
        /** evt_op1_tee1_en : R/W; bitpos: [1]; default: 0;
         *  Configures whether or not to enable PWM generator1 timer equal OP1_TSTMP_E1_REG
         *  event generate.\\0: Disable\\1: Enable
         */
        uint32_t evt_op1_tee1_en:1;
        /** evt_op2_tee1_en : R/W; bitpos: [2]; default: 0;
         *  Configures whether or not to enable PWM generator2 timer equal OP2_TSTMP_E1_REG
         *  event generate.\\0: Disable\\1: Enable
         */
        uint32_t evt_op2_tee1_en:1;
        /** evt_op0_tee2_en : R/W; bitpos: [3]; default: 0;
         *  Configures whether or not to enable PWM generator0 timer equal OP0_TSTMP_E2_REG
         *  event generate.\\0: Disable\\1: Enable
         */
        uint32_t evt_op0_tee2_en:1;
        /** evt_op1_tee2_en : R/W; bitpos: [4]; default: 0;
         *  Configures whether or not to enable PWM generator1 timer equal OP1_TSTMP_E2_REG
         *  event generate.\\0: Disable\\1: Enable
         */
        uint32_t evt_op1_tee2_en:1;
        /** evt_op2_tee2_en : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to enable PWM generator2 timer equal OP2_TSTMP_E2_REG
         *  event generate.\\0: Disable\\1: Enable
         */
        uint32_t evt_op2_tee2_en:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} mcpwm_evt_en2_reg_t;


/** Group: tstmp */
/** Type of opn_tstmp_e1 register
 *  Generatorn timer stamp E1 value register
 */
typedef union {
    struct {
        /** opn_tstmp_e1 : R/W; bitpos: [15:0]; default: 0;
         *  Configures generatorn timer stamp E1 value register
         */
        uint32_t opn_tstmp_e1:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} mcpwm_opn_tstmp_e1_reg_t;

/** Type of opn_tstmp_e2 register
 *  Generatorn timer stamp E2 value register
 */
typedef union {
    struct {
        /** opn_tstmp_e2 : R/W; bitpos: [15:0]; default: 0;
         *  Configures generatorn timer stamp E2 value register
         */
        uint32_t opn_tstmp_e2:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} mcpwm_opn_tstmp_e2_reg_t;

/** Type of clk register
 *  Global configuration register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to open register clock gate.\\0: Open the clock gate only
         *  when application writes registers\\1: Force open the clock gate for register
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} mcpwm_clk_reg_t;


/** Group: Version register */
/** Type of version register
 *  Version register.
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 35725968;
         *  Configures the version.
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} mcpwm_version_reg_t;


typedef struct mcpwm_dev_t {
    volatile mcpwm_clk_cfg_reg_t clk_cfg;
    volatile mcpwm_timern_cfg0_reg_t timer0_cfg0;
    volatile mcpwm_timern_cfg1_reg_t timer0_cfg1;
    volatile mcpwm_timern_sync_reg_t timer0_sync;
    volatile mcpwm_timern_status_reg_t timer0_status;
    volatile mcpwm_timern_cfg0_reg_t timer1_cfg0;
    volatile mcpwm_timern_cfg1_reg_t timer1_cfg1;
    volatile mcpwm_timern_sync_reg_t timer1_sync;
    volatile mcpwm_timern_status_reg_t timer1_status;
    volatile mcpwm_timern_cfg0_reg_t timer2_cfg0;
    volatile mcpwm_timern_cfg1_reg_t timer2_cfg1;
    volatile mcpwm_timern_sync_reg_t timer2_sync;
    volatile mcpwm_timern_status_reg_t timer2_status;
    volatile mcpwm_timer_synci_cfg_reg_t timer_synci_cfg;
    volatile mcpwm_operator_timersel_reg_t operator_timersel;
    volatile mcpwm_genn_stmp_cfg_reg_t gen0_stmp_cfg;
    volatile mcpwm_genn_tstmp_a_reg_t gen0_tstmp_a;
    volatile mcpwm_genn_tstmp_b_reg_t gen0_tstmp_b;
    volatile mcpwm_genn_cfg0_reg_t gen0_cfg0;
    volatile mcpwm_genn_force_reg_t gen0_force;
    volatile mcpwm_genn_a_reg_t gen0_a;
    volatile mcpwm_genn_b_reg_t gen0_b;
    volatile mcpwm_dtn_cfg_reg_t dt0_cfg;
    volatile mcpwm_dtn_fed_cfg_reg_t dt0_fed_cfg;
    volatile mcpwm_dtn_red_cfg_reg_t dt0_red_cfg;
    volatile mcpwm_carriern_cfg_reg_t carrier0_cfg;
    volatile mcpwm_fhn_cfg0_reg_t fh0_cfg0;
    volatile mcpwm_fhn_cfg1_reg_t fh0_cfg1;
    volatile mcpwm_fhn_status_reg_t fh0_status;
    volatile mcpwm_genn_stmp_cfg_reg_t gen1_stmp_cfg;
    volatile mcpwm_genn_tstmp_a_reg_t gen1_tstmp_a;
    volatile mcpwm_genn_tstmp_b_reg_t gen1_tstmp_b;
    volatile mcpwm_genn_cfg0_reg_t gen1_cfg0;
    volatile mcpwm_genn_force_reg_t gen1_force;
    volatile mcpwm_genn_a_reg_t gen1_a;
    volatile mcpwm_genn_b_reg_t gen1_b;
    volatile mcpwm_dtn_cfg_reg_t dt1_cfg;
    volatile mcpwm_dtn_fed_cfg_reg_t dt1_fed_cfg;
    volatile mcpwm_dtn_red_cfg_reg_t dt1_red_cfg;
    volatile mcpwm_carriern_cfg_reg_t carrier1_cfg;
    volatile mcpwm_fhn_cfg0_reg_t fh1_cfg0;
    volatile mcpwm_fhn_cfg1_reg_t fh1_cfg1;
    volatile mcpwm_fhn_status_reg_t fh1_status;
    volatile mcpwm_genn_stmp_cfg_reg_t gen2_stmp_cfg;
    volatile mcpwm_genn_tstmp_a_reg_t gen2_tstmp_a;
    volatile mcpwm_genn_tstmp_b_reg_t gen2_tstmp_b;
    volatile mcpwm_genn_cfg0_reg_t gen2_cfg0;
    volatile mcpwm_genn_force_reg_t gen2_force;
    volatile mcpwm_genn_a_reg_t gen2_a;
    volatile mcpwm_genn_b_reg_t gen2_b;
    volatile mcpwm_dtn_cfg_reg_t dt2_cfg;
    volatile mcpwm_dtn_fed_cfg_reg_t dt2_fed_cfg;
    volatile mcpwm_dtn_red_cfg_reg_t dt2_red_cfg;
    volatile mcpwm_carriern_cfg_reg_t carrier2_cfg;
    volatile mcpwm_fhn_cfg0_reg_t fh2_cfg0;
    volatile mcpwm_fhn_cfg1_reg_t fh2_cfg1;
    volatile mcpwm_fhn_status_reg_t fh2_status;
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
    volatile mcpwm_evt_en2_reg_t evt_en2;
    volatile mcpwm_opn_tstmp_e1_reg_t op0_tstmp_e1;
    volatile mcpwm_opn_tstmp_e2_reg_t op0_tstmp_e2;
    volatile mcpwm_opn_tstmp_e1_reg_t op1_tstmp_e1;
    volatile mcpwm_opn_tstmp_e2_reg_t op1_tstmp_e2;
    volatile mcpwm_opn_tstmp_e1_reg_t op2_tstmp_e1;
    volatile mcpwm_opn_tstmp_e2_reg_t op2_tstmp_e2;
    volatile mcpwm_clk_reg_t clk;
    volatile mcpwm_version_reg_t version;
} mcpwm_dev_t;

extern mcpwm_dev_t MCPWM;

#ifndef __cplusplus
_Static_assert(sizeof(mcpwm_dev_t) == 0x14c, "Invalid size of mcpwm_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
