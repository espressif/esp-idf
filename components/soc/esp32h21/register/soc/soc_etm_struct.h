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

/** Group: Configuration Register */
/** Type of ch_ena_ad0 register
 *  Channel enable status register
 */
typedef union {
    struct {
        /** ch_enabled0 : R/WTC/SS; bitpos: [0]; default: 0;
         *  Represents channel0 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled0:1;
        /** ch_enabled1 : R/WTC/SS; bitpos: [1]; default: 0;
         *  Represents channel1 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled1:1;
        /** ch_enabled2 : R/WTC/SS; bitpos: [2]; default: 0;
         *  Represents channel2 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled2:1;
        /** ch_enabled3 : R/WTC/SS; bitpos: [3]; default: 0;
         *  Represents channel3 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled3:1;
        /** ch_enabled4 : R/WTC/SS; bitpos: [4]; default: 0;
         *  Represents channel4 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled4:1;
        /** ch_enabled5 : R/WTC/SS; bitpos: [5]; default: 0;
         *  Represents channel5 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled5:1;
        /** ch_enabled6 : R/WTC/SS; bitpos: [6]; default: 0;
         *  Represents channel6 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled6:1;
        /** ch_enabled7 : R/WTC/SS; bitpos: [7]; default: 0;
         *  Represents channel7 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled7:1;
        /** ch_enabled8 : R/WTC/SS; bitpos: [8]; default: 0;
         *  Represents channel8 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled8:1;
        /** ch_enabled9 : R/WTC/SS; bitpos: [9]; default: 0;
         *  Represents channel9 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled9:1;
        /** ch_enabled10 : R/WTC/SS; bitpos: [10]; default: 0;
         *  Represents channel10 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled10:1;
        /** ch_enabled11 : R/WTC/SS; bitpos: [11]; default: 0;
         *  Represents channel11 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled11:1;
        /** ch_enabled12 : R/WTC/SS; bitpos: [12]; default: 0;
         *  Represents channel12 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled12:1;
        /** ch_enabled13 : R/WTC/SS; bitpos: [13]; default: 0;
         *  Represents channel13 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled13:1;
        /** ch_enabled14 : R/WTC/SS; bitpos: [14]; default: 0;
         *  Represents channel14 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled14:1;
        /** ch_enabled15 : R/WTC/SS; bitpos: [15]; default: 0;
         *  Represents channel15 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled15:1;
        /** ch_enabled16 : R/WTC/SS; bitpos: [16]; default: 0;
         *  Represents channel16 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled16:1;
        /** ch_enabled17 : R/WTC/SS; bitpos: [17]; default: 0;
         *  Represents channel17 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled17:1;
        /** ch_enabled18 : R/WTC/SS; bitpos: [18]; default: 0;
         *  Represents channel18 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled18:1;
        /** ch_enabled19 : R/WTC/SS; bitpos: [19]; default: 0;
         *  Represents channel19 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled19:1;
        /** ch_enabled20 : R/WTC/SS; bitpos: [20]; default: 0;
         *  Represents channel20 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled20:1;
        /** ch_enabled21 : R/WTC/SS; bitpos: [21]; default: 0;
         *  Represents channel21 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled21:1;
        /** ch_enabled22 : R/WTC/SS; bitpos: [22]; default: 0;
         *  Represents channel22 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled22:1;
        /** ch_enabled23 : R/WTC/SS; bitpos: [23]; default: 0;
         *  Represents channel23 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled23:1;
        /** ch_enabled24 : R/WTC/SS; bitpos: [24]; default: 0;
         *  Represents channel24 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled24:1;
        /** ch_enabled25 : R/WTC/SS; bitpos: [25]; default: 0;
         *  Represents channel25 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled25:1;
        /** ch_enabled26 : R/WTC/SS; bitpos: [26]; default: 0;
         *  Represents channel26 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled26:1;
        /** ch_enabled27 : R/WTC/SS; bitpos: [27]; default: 0;
         *  Represents channel27 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled27:1;
        /** ch_enabled28 : R/WTC/SS; bitpos: [28]; default: 0;
         *  Represents channel28 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled28:1;
        /** ch_enabled29 : R/WTC/SS; bitpos: [29]; default: 0;
         *  Represents channel29 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled29:1;
        /** ch_enabled30 : R/WTC/SS; bitpos: [30]; default: 0;
         *  Represents channel30 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled30:1;
        /** ch_enabled31 : R/WTC/SS; bitpos: [31]; default: 0;
         *  Represents channel31 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled31:1;
    };
    uint32_t val;
} soc_etm_ch_ena_ad0_reg_t;

/** Type of ch_ena_ad0_set register
 *  Channel enable register
 */
typedef union {
    struct {
        /** ch_enable0 : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to enable channel0.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable0:1;
        /** ch_enable1 : WT; bitpos: [1]; default: 0;
         *  Configures whether or not to enable channel1.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable1:1;
        /** ch_enable2 : WT; bitpos: [2]; default: 0;
         *  Configures whether or not to enable channel2.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable2:1;
        /** ch_enable3 : WT; bitpos: [3]; default: 0;
         *  Configures whether or not to enable channel3.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable3:1;
        /** ch_enable4 : WT; bitpos: [4]; default: 0;
         *  Configures whether or not to enable channel4.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable4:1;
        /** ch_enable5 : WT; bitpos: [5]; default: 0;
         *  Configures whether or not to enable channel5.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable5:1;
        /** ch_enable6 : WT; bitpos: [6]; default: 0;
         *  Configures whether or not to enable channel6.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable6:1;
        /** ch_enable7 : WT; bitpos: [7]; default: 0;
         *  Configures whether or not to enable channel7.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable7:1;
        /** ch_enable8 : WT; bitpos: [8]; default: 0;
         *  Configures whether or not to enable channel8.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable8:1;
        /** ch_enable9 : WT; bitpos: [9]; default: 0;
         *  Configures whether or not to enable channel9.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable9:1;
        /** ch_enable10 : WT; bitpos: [10]; default: 0;
         *  Configures whether or not to enable channel10.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable10:1;
        /** ch_enable11 : WT; bitpos: [11]; default: 0;
         *  Configures whether or not to enable channel11.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable11:1;
        /** ch_enable12 : WT; bitpos: [12]; default: 0;
         *  Configures whether or not to enable channel12.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable12:1;
        /** ch_enable13 : WT; bitpos: [13]; default: 0;
         *  Configures whether or not to enable channel13.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable13:1;
        /** ch_enable14 : WT; bitpos: [14]; default: 0;
         *  Configures whether or not to enable channel14.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable14:1;
        /** ch_enable15 : WT; bitpos: [15]; default: 0;
         *  Configures whether or not to enable channel15.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable15:1;
        /** ch_enable16 : WT; bitpos: [16]; default: 0;
         *  Configures whether or not to enable channel16.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable16:1;
        /** ch_enable17 : WT; bitpos: [17]; default: 0;
         *  Configures whether or not to enable channel17.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable17:1;
        /** ch_enable18 : WT; bitpos: [18]; default: 0;
         *  Configures whether or not to enable channel18.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable18:1;
        /** ch_enable19 : WT; bitpos: [19]; default: 0;
         *  Configures whether or not to enable channel19.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable19:1;
        /** ch_enable20 : WT; bitpos: [20]; default: 0;
         *  Configures whether or not to enable channel20.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable20:1;
        /** ch_enable21 : WT; bitpos: [21]; default: 0;
         *  Configures whether or not to enable channel21.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable21:1;
        /** ch_enable22 : WT; bitpos: [22]; default: 0;
         *  Configures whether or not to enable channel22.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable22:1;
        /** ch_enable23 : WT; bitpos: [23]; default: 0;
         *  Configures whether or not to enable channel23.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable23:1;
        /** ch_enable24 : WT; bitpos: [24]; default: 0;
         *  Configures whether or not to enable channel24.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable24:1;
        /** ch_enable25 : WT; bitpos: [25]; default: 0;
         *  Configures whether or not to enable channel25.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable25:1;
        /** ch_enable26 : WT; bitpos: [26]; default: 0;
         *  Configures whether or not to enable channel26.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable26:1;
        /** ch_enable27 : WT; bitpos: [27]; default: 0;
         *  Configures whether or not to enable channel27.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable27:1;
        /** ch_enable28 : WT; bitpos: [28]; default: 0;
         *  Configures whether or not to enable channel28.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable28:1;
        /** ch_enable29 : WT; bitpos: [29]; default: 0;
         *  Configures whether or not to enable channel29.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable29:1;
        /** ch_enable30 : WT; bitpos: [30]; default: 0;
         *  Configures whether or not to enable channel30.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable30:1;
        /** ch_enable31 : WT; bitpos: [31]; default: 0;
         *  Configures whether or not to enable channel31.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable31:1;
    };
    uint32_t val;
} soc_etm_ch_ena_ad0_set_reg_t;

/** Type of ch_ena_ad0_clr register
 *  Channel disable register
 */
typedef union {
    struct {
        /** ch_disable0 : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to disable channel0.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable0:1;
        /** ch_disable1 : WT; bitpos: [1]; default: 0;
         *  Configures whether or not to disable channel1.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable1:1;
        /** ch_disable2 : WT; bitpos: [2]; default: 0;
         *  Configures whether or not to disable channel2.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable2:1;
        /** ch_disable3 : WT; bitpos: [3]; default: 0;
         *  Configures whether or not to disable channel3.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable3:1;
        /** ch_disable4 : WT; bitpos: [4]; default: 0;
         *  Configures whether or not to disable channel4.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable4:1;
        /** ch_disable5 : WT; bitpos: [5]; default: 0;
         *  Configures whether or not to disable channel5.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable5:1;
        /** ch_disable6 : WT; bitpos: [6]; default: 0;
         *  Configures whether or not to disable channel6.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable6:1;
        /** ch_disable7 : WT; bitpos: [7]; default: 0;
         *  Configures whether or not to disable channel7.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable7:1;
        /** ch_disable8 : WT; bitpos: [8]; default: 0;
         *  Configures whether or not to disable channel8.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable8:1;
        /** ch_disable9 : WT; bitpos: [9]; default: 0;
         *  Configures whether or not to disable channel9.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable9:1;
        /** ch_disable10 : WT; bitpos: [10]; default: 0;
         *  Configures whether or not to disable channel10.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable10:1;
        /** ch_disable11 : WT; bitpos: [11]; default: 0;
         *  Configures whether or not to disable channel11.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable11:1;
        /** ch_disable12 : WT; bitpos: [12]; default: 0;
         *  Configures whether or not to disable channel12.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable12:1;
        /** ch_disable13 : WT; bitpos: [13]; default: 0;
         *  Configures whether or not to disable channel13.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable13:1;
        /** ch_disable14 : WT; bitpos: [14]; default: 0;
         *  Configures whether or not to disable channel14.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable14:1;
        /** ch_disable15 : WT; bitpos: [15]; default: 0;
         *  Configures whether or not to disable channel15.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable15:1;
        /** ch_disable16 : WT; bitpos: [16]; default: 0;
         *  Configures whether or not to disable channel16.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable16:1;
        /** ch_disable17 : WT; bitpos: [17]; default: 0;
         *  Configures whether or not to disable channel17.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable17:1;
        /** ch_disable18 : WT; bitpos: [18]; default: 0;
         *  Configures whether or not to disable channel18.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable18:1;
        /** ch_disable19 : WT; bitpos: [19]; default: 0;
         *  Configures whether or not to disable channel19.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable19:1;
        /** ch_disable20 : WT; bitpos: [20]; default: 0;
         *  Configures whether or not to disable channel20.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable20:1;
        /** ch_disable21 : WT; bitpos: [21]; default: 0;
         *  Configures whether or not to disable channel21.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable21:1;
        /** ch_disable22 : WT; bitpos: [22]; default: 0;
         *  Configures whether or not to disable channel22.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable22:1;
        /** ch_disable23 : WT; bitpos: [23]; default: 0;
         *  Configures whether or not to disable channel23.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable23:1;
        /** ch_disable24 : WT; bitpos: [24]; default: 0;
         *  Configures whether or not to disable channel24.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable24:1;
        /** ch_disable25 : WT; bitpos: [25]; default: 0;
         *  Configures whether or not to disable channel25.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable25:1;
        /** ch_disable26 : WT; bitpos: [26]; default: 0;
         *  Configures whether or not to disable channel26.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable26:1;
        /** ch_disable27 : WT; bitpos: [27]; default: 0;
         *  Configures whether or not to disable channel27.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable27:1;
        /** ch_disable28 : WT; bitpos: [28]; default: 0;
         *  Configures whether or not to disable channel28.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable28:1;
        /** ch_disable29 : WT; bitpos: [29]; default: 0;
         *  Configures whether or not to disable channel29.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable29:1;
        /** ch_disable30 : WT; bitpos: [30]; default: 0;
         *  Configures whether or not to disable channel30.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable30:1;
        /** ch_disable31 : WT; bitpos: [31]; default: 0;
         *  Configures whether or not to disable channel31.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable31:1;
    };
    uint32_t val;
} soc_etm_ch_ena_ad0_clr_reg_t;

/** Type of ch_ena_ad1 register
 *  Channel enable status register
 */
typedef union {
    struct {
        /** ch_enabled32 : R/WTC/SS; bitpos: [0]; default: 0;
         *  Represents channel32 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled32:1;
        /** ch_enabled33 : R/WTC/SS; bitpos: [1]; default: 0;
         *  Represents channel33 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled33:1;
        /** ch_enabled34 : R/WTC/SS; bitpos: [2]; default: 0;
         *  Represents channel34 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled34:1;
        /** ch_enabled35 : R/WTC/SS; bitpos: [3]; default: 0;
         *  Represents channel35 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled35:1;
        /** ch_enabled36 : R/WTC/SS; bitpos: [4]; default: 0;
         *  Represents channel36 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled36:1;
        /** ch_enabled37 : R/WTC/SS; bitpos: [5]; default: 0;
         *  Represents channel37 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled37:1;
        /** ch_enabled38 : R/WTC/SS; bitpos: [6]; default: 0;
         *  Represents channel38 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled38:1;
        /** ch_enabled39 : R/WTC/SS; bitpos: [7]; default: 0;
         *  Represents channel39 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled39:1;
        /** ch_enabled40 : R/WTC/SS; bitpos: [8]; default: 0;
         *  Represents channel40 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled40:1;
        /** ch_enabled41 : R/WTC/SS; bitpos: [9]; default: 0;
         *  Represents channel41 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled41:1;
        /** ch_enabled42 : R/WTC/SS; bitpos: [10]; default: 0;
         *  Represents channel42 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled42:1;
        /** ch_enabled43 : R/WTC/SS; bitpos: [11]; default: 0;
         *  Represents channel43 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled43:1;
        /** ch_enabled44 : R/WTC/SS; bitpos: [12]; default: 0;
         *  Represents channel44 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled44:1;
        /** ch_enabled45 : R/WTC/SS; bitpos: [13]; default: 0;
         *  Represents channel45 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled45:1;
        /** ch_enabled46 : R/WTC/SS; bitpos: [14]; default: 0;
         *  Represents channel46 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled46:1;
        /** ch_enabled47 : R/WTC/SS; bitpos: [15]; default: 0;
         *  Represents channel47 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled47:1;
        /** ch_enabled48 : R/WTC/SS; bitpos: [16]; default: 0;
         *  Represents channel48 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled48:1;
        /** ch_enabled49 : R/WTC/SS; bitpos: [17]; default: 0;
         *  Represents channel49 enable status.
         *  0: Disable
         *  1: Enable
         */
        uint32_t ch_enabled49:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} soc_etm_ch_ena_ad1_reg_t;

/** Type of ch_ena_ad1_set register
 *  Channel enable register
 */
typedef union {
    struct {
        /** ch_enable32 : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to enable channel32.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable32:1;
        /** ch_enable33 : WT; bitpos: [1]; default: 0;
         *  Configures whether or not to enable channel33.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable33:1;
        /** ch_enable34 : WT; bitpos: [2]; default: 0;
         *  Configures whether or not to enable channel34.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable34:1;
        /** ch_enable35 : WT; bitpos: [3]; default: 0;
         *  Configures whether or not to enable channel35.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable35:1;
        /** ch_enable36 : WT; bitpos: [4]; default: 0;
         *  Configures whether or not to enable channel36.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable36:1;
        /** ch_enable37 : WT; bitpos: [5]; default: 0;
         *  Configures whether or not to enable channel37.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable37:1;
        /** ch_enable38 : WT; bitpos: [6]; default: 0;
         *  Configures whether or not to enable channel38.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable38:1;
        /** ch_enable39 : WT; bitpos: [7]; default: 0;
         *  Configures whether or not to enable channel39.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable39:1;
        /** ch_enable40 : WT; bitpos: [8]; default: 0;
         *  Configures whether or not to enable channel40.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable40:1;
        /** ch_enable41 : WT; bitpos: [9]; default: 0;
         *  Configures whether or not to enable channel41.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable41:1;
        /** ch_enable42 : WT; bitpos: [10]; default: 0;
         *  Configures whether or not to enable channel42.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable42:1;
        /** ch_enable43 : WT; bitpos: [11]; default: 0;
         *  Configures whether or not to enable channel43.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable43:1;
        /** ch_enable44 : WT; bitpos: [12]; default: 0;
         *  Configures whether or not to enable channel44.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable44:1;
        /** ch_enable45 : WT; bitpos: [13]; default: 0;
         *  Configures whether or not to enable channel45.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable45:1;
        /** ch_enable46 : WT; bitpos: [14]; default: 0;
         *  Configures whether or not to enable channel46.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable46:1;
        /** ch_enable47 : WT; bitpos: [15]; default: 0;
         *  Configures whether or not to enable channel47.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable47:1;
        /** ch_enable48 : WT; bitpos: [16]; default: 0;
         *  Configures whether or not to enable channel48.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable48:1;
        /** ch_enable49 : WT; bitpos: [17]; default: 0;
         *  Configures whether or not to enable channel49.
         *  0: Invalid. No effect
         *  1: Enable
         */
        uint32_t ch_enable49:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} soc_etm_ch_ena_ad1_set_reg_t;

/** Type of ch_ena_ad1_clr register
 *  Channel disable register
 */
typedef union {
    struct {
        /** ch_disable32 : WT; bitpos: [0]; default: 0;
         *  Configures whether or not to disable channel32.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable32:1;
        /** ch_disable33 : WT; bitpos: [1]; default: 0;
         *  Configures whether or not to disable channel33.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable33:1;
        /** ch_disable34 : WT; bitpos: [2]; default: 0;
         *  Configures whether or not to disable channel34.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable34:1;
        /** ch_disable35 : WT; bitpos: [3]; default: 0;
         *  Configures whether or not to disable channel35.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable35:1;
        /** ch_disable36 : WT; bitpos: [4]; default: 0;
         *  Configures whether or not to disable channel36.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable36:1;
        /** ch_disable37 : WT; bitpos: [5]; default: 0;
         *  Configures whether or not to disable channel37.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable37:1;
        /** ch_disable38 : WT; bitpos: [6]; default: 0;
         *  Configures whether or not to disable channel38.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable38:1;
        /** ch_disable39 : WT; bitpos: [7]; default: 0;
         *  Configures whether or not to disable channel39.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable39:1;
        /** ch_disable40 : WT; bitpos: [8]; default: 0;
         *  Configures whether or not to disable channel40.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable40:1;
        /** ch_disable41 : WT; bitpos: [9]; default: 0;
         *  Configures whether or not to disable channel41.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable41:1;
        /** ch_disable42 : WT; bitpos: [10]; default: 0;
         *  Configures whether or not to disable channel42.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable42:1;
        /** ch_disable43 : WT; bitpos: [11]; default: 0;
         *  Configures whether or not to disable channel43.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable43:1;
        /** ch_disable44 : WT; bitpos: [12]; default: 0;
         *  Configures whether or not to disable channel44.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable44:1;
        /** ch_disable45 : WT; bitpos: [13]; default: 0;
         *  Configures whether or not to disable channel45.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable45:1;
        /** ch_disable46 : WT; bitpos: [14]; default: 0;
         *  Configures whether or not to disable channel46.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable46:1;
        /** ch_disable47 : WT; bitpos: [15]; default: 0;
         *  Configures whether or not to disable channel47.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable47:1;
        /** ch_disable48 : WT; bitpos: [16]; default: 0;
         *  Configures whether or not to disable channel48.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable48:1;
        /** ch_disable49 : WT; bitpos: [17]; default: 0;
         *  Configures whether or not to disable channel49.
         *  0: Invalid. No effect
         *  1: Clear
         */
        uint32_t ch_disable49:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} soc_etm_ch_ena_ad1_clr_reg_t;

/** Type of chn_evt_id register
 *  Channeln event ID register
 */
typedef union {
    struct {
        /** chn_evt_id : R/W; bitpos: [7:0]; default: 0;
         *  Configures the event ID of channeln.
         */
        uint32_t chn_evt_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_chn_evt_id_reg_t;

/** Type of chn_task_id register
 *  Channeln task ID register
 */
typedef union {
    struct {
        /** chn_task_id : R/W; bitpos: [7:0]; default: 0;
         *  Configures the task ID of channeln.
         */
        uint32_t chn_task_id:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} soc_etm_chn_task_id_reg_t;

/** Type of clk_en register
 *  ETM clock enable register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 0;
         *  Configures resister clock gating.
         *  0: Support clock only when application writes registers
         *  1: Force on clock gating for registers
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} soc_etm_clk_en_reg_t;


/** Group: Version Register */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 35664018;
         *  Version control register.
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} soc_etm_date_reg_t;


typedef struct {
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
