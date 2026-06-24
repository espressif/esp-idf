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
        /** reg_psram_pin_drv : R/W; bitpos: [14:12]; default: 4;
         *  psram pin drv
         */
        uint32_t reg_psram_pin_drv:3;
        /** reg_psram_pin_hold : R/W; bitpos: [15]; default: 0;
         *  psram pin hold
         */
        uint32_t reg_psram_pin_hold:1;
        uint32_t reserved_16:16;
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
        /** reg_psram_dqs_drv : R/W; bitpos: [17:15]; default: 4;
         *  psram dqs drv
         */
        uint32_t reg_psram_dqs_drv:3;
        /** reg_psram_dqs_delay_270 : R/W; bitpos: [21:18]; default: 0;
         *  psram dqs delay 270
         */
        uint32_t reg_psram_dqs_delay_270:4;
        /** reg_psram_dqs_hold : R/W; bitpos: [22]; default: 0;
         *  psram dqs hold
         */
        uint32_t reg_psram_dqs_hold:1;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} iomux_mspi_pin_psram_dqs_pin_reg_t;

/** psram_pin group */
typedef struct {
    volatile iomux_mspi_pin_psram_pin_reg_t pin_group0[8];   // for d, q, wp, hold, dq4, dq5, dq6, dq7
    volatile iomux_mspi_pin_psram_dqs_pin_reg_t dqs0;
    volatile iomux_mspi_pin_psram_pin_reg_t pin_clk;
    volatile iomux_mspi_pin_psram_pin_reg_t pin_cs;
} iomux_mspi_pin_psram_pin_grp_reg_t;

typedef struct {
    volatile iomux_mspi_pin_clk_en0_reg_t clk_en0;
    uint32_t reserved_004[6];
    volatile iomux_mspi_pin_psram_pin_grp_reg_t psram_pin_group;
} iomux_mspi_pin_dev_t;

extern iomux_mspi_pin_dev_t MSPI_IOMUX;


#ifndef __cplusplus
_Static_assert(sizeof(iomux_mspi_pin_dev_t) == 0x48, "Invalid size of iomux_mspi_pin_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
