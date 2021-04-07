// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct mcpwm_dev_s {
    union {
        struct {
            uint32_t prescale : 8; /*Period of PWM_clk = 6.25ns * (PWM_CLK_PRESCALE + 1)*/
            uint32_t reserved8 : 24;
        };
        uint32_t val;
    } clk_cfg;
    struct {
        union {
            struct {
                uint32_t prescale : 8; /*period of PT0_clk = Period of PWM_clk * (PWM_TIMER0_PRESCALE + 1)*/
                uint32_t period : 16;  /*period shadow reg of PWM timer0*/
                uint32_t upmethod : 2; /*Update method for active reg of PWM timer0 period  0: immediate  1: TEZ  2: sync  3: TEZ | sync. TEZ here and below means timer equal zero event*/
                uint32_t reserved26 : 6;
            };
            uint32_t val;
        } period;
        union {
            struct {
                uint32_t start : 3; /*PWM timer0 start and stop control. 0: stop @ TEZ  1: stop @ TEP  2: free run  3: start and stop @ next TEZ  4: start and stop @ next TEP. TEP here and below means timer equal period event*/
                uint32_t mode : 2;  /*PWM timer0 working mode  0: freeze  1: increase mod  2: decrease mod  3: up-down mod*/
                uint32_t reserved5 : 27;
            };
            uint32_t val;
        } mode;
        union {
            struct {
                uint32_t in_en : 1;        /*when set timer reload with phase on sync input event is enabled*/
                uint32_t sync_sw : 1;      /*write the negate value will trigger a software sync*/
                uint32_t out_sel : 2;      /*PWM timer0 synco selection  0: synci  1: TEZ  2: TEP  else 0*/
                uint32_t timer_phase : 16; /*phase for timer reload on sync event*/
                uint32_t phase_direct : 1; /*counter direction to apply on sync event*/
                uint32_t reserved21 : 11;
            };
            uint32_t val;
        } sync;
        union {
            struct {
                uint32_t value : 16;    /*current PWM timer0 counter value*/
                uint32_t direction : 1; /*current PWM timer0 counter direction  0: increment 1: decrement*/
                uint32_t reserved17 : 15;
            };
            uint32_t val;
        } status;
    } timer[3];

    union {
        struct {
            uint32_t t0_in_sel : 3;   /*select sync input for PWM timer0  1: PWM timer0 synco  2: PWM timer1 synco  3: PWM timer2 synco  4: SYNC0 from GPIO matrix  5: SYNC1 from GPIO matrix  6: SYNC2 from GPIO matrix  else: none*/
            uint32_t t1_in_sel : 3;   /*select sync input for PWM timer1  1: PWM timer0 synco  2: PWM timer1 synco  3: PWM timer2 synco  4: SYNC0 from GPIO matrix  5: SYNC1 from GPIO matrix  6: SYNC2 from GPIO matrix  else: none*/
            uint32_t t2_in_sel : 3;   /*select sync input for PWM timer2  1: PWM timer0 synco  2: PWM timer1 synco  3: PWM timer2 synco  4: SYNC0 from GPIO matrix  5: SYNC1 from GPIO matrix  6: SYNC2 from GPIO matrix  else: none*/
            uint32_t ext_in0_inv : 1; /*invert SYNC0 from GPIO matrix*/
            uint32_t ext_in1_inv : 1; /*invert SYNC1 from GPIO matrix*/
            uint32_t ext_in2_inv : 1; /*invert SYNC2 from GPIO matrix*/
            uint32_t reserved12 : 20;
        };
        uint32_t val;
    } timer_synci_cfg;
    union {
        struct {
            uint32_t operator0_sel : 2; /*Select which PWM timer's is the timing reference for PWM operator0  0: timer0  1: timer1  2: timer2*/
            uint32_t operator1_sel : 2; /*Select which PWM timer's is the timing reference for PWM operator1  0: timer0  1: timer1  2: timer2*/
            uint32_t operator2_sel : 2; /*Select which PWM timer's is the timing reference for PWM operator2  0: timer0  1: timer1  2: timer2*/
            uint32_t reserved6 : 26;
        };
        uint32_t val;
    } timer_sel;

    struct {
        union {
            struct {
                uint32_t a_upmethod : 4;  /*Update method for PWM compare0 A's active reg. 0: immediate  bit0: TEZ  bit1: TEP  bit2: sync  bit3: freeze*/
                uint32_t b_upmethod : 4;  /*Update method for PWM compare0 B's active reg. 0: immediate  bit0: TEZ  bit1: TEP  bit2: sync  bit3: freeze*/
                uint32_t a_shdw_full : 1; /*Set and reset by hardware. If set  PWM compare0 A's shadow reg is filled and waiting to be transferred to A's active reg. If cleared  A's active reg has been updated with shadow reg latest value*/
                uint32_t b_shdw_full : 1; /*Set and reset by hardware. If set  PWM compare0 B's shadow reg is filled and waiting to be transferred to B's active reg. If cleared  B's active reg has been updated with shadow reg latest value*/
                uint32_t reserved10 : 22;
            };
            uint32_t val;
        } cmpr_cfg;
        union {
            struct {
                uint32_t cmpr_val : 16; /*PWM compare0 A's shadow reg*/
                uint32_t reserved16 : 16;
            };
            uint32_t val;
        } cmpr_value[2];
        union {
            struct {
                uint32_t upmethod : 4; /*Update method for PWM generate0's active reg of configuration. 0: immediate  bit0: TEZ  bit1: TEP  bit2: sync. bit3: freeze*/
                uint32_t t0_sel : 3;   /*Source selection for PWM generate0 event_t0  take effect immediately  0: fault_event0  1: fault_event1  2: fault_event2  3: sync_taken  4: none*/
                uint32_t t1_sel : 3;   /*Source selection for PWM generate0 event_t1  take effect immediately  0: fault_event0  1: fault_event1  2: fault_event2  3: sync_taken  4: none*/
                uint32_t reserved10 : 22;
            };
            uint32_t val;
        } gen_cfg0;
        union {
            struct {
                uint32_t cntu_force_upmethod : 6; /*Update method for continuous software force of PWM generate0. 0: immediate  bit0: TEZ  bit1: TEP  bit2: TEA  bit3: TEB  bit4: sync  bit5: freeze. (TEA/B here and below means timer equals A/B event)*/
                uint32_t a_cntuforce_mode : 2;    /*Continuous software force mode for PWM0A. 0: disabled  1: low  2: high  3: disabled*/
                uint32_t b_cntuforce_mode : 2;    /*Continuous software force mode for PWM0B. 0: disabled  1: low  2: high  3: disabled*/
                uint32_t a_nciforce : 1;          /*non-continuous immediate software force trigger for PWM0A  a toggle will trigger a force event*/
                uint32_t a_nciforce_mode : 2;     /*non-continuous immediate software force mode for PWM0A  0: disabled  1: low  2: high  3: disabled*/
                uint32_t b_nciforce : 1;          /*non-continuous immediate software force trigger for PWM0B  a toggle will trigger a force event*/
                uint32_t b_nciforce_mode : 2;     /*non-continuous immediate software force mode for PWM0B  0: disabled  1: low  2: high  3: disabled*/
                uint32_t reserved16 : 16;
            };
            uint32_t val;
        } gen_force;
        union {
            struct {
                uint32_t utez : 2; /*Action on PWM0A triggered by event TEZ when timer increasing*/
                uint32_t utep : 2; /*Action on PWM0A triggered by event TEP when timer increasing*/
                uint32_t utea : 2; /*Action on PWM0A triggered by event TEA when timer increasing*/
                uint32_t uteb : 2; /*Action on PWM0A triggered by event TEB when timer increasing*/
                uint32_t ut0 : 2;  /*Action on PWM0A triggered by event_t0 when timer increasing*/
                uint32_t ut1 : 2;  /*Action on PWM0A triggered by event_t1 when timer increasing*/
                uint32_t dtez : 2; /*Action on PWM0A triggered by event TEZ when timer decreasing*/
                uint32_t dtep : 2; /*Action on PWM0A triggered by event TEP when timer decreasing*/
                uint32_t dtea : 2; /*Action on PWM0A triggered by event TEA when timer decreasing*/
                uint32_t dteb : 2; /*Action on PWM0A triggered by event TEB when timer decreasing*/
                uint32_t dt0 : 2;  /*Action on PWM0A triggered by event_t0 when timer decreasing*/
                uint32_t dt1 : 2;  /*Action on PWM0A triggered by event_t1 when timer decreasing. 0: no change  1: low  2: high  3: toggle*/
                uint32_t reserved24 : 8;
            };
            uint32_t val;
        } generator[2];
        union {
            struct {
                uint32_t fed_upmethod : 4;  /*Update method for FED (falling edge delay) active reg. 0: immediate  bit0: tez  bit1: tep  bit2: sync  bit3: freeze*/
                uint32_t red_upmethod : 4;  /*Update method for RED (rising edge delay) active reg. 0: immediate  bit0: tez  bit1: tep  bit2: sync  bit3: freeze*/
                uint32_t deb_mode : 1;      /*S8 in documentation  dual-edge B mode  0: fed/red take effect on different path separately  1: fed/red take effect on B path  A out is in bypass or dulpB mode*/
                uint32_t a_outswap : 1;     /*S6 in documentation*/
                uint32_t b_outswap : 1;     /*S7 in documentation*/
                uint32_t red_insel : 1;     /*S4 in documentation*/
                uint32_t fed_insel : 1;     /*S5 in documentation*/
                uint32_t red_outinvert : 1; /*S2 in documentation*/
                uint32_t fed_outinvert : 1; /*S3 in documentation*/
                uint32_t a_outbypass : 1;   /*S1 in documentation*/
                uint32_t b_outbypass : 1;   /*S0 in documentation*/
                uint32_t clk_sel : 1;       /*Dead band0 clock selection. 0: PWM_clk  1: PT_clk*/
                uint32_t reserved18 : 14;
            };
            uint32_t val;
        } db_cfg;
        union {
            struct {
                uint32_t fed : 16; /*Shadow reg for FED*/
                uint32_t reserved16 : 16;
            };
            uint32_t val;
        } db_fed_cfg;
        union {
            struct {
                uint32_t red : 16; /*Shadow reg for RED*/
                uint32_t reserved16 : 16;
            };
            uint32_t val;
        } db_red_cfg;
        union {
            struct {
                uint32_t en : 1;         /*When set  carrier0 function is enabled. When reset  carrier0 is bypassed*/
                uint32_t prescale : 4;   /*carrier0 clk (CP_clk) prescale value. Period of CP_clk = period of PWM_clk * (PWM_CARRIER0_PRESCALE + 1)*/
                uint32_t duty : 3;       /*carrier duty selection. Duty = PWM_CARRIER0_DUTY / 8*/
                uint32_t oshtwth : 4;    /*width of the fist pulse in number of periods of the carrier*/
                uint32_t out_invert : 1; /*when set  invert the output of PWM0A and PWM0B for this submodule*/
                uint32_t in_invert : 1;  /*when set  invert the input of PWM0A and PWM0B for this submodule*/
                uint32_t reserved14 : 18;
            };
            uint32_t val;
        } carrier_cfg;
        union {
            struct {
                uint32_t sw_cbc : 1;  /*Cycle-by-cycle tripping software force event will trigger cycle-by-cycle trip event. 0: disable  1: enable*/
                uint32_t f2_cbc : 1;  /*event_f2 will trigger cycle-by-cycle trip event. 0: disable  1: enable*/
                uint32_t f1_cbc : 1;  /*event_f1 will trigger cycle-by-cycle trip event. 0: disable  1: enable*/
                uint32_t f0_cbc : 1;  /*event_f0 will trigger cycle-by-cycle trip event. 0: disable  1: enable*/
                uint32_t sw_ost : 1;  /*one-shot tripping software force event will trigger one-shot trip event. 0: disable  1: enable*/
                uint32_t f2_ost : 1;  /*event_f2 will trigger one-shot trip event. 0: disable  1: enable*/
                uint32_t f1_ost : 1;  /*event_f1 will trigger one-shot trip event. 0: disable  1: enable*/
                uint32_t f0_ost : 1;  /*event_f0 will trigger one-shot trip event. 0: disable  1: enable*/
                uint32_t a_cbc_d : 2; /*Action on PWM0A when cycle-by-cycle trip event occurs and timer is decreasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
                uint32_t a_cbc_u : 2; /*Action on PWM0A when cycle-by-cycle trip event occurs and timer is increasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
                uint32_t a_ost_d : 2; /*Action on PWM0A when one-shot trip event occurs and timer is decreasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
                uint32_t a_ost_u : 2; /*Action on PWM0A when one-shot trip event occurs and timer is increasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
                uint32_t b_cbc_d : 2; /*Action on PWM0B when cycle-by-cycle trip event occurs and timer is decreasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
                uint32_t b_cbc_u : 2; /*Action on PWM0B when cycle-by-cycle trip event occurs and timer is increasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
                uint32_t b_ost_d : 2; /*Action on PWM0B when one-shot trip event occurs and timer is decreasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
                uint32_t b_ost_u : 2; /*Action on PWM0B when one-shot trip event occurs and timer is increasing. 0: do nothing  1: force lo  2: force hi  3: toggle*/
                uint32_t reserved24 : 8;
            };
            uint32_t val;
        } tz_cfg0;
        union {
            struct {
                uint32_t clr_ost : 1;   /*a posedge will clear on going one-shot tripping*/
                uint32_t cbcpulse : 2;  /*cycle-by-cycle tripping refresh moment selection. Bit0: TEZ  bit1:TEP*/
                uint32_t force_cbc : 1; /*a toggle trigger a cycle-by-cycle tripping software force event*/
                uint32_t force_ost : 1; /*a toggle (software negate its value) trigger a one-shot tripping software force event*/
                uint32_t reserved5 : 27;
            };
            uint32_t val;
        } tz_cfg1;
        union {
            struct {
                uint32_t cbc_on : 1; /*Set and reset by hardware. If set  an cycle-by-cycle trip event is on going*/
                uint32_t ost_on : 1; /*Set and reset by hardware. If set  an one-shot trip event is on going*/
                uint32_t reserved2 : 30;
            };
            uint32_t val;
        } tz_status;
    } channel[3];

    union {
        struct {
            uint32_t f0_en : 1;    /*When set  event_f0 generation is enabled*/
            uint32_t f1_en : 1;    /*When set  event_f1 generation is enabled*/
            uint32_t f2_en : 1;    /*When set  event_f2 generation is enabled*/
            uint32_t f0_pole : 1;  /*Set event_f0 trigger polarity on FAULT0 source from GPIO matrix. 0: level low  1: level high*/
            uint32_t f1_pole : 1;  /*Set event_f1 trigger polarity on FAULT1 source from GPIO matrix. 0: level low  1: level high*/
            uint32_t f2_pole : 1;  /*Set event_f2 trigger polarity on FAULT2 source from GPIO matrix. 0: level low  1: level high*/
            uint32_t event_f0 : 1; /*Set and reset by hardware. If set  event_f0 is on going*/
            uint32_t event_f1 : 1; /*Set and reset by hardware. If set  event_f1 is on going*/
            uint32_t event_f2 : 1; /*Set and reset by hardware. If set  event_f2 is on going*/
            uint32_t reserved9 : 23;
        };
        uint32_t val;
    } fault_detect;
    union {
        struct {
            uint32_t timer_en : 1;  /*When set  capture timer incrementing under APB_clk is enabled.*/
            uint32_t synci_en : 1;  /*When set  capture timer sync is enabled.*/
            uint32_t synci_sel : 3; /*capture module sync input selection. 0: none  1: timer0 synco  2: timer1 synco  3: timer2 synco  4: SYNC0 from GPIO matrix  5: SYNC1 from GPIO matrix  6: SYNC2 from GPIO matrix*/
            uint32_t sync_sw : 1;   /*Write 1 will force a capture timer sync  capture timer is loaded with value in phase register.*/
            uint32_t reserved6 : 26;
        };
        uint32_t val;
    } cap_timer_cfg;
    uint32_t cap_timer_phase; /*Phase value for capture timer sync operation.*/
    union {
        struct {
            uint32_t en : 1;        /*When set  capture on channel 0 is enabled*/
            uint32_t mode : 2;      /*Edge of capture on channel 0 after prescale.  bit0: negedge cap en  bit1: posedge cap en*/
            uint32_t prescale : 8;  /*Value of prescale on possitive edge of CAP0. Prescale value = PWM_CAP0_PRESCALE + 1*/
            uint32_t in_invert : 1; /*when set  CAP0 form GPIO matrix is inverted before prescale*/
            uint32_t sw : 1;        /*Write 1 will trigger a software forced capture on channel 0*/
            uint32_t reserved13 : 19;
        };
        uint32_t val;
    } cap_cfg_ch[3];
    uint32_t cap_val_ch[3]; /*Value of last capture on channel 0*/
    union {
        struct {
            uint32_t cap0_edge : 1; /*Edge of last capture trigger on channel 0  0: posedge  1: negedge*/
            uint32_t cap1_edge : 1; /*Edge of last capture trigger on channel 1  0: posedge  1: negedge*/
            uint32_t cap2_edge : 1; /*Edge of last capture trigger on channel 2  0: posedge  1: negedge*/
            uint32_t reserved3 : 29;
        };
        uint32_t val;
    } cap_status;
    union {
        struct {
            uint32_t global_up_en : 1;    /*The global enable of update of all active registers in MCPWM module*/
            uint32_t global_force_up : 1; /*a toggle (software invert its value) will trigger a forced update of all active registers in MCPWM module*/
            uint32_t op0_up_en : 1;       /*When set and PWM_GLOBAL_UP_EN is set  update of active registers in PWM operator 0 are enabled*/
            uint32_t op0_force_up : 1;    /*a toggle (software invert its value) will trigger a forced update of active registers in PWM operator 0*/
            uint32_t op1_up_en : 1;       /*When set and PWM_GLOBAL_UP_EN is set  update of active registers in PWM operator 1 are enabled*/
            uint32_t op1_force_up : 1;    /*a toggle (software invert its value) will trigger a forced update of active registers in PWM operator 1*/
            uint32_t op2_up_en : 1;       /*When set and PWM_GLOBAL_UP_EN is set  update of active registers in PWM operator 2 are enabled*/
            uint32_t op2_force_up : 1;    /*a toggle (software invert its value) will trigger a forced update of active registers in PWM operator 2*/
            uint32_t reserved8 : 24;
        };
        uint32_t val;
    } update_cfg;
    union {
        struct {
            uint32_t timer0_stop_int_ena : 1; /*Interrupt when timer 0 stops*/
            uint32_t timer1_stop_int_ena : 1; /*Interrupt when timer 1 stops*/
            uint32_t timer2_stop_int_ena : 1; /*Interrupt when timer 2 stops*/
            uint32_t timer0_tez_int_ena : 1;  /*A PWM timer 0 TEZ event will trigger this interrupt*/
            uint32_t timer1_tez_int_ena : 1;  /*A PWM timer 1 TEZ event will trigger this interrupt*/
            uint32_t timer2_tez_int_ena : 1;  /*A PWM timer 2 TEZ event will trigger this interrupt*/
            uint32_t timer0_tep_int_ena : 1;  /*A PWM timer 0 TEP event will trigger this interrupt*/
            uint32_t timer1_tep_int_ena : 1;  /*A PWM timer 1 TEP event will trigger this interrupt*/
            uint32_t timer2_tep_int_ena : 1;  /*A PWM timer 2 TEP event will trigger this interrupt*/
            uint32_t fault0_int_ena : 1;      /*Interrupt when event_f0 starts*/
            uint32_t fault1_int_ena : 1;      /*Interrupt when event_f1 starts*/
            uint32_t fault2_int_ena : 1;      /*Interrupt when event_f2 starts*/
            uint32_t fault0_clr_int_ena : 1;  /*Interrupt when event_f0 ends*/
            uint32_t fault1_clr_int_ena : 1;  /*Interrupt when event_f1 ends*/
            uint32_t fault2_clr_int_ena : 1;  /*Interrupt when event_f2 ends*/
            uint32_t cmpr0_tea_int_ena : 1;   /*A PWM operator 0 TEA event will trigger this interrupt*/
            uint32_t cmpr1_tea_int_ena : 1;   /*A PWM operator 1 TEA event will trigger this interrupt*/
            uint32_t cmpr2_tea_int_ena : 1;   /*A PWM operator 2 TEA event will trigger this interrupt*/
            uint32_t cmpr0_teb_int_ena : 1;   /*A PWM operator 0 TEB event will trigger this interrupt*/
            uint32_t cmpr1_teb_int_ena : 1;   /*A PWM operator 1 TEB event will trigger this interrupt*/
            uint32_t cmpr2_teb_int_ena : 1;   /*A PWM operator 2 TEB event will trigger this interrupt*/
            uint32_t tz0_cbc_int_ena : 1;     /*An cycle-by-cycle trip event on PWM0 will trigger this interrupt*/
            uint32_t tz1_cbc_int_ena : 1;     /*An cycle-by-cycle trip event on PWM1 will trigger this interrupt*/
            uint32_t tz2_cbc_int_ena : 1;     /*An cycle-by-cycle trip event on PWM2 will trigger this interrupt*/
            uint32_t tz0_ost_int_ena : 1;     /*An one-shot trip event on PWM0 will trigger this interrupt*/
            uint32_t tz1_ost_int_ena : 1;     /*An one-shot trip event on PWM1 will trigger this interrupt*/
            uint32_t tz2_ost_int_ena : 1;     /*An one-shot trip event on PWM2 will trigger this interrupt*/
            uint32_t cap0_int_ena : 1;        /*A capture on channel 0 will trigger this interrupt*/
            uint32_t cap1_int_ena : 1;        /*A capture on channel 1 will trigger this interrupt*/
            uint32_t cap2_int_ena : 1;        /*A capture on channel 2 will trigger this interrupt*/
            uint32_t reserved30 : 2;
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t timer0_stop_int_raw : 1; /*Interrupt when timer 0 stops*/
            uint32_t timer1_stop_int_raw : 1; /*Interrupt when timer 1 stops*/
            uint32_t timer2_stop_int_raw : 1; /*Interrupt when timer 2 stops*/
            uint32_t timer0_tez_int_raw : 1;  /*A PWM timer 0 TEZ event will trigger this interrupt*/
            uint32_t timer1_tez_int_raw : 1;  /*A PWM timer 1 TEZ event will trigger this interrupt*/
            uint32_t timer2_tez_int_raw : 1;  /*A PWM timer 2 TEZ event will trigger this interrupt*/
            uint32_t timer0_tep_int_raw : 1;  /*A PWM timer 0 TEP event will trigger this interrupt*/
            uint32_t timer1_tep_int_raw : 1;  /*A PWM timer 1 TEP event will trigger this interrupt*/
            uint32_t timer2_tep_int_raw : 1;  /*A PWM timer 2 TEP event will trigger this interrupt*/
            uint32_t fault0_int_raw : 1;      /*Interrupt when event_f0 starts*/
            uint32_t fault1_int_raw : 1;      /*Interrupt when event_f1 starts*/
            uint32_t fault2_int_raw : 1;      /*Interrupt when event_f2 starts*/
            uint32_t fault0_clr_int_raw : 1;  /*Interrupt when event_f0 ends*/
            uint32_t fault1_clr_int_raw : 1;  /*Interrupt when event_f1 ends*/
            uint32_t fault2_clr_int_raw : 1;  /*Interrupt when event_f2 ends*/
            uint32_t cmpr0_tea_int_raw : 1;   /*A PWM operator 0 TEA event will trigger this interrupt*/
            uint32_t cmpr1_tea_int_raw : 1;   /*A PWM operator 1 TEA event will trigger this interrupt*/
            uint32_t cmpr2_tea_int_raw : 1;   /*A PWM operator 2 TEA event will trigger this interrupt*/
            uint32_t cmpr0_teb_int_raw : 1;   /*A PWM operator 0 TEB event will trigger this interrupt*/
            uint32_t cmpr1_teb_int_raw : 1;   /*A PWM operator 1 TEB event will trigger this interrupt*/
            uint32_t cmpr2_teb_int_raw : 1;   /*A PWM operator 2 TEB event will trigger this interrupt*/
            uint32_t tz0_cbc_int_raw : 1;     /*An cycle-by-cycle trip event on PWM0 will trigger this interrupt*/
            uint32_t tz1_cbc_int_raw : 1;     /*An cycle-by-cycle trip event on PWM1 will trigger this interrupt*/
            uint32_t tz2_cbc_int_raw : 1;     /*An cycle-by-cycle trip event on PWM2 will trigger this interrupt*/
            uint32_t tz0_ost_int_raw : 1;     /*An one-shot trip event on PWM0 will trigger this interrupt*/
            uint32_t tz1_ost_int_raw : 1;     /*An one-shot trip event on PWM1 will trigger this interrupt*/
            uint32_t tz2_ost_int_raw : 1;     /*An one-shot trip event on PWM2 will trigger this interrupt*/
            uint32_t cap0_int_raw : 1;        /*A capture on channel 0 will trigger this interrupt*/
            uint32_t cap1_int_raw : 1;        /*A capture on channel 1 will trigger this interrupt*/
            uint32_t cap2_int_raw : 1;        /*A capture on channel 2 will trigger this interrupt*/
            uint32_t reserved30 : 2;
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t timer0_stop_int_st : 1; /*Interrupt when timer 0 stops*/
            uint32_t timer1_stop_int_st : 1; /*Interrupt when timer 1 stops*/
            uint32_t timer2_stop_int_st : 1; /*Interrupt when timer 2 stops*/
            uint32_t timer0_tez_int_st : 1;  /*A PWM timer 0 TEZ event will trigger this interrupt*/
            uint32_t timer1_tez_int_st : 1;  /*A PWM timer 1 TEZ event will trigger this interrupt*/
            uint32_t timer2_tez_int_st : 1;  /*A PWM timer 2 TEZ event will trigger this interrupt*/
            uint32_t timer0_tep_int_st : 1;  /*A PWM timer 0 TEP event will trigger this interrupt*/
            uint32_t timer1_tep_int_st : 1;  /*A PWM timer 1 TEP event will trigger this interrupt*/
            uint32_t timer2_tep_int_st : 1;  /*A PWM timer 2 TEP event will trigger this interrupt*/
            uint32_t fault0_int_st : 1;      /*Interrupt when event_f0 starts*/
            uint32_t fault1_int_st : 1;      /*Interrupt when event_f1 starts*/
            uint32_t fault2_int_st : 1;      /*Interrupt when event_f2 starts*/
            uint32_t fault0_clr_int_st : 1;  /*Interrupt when event_f0 ends*/
            uint32_t fault1_clr_int_st : 1;  /*Interrupt when event_f1 ends*/
            uint32_t fault2_clr_int_st : 1;  /*Interrupt when event_f2 ends*/
            uint32_t cmpr0_tea_int_st : 1;   /*A PWM operator 0 TEA event will trigger this interrupt*/
            uint32_t cmpr1_tea_int_st : 1;   /*A PWM operator 1 TEA event will trigger this interrupt*/
            uint32_t cmpr2_tea_int_st : 1;   /*A PWM operator 2 TEA event will trigger this interrupt*/
            uint32_t cmpr0_teb_int_st : 1;   /*A PWM operator 0 TEB event will trigger this interrupt*/
            uint32_t cmpr1_teb_int_st : 1;   /*A PWM operator 1 TEB event will trigger this interrupt*/
            uint32_t cmpr2_teb_int_st : 1;   /*A PWM operator 2 TEB event will trigger this interrupt*/
            uint32_t tz0_cbc_int_st : 1;     /*An cycle-by-cycle trip event on PWM0 will trigger this interrupt*/
            uint32_t tz1_cbc_int_st : 1;     /*An cycle-by-cycle trip event on PWM1 will trigger this interrupt*/
            uint32_t tz2_cbc_int_st : 1;     /*An cycle-by-cycle trip event on PWM2 will trigger this interrupt*/
            uint32_t tz0_ost_int_st : 1;     /*An one-shot trip event on PWM0 will trigger this interrupt*/
            uint32_t tz1_ost_int_st : 1;     /*An one-shot trip event on PWM1 will trigger this interrupt*/
            uint32_t tz2_ost_int_st : 1;     /*An one-shot trip event on PWM2 will trigger this interrupt*/
            uint32_t cap0_int_st : 1;        /*A capture on channel 0 will trigger this interrupt*/
            uint32_t cap1_int_st : 1;        /*A capture on channel 1 will trigger this interrupt*/
            uint32_t cap2_int_st : 1;        /*A capture on channel 2 will trigger this interrupt*/
            uint32_t reserved30 : 2;
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t timer0_stop_int_clr : 1; /*Interrupt when timer 0 stops*/
            uint32_t timer1_stop_int_clr : 1; /*Interrupt when timer 1 stops*/
            uint32_t timer2_stop_int_clr : 1; /*Interrupt when timer 2 stops*/
            uint32_t timer0_tez_int_clr : 1;  /*A PWM timer 0 TEZ event will trigger this interrupt*/
            uint32_t timer1_tez_int_clr : 1;  /*A PWM timer 1 TEZ event will trigger this interrupt*/
            uint32_t timer2_tez_int_clr : 1;  /*A PWM timer 2 TEZ event will trigger this interrupt*/
            uint32_t timer0_tep_int_clr : 1;  /*A PWM timer 0 TEP event will trigger this interrupt*/
            uint32_t timer1_tep_int_clr : 1;  /*A PWM timer 1 TEP event will trigger this interrupt*/
            uint32_t timer2_tep_int_clr : 1;  /*A PWM timer 2 TEP event will trigger this interrupt*/
            uint32_t fault0_int_clr : 1;      /*Interrupt when event_f0 starts*/
            uint32_t fault1_int_clr : 1;      /*Interrupt when event_f1 starts*/
            uint32_t fault2_int_clr : 1;      /*Interrupt when event_f2 starts*/
            uint32_t fault0_clr_int_clr : 1;  /*Interrupt when event_f0 ends*/
            uint32_t fault1_clr_int_clr : 1;  /*Interrupt when event_f1 ends*/
            uint32_t fault2_clr_int_clr : 1;  /*Interrupt when event_f2 ends*/
            uint32_t cmpr0_tea_int_clr : 1;   /*A PWM operator 0 TEA event will trigger this interrupt*/
            uint32_t cmpr1_tea_int_clr : 1;   /*A PWM operator 1 TEA event will trigger this interrupt*/
            uint32_t cmpr2_tea_int_clr : 1;   /*A PWM operator 2 TEA event will trigger this interrupt*/
            uint32_t cmpr0_teb_int_clr : 1;   /*A PWM operator 0 TEB event will trigger this interrupt*/
            uint32_t cmpr1_teb_int_clr : 1;   /*A PWM operator 1 TEB event will trigger this interrupt*/
            uint32_t cmpr2_teb_int_clr : 1;   /*A PWM operator 2 TEB event will trigger this interrupt*/
            uint32_t tz0_cbc_int_clr : 1;     /*An cycle-by-cycle trip event on PWM0 will trigger this interrupt*/
            uint32_t tz1_cbc_int_clr : 1;     /*An cycle-by-cycle trip event on PWM1 will trigger this interrupt*/
            uint32_t tz2_cbc_int_clr : 1;     /*An cycle-by-cycle trip event on PWM2 will trigger this interrupt*/
            uint32_t tz0_ost_int_clr : 1;     /*An one-shot trip event on PWM0 will trigger this interrupt*/
            uint32_t tz1_ost_int_clr : 1;     /*An one-shot trip event on PWM1 will trigger this interrupt*/
            uint32_t tz2_ost_int_clr : 1;     /*An one-shot trip event on PWM2 will trigger this interrupt*/
            uint32_t cap0_int_clr : 1;        /*A capture on channel 0 will trigger this interrupt*/
            uint32_t cap1_int_clr : 1;        /*A capture on channel 1 will trigger this interrupt*/
            uint32_t cap2_int_clr : 1;        /*A capture on channel 2 will trigger this interrupt*/
            uint32_t reserved30 : 2;
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t clk_en : 1; /*Force clock on for this reg file*/
            uint32_t reserved1 : 31;
        };
        uint32_t val;
    } reg_clk;
    union {
        struct {
            uint32_t date : 28; /*Version of this reg file*/
            uint32_t reserved28 : 4;
        };
        uint32_t val;
    } version;
} mcpwm_dev_t;

extern mcpwm_dev_t MCPWM0;
extern mcpwm_dev_t MCPWM1;

#ifdef __cplusplus
}
#endif
