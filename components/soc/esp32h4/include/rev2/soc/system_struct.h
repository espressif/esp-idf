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

/** Group: Configuration Registers */
/** Type of cpu_peri_clk_en register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:6;
        /** clk_en_assist_debug : R/W; bitpos: [6]; default: 0;
         *  Need add description
         */
        uint32_t clk_en_assist_debug:1;
        /** clk_en_dedicated_gpio : R/W; bitpos: [7]; default: 0;
         *  Need add description
         */
        uint32_t clk_en_dedicated_gpio:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} system_cpu_peri_clk_en_reg_t;

/** Type of cpu_peri_rst_en register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:6;
        /** rst_en_assist_debug : R/W; bitpos: [6]; default: 1;
         *  Need add description
         */
        uint32_t rst_en_assist_debug:1;
        /** rst_en_dedicated_gpio : R/W; bitpos: [7]; default: 1;
         *  Need add description
         */
        uint32_t rst_en_dedicated_gpio:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} system_cpu_peri_rst_en_reg_t;

/** Type of cpu_per_conf register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:3;
        /** cpu_wait_mode_force_on : R/W; bitpos: [3]; default: 1;
         *  Need add description
         */
        uint32_t cpu_wait_mode_force_on:1;
        /** cpu_waiti_delay_num : R/W; bitpos: [7:4]; default: 0;
         *  Need add description
         */
        uint32_t cpu_waiti_delay_num:4;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} system_cpu_per_conf_reg_t;

/** Type of mem_pd_mask register
 *  register description
 */
typedef union {
    struct {
        /** lslp_mem_pd_mask : R/W; bitpos: [0]; default: 1;
         *  Need add description
         */
        uint32_t lslp_mem_pd_mask:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} system_mem_pd_mask_reg_t;

/** Type of cpu_intr_from_cpu_0 register
 *  register description
 */
typedef union {
    struct {
        /** cpu_intr_from_cpu_0 : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t cpu_intr_from_cpu_0:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} system_cpu_intr_from_cpu_0_reg_t;

/** Type of cpu_intr_from_cpu_1 register
 *  register description
 */
typedef union {
    struct {
        /** cpu_intr_from_cpu_1 : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t cpu_intr_from_cpu_1:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} system_cpu_intr_from_cpu_1_reg_t;

/** Type of cpu_intr_from_cpu_2 register
 *  register description
 */
typedef union {
    struct {
        /** cpu_intr_from_cpu_2 : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t cpu_intr_from_cpu_2:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} system_cpu_intr_from_cpu_2_reg_t;

/** Type of cpu_intr_from_cpu_3 register
 *  register description
 */
typedef union {
    struct {
        /** cpu_intr_from_cpu_3 : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t cpu_intr_from_cpu_3:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} system_cpu_intr_from_cpu_3_reg_t;

/** Type of rsa_pd_ctrl register
 *  register description
 */
typedef union {
    struct {
        /** rsa_mem_pd : R/W; bitpos: [0]; default: 1;
         *  Need add description
         */
        uint32_t rsa_mem_pd:1;
        /** rsa_mem_force_pu : R/W; bitpos: [1]; default: 0;
         *  Need add description
         */
        uint32_t rsa_mem_force_pu:1;
        /** rsa_mem_force_pd : R/W; bitpos: [2]; default: 0;
         *  Need add description
         */
        uint32_t rsa_mem_force_pd:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} system_rsa_pd_ctrl_reg_t;

/** Type of edma_ctrl register
 *  register description
 */
typedef union {
    struct {
        /** edma_clk_on : R/W; bitpos: [0]; default: 1;
         *  Need add description
         */
        uint32_t edma_clk_on:1;
        /** edma_reset : R/W; bitpos: [1]; default: 0;
         *  Need add description
         */
        uint32_t edma_reset:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} system_edma_ctrl_reg_t;

/** Type of cache_control register
 *  register description
 */
typedef union {
    struct {
        /** icache_clk_on : R/W; bitpos: [0]; default: 1;
         *  Need add description
         */
        uint32_t icache_clk_on:1;
        /** icache_reset : R/W; bitpos: [1]; default: 0;
         *  Need add description
         */
        uint32_t icache_reset:1;
        /** dcache_clk_on : R/W; bitpos: [2]; default: 1;
         *  Need add description
         */
        uint32_t dcache_clk_on:1;
        /** dcache_reset : R/W; bitpos: [3]; default: 0;
         *  Need add description
         */
        uint32_t dcache_reset:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} system_cache_control_reg_t;

/** Type of external_device_encrypt_decrypt_control register
 *  register description
 */
typedef union {
    struct {
        /** enable_spi_manual_encrypt : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t enable_spi_manual_encrypt:1;
        /** enable_download_db_encrypt : R/W; bitpos: [1]; default: 0;
         *  Need add description
         */
        uint32_t enable_download_db_encrypt:1;
        /** enable_download_g0cb_decrypt : R/W; bitpos: [2]; default: 0;
         *  Need add description
         */
        uint32_t enable_download_g0cb_decrypt:1;
        /** enable_download_manual_encrypt : R/W; bitpos: [3]; default: 0;
         *  Need add description
         */
        uint32_t enable_download_manual_encrypt:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} system_external_device_encrypt_decrypt_control_reg_t;

/** Type of rtc_fastmem_config register
 *  register description
 */
typedef union {
    struct {
        uint32_t reserved_0:8;
        /** rtc_mem_crc_start : R/W; bitpos: [8]; default: 0;
         *  Need add description
         */
        uint32_t rtc_mem_crc_start:1;
        /** rtc_mem_crc_addr : R/W; bitpos: [19:9]; default: 0;
         *  Need add description
         */
        uint32_t rtc_mem_crc_addr:11;
        /** rtc_mem_crc_len : R/W; bitpos: [30:20]; default: 2047;
         *  Need add description
         */
        uint32_t rtc_mem_crc_len:11;
        /** rtc_mem_crc_finish : RO; bitpos: [31]; default: 0;
         *  Need add description
         */
        uint32_t rtc_mem_crc_finish:1;
    };
    uint32_t val;
} system_rtc_fastmem_config_reg_t;

/** Type of rtc_fastmem_crc register
 *  register description
 */
typedef union {
    struct {
        /** rtc_mem_crc_res : RO; bitpos: [31:0]; default: 0;
         *  Need add description
         */
        uint32_t rtc_mem_crc_res:32;
    };
    uint32_t val;
} system_rtc_fastmem_crc_reg_t;

/** Type of redundant_eco_ctrl register
 *  register description
 */
typedef union {
    struct {
        /** redundant_eco_drive : R/W; bitpos: [0]; default: 0;
         *  Need add description
         */
        uint32_t redundant_eco_drive:1;
        /** redundant_eco_result : RO; bitpos: [1]; default: 0;
         *  Need add description
         */
        uint32_t redundant_eco_result:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} system_redundant_eco_ctrl_reg_t;

/** Type of clock_gate register
 *  register description
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 1;
         *  Need add description
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} system_clock_gate_reg_t;

/** Type of mem_pvt register
 *  register description
 */
typedef union {
    struct {
        /** mem_path_len : R/W; bitpos: [3:0]; default: 3;
         *  Need add description
         */
        uint32_t mem_path_len:4;
        /** mem_err_cnt_clr : WO; bitpos: [4]; default: 0;
         *  Need add description
         */
        uint32_t mem_err_cnt_clr:1;
        /** mem_pvt_monitor_en : R/W; bitpos: [5]; default: 0;
         *  Need add description
         */
        uint32_t mem_pvt_monitor_en:1;
        /** mem_timing_err_cnt : RO; bitpos: [21:6]; default: 0;
         *  Need add description
         */
        uint32_t mem_timing_err_cnt:16;
        /** mem_vt_sel : R/W; bitpos: [23:22]; default: 0;
         *  Need add description
         */
        uint32_t mem_vt_sel:2;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} system_mem_pvt_reg_t;

/** Type of system_reg_date register
 *  register description
 */
typedef union {
    struct {
        /** system_reg_date : R/W; bitpos: [27:0]; default: 34615872;
         *  Need add description
         */
        uint32_t system_reg_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} system_system_reg_date_reg_t;


typedef struct {
    volatile system_cpu_peri_clk_en_reg_t cpu_peri_clk_en;
    volatile system_cpu_peri_rst_en_reg_t cpu_peri_rst_en;
    volatile system_cpu_per_conf_reg_t cpu_per_conf;
    volatile system_mem_pd_mask_reg_t mem_pd_mask;
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
    volatile system_mem_pvt_reg_t mem_pvt;
    uint32_t reserved_044[1006];
    volatile system_system_reg_date_reg_t system_reg_date;
} system_dev_t;

extern system_dev_t SYSTEM;

#ifndef __cplusplus
_Static_assert(sizeof(system_dev_t) == 0x1000, "Invalid size of system_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
