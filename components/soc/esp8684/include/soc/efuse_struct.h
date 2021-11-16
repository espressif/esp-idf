/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_EFUSE_STRUCT_H_
#define _SOC_EFUSE_STRUCT_H_


#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct efuse_dev_s {
    uint32_t pgm_data0;
    uint32_t pgm_data1;
    uint32_t pgm_data2;
    uint32_t pgm_data3;
    uint32_t pgm_data4;
    uint32_t pgm_data5;
    uint32_t pgm_data6;
    uint32_t pgm_data7;
    uint32_t pgm_check_value0;
    uint32_t pgm_check_value1;
    uint32_t pgm_check_value2;
    union {
        struct {
            uint32_t reg_wr_dis                    :    8;  /*Disable programming of individual eFuses.*/
            uint32_t reserved8                     :    24;  /*Reserved.*/
        };
        uint32_t val;
    } rd_wr_dis;
    union {
        struct {
            uint32_t reg_rd_dis                    :    2;  /*The bit be set to disable software read high/low 128-bit of BLK3.*/
            uint32_t reg_wdt_delay_sel             :    2;  /*Selects RTC watchdog timeout threshold, in unit of slow clock cycle. 0: 40000. 1: 80000. 2: 160000. 3:320000.*/
            uint32_t reg_dis_pad_jtag              :    1;  /*Set this bit to disable pad jtag.*/
            uint32_t reg_dis_download_icache       :    1;  /*The bit be set to disable icache in download mode.*/
            uint32_t reg_dis_download_manual_encrypt:    1;  /*The bit be set to disable manual encryption.*/
            uint32_t reg_spi_boot_encrypt_decrypt_cnt:    3;  /*These bits be set to enable SPI boot encrypt/decrypt. Odd number of 1: enable. even number of 1: disable.*/
            uint32_t reg_xts_key_length_256        :    1;  /*The bit be set means XTS_AES use the whole 256-bit efuse data in BLOCK3. Otherwise, XTS_AES use 128-bit eFuse data in BLOCK3.*/
            uint32_t reg_uart_print_control        :    2;  /*Set this bit to disable usb printing.*/
            uint32_t reg_force_send_resume         :    1;  /*Set this bit to force ROM code to send a resume command during SPI boot.*/
            uint32_t reg_dis_download_mode         :    1;  /*Set this bit to disable download mode (boot_mode[3:0] = 0, 1, 2, 4, 5, 6, 7).*/
            uint32_t reg_dis_direct_boot           :    1;  /*This bit set means disable direct_boot mode.*/
            uint32_t reg_enable_security_download  :    1;  /*Set this bit to enable secure UART download mode.*/
            uint32_t reg_flash_tpuw                :    4;  /*Configures flash waiting time after power-up, in unit of ms. If the value is less than 15, the waiting time is the configurable value.  Otherwise, the waiting time is twice the configurable value.*/
            uint32_t reg_secure_boot_en            :    1;  /*The bit be set to enable secure boot.*/
            uint32_t reg_rpt4_reserved             :    10;  /*Reserved (used for four backups method).*/
        };
        uint32_t val;
    } rd_repeat_data0;
    uint32_t rd_blk1_data0;
    uint32_t rd_blk1_data1;
    union {
        struct {
            uint32_t reg_system_data2              :    24;  /*Stores the bits [64:87] of system data.*/
            uint32_t reserved24                    :    8;  /*Reserved.*/
        };
        uint32_t val;
    } rd_blk1_data2;
    uint32_t rd_blk2_data0;
    union {
        struct {
            uint32_t reg_mac_id_high               :    16;  /*Store the bit [31:47] of MAC.*/
            uint32_t reg_wafer_version             :    3;  /*Store wafer version.*/
            uint32_t reg_pkg_version               :    3;  /*Store package version.*/
            uint32_t reg_blk2_efuse_version        :    3;  /*Store efuse version.*/
            uint32_t reg_rf_ref_i_bias_config      :    4;  /*Store rf configuration parameters.*/
            uint32_t reg_ldo_vol_bias_config_low   :    3;  /*Store the bit [0:2] of ido configuration parameters.*/
        };
        uint32_t val;
    } rd_blk2_data1;
    union {
        struct {
            uint32_t reg_ldo_vol_bias_config_high  :    27;  /*Store the bit [3:29] of ido configuration parameters.*/
            uint32_t reg_pvt_low                   :    5;  /*Store the bit [0:4] of pvt.*/
        };
        uint32_t val;
    } rd_blk2_data2;
    union {
        struct {
            uint32_t reg_pvt_high                  :    10;  /*Store the bit [5:14] of pvt.*/
            uint32_t reg_adc_calibration_0         :    22;  /*Store the bit [0:21] of ADC calibration data.*/
        };
        uint32_t val;
    } rd_blk2_data3;
    uint32_t rd_blk2_data4;
    uint32_t rd_blk2_data5;
    union {
        struct {
            uint32_t reg_adc_calibration_3         :    11;  /*Store the bit [86:96] of ADC calibration data.*/
            uint32_t reg_blk2_reserved_data_0      :    21;  /*Store the bit [0:20] of block2 reserved data.*/
        };
        uint32_t val;
    } rd_blk2_data6;
    uint32_t rd_blk2_data7;
    uint32_t rd_blk3_data0;
    uint32_t rd_blk3_data1;
    uint32_t rd_blk3_data2;
    uint32_t rd_blk3_data3;
    uint32_t rd_blk3_data4;
    uint32_t rd_blk3_data5;
    uint32_t rd_blk3_data6;
    uint32_t rd_blk3_data7;
    union {
        struct {
            uint32_t reg_rd_dis_err                :    2;  /*If any bit in RD_DIS is 1, then it indicates a programming error.*/
            uint32_t reg_wdt_delay_sel_err         :    2;  /*If any bit in WDT_DELAY_SEL is 1, then it indicates a programming error.*/
            uint32_t reg_dis_pad_jtag_err          :    1;  /*If any bit in DIS_PAD_JTAG is 1, then it indicates a programming error.*/
            uint32_t reg_dis_download_icache       :    1;  /*If any bit in this filed is 1, then it indicates a programming error.*/
            uint32_t reg_dis_download_manual_encrypt_err:    1;  /*If any bit in DIS_DOWNLOAD_MANUAL_ENCRYPT is 1, then it indicates a programming error.*/
            uint32_t reg_spi_boot_encrypt_decrypt_cnt_err:    3;  /*If any bit in SPI_BOOT_ENCRYPT_DECRYPT_CNT is 1, then it indicates a programming error.*/
            uint32_t reg_xts_key_length_256_err    :    1;  /*If any bit in XTS_KEY_LENGTH_256 is 1, then it indicates a programming error.*/
            uint32_t reg_uart_print_control_err    :    2;  /*If any bit in UART_PRINT_CONTROL is 1, then it indicates a programming error.*/
            uint32_t reg_force_send_resume_err     :    1;  /*If any bit in FORCE_SEND_RESUME is 1, then it indicates a programming error.*/
            uint32_t reg_dis_download_mode_err     :    1;  /*If any bit in this filed is 1, then it indicates a programming error.*/
            uint32_t reg_dis_direct_boot_err       :    1;  /*If any bit in this filed is 1, then it indicates a programming error.*/
            uint32_t reg_enable_security_download_err:    1;  /*If any bit in this filed is 1, then it indicates a programming error.*/
            uint32_t reg_flash_tpuw_err            :    4;  /*If any bit in this filed is 1, then it indicates a programming error.*/
            uint32_t reg_secure_boot_en_err        :    1;  /*If any bit in this filed is 1, then it indicates a programming error.*/
            uint32_t reg_rpt4_reserved_err         :    10;  /*Reserved.*/
        };
        uint32_t val;
    } rd_repeat_err;
    union {
        struct {
            uint32_t blk1_err_num                  :    3;  /*The value of this signal means the number of error bytes in block1.*/
            uint32_t blk1_fail                     :    1;  /*0: Means no failure and that the data of block1 is reliable 1: Means that programming user data failed and the number of error bytes is over 6.*/
            uint32_t blk2_err_num                  :    3;  /*The value of this signal means the number of error bytes in block2.*/
            uint32_t blk2_fail                     :    1;  /*0: Means no failure and that the data of block2 is reliable 1: Means that programming user data failed and the number of error bytes is over 6.*/
            uint32_t blk3_err_num                  :    3;  /*The value of this signal means the number of error bytes in block3.*/
            uint32_t blk3_fail                     :    1;  /*0: Means no failure and that the block3 data is reliable 1: Means that programming user data failed and the number of error bytes is over 6.*/
            uint32_t reserved12                    :    20;  /*Reserved.*/
        };
        uint32_t val;
    } rd_rs_err;
    union {
        struct {
            uint32_t reg_efuse_mem_force_pd        :    1;  /*Set this bit to force eFuse SRAM into power-saving mode.*/
            uint32_t reg_mem_clk_force_on          :    1;  /*Set this bit and force to activate clock signal of eFuse SRAM.*/
            uint32_t reg_efuse_mem_force_pu        :    1;  /*Set this bit to force eFuse SRAM into working mode.*/
            uint32_t reserved3                     :    13;  /*Reserved.*/
            uint32_t reg_clk_en                    :    1;  /*Set this bit and force to enable clock signal of eFuse memory.*/
            uint32_t reserved17                    :    15;  /*Reserved.*/
        };
        uint32_t val;
    } clk;
    union {
        struct {
            uint32_t reg_op_code                   :    16;  /*0x5A5A: Operate programming command 0x5AA5: Operate read command.*/
            uint32_t reserved16                    :    16;  /*Reserved.*/
        };
        uint32_t val;
    } conf;
    union {
        struct {
            uint32_t reg_state                     :    4;  /*Indicates the state of the eFuse state machine.*/
            uint32_t reg_otp_load_sw               :    1;  /*The value of OTP_LOAD_SW.*/
            uint32_t reg_otp_vddq_c_sync2          :    1;  /*The value of OTP_VDDQ_C_SYNC2.*/
            uint32_t reg_otp_strobe_sw             :    1;  /*The value of OTP_STROBE_SW.*/
            uint32_t reg_otp_csb_sw                :    1;  /*The value of OTP_CSB_SW.*/
            uint32_t reg_otp_pgenb_sw              :    1;  /*The value of OTP_PGENB_SW.*/
            uint32_t reg_otp_vddq_is_sw            :    1;  /*The value of OTP_VDDQ_IS_SW.*/
            uint32_t reg_blk0_valid_bit_cnt        :    6;  /*Record the number of bit '1' in BLOCK0.*/
            uint32_t reserved16                    :    16;  /*Reserved.*/
        };
        uint32_t val;
    } status;
    union {
        struct {
            uint32_t reg_read_cmd                  :    1;  /*Set this bit to send read command.*/
            uint32_t reg_pgm_cmd                   :    1;  /*Set this bit to send programming command.*/
            uint32_t reg_blk_num                   :    2;  /*The serial number of the block to be programmed. Value 0-3 corresponds to block number 0-3, respectively.*/
            uint32_t reserved4                     :    28;  /*Reserved.*/
        };
        uint32_t val;
    } cmd;
    union {
        struct {
            uint32_t reg_read_done_int_raw         :    1;  /*The raw bit signal for read_done interrupt.*/
            uint32_t reg_pgm_done_int_raw          :    1;  /*The raw bit signal for pgm_done interrupt.*/
            uint32_t reserved2                     :    30;  /*Reserved.*/
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t reg_read_done_int_st          :    1;  /*The status signal for read_done interrupt.*/
            uint32_t reg_pgm_done_int_st           :    1;  /*The status signal for pgm_done interrupt.*/
            uint32_t reserved2                     :    30;  /*Reserved.*/
        };
        uint32_t val;
    } int_st;
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
    union {
        struct {
            uint32_t reg_read_done_int_ena         :    1;  /*The enable signal for read_done interrupt.*/
            uint32_t reg_pgm_done_int_ena          :    1;  /*The enable signal for pgm_done interrupt.*/
            uint32_t reserved2                     :    30;  /*Reserved.*/
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t reg_read_done_int_clr         :    1;  /*The clear signal for read_done interrupt.*/
            uint32_t reg_pgm_done_int_clr          :    1;  /*The clear signal for pgm_done interrupt.*/
            uint32_t reserved2                     :    30;  /*Reserved.*/
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t reg_dac_clk_div               :    8;  /*Controls the division factor of the rising clock of the programming voltage.*/
            uint32_t reg_dac_clk_pad_sel           :    1;  /*Don't care.*/
            uint32_t reg_dac_num                   :    8;  /*Controls the rising period of the programming voltage.*/
            uint32_t reg_oe_clr                    :    1;  /*Reduces the power supply of the programming voltage.*/
            uint32_t reserved18                    :    14;  /*Reserved.*/
        };
        uint32_t val;
    } dac_conf;
    union {
        struct {
            uint32_t reg_thr_a                     :    8;  /*Configures hold time for efuse read.*/
            uint32_t reg_trd                       :    8;  /*Configures pulse time for efuse read.*/
            uint32_t reg_tsur_a                    :    8;  /*Configures setup time for efuse read.*/
            uint32_t reg_read_init_num             :    8;  /*Configures the initial read time of eFuse.*/
        };
        uint32_t val;
    } rd_tim_conf;
    union {
        struct {
            uint32_t reg_thp_a                     :    8;  /*Configures hold time for efuse program.*/
            uint32_t reg_tpgm_inactive             :    8;  /*Configures pulse time for burning '0' bit.*/
            uint32_t reg_tpgm                      :    16;  /*Configures pulse time for burning '1' bit.*/
        };
        uint32_t val;
    } wr_tim_conf0;
    union {
        struct {
            uint32_t reg_tsup_a                    :    8;  /*Configures setup time for efuse program.*/
            uint32_t reg_pwr_on_num                :    16;  /*Configures the power up time for VDDQ.*/
            uint32_t reserved24                    :    8;  /*Reserved.*/
        };
        uint32_t val;
    } wr_tim_conf1;
    union {
        struct {
            uint32_t reg_pwr_off_num               :    16;  /*Configures the power outage time for VDDQ.*/
            uint32_t reserved16                    :    16;  /*Reserved.*/
        };
        uint32_t val;
    } wr_tim_conf2;
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
    union {
        struct {
            uint32_t reg_efuse_date                :    28;  /*Stores eFuse version.*/
            uint32_t reserved28                    :    4;  /*Reserved.*/
        };
        uint32_t val;
    } date;
} efuse_dev_t;
extern efuse_dev_t EFUSE;
#ifdef __cplusplus
}
#endif



#endif /*_SOC_EFUSE_STRUCT_H_ */
