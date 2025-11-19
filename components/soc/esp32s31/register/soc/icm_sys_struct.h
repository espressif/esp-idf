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

/** Group: ICM VER DATE REG */
/** Type of ver_date register
 *  NA
 */
typedef union {
    struct {
        /** reg_ver_date : R/W; bitpos: [31:0]; default: 539296517;
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
        /** reg_axi_pdma_int_arqos : R/W; bitpos: [15:12]; default: 0;
         *  NA
         */
        uint32_t reg_axi_pdma_int_arqos:4;
        /** reg_gmac_arqos : R/W; bitpos: [19:16]; default: 0;
         *  NA
         */
        uint32_t reg_gmac_arqos:4;
        /** reg_axi_sdmmc_int_arqos : R/W; bitpos: [23:20]; default: 0;
         *  NA
         */
        uint32_t reg_axi_sdmmc_int_arqos:4;
        uint32_t reserved_24:8;
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
        /** reg_pdma_int_awqos : R/W; bitpos: [15:12]; default: 0;
         *  NA
         */
        uint32_t reg_pdma_int_awqos:4;
        /** reg_gmac_awqos : R/W; bitpos: [19:16]; default: 0;
         *  NA
         */
        uint32_t reg_gmac_awqos:4;
        /** reg_axi_sdmmc_int_awqos : R/W; bitpos: [23:20]; default: 0;
         *  NA
         */
        uint32_t reg_axi_sdmmc_int_awqos:4;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} icm_mst_awqos_reg0_reg_t;


typedef struct {
    volatile icm_ver_date_reg_t ver_date;
    volatile icm_clk_en_reg_t clk_en;
    uint32_t reserved_008[8];
    volatile icm_mst_arqos_reg0_reg_t mst_arqos_reg0;
    uint32_t reserved_02c;
    volatile icm_mst_awqos_reg0_reg_t mst_awqos_reg0;
} icm_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(icm_dev_t) == 0x34, "Invalid size of icm_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
