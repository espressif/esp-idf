/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: PGM Data Register */
/** Type of pgm_data0 register
 *  Register 0 that stores data to be programmed.
 */
typedef union {
    struct {
        /** pgm_data_0 : R/W; bitpos: [31:0]; default: 0;
         *  The content of the 0th 32-bit data to be programmed.
         */
        uint32_t pgm_data_0:32;
    };
    uint32_t val;
} efuse_pgm_data0_reg_t;

/** Type of pgm_data1 register
 *  Register 1 that stores data to be programmed.
 */
typedef union {
    struct {
        /** pgm_data_1 : R/W; bitpos: [31:0]; default: 0;
         *  The content of the 1st 32-bit data to be programmed.
         */
        uint32_t pgm_data_1:32;
    };
    uint32_t val;
} efuse_pgm_data1_reg_t;

/** Type of pgm_data2 register
 *  Register 2 that stores data to be programmed.
 */
typedef union {
    struct {
        /** pgm_data_2 : R/W; bitpos: [31:0]; default: 0;
         *  The content of the 2nd 32-bit data to be programmed.
         */
        uint32_t pgm_data_2:32;
    };
    uint32_t val;
} efuse_pgm_data2_reg_t;

/** Type of pgm_data3 register
 *  Register 3 that stores data to be programmed.
 */
typedef union {
    struct {
        /** pgm_data_3 : R/W; bitpos: [31:0]; default: 0;
         *  The content of the 3rd 32-bit data to be programmed.
         */
        uint32_t pgm_data_3:32;
    };
    uint32_t val;
} efuse_pgm_data3_reg_t;

/** Type of pgm_data4 register
 *  Register 4 that stores data to be programmed.
 */
typedef union {
    struct {
        /** pgm_data_4 : R/W; bitpos: [31:0]; default: 0;
         *  The content of the 4th 32-bit data to be programmed.
         */
        uint32_t pgm_data_4:32;
    };
    uint32_t val;
} efuse_pgm_data4_reg_t;

/** Type of pgm_data5 register
 *  Register 5 that stores data to be programmed.
 */
typedef union {
    struct {
        /** pgm_data_5 : R/W; bitpos: [31:0]; default: 0;
         *  The content of the 5th 32-bit data to be programmed.
         */
        uint32_t pgm_data_5:32;
    };
    uint32_t val;
} efuse_pgm_data5_reg_t;

/** Type of pgm_data6 register
 *  Register 6 that stores data to be programmed.
 */
typedef union {
    struct {
        /** pgm_data_6 : R/W; bitpos: [31:0]; default: 0;
         *  The content of the 6th 32-bit data to be programmed.
         */
        uint32_t pgm_data_6:32;
    };
    uint32_t val;
} efuse_pgm_data6_reg_t;

/** Type of pgm_data7 register
 *  Register 7 that stores data to be programmed.
 */
typedef union {
    struct {
        /** pgm_data_7 : R/W; bitpos: [31:0]; default: 0;
         *  The content of the 7th 32-bit data to be programmed.
         */
        uint32_t pgm_data_7:32;
    };
    uint32_t val;
} efuse_pgm_data7_reg_t;

/** Type of pgm_check_value0 register
 *  Register 0 that stores the RS code to be programmed.
 */
typedef union {
    struct {
        /** pgm_rs_data_0 : R/W; bitpos: [31:0]; default: 0;
         *  The content of the 0th 32-bit RS code to be programmed.
         */
        uint32_t pgm_rs_data_0:32;
    };
    uint32_t val;
} efuse_pgm_check_value0_reg_t;

/** Type of pgm_check_value1 register
 *  Register 1 that stores the RS code to be programmed.
 */
typedef union {
    struct {
        /** pgm_rs_data_1 : R/W; bitpos: [31:0]; default: 0;
         *  The content of the 1st 32-bit RS code to be programmed.
         */
        uint32_t pgm_rs_data_1:32;
    };
    uint32_t val;
} efuse_pgm_check_value1_reg_t;

/** Type of pgm_check_value2 register
 *  Register 2 that stores the RS code to be programmed.
 */
typedef union {
    struct {
        /** pgm_rs_data_2 : R/W; bitpos: [31:0]; default: 0;
         *  The content of the 2nd 32-bit RS code to be programmed.
         */
        uint32_t pgm_rs_data_2:32;
    };
    uint32_t val;
} efuse_pgm_check_value2_reg_t;


/** Group: Read Data Register */
/** Type of rd_wr_dis register
 *  BLOCK0 data register 0.
 */
typedef union {
    struct {
        /** wr_dis : RO; bitpos: [7:0]; default: 0;
         *  Disable programming of individual eFuses.
         */
        uint32_t wr_dis:8;
        /** reserved_0_8 : RW; bitpos: [31:8]; default: 0; */
        uint32_t reserved_0_8:24;
    };
    uint32_t val;
} efuse_rd_wr_dis_reg_t;

/** Type of rd_repeat_data0 register
 *  BLOCK0 data register 1.
 */
typedef union {
    struct {
        /** rd_dis : RO; bitpos: [1:0]; default: 0;
         *  The bit be set to disable software read high/low 128-bit of BLK3.
         */
        uint32_t rd_dis:2;
        /** wdt_delay_sel : RO; bitpos: [3:2]; default: 0;
         *  Selects RTC watchdog timeout threshold, in unit of slow clock cycle. 0: 40000. 1:
         *  80000. 2: 160000. 3:320000.
         */
        uint32_t wdt_delay_sel:2;
        /** dis_pad_jtag : RO; bitpos: [4]; default: 0;
         *  Set this bit to disable pad jtag.
         */
        uint32_t dis_pad_jtag:1;
        /** dis_download_icache : RO; bitpos: [5]; default: 0;
         *  The bit be set to disable icache in download mode.
         */
        uint32_t dis_download_icache:1;
        /** dis_download_manual_encrypt : RO; bitpos: [6]; default: 0;
         *  The bit be set to disable manual encryption.
         */
        uint32_t dis_download_manual_encrypt:1;
        /** spi_boot_crypt_cnt : RO; bitpos: [9:7]; default: 0;
         *  These bits be set to enable SPI boot encrypt/decrypt. Odd number of 1: enable. even
         *  number of 1: disable.
         */
        uint32_t spi_boot_crypt_cnt:3;
        /** xts_key_length_256 : RO; bitpos: [10]; default: 0;
         *  The bit be set means XTS_AES use the whole 256-bit efuse data in BLOCK3. Otherwise,
         *  XTS_AES use 128-bit eFuse data in BLOCK3.
         */
        uint32_t xts_key_length_256:1;
        /** uart_print_control : RO; bitpos: [12:11]; default: 0;
         *  Set this bit to disable usb printing.
         */
        uint32_t uart_print_control:2;
        /** force_send_resume : RO; bitpos: [13]; default: 0;
         *  Set this bit to force ROM code to send a resume command during SPI boot.
         */
        uint32_t force_send_resume:1;
        /** dis_download_mode : RO; bitpos: [14]; default: 0;
         *  Set this bit to disable download mode (boot_mode[3:0] = 0, 1, 2, 4, 5, 6, 7).
         */
        uint32_t dis_download_mode:1;
        /** dis_direct_boot : RO; bitpos: [15]; default: 0;
         *  This bit set means disable direct_boot mode.
         */
        uint32_t dis_direct_boot:1;
        /** enable_security_download : RO; bitpos: [16]; default: 0;
         *  Set this bit to enable secure UART download mode.
         */
        uint32_t enable_security_download:1;
        /** flash_tpuw : RO; bitpos: [20:17]; default: 0;
         *  Configures flash waiting time after power-up, in unit of ms. If the value is less
         *  than 15, the waiting time is the configurable value.  Otherwise, the waiting time
         *  is twice the configurable value.
         */
        uint32_t flash_tpuw:4;
        /** secure_boot_en : RO; bitpos: [21]; default: 0;
         *  The bit be set to enable secure boot.
         */
        uint32_t secure_boot_en:1;
        /** secure_version : R; bitpos: [25:22]; default: 0;
         *  Secure version for anti-rollback
         */
        uint32_t secure_version:4;
        /** custom_mac_used : R; bitpos: [26]; default: 0;
         *  True if MAC_CUSTOM is burned
         */
        uint32_t custom_mac_used:1;
        /** disable_wafer_version_major : R; bitpos: [27]; default: 0;
         *  Disables check of wafer version major
         */
        uint32_t disable_wafer_version_major:1;
        /** disable_blk_version_major : R; bitpos: [28]; default: 0;
         *  Disables check of blk version major
         */
        uint32_t disable_blk_version_major:1;
        /** reserved_0_61 : R; bitpos: [31:29]; default: 0;
         *  reserved
         */
        uint32_t reserved_0_61:3;
    };
    uint32_t val;
} efuse_rd_repeat_data0_reg_t;

/** Type of rd_blk1_data0 register
 *  BLOCK1 data register 0.
 */
typedef union {
    struct {
        /** custom_mac : R; bitpos: [31:0]; default: 0;
         *  Custom MAC address
         */
        uint32_t custom_mac:32;
    };
    uint32_t val;
} efuse_rd_blk1_data0_reg_t;

/** Type of rd_blk1_data1 register
 *  BLOCK1 data register 1.
 */
typedef union {
    struct {
        /** custom_mac_1 : R; bitpos: [15:0]; default: 0;
         *  Custom MAC address
         */
        uint32_t custom_mac_1:16;
        /** reserved_1_48 : R; bitpos: [31:16]; default: 0;
         *  reserved
         */
        uint32_t reserved_1_48:16;
    };
    uint32_t val;
} efuse_rd_blk1_data1_reg_t;

/** Type of rd_blk1_data2 register
 *  BLOCK1 data register 2.
 */
typedef union {
    struct {
        /** system_data2 : RO; bitpos: [23:0]; default: 0;
         *  Stores the bits [64:87] of system data.
         */
        uint32_t system_data2:24;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} efuse_rd_blk1_data2_reg_t;

/** Type of rd_blk2_data0 register
 *  Register 0 of BLOCK2.
 */
typedef union {
    struct {
        /** mac : R; bitpos: [31:0]; default: 0;
         *  MAC address
         */
        uint32_t mac:32;
    };
    uint32_t val;
} efuse_rd_blk2_data0_reg_t;

/** Type of rd_blk2_data1 register
 *  Register 1 of BLOCK2.
 */
typedef union {
    struct {
        /** mac_1 : R; bitpos: [15:0]; default: 0;
         *  MAC address
         */
        uint32_t mac_1:16;
        /** wafer_version_minor : R; bitpos: [19:16]; default: 0;
         *  WAFER_VERSION_MINOR
         */
        uint32_t wafer_version_minor:4;
        /** wafer_version_major : R; bitpos: [21:20]; default: 0;
         *  WAFER_VERSION_MAJOR
         */
        uint32_t wafer_version_major:2;
        /** pkg_version : R; bitpos: [24:22]; default: 0;
         *  EFUSE_PKG_VERSION
         */
        uint32_t pkg_version:3;
        /** blk_version_minor : R; bitpos: [27:25]; default: 0;
         *  Minor version of BLOCK2
         */
        uint32_t blk_version_minor:3;
        /** blk_version_major : R; bitpos: [29:28]; default: 0;
         *  Major version of BLOCK2
         */
        uint32_t blk_version_major:2;
        /** ocode : R; bitpos: [31:30]; default: 0;
         *  OCode
         */
        uint32_t ocode:2;
    };
    uint32_t val;
} efuse_rd_blk2_data1_reg_t;

/** Type of rd_blk2_data2 register
 *  Register 2 of BLOCK2.
 */
typedef union {
    struct {
        /** ocode_1 : R; bitpos: [4:0]; default: 0;
         *  OCode
         */
        uint32_t ocode_1:5;
        /** temp_calib : R; bitpos: [13:5]; default: 0;
         *  Temperature calibration data
         */
        uint32_t temp_calib:9;
        /** adc1_init_code_atten0 : R; bitpos: [21:14]; default: 0;
         *  ADC1 init code at atten0
         */
        uint32_t adc1_init_code_atten0:8;
        /** adc1_init_code_atten3 : R; bitpos: [26:22]; default: 0;
         *  ADC1 init code at atten3
         */
        uint32_t adc1_init_code_atten3:5;
        /** adc1_cal_vol_atten0 : R; bitpos: [31:27]; default: 0;
         *  ADC1 calibration voltage at atten0
         */
        uint32_t adc1_cal_vol_atten0:5;
    };
    uint32_t val;
} efuse_rd_blk2_data2_reg_t;

/** Type of rd_blk2_data3 register
 *  Register 3 of BLOCK2.
 */
typedef union {
    struct {
        /** adc1_cal_vol_atten0_1 : R; bitpos: [2:0]; default: 0;
         *  ADC1 calibration voltage at atten0
         */
        uint32_t adc1_cal_vol_atten0_1:3;
        /** adc1_cal_vol_atten3 : R; bitpos: [8:3]; default: 0;
         *  ADC1 calibration voltage at atten3
         */
        uint32_t adc1_cal_vol_atten3:6;
        /** dig_dbias_hvt : R; bitpos: [13:9]; default: 0;
         *  BLOCK2 digital dbias when hvt
         */
        uint32_t dig_dbias_hvt:5;
        /** dig_ldo_slp_dbias2 : R; bitpos: [20:14]; default: 0;
         *  BLOCK2 DIG_LDO_DBG0_DBIAS2
         */
        uint32_t dig_ldo_slp_dbias2:7;
        /** dig_ldo_slp_dbias26 : R; bitpos: [28:21]; default: 0;
         *  BLOCK2 DIG_LDO_DBG0_DBIAS26
         */
        uint32_t dig_ldo_slp_dbias26:8;
        /** dig_ldo_act_dbias26 : R; bitpos: [31:29]; default: 0;
         *  BLOCK2 DIG_LDO_ACT_DBIAS26
         */
        uint32_t dig_ldo_act_dbias26:3;
    };
    uint32_t val;
} efuse_rd_blk2_data3_reg_t;

/** Type of rd_blk2_data4 register
 *  Register 4 of BLOCK2.
 */
typedef union {
    struct {
        /** dig_ldo_act_dbias26_1 : R; bitpos: [2:0]; default: 0;
         *  BLOCK2 DIG_LDO_ACT_DBIAS26
         */
        uint32_t dig_ldo_act_dbias26_1:3;
        /** dig_ldo_act_stepd10 : R; bitpos: [6:3]; default: 0;
         *  BLOCK2 DIG_LDO_ACT_STEPD10
         */
        uint32_t dig_ldo_act_stepd10:4;
        /** rtc_ldo_slp_dbias13 : R; bitpos: [13:7]; default: 0;
         *  BLOCK2 DIG_LDO_SLP_DBIAS13
         */
        uint32_t rtc_ldo_slp_dbias13:7;
        /** rtc_ldo_slp_dbias29 : R; bitpos: [22:14]; default: 0;
         *  BLOCK2 DIG_LDO_SLP_DBIAS29
         */
        uint32_t rtc_ldo_slp_dbias29:9;
        /** rtc_ldo_slp_dbias31 : R; bitpos: [28:23]; default: 0;
         *  BLOCK2 DIG_LDO_SLP_DBIAS31
         */
        uint32_t rtc_ldo_slp_dbias31:6;
        /** rtc_ldo_act_dbias31 : R; bitpos: [31:29]; default: 0;
         *  BLOCK2 DIG_LDO_ACT_DBIAS31
         */
        uint32_t rtc_ldo_act_dbias31:3;
    };
    uint32_t val;
} efuse_rd_blk2_data4_reg_t;

/** Type of rd_blk2_data5 register
 *  Register 5 of BLOCK2.
 */
typedef union {
    struct {
        /** rtc_ldo_act_dbias31_1 : R; bitpos: [2:0]; default: 0;
         *  BLOCK2 DIG_LDO_ACT_DBIAS31
         */
        uint32_t rtc_ldo_act_dbias31_1:3;
        /** rtc_ldo_act_dbias13 : R; bitpos: [10:3]; default: 0;
         *  BLOCK2 DIG_LDO_ACT_DBIAS13
         */
        uint32_t rtc_ldo_act_dbias13:8;
        /** reserved_2_171 : R; bitpos: [31:11]; default: 0;
         *  reserved
         */
        uint32_t reserved_2_171:21;
    };
    uint32_t val;
} efuse_rd_blk2_data5_reg_t;

/** Type of rd_blk2_data6 register
 *  Register 6 of BLOCK2.
 */
typedef union {
    struct {
        /** adc_calibration_3 : RO; bitpos: [10:0]; default: 0;
         *  Store the bit [86:96] of ADC calibration data.
         */
        uint32_t adc_calibration_3:11;
        /** blk2_reserved_data_0 : RO; bitpos: [31:11]; default: 0;
         *  Store the bit [0:20] of block2 reserved data.
         */
        uint32_t blk2_reserved_data_0:21;
    };
    uint32_t val;
} efuse_rd_blk2_data6_reg_t;

/** Type of rd_blk2_data7 register
 *  Register 7 of BLOCK2.
 */
typedef union {
    struct {
        /** blk2_reserved_data_1 : RO; bitpos: [31:0]; default: 0;
         *  Store the bit [21:52] of block2 reserved data.
         */
        uint32_t blk2_reserved_data_1:32;
    };
    uint32_t val;
} efuse_rd_blk2_data7_reg_t;

/** Type of rd_blk3_data0 register
 *  Register 0 of BLOCK3.
 */
typedef union {
    struct {
        /** blk3_data0 : RO; bitpos: [31:0]; default: 0;
         *  Store the first 32-bit of Block3.
         */
        uint32_t blk3_data0:32;
    };
    uint32_t val;
} efuse_rd_blk3_data0_reg_t;

/** Type of rd_blk3_data1 register
 *  Register 1 of BLOCK3.
 */
typedef union {
    struct {
        /** blk3_data1 : RO; bitpos: [31:0]; default: 0;
         *  Store the second 32-bit of Block3.
         */
        uint32_t blk3_data1:32;
    };
    uint32_t val;
} efuse_rd_blk3_data1_reg_t;

/** Type of rd_blk3_data2 register
 *  Register 2 of BLOCK3.
 */
typedef union {
    struct {
        /** blk3_data2 : RO; bitpos: [31:0]; default: 0;
         *  Store the third 32-bit of Block3.
         */
        uint32_t blk3_data2:32;
    };
    uint32_t val;
} efuse_rd_blk3_data2_reg_t;

/** Type of rd_blk3_data3 register
 *  Register 3 of BLOCK3.
 */
typedef union {
    struct {
        /** blk3_data3 : RO; bitpos: [31:0]; default: 0;
         *  Store the fourth 32-bit of Block3.
         */
        uint32_t blk3_data3:32;
    };
    uint32_t val;
} efuse_rd_blk3_data3_reg_t;

/** Type of rd_blk3_data4 register
 *  Register 4 of BLOCK3.
 */
typedef union {
    struct {
        /** blk3_data4 : RO; bitpos: [31:0]; default: 0;
         *  Store the fifth 32-bit of Block3.
         */
        uint32_t blk3_data4:32;
    };
    uint32_t val;
} efuse_rd_blk3_data4_reg_t;

/** Type of rd_blk3_data5 register
 *  Register 5 of BLOCK3.
 */
typedef union {
    struct {
        /** blk3_data5 : RO; bitpos: [31:0]; default: 0;
         *  Store the sixth 32-bit of Block3.
         */
        uint32_t blk3_data5:32;
    };
    uint32_t val;
} efuse_rd_blk3_data5_reg_t;

/** Type of rd_blk3_data6 register
 *  Register 6 of BLOCK3.
 */
typedef union {
    struct {
        /** blk3_data6 : RO; bitpos: [31:0]; default: 0;
         *  Store the seventh 32-bit of Block3.
         */
        uint32_t blk3_data6:32;
    };
    uint32_t val;
} efuse_rd_blk3_data6_reg_t;

/** Type of rd_blk3_data7 register
 *  Register 7 of BLOCK3.
 */
typedef union {
    struct {
        /** blk3_data7 : RO; bitpos: [31:0]; default: 0;
         *  Store the eighth 32-bit of Block3.
         */
        uint32_t blk3_data7:32;
    };
    uint32_t val;
} efuse_rd_blk3_data7_reg_t;


/** Group: Report Register */
/** Type of rd_repeat_err register
 *  Programming error record register 0 of BLOCK0.
 */
typedef union {
    struct {
        /** rd_dis_err : RO; bitpos: [1:0]; default: 0;
         *  If any bit in RD_DIS is 1, then it indicates a programming error.
         */
        uint32_t rd_dis_err:2;
        /** wdt_delay_sel_err : RO; bitpos: [3:2]; default: 0;
         *  If any bit in WDT_DELAY_SEL is 1, then it indicates a programming error.
         */
        uint32_t wdt_delay_sel_err:2;
        /** dis_pad_jtag_err : RO; bitpos: [4]; default: 0;
         *  If any bit in DIS_PAD_JTAG is 1, then it indicates a programming error.
         */
        uint32_t dis_pad_jtag_err:1;
        /** dis_download_icache_err : RO; bitpos: [5]; default: 0;
         *  If any bit in this filed is 1, then it indicates a programming error.
         */
        uint32_t dis_download_icache_err:1;
        /** dis_download_manual_encrypt_err : RO; bitpos: [6]; default: 0;
         *  If any bit in DIS_DOWNLOAD_MANUAL_ENCRYPT is 1, then it indicates a programming
         *  error.
         */
        uint32_t dis_download_manual_encrypt_err:1;
        /** spi_boot_encrypt_decrypt_cnt_err : RO; bitpos: [9:7]; default: 0;
         *  If any bit in SPI_BOOT_ENCRYPT_DECRYPT_CNT is 1, then it indicates a programming
         *  error.
         */
        uint32_t spi_boot_encrypt_decrypt_cnt_err:3;
        /** xts_key_length_256_err : RO; bitpos: [10]; default: 0;
         *  If any bit in XTS_KEY_LENGTH_256 is 1, then it indicates a programming error.
         */
        uint32_t xts_key_length_256_err:1;
        /** uart_print_control_err : RO; bitpos: [12:11]; default: 0;
         *  If any bit in UART_PRINT_CONTROL is 1, then it indicates a programming error.
         */
        uint32_t uart_print_control_err:2;
        /** force_send_resume_err : RO; bitpos: [13]; default: 0;
         *  If any bit in FORCE_SEND_RESUME is 1, then it indicates a programming error.
         */
        uint32_t force_send_resume_err:1;
        /** dis_download_mode_err : RO; bitpos: [14]; default: 0;
         *  If any bit in this filed is 1, then it indicates a programming error.
         */
        uint32_t dis_download_mode_err:1;
        /** dis_direct_boot_err : RO; bitpos: [15]; default: 0;
         *  If any bit in this filed is 1, then it indicates a programming error.
         */
        uint32_t dis_direct_boot_err:1;
        /** enable_security_download_err : RO; bitpos: [16]; default: 0;
         *  If any bit in this filed is 1, then it indicates a programming error.
         */
        uint32_t enable_security_download_err:1;
        /** flash_tpuw_err : RO; bitpos: [20:17]; default: 0;
         *  If any bit in this filed is 1, then it indicates a programming error.
         */
        uint32_t flash_tpuw_err:4;
        /** secure_boot_en_err : RO; bitpos: [21]; default: 0;
         *  If any bit in this filed is 1, then it indicates a programming error.
         */
        uint32_t secure_boot_en_err:1;
        /** rpt4_reserved_err : RO; bitpos: [31:22]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved_err:10;
    };
    uint32_t val;
} efuse_rd_repeat_err_reg_t;

/** Type of rd_rs_err register
 *  Programming error record register 0 of BLOCK1-10.
 */
typedef union {
    struct {
        /** blk1_err_num : RO; bitpos: [2:0]; default: 0;
         *  The value of this signal means the number of error bytes in block1.
         */
        uint32_t blk1_err_num:3;
        /** blk1_fail : RO; bitpos: [3]; default: 0;
         *  0: Means no failure and that the data of block1 is reliable 1: Means that
         *  programming user data failed and the number of error bytes is over 6.
         */
        uint32_t blk1_fail:1;
        /** blk2_err_num : RO; bitpos: [6:4]; default: 0;
         *  The value of this signal means the number of error bytes in block2.
         */
        uint32_t blk2_err_num:3;
        /** blk2_fail : RO; bitpos: [7]; default: 0;
         *  0: Means no failure and that the data of block2 is reliable 1: Means that
         *  programming user data failed and the number of error bytes is over 6.
         */
        uint32_t blk2_fail:1;
        /** blk3_err_num : RO; bitpos: [10:8]; default: 0;
         *  The value of this signal means the number of error bytes in block3.
         */
        uint32_t blk3_err_num:3;
        /** blk3_fail : RO; bitpos: [11]; default: 0;
         *  0: Means no failure and that the block3 data is reliable 1: Means that programming
         *  user data failed and the number of error bytes is over 6.
         */
        uint32_t blk3_fail:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} efuse_rd_rs_err_reg_t;


/** Group: Configuration Register */
/** Type of clk register
 *  eFuse clcok configuration register.
 */
typedef union {
    struct {
        /** efuse_mem_force_pd : R/W; bitpos: [0]; default: 0;
         *  Set this bit to force eFuse SRAM into power-saving mode.
         */
        uint32_t efuse_mem_force_pd:1;
        /** mem_clk_force_on : R/W; bitpos: [1]; default: 1;
         *  Set this bit and force to activate clock signal of eFuse SRAM.
         */
        uint32_t mem_clk_force_on:1;
        /** efuse_mem_force_pu : R/W; bitpos: [2]; default: 0;
         *  Set this bit to force eFuse SRAM into working mode.
         */
        uint32_t efuse_mem_force_pu:1;
        uint32_t reserved_3:13;
        /** clk_en : R/W; bitpos: [16]; default: 0;
         *  Set this bit and force to enable clock signal of eFuse memory.
         */
        uint32_t clk_en:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} efuse_clk_reg_t;

/** Type of conf register
 *  eFuse operation mode configuraiton register
 */
typedef union {
    struct {
        /** op_code : R/W; bitpos: [15:0]; default: 0;
         *  0x5A5A: Operate programming command 0x5AA5: Operate read command.
         */
        uint32_t op_code:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} efuse_conf_reg_t;

/** Type of cmd register
 *  eFuse command register.
 */
typedef union {
    struct {
        /** read_cmd : R/W/SC; bitpos: [0]; default: 0;
         *  Set this bit to send read command.
         */
        uint32_t read_cmd:1;
        /** pgm_cmd : R/W/SC; bitpos: [1]; default: 0;
         *  Set this bit to send programming command.
         */
        uint32_t pgm_cmd:1;
        /** blk_num : R/W; bitpos: [3:2]; default: 0;
         *  The serial number of the block to be programmed. Value 0-3 corresponds to block
         *  number 0-3, respectively.
         */
        uint32_t blk_num:2;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} efuse_cmd_reg_t;

/** Type of dac_conf register
 *  Controls the eFuse programming voltage.
 */
typedef union {
    struct {
        /** dac_clk_div : R/W; bitpos: [7:0]; default: 28;
         *  Controls the division factor of the rising clock of the programming voltage.
         */
        uint32_t dac_clk_div:8;
        /** dac_clk_pad_sel : R/W; bitpos: [8]; default: 0;
         *  Don't care.
         */
        uint32_t dac_clk_pad_sel:1;
        /** dac_num : R/W; bitpos: [16:9]; default: 255;
         *  Controls the rising period of the programming voltage.
         */
        uint32_t dac_num:8;
        /** oe_clr : R/W; bitpos: [17]; default: 0;
         *  Reduces the power supply of the programming voltage.
         */
        uint32_t oe_clr:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} efuse_dac_conf_reg_t;

/** Type of rd_tim_conf register
 *  Configures read timing parameters.
 */
typedef union {
    struct {
        /** thr_a : R/W; bitpos: [7:0]; default: 1;
         *  Configures hold time for efuse read.
         */
        uint32_t thr_a:8;
        /** trd : R/W; bitpos: [15:8]; default: 2;
         *  Configures pulse time for efuse read.
         */
        uint32_t trd:8;
        /** tsur_a : R/W; bitpos: [23:16]; default: 1;
         *  Configures setup time for efuse read.
         */
        uint32_t tsur_a:8;
        /** read_init_num : R/W; bitpos: [31:24]; default: 18;
         *  Configures the initial read time of eFuse.
         */
        uint32_t read_init_num:8;
    };
    uint32_t val;
} efuse_rd_tim_conf_reg_t;

/** Type of wr_tim_conf0 register
 *  Configurarion register 0 of eFuse programming timing parameters.
 */
typedef union {
    struct {
        /** thp_a : R/W; bitpos: [7:0]; default: 1;
         *  Configures hold time for efuse program.
         */
        uint32_t thp_a:8;
        /** tpgm_inactive : R/W; bitpos: [15:8]; default: 1;
         *  Configures pulse time for burning '0' bit.
         */
        uint32_t tpgm_inactive:8;
        /** tpgm : R/W; bitpos: [31:16]; default: 200;
         *  Configures pulse time for burning '1' bit.
         */
        uint32_t tpgm:16;
    };
    uint32_t val;
} efuse_wr_tim_conf0_reg_t;

/** Type of wr_tim_conf1 register
 *  Configurarion register 1 of eFuse programming timing parameters.
 */
typedef union {
    struct {
        /** tsup_a : R/W; bitpos: [7:0]; default: 1;
         *  Configures setup time for efuse program.
         */
        uint32_t tsup_a:8;
        /** pwr_on_num : R/W; bitpos: [23:8]; default: 12288;
         *  Configures the power up time for VDDQ.
         */
        uint32_t pwr_on_num:16;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} efuse_wr_tim_conf1_reg_t;

/** Type of wr_tim_conf2 register
 *  Configurarion register 2 of eFuse programming timing parameters.
 */
typedef union {
    struct {
        /** pwr_off_num : R/W; bitpos: [15:0]; default: 400;
         *  Configures the power outage time for VDDQ.
         */
        uint32_t pwr_off_num:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} efuse_wr_tim_conf2_reg_t;


/** Group: Status Register */
/** Type of status register
 *  eFuse status register.
 */
typedef union {
    struct {
        /** state : RO; bitpos: [3:0]; default: 0;
         *  Indicates the state of the eFuse state machine.
         */
        uint32_t state:4;
        /** otp_load_sw : RO; bitpos: [4]; default: 0;
         *  The value of OTP_LOAD_SW.
         */
        uint32_t otp_load_sw:1;
        /** otp_vddq_c_sync2 : RO; bitpos: [5]; default: 0;
         *  The value of OTP_VDDQ_C_SYNC2.
         */
        uint32_t otp_vddq_c_sync2:1;
        /** otp_strobe_sw : RO; bitpos: [6]; default: 0;
         *  The value of OTP_STROBE_SW.
         */
        uint32_t otp_strobe_sw:1;
        /** otp_csb_sw : RO; bitpos: [7]; default: 0;
         *  The value of OTP_CSB_SW.
         */
        uint32_t otp_csb_sw:1;
        /** otp_pgenb_sw : RO; bitpos: [8]; default: 0;
         *  The value of OTP_PGENB_SW.
         */
        uint32_t otp_pgenb_sw:1;
        /** otp_vddq_is_sw : RO; bitpos: [9]; default: 0;
         *  The value of OTP_VDDQ_IS_SW.
         */
        uint32_t otp_vddq_is_sw:1;
        /** blk0_valid_bit_cnt : RO; bitpos: [15:10]; default: 0;
         *  Record the number of bit '1' in BLOCK0.
         */
        uint32_t blk0_valid_bit_cnt:6;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} efuse_status_reg_t;


/** Group: Interrupt Register */
/** Type of int_raw register
 *  eFuse raw interrupt register.
 */
typedef union {
    struct {
        /** read_done_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw bit signal for read_done interrupt.
         */
        uint32_t read_done_int_raw:1;
        /** pgm_done_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw bit signal for pgm_done interrupt.
         */
        uint32_t pgm_done_int_raw:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} efuse_int_raw_reg_t;

/** Type of int_st register
 *  eFuse interrupt status register.
 */
typedef union {
    struct {
        /** read_done_int_st : RO; bitpos: [0]; default: 0;
         *  The status signal for read_done interrupt.
         */
        uint32_t read_done_int_st:1;
        /** pgm_done_int_st : RO; bitpos: [1]; default: 0;
         *  The status signal for pgm_done interrupt.
         */
        uint32_t pgm_done_int_st:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} efuse_int_st_reg_t;

/** Type of int_ena register
 *  eFuse interrupt enable register.
 */
typedef union {
    struct {
        /** read_done_int_ena : R/W; bitpos: [0]; default: 0;
         *  The enable signal for read_done interrupt.
         */
        uint32_t read_done_int_ena:1;
        /** pgm_done_int_ena : R/W; bitpos: [1]; default: 0;
         *  The enable signal for pgm_done interrupt.
         */
        uint32_t pgm_done_int_ena:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} efuse_int_ena_reg_t;

/** Type of int_clr register
 *  eFuse interrupt clear register.
 */
typedef union {
    struct {
        /** read_done_int_clr : WT; bitpos: [0]; default: 0;
         *  The clear signal for read_done interrupt.
         */
        uint32_t read_done_int_clr:1;
        /** pgm_done_int_clr : WT; bitpos: [1]; default: 0;
         *  The clear signal for pgm_done interrupt.
         */
        uint32_t pgm_done_int_clr:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} efuse_int_clr_reg_t;


/** Group: Version Register */
/** Type of date register
 *  eFuse version register.
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 34636176;
         *  Stores eFuse version.
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} efuse_date_reg_t;


typedef struct {
    volatile efuse_pgm_data0_reg_t pgm_data0;
    volatile efuse_pgm_data1_reg_t pgm_data1;
    volatile efuse_pgm_data2_reg_t pgm_data2;
    volatile efuse_pgm_data3_reg_t pgm_data3;
    volatile efuse_pgm_data4_reg_t pgm_data4;
    volatile efuse_pgm_data5_reg_t pgm_data5;
    volatile efuse_pgm_data6_reg_t pgm_data6;
    volatile efuse_pgm_data7_reg_t pgm_data7;
    volatile efuse_pgm_check_value0_reg_t pgm_check_value0;
    volatile efuse_pgm_check_value1_reg_t pgm_check_value1;
    volatile efuse_pgm_check_value2_reg_t pgm_check_value2;
    volatile efuse_rd_wr_dis_reg_t rd_wr_dis;
    volatile efuse_rd_repeat_data0_reg_t rd_repeat_data0;
    volatile efuse_rd_blk1_data0_reg_t rd_blk1_data0;
    volatile efuse_rd_blk1_data1_reg_t rd_blk1_data1;
    volatile efuse_rd_blk1_data2_reg_t rd_blk1_data2;
    volatile efuse_rd_blk2_data0_reg_t rd_blk2_data0;
    volatile efuse_rd_blk2_data1_reg_t rd_blk2_data1;
    volatile efuse_rd_blk2_data2_reg_t rd_blk2_data2;
    volatile efuse_rd_blk2_data3_reg_t rd_blk2_data3;
    volatile efuse_rd_blk2_data4_reg_t rd_blk2_data4;
    volatile efuse_rd_blk2_data5_reg_t rd_blk2_data5;
    volatile efuse_rd_blk2_data6_reg_t rd_blk2_data6;
    volatile efuse_rd_blk2_data7_reg_t rd_blk2_data7;
    volatile efuse_rd_blk3_data0_reg_t rd_blk3_data0;
    volatile efuse_rd_blk3_data1_reg_t rd_blk3_data1;
    volatile efuse_rd_blk3_data2_reg_t rd_blk3_data2;
    volatile efuse_rd_blk3_data3_reg_t rd_blk3_data3;
    volatile efuse_rd_blk3_data4_reg_t rd_blk3_data4;
    volatile efuse_rd_blk3_data5_reg_t rd_blk3_data5;
    volatile efuse_rd_blk3_data6_reg_t rd_blk3_data6;
    volatile efuse_rd_blk3_data7_reg_t rd_blk3_data7;
    volatile efuse_rd_repeat_err_reg_t rd_repeat_err;
    volatile efuse_rd_rs_err_reg_t rd_rs_err;
    volatile efuse_clk_reg_t clk;
    volatile efuse_conf_reg_t conf;
    volatile efuse_status_reg_t status;
    volatile efuse_cmd_reg_t cmd;
    volatile efuse_int_raw_reg_t int_raw;
    volatile efuse_int_st_reg_t int_st;
    uint32_t reserved_0a0[24];
    volatile efuse_int_ena_reg_t int_ena;
    volatile efuse_int_clr_reg_t int_clr;
    volatile efuse_dac_conf_reg_t dac_conf;
    volatile efuse_rd_tim_conf_reg_t rd_tim_conf;
    volatile efuse_wr_tim_conf0_reg_t wr_tim_conf0;
    volatile efuse_wr_tim_conf1_reg_t wr_tim_conf1;
    volatile efuse_wr_tim_conf2_reg_t wr_tim_conf2;
    uint32_t reserved_11c[56];
    volatile efuse_date_reg_t date;
} efuse_dev_t;

extern efuse_dev_t EFUSE;

#ifndef __cplusplus
_Static_assert(sizeof(efuse_dev_t) == 0x200, "Invalid size of efuse_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
