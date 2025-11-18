/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: HP SYS VER DATE REG */
/** Type of sys_ver_date register
 *  NA
 */
typedef union {
    struct {
        /** reg_ver_date : R/W; bitpos: [31:0]; default: 539296770;
         *  NA
         */
        uint32_t reg_ver_date:32;
    };
    uint32_t val;
} hp_system_ver_date_reg_t;


/** Group: HP CLK EN REG */
/** Type of clk_en register
 *  NA
 */
typedef union {
    struct {
        /** reg_clk_en : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t reg_clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_clk_en_reg_t;


/** Group: HP CPU INT FROM CPU 0 REG */
/** Type of cpu_int_from_cpu_0 register
 *  NA
 */
typedef union {
    struct {
        /** cpu_int_from_cpu_0 : R/W; bitpos: [0]; default: 0;
         *  set 1 will trigger a interrupt
         */
        uint32_t cpu_int_from_cpu_0:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_cpu_int_from_cpu_0_reg_t;


/** Group: HP CPU INT FROM CPU 1 REG */
/** Type of cpu_int_from_cpu_1 register
 *  NA
 */
typedef union {
    struct {
        /** cpu_int_from_cpu_1 : R/W; bitpos: [0]; default: 0;
         *  set 1 will trigger a interrupt
         */
        uint32_t cpu_int_from_cpu_1:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_cpu_int_from_cpu_1_reg_t;


/** Group: HP CPU INT FROM CPU 2 REG */
/** Type of cpu_int_from_cpu_2 register
 *  NA
 */
typedef union {
    struct {
        /** cpu_int_from_cpu_2 : R/W; bitpos: [0]; default: 0;
         *  set 1 will trigger a interrupt
         */
        uint32_t cpu_int_from_cpu_2:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_cpu_int_from_cpu_2_reg_t;


/** Group: HP CPU INT FROM CPU 3 REG */
/** Type of cpu_int_from_cpu_3 register
 *  NA
 */
typedef union {
    struct {
        /** cpu_int_from_cpu_3 : R/W; bitpos: [0]; default: 0;
         *  set 1 will trigger a interrupt
         */
        uint32_t cpu_int_from_cpu_3:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_cpu_int_from_cpu_3_reg_t;


/** Group: HP MODEM DIAG CONFIG REG */
/** Type of modem_diag_en register
 *  NA
 */
typedef union {
    struct {
        /** reg_modem_diag_en : R/W; bitpos: [31:0]; default: 0;
         *  set reg_modem_diag_en[15:0] will let hp_system_probe_out[15:0] used as modem_diag[15:0],
         *  set reg_modem_diag_en[31:16] will let lcd_data_out[15:0] used as modem_diag[31:16]
         */
        uint32_t reg_modem_diag_en:32;
    };
    uint32_t val;
} hp_system_modem_diag_en_reg_t;


/** Group: HP TCM RAM PWR CTRL0 REG */
/** Type of tcm_ram_pwr_ctrl0 register
 *  NA
 */
typedef union {
    struct {
        /** reg_hp_system_tcm_clk_force_on : R/W; bitpos: [0]; default: 0;
         *  hp_system_tcm clk gatig force on
         */
        uint32_t reg_hp_system_tcm_clk_force_on:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_tcm_ram_pwr_ctrl0_reg_t;


/** Group: HP ROM PWR CTRL0 REG */
/** Type of rom_pwr_ctrl0 register
 *  NA
 */
typedef union {
    struct {
        /** reg_hp_system_rom_clk_force_on : R/W; bitpos: [0]; default: 0;
         *  hp_system_rom clk gating force on
         */
        uint32_t reg_hp_system_rom_clk_force_on:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_rom_pwr_ctrl0_reg_t;


/** Group: HP PROBEA CTRL REG */
/** Type of probea_ctrl register
 *  NA
 */
typedef union {
    struct {
        /** reg_probe_a_mod_sel : R/W; bitpos: [15:0]; default: 0;
         *  This field is used to selec probe_group from probe_group0 to probe_group15 for
         *  module's probe_out[31:0] in a mode
         */
        uint32_t reg_probe_a_mod_sel:16;
        /** reg_probe_a_top_sel : R/W; bitpos: [23:16]; default: 0;
         *  This field is used to selec module's probe_out[31:0] as probe out in a mode
         */
        uint32_t reg_probe_a_top_sel:8;
        /** reg_probe_l_sel : R/W; bitpos: [25:24]; default: 0;
         *  This field is used to selec probe_out[31:16]
         */
        uint32_t reg_probe_l_sel:2;
        /** reg_probe_h_sel : R/W; bitpos: [27:26]; default: 0;
         *  This field is used to selec probe_out[31:16]
         */
        uint32_t reg_probe_h_sel:2;
        /** reg_probe_global_en : R/W; bitpos: [28]; default: 0;
         *  Set this bit to enable global debug probe in hp system.
         */
        uint32_t reg_probe_global_en:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} hp_system_probea_ctrl_reg_t;


/** Group: HP PROBEB CTRL REG */
/** Type of probeb_ctrl register
 *  NA
 */
typedef union {
    struct {
        /** reg_probe_b_mod_sel : R/W; bitpos: [15:0]; default: 0;
         *  This field is used to selec probe_group from probe_group0 to probe_group15 for
         *  module's probe_out[31:0] in b mode.
         */
        uint32_t reg_probe_b_mod_sel:16;
        /** reg_probe_b_top_sel : R/W; bitpos: [23:16]; default: 0;
         *  This field is used to select module's probe_out[31:0]  as probe_out in b mode
         */
        uint32_t reg_probe_b_top_sel:8;
        /** reg_probe_b_en : R/W; bitpos: [24]; default: 0;
         *  Set this bit to enable b mode for debug probe. 1:  b mode, 0: a mode.
         */
        uint32_t reg_probe_b_en:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} hp_system_probeb_ctrl_reg_t;


/** Group: HP PROBE OUT REG */
/** Type of probe_out register
 *  NA
 */
typedef union {
    struct {
        /** reg_probe_top_out : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t reg_probe_top_out:32;
    };
    uint32_t val;
} hp_system_probe_out_reg_t;


/** Group: HP CPU CORESTALLED ST REG */
/** Type of cpu_corestalled_st register
 *  NA
 */
typedef union {
    struct {
        /** reg_core0_corestalled_st : RO; bitpos: [0]; default: 0;
         *  hp core0 corestalled status
         */
        uint32_t reg_core0_corestalled_st:1;
        /** reg_core1_corestalled_st : RO; bitpos: [1]; default: 0;
         *  hp core1 corestalled status
         */
        uint32_t reg_core1_corestalled_st:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_cpu_corestalled_st_reg_t;


/** Group: HP CRYPTO CTRL REG */
/** Type of crypto_ctrl register
 *  NA
 */
typedef union {
    struct {
        /** reg_enable_spi_manual_encrypt : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t reg_enable_spi_manual_encrypt:1;
        /** reg_enable_download_db_encrypt : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t reg_enable_download_db_encrypt:1;
        /** reg_enable_download_g0cb_decrypt : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t reg_enable_download_g0cb_decrypt:1;
        /** reg_enable_download_manual_encrypt : R/W; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t reg_enable_download_manual_encrypt:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_system_crypto_ctrl_reg_t;


/** Group: HP IOMUX FPGA DEBUG REG */
/** Type of iomux_fpga_debug register
 *  NA
 */
typedef union {
    struct {
        /** reg_fpga_debug : R/W; bitpos: [0]; default: 1;
         *  iomux fpga debug
         */
        uint32_t reg_fpga_debug:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_iomux_fpga_debug_reg_t;


/** Group: HP SYS RDN ECO CS REG */
/** Type of sys_rdn_eco_cs register
 *  NA
 */
typedef union {
    struct {
        /** reg_hp_system_rdn_eco_en : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t reg_hp_system_rdn_eco_en:1;
        /** reg_hp_system_rdn_eco_result : RO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t reg_hp_system_rdn_eco_result:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_rdn_eco_cs_reg_t;


/** Group: HP TCM RDN ECO CS REG */
/** Type of tcm_rdn_eco_cs register
 *  NA
 */
typedef union {
    struct {
        /** reg_hp_system_tcm_rdn_eco_en : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t reg_hp_system_tcm_rdn_eco_en:1;
        /** reg_hp_system_tcm_rdn_eco_result : RO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t reg_hp_system_tcm_rdn_eco_result:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_tcm_rdn_eco_cs_reg_t;


/** Group: HP TCM RDN ECO LOW REG */
/** Type of tcm_rdn_eco_low register
 *  NA
 */
typedef union {
    struct {
        /** reg_hp_system_tcm_rdn_eco_low : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t reg_hp_system_tcm_rdn_eco_low:32;
    };
    uint32_t val;
} hp_system_tcm_rdn_eco_low_reg_t;


/** Group: HP TCM RDN ECO HIGH REG */
/** Type of tcm_rdn_eco_high register
 *  NA
 */
typedef union {
    struct {
        /** reg_hp_system_tcm_rdn_eco_high : R/W; bitpos: [31:0]; default: 4294967295;
         *  NA
         */
        uint32_t reg_hp_system_tcm_rdn_eco_high:32;
    };
    uint32_t val;
} hp_system_tcm_rdn_eco_high_reg_t;


/** Group: HP CACHE PWR CTRL REG */
/** Type of cache_pwr_ctrl register
 *  NA
 */
typedef union {
    struct {
        /** reg_ac_mem_fo : R/W; bitpos: [7:0]; default: 0;
         *  need_des
         */
        uint32_t reg_ac_mem_fo:8;
        /** reg_ic_mem_fo : R/W; bitpos: [15:8]; default: 0;
         *  need_des
         */
        uint32_t reg_ic_mem_fo:8;
        /** reg_dc_mem_fo : R/W; bitpos: [23:16]; default: 0;
         *  need_des
         */
        uint32_t reg_dc_mem_fo:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} hp_system_cache_pwr_ctrl_reg_t;


/** Group: HP_TCM_DATA_DUMP_CTRL_REG */
/** Type of tcm_data_dump_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** tcm_data_dump_en : R/W; bitpos: [31:0]; default: 0;
         *  hp tcm_data dump switch
         */
        uint32_t tcm_data_dump_en:32;
    };
    uint32_t val;
} hp_system_tcm_data_dump_ctrl_reg_t;


/** Group: Configuration Register */
/** Type of cpu_waiti_conf register
 *  CPU_WAITI configuration register
 */
typedef union {
    struct {
        /** cpu_wait_mode_force_on : R/W; bitpos: [0]; default: 1;
         *  Set 1 to force cpu_waiti_clk enable.
         */
        uint32_t cpu_wait_mode_force_on:1;
        /** cpu_waiti_delay_num : R/W; bitpos: [4:1]; default: 0;
         *  This field used to set delay cycle when cpu enter waiti mode, after delay waiti_clk
         *  will close
         */
        uint32_t cpu_waiti_delay_num:4;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} hp_system_cpu_waiti_conf_reg_t;

/** Type of sys_core_debug_runstall_conf register
 *  Core Debug runstall configure register
 */
typedef union {
    struct {
        /** sys_core_debug_runstall_enable : R/W; bitpos: [0]; default: 0;
         *  Set this field to 1 to enable debug runstall feature between HP-core and LP-core.
         */
        uint32_t sys_core_debug_runstall_enable:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_core_debug_runstall_conf_reg_t;

/** Type of core_pad_wakeup_event register
 *  pad wakeup event register
 */
typedef union {
    struct {
        /** core0_pad_wakeup_event : R/W; bitpos: [0]; default: 0;
         *  wakeup core0 register
         */
        uint32_t core0_pad_wakeup_event:1;
        /** core1_pad_wakeup_event : R/W; bitpos: [1]; default: 0;
         *  wakeup core1 register
         */
        uint32_t core1_pad_wakeup_event:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_core_pad_wakeup_event_reg_t;

/** Type of twai0_timestamp_l register
 *  twai0 timestamp low bit reg
 */
typedef union {
    struct {
        /** twai0_timestamp_i : R/W; bitpos: [31:0]; default: 4294967295;
         *  This field used to set lower 32bits of timestamp hp twai0
         */
        uint32_t twai0_timestamp_i:32;
    };
    uint32_t val;
} hp_system_twai0_timestamp_l_reg_t;

/** Type of twai0_timestamp_h register
 *  twai0 timestamp high bit reg
 */
typedef union {
    struct {
        /** twai0_timestamp_h : R/W; bitpos: [31:0]; default: 4294967295;
         *  This field used to set upper 32bits of timestamp hp twai0
         */
        uint32_t twai0_timestamp_h:32;
    };
    uint32_t val;
} hp_system_twai0_timestamp_h_reg_t;

/** Type of twai1_timestamp_l register
 *  twai1 timestamp low bit reg
 */
typedef union {
    struct {
        /** twai1_timestamp_i : R/W; bitpos: [31:0]; default: 4294967295;
         *  This field used to set lower 32bits of timestamp hp twai1
         */
        uint32_t twai1_timestamp_i:32;
    };
    uint32_t val;
} hp_system_twai1_timestamp_l_reg_t;

/** Type of twai1_timestamp_h register
 *  twai1 timestamp high bit reg
 */
typedef union {
    struct {
        /** twai1_timestamp_h : R/W; bitpos: [31:0]; default: 4294967295;
         *  This field used to set upper 32bits of timestamp hp twai1
         */
        uint32_t twai1_timestamp_h:32;
    };
    uint32_t val;
} hp_system_twai1_timestamp_h_reg_t;

/** Type of rng_cfg register
 *  rng cfg register
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** rng_chain_clk_div_num : R/W; bitpos: [23:16]; default: 0;
         *  chain clk div num to pad for debug
         */
        uint32_t rng_chain_clk_div_num:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} hp_system_rng_cfg_reg_t;

/** Type of peri_mem_clk_force_on register
 *  hp peri mem clk force on regpster
 */
typedef union {
    struct {
        /** rmt_mem_clk_force_on : R/W; bitpos: [0]; default: 0;
         *  Set this bit to force on mem clk in rmt
         */
        uint32_t rmt_mem_clk_force_on:1;
        /** bitscrambler_tx_mem_clk_force_on : R/W; bitpos: [1]; default: 0;
         *  Set this bit to force on tx mem clk in bitscrambler
         */
        uint32_t bitscrambler_tx_mem_clk_force_on:1;
        /** bitscrambler_rx_mem_clk_force_on : R/W; bitpos: [2]; default: 0;
         *  Set this bit to force on rx mem clk in bitscrambler
         */
        uint32_t bitscrambler_rx_mem_clk_force_on:1;
        /** gdma_mem_clk_force_on : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force on mem clk in gdma
         */
        uint32_t gdma_mem_clk_force_on:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_system_peri_mem_clk_force_on_reg_t;

/** Type of sys_prdyn_st register
 *  hp system prdyn status register
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** rom_prdyn : RO; bitpos: [1]; default: 1;
         *  record memory power status
         */
        uint32_t rom_prdyn:1;
        uint32_t reserved_2:1;
        /** cpu_mem_prdyn : RO; bitpos: [3]; default: 1;
         *  record memory power status
         */
        uint32_t cpu_mem_prdyn:1;
        /** bitscrambler_mem_prdyn : RO; bitpos: [4]; default: 1;
         *  record memory power status
         */
        uint32_t bitscrambler_mem_prdyn:1;
        uint32_t reserved_5:1;
        /** usb_mem_prdyn : RO; bitpos: [6]; default: 1;
         *  record memory power status
         */
        uint32_t usb_mem_prdyn:1;
        /** rmt_mem_prdyn : RO; bitpos: [7]; default: 1;
         *  record memory power status
         */
        uint32_t rmt_mem_prdyn:1;
        /** ledc0_mem_prdyn : RO; bitpos: [8]; default: 1;
         *  record memory power status
         */
        uint32_t ledc0_mem_prdyn:1;
        /** key_manager_mem_prdyn : RO; bitpos: [9]; default: 1;
         *  record memory power status
         */
        uint32_t key_manager_mem_prdyn:1;
        /** vpu_mem_prdyn : RO; bitpos: [10]; default: 1;
         *  record memory power status
         */
        uint32_t vpu_mem_prdyn:1;
        uint32_t reserved_11:1;
        /** gdma_mem_prdyn : RO; bitpos: [12]; default: 1;
         *  record memory power status
         */
        uint32_t gdma_mem_prdyn:1;
        uint32_t reserved_13:1;
        /** uart_mem_prdyn : RO; bitpos: [14]; default: 1;
         *  record memory power status
         */
        uint32_t uart_mem_prdyn:1;
        /** crypto_mem_prdyn : RO; bitpos: [15]; default: 1;
         *  record memory power status
         */
        uint32_t crypto_mem_prdyn:1;
        /** flash_mspi_mem_prdyn : RO; bitpos: [16]; default: 1;
         *  record memory power status
         */
        uint32_t flash_mspi_mem_prdyn:1;
        /** psram_mspi_mem_prdyn : RO; bitpos: [17]; default: 1;
         *  record memory power status
         */
        uint32_t psram_mspi_mem_prdyn:1;
        /** modem_mem_prdyn : RO; bitpos: [18]; default: 1;
         *  record memory power status
         */
        uint32_t modem_mem_prdyn:1;
        /** ledc1_mem_prdyn : RO; bitpos: [19]; default: 1;
         *  record memory power status
         */
        uint32_t ledc1_mem_prdyn:1;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} hp_system_prdyn_st_reg_t;

/** Type of sys_icm_cfg register
 *  hp system axi icm ctrl register
 */
typedef union {
    struct {
        /** sys_icm_dis_rd : R/W; bitpos: [2:0]; default: 0;
         *  0:mean master can access slave , 1:mean master can not rd slave ,will returen
         *  decerro on axi bus
         */
        uint32_t sys_icm_dis_rd:3;
        /** sys_icm_dis_wr : R/W; bitpos: [5:3]; default: 0;
         *  0:mean master can access slave , 1:mean master can not wr slave ,will returen
         *  decerro on axi bus
         */
        uint32_t sys_icm_dis_wr:3;
        /** sys_icm_cfg_secure : R/W; bitpos: [8:6]; default: 0;
         *  axi_matrix slave ,0: non-secure, 1: secure , total 3 slave.
         */
        uint32_t sys_icm_cfg_secure:3;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_system_icm_cfg_reg_t;

/** Type of sys_uart3_mem_lp_ctrl register
 *  uart memory power control register
 */
typedef union {
    struct {
        /** sys_uart3_mem_lp_mode : R/W; bitpos: [1:0]; default: 2;
         *  Configures uart3 memory low power mode in low power stage.
         *  0(default): deep sleep
         *  1: light sleep
         *  2: shut down
         *  3: disable low power stage
         */
        uint32_t sys_uart3_mem_lp_mode:2;
        /** sys_uart3_mem_lp_en : R/W; bitpos: [2]; default: 1;
         *  Set this bit to power down uart3 memory.
         */
        uint32_t sys_uart3_mem_lp_en:1;
        /** sys_uart3_mem_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control uart3 memory, disable hardware control.
         */
        uint32_t sys_uart3_mem_force_ctrl:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_system_uart3_mem_lp_ctrl_reg_t;

/** Type of sys_rmt_mem_lp_ctrl register
 *  rmt memory power control register
 */
typedef union {
    struct {
        /** sys_rmt_mem_lp_mode : R/W; bitpos: [1:0]; default: 2;
         *  Configures rmt memory low power mode in low power stage.
         *  0(default): deep sleep
         *  1: light sleep
         *  2: shut down
         *  3: disable low power stage
         */
        uint32_t sys_rmt_mem_lp_mode:2;
        /** sys_rmt_mem_lp_en : R/W; bitpos: [2]; default: 1;
         *  Set this bit to power down rmt memory.
         */
        uint32_t sys_rmt_mem_lp_en:1;
        /** sys_rmt_mem_lp_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control rmt memory, disable hardware control.
         */
        uint32_t sys_rmt_mem_lp_force_ctrl:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_system_rmt_mem_lp_ctrl_reg_t;

/** Type of sys_ledc0_mem_lp_ctrl register
 *  ledc0 memory power control register
 */
typedef union {
    struct {
        /** sys_ledc0_mem_lp_mode : R/W; bitpos: [1:0]; default: 2;
         *  Configures ledc0 memory low power mode in low power stage.
         *  0(default): deep sleep
         *  1: light sleep
         *  2: shut down
         *  3: disable low power stage
         */
        uint32_t sys_ledc0_mem_lp_mode:2;
        /** sys_ledc0_mem_lp_en : R/W; bitpos: [2]; default: 1;
         *  Set this bit to power down ledc0 memory.
         */
        uint32_t sys_ledc0_mem_lp_en:1;
        /** sys_ledc0_mem_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control ledc0 memory, disable hardware control.
         */
        uint32_t sys_ledc0_mem_force_ctrl:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_system_ledc0_mem_lp_ctrl_reg_t;

/** Type of sys_km_mem_lp_ctrl register
 *  key manager memory power control register
 */
typedef union {
    struct {
        /** sys_km_mem_lp_mode : R/W; bitpos: [1:0]; default: 2;
         *  Configures km memory low power mode in low power stage.
         *  0(default): deep sleep
         *  1: light sleep
         *  2: shut down
         *  3: disable low power stage
         */
        uint32_t sys_km_mem_lp_mode:2;
        /** sys_km_mem_lp_en : R/W; bitpos: [2]; default: 0;
         *  Set this bit to power down km memory.
         */
        uint32_t sys_km_mem_lp_en:1;
        /** sys_km_mem_lp_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control km memory, disable hardware control.
         */
        uint32_t sys_km_mem_lp_force_ctrl:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_system_km_mem_lp_ctrl_reg_t;

/** Type of sys_dma2d_mem_lp_ctrl register
 *  vpu memory power control register
 */
typedef union {
    struct {
        /** sys_2ddma_mem_lp_mode : R/W; bitpos: [1:0]; default: 2;
         *  Configures 2ddma memory low power mode in low power stage.
         *  0(default): deep sleep
         *  1: light sleep
         *  2: shut down
         *  3: disable low power stage
         */
        uint32_t sys_2ddma_mem_lp_mode:2;
        /** sys_2ddma_mem_lp_en : R/W; bitpos: [2]; default: 1;
         *  Set this bit to power down 2ddma memory.
         */
        uint32_t sys_2ddma_mem_lp_en:1;
        /** sys_2ddma_mem_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control 2ddma memory, disable hardware control.
         */
        uint32_t sys_2ddma_mem_force_ctrl:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_system_dma2d_mem_lp_ctrl_reg_t;

/** Type of sys_axi_pdma_mem_lp_ctrl register
 *  vpu memory power control register
 */
typedef union {
    struct {
        /** sys_axi_pdma_mem_lp_mode : R/W; bitpos: [1:0]; default: 0;
         *  Configures axi_pdma memory low power mode in low power stage.
         *  0(default): deep sleep
         *  1: light sleep
         *  2: shut down
         *  3: disable low power stage
         */
        uint32_t sys_axi_pdma_mem_lp_mode:2;
        /** sys_axi_pdma_mem_lp_en : R/W; bitpos: [2]; default: 0;
         *  Set this bit to power down axi_pdma memory.
         */
        uint32_t sys_axi_pdma_mem_lp_en:1;
        /** sys_axi_pdma_mem_lp_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control axi_pdma memory, disable hardware control.
         */
        uint32_t sys_axi_pdma_mem_lp_force_ctrl:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_system_axi_pdma_mem_lp_ctrl_reg_t;

/** Type of sys_ecc_mem_lp_ctrl register
 *  HP CORE0 & HP CORE1 memory power control register
 */
typedef union {
    struct {
        /** sys_ecc_mem_lp_mode : R/W; bitpos: [1:0]; default: 2;
         *  Configures ecc memory low power mode in low power stage.
         *  0: deep sleep
         *  1: light sleep
         *  2(default): shut down
         *  3: disable low power stage
         */
        uint32_t sys_ecc_mem_lp_mode:2;
        /** sys_ecc_mem_lp_en : R/W; bitpos: [2]; default: 0;
         *  Set this bit to power down ecc memory.
         */
        uint32_t sys_ecc_mem_lp_en:1;
        /** sys_ecc_mem_lp_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control ecc memory, disable hardware control.
         */
        uint32_t sys_ecc_mem_lp_force_ctrl:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_system_ecc_mem_lp_ctrl_reg_t;

/** Type of sys_rsa_mem_lp_ctrl register
 *  HP CORE0 & HP CORE1 memory power control register
 */
typedef union {
    struct {
        /** sys_rsa_mem_lp_mode : R/W; bitpos: [1:0]; default: 2;
         *  Configures rsa memory low power mode in low power stage.
         *  0: deep sleep
         *  1: light sleep
         *  2(default): shut down
         *  3: disable low power stage
         */
        uint32_t sys_rsa_mem_lp_mode:2;
        /** sys_rsa_mem_lp_en : R/W; bitpos: [2]; default: 0;
         *  Set this bit to power down rsa memory.
         */
        uint32_t sys_rsa_mem_lp_en:1;
        /** sys_rsa_mem_lp_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control rsa memory, disable hardware control.
         */
        uint32_t sys_rsa_mem_lp_force_ctrl:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_system_rsa_mem_lp_ctrl_reg_t;

/** Type of sys_bitscram_mem_lp_ctrl register
 *  HP CORE0 & HP CORE1 memory power control register
 */
typedef union {
    struct {
        /** sys_bitscram_mem_lp_mode : R/W; bitpos: [1:0]; default: 2;
         *  Configures bitscram memory low power mode in low power stage.
         *  0: deep sleep
         *  1: light sleep
         *  2(default): shut down
         *  3: disable low power stage
         */
        uint32_t sys_bitscram_mem_lp_mode:2;
        /** sys_bitscram_mem_lp_en : R/W; bitpos: [2]; default: 1;
         *  Set this bit to power down bitscram memory.
         */
        uint32_t sys_bitscram_mem_lp_en:1;
        /** sys_bitscram_mem_lp_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control bitscram memory, disable hardware control.
         */
        uint32_t sys_bitscram_mem_lp_force_ctrl:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_system_bitscram_mem_lp_ctrl_reg_t;

/** Type of sys_can0_mem_lp_ctrl register
 *  HP CORE0 & HP CORE1 memory power control register
 */
typedef union {
    struct {
        /** sys_can0_mem_lp_mode : R/W; bitpos: [1:0]; default: 2;
         *  Configures can0 memory low power mode in low power stage.
         *  0(default): deep sleep
         *  1: light sleep
         *  2: shut down
         *  3: disable low power stage
         */
        uint32_t sys_can0_mem_lp_mode:2;
        /** sys_can0_mem_lp_en : R/W; bitpos: [2]; default: 1;
         *  Set this bit to power down can0 memory.
         */
        uint32_t sys_can0_mem_lp_en:1;
        /** sys_can0_mem_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control can0 memory, disable hardware control.
         */
        uint32_t sys_can0_mem_force_ctrl:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_system_can0_mem_lp_ctrl_reg_t;

/** Type of sys_mspi_mem_lp_ctrl register
 *  HP CORE0 & HP CORE1 memory power control register
 */
typedef union {
    struct {
        /** sys_psram_mspi_mem_lp_mode : R/W; bitpos: [1:0]; default: 2;
         *  Configures psram_mspi memory low power mode in low power stage.
         *  0: deep sleep
         *  1: light sleep
         *  2(default): shut down
         *  3: disable low power stage
         */
        uint32_t sys_psram_mspi_mem_lp_mode:2;
        /** sys_psram_mspi_mem_lp_en : R/W; bitpos: [2]; default: 0;
         *  Set this bit to power down psram_mspi memory.
         */
        uint32_t sys_psram_mspi_mem_lp_en:1;
        /** sys_psram_mspi_mem_lp_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control psram_mspi memory, disable hardware control.
         */
        uint32_t sys_psram_mspi_mem_lp_force_ctrl:1;
        /** sys_flash_mspi_mem_lp_mode : R/W; bitpos: [5:4]; default: 2;
         *  Configures flash_mspi memory low power mode in low power stage.
         *  0: deep sleep
         *  1: light sleep
         *  2(default): shut down
         *  3: disable low power stage
         */
        uint32_t sys_flash_mspi_mem_lp_mode:2;
        /** sys_flash_mspi_mem_lp_en : R/W; bitpos: [6]; default: 0;
         *  Set this bit to power down flash_mspi memory.
         */
        uint32_t sys_flash_mspi_mem_lp_en:1;
        /** sys_flash_mspi_mem_lp_force_ctrl : R/W; bitpos: [7]; default: 0;
         *  Set this bit to force software control flash_mspi memory, disable hardware control.
         */
        uint32_t sys_flash_mspi_mem_lp_force_ctrl:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} hp_system_mspi_mem_lp_ctrl_reg_t;

/** Type of sys_hpcore_mem_lp_ctrl register
 *  HP CORE0 & HP CORE1 memory power control register
 */
typedef union {
    struct {
        /** sys_hpcore_mem_lp_mode : R/W; bitpos: [1:0]; default: 0;
         *  Configures hpcore memory low power mode in low power stage.
         *  0: deep sleep
         *  1: light sleep
         *  2(default): shut down
         *  3: disable low power stage
         */
        uint32_t sys_hpcore_mem_lp_mode:2;
        /** sys_hpcore_mem_lp_en : R/W; bitpos: [2]; default: 0;
         *  Set this bit to power down hpcore memory.
         */
        uint32_t sys_hpcore_mem_lp_en:1;
        /** sys_hpcore_mem_lp_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control hpcore memory, disable hardware control.
         */
        uint32_t sys_hpcore_mem_lp_force_ctrl:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_system_hpcore_mem_lp_ctrl_reg_t;

/** Type of sys_rom_mem_lp_ctrl register
 *  rom power control register
 */
typedef union {
    struct {
        /** sys_rom_mem_lp_mode : R/W; bitpos: [1:0]; default: 2;
         *  Configures rom low power mode in low power stage.
         *  0(default): deep sleep
         *  1: light sleep
         *  2: shut down
         *  3: disable low power stage
         */
        uint32_t sys_rom_mem_lp_mode:2;
        /** sys_rom_mem_lp_en : R/W; bitpos: [3:2]; default: 0;
         *  Set this bit to power down rom.
         */
        uint32_t sys_rom_mem_lp_en:2;
        /** sys_rom_mem_lp_force_ctrl : R/W; bitpos: [5:4]; default: 0;
         *  Set this bit to force software control rom, disable hardware control.
         */
        uint32_t sys_rom_mem_lp_force_ctrl:2;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} hp_system_rom_mem_lp_ctrl_reg_t;

/** Type of sys_l1_cache_mem_lp_ctrl register
 *  HP CORE0 & HP CORE1 memory power control register
 */
typedef union {
    struct {
        /** sys_l1_cache_mem_lp_mode : R/W; bitpos: [1:0]; default: 2;
         *  Configures l1_cache memory low power mode in low power stage.
         *  0: deep sleep
         *  1: light sleep
         *  2(default): shut down
         *  3: disable low power stage
         */
        uint32_t sys_l1_cache_mem_lp_mode:2;
        /** sys_l1_cache_mem_lp_en : R/W; bitpos: [2]; default: 0;
         *  Set this bit to power down l1_cache memory.
         */
        uint32_t sys_l1_cache_mem_lp_en:1;
        /** sys_l1_cache_mem_lp_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control l1_cache memory, disable hardware control.
         */
        uint32_t sys_l1_cache_mem_lp_force_ctrl:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_system_l1_cache_mem_lp_ctrl_reg_t;

/** Type of sys_kyber_mem_lp_ctrl register
 *  HP CORE0 & HP CORE1 memory power control register
 */
typedef union {
    struct {
        /** sys_kyber_mem_lp_mode : R/W; bitpos: [1:0]; default: 2;
         *  Configures kyber memory low power mode in low power stage.
         *  0: deep sleep
         *  1: light sleep
         *  2(default): shut down
         *  3: disable low power stage
         */
        uint32_t sys_kyber_mem_lp_mode:2;
        /** sys_kyber_mem_lp_en : R/W; bitpos: [2]; default: 1;
         *  Set this bit to power down kyber memory.
         */
        uint32_t sys_kyber_mem_lp_en:1;
        /** sys_kyber_mem_lp_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control kyber memory, disable hardware control.
         */
        uint32_t sys_kyber_mem_lp_force_ctrl:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_system_kyber_mem_lp_ctrl_reg_t;

/** Type of sys_lcdcam_mem_lp_ctrl register
 *  HP CORE0 & HP CORE1 memory power control register
 */
typedef union {
    struct {
        /** sys_lcdcam_mem_lp_mode : R/W; bitpos: [1:0]; default: 2;
         *  Configures lcdcam memory low power mode in low power stage.
         *  0: deep sleep
         *  1: light sleep
         *  2(default): shut down
         *  3: disable low power stage
         */
        uint32_t sys_lcdcam_mem_lp_mode:2;
        /** sys_lcdcam_mem_lp_en : R/W; bitpos: [2]; default: 1;
         *  Set this bit to power down lcdcam memory.
         */
        uint32_t sys_lcdcam_mem_lp_en:1;
        /** sys_lcdcam_mem_lp_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control lcdcam memory, disable hardware control.
         */
        uint32_t sys_lcdcam_mem_lp_force_ctrl:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_system_lcdcam_mem_lp_ctrl_reg_t;

/** Type of sys_uart0_mem_lp_ctrl register
 *  uart memory power control register
 */
typedef union {
    struct {
        /** sys_uart0_mem_lp_mode : R/W; bitpos: [1:0]; default: 0;
         *  Configures uart0 memory low power mode in low power stage.
         *  0(default): deep sleep
         *  1: light sleep
         *  2: shut down
         *  3: disable low power stage
         */
        uint32_t sys_uart0_mem_lp_mode:2;
        /** sys_uart0_mem_lp_en : R/W; bitpos: [2]; default: 0;
         *  Set this bit to power down uart0 memory.
         */
        uint32_t sys_uart0_mem_lp_en:1;
        /** sys_uart0_mem_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control uart0 memory, disable hardware control.
         */
        uint32_t sys_uart0_mem_force_ctrl:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_system_uart0_mem_lp_ctrl_reg_t;

/** Type of sys_uart1_mem_lp_ctrl register
 *  uart memory power control register
 */
typedef union {
    struct {
        /** sys_uart1_mem_lp_mode : R/W; bitpos: [1:0]; default: 2;
         *  Configures uart1 memory low power mode in low power stage.
         *  0(default): deep sleep
         *  1: light sleep
         *  2: shut down
         *  3: disable low power stage
         */
        uint32_t sys_uart1_mem_lp_mode:2;
        /** sys_uart1_mem_lp_en : R/W; bitpos: [2]; default: 1;
         *  Set this bit to power down uart1 memory.
         */
        uint32_t sys_uart1_mem_lp_en:1;
        /** sys_uart1_mem_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control uart1 memory, disable hardware control.
         */
        uint32_t sys_uart1_mem_force_ctrl:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_system_uart1_mem_lp_ctrl_reg_t;

/** Type of sys_uart2_mem_lp_ctrl register
 *  uart memory power control register
 */
typedef union {
    struct {
        /** sys_uart2_mem_lp_mode : R/W; bitpos: [1:0]; default: 2;
         *  Configures uart2 memory low power mode in low power stage.
         *  0(default): deep sleep
         *  1: light sleep
         *  2: shut down
         *  3: disable low power stage
         */
        uint32_t sys_uart2_mem_lp_mode:2;
        /** sys_uart2_mem_lp_en : R/W; bitpos: [2]; default: 1;
         *  Set this bit to power down uart2 memory.
         */
        uint32_t sys_uart2_mem_lp_en:1;
        /** sys_uart2_mem_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control uart2 memory, disable hardware control.
         */
        uint32_t sys_uart2_mem_force_ctrl:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_system_uart2_mem_lp_ctrl_reg_t;

/** Type of sys_ledc1_mem_lp_ctrl register
 *  ledc1 memory power control register
 */
typedef union {
    struct {
        /** sys_ledc1_mem_lp_mode : R/W; bitpos: [1:0]; default: 2;
         *  Configures ledc1 memory low power mode in low power stage.
         *  0(default): deep sleep
         *  1: light sleep
         *  2: shut down
         *  3: disable low power stage
         */
        uint32_t sys_ledc1_mem_lp_mode:2;
        /** sys_ledc1_mem_lp_en : R/W; bitpos: [2]; default: 1;
         *  Set this bit to power down ledc1 memory.
         */
        uint32_t sys_ledc1_mem_lp_en:1;
        /** sys_ledc1_mem_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control ledc1 memory, disable hardware control.
         */
        uint32_t sys_ledc1_mem_force_ctrl:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_system_ledc1_mem_lp_ctrl_reg_t;

/** Type of sys_ppa_mem_lp_ctrl register
 *  vpu memory power control register
 */
typedef union {
    struct {
        /** sys_ppa_sprf_mem_lp_mode : R/W; bitpos: [1:0]; default: 2;
         *  Configures ppa_sprf memory low power mode in low power stage.
         *  0(default): deep sleep
         *  1: light sleep
         *  2: shut down
         *  3: disable low power stage
         */
        uint32_t sys_ppa_sprf_mem_lp_mode:2;
        /** sys_ppa_sprf_mem_lp_en : R/W; bitpos: [2]; default: 1;
         *  Set this bit to power down ppa_sprf memory.
         */
        uint32_t sys_ppa_sprf_mem_lp_en:1;
        /** sys_ppa_sprf_mem_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control ppa_sprf memory, disable hardware control.
         */
        uint32_t sys_ppa_sprf_mem_force_ctrl:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_system_ppa_mem_lp_ctrl_reg_t;

/** Type of sys_jpeg_mem_lp_ctrl register
 *  vpu memory power control register
 */
typedef union {
    struct {
        /** sys_jpeg_mem_lp_mode : R/W; bitpos: [1:0]; default: 2;
         *  Configures jpeg memory low power mode in low power stage.
         *  0(default): deep sleep
         *  1: light sleep
         *  2: shut down
         *  3: disable low power stage
         */
        uint32_t sys_jpeg_mem_lp_mode:2;
        /** sys_jpeg_mem_lp_en : R/W; bitpos: [2]; default: 1;
         *  Set this bit to power down jpeg memory.
         */
        uint32_t sys_jpeg_mem_lp_en:1;
        /** sys_jpeg_mem_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control jpeg memory, disable hardware control.
         */
        uint32_t sys_jpeg_mem_force_ctrl:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_system_jpeg_mem_lp_ctrl_reg_t;

/** Type of sys_can1_mem_lp_ctrl register
 *  HP CORE0 & HP CORE1 memory power control register
 */
typedef union {
    struct {
        /** sys_can1_mem_lp_mode : R/W; bitpos: [1:0]; default: 2;
         *  Configures can1 memory low power mode in low power stage.
         *  0(default): deep sleep
         *  1: light sleep
         *  2: shut down
         *  3: disable low power stage
         */
        uint32_t sys_can1_mem_lp_mode:2;
        /** sys_can1_mem_lp_en : R/W; bitpos: [2]; default: 1;
         *  Set this bit to power down can1 memory.
         */
        uint32_t sys_can1_mem_lp_en:1;
        /** sys_can1_mem_force_ctrl : R/W; bitpos: [3]; default: 0;
         *  Set this bit to force software control can1 memory, disable hardware control.
         */
        uint32_t sys_can1_mem_force_ctrl:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_system_can1_mem_lp_ctrl_reg_t;

/** Type of sys_cpu_acs_cache_mem_conf register
 *  CPU access Cache data mem configuration register
 */
typedef union {
    struct {
        /** sys_cpu_access_ac0_datamem_en : HRO; bitpos: [0]; default: 0;
         *  Set this bit to enable CPU accessROMCache0 data mem path
         */
        uint32_t sys_cpu_access_ac0_datamem_en:1;
        /** sys_cpu_access_ac1_datamem_en : HRO; bitpos: [1]; default: 0;
         *  Set this bit to enable CPU access ROMCache1 data mem path
         */
        uint32_t sys_cpu_access_ac1_datamem_en:1;
        /** sys_cpu_access_ic0_datamem_en : R/W; bitpos: [2]; default: 0;
         *  Set this bit to enable CPU access ICache0 data mem path
         */
        uint32_t sys_cpu_access_ic0_datamem_en:1;
        /** sys_cpu_access_ic1_datamem_en : R/W; bitpos: [3]; default: 0;
         *  Set this bit to enable CPU access ICache1 data mem path
         */
        uint32_t sys_cpu_access_ic1_datamem_en:1;
        /** sys_cpu_access_dc_datamem_en : R/W; bitpos: [4]; default: 0;
         *  Set this bit to enable CPU access DCache data mem path
         */
        uint32_t sys_cpu_access_dc_datamem_en:1;
        /** sys_cpu_acs_cache_mem_lock : R/W; bitpos: [5]; default: 0;
         *  Set 1 to lock cpu acs mem data mem path configuration
         */
        uint32_t sys_cpu_acs_cache_mem_lock:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} hp_system_cpu_acs_cache_mem_conf_reg_t;

/** Type of sys_sdio_pad_bist_cfg register
 *  sdio pad bist control register
 */
typedef union {
    struct {
        /** sys_sdio_pad_bist_start : WT; bitpos: [0]; default: 0;
         *  Write 1 to start sdio pad bist
         */
        uint32_t sys_sdio_pad_bist_start:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_sdio_pad_bist_cfg_reg_t;

/** Type of sys_gmac0_pad_bist_cfg register
 *  gmac0 pad bist control register
 */
typedef union {
    struct {
        /** sys_gmac0_pad_bist_start : WT; bitpos: [0]; default: 0;
         *  Write 1 to start gmac0 pad bist
         */
        uint32_t sys_gmac0_pad_bist_start:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_gmac0_pad_bist_cfg_reg_t;

/** Type of sys_gmac1_pad_bist_cfg register
 *  gmac1 pad bist control register
 */
typedef union {
    struct {
        /** sys_gmac1_pad_bist_start : WT; bitpos: [0]; default: 0;
         *  Write 1 to start gmac1 pad bist
         */
        uint32_t sys_gmac1_pad_bist_start:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_gmac1_pad_bist_cfg_reg_t;


/** Group: configure_register */
/** Type of core_ibus_timeout_conf register
 *  need_des
 */
typedef union {
    struct {
        /** core_ibus_timeout_protect_en : R/W; bitpos: [0]; default: 1;
         *  set this field to 1 to enable hp core0&1 ibus timeout handle
         */
        uint32_t core_ibus_timeout_protect_en:1;
        /** core_ibus_timeout_thres : R/W; bitpos: [16:1]; default: 65535;
         *  This field used to set hp core0&1 ibus timeout  threshold
         */
        uint32_t core_ibus_timeout_thres:16;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} hp_system_core_ibus_timeout_conf_reg_t;

/** Type of core_dbus_timeout_conf register
 *  need_des
 */
typedef union {
    struct {
        /** core_dbus_timeout_protect_en : R/W; bitpos: [0]; default: 1;
         *  set this field to 1 to enable hp core0&1 dbus timeout handle
         */
        uint32_t core_dbus_timeout_protect_en:1;
        /** core_dbus_timeout_thres : R/W; bitpos: [16:1]; default: 65535;
         *  This field used to set hp core0&1 dbus timeout  threshold
         */
        uint32_t core_dbus_timeout_thres:16;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} hp_system_core_dbus_timeout_conf_reg_t;

/** Type of icm_h2x_cfg register
 *  need_des
 */
typedef union {
    struct {
        /** cpu_icm_h2x_post_wr_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t cpu_icm_h2x_post_wr_en:1;
        /** cpu_icm_h2x_cut_through_en : R/W; bitpos: [1]; default: 1;
         *  need_des
         */
        uint32_t cpu_icm_h2x_cut_through_en:1;
        /** cpu_icm_h2x_bridge_busy : RO; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t cpu_icm_h2x_bridge_busy:1;
        /** sdmmc_icm_h2x_post_wr_en : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t sdmmc_icm_h2x_post_wr_en:1;
        /** sdmmc_icm_h2x_cut_through_en : R/W; bitpos: [4]; default: 1;
         *  need_des
         */
        uint32_t sdmmc_icm_h2x_cut_through_en:1;
        /** sdmmc_icm_h2x_bridge_busy : RO; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t sdmmc_icm_h2x_bridge_busy:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} hp_system_icm_h2x_cfg_reg_t;

/** Type of bitscrambler_peri_sel register
 *  Bitscrambler Peri Sel
 */
typedef union {
    struct {
        /** bitscrambler_peri_rx_sel : R/W; bitpos: [4:0]; default: 0;
         *  Set  this field to sel peri with DMA RX interface to connect with bitscrambler:
         *  lcd_cam: 5'h1, gpspi2: 5'h2: gpspi3: 5'h3, parl_io:5'h4, aes: 5'h5, sha: 5'h6, adc:
         *  5'h7, i2s0_ch0: 5'h8, i2s0_ch1: 5'h9, i2s0_ch2: 5'ha, i2s0_ch3: 5'hb,
         *  i2s1_ch0: 5'hc, i2s1_ch1: 5'hd, i2s1_ch2: 5'he, i2s1_ch3: 5'hf, uhci0: 5'h10, RMT:
         *  5'h11, none: else.
         */
        uint32_t bitscrambler_peri_rx_sel:5;
        /** bitscrambler_peri_tx_sel : R/W; bitpos: [9:5]; default: 0;
         *  Set  this field to sel peri with DMA TX interface to connect with bitscrambler:
         *  lcd_cam: 5'h1, gpspi2: 5'h2: gpspi3: 5'h3, parl_io:5'h4, aes: 5'h5, sha: 5'h6, adc:
         *  5'h7, i2s0_ch0: 5'h8, i2s0_ch1: 5'h9, i2s0_ch2: 5'ha, i2s0_ch3: 5'hb,
         *  i2s1_ch0: 5'hc, i2s1_ch1: 5'hd, i2s1_ch2: 5'he, i2s1_ch3: 5'hf, uhci0: 5'h10, RMT:
         *  5'h11, none: else.
         */
        uint32_t bitscrambler_peri_tx_sel:5;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} hp_system_bitscrambler_peri_sel_reg_t;


/** Group: GDMA Ctonrol Register */
/** Type of sys_gdma_ctrl register
 *  N/A
 */
typedef union {
    struct {
        /** sys_debug_ch_num : R/W; bitpos: [1:0]; default: 0;
         *  N/A
         */
        uint32_t sys_debug_ch_num:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_gdma_ctrl_reg_t;


/** Group: VPU Control Register */
/** Type of sys_vpu_ctrl register
 *  N/A
 */
typedef union {
    struct {
        /** sys_ppa_lslp_mem_pd : R/W; bitpos: [0]; default: 0;
         *  N/A
         */
        uint32_t sys_ppa_lslp_mem_pd:1;
        /** sys_jpeg_sdslp_mem_pd : R/W; bitpos: [1]; default: 0;
         *  N/A
         */
        uint32_t sys_jpeg_sdslp_mem_pd:1;
        /** sys_jpeg_lslp_mem_pd : R/W; bitpos: [2]; default: 0;
         *  N/A
         */
        uint32_t sys_jpeg_lslp_mem_pd:1;
        /** sys_jpeg_dslp_mem_pd : R/W; bitpos: [3]; default: 0;
         *  N/A
         */
        uint32_t sys_jpeg_dslp_mem_pd:1;
        /** sys_dma2d_lslp_mem_pd : R/W; bitpos: [4]; default: 0;
         *  N/A
         */
        uint32_t sys_dma2d_lslp_mem_pd:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} hp_system_vpu_ctrl_reg_t;


/** Group: HP_DESIGN_FOR_VERIFICATION0 */
/** Type of design_for_verification0 register
 *  need_des
 */
typedef union {
    struct {
        /** dfv0 : R/W; bitpos: [31:0]; default: 0;
         *  register for DV
         */
        uint32_t dfv0:32;
    };
    uint32_t val;
} hp_system_design_for_verification0_reg_t;


/** Group: HP_DESIGN_FOR_VERIFICATION1 */
/** Type of design_for_verification1 register
 *  need_des
 */
typedef union {
    struct {
        /** dfv1 : R/W; bitpos: [31:0]; default: 0;
         *  register for DV
         */
        uint32_t dfv1:32;
    };
    uint32_t val;
} hp_system_design_for_verification1_reg_t;


/** Group: Interrupt Registers */
/** Type of ahb2axi_bresp_err_int_raw register
 *  NA
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** sdmmc_icm_h2x_bresp_err_int_raw : R/WTC/SS; bitpos: [30]; default: 0;
         *  the raw interrupt status of bresp error,  triggered when if  bresp err occurs  in
         *  post write mode in ahb2axi.
         */
        uint32_t sdmmc_icm_h2x_bresp_err_int_raw:1;
        /** cpu_icm_h2x_bresp_err_int_raw : R/WTC/SS; bitpos: [31]; default: 0;
         *  the raw interrupt status of bresp error,  triggered when if  bresp err occurs  in
         *  post write mode in ahb2axi.
         */
        uint32_t cpu_icm_h2x_bresp_err_int_raw:1;
    };
    uint32_t val;
} hp_system_ahb2axi_bresp_err_int_raw_reg_t;

/** Type of ahb2axi_bresp_err_int_st register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** sdmmc_icm_h2x_bresp_err_int_st : RO; bitpos: [30]; default: 0;
         *  the masked interrupt status of  sdmmc_icm_h2x_bresp_err
         */
        uint32_t sdmmc_icm_h2x_bresp_err_int_st:1;
        /** cpu_icm_h2x_bresp_err_int_st : RO; bitpos: [31]; default: 0;
         *  the masked interrupt status of  cpu_icm_h2x_bresp_err
         */
        uint32_t cpu_icm_h2x_bresp_err_int_st:1;
    };
    uint32_t val;
} hp_system_ahb2axi_bresp_err_int_st_reg_t;

/** Type of ahb2axi_bresp_err_int_ena register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** sdmmc_icm_h2x_bresp_err_int_ena : R/W; bitpos: [30]; default: 0;
         *  Write 1 to enable sdmmc_icm_h2x_bresp_err int
         */
        uint32_t sdmmc_icm_h2x_bresp_err_int_ena:1;
        /** cpu_icm_h2x_bresp_err_int_ena : R/W; bitpos: [31]; default: 0;
         *  Write 1 to enable cpu_icm_h2x_bresp_err int
         */
        uint32_t cpu_icm_h2x_bresp_err_int_ena:1;
    };
    uint32_t val;
} hp_system_ahb2axi_bresp_err_int_ena_reg_t;

/** Type of ahb2axi_bresp_err_int_clr register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** sdmmc_icm_h2x_bresp_err_int_clr : WT; bitpos: [30]; default: 0;
         *  Write 1 to clear sdmmc_icm_h2x_bresp_err  int
         */
        uint32_t sdmmc_icm_h2x_bresp_err_int_clr:1;
        /** cpu_icm_h2x_bresp_err_int_clr : WT; bitpos: [31]; default: 0;
         *  Write 1 to clear cpu_icm_h2x_bresp_err  int
         */
        uint32_t cpu_icm_h2x_bresp_err_int_clr:1;
    };
    uint32_t val;
} hp_system_ahb2axi_bresp_err_int_clr_reg_t;

/** Type of core_timeout_int_raw register
 *  Hp core bus timeout interrupt raw register
 */
typedef union {
    struct {
        uint32_t reserved_0:2;
        /** core0_ibus_timeout_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  the raw interrupt status of hp core0  ibus timeout
         */
        uint32_t core0_ibus_timeout_int_raw:1;
        /** core1_ibus_timeout_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  the raw interrupt status of hp core1  ibus timeout
         */
        uint32_t core1_ibus_timeout_int_raw:1;
        /** core0_dbus_timeout_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  the raw interrupt status of hp core0  dbus timeout
         */
        uint32_t core0_dbus_timeout_int_raw:1;
        /** core1_dbus_timeout_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  the raw interrupt status of hp core1  dbus timeout
         */
        uint32_t core1_dbus_timeout_int_raw:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} hp_system_core_timeout_int_raw_reg_t;

/** Type of core_timeout_int_st register
 *  masked interrupt register
 */
typedef union {
    struct {
        uint32_t reserved_0:2;
        /** core0_ibus_timeout_int_st : RO; bitpos: [2]; default: 0;
         *  the masked interrupt status of hp core0  ibus timeout
         */
        uint32_t core0_ibus_timeout_int_st:1;
        /** core1_ibus_timeout_int_st : RO; bitpos: [3]; default: 0;
         *  the masked interrupt status of hp core1  ibus timeout
         */
        uint32_t core1_ibus_timeout_int_st:1;
        /** core0_dbus_timeout_int_st : RO; bitpos: [4]; default: 0;
         *  the masked interrupt status of hp core0  dbus timeout
         */
        uint32_t core0_dbus_timeout_int_st:1;
        /** core1_dbus_timeout_int_st : RO; bitpos: [5]; default: 0;
         *  the masked interrupt status of hp core1  dbus timeout
         */
        uint32_t core1_dbus_timeout_int_st:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} hp_system_core_timeout_int_st_reg_t;

/** Type of core_timeout_int_ena register
 *  masked interrupt register
 */
typedef union {
    struct {
        uint32_t reserved_0:2;
        /** core0_ibus_timeout_int_ena : R/W; bitpos: [2]; default: 0;
         *  Write 1 to enable hp_system_core0_ibus_timeout int
         */
        uint32_t core0_ibus_timeout_int_ena:1;
        /** core1_ibus_timeout_int_ena : R/W; bitpos: [3]; default: 0;
         *  Write 1 to enable hp_system_core1_ibus_timeout int
         */
        uint32_t core1_ibus_timeout_int_ena:1;
        /** core0_dbus_timeout_int_ena : R/W; bitpos: [4]; default: 0;
         *  Write 1 to enable hp_system_core0_dbus_timeout int
         */
        uint32_t core0_dbus_timeout_int_ena:1;
        /** core1_dbus_timeout_int_ena : R/W; bitpos: [5]; default: 0;
         *  Write 1 to enable hp_system_core1_dbus_timeout int
         */
        uint32_t core1_dbus_timeout_int_ena:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} hp_system_core_timeout_int_ena_reg_t;

/** Type of core_timeout_int_clr register
 *  interrupt clear register
 */
typedef union {
    struct {
        uint32_t reserved_0:2;
        /** core0_ibus_timeout_int_clr : WT; bitpos: [2]; default: 0;
         *  Write 1 to clear hp_system_core0_ibus_timeout int
         */
        uint32_t core0_ibus_timeout_int_clr:1;
        /** core1_ibus_timeout_int_clr : WT; bitpos: [3]; default: 0;
         *  Write 1 to clear hp_system_core1_ibus_timeout int
         */
        uint32_t core1_ibus_timeout_int_clr:1;
        /** core0_dbus_timeout_int_clr : WT; bitpos: [4]; default: 0;
         *  Write 1 to clear hp_system_core0_dbus_timeout int
         */
        uint32_t core0_dbus_timeout_int_clr:1;
        /** core1_dbus_timeout_int_clr : WT; bitpos: [5]; default: 0;
         *  Write 1 to clear hp_system_core1_dbus_timeout int
         */
        uint32_t core1_dbus_timeout_int_clr:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} hp_system_core_timeout_int_clr_reg_t;

/** Type of sys_sdio_pad_bist_int_raw register
 *  sdio pad bist interrupt raw register
 */
typedef union {
    struct {
        /** sys_sdio_pad_bist_ok_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  intr triggered when bist finish and status is bist ok
         */
        uint32_t sys_sdio_pad_bist_ok_int_raw:1;
        /** sys_sdio_pad_bist_fail_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  intr triggered when bist finish and status is bist fail
         */
        uint32_t sys_sdio_pad_bist_fail_int_raw:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_sdio_pad_bist_int_raw_reg_t;

/** Type of sys_sdio_pad_bist_int_st register
 *  masked interrupt register
 */
typedef union {
    struct {
        /** sys_sdio_pad_bist_ok_int_st : RO; bitpos: [0]; default: 0;
         *  the masked interrupt status of sdio pad bist ok interrupt
         */
        uint32_t sys_sdio_pad_bist_ok_int_st:1;
        /** sys_sdio_pad_bist_fail_int_st : RO; bitpos: [1]; default: 0;
         *  the masked interrupt status of sdio pad bist fail interrupt
         */
        uint32_t sys_sdio_pad_bist_fail_int_st:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_sdio_pad_bist_int_st_reg_t;

/** Type of sys_sdio_pad_bist_int_ena register
 *  interrupt enable register
 */
typedef union {
    struct {
        /** sys_sdio_pad_bist_ok_int_ena : R/W; bitpos: [0]; default: 0;
         *  Write 1 to enable sdio pad bist ok interrupt
         */
        uint32_t sys_sdio_pad_bist_ok_int_ena:1;
        /** sys_sdio_pad_bist_fail_int_ena : R/W; bitpos: [1]; default: 0;
         *  Write 1 to enable sdio pad bist fail interrupt
         */
        uint32_t sys_sdio_pad_bist_fail_int_ena:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_sdio_pad_bist_int_ena_reg_t;

/** Type of sys_sdio_pad_bist_int_clr register
 *  interrupt clear register
 */
typedef union {
    struct {
        /** sys_sdio_pad_bist_ok_int_clr : WT; bitpos: [0]; default: 0;
         *  Write 1 to clr sdio pad bist ok interrupt
         */
        uint32_t sys_sdio_pad_bist_ok_int_clr:1;
        /** sys_sdio_pad_bist_fail_int_clr : WT; bitpos: [1]; default: 0;
         *  Write 1 to clr sdio pad bist fail interrupt
         */
        uint32_t sys_sdio_pad_bist_fail_int_clr:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_sdio_pad_bist_int_clr_reg_t;

/** Type of sys_gmac0_pad_bist_int_raw register
 *  gmac0 pad bist interrupt raw register
 */
typedef union {
    struct {
        /** sys_gmac0_pad_bist_ok_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  intr triggered when bist finish and status is bist ok
         */
        uint32_t sys_gmac0_pad_bist_ok_int_raw:1;
        /** sys_gmac0_pad_bist_fail_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  intr triggered when bist finish and status is bist fail
         */
        uint32_t sys_gmac0_pad_bist_fail_int_raw:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_gmac0_pad_bist_int_raw_reg_t;

/** Type of sys_gmac0_pad_bist_int_st register
 *  masked interrupt register
 */
typedef union {
    struct {
        /** sys_gmac0_pad_bist_ok_int_st : RO; bitpos: [0]; default: 0;
         *  the masked interrupt status of gmac0 pad bist ok interrupt
         */
        uint32_t sys_gmac0_pad_bist_ok_int_st:1;
        /** sys_gmac0_pad_bist_fail_int_st : RO; bitpos: [1]; default: 0;
         *  the masked interrupt status of gmac0 pad bist fail interrupt
         */
        uint32_t sys_gmac0_pad_bist_fail_int_st:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_gmac0_pad_bist_int_st_reg_t;

/** Type of sys_gmac0_pad_bist_int_ena register
 *  interrupt enable register
 */
typedef union {
    struct {
        /** sys_gmac0_pad_bist_ok_int_ena : R/W; bitpos: [0]; default: 0;
         *  Write 1 to enable gmac0 pad bist ok interrupt
         */
        uint32_t sys_gmac0_pad_bist_ok_int_ena:1;
        /** sys_gmac0_pad_bist_fail_int_ena : R/W; bitpos: [1]; default: 0;
         *  Write 1 to enable gmac0 pad bist fail interrupt
         */
        uint32_t sys_gmac0_pad_bist_fail_int_ena:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_gmac0_pad_bist_int_ena_reg_t;

/** Type of sys_gmac0_pad_bist_int_clr register
 *  interrupt clear register
 */
typedef union {
    struct {
        /** sys_gmac0_pad_bist_ok_int_clr : WT; bitpos: [0]; default: 0;
         *  Write 1 to clr gmac0 pad bist ok interrupt
         */
        uint32_t sys_gmac0_pad_bist_ok_int_clr:1;
        /** sys_gmac0_pad_bist_fail_int_clr : WT; bitpos: [1]; default: 0;
         *  Write 1 to clr gmac0 pad bist fail interrupt
         */
        uint32_t sys_gmac0_pad_bist_fail_int_clr:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_gmac0_pad_bist_int_clr_reg_t;

/** Type of sys_gmac1_pad_bist_int_raw register
 *  gmac1 pad bist interrupt raw register
 */
typedef union {
    struct {
        /** sys_gmac1_pad_bist_ok_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  intr triggered when bist finish and status is bist ok
         */
        uint32_t sys_gmac1_pad_bist_ok_int_raw:1;
        /** sys_gmac1_pad_bist_fail_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  intr triggered when bist finish and status is bist fail
         */
        uint32_t sys_gmac1_pad_bist_fail_int_raw:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_gmac1_pad_bist_int_raw_reg_t;

/** Type of sys_gmac1_pad_bist_int_st register
 *  masked interrupt register
 */
typedef union {
    struct {
        /** sys_gmac1_pad_bist_ok_int_st : RO; bitpos: [0]; default: 0;
         *  the masked interrupt status of gmac1 pad bist ok interrupt
         */
        uint32_t sys_gmac1_pad_bist_ok_int_st:1;
        /** sys_gmac1_pad_bist_fail_int_st : RO; bitpos: [1]; default: 0;
         *  the masked interrupt status of gmac1 pad bist fail interrupt
         */
        uint32_t sys_gmac1_pad_bist_fail_int_st:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_gmac1_pad_bist_int_st_reg_t;

/** Type of sys_gmac1_pad_bist_int_ena register
 *  interrupt enable register
 */
typedef union {
    struct {
        /** sys_gmac1_pad_bist_ok_int_ena : R/W; bitpos: [0]; default: 0;
         *  Write 1 to enable gmac1 pad bist ok interrupt
         */
        uint32_t sys_gmac1_pad_bist_ok_int_ena:1;
        /** sys_gmac1_pad_bist_fail_int_ena : R/W; bitpos: [1]; default: 0;
         *  Write 1 to enable gmac1 pad bist fail interrupt
         */
        uint32_t sys_gmac1_pad_bist_fail_int_ena:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_gmac1_pad_bist_int_ena_reg_t;

/** Type of sys_gmac1_pad_bist_int_clr register
 *  interrupt clear register
 */
typedef union {
    struct {
        /** sys_gmac1_pad_bist_ok_int_clr : WT; bitpos: [0]; default: 0;
         *  Write 1 to clr gmac1 pad bist ok interrupt
         */
        uint32_t sys_gmac1_pad_bist_ok_int_clr:1;
        /** sys_gmac1_pad_bist_fail_int_clr : WT; bitpos: [1]; default: 0;
         *  Write 1 to clr gmac1 pad bist fail interrupt
         */
        uint32_t sys_gmac1_pad_bist_fail_int_clr:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_gmac1_pad_bist_int_clr_reg_t;


/** Group: HP_CORE_DMACTIVE_LPCORE_REG */
/** Type of core_dmactive_lpcore register
 *  need_des
 */
typedef union {
    struct {
        /** core_dmactive_lpcore : RO; bitpos: [0]; default: 0;
         *  hp core dmactive_lpcore value
         */
        uint32_t core_dmactive_lpcore:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_core_dmactive_lpcore_reg_t;


/** Group: control registers */
/** Type of core_err_resp_dis register
 *  need_des
 */
typedef union {
    struct {
        /** core_err_resp_dis : R/W; bitpos: [2:0]; default: 0;
         *  Set bit0 to disable ibus err resp. Set bit1 to disable dbus err resp.  Set bit 2 to
         *  disable ahb err resp.
         */
        uint32_t core_err_resp_dis:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_system_core_err_resp_dis_reg_t;

/** Type of sys_tcm_performace_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** sys_ahb_to_tcm_mem_en_hold : R/W; bitpos: [0]; default: 0;
         *  Set 1 to keep TCM mem enable hold when mem access is busy.
         */
        uint32_t sys_ahb_to_tcm_mem_en_hold:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_tcm_performace_ctrl_reg_t;


/** Group: Timeout Register */
/** Type of sys_cpu_peri0_timeout_conf register
 *  CPU_PERI0_TIMEOUT configuration register
 */
typedef union {
    struct {
        /** sys_cpu_peri0_timeout_thres : R/W; bitpos: [15:0]; default: 65535;
         *  Configures the timeout threshold for bus access for accessing CPU peripheral0
         *  register in the number of clock cycles of the clock domain.
         */
        uint32_t sys_cpu_peri0_timeout_thres:16;
        /** sys_cpu_peri0_timeout_int_clr : WT; bitpos: [16]; default: 0;
         *  Write 1 to clear timeout interrupt.
         */
        uint32_t sys_cpu_peri0_timeout_int_clr:1;
        /** sys_cpu_peri0_timeout_protect_en : R/W; bitpos: [17]; default: 1;
         *  Configures whether or not to enable timeout protection for accessing CPU
         *  peripheral0 registers.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sys_cpu_peri0_timeout_protect_en:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} hp_system_cpu_peri0_timeout_conf_reg_t;

/** Type of sys_cpu_peri0_timeout_addr register
 *  CPU_PERI0_TIMEOUT_ADDR register
 */
typedef union {
    struct {
        /** sys_cpu_peri0_timeout_addr : RO; bitpos: [31:0]; default: 0;
         *  Represents the address information of abnormal access.
         */
        uint32_t sys_cpu_peri0_timeout_addr:32;
    };
    uint32_t val;
} hp_system_cpu_peri0_timeout_addr_reg_t;

/** Type of sys_cpu_peri0_timeout_uid register
 *  CPU_PERI0_TIMEOUT_UID register
 */
typedef union {
    struct {
        /** sys_cpu_peri0_timeout_uid : RO; bitpos: [7:0]; default: 0;
         *  Represents the master id[5:0] and master permission[6:5] when trigger timeout. This
         *  register will be cleared after the interrupt is cleared.
         */
        uint32_t sys_cpu_peri0_timeout_uid:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} hp_system_cpu_peri0_timeout_uid_reg_t;

/** Type of sys_hp_system_peri0_timeout_conf register
 *  HP_PERI0_TIMEOUT configuration register
 */
typedef union {
    struct {
        /** sys_hp_system_peri0_timeout_thres : R/W; bitpos: [15:0]; default: 65535;
         *  Configures the timeout threshold for bus access for accessing CPU peripheral
         *  register in the number of clock cycles of the clock domain.
         */
        uint32_t sys_hp_system_peri0_timeout_thres:16;
        /** sys_hp_system_peri0_timeout_int_clr : WT; bitpos: [16]; default: 0;
         *  Write 1 to clear timeout interrupt.
         */
        uint32_t sys_hp_system_peri0_timeout_int_clr:1;
        /** sys_hp_system_peri0_timeout_protect_en : R/W; bitpos: [17]; default: 1;
         *  Configures whether or not to enable timeout protection for accessing CPU peripheral
         *  registers.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sys_hp_system_peri0_timeout_protect_en:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} hp_system_hp_system_peri0_timeout_conf_reg_t;

/** Type of sys_hp_system_peri0_timeout_addr register
 *  HP_PERI0_TIMEOUT_ADDR register
 */
typedef union {
    struct {
        /** sys_hp_system_peri0_timeout_addr : RO; bitpos: [31:0]; default: 0;
         *  Represents the address information of abnormal access.
         */
        uint32_t sys_hp_system_peri0_timeout_addr:32;
    };
    uint32_t val;
} hp_system_hp_system_peri0_timeout_addr_reg_t;

/** Type of sys_hp_system_peri0_timeout_uid register
 *  HP_PERI0_TIMEOUT_UID register
 */
typedef union {
    struct {
        /** sys_hp_system_peri0_timeout_uid : RO; bitpos: [7:0]; default: 0;
         *  Represents the master id[5:0] and master permission[6:5] when trigger timeout. This
         *  register will be cleared after the interrupt is cleared.
         */
        uint32_t sys_hp_system_peri0_timeout_uid:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} hp_system_hp_system_peri0_timeout_uid_reg_t;

/** Type of sys_hp_system_peri1_timeout_conf register
 *  HP_PERI1_TIMEOUT configuration register
 */
typedef union {
    struct {
        /** sys_hp_system_peri1_timeout_thres : R/W; bitpos: [15:0]; default: 65535;
         *  Configures the timeout threshold for bus access for accessing CPU peripheral
         *  register in the number of clock cycles of the clock domain.
         */
        uint32_t sys_hp_system_peri1_timeout_thres:16;
        /** sys_hp_system_peri1_timeout_int_clr : WT; bitpos: [16]; default: 0;
         *  Write 1 to clear timeout interrupt.
         */
        uint32_t sys_hp_system_peri1_timeout_int_clr:1;
        /** sys_hp_system_peri1_timeout_protect_en : R/W; bitpos: [17]; default: 1;
         *  Configures whether or not to enable timeout protection for accessing CPU peripheral
         *  registers.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sys_hp_system_peri1_timeout_protect_en:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} hp_system_hp_system_peri1_timeout_conf_reg_t;

/** Type of sys_hp_system_peri1_timeout_addr register
 *  HP_PERI1_TIMEOUT_ADDR register
 */
typedef union {
    struct {
        /** sys_hp_system_peri1_timeout_addr : RO; bitpos: [31:0]; default: 0;
         *  Represents the address information of abnormal access.
         */
        uint32_t sys_hp_system_peri1_timeout_addr:32;
    };
    uint32_t val;
} hp_system_hp_system_peri1_timeout_addr_reg_t;

/** Type of sys_hp_system_peri1_timeout_uid register
 *  HP_PERI1_TIMEOUT_UID register
 */
typedef union {
    struct {
        /** sys_hp_system_peri1_timeout_uid : RO; bitpos: [7:0]; default: 0;
         *  Represents the master id[5:0] and master permission[6:5] when trigger timeout. This
         *  register will be cleared after the interrupt is cleared.
         */
        uint32_t sys_hp_system_peri1_timeout_uid:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} hp_system_hp_system_peri1_timeout_uid_reg_t;

/** Type of sys_cpu_peri1_timeout_conf register
 *  CPU_PERI1_TIMEOUT configuration register
 */
typedef union {
    struct {
        /** sys_cpu_peri1_timeout_thres : R/W; bitpos: [15:0]; default: 65535;
         *  Configures the timeout threshold for bus access for accessing CPU peripheral
         *  register in the number of clock cycles of the clock domain.
         */
        uint32_t sys_cpu_peri1_timeout_thres:16;
        /** sys_cpu_peri1_timeout_int_clr : WT; bitpos: [16]; default: 0;
         *  Write 1 to clear timeout interrupt.
         */
        uint32_t sys_cpu_peri1_timeout_int_clr:1;
        /** sys_cpu_peri1_timeout_protect_en : R/W; bitpos: [17]; default: 1;
         *  Configures whether or not to enable timeout protection for accessing CPU
         *  peripheral11 registers.
         *  0: Disable
         *  1: Enable
         */
        uint32_t sys_cpu_peri1_timeout_protect_en:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} hp_system_cpu_peri1_timeout_conf_reg_t;

/** Type of sys_cpu_peri1_timeout_addr register
 *  CPU_PERI1_TIMEOUT_ADDR register
 */
typedef union {
    struct {
        /** sys_cpu_peri1_timeout_addr : RO; bitpos: [31:0]; default: 0;
         *  Represents the address information of abnormal access.
         */
        uint32_t sys_cpu_peri1_timeout_addr:32;
    };
    uint32_t val;
} hp_system_cpu_peri1_timeout_addr_reg_t;

/** Type of sys_cpu_peri1_timeout_uid register
 *  CPU_PERI1_TIMEOUT_UID register
 */
typedef union {
    struct {
        /** sys_cpu_peri1_timeout_uid : RO; bitpos: [7:0]; default: 0;
         *  Represents the master id[5:0] and master permission[6:5] when trigger timeout. This
         *  register will be cleared after the interrupt is cleared.
         */
        uint32_t sys_cpu_peri1_timeout_uid:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} hp_system_cpu_peri1_timeout_uid_reg_t;


/** Group: Status Register */
/** Type of sys_sdio_pad_bist_st register
 *  sdio pad bist status register
 */
typedef union {
    struct {
        /** sys_sdio_pad_bist_rcnt : RO; bitpos: [5:0]; default: 0;
         *  Indicate number of clock cycles received for pad bist debug
         */
        uint32_t sys_sdio_pad_bist_rcnt:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} hp_system_sdio_pad_bist_st_reg_t;

/** Type of sys_gmac0_pad_bist_st register
 *  gmac0 pad bist status register
 */
typedef union {
    struct {
        /** sys_gmac0_pad_bist_rcnt : RO; bitpos: [5:0]; default: 0;
         *  Indicate number of clock cycles received for pad bist debug
         */
        uint32_t sys_gmac0_pad_bist_rcnt:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} hp_system_gmac0_pad_bist_st_reg_t;

/** Type of sys_gmac1_pad_bist_st register
 *  gmac1 pad bist status register
 */
typedef union {
    struct {
        /** sys_gmac1_pad_bist_rcnt : RO; bitpos: [5:0]; default: 0;
         *  Indicate number of clock cycles received for pad bist debug
         */
        uint32_t sys_gmac1_pad_bist_rcnt:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} hp_system_gmac1_pad_bist_st_reg_t;

/** Type of sys_cpu_pwait_mode register
 *  Indicate status of core0 and core1 pwait
 */
typedef union {
    struct {
        /** sys_cpu_pwait_mode : RO; bitpos: [1:0]; default: 0;
         *  Indicate status of core0 and core1 pwait
         */
        uint32_t sys_cpu_pwait_mode:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_cpu_pwait_mode_reg_t;


typedef struct {
    volatile hp_system_ver_date_reg_t sys_ver_date;
    volatile hp_system_clk_en_reg_t clk_en;
    uint32_t reserved_008[2];
    volatile hp_system_cpu_int_from_cpu_0_reg_t cpu_int_from_cpu_0;
    volatile hp_system_cpu_int_from_cpu_1_reg_t cpu_int_from_cpu_1;
    volatile hp_system_cpu_int_from_cpu_2_reg_t cpu_int_from_cpu_2;
    volatile hp_system_cpu_int_from_cpu_3_reg_t cpu_int_from_cpu_3;
    uint32_t reserved_020[2];
    volatile hp_system_modem_diag_en_reg_t modem_diag_en;
    uint32_t reserved_02c[4];
    volatile hp_system_tcm_ram_pwr_ctrl0_reg_t tcm_ram_pwr_ctrl0;
    volatile hp_system_rom_pwr_ctrl0_reg_t rom_pwr_ctrl0;
    uint32_t reserved_044[3];
    volatile hp_system_probea_ctrl_reg_t probea_ctrl;
    volatile hp_system_probeb_ctrl_reg_t probeb_ctrl;
    uint32_t reserved_058;
    volatile hp_system_probe_out_reg_t probe_out;
    uint32_t reserved_060;
    volatile hp_system_cpu_corestalled_st_reg_t cpu_corestalled_st;
    uint32_t reserved_068[2];
    volatile hp_system_crypto_ctrl_reg_t crypto_ctrl;
    uint32_t reserved_074[2];
    volatile hp_system_iomux_fpga_debug_reg_t iomux_fpga_debug;
    volatile hp_system_rdn_eco_cs_reg_t sys_rdn_eco_cs;
    uint32_t reserved_084[24];
    volatile hp_system_tcm_rdn_eco_cs_reg_t tcm_rdn_eco_cs;
    volatile hp_system_tcm_rdn_eco_low_reg_t tcm_rdn_eco_low;
    volatile hp_system_tcm_rdn_eco_high_reg_t tcm_rdn_eco_high;
    uint32_t reserved_0f0[8];
    volatile hp_system_cache_pwr_ctrl_reg_t cache_pwr_ctrl;
    volatile hp_system_tcm_data_dump_ctrl_reg_t tcm_data_dump_ctrl;
    volatile hp_system_cpu_waiti_conf_reg_t cpu_waiti_conf;
    volatile hp_system_core_debug_runstall_conf_reg_t sys_core_debug_runstall_conf;
    uint32_t reserved_120;
    volatile hp_system_core_ibus_timeout_conf_reg_t core_ibus_timeout_conf;
    volatile hp_system_core_dbus_timeout_conf_reg_t core_dbus_timeout_conf;
    uint32_t reserved_12c[3];
    volatile hp_system_icm_h2x_cfg_reg_t icm_h2x_cfg;
    uint32_t reserved_13c;
    volatile hp_system_bitscrambler_peri_sel_reg_t bitscrambler_peri_sel;
    uint32_t reserved_144;
    volatile hp_system_gdma_ctrl_reg_t sys_gdma_ctrl;
    uint32_t reserved_14c[3];
    volatile hp_system_vpu_ctrl_reg_t sys_vpu_ctrl;
    uint32_t reserved_15c[5];
    volatile hp_system_design_for_verification0_reg_t design_for_verification0;
    volatile hp_system_design_for_verification1_reg_t design_for_verification1;
    uint32_t reserved_178[4];
    volatile hp_system_ahb2axi_bresp_err_int_raw_reg_t ahb2axi_bresp_err_int_raw;
    volatile hp_system_ahb2axi_bresp_err_int_st_reg_t ahb2axi_bresp_err_int_st;
    volatile hp_system_ahb2axi_bresp_err_int_ena_reg_t ahb2axi_bresp_err_int_ena;
    volatile hp_system_ahb2axi_bresp_err_int_clr_reg_t ahb2axi_bresp_err_int_clr;
    uint32_t reserved_198[2];
    volatile hp_system_core_dmactive_lpcore_reg_t core_dmactive_lpcore;
    volatile hp_system_core_err_resp_dis_reg_t core_err_resp_dis;
    volatile hp_system_core_timeout_int_raw_reg_t core_timeout_int_raw;
    volatile hp_system_core_timeout_int_st_reg_t core_timeout_int_st;
    volatile hp_system_core_timeout_int_ena_reg_t core_timeout_int_ena;
    volatile hp_system_core_timeout_int_clr_reg_t core_timeout_int_clr;
    volatile hp_system_core_pad_wakeup_event_reg_t core_pad_wakeup_event;
    uint32_t reserved_1bc;
    volatile hp_system_twai0_timestamp_l_reg_t twai0_timestamp_l;
    volatile hp_system_twai0_timestamp_h_reg_t twai0_timestamp_h;
    volatile hp_system_twai1_timestamp_l_reg_t twai1_timestamp_l;
    volatile hp_system_twai1_timestamp_h_reg_t twai1_timestamp_h;
    uint32_t reserved_1d0[2];
    volatile hp_system_rng_cfg_reg_t rng_cfg;
    uint32_t reserved_1dc;
    volatile hp_system_peri_mem_clk_force_on_reg_t peri_mem_clk_force_on;
    uint32_t reserved_1e4;
    volatile hp_system_prdyn_st_reg_t sys_prdyn_st;
    volatile hp_system_icm_cfg_reg_t sys_icm_cfg;
    volatile hp_system_uart3_mem_lp_ctrl_reg_t sys_uart3_mem_lp_ctrl;
    uint32_t reserved_1f4[4];
    volatile hp_system_rmt_mem_lp_ctrl_reg_t sys_rmt_mem_lp_ctrl;
    volatile hp_system_ledc0_mem_lp_ctrl_reg_t sys_ledc0_mem_lp_ctrl;
    volatile hp_system_km_mem_lp_ctrl_reg_t sys_km_mem_lp_ctrl;
    volatile hp_system_dma2d_mem_lp_ctrl_reg_t sys_dma2d_mem_lp_ctrl;
    volatile hp_system_axi_pdma_mem_lp_ctrl_reg_t sys_axi_pdma_mem_lp_ctrl;
    volatile hp_system_ecc_mem_lp_ctrl_reg_t sys_ecc_mem_lp_ctrl;
    volatile hp_system_rsa_mem_lp_ctrl_reg_t sys_rsa_mem_lp_ctrl;
    volatile hp_system_bitscram_mem_lp_ctrl_reg_t sys_bitscram_mem_lp_ctrl;
    uint32_t reserved_224;
    volatile hp_system_can0_mem_lp_ctrl_reg_t sys_can0_mem_lp_ctrl;
    uint32_t reserved_22c[2];
    volatile hp_system_mspi_mem_lp_ctrl_reg_t sys_mspi_mem_lp_ctrl;
    uint32_t reserved_238[5];
    volatile hp_system_hpcore_mem_lp_ctrl_reg_t sys_hpcore_mem_lp_ctrl;
    volatile hp_system_rom_mem_lp_ctrl_reg_t sys_rom_mem_lp_ctrl;
    uint32_t reserved_254[2];
    volatile hp_system_l1_cache_mem_lp_ctrl_reg_t sys_l1_cache_mem_lp_ctrl;
    volatile hp_system_kyber_mem_lp_ctrl_reg_t sys_kyber_mem_lp_ctrl;
    volatile hp_system_lcdcam_mem_lp_ctrl_reg_t sys_lcdcam_mem_lp_ctrl;
    volatile hp_system_cpu_peri0_timeout_conf_reg_t sys_cpu_peri0_timeout_conf;
    volatile hp_system_cpu_peri0_timeout_addr_reg_t sys_cpu_peri0_timeout_addr;
    volatile hp_system_cpu_peri0_timeout_uid_reg_t sys_cpu_peri0_timeout_uid;
    volatile hp_system_hp_system_peri0_timeout_conf_reg_t sys_hp_system_peri0_timeout_conf;
    volatile hp_system_hp_system_peri0_timeout_addr_reg_t sys_hp_system_peri0_timeout_addr;
    volatile hp_system_hp_system_peri0_timeout_uid_reg_t sys_hp_system_peri0_timeout_uid;
    volatile hp_system_hp_system_peri1_timeout_conf_reg_t sys_hp_system_peri1_timeout_conf;
    volatile hp_system_hp_system_peri1_timeout_addr_reg_t sys_hp_system_peri1_timeout_addr;
    volatile hp_system_hp_system_peri1_timeout_uid_reg_t sys_hp_system_peri1_timeout_uid;
    volatile hp_system_uart0_mem_lp_ctrl_reg_t sys_uart0_mem_lp_ctrl;
    volatile hp_system_uart1_mem_lp_ctrl_reg_t sys_uart1_mem_lp_ctrl;
    volatile hp_system_uart2_mem_lp_ctrl_reg_t sys_uart2_mem_lp_ctrl;
    volatile hp_system_ledc1_mem_lp_ctrl_reg_t sys_ledc1_mem_lp_ctrl;
    volatile hp_system_ppa_mem_lp_ctrl_reg_t sys_ppa_mem_lp_ctrl;
    volatile hp_system_jpeg_mem_lp_ctrl_reg_t sys_jpeg_mem_lp_ctrl;
    volatile hp_system_can1_mem_lp_ctrl_reg_t sys_can1_mem_lp_ctrl;
    volatile hp_system_cpu_peri1_timeout_conf_reg_t sys_cpu_peri1_timeout_conf;
    volatile hp_system_cpu_peri1_timeout_addr_reg_t sys_cpu_peri1_timeout_addr;
    volatile hp_system_cpu_peri1_timeout_uid_reg_t sys_cpu_peri1_timeout_uid;
    volatile hp_system_cpu_acs_cache_mem_conf_reg_t sys_cpu_acs_cache_mem_conf;
    uint32_t reserved_2b8[10];
    volatile hp_system_tcm_performace_ctrl_reg_t sys_tcm_performace_ctrl;
    volatile hp_system_sdio_pad_bist_cfg_reg_t sys_sdio_pad_bist_cfg;
    volatile hp_system_sdio_pad_bist_int_raw_reg_t sys_sdio_pad_bist_int_raw;
    volatile hp_system_sdio_pad_bist_int_st_reg_t sys_sdio_pad_bist_int_st;
    volatile hp_system_sdio_pad_bist_int_ena_reg_t sys_sdio_pad_bist_int_ena;
    volatile hp_system_sdio_pad_bist_int_clr_reg_t sys_sdio_pad_bist_int_clr;
    volatile hp_system_sdio_pad_bist_st_reg_t sys_sdio_pad_bist_st;
    volatile hp_system_gmac0_pad_bist_cfg_reg_t sys_gmac0_pad_bist_cfg;
    volatile hp_system_gmac0_pad_bist_int_raw_reg_t sys_gmac0_pad_bist_int_raw;
    volatile hp_system_gmac0_pad_bist_int_st_reg_t sys_gmac0_pad_bist_int_st;
    volatile hp_system_gmac0_pad_bist_int_ena_reg_t sys_gmac0_pad_bist_int_ena;
    volatile hp_system_gmac0_pad_bist_int_clr_reg_t sys_gmac0_pad_bist_int_clr;
    volatile hp_system_gmac0_pad_bist_st_reg_t sys_gmac0_pad_bist_st;
    volatile hp_system_gmac1_pad_bist_cfg_reg_t sys_gmac1_pad_bist_cfg;
    volatile hp_system_gmac1_pad_bist_int_raw_reg_t sys_gmac1_pad_bist_int_raw;
    volatile hp_system_gmac1_pad_bist_int_st_reg_t sys_gmac1_pad_bist_int_st;
    volatile hp_system_gmac1_pad_bist_int_ena_reg_t sys_gmac1_pad_bist_int_ena;
    volatile hp_system_gmac1_pad_bist_int_clr_reg_t sys_gmac1_pad_bist_int_clr;
    volatile hp_system_gmac1_pad_bist_st_reg_t sys_gmac1_pad_bist_st;
    uint32_t reserved_32c[3];
    volatile hp_system_cpu_pwait_mode_reg_t sys_cpu_pwait_mode;
} hp_system_dev_t;

extern hp_system_dev_t HP_SYSTEM;

#ifndef __cplusplus
_Static_assert(sizeof(hp_system_dev_t) == 0x33c, "Invalid size of hp_system_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
