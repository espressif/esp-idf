// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _DRIVER_LEDC_PWM_STRUCT_H_
#define _DRIVER_LEDC_PWM_STRUCT_H_

#include "soc/ledc_reg.h"

#define ledc_reg_s ((ledc_reg*)(DR_REG_LEDC_BASE))

typedef struct ledc_pwm_s {
	struct ledcH_channel_config{
		union {
			struct {
				unsigned int timer_sel_hsch0: 2;
				unsigned int sig_out_en_hsch0: 1;
				unsigned int idle_lv_hsch0: 1;
				unsigned int reserved4: 27;
				unsigned int clk_en: 1;
			} ;
			uint32_t val;
		} LEDC_HSCHX_CONF0;
		union {
			struct {
				unsigned int hpoint_hsch0: 20;
				unsigned int reserved20: 12;
			} ;
			uint32_t val;
		} LEDC_HSCHX_HPOINT;
		union {
			struct {
				unsigned int duty_hsch0: 25;
				unsigned int reserved25: 7;
			} ;
			uint32_t val;
		} LEDC_HSCHX_DUTY;
		union {
			struct {
				unsigned int duty_scale_hsch0: 10;
				unsigned int duty_cycle_hsch0: 10;
				unsigned int duty_num_hsch0: 10;
				unsigned int duty_inc_hsch0: 1;
				unsigned int duty_start_hsch0: 1;
			} ;
			uint32_t val;
		} LEDC_HSCHX_CONF1;
		union {
			struct {
				unsigned int duty_hsch0: 25;
				unsigned int reserved25: 7;
			} ;
			uint32_t val;
		} LEDC_HSCHX_DUTY_R;
	}ledch_channel_config[8];
	struct ledcL_channel_config{
		union {
			struct {
				unsigned int timer_sel_lsch0: 2;
				unsigned int sig_out_en_lsch0: 1;
				unsigned int idle_lv_lsch0: 1;
				unsigned int para_up_lsch0: 1;
				unsigned int reserved5: 27;
			} ;
			uint32_t val;
		} LEDC_LSCHX_CONF0;
		union {
			struct {
				unsigned int hpoint_lsch0: 20;
				unsigned int reserved20: 12;
			} ;
			uint32_t val;
		} LEDC_LSCHX_HPOINT;
		union {
			struct {
				unsigned int duty_lsch0: 25;
				unsigned int reserved25: 7;
			} ;
			uint32_t val;
		} LEDC_LSCHX_DUTY;
		union {
			struct {
				unsigned int duty_scale_lsch0: 10;
				unsigned int duty_cycle_lsch0: 10;
				unsigned int duty_num_lsch0: 10;
				unsigned int duty_inc_lsch0: 1;
				unsigned int duty_start_lsch0: 1;
			} ;
			uint32_t val;
		} LEDC_LSCHX_CONF1;
		union {
			struct {
				unsigned int duty_lsch0: 25;
				unsigned int reserved25: 7;
			} ;
			uint32_t val;
		} LEDC_LSCHX_DUTY_R;
	}ledcl_channel_config[8];
	struct ledcH_time_config{
		union {
			struct {
				unsigned int hstimer0_lim: 5;
				unsigned int div_num_hstimer0: 18;
				unsigned int hstimer0_pause: 1;
				unsigned int hstimer0_rst: 1;
				unsigned int tick_sel_hstimer0: 1;
				unsigned int reserved26: 6;
			} ;
			uint32_t val;
		} LEDC_HSTIMERX_CONF;
		union {
			struct {
				unsigned int hstimer0_cnt: 20;
				unsigned int reserved20: 12;
			} ;
			uint32_t val;
		} LEDC_HSTIMERX_VALUE;
	}ledch_time_config[4];
	struct ledcL_timer_config{
		union {
			struct {
				unsigned int lstimer0_lim: 5;
				unsigned int div_num_lstimer0: 18;
				unsigned int lstimer0_pause: 1;
				unsigned int lstimer0_rst: 1;
				unsigned int tick_sel_lstimer0: 1;
				unsigned int lstimer0_para_up: 1;
				unsigned int reserved27: 5;
			} ;
			uint32_t val;
		} LEDC_LSTIMERX_CONF;
		union {
			struct {
				unsigned int lstimer0_cnt: 20;
				unsigned int reserved20: 12;
			} ;
			uint32_t val;
		} LEDC_LSTIMERX_VALUE;
		
	}ledcl_time_config[4];
		union {
			struct {
				unsigned int hstimer0_ovf_int_raw: 1;
				unsigned int hstimer1_ovf_int_raw: 1;
				unsigned int hstimer2_ovf_int_raw: 1;
				unsigned int hstimer3_ovf_int_raw: 1;
				unsigned int lstimer0_ovf_int_raw: 1;
				unsigned int lstimer1_ovf_int_raw: 1;
				unsigned int lstimer2_ovf_int_raw: 1;
				unsigned int lstimer3_ovf_int_raw: 1;
				unsigned int duty_chng_end_hsch0_int_raw: 1;
				unsigned int duty_chng_end_hsch1_int_raw: 1;
				unsigned int duty_chng_end_hsch2_int_raw: 1;
				unsigned int duty_chng_end_hsch3_int_raw: 1;
				unsigned int duty_chng_end_hsch4_int_raw: 1;
				unsigned int duty_chng_end_hsch5_int_raw: 1;
				unsigned int duty_chng_end_hsch6_int_raw: 1;
				unsigned int duty_chng_end_hsch7_int_raw: 1;
				unsigned int duty_chng_end_lsch0_int_raw: 1;
				unsigned int duty_chng_end_lsch1_int_raw: 1;
				unsigned int duty_chng_end_lsch2_int_raw: 1;
				unsigned int duty_chng_end_lsch3_int_raw: 1;
				unsigned int duty_chng_end_lsch4_int_raw: 1;
				unsigned int duty_chng_end_lsch5_int_raw: 1;
				unsigned int duty_chng_end_lsch6_int_raw: 1;
				unsigned int duty_chng_end_lsch7_int_raw: 1;
				unsigned int reserved24: 8;
			} ;
			uint32_t val;
		} LEDC_INT_RAW;
		union {
			struct {
				unsigned int hstimer0_ovf_int_st: 1;
				unsigned int hstimer1_ovf_int_st: 1;
				unsigned int hstimer2_ovf_int_st: 1;
				unsigned int hstimer3_ovf_int_st: 1;
				unsigned int lstimer0_ovf_int_st: 1;
				unsigned int lstimer1_ovf_int_st: 1;
				unsigned int lstimer2_ovf_int_st: 1;
				unsigned int lstimer3_ovf_int_st: 1;
				unsigned int duty_chng_end_hsch0_int_st: 1;
				unsigned int duty_chng_end_hsch1_int_st: 1;
				unsigned int duty_chng_end_hsch2_int_st: 1;
				unsigned int duty_chng_end_hsch3_int_st: 1;
				unsigned int duty_chng_end_hsch4_int_st: 1;
				unsigned int duty_chng_end_hsch5_int_st: 1;
				unsigned int duty_chng_end_hsch6_int_st: 1;
				unsigned int duty_chng_end_hsch7_int_st: 1;
				unsigned int duty_chng_end_lsch0_int_st: 1;
				unsigned int duty_chng_end_lsch1_int_st: 1;
				unsigned int duty_chng_end_lsch2_int_st: 1;
				unsigned int duty_chng_end_lsch3_int_st: 1;
				unsigned int duty_chng_end_lsch4_int_st: 1;
				unsigned int duty_chng_end_lsch5_int_st: 1;
				unsigned int duty_chng_end_lsch6_int_st: 1;
				unsigned int duty_chng_end_lsch7_int_st: 1;
				unsigned int reserved24: 8;
			} ;
			uint32_t val;
		} LEDC_INT_ST;
		union {
			struct {
				unsigned int hstimer0_ovf_int_ena: 1;
				unsigned int hstimer1_ovf_int_ena: 1;
				unsigned int hstimer2_ovf_int_ena: 1;
				unsigned int hstimer3_ovf_int_ena: 1;
				unsigned int lstimer0_ovf_int_ena: 1;
				unsigned int lstimer1_ovf_int_ena: 1;
				unsigned int lstimer2_ovf_int_ena: 1;
				unsigned int lstimer3_ovf_int_ena: 1;
				unsigned int duty_chng_end_hsch0_int_ena: 1;
				unsigned int duty_chng_end_hsch1_int_ena: 1;
				unsigned int duty_chng_end_hsch2_int_ena: 1;
				unsigned int duty_chng_end_hsch3_int_ena: 1;
				unsigned int duty_chng_end_hsch4_int_ena: 1;
				unsigned int duty_chng_end_hsch5_int_ena: 1;
				unsigned int duty_chng_end_hsch6_int_ena: 1;
				unsigned int duty_chng_end_hsch7_int_ena: 1;
				unsigned int duty_chng_end_lsch0_int_ena: 1;
				unsigned int duty_chng_end_lsch1_int_ena: 1;
				unsigned int duty_chng_end_lsch2_int_ena: 1;
				unsigned int duty_chng_end_lsch3_int_ena: 1;
				unsigned int duty_chng_end_lsch4_int_ena: 1;
				unsigned int duty_chng_end_lsch5_int_ena: 1;
				unsigned int duty_chng_end_lsch6_int_ena: 1;
				unsigned int duty_chng_end_lsch7_int_ena: 1;
				unsigned int reserved24: 8;
			} ;
			uint32_t val;
		} LEDC_INT_ENA;
		union {
			struct {
				unsigned int hstimer0_ovf_int_clr: 1;
				unsigned int hstimer1_ovf_int_clr: 1;
				unsigned int hstimer2_ovf_int_clr: 1;
				unsigned int hstimer3_ovf_int_clr: 1;
				unsigned int lstimer0_ovf_int_clr: 1;
				unsigned int lstimer1_ovf_int_clr: 1;
				unsigned int lstimer2_ovf_int_clr: 1;
				unsigned int lstimer3_ovf_int_clr: 1;
				unsigned int duty_chng_end_hsch0_int_clr: 1;
				unsigned int duty_chng_end_hsch1_int_clr: 1;
				unsigned int duty_chng_end_hsch2_int_clr: 1;
				unsigned int duty_chng_end_hsch3_int_clr: 1;
				unsigned int duty_chng_end_hsch4_int_clr: 1;
				unsigned int duty_chng_end_hsch5_int_clr: 1;
				unsigned int duty_chng_end_hsch6_int_clr: 1;
				unsigned int duty_chng_end_hsch7_int_clr: 1;
				unsigned int duty_chng_end_lsch0_int_clr: 1;
				unsigned int duty_chng_end_lsch1_int_clr: 1;
				unsigned int duty_chng_end_lsch2_int_clr: 1;
				unsigned int duty_chng_end_lsch3_int_clr: 1;
				unsigned int duty_chng_end_lsch4_int_clr: 1;
				unsigned int duty_chng_end_lsch5_int_clr: 1;
				unsigned int duty_chng_end_lsch6_int_clr: 1;
				unsigned int duty_chng_end_lsch7_int_clr: 1;
				unsigned int reserved24: 8;
			} ;
			uint32_t val;
		} LEDC_INT_CLR;
		union {
		struct {
			unsigned int apb_clk_sel: 1;
			unsigned int reserved1: 31;
		} ;
		uint32_t val;
	} LEDC_CONF;
	uint32_t reserved_194;
	uint32_t reserved_198;
	uint32_t reserved_19c;
	uint32_t reserved_1a0;
	uint32_t reserved_1a4;
	uint32_t reserved_1a8;
	uint32_t reserved_1ac;
	uint32_t reserved_1b0;
	uint32_t reserved_1b4;
	uint32_t reserved_1b8;
	uint32_t reserved_1bc;
	uint32_t reserved_1c0;
	uint32_t reserved_1c4;
	uint32_t reserved_1c8;
	uint32_t reserved_1cc;
	uint32_t reserved_1d0;
	uint32_t reserved_1d4;
	uint32_t reserved_1d8;
	uint32_t reserved_1dc;
	uint32_t reserved_1e0;
	uint32_t reserved_1e4;
	uint32_t reserved_1e8;
	uint32_t reserved_1ec;
	uint32_t reserved_1f0;
	uint32_t reserved_1f4;
	uint32_t reserved_1f8;
	struct {
		uint32_t val;
	} LEDC_DATE1;
}ledc_reg;

#endif /* _DRIVER_LEDC_PWM_STRUCT_H_ */
