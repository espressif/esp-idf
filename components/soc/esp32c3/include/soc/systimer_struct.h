// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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
#ifndef _SOC_SYS_TIMER_STRUCT_H_
#define _SOC_SYS_TIMER_STRUCT_H_
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct {
    union {
        struct {
            uint32_t systimer_clk_fo:            1;       /*systimer clock force on*/
            uint32_t reserved1:                 21;
            uint32_t target2_work_en:            1;       /*target2 work enable*/
            uint32_t target1_work_en:            1;       /*target1 work enable*/
            uint32_t target0_work_en:            1;       /*target0 work enable*/
            uint32_t timer_unit1_core1_stall_en: 1;       /*If timer unit1 is stalled when core1 stalled*/
            uint32_t timer_unit1_core0_stall_en: 1;       /*If timer unit1 is stalled when core0 stalled*/
            uint32_t timer_unit0_core1_stall_en: 1;       /*If timer unit0 is stalled when core1 stalled*/
            uint32_t timer_unit0_core0_stall_en: 1;       /*If timer unit0 is stalled when core0 stalled*/
            uint32_t timer_unit1_work_en:        1;       /*timer unit1 work enable*/
            uint32_t timer_unit0_work_en:        1;       /*timer unit0 work enable*/
            uint32_t clk_en:                     1;       /*register file clk gating*/
        };
        uint32_t val;
    } systimer_conf;
    union {
        struct {
            uint32_t reserved0:              29;
            uint32_t timer_unit0_value_valid: 1;
            uint32_t timer_unit0_update:      1;          /*update timer_unit0*/
            uint32_t reserved31:              1;
        };
        uint32_t val;
    } systimer_unit0_op;
    union {
        struct {
            uint32_t reserved0:              29;
            uint32_t timer_unit1_value_valid: 1;          /*timer value is sync and valid*/
            uint32_t timer_unit1_update:      1;          /*update timer unit1*/
            uint32_t reserved31:              1;
        };
        uint32_t val;
    } systimer_unit1_op;
    union {
        struct {
            uint32_t timer_unit0_load_hi:20;              /*timer unit0 load high 32 bit*/
            uint32_t reserved20:         12;
        };
        uint32_t val;
    } systimer_unit0_load_hi;
    uint32_t systimer_unit0_load_lo;                      /**/
    union {
        struct {
            uint32_t timer_unit1_load_hi:20;              /*timer unit1 load high 32 bit*/
            uint32_t reserved20:         12;
        };
        uint32_t val;
    } systimer_unit1_load_hi;
    uint32_t systimer_unit1_load_lo;                      /**/
    union {
        struct {
            uint32_t timer_target0_hi:20;                 /*timer taget0 high 32 bit*/
            uint32_t reserved20:      12;
        };
        uint32_t val;
    } systimer_target0_hi;
    uint32_t systimer_target0_lo;                         /**/
    union {
        struct {
            uint32_t timer_target1_hi:20;                 /*timer taget1 high 32 bit*/
            uint32_t reserved20:      12;
        };
        uint32_t val;
    } systimer_target1_hi;
    uint32_t systimer_target1_lo;                         /**/
    union {
        struct {
            uint32_t timer_target2_hi:20;                 /*timer taget2 high 32 bit*/
            uint32_t reserved20:      12;
        };
        uint32_t val;
    } systimer_target2_hi;
    uint32_t systimer_target2_lo;                         /**/
    union {
        struct {
            uint32_t target0_period:        26;           /*target0 period*/
            uint32_t reserved26:             4;
            uint32_t target0_period_mode:    1;           /*Set target0 to period mode*/
            uint32_t target0_timer_unit_sel: 1;           /*select which unit to compare*/
        };
        uint32_t val;
    } systimer_target0_conf;
    union {
        struct {
            uint32_t target1_period:        26;           /*target1 period*/
            uint32_t reserved26:             4;
            uint32_t target1_period_mode:    1;           /*Set target1 to period mode*/
            uint32_t target1_timer_unit_sel: 1;           /*select which unit to compare*/
        };
        uint32_t val;
    } systimer_target1_conf;
    union {
        struct {
            uint32_t target2_period:        26;           /*target2 period*/
            uint32_t reserved26:             4;
            uint32_t target2_period_mode:    1;           /*Set target2 to period mode*/
            uint32_t target2_timer_unit_sel: 1;           /*select which unit to compare*/
        };
        uint32_t val;
    } systimer_target2_conf;
    union {
        struct {
            uint32_t timer_unit0_value_hi:20;             /*timer read value high 32bit*/
            uint32_t reserved20:          12;
        };
        uint32_t val;
    } systimer_unit0_value_hi;
    uint32_t systimer_unit0_value_lo;                     /**/
    union {
        struct {
            uint32_t timer_unit1_value_hi:20;             /*timer read value high 32bit*/
            uint32_t reserved20:          12;
        };
        uint32_t val;
    } systimer_unit1_value_hi;
    uint32_t systimer_unit1_value_lo;                     /**/
    union {
        struct {
            uint32_t timer_comp0_load: 1;                 /*timer comp0 load value*/
            uint32_t reserved1:       31;
        };
        uint32_t val;
    } systimer_comp0_load;
    union {
        struct {
            uint32_t timer_comp1_load: 1;                 /*timer comp1 load value*/
            uint32_t reserved1:       31;
        };
        uint32_t val;
    } systimer_comp1_load;
    union {
        struct {
            uint32_t timer_comp2_load: 1;                 /*timer comp2 load value*/
            uint32_t reserved1:       31;
        };
        uint32_t val;
    } systimer_comp2_load;
    union {
        struct {
            uint32_t timer_unit0_load: 1;                 /*timer unit0 load value*/
            uint32_t reserved1:       31;
        };
        uint32_t val;
    } systimer_unit0_load;
    union {
        struct {
            uint32_t timer_unit1_load: 1;                 /*timer unit1 load value*/
            uint32_t reserved1:       31;
        };
        uint32_t val;
    } systimer_unit1_load;
    union {
        struct {
            uint32_t target0:         1;                  /*interupt0 enable*/
            uint32_t target1:         1;                  /*interupt1 enable*/
            uint32_t target2:         1;                  /*interupt2 enable*/
            uint32_t reserved3:      29;
        };
        uint32_t val;
    } systimer_int_ena;
    union {
        struct {
            uint32_t target0:         1;                  /*interupt0 raw*/
            uint32_t target1:         1;                  /*interupt1 raw*/
            uint32_t target2:         1;                  /*interupt2 raw*/
            uint32_t reserved3:      29;
        };
        uint32_t val;
    } systimer_int_raw;
    union {
        struct {
            uint32_t target0:         1;                  /*interupt0 clear*/
            uint32_t target1:         1;                  /*interupt1 clear*/
            uint32_t target2:         1;                  /*interupt2 clear*/
            uint32_t reserved3:      29;
        };
        uint32_t val;
    } systimer_int_clr;
    union {
        struct {
            uint32_t target0:        1;
            uint32_t target1:        1;
            uint32_t target2:        1;
            uint32_t reserved3:     29;
        };
        uint32_t val;
    } systimer_int_st;
    uint32_t reserved_74;
    uint32_t reserved_78;
    uint32_t reserved_7c;
    uint32_t reserved_80;
    uint32_t reserved_84;
    uint32_t reserved_88;
    uint32_t reserved_8c;
    uint32_t reserved_90;
    uint32_t reserved_94;
    uint32_t reserved_98;
    uint32_t reserved_9c;
    uint32_t reserved_a0;
    uint32_t reserved_a4;
    uint32_t reserved_a8;
    uint32_t reserved_ac;
    uint32_t reserved_b0;
    uint32_t reserved_b4;
    uint32_t reserved_b8;
    uint32_t reserved_bc;
    uint32_t reserved_c0;
    uint32_t reserved_c4;
    uint32_t reserved_c8;
    uint32_t reserved_cc;
    uint32_t reserved_d0;
    uint32_t reserved_d4;
    uint32_t reserved_d8;
    uint32_t reserved_dc;
    uint32_t reserved_e0;
    uint32_t reserved_e4;
    uint32_t reserved_e8;
    uint32_t reserved_ec;
    uint32_t reserved_f0;
    uint32_t reserved_f4;
    uint32_t reserved_f8;
    uint32_t systimer_date;                               /**/
} sys_timer_dev_t;
extern sys_timer_dev_t SYS_TIMER;
#ifdef __cplusplus
}
#endif

#endif  /* _SOC_SYS_TIMER_STRUCT_H_ */
