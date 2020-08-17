// Copyright 2017-2020 Espressif Systems (Shanghai) PTE LTD
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
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef volatile struct {
    uint32_t reserved_0;
    uint32_t reserved_4;
    uint32_t reserved_8;
    uint32_t reserved_c;
    uint32_t reserved_10;
    uint32_t reserved_14;
    uint32_t reserved_18;
    uint32_t reserved_1c;
    uint32_t reserved_20;
    uint32_t reserved_24;
    uint32_t reserved_28;
    uint32_t reserved_2c;
    uint32_t reserved_30;
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
    uint32_t reserved_7c;
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
    union {
        struct {
            uint32_t rd_mac_spi_8m_err_num: 3;     /*The value of this signal means the number of error bytes.*/
            uint32_t rd_mac_spi_8m_fail:    1;     /*0: Means no failure and that the data of MAC_SPI_8M is reliable 1: Means that programming user data failed and the number of error bytes is over 6.*/
            uint32_t rd_sys_part1_num:      3;     /*The value of this signal means the number of error bytes.*/
            uint32_t rd_sys_part1_fail:     1;     /*0: Means no failure and that the data of system part1 is reliable 1: Means that programming user data failed and the number of error bytes is over 6.*/
            uint32_t rd_usr_data_err_num:   3;     /*The value of this signal means the number of error bytes.*/
            uint32_t rd_usr_data_fail:      1;     /*0: Means no failure and that the user data is reliable 1: Means that programming user data failed and the number of error bytes is over 6.*/
            uint32_t rd_key0_err_num:       3;     /*The value of this signal means the number of error bytes.*/
            uint32_t rd_key0_fail:          1;     /*0: Means no failure and that the data of key$n is reliable 1: Means that programming key$n failed and the number of error bytes is over 6.*/
            uint32_t rd_key1_err_num:       3;     /*The value of this signal means the number of error bytes.*/
            uint32_t rd_key1_fail:          1;     /*0: Means no failure and that the data of key$n is reliable 1: Means that programming key$n failed and the number of error bytes is over 6.*/
            uint32_t rd_key2_err_num:       3;     /*The value of this signal means the number of error bytes.*/
            uint32_t rd_key2_fail:          1;     /*0: Means no failure and that the data of key$n is reliable 1: Means that programming key$n failed and the number of error bytes is over 6.*/
            uint32_t rd_key3_err_num:       3;     /*The value of this signal means the number of error bytes.*/
            uint32_t rd_key3_fail:          1;     /*0: Means no failure and that the data of key$n is reliable 1: Means that programming key$n failed and the number of error bytes is over 6.*/
            uint32_t rd_key4_err_num:       3;     /*The value of this signal means the number of error bytes.*/
            uint32_t rd_key4_fail:          1;     /*0: Means no failure and that the data of key$n is reliable 1: Means that programming key$n failed and the number of error bytes is over 6.*/
        };
        uint32_t val;
    } rd_rs_err0;
    union {
        struct {
            uint32_t rd_key5_err_num:   3;         /*The value of this signal means the number of error bytes.*/
            uint32_t rd_key5_fail:      1;         /*0: Means no failure and that the data of KEY5 is reliable 1: Means that programming user data failed and the number of error bytes is over 6.*/
            uint32_t rd_sys_part2_num:  3;         /*The value of this signal means the number of error bytes.*/
            uint32_t rd_sys_part2_fail: 1;         /*0: Means no failure and that the data of system part2 is reliable 1: Means that programming user data failed and the number of error bytes is over 6.*/
            uint32_t reserved8:        24;         /*Reserved.*/
        };
        uint32_t val;
    } rd_rs_err1;
    union {
        struct {
            uint32_t mem_force_pd:     1;          /*Set this bit to force eFuse SRAM into power-saving mode.*/
            uint32_t mem_clk_force_on: 1;          /*Set this bit and force to activate clock signal of eFuse SRAM.*/
            uint32_t mem_force_pu:     1;          /*Set this bit to force eFuse SRAM into working mode.*/
            uint32_t reserved3:       13;          /*Reserved.*/
            uint32_t clk_en:           1;          /*Set this bit and force to enable clock signal of eFuse memory.*/
            uint32_t reserved17:      15;          /*Reserved.*/
        };
        uint32_t val;
    } clk;
    union {
        struct {
            uint32_t op_code:   16;                /*0x5A5A: Operate programming command 0x5AA5: Operate read command.*/
            uint32_t reserved16: 16;               /*Reserved.*/
        };
        uint32_t val;
    } conf;
    union {
        struct {
            uint32_t state:            4;          /*Indicates the state of the eFuse state machine.*/
            uint32_t otp_load_sw:      1;          /*The value of OTP_LOAD_SW.*/
            uint32_t otp_vddq_c_sync2: 1;          /*The value of OTP_VDDQ_C_SYNC2.*/
            uint32_t otp_strobe_sw:    1;          /*The value of OTP_STROBE_SW.*/
            uint32_t otp_csb_sw:       1;          /*The value of OTP_CSB_SW.*/
            uint32_t otp_pgenb_sw:     1;          /*The value of OTP_PGENB_SW.*/
            uint32_t otp_vddq_is_sw:   1;          /*The value of OTP_VDDQ_IS_SW.*/
            uint32_t repeat_err_cnt:   8;          /*Indicates the number of error bits during programming BLOCK0.*/
            uint32_t reserved18:      14;          /*Reserved.*/
        };
        uint32_t val;
    } status;
    union {
        struct {
            uint32_t read_cmd:   1;                /*Set this bit to send read command.*/
            uint32_t pgm_cmd:    1;                /*Set this bit to send programming command.*/
            uint32_t blk_num:    4;                /*The serial number of the block to be programmed. Value 0-10 corresponds to block number 0-10  respectively.*/
            uint32_t reserved6: 26;                /*Reserved.*/
        };
        uint32_t val;
    } cmd;
    union {
        struct {
            uint32_t read_done:         1;         /*The raw bit signal for read_done interrupt.*/
            uint32_t pgm_done:          1;         /*The raw bit signal for pgm_done interrupt.*/
            uint32_t reserved2:        30;         /*Reserved.*/
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t read_done:        1;          /*The status signal for read_done interrupt.*/
            uint32_t pgm_done:         1;          /*The status signal for pgm_done interrupt.*/
            uint32_t reserved2:       30;          /*Reserved.*/
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t read_done:         1;         /*The enable signal for read_done interrupt.*/
            uint32_t pgm_done:          1;         /*The enable signal for pgm_done interrupt.*/
            uint32_t reserved2:        30;         /*Reserved.*/
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t read_done:         1;         /*The clear signal for read_done interrupt.*/
            uint32_t pgm_done:          1;         /*The clear signal for pgm_done interrupt.*/
            uint32_t reserved2:        30;         /*Reserved.*/
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t dac_clk_div:     8;           /*Controls the division factor of the rising clock of the programming voltage.*/
            uint32_t dac_clk_pad_sel: 1;           /*Don't care.*/
            uint32_t dac_num:         8;           /*Controls the rising period of the programming voltage.*/
            uint32_t oe_clr:          1;           /*Reduces the power supply of the programming voltage.*/
            uint32_t reserved18:     14;           /*Reserved.*/
        };
        uint32_t val;
    } dac_conf;
    union {
        struct {
            uint32_t thr_a:         8;             /*Configures the hold time of read operation.*/
            uint32_t trd:           8;             /*Configures the length of pulse of read operation.*/
            uint32_t tsur_a:        8;             /*Configures the setup time of read operation.*/
            uint32_t read_init_num: 8;             /*Configures the initial read time of eFuse.*/
        };
        uint32_t val;
    } rd_tim_conf;
    union {
        struct {
            uint32_t thp_a:         8;             /*Configures the hold time of programming operation.*/
            uint32_t tpgm_inactive: 8;             /*Configures the length of pulse during programming 0 to eFuse.*/
            uint32_t tpgm:         16;             /*Configures the length of pulse during programming 1 to eFuse.*/
        };
        uint32_t val;
    } wr_tim_conf0;
    union {
        struct {
            uint32_t tsup_a:     8;                /*Configures the setup time of programming operation.*/
            uint32_t pwr_on_num: 16;               /*Configures the power up time for VDDQ.*/
            uint32_t reserved24: 8;                /*Reserved.*/
        };
        uint32_t val;
    } wr_tim_conf1;
    union {
        struct {
            uint32_t pwr_off_num: 16;              /*Configures the power outage time for VDDQ.*/
            uint32_t reserved16: 16;               /*Reserved.*/
        };
        uint32_t val;
    } wr_tim_conf2;
    union {
        struct {
            uint32_t date:      28;                /*Stores eFuse version.*/
            uint32_t reserved28: 4;                /*Reserved.*/
        };
        uint32_t val;
    } date;
} efuse_dev_t;

extern efuse_dev_t EFUSE;

#ifdef __cplusplus
}
#endif
