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

/** Group: Version Register */
/** Type of version register
 *  NA
 */
typedef union {
    struct {
        /** version : RO; bitpos: [31:0]; default: 825569322;
         *  NA
         */
        uint32_t version:32;
    };
    uint32_t val;
} csi_host_version_reg_t;


/** Group: Configuration Registers */
/** Type of n_lanes register
 *  NA
 */
typedef union {
    struct {
        /** n_lanes : R/W; bitpos: [2:0]; default: 1;
         *  NA
         */
        uint32_t n_lanes:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} csi_host_n_lanes_reg_t;

/** Type of csi2_resetn register
 *  NA
 */
typedef union {
    struct {
        /** csi2_resetn : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t csi2_resetn:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} csi_host_csi2_resetn_reg_t;

/** Type of phy_shutdownz register
 *  NA
 */
typedef union {
    struct {
        /** phy_shutdownz : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t phy_shutdownz:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} csi_host_phy_shutdownz_reg_t;

/** Type of dphy_rstz register
 *  NA
 */
typedef union {
    struct {
        /** dphy_rstz : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t dphy_rstz:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} csi_host_dphy_rstz_reg_t;

/** Type of phy_rx register
 *  NA
 */
typedef union {
    struct {
        /** phy_rxulpsesc_0 : RO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t phy_rxulpsesc_0:1;
        /** phy_rxulpsesc_1 : RO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t phy_rxulpsesc_1:1;
        uint32_t reserved_2:14;
        /** phy_rxulpsclknot : RO; bitpos: [16]; default: 1;
         *  NA
         */
        uint32_t phy_rxulpsclknot:1;
        /** phy_rxclkactivehs : RO; bitpos: [17]; default: 0;
         *  NA
         */
        uint32_t phy_rxclkactivehs:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} csi_host_phy_rx_reg_t;

/** Type of phy_test_ctrl0 register
 *  NA
 */
typedef union {
    struct {
        /** phy_testclr : R/W; bitpos: [0]; default: 1;
         *  NA
         */
        uint32_t phy_testclr:1;
        /** phy_testclk : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t phy_testclk:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} csi_host_phy_test_ctrl0_reg_t;

/** Type of phy_test_ctrl1 register
 *  NA
 */
typedef union {
    struct {
        /** phy_testdin : R/W; bitpos: [7:0]; default: 0;
         *  NA
         */
        uint32_t phy_testdin:8;
        /** phy_testdout : RO; bitpos: [15:8]; default: 0;
         *  NA
         */
        uint32_t phy_testdout:8;
        /** phy_testen : R/W; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t phy_testen:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} csi_host_phy_test_ctrl1_reg_t;

/** Type of vc_extension register
 *  NA
 */
typedef union {
    struct {
        /** vcx : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t vcx:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} csi_host_vc_extension_reg_t;

/** Type of phy_cal register
 *  NA
 */
typedef union {
    struct {
        /** rxskewcalhs : RC; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t rxskewcalhs:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} csi_host_phy_cal_reg_t;

/** Type of scrambling register
 *  NA
 */
typedef union {
    struct {
        /** scramble_enable : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t scramble_enable:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} csi_host_scrambling_reg_t;

/** Type of scrambling_seed1 register
 *  NA
 */
typedef union {
    struct {
        /** scramble_seed_lane1 : R/W; bitpos: [15:0]; default: 4104;
         *  NA
         */
        uint32_t scramble_seed_lane1:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} csi_host_scrambling_seed1_reg_t;

/** Type of scrambling_seed2 register
 *  NA
 */
typedef union {
    struct {
        /** scramble_seed_lane2 : R/W; bitpos: [15:0]; default: 4488;
         *  NA
         */
        uint32_t scramble_seed_lane2:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} csi_host_scrambling_seed2_reg_t;


/** Group: Interrupt Registers */
/** Type of int_st_main register
 *  NA
 */
typedef union {
    struct {
        /** st_status_int_phy_fatal : RC; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t st_status_int_phy_fatal:1;
        /** st_status_int_pkt_fatal : RC; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t st_status_int_pkt_fatal:1;
        /** st_status_int_bndry_frame_fatal : RC; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t st_status_int_bndry_frame_fatal:1;
        /** st_status_int_seq_frame_fatal : RC; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t st_status_int_seq_frame_fatal:1;
        /** st_status_int_crc_frame_fatal : RC; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t st_status_int_crc_frame_fatal:1;
        /** st_status_int_pld_crc_fatal : RC; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t st_status_int_pld_crc_fatal:1;
        /** st_status_int_data_id : RC; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t st_status_int_data_id:1;
        /** st_status_int_ecc_corrected : RC; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t st_status_int_ecc_corrected:1;
        uint32_t reserved_8:8;
        /** st_status_int_phy : RC; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t st_status_int_phy:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} csi_host_int_st_main_reg_t;

/** Type of int_st_phy_fatal register
 *  NA
 */
typedef union {
    struct {
        /** st_phy_errsotsynchs_0 : RC; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t st_phy_errsotsynchs_0:1;
        /** st_phy_errsotsynchs_1 : RC; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t st_phy_errsotsynchs_1:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} csi_host_int_st_phy_fatal_reg_t;

/** Type of int_msk_phy_fatal register
 *  NA
 */
typedef union {
    struct {
        /** mask_phy_errsotsynchs_0 : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t mask_phy_errsotsynchs_0:1;
        /** mask_phy_errsotsynchs_1 : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t mask_phy_errsotsynchs_1:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} csi_host_int_msk_phy_fatal_reg_t;

/** Type of int_force_phy_fatal register
 *  NA
 */
typedef union {
    struct {
        /** force_phy_errsotsynchs_0 : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t force_phy_errsotsynchs_0:1;
        /** force_phy_errsotsynchs_1 : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t force_phy_errsotsynchs_1:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} csi_host_int_force_phy_fatal_reg_t;

/** Type of int_st_pkt_fatal register
 *  NA
 */
typedef union {
    struct {
        /** st_err_ecc_double : RC; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t st_err_ecc_double:1;
        /** st_shorter_payload : RC; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t st_shorter_payload:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} csi_host_int_st_pkt_fatal_reg_t;

/** Type of int_msk_pkt_fatal register
 *  NA
 */
typedef union {
    struct {
        /** mask_err_ecc_double : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t mask_err_ecc_double:1;
        /** mask_shorter_payload : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t mask_shorter_payload:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} csi_host_int_msk_pkt_fatal_reg_t;

/** Type of int_force_pkt_fatal register
 *  NA
 */
typedef union {
    struct {
        /** force_err_ecc_double : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t force_err_ecc_double:1;
        /** force_shorter_payload : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t force_shorter_payload:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} csi_host_int_force_pkt_fatal_reg_t;

/** Type of int_st_phy register
 *  NA
 */
typedef union {
    struct {
        /** st_phy_errsoths_0 : RC; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t st_phy_errsoths_0:1;
        /** st_phy_errsoths_1 : RC; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t st_phy_errsoths_1:1;
        uint32_t reserved_2:14;
        /** st_phy_erresc_0 : RC; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t st_phy_erresc_0:1;
        /** st_phy_erresc_1 : RC; bitpos: [17]; default: 0;
         *  NA
         */
        uint32_t st_phy_erresc_1:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} csi_host_int_st_phy_reg_t;

/** Type of int_msk_phy register
 *  NA
 */
typedef union {
    struct {
        /** mask_phy_errsoths_0 : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t mask_phy_errsoths_0:1;
        /** mask_phy_errsoths_1 : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t mask_phy_errsoths_1:1;
        uint32_t reserved_2:14;
        /** mask_phy_erresc_0 : R/W; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t mask_phy_erresc_0:1;
        /** mask_phy_erresc_1 : R/W; bitpos: [17]; default: 0;
         *  NA
         */
        uint32_t mask_phy_erresc_1:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} csi_host_int_msk_phy_reg_t;

/** Type of int_force_phy register
 *  NA
 */
typedef union {
    struct {
        /** force_phy_errsoths_0 : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t force_phy_errsoths_0:1;
        /** force_phy_errsoths_1 : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t force_phy_errsoths_1:1;
        uint32_t reserved_2:14;
        /** force_phy_erresc_0 : R/W; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t force_phy_erresc_0:1;
        /** force_phy_erresc_1 : R/W; bitpos: [17]; default: 0;
         *  NA
         */
        uint32_t force_phy_erresc_1:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} csi_host_int_force_phy_reg_t;

/** Type of int_st_bndry_frame_fatal register
 *  NA
 */
typedef union {
    struct {
        /** st_err_f_bndry_match_vc0 : RC; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t st_err_f_bndry_match_vc0:1;
        /** st_err_f_bndry_match_vc1 : RC; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t st_err_f_bndry_match_vc1:1;
        /** st_err_f_bndry_match_vc2 : RC; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t st_err_f_bndry_match_vc2:1;
        /** st_err_f_bndry_match_vc3 : RC; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t st_err_f_bndry_match_vc3:1;
        /** st_err_f_bndry_match_vc4 : RC; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t st_err_f_bndry_match_vc4:1;
        /** st_err_f_bndry_match_vc5 : RC; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t st_err_f_bndry_match_vc5:1;
        /** st_err_f_bndry_match_vc6 : RC; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t st_err_f_bndry_match_vc6:1;
        /** st_err_f_bndry_match_vc7 : RC; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t st_err_f_bndry_match_vc7:1;
        /** st_err_f_bndry_match_vc8 : RC; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t st_err_f_bndry_match_vc8:1;
        /** st_err_f_bndry_match_vc9 : RC; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t st_err_f_bndry_match_vc9:1;
        /** st_err_f_bndry_match_vc10 : RC; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t st_err_f_bndry_match_vc10:1;
        /** st_err_f_bndry_match_vc11 : RC; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t st_err_f_bndry_match_vc11:1;
        /** st_err_f_bndry_match_vc12 : RC; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t st_err_f_bndry_match_vc12:1;
        /** st_err_f_bndry_match_vc13 : RC; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t st_err_f_bndry_match_vc13:1;
        /** st_err_f_bndry_match_vc14 : RC; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t st_err_f_bndry_match_vc14:1;
        /** st_err_f_bndry_match_vc15 : RC; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t st_err_f_bndry_match_vc15:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} csi_host_int_st_bndry_frame_fatal_reg_t;

/** Type of int_msk_bndry_frame_fatal register
 *  NA
 */
typedef union {
    struct {
        /** mask_err_f_bndry_match_vc0 : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_bndry_match_vc0:1;
        /** mask_err_f_bndry_match_vc1 : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_bndry_match_vc1:1;
        /** mask_err_f_bndry_match_vc2 : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_bndry_match_vc2:1;
        /** mask_err_f_bndry_match_vc3 : R/W; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_bndry_match_vc3:1;
        /** mask_err_f_bndry_match_vc4 : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_bndry_match_vc4:1;
        /** mask_err_f_bndry_match_vc5 : R/W; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_bndry_match_vc5:1;
        /** mask_err_f_bndry_match_vc6 : R/W; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_bndry_match_vc6:1;
        /** mask_err_f_bndry_match_vc7 : R/W; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_bndry_match_vc7:1;
        /** mask_err_f_bndry_match_vc8 : R/W; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_bndry_match_vc8:1;
        /** mask_err_f_bndry_match_vc9 : R/W; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_bndry_match_vc9:1;
        /** mask_err_f_bndry_match_vc10 : R/W; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_bndry_match_vc10:1;
        /** mask_err_f_bndry_match_vc11 : R/W; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_bndry_match_vc11:1;
        /** mask_err_f_bndry_match_vc12 : R/W; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_bndry_match_vc12:1;
        /** mask_err_f_bndry_match_vc13 : R/W; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_bndry_match_vc13:1;
        /** mask_err_f_bndry_match_vc14 : R/W; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_bndry_match_vc14:1;
        /** mask_err_f_bndry_match_vc15 : R/W; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_bndry_match_vc15:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} csi_host_int_msk_bndry_frame_fatal_reg_t;

/** Type of int_force_bndry_frame_fatal register
 *  NA
 */
typedef union {
    struct {
        /** force_err_f_bndry_match_vc0 : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t force_err_f_bndry_match_vc0:1;
        /** force_err_f_bndry_match_vc1 : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t force_err_f_bndry_match_vc1:1;
        /** force_err_f_bndry_match_vc2 : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t force_err_f_bndry_match_vc2:1;
        /** force_err_f_bndry_match_vc3 : R/W; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t force_err_f_bndry_match_vc3:1;
        /** force_err_f_bndry_match_vc4 : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t force_err_f_bndry_match_vc4:1;
        /** force_err_f_bndry_match_vc5 : R/W; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t force_err_f_bndry_match_vc5:1;
        /** force_err_f_bndry_match_vc6 : R/W; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t force_err_f_bndry_match_vc6:1;
        /** force_err_f_bndry_match_vc7 : R/W; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t force_err_f_bndry_match_vc7:1;
        /** force_err_f_bndry_match_vc8 : R/W; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t force_err_f_bndry_match_vc8:1;
        /** force_err_f_bndry_match_vc9 : R/W; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t force_err_f_bndry_match_vc9:1;
        /** force_err_f_bndry_match_vc10 : R/W; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t force_err_f_bndry_match_vc10:1;
        /** force_err_f_bndry_match_vc11 : R/W; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t force_err_f_bndry_match_vc11:1;
        /** force_err_f_bndry_match_vc12 : R/W; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t force_err_f_bndry_match_vc12:1;
        /** force_err_f_bndry_match_vc13 : R/W; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t force_err_f_bndry_match_vc13:1;
        /** force_err_f_bndry_match_vc14 : R/W; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t force_err_f_bndry_match_vc14:1;
        /** force_err_f_bndry_match_vc15 : R/W; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t force_err_f_bndry_match_vc15:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} csi_host_int_force_bndry_frame_fatal_reg_t;

/** Type of int_st_seq_frame_fatal register
 *  NA
 */
typedef union {
    struct {
        /** st_err_f_seq_vc0 : RC; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t st_err_f_seq_vc0:1;
        /** st_err_f_seq_vc1 : RC; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t st_err_f_seq_vc1:1;
        /** st_err_f_seq_vc2 : RC; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t st_err_f_seq_vc2:1;
        /** st_err_f_seq_vc3 : RC; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t st_err_f_seq_vc3:1;
        /** st_err_f_seq_vc4 : RC; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t st_err_f_seq_vc4:1;
        /** st_err_f_seq_vc5 : RC; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t st_err_f_seq_vc5:1;
        /** st_err_f_seq_vc6 : RC; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t st_err_f_seq_vc6:1;
        /** st_err_f_seq_vc7 : RC; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t st_err_f_seq_vc7:1;
        /** st_err_f_seq_vc8 : RC; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t st_err_f_seq_vc8:1;
        /** st_err_f_seq_vc9 : RC; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t st_err_f_seq_vc9:1;
        /** st_err_f_seq_vc10 : RC; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t st_err_f_seq_vc10:1;
        /** st_err_f_seq_vc11 : RC; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t st_err_f_seq_vc11:1;
        /** st_err_f_seq_vc12 : RC; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t st_err_f_seq_vc12:1;
        /** st_err_f_seq_vc13 : RC; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t st_err_f_seq_vc13:1;
        /** st_err_f_seq_vc14 : RC; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t st_err_f_seq_vc14:1;
        /** st_err_f_seq_vc15 : RC; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t st_err_f_seq_vc15:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} csi_host_int_st_seq_frame_fatal_reg_t;

/** Type of int_msk_seq_frame_fatal register
 *  NA
 */
typedef union {
    struct {
        /** mask_err_f_seq_vc0 : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_seq_vc0:1;
        /** mask_err_f_seq_vc1 : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_seq_vc1:1;
        /** mask_err_f_seq_vc2 : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_seq_vc2:1;
        /** mask_err_f_seq_vc3 : R/W; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_seq_vc3:1;
        /** mask_err_f_seq_vc4 : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_seq_vc4:1;
        /** mask_err_f_seq_vc5 : R/W; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_seq_vc5:1;
        /** mask_err_f_seq_vc6 : R/W; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_seq_vc6:1;
        /** mask_err_f_seq_vc7 : R/W; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_seq_vc7:1;
        /** mask_err_f_seq_vc8 : R/W; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_seq_vc8:1;
        /** mask_err_f_seq_vc9 : R/W; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_seq_vc9:1;
        /** mask_err_f_seq_vc10 : R/W; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_seq_vc10:1;
        /** mask_err_f_seq_vc11 : R/W; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_seq_vc11:1;
        /** mask_err_f_seq_vc12 : R/W; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_seq_vc12:1;
        /** mask_err_f_seq_vc13 : R/W; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_seq_vc13:1;
        /** mask_err_f_seq_vc14 : R/W; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_seq_vc14:1;
        /** mask_err_f_seq_vc15 : R/W; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t mask_err_f_seq_vc15:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} csi_host_int_msk_seq_frame_fatal_reg_t;

/** Type of int_force_seq_frame_fatal register
 *  NA
 */
typedef union {
    struct {
        /** force_err_f_seq_vc0 : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t force_err_f_seq_vc0:1;
        /** force_err_f_seq_vc1 : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t force_err_f_seq_vc1:1;
        /** force_err_f_seq_vc2 : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t force_err_f_seq_vc2:1;
        /** force_err_f_seq_vc3 : R/W; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t force_err_f_seq_vc3:1;
        /** force_err_f_seq_vc4 : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t force_err_f_seq_vc4:1;
        /** force_err_f_seq_vc5 : R/W; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t force_err_f_seq_vc5:1;
        /** force_err_f_seq_vc6 : R/W; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t force_err_f_seq_vc6:1;
        /** force_err_f_seq_vc7 : R/W; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t force_err_f_seq_vc7:1;
        /** force_err_f_seq_vc8 : R/W; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t force_err_f_seq_vc8:1;
        /** force_err_f_seq_vc9 : R/W; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t force_err_f_seq_vc9:1;
        /** force_err_f_seq_vc10 : R/W; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t force_err_f_seq_vc10:1;
        /** force_err_f_seq_vc11 : R/W; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t force_err_f_seq_vc11:1;
        /** force_err_f_seq_vc12 : R/W; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t force_err_f_seq_vc12:1;
        /** force_err_f_seq_vc13 : R/W; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t force_err_f_seq_vc13:1;
        /** force_err_f_seq_vc14 : R/W; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t force_err_f_seq_vc14:1;
        /** force_err_f_seq_vc15 : R/W; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t force_err_f_seq_vc15:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} csi_host_int_force_seq_frame_fatal_reg_t;

/** Type of int_st_crc_frame_fatal register
 *  NA
 */
typedef union {
    struct {
        /** st_err_frame_data_vc0 : RC; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t st_err_frame_data_vc0:1;
        /** st_err_frame_data_vc1 : RC; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t st_err_frame_data_vc1:1;
        /** st_err_frame_data_vc2 : RC; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t st_err_frame_data_vc2:1;
        /** st_err_frame_data_vc3 : RC; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t st_err_frame_data_vc3:1;
        /** st_err_frame_data_vc4 : RC; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t st_err_frame_data_vc4:1;
        /** st_err_frame_data_vc5 : RC; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t st_err_frame_data_vc5:1;
        /** st_err_frame_data_vc6 : RC; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t st_err_frame_data_vc6:1;
        /** st_err_frame_data_vc7 : RC; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t st_err_frame_data_vc7:1;
        /** st_err_frame_data_vc8 : RC; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t st_err_frame_data_vc8:1;
        /** st_err_frame_data_vc9 : RC; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t st_err_frame_data_vc9:1;
        /** st_err_frame_data_vc10 : RC; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t st_err_frame_data_vc10:1;
        /** st_err_frame_data_vc11 : RC; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t st_err_frame_data_vc11:1;
        /** st_err_frame_data_vc12 : RC; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t st_err_frame_data_vc12:1;
        /** st_err_frame_data_vc13 : RC; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t st_err_frame_data_vc13:1;
        /** st_err_frame_data_vc14 : RC; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t st_err_frame_data_vc14:1;
        /** st_err_frame_data_vc15 : RC; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t st_err_frame_data_vc15:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} csi_host_int_st_crc_frame_fatal_reg_t;

/** Type of int_msk_crc_frame_fatal register
 *  NA
 */
typedef union {
    struct {
        /** mask_err_frame_data_vc0 : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t mask_err_frame_data_vc0:1;
        /** mask_err_frame_data_vc1 : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t mask_err_frame_data_vc1:1;
        /** mask_err_frame_data_vc2 : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t mask_err_frame_data_vc2:1;
        /** mask_err_frame_data_vc3 : R/W; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t mask_err_frame_data_vc3:1;
        /** mask_err_frame_data_vc4 : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t mask_err_frame_data_vc4:1;
        /** mask_err_frame_data_vc5 : R/W; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t mask_err_frame_data_vc5:1;
        /** mask_err_frame_data_vc6 : R/W; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t mask_err_frame_data_vc6:1;
        /** mask_err_frame_data_vc7 : R/W; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t mask_err_frame_data_vc7:1;
        /** mask_err_frame_data_vc8 : R/W; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t mask_err_frame_data_vc8:1;
        /** mask_err_frame_data_vc9 : R/W; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t mask_err_frame_data_vc9:1;
        /** mask_err_frame_data_vc10 : R/W; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t mask_err_frame_data_vc10:1;
        /** mask_err_frame_data_vc11 : R/W; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t mask_err_frame_data_vc11:1;
        /** mask_err_frame_data_vc12 : R/W; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t mask_err_frame_data_vc12:1;
        /** mask_err_frame_data_vc13 : R/W; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t mask_err_frame_data_vc13:1;
        /** mask_err_frame_data_vc14 : R/W; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t mask_err_frame_data_vc14:1;
        /** mask_err_frame_data_vc15 : R/W; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t mask_err_frame_data_vc15:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} csi_host_int_msk_crc_frame_fatal_reg_t;

/** Type of int_force_crc_frame_fatal register
 *  NA
 */
typedef union {
    struct {
        /** force_err_frame_data_vc0 : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t force_err_frame_data_vc0:1;
        /** force_err_frame_data_vc1 : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t force_err_frame_data_vc1:1;
        /** force_err_frame_data_vc2 : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t force_err_frame_data_vc2:1;
        /** force_err_frame_data_vc3 : R/W; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t force_err_frame_data_vc3:1;
        /** force_err_frame_data_vc4 : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t force_err_frame_data_vc4:1;
        /** force_err_frame_data_vc5 : R/W; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t force_err_frame_data_vc5:1;
        /** force_err_frame_data_vc6 : R/W; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t force_err_frame_data_vc6:1;
        /** force_err_frame_data_vc7 : R/W; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t force_err_frame_data_vc7:1;
        /** force_err_frame_data_vc8 : R/W; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t force_err_frame_data_vc8:1;
        /** force_err_frame_data_vc9 : R/W; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t force_err_frame_data_vc9:1;
        /** force_err_frame_data_vc10 : R/W; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t force_err_frame_data_vc10:1;
        /** force_err_frame_data_vc11 : R/W; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t force_err_frame_data_vc11:1;
        /** force_err_frame_data_vc12 : R/W; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t force_err_frame_data_vc12:1;
        /** force_err_frame_data_vc13 : R/W; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t force_err_frame_data_vc13:1;
        /** force_err_frame_data_vc14 : R/W; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t force_err_frame_data_vc14:1;
        /** force_err_frame_data_vc15 : R/W; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t force_err_frame_data_vc15:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} csi_host_int_force_crc_frame_fatal_reg_t;

/** Type of int_st_pld_crc_fatal register
 *  NA
 */
typedef union {
    struct {
        /** st_err_crc_vc0 : RC; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t st_err_crc_vc0:1;
        /** st_err_crc_vc1 : RC; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t st_err_crc_vc1:1;
        /** st_err_crc_vc2 : RC; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t st_err_crc_vc2:1;
        /** st_err_crc_vc3 : RC; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t st_err_crc_vc3:1;
        /** st_err_crc_vc4 : RC; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t st_err_crc_vc4:1;
        /** st_err_crc_vc5 : RC; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t st_err_crc_vc5:1;
        /** st_err_crc_vc6 : RC; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t st_err_crc_vc6:1;
        /** st_err_crc_vc7 : RC; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t st_err_crc_vc7:1;
        /** st_err_crc_vc8 : RC; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t st_err_crc_vc8:1;
        /** st_err_crc_vc9 : RC; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t st_err_crc_vc9:1;
        /** st_err_crc_vc10 : RC; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t st_err_crc_vc10:1;
        /** st_err_crc_vc11 : RC; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t st_err_crc_vc11:1;
        /** st_err_crc_vc12 : RC; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t st_err_crc_vc12:1;
        /** st_err_crc_vc13 : RC; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t st_err_crc_vc13:1;
        /** st_err_crc_vc14 : RC; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t st_err_crc_vc14:1;
        /** st_err_crc_vc15 : RC; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t st_err_crc_vc15:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} csi_host_int_st_pld_crc_fatal_reg_t;

/** Type of int_msk_pld_crc_fatal register
 *  NA
 */
typedef union {
    struct {
        /** mask_err_crc_vc0 : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t mask_err_crc_vc0:1;
        /** mask_err_crc_vc1 : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t mask_err_crc_vc1:1;
        /** mask_err_crc_vc2 : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t mask_err_crc_vc2:1;
        /** mask_err_crc_vc3 : R/W; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t mask_err_crc_vc3:1;
        /** mask_err_crc_vc4 : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t mask_err_crc_vc4:1;
        /** mask_err_crc_vc5 : R/W; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t mask_err_crc_vc5:1;
        /** mask_err_crc_vc6 : R/W; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t mask_err_crc_vc6:1;
        /** mask_err_crc_vc7 : R/W; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t mask_err_crc_vc7:1;
        /** mask_err_crc_vc8 : R/W; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t mask_err_crc_vc8:1;
        /** mask_err_crc_vc9 : R/W; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t mask_err_crc_vc9:1;
        /** mask_err_crc_vc10 : R/W; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t mask_err_crc_vc10:1;
        /** mask_err_crc_vc11 : R/W; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t mask_err_crc_vc11:1;
        /** mask_err_crc_vc12 : R/W; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t mask_err_crc_vc12:1;
        /** mask_err_crc_vc13 : R/W; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t mask_err_crc_vc13:1;
        /** mask_err_crc_vc14 : R/W; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t mask_err_crc_vc14:1;
        /** mask_err_crc_vc15 : R/W; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t mask_err_crc_vc15:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} csi_host_int_msk_pld_crc_fatal_reg_t;

/** Type of int_force_pld_crc_fatal register
 *  NA
 */
typedef union {
    struct {
        /** force_err_crc_vc0 : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t force_err_crc_vc0:1;
        /** force_err_crc_vc1 : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t force_err_crc_vc1:1;
        /** force_err_crc_vc2 : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t force_err_crc_vc2:1;
        /** force_err_crc_vc3 : R/W; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t force_err_crc_vc3:1;
        /** force_err_crc_vc4 : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t force_err_crc_vc4:1;
        /** force_err_crc_vc5 : R/W; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t force_err_crc_vc5:1;
        /** force_err_crc_vc6 : R/W; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t force_err_crc_vc6:1;
        /** force_err_crc_vc7 : R/W; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t force_err_crc_vc7:1;
        /** force_err_crc_vc8 : R/W; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t force_err_crc_vc8:1;
        /** force_err_crc_vc9 : R/W; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t force_err_crc_vc9:1;
        /** force_err_crc_vc10 : R/W; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t force_err_crc_vc10:1;
        /** force_err_crc_vc11 : R/W; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t force_err_crc_vc11:1;
        /** force_err_crc_vc12 : R/W; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t force_err_crc_vc12:1;
        /** force_err_crc_vc13 : R/W; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t force_err_crc_vc13:1;
        /** force_err_crc_vc14 : R/W; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t force_err_crc_vc14:1;
        /** force_err_crc_vc15 : R/W; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t force_err_crc_vc15:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} csi_host_int_force_pld_crc_fatal_reg_t;

/** Type of int_st_data_id register
 *  NA
 */
typedef union {
    struct {
        /** st_err_id_vc0 : RC; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t st_err_id_vc0:1;
        /** st_err_id_vc1 : RC; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t st_err_id_vc1:1;
        /** st_err_id_vc2 : RC; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t st_err_id_vc2:1;
        /** st_err_id_vc3 : RC; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t st_err_id_vc3:1;
        /** st_err_id_vc4 : RC; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t st_err_id_vc4:1;
        /** st_err_id_vc5 : RC; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t st_err_id_vc5:1;
        /** st_err_id_vc6 : RC; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t st_err_id_vc6:1;
        /** st_err_id_vc7 : RC; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t st_err_id_vc7:1;
        /** st_err_id_vc8 : RC; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t st_err_id_vc8:1;
        /** st_err_id_vc9 : RC; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t st_err_id_vc9:1;
        /** st_err_id_vc10 : RC; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t st_err_id_vc10:1;
        /** st_err_id_vc11 : RC; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t st_err_id_vc11:1;
        /** st_err_id_vc12 : RC; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t st_err_id_vc12:1;
        /** st_err_id_vc13 : RC; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t st_err_id_vc13:1;
        /** st_err_id_vc14 : RC; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t st_err_id_vc14:1;
        /** st_err_id_vc15 : RC; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t st_err_id_vc15:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} csi_host_int_st_data_id_reg_t;

/** Type of int_msk_data_id register
 *  NA
 */
typedef union {
    struct {
        /** mask_err_id_vc0 : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t mask_err_id_vc0:1;
        /** mask_err_id_vc1 : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t mask_err_id_vc1:1;
        /** mask_err_id_vc2 : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t mask_err_id_vc2:1;
        /** mask_err_id_vc3 : R/W; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t mask_err_id_vc3:1;
        /** mask_err_id_vc4 : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t mask_err_id_vc4:1;
        /** mask_err_id_vc5 : R/W; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t mask_err_id_vc5:1;
        /** mask_err_id_vc6 : R/W; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t mask_err_id_vc6:1;
        /** mask_err_id_vc7 : R/W; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t mask_err_id_vc7:1;
        /** mask_err_id_vc8 : R/W; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t mask_err_id_vc8:1;
        /** mask_err_id_vc9 : R/W; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t mask_err_id_vc9:1;
        /** mask_err_id_vc10 : R/W; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t mask_err_id_vc10:1;
        /** mask_err_id_vc11 : R/W; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t mask_err_id_vc11:1;
        /** mask_err_id_vc12 : R/W; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t mask_err_id_vc12:1;
        /** mask_err_id_vc13 : R/W; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t mask_err_id_vc13:1;
        /** mask_err_id_vc14 : R/W; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t mask_err_id_vc14:1;
        /** mask_err_id_vc15 : R/W; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t mask_err_id_vc15:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} csi_host_int_msk_data_id_reg_t;

/** Type of int_force_data_id register
 *  NA
 */
typedef union {
    struct {
        /** force_err_id_vc0 : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t force_err_id_vc0:1;
        /** force_err_id_vc1 : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t force_err_id_vc1:1;
        /** force_err_id_vc2 : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t force_err_id_vc2:1;
        /** force_err_id_vc3 : R/W; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t force_err_id_vc3:1;
        /** force_err_id_vc4 : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t force_err_id_vc4:1;
        /** force_err_id_vc5 : R/W; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t force_err_id_vc5:1;
        /** force_err_id_vc6 : R/W; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t force_err_id_vc6:1;
        /** force_err_id_vc7 : R/W; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t force_err_id_vc7:1;
        /** force_err_id_vc8 : R/W; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t force_err_id_vc8:1;
        /** force_err_id_vc9 : R/W; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t force_err_id_vc9:1;
        /** force_err_id_vc10 : R/W; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t force_err_id_vc10:1;
        /** force_err_id_vc11 : R/W; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t force_err_id_vc11:1;
        /** force_err_id_vc12 : R/W; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t force_err_id_vc12:1;
        /** force_err_id_vc13 : R/W; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t force_err_id_vc13:1;
        /** force_err_id_vc14 : R/W; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t force_err_id_vc14:1;
        /** force_err_id_vc15 : R/W; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t force_err_id_vc15:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} csi_host_int_force_data_id_reg_t;

/** Type of int_st_ecc_corrected register
 *  NA
 */
typedef union {
    struct {
        /** st_err_ecc_corrected_vc0 : RC; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t st_err_ecc_corrected_vc0:1;
        /** st_err_ecc_corrected_vc1 : RC; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t st_err_ecc_corrected_vc1:1;
        /** st_err_ecc_corrected_vc2 : RC; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t st_err_ecc_corrected_vc2:1;
        /** st_err_ecc_corrected_vc3 : RC; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t st_err_ecc_corrected_vc3:1;
        /** st_err_ecc_corrected_vc4 : RC; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t st_err_ecc_corrected_vc4:1;
        /** st_err_ecc_corrected_vc5 : RC; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t st_err_ecc_corrected_vc5:1;
        /** st_err_ecc_corrected_vc6 : RC; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t st_err_ecc_corrected_vc6:1;
        /** st_err_ecc_corrected_vc7 : RC; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t st_err_ecc_corrected_vc7:1;
        /** st_err_ecc_corrected_vc8 : RC; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t st_err_ecc_corrected_vc8:1;
        /** st_err_ecc_corrected_vc9 : RC; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t st_err_ecc_corrected_vc9:1;
        /** st_err_ecc_corrected_vc10 : RC; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t st_err_ecc_corrected_vc10:1;
        /** st_err_ecc_corrected_vc11 : RC; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t st_err_ecc_corrected_vc11:1;
        /** st_err_ecc_corrected_vc12 : RC; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t st_err_ecc_corrected_vc12:1;
        /** st_err_ecc_corrected_vc13 : RC; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t st_err_ecc_corrected_vc13:1;
        /** st_err_ecc_corrected_vc14 : RC; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t st_err_ecc_corrected_vc14:1;
        /** st_err_ecc_corrected_vc15 : RC; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t st_err_ecc_corrected_vc15:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} csi_host_int_st_ecc_corrected_reg_t;

/** Type of int_msk_ecc_corrected register
 *  NA
 */
typedef union {
    struct {
        /** mask_err_ecc_corrected_vc0 : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t mask_err_ecc_corrected_vc0:1;
        /** mask_err_ecc_corrected_vc1 : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t mask_err_ecc_corrected_vc1:1;
        /** mask_err_ecc_corrected_vc2 : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t mask_err_ecc_corrected_vc2:1;
        /** mask_err_ecc_corrected_vc3 : R/W; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t mask_err_ecc_corrected_vc3:1;
        /** mask_err_ecc_corrected_vc4 : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t mask_err_ecc_corrected_vc4:1;
        /** mask_err_ecc_corrected_vc5 : R/W; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t mask_err_ecc_corrected_vc5:1;
        /** mask_err_ecc_corrected_vc6 : R/W; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t mask_err_ecc_corrected_vc6:1;
        /** mask_err_ecc_corrected_vc7 : R/W; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t mask_err_ecc_corrected_vc7:1;
        /** mask_err_ecc_corrected_vc8 : R/W; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t mask_err_ecc_corrected_vc8:1;
        /** mask_err_ecc_corrected_vc9 : R/W; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t mask_err_ecc_corrected_vc9:1;
        /** mask_err_ecc_corrected_vc10 : R/W; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t mask_err_ecc_corrected_vc10:1;
        /** mask_err_ecc_corrected_vc11 : R/W; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t mask_err_ecc_corrected_vc11:1;
        /** mask_err_ecc_corrected_vc12 : R/W; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t mask_err_ecc_corrected_vc12:1;
        /** mask_err_ecc_corrected_vc13 : R/W; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t mask_err_ecc_corrected_vc13:1;
        /** mask_err_ecc_corrected_vc14 : R/W; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t mask_err_ecc_corrected_vc14:1;
        /** mask_err_ecc_corrected_vc15 : R/W; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t mask_err_ecc_corrected_vc15:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} csi_host_int_msk_ecc_corrected_reg_t;

/** Type of int_force_ecc_corrected register
 *  NA
 */
typedef union {
    struct {
        /** force_err_ecc_corrected_vc0 : R/W; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t force_err_ecc_corrected_vc0:1;
        /** force_err_ecc_corrected_vc1 : R/W; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t force_err_ecc_corrected_vc1:1;
        /** force_err_ecc_corrected_vc2 : R/W; bitpos: [2]; default: 0;
         *  NA
         */
        uint32_t force_err_ecc_corrected_vc2:1;
        /** force_err_ecc_corrected_vc3 : R/W; bitpos: [3]; default: 0;
         *  NA
         */
        uint32_t force_err_ecc_corrected_vc3:1;
        /** force_err_ecc_corrected_vc4 : R/W; bitpos: [4]; default: 0;
         *  NA
         */
        uint32_t force_err_ecc_corrected_vc4:1;
        /** force_err_ecc_corrected_vc5 : R/W; bitpos: [5]; default: 0;
         *  NA
         */
        uint32_t force_err_ecc_corrected_vc5:1;
        /** force_err_ecc_corrected_vc6 : R/W; bitpos: [6]; default: 0;
         *  NA
         */
        uint32_t force_err_ecc_corrected_vc6:1;
        /** force_err_ecc_corrected_vc7 : R/W; bitpos: [7]; default: 0;
         *  NA
         */
        uint32_t force_err_ecc_corrected_vc7:1;
        /** force_err_ecc_corrected_vc8 : R/W; bitpos: [8]; default: 0;
         *  NA
         */
        uint32_t force_err_ecc_corrected_vc8:1;
        /** force_err_ecc_corrected_vc9 : R/W; bitpos: [9]; default: 0;
         *  NA
         */
        uint32_t force_err_ecc_corrected_vc9:1;
        /** force_err_ecc_corrected_vc10 : R/W; bitpos: [10]; default: 0;
         *  NA
         */
        uint32_t force_err_ecc_corrected_vc10:1;
        /** force_err_ecc_corrected_vc11 : R/W; bitpos: [11]; default: 0;
         *  NA
         */
        uint32_t force_err_ecc_corrected_vc11:1;
        /** force_err_ecc_corrected_vc12 : R/W; bitpos: [12]; default: 0;
         *  NA
         */
        uint32_t force_err_ecc_corrected_vc12:1;
        /** force_err_ecc_corrected_vc13 : R/W; bitpos: [13]; default: 0;
         *  NA
         */
        uint32_t force_err_ecc_corrected_vc13:1;
        /** force_err_ecc_corrected_vc14 : R/W; bitpos: [14]; default: 0;
         *  NA
         */
        uint32_t force_err_ecc_corrected_vc14:1;
        /** force_err_ecc_corrected_vc15 : R/W; bitpos: [15]; default: 0;
         *  NA
         */
        uint32_t force_err_ecc_corrected_vc15:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} csi_host_int_force_ecc_corrected_reg_t;


/** Group: Status Registers */
/** Type of phy_stopstate register
 *  NA
 */
typedef union {
    struct {
        /** phy_stopstatedata_0 : RO; bitpos: [0]; default: 0;
         *  NA
         */
        uint32_t phy_stopstatedata_0:1;
        /** phy_stopstatedata_1 : RO; bitpos: [1]; default: 0;
         *  NA
         */
        uint32_t phy_stopstatedata_1:1;
        uint32_t reserved_2:14;
        /** phy_stopstateclk : RO; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t phy_stopstateclk:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} csi_host_phy_stopstate_reg_t;


typedef struct csi_host_dev_t {
    volatile csi_host_version_reg_t version;
    volatile csi_host_n_lanes_reg_t n_lanes;
    volatile csi_host_csi2_resetn_reg_t csi2_resetn;
    volatile csi_host_int_st_main_reg_t int_st_main;
    uint32_t reserved_010[12];
    volatile csi_host_phy_shutdownz_reg_t phy_shutdownz;
    volatile csi_host_dphy_rstz_reg_t dphy_rstz;
    volatile csi_host_phy_rx_reg_t phy_rx;
    volatile csi_host_phy_stopstate_reg_t phy_stopstate;
    volatile csi_host_phy_test_ctrl0_reg_t phy_test_ctrl0;
    volatile csi_host_phy_test_ctrl1_reg_t phy_test_ctrl1;
    uint32_t reserved_058[28];
    volatile csi_host_vc_extension_reg_t vc_extension;
    volatile csi_host_phy_cal_reg_t phy_cal;
    uint32_t reserved_0d0[4];
    volatile csi_host_int_st_phy_fatal_reg_t int_st_phy_fatal;
    volatile csi_host_int_msk_phy_fatal_reg_t int_msk_phy_fatal;
    volatile csi_host_int_force_phy_fatal_reg_t int_force_phy_fatal;
    uint32_t reserved_0ec;
    volatile csi_host_int_st_pkt_fatal_reg_t int_st_pkt_fatal;
    volatile csi_host_int_msk_pkt_fatal_reg_t int_msk_pkt_fatal;
    volatile csi_host_int_force_pkt_fatal_reg_t int_force_pkt_fatal;
    uint32_t reserved_0fc[5];
    volatile csi_host_int_st_phy_reg_t int_st_phy;
    volatile csi_host_int_msk_phy_reg_t int_msk_phy;
    volatile csi_host_int_force_phy_reg_t int_force_phy;
    uint32_t reserved_11c[89];
    volatile csi_host_int_st_bndry_frame_fatal_reg_t int_st_bndry_frame_fatal;
    volatile csi_host_int_msk_bndry_frame_fatal_reg_t int_msk_bndry_frame_fatal;
    volatile csi_host_int_force_bndry_frame_fatal_reg_t int_force_bndry_frame_fatal;
    uint32_t reserved_28c;
    volatile csi_host_int_st_seq_frame_fatal_reg_t int_st_seq_frame_fatal;
    volatile csi_host_int_msk_seq_frame_fatal_reg_t int_msk_seq_frame_fatal;
    volatile csi_host_int_force_seq_frame_fatal_reg_t int_force_seq_frame_fatal;
    uint32_t reserved_29c;
    volatile csi_host_int_st_crc_frame_fatal_reg_t int_st_crc_frame_fatal;
    volatile csi_host_int_msk_crc_frame_fatal_reg_t int_msk_crc_frame_fatal;
    volatile csi_host_int_force_crc_frame_fatal_reg_t int_force_crc_frame_fatal;
    uint32_t reserved_2ac;
    volatile csi_host_int_st_pld_crc_fatal_reg_t int_st_pld_crc_fatal;
    volatile csi_host_int_msk_pld_crc_fatal_reg_t int_msk_pld_crc_fatal;
    volatile csi_host_int_force_pld_crc_fatal_reg_t int_force_pld_crc_fatal;
    uint32_t reserved_2bc;
    volatile csi_host_int_st_data_id_reg_t int_st_data_id;
    volatile csi_host_int_msk_data_id_reg_t int_msk_data_id;
    volatile csi_host_int_force_data_id_reg_t int_force_data_id;
    uint32_t reserved_2cc;
    volatile csi_host_int_st_ecc_corrected_reg_t int_st_ecc_corrected;
    volatile csi_host_int_msk_ecc_corrected_reg_t int_msk_ecc_corrected;
    volatile csi_host_int_force_ecc_corrected_reg_t int_force_ecc_corrected;
    uint32_t reserved_2dc[9];
    volatile csi_host_scrambling_reg_t scrambling;
    volatile csi_host_scrambling_seed1_reg_t scrambling_seed1;
    volatile csi_host_scrambling_seed2_reg_t scrambling_seed2;
} csi_host_dev_t;

extern csi_host_dev_t MIPI_CSI_HOST;

#ifndef __cplusplus
_Static_assert(sizeof(csi_host_dev_t) == 0x30c, "Invalid size of csi_host_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
