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
         *  Configures the 0th 32-bit data to be programmed.
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
         *  Configures the 1st 32-bit data to be programmed.
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
         *  Configures the 2nd 32-bit data to be programmed.
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
         *  Configures the 3rd 32-bit data to be programmed.
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
         *  Configures the 4th 32-bit data to be programmed.
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
         *  Configures the 5th 32-bit data to be programmed.
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
         *  Configures the 6th 32-bit data to be programmed.
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
         *  Configures the 7th 32-bit data to be programmed.
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
         *  Configures the 0th 32-bit RS code to be programmed.
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
         *  Configures the 1st 32-bit RS code to be programmed.
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
         *  Configures the 2nd 32-bit RS code to be programmed.
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
         *  Represents whether programming of individual eFuse memory bit is disabled or
         *  enabled. 1: Disabled. 0 Enabled.
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
         *  Represents whether reading of individual eFuse block(block4~block10) is disabled or
         *  enabled. 1: disabled. 0: enabled.
         */
        uint32_t rd_dis:7;
        /** swap_uart_sdio_en : RO; bitpos: [7]; default: 0;
         *  Represents whether pad of uart and sdio is swapped or not. 1: swapped. 0: not
         *  swapped.
         */
        uint32_t swap_uart_sdio_en:1;
        /** dis_icache : RO; bitpos: [8]; default: 0;
         *  Represents whether icache is disabled or enabled. 1: disabled. 0: enabled.
         */
        uint32_t dis_icache:1;
        /** dis_usb_jtag : RO; bitpos: [9]; default: 0;
         *  Represents whether the function of usb switch to jtag is disabled or enabled. 1:
         *  disabled. 0: enabled.
         */
        uint32_t dis_usb_jtag:1;
        /** dis_download_icache : RO; bitpos: [10]; default: 0;
         *  Represents whether icache is disabled or enabled in Download mode. 1: disabled. 0:
         *  enabled.
         */
        uint32_t dis_download_icache:1;
        /** dis_usb_serial_jtag : RO; bitpos: [11]; default: 0;
         *  Represents whether USB-Serial-JTAG is disabled or enabled. 1: disabled. 0: enabled.
         */
        uint32_t dis_usb_serial_jtag:1;
        /** dis_force_download : RO; bitpos: [12]; default: 0;
         *  Represents whether the function that forces chip into download mode is disabled or
         *  enabled. 1: disabled. 0: enabled.
         */
        uint32_t dis_force_download:1;
        /** spi_download_mspi_dis : RO; bitpos: [13]; default: 0;
         *  Represents whether SPI0 controller during boot_mode_download is disabled or
         *  enabled. 1: disabled. 0: enabled.
         */
        uint32_t spi_download_mspi_dis:1;
        /** dis_twai : RO; bitpos: [14]; default: 0;
         *  Represents whether TWAI function is disabled or enabled. 1: disabled. 0: enabled.
         */
        uint32_t dis_twai:1;
        /** jtag_sel_enable : RO; bitpos: [15]; default: 0;
         *  Represents whether the selection between usb_to_jtag and pad_to_jtag through
         *  strapping gpio15 when both EFUSE_DIS_PAD_JTAG and EFUSE_DIS_USB_JTAG are equal to 0
         *  is enabled or disabled. 1: enabled. 0: disabled.
         */
        uint32_t jtag_sel_enable:1;
        /** soft_dis_jtag : RO; bitpos: [18:16]; default: 0;
         *  Represents whether JTAG is disabled in soft way. Odd number: disabled. Even number:
         *  enabled.
         */
        uint32_t soft_dis_jtag:3;
        /** dis_pad_jtag : RO; bitpos: [19]; default: 0;
         *  Represents whether JTAG is disabled in the hard way(permanently). 1: disabled. 0:
         *  enabled.
         */
        uint32_t dis_pad_jtag:1;
        /** dis_download_manual_encrypt : RO; bitpos: [20]; default: 0;
         *  Represents whether flash encrypt function is disabled or enabled(except in SPI boot
         *  mode). 1: disabled. 0: enabled.
         */
        uint32_t dis_download_manual_encrypt:1;
        /** usb_drefh : RO; bitpos: [22:21]; default: 0;
         *  Represents the single-end input threhold vrefh, 1.76 V to 2 V with step of 80 mV.
         */
        uint32_t usb_drefh:2;
        /** usb_drefl : RO; bitpos: [24:23]; default: 0;
         *  Represents the single-end input threhold vrefl, 1.76 V to 2 V with step of 80 mV.
         */
        uint32_t usb_drefl:2;
        /** usb_exchg_pins : RO; bitpos: [25]; default: 0;
         *  Represents whether the D+ and D- pins is exchanged. 1: exchanged. 0: not exchanged.
         */
        uint32_t usb_exchg_pins:1;
        /** vdd_spi_as_gpio : RO; bitpos: [26]; default: 0;
         *  Represents whether vdd spi pin is functioned as gpio. 1: functioned. 0: not
         *  functioned.
         */
        uint32_t vdd_spi_as_gpio:1;
        /** rpt4_reserved0_2 : RO; bitpos: [28:27]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved0_2:2;
        /** rpt4_reserved0_1 : RO; bitpos: [29]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved0_1:1;
        /** rpt4_reserved0_0 : RO; bitpos: [31:30]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved0_0:2;
    };
    uint32_t val;
} efuse_rd_repeat_data0_reg_t;

/** Type of rd_repeat_data1 register
 *  BLOCK0 data register 2.
 */
typedef union {
    struct {
        /** rpt4_reserved1_0 : RO; bitpos: [15:0]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved1_0:16;
        /** wdt_delay_sel : RO; bitpos: [17:16]; default: 0;
         *  Represents whether RTC watchdog timeout threshold is selected at startup. 1:
         *  selected. 0: not selected.
         */
        uint32_t wdt_delay_sel:2;
        /** spi_boot_crypt_cnt : RO; bitpos: [20:18]; default: 0;
         *  Represents whether SPI boot encrypt/decrypt is disabled or enabled. Odd number of
         *  1: enabled. Even number of 1: disabled.
         */
        uint32_t spi_boot_crypt_cnt:3;
        /** secure_boot_key_revoke0 : RO; bitpos: [21]; default: 0;
         *  Represents whether revoking first secure boot key is enabled or disabled. 1:
         *  enabled. 0: disabled.
         */
        uint32_t secure_boot_key_revoke0:1;
        /** secure_boot_key_revoke1 : RO; bitpos: [22]; default: 0;
         *  Represents whether revoking second secure boot key is enabled or disabled. 1:
         *  enabled. 0: disabled.
         */
        uint32_t secure_boot_key_revoke1:1;
        /** secure_boot_key_revoke2 : RO; bitpos: [23]; default: 0;
         *  Represents whether revoking third secure boot key is enabled or disabled. 1:
         *  enabled. 0: disabled.
         */
        uint32_t secure_boot_key_revoke2:1;
        /** key_purpose_0 : RO; bitpos: [27:24]; default: 0;
         *  Represents the purpose of Key0.
         */
        uint32_t key_purpose_0:4;
        /** key_purpose_1 : RO; bitpos: [31:28]; default: 0;
         *  Represents the purpose of Key1.
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
         *  Represents the purpose of Key2.
         */
        uint32_t key_purpose_2:4;
        /** key_purpose_3 : RO; bitpos: [7:4]; default: 0;
         *  Represents the purpose of Key3.
         */
        uint32_t key_purpose_3:4;
        /** key_purpose_4 : RO; bitpos: [11:8]; default: 0;
         *  Represents the purpose of Key4.
         */
        uint32_t key_purpose_4:4;
        /** key_purpose_5 : RO; bitpos: [15:12]; default: 0;
         *  Represents the purpose of Key5.
         */
        uint32_t key_purpose_5:4;
        /** sec_dpa_level : RO; bitpos: [17:16]; default: 0;
         *  Represents the spa secure level by configuring the clock random divide mode.
         */
        uint32_t sec_dpa_level:2;
        /** crypt_dpa_enable : RO; bitpos: [18]; default: 0;
         *  Represents whether anti-dpa attack is enabled. 1:enabled. 0: disabled.
         */
        uint32_t crypt_dpa_enable:1;
        /** rpt4_reserved2_1 : RO; bitpos: [19]; default: 1;
         *  Reserved.
         */
        uint32_t rpt4_reserved2_1:1;
        /** secure_boot_en : RO; bitpos: [20]; default: 0;
         *  Represents whether secure boot is enabled or disabled. 1: enabled. 0: disabled.
         */
        uint32_t secure_boot_en:1;
        /** secure_boot_aggressive_revoke : RO; bitpos: [21]; default: 0;
         *  Represents whether revoking aggressive secure boot is enabled or disabled. 1:
         *  enabled. 0: disabled.
         */
        uint32_t secure_boot_aggressive_revoke:1;
        /** rpt4_reserved2_0 : RO; bitpos: [27:22]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved2_0:6;
        /** flash_tpuw : RO; bitpos: [31:28]; default: 0;
         *  Represents the flash waiting time after power-up, in unit of ms. When the value
         *  less than 15, the waiting time is the programmed value. Otherwise, the waiting time
         *  is 2 times the programmed value.
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
         *  Represents whether Download mode is disabled or enabled. 1: disabled. 0: enabled.
         */
        uint32_t dis_download_mode:1;
        /** dis_direct_boot : RO; bitpos: [1]; default: 0;
         *  Represents whether direct boot mode is disabled or enabled. 1: disabled. 0: enabled.
         */
        uint32_t dis_direct_boot:1;
        /** dis_usb_serial_jtag_rom_print : RO; bitpos: [2]; default: 0;
         *  Represents whether print from USB-Serial-JTAG is disabled or enabled. 1: disabled.
         *  0: enabled.
         */
        uint32_t dis_usb_serial_jtag_rom_print:1;
        /** rpt4_reserved3_5 : RO; bitpos: [3]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved3_5:1;
        /** dis_usb_serial_jtag_download_mode : RO; bitpos: [4]; default: 0;
         *  Represents whether the USB-Serial-JTAG download function is disabled or enabled. 1:
         *  disabled. 0: enabled.
         */
        uint32_t dis_usb_serial_jtag_download_mode:1;
        /** enable_security_download : RO; bitpos: [5]; default: 0;
         *  Represents whether security download is enabled or disabled. 1: enabled. 0:
         *  disabled.
         */
        uint32_t enable_security_download:1;
        /** uart_print_control : RO; bitpos: [7:6]; default: 0;
         *  Represents the type of UART printing. 00: force enable printing. 01: enable
         *  printing when GPIO8 is reset at low level. 10: enable printing when GPIO8 is reset
         *  at high level. 11: force disable printing.
         */
        uint32_t uart_print_control:2;
        /** rpt4_reserved3_4 : RO; bitpos: [8]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved3_4:1;
        /** rpt4_reserved3_3 : RO; bitpos: [9]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved3_3:1;
        /** rpt4_reserved3_2 : RO; bitpos: [11:10]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved3_2:2;
        /** rpt4_reserved3_1 : RO; bitpos: [12]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved3_1:1;
        /** force_send_resume : RO; bitpos: [13]; default: 0;
         *  Represents whether ROM code is forced to send a resume command during SPI boot. 1:
         *  forced. 0:not forced.
         */
        uint32_t force_send_resume:1;
        /** secure_version : RO; bitpos: [29:14]; default: 0;
         *  Represents the version used by ESP-IDF anti-rollback feature.
         */
        uint32_t secure_version:16;
        /** secure_boot_disable_fast_wake : RO; bitpos: [30]; default: 0;
         *  Represents whether FAST VERIFY ON WAKE is disabled or enabled when Secure Boot is
         *  enabled. 1: disabled. 0: enabled.
         */
        uint32_t secure_boot_disable_fast_wake:1;
        /** rpt4_reserved3_0 : RO; bitpos: [31]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved3_0:1;
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
        /** rpt4_reserved4_0 : RO; bitpos: [31:24]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved4_0:8;
    };
    uint32_t val;
} efuse_rd_repeat_data4_reg_t;

/** Type of rd_mac_spi_sys_0 register
 *  BLOCK1 data register $n.
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
 *  BLOCK1 data register $n.
 */
typedef union {
    struct {
        /** mac_1 : RO; bitpos: [15:0]; default: 0;
         *  Stores the high 16 bits of MAC address.
         */
        uint32_t mac_1:16;
        /** mac_ext : RO; bitpos: [31:16]; default: 0;
         *  Stores the extended bits of MAC address.
         */
        uint32_t mac_ext:16;
    };
    uint32_t val;
} efuse_rd_mac_spi_sys_1_reg_t;

/** Type of rd_mac_spi_sys_2 register
 *  BLOCK1 data register $n.
 */
typedef union {
    struct {
        /** mac_spi_reserved : RO; bitpos: [13:0]; default: 0;
         *  Reserved.
         */
        uint32_t mac_spi_reserved:14;
        /** spi_pad_conf_1 : RO; bitpos: [31:14]; default: 0;
         *  Stores the first part of SPI_PAD_CONF.
         */
        uint32_t spi_pad_conf_1:18;
    };
    uint32_t val;
} efuse_rd_mac_spi_sys_2_reg_t;

/** Type of rd_mac_spi_sys_3 register
 *  BLOCK1 data register $n.
 */
typedef union {
    struct {
        /** spi_pad_conf_2 : RO; bitpos: [17:0]; default: 0;
         *  Stores the second part of SPI_PAD_CONF.
         */
        uint32_t spi_pad_conf_2:18;
        /** wafer_version_minor : R; bitpos: [21:18]; default: 0; */
        uint32_t wafer_version_minor:4;
        /** wafer_version_major : R; bitpos: [23:22]; default: 0; */
        uint32_t wafer_version_major:2;
        /** pkg_version : R; bitpos: [26:24]; default: 0;
         *  Package version
         */
        uint32_t pkg_version:3;
        /** blk_version_minor : R; bitpos: [29:27]; default: 0;
         *  BLK_VERSION_MINOR of BLOCK2
         */
        uint32_t blk_version_minor:3;
        /** blk_version_major : R; bitpos: [31:30]; default: 0;
         *  BLK_VERSION_MAJOR of BLOCK2
         */
        uint32_t blk_version_major:2;
    };
    uint32_t val;
} efuse_rd_mac_spi_sys_3_reg_t;

/** Type of rd_mac_spi_sys_4 register
 *  BLOCK1 data register $n.
 */
typedef union {
    struct {
        /** flash_cap : R; bitpos: [2:0]; default: 0; */
        uint32_t flash_cap:3;
        /** flash_temp : R; bitpos: [4:3]; default: 0; */
        uint32_t flash_temp:2;
        /** flash_vendor : R; bitpos: [7:5]; default: 0; */
        uint32_t flash_vendor:3;
        /** reserved_1_136 : R; bitpos: [31:8]; default: 0;
         *  reserved
         */
        uint32_t reserved_1_136:24;
    };
    uint32_t val;
} efuse_rd_mac_spi_sys_4_reg_t;

/** Type of rd_mac_spi_sys_5 register
 *  BLOCK1 data register $n.
 */
typedef union {
    struct {
        /** sys_data_part0_2 : RO; bitpos: [31:0]; default: 0;
         *  Stores the second 32 bits of the zeroth part of system data.
         */
        uint32_t sys_data_part0_2:32;
    };
    uint32_t val;
} efuse_rd_mac_spi_sys_5_reg_t;

/** Type of rd_sys_part1_data0 register
 *  Register $n of BLOCK2 (system).
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
 *  Register $n of BLOCK2 (system).
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
 *  Register $n of BLOCK2 (system).
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
 *  Register $n of BLOCK2 (system).
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
 *  Register $n of BLOCK2 (system).
 */
typedef union {
    struct {
        /** temp_calib : R; bitpos: [8:0]; default: 0;
         *  Temperature calibration data
         */
        uint32_t temp_calib:9;
        /** ocode : R; bitpos: [16:9]; default: 0;
         *  ADC OCode
         */
        uint32_t ocode:8;
        /** adc1_init_code_atten0 : R; bitpos: [26:17]; default: 0;
         *  ADC1 init code at atten0
         */
        uint32_t adc1_init_code_atten0:10;
        /** adc1_init_code_atten1 : R; bitpos: [31:27]; default: 0;
         *  ADC1 init code at atten1
         */
        uint32_t adc1_init_code_atten1:5;
    };
    uint32_t val;
} efuse_rd_sys_part1_data4_reg_t;

/** Type of rd_sys_part1_data5 register
 *  Register $n of BLOCK2 (system).
 */
typedef union {
    struct {
        /** adc1_init_code_atten1_1 : R; bitpos: [4:0]; default: 0;
         *  ADC1 init code at atten1
         */
        uint32_t adc1_init_code_atten1_1:5;
        /** adc1_init_code_atten2 : R; bitpos: [14:5]; default: 0;
         *  ADC1 init code at atten2
         */
        uint32_t adc1_init_code_atten2:10;
        /** adc1_init_code_atten3 : R; bitpos: [24:15]; default: 0;
         *  ADC1 init code at atten3
         */
        uint32_t adc1_init_code_atten3:10;
        /** adc1_cal_vol_atten0 : R; bitpos: [31:25]; default: 0;
         *  ADC1 calibration voltage at atten0
         */
        uint32_t adc1_cal_vol_atten0:7;
    };
    uint32_t val;
} efuse_rd_sys_part1_data5_reg_t;

/** Type of rd_sys_part1_data6 register
 *  Register $n of BLOCK2 (system).
 */
typedef union {
    struct {
        /** adc1_cal_vol_atten0_1 : R; bitpos: [2:0]; default: 0;
         *  ADC1 calibration voltage at atten0
         */
        uint32_t adc1_cal_vol_atten0_1:3;
        /** adc1_cal_vol_atten1 : R; bitpos: [12:3]; default: 0;
         *  ADC1 calibration voltage at atten1
         */
        uint32_t adc1_cal_vol_atten1:10;
        /** adc1_cal_vol_atten2 : R; bitpos: [22:13]; default: 0;
         *  ADC1 calibration voltage at atten2
         */
        uint32_t adc1_cal_vol_atten2:10;
        /** adc1_cal_vol_atten3 : R; bitpos: [31:23]; default: 0;
         *  ADC1 calibration voltage at atten3
         */
        uint32_t adc1_cal_vol_atten3:9;
    };
    uint32_t val;
} efuse_rd_sys_part1_data6_reg_t;

/** Type of rd_sys_part1_data7 register
 *  Register $n of BLOCK2 (system).
 */
typedef union {
    struct {
        /** adc1_cal_vol_atten3_1 : R; bitpos: [0]; default: 0;
         *  ADC1 calibration voltage at atten3
         */
        uint32_t adc1_cal_vol_atten3_1:1;
        /** adc1_init_code_atten0_ch0 : R; bitpos: [4:1]; default: 0;
         *  ADC1 init code at atten0 ch0
         */
        uint32_t adc1_init_code_atten0_ch0:4;
        /** adc1_init_code_atten0_ch1 : R; bitpos: [8:5]; default: 0;
         *  ADC1 init code at atten0 ch1
         */
        uint32_t adc1_init_code_atten0_ch1:4;
        /** adc1_init_code_atten0_ch2 : R; bitpos: [12:9]; default: 0;
         *  ADC1 init code at atten0 ch2
         */
        uint32_t adc1_init_code_atten0_ch2:4;
        /** adc1_init_code_atten0_ch3 : R; bitpos: [16:13]; default: 0;
         *  ADC1 init code at atten0 ch3
         */
        uint32_t adc1_init_code_atten0_ch3:4;
        /** adc1_init_code_atten0_ch4 : R; bitpos: [20:17]; default: 0;
         *  ADC1 init code at atten0 ch4
         */
        uint32_t adc1_init_code_atten0_ch4:4;
        /** adc1_init_code_atten0_ch5 : R; bitpos: [24:21]; default: 0;
         *  ADC1 init code at atten0 ch5
         */
        uint32_t adc1_init_code_atten0_ch5:4;
        /** adc1_init_code_atten0_ch6 : R; bitpos: [28:25]; default: 0;
         *  ADC1 init code at atten0 ch6
         */
        uint32_t adc1_init_code_atten0_ch6:4;
        /** reserved_2_253 : R; bitpos: [31:29]; default: 0;
         *  reserved
         */
        uint32_t reserved_2_253:3;
    };
    uint32_t val;
} efuse_rd_sys_part1_data7_reg_t;

/** Type of rd_usr_data0 register
 *  Register $n of BLOCK3 (user).
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
 *  Register $n of BLOCK3 (user).
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
 *  Register $n of BLOCK3 (user).
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
 *  Register $n of BLOCK3 (user).
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
 *  Register $n of BLOCK3 (user).
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
 *  Register $n of BLOCK3 (user).
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
 *  Register $n of BLOCK3 (user).
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
 *  Register $n of BLOCK3 (user).
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
 *  Register $n of BLOCK4 (KEY0).
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
 *  Register $n of BLOCK4 (KEY0).
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
 *  Register $n of BLOCK4 (KEY0).
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
 *  Register $n of BLOCK4 (KEY0).
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
 *  Register $n of BLOCK4 (KEY0).
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
 *  Register $n of BLOCK4 (KEY0).
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
 *  Register $n of BLOCK4 (KEY0).
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
 *  Register $n of BLOCK4 (KEY0).
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
 *  Register $n of BLOCK5 (KEY1).
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
 *  Register $n of BLOCK5 (KEY1).
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
 *  Register $n of BLOCK5 (KEY1).
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
 *  Register $n of BLOCK5 (KEY1).
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
 *  Register $n of BLOCK5 (KEY1).
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
 *  Register $n of BLOCK5 (KEY1).
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
 *  Register $n of BLOCK5 (KEY1).
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
 *  Register $n of BLOCK5 (KEY1).
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
 *  Register $n of BLOCK6 (KEY2).
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
 *  Register $n of BLOCK6 (KEY2).
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
 *  Register $n of BLOCK6 (KEY2).
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
 *  Register $n of BLOCK6 (KEY2).
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
 *  Register $n of BLOCK6 (KEY2).
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
 *  Register $n of BLOCK6 (KEY2).
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
 *  Register $n of BLOCK6 (KEY2).
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
 *  Register $n of BLOCK6 (KEY2).
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
 *  Register $n of BLOCK7 (KEY3).
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
 *  Register $n of BLOCK7 (KEY3).
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
 *  Register $n of BLOCK7 (KEY3).
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
 *  Register $n of BLOCK7 (KEY3).
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
 *  Register $n of BLOCK7 (KEY3).
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
 *  Register $n of BLOCK7 (KEY3).
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
 *  Register $n of BLOCK7 (KEY3).
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
 *  Register $n of BLOCK7 (KEY3).
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
 *  Register $n of BLOCK8 (KEY4).
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
 *  Register $n of BLOCK8 (KEY4).
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
 *  Register $n of BLOCK8 (KEY4).
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
 *  Register $n of BLOCK8 (KEY4).
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
 *  Register $n of BLOCK8 (KEY4).
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
 *  Register $n of BLOCK8 (KEY4).
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
 *  Register $n of BLOCK8 (KEY4).
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
 *  Register $n of BLOCK8 (KEY4).
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
 *  Register $n of BLOCK9 (KEY5).
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
 *  Register $n of BLOCK9 (KEY5).
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
 *  Register $n of BLOCK9 (KEY5).
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
 *  Register $n of BLOCK9 (KEY5).
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
 *  Register $n of BLOCK9 (KEY5).
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
 *  Register $n of BLOCK9 (KEY5).
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
 *  Register $n of BLOCK9 (KEY5).
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
 *  Register $n of BLOCK9 (KEY5).
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
 *  Register $n of BLOCK10 (system).
 */
typedef union {
    struct {
        /** sys_data_part2_0 : RO; bitpos: [31:0]; default: 0;
         *  Stores the $nth 32 bits of the 2nd part of system data.
         */
        uint32_t sys_data_part2_0:32;
    };
    uint32_t val;
} efuse_rd_sys_part2_data0_reg_t;

/** Type of rd_sys_part2_data1 register
 *  Register $n of BLOCK9 (KEY5).
 */
typedef union {
    struct {
        /** sys_data_part2_1 : RO; bitpos: [31:0]; default: 0;
         *  Stores the $nth 32 bits of the 2nd part of system data.
         */
        uint32_t sys_data_part2_1:32;
    };
    uint32_t val;
} efuse_rd_sys_part2_data1_reg_t;

/** Type of rd_sys_part2_data2 register
 *  Register $n of BLOCK10 (system).
 */
typedef union {
    struct {
        /** sys_data_part2_2 : RO; bitpos: [31:0]; default: 0;
         *  Stores the $nth 32 bits of the 2nd part of system data.
         */
        uint32_t sys_data_part2_2:32;
    };
    uint32_t val;
} efuse_rd_sys_part2_data2_reg_t;

/** Type of rd_sys_part2_data3 register
 *  Register $n of BLOCK10 (system).
 */
typedef union {
    struct {
        /** sys_data_part2_3 : RO; bitpos: [31:0]; default: 0;
         *  Stores the $nth 32 bits of the 2nd part of system data.
         */
        uint32_t sys_data_part2_3:32;
    };
    uint32_t val;
} efuse_rd_sys_part2_data3_reg_t;

/** Type of rd_sys_part2_data4 register
 *  Register $n of BLOCK10 (system).
 */
typedef union {
    struct {
        /** sys_data_part2_4 : RO; bitpos: [31:0]; default: 0;
         *  Stores the $nth 32 bits of the 2nd part of system data.
         */
        uint32_t sys_data_part2_4:32;
    };
    uint32_t val;
} efuse_rd_sys_part2_data4_reg_t;

/** Type of rd_sys_part2_data5 register
 *  Register $n of BLOCK10 (system).
 */
typedef union {
    struct {
        /** sys_data_part2_5 : RO; bitpos: [31:0]; default: 0;
         *  Stores the $nth 32 bits of the 2nd part of system data.
         */
        uint32_t sys_data_part2_5:32;
    };
    uint32_t val;
} efuse_rd_sys_part2_data5_reg_t;

/** Type of rd_sys_part2_data6 register
 *  Register $n of BLOCK10 (system).
 */
typedef union {
    struct {
        /** sys_data_part2_6 : RO; bitpos: [31:0]; default: 0;
         *  Stores the $nth 32 bits of the 2nd part of system data.
         */
        uint32_t sys_data_part2_6:32;
    };
    uint32_t val;
} efuse_rd_sys_part2_data6_reg_t;

/** Type of rd_sys_part2_data7 register
 *  Register $n of BLOCK10 (system).
 */
typedef union {
    struct {
        /** sys_data_part2_7 : RO; bitpos: [31:0]; default: 0;
         *  Stores the $nth 32 bits of the 2nd part of system data.
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
         *  Indicates a programming error of RD_DIS.
         */
        uint32_t rd_dis_err:7;
        /** swap_uart_sdio_en_err : RO; bitpos: [7]; default: 0;
         *  Indicates a programming error of SWAP_UART_SDIO_EN.
         */
        uint32_t swap_uart_sdio_en_err:1;
        /** dis_icache_err : RO; bitpos: [8]; default: 0;
         *  Indicates a programming error of DIS_ICACHE.
         */
        uint32_t dis_icache_err:1;
        /** dis_usb_jtag_err : RO; bitpos: [9]; default: 0;
         *  Indicates a programming error of DIS_USB_JTAG.
         */
        uint32_t dis_usb_jtag_err:1;
        /** dis_download_icache_err : RO; bitpos: [10]; default: 0;
         *  Indicates a programming error of DIS_DOWNLOAD_ICACHE.
         */
        uint32_t dis_download_icache_err:1;
        /** dis_usb_serial_jtag_err : RO; bitpos: [11]; default: 0;
         *  Indicates a programming error of DIS_USB_DEVICE.
         */
        uint32_t dis_usb_serial_jtag_err:1;
        /** dis_force_download_err : RO; bitpos: [12]; default: 0;
         *  Indicates a programming error of DIS_FORCE_DOWNLOAD.
         */
        uint32_t dis_force_download_err:1;
        /** spi_download_mspi_dis_err : RO; bitpos: [13]; default: 0;
         *  Indicates a programming error of SPI_DOWNLOAD_MSPI_DIS.
         */
        uint32_t spi_download_mspi_dis_err:1;
        /** dis_twai_err : RO; bitpos: [14]; default: 0;
         *  Indicates a programming error of DIS_CAN.
         */
        uint32_t dis_twai_err:1;
        /** jtag_sel_enable_err : RO; bitpos: [15]; default: 0;
         *  Indicates a programming error of JTAG_SEL_ENABLE.
         */
        uint32_t jtag_sel_enable_err:1;
        /** soft_dis_jtag_err : RO; bitpos: [18:16]; default: 0;
         *  Indicates a programming error of SOFT_DIS_JTAG.
         */
        uint32_t soft_dis_jtag_err:3;
        /** dis_pad_jtag_err : RO; bitpos: [19]; default: 0;
         *  Indicates a programming error of DIS_PAD_JTAG.
         */
        uint32_t dis_pad_jtag_err:1;
        /** dis_download_manual_encrypt_err : RO; bitpos: [20]; default: 0;
         *  Indicates a programming error of DIS_DOWNLOAD_MANUAL_ENCRYPT.
         */
        uint32_t dis_download_manual_encrypt_err:1;
        /** usb_drefh_err : RO; bitpos: [22:21]; default: 0;
         *  Indicates a programming error of USB_DREFH.
         */
        uint32_t usb_drefh_err:2;
        /** usb_drefl_err : RO; bitpos: [24:23]; default: 0;
         *  Indicates a programming error of USB_DREFL.
         */
        uint32_t usb_drefl_err:2;
        /** usb_exchg_pins_err : RO; bitpos: [25]; default: 0;
         *  Indicates a programming error of USB_EXCHG_PINS.
         */
        uint32_t usb_exchg_pins_err:1;
        /** vdd_spi_as_gpio_err : RO; bitpos: [26]; default: 0;
         *  Indicates a programming error of VDD_SPI_AS_GPIO.
         */
        uint32_t vdd_spi_as_gpio_err:1;
        /** rpt4_reserved0_err_2 : RO; bitpos: [28:27]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved0_err_2:2;
        /** rpt4_reserved0_err_1 : RO; bitpos: [29]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved0_err_1:1;
        /** rpt4_reserved0_err_0 : RO; bitpos: [31:30]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved0_err_0:2;
    };
    uint32_t val;
} efuse_rd_repeat_err0_reg_t;

/** Type of rd_repeat_err1 register
 *  Programming error record register 1 of BLOCK0.
 */
typedef union {
    struct {
        /** rpt4_reserved1_err_0 : RO; bitpos: [15:0]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved1_err_0:16;
        /** wdt_delay_sel_err : RO; bitpos: [17:16]; default: 0;
         *  Indicates a programming error of WDT_DELAY_SEL.
         */
        uint32_t wdt_delay_sel_err:2;
        /** spi_boot_crypt_cnt_err : RO; bitpos: [20:18]; default: 0;
         *  Indicates a programming error of SPI_BOOT_CRYPT_CNT.
         */
        uint32_t spi_boot_crypt_cnt_err:3;
        /** secure_boot_key_revoke0_err : RO; bitpos: [21]; default: 0;
         *  Indicates a programming error of SECURE_BOOT_KEY_REVOKE0.
         */
        uint32_t secure_boot_key_revoke0_err:1;
        /** secure_boot_key_revoke1_err : RO; bitpos: [22]; default: 0;
         *  Indicates a programming error of SECURE_BOOT_KEY_REVOKE1.
         */
        uint32_t secure_boot_key_revoke1_err:1;
        /** secure_boot_key_revoke2_err : RO; bitpos: [23]; default: 0;
         *  Indicates a programming error of SECURE_BOOT_KEY_REVOKE2.
         */
        uint32_t secure_boot_key_revoke2_err:1;
        /** key_purpose_0_err : RO; bitpos: [27:24]; default: 0;
         *  Indicates a programming error of KEY_PURPOSE_0.
         */
        uint32_t key_purpose_0_err:4;
        /** key_purpose_1_err : RO; bitpos: [31:28]; default: 0;
         *  Indicates a programming error of KEY_PURPOSE_1.
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
         *  Indicates a programming error of KEY_PURPOSE_2.
         */
        uint32_t key_purpose_2_err:4;
        /** key_purpose_3_err : RO; bitpos: [7:4]; default: 0;
         *  Indicates a programming error of KEY_PURPOSE_3.
         */
        uint32_t key_purpose_3_err:4;
        /** key_purpose_4_err : RO; bitpos: [11:8]; default: 0;
         *  Indicates a programming error of KEY_PURPOSE_4.
         */
        uint32_t key_purpose_4_err:4;
        /** key_purpose_5_err : RO; bitpos: [15:12]; default: 0;
         *  Indicates a programming error of KEY_PURPOSE_5.
         */
        uint32_t key_purpose_5_err:4;
        /** sec_dpa_level_err : RO; bitpos: [17:16]; default: 0;
         *  Indicates a programming error of SEC_DPA_LEVEL.
         */
        uint32_t sec_dpa_level_err:2;
        /** rpt4_reserved2_err_1 : RO; bitpos: [18]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved2_err_1:1;
        /** crypt_dpa_enable_err : RO; bitpos: [19]; default: 0;
         *  Indicates a programming error of CRYPT_DPA_ENABLE.
         */
        uint32_t crypt_dpa_enable_err:1;
        /** secure_boot_en_err : RO; bitpos: [20]; default: 0;
         *  Indicates a programming error of SECURE_BOOT_EN.
         */
        uint32_t secure_boot_en_err:1;
        /** secure_boot_aggressive_revoke_err : RO; bitpos: [21]; default: 0;
         *  Indicates a programming error of SECURE_BOOT_AGGRESSIVE_REVOKE.
         */
        uint32_t secure_boot_aggressive_revoke_err:1;
        /** rpt4_reserved2_err_0 : RO; bitpos: [27:22]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved2_err_0:6;
        /** flash_tpuw_err : RO; bitpos: [31:28]; default: 0;
         *  Indicates a programming error of FLASH_TPUW.
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
         *  Indicates a programming error of DIS_DOWNLOAD_MODE.
         */
        uint32_t dis_download_mode_err:1;
        /** dis_direct_boot_err : RO; bitpos: [1]; default: 0;
         *  Indicates a programming error of DIS_DIRECT_BOOT.
         */
        uint32_t dis_direct_boot_err:1;
        /** usb_print_err : RO; bitpos: [2]; default: 0;
         *  Indicates a programming error of UART_PRINT_CHANNEL.
         */
        uint32_t usb_print_err:1;
        /** rpt4_reserved3_err_5 : RO; bitpos: [3]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved3_err_5:1;
        /** dis_usb_serial_jtag_download_mode_err : RO; bitpos: [4]; default: 0;
         *  Indicates a programming error of DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE.
         */
        uint32_t dis_usb_serial_jtag_download_mode_err:1;
        /** enable_security_download_err : RO; bitpos: [5]; default: 0;
         *  Indicates a programming error of ENABLE_SECURITY_DOWNLOAD.
         */
        uint32_t enable_security_download_err:1;
        /** uart_print_control_err : RO; bitpos: [7:6]; default: 0;
         *  Indicates a programming error of UART_PRINT_CONTROL.
         */
        uint32_t uart_print_control_err:2;
        /** rpt4_reserved3_err_4 : RO; bitpos: [8]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved3_err_4:1;
        /** rpt4_reserved3_err_3 : RO; bitpos: [9]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved3_err_3:1;
        /** rpt4_reserved3_err_2 : RO; bitpos: [11:10]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved3_err_2:2;
        /** rpt4_reserved3_err_1 : RO; bitpos: [12]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved3_err_1:1;
        /** force_send_resume_err : RO; bitpos: [13]; default: 0;
         *  Indicates a programming error of FORCE_SEND_RESUME.
         */
        uint32_t force_send_resume_err:1;
        /** secure_version_err : RO; bitpos: [29:14]; default: 0;
         *  Indicates a programming error of SECURE_VERSION.
         */
        uint32_t secure_version_err:16;
        /** rpt4_reserved3_err_0 : RO; bitpos: [31:30]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved3_err_0:2;
    };
    uint32_t val;
} efuse_rd_repeat_err3_reg_t;

/** Type of rd_repeat_err4 register
 *  Programming error record register 4 of BLOCK0.
 */
typedef union {
    struct {
        /** rpt4_reserved4_err_1 : RO; bitpos: [23:0]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved4_err_1:24;
        /** rpt4_reserved4_err_0 : RO; bitpos: [31:24]; default: 0;
         *  Reserved.
         */
        uint32_t rpt4_reserved4_err_0:8;
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
        /** mac_spi_8m_fail : RO; bitpos: [3]; default: 0;
         *  0: Means no failure and that the data of MAC_SPI_8M is reliable 1: Means that
         *  programming user data failed and the number of error bytes is over 6.
         */
        uint32_t mac_spi_8m_fail:1;
        /** sys_part1_num : RO; bitpos: [6:4]; default: 0;
         *  The value of this signal means the number of error bytes.
         */
        uint32_t sys_part1_num:3;
        /** sys_part1_fail : RO; bitpos: [7]; default: 0;
         *  0: Means no failure and that the data of system part1 is reliable 1: Means that
         *  programming user data failed and the number of error bytes is over 6.
         */
        uint32_t sys_part1_fail:1;
        /** usr_data_err_num : RO; bitpos: [10:8]; default: 0;
         *  The value of this signal means the number of error bytes.
         */
        uint32_t usr_data_err_num:3;
        /** usr_data_fail : RO; bitpos: [11]; default: 0;
         *  0: Means no failure and that the user data is reliable 1: Means that programming
         *  user data failed and the number of error bytes is over 6.
         */
        uint32_t usr_data_fail:1;
        /** key0_err_num : RO; bitpos: [14:12]; default: 0;
         *  The value of this signal means the number of error bytes.
         */
        uint32_t key0_err_num:3;
        /** key0_fail : RO; bitpos: [15]; default: 0;
         *  0: Means no failure and that the data of key0 is reliable 1: Means that programming
         *  key0 failed and the number of error bytes is over 6.
         */
        uint32_t key0_fail:1;
        /** key1_err_num : RO; bitpos: [18:16]; default: 0;
         *  The value of this signal means the number of error bytes.
         */
        uint32_t key1_err_num:3;
        /** key1_fail : RO; bitpos: [19]; default: 0;
         *  0: Means no failure and that the data of key1 is reliable 1: Means that programming
         *  key1 failed and the number of error bytes is over 6.
         */
        uint32_t key1_fail:1;
        /** key2_err_num : RO; bitpos: [22:20]; default: 0;
         *  The value of this signal means the number of error bytes.
         */
        uint32_t key2_err_num:3;
        /** key2_fail : RO; bitpos: [23]; default: 0;
         *  0: Means no failure and that the data of key2 is reliable 1: Means that programming
         *  key2 failed and the number of error bytes is over 6.
         */
        uint32_t key2_fail:1;
        /** key3_err_num : RO; bitpos: [26:24]; default: 0;
         *  The value of this signal means the number of error bytes.
         */
        uint32_t key3_err_num:3;
        /** key3_fail : RO; bitpos: [27]; default: 0;
         *  0: Means no failure and that the data of key3 is reliable 1: Means that programming
         *  key3 failed and the number of error bytes is over 6.
         */
        uint32_t key3_fail:1;
        /** key4_err_num : RO; bitpos: [30:28]; default: 0;
         *  The value of this signal means the number of error bytes.
         */
        uint32_t key4_err_num:3;
        /** key4_fail : RO; bitpos: [31]; default: 0;
         *  0: Means no failure and that the data of key4 is reliable 1: Means that programming
         *  key4 failed and the number of error bytes is over 6.
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
         *  The value of this signal means the number of error bytes.
         */
        uint32_t key5_err_num:3;
        /** key5_fail : RO; bitpos: [3]; default: 0;
         *  0: Means no failure and that the data of key5 is reliable 1: Means that programming
         *  key5 failed and the number of error bytes is over 6.
         */
        uint32_t key5_fail:1;
        /** sys_part2_err_num : RO; bitpos: [6:4]; default: 0;
         *  The value of this signal means the number of error bytes.
         */
        uint32_t sys_part2_err_num:3;
        /** sys_part2_fail : RO; bitpos: [7]; default: 0;
         *  0: Means no failure and that the data of system part2 is reliable 1: Means that
         *  programming user data failed and the number of error bytes is over 6.
         */
        uint32_t sys_part2_fail:1;
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
        /** mem_force_pd : R/W; bitpos: [0]; default: 0;
         *  Set this bit to force eFuse SRAM into power-saving mode.
         */
        uint32_t mem_force_pd:1;
        /** mem_clk_force_on : R/W; bitpos: [1]; default: 1;
         *  Set this bit and force to activate clock signal of eFuse SRAM.
         */
        uint32_t mem_clk_force_on:1;
        /** mem_force_pu : R/W; bitpos: [2]; default: 0;
         *  Set this bit to force eFuse SRAM into working mode.
         */
        uint32_t mem_force_pu:1;
        uint32_t reserved_3:13;
        /** clk_en : R/W; bitpos: [16]; default: 0;
         *  Set this bit to force enable eFuse register configuration clock signal.
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
         *  0x5A5A:  programming operation command 0x5AA5: read operation command.
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
        /** thr_a : R/W; bitpos: [7:0]; default: 1;
         *  Configures the read hold time.
         */
        uint32_t thr_a:8;
        /** trd : R/W; bitpos: [15:8]; default: 2;
         *  Configures the read time.
         */
        uint32_t trd:8;
        /** tsur_a : R/W; bitpos: [23:16]; default: 1;
         *  Configures the read setup time.
         */
        uint32_t tsur_a:8;
        /** read_init_num : R/W; bitpos: [31:24]; default: 18;
         *  Configures the waiting time of reading eFuse memory.
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
        /** tsup_a : R/W; bitpos: [7:0]; default: 1;
         *  Configures the programming setup time.
         */
        uint32_t tsup_a:8;
        /** pwr_on_num : R/W; bitpos: [23:8]; default: 12288;
         *  Configures the power up time for VDDQ.
         */
        uint32_t pwr_on_num:16;
        /** thp_a : R/W; bitpos: [31:24]; default: 1;
         *  Configures the programming hold time.
         */
        uint32_t thp_a:8;
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
        /** tpgm : R/W; bitpos: [31:16]; default: 200;
         *  Configures the active programming time.
         */
        uint32_t tpgm:16;
    };
    uint32_t val;
} efuse_wr_tim_conf2_reg_t;

/** Type of wr_tim_conf0_rs_bypass register
 *  Configurarion register0 of eFuse programming time parameters and rs bypass
 *  operation.
 */
typedef union {
    struct {
        /** bypass_rs_correction : R/W; bitpos: [0]; default: 0;
         *  Set this bit to bypass reed solomon correction step.
         */
        uint32_t bypass_rs_correction:1;
        /** bypass_rs_blk_num : R/W; bitpos: [11:1]; default: 0;
         *  Configures block number of programming twice operation.
         */
        uint32_t bypass_rs_blk_num:11;
        /** update : WT; bitpos: [12]; default: 0;
         *  Set this bit to update multi-bit register signals.
         */
        uint32_t update:1;
        /** tpgm_inactive : R/W; bitpos: [20:13]; default: 1;
         *  Configures the inactive programming time.
         */
        uint32_t tpgm_inactive:8;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} efuse_wr_tim_conf0_rs_bypass_reg_t;


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
        /** blk0_valid_bit_cnt : RO; bitpos: [19:10]; default: 0;
         *  Indicates the number of block valid bit.
         */
        uint32_t blk0_valid_bit_cnt:10;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} efuse_status_reg_t;


/** Group: Interrupt Register */
/** Type of int_raw register
 *  eFuse raw interrupt register.
 */
typedef union {
    struct {
        /** read_done_int_raw : R/SS/WTC; bitpos: [0]; default: 0;
         *  The raw bit signal for read_done interrupt.
         */
        uint32_t read_done_int_raw:1;
        /** pgm_done_int_raw : R/SS/WTC; bitpos: [1]; default: 0;
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
        /** date : R/W; bitpos: [27:0]; default: 35676928;
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
    volatile efuse_wr_tim_conf0_rs_bypass_reg_t wr_tim_conf0_rs_bypass;
    volatile efuse_date_reg_t date;
} efuse_dev_t;

extern efuse_dev_t EFUSE;

#ifndef __cplusplus
_Static_assert(sizeof(efuse_dev_t) == 0x200, "Invalid size of efuse_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
