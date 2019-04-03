// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
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
#ifndef _SOC_EFUSE_STRUCT_H_
#define _SOC_EFUSE_STRUCT_H_
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct {
    union {
        struct {
            uint32_t rd_efuse_wr_dis:   16;
            uint32_t rd_efuse_rd_dis:    4;
            uint32_t rd_flash_crypt_cnt: 8;
            uint32_t reserved28:         4;
        };
        uint32_t val;
    } blk0_rdata0;
    uint32_t blk0_rdata1;                           /**/
    union {
        struct {
            uint32_t rd_wifi_mac_crc_high:24;
            uint32_t reserved24:           8;
        };
        uint32_t val;
    } blk0_rdata2;
    union {
        struct {
            uint32_t rd_soft_disable_jtag:  1;
            uint32_t rd_chip_ver_dis_bt:    1;
            uint32_t rd_chip_ver_32pad:     1;
            uint32_t rd_chip_ver_dis_cache: 1;
            uint32_t rd_chip_ver_reserve:  13;
            uint32_t reserved17:           15;
        };
        uint32_t val;
    } blk0_rdata3;
    union {
        struct {
            uint32_t rd_ck8m_freq:  8;
            uint32_t rd_sdio_drefh: 2;
            uint32_t rd_sdio_drefm: 2;
            uint32_t rd_sdio_drefl: 2;
            uint32_t rd_xpd_sdio:   1;
            uint32_t rd_sdio_tieh:  1;
            uint32_t rd_sdio_force: 1;
            uint32_t reserved17:   15;
        };
        uint32_t val;
    } blk0_rdata4;
    union {
        struct {
            uint32_t rd_spi_pad_config:    20;
            uint32_t rd_tsens_dos:          4;
            uint32_t rd_reserve_use1:       4;
            uint32_t rd_flash_crypt_config: 4;
        };
        uint32_t val;
    } blk0_rdata5;
    union {
        struct {
            uint32_t rd_coding_scheme:      2;
            uint32_t rd_dig_reserve:        1;
            uint32_t rd_disable_sdio_host:  1;
            uint32_t rd_abs_done_0:         1;
            uint32_t rd_abs_done_1:         1;
            uint32_t rd_hard_disable_jtag:  1;
            uint32_t rd_disable_dl_encrypt: 1;
            uint32_t rd_disable_dl_decrypt: 1;
            uint32_t rd_disable_dl_cache:   1;
            uint32_t rd_key_status:         1;
            uint32_t reserved11:           21;
        };
        uint32_t val;
    } blk0_rdata6;
    union {
        struct {
            uint32_t wr_dis:         16;
            uint32_t rd_dis:          4;
            uint32_t flash_crypt_cnt: 8;
            uint32_t reserved28:      4;
        };
        uint32_t val;
    } blk0_wdata0;
    uint32_t blk0_wdata1;                           /**/
    union {
        struct {
            uint32_t wifi_mac_crc_high:24;
            uint32_t reserved24:        8;
        };
        uint32_t val;
    } blk0_wdata2;
    union {
        struct {
            uint32_t soft_disable_jtag:  1;
            uint32_t chip_ver_dis_bt:    1;
            uint32_t chip_ver_32pad:     1;
            uint32_t chip_ver_dis_cache: 1;
            uint32_t chip_ver_reserve:  13;
            uint32_t reserved17:        15;
        };
        uint32_t val;
    } blk0_wdata3;
    union {
        struct {
            uint32_t ck8m_freq:  8;
            uint32_t sdio_drefh: 2;
            uint32_t sdio_drefm: 2;
            uint32_t sdio_drefl: 2;
            uint32_t xpd_sdio:   1;
            uint32_t sdio_tieh:  1;
            uint32_t sdio_force: 1;
            uint32_t reserved17:15;
        };
        uint32_t val;
    } blk0_wdata4;
    union {
        struct {
            uint32_t spi_pad_config:    20;
            uint32_t tsens_dos:          4;
            uint32_t reserve_use1:       4;
            uint32_t flash_crypt_config: 4;
        };
        uint32_t val;
    } blk0_wdata5;
    union {
        struct {
            uint32_t coding_scheme:      2;
            uint32_t dig_reserve:        1;
            uint32_t disable_sdio_host:  1;
            uint32_t abs_done_0:         1;
            uint32_t abs_done_1:         1;
            uint32_t hard_disable_jtag:  1;
            uint32_t disable_dl_encrypt: 1;
            uint32_t disable_dl_decrypt: 1;
            uint32_t disable_dl_cache:   1;
            uint32_t key_status:         1;
            uint32_t reserved11:        21;
        };
        uint32_t val;
    } blk0_wdata6;
    uint32_t blk1_rdata0;                           /**/
    uint32_t blk1_rdata1;                           /**/
    uint32_t blk1_rdata2;                           /**/
    uint32_t blk1_rdata3;                           /**/
    uint32_t blk1_rdata4;                           /**/
    uint32_t blk1_rdata5;                           /**/
    uint32_t blk1_rdata6;                           /**/
    uint32_t blk1_rdata7;                           /**/
    uint32_t blk2_rdata0;                           /**/
    uint32_t blk2_rdata1;                           /**/
    uint32_t blk2_rdata2;                           /**/
    uint32_t blk2_rdata3;                           /**/
    uint32_t blk2_rdata4;                           /**/
    uint32_t blk2_rdata5;                           /**/
    uint32_t blk2_rdata6;                           /**/
    uint32_t blk2_rdata7;                           /**/
    uint32_t blk3_rdata0;                           /**/
    uint32_t blk3_rdata1;                           /**/
    uint32_t blk3_rdata2;                           /**/
    uint32_t blk3_rdata3;                           /**/
    uint32_t blk3_rdata4;                           /**/
    uint32_t blk3_rdata5;                           /**/
    uint32_t blk3_rdata6;                           /**/
    uint32_t blk3_rdata7;                           /**/
    uint32_t blk1_wdata0;                           /**/
    uint32_t blk1_wdata1;                           /**/
    uint32_t blk1_wdata2;                           /**/
    uint32_t blk1_wdata3;                           /**/
    uint32_t blk1_wdata4;                           /**/
    uint32_t blk1_wdata5;                           /**/
    uint32_t blk1_wdata6;                           /**/
    uint32_t blk1_wdata7;                           /**/
    uint32_t blk2_wdata0;                           /**/
    uint32_t blk2_wdata1;                           /**/
    uint32_t blk2_wdata2;                           /**/
    uint32_t blk2_wdata3;                           /**/
    uint32_t blk2_wdata4;                           /**/
    uint32_t blk2_wdata5;                           /**/
    uint32_t blk2_wdata6;                           /**/
    uint32_t blk2_wdata7;                           /**/
    uint32_t blk3_wdata0;                           /**/
    uint32_t blk3_wdata1;                           /**/
    uint32_t blk3_wdata2;                           /**/
    uint32_t blk3_wdata3;                           /**/
    uint32_t blk3_wdata4;                           /**/
    uint32_t blk3_wdata5;                           /**/
    uint32_t blk3_wdata6;                           /**/
    uint32_t blk3_wdata7;                           /**/
    union {
        struct {
            uint32_t reserved0: 16;
            uint32_t clk_en:     1;
            uint32_t reserved17:15;
        };
        uint32_t val;
    } clk;
    union {
        struct {
            uint32_t op_code:           16;
            uint32_t force_no_wr_rd_dis: 1;
            uint32_t reserved17:        15;
        };
        uint32_t val;
    } conf;
    uint32_t status;                                /**/
    union {
        struct {
            uint32_t read_cmd:   1;
            uint32_t pgm_cmd:    1;
            uint32_t reserved2: 30;
        };
        uint32_t val;
    } cmd;
    union {
        struct {
            uint32_t read_done:         1;
            uint32_t pgm_done:          1;
            uint32_t reserved2:        30;
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t read_done:        1;
            uint32_t pgm_done:         1;
            uint32_t reserved2:       30;
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t read_done:         1;
            uint32_t pgm_done:          1;
            uint32_t reserved2:        30;
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t read_done:         1;
            uint32_t pgm_done:          1;
            uint32_t reserved2:        30;
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t dac_clk_div:     8;
            uint32_t dac_clk_pad_sel: 1;
            uint32_t reserved9:      23;
        };
        uint32_t val;
    } dac_conf;
    union {
        struct {
            uint32_t dec_warnings:12;
            uint32_t reserved12:  20;
        };
        uint32_t val;
    } dec_status;
    union {
        struct {
            uint32_t thr_a:         8;
            uint32_t trd:           8;
            uint32_t tsur_a:        8;
            uint32_t read_init_num: 8;
        };
        uint32_t val;
    } rd_tim_conf;
    union {
        struct {
            uint32_t thp_a:         8;
            uint32_t tpgm_inactive: 8;
            uint32_t tpgm:         16;
        };
        uint32_t val;
    } wr_tim_conf0;
    union {
        struct {
            uint32_t tsup_a:     8;
            uint32_t pwr_on_num:16;
            uint32_t reserved24: 8;
        };
        uint32_t val;
    } wr_tim_conf1;
    union {
        struct {
            uint32_t rd_dis_err:           4;
            uint32_t chip_ver_reserve_err:13;
            uint32_t ck8m_freq_err:        8;
            uint32_t sdio_drefh_err:       2;
            uint32_t sdio_drefm_err:       2;
            uint32_t sdio_drefl_err:       2;
            uint32_t key_status_err:       1;
        };
        uint32_t val;
    } repeat_err0;
    union {
        struct {
            uint32_t spi_pad_config_err:    20;
            uint32_t flash_crypt_config_err: 4;
            uint32_t tsens_dos_err:          4;
            uint32_t reserve_use1_err:       4;
        };
        uint32_t val;
    } repeat_err1;
    union {
        struct {
            uint32_t abs_done_0_err:         1;
            uint32_t abs_done_1_err:         1;
            uint32_t hard_disable_jtag_err:  1;
            uint32_t disable_dl_encrypt_err: 1;
            uint32_t disable_dl_decrypt_err: 1;
            uint32_t disable_dl_cache_err:   1;
            uint32_t disable_sdio_host_err:  1;
            uint32_t dig_reserve_err:        1;
            uint32_t soft_disable_jtag_err:  1;
            uint32_t chip_ver_dis_bt_err:    1;
            uint32_t chip_ver_32pad_err:     1;
            uint32_t chip_ver_dis_cache_err: 1;
            uint32_t xpd_sdio_err:           1;
            uint32_t sdio_tieh_err:          1;
            uint32_t sdio_force_err:         1;
            uint32_t reserved15:            17;
        };
        uint32_t val;
    } repeat_err2;
    uint32_t repeat_blk1_err0;                      /**/
    uint32_t repeat_blk1_err1;                      /**/
    uint32_t repeat_blk1_err2;                      /**/
    uint32_t repeat_blk1_err3;                      /**/
    uint32_t repeat_blk2_err0;                      /**/
    uint32_t repeat_blk2_err1;                      /**/
    uint32_t repeat_blk2_err2;                      /**/
    uint32_t repeat_blk2_err3;                      /**/
    uint32_t repeat_blk3_err0;                      /**/
    uint32_t repeat_blk3_err1;                      /**/
    uint32_t repeat_blk3_err2;                      /**/
    uint32_t repeat_blk3_err3;                      /**/
    union {
        struct {
            uint32_t rs_errornum_blk1: 4;
            uint32_t rs_fail_blk1:     1;
            uint32_t rs_errornum_blk2: 4;
            uint32_t rs_fail_blk2:     1;
            uint32_t rs_errornum_blk3: 4;
            uint32_t rs_fail_blk3:     1;
            uint32_t reserved15:      17;
        };
        uint32_t val;
    } rs_state;
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
    uint32_t date;                                  /**/
} efuse_dev_t;
extern efuse_dev_t EFUSE;
#ifdef __cplusplus
}
#endif

#endif  /* _SOC_EFUSE_STRUCT_H_ */
