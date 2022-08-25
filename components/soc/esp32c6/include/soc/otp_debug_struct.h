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

/** Group: OTP_DEBUG Block0 Write Disable Data */
/** Type of wr_dis register
 *  Otp debuger block0 data register1.
 */
typedef union {
    struct {
        /** block0_wr_dis : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 write disable data.
         */
        uint32_t block0_wr_dis:32;
    };
    uint32_t val;
} otp_debug_wr_dis_reg_t;


/** Group: OTP_DEBUG Block0 Backup1 Word1 Data */
/** Type of blk0_backup1_w1 register
 *  Otp debuger block0 data register2.
 */
typedef union {
    struct {
        /** block0_backup1_w1 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup1 word1 data.
         */
        uint32_t block0_backup1_w1:32;
    };
    uint32_t val;
} otp_debug_blk0_backup1_w1_reg_t;


/** Group: OTP_DEBUG Block0 Backup1 Word2 Data */
/** Type of blk0_backup1_w2 register
 *  Otp debuger block0 data register3.
 */
typedef union {
    struct {
        /** block0_backup1_w2 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup1 word2 data.
         */
        uint32_t block0_backup1_w2:32;
    };
    uint32_t val;
} otp_debug_blk0_backup1_w2_reg_t;


/** Group: OTP_DEBUG Block0 Backup1 Word3 Data */
/** Type of blk0_backup1_w3 register
 *  Otp debuger block0 data register4.
 */
typedef union {
    struct {
        /** block0_backup1_w3 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup1 word3 data.
         */
        uint32_t block0_backup1_w3:32;
    };
    uint32_t val;
} otp_debug_blk0_backup1_w3_reg_t;


/** Group: OTP_DEBUG Block0 Backup1 Word4 Data */
/** Type of blk0_backup1_w4 register
 *  Otp debuger block0 data register5.
 */
typedef union {
    struct {
        /** block0_backup1_w4 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup1 word4 data.
         */
        uint32_t block0_backup1_w4:32;
    };
    uint32_t val;
} otp_debug_blk0_backup1_w4_reg_t;


/** Group: OTP_DEBUG Block0 Backup1 Word5 Data */
/** Type of blk0_backup1_w5 register
 *  Otp debuger block0 data register6.
 */
typedef union {
    struct {
        /** block0_backup1_w5 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup1 word5 data.
         */
        uint32_t block0_backup1_w5:32;
    };
    uint32_t val;
} otp_debug_blk0_backup1_w5_reg_t;


/** Group: OTP_DEBUG Block0 Backup2 Word1 Data */
/** Type of blk0_backup2_w1 register
 *  Otp debuger block0 data register7.
 */
typedef union {
    struct {
        /** block0_backup2_w1 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup2 word1 data.
         */
        uint32_t block0_backup2_w1:32;
    };
    uint32_t val;
} otp_debug_blk0_backup2_w1_reg_t;


/** Group: OTP_DEBUG Block0 Backup2 Word2 Data */
/** Type of blk0_backup2_w2 register
 *  Otp debuger block0 data register8.
 */
typedef union {
    struct {
        /** block0_backup2_w2 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup2 word2 data.
         */
        uint32_t block0_backup2_w2:32;
    };
    uint32_t val;
} otp_debug_blk0_backup2_w2_reg_t;


/** Group: OTP_DEBUG Block0 Backup2 Word3 Data */
/** Type of blk0_backup2_w3 register
 *  Otp debuger block0 data register9.
 */
typedef union {
    struct {
        /** block0_backup2_w3 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup2 word3 data.
         */
        uint32_t block0_backup2_w3:32;
    };
    uint32_t val;
} otp_debug_blk0_backup2_w3_reg_t;


/** Group: OTP_DEBUG Block0 Backup2 Word4 Data */
/** Type of blk0_backup2_w4 register
 *  Otp debuger block0 data register10.
 */
typedef union {
    struct {
        /** block0_backup2_w4 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup2 word4 data.
         */
        uint32_t block0_backup2_w4:32;
    };
    uint32_t val;
} otp_debug_blk0_backup2_w4_reg_t;


/** Group: OTP_DEBUG Block0 Backup2 Word5 Data */
/** Type of blk0_backup2_w5 register
 *  Otp debuger block0 data register11.
 */
typedef union {
    struct {
        /** block0_backup2_w5 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup2 word5 data.
         */
        uint32_t block0_backup2_w5:32;
    };
    uint32_t val;
} otp_debug_blk0_backup2_w5_reg_t;


/** Group: OTP_DEBUG Block0 Backup3 Word1 Data */
/** Type of blk0_backup3_w1 register
 *  Otp debuger block0 data register12.
 */
typedef union {
    struct {
        /** block0_backup3_w1 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup3 word1 data.
         */
        uint32_t block0_backup3_w1:32;
    };
    uint32_t val;
} otp_debug_blk0_backup3_w1_reg_t;


/** Group: OTP_DEBUG Block0 Backup3 Word2 Data */
/** Type of blk0_backup3_w2 register
 *  Otp debuger block0 data register13.
 */
typedef union {
    struct {
        /** block0_backup3_w2 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup3 word2 data.
         */
        uint32_t block0_backup3_w2:32;
    };
    uint32_t val;
} otp_debug_blk0_backup3_w2_reg_t;


/** Group: OTP_DEBUG Block0 Backup3 Word3 Data */
/** Type of blk0_backup3_w3 register
 *  Otp debuger block0 data register14.
 */
typedef union {
    struct {
        /** block0_backup3_w3 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup3 word3 data.
         */
        uint32_t block0_backup3_w3:32;
    };
    uint32_t val;
} otp_debug_blk0_backup3_w3_reg_t;


/** Group: OTP_DEBUG Block0 Backup3 Word4 Data */
/** Type of blk0_backup3_w4 register
 *  Otp debuger block0 data register15.
 */
typedef union {
    struct {
        /** block0_backup3_w4 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup3 word4 data.
         */
        uint32_t block0_backup3_w4:32;
    };
    uint32_t val;
} otp_debug_blk0_backup3_w4_reg_t;


/** Group: OTP_DEBUG Block0 Backup3 Word5 Data */
/** Type of blk0_backup3_w5 register
 *  Otp debuger block0 data register16.
 */
typedef union {
    struct {
        /** block0_backup3_w5 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup3 word5 data.
         */
        uint32_t block0_backup3_w5:32;
    };
    uint32_t val;
} otp_debug_blk0_backup3_w5_reg_t;


/** Group: OTP_DEBUG Block0 Backup4 Word1 Data */
/** Type of blk0_backup4_w1 register
 *  Otp debuger block0 data register17.
 */
typedef union {
    struct {
        /** block0_backup4_w1 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup4 word1 data.
         */
        uint32_t block0_backup4_w1:32;
    };
    uint32_t val;
} otp_debug_blk0_backup4_w1_reg_t;


/** Group: OTP_DEBUG Block0 Backup4 Word2 Data */
/** Type of blk0_backup4_w2 register
 *  Otp debuger block0 data register18.
 */
typedef union {
    struct {
        /** block0_backup4_w2 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup4 word2 data.
         */
        uint32_t block0_backup4_w2:32;
    };
    uint32_t val;
} otp_debug_blk0_backup4_w2_reg_t;


/** Group: OTP_DEBUG Block0 Backup4 Word3 Data */
/** Type of blk0_backup4_w3 register
 *  Otp debuger block0 data register19.
 */
typedef union {
    struct {
        /** block0_backup4_w3 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup4 word3 data.
         */
        uint32_t block0_backup4_w3:32;
    };
    uint32_t val;
} otp_debug_blk0_backup4_w3_reg_t;


/** Group: OTP_DEBUG Block0 Backup4 Word4 Data */
/** Type of blk0_backup4_w4 register
 *  Otp debuger block0 data register20.
 */
typedef union {
    struct {
        /** block0_backup4_w4 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup4 word4 data.
         */
        uint32_t block0_backup4_w4:32;
    };
    uint32_t val;
} otp_debug_blk0_backup4_w4_reg_t;


/** Group: OTP_DEBUG Block0 Backup4 Word5 Data */
/** Type of blk0_backup4_w5 register
 *  Otp debuger block0 data register21.
 */
typedef union {
    struct {
        /** block0_backup4_w5 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup4 word5 data.
         */
        uint32_t block0_backup4_w5:32;
    };
    uint32_t val;
} otp_debug_blk0_backup4_w5_reg_t;


/** Group: OTP_DEBUG Block1 Word1 Data */
/** Type of blk1_w1 register
 *  Otp debuger block1 data register1.
 */
typedef union {
    struct {
        /** block1_w1 : RO; bitpos: [31:0]; default: 0;
         *  Otp block1  word1 data.
         */
        uint32_t block1_w1:32;
    };
    uint32_t val;
} otp_debug_blk1_w1_reg_t;


/** Group: OTP_DEBUG Block1 Word2 Data */
/** Type of blk1_w2 register
 *  Otp debuger block1 data register2.
 */
typedef union {
    struct {
        /** block1_w2 : RO; bitpos: [31:0]; default: 0;
         *  Otp block1  word2 data.
         */
        uint32_t block1_w2:32;
    };
    uint32_t val;
} otp_debug_blk1_w2_reg_t;


/** Group: OTP_DEBUG Block1 Word3 Data */
/** Type of blk1_w3 register
 *  Otp debuger block1 data register3.
 */
typedef union {
    struct {
        /** block1_w3 : RO; bitpos: [31:0]; default: 0;
         *  Otp block1  word3 data.
         */
        uint32_t block1_w3:32;
    };
    uint32_t val;
} otp_debug_blk1_w3_reg_t;


/** Group: OTP_DEBUG Block1 Word4 Data */
/** Type of blk1_w4 register
 *  Otp debuger block1 data register4.
 */
typedef union {
    struct {
        /** block1_w4 : RO; bitpos: [31:0]; default: 0;
         *  Otp block1  word4 data.
         */
        uint32_t block1_w4:32;
    };
    uint32_t val;
} otp_debug_blk1_w4_reg_t;


/** Group: OTP_DEBUG Block1 Word5 Data */
/** Type of blk1_w5 register
 *  Otp debuger block1 data register5.
 */
typedef union {
    struct {
        /** block1_w5 : RO; bitpos: [31:0]; default: 0;
         *  Otp block1  word5 data.
         */
        uint32_t block1_w5:32;
    };
    uint32_t val;
} otp_debug_blk1_w5_reg_t;


/** Group: OTP_DEBUG Block1 Word6 Data */
/** Type of blk1_w6 register
 *  Otp debuger block1 data register6.
 */
typedef union {
    struct {
        /** block1_w6 : RO; bitpos: [31:0]; default: 0;
         *  Otp block1  word6 data.
         */
        uint32_t block1_w6:32;
    };
    uint32_t val;
} otp_debug_blk1_w6_reg_t;


/** Group: OTP_DEBUG Block1 Word7 Data */
/** Type of blk1_w7 register
 *  Otp debuger block1 data register7.
 */
typedef union {
    struct {
        /** block1_w7 : RO; bitpos: [31:0]; default: 0;
         *  Otp block1  word7 data.
         */
        uint32_t block1_w7:32;
    };
    uint32_t val;
} otp_debug_blk1_w7_reg_t;


/** Group: OTP_DEBUG Block1 Word8 Data */
/** Type of blk1_w8 register
 *  Otp debuger block1 data register8.
 */
typedef union {
    struct {
        /** block1_w8 : RO; bitpos: [31:0]; default: 0;
         *  Otp block1  word8 data.
         */
        uint32_t block1_w8:32;
    };
    uint32_t val;
} otp_debug_blk1_w8_reg_t;


/** Group: OTP_DEBUG Block1 Word9 Data */
/** Type of blk1_w9 register
 *  Otp debuger block1 data register9.
 */
typedef union {
    struct {
        /** block1_w9 : RO; bitpos: [31:0]; default: 0;
         *  Otp block1  word9 data.
         */
        uint32_t block1_w9:32;
    };
    uint32_t val;
} otp_debug_blk1_w9_reg_t;


/** Group: OTP_DEBUG Block2 Word1 Data */
/** Type of blk2_w1 register
 *  Otp debuger block2 data register1.
 */
typedef union {
    struct {
        /** block2_w1 : RO; bitpos: [31:0]; default: 0;
         *  Otp block2 word1 data.
         */
        uint32_t block2_w1:32;
    };
    uint32_t val;
} otp_debug_blk2_w1_reg_t;


/** Group: OTP_DEBUG Block2 Word2 Data */
/** Type of blk2_w2 register
 *  Otp debuger block2 data register2.
 */
typedef union {
    struct {
        /** block2_w2 : RO; bitpos: [31:0]; default: 0;
         *  Otp block2 word2 data.
         */
        uint32_t block2_w2:32;
    };
    uint32_t val;
} otp_debug_blk2_w2_reg_t;


/** Group: OTP_DEBUG Block2 Word3 Data */
/** Type of blk2_w3 register
 *  Otp debuger block2 data register3.
 */
typedef union {
    struct {
        /** block2_w3 : RO; bitpos: [31:0]; default: 0;
         *  Otp block2 word3 data.
         */
        uint32_t block2_w3:32;
    };
    uint32_t val;
} otp_debug_blk2_w3_reg_t;


/** Group: OTP_DEBUG Block2 Word4 Data */
/** Type of blk2_w4 register
 *  Otp debuger block2 data register4.
 */
typedef union {
    struct {
        /** block2_w4 : RO; bitpos: [31:0]; default: 0;
         *  Otp block2 word4 data.
         */
        uint32_t block2_w4:32;
    };
    uint32_t val;
} otp_debug_blk2_w4_reg_t;


/** Group: OTP_DEBUG Block2 Word5 Data */
/** Type of blk2_w5 register
 *  Otp debuger block2 data register5.
 */
typedef union {
    struct {
        /** block2_w5 : RO; bitpos: [31:0]; default: 0;
         *  Otp block2 word5 data.
         */
        uint32_t block2_w5:32;
    };
    uint32_t val;
} otp_debug_blk2_w5_reg_t;


/** Group: OTP_DEBUG Block2 Word6 Data */
/** Type of blk2_w6 register
 *  Otp debuger block2 data register6.
 */
typedef union {
    struct {
        /** block2_w6 : RO; bitpos: [31:0]; default: 0;
         *  Otp block2 word6 data.
         */
        uint32_t block2_w6:32;
    };
    uint32_t val;
} otp_debug_blk2_w6_reg_t;


/** Group: OTP_DEBUG Block2 Word7 Data */
/** Type of blk2_w7 register
 *  Otp debuger block2 data register7.
 */
typedef union {
    struct {
        /** block2_w7 : RO; bitpos: [31:0]; default: 0;
         *  Otp block2 word7 data.
         */
        uint32_t block2_w7:32;
    };
    uint32_t val;
} otp_debug_blk2_w7_reg_t;


/** Group: OTP_DEBUG Block2 Word8 Data */
/** Type of blk2_w8 register
 *  Otp debuger block2 data register8.
 */
typedef union {
    struct {
        /** block2_w8 : RO; bitpos: [31:0]; default: 0;
         *  Otp block2 word8 data.
         */
        uint32_t block2_w8:32;
    };
    uint32_t val;
} otp_debug_blk2_w8_reg_t;


/** Group: OTP_DEBUG Block2 Word9 Data */
/** Type of blk2_w9 register
 *  Otp debuger block2 data register9.
 */
typedef union {
    struct {
        /** block2_w9 : RO; bitpos: [31:0]; default: 0;
         *  Otp block2 word9 data.
         */
        uint32_t block2_w9:32;
    };
    uint32_t val;
} otp_debug_blk2_w9_reg_t;


/** Group: OTP_DEBUG Block2 Word10 Data */
/** Type of blk2_w10 register
 *  Otp debuger block2 data register10.
 */
typedef union {
    struct {
        /** block2_w10 : RO; bitpos: [31:0]; default: 0;
         *  Otp block2 word10 data.
         */
        uint32_t block2_w10:32;
    };
    uint32_t val;
} otp_debug_blk2_w10_reg_t;


/** Group: OTP_DEBUG Block2 Word11 Data */
/** Type of blk2_w11 register
 *  Otp debuger block2 data register11.
 */
typedef union {
    struct {
        /** block2_w11 : RO; bitpos: [31:0]; default: 0;
         *  Otp block2 word11 data.
         */
        uint32_t block2_w11:32;
    };
    uint32_t val;
} otp_debug_blk2_w11_reg_t;

/** Type of blk10_w11 register
 *  Otp debuger block10 data register11.
 */
typedef union {
    struct {
        /** block10_w11 : RO; bitpos: [31:0]; default: 0;
         *  Otp block10 word11 data.
         */
        uint32_t block10_w11:32;
    };
    uint32_t val;
} otp_debug_blk10_w11_reg_t;


/** Group: OTP_DEBUG Block3 Word1 Data */
/** Type of blk3_w1 register
 *  Otp debuger block3 data register1.
 */
typedef union {
    struct {
        /** block3_w1 : RO; bitpos: [31:0]; default: 0;
         *  Otp block3 word1 data.
         */
        uint32_t block3_w1:32;
    };
    uint32_t val;
} otp_debug_blk3_w1_reg_t;


/** Group: OTP_DEBUG Block3 Word2 Data */
/** Type of blk3_w2 register
 *  Otp debuger block3 data register2.
 */
typedef union {
    struct {
        /** block3_w2 : RO; bitpos: [31:0]; default: 0;
         *  Otp block3 word2 data.
         */
        uint32_t block3_w2:32;
    };
    uint32_t val;
} otp_debug_blk3_w2_reg_t;


/** Group: OTP_DEBUG Block3 Word3 Data */
/** Type of blk3_w3 register
 *  Otp debuger block3 data register3.
 */
typedef union {
    struct {
        /** block3_w3 : RO; bitpos: [31:0]; default: 0;
         *  Otp block3 word3 data.
         */
        uint32_t block3_w3:32;
    };
    uint32_t val;
} otp_debug_blk3_w3_reg_t;


/** Group: OTP_DEBUG Block3 Word4 Data */
/** Type of blk3_w4 register
 *  Otp debuger block3 data register4.
 */
typedef union {
    struct {
        /** block3_w4 : RO; bitpos: [31:0]; default: 0;
         *  Otp block3 word4 data.
         */
        uint32_t block3_w4:32;
    };
    uint32_t val;
} otp_debug_blk3_w4_reg_t;


/** Group: OTP_DEBUG Block3 Word5 Data */
/** Type of blk3_w5 register
 *  Otp debuger block3 data register5.
 */
typedef union {
    struct {
        /** block3_w5 : RO; bitpos: [31:0]; default: 0;
         *  Otp block3 word5 data.
         */
        uint32_t block3_w5:32;
    };
    uint32_t val;
} otp_debug_blk3_w5_reg_t;


/** Group: OTP_DEBUG Block3 Word6 Data */
/** Type of blk3_w6 register
 *  Otp debuger block3 data register6.
 */
typedef union {
    struct {
        /** block3_w6 : RO; bitpos: [31:0]; default: 0;
         *  Otp block3 word6 data.
         */
        uint32_t block3_w6:32;
    };
    uint32_t val;
} otp_debug_blk3_w6_reg_t;


/** Group: OTP_DEBUG Block3 Word7 Data */
/** Type of blk3_w7 register
 *  Otp debuger block3 data register7.
 */
typedef union {
    struct {
        /** block3_w7 : RO; bitpos: [31:0]; default: 0;
         *  Otp block3 word7 data.
         */
        uint32_t block3_w7:32;
    };
    uint32_t val;
} otp_debug_blk3_w7_reg_t;


/** Group: OTP_DEBUG Block3 Word8 Data */
/** Type of blk3_w8 register
 *  Otp debuger block3 data register8.
 */
typedef union {
    struct {
        /** block3_w8 : RO; bitpos: [31:0]; default: 0;
         *  Otp block3 word8 data.
         */
        uint32_t block3_w8:32;
    };
    uint32_t val;
} otp_debug_blk3_w8_reg_t;


/** Group: OTP_DEBUG Block3 Word9 Data */
/** Type of blk3_w9 register
 *  Otp debuger block3 data register9.
 */
typedef union {
    struct {
        /** block3_w9 : RO; bitpos: [31:0]; default: 0;
         *  Otp block3 word9 data.
         */
        uint32_t block3_w9:32;
    };
    uint32_t val;
} otp_debug_blk3_w9_reg_t;


/** Group: OTP_DEBUG Block3 Word10 Data */
/** Type of blk3_w10 register
 *  Otp debuger block3 data register10.
 */
typedef union {
    struct {
        /** block3_w10 : RO; bitpos: [31:0]; default: 0;
         *  Otp block3 word10 data.
         */
        uint32_t block3_w10:32;
    };
    uint32_t val;
} otp_debug_blk3_w10_reg_t;


/** Group: OTP_DEBUG Block3 Word11 Data */
/** Type of blk3_w11 register
 *  Otp debuger block3 data register11.
 */
typedef union {
    struct {
        /** block3_w11 : RO; bitpos: [31:0]; default: 0;
         *  Otp block3 word11 data.
         */
        uint32_t block3_w11:32;
    };
    uint32_t val;
} otp_debug_blk3_w11_reg_t;


/** Group: OTP_DEBUG Block4 Word1 Data */
/** Type of blk4_w1 register
 *  Otp debuger block4 data register1.
 */
typedef union {
    struct {
        /** block4_w1 : RO; bitpos: [31:0]; default: 0;
         *  Otp block4 word1 data.
         */
        uint32_t block4_w1:32;
    };
    uint32_t val;
} otp_debug_blk4_w1_reg_t;


/** Group: OTP_DEBUG Block4 Word2 Data */
/** Type of blk4_w2 register
 *  Otp debuger block4 data register2.
 */
typedef union {
    struct {
        /** block4_w2 : RO; bitpos: [31:0]; default: 0;
         *  Otp block4 word2 data.
         */
        uint32_t block4_w2:32;
    };
    uint32_t val;
} otp_debug_blk4_w2_reg_t;


/** Group: OTP_DEBUG Block4 Word3 Data */
/** Type of blk4_w3 register
 *  Otp debuger block4 data register3.
 */
typedef union {
    struct {
        /** block4_w3 : RO; bitpos: [31:0]; default: 0;
         *  Otp block4 word3 data.
         */
        uint32_t block4_w3:32;
    };
    uint32_t val;
} otp_debug_blk4_w3_reg_t;


/** Group: OTP_DEBUG Block4 Word4 Data */
/** Type of blk4_w4 register
 *  Otp debuger block4 data register4.
 */
typedef union {
    struct {
        /** block4_w4 : RO; bitpos: [31:0]; default: 0;
         *  Otp block4 word4 data.
         */
        uint32_t block4_w4:32;
    };
    uint32_t val;
} otp_debug_blk4_w4_reg_t;


/** Group: OTP_DEBUG Block4 Word5 Data */
/** Type of blk4_w5 register
 *  Otp debuger block4 data register5.
 */
typedef union {
    struct {
        /** block4_w5 : RO; bitpos: [31:0]; default: 0;
         *  Otp block4 word5 data.
         */
        uint32_t block4_w5:32;
    };
    uint32_t val;
} otp_debug_blk4_w5_reg_t;


/** Group: OTP_DEBUG Block4 Word6 Data */
/** Type of blk4_w6 register
 *  Otp debuger block4 data register6.
 */
typedef union {
    struct {
        /** block4_w6 : RO; bitpos: [31:0]; default: 0;
         *  Otp block4 word6 data.
         */
        uint32_t block4_w6:32;
    };
    uint32_t val;
} otp_debug_blk4_w6_reg_t;


/** Group: OTP_DEBUG Block4 Word7 Data */
/** Type of blk4_w7 register
 *  Otp debuger block4 data register7.
 */
typedef union {
    struct {
        /** block4_w7 : RO; bitpos: [31:0]; default: 0;
         *  Otp block4 word7 data.
         */
        uint32_t block4_w7:32;
    };
    uint32_t val;
} otp_debug_blk4_w7_reg_t;


/** Group: OTP_DEBUG Block4 Word8 Data */
/** Type of blk4_w8 register
 *  Otp debuger block4 data register8.
 */
typedef union {
    struct {
        /** block4_w8 : RO; bitpos: [31:0]; default: 0;
         *  Otp block4 word8 data.
         */
        uint32_t block4_w8:32;
    };
    uint32_t val;
} otp_debug_blk4_w8_reg_t;


/** Group: OTP_DEBUG Block4 Word9 Data */
/** Type of blk4_w9 register
 *  Otp debuger block4 data register9.
 */
typedef union {
    struct {
        /** block4_w9 : RO; bitpos: [31:0]; default: 0;
         *  Otp block4 word9 data.
         */
        uint32_t block4_w9:32;
    };
    uint32_t val;
} otp_debug_blk4_w9_reg_t;


/** Group: OTP_DEBUG Block4 Word10 Data */
/** Type of blk4_w10 register
 *  Otp debuger block4 data registe10.
 */
typedef union {
    struct {
        /** block4_w10 : RO; bitpos: [31:0]; default: 0;
         *  Otp block4 word10 data.
         */
        uint32_t block4_w10:32;
    };
    uint32_t val;
} otp_debug_blk4_w10_reg_t;


/** Group: OTP_DEBUG Block4 Word11 Data */
/** Type of blk4_w11 register
 *  Otp debuger block4 data register11.
 */
typedef union {
    struct {
        /** block4_w11 : RO; bitpos: [31:0]; default: 0;
         *  Otp block4 word11 data.
         */
        uint32_t block4_w11:32;
    };
    uint32_t val;
} otp_debug_blk4_w11_reg_t;


/** Group: OTP_DEBUG Block5 Word1 Data */
/** Type of blk5_w1 register
 *  Otp debuger block5 data register1.
 */
typedef union {
    struct {
        /** block5_w1 : RO; bitpos: [31:0]; default: 0;
         *  Otp block5 word1 data.
         */
        uint32_t block5_w1:32;
    };
    uint32_t val;
} otp_debug_blk5_w1_reg_t;


/** Group: OTP_DEBUG Block5 Word2 Data */
/** Type of blk5_w2 register
 *  Otp debuger block5 data register2.
 */
typedef union {
    struct {
        /** block5_w2 : RO; bitpos: [31:0]; default: 0;
         *  Otp block5 word2 data.
         */
        uint32_t block5_w2:32;
    };
    uint32_t val;
} otp_debug_blk5_w2_reg_t;


/** Group: OTP_DEBUG Block5 Word3 Data */
/** Type of blk5_w3 register
 *  Otp debuger block5 data register3.
 */
typedef union {
    struct {
        /** block5_w3 : RO; bitpos: [31:0]; default: 0;
         *  Otp block5 word3 data.
         */
        uint32_t block5_w3:32;
    };
    uint32_t val;
} otp_debug_blk5_w3_reg_t;


/** Group: OTP_DEBUG Block5 Word4 Data */
/** Type of blk5_w4 register
 *  Otp debuger block5 data register4.
 */
typedef union {
    struct {
        /** block5_w4 : RO; bitpos: [31:0]; default: 0;
         *  Otp block5 word4 data.
         */
        uint32_t block5_w4:32;
    };
    uint32_t val;
} otp_debug_blk5_w4_reg_t;


/** Group: OTP_DEBUG Block5 Word5 Data */
/** Type of blk5_w5 register
 *  Otp debuger block5 data register5.
 */
typedef union {
    struct {
        /** block5_w5 : RO; bitpos: [31:0]; default: 0;
         *  Otp block5 word5 data.
         */
        uint32_t block5_w5:32;
    };
    uint32_t val;
} otp_debug_blk5_w5_reg_t;


/** Group: OTP_DEBUG Block5 Word6 Data */
/** Type of blk5_w6 register
 *  Otp debuger block5 data register6.
 */
typedef union {
    struct {
        /** block5_w6 : RO; bitpos: [31:0]; default: 0;
         *  Otp block5 word6 data.
         */
        uint32_t block5_w6:32;
    };
    uint32_t val;
} otp_debug_blk5_w6_reg_t;


/** Group: OTP_DEBUG Block5 Word7 Data */
/** Type of blk5_w7 register
 *  Otp debuger block5 data register7.
 */
typedef union {
    struct {
        /** block5_w7 : RO; bitpos: [31:0]; default: 0;
         *  Otp block5 word7 data.
         */
        uint32_t block5_w7:32;
    };
    uint32_t val;
} otp_debug_blk5_w7_reg_t;


/** Group: OTP_DEBUG Block5 Word8 Data */
/** Type of blk5_w8 register
 *  Otp debuger block5 data register8.
 */
typedef union {
    struct {
        /** block5_w8 : RO; bitpos: [31:0]; default: 0;
         *  Otp block5 word8 data.
         */
        uint32_t block5_w8:32;
    };
    uint32_t val;
} otp_debug_blk5_w8_reg_t;


/** Group: OTP_DEBUG Block5 Word9 Data */
/** Type of blk5_w9 register
 *  Otp debuger block5 data register9.
 */
typedef union {
    struct {
        /** block5_w9 : RO; bitpos: [31:0]; default: 0;
         *  Otp block5 word9 data.
         */
        uint32_t block5_w9:32;
    };
    uint32_t val;
} otp_debug_blk5_w9_reg_t;


/** Group: OTP_DEBUG Block5 Word10 Data */
/** Type of blk5_w10 register
 *  Otp debuger block5 data register10.
 */
typedef union {
    struct {
        /** block5_w10 : RO; bitpos: [31:0]; default: 0;
         *  Otp block5 word10 data.
         */
        uint32_t block5_w10:32;
    };
    uint32_t val;
} otp_debug_blk5_w10_reg_t;


/** Group: OTP_DEBUG Block5 Word11 Data */
/** Type of blk5_w11 register
 *  Otp debuger block5 data register11.
 */
typedef union {
    struct {
        /** block5_w11 : RO; bitpos: [31:0]; default: 0;
         *  Otp block5 word11 data.
         */
        uint32_t block5_w11:32;
    };
    uint32_t val;
} otp_debug_blk5_w11_reg_t;


/** Group: OTP_DEBUG Block6 Word1 Data */
/** Type of blk6_w1 register
 *  Otp debuger block6 data register1.
 */
typedef union {
    struct {
        /** block6_w1 : RO; bitpos: [31:0]; default: 0;
         *  Otp block6 word1 data.
         */
        uint32_t block6_w1:32;
    };
    uint32_t val;
} otp_debug_blk6_w1_reg_t;


/** Group: OTP_DEBUG Block6 Word2 Data */
/** Type of blk6_w2 register
 *  Otp debuger block6 data register2.
 */
typedef union {
    struct {
        /** block6_w2 : RO; bitpos: [31:0]; default: 0;
         *  Otp block6 word2 data.
         */
        uint32_t block6_w2:32;
    };
    uint32_t val;
} otp_debug_blk6_w2_reg_t;


/** Group: OTP_DEBUG Block6 Word3 Data */
/** Type of blk6_w3 register
 *  Otp debuger block6 data register3.
 */
typedef union {
    struct {
        /** block6_w3 : RO; bitpos: [31:0]; default: 0;
         *  Otp block6 word3 data.
         */
        uint32_t block6_w3:32;
    };
    uint32_t val;
} otp_debug_blk6_w3_reg_t;


/** Group: OTP_DEBUG Block6 Word4 Data */
/** Type of blk6_w4 register
 *  Otp debuger block6 data register4.
 */
typedef union {
    struct {
        /** block6_w4 : RO; bitpos: [31:0]; default: 0;
         *  Otp block6 word4 data.
         */
        uint32_t block6_w4:32;
    };
    uint32_t val;
} otp_debug_blk6_w4_reg_t;


/** Group: OTP_DEBUG Block6 Word5 Data */
/** Type of blk6_w5 register
 *  Otp debuger block6 data register5.
 */
typedef union {
    struct {
        /** block6_w5 : RO; bitpos: [31:0]; default: 0;
         *  Otp block6 word5 data.
         */
        uint32_t block6_w5:32;
    };
    uint32_t val;
} otp_debug_blk6_w5_reg_t;


/** Group: OTP_DEBUG Block6 Word6 Data */
/** Type of blk6_w6 register
 *  Otp debuger block6 data register6.
 */
typedef union {
    struct {
        /** block6_w6 : RO; bitpos: [31:0]; default: 0;
         *  Otp block6 word6 data.
         */
        uint32_t block6_w6:32;
    };
    uint32_t val;
} otp_debug_blk6_w6_reg_t;


/** Group: OTP_DEBUG Block6 Word7 Data */
/** Type of blk6_w7 register
 *  Otp debuger block6 data register7.
 */
typedef union {
    struct {
        /** block6_w7 : RO; bitpos: [31:0]; default: 0;
         *  Otp block6 word7 data.
         */
        uint32_t block6_w7:32;
    };
    uint32_t val;
} otp_debug_blk6_w7_reg_t;


/** Group: OTP_DEBUG Block6 Word8 Data */
/** Type of blk6_w8 register
 *  Otp debuger block6 data register8.
 */
typedef union {
    struct {
        /** block6_w8 : RO; bitpos: [31:0]; default: 0;
         *  Otp block6 word8 data.
         */
        uint32_t block6_w8:32;
    };
    uint32_t val;
} otp_debug_blk6_w8_reg_t;


/** Group: OTP_DEBUG Block6 Word9 Data */
/** Type of blk6_w9 register
 *  Otp debuger block6 data register9.
 */
typedef union {
    struct {
        /** block6_w9 : RO; bitpos: [31:0]; default: 0;
         *  Otp block6 word9 data.
         */
        uint32_t block6_w9:32;
    };
    uint32_t val;
} otp_debug_blk6_w9_reg_t;


/** Group: OTP_DEBUG Block6 Word10 Data */
/** Type of blk6_w10 register
 *  Otp debuger block6 data register10.
 */
typedef union {
    struct {
        /** block6_w10 : RO; bitpos: [31:0]; default: 0;
         *  Otp block6 word10 data.
         */
        uint32_t block6_w10:32;
    };
    uint32_t val;
} otp_debug_blk6_w10_reg_t;


/** Group: OTP_DEBUG Block6 Word11 Data */
/** Type of blk6_w11 register
 *  Otp debuger block6 data register11.
 */
typedef union {
    struct {
        /** block6_w11 : RO; bitpos: [31:0]; default: 0;
         *  Otp block6 word11 data.
         */
        uint32_t block6_w11:32;
    };
    uint32_t val;
} otp_debug_blk6_w11_reg_t;


/** Group: OTP_DEBUG Block7 Word1 Data */
/** Type of blk7_w1 register
 *  Otp debuger block7 data register1.
 */
typedef union {
    struct {
        /** block7_w1 : RO; bitpos: [31:0]; default: 0;
         *  Otp block7 word1 data.
         */
        uint32_t block7_w1:32;
    };
    uint32_t val;
} otp_debug_blk7_w1_reg_t;


/** Group: OTP_DEBUG Block7 Word2 Data */
/** Type of blk7_w2 register
 *  Otp debuger block7 data register2.
 */
typedef union {
    struct {
        /** block7_w2 : RO; bitpos: [31:0]; default: 0;
         *  Otp block7 word2 data.
         */
        uint32_t block7_w2:32;
    };
    uint32_t val;
} otp_debug_blk7_w2_reg_t;


/** Group: OTP_DEBUG Block7 Word3 Data */
/** Type of blk7_w3 register
 *  Otp debuger block7 data register3.
 */
typedef union {
    struct {
        /** block7_w3 : RO; bitpos: [31:0]; default: 0;
         *  Otp block7 word3 data.
         */
        uint32_t block7_w3:32;
    };
    uint32_t val;
} otp_debug_blk7_w3_reg_t;


/** Group: OTP_DEBUG Block7 Word4 Data */
/** Type of blk7_w4 register
 *  Otp debuger block7 data register4.
 */
typedef union {
    struct {
        /** block7_w4 : RO; bitpos: [31:0]; default: 0;
         *  Otp block7 word4 data.
         */
        uint32_t block7_w4:32;
    };
    uint32_t val;
} otp_debug_blk7_w4_reg_t;


/** Group: OTP_DEBUG Block7 Word5 Data */
/** Type of blk7_w5 register
 *  Otp debuger block7 data register5.
 */
typedef union {
    struct {
        /** block7_w5 : RO; bitpos: [31:0]; default: 0;
         *  Otp block7 word5 data.
         */
        uint32_t block7_w5:32;
    };
    uint32_t val;
} otp_debug_blk7_w5_reg_t;


/** Group: OTP_DEBUG Block7 Word6 Data */
/** Type of blk7_w6 register
 *  Otp debuger block7 data register6.
 */
typedef union {
    struct {
        /** block7_w6 : RO; bitpos: [31:0]; default: 0;
         *  Otp block7 word6 data.
         */
        uint32_t block7_w6:32;
    };
    uint32_t val;
} otp_debug_blk7_w6_reg_t;


/** Group: OTP_DEBUG Block7 Word7 Data */
/** Type of blk7_w7 register
 *  Otp debuger block7 data register7.
 */
typedef union {
    struct {
        /** block7_w7 : RO; bitpos: [31:0]; default: 0;
         *  Otp block7 word7 data.
         */
        uint32_t block7_w7:32;
    };
    uint32_t val;
} otp_debug_blk7_w7_reg_t;


/** Group: OTP_DEBUG Block7 Word8 Data */
/** Type of blk7_w8 register
 *  Otp debuger block7 data register8.
 */
typedef union {
    struct {
        /** block7_w8 : RO; bitpos: [31:0]; default: 0;
         *  Otp block7 word8 data.
         */
        uint32_t block7_w8:32;
    };
    uint32_t val;
} otp_debug_blk7_w8_reg_t;


/** Group: OTP_DEBUG Block7 Word9 Data */
/** Type of blk7_w9 register
 *  Otp debuger block7 data register9.
 */
typedef union {
    struct {
        /** block7_w9 : RO; bitpos: [31:0]; default: 0;
         *  Otp block7 word9 data.
         */
        uint32_t block7_w9:32;
    };
    uint32_t val;
} otp_debug_blk7_w9_reg_t;


/** Group: OTP_DEBUG Block7 Word10 Data */
/** Type of blk7_w10 register
 *  Otp debuger block7 data register10.
 */
typedef union {
    struct {
        /** block7_w10 : RO; bitpos: [31:0]; default: 0;
         *  Otp block7 word10 data.
         */
        uint32_t block7_w10:32;
    };
    uint32_t val;
} otp_debug_blk7_w10_reg_t;


/** Group: OTP_DEBUG Block7 Word11 Data */
/** Type of blk7_w11 register
 *  Otp debuger block7 data register11.
 */
typedef union {
    struct {
        /** block7_w11 : RO; bitpos: [31:0]; default: 0;
         *  Otp block7 word11 data.
         */
        uint32_t block7_w11:32;
    };
    uint32_t val;
} otp_debug_blk7_w11_reg_t;


/** Group: OTP_DEBUG Block8 Word1 Data */
/** Type of blk8_w1 register
 *  Otp debuger block8 data register1.
 */
typedef union {
    struct {
        /** block8_w1 : RO; bitpos: [31:0]; default: 0;
         *  Otp block8 word1 data.
         */
        uint32_t block8_w1:32;
    };
    uint32_t val;
} otp_debug_blk8_w1_reg_t;


/** Group: OTP_DEBUG Block8 Word2 Data */
/** Type of blk8_w2 register
 *  Otp debuger block8 data register2.
 */
typedef union {
    struct {
        /** block8_w2 : RO; bitpos: [31:0]; default: 0;
         *  Otp block8 word2 data.
         */
        uint32_t block8_w2:32;
    };
    uint32_t val;
} otp_debug_blk8_w2_reg_t;


/** Group: OTP_DEBUG Block8 Word3 Data */
/** Type of blk8_w3 register
 *  Otp debuger block8 data register3.
 */
typedef union {
    struct {
        /** block8_w3 : RO; bitpos: [31:0]; default: 0;
         *  Otp block8 word3 data.
         */
        uint32_t block8_w3:32;
    };
    uint32_t val;
} otp_debug_blk8_w3_reg_t;


/** Group: OTP_DEBUG Block8 Word4 Data */
/** Type of blk8_w4 register
 *  Otp debuger block8 data register4.
 */
typedef union {
    struct {
        /** block8_w4 : RO; bitpos: [31:0]; default: 0;
         *  Otp block8 word4 data.
         */
        uint32_t block8_w4:32;
    };
    uint32_t val;
} otp_debug_blk8_w4_reg_t;


/** Group: OTP_DEBUG Block8 Word5 Data */
/** Type of blk8_w5 register
 *  Otp debuger block8 data register5.
 */
typedef union {
    struct {
        /** block8_w5 : RO; bitpos: [31:0]; default: 0;
         *  Otp block8 word5 data.
         */
        uint32_t block8_w5:32;
    };
    uint32_t val;
} otp_debug_blk8_w5_reg_t;


/** Group: OTP_DEBUG Block8 Word6 Data */
/** Type of blk8_w6 register
 *  Otp debuger block8 data register6.
 */
typedef union {
    struct {
        /** block8_w6 : RO; bitpos: [31:0]; default: 0;
         *  Otp block8 word6 data.
         */
        uint32_t block8_w6:32;
    };
    uint32_t val;
} otp_debug_blk8_w6_reg_t;


/** Group: OTP_DEBUG Block8 Word7 Data */
/** Type of blk8_w7 register
 *  Otp debuger block8 data register7.
 */
typedef union {
    struct {
        /** block8_w7 : RO; bitpos: [31:0]; default: 0;
         *  Otp block8 word7 data.
         */
        uint32_t block8_w7:32;
    };
    uint32_t val;
} otp_debug_blk8_w7_reg_t;


/** Group: OTP_DEBUG Block8 Word8 Data */
/** Type of blk8_w8 register
 *  Otp debuger block8 data register8.
 */
typedef union {
    struct {
        /** block8_w8 : RO; bitpos: [31:0]; default: 0;
         *  Otp block8 word8 data.
         */
        uint32_t block8_w8:32;
    };
    uint32_t val;
} otp_debug_blk8_w8_reg_t;


/** Group: OTP_DEBUG Block8 Word9 Data */
/** Type of blk8_w9 register
 *  Otp debuger block8 data register9.
 */
typedef union {
    struct {
        /** block8_w9 : RO; bitpos: [31:0]; default: 0;
         *  Otp block8 word9 data.
         */
        uint32_t block8_w9:32;
    };
    uint32_t val;
} otp_debug_blk8_w9_reg_t;


/** Group: OTP_DEBUG Block8 Word10 Data */
/** Type of blk8_w10 register
 *  Otp debuger block8 data register10.
 */
typedef union {
    struct {
        /** block8_w10 : RO; bitpos: [31:0]; default: 0;
         *  Otp block8 word10 data.
         */
        uint32_t block8_w10:32;
    };
    uint32_t val;
} otp_debug_blk8_w10_reg_t;


/** Group: OTP_DEBUG Block8 Word11 Data */
/** Type of blk8_w11 register
 *  Otp debuger block8 data register11.
 */
typedef union {
    struct {
        /** block8_w11 : RO; bitpos: [31:0]; default: 0;
         *  Otp block8 word11 data.
         */
        uint32_t block8_w11:32;
    };
    uint32_t val;
} otp_debug_blk8_w11_reg_t;


/** Group: OTP_DEBUG Block9 Word1 Data */
/** Type of blk9_w1 register
 *  Otp debuger block9 data register1.
 */
typedef union {
    struct {
        /** block9_w1 : RO; bitpos: [31:0]; default: 0;
         *  Otp block9 word1 data.
         */
        uint32_t block9_w1:32;
    };
    uint32_t val;
} otp_debug_blk9_w1_reg_t;


/** Group: OTP_DEBUG Block9 Word2 Data */
/** Type of blk9_w2 register
 *  Otp debuger block9 data register2.
 */
typedef union {
    struct {
        /** block9_w2 : RO; bitpos: [31:0]; default: 0;
         *  Otp block9 word2 data.
         */
        uint32_t block9_w2:32;
    };
    uint32_t val;
} otp_debug_blk9_w2_reg_t;


/** Group: OTP_DEBUG Block9 Word3 Data */
/** Type of blk9_w3 register
 *  Otp debuger block9 data register3.
 */
typedef union {
    struct {
        /** block9_w3 : RO; bitpos: [31:0]; default: 0;
         *  Otp block9 word3 data.
         */
        uint32_t block9_w3:32;
    };
    uint32_t val;
} otp_debug_blk9_w3_reg_t;


/** Group: OTP_DEBUG Block9 Word4 Data */
/** Type of blk9_w4 register
 *  Otp debuger block9 data register4.
 */
typedef union {
    struct {
        /** block9_w4 : RO; bitpos: [31:0]; default: 0;
         *  Otp block9 word4 data.
         */
        uint32_t block9_w4:32;
    };
    uint32_t val;
} otp_debug_blk9_w4_reg_t;


/** Group: OTP_DEBUG Block9 Word5 Data */
/** Type of blk9_w5 register
 *  Otp debuger block9 data register5.
 */
typedef union {
    struct {
        /** block9_w5 : RO; bitpos: [31:0]; default: 0;
         *  Otp block9 word5 data.
         */
        uint32_t block9_w5:32;
    };
    uint32_t val;
} otp_debug_blk9_w5_reg_t;


/** Group: OTP_DEBUG Block9 Word6 Data */
/** Type of blk9_w6 register
 *  Otp debuger block9 data register6.
 */
typedef union {
    struct {
        /** block9_w6 : RO; bitpos: [31:0]; default: 0;
         *  Otp block9 word6 data.
         */
        uint32_t block9_w6:32;
    };
    uint32_t val;
} otp_debug_blk9_w6_reg_t;


/** Group: OTP_DEBUG Block9 Word7 Data */
/** Type of blk9_w7 register
 *  Otp debuger block9 data register7.
 */
typedef union {
    struct {
        /** block9_w7 : RO; bitpos: [31:0]; default: 0;
         *  Otp block9 word7 data.
         */
        uint32_t block9_w7:32;
    };
    uint32_t val;
} otp_debug_blk9_w7_reg_t;


/** Group: OTP_DEBUG Block9 Word8 Data */
/** Type of blk9_w8 register
 *  Otp debuger block9 data register8.
 */
typedef union {
    struct {
        /** block9_w8 : RO; bitpos: [31:0]; default: 0;
         *  Otp block9 word8 data.
         */
        uint32_t block9_w8:32;
    };
    uint32_t val;
} otp_debug_blk9_w8_reg_t;


/** Group: OTP_DEBUG Block9 Word9 Data */
/** Type of blk9_w9 register
 *  Otp debuger block9 data register9.
 */
typedef union {
    struct {
        /** block9_w9 : RO; bitpos: [31:0]; default: 0;
         *  Otp block9 word9 data.
         */
        uint32_t block9_w9:32;
    };
    uint32_t val;
} otp_debug_blk9_w9_reg_t;


/** Group: OTP_DEBUG Block9 Word10 Data */
/** Type of blk9_w10 register
 *  Otp debuger block9 data register10.
 */
typedef union {
    struct {
        /** block9_w10 : RO; bitpos: [31:0]; default: 0;
         *  Otp block9 word10 data.
         */
        uint32_t block9_w10:32;
    };
    uint32_t val;
} otp_debug_blk9_w10_reg_t;


/** Group: OTP_DEBUG Block9 Word11 Data */
/** Type of blk9_w11 register
 *  Otp debuger block9 data register11.
 */
typedef union {
    struct {
        /** block9_w11 : RO; bitpos: [31:0]; default: 0;
         *  Otp block9 word11 data.
         */
        uint32_t block9_w11:32;
    };
    uint32_t val;
} otp_debug_blk9_w11_reg_t;


/** Group: OTP_DEBUG Block10 Word1 Data */
/** Type of blk10_w1 register
 *  Otp debuger block10 data register1.
 */
typedef union {
    struct {
        /** block10_w1 : RO; bitpos: [31:0]; default: 0;
         *  Otp block10 word1 data.
         */
        uint32_t block10_w1:32;
    };
    uint32_t val;
} otp_debug_blk10_w1_reg_t;


/** Group: OTP_DEBUG Block10 Word2 Data */
/** Type of blk10_w2 register
 *  Otp debuger block10 data register2.
 */
typedef union {
    struct {
        /** block10_w2 : RO; bitpos: [31:0]; default: 0;
         *  Otp block10 word2 data.
         */
        uint32_t block10_w2:32;
    };
    uint32_t val;
} otp_debug_blk10_w2_reg_t;


/** Group: OTP_DEBUG Block10 Word3 Data */
/** Type of blk10_w3 register
 *  Otp debuger block10 data register3.
 */
typedef union {
    struct {
        /** block10_w3 : RO; bitpos: [31:0]; default: 0;
         *  Otp block10 word3 data.
         */
        uint32_t block10_w3:32;
    };
    uint32_t val;
} otp_debug_blk10_w3_reg_t;


/** Group: OTP_DEBUG Block10 Word4 Data */
/** Type of blk10_w4 register
 *  Otp debuger block10 data register4.
 */
typedef union {
    struct {
        /** block10_w4 : RO; bitpos: [31:0]; default: 0;
         *  Otp block10 word4 data.
         */
        uint32_t block10_w4:32;
    };
    uint32_t val;
} otp_debug_blk10_w4_reg_t;


/** Group: OTP_DEBUG Block10 Word5 Data */
/** Type of blk10_w5 register
 *  Otp debuger block10 data register5.
 */
typedef union {
    struct {
        /** block10_w5 : RO; bitpos: [31:0]; default: 0;
         *  Otp block10 word5 data.
         */
        uint32_t block10_w5:32;
    };
    uint32_t val;
} otp_debug_blk10_w5_reg_t;


/** Group: OTP_DEBUG Block10 Word6 Data */
/** Type of blk10_w6 register
 *  Otp debuger block10 data register6.
 */
typedef union {
    struct {
        /** block10_w6 : RO; bitpos: [31:0]; default: 0;
         *  Otp block10 word6 data.
         */
        uint32_t block10_w6:32;
    };
    uint32_t val;
} otp_debug_blk10_w6_reg_t;


/** Group: OTP_DEBUG Block10 Word7 Data */
/** Type of blk10_w7 register
 *  Otp debuger block10 data register7.
 */
typedef union {
    struct {
        /** block10_w7 : RO; bitpos: [31:0]; default: 0;
         *  Otp block10 word7 data.
         */
        uint32_t block10_w7:32;
    };
    uint32_t val;
} otp_debug_blk10_w7_reg_t;


/** Group: OTP_DEBUG Block10 Word8 Data */
/** Type of blk10_w8 register
 *  Otp debuger block10 data register8.
 */
typedef union {
    struct {
        /** block10_w8 : RO; bitpos: [31:0]; default: 0;
         *  Otp block10 word8 data.
         */
        uint32_t block10_w8:32;
    };
    uint32_t val;
} otp_debug_blk10_w8_reg_t;


/** Group: OTP_DEBUG Block10 Word9 Data */
/** Type of blk10_w9 register
 *  Otp debuger block10 data register9.
 */
typedef union {
    struct {
        /** block10_w9 : RO; bitpos: [31:0]; default: 0;
         *  Otp block10 word9 data.
         */
        uint32_t block10_w9:32;
    };
    uint32_t val;
} otp_debug_blk10_w9_reg_t;


/** Group: OTP_DEBUG Block10 Word10 Data */
/** Type of blk10_w10 register
 *  Otp debuger block10 data register10.
 */
typedef union {
    struct {
        /** block19_w10 : RO; bitpos: [31:0]; default: 0;
         *  Otp block10 word10 data.
         */
        uint32_t block19_w10:32;
    };
    uint32_t val;
} otp_debug_blk10_w10_reg_t;


/** Group: OTP_DEBUG Clock_en Configuration Register */
/** Type of clk register
 *  Otp debuger clk_en configuration register.
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 0;
         *  Force clock on for this register file.
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} otp_debug_clk_reg_t;


/** Group: OTP_DEBUG Apb2otp Enable Singal */
/** Type of apb2otp_en register
 *  Otp_debuger apb2otp enable configuration register.
 */
typedef union {
    struct {
        /** apb2otp_en : R/W; bitpos: [0]; default: 0;
         *  Debug mode enable signal.
         */
        uint32_t apb2otp_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} otp_debug_apb2otp_en_reg_t;


/** Group: OTP_DEBUG Version Register */
/** Type of date register
 *  eFuse version register.
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 539037736;
         *  Stores otp_debug version.
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} otp_debug_date_reg_t;


typedef struct otp_debug_dev_t {
    volatile otp_debug_wr_dis_reg_t wr_dis;
    volatile otp_debug_blk0_backup1_w1_reg_t blk0_backup1_w1;
    volatile otp_debug_blk0_backup1_w2_reg_t blk0_backup1_w2;
    volatile otp_debug_blk0_backup1_w3_reg_t blk0_backup1_w3;
    volatile otp_debug_blk0_backup1_w4_reg_t blk0_backup1_w4;
    volatile otp_debug_blk0_backup1_w5_reg_t blk0_backup1_w5;
    volatile otp_debug_blk0_backup2_w1_reg_t blk0_backup2_w1;
    volatile otp_debug_blk0_backup2_w2_reg_t blk0_backup2_w2;
    volatile otp_debug_blk0_backup2_w3_reg_t blk0_backup2_w3;
    volatile otp_debug_blk0_backup2_w4_reg_t blk0_backup2_w4;
    volatile otp_debug_blk0_backup2_w5_reg_t blk0_backup2_w5;
    volatile otp_debug_blk0_backup3_w1_reg_t blk0_backup3_w1;
    volatile otp_debug_blk0_backup3_w2_reg_t blk0_backup3_w2;
    volatile otp_debug_blk0_backup3_w3_reg_t blk0_backup3_w3;
    volatile otp_debug_blk0_backup3_w4_reg_t blk0_backup3_w4;
    volatile otp_debug_blk0_backup3_w5_reg_t blk0_backup3_w5;
    volatile otp_debug_blk0_backup4_w1_reg_t blk0_backup4_w1;
    volatile otp_debug_blk0_backup4_w2_reg_t blk0_backup4_w2;
    volatile otp_debug_blk0_backup4_w3_reg_t blk0_backup4_w3;
    volatile otp_debug_blk0_backup4_w4_reg_t blk0_backup4_w4;
    volatile otp_debug_blk0_backup4_w5_reg_t blk0_backup4_w5;
    volatile otp_debug_blk1_w1_reg_t blk1_w1;
    volatile otp_debug_blk1_w2_reg_t blk1_w2;
    volatile otp_debug_blk1_w3_reg_t blk1_w3;
    volatile otp_debug_blk1_w4_reg_t blk1_w4;
    volatile otp_debug_blk1_w5_reg_t blk1_w5;
    volatile otp_debug_blk1_w6_reg_t blk1_w6;
    volatile otp_debug_blk1_w7_reg_t blk1_w7;
    volatile otp_debug_blk1_w8_reg_t blk1_w8;
    volatile otp_debug_blk1_w9_reg_t blk1_w9;
    volatile otp_debug_blk2_w1_reg_t blk2_w1;
    volatile otp_debug_blk2_w2_reg_t blk2_w2;
    volatile otp_debug_blk2_w3_reg_t blk2_w3;
    volatile otp_debug_blk2_w4_reg_t blk2_w4;
    volatile otp_debug_blk2_w5_reg_t blk2_w5;
    volatile otp_debug_blk2_w6_reg_t blk2_w6;
    volatile otp_debug_blk2_w7_reg_t blk2_w7;
    volatile otp_debug_blk2_w8_reg_t blk2_w8;
    volatile otp_debug_blk2_w9_reg_t blk2_w9;
    volatile otp_debug_blk2_w10_reg_t blk2_w10;
    volatile otp_debug_blk2_w11_reg_t blk2_w11;
    volatile otp_debug_blk3_w1_reg_t blk3_w1;
    volatile otp_debug_blk3_w2_reg_t blk3_w2;
    volatile otp_debug_blk3_w3_reg_t blk3_w3;
    volatile otp_debug_blk3_w4_reg_t blk3_w4;
    volatile otp_debug_blk3_w5_reg_t blk3_w5;
    volatile otp_debug_blk3_w6_reg_t blk3_w6;
    volatile otp_debug_blk3_w7_reg_t blk3_w7;
    volatile otp_debug_blk3_w8_reg_t blk3_w8;
    volatile otp_debug_blk3_w9_reg_t blk3_w9;
    volatile otp_debug_blk3_w10_reg_t blk3_w10;
    volatile otp_debug_blk3_w11_reg_t blk3_w11;
    volatile otp_debug_blk4_w1_reg_t blk4_w1;
    volatile otp_debug_blk4_w2_reg_t blk4_w2;
    volatile otp_debug_blk4_w3_reg_t blk4_w3;
    volatile otp_debug_blk4_w4_reg_t blk4_w4;
    volatile otp_debug_blk4_w5_reg_t blk4_w5;
    volatile otp_debug_blk4_w6_reg_t blk4_w6;
    volatile otp_debug_blk4_w7_reg_t blk4_w7;
    volatile otp_debug_blk4_w8_reg_t blk4_w8;
    volatile otp_debug_blk4_w9_reg_t blk4_w9;
    volatile otp_debug_blk4_w10_reg_t blk4_w10;
    volatile otp_debug_blk4_w11_reg_t blk4_w11;
    volatile otp_debug_blk5_w1_reg_t blk5_w1;
    volatile otp_debug_blk5_w2_reg_t blk5_w2;
    volatile otp_debug_blk5_w3_reg_t blk5_w3;
    volatile otp_debug_blk5_w4_reg_t blk5_w4;
    volatile otp_debug_blk5_w5_reg_t blk5_w5;
    volatile otp_debug_blk5_w6_reg_t blk5_w6;
    volatile otp_debug_blk5_w7_reg_t blk5_w7;
    volatile otp_debug_blk5_w8_reg_t blk5_w8;
    volatile otp_debug_blk5_w9_reg_t blk5_w9;
    volatile otp_debug_blk5_w10_reg_t blk5_w10;
    volatile otp_debug_blk5_w11_reg_t blk5_w11;
    volatile otp_debug_blk6_w1_reg_t blk6_w1;
    volatile otp_debug_blk6_w2_reg_t blk6_w2;
    volatile otp_debug_blk6_w3_reg_t blk6_w3;
    volatile otp_debug_blk6_w4_reg_t blk6_w4;
    volatile otp_debug_blk6_w5_reg_t blk6_w5;
    volatile otp_debug_blk6_w6_reg_t blk6_w6;
    volatile otp_debug_blk6_w7_reg_t blk6_w7;
    volatile otp_debug_blk6_w8_reg_t blk6_w8;
    volatile otp_debug_blk6_w9_reg_t blk6_w9;
    volatile otp_debug_blk6_w10_reg_t blk6_w10;
    volatile otp_debug_blk6_w11_reg_t blk6_w11;
    volatile otp_debug_blk7_w1_reg_t blk7_w1;
    volatile otp_debug_blk7_w2_reg_t blk7_w2;
    volatile otp_debug_blk7_w3_reg_t blk7_w3;
    volatile otp_debug_blk7_w4_reg_t blk7_w4;
    volatile otp_debug_blk7_w5_reg_t blk7_w5;
    volatile otp_debug_blk7_w6_reg_t blk7_w6;
    volatile otp_debug_blk7_w7_reg_t blk7_w7;
    volatile otp_debug_blk7_w8_reg_t blk7_w8;
    volatile otp_debug_blk7_w9_reg_t blk7_w9;
    volatile otp_debug_blk7_w10_reg_t blk7_w10;
    volatile otp_debug_blk7_w11_reg_t blk7_w11;
    volatile otp_debug_blk8_w1_reg_t blk8_w1;
    volatile otp_debug_blk8_w2_reg_t blk8_w2;
    volatile otp_debug_blk8_w3_reg_t blk8_w3;
    volatile otp_debug_blk8_w4_reg_t blk8_w4;
    volatile otp_debug_blk8_w5_reg_t blk8_w5;
    volatile otp_debug_blk8_w6_reg_t blk8_w6;
    volatile otp_debug_blk8_w7_reg_t blk8_w7;
    volatile otp_debug_blk8_w8_reg_t blk8_w8;
    volatile otp_debug_blk8_w9_reg_t blk8_w9;
    volatile otp_debug_blk8_w10_reg_t blk8_w10;
    volatile otp_debug_blk8_w11_reg_t blk8_w11;
    volatile otp_debug_blk9_w1_reg_t blk9_w1;
    volatile otp_debug_blk9_w2_reg_t blk9_w2;
    volatile otp_debug_blk9_w3_reg_t blk9_w3;
    volatile otp_debug_blk9_w4_reg_t blk9_w4;
    volatile otp_debug_blk9_w5_reg_t blk9_w5;
    volatile otp_debug_blk9_w6_reg_t blk9_w6;
    volatile otp_debug_blk9_w7_reg_t blk9_w7;
    volatile otp_debug_blk9_w8_reg_t blk9_w8;
    volatile otp_debug_blk9_w9_reg_t blk9_w9;
    volatile otp_debug_blk9_w10_reg_t blk9_w10;
    volatile otp_debug_blk9_w11_reg_t blk9_w11;
    volatile otp_debug_blk10_w1_reg_t blk10_w1;
    volatile otp_debug_blk10_w2_reg_t blk10_w2;
    volatile otp_debug_blk10_w3_reg_t blk10_w3;
    volatile otp_debug_blk10_w4_reg_t blk10_w4;
    volatile otp_debug_blk10_w5_reg_t blk10_w5;
    volatile otp_debug_blk10_w6_reg_t blk10_w6;
    volatile otp_debug_blk10_w7_reg_t blk10_w7;
    volatile otp_debug_blk10_w8_reg_t blk10_w8;
    volatile otp_debug_blk10_w9_reg_t blk10_w9;
    volatile otp_debug_blk10_w10_reg_t blk10_w10;
    volatile otp_debug_blk10_w11_reg_t blk10_w11;
    volatile otp_debug_clk_reg_t clk;
    volatile otp_debug_apb2otp_en_reg_t apb2otp_en;
    volatile otp_debug_date_reg_t date;
} otp_debug_dev_t;

extern otp_debug_dev_t OTP_DEBUG;

#ifndef __cplusplus
_Static_assert(sizeof(otp_debug_dev_t) == 0x210, "Invalid size of otp_debug_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
