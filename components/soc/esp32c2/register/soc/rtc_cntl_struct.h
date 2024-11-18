/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct rtc_cntl_dev_s{
    union {
        struct {
            uint32_t reserved0                     :    2;  /*Reserved*/
            uint32_t sw_stall_procpu_c0            :    2;  /*{reg_sw_stall_procpu_c1[5:0],  reg_sw_stall_procpu_c0[1:0]} == 0x86 will stall PRO CPU*/
            uint32_t reserved4                     :    1;  /*Reserved*/
            uint32_t sw_procpu_rst                 :    1;  /*PRO CPU SW reset*/
            uint32_t bb_i2c_force_pd               :    1;  /*BB_I2C force power down*/
            uint32_t bb_i2c_force_pu               :    1;  /*BB_I2C force power up*/
            uint32_t bbpll_i2c_force_pd            :    1;  /*BB_PLL _I2C force power down*/
            uint32_t bbpll_i2c_force_pu            :    1;  /*BB_PLL_I2C force power up*/
            uint32_t bbpll_force_pd                :    1;  /*BB_PLL force power down*/
            uint32_t bbpll_force_pu                :    1;  /*BB_PLL force power up*/
            uint32_t xtl_force_pd                  :    1;  /*crystall force power down*/
            uint32_t xtl_force_pu                  :    1;  /*crystall force power up*/
            uint32_t xtl_en_wait                   :    4;  /*wait bias_sleep and current source wakeup*/
            uint32_t reserved18                    :    2;  /*Reserved*/
            uint32_t ctr_sel                       :    3;  /*Need add desc*/
            uint32_t reserved23                    :    2;  /*Reserved*/
            uint32_t analog_force_iso              :    1;  /*Need add desc*/
            uint32_t reserved26                    :    2;  /*Reserved*/
            uint32_t analog_force_noiso            :    1;  /*Need add desc*/
            uint32_t dg_wrap_force_rst             :    1;  /*digital wrap force reset in deep sleep*/
            uint32_t dg_wrap_force_norst           :    1;  /*digital core force no reset in deep sleep*/
            uint32_t sw_sys_rst                    :    1;  /*SW system reset*/
        };
        uint32_t val;
    } options0;
    uint32_t slp_timer0;
    union {
        struct {
            uint32_t slp_val_hi                    :    16;  /*RTC sleep timer high 16 bits*/
            uint32_t main_timer_alarm_en           :    1;  /*timer alarm enable bit*/
            uint32_t reserved17                    :    15;  /*Reserved*/
        };
        uint32_t val;
    } slp_timer1;
    union {
        struct {
            uint32_t reserved0                     :    27;  /*Reserved*/
            uint32_t timer_sys_stall               :    1;  /*Enable to record system stall time*/
            uint32_t timer_xtl_off                 :    1;  /*Enable to record 40M XTAL OFF time*/
            uint32_t timer_sys_rst                 :    1;  /*enable to record system reset time*/
            uint32_t reserved30                    :    1;  /*Reserved*/
            uint32_t update                        :    1;  /*Set 1: to update register with RTC timer*/
        };
        uint32_t val;
    } time_update;
    uint32_t time_low0;
    union {
        struct {
            uint32_t rtc_timer_value0_high         :    16;  /*RTC timer high 16 bits*/
            uint32_t reserved16                    :    16;  /*Reserved*/
        };
        uint32_t val;
    } time_high0;
    union {
        struct {
            uint32_t rtc_sw_cpu_int                :    1;  /*rtc software interrupt to main cpu*/
            uint32_t rtc_slp_reject_cause_clr      :    1;  /*clear rtc sleep reject cause*/
            uint32_t reserved2                     :    20;  /*Reserved*/
            uint32_t apb2rtc_bridge_sel            :    1;  /*1: APB to RTC using bridge*/
            uint32_t reserved23                    :    5;  /*Reserved*/
            uint32_t sdio_active_ind               :    1;  /*SDIO active indication*/
            uint32_t slp_wakeup                    :    1;  /*leep wakeup bit*/
            uint32_t slp_reject                    :    1;  /*leep reject bit*/
            uint32_t sleep_en                      :    1;  /*sleep enable bit*/
        };
        uint32_t val;
    } state0;
    union {
        struct {
            uint32_t cpu_stall_en                  :    1;  /*CPU stall enable bit*/
            uint32_t cpu_stall_wait                :    5;  /*CPU stall wait cycles in fast_clk_rtc*/
            uint32_t ck8m_wait                     :    8;  /*CK8M wait cycles in slow_clk_rtc*/
            uint32_t xtl_buf_wait                  :    10;  /*XTAL wait cycles in slow_clk_rtc*/
            uint32_t pll_buf_wait                  :    8;  /*PLL wait cycles in slow_clk_rtc*/
        };
        uint32_t val;
    } timer1;
    union {
        struct {
            uint32_t reserved0                     :    24;  /*Reserved*/
            uint32_t min_time_ck8m_off             :    8;  /*minimal cycles in slow_clk_rtc for CK8M in power down state*/
        };
        uint32_t val;
    } timer2;
    union {
        struct {
            uint32_t reserved0                     :    16;  /*Reserved*/
            uint32_t dg_wrap_wait_timer            :    9;  /*Need add desc*/
            uint32_t dg_wrap_powerup_timer         :    7;  /*Need add desc*/
        };
        uint32_t val;
    } timer4;
    union {
        struct {
            uint32_t reserved0                     :    8;  /*Reserved*/
            uint32_t min_slp_val                   :    8;  /*minimal sleep cycles in slow_clk_rtc*/
            uint32_t reserved16                    :    16;  /*Reserved*/
        };
        uint32_t val;
    } timer5;
    union {
        struct {
            uint32_t reserved0                     :    18;  /*Reserved*/
            uint32_t i2c_reset_por_force_pd        :    1;  /*Need add desc*/
            uint32_t i2c_reset_por_force_pu        :    1;  /*Need add desc*/
            uint32_t reserved20                    :    2;  /*ReservedPLLA force power down*/
            uint32_t sar_i2c_pu                    :    1;  /*PLLA force power up*/
            uint32_t reserved23                    :    2;  /*Reserved*/
            uint32_t bbpll_cal_slp_start           :    1;  /*start BBPLL calibration during sleep*/
            uint32_t reserved26                    :    1;  /*Reserved*/
            uint32_t txrf_i2c_pu                   :    1;  /*1: TXRF_I2C power up*/
            uint32_t rfrx_pbus_pu                  :    1;  /*1: RFRX_PBUS power up*/
            uint32_t reserved29                    :    1;  /*Reserved*/
            uint32_t ckgen_i2c_pu                  :    1;  /*1: CKGEN_I2C power up*/
            uint32_t pll_i2c_pu                    :    1;  /*Need add desc*/
        };
        uint32_t val;
    } ana_conf;
    union {
        struct {
            uint32_t reset_cause_procpu            :    6;  /*reset cause of PRO CPU*/
            uint32_t reserved6                     :    7;  /*Reserved*/
            uint32_t stat_vector_sel_procpu        :    1;  /*PRO CPU state vector sel*/
            uint32_t reserved14                    :    5;  /*Reserved*/
            uint32_t ocd_halt_on_reset_procpu      :    1;  /*PROCPU OcdHaltOnReset*/
            uint32_t rtc_dreset_mask_procpu        :    1;  /*Need add desc*/
            uint32_t reserved21                    :    11;  /*Reserved*/
        };
        uint32_t val;
    } reset_state;
    union {
        struct {
            uint32_t reserved0                     :    15;  /*Reserved*/
            uint32_t rtc_wakeup_ena                :    17;  /*wakeup enable bitmap*/
        };
        uint32_t val;
    } wakeup_state;
    union {
        struct {
            uint32_t slp_wakeup                    :    1;  /*enable sleep wakeup interrupt*/
            uint32_t slp_reject                    :    1;  /*enable sleep reject interrupt*/
            uint32_t reserved2                     :    1;  /*Reservedenable SDIO idle interrupt*/
            uint32_t rtc_wdt                       :    1;  /*enable RTC WDT interrupt*/
            uint32_t reserved4                     :    5;  /*Reserved*/
            uint32_t rtc_brown_out                 :    1;  /*enable brown out interrupt*/
            uint32_t rtc_main_timer                :    1;  /*enable RTC main timer interrupt*/
            uint32_t reserved11                    :    4;  /*Reservedenable saradc2 interrupt*/
            uint32_t rtc_swd                       :    1;  /*enable super watch dog interrupt*/
            uint32_t reserved16                    :    4;  /*Reservedenable touch timeout interrupt*/
            uint32_t rtc_bbpll_cal                 :    1;  /*Need add desc*/
            uint32_t reserved21                    :    11;  /*Reserved*/
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t slp_wakeup                    :    1;  /*sleep wakeup interrupt raw*/
            uint32_t slp_reject                    :    1;  /*sleep reject interrupt raw*/
            uint32_t reserved2                     :    1;  /*ReservedSDIO idle interrupt raw*/
            uint32_t rtc_wdt                       :    1;  /*RTC WDT interrupt raw*/
            uint32_t reserved4                     :    5;  /*Reservedtouch inactive interrupt raw*/
            uint32_t rtc_brown_out                 :    1;  /*brown out interrupt raw*/
            uint32_t rtc_main_timer                :    1;  /*RTC main timer interrupt raw*/
            uint32_t reserved11                    :    4;  /*Reservedsaradc2 interrupt raw*/
            uint32_t rtc_swd                       :    1;  /*super watch dog interrupt raw*/
            uint32_t reserved16                    :    4;  /*Reservedtouch timeout interrupt raw*/
            uint32_t rtc_bbpll_cal                 :    1;  /*Need add desc*/
            uint32_t reserved21                    :    11;  /*Reserved*/
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t slp_wakeup                    :    1;  /*sleep wakeup interrupt state*/
            uint32_t slp_reject                    :    1;  /*sleep reject interrupt state*/
            uint32_t reserved2                     :    1;  /*Reserved*/
            uint32_t rtc_wdt                       :    1;  /*RTC WDT interrupt state*/
            uint32_t reserved4                     :    5;  /*Reserved*/
            uint32_t rtc_brown_out                 :    1;  /*brown out interrupt state*/
            uint32_t rtc_main_timer                :    1;  /*RTC main timer interrupt state*/
            uint32_t reserved11                    :    4;  /*Reserved*/
            uint32_t rtc_swd                       :    1;  /*super watch dog interrupt state*/
            uint32_t reserved16                    :    4;  /*Reserved*/
            uint32_t rtc_bbpll_cal                 :    1;  /*Need add desc*/
            uint32_t reserved21                    :    11;  /*Reserved*/
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t slp_wakeup                    :    1;  /*Clear sleep wakeup interrupt state*/
            uint32_t slp_reject                    :    1;  /*Clear sleep reject interrupt state*/
            uint32_t reserved2                     :    1;  /*Reserved*/
            uint32_t rtc_wdt                       :    1;  /*Clear RTC WDT interrupt state*/
            uint32_t reserved4                     :    5;  /*Reserved*/
            uint32_t rtc_brown_out                 :    1;  /*Clear brown out interrupt state*/
            uint32_t rtc_main_timer                :    1;  /*Clear RTC main timer interrupt state*/
            uint32_t reserved11                    :    4;  /*Reserved*/
            uint32_t rtc_swd                       :    1;  /*Clear super watch dog interrupt state*/
            uint32_t reserved16                    :    4;  /*Reserved*/
            uint32_t rtc_bbpll_cal                 :    1;  /*Need add desc*/
            uint32_t reserved21                    :    11;  /*Reserved*/
        };
        uint32_t val;
    } int_clr;
    uint32_t store[4];
    union {
        struct {
            uint32_t reserved0                     :    30;  /*Reserved*/
            uint32_t ctr_lv                        :    1;  /*0: power down XTAL at high level*/
            uint32_t ctr_en                        :    1;  /*Need add desc*/
        };
        uint32_t val;
    } ext_xtl_conf;
    union {
        struct {
            uint32_t reserved0                     :    31;  /*Reserved*/
            uint32_t gpio_wakeup_filter            :    1;  /*enable filter for gpio wakeup event*/
        };
        uint32_t val;
    } ext_wakeup_conf;
    union {
        struct {
            uint32_t reserved0                     :    12;  /*Reserved*/
            uint32_t rtc_sleep_reject_ena          :    18;  /*sleep reject enable*/
            uint32_t light_slp_reject_en           :    1;  /*enable reject for light sleep*/
            uint32_t deep_slp_reject_en            :    1;  /*enable reject for deep sleep*/
        };
        uint32_t val;
    } slp_reject_conf;
    union {
        struct {
            uint32_t reserved0                     :    29;  /*Reserved*/
            uint32_t cpusel_conf                   :    1;  /*CPU sel option*/
            uint32_t cpuperiod_sel                 :    2;  /*Need add desc*/
        };
        uint32_t val;
    } cpu_period_conf;
    union {
        struct {
            uint32_t reserved0                     :    1;  /*Reserved*/
            uint32_t efuse_clk_force_gating        :    1;  /*Need add desc*/
            uint32_t efuse_clk_force_nogating      :    1;  /*Need add desc*/
            uint32_t ck8m_div_sel_vld              :    1;  /*used to sync reg_ck8m_div_sel bus. Clear vld before set reg_ck8m_div_sel*/
            uint32_t ck8m_div                      :    2;  /*CK8M_D256_OUT divider. 00: div128*/
            uint32_t enb_ck8m                      :    1;  /*disable CK8M and CK8M_D256_OUT*/
            uint32_t enb_ck8m_div                  :    1;  /*1: CK8M_D256_OUT is actually CK8M*/
            uint32_t dig_xtal32k_en                :    1;  /*enable CK_XTAL_32K for digital core (no relationship with RTC core)*/
            uint32_t dig_clk8m_d256_en             :    1;  /*enable CK8M_D256_OUT for digital core (no relationship with RTC core)*/
            uint32_t dig_clk8m_en                  :    1;  /*enable CK8M for digital core (no relationship with RTC core)*/
            uint32_t reserved11                    :    1;  /*Reserved*/
            uint32_t ck8m_div_sel                  :    3;  /*divider = reg_ck8m_div_sel + 1*/
            uint32_t xtal_force_nogating           :    1;  /*XTAL force no gating during sleep*/
            uint32_t ck8m_force_nogating           :    1;  /*CK8M force no gating during sleep*/
            uint32_t ck8m_dfreq                    :    8;  /*CK8M_DFREQ*/
            uint32_t ck8m_force_pd                 :    1;  /*CK8M force power down*/
            uint32_t ck8m_force_pu                 :    1;  /*CK8M force power up*/
            uint32_t xtal_global_force_gating      :    1;  /*Need add desc*/
            uint32_t xtal_global_force_nogating    :    1;  /*Need add desc*/
            uint32_t fast_clk_rtc_sel              :    1;  /*fast_clk_rtc sel. 0: XTAL div 2*/
            uint32_t ana_clk_rtc_sel               :    2;  /*Need add desc*/
        };
        uint32_t val;
    } clk_conf;
    union {
        struct {
            uint32_t reserved0                     :    22;  /*Reserved*/
            uint32_t rtc_ana_clk_div_vld           :    1;  /*used to sync div bus. clear vld before set reg_rtc_ana_clk_div*/
            uint32_t rtc_ana_clk_div               :    8;  /*Need add desc*/
            uint32_t slow_clk_next_edge            :    1;  /*Need add desc*/
        };
        uint32_t val;
    } slow_clk_conf;
    union {
        struct {
            uint32_t dg_vdd_drv_b_slp              :    8;  /*Need add desc*/
            uint32_t dg_vdd_drv_b_slp_en           :    1;  /*Need add desc*/
            uint32_t reserved9                     :    1;  /*Reserved*/
            uint32_t bias_buf_idle                 :    1;  /*Need add desc*/
            uint32_t bias_buf_wake                 :    1;  /*Need add desc*/
            uint32_t bias_buf_deep_slp             :    1;  /*Need add desc*/
            uint32_t bias_buf_monitor              :    1;  /*Need add desc*/
            uint32_t pd_cur_deep_slp               :    1;  /*xpd cur when rtc in sleep_state*/
            uint32_t pd_cur_monitor                :    1;  /*xpd cur when rtc in monitor state*/
            uint32_t bias_sleep_deep_slp           :    1;  /*bias_sleep when rtc in sleep_state*/
            uint32_t bias_sleep_monitor            :    1;  /*bias_sleep when rtc in monitor state*/
            uint32_t dbg_atten_deep_slp            :    4;  /*DBG_ATTEN when rtc in sleep state*/
            uint32_t dbg_atten_monitor             :    4;  /*DBG_ATTEN when rtc in active state*/
            uint32_t dbg_atten_active              :    4;  /*Need add desc*/
            uint32_t reserved30                    :    2;  /*Reserved*/
        };
        uint32_t val;
    } bias_conf;
    union {
        struct {
            uint32_t reserved0                     :    7;  /*Reserved*/
            uint32_t dig_cal_en                    :    1;  /*Need add desc*/
            uint32_t reserved8                     :    6;  /*Reserved*/
            uint32_t sck_dcap                      :    8;  /*SCK_DCAP*/
            uint32_t reserved22                    :    8;  /*Reserved*/
            uint32_t rtculator_force_pd            :    1;  /*RTC_REG force power down (for RTC_REG power down means decrease the voltage to 0.8v or lower )*/
            uint32_t rtculator_force_pu            :    1;  /*Need add desc*/
        };
        uint32_t val;
    } rtc;
    union {
        struct {
            uint32_t reserved0                     :    21;  /*Reserved*/
            uint32_t rtc_pad_force_hold            :    1;  /*rtc pad force hold*/
            uint32_t reserved22                    :    10;  /*Reserved*/
        };
        uint32_t val;
    } pwc;
    union {
        struct {
            uint32_t vdd_spi_pwr_drv               :    2;  /*Need add desc*/
            uint32_t vdd_spi_pwr_force             :    1;  /*Need add desc*/
            uint32_t vdd_spi_pd_en                 :    1;  /*Need add desc*/
            uint32_t lslp_mem_force_pd             :    1;  /*memories in digital core force PD in sleep*/
            uint32_t lslp_mem_force_pu             :    1;  /*memories in digital core force no PD in sleep*/
            uint32_t reserved6                     :    13;  /*Reserved*/
            uint32_t dg_wrap_force_pd              :    1;  /*digital core force power down*/
            uint32_t dg_wrap_force_pu              :    1;  /*digital core force power up*/
            uint32_t reserved21                    :    10;  /*Reserved*/
            uint32_t dg_wrap_pd_en                 :    1;  /*Need add desc*/
        };
        uint32_t val;
    } dig_pwc;
    union {
        struct {
            uint32_t reserved0                     :    7;  /*Reserved*/
            uint32_t dig_iso_force_off             :    1;  /*Need add desc*/
            uint32_t dig_iso_force_on              :    1;  /*Need add desc*/
            uint32_t dg_pad_autohold               :    1;  /*read only register to indicate digital pad auto-hold status*/
            uint32_t clr_dg_pad_autohold           :    1;  /*wtite only register to clear digital pad auto-hold*/
            uint32_t dg_pad_autohold_en            :    1;  /*digital pad enable auto-hold*/
            uint32_t dg_pad_force_noiso            :    1;  /*digital pad force no ISO*/
            uint32_t dg_pad_force_iso              :    1;  /*digital pad force ISO*/
            uint32_t dg_pad_force_unhold           :    1;  /*digital pad force un-hold*/
            uint32_t dg_pad_force_hold             :    1;  /*digital pad force hold*/
            uint32_t reserved16                    :    14;  /*Reserved*/
            uint32_t dg_wrap_force_iso             :    1;  /*digital core force ISO*/
            uint32_t dg_wrap_force_noiso           :    1;  /*Need add desc*/
        };
        uint32_t val;
    } dig_iso;
    union {
        struct {
            uint32_t chip_reset_width              :    8;  /*chip reset siginal pulse width*/
            uint32_t chip_reset_en                 :    1;  /*wdt reset whole chip enable*/
            uint32_t pause_in_slp                  :    1;  /*pause WDT in sleep*/
            uint32_t reserved10                    :    1;  /*Reservedenable WDT reset APP CPU*/
            uint32_t procpu_reset_en               :    1;  /*enable WDT reset PRO CPU*/
            uint32_t flashboot_mod_en              :    1;  /*enable WDT in flash boot*/
            uint32_t sys_reset_length              :    3;  /*system reset counter length*/
            uint32_t cpu_reset_length              :    3;  /*CPU reset counter length*/
            uint32_t stg3                          :    3;  /*1: interrupt stage en*/
            uint32_t stg2                          :    3;  /*1: interrupt stage en*/
            uint32_t stg1                          :    3;  /*1: interrupt stage en*/
            uint32_t stg0                          :    3;  /*1: interrupt stage en*/
            uint32_t en                            :    1;  /*Need add desc*/
        };
        uint32_t val;
    } wdt_config0;
    uint32_t wdt_config1;
    uint32_t wdt_config2;
    uint32_t wdt_config3;
    uint32_t wdt_config4;
    union {
        struct {
            uint32_t reserved0                     :    31;  /*Reserved*/
            uint32_t feed                          :    1;  /*Need add desc*/
        };
        uint32_t val;
    } wdt_feed;
    uint32_t wdt_wprotect;
    union {
        struct {
            uint32_t swd_reset_flag                :    1;  /*swd reset flag*/
            uint32_t swd_feed_int                  :    1;  /*swd interrupt for feeding*/
            uint32_t reserved2                     :    15;  /*Reserved*/
            uint32_t swd_bypass_rst                :    1;  /*Need add desc*/
            uint32_t swd_signal_width              :    10;  /*adjust signal width send to swd*/
            uint32_t swd_rst_flag_clr              :    1;  /*reset swd reset flag*/
            uint32_t swd_feed                      :    1;  /*Sw feed swd*/
            uint32_t swd_disable                   :    1;  /*disable SWD*/
            uint32_t swd_auto_feed_en              :    1;  /*automatically feed swd when int comes*/
        };
        uint32_t val;
    } swd_conf;
    uint32_t swd_wprotect;
    union {
        struct {
            uint32_t reserved0                     :    26;  /*Reserved*/
            uint32_t procpu_c1                     :    6;  /*Need add desc*/
        };
        uint32_t val;
    } sw_cpu_stall;
    uint32_t store4;
    uint32_t store5;
    uint32_t store6;
    uint32_t store7;
    union {
        struct {
            uint32_t reserved0                     :    8;  /*Reserveddigital wrap iso*/
            uint32_t xpd_dig                       :    1;  /*digital wrap power down*/
            uint32_t rtc_touch_state_start         :    1;  /*touch should start to work*/
            uint32_t rtc_touch_state_switch        :    1;  /*touch is about to working. Switch rtc main state*/
            uint32_t rtc_touch_state_slp           :    1;  /*touch is in sleep state*/
            uint32_t rtc_touch_state_done          :    1;  /*touch is done*/
            uint32_t rtc_cocpu_state_start         :    1;  /*ulp/cocpu should start to work*/
            uint32_t rtc_cocpu_state_switch        :    1;  /*ulp/cocpu is about to working. Switch rtc main state*/
            uint32_t rtc_cocpu_state_slp           :    1;  /*ulp/cocpu is in sleep state*/
            uint32_t rtc_cocpu_state_done          :    1;  /*ulp/cocpu is done*/
            uint32_t rtc_main_state_xtal_iso       :    1;  /*no use any more*/
            uint32_t rtc_main_state_pll_on         :    1;  /*rtc main state machine is in states that pll should be running*/
            uint32_t rtc_rdy_for_wakeup            :    1;  /*rtc is ready to receive wake up trigger from wake up source*/
            uint32_t rtc_main_state_wait_end       :    1;  /*rtc main state machine has been waited for some cycles*/
            uint32_t rtc_in_wakeup_state           :    1;  /*rtc main state machine is in the states of wakeup process*/
            uint32_t rtc_in_low_power_state        :    1;  /*rtc main state machine is in the states of low power*/
            uint32_t rtc_main_state_in_wait_8m     :    1;  /*rtc main state machine is in wait 8m state*/
            uint32_t rtc_main_state_in_wait_pll    :    1;  /*rtc main state machine is in wait pll state*/
            uint32_t rtc_main_state_in_wait_xtl    :    1;  /*rtc main state machine is in wait xtal state*/
            uint32_t rtc_main_state_in_slp         :    1;  /*rtc main state machine is in sleep state*/
            uint32_t rtc_main_state_in_idle        :    1;  /*rtc main state machine is in idle state*/
            uint32_t rtc_main_state                :    4;  /*rtc main state machine status*/
        };
        uint32_t val;
    } low_power_st;
    uint32_t diag0;
    union {
        struct {
            uint32_t rtc_gpio_pin0_hold            :    1;  /*Need add desc*/
            uint32_t rtc_gpio_pin1_hold            :    1;  /*Need add desc*/
            uint32_t rtc_gpio_pin2_hold            :    1;  /*Need add desc*/
            uint32_t rtc_gpio_pin3_hold            :    1;  /*Need add desc*/
            uint32_t rtc_gpio_pin4_hold            :    1;  /*Need add desc*/
            uint32_t rtc_gpio_pin5_hold            :    1;  /*Need add desc*/
            uint32_t reserved6                     :    26;  /*Reserved*/
        };
        uint32_t val;
    } pad_hold;
    uint32_t dig_pad_hold;
    union {
        struct {
            uint32_t reserved0                     :    4;  /*Reserved*/
            uint32_t int_wait                      :    10;  /*brown out interrupt wait cycles*/
            uint32_t close_flash_ena               :    1;  /*enable close flash when brown out happens*/
            uint32_t pd_rf_ena                     :    1;  /*enable power down RF when brown out happens*/
            uint32_t rst_wait                      :    10;  /*brown out reset wait cycles*/
            uint32_t rst_ena                       :    1;  /*enable brown out reset*/
            uint32_t rst_sel                       :    1;  /*1:  4-pos reset*/
            uint32_t ana_rst_en                    :    1;  /*Need add desc*/
            uint32_t cnt_clr                       :    1;  /*clear brown out counter*/
            uint32_t ena                           :    1;  /*enable brown out*/
            uint32_t det                           :    1;  /*Need add desc*/
        };
        uint32_t val;
    } brown_out;
    uint32_t time_low1;
    union {
        struct {
            uint32_t rtc_timer_value1_high         :    16;  /*RTC timer high 16 bits*/
            uint32_t reserved16                    :    16;  /*Reserved*/
        };
        uint32_t val;
    } time_high1;
    union {
        struct {
            uint32_t reserved0                     :    18;  /*Reserved*/
            uint32_t io_mux_reset_disable          :    1;  /*Need add desc*/
            uint32_t reserved19                    :    13;  /*Reserved*/
        };
        uint32_t val;
    } usb_conf;
    union {
        struct {
            uint32_t reject_cause                  :    18;  /*sleep reject cause*/
            uint32_t reserved18                    :    14;  /*Reserved*/
        };
        uint32_t val;
    } slp_reject_cause;
    union {
        struct {
            uint32_t force_download_boot           :    1;  /*Need add desc*/
            uint32_t reserved1                     :    31;  /*Reserved*/
        };
        uint32_t val;
    } option1;
    union {
        struct {
            uint32_t wakeup_cause                  :    17;  /*sleep wakeup cause*/
            uint32_t reserved17                    :    15;  /*Reserved*/
        };
        uint32_t val;
    } slp_wakeup_cause;
    union {
        struct {
            uint32_t reserved0                     :    8;  /*Reserved*/
            uint32_t ulp_cp_timer_slp_cycle        :    24;  /*sleep cycles for ULP-coprocessor timer*/
        };
        uint32_t val;
    } ulp_cp_timer_1;
    union {
        struct {
            uint32_t slp_wakeup_w1ts               :    1;  /*enable sleep wakeup interrupt*/
            uint32_t slp_reject_w1ts               :    1;  /*enable sleep reject interrupt*/
            uint32_t reserved2                     :    1;  /*Reserved*/
            uint32_t rtc_wdt_w1ts                  :    1;  /*enable RTC WDT interrupt*/
            uint32_t reserved4                     :    5;  /*Reserved*/
            uint32_t w1ts                          :    1;  /*enable brown out interrupt*/
            uint32_t rtc_main_timer_w1ts           :    1;  /*enable RTC main timer interrupt*/
            uint32_t reserved11                    :    4;  /*Reserved*/
            uint32_t rtc_swd_w1ts                  :    1;  /*enable super watch dog interrupt*/
            uint32_t reserved16                    :    4;  /*Reservedenbale gitch det interrupt*/
            uint32_t rtc_bbpll_cal_w1ts            :    1;  /*Need add desc*/
            uint32_t reserved21                    :    11;  /*Reserved*/
        };
        uint32_t val;
    } int_ena_w1ts;
    union {
        struct {
            uint32_t slp_wakeup_w1tc               :    1;  /*enable sleep wakeup interrupt*/
            uint32_t slp_reject_w1tc               :    1;  /*enable sleep reject interrupt*/
            uint32_t reserved2                     :    1;  /*Reserved*/
            uint32_t rtc_wdt_w1tc                  :    1;  /*enable RTC WDT interrupt*/
            uint32_t reserved4                     :    5;  /*Reserved*/
            uint32_t w1tc                          :    1;  /*enable brown out interrupt*/
            uint32_t rtc_main_timer_w1tc           :    1;  /*enable RTC main timer interrupt*/
            uint32_t reserved11                    :    4;  /*Reserved*/
            uint32_t rtc_swd_w1tc                  :    1;  /*enable super watch dog interrupt*/
            uint32_t reserved16                    :    4;  /*Reservedenbale gitch det interrupt*/
            uint32_t rtc_bbpll_cal_w1tc            :    1;  /*Need add desc*/
            uint32_t reserved21                    :    11;  /*Reserved*/
        };
        uint32_t val;
    } int_ena_w1tc;
    union {
        struct {
            uint32_t reserved0                     :    18;  /*Reserved*/
            uint32_t retention_clk_sel             :    1;  /*Need add desc*/
            uint32_t retention_done_wait           :    3;  /*Need add desc*/
            uint32_t retention_clkoff_wait         :    4;  /*Need add desc*/
            uint32_t retention_en                  :    1;  /*Need add desc*/
            uint32_t retention_wait                :    5;  /*wait cycles for rention operation*/
        };
        uint32_t val;
    } retention_ctrl;
    union {
        struct {
            uint32_t rtc_fib_sel                   :    3;  /*select use analog fib signal*/
            uint32_t reserved3                     :    29;  /*Reserved*/
        };
        uint32_t val;
    } fib_sel;
    union {
        struct {
            uint32_t rtc_gpio_wakeup_status        :    6;  /*Need add desc*/
            uint32_t rtc_gpio_wakeup_status_clr    :    1;  /*Need add desc*/
            uint32_t rtc_gpio_pin_clk_gate         :    1;  /*Need add desc*/
            uint32_t rtc_gpio_pin5_int_type        :    3;  /*Need add desc*/
            uint32_t rtc_gpio_pin4_int_type        :    3;  /*Need add desc*/
            uint32_t rtc_gpio_pin3_int_type        :    3;  /*Need add desc*/
            uint32_t rtc_gpio_pin2_int_type        :    3;  /*Need add desc*/
            uint32_t rtc_gpio_pin1_int_type        :    3;  /*Need add desc*/
            uint32_t rtc_gpio_pin0_int_type        :    3;  /*Need add desc*/
            uint32_t rtc_gpio_pin5_wakeup_enable   :    1;  /*Need add desc*/
            uint32_t rtc_gpio_pin4_wakeup_enable   :    1;  /*Need add desc*/
            uint32_t rtc_gpio_pin3_wakeup_enable   :    1;  /*Need add desc*/
            uint32_t rtc_gpio_pin2_wakeup_enable   :    1;  /*Need add desc*/
            uint32_t rtc_gpio_pin1_wakeup_enable   :    1;  /*Need add desc*/
            uint32_t rtc_gpio_pin0_wakeup_enable   :    1;  /*Need add desc*/
        };
        uint32_t val;
    } gpio_wakeup;
    union {
        struct {
            uint32_t reserved0                     :    1;  /*Reserved*/
            uint32_t rtc_debug_12m_no_gating       :    1;  /*Need add desc*/
            uint32_t rtc_debug_bit_sel             :    5;  /*Need add desc*/
            uint32_t rtc_debug_sel0                :    5;  /*Need add desc*/
            uint32_t rtc_debug_sel1                :    5;  /*Need add desc*/
            uint32_t rtc_debug_sel2                :    5;  /*Need add desc*/
            uint32_t rtc_debug_sel3                :    5;  /*Need add desc*/
            uint32_t rtc_debug_sel4                :    5;  /*Need add desc*/
        };
        uint32_t val;
    } dbg_sel;
    union {
        struct {
            uint32_t reserved0                     :    2;  /*Reserved*/
            uint32_t rtc_gpio_pin5_mux_sel         :    1;  /*Need add desc*/
            uint32_t rtc_gpio_pin4_mux_sel         :    1;  /*Need add desc*/
            uint32_t rtc_gpio_pin3_mux_sel         :    1;  /*Need add desc*/
            uint32_t rtc_gpio_pin2_mux_sel         :    1;  /*Need add desc*/
            uint32_t rtc_gpio_pin1_mux_sel         :    1;  /*Need add desc*/
            uint32_t rtc_gpio_pin0_mux_sel         :    1;  /*Need add desc*/
            uint32_t rtc_gpio_pin5_fun_sel         :    4;  /*Need add desc*/
            uint32_t rtc_gpio_pin4_fun_sel         :    4;  /*Need add desc*/
            uint32_t rtc_gpio_pin3_fun_sel         :    4;  /*Need add desc*/
            uint32_t rtc_gpio_pin2_fun_sel         :    4;  /*Need add desc*/
            uint32_t rtc_gpio_pin1_fun_sel         :    4;  /*Need add desc*/
            uint32_t rtc_gpio_pin0_fun_sel         :    4;  /*Need add desc*/
        };
        uint32_t val;
    } dbg_map;
    union {
        struct {
            uint32_t reserved0                     :    27;  /*Reserved*/
            uint32_t sar2_pwdet_cct                :    3;  /*Need add desc*/
            uint32_t force_xpd_sar                 :    2;  /*Need add desc*/
        };
        uint32_t val;
    } sensor_ctrl;
    union {
        struct {
            uint32_t reserved0                     :    27;  /*Reserved*/
            uint32_t sar_debug_sel                 :    5;  /*Need add desc*/
        };
        uint32_t val;
    } dbg_sar_sel;
    uint32_t reserved_110;
    uint32_t reserved_114;
    uint32_t reserved_118;
    uint32_t reserved_11c;
    uint32_t reserved_120;
    uint32_t reserved_124;
    uint32_t reserved_128;
    uint32_t reserved_12c;
    uint32_t reserved_130;
    uint32_t reserved_134;
    uint32_t reserved_138;
    uint32_t reserved_13c;
    uint32_t reserved_140;
    uint32_t reserved_144;
    uint32_t reserved_148;
    uint32_t reserved_14c;
    uint32_t reserved_150;
    uint32_t reserved_154;
    uint32_t reserved_158;
    uint32_t reserved_15c;
    uint32_t reserved_160;
    uint32_t reserved_164;
    uint32_t reserved_168;
    uint32_t reserved_16c;
    uint32_t reserved_170;
    uint32_t reserved_174;
    uint32_t reserved_178;
    uint32_t reserved_17c;
    uint32_t reserved_180;
    uint32_t reserved_184;
    uint32_t reserved_188;
    uint32_t reserved_18c;
    uint32_t reserved_190;
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
    union {
        struct {
            uint32_t date                          :    28;  /*Need add desc*/
            uint32_t reserved28                    :    4;  /*Reserved*/
        };
        uint32_t val;
    } date;
} rtc_cntl_dev_t;
extern rtc_cntl_dev_t RTCCNTL;
#ifdef __cplusplus
}
#endif
