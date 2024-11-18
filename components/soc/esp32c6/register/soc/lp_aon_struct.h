/**
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: configure_register */
/** Type of store register
 *  need_des
 */
typedef union {
    struct {
        /** store : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t store:32;
    };
    uint32_t val;
} lp_aon_store_reg_t;

/** Type of gpio_mux register
 *  need_des
 */
typedef union {
    struct {
        /** gpio_mux_sel : R/W; bitpos: [7:0]; default: 0;
         *  need_des
         */
        uint32_t gpio_mux_sel:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_aon_gpio_mux_reg_t;

/** Type of gpio_hold0 register
 *  need_des
 */
typedef union {
    struct {
        /** gpio_hold0 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t gpio_hold0:32;
    };
    uint32_t val;
} lp_aon_gpio_hold0_reg_t;

/** Type of gpio_hold1 register
 *  need_des
 */
typedef union {
    struct {
        /** gpio_hold1 : R/W; bitpos: [31:0]; default: 0;
         *  need_des
         */
        uint32_t gpio_hold1:32;
    };
    uint32_t val;
} lp_aon_gpio_hold1_reg_t;

/** Type of sys_cfg register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** force_download_boot : R/W; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t force_download_boot:1;
        /** hpsys_sw_reset : WT; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t hpsys_sw_reset:1;
    };
    uint32_t val;
} lp_aon_sys_cfg_reg_t;

/** Type of cpucore0_cfg register
 *  need_des
 */
typedef union {
    struct {
        /** cpu_core0_sw_stall : R/W; bitpos: [7:0]; default: 0;
         *  need_des
         */
        uint32_t cpu_core0_sw_stall:8;
        uint32_t reserved_8:20;
        /** cpu_core0_sw_reset : WT; bitpos: [28]; default: 0;
         *  need_des
         */
        uint32_t cpu_core0_sw_reset:1;
        /** cpu_core0_ocd_halt_on_reset : R/W; bitpos: [29]; default: 0;
         *  need_des
         */
        uint32_t cpu_core0_ocd_halt_on_reset:1;
        /** cpu_core0_stat_vector_sel : R/W; bitpos: [30]; default: 1;
         *  need_des
         */
        uint32_t cpu_core0_stat_vector_sel:1;
        /** cpu_core0_dreset_mask : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t cpu_core0_dreset_mask:1;
    };
    uint32_t val;
} lp_aon_cpucore0_cfg_reg_t;

/** Type of io_mux register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** io_mux_reset_disable : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t io_mux_reset_disable:1;
    };
    uint32_t val;
} lp_aon_io_mux_reg_t;

/** Type of ext_wakeup_cntl register
 *  need_des
 */
typedef union {
    struct {
        /** ext_wakeup_status : RO; bitpos: [7:0]; default: 0;
         *  need_des
         */
        uint32_t ext_wakeup_status:8;
        uint32_t reserved_8:6;
        /** ext_wakeup_status_clr : WT; bitpos: [14]; default: 0;
         *  need_des
         */
        uint32_t ext_wakeup_status_clr:1;
        /** ext_wakeup_sel : R/W; bitpos: [22:15]; default: 0;
         *  need_des
         */
        uint32_t ext_wakeup_sel:8;
        /** ext_wakeup_lv : R/W; bitpos: [30:23]; default: 0;
         *  need_des
         */
        uint32_t ext_wakeup_lv:8;
        /** ext_wakeup_filter : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t ext_wakeup_filter:1;
    };
    uint32_t val;
} lp_aon_ext_wakeup_cntl_reg_t;

/** Type of usb register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** usb_reset_disable : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t usb_reset_disable:1;
    };
    uint32_t val;
} lp_aon_usb_reg_t;

/** Type of lpbus register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:16;
        /** fast_mem_wpulse : R/W; bitpos: [18:16]; default: 0;
         *  This field controls fast memory WPULSE parameter.
         */
        uint32_t fast_mem_wpulse:3;
        /** fast_mem_wa : R/W; bitpos: [21:19]; default: 4;
         *  This field controls fast memory WA parameter.
         */
        uint32_t fast_mem_wa:3;
        /** fast_mem_ra : R/W; bitpos: [23:22]; default: 0;
         *  This field controls fast memory RA parameter.
         */
        uint32_t fast_mem_ra:2;
        uint32_t reserved_24:4;
        /** fast_mem_mux_fsm_idle : RO; bitpos: [28]; default: 1;
         *  need_des
         */
        uint32_t fast_mem_mux_fsm_idle:1;
        /** fast_mem_mux_sel_status : RO; bitpos: [29]; default: 1;
         *  need_des
         */
        uint32_t fast_mem_mux_sel_status:1;
        /** fast_mem_mux_sel_update : WT; bitpos: [30]; default: 0;
         *  need_des
         */
        uint32_t fast_mem_mux_sel_update:1;
        /** fast_mem_mux_sel : R/W; bitpos: [31]; default: 1;
         *  need_des
         */
        uint32_t fast_mem_mux_sel:1;
    };
    uint32_t val;
} lp_aon_lpbus_reg_t;

/** Type of sdio_active register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:22;
        /** sdio_act_dnum : R/W; bitpos: [31:22]; default: 10;
         *  need_des
         */
        uint32_t sdio_act_dnum:10;
    };
    uint32_t val;
} lp_aon_sdio_active_reg_t;

/** Type of lpcore register
 *  need_des
 */
typedef union {
    struct {
        /** lpcore_etm_wakeup_flag_clr : WT; bitpos: [0]; default: 0;
         *  need_des
         */
        uint32_t lpcore_etm_wakeup_flag_clr:1;
        /** lpcore_etm_wakeup_flag : R/WTC/SS; bitpos: [1]; default: 0;
         *  need_des
         */
        uint32_t lpcore_etm_wakeup_flag:1;
        uint32_t reserved_2:29;
        /** lpcore_disable : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t lpcore_disable:1;
    };
    uint32_t val;
} lp_aon_lpcore_reg_t;

/** Type of sar_cct register
 *  need_des
 */
typedef union {
    struct {
        uint32_t reserved_0:29;
        /** sar2_pwdet_cct : R/W; bitpos: [31:29]; default: 0;
         *  need_des
         */
        uint32_t sar2_pwdet_cct:3;
    };
    uint32_t val;
} lp_aon_sar_cct_reg_t;

/** Type of date register
 *  need_des
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [30:0]; default: 35672704;
         *  need_des
         */
        uint32_t date:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  need_des
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} lp_aon_date_reg_t;


typedef struct lp_aon_dev_t {
    volatile lp_aon_store_reg_t store[10];
    volatile lp_aon_gpio_mux_reg_t gpio_mux;
    volatile lp_aon_gpio_hold0_reg_t gpio_hold0;
    volatile lp_aon_gpio_hold1_reg_t gpio_hold1;
    volatile lp_aon_sys_cfg_reg_t sys_cfg;
    volatile lp_aon_cpucore0_cfg_reg_t cpucore0_cfg;
    volatile lp_aon_io_mux_reg_t io_mux;
    volatile lp_aon_ext_wakeup_cntl_reg_t ext_wakeup_cntl;
    volatile lp_aon_usb_reg_t usb;
    volatile lp_aon_lpbus_reg_t lpbus;
    volatile lp_aon_sdio_active_reg_t sdio_active;
    volatile lp_aon_lpcore_reg_t lpcore;
    volatile lp_aon_sar_cct_reg_t sar_cct;
    uint32_t reserved_058[233];
    volatile lp_aon_date_reg_t date;
} lp_aon_dev_t;

extern lp_aon_dev_t LP_AON;

#ifndef __cplusplus
_Static_assert(sizeof(lp_aon_dev_t) == 0x400, "Invalid size of lp_aon_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
