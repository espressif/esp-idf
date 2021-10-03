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

typedef volatile struct efuse_dev_s {
    uint32_t pgm_data0;                                         /**/
    union {
        struct {
            uint32_t rd_dis:                      7;
            uint32_t dis_rtc_ram_boot:            1;
            uint32_t dis_icache:                  1;
            uint32_t dis_dcache:                  1;
            uint32_t dis_download_icache:         1;
            uint32_t dis_download_dcache:         1;
            uint32_t dis_force_download:          1;
            uint32_t dis_usb:                     1;
            uint32_t dis_can:                     1;
            uint32_t dis_sdio_access:             1;
            uint32_t dis_efuse_ate_wr:            1;
            uint32_t soft_dis_jtag:               1;
            uint32_t hard_dis_jtag:               1;
            uint32_t dis_download_manual_encrypt: 1;
            uint32_t usb_drefh:                   2;
            uint32_t usb_drefl:                   2;
            uint32_t usb_exchg_pins:              1;
            uint32_t ext_phy_enable:              1;
            uint32_t usb_force_b:                 1;
            uint32_t usb_dres:                    2;
            uint32_t sdio_modecurlim:             1;
            uint32_t sdio_drefh:                  2;
        };
        uint32_t val;
    } pgm_data1;
    union {
        struct {
            uint32_t sdio_drefm:              2;
            uint32_t sdio_drefl:              2;
            uint32_t sdio_xpd:                1;
            uint32_t sdio_tieh:               1;
            uint32_t sdio_force:              1;
            uint32_t sdio_en_init:            1;
            uint32_t sdio_encurlim:           1;
            uint32_t sdio_dcurlim:            3;
            uint32_t sdio_init:               2;
            uint32_t sdio_dcap:               2;
            uint32_t wdt_delay_sel:           2;
            uint32_t spi_boot_crypt_cnt:      3;
            uint32_t secure_boot_key_revoke0: 1;
            uint32_t secure_boot_key_revoke1: 1;
            uint32_t secure_boot_key_revoke2: 1;
            uint32_t key_purpose_0:           4;
            uint32_t key_purpose_1:           4;
        };
        uint32_t val;
    } pgm_data2;
    union {
        struct {
            uint32_t key_purpose_2:                 4;
            uint32_t key_purpose_3:                 4;
            uint32_t key_purpose_4:                 4;
            uint32_t key_purpose_5:                 4;
            uint32_t key_purpose_6:                 4;
            uint32_t secure_boot_en:                1;
            uint32_t secure_boot_aggressive_revoke: 1;
            uint32_t xtal_freq:                     6;
            uint32_t flash_tpuw:                    4;
        };
        uint32_t val;
    } pgm_data3;
    union {
        struct {
            uint32_t dis_download_mode:        1;
            uint32_t dis_legacy_spi_boot:      1;
            uint32_t uart_print_channel:       1;
            uint32_t dis_tiny_basic:           1;
            uint32_t dis_usb_download_mode:    1;
            uint32_t enable_security_download: 1;
            uint32_t uart_print_control:       2;
            uint32_t reserve:                 24;
        };
        uint32_t val;
    } pgm_data4;
    union {
        struct {
            uint32_t chip_version:24;
            uint32_t rs_data_23:   8;
        };
        uint32_t val;
    } pgm_data5;
    uint32_t pgm_data6;                                         /**/
    uint32_t pgm_data7;                                         /**/
    uint32_t pgm_check_value0;                                  /**/
    uint32_t pgm_check_value1;                                  /**/
    uint32_t pgm_check_value2;                                  /**/
    uint32_t rd_wr_dis;                                         /**/
    union {
        struct {
            uint32_t rd_dis:                      7;
            uint32_t dis_rtc_ram_boot:            1;
            uint32_t dis_icache:                  1;
            uint32_t dis_dcache:                  1;
            uint32_t dis_download_icache:         1;
            uint32_t dis_download_dcache:         1;
            uint32_t dis_force_download:          1;
            uint32_t dis_usb:                     1;
            uint32_t dis_can:                     1;
            uint32_t dis_sdio_access:             1;
            uint32_t dis_ate_wr:                  1;
            uint32_t soft_dis_jtag:               1;
            uint32_t hard_dis_jtag:               1;
            uint32_t dis_download_manual_encrypt: 1;
            uint32_t usb_drefh:                   2;
            uint32_t usb_drefl:                   2;
            uint32_t usb_exchg_pins:              1;
            uint32_t ext_phy_enable:              1;
            uint32_t usb_force_b:                 1;
            uint32_t usb_dres:                    2;
            uint32_t sdio_modecurlim:             1;
            uint32_t sdio_drefh:                  2;
        };
        uint32_t val;
    } rd_repeat_data0;
    union {
        struct {
            uint32_t sdio_drefm:              2;
            uint32_t sdio_drefl:              2;
            uint32_t sdio_xpd:                1;
            uint32_t sdio_tieh:               1;
            uint32_t sdio_force:              1;
            uint32_t sdio_en_init:            1;
            uint32_t sdio_encurlim:           1;
            uint32_t sdio_dcurlim:            3;
            uint32_t sdio_init:               2;
            uint32_t eufse_sdio_dcap:         2;
            uint32_t wdt_delay_sel:           2;
            uint32_t spi_boot_crypt_cnt:      3;
            uint32_t secure_boot_key_revoke0: 1;
            uint32_t secure_boot_key_revoke1: 1;
            uint32_t secure_boot_key_revoke2: 1;
            uint32_t key_purpose_0:           4;
            uint32_t key_purpose_1:           4;
        };
        uint32_t val;
    } rd_repeat_data1;
    union {
        struct {
            uint32_t key_purpose_2:                 4;
            uint32_t key_purpose_3:                 4;
            uint32_t key_purpose_4:                 4;
            uint32_t key_purpose_5:                 4;
            uint32_t key_purpose_6:                 4;
            uint32_t secure_boot_en:                1;
            uint32_t secure_boot_aggressive_revoke: 1;
            uint32_t xtal_freq:                     6;
            uint32_t flash_tpuw:                    4;
        };
        uint32_t val;
    } rd_repeat_data2;
    union {
        struct {
            uint32_t dis_download_mode:        1;
            uint32_t dis_legacy_spi_boot:      1;
            uint32_t uart_print_channel:       1;
            uint32_t dis_tiny_basic:           1;
            uint32_t dis_usb_download_mode:    1;
            uint32_t enable_security_download: 1;
            uint32_t uart_print_control:       2;
            uint32_t reserve:                 24;
        };
        uint32_t val;
    } rd_repeat_data3;
    union {
        struct {
            uint32_t chip_version:24;
            uint32_t reserved24:   8;
        };
        uint32_t val;
    } rd_repeat_data4;
    uint32_t rd_mac_spi_8m_0;                                   /**/
    union {
        struct {
            uint32_t mac_1:         16;
            uint32_t spi_pad_conf_0:16;
        };
        uint32_t val;
    } rd_mac_spi_8m_1;
    union {
        struct {
            uint32_t spi_pad_conf_1:20;
            uint32_t clk8m_freq:    12;
        };
        uint32_t val;
    } rd_mac_spi_8m_2;
    uint32_t rd_mac_spi_8m_3;                                   /**/
    uint32_t rd_mac_spi_8m_4;                                   /**/
    uint32_t rd_mac_spi_8m_5;                                   /**/
    uint32_t rd_sys_data0;                                      /**/
    uint32_t rd_sys_data1;                                      /**/
    uint32_t rd_sys_data2;                                      /**/
    uint32_t rd_sys_data3;                                      /**/
    uint32_t rd_sys_data4;                                      /**/
    uint32_t rd_sys_data5;                                      /**/
    uint32_t rd_sys_data6;                                      /**/
    uint32_t rd_sys_data7;                                      /**/
    uint32_t rd_usr_data0;                                      /**/
    uint32_t rd_usr_data1;                                      /**/
    uint32_t rd_usr_data2;                                      /**/
    uint32_t rd_usr_data3;                                      /**/
    uint32_t rd_usr_data4;                                      /**/
    uint32_t rd_usr_data5;                                      /**/
    uint32_t rd_usr_data6;                                      /**/
    uint32_t rd_usr_data7;                                      /**/
    uint32_t rd_key0_data0;                                     /**/
    uint32_t rd_key0_data1;                                     /**/
    uint32_t rd_key0_data2;                                     /**/
    uint32_t rd_key0_data3;                                     /**/
    uint32_t rd_key0_data4;                                     /**/
    uint32_t rd_key0_data5;                                     /**/
    uint32_t rd_key0_data6;                                     /**/
    uint32_t rd_key0_data7;                                     /**/
    uint32_t rd_key1_data0;                                     /**/
    uint32_t rd_key1_data1;                                     /**/
    uint32_t rd_key1_data2;                                     /**/
    uint32_t rd_key1_data3;                                     /**/
    uint32_t rd_key1_data4;                                     /**/
    uint32_t rd_key1_data5;                                     /**/
    uint32_t rd_key1_data6;                                     /**/
    uint32_t rd_key1_data7;                                     /**/
    uint32_t rd_key2_data0;                                     /**/
    uint32_t rd_key2_data1;                                     /**/
    uint32_t rd_key2_data2;                                     /**/
    uint32_t rd_key2_data3;                                     /**/
    uint32_t rd_key2_data4;                                     /**/
    uint32_t rd_key2_data5;                                     /**/
    uint32_t rd_key2_data6;                                     /**/
    uint32_t rd_key2_data7;                                     /**/
    uint32_t rd_key3_data0;                                     /**/
    uint32_t rd_key3_data1;                                     /**/
    uint32_t rd_key3_data2;                                     /**/
    uint32_t rd_key3_data3;                                     /**/
    uint32_t rd_key3_data4;                                     /**/
    uint32_t rd_key3_data5;                                     /**/
    uint32_t rd_key3_data6;                                     /**/
    uint32_t rd_key3_data7;                                     /**/
    uint32_t rd_key4_data0;                                     /**/
    uint32_t rd_key4_data1;                                     /**/
    uint32_t rd_key4_data2;                                     /**/
    uint32_t rd_key4_data3;                                     /**/
    uint32_t rd_key4_data4;                                     /**/
    uint32_t rd_key4_data5;                                     /**/
    uint32_t rd_key4_data6;                                     /**/
    uint32_t rd_key4_data7;                                     /**/
    uint32_t rd_key5_data0;                                     /**/
    uint32_t rd_key5_data1;                                     /**/
    uint32_t rd_key5_data2;                                     /**/
    uint32_t rd_key5_data3;                                     /**/
    uint32_t rd_key5_data4;                                     /**/
    uint32_t rd_key5_data5;                                     /**/
    uint32_t rd_key5_data6;                                     /**/
    uint32_t rd_key5_data7;                                     /**/
    uint32_t rd_key6_data0;                                     /**/
    uint32_t rd_key6_data1;                                     /**/
    uint32_t rd_key6_data2;                                     /**/
    uint32_t rd_key6_data3;                                     /**/
    uint32_t rd_key6_data4;                                     /**/
    uint32_t rd_key6_data5;                                     /**/
    uint32_t rd_key6_data6;                                     /**/
    uint32_t rd_key6_data7;                                     /**/
    union {
        struct {
            uint32_t rd_rd_dis_err:                      7;
            uint32_t rd_dis_rtc_ram_boot_err:            1;
            uint32_t rd_dis_icache_err:                  1;
            uint32_t rd_dis_dcache_err:                  1;
            uint32_t rd_dis_download_icache_err:         1;
            uint32_t rd_dis_download_dcache_err:         1;
            uint32_t rd_dis_force_download:              1;
            uint32_t rd_dis_usb_err:                     1;
            uint32_t rd_dis_can_err:                     1;
            uint32_t rd_dis_sdio_access_err:             1;
            uint32_t rd_dis_efuse_ate_wr_err:            1;
            uint32_t rd_soft_dis_jtag_err:               1;
            uint32_t rd_hard_dis_jtag_err:               1;
            uint32_t rd_dis_download_manual_encrypt_err: 1;
            uint32_t rd_usb_drefh_err:                   2;
            uint32_t rd_usb_drefl_err:                   2;
            uint32_t rd_usb_exchg_pins_err:              1;
            uint32_t rd_ext_phy_enable:                  1;
            uint32_t rd_usb_force:                       1;
            uint32_t rd_usb_dres_err:                    2;
            uint32_t rd_sdio_modecurlim_err:             1;
            uint32_t rd_sdio_drefh_err:                  2;
        };
        uint32_t val;
    } rd_repeat_err0;
    union {
        struct {
            uint32_t rd_sdio_drefm_err:              2;
            uint32_t rd_sdio_drefl_err:              2;
            uint32_t rd_sdio_xpd_err:                1;
            uint32_t rd_sdio_tieh_err:               1;
            uint32_t rd_sdio_force_err:              1;
            uint32_t rd_sdio_en_init_err:            1;
            uint32_t rd_sdio_encurlim_err:           1;
            uint32_t rd_sdio_dcurlim_err:            3;
            uint32_t rd_sdio_init_err:               2;
            uint32_t rd_sdio_dcap_err:               2;
            uint32_t rd_wdt_delay_sel_err:           2;
            uint32_t rd_spi_boot_crypt_cnt_err:      3;
            uint32_t rd_secure_boot_key_revoke0_err: 1;
            uint32_t rd_secure_boot_key_revoke1_err: 1;
            uint32_t rd_secure_boot_key_revoke2_err: 1;
            uint32_t rd_key_purpose_0_err:           4;
            uint32_t rd_key_purpose_1_err:           4;
        };
        uint32_t val;
    } rd_repeat_err1;
    union {
        struct {
            uint32_t rd_key_purpose_2_err:                 4;
            uint32_t rd_key_purpose_3_err:                 4;
            uint32_t rd_key_purpose_4_err:                 4;
            uint32_t rd_key_purpose_5_err:                 4;
            uint32_t rd_key_purpose_6_err:                 4;
            uint32_t rd_secure_boot_en_err:                1;
            uint32_t rd_secure_boot_aggressive_revoke_err: 1;
            uint32_t rd_xtal_freq_err:                     6;
            uint32_t rd_flash_tpuw_err:                    4;
        };
        uint32_t val;
    } rd_repeat_err2;
    union {
        struct {
            uint32_t rd_dis_download_mode_err:    1;
            uint32_t rd_dis_legacy_spi_boot_err:  1;
            uint32_t rd_uart_print_channel:       1;
            uint32_t rd_dis_tiny_basic:           1;
            uint32_t rd_dis_usb_download_mode:    1;
            uint32_t rd_enable_security_download: 1;
            uint32_t rd_uart_print_control:       2;
            uint32_t rd_reserve_err:             24;
        };
        uint32_t val;
    } rd_repeat_err3;
    union {
        struct {
            uint32_t rd_chip_version_err:24;
            uint32_t reserved24:          8;
        };
        uint32_t val;
    } rd_repeat_err4;
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
            uint32_t rd_mac_spi_8m_err_num: 3;
            uint32_t rd_mac_spi_8m_fail:    1;
            uint32_t rd_sys_err_num:        3;
            uint32_t rd_sys_err_fail:       1;
            uint32_t rd_usr_data_err_num:   3;
            uint32_t rd_usr_data_fail:      1;
            uint32_t rd_key0_err_num:       3;
            uint32_t rd_key0_fail:          1;
            uint32_t rd_key1_err_num:       3;
            uint32_t rd_key1_fail:          1;
            uint32_t rd_key2_err_num:       3;
            uint32_t rd_key2_fail:          1;
            uint32_t rd_key3_err_num:       3;
            uint32_t rd_key3_fail:          1;
            uint32_t rd_key4_err_num:       3;
            uint32_t rd_key4_fail:          1;
        };
        uint32_t val;
    } rd_rs_err0;
    union {
        struct {
            uint32_t rd_key5_err_num: 3;
            uint32_t rd_key5_fail:    1;
            uint32_t rd_key6_err_num: 3;
            uint32_t rd_key6_fail:    1;
            uint32_t reserved8:      24;
        };
        uint32_t val;
    } rd_rs_err1;
    union {
        struct {
            uint32_t mem_pd:     1;
            uint32_t reserved1: 15;
            uint32_t clk_en:     1;
            uint32_t reserved17:15;
        };
        uint32_t val;
    } clk;
    union {
        struct {
            uint32_t op_code:   16;
            uint32_t reserved16:16;
        };
        uint32_t val;
    } conf;
    union {
        struct {
            uint32_t state:            3;
            uint32_t otp_load_sw:      1;
            uint32_t otp_vddq_c_sync2: 1;
            uint32_t otp_strobe_sw:    1;
            uint32_t otp_csb_sw:       1;
            uint32_t otp_pgenb_sw:     1;
            uint32_t otp_vddq_is_sw:   1;
            uint32_t repeat_err_cnt:   8;
            uint32_t reserved17:      15;
        };
        uint32_t val;
    } status;
    union {
        struct {
            uint32_t read_cmd:   1;
            uint32_t pgm_cmd:    1;
            uint32_t blk_num:    4;
            uint32_t reserved6: 26;
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
    uint32_t reserved_1f8;
    uint32_t date;                                              /**/
} efuse_dev_t;
extern efuse_dev_t EFUSE;
#ifdef __cplusplus
}
#endif

#endif  /* _SOC_EFUSE_STRUCT_H_ */
