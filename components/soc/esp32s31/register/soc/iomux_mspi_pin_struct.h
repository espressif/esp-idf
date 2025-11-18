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

/** Group: clk_en */
/** Type of clk_en0 register
 *  apb registers auto clock gating reg
 */
typedef union {
    struct {
        /** reg_clk_en : R/W; bitpos: [0]; default: 1;
         *  1: auto clock gating on
         *  0: auto clock gating off
         */
        uint32_t reg_clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} iomux_mspi_pin_clk_en0_reg_t;


/** Group: psram_d_pin */
/** Type of psram_d_pin0 register
 *  IOMUX_MSPI_PIN_PSRAM_D_PIN0_REG
 */
typedef union {
    struct {
        /** reg_psram_d_dli : R/W; bitpos: [3:0]; default: 0;
         *  psram d dli
         */
        uint32_t reg_psram_d_dli:4;
        /** reg_psram_d_dlc : R/W; bitpos: [7:4]; default: 0;
         *  psram d dlc
         */
        uint32_t reg_psram_d_dlc:4;
        /** reg_psram_d_hys : R/W; bitpos: [8]; default: 0;
         *  psram d sl
         */
        uint32_t reg_psram_d_hys:1;
        /** reg_psram_d_ie : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_psram_d_ie:1;
        /** reg_psram_d_wpu : R/W; bitpos: [10]; default: 0;
         *  psram d wpu
         */
        uint32_t reg_psram_d_wpu:1;
        /** reg_psram_d_wpd : R/W; bitpos: [11]; default: 0;
         *  psram d wpd
         */
        uint32_t reg_psram_d_wpd:1;
        /** reg_psram_d_drv : R/W; bitpos: [14:12]; default: 4;
         *  psram d drv
         */
        uint32_t reg_psram_d_drv:3;
        /** reg_psram_d_hold : R/W; bitpos: [15]; default: 0;
         *  psram d hold
         */
        uint32_t reg_psram_d_hold:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} iomux_mspi_pin_psram_d_pin0_reg_t;


/** Group: psram_q_pin */
/** Type of psram_q_pin0 register
 *  IOMUX_MSPI_PIN_PSRAM_D_PIN0_REG
 */
typedef union {
    struct {
        /** reg_psram_q_dli : R/W; bitpos: [3:0]; default: 0;
         *  psram q dli
         */
        uint32_t reg_psram_q_dli:4;
        /** reg_psram_q_dlc : R/W; bitpos: [7:4]; default: 0;
         *  psram q dlc
         */
        uint32_t reg_psram_q_dlc:4;
        /** reg_psram_q_hys : R/W; bitpos: [8]; default: 0;
         *  psram q sl
         */
        uint32_t reg_psram_q_hys:1;
        /** reg_psram_q_ie : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_psram_q_ie:1;
        /** reg_psram_q_wpu : R/W; bitpos: [10]; default: 0;
         *  psram q wpu
         */
        uint32_t reg_psram_q_wpu:1;
        /** reg_psram_q_wpd : R/W; bitpos: [11]; default: 0;
         *  psram q wpd
         */
        uint32_t reg_psram_q_wpd:1;
        /** reg_psram_q_drv : R/W; bitpos: [14:12]; default: 4;
         *  psram q drv
         */
        uint32_t reg_psram_q_drv:3;
        /** reg_psram_q_hold : R/W; bitpos: [15]; default: 0;
         *  psram q hold
         */
        uint32_t reg_psram_q_hold:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} iomux_mspi_pin_psram_q_pin0_reg_t;


/** Group: psram_wp_pin */
/** Type of psram_wp_pin0 register
 *  IOMUX_MSPI_PIN_PSRAM_D_PIN0_REG
 */
typedef union {
    struct {
        /** reg_psram_wp_dli : R/W; bitpos: [3:0]; default: 0;
         *  psram wp dli
         */
        uint32_t reg_psram_wp_dli:4;
        /** reg_psram_wp_dlc : R/W; bitpos: [7:4]; default: 0;
         *  psram wp dlc
         */
        uint32_t reg_psram_wp_dlc:4;
        /** reg_psram_wp_hys : R/W; bitpos: [8]; default: 0;
         *  psram wp sl
         */
        uint32_t reg_psram_wp_hys:1;
        /** reg_psram_wp_ie : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_psram_wp_ie:1;
        /** reg_psram_wp_wpu : R/W; bitpos: [10]; default: 0;
         *  psram wp wpu
         */
        uint32_t reg_psram_wp_wpu:1;
        /** reg_psram_wp_wpd : R/W; bitpos: [11]; default: 0;
         *  psram wp wpd
         */
        uint32_t reg_psram_wp_wpd:1;
        /** reg_psram_wp_drv : R/W; bitpos: [14:12]; default: 4;
         *  psram wp drv
         */
        uint32_t reg_psram_wp_drv:3;
        /** reg_psram_wp_hold : R/W; bitpos: [15]; default: 0;
         *  psram wp hold
         */
        uint32_t reg_psram_wp_hold:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} iomux_mspi_pin_psram_wp_pin0_reg_t;


/** Group: psram_hold_pin */
/** Type of psram_hold_pin0 register
 *  IOMUX_MSPI_PIN_PSRAM_D_PIN0_REG
 */
typedef union {
    struct {
        /** reg_psram_hold_dli : R/W; bitpos: [3:0]; default: 0;
         *  psram hold dli
         */
        uint32_t reg_psram_hold_dli:4;
        /** reg_psram_hold_dlc : R/W; bitpos: [7:4]; default: 0;
         *  psram hold dlc
         */
        uint32_t reg_psram_hold_dlc:4;
        /** reg_psram_hold_hys : R/W; bitpos: [8]; default: 0;
         *  psram hold sl
         */
        uint32_t reg_psram_hold_hys:1;
        /** reg_psram_hold_ie : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_psram_hold_ie:1;
        /** reg_psram_hold_wpu : R/W; bitpos: [10]; default: 0;
         *  psram hold wpu
         */
        uint32_t reg_psram_hold_wpu:1;
        /** reg_psram_hold_wpd : R/W; bitpos: [11]; default: 0;
         *  psram hold wpd
         */
        uint32_t reg_psram_hold_wpd:1;
        /** reg_psram_hold_drv : R/W; bitpos: [14:12]; default: 4;
         *  psram hold drv
         */
        uint32_t reg_psram_hold_drv:3;
        /** reg_psram_hold_hold : R/W; bitpos: [15]; default: 0;
         *  psram hold hold
         */
        uint32_t reg_psram_hold_hold:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} iomux_mspi_pin_psram_hold_pin0_reg_t;


/** Group: psram_dq4_pin */
/** Type of psram_dq4_pin0 register
 *  IOMUX_MSPI_PIN_PSRAM_D_PIN0_REG
 */
typedef union {
    struct {
        /** reg_psram_dq4_dli : R/W; bitpos: [3:0]; default: 0;
         *  psram dq4 dli
         */
        uint32_t reg_psram_dq4_dli:4;
        /** reg_psram_dq4_dlc : R/W; bitpos: [7:4]; default: 0;
         *  psram dq4 dlc
         */
        uint32_t reg_psram_dq4_dlc:4;
        /** reg_psram_dq4_hys : R/W; bitpos: [8]; default: 0;
         *  psram dq4 sl
         */
        uint32_t reg_psram_dq4_hys:1;
        /** reg_psram_dq4_ie : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_psram_dq4_ie:1;
        /** reg_psram_dq4_wpu : R/W; bitpos: [10]; default: 0;
         *  psram dq4 wpu
         */
        uint32_t reg_psram_dq4_wpu:1;
        /** reg_psram_dq4_wpd : R/W; bitpos: [11]; default: 0;
         *  psram dq4 wpd
         */
        uint32_t reg_psram_dq4_wpd:1;
        /** reg_psram_dq4_drv : R/W; bitpos: [14:12]; default: 4;
         *  psram dq4 drv
         */
        uint32_t reg_psram_dq4_drv:3;
        /** reg_psram_dq4_hold : R/W; bitpos: [15]; default: 0;
         *  psram dq4 hold
         */
        uint32_t reg_psram_dq4_hold:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} iomux_mspi_pin_psram_dq4_pin0_reg_t;


/** Group: psram_dq5_pin */
/** Type of psram_dq5_pin0 register
 *  IOMUX_MSPI_PIN_PSRAM_D_PIN0_REG
 */
typedef union {
    struct {
        /** reg_psram_dq5_dli : R/W; bitpos: [3:0]; default: 0;
         *  psram dq5 dli
         */
        uint32_t reg_psram_dq5_dli:4;
        /** reg_psram_dq5_dlc : R/W; bitpos: [7:4]; default: 0;
         *  psram dq5 dlc
         */
        uint32_t reg_psram_dq5_dlc:4;
        /** reg_psram_dq5_hys : R/W; bitpos: [8]; default: 0;
         *  psram dq5 sl
         */
        uint32_t reg_psram_dq5_hys:1;
        /** reg_psram_dq5_ie : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_psram_dq5_ie:1;
        /** reg_psram_dq5_wpu : R/W; bitpos: [10]; default: 0;
         *  psram dq5 wpu
         */
        uint32_t reg_psram_dq5_wpu:1;
        /** reg_psram_dq5_wpd : R/W; bitpos: [11]; default: 0;
         *  psram dq5 wpd
         */
        uint32_t reg_psram_dq5_wpd:1;
        /** reg_psram_dq5_drv : R/W; bitpos: [14:12]; default: 4;
         *  psram dq5 drv
         */
        uint32_t reg_psram_dq5_drv:3;
        /** reg_psram_dq5_hold : R/W; bitpos: [15]; default: 0;
         *  psram dq5 hold
         */
        uint32_t reg_psram_dq5_hold:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} iomux_mspi_pin_psram_dq5_pin0_reg_t;


/** Group: psram_dq6_pin */
/** Type of psram_dq6_pin0 register
 *  IOMUX_MSPI_PIN_PSRAM_D_PIN0_REG
 */
typedef union {
    struct {
        /** reg_psram_dq6_dli : R/W; bitpos: [3:0]; default: 0;
         *  psram dq6 dli
         */
        uint32_t reg_psram_dq6_dli:4;
        /** reg_psram_dq6_dlc : R/W; bitpos: [7:4]; default: 0;
         *  psram dq6 dlc
         */
        uint32_t reg_psram_dq6_dlc:4;
        /** reg_psram_dq6_hys : R/W; bitpos: [8]; default: 0;
         *  psram dq6 sl
         */
        uint32_t reg_psram_dq6_hys:1;
        /** reg_psram_dq6_ie : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_psram_dq6_ie:1;
        /** reg_psram_dq6_wpu : R/W; bitpos: [10]; default: 0;
         *  psram dq6 wpu
         */
        uint32_t reg_psram_dq6_wpu:1;
        /** reg_psram_dq6_wpd : R/W; bitpos: [11]; default: 0;
         *  psram dq6 wpd
         */
        uint32_t reg_psram_dq6_wpd:1;
        /** reg_psram_dq6_drv : R/W; bitpos: [14:12]; default: 4;
         *  psram dq6 drv
         */
        uint32_t reg_psram_dq6_drv:3;
        /** reg_psram_dq6_hold : R/W; bitpos: [15]; default: 0;
         *  psram dq6 hold
         */
        uint32_t reg_psram_dq6_hold:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} iomux_mspi_pin_psram_dq6_pin0_reg_t;


/** Group: psram_dq7_pin */
/** Type of psram_dq7_pin0 register
 *  IOMUX_MSPI_PIN_PSRAM_D_PIN0_REG
 */
typedef union {
    struct {
        /** reg_psram_dq7_dli : R/W; bitpos: [3:0]; default: 0;
         *  psram dq7 dli
         */
        uint32_t reg_psram_dq7_dli:4;
        /** reg_psram_dq7_dlc : R/W; bitpos: [7:4]; default: 0;
         *  psram dq7 dlc
         */
        uint32_t reg_psram_dq7_dlc:4;
        /** reg_psram_dq7_hys : R/W; bitpos: [8]; default: 0;
         *  psram dq7 sl
         */
        uint32_t reg_psram_dq7_hys:1;
        /** reg_psram_dq7_ie : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_psram_dq7_ie:1;
        /** reg_psram_dq7_wpu : R/W; bitpos: [10]; default: 0;
         *  psram dq7 wpu
         */
        uint32_t reg_psram_dq7_wpu:1;
        /** reg_psram_dq7_wpd : R/W; bitpos: [11]; default: 0;
         *  psram dq7 wpd
         */
        uint32_t reg_psram_dq7_wpd:1;
        /** reg_psram_dq7_drv : R/W; bitpos: [14:12]; default: 4;
         *  psram dq7 drv
         */
        uint32_t reg_psram_dq7_drv:3;
        /** reg_psram_dq7_hold : R/W; bitpos: [15]; default: 0;
         *  psram dq7 hold
         */
        uint32_t reg_psram_dq7_hold:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} iomux_mspi_pin_psram_dq7_pin0_reg_t;


/** Group: psram_dqs_0_pin */
/** Type of psram_dqs_0_pin0 register
 *  IOMUX_MSPI_PIN_PSRAM_D_PIN0_REG
 */
typedef union {
    struct {
        /** reg_psram_dqs_0_xpd : R/W; bitpos: [0]; default: 0;
         *  psram xpd dqs0
         */
        uint32_t reg_psram_dqs_0_xpd:1;
        /** reg_psram_dqs_0_phase : R/W; bitpos: [2:1]; default: 0;
         *  psram dqs0 phase
         */
        uint32_t reg_psram_dqs_0_phase:2;
        /** reg_psram_dqs_0_dli : R/W; bitpos: [6:3]; default: 0;
         *  psram dqs0 dli
         */
        uint32_t reg_psram_dqs_0_dli:4;
        /** reg_psram_dqs_0_delay_90 : R/W; bitpos: [10:7]; default: 0;
         *  psram dqs0 delay 90
         */
        uint32_t reg_psram_dqs_0_delay_90:4;
        /** reg_psram_dqs_0_hys : R/W; bitpos: [11]; default: 0;
         *  psram dqs0 sl
         */
        uint32_t reg_psram_dqs_0_hys:1;
        /** reg_psram_dqs_0_ie : R/W; bitpos: [12]; default: 0;
         *  Reserved
         */
        uint32_t reg_psram_dqs_0_ie:1;
        /** reg_psram_dqs_0_wpu : R/W; bitpos: [13]; default: 0;
         *  psram dqs0 wpu
         */
        uint32_t reg_psram_dqs_0_wpu:1;
        /** reg_psram_dqs_0_wpd : R/W; bitpos: [14]; default: 0;
         *  psram dqs0 wpd
         */
        uint32_t reg_psram_dqs_0_wpd:1;
        /** reg_psram_dqs_0_drv : R/W; bitpos: [17:15]; default: 4;
         *  psram dqs0 drv
         */
        uint32_t reg_psram_dqs_0_drv:3;
        /** reg_psram_dqs_0_delay_270 : R/W; bitpos: [21:18]; default: 0;
         *  psram dqs0 delay 270
         */
        uint32_t reg_psram_dqs_0_delay_270:4;
        /** reg_psram_dqs_hold : R/W; bitpos: [22]; default: 0;
         *  psram dqs hold
         */
        uint32_t reg_psram_dqs_hold:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} iomux_mspi_pin_psram_dqs_0_pin0_reg_t;


/** Group: psram_ck_pin */
/** Type of psram_ck_pin0 register
 *  IOMUX_MSPI_PIN_PSRAM_D_PIN0_REG
 */
typedef union {
    struct {
        /** reg_psram_ck_dli : R/W; bitpos: [3:0]; default: 0;
         *  psram ck dli
         */
        uint32_t reg_psram_ck_dli:4;
        /** reg_psram_ck_dlc : R/W; bitpos: [7:4]; default: 0;
         *  psram ck dlc
         */
        uint32_t reg_psram_ck_dlc:4;
        /** reg_psram_ck_hys : R/W; bitpos: [8]; default: 0;
         *  psram ck sl
         */
        uint32_t reg_psram_ck_hys:1;
        /** reg_psram_ck_ie : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_psram_ck_ie:1;
        /** reg_psram_ck_wpu : R/W; bitpos: [10]; default: 0;
         *  psram ck wpu
         */
        uint32_t reg_psram_ck_wpu:1;
        /** reg_psram_ck_wpd : R/W; bitpos: [11]; default: 0;
         *  psram ck wpd
         */
        uint32_t reg_psram_ck_wpd:1;
        /** reg_psram_ck_drv : R/W; bitpos: [14:12]; default: 4;
         *  psram ck drv
         */
        uint32_t reg_psram_ck_drv:3;
        /** reg_psram_ck_hold : R/W; bitpos: [15]; default: 0;
         *  psram ck hold
         */
        uint32_t reg_psram_ck_hold:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} iomux_mspi_pin_psram_ck_pin0_reg_t;


/** Group: psram_cs_pin */
/** Type of psram_cs_pin0 register
 *  IOMUX_MSPI_PIN_PSRAM_D_PIN0_REG
 */
typedef union {
    struct {
        /** reg_psram_cs_dli : R/W; bitpos: [3:0]; default: 0;
         *  psram cs dli
         */
        uint32_t reg_psram_cs_dli:4;
        /** reg_psram_cs_dlc : R/W; bitpos: [7:4]; default: 0;
         *  psram cs dlc
         */
        uint32_t reg_psram_cs_dlc:4;
        /** reg_psram_cs_hys : R/W; bitpos: [8]; default: 0;
         *  psram cs sl
         */
        uint32_t reg_psram_cs_hys:1;
        /** reg_psram_cs_ie : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_psram_cs_ie:1;
        /** reg_psram_cs_wpu : R/W; bitpos: [10]; default: 0;
         *  psram cs wpu
         */
        uint32_t reg_psram_cs_wpu:1;
        /** reg_psram_cs_wpd : R/W; bitpos: [11]; default: 0;
         *  psram cs wpd
         */
        uint32_t reg_psram_cs_wpd:1;
        /** reg_psram_cs_drv : R/W; bitpos: [14:12]; default: 4;
         *  psram cs drv
         */
        uint32_t reg_psram_cs_drv:3;
        /** reg_psram_cs_hold : R/W; bitpos: [15]; default: 0;
         *  psram cs hold
         */
        uint32_t reg_psram_cs_hold:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} iomux_mspi_pin_psram_cs_pin0_reg_t;


typedef struct {
    volatile iomux_mspi_pin_clk_en0_reg_t clk_en0;
    uint32_t reserved_004[6];
    volatile iomux_mspi_pin_psram_d_pin0_reg_t psram_d_pin0;
    volatile iomux_mspi_pin_psram_q_pin0_reg_t psram_q_pin0;
    volatile iomux_mspi_pin_psram_wp_pin0_reg_t psram_wp_pin0;
    volatile iomux_mspi_pin_psram_hold_pin0_reg_t psram_hold_pin0;
    volatile iomux_mspi_pin_psram_dq4_pin0_reg_t psram_dq4_pin0;
    volatile iomux_mspi_pin_psram_dq5_pin0_reg_t psram_dq5_pin0;
    volatile iomux_mspi_pin_psram_dq6_pin0_reg_t psram_dq6_pin0;
    volatile iomux_mspi_pin_psram_dq7_pin0_reg_t psram_dq7_pin0;
    volatile iomux_mspi_pin_psram_dqs_0_pin0_reg_t psram_dqs_0_pin0;
    volatile iomux_mspi_pin_psram_ck_pin0_reg_t psram_ck_pin0;
    volatile iomux_mspi_pin_psram_cs_pin0_reg_t psram_cs_pin0;
} iomux_mspi_pin_dev_t;

extern iomux_mspi_pin_dev_t MSPI_IOMUX;


#ifndef __cplusplus
_Static_assert(sizeof(iomux_mspi_pin_dev_t) == 0x48, "Invalid size of iomux_mspi_pin_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
