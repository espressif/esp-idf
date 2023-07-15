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
        /** wr_dis : RO; bitpos: [31:0]; default: 0;
         *  Disable programming of individual eFuses.
         */
        uint32_t wr_dis:32;
    };
    uint32_t val;
} efuse_rd_wr_dis_reg_t;

/** Type of rd_repeat_data0 register
 *  BLOCK0 data register 1.
 */
typedef union {
    struct {
        /** rd_dis : RO; bitpos: [6:0]; default: 0;
         *  Set this bit to disable reading from BlOCK4-10.
         */
        uint32_t rd_dis:7;
        /** dis_rtc_ram_boot : RO; bitpos: [7]; default: 0;
         *  Set this bit to disable boot from RTC RAM.
         */
        uint32_t dis_rtc_ram_boot:1;
        /** dis_icache : RO; bitpos: [8]; default: 0;
         *  Set this bit to disable Icache.
         */
        uint32_t dis_icache:1;
        /** dis_usb_jtag : RO; bitpos: [9]; default: 0;
         *  Set this bit to disable function of usb switch to jtag in module of usb device.
         */
        uint32_t dis_usb_jtag:1;
        /** dis_download_icache : RO; bitpos: [10]; default: 0;
         *  Set this bit to disable Icache in download mode (boot_mode[3:0] is 0, 1, 2, 3, 6,
         *  7).
         */
        uint32_t dis_download_icache:1;
        /** dis_usb_serial_jtag : RO; bitpos: [11]; default: 0;
         *  Represents whether USB-Serial-JTAG is disabled. 1: Disabled. 0: Enabled
         */
        uint32_t dis_usb_serial_jtag:1;
        /** dis_force_download : RO; bitpos: [12]; default: 0;
         *  Set this bit to disable the function that forces chip into download mode.
         */
        uint32_t dis_force_download:1;
        /** rpt4_reserved6 : RO; bitpos: [13]; default: 0;
         *  Reserved (used for four backups method).
         */
        uint32_t rpt4_reserved6:1;
        /** dis_twai : RO; bitpos: [14]; default: 0;
         *  Set this bit to disable CAN function.
         */
        uint32_t dis_twai:1;
        /** jtag_sel_enable : RO; bitpos: [15]; default: 0;
         *  Set this bit to enable selection between usb_to_jtag and pad_to_jtag through
         *  strapping gpio10 when both reg_dis_usb_jtag and reg_dis_pad_jtag are equal to 0.
         */
        uint32_t jtag_sel_enable:1;
        /** soft_dis_jtag : RO; bitpos: [18:16]; default: 0;
         *  Set these bits to disable JTAG in the soft way (odd number 1 means disable ). JTAG
         *  can be enabled in HMAC module.
         */
        uint32_t soft_dis_jtag:3;
        /** dis_pad_jtag : RO; bitpos: [19]; default: 0;
         *  Set this bit to disable JTAG in the hard way. JTAG is disabled permanently.
         */
        uint32_t dis_pad_jtag:1;
        /** dis_download_manual_encrypt : RO; bitpos: [20]; default: 0;
         *  Set this bit to disable flash encryption when in download boot modes.
         */
        uint32_t dis_download_manual_encrypt:1;
        /** usb_drefh : RO; bitpos: [22:21]; default: 0;
         *  Controls single-end input threshold vrefh, 1.76 V to 2 V with step of 80 mV, stored
         *  in eFuse.
         */
        uint32_t usb_drefh:2;
        /** usb_drefl : RO; bitpos: [24:23]; default: 0;
         *  Controls single-end input threshold vrefl, 0.8 V to 1.04 V with step of 80 mV,
         *  stored in eFuse.
         */
        uint32_t usb_drefl:2;
        /** usb_exchg_pins : RO; bitpos: [25]; default: 0;
         *  Set this bit to exchange USB D+ and D- pins.
         */
        uint32_t usb_exchg_pins:1;
        /** vdd_spi_as_gpio : RO; bitpos: [26]; default: 0;
         *  Set this bit to vdd spi pin function as gpio.
         */
        uint32_t vdd_spi_as_gpio:1;
        /** btlc_gpio_enable : RO; bitpos: [28:27]; default: 0;
         *  Enable btlc gpio.
         */
        uint32_t btlc_gpio_enable:2;
        /** powerglitch_en : RO; bitpos: [29]; default: 0;
         *  Set this bit to enable power glitch function.
         */
        uint32_t powerglitch_en:1;
        /** power_glitch_dsense : RO; bitpos: [31:30]; default: 0;
         *  Sample delay configuration of power glitch.
         */
        uint32_t power_glitch_dsense:2;
    };
    uint32_t val;
} efuse_rd_repeat_data0_reg_t;

/** Type of rd_repeat_data1 register
 *  BLOCK0 data register 2.
 */
typedef union {
    struct {
        /** rpt4_reserved2 : RO; bitpos: [15:0]; default: 0;
         *  Reserved (used for four backups method).
         */
        uint32_t rpt4_reserved2:16;
        /** wdt_delay_sel : RO; bitpos: [17:16]; default: 0;
         *  Selects RTC watchdog timeout threshold, in unit of slow clock cycle. 0: 40000. 1:
         *  80000. 2: 160000. 3:320000.
         */
        uint32_t wdt_delay_sel:2;
        /** spi_boot_crypt_cnt : RO; bitpos: [20:18]; default: 0;
         *  Set this bit to enable SPI boot encrypt/decrypt. Odd number of 1: enable. even
         *  number of 1: disable.
         */
        uint32_t spi_boot_crypt_cnt:3;
        /** secure_boot_key_revoke0 : RO; bitpos: [21]; default: 0;
         *  Set this bit to enable revoking first secure boot key.
         */
        uint32_t secure_boot_key_revoke0:1;
        /** secure_boot_key_revoke1 : RO; bitpos: [22]; default: 0;
         *  Set this bit to enable revoking second secure boot key.
         */
        uint32_t secure_boot_key_revoke1:1;
        /** secure_boot_key_revoke2 : RO; bitpos: [23]; default: 0;
         *  Set this bit to enable revoking third secure boot key.
         */
        uint32_t secure_boot_key_revoke2:1;
        /** key_purpose_0 : RO; bitpos: [27:24]; default: 0;
         *  Purpose of Key0.
         */
        uint32_t key_purpose_0:4;
        /** key_purpose_1 : RO; bitpos: [31:28]; default: 0;
         *  Purpose of Key1.
         */
        uint32_t key_purpose_1:4;
    };
    uint32_t val;
} efuse_rd_repeat_data1_reg_t;

/** Type of rd_repeat_data2 register
 *  BLOCK0 data register 3.
 */
typedef union {
    struct {
        /** key_purpose_2 : RO; bitpos: [3:0]; default: 0;
         *  Purpose of Key2.
         */
        uint32_t key_purpose_2:4;
        /** key_purpose_3 : RO; bitpos: [7:4]; default: 0;
         *  Purpose of Key3.
         */
        uint32_t key_purpose_3:4;
        /** key_purpose_4 : RO; bitpos: [11:8]; default: 0;
         *  Purpose of Key4.
         */
        uint32_t key_purpose_4:4;
        /** key_purpose_5 : RO; bitpos: [15:12]; default: 0;
         *  Purpose of Key5.
         */
        uint32_t key_purpose_5:4;
        /** rpt4_reserved3 : RO; bitpos: [19:16]; default: 0;
         *  Reserved (used for four backups method).
         */
        uint32_t rpt4_reserved3:4;
        /** secure_boot_en : RO; bitpos: [20]; default: 0;
         *  Set this bit to enable secure boot.
         */
        uint32_t secure_boot_en:1;
        /** secure_boot_aggressive_revoke : RO; bitpos: [21]; default: 0;
         *  Set this bit to enable revoking aggressive secure boot.
         */
        uint32_t secure_boot_aggressive_revoke:1;
        /** rpt4_reserved0 : RO; bitpos: [27:22]; default: 0;
         *  Reserved (used for four backups method).
         */
        uint32_t rpt4_reserved0:6;
        /** flash_tpuw : RO; bitpos: [31:28]; default: 0;
         *  Configures flash waiting time after power-up, in unit of ms. If the value is less
         *  than 15, the waiting time is the configurable value; Otherwise, the waiting time is
         *  twice the configurable value.
         */
        uint32_t flash_tpuw:4;
    };
    uint32_t val;
} efuse_rd_repeat_data2_reg_t;

/** Type of rd_repeat_data3 register
 *  BLOCK0 data register 4.
 */
typedef union {
    struct {
        /** dis_download_mode : RO; bitpos: [0]; default: 0;
         *  Set this bit to disable download mode (boot_mode[3:0] = 0, 1, 2, 3, 6, 7).
         */
        uint32_t dis_download_mode:1;
        /** dis_direct_boot : RO; bitpos: [1]; default: 0;
         *  Disable direct boot mode
         */
        uint32_t dis_direct_boot:1;
        /** dis_usb_serial_jtag_rom_print : RO; bitpos: [2]; default: 0;
         *  Represents whether USB printing is disabled or enabled. 1: Disabled. 0: Enabled
         */
        uint32_t dis_usb_serial_jtag_rom_print:1;
        /** flash_ecc_mode : RO; bitpos: [3]; default: 0;
         *  Set ECC mode in ROM, 0: ROM would Enable Flash ECC 16to18 byte mode. 1:ROM would
         *  use 16to17 byte mode.
         */
        uint32_t flash_ecc_mode:1;
        /** dis_usb_serial_jtag_download_mode : RO; bitpos: [4]; default: 0;
         *  Disable UART download mode through USB-Serial-JTAG
         */
        uint32_t dis_usb_serial_jtag_download_mode:1;
        /** enable_security_download : RO; bitpos: [5]; default: 0;
         *  Set this bit to enable secure UART download mode.
         */
        uint32_t enable_security_download:1;
        /** uart_print_control : RO; bitpos: [7:6]; default: 0;
         *  Set the default UARTboot message output mode. 00: Enabled. 01: Enabled when GPIO8
         *  is low at reset. 10: Enabled when GPIO8 is high at reset. 11:disabled.
         */
        uint32_t uart_print_control:2;
        /** pin_power_selection : RO; bitpos: [8]; default: 0;
         *  GPIO33-GPIO37 power supply selection in ROM code. 0: VDD3P3_CPU. 1: VDD_SPI.
         */
        uint32_t pin_power_selection:1;
        /** flash_type : RO; bitpos: [9]; default: 0;
         *  Set the maximum lines of SPI flash. 0: four lines. 1: eight lines.
         */
        uint32_t flash_type:1;
        /** flash_page_size : RO; bitpos: [11:10]; default: 0;
         *  Set Flash page size.
         */
        uint32_t flash_page_size:2;
        /** flash_ecc_en : RO; bitpos: [12]; default: 0;
         *  Set 1 to enable ECC for flash boot.
         */
        uint32_t flash_ecc_en:1;
        /** force_send_resume : RO; bitpos: [13]; default: 0;
         *  Set this bit to force ROM code to send a resume command during SPI boot.
         */
        uint32_t force_send_resume:1;
        /** secure_version : RO; bitpos: [29:14]; default: 0;
         *  Secure version (used by ESP-IDF anti-rollback feature).
         */
        uint32_t secure_version:16;
        /** reserved_0_158 : R; bitpos: [30]; default: 0;
         *  reserved
         */
        uint32_t reserved_0_158:1;
        /** err_rst_enable : R; bitpos: [31]; default: 0;
         *  Use BLOCK0 to check error record registers
         */
        uint32_t err_rst_enable:1;
    };
    uint32_t val;
} efuse_rd_repeat_data3_reg_t;

/** Type of rd_repeat_data4 register
 *  BLOCK0 data register 5.
 */
typedef union {
    struct {
        /** disable_wafer_version_major : R; bitpos: [0]; default: 0;
         *  Disables check of wafer version major
         */
        uint32_t disable_wafer_version_major:1;
        /** disable_blk_version_major : R; bitpos: [1]; default: 0;
         *  Disables check of blk version major
         */
        uint32_t disable_blk_version_major:1;
        /** reserved_0_162 : R; bitpos: [23:2]; default: 0;
         *  reserved
         */
        uint32_t reserved_0_162:22;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} efuse_rd_repeat_data4_reg_t;

/** Type of rd_mac_spi_sys_0 register
 *  BLOCK1 data register 0.
 */
typedef union {
    struct {
        /** mac_0 : RO; bitpos: [31:0]; default: 0;
         *  Stores the low 32 bits of MAC address.
         */
        uint32_t mac_0:32;
    };
    uint32_t val;
} efuse_rd_mac_spi_sys_0_reg_t;

/** Type of rd_mac_spi_sys_1 register
 *  BLOCK1 data register 1.
 */
typedef union {
    struct {
        /** mac_1 : RO; bitpos: [15:0]; default: 0;
         *  Stores the high 16 bits of MAC address.
         */
        uint32_t mac_1:16;
        /** spi_pad_config_clk : R; bitpos: [21:16]; default: 0;
         *  SPI PAD CLK
         */
        uint32_t spi_pad_config_clk:6;
        /** spi_pad_config_q : R; bitpos: [27:22]; default: 0;
         *  SPI PAD Q(D1)
         */
        uint32_t spi_pad_config_q:6;
        /** spi_pad_config_d : R; bitpos: [31:28]; default: 0;
         *  SPI PAD D(D0)
         */
        uint32_t spi_pad_config_d:4;
    };
    uint32_t val;
} efuse_rd_mac_spi_sys_1_reg_t;

/** Type of rd_mac_spi_sys_2 register
 *  BLOCK1 data register 2.
 */
typedef union {
    struct {
        /** spi_pad_config_d_1 : R; bitpos: [1:0]; default: 0;
         *  SPI PAD D(D0)
         */
        uint32_t spi_pad_config_d_1:2;
        /** spi_pad_config_cs : R; bitpos: [7:2]; default: 0;
         *  SPI PAD CS
         */
        uint32_t spi_pad_config_cs:6;
        /** spi_pad_config_hd : R; bitpos: [13:8]; default: 0;
         *  SPI PAD HD(D3)
         */
        uint32_t spi_pad_config_hd:6;
        /** spi_pad_config_wp : R; bitpos: [19:14]; default: 0;
         *  SPI PAD WP(D2)
         */
        uint32_t spi_pad_config_wp:6;
        /** spi_pad_config_dqs : R; bitpos: [25:20]; default: 0;
         *  SPI PAD DQS
         */
        uint32_t spi_pad_config_dqs:6;
        /** spi_pad_config_d4 : R; bitpos: [31:26]; default: 0;
         *  SPI PAD D4
         */
        uint32_t spi_pad_config_d4:6;
    };
    uint32_t val;
} efuse_rd_mac_spi_sys_2_reg_t;

/** Type of rd_mac_spi_sys_3 register
 *  BLOCK1 data register 3.
 */
typedef union {
    struct {
        /** spi_pad_config_d5 : R; bitpos: [5:0]; default: 0;
         *  SPI PAD D5
         */
        uint32_t spi_pad_config_d5:6;
        /** spi_pad_config_d6 : R; bitpos: [11:6]; default: 0;
         *  SPI PAD D6
         */
        uint32_t spi_pad_config_d6:6;
        /** spi_pad_config_d7 : R; bitpos: [17:12]; default: 0;
         *  SPI PAD D7
         */
        uint32_t spi_pad_config_d7:6;
        /** wafer_version_minor_lo : R; bitpos: [20:18]; default: 0;
         *  WAFER_VERSION_MINOR least significant bits
         */
        uint32_t wafer_version_minor_lo:3;
        /** pkg_version : R; bitpos: [23:21]; default: 0;
         *  Package version
         */
        uint32_t pkg_version:3;
        /** blk_version_minor : R; bitpos: [26:24]; default: 0;
         *  BLK_VERSION_MINOR
         */
        uint32_t blk_version_minor:3;
        /** flash_cap : R; bitpos: [29:27]; default: 0;
         *  Flash capacity
         */
        uint32_t flash_cap:3;
        /** flash_temp : R; bitpos: [31:30]; default: 0;
         *  Flash temperature
         */
        uint32_t flash_temp:2;
    };
    uint32_t val;
} efuse_rd_mac_spi_sys_3_reg_t;

/** Type of rd_mac_spi_sys_4 register
 *  BLOCK1 data register 4.
 */
typedef union {
    struct {
        /** flash_vendor : R; bitpos: [2:0]; default: 0;
         *  Flash vendor
         */
        uint32_t flash_vendor:3;
        /** reserved_1_131 : R; bitpos: [6:3]; default: 0;
         *  reserved
         */
        uint32_t reserved_1_131:4;
        /** k_rtc_ldo : R; bitpos: [13:7]; default: 0;
         *  BLOCK1 K_RTC_LDO
         */
        uint32_t k_rtc_ldo:7;
        /** k_dig_ldo : R; bitpos: [20:14]; default: 0;
         *  BLOCK1 K_DIG_LDO
         */
        uint32_t k_dig_ldo:7;
        /** v_rtc_dbias20 : R; bitpos: [28:21]; default: 0;
         *  BLOCK1 voltage of rtc dbias20
         */
        uint32_t v_rtc_dbias20:8;
        /** v_dig_dbias20 : R; bitpos: [31:29]; default: 0;
         *  BLOCK1 voltage of digital dbias20
         */
        uint32_t v_dig_dbias20:3;
    };
    uint32_t val;
} efuse_rd_mac_spi_sys_4_reg_t;

/** Type of rd_mac_spi_sys_5 register
 *  BLOCK1 data register 5.
 */
typedef union {
    struct {
        /** v_dig_dbias20_1 : R; bitpos: [4:0]; default: 0;
         *  BLOCK1 voltage of digital dbias20
         */
        uint32_t v_dig_dbias20_1:5;
        /** dig_dbias_hvt : R; bitpos: [9:5]; default: 0;
         *  BLOCK1 digital dbias when hvt
         */
        uint32_t dig_dbias_hvt:5;
        /** thres_hvt : R; bitpos: [19:10]; default: 0;
         *  BLOCK1 pvt threshold when hvt
         */
        uint32_t thres_hvt:10;
        /** reserved_1_180 : R; bitpos: [22:20]; default: 0;
         *  reserved
         */
        uint32_t reserved_1_180:3;
        /** wafer_version_minor_hi : R; bitpos: [23]; default: 0;
         *  WAFER_VERSION_MINOR most significant bit
         */
        uint32_t wafer_version_minor_hi:1;
        /** wafer_version_major : R; bitpos: [25:24]; default: 0;
         *  WAFER_VERSION_MAJOR
         */
        uint32_t wafer_version_major:2;
        /** reserved_1_186 : R; bitpos: [31:26]; default: 0;
         *  reserved
         */
        uint32_t reserved_1_186:6;
    };
    uint32_t val;
} efuse_rd_mac_spi_sys_5_reg_t;

/** Type of rd_sys_part1_data0 register
 *  Register 0 of BLOCK2 (system).
 */
typedef union {
    struct {
        /** optional_unique_id : R; bitpos: [31:0]; default: 0;
         *  Optional unique 128-bit ID
         */
        uint32_t optional_unique_id:32;
    };
    uint32_t val;
} efuse_rd_sys_part1_data0_reg_t;

/** Type of rd_sys_part1_data1 register
 *  Register 1 of BLOCK2 (system).
 */
typedef union {
    struct {
        /** optional_unique_id_1 : R; bitpos: [31:0]; default: 0;
         *  Optional unique 128-bit ID
         */
        uint32_t optional_unique_id_1:32;
    };
    uint32_t val;
} efuse_rd_sys_part1_data1_reg_t;

/** Type of rd_sys_part1_data2 register
 *  Register 2 of BLOCK2 (system).
 */
typedef union {
    struct {
        /** optional_unique_id_2 : R; bitpos: [31:0]; default: 0;
         *  Optional unique 128-bit ID
         */
        uint32_t optional_unique_id_2:32;
    };
    uint32_t val;
} efuse_rd_sys_part1_data2_reg_t;

/** Type of rd_sys_part1_data3 register
 *  Register 3 of BLOCK2 (system).
 */
typedef union {
    struct {
        /** optional_unique_id_3 : R; bitpos: [31:0]; default: 0;
         *  Optional unique 128-bit ID
         */
        uint32_t optional_unique_id_3:32;
    };
    uint32_t val;
} efuse_rd_sys_part1_data3_reg_t;

/** Type of rd_sys_part1_data4 register
 *  Register 4 of BLOCK2 (system).
 */
typedef union {
    struct {
        /** blk_version_major : R; bitpos: [1:0]; default: 0;
         *  BLK_VERSION_MAJOR of BLOCK2
         */
        uint32_t blk_version_major:2;
        /** reserved_2_130 : R; bitpos: [2]; default: 0;
         *  reserved
         */
        uint32_t reserved_2_130:1;
        /** temp_calib : R; bitpos: [11:3]; default: 0;
         *  Temperature calibration data
         */
        uint32_t temp_calib:9;
        /** ocode : R; bitpos: [19:12]; default: 0;
         *  ADC OCode
         */
        uint32_t ocode:8;
        /** adc1_init_code_atten0 : R; bitpos: [29:20]; default: 0;
         *  ADC1 init code at atten0
         */
        uint32_t adc1_init_code_atten0:10;
        /** adc1_init_code_atten1 : R; bitpos: [31:30]; default: 0;
         *  ADC1 init code at atten1
         */
        uint32_t adc1_init_code_atten1:2;
    };
    uint32_t val;
} efuse_rd_sys_part1_data4_reg_t;

/** Type of rd_sys_part1_data5 register
 *  Register 5 of BLOCK2 (system).
 */
typedef union {
    struct {
        /** adc1_init_code_atten1_1 : R; bitpos: [7:0]; default: 0;
         *  ADC1 init code at atten1
         */
        uint32_t adc1_init_code_atten1_1:8;
        /** adc1_init_code_atten2 : R; bitpos: [17:8]; default: 0;
         *  ADC1 init code at atten2
         */
        uint32_t adc1_init_code_atten2:10;
        /** adc1_init_code_atten3 : R; bitpos: [27:18]; default: 0;
         *  ADC1 init code at atten3
         */
        uint32_t adc1_init_code_atten3:10;
        /** adc1_cal_vol_atten0 : R; bitpos: [31:28]; default: 0;
         *  ADC1 calibration voltage at atten0
         */
        uint32_t adc1_cal_vol_atten0:4;
    };
    uint32_t val;
} efuse_rd_sys_part1_data5_reg_t;

/** Type of rd_sys_part1_data6 register
 *  Register 6 of BLOCK2 (system).
 */
typedef union {
    struct {
        /** adc1_cal_vol_atten0_1 : R; bitpos: [5:0]; default: 0;
         *  ADC1 calibration voltage at atten0
         */
        uint32_t adc1_cal_vol_atten0_1:6;
        /** adc1_cal_vol_atten1 : R; bitpos: [15:6]; default: 0;
         *  ADC1 calibration voltage at atten1
         */
        uint32_t adc1_cal_vol_atten1:10;
        /** adc1_cal_vol_atten2 : R; bitpos: [25:16]; default: 0;
         *  ADC1 calibration voltage at atten2
         */
        uint32_t adc1_cal_vol_atten2:10;
        /** adc1_cal_vol_atten3 : R; bitpos: [31:26]; default: 0;
         *  ADC1 calibration voltage at atten3
         */
        uint32_t adc1_cal_vol_atten3:6;
    };
    uint32_t val;
} efuse_rd_sys_part1_data6_reg_t;

/** Type of rd_sys_part1_data7 register
 *  Register 7 of BLOCK2 (system).
 */
typedef union {
    struct {
        /** adc1_cal_vol_atten3_1 : R; bitpos: [3:0]; default: 0;
         *  ADC1 calibration voltage at atten3
         */
        uint32_t adc1_cal_vol_atten3_1:4;
        /** reserved_2_228 : R; bitpos: [31:4]; default: 0;
         *  reserved
         */
        uint32_t reserved_2_228:28;
    };
    uint32_t val;
} efuse_rd_sys_part1_data7_reg_t;

/** Type of rd_usr_data0 register
 *  Register 0 of BLOCK3 (user).
 */
typedef union {
    struct {
        /** usr_data0 : RO; bitpos: [31:0]; default: 0;
         *  Stores the zeroth 32 bits of BLOCK3 (user).
         */
        uint32_t usr_data0:32;
    };
    uint32_t val;
} efuse_rd_usr_data0_reg_t;

/** Type of rd_usr_data1 register
 *  Register 1 of BLOCK3 (user).
 */
typedef union {
    struct {
        /** usr_data1 : RO; bitpos: [31:0]; default: 0;
         *  Stores the first 32 bits of BLOCK3 (user).
         */
        uint32_t usr_data1:32;
    };
    uint32_t val;
} efuse_rd_usr_data1_reg_t;

/** Type of rd_usr_data2 register
 *  Register 2 of BLOCK3 (user).
 */
typedef union {
    struct {
        /** usr_data2 : RO; bitpos: [31:0]; default: 0;
         *  Stores the second 32 bits of BLOCK3 (user).
         */
        uint32_t usr_data2:32;
    };
    uint32_t val;
} efuse_rd_usr_data2_reg_t;

/** Type of rd_usr_data3 register
 *  Register 3 of BLOCK3 (user).
 */
typedef union {
    struct {
        /** usr_data3 : RO; bitpos: [31:0]; default: 0;
         *  Stores the third 32 bits of BLOCK3 (user).
         */
        uint32_t usr_data3:32;
    };
    uint32_t val;
} efuse_rd_usr_data3_reg_t;

/** Type of rd_usr_data4 register
 *  Register 4 of BLOCK3 (user).
 */
typedef union {
    struct {
        /** usr_data4 : RO; bitpos: [31:0]; default: 0;
         *  Stores the fourth 32 bits of BLOCK3 (user).
         */
        uint32_t usr_data4:32;
    };
    uint32_t val;
} efuse_rd_usr_data4_reg_t;

/** Type of rd_usr_data5 register
 *  Register 5 of BLOCK3 (user).
 */
typedef union {
    struct {
        /** usr_data5 : RO; bitpos: [31:0]; default: 0;
         *  Stores the fifth 32 bits of BLOCK3 (user).
         */
        uint32_t usr_data5:32;
    };
    uint32_t val;
} efuse_rd_usr_data5_reg_t;

/** Type of rd_usr_data6 register
 *  Register 6 of BLOCK3 (user).
 */
typedef union {
    struct {
        /** reserved_3_192 : R; bitpos: [7:0]; default: 0;
         *  reserved
         */
        uint32_t reserved_3_192:8;
        /** custom_mac : R; bitpos: [31:8]; default: 0;
         *  Custom MAC address
         */
        uint32_t custom_mac:24;
    };
    uint32_t val;
} efuse_rd_usr_data6_reg_t;

/** Type of rd_usr_data7 register
 *  Register 7 of BLOCK3 (user).
 */
typedef union {
    struct {
        /** custom_mac_1 : R; bitpos: [23:0]; default: 0;
         *  Custom MAC address
         */
        uint32_t custom_mac_1:24;
        /** reserved_3_248 : R; bitpos: [31:24]; default: 0;
         *  reserved
         */
        uint32_t reserved_3_248:8;
    };
    uint32_t val;
} efuse_rd_usr_data7_reg_t;

/** Type of rd_key0_data0 register
 *  Register 0 of BLOCK4 (KEY0).
 */
typedef union {
    struct {
        /** key0_data0 : RO; bitpos: [31:0]; default: 0;
         *  Stores the zeroth 32 bits of KEY0.
         */
        uint32_t key0_data0:32;
    };
    uint32_t val;
} efuse_rd_key0_data0_reg_t;

/** Type of rd_key0_data1 register
 *  Register 1 of BLOCK4 (KEY0).
 */
typedef union {
    struct {
        /** key0_data1 : RO; bitpos: [31:0]; default: 0;
         *  Stores the first 32 bits of KEY0.
         */
        uint32_t key0_data1:32;
    };
    uint32_t val;
} efuse_rd_key0_data1_reg_t;

/** Type of rd_key0_data2 register
 *  Register 2 of BLOCK4 (KEY0).
 */
typedef union {
    struct {
        /** key0_data2 : RO; bitpos: [31:0]; default: 0;
         *  Stores the second 32 bits of KEY0.
         */
        uint32_t key0_data2:32;
    };
    uint32_t val;
} efuse_rd_key0_data2_reg_t;

/** Type of rd_key0_data3 register
 *  Register 3 of BLOCK4 (KEY0).
 */
typedef union {
    struct {
        /** key0_data3 : RO; bitpos: [31:0]; default: 0;
         *  Stores the third 32 bits of KEY0.
         */
        uint32_t key0_data3:32;
    };
    uint32_t val;
} efuse_rd_key0_data3_reg_t;

/** Type of rd_key0_data4 register
 *  Register 4 of BLOCK4 (KEY0).
 */
typedef union {
    struct {
        /** key0_data4 : RO; bitpos: [31:0]; default: 0;
         *  Stores the fourth 32 bits of KEY0.
         */
        uint32_t key0_data4:32;
    };
    uint32_t val;
} efuse_rd_key0_data4_reg_t;

/** Type of rd_key0_data5 register
 *  Register 5 of BLOCK4 (KEY0).
 */
typedef union {
    struct {
        /** key0_data5 : RO; bitpos: [31:0]; default: 0;
         *  Stores the fifth 32 bits of KEY0.
         */
        uint32_t key0_data5:32;
    };
    uint32_t val;
} efuse_rd_key0_data5_reg_t;

/** Type of rd_key0_data6 register
 *  Register 6 of BLOCK4 (KEY0).
 */
typedef union {
    struct {
        /** key0_data6 : RO; bitpos: [31:0]; default: 0;
         *  Stores the sixth 32 bits of KEY0.
         */
        uint32_t key0_data6:32;
    };
    uint32_t val;
} efuse_rd_key0_data6_reg_t;

/** Type of rd_key0_data7 register
 *  Register 7 of BLOCK4 (KEY0).
 */
typedef union {
    struct {
        /** key0_data7 : RO; bitpos: [31:0]; default: 0;
         *  Stores the seventh 32 bits of KEY0.
         */
        uint32_t key0_data7:32;
    };
    uint32_t val;
} efuse_rd_key0_data7_reg_t;

/** Type of rd_key1_data0 register
 *  Register 0 of BLOCK5 (KEY1).
 */
typedef union {
    struct {
        /** key1_data0 : RO; bitpos: [31:0]; default: 0;
         *  Stores the zeroth 32 bits of KEY1.
         */
        uint32_t key1_data0:32;
    };
    uint32_t val;
} efuse_rd_key1_data0_reg_t;

/** Type of rd_key1_data1 register
 *  Register 1 of BLOCK5 (KEY1).
 */
typedef union {
    struct {
        /** key1_data1 : RO; bitpos: [31:0]; default: 0;
         *  Stores the first 32 bits of KEY1.
         */
        uint32_t key1_data1:32;
    };
    uint32_t val;
} efuse_rd_key1_data1_reg_t;

/** Type of rd_key1_data2 register
 *  Register 2 of BLOCK5 (KEY1).
 */
typedef union {
    struct {
        /** key1_data2 : RO; bitpos: [31:0]; default: 0;
         *  Stores the second 32 bits of KEY1.
         */
        uint32_t key1_data2:32;
    };
    uint32_t val;
} efuse_rd_key1_data2_reg_t;

/** Type of rd_key1_data3 register
 *  Register 3 of BLOCK5 (KEY1).
 */
typedef union {
    struct {
        /** key1_data3 : RO; bitpos: [31:0]; default: 0;
         *  Stores the third 32 bits of KEY1.
         */
        uint32_t key1_data3:32;
    };
    uint32_t val;
} efuse_rd_key1_data3_reg_t;

/** Type of rd_key1_data4 register
 *  Register 4 of BLOCK5 (KEY1).
 */
typedef union {
    struct {
        /** key1_data4 : RO; bitpos: [31:0]; default: 0;
         *  Stores the fourth 32 bits of KEY1.
         */
        uint32_t key1_data4:32;
    };
    uint32_t val;
} efuse_rd_key1_data4_reg_t;

/** Type of rd_key1_data5 register
 *  Register 5 of BLOCK5 (KEY1).
 */
typedef union {
    struct {
        /** key1_data5 : RO; bitpos: [31:0]; default: 0;
         *  Stores the fifth 32 bits of KEY1.
         */
        uint32_t key1_data5:32;
    };
    uint32_t val;
} efuse_rd_key1_data5_reg_t;

/** Type of rd_key1_data6 register
 *  Register 6 of BLOCK5 (KEY1).
 */
typedef union {
    struct {
        /** key1_data6 : RO; bitpos: [31:0]; default: 0;
         *  Stores the sixth 32 bits of KEY1.
         */
        uint32_t key1_data6:32;
    };
    uint32_t val;
} efuse_rd_key1_data6_reg_t;

/** Type of rd_key1_data7 register
 *  Register 7 of BLOCK5 (KEY1).
 */
typedef union {
    struct {
        /** key1_data7 : RO; bitpos: [31:0]; default: 0;
         *  Stores the seventh 32 bits of KEY1.
         */
        uint32_t key1_data7:32;
    };
    uint32_t val;
} efuse_rd_key1_data7_reg_t;

/** Type of rd_key2_data0 register
 *  Register 0 of BLOCK6 (KEY2).
 */
typedef union {
    struct {
        /** key2_data0 : RO; bitpos: [31:0]; default: 0;
         *  Stores the zeroth 32 bits of KEY2.
         */
        uint32_t key2_data0:32;
    };
    uint32_t val;
} efuse_rd_key2_data0_reg_t;

/** Type of rd_key2_data1 register
 *  Register 1 of BLOCK6 (KEY2).
 */
typedef union {
    struct {
        /** key2_data1 : RO; bitpos: [31:0]; default: 0;
         *  Stores the first 32 bits of KEY2.
         */
        uint32_t key2_data1:32;
    };
    uint32_t val;
} efuse_rd_key2_data1_reg_t;

/** Type of rd_key2_data2 register
 *  Register 2 of BLOCK6 (KEY2).
 */
typedef union {
    struct {
        /** key2_data2 : RO; bitpos: [31:0]; default: 0;
         *  Stores the second 32 bits of KEY2.
         */
        uint32_t key2_data2:32;
    };
    uint32_t val;
} efuse_rd_key2_data2_reg_t;

/** Type of rd_key2_data3 register
 *  Register 3 of BLOCK6 (KEY2).
 */
typedef union {
    struct {
        /** key2_data3 : RO; bitpos: [31:0]; default: 0;
         *  Stores the third 32 bits of KEY2.
         */
        uint32_t key2_data3:32;
    };
    uint32_t val;
} efuse_rd_key2_data3_reg_t;

/** Type of rd_key2_data4 register
 *  Register 4 of BLOCK6 (KEY2).
 */
typedef union {
    struct {
        /** key2_data4 : RO; bitpos: [31:0]; default: 0;
         *  Stores the fourth 32 bits of KEY2.
         */
        uint32_t key2_data4:32;
    };
    uint32_t val;
} efuse_rd_key2_data4_reg_t;

/** Type of rd_key2_data5 register
 *  Register 5 of BLOCK6 (KEY2).
 */
typedef union {
    struct {
        /** key2_data5 : RO; bitpos: [31:0]; default: 0;
         *  Stores the fifth 32 bits of KEY2.
         */
        uint32_t key2_data5:32;
    };
    uint32_t val;
} efuse_rd_key2_data5_reg_t;

/** Type of rd_key2_data6 register
 *  Register 6 of BLOCK6 (KEY2).
 */
typedef union {
    struct {
        /** key2_data6 : RO; bitpos: [31:0]; default: 0;
         *  Stores the sixth 32 bits of KEY2.
         */
        uint32_t key2_data6:32;
    };
    uint32_t val;
} efuse_rd_key2_data6_reg_t;

/** Type of rd_key2_data7 register
 *  Register 7 of BLOCK6 (KEY2).
 */
typedef union {
    struct {
        /** key2_data7 : RO; bitpos: [31:0]; default: 0;
         *  Stores the seventh 32 bits of KEY2.
         */
        uint32_t key2_data7:32;
    };
    uint32_t val;
} efuse_rd_key2_data7_reg_t;

/** Type of rd_key3_data0 register
 *  Register 0 of BLOCK7 (KEY3).
 */
typedef union {
    struct {
        /** key3_data0 : RO; bitpos: [31:0]; default: 0;
         *  Stores the zeroth 32 bits of KEY3.
         */
        uint32_t key3_data0:32;
    };
    uint32_t val;
} efuse_rd_key3_data0_reg_t;

/** Type of rd_key3_data1 register
 *  Register 1 of BLOCK7 (KEY3).
 */
typedef union {
    struct {
        /** key3_data1 : RO; bitpos: [31:0]; default: 0;
         *  Stores the first 32 bits of KEY3.
         */
        uint32_t key3_data1:32;
    };
    uint32_t val;
} efuse_rd_key3_data1_reg_t;

/** Type of rd_key3_data2 register
 *  Register 2 of BLOCK7 (KEY3).
 */
typedef union {
    struct {
        /** key3_data2 : RO; bitpos: [31:0]; default: 0;
         *  Stores the second 32 bits of KEY3.
         */
        uint32_t key3_data2:32;
    };
    uint32_t val;
} efuse_rd_key3_data2_reg_t;

/** Type of rd_key3_data3 register
 *  Register 3 of BLOCK7 (KEY3).
 */
typedef union {
    struct {
        /** key3_data3 : RO; bitpos: [31:0]; default: 0;
         *  Stores the third 32 bits of KEY3.
         */
        uint32_t key3_data3:32;
    };
    uint32_t val;
} efuse_rd_key3_data3_reg_t;

/** Type of rd_key3_data4 register
 *  Register 4 of BLOCK7 (KEY3).
 */
typedef union {
    struct {
        /** key3_data4 : RO; bitpos: [31:0]; default: 0;
         *  Stores the fourth 32 bits of KEY3.
         */
        uint32_t key3_data4:32;
    };
    uint32_t val;
} efuse_rd_key3_data4_reg_t;

/** Type of rd_key3_data5 register
 *  Register 5 of BLOCK7 (KEY3).
 */
typedef union {
    struct {
        /** key3_data5 : RO; bitpos: [31:0]; default: 0;
         *  Stores the fifth 32 bits of KEY3.
         */
        uint32_t key3_data5:32;
    };
    uint32_t val;
} efuse_rd_key3_data5_reg_t;

/** Type of rd_key3_data6 register
 *  Register 6 of BLOCK7 (KEY3).
 */
typedef union {
    struct {
        /** key3_data6 : RO; bitpos: [31:0]; default: 0;
         *  Stores the sixth 32 bits of KEY3.
         */
        uint32_t key3_data6:32;
    };
    uint32_t val;
} efuse_rd_key3_data6_reg_t;

/** Type of rd_key3_data7 register
 *  Register 7 of BLOCK7 (KEY3).
 */
typedef union {
    struct {
        /** key3_data7 : RO; bitpos: [31:0]; default: 0;
         *  Stores the seventh 32 bits of KEY3.
         */
        uint32_t key3_data7:32;
    };
    uint32_t val;
} efuse_rd_key3_data7_reg_t;

/** Type of rd_key4_data0 register
 *  Register 0 of BLOCK8 (KEY4).
 */
typedef union {
    struct {
        /** key4_data0 : RO; bitpos: [31:0]; default: 0;
         *  Stores the zeroth 32 bits of KEY4.
         */
        uint32_t key4_data0:32;
    };
    uint32_t val;
} efuse_rd_key4_data0_reg_t;

/** Type of rd_key4_data1 register
 *  Register 1 of BLOCK8 (KEY4).
 */
typedef union {
    struct {
        /** key4_data1 : RO; bitpos: [31:0]; default: 0;
         *  Stores the first 32 bits of KEY4.
         */
        uint32_t key4_data1:32;
    };
    uint32_t val;
} efuse_rd_key4_data1_reg_t;

/** Type of rd_key4_data2 register
 *  Register 2 of BLOCK8 (KEY4).
 */
typedef union {
    struct {
        /** key4_data2 : RO; bitpos: [31:0]; default: 0;
         *  Stores the second 32 bits of KEY4.
         */
        uint32_t key4_data2:32;
    };
    uint32_t val;
} efuse_rd_key4_data2_reg_t;

/** Type of rd_key4_data3 register
 *  Register 3 of BLOCK8 (KEY4).
 */
typedef union {
    struct {
        /** key4_data3 : RO; bitpos: [31:0]; default: 0;
         *  Stores the third 32 bits of KEY4.
         */
        uint32_t key4_data3:32;
    };
    uint32_t val;
} efuse_rd_key4_data3_reg_t;

/** Type of rd_key4_data4 register
 *  Register 4 of BLOCK8 (KEY4).
 */
typedef union {
    struct {
        /** key4_data4 : RO; bitpos: [31:0]; default: 0;
         *  Stores the fourth 32 bits of KEY4.
         */
        uint32_t key4_data4:32;
    };
    uint32_t val;
} efuse_rd_key4_data4_reg_t;

/** Type of rd_key4_data5 register
 *  Register 5 of BLOCK8 (KEY4).
 */
typedef union {
    struct {
        /** key4_data5 : RO; bitpos: [31:0]; default: 0;
         *  Stores the fifth 32 bits of KEY4.
         */
        uint32_t key4_data5:32;
    };
    uint32_t val;
} efuse_rd_key4_data5_reg_t;

/** Type of rd_key4_data6 register
 *  Register 6 of BLOCK8 (KEY4).
 */
typedef union {
    struct {
        /** key4_data6 : RO; bitpos: [31:0]; default: 0;
         *  Stores the sixth 32 bits of KEY4.
         */
        uint32_t key4_data6:32;
    };
    uint32_t val;
} efuse_rd_key4_data6_reg_t;

/** Type of rd_key4_data7 register
 *  Register 7 of BLOCK8 (KEY4).
 */
typedef union {
    struct {
        /** key4_data7 : RO; bitpos: [31:0]; default: 0;
         *  Stores the seventh 32 bits of KEY4.
         */
        uint32_t key4_data7:32;
    };
    uint32_t val;
} efuse_rd_key4_data7_reg_t;

/** Type of rd_key5_data0 register
 *  Register 0 of BLOCK9 (KEY5).
 */
typedef union {
    struct {
        /** key5_data0 : RO; bitpos: [31:0]; default: 0;
         *  Stores the zeroth 32 bits of KEY5.
         */
        uint32_t key5_data0:32;
    };
    uint32_t val;
} efuse_rd_key5_data0_reg_t;

/** Type of rd_key5_data1 register
 *  Register 1 of BLOCK9 (KEY5).
 */
typedef union {
    struct {
        /** key5_data1 : RO; bitpos: [31:0]; default: 0;
         *  Stores the first 32 bits of KEY5.
         */
        uint32_t key5_data1:32;
    };
    uint32_t val;
} efuse_rd_key5_data1_reg_t;

/** Type of rd_key5_data2 register
 *  Register 2 of BLOCK9 (KEY5).
 */
typedef union {
    struct {
        /** key5_data2 : RO; bitpos: [31:0]; default: 0;
         *  Stores the second 32 bits of KEY5.
         */
        uint32_t key5_data2:32;
    };
    uint32_t val;
} efuse_rd_key5_data2_reg_t;

/** Type of rd_key5_data3 register
 *  Register 3 of BLOCK9 (KEY5).
 */
typedef union {
    struct {
        /** key5_data3 : RO; bitpos: [31:0]; default: 0;
         *  Stores the third 32 bits of KEY5.
         */
        uint32_t key5_data3:32;
    };
    uint32_t val;
} efuse_rd_key5_data3_reg_t;

/** Type of rd_key5_data4 register
 *  Register 4 of BLOCK9 (KEY5).
 */
typedef union {
    struct {
        /** key5_data4 : RO; bitpos: [31:0]; default: 0;
         *  Stores the fourth 32 bits of KEY5.
         */
        uint32_t key5_data4:32;
    };
    uint32_t val;
} efuse_rd_key5_data4_reg_t;

/** Type of rd_key5_data5 register
 *  Register 5 of BLOCK9 (KEY5).
 */
typedef union {
    struct {
        /** key5_data5 : RO; bitpos: [31:0]; default: 0;
         *  Stores the fifth 32 bits of KEY5.
         */
        uint32_t key5_data5:32;
    };
    uint32_t val;
} efuse_rd_key5_data5_reg_t;

/** Type of rd_key5_data6 register
 *  Register 6 of BLOCK9 (KEY5).
 */
typedef union {
    struct {
        /** key5_data6 : RO; bitpos: [31:0]; default: 0;
         *  Stores the sixth 32 bits of KEY5.
         */
        uint32_t key5_data6:32;
    };
    uint32_t val;
} efuse_rd_key5_data6_reg_t;

/** Type of rd_key5_data7 register
 *  Register 7 of BLOCK9 (KEY5).
 */
typedef union {
    struct {
        /** key5_data7 : RO; bitpos: [31:0]; default: 0;
         *  Stores the seventh 32 bits of KEY5.
         */
        uint32_t key5_data7:32;
    };
    uint32_t val;
} efuse_rd_key5_data7_reg_t;

/** Type of rd_sys_part2_data0 register
 *  Register 0 of BLOCK10 (system).
 */
typedef union {
    struct {
        /** sys_data_part2_0 : RO; bitpos: [31:0]; default: 0;
         *  Stores the 0th 32 bits of the 2nd part of system data.
         */
        uint32_t sys_data_part2_0:32;
    };
    uint32_t val;
} efuse_rd_sys_part2_data0_reg_t;

/** Type of rd_sys_part2_data1 register
 *  Register 1 of BLOCK9 (KEY5).
 */
typedef union {
    struct {
        /** sys_data_part2_1 : RO; bitpos: [31:0]; default: 0;
         *  Stores the 1st 32 bits of the 2nd part of system data.
         */
        uint32_t sys_data_part2_1:32;
    };
    uint32_t val;
} efuse_rd_sys_part2_data1_reg_t;

/** Type of rd_sys_part2_data2 register
 *  Register 2 of BLOCK10 (system).
 */
typedef union {
    struct {
        /** sys_data_part2_2 : RO; bitpos: [31:0]; default: 0;
         *  Stores the 2nd 32 bits of the 2nd part of system data.
         */
        uint32_t sys_data_part2_2:32;
    };
    uint32_t val;
} efuse_rd_sys_part2_data2_reg_t;

/** Type of rd_sys_part2_data3 register
 *  Register 3 of BLOCK10 (system).
 */
typedef union {
    struct {
        /** sys_data_part2_3 : RO; bitpos: [31:0]; default: 0;
         *  Stores the 3rd 32 bits of the 2nd part of system data.
         */
        uint32_t sys_data_part2_3:32;
    };
    uint32_t val;
} efuse_rd_sys_part2_data3_reg_t;

/** Type of rd_sys_part2_data4 register
 *  Register 4 of BLOCK10 (system).
 */
typedef union {
    struct {
        /** sys_data_part2_4 : RO; bitpos: [31:0]; default: 0;
         *  Stores the 4th 32 bits of the 2nd part of system data.
         */
        uint32_t sys_data_part2_4:32;
    };
    uint32_t val;
} efuse_rd_sys_part2_data4_reg_t;

/** Type of rd_sys_part2_data5 register
 *  Register 5 of BLOCK10 (system).
 */
typedef union {
    struct {
        /** sys_data_part2_5 : RO; bitpos: [31:0]; default: 0;
         *  Stores the 5th 32 bits of the 2nd part of system data.
         */
        uint32_t sys_data_part2_5:32;
    };
    uint32_t val;
} efuse_rd_sys_part2_data5_reg_t;

/** Type of rd_sys_part2_data6 register
 *  Register 6 of BLOCK10 (system).
 */
typedef union {
    struct {
        /** sys_data_part2_6 : RO; bitpos: [31:0]; default: 0;
         *  Stores the 6th 32 bits of the 2nd part of system data.
         */
        uint32_t sys_data_part2_6:32;
    };
    uint32_t val;
} efuse_rd_sys_part2_data6_reg_t;

/** Type of rd_sys_part2_data7 register
 *  Register 7 of BLOCK10 (system).
 */
typedef union {
    struct {
        /** sys_data_part2_7 : RO; bitpos: [31:0]; default: 0;
         *  Stores the 7th 32 bits of the 2nd part of system data.
         */
        uint32_t sys_data_part2_7:32;
    };
    uint32_t val;
} efuse_rd_sys_part2_data7_reg_t;


/** Group: Report Register */
/** Type of rd_repeat_err0 register
 *  Programming error record register 0 of BLOCK0.
 */
typedef union {
    struct {
        /** rd_dis_err : RO; bitpos: [6:0]; default: 0;
         *  If any bit in RD_DIS is 1, then it indicates a programming error.
         */
        uint32_t rd_dis_err:7;
        /** dis_rtc_ram_boot_err : RO; bitpos: [7]; default: 0;
         *  If DIS_RTC_RAM_BOOT is 1, then it indicates a programming error.
         */
        uint32_t dis_rtc_ram_boot_err:1;
        /** dis_icache_err : RO; bitpos: [8]; default: 0;
         *  If DIS_ICACHE is 1, then it indicates a programming error.
         */
        uint32_t dis_icache_err:1;
        /** dis_usb_jtag_err : RO; bitpos: [9]; default: 0;
         *  If DIS_USB_JTAG is 1, then it indicates a programming error.
         */
        uint32_t dis_usb_jtag_err:1;
        /** dis_download_icache_err : RO; bitpos: [10]; default: 0;
         *  If DIS_DOWNLOAD_ICACHE is 1, then it indicates a programming error.
         */
        uint32_t dis_download_icache_err:1;
        /** dis_usb_device_err : RO; bitpos: [11]; default: 0;
         *  If DIS_USB_DEVICE is 1, then it indicates a programming error.
         */
        uint32_t dis_usb_device_err:1;
        /** dis_force_download_err : RO; bitpos: [12]; default: 0;
         *  If DIS_FORCE_DOWNLOAD is 1, then it indicates a programming error.
         */
        uint32_t dis_force_download_err:1;
        /** rpt4_reserved6_err : RO; bitpos: [13]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved6_err:1;
        /** dis_can_err : RO; bitpos: [14]; default: 0;
         *  If DIS_CAN is 1, then it indicates a programming error.
         */
        uint32_t dis_can_err:1;
        /** jtag_sel_enable_err : RO; bitpos: [15]; default: 0;
         *  If JTAG_SEL_ENABLE is 1, then it indicates a programming error.
         */
        uint32_t jtag_sel_enable_err:1;
        /** soft_dis_jtag_err : RO; bitpos: [18:16]; default: 0;
         *  If SOFT_DIS_JTAG is 1, then it indicates a programming error.
         */
        uint32_t soft_dis_jtag_err:3;
        /** dis_pad_jtag_err : RO; bitpos: [19]; default: 0;
         *  If DIS_PAD_JTAG is 1, then it indicates a programming error.
         */
        uint32_t dis_pad_jtag_err:1;
        /** dis_download_manual_encrypt_err : RO; bitpos: [20]; default: 0;
         *  If DIS_DOWNLOAD_MANUAL_ENCRYPT is 1, then it indicates a programming error.
         */
        uint32_t dis_download_manual_encrypt_err:1;
        /** usb_drefh_err : RO; bitpos: [22:21]; default: 0;
         *  If any bit in USB_DREFH is 1, then it indicates a programming error.
         */
        uint32_t usb_drefh_err:2;
        /** usb_drefl_err : RO; bitpos: [24:23]; default: 0;
         *  If any bit in USB_DREFL is 1, then it indicates a programming error.
         */
        uint32_t usb_drefl_err:2;
        /** usb_exchg_pins_err : RO; bitpos: [25]; default: 0;
         *  If USB_EXCHG_PINS is 1, then it indicates a programming error.
         */
        uint32_t usb_exchg_pins_err:1;
        /** vdd_spi_as_gpio_err : RO; bitpos: [26]; default: 0;
         *  If VDD_SPI_AS_GPIO is 1, then it indicates a programming error.
         */
        uint32_t vdd_spi_as_gpio_err:1;
        /** btlc_gpio_enable_err : RO; bitpos: [28:27]; default: 0;
         *  If any bit in BTLC_GPIO_ENABLE is 1, then it indicates a programming error.
         */
        uint32_t btlc_gpio_enable_err:2;
        /** powerglitch_en_err : RO; bitpos: [29]; default: 0;
         *  If POWERGLITCH_EN is 1, then it indicates a programming error.
         */
        uint32_t powerglitch_en_err:1;
        /** power_glitch_dsense_err : RO; bitpos: [31:30]; default: 0;
         *  If any bit in POWER_GLITCH_DSENSE is 1, then it indicates a programming error.
         */
        uint32_t power_glitch_dsense_err:2;
    };
    uint32_t val;
} efuse_rd_repeat_err0_reg_t;

/** Type of rd_repeat_err1 register
 *  Programming error record register 1 of BLOCK0.
 */
typedef union {
    struct {
        /** rpt4_reserved2_err : RO; bitpos: [15:0]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved2_err:16;
        /** wdt_delay_sel_err : RO; bitpos: [17:16]; default: 0;
         *  If any bit in WDT_DELAY_SEL is 1, then it indicates a programming error.
         */
        uint32_t wdt_delay_sel_err:2;
        /** spi_boot_crypt_cnt_err : RO; bitpos: [20:18]; default: 0;
         *  If any bit in SPI_BOOT_CRYPT_CNT is 1, then it indicates a programming error.
         */
        uint32_t spi_boot_crypt_cnt_err:3;
        /** secure_boot_key_revoke0_err : RO; bitpos: [21]; default: 0;
         *  If SECURE_BOOT_KEY_REVOKE0 is 1, then it indicates a programming error.
         */
        uint32_t secure_boot_key_revoke0_err:1;
        /** secure_boot_key_revoke1_err : RO; bitpos: [22]; default: 0;
         *  If SECURE_BOOT_KEY_REVOKE1 is 1, then it indicates a programming error.
         */
        uint32_t secure_boot_key_revoke1_err:1;
        /** secure_boot_key_revoke2_err : RO; bitpos: [23]; default: 0;
         *  If SECURE_BOOT_KEY_REVOKE2 is 1, then it indicates a programming error.
         */
        uint32_t secure_boot_key_revoke2_err:1;
        /** key_purpose_0_err : RO; bitpos: [27:24]; default: 0;
         *  If any bit in KEY_PURPOSE_0 is 1, then it indicates a programming error.
         */
        uint32_t key_purpose_0_err:4;
        /** key_purpose_1_err : RO; bitpos: [31:28]; default: 0;
         *  If any bit in KEY_PURPOSE_1 is 1, then it indicates a programming error.
         */
        uint32_t key_purpose_1_err:4;
    };
    uint32_t val;
} efuse_rd_repeat_err1_reg_t;

/** Type of rd_repeat_err2 register
 *  Programming error record register 2 of BLOCK0.
 */
typedef union {
    struct {
        /** key_purpose_2_err : RO; bitpos: [3:0]; default: 0;
         *  If any bit in KEY_PURPOSE_2 is 1, then it indicates a programming error.
         */
        uint32_t key_purpose_2_err:4;
        /** key_purpose_3_err : RO; bitpos: [7:4]; default: 0;
         *  If any bit in KEY_PURPOSE_3 is 1, then it indicates a programming error.
         */
        uint32_t key_purpose_3_err:4;
        /** key_purpose_4_err : RO; bitpos: [11:8]; default: 0;
         *  If any bit in KEY_PURPOSE_4 is 1, then it indicates a programming error.
         */
        uint32_t key_purpose_4_err:4;
        /** key_purpose_5_err : RO; bitpos: [15:12]; default: 0;
         *  If any bit in KEY_PURPOSE_5 is 1, then it indicates a programming error.
         */
        uint32_t key_purpose_5_err:4;
        /** rpt4_reserved3_err : RO; bitpos: [19:16]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved3_err:4;
        /** secure_boot_en_err : RO; bitpos: [20]; default: 0;
         *  If SECURE_BOOT_EN is 1, then it indicates a programming error.
         */
        uint32_t secure_boot_en_err:1;
        /** secure_boot_aggressive_revoke_err : RO; bitpos: [21]; default: 0;
         *  If SECURE_BOOT_AGGRESSIVE_REVOKE is 1, then it indicates a programming error.
         */
        uint32_t secure_boot_aggressive_revoke_err:1;
        /** rpt4_reserved0_err : RO; bitpos: [27:22]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved0_err:6;
        /** flash_tpuw_err : RO; bitpos: [31:28]; default: 0;
         *  If any bit in FLASH_TPUM is 1, then it indicates a programming error.
         */
        uint32_t flash_tpuw_err:4;
    };
    uint32_t val;
} efuse_rd_repeat_err2_reg_t;

/** Type of rd_repeat_err3 register
 *  Programming error record register 3 of BLOCK0.
 */
typedef union {
    struct {
        /** dis_download_mode_err : RO; bitpos: [0]; default: 0;
         *  If DIS_DOWNLOAD_MODE is 1, then it indicates a programming error.
         */
        uint32_t dis_download_mode_err:1;
        /** dis_legacy_spi_boot_err : RO; bitpos: [1]; default: 0;
         *  If DIS_LEGACY_SPI_BOOT is 1, then it indicates a programming error.
         */
        uint32_t dis_legacy_spi_boot_err:1;
        /** uart_print_channel_err : RO; bitpos: [2]; default: 0;
         *  If UART_PRINT_CHANNEL is 1, then it indicates a programming error.
         */
        uint32_t uart_print_channel_err:1;
        /** flash_ecc_mode_err : RO; bitpos: [3]; default: 0;
         *  If FLASH_ECC_MODE is 1, then it indicates a programming error.
         */
        uint32_t flash_ecc_mode_err:1;
        /** dis_usb_download_mode_err : RO; bitpos: [4]; default: 0;
         *  If DIS_USB_DOWNLOAD_MODE is 1, then it indicates a programming error.
         */
        uint32_t dis_usb_download_mode_err:1;
        /** enable_security_download_err : RO; bitpos: [5]; default: 0;
         *  If ENABLE_SECURITY_DOWNLOAD is 1, then it indicates a programming error.
         */
        uint32_t enable_security_download_err:1;
        /** uart_print_control_err : RO; bitpos: [7:6]; default: 0;
         *  If any bit in UART_PRINT_CONTROL is 1, then it indicates a programming error.
         */
        uint32_t uart_print_control_err:2;
        /** pin_power_selection_err : RO; bitpos: [8]; default: 0;
         *  If PIN_POWER_SELECTION is 1, then it indicates a programming error.
         */
        uint32_t pin_power_selection_err:1;
        /** flash_type_err : RO; bitpos: [9]; default: 0;
         *  If FLASH_TYPE is 1, then it indicates a programming error.
         */
        uint32_t flash_type_err:1;
        /** flash_page_size_err : RO; bitpos: [11:10]; default: 0;
         *  If any bits in FLASH_PAGE_SIZE is 1, then it indicates a programming error.
         */
        uint32_t flash_page_size_err:2;
        /** flash_ecc_en_err : RO; bitpos: [12]; default: 0;
         *  If FLASH_ECC_EN_ERR is 1, then it indicates a programming error.
         */
        uint32_t flash_ecc_en_err:1;
        /** force_send_resume_err : RO; bitpos: [13]; default: 0;
         *  If FORCE_SEND_RESUME is 1, then it indicates a programming error.
         */
        uint32_t force_send_resume_err:1;
        /** secure_version_err : RO; bitpos: [29:14]; default: 0;
         *  If any bit in SECURE_VERSION is 1, then it indicates a programming error.
         */
        uint32_t secure_version_err:16;
        /** rpt4_reserved1_err : RO; bitpos: [31:30]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved1_err:2;
    };
    uint32_t val;
} efuse_rd_repeat_err3_reg_t;

/** Type of rd_repeat_err4 register
 *  Programming error record register 4 of BLOCK0.
 */
typedef union {
    struct {
        /** rpt4_reserved4_err : RO; bitpos: [23:0]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved4_err:24;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} efuse_rd_repeat_err4_reg_t;

/** Type of rd_rs_err0 register
 *  Programming error record register 0 of BLOCK1-10.
 */
typedef union {
    struct {
        /** mac_spi_8m_err_num : RO; bitpos: [2:0]; default: 0;
         *  The value of this signal means the number of error bytes.
         */
        uint32_t mac_spi_8m_err_num:3;
        /** reserved_fail : RO; bitpos: [3]; default: 0;
         *  0: Means no failure and that the data of MAC_SPI_8M is reliable 1: Means that
         *  programming user data failed and the number of error bytes is over 6.
         */
        uint32_t reserved_fail:1;
        /** sys_part1_num : RO; bitpos: [6:4]; default: 0;
         *  The value of this signal means the number of error bytes.
         */
        uint32_t sys_part1_num:3;
        /** mac_spi_8m_fail : RO; bitpos: [7]; default: 0;
         *  0: Means no failure and that the data of system part1 is reliable 1: Means that
         *  programming user data failed and the number of error bytes is over 6.
         */
        uint32_t mac_spi_8m_fail:1;
        /** usr_data_err_num : RO; bitpos: [10:8]; default: 0;
         *  The value of this signal means the number of error bytes.
         */
        uint32_t usr_data_err_num:3;
        /** sys_part1_fail : RO; bitpos: [11]; default: 0;
         *  0: Means no failure and that the user data is reliable 1: Means that programming
         *  user data failed and the number of error bytes is over 6.
         */
        uint32_t sys_part1_fail:1;
        /** key0_err_num : RO; bitpos: [14:12]; default: 0;
         *  The value of this signal means the number of error bytes.
         */
        uint32_t key0_err_num:3;
        /** usr_data_fail : RO; bitpos: [15]; default: 0;
         *  0: Means no failure and that the data of key0 is reliable 1: Means that programming
         *  key0 failed and the number of error bytes is over 6.
         */
        uint32_t usr_data_fail:1;
        /** key1_err_num : RO; bitpos: [18:16]; default: 0;
         *  The value of this signal means the number of error bytes.
         */
        uint32_t key1_err_num:3;
        /** key0_fail : RO; bitpos: [19]; default: 0;
         *  0: Means no failure and that the data of key1 is reliable 1: Means that programming
         *  key1 failed and the number of error bytes is over 6.
         */
        uint32_t key0_fail:1;
        /** key2_err_num : RO; bitpos: [22:20]; default: 0;
         *  The value of this signal means the number of error bytes.
         */
        uint32_t key2_err_num:3;
        /** key1_fail : RO; bitpos: [23]; default: 0;
         *  0: Means no failure and that the data of key2 is reliable 1: Means that programming
         *  key2 failed and the number of error bytes is over 6.
         */
        uint32_t key1_fail:1;
        /** key3_err_num : RO; bitpos: [26:24]; default: 0;
         *  The value of this signal means the number of error bytes.
         */
        uint32_t key3_err_num:3;
        /** key2_fail : RO; bitpos: [27]; default: 0;
         *  0: Means no failure and that the data of key3 is reliable 1: Means that programming
         *  key3 failed and the number of error bytes is over 6.
         */
        uint32_t key2_fail:1;
        /** key4_err_num : RO; bitpos: [30:28]; default: 0;
         *  The value of this signal means the number of error bytes.
         */
        uint32_t key4_err_num:3;
        /** key3_fail : RO; bitpos: [31]; default: 0;
         *  0: Means no failure and that the data of key4 is reliable 1: Means that programming
         *  key4 failed and the number of error bytes is over 6.
         */
        uint32_t key3_fail:1;
    };
    uint32_t val;
} efuse_rd_rs_err0_reg_t;

/** Type of rd_rs_err1 register
 *  Programming error record register 1 of BLOCK1-10.
 */
typedef union {
    struct {
        /** key5_err_num : RO; bitpos: [2:0]; default: 0;
         *  The value of this signal means the number of error bytes.
         */
        uint32_t key5_err_num:3;
        /** key4_fail : RO; bitpos: [3]; default: 0;
         *  0: Means no failure and that the data of KEY5 is reliable 1: Means that programming
         *  user data failed and the number of error bytes is over 6.
         */
        uint32_t key4_fail:1;
        /** sys_part2_err_num : RO; bitpos: [6:4]; default: 0;
         *  The value of this signal means the number of error bytes.
         */
        uint32_t sys_part2_err_num:3;
        /** key5_fail : RO; bitpos: [7]; default: 0;
         *  0: Means no failure and that the data of system part2 is reliable 1: Means that
         *  programming user data failed and the number of error bytes is over 6.
         */
        uint32_t key5_fail:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} efuse_rd_rs_err1_reg_t;


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
 *  eFuse operation mode configuraiton register;
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
        /** read_cmd : R/WS/SC; bitpos: [0]; default: 0;
         *  Set this bit to send read command.
         */
        uint32_t read_cmd:1;
        /** pgm_cmd : R/WS/SC; bitpos: [1]; default: 0;
         *  Set this bit to send programming command.
         */
        uint32_t pgm_cmd:1;
        /** blk_num : R/W; bitpos: [5:2]; default: 0;
         *  The serial number of the block to be programmed. Value 0-10 corresponds to block
         *  number 0-10, respectively.
         */
        uint32_t blk_num:4;
        uint32_t reserved_6:26;
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
        uint32_t reserved_0:24;
        /** read_init_num : R/W; bitpos: [31:24]; default: 18;
         *  Configures the initial read time of eFuse.
         */
        uint32_t read_init_num:8;
    };
    uint32_t val;
} efuse_rd_tim_conf_reg_t;

/** Type of wr_tim_conf1 register
 *  Configurarion register 1 of eFuse programming timing parameters.
 */
typedef union {
    struct {
        uint32_t reserved_0:8;
        /** pwr_on_num : R/W; bitpos: [23:8]; default: 10368;
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
        /** repeat_err_cnt : RO; bitpos: [17:10]; default: 0;
         *  Indicates the number of error bits during programming BLOCK0.
         */
        uint32_t repeat_err_cnt:8;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} efuse_status_reg_t;


/** Group: Interrupt Register */
/** Type of int_raw register
 *  eFuse raw interrupt register.
 */
typedef union {
    struct {
        /** read_done_int_raw : R/WC/SS; bitpos: [0]; default: 0;
         *  The raw bit signal for read_done interrupt.
         */
        uint32_t read_done_int_raw:1;
        /** pgm_done_int_raw : R/WC/SS; bitpos: [1]; default: 0;
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
        /** read_done_int_clr : WO; bitpos: [0]; default: 0;
         *  The clear signal for read_done interrupt.
         */
        uint32_t read_done_int_clr:1;
        /** pgm_done_int_clr : WO; bitpos: [1]; default: 0;
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
        /** date : R/W; bitpos: [27:0]; default: 33583616;
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
    volatile efuse_rd_repeat_data1_reg_t rd_repeat_data1;
    volatile efuse_rd_repeat_data2_reg_t rd_repeat_data2;
    volatile efuse_rd_repeat_data3_reg_t rd_repeat_data3;
    volatile efuse_rd_repeat_data4_reg_t rd_repeat_data4;
    volatile efuse_rd_mac_spi_sys_0_reg_t rd_mac_spi_sys_0;
    volatile efuse_rd_mac_spi_sys_1_reg_t rd_mac_spi_sys_1;
    volatile efuse_rd_mac_spi_sys_2_reg_t rd_mac_spi_sys_2;
    volatile efuse_rd_mac_spi_sys_3_reg_t rd_mac_spi_sys_3;
    volatile efuse_rd_mac_spi_sys_4_reg_t rd_mac_spi_sys_4;
    volatile efuse_rd_mac_spi_sys_5_reg_t rd_mac_spi_sys_5;
    volatile efuse_rd_sys_part1_data0_reg_t rd_sys_part1_data0;
    volatile efuse_rd_sys_part1_data1_reg_t rd_sys_part1_data1;
    volatile efuse_rd_sys_part1_data2_reg_t rd_sys_part1_data2;
    volatile efuse_rd_sys_part1_data3_reg_t rd_sys_part1_data3;
    volatile efuse_rd_sys_part1_data4_reg_t rd_sys_part1_data4;
    volatile efuse_rd_sys_part1_data5_reg_t rd_sys_part1_data5;
    volatile efuse_rd_sys_part1_data6_reg_t rd_sys_part1_data6;
    volatile efuse_rd_sys_part1_data7_reg_t rd_sys_part1_data7;
    volatile efuse_rd_usr_data0_reg_t rd_usr_data0;
    volatile efuse_rd_usr_data1_reg_t rd_usr_data1;
    volatile efuse_rd_usr_data2_reg_t rd_usr_data2;
    volatile efuse_rd_usr_data3_reg_t rd_usr_data3;
    volatile efuse_rd_usr_data4_reg_t rd_usr_data4;
    volatile efuse_rd_usr_data5_reg_t rd_usr_data5;
    volatile efuse_rd_usr_data6_reg_t rd_usr_data6;
    volatile efuse_rd_usr_data7_reg_t rd_usr_data7;
    volatile efuse_rd_key0_data0_reg_t rd_key0_data0;
    volatile efuse_rd_key0_data1_reg_t rd_key0_data1;
    volatile efuse_rd_key0_data2_reg_t rd_key0_data2;
    volatile efuse_rd_key0_data3_reg_t rd_key0_data3;
    volatile efuse_rd_key0_data4_reg_t rd_key0_data4;
    volatile efuse_rd_key0_data5_reg_t rd_key0_data5;
    volatile efuse_rd_key0_data6_reg_t rd_key0_data6;
    volatile efuse_rd_key0_data7_reg_t rd_key0_data7;
    volatile efuse_rd_key1_data0_reg_t rd_key1_data0;
    volatile efuse_rd_key1_data1_reg_t rd_key1_data1;
    volatile efuse_rd_key1_data2_reg_t rd_key1_data2;
    volatile efuse_rd_key1_data3_reg_t rd_key1_data3;
    volatile efuse_rd_key1_data4_reg_t rd_key1_data4;
    volatile efuse_rd_key1_data5_reg_t rd_key1_data5;
    volatile efuse_rd_key1_data6_reg_t rd_key1_data6;
    volatile efuse_rd_key1_data7_reg_t rd_key1_data7;
    volatile efuse_rd_key2_data0_reg_t rd_key2_data0;
    volatile efuse_rd_key2_data1_reg_t rd_key2_data1;
    volatile efuse_rd_key2_data2_reg_t rd_key2_data2;
    volatile efuse_rd_key2_data3_reg_t rd_key2_data3;
    volatile efuse_rd_key2_data4_reg_t rd_key2_data4;
    volatile efuse_rd_key2_data5_reg_t rd_key2_data5;
    volatile efuse_rd_key2_data6_reg_t rd_key2_data6;
    volatile efuse_rd_key2_data7_reg_t rd_key2_data7;
    volatile efuse_rd_key3_data0_reg_t rd_key3_data0;
    volatile efuse_rd_key3_data1_reg_t rd_key3_data1;
    volatile efuse_rd_key3_data2_reg_t rd_key3_data2;
    volatile efuse_rd_key3_data3_reg_t rd_key3_data3;
    volatile efuse_rd_key3_data4_reg_t rd_key3_data4;
    volatile efuse_rd_key3_data5_reg_t rd_key3_data5;
    volatile efuse_rd_key3_data6_reg_t rd_key3_data6;
    volatile efuse_rd_key3_data7_reg_t rd_key3_data7;
    volatile efuse_rd_key4_data0_reg_t rd_key4_data0;
    volatile efuse_rd_key4_data1_reg_t rd_key4_data1;
    volatile efuse_rd_key4_data2_reg_t rd_key4_data2;
    volatile efuse_rd_key4_data3_reg_t rd_key4_data3;
    volatile efuse_rd_key4_data4_reg_t rd_key4_data4;
    volatile efuse_rd_key4_data5_reg_t rd_key4_data5;
    volatile efuse_rd_key4_data6_reg_t rd_key4_data6;
    volatile efuse_rd_key4_data7_reg_t rd_key4_data7;
    volatile efuse_rd_key5_data0_reg_t rd_key5_data0;
    volatile efuse_rd_key5_data1_reg_t rd_key5_data1;
    volatile efuse_rd_key5_data2_reg_t rd_key5_data2;
    volatile efuse_rd_key5_data3_reg_t rd_key5_data3;
    volatile efuse_rd_key5_data4_reg_t rd_key5_data4;
    volatile efuse_rd_key5_data5_reg_t rd_key5_data5;
    volatile efuse_rd_key5_data6_reg_t rd_key5_data6;
    volatile efuse_rd_key5_data7_reg_t rd_key5_data7;
    volatile efuse_rd_sys_part2_data0_reg_t rd_sys_part2_data0;
    volatile efuse_rd_sys_part2_data1_reg_t rd_sys_part2_data1;
    volatile efuse_rd_sys_part2_data2_reg_t rd_sys_part2_data2;
    volatile efuse_rd_sys_part2_data3_reg_t rd_sys_part2_data3;
    volatile efuse_rd_sys_part2_data4_reg_t rd_sys_part2_data4;
    volatile efuse_rd_sys_part2_data5_reg_t rd_sys_part2_data5;
    volatile efuse_rd_sys_part2_data6_reg_t rd_sys_part2_data6;
    volatile efuse_rd_sys_part2_data7_reg_t rd_sys_part2_data7;
    volatile efuse_rd_repeat_err0_reg_t rd_repeat_err0;
    volatile efuse_rd_repeat_err1_reg_t rd_repeat_err1;
    volatile efuse_rd_repeat_err2_reg_t rd_repeat_err2;
    volatile efuse_rd_repeat_err3_reg_t rd_repeat_err3;
    uint32_t reserved_18c;
    volatile efuse_rd_repeat_err4_reg_t rd_repeat_err4;
    uint32_t reserved_194[11];
    volatile efuse_rd_rs_err0_reg_t rd_rs_err0;
    volatile efuse_rd_rs_err1_reg_t rd_rs_err1;
    volatile efuse_clk_reg_t clk;
    volatile efuse_conf_reg_t conf;
    volatile efuse_status_reg_t status;
    volatile efuse_cmd_reg_t cmd;
    volatile efuse_int_raw_reg_t int_raw;
    volatile efuse_int_st_reg_t int_st;
    volatile efuse_int_ena_reg_t int_ena;
    volatile efuse_int_clr_reg_t int_clr;
    volatile efuse_dac_conf_reg_t dac_conf;
    volatile efuse_rd_tim_conf_reg_t rd_tim_conf;
    volatile efuse_wr_tim_conf1_reg_t wr_tim_conf1;
    volatile efuse_wr_tim_conf2_reg_t wr_tim_conf2;
    uint32_t reserved_1f8;
    volatile efuse_date_reg_t date;
} efuse_dev_t;
extern efuse_dev_t EFUSE;

#ifndef __cplusplus
_Static_assert(sizeof(efuse_dev_t) == 0x200, "Invalid size of efuse_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
