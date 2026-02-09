/**
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: root_clk_sel */
/** Type of soc_clk_sel register
 *  need_des
 */
typedef union {
    struct {
        /** reg_soc_clk_sel : R/W; bitpos: [1:0]; default: 0;
         *  Configures to select the clock source of HP_ROOT_CLK.
         *  0 (default): XTAL_CLK
         *  1: PLL_F300M_CLK
         *  2: RC_FAST_CLK
         */
        uint32_t reg_soc_clk_sel:2;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_sys_clkrst_soc_clk_sel_reg_t;


/** Group: cpu_freq_ctrl */
/** Type of cpu_freq_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_cpu_clk_div_num : R/W; bitpos: [7:0]; default: 0;
         *  need_des
         */
        uint32_t reg_cpu_clk_div_num:8;
        /** reg_cpu_clk_div_numerator : R/W; bitpos: [10:8]; default: 0;
         *  need_des
         */
        uint32_t reg_cpu_clk_div_numerator:3;
        /** reg_cpu_clk_div_denominator : R/W; bitpos: [13:11]; default: 0;
         *  need_des
         */
        uint32_t reg_cpu_clk_div_denominator:3;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} hp_sys_clkrst_cpu_freq_ctrl0_reg_t;


/** Group: mem_freq_ctrl */
/** Type of mem_freq_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_mem_clk_div_num : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_mem_clk_div_num:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_sys_clkrst_mem_freq_ctrl0_reg_t;


/** Group: sys_freq_ctrl */
/** Type of sys_freq_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_sys_clk_div_num : R/W; bitpos: [7:0]; default: 2;
         *  need_des
         */
        uint32_t reg_sys_clk_div_num:8;
        /** reg_sys_clk_div_numerator : R/W; bitpos: [10:8]; default: 0;
         *  need_des
         */
        uint32_t reg_sys_clk_div_numerator:3;
        /** reg_sys_clk_div_denominator : R/W; bitpos: [13:11]; default: 0;
         *  need_des
         */
        uint32_t reg_sys_clk_div_denominator:3;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} hp_sys_clkrst_sys_freq_ctrl0_reg_t;


/** Group: apb_freq_ctrl */
/** Type of apb_freq_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_apb_clk_div_num : R/W; bitpos: [7:0]; default: 1;
         *  need_des
         */
        uint32_t reg_apb_clk_div_num:8;
        /** reg_apb_clk_div_numerator : R/W; bitpos: [10:8]; default: 0;
         *  need_des
         */
        uint32_t reg_apb_clk_div_numerator:3;
        /** reg_apb_clk_div_denominator : R/W; bitpos: [13:11]; default: 0;
         *  need_des
         */
        uint32_t reg_apb_clk_div_denominator:3;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} hp_sys_clkrst_apb_freq_ctrl0_reg_t;


/** Group: root_clk_ctrl */
/** Type of root_clk_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_soc_clk_update : R/W/WTC; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_soc_clk_update:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_sys_clkrst_root_clk_ctrl0_reg_t;


/** Group: cpu_wfi_delay_ctrl */
/** Type of cpu_wfi_delay_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_cpuicm_delay_num : R/W; bitpos: [3:0]; default: 0;
         *  need_des
         */
        uint32_t reg_cpuicm_delay_num:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_sys_clkrst_cpu_wfi_delay_ctrl0_reg_t;


/** Group: hpcore0_ctrl */
/** Type of hpcore0_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_core0_clic_clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_core0_clic_clk_en:1;
        /** reg_core0_cpu_clk_en : R/W; bitpos: [1]; default: 1;
         *  need_des
         */
        uint32_t reg_core0_cpu_clk_en:1;
        /** reg_core0_global_rst_en : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_core0_global_rst_en:1;
        /** reg_core0_force_norst : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_core0_force_norst:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_sys_clkrst_hpcore0_ctrl0_reg_t;


/** Group: hpcore1_ctrl */
/** Type of hpcore1_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_core1_clic_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_core1_clic_clk_en:1;
        /** reg_core1_cpu_clk_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_core1_cpu_clk_en:1;
        /** reg_core1_global_rst_en : R/W; bitpos: [2]; default: 1;
         *  need_des
         */
        uint32_t reg_core1_global_rst_en:1;
        /** reg_core1_force_norst : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_core1_force_norst:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_sys_clkrst_hpcore1_ctrl0_reg_t;


/** Group: trace_ctrl */
/** Type of trace_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_trace_cpu_clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_trace_cpu_clk_en:1;
        /** reg_trace_sys_clk_en : R/W; bitpos: [1]; default: 1;
         *  need_des
         */
        uint32_t reg_trace_sys_clk_en:1;
        /** reg_trace_cpu_clk_force_on : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_trace_cpu_clk_force_on:1;
        /** reg_trace_sys_clk_force_on : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_trace_sys_clk_force_on:1;
        /** reg_trace0_rst_en : R/W; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t reg_trace0_rst_en:1;
        /** reg_trace1_rst_en : R/W; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t reg_trace1_rst_en:1;
        /** reg_trace0_force_norst : R/W; bitpos: [6]; default: 0;
         *  need_des
         */
        uint32_t reg_trace0_force_norst:1;
        /** reg_trace1_force_norst : R/W; bitpos: [7]; default: 0;
         *  need_des
         */
        uint32_t reg_trace1_force_norst:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} hp_sys_clkrst_trace_ctrl0_reg_t;


/** Group: tcm_ctrl */
/** Type of tcm_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_tcm_cpu_clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_tcm_cpu_clk_en:1;
        /** reg_tcm_cpu_clk_force_on : R/W; bitpos: [1]; default: 1;
         *  need_des
         */
        uint32_t reg_tcm_cpu_clk_force_on:1;
        /** reg_tcm_rst_en : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_tcm_rst_en:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_sys_clkrst_tcm_ctrl0_reg_t;


/** Group: tcmmon_ctrl */
/** Type of tcmmon_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_tcmmon_cpu_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_tcmmon_cpu_clk_en:1;
        /** reg_tcmmon_sys_clk_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_tcmmon_sys_clk_en:1;
        /** reg_tcmmon_rst_en : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_tcmmon_rst_en:1;
        /** reg_tcmmon_force_norst : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_tcmmon_force_norst:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_sys_clkrst_tcmmon_ctrl0_reg_t;


/** Group: psrammon_ctrl */
/** Type of psrammon_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_psram_mon_sys_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_psram_mon_sys_clk_en:1;
        /** reg_psram_mon_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_psram_mon_rst_en:1;
        /** reg_psram_mon_force_norst : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_psram_mon_force_norst:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_sys_clkrst_psrammon_ctrl0_reg_t;


/** Group: rom_ctrl */
/** Type of rom_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_rom_mem_clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_rom_mem_clk_en:1;
        /** reg_rom_mem_clk_force_on : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_rom_mem_clk_force_on:1;
        /** reg_rom_rst_en : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_rom_rst_en:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_sys_clkrst_rom_ctrl0_reg_t;


/** Group: cache_ctrl */
/** Type of cache_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_cpu_acache_cpu_clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_cpu_acache_cpu_clk_en:1;
        /** reg_cpu_acache_cpu_clk_force_on : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_cpu_acache_cpu_clk_force_on:1;
        /** reg_cpu_acache_rst_en : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_cpu_acache_rst_en:1;
        /** reg_rom_acache_mem_clk_en : R/W; bitpos: [3]; default: 1;
         *  need_des
         */
        uint32_t reg_rom_acache_mem_clk_en:1;
        /** reg_rom_acache_mem_clk_force_on : R/W; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t reg_rom_acache_mem_clk_force_on:1;
        /** reg_rom_acache_rst_en : R/W; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t reg_rom_acache_rst_en:1;
        /** reg_cpu_cache_cpu_clk_en : R/W; bitpos: [6]; default: 1;
         *  need_des
         */
        uint32_t reg_cpu_cache_cpu_clk_en:1;
        /** reg_cpu_cache_cpu_clk_force_on : R/W; bitpos: [7]; default: 0;
         *  need_des
         */
        uint32_t reg_cpu_cache_cpu_clk_force_on:1;
        /** reg_cpu_cache_rst_en : R/W; bitpos: [8]; default: 0;
         *  need_des
         */
        uint32_t reg_cpu_cache_rst_en:1;
        /** reg_mspi_cache_sys_clk_en : R/W; bitpos: [9]; default: 1;
         *  need_des
         */
        uint32_t reg_mspi_cache_sys_clk_en:1;
        /** reg_mspi_cache_sys_clk_force_on : R/W; bitpos: [10]; default: 0;
         *  need_des
         */
        uint32_t reg_mspi_cache_sys_clk_force_on:1;
        /** reg_mspi_cache_rst_en : R/W; bitpos: [11]; default: 0;
         *  need_des
         */
        uint32_t reg_mspi_cache_rst_en:1;
        /** reg_dcache_clk_en : R/W; bitpos: [12]; default: 1;
         *  need_des
         */
        uint32_t reg_dcache_clk_en:1;
        /** reg_dcache_rst_en : R/W; bitpos: [13]; default: 0;
         *  need_des
         */
        uint32_t reg_dcache_rst_en:1;
        /** reg_icache0_clk_en : R/W; bitpos: [14]; default: 1;
         *  need_des
         */
        uint32_t reg_icache0_clk_en:1;
        /** reg_icache0_rst_en : R/W; bitpos: [15]; default: 0;
         *  need_des
         */
        uint32_t reg_icache0_rst_en:1;
        /** reg_icache1_clk_en : R/W; bitpos: [16]; default: 1;
         *  need_des
         */
        uint32_t reg_icache1_clk_en:1;
        /** reg_icache1_rst_en : R/W; bitpos: [17]; default: 0;
         *  need_des
         */
        uint32_t reg_icache1_rst_en:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} hp_sys_clkrst_cache_ctrl0_reg_t;


/** Group: icm_ctrl */
/** Type of icm_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_icm_cpu_clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_icm_cpu_clk_en:1;
        /** reg_icm_mem_clk_en : R/W; bitpos: [1]; default: 1;
         *  need_des
         */
        uint32_t reg_icm_mem_clk_en:1;
        /** reg_icm_sys_clk_en : R/W; bitpos: [2]; default: 1;
         *  need_des
         */
        uint32_t reg_icm_sys_clk_en:1;
        /** reg_icm_apb_clk_en : R/W; bitpos: [3]; default: 1;
         *  need_des
         */
        uint32_t reg_icm_apb_clk_en:1;
        /** reg_cpuicm_gated_clk_force_on : R/W; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t reg_cpuicm_gated_clk_force_on:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} hp_sys_clkrst_icm_ctrl0_reg_t;


/** Group: modem_ctrl */
/** Type of modem_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_modem_clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_modem_clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_sys_clkrst_modem_ctrl0_reg_t;


/** Group: misc_ctrl */
/** Type of misc_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_misc_cpu_clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_misc_cpu_clk_en:1;
        /** reg_misc_sys_clk_en : R/W; bitpos: [1]; default: 1;
         *  need_des
         */
        uint32_t reg_misc_sys_clk_en:1;
        /** reg_sysreg_apb_clk_en : R/W; bitpos: [2]; default: 1;
         *  need_des
         */
        uint32_t reg_sysreg_apb_clk_en:1;
        /** reg_hp_clkrst_apb_clk_en : R/W; bitpos: [3]; default: 1;
         *  need_des
         */
        uint32_t reg_hp_clkrst_apb_clk_en:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_sys_clkrst_misc_ctrl0_reg_t;


/** Group: busmon_ctrl */
/** Type of busmon_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_busmon_core0_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_busmon_core0_clk_en:1;
        /** reg_busmon_core0_clk_force_on : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_busmon_core0_clk_force_on:1;
        /** reg_busmon_core0_rst_en : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_busmon_core0_rst_en:1;
        /** reg_busmon_core0_force_norst : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_busmon_core0_force_norst:1;
        /** reg_busmon_core1_clk_en : R/W; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t reg_busmon_core1_clk_en:1;
        /** reg_busmon_core1_clk_force_on : R/W; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t reg_busmon_core1_clk_force_on:1;
        /** reg_busmon_core1_rst_en : R/W; bitpos: [6]; default: 0;
         *  need_des
         */
        uint32_t reg_busmon_core1_rst_en:1;
        /** reg_busmon_core1_force_norst : R/W; bitpos: [7]; default: 0;
         *  need_des
         */
        uint32_t reg_busmon_core1_force_norst:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} hp_sys_clkrst_busmon_ctrl0_reg_t;


/** Group: remove_tmp */
/** Type of remove_tmp0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_gdma_cpu_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_gdma_cpu_clk_en:1;
        /** reg_vpu_cpu_clk_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_vpu_cpu_clk_en:1;
        /** reg_vpu_sys_clk_en : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_vpu_sys_clk_en:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_sys_clkrst_remove_tmp0_reg_t;


/** Group: pvt0_ctrl */
/** Type of pvt0_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_pvt0_sys_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_pvt0_sys_clk_en:1;
        /** reg_pvt0_top_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_pvt0_top_rst_en:1;
        /** reg_pvt0_apb_rst_en : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_pvt0_apb_rst_en:1;
        /** reg_pvt0_clk_div_num : R/W; bitpos: [10:3]; default: 0;
         *  need_des
         */
        uint32_t reg_pvt0_clk_div_num:8;
        /** reg_pvt0_clk_en : R/W; bitpos: [11]; default: 0;
         *  need_des
         */
        uint32_t reg_pvt0_clk_en:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} hp_sys_clkrst_pvt0_ctrl0_reg_t;


/** Group: pvt0_peri_ctrl */
/** Type of pvt0_peri_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_pvt0_peri_group1_rst_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_pvt0_peri_group1_rst_en:1;
        /** reg_pvt0_peri_group2_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_pvt0_peri_group2_rst_en:1;
        /** reg_pvt0_peri_group3_rst_en : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_pvt0_peri_group3_rst_en:1;
        /** reg_pvt0_peri_group4_rst_en : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_pvt0_peri_group4_rst_en:1;
        /** reg_pvt0_peri_group1_clk_en : R/W; bitpos: [4]; default: 0;
         *  TOP power domain pvt clk en
         */
        uint32_t reg_pvt0_peri_group1_clk_en:1;
        /** reg_pvt0_peri_group2_clk_en : R/W; bitpos: [5]; default: 0;
         *  TOP power domain pvt clk en
         */
        uint32_t reg_pvt0_peri_group2_clk_en:1;
        /** reg_pvt0_peri_group3_clk_en : R/W; bitpos: [6]; default: 0;
         *  modem power domain pvt clk en
         */
        uint32_t reg_pvt0_peri_group3_clk_en:1;
        /** reg_pvt0_peri_group4_clk_en : R/W; bitpos: [7]; default: 0;
         *  modem power domain pvt clk en
         */
        uint32_t reg_pvt0_peri_group4_clk_en:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} hp_sys_clkrst_pvt0_peri_ctrl0_reg_t;


/** Group: crypto_ctrl */
/** Type of crypto_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_crypto_sys_clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_crypto_sys_clk_en:1;
        /** reg_crypto_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_crypto_rst_en:1;
        /** reg_crypto_sec_clk_en : R/W; bitpos: [2]; default: 1;
         *  need_des
         */
        uint32_t reg_crypto_sec_clk_en:1;
        /** reg_crypto_sec_rst_en : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_crypto_sec_rst_en:1;
        /** reg_crypto_aes_clk_en : R/W; bitpos: [4]; default: 1;
         *  need_des
         */
        uint32_t reg_crypto_aes_clk_en:1;
        /** reg_crypto_aes_rst_en : R/W; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t reg_crypto_aes_rst_en:1;
        /** reg_crypto_sha_clk_en : R/W; bitpos: [6]; default: 1;
         *  need_des
         */
        uint32_t reg_crypto_sha_clk_en:1;
        /** reg_crypto_sha_rst_en : R/W; bitpos: [7]; default: 0;
         *  need_des
         */
        uint32_t reg_crypto_sha_rst_en:1;
        /** reg_crypto_rsa_clk_en : R/W; bitpos: [8]; default: 1;
         *  need_des
         */
        uint32_t reg_crypto_rsa_clk_en:1;
        /** reg_crypto_rsa_rst_en : R/W; bitpos: [9]; default: 0;
         *  need_des
         */
        uint32_t reg_crypto_rsa_rst_en:1;
        /** reg_crypto_ds_clk_en : R/W; bitpos: [10]; default: 1;
         *  need_des
         */
        uint32_t reg_crypto_ds_clk_en:1;
        /** reg_crypto_ds_rst_en : R/W; bitpos: [11]; default: 0;
         *  need_des
         */
        uint32_t reg_crypto_ds_rst_en:1;
        /** reg_crypto_ecc_clk_en : R/W; bitpos: [12]; default: 1;
         *  need_des
         */
        uint32_t reg_crypto_ecc_clk_en:1;
        /** reg_crypto_ecc_rst_en : R/W; bitpos: [13]; default: 0;
         *  need_des
         */
        uint32_t reg_crypto_ecc_rst_en:1;
        /** reg_crypto_hmac_clk_en : R/W; bitpos: [14]; default: 1;
         *  need_des
         */
        uint32_t reg_crypto_hmac_clk_en:1;
        /** reg_crypto_hmac_rst_en : R/W; bitpos: [15]; default: 0;
         *  need_des
         */
        uint32_t reg_crypto_hmac_rst_en:1;
        /** reg_crypto_ecdsa_clk_en : R/W; bitpos: [16]; default: 1;
         *  need_des
         */
        uint32_t reg_crypto_ecdsa_clk_en:1;
        /** reg_crypto_ecdsa_rst_en : R/W; bitpos: [17]; default: 0;
         *  need_des
         */
        uint32_t reg_crypto_ecdsa_rst_en:1;
        uint32_t reserved_18:2;
        /** reg_crypto_clk_src_sel : R/W; bitpos: [21:20]; default: 0;
         *  need_des
         */
        uint32_t reg_crypto_clk_src_sel:2;
        /** reg_crypto_rma_clk_en : R/W; bitpos: [22]; default: 1;
         *  need_des
         */
        uint32_t reg_crypto_rma_clk_en:1;
        /** reg_crypto_rma_rst_en : R/W; bitpos: [23]; default: 0;
         *  need_des
         */
        uint32_t reg_crypto_rma_rst_en:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} hp_sys_clkrst_crypto_ctrl0_reg_t;


/** Group: key_manager_ctrl */
/** Type of key_manager_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_key_manager_sys_clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_key_manager_sys_clk_en:1;
        /** reg_crypto_km_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_crypto_km_rst_en:1;
        /** reg_crypto_km_clk_en : R/W; bitpos: [2]; default: 1;
         *  need_des
         */
        uint32_t reg_crypto_km_clk_en:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_sys_clkrst_key_manager_ctrl0_reg_t;


/** Group: dpa_ctrl */
/** Type of dpa_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_sec_dpa_level : R/W; bitpos: [1:0]; default: 0;
         *  need_des
         */
        uint32_t reg_sec_dpa_level:2;
        /** reg_sec_dpa_cfg_sel : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_sec_dpa_cfg_sel:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_sys_clkrst_dpa_ctrl0_reg_t;


/** Group: flash_ctrl */
/** Type of flash_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_flash_sys_clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_flash_sys_clk_en:1;
        /** reg_flash_axi_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_flash_axi_rst_en:1;
        /** reg_flash_apb_rst_en : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_flash_apb_rst_en:1;
        /** reg_flash_axi_force_norst : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_flash_axi_force_norst:1;
        /** reg_flash_apb_force_norst : R/W; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t reg_flash_apb_force_norst:1;
        /** reg_flash_clk_src_sel : R/W; bitpos: [6:5]; default: 0;
         *  need_des
         */
        uint32_t reg_flash_clk_src_sel:2;
        /** reg_flash_pll_clk_en : R/W; bitpos: [7]; default: 1;
         *  need_des
         */
        uint32_t reg_flash_pll_clk_en:1;
        /** reg_flash_core_clk_en : R/W; bitpos: [8]; default: 1;
         *  need_des
         */
        uint32_t reg_flash_core_clk_en:1;
        /** reg_flash_core_clk_div_num : R/W; bitpos: [16:9]; default: 3;
         *  need_des
         */
        uint32_t reg_flash_core_clk_div_num:8;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} hp_sys_clkrst_flash_ctrl0_reg_t;


/** Group: psram_ctrl */
/** Type of psram_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_psram_sys_clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_psram_sys_clk_en:1;
        /** reg_psram_axi_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_psram_axi_rst_en:1;
        /** reg_psram_apb_rst_en : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_psram_apb_rst_en:1;
        /** reg_psram_axi_force_norst : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_psram_axi_force_norst:1;
        /** reg_psram_apb_force_norst : R/W; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t reg_psram_apb_force_norst:1;
        /** reg_psram_clk_src_sel : R/W; bitpos: [6:5]; default: 0;
         *  need_des
         */
        uint32_t reg_psram_clk_src_sel:2;
        /** reg_psram_pll_clk_en : R/W; bitpos: [7]; default: 1;
         *  need_des
         */
        uint32_t reg_psram_pll_clk_en:1;
        /** reg_psram_core_clk_en : R/W; bitpos: [8]; default: 1;
         *  need_des
         */
        uint32_t reg_psram_core_clk_en:1;
        /** reg_psram_core_clk_div_num : R/W; bitpos: [16:9]; default: 0;
         *  need_des
         */
        uint32_t reg_psram_core_clk_div_num:8;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} hp_sys_clkrst_psram_ctrl0_reg_t;


/** Group: gpspi2_ctrl */
/** Type of gpspi2_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_gpspi2_sys_clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_gpspi2_sys_clk_en:1;
        /** reg_gpspi2_apb_clk_en : R/W; bitpos: [1]; default: 1;
         *  need_des
         */
        uint32_t reg_gpspi2_apb_clk_en:1;
        /** reg_gpspi2_rst_en : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_gpspi2_rst_en:1;
        /** reg_gpspi2_force_norst : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_gpspi2_force_norst:1;
        /** reg_gpspi2_clk_src_sel : R/W; bitpos: [5:4]; default: 0;
         *  need_des
         */
        uint32_t reg_gpspi2_clk_src_sel:2;
        /** reg_gpspi2_hs_clk_en : R/W; bitpos: [6]; default: 1;
         *  need_des
         */
        uint32_t reg_gpspi2_hs_clk_en:1;
        /** reg_gpspi2_hs_clk_div_num : R/W; bitpos: [14:7]; default: 0;
         *  need_des
         */
        uint32_t reg_gpspi2_hs_clk_div_num:8;
        /** reg_gpspi2_mst_clk_div_num : R/W; bitpos: [22:15]; default: 0;
         *  need_des
         */
        uint32_t reg_gpspi2_mst_clk_div_num:8;
        /** reg_gpspi2_mst_clk_en : R/W; bitpos: [23]; default: 1;
         *  need_des
         */
        uint32_t reg_gpspi2_mst_clk_en:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} hp_sys_clkrst_gpspi2_ctrl0_reg_t;


/** Group: gpspi3_ctrl */
/** Type of gpspi3_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_gpspi3_sys_clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_gpspi3_sys_clk_en:1;
        /** reg_gpspi3_apb_clk_en : R/W; bitpos: [1]; default: 1;
         *  need_des
         */
        uint32_t reg_gpspi3_apb_clk_en:1;
        /** reg_gpspi3_rst_en : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_gpspi3_rst_en:1;
        /** reg_gpspi3_force_norst : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_gpspi3_force_norst:1;
        /** reg_gpspi3_clk_src_sel : R/W; bitpos: [5:4]; default: 0;
         *  need_des
         */
        uint32_t reg_gpspi3_clk_src_sel:2;
        /** reg_gpspi3_hs_clk_en : R/W; bitpos: [6]; default: 1;
         *  need_des
         */
        uint32_t reg_gpspi3_hs_clk_en:1;
        /** reg_gpspi3_hs_clk_div_num : R/W; bitpos: [14:7]; default: 0;
         *  need_des
         */
        uint32_t reg_gpspi3_hs_clk_div_num:8;
        /** reg_gpspi3_mst_clk_div_num : R/W; bitpos: [22:15]; default: 0;
         *  need_des
         */
        uint32_t reg_gpspi3_mst_clk_div_num:8;
        /** reg_gpspi3_mst_clk_en : R/W; bitpos: [23]; default: 1;
         *  need_des
         */
        uint32_t reg_gpspi3_mst_clk_en:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} hp_sys_clkrst_gpspi3_ctrl0_reg_t;


/** Group: gdma_ctrl */
/** Type of gdma_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_gdma_sys_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_gdma_sys_clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_sys_clkrst_gdma_ctrl0_reg_t;


/** Group: axi_pdma_ctrl */
/** Type of axi_pdma_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_axi_pdma_sys_clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_axi_pdma_sys_clk_en:1;
        /** reg_axi_pdma_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_axi_pdma_rst_en:1;
        /** reg_axi_pdma_force_norst : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_axi_pdma_force_norst:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_sys_clkrst_axi_pdma_ctrl0_reg_t;


/** Group: ahb_pdma_ctrl */
/** Type of ahb_pdma_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_ahb_pdma_sys_clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_ahb_pdma_sys_clk_en:1;
        /** reg_ahb_pdma_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_ahb_pdma_rst_en:1;
        /** reg_ahb_pdma_force_norst : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_ahb_pdma_force_norst:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_sys_clkrst_ahb_pdma_ctrl0_reg_t;


/** Group: regdma_ctrl */
/** Type of regdma_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_regdma_sys_clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_regdma_sys_clk_en:1;
        /** reg_regdma_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_regdma_rst_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_sys_clkrst_regdma_ctrl0_reg_t;


/** Group: uhci_ctrl */
/** Type of uhci_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_uhci_sys_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_uhci_sys_clk_en:1;
        /** reg_uhci_apb_clk_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_uhci_apb_clk_en:1;
        /** reg_uhci_rst_en : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_uhci_rst_en:1;
        /** reg_uhci_force_norst : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_uhci_force_norst:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_sys_clkrst_uhci_ctrl0_reg_t;


/** Group: uart0_ctrl */
/** Type of uart0_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_uart0_sys_clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_uart0_sys_clk_en:1;
        /** reg_uart0_apb_clk_en : R/W; bitpos: [1]; default: 1;
         *  need_des
         */
        uint32_t reg_uart0_apb_clk_en:1;
        /** reg_uart0_core_rst_en : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_uart0_core_rst_en:1;
        /** reg_uart0_apb_rst_en : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_uart0_apb_rst_en:1;
        /** reg_uart0_force_norst : R/W; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t reg_uart0_force_norst:1;
        /** reg_uart0_clk_src_sel : R/W; bitpos: [6:5]; default: 0;
         *  need_des
         */
        uint32_t reg_uart0_clk_src_sel:2;
        /** reg_uart0_clk_en : R/W; bitpos: [7]; default: 1;
         *  need_des
         */
        uint32_t reg_uart0_clk_en:1;
        /** reg_uart0_sclk_div_num : R/W; bitpos: [15:8]; default: 0;
         *  need_des
         */
        uint32_t reg_uart0_sclk_div_num:8;
        /** reg_uart0_sclk_div_numerator : R/W; bitpos: [23:16]; default: 0;
         *  need_des
         */
        uint32_t reg_uart0_sclk_div_numerator:8;
        /** reg_uart0_sclk_div_denominator : R/W; bitpos: [31:24]; default: 0;
         *  need_des
         */
        uint32_t reg_uart0_sclk_div_denominator:8;
    };
    uint32_t val;
} hp_sys_clkrst_uart0_ctrl0_reg_t;


/** Group: uart1_ctrl */
/** Type of uart1_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_uart1_sys_clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_uart1_sys_clk_en:1;
        /** reg_uart1_apb_clk_en : R/W; bitpos: [1]; default: 1;
         *  need_des
         */
        uint32_t reg_uart1_apb_clk_en:1;
        /** reg_uart1_core_rst_en : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_uart1_core_rst_en:1;
        /** reg_uart1_apb_rst_en : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_uart1_apb_rst_en:1;
        /** reg_uart1_force_norst : R/W; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t reg_uart1_force_norst:1;
        /** reg_uart1_clk_src_sel : R/W; bitpos: [6:5]; default: 0;
         *  need_des
         */
        uint32_t reg_uart1_clk_src_sel:2;
        /** reg_uart1_clk_en : R/W; bitpos: [7]; default: 1;
         *  need_des
         */
        uint32_t reg_uart1_clk_en:1;
        /** reg_uart1_sclk_div_num : R/W; bitpos: [15:8]; default: 0;
         *  need_des
         */
        uint32_t reg_uart1_sclk_div_num:8;
        /** reg_uart1_sclk_div_numerator : R/W; bitpos: [23:16]; default: 0;
         *  need_des
         */
        uint32_t reg_uart1_sclk_div_numerator:8;
        /** reg_uart1_sclk_div_denominator : R/W; bitpos: [31:24]; default: 0;
         *  need_des
         */
        uint32_t reg_uart1_sclk_div_denominator:8;
    };
    uint32_t val;
} hp_sys_clkrst_uart1_ctrl0_reg_t;


/** Group: uart2_ctrl */
/** Type of uart2_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_uart2_sys_clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_uart2_sys_clk_en:1;
        /** reg_uart2_apb_clk_en : R/W; bitpos: [1]; default: 1;
         *  need_des
         */
        uint32_t reg_uart2_apb_clk_en:1;
        /** reg_uart2_core_rst_en : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_uart2_core_rst_en:1;
        /** reg_uart2_apb_rst_en : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_uart2_apb_rst_en:1;
        /** reg_uart2_force_norst : R/W; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t reg_uart2_force_norst:1;
        /** reg_uart2_clk_src_sel : R/W; bitpos: [6:5]; default: 0;
         *  need_des
         */
        uint32_t reg_uart2_clk_src_sel:2;
        /** reg_uart2_clk_en : R/W; bitpos: [7]; default: 1;
         *  need_des
         */
        uint32_t reg_uart2_clk_en:1;
        /** reg_uart2_sclk_div_num : R/W; bitpos: [15:8]; default: 0;
         *  need_des
         */
        uint32_t reg_uart2_sclk_div_num:8;
        /** reg_uart2_sclk_div_numerator : R/W; bitpos: [23:16]; default: 0;
         *  need_des
         */
        uint32_t reg_uart2_sclk_div_numerator:8;
        /** reg_uart2_sclk_div_denominator : R/W; bitpos: [31:24]; default: 0;
         *  need_des
         */
        uint32_t reg_uart2_sclk_div_denominator:8;
    };
    uint32_t val;
} hp_sys_clkrst_uart2_ctrl0_reg_t;


/** Group: uart3_ctrl */
/** Type of uart3_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_uart3_sys_clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_uart3_sys_clk_en:1;
        /** reg_uart3_apb_clk_en : R/W; bitpos: [1]; default: 1;
         *  need_des
         */
        uint32_t reg_uart3_apb_clk_en:1;
        /** reg_uart3_core_rst_en : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_uart3_core_rst_en:1;
        /** reg_uart3_apb_rst_en : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_uart3_apb_rst_en:1;
        /** reg_uart3_force_norst : R/W; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t reg_uart3_force_norst:1;
        /** reg_uart3_clk_src_sel : R/W; bitpos: [6:5]; default: 0;
         *  need_des
         */
        uint32_t reg_uart3_clk_src_sel:2;
        /** reg_uart3_clk_en : R/W; bitpos: [7]; default: 1;
         *  need_des
         */
        uint32_t reg_uart3_clk_en:1;
        /** reg_uart3_sclk_div_num : R/W; bitpos: [15:8]; default: 0;
         *  need_des
         */
        uint32_t reg_uart3_sclk_div_num:8;
        /** reg_uart3_sclk_div_numerator : R/W; bitpos: [23:16]; default: 0;
         *  need_des
         */
        uint32_t reg_uart3_sclk_div_numerator:8;
        /** reg_uart3_sclk_div_denominator : R/W; bitpos: [31:24]; default: 0;
         *  need_des
         */
        uint32_t reg_uart3_sclk_div_denominator:8;
    };
    uint32_t val;
} hp_sys_clkrst_uart3_ctrl0_reg_t;


/** Group: parlio_ctrl */
/** Type of parlio_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_parlio_sys_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_parlio_sys_clk_en:1;
        /** reg_parlio_apb_clk_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_parlio_apb_clk_en:1;
        /** reg_parlio_rst_en : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_parlio_rst_en:1;
        /** reg_parlio_force_norst : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_parlio_force_norst:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_sys_clkrst_parlio_ctrl0_reg_t;


/** Group: parlio_rx_ctrl */
/** Type of parlio_rx_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_parlio_rx_rst_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_parlio_rx_rst_en:1;
        /** reg_parlio_rx_force_norst : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_parlio_rx_force_norst:1;
        /** reg_parlio_rx_clk_src_sel : R/W; bitpos: [3:2]; default: 0;
         *  need_des
         */
        uint32_t reg_parlio_rx_clk_src_sel:2;
        /** reg_parlio_rx_clk_en : R/W; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t reg_parlio_rx_clk_en:1;
        /** reg_parlio_rx_clk_div_num : R/W; bitpos: [12:5]; default: 0;
         *  need_des
         */
        uint32_t reg_parlio_rx_clk_div_num:8;
        /** reg_parlio_rx_clk_div_numerator : R/W; bitpos: [20:13]; default: 0;
         *  need_des
         */
        uint32_t reg_parlio_rx_clk_div_numerator:8;
        /** reg_parlio_rx_clk_div_denominator : R/W; bitpos: [28:21]; default: 0;
         *  need_des
         */
        uint32_t reg_parlio_rx_clk_div_denominator:8;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} hp_sys_clkrst_parlio_rx_ctrl0_reg_t;


/** Group: parlio_tx_ctrl */
/** Type of parlio_tx_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_parlio_tx_rst_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_parlio_tx_rst_en:1;
        /** reg_parlio_tx_force_norst : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_parlio_tx_force_norst:1;
        /** reg_parlio_tx_clk_src_sel : R/W; bitpos: [3:2]; default: 0;
         *  need_des
         */
        uint32_t reg_parlio_tx_clk_src_sel:2;
        /** reg_parlio_tx_clk_en : R/W; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t reg_parlio_tx_clk_en:1;
        /** reg_parlio_tx_clk_div_num : R/W; bitpos: [12:5]; default: 0;
         *  need_des
         */
        uint32_t reg_parlio_tx_clk_div_num:8;
        /** reg_parlio_tx_clk_div_numerator : R/W; bitpos: [20:13]; default: 0;
         *  need_des
         */
        uint32_t reg_parlio_tx_clk_div_numerator:8;
        /** reg_parlio_tx_clk_div_denominator : R/W; bitpos: [28:21]; default: 0;
         *  need_des
         */
        uint32_t reg_parlio_tx_clk_div_denominator:8;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} hp_sys_clkrst_parlio_tx_ctrl0_reg_t;


/** Group: bitscrambler_ctrl */
/** Type of bitscrambler_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_bitscrambler_sys_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_bitscrambler_sys_clk_en:1;
        /** reg_bitscrambler_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_bitscrambler_rst_en:1;
        /** reg_bitscrambler_force_norst : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_bitscrambler_force_norst:1;
        /** reg_bitscrambler_rx_sys_clk_en : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_bitscrambler_rx_sys_clk_en:1;
        /** reg_bitscrambler_rx_rst_en : R/W; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t reg_bitscrambler_rx_rst_en:1;
        /** reg_bitscrambler_rx_force_norst : R/W; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t reg_bitscrambler_rx_force_norst:1;
        /** reg_bitscrambler_tx_sys_clk_en : R/W; bitpos: [6]; default: 0;
         *  need_des
         */
        uint32_t reg_bitscrambler_tx_sys_clk_en:1;
        /** reg_bitscrambler_tx_rst_en : R/W; bitpos: [7]; default: 0;
         *  need_des
         */
        uint32_t reg_bitscrambler_tx_rst_en:1;
        /** reg_bitscrambler_tx_force_norst : R/W; bitpos: [8]; default: 0;
         *  need_des
         */
        uint32_t reg_bitscrambler_tx_force_norst:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_sys_clkrst_bitscrambler_ctrl0_reg_t;


/** Group: etm_ctrl */
/** Type of etm_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:1;
        /** reg_etm_apb_clk_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_etm_apb_clk_en:1;
        /** reg_etm_rst_en : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_etm_rst_en:1;
        /** reg_etm_force_norst : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_etm_force_norst:1;
        /** reg_soc_etm_clk_sel : R/W; bitpos: [5:4]; default: 0;
         *  need_des
         */
        uint32_t reg_soc_etm_clk_sel:2;
        /** reg_soc_etm_clk_en : R/W; bitpos: [6]; default: 1;
         *  need_des
         */
        uint32_t reg_soc_etm_clk_en:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} hp_sys_clkrst_etm_ctrl0_reg_t;


/** Group: usb_otghs_ctrl */
/** Type of usb_otghs_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_usb_otghs_apb_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_usb_otghs_apb_clk_en:1;
        /** reg_usb_otghs_sys_clk_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_usb_otghs_sys_clk_en:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_sys_clkrst_usb_otghs_ctrl0_reg_t;


/** Group: dma2d_ctrl */
/** Type of dma2d_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_dma2d_sys_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_dma2d_sys_clk_en:1;
        /** reg_dma2d_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_dma2d_rst_en:1;
        /** reg_dma2d_force_norst : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_dma2d_force_norst:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_sys_clkrst_dma2d_ctrl0_reg_t;


/** Group: ppa_ctrl */
/** Type of ppa_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_ppa_sys_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_ppa_sys_clk_en:1;
        /** reg_ppa_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_ppa_rst_en:1;
        /** reg_ppa_force_norst : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_ppa_force_norst:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_sys_clkrst_ppa_ctrl0_reg_t;


/** Group: jpeg_ctrl */
/** Type of jpeg_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_jpeg_sys_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_jpeg_sys_clk_en:1;
        /** reg_jpeg_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_jpeg_rst_en:1;
        /** reg_jpeg_force_norst : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_jpeg_force_norst:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_sys_clkrst_jpeg_ctrl0_reg_t;


/** Group: rmt_ctrl */
/** Type of rmt_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_rmt_sys_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_rmt_sys_clk_en:1;
        /** reg_rmt_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_rmt_rst_en:1;
        /** reg_rmt_force_norst : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_rmt_force_norst:1;
        /** reg_rmt_clk_src_sel : R/W; bitpos: [4:3]; default: 0;
         *  need_des
         */
        uint32_t reg_rmt_clk_src_sel:2;
        /** reg_rmt_clk_en : R/W; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t reg_rmt_clk_en:1;
        /** reg_rmt_clk_div_num : R/W; bitpos: [13:6]; default: 0;
         *  need_des
         */
        uint32_t reg_rmt_clk_div_num:8;
        /** reg_rmt_clk_div_numerator : R/W; bitpos: [21:14]; default: 0;
         *  need_des
         */
        uint32_t reg_rmt_clk_div_numerator:8;
        /** reg_rmt_clk_div_denominator : R/W; bitpos: [29:22]; default: 0;
         *  need_des
         */
        uint32_t reg_rmt_clk_div_denominator:8;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} hp_sys_clkrst_rmt_ctrl0_reg_t;


/** Group: sdio_host_ctrl */
/** Type of sdio_host_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_sdmmc_sys_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_sdmmc_sys_clk_en:1;
        /** reg_sdio_hs_mode : R/W; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t reg_sdio_hs_mode:1;
        /** reg_sdio_ls_clk_src_sel : R/W; bitpos: [2]; default: 0;
         *  reserved
         */
        uint32_t reg_sdio_ls_clk_src_sel:1;
        /** reg_sdio_ls_clk_en : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_sdio_ls_clk_en:1;
        /** reg_sdio_ls_clk_div_num : R/W; bitpos: [11:4]; default: 0;
         *  need_des
         */
        uint32_t reg_sdio_ls_clk_div_num:8;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} hp_sys_clkrst_sdio_host_ctrl0_reg_t;


/** Group: sdio_host_func_ctrl */
/** Type of sdio_host_func_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_sdio_ls_clk_edge_cfg_update : WT; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_sdio_ls_clk_edge_cfg_update:1;
        /** reg_sdio_ls_clk_edge_l : R/W; bitpos: [4:1]; default: 0;
         *  need_des
         */
        uint32_t reg_sdio_ls_clk_edge_l:4;
        /** reg_sdio_ls_clk_edge_h : R/W; bitpos: [8:5]; default: 0;
         *  need_des
         */
        uint32_t reg_sdio_ls_clk_edge_h:4;
        /** reg_sdio_ls_clk_edge_n : R/W; bitpos: [12:9]; default: 0;
         *  need_des
         */
        uint32_t reg_sdio_ls_clk_edge_n:4;
        /** reg_sdio_ls_slf_clk_edge_sel : R/W; bitpos: [15:13]; default: 0;
         *  need_des
         */
        uint32_t reg_sdio_ls_slf_clk_edge_sel:3;
        /** reg_sdio_ls_drv_clk_edge_sel : R/W; bitpos: [18:16]; default: 0;
         *  need_des
         */
        uint32_t reg_sdio_ls_drv_clk_edge_sel:3;
        /** reg_sdio_ls_sam_clk_edge_sel : R/W; bitpos: [21:19]; default: 0;
         *  need_des
         */
        uint32_t reg_sdio_ls_sam_clk_edge_sel:3;
        /** reg_sdio_ls_slf_clk_en : R/W; bitpos: [22]; default: 0;
         *  need_des
         */
        uint32_t reg_sdio_ls_slf_clk_en:1;
        /** reg_sdio_ls_drv_clk_en : R/W; bitpos: [23]; default: 0;
         *  need_des
         */
        uint32_t reg_sdio_ls_drv_clk_en:1;
        /** reg_sdio_ls_sam_clk_en : R/W; bitpos: [24]; default: 0;
         *  need_des
         */
        uint32_t reg_sdio_ls_sam_clk_en:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} hp_sys_clkrst_sdio_host_func_ctrl0_reg_t;


/** Group: emac_ctrl */
/** Type of emac_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_emac_sys_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_emac_sys_clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_sys_clkrst_emac_ctrl0_reg_t;


/** Group: dsi_ctrl */
/** Type of dsi_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_dsi_sys_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_dsi_sys_clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_sys_clkrst_dsi_ctrl0_reg_t;


/** Group: dsi_phy_ctrl */
/** Type of dsi_phy_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_mipi_dsi_dphy_clk_src_sel : R/W; bitpos: [1:0]; default: 0;
         *  need_des
         */
        uint32_t reg_mipi_dsi_dphy_clk_src_sel:2;
        /** reg_mipi_dsi_dphy_cfg_clk_en : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_mipi_dsi_dphy_cfg_clk_en:1;
        /** reg_mipi_dsi_dphy_pll_refclk_en : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_mipi_dsi_dphy_pll_refclk_en:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_sys_clkrst_dsi_phy_ctrl0_reg_t;


/** Group: dsi_dpi_ctrl */
/** Type of dsi_dpi_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_mipi_dsi_dpiclk_src_sel : R/W; bitpos: [1:0]; default: 0;
         *  need_des
         */
        uint32_t reg_mipi_dsi_dpiclk_src_sel:2;
        /** reg_mipi_dsi_dpiclk_en : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_mipi_dsi_dpiclk_en:1;
        /** reg_mipi_dsi_dpiclk_div_num : R/W; bitpos: [10:3]; default: 0;
         *  need_des
         */
        uint32_t reg_mipi_dsi_dpiclk_div_num:8;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} hp_sys_clkrst_dsi_dpi_ctrl0_reg_t;


/** Group: mspi_pad_ctrl */
/** Type of mspi_pad_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_mspi_pad_ctrl_apb_clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_mspi_pad_ctrl_apb_clk_en:1;
        /** reg_mspi_pad_ctrl_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_mspi_pad_ctrl_rst_en:1;
        /** reg_mspi_pad_ctrl_force_norst : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_mspi_pad_ctrl_force_norst:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_sys_clkrst_mspi_pad_ctrl0_reg_t;


/** Group: i2c0_ctrl */
/** Type of i2c0_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_i2c0_apb_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_i2c0_apb_clk_en:1;
        /** reg_i2c0_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_i2c0_rst_en:1;
        /** reg_i2c0_force_norst : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_i2c0_force_norst:1;
        /** reg_i2c0_clk_src_sel : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_i2c0_clk_src_sel:1;
        /** reg_i2c0_clk_en : R/W; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t reg_i2c0_clk_en:1;
        /** reg_i2c0_clk_div_num : R/W; bitpos: [12:5]; default: 0;
         *  need_des
         */
        uint32_t reg_i2c0_clk_div_num:8;
        /** reg_i2c0_clk_div_numerator : R/W; bitpos: [20:13]; default: 0;
         *  need_des
         */
        uint32_t reg_i2c0_clk_div_numerator:8;
        /** reg_i2c0_clk_div_denominator : R/W; bitpos: [28:21]; default: 0;
         *  need_des
         */
        uint32_t reg_i2c0_clk_div_denominator:8;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} hp_sys_clkrst_i2c0_ctrl0_reg_t;


/** Group: i2c1_ctrl */
/** Type of i2c1_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_i2c1_apb_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_i2c1_apb_clk_en:1;
        /** reg_i2c1_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_i2c1_rst_en:1;
        /** reg_i2c1_force_norst : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_i2c1_force_norst:1;
        /** reg_i2c1_clk_src_sel : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_i2c1_clk_src_sel:1;
        /** reg_i2c1_clk_en : R/W; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t reg_i2c1_clk_en:1;
        /** reg_i2c1_clk_div_num : R/W; bitpos: [12:5]; default: 0;
         *  need_des
         */
        uint32_t reg_i2c1_clk_div_num:8;
        /** reg_i2c1_clk_div_numerator : R/W; bitpos: [20:13]; default: 0;
         *  need_des
         */
        uint32_t reg_i2c1_clk_div_numerator:8;
        /** reg_i2c1_clk_div_denominator : R/W; bitpos: [28:21]; default: 0;
         *  need_des
         */
        uint32_t reg_i2c1_clk_div_denominator:8;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} hp_sys_clkrst_i2c1_ctrl0_reg_t;


/** Group: i2s0_ctrl */
/** Type of i2s0_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_i2s0_apb_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s0_apb_clk_en:1;
        /** reg_i2s0_apb_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s0_apb_rst_en:1;
        /** reg_i2s0_force_norst : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s0_force_norst:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_sys_clkrst_i2s0_ctrl0_reg_t;


/** Group: i2s0_rx_ctrl */
/** Type of i2s0_rx_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_i2s0_rx_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s0_rx_clk_en:1;
        /** reg_i2s0_rx_clk_src_sel : R/W; bitpos: [2:1]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s0_rx_clk_src_sel:2;
        /** reg_i2s0_rx_div_n : R/W; bitpos: [10:3]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s0_rx_div_n:8;
        /** reg_i2s0_mst_clk_sel : R/W; bitpos: [11]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s0_mst_clk_sel:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} hp_sys_clkrst_i2s0_rx_ctrl0_reg_t;


/** Group: i2s0_rx_div_ctrl */
/** Type of i2s0_rx_div_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_i2s0_rx_div_x : R/W; bitpos: [8:0]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s0_rx_div_x:9;
        /** reg_i2s0_rx_div_y : R/W; bitpos: [17:9]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s0_rx_div_y:9;
        /** reg_i2s0_rx_div_z : R/W; bitpos: [26:18]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s0_rx_div_z:9;
        /** reg_i2s0_rx_div_yn1 : R/W; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s0_rx_div_yn1:1;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} hp_sys_clkrst_i2s0_rx_div_ctrl0_reg_t;


/** Group: i2s0_tx_ctrl */
/** Type of i2s0_tx_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_i2s0_tx_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s0_tx_clk_en:1;
        /** reg_i2s0_tx_clk_src_sel : R/W; bitpos: [2:1]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s0_tx_clk_src_sel:2;
        /** reg_i2s0_tx_div_n : R/W; bitpos: [10:3]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s0_tx_div_n:8;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} hp_sys_clkrst_i2s0_tx_ctrl0_reg_t;


/** Group: i2s0_tx_div_ctrl */
/** Type of i2s0_tx_div_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_i2s0_tx_div_x : R/W; bitpos: [8:0]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s0_tx_div_x:9;
        /** reg_i2s0_tx_div_y : R/W; bitpos: [17:9]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s0_tx_div_y:9;
        /** reg_i2s0_tx_div_z : R/W; bitpos: [26:18]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s0_tx_div_z:9;
        /** reg_i2s0_tx_div_yn1 : R/W; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s0_tx_div_yn1:1;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} hp_sys_clkrst_i2s0_tx_div_ctrl0_reg_t;


/** Group: i2s1_ctrl */
/** Type of i2s1_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_i2s1_apb_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s1_apb_clk_en:1;
        /** reg_i2s1_apb_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s1_apb_rst_en:1;
        /** reg_i2s1_force_norst : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s1_force_norst:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_sys_clkrst_i2s1_ctrl0_reg_t;


/** Group: i2s1_rx_ctrl */
/** Type of i2s1_rx_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_i2s1_rx_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s1_rx_clk_en:1;
        /** reg_i2s1_rx_clk_src_sel : R/W; bitpos: [2:1]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s1_rx_clk_src_sel:2;
        /** reg_i2s1_rx_div_n : R/W; bitpos: [10:3]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s1_rx_div_n:8;
        /** reg_i2s1_mst_clk_sel : R/W; bitpos: [11]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s1_mst_clk_sel:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} hp_sys_clkrst_i2s1_rx_ctrl0_reg_t;


/** Group: i2s1_rx_div_ctrl */
/** Type of i2s1_rx_div_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_i2s1_rx_div_x : R/W; bitpos: [8:0]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s1_rx_div_x:9;
        /** reg_i2s1_rx_div_y : R/W; bitpos: [17:9]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s1_rx_div_y:9;
        /** reg_i2s1_rx_div_z : R/W; bitpos: [26:18]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s1_rx_div_z:9;
        /** reg_i2s1_rx_div_yn1 : R/W; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s1_rx_div_yn1:1;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} hp_sys_clkrst_i2s1_rx_div_ctrl0_reg_t;


/** Group: i2s1_tx_ctrl */
/** Type of i2s1_tx_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_i2s1_tx_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s1_tx_clk_en:1;
        /** reg_i2s1_tx_clk_src_sel : R/W; bitpos: [2:1]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s1_tx_clk_src_sel:2;
        /** reg_i2s1_tx_div_n : R/W; bitpos: [10:3]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s1_tx_div_n:8;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} hp_sys_clkrst_i2s1_tx_ctrl0_reg_t;


/** Group: i2s1_tx_div_ctrl */
/** Type of i2s1_tx_div_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_i2s1_tx_div_x : R/W; bitpos: [8:0]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s1_tx_div_x:9;
        /** reg_i2s1_tx_div_y : R/W; bitpos: [17:9]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s1_tx_div_y:9;
        /** reg_i2s1_tx_div_z : R/W; bitpos: [26:18]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s1_tx_div_z:9;
        /** reg_i2s1_tx_div_yn1 : R/W; bitpos: [27]; default: 0;
         *  need_des
         */
        uint32_t reg_i2s1_tx_div_yn1:1;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} hp_sys_clkrst_i2s1_tx_div_ctrl0_reg_t;


/** Group: twai0_ctrl */
/** Type of twai0_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_twai0_apb_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_twai0_apb_clk_en:1;
        /** reg_twai0_clk_src_sel : R/W; bitpos: [2:1]; default: 0;
         *  need_des
         */
        uint32_t reg_twai0_clk_src_sel:2;
        /** reg_twai0_clk_en : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_twai0_clk_en:1;
        /** reg_twai0_rst_en : R/W; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t reg_twai0_rst_en:1;
        /** reg_twai0_force_norst : R/W; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t reg_twai0_force_norst:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} hp_sys_clkrst_twai0_ctrl0_reg_t;


/** Group: twai1_ctrl */
/** Type of twai1_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_twai1_apb_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_twai1_apb_clk_en:1;
        /** reg_twai1_clk_src_sel : R/W; bitpos: [2:1]; default: 0;
         *  need_des
         */
        uint32_t reg_twai1_clk_src_sel:2;
        /** reg_twai1_clk_en : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_twai1_clk_en:1;
        /** reg_twai1_rst_en : R/W; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t reg_twai1_rst_en:1;
        /** reg_twai1_force_norst : R/W; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t reg_twai1_force_norst:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} hp_sys_clkrst_twai1_ctrl0_reg_t;


/** Group: timergrp0_ctrl */
/** Type of timergrp0_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_timergrp0_apb_clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_timergrp0_apb_clk_en:1;
        /** reg_timergrp0_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_timergrp0_rst_en:1;
        /** reg_timergrp0_force_norst : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_timergrp0_force_norst:1;
        /** reg_timergrp0_t0_src_sel : R/W; bitpos: [4:3]; default: 0;
         *  need_des
         */
        uint32_t reg_timergrp0_t0_src_sel:2;
        /** reg_timergrp0_t0_clk_en : R/W; bitpos: [5]; default: 1;
         *  need_des
         */
        uint32_t reg_timergrp0_t0_clk_en:1;
        /** reg_timergrp0_t1_src_sel : R/W; bitpos: [7:6]; default: 0;
         *  need_des
         */
        uint32_t reg_timergrp0_t1_src_sel:2;
        /** reg_timergrp0_t1_clk_en : R/W; bitpos: [8]; default: 1;
         *  need_des
         */
        uint32_t reg_timergrp0_t1_clk_en:1;
        /** reg_timergrp0_wdt_src_sel : R/W; bitpos: [10:9]; default: 0;
         *  need_des
         */
        uint32_t reg_timergrp0_wdt_src_sel:2;
        /** reg_timergrp0_wdt_clk_en : R/W; bitpos: [11]; default: 1;
         *  need_des
         */
        uint32_t reg_timergrp0_wdt_clk_en:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} hp_sys_clkrst_timergrp0_ctrl0_reg_t;


/** Group: timergrp0_tgrt_ctrl */
/** Type of timergrp0_tgrt_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_timergrp0_tgrt_clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_timergrp0_tgrt_clk_en:1;
        /** reg_timergrp0_tgrt_clk_src_sel : R/W; bitpos: [4:1]; default: 8;
         *  need_des
         */
        uint32_t reg_timergrp0_tgrt_clk_src_sel:4;
        /** reg_timergrp0_tgrt_clk_div_num : R/W; bitpos: [20:5]; default: 0;
         *  need_des
         */
        uint32_t reg_timergrp0_tgrt_clk_div_num:16;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} hp_sys_clkrst_timergrp0_tgrt_ctrl0_reg_t;


/** Group: timergrp1_ctrl */
/** Type of timergrp1_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_timergrp1_apb_clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_timergrp1_apb_clk_en:1;
        /** reg_timergrp1_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_timergrp1_rst_en:1;
        /** reg_timergrp1_force_norst : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_timergrp1_force_norst:1;
        /** reg_timergrp1_t0_src_sel : R/W; bitpos: [4:3]; default: 0;
         *  need_des
         */
        uint32_t reg_timergrp1_t0_src_sel:2;
        /** reg_timergrp1_t0_clk_en : R/W; bitpos: [5]; default: 1;
         *  need_des
         */
        uint32_t reg_timergrp1_t0_clk_en:1;
        /** reg_timergrp1_t1_src_sel : R/W; bitpos: [7:6]; default: 0;
         *  need_des
         */
        uint32_t reg_timergrp1_t1_src_sel:2;
        /** reg_timergrp1_t1_clk_en : R/W; bitpos: [8]; default: 1;
         *  need_des
         */
        uint32_t reg_timergrp1_t1_clk_en:1;
        /** reg_timergrp1_wdt_src_sel : R/W; bitpos: [10:9]; default: 0;
         *  need_des
         */
        uint32_t reg_timergrp1_wdt_src_sel:2;
        /** reg_timergrp1_wdt_clk_en : R/W; bitpos: [11]; default: 1;
         *  need_des
         */
        uint32_t reg_timergrp1_wdt_clk_en:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} hp_sys_clkrst_timergrp1_ctrl0_reg_t;


/** Group: systimer_ctrl */
/** Type of systimer_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_systimer_apb_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_systimer_apb_clk_en:1;
        /** reg_systimer_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_systimer_rst_en:1;
        /** reg_systimer_force_norst : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_systimer_force_norst:1;
        /** reg_systimer_clk_src_sel : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_systimer_clk_src_sel:1;
        /** reg_systimer_clk_en : R/W; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t reg_systimer_clk_en:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} hp_sys_clkrst_systimer_ctrl0_reg_t;


/** Group: mcpwm0_ctrl */
/** Type of mcpwm0_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_mcpwm0_apb_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_mcpwm0_apb_clk_en:1;
        /** reg_mcpwm0_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_mcpwm0_rst_en:1;
        /** reg_mcpwm0_force_norst : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_mcpwm0_force_norst:1;
        /** reg_mcpwm0_clk_src_sel : R/W; bitpos: [4:3]; default: 0;
         *  need_des
         */
        uint32_t reg_mcpwm0_clk_src_sel:2;
        /** reg_mcpwm0_clk_en : R/W; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t reg_mcpwm0_clk_en:1;
        /** reg_mcpwm0_clk_div_num : R/W; bitpos: [13:6]; default: 0;
         *  need_des
         */
        uint32_t reg_mcpwm0_clk_div_num:8;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} hp_sys_clkrst_mcpwm0_ctrl0_reg_t;


/** Group: mcpwm1_ctrl */
/** Type of mcpwm1_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_mcpwm1_apb_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_mcpwm1_apb_clk_en:1;
        /** reg_mcpwm1_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_mcpwm1_rst_en:1;
        /** reg_mcpwm1_force_norst : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_mcpwm1_force_norst:1;
        /** reg_mcpwm1_clk_src_sel : R/W; bitpos: [4:3]; default: 0;
         *  need_des
         */
        uint32_t reg_mcpwm1_clk_src_sel:2;
        /** reg_mcpwm1_clk_en : R/W; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t reg_mcpwm1_clk_en:1;
        /** reg_mcpwm1_clk_div_num : R/W; bitpos: [13:6]; default: 0;
         *  need_des
         */
        uint32_t reg_mcpwm1_clk_div_num:8;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} hp_sys_clkrst_mcpwm1_ctrl0_reg_t;


/** Group: mcpwm2_ctrl */
/** Type of mcpwm2_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_mcpwm2_apb_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_mcpwm2_apb_clk_en:1;
        /** reg_mcpwm2_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_mcpwm2_rst_en:1;
        /** reg_mcpwm2_force_norst : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_mcpwm2_force_norst:1;
        /** reg_mcpwm2_clk_src_sel : R/W; bitpos: [4:3]; default: 0;
         *  need_des
         */
        uint32_t reg_mcpwm2_clk_src_sel:2;
        /** reg_mcpwm2_clk_en : R/W; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t reg_mcpwm2_clk_en:1;
        /** reg_mcpwm2_clk_div_num : R/W; bitpos: [13:6]; default: 0;
         *  need_des
         */
        uint32_t reg_mcpwm2_clk_div_num:8;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} hp_sys_clkrst_mcpwm2_ctrl0_reg_t;


/** Group: mcpwm3_ctrl */
/** Type of mcpwm3_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_mcpwm3_apb_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_mcpwm3_apb_clk_en:1;
        /** reg_mcpwm3_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_mcpwm3_rst_en:1;
        /** reg_mcpwm3_force_norst : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_mcpwm3_force_norst:1;
        /** reg_mcpwm3_clk_src_sel : R/W; bitpos: [4:3]; default: 0;
         *  need_des
         */
        uint32_t reg_mcpwm3_clk_src_sel:2;
        /** reg_mcpwm3_clk_en : R/W; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t reg_mcpwm3_clk_en:1;
        /** reg_mcpwm3_clk_div_num : R/W; bitpos: [13:6]; default: 0;
         *  need_des
         */
        uint32_t reg_mcpwm3_clk_div_num:8;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} hp_sys_clkrst_mcpwm3_ctrl0_reg_t;


/** Group: intrmtx_ctrl */
/** Type of intrmtx_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_intrmtx_apb_clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_intrmtx_apb_clk_en:1;
        /** reg_intrmtx_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_intrmtx_rst_en:1;
        /** reg_intrmtx_force_norst : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_intrmtx_force_norst:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_sys_clkrst_intrmtx_ctrl0_reg_t;


/** Group: pcnt_ctrl */
/** Type of pcnt_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_pcnt0_apb_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_pcnt0_apb_clk_en:1;
        /** reg_pcnt0_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_pcnt0_rst_en:1;
        /** reg_pcnt0_force_norst : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_pcnt0_force_norst:1;
        /** reg_pcnt1_apb_clk_en : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_pcnt1_apb_clk_en:1;
        /** reg_pcnt1_rst_en : R/W; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t reg_pcnt1_rst_en:1;
        /** reg_pcnt1_force_norst : R/W; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t reg_pcnt1_force_norst:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} hp_sys_clkrst_pcnt_ctrl0_reg_t;


/** Group: usb_device_ctrl */
/** Type of usb_device_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_usb_device_apb_clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_usb_device_apb_clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_sys_clkrst_usb_device_ctrl0_reg_t;


/** Group: ledc_ctrl */
/** Type of ledc_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_ledc0_apb_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_ledc0_apb_clk_en:1;
        /** reg_ledc0_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_ledc0_rst_en:1;
        /** reg_ledc0_force_norst : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_ledc0_force_norst:1;
        /** reg_ledc0_clk_src_sel : R/W; bitpos: [4:3]; default: 0;
         *  need_des
         */
        uint32_t reg_ledc0_clk_src_sel:2;
        /** reg_ledc0_clk_en : R/W; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t reg_ledc0_clk_en:1;
        /** reg_ledc1_apb_clk_en : R/W; bitpos: [6]; default: 0;
         *  need_des
         */
        uint32_t reg_ledc1_apb_clk_en:1;
        /** reg_ledc1_rst_en : R/W; bitpos: [7]; default: 0;
         *  need_des
         */
        uint32_t reg_ledc1_rst_en:1;
        /** reg_ledc1_force_norst : R/W; bitpos: [8]; default: 0;
         *  need_des
         */
        uint32_t reg_ledc1_force_norst:1;
        /** reg_ledc1_clk_src_sel : R/W; bitpos: [10:9]; default: 0;
         *  need_des
         */
        uint32_t reg_ledc1_clk_src_sel:2;
        /** reg_ledc1_clk_en : R/W; bitpos: [11]; default: 0;
         *  need_des
         */
        uint32_t reg_ledc1_clk_en:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} hp_sys_clkrst_ledc_ctrl0_reg_t;


/** Group: lcdcam_ctrl */
/** Type of lcdcam_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_lcdcam_apb_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_lcdcam_apb_clk_en:1;
        /** reg_lcdcam_apb_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_lcdcam_apb_rst_en:1;
        /** reg_lcdcam_sys_clk_en : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_lcdcam_sys_clk_en:1;
        /** reg_lcdcam_rst_en : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_lcdcam_rst_en:1;
        /** reg_lcdcam_force_norst : R/W; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t reg_lcdcam_force_norst:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} hp_sys_clkrst_lcdcam_ctrl0_reg_t;


/** Group: lcdcam_lcd_ctrl */
/** Type of lcdcam_lcd_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_lcd_clk_src_sel : R/W; bitpos: [1:0]; default: 0;
         *  need_des
         */
        uint32_t reg_lcd_clk_src_sel:2;
        /** reg_lcd_clk_en : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_lcd_clk_en:1;
        /** reg_lcd_clk_div_num : R/W; bitpos: [10:3]; default: 0;
         *  need_des
         */
        uint32_t reg_lcd_clk_div_num:8;
        /** reg_lcd_clk_div_numerator : R/W; bitpos: [18:11]; default: 0;
         *  need_des
         */
        uint32_t reg_lcd_clk_div_numerator:8;
        /** reg_lcd_clk_div_denominator : R/W; bitpos: [26:19]; default: 0;
         *  need_des
         */
        uint32_t reg_lcd_clk_div_denominator:8;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} hp_sys_clkrst_lcdcam_lcd_ctrl0_reg_t;


/** Group: lcdcam_lcdcam_ctrl */
/** Type of lcdcam_lcdcam_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_lcdcam_clk_src_sel : R/W; bitpos: [1:0]; default: 0;
         *  need_des
         */
        uint32_t reg_lcdcam_clk_src_sel:2;
        /** reg_lcdcam_clk_en : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_lcdcam_clk_en:1;
        /** reg_lcdcam_clk_div_num : R/W; bitpos: [10:3]; default: 0;
         *  need_des
         */
        uint32_t reg_lcdcam_clk_div_num:8;
        /** reg_lcdcam_clk_div_numerator : R/W; bitpos: [18:11]; default: 0;
         *  need_des
         */
        uint32_t reg_lcdcam_clk_div_numerator:8;
        /** reg_lcdcam_clk_div_denominator : R/W; bitpos: [26:19]; default: 0;
         *  need_des
         */
        uint32_t reg_lcdcam_clk_div_denominator:8;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} hp_sys_clkrst_lcdcam_lcdcam_ctrl0_reg_t;


/** Group: lcdcam_cam_ctrl */
/** Type of lcdcam_cam_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_cam_clk_src_sel : R/W; bitpos: [1:0]; default: 0;
         *  need_des
         */
        uint32_t reg_cam_clk_src_sel:2;
        /** reg_cam_clk_en : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_cam_clk_en:1;
        /** reg_cam_clk_div_num : R/W; bitpos: [10:3]; default: 0;
         *  need_des
         */
        uint32_t reg_cam_clk_div_num:8;
        /** reg_cam_clk_div_numerator : R/W; bitpos: [18:11]; default: 0;
         *  need_des
         */
        uint32_t reg_cam_clk_div_numerator:8;
        /** reg_cam_clk_div_denominator : R/W; bitpos: [26:19]; default: 0;
         *  need_des
         */
        uint32_t reg_cam_clk_div_denominator:8;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} hp_sys_clkrst_lcdcam_cam_ctrl0_reg_t;


/** Group: iomux_ctrl */
/** Type of iomux_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_iomux_apb_clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_iomux_apb_clk_en:1;
        /** reg_iomux_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_iomux_rst_en:1;
        /** reg_iomux_force_norst : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_iomux_force_norst:1;
        /** reg_iomux_clk_src_sel : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_iomux_clk_src_sel:1;
        /** reg_iomux_clk_en : R/W; bitpos: [4]; default: 1;
         *  need_des
         */
        uint32_t reg_iomux_clk_en:1;
        /** reg_iomux_clk_div_num : R/W; bitpos: [12:5]; default: 0;
         *  need_des
         */
        uint32_t reg_iomux_clk_div_num:8;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} hp_sys_clkrst_iomux_ctrl0_reg_t;


/** Group: hpwdt_core0_rst_ctrl */
/** Type of hpwdt_core0_rst_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_hpcore0_stall_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_hpcore0_stall_en:1;
        /** reg_hpcore0_stall_wait_num : R/W; bitpos: [8:1]; default: 8;
         *  need_des
         */
        uint32_t reg_hpcore0_stall_wait_num:8;
        /** reg_wdt_hpcore0_rst_len : R/W; bitpos: [16:9]; default: 8;
         *  need_des
         */
        uint32_t reg_wdt_hpcore0_rst_len:8;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} hp_sys_clkrst_hpwdt_core0_rst_ctrl0_reg_t;


/** Group: hpwdt_core1_rst_ctrl */
/** Type of hpwdt_core1_rst_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_hpcore1_stall_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_hpcore1_stall_en:1;
        /** reg_hpcore1_stall_wait_num : R/W; bitpos: [8:1]; default: 8;
         *  need_des
         */
        uint32_t reg_hpcore1_stall_wait_num:8;
        /** reg_wdt_hpcore1_rst_len : R/W; bitpos: [16:9]; default: 8;
         *  need_des
         */
        uint32_t reg_wdt_hpcore1_rst_len:8;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} hp_sys_clkrst_hpwdt_core1_rst_ctrl0_reg_t;


/** Group: cpu_src_freq */
/** Type of cpu_src_freq0 register
 *  CPU Source Frequency
 */
typedef union {
    struct {
        /** reg_cpu_src_freq : RO; bitpos: [31:0]; default: 0;
         *  cpu source clock frequency, step by 0.25MHz
         */
        uint32_t reg_cpu_src_freq:32;
    };
    uint32_t val;
} hp_sys_clkrst_cpu_src_freq0_reg_t;


/** Group: cpu_clk_status */
/** Type of cpu_clk_status0 register
 *  CPU Clock Status
 */
typedef union {
    struct {
        /** reg_asic_or_fpga : RO; bitpos: [0]; default: 0;
         *  0: ASIC mode, 1: FPGA mode
         */
        uint32_t reg_asic_or_fpga:1;
        /** reg_cpu_div_effect : RO; bitpos: [1]; default: 0;
         *  0: Divider bypass, 1: Divider takes effect
         */
        uint32_t reg_cpu_div_effect:1;
        /** reg_cpu_src_is_cpll : RO; bitpos: [2]; default: 0;
         *  0: CPU source isn't cpll_400m, 1: CPU Source is cll_400m
         */
        uint32_t reg_cpu_src_is_cpll:1;
        /** reg_cpu_div_num_cur : RO; bitpos: [10:3]; default: 0;
         *  cpu current div number
         */
        uint32_t reg_cpu_div_num_cur:8;
        /** reg_cpu_div_numerator_cur : RO; bitpos: [13:11]; default: 0;
         *  cpu current div numerator
         */
        uint32_t reg_cpu_div_numerator_cur:3;
        /** reg_cpu_div_denominator_cur : RO; bitpos: [16:14]; default: 0;
         *  cpu current div denominator
         */
        uint32_t reg_cpu_div_denominator_cur:3;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} hp_sys_clkrst_cpu_clk_status0_reg_t;


/** Group: hpcore_wdt_reset_source */
/** Type of hpcore_wdt_reset_source0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_hpcore0_wdt_reset_source_sel : R/W; bitpos: [0]; default: 0;
         *  1'b0: use wdt0 to reset hpcore0, 1'b1: use wdt1 to reset hpcore0
         */
        uint32_t reg_hpcore0_wdt_reset_source_sel:1;
        /** reg_hpcore1_wdt_reset_source_sel : R/W; bitpos: [1]; default: 1;
         *  1'b0: use wdt0 to reset hpcore1, 1'b1: use wdt1 to reset hpcore1
         */
        uint32_t reg_hpcore1_wdt_reset_source_sel:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} hp_sys_clkrst_hpcore_wdt_reset_source0_reg_t;


/** Group: ana_pll_ctrl */
/** Type of ana_pll_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_plla_cal_end : RO; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_plla_cal_end:1;
        /** reg_plla_cal_stop : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_plla_cal_stop:1;
        /** reg_cpu_pll_cal_end : RO; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_cpu_pll_cal_end:1;
        /** reg_cpu_pll_cal_stop : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_cpu_pll_cal_stop:1;
        /** reg_sdio_pll_cal_end : RO; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t reg_sdio_pll_cal_end:1;
        /** reg_sdio_pll_cal_stop : R/W; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t reg_sdio_pll_cal_stop:1;
        /** reg_bbpll_cal_end : RO; bitpos: [6]; default: 0;
         *  need_des
         */
        uint32_t reg_bbpll_cal_end:1;
        /** reg_bbpll_cal_stop : R/W; bitpos: [7]; default: 0;
         *  need_des
         */
        uint32_t reg_bbpll_cal_stop:1;
        /** reg_mspi_cal_end : RO; bitpos: [8]; default: 0;
         *  need_des
         */
        uint32_t reg_mspi_cal_end:1;
        /** reg_mspi_cal_stop : R/W; bitpos: [9]; default: 0;
         *  need_des
         */
        uint32_t reg_mspi_cal_stop:1;
        /** reg_pll_cal_end : RO; bitpos: [10]; default: 0;
         *  need_des
         */
        uint32_t reg_pll_cal_end:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} hp_sys_clkrst_ana_pll_ctrl0_reg_t;


/** Group: ref_500m_ctrl */
/** Type of ref_500m_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_ref_500m_sel : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_ref_500m_sel:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_sys_clkrst_ref_500m_ctrl0_reg_t;


/** Group: ref_240m_ctrl */
/** Type of ref_240m_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_ref_240m_clk_div_num : R/W; bitpos: [7:0]; default: 1;
         *  need_des
         */
        uint32_t reg_ref_240m_clk_div_num:8;
        /** reg_ref_240m_clk_en : R/W; bitpos: [8]; default: 1;
         *  need_des
         */
        uint32_t reg_ref_240m_clk_en:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_sys_clkrst_ref_240m_ctrl0_reg_t;


/** Group: ref_160m_ctrl */
/** Type of ref_160m_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_ref_160m_clk_div_num : R/W; bitpos: [7:0]; default: 2;
         *  need_des
         */
        uint32_t reg_ref_160m_clk_div_num:8;
        /** reg_ref_160m_clk_en : R/W; bitpos: [8]; default: 1;
         *  need_des
         */
        uint32_t reg_ref_160m_clk_en:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_sys_clkrst_ref_160m_ctrl0_reg_t;


/** Group: ref_120m_ctrl */
/** Type of ref_120m_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_ref_120m_clk_div_num : R/W; bitpos: [7:0]; default: 3;
         *  need_des
         */
        uint32_t reg_ref_120m_clk_div_num:8;
        /** reg_ref_120m_clk_en : R/W; bitpos: [8]; default: 1;
         *  need_des
         */
        uint32_t reg_ref_120m_clk_en:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_sys_clkrst_ref_120m_ctrl0_reg_t;


/** Group: ref_80m_ctrl */
/** Type of ref_80m_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_ref_80m_clk_div_num : R/W; bitpos: [7:0]; default: 5;
         *  need_des
         */
        uint32_t reg_ref_80m_clk_div_num:8;
        /** reg_ref_80m_clk_en : R/W; bitpos: [8]; default: 1;
         *  need_des
         */
        uint32_t reg_ref_80m_clk_en:1;
        /** reg_ref_80m_sel : R/W; bitpos: [9]; default: 0;
         *  need_des
         */
        uint32_t reg_ref_80m_sel:1;
        /** reg_ref_80m_mux_clk_en : R/W; bitpos: [10]; default: 1;
         *  need_des
         */
        uint32_t reg_ref_80m_mux_clk_en:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} hp_sys_clkrst_ref_80m_ctrl0_reg_t;


/** Group: ref_60m_ctrl */
/** Type of ref_60m_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_ref_60m_clk_div_num : R/W; bitpos: [7:0]; default: 7;
         *  need_des
         */
        uint32_t reg_ref_60m_clk_div_num:8;
        /** reg_ref_60m_clk_en : R/W; bitpos: [8]; default: 1;
         *  need_des
         */
        uint32_t reg_ref_60m_clk_en:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_sys_clkrst_ref_60m_ctrl0_reg_t;


/** Group: ref_20m_ctrl */
/** Type of ref_20m_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_ref_20m_clk_div_num : R/W; bitpos: [7:0]; default: 23;
         *  need_des
         */
        uint32_t reg_ref_20m_clk_div_num:8;
        /** reg_ref_20m_clk_en : R/W; bitpos: [8]; default: 1;
         *  need_des
         */
        uint32_t reg_ref_20m_clk_en:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_sys_clkrst_ref_20m_ctrl0_reg_t;


/** Group: ref_50m_ctrl */
/** Type of ref_50m_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_ref_50m_clk_div_num : R/W; bitpos: [7:0]; default: 5;
         *  need_des
         */
        uint32_t reg_ref_50m_clk_div_num:8;
        /** reg_ref_50m_clk_en : R/W; bitpos: [8]; default: 1;
         *  need_des
         */
        uint32_t reg_ref_50m_clk_en:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_sys_clkrst_ref_50m_ctrl0_reg_t;


/** Group: ref_25m_ctrl */
/** Type of ref_25m_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_ref_25m_clk_div_num : R/W; bitpos: [7:0]; default: 11;
         *  need_des
         */
        uint32_t reg_ref_25m_clk_div_num:8;
        /** reg_ref_25m_clk_en : R/W; bitpos: [8]; default: 1;
         *  need_des
         */
        uint32_t reg_ref_25m_clk_en:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_sys_clkrst_ref_25m_ctrl0_reg_t;


/** Group: tm_clk_ctrl */
/** Type of tm_clk_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_tm_480m_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_tm_480m_clk_en:1;
        /** reg_tm_240m_clk_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_tm_240m_clk_en:1;
        /** reg_tm_160m_clk_en : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_tm_160m_clk_en:1;
        /** reg_tm_120m_clk_en : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_tm_120m_clk_en:1;
        /** reg_tm_80m_clk_en : R/W; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t reg_tm_80m_clk_en:1;
        /** reg_tm_60m_clk_en : R/W; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t reg_tm_60m_clk_en:1;
        /** reg_tm_48m_clk_en : R/W; bitpos: [6]; default: 0;
         *  need_des
         */
        uint32_t reg_tm_48m_clk_en:1;
        /** reg_tm_20m_clk_en : R/W; bitpos: [7]; default: 0;
         *  need_des
         */
        uint32_t reg_tm_20m_clk_en:1;
        /** reg_tm_250m_clk_en : R/W; bitpos: [8]; default: 0;
         *  need_des
         */
        uint32_t reg_tm_250m_clk_en:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_sys_clkrst_tm_clk_ctrl0_reg_t;


/** Group: dbg0_clk_ctrl */
/** Type of dbg0_clk_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_dbg_ch0_sel : R/W; bitpos: [7:0]; default: 255;
         *  need_des
         */
        uint32_t reg_dbg_ch0_sel:8;
        /** reg_dbg_ch0_div_num : R/W; bitpos: [15:8]; default: 3;
         *  need_des
         */
        uint32_t reg_dbg_ch0_div_num:8;
        /** reg_dbg_ch0_en : R/W; bitpos: [16]; default: 0;
         *  need_des
         */
        uint32_t reg_dbg_ch0_en:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} hp_sys_clkrst_dbg0_clk_ctrl0_reg_t;


/** Group: dbg1_clk_ctrl */
/** Type of dbg1_clk_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_dbg_ch1_sel : R/W; bitpos: [7:0]; default: 255;
         *  need_des
         */
        uint32_t reg_dbg_ch1_sel:8;
        /** reg_dbg_ch1_div_num : R/W; bitpos: [15:8]; default: 3;
         *  need_des
         */
        uint32_t reg_dbg_ch1_div_num:8;
        /** reg_dbg_ch1_en : R/W; bitpos: [16]; default: 0;
         *  need_des
         */
        uint32_t reg_dbg_ch1_en:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} hp_sys_clkrst_dbg1_clk_ctrl0_reg_t;


/** Group: dbg2_clk_ctrl */
/** Type of dbg2_clk_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_dbg_ch2_sel : R/W; bitpos: [7:0]; default: 255;
         *  need_des
         */
        uint32_t reg_dbg_ch2_sel:8;
        /** reg_dbg_ch2_div_num : R/W; bitpos: [15:8]; default: 3;
         *  need_des
         */
        uint32_t reg_dbg_ch2_div_num:8;
        /** reg_dbg_ch2_en : R/W; bitpos: [16]; default: 0;
         *  need_des
         */
        uint32_t reg_dbg_ch2_en:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} hp_sys_clkrst_dbg2_clk_ctrl0_reg_t;


/** Group: lp_clk_ctrl */
/** Type of lp_clk_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_lpaon_fosc_clk_force_on : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_lpaon_fosc_clk_force_on:1;
        /** reg_lpaon_xtal_clk_force_on : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_lpaon_xtal_clk_force_on:1;
        /** reg_lpaon_pll80m_clk_force_on : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_lpaon_pll80m_clk_force_on:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_sys_clkrst_lp_clk_ctrl0_reg_t;


/** Group: ahb_asrc_ctrl */
/** Type of ahb_asrc_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_ahb_asrc_sys_clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_ahb_asrc_sys_clk_en:1;
        /** reg_ahb_asrc_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_ahb_asrc_rst_en:1;
        /** reg_ahb_asrc_force_norst : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_ahb_asrc_force_norst:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_sys_clkrst_ahb_asrc_ctrl0_reg_t;


/** Group: cordic_ctrl */
/** Type of cordic_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_cordic_apb_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_cordic_apb_clk_en:1;
        /** reg_cordic_apb_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_cordic_apb_rst_en:1;
        /** reg_cordic_sys_clk_en : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_cordic_sys_clk_en:1;
        /** reg_cordic_sys_rst_en : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_cordic_sys_rst_en:1;
        /** reg_cordic_clk_en : R/W; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t reg_cordic_clk_en:1;
        /** reg_cordic_core_rst_en : R/W; bitpos: [5]; default: 0;
         *  need_des
         */
        uint32_t reg_cordic_core_rst_en:1;
        /** reg_cordic_force_norst : R/W; bitpos: [6]; default: 0;
         *  need_des
         */
        uint32_t reg_cordic_force_norst:1;
        /** reg_cordic_clk_src_sel : R/W; bitpos: [8:7]; default: 0;
         *  need_des
         */
        uint32_t reg_cordic_clk_src_sel:2;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_sys_clkrst_cordic_ctrl0_reg_t;

/** Type of cordic_ctrl1 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_cordic_clk_div_num : R/W; bitpos: [7:0]; default: 0;
         *  need_des
         */
        uint32_t reg_cordic_clk_div_num:8;
        /** reg_cordic_clk_div_numerator : R/W; bitpos: [15:8]; default: 0;
         *  need_des
         */
        uint32_t reg_cordic_clk_div_numerator:8;
        /** reg_cordic_clk_div_denonimator : R/W; bitpos: [23:16]; default: 0;
         *  need_des
         */
        uint32_t reg_cordic_clk_div_denonimator:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} hp_sys_clkrst_cordic_ctrl1_reg_t;


/** Group: zero_det_ctrl */
/** Type of zero_det_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_zero_det_apb_clk_en : R/W; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t reg_zero_det_apb_clk_en:1;
        /** reg_zero_det_apb_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_zero_det_apb_rst_en:1;
        /** reg_zero_det_clk_en : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_zero_det_clk_en:1;
        /** reg_zero_det_core_rst_en : R/W; bitpos: [3]; default: 0;
         *  need_des
         */
        uint32_t reg_zero_det_core_rst_en:1;
        /** reg_zero_det_force_norst : R/W; bitpos: [4]; default: 0;
         *  need_des
         */
        uint32_t reg_zero_det_force_norst:1;
        /** reg_zero_det_clk_src_sel : R/W; bitpos: [6:5]; default: 0;
         *  need_des
         */
        uint32_t reg_zero_det_clk_src_sel:2;
        /** reg_zero_det_clk_div_num : R/W; bitpos: [14:7]; default: 0;
         *  need_des
         */
        uint32_t reg_zero_det_clk_div_num:8;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} hp_sys_clkrst_zero_det_ctrl0_reg_t;


/** Group: power_ctrl */
/** Type of clk_pwr_decrease register
 *  need_des
 */
typedef union {
    struct {
        /** reg_apb_hw_decrease_div_num : R/W; bitpos: [7:0]; default: 0;
         *  need_des
         */
        uint32_t reg_apb_hw_decrease_div_num:8;
        /** reg_cpu_wfi_decrease_en : R/W; bitpos: [8]; default: 0;
         *  need_des
         */
        uint32_t reg_cpu_wfi_decrease_en:1;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} hp_sys_clkrst_clk_pwr_decrease_reg_t;


/** Group: cnnt_iomux_ctrl */
/** Type of cnnt_iomux_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_cnnt_iomux_apb_clk_en : R/W; bitpos: [0]; default: 1;
         *  cnnt_iomux clk en
         */
        uint32_t reg_cnnt_iomux_apb_clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_sys_clkrst_cnnt_iomux_ctrl0_reg_t;


/** Group: hp_i2cmst_ctrl */
/** Type of hp_i2cmst_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_hp_i2cmst_apb_clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_hp_i2cmst_apb_clk_en:1;
        /** reg_hp_i2cmst_rst_en : R/W; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t reg_hp_i2cmst_rst_en:1;
        /** reg_hp_i2cmst_force_norst : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_hp_i2cmst_force_norst:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_sys_clkrst_hp_i2cmst_ctrl0_reg_t;


/** Group: axi_perf_mon_ctrl */
/** Type of axi_perf_mon_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_axi_perf_mon_rst_en : R/W; bitpos: [0]; default: 0;
         *  axi_perf_mon rst en
         */
        uint32_t reg_axi_perf_mon_rst_en:1;
        /** reg_axi_perf_mon_clk_en : R/W; bitpos: [1]; default: 0;
         *  axi_perf_mon clk en
         */
        uint32_t reg_axi_perf_mon_clk_en:1;
        /** reg_axi_perf_mon_force_norst : R/W; bitpos: [2]; default: 0;
         *  need_des
         */
        uint32_t reg_axi_perf_mon_force_norst:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_sys_clkrst_axi_perf_mon_ctrl0_reg_t;


/** Group: cnnt_sysreg_ctrl */
/** Type of cnnt_sysreg_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_cnnt_sysreg_sys_clk_en : R/W; bitpos: [0]; default: 1;
         *  cnnt_sysreg clk en
         */
        uint32_t reg_cnnt_sysreg_sys_clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_sys_clkrst_cnnt_sysreg_ctrl0_reg_t;


/** Group: hp alive_sysreg_ctrl */
/** Type of hp_alive_sysreg_ctrl0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_hp_alive_sysreg_apb_clk_en : R/W; bitpos: [0]; default: 1;
         *  hp alive_sysreg clk en
         */
        uint32_t reg_hp_alive_sysreg_apb_clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_sys_clkrst_hp_alive_sysreg_ctrl0_reg_t;


/** Group: Configuration Register */
/** Type of modem_conf register
 *  MODEM_APB configuration register
 */
typedef union {
    struct {
        /** modem_apb_clk_en : R/W; bitpos: [0]; default: 1;
         *  This field indicates if modem_apb clock is enable. 0: disable, 1: enable(default).
         */
        uint32_t modem_apb_clk_en:1;
        /** modem_rst_en : R/W; bitpos: [1]; default: 0;
         *  Set this file as 1 to reset modem-subsystem.
         */
        uint32_t modem_rst_en:1;
        /** modem_clk_en : R/W; bitpos: [2]; default: 1;
         *  This field indicates if modem source clock is enable. 0: disable, 1:
         *  enable(default).
         */
        uint32_t modem_clk_en:1;
        /** modem_clk_source_sel : R/W; bitpos: [3]; default: 0;
         *  modem clk source sel
         */
        uint32_t modem_clk_source_sel:1;
        /** modem_pll_clk_en : R/W; bitpos: [4]; default: 0;
         *  modem clk source pll on
         */
        uint32_t modem_pll_clk_en:1;
        /** modem_xtal_clk_en : R/W; bitpos: [5]; default: 1;
         *  modem clk source xtal on
         */
        uint32_t modem_xtal_clk_en:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} hp_sys_clkrst_modem_conf_reg_t;


/** Group: clk_en */
/** Type of clk_en0 register
 *  need_des
 */
typedef union {
    struct {
        /** reg_clk_en : R/W; bitpos: [0]; default: 1;
         *  need_des
         */
        uint32_t reg_clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_sys_clkrst_clk_en0_reg_t;


/** Group: date_reg */
/** Type of date register
 *  need_des
 */
typedef union {
    struct {
        /** reg_date : R/W; bitpos: [27:0]; default: 38834448;
         *  Version control register
         */
        uint32_t reg_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} hp_sys_clkrst_date_reg_t;


typedef struct {
    volatile hp_sys_clkrst_soc_clk_sel_reg_t soc_clk_sel;
    volatile hp_sys_clkrst_cpu_freq_ctrl0_reg_t cpu_freq_ctrl0;
    volatile hp_sys_clkrst_mem_freq_ctrl0_reg_t mem_freq_ctrl0;
    volatile hp_sys_clkrst_sys_freq_ctrl0_reg_t sys_freq_ctrl0;
    volatile hp_sys_clkrst_apb_freq_ctrl0_reg_t apb_freq_ctrl0;
    volatile hp_sys_clkrst_root_clk_ctrl0_reg_t root_clk_ctrl0;
    volatile hp_sys_clkrst_cpu_wfi_delay_ctrl0_reg_t cpu_wfi_delay_ctrl0;
    volatile hp_sys_clkrst_hpcore0_ctrl0_reg_t hpcore0_ctrl0;
    volatile hp_sys_clkrst_hpcore1_ctrl0_reg_t hpcore1_ctrl0;
    volatile hp_sys_clkrst_trace_ctrl0_reg_t trace_ctrl0;
    volatile hp_sys_clkrst_tcm_ctrl0_reg_t tcm_ctrl0;
    volatile hp_sys_clkrst_tcmmon_ctrl0_reg_t tcmmon_ctrl0;
    volatile hp_sys_clkrst_psrammon_ctrl0_reg_t psrammon_ctrl0;
    volatile hp_sys_clkrst_rom_ctrl0_reg_t rom_ctrl0;
    volatile hp_sys_clkrst_cache_ctrl0_reg_t cache_ctrl0;
    volatile hp_sys_clkrst_icm_ctrl0_reg_t icm_ctrl0;
    volatile hp_sys_clkrst_modem_ctrl0_reg_t modem_ctrl0;
    volatile hp_sys_clkrst_misc_ctrl0_reg_t misc_ctrl0;
    volatile hp_sys_clkrst_busmon_ctrl0_reg_t busmon_ctrl0;
    volatile hp_sys_clkrst_remove_tmp0_reg_t remove_tmp0;
    volatile hp_sys_clkrst_pvt0_ctrl0_reg_t pvt0_ctrl0;
    volatile hp_sys_clkrst_pvt0_peri_ctrl0_reg_t pvt0_peri_ctrl0;
    volatile hp_sys_clkrst_crypto_ctrl0_reg_t crypto_ctrl0;
    volatile hp_sys_clkrst_key_manager_ctrl0_reg_t key_manager_ctrl0;
    volatile hp_sys_clkrst_dpa_ctrl0_reg_t dpa_ctrl0;
    volatile hp_sys_clkrst_flash_ctrl0_reg_t flash_ctrl0;
    volatile hp_sys_clkrst_psram_ctrl0_reg_t psram_ctrl0;
    volatile hp_sys_clkrst_gpspi2_ctrl0_reg_t gpspi2_ctrl0;
    volatile hp_sys_clkrst_gpspi3_ctrl0_reg_t gpspi3_ctrl0;
    volatile hp_sys_clkrst_gdma_ctrl0_reg_t gdma_ctrl0;
    volatile hp_sys_clkrst_axi_pdma_ctrl0_reg_t axi_pdma_ctrl0;
    volatile hp_sys_clkrst_ahb_pdma_ctrl0_reg_t ahb_pdma_ctrl0;
    volatile hp_sys_clkrst_regdma_ctrl0_reg_t regdma_ctrl0;
    volatile hp_sys_clkrst_uhci_ctrl0_reg_t uhci_ctrl0;
    volatile hp_sys_clkrst_uart0_ctrl0_reg_t uart0_ctrl0;
    volatile hp_sys_clkrst_uart1_ctrl0_reg_t uart1_ctrl0;
    volatile hp_sys_clkrst_uart2_ctrl0_reg_t uart2_ctrl0;
    volatile hp_sys_clkrst_uart3_ctrl0_reg_t uart3_ctrl0;
    volatile hp_sys_clkrst_parlio_ctrl0_reg_t parlio_ctrl0;
    volatile hp_sys_clkrst_parlio_rx_ctrl0_reg_t parlio_rx_ctrl0;
    volatile hp_sys_clkrst_parlio_tx_ctrl0_reg_t parlio_tx_ctrl0;
    volatile hp_sys_clkrst_bitscrambler_ctrl0_reg_t bitscrambler_ctrl0;
    volatile hp_sys_clkrst_etm_ctrl0_reg_t etm_ctrl0;
    volatile hp_sys_clkrst_usb_otghs_ctrl0_reg_t usb_otghs_ctrl0;
    volatile hp_sys_clkrst_dma2d_ctrl0_reg_t dma2d_ctrl0;
    volatile hp_sys_clkrst_ppa_ctrl0_reg_t ppa_ctrl0;
    volatile hp_sys_clkrst_jpeg_ctrl0_reg_t jpeg_ctrl0;
    volatile hp_sys_clkrst_rmt_ctrl0_reg_t rmt_ctrl0;
    volatile hp_sys_clkrst_sdio_host_ctrl0_reg_t sdio_host_ctrl0;
    volatile hp_sys_clkrst_sdio_host_func_ctrl0_reg_t sdio_host_func_ctrl0;
    volatile hp_sys_clkrst_emac_ctrl0_reg_t emac_ctrl0;
    volatile hp_sys_clkrst_dsi_ctrl0_reg_t dsi_ctrl0;
    volatile hp_sys_clkrst_dsi_phy_ctrl0_reg_t dsi_phy_ctrl0;
    volatile hp_sys_clkrst_dsi_dpi_ctrl0_reg_t dsi_dpi_ctrl0;
    volatile hp_sys_clkrst_mspi_pad_ctrl0_reg_t mspi_pad_ctrl0;
    volatile hp_sys_clkrst_i2c0_ctrl0_reg_t i2c0_ctrl0;
    volatile hp_sys_clkrst_i2c1_ctrl0_reg_t i2c1_ctrl0;
    volatile hp_sys_clkrst_i2s0_ctrl0_reg_t i2s0_ctrl0;
    volatile hp_sys_clkrst_i2s0_rx_ctrl0_reg_t i2s0_rx_ctrl0;
    volatile hp_sys_clkrst_i2s0_rx_div_ctrl0_reg_t i2s0_rx_div_ctrl0;
    volatile hp_sys_clkrst_i2s0_tx_ctrl0_reg_t i2s0_tx_ctrl0;
    volatile hp_sys_clkrst_i2s0_tx_div_ctrl0_reg_t i2s0_tx_div_ctrl0;
    volatile hp_sys_clkrst_i2s1_ctrl0_reg_t i2s1_ctrl0;
    volatile hp_sys_clkrst_i2s1_rx_ctrl0_reg_t i2s1_rx_ctrl0;
    volatile hp_sys_clkrst_i2s1_rx_div_ctrl0_reg_t i2s1_rx_div_ctrl0;
    volatile hp_sys_clkrst_i2s1_tx_ctrl0_reg_t i2s1_tx_ctrl0;
    volatile hp_sys_clkrst_i2s1_tx_div_ctrl0_reg_t i2s1_tx_div_ctrl0;
    volatile hp_sys_clkrst_twai0_ctrl0_reg_t twai0_ctrl0;
    volatile hp_sys_clkrst_twai1_ctrl0_reg_t twai1_ctrl0;
    volatile hp_sys_clkrst_timergrp0_ctrl0_reg_t timergrp0_ctrl0;
    volatile hp_sys_clkrst_timergrp0_tgrt_ctrl0_reg_t timergrp0_tgrt_ctrl0;
    volatile hp_sys_clkrst_timergrp1_ctrl0_reg_t timergrp1_ctrl0;
    volatile hp_sys_clkrst_systimer_ctrl0_reg_t systimer_ctrl0;
    volatile hp_sys_clkrst_mcpwm0_ctrl0_reg_t mcpwm0_ctrl0;
    volatile hp_sys_clkrst_mcpwm1_ctrl0_reg_t mcpwm1_ctrl0;
    volatile hp_sys_clkrst_mcpwm2_ctrl0_reg_t mcpwm2_ctrl0;
    volatile hp_sys_clkrst_mcpwm3_ctrl0_reg_t mcpwm3_ctrl0;
    volatile hp_sys_clkrst_intrmtx_ctrl0_reg_t intrmtx_ctrl0;
    volatile hp_sys_clkrst_pcnt_ctrl0_reg_t pcnt_ctrl0;
    uint32_t reserved_13c[2];
    volatile hp_sys_clkrst_usb_device_ctrl0_reg_t usb_device_ctrl0;
    volatile hp_sys_clkrst_ledc_ctrl0_reg_t ledc_ctrl0;
    volatile hp_sys_clkrst_lcdcam_ctrl0_reg_t lcdcam_ctrl0;
    volatile hp_sys_clkrst_lcdcam_lcd_ctrl0_reg_t lcdcam_lcd_ctrl0;
    volatile hp_sys_clkrst_lcdcam_lcdcam_ctrl0_reg_t lcdcam_lcdcam_ctrl0;
    volatile hp_sys_clkrst_lcdcam_cam_ctrl0_reg_t lcdcam_cam_ctrl0;
    volatile hp_sys_clkrst_iomux_ctrl0_reg_t iomux_ctrl0;
    volatile hp_sys_clkrst_hpwdt_core0_rst_ctrl0_reg_t hpwdt_core0_rst_ctrl0;
    volatile hp_sys_clkrst_hpwdt_core1_rst_ctrl0_reg_t hpwdt_core1_rst_ctrl0;
    volatile hp_sys_clkrst_cpu_src_freq0_reg_t cpu_src_freq0;
    volatile hp_sys_clkrst_cpu_clk_status0_reg_t cpu_clk_status0;
    volatile hp_sys_clkrst_hpcore_wdt_reset_source0_reg_t hpcore_wdt_reset_source0;
    volatile hp_sys_clkrst_ana_pll_ctrl0_reg_t ana_pll_ctrl0;
    volatile hp_sys_clkrst_ref_500m_ctrl0_reg_t ref_500m_ctrl0;
    volatile hp_sys_clkrst_ref_240m_ctrl0_reg_t ref_240m_ctrl0;
    volatile hp_sys_clkrst_ref_160m_ctrl0_reg_t ref_160m_ctrl0;
    volatile hp_sys_clkrst_ref_120m_ctrl0_reg_t ref_120m_ctrl0;
    volatile hp_sys_clkrst_ref_80m_ctrl0_reg_t ref_80m_ctrl0;
    volatile hp_sys_clkrst_ref_60m_ctrl0_reg_t ref_60m_ctrl0;
    volatile hp_sys_clkrst_ref_20m_ctrl0_reg_t ref_20m_ctrl0;
    volatile hp_sys_clkrst_ref_50m_ctrl0_reg_t ref_50m_ctrl0;
    volatile hp_sys_clkrst_ref_25m_ctrl0_reg_t ref_25m_ctrl0;
    volatile hp_sys_clkrst_tm_clk_ctrl0_reg_t tm_clk_ctrl0;
    volatile hp_sys_clkrst_dbg0_clk_ctrl0_reg_t dbg0_clk_ctrl0;
    volatile hp_sys_clkrst_dbg1_clk_ctrl0_reg_t dbg1_clk_ctrl0;
    volatile hp_sys_clkrst_dbg2_clk_ctrl0_reg_t dbg2_clk_ctrl0;
    volatile hp_sys_clkrst_lp_clk_ctrl0_reg_t lp_clk_ctrl0;
    volatile hp_sys_clkrst_ahb_asrc_ctrl0_reg_t ahb_asrc_ctrl0;
    volatile hp_sys_clkrst_cordic_ctrl0_reg_t cordic_ctrl0;
    volatile hp_sys_clkrst_zero_det_ctrl0_reg_t zero_det_ctrl0;
    volatile hp_sys_clkrst_cordic_ctrl1_reg_t cordic_ctrl1;
    uint32_t reserved_1c0[2];
    volatile hp_sys_clkrst_clk_pwr_decrease_reg_t clk_pwr_decrease;
    volatile hp_sys_clkrst_cnnt_iomux_ctrl0_reg_t cnnt_iomux_ctrl0;
    volatile hp_sys_clkrst_hp_i2cmst_ctrl0_reg_t hp_i2cmst_ctrl0;
    volatile hp_sys_clkrst_axi_perf_mon_ctrl0_reg_t axi_perf_mon_ctrl0;
    volatile hp_sys_clkrst_cnnt_sysreg_ctrl0_reg_t cnnt_sysreg_ctrl0;
    volatile hp_sys_clkrst_hp_alive_sysreg_ctrl0_reg_t hp_alive_sysreg_ctrl0;
    volatile hp_sys_clkrst_modem_conf_reg_t modem_conf;
    uint32_t reserved_1e4[5];
    volatile hp_sys_clkrst_clk_en0_reg_t clk_en0;
    volatile hp_sys_clkrst_date_reg_t date;
} hp_sys_clkrst_dev_t;

extern hp_sys_clkrst_dev_t HP_SYS_CLKRST;

#ifndef __cplusplus
_Static_assert(sizeof(hp_sys_clkrst_dev_t) == 0x200, "Invalid size of hp_sys_clkrst_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
