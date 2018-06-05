// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef _SOC_HOST_STRUCT_H_
#define _SOC_HOST_STRUCT_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct {
    uint32_t reserved_0;
    uint32_t reserved_4;
    uint32_t reserved_8;
    uint32_t reserved_c;
    union {
        struct {
            uint32_t reserved0:    24;
            uint32_t func2_int:     1;
            uint32_t reserved25:    7;
        };
        uint32_t val;
    } func2_0;
    union {
        struct {
            uint32_t func2_int_en:     1;
            uint32_t reserved1:       31;
        };
        uint32_t val;
    } func2_1;
    uint32_t reserved_18;
    uint32_t reserved_1c;
    union {
        struct {
            uint32_t func1_mdstat:     1;
            uint32_t reserved1:       31;
        };
        uint32_t val;
    } func2_2;
    uint32_t reserved_24;
    uint32_t reserved_28;
    uint32_t reserved_2c;
    uint32_t reserved_30;
    uint32_t gpio_status0;                                  /**/
    union {
        struct {
            uint32_t sdio_int1:      8;
            uint32_t reserved8:     24;
        };
        uint32_t val;
    } gpio_status1;
    uint32_t gpio_in0;                                      /**/
    union {
        struct {
            uint32_t sdio_in1:      8;
            uint32_t reserved8:    24;
        };
        uint32_t val;
    } gpio_in1;
    union {
        struct {
            uint32_t token0:          12;
            uint32_t rx_pf_valid:      1;
            uint32_t reserved13:       3;
            uint32_t reg_token1:      12;
            uint32_t rx_pf_eof:        4;
        };
        uint32_t val;
    } slc0_token_rdata;
    uint32_t slc0_pf;                                          /**/
    uint32_t slc1_pf;                                          /**/
    union {
        struct {
            uint32_t tohost_bit0:                1;
            uint32_t tohost_bit1:                1;
            uint32_t tohost_bit2:                1;
            uint32_t tohost_bit3:                1;
            uint32_t tohost_bit4:                1;
            uint32_t tohost_bit5:                1;
            uint32_t tohost_bit6:                1;
            uint32_t tohost_bit7:                1;
            uint32_t token0_1to0:                1;
            uint32_t token1_1to0:                1;
            uint32_t token0_0to1:                1;
            uint32_t token1_0to1:                1;
            uint32_t rx_sof:                     1;
            uint32_t rx_eof:                     1;
            uint32_t rx_start:                   1;
            uint32_t tx_start:                   1;
            uint32_t rx_udf:                     1;
            uint32_t tx_ovf:                     1;
            uint32_t rx_pf_valid:                1;
            uint32_t ext_bit0:                   1;
            uint32_t ext_bit1:                   1;
            uint32_t ext_bit2:                   1;
            uint32_t ext_bit3:                   1;
            uint32_t rx_new_packet:              1;
            uint32_t rd_retry:                   1;
            uint32_t gpio_sdio:                  1;
            uint32_t reserved26:                 6;
        };
        uint32_t val;
    } slc0_int_raw;
    union {
        struct {
            uint32_t tohost_bit0:                     1;
            uint32_t tohost_bit1:                     1;
            uint32_t tohost_bit2:                     1;
            uint32_t tohost_bit3:                     1;
            uint32_t tohost_bit4:                     1;
            uint32_t tohost_bit5:                     1;
            uint32_t tohost_bit6:                     1;
            uint32_t tohost_bit7:                     1;
            uint32_t token0_1to0:                     1;
            uint32_t token1_1to0:                     1;
            uint32_t token0_0to1:                     1;
            uint32_t token1_0to1:                     1;
            uint32_t rx_sof:                          1;
            uint32_t rx_eof:                          1;
            uint32_t rx_start:                        1;
            uint32_t tx_start:                        1;
            uint32_t rx_udf:                          1;
            uint32_t tx_ovf:                          1;
            uint32_t rx_pf_valid:                     1;
            uint32_t ext_bit0:                        1;
            uint32_t ext_bit1:                        1;
            uint32_t ext_bit2:                        1;
            uint32_t ext_bit3:                        1;
            uint32_t wifi_rx_new_packet:              1;
            uint32_t rd_retry:                        1;
            uint32_t bt_rx_new_packet:                1;
            uint32_t reserved26:                      6;
        };
        uint32_t val;
    } slc1_int_raw;
    union {
        struct {
            uint32_t tohost_bit0:               1;
            uint32_t tohost_bit1:               1;
            uint32_t tohost_bit2:               1;
            uint32_t tohost_bit3:               1;
            uint32_t tohost_bit4:               1;
            uint32_t tohost_bit5:               1;
            uint32_t tohost_bit6:               1;
            uint32_t tohost_bit7:               1;
            uint32_t token0_1to0:               1;
            uint32_t token1_1to0:               1;
            uint32_t token0_0to1:               1;
            uint32_t token1_0to1:               1;
            uint32_t rx_sof:                    1;
            uint32_t rx_eof:                    1;
            uint32_t rx_start:                  1;
            uint32_t tx_start:                  1;
            uint32_t rx_udf:                    1;
            uint32_t tx_ovf:                    1;
            uint32_t rx_pf_valid:               1;
            uint32_t ext_bit0:                  1;
            uint32_t ext_bit1:                  1;
            uint32_t ext_bit2:                  1;
            uint32_t ext_bit3:                  1;
            uint32_t rx_new_packet:             1;
            uint32_t rd_retry:                  1;
            uint32_t gpio_sdio:                 1;
            uint32_t reserved26:                6;
        };
        uint32_t val;
    } slc0_int_st;
    union {
        struct {
            uint32_t tohost_bit0:                    1;
            uint32_t tohost_bit1:                    1;
            uint32_t tohost_bit2:                    1;
            uint32_t tohost_bit3:                    1;
            uint32_t tohost_bit4:                    1;
            uint32_t tohost_bit5:                    1;
            uint32_t tohost_bit6:                    1;
            uint32_t tohost_bit7:                    1;
            uint32_t token0_1to0:                    1;
            uint32_t token1_1to0:                    1;
            uint32_t token0_0to1:                    1;
            uint32_t token1_0to1:                    1;
            uint32_t rx_sof:                         1;
            uint32_t rx_eof:                         1;
            uint32_t rx_start:                       1;
            uint32_t tx_start:                       1;
            uint32_t rx_udf:                         1;
            uint32_t tx_ovf:                         1;
            uint32_t rx_pf_valid:                    1;
            uint32_t ext_bit0:                       1;
            uint32_t ext_bit1:                       1;
            uint32_t ext_bit2:                       1;
            uint32_t ext_bit3:                       1;
            uint32_t wifi_rx_new_packet:             1;
            uint32_t rd_retry:                       1;
            uint32_t bt_rx_new_packet:               1;
            uint32_t reserved26:                     6;
        };
        uint32_t val;
    } slc1_int_st;
    union {
        struct {
            uint32_t reg_slc0_len:      20;
            uint32_t reg_slc0_len_check:12;
        };
        uint32_t val;
    } pkt_len;
    union {
        struct {
            uint32_t state0:         8;
            uint32_t state1:         8;
            uint32_t state2:         8;
            uint32_t state3:         8;
        };
        uint32_t val;
    } state_w0;
    union {
        struct {
            uint32_t state4:         8;
            uint32_t state5:         8;
            uint32_t state6:         8;
            uint32_t state7:         8;
        };
        uint32_t val;
    } state_w1;
    union {
        struct {
            uint32_t conf0:         8;
            uint32_t conf1:         8;
            uint32_t conf2:         8;
            uint32_t conf3:         8;
        };
        uint32_t val;
    } conf_w0;
    union {
        struct {
            uint32_t conf4:         8;
            uint32_t conf5:         8;
            uint32_t conf6:         8;
            uint32_t conf7:         8;
        };
        uint32_t val;
    } conf_w1;
    union {
        struct {
            uint32_t conf8:          8;
            uint32_t conf9:          8;
            uint32_t conf10:         8;
            uint32_t conf11:         8;
        };
        uint32_t val;
    } conf_w2;
    union {
        struct {
            uint32_t conf12:         8;
            uint32_t conf13:         8;
            uint32_t conf14:         8;
            uint32_t conf15:         8;
        };
        uint32_t val;
    } conf_w3;
    union {
        struct {
            uint32_t conf16:         8;                        /*SLC timeout value*/
            uint32_t conf17:         8;                        /*SLC timeout enable*/
            uint32_t conf18:         8;
            uint32_t conf19:         8;                        /*Interrupt to target CPU*/
        };
        uint32_t val;
    } conf_w4;
    union {
        struct {
            uint32_t conf20:         8;
            uint32_t conf21:         8;
            uint32_t conf22:         8;
            uint32_t conf23:         8;
        };
        uint32_t val;
    } conf_w5;
    uint32_t win_cmd;                                       /**/
    union {
        struct {
            uint32_t conf24:         8;
            uint32_t conf25:         8;
            uint32_t conf26:         8;
            uint32_t conf27:         8;
        };
        uint32_t val;
    } conf_w6;
    union {
        struct {
            uint32_t conf28:         8;
            uint32_t conf29:         8;
            uint32_t conf30:         8;
            uint32_t conf31:         8;
        };
        uint32_t val;
    } conf_w7;
    union {
        struct {
            uint32_t reg_slc0_len0:20;
            uint32_t reserved20:   12;
        };
        uint32_t val;
    } pkt_len0;
    union {
        struct {
            uint32_t reg_slc0_len1:20;
            uint32_t reserved20:   12;
        };
        uint32_t val;
    } pkt_len1;
    union {
        struct {
            uint32_t reg_slc0_len2:20;
            uint32_t reserved20:   12;
        };
        uint32_t val;
    } pkt_len2;
    union {
        struct {
            uint32_t conf32:         8;
            uint32_t conf33:         8;
            uint32_t conf34:         8;
            uint32_t conf35:         8;
        };
        uint32_t val;
    } conf_w8;
    union {
        struct {
            uint32_t conf36:         8;
            uint32_t conf37:         8;
            uint32_t conf38:         8;
            uint32_t conf39:         8;
        };
        uint32_t val;
    } conf_w9;
    union {
        struct {
            uint32_t conf40:         8;
            uint32_t conf41:         8;
            uint32_t conf42:         8;
            uint32_t conf43:         8;
        };
        uint32_t val;
    } conf_w10;
    union {
        struct {
            uint32_t conf44:         8;
            uint32_t conf45:         8;
            uint32_t conf46:         8;
            uint32_t conf47:         8;
        };
        uint32_t val;
    } conf_w11;
    union {
        struct {
            uint32_t conf48:         8;
            uint32_t conf49:         8;
            uint32_t conf50:         8;
            uint32_t conf51:         8;
        };
        uint32_t val;
    } conf_w12;
    union {
        struct {
            uint32_t conf52:         8;
            uint32_t conf53:         8;
            uint32_t conf54:         8;
            uint32_t conf55:         8;
        };
        uint32_t val;
    } conf_w13;
    union {
        struct {
            uint32_t conf56:         8;
            uint32_t conf57:         8;
            uint32_t conf58:         8;
            uint32_t conf59:         8;
        };
        uint32_t val;
    } conf_w14;
    union {
        struct {
            uint32_t conf60:         8;
            uint32_t conf61:         8;
            uint32_t conf62:         8;
            uint32_t conf63:         8;
        };
        uint32_t val;
    } conf_w15;
    uint32_t check_sum0;                                    /**/
    uint32_t check_sum1;                                    /**/
    union {
        struct {
            uint32_t token0:          12;
            uint32_t rx_pf_valid:      1;
            uint32_t reserved13:       3;
            uint32_t reg_token1:      12;
            uint32_t rx_pf_eof:        4;
        };
        uint32_t val;
    } slc1_token_rdata;
    union {
        struct {
            uint32_t token0_wd:         12;
            uint32_t reserved12:         4;
            uint32_t token1_wd:         12;
            uint32_t reserved28:         4;
        };
        uint32_t val;
    } slc0_token_wdata;
    union {
        struct {
            uint32_t token0_wd:         12;
            uint32_t reserved12:         4;
            uint32_t token1_wd:         12;
            uint32_t reserved28:         4;
        };
        uint32_t val;
    } slc1_token_wdata;
    union {
        struct {
            uint32_t slc0_token0_dec:     1;
            uint32_t slc0_token1_dec:     1;
            uint32_t slc0_token0_wr:      1;
            uint32_t slc0_token1_wr:      1;
            uint32_t slc1_token0_dec:     1;
            uint32_t slc1_token1_dec:     1;
            uint32_t slc1_token0_wr:      1;
            uint32_t slc1_token1_wr:      1;
            uint32_t slc0_len_wr:         1;
            uint32_t reserved9:          23;
        };
        uint32_t val;
    } token_con;
    union {
        struct {
            uint32_t tohost_bit0:                1;
            uint32_t tohost_bit1:                1;
            uint32_t tohost_bit2:                1;
            uint32_t tohost_bit3:                1;
            uint32_t tohost_bit4:                1;
            uint32_t tohost_bit5:                1;
            uint32_t tohost_bit6:                1;
            uint32_t tohost_bit7:                1;
            uint32_t token0_1to0:                1;
            uint32_t token1_1to0:                1;
            uint32_t token0_0to1:                1;
            uint32_t token1_0to1:                1;
            uint32_t rx_sof:                     1;
            uint32_t rx_eof:                     1;
            uint32_t rx_start:                   1;
            uint32_t tx_start:                   1;
            uint32_t rx_udf:                     1;
            uint32_t tx_ovf:                     1;
            uint32_t rx_pf_valid:                1;
            uint32_t ext_bit0:                   1;
            uint32_t ext_bit1:                   1;
            uint32_t ext_bit2:                   1;
            uint32_t ext_bit3:                   1;
            uint32_t rx_new_packet:              1;
            uint32_t rd_retry:                   1;
            uint32_t gpio_sdio:                  1;
            uint32_t reserved26:                 6;
        };
        uint32_t val;
    } slc0_int_clr;
    union {
        struct {
            uint32_t tohost_bit0:                     1;
            uint32_t tohost_bit1:                     1;
            uint32_t tohost_bit2:                     1;
            uint32_t tohost_bit3:                     1;
            uint32_t tohost_bit4:                     1;
            uint32_t tohost_bit5:                     1;
            uint32_t tohost_bit6:                     1;
            uint32_t tohost_bit7:                     1;
            uint32_t token0_1to0:                     1;
            uint32_t token1_1to0:                     1;
            uint32_t token0_0to1:                     1;
            uint32_t token1_0to1:                     1;
            uint32_t rx_sof:                          1;
            uint32_t rx_eof:                          1;
            uint32_t rx_start:                        1;
            uint32_t tx_start:                        1;
            uint32_t rx_udf:                          1;
            uint32_t tx_ovf:                          1;
            uint32_t rx_pf_valid:                     1;
            uint32_t ext_bit0:                        1;
            uint32_t ext_bit1:                        1;
            uint32_t ext_bit2:                        1;
            uint32_t ext_bit3:                        1;
            uint32_t wifi_rx_new_packet:              1;
            uint32_t rd_retry:                        1;
            uint32_t bt_rx_new_packet:                1;
            uint32_t reserved26:                      6;
        };
        uint32_t val;
    } slc1_int_clr;
    union {
        struct {
            uint32_t tohost_bit0:                    1;
            uint32_t tohost_bit1:                    1;
            uint32_t tohost_bit2:                    1;
            uint32_t tohost_bit3:                    1;
            uint32_t tohost_bit4:                    1;
            uint32_t tohost_bit5:                    1;
            uint32_t tohost_bit6:                    1;
            uint32_t tohost_bit7:                    1;
            uint32_t token0_1to0:                    1;
            uint32_t token1_1to0:                    1;
            uint32_t token0_0to1:                    1;
            uint32_t token1_0to1:                    1;
            uint32_t rx_sof:                         1;
            uint32_t rx_eof:                         1;
            uint32_t rx_start:                       1;
            uint32_t tx_start:                       1;
            uint32_t rx_udf:                         1;
            uint32_t tx_ovf:                         1;
            uint32_t rx_pf_valid:                    1;
            uint32_t ext_bit0:                       1;
            uint32_t ext_bit1:                       1;
            uint32_t ext_bit2:                       1;
            uint32_t ext_bit3:                       1;
            uint32_t rx_new_packet:                  1;
            uint32_t rd_retry:                       1;
            uint32_t gpio_sdio:                      1;
            uint32_t reserved26:                     6;
        };
        uint32_t val;
    } slc0_func1_int_ena;
    union {
        struct {
            uint32_t tohost_bit0:                         1;
            uint32_t tohost_bit1:                         1;
            uint32_t tohost_bit2:                         1;
            uint32_t tohost_bit3:                         1;
            uint32_t tohost_bit4:                         1;
            uint32_t tohost_bit5:                         1;
            uint32_t tohost_bit6:                         1;
            uint32_t tohost_bit7:                         1;
            uint32_t token0_1to0:                         1;
            uint32_t token1_1to0:                         1;
            uint32_t token0_0to1:                         1;
            uint32_t token1_0to1:                         1;
            uint32_t rx_sof:                              1;
            uint32_t rx_eof:                              1;
            uint32_t rx_start:                            1;
            uint32_t tx_start:                            1;
            uint32_t rx_udf:                              1;
            uint32_t tx_ovf:                              1;
            uint32_t rx_pf_valid:                         1;
            uint32_t ext_bit0:                            1;
            uint32_t ext_bit1:                            1;
            uint32_t ext_bit2:                            1;
            uint32_t ext_bit3:                            1;
            uint32_t wifi_rx_new_packet:                  1;
            uint32_t rd_retry:                            1;
            uint32_t bt_rx_new_packet:                    1;
            uint32_t reserved26:                          6;
        };
        uint32_t val;
    } slc1_func1_int_ena;
    union {
        struct {
            uint32_t tohost_bit0:                    1;
            uint32_t tohost_bit1:                    1;
            uint32_t tohost_bit2:                    1;
            uint32_t tohost_bit3:                    1;
            uint32_t tohost_bit4:                    1;
            uint32_t tohost_bit5:                    1;
            uint32_t tohost_bit6:                    1;
            uint32_t tohost_bit7:                    1;
            uint32_t token0_1to0:                    1;
            uint32_t token1_1to0:                    1;
            uint32_t token0_0to1:                    1;
            uint32_t token1_0to1:                    1;
            uint32_t rx_sof:                         1;
            uint32_t rx_eof:                         1;
            uint32_t rx_start:                       1;
            uint32_t tx_start:                       1;
            uint32_t rx_udf:                         1;
            uint32_t tx_ovf:                         1;
            uint32_t rx_pf_valid:                    1;
            uint32_t ext_bit0:                       1;
            uint32_t ext_bit1:                       1;
            uint32_t ext_bit2:                       1;
            uint32_t ext_bit3:                       1;
            uint32_t rx_new_packet:                  1;
            uint32_t rd_retry:                       1;
            uint32_t gpio_sdio:                      1;
            uint32_t reserved26:                     6;
        };
        uint32_t val;
    } slc0_func2_int_ena;
    union {
        struct {
            uint32_t tohost_bit0:                         1;
            uint32_t tohost_bit1:                         1;
            uint32_t tohost_bit2:                         1;
            uint32_t tohost_bit3:                         1;
            uint32_t tohost_bit4:                         1;
            uint32_t tohost_bit5:                         1;
            uint32_t tohost_bit6:                         1;
            uint32_t tohost_bit7:                         1;
            uint32_t token0_1to0:                         1;
            uint32_t token1_1to0:                         1;
            uint32_t token0_0to1:                         1;
            uint32_t token1_0to1:                         1;
            uint32_t rx_sof:                              1;
            uint32_t rx_eof:                              1;
            uint32_t rx_start:                            1;
            uint32_t tx_start:                            1;
            uint32_t rx_udf:                              1;
            uint32_t tx_ovf:                              1;
            uint32_t rx_pf_valid:                         1;
            uint32_t ext_bit0:                            1;
            uint32_t ext_bit1:                            1;
            uint32_t ext_bit2:                            1;
            uint32_t ext_bit3:                            1;
            uint32_t wifi_rx_new_packet:                  1;
            uint32_t rd_retry:                            1;
            uint32_t bt_rx_new_packet:                    1;
            uint32_t reserved26:                          6;
        };
        uint32_t val;
    } slc1_func2_int_ena;
    union {
        struct {
            uint32_t tohost_bit0:                1;
            uint32_t tohost_bit1:                1;
            uint32_t tohost_bit2:                1;
            uint32_t tohost_bit3:                1;
            uint32_t tohost_bit4:                1;
            uint32_t tohost_bit5:                1;
            uint32_t tohost_bit6:                1;
            uint32_t tohost_bit7:                1;
            uint32_t token0_1to0:                1;
            uint32_t token1_1to0:                1;
            uint32_t token0_0to1:                1;
            uint32_t token1_0to1:                1;
            uint32_t rx_sof:                     1;
            uint32_t rx_eof:                     1;
            uint32_t rx_start:                   1;
            uint32_t tx_start:                   1;
            uint32_t rx_udf:                     1;
            uint32_t tx_ovf:                     1;
            uint32_t rx_pf_valid:                1;
            uint32_t ext_bit0:                   1;
            uint32_t ext_bit1:                   1;
            uint32_t ext_bit2:                   1;
            uint32_t ext_bit3:                   1;
            uint32_t rx_new_packet:              1;
            uint32_t rd_retry:                   1;
            uint32_t gpio_sdio:                  1;
            uint32_t reserved26:                 6;
        };
        uint32_t val;
    } slc0_int_ena;
    union {
        struct {
            uint32_t tohost_bit0:                     1;
            uint32_t tohost_bit1:                     1;
            uint32_t tohost_bit2:                     1;
            uint32_t tohost_bit3:                     1;
            uint32_t tohost_bit4:                     1;
            uint32_t tohost_bit5:                     1;
            uint32_t tohost_bit6:                     1;
            uint32_t tohost_bit7:                     1;
            uint32_t token0_1to0:                     1;
            uint32_t token1_1to0:                     1;
            uint32_t token0_0to1:                     1;
            uint32_t token1_0to1:                     1;
            uint32_t rx_sof:                          1;
            uint32_t rx_eof:                          1;
            uint32_t rx_start:                        1;
            uint32_t tx_start:                        1;
            uint32_t rx_udf:                          1;
            uint32_t tx_ovf:                          1;
            uint32_t rx_pf_valid:                     1;
            uint32_t ext_bit0:                        1;
            uint32_t ext_bit1:                        1;
            uint32_t ext_bit2:                        1;
            uint32_t ext_bit3:                        1;
            uint32_t wifi_rx_new_packet:              1;
            uint32_t rd_retry:                        1;
            uint32_t bt_rx_new_packet:                1;
            uint32_t reserved26:                      6;
        };
        uint32_t val;
    } slc1_int_ena;
    union {
        struct {
            uint32_t infor:            20;
            uint32_t reserved20:       12;
        };
        uint32_t val;
    } slc0_rx_infor;
    union {
        struct {
            uint32_t infor:            20;
            uint32_t reserved20:       12;
        };
        uint32_t val;
    } slc1_rx_infor;
    uint32_t slc0_len_wd;                                       /**/
    uint32_t apbwin_wdata;                                 /**/
    union {
        struct {
            uint32_t addr:            28;
            uint32_t wr:               1;
            uint32_t start:            1;
            uint32_t reserved30:       2;
        };
        uint32_t val;
    } apbwin_conf;
    uint32_t apbwin_rdata;                                 /**/
    union {
        struct {
            uint32_t bit7_clraddr:              9;
            uint32_t bit6_clraddr:              9;
            uint32_t reserved18:               14;
        };
        uint32_t val;
    } slc0_rdclr;
    union {
        struct {
            uint32_t bit7_clraddr:              9;
            uint32_t bit6_clraddr:              9;
            uint32_t reserved18:               14;
        };
        uint32_t val;
    } slc1_rdclr;
    union {
        struct {
            uint32_t tohost_bit01:                1;
            uint32_t tohost_bit11:                1;
            uint32_t tohost_bit21:                1;
            uint32_t tohost_bit31:                1;
            uint32_t tohost_bit41:                1;
            uint32_t tohost_bit51:                1;
            uint32_t tohost_bit61:                1;
            uint32_t tohost_bit71:                1;
            uint32_t token0_1to01:                1;
            uint32_t token1_1to01:                1;
            uint32_t token0_0to11:                1;
            uint32_t token1_0to11:                1;
            uint32_t rx_sof1:                     1;
            uint32_t rx_eof1:                     1;
            uint32_t rx_start1:                   1;
            uint32_t tx_start1:                   1;
            uint32_t rx_udf1:                     1;
            uint32_t tx_ovf1:                     1;
            uint32_t rx_pf_valid1:                1;
            uint32_t ext_bit01:                   1;
            uint32_t ext_bit11:                   1;
            uint32_t ext_bit21:                   1;
            uint32_t ext_bit31:                   1;
            uint32_t rx_new_packet1:              1;
            uint32_t rd_retry1:                   1;
            uint32_t gpio_sdio1:                  1;
            uint32_t reserved26:                  6;
        };
        uint32_t val;
    } slc0_int_ena1;
    union {
        struct {
            uint32_t tohost_bit01:                     1;
            uint32_t tohost_bit11:                     1;
            uint32_t tohost_bit21:                     1;
            uint32_t tohost_bit31:                     1;
            uint32_t tohost_bit41:                     1;
            uint32_t tohost_bit51:                     1;
            uint32_t tohost_bit61:                     1;
            uint32_t tohost_bit71:                     1;
            uint32_t token0_1to01:                     1;
            uint32_t token1_1to01:                     1;
            uint32_t token0_0to11:                     1;
            uint32_t token1_0to11:                     1;
            uint32_t rx_sof1:                          1;
            uint32_t rx_eof1:                          1;
            uint32_t rx_start1:                        1;
            uint32_t tx_start1:                        1;
            uint32_t rx_udf1:                          1;
            uint32_t tx_ovf1:                          1;
            uint32_t rx_pf_valid1:                     1;
            uint32_t ext_bit01:                        1;
            uint32_t ext_bit11:                        1;
            uint32_t ext_bit21:                        1;
            uint32_t ext_bit31:                        1;
            uint32_t wifi_rx_new_packet1:              1;
            uint32_t rd_retry1:                        1;
            uint32_t bt_rx_new_packet1:                1;
            uint32_t reserved26:                       6;
        };
        uint32_t val;
    } slc1_int_ena1;
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
    uint32_t date;                                          /**/
    uint32_t id;                                            /**/
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
    union {
        struct {
            uint32_t frc_sdio11:       5;
            uint32_t frc_sdio20:       5;
            uint32_t frc_neg_samp:     5;
            uint32_t frc_pos_samp:     5;
            uint32_t frc_quick_in:     5;
            uint32_t sdio20_int_delay: 1;
            uint32_t sdio_pad_pullup:  1;
            uint32_t hspeed_con_en:    1;
            uint32_t reserved28:       4;
        };
        uint32_t val;
    } conf;
    union {
        struct {
            uint32_t sdio20_mode:   5;
            uint32_t sdio_neg_samp: 5;
            uint32_t sdio_quick_in: 5;
            uint32_t reserved15:   17;
        };
        uint32_t val;
    } inf_st;
} host_dev_t;
extern host_dev_t HOST;

#ifdef __cplusplus
}
#endif

#endif  /* _SOC_HOST_STRUCT_H_ */
