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

/** Group: ICM VER DATE REG */
/** Type of ver_date register
 *  NA
 */
typedef union {
    struct {
        /** reg_ver_date : R/W; bitpos: [31:0]; default: 539165204;
         *  NA
         */
        uint32_t reg_ver_date:32;
    };
    uint32_t val;
} icm_ver_date_reg_t;


/** Group: ICM CLK EN REG */
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
} icm_clk_en_reg_t;


/** Group: ICM DLOCK STATUS REG */
/** Type of dlock_status register
 *  NA
 */
typedef union {
    struct {
        /** reg_dlock_mst : RO; bitpos: [3:0]; default: 0;
         *  Lowest numbered deadlocked master
         */
        uint32_t reg_dlock_mst:4;
        /** reg_dlock_slv : RO; bitpos: [6:4]; default: 0;
         *  Slave with which dlock_mst is deadlocked
         */
        uint32_t reg_dlock_slv:3;
        /** reg_dlock_id : RO; bitpos: [10:7]; default: 0;
         *  AXI ID of deadlocked transaction
         */
        uint32_t reg_dlock_id:4;
        /** reg_dlock_wr : RO; bitpos: [11]; default: 0;
         *  Asserted if deadlocked transaction is a write
         */
        uint32_t reg_dlock_wr:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} icm_dlock_status_reg_t;


/** Group: ICM INT RAW REG */
/** Type of int_raw register
 *  NA
 */
typedef union {
    struct {
        /** reg_dlock_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t reg_dlock_int_raw:1;
        /** reg_icm_sys_addrhole_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t reg_icm_sys_addrhole_int_raw:1;
        /** reg_icm_cpu_addrhole_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t reg_icm_cpu_addrhole_int_raw:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} icm_int_raw_reg_t;


/** Group: ICM INT ST REG */
/** Type of int_st register
 *  NA
 */
typedef union {
    struct {
        /** reg_dlock_int_st : RO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t reg_dlock_int_st:1;
        /** reg_icm_sys_addrhole_int_st : RO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t reg_icm_sys_addrhole_int_st:1;
        /** reg_icm_cpu_addrhole_int_st : RO; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t reg_icm_cpu_addrhole_int_st:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} icm_int_st_reg_t;


/** Group: ICM INT ENA REG */
/** Type of int_ena register
 *  NA
 */
typedef union {
    struct {
        /** reg_dlock_int_ena : R/W; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t reg_dlock_int_ena:1;
        /** reg_icm_sys_addrhole_int_ena : R/W; bitpos: [1]; default: 1;
         *  NA
         */
        uint32_t reg_icm_sys_addrhole_int_ena:1;
        /** reg_icm_cpu_addrhole_int_ena : R/W; bitpos: [2]; default: 1;
         *  NA
         */
        uint32_t reg_icm_cpu_addrhole_int_ena:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} icm_int_ena_reg_t;


/** Group: ICM INT CLR REG */
/** Type of int_clr register
 *  NA
 */
typedef union {
    struct {
        /** reg_dlock_int_clr : WT; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t reg_dlock_int_clr:1;
        /** reg_icm_sys_addrhole_int_clr : WT; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t reg_icm_sys_addrhole_int_clr:1;
        /** reg_icm_cpu_addrhole_int_clr : WT; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t reg_icm_cpu_addrhole_int_clr:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} icm_int_clr_reg_t;


/** Group: ICM MST ARB PRIORITY REG0 REG */
/** Type of mst_arb_priority_reg0 register
 *  NA
 */
typedef union {
    struct {
        /** reg_cpu_priority : R/W; bitpos: [3:0]; default: 0;
         *  CPU arbitration priority for command channels between masters connected to sys_icm
         */
        uint32_t reg_cpu_priority:4;
        /** reg_cache_priority : R/W; bitpos: [7:4]; default: 0;
         *  CACHE arbitration priority for command channels between masters connected to sys_icm
         */
        uint32_t reg_cache_priority:4;
        /** reg_dma2d_priority : R/W; bitpos: [11:8]; default: 0;
         *  GFX arbitration priority for command channels between masters connected to sys_icm
         */
        uint32_t reg_dma2d_priority:4;
        /** reg_gdma_mst1_priority : R/W; bitpos: [15:12]; default: 0;
         *  GDMA mst1 arbitration priority for command channels between masters connected to
         *  sys_icm
         */
        uint32_t reg_gdma_mst1_priority:4;
        /** reg_gdma_mst2_priority : R/W; bitpos: [19:16]; default: 0;
         *  GDMA mst2 arbitration priority for command channels between masters connected to
         *  sys_icm
         */
        uint32_t reg_gdma_mst2_priority:4;
        /** reg_h264_m1_priority : R/W; bitpos: [23:20]; default: 0;
         *  H264 mst1 arbitration priority for command channels between masters connected to
         *  sys_icm
         */
        uint32_t reg_h264_m1_priority:4;
        /** reg_h264_m2_priority : R/W; bitpos: [27:24]; default: 0;
         *  H264 mst2 arbitration priority for command channels between masters connected to
         *  sys_icm
         */
        uint32_t reg_h264_m2_priority:4;
        /** reg_axi_pdma_priority : R/W; bitpos: [31:28]; default: 0;
         *  AXI PDMA  arbitration priority for command channels between masters connected to
         *  sys_icm
         */
        uint32_t reg_axi_pdma_priority:4;
    };
    uint32_t val;
} icm_mst_arb_priority_reg0_reg_t;


/** Group: ICM SLV ARB PRIORITY REG */
/** Type of slv_arb_priority register
 *  NA
 */
typedef union {
    struct {
        uint32_t reserved_0:3;
        /** reg_l2mem_priority : R/W; bitpos: [5:3]; default: 0;
         *  L2MEM arbitration priority for response channels between slaves connected to sys_icm
         */
        uint32_t reg_l2mem_priority:3;
        uint32_t reserved_6:6;
        /** reg_flash_mspi_priority : R/W; bitpos: [14:12]; default: 0;
         *  FLASH MSPI arbitration priority for response channels between slaves connected to
         *  sys_icm
         */
        uint32_t reg_flash_mspi_priority:3;
        /** reg_psram_mspi_priority : R/W; bitpos: [17:15]; default: 0;
         *  PSRAM MSPI arbitration priority for response channels between slaves connected to
         *  sys_icm
         */
        uint32_t reg_psram_mspi_priority:3;
        /** reg_lcd_priority : R/W; bitpos: [20:18]; default: 0;
         *  MIPI_LCD registers arbitration priority for response channels between slaves
         *  connected to sys_icm
         */
        uint32_t reg_lcd_priority:3;
        /** reg_cam_priority : R/W; bitpos: [23:21]; default: 0;
         *  MIPI_CAM registers arbitration priority for response channels between slaves
         *  connected to sys_icm
         */
        uint32_t reg_cam_priority:3;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} icm_slv_arb_priority_reg_t;


/** Group: ICM MST ARQOS REG0 REG */
/** Type of mst_arqos_reg0 register
 *  NA
 */
typedef union {
    struct {
        /** reg_cpu_arqos : R/W; bitpos: [3:0]; default: 0;
         *  NA
         */
        uint32_t reg_cpu_arqos:4;
        /** reg_cache_arqos : R/W; bitpos: [7:4]; default: 0;
         *  NA
         */
        uint32_t reg_cache_arqos:4;
        /** reg_dma2d_arqos : R/W; bitpos: [11:8]; default: 0;
         *  NA
         */
        uint32_t reg_dma2d_arqos:4;
        /** reg_gdma_mst1_arqos : R/W; bitpos: [15:12]; default: 0;
         *  NA
         */
        uint32_t reg_gdma_mst1_arqos:4;
        /** reg_gdma_mst2_arqos : R/W; bitpos: [19:16]; default: 0;
         *  NA
         */
        uint32_t reg_gdma_mst2_arqos:4;
        /** reg_h264_dma2d_m1_arqos : R/W; bitpos: [23:20]; default: 0;
         *  NA
         */
        uint32_t reg_h264_dma2d_m1_arqos:4;
        /** reg_h264_dma2d_m2_arqos : R/W; bitpos: [27:24]; default: 0;
         *  NA
         */
        uint32_t reg_h264_dma2d_m2_arqos:4;
        /** reg_axi_pdma_int_arqos : R/W; bitpos: [31:28]; default: 0;
         *  NA
         */
        uint32_t reg_axi_pdma_int_arqos:4;
    };
    uint32_t val;
} icm_mst_arqos_reg0_reg_t;


/** Group: ICM MST AWQOS REG0 REG */
/** Type of mst_awqos_reg0 register
 *  NA
 */
typedef union {
    struct {
        /** reg_cpu_awqos : R/W; bitpos: [3:0]; default: 0;
         *  NA
         */
        uint32_t reg_cpu_awqos:4;
        /** reg_cache_awqos : R/W; bitpos: [7:4]; default: 0;
         *  NA
         */
        uint32_t reg_cache_awqos:4;
        /** reg_dma2d_awqos : R/W; bitpos: [11:8]; default: 0;
         *  NA
         */
        uint32_t reg_dma2d_awqos:4;
        /** reg_gdma_mst1_awqos : R/W; bitpos: [15:12]; default: 0;
         *  NA
         */
        uint32_t reg_gdma_mst1_awqos:4;
        /** reg_gdma_mst2_awqos : R/W; bitpos: [19:16]; default: 0;
         *  NA
         */
        uint32_t reg_gdma_mst2_awqos:4;
        /** reg_h264_dma2d_m1_awqos : R/W; bitpos: [23:20]; default: 0;
         *  NA
         */
        uint32_t reg_h264_dma2d_m1_awqos:4;
        /** reg_h264_dma2d_m2_awqos : R/W; bitpos: [27:24]; default: 0;
         *  NA
         */
        uint32_t reg_h264_dma2d_m2_awqos:4;
        /** reg_pdma_int_awqos : R/W; bitpos: [31:28]; default: 0;
         *  NA
         */
        uint32_t reg_pdma_int_awqos:4;
    };
    uint32_t val;
} icm_mst_awqos_reg0_reg_t;


/** Group: ICM ADDRHOLE ADDR REG */
/** Type of sys_addrhole_addr register
 *  icm sys addr hole address registers
 */
typedef union {
    struct {
        /** reg_icm_sys_addrhole_addr : RO; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t reg_icm_sys_addrhole_addr:32;
    };
    uint32_t val;
} icm_sys_addrhole_addr_reg_t;

/** Type of cpu_addrhole_addr register
 *  icm cpu addr hole address registers
 */
typedef union {
    struct {
        /** reg_icm_cpu_addrhole_addr : RO; bitpos: [31:0]; default: 0;
         *  It is illegall access address if reg_icm_cpu_addrhole_secure is 1. Otherwise, it
         *  the address without permission to access.
         */
        uint32_t reg_icm_cpu_addrhole_addr:32;
    };
    uint32_t val;
} icm_cpu_addrhole_addr_reg_t;


/** Group: ICM ADDRHOLE INFO REG */
/** Type of sys_addrhole_info register
 *  NA
 */
typedef union {
    struct {
        /** reg_icm_sys_addrhole_id : RO; bitpos: [7:0]; default: 0;
         *  master id = 4-bit CID + 4-bit  UID(refer to related IP) . CID is used to verfiy
         *  master in icm. CID: 4'h1: cache, 4'h5 gdma mst1, 4'h6: gdma mst2, 4'h8: axi pdma,
         *  4'ha: dma2d, 4'hb: h264 mst1, 4'hc: h264 mst2.
         */
        uint32_t reg_icm_sys_addrhole_id:8;
        /** reg_icm_sys_addrhole_wr : RO; bitpos: [8]; default: 0;
         *  1: illegal address access, 0: access without permission
         */
        uint32_t reg_icm_sys_addrhole_wr:1;
        /** reg_icm_sys_addrhole_secure : RO; bitpos: [9]; default: 0;
         *  It is illegall access address if reg_icm_cpu_addrhole_secure is 1, Otherwise, it
         *  the address without permission to access.
         */
        uint32_t reg_icm_sys_addrhole_secure:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} icm_sys_addrhole_info_reg_t;

/** Type of cpu_addrhole_info register
 *  NA
 */
typedef union {
    struct {
        /** reg_icm_cpu_addrhole_id : RO; bitpos: [4:0]; default: 0;
         *  master id: 5'h0: hp core0, 5'h1:hp core1, 5'h2:lp core, 5'h3:usb otg11, 5'h4:
         *  regdma, 5'h5: gmac, 5'h5 sdmmc, 5'h7: usbotg20, 5'h8: trace0, 5'h9: trace1, 5'ha
         *  tcm monitor, 5'hb: l2mem monitor. 5'h10~5'h1f: ahb pdma.
         */
        uint32_t reg_icm_cpu_addrhole_id:5;
        uint32_t reserved_5:3;
        /** reg_icm_cpu_addrhole_wr : RO; bitpos: [8]; default: 0;
         *  1:write trans, 0: read trans.
         */
        uint32_t reg_icm_cpu_addrhole_wr:1;
        /** reg_icm_cpu_addrhole_secure : RO; bitpos: [9]; default: 0;
         *  1: illegal address access, 0: access without permission
         */
        uint32_t reg_icm_cpu_addrhole_secure:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} icm_cpu_addrhole_info_reg_t;


/** Group: ICM DLOCK TIMEOUT REG */
/** Type of dlock_timeout register
 *  NA
 */
typedef union {
    struct {
        /** reg_dlock_timeout : R/W; bitpos: [12:0]; default: 2048;
         *  if no response until reg_dlock_timeout bus clock cycle, deadlock will happen
         */
        uint32_t reg_dlock_timeout:13;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} icm_dlock_timeout_reg_t;


/** Group: ICM RDN ECO CS REG */
/** Type of rdn_eco_cs register
 *  NA
 */
typedef union {
    struct {
        /** reg_rdn_eco_en : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t reg_rdn_eco_en:1;
        /** reg_rdn_eco_result : RO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t reg_rdn_eco_result:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} icm_rdn_eco_cs_reg_t;


/** Group: ICM RDN ECO LOW REG */
/** Type of rdn_eco_low register
 *  NA
 */
typedef union {
    struct {
        /** rdn_eco_low : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t rdn_eco_low:32;
    };
    uint32_t val;
} icm_rdn_eco_low_reg_t;


/** Group: ICM RDN ECO HIGH REG */
/** Type of rdn_eco_high register
 *  NA
 */
typedef union {
    struct {
        /** rdn_eco_high : R/W; bitpos: [31:0]; default: 4294967295;
         *  NA
         */
        uint32_t rdn_eco_high:32;
    };
    uint32_t val;
} icm_rdn_eco_high_reg_t;


typedef struct {
    volatile icm_ver_date_reg_t ver_date;
    volatile icm_clk_en_reg_t clk_en;
    volatile icm_dlock_status_reg_t dlock_status;
    volatile icm_int_raw_reg_t int_raw;
    volatile icm_int_st_reg_t int_st;
    volatile icm_int_ena_reg_t int_ena;
    volatile icm_int_clr_reg_t int_clr;
    volatile icm_mst_arb_priority_reg0_reg_t mst_arb_priority_reg0;
    uint32_t reserved_020;
    volatile icm_slv_arb_priority_reg_t slv_arb_priority;
    volatile icm_mst_arqos_reg0_reg_t mst_arqos_reg0;
    uint32_t reserved_02c;
    volatile icm_mst_awqos_reg0_reg_t mst_awqos_reg0;
    uint32_t reserved_034;
    volatile icm_sys_addrhole_addr_reg_t sys_addrhole_addr;
    volatile icm_sys_addrhole_info_reg_t sys_addrhole_info;
    volatile icm_cpu_addrhole_addr_reg_t cpu_addrhole_addr;
    volatile icm_cpu_addrhole_info_reg_t cpu_addrhole_info;
    volatile icm_dlock_timeout_reg_t dlock_timeout;
    uint32_t reserved_04c;
    volatile icm_rdn_eco_cs_reg_t rdn_eco_cs;
    volatile icm_rdn_eco_low_reg_t rdn_eco_low;
    volatile icm_rdn_eco_high_reg_t rdn_eco_high;
} icm_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(icm_dev_t) == 0x5c, "Invalid size of icm_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
