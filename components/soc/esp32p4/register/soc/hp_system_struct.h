/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
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
        /** reg_ver_date : R/W; bitpos: [31:0]; default: 539165977;
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


/** Group: HP CACHE CLK CONFIG REG */
/** Type of cache_clk_config register
 *  NA
 */
typedef union {
    struct {
        /** reg_l2_cache_clk_on : R/W; bitpos: [0]; default: 1;
         *  l2 cache clk enable
         */
        uint32_t reg_l2_cache_clk_on:1;
        /** reg_l1_d_cache_clk_on : R/W; bitpos: [1]; default: 1;
         *  l1 dcahce clk enable
         */
        uint32_t reg_l1_d_cache_clk_on:1;
        uint32_t reserved_2:2;
        /** reg_l1_i1_cache_clk_on : R/W; bitpos: [4]; default: 1;
         *  l1 icahce1 clk enable
         */
        uint32_t reg_l1_i1_cache_clk_on:1;
        /** reg_l1_i0_cache_clk_on : R/W; bitpos: [5]; default: 1;
         *  l1 icahce0 clk enable
         */
        uint32_t reg_l1_i0_cache_clk_on:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} hp_system_cache_clk_config_reg_t;


/** Group: HP CACHE RESET CONFIG REG */
/** Type of cache_reset_config register
 *  NA
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** reg_l1_d_cache_reset : R/W; bitpos: [1]; default: 0;
         *  set 1 to reset l1 dcahce
         */
        uint32_t reg_l1_d_cache_reset:1;
        uint32_t reserved_2:2;
        /** reg_l1_i1_cache_reset : R/W; bitpos: [4]; default: 0;
         *  set 1 to reset l1 icahce1
         */
        uint32_t reg_l1_i1_cache_reset:1;
        /** reg_l1_i0_cache_reset : R/W; bitpos: [5]; default: 0;
         *  set 1 to reset l1 icahce0
         */
        uint32_t reg_l1_i0_cache_reset:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} hp_system_cache_reset_config_reg_t;


/** Group: HP SYS DMA ADDR CTRL REG */
/** Type of sys_dma_addr_ctrl register
 *  NA
 */
typedef union {
    struct {
        /** reg_sys_dma_addr_sel : R/W; bitpos: [0]; default: 0;
         *  0 means dma access extmem use 8xxx_xxxx else use 4xxx_xxxx
         */
        uint32_t reg_sys_dma_addr_sel:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_dma_addr_ctrl_reg_t;


/** Group: HP TCM RAM WRR CONFIG REG */
/** Type of tcm_ram_wrr_config register
 *  NA
 */
typedef union {
    struct {
        /** reg_tcm_ram_ibus0_wt : R/W; bitpos: [2:0]; default: 7;
         *  weight value of ibus0
         */
        uint32_t reg_tcm_ram_ibus0_wt:3;
        /** reg_tcm_ram_ibus1_wt : R/W; bitpos: [5:3]; default: 7;
         *  weight value of ibus1
         */
        uint32_t reg_tcm_ram_ibus1_wt:3;
        /** reg_tcm_ram_ibus2_wt : R/W; bitpos: [8:6]; default: 4;
         *  weight value of ibus2
         */
        uint32_t reg_tcm_ram_ibus2_wt:3;
        /** reg_tcm_ram_ibus3_wt : R/W; bitpos: [11:9]; default: 4;
         *  weight value of ibus3
         */
        uint32_t reg_tcm_ram_ibus3_wt:3;
        /** reg_tcm_ram_dbus0_wt : R/W; bitpos: [14:12]; default: 5;
         *  weight value of dbus0
         */
        uint32_t reg_tcm_ram_dbus0_wt:3;
        /** reg_tcm_ram_dbus1_wt : R/W; bitpos: [17:15]; default: 5;
         *  weight value of dbus1
         */
        uint32_t reg_tcm_ram_dbus1_wt:3;
        /** reg_tcm_ram_dbus2_wt : R/W; bitpos: [20:18]; default: 3;
         *  weight value of dbus2
         */
        uint32_t reg_tcm_ram_dbus2_wt:3;
        /** reg_tcm_ram_dbus3_wt : R/W; bitpos: [23:21]; default: 3;
         *  weight value of dbus3
         */
        uint32_t reg_tcm_ram_dbus3_wt:3;
        /** reg_tcm_ram_dma_wt : R/W; bitpos: [26:24]; default: 2;
         *  weight value of dma
         */
        uint32_t reg_tcm_ram_dma_wt:3;
        uint32_t reserved_27:4;
        /** reg_tcm_ram_wrr_high : R/W; bitpos: [31]; default: 1;
         *  enable weighted round robin arbitration
         */
        uint32_t reg_tcm_ram_wrr_high:1;
    };
    uint32_t val;
} hp_system_tcm_ram_wrr_config_reg_t;


/** Group: HP TCM SW PARITY BWE MASK REG */
/** Type of tcm_sw_parity_bwe_mask register
 *  NA
 */
typedef union {
    struct {
        /** reg_tcm_sw_parity_bwe_mask_ctrl : R/W; bitpos: [0]; default: 0;
         *  Set 1 to mask tcm bwe parity code bit
         */
        uint32_t reg_tcm_sw_parity_bwe_mask_ctrl:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_tcm_sw_parity_bwe_mask_reg_t;


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


/** Group: HP L2 ROM PWR CTRL0 REG */
/** Type of l2_rom_pwr_ctrl0 register
 *  NA
 */
typedef union {
    struct {
        /** reg_l2_rom_clk_force_on : R/W; bitpos: [0]; default: 0;
         *  l2_rom clk gating force on
         */
        uint32_t reg_l2_rom_clk_force_on:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_l2_rom_pwr_ctrl0_reg_t;


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


/** Group: HP L2 MEM RAM PWR CTRL0 REG */
/** Type of l2_mem_ram_pwr_ctrl0 register
 *  NA
 */
typedef union {
    struct {
        /** reg_l2_mem_clk_force_on : R/W; bitpos: [0]; default: 0;
         *  l2ram clk_gating force on
         */
        uint32_t reg_l2_mem_clk_force_on:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_l2_mem_ram_pwr_ctrl0_reg_t;


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


/** Group: HP GPIO O HOLD CTRL0 REG */
/** Type of gpio_o_hold_ctrl0 register
 *  NA
 */
typedef union {
    struct {
        /** reg_gpio_0_hold_low : R/W; bitpos: [31:0]; default: 0;
         *  hold control for gpio47~16
         */
        uint32_t reg_gpio_0_hold_low:32;
    };
    uint32_t val;
} hp_system_gpio_o_hold_ctrl0_reg_t;


/** Group: HP GPIO O HOLD CTRL1 REG */
/** Type of gpio_o_hold_ctrl1 register
 *  NA
 */
typedef union {
    struct {
        /** reg_gpio_0_hold_high : R/W; bitpos: [8:0]; default: 0;
         *  hold control for gpio56~48
         */
        uint32_t reg_gpio_0_hold_high:9;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_system_gpio_o_hold_ctrl1_reg_t;


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


/** Group: HP CACHE APB POSTW EN REG */
/** Type of cache_apb_postw_en register
 *  NA
 */
typedef union {
    struct {
        /** reg_cache_apb_postw_en : R/W; bitpos: [0]; default: 0;
         *  cache apb register interface post write enable, 1 will speed up write, but will
         *  take some time to update value to register
         */
        uint32_t reg_cache_apb_postw_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_cache_apb_postw_en_reg_t;


/** Group: HP L2 MEM SUBSIZE REG */
/** Type of l2_mem_subsize register
 *  NA
 */
typedef union {
    struct {
        /** reg_l2_mem_sub_blksize : R/W; bitpos: [1:0]; default: 0;
         *  l2mem sub block size 00=>32 01=>64 10=>128 11=>256
         */
        uint32_t reg_l2_mem_sub_blksize:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_l2_mem_subsize_reg_t;


/** Group: HP L2 MEM INT RAW REG */
/** Type of l2_mem_int_raw register
 *  NA
 */
typedef union {
    struct {
        /** reg_l2_mem_ecc_err_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  intr triggered when two bit error detected and corrected from ecc
         */
        uint32_t reg_l2_mem_ecc_err_int_raw:1;
        /** reg_l2_mem_exceed_addr_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  intr triggered when access addr exceeds 0xff9ffff at bypass mode or exceeds
         *  0xff80000 at l2cache 128kb mode or exceeds 0xff60000 at l2cache 256kb mode
         */
        uint32_t reg_l2_mem_exceed_addr_int_raw:1;
        /** reg_l2_mem_err_resp_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  intr triggered when err response occurs
         */
        uint32_t reg_l2_mem_err_resp_int_raw:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_system_l2_mem_int_raw_reg_t;


/** Group: HP L2 MEM INT ST REG */
/** Type of l2_mem_int_st register
 *  NA
 */
typedef union {
    struct {
        /** reg_l2_mem_ecc_err_int_st : RO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t reg_l2_mem_ecc_err_int_st:1;
        /** reg_l2_mem_exceed_addr_int_st : RO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t reg_l2_mem_exceed_addr_int_st:1;
        /** reg_l2_mem_err_resp_int_st : RO; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t reg_l2_mem_err_resp_int_st:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_system_l2_mem_int_st_reg_t;


/** Group: HP L2 MEM INT ENA REG */
/** Type of l2_mem_int_ena register
 *  NA
 */
typedef union {
    struct {
        /** reg_l2_mem_ecc_err_int_ena : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t reg_l2_mem_ecc_err_int_ena:1;
        /** reg_l2_mem_exceed_addr_int_ena : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t reg_l2_mem_exceed_addr_int_ena:1;
        /** reg_l2_mem_err_resp_int_ena : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t reg_l2_mem_err_resp_int_ena:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_system_l2_mem_int_ena_reg_t;


/** Group: HP L2 MEM INT CLR REG */
/** Type of l2_mem_int_clr register
 *  NA
 */
typedef union {
    struct {
        /** reg_l2_mem_ecc_err_int_clr : WT; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t reg_l2_mem_ecc_err_int_clr:1;
        /** reg_l2_mem_exceed_addr_int_clr : WT; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t reg_l2_mem_exceed_addr_int_clr:1;
        /** reg_l2_mem_err_resp_int_clr : WT; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t reg_l2_mem_err_resp_int_clr:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_system_l2_mem_int_clr_reg_t;


/** Group: HP L2 MEM L2 RAM ECC REG */
/** Type of l2_mem_l2_ram_ecc register
 *  NA
 */
typedef union {
    struct {
        /** reg_l2_ram_unit0_ecc_en : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t reg_l2_ram_unit0_ecc_en:1;
        /** reg_l2_ram_unit1_ecc_en : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t reg_l2_ram_unit1_ecc_en:1;
        /** reg_l2_ram_unit2_ecc_en : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t reg_l2_ram_unit2_ecc_en:1;
        /** reg_l2_ram_unit3_ecc_en : R/W; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t reg_l2_ram_unit3_ecc_en:1;
        /** reg_l2_ram_unit4_ecc_en : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t reg_l2_ram_unit4_ecc_en:1;
        /** reg_l2_ram_unit5_ecc_en : R/W; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t reg_l2_ram_unit5_ecc_en:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} hp_system_l2_mem_l2_ram_ecc_reg_t;


/** Group: HP L2 MEM INT RECORD0 REG */
/** Type of l2_mem_int_record0 register
 *  NA
 */
typedef union {
    struct {
        /** reg_l2_mem_exceed_addr_int_addr : RO; bitpos: [20:0]; default: 0;
         *  NA
         */
        uint32_t reg_l2_mem_exceed_addr_int_addr:21;
        /** reg_l2_mem_exceed_addr_int_we : RO; bitpos: [21]; default: 0;
         *  NA
         */
        uint32_t reg_l2_mem_exceed_addr_int_we:1;
        /** reg_l2_mem_exceed_addr_int_master : RO; bitpos: [24:22]; default: 0;
         *  NA
         */
        uint32_t reg_l2_mem_exceed_addr_int_master:3;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} hp_system_l2_mem_int_record0_reg_t;


/** Group: HP L2 MEM INT RECORD1 REG */
/** Type of l2_mem_int_record1 register
 *  NA
 */
typedef union {
    struct {
        /** reg_l2_mem_ecc_err_int_addr : RO; bitpos: [14:0]; default: 0;
         *  NA
         */
        uint32_t reg_l2_mem_ecc_err_int_addr:15;
        /** reg_l2_mem_ecc_one_bit_err : RO; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t reg_l2_mem_ecc_one_bit_err:1;
        /** reg_l2_mem_ecc_two_bit_err : RO; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t reg_l2_mem_ecc_two_bit_err:1;
        /** reg_l2_mem_ecc_err_bit : RO; bitpos: [25:17]; default: 0;
         *  NA
         */
        uint32_t reg_l2_mem_ecc_err_bit:9;
        /** reg_l2_cache_err_bank : RO; bitpos: [26]; default: 0;
         *  NA
         */
        uint32_t reg_l2_cache_err_bank:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} hp_system_l2_mem_int_record1_reg_t;


/** Group: HP L2 MEM L2 CACHE ECC REG */
/** Type of l2_mem_l2_cache_ecc register
 *  NA
 */
typedef union {
    struct {
        /** reg_l2_cache_ecc_en : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t reg_l2_cache_ecc_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_l2_mem_l2_cache_ecc_reg_t;


/** Group: HP L1CACHE BUS0 ID REG */
/** Type of l1cache_bus0_id register
 *  NA
 */
typedef union {
    struct {
        /** reg_l1_cache_bus0_id : R/W; bitpos: [3:0]; default: 0;
         *  NA
         */
        uint32_t reg_l1_cache_bus0_id:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_system_l1cache_bus0_id_reg_t;


/** Group: HP L1CACHE BUS1 ID REG */
/** Type of l1cache_bus1_id register
 *  NA
 */
typedef union {
    struct {
        /** reg_l1_cache_bus1_id : R/W; bitpos: [3:0]; default: 0;
         *  NA
         */
        uint32_t reg_l1_cache_bus1_id:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_system_l1cache_bus1_id_reg_t;


/** Group: HP L2 MEM RDN ECO CS REG */
/** Type of l2_mem_rdn_eco_cs register
 *  NA
 */
typedef union {
    struct {
        /** reg_l2_mem_rdn_eco_en : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t reg_l2_mem_rdn_eco_en:1;
        /** reg_l2_mem_rdn_eco_result : RO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t reg_l2_mem_rdn_eco_result:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_l2_mem_rdn_eco_cs_reg_t;


/** Group: HP L2 MEM RDN ECO LOW REG */
/** Type of l2_mem_rdn_eco_low register
 *  NA
 */
typedef union {
    struct {
        /** reg_l2_mem_rdn_eco_low : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t reg_l2_mem_rdn_eco_low:32;
    };
    uint32_t val;
} hp_system_l2_mem_rdn_eco_low_reg_t;


/** Group: HP L2 MEM RDN ECO HIGH REG */
/** Type of l2_mem_rdn_eco_high register
 *  NA
 */
typedef union {
    struct {
        /** reg_l2_mem_rdn_eco_high : R/W; bitpos: [31:0]; default: 4294967295;
         *  NA
         */
        uint32_t reg_l2_mem_rdn_eco_high:32;
    };
    uint32_t val;
} hp_system_l2_mem_rdn_eco_high_reg_t;


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


/** Group: HP GPIO DEAD HOLD CTRL REG */
/** Type of gpio_ded_hold_ctrl register
 *  NA
 */
typedef union {
    struct {
        /** reg_gpio_ded_hold : R/W; bitpos: [25:0]; default: 0;
         *  hold control for gpio63~56
         */
        uint32_t reg_gpio_ded_hold:26;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} hp_system_gpio_ded_hold_ctrl_reg_t;


/** Group: HP L2 MEM SW ECC BWE MASK REG */
/** Type of l2_mem_sw_ecc_bwe_mask register
 *  NA
 */
typedef union {
    struct {
        /** reg_l2_mem_sw_ecc_bwe_mask_ctrl : R/W; bitpos: [0]; default: 0;
         *  Set 1 to mask bwe hamming code bit
         */
        uint32_t reg_l2_mem_sw_ecc_bwe_mask_ctrl:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_l2_mem_sw_ecc_bwe_mask_reg_t;


/** Group: HP USB20OTG MEM CTRL REG */
/** Type of usb20otg_mem_ctrl register
 *  NA
 */
typedef union {
    struct {
        /** reg_usb20_mem_clk_force_on : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t reg_usb20_mem_clk_force_on:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_usb20otg_mem_ctrl_reg_t;


/** Group: configure_register */
/** Type of tcm_int_raw register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** tcm_parity_err_int_raw : R/WTC/SS; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t tcm_parity_err_int_raw:1;
    };
    uint32_t val;
} hp_system_tcm_int_raw_reg_t;

/** Type of tcm_int_st register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** tcm_parity_err_int_st : RO; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t tcm_parity_err_int_st:1;
    };
    uint32_t val;
} hp_system_tcm_int_st_reg_t;

/** Type of tcm_int_ena register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** tcm_parity_err_int_ena : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t tcm_parity_err_int_ena:1;
    };
    uint32_t val;
} hp_system_tcm_int_ena_reg_t;

/** Type of tcm_int_clr register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** tcm_parity_err_int_clr : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t tcm_parity_err_int_clr:1;
    };
    uint32_t val;
} hp_system_tcm_int_clr_reg_t;

/** Type of core_ahb_timeout register
 *  need_des
 */
typedef union {
    struct {
        /** core_ahb_timeout_en : R/W; bitpos: [0]; default: 1;
         *  set this field to 1 to enable hp core0&1 ahb timeout handle
         */
        uint32_t core_ahb_timeout_en:1;
        /** core_ahb_timeout_thres : R/W; bitpos: [16:1]; default: 65535;
         *  This field used to set hp core0&1 ahb bus timeout  threshold
         */
        uint32_t core_ahb_timeout_thres:16;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} hp_system_core_ahb_timeout_reg_t;

/** Type of core_ibus_timeout register
 *  need_des
 */
typedef union {
    struct {
        /** core_ibus_timeout_en : R/W; bitpos: [0]; default: 1;
         *  set this field to 1 to enable hp core0&1 ibus timeout handle
         */
        uint32_t core_ibus_timeout_en:1;
        /** core_ibus_timeout_thres : R/W; bitpos: [16:1]; default: 65535;
         *  This field used to set hp core0&1 ibus timeout  threshold
         */
        uint32_t core_ibus_timeout_thres:16;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} hp_system_core_ibus_timeout_reg_t;

/** Type of core_dbus_timeout register
 *  need_des
 */
typedef union {
    struct {
        /** core_dbus_timeout_en : R/W; bitpos: [0]; default: 1;
         *  set this field to 1 to enable hp core0&1 dbus timeout handle
         */
        uint32_t core_dbus_timeout_en:1;
        /** core_dbus_timeout_thres : R/W; bitpos: [16:1]; default: 65535;
         *  This field used to set hp core0&1 dbus timeout  threshold
         */
        uint32_t core_dbus_timeout_thres:16;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} hp_system_core_dbus_timeout_reg_t;

/** Type of icm_cpu_h2x_cfg register
 *  need_des
 */
typedef union {
    struct {
        /** cpu_icm_h2x_post_wr_en : R/W; bitpos: [0]; default: 1;
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
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_system_icm_cpu_h2x_cfg_reg_t;

/** Type of bitscrambler_peri_sel register
 *  Bitscrambler Peri Sel
 */
typedef union {
    struct {
        /** bitscrambler_peri_rx_sel : R/W; bitpos: [3:0]; default: 15;
         *  Set  this field to sel peri with DMA RX interface to connect with bitscrambler: 4'h0
         *  : lcd_cam, 4'h1: gpspi2, 4'h2: gpspi3, 4'h3: parl_io, 4'h4: aes, 4'h5: sha, 4'h6:
         *  adc, 4'h7: i2s0, 4'h8: i2s1, 4'h9: i2s2, 4'ha: i3c_mst, 4'hb: uhci0, 4'hc: RMT,
         *  else : none
         */
        uint32_t bitscrambler_peri_rx_sel:4;
        /** bitscrambler_peri_tx_sel : R/W; bitpos: [7:4]; default: 15;
         *  Set  this field to sel peri with DMA TX interface to connect with bitscrambler: 4'h0
         *  : lcd_cam, 4'h1: gpspi2, 4'h2: gpspi3, 4'h3: parl_io, 4'h4: aes, 4'h5: sha, 4'h6:
         *  adc, 4'h7: i2s0, 4'h8: i2s1, 4'h9: i2s2, 4'ha: i3c_mst, 4'hb: uhci0, 4'hc: RMT,
         *  else : none
         */
        uint32_t bitscrambler_peri_tx_sel:4;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} hp_system_bitscrambler_peri_sel_reg_t;


/** Group: hp_system_TCM_PARITY_INT_RECORD_REG */
/** Type of tcm_parity_int_record register
 *  need_des
 */
typedef union {
    struct {
        /** tcm_parity_err_int_addr : RO; bitpos: [12:0]; default: 0;
         *  hp tcm_parity_err_addr
         */
        uint32_t tcm_parity_err_int_addr:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} hp_system_tcm_parity_int_record_reg_t;


/** Group: HP L1 CACHE PWR CTRL REG */
/** Type of l1_cache_pwr_ctrl register
 *  NA
 */
typedef union {
    struct {
        /** reg_l1_cache_mem_fo : R/W; bitpos: [5:0]; default: 0;
         *  need_des
         */
        uint32_t reg_l1_cache_mem_fo:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} hp_system_l1_cache_pwr_ctrl_reg_t;


/** Group: HP L2 CACHE PWR CTRL REG */
/** Type of l2_cache_pwr_ctrl register
 *  NA
 */
typedef union {
    struct {
        /** reg_l2_cache_mem_fo : R/W; bitpos: [1:0]; default: 0;
         *  need_des
         */
        uint32_t reg_l2_cache_mem_fo:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_l2_cache_pwr_ctrl_reg_t;


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

/** Type of rsa_pd_ctrl register
 *  rsa pd ctrl register
 */
typedef union {
    struct {
        /** rsa_mem_force_pd : R/W; bitpos: [0]; default: 0;
         *  Set this bit to power down rsa internal memory.
         */
        uint32_t rsa_mem_force_pd:1;
        /** rsa_mem_force_pu : R/W; bitpos: [1]; default: 1;
         *  Set this bit to force power up rsa internal memory
         */
        uint32_t rsa_mem_force_pu:1;
        /** rsa_mem_pd : R/W; bitpos: [2]; default: 0;
         *  Set this bit to force power down rsa internal memory.
         */
        uint32_t rsa_mem_pd:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_system_rsa_pd_ctrl_reg_t;

/** Type of ecc_pd_ctrl register
 *  ecc pd ctrl register
 */
typedef union {
    struct {
        /** ecc_mem_force_pd : R/W; bitpos: [0]; default: 0;
         *  Set this bit to power down ecc internal memory.
         */
        uint32_t ecc_mem_force_pd:1;
        /** ecc_mem_force_pu : R/W; bitpos: [1]; default: 1;
         *  Set this bit to force power up ecc internal memory
         */
        uint32_t ecc_mem_force_pu:1;
        /** ecc_mem_pd : R/W; bitpos: [2]; default: 0;
         *  Set this bit to force power down ecc internal memory.
         */
        uint32_t ecc_mem_pd:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_system_ecc_pd_ctrl_reg_t;

/** Type of rng_cfg register
 *  rng cfg register
 */
typedef union {
    struct {
        /** rng_sample_enable : R/W; bitpos: [0]; default: 0;
         *  enable rng sample chain
         */
        uint32_t rng_sample_enable:1;
        uint32_t reserved_1:15;
        /** rng_chain_clk_div_num : R/W; bitpos: [23:16]; default: 0;
         *  chain clk div num to pad for debug
         */
        uint32_t rng_chain_clk_div_num:8;
        /** rng_sample_cnt : RO; bitpos: [31:24]; default: 0;
         *  debug rng sample cnt
         */
        uint32_t rng_sample_cnt:8;
    };
    uint32_t val;
} hp_system_rng_cfg_reg_t;

/** Type of uart_pd_ctrl register
 *  ecc pd ctrl register
 */
typedef union {
    struct {
        /** uart_mem_force_pd : R/W; bitpos: [0]; default: 0;
         *  Set this bit to power down hp uart internal memory.
         */
        uint32_t uart_mem_force_pd:1;
        /** uart_mem_force_pu : R/W; bitpos: [1]; default: 1;
         *  Set this bit to force power up hp uart  internal memory
         */
        uint32_t uart_mem_force_pu:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_uart_pd_ctrl_reg_t;

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


/** Group: HP PERI1 APB POSTW EN REG */
/** Type of peri1_apb_postw_en register
 *  NA
 */
typedef union {
    struct {
        /** peri1_apb_postw_en : R/W; bitpos: [0]; default: 0;
         *  hp_system_peri1 apb register interface post write enable, 1 will speed up write, but will
         *  take some time to update value to register
         */
        uint32_t peri1_apb_postw_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_peri1_apb_postw_en_reg_t;


/** Group: APB Sync Register */
/** Type of sys_apb_sync_postw_en register
 *  N/A
 */
typedef union {
    struct {
        /** sys_gmac_apb_postw_en : R/W; bitpos: [0]; default: 0;
         *  N/A
         */
        uint32_t sys_gmac_apb_postw_en:1;
        /** sys_dsi_host_apb_postw_en : R/W; bitpos: [1]; default: 0;
         *  N/A
         */
        uint32_t sys_dsi_host_apb_postw_en:1;
        /** sys_csi_host_apb_sync_postw_en : R/W; bitpos: [2]; default: 0;
         *  N/A
         */
        uint32_t sys_csi_host_apb_sync_postw_en:1;
        /** sys_csi_host_apb_async_postw_en : R/W; bitpos: [3]; default: 0;
         *  N/A
         */
        uint32_t sys_csi_host_apb_async_postw_en:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_system_apb_sync_postw_en_reg_t;


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


/** Group: GMAC Control Register */
/** Type of sys_gmac_ctrl0 register
 *  N/A
 */
typedef union {
    struct {
        /** sys_ptp_pps : RO; bitpos: [0]; default: 0;
         *  N/A
         */
        uint32_t sys_ptp_pps:1;
        /** sys_sbd_flowctrl : R/W; bitpos: [1]; default: 0;
         *  N/A
         */
        uint32_t sys_sbd_flowctrl:1;
        /** sys_phy_intf_sel : R/W; bitpos: [4:2]; default: 0;
         *  N/A
         */
        uint32_t sys_phy_intf_sel:3;
        /** sys_gmac_mem_clk_force_on : R/W; bitpos: [5]; default: 0;
         *  N/A
         */
        uint32_t sys_gmac_mem_clk_force_on:1;
        /** sys_gmac_rst_clk_tx_n : RO; bitpos: [6]; default: 0;
         *  N/A
         */
        uint32_t sys_gmac_rst_clk_tx_n:1;
        /** sys_gmac_rst_clk_rx_n : RO; bitpos: [7]; default: 0;
         *  N/A
         */
        uint32_t sys_gmac_rst_clk_rx_n:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} hp_system_gmac_ctrl0_reg_t;

/** Type of sys_gmac_ctrl1 register
 *  N/A
 */
typedef union {
    struct {
        /** sys_ptp_timestamp_l : RO; bitpos: [31:0]; default: 0;
         *  N/A
         */
        uint32_t sys_ptp_timestamp_l:32;
    };
    uint32_t val;
} hp_system_gmac_ctrl1_reg_t;

/** Type of sys_gmac_ctrl2 register
 *  N/A
 */
typedef union {
    struct {
        /** sys_ptp_timestamp_h : RO; bitpos: [31:0]; default: 0;
         *  N/A
         */
        uint32_t sys_ptp_timestamp_h:32;
    };
    uint32_t val;
} hp_system_gmac_ctrl2_reg_t;


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


/** Group: USB OTG20 Control Register */
/** Type of sys_usbotg20_ctrl register
 *  N/A
 */
typedef union {
    struct {
        /** sys_otg_phy_test_done : RO; bitpos: [0]; default: 0;
         *  N/A
         */
        uint32_t sys_otg_phy_test_done:1;
        /** sys_usb_mem_aux_ctrl : R/W; bitpos: [14:1]; default: 4896;
         *  N/A
         */
        uint32_t sys_usb_mem_aux_ctrl:14;
        /** sys_phy_suspendm : R/W; bitpos: [15]; default: 0;
         *  N/A
         */
        uint32_t sys_phy_suspendm:1;
        /** sys_phy_suspend_force_en : R/W; bitpos: [16]; default: 0;
         *  N/A
         */
        uint32_t sys_phy_suspend_force_en:1;
        /** sys_phy_rstn : R/W; bitpos: [17]; default: 1;
         *  N/A
         */
        uint32_t sys_phy_rstn:1;
        /** sys_phy_reset_force_en : R/W; bitpos: [18]; default: 0;
         *  N/A
         */
        uint32_t sys_phy_reset_force_en:1;
        /** sys_phy_pll_force_en : R/W; bitpos: [19]; default: 0;
         *  N/A
         */
        uint32_t sys_phy_pll_force_en:1;
        /** sys_phy_pll_en : R/W; bitpos: [20]; default: 0;
         *  N/A
         */
        uint32_t sys_phy_pll_en:1;
        /** sys_otg_suspendm : R/W; bitpos: [21]; default: 0;
         *  N/A
         */
        uint32_t sys_otg_suspendm:1;
        /** sys_otg_phy_txbitstuff_en : R/W; bitpos: [22]; default: 0;
         *  N/A
         */
        uint32_t sys_otg_phy_txbitstuff_en:1;
        /** sys_otg_phy_refclk_mode : R/W; bitpos: [23]; default: 1;
         *  N/A
         */
        uint32_t sys_otg_phy_refclk_mode:1;
        /** sys_otg_phy_bisten : R/W; bitpos: [24]; default: 0;
         *  N/A
         */
        uint32_t sys_otg_phy_bisten:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} hp_system_usbotg20_ctrl_reg_t;


/** Group: hp_system_TCM_ERR_RESP_CTRL_REG */
/** Type of tcm_err_resp_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** tcm_err_resp_en : R/W; bitpos: [0]; default: 0;
         *  Set 1 to turn on tcm error response
         */
        uint32_t tcm_err_resp_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_tcm_err_resp_ctrl_reg_t;


/** Group: HP L2 MEM REFRESH REG */
/** Type of l2_mem_refresh register
 *  NA
 */
typedef union {
    struct {
        /** reg_l2_mem_unit0_refersh_en : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t reg_l2_mem_unit0_refersh_en:1;
        /** reg_l2_mem_unit1_refersh_en : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t reg_l2_mem_unit1_refersh_en:1;
        /** reg_l2_mem_unit2_refersh_en : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t reg_l2_mem_unit2_refersh_en:1;
        /** reg_l2_mem_unit3_refersh_en : R/W; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t reg_l2_mem_unit3_refersh_en:1;
        /** reg_l2_mem_unit4_refersh_en : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t reg_l2_mem_unit4_refersh_en:1;
        /** reg_l2_mem_unit5_refersh_en : R/W; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t reg_l2_mem_unit5_refersh_en:1;
        /** reg_l2_mem_refersh_cnt_reset : R/W; bitpos: [6]; default: 1;
         *  Set 1 to reset l2mem_refresh_cnt
         */
        uint32_t reg_l2_mem_refersh_cnt_reset:1;
        /** reg_l2_mem_unit0_refresh_done : RO; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t reg_l2_mem_unit0_refresh_done:1;
        /** reg_l2_mem_unit1_refresh_done : RO; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t reg_l2_mem_unit1_refresh_done:1;
        /** reg_l2_mem_unit2_refresh_done : RO; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t reg_l2_mem_unit2_refresh_done:1;
        /** reg_l2_mem_unit3_refresh_done : RO; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t reg_l2_mem_unit3_refresh_done:1;
        /** reg_l2_mem_unit4_refresh_done : RO; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t reg_l2_mem_unit4_refresh_done:1;
        /** reg_l2_mem_unit5_refresh_done : RO; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t reg_l2_mem_unit5_refresh_done:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} hp_system_l2_mem_refresh_reg_t;


/** Group: HP TCM INIT REG */
/** Type of tcm_init register
 *  NA
 */
typedef union {
    struct {
        /** reg_tcm_init_en : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t reg_tcm_init_en:1;
        /** reg_tcm_init_cnt_reset : R/W; bitpos: [1]; default: 1;
         *  Set 1 to reset tcm init cnt
         */
        uint32_t reg_tcm_init_cnt_reset:1;
        /** reg_tcm_init_done : RO; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t reg_tcm_init_done:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_system_tcm_init_reg_t;


/** Group: hp_system_TCM_PARITY_CHECK_CTRL_REG */
/** Type of tcm_parity_check_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** tcm_parity_check_en : R/W; bitpos: [0]; default: 0;
         *  Set 1 to turn on tcm parity check
         */
        uint32_t tcm_parity_check_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_tcm_parity_check_ctrl_reg_t;


/** Group: hp_system_DESIGN_FOR_VERIFICATION0 */
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


/** Group: hp_system_DESIGN_FOR_VERIFICATION1 */
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


/** Group: hp_system_PSRAM_FLASH_ADDR_INTERCHANGE */
/** Type of psram_flash_addr_interchange register
 *  need_des
 */
typedef union {
    struct {
        /** psram_flash_addr_interchange_cpu : R/W; bitpos: [0]; default: 0;
         *  Set 1 to enable addr interchange between psram and flash in axi matrix when hp cpu
         *  access through cache
         */
        uint32_t psram_flash_addr_interchange_cpu:1;
        /** psram_flash_addr_interchange_dma : R/W; bitpos: [1]; default: 0;
         *  Set 1 to enable addr interchange between psram and flash in axi matrix when dma
         *  device access, lp core access and hp core access through ahb
         */
        uint32_t psram_flash_addr_interchange_dma:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_psram_flash_addr_interchange_reg_t;


/** Group: Interrupt Registers */
/** Type of ahb2axi_bresp_err_int_raw register
 *  NA
 */
typedef union {
    struct {
        /** cpu_icm_h2x_bresp_err_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  the raw interrupt status of bresp error,  triggered when if  bresp err occurs  in
         *  post write mode in ahb2axi.
         */
        uint32_t cpu_icm_h2x_bresp_err_int_raw:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_ahb2axi_bresp_err_int_raw_reg_t;

/** Type of ahb2axi_bresp_err_int_st register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
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
        uint32_t reserved_0:31;
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
        uint32_t reserved_0:31;
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
        /** core0_ahb_timeout_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  the raw interrupt status of hp core0  ahb timeout
         */
        uint32_t core0_ahb_timeout_int_raw:1;
        /** core1_ahb_timeout_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  the raw interrupt status of hp core1  ahb timeout
         */
        uint32_t core1_ahb_timeout_int_raw:1;
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
        /** core0_ahb_timeout_int_st : RO; bitpos: [0]; default: 0;
         *  the masked interrupt status of hp core0  ahb timeout
         */
        uint32_t core0_ahb_timeout_int_st:1;
        /** core1_ahb_timeout_int_st : RO; bitpos: [1]; default: 0;
         *  the masked interrupt status of hp core1  ahb timeout
         */
        uint32_t core1_ahb_timeout_int_st:1;
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
        /** core0_ahb_timeout_int_ena : R/W; bitpos: [0]; default: 0;
         *  Write 1 to enable hp_system_core0_ahb_timeout int
         */
        uint32_t core0_ahb_timeout_int_ena:1;
        /** core1_ahb_timeout_int_ena : R/W; bitpos: [1]; default: 0;
         *  Write 1 to enable hp_system_core1_ahb_timeout int
         */
        uint32_t core1_ahb_timeout_int_ena:1;
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
        /** core0_ahb_timeout_int_clr : WT; bitpos: [0]; default: 0;
         *  Write 1 to clear hp_system_core0_ahb_timeout int
         */
        uint32_t core0_ahb_timeout_int_clr:1;
        /** core1_ahb_timeout_int_clr : WT; bitpos: [1]; default: 0;
         *  Write 1 to clear hp_system_core1_ahb_timeout int
         */
        uint32_t core1_ahb_timeout_int_clr:1;
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


/** Group: hp_system_L2_MEM_ERR_RESP_CTRL_REG */
/** Type of l2_mem_err_resp_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** l2_mem_err_resp_en : R/W; bitpos: [0]; default: 0;
         *  Set 1 to turn on l2mem error response
         */
        uint32_t l2_mem_err_resp_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_system_l2_mem_err_resp_ctrl_reg_t;


/** Group: hp_system_L2_MEM_AHB_BUFFER_CTRL_REG */
/** Type of l2_mem_ahb_buffer_ctrl register
 *  need_des
 */
typedef union {
    struct {
        /** l2_mem_ahb_wrbuffer_en : R/W; bitpos: [0]; default: 0;
         *  Set 1 to turn on l2mem ahb wr buffer
         */
        uint32_t l2_mem_ahb_wrbuffer_en:1;
        /** l2_mem_ahb_rdbuffer_en : R/W; bitpos: [1]; default: 0;
         *  Set 1 to turn on l2mem ahb rd buffer
         */
        uint32_t l2_mem_ahb_rdbuffer_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_system_l2_mem_ahb_buffer_ctrl_reg_t;


/** Group: hp_system_CORE_DMACTIVE_LPCORE_REG */
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


/** Group: HP GPIO O HYS CTRL0 REG */
/** Type of gpio_o_hys_ctrl0 register
 *  NA
 */
typedef union {
    struct {
        /** reg_gpio_0_hys_low : R/W; bitpos: [31:0]; default: 0;
         *  hys control for gpio47~16
         */
        uint32_t reg_gpio_0_hys_low:32;
    };
    uint32_t val;
} hp_system_gpio_o_hys_ctrl0_reg_t;


/** Group: HP GPIO O HYS CTRL1 REG */
/** Type of gpio_o_hys_ctrl1 register
 *  NA
 */
typedef union {
    struct {
        /** reg_gpio_0_hys_high : R/W; bitpos: [8:0]; default: 0;
         *  hys control for gpio56~48
         */
        uint32_t reg_gpio_0_hys_high:9;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_system_gpio_o_hys_ctrl1_reg_t;


typedef struct hp_system_dev_t {
    volatile hp_system_ver_date_reg_t sys_ver_date;
    volatile hp_system_clk_en_reg_t clk_en;
    uint32_t reserved_008[2];
    volatile hp_system_cpu_int_from_cpu_0_reg_t cpu_int_from_cpu_0;
    volatile hp_system_cpu_int_from_cpu_1_reg_t cpu_int_from_cpu_1;
    volatile hp_system_cpu_int_from_cpu_2_reg_t cpu_int_from_cpu_2;
    volatile hp_system_cpu_int_from_cpu_3_reg_t cpu_int_from_cpu_3;
    volatile hp_system_cache_clk_config_reg_t cache_clk_config;
    volatile hp_system_cache_reset_config_reg_t cache_reset_config;
    uint32_t reserved_028;
    volatile hp_system_dma_addr_ctrl_reg_t sys_dma_addr_ctrl;
    uint32_t reserved_030;
    volatile hp_system_tcm_ram_wrr_config_reg_t tcm_ram_wrr_config;
    volatile hp_system_tcm_sw_parity_bwe_mask_reg_t tcm_sw_parity_bwe_mask;
    volatile hp_system_tcm_ram_pwr_ctrl0_reg_t tcm_ram_pwr_ctrl0;
    volatile hp_system_l2_rom_pwr_ctrl0_reg_t l2_rom_pwr_ctrl0;
    uint32_t reserved_044[3];
    volatile hp_system_probea_ctrl_reg_t probea_ctrl;
    volatile hp_system_probeb_ctrl_reg_t probeb_ctrl;
    uint32_t reserved_058;
    volatile hp_system_probe_out_reg_t probe_out;
    volatile hp_system_l2_mem_ram_pwr_ctrl0_reg_t l2_mem_ram_pwr_ctrl0;
    volatile hp_system_cpu_corestalled_st_reg_t cpu_corestalled_st;
    uint32_t reserved_068[2];
    volatile hp_system_crypto_ctrl_reg_t crypto_ctrl;
    volatile hp_system_gpio_o_hold_ctrl0_reg_t gpio_o_hold_ctrl0;
    volatile hp_system_gpio_o_hold_ctrl1_reg_t gpio_o_hold_ctrl1;
    volatile hp_system_rdn_eco_cs_reg_t sys_rdn_eco_cs;
    volatile hp_system_cache_apb_postw_en_reg_t cache_apb_postw_en;
    volatile hp_system_l2_mem_subsize_reg_t l2_mem_subsize;
    uint32_t reserved_088[5];
    volatile hp_system_l2_mem_int_raw_reg_t l2_mem_int_raw;
    volatile hp_system_l2_mem_int_st_reg_t l2_mem_int_st;
    volatile hp_system_l2_mem_int_ena_reg_t l2_mem_int_ena;
    volatile hp_system_l2_mem_int_clr_reg_t l2_mem_int_clr;
    volatile hp_system_l2_mem_l2_ram_ecc_reg_t l2_mem_l2_ram_ecc;
    volatile hp_system_l2_mem_int_record0_reg_t l2_mem_int_record0;
    volatile hp_system_l2_mem_int_record1_reg_t l2_mem_int_record1;
    uint32_t reserved_0b8[3];
    volatile hp_system_l2_mem_l2_cache_ecc_reg_t l2_mem_l2_cache_ecc;
    volatile hp_system_l1cache_bus0_id_reg_t l1cache_bus0_id;
    volatile hp_system_l1cache_bus1_id_reg_t l1cache_bus1_id;
    uint32_t reserved_0d0[2];
    volatile hp_system_l2_mem_rdn_eco_cs_reg_t l2_mem_rdn_eco_cs;
    volatile hp_system_l2_mem_rdn_eco_low_reg_t l2_mem_rdn_eco_low;
    volatile hp_system_l2_mem_rdn_eco_high_reg_t l2_mem_rdn_eco_high;
    volatile hp_system_tcm_rdn_eco_cs_reg_t tcm_rdn_eco_cs;
    volatile hp_system_tcm_rdn_eco_low_reg_t tcm_rdn_eco_low;
    volatile hp_system_tcm_rdn_eco_high_reg_t tcm_rdn_eco_high;
    volatile hp_system_gpio_ded_hold_ctrl_reg_t gpio_ded_hold_ctrl;
    volatile hp_system_l2_mem_sw_ecc_bwe_mask_reg_t l2_mem_sw_ecc_bwe_mask;
    volatile hp_system_usb20otg_mem_ctrl_reg_t usb20otg_mem_ctrl;
    volatile hp_system_tcm_int_raw_reg_t tcm_int_raw;
    volatile hp_system_tcm_int_st_reg_t tcm_int_st;
    volatile hp_system_tcm_int_ena_reg_t tcm_int_ena;
    volatile hp_system_tcm_int_clr_reg_t tcm_int_clr;
    volatile hp_system_tcm_parity_int_record_reg_t tcm_parity_int_record;
    volatile hp_system_l1_cache_pwr_ctrl_reg_t l1_cache_pwr_ctrl;
    volatile hp_system_l2_cache_pwr_ctrl_reg_t l2_cache_pwr_ctrl;
    volatile hp_system_cpu_waiti_conf_reg_t cpu_waiti_conf;
    volatile hp_system_core_debug_runstall_conf_reg_t sys_core_debug_runstall_conf;
    volatile hp_system_core_ahb_timeout_reg_t core_ahb_timeout;
    volatile hp_system_core_ibus_timeout_reg_t core_ibus_timeout;
    volatile hp_system_core_dbus_timeout_reg_t core_dbus_timeout;
    uint32_t reserved_12c[3];
    volatile hp_system_icm_cpu_h2x_cfg_reg_t icm_cpu_h2x_cfg;
    volatile hp_system_peri1_apb_postw_en_reg_t peri1_apb_postw_en;
    volatile hp_system_bitscrambler_peri_sel_reg_t bitscrambler_peri_sel;
    volatile hp_system_apb_sync_postw_en_reg_t sys_apb_sync_postw_en;
    volatile hp_system_gdma_ctrl_reg_t sys_gdma_ctrl;
    volatile hp_system_gmac_ctrl0_reg_t sys_gmac_ctrl0;
    volatile hp_system_gmac_ctrl1_reg_t sys_gmac_ctrl1;
    volatile hp_system_gmac_ctrl2_reg_t sys_gmac_ctrl2;
    volatile hp_system_vpu_ctrl_reg_t sys_vpu_ctrl;
    volatile hp_system_usbotg20_ctrl_reg_t sys_usbotg20_ctrl;
    volatile hp_system_tcm_err_resp_ctrl_reg_t tcm_err_resp_ctrl;
    volatile hp_system_l2_mem_refresh_reg_t l2_mem_refresh;
    volatile hp_system_tcm_init_reg_t tcm_init;
    volatile hp_system_tcm_parity_check_ctrl_reg_t tcm_parity_check_ctrl;
    volatile hp_system_design_for_verification0_reg_t design_for_verification0;
    volatile hp_system_design_for_verification1_reg_t design_for_verification1;
    uint32_t reserved_178[2];
    volatile hp_system_psram_flash_addr_interchange_reg_t psram_flash_addr_interchange;
    uint32_t reserved_184;
    volatile hp_system_ahb2axi_bresp_err_int_raw_reg_t ahb2axi_bresp_err_int_raw;
    volatile hp_system_ahb2axi_bresp_err_int_st_reg_t ahb2axi_bresp_err_int_st;
    volatile hp_system_ahb2axi_bresp_err_int_ena_reg_t ahb2axi_bresp_err_int_ena;
    volatile hp_system_ahb2axi_bresp_err_int_clr_reg_t ahb2axi_bresp_err_int_clr;
    volatile hp_system_l2_mem_err_resp_ctrl_reg_t l2_mem_err_resp_ctrl;
    volatile hp_system_l2_mem_ahb_buffer_ctrl_reg_t l2_mem_ahb_buffer_ctrl;
    volatile hp_system_core_dmactive_lpcore_reg_t core_dmactive_lpcore;
    volatile hp_system_core_err_resp_dis_reg_t core_err_resp_dis;
    volatile hp_system_core_timeout_int_raw_reg_t core_timeout_int_raw;
    volatile hp_system_core_timeout_int_st_reg_t core_timeout_int_st;
    volatile hp_system_core_timeout_int_ena_reg_t core_timeout_int_ena;
    volatile hp_system_core_timeout_int_clr_reg_t core_timeout_int_clr;
    uint32_t reserved_1b8[2];
    volatile hp_system_gpio_o_hys_ctrl0_reg_t gpio_o_hys_ctrl0;
    volatile hp_system_gpio_o_hys_ctrl1_reg_t gpio_o_hys_ctrl1;
    uint32_t reserved_1c8[2];
    volatile hp_system_rsa_pd_ctrl_reg_t rsa_pd_ctrl;
    volatile hp_system_ecc_pd_ctrl_reg_t ecc_pd_ctrl;
    volatile hp_system_rng_cfg_reg_t rng_cfg;
    volatile hp_system_uart_pd_ctrl_reg_t uart_pd_ctrl;
    volatile hp_system_peri_mem_clk_force_on_reg_t peri_mem_clk_force_on;
} hp_system_dev_t;

extern hp_system_dev_t HP_SYSTEM;

#ifndef __cplusplus
_Static_assert(sizeof(hp_system_dev_t) == 0x1e4, "Invalid size of hp_system_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
