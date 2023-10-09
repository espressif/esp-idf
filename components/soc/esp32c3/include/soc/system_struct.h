/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_SYSTEM_STRUCT_H_
#define _SOC_SYSTEM_STRUCT_H_


#ifdef __cplusplus
extern "C" {
#endif
#include "soc.h"

typedef volatile struct system_dev_s {
    union {
        struct {
            uint32_t reserved0                     :    6;  /*reserved*/
            uint32_t reg_clk_en_assist_debug       :    1;  /*reg_clk_en_assist_debug*/
            uint32_t reg_clk_en_dedicated_gpio     :    1;  /*reg_clk_en_dedicated_gpio*/
            uint32_t reserved8                     :    24;  /*reserved*/
        };
        uint32_t val;
    } cpu_peri_clk_en;
    union {
        struct {
            uint32_t reserved0                     :    6;  /*reserved*/
            uint32_t reg_rst_en_assist_debug       :    1;  /*reg_rst_en_assist_debug*/
            uint32_t reg_rst_en_dedicated_gpio     :    1;  /*reg_rst_en_dedicated_gpio*/
            uint32_t reserved8                     :    24;  /*reserved*/
        };
        uint32_t val;
    } cpu_peri_rst_en;
    union {
        struct {
            uint32_t reg_cpuperiod_sel             :    2;  /*reg_cpuperiod_sel*/
            uint32_t reg_pll_freq_sel              :    1;  /*reg_pll_freq_sel*/
            uint32_t reg_cpu_wait_mode_force_on    :    1;  /*reg_cpu_wait_mode_force_on*/
            uint32_t reg_cpu_waiti_delay_num       :    4;  /*reg_cpu_waiti_delay_num*/
            uint32_t reserved8                     :    24;  /*reserved*/
        };
        uint32_t val;
    } cpu_per_conf;
    union {
        struct {
            uint32_t reg_lslp_mem_pd_mask          :    1;  /*reg_lslp_mem_pd_mask*/
            uint32_t reserved1                     :    31;  /*reserved*/
        };
        uint32_t val;
    } mem_pd_mask;
    union {
        struct {
            uint32_t reg_timers_clk_en             :    1;  /*reg_timers_clk_en*/
            uint32_t reg_spi01_clk_en              :    1;  /*reg_spi01_clk_en*/
            uint32_t reg_uart_clk_en               :    1;  /*reg_uart_clk_en*/
            uint32_t reg_wdg_clk_en                :    1;  /*reg_wdg_clk_en*/
            uint32_t reg_i2s0_clk_en               :    1;  /*reg_i2s0_clk_en*/
            uint32_t reg_uart1_clk_en              :    1;  /*reg_uart1_clk_en*/
            uint32_t reg_spi2_clk_en               :    1;  /*reg_spi2_clk_en*/
            uint32_t reg_i2c_ext0_clk_en           :    1;  /*reg_i2c_ext0_clk_en*/
            uint32_t reg_uhci0_clk_en              :    1;  /*reg_uhci0_clk_en*/
            uint32_t reg_rmt_clk_en                :    1;  /*reg_rmt_clk_en*/
            uint32_t reg_pcnt_clk_en               :    1;  /*reg_pcnt_clk_en*/
            uint32_t reg_ledc_clk_en               :    1;  /*reg_ledc_clk_en*/
            uint32_t reg_uhci1_clk_en              :    1;  /*reg_uhci1_clk_en*/
            uint32_t reg_timergroup_clk_en         :    1;  /*reg_timergroup_clk_en*/
            uint32_t reg_efuse_clk_en              :    1;  /*reg_efuse_clk_en*/
            uint32_t reg_timergroup1_clk_en        :    1;  /*reg_timergroup1_clk_en*/
            uint32_t reg_spi3_clk_en               :    1;  /*reg_spi3_clk_en*/
            uint32_t reg_pwm0_clk_en               :    1;  /*reg_pwm0_clk_en*/
            uint32_t reg_i2c_ext1_clk_en           :    1;  /*reg_i2c_ext1_clk_en*/
            uint32_t reg_can_clk_en                :    1;  /*reg_can_clk_en*/
            uint32_t reg_pwm1_clk_en               :    1;  /*reg_pwm1_clk_en*/
            uint32_t reg_i2s1_clk_en               :    1;  /*reg_i2s1_clk_en*/
            uint32_t reg_spi2_dma_clk_en           :    1;  /*reg_spi2_dma_clk_en*/
            uint32_t reg_usb_device_clk_en         :    1;  /*reg_usb_device_clk_en*/
            uint32_t reg_uart_mem_clk_en           :    1;  /*reg_uart_mem_clk_en*/
            uint32_t reg_pwm2_clk_en               :    1;  /*reg_pwm2_clk_en*/
            uint32_t reg_pwm3_clk_en               :    1;  /*reg_pwm3_clk_en*/
            uint32_t reg_spi3_dma_clk_en           :    1;  /*reg_spi3_dma_clk_en*/
            uint32_t reg_apb_saradc_clk_en         :    1;  /*reg_apb_saradc_clk_en*/
            uint32_t reg_systimer_clk_en           :    1;  /*reg_systimer_clk_en*/
            uint32_t reg_adc2_arb_clk_en           :    1;  /*reg_adc2_arb_clk_en*/
            uint32_t reg_spi4_clk_en               :    1;  /*reg_spi4_clk_en*/
        };
        uint32_t val;
    } perip_clk_en0;
    union {
        struct {
            uint32_t reserved0                     :    1;  /*peripheral reset register*/
            uint32_t reg_crypto_aes_clk_en         :    1;  /*reg_crypto_aes_clk_en*/
            uint32_t reg_crypto_sha_clk_en         :    1;  /*reg_crypto_sha_clk_en*/
            uint32_t reg_crypto_rsa_clk_en         :    1;  /*reg_crypto_rsa_clk_en*/
            uint32_t reg_crypto_ds_clk_en          :    1;  /*reg_crypto_ds_clk_en*/
            uint32_t reg_crypto_hmac_clk_en        :    1;  /*reg_crypto_hmac_clk_en*/
            uint32_t reg_dma_clk_en                :    1;  /*reg_dma_clk_en*/
            uint32_t reg_sdio_host_clk_en          :    1;  /*reg_sdio_host_clk_en*/
            uint32_t reg_lcd_cam_clk_en            :    1;  /*reg_lcd_cam_clk_en*/
            uint32_t reserved9                     :    1;  /*reserved*/
            uint32_t reg_tsens_clk_en              :    1;  /*reg_tsens_clk_en*/
            uint32_t reserved11                    :    21;  /*reserved*/
        };
        uint32_t val;
    } perip_clk_en1;
    union {
        struct {
            uint32_t reg_timers_rst                :    1;  /*reg_timers_rst*/
            uint32_t reg_spi01_rst                 :    1;  /*reg_spi01_rst*/
            uint32_t reg_uart_rst                  :    1;  /*reg_uart_rst*/
            uint32_t reg_wdg_rst                   :    1;  /*reg_wdg_rst*/
            uint32_t reg_i2s0_rst                  :    1;  /*reg_i2s0_rst*/
            uint32_t reg_uart1_rst                 :    1;  /*reg_uart1_rst*/
            uint32_t reg_spi2_rst                  :    1;  /*reg_spi2_rst*/
            uint32_t reg_i2c_ext0_rst              :    1;  /*reg_i2c_ext0_rst*/
            uint32_t reg_uhci0_rst                 :    1;  /*reg_uhci0_rst*/
            uint32_t reg_rmt_rst                   :    1;  /*reg_rmt_rst*/
            uint32_t reg_pcnt_rst                  :    1;  /*reg_pcnt_rst*/
            uint32_t reg_ledc_rst                  :    1;  /*reg_ledc_rst*/
            uint32_t reg_uhci1_rst                 :    1;  /*reg_uhci1_rst*/
            uint32_t reg_timergroup_rst            :    1;  /*reg_timergroup_rst*/
            uint32_t reg_efuse_rst                 :    1;  /*reg_efuse_rst*/
            uint32_t reg_timergroup1_rst           :    1;  /*reg_timergroup1_rst*/
            uint32_t reg_spi3_rst                  :    1;  /*reg_spi3_rst*/
            uint32_t reg_pwm0_rst                  :    1;  /*reg_pwm0_rst*/
            uint32_t reg_i2c_ext1_rst              :    1;  /*reg_i2c_ext1_rst*/
            uint32_t reg_can_rst                   :    1;  /*reg_can_rst*/
            uint32_t reg_pwm1_rst                  :    1;  /*reg_pwm1_rst*/
            uint32_t reg_i2s1_rst                  :    1;  /*reg_i2s1_rst*/
            uint32_t reg_spi2_dma_rst              :    1;  /*reg_spi2_dma_rst*/
            uint32_t reg_usb_device_rst            :    1;  /*reg_usb_device_rst*/
            uint32_t reg_uart_mem_rst              :    1;  /*reg_uart_mem_rst*/
            uint32_t reg_pwm2_rst                  :    1;  /*reg_pwm2_rst*/
            uint32_t reg_pwm3_rst                  :    1;  /*reg_pwm3_rst*/
            uint32_t reg_spi3_dma_rst              :    1;  /*reg_spi3_dma_rst*/
            uint32_t reg_apb_saradc_rst            :    1;  /*reg_apb_saradc_rst*/
            uint32_t reg_systimer_rst              :    1;  /*reg_systimer_rst*/
            uint32_t reg_adc2_arb_rst              :    1;  /*reg_adc2_arb_rst*/
            uint32_t reg_spi4_rst                  :    1;  /*reg_spi4_rst*/
        };
        uint32_t val;
    } perip_rst_en0;
    union {
        struct {
            uint32_t reserved0                     :    1;  /*reserved*/
            uint32_t reg_crypto_aes_rst            :    1;  /*reg_crypto_aes_rst*/
            uint32_t reg_crypto_sha_rst            :    1;  /*reg_crypto_sha_rst*/
            uint32_t reg_crypto_rsa_rst            :    1;  /*reg_crypto_rsa_rst*/
            uint32_t reg_crypto_ds_rst             :    1;  /*reg_crypto_ds_rst*/
            uint32_t reg_crypto_hmac_rst           :    1;  /*reg_crypto_hmac_rst*/
            uint32_t reg_dma_rst                   :    1;  /*reg_dma_rst*/
            uint32_t reg_sdio_host_rst             :    1;  /*reg_sdio_host_rst*/
            uint32_t reg_lcd_cam_rst               :    1;  /*reg_lcd_cam_rst*/
            uint32_t reserved9                     :    1;  /*reserved*/
            uint32_t reg_tsens_rst                 :    1;  /*reg_tsens_rst*/
            uint32_t reserved11                    :    21;  /*reserved*/
        };
        uint32_t val;
    } perip_rst_en1;
    union {
        struct {
            uint32_t reg_bt_lpck_div_num           :    12;  /*reg_bt_lpck_div_num*/
            uint32_t reserved12                    :    20;  /*reserved*/
        };
        uint32_t val;
    } bt_lpck_div_int;
    union {
        struct {
            uint32_t reg_bt_lpck_div_b             :    12;  /*reg_bt_lpck_div_b*/
            uint32_t reg_bt_lpck_div_a             :    12;  /*reg_bt_lpck_div_a*/
            uint32_t reg_lpclk_sel_rtc_slow        :    1;  /*reg_lpclk_sel_rtc_slow*/
            uint32_t reg_lpclk_sel_8m              :    1;  /*reg_lpclk_sel_8m*/
            uint32_t reg_lpclk_sel_xtal            :    1;  /*reg_lpclk_sel_xtal*/
            uint32_t reg_lpclk_sel_xtal32k         :    1;  /*reg_lpclk_sel_xtal32k*/
            uint32_t reg_lpclk_rtc_en              :    1;  /*reg_lpclk_rtc_en*/
            uint32_t reserved29                    :    3;  /*reserved*/
        };
        uint32_t val;
    } bt_lpck_div_frac;
    union {
        struct {
            uint32_t reg_cpu_intr_from_cpu_0       :    1;  /*reg_cpu_intr_from_cpu_0*/
            uint32_t reserved1                     :    31;  /*reserved*/
        };
        uint32_t val;
    } cpu_intr_from_cpu_0;
    union {
        struct {
            uint32_t reg_cpu_intr_from_cpu_1       :    1;  /*reg_cpu_intr_from_cpu_1*/
            uint32_t reserved1                     :    31;  /*reserved*/
        };
        uint32_t val;
    } cpu_intr_from_cpu_1;
    union {
        struct {
            uint32_t reg_cpu_intr_from_cpu_2       :    1;  /*reg_cpu_intr_from_cpu_2*/
            uint32_t reserved1                     :    31;  /*reserved*/
        };
        uint32_t val;
    } cpu_intr_from_cpu_2;
    union {
        struct {
            uint32_t reg_cpu_intr_from_cpu_3       :    1;  /*reg_cpu_intr_from_cpu_3*/
            uint32_t reserved1                     :    31;  /*reserved*/
        };
        uint32_t val;
    } cpu_intr_from_cpu_3;
    union {
        struct {
            uint32_t reg_rsa_mem_pd                :    1;  /*reg_rsa_mem_pd*/
            uint32_t reg_rsa_mem_force_pu          :    1;  /*reg_rsa_mem_force_pu*/
            uint32_t reg_rsa_mem_force_pd          :    1;  /*reg_rsa_mem_force_pd*/
            uint32_t reserved3                     :    29;  /*reserved*/
        };
        uint32_t val;
    } rsa_pd_ctrl;
    union {
        struct {
            uint32_t reg_edma_clk_on               :    1;  /*reg_edma_clk_on*/
            uint32_t reg_edma_reset                :    1;  /*reg_edma_reset*/
            uint32_t reserved2                     :    30;  /*reserved*/
        };
        uint32_t val;
    } edma_ctrl;
    union {
        struct {
            uint32_t reg_icache_clk_on             :    1;  /*reg_icache_clk_on*/
            uint32_t reg_icache_reset              :    1;  /*reg_icache_reset*/
            uint32_t reg_dcache_clk_on             :    1;  /*reg_dcache_clk_on*/
            uint32_t reg_dcache_reset              :    1;  /*reg_dcache_reset*/
            uint32_t reserved4                     :    28;  /*reserved*/
        };
        uint32_t val;
    } cache_control;
    union {
        struct {
            uint32_t reg_enable_spi_manual_encrypt :    1;  /*reg_enable_spi_manual_encrypt*/
            uint32_t reg_enable_download_db_encrypt:    1;  /*reg_enable_download_db_encrypt*/
            uint32_t reg_enable_download_g0cb_decrypt:    1;  /*reg_enable_download_g0cb_decrypt*/
            uint32_t reg_enable_download_manual_encrypt:    1;  /*reg_enable_download_manual_encrypt*/
            uint32_t reserved4                     :    28;  /*reserved*/
        };
        uint32_t val;
    } external_device_encrypt_decrypt_control;
    union {
        struct {
            uint32_t reserved0                     :    8;  /*fast memory crc register*/
            uint32_t reg_rtc_mem_crc_start         :    1;  /*reg_rtc_mem_crc_start*/
            uint32_t reg_rtc_mem_crc_addr          :    11;  /*reg_rtc_mem_crc_addr*/
            uint32_t reg_rtc_mem_crc_len           :    11;  /*reg_rtc_mem_crc_len*/
            uint32_t reg_rtc_mem_crc_finish        :    1;  /*reg_rtc_mem_crc_finish*/
        };
        uint32_t val;
    } rtc_fastmem_config;
    uint32_t rtc_fastmem_crc;
    union {
        struct {
            uint32_t reg_redundant_eco_drive       :    1;  /*reg_redundant_eco_drive*/
            uint32_t reg_redundant_eco_result      :    1;  /*reg_redundant_eco_result*/
            uint32_t reserved2                     :    30;  /*reserved*/
        };
        uint32_t val;
    } redundant_eco_ctrl;
    union {
        struct {
            uint32_t reg_clk_en                    :    1;  /*reg_clk_en*/
            uint32_t reserved1                     :    31;  /*reserved*/
        };
        uint32_t val;
    } clock_gate;
    union {
        struct {
            uint32_t reg_pre_div_cnt               :    10;  /*reg_pre_div_cnt*/
            uint32_t reg_soc_clk_sel               :    2;  /*reg_soc_clk_sel*/
            uint32_t reg_clk_xtal_freq             :    7;  /*reg_clk_xtal_freq*/
            uint32_t reg_clk_div_en                :    1;  /*reg_clk_div_en*/
            uint32_t reserved20                    :    12;  /*reserved*/
        };
        uint32_t val;
    } sysclk_conf;
    union {
        struct {
            uint32_t reg_mem_path_len              :    4;  /*reg_mem_path_len*/
            uint32_t reg_mem_err_cnt_clr           :    1;  /*reg_mem_err_cnt_clr*/
            uint32_t reg_mem_pvt_monitor_en        :    1;  /*reg_mem_pvt_monitor_en*/
            uint32_t reg_mem_timing_err_cnt        :    16;  /*reg_mem_timing_err_cnt*/
            uint32_t reg_mem_vt_sel                :    2;  /*reg_mem_vt_sel*/
            uint32_t reserved24                    :    8;  /*reserved*/
        };
        uint32_t val;
    } mem_pvt;
    union {
        struct {
            uint32_t reg_comb_path_len_lvt         :    5;  /*reg_comb_path_len_lvt*/
            uint32_t reg_comb_err_cnt_clr_lvt      :    1;  /*reg_comb_err_cnt_clr_lvt*/
            uint32_t reg_comb_pvt_monitor_en_lvt   :    1;  /*reg_comb_pvt_monitor_en_lvt*/
            uint32_t reserved7                     :    18;  /*reserved*/
            uint32_t reserved25                    :    7;  /*reserved*/
        };
        uint32_t val;
    } comb_pvt_lvt_conf;
    union {
        struct {
            uint32_t reg_comb_path_len_nvt         :    5;  /*reg_comb_path_len_nvt*/
            uint32_t reg_comb_err_cnt_clr_nvt      :    1;  /*reg_comb_err_cnt_clr_nvt*/
            uint32_t reg_comb_pvt_monitor_en_nvt   :    1;  /*reg_comb_pvt_monitor_en_nvt*/
            uint32_t reserved7                     :    18;  /*reserved*/
            uint32_t reserved25                    :    7;  /*reserved*/
        };
        uint32_t val;
    } comb_pvt_nvt_conf;
    union {
        struct {
            uint32_t reg_comb_path_len_hvt         :    5;  /*reg_comb_path_len_hvt*/
            uint32_t reg_comb_err_cnt_clr_hvt      :    1;  /*reg_comb_err_cnt_clr_hvt*/
            uint32_t reg_comb_pvt_monitor_en_hvt   :    1;  /*reg_comb_pvt_monitor_en_hvt*/
            uint32_t reserved7                     :    18;  /*reserved*/
            uint32_t reserved25                    :    7;  /*reserved*/
        };
        uint32_t val;
    } comb_pvt_hvt_conf;
    union {
        struct {
            uint32_t reg_comb_timing_err_cnt_lvt_site0:    16;  /*reg_comb_timing_err_cnt_lvt_site0*/
            uint32_t reserved16                    :    16;  /*reserved*/
        };
        uint32_t val;
    } comb_pvt_err_lvt_site0;
    union {
        struct {
            uint32_t reg_comb_timing_err_cnt_nvt_site0:    16;  /*reg_comb_timing_err_cnt_nvt_site0*/
            uint32_t reserved16                    :    16;  /*reserved*/
        };
        uint32_t val;
    } comb_pvt_err_nvt_site0;
    union {
        struct {
            uint32_t reg_comb_timing_err_cnt_hvt_site0:    16;  /*reg_comb_timing_err_cnt_hvt_site0*/
            uint32_t reserved16                    :    16;  /*reserved*/
        };
        uint32_t val;
    } comb_pvt_err_hvt_site0;
    union {
        struct {
            uint32_t reg_comb_timing_err_cnt_lvt_site1:    16;  /*reg_comb_timing_err_cnt_lvt_site1*/
            uint32_t reserved16                    :    16;  /*reserved*/
        };
        uint32_t val;
    } comb_pvt_err_lvt_site1;
    union {
        struct {
            uint32_t reg_comb_timing_err_cnt_nvt_site1:    16;  /*reg_comb_timing_err_cnt_nvt_site1*/
            uint32_t reserved16                    :    16;  /*reserved*/
        };
        uint32_t val;
    } comb_pvt_err_nvt_site1;
    union {
        struct {
            uint32_t reg_comb_timing_err_cnt_hvt_site1:    16;  /*reg_comb_timing_err_cnt_hvt_site1*/
            uint32_t reserved16                    :    16;  /*reserved*/
        };
        uint32_t val;
    } comb_pvt_err_hvt_site1;
    union {
        struct {
            uint32_t reg_comb_timing_err_cnt_lvt_site2:    16;  /*reg_comb_timing_err_cnt_lvt_site2*/
            uint32_t reserved16                    :    16;  /*reserved*/
        };
        uint32_t val;
    } comb_pvt_err_lvt_site2;
    union {
        struct {
            uint32_t reg_comb_timing_err_cnt_nvt_site2:    16;  /*reg_comb_timing_err_cnt_nvt_site2*/
            uint32_t reserved16                    :    16;  /*reserved*/
        };
        uint32_t val;
    } comb_pvt_err_nvt_site2;
    union {
        struct {
            uint32_t reg_comb_timing_err_cnt_hvt_site2:    16;  /*reg_comb_timing_err_cnt_hvt_site2*/
            uint32_t reserved16                    :    16;  /*reserved*/
        };
        uint32_t val;
    } comb_pvt_err_hvt_site2;
    union {
        struct {
            uint32_t reg_comb_timing_err_cnt_lvt_site3:    16;  /*reg_comb_timing_err_cnt_lvt_site3*/
            uint32_t reserved16                    :    16;  /*reserved*/
        };
        uint32_t val;
    } comb_pvt_err_lvt_site3;
    union {
        struct {
            uint32_t reg_comb_timing_err_cnt_nvt_site3:    16;  /*reg_comb_timing_err_cnt_nvt_site3*/
            uint32_t reserved16                    :    16;  /*reserved*/
        };
        uint32_t val;
    } comb_pvt_err_nvt_site3;
    union {
        struct {
            uint32_t reg_comb_timing_err_cnt_hvt_site3:    16;  /*reg_comb_timing_err_cnt_hvt_site3*/
            uint32_t reserved16                    :    16;  /*reserved*/
        };
        uint32_t val;
    } comb_pvt_err_hvt_site3;
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
    uint32_t reserved_fc;
    uint32_t reserved_100;
    uint32_t reserved_104;
    uint32_t reserved_108;
    uint32_t reserved_10c;
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
    uint32_t reserved_1fc;
    uint32_t reserved_200;
    uint32_t reserved_204;
    uint32_t reserved_208;
    uint32_t reserved_20c;
    uint32_t reserved_210;
    uint32_t reserved_214;
    uint32_t reserved_218;
    uint32_t reserved_21c;
    uint32_t reserved_220;
    uint32_t reserved_224;
    uint32_t reserved_228;
    uint32_t reserved_22c;
    uint32_t reserved_230;
    uint32_t reserved_234;
    uint32_t reserved_238;
    uint32_t reserved_23c;
    uint32_t reserved_240;
    uint32_t reserved_244;
    uint32_t reserved_248;
    uint32_t reserved_24c;
    uint32_t reserved_250;
    uint32_t reserved_254;
    uint32_t reserved_258;
    uint32_t reserved_25c;
    uint32_t reserved_260;
    uint32_t reserved_264;
    uint32_t reserved_268;
    uint32_t reserved_26c;
    uint32_t reserved_270;
    uint32_t reserved_274;
    uint32_t reserved_278;
    uint32_t reserved_27c;
    uint32_t reserved_280;
    uint32_t reserved_284;
    uint32_t reserved_288;
    uint32_t reserved_28c;
    uint32_t reserved_290;
    uint32_t reserved_294;
    uint32_t reserved_298;
    uint32_t reserved_29c;
    uint32_t reserved_2a0;
    uint32_t reserved_2a4;
    uint32_t reserved_2a8;
    uint32_t reserved_2ac;
    uint32_t reserved_2b0;
    uint32_t reserved_2b4;
    uint32_t reserved_2b8;
    uint32_t reserved_2bc;
    uint32_t reserved_2c0;
    uint32_t reserved_2c4;
    uint32_t reserved_2c8;
    uint32_t reserved_2cc;
    uint32_t reserved_2d0;
    uint32_t reserved_2d4;
    uint32_t reserved_2d8;
    uint32_t reserved_2dc;
    uint32_t reserved_2e0;
    uint32_t reserved_2e4;
    uint32_t reserved_2e8;
    uint32_t reserved_2ec;
    uint32_t reserved_2f0;
    uint32_t reserved_2f4;
    uint32_t reserved_2f8;
    uint32_t reserved_2fc;
    uint32_t reserved_300;
    uint32_t reserved_304;
    uint32_t reserved_308;
    uint32_t reserved_30c;
    uint32_t reserved_310;
    uint32_t reserved_314;
    uint32_t reserved_318;
    uint32_t reserved_31c;
    uint32_t reserved_320;
    uint32_t reserved_324;
    uint32_t reserved_328;
    uint32_t reserved_32c;
    uint32_t reserved_330;
    uint32_t reserved_334;
    uint32_t reserved_338;
    uint32_t reserved_33c;
    uint32_t reserved_340;
    uint32_t reserved_344;
    uint32_t reserved_348;
    uint32_t reserved_34c;
    uint32_t reserved_350;
    uint32_t reserved_354;
    uint32_t reserved_358;
    uint32_t reserved_35c;
    uint32_t reserved_360;
    uint32_t reserved_364;
    uint32_t reserved_368;
    uint32_t reserved_36c;
    uint32_t reserved_370;
    uint32_t reserved_374;
    uint32_t reserved_378;
    uint32_t reserved_37c;
    uint32_t reserved_380;
    uint32_t reserved_384;
    uint32_t reserved_388;
    uint32_t reserved_38c;
    uint32_t reserved_390;
    uint32_t reserved_394;
    uint32_t reserved_398;
    uint32_t reserved_39c;
    uint32_t reserved_3a0;
    uint32_t reserved_3a4;
    uint32_t reserved_3a8;
    uint32_t reserved_3ac;
    uint32_t reserved_3b0;
    uint32_t reserved_3b4;
    uint32_t reserved_3b8;
    uint32_t reserved_3bc;
    uint32_t reserved_3c0;
    uint32_t reserved_3c4;
    uint32_t reserved_3c8;
    uint32_t reserved_3cc;
    uint32_t reserved_3d0;
    uint32_t reserved_3d4;
    uint32_t reserved_3d8;
    uint32_t reserved_3dc;
    uint32_t reserved_3e0;
    uint32_t reserved_3e4;
    uint32_t reserved_3e8;
    uint32_t reserved_3ec;
    uint32_t reserved_3f0;
    uint32_t reserved_3f4;
    uint32_t reserved_3f8;
    uint32_t reserved_3fc;
    uint32_t reserved_400;
    uint32_t reserved_404;
    uint32_t reserved_408;
    uint32_t reserved_40c;
    uint32_t reserved_410;
    uint32_t reserved_414;
    uint32_t reserved_418;
    uint32_t reserved_41c;
    uint32_t reserved_420;
    uint32_t reserved_424;
    uint32_t reserved_428;
    uint32_t reserved_42c;
    uint32_t reserved_430;
    uint32_t reserved_434;
    uint32_t reserved_438;
    uint32_t reserved_43c;
    uint32_t reserved_440;
    uint32_t reserved_444;
    uint32_t reserved_448;
    uint32_t reserved_44c;
    uint32_t reserved_450;
    uint32_t reserved_454;
    uint32_t reserved_458;
    uint32_t reserved_45c;
    uint32_t reserved_460;
    uint32_t reserved_464;
    uint32_t reserved_468;
    uint32_t reserved_46c;
    uint32_t reserved_470;
    uint32_t reserved_474;
    uint32_t reserved_478;
    uint32_t reserved_47c;
    uint32_t reserved_480;
    uint32_t reserved_484;
    uint32_t reserved_488;
    uint32_t reserved_48c;
    uint32_t reserved_490;
    uint32_t reserved_494;
    uint32_t reserved_498;
    uint32_t reserved_49c;
    uint32_t reserved_4a0;
    uint32_t reserved_4a4;
    uint32_t reserved_4a8;
    uint32_t reserved_4ac;
    uint32_t reserved_4b0;
    uint32_t reserved_4b4;
    uint32_t reserved_4b8;
    uint32_t reserved_4bc;
    uint32_t reserved_4c0;
    uint32_t reserved_4c4;
    uint32_t reserved_4c8;
    uint32_t reserved_4cc;
    uint32_t reserved_4d0;
    uint32_t reserved_4d4;
    uint32_t reserved_4d8;
    uint32_t reserved_4dc;
    uint32_t reserved_4e0;
    uint32_t reserved_4e4;
    uint32_t reserved_4e8;
    uint32_t reserved_4ec;
    uint32_t reserved_4f0;
    uint32_t reserved_4f4;
    uint32_t reserved_4f8;
    uint32_t reserved_4fc;
    uint32_t reserved_500;
    uint32_t reserved_504;
    uint32_t reserved_508;
    uint32_t reserved_50c;
    uint32_t reserved_510;
    uint32_t reserved_514;
    uint32_t reserved_518;
    uint32_t reserved_51c;
    uint32_t reserved_520;
    uint32_t reserved_524;
    uint32_t reserved_528;
    uint32_t reserved_52c;
    uint32_t reserved_530;
    uint32_t reserved_534;
    uint32_t reserved_538;
    uint32_t reserved_53c;
    uint32_t reserved_540;
    uint32_t reserved_544;
    uint32_t reserved_548;
    uint32_t reserved_54c;
    uint32_t reserved_550;
    uint32_t reserved_554;
    uint32_t reserved_558;
    uint32_t reserved_55c;
    uint32_t reserved_560;
    uint32_t reserved_564;
    uint32_t reserved_568;
    uint32_t reserved_56c;
    uint32_t reserved_570;
    uint32_t reserved_574;
    uint32_t reserved_578;
    uint32_t reserved_57c;
    uint32_t reserved_580;
    uint32_t reserved_584;
    uint32_t reserved_588;
    uint32_t reserved_58c;
    uint32_t reserved_590;
    uint32_t reserved_594;
    uint32_t reserved_598;
    uint32_t reserved_59c;
    uint32_t reserved_5a0;
    uint32_t reserved_5a4;
    uint32_t reserved_5a8;
    uint32_t reserved_5ac;
    uint32_t reserved_5b0;
    uint32_t reserved_5b4;
    uint32_t reserved_5b8;
    uint32_t reserved_5bc;
    uint32_t reserved_5c0;
    uint32_t reserved_5c4;
    uint32_t reserved_5c8;
    uint32_t reserved_5cc;
    uint32_t reserved_5d0;
    uint32_t reserved_5d4;
    uint32_t reserved_5d8;
    uint32_t reserved_5dc;
    uint32_t reserved_5e0;
    uint32_t reserved_5e4;
    uint32_t reserved_5e8;
    uint32_t reserved_5ec;
    uint32_t reserved_5f0;
    uint32_t reserved_5f4;
    uint32_t reserved_5f8;
    uint32_t reserved_5fc;
    uint32_t reserved_600;
    uint32_t reserved_604;
    uint32_t reserved_608;
    uint32_t reserved_60c;
    uint32_t reserved_610;
    uint32_t reserved_614;
    uint32_t reserved_618;
    uint32_t reserved_61c;
    uint32_t reserved_620;
    uint32_t reserved_624;
    uint32_t reserved_628;
    uint32_t reserved_62c;
    uint32_t reserved_630;
    uint32_t reserved_634;
    uint32_t reserved_638;
    uint32_t reserved_63c;
    uint32_t reserved_640;
    uint32_t reserved_644;
    uint32_t reserved_648;
    uint32_t reserved_64c;
    uint32_t reserved_650;
    uint32_t reserved_654;
    uint32_t reserved_658;
    uint32_t reserved_65c;
    uint32_t reserved_660;
    uint32_t reserved_664;
    uint32_t reserved_668;
    uint32_t reserved_66c;
    uint32_t reserved_670;
    uint32_t reserved_674;
    uint32_t reserved_678;
    uint32_t reserved_67c;
    uint32_t reserved_680;
    uint32_t reserved_684;
    uint32_t reserved_688;
    uint32_t reserved_68c;
    uint32_t reserved_690;
    uint32_t reserved_694;
    uint32_t reserved_698;
    uint32_t reserved_69c;
    uint32_t reserved_6a0;
    uint32_t reserved_6a4;
    uint32_t reserved_6a8;
    uint32_t reserved_6ac;
    uint32_t reserved_6b0;
    uint32_t reserved_6b4;
    uint32_t reserved_6b8;
    uint32_t reserved_6bc;
    uint32_t reserved_6c0;
    uint32_t reserved_6c4;
    uint32_t reserved_6c8;
    uint32_t reserved_6cc;
    uint32_t reserved_6d0;
    uint32_t reserved_6d4;
    uint32_t reserved_6d8;
    uint32_t reserved_6dc;
    uint32_t reserved_6e0;
    uint32_t reserved_6e4;
    uint32_t reserved_6e8;
    uint32_t reserved_6ec;
    uint32_t reserved_6f0;
    uint32_t reserved_6f4;
    uint32_t reserved_6f8;
    uint32_t reserved_6fc;
    uint32_t reserved_700;
    uint32_t reserved_704;
    uint32_t reserved_708;
    uint32_t reserved_70c;
    uint32_t reserved_710;
    uint32_t reserved_714;
    uint32_t reserved_718;
    uint32_t reserved_71c;
    uint32_t reserved_720;
    uint32_t reserved_724;
    uint32_t reserved_728;
    uint32_t reserved_72c;
    uint32_t reserved_730;
    uint32_t reserved_734;
    uint32_t reserved_738;
    uint32_t reserved_73c;
    uint32_t reserved_740;
    uint32_t reserved_744;
    uint32_t reserved_748;
    uint32_t reserved_74c;
    uint32_t reserved_750;
    uint32_t reserved_754;
    uint32_t reserved_758;
    uint32_t reserved_75c;
    uint32_t reserved_760;
    uint32_t reserved_764;
    uint32_t reserved_768;
    uint32_t reserved_76c;
    uint32_t reserved_770;
    uint32_t reserved_774;
    uint32_t reserved_778;
    uint32_t reserved_77c;
    uint32_t reserved_780;
    uint32_t reserved_784;
    uint32_t reserved_788;
    uint32_t reserved_78c;
    uint32_t reserved_790;
    uint32_t reserved_794;
    uint32_t reserved_798;
    uint32_t reserved_79c;
    uint32_t reserved_7a0;
    uint32_t reserved_7a4;
    uint32_t reserved_7a8;
    uint32_t reserved_7ac;
    uint32_t reserved_7b0;
    uint32_t reserved_7b4;
    uint32_t reserved_7b8;
    uint32_t reserved_7bc;
    uint32_t reserved_7c0;
    uint32_t reserved_7c4;
    uint32_t reserved_7c8;
    uint32_t reserved_7cc;
    uint32_t reserved_7d0;
    uint32_t reserved_7d4;
    uint32_t reserved_7d8;
    uint32_t reserved_7dc;
    uint32_t reserved_7e0;
    uint32_t reserved_7e4;
    uint32_t reserved_7e8;
    uint32_t reserved_7ec;
    uint32_t reserved_7f0;
    uint32_t reserved_7f4;
    uint32_t reserved_7f8;
    uint32_t reserved_7fc;
    uint32_t reserved_800;
    uint32_t reserved_804;
    uint32_t reserved_808;
    uint32_t reserved_80c;
    uint32_t reserved_810;
    uint32_t reserved_814;
    uint32_t reserved_818;
    uint32_t reserved_81c;
    uint32_t reserved_820;
    uint32_t reserved_824;
    uint32_t reserved_828;
    uint32_t reserved_82c;
    uint32_t reserved_830;
    uint32_t reserved_834;
    uint32_t reserved_838;
    uint32_t reserved_83c;
    uint32_t reserved_840;
    uint32_t reserved_844;
    uint32_t reserved_848;
    uint32_t reserved_84c;
    uint32_t reserved_850;
    uint32_t reserved_854;
    uint32_t reserved_858;
    uint32_t reserved_85c;
    uint32_t reserved_860;
    uint32_t reserved_864;
    uint32_t reserved_868;
    uint32_t reserved_86c;
    uint32_t reserved_870;
    uint32_t reserved_874;
    uint32_t reserved_878;
    uint32_t reserved_87c;
    uint32_t reserved_880;
    uint32_t reserved_884;
    uint32_t reserved_888;
    uint32_t reserved_88c;
    uint32_t reserved_890;
    uint32_t reserved_894;
    uint32_t reserved_898;
    uint32_t reserved_89c;
    uint32_t reserved_8a0;
    uint32_t reserved_8a4;
    uint32_t reserved_8a8;
    uint32_t reserved_8ac;
    uint32_t reserved_8b0;
    uint32_t reserved_8b4;
    uint32_t reserved_8b8;
    uint32_t reserved_8bc;
    uint32_t reserved_8c0;
    uint32_t reserved_8c4;
    uint32_t reserved_8c8;
    uint32_t reserved_8cc;
    uint32_t reserved_8d0;
    uint32_t reserved_8d4;
    uint32_t reserved_8d8;
    uint32_t reserved_8dc;
    uint32_t reserved_8e0;
    uint32_t reserved_8e4;
    uint32_t reserved_8e8;
    uint32_t reserved_8ec;
    uint32_t reserved_8f0;
    uint32_t reserved_8f4;
    uint32_t reserved_8f8;
    uint32_t reserved_8fc;
    uint32_t reserved_900;
    uint32_t reserved_904;
    uint32_t reserved_908;
    uint32_t reserved_90c;
    uint32_t reserved_910;
    uint32_t reserved_914;
    uint32_t reserved_918;
    uint32_t reserved_91c;
    uint32_t reserved_920;
    uint32_t reserved_924;
    uint32_t reserved_928;
    uint32_t reserved_92c;
    uint32_t reserved_930;
    uint32_t reserved_934;
    uint32_t reserved_938;
    uint32_t reserved_93c;
    uint32_t reserved_940;
    uint32_t reserved_944;
    uint32_t reserved_948;
    uint32_t reserved_94c;
    uint32_t reserved_950;
    uint32_t reserved_954;
    uint32_t reserved_958;
    uint32_t reserved_95c;
    uint32_t reserved_960;
    uint32_t reserved_964;
    uint32_t reserved_968;
    uint32_t reserved_96c;
    uint32_t reserved_970;
    uint32_t reserved_974;
    uint32_t reserved_978;
    uint32_t reserved_97c;
    uint32_t reserved_980;
    uint32_t reserved_984;
    uint32_t reserved_988;
    uint32_t reserved_98c;
    uint32_t reserved_990;
    uint32_t reserved_994;
    uint32_t reserved_998;
    uint32_t reserved_99c;
    uint32_t reserved_9a0;
    uint32_t reserved_9a4;
    uint32_t reserved_9a8;
    uint32_t reserved_9ac;
    uint32_t reserved_9b0;
    uint32_t reserved_9b4;
    uint32_t reserved_9b8;
    uint32_t reserved_9bc;
    uint32_t reserved_9c0;
    uint32_t reserved_9c4;
    uint32_t reserved_9c8;
    uint32_t reserved_9cc;
    uint32_t reserved_9d0;
    uint32_t reserved_9d4;
    uint32_t reserved_9d8;
    uint32_t reserved_9dc;
    uint32_t reserved_9e0;
    uint32_t reserved_9e4;
    uint32_t reserved_9e8;
    uint32_t reserved_9ec;
    uint32_t reserved_9f0;
    uint32_t reserved_9f4;
    uint32_t reserved_9f8;
    uint32_t reserved_9fc;
    uint32_t reserved_a00;
    uint32_t reserved_a04;
    uint32_t reserved_a08;
    uint32_t reserved_a0c;
    uint32_t reserved_a10;
    uint32_t reserved_a14;
    uint32_t reserved_a18;
    uint32_t reserved_a1c;
    uint32_t reserved_a20;
    uint32_t reserved_a24;
    uint32_t reserved_a28;
    uint32_t reserved_a2c;
    uint32_t reserved_a30;
    uint32_t reserved_a34;
    uint32_t reserved_a38;
    uint32_t reserved_a3c;
    uint32_t reserved_a40;
    uint32_t reserved_a44;
    uint32_t reserved_a48;
    uint32_t reserved_a4c;
    uint32_t reserved_a50;
    uint32_t reserved_a54;
    uint32_t reserved_a58;
    uint32_t reserved_a5c;
    uint32_t reserved_a60;
    uint32_t reserved_a64;
    uint32_t reserved_a68;
    uint32_t reserved_a6c;
    uint32_t reserved_a70;
    uint32_t reserved_a74;
    uint32_t reserved_a78;
    uint32_t reserved_a7c;
    uint32_t reserved_a80;
    uint32_t reserved_a84;
    uint32_t reserved_a88;
    uint32_t reserved_a8c;
    uint32_t reserved_a90;
    uint32_t reserved_a94;
    uint32_t reserved_a98;
    uint32_t reserved_a9c;
    uint32_t reserved_aa0;
    uint32_t reserved_aa4;
    uint32_t reserved_aa8;
    uint32_t reserved_aac;
    uint32_t reserved_ab0;
    uint32_t reserved_ab4;
    uint32_t reserved_ab8;
    uint32_t reserved_abc;
    uint32_t reserved_ac0;
    uint32_t reserved_ac4;
    uint32_t reserved_ac8;
    uint32_t reserved_acc;
    uint32_t reserved_ad0;
    uint32_t reserved_ad4;
    uint32_t reserved_ad8;
    uint32_t reserved_adc;
    uint32_t reserved_ae0;
    uint32_t reserved_ae4;
    uint32_t reserved_ae8;
    uint32_t reserved_aec;
    uint32_t reserved_af0;
    uint32_t reserved_af4;
    uint32_t reserved_af8;
    uint32_t reserved_afc;
    uint32_t reserved_b00;
    uint32_t reserved_b04;
    uint32_t reserved_b08;
    uint32_t reserved_b0c;
    uint32_t reserved_b10;
    uint32_t reserved_b14;
    uint32_t reserved_b18;
    uint32_t reserved_b1c;
    uint32_t reserved_b20;
    uint32_t reserved_b24;
    uint32_t reserved_b28;
    uint32_t reserved_b2c;
    uint32_t reserved_b30;
    uint32_t reserved_b34;
    uint32_t reserved_b38;
    uint32_t reserved_b3c;
    uint32_t reserved_b40;
    uint32_t reserved_b44;
    uint32_t reserved_b48;
    uint32_t reserved_b4c;
    uint32_t reserved_b50;
    uint32_t reserved_b54;
    uint32_t reserved_b58;
    uint32_t reserved_b5c;
    uint32_t reserved_b60;
    uint32_t reserved_b64;
    uint32_t reserved_b68;
    uint32_t reserved_b6c;
    uint32_t reserved_b70;
    uint32_t reserved_b74;
    uint32_t reserved_b78;
    uint32_t reserved_b7c;
    uint32_t reserved_b80;
    uint32_t reserved_b84;
    uint32_t reserved_b88;
    uint32_t reserved_b8c;
    uint32_t reserved_b90;
    uint32_t reserved_b94;
    uint32_t reserved_b98;
    uint32_t reserved_b9c;
    uint32_t reserved_ba0;
    uint32_t reserved_ba4;
    uint32_t reserved_ba8;
    uint32_t reserved_bac;
    uint32_t reserved_bb0;
    uint32_t reserved_bb4;
    uint32_t reserved_bb8;
    uint32_t reserved_bbc;
    uint32_t reserved_bc0;
    uint32_t reserved_bc4;
    uint32_t reserved_bc8;
    uint32_t reserved_bcc;
    uint32_t reserved_bd0;
    uint32_t reserved_bd4;
    uint32_t reserved_bd8;
    uint32_t reserved_bdc;
    uint32_t reserved_be0;
    uint32_t reserved_be4;
    uint32_t reserved_be8;
    uint32_t reserved_bec;
    uint32_t reserved_bf0;
    uint32_t reserved_bf4;
    uint32_t reserved_bf8;
    uint32_t reserved_bfc;
    uint32_t reserved_c00;
    uint32_t reserved_c04;
    uint32_t reserved_c08;
    uint32_t reserved_c0c;
    uint32_t reserved_c10;
    uint32_t reserved_c14;
    uint32_t reserved_c18;
    uint32_t reserved_c1c;
    uint32_t reserved_c20;
    uint32_t reserved_c24;
    uint32_t reserved_c28;
    uint32_t reserved_c2c;
    uint32_t reserved_c30;
    uint32_t reserved_c34;
    uint32_t reserved_c38;
    uint32_t reserved_c3c;
    uint32_t reserved_c40;
    uint32_t reserved_c44;
    uint32_t reserved_c48;
    uint32_t reserved_c4c;
    uint32_t reserved_c50;
    uint32_t reserved_c54;
    uint32_t reserved_c58;
    uint32_t reserved_c5c;
    uint32_t reserved_c60;
    uint32_t reserved_c64;
    uint32_t reserved_c68;
    uint32_t reserved_c6c;
    uint32_t reserved_c70;
    uint32_t reserved_c74;
    uint32_t reserved_c78;
    uint32_t reserved_c7c;
    uint32_t reserved_c80;
    uint32_t reserved_c84;
    uint32_t reserved_c88;
    uint32_t reserved_c8c;
    uint32_t reserved_c90;
    uint32_t reserved_c94;
    uint32_t reserved_c98;
    uint32_t reserved_c9c;
    uint32_t reserved_ca0;
    uint32_t reserved_ca4;
    uint32_t reserved_ca8;
    uint32_t reserved_cac;
    uint32_t reserved_cb0;
    uint32_t reserved_cb4;
    uint32_t reserved_cb8;
    uint32_t reserved_cbc;
    uint32_t reserved_cc0;
    uint32_t reserved_cc4;
    uint32_t reserved_cc8;
    uint32_t reserved_ccc;
    uint32_t reserved_cd0;
    uint32_t reserved_cd4;
    uint32_t reserved_cd8;
    uint32_t reserved_cdc;
    uint32_t reserved_ce0;
    uint32_t reserved_ce4;
    uint32_t reserved_ce8;
    uint32_t reserved_cec;
    uint32_t reserved_cf0;
    uint32_t reserved_cf4;
    uint32_t reserved_cf8;
    uint32_t reserved_cfc;
    uint32_t reserved_d00;
    uint32_t reserved_d04;
    uint32_t reserved_d08;
    uint32_t reserved_d0c;
    uint32_t reserved_d10;
    uint32_t reserved_d14;
    uint32_t reserved_d18;
    uint32_t reserved_d1c;
    uint32_t reserved_d20;
    uint32_t reserved_d24;
    uint32_t reserved_d28;
    uint32_t reserved_d2c;
    uint32_t reserved_d30;
    uint32_t reserved_d34;
    uint32_t reserved_d38;
    uint32_t reserved_d3c;
    uint32_t reserved_d40;
    uint32_t reserved_d44;
    uint32_t reserved_d48;
    uint32_t reserved_d4c;
    uint32_t reserved_d50;
    uint32_t reserved_d54;
    uint32_t reserved_d58;
    uint32_t reserved_d5c;
    uint32_t reserved_d60;
    uint32_t reserved_d64;
    uint32_t reserved_d68;
    uint32_t reserved_d6c;
    uint32_t reserved_d70;
    uint32_t reserved_d74;
    uint32_t reserved_d78;
    uint32_t reserved_d7c;
    uint32_t reserved_d80;
    uint32_t reserved_d84;
    uint32_t reserved_d88;
    uint32_t reserved_d8c;
    uint32_t reserved_d90;
    uint32_t reserved_d94;
    uint32_t reserved_d98;
    uint32_t reserved_d9c;
    uint32_t reserved_da0;
    uint32_t reserved_da4;
    uint32_t reserved_da8;
    uint32_t reserved_dac;
    uint32_t reserved_db0;
    uint32_t reserved_db4;
    uint32_t reserved_db8;
    uint32_t reserved_dbc;
    uint32_t reserved_dc0;
    uint32_t reserved_dc4;
    uint32_t reserved_dc8;
    uint32_t reserved_dcc;
    uint32_t reserved_dd0;
    uint32_t reserved_dd4;
    uint32_t reserved_dd8;
    uint32_t reserved_ddc;
    uint32_t reserved_de0;
    uint32_t reserved_de4;
    uint32_t reserved_de8;
    uint32_t reserved_dec;
    uint32_t reserved_df0;
    uint32_t reserved_df4;
    uint32_t reserved_df8;
    uint32_t reserved_dfc;
    uint32_t reserved_e00;
    uint32_t reserved_e04;
    uint32_t reserved_e08;
    uint32_t reserved_e0c;
    uint32_t reserved_e10;
    uint32_t reserved_e14;
    uint32_t reserved_e18;
    uint32_t reserved_e1c;
    uint32_t reserved_e20;
    uint32_t reserved_e24;
    uint32_t reserved_e28;
    uint32_t reserved_e2c;
    uint32_t reserved_e30;
    uint32_t reserved_e34;
    uint32_t reserved_e38;
    uint32_t reserved_e3c;
    uint32_t reserved_e40;
    uint32_t reserved_e44;
    uint32_t reserved_e48;
    uint32_t reserved_e4c;
    uint32_t reserved_e50;
    uint32_t reserved_e54;
    uint32_t reserved_e58;
    uint32_t reserved_e5c;
    uint32_t reserved_e60;
    uint32_t reserved_e64;
    uint32_t reserved_e68;
    uint32_t reserved_e6c;
    uint32_t reserved_e70;
    uint32_t reserved_e74;
    uint32_t reserved_e78;
    uint32_t reserved_e7c;
    uint32_t reserved_e80;
    uint32_t reserved_e84;
    uint32_t reserved_e88;
    uint32_t reserved_e8c;
    uint32_t reserved_e90;
    uint32_t reserved_e94;
    uint32_t reserved_e98;
    uint32_t reserved_e9c;
    uint32_t reserved_ea0;
    uint32_t reserved_ea4;
    uint32_t reserved_ea8;
    uint32_t reserved_eac;
    uint32_t reserved_eb0;
    uint32_t reserved_eb4;
    uint32_t reserved_eb8;
    uint32_t reserved_ebc;
    uint32_t reserved_ec0;
    uint32_t reserved_ec4;
    uint32_t reserved_ec8;
    uint32_t reserved_ecc;
    uint32_t reserved_ed0;
    uint32_t reserved_ed4;
    uint32_t reserved_ed8;
    uint32_t reserved_edc;
    uint32_t reserved_ee0;
    uint32_t reserved_ee4;
    uint32_t reserved_ee8;
    uint32_t reserved_eec;
    uint32_t reserved_ef0;
    uint32_t reserved_ef4;
    uint32_t reserved_ef8;
    uint32_t reserved_efc;
    uint32_t reserved_f00;
    uint32_t reserved_f04;
    uint32_t reserved_f08;
    uint32_t reserved_f0c;
    uint32_t reserved_f10;
    uint32_t reserved_f14;
    uint32_t reserved_f18;
    uint32_t reserved_f1c;
    uint32_t reserved_f20;
    uint32_t reserved_f24;
    uint32_t reserved_f28;
    uint32_t reserved_f2c;
    uint32_t reserved_f30;
    uint32_t reserved_f34;
    uint32_t reserved_f38;
    uint32_t reserved_f3c;
    uint32_t reserved_f40;
    uint32_t reserved_f44;
    uint32_t reserved_f48;
    uint32_t reserved_f4c;
    uint32_t reserved_f50;
    uint32_t reserved_f54;
    uint32_t reserved_f58;
    uint32_t reserved_f5c;
    uint32_t reserved_f60;
    uint32_t reserved_f64;
    uint32_t reserved_f68;
    uint32_t reserved_f6c;
    uint32_t reserved_f70;
    uint32_t reserved_f74;
    uint32_t reserved_f78;
    uint32_t reserved_f7c;
    uint32_t reserved_f80;
    uint32_t reserved_f84;
    uint32_t reserved_f88;
    uint32_t reserved_f8c;
    uint32_t reserved_f90;
    uint32_t reserved_f94;
    uint32_t reserved_f98;
    uint32_t reserved_f9c;
    uint32_t reserved_fa0;
    uint32_t reserved_fa4;
    uint32_t reserved_fa8;
    uint32_t reserved_fac;
    uint32_t reserved_fb0;
    uint32_t reserved_fb4;
    uint32_t reserved_fb8;
    uint32_t reserved_fbc;
    uint32_t reserved_fc0;
    uint32_t reserved_fc4;
    uint32_t reserved_fc8;
    uint32_t reserved_fcc;
    uint32_t reserved_fd0;
    uint32_t reserved_fd4;
    uint32_t reserved_fd8;
    uint32_t reserved_fdc;
    uint32_t reserved_fe0;
    uint32_t reserved_fe4;
    uint32_t reserved_fe8;
    uint32_t reserved_fec;
    uint32_t reserved_ff0;
    uint32_t reserved_ff4;
    uint32_t reserved_ff8;
    union {
        struct {
            uint32_t reg_system_reg_date           :    28;  /*reg_system_reg_date*/
            uint32_t reserved28                    :    4;  /*reserved*/
        };
        uint32_t val;
    } date;
} system_dev_t;
extern system_dev_t SYSTEM;
#ifdef __cplusplus
}
#endif



#endif /*_SOC_SYSTEM_STRUCT_H_ */
