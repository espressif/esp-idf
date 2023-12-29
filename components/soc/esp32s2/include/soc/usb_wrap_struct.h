/** Copyright 2020 Espressif Systems (Shanghai) PTE LTD
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Control/Status registers */
/** Type of otg_conf register
 *  PAD/DFIFO/PHY configuration register.
 */
typedef union {
    struct {
        /** srp_sessend_override : R/W; bitpos: [0]; default: 0;
         *  This bit is used to enable the software override of srp session end signal.1'b0:
         *  the signal is controlled by the chip input.1'b1: the signal is controlled by the
         *  software.
         */
        uint32_t srp_sessend_override:1;
        /** srp_sessend_value : R/W; bitpos: [1]; default: 0;
         *  Software override value of srp session end signal.
         */
        uint32_t srp_sessend_value:1;
        /** phy_sel : R/W; bitpos: [2]; default: 0;
         *  Select internal or external PHY.1'b0: Select internal PHY.1'b1: Select external PHY
         */
        uint32_t phy_sel:1;
        /** dfifo_force_pd : R/W; bitpos: [3]; default: 0;
         *  Force the dfifo to go into low power mode. The data in dfifo will not lost.
         */
        uint32_t dfifo_force_pd:1;
        /** dbnce_fltr_bypass : R/W; bitpos: [4]; default: 0;
         *  Bypass Debounce filters for avalid.
         */
        uint32_t dbnce_fltr_bypass:1;
        /** exchg_pins_override : R/W; bitpos: [5]; default: 0;
         *  Enable software to control USB D+ D- exchange
         */
        uint32_t exchg_pins_override:1;
        /** exchg_pins : R/W; bitpos: [6]; default: 0;
         *  USB D+/D- exchange.1'b0: don't change.1'b1: exchange D+ D-.
         */
        uint32_t exchg_pins:1;
        /** vrefh : R/W; bitpos: [8:7]; default: 0;
         *  Control single-end input high threshold.
         */
        uint32_t vrefh:2;
        /** vrefl : R/W; bitpos: [10:9]; default: 0;
         *  Control single-end input low threshold.
         */
        uint32_t vrefl:2;
        /** vref_override : R/W; bitpos: [11]; default: 0;
         *  Enable software to control input  threshold.
         */
        uint32_t vref_override:1;
        /** pad_pull_override : R/W; bitpos: [12]; default: 0;
         *  Enable software to control USB pad in pullup or pulldown mode.
         */
        uint32_t pad_pull_override:1;
        /** dp_pullup : R/W; bitpos: [13]; default: 0;
         *  Control USB D+ pullup.
         */
        uint32_t dp_pullup:1;
        /** dp_pulldown : R/W; bitpos: [14]; default: 0;
         *  Control USB D+ pulldown.
         */
        uint32_t dp_pulldown:1;
        /** dm_pullup : R/W; bitpos: [15]; default: 0;
         *  Control USB D+ pullup.
         */
        uint32_t dm_pullup:1;
        /** dm_pulldown : R/W; bitpos: [16]; default: 0;
         *  Control USB D+ pulldown.
         */
        uint32_t dm_pulldown:1;
        /** pullup_value : R/W; bitpos: [17]; default: 0;
         *  Control pullup value.1'b0: typical value is 2.4K.1'b1: typical value is 1.2K.
         */
        uint32_t pullup_value:1;
        /** pad_enable : R/W; bitpos: [18]; default: 0;
         *  Enable USB pad function.
         */
        uint32_t pad_enable:1;
        /** ahb_clk_force_on : R/W; bitpos: [19]; default: 1;
         *  Force AHB clock always on.
         */
        uint32_t ahb_clk_force_on:1;
        /** phy_clk_force_on : R/W; bitpos: [20]; default: 1;
         *  Force PHY clock always on.
         */
        uint32_t phy_clk_force_on:1;
        /** phy_tx_edge_sel : R/W; bitpos: [21]; default: 0;
         *  Select PHY tx signal output clock edge.1'b0: negedge;1'b1: posedge.
         */
        uint32_t phy_tx_edge_sel:1;
        /** dfifo_force_pu : R/W; bitpos: [22]; default: 0;
         *  Disable the dfifo to go into low power mode. The data in dfifo will not lost.
         */
        uint32_t dfifo_force_pu:1;
        uint32_t reserved_23:8;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  Disable auto clock gating of CSR registers.
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} usb_wrap_otg_conf_reg_t;

/** Type of test_conf register
 *  TEST relative configuration registers.
 */
typedef union {
    struct {
        /** test_enable : R/W; bitpos: [0]; default: 0;
         *  Enable to test the USB pad.
         */
        uint32_t test_enable:1;
        /** test_usb_wrap_oe : R/W; bitpos: [1]; default: 0;
         *  USB pad oen in test.
         */
        uint32_t test_usb_wrap_oe:1;
        /** test_tx_dp : R/W; bitpos: [2]; default: 0;
         *  USB D+ tx value in test.
         */
        uint32_t test_tx_dp:1;
        /** test_tx_dm : R/W; bitpos: [3]; default: 0;
         *  USB D- tx value in test.
         */
        uint32_t test_tx_dm:1;
        /** test_rx_rcv : RO; bitpos: [4]; default: 0;
         *  USB differential rx value in test.
         */
        uint32_t test_rx_rcv:1;
        /** test_rx_dp : RO; bitpos: [5]; default: 0;
         *  USB D+ rx value in test.
         */
        uint32_t test_rx_dp:1;
        /** test_rx_dm : RO; bitpos: [6]; default: 0;
         *  USB D- rx value in test.
         */
        uint32_t test_rx_dm:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} usb_wrap_test_conf_reg_t;


/** Status registers */
/** Type of date register
 *  Version register.
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [31:0]; default: 419631616;
         *  data register.
         */
        uint32_t date:32;
    };
    uint32_t val;
} usb_wrap_date_reg_t;


typedef struct {
    volatile usb_wrap_otg_conf_reg_t otg_conf;
    volatile usb_wrap_test_conf_reg_t test_conf;
    uint32_t reserved_008;
    uint32_t reserved_00c;
    uint32_t reserved_010;
    uint32_t reserved_014;
    uint32_t reserved_018;
    uint32_t reserved_01c;
    uint32_t reserved_020;
    uint32_t reserved_024;
    uint32_t reserved_028;
    uint32_t reserved_02c;
    uint32_t reserved_030;
    uint32_t reserved_034;
    uint32_t reserved_038;
    uint32_t reserved_03c;
    uint32_t reserved_040;
    uint32_t reserved_044;
    uint32_t reserved_048;
    uint32_t reserved_04c;
    uint32_t reserved_050;
    uint32_t reserved_054;
    uint32_t reserved_058;
    uint32_t reserved_05c;
    uint32_t reserved_060;
    uint32_t reserved_064;
    uint32_t reserved_068;
    uint32_t reserved_06c;
    uint32_t reserved_070;
    uint32_t reserved_074;
    uint32_t reserved_078;
    uint32_t reserved_07c;
    uint32_t reserved_080;
    uint32_t reserved_084;
    uint32_t reserved_088;
    uint32_t reserved_08c;
    uint32_t reserved_090;
    uint32_t reserved_094;
    uint32_t reserved_098;
    uint32_t reserved_09c;
    uint32_t reserved_0a0;
    uint32_t reserved_0a4;
    uint32_t reserved_0a8;
    uint32_t reserved_0ac;
    uint32_t reserved_0b0;
    uint32_t reserved_0b4;
    uint32_t reserved_0b8;
    uint32_t reserved_0bc;
    uint32_t reserved_0c0;
    uint32_t reserved_0c4;
    uint32_t reserved_0c8;
    uint32_t reserved_0cc;
    uint32_t reserved_0d0;
    uint32_t reserved_0d4;
    uint32_t reserved_0d8;
    uint32_t reserved_0dc;
    uint32_t reserved_0e0;
    uint32_t reserved_0e4;
    uint32_t reserved_0e8;
    uint32_t reserved_0ec;
    uint32_t reserved_0f0;
    uint32_t reserved_0f4;
    uint32_t reserved_0f8;
    uint32_t reserved_0fc;
    uint32_t reserved_100;
    uint32_t reserved_104;
    uint32_t reserved_108;
    uint32_t reserved_10c;
    uint32_t reserved_110;
    uint32_t reserved_114;
    uint32_t reserved_118;
    uint32_t reserved_11c;
    uint32_t reserved_120;
    uint32_t reserved_124;
    uint32_t reserved_128;
    uint32_t reserved_12c;
    uint32_t reserved_130;
    uint32_t reserved_134;
    uint32_t reserved_138;
    uint32_t reserved_13c;
    uint32_t reserved_140;
    uint32_t reserved_144;
    uint32_t reserved_148;
    uint32_t reserved_14c;
    uint32_t reserved_150;
    uint32_t reserved_154;
    uint32_t reserved_158;
    uint32_t reserved_15c;
    uint32_t reserved_160;
    uint32_t reserved_164;
    uint32_t reserved_168;
    uint32_t reserved_16c;
    uint32_t reserved_170;
    uint32_t reserved_174;
    uint32_t reserved_178;
    uint32_t reserved_17c;
    uint32_t reserved_180;
    uint32_t reserved_184;
    uint32_t reserved_188;
    uint32_t reserved_18c;
    uint32_t reserved_190;
    uint32_t reserved_194;
    uint32_t reserved_198;
    uint32_t reserved_19c;
    uint32_t reserved_1a0;
    uint32_t reserved_1a4;
    uint32_t reserved_1a8;
    uint32_t reserved_1ac;
    uint32_t reserved_1b0;
    uint32_t reserved_1b4;
    uint32_t reserved_1b8;
    uint32_t reserved_1bc;
    uint32_t reserved_1c0;
    uint32_t reserved_1c4;
    uint32_t reserved_1c8;
    uint32_t reserved_1cc;
    uint32_t reserved_1d0;
    uint32_t reserved_1d4;
    uint32_t reserved_1d8;
    uint32_t reserved_1dc;
    uint32_t reserved_1e0;
    uint32_t reserved_1e4;
    uint32_t reserved_1e8;
    uint32_t reserved_1ec;
    uint32_t reserved_1f0;
    uint32_t reserved_1f4;
    uint32_t reserved_1f8;
    uint32_t reserved_1fc;
    uint32_t reserved_200;
    uint32_t reserved_204;
    uint32_t reserved_208;
    uint32_t reserved_20c;
    uint32_t reserved_210;
    uint32_t reserved_214;
    uint32_t reserved_218;
    uint32_t reserved_21c;
    uint32_t reserved_220;
    uint32_t reserved_224;
    uint32_t reserved_228;
    uint32_t reserved_22c;
    uint32_t reserved_230;
    uint32_t reserved_234;
    uint32_t reserved_238;
    uint32_t reserved_23c;
    uint32_t reserved_240;
    uint32_t reserved_244;
    uint32_t reserved_248;
    uint32_t reserved_24c;
    uint32_t reserved_250;
    uint32_t reserved_254;
    uint32_t reserved_258;
    uint32_t reserved_25c;
    uint32_t reserved_260;
    uint32_t reserved_264;
    uint32_t reserved_268;
    uint32_t reserved_26c;
    uint32_t reserved_270;
    uint32_t reserved_274;
    uint32_t reserved_278;
    uint32_t reserved_27c;
    uint32_t reserved_280;
    uint32_t reserved_284;
    uint32_t reserved_288;
    uint32_t reserved_28c;
    uint32_t reserved_290;
    uint32_t reserved_294;
    uint32_t reserved_298;
    uint32_t reserved_29c;
    uint32_t reserved_2a0;
    uint32_t reserved_2a4;
    uint32_t reserved_2a8;
    uint32_t reserved_2ac;
    uint32_t reserved_2b0;
    uint32_t reserved_2b4;
    uint32_t reserved_2b8;
    uint32_t reserved_2bc;
    uint32_t reserved_2c0;
    uint32_t reserved_2c4;
    uint32_t reserved_2c8;
    uint32_t reserved_2cc;
    uint32_t reserved_2d0;
    uint32_t reserved_2d4;
    uint32_t reserved_2d8;
    uint32_t reserved_2dc;
    uint32_t reserved_2e0;
    uint32_t reserved_2e4;
    uint32_t reserved_2e8;
    uint32_t reserved_2ec;
    uint32_t reserved_2f0;
    uint32_t reserved_2f4;
    uint32_t reserved_2f8;
    uint32_t reserved_2fc;
    uint32_t reserved_300;
    uint32_t reserved_304;
    uint32_t reserved_308;
    uint32_t reserved_30c;
    uint32_t reserved_310;
    uint32_t reserved_314;
    uint32_t reserved_318;
    uint32_t reserved_31c;
    uint32_t reserved_320;
    uint32_t reserved_324;
    uint32_t reserved_328;
    uint32_t reserved_32c;
    uint32_t reserved_330;
    uint32_t reserved_334;
    uint32_t reserved_338;
    uint32_t reserved_33c;
    uint32_t reserved_340;
    uint32_t reserved_344;
    uint32_t reserved_348;
    uint32_t reserved_34c;
    uint32_t reserved_350;
    uint32_t reserved_354;
    uint32_t reserved_358;
    uint32_t reserved_35c;
    uint32_t reserved_360;
    uint32_t reserved_364;
    uint32_t reserved_368;
    uint32_t reserved_36c;
    uint32_t reserved_370;
    uint32_t reserved_374;
    uint32_t reserved_378;
    uint32_t reserved_37c;
    uint32_t reserved_380;
    uint32_t reserved_384;
    uint32_t reserved_388;
    uint32_t reserved_38c;
    uint32_t reserved_390;
    uint32_t reserved_394;
    uint32_t reserved_398;
    uint32_t reserved_39c;
    uint32_t reserved_3a0;
    uint32_t reserved_3a4;
    uint32_t reserved_3a8;
    uint32_t reserved_3ac;
    uint32_t reserved_3b0;
    uint32_t reserved_3b4;
    uint32_t reserved_3b8;
    uint32_t reserved_3bc;
    uint32_t reserved_3c0;
    uint32_t reserved_3c4;
    uint32_t reserved_3c8;
    uint32_t reserved_3cc;
    uint32_t reserved_3d0;
    uint32_t reserved_3d4;
    uint32_t reserved_3d8;
    uint32_t reserved_3dc;
    uint32_t reserved_3e0;
    uint32_t reserved_3e4;
    uint32_t reserved_3e8;
    uint32_t reserved_3ec;
    uint32_t reserved_3f0;
    uint32_t reserved_3f4;
    uint32_t reserved_3f8;
    volatile usb_wrap_date_reg_t date;
} usb_wrap_dev_t;

#ifndef __cplusplus
_Static_assert(sizeof(usb_wrap_dev_t)==0x400, "Invalid USB_WRAP size");
#endif

extern usb_wrap_dev_t USB_WRAP;

#ifdef __cplusplus
}
#endif
