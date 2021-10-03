// Copyright 2017-2021 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _SOC_WORLD_CONTROLLER_STRUCT_H_
#define _SOC_WORLD_CONTROLLER_STRUCT_H_


#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct world_controller_dev_s {
    uint32_t wcl_core_0_entry_1_addr;
    uint32_t wcl_core_0_entry_2_addr;
    uint32_t wcl_core_0_entry_3_addr;
    uint32_t wcl_core_0_entry_4_addr;
    uint32_t wcl_core_0_entry_5_addr;
    uint32_t wcl_core_0_entry_6_addr;
    uint32_t wcl_core_0_entry_7_addr;
    uint32_t wcl_core_0_entry_8_addr;
    uint32_t wcl_core_0_entry_9_addr;
    uint32_t wcl_core_0_entry_10_addr;
    uint32_t wcl_core_0_entry_11_addr;
    uint32_t wcl_core_0_entry_12_addr;
    uint32_t wcl_core_0_entry_13_addr;
    uint32_t reserved_34;
    uint32_t reserved_38;
    uint32_t reserved_3c;
    uint32_t reserved_40;
    uint32_t reserved_44;
    uint32_t reserved_48;
    uint32_t reserved_4c;
    uint32_t reserved_50;
    uint32_t reserved_54;
    uint32_t reserved_58;
    uint32_t reserved_5c;
    uint32_t reserved_60;
    uint32_t reserved_64;
    uint32_t reserved_68;
    uint32_t reserved_6c;
    uint32_t reserved_70;
    uint32_t reserved_74;
    uint32_t reserved_78;
    union {
        struct {
            uint32_t reserved0                     :    1;
            uint32_t reg_core_0_entry_check        :    13;  /*This filed is used to enable entry address check */
            uint32_t reserved14                    :    18;  /*reserved*/
        };
        uint32_t val;
    } wcl_core_0_entry_check;
    uint32_t reserved_80;
    uint32_t reserved_84;
    uint32_t reserved_88;
    uint32_t reserved_8c;
    uint32_t reserved_90;
    uint32_t reserved_94;
    uint32_t reserved_98;
    uint32_t reserved_9c;
    uint32_t reserved_a0;
    uint32_t reserved_a4;
    uint32_t reserved_a8;
    uint32_t reserved_ac;
    uint32_t reserved_b0;
    uint32_t reserved_b4;
    uint32_t reserved_b8;
    uint32_t reserved_bc;
    uint32_t reserved_c0;
    uint32_t reserved_c4;
    uint32_t reserved_c8;
    uint32_t reserved_cc;
    uint32_t reserved_d0;
    uint32_t reserved_d4;
    uint32_t reserved_d8;
    uint32_t reserved_dc;
    uint32_t reserved_e0;
    uint32_t reserved_e4;
    uint32_t reserved_e8;
    uint32_t reserved_ec;
    uint32_t reserved_f0;
    uint32_t reserved_f4;
    uint32_t reserved_f8;
    uint32_t reserved_fc;
    uint32_t wcl_core_0_message_addr;
    union {
        struct {
            uint32_t reg_core_0_message_max        :    4;  /*This filed is used to set the max value of clear write_buffer*/
            uint32_t reserved4                     :    28;
        };
        uint32_t val;
    } wcl_core_0_message_max;
    union {
        struct {
            uint32_t reg_core_0_from_world_1       :    1;  /*This bit is used to confirm world before enter entry 1 */
            uint32_t reg_core_0_from_entry_1       :    4;  /*This filed is used to confirm in which entry before enter entry 1*/
            uint32_t reg_core_0_current_1          :    1;  /*This bit is used to confirm whether the current state is in entry 1 */
            uint32_t reserved6                     :    26;
        };
        uint32_t val;
    } wcl_core_0_statustable1;
    union {
        struct {
            uint32_t reg_core_0_from_world_2       :    1;  /*This bit is used to confirm world before enter entry 2 */
            uint32_t reg_core_0_from_entry_2       :    4;  /*This filed is used to confirm in which entry before enter entry 2*/
            uint32_t reg_core_0_current_2          :    1;  /*This bit is used to confirm whether the current state is in entry 2 */
            uint32_t reserved6                     :    26;
        };
        uint32_t val;
    } wcl_core_0_statustable2;
    union {
        struct {
            uint32_t reg_core_0_from_world_3       :    1;  /*This bit is used to confirm world before enter entry 3 */
            uint32_t reg_core_0_from_entry_3       :    4;  /*This filed is used to confirm in which entry before enter entry 3*/
            uint32_t reg_core_0_current_3          :    1;  /*This bit is used to confirm whether the current state is in entry 3 */
            uint32_t reserved6                     :    26;
        };
        uint32_t val;
    } wcl_core_0_statustable3;
    union {
        struct {
            uint32_t reg_core_0_from_world_4       :    1;  /*This bit is used to confirm world before enter entry 4 */
            uint32_t reg_core_0_from_entry_4       :    4;  /*This filed is used to confirm in which entry before enter entry 4*/
            uint32_t reg_core_0_current_4          :    1;  /*This bit is used to confirm whether the current state is in entry 4 */
            uint32_t reserved6                     :    26;
        };
        uint32_t val;
    } wcl_core_0_statustable4;
    union {
        struct {
            uint32_t reg_core_0_from_world_5       :    1;  /*This bit is used to confirm world before enter entry 5 */
            uint32_t reg_core_0_from_entry_5       :    4;  /*This filed is used to confirm in which entry before enter entry 5*/
            uint32_t reg_core_0_current_5          :    1;  /*This bit is used to confirm whether the current state is in entry 5 */
            uint32_t reserved6                     :    26;
        };
        uint32_t val;
    } wcl_core_0_statustable5;
    union {
        struct {
            uint32_t reg_core_0_from_world_6       :    1;  /*This bit is used to confirm world before enter entry 6 */
            uint32_t reg_core_0_from_entry_6       :    4;  /*This filed is used to confirm in which entry before enter entry 6*/
            uint32_t reg_core_0_current_6          :    1;  /*This bit is used to confirm whether the current state is in entry 6 */
            uint32_t reserved6                     :    26;
        };
        uint32_t val;
    } wcl_core_0_statustable6;
    union {
        struct {
            uint32_t reg_core_0_from_world_7       :    1;  /*This bit is used to confirm world before enter entry 7 */
            uint32_t reg_core_0_from_entry_7       :    4;  /*This filed is used to confirm in which entry before enter entry 7*/
            uint32_t reg_core_0_current_7          :    1;  /*This bit is used to confirm whether the current state is in entry 7 */
            uint32_t reserved6                     :    26;
        };
        uint32_t val;
    } wcl_core_0_statustable7;
    union {
        struct {
            uint32_t reg_core_0_from_world_8       :    1;  /*This bit is used to confirm world before enter entry 8 */
            uint32_t reg_core_0_from_entry_8       :    4;  /*This filed is used to confirm in which entry before enter entry 8*/
            uint32_t reg_core_0_current_8          :    1;  /*This bit is used to confirm whether the current state is in entry 8 */
            uint32_t reserved6                     :    26;
        };
        uint32_t val;
    } wcl_core_0_statustable8;
    union {
        struct {
            uint32_t reg_core_0_from_world_9       :    1;  /*This bit is used to confirm world before enter entry 9 */
            uint32_t reg_core_0_from_entry_9       :    4;  /*This filed is used to confirm in which entry before enter entry 9*/
            uint32_t reg_core_0_current_9          :    1;  /*This bit is used to confirm whether the current state is in entry 9 */
            uint32_t reserved6                     :    26;
        };
        uint32_t val;
    } wcl_core_0_statustable9;
    union {
        struct {
            uint32_t reg_core_0_from_world_10      :    1;  /*This bit is used to confirm world before enter entry 10 */
            uint32_t reg_core_0_from_entry_10      :    4;  /*This filed is used to confirm in which entry before enter entry 10*/
            uint32_t reg_core_0_current_10         :    1;  /*This bit is used to confirm whether the current state is in entry 10 */
            uint32_t reserved6                     :    26;
        };
        uint32_t val;
    } wcl_core_0_statustable10;
    union {
        struct {
            uint32_t reg_core_0_from_world_11      :    1;  /*This bit is used to confirm world before enter entry 11 */
            uint32_t reg_core_0_from_entry_11      :    4;  /*This filed is used to confirm in which entry before enter entry 11*/
            uint32_t reg_core_0_current_11         :    1;  /*This bit is used to confirm whether the current state is in entry 11 */
            uint32_t reserved6                     :    26;
        };
        uint32_t val;
    } wcl_core_0_statustable11;
    union {
        struct {
            uint32_t core_0_from_world_12          :    1;  /*This bit is used to confirm world before enter entry 12 */
            uint32_t core_0_from_entry_12          :    4;  /*This filed is used to confirm in which entry before enter entry 12*/
            uint32_t core_0_current_12             :    1;  /*This bit is used to confirm whether the current state is in entry 12 */
            uint32_t reserved6                     :    26;
        };
        uint32_t val;
    } wcl_core_0_statustable12;
    union {
        struct {
            uint32_t reg_core_0_from_world_13      :    1;  /*This bit is used to confirm world before enter entry 13 */
            uint32_t reg_core_0_from_entry_13      :    4;  /*This filed is used to confirm in which entry before enter entry 13*/
            uint32_t reg_core_0_current_13         :    1;  /*This bit is used to confirm whether the current state is in entry 13 */
            uint32_t reserved6                     :    26;
        };
        uint32_t val;
    } wcl_core_0_statustable13;
    uint32_t reserved_b4;
    uint32_t reserved_b8;
    uint32_t reserved_bc;
    uint32_t reserved_c0;
    uint32_t reserved_c4;
    uint32_t reserved_c8;
    uint32_t reserved_cc;
    uint32_t reserved_d0;
    uint32_t reserved_d4;
    uint32_t reserved_d8;
    uint32_t reserved_dc;
    uint32_t reserved_e0;
    uint32_t reserved_e4;
    uint32_t reserved_e8;
    uint32_t reserved_ec;
    uint32_t reserved_f0;
    uint32_t reserved_f4;
    uint32_t reserved_f8;
    union {
        struct {
            uint32_t reserved0                     :    1;
            uint32_t reg_core_0_statustable_current:    13;  /*This field is used to quickly read and rewrite the current field of all STATUSTABLE registers.For example*/
            uint32_t reserved14                    :    18;
        };
        uint32_t val;
    } wcl_core_0_statustable_current;
    uint32_t reserved_100;
    uint32_t reserved_104;
    union {
        struct {
            uint32_t reg_core_0_message_match      :    1;  /*This bit indicates whether the check is successful*/
            uint32_t reg_core_0_message_expect     :    4;  /*This field indicates the data to be written next time*/
            uint32_t reg_core_0_message_dataphase  :    1;  /*If this bit is 1*/
            uint32_t reg_core_0_message_addressphase:    1;  /*If this bit is 1*/
            uint32_t reserved7                     :    25;
        };
        uint32_t val;
    } wcl_core_0_message_phase;
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
    uint32_t wcl_core_0_world_trigger_addr;
    union {
        struct {
            uint32_t reg_core_0_world_prepare      :    2;  /*This field to used to set world to enter*/
            uint32_t reserved2                     :    30;
        };
        uint32_t val;
    } wcl_core_0_world_prepare;
    uint32_t wcl_core_0_world_update;
    uint32_t wcl_core_0_world_cancel;
    union {
        struct {
            uint32_t reg_core_0_world_iram0        :    2;  /*this field is used to read current world of Iram0 bus*/
            uint32_t reserved2                     :    30;
        };
        uint32_t val;
    } wcl_core_0_world_iram0;
    union {
        struct {
            uint32_t reg_core_0_world_dram0_pif    :    2;  /*this field is used to read current world of Dram0 bus and PIF bus*/
            uint32_t reserved2                     :    30;
        };
        uint32_t val;
    } wcl_core_0_world_dram0_pif;
    union {
        struct {
            uint32_t reg_core_0_world_phase        :    1;  /*This bit indicates whether is preparing to switch to WORLD1*/
            uint32_t reserved1                     :    31;
        };
        uint32_t val;
    } wcl_core_0_world_phase;
    uint32_t reserved_15c;
    uint32_t reserved_160;
    uint32_t reserved_164;
    uint32_t reserved_168;
    uint32_t reserved_16c;
    uint32_t reserved_170;
    uint32_t reserved_174;
    uint32_t reserved_178;
    uint32_t reserved_17c;
    uint32_t wcl_core_0_nmi_mask_enable;
    uint32_t wcl_core_0_nmi_mask_trigger_addr;
    uint32_t wcl_core_0_nmi_mask_disable;
    uint32_t wcl_core_0_nmi_mask_cancle;
    union {
        struct {
            uint32_t reg_core_0_nmi_mask           :    1;  /*this bit is used to mask NMI interrupt*/
            uint32_t reserved1                     :    31;
        };
        uint32_t val;
    } wcl_core_0_nmi_mask;
    union {
        struct {
            uint32_t reg_core_0_nmi_mask_phase     :    1;  /*this bit is used to indicates whether the NMI interrupt is being masked*/
            uint32_t reserved1                     :    31;
        };
        uint32_t val;
    } wcl_core_0_nmi_mask_phase;
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
    uint32_t reserved_3fc;
    uint32_t wcl_core_1_entry_1_addr;
    uint32_t wcl_core_1_entry_2_addr;
    uint32_t wcl_core_1_entry_3_addr;
    uint32_t wcl_core_1_entry_4_addr;
    uint32_t wcl_core_1_entry_5_addr;
    uint32_t wcl_core_1_entry_6_addr;
    uint32_t wcl_core_1_entry_7_addr;
    uint32_t wcl_core_1_entry_8_addr;
    uint32_t wcl_core_1_entry_9_addr;
    uint32_t wcl_core_1_entry_10_addr;
    uint32_t wcl_core_1_entry_11_addr;
    uint32_t wcl_core_1_entry_12_addr;
    uint32_t wcl_core_1_entry_13_addr;
    uint32_t reserved_434;
    uint32_t reserved_438;
    uint32_t reserved_43c;
    uint32_t reserved_440;
    uint32_t reserved_444;
    uint32_t reserved_448;
    uint32_t reserved_44c;
    uint32_t reserved_450;
    uint32_t reserved_454;
    uint32_t reserved_458;
    uint32_t reserved_45c;
    uint32_t reserved_460;
    uint32_t reserved_464;
    uint32_t reserved_468;
    uint32_t reserved_46c;
    uint32_t reserved_470;
    uint32_t reserved_474;
    uint32_t reserved_478;
    union {
        struct {
            uint32_t reserved0                     :    1;
            uint32_t reg_core_1_entry_check        :    13;  /*This filed is used to enable entry address check */
            uint32_t reserved14                    :    18;  /*reserved*/
        };
        uint32_t val;
    } wcl_core_1_entry_check;
    uint32_t reserved_480;
    uint32_t reserved_484;
    uint32_t reserved_488;
    uint32_t reserved_48c;
    uint32_t reserved_490;
    uint32_t reserved_494;
    uint32_t reserved_498;
    uint32_t reserved_49c;
    uint32_t reserved_4a0;
    uint32_t reserved_4a4;
    uint32_t reserved_4a8;
    uint32_t reserved_4ac;
    uint32_t reserved_4b0;
    uint32_t reserved_4b4;
    uint32_t reserved_4b8;
    uint32_t reserved_4bc;
    uint32_t reserved_4c0;
    uint32_t reserved_4c4;
    uint32_t reserved_4c8;
    uint32_t reserved_4cc;
    uint32_t reserved_4d0;
    uint32_t reserved_4d4;
    uint32_t reserved_4d8;
    uint32_t reserved_4dc;
    uint32_t reserved_4e0;
    uint32_t reserved_4e4;
    uint32_t reserved_4e8;
    uint32_t reserved_4ec;
    uint32_t reserved_4f0;
    uint32_t reserved_4f4;
    uint32_t reserved_4f8;
    uint32_t reserved_4fc;
    uint32_t wcl_core_1_message_addr;
    union {
        struct {
            uint32_t reg_core_1_message_max        :    4;  /*This filed is used to set the max value of clear write_buffer*/
            uint32_t reserved4                     :    28;
        };
        uint32_t val;
    } wcl_core_1_message_max;
    union {
        struct {
            uint32_t reg_core_1_from_world_1       :    1;  /*This bit is used to confirm world before enter entry 1 */
            uint32_t reg_core_1_from_entry_1       :    4;  /*This filed is used to confirm in which entry before enter entry 1*/
            uint32_t reg_core_1_current_1          :    1;  /*This bit is used to confirm whether the current state is in entry 1 */
            uint32_t reserved6                     :    26;
        };
        uint32_t val;
    } wcl_core_1_statustable1;
    union {
        struct {
            uint32_t reg_core_1_from_world_2       :    1;  /*This bit is used to confirm world before enter entry 2 */
            uint32_t reg_core_1_from_entry_2       :    4;  /*This filed is used to confirm in which entry before enter entry 2*/
            uint32_t reg_core_1_current_2          :    1;  /*This bit is used to confirm whether the current state is in entry 2 */
            uint32_t reserved6                     :    26;
        };
        uint32_t val;
    } wcl_core_1_statustable2;
    union {
        struct {
            uint32_t reg_core_1_from_world_3       :    1;  /*This bit is used to confirm world before enter entry 3 */
            uint32_t reg_core_1_from_entry_3       :    4;  /*This filed is used to confirm in which entry before enter entry 3*/
            uint32_t reg_core_1_current_3          :    1;  /*This bit is used to confirm whether the current state is in entry 3 */
            uint32_t reserved6                     :    26;
        };
        uint32_t val;
    } wcl_core_1_statustable3;
    union {
        struct {
            uint32_t reg_core_1_from_world_4       :    1;  /*This bit is used to confirm world before enter entry 4 */
            uint32_t reg_core_1_from_entry_4       :    4;  /*This filed is used to confirm in which entry before enter entry 4*/
            uint32_t reg_core_1_current_4          :    1;  /*This bit is used to confirm whether the current state is in entry 4 */
            uint32_t reserved6                     :    26;
        };
        uint32_t val;
    } wcl_core_1_statustable4;
    union {
        struct {
            uint32_t reg_core_1_from_world_5       :    1;  /*This bit is used to confirm world before enter entry 5 */
            uint32_t reg_core_1_from_entry_5       :    4;  /*This filed is used to confirm in which entry before enter entry 5*/
            uint32_t reg_core_1_current_5          :    1;  /*This bit is used to confirm whether the current state is in entry 5 */
            uint32_t reserved6                     :    26;
        };
        uint32_t val;
    } wcl_core_1_statustable5;
    union {
        struct {
            uint32_t reg_core_1_from_world_6       :    1;  /*This bit is used to confirm world before enter entry 6 */
            uint32_t reg_core_1_from_entry_6       :    4;  /*This filed is used to confirm in which entry before enter entry 6*/
            uint32_t reg_core_1_current_6          :    1;  /*This bit is used to confirm whether the current state is in entry 6 */
            uint32_t reserved6                     :    26;
        };
        uint32_t val;
    } wcl_core_1_statustable6;
    union {
        struct {
            uint32_t reg_core_1_from_world_7       :    1;  /*This bit is used to confirm world before enter entry 7 */
            uint32_t reg_core_1_from_entry_7       :    4;  /*This filed is used to confirm in which entry before enter entry 7*/
            uint32_t reg_core_1_current_7          :    1;  /*This bit is used to confirm whether the current state is in entry 7 */
            uint32_t reserved6                     :    26;
        };
        uint32_t val;
    } wcl_core_1_statustable7;
    union {
        struct {
            uint32_t reg_core_1_from_world_8       :    1;  /*This bit is used to confirm world before enter entry 8 */
            uint32_t reg_core_1_from_entry_8       :    4;  /*This filed is used to confirm in which entry before enter entry 8*/
            uint32_t reg_core_1_current_8          :    1;  /*This bit is used to confirm whether the current state is in entry 8 */
            uint32_t reserved6                     :    26;
        };
        uint32_t val;
    } wcl_core_1_statustable8;
    union {
        struct {
            uint32_t reg_core_1_from_world_9       :    1;  /*This bit is used to confirm world before enter entry 9 */
            uint32_t reg_core_1_from_entry_9       :    4;  /*This filed is used to confirm in which entry before enter entry 9*/
            uint32_t reg_core_1_current_9          :    1;  /*This bit is used to confirm whether the current state is in entry 9 */
            uint32_t reserved6                     :    26;
        };
        uint32_t val;
    } wcl_core_1_statustable9;
    union {
        struct {
            uint32_t reg_core_1_from_world_10      :    1;  /*This bit is used to confirm world before enter entry 10 */
            uint32_t reg_core_1_from_entry_10      :    4;  /*This filed is used to confirm in which entry before enter entry 10*/
            uint32_t reg_core_1_current_10         :    1;  /*This bit is used to confirm whether the current state is in entry 10 */
            uint32_t reserved6                     :    26;
        };
        uint32_t val;
    } wcl_core_1_statustable10;
    union {
        struct {
            uint32_t reg_core_1_from_world_11      :    1;  /*This bit is used to confirm world before enter entry 11 */
            uint32_t reg_core_1_from_entry_11      :    4;  /*This filed is used to confirm in which entry before enter entry 11*/
            uint32_t reg_core_1_current_11         :    1;  /*This bit is used to confirm whether the current state is in entry 11 */
            uint32_t reserved6                     :    26;
        };
        uint32_t val;
    } wcl_core_1_statustable11;
    union {
        struct {
            uint32_t core_1_from_world_12          :    1;  /*This bit is used to confirm world before enter entry 12 */
            uint32_t core_1_from_entry_12          :    4;  /*This filed is used to confirm in which entry before enter entry 12*/
            uint32_t core_1_current_12             :    1;  /*This bit is used to confirm whether the current state is in entry 12 */
            uint32_t reserved6                     :    26;
        };
        uint32_t val;
    } wcl_core_1_statustable12;
    union {
        struct {
            uint32_t reg_core_1_from_world_13      :    1;  /*This bit is used to confirm world before enter entry 13 */
            uint32_t reg_core_1_from_entry_13      :    4;  /*This filed is used to confirm in which entry before enter entry 13*/
            uint32_t reg_core_1_current_13         :    1;  /*This bit is used to confirm whether the current state is in entry 13 */
            uint32_t reserved6                     :    26;
        };
        uint32_t val;
    } wcl_core_1_statustable13;
    uint32_t reserved_4b4;
    uint32_t reserved_4b8;
    uint32_t reserved_4bc;
    uint32_t reserved_4c0;
    uint32_t reserved_4c4;
    uint32_t reserved_4c8;
    uint32_t reserved_4cc;
    uint32_t reserved_4d0;
    uint32_t reserved_4d4;
    uint32_t reserved_4d8;
    uint32_t reserved_4dc;
    uint32_t reserved_4e0;
    uint32_t reserved_4e4;
    uint32_t reserved_4e8;
    uint32_t reserved_4ec;
    uint32_t reserved_4f0;
    uint32_t reserved_4f4;
    uint32_t reserved_4f8;
    union {
        struct {
            uint32_t reserved0                     :    1;
            uint32_t reg_core_1_statustable_current:    13;  /*This field is used to quickly read and rewrite the current field of all STATUSTABLE registers.For example*/
            uint32_t reserved14                    :    18;
        };
        uint32_t val;
    } wcl_core_1_statustable_current;
    uint32_t reserved_500;
    uint32_t reserved_504;
    union {
        struct {
            uint32_t reg_core_1_message_match      :    1;  /*This bit indicates whether the check is successful*/
            uint32_t reg_core_1_message_expect     :    4;  /*This field indicates the data to be written next time*/
            uint32_t reg_core_1_message_dataphase  :    1;  /*If this bit is 1*/
            uint32_t reg_core_1_message_addressphase:    1;  /*If this bit is 1*/
            uint32_t reserved7                     :    25;
        };
        uint32_t val;
    } wcl_core_1_message_phase;
    uint32_t reserved_50c;
    uint32_t reserved_510;
    uint32_t reserved_514;
    uint32_t reserved_518;
    uint32_t reserved_51c;
    uint32_t reserved_520;
    uint32_t reserved_524;
    uint32_t reserved_528;
    uint32_t reserved_52c;
    uint32_t reserved_530;
    uint32_t reserved_534;
    uint32_t reserved_538;
    uint32_t reserved_53c;
    uint32_t wcl_core_1_world_trigger_addr;
    union {
        struct {
            uint32_t reg_core_1_world_prepare      :    2;  /*This field to used to set world to enter*/
            uint32_t reserved2                     :    30;
        };
        uint32_t val;
    } wcl_core_1_world_prepare;
    uint32_t wcl_core_1_world_update;
    uint32_t wcl_core_1_world_cancel;
    union {
        struct {
            uint32_t reg_core_1_world_iram0        :    2;  /*this field is used to read current world of Iram0 bus*/
            uint32_t reserved2                     :    30;
        };
        uint32_t val;
    } wcl_core_1_world_iram0;
    union {
        struct {
            uint32_t reg_core_1_world_dram0_pif    :    2;  /*this field is used to read current world of Dram0 bus and PIF bus*/
            uint32_t reserved2                     :    30;
        };
        uint32_t val;
    } wcl_core_1_world_dram0_pif;
    union {
        struct {
            uint32_t reg_core_1_world_phase        :    1;  /*This bit indicates whether is preparing to switch to WORLD1*/
            uint32_t reserved1                     :    31;
        };
        uint32_t val;
    } wcl_core_1_world_phase;
    uint32_t reserved_55c;
    uint32_t reserved_560;
    uint32_t reserved_564;
    uint32_t reserved_568;
    uint32_t reserved_56c;
    uint32_t reserved_570;
    uint32_t reserved_574;
    uint32_t reserved_578;
    uint32_t reserved_57c;
    uint32_t wcl_core_1_nmi_mask_enable;
    uint32_t wcl_core_1_nmi_mask_trigger_addr;
    uint32_t wcl_core_1_nmi_mask_disable;
    uint32_t wcl_core_1_nmi_mask_cancle;
    union {
        struct {
            uint32_t reg_core_1_nmi_mask           :    1;  /*this bit is used to mask NMI interrupt*/
            uint32_t reserved1                     :    31;
        };
        uint32_t val;
    } wcl_core_1_nmi_mask;
    union {
        struct {
            uint32_t reg_core_1_nmi_mask_phase     :    1;  /*this bit is used to indicates whether the NMI interrupt is being masked*/
            uint32_t reserved1                     :    31;
        };
        uint32_t val;
    } wcl_core_1_nmi_mask_phase;
} world_controller_dev_t;
extern world_controller_dev_t WORLD_CONTROLLER;
#ifdef __cplusplus
}
#endif



#endif /*_SOC_WORLD_CONTROLLER_STRUCT_H_ */
