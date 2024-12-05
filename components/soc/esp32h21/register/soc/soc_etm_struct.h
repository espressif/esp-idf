/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Configuration Register */
/** Type of etm_ch_ena_ad0 register
 *  channel enable register
 */
typedef union {
    struct {
        /** etm_ch_ena0 : R/WTC/WTS; bitpos: [0]; default: 0;
         *  ch0 enable
         */
        uint32_t etm_ch_ena0:1;
        /** etm_ch_ena1 : R/WTC/WTS; bitpos: [1]; default: 0;
         *  ch1 enable
         */
        uint32_t etm_ch_ena1:1;
        /** etm_ch_ena2 : R/WTC/WTS; bitpos: [2]; default: 0;
         *  ch2 enable
         */
        uint32_t etm_ch_ena2:1;
        /** etm_ch_ena3 : R/WTC/WTS; bitpos: [3]; default: 0;
         *  ch3 enable
         */
        uint32_t etm_ch_ena3:1;
        /** etm_ch_ena4 : R/WTC/WTS; bitpos: [4]; default: 0;
         *  ch4 enable
         */
        uint32_t etm_ch_ena4:1;
        /** etm_ch_ena5 : R/WTC/WTS; bitpos: [5]; default: 0;
         *  ch5 enable
         */
        uint32_t etm_ch_ena5:1;
        /** etm_ch_ena6 : R/WTC/WTS; bitpos: [6]; default: 0;
         *  ch6 enable
         */
        uint32_t etm_ch_ena6:1;
        /** etm_ch_ena7 : R/WTC/WTS; bitpos: [7]; default: 0;
         *  ch7 enable
         */
        uint32_t etm_ch_ena7:1;
        /** etm_ch_ena8 : R/WTC/WTS; bitpos: [8]; default: 0;
         *  ch8 enable
         */
        uint32_t etm_ch_ena8:1;
        /** etm_ch_ena9 : R/WTC/WTS; bitpos: [9]; default: 0;
         *  ch9 enable
         */
        uint32_t etm_ch_ena9:1;
        /** etm_ch_ena10 : R/WTC/WTS; bitpos: [10]; default: 0;
         *  ch10 enable
         */
        uint32_t etm_ch_ena10:1;
        /** etm_ch_ena11 : R/WTC/WTS; bitpos: [11]; default: 0;
         *  ch11 enable
         */
        uint32_t etm_ch_ena11:1;
        /** etm_ch_ena12 : R/WTC/WTS; bitpos: [12]; default: 0;
         *  ch12 enable
         */
        uint32_t etm_ch_ena12:1;
        /** etm_ch_ena13 : R/WTC/WTS; bitpos: [13]; default: 0;
         *  ch13 enable
         */
        uint32_t etm_ch_ena13:1;
        /** etm_ch_ena14 : R/WTC/WTS; bitpos: [14]; default: 0;
         *  ch14 enable
         */
        uint32_t etm_ch_ena14:1;
        /** etm_ch_ena15 : R/WTC/WTS; bitpos: [15]; default: 0;
         *  ch15 enable
         */
        uint32_t etm_ch_ena15:1;
        /** etm_ch_ena16 : R/WTC/WTS; bitpos: [16]; default: 0;
         *  ch16 enable
         */
        uint32_t etm_ch_ena16:1;
        /** etm_ch_ena17 : R/WTC/WTS; bitpos: [17]; default: 0;
         *  ch17 enable
         */
        uint32_t etm_ch_ena17:1;
        /** etm_ch_ena18 : R/WTC/WTS; bitpos: [18]; default: 0;
         *  ch18 enable
         */
        uint32_t etm_ch_ena18:1;
        /** etm_ch_ena19 : R/WTC/WTS; bitpos: [19]; default: 0;
         *  ch19 enable
         */
        uint32_t etm_ch_ena19:1;
        /** etm_ch_ena20 : R/WTC/WTS; bitpos: [20]; default: 0;
         *  ch20 enable
         */
        uint32_t etm_ch_ena20:1;
        /** etm_ch_ena21 : R/WTC/WTS; bitpos: [21]; default: 0;
         *  ch21 enable
         */
        uint32_t etm_ch_ena21:1;
        /** etm_ch_ena22 : R/WTC/WTS; bitpos: [22]; default: 0;
         *  ch22 enable
         */
        uint32_t etm_ch_ena22:1;
        /** etm_ch_ena23 : R/WTC/WTS; bitpos: [23]; default: 0;
         *  ch23 enable
         */
        uint32_t etm_ch_ena23:1;
        /** etm_ch_ena24 : R/WTC/WTS; bitpos: [24]; default: 0;
         *  ch24 enable
         */
        uint32_t etm_ch_ena24:1;
        /** etm_ch_ena25 : R/WTC/WTS; bitpos: [25]; default: 0;
         *  ch25 enable
         */
        uint32_t etm_ch_ena25:1;
        /** etm_ch_ena26 : R/WTC/WTS; bitpos: [26]; default: 0;
         *  ch26 enable
         */
        uint32_t etm_ch_ena26:1;
        /** etm_ch_ena27 : R/WTC/WTS; bitpos: [27]; default: 0;
         *  ch27 enable
         */
        uint32_t etm_ch_ena27:1;
        /** etm_ch_ena28 : R/WTC/WTS; bitpos: [28]; default: 0;
         *  ch28 enable
         */
        uint32_t etm_ch_ena28:1;
        /** etm_ch_ena29 : R/WTC/WTS; bitpos: [29]; default: 0;
         *  ch29 enable
         */
        uint32_t etm_ch_ena29:1;
        /** etm_ch_ena30 : R/WTC/WTS; bitpos: [30]; default: 0;
         *  ch30 enable
         */
        uint32_t etm_ch_ena30:1;
        /** etm_ch_ena31 : R/WTC/WTS; bitpos: [31]; default: 0;
         *  ch31 enable
         */
        uint32_t etm_ch_ena31:1;
    };
    uint32_t val;
} soc_etm_ch_ena_ad0_reg_t;

/** Type of etm_ch_ena_ad0_set register
 *  channel enable set register
 */
typedef union {
    struct {
        /** etm_ch_set0 : WT; bitpos: [0]; default: 0;
         *  ch0 set
         */
        uint32_t etm_ch_set0:1;
        /** etm_ch_set1 : WT; bitpos: [1]; default: 0;
         *  ch1 set
         */
        uint32_t etm_ch_set1:1;
        /** etm_ch_set2 : WT; bitpos: [2]; default: 0;
         *  ch2 set
         */
        uint32_t etm_ch_set2:1;
        /** etm_ch_set3 : WT; bitpos: [3]; default: 0;
         *  ch3 set
         */
        uint32_t etm_ch_set3:1;
        /** etm_ch_set4 : WT; bitpos: [4]; default: 0;
         *  ch4 set
         */
        uint32_t etm_ch_set4:1;
        /** etm_ch_set5 : WT; bitpos: [5]; default: 0;
         *  ch5 set
         */
        uint32_t etm_ch_set5:1;
        /** etm_ch_set6 : WT; bitpos: [6]; default: 0;
         *  ch6 set
         */
        uint32_t etm_ch_set6:1;
        /** etm_ch_set7 : WT; bitpos: [7]; default: 0;
         *  ch7 set
         */
        uint32_t etm_ch_set7:1;
        /** etm_ch_set8 : WT; bitpos: [8]; default: 0;
         *  ch8 set
         */
        uint32_t etm_ch_set8:1;
        /** etm_ch_set9 : WT; bitpos: [9]; default: 0;
         *  ch9 set
         */
        uint32_t etm_ch_set9:1;
        /** etm_ch_set10 : WT; bitpos: [10]; default: 0;
         *  ch10 set
         */
        uint32_t etm_ch_set10:1;
        /** etm_ch_set11 : WT; bitpos: [11]; default: 0;
         *  ch11 set
         */
        uint32_t etm_ch_set11:1;
        /** etm_ch_set12 : WT; bitpos: [12]; default: 0;
         *  ch12 set
         */
        uint32_t etm_ch_set12:1;
        /** etm_ch_set13 : WT; bitpos: [13]; default: 0;
         *  ch13 set
         */
        uint32_t etm_ch_set13:1;
        /** etm_ch_set14 : WT; bitpos: [14]; default: 0;
         *  ch14 set
         */
        uint32_t etm_ch_set14:1;
        /** etm_ch_set15 : WT; bitpos: [15]; default: 0;
         *  ch15 set
         */
        uint32_t etm_ch_set15:1;
        /** etm_ch_set16 : WT; bitpos: [16]; default: 0;
         *  ch16 set
         */
        uint32_t etm_ch_set16:1;
        /** etm_ch_set17 : WT; bitpos: [17]; default: 0;
         *  ch17 set
         */
        uint32_t etm_ch_set17:1;
        /** etm_ch_set18 : WT; bitpos: [18]; default: 0;
         *  ch18 set
         */
        uint32_t etm_ch_set18:1;
        /** etm_ch_set19 : WT; bitpos: [19]; default: 0;
         *  ch19 set
         */
        uint32_t etm_ch_set19:1;
        /** etm_ch_set20 : WT; bitpos: [20]; default: 0;
         *  ch20 set
         */
        uint32_t etm_ch_set20:1;
        /** etm_ch_set21 : WT; bitpos: [21]; default: 0;
         *  ch21 set
         */
        uint32_t etm_ch_set21:1;
        /** etm_ch_set22 : WT; bitpos: [22]; default: 0;
         *  ch22 set
         */
        uint32_t etm_ch_set22:1;
        /** etm_ch_set23 : WT; bitpos: [23]; default: 0;
         *  ch23 set
         */
        uint32_t etm_ch_set23:1;
        /** etm_ch_set24 : WT; bitpos: [24]; default: 0;
         *  ch24 set
         */
        uint32_t etm_ch_set24:1;
        /** etm_ch_set25 : WT; bitpos: [25]; default: 0;
         *  ch25 set
         */
        uint32_t etm_ch_set25:1;
        /** etm_ch_set26 : WT; bitpos: [26]; default: 0;
         *  ch26 set
         */
        uint32_t etm_ch_set26:1;
        /** etm_ch_set27 : WT; bitpos: [27]; default: 0;
         *  ch27 set
         */
        uint32_t etm_ch_set27:1;
        /** etm_ch_set28 : WT; bitpos: [28]; default: 0;
         *  ch28 set
         */
        uint32_t etm_ch_set28:1;
        /** etm_ch_set29 : WT; bitpos: [29]; default: 0;
         *  ch29 set
         */
        uint32_t etm_ch_set29:1;
        /** etm_ch_set30 : WT; bitpos: [30]; default: 0;
         *  ch30 set
         */
        uint32_t etm_ch_set30:1;
        /** etm_ch_set31 : WT; bitpos: [31]; default: 0;
         *  ch31 set
         */
        uint32_t etm_ch_set31:1;
    };
    uint32_t val;
} soc_etm_ch_ena_ad0_set_reg_t;

/** Type of etm_ch_ena_ad0_clr register
 *  channel enable clear register
 */
typedef union {
    struct {
        /** etm_ch_clr0 : WT; bitpos: [0]; default: 0;
         *  ch0 clear
         */
        uint32_t etm_ch_clr0:1;
        /** etm_ch_clr1 : WT; bitpos: [1]; default: 0;
         *  ch1 clear
         */
        uint32_t etm_ch_clr1:1;
        /** etm_ch_clr2 : WT; bitpos: [2]; default: 0;
         *  ch2 clear
         */
        uint32_t etm_ch_clr2:1;
        /** etm_ch_clr3 : WT; bitpos: [3]; default: 0;
         *  ch3 clear
         */
        uint32_t etm_ch_clr3:1;
        /** etm_ch_clr4 : WT; bitpos: [4]; default: 0;
         *  ch4 clear
         */
        uint32_t etm_ch_clr4:1;
        /** etm_ch_clr5 : WT; bitpos: [5]; default: 0;
         *  ch5 clear
         */
        uint32_t etm_ch_clr5:1;
        /** etm_ch_clr6 : WT; bitpos: [6]; default: 0;
         *  ch6 clear
         */
        uint32_t etm_ch_clr6:1;
        /** etm_ch_clr7 : WT; bitpos: [7]; default: 0;
         *  ch7 clear
         */
        uint32_t etm_ch_clr7:1;
        /** etm_ch_clr8 : WT; bitpos: [8]; default: 0;
         *  ch8 clear
         */
        uint32_t etm_ch_clr8:1;
        /** etm_ch_clr9 : WT; bitpos: [9]; default: 0;
         *  ch9 clear
         */
        uint32_t etm_ch_clr9:1;
        /** etm_ch_clr10 : WT; bitpos: [10]; default: 0;
         *  ch10 clear
         */
        uint32_t etm_ch_clr10:1;
        /** etm_ch_clr11 : WT; bitpos: [11]; default: 0;
         *  ch11 clear
         */
        uint32_t etm_ch_clr11:1;
        /** etm_ch_clr12 : WT; bitpos: [12]; default: 0;
         *  ch12 clear
         */
        uint32_t etm_ch_clr12:1;
        /** etm_ch_clr13 : WT; bitpos: [13]; default: 0;
         *  ch13 clear
         */
        uint32_t etm_ch_clr13:1;
        /** etm_ch_clr14 : WT; bitpos: [14]; default: 0;
         *  ch14 clear
         */
        uint32_t etm_ch_clr14:1;
        /** etm_ch_clr15 : WT; bitpos: [15]; default: 0;
         *  ch15 clear
         */
        uint32_t etm_ch_clr15:1;
        /** etm_ch_clr16 : WT; bitpos: [16]; default: 0;
         *  ch16 clear
         */
        uint32_t etm_ch_clr16:1;
        /** etm_ch_clr17 : WT; bitpos: [17]; default: 0;
         *  ch17 clear
         */
        uint32_t etm_ch_clr17:1;
        /** etm_ch_clr18 : WT; bitpos: [18]; default: 0;
         *  ch18 clear
         */
        uint32_t etm_ch_clr18:1;
        /** etm_ch_clr19 : WT; bitpos: [19]; default: 0;
         *  ch19 clear
         */
        uint32_t etm_ch_clr19:1;
        /** etm_ch_clr20 : WT; bitpos: [20]; default: 0;
         *  ch20 clear
         */
        uint32_t etm_ch_clr20:1;
        /** etm_ch_clr21 : WT; bitpos: [21]; default: 0;
         *  ch21 clear
         */
        uint32_t etm_ch_clr21:1;
        /** etm_ch_clr22 : WT; bitpos: [22]; default: 0;
         *  ch22 clear
         */
        uint32_t etm_ch_clr22:1;
        /** etm_ch_clr23 : WT; bitpos: [23]; default: 0;
         *  ch23 clear
         */
        uint32_t etm_ch_clr23:1;
        /** etm_ch_clr24 : WT; bitpos: [24]; default: 0;
         *  ch24 clear
         */
        uint32_t etm_ch_clr24:1;
        /** etm_ch_clr25 : WT; bitpos: [25]; default: 0;
         *  ch25 clear
         */
        uint32_t etm_ch_clr25:1;
        /** etm_ch_clr26 : WT; bitpos: [26]; default: 0;
         *  ch26 clear
         */
        uint32_t etm_ch_clr26:1;
        /** etm_ch_clr27 : WT; bitpos: [27]; default: 0;
         *  ch27 clear
         */
        uint32_t etm_ch_clr27:1;
        /** etm_ch_clr28 : WT; bitpos: [28]; default: 0;
         *  ch28 clear
         */
        uint32_t etm_ch_clr28:1;
        /** etm_ch_clr29 : WT; bitpos: [29]; default: 0;
         *  ch29 clear
         */
        uint32_t etm_ch_clr29:1;
        /** etm_ch_clr30 : WT; bitpos: [30]; default: 0;
         *  ch30 clear
         */
        uint32_t etm_ch_clr30:1;
        /** etm_ch_clr31 : WT; bitpos: [31]; default: 0;
         *  ch31 clear
         */
        uint32_t etm_ch_clr31:1;
    };
    uint32_t val;
} soc_etm_ch_ena_ad0_clr_reg_t;

/** Type of etm_ch_ena_ad1 register
 *  channel enable register
 */
typedef union {
    struct {
        /** etm_ch_ena32 : R/WTC/WTS; bitpos: [0]; default: 0;
         *  ch32 enable
         */
        uint32_t etm_ch_ena32:1;
        /** etm_ch_ena33 : R/WTC/WTS; bitpos: [1]; default: 0;
         *  ch33 enable
         */
        uint32_t etm_ch_ena33:1;
        /** etm_ch_ena34 : R/WTC/WTS; bitpos: [2]; default: 0;
         *  ch34 enable
         */
        uint32_t etm_ch_ena34:1;
        /** etm_ch_ena35 : R/WTC/WTS; bitpos: [3]; default: 0;
         *  ch35 enable
         */
        uint32_t etm_ch_ena35:1;
        /** etm_ch_ena36 : R/WTC/WTS; bitpos: [4]; default: 0;
         *  ch36 enable
         */
        uint32_t etm_ch_ena36:1;
        /** etm_ch_ena37 : R/WTC/WTS; bitpos: [5]; default: 0;
         *  ch37 enable
         */
        uint32_t etm_ch_ena37:1;
        /** etm_ch_ena38 : R/WTC/WTS; bitpos: [6]; default: 0;
         *  ch38 enable
         */
        uint32_t etm_ch_ena38:1;
        /** etm_ch_ena39 : R/WTC/WTS; bitpos: [7]; default: 0;
         *  ch39 enable
         */
        uint32_t etm_ch_ena39:1;
        /** etm_ch_ena40 : R/WTC/WTS; bitpos: [8]; default: 0;
         *  ch40 enable
         */
        uint32_t etm_ch_ena40:1;
        /** etm_ch_ena41 : R/WTC/WTS; bitpos: [9]; default: 0;
         *  ch41 enable
         */
        uint32_t etm_ch_ena41:1;
        /** etm_ch_ena42 : R/WTC/WTS; bitpos: [10]; default: 0;
         *  ch42 enable
         */
        uint32_t etm_ch_ena42:1;
        /** etm_ch_ena43 : R/WTC/WTS; bitpos: [11]; default: 0;
         *  ch43 enable
         */
        uint32_t etm_ch_ena43:1;
        /** etm_ch_ena44 : R/WTC/WTS; bitpos: [12]; default: 0;
         *  ch44 enable
         */
        uint32_t etm_ch_ena44:1;
        /** etm_ch_ena45 : R/WTC/WTS; bitpos: [13]; default: 0;
         *  ch45 enable
         */
        uint32_t etm_ch_ena45:1;
        /** etm_ch_ena46 : R/WTC/WTS; bitpos: [14]; default: 0;
         *  ch46 enable
         */
        uint32_t etm_ch_ena46:1;
        /** etm_ch_ena47 : R/WTC/WTS; bitpos: [15]; default: 0;
         *  ch47 enable
         */
        uint32_t etm_ch_ena47:1;
        /** etm_ch_ena48 : R/WTC/WTS; bitpos: [16]; default: 0;
         *  ch48 enable
         */
        uint32_t etm_ch_ena48:1;
        /** etm_ch_ena49 : R/WTC/WTS; bitpos: [17]; default: 0;
         *  ch49 enable
         */
        uint32_t etm_ch_ena49:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} soc_etm_ch_ena_ad1_reg_t;

/** Type of etm_ch_ena_ad1_set register
 *  channel enable set register
 */
typedef union {
    struct {
        /** etm_ch_set32 : WT; bitpos: [0]; default: 0;
         *  ch32 set
         */
        uint32_t etm_ch_set32:1;
        /** etm_ch_set33 : WT; bitpos: [1]; default: 0;
         *  ch33 set
         */
        uint32_t etm_ch_set33:1;
        /** etm_ch_set34 : WT; bitpos: [2]; default: 0;
         *  ch34 set
         */
        uint32_t etm_ch_set34:1;
        /** etm_ch_set35 : WT; bitpos: [3]; default: 0;
         *  ch35 set
         */
        uint32_t etm_ch_set35:1;
        /** etm_ch_set36 : WT; bitpos: [4]; default: 0;
         *  ch36 set
         */
        uint32_t etm_ch_set36:1;
        /** etm_ch_set37 : WT; bitpos: [5]; default: 0;
         *  ch37 set
         */
        uint32_t etm_ch_set37:1;
        /** etm_ch_set38 : WT; bitpos: [6]; default: 0;
         *  ch38 set
         */
        uint32_t etm_ch_set38:1;
        /** etm_ch_set39 : WT; bitpos: [7]; default: 0;
         *  ch39 set
         */
        uint32_t etm_ch_set39:1;
        /** etm_ch_set40 : WT; bitpos: [8]; default: 0;
         *  ch40 set
         */
        uint32_t etm_ch_set40:1;
        /** etm_ch_set41 : WT; bitpos: [9]; default: 0;
         *  ch41 set
         */
        uint32_t etm_ch_set41:1;
        /** etm_ch_set42 : WT; bitpos: [10]; default: 0;
         *  ch42 set
         */
        uint32_t etm_ch_set42:1;
        /** etm_ch_set43 : WT; bitpos: [11]; default: 0;
         *  ch43 set
         */
        uint32_t etm_ch_set43:1;
        /** etm_ch_set44 : WT; bitpos: [12]; default: 0;
         *  ch44 set
         */
        uint32_t etm_ch_set44:1;
        /** etm_ch_set45 : WT; bitpos: [13]; default: 0;
         *  ch45 set
         */
        uint32_t etm_ch_set45:1;
        /** etm_ch_set46 : WT; bitpos: [14]; default: 0;
         *  ch46 set
         */
        uint32_t etm_ch_set46:1;
        /** etm_ch_set47 : WT; bitpos: [15]; default: 0;
         *  ch47 set
         */
        uint32_t etm_ch_set47:1;
        /** etm_ch_set48 : WT; bitpos: [16]; default: 0;
         *  ch48 set
         */
        uint32_t etm_ch_set48:1;
        /** etm_ch_set49 : WT; bitpos: [17]; default: 0;
         *  ch49 set
         */
        uint32_t etm_ch_set49:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} soc_etm_ch_ena_ad1_set_reg_t;

/** Type of etm_ch_ena_ad1_clr register
 *  channel enable clear register
 */
typedef union {
    struct {
        /** etm_ch_clr32 : WT; bitpos: [0]; default: 0;
         *  ch32 clear
         */
        uint32_t etm_ch_clr32:1;
        /** etm_ch_clr33 : WT; bitpos: [1]; default: 0;
         *  ch33 clear
         */
        uint32_t etm_ch_clr33:1;
        /** etm_ch_clr34 : WT; bitpos: [2]; default: 0;
         *  ch34 clear
         */
        uint32_t etm_ch_clr34:1;
        /** etm_ch_clr35 : WT; bitpos: [3]; default: 0;
         *  ch35 clear
         */
        uint32_t etm_ch_clr35:1;
        /** etm_ch_clr36 : WT; bitpos: [4]; default: 0;
         *  ch36 clear
         */
        uint32_t etm_ch_clr36:1;
        /** etm_ch_clr37 : WT; bitpos: [5]; default: 0;
         *  ch37 clear
         */
        uint32_t etm_ch_clr37:1;
        /** etm_ch_clr38 : WT; bitpos: [6]; default: 0;
         *  ch38 clear
         */
        uint32_t etm_ch_clr38:1;
        /** etm_ch_clr39 : WT; bitpos: [7]; default: 0;
         *  ch39 clear
         */
        uint32_t etm_ch_clr39:1;
        /** etm_ch_clr40 : WT; bitpos: [8]; default: 0;
         *  ch40 clear
         */
        uint32_t etm_ch_clr40:1;
        /** etm_ch_clr41 : WT; bitpos: [9]; default: 0;
         *  ch41 clear
         */
        uint32_t etm_ch_clr41:1;
        /** etm_ch_clr42 : WT; bitpos: [10]; default: 0;
         *  ch42 clear
         */
        uint32_t etm_ch_clr42:1;
        /** etm_ch_clr43 : WT; bitpos: [11]; default: 0;
         *  ch43 clear
         */
        uint32_t etm_ch_clr43:1;
        /** etm_ch_clr44 : WT; bitpos: [12]; default: 0;
         *  ch44 clear
         */
        uint32_t etm_ch_clr44:1;
        /** etm_ch_clr45 : WT; bitpos: [13]; default: 0;
         *  ch45 clear
         */
        uint32_t etm_ch_clr45:1;
        /** etm_ch_clr46 : WT; bitpos: [14]; default: 0;
         *  ch46 clear
         */
        uint32_t etm_ch_clr46:1;
        /** etm_ch_clr47 : WT; bitpos: [15]; default: 0;
         *  ch47 clear
         */
        uint32_t etm_ch_clr47:1;
        /** etm_ch_clr48 : WT; bitpos: [16]; default: 0;
         *  ch48 clear
         */
        uint32_t etm_ch_clr48:1;
        /** etm_ch_clr49 : WT; bitpos: [17]; default: 0;
         *  ch49 clear
         */
        uint32_t etm_ch_clr49:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} soc_etm_ch_ena_ad1_clr_reg_t;

/** Type of etm_ch0_evt_id register
 *  channel0 event id register
 */
typedef union {
    struct {
        /** etm_ch0_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch0_evt_id
         */
        uint32_t etm_ch0_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch0_evt_id_reg_t;

/** Type of etm_ch0_task_id register
 *  channel0 task id register
 */
typedef union {
    struct {
        /** etm_ch0_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch0_task_id
         */
        uint32_t etm_ch0_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch0_task_id_reg_t;

/** Type of etm_ch1_evt_id register
 *  channel1 event id register
 */
typedef union {
    struct {
        /** etm_ch1_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch1_evt_id
         */
        uint32_t etm_ch1_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch1_evt_id_reg_t;

/** Type of etm_ch1_task_id register
 *  channel1 task id register
 */
typedef union {
    struct {
        /** etm_ch1_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch1_task_id
         */
        uint32_t etm_ch1_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch1_task_id_reg_t;

/** Type of etm_ch2_evt_id register
 *  channel2 event id register
 */
typedef union {
    struct {
        /** etm_ch2_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch2_evt_id
         */
        uint32_t etm_ch2_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch2_evt_id_reg_t;

/** Type of etm_ch2_task_id register
 *  channel2 task id register
 */
typedef union {
    struct {
        /** etm_ch2_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch2_task_id
         */
        uint32_t etm_ch2_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch2_task_id_reg_t;

/** Type of etm_ch3_evt_id register
 *  channel3 event id register
 */
typedef union {
    struct {
        /** etm_ch3_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch3_evt_id
         */
        uint32_t etm_ch3_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch3_evt_id_reg_t;

/** Type of etm_ch3_task_id register
 *  channel3 task id register
 */
typedef union {
    struct {
        /** etm_ch3_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch3_task_id
         */
        uint32_t etm_ch3_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch3_task_id_reg_t;

/** Type of etm_ch4_evt_id register
 *  channel4 event id register
 */
typedef union {
    struct {
        /** etm_ch4_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch4_evt_id
         */
        uint32_t etm_ch4_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch4_evt_id_reg_t;

/** Type of etm_ch4_task_id register
 *  channel4 task id register
 */
typedef union {
    struct {
        /** etm_ch4_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch4_task_id
         */
        uint32_t etm_ch4_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch4_task_id_reg_t;

/** Type of etm_ch5_evt_id register
 *  channel5 event id register
 */
typedef union {
    struct {
        /** etm_ch5_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch5_evt_id
         */
        uint32_t etm_ch5_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch5_evt_id_reg_t;

/** Type of etm_ch5_task_id register
 *  channel5 task id register
 */
typedef union {
    struct {
        /** etm_ch5_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch5_task_id
         */
        uint32_t etm_ch5_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch5_task_id_reg_t;

/** Type of etm_ch6_evt_id register
 *  channel6 event id register
 */
typedef union {
    struct {
        /** etm_ch6_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch6_evt_id
         */
        uint32_t etm_ch6_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch6_evt_id_reg_t;

/** Type of etm_ch6_task_id register
 *  channel6 task id register
 */
typedef union {
    struct {
        /** etm_ch6_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch6_task_id
         */
        uint32_t etm_ch6_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch6_task_id_reg_t;

/** Type of etm_ch7_evt_id register
 *  channel7 event id register
 */
typedef union {
    struct {
        /** etm_ch7_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch7_evt_id
         */
        uint32_t etm_ch7_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch7_evt_id_reg_t;

/** Type of etm_ch7_task_id register
 *  channel7 task id register
 */
typedef union {
    struct {
        /** etm_ch7_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch7_task_id
         */
        uint32_t etm_ch7_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch7_task_id_reg_t;

/** Type of etm_ch8_evt_id register
 *  channel8 event id register
 */
typedef union {
    struct {
        /** etm_ch8_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch8_evt_id
         */
        uint32_t etm_ch8_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch8_evt_id_reg_t;

/** Type of etm_ch8_task_id register
 *  channel8 task id register
 */
typedef union {
    struct {
        /** etm_ch8_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch8_task_id
         */
        uint32_t etm_ch8_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch8_task_id_reg_t;

/** Type of etm_ch9_evt_id register
 *  channel9 event id register
 */
typedef union {
    struct {
        /** etm_ch9_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch9_evt_id
         */
        uint32_t etm_ch9_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch9_evt_id_reg_t;

/** Type of etm_ch9_task_id register
 *  channel9 task id register
 */
typedef union {
    struct {
        /** etm_ch9_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch9_task_id
         */
        uint32_t etm_ch9_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch9_task_id_reg_t;

/** Type of etm_ch10_evt_id register
 *  channel10 event id register
 */
typedef union {
    struct {
        /** etm_ch10_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch10_evt_id
         */
        uint32_t etm_ch10_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch10_evt_id_reg_t;

/** Type of etm_ch10_task_id register
 *  channel10 task id register
 */
typedef union {
    struct {
        /** etm_ch10_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch10_task_id
         */
        uint32_t etm_ch10_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch10_task_id_reg_t;

/** Type of etm_ch11_evt_id register
 *  channel11 event id register
 */
typedef union {
    struct {
        /** etm_ch11_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch11_evt_id
         */
        uint32_t etm_ch11_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch11_evt_id_reg_t;

/** Type of etm_ch11_task_id register
 *  channel11 task id register
 */
typedef union {
    struct {
        /** etm_ch11_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch11_task_id
         */
        uint32_t etm_ch11_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch11_task_id_reg_t;

/** Type of etm_ch12_evt_id register
 *  channel12 event id register
 */
typedef union {
    struct {
        /** etm_ch12_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch12_evt_id
         */
        uint32_t etm_ch12_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch12_evt_id_reg_t;

/** Type of etm_ch12_task_id register
 *  channel12 task id register
 */
typedef union {
    struct {
        /** etm_ch12_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch12_task_id
         */
        uint32_t etm_ch12_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch12_task_id_reg_t;

/** Type of etm_ch13_evt_id register
 *  channel13 event id register
 */
typedef union {
    struct {
        /** etm_ch13_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch13_evt_id
         */
        uint32_t etm_ch13_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch13_evt_id_reg_t;

/** Type of etm_ch13_task_id register
 *  channel13 task id register
 */
typedef union {
    struct {
        /** etm_ch13_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch13_task_id
         */
        uint32_t etm_ch13_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch13_task_id_reg_t;

/** Type of etm_ch14_evt_id register
 *  channel14 event id register
 */
typedef union {
    struct {
        /** etm_ch14_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch14_evt_id
         */
        uint32_t etm_ch14_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch14_evt_id_reg_t;

/** Type of etm_ch14_task_id register
 *  channel14 task id register
 */
typedef union {
    struct {
        /** etm_ch14_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch14_task_id
         */
        uint32_t etm_ch14_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch14_task_id_reg_t;

/** Type of etm_ch15_evt_id register
 *  channel15 event id register
 */
typedef union {
    struct {
        /** etm_ch15_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch15_evt_id
         */
        uint32_t etm_ch15_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch15_evt_id_reg_t;

/** Type of etm_ch15_task_id register
 *  channel15 task id register
 */
typedef union {
    struct {
        /** etm_ch15_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch15_task_id
         */
        uint32_t etm_ch15_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch15_task_id_reg_t;

/** Type of etm_ch16_evt_id register
 *  channel16 event id register
 */
typedef union {
    struct {
        /** etm_ch16_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch16_evt_id
         */
        uint32_t etm_ch16_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch16_evt_id_reg_t;

/** Type of etm_ch16_task_id register
 *  channel16 task id register
 */
typedef union {
    struct {
        /** etm_ch16_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch16_task_id
         */
        uint32_t etm_ch16_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch16_task_id_reg_t;

/** Type of etm_ch17_evt_id register
 *  channel17 event id register
 */
typedef union {
    struct {
        /** etm_ch17_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch17_evt_id
         */
        uint32_t etm_ch17_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch17_evt_id_reg_t;

/** Type of etm_ch17_task_id register
 *  channel17 task id register
 */
typedef union {
    struct {
        /** etm_ch17_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch17_task_id
         */
        uint32_t etm_ch17_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch17_task_id_reg_t;

/** Type of etm_ch18_evt_id register
 *  channel18 event id register
 */
typedef union {
    struct {
        /** etm_ch18_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch18_evt_id
         */
        uint32_t etm_ch18_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch18_evt_id_reg_t;

/** Type of etm_ch18_task_id register
 *  channel18 task id register
 */
typedef union {
    struct {
        /** etm_ch18_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch18_task_id
         */
        uint32_t etm_ch18_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch18_task_id_reg_t;

/** Type of etm_ch19_evt_id register
 *  channel19 event id register
 */
typedef union {
    struct {
        /** etm_ch19_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch19_evt_id
         */
        uint32_t etm_ch19_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch19_evt_id_reg_t;

/** Type of etm_ch19_task_id register
 *  channel19 task id register
 */
typedef union {
    struct {
        /** etm_ch19_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch19_task_id
         */
        uint32_t etm_ch19_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch19_task_id_reg_t;

/** Type of etm_ch20_evt_id register
 *  channel20 event id register
 */
typedef union {
    struct {
        /** etm_ch20_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch20_evt_id
         */
        uint32_t etm_ch20_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch20_evt_id_reg_t;

/** Type of etm_ch20_task_id register
 *  channel20 task id register
 */
typedef union {
    struct {
        /** etm_ch20_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch20_task_id
         */
        uint32_t etm_ch20_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch20_task_id_reg_t;

/** Type of etm_ch21_evt_id register
 *  channel21 event id register
 */
typedef union {
    struct {
        /** etm_ch21_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch21_evt_id
         */
        uint32_t etm_ch21_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch21_evt_id_reg_t;

/** Type of etm_ch21_task_id register
 *  channel21 task id register
 */
typedef union {
    struct {
        /** etm_ch21_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch21_task_id
         */
        uint32_t etm_ch21_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch21_task_id_reg_t;

/** Type of etm_ch22_evt_id register
 *  channel22 event id register
 */
typedef union {
    struct {
        /** etm_ch22_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch22_evt_id
         */
        uint32_t etm_ch22_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch22_evt_id_reg_t;

/** Type of etm_ch22_task_id register
 *  channel22 task id register
 */
typedef union {
    struct {
        /** etm_ch22_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch22_task_id
         */
        uint32_t etm_ch22_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch22_task_id_reg_t;

/** Type of etm_ch23_evt_id register
 *  channel23 event id register
 */
typedef union {
    struct {
        /** etm_ch23_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch23_evt_id
         */
        uint32_t etm_ch23_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch23_evt_id_reg_t;

/** Type of etm_ch23_task_id register
 *  channel23 task id register
 */
typedef union {
    struct {
        /** etm_ch23_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch23_task_id
         */
        uint32_t etm_ch23_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch23_task_id_reg_t;

/** Type of etm_ch24_evt_id register
 *  channel24 event id register
 */
typedef union {
    struct {
        /** etm_ch24_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch24_evt_id
         */
        uint32_t etm_ch24_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch24_evt_id_reg_t;

/** Type of etm_ch24_task_id register
 *  channel24 task id register
 */
typedef union {
    struct {
        /** etm_ch24_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch24_task_id
         */
        uint32_t etm_ch24_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch24_task_id_reg_t;

/** Type of etm_ch25_evt_id register
 *  channel25 event id register
 */
typedef union {
    struct {
        /** etm_ch25_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch25_evt_id
         */
        uint32_t etm_ch25_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch25_evt_id_reg_t;

/** Type of etm_ch25_task_id register
 *  channel25 task id register
 */
typedef union {
    struct {
        /** etm_ch25_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch25_task_id
         */
        uint32_t etm_ch25_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch25_task_id_reg_t;

/** Type of etm_ch26_evt_id register
 *  channel26 event id register
 */
typedef union {
    struct {
        /** etm_ch26_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch26_evt_id
         */
        uint32_t etm_ch26_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch26_evt_id_reg_t;

/** Type of etm_ch26_task_id register
 *  channel26 task id register
 */
typedef union {
    struct {
        /** etm_ch26_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch26_task_id
         */
        uint32_t etm_ch26_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch26_task_id_reg_t;

/** Type of etm_ch27_evt_id register
 *  channel27 event id register
 */
typedef union {
    struct {
        /** etm_ch27_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch27_evt_id
         */
        uint32_t etm_ch27_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch27_evt_id_reg_t;

/** Type of etm_ch27_task_id register
 *  channel27 task id register
 */
typedef union {
    struct {
        /** etm_ch27_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch27_task_id
         */
        uint32_t etm_ch27_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch27_task_id_reg_t;

/** Type of etm_ch28_evt_id register
 *  channel28 event id register
 */
typedef union {
    struct {
        /** etm_ch28_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch28_evt_id
         */
        uint32_t etm_ch28_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch28_evt_id_reg_t;

/** Type of etm_ch28_task_id register
 *  channel28 task id register
 */
typedef union {
    struct {
        /** etm_ch28_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch28_task_id
         */
        uint32_t etm_ch28_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch28_task_id_reg_t;

/** Type of etm_ch29_evt_id register
 *  channel29 event id register
 */
typedef union {
    struct {
        /** etm_ch29_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch29_evt_id
         */
        uint32_t etm_ch29_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch29_evt_id_reg_t;

/** Type of etm_ch29_task_id register
 *  channel29 task id register
 */
typedef union {
    struct {
        /** etm_ch29_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch29_task_id
         */
        uint32_t etm_ch29_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch29_task_id_reg_t;

/** Type of etm_ch30_evt_id register
 *  channel30 event id register
 */
typedef union {
    struct {
        /** etm_ch30_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch30_evt_id
         */
        uint32_t etm_ch30_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch30_evt_id_reg_t;

/** Type of etm_ch30_task_id register
 *  channel30 task id register
 */
typedef union {
    struct {
        /** etm_ch30_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch30_task_id
         */
        uint32_t etm_ch30_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch30_task_id_reg_t;

/** Type of etm_ch31_evt_id register
 *  channel31 event id register
 */
typedef union {
    struct {
        /** etm_ch31_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch31_evt_id
         */
        uint32_t etm_ch31_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch31_evt_id_reg_t;

/** Type of etm_ch31_task_id register
 *  channel31 task id register
 */
typedef union {
    struct {
        /** etm_ch31_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch31_task_id
         */
        uint32_t etm_ch31_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch31_task_id_reg_t;

/** Type of etm_ch32_evt_id register
 *  channel32 event id register
 */
typedef union {
    struct {
        /** etm_ch32_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch32_evt_id
         */
        uint32_t etm_ch32_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch32_evt_id_reg_t;

/** Type of etm_ch32_task_id register
 *  channel32 task id register
 */
typedef union {
    struct {
        /** etm_ch32_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch32_task_id
         */
        uint32_t etm_ch32_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch32_task_id_reg_t;

/** Type of etm_ch33_evt_id register
 *  channel33 event id register
 */
typedef union {
    struct {
        /** etm_ch33_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch33_evt_id
         */
        uint32_t etm_ch33_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch33_evt_id_reg_t;

/** Type of etm_ch33_task_id register
 *  channel33 task id register
 */
typedef union {
    struct {
        /** etm_ch33_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch33_task_id
         */
        uint32_t etm_ch33_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch33_task_id_reg_t;

/** Type of etm_ch34_evt_id register
 *  channel34 event id register
 */
typedef union {
    struct {
        /** etm_ch34_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch34_evt_id
         */
        uint32_t etm_ch34_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch34_evt_id_reg_t;

/** Type of etm_ch34_task_id register
 *  channel34 task id register
 */
typedef union {
    struct {
        /** etm_ch34_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch34_task_id
         */
        uint32_t etm_ch34_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch34_task_id_reg_t;

/** Type of etm_ch35_evt_id register
 *  channel35 event id register
 */
typedef union {
    struct {
        /** etm_ch35_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch35_evt_id
         */
        uint32_t etm_ch35_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch35_evt_id_reg_t;

/** Type of etm_ch35_task_id register
 *  channel35 task id register
 */
typedef union {
    struct {
        /** etm_ch35_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch35_task_id
         */
        uint32_t etm_ch35_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch35_task_id_reg_t;

/** Type of etm_ch36_evt_id register
 *  channel36 event id register
 */
typedef union {
    struct {
        /** etm_ch36_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch36_evt_id
         */
        uint32_t etm_ch36_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch36_evt_id_reg_t;

/** Type of etm_ch36_task_id register
 *  channel36 task id register
 */
typedef union {
    struct {
        /** etm_ch36_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch36_task_id
         */
        uint32_t etm_ch36_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch36_task_id_reg_t;

/** Type of etm_ch37_evt_id register
 *  channel37 event id register
 */
typedef union {
    struct {
        /** etm_ch37_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch37_evt_id
         */
        uint32_t etm_ch37_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch37_evt_id_reg_t;

/** Type of etm_ch37_task_id register
 *  channel37 task id register
 */
typedef union {
    struct {
        /** etm_ch37_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch37_task_id
         */
        uint32_t etm_ch37_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch37_task_id_reg_t;

/** Type of etm_ch38_evt_id register
 *  channel38 event id register
 */
typedef union {
    struct {
        /** etm_ch38_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch38_evt_id
         */
        uint32_t etm_ch38_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch38_evt_id_reg_t;

/** Type of etm_ch38_task_id register
 *  channel38 task id register
 */
typedef union {
    struct {
        /** etm_ch38_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch38_task_id
         */
        uint32_t etm_ch38_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch38_task_id_reg_t;

/** Type of etm_ch39_evt_id register
 *  channel39 event id register
 */
typedef union {
    struct {
        /** etm_ch39_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch39_evt_id
         */
        uint32_t etm_ch39_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch39_evt_id_reg_t;

/** Type of etm_ch39_task_id register
 *  channel39 task id register
 */
typedef union {
    struct {
        /** etm_ch39_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch39_task_id
         */
        uint32_t etm_ch39_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch39_task_id_reg_t;

/** Type of etm_ch40_evt_id register
 *  channel40 event id register
 */
typedef union {
    struct {
        /** etm_ch40_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch40_evt_id
         */
        uint32_t etm_ch40_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch40_evt_id_reg_t;

/** Type of etm_ch40_task_id register
 *  channel40 task id register
 */
typedef union {
    struct {
        /** etm_ch40_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch40_task_id
         */
        uint32_t etm_ch40_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch40_task_id_reg_t;

/** Type of etm_ch41_evt_id register
 *  channel41 event id register
 */
typedef union {
    struct {
        /** etm_ch41_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch41_evt_id
         */
        uint32_t etm_ch41_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch41_evt_id_reg_t;

/** Type of etm_ch41_task_id register
 *  channel41 task id register
 */
typedef union {
    struct {
        /** etm_ch41_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch41_task_id
         */
        uint32_t etm_ch41_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch41_task_id_reg_t;

/** Type of etm_ch42_evt_id register
 *  channel42 event id register
 */
typedef union {
    struct {
        /** etm_ch42_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch42_evt_id
         */
        uint32_t etm_ch42_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch42_evt_id_reg_t;

/** Type of etm_ch42_task_id register
 *  channel42 task id register
 */
typedef union {
    struct {
        /** etm_ch42_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch42_task_id
         */
        uint32_t etm_ch42_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch42_task_id_reg_t;

/** Type of etm_ch43_evt_id register
 *  channel43 event id register
 */
typedef union {
    struct {
        /** etm_ch43_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch43_evt_id
         */
        uint32_t etm_ch43_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch43_evt_id_reg_t;

/** Type of etm_ch43_task_id register
 *  channel43 task id register
 */
typedef union {
    struct {
        /** etm_ch43_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch43_task_id
         */
        uint32_t etm_ch43_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch43_task_id_reg_t;

/** Type of etm_ch44_evt_id register
 *  channel44 event id register
 */
typedef union {
    struct {
        /** etm_ch44_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch44_evt_id
         */
        uint32_t etm_ch44_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch44_evt_id_reg_t;

/** Type of etm_ch44_task_id register
 *  channel44 task id register
 */
typedef union {
    struct {
        /** etm_ch44_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch44_task_id
         */
        uint32_t etm_ch44_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch44_task_id_reg_t;

/** Type of etm_ch45_evt_id register
 *  channel45 event id register
 */
typedef union {
    struct {
        /** etm_ch45_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch45_evt_id
         */
        uint32_t etm_ch45_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch45_evt_id_reg_t;

/** Type of etm_ch45_task_id register
 *  channel45 task id register
 */
typedef union {
    struct {
        /** etm_ch45_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch45_task_id
         */
        uint32_t etm_ch45_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch45_task_id_reg_t;

/** Type of etm_ch46_evt_id register
 *  channel46 event id register
 */
typedef union {
    struct {
        /** etm_ch46_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch46_evt_id
         */
        uint32_t etm_ch46_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch46_evt_id_reg_t;

/** Type of etm_ch46_task_id register
 *  channel46 task id register
 */
typedef union {
    struct {
        /** etm_ch46_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch46_task_id
         */
        uint32_t etm_ch46_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch46_task_id_reg_t;

/** Type of etm_ch47_evt_id register
 *  channel47 event id register
 */
typedef union {
    struct {
        /** etm_ch47_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch47_evt_id
         */
        uint32_t etm_ch47_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch47_evt_id_reg_t;

/** Type of etm_ch47_task_id register
 *  channel47 task id register
 */
typedef union {
    struct {
        /** etm_ch47_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch47_task_id
         */
        uint32_t etm_ch47_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch47_task_id_reg_t;

/** Type of etm_ch48_evt_id register
 *  channel48 event id register
 */
typedef union {
    struct {
        /** etm_ch48_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch48_evt_id
         */
        uint32_t etm_ch48_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch48_evt_id_reg_t;

/** Type of etm_ch48_task_id register
 *  channel48 task id register
 */
typedef union {
    struct {
        /** etm_ch48_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch48_task_id
         */
        uint32_t etm_ch48_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch48_task_id_reg_t;

/** Type of etm_ch49_evt_id register
 *  channel49 event id register
 */
typedef union {
    struct {
        /** etm_ch49_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  ch49_evt_id
         */
        uint32_t etm_ch49_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch49_evt_id_reg_t;

/** Type of etm_ch49_task_id register
 *  channel49 task id register
 */
typedef union {
    struct {
        /** etm_ch49_task_id : R/W; bitpos: [7:0]; default: 0;
         *  ch49_task_id
         */
        uint32_t etm_ch49_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_ch49_task_id_reg_t;

/** Type of etm_clk_en register
 *  etm clock enable register
 */
typedef union {
    struct {
        /** etm_clk_en : R/W; bitpos: [0]; default: 0;
         *  clock enable
         */
        uint32_t etm_clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} soc_etm_clk_en_reg_t;


/** Group: Version Register */
/** Type of etm_date register
 *  etm date register
 */
typedef union {
    struct {
        /** etm_date : R/W; bitpos: [27:0]; default: 35664018;
         *  date
         */
        uint32_t etm_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} soc_etm_date_reg_t;


typedef struct {
    volatile soc_etm_ch_ena_ad0_reg_t etm_ch_ena_ad0;
    volatile soc_etm_ch_ena_ad0_set_reg_t etm_ch_ena_ad0_set;
    volatile soc_etm_ch_ena_ad0_clr_reg_t etm_ch_ena_ad0_clr;
    volatile soc_etm_ch_ena_ad1_reg_t etm_ch_ena_ad1;
    volatile soc_etm_ch_ena_ad1_set_reg_t etm_ch_ena_ad1_set;
    volatile soc_etm_ch_ena_ad1_clr_reg_t etm_ch_ena_ad1_clr;
    volatile soc_etm_ch0_evt_id_reg_t etm_ch0_evt_id;
    volatile soc_etm_ch0_task_id_reg_t etm_ch0_task_id;
    volatile soc_etm_ch1_evt_id_reg_t etm_ch1_evt_id;
    volatile soc_etm_ch1_task_id_reg_t etm_ch1_task_id;
    volatile soc_etm_ch2_evt_id_reg_t etm_ch2_evt_id;
    volatile soc_etm_ch2_task_id_reg_t etm_ch2_task_id;
    volatile soc_etm_ch3_evt_id_reg_t etm_ch3_evt_id;
    volatile soc_etm_ch3_task_id_reg_t etm_ch3_task_id;
    volatile soc_etm_ch4_evt_id_reg_t etm_ch4_evt_id;
    volatile soc_etm_ch4_task_id_reg_t etm_ch4_task_id;
    volatile soc_etm_ch5_evt_id_reg_t etm_ch5_evt_id;
    volatile soc_etm_ch5_task_id_reg_t etm_ch5_task_id;
    volatile soc_etm_ch6_evt_id_reg_t etm_ch6_evt_id;
    volatile soc_etm_ch6_task_id_reg_t etm_ch6_task_id;
    volatile soc_etm_ch7_evt_id_reg_t etm_ch7_evt_id;
    volatile soc_etm_ch7_task_id_reg_t etm_ch7_task_id;
    volatile soc_etm_ch8_evt_id_reg_t etm_ch8_evt_id;
    volatile soc_etm_ch8_task_id_reg_t etm_ch8_task_id;
    volatile soc_etm_ch9_evt_id_reg_t etm_ch9_evt_id;
    volatile soc_etm_ch9_task_id_reg_t etm_ch9_task_id;
    volatile soc_etm_ch10_evt_id_reg_t etm_ch10_evt_id;
    volatile soc_etm_ch10_task_id_reg_t etm_ch10_task_id;
    volatile soc_etm_ch11_evt_id_reg_t etm_ch11_evt_id;
    volatile soc_etm_ch11_task_id_reg_t etm_ch11_task_id;
    volatile soc_etm_ch12_evt_id_reg_t etm_ch12_evt_id;
    volatile soc_etm_ch12_task_id_reg_t etm_ch12_task_id;
    volatile soc_etm_ch13_evt_id_reg_t etm_ch13_evt_id;
    volatile soc_etm_ch13_task_id_reg_t etm_ch13_task_id;
    volatile soc_etm_ch14_evt_id_reg_t etm_ch14_evt_id;
    volatile soc_etm_ch14_task_id_reg_t etm_ch14_task_id;
    volatile soc_etm_ch15_evt_id_reg_t etm_ch15_evt_id;
    volatile soc_etm_ch15_task_id_reg_t etm_ch15_task_id;
    volatile soc_etm_ch16_evt_id_reg_t etm_ch16_evt_id;
    volatile soc_etm_ch16_task_id_reg_t etm_ch16_task_id;
    volatile soc_etm_ch17_evt_id_reg_t etm_ch17_evt_id;
    volatile soc_etm_ch17_task_id_reg_t etm_ch17_task_id;
    volatile soc_etm_ch18_evt_id_reg_t etm_ch18_evt_id;
    volatile soc_etm_ch18_task_id_reg_t etm_ch18_task_id;
    volatile soc_etm_ch19_evt_id_reg_t etm_ch19_evt_id;
    volatile soc_etm_ch19_task_id_reg_t etm_ch19_task_id;
    volatile soc_etm_ch20_evt_id_reg_t etm_ch20_evt_id;
    volatile soc_etm_ch20_task_id_reg_t etm_ch20_task_id;
    volatile soc_etm_ch21_evt_id_reg_t etm_ch21_evt_id;
    volatile soc_etm_ch21_task_id_reg_t etm_ch21_task_id;
    volatile soc_etm_ch22_evt_id_reg_t etm_ch22_evt_id;
    volatile soc_etm_ch22_task_id_reg_t etm_ch22_task_id;
    volatile soc_etm_ch23_evt_id_reg_t etm_ch23_evt_id;
    volatile soc_etm_ch23_task_id_reg_t etm_ch23_task_id;
    volatile soc_etm_ch24_evt_id_reg_t etm_ch24_evt_id;
    volatile soc_etm_ch24_task_id_reg_t etm_ch24_task_id;
    volatile soc_etm_ch25_evt_id_reg_t etm_ch25_evt_id;
    volatile soc_etm_ch25_task_id_reg_t etm_ch25_task_id;
    volatile soc_etm_ch26_evt_id_reg_t etm_ch26_evt_id;
    volatile soc_etm_ch26_task_id_reg_t etm_ch26_task_id;
    volatile soc_etm_ch27_evt_id_reg_t etm_ch27_evt_id;
    volatile soc_etm_ch27_task_id_reg_t etm_ch27_task_id;
    volatile soc_etm_ch28_evt_id_reg_t etm_ch28_evt_id;
    volatile soc_etm_ch28_task_id_reg_t etm_ch28_task_id;
    volatile soc_etm_ch29_evt_id_reg_t etm_ch29_evt_id;
    volatile soc_etm_ch29_task_id_reg_t etm_ch29_task_id;
    volatile soc_etm_ch30_evt_id_reg_t etm_ch30_evt_id;
    volatile soc_etm_ch30_task_id_reg_t etm_ch30_task_id;
    volatile soc_etm_ch31_evt_id_reg_t etm_ch31_evt_id;
    volatile soc_etm_ch31_task_id_reg_t etm_ch31_task_id;
    volatile soc_etm_ch32_evt_id_reg_t etm_ch32_evt_id;
    volatile soc_etm_ch32_task_id_reg_t etm_ch32_task_id;
    volatile soc_etm_ch33_evt_id_reg_t etm_ch33_evt_id;
    volatile soc_etm_ch33_task_id_reg_t etm_ch33_task_id;
    volatile soc_etm_ch34_evt_id_reg_t etm_ch34_evt_id;
    volatile soc_etm_ch34_task_id_reg_t etm_ch34_task_id;
    volatile soc_etm_ch35_evt_id_reg_t etm_ch35_evt_id;
    volatile soc_etm_ch35_task_id_reg_t etm_ch35_task_id;
    volatile soc_etm_ch36_evt_id_reg_t etm_ch36_evt_id;
    volatile soc_etm_ch36_task_id_reg_t etm_ch36_task_id;
    volatile soc_etm_ch37_evt_id_reg_t etm_ch37_evt_id;
    volatile soc_etm_ch37_task_id_reg_t etm_ch37_task_id;
    volatile soc_etm_ch38_evt_id_reg_t etm_ch38_evt_id;
    volatile soc_etm_ch38_task_id_reg_t etm_ch38_task_id;
    volatile soc_etm_ch39_evt_id_reg_t etm_ch39_evt_id;
    volatile soc_etm_ch39_task_id_reg_t etm_ch39_task_id;
    volatile soc_etm_ch40_evt_id_reg_t etm_ch40_evt_id;
    volatile soc_etm_ch40_task_id_reg_t etm_ch40_task_id;
    volatile soc_etm_ch41_evt_id_reg_t etm_ch41_evt_id;
    volatile soc_etm_ch41_task_id_reg_t etm_ch41_task_id;
    volatile soc_etm_ch42_evt_id_reg_t etm_ch42_evt_id;
    volatile soc_etm_ch42_task_id_reg_t etm_ch42_task_id;
    volatile soc_etm_ch43_evt_id_reg_t etm_ch43_evt_id;
    volatile soc_etm_ch43_task_id_reg_t etm_ch43_task_id;
    volatile soc_etm_ch44_evt_id_reg_t etm_ch44_evt_id;
    volatile soc_etm_ch44_task_id_reg_t etm_ch44_task_id;
    volatile soc_etm_ch45_evt_id_reg_t etm_ch45_evt_id;
    volatile soc_etm_ch45_task_id_reg_t etm_ch45_task_id;
    volatile soc_etm_ch46_evt_id_reg_t etm_ch46_evt_id;
    volatile soc_etm_ch46_task_id_reg_t etm_ch46_task_id;
    volatile soc_etm_ch47_evt_id_reg_t etm_ch47_evt_id;
    volatile soc_etm_ch47_task_id_reg_t etm_ch47_task_id;
    volatile soc_etm_ch48_evt_id_reg_t etm_ch48_evt_id;
    volatile soc_etm_ch48_task_id_reg_t etm_ch48_task_id;
    volatile soc_etm_ch49_evt_id_reg_t etm_ch49_evt_id;
    volatile soc_etm_ch49_task_id_reg_t etm_ch49_task_id;
    volatile soc_etm_clk_en_reg_t etm_clk_en;
    volatile soc_etm_date_reg_t etm_date;
} soc_etm_dev_t;

extern soc_etm_dev_t SOC_ETM;

#ifndef __cplusplus
_Static_assert(sizeof(soc_etm_dev_t) == 0x1b0, "Invalid size of soc_etm_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
