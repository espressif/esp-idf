/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: configure_register */
/** Type of ver_date register
 *  need_des
 */
typedef union {
    struct {
        /** ver_date : R/W; bitpos: [30:0]; default: 2426628;
         *  need_des
         */
        uint32_t ver_date:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} lp_adi_ver_date_reg_t;

/** Type of dcdc_top_2_sts register
 *  ANALOG DCDC 2 related status
 */
typedef union {
    struct {
        /** dcdc_top_2_cal_done : RO; bitpos: [0]; default: 0;
         *  cali done signal from dcdc 2 top
         */
        uint32_t dcdc_top_2_cal_done:1;
        /** dcdc_top_2_in_vcm : RO; bitpos: [1]; default: 0;
         *  in vcm signal from dcdc 2 top
         */
        uint32_t dcdc_top_2_in_vcm:1;
        /** dcdc_top_2_in_ccm : RO; bitpos: [2]; default: 0;
         *  in ccm signal from dcdc 2 top
         */
        uint32_t dcdc_top_2_in_ccm:1;
        /** dcdc_top_2_vcm_ready : RO; bitpos: [3]; default: 0;
         *  vcm_ready signal from dcdc 2 top
         */
        uint32_t dcdc_top_2_vcm_ready:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} lp_adi_dcdc_top_2_sts_reg_t;

/** Type of dcdc_top_2_cfg register
 *  ANALOG DCDC 2 related CONFIG
 */
typedef union {
    struct {
        /** dcdc_top_2_dreg_mode : R/W; bitpos: [1:0]; default: 3;
         *  DCDC 2 dreg mode
         */
        uint32_t dcdc_top_2_dreg_mode:2;
        /** dcdc_top_2_clear_vcm_rdy : R/W; bitpos: [2]; default: 0;
         *  1: clear vcm rdy signal
         */
        uint32_t dcdc_top_2_clear_vcm_rdy:1;
        /** dcdc_top_2_ifsm_pcur_limit : R/W; bitpos: [5:3]; default: 0;
         *  Current limit config
         */
        uint32_t dcdc_top_2_ifsm_pcur_limit:3;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} lp_adi_dcdc_top_2_cfg_reg_t;

/** Type of pm_top_sts register
 *  ANALOG PM TOP related status
 */
typedef union {
    struct {
        /** pm_top_cal_done : RO; bitpos: [0]; default: 0;
         *  cali done signal from PM TOP
         */
        uint32_t pm_top_cal_done:1;
        /** pm_top_dcdc_vcm_rdy : RO; bitpos: [1]; default: 0;
         *  vcm_ready signal from PM TOP
         */
        uint32_t pm_top_dcdc_vcm_rdy:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} lp_adi_pm_top_sts_reg_t;

/** Type of pm_top_cfg register
 *  ANALOG PM TOP related CONFIG
 */
typedef union {
    struct {
        /** pm_top_dreg_mode : R/W; bitpos: [1:0]; default: 3;
         *  PM TOP dreg mode
         */
        uint32_t pm_top_dreg_mode:2;
        /** pm_top_dcdc_clr_vcm_rdy : R/W; bitpos: [2]; default: 0;
         *  1: clear vcm rdy signal
         */
        uint32_t pm_top_dcdc_clr_vcm_rdy:1;
        /** pm_top_dig_reg_dsfmos : R/W; bitpos: [5:3]; default: 0;
         *  Current limit config
         */
        uint32_t pm_top_dig_reg_dsfmos:3;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} lp_adi_pm_top_cfg_reg_t;


typedef struct {
    volatile lp_adi_ver_date_reg_t ver_date;
    volatile lp_adi_dcdc_top_2_sts_reg_t dcdc_top_2_sts;
    volatile lp_adi_dcdc_top_2_cfg_reg_t dcdc_top_2_cfg;
    volatile lp_adi_pm_top_sts_reg_t pm_top_sts;
    volatile lp_adi_pm_top_cfg_reg_t pm_top_cfg;
} lp_adi_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(lp_adi_dev_t) == 0x14, "Invalid size of lp_adi_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
