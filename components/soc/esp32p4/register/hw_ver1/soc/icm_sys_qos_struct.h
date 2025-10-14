/**
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: ICM AXI VERID FIELDS REG */
/** Type of verid_fileds register
 *  NA
 */
typedef union {
    struct {
        /** reg_verid : RO; bitpos: [31:0]; default: 875574314;
         *  NA
         */
        uint32_t reg_verid:32;
    };
    uint32_t val;
} icm_verid_fileds_reg_t;


/** Group: ICM AXI HW CFG REG REG */
/** Type of hw_cfg_reg register
 *  NA
 */
typedef union {
    struct {
        /** reg_axi_hwcfg_qos_support : RO; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t reg_axi_hwcfg_qos_support:1;
        /** reg_axi_hwcfg_apb3_support : RO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t reg_axi_hwcfg_apb3_support:1;
        /** reg_axi_hwcfg_axi4_support : RO; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t reg_axi_hwcfg_axi4_support:1;
        /** reg_axi_hwcfg_lock_en : RO; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t reg_axi_hwcfg_lock_en:1;
        /** reg_axi_hwcfg_trust_zone_en : RO; bitpos: [4]; default: 1;
         *  NA
         */
        uint32_t reg_axi_hwcfg_trust_zone_en:1;
        /** reg_axi_hwcfg_decoder_type : RO; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t reg_axi_hwcfg_decoder_type:1;
        /** reg_axi_hwcfg_remap_en : RO; bitpos: [6]; default: 1;
         *  NA
         */
        uint32_t reg_axi_hwcfg_remap_en:1;
        /** reg_axi_hwcfg_bi_dir_cmd_en : RO; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t reg_axi_hwcfg_bi_dir_cmd_en:1;
        /** reg_axi_hwcfg_low_power_inf_en : RO; bitpos: [8]; default: 1;
         *  NA
         */
        uint32_t reg_axi_hwcfg_low_power_inf_en:1;
        uint32_t reserved_9:3;
        /** reg_axi_hwcfg_axi_num_masters : RO; bitpos: [16:12]; default: 13;
         *  NA
         */
        uint32_t reg_axi_hwcfg_axi_num_masters:5;
        uint32_t reserved_17:3;
        /** reg_axi_hwcfg_axi_num_slaves : RO; bitpos: [24:20]; default: 7;
         *  NA
         */
        uint32_t reg_axi_hwcfg_axi_num_slaves:5;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} icm_hw_cfg_reg_reg_t;


/** Group: ICM AXI CMD REG */
/** Type of cmd register
 *  NA
 */
typedef union {
    struct {
        /** reg_axi_cmd : R/W; bitpos: [2:0]; default: 0;
         *  NA
         */
        uint32_t reg_axi_cmd:3;
        uint32_t reserved_3:4;
        /** reg_rd_wr_chan : R/W; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t reg_rd_wr_chan:1;
        /** reg_axi_master_port : R/W; bitpos: [11:8]; default: 0;
         *  NA
         */
        uint32_t reg_axi_master_port:4;
        uint32_t reserved_12:16;
        /** reg_axi_err_bit : RO; bitpos: [28]; default: 0;
         *  NA
         */
        uint32_t reg_axi_err_bit:1;
        /** reg_axi_soft_reset_bit : R/W; bitpos: [29]; default: 0;
         *  NA
         */
        uint32_t reg_axi_soft_reset_bit:1;
        /** reg_axi_rd_wr_cmd : R/W; bitpos: [30]; default: 0;
         *  NA
         */
        uint32_t reg_axi_rd_wr_cmd:1;
        /** reg_axi_cmd_en : R/W; bitpos: [31]; default: 0;
         *  NA
         */
        uint32_t reg_axi_cmd_en:1;
    };
    uint32_t val;
} icm_cmd_reg_t;


/** Group: ICM AXI DATA REG */
/** Type of data register
 *  NA
 */
typedef union {
    struct {
        /** reg_data : R/W; bitpos: [31:0]; default: 0;
         *  NA
         */
        uint32_t reg_data:32;
    };
    uint32_t val;
} icm_data_reg_t;


typedef struct {
    volatile icm_verid_fileds_reg_t verid_fileds;
    volatile icm_hw_cfg_reg_reg_t hw_cfg_reg;
    volatile icm_cmd_reg_t cmd;
    volatile icm_data_reg_t data;
} axi_icm_qos_dev_t;

extern axi_icm_qos_dev_t AXI_ICM_QOS;


#ifndef __cplusplus
_Static_assert(sizeof(axi_icm_qos_dev_t) == 0x10, "Invalid size of axi_icm_qos_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
