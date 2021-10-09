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
#ifndef _SOC_MCP_STRUCT_H_
#define _SOC_MCP_STRUCT_H_
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct mcp_dev_s {
    union {
        struct {
            uint32_t in_done:                  1;
            uint32_t in_suc_eof:               1;
            uint32_t out_done:                 1;
            uint32_t out_eof:                  1;
            uint32_t in_dscr_err:              1;
            uint32_t out_dscr_err:             1;
            uint32_t in_dscr_empty:            1;
            uint32_t out_total_eof:            1;
            uint32_t crc_done:                 1;
            uint32_t reserved9:               23;
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t in_done:                 1;
            uint32_t in_suc_eof:              1;
            uint32_t out_done:                1;
            uint32_t out_eof:                 1;
            uint32_t in_dscr_err:             1;
            uint32_t out_dscr_err:            1;
            uint32_t in_dscr_empty:           1;
            uint32_t out_total_eof:           1;
            uint32_t crc_done:                1;
            uint32_t reserved9:              23;
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t in_done:                  1;
            uint32_t in_suc_eof:               1;
            uint32_t out_done:                 1;
            uint32_t out_eof:                  1;
            uint32_t in_dscr_err:              1;
            uint32_t out_dscr_err:             1;
            uint32_t in_dscr_empty:            1;
            uint32_t out_total_eof:            1;
            uint32_t crc_done:                 1;
            uint32_t reserved9:               23;
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t in_done:                  1;
            uint32_t in_suc_eof:               1;
            uint32_t out_done:                 1;
            uint32_t out_eof:                  1;
            uint32_t in_dscr_err:              1;
            uint32_t out_dscr_err:             1;
            uint32_t in_dscr_empty:            1;
            uint32_t out_total_eof:            1;
            uint32_t crc_done:                 1;
            uint32_t reserved9:               23;
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t addr:              20;
            uint32_t reserved20:         8;
            uint32_t stop:               1;
            uint32_t start:              1;
            uint32_t restart:            1;
            uint32_t park:               1;
        };
        uint32_t val;
    } out_link;
    union {
        struct {
            uint32_t addr:             20;
            uint32_t reserved20:        8;
            uint32_t stop:              1;
            uint32_t start:             1;
            uint32_t restart:           1;
            uint32_t park:              1;
        };
        uint32_t val;
    } in_link;
    uint32_t out_eof_des_addr;                   /**/
    uint32_t in_eof_des_addr;                    /**/
    uint32_t out_eof_bfr_des_addr;               /**/
    uint32_t inlink_dscr;                        /**/
    uint32_t inlink_dscr_bf0;                    /**/
    uint32_t inlink_dscr_bf1;                    /**/
    uint32_t outlink_dscr;                       /**/
    uint32_t outlink_dscr_bf0;                   /**/
    uint32_t outlink_dscr_bf1;                   /**/
    union {
        struct {
            uint32_t in_rst:             1;         /*set this bit to reset in_inf state machine.*/
            uint32_t out_rst:            1;         /*set this bit to reset out_inf state machine.*/
            uint32_t cmdfifo_rst:        1;         /*set this bit to reset in_cmdfifo and out_cmdfifo.*/
            uint32_t fifo_rst:           1;
            uint32_t out_owner:          1;         /*This is used to configure the owner bit in OUT descriptor. This is effective only when you set reg_out_auto_wrback.*/
            uint32_t in_owner:           1;         /*This is used to configure the owner bit in IN descriptor.*/
            uint32_t out_auto_wrback:    1;         /*this bit is used to write back out descriptor when hardware has already used this descriptor.*/
            uint32_t check_owner:        1;         /*Set this bit to enable owner bit check in descriptor.*/
            uint32_t crc_cal_reset:      1;         /*Set this bit to reset crc calculation.*/
            uint32_t crc_cal_en:         1;         /*Set this bit enable crc calculation function.*/
            uint32_t crc_big_endian_en:  1;         /*Set this bit to reorder the bit of data which will be send to excute crc.*/
            uint32_t crc_out_reverse_en: 1;
            uint32_t reserved12:        19;
            uint32_t clk_en:             1;
        };
        uint32_t val;
    } conf;
    union {
        struct {
            uint32_t dscr_addr:       18;
            uint32_t dscr_state:       2;
            uint32_t state:            3;
            uint32_t fifo_empty:       1;
            uint32_t reserved24:       8;
        };
        uint32_t val;
    } in_st;
    union {
        struct {
            uint32_t dscr_addr:        18;
            uint32_t dscr_state:        2;
            uint32_t state:             3;
            uint32_t fifo_full:         1;
            uint32_t reserved24:        8;
        };
        uint32_t val;
    } out_st;
    uint32_t crc_out;                            /**/
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
    uint32_t date;                               /**/
} mcp_dev_t;
extern mcp_dev_t MCP;
#ifdef __cplusplus
}
#endif

#endif  /* _SOC_MCP_STRUCT_H_ */
