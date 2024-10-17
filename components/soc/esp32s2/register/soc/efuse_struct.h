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

/** Group: PGM Data Registers */
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
        /** pgm_data_0 : R/W; bitpos: [31:0]; default: 0;
         *  The content of the 1th 32-bit data to be programmed.
         */
        uint32_t pgm_data_0:32;
    };
    uint32_t val;
} efuse_pgm_data1_reg_t;

/** Type of pgm_data2 register
 *  Register 2 that stores data to be programmed.
 */
typedef union {
    struct {
        /** pgm_data_0 : R/W; bitpos: [31:0]; default: 0;
         *  The content of the 2th 32-bit data to be programmed.
         */
        uint32_t pgm_data_0:32;
    };
    uint32_t val;
} efuse_pgm_data2_reg_t;

/** Type of pgm_data3 register
 *  Register 3 that stores data to be programmed.
 */
typedef union {
    struct {
        /** pgm_data_0 : R/W; bitpos: [31:0]; default: 0;
         *  The content of the 3th 32-bit data to be programmed.
         */
        uint32_t pgm_data_0:32;
    };
    uint32_t val;
} efuse_pgm_data3_reg_t;

/** Type of pgm_data4 register
 *  Register 4 that stores data to be programmed.
 */
typedef union {
    struct {
        /** pgm_data_0 : R/W; bitpos: [31:0]; default: 0;
         *  The content of the 4th 32-bit data to be programmed.
         */
        uint32_t pgm_data_0:32;
    };
    uint32_t val;
} efuse_pgm_data4_reg_t;

/** Type of pgm_data5 register
 *  Register 5 that stores data to be programmed.
 */
typedef union {
    struct {
        /** pgm_data_0 : R/W; bitpos: [31:0]; default: 0;
         *  The content of the 5th 32-bit data to be programmed.
         */
        uint32_t pgm_data_0:32;
    };
    uint32_t val;
} efuse_pgm_data5_reg_t;

/** Type of pgm_data6 register
 *  Register 6 that stores data to be programmed.
 */
typedef union {
    struct {
        /** pgm_data_0 : R/W; bitpos: [31:0]; default: 0;
         *  The content of the 6th 32-bit data to be programmed.
         */
        uint32_t pgm_data_0:32;
    };
    uint32_t val;
} efuse_pgm_data6_reg_t;

/** Type of pgm_data7 register
 *  Register 7 that stores data to be programmed.
 */
typedef union {
    struct {
        /** pgm_data_0 : R/W; bitpos: [31:0]; default: 0;
         *  The content of the 7th 32-bit data to be programmed.
         */
        uint32_t pgm_data_0:32;
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
        /** pgm_rs_data_0 : R/W; bitpos: [31:0]; default: 0;
         *  The content of the 1th 32-bit RS code to be programmed.
         */
        uint32_t pgm_rs_data_0:32;
    };
    uint32_t val;
} efuse_pgm_check_value1_reg_t;

/** Type of pgm_check_value2 register
 *  Register 2 that stores the RS code to be programmed.
 */
typedef union {
    struct {
        /** pgm_rs_data_0 : R/W; bitpos: [31:0]; default: 0;
         *  The content of the 2th 32-bit RS code to be programmed.
         */
        uint32_t pgm_rs_data_0:32;
    };
    uint32_t val;
} efuse_pgm_check_value2_reg_t;


/** Group: Read Data Registers */
/** Type of rd_wr_dis register
 *  Register 0 of BLOCK0.
 */
typedef union {
    struct {
        /** wr_dis : RO; bitpos: [31:0]; default: 0;
         *  Disables programming of individual eFuses.
         */
        uint32_t wr_dis:32;
    };
    uint32_t val;
} efuse_rd_wr_dis_reg_t;

/** Type of rd_repeat_data0 register
 *  Register 1 of BLOCK0.
 */
typedef union {
    struct {
        /** rd_dis : RO; bitpos: [6:0]; default: 0;
         *  Disables software reading from individual eFuse blocks (BLOCK4-10).
         */
        uint32_t rd_dis:7;
        /** dis_rtc_ram_boot : RO; bitpos: [7]; default: 0;
         *  Reserved.
         */
        uint32_t dis_rtc_ram_boot:1;
        /** dis_icache : RO; bitpos: [8]; default: 0;
         *  Set this bit to disable Icache.
         */
        uint32_t dis_icache:1;
        /** dis_dcache : RO; bitpos: [9]; default: 0;
         *  Set this bit to disable Dcache.
         */
        uint32_t dis_dcache:1;
        /** dis_download_icache : RO; bitpos: [10]; default: 0;
         *  Disables Icache when SoC is in Download mode.
         */
        uint32_t dis_download_icache:1;
        /** dis_download_dcache : RO; bitpos: [11]; default: 0;
         *  Disables Dcache when SoC is in Download mode.
         */
        uint32_t dis_download_dcache:1;
        /** dis_force_download : RO; bitpos: [12]; default: 0;
         *  Set this bit to disable the function that forces chip into download mode.
         */
        uint32_t dis_force_download:1;
        /** dis_usb : RO; bitpos: [13]; default: 0;
         *  Set this bit to disable USB OTG function.
         */
        uint32_t dis_usb:1;
        /** dis_twai : RO; bitpos: [14]; default: 0;
         *  Set this bit to disable the TWAI Controller function.
         */
        uint32_t dis_twai:1;
        /** dis_boot_remap : RO; bitpos: [15]; default: 0;
         *  Disables capability to Remap RAM to ROM address space.
         */
        uint32_t dis_boot_remap:1;
        /** rpt4_reserved5 : RO; bitpos: [16]; default: 0;
         *  Reserved (used for four backups method).
         */
        uint32_t rpt4_reserved5:1;
        /** soft_dis_jtag : RO; bitpos: [17]; default: 0;
         *  Software disables JTAG. When software disabled, JTAG can be activated temporarily
         *  by HMAC peripheral.
         */
        uint32_t soft_dis_jtag:1;
        /** hard_dis_jtag : RO; bitpos: [18]; default: 0;
         *  Hardware disables JTAG permanently.
         */
        uint32_t hard_dis_jtag:1;
        /** dis_download_manual_encrypt : RO; bitpos: [19]; default: 0;
         *  Disables flash encryption when in download boot modes.
         */
        uint32_t dis_download_manual_encrypt:1;
        /** usb_drefh : RO; bitpos: [21:20]; default: 0;
         *  Controls single-end input threshold vrefh, 1.76 V to 2 V with step of 80 mV, stored
         *  in eFuse.
         */
        uint32_t usb_drefh:2;
        /** usb_drefl : RO; bitpos: [23:22]; default: 0;
         *  Controls single-end input threshold vrefl, 0.8 V to 1.04 V with step of 80 mV,
         *  stored in eFuse.
         */
        uint32_t usb_drefl:2;
        /** usb_exchg_pins : RO; bitpos: [24]; default: 0;
         *  Set this bit to exchange USB D+ and D- pins.
         */
        uint32_t usb_exchg_pins:1;
        /** usb_ext_phy_enable : RO; bitpos: [25]; default: 0;
         *  Set this bit to enable external USB PHY.
         */
        uint32_t usb_ext_phy_enable:1;
        /** usb_force_nopersist : RO; bitpos: [26]; default: 0;
         *  If set, forces USB BVALID to 1.
         */
        uint32_t usb_force_nopersist:1;
        /** block0_version : R; bitpos: [28:27]; default: 0;
         *  BLOCK0 efuse version
         */
        uint32_t block0_version:2;
        /** vdd_spi_modecurlim : RO; bitpos: [29]; default: 0;
         *  SPI regulator switches current limit mode.
         */
        uint32_t vdd_spi_modecurlim:1;
        /** vdd_spi_drefh : RO; bitpos: [31:30]; default: 0;
         *  SPI regulator high voltage reference.
         */
        uint32_t vdd_spi_drefh:2;
    };
    uint32_t val;
} efuse_rd_repeat_data0_reg_t;

/** Type of rd_repeat_data1 register
 *  Register 2 of BLOCK0.
 */
typedef union {
    struct {
        /** vdd_spi_drefm : RO; bitpos: [1:0]; default: 0;
         *  SPI regulator medium voltage reference.
         */
        uint32_t vdd_spi_drefm:2;
        /** vdd_spi_drefl : RO; bitpos: [3:2]; default: 0;
         *  SPI regulator low voltage reference.
         */
        uint32_t vdd_spi_drefl:2;
        /** vdd_spi_xpd : RO; bitpos: [4]; default: 0;
         *  If VDD_SPI_FORCE is 1, this value determines if the VDD_SPI regulator is powered on.
         */
        uint32_t vdd_spi_xpd:1;
        /** vdd_spi_tieh : RO; bitpos: [5]; default: 0;
         *  If VDD_SPI_FORCE is 1, determines VDD_SPI voltage. 0: VDD_SPI connects to 1.8 V
         *  LDO. 1: VDD_SPI connects to VDD_RTC_IO.
         */
        uint32_t vdd_spi_tieh:1;
        /** vdd_spi_force : RO; bitpos: [6]; default: 0;
         *  Set this bit to use XPD_VDD_PSI_REG and VDD_SPI_TIEH to configure VDD_SPI LDO.
         */
        uint32_t vdd_spi_force:1;
        /** vdd_spi_en_init : RO; bitpos: [7]; default: 0;
         *  Set SPI regulator to 0 to configure init[1:0]=0.
         */
        uint32_t vdd_spi_en_init:1;
        /** vdd_spi_encurlim : RO; bitpos: [8]; default: 0;
         *  Set SPI regulator to 1 to enable output current limit.
         */
        uint32_t vdd_spi_encurlim:1;
        /** vdd_spi_dcurlim : RO; bitpos: [11:9]; default: 0;
         *  Tunes the current limit threshold of SPI regulator when tieh=0, about 800 mA/(8+d).
         */
        uint32_t vdd_spi_dcurlim:3;
        /** vdd_spi_init : RO; bitpos: [13:12]; default: 0;
         *  Adds resistor from LDO output to ground. 0: no resistance. 1: 6 K. 2: 4 K. 3: 2 K.
         */
        uint32_t vdd_spi_init:2;
        /** vdd_spi_dcap : RO; bitpos: [15:14]; default: 0;
         *  Prevents SPI regulator from overshoot.
         */
        uint32_t vdd_spi_dcap:2;
        /** wdt_delay_sel : RO; bitpos: [17:16]; default: 0;
         *  Selects RTC watchdog timeout threshold at startup. 0: 40,000 slow clock cycles. 1:
         *  80,000 slow clock cycles. 2: 160,000 slow clock cycles. 3: 320,000 slow clock
         *  cycles.
         */
        uint32_t wdt_delay_sel:2;
        /** spi_boot_crypt_cnt : RO; bitpos: [20:18]; default: 0;
         *  Enables encryption and decryption, when an SPI boot mode is set. Feature is enabled
         *  1 or 3 bits are set in the eFuse, disabled otherwise.
         */
        uint32_t spi_boot_crypt_cnt:3;
        /** secure_boot_key_revoke0 : RO; bitpos: [21]; default: 0;
         *  If set, revokes use of secure boot key digest 0.
         */
        uint32_t secure_boot_key_revoke0:1;
        /** secure_boot_key_revoke1 : RO; bitpos: [22]; default: 0;
         *  If set, revokes use of secure boot key digest 1.
         */
        uint32_t secure_boot_key_revoke1:1;
        /** secure_boot_key_revoke2 : RO; bitpos: [23]; default: 0;
         *  If set, revokes use of secure boot key digest 2.
         */
        uint32_t secure_boot_key_revoke2:1;
        /** key_purpose_0 : RO; bitpos: [27:24]; default: 0;
         *  Purpose of KEY0. Refer to Table Key Purpose Values.
         */
        uint32_t key_purpose_0:4;
        /** key_purpose_1 : RO; bitpos: [31:28]; default: 0;
         *  Purpose of KEY1. Refer to Table Key Purpose Values.
         */
        uint32_t key_purpose_1:4;
    };
    uint32_t val;
} efuse_rd_repeat_data1_reg_t;

/** Type of rd_repeat_data2 register
 *  Register 3 of BLOCK0.
 */
typedef union {
    struct {
        /** key_purpose_2 : RO; bitpos: [3:0]; default: 0;
         *  Purpose of KEY2. Refer to Table Key Purpose Values.
         */
        uint32_t key_purpose_2:4;
        /** key_purpose_3 : RO; bitpos: [7:4]; default: 0;
         *  Purpose of KEY3. Refer to Table Key Purpose Values.
         */
        uint32_t key_purpose_3:4;
        /** key_purpose_4 : RO; bitpos: [11:8]; default: 0;
         *  Purpose of KEY4. Refer to Table Key Purpose Values.
         */
        uint32_t key_purpose_4:4;
        /** key_purpose_5 : RO; bitpos: [15:12]; default: 0;
         *  Purpose of KEY5. Refer to Table Key Purpose Values.
         */
        uint32_t key_purpose_5:4;
        /** key_purpose_6 : RO; bitpos: [19:16]; default: 0;
         *  Purpose of KEY6. Refer to Table Key Purpose Values.
         */
        uint32_t key_purpose_6:4;
        /** secure_boot_en : RO; bitpos: [20]; default: 0;
         *  Set this bit to enable secure boot.
         */
        uint32_t secure_boot_en:1;
        /** secure_boot_aggressive_revoke : RO; bitpos: [21]; default: 0;
         *  Set this bit to enable aggressive secure boot key revocation mode.
         */
        uint32_t secure_boot_aggressive_revoke:1;
        /** rpt4_reserved1 : RO; bitpos: [27:22]; default: 0;
         *  Reserved (used for four backups method).
         */
        uint32_t rpt4_reserved1:6;
        /** flash_tpuw : RO; bitpos: [31:28]; default: 0;
         *  Configures flash startup delay after SoC power-up, in unit of (ms/2). When the
         *  value is 15, delay is 7.5 ms.
         */
        uint32_t flash_tpuw:4;
    };
    uint32_t val;
} efuse_rd_repeat_data2_reg_t;

/** Type of rd_repeat_data3 register
 *  Register 4 of BLOCK0.
 */
typedef union {
    struct {
        /** dis_download_mode : RO; bitpos: [0]; default: 0;
         *  Set this bit to disable all download boot modes.
         */
        uint32_t dis_download_mode:1;
        /** dis_legacy_spi_boot : RO; bitpos: [1]; default: 0;
         *  Set this bit to disable Legacy SPI boot mode.
         */
        uint32_t dis_legacy_spi_boot:1;
        /** uart_print_channel : RO; bitpos: [2]; default: 0;
         *  Selects the default UART for printing boot messages. 0: UART0. 1: UART1.
         */
        uint32_t uart_print_channel:1;
        /** rpt4_reserved3 : RO; bitpos: [3]; default: 0;
         *  Reserved (used for four backups method).
         */
        uint32_t rpt4_reserved3:1;
        /** dis_usb_download_mode : RO; bitpos: [4]; default: 0;
         *  Set this bit to disable use of USB OTG in UART download boot mode.
         */
        uint32_t dis_usb_download_mode:1;
        /** enable_security_download : RO; bitpos: [5]; default: 0;
         *  Set this bit to enable secure UART download mode (read/write flash only).
         */
        uint32_t enable_security_download:1;
        /** uart_print_control : RO; bitpos: [7:6]; default: 0;
         *  Set the default UART boot message output mode. 00: Enabled. 01: Enable when GPIO46
         *  is low at reset. 10: Enable when GPIO46 is high at reset. 11: Disabled.
         */
        uint32_t uart_print_control:2;
        /** pin_power_selection : RO; bitpos: [8]; default: 0;
         *  Set default power supply for GPIO33-GPIO37, set when SPI flash is initialized. 0:
         *  VDD3P3_CPU. 1: VDD_SPI.
         */
        uint32_t pin_power_selection:1;
        /** flash_type : RO; bitpos: [9]; default: 0;
         *  SPI flash type. 0: maximum four data lines, 1: eight data lines.
         */
        uint32_t flash_type:1;
        /** force_send_resume : RO; bitpos: [10]; default: 0;
         *  If set, forces ROM code to send an SPI flash resume command during SPI boot.
         */
        uint32_t force_send_resume:1;
        /** secure_version : RO; bitpos: [26:11]; default: 0;
         *  Secure version (used by ESP-IDF anti-rollback feature).
         */
        uint32_t secure_version:16;
        /** rpt4_reserved2 : RO; bitpos: [31:27]; default: 0;
         *  Reserved (used for four backups method).
         */
        uint32_t rpt4_reserved2:5;
    };
    uint32_t val;
} efuse_rd_repeat_data3_reg_t;

/** Type of rd_repeat_data4 register
 *  Register 5 of BLOCK0.
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
 *  Register 0 of BLOCK1.
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
 *  Register 1 of BLOCK1.
 */
typedef union {
    struct {
        /** mac_1 : RO; bitpos: [15:0]; default: 0;
         *  Stores the high 16 bits of MAC address.
         */
        uint32_t mac_1:16;
        /** spi_pad_config_clk : R; bitpos: [21:16]; default: 0;
         *  SPI_PAD_configure CLK
         */
        uint32_t spi_pad_config_clk:6;
        /** spi_pad_config_q : R; bitpos: [27:22]; default: 0;
         *  SPI_PAD_configure Q(D1)
         */
        uint32_t spi_pad_config_q:6;
        /** spi_pad_config_d : R; bitpos: [31:28]; default: 0;
         *  SPI_PAD_configure D(D0)
         */
        uint32_t spi_pad_config_d:4;
    };
    uint32_t val;
} efuse_rd_mac_spi_sys_1_reg_t;

/** Type of rd_mac_spi_sys_2 register
 *  Register 2 of BLOCK1.
 */
typedef union {
    struct {
        /** spi_pad_config_d_1 : R; bitpos: [1:0]; default: 0;
         *  SPI_PAD_configure D(D0)
         */
        uint32_t spi_pad_config_d_1:2;
        /** spi_pad_config_cs : R; bitpos: [7:2]; default: 0;
         *  SPI_PAD_configure CS
         */
        uint32_t spi_pad_config_cs:6;
        /** spi_pad_config_hd : R; bitpos: [13:8]; default: 0;
         *  SPI_PAD_configure HD(D3)
         */
        uint32_t spi_pad_config_hd:6;
        /** spi_pad_config_wp : R; bitpos: [19:14]; default: 0;
         *  SPI_PAD_configure WP(D2)
         */
        uint32_t spi_pad_config_wp:6;
        /** spi_pad_config_dqs : R; bitpos: [25:20]; default: 0;
         *  SPI_PAD_configure DQS
         */
        uint32_t spi_pad_config_dqs:6;
        /** spi_pad_config_d4 : R; bitpos: [31:26]; default: 0;
         *  SPI_PAD_configure D4
         */
        uint32_t spi_pad_config_d4:6;
    };
    uint32_t val;
} efuse_rd_mac_spi_sys_2_reg_t;

/** Type of rd_mac_spi_sys_3 register
 *  Register 3 of BLOCK1.
 */
typedef union {
    struct {
        /** spi_pad_config_d5 : R; bitpos: [5:0]; default: 0;
         *  SPI_PAD_configure D5
         */
        uint32_t spi_pad_config_d5:6;
        /** spi_pad_config_d6 : R; bitpos: [11:6]; default: 0;
         *  SPI_PAD_configure D6
         */
        uint32_t spi_pad_config_d6:6;
        /** spi_pad_config_d7 : R; bitpos: [17:12]; default: 0;
         *  SPI_PAD_configure D7
         */
        uint32_t spi_pad_config_d7:6;
        /** wafer_version_major : R; bitpos: [19:18]; default: 0;
         *  WAFER_VERSION_MAJOR
         */
        uint32_t wafer_version_major:2;
        /** wafer_version_minor_hi : R; bitpos: [20]; default: 0;
         *  WAFER_VERSION_MINOR most significant bit
         */
        uint32_t wafer_version_minor_hi:1;
        /** flash_version : R; bitpos: [24:21]; default: 0;
         *  Flash version
         */
        uint32_t flash_version:4;
        /** blk_version_major : R; bitpos: [26:25]; default: 0;
         *  BLK_VERSION_MAJOR
         */
        uint32_t blk_version_major:2;
        /** reserved_1_123 : R; bitpos: [27]; default: 0;
         *  reserved
         */
        uint32_t reserved_1_123:1;
        /** psram_version : R; bitpos: [31:28]; default: 0;
         *  PSRAM version
         */
        uint32_t psram_version:4;
    };
    uint32_t val;
} efuse_rd_mac_spi_sys_3_reg_t;

/** Type of rd_mac_spi_sys_4 register
 *  Register 4 of BLOCK1.
 */
typedef union {
    struct {
        /** pkg_version : R; bitpos: [3:0]; default: 0;
         *  Package version
         */
        uint32_t pkg_version:4;
        /** wafer_version_minor_lo : R; bitpos: [6:4]; default: 0;
         *  WAFER_VERSION_MINOR least significant bits
         */
        uint32_t wafer_version_minor_lo:3;
        /** reserved_1_135 : R; bitpos: [31:7]; default: 0;
         *  reserved
         */
        uint32_t reserved_1_135:25;
    };
    uint32_t val;
} efuse_rd_mac_spi_sys_4_reg_t;

/** Type of rd_mac_spi_sys_5 register
 *  Register 5 of BLOCK1.
 */
typedef union {
    struct {
        /** sys_data_part0_2 : RO; bitpos: [31:0]; default: 0;
         *  Stores the second part of the zeroth part of system data.
         */
        uint32_t sys_data_part0_2:32;
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
        /** adc_calib : R; bitpos: [3:0]; default: 0;
         *  4 bit of ADC calibration
         */
        uint32_t adc_calib:4;
        /** blk_version_minor : R; bitpos: [6:4]; default: 0;
         *  BLK_VERSION_MINOR of BLOCK2: 0-No ADC calib; 1-ADC calib V1; 2-ADC calib V2
         */
        uint32_t blk_version_minor:3;
        /** temp_calib : R; bitpos: [15:7]; default: 0;
         *  Temperature calibration data
         */
        uint32_t temp_calib:9;
        /** rtccalib_v1idx_a10h : R; bitpos: [23:16]; default: 0; */
        uint32_t rtccalib_v1idx_a10h:8;
        /** rtccalib_v1idx_a11h : R; bitpos: [31:24]; default: 0; */
        uint32_t rtccalib_v1idx_a11h:8;
    };
    uint32_t val;
} efuse_rd_sys_part1_data4_reg_t;

/** Type of rd_sys_part1_data5 register
 *  Register 5 of BLOCK2 (system).
 */
typedef union {
    struct {
        /** rtccalib_v1idx_a12h : R; bitpos: [7:0]; default: 0; */
        uint32_t rtccalib_v1idx_a12h:8;
        /** rtccalib_v1idx_a13h : R; bitpos: [15:8]; default: 0; */
        uint32_t rtccalib_v1idx_a13h:8;
        /** rtccalib_v1idx_a20h : R; bitpos: [23:16]; default: 0; */
        uint32_t rtccalib_v1idx_a20h:8;
        /** rtccalib_v1idx_a21h : R; bitpos: [31:24]; default: 0; */
        uint32_t rtccalib_v1idx_a21h:8;
    };
    uint32_t val;
} efuse_rd_sys_part1_data5_reg_t;

/** Type of rd_sys_part1_data6 register
 *  Register 6 of BLOCK2 (system).
 */
typedef union {
    struct {
        /** rtccalib_v1idx_a22h : R; bitpos: [7:0]; default: 0; */
        uint32_t rtccalib_v1idx_a22h:8;
        /** rtccalib_v1idx_a23h : R; bitpos: [15:8]; default: 0; */
        uint32_t rtccalib_v1idx_a23h:8;
        /** rtccalib_v1idx_a10l : R; bitpos: [21:16]; default: 0; */
        uint32_t rtccalib_v1idx_a10l:6;
        /** rtccalib_v1idx_a11l : R; bitpos: [27:22]; default: 0; */
        uint32_t rtccalib_v1idx_a11l:6;
        /** rtccalib_v1idx_a12l : R; bitpos: [31:28]; default: 0; */
        uint32_t rtccalib_v1idx_a12l:4;
    };
    uint32_t val;
} efuse_rd_sys_part1_data6_reg_t;

/** Type of rd_sys_part1_data7 register
 *  Register 7 of BLOCK2 (system).
 */
typedef union {
    struct {
        /** rtccalib_v1idx_a12l_1 : R; bitpos: [1:0]; default: 0; */
        uint32_t rtccalib_v1idx_a12l_1:2;
        /** rtccalib_v1idx_a13l : R; bitpos: [7:2]; default: 0; */
        uint32_t rtccalib_v1idx_a13l:6;
        /** rtccalib_v1idx_a20l : R; bitpos: [13:8]; default: 0; */
        uint32_t rtccalib_v1idx_a20l:6;
        /** rtccalib_v1idx_a21l : R; bitpos: [19:14]; default: 0; */
        uint32_t rtccalib_v1idx_a21l:6;
        /** rtccalib_v1idx_a22l : R; bitpos: [25:20]; default: 0; */
        uint32_t rtccalib_v1idx_a22l:6;
        /** rtccalib_v1idx_a23l : R; bitpos: [31:26]; default: 0; */
        uint32_t rtccalib_v1idx_a23l:6;
    };
    uint32_t val;
} efuse_rd_sys_part1_data7_reg_t;

/** Type of rd_usr_data0 register
 *  Register 0 of BLOCK3 (user).
 */
typedef union {
    struct {
        /** usr_data0 : RO; bitpos: [31:0]; default: 0;
         *  Stores the 0th 32 bits of BLOCK3 (user).
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
         *  Stores the 1th 32 bits of BLOCK3 (user).
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
         *  Stores the 2th 32 bits of BLOCK3 (user).
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
         *  Stores the 3th 32 bits of BLOCK3 (user).
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
         *  Stores the 4th 32 bits of BLOCK3 (user).
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
         *  Stores the 5th 32 bits of BLOCK3 (user).
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
         *  Custom MAC
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
         *  Custom MAC
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
         *  Stores the 0th 32 bits of KEY0.
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
         *  Stores the 1th 32 bits of KEY0.
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
         *  Stores the 2th 32 bits of KEY0.
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
         *  Stores the 3th 32 bits of KEY0.
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
         *  Stores the 4th 32 bits of KEY0.
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
         *  Stores the 5th 32 bits of KEY0.
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
         *  Stores the 6th 32 bits of KEY0.
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
         *  Stores the 7th 32 bits of KEY0.
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
         *  Stores the 0th 32 bits of KEY1.
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
         *  Stores the 1th 32 bits of KEY1.
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
         *  Stores the 2th 32 bits of KEY1.
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
         *  Stores the 3th 32 bits of KEY1.
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
         *  Stores the 4th 32 bits of KEY1.
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
         *  Stores the 5th 32 bits of KEY1.
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
         *  Stores the 6th 32 bits of KEY1.
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
         *  Stores the 7th 32 bits of KEY1.
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
         *  Stores the 0th 32 bits of KEY2.
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
         *  Stores the 1th 32 bits of KEY2.
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
         *  Stores the 2th 32 bits of KEY2.
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
         *  Stores the 3th 32 bits of KEY2.
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
         *  Stores the 4th 32 bits of KEY2.
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
         *  Stores the 5th 32 bits of KEY2.
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
         *  Stores the 6th 32 bits of KEY2.
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
         *  Stores the 7th 32 bits of KEY2.
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
         *  Stores the 0th 32 bits of KEY3.
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
         *  Stores the 1th 32 bits of KEY3.
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
         *  Stores the 2th 32 bits of KEY3.
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
         *  Stores the 3th 32 bits of KEY3.
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
         *  Stores the 4th 32 bits of KEY3.
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
         *  Stores the 5th 32 bits of KEY3.
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
         *  Stores the 6th 32 bits of KEY3.
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
         *  Stores the 7th 32 bits of KEY3.
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
         *  Stores the 0th 32 bits of KEY4.
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
         *  Stores the 1th 32 bits of KEY4.
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
         *  Stores the 2th 32 bits of KEY4.
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
         *  Stores the 3th 32 bits of KEY4.
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
         *  Stores the 4th 32 bits of KEY4.
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
         *  Stores the 5th 32 bits of KEY4.
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
         *  Stores the 6th 32 bits of KEY4.
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
         *  Stores the 7th 32 bits of KEY4.
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
         *  Stores the 0th 32 bits of KEY5.
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
         *  Stores the 1th 32 bits of KEY5.
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
         *  Stores the 2th 32 bits of KEY5.
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
         *  Stores the 3th 32 bits of KEY5.
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
         *  Stores the 4th 32 bits of KEY5.
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
         *  Stores the 5th 32 bits of KEY5.
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
         *  Stores the 6th 32 bits of KEY5.
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
         *  Stores the 7th 32 bits of KEY5.
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
 *  Register 1 of BLOCK10 (system).
 */
typedef union {
    struct {
        /** sys_data_part2_1 : RO; bitpos: [31:0]; default: 0;
         *  Stores the 1th 32 bits of the 2nd part of system data.
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
         *  Stores the 2th 32 bits of the 2nd part of system data.
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
         *  Stores the 3th 32 bits of the 2nd part of system data.
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


/** Group: Error Status Registers */
/** Type of rd_repeat_err0 register
 *  Programming error record register 0 of BLOCK0.
 */
typedef union {
    struct {
        /** rd_dis_err : RO; bitpos: [6:0]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_RD_DIS.
         */
        uint32_t rd_dis_err:7;
        /** dis_rtc_ram_boot_err : RO; bitpos: [7]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_DIS_RTC_RAM_BOOT.
         */
        uint32_t dis_rtc_ram_boot_err:1;
        /** dis_icache_err : RO; bitpos: [8]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_DIS_ICACHE.
         */
        uint32_t dis_icache_err:1;
        /** dis_dcache_err : RO; bitpos: [9]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_DIS_DCACHE.
         */
        uint32_t dis_dcache_err:1;
        /** dis_download_icache_err : RO; bitpos: [10]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_DIS_DOWNLOAD_ICACHE.
         */
        uint32_t dis_download_icache_err:1;
        /** dis_download_dcache_err : RO; bitpos: [11]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_DIS_DOWNLOAD_DCACHE.
         */
        uint32_t dis_download_dcache_err:1;
        /** dis_force_download_err : RO; bitpos: [12]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_DIS_FORCE_DOWNLOAD.
         */
        uint32_t dis_force_download_err:1;
        /** dis_usb_err : RO; bitpos: [13]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_DIS_USB.
         */
        uint32_t dis_usb_err:1;
        /** dis_can_err : RO; bitpos: [14]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_DIS_CAN.
         */
        uint32_t dis_can_err:1;
        /** dis_boot_remap_err : RO; bitpos: [15]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_DIS_BOOT_REMAP.
         */
        uint32_t dis_boot_remap_err:1;
        /** rpt4_reserved5_err : RO; bitpos: [16]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_RPT4_RESERVED5.
         */
        uint32_t rpt4_reserved5_err:1;
        /** soft_dis_jtag_err : RO; bitpos: [17]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_SOFT_DIS_JTAG.
         */
        uint32_t soft_dis_jtag_err:1;
        /** hard_dis_jtag_err : RO; bitpos: [18]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_HARD_DIS_JTAG.
         */
        uint32_t hard_dis_jtag_err:1;
        /** dis_download_manual_encrypt_err : RO; bitpos: [19]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT.
         */
        uint32_t dis_download_manual_encrypt_err:1;
        /** usb_drefh_err : RO; bitpos: [21:20]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_USB_DREFH.
         */
        uint32_t usb_drefh_err:2;
        /** usb_drefl_err : RO; bitpos: [23:22]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_USB_DREFL.
         */
        uint32_t usb_drefl_err:2;
        /** usb_exchg_pins_err : RO; bitpos: [24]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_USB_EXCHG_PINS.
         */
        uint32_t usb_exchg_pins_err:1;
        /** ext_phy_enable_err : RO; bitpos: [25]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_EXT_PHY_ENABLE.
         */
        uint32_t ext_phy_enable_err:1;
        /** usb_force_nopersist_err : RO; bitpos: [26]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_USB_FORCE_NOPERSIST.
         */
        uint32_t usb_force_nopersist_err:1;
        /** rpt4_reserved0_err : RO; bitpos: [28:27]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_RPT4_RESERVED0.
         */
        uint32_t rpt4_reserved0_err:2;
        /** vdd_spi_modecurlim_err : RO; bitpos: [29]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_VDD_SPI_MODECURLIM.
         */
        uint32_t vdd_spi_modecurlim_err:1;
        /** vdd_spi_drefh_err : RO; bitpos: [31:30]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_VDD_SPI_DREFH.
         */
        uint32_t vdd_spi_drefh_err:2;
    };
    uint32_t val;
} efuse_rd_repeat_err0_reg_t;

/** Type of rd_repeat_err1 register
 *  Programming error record register 1 of BLOCK0.
 */
typedef union {
    struct {
        /** vdd_spi_drefm_err : RO; bitpos: [1:0]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_VDD_SPI_DREFM.
         */
        uint32_t vdd_spi_drefm_err:2;
        /** vdd_spi_drefl_err : RO; bitpos: [3:2]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_VDD_SPI_DREFL.
         */
        uint32_t vdd_spi_drefl_err:2;
        /** vdd_spi_xpd_err : RO; bitpos: [4]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_VDD_SPI_XPD.
         */
        uint32_t vdd_spi_xpd_err:1;
        /** vdd_spi_tieh_err : RO; bitpos: [5]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_VDD_SPI_TIEH.
         */
        uint32_t vdd_spi_tieh_err:1;
        /** vdd_spi_force_err : RO; bitpos: [6]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_VDD_SPI_FORCE.
         */
        uint32_t vdd_spi_force_err:1;
        /** vdd_spi_en_init_err : RO; bitpos: [7]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_VDD_SPI_EN_INIT.
         */
        uint32_t vdd_spi_en_init_err:1;
        /** vdd_spi_encurlim_err : RO; bitpos: [8]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_VDD_SPI_ENCURLIM.
         */
        uint32_t vdd_spi_encurlim_err:1;
        /** vdd_spi_dcurlim_err : RO; bitpos: [11:9]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_VDD_SPI_DCURLIM.
         */
        uint32_t vdd_spi_dcurlim_err:3;
        /** vdd_spi_init_err : RO; bitpos: [13:12]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_VDD_SPI_INIT.
         */
        uint32_t vdd_spi_init_err:2;
        /** vdd_spi_dcap_err : RO; bitpos: [15:14]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_VDD_SPI_DCAP.
         */
        uint32_t vdd_spi_dcap_err:2;
        /** wdt_delay_sel_err : RO; bitpos: [17:16]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_WDT_DELAY_SEL.
         */
        uint32_t wdt_delay_sel_err:2;
        /** spi_boot_crypt_cnt_err : RO; bitpos: [20:18]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_SPI_BOOT_CRYPT_CNT.
         */
        uint32_t spi_boot_crypt_cnt_err:3;
        /** secure_boot_key_revoke0_err : RO; bitpos: [21]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_SECURE_BOOT_KEY_REVOKE0.
         */
        uint32_t secure_boot_key_revoke0_err:1;
        /** secure_boot_key_revoke1_err : RO; bitpos: [22]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_SECURE_BOOT_KEY_REVOKE1.
         */
        uint32_t secure_boot_key_revoke1_err:1;
        /** secure_boot_key_revoke2_err : RO; bitpos: [23]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_SECURE_BOOT_KEY_REVOKE2.
         */
        uint32_t secure_boot_key_revoke2_err:1;
        /** key_purpose_0_err : RO; bitpos: [27:24]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_KEY_PURPOSE_0.
         */
        uint32_t key_purpose_0_err:4;
        /** key_purpose_1_err : RO; bitpos: [31:28]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_KEY_PURPOSE_1.
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
         *  Any bit equal to 1 denotes a programming error in EFUSE_KEY_PURPOSE_2.
         */
        uint32_t key_purpose_2_err:4;
        /** key_purpose_3_err : RO; bitpos: [7:4]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_KEY_PURPOSE_3.
         */
        uint32_t key_purpose_3_err:4;
        /** key_purpose_4_err : RO; bitpos: [11:8]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_KEY_PURPOSE_4.
         */
        uint32_t key_purpose_4_err:4;
        /** key_purpose_5_err : RO; bitpos: [15:12]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_KEY_PURPOSE_5.
         */
        uint32_t key_purpose_5_err:4;
        /** key_purpose_6_err : RO; bitpos: [19:16]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_KEY_PURPOSE_6.
         */
        uint32_t key_purpose_6_err:4;
        /** secure_boot_en_err : RO; bitpos: [20]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_SECURE_BOOT_EN.
         */
        uint32_t secure_boot_en_err:1;
        /** secure_boot_aggressive_revoke_err : RO; bitpos: [21]; default: 0;
         *  Any bit equal to 1 denotes a programming error in
         *  EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE.
         */
        uint32_t secure_boot_aggressive_revoke_err:1;
        /** rpt4_reserved1_err : RO; bitpos: [27:22]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_RPT4_RESERVED1.
         */
        uint32_t rpt4_reserved1_err:6;
        /** flash_tpuw_err : RO; bitpos: [31:28]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_FLASH_TPUW.
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
         *  Any bit equal to 1 denotes a programming error in EFUSE_DIS_DOWNLOAD_MODE.
         */
        uint32_t dis_download_mode_err:1;
        /** dis_legacy_spi_boot_err : RO; bitpos: [1]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_DIS_LEGACY_SPI_BOOT.
         */
        uint32_t dis_legacy_spi_boot_err:1;
        /** uart_print_channel_err : RO; bitpos: [2]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_UART_PRINT_CHANNEL.
         */
        uint32_t uart_print_channel_err:1;
        /** rpt4_reserved3_err : RO; bitpos: [3]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_RPT4_RESERVED3.
         */
        uint32_t rpt4_reserved3_err:1;
        /** dis_usb_download_mode_err : RO; bitpos: [4]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_DIS_USB_DOWNLOAD_MODE.
         */
        uint32_t dis_usb_download_mode_err:1;
        /** enable_security_download_err : RO; bitpos: [5]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_ENABLE_SECURITY_DOWNLOAD.
         */
        uint32_t enable_security_download_err:1;
        /** uart_print_control_err : RO; bitpos: [7:6]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_UART_PRINT_CONTROL.
         */
        uint32_t uart_print_control_err:2;
        /** pin_power_selection_err : RO; bitpos: [8]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_PIN_POWER_SELECTION.
         */
        uint32_t pin_power_selection_err:1;
        /** flash_type_err : RO; bitpos: [9]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_FLASH_TYPE.
         */
        uint32_t flash_type_err:1;
        /** force_send_resume_err : RO; bitpos: [10]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_FORCE_SEND_RESUME.
         */
        uint32_t force_send_resume_err:1;
        /** secure_version_err : RO; bitpos: [26:11]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_SECURE_VERSION.
         */
        uint32_t secure_version_err:16;
        /** rpt4_reserved2_err : RO; bitpos: [31:27]; default: 0;
         *  Any bit equal to 1 denotes a programming error in EFUSE_RPT4_RESERVED2.
         */
        uint32_t rpt4_reserved2_err:5;
    };
    uint32_t val;
} efuse_rd_repeat_err3_reg_t;

/** Type of rd_repeat_err4 register
 *  Programming error record register 4 of BLOCK0.
 */
typedef union {
    struct {
        /** rpt4_reserved4_err : RO; bitpos: [23:0]; default: 0;
         *  If any bit in RPT4_RESERVED4 is 1, there is a programming error in
         *  EFUSE_RPT4_RESERVED4.
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
         *  The value of this signal means the number of error bytes in BLOCK1.
         */
        uint32_t mac_spi_8m_err_num:3;
        /** mac_spi_8m_fail : RO; bitpos: [3]; default: 0;
         *  0: Means no failure and that the data of BLOCK1 is reliable. 1: Means that
         *  programming BLOCK1 data failed and the number of error bytes is over 5.
         */
        uint32_t mac_spi_8m_fail:1;
        /** sys_part1_num : RO; bitpos: [6:4]; default: 0;
         *  The value of this signal means the number of error bytes in BLOCK2.
         */
        uint32_t sys_part1_num:3;
        /** sys_part1_fail : RO; bitpos: [7]; default: 0;
         *  0: Means no failure and that the data of BLOCK2 is reliable. 1: Means that
         *  programming BLOCK2 data failed and the number of error bytes is over 5.
         */
        uint32_t sys_part1_fail:1;
        /** usr_data_err_num : RO; bitpos: [10:8]; default: 0;
         *  The value of this signal means the number of error bytes in BLOCK3.
         */
        uint32_t usr_data_err_num:3;
        /** usr_data_fail : RO; bitpos: [11]; default: 0;
         *  0: Means no failure and that the data of BLOCK3 is reliable. 1: Means that
         *  programming BLOCK3 data failed and the number of error bytes is over 5.
         */
        uint32_t usr_data_fail:1;
        /** key0_err_num : RO; bitpos: [14:12]; default: 0;
         *  The value of this signal means the number of error bytes in KEY0.
         */
        uint32_t key0_err_num:3;
        /** key0_fail : RO; bitpos: [15]; default: 0;
         *  0: Means no failure and that the data of KEY0 is reliable. 1: Means that
         *  programming KEY0 failed and the number of error bytes is over 5.
         */
        uint32_t key0_fail:1;
        /** key1_err_num : RO; bitpos: [18:16]; default: 0;
         *  The value of this signal means the number of error bytes in KEY1.
         */
        uint32_t key1_err_num:3;
        /** key1_fail : RO; bitpos: [19]; default: 0;
         *  0: Means no failure and that the data of KEY1 is reliable. 1: Means that
         *  programming KEY1 failed and the number of error bytes is over 5.
         */
        uint32_t key1_fail:1;
        /** key2_err_num : RO; bitpos: [22:20]; default: 0;
         *  The value of this signal means the number of error bytes in KEY2.
         */
        uint32_t key2_err_num:3;
        /** key2_fail : RO; bitpos: [23]; default: 0;
         *  0: Means no failure and that the data of KEY2 is reliable. 1: Means that
         *  programming KEY2 failed and the number of error bytes is over 5.
         */
        uint32_t key2_fail:1;
        /** key3_err_num : RO; bitpos: [26:24]; default: 0;
         *  The value of this signal means the number of error bytes in KEY3.
         */
        uint32_t key3_err_num:3;
        /** key3_fail : RO; bitpos: [27]; default: 0;
         *  0: Means no failure and that the data of KEY3 is reliable. 1: Means that
         *  programming KEY3 failed and the number of error bytes is over 5.
         */
        uint32_t key3_fail:1;
        /** key4_err_num : RO; bitpos: [30:28]; default: 0;
         *  The value of this signal means the number of error bytes in KEY4.
         */
        uint32_t key4_err_num:3;
        /** key4_fail : RO; bitpos: [31]; default: 0;
         *  0: Means no failure and that the data of KEY4 is reliable. 1: Means that
         *  programming KEY4 failed and the number of error bytes is over 5.
         */
        uint32_t key4_fail:1;
    };
    uint32_t val;
} efuse_rd_rs_err0_reg_t;

/** Type of rd_rs_err1 register
 *  Programming error record register 1 of BLOCK1-10.
 */
typedef union {
    struct {
        /** key5_err_num : RO; bitpos: [2:0]; default: 0;
         *  The value of this signal means the number of error bytes in KEY5.
         */
        uint32_t key5_err_num:3;
        /** key5_fail : RO; bitpos: [3]; default: 0;
         *  0: Means no failure and that the data of KEY5 is reliable. 1: Means that
         *  programming user data failed and the number of error bytes is over 5.
         */
        uint32_t key5_fail:1;
        /** sys_part2_err_num : RO; bitpos: [6:4]; default: 0;
         *  The value of this signal means the number of error bytes in BLOCK10.
         */
        uint32_t sys_part2_err_num:3;
        /** sys_part2_fail : RO; bitpos: [7]; default: 0;
         *  0: Means no failure and that the data of BLOCK10 is reliable. 1: Means that
         *  programming BLOCK10 data failed and the number of error bytes is over 5.
         */
        uint32_t sys_part2_fail:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} efuse_rd_rs_err1_reg_t;


/** Group: Control/Status Registers */
/** Type of clk register
 *  eFuse clock configuration register.
 */
typedef union {
    struct {
        /** efuse_mem_force_pd : R/W; bitpos: [0]; default: 0;
         *  If set, forces eFuse SRAM into power-saving mode.
         */
        uint32_t efuse_mem_force_pd:1;
        /** mem_clk_force_on : R/W; bitpos: [1]; default: 1;
         *  If set, forces to activate clock signal of eFuse SRAM.
         */
        uint32_t mem_clk_force_on:1;
        /** efuse_mem_force_pu : R/W; bitpos: [2]; default: 0;
         *  If set, forces eFuse SRAM into working mode.
         */
        uint32_t efuse_mem_force_pu:1;
        uint32_t reserved_3:13;
        /** clk_en : R/W; bitpos: [16]; default: 0;
         *  If set, forces to enable clock signal of eFuse memory.
         */
        uint32_t clk_en:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} efuse_clk_reg_t;

/** Type of conf register
 *  eFuse operation mode configuration register.
 */
typedef union {
    struct {
        /** op_code : R/W; bitpos: [15:0]; default: 0;
         *  0x5A5A: Operate programming command. 0x5AA5: Operate read command.
         */
        uint32_t op_code:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} efuse_conf_reg_t;

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

/** Type of cmd register
 *  eFuse command register.
 */
typedef union {
    struct {
        /** read_cmd : R/W; bitpos: [0]; default: 0;
         *  Set this bit to send read command.
         */
        uint32_t read_cmd:1;
        /** pgm_cmd : R/W; bitpos: [1]; default: 0;
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


/** Group: Interrupt Registers */
/** Type of int_raw register
 *  eFuse raw interrupt register.
 */
typedef union {
    struct {
        /** read_done_int_raw : RO; bitpos: [0]; default: 0;
         *  The raw bit signal for read_done interrupt.
         */
        uint32_t read_done_int_raw:1;
        /** pgm_done_int_raw : RO; bitpos: [1]; default: 0;
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


/** Group: Configuration Registers */
/** Type of rd_tim_conf register
 *  Configures read timing parameters.
 */
typedef union {
    struct {
        /** thr_a : R/W; bitpos: [7:0]; default: 1;
         *  Configures the hold time of read operation.
         */
        uint32_t thr_a:8;
        /** trd : R/W; bitpos: [15:8]; default: 1;
         *  Configures the length of pulse of read operation.
         */
        uint32_t trd:8;
        /** tsur_a : R/W; bitpos: [23:16]; default: 1;
         *  Configures the setup time of read operation.
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
 *  Configuration register 0 of eFuse programming timing parameters.
 */
typedef union {
    struct {
        /** thp_a : R/W; bitpos: [7:0]; default: 1;
         *  Configures the hold time of programming operation.
         */
        uint32_t thp_a:8;
        /** tpgm_inactive : R/W; bitpos: [15:8]; default: 1;
         *  Configures the length of pulse during programming 0 to eFuse.
         */
        uint32_t tpgm_inactive:8;
        /** tpgm : R/W; bitpos: [31:16]; default: 200;
         *  Configures the length of pulse during programming 1 to eFuse.
         */
        uint32_t tpgm:16;
    };
    uint32_t val;
} efuse_wr_tim_conf0_reg_t;

/** Type of wr_tim_conf1 register
 *  Configuration register 1 of eFuse programming timing parameters.
 */
typedef union {
    struct {
        /** tsup_a : R/W; bitpos: [7:0]; default: 1;
         *  Configures the setup time of programming operation.
         */
        uint32_t tsup_a:8;
        /** pwr_on_num : R/W; bitpos: [23:8]; default: 10368;
         *  Configures the power up time for VDDQ.
         */
        uint32_t pwr_on_num:16;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} efuse_wr_tim_conf1_reg_t;

/** Type of wr_tim_conf2 register
 *  Configuration register 2 of eFuse programming timing parameters.
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


/** Group: Version Register */
/** Type of date register
 *  Version control register.
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [31:0]; default: 419959040;
         *  Version control register.
         */
        uint32_t date:32;
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
    volatile efuse_wr_tim_conf0_reg_t wr_tim_conf0;
    volatile efuse_wr_tim_conf1_reg_t wr_tim_conf1;
    volatile efuse_wr_tim_conf2_reg_t wr_tim_conf2;
    volatile efuse_date_reg_t date;
} efuse_dev_t;

extern efuse_dev_t EFUSE;

#ifndef __cplusplus
_Static_assert(sizeof(efuse_dev_t) == 0x200, "Invalid size of efuse_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
