/**
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct {
    union {
        struct {
            uint32_t reserved0                     :    2;
            uint32_t wdt_chip_reset_width          :    8;  /*chip reset siginal pulse width*/
            uint32_t wdt_chip_reset_en             :    1;  /*wdt reset whole chip enable*/
            uint32_t wdt_pause_in_slp              :    1;  /*pause WDT in sleep*/
            uint32_t wdt_flashboot_mod_en          :    1;  /*enable WDT in flash boot*/
            uint32_t wdt_sys_reset_length          :    3;  /*system reset counter length*/
            uint32_t wdt_cpu_reset_length          :    3;  /*CPU reset counter length*/
            uint32_t wdt_stg3                      :    3;  /*1: interrupt stage en; 2: CPU reset stage en; 3: system reset stage en ; 4: RTC reset stage en*/
            uint32_t wdt_stg2                      :    3;  /*1: interrupt stage en; 2: CPU reset stage en; 3: system reset stage en ; 4: RTC reset stage en*/
            uint32_t wdt_stg1                      :    3;  /*1: interrupt stage en; 2: CPU reset stage en; 3: system reset stage en ; 4: RTC reset stage en*/
            uint32_t wdt_stg0                      :    3;  /*1: interrupt stage en; 2: CPU reset stage en; 3: system reset stage en ; 4: RTC reset stage en*/
            uint32_t wdt_en                        :    1;
        };
        uint32_t val;
    } wdtconfig0;
    union {
        struct {
            uint32_t reserved0                     :    26;
            uint32_t wdt_lp_peri_reset_en          :    1;  /*enable WDT reset LP PERI*/
            uint32_t wdt_lp_cpu_reset_en           :    1;  /*enable WDT reset LP CPU*/
            uint32_t wdt_core3cpu_reset_en         :    1;  /*enable WDT reset CORE3 CPU*/
            uint32_t wdt_core2cpu_reset_en         :    1;  /*enable WDT reset CORE2 CPU*/
            uint32_t wdt_core1cpu_reset_en         :    1;  /*enable WDT reset CORE1 CPU*/
            uint32_t wdt_core0cpu_reset_en         :    1;  /*enable WDT reset CORE0 CPU*/
        };
        uint32_t val;
    } wdtcpurst;
    uint32_t wdtconfig1;
    uint32_t wdtconfig2;
    uint32_t wdtconfig3;
    uint32_t wdtconfig4;
    union {
        struct {
            uint32_t reserved0                     :    31;
            uint32_t wdt_feed                      :    1;
        };
        uint32_t val;
    } wdtfeed;
    uint32_t wdtwprotect;
    union {
        struct {
            uint32_t swd_reset_flag                :    1;  /*swd reset flag*/
            uint32_t swd_feed_int                  :    1;  /*swd interrupt for feeding*/
            uint32_t reserved2                     :    16;
            uint32_t swd_signal_width              :    10;  /*adjust signal width send to swd*/
            uint32_t swd_rst_flag_clr              :    1;  /*reset swd reset flag*/
            uint32_t swd_feed                      :    1;  /*Sw feed swd*/
            uint32_t swd_disable                   :    1;  /*disabel SWD*/
            uint32_t swd_auto_feed_en              :    1;  /*automatically feed swd when int comes*/
        };
        uint32_t val;
    } swd_conf;
    uint32_t swd_wprotect;
    union {
        struct {
            uint32_t clk_en                        :    1;
            uint32_t reserved1                     :    31;
        };
        uint32_t val;
    } wdt_clk_en;
    union {
        struct {
            uint32_t wdt                           :    1;  /*enable RTC WDT interrupt*/
            uint32_t swd                           :    1;  /*enable super watch dog interrupt*/
            uint32_t reserved2                     :    30;
        };
        uint32_t val;
    } int_ena_w1ts;
    union {
        struct {
            uint32_t wdt                           :    1;  /*enable RTC WDT interrupt*/
            uint32_t swd                           :    1;  /*enable super watch dog interrupt*/
            uint32_t reserved2                     :    30;
        };
        uint32_t val;
    } int_ena_w1tc;
    union {
        struct {
            uint32_t wdt                           :    1;  /*enable RTC WDT interrupt*/
            uint32_t swd                           :    1;  /*enable super watch dog interrupt*/
            uint32_t xtal32k_dead                  :    1;  /*enable xtal32k_dead  interrupt*/
            uint32_t reserved3                     :    29;
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t wdt                           :    1;  /*RTC WDT interrupt raw*/
            uint32_t swd                           :    1;  /*super watch dog interrupt raw*/
            uint32_t xtal32k_dead                  :    1;  /*xtal32k dead detection interrupt raw*/
            uint32_t reserved3                     :    29;
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t wdt                           :    1;  /*watch dog interrupt state*/
            uint32_t swd                           :    1;  /*super watch dog interrupt state*/
            uint32_t xtal32k_dead                  :    1;  /*xtal32k dead detection interrupt state*/
            uint32_t reserved3                     :    29;
        };
        uint32_t val;
    } int_swd_st;
    union {
        struct {
            uint32_t wdt                           :    1;  /*Clear RTC WDT interrupt state*/
            uint32_t swd                           :    1;  /*Clear super watch dog interrupt state*/
            uint32_t xtal32k_dead                  :    1;  /*Clear RTC WDT interrupt state*/
            uint32_t reserved3                     :    29;
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t xtal32k_wdt_en                :    1;  /*xtal 32k watch dog enable*/
            uint32_t xtal32k_wdt_clk_fo            :    1;  /*xtal 32k watch dog clock force on*/
            uint32_t xtal32k_wdt_reset             :    1;  /*xtal 32k watch dog sw reset*/
            uint32_t xtal32k_ext_clk_fo            :    1;  /*xtal 32k external xtal clock force on*/
            uint32_t xtal32k_auto_backup           :    1;  /*xtal 32k switch to back up clock when xtal is dead*/
            uint32_t xtal32k_auto_restart          :    1;  /*xtal 32k restart xtal when xtal is dead*/
            uint32_t xtal32k_auto_return           :    1;  /*xtal 32k switch back xtal when xtal is restarted*/
            uint32_t xtal32k_xpd_force             :    1;  /*Xtal 32k xpd control by sw or fsm*/
            uint32_t enckinit_xtal_32k             :    1;  /*apply an internal clock to help xtal 32k to start*/
            uint32_t dbuf_xtal_32k                 :    1;  /*0: single-end buffer 1: differential buffer*/
            uint32_t dgm_xtal_32k                  :    3;  /*xtal_32k gm control*/
            uint32_t dres_xtal_32k                 :    3;  /*DRES_XTAL_32K*/
            uint32_t xpd_xtal_32k                  :    1;  /*XPD_XTAL_32K*/
            uint32_t dac_xtal_32k                  :    3;  /*DAC_XTAL_32K*/
            uint32_t wdt_state                     :    3;  /*state of 32k_wdt*/
            uint32_t xtal32k_gpio_sel              :    1;  /*XTAL_32K sel. ; 0: external XTAL_32K; 1: CLK from RTC pad X32P_C*/
            uint32_t reserved24                    :    6;
            uint32_t xtl_ext_ctr_lv                :    1;  /*0: power down XTAL at high level; 1: power down XTAL at low level*/
            uint32_t xtl_ext_ctr_en                :    1;
        };
        uint32_t val;
    } ext_xtl_conf;
    uint32_t xtal32k_clk_factor;
    uint32_t reserved_4c;
    uint32_t reserved_50;
    uint32_t reserved_54;
    uint32_t reserved_58;
    union {
        struct {
            uint32_t xtal32k_return_wait           :    4;  /*cycles to wait to return noral xtal 32k*/
            uint32_t xtal32k_restart_wait          :    16;  /*cycles to wait to repower on xtal 32k*/
            uint32_t xtal32k_wdt_timeout           :    8;  /*If no clock detected for this amount of time,32k is regarded as dead*/
            uint32_t xtal32k_stable_thres          :    4;  /*if restarted xtal32k period is smaller than this, it is regarded as stable*/
        };
        uint32_t val;
    } xtal32k_conf;
    union {
        struct {
            uint32_t swd_disable_efuse_force       :    1;  /*swd disable default choose efuse control bit*/
            uint32_t wdt_flashboot_efuse_force     :    1;  /*lp_wdt flashboot en default choose efuse control bit*/
            uint32_t reserved2                     :    30;
        };
        uint32_t val;
    } efuse_force;
} lp_wdt_dev_t;

extern lp_wdt_dev_t LP_WDT;

#ifdef __cplusplus
}
#endif
