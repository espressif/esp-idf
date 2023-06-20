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


/** Group: flash_cs_pin */
/** Type of flash_cs_pin0 register
 *  IOMUX_MSPI_PIN_FLASH_CS_PIN0_REG
 */
typedef union {
    struct {
        /** reg_flash_cs_hys : R/W; bitpos: [0]; default: 0;
         *  flash cs hys
         */
        uint32_t reg_flash_cs_hys:1;
        /** reg_flash_cs_ie : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_flash_cs_ie:1;
        /** reg_flash_cs_wpu : R/W; bitpos: [2]; default: 0;
         *  flash cs wpu
         */
        uint32_t reg_flash_cs_wpu:1;
        /** reg_flash_cs_wpd : R/W; bitpos: [3]; default: 0;
         *  flash cs wpd
         */
        uint32_t reg_flash_cs_wpd:1;
        /** reg_flash_cs_drv : R/W; bitpos: [5:4]; default: 0;
         *  flash cs drv
         */
        uint32_t reg_flash_cs_drv:2;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} iomux_mspi_pin_flash_cs_pin0_reg_t;


/** Group: flash_q_pin */
/** Type of flash_q_pin0 register
 *  IOMUX_MSPI_PIN_FLASH_Q_PIN0_REG
 */
typedef union {
    struct {
        /** reg_flash_q_hys : R/W; bitpos: [0]; default: 0;
         *  flash q hys
         */
        uint32_t reg_flash_q_hys:1;
        /** reg_flash_q_ie : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_flash_q_ie:1;
        /** reg_flash_q_wpu : R/W; bitpos: [2]; default: 0;
         *  flash q wpu
         */
        uint32_t reg_flash_q_wpu:1;
        /** reg_flash_q_wpd : R/W; bitpos: [3]; default: 0;
         *  flash q wpd
         */
        uint32_t reg_flash_q_wpd:1;
        /** reg_flash_q_drv : R/W; bitpos: [5:4]; default: 0;
         *  flash q drv
         */
        uint32_t reg_flash_q_drv:2;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} iomux_mspi_pin_flash_q_pin0_reg_t;


/** Group: flash_wp_pin */
/** Type of flash_wp_pin0 register
 *  IOMUX_MSPI_PIN_FLASH_WP_PIN0_REG
 */
typedef union {
    struct {
        /** reg_flash_wp_hys : R/W; bitpos: [0]; default: 0;
         *  flash wp hys
         */
        uint32_t reg_flash_wp_hys:1;
        /** reg_flash_wp_ie : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_flash_wp_ie:1;
        /** reg_flash_wp_wpu : R/W; bitpos: [2]; default: 0;
         *  flash wp wpu
         */
        uint32_t reg_flash_wp_wpu:1;
        /** reg_flash_wp_wpd : R/W; bitpos: [3]; default: 0;
         *  flash wp wpd
         */
        uint32_t reg_flash_wp_wpd:1;
        /** reg_flash_wp_drv : R/W; bitpos: [5:4]; default: 0;
         *  flash wp drv
         */
        uint32_t reg_flash_wp_drv:2;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} iomux_mspi_pin_flash_wp_pin0_reg_t;


/** Group: flash_hold_pin */
/** Type of flash_hold_pin0 register
 *  IOMUX_MSPI_PIN_FLASH_HOLD_PIN0_REG
 */
typedef union {
    struct {
        /** reg_flash_hold_hys : R/W; bitpos: [0]; default: 0;
         *  flash hold hys
         */
        uint32_t reg_flash_hold_hys:1;
        /** reg_flash_hold_ie : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_flash_hold_ie:1;
        /** reg_flash_hold_wpu : R/W; bitpos: [2]; default: 0;
         *  flash hold wpu
         */
        uint32_t reg_flash_hold_wpu:1;
        /** reg_flash_hold_wpd : R/W; bitpos: [3]; default: 0;
         *  flash hold wpd
         */
        uint32_t reg_flash_hold_wpd:1;
        /** reg_flash_hold_drv : R/W; bitpos: [5:4]; default: 0;
         *  flash hold drv
         */
        uint32_t reg_flash_hold_drv:2;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} iomux_mspi_pin_flash_hold_pin0_reg_t;


/** Group: flash_ck_pin */
/** Type of flash_ck_pin0 register
 *  IOMUX_MSPI_PIN_FLASH_CK_PIN0_REG
 */
typedef union {
    struct {
        /** reg_flash_ck_hys : R/W; bitpos: [0]; default: 0;
         *  flash ck hys
         */
        uint32_t reg_flash_ck_hys:1;
        /** reg_flash_ck_ie : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_flash_ck_ie:1;
        /** reg_flash_ck_wpu : R/W; bitpos: [2]; default: 0;
         *  flash ck wpu
         */
        uint32_t reg_flash_ck_wpu:1;
        /** reg_flash_ck_wpd : R/W; bitpos: [3]; default: 0;
         *  flash ck wpd
         */
        uint32_t reg_flash_ck_wpd:1;
        /** reg_flash_ck_drv : R/W; bitpos: [5:4]; default: 0;
         *  flash ck drv
         */
        uint32_t reg_flash_ck_drv:2;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} iomux_mspi_pin_flash_ck_pin0_reg_t;


/** Group: flash_d_pin */
/** Type of flash_d_pin0 register
 *  IOMUX_MSPI_PIN_FLASH_D_PIN0_REG
 */
typedef union {
    struct {
        /** reg_flash_d_hys : R/W; bitpos: [0]; default: 0;
         *  flash d hys
         */
        uint32_t reg_flash_d_hys:1;
        /** reg_flash_d_ie : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_flash_d_ie:1;
        /** reg_flash_d_wpu : R/W; bitpos: [2]; default: 0;
         *  flash d wpu
         */
        uint32_t reg_flash_d_wpu:1;
        /** reg_flash_d_wpd : R/W; bitpos: [3]; default: 0;
         *  flash d wpd
         */
        uint32_t reg_flash_d_wpd:1;
        /** reg_flash_d_drv : R/W; bitpos: [5:4]; default: 0;
         *  flash d drv
         */
        uint32_t reg_flash_d_drv:2;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} iomux_mspi_pin_flash_d_pin0_reg_t;


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
         *  psram d hys
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
        /** reg_psram_d_drv : R/W; bitpos: [13:12]; default: 0;
         *  psram d drv
         */
        uint32_t reg_psram_d_drv:2;
        uint32_t reserved_14:18;
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
         *  psram q hys
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
        /** reg_psram_q_drv : R/W; bitpos: [13:12]; default: 0;
         *  psram q drv
         */
        uint32_t reg_psram_q_drv:2;
        uint32_t reserved_14:18;
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
         *  psram wp hys
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
        /** reg_psram_wp_drv : R/W; bitpos: [13:12]; default: 0;
         *  psram wp drv
         */
        uint32_t reg_psram_wp_drv:2;
        uint32_t reserved_14:18;
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
         *  psram hold hys
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
        /** reg_psram_hold_drv : R/W; bitpos: [13:12]; default: 0;
         *  psram hold drv
         */
        uint32_t reg_psram_hold_drv:2;
        uint32_t reserved_14:18;
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
         *  psram dq4 hys
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
        /** reg_psram_dq4_drv : R/W; bitpos: [13:12]; default: 0;
         *  psram dq4 drv
         */
        uint32_t reg_psram_dq4_drv:2;
        uint32_t reserved_14:18;
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
         *  psram dq5 hys
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
        /** reg_psram_dq5_drv : R/W; bitpos: [13:12]; default: 0;
         *  psram dq5 drv
         */
        uint32_t reg_psram_dq5_drv:2;
        uint32_t reserved_14:18;
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
         *  psram dq6 hys
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
        /** reg_psram_dq6_drv : R/W; bitpos: [13:12]; default: 0;
         *  psram dq6 drv
         */
        uint32_t reg_psram_dq6_drv:2;
        uint32_t reserved_14:18;
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
         *  psram dq7 hys
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
        /** reg_psram_dq7_drv : R/W; bitpos: [13:12]; default: 0;
         *  psram dq7 drv
         */
        uint32_t reg_psram_dq7_drv:2;
        uint32_t reserved_14:18;
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
         *  psram dqs0 hys
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
        /** reg_psram_dqs_0_drv : R/W; bitpos: [16:15]; default: 0;
         *  psram dqs0 drv
         */
        uint32_t reg_psram_dqs_0_drv:2;
        /** reg_psram_dqs_0_delay_270 : R/W; bitpos: [20:17]; default: 0;
         *  psram dqs0 delay 270
         */
        uint32_t reg_psram_dqs_0_delay_270:4;
        uint32_t reserved_21:11;
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
         *  psram ck hys
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
        /** reg_psram_ck_drv : R/W; bitpos: [13:12]; default: 0;
         *  psram ck drv
         */
        uint32_t reg_psram_ck_drv:2;
        uint32_t reserved_14:18;
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
         *  psram cs hys
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
        /** reg_psram_cs_drv : R/W; bitpos: [13:12]; default: 0;
         *  psram cs drv
         */
        uint32_t reg_psram_cs_drv:2;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} iomux_mspi_pin_psram_cs_pin0_reg_t;


/** Group: psram_dq8_pin */
/** Type of psram_dq8_pin0 register
 *  IOMUX_MSPI_PIN_PSRAM_D_PIN0_REG
 */
typedef union {
    struct {
        /** reg_psram_dq8_dli : R/W; bitpos: [3:0]; default: 0;
         *  psram dq8 dli
         */
        uint32_t reg_psram_dq8_dli:4;
        /** reg_psram_dq8_dlc : R/W; bitpos: [7:4]; default: 0;
         *  psram dq8 dlc
         */
        uint32_t reg_psram_dq8_dlc:4;
        /** reg_psram_dq8_hys : R/W; bitpos: [8]; default: 0;
         *  psram dq8 hys
         */
        uint32_t reg_psram_dq8_hys:1;
        /** reg_psram_dq8_ie : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_psram_dq8_ie:1;
        /** reg_psram_dq8_wpu : R/W; bitpos: [10]; default: 0;
         *  psram dq8 wpu
         */
        uint32_t reg_psram_dq8_wpu:1;
        /** reg_psram_dq8_wpd : R/W; bitpos: [11]; default: 0;
         *  psram dq8 wpd
         */
        uint32_t reg_psram_dq8_wpd:1;
        /** reg_psram_dq8_drv : R/W; bitpos: [13:12]; default: 0;
         *  psram dq8 drv
         */
        uint32_t reg_psram_dq8_drv:2;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} iomux_mspi_pin_psram_dq8_pin0_reg_t;


/** Group: psram_dq9_pin */
/** Type of psram_dq9_pin0 register
 *  IOMUX_MSPI_PIN_PSRAM_D_PIN0_REG
 */
typedef union {
    struct {
        /** reg_psram_dq9_dli : R/W; bitpos: [3:0]; default: 0;
         *  psram dq9 dli
         */
        uint32_t reg_psram_dq9_dli:4;
        /** reg_psram_dq9_dlc : R/W; bitpos: [7:4]; default: 0;
         *  psram dq9 dlc
         */
        uint32_t reg_psram_dq9_dlc:4;
        /** reg_psram_dq9_hys : R/W; bitpos: [8]; default: 0;
         *  psram dq9 hys
         */
        uint32_t reg_psram_dq9_hys:1;
        /** reg_psram_dq9_ie : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_psram_dq9_ie:1;
        /** reg_psram_dq9_wpu : R/W; bitpos: [10]; default: 0;
         *  psram dq9 wpu
         */
        uint32_t reg_psram_dq9_wpu:1;
        /** reg_psram_dq9_wpd : R/W; bitpos: [11]; default: 0;
         *  psram dq9 wpd
         */
        uint32_t reg_psram_dq9_wpd:1;
        /** reg_psram_dq9_drv : R/W; bitpos: [13:12]; default: 0;
         *  psram dq9 drv
         */
        uint32_t reg_psram_dq9_drv:2;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} iomux_mspi_pin_psram_dq9_pin0_reg_t;


/** Group: psram_dq10_pin */
/** Type of psram_dq10_pin0 register
 *  IOMUX_MSPI_PIN_PSRAM_D_PIN0_REG
 */
typedef union {
    struct {
        /** reg_psram_dq10_dli : R/W; bitpos: [3:0]; default: 0;
         *  psram dq10 dli
         */
        uint32_t reg_psram_dq10_dli:4;
        /** reg_psram_dq10_dlc : R/W; bitpos: [7:4]; default: 0;
         *  psram dq10 dlc
         */
        uint32_t reg_psram_dq10_dlc:4;
        /** reg_psram_dq10_hys : R/W; bitpos: [8]; default: 0;
         *  psram dq10 hys
         */
        uint32_t reg_psram_dq10_hys:1;
        /** reg_psram_dq10_ie : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_psram_dq10_ie:1;
        /** reg_psram_dq10_wpu : R/W; bitpos: [10]; default: 0;
         *  psram dq10 wpu
         */
        uint32_t reg_psram_dq10_wpu:1;
        /** reg_psram_dq10_wpd : R/W; bitpos: [11]; default: 0;
         *  psram dq10 wpd
         */
        uint32_t reg_psram_dq10_wpd:1;
        /** reg_psram_dq10_drv : R/W; bitpos: [13:12]; default: 0;
         *  psram dq10 drv
         */
        uint32_t reg_psram_dq10_drv:2;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} iomux_mspi_pin_psram_dq10_pin0_reg_t;


/** Group: psram_dq11_pin */
/** Type of psram_dq11_pin0 register
 *  IOMUX_MSPI_PIN_PSRAM_D_PIN0_REG
 */
typedef union {
    struct {
        /** reg_psram_dq11_dli : R/W; bitpos: [3:0]; default: 0;
         *  psram dq11 dli
         */
        uint32_t reg_psram_dq11_dli:4;
        /** reg_psram_dq11_dlc : R/W; bitpos: [7:4]; default: 0;
         *  psram dq11 dlc
         */
        uint32_t reg_psram_dq11_dlc:4;
        /** reg_psram_dq11_hys : R/W; bitpos: [8]; default: 0;
         *  psram dq11 hys
         */
        uint32_t reg_psram_dq11_hys:1;
        /** reg_psram_dq11_ie : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_psram_dq11_ie:1;
        /** reg_psram_dq11_wpu : R/W; bitpos: [10]; default: 0;
         *  psram dq11 wpu
         */
        uint32_t reg_psram_dq11_wpu:1;
        /** reg_psram_dq11_wpd : R/W; bitpos: [11]; default: 0;
         *  psram dq11 wpd
         */
        uint32_t reg_psram_dq11_wpd:1;
        /** reg_psram_dq11_drv : R/W; bitpos: [13:12]; default: 0;
         *  psram dq11 drv
         */
        uint32_t reg_psram_dq11_drv:2;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} iomux_mspi_pin_psram_dq11_pin0_reg_t;


/** Group: psram_dq12_pin */
/** Type of psram_dq12_pin0 register
 *  IOMUX_MSPI_PIN_PSRAM_D_PIN0_REG
 */
typedef union {
    struct {
        /** reg_psram_dq12_dli : R/W; bitpos: [3:0]; default: 0;
         *  psram dq12 dli
         */
        uint32_t reg_psram_dq12_dli:4;
        /** reg_psram_dq12_dlc : R/W; bitpos: [7:4]; default: 0;
         *  psram dq12 dlc
         */
        uint32_t reg_psram_dq12_dlc:4;
        /** reg_psram_dq12_hys : R/W; bitpos: [8]; default: 0;
         *  psram dq12 hys
         */
        uint32_t reg_psram_dq12_hys:1;
        /** reg_psram_dq12_ie : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_psram_dq12_ie:1;
        /** reg_psram_dq12_wpu : R/W; bitpos: [10]; default: 0;
         *  psram dq12 wpu
         */
        uint32_t reg_psram_dq12_wpu:1;
        /** reg_psram_dq12_wpd : R/W; bitpos: [11]; default: 0;
         *  psram dq12 wpd
         */
        uint32_t reg_psram_dq12_wpd:1;
        /** reg_psram_dq12_drv : R/W; bitpos: [13:12]; default: 0;
         *  psram dq12 drv
         */
        uint32_t reg_psram_dq12_drv:2;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} iomux_mspi_pin_psram_dq12_pin0_reg_t;


/** Group: psram_dq13_pin */
/** Type of psram_dq13_pin0 register
 *  IOMUX_MSPI_PIN_PSRAM_D_PIN0_REG
 */
typedef union {
    struct {
        /** reg_psram_dq13_dli : R/W; bitpos: [3:0]; default: 0;
         *  psram dq13 dli
         */
        uint32_t reg_psram_dq13_dli:4;
        /** reg_psram_dq13_dlc : R/W; bitpos: [7:4]; default: 0;
         *  psram dq13 dlc
         */
        uint32_t reg_psram_dq13_dlc:4;
        /** reg_psram_dq13_hys : R/W; bitpos: [8]; default: 0;
         *  psram dq13 hys
         */
        uint32_t reg_psram_dq13_hys:1;
        /** reg_psram_dq13_ie : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_psram_dq13_ie:1;
        /** reg_psram_dq13_wpu : R/W; bitpos: [10]; default: 0;
         *  psram dq13 wpu
         */
        uint32_t reg_psram_dq13_wpu:1;
        /** reg_psram_dq13_wpd : R/W; bitpos: [11]; default: 0;
         *  psram dq13 wpd
         */
        uint32_t reg_psram_dq13_wpd:1;
        /** reg_psram_dq13_drv : R/W; bitpos: [13:12]; default: 0;
         *  psram dq13 drv
         */
        uint32_t reg_psram_dq13_drv:2;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} iomux_mspi_pin_psram_dq13_pin0_reg_t;


/** Group: psram_dq14_pin */
/** Type of psram_dq14_pin0 register
 *  IOMUX_MSPI_PIN_PSRAM_D_PIN0_REG
 */
typedef union {
    struct {
        /** reg_psram_dq14_dli : R/W; bitpos: [3:0]; default: 0;
         *  psram dq14 dli
         */
        uint32_t reg_psram_dq14_dli:4;
        /** reg_psram_dq14_dlc : R/W; bitpos: [7:4]; default: 0;
         *  psram dq14 dlc
         */
        uint32_t reg_psram_dq14_dlc:4;
        /** reg_psram_dq14_hys : R/W; bitpos: [8]; default: 0;
         *  psram dq14 hys
         */
        uint32_t reg_psram_dq14_hys:1;
        /** reg_psram_dq14_ie : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_psram_dq14_ie:1;
        /** reg_psram_dq14_wpu : R/W; bitpos: [10]; default: 0;
         *  psram dq14 wpu
         */
        uint32_t reg_psram_dq14_wpu:1;
        /** reg_psram_dq14_wpd : R/W; bitpos: [11]; default: 0;
         *  psram dq14 wpd
         */
        uint32_t reg_psram_dq14_wpd:1;
        /** reg_psram_dq14_drv : R/W; bitpos: [13:12]; default: 0;
         *  psram dq14 drv
         */
        uint32_t reg_psram_dq14_drv:2;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} iomux_mspi_pin_psram_dq14_pin0_reg_t;


/** Group: psram_dq15_pin */
/** Type of psram_dq15_pin0 register
 *  IOMUX_MSPI_PIN_PSRAM_D_PIN0_REG
 */
typedef union {
    struct {
        /** reg_psram_dq15_dli : R/W; bitpos: [3:0]; default: 0;
         *  psram dq15 dli
         */
        uint32_t reg_psram_dq15_dli:4;
        /** reg_psram_dq15_dlc : R/W; bitpos: [7:4]; default: 0;
         *  psram dq15 dlc
         */
        uint32_t reg_psram_dq15_dlc:4;
        /** reg_psram_dq15_hys : R/W; bitpos: [8]; default: 0;
         *  psram dq15 hys
         */
        uint32_t reg_psram_dq15_hys:1;
        /** reg_psram_dq15_ie : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_psram_dq15_ie:1;
        /** reg_psram_dq15_wpu : R/W; bitpos: [10]; default: 0;
         *  psram dq15 wpu
         */
        uint32_t reg_psram_dq15_wpu:1;
        /** reg_psram_dq15_wpd : R/W; bitpos: [11]; default: 0;
         *  psram dq15 wpd
         */
        uint32_t reg_psram_dq15_wpd:1;
        /** reg_psram_dq15_drv : R/W; bitpos: [13:12]; default: 0;
         *  psram dq15 drv
         */
        uint32_t reg_psram_dq15_drv:2;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} iomux_mspi_pin_psram_dq15_pin0_reg_t;


/** Group: psram_dqs_1_pin */
/** Type of psram_dqs_1_pin0 register
 *  IOMUX_MSPI_PIN_PSRAM_D_PIN0_REG
 */
typedef union {
    struct {
        /** reg_psram_dqs_1_xpd : R/W; bitpos: [0]; default: 0;
         *  psram xpd dqs1
         */
        uint32_t reg_psram_dqs_1_xpd:1;
        /** reg_psram_dqs_1_phase : R/W; bitpos: [2:1]; default: 0;
         *  psram dqs1 phase
         */
        uint32_t reg_psram_dqs_1_phase:2;
        /** reg_psram_dqs_1_dli : R/W; bitpos: [6:3]; default: 0;
         *  psram dqs1 dli
         */
        uint32_t reg_psram_dqs_1_dli:4;
        /** reg_psram_dqs_1_delay_90 : R/W; bitpos: [10:7]; default: 0;
         *  psram dqs1 delay 90
         */
        uint32_t reg_psram_dqs_1_delay_90:4;
        /** reg_psram_dqs_1_hys : R/W; bitpos: [11]; default: 0;
         *  psram dqs1 hys
         */
        uint32_t reg_psram_dqs_1_hys:1;
        /** reg_psram_dqs_1_ie : R/W; bitpos: [12]; default: 0;
         *  Reserved
         */
        uint32_t reg_psram_dqs_1_ie:1;
        /** reg_psram_dqs_1_wpu : R/W; bitpos: [13]; default: 0;
         *  psram dqs1 wpu
         */
        uint32_t reg_psram_dqs_1_wpu:1;
        /** reg_psram_dqs_1_wpd : R/W; bitpos: [14]; default: 0;
         *  psram dqs1 wpd
         */
        uint32_t reg_psram_dqs_1_wpd:1;
        /** reg_psram_dqs_1_drv : R/W; bitpos: [16:15]; default: 0;
         *  psram dqs1 drv
         */
        uint32_t reg_psram_dqs_1_drv:2;
        /** reg_psram_dqs_1_delay_270 : R/W; bitpos: [20:17]; default: 0;
         *  psram dqs1 delay 270
         */
        uint32_t reg_psram_dqs_1_delay_270:4;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} iomux_mspi_pin_psram_dqs_1_pin0_reg_t;


typedef struct {
    volatile iomux_mspi_pin_clk_en0_reg_t clk_en0;
    volatile iomux_mspi_pin_flash_cs_pin0_reg_t flash_cs_pin0;
    volatile iomux_mspi_pin_flash_q_pin0_reg_t flash_q_pin0;
    volatile iomux_mspi_pin_flash_wp_pin0_reg_t flash_wp_pin0;
    volatile iomux_mspi_pin_flash_hold_pin0_reg_t flash_hold_pin0;
    volatile iomux_mspi_pin_flash_ck_pin0_reg_t flash_ck_pin0;
    volatile iomux_mspi_pin_flash_d_pin0_reg_t flash_d_pin0;
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
    volatile iomux_mspi_pin_psram_dq8_pin0_reg_t psram_dq8_pin0;
    volatile iomux_mspi_pin_psram_dq9_pin0_reg_t psram_dq9_pin0;
    volatile iomux_mspi_pin_psram_dq10_pin0_reg_t psram_dq10_pin0;
    volatile iomux_mspi_pin_psram_dq11_pin0_reg_t psram_dq11_pin0;
    volatile iomux_mspi_pin_psram_dq12_pin0_reg_t psram_dq12_pin0;
    volatile iomux_mspi_pin_psram_dq13_pin0_reg_t psram_dq13_pin0;
    volatile iomux_mspi_pin_psram_dq14_pin0_reg_t psram_dq14_pin0;
    volatile iomux_mspi_pin_psram_dq15_pin0_reg_t psram_dq15_pin0;
    volatile iomux_mspi_pin_psram_dqs_1_pin0_reg_t psram_dqs_1_pin0;
} iomux_mspi_pin_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(iomux_mspi_pin_dev_t) == 0x6c, "Invalid size of iomux_mspi_pin_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
