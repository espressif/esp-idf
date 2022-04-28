// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
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
#ifndef _SOC_RTC_CNTL_STRUCT_H_
#define _SOC_RTC_CNTL_STRUCT_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef volatile struct rtc_cntl_dev_s {
    union {
        struct {
            uint32_t sw_stall_appcpu_c0:  2;             /*{reg_sw_stall_appcpu_c1[5:0]   reg_sw_stall_appcpu_c0[1:0]} == 0x86 will stall APP CPU*/
            uint32_t sw_stall_procpu_c0:  2;             /*{reg_sw_stall_procpu_c1[5:0]   reg_sw_stall_procpu_c0[1:0]} == 0x86 will stall PRO CPU*/
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
            uint32_t xtl_en_wait:         4;             /*wait bias_sleep and current source wakeup*/
            uint32_t reserved18:          5;
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
            uint32_t main_timer_alarm_en:     1;         /*timer alarm enable bit*/
            uint32_t reserved17:             15;
        };
        uint32_t val;
    } slp_timer1;
    union {
        struct {
            uint32_t reserved0:      27;
            uint32_t timer_sys_stall: 1;                 /*Enable to record system stall time*/
            uint32_t timer_xtl_off:   1;                 /*Enable to record 40M XTAL OFF time*/
            uint32_t timer_sys_rst:   1;                 /*enable to record system reset time*/
            uint32_t reserved30:      1;
            uint32_t update:          1;                 /*Set 1: to update register with RTC timer*/
        };
        uint32_t val;
    } time_update;
    uint32_t time_low0;                              /*RTC timer low 32 bits*/
    union {
        struct {
            uint32_t rtc_timer_value0_high:16;           /*RTC timer high 16 bits*/
            uint32_t reserved16:           16;
        };
        uint32_t val;
    } time_high0;
    union {
        struct {
            uint32_t rtc_sw_cpu_int:           1;        /*rtc software interrupt to main cpu*/
            uint32_t rtc_slp_reject_cause_clr: 1;        /*clear rtc sleep reject cause*/
            uint32_t reserved2:               20;
            uint32_t apb2rtc_bridge_sel:       1;        /*1: APB to RTC using bridge   0: APB to RTC using sync*/
            uint32_t reserved23:               5;
            uint32_t sdio_active_ind:          1;        /*SDIO active indication*/
            uint32_t slp_wakeup:               1;        /*leep wakeup bit*/
            uint32_t slp_reject:               1;        /*leep reject bit*/
            uint32_t sleep_en:                 1;        /*sleep enable bit*/
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
            uint32_t reserved0:            8;
            uint32_t min_slp_val:          8;            /*minimal sleep cycles in slow_clk_rtc*/
            uint32_t rtcmem_wait_timer:    9;
            uint32_t rtcmem_powerup_timer: 7;
        };
        uint32_t val;
    } timer5;
    union {
        struct {
            uint32_t reserved0:            16;
            uint32_t dg_dcdc_wait_timer:    9;
            uint32_t dg_dcdc_powerup_timer: 7;
        };
        uint32_t val;
    } timer6;
    union {
        struct {
            uint32_t reserved0:             18;
            uint32_t i2c_reset_por_force_pd: 1;
            uint32_t i2c_reset_por_force_pu: 1;
            uint32_t glitch_rst_en:          1;
            uint32_t sar_i2c_force_pd:       1;          /*PLLA force power down*/
            uint32_t sar_i2c_force_pu:       1;          /*PLLA force power up*/
            uint32_t plla_force_pd:          1;          /*PLLA force power down*/
            uint32_t plla_force_pu:          1;          /*PLLA force power up*/
            uint32_t bbpll_cal_slp_start:    1;          /*start BBPLL calibration during sleep*/
            uint32_t pvtmon_pu:              1;          /*1: PVTMON power up   otherwise power down*/
            uint32_t txrf_i2c_pu:            1;          /*1: TXRF_I2C power up   otherwise power down*/
            uint32_t rfrx_pbus_pu:           1;          /*1: RFRX_PBUS power up   otherwise power down*/
            uint32_t reserved29:             1;
            uint32_t ckgen_i2c_pu:           1;          /*1: CKGEN_I2C power up   otherwise power down*/
            uint32_t pll_i2c_pu:             1;
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
            uint32_t reserved0:     15;
            uint32_t rtc_wakeup_ena:17;                  /*wakeup enable bitmap*/
        };
        uint32_t val;
    } wakeup_state;
    union {
        struct {
            uint32_t slp_wakeup:                  1;     /*enable sleep wakeup interrupt*/
            uint32_t slp_reject:                  1;     /*enable sleep reject interrupt*/
            uint32_t sdio_idle:                   1;     /*enable SDIO idle interrupt*/
            uint32_t rtc_wdt:                     1;     /*enable RTC WDT interrupt*/
            uint32_t rtc_touch_scan_done:         1;     /*enable touch scan done interrupt*/
            uint32_t rtc_ulp_cp:                  1;     /*enable ULP-coprocessor interrupt*/
            uint32_t rtc_touch_done:              1;     /*enable touch done interrupt*/
            uint32_t rtc_touch_active:            1;     /*enable touch active interrupt*/
            uint32_t rtc_touch_inactive:          1;     /*enable touch inactive interrupt*/
            uint32_t rtc_brown_out:               1;     /*enable brown out interrupt*/
            uint32_t rtc_main_timer:              1;     /*enable RTC main timer interrupt*/
            uint32_t rtc_saradc1:                 1;     /*enable saradc1 interrupt*/
            uint32_t rtc_tsens:                   1;     /*enable tsens interrupt*/
            uint32_t rtc_cocpu:                   1;     /*enable riscV cocpu interrupt*/
            uint32_t rtc_saradc2:                 1;     /*enable saradc2 interrupt*/
            uint32_t rtc_swd:                     1;     /*enable super watch dog interrupt*/
            uint32_t rtc_xtal32k_dead:            1;     /*enable xtal32k_dead  interrupt*/
            uint32_t rtc_cocpu_trap:              1;     /*enable cocpu trap interrupt*/
            uint32_t rtc_touch_timeout:           1;     /*enable touch timeout interrupt*/
            uint32_t rtc_glitch_det:              1;     /*enbale gitch det interrupt*/
            uint32_t reserved20:                 12;
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t slp_wakeup:                  1;     /*sleep wakeup interrupt raw*/
            uint32_t slp_reject:                  1;     /*sleep reject interrupt raw*/
            uint32_t sdio_idle:                   1;     /*SDIO idle interrupt raw*/
            uint32_t rtc_wdt:                     1;     /*RTC WDT interrupt raw*/
            uint32_t rtc_touch_scan_done:         1;
            uint32_t rtc_ulp_cp:                  1;     /*ULP-coprocessor interrupt raw*/
            uint32_t rtc_touch_done:              1;     /*touch interrupt raw*/
            uint32_t rtc_touch_active:            1;     /*touch active interrupt raw*/
            uint32_t rtc_touch_inactive:          1;     /*touch inactive interrupt raw*/
            uint32_t rtc_brown_out:               1;     /*brown out interrupt raw*/
            uint32_t rtc_main_timer:              1;     /*RTC main timer interrupt raw*/
            uint32_t rtc_saradc1:                 1;     /*saradc1 interrupt raw*/
            uint32_t rtc_tsens:                   1;     /*tsens interrupt raw*/
            uint32_t rtc_cocpu:                   1;     /*riscV cocpu interrupt raw*/
            uint32_t rtc_saradc2:                 1;     /*saradc2 interrupt raw*/
            uint32_t rtc_swd:                     1;     /*super watch dog interrupt raw*/
            uint32_t rtc_xtal32k_dead:            1;     /*xtal32k dead detection interrupt raw*/
            uint32_t rtc_cocpu_trap:              1;     /*cocpu trap interrupt raw*/
            uint32_t rtc_touch_timeout:           1;     /*touch timeout interrupt raw*/
            uint32_t rtc_glitch_det:              1;     /*glitch_det_interrupt_raw*/
            uint32_t reserved20:                 12;
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t slp_wakeup:                 1;      /*sleep wakeup interrupt state*/
            uint32_t slp_reject:                 1;      /*sleep reject interrupt state*/
            uint32_t sdio_idle:                  1;      /*SDIO idle interrupt state*/
            uint32_t rtc_wdt:                    1;      /*RTC WDT interrupt state*/
            uint32_t rtc_touch_scan_done:        1;
            uint32_t rtc_ulp_cp:                 1;      /*ULP-coprocessor interrupt state*/
            uint32_t rtc_touch_done:             1;      /*touch done interrupt state*/
            uint32_t rtc_touch_active:           1;      /*touch active interrupt state*/
            uint32_t rtc_touch_inactive:         1;      /*touch inactive interrupt state*/
            uint32_t rtc_brown_out:              1;      /*brown out interrupt state*/
            uint32_t rtc_main_timer:             1;      /*RTC main timer interrupt state*/
            uint32_t rtc_saradc1:                1;      /*saradc1 interrupt state*/
            uint32_t rtc_tsens:                  1;      /*tsens interrupt state*/
            uint32_t rtc_cocpu:                  1;      /*riscV cocpu interrupt state*/
            uint32_t rtc_saradc2:                1;      /*saradc2 interrupt state*/
            uint32_t rtc_swd:                    1;      /*super watch dog interrupt state*/
            uint32_t rtc_xtal32k_dead:           1;      /*xtal32k dead detection interrupt state*/
            uint32_t rtc_cocpu_trap:             1;      /*cocpu trap interrupt state*/
            uint32_t rtc_touch_timeout:          1;      /*Touch timeout interrupt state*/
            uint32_t rtc_glitch_det:             1;      /*glitch_det_interrupt state*/
            uint32_t reserved20:                12;
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t slp_wakeup:                  1;     /*Clear sleep wakeup interrupt state*/
            uint32_t slp_reject:                  1;     /*Clear sleep reject interrupt state*/
            uint32_t sdio_idle:                   1;     /*Clear SDIO idle interrupt state*/
            uint32_t rtc_wdt:                     1;     /*Clear RTC WDT interrupt state*/
            uint32_t rtc_touch_scan_done:         1;
            uint32_t rtc_ulp_cp:                  1;     /*Clear ULP-coprocessor interrupt state*/
            uint32_t rtc_touch_done:              1;     /*Clear touch done interrupt state*/
            uint32_t rtc_touch_active:            1;     /*Clear touch active interrupt state*/
            uint32_t rtc_touch_inactive:          1;     /*Clear touch inactive interrupt state*/
            uint32_t rtc_brown_out:               1;     /*Clear brown out interrupt state*/
            uint32_t rtc_main_timer:              1;     /*Clear RTC main timer interrupt state*/
            uint32_t rtc_saradc1:                 1;     /*Clear saradc1 interrupt state*/
            uint32_t rtc_tsens:                   1;     /*Clear tsens interrupt state*/
            uint32_t rtc_cocpu:                   1;     /*Clear riscV cocpu interrupt state*/
            uint32_t rtc_saradc2:                 1;     /*Clear saradc2 interrupt state*/
            uint32_t rtc_swd:                     1;     /*Clear super watch dog interrupt state*/
            uint32_t rtc_xtal32k_dead:            1;     /*Clear RTC WDT interrupt state*/
            uint32_t rtc_cocpu_trap:              1;     /*Clear cocpu trap interrupt state*/
            uint32_t rtc_touch_timeout:           1;     /*Clear touch timeout interrupt state*/
            uint32_t rtc_glitch_det:              1;     /*Clear glitch det interrupt state*/
            uint32_t reserved20:                 12;
        };
        uint32_t val;
    } int_clr;
    uint32_t store[4];                                 /**/
    union {
        struct {
            uint32_t xtal32k_wdt_en:       1;            /*xtal 32k watch dog enable*/
            uint32_t xtal32k_wdt_clk_fo:   1;            /*xtal 32k watch dog clock force on*/
            uint32_t xtal32k_wdt_reset:    1;            /*xtal 32k watch dog sw reset*/
            uint32_t xtal32k_ext_clk_fo:   1;            /*xtal 32k external xtal clock force on*/
            uint32_t xtal32k_auto_backup:  1;            /*xtal 32k switch to back up clock when xtal is dead*/
            uint32_t xtal32k_auto_restart: 1;            /*xtal 32k restart xtal when xtal is dead*/
            uint32_t xtal32k_auto_return:  1;            /*xtal 32k switch back xtal when xtal is restarted*/
            uint32_t xtal32k_xpd_force:    1;            /*Xtal 32k xpd control by sw or fsm*/
            uint32_t enckinit_xtal_32k:    1;            /*apply an internal clock to help xtal 32k to start*/
            uint32_t dbuf_xtal_32k:        1;            /*0: single-end buffer 1: differential buffer*/
            uint32_t dgm_xtal_32k:         3;            /*xtal_32k gm control*/
            uint32_t dres_xtal_32k:        3;            /*DRES_XTAL_32K*/
            uint32_t xpd_xtal_32k:         1;            /*XPD_XTAL_32K*/
            uint32_t dac_xtal_32k:         3;            /*DAC_XTAL_32K*/
            uint32_t rtc_wdt_state:        3;            /*state of 32k_wdt*/
            uint32_t rtc_xtal32k_gpio_sel: 1;            /*XTAL_32K sel. 0: external XTAL_32K  1: CLK from RTC pad X32P_C*/
            uint32_t reserved24:           6;
            uint32_t ctr_lv:               1;            /*0: power down XTAL at high level  1: power down XTAL at low level*/
            uint32_t ctr_en:               1;
        };
        uint32_t val;
    } ext_xtl_conf;
    union {
        struct {
            uint32_t reserved0:         29;
            uint32_t gpio_wakeup_filter: 1;              /*enable filter for gpio wakeup event*/
            uint32_t wakeup0_lv:         1;              /*0: external wakeup at low level  1: external wakeup at high level*/
            uint32_t wakeup1_lv:         1;
        };
        uint32_t val;
    } ext_wakeup_conf;
    union {
        struct {
            uint32_t reserved0:           13;
            uint32_t rtc_sleep_reject_ena:17;            /*sleep reject enable*/
            uint32_t light_slp_reject_en:  1;            /*enable reject for light sleep*/
            uint32_t deep_slp_reject_en:   1;            /*enable reject for deep sleep*/
        };
        uint32_t val;
    } slp_reject_conf;
    union {
        struct {
            uint32_t reserved0:        29;
            uint32_t cpusel_conf:       1;               /*CPU sel option*/
            uint32_t cpuperiod_sel:     2;
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
            uint32_t reserved0:           3;
            uint32_t ck8m_div_sel_vld:    1;             /*used to sync reg_ck8m_div_sel bus. Clear vld before set reg_ck8m_div_sel  then set vld to actually switch the clk*/
            uint32_t ck8m_div:            2;             /*CK8M_D256_OUT divider. 00: div128  01: div256  10: div512  11: div1024.*/
            uint32_t enb_ck8m:            1;             /*disable CK8M and CK8M_D256_OUT*/
            uint32_t enb_ck8m_div:        1;             /*1: CK8M_D256_OUT is actually CK8M  0: CK8M_D256_OUT is CK8M divided by 256*/
            uint32_t dig_xtal32k_en:      1;             /*enable CK_XTAL_32K for digital core (no relationship with RTC core)*/
            uint32_t dig_clk8m_d256_en:   1;             /*enable CK8M_D256_OUT for digital core (no relationship with RTC core)*/
            uint32_t dig_clk8m_en:        1;             /*enable CK8M for digital core (no relationship with RTC core)*/
            uint32_t reserved11:          1;
            uint32_t ck8m_div_sel:        3;             /*divider = reg_ck8m_div_sel + 1*/
            uint32_t xtal_force_nogating: 1;             /*XTAL force no gating during sleep*/
            uint32_t ck8m_force_nogating: 1;             /*CK8M force no gating during sleep*/
            uint32_t ck8m_dfreq:          8;             /*CK8M_DFREQ*/
            uint32_t ck8m_force_pd:       1;             /*CK8M force power down*/
            uint32_t ck8m_force_pu:       1;             /*CK8M force power up*/
            uint32_t reserved27:          2;
            uint32_t fast_clk_rtc_sel:    1;             /*fast_clk_rtc sel. 0: XTAL div 4  1: CK8M*/
            uint32_t ana_clk_rtc_sel:     2;
        };
        uint32_t val;
    } clk_conf;
    union {
        struct {
            uint32_t reserved0:             22;
            uint32_t rtc_ana_clk_div_vld:    1;          /*used to sync div bus. clear vld before set reg_rtc_ana_clk_div  then set vld to actually switch the clk*/
            uint32_t rtc_ana_clk_div:        8;
            uint32_t slow_clk_next_edge:     1;
        };
        uint32_t val;
    } slow_clk_conf;
    union {
        struct {
            uint32_t sdio_timer_target: 8;               /*timer count to apply reg_sdio_dcap after sdio power on*/
            uint32_t reserved8:         1;
            uint32_t sdio_dthdrv:       2;               /*Tieh = 1 mode drive ability. Initially set to 0 to limit charge current  set to 3 after several us.*/
            uint32_t sdio_dcap:         2;               /*ability to prevent LDO from overshoot*/
            uint32_t sdio_initi:        2;               /*add resistor from ldo output to ground. 0: no res  1: 6k  2: 4k  3: 2k*/
            uint32_t sdio_en_initi:     1;               /*0 to set init[1:0]=0*/
            uint32_t sdio_dcurlim:      3;               /*tune current limit threshold when tieh = 0. About 800mA/(8+d)*/
            uint32_t sdio_modecurlim:   1;               /*select current limit mode*/
            uint32_t sdio_encurlim:     1;               /*enable current limit*/
            uint32_t sdio_pd_en:        1;               /*power down SDIO_REG in sleep. Only active when reg_sdio_force = 0*/
            uint32_t sdio_force:        1;               /*1: use SW option to control SDIO_REG  0: use state machine*/
            uint32_t sdio_tieh:         1;               /*SW option for SDIO_TIEH. Only active when reg_sdio_force = 1*/
            uint32_t reg1p8_ready:      1;               /*read only register for REG1P8_READY*/
            uint32_t drefl_sdio:        2;               /*SW option for DREFL_SDIO. Only active when reg_sdio_force = 1*/
            uint32_t drefm_sdio:        2;               /*SW option for DREFM_SDIO. Only active when reg_sdio_force = 1*/
            uint32_t drefh_sdio:        2;               /*SW option for DREFH_SDIO. Only active when reg_sdio_force = 1*/
            uint32_t xpd_sdio:          1;
        };
        uint32_t val;
    } sdio_conf;
    union {
        struct {
            uint32_t reserved0:            10;
            uint32_t bias_buf_idle:         1;
            uint32_t bias_buf_wake:         1;
            uint32_t bias_buf_deep_slp:     1;
            uint32_t bias_buf_monitor:      1;
            uint32_t pd_cur_deep_slp:       1;           /*xpd cur when rtc in sleep_state*/
            uint32_t pd_cur_monitor:        1;           /*xpd cur when rtc in monitor state*/
            uint32_t bias_sleep_deep_slp:   1;           /*bias_sleep when rtc in sleep_state*/
            uint32_t bias_sleep_monitor:    1;           /*bias_sleep when rtc in monitor state*/
            uint32_t dbg_atten_deep_slp:    4;           /*DBG_ATTEN when rtc in sleep state*/
            uint32_t dbg_atten_monitor:     4;           /*DBG_ATTEN when rtc in monitor state*/
            uint32_t enb_sck_xtal:          1;           /*ENB_SCK_XTAL*/
            uint32_t inc_heartbeat_refresh: 1;           /*INC_HEARTBEAT_REFRESH*/
            uint32_t dec_heartbeat_period:  1;           /*DEC_HEARTBEAT_PERIOD*/
            uint32_t inc_heartbeat_period:  1;           /*INC_HEARTBEAT_PERIOD*/
            uint32_t dec_heartbeat_width:   1;           /*DEC_HEARTBEAT_WIDTH*/
            uint32_t rst_bias_i2c:          1;
        };
        uint32_t val;
    } bias_conf;
    union {
        struct {
            uint32_t reserved0:           8;
            uint32_t dig_dbias_slp:       3;             /*DIG_REG_DBIAS during sleep*/
            uint32_t dig_dbias_wak:       3;             /*DIG_REG_DBIAS during wakeup*/
            uint32_t sck_dcap:            8;             /*SCK_DCAP*/
            uint32_t rtc_dbias_slp:       3;             /*RTC_DBIAS during sleep*/
            uint32_t rtc_dbias_wak:       3;             /*RTC_DBIAS during wakeup*/
            uint32_t rtc_dboost_force_pd: 1;             /*RTC_DBOOST force power down*/
            uint32_t rtc_dboost_force_pu: 1;             /*RTC_DBOOST force power up*/
            uint32_t rtculator_force_pd:  1;             /*RTC_REG force power down (for RTC_REG power down means decrease the voltage to 0.8v or lower )*/
            uint32_t rtculator_force_pu:  1;
        };
        uint32_t val;
    } rtc;
    union {
        struct {
            uint32_t fastmem_force_noiso:     1;         /*Fast RTC memory force no ISO*/
            uint32_t fastmem_force_iso:       1;         /*Fast RTC memory force ISO*/
            uint32_t slowmem_force_noiso:     1;         /*RTC memory force no ISO*/
            uint32_t slowmem_force_iso:       1;         /*RTC memory force ISO*/
            uint32_t rtc_force_iso:           1;         /*rtc_peri force ISO*/
            uint32_t rtc_force_noiso:         1;         /*rtc_peri force no ISO*/
            uint32_t fastmem_folw_cpu:        1;         /*1: Fast RTC memory PD following CPU  0: fast RTC memory PD following RTC state machine*/
            uint32_t fastmem_force_lpd:       1;         /*Fast RTC memory force PD*/
            uint32_t fastmem_force_lpu:       1;         /*Fast RTC memory force no PD*/
            uint32_t slowmem_folw_cpu:        1;         /*1: RTC memory  PD following CPU  0: RTC memory PD following RTC state machine*/
            uint32_t slowmem_force_lpd:       1;         /*RTC memory force PD*/
            uint32_t slowmem_force_lpu:       1;         /*RTC memory force no PD*/
            uint32_t fastmem_force_pd:        1;         /*Fast RTC memory force power down*/
            uint32_t fastmem_force_pu:        1;         /*Fast RTC memory force power up*/
            uint32_t fastmem_pd_en:           1;         /*enable power down fast RTC memory in sleep*/
            uint32_t slowmem_force_pd:        1;         /*RTC memory force power down*/
            uint32_t slowmem_force_pu:        1;         /*RTC memory force power up*/
            uint32_t slowmem_pd_en:           1;         /*enable power down RTC memory in sleep*/
            uint32_t rtc_force_pd:            1;         /*rtc_peri force power down*/
            uint32_t rtc_force_pu:            1;         /*rtc_peri force power up*/
            uint32_t rtc_pd_en:               1;         /*enable power down rtc_peri in sleep*/
            uint32_t rtc_pad_force_hold:      1;         /*rtc pad force hold*/
            uint32_t reserved22:             10;
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
            uint32_t dg_dcdc_force_pd:    1;             /*digital dcdc force power down*/
            uint32_t dg_dcdc_force_pu:    1;             /*digital dcdc force power up*/
            uint32_t dg_dcdc_pd_en:       1;             /*enable power down digital dcdc in sleep*/
            uint32_t rom0_pd_en:          1;             /*enable power down ROM in sleep*/
            uint32_t inter_ram0_pd_en:    1;             /*enable power down internal SRAM 0 in sleep*/
            uint32_t inter_ram1_pd_en:    1;             /*enable power down internal SRAM 1 in sleep*/
            uint32_t inter_ram2_pd_en:    1;             /*enable power down internal SRAM 2 in sleep*/
            uint32_t inter_ram3_pd_en:    1;             /*enable power down internal SRAM 3 in sleep*/
            uint32_t inter_ram4_pd_en:    1;             /*enable power down internal SRAM 4 in sleep*/
            uint32_t wifi_pd_en:          1;             /*enable power down wifi in sleep*/
            uint32_t dg_wrap_pd_en:       1;
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
            uint32_t dg_wrap_force_noiso:    1;
        };
        uint32_t val;
    } dig_iso;
    union {
        struct {
            uint32_t chip_reset_width:     8;            /*chip reset siginal pulse width*/
            uint32_t chip_reset_en:        1;            /*wdt reset whole chip enable*/
            uint32_t pause_in_slp:         1;            /*pause WDT in sleep*/
            uint32_t appcpu_reset_en:      1;            /*enable WDT reset APP CPU*/
            uint32_t procpu_reset_en:      1;            /*enable WDT reset PRO CPU*/
            uint32_t flashboot_mod_en:     1;            /*enable WDT in flash boot*/
            uint32_t sys_reset_length:     3;            /*system reset counter length*/
            uint32_t cpu_reset_length:     3;            /*CPU reset counter length*/
            uint32_t stg3:                 3;            /*1: interrupt stage en  2: CPU reset stage en  3: system reset stage en  4: RTC reset stage en*/
            uint32_t stg2:                 3;            /*1: interrupt stage en  2: CPU reset stage en  3: system reset stage en  4: RTC reset stage en*/
            uint32_t stg1:                 3;            /*1: interrupt stage en  2: CPU reset stage en  3: system reset stage en  4: RTC reset stage en*/
            uint32_t stg0:                 3;            /*1: interrupt stage en  2: CPU reset stage en  3: system reset stage en  4: RTC reset stage en*/
            uint32_t en:                   1;
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
            uint32_t feed:         1;
        };
        uint32_t val;
    } wdt_feed;
    uint32_t wdt_wprotect;                            /**/
    union {
        struct {
            uint32_t swd_reset_flag:   1;                /*swd reset flag*/
            uint32_t swd_feed_int:     1;                /*swd interrupt for feeding*/
            uint32_t reserved2:       16;
            uint32_t swd_signal_width:10;                /*adjust signal width send to swd*/
            uint32_t swd_rst_flag_clr: 1;                /*reset swd reset flag*/
            uint32_t swd_feed:         1;                /*Sw feed swd*/
            uint32_t swd_disable:      1;                /*disabel SWD*/
            uint32_t swd_auto_feed_en: 1;                /*automatically feed swd when int comes*/
        };
        uint32_t val;
    } swd_conf;
    uint32_t swd_wprotect;                           /*swd write protect*/
    union {
        struct {
            uint32_t reserved0:         20;
            uint32_t appcpu_c1:          6;              /*{reg_sw_stall_appcpu_c1[5:0]   reg_sw_stall_appcpu_c0[1:0]} == 0x86 will stall APP CPU*/
            uint32_t procpu_c1:          6;
        };
        uint32_t val;
    } sw_cpu_stall;
    uint32_t store4;                                 /**/
    uint32_t store5;                                 /**/
    uint32_t store6;                                 /**/
    uint32_t store7;                                 /**/
    union {
        struct {
            uint32_t xpd_rom0:                   1;      /*rom0 power down*/
            uint32_t reserved1:                  1;
            uint32_t xpd_dig_dcdc:               1;      /*External DCDC power down*/
            uint32_t rtc_peri_iso:               1;      /*rtc peripheral iso*/
            uint32_t xpd_rtc_peri:               1;      /*rtc peripheral power down*/
            uint32_t wifi_iso:                   1;      /*wifi iso*/
            uint32_t xpd_wifi:                   1;      /*wifi wrap power down*/
            uint32_t dig_iso:                    1;      /*digital wrap iso*/
            uint32_t xpd_dig:                    1;      /*digital wrap power down*/
            uint32_t rtc_touch_state_start:      1;      /*touch should start to work*/
            uint32_t rtc_touch_state_switch:     1;      /*touch is about to working. Switch rtc main state*/
            uint32_t rtc_touch_state_slp:        1;      /*touch is in sleep state*/
            uint32_t rtc_touch_state_done:       1;      /*touch is done*/
            uint32_t rtc_cocpu_state_start:      1;      /*ulp/cocpu should start to work*/
            uint32_t rtc_cocpu_state_switch:     1;      /*ulp/cocpu is about to working. Switch rtc main state*/
            uint32_t rtc_cocpu_state_slp:        1;      /*ulp/cocpu is in sleep state*/
            uint32_t rtc_cocpu_state_done:       1;      /*ulp/cocpu is done*/
            uint32_t rtc_main_state_xtal_iso:    1;      /*no use any more*/
            uint32_t rtc_main_state_pll_on:      1;      /*rtc main state machine is in states that pll should be running*/
            uint32_t rtc_rdy_for_wakeup:         1;      /*rtc is ready to receive wake up trigger from wake up source*/
            uint32_t rtc_main_state_wait_end:    1;      /*rtc main state machine has been waited for some cycles*/
            uint32_t rtc_in_wakeup_state:        1;      /*rtc main state machine is in the states of wakeup process*/
            uint32_t rtc_in_low_power_state:     1;      /*rtc main state machine is in the states of low power*/
            uint32_t rtc_main_state_in_wait_8m:  1;      /*rtc main state machine is in wait 8m state*/
            uint32_t rtc_main_state_in_wait_pll: 1;      /*rtc main state machine is in wait pll state*/
            uint32_t rtc_main_state_in_wait_xtl: 1;      /*rtc main state machine is in wait xtal state*/
            uint32_t rtc_main_state_in_slp:      1;      /*rtc main state machine is in sleep state*/
            uint32_t rtc_main_state_in_idle:     1;      /*rtc main state machine is in idle state*/
            uint32_t rtc_main_state:             4;      /*rtc main state machine status*/
        };
        uint32_t val;
    } low_power_st;
    uint32_t diag0;                                  /**/
    union {
        struct {
            uint32_t touch_pad0_hold:  1;
            uint32_t touch_pad1_hold:  1;
            uint32_t touch_pad2_hold:  1;
            uint32_t touch_pad3_hold:  1;
            uint32_t touch_pad4_hold:  1;
            uint32_t touch_pad5_hold:  1;
            uint32_t touch_pad6_hold:  1;
            uint32_t touch_pad7_hold:  1;
            uint32_t touch_pad8_hold:  1;
            uint32_t touch_pad9_hold:  1;
            uint32_t touch_pad10_hold: 1;
            uint32_t touch_pad11_hold: 1;
            uint32_t touch_pad12_hold: 1;
            uint32_t touch_pad13_hold: 1;
            uint32_t touch_pad14_hold: 1;
            uint32_t x32p_hold:        1;
            uint32_t x32n_hold:        1;
            uint32_t pdac1_hold:       1;
            uint32_t pdac2_hold:       1;
            uint32_t rtc_pad19_hold:   1;
            uint32_t rtc_pad20_hold:   1;
            uint32_t rtc_pad21_hold:   1;
            uint32_t reserved22:      10;
        };
        uint32_t val;
    } pad_hold;
    uint32_t dig_pad_hold;                               /**/
    union {
        struct {
            uint32_t sel:                   22;          /*Bitmap to select RTC pads for ext wakeup1*/
            uint32_t status_clr:             1;          /*clear ext wakeup1 status*/
            uint32_t reserved23:             9;
        };
        uint32_t val;
    } ext_wakeup1;
    union {
        struct {
            uint32_t status:            22;              /*ext wakeup1 status*/
            uint32_t reserved22:        10;
        };
        uint32_t val;
    } ext_wakeup1_status;
    union {
        struct {
            uint32_t out2_ena:                  1;       /*enable brown_out2 to start chip reset*/
            uint32_t reserved1:                 3;
            uint32_t int_wait:                 10;       /*brown out interrupt wait cycles*/
            uint32_t close_flash_ena:           1;       /*enable close flash when brown out happens*/
            uint32_t pd_rf_ena:                 1;       /*enable power down RF when brown out happens*/
            uint32_t rst_wait:                 10;       /*brown out reset wait cycles*/
            uint32_t rst_ena:                   1;       /*enable brown out reset*/
            uint32_t rst_sel:                   1;       /*1:  4-pos reset*/
            uint32_t reserved28:                1;
            uint32_t cnt_clr:                   1;       /*clear brown out counter*/
            uint32_t ena:                       1;       /*enable brown out*/
            uint32_t det:                       1;
        };
        uint32_t val;
    } brown_out;
    uint32_t time_low1;                              /*RTC timer low 32 bits*/
    union {
        struct {
            uint32_t rtc_timer_value1_high:16;           /*RTC timer high 16 bits*/
            uint32_t reserved16:           16;
        };
        uint32_t val;
    } time_high1;
    uint32_t xtal32k_clk_factor;                     /*xtal 32k watch dog backup clock factor*/
    union {
        struct {
            uint32_t xtal32k_return_wait:  4;            /*cycles to wait to return noral xtal 32k*/
            uint32_t xtal32k_restart_wait:16;            /*cycles to wait to repower on xtal 32k*/
            uint32_t xtal32k_wdt_timeout:  8;            /*If no clock detected for this amount of time  32k is regarded as dead*/
            uint32_t xtal32k_stable_thres: 4;            /*if restarted xtal32k period is smaller than this  it is regarded as stable*/
        };
        uint32_t val;
    } xtal32k_conf;
    union {
        struct {
            uint32_t ulp_cp_pc_init:        11;          /*ULP-coprocessor PC initial address*/
            uint32_t reserved11:            18;
            uint32_t ulp_cp_gpio_wakeup_ena: 1;          /*ULP-coprocessor wakeup by GPIO enable*/
            uint32_t ulp_cp_gpio_wakeup_clr: 1;          /*ULP-coprocessor wakeup by GPIO state clear*/
            uint32_t ulp_cp_slp_timer_en:    1;          /*ULP-coprocessor timer enable bit*/
        };
        uint32_t val;
    } ulp_cp_timer;
    union {
        struct {
            uint32_t ulp_cp_mem_addr_init:  11;
            uint32_t ulp_cp_mem_addr_size:  11;
            uint32_t ulp_cp_mem_offst_clr:   1;
            uint32_t reserved23:             5;
            uint32_t ulp_cp_clk_fo:          1;          /*ulp coprocessor clk force on*/
            uint32_t ulp_cp_reset:           1;          /*ulp coprocessor clk software reset*/
            uint32_t ulp_cp_force_start_top: 1;          /*1: ULP-coprocessor is started by SW*/
            uint32_t ulp_cp_start_top:       1;          /*Write 1 to start ULP-coprocessor*/
        };
        uint32_t val;
    } ulp_cp_ctrl;
    union {
        struct {
            uint32_t cocpu_clk_fo:            1;         /*cocpu clk force on*/
            uint32_t cocpu_start_2_reset_dis: 6;         /*time from start cocpu to pull down reset*/
            uint32_t cocpu_start_2_intr_en:   6;         /*time from start cocpu to give start interrupt*/
            uint32_t cocpu_shut:              1;         /*to shut cocpu*/
            uint32_t cocpu_shut_2_clk_dis:    8;         /*time from shut cocpu to disable clk*/
            uint32_t cocpu_shut_reset_en:     1;         /*to reset cocpu*/
            uint32_t cocpu_sel:               1;         /*1: old ULP 0: new riscV*/
            uint32_t cocpu_done_force:        1;         /*1: select riscv done 0: select ulp done*/
            uint32_t cocpu_done:              1;         /*done signal used by riscv to control timer.*/
            uint32_t cocpu_sw_int_trigger:    1;         /*trigger cocpu register interrupt*/
            uint32_t reserved27:              5;
        };
        uint32_t val;
    } cocpu_ctrl;
    union {
        struct {
            uint32_t touch_sleep_cycles:16;              /*sleep cycles for timer*/
            uint32_t touch_meas_num:    16;              /*the meas length (in 8MHz)*/
        };
        uint32_t val;
    } touch_ctrl1;
    union {
        struct {
            uint32_t reserved0:              2;
            uint32_t touch_drange:           2;          /*TOUCH_DRANGE*/
            uint32_t touch_drefl:            2;          /*TOUCH_DREFL*/
            uint32_t touch_drefh:            2;          /*TOUCH_DREFH*/
            uint32_t touch_xpd_bias:         1;          /*TOUCH_XPD_BIAS*/
            uint32_t touch_refc:             3;          /*TOUCH pad0 reference cap*/
            uint32_t touch_dbias:            1;          /*1:use self bias 0:use bandgap bias*/
            uint32_t touch_slp_timer_en:     1;          /*touch timer enable bit*/
            uint32_t touch_start_fsm_en:     1;          /*1: TOUCH_START & TOUCH_XPD is controlled by touch fsm*/
            uint32_t touch_start_en:         1;          /*1: start touch fsm*/
            uint32_t touch_start_force:      1;          /*1: to start touch fsm by SW*/
            uint32_t touch_xpd_wait:         8;          /*the waiting cycles (in 8MHz) between TOUCH_START and TOUCH_XPD*/
            uint32_t touch_slp_cyc_div:      2;          /*when a touch pad is active  sleep cycle could be divided by this number*/
            uint32_t touch_timer_force_done: 2;          /*force touch timer done*/
            uint32_t touch_reset:            1;          /*reset upgrade touch*/
            uint32_t touch_clk_fo:           1;          /*touch clock force on*/
            uint32_t touch_clkgate_en:       1;          /*touch clock enable*/
        };
        uint32_t val;
    } touch_ctrl2;
    union {
        struct {
            uint32_t touch_denoise_res:         2;       /*De-noise resolution: 12/10/8/4 bit*/
            uint32_t touch_denoise_en:          1;       /*touch pad0 will be used to de-noise*/
            uint32_t reserved3:                 5;
            uint32_t touch_inactive_connection: 1;       /*inactive touch pads connect to 1: gnd 0: HighZ*/
            uint32_t touch_shield_pad_en:       1;       /*touch pad14 will be used as shield*/
            uint32_t touch_scan_pad_map:       15;       /*touch scan mode pad enable map*/
            uint32_t touch_bufdrv:              3;       /*touch7 buffer driver strength*/
            uint32_t touch_out_ring:            4;       /*select out ring pad*/
        };
        uint32_t val;
    } touch_scan_ctrl;
    union {
        struct {
            uint32_t touch_slp_th:         22;           /*the threshold for sleep touch pad*/
            uint32_t reserved22:            4;
            uint32_t touch_slp_approach_en: 1;           /*sleep pad approach function enable*/
            uint32_t touch_slp_pad:         5;
        };
        uint32_t val;
    } touch_slp_thres;
    union {
        struct {
            uint32_t reserved0:               23;
            uint32_t touch_slp_channel_clr:    1;        /*clear touch slp channel*/
            uint32_t touch_approach_meas_time: 8;        /*approach pads total meas times*/
        };
        uint32_t val;
    } touch_approach;
    union {
        struct {
            uint32_t reserved0:             9;
            uint32_t touch_smooth_lvl:      2;
            uint32_t touch_jitter_step:     4;           /*touch jitter step*/
            uint32_t config1:               4;
            uint32_t config2:               2;
            uint32_t touch_noise_thres:     2;
            uint32_t config3:               2;
            uint32_t touch_debounce:        3;           /*debounce counter*/
            uint32_t touch_filter_mode:     3;           /*0: IIR ? 1: IIR ? 2: IIR 1/8 3: Jitter*/
            uint32_t touch_filter_en:       1;           /*touch filter enable*/
        };
        uint32_t val;
    } touch_filter_ctrl;
    union {
        struct {
            uint32_t usb_vrefh:               2;
            uint32_t usb_vrefl:               2;
            uint32_t usb_vref_override:       1;
            uint32_t usb_pad_pull_override:   1;
            uint32_t usb_dp_pullup:           1;
            uint32_t usb_dp_pulldown:         1;
            uint32_t usb_dm_pullup:           1;
            uint32_t usb_dm_pulldown:         1;
            uint32_t usb_pullup_value:        1;
            uint32_t usb_pad_enable_override: 1;
            uint32_t usb_pad_enable:          1;
            uint32_t usb_txm:                 1;
            uint32_t usb_txp:                 1;
            uint32_t usb_tx_en:               1;
            uint32_t usb_tx_en_override:      1;
            uint32_t usb_reset_disable:       1;
            uint32_t io_mux_reset_disable:    1;
            uint32_t reserved19:             13;
        };
        uint32_t val;
    } usb_conf;
    union {
        struct {
            uint32_t touch_timeout_num:22;
            uint32_t touch_timeout_en:  1;
            uint32_t reserved23:        9;
        };
        uint32_t val;
    } touch_timeout_ctrl;
    union {
        struct {
            uint32_t reject_cause:17;                    /*sleep reject cause*/
            uint32_t reserved17:  15;
        };
        uint32_t val;
    } slp_reject_cause;
    union {
        struct {
            uint32_t force_download_boot: 1;
            uint32_t reserved1:          31;
        };
        uint32_t val;
    } option1;
    union {
        struct {
            uint32_t wakeup_cause:17;                    /*sleep wakeup cause*/
            uint32_t reserved17:  15;
        };
        uint32_t val;
    } slp_wakeup_cause;
    union {
        struct {
            uint32_t reserved0:              8;
            uint32_t ulp_cp_timer_slp_cycle:24;          /*sleep cycles for ULP-coprocessor timer*/
        };
        uint32_t val;
    } ulp_cp_timer_1;
    uint32_t reserved_134;
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
