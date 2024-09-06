/**
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: peripheral clock gating and reset registers */
/** Type of cpu_peri_clk_en register
 *  cpu_peripheral clock gating register
 */
typedef union {
    struct {
        uint32_t reserved_0:6;
        /** clk_en_assist_debug : R/W; bitpos: [6]; default: 0;
         *  Set 1 to open assist_debug module clock
         */
        uint32_t clk_en_assist_debug:1;
        /** clk_en_dedicated_gpio : R/W; bitpos: [7]; default: 0;
         *  Set 1 to open dedicated_gpio module clk
         */
        uint32_t clk_en_dedicated_gpio:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} system_cpu_peri_clk_en_reg_t;

/** Type of cpu_peri_rst_en register
 *  cpu_peripheral reset register
 */
typedef union {
    struct {
        uint32_t reserved_0:6;
        /** rst_en_assist_debug : R/W; bitpos: [6]; default: 1;
         *  Set 1 to let assist_debug module reset
         */
        uint32_t rst_en_assist_debug:1;
        /** rst_en_dedicated_gpio : R/W; bitpos: [7]; default: 1;
         *  Set 1 to let dedicated_gpio module reset
         */
        uint32_t rst_en_dedicated_gpio:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} system_cpu_peri_rst_en_reg_t;

/** Type of perip_clk_en0 register
 *  peripheral clock gating register
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** spi01_clk_en : R/W; bitpos: [1]; default: 1;
         *  Set 1 to enable SPI01 clock
         */
        uint32_t spi01_clk_en:1;
        /** uart_clk_en : R/W; bitpos: [2]; default: 1;
         *  Set 1 to enable UART clock
         */
        uint32_t uart_clk_en:1;
        uint32_t reserved_3:2;
        /** uart1_clk_en : R/W; bitpos: [5]; default: 1;
         *  Set 1 to enable UART1 clock
         */
        uint32_t uart1_clk_en:1;
        /** spi2_clk_en : R/W; bitpos: [6]; default: 1;
         *  Set 1 to enable SPI2 clock
         */
        uint32_t spi2_clk_en:1;
        /** i2c_ext0_clk_en : R/W; bitpos: [7]; default: 0;
         *  Set 1 to enable I2C_EXT0 clock
         */
        uint32_t i2c_ext0_clk_en:1;
        uint32_t reserved_8:3;
        /** ledc_clk_en : R/W; bitpos: [11]; default: 0;
         *  Set 1 to enable LEDC clock
         */
        uint32_t ledc_clk_en:1;
        uint32_t reserved_12:1;
        /** timergroup_clk_en : R/W; bitpos: [13]; default: 1;
         *  Set 1 to enable TIMERGROUP clock
         */
        uint32_t timergroup_clk_en:1;
        uint32_t reserved_14:10;
        /** uart_mem_clk_en : R/W; bitpos: [24]; default: 1;
         *  Set 1 to enable UART_MEM clock
         */
        uint32_t uart_mem_clk_en:1;
        uint32_t reserved_25:3;
        /** apb_saradc_clk_en : R/W; bitpos: [28]; default: 1;
         *  Set 1 to enable APB_SARADC clock
         */
        uint32_t apb_saradc_clk_en:1;
        /** systimer_clk_en : R/W; bitpos: [29]; default: 1;
         *  Set 1 to enable SYSTEMTIMER clock
         */
        uint32_t systimer_clk_en:1;
        /** adc2_arb_clk_en : R/W; bitpos: [30]; default: 1;
         *  Set 1 to enable ADC2_ARB clock
         */
        uint32_t adc2_arb_clk_en:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} system_perip_clk_en0_reg_t;

/** Type of perip_clk_en1 register
 *  peripheral clock gating register
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** crypto_ecc_clk_en : R/W; bitpos: [1]; default: 0;
         *  Set 1 to enable ECC clock
         */
        uint32_t crypto_ecc_clk_en:1;
        /** crypto_sha_clk_en : R/W; bitpos: [2]; default: 0;
         *  Set 1 to enable SHA clock
         */
        uint32_t crypto_sha_clk_en:1;
        uint32_t reserved_3:3;
        /** dma_clk_en : R/W; bitpos: [6]; default: 0;
         *  Set 1 to enable DMA clock
         */
        uint32_t dma_clk_en:1;
        uint32_t reserved_7:3;
        /** tsens_clk_en : R/W; bitpos: [10]; default: 0;
         *  Set 1 to enable TSENS clock
         */
        uint32_t tsens_clk_en:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} system_perip_clk_en1_reg_t;

/** Type of perip_rst_en0 register
 *  reserved
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** spi01_rst : R/W; bitpos: [1]; default: 0;
         *  Set 1 to let SPI01 reset
         */
        uint32_t spi01_rst:1;
        /** uart_rst : R/W; bitpos: [2]; default: 0;
         *  Set 1 to let UART reset
         */
        uint32_t uart_rst:1;
        uint32_t reserved_3:2;
        /** uart1_rst : R/W; bitpos: [5]; default: 0;
         *  Set 1 to let UART1 reset
         */
        uint32_t uart1_rst:1;
        /** spi2_rst : R/W; bitpos: [6]; default: 0;
         *  Set 1 to let SPI2 reset
         */
        uint32_t spi2_rst:1;
        /** i2c_ext0_rst : R/W; bitpos: [7]; default: 0;
         *  Set 1 to let I2C_EXT0 reset
         */
        uint32_t i2c_ext0_rst:1;
        uint32_t reserved_8:3;
        /** ledc_rst : R/W; bitpos: [11]; default: 0;
         *  Set 1 to let LEDC reset
         */
        uint32_t ledc_rst:1;
        uint32_t reserved_12:1;
        /** timergroup_rst : R/W; bitpos: [13]; default: 0;
         *  Set 1 to let TIMERGROUP reset
         */
        uint32_t timergroup_rst:1;
        uint32_t reserved_14:10;
        /** uart_mem_rst : R/W; bitpos: [24]; default: 0;
         *  Set 1 to let UART_MEM reset
         */
        uint32_t uart_mem_rst:1;
        uint32_t reserved_25:3;
        /** apb_saradc_rst : R/W; bitpos: [28]; default: 0;
         *  Set 1 to let APB_SARADC reset
         */
        uint32_t apb_saradc_rst:1;
        /** systimer_rst : R/W; bitpos: [29]; default: 0;
         *  Set 1 to let SYSTIMER reset
         */
        uint32_t systimer_rst:1;
        /** adc2_arb_rst : R/W; bitpos: [30]; default: 0;
         *  Set 1 to let ADC2_ARB reset
         */
        uint32_t adc2_arb_rst:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} system_perip_rst_en0_reg_t;

/** Type of perip_rst_en1 register
 *  peripheral reset register
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** crypto_ecc_rst : R/W; bitpos: [1]; default: 1;
         *  Set 1 to let CRYPTO_ECC reset
         */
        uint32_t crypto_ecc_rst:1;
        /** crypto_sha_rst : R/W; bitpos: [2]; default: 1;
         *  Set 1 to let CRYPTO_SHA reset
         */
        uint32_t crypto_sha_rst:1;
        uint32_t reserved_3:3;
        /** dma_rst : R/W; bitpos: [6]; default: 1;
         *  Set 1 to let DMA reset
         */
        uint32_t dma_rst:1;
        uint32_t reserved_7:3;
        /** tsens_rst : R/W; bitpos: [10]; default: 0;
         *  Set 1 to let TSENS reset
         */
        uint32_t tsens_rst:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} system_perip_rst_en1_reg_t;

/** Type of edma_ctrl register
 *  edma clcok and reset register
 */
typedef union {
    struct {
        /** edma_clk_on : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable EDMA clock.
         */
        uint32_t edma_clk_on:1;
        /** edma_reset : R/W; bitpos: [1]; default: 0;
         *  Set 1 to let EDMA reset
         */
        uint32_t edma_reset:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} system_edma_ctrl_reg_t;

/** Type of cache_control register
 *  cache control register
 */
typedef union {
    struct {
        /** icache_clk_on : R/W; bitpos: [0]; default: 1;
         *  Set 1 to enable icache clock
         */
        uint32_t icache_clk_on:1;
        /** icache_reset : R/W; bitpos: [1]; default: 0;
         *  Set 1 to let icache reset
         */
        uint32_t icache_reset:1;
        /** dcache_clk_on : R/W; bitpos: [2]; default: 1;
         *  Set 1 to enable dcache clock
         */
        uint32_t dcache_clk_on:1;
        /** dcache_reset : R/W; bitpos: [3]; default: 0;
         *  Set 1 to let dcache reset
         */
        uint32_t dcache_reset:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} system_cache_control_reg_t;


/** Group: clock config register */
/** Type of cpu_per_conf register
 *  cpu clock config register
 */
typedef union {
    struct {
        /** cpuperiod_sel : R/W; bitpos: [1:0]; default: 0;
         *  This field used to sel cpu clock frequent.
         */
        uint32_t cpuperiod_sel:2;
        /** pll_freq_sel : R/W; bitpos: [2]; default: 1;
         *  This field used to sel pll frequent.
         */
        uint32_t pll_freq_sel:1;
        /** cpu_wait_mode_force_on : R/W; bitpos: [3]; default: 1;
         *  Set 1 to force cpu_waiti_clk enable.
         */
        uint32_t cpu_wait_mode_force_on:1;
        /** cpu_waiti_delay_num : R/W; bitpos: [7:4]; default: 0;
         *  This field used to set delay cycle when cpu enter waiti mode, after delay waiti_clk
         *  will close
         */
        uint32_t cpu_waiti_delay_num:4;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} system_cpu_per_conf_reg_t;

/** Type of bt_lpck_div_int register
 *  clock config register
 */
typedef union {
    struct {
        /** bt_lpck_div_num : R/W; bitpos: [11:0]; default: 255;
         *  This field is lower power clock frequent division factor
         */
        uint32_t bt_lpck_div_num:12;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} system_bt_lpck_div_int_reg_t;

/** Type of bt_lpck_div_frac register
 *  low power clock configuration register
 */
typedef union {
    struct {
        /** bt_lpck_div_b : R/W; bitpos: [11:0]; default: 1;
         *  This field is lower power clock frequent division factor b
         */
        uint32_t bt_lpck_div_b:12;
        /** bt_lpck_div_a : R/W; bitpos: [23:12]; default: 1;
         *  This field is lower power clock frequent division factor a
         */
        uint32_t bt_lpck_div_a:12;
        /** lpclk_sel_rtc_slow : R/W; bitpos: [24]; default: 0;
         *  Set 1 to select rtc-slow clock as rtc low power clock
         */
        uint32_t lpclk_sel_rtc_slow:1;
        /** lpclk_sel_8m : R/W; bitpos: [25]; default: 1;
         *  Set 1 to select 8m clock as rtc low power clock
         */
        uint32_t lpclk_sel_8m:1;
        /** lpclk_sel_xtal : R/W; bitpos: [26]; default: 0;
         *  Set 1 to select xtal clock as rtc low power clock
         */
        uint32_t lpclk_sel_xtal:1;
        /** lpclk_sel_xtal32k : R/W; bitpos: [27]; default: 0;
         *  Set 1 to select xtal32k clock as low power clock
         */
        uint32_t lpclk_sel_xtal32k:1;
        /** lpclk_rtc_en : R/W; bitpos: [28]; default: 0;
         *  Set 1 to enable RTC low power clock
         */
        uint32_t lpclk_rtc_en:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} system_bt_lpck_div_frac_reg_t;

/** Type of sysclk_conf register
 *  system clock config register
 */
typedef union {
    struct {
        /** pre_div_cnt : R/W; bitpos: [9:0]; default: 1;
         *  This field is used to set the count of prescaler of XTAL_CLK.
         */
        uint32_t pre_div_cnt:10;
        /** soc_clk_sel : R/W; bitpos: [11:10]; default: 0;
         *  This field is used to select soc clock.
         */
        uint32_t soc_clk_sel:2;
        /** clk_xtal_freq : RO; bitpos: [18:12]; default: 0;
         *  This field is used to read xtal frequency in MHz.
         */
        uint32_t clk_xtal_freq:7;
        /** clk_div_en : RO; bitpos: [19]; default: 0;
         *  reg_clk_div_en
         */
        uint32_t clk_div_en:1;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} system_sysclk_conf_reg_t;


/** Group: Low-power management register */
/** Type of mem_pd_mask register
 *  memory power down mask register
 */
typedef union {
    struct {
        /** lslp_mem_pd_mask : R/W; bitpos: [0]; default: 1;
         *  Set 1 to mask memory power down.
         */
        uint32_t lslp_mem_pd_mask:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} system_mem_pd_mask_reg_t;

/** Type of rtc_fastmem_config register
 *  fast memory config register
 */
typedef union {
    struct {
        uint32_t reserved_0:8;
        /** rtc_mem_crc_start : R/W; bitpos: [8]; default: 0;
         *  Set 1 to start the CRC of RTC memory
         */
        uint32_t rtc_mem_crc_start:1;
        /** rtc_mem_crc_addr : R/W; bitpos: [19:9]; default: 0;
         *  This field is used to set address of RTC memory for CRC.
         */
        uint32_t rtc_mem_crc_addr:11;
        /** rtc_mem_crc_len : R/W; bitpos: [30:20]; default: 2047;
         *  This field is used to set length of RTC memory for CRC based on start address.
         */
        uint32_t rtc_mem_crc_len:11;
        /** rtc_mem_crc_finish : RO; bitpos: [31]; default: 0;
         *  This bit stores the status of RTC memory CRC.1 means finished.
         */
        uint32_t rtc_mem_crc_finish:1;
    };
    uint32_t val;
} system_rtc_fastmem_config_reg_t;

/** Type of rtc_fastmem_crc register
 *  reserved
 */
typedef union {
    struct {
        /** rtc_mem_crc_res : RO; bitpos: [31:0]; default: 0;
         *  This field stores the CRC result of RTC memory.
         */
        uint32_t rtc_mem_crc_res:32;
    };
    uint32_t val;
} system_rtc_fastmem_crc_reg_t;


/** Group: interrupt register */
/** Type of cpu_intr_from_cpu_0 register
 *  interrupt generate register
 */
typedef union {
    struct {
        /** cpu_intr_from_cpu_0 : R/W; bitpos: [0]; default: 0;
         *  Set 1 to generate cpu interrupt 0
         */
        uint32_t cpu_intr_from_cpu_0:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} system_cpu_intr_from_cpu_0_reg_t;

/** Type of cpu_intr_from_cpu_1 register
 *  interrupt generate register
 */
typedef union {
    struct {
        /** cpu_intr_from_cpu_1 : R/W; bitpos: [0]; default: 0;
         *  Set 1 to generate cpu interrupt 1
         */
        uint32_t cpu_intr_from_cpu_1:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} system_cpu_intr_from_cpu_1_reg_t;

/** Type of cpu_intr_from_cpu_2 register
 *  interrupt generate register
 */
typedef union {
    struct {
        /** cpu_intr_from_cpu_2 : R/W; bitpos: [0]; default: 0;
         *  Set 1 to generate cpu interrupt 2
         */
        uint32_t cpu_intr_from_cpu_2:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} system_cpu_intr_from_cpu_2_reg_t;

/** Type of cpu_intr_from_cpu_3 register
 *  interrupt generate register
 */
typedef union {
    struct {
        /** cpu_intr_from_cpu_3 : R/W; bitpos: [0]; default: 0;
         *  Set 1 to generate cpu interrupt 3
         */
        uint32_t cpu_intr_from_cpu_3:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} system_cpu_intr_from_cpu_3_reg_t;


/** Group: system and memory register */
/** Type of rsa_pd_ctrl register
 *  rsa memory power control register
 */
typedef union {
    struct {
        /** rsa_mem_pd : R/W; bitpos: [0]; default: 1;
         *  Set 1 to power down RSA memory. This bit has the lowest priority.When Digital
         *  Signature occupies the RSA. This bit is invalid.
         */
        uint32_t rsa_mem_pd:1;
        /** rsa_mem_force_pu : R/W; bitpos: [1]; default: 0;
         *  Set 1 to force power up RSA memory. This bit has the second highest priority.
         */
        uint32_t rsa_mem_force_pu:1;
        /** rsa_mem_force_pd : R/W; bitpos: [2]; default: 0;
         *  Set 1 to force power down RSA memory. This bit has the highest priority.
         */
        uint32_t rsa_mem_force_pd:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} system_rsa_pd_ctrl_reg_t;

/** Type of external_device_encrypt_decrypt_control register
 *  SYSTEM_EXTERNAL_DEVICE_ENCRYPT_DECRYPT_CONTROL_REG
 */
typedef union {
    struct {
        /** enable_spi_manual_encrypt : R/W; bitpos: [0]; default: 0;
         *  Set 1 to enable the SPI manual encrypt.
         */
        uint32_t enable_spi_manual_encrypt:1;
        /** enable_download_db_encrypt : R/W; bitpos: [1]; default: 0;
         *  Set 1 to enable download DB encrypt.
         */
        uint32_t enable_download_db_encrypt:1;
        /** enable_download_g0cb_decrypt : R/W; bitpos: [2]; default: 0;
         *  Set 1 to enable download G0CB decrypt
         */
        uint32_t enable_download_g0cb_decrypt:1;
        /** enable_download_manual_encrypt : R/W; bitpos: [3]; default: 0;
         *  Set 1 to enable download manual encrypt
         */
        uint32_t enable_download_manual_encrypt:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} system_external_device_encrypt_decrypt_control_reg_t;


/** Group: eco register */
/** Type of redundant_eco_ctrl register
 *  eco register
 */
typedef union {
    struct {
        /** redundant_eco_drive : R/W; bitpos: [0]; default: 0;
         *  reg_redundant_eco_drive
         */
        uint32_t redundant_eco_drive:1;
        /** redundant_eco_result : RO; bitpos: [1]; default: 0;
         *  reg_redundant_eco_result
         */
        uint32_t redundant_eco_result:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} system_redundant_eco_ctrl_reg_t;


/** Group: clock gating register */
/** Type of clock_gate register
 *  clock gating register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 1;
         *  reg_clk_en
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} system_clock_gate_reg_t;


/** Group: pvt register */
/** Type of mem_pvt register
 *  mem pvt register
 */
typedef union {
    struct {
        /** mem_path_len : R/W; bitpos: [3:0]; default: 3;
         *  reg_mem_path_len
         */
        uint32_t mem_path_len:4;
        /** mem_err_cnt_clr : WT; bitpos: [4]; default: 0;
         *  reg_mem_err_cnt_clr
         */
        uint32_t mem_err_cnt_clr:1;
        /** mem_pvt_monitor_en : R/W; bitpos: [5]; default: 0;
         *  reg_mem_pvt_monitor_en
         */
        uint32_t mem_pvt_monitor_en:1;
        /** mem_timing_err_cnt : RO; bitpos: [21:6]; default: 0;
         *  reg_mem_timing_err_cnt
         */
        uint32_t mem_timing_err_cnt:16;
        /** mem_vt_sel : R/W; bitpos: [23:22]; default: 0;
         *  reg_mem_vt_sel
         */
        uint32_t mem_vt_sel:2;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} system_mem_pvt_reg_t;

/** Type of comb_pvt_lvt_conf register
 *  mem pvt register
 */
typedef union {
    struct {
        /** comb_path_len_lvt : R/W; bitpos: [5:0]; default: 3;
         *  reg_comb_path_len_lvt
         */
        uint32_t comb_path_len_lvt:6;
        /** comb_err_cnt_clr_lvt : WT; bitpos: [6]; default: 0;
         *  reg_comb_err_cnt_clr_lvt
         */
        uint32_t comb_err_cnt_clr_lvt:1;
        /** comb_pvt_monitor_en_lvt : R/W; bitpos: [7]; default: 0;
         *  reg_comb_pvt_monitor_en_lvt
         */
        uint32_t comb_pvt_monitor_en_lvt:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} system_comb_pvt_lvt_conf_reg_t;

/** Type of comb_pvt_nvt_conf register
 *  mem pvt register
 */
typedef union {
    struct {
        /** comb_path_len_nvt : R/W; bitpos: [5:0]; default: 3;
         *  reg_comb_path_len_nvt
         */
        uint32_t comb_path_len_nvt:6;
        /** comb_err_cnt_clr_nvt : WT; bitpos: [6]; default: 0;
         *  reg_comb_err_cnt_clr_nvt
         */
        uint32_t comb_err_cnt_clr_nvt:1;
        /** comb_pvt_monitor_en_nvt : R/W; bitpos: [7]; default: 0;
         *  reg_comb_pvt_monitor_en_nvt
         */
        uint32_t comb_pvt_monitor_en_nvt:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} system_comb_pvt_nvt_conf_reg_t;

/** Type of comb_pvt_hvt_conf register
 *  mem pvt register
 */
typedef union {
    struct {
        /** comb_path_len_hvt : R/W; bitpos: [5:0]; default: 3;
         *  reg_comb_path_len_hvt
         */
        uint32_t comb_path_len_hvt:6;
        /** comb_err_cnt_clr_hvt : WT; bitpos: [6]; default: 0;
         *  reg_comb_err_cnt_clr_hvt
         */
        uint32_t comb_err_cnt_clr_hvt:1;
        /** comb_pvt_monitor_en_hvt : R/W; bitpos: [7]; default: 0;
         *  reg_comb_pvt_monitor_en_hvt
         */
        uint32_t comb_pvt_monitor_en_hvt:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} system_comb_pvt_hvt_conf_reg_t;

/** Type of comb_pvt_err_lvt_site0 register
 *  mem pvt register
 */
typedef union {
    struct {
        /** comb_timing_err_cnt_lvt_site0 : RO; bitpos: [15:0]; default: 0;
         *  reg_comb_timing_err_cnt_lvt_site0
         */
        uint32_t comb_timing_err_cnt_lvt_site0:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} system_comb_pvt_err_lvt_site0_reg_t;

/** Type of comb_pvt_err_nvt_site0 register
 *  mem pvt register
 */
typedef union {
    struct {
        /** comb_timing_err_cnt_nvt_site0 : RO; bitpos: [15:0]; default: 0;
         *  reg_comb_timing_err_cnt_nvt_site0
         */
        uint32_t comb_timing_err_cnt_nvt_site0:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} system_comb_pvt_err_nvt_site0_reg_t;

/** Type of comb_pvt_err_hvt_site0 register
 *  mem pvt register
 */
typedef union {
    struct {
        /** comb_timing_err_cnt_hvt_site0 : RO; bitpos: [15:0]; default: 0;
         *  reg_comb_timing_err_cnt_hvt_site0
         */
        uint32_t comb_timing_err_cnt_hvt_site0:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} system_comb_pvt_err_hvt_site0_reg_t;

/** Type of comb_pvt_err_lvt_site1 register
 *  mem pvt register
 */
typedef union {
    struct {
        /** comb_timing_err_cnt_lvt_site1 : RO; bitpos: [15:0]; default: 0;
         *  reg_comb_timing_err_cnt_lvt_site1
         */
        uint32_t comb_timing_err_cnt_lvt_site1:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} system_comb_pvt_err_lvt_site1_reg_t;

/** Type of comb_pvt_err_nvt_site1 register
 *  mem pvt register
 */
typedef union {
    struct {
        /** comb_timing_err_cnt_nvt_site1 : RO; bitpos: [15:0]; default: 0;
         *  reg_comb_timing_err_cnt_nvt_site1
         */
        uint32_t comb_timing_err_cnt_nvt_site1:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} system_comb_pvt_err_nvt_site1_reg_t;

/** Type of comb_pvt_err_hvt_site1 register
 *  mem pvt register
 */
typedef union {
    struct {
        /** comb_timing_err_cnt_hvt_site1 : RO; bitpos: [15:0]; default: 0;
         *  reg_comb_timing_err_cnt_hvt_site1
         */
        uint32_t comb_timing_err_cnt_hvt_site1:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} system_comb_pvt_err_hvt_site1_reg_t;

/** Type of comb_pvt_err_lvt_site2 register
 *  mem pvt register
 */
typedef union {
    struct {
        /** comb_timing_err_cnt_lvt_site2 : RO; bitpos: [15:0]; default: 0;
         *  reg_comb_timing_err_cnt_lvt_site2
         */
        uint32_t comb_timing_err_cnt_lvt_site2:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} system_comb_pvt_err_lvt_site2_reg_t;

/** Type of comb_pvt_err_nvt_site2 register
 *  mem pvt register
 */
typedef union {
    struct {
        /** comb_timing_err_cnt_nvt_site2 : RO; bitpos: [15:0]; default: 0;
         *  reg_comb_timing_err_cnt_nvt_site2
         */
        uint32_t comb_timing_err_cnt_nvt_site2:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} system_comb_pvt_err_nvt_site2_reg_t;

/** Type of comb_pvt_err_hvt_site2 register
 *  mem pvt register
 */
typedef union {
    struct {
        /** comb_timing_err_cnt_hvt_site2 : RO; bitpos: [15:0]; default: 0;
         *  reg_comb_timing_err_cnt_hvt_site2
         */
        uint32_t comb_timing_err_cnt_hvt_site2:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} system_comb_pvt_err_hvt_site2_reg_t;

/** Type of comb_pvt_err_lvt_site3 register
 *  mem pvt register
 */
typedef union {
    struct {
        /** comb_timing_err_cnt_lvt_site3 : RO; bitpos: [15:0]; default: 0;
         *  reg_comb_timing_err_cnt_lvt_site3
         */
        uint32_t comb_timing_err_cnt_lvt_site3:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} system_comb_pvt_err_lvt_site3_reg_t;

/** Type of comb_pvt_err_nvt_site3 register
 *  mem pvt register
 */
typedef union {
    struct {
        /** comb_timing_err_cnt_nvt_site3 : RO; bitpos: [15:0]; default: 0;
         *  reg_comb_timing_err_cnt_nvt_site3
         */
        uint32_t comb_timing_err_cnt_nvt_site3:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} system_comb_pvt_err_nvt_site3_reg_t;

/** Type of comb_pvt_err_hvt_site3 register
 *  mem pvt register
 */
typedef union {
    struct {
        /** comb_timing_err_cnt_hvt_site3 : RO; bitpos: [15:0]; default: 0;
         *  reg_comb_timing_err_cnt_hvt_site3
         */
        uint32_t comb_timing_err_cnt_hvt_site3:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} system_comb_pvt_err_hvt_site3_reg_t;


/** Group: date register */
/** Type of reg_date register
 *  Version register
 */
typedef union {
    struct {
        /** system_reg_date : R/W; bitpos: [27:0]; default: 34636176;
         *  reg_system_reg_date
         */
        uint32_t system_reg_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} system_reg_date_reg_t;


typedef struct {
    volatile system_cpu_peri_clk_en_reg_t cpu_peri_clk_en;
    volatile system_cpu_peri_rst_en_reg_t cpu_peri_rst_en;
    volatile system_cpu_per_conf_reg_t cpu_per_conf;
    volatile system_mem_pd_mask_reg_t mem_pd_mask;
    volatile system_perip_clk_en0_reg_t perip_clk_en0;
    volatile system_perip_clk_en1_reg_t perip_clk_en1;
    volatile system_perip_rst_en0_reg_t perip_rst_en0;
    volatile system_perip_rst_en1_reg_t perip_rst_en1;
    volatile system_bt_lpck_div_int_reg_t bt_lpck_div_int;
    volatile system_bt_lpck_div_frac_reg_t bt_lpck_div_frac;
    volatile system_cpu_intr_from_cpu_0_reg_t cpu_intr_from_cpu_0;
    volatile system_cpu_intr_from_cpu_1_reg_t cpu_intr_from_cpu_1;
    volatile system_cpu_intr_from_cpu_2_reg_t cpu_intr_from_cpu_2;
    volatile system_cpu_intr_from_cpu_3_reg_t cpu_intr_from_cpu_3;
    volatile system_rsa_pd_ctrl_reg_t rsa_pd_ctrl;
    volatile system_edma_ctrl_reg_t edma_ctrl;
    volatile system_cache_control_reg_t cache_control;
    volatile system_external_device_encrypt_decrypt_control_reg_t external_device_encrypt_decrypt_control;
    volatile system_rtc_fastmem_config_reg_t rtc_fastmem_config;
    volatile system_rtc_fastmem_crc_reg_t rtc_fastmem_crc;
    volatile system_redundant_eco_ctrl_reg_t redundant_eco_ctrl;
    volatile system_clock_gate_reg_t clock_gate;
    volatile system_sysclk_conf_reg_t sysclk_conf;
    volatile system_mem_pvt_reg_t mem_pvt;
    volatile system_comb_pvt_lvt_conf_reg_t comb_pvt_lvt_conf;
    volatile system_comb_pvt_nvt_conf_reg_t comb_pvt_nvt_conf;
    volatile system_comb_pvt_hvt_conf_reg_t comb_pvt_hvt_conf;
    volatile system_comb_pvt_err_lvt_site0_reg_t comb_pvt_err_lvt_site0;
    volatile system_comb_pvt_err_nvt_site0_reg_t comb_pvt_err_nvt_site0;
    volatile system_comb_pvt_err_hvt_site0_reg_t comb_pvt_err_hvt_site0;
    volatile system_comb_pvt_err_lvt_site1_reg_t comb_pvt_err_lvt_site1;
    volatile system_comb_pvt_err_nvt_site1_reg_t comb_pvt_err_nvt_site1;
    volatile system_comb_pvt_err_hvt_site1_reg_t comb_pvt_err_hvt_site1;
    volatile system_comb_pvt_err_lvt_site2_reg_t comb_pvt_err_lvt_site2;
    volatile system_comb_pvt_err_nvt_site2_reg_t comb_pvt_err_nvt_site2;
    volatile system_comb_pvt_err_hvt_site2_reg_t comb_pvt_err_hvt_site2;
    volatile system_comb_pvt_err_lvt_site3_reg_t comb_pvt_err_lvt_site3;
    volatile system_comb_pvt_err_nvt_site3_reg_t comb_pvt_err_nvt_site3;
    volatile system_comb_pvt_err_hvt_site3_reg_t comb_pvt_err_hvt_site3;
    uint32_t reserved_09c[984];
    volatile system_reg_date_reg_t reg_date;
} system_dev_t;

extern system_dev_t SYSTEM;

#ifndef __cplusplus
_Static_assert(sizeof(system_dev_t) == 0x1000, "Invalid size of system_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
