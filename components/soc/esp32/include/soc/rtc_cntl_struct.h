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
#ifndef _SOC_RTC_CNTL_STRUCT_H_
#define _SOC_RTC_CNTL_STRUCT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct {
    union {
        struct {
            uint32_t sw_stall_appcpu_c0:  2;             /*{reg_sw_stall_appcpu_c1[5:0]   reg_sw_stall_appcpu_c0[1:0]}  == 0x86 will stall APP CPU*/
            uint32_t sw_stall_procpu_c0:  2;             /*{reg_sw_stall_procpu_c1[5:0]   reg_sw_stall_procpu_c0[1:0]}  == 0x86 will stall PRO CPU*/
            uint32_t sw_appcpu_rst:       1;             /*APP CPU SW reset*/
            uint32_t sw_procpu_rst:       1;             /*PRO CPU SW reset*/
            uint32_t bb_i2c_force_pd:     1;             /*BB_I2C force power down*/
            uint32_t bb_i2c_force_pu:     1;             /*BB_I2C force power up*/
            uint32_t bbpll_i2c_force_pd:  1;             /*BB_PLL _I2C force power down*/
            uint32_t bbpll_i2c_force_pu:  1;             /*BB_PLL_I2C force power up*/
            uint32_t bbpll_force_pd:      1;             /*BB_PLL force power down*/
            uint32_t bbpll_force_pu:      1;             /*BB_PLL force power up*/
            uint32_t xtl_force_pd:        1;             /*crystall force power down*/
            uint32_t xtl_force_pu:        1;             /*crystall force power up*/
            uint32_t bias_sleep_folw_8m:  1;             /*BIAS_SLEEP follow CK8M*/
            uint32_t bias_force_sleep:    1;             /*BIAS_SLEEP force sleep*/
            uint32_t bias_force_nosleep:  1;             /*BIAS_SLEEP force no sleep*/
            uint32_t bias_i2c_folw_8m:    1;             /*BIAS_I2C follow CK8M*/
            uint32_t bias_i2c_force_pd:   1;             /*BIAS_I2C force power down*/
            uint32_t bias_i2c_force_pu:   1;             /*BIAS_I2C force power up*/
            uint32_t bias_core_folw_8m:   1;             /*BIAS_CORE follow CK8M*/
            uint32_t bias_core_force_pd:  1;             /*BIAS_CORE force power down*/
            uint32_t bias_core_force_pu:  1;             /*BIAS_CORE force power up*/
            uint32_t xtl_force_iso:       1;
            uint32_t pll_force_iso:       1;
            uint32_t analog_force_iso:    1;
            uint32_t xtl_force_noiso:     1;
            uint32_t pll_force_noiso:     1;
            uint32_t analog_force_noiso:  1;
            uint32_t dg_wrap_force_rst:   1;             /*digital wrap force reset in deep sleep*/
            uint32_t dg_wrap_force_norst: 1;             /*digital core force no reset in deep sleep*/
            uint32_t sw_sys_rst:          1;             /*SW system reset*/
        };
        uint32_t val;
    } options0;
    uint32_t slp_timer0;                             /*RTC sleep timer low 32 bits*/
    union {
        struct {
            uint32_t slp_val_hi:             16;         /*RTC sleep timer high 16 bits*/
            uint32_t main_timer_alarm_en: 1;         /*timer alarm enable bit*/
            uint32_t reserved17:             15;
        };
        uint32_t val;
    } slp_timer1;
    union {
        struct {
            uint32_t reserved0:      30;
            uint32_t valid:  1;                 /*To indicate the register is updated*/
            uint32_t update: 1;                 /*Set 1: to update register with RTC timer*/
        };
        uint32_t val;
    } time_update;
    uint32_t time0;                                  /*RTC timer low 32 bits*/
    union {
        struct {
            uint32_t time_hi:16;                     /*RTC timer high 16 bits*/
            uint32_t reserved16: 16;
        };
        uint32_t val;
    } time1;
    union {
        struct {
            uint32_t reserved0:             20;
            uint32_t touch_wakeup_force_en:  1;          /*touch controller force wake up*/
            uint32_t ulp_cp_wakeup_force_en: 1;          /*ULP-coprocessor force wake up*/
            uint32_t apb2rtc_bridge_sel:     1;          /*1: APB to RTC using bridge   0: APB to RTC using sync*/
            uint32_t touch_slp_timer_en:     1;          /*touch timer enable bit*/
            uint32_t ulp_cp_slp_timer_en:    1;          /*ULP-coprocessor timer enable bit*/
            uint32_t reserved25:             3;
            uint32_t sdio_active_ind:        1;          /*SDIO active indication*/
            uint32_t slp_wakeup:             1;          /*sleep wakeup bit*/
            uint32_t slp_reject:             1;          /*sleep reject bit*/
            uint32_t sleep_en:               1;          /*sleep enable bit*/
        };
        uint32_t val;
    } state0;
    union {
        struct {
            uint32_t cpu_stall_en:   1;                  /*CPU stall enable bit*/
            uint32_t cpu_stall_wait: 5;                  /*CPU stall wait cycles in fast_clk_rtc*/
            uint32_t ck8m_wait:      8;                  /*CK8M wait cycles in slow_clk_rtc*/
            uint32_t xtl_buf_wait:  10;                  /*XTAL wait cycles in slow_clk_rtc*/
            uint32_t pll_buf_wait:   8;                  /*PLL wait cycles in slow_clk_rtc*/
        };
        uint32_t val;
    } timer1;
    union {
        struct {
            uint32_t reserved0:             15;
            uint32_t ulpcp_touch_start_wait: 9;          /*wait cycles in slow_clk_rtc before ULP-coprocessor / touch controller start to work*/
            uint32_t min_time_ck8m_off:      8;          /*minimal cycles in slow_clk_rtc for CK8M in power down state*/
        };
        uint32_t val;
    } timer2;
    union {
        struct {
            uint32_t wifi_wait_timer:       9;
            uint32_t wifi_powerup_timer:    7;
            uint32_t rom_ram_wait_timer:    9;
            uint32_t rom_ram_powerup_timer: 7;
        };
        uint32_t val;
    } timer3;
    union {
        struct {
            uint32_t rtc_wait_timer:        9;
            uint32_t rtc_powerup_timer:     7;
            uint32_t dg_wrap_wait_timer:    9;
            uint32_t dg_wrap_powerup_timer: 7;
        };
        uint32_t val;
    } timer4;
    union {
        struct {
            uint32_t ulp_cp_subtimer_prediv: 8;
            uint32_t min_slp_val:            8;          /*minimal sleep cycles in slow_clk_rtc*/
            uint32_t rtcmem_wait_timer:      9;
            uint32_t rtcmem_powerup_timer:   7;
        };
        uint32_t val;
    } timer5;
    union {
        struct {
            uint32_t reserved0:          23;
            uint32_t plla_force_pd:       1;             /*PLLA force power down*/
            uint32_t plla_force_pu:       1;             /*PLLA force power up*/
            uint32_t bbpll_cal_slp_start: 1;             /*start BBPLL calibration during sleep*/
            uint32_t pvtmon_pu:           1;             /*1: PVTMON power up   otherwise power down*/
            uint32_t txrf_i2c_pu:         1;             /*1: TXRF_I2C power up   otherwise power down*/
            uint32_t rfrx_pbus_pu:        1;             /*1: RFRX_PBUS power up   otherwise power down*/
            uint32_t reserved29:          1;
            uint32_t ckgen_i2c_pu:        1;             /*1: CKGEN_I2C power up   otherwise power down*/
            uint32_t pll_i2c_pu:          1;             /*1: PLL_I2C power up   otherwise power down*/
        };
        uint32_t val;
    } ana_conf;
    union {
        struct {
            uint32_t reset_cause_procpu:     6;          /*reset cause of PRO CPU*/
            uint32_t reset_cause_appcpu:     6;          /*reset cause of APP CPU*/
            uint32_t appcpu_stat_vector_sel: 1;          /*APP CPU state vector sel*/
            uint32_t procpu_stat_vector_sel: 1;          /*PRO CPU state vector sel*/
            uint32_t reserved14:            18;
        };
        uint32_t val;
    } reset_state;
    union {
        struct {
            uint32_t wakeup_cause:      11;              /*wakeup cause*/
            uint32_t rtc_wakeup_ena:    11;              /*wakeup enable bitmap*/
            uint32_t gpio_wakeup_filter: 1;              /*enable filter for gpio wakeup event*/
            uint32_t reserved23:         9;
        };
        uint32_t val;
    } wakeup_state;
    union {
        struct {
            uint32_t slp_wakeup:     1;          /*enable sleep wakeup interrupt*/
            uint32_t slp_reject:     1;          /*enable sleep reject interrupt*/
            uint32_t sdio_idle:      1;          /*enable SDIO idle interrupt*/
            uint32_t rtc_wdt:        1;          /*enable RTC WDT interrupt*/
            uint32_t rtc_time_valid: 1;          /*enable RTC time valid interrupt*/
            uint32_t rtc_ulp_cp:     1;          /*enable ULP-coprocessor interrupt*/
            uint32_t rtc_touch:      1;          /*enable touch interrupt*/
            uint32_t rtc_brown_out:  1;          /*enable brown out interrupt*/
            uint32_t rtc_main_timer: 1;          /*enable RTC main timer interrupt*/
            uint32_t reserved9:             23;
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t slp_wakeup:     1;          /*sleep wakeup interrupt raw*/
            uint32_t slp_reject:     1;          /*sleep reject interrupt raw*/
            uint32_t sdio_idle:      1;          /*SDIO idle interrupt raw*/
            uint32_t rtc_wdt:        1;          /*RTC WDT interrupt raw*/
            uint32_t rtc_time_valid: 1;          /*RTC time valid interrupt raw*/
            uint32_t rtc_ulp_cp:     1;          /*ULP-coprocessor interrupt raw*/
            uint32_t rtc_touch:      1;          /*touch interrupt raw*/
            uint32_t rtc_brown_out:  1;          /*brown out interrupt raw*/
            uint32_t rtc_main_timer: 1;          /*RTC main timer interrupt raw*/
            uint32_t reserved9:             23;
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t slp_wakeup:     1;           /*sleep wakeup interrupt state*/
            uint32_t slp_reject:     1;           /*sleep reject interrupt state*/
            uint32_t sdio_idle:      1;           /*SDIO idle interrupt state*/
            uint32_t rtc_wdt:        1;           /*RTC WDT interrupt state*/
            uint32_t rtc_time_valid: 1;           /*RTC time valid interrupt state*/
            uint32_t rtc_sar:        1;           /*ULP-coprocessor interrupt state*/
            uint32_t rtc_touch:      1;           /*touch interrupt state*/
            uint32_t rtc_brown_out:  1;           /*brown out interrupt state*/
            uint32_t rtc_main_timer: 1;           /*RTC main timer interrupt state*/
            uint32_t reserved9:            23;
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t slp_wakeup:     1;          /*Clear sleep wakeup interrupt state*/
            uint32_t slp_reject:     1;          /*Clear sleep reject interrupt state*/
            uint32_t sdio_idle:      1;          /*Clear SDIO idle interrupt state*/
            uint32_t rtc_wdt:        1;          /*Clear RTC WDT interrupt state*/
            uint32_t rtc_time_valid: 1;          /*Clear RTC time valid interrupt state*/
            uint32_t rtc_sar:        1;          /*Clear ULP-coprocessor interrupt state*/
            uint32_t rtc_touch:      1;          /*Clear touch interrupt state*/
            uint32_t rtc_brown_out:  1;          /*Clear brown out interrupt state*/
            uint32_t rtc_main_timer: 1;          /*Clear RTC main timer interrupt state*/
            uint32_t reserved9:             23;
        };
        uint32_t val;
    } int_clr;
    uint32_t rtc_store0;                                 /*32-bit general purpose retention register*/
    uint32_t rtc_store1;                                 /*32-bit general purpose retention register*/
    uint32_t rtc_store2;                                 /*32-bit general purpose retention register*/
    uint32_t rtc_store3;                                 /*32-bit general purpose retention register*/
    union {
        struct {
            uint32_t reserved0:     30;
            uint32_t ctr_lv: 1;                  /*0: power down XTAL at high level  1: power down XTAL at low level*/
            uint32_t ctr_en: 1;                  /*enable control XTAL by external pads*/
        };
        uint32_t val;
    } ext_xtl_conf;
    union {
        struct {
            uint32_t reserved0:     30;
            uint32_t wakeup0_lv: 1;                  /*0: external wakeup at low level  1: external wakeup at high level*/
            uint32_t wakeup1_lv: 1;                  /*0: external wakeup at low level  1: external wakeup at high level*/
        };
        uint32_t val;
    } ext_wakeup_conf;
    union {
        struct {
            uint32_t reserved0:          24;
            uint32_t gpio_reject_en:      1;             /*enable GPIO reject*/
            uint32_t sdio_reject_en:      1;             /*enable SDIO reject*/
            uint32_t light_slp_reject_en: 1;             /*enable reject for light sleep*/
            uint32_t deep_slp_reject_en:  1;             /*enable reject for deep sleep*/
            uint32_t reject_cause:        4;             /*sleep reject cause*/
        };
        uint32_t val;
    } slp_reject_conf;
    union {
        struct {
            uint32_t reserved0:        29;
            uint32_t cpusel_conf:   1;               /*CPU sel option*/
            uint32_t cpuperiod_sel: 2;               /*CPU period sel*/
        };
        uint32_t val;
    } cpu_period_conf;
    union {
        struct {
            uint32_t reserved0:    22;
            uint32_t sdio_act_dnum:10;
        };
        uint32_t val;
    } sdio_act_conf;
    union {
        struct {
            uint32_t reserved0:           4;
            uint32_t ck8m_div:            2;             /*CK8M_D256_OUT divider. 00: div128  01: div256  10: div512  11: div1024.*/
            uint32_t enb_ck8m:            1;             /*disable CK8M and CK8M_D256_OUT*/
            uint32_t enb_ck8m_div:        1;             /*1: CK8M_D256_OUT is actually CK8M  0: CK8M_D256_OUT is CK8M divided by 256*/
            uint32_t dig_xtal32k_en:      1;             /*enable CK_XTAL_32K for digital core (no relationship with RTC core)*/
            uint32_t dig_clk8m_d256_en:   1;             /*enable CK8M_D256_OUT for digital core (no relationship with RTC core)*/
            uint32_t dig_clk8m_en:        1;             /*enable CK8M for digital core (no relationship with RTC core)*/
            uint32_t ck8m_dfreq_force:    1;
            uint32_t ck8m_div_sel:        3;             /*divider = reg_ck8m_div_sel + 1*/
            uint32_t xtal_force_nogating: 1;             /*XTAL force no gating during sleep*/
            uint32_t ck8m_force_nogating: 1;             /*CK8M force no gating during sleep*/
            uint32_t ck8m_dfreq:          8;             /*CK8M_DFREQ*/
            uint32_t ck8m_force_pd:       1;             /*CK8M force power down*/
            uint32_t ck8m_force_pu:       1;             /*CK8M force power up*/
            uint32_t soc_clk_sel:         2;             /*SOC clock sel. 0: XTAL  1: PLL  2: CK8M  3: APLL*/
            uint32_t fast_clk_rtc_sel:    1;             /*fast_clk_rtc sel. 0: XTAL div 4  1: CK8M*/
            uint32_t ana_clk_rtc_sel:     2;             /*slow_clk_rtc sel. 0: SLOW_CK  1: CK_XTAL_32K  2: CK8M_D256_OUT*/
        };
        uint32_t val;
    } clk_conf;
    union {
        struct {
            uint32_t reserved0:   21;
            uint32_t sdio_pd_en:   1;                    /*power down SDIO_REG in sleep. Only active when reg_sdio_force = 0*/
            uint32_t sdio_force:   1;                    /*1: use SW option to control SDIO_REG  0: use state machine*/
            uint32_t sdio_tieh:    1;                    /*SW option for SDIO_TIEH. Only active when reg_sdio_force = 1*/
            uint32_t reg1p8_ready: 1;                    /*read only register for REG1P8_READY*/
            uint32_t drefl_sdio:   2;                    /*SW option for DREFL_SDIO. Only active when reg_sdio_force = 1*/
            uint32_t drefm_sdio:   2;                    /*SW option for DREFM_SDIO. Only active when reg_sdio_force = 1*/
            uint32_t drefh_sdio:   2;                    /*SW option for DREFH_SDIO. Only active when reg_sdio_force = 1*/
            uint32_t xpd_sdio:     1;                    /*SW option for XPD_SDIO_REG. Only active when reg_sdio_force = 1*/
        };
        uint32_t val;
    } sdio_conf;
    union {
        struct {
            uint32_t reserved0:            24;
            uint32_t dbg_atten:             2;           /*DBG_ATTEN*/
            uint32_t enb_sck_xtal:          1;           /*ENB_SCK_XTAL*/
            uint32_t inc_heartbeat_refresh: 1;           /*INC_HEARTBEAT_REFRESH*/
            uint32_t dec_heartbeat_period:  1;           /*DEC_HEARTBEAT_PERIOD*/
            uint32_t inc_heartbeat_period:  1;           /*INC_HEARTBEAT_PERIOD*/
            uint32_t dec_heartbeat_width:   1;           /*DEC_HEARTBEAT_WIDTH*/
            uint32_t rst_bias_i2c:          1;           /*RST_BIAS_I2C*/
        };
        uint32_t val;
    } bias_conf;
    union {
        struct {
            uint32_t reserved0:           7;
            uint32_t sck_dcap_force:      1;             /*N/A*/
            uint32_t dig_dbias_slp:       3;             /*DIG_REG_DBIAS during sleep*/
            uint32_t dig_dbias_wak:       3;             /*DIG_REG_DBIAS during wakeup*/
            uint32_t sck_dcap:            8;             /*SCK_DCAP*/
            uint32_t rtc_dbias_slp:       3;             /*RTC_DBIAS during sleep*/
            uint32_t rtc_dbias_wak:       3;             /*RTC_DBIAS during wakeup*/
            uint32_t rtc_dboost_force_pd: 1;             /*RTC_DBOOST force power down*/
            uint32_t rtc_dboost_force_pu: 1;             /*RTC_DBOOST force power up*/
            uint32_t rtc_force_pd:        1;             /*RTC_REG force power down (for RTC_REG power down means decrease the voltage to 0.8v or lower )*/
            uint32_t rtc_force_pu:        1;             /*RTC_REG force power up*/
        };
        uint32_t val;
    } rtc;
    union {
        struct {
            uint32_t fastmem_force_noiso: 1;         /*Fast RTC memory force no ISO*/
            uint32_t fastmem_force_iso:   1;         /*Fast RTC memory force ISO*/
            uint32_t slowmem_force_noiso: 1;         /*RTC memory force no ISO*/
            uint32_t slowmem_force_iso:   1;         /*RTC memory force ISO*/
            uint32_t rtc_force_iso:           1;         /*rtc_peri force ISO*/
            uint32_t force_noiso:         1;         /*rtc_peri force no ISO*/
            uint32_t fastmem_folw_cpu:    1;         /*1: Fast RTC memory PD following CPU  0: fast RTC memory PD following RTC state machine*/
            uint32_t fastmem_force_lpd:   1;         /*Fast RTC memory force PD*/
            uint32_t fastmem_force_lpu:   1;         /*Fast RTC memory force no PD*/
            uint32_t slowmem_folw_cpu:    1;         /*1: RTC memory  PD following CPU  0: RTC memory PD following RTC state machine*/
            uint32_t slowmem_force_lpd:   1;         /*RTC memory force PD*/
            uint32_t slowmem_force_lpu:   1;         /*RTC memory force no PD*/
            uint32_t fastmem_force_pd:    1;         /*Fast RTC memory force power down*/
            uint32_t fastmem_force_pu:    1;         /*Fast RTC memory force power up*/
            uint32_t fastmem_pd_en:       1;         /*enable power down fast RTC memory in sleep*/
            uint32_t slowmem_force_pd:    1;         /*RTC memory force power down*/
            uint32_t slowmem_force_pu:    1;         /*RTC memory force power up*/
            uint32_t slowmem_pd_en:       1;         /*enable power down RTC memory in sleep*/
            uint32_t pwc_force_pd:        1;         /*rtc_peri force power down*/
            uint32_t pwc_force_pu:        1;         /*rtc_peri force power up*/
            uint32_t pd_en:               1;         /*enable power down rtc_peri in sleep*/
            uint32_t reserved21:             11;
        };
        uint32_t val;
    } rtc_pwc;
    union {
        struct {
            uint32_t reserved0:           3;
            uint32_t lslp_mem_force_pd:   1;             /*memories in digital core force PD in sleep*/
            uint32_t lslp_mem_force_pu:   1;             /*memories in digital core force no PD in sleep*/
            uint32_t rom0_force_pd:       1;             /*ROM force power down*/
            uint32_t rom0_force_pu:       1;             /*ROM force power up*/
            uint32_t inter_ram0_force_pd: 1;             /*internal SRAM 0 force power down*/
            uint32_t inter_ram0_force_pu: 1;             /*internal SRAM 0 force power up*/
            uint32_t inter_ram1_force_pd: 1;             /*internal SRAM 1 force power down*/
            uint32_t inter_ram1_force_pu: 1;             /*internal SRAM 1 force power up*/
            uint32_t inter_ram2_force_pd: 1;             /*internal SRAM 2 force power down*/
            uint32_t inter_ram2_force_pu: 1;             /*internal SRAM 2 force power up*/
            uint32_t inter_ram3_force_pd: 1;             /*internal SRAM 3 force power down*/
            uint32_t inter_ram3_force_pu: 1;             /*internal SRAM 3 force power up*/
            uint32_t inter_ram4_force_pd: 1;             /*internal SRAM 4 force power down*/
            uint32_t inter_ram4_force_pu: 1;             /*internal SRAM 4 force power up*/
            uint32_t wifi_force_pd:       1;             /*wifi force power down*/
            uint32_t wifi_force_pu:       1;             /*wifi force power up*/
            uint32_t dg_wrap_force_pd:    1;             /*digital core force power down*/
            uint32_t dg_wrap_force_pu:    1;             /*digital core force power up*/
            uint32_t reserved21:          3;
            uint32_t rom0_pd_en:          1;             /*enable power down ROM in sleep*/
            uint32_t inter_ram0_pd_en:    1;             /*enable power down internal SRAM 0 in sleep*/
            uint32_t inter_ram1_pd_en:    1;             /*enable power down internal SRAM 1 in sleep*/
            uint32_t inter_ram2_pd_en:    1;             /*enable power down internal SRAM 2 in sleep*/
            uint32_t inter_ram3_pd_en:    1;             /*enable power down internal SRAM 3 in sleep*/
            uint32_t inter_ram4_pd_en:    1;             /*enable power down internal SRAM 4 in sleep*/
            uint32_t wifi_pd_en:          1;             /*enable power down wifi in sleep*/
            uint32_t dg_wrap_pd_en:       1;             /*enable power down digital core in sleep*/
        };
        uint32_t val;
    } dig_pwc;
    union {
        struct {
            uint32_t reserved0:              7;
            uint32_t dig_iso_force_off:      1;
            uint32_t dig_iso_force_on:       1;
            uint32_t dg_pad_autohold:        1;          /*read only register to indicate digital pad auto-hold status*/
            uint32_t clr_dg_pad_autohold:    1;          /*wtite only register to clear digital pad auto-hold*/
            uint32_t dg_pad_autohold_en:     1;          /*digital pad enable auto-hold*/
            uint32_t dg_pad_force_noiso:     1;          /*digital pad force no ISO*/
            uint32_t dg_pad_force_iso:       1;          /*digital pad force ISO*/
            uint32_t dg_pad_force_unhold:    1;          /*digital pad force un-hold*/
            uint32_t dg_pad_force_hold:      1;          /*digital pad force hold*/
            uint32_t rom0_force_iso:         1;          /*ROM force ISO*/
            uint32_t rom0_force_noiso:       1;          /*ROM force no ISO*/
            uint32_t inter_ram0_force_iso:   1;          /*internal SRAM 0 force ISO*/
            uint32_t inter_ram0_force_noiso: 1;          /*internal SRAM 0 force no ISO*/
            uint32_t inter_ram1_force_iso:   1;          /*internal SRAM 1 force ISO*/
            uint32_t inter_ram1_force_noiso: 1;          /*internal SRAM 1 force no ISO*/
            uint32_t inter_ram2_force_iso:   1;          /*internal SRAM 2 force ISO*/
            uint32_t inter_ram2_force_noiso: 1;          /*internal SRAM 2 force no ISO*/
            uint32_t inter_ram3_force_iso:   1;          /*internal SRAM 3 force ISO*/
            uint32_t inter_ram3_force_noiso: 1;          /*internal SRAM 3 force no ISO*/
            uint32_t inter_ram4_force_iso:   1;          /*internal SRAM 4 force ISO*/
            uint32_t inter_ram4_force_noiso: 1;          /*internal SRAM 4 force no ISO*/
            uint32_t wifi_force_iso:         1;          /*wifi force ISO*/
            uint32_t wifi_force_noiso:       1;          /*wifi force no ISO*/
            uint32_t dg_wrap_force_iso:      1;          /*digital core force ISO*/
            uint32_t dg_wrap_force_noiso:    1;          /*digital core force no ISO*/
        };
        uint32_t val;
    } dig_iso;
    union {
        struct {
            uint32_t reserved0:            7;
            uint32_t pause_in_slp:     1;            /*pause WDT in sleep*/
            uint32_t appcpu_reset_en:  1;            /*enable WDT reset APP CPU*/
            uint32_t procpu_reset_en:  1;            /*enable WDT reset PRO CPU*/
            uint32_t flashboot_mod_en: 1;            /*enable WDT in flash boot*/
            uint32_t sys_reset_length: 3;            /*system reset counter length*/
            uint32_t cpu_reset_length: 3;            /*CPU reset counter length*/
            uint32_t level_int_en:     1;            /*N/A*/
            uint32_t edge_int_en:      1;            /*N/A*/
            uint32_t stg3:             3;            /*1: interrupt stage en  2: CPU reset stage en  3: system reset stage en  4: RTC reset stage en*/
            uint32_t stg2:             3;            /*1: interrupt stage en  2: CPU reset stage en  3: system reset stage en  4: RTC reset stage en*/
            uint32_t stg1:             3;            /*1: interrupt stage en  2: CPU reset stage en  3: system reset stage en  4: RTC reset stage en*/
            uint32_t stg0:             3;            /*1: interrupt stage en  2: CPU reset stage en  3: system reset stage en  4: RTC reset stage en*/
            uint32_t en:               1;            /*enable RTC WDT*/
        };
        uint32_t val;
    } wdt_config0;
    uint32_t wdt_config1;                             /**/
    uint32_t wdt_config2;                             /**/
    uint32_t wdt_config3;                             /**/
    uint32_t wdt_config4;                             /**/
    union {
        struct {
            uint32_t reserved0:   31;
            uint32_t feed: 1;
        };
        uint32_t val;
    } wdt_feed;
    uint32_t wdt_wprotect;                            /**/
    union {
        struct {
            uint32_t reserved0: 29;
            uint32_t ent_rtc:    1;                      /*ENT_RTC*/
            uint32_t dtest_rtc:  2;                      /*DTEST_RTC*/
        };
        uint32_t val;
    } test_mux;
    union {
        struct {
            uint32_t reserved0:         20;
            uint32_t appcpu_c1: 6;              /*{reg_sw_stall_appcpu_c1[5:0]   reg_sw_stall_appcpu_c0[1:0]}  == 0x86 will stall APP CPU*/
            uint32_t procpu_c1: 6;              /*{reg_sw_stall_procpu_c1[5:0]   reg_sw_stall_procpu_c0[1:0]}  == 0x86 will stall PRO CPU*/
        };
        uint32_t val;
    } sw_cpu_stall;
    uint32_t store4;                                 /*32-bit general purpose retention register*/
    uint32_t store5;                                 /*32-bit general purpose retention register*/
    uint32_t store6;                                 /*32-bit general purpose retention register*/
    uint32_t store7;                                 /*32-bit general purpose retention register*/
    uint32_t diag0;                                  /**/
    uint32_t diag1;                                  /**/
    union {
        struct {
            uint32_t adc1_hold_force:       1;
            uint32_t adc2_hold_force:       1;
            uint32_t pdac1_hold_force:      1;
            uint32_t pdac2_hold_force:      1;
            uint32_t sense1_hold_force:     1;
            uint32_t sense2_hold_force:     1;
            uint32_t sense3_hold_force:     1;
            uint32_t sense4_hold_force:     1;
            uint32_t touch_pad0_hold_force: 1;
            uint32_t touch_pad1_hold_force: 1;
            uint32_t touch_pad2_hold_force: 1;
            uint32_t touch_pad3_hold_force: 1;
            uint32_t touch_pad4_hold_force: 1;
            uint32_t touch_pad5_hold_force: 1;
            uint32_t touch_pad6_hold_force: 1;
            uint32_t touch_pad7_hold_force: 1;
            uint32_t x32p_hold_force:       1;
            uint32_t x32n_hold_force:       1;
            uint32_t reserved18:           14;
        };
        uint32_t val;
    } hold_force;
    union {
        struct {
            uint32_t ext_wakeup1_sel:       18;          /*Bitmap to select RTC pads for ext wakeup1*/
            uint32_t ext_wakeup1_status_clr: 1;          /*clear ext wakeup1 status*/
            uint32_t reserved19:            13;
        };
        uint32_t val;
    } ext_wakeup1;
    union {
        struct {
            uint32_t ext_wakeup1_status:18;              /*ext wakeup1 status*/
            uint32_t reserved18:        14;
        };
        uint32_t val;
    } ext_wakeup1_status;
    union {
        struct {
            uint32_t reserved0:                14;
            uint32_t close_flash_ena: 1;       /*enable close flash when brown out happens*/
            uint32_t pd_rf_ena:       1;       /*enable power down RF when brown out happens*/
            uint32_t rst_wait:       10;       /*brown out reset wait cycles*/
            uint32_t rst_ena:         1;       /*enable brown out reset*/
            uint32_t thres:          3;       /*brown out threshold*/
            uint32_t ena:             1;       /*enable brown out*/
            uint32_t det:         1;       /*brown out detect*/
        };
        uint32_t val;
    } brown_out;
    uint32_t reserved_39;
    uint32_t reserved_3d;
    uint32_t reserved_41;
    uint32_t reserved_45;
    uint32_t reserved_49;
    uint32_t reserved_4d;
    union {
        struct {
            uint32_t date:      28;
            uint32_t reserved28: 4;
        };
        uint32_t val;
    } date;
} rtc_cntl_dev_t;
extern rtc_cntl_dev_t RTCCNTL;

#ifdef __cplusplus
}
#endif

#endif  /* _SOC_RTC_CNTL_STRUCT_H_ */
