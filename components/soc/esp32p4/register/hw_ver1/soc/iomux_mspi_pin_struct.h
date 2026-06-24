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


/** psram_pin */
typedef union {
    struct {
        /** reg_psram_pin_dli : R/W; bitpos: [3:0]; default: 0;
         *  psram pin dli
         */
        uint32_t reg_psram_pin_dli:4;
        /** reg_psram_pin_dlc : R/W; bitpos: [7:4]; default: 0;
         *  psram pin dlc
         */
        uint32_t reg_psram_pin_dlc:4;
        /** reg_psram_pin_hys : R/W; bitpos: [8]; default: 0;
         *  psram pin hys
         */
        uint32_t reg_psram_pin_hys:1;
        /** reg_psram_pin_ie : R/W; bitpos: [9]; default: 0;
         *  Reserved
         */
        uint32_t reg_psram_pin_ie:1;
        /** reg_psram_pin_wpu : R/W; bitpos: [10]; default: 0;
         *  psram pin wpu
         */
        uint32_t reg_psram_pin_wpu:1;
        /** reg_psram_pin_wpd : R/W; bitpos: [11]; default: 0;
         *  psram pin wpd
         */
        uint32_t reg_psram_pin_wpd:1;
        /** reg_psram_d_drv : R/W; bitpos: [13:12]; default: 0;
         *  psram pin drv
         */
        uint32_t reg_psram_d_drv:2;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} iomux_mspi_pin_psram_pin_reg_t;

/** psram_dqs_pin */
typedef union {
    struct {
        /** reg_psram_dqs_xpd : R/W; bitpos: [0]; default: 0;
         *  psram xpd dqs
         */
        uint32_t reg_psram_dqs_xpd:1;
        /** reg_psram_dqs_phase : R/W; bitpos: [2:1]; default: 0;
         *  psram dqs phase
         */
        uint32_t reg_psram_dqs_phase:2;
        /** reg_psram_dqs_dli : R/W; bitpos: [6:3]; default: 0;
         *  psram dqs dli
         */
        uint32_t reg_psram_dqs_dli:4;
        /** reg_psram_dqs_delay_90 : R/W; bitpos: [10:7]; default: 0;
         *  psram dqs delay 90
         */
        uint32_t reg_psram_dqs_delay_90:4;
        /** reg_psram_dqs_hys : R/W; bitpos: [11]; default: 0;
         *  psram dqs hys
         */
        uint32_t reg_psram_dqs_hys:1;
        /** reg_psram_dqs_ie : R/W; bitpos: [12]; default: 0;
         *  Reserved
         */
        uint32_t reg_psram_dqs_ie:1;
        /** reg_psram_dqs_wpu : R/W; bitpos: [13]; default: 0;
         *  psram dqs wpu
         */
        uint32_t reg_psram_dqs_wpu:1;
        /** reg_psram_dqs_wpd : R/W; bitpos: [14]; default: 0;
         *  psram dqs wpd
         */
        uint32_t reg_psram_dqs_wpd:1;
        /** reg_psram_dqs_drv : R/W; bitpos: [16:15]; default: 0;
         *  psram dqs drv
         */
        uint32_t reg_psram_dqs_drv:2;
        /** reg_psram_dqs_delay_270 : R/W; bitpos: [20:17]; default: 0;
         *  psram dqs delay 270
         */
        uint32_t reg_psram_dqs_delay_270:4;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} iomux_mspi_pin_psram_dqs_pin_reg_t;

/** psram_pin group */
typedef struct {
    volatile iomux_mspi_pin_psram_pin_reg_t pin_group0[8];   //for d, q, wp, hold, dq4, dq5, dq6, dq7
    volatile iomux_mspi_pin_psram_dqs_pin_reg_t dqs0;
    volatile iomux_mspi_pin_psram_pin_reg_t pin_group1[10];  //for ck, cs, dq8, dq9, dq10, dq11, dq12, dq13, dq14, dq15
    volatile iomux_mspi_pin_psram_dqs_pin_reg_t dqs1;
} iomux_mspi_pin_psram_pin_grp_reg_t;

/**
 * @brief Union type for Flash MSPI IOMUX pin registers
 * All Flash register types share the same size (uint32_t) and have a 'val' member for atomic access
 */
typedef union {
    iomux_mspi_pin_flash_cs_pin0_reg_t flash_cs;
    iomux_mspi_pin_flash_q_pin0_reg_t flash_q;
    iomux_mspi_pin_flash_wp_pin0_reg_t flash_wp;
    iomux_mspi_pin_flash_hold_pin0_reg_t flash_hold;
    iomux_mspi_pin_flash_ck_pin0_reg_t flash_ck;
    iomux_mspi_pin_flash_d_pin0_reg_t flash_d;
    uint32_t val;
} iomux_mspi_pin_reg_union_t;

typedef struct {
    volatile iomux_mspi_pin_clk_en0_reg_t clk_en0;
    union {
        struct {
            volatile iomux_mspi_pin_flash_cs_pin0_reg_t flash_cs_pin0;
            volatile iomux_mspi_pin_flash_q_pin0_reg_t flash_q_pin0;
            volatile iomux_mspi_pin_flash_wp_pin0_reg_t flash_wp_pin0;
            volatile iomux_mspi_pin_flash_hold_pin0_reg_t flash_hold_pin0;
            volatile iomux_mspi_pin_flash_ck_pin0_reg_t flash_ck_pin0;
            volatile iomux_mspi_pin_flash_d_pin0_reg_t flash_d_pin0;
        };
        /**
         * @brief Flash pin register array for direct access by pin ID
         * Array layout matches Flash pins in mspi_iomux_pin_t enumeration:
         *   [0]: Flash CS pin
         *   [1]: Flash Q pin
         *   [2]: Flash WP pin
         *   [3]: Flash HOLD pin
         *   [4]: Flash CK pin
         *   [5]: Flash D pin
         */
        struct {
            volatile iomux_mspi_pin_reg_union_t flash_pin_regs[6];
        };
    };
    volatile iomux_mspi_pin_psram_pin_grp_reg_t psram_pin_group;
} iomux_mspi_pin_dev_t;

extern iomux_mspi_pin_dev_t MSPI_IOMUX;

#ifndef __cplusplus
_Static_assert(sizeof(iomux_mspi_pin_dev_t) == 0x6c, "Invalid size of iomux_mspi_pin_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
