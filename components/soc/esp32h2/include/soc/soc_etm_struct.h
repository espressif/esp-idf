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

/** Group: Configuration Register */
/** Type of ch_ena_ad0 register
 *  channel enable register
 */
typedef union {
    struct {
        /** ch_ena0 : R/WTC/WTS; bitpos: [0]; default: 0;
         *  ch0 enable
         */
        uint32_t ch_ena0:1;
        /** ch_ena1 : R/WTC/WTS; bitpos: [1]; default: 0;
         *  ch1 enable
         */
        uint32_t ch_ena1:1;
        /** ch_ena2 : R/WTC/WTS; bitpos: [2]; default: 0;
         *  ch2 enable
         */
        uint32_t ch_ena2:1;
        /** ch_ena3 : R/WTC/WTS; bitpos: [3]; default: 0;
         *  ch3 enable
         */
        uint32_t ch_ena3:1;
        /** ch_ena4 : R/WTC/WTS; bitpos: [4]; default: 0;
         *  ch4 enable
         */
        uint32_t ch_ena4:1;
        /** ch_ena5 : R/WTC/WTS; bitpos: [5]; default: 0;
         *  ch5 enable
         */
        uint32_t ch_ena5:1;
        /** ch_ena6 : R/WTC/WTS; bitpos: [6]; default: 0;
         *  ch6 enable
         */
        uint32_t ch_ena6:1;
        /** ch_ena7 : R/WTC/WTS; bitpos: [7]; default: 0;
         *  ch7 enable
         */
        uint32_t ch_ena7:1;
        /** ch_ena8 : R/WTC/WTS; bitpos: [8]; default: 0;
         *  ch8 enable
         */
        uint32_t ch_ena8:1;
        /** ch_ena9 : R/WTC/WTS; bitpos: [9]; default: 0;
         *  ch9 enable
         */
        uint32_t ch_ena9:1;
        /** ch_ena10 : R/WTC/WTS; bitpos: [10]; default: 0;
         *  ch10 enable
         */
        uint32_t ch_ena10:1;
        /** ch_ena11 : R/WTC/WTS; bitpos: [11]; default: 0;
         *  ch11 enable
         */
        uint32_t ch_ena11:1;
        /** ch_ena12 : R/WTC/WTS; bitpos: [12]; default: 0;
         *  ch12 enable
         */
        uint32_t ch_ena12:1;
        /** ch_ena13 : R/WTC/WTS; bitpos: [13]; default: 0;
         *  ch13 enable
         */
        uint32_t ch_ena13:1;
        /** ch_ena14 : R/WTC/WTS; bitpos: [14]; default: 0;
         *  ch14 enable
         */
        uint32_t ch_ena14:1;
        /** ch_ena15 : R/WTC/WTS; bitpos: [15]; default: 0;
         *  ch15 enable
         */
        uint32_t ch_ena15:1;
        /** ch_ena16 : R/WTC/WTS; bitpos: [16]; default: 0;
         *  ch16 enable
         */
        uint32_t ch_ena16:1;
        /** ch_ena17 : R/WTC/WTS; bitpos: [17]; default: 0;
         *  ch17 enable
         */
        uint32_t ch_ena17:1;
        /** ch_ena18 : R/WTC/WTS; bitpos: [18]; default: 0;
         *  ch18 enable
         */
        uint32_t ch_ena18:1;
        /** ch_ena19 : R/WTC/WTS; bitpos: [19]; default: 0;
         *  ch19 enable
         */
        uint32_t ch_ena19:1;
        /** ch_ena20 : R/WTC/WTS; bitpos: [20]; default: 0;
         *  ch20 enable
         */
        uint32_t ch_ena20:1;
        /** ch_ena21 : R/WTC/WTS; bitpos: [21]; default: 0;
         *  ch21 enable
         */
        uint32_t ch_ena21:1;
        /** ch_ena22 : R/WTC/WTS; bitpos: [22]; default: 0;
         *  ch22 enable
         */
        uint32_t ch_ena22:1;
        /** ch_ena23 : R/WTC/WTS; bitpos: [23]; default: 0;
         *  ch23 enable
         */
        uint32_t ch_ena23:1;
        /** ch_ena24 : R/WTC/WTS; bitpos: [24]; default: 0;
         *  ch24 enable
         */
        uint32_t ch_ena24:1;
        /** ch_ena25 : R/WTC/WTS; bitpos: [25]; default: 0;
         *  ch25 enable
         */
        uint32_t ch_ena25:1;
        /** ch_ena26 : R/WTC/WTS; bitpos: [26]; default: 0;
         *  ch26 enable
         */
        uint32_t ch_ena26:1;
        /** ch_ena27 : R/WTC/WTS; bitpos: [27]; default: 0;
         *  ch27 enable
         */
        uint32_t ch_ena27:1;
        /** ch_ena28 : R/WTC/WTS; bitpos: [28]; default: 0;
         *  ch28 enable
         */
        uint32_t ch_ena28:1;
        /** ch_ena29 : R/WTC/WTS; bitpos: [29]; default: 0;
         *  ch29 enable
         */
        uint32_t ch_ena29:1;
        /** ch_ena30 : R/WTC/WTS; bitpos: [30]; default: 0;
         *  ch30 enable
         */
        uint32_t ch_ena30:1;
        /** ch_ena31 : R/WTC/WTS; bitpos: [31]; default: 0;
         *  ch31 enable
         */
        uint32_t ch_ena31:1;
    };
    uint32_t val;
} soc_etm_ch_ena_ad0_reg_t;

/** Type of ch_ena_ad0_set register
 *  channel enable set register
 */
typedef union {
    struct {
        /** ch_set0 : WT; bitpos: [0]; default: 0;
         *  ch0 set
         */
        uint32_t ch_set0:1;
        /** ch_set1 : WT; bitpos: [1]; default: 0;
         *  ch1 set
         */
        uint32_t ch_set1:1;
        /** ch_set2 : WT; bitpos: [2]; default: 0;
         *  ch2 set
         */
        uint32_t ch_set2:1;
        /** ch_set3 : WT; bitpos: [3]; default: 0;
         *  ch3 set
         */
        uint32_t ch_set3:1;
        /** ch_set4 : WT; bitpos: [4]; default: 0;
         *  ch4 set
         */
        uint32_t ch_set4:1;
        /** ch_set5 : WT; bitpos: [5]; default: 0;
         *  ch5 set
         */
        uint32_t ch_set5:1;
        /** ch_set6 : WT; bitpos: [6]; default: 0;
         *  ch6 set
         */
        uint32_t ch_set6:1;
        /** ch_set7 : WT; bitpos: [7]; default: 0;
         *  ch7 set
         */
        uint32_t ch_set7:1;
        /** ch_set8 : WT; bitpos: [8]; default: 0;
         *  ch8 set
         */
        uint32_t ch_set8:1;
        /** ch_set9 : WT; bitpos: [9]; default: 0;
         *  ch9 set
         */
        uint32_t ch_set9:1;
        /** ch_set10 : WT; bitpos: [10]; default: 0;
         *  ch10 set
         */
        uint32_t ch_set10:1;
        /** ch_set11 : WT; bitpos: [11]; default: 0;
         *  ch11 set
         */
        uint32_t ch_set11:1;
        /** ch_set12 : WT; bitpos: [12]; default: 0;
         *  ch12 set
         */
        uint32_t ch_set12:1;
        /** ch_set13 : WT; bitpos: [13]; default: 0;
         *  ch13 set
         */
        uint32_t ch_set13:1;
        /** ch_set14 : WT; bitpos: [14]; default: 0;
         *  ch14 set
         */
        uint32_t ch_set14:1;
        /** ch_set15 : WT; bitpos: [15]; default: 0;
         *  ch15 set
         */
        uint32_t ch_set15:1;
        /** ch_set16 : WT; bitpos: [16]; default: 0;
         *  ch16 set
         */
        uint32_t ch_set16:1;
        /** ch_set17 : WT; bitpos: [17]; default: 0;
         *  ch17 set
         */
        uint32_t ch_set17:1;
        /** ch_set18 : WT; bitpos: [18]; default: 0;
         *  ch18 set
         */
        uint32_t ch_set18:1;
        /** ch_set19 : WT; bitpos: [19]; default: 0;
         *  ch19 set
         */
        uint32_t ch_set19:1;
        /** ch_set20 : WT; bitpos: [20]; default: 0;
         *  ch20 set
         */
        uint32_t ch_set20:1;
        /** ch_set21 : WT; bitpos: [21]; default: 0;
         *  ch21 set
         */
        uint32_t ch_set21:1;
        /** ch_set22 : WT; bitpos: [22]; default: 0;
         *  ch22 set
         */
        uint32_t ch_set22:1;
        /** ch_set23 : WT; bitpos: [23]; default: 0;
         *  ch23 set
         */
        uint32_t ch_set23:1;
        /** ch_set24 : WT; bitpos: [24]; default: 0;
         *  ch24 set
         */
        uint32_t ch_set24:1;
        /** ch_set25 : WT; bitpos: [25]; default: 0;
         *  ch25 set
         */
        uint32_t ch_set25:1;
        /** ch_set26 : WT; bitpos: [26]; default: 0;
         *  ch26 set
         */
        uint32_t ch_set26:1;
        /** ch_set27 : WT; bitpos: [27]; default: 0;
         *  ch27 set
         */
        uint32_t ch_set27:1;
        /** ch_set28 : WT; bitpos: [28]; default: 0;
         *  ch28 set
         */
        uint32_t ch_set28:1;
        /** ch_set29 : WT; bitpos: [29]; default: 0;
         *  ch29 set
         */
        uint32_t ch_set29:1;
        /** ch_set30 : WT; bitpos: [30]; default: 0;
         *  ch30 set
         */
        uint32_t ch_set30:1;
        /** ch_set31 : WT; bitpos: [31]; default: 0;
         *  ch31 set
         */
        uint32_t ch_set31:1;
    };
    uint32_t val;
} soc_etm_ch_ena_ad0_set_reg_t;

/** Type of ch_ena_ad0_clr register
 *  channel enable clear register
 */
typedef union {
    struct {
        /** ch_clr0 : WT; bitpos: [0]; default: 0;
         *  ch0 clear
         */
        uint32_t ch_clr0:1;
        /** ch_clr1 : WT; bitpos: [1]; default: 0;
         *  ch1 clear
         */
        uint32_t ch_clr1:1;
        /** ch_clr2 : WT; bitpos: [2]; default: 0;
         *  ch2 clear
         */
        uint32_t ch_clr2:1;
        /** ch_clr3 : WT; bitpos: [3]; default: 0;
         *  ch3 clear
         */
        uint32_t ch_clr3:1;
        /** ch_clr4 : WT; bitpos: [4]; default: 0;
         *  ch4 clear
         */
        uint32_t ch_clr4:1;
        /** ch_clr5 : WT; bitpos: [5]; default: 0;
         *  ch5 clear
         */
        uint32_t ch_clr5:1;
        /** ch_clr6 : WT; bitpos: [6]; default: 0;
         *  ch6 clear
         */
        uint32_t ch_clr6:1;
        /** ch_clr7 : WT; bitpos: [7]; default: 0;
         *  ch7 clear
         */
        uint32_t ch_clr7:1;
        /** ch_clr8 : WT; bitpos: [8]; default: 0;
         *  ch8 clear
         */
        uint32_t ch_clr8:1;
        /** ch_clr9 : WT; bitpos: [9]; default: 0;
         *  ch9 clear
         */
        uint32_t ch_clr9:1;
        /** ch_clr10 : WT; bitpos: [10]; default: 0;
         *  ch10 clear
         */
        uint32_t ch_clr10:1;
        /** ch_clr11 : WT; bitpos: [11]; default: 0;
         *  ch11 clear
         */
        uint32_t ch_clr11:1;
        /** ch_clr12 : WT; bitpos: [12]; default: 0;
         *  ch12 clear
         */
        uint32_t ch_clr12:1;
        /** ch_clr13 : WT; bitpos: [13]; default: 0;
         *  ch13 clear
         */
        uint32_t ch_clr13:1;
        /** ch_clr14 : WT; bitpos: [14]; default: 0;
         *  ch14 clear
         */
        uint32_t ch_clr14:1;
        /** ch_clr15 : WT; bitpos: [15]; default: 0;
         *  ch15 clear
         */
        uint32_t ch_clr15:1;
        /** ch_clr16 : WT; bitpos: [16]; default: 0;
         *  ch16 clear
         */
        uint32_t ch_clr16:1;
        /** ch_clr17 : WT; bitpos: [17]; default: 0;
         *  ch17 clear
         */
        uint32_t ch_clr17:1;
        /** ch_clr18 : WT; bitpos: [18]; default: 0;
         *  ch18 clear
         */
        uint32_t ch_clr18:1;
        /** ch_clr19 : WT; bitpos: [19]; default: 0;
         *  ch19 clear
         */
        uint32_t ch_clr19:1;
        /** ch_clr20 : WT; bitpos: [20]; default: 0;
         *  ch20 clear
         */
        uint32_t ch_clr20:1;
        /** ch_clr21 : WT; bitpos: [21]; default: 0;
         *  ch21 clear
         */
        uint32_t ch_clr21:1;
        /** ch_clr22 : WT; bitpos: [22]; default: 0;
         *  ch22 clear
         */
        uint32_t ch_clr22:1;
        /** ch_clr23 : WT; bitpos: [23]; default: 0;
         *  ch23 clear
         */
        uint32_t ch_clr23:1;
        /** ch_clr24 : WT; bitpos: [24]; default: 0;
         *  ch24 clear
         */
        uint32_t ch_clr24:1;
        /** ch_clr25 : WT; bitpos: [25]; default: 0;
         *  ch25 clear
         */
        uint32_t ch_clr25:1;
        /** ch_clr26 : WT; bitpos: [26]; default: 0;
         *  ch26 clear
         */
        uint32_t ch_clr26:1;
        /** ch_clr27 : WT; bitpos: [27]; default: 0;
         *  ch27 clear
         */
        uint32_t ch_clr27:1;
        /** ch_clr28 : WT; bitpos: [28]; default: 0;
         *  ch28 clear
         */
        uint32_t ch_clr28:1;
        /** ch_clr29 : WT; bitpos: [29]; default: 0;
         *  ch29 clear
         */
        uint32_t ch_clr29:1;
        /** ch_clr30 : WT; bitpos: [30]; default: 0;
         *  ch30 clear
         */
        uint32_t ch_clr30:1;
        /** ch_clr31 : WT; bitpos: [31]; default: 0;
         *  ch31 clear
         */
        uint32_t ch_clr31:1;
    };
    uint32_t val;
} soc_etm_ch_ena_ad0_clr_reg_t;

/** Type of ch_ena_ad1 register
 *  channel enable register
 */
typedef union {
    struct {
        /** ch_ena32 : R/WTC/WTS; bitpos: [0]; default: 0;
         *  ch32 enable
         */
        uint32_t ch_ena32:1;
        /** ch_ena33 : R/WTC/WTS; bitpos: [1]; default: 0;
         *  ch33 enable
         */
        uint32_t ch_ena33:1;
        /** ch_ena34 : R/WTC/WTS; bitpos: [2]; default: 0;
         *  ch34 enable
         */
        uint32_t ch_ena34:1;
        /** ch_ena35 : R/WTC/WTS; bitpos: [3]; default: 0;
         *  ch35 enable
         */
        uint32_t ch_ena35:1;
        /** ch_ena36 : R/WTC/WTS; bitpos: [4]; default: 0;
         *  ch36 enable
         */
        uint32_t ch_ena36:1;
        /** ch_ena37 : R/WTC/WTS; bitpos: [5]; default: 0;
         *  ch37 enable
         */
        uint32_t ch_ena37:1;
        /** ch_ena38 : R/WTC/WTS; bitpos: [6]; default: 0;
         *  ch38 enable
         */
        uint32_t ch_ena38:1;
        /** ch_ena39 : R/WTC/WTS; bitpos: [7]; default: 0;
         *  ch39 enable
         */
        uint32_t ch_ena39:1;
        /** ch_ena40 : R/WTC/WTS; bitpos: [8]; default: 0;
         *  ch40 enable
         */
        uint32_t ch_ena40:1;
        /** ch_ena41 : R/WTC/WTS; bitpos: [9]; default: 0;
         *  ch41 enable
         */
        uint32_t ch_ena41:1;
        /** ch_ena42 : R/WTC/WTS; bitpos: [10]; default: 0;
         *  ch42 enable
         */
        uint32_t ch_ena42:1;
        /** ch_ena43 : R/WTC/WTS; bitpos: [11]; default: 0;
         *  ch43 enable
         */
        uint32_t ch_ena43:1;
        /** ch_ena44 : R/WTC/WTS; bitpos: [12]; default: 0;
         *  ch44 enable
         */
        uint32_t ch_ena44:1;
        /** ch_ena45 : R/WTC/WTS; bitpos: [13]; default: 0;
         *  ch45 enable
         */
        uint32_t ch_ena45:1;
        /** ch_ena46 : R/WTC/WTS; bitpos: [14]; default: 0;
         *  ch46 enable
         */
        uint32_t ch_ena46:1;
        /** ch_ena47 : R/WTC/WTS; bitpos: [15]; default: 0;
         *  ch47 enable
         */
        uint32_t ch_ena47:1;
        /** ch_ena48 : R/WTC/WTS; bitpos: [16]; default: 0;
         *  ch48 enable
         */
        uint32_t ch_ena48:1;
        /** ch_ena49 : R/WTC/WTS; bitpos: [17]; default: 0;
         *  ch49 enable
         */
        uint32_t ch_ena49:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} soc_etm_ch_ena_ad1_reg_t;

/** Type of ch_ena_ad1_set register
 *  channel enable set register
 */
typedef union {
    struct {
        /** ch_set32 : WT; bitpos: [0]; default: 0;
         *  ch32 set
         */
        uint32_t ch_set32:1;
        /** ch_set33 : WT; bitpos: [1]; default: 0;
         *  ch33 set
         */
        uint32_t ch_set33:1;
        /** ch_set34 : WT; bitpos: [2]; default: 0;
         *  ch34 set
         */
        uint32_t ch_set34:1;
        /** ch_set35 : WT; bitpos: [3]; default: 0;
         *  ch35 set
         */
        uint32_t ch_set35:1;
        /** ch_set36 : WT; bitpos: [4]; default: 0;
         *  ch36 set
         */
        uint32_t ch_set36:1;
        /** ch_set37 : WT; bitpos: [5]; default: 0;
         *  ch37 set
         */
        uint32_t ch_set37:1;
        /** ch_set38 : WT; bitpos: [6]; default: 0;
         *  ch38 set
         */
        uint32_t ch_set38:1;
        /** ch_set39 : WT; bitpos: [7]; default: 0;
         *  ch39 set
         */
        uint32_t ch_set39:1;
        /** ch_set40 : WT; bitpos: [8]; default: 0;
         *  ch40 set
         */
        uint32_t ch_set40:1;
        /** ch_set41 : WT; bitpos: [9]; default: 0;
         *  ch41 set
         */
        uint32_t ch_set41:1;
        /** ch_set42 : WT; bitpos: [10]; default: 0;
         *  ch42 set
         */
        uint32_t ch_set42:1;
        /** ch_set43 : WT; bitpos: [11]; default: 0;
         *  ch43 set
         */
        uint32_t ch_set43:1;
        /** ch_set44 : WT; bitpos: [12]; default: 0;
         *  ch44 set
         */
        uint32_t ch_set44:1;
        /** ch_set45 : WT; bitpos: [13]; default: 0;
         *  ch45 set
         */
        uint32_t ch_set45:1;
        /** ch_set46 : WT; bitpos: [14]; default: 0;
         *  ch46 set
         */
        uint32_t ch_set46:1;
        /** ch_set47 : WT; bitpos: [15]; default: 0;
         *  ch47 set
         */
        uint32_t ch_set47:1;
        /** ch_set48 : WT; bitpos: [16]; default: 0;
         *  ch48 set
         */
        uint32_t ch_set48:1;
        /** ch_set49 : WT; bitpos: [17]; default: 0;
         *  ch49 set
         */
        uint32_t ch_set49:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} soc_etm_ch_ena_ad1_set_reg_t;

/** Type of ch_ena_ad1_clr register
 *  channel enable clear register
 */
typedef union {
    struct {
        /** ch_clr32 : WT; bitpos: [0]; default: 0;
         *  ch32 clear
         */
        uint32_t ch_clr32:1;
        /** ch_clr33 : WT; bitpos: [1]; default: 0;
         *  ch33 clear
         */
        uint32_t ch_clr33:1;
        /** ch_clr34 : WT; bitpos: [2]; default: 0;
         *  ch34 clear
         */
        uint32_t ch_clr34:1;
        /** ch_clr35 : WT; bitpos: [3]; default: 0;
         *  ch35 clear
         */
        uint32_t ch_clr35:1;
        /** ch_clr36 : WT; bitpos: [4]; default: 0;
         *  ch36 clear
         */
        uint32_t ch_clr36:1;
        /** ch_clr37 : WT; bitpos: [5]; default: 0;
         *  ch37 clear
         */
        uint32_t ch_clr37:1;
        /** ch_clr38 : WT; bitpos: [6]; default: 0;
         *  ch38 clear
         */
        uint32_t ch_clr38:1;
        /** ch_clr39 : WT; bitpos: [7]; default: 0;
         *  ch39 clear
         */
        uint32_t ch_clr39:1;
        /** ch_clr40 : WT; bitpos: [8]; default: 0;
         *  ch40 clear
         */
        uint32_t ch_clr40:1;
        /** ch_clr41 : WT; bitpos: [9]; default: 0;
         *  ch41 clear
         */
        uint32_t ch_clr41:1;
        /** ch_clr42 : WT; bitpos: [10]; default: 0;
         *  ch42 clear
         */
        uint32_t ch_clr42:1;
        /** ch_clr43 : WT; bitpos: [11]; default: 0;
         *  ch43 clear
         */
        uint32_t ch_clr43:1;
        /** ch_clr44 : WT; bitpos: [12]; default: 0;
         *  ch44 clear
         */
        uint32_t ch_clr44:1;
        /** ch_clr45 : WT; bitpos: [13]; default: 0;
         *  ch45 clear
         */
        uint32_t ch_clr45:1;
        /** ch_clr46 : WT; bitpos: [14]; default: 0;
         *  ch46 clear
         */
        uint32_t ch_clr46:1;
        /** ch_clr47 : WT; bitpos: [15]; default: 0;
         *  ch47 clear
         */
        uint32_t ch_clr47:1;
        /** ch_clr48 : WT; bitpos: [16]; default: 0;
         *  ch48 clear
         */
        uint32_t ch_clr48:1;
        /** ch_clr49 : WT; bitpos: [17]; default: 0;
         *  ch49 clear
         */
        uint32_t ch_clr49:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} soc_etm_ch_ena_ad1_clr_reg_t;

/** Type of chn_evt_id register
 *  channeln event id register
 */
typedef union {
    struct {
        /** evt_id : R/W; bitpos: [7:0]; default: 0;
         *  chn_evt_id
         */
        uint32_t evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_chn_evt_id_reg_t;

/** Type of chn_task_id register
 *  channeln task id register
 */
typedef union {
    struct {
        /** task_id : R/W; bitpos: [7:0]; default: 0;
         *  chn_task_id
         */
        uint32_t task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_chn_task_id_reg_t;

/** Type of clk_en register
 *  etm clock enable register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 0;
         *  clock enable
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} soc_etm_clk_en_reg_t;


/** Group: Version Register */
/** Type of date register
 *  etm date register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 35664018;
         *  date
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} soc_etm_date_reg_t;


typedef struct soc_etm_dev_t {
    volatile soc_etm_ch_ena_ad0_reg_t ch_ena_ad0;
    volatile soc_etm_ch_ena_ad0_set_reg_t ch_ena_ad0_set;
    volatile soc_etm_ch_ena_ad0_clr_reg_t ch_ena_ad0_clr;
    volatile soc_etm_ch_ena_ad1_reg_t ch_ena_ad1;
    volatile soc_etm_ch_ena_ad1_set_reg_t ch_ena_ad1_set;
    volatile soc_etm_ch_ena_ad1_clr_reg_t ch_ena_ad1_clr;
    volatile struct {
        soc_etm_chn_evt_id_reg_t eid;
        soc_etm_chn_task_id_reg_t tid;
    } channel[50];
    volatile soc_etm_clk_en_reg_t clk_en;
    volatile soc_etm_date_reg_t date;
} soc_etm_dev_t;

extern soc_etm_dev_t SOC_ETM;

#ifndef __cplusplus
_Static_assert(sizeof(soc_etm_dev_t) == 0x1b0, "Invalid size of soc_etm_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
