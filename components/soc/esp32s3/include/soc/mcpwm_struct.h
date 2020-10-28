// Copyright 2017-2020 Espressif Systems (Shanghai) PTE LTD
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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef volatile struct {
    union {
        struct {
            uint32_t prescale:     8;
            uint32_t reserved8:   24;
        };
        uint32_t val;
    } clk_cfg;
    struct {
        union {
            struct {
                uint32_t prescale:               8;
                uint32_t period:                16;
                uint32_t upmethod:               2;        /*0: immediate  1: eqz  2: sync  3: eqz | sync*/
                uint32_t reserved26:             6;
            };
            uint32_t val;
        } period;
        union {
            struct {
                uint32_t start:        3;                  /*0: stop @ eqz  1: stop @ eqp  2: free run  3: start and stop @ next eqz  4: start and stop @ next eqp*/
                uint32_t mode:         2;                  /*0: freeze  1: inc  2: dec  3: up-down*/
                uint32_t reserved5:   27;
            };
            uint32_t val;
        } mode;
        union {
            struct {
                uint32_t in_en:            1;
                uint32_t sync_sw:          1;              /*write the negate value will trigger a sw sync*/
                uint32_t out_sel:          2;
                uint32_t timer_phase:     17;
                uint32_t reserved21:      11;
            };
            uint32_t val;
        } sync;
        union {
            struct {
                uint32_t value:           16;
                uint32_t direction:        1;
                uint32_t reserved17:      15;
            };
            uint32_t val;
        } status;
    } timer[3];
    union {
        struct {
            uint32_t t0_in_sel:              3;
            uint32_t t1_in_sel:              3;
            uint32_t t2_in_sel:              3;
            uint32_t ext_in0_inv:            1;
            uint32_t ext_in1_inv:            1;
            uint32_t ext_in2_inv:            1;
            uint32_t reserved12:            20;
        };
        uint32_t val;
    } timer_synci_cfg;
    union {
        struct {
            uint32_t operator0_sel:      2;            /*0: timer0  1: timer1  2: timer2*/
            uint32_t operator1_sel:      2;            /*0: timer0  1: timer1  2: timer2*/
            uint32_t operator2_sel:      2;            /*0: timer0  1: timer1  2: timer2*/
            uint32_t reserved6:         26;
        };
        uint32_t val;
    } timer_sel;
    struct {
        union {
            struct {
                uint32_t a_upmethod:        4;             /*0: immediate  bit0: tez  bit1: tep  bit2: sync  bit3: freeze*/
                uint32_t b_upmethod:        4;             /*0: immediate  bit0: tez  bit1: tep  bit2: sync  bit3: freeze*/
                uint32_t a_shdw_full:       1;
                uint32_t b_shdw_full:       1;
                uint32_t reserved10:       22;
            };
            uint32_t val;
        } cmpr_cfg;
        union {
            struct {
                uint32_t cmpr_val:  16;
                uint32_t reserved16: 16;
            };
            uint32_t val;
        } cmpr_value[2];
        union {
            struct {
                uint32_t upmethod:          4;             /*0: immediate  bit0: tez  bit1: tep  bit2: sync. bit3: freeze*/
                uint32_t t0_sel:            3;             /*take effect immediately  0: extra0  1: extra1  2: extra2  3: sync_taken  4: none*/
                uint32_t t1_sel:            3;             /*take effect immediately  0: extra0  1: extra1  2: extra2  3: sync_taken  4: none*/
                uint32_t reserved10:       22;
            };
            uint32_t val;
        } gen_cfg0;
        union {
            struct {
                uint32_t cntu_force_upmethod:     6;       /*0: immediate  bit0: tez  bit1: tep  bit2: tea  bit3: teb  bit4: sync  bit5: freeze*/
                uint32_t a_cntuforce_mode:        2;       /*0: disabled  1: low  2: high  3: disabled*/
                uint32_t b_cntuforce_mode:        2;       /*0: disabled  1: low  2: high  3: disabled*/
                uint32_t a_nciforce:              1;       /*non-continuous immediate sw force  a toggle will trigger a force event*/
                uint32_t a_nciforce_mode:         2;       /*0: disabled  1: low  2: high  3: disabled*/
                uint32_t b_nciforce:              1;       /*non-continuous immediate sw force  a toggle will trigger a force event*/
                uint32_t b_nciforce_mode:         2;       /*0: disabled  1: low  2: high  3: disabled*/
                uint32_t reserved16:             16;
            };
            uint32_t val;
        } gen_force;
        union {
            struct {
                uint32_t utez:        2;
                uint32_t utep:        2;
                uint32_t utea:        2;
                uint32_t uteb:        2;
                uint32_t ut0:         2;
                uint32_t ut1:         2;
                uint32_t dtez:        2;
                uint32_t dtep:        2;
                uint32_t dtea:        2;
                uint32_t dteb:        2;
                uint32_t dt0:         2;
                uint32_t dt1:         2;                   /*0: no change  1: low  2: high  3: toggle*/
                uint32_t reserved24:  8;
            };
            uint32_t val;
        } generator[2];
        union {
            struct {
                uint32_t fed_upmethod:      4;             /*0: immediate  bit0: tez  bit1: tep  bit2: sync  bit3: freeze*/
                uint32_t red_upmethod:      4;             /*0: immediate  bit0: tez  bit1: tep  bit2: sync  bit3: freeze*/
                uint32_t deb_mode:          1;             /*immediate  dual-edge B mode  0: fed/red take effect on different path separately  1: fed/red take effect on B path  A out is in bypass or dulpB mode*/
                uint32_t a_outswap:         1;
                uint32_t b_outswap:         1;
                uint32_t red_insel:         1;
                uint32_t fed_insel:         1;
                uint32_t red_outinvert:     1;
                uint32_t fed_outinvert:     1;
                uint32_t a_outbypass:       1;
                uint32_t b_outbypass:       1;
                uint32_t clk_sel:           1;
                uint32_t reserved18:       14;
            };
            uint32_t val;
        } db_cfg;
        union {
            struct {
                uint32_t fed:       16;
                uint32_t reserved16: 16;
            };
            uint32_t val;
        } db_fed_cfg;
        union {
            struct {
                uint32_t red:       16;
                uint32_t reserved16: 16;
            };
            uint32_t val;
        } db_red_cfg;
        union {
            struct {
                uint32_t en:                  1;
                uint32_t prescale:            4;
                uint32_t duty:                3;
                uint32_t oshtwth:             4;
                uint32_t out_invert:          1;
                uint32_t in_invert:           1;
                uint32_t reserved14:         18;
            };
            uint32_t val;
        } carrier_cfg;
        union {
            struct {
                uint32_t sw_cbc:      1;                   /*0: disable  1: enable*/
                uint32_t f2_cbc:      1;                   /*0: disable  1: enable*/
                uint32_t f1_cbc:      1;                   /*0: disable  1: enable*/
                uint32_t f0_cbc:      1;                   /*0: disable  1: enable*/
                uint32_t sw_ost:      1;                   /*0: disable  1: enable*/
                uint32_t f2_ost:      1;                   /*0: disable  1: enable*/
                uint32_t f1_ost:      1;                   /*0: disable  1: enable*/
                uint32_t f0_ost:      1;                   /*0: disable  1: enable*/
                uint32_t a_cbc_d:     2;                   /*0: do nothing  1: force lo  2: force hi  3: toggle*/
                uint32_t a_cbc_u:     2;                   /*0: do nothing  1: force lo  2: force hi  3: toggle*/
                uint32_t a_ost_d:     2;                   /*0: do nothing  1: force lo  2: force hi  3: toggle*/
                uint32_t a_ost_u:     2;                   /*0: do nothing  1: force lo  2: force hi  3: toggle*/
                uint32_t b_cbc_d:     2;                   /*0: do nothing  1: force lo  2: force hi  3: toggle*/
                uint32_t b_cbc_u:     2;                   /*0: do nothing  1: force lo  2: force hi  3: toggle*/
                uint32_t b_ost_d:     2;                   /*0: do nothing  1: force lo  2: force hi  3: toggle*/
                uint32_t b_ost_u:     2;                   /*0: do nothing  1: force lo  2: force hi  3: toggle*/
                uint32_t reserved24:  8;
            };
            uint32_t val;
        } tz_cfg0;
        union {
            struct {
                uint32_t clr_ost:       1;                 /*a toggle will clear oneshot tripping*/
                uint32_t cbcpulse:      2;                 /*bit0: tez  bit1: tep*/
                uint32_t force_cbc:     1;                 /*a toggle trigger a cycle-by-cycle tripping*/
                uint32_t force_ost:     1;                 /*a toggle trigger a oneshot tripping*/
                uint32_t reserved5:    27;
            };
            uint32_t val;
        } tz_cfg1;
        union {
            struct {
                uint32_t cbc_on:     1;
                uint32_t ost_on:     1;
                uint32_t reserved2: 30;
            };
            uint32_t val;
        } tz_status;
    } channel[3];
    union {
        struct {
            uint32_t f0_en:      1;
            uint32_t f1_en:      1;
            uint32_t f2_en:      1;
            uint32_t f0_pole:    1;
            uint32_t f1_pole:    1;
            uint32_t f2_pole:    1;
            uint32_t event_f0:   1;
            uint32_t event_f1:   1;
            uint32_t event_f2:   1;
            uint32_t reserved9: 23;
        };
        uint32_t val;
    } fault_detect;
    union {
        struct {
            uint32_t timer_en:      1;
            uint32_t synci_en:      1;
            uint32_t synci_sel:     3;
            uint32_t sync_sw:       1;                 /*Write 1 will force a timer sync*/
            uint32_t reserved6:    26;
        };
        uint32_t val;
    } cap_timer_cfg;
    uint32_t cap_timer_phase;                      /**/
    union {
        struct {
            uint32_t en:             1;
            uint32_t mode:           2;                /*bit0: negedge cap en  bit1: posedge cap en*/
            uint32_t prescale:       8;
            uint32_t in_invert:      1;
            uint32_t sw:             1;                /*Write 1 will trigger a sw capture*/
            uint32_t reserved13:    19;
        };
        uint32_t val;
    } cap_cfg_ch[3];
    uint32_t cap_val_ch[3];                              /**/
    union {
        struct {
            uint32_t cap0_edge:  1;
            uint32_t cap1_edge:  1;
            uint32_t cap2_edge:  1;                    /*cap trigger's edge  0: posedge  1: negedge*/
            uint32_t reserved3: 29;
        };
        uint32_t val;
    } cap_status;
    union {
        struct {
            uint32_t global_up_en:    1;
            uint32_t global_force_up: 1;               /*a toggle will trigger a force update  all timers and operators will update their active regs*/
            uint32_t op0_up_en:       1;
            uint32_t op0_force_up:    1;               /*a toggle will trigger a force update*/
            uint32_t op1_up_en:       1;
            uint32_t op1_force_up:    1;               /*a toggle will trigger a force update*/
            uint32_t op2_up_en:       1;               /*reg update local enable*/
            uint32_t op2_force_up:    1;               /*a toggle will trigger a force update*/
            uint32_t reserved8:      24;
        };
        uint32_t val;
    } update_cfg;
    union {
        struct {
            uint32_t timer0_stop:         1;
            uint32_t timer1_stop:         1;
            uint32_t timer2_stop:         1;
            uint32_t timer0_tez:          1;
            uint32_t timer1_tez:          1;
            uint32_t timer2_tez:          1;
            uint32_t timer0_tep:          1;
            uint32_t timer1_tep:          1;
            uint32_t timer2_tep:          1;
            uint32_t fault0:              1;
            uint32_t fault1:              1;
            uint32_t fault2:              1;
            uint32_t fault0_clr:          1;
            uint32_t fault1_clr:          1;
            uint32_t fault2_clr:          1;
            uint32_t cmpr0_tea:           1;
            uint32_t cmpr1_tea:           1;
            uint32_t cmpr2_tea:           1;
            uint32_t cmpr0_teb:           1;
            uint32_t cmpr1_teb:           1;
            uint32_t cmpr2_teb:           1;
            uint32_t tz0_cbc:             1;
            uint32_t tz1_cbc:             1;
            uint32_t tz2_cbc:             1;
            uint32_t tz0_ost:             1;
            uint32_t tz1_ost:             1;
            uint32_t tz2_ost:             1;
            uint32_t cap0:                1;
            uint32_t cap1:                1;
            uint32_t cap2:                1;
            uint32_t reserved30:          2;
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t timer0_stop:         1;
            uint32_t timer1_stop:         1;
            uint32_t timer2_stop:         1;
            uint32_t timer0_tez:          1;
            uint32_t timer1_tez:          1;
            uint32_t timer2_tez:          1;
            uint32_t timer0_tep:          1;
            uint32_t timer1_tep:          1;
            uint32_t timer2_tep:          1;
            uint32_t fault0:              1;
            uint32_t fault1:              1;
            uint32_t fault2:              1;
            uint32_t fault0_clr:          1;
            uint32_t fault1_clr:          1;
            uint32_t fault2_clr:          1;
            uint32_t cmpr0_tea:           1;
            uint32_t cmpr1_tea:           1;
            uint32_t cmpr2_tea:           1;
            uint32_t cmpr0_teb:           1;
            uint32_t cmpr1_teb:           1;
            uint32_t cmpr2_teb:           1;
            uint32_t tz0_cbc:             1;
            uint32_t tz1_cbc:             1;
            uint32_t tz2_cbc:             1;
            uint32_t tz0_ost:             1;
            uint32_t tz1_ost:             1;
            uint32_t tz2_ost:             1;
            uint32_t cap0:                1;
            uint32_t cap1:                1;
            uint32_t cap2:                1;
            uint32_t reserved30:          2;
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t timer0_stop:        1;
            uint32_t timer1_stop:        1;
            uint32_t timer2_stop:        1;
            uint32_t timer0_tez:         1;
            uint32_t timer1_tez:         1;
            uint32_t timer2_tez:         1;
            uint32_t timer0_tep:         1;
            uint32_t timer1_tep:         1;
            uint32_t timer2_tep:         1;
            uint32_t fault0:             1;
            uint32_t fault1:             1;
            uint32_t fault2:             1;
            uint32_t fault0_clr:         1;
            uint32_t fault1_clr:         1;
            uint32_t fault2_clr:         1;
            uint32_t cmpr0_tea:          1;
            uint32_t cmpr1_tea:          1;
            uint32_t cmpr2_tea:          1;
            uint32_t cmpr0_teb:          1;
            uint32_t cmpr1_teb:          1;
            uint32_t cmpr2_teb:          1;
            uint32_t tz0_cbc:            1;
            uint32_t tz1_cbc:            1;
            uint32_t tz2_cbc:            1;
            uint32_t tz0_ost:            1;
            uint32_t tz1_ost:            1;
            uint32_t tz2_ost:            1;
            uint32_t cap0:               1;
            uint32_t cap1:               1;
            uint32_t cap2:               1;
            uint32_t reserved30:         2;
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t timer0_stop:         1;
            uint32_t timer1_stop:         1;
            uint32_t timer2_stop:         1;
            uint32_t timer0_tez:          1;
            uint32_t timer1_tez:          1;
            uint32_t timer2_tez:          1;
            uint32_t timer0_tep:          1;
            uint32_t timer1_tep:          1;
            uint32_t timer2_tep:          1;
            uint32_t fault0:              1;
            uint32_t fault1:              1;
            uint32_t fault2:              1;
            uint32_t fault0_clr:          1;
            uint32_t fault1_clr:          1;
            uint32_t fault2_clr:          1;
            uint32_t cmpr0_tea:           1;
            uint32_t cmpr1_tea:           1;
            uint32_t cmpr2_tea:           1;
            uint32_t cmpr0_teb:           1;
            uint32_t cmpr1_teb:           1;
            uint32_t cmpr2_teb:           1;
            uint32_t tz0_cbc:             1;
            uint32_t tz1_cbc:             1;
            uint32_t tz2_cbc:             1;
            uint32_t tz0_ost:             1;
            uint32_t tz1_ost:             1;
            uint32_t tz2_ost:             1;
            uint32_t cap0:                1;
            uint32_t cap1:                1;
            uint32_t cap2:                1;
            uint32_t reserved30:          2;
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t clk_en:     1;
            uint32_t reserved1: 31;
        };
        uint32_t val;
    } reg_clk;
    union {
        struct {
            uint32_t date:      28;
            uint32_t reserved28: 4;
        };
        uint32_t val;
    } version;
} mcpwm_dev_t;

extern mcpwm_dev_t MCPWM0;
extern mcpwm_dev_t MCPWM1;

#ifdef __cplusplus
}
#endif
