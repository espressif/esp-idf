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

/** Group: clk_en */
/** Type of clk_en0 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_clk_en : R/W; bitpos: [0]; default: 1;
         *  Reserved
         */
        uint32_t reg_clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} hp_sys_clkrst_clk_en0_reg_t;


/** Group: root_clk_ctrl */
/** Type of root_clk_ctrl0 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_cpuicm_delay_num : R/W; bitpos: [3:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_cpuicm_delay_num:4;
        /** reg_soc_clk_div_update : WT; bitpos: [4]; default: 0;
         *  Reserved
         */
        uint32_t reg_soc_clk_div_update:1;
        /** reg_cpu_clk_div_num : R/W; bitpos: [12:5]; default: 0;
         *  Reserved
         */
        uint32_t reg_cpu_clk_div_num:8;
        /** reg_cpu_clk_div_numerator : R/W; bitpos: [20:13]; default: 0;
         *  Reserved
         */
        uint32_t reg_cpu_clk_div_numerator:8;
        /** reg_cpu_clk_div_denominator : R/W; bitpos: [28:21]; default: 0;
         *  Reserved
         */
        uint32_t reg_cpu_clk_div_denominator:8;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} hp_sys_clkrst_root_clk_ctrl0_reg_t;

/** Type of root_clk_ctrl1 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_mem_clk_div_num : R/W; bitpos: [7:0]; default: 1;
         *  Reserved
         */
        uint32_t reg_mem_clk_div_num:8;
        /** reg_mem_clk_div_numerator : R/W; bitpos: [15:8]; default: 0;
         *  Reserved
         */
        uint32_t reg_mem_clk_div_numerator:8;
        /** reg_mem_clk_div_denominator : R/W; bitpos: [23:16]; default: 0;
         *  Reserved
         */
        uint32_t reg_mem_clk_div_denominator:8;
        /** reg_sys_clk_div_num : R/W; bitpos: [31:24]; default: 0;
         *  Reserved
         */
        uint32_t reg_sys_clk_div_num:8;
    };
    uint32_t val;
} hp_sys_clkrst_root_clk_ctrl1_reg_t;

/** Type of root_clk_ctrl2 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_sys_clk_div_numerator : R/W; bitpos: [7:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_sys_clk_div_numerator:8;
        /** reg_sys_clk_div_denominator : R/W; bitpos: [15:8]; default: 0;
         *  Reserved
         */
        uint32_t reg_sys_clk_div_denominator:8;
        /** reg_apb_clk_div_num : R/W; bitpos: [23:16]; default: 1;
         *  Reserved
         */
        uint32_t reg_apb_clk_div_num:8;
        /** reg_apb_clk_div_numerator : R/W; bitpos: [31:24]; default: 0;
         *  Reserved
         */
        uint32_t reg_apb_clk_div_numerator:8;
    };
    uint32_t val;
} hp_sys_clkrst_root_clk_ctrl2_reg_t;

/** Type of root_clk_ctrl3 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_apb_clk_div_denominator : R/W; bitpos: [7:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_apb_clk_div_denominator:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} hp_sys_clkrst_root_clk_ctrl3_reg_t;


/** Group: soc_clk_ctrl */
/** Type of soc_clk_ctrl0 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_core0_clic_clk_en : R/W; bitpos: [0]; default: 1;
         *  Reserved
         */
        uint32_t reg_core0_clic_clk_en:1;
        /** reg_core1_clic_clk_en : R/W; bitpos: [1]; default: 1;
         *  Reserved
         */
        uint32_t reg_core1_clic_clk_en:1;
        /** reg_misc_cpu_clk_en : R/W; bitpos: [2]; default: 1;
         *  Reserved
         */
        uint32_t reg_misc_cpu_clk_en:1;
        /** reg_core0_cpu_clk_en : R/W; bitpos: [3]; default: 1;
         *  Reserved
         */
        uint32_t reg_core0_cpu_clk_en:1;
        /** reg_core1_cpu_clk_en : R/W; bitpos: [4]; default: 0;
         *  Reserved
         */
        uint32_t reg_core1_cpu_clk_en:1;
        /** reg_tcm_cpu_clk_en : R/W; bitpos: [5]; default: 1;
         *  Reserved
         */
        uint32_t reg_tcm_cpu_clk_en:1;
        /** reg_busmon_cpu_clk_en : R/W; bitpos: [6]; default: 0;
         *  Reserved
         */
        uint32_t reg_busmon_cpu_clk_en:1;
        /** reg_l1cache_cpu_clk_en : R/W; bitpos: [7]; default: 1;
         *  Reserved
         */
        uint32_t reg_l1cache_cpu_clk_en:1;
        /** reg_l1cache_d_cpu_clk_en : R/W; bitpos: [8]; default: 1;
         *  Reserved
         */
        uint32_t reg_l1cache_d_cpu_clk_en:1;
        /** reg_l1cache_i0_cpu_clk_en : R/W; bitpos: [9]; default: 1;
         *  Reserved
         */
        uint32_t reg_l1cache_i0_cpu_clk_en:1;
        /** reg_l1cache_i1_cpu_clk_en : R/W; bitpos: [10]; default: 1;
         *  Reserved
         */
        uint32_t reg_l1cache_i1_cpu_clk_en:1;
        /** reg_trace_cpu_clk_en : R/W; bitpos: [11]; default: 0;
         *  Reserved
         */
        uint32_t reg_trace_cpu_clk_en:1;
        /** reg_icm_cpu_clk_en : R/W; bitpos: [12]; default: 1;
         *  Reserved
         */
        uint32_t reg_icm_cpu_clk_en:1;
        /** reg_gdma_cpu_clk_en : R/W; bitpos: [13]; default: 0;
         *  Reserved
         */
        uint32_t reg_gdma_cpu_clk_en:1;
        /** reg_vpu_cpu_clk_en : R/W; bitpos: [14]; default: 0;
         *  Reserved
         */
        uint32_t reg_vpu_cpu_clk_en:1;
        /** reg_l1cache_mem_clk_en : R/W; bitpos: [15]; default: 1;
         *  Reserved
         */
        uint32_t reg_l1cache_mem_clk_en:1;
        /** reg_l1cache_d_mem_clk_en : R/W; bitpos: [16]; default: 1;
         *  Reserved
         */
        uint32_t reg_l1cache_d_mem_clk_en:1;
        /** reg_l1cache_i0_mem_clk_en : R/W; bitpos: [17]; default: 1;
         *  Reserved
         */
        uint32_t reg_l1cache_i0_mem_clk_en:1;
        /** reg_l1cache_i1_mem_clk_en : R/W; bitpos: [18]; default: 1;
         *  Reserved
         */
        uint32_t reg_l1cache_i1_mem_clk_en:1;
        /** reg_l2cache_mem_clk_en : R/W; bitpos: [19]; default: 1;
         *  Reserved
         */
        uint32_t reg_l2cache_mem_clk_en:1;
        /** reg_l2mem_mem_clk_en : R/W; bitpos: [20]; default: 1;
         *  Reserved
         */
        uint32_t reg_l2mem_mem_clk_en:1;
        /** reg_l2memmon_mem_clk_en : R/W; bitpos: [21]; default: 0;
         *  Reserved
         */
        uint32_t reg_l2memmon_mem_clk_en:1;
        /** reg_icm_mem_clk_en : R/W; bitpos: [22]; default: 1;
         *  Reserved
         */
        uint32_t reg_icm_mem_clk_en:1;
        /** reg_misc_sys_clk_en : R/W; bitpos: [23]; default: 1;
         *  Reserved
         */
        uint32_t reg_misc_sys_clk_en:1;
        /** reg_trace_sys_clk_en : R/W; bitpos: [24]; default: 0;
         *  Reserved
         */
        uint32_t reg_trace_sys_clk_en:1;
        /** reg_l2cache_sys_clk_en : R/W; bitpos: [25]; default: 1;
         *  Reserved
         */
        uint32_t reg_l2cache_sys_clk_en:1;
        /** reg_l2mem_sys_clk_en : R/W; bitpos: [26]; default: 1;
         *  Reserved
         */
        uint32_t reg_l2mem_sys_clk_en:1;
        /** reg_l2memmon_sys_clk_en : R/W; bitpos: [27]; default: 0;
         *  Reserved
         */
        uint32_t reg_l2memmon_sys_clk_en:1;
        /** reg_tcmmon_sys_clk_en : R/W; bitpos: [28]; default: 0;
         *  Reserved
         */
        uint32_t reg_tcmmon_sys_clk_en:1;
        /** reg_icm_sys_clk_en : R/W; bitpos: [29]; default: 1;
         *  Reserved
         */
        uint32_t reg_icm_sys_clk_en:1;
        /** reg_flash_sys_clk_en : R/W; bitpos: [30]; default: 1;
         *  Reserved
         */
        uint32_t reg_flash_sys_clk_en:1;
        /** reg_psram_sys_clk_en : R/W; bitpos: [31]; default: 1;
         *  Reserved
         */
        uint32_t reg_psram_sys_clk_en:1;
    };
    uint32_t val;
} hp_sys_clkrst_soc_clk_ctrl0_reg_t;

/** Type of soc_clk_ctrl1 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpspi2_sys_clk_en : R/W; bitpos: [0]; default: 1;
         *  Reserved
         */
        uint32_t reg_gpspi2_sys_clk_en:1;
        /** reg_gpspi3_sys_clk_en : R/W; bitpos: [1]; default: 1;
         *  Reserved
         */
        uint32_t reg_gpspi3_sys_clk_en:1;
        /** reg_regdma_sys_clk_en : R/W; bitpos: [2]; default: 1;
         *  Reserved
         */
        uint32_t reg_regdma_sys_clk_en:1;
        /** reg_ahb_pdma_sys_clk_en : R/W; bitpos: [3]; default: 1;
         *  Reserved
         */
        uint32_t reg_ahb_pdma_sys_clk_en:1;
        /** reg_axi_pdma_sys_clk_en : R/W; bitpos: [4]; default: 1;
         *  Reserved
         */
        uint32_t reg_axi_pdma_sys_clk_en:1;
        /** reg_gdma_sys_clk_en : R/W; bitpos: [5]; default: 0;
         *  Reserved
         */
        uint32_t reg_gdma_sys_clk_en:1;
        /** reg_dma2d_sys_clk_en : R/W; bitpos: [6]; default: 0;
         *  Reserved
         */
        uint32_t reg_dma2d_sys_clk_en:1;
        /** reg_vpu_sys_clk_en : R/W; bitpos: [7]; default: 0;
         *  Reserved
         */
        uint32_t reg_vpu_sys_clk_en:1;
        /** reg_jpeg_sys_clk_en : R/W; bitpos: [8]; default: 0;
         *  Reserved
         */
        uint32_t reg_jpeg_sys_clk_en:1;
        /** reg_ppa_sys_clk_en : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_ppa_sys_clk_en:1;
        /** reg_csi_brg_sys_clk_en : R/W; bitpos: [10]; default: 0;
         *  Reserved
         */
        uint32_t reg_csi_brg_sys_clk_en:1;
        /** reg_csi_host_sys_clk_en : R/W; bitpos: [11]; default: 0;
         *  Reserved
         */
        uint32_t reg_csi_host_sys_clk_en:1;
        /** reg_dsi_sys_clk_en : R/W; bitpos: [12]; default: 0;
         *  Reserved
         */
        uint32_t reg_dsi_sys_clk_en:1;
        /** reg_emac_sys_clk_en : R/W; bitpos: [13]; default: 0;
         *  Reserved
         */
        uint32_t reg_emac_sys_clk_en:1;
        /** reg_sdmmc_sys_clk_en : R/W; bitpos: [14]; default: 0;
         *  Reserved
         */
        uint32_t reg_sdmmc_sys_clk_en:1;
        /** reg_usb_otg11_sys_clk_en : R/W; bitpos: [15]; default: 1;
         *  Reserved
         */
        uint32_t reg_usb_otg11_sys_clk_en:1;
        /** reg_usb_otg20_sys_clk_en : R/W; bitpos: [16]; default: 1;
         *  Reserved
         */
        uint32_t reg_usb_otg20_sys_clk_en:1;
        /** reg_uhci_sys_clk_en : R/W; bitpos: [17]; default: 1;
         *  Reserved
         */
        uint32_t reg_uhci_sys_clk_en:1;
        /** reg_uart0_sys_clk_en : R/W; bitpos: [18]; default: 1;
         *  Reserved
         */
        uint32_t reg_uart0_sys_clk_en:1;
        /** reg_uart1_sys_clk_en : R/W; bitpos: [19]; default: 1;
         *  Reserved
         */
        uint32_t reg_uart1_sys_clk_en:1;
        /** reg_uart2_sys_clk_en : R/W; bitpos: [20]; default: 1;
         *  Reserved
         */
        uint32_t reg_uart2_sys_clk_en:1;
        /** reg_uart3_sys_clk_en : R/W; bitpos: [21]; default: 1;
         *  Reserved
         */
        uint32_t reg_uart3_sys_clk_en:1;
        /** reg_uart4_sys_clk_en : R/W; bitpos: [22]; default: 1;
         *  Reserved
         */
        uint32_t reg_uart4_sys_clk_en:1;
        /** reg_parlio_sys_clk_en : R/W; bitpos: [23]; default: 0;
         *  Reserved
         */
        uint32_t reg_parlio_sys_clk_en:1;
        /** reg_etm_sys_clk_en : R/W; bitpos: [24]; default: 0;
         *  Reserved
         */
        uint32_t reg_etm_sys_clk_en:1;
        /** reg_pvt_sys_clk_en : R/W; bitpos: [25]; default: 0;
         *  Reserved
         */
        uint32_t reg_pvt_sys_clk_en:1;
        /** reg_crypto_sys_clk_en : R/W; bitpos: [26]; default: 1;
         *  Reserved
         */
        uint32_t reg_crypto_sys_clk_en:1;
        /** reg_key_manager_sys_clk_en : R/W; bitpos: [27]; default: 1;
         *  Reserved
         */
        uint32_t reg_key_manager_sys_clk_en:1;
        /** reg_bitsrambler_sys_clk_en : R/W; bitpos: [28]; default: 1;
         *  Reserved
         */
        uint32_t reg_bitsrambler_sys_clk_en:1;
        /** reg_bitsrambler_rx_sys_clk_en : R/W; bitpos: [29]; default: 1;
         *  Reserved
         */
        uint32_t reg_bitsrambler_rx_sys_clk_en:1;
        /** reg_bitsrambler_tx_sys_clk_en : R/W; bitpos: [30]; default: 1;
         *  Reserved
         */
        uint32_t reg_bitsrambler_tx_sys_clk_en:1;
        /** reg_h264_sys_clk_en : R/W; bitpos: [31]; default: 0;
         *  Reserved
         */
        uint32_t reg_h264_sys_clk_en:1;
    };
    uint32_t val;
} hp_sys_clkrst_soc_clk_ctrl1_reg_t;

/** Type of soc_clk_ctrl2 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_rmt_sys_clk_en : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_rmt_sys_clk_en:1;
        /** reg_hp_clkrst_apb_clk_en : R/W; bitpos: [1]; default: 1;
         *  Reserved
         */
        uint32_t reg_hp_clkrst_apb_clk_en:1;
        /** reg_sysreg_apb_clk_en : R/W; bitpos: [2]; default: 1;
         *  Reserved
         */
        uint32_t reg_sysreg_apb_clk_en:1;
        /** reg_icm_apb_clk_en : R/W; bitpos: [3]; default: 1;
         *  Reserved
         */
        uint32_t reg_icm_apb_clk_en:1;
        /** reg_intrmtx_apb_clk_en : R/W; bitpos: [4]; default: 1;
         *  Reserved
         */
        uint32_t reg_intrmtx_apb_clk_en:1;
        /** reg_adc_apb_clk_en : R/W; bitpos: [5]; default: 0;
         *  Reserved
         */
        uint32_t reg_adc_apb_clk_en:1;
        /** reg_uhci_apb_clk_en : R/W; bitpos: [6]; default: 1;
         *  Reserved
         */
        uint32_t reg_uhci_apb_clk_en:1;
        /** reg_uart0_apb_clk_en : R/W; bitpos: [7]; default: 1;
         *  Reserved
         */
        uint32_t reg_uart0_apb_clk_en:1;
        /** reg_uart1_apb_clk_en : R/W; bitpos: [8]; default: 1;
         *  Reserved
         */
        uint32_t reg_uart1_apb_clk_en:1;
        /** reg_uart2_apb_clk_en : R/W; bitpos: [9]; default: 1;
         *  Reserved
         */
        uint32_t reg_uart2_apb_clk_en:1;
        /** reg_uart3_apb_clk_en : R/W; bitpos: [10]; default: 1;
         *  Reserved
         */
        uint32_t reg_uart3_apb_clk_en:1;
        /** reg_uart4_apb_clk_en : R/W; bitpos: [11]; default: 1;
         *  Reserved
         */
        uint32_t reg_uart4_apb_clk_en:1;
        /** reg_i2c0_apb_clk_en : R/W; bitpos: [12]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2c0_apb_clk_en:1;
        /** reg_i2c1_apb_clk_en : R/W; bitpos: [13]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2c1_apb_clk_en:1;
        /** reg_i2s0_apb_clk_en : R/W; bitpos: [14]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s0_apb_clk_en:1;
        /** reg_i2s1_apb_clk_en : R/W; bitpos: [15]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s1_apb_clk_en:1;
        /** reg_i2s2_apb_clk_en : R/W; bitpos: [16]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s2_apb_clk_en:1;
        /** reg_i3c_mst_apb_clk_en : R/W; bitpos: [17]; default: 0;
         *  Reserved
         */
        uint32_t reg_i3c_mst_apb_clk_en:1;
        /** reg_i3c_slv_apb_clk_en : R/W; bitpos: [18]; default: 0;
         *  Reserved
         */
        uint32_t reg_i3c_slv_apb_clk_en:1;
        /** reg_gpspi2_apb_clk_en : R/W; bitpos: [19]; default: 1;
         *  Reserved
         */
        uint32_t reg_gpspi2_apb_clk_en:1;
        /** reg_gpspi3_apb_clk_en : R/W; bitpos: [20]; default: 1;
         *  Reserved
         */
        uint32_t reg_gpspi3_apb_clk_en:1;
        /** reg_timergrp0_apb_clk_en : R/W; bitpos: [21]; default: 1;
         *  Reserved
         */
        uint32_t reg_timergrp0_apb_clk_en:1;
        /** reg_timergrp1_apb_clk_en : R/W; bitpos: [22]; default: 1;
         *  Reserved
         */
        uint32_t reg_timergrp1_apb_clk_en:1;
        /** reg_systimer_apb_clk_en : R/W; bitpos: [23]; default: 1;
         *  Reserved
         */
        uint32_t reg_systimer_apb_clk_en:1;
        /** reg_twai0_apb_clk_en : R/W; bitpos: [24]; default: 0;
         *  Reserved
         */
        uint32_t reg_twai0_apb_clk_en:1;
        /** reg_twai1_apb_clk_en : R/W; bitpos: [25]; default: 0;
         *  Reserved
         */
        uint32_t reg_twai1_apb_clk_en:1;
        /** reg_twai2_apb_clk_en : R/W; bitpos: [26]; default: 0;
         *  Reserved
         */
        uint32_t reg_twai2_apb_clk_en:1;
        /** reg_mcpwm0_apb_clk_en : R/W; bitpos: [27]; default: 0;
         *  Reserved
         */
        uint32_t reg_mcpwm0_apb_clk_en:1;
        /** reg_mcpwm1_apb_clk_en : R/W; bitpos: [28]; default: 0;
         *  Reserved
         */
        uint32_t reg_mcpwm1_apb_clk_en:1;
        /** reg_usb_device_apb_clk_en : R/W; bitpos: [29]; default: 1;
         *  Reserved
         */
        uint32_t reg_usb_device_apb_clk_en:1;
        /** reg_pcnt_apb_clk_en : R/W; bitpos: [30]; default: 0;
         *  Reserved
         */
        uint32_t reg_pcnt_apb_clk_en:1;
        /** reg_parlio_apb_clk_en : R/W; bitpos: [31]; default: 0;
         *  Reserved
         */
        uint32_t reg_parlio_apb_clk_en:1;
    };
    uint32_t val;
} hp_sys_clkrst_soc_clk_ctrl2_reg_t;

/** Type of soc_clk_ctrl3 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_ledc_apb_clk_en : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_ledc_apb_clk_en:1;
        /** reg_lcdcam_apb_clk_en : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_lcdcam_apb_clk_en:1;
        /** reg_etm_apb_clk_en : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_etm_apb_clk_en:1;
        /** reg_iomux_apb_clk_en : R/W; bitpos: [3]; default: 1;
         *  Reserved
         */
        uint32_t reg_iomux_apb_clk_en:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} hp_sys_clkrst_soc_clk_ctrl3_reg_t;


/** Group: ref_clk_ctrl */
/** Type of ref_clk_ctrl0 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_ref_50m_clk_div_num : R/W; bitpos: [7:0]; default: 9;
         *  Reserved
         */
        uint32_t reg_ref_50m_clk_div_num:8;
        /** reg_ref_25m_clk_div_num : R/W; bitpos: [15:8]; default: 19;
         *  Reserved
         */
        uint32_t reg_ref_25m_clk_div_num:8;
        /** reg_ref_240m_clk_div_num : R/W; bitpos: [23:16]; default: 1;
         *  Reserved
         */
        uint32_t reg_ref_240m_clk_div_num:8;
        /** reg_ref_160m_clk_div_num : R/W; bitpos: [31:24]; default: 2;
         *  Reserved
         */
        uint32_t reg_ref_160m_clk_div_num:8;
    };
    uint32_t val;
} hp_sys_clkrst_ref_clk_ctrl0_reg_t;

/** Type of ref_clk_ctrl1 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_ref_120m_clk_div_num : R/W; bitpos: [7:0]; default: 3;
         *  Reserved
         */
        uint32_t reg_ref_120m_clk_div_num:8;
        /** reg_ref_80m_clk_div_num : R/W; bitpos: [15:8]; default: 5;
         *  Reserved
         */
        uint32_t reg_ref_80m_clk_div_num:8;
        /** reg_ref_20m_clk_div_num : R/W; bitpos: [23:16]; default: 23;
         *  Reserved
         */
        uint32_t reg_ref_20m_clk_div_num:8;
        /** reg_tm_400m_clk_en : R/W; bitpos: [24]; default: 0;
         *  Reserved
         */
        uint32_t reg_tm_400m_clk_en:1;
        /** reg_tm_200m_clk_en : R/W; bitpos: [25]; default: 0;
         *  Reserved
         */
        uint32_t reg_tm_200m_clk_en:1;
        /** reg_tm_100m_clk_en : R/W; bitpos: [26]; default: 0;
         *  Reserved
         */
        uint32_t reg_tm_100m_clk_en:1;
        /** reg_ref_50m_clk_en : R/W; bitpos: [27]; default: 1;
         *  Reserved
         */
        uint32_t reg_ref_50m_clk_en:1;
        /** reg_ref_25m_clk_en : R/W; bitpos: [28]; default: 1;
         *  Reserved
         */
        uint32_t reg_ref_25m_clk_en:1;
        /** reg_tm_480m_clk_en : R/W; bitpos: [29]; default: 0;
         *  Reserved
         */
        uint32_t reg_tm_480m_clk_en:1;
        /** reg_ref_240m_clk_en : R/W; bitpos: [30]; default: 1;
         *  Reserved
         */
        uint32_t reg_ref_240m_clk_en:1;
        /** reg_tm_240m_clk_en : R/W; bitpos: [31]; default: 0;
         *  Reserved
         */
        uint32_t reg_tm_240m_clk_en:1;
    };
    uint32_t val;
} hp_sys_clkrst_ref_clk_ctrl1_reg_t;

/** Type of ref_clk_ctrl2 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_ref_160m_clk_en : R/W; bitpos: [0]; default: 1;
         *  Reserved
         */
        uint32_t reg_ref_160m_clk_en:1;
        /** reg_tm_160m_clk_en : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_tm_160m_clk_en:1;
        /** reg_ref_120m_clk_en : R/W; bitpos: [2]; default: 1;
         *  Reserved
         */
        uint32_t reg_ref_120m_clk_en:1;
        /** reg_tm_120m_clk_en : R/W; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t reg_tm_120m_clk_en:1;
        /** reg_ref_80m_clk_en : R/W; bitpos: [4]; default: 1;
         *  Reserved
         */
        uint32_t reg_ref_80m_clk_en:1;
        /** reg_tm_80m_clk_en : R/W; bitpos: [5]; default: 0;
         *  Reserved
         */
        uint32_t reg_tm_80m_clk_en:1;
        /** reg_tm_60m_clk_en : R/W; bitpos: [6]; default: 0;
         *  Reserved
         */
        uint32_t reg_tm_60m_clk_en:1;
        /** reg_tm_48m_clk_en : R/W; bitpos: [7]; default: 0;
         *  Reserved
         */
        uint32_t reg_tm_48m_clk_en:1;
        /** reg_ref_20m_clk_en : R/W; bitpos: [8]; default: 1;
         *  Reserved
         */
        uint32_t reg_ref_20m_clk_en:1;
        /** reg_tm_20m_clk_en : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_tm_20m_clk_en:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} hp_sys_clkrst_ref_clk_ctrl2_reg_t;


/** Group: peri_clk_ctrl0 */
/** Type of peri_clk_ctrl00 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_flash_clk_src_sel : R/W; bitpos: [1:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_flash_clk_src_sel:2;
        /** reg_flash_pll_clk_en : R/W; bitpos: [2]; default: 1;
         *  Reserved
         */
        uint32_t reg_flash_pll_clk_en:1;
        /** reg_flash_core_clk_en : R/W; bitpos: [3]; default: 1;
         *  Reserved
         */
        uint32_t reg_flash_core_clk_en:1;
        /** reg_flash_core_clk_div_num : R/W; bitpos: [11:4]; default: 3;
         *  Reserved
         */
        uint32_t reg_flash_core_clk_div_num:8;
        /** reg_psram_clk_src_sel : R/W; bitpos: [13:12]; default: 0;
         *  Reserved
         */
        uint32_t reg_psram_clk_src_sel:2;
        /** reg_psram_pll_clk_en : R/W; bitpos: [14]; default: 1;
         *  Reserved
         */
        uint32_t reg_psram_pll_clk_en:1;
        /** reg_psram_core_clk_en : R/W; bitpos: [15]; default: 1;
         *  Reserved
         */
        uint32_t reg_psram_core_clk_en:1;
        /** reg_psram_core_clk_div_num : R/W; bitpos: [23:16]; default: 0;
         *  Reserved
         */
        uint32_t reg_psram_core_clk_div_num:8;
        /** reg_pad_emac_ref_clk_en : R/W; bitpos: [24]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad_emac_ref_clk_en:1;
        /** reg_emac_rmii_clk_src_sel : R/W; bitpos: [26:25]; default: 0;
         *  Reserved
         */
        uint32_t reg_emac_rmii_clk_src_sel:2;
        /** reg_emac_rmii_clk_en : R/W; bitpos: [27]; default: 0;
         *  Reserved
         */
        uint32_t reg_emac_rmii_clk_en:1;
        /** reg_emac_rx_clk_src_sel : R/W; bitpos: [28]; default: 0;
         *  Reserved
         */
        uint32_t reg_emac_rx_clk_src_sel:1;
        /** reg_emac_rx_clk_en : R/W; bitpos: [29]; default: 0;
         *  Reserved
         */
        uint32_t reg_emac_rx_clk_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl00_reg_t;

/** Type of peri_clk_ctrl01 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_emac_rx_clk_div_num : R/W; bitpos: [7:0]; default: 1;
         *  Reserved
         */
        uint32_t reg_emac_rx_clk_div_num:8;
        /** reg_emac_tx_clk_src_sel : R/W; bitpos: [8]; default: 0;
         *  Reserved
         */
        uint32_t reg_emac_tx_clk_src_sel:1;
        /** reg_emac_tx_clk_en : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_emac_tx_clk_en:1;
        /** reg_emac_tx_clk_div_num : R/W; bitpos: [17:10]; default: 1;
         *  Reserved
         */
        uint32_t reg_emac_tx_clk_div_num:8;
        /** reg_emac_ptp_ref_clk_src_sel : R/W; bitpos: [18]; default: 0;
         *  Reserved
         */
        uint32_t reg_emac_ptp_ref_clk_src_sel:1;
        /** reg_emac_ptp_ref_clk_en : R/W; bitpos: [19]; default: 0;
         *  Reserved
         */
        uint32_t reg_emac_ptp_ref_clk_en:1;
        /** reg_emac_unused0_clk_en : R/W; bitpos: [20]; default: 0;
         *  Reserved
         */
        uint32_t reg_emac_unused0_clk_en:1;
        /** reg_emac_unused1_clk_en : R/W; bitpos: [21]; default: 0;
         *  Reserved
         */
        uint32_t reg_emac_unused1_clk_en:1;
        /** reg_sdio_hs_mode : R/W; bitpos: [22]; default: 0;
         *  Reserved
         */
        uint32_t reg_sdio_hs_mode:1;
        /** reg_sdio_ls_clk_src_sel : R/W; bitpos: [23]; default: 0;
         *  Reserved
         */
        uint32_t reg_sdio_ls_clk_src_sel:1;
        /** reg_sdio_ls_clk_en : R/W; bitpos: [24]; default: 0;
         *  Reserved
         */
        uint32_t reg_sdio_ls_clk_en:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl01_reg_t;

/** Type of peri_clk_ctrl02 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_sdio_ls_clk_div_num : R/W; bitpos: [7:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_sdio_ls_clk_div_num:8;
        /** reg_sdio_ls_clk_edge_cfg_update : WT; bitpos: [8]; default: 0;
         *  Reserved
         */
        uint32_t reg_sdio_ls_clk_edge_cfg_update:1;
        /** reg_sdio_ls_clk_edge_l : R/W; bitpos: [12:9]; default: 0;
         *  Reserved
         */
        uint32_t reg_sdio_ls_clk_edge_l:4;
        /** reg_sdio_ls_clk_edge_h : R/W; bitpos: [16:13]; default: 0;
         *  Reserved
         */
        uint32_t reg_sdio_ls_clk_edge_h:4;
        /** reg_sdio_ls_clk_edge_n : R/W; bitpos: [20:17]; default: 0;
         *  Reserved
         */
        uint32_t reg_sdio_ls_clk_edge_n:4;
        /** reg_sdio_ls_slf_clk_edge_sel : R/W; bitpos: [22:21]; default: 0;
         *  Reserved
         */
        uint32_t reg_sdio_ls_slf_clk_edge_sel:2;
        /** reg_sdio_ls_drv_clk_edge_sel : R/W; bitpos: [24:23]; default: 0;
         *  Reserved
         */
        uint32_t reg_sdio_ls_drv_clk_edge_sel:2;
        /** reg_sdio_ls_sam_clk_edge_sel : R/W; bitpos: [26:25]; default: 0;
         *  Reserved
         */
        uint32_t reg_sdio_ls_sam_clk_edge_sel:2;
        /** reg_sdio_ls_slf_clk_en : R/W; bitpos: [27]; default: 0;
         *  Reserved
         */
        uint32_t reg_sdio_ls_slf_clk_en:1;
        /** reg_sdio_ls_drv_clk_en : R/W; bitpos: [28]; default: 0;
         *  Reserved
         */
        uint32_t reg_sdio_ls_drv_clk_en:1;
        /** reg_sdio_ls_sam_clk_en : R/W; bitpos: [29]; default: 0;
         *  Reserved
         */
        uint32_t reg_sdio_ls_sam_clk_en:1;
        /** reg_mipi_dsi_dphy_clk_src_sel : R/W; bitpos: [31:30]; default: 0;
         *  Reserved
         */
        uint32_t reg_mipi_dsi_dphy_clk_src_sel:2;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl02_reg_t;

/** Type of peri_clk_ctrl03 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_mipi_dsi_dphy_cfg_clk_en : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_mipi_dsi_dphy_cfg_clk_en:1;
        /** reg_mipi_dsi_dphy_pll_refclk_en : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_mipi_dsi_dphy_pll_refclk_en:1;
        /** reg_mipi_csi_dphy_clk_src_sel : R/W; bitpos: [3:2]; default: 0;
         *  Reserved
         */
        uint32_t reg_mipi_csi_dphy_clk_src_sel:2;
        /** reg_mipi_csi_dphy_cfg_clk_en : R/W; bitpos: [4]; default: 0;
         *  Reserved
         */
        uint32_t reg_mipi_csi_dphy_cfg_clk_en:1;
        /** reg_mipi_dsi_dpiclk_src_sel : R/W; bitpos: [6:5]; default: 0;
         *  Reserved
         */
        uint32_t reg_mipi_dsi_dpiclk_src_sel:2;
        /** reg_mipi_dsi_dpiclk_en : R/W; bitpos: [7]; default: 0;
         *  Reserved
         */
        uint32_t reg_mipi_dsi_dpiclk_en:1;
        /** reg_mipi_dsi_dpiclk_div_num : R/W; bitpos: [15:8]; default: 0;
         *  Reserved
         */
        uint32_t reg_mipi_dsi_dpiclk_div_num:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl03_reg_t;


/** Group: peri_clk_ctrl1 */
/** Type of peri_clk_ctrl10 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_i2c0_clk_src_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2c0_clk_src_sel:1;
        /** reg_i2c0_clk_en : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2c0_clk_en:1;
        /** reg_i2c0_clk_div_num : R/W; bitpos: [9:2]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2c0_clk_div_num:8;
        /** reg_i2c0_clk_div_numerator : R/W; bitpos: [17:10]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2c0_clk_div_numerator:8;
        /** reg_i2c0_clk_div_denominator : R/W; bitpos: [25:18]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2c0_clk_div_denominator:8;
        /** reg_i2c1_clk_src_sel : R/W; bitpos: [26]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2c1_clk_src_sel:1;
        /** reg_i2c1_clk_en : R/W; bitpos: [27]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2c1_clk_en:1;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl10_reg_t;

/** Type of peri_clk_ctrl11 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_i2c1_clk_div_num : R/W; bitpos: [7:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2c1_clk_div_num:8;
        /** reg_i2c1_clk_div_numerator : R/W; bitpos: [15:8]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2c1_clk_div_numerator:8;
        /** reg_i2c1_clk_div_denominator : R/W; bitpos: [23:16]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2c1_clk_div_denominator:8;
        /** reg_i2s0_rx_clk_en : R/W; bitpos: [24]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s0_rx_clk_en:1;
        /** reg_i2s0_rx_clk_src_sel : R/W; bitpos: [26:25]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s0_rx_clk_src_sel:2;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl11_reg_t;

/** Type of peri_clk_ctrl12 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_i2s0_rx_div_n : R/W; bitpos: [7:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s0_rx_div_n:8;
        /** reg_i2s0_rx_div_x : R/W; bitpos: [16:8]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s0_rx_div_x:9;
        /** reg_i2s0_rx_div_y : R/W; bitpos: [25:17]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s0_rx_div_y:9;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl12_reg_t;

/** Type of peri_clk_ctrl13 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_i2s0_rx_div_z : R/W; bitpos: [8:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s0_rx_div_z:9;
        /** reg_i2s0_rx_div_yn1 : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s0_rx_div_yn1:1;
        /** reg_i2s0_tx_clk_en : R/W; bitpos: [10]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s0_tx_clk_en:1;
        /** reg_i2s0_tx_clk_src_sel : R/W; bitpos: [12:11]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s0_tx_clk_src_sel:2;
        /** reg_i2s0_tx_div_n : R/W; bitpos: [20:13]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s0_tx_div_n:8;
        /** reg_i2s0_tx_div_x : R/W; bitpos: [29:21]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s0_tx_div_x:9;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl13_reg_t;

/** Type of peri_clk_ctrl14 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_i2s0_tx_div_y : R/W; bitpos: [8:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s0_tx_div_y:9;
        /** reg_i2s0_tx_div_z : R/W; bitpos: [17:9]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s0_tx_div_z:9;
        /** reg_i2s0_tx_div_yn1 : R/W; bitpos: [18]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s0_tx_div_yn1:1;
        /** reg_i2s0_mst_clk_sel : R/W; bitpos: [19]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s0_mst_clk_sel:1;
        /** reg_i2s1_rx_clk_en : R/W; bitpos: [20]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s1_rx_clk_en:1;
        /** reg_i2s1_rx_clk_src_sel : R/W; bitpos: [22:21]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s1_rx_clk_src_sel:2;
        /** reg_i2s1_rx_div_n : R/W; bitpos: [30:23]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s1_rx_div_n:8;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl14_reg_t;

/** Type of peri_clk_ctrl15 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_i2s1_rx_div_x : R/W; bitpos: [8:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s1_rx_div_x:9;
        /** reg_i2s1_rx_div_y : R/W; bitpos: [17:9]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s1_rx_div_y:9;
        /** reg_i2s1_rx_div_z : R/W; bitpos: [26:18]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s1_rx_div_z:9;
        /** reg_i2s1_rx_div_yn1 : R/W; bitpos: [27]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s1_rx_div_yn1:1;
        /** reg_i2s1_tx_clk_en : R/W; bitpos: [28]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s1_tx_clk_en:1;
        /** reg_i2s1_tx_clk_src_sel : R/W; bitpos: [30:29]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s1_tx_clk_src_sel:2;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl15_reg_t;

/** Type of peri_clk_ctrl16 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_i2s1_tx_div_n : R/W; bitpos: [7:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s1_tx_div_n:8;
        /** reg_i2s1_tx_div_x : R/W; bitpos: [16:8]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s1_tx_div_x:9;
        /** reg_i2s1_tx_div_y : R/W; bitpos: [25:17]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s1_tx_div_y:9;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl16_reg_t;

/** Type of peri_clk_ctrl17 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_i2s1_tx_div_z : R/W; bitpos: [8:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s1_tx_div_z:9;
        /** reg_i2s1_tx_div_yn1 : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s1_tx_div_yn1:1;
        /** reg_i2s1_mst_clk_sel : R/W; bitpos: [10]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s1_mst_clk_sel:1;
        /** reg_i2s2_rx_clk_en : R/W; bitpos: [11]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s2_rx_clk_en:1;
        /** reg_i2s2_rx_clk_src_sel : R/W; bitpos: [13:12]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s2_rx_clk_src_sel:2;
        /** reg_i2s2_rx_div_n : R/W; bitpos: [21:14]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s2_rx_div_n:8;
        /** reg_i2s2_rx_div_x : R/W; bitpos: [30:22]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s2_rx_div_x:9;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl17_reg_t;

/** Type of peri_clk_ctrl18 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_i2s2_rx_div_y : R/W; bitpos: [8:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s2_rx_div_y:9;
        /** reg_i2s2_rx_div_z : R/W; bitpos: [17:9]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s2_rx_div_z:9;
        /** reg_i2s2_rx_div_yn1 : R/W; bitpos: [18]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s2_rx_div_yn1:1;
        /** reg_i2s2_tx_clk_en : R/W; bitpos: [19]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s2_tx_clk_en:1;
        /** reg_i2s2_tx_clk_src_sel : R/W; bitpos: [21:20]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s2_tx_clk_src_sel:2;
        /** reg_i2s2_tx_div_n : R/W; bitpos: [29:22]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s2_tx_div_n:8;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl18_reg_t;

/** Type of peri_clk_ctrl19 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_i2s2_tx_div_x : R/W; bitpos: [8:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s2_tx_div_x:9;
        /** reg_i2s2_tx_div_y : R/W; bitpos: [17:9]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s2_tx_div_y:9;
        /** reg_i2s2_tx_div_z : R/W; bitpos: [26:18]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s2_tx_div_z:9;
        /** reg_i2s2_tx_div_yn1 : R/W; bitpos: [27]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s2_tx_div_yn1:1;
        /** reg_i2s2_mst_clk_sel : R/W; bitpos: [28]; default: 0;
         *  Reserved
         */
        uint32_t reg_i2s2_mst_clk_sel:1;
        /** reg_lcd_clk_src_sel : R/W; bitpos: [30:29]; default: 0;
         *  Reserved
         */
        uint32_t reg_lcd_clk_src_sel:2;
        /** reg_lcd_clk_en : R/W; bitpos: [31]; default: 0;
         *  Reserved
         */
        uint32_t reg_lcd_clk_en:1;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl19_reg_t;

/** Type of peri_clk_ctrl110 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_lcd_clk_div_num : R/W; bitpos: [7:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_lcd_clk_div_num:8;
        /** reg_lcd_clk_div_numerator : R/W; bitpos: [15:8]; default: 0;
         *  Reserved
         */
        uint32_t reg_lcd_clk_div_numerator:8;
        /** reg_lcd_clk_div_denominator : R/W; bitpos: [23:16]; default: 0;
         *  Reserved
         */
        uint32_t reg_lcd_clk_div_denominator:8;
        /** reg_uart0_clk_src_sel : R/W; bitpos: [25:24]; default: 0;
         *  Reserved
         */
        uint32_t reg_uart0_clk_src_sel:2;
        /** reg_uart0_clk_en : R/W; bitpos: [26]; default: 1;
         *  Reserved
         */
        uint32_t reg_uart0_clk_en:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl110_reg_t;

/** Type of peri_clk_ctrl111 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_uart0_sclk_div_num : R/W; bitpos: [7:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_uart0_sclk_div_num:8;
        /** reg_uart0_sclk_div_numerator : R/W; bitpos: [15:8]; default: 0;
         *  Reserved
         */
        uint32_t reg_uart0_sclk_div_numerator:8;
        /** reg_uart0_sclk_div_denominator : R/W; bitpos: [23:16]; default: 0;
         *  Reserved
         */
        uint32_t reg_uart0_sclk_div_denominator:8;
        /** reg_uart1_clk_src_sel : R/W; bitpos: [25:24]; default: 0;
         *  Reserved
         */
        uint32_t reg_uart1_clk_src_sel:2;
        /** reg_uart1_clk_en : R/W; bitpos: [26]; default: 1;
         *  Reserved
         */
        uint32_t reg_uart1_clk_en:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl111_reg_t;

/** Type of peri_clk_ctrl112 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_uart1_sclk_div_num : R/W; bitpos: [7:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_uart1_sclk_div_num:8;
        /** reg_uart1_sclk_div_numerator : R/W; bitpos: [15:8]; default: 0;
         *  Reserved
         */
        uint32_t reg_uart1_sclk_div_numerator:8;
        /** reg_uart1_sclk_div_denominator : R/W; bitpos: [23:16]; default: 0;
         *  Reserved
         */
        uint32_t reg_uart1_sclk_div_denominator:8;
        /** reg_uart2_clk_src_sel : R/W; bitpos: [25:24]; default: 0;
         *  Reserved
         */
        uint32_t reg_uart2_clk_src_sel:2;
        /** reg_uart2_clk_en : R/W; bitpos: [26]; default: 1;
         *  Reserved
         */
        uint32_t reg_uart2_clk_en:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl112_reg_t;

/** Type of peri_clk_ctrl113 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_uart2_sclk_div_num : R/W; bitpos: [7:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_uart2_sclk_div_num:8;
        /** reg_uart2_sclk_div_numerator : R/W; bitpos: [15:8]; default: 0;
         *  Reserved
         */
        uint32_t reg_uart2_sclk_div_numerator:8;
        /** reg_uart2_sclk_div_denominator : R/W; bitpos: [23:16]; default: 0;
         *  Reserved
         */
        uint32_t reg_uart2_sclk_div_denominator:8;
        /** reg_uart3_clk_src_sel : R/W; bitpos: [25:24]; default: 0;
         *  Reserved
         */
        uint32_t reg_uart3_clk_src_sel:2;
        /** reg_uart3_clk_en : R/W; bitpos: [26]; default: 1;
         *  Reserved
         */
        uint32_t reg_uart3_clk_en:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl113_reg_t;

/** Type of peri_clk_ctrl114 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_uart3_sclk_div_num : R/W; bitpos: [7:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_uart3_sclk_div_num:8;
        /** reg_uart3_sclk_div_numerator : R/W; bitpos: [15:8]; default: 0;
         *  Reserved
         */
        uint32_t reg_uart3_sclk_div_numerator:8;
        /** reg_uart3_sclk_div_denominator : R/W; bitpos: [23:16]; default: 0;
         *  Reserved
         */
        uint32_t reg_uart3_sclk_div_denominator:8;
        /** reg_uart4_clk_src_sel : R/W; bitpos: [25:24]; default: 0;
         *  Reserved
         */
        uint32_t reg_uart4_clk_src_sel:2;
        /** reg_uart4_clk_en : R/W; bitpos: [26]; default: 1;
         *  Reserved
         */
        uint32_t reg_uart4_clk_en:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl114_reg_t;

/** Type of peri_clk_ctrl115 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_uart4_sclk_div_num : R/W; bitpos: [7:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_uart4_sclk_div_num:8;
        /** reg_uart4_sclk_div_numerator : R/W; bitpos: [15:8]; default: 0;
         *  Reserved
         */
        uint32_t reg_uart4_sclk_div_numerator:8;
        /** reg_uart4_sclk_div_denominator : R/W; bitpos: [23:16]; default: 0;
         *  Reserved
         */
        uint32_t reg_uart4_sclk_div_denominator:8;
        /** reg_twai0_clk_src_sel : R/W; bitpos: [24]; default: 0;
         *  Reserved
         */
        uint32_t reg_twai0_clk_src_sel:1;
        /** reg_twai0_clk_en : R/W; bitpos: [25]; default: 0;
         *  Reserved
         */
        uint32_t reg_twai0_clk_en:1;
        /** reg_twai1_clk_src_sel : R/W; bitpos: [26]; default: 0;
         *  Reserved
         */
        uint32_t reg_twai1_clk_src_sel:1;
        /** reg_twai1_clk_en : R/W; bitpos: [27]; default: 0;
         *  Reserved
         */
        uint32_t reg_twai1_clk_en:1;
        /** reg_twai2_clk_src_sel : R/W; bitpos: [28]; default: 0;
         *  Reserved
         */
        uint32_t reg_twai2_clk_src_sel:1;
        /** reg_twai2_clk_en : R/W; bitpos: [29]; default: 0;
         *  Reserved
         */
        uint32_t reg_twai2_clk_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl115_reg_t;

/** Type of peri_clk_ctrl116 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpspi2_clk_src_sel : R/W; bitpos: [2:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpspi2_clk_src_sel:3;
        /** reg_gpspi2_hs_clk_en : R/W; bitpos: [3]; default: 1;
         *  Reserved
         */
        uint32_t reg_gpspi2_hs_clk_en:1;
        /** reg_gpspi2_hs_clk_div_num : R/W; bitpos: [11:4]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpspi2_hs_clk_div_num:8;
        /** reg_gpspi2_mst_clk_div_num : R/W; bitpos: [19:12]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpspi2_mst_clk_div_num:8;
        /** reg_gpspi2_mst_clk_en : R/W; bitpos: [20]; default: 1;
         *  Reserved
         */
        uint32_t reg_gpspi2_mst_clk_en:1;
        /** reg_gpspi3_clk_src_sel : R/W; bitpos: [23:21]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpspi3_clk_src_sel:3;
        /** reg_gpspi3_hs_clk_en : R/W; bitpos: [24]; default: 1;
         *  Reserved
         */
        uint32_t reg_gpspi3_hs_clk_en:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl116_reg_t;

/** Type of peri_clk_ctrl117 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpspi3_hs_clk_div_num : R/W; bitpos: [7:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpspi3_hs_clk_div_num:8;
        /** reg_gpspi3_mst_clk_div_num : R/W; bitpos: [15:8]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpspi3_mst_clk_div_num:8;
        /** reg_gpspi3_mst_clk_en : R/W; bitpos: [16]; default: 1;
         *  Reserved
         */
        uint32_t reg_gpspi3_mst_clk_en:1;
        /** reg_parlio_rx_clk_src_sel : R/W; bitpos: [18:17]; default: 0;
         *  Reserved
         */
        uint32_t reg_parlio_rx_clk_src_sel:2;
        /** reg_parlio_rx_clk_en : R/W; bitpos: [19]; default: 0;
         *  Reserved
         */
        uint32_t reg_parlio_rx_clk_en:1;
        /** reg_parlio_rx_clk_div_num : R/W; bitpos: [27:20]; default: 0;
         *  Reserved
         */
        uint32_t reg_parlio_rx_clk_div_num:8;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl117_reg_t;

/** Type of peri_clk_ctrl118 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_parlio_rx_clk_div_numerator : R/W; bitpos: [7:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_parlio_rx_clk_div_numerator:8;
        /** reg_parlio_rx_clk_div_denominator : R/W; bitpos: [15:8]; default: 0;
         *  Reserved
         */
        uint32_t reg_parlio_rx_clk_div_denominator:8;
        /** reg_parlio_tx_clk_src_sel : R/W; bitpos: [17:16]; default: 0;
         *  Reserved
         */
        uint32_t reg_parlio_tx_clk_src_sel:2;
        /** reg_parlio_tx_clk_en : R/W; bitpos: [18]; default: 0;
         *  Reserved
         */
        uint32_t reg_parlio_tx_clk_en:1;
        /** reg_parlio_tx_clk_div_num : R/W; bitpos: [26:19]; default: 0;
         *  Reserved
         */
        uint32_t reg_parlio_tx_clk_div_num:8;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl118_reg_t;

/** Type of peri_clk_ctrl119 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_parlio_tx_clk_div_numerator : R/W; bitpos: [7:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_parlio_tx_clk_div_numerator:8;
        /** reg_parlio_tx_clk_div_denominator : R/W; bitpos: [15:8]; default: 0;
         *  Reserved
         */
        uint32_t reg_parlio_tx_clk_div_denominator:8;
        /** reg_i3c_mst_clk_src_sel : R/W; bitpos: [17:16]; default: 0;
         *  Reserved
         */
        uint32_t reg_i3c_mst_clk_src_sel:2;
        /** reg_i3c_mst_clk_en : R/W; bitpos: [18]; default: 0;
         *  Reserved
         */
        uint32_t reg_i3c_mst_clk_en:1;
        /** reg_i3c_mst_clk_div_num : R/W; bitpos: [26:19]; default: 0;
         *  Reserved
         */
        uint32_t reg_i3c_mst_clk_div_num:8;
        /** reg_cam_clk_src_sel : R/W; bitpos: [28:27]; default: 0;
         *  Reserved
         */
        uint32_t reg_cam_clk_src_sel:2;
        /** reg_cam_clk_en : R/W; bitpos: [29]; default: 0;
         *  Reserved
         */
        uint32_t reg_cam_clk_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl119_reg_t;

/** Type of peri_clk_ctrl120 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_cam_clk_div_num : R/W; bitpos: [7:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_cam_clk_div_num:8;
        /** reg_cam_clk_div_numerator : R/W; bitpos: [15:8]; default: 0;
         *  Reserved
         */
        uint32_t reg_cam_clk_div_numerator:8;
        /** reg_cam_clk_div_denominator : R/W; bitpos: [23:16]; default: 0;
         *  Reserved
         */
        uint32_t reg_cam_clk_div_denominator:8;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl120_reg_t;


/** Group: peri_clk_ctrl2 */
/** Type of peri_clk_ctrl20 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_mcpwm0_clk_src_sel : R/W; bitpos: [1:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_mcpwm0_clk_src_sel:2;
        /** reg_mcpwm0_clk_en : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_mcpwm0_clk_en:1;
        /** reg_mcpwm0_clk_div_num : R/W; bitpos: [10:3]; default: 0;
         *  Reserved
         */
        uint32_t reg_mcpwm0_clk_div_num:8;
        /** reg_mcpwm1_clk_src_sel : R/W; bitpos: [12:11]; default: 0;
         *  Reserved
         */
        uint32_t reg_mcpwm1_clk_src_sel:2;
        /** reg_mcpwm1_clk_en : R/W; bitpos: [13]; default: 0;
         *  Reserved
         */
        uint32_t reg_mcpwm1_clk_en:1;
        /** reg_mcpwm1_clk_div_num : R/W; bitpos: [21:14]; default: 0;
         *  Reserved
         */
        uint32_t reg_mcpwm1_clk_div_num:8;
        /** reg_timergrp0_t0_src_sel : R/W; bitpos: [23:22]; default: 0;
         *  Reserved
         */
        uint32_t reg_timergrp0_t0_src_sel:2;
        /** reg_timergrp0_t0_clk_en : R/W; bitpos: [24]; default: 1;
         *  Reserved
         */
        uint32_t reg_timergrp0_t0_clk_en:1;
        /** reg_timergrp0_t1_src_sel : R/W; bitpos: [26:25]; default: 0;
         *  Reserved
         */
        uint32_t reg_timergrp0_t1_src_sel:2;
        /** reg_timergrp0_t1_clk_en : R/W; bitpos: [27]; default: 1;
         *  Reserved
         */
        uint32_t reg_timergrp0_t1_clk_en:1;
        /** reg_timergrp0_wdt_src_sel : R/W; bitpos: [29:28]; default: 0;
         *  Reserved
         */
        uint32_t reg_timergrp0_wdt_src_sel:2;
        /** reg_timergrp0_wdt_clk_en : R/W; bitpos: [30]; default: 1;
         *  Reserved
         */
        uint32_t reg_timergrp0_wdt_clk_en:1;
        /** reg_timergrp0_tgrt_clk_en : R/W; bitpos: [31]; default: 1;
         *  Reserved
         */
        uint32_t reg_timergrp0_tgrt_clk_en:1;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl20_reg_t;

/** Type of peri_clk_ctrl21 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_timergrp0_tgrt_clk_src_sel : R/W; bitpos: [3:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_timergrp0_tgrt_clk_src_sel:4;
        /** reg_timergrp0_tgrt_clk_div_num : R/W; bitpos: [19:4]; default: 0;
         *  Reserved
         */
        uint32_t reg_timergrp0_tgrt_clk_div_num:16;
        /** reg_timergrp1_t0_src_sel : R/W; bitpos: [21:20]; default: 0;
         *  Reserved
         */
        uint32_t reg_timergrp1_t0_src_sel:2;
        /** reg_timergrp1_t0_clk_en : R/W; bitpos: [22]; default: 1;
         *  Reserved
         */
        uint32_t reg_timergrp1_t0_clk_en:1;
        /** reg_timergrp1_t1_src_sel : R/W; bitpos: [24:23]; default: 0;
         *  Reserved
         */
        uint32_t reg_timergrp1_t1_src_sel:2;
        /** reg_timergrp1_t1_clk_en : R/W; bitpos: [25]; default: 1;
         *  Reserved
         */
        uint32_t reg_timergrp1_t1_clk_en:1;
        /** reg_timergrp1_wdt_src_sel : R/W; bitpos: [27:26]; default: 0;
         *  Reserved
         */
        uint32_t reg_timergrp1_wdt_src_sel:2;
        /** reg_timergrp1_wdt_clk_en : R/W; bitpos: [28]; default: 1;
         *  Reserved
         */
        uint32_t reg_timergrp1_wdt_clk_en:1;
        /** reg_systimer_clk_src_sel : R/W; bitpos: [29]; default: 0;
         *  Reserved
         */
        uint32_t reg_systimer_clk_src_sel:1;
        /** reg_systimer_clk_en : R/W; bitpos: [30]; default: 1;
         *  Reserved
         */
        uint32_t reg_systimer_clk_en:1;
        uint32_t reserved_31:1;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl21_reg_t;

/** Type of peri_clk_ctrl22 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_ledc_clk_src_sel : R/W; bitpos: [1:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_ledc_clk_src_sel:2;
        /** reg_ledc_clk_en : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_ledc_clk_en:1;
        /** reg_rmt_clk_src_sel : R/W; bitpos: [4:3]; default: 0;
         *  Reserved
         */
        uint32_t reg_rmt_clk_src_sel:2;
        /** reg_rmt_clk_en : R/W; bitpos: [5]; default: 0;
         *  Reserved
         */
        uint32_t reg_rmt_clk_en:1;
        /** reg_rmt_clk_div_num : R/W; bitpos: [13:6]; default: 0;
         *  Reserved
         */
        uint32_t reg_rmt_clk_div_num:8;
        /** reg_rmt_clk_div_numerator : R/W; bitpos: [21:14]; default: 0;
         *  Reserved
         */
        uint32_t reg_rmt_clk_div_numerator:8;
        /** reg_rmt_clk_div_denominator : R/W; bitpos: [29:22]; default: 0;
         *  Reserved
         */
        uint32_t reg_rmt_clk_div_denominator:8;
        /** reg_adc_clk_src_sel : R/W; bitpos: [31:30]; default: 0;
         *  Reserved
         */
        uint32_t reg_adc_clk_src_sel:2;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl22_reg_t;

/** Type of peri_clk_ctrl23 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_adc_clk_en : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_adc_clk_en:1;
        /** reg_adc_clk_div_num : R/W; bitpos: [8:1]; default: 4;
         *  Reserved
         */
        uint32_t reg_adc_clk_div_num:8;
        /** reg_adc_clk_div_numerator : R/W; bitpos: [16:9]; default: 0;
         *  Reserved
         */
        uint32_t reg_adc_clk_div_numerator:8;
        /** reg_adc_clk_div_denominator : R/W; bitpos: [24:17]; default: 0;
         *  Reserved
         */
        uint32_t reg_adc_clk_div_denominator:8;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl23_reg_t;

/** Type of peri_clk_ctrl24 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_adc_sar1_clk_div_num : R/W; bitpos: [7:0]; default: 4;
         *  Reserved
         */
        uint32_t reg_adc_sar1_clk_div_num:8;
        /** reg_adc_sar2_clk_div_num : R/W; bitpos: [15:8]; default: 4;
         *  Reserved
         */
        uint32_t reg_adc_sar2_clk_div_num:8;
        /** reg_pvt_clk_div_num : R/W; bitpos: [23:16]; default: 0;
         *  Reserved
         */
        uint32_t reg_pvt_clk_div_num:8;
        /** reg_pvt_clk_en : R/W; bitpos: [24]; default: 0;
         *  Reserved
         */
        uint32_t reg_pvt_clk_en:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl24_reg_t;

/** Type of peri_clk_ctrl25 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_pvt_peri_group_clk_div_num : R/W; bitpos: [7:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_pvt_peri_group_clk_div_num:8;
        /** reg_pvt_peri_group1_clk_en : R/W; bitpos: [8]; default: 0;
         *  Reserved
         */
        uint32_t reg_pvt_peri_group1_clk_en:1;
        /** reg_pvt_peri_group2_clk_en : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_pvt_peri_group2_clk_en:1;
        /** reg_pvt_peri_group3_clk_en : R/W; bitpos: [10]; default: 0;
         *  Reserved
         */
        uint32_t reg_pvt_peri_group3_clk_en:1;
        /** reg_pvt_peri_group4_clk_en : R/W; bitpos: [11]; default: 0;
         *  Reserved
         */
        uint32_t reg_pvt_peri_group4_clk_en:1;
        /** reg_crypto_clk_src_sel : R/W; bitpos: [13:12]; default: 0;
         *  Reserved
         */
        uint32_t reg_crypto_clk_src_sel:2;
        /** reg_crypto_aes_clk_en : R/W; bitpos: [14]; default: 1;
         *  Reserved
         */
        uint32_t reg_crypto_aes_clk_en:1;
        /** reg_crypto_ds_clk_en : R/W; bitpos: [15]; default: 1;
         *  Reserved
         */
        uint32_t reg_crypto_ds_clk_en:1;
        /** reg_crypto_ecc_clk_en : R/W; bitpos: [16]; default: 1;
         *  Reserved
         */
        uint32_t reg_crypto_ecc_clk_en:1;
        /** reg_crypto_hmac_clk_en : R/W; bitpos: [17]; default: 1;
         *  Reserved
         */
        uint32_t reg_crypto_hmac_clk_en:1;
        /** reg_crypto_rsa_clk_en : R/W; bitpos: [18]; default: 1;
         *  Reserved
         */
        uint32_t reg_crypto_rsa_clk_en:1;
        /** reg_crypto_sec_clk_en : R/W; bitpos: [19]; default: 1;
         *  Reserved
         */
        uint32_t reg_crypto_sec_clk_en:1;
        /** reg_crypto_sha_clk_en : R/W; bitpos: [20]; default: 1;
         *  Reserved
         */
        uint32_t reg_crypto_sha_clk_en:1;
        /** reg_crypto_ecdsa_clk_en : R/W; bitpos: [21]; default: 1;
         *  Reserved
         */
        uint32_t reg_crypto_ecdsa_clk_en:1;
        /** reg_crypto_km_clk_en : R/W; bitpos: [22]; default: 1;
         *  Reserved
         */
        uint32_t reg_crypto_km_clk_en:1;
        /** reg_isp_clk_src_sel : R/W; bitpos: [24:23]; default: 0;
         *  Reserved
         */
        uint32_t reg_isp_clk_src_sel:2;
        /** reg_isp_clk_en : R/W; bitpos: [25]; default: 0;
         *  Reserved
         */
        uint32_t reg_isp_clk_en:1;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl25_reg_t;

/** Type of peri_clk_ctrl26 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_isp_clk_div_num : R/W; bitpos: [7:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_isp_clk_div_num:8;
        /** reg_iomux_clk_src_sel : R/W; bitpos: [8]; default: 0;
         *  Reserved
         */
        uint32_t reg_iomux_clk_src_sel:1;
        /** reg_iomux_clk_en : R/W; bitpos: [9]; default: 1;
         *  Reserved
         */
        uint32_t reg_iomux_clk_en:1;
        /** reg_iomux_clk_div_num : R/W; bitpos: [17:10]; default: 0;
         *  Reserved
         */
        uint32_t reg_iomux_clk_div_num:8;
        /** reg_h264_clk_src_sel : R/W; bitpos: [18]; default: 0;
         *  Reserved
         */
        uint32_t reg_h264_clk_src_sel:1;
        /** reg_h264_clk_en : R/W; bitpos: [19]; default: 0;
         *  Reserved
         */
        uint32_t reg_h264_clk_en:1;
        /** reg_h264_clk_div_num : R/W; bitpos: [27:20]; default: 0;
         *  Reserved
         */
        uint32_t reg_h264_clk_div_num:8;
        /** reg_padbist_rx_clk_src_sel : R/W; bitpos: [28]; default: 0;
         *  Reserved
         */
        uint32_t reg_padbist_rx_clk_src_sel:1;
        /** reg_padbist_rx_clk_en : R/W; bitpos: [29]; default: 0;
         *  Reserved
         */
        uint32_t reg_padbist_rx_clk_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl26_reg_t;

/** Type of peri_clk_ctrl27 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_padbist_rx_clk_div_num : R/W; bitpos: [7:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_padbist_rx_clk_div_num:8;
        /** reg_padbist_tx_clk_src_sel : R/W; bitpos: [8]; default: 0;
         *  Reserved
         */
        uint32_t reg_padbist_tx_clk_src_sel:1;
        /** reg_padbist_tx_clk_en : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_padbist_tx_clk_en:1;
        /** reg_padbist_tx_clk_div_num : R/W; bitpos: [17:10]; default: 0;
         *  Reserved
         */
        uint32_t reg_padbist_tx_clk_div_num:8;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} hp_sys_clkrst_peri_clk_ctrl27_reg_t;


/** Group: clk_force_on_ctrl */
/** Type of clk_force_on_ctrl0 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_cpuicm_gated_clk_force_on : R/W; bitpos: [0]; default: 1;
         *  Reserved
         */
        uint32_t reg_cpuicm_gated_clk_force_on:1;
        /** reg_tcm_cpu_clk_force_on : R/W; bitpos: [1]; default: 1;
         *  Reserved
         */
        uint32_t reg_tcm_cpu_clk_force_on:1;
        /** reg_busmon_cpu_clk_force_on : R/W; bitpos: [2]; default: 1;
         *  Reserved
         */
        uint32_t reg_busmon_cpu_clk_force_on:1;
        /** reg_l1cache_cpu_clk_force_on : R/W; bitpos: [3]; default: 1;
         *  Reserved
         */
        uint32_t reg_l1cache_cpu_clk_force_on:1;
        /** reg_l1cache_d_cpu_clk_force_on : R/W; bitpos: [4]; default: 1;
         *  Reserved
         */
        uint32_t reg_l1cache_d_cpu_clk_force_on:1;
        /** reg_l1cache_i0_cpu_clk_force_on : R/W; bitpos: [5]; default: 1;
         *  Reserved
         */
        uint32_t reg_l1cache_i0_cpu_clk_force_on:1;
        /** reg_l1cache_i1_cpu_clk_force_on : R/W; bitpos: [6]; default: 1;
         *  Reserved
         */
        uint32_t reg_l1cache_i1_cpu_clk_force_on:1;
        /** reg_trace_cpu_clk_force_on : R/W; bitpos: [7]; default: 1;
         *  Reserved
         */
        uint32_t reg_trace_cpu_clk_force_on:1;
        /** reg_trace_sys_clk_force_on : R/W; bitpos: [8]; default: 1;
         *  Reserved
         */
        uint32_t reg_trace_sys_clk_force_on:1;
        /** reg_l1cache_mem_clk_force_on : R/W; bitpos: [9]; default: 1;
         *  Reserved
         */
        uint32_t reg_l1cache_mem_clk_force_on:1;
        /** reg_l1cache_d_mem_clk_force_on : R/W; bitpos: [10]; default: 1;
         *  Reserved
         */
        uint32_t reg_l1cache_d_mem_clk_force_on:1;
        /** reg_l1cache_i0_mem_clk_force_on : R/W; bitpos: [11]; default: 1;
         *  Reserved
         */
        uint32_t reg_l1cache_i0_mem_clk_force_on:1;
        /** reg_l1cache_i1_mem_clk_force_on : R/W; bitpos: [12]; default: 1;
         *  Reserved
         */
        uint32_t reg_l1cache_i1_mem_clk_force_on:1;
        /** reg_l2cache_mem_clk_force_on : R/W; bitpos: [13]; default: 1;
         *  Reserved
         */
        uint32_t reg_l2cache_mem_clk_force_on:1;
        /** reg_l2mem_mem_clk_force_on : R/W; bitpos: [14]; default: 1;
         *  Reserved
         */
        uint32_t reg_l2mem_mem_clk_force_on:1;
        /** reg_sar1_clk_force_on : R/W; bitpos: [15]; default: 1;
         *  Reserved
         */
        uint32_t reg_sar1_clk_force_on:1;
        /** reg_sar2_clk_force_on : R/W; bitpos: [16]; default: 1;
         *  Reserved
         */
        uint32_t reg_sar2_clk_force_on:1;
        /** reg_gmac_tx_clk_force_on : R/W; bitpos: [17]; default: 1;
         *  Reserved
         */
        uint32_t reg_gmac_tx_clk_force_on:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} hp_sys_clkrst_clk_force_on_ctrl0_reg_t;


/** Group: dpa_ctrl */
/** Type of dpa_ctrl0 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_sec_dpa_level : R/W; bitpos: [1:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_sec_dpa_level:2;
        /** reg_sec_dpa_cfg_sel : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_sec_dpa_cfg_sel:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} hp_sys_clkrst_dpa_ctrl0_reg_t;


/** Group: ana_pll_ctrl */
/** Type of ana_pll_ctrl0 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_plla_cal_end : RO; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_plla_cal_end:1;
        /** reg_plla_cal_stop : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_plla_cal_stop:1;
        /** reg_cpu_pll_cal_end : RO; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_cpu_pll_cal_end:1;
        /** reg_cpu_pll_cal_stop : R/W; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t reg_cpu_pll_cal_stop:1;
        /** reg_sdio_pll_cal_end : RO; bitpos: [4]; default: 0;
         *  Reserved
         */
        uint32_t reg_sdio_pll_cal_end:1;
        /** reg_sdio_pll_cal_stop : R/W; bitpos: [5]; default: 0;
         *  Reserved
         */
        uint32_t reg_sdio_pll_cal_stop:1;
        /** reg_sys_pll_cal_end : RO; bitpos: [6]; default: 0;
         *  Reserved
         */
        uint32_t reg_sys_pll_cal_end:1;
        /** reg_sys_pll_cal_stop : R/W; bitpos: [7]; default: 0;
         *  Reserved
         */
        uint32_t reg_sys_pll_cal_stop:1;
        /** reg_mspi_cal_end : RO; bitpos: [8]; default: 0;
         *  Reserved
         */
        uint32_t reg_mspi_cal_end:1;
        /** reg_mspi_cal_stop : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_mspi_cal_stop:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} hp_sys_clkrst_ana_pll_ctrl0_reg_t;


/** Group: hp_rst_en */
/** Type of hp_rst_en0 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_rst_en_corectrl : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_corectrl:1;
        /** reg_rst_en_pvt_top : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_pvt_top:1;
        /** reg_rst_en_pvt_peri_group1 : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_pvt_peri_group1:1;
        /** reg_rst_en_pvt_peri_group2 : R/W; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_pvt_peri_group2:1;
        /** reg_rst_en_pvt_peri_group3 : R/W; bitpos: [4]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_pvt_peri_group3:1;
        /** reg_rst_en_pvt_peri_group4 : R/W; bitpos: [5]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_pvt_peri_group4:1;
        /** reg_rst_en_regdma : R/W; bitpos: [6]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_regdma:1;
        /** reg_rst_en_core0_global : R/W; bitpos: [7]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_core0_global:1;
        /** reg_rst_en_core1_global : R/W; bitpos: [8]; default: 1;
         *  Reserved
         */
        uint32_t reg_rst_en_core1_global:1;
        /** reg_rst_en_coretrace0 : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_coretrace0:1;
        /** reg_rst_en_coretrace1 : R/W; bitpos: [10]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_coretrace1:1;
        /** reg_rst_en_hp_tcm : R/W; bitpos: [11]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_hp_tcm:1;
        /** reg_rst_en_hp_cache : R/W; bitpos: [12]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_hp_cache:1;
        /** reg_rst_en_l1_i0_cache : R/W; bitpos: [13]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_l1_i0_cache:1;
        /** reg_rst_en_l1_i1_cache : R/W; bitpos: [14]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_l1_i1_cache:1;
        /** reg_rst_en_l1_d_cache : R/W; bitpos: [15]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_l1_d_cache:1;
        /** reg_rst_en_l2_cache : R/W; bitpos: [16]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_l2_cache:1;
        /** reg_rst_en_l2_mem : R/W; bitpos: [17]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_l2_mem:1;
        /** reg_rst_en_l2memmon : R/W; bitpos: [18]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_l2memmon:1;
        /** reg_rst_en_tcmmon : R/W; bitpos: [19]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_tcmmon:1;
        /** reg_rst_en_pvt_apb : R/W; bitpos: [20]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_pvt_apb:1;
        /** reg_rst_en_gdma : R/W; bitpos: [21]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_gdma:1;
        /** reg_rst_en_mspi_axi : R/W; bitpos: [22]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_mspi_axi:1;
        /** reg_rst_en_dual_mspi_axi : R/W; bitpos: [23]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_dual_mspi_axi:1;
        /** reg_rst_en_mspi_apb : R/W; bitpos: [24]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_mspi_apb:1;
        /** reg_rst_en_dual_mspi_apb : R/W; bitpos: [25]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_dual_mspi_apb:1;
        /** reg_rst_en_dsi_brg : R/W; bitpos: [26]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_dsi_brg:1;
        /** reg_rst_en_csi_host : R/W; bitpos: [27]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_csi_host:1;
        /** reg_rst_en_csi_brg : R/W; bitpos: [28]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_csi_brg:1;
        /** reg_rst_en_isp : R/W; bitpos: [29]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_isp:1;
        /** reg_rst_en_jpeg : R/W; bitpos: [30]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_jpeg:1;
        /** reg_rst_en_dma2d : R/W; bitpos: [31]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_dma2d:1;
    };
    uint32_t val;
} hp_sys_clkrst_hp_rst_en0_reg_t;

/** Type of hp_rst_en1 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_rst_en_ppa : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_ppa:1;
        /** reg_rst_en_ahb_pdma : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_ahb_pdma:1;
        /** reg_rst_en_axi_pdma : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_axi_pdma:1;
        /** reg_rst_en_iomux : R/W; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_iomux:1;
        /** reg_rst_en_padbist : R/W; bitpos: [4]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_padbist:1;
        /** reg_rst_en_stimer : R/W; bitpos: [5]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_stimer:1;
        /** reg_rst_en_timergrp0 : R/W; bitpos: [6]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_timergrp0:1;
        /** reg_rst_en_timergrp1 : R/W; bitpos: [7]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_timergrp1:1;
        /** reg_rst_en_uart0_core : R/W; bitpos: [8]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_uart0_core:1;
        /** reg_rst_en_uart1_core : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_uart1_core:1;
        /** reg_rst_en_uart2_core : R/W; bitpos: [10]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_uart2_core:1;
        /** reg_rst_en_uart3_core : R/W; bitpos: [11]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_uart3_core:1;
        /** reg_rst_en_uart4_core : R/W; bitpos: [12]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_uart4_core:1;
        /** reg_rst_en_uart0_apb : R/W; bitpos: [13]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_uart0_apb:1;
        /** reg_rst_en_uart1_apb : R/W; bitpos: [14]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_uart1_apb:1;
        /** reg_rst_en_uart2_apb : R/W; bitpos: [15]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_uart2_apb:1;
        /** reg_rst_en_uart3_apb : R/W; bitpos: [16]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_uart3_apb:1;
        /** reg_rst_en_uart4_apb : R/W; bitpos: [17]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_uart4_apb:1;
        /** reg_rst_en_uhci : R/W; bitpos: [18]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_uhci:1;
        /** reg_rst_en_i3cmst : R/W; bitpos: [19]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_i3cmst:1;
        /** reg_rst_en_i3cslv : R/W; bitpos: [20]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_i3cslv:1;
        /** reg_rst_en_i2c1 : R/W; bitpos: [21]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_i2c1:1;
        /** reg_rst_en_i2c0 : R/W; bitpos: [22]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_i2c0:1;
        /** reg_rst_en_rmt : R/W; bitpos: [23]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_rmt:1;
        /** reg_rst_en_pwm0 : R/W; bitpos: [24]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_pwm0:1;
        /** reg_rst_en_pwm1 : R/W; bitpos: [25]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_pwm1:1;
        /** reg_rst_en_can0 : R/W; bitpos: [26]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_can0:1;
        /** reg_rst_en_can1 : R/W; bitpos: [27]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_can1:1;
        /** reg_rst_en_can2 : R/W; bitpos: [28]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_can2:1;
        /** reg_rst_en_ledc : R/W; bitpos: [29]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_ledc:1;
        /** reg_rst_en_pcnt : R/W; bitpos: [30]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_pcnt:1;
        /** reg_rst_en_etm : R/W; bitpos: [31]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_etm:1;
    };
    uint32_t val;
} hp_sys_clkrst_hp_rst_en1_reg_t;

/** Type of hp_rst_en2 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_rst_en_intrmtx : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_intrmtx:1;
        /** reg_rst_en_parlio : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_parlio:1;
        /** reg_rst_en_parlio_rx : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_parlio_rx:1;
        /** reg_rst_en_parlio_tx : R/W; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_parlio_tx:1;
        /** reg_rst_en_i2s0_apb : R/W; bitpos: [4]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_i2s0_apb:1;
        /** reg_rst_en_i2s1_apb : R/W; bitpos: [5]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_i2s1_apb:1;
        /** reg_rst_en_i2s2_apb : R/W; bitpos: [6]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_i2s2_apb:1;
        /** reg_rst_en_spi2 : R/W; bitpos: [7]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_spi2:1;
        /** reg_rst_en_spi3 : R/W; bitpos: [8]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_spi3:1;
        /** reg_rst_en_lcdcam : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_lcdcam:1;
        /** reg_rst_en_adc : R/W; bitpos: [10]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_adc:1;
        /** reg_rst_en_bitsrambler : R/W; bitpos: [11]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_bitsrambler:1;
        /** reg_rst_en_bitsrambler_rx : R/W; bitpos: [12]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_bitsrambler_rx:1;
        /** reg_rst_en_bitsrambler_tx : R/W; bitpos: [13]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_bitsrambler_tx:1;
        /** reg_rst_en_crypto : R/W; bitpos: [14]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_crypto:1;
        /** reg_rst_en_sec : R/W; bitpos: [15]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_sec:1;
        /** reg_rst_en_aes : R/W; bitpos: [16]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_aes:1;
        /** reg_rst_en_ds : R/W; bitpos: [17]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_ds:1;
        /** reg_rst_en_sha : R/W; bitpos: [18]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_sha:1;
        /** reg_rst_en_hmac : R/W; bitpos: [19]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_hmac:1;
        /** reg_rst_en_ecdsa : R/W; bitpos: [20]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_ecdsa:1;
        /** reg_rst_en_rsa : R/W; bitpos: [21]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_rsa:1;
        /** reg_rst_en_ecc : R/W; bitpos: [22]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_ecc:1;
        /** reg_rst_en_km : R/W; bitpos: [23]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_km:1;
        /** reg_rst_en_h264 : R/W; bitpos: [24]; default: 0;
         *  Reserved
         */
        uint32_t reg_rst_en_h264:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} hp_sys_clkrst_hp_rst_en2_reg_t;


/** Group: hp_force_norst */
/** Type of hp_force_norst0 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_force_norst_core0 : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_core0:1;
        /** reg_force_norst_core1 : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_core1:1;
        /** reg_force_norst_coretrace0 : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_coretrace0:1;
        /** reg_force_norst_coretrace1 : R/W; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_coretrace1:1;
        /** reg_force_norst_l2memmon : R/W; bitpos: [4]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_l2memmon:1;
        /** reg_force_norst_tcmmon : R/W; bitpos: [5]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_tcmmon:1;
        /** reg_force_norst_gdma : R/W; bitpos: [6]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_gdma:1;
        /** reg_force_norst_mspi_axi : R/W; bitpos: [7]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_mspi_axi:1;
        /** reg_force_norst_dual_mspi_axi : R/W; bitpos: [8]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_dual_mspi_axi:1;
        /** reg_force_norst_mspi_apb : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_mspi_apb:1;
        /** reg_force_norst_dual_mspi_apb : R/W; bitpos: [10]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_dual_mspi_apb:1;
        /** reg_force_norst_dsi_brg : R/W; bitpos: [11]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_dsi_brg:1;
        /** reg_force_norst_csi_host : R/W; bitpos: [12]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_csi_host:1;
        /** reg_force_norst_csi_brg : R/W; bitpos: [13]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_csi_brg:1;
        /** reg_force_norst_isp : R/W; bitpos: [14]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_isp:1;
        /** reg_force_norst_jpeg : R/W; bitpos: [15]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_jpeg:1;
        /** reg_force_norst_dma2d : R/W; bitpos: [16]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_dma2d:1;
        /** reg_force_norst_ppa : R/W; bitpos: [17]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_ppa:1;
        /** reg_force_norst_ahb_pdma : R/W; bitpos: [18]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_ahb_pdma:1;
        /** reg_force_norst_axi_pdma : R/W; bitpos: [19]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_axi_pdma:1;
        /** reg_force_norst_iomux : R/W; bitpos: [20]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_iomux:1;
        /** reg_force_norst_padbist : R/W; bitpos: [21]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_padbist:1;
        /** reg_force_norst_stimer : R/W; bitpos: [22]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_stimer:1;
        /** reg_force_norst_timergrp0 : R/W; bitpos: [23]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_timergrp0:1;
        /** reg_force_norst_timergrp1 : R/W; bitpos: [24]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_timergrp1:1;
        /** reg_force_norst_uart0 : R/W; bitpos: [25]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_uart0:1;
        /** reg_force_norst_uart1 : R/W; bitpos: [26]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_uart1:1;
        /** reg_force_norst_uart2 : R/W; bitpos: [27]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_uart2:1;
        /** reg_force_norst_uart3 : R/W; bitpos: [28]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_uart3:1;
        /** reg_force_norst_uart4 : R/W; bitpos: [29]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_uart4:1;
        /** reg_force_norst_uhci : R/W; bitpos: [30]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_uhci:1;
        /** reg_force_norst_i3cmst : R/W; bitpos: [31]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_i3cmst:1;
    };
    uint32_t val;
} hp_sys_clkrst_hp_force_norst0_reg_t;

/** Type of hp_force_norst1 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_force_norst_i3cslv : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_i3cslv:1;
        /** reg_force_norst_i2c1 : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_i2c1:1;
        /** reg_force_norst_i2c0 : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_i2c0:1;
        /** reg_force_norst_rmt : R/W; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_rmt:1;
        /** reg_force_norst_pwm0 : R/W; bitpos: [4]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_pwm0:1;
        /** reg_force_norst_pwm1 : R/W; bitpos: [5]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_pwm1:1;
        /** reg_force_norst_can0 : R/W; bitpos: [6]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_can0:1;
        /** reg_force_norst_can1 : R/W; bitpos: [7]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_can1:1;
        /** reg_force_norst_can2 : R/W; bitpos: [8]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_can2:1;
        /** reg_force_norst_ledc : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_ledc:1;
        /** reg_force_norst_pcnt : R/W; bitpos: [10]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_pcnt:1;
        /** reg_force_norst_etm : R/W; bitpos: [11]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_etm:1;
        /** reg_force_norst_intrmtx : R/W; bitpos: [12]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_intrmtx:1;
        /** reg_force_norst_parlio : R/W; bitpos: [13]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_parlio:1;
        /** reg_force_norst_parlio_rx : R/W; bitpos: [14]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_parlio_rx:1;
        /** reg_force_norst_parlio_tx : R/W; bitpos: [15]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_parlio_tx:1;
        /** reg_force_norst_i2s0 : R/W; bitpos: [16]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_i2s0:1;
        /** reg_force_norst_i2s1 : R/W; bitpos: [17]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_i2s1:1;
        /** reg_force_norst_i2s2 : R/W; bitpos: [18]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_i2s2:1;
        /** reg_force_norst_spi2 : R/W; bitpos: [19]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_spi2:1;
        /** reg_force_norst_spi3 : R/W; bitpos: [20]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_spi3:1;
        /** reg_force_norst_lcdcam : R/W; bitpos: [21]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_lcdcam:1;
        /** reg_force_norst_adc : R/W; bitpos: [22]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_adc:1;
        /** reg_force_norst_bitsrambler : R/W; bitpos: [23]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_bitsrambler:1;
        /** reg_force_norst_bitsrambler_rx : R/W; bitpos: [24]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_bitsrambler_rx:1;
        /** reg_force_norst_bitsrambler_tx : R/W; bitpos: [25]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_bitsrambler_tx:1;
        /** reg_force_norst_h264 : R/W; bitpos: [26]; default: 0;
         *  Reserved
         */
        uint32_t reg_force_norst_h264:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} hp_sys_clkrst_hp_force_norst1_reg_t;


/** Group: hpwdt_core0_rst_ctrl */
/** Type of hpwdt_core0_rst_ctrl0 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_hpcore0_stall_en : R/W; bitpos: [0]; default: 1;
         *  Reserved
         */
        uint32_t reg_hpcore0_stall_en:1;
        /** reg_hpcore0_stall_wait_num : R/W; bitpos: [8:1]; default: 8;
         *  Reserved
         */
        uint32_t reg_hpcore0_stall_wait_num:8;
        /** reg_wdt_hpcore0_rst_len : R/W; bitpos: [16:9]; default: 8;
         *  Reserved
         */
        uint32_t reg_wdt_hpcore0_rst_len:8;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} hp_sys_clkrst_hpwdt_core0_rst_ctrl0_reg_t;


/** Group: hpwdt_core1_rst_ctrl */
/** Type of hpwdt_core1_rst_ctrl0 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_hpcore1_stall_en : R/W; bitpos: [0]; default: 1;
         *  Reserved
         */
        uint32_t reg_hpcore1_stall_en:1;
        /** reg_hpcore1_stall_wait_num : R/W; bitpos: [8:1]; default: 8;
         *  Reserved
         */
        uint32_t reg_hpcore1_stall_wait_num:8;
        /** reg_wdt_hpcore1_rst_len : R/W; bitpos: [16:9]; default: 8;
         *  Reserved
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
        /** reg_cpu_div_numerator_cur : RO; bitpos: [18:11]; default: 0;
         *  cpu current div numerator
         */
        uint32_t reg_cpu_div_numerator_cur:8;
        /** reg_cpu_div_denominator_cur : RO; bitpos: [26:19]; default: 0;
         *  cpu current div denominator
         */
        uint32_t reg_cpu_div_denominator_cur:8;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} hp_sys_clkrst_cpu_clk_status0_reg_t;


/** Group: dbg_clk_ctrl */
/** Type of dbg_clk_ctrl0 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_dbg_ch0_sel : R/W; bitpos: [7:0]; default: 255;
         *  Reserved
         */
        uint32_t reg_dbg_ch0_sel:8;
        /** reg_dbg_ch1_sel : R/W; bitpos: [15:8]; default: 255;
         *  Reserved
         */
        uint32_t reg_dbg_ch1_sel:8;
        /** reg_dbg_ch2_sel : R/W; bitpos: [23:16]; default: 255;
         *  Reserved
         */
        uint32_t reg_dbg_ch2_sel:8;
        /** reg_dbg_ch0_div_num : R/W; bitpos: [31:24]; default: 3;
         *  Reserved
         */
        uint32_t reg_dbg_ch0_div_num:8;
    };
    uint32_t val;
} hp_sys_clkrst_dbg_clk_ctrl0_reg_t;

/** Type of dbg_clk_ctrl1 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_dbg_ch1_div_num : R/W; bitpos: [7:0]; default: 3;
         *  Reserved
         */
        uint32_t reg_dbg_ch1_div_num:8;
        /** reg_dbg_ch2_div_num : R/W; bitpos: [15:8]; default: 3;
         *  Reserved
         */
        uint32_t reg_dbg_ch2_div_num:8;
        /** reg_dbg_ch0_en : R/W; bitpos: [16]; default: 0;
         *  Reserved
         */
        uint32_t reg_dbg_ch0_en:1;
        /** reg_dbg_ch1_en : R/W; bitpos: [17]; default: 0;
         *  Reserved
         */
        uint32_t reg_dbg_ch1_en:1;
        /** reg_dbg_ch2_en : R/W; bitpos: [18]; default: 0;
         *  Reserved
         */
        uint32_t reg_dbg_ch2_en:1;
        uint32_t reserved_19:13;
    };
    uint32_t val;
} hp_sys_clkrst_dbg_clk_ctrl1_reg_t;


/** Group: hpcore_wdt_reset_source */
/** Type of hpcore_wdt_reset_source0 register
 *  Reserved
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


typedef struct hp_sys_clkrst_dev_t {
    volatile hp_sys_clkrst_clk_en0_reg_t clk_en0;
    volatile hp_sys_clkrst_root_clk_ctrl0_reg_t root_clk_ctrl0;
    volatile hp_sys_clkrst_root_clk_ctrl1_reg_t root_clk_ctrl1;
    volatile hp_sys_clkrst_root_clk_ctrl2_reg_t root_clk_ctrl2;
    volatile hp_sys_clkrst_root_clk_ctrl3_reg_t root_clk_ctrl3;
    volatile hp_sys_clkrst_soc_clk_ctrl0_reg_t soc_clk_ctrl0;
    volatile hp_sys_clkrst_soc_clk_ctrl1_reg_t soc_clk_ctrl1;
    volatile hp_sys_clkrst_soc_clk_ctrl2_reg_t soc_clk_ctrl2;
    volatile hp_sys_clkrst_soc_clk_ctrl3_reg_t soc_clk_ctrl3;
    volatile hp_sys_clkrst_ref_clk_ctrl0_reg_t ref_clk_ctrl0;
    volatile hp_sys_clkrst_ref_clk_ctrl1_reg_t ref_clk_ctrl1;
    volatile hp_sys_clkrst_ref_clk_ctrl2_reg_t ref_clk_ctrl2;
    volatile hp_sys_clkrst_peri_clk_ctrl00_reg_t peri_clk_ctrl00;
    volatile hp_sys_clkrst_peri_clk_ctrl01_reg_t peri_clk_ctrl01;
    volatile hp_sys_clkrst_peri_clk_ctrl02_reg_t peri_clk_ctrl02;
    volatile hp_sys_clkrst_peri_clk_ctrl03_reg_t peri_clk_ctrl03;
    volatile hp_sys_clkrst_peri_clk_ctrl10_reg_t peri_clk_ctrl10;
    volatile hp_sys_clkrst_peri_clk_ctrl11_reg_t peri_clk_ctrl11;
    volatile hp_sys_clkrst_peri_clk_ctrl12_reg_t peri_clk_ctrl12;
    volatile hp_sys_clkrst_peri_clk_ctrl13_reg_t peri_clk_ctrl13;
    volatile hp_sys_clkrst_peri_clk_ctrl14_reg_t peri_clk_ctrl14;
    volatile hp_sys_clkrst_peri_clk_ctrl15_reg_t peri_clk_ctrl15;
    volatile hp_sys_clkrst_peri_clk_ctrl16_reg_t peri_clk_ctrl16;
    volatile hp_sys_clkrst_peri_clk_ctrl17_reg_t peri_clk_ctrl17;
    volatile hp_sys_clkrst_peri_clk_ctrl18_reg_t peri_clk_ctrl18;
    volatile hp_sys_clkrst_peri_clk_ctrl19_reg_t peri_clk_ctrl19;
    volatile hp_sys_clkrst_peri_clk_ctrl110_reg_t peri_clk_ctrl110;
    volatile hp_sys_clkrst_peri_clk_ctrl111_reg_t peri_clk_ctrl111;
    volatile hp_sys_clkrst_peri_clk_ctrl112_reg_t peri_clk_ctrl112;
    volatile hp_sys_clkrst_peri_clk_ctrl113_reg_t peri_clk_ctrl113;
    volatile hp_sys_clkrst_peri_clk_ctrl114_reg_t peri_clk_ctrl114;
    volatile hp_sys_clkrst_peri_clk_ctrl115_reg_t peri_clk_ctrl115;
    volatile hp_sys_clkrst_peri_clk_ctrl116_reg_t peri_clk_ctrl116;
    volatile hp_sys_clkrst_peri_clk_ctrl117_reg_t peri_clk_ctrl117;
    volatile hp_sys_clkrst_peri_clk_ctrl118_reg_t peri_clk_ctrl118;
    volatile hp_sys_clkrst_peri_clk_ctrl119_reg_t peri_clk_ctrl119;
    volatile hp_sys_clkrst_peri_clk_ctrl120_reg_t peri_clk_ctrl120;
    volatile hp_sys_clkrst_peri_clk_ctrl20_reg_t peri_clk_ctrl20;
    volatile hp_sys_clkrst_peri_clk_ctrl21_reg_t peri_clk_ctrl21;
    volatile hp_sys_clkrst_peri_clk_ctrl22_reg_t peri_clk_ctrl22;
    volatile hp_sys_clkrst_peri_clk_ctrl23_reg_t peri_clk_ctrl23;
    volatile hp_sys_clkrst_peri_clk_ctrl24_reg_t peri_clk_ctrl24;
    volatile hp_sys_clkrst_peri_clk_ctrl25_reg_t peri_clk_ctrl25;
    volatile hp_sys_clkrst_peri_clk_ctrl26_reg_t peri_clk_ctrl26;
    volatile hp_sys_clkrst_peri_clk_ctrl27_reg_t peri_clk_ctrl27;
    volatile hp_sys_clkrst_clk_force_on_ctrl0_reg_t clk_force_on_ctrl0;
    volatile hp_sys_clkrst_dpa_ctrl0_reg_t dpa_ctrl0;
    volatile hp_sys_clkrst_ana_pll_ctrl0_reg_t ana_pll_ctrl0;
    volatile hp_sys_clkrst_hp_rst_en0_reg_t hp_rst_en0;
    volatile hp_sys_clkrst_hp_rst_en1_reg_t hp_rst_en1;
    volatile hp_sys_clkrst_hp_rst_en2_reg_t hp_rst_en2;
    volatile hp_sys_clkrst_hp_force_norst0_reg_t hp_force_norst0;
    volatile hp_sys_clkrst_hp_force_norst1_reg_t hp_force_norst1;
    volatile hp_sys_clkrst_hpwdt_core0_rst_ctrl0_reg_t hpwdt_core0_rst_ctrl0;
    volatile hp_sys_clkrst_hpwdt_core1_rst_ctrl0_reg_t hpwdt_core1_rst_ctrl0;
    volatile hp_sys_clkrst_cpu_src_freq0_reg_t cpu_src_freq0;
    volatile hp_sys_clkrst_cpu_clk_status0_reg_t cpu_clk_status0;
    volatile hp_sys_clkrst_dbg_clk_ctrl0_reg_t dbg_clk_ctrl0;
    volatile hp_sys_clkrst_dbg_clk_ctrl1_reg_t dbg_clk_ctrl1;
    volatile hp_sys_clkrst_hpcore_wdt_reset_source0_reg_t hpcore_wdt_reset_source0;
} hp_sys_clkrst_dev_t;

extern hp_sys_clkrst_dev_t HP_SYS_CLKRST;


#ifndef __cplusplus
_Static_assert(sizeof(hp_sys_clkrst_dev_t) == 0xf0, "Invalid size of hp_sys_clkrst_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
