/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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


/** Group: ******** Registers */
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
        /** usb_device_exchg_pins : RO; bitpos: [7]; default: 0;
         *  Enable usb device exchange pins of D+ and D-.
         */
        uint32_t usb_device_exchg_pins:1;
        /** usb_otg11_exchg_pins : RO; bitpos: [8]; default: 0;
         *  Enable usb otg11 exchange pins of D+ and D-.
         */
        uint32_t usb_otg11_exchg_pins:1;
        /** dis_usb_jtag : RO; bitpos: [9]; default: 0;
         *  Represents whether the function of usb switch to jtag is disabled or enabled. 1:
         *  disabled. 0: enabled.
         */
        uint32_t dis_usb_jtag:1;
        /** powerglitch_en : RO; bitpos: [10]; default: 0;
         *  Represents whether power glitch function is enabled. 1: enabled. 0: disabled.
         */
        uint32_t powerglitch_en:1;
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
         *  Set this bit to disable accessing MSPI flash/MSPI ram by SYS AXI matrix during
         *  boot_mode_download.
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
        /** usb_device_drefh : RO; bitpos: [22:21]; default: 0;
         *  USB intphy of usb device signle-end input high threshold, 1.76V to 2V. Step by 80mV
         */
        uint32_t usb_device_drefh:2;
        /** usb_otg11_drefh : RO; bitpos: [24:23]; default: 0;
         *  USB intphy of usb otg11 signle-end input high threshold, 1.76V to 2V. Step by 80mV
         */
        uint32_t usb_otg11_drefh:2;
        /** usb_phy_sel : RO; bitpos: [25]; default: 0;
         *  TBD
         */
        uint32_t usb_phy_sel:1;
        /** km_huk_gen_state_low : RO; bitpos: [31:26]; default: 0;
         *  Set this bit to control validation of HUK generate mode. Odd of 1 is invalid, even
         *  of 1 is valid.
         */
        uint32_t km_huk_gen_state_low:6;
    };
    uint32_t val;
} efuse_rd_repeat_data0_reg_t;

/** Type of rd_repeat_data1 register
 *  BLOCK0 data register 2.
 */
typedef union {
    struct {
        /** km_huk_gen_state_high : RO; bitpos: [2:0]; default: 0;
         *  Set this bit to control validation of HUK generate mode. Odd of 1 is invalid, even
         *  of 1 is valid.
         */
        uint32_t km_huk_gen_state_high:3;
        /** km_rnd_switch_cycle : RO; bitpos: [4:3]; default: 0;
         *  Set bits to control key manager random number switch cycle. 0: control by register.
         *  1: 8 km clk cycles. 2: 16 km cycles. 3: 32 km cycles.
         */
        uint32_t km_rnd_switch_cycle:2;
        /** km_deploy_only_once : RO; bitpos: [8:5]; default: 0;
         *  Set each bit to control whether corresponding key can only be deployed once. 1 is
         *  true, 0 is false. Bit0: ecdsa. Bit1: xts. Bit2: hmac. Bit3: ds.
         */
        uint32_t km_deploy_only_once:4;
        /** force_use_key_manager_key : RO; bitpos: [12:9]; default: 0;
         *  Set each bit to control whether corresponding key must come from key manager.. 1 is
         *  true, 0 is false. Bit0: ecdsa. Bit1: xts. Bit2: hmac. Bit3: ds.
         */
        uint32_t force_use_key_manager_key:4;
        /** force_disable_sw_init_key : RO; bitpos: [13]; default: 0;
         *  Set this bit to disable software written init key, and force use efuse_init_key.
         */
        uint32_t force_disable_sw_init_key:1;
        /** xts_key_length_256 : RO; bitpos: [14]; default: 0;
         *  Set this bit to configure flash encryption use xts-128 key, else use xts-256 key.
         */
        uint32_t xts_key_length_256:1;
        /** rd_reserve_0_79 : RW; bitpos: [15]; default: 0;
         *  Reserved, it was created by set_missed_fields_in_regs func
         */
        uint32_t rd_reserve_0_79:1;
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
        /** ecdsa_enable_soft_k : RO; bitpos: [18]; default: 0;
         *  Represents whether hardware random number k is forced used in ESDCA. 1: force used.
         *  0: not force used.
         */
        uint32_t ecdsa_enable_soft_k:1;
        /** crypt_dpa_enable : RO; bitpos: [19]; default: 1;
         *  Represents whether anti-dpa attack is enabled. 1:enabled. 0: disabled.
         */
        uint32_t crypt_dpa_enable:1;
        /** secure_boot_en : RO; bitpos: [20]; default: 0;
         *  Represents whether secure boot is enabled or disabled. 1: enabled. 0: disabled.
         */
        uint32_t secure_boot_en:1;
        /** secure_boot_aggressive_revoke : RO; bitpos: [21]; default: 0;
         *  Represents whether revoking aggressive secure boot is enabled or disabled. 1:
         *  enabled. 0: disabled.
         */
        uint32_t secure_boot_aggressive_revoke:1;
        /** rd_reserve_0_118 : RW; bitpos: [22]; default: 0;
         *  Reserved, it was created by set_missed_fields_in_regs func
         */
        uint32_t rd_reserve_0_118:1;
        /** flash_type : RO; bitpos: [23]; default: 0;
         *  The type of interfaced flash. 0: four data lines, 1: eight data lines.
         */
        uint32_t flash_type:1;
        /** flash_page_size : RO; bitpos: [25:24]; default: 0;
         *  Set flash page size.
         */
        uint32_t flash_page_size:2;
        /** flash_ecc_en : RO; bitpos: [26]; default: 0;
         *  Set this bit to enable ecc for flash boot.
         */
        uint32_t flash_ecc_en:1;
        /** dis_usb_otg_download_mode : RO; bitpos: [27]; default: 0;
         *  Set this bit to disable download via USB-OTG.
         */
        uint32_t dis_usb_otg_download_mode:1;
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
        /** lock_km_key : RO; bitpos: [3]; default: 0;
         *  TBD
         */
        uint32_t lock_km_key:1;
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
        /** force_send_resume : RO; bitpos: [8]; default: 0;
         *  Represents whether ROM code is forced to send a resume command during SPI boot. 1:
         *  forced. 0:not forced.
         */
        uint32_t force_send_resume:1;
        /** secure_version : RO; bitpos: [24:9]; default: 0;
         *  Represents the version used by ESP-IDF anti-rollback feature.
         */
        uint32_t secure_version:16;
        /** secure_boot_disable_fast_wake : RO; bitpos: [25]; default: 0;
         *  Represents whether FAST VERIFY ON WAKE is disabled or enabled when Secure Boot is
         *  enabled. 1: disabled. 0: enabled.
         */
        uint32_t secure_boot_disable_fast_wake:1;
        /** hys_en_pad : RO; bitpos: [26]; default: 0;
         *  Represents whether the hysteresis function of corresponding PAD is enabled. 1:
         *  enabled. 0:disabled.
         */
        uint32_t hys_en_pad:1;
        /** dcdc_vset : RO; bitpos: [31:27]; default: 0;
         *  Set the dcdc voltage default.
         */
        uint32_t dcdc_vset:5;
    };
    uint32_t val;
} efuse_rd_repeat_data3_reg_t;

/** Type of rd_repeat_data4 register
 *  BLOCK0 data register 5.
 */
typedef union {
    struct {
        /** pxa0_tieh_sel_0 : RO; bitpos: [1:0]; default: 0;
         *  TBD
         */
        uint32_t pxa0_tieh_sel_0:2;
        /** pxa0_tieh_sel_1 : RO; bitpos: [3:2]; default: 0;
         *  TBD.
         */
        uint32_t pxa0_tieh_sel_1:2;
        /** pxa0_tieh_sel_2 : RO; bitpos: [5:4]; default: 0;
         *  TBD.
         */
        uint32_t pxa0_tieh_sel_2:2;
        /** pxa0_tieh_sel_3 : RO; bitpos: [7:6]; default: 0;
         *  TBD.
         */
        uint32_t pxa0_tieh_sel_3:2;
        /** km_disable_deploy_mode : RO; bitpos: [11:8]; default: 0;
         *  TBD.
         */
        uint32_t km_disable_deploy_mode:4;
        /** usb_device_drefl : RO; bitpos: [13:12]; default: 0;
         *  Represents the usb device single-end input low threhold, 0.8 V to 1.04 V with step
         *  of 80 mV.
         */
        uint32_t usb_device_drefl:2;
        /** usb_otg11_drefl : RO; bitpos: [15:14]; default: 0;
         *  Represents the usb otg11 single-end input low threhold, 0.8 V to 1.04 V with step
         *  of 80 mV.
         */
        uint32_t usb_otg11_drefl:2;
        /** rd_reserve_0_176 : RW; bitpos: [17:16]; default: 0;
         *  Reserved, it was created by set_missed_fields_in_regs func
         */
        uint32_t rd_reserve_0_176:2;
        /** hp_pwr_src_sel : RO; bitpos: [18]; default: 0;
         *  HP system power source select. 0:LDO. 1: DCDC.
         */
        uint32_t hp_pwr_src_sel:1;
        /** dcdc_vset_en : RO; bitpos: [19]; default: 0;
         *  Select dcdc vset use efuse_dcdc_vset.
         */
        uint32_t dcdc_vset_en:1;
        /** dis_wdt : RO; bitpos: [20]; default: 0;
         *  Set this bit to disable watch dog.
         */
        uint32_t dis_wdt:1;
        /** dis_swd : RO; bitpos: [21]; default: 0;
         *  Set this bit to disable super-watchdog.
         */
        uint32_t dis_swd:1;
        /** rd_reserve_0_182 : RW; bitpos: [31:22]; default: 0;
         *  Reserved, it was created by set_missed_fields_in_regs func
         */
        uint32_t rd_reserve_0_182:10;
    };
    uint32_t val;
} efuse_rd_repeat_data4_reg_t;

/** Type of rd_mac_sys_0 register
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
} efuse_rd_mac_sys_0_reg_t;

/** Type of rd_mac_sys_1 register
 *  BLOCK1 data register $n.
 */
typedef union {
    struct {
        /** mac_1 : RO; bitpos: [15:0]; default: 0;
         *  Stores the high 16 bits of MAC address.
         */
        uint32_t mac_1:16;
        /** reserved_1_16 : RO; bitpos: [31:16]; default: 0;
         *  Stores the extended bits of MAC address.
         */
        uint32_t reserved_1_16:16;
    };
    uint32_t val;
} efuse_rd_mac_sys_1_reg_t;

/** Type of rd_mac_sys_2 register
 *  BLOCK1 data register $n.
 */
typedef union {
    struct {
        /** wafer_version_minor : R; bitpos: [3:0]; default: 0;
         *  Minor chip version
         */
        uint32_t wafer_version_minor:4;
        /** wafer_version_major : R; bitpos: [5:4]; default: 0;
         *  Major chip version
         */
        uint32_t wafer_version_major:2;
        /** disable_wafer_version_major : R; bitpos: [6]; default: 0;
         *  Disables check of wafer version major
         */
        uint32_t disable_wafer_version_major:1;
        /** disable_blk_version_major : R; bitpos: [7]; default: 0;
         *  Disables check of blk version major
         */
        uint32_t disable_blk_version_major:1;
        /** blk_version_minor : R; bitpos: [10:8]; default: 0;
         *  BLK_VERSION_MINOR of BLOCK2
         */
        uint32_t blk_version_minor:3;
        /** blk_version_major : R; bitpos: [12:11]; default: 0;
         *  BLK_VERSION_MAJOR of BLOCK2
         */
        uint32_t blk_version_major:2;
        /** flash_cap : R; bitpos: [15:13]; default: 0;
         *  Flash capacity
         */
        uint32_t flash_cap:3;
        /** flash_temp : R; bitpos: [17:16]; default: 0;
         *  Flash temperature
         */
        uint32_t flash_temp:2;
        /** flash_vendor : R; bitpos: [20:18]; default: 0;
         *  Flash vendor
         */
        uint32_t flash_vendor:3;
        /** psram_cap : R; bitpos: [22:21]; default: 0;
         *  PSRAM capacity
         */
        uint32_t psram_cap:2;
        /** psram_temp : R; bitpos: [24:23]; default: 0;
         *  PSRAM temperature
         */
        uint32_t psram_temp:2;
        /** psram_vendor : R; bitpos: [26:25]; default: 0;
         *  PSRAM vendor
         */
        uint32_t psram_vendor:2;
        /** pkg_version : R; bitpos: [29:27]; default: 0;
         *  Package version
         */
        uint32_t pkg_version:3;
        /** reserved_1_94 : R; bitpos: [31:30]; default: 0;
         *  reserved
         */
        uint32_t reserved_1_94:2;
    };
    uint32_t val;
} efuse_rd_mac_sys_2_reg_t;

/** Type of rd_mac_sys_3 register
 *  BLOCK1 data register $n.
 */
typedef union {
    struct {
        /** mac_reserved_2 : RO; bitpos: [17:0]; default: 0;
         *  Reserved.
         */
        uint32_t mac_reserved_2:18;
        /** sys_data_part0_0 : RO; bitpos: [31:18]; default: 0;
         *  Stores the first 14 bits of the zeroth part of system data.
         */
        uint32_t sys_data_part0_0:14;
    };
    uint32_t val;
} efuse_rd_mac_sys_3_reg_t;

/** Type of rd_mac_sys_4 register
 *  BLOCK1 data register $n.
 */
typedef union {
    struct {
        /** sys_data_part0_1 : RO; bitpos: [31:0]; default: 0;
         *  Stores the first 32 bits of the zeroth part of system data.
         */
        uint32_t sys_data_part0_1:32;
    };
    uint32_t val;
} efuse_rd_mac_sys_4_reg_t;

/** Type of rd_mac_sys_5 register
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
} efuse_rd_mac_sys_5_reg_t;

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
        /** sys_data_part1_4 : RO; bitpos: [31:0]; default: 0;
         *  Stores the fourth 32 bits of the first part of system data.
         */
        uint32_t sys_data_part1_4:32;
    };
    uint32_t val;
} efuse_rd_sys_part1_data4_reg_t;

/** Type of rd_sys_part1_data5 register
 *  Register $n of BLOCK2 (system).
 */
typedef union {
    struct {
        /** sys_data_part1_5 : RO; bitpos: [31:0]; default: 0;
         *  Stores the fifth 32 bits of the first part of system data.
         */
        uint32_t sys_data_part1_5:32;
    };
    uint32_t val;
} efuse_rd_sys_part1_data5_reg_t;

/** Type of rd_sys_part1_data6 register
 *  Register $n of BLOCK2 (system).
 */
typedef union {
    struct {
        /** sys_data_part1_6 : RO; bitpos: [31:0]; default: 0;
         *  Stores the sixth 32 bits of the first part of system data.
         */
        uint32_t sys_data_part1_6:32;
    };
    uint32_t val;
} efuse_rd_sys_part1_data6_reg_t;

/** Type of rd_sys_part1_data7 register
 *  Register $n of BLOCK2 (system).
 */
typedef union {
    struct {
        /** sys_data_part1_7 : RO; bitpos: [31:0]; default: 0;
         *  Stores the seventh 32 bits of the first part of system data.
         */
        uint32_t sys_data_part1_7:32;
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

/** Type of rd_repeat_err0 register
 *  Programming error record register 0 of BLOCK0.
 */
typedef union {
    struct {
        /** rd_dis_err : RO; bitpos: [6:0]; default: 0;
         *  Indicates a programming error of RD_DIS.
         */
        uint32_t rd_dis_err:7;
        /** dis_usb_device_exchg_pins_err : RO; bitpos: [7]; default: 0;
         *  Indicates a programming error of DIS_USB_DEVICE_EXCHG_PINS.
         */
        uint32_t dis_usb_device_exchg_pins_err:1;
        /** dis_usb_otg11_exchg_pins_err : RO; bitpos: [8]; default: 0;
         *  Indicates a programming error of DIS_USB_OTG11_EXCHG_PINS.
         */
        uint32_t dis_usb_otg11_exchg_pins_err:1;
        /** dis_usb_jtag_err : RO; bitpos: [9]; default: 0;
         *  Indicates a programming error of DIS_USB_JTAG.
         */
        uint32_t dis_usb_jtag_err:1;
        /** powerglitch_en_err : RO; bitpos: [10]; default: 0;
         *  Indicates a programming error of POWERGLITCH_EN.
         */
        uint32_t powerglitch_en_err:1;
        /** dis_usb_serial_jtag_err : RO; bitpos: [11]; default: 0;
         *  Indicates a programming error of DIS_USB_SERIAL_JTAG.
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
         *  Indicates a programming error of DIS_TWAI.
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
        /** usb_device_drefh_err : RO; bitpos: [22:21]; default: 0;
         *  Indicates a programming error of USB_DEVICE_DREFH.
         */
        uint32_t usb_device_drefh_err:2;
        /** usb_otg11_drefh_err : RO; bitpos: [24:23]; default: 0;
         *  Indicates a programming error of USB_OTG11_DREFH.
         */
        uint32_t usb_otg11_drefh_err:2;
        /** usb_phy_sel_err : RO; bitpos: [25]; default: 0;
         *  Indicates a programming error of USB_PHY_SEL.
         */
        uint32_t usb_phy_sel_err:1;
        /** huk_gen_state_low_err : RO; bitpos: [31:26]; default: 0;
         *  Indicates a programming error of HUK_GEN_STATE_LOW.
         */
        uint32_t huk_gen_state_low_err:6;
    };
    uint32_t val;
} efuse_rd_repeat_err0_reg_t;

/** Type of rd_repeat_err1 register
 *  Programming error record register 1 of BLOCK0.
 */
typedef union {
    struct {
        /** km_huk_gen_state_high_err : RO; bitpos: [2:0]; default: 0;
         *  Indicates a programming error of HUK_GEN_STATE_HIGH.
         */
        uint32_t km_huk_gen_state_high_err:3;
        /** km_rnd_switch_cycle_err : RO; bitpos: [4:3]; default: 0;
         *  Indicates a programming error of KM_RND_SWITCH_CYCLE.
         */
        uint32_t km_rnd_switch_cycle_err:2;
        /** km_deploy_only_once_err : RO; bitpos: [8:5]; default: 0;
         *  Indicates a programming error of KM_DEPLOY_ONLY_ONCE.
         */
        uint32_t km_deploy_only_once_err:4;
        /** force_use_key_manager_key_err : RO; bitpos: [12:9]; default: 0;
         *  Indicates a programming error of FORCE_USE_KEY_MANAGER_KEY.
         */
        uint32_t force_use_key_manager_key_err:4;
        /** force_disable_sw_init_key_err : RO; bitpos: [13]; default: 0;
         *  Indicates a programming error of FORCE_DISABLE_SW_INIT_KEY.
         */
        uint32_t force_disable_sw_init_key_err:1;
        /** xts_key_length_256_err : RO; bitpos: [14]; default: 0;
         *  Indicates a programming error of XTS_KEY_LENGTH_256.
         */
        uint32_t xts_key_length_256_err:1;
        uint32_t reserved_15:1;
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
        /** ecdsa_enable_soft_k_err : RO; bitpos: [18]; default: 0;
         *  Indicates a programming error of ECDSA_FORCE_USE_HARDWARE_K.
         */
        uint32_t ecdsa_enable_soft_k_err:1;
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
        uint32_t reserved_22:1;
        /** flash_type_err : RO; bitpos: [23]; default: 0;
         *  Indicates a programming error of FLASH_TYPE.
         */
        uint32_t flash_type_err:1;
        /** flash_page_size_err : RO; bitpos: [25:24]; default: 0;
         *  Indicates a programming error of FLASH_PAGE_SIZE.
         */
        uint32_t flash_page_size_err:2;
        /** flash_ecc_en_err : RO; bitpos: [26]; default: 0;
         *  Indicates a programming error of FLASH_ECC_EN.
         */
        uint32_t flash_ecc_en_err:1;
        /** dis_usb_otg_download_mode_err : RO; bitpos: [27]; default: 0;
         *  Indicates a programming error of DIS_USB_OTG_DOWNLOAD_MODE.
         */
        uint32_t dis_usb_otg_download_mode_err:1;
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
        /** dis_usb_serial_jtag_rom_print_err : RO; bitpos: [2]; default: 0;
         *  Indicates a programming error of DIS_USB_SERIAL_JTAG_ROM_PRINT_ERR.
         */
        uint32_t dis_usb_serial_jtag_rom_print_err:1;
        /** lock_km_key_err : RO; bitpos: [3]; default: 0;
         *  TBD
         */
        uint32_t lock_km_key_err:1;
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
        /** force_send_resume_err : RO; bitpos: [8]; default: 0;
         *  Indicates a programming error of FORCE_SEND_RESUME.
         */
        uint32_t force_send_resume_err:1;
        /** secure_version_err : RO; bitpos: [24:9]; default: 0;
         *  Indicates a programming error of SECURE VERSION.
         */
        uint32_t secure_version_err:16;
        /** secure_boot_disable_fast_wake_err : RO; bitpos: [25]; default: 0;
         *  Indicates a programming error of SECURE_BOOT_DISABLE_FAST_WAKE.
         */
        uint32_t secure_boot_disable_fast_wake_err:1;
        /** hys_en_pad_err : RO; bitpos: [26]; default: 0;
         *  Indicates a programming error of HYS_EN_PAD.
         */
        uint32_t hys_en_pad_err:1;
        /** dcdc_vset_err : RO; bitpos: [31:27]; default: 0;
         *  Indicates a programming error of DCDC_VSET.
         */
        uint32_t dcdc_vset_err:5;
    };
    uint32_t val;
} efuse_rd_repeat_err3_reg_t;

/** Type of rd_repeat_err4 register
 *  Programming error record register 4 of BLOCK0.
 */
typedef union {
    struct {
        /** pxa0_tieh_sel_0_err : RO; bitpos: [1:0]; default: 0;
         *  Indicates a programming error of 0PXA_TIEH_SEL_0.
         */
        uint32_t pxa0_tieh_sel_0_err:2;
        /** pxa0_tieh_sel_1_err : RO; bitpos: [3:2]; default: 0;
         *  Indicates a programming error of 0PXA_TIEH_SEL_1.
         */
        uint32_t pxa0_tieh_sel_1_err:2;
        /** pxa0_tieh_sel_2_err : RO; bitpos: [5:4]; default: 0;
         *  Indicates a programming error of 0PXA_TIEH_SEL_2.
         */
        uint32_t pxa0_tieh_sel_2_err:2;
        /** pxa0_tieh_sel_3_err : RO; bitpos: [7:6]; default: 0;
         *  Indicates a programming error of 0PXA_TIEH_SEL_3.
         */
        uint32_t pxa0_tieh_sel_3_err:2;
        /** km_disable_deploy_mode_err : RO; bitpos: [11:8]; default: 0;
         *  TBD.
         */
        uint32_t km_disable_deploy_mode_err:4;
        /** usb_device_drefl_err : RO; bitpos: [13:12]; default: 0;
         *  Indicates a programming error of USB_DEVICE_DREFL.
         */
        uint32_t usb_device_drefl_err:2;
        /** usb_otg11_drefl_err : RO; bitpos: [15:14]; default: 0;
         *  Indicates a programming error of USB_OTG11_DREFL.
         */
        uint32_t usb_otg11_drefl_err:2;
        uint32_t reserved_16:2;
        /** hp_pwr_src_sel_err : RO; bitpos: [18]; default: 0;
         *  Indicates a programming error of HP_PWR_SRC_SEL.
         */
        uint32_t hp_pwr_src_sel_err:1;
        /** dcdc_vset_en_err : RO; bitpos: [19]; default: 0;
         *  Indicates a programming error of DCDC_VSET_EN.
         */
        uint32_t dcdc_vset_en_err:1;
        /** dis_wdt_err : RO; bitpos: [20]; default: 0;
         *  Indicates a programming error of DIS_WDT.
         */
        uint32_t dis_wdt_err:1;
        /** dis_swd_err : RO; bitpos: [21]; default: 0;
         *  Indicates a programming error of DIS_SWD.
         */
        uint32_t dis_swd_err:1;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} efuse_rd_repeat_err4_reg_t;

/** Type of rd_rs_err0 register
 *  Programming error record register 0 of BLOCK1-10.
 */
typedef union {
    struct {
        /** mac_sys_err_num : RO; bitpos: [2:0]; default: 0;
         *  The value of this signal means the number of error bytes.
         */
        uint32_t mac_sys_err_num:3;
        /** mac_sys_fail : RO; bitpos: [3]; default: 0;
         *  0: Means no failure and that the data of MAC_SPI_8M is reliable 1: Means that
         *  programming user data failed and the number of error bytes is over 6.
         */
        uint32_t mac_sys_fail:1;
        /** sys_part1_err_num : RO; bitpos: [6:4]; default: 0;
         *  The value of this signal means the number of error bytes.
         */
        uint32_t sys_part1_err_num:3;
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

/** Type of clk register
 *  eFuse clcok configuration register.
 */
typedef union {
    struct {
        /** mem_force_pd : R/W; bitpos: [0]; default: 0;
         *  Set this bit to force eFuse SRAM into power-saving mode.
         */
        uint32_t mem_force_pd:1;
        /** mem_clk_force_on : R/W; bitpos: [1]; default: 0;
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
        /** cfg_ecdsa_blk : R/W; bitpos: [19:16]; default: 0;
         *  Configures which block to use for ECDSA key output.
         */
        uint32_t cfg_ecdsa_blk:4;
        uint32_t reserved_20:12;
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
        /** blk0_valid_bit_cnt : RO; bitpos: [19:10]; default: 0;
         *  Indicates the number of block valid bit.
         */
        uint32_t blk0_valid_bit_cnt:10;
        /** cur_ecdsa_blk : RO; bitpos: [23:20]; default: 0;
         *  Indicates which block is used for ECDSA key output.
         */
        uint32_t cur_ecdsa_blk:4;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} efuse_status_reg_t;

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

/** Type of dac_conf register
 *  Controls the eFuse programming voltage.
 */
typedef union {
    struct {
        /** dac_clk_div : R/W; bitpos: [7:0]; default: 23;
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
        /** read_init_num : R/W; bitpos: [31:24]; default: 15;
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
        /** pwr_on_num : R/W; bitpos: [23:8]; default: 9831;
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
        /** pwr_off_num : R/W; bitpos: [15:0]; default: 320;
         *  Configures the power outage time for VDDQ.
         */
        uint32_t pwr_off_num:16;
        /** tpgm : R/W; bitpos: [31:16]; default: 160;
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


/** Group: EFUSE Version Register */
/** Type of date register
 *  eFuse version register.
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 36720720;
         *  Stores eFuse version.
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} efuse_date_reg_t;


/** Group: EFUSE_APB2OTP Block0 Write Disable Data */
/** Type of apb2otp_wr_dis register
 *  eFuse apb2otp block0 data register1.
 */
typedef union {
    struct {
        /** apb2otp_block0_wr_dis : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 write disable data.
         */
        uint32_t apb2otp_block0_wr_dis:32;
    };
    uint32_t val;
} efuse_apb2otp_wr_dis_reg_t;


/** Group: EFUSE_APB2OTP Block0 Backup1 Word1 Data */
/** Type of apb2otp_blk0_backup1_w1 register
 *  eFuse apb2otp block0 data register2.
 */
typedef union {
    struct {
        /** apb2otp_block0_backup1_w1 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup1 word1 data.
         */
        uint32_t apb2otp_block0_backup1_w1:32;
    };
    uint32_t val;
} efuse_apb2otp_blk0_backup1_w1_reg_t;


/** Group: EFUSE_APB2OTP Block0 Backup1 Word2 Data */
/** Type of apb2otp_blk0_backup1_w2 register
 *  eFuse apb2otp block0 data register3.
 */
typedef union {
    struct {
        /** apb2otp_block0_backup1_w2 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup1 word2 data.
         */
        uint32_t apb2otp_block0_backup1_w2:32;
    };
    uint32_t val;
} efuse_apb2otp_blk0_backup1_w2_reg_t;


/** Group: EFUSE_APB2OTP Block0 Backup1 Word3 Data */
/** Type of apb2otp_blk0_backup1_w3 register
 *  eFuse apb2otp block0 data register4.
 */
typedef union {
    struct {
        /** apb2otp_block0_backup1_w3 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup1 word3 data.
         */
        uint32_t apb2otp_block0_backup1_w3:32;
    };
    uint32_t val;
} efuse_apb2otp_blk0_backup1_w3_reg_t;


/** Group: EFUSE_APB2OTP Block0 Backup1 Word4 Data */
/** Type of apb2otp_blk0_backup1_w4 register
 *  eFuse apb2otp block0 data register5.
 */
typedef union {
    struct {
        /** apb2otp_block0_backup1_w4 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup1 word4 data.
         */
        uint32_t apb2otp_block0_backup1_w4:32;
    };
    uint32_t val;
} efuse_apb2otp_blk0_backup1_w4_reg_t;


/** Group: EFUSE_APB2OTP Block0 Backup1 Word5 Data */
/** Type of apb2otp_blk0_backup1_w5 register
 *  eFuse apb2otp block0 data register6.
 */
typedef union {
    struct {
        /** apb2otp_block0_backup1_w5 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup1 word5 data.
         */
        uint32_t apb2otp_block0_backup1_w5:32;
    };
    uint32_t val;
} efuse_apb2otp_blk0_backup1_w5_reg_t;


/** Group: EFUSE_APB2OTP Block0 Backup2 Word1 Data */
/** Type of apb2otp_blk0_backup2_w1 register
 *  eFuse apb2otp block0 data register7.
 */
typedef union {
    struct {
        /** apb2otp_block0_backup2_w1 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup2 word1 data.
         */
        uint32_t apb2otp_block0_backup2_w1:32;
    };
    uint32_t val;
} efuse_apb2otp_blk0_backup2_w1_reg_t;


/** Group: EFUSE_APB2OTP Block0 Backup2 Word2 Data */
/** Type of apb2otp_blk0_backup2_w2 register
 *  eFuse apb2otp block0 data register8.
 */
typedef union {
    struct {
        /** apb2otp_block0_backup2_w2 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup2 word2 data.
         */
        uint32_t apb2otp_block0_backup2_w2:32;
    };
    uint32_t val;
} efuse_apb2otp_blk0_backup2_w2_reg_t;


/** Group: EFUSE_APB2OTP Block0 Backup2 Word3 Data */
/** Type of apb2otp_blk0_backup2_w3 register
 *  eFuse apb2otp block0 data register9.
 */
typedef union {
    struct {
        /** apb2otp_block0_backup2_w3 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup2 word3 data.
         */
        uint32_t apb2otp_block0_backup2_w3:32;
    };
    uint32_t val;
} efuse_apb2otp_blk0_backup2_w3_reg_t;


/** Group: EFUSE_APB2OTP Block0 Backup2 Word4 Data */
/** Type of apb2otp_blk0_backup2_w4 register
 *  eFuse apb2otp block0 data register10.
 */
typedef union {
    struct {
        /** apb2otp_block0_backup2_w4 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup2 word4 data.
         */
        uint32_t apb2otp_block0_backup2_w4:32;
    };
    uint32_t val;
} efuse_apb2otp_blk0_backup2_w4_reg_t;


/** Group: EFUSE_APB2OTP Block0 Backup2 Word5 Data */
/** Type of apb2otp_blk0_backup2_w5 register
 *  eFuse apb2otp block0 data register11.
 */
typedef union {
    struct {
        /** apb2otp_block0_backup2_w5 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup2 word5 data.
         */
        uint32_t apb2otp_block0_backup2_w5:32;
    };
    uint32_t val;
} efuse_apb2otp_blk0_backup2_w5_reg_t;


/** Group: EFUSE_APB2OTP Block0 Backup3 Word1 Data */
/** Type of apb2otp_blk0_backup3_w1 register
 *  eFuse apb2otp block0 data register12.
 */
typedef union {
    struct {
        /** apb2otp_block0_backup3_w1 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup3 word1 data.
         */
        uint32_t apb2otp_block0_backup3_w1:32;
    };
    uint32_t val;
} efuse_apb2otp_blk0_backup3_w1_reg_t;


/** Group: EFUSE_APB2OTP Block0 Backup3 Word2 Data */
/** Type of apb2otp_blk0_backup3_w2 register
 *  eFuse apb2otp block0 data register13.
 */
typedef union {
    struct {
        /** apb2otp_block0_backup3_w2 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup3 word2 data.
         */
        uint32_t apb2otp_block0_backup3_w2:32;
    };
    uint32_t val;
} efuse_apb2otp_blk0_backup3_w2_reg_t;


/** Group: EFUSE_APB2OTP Block0 Backup3 Word3 Data */
/** Type of apb2otp_blk0_backup3_w3 register
 *  eFuse apb2otp block0 data register14.
 */
typedef union {
    struct {
        /** apb2otp_block0_backup3_w3 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup3 word3 data.
         */
        uint32_t apb2otp_block0_backup3_w3:32;
    };
    uint32_t val;
} efuse_apb2otp_blk0_backup3_w3_reg_t;


/** Group: EFUSE_APB2OTP Block0 Backup3 Word4 Data */
/** Type of apb2otp_blk0_backup3_w4 register
 *  eFuse apb2otp block0 data register15.
 */
typedef union {
    struct {
        /** apb2otp_block0_backup3_w4 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup3 word4 data.
         */
        uint32_t apb2otp_block0_backup3_w4:32;
    };
    uint32_t val;
} efuse_apb2otp_blk0_backup3_w4_reg_t;


/** Group: EFUSE_APB2OTP Block0 Backup3 Word5 Data */
/** Type of apb2otp_blk0_backup3_w5 register
 *  eFuse apb2otp block0 data register16.
 */
typedef union {
    struct {
        /** apb2otp_block0_backup3_w5 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup3 word5 data.
         */
        uint32_t apb2otp_block0_backup3_w5:32;
    };
    uint32_t val;
} efuse_apb2otp_blk0_backup3_w5_reg_t;


/** Group: EFUSE_APB2OTP Block0 Backup4 Word1 Data */
/** Type of apb2otp_blk0_backup4_w1 register
 *  eFuse apb2otp block0 data register17.
 */
typedef union {
    struct {
        /** apb2otp_block0_backup4_w1 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup4 word1 data.
         */
        uint32_t apb2otp_block0_backup4_w1:32;
    };
    uint32_t val;
} efuse_apb2otp_blk0_backup4_w1_reg_t;


/** Group: EFUSE_APB2OTP Block0 Backup4 Word2 Data */
/** Type of apb2otp_blk0_backup4_w2 register
 *  eFuse apb2otp block0 data register18.
 */
typedef union {
    struct {
        /** apb2otp_block0_backup4_w2 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup4 word2 data.
         */
        uint32_t apb2otp_block0_backup4_w2:32;
    };
    uint32_t val;
} efuse_apb2otp_blk0_backup4_w2_reg_t;


/** Group: EFUSE_APB2OTP Block0 Backup4 Word3 Data */
/** Type of apb2otp_blk0_backup4_w3 register
 *  eFuse apb2otp block0 data register19.
 */
typedef union {
    struct {
        /** apb2otp_block0_backup4_w3 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup4 word3 data.
         */
        uint32_t apb2otp_block0_backup4_w3:32;
    };
    uint32_t val;
} efuse_apb2otp_blk0_backup4_w3_reg_t;


/** Group: EFUSE_APB2OTP Block0 Backup4 Word4 Data */
/** Type of apb2otp_blk0_backup4_w4 register
 *  eFuse apb2otp block0 data register20.
 */
typedef union {
    struct {
        /** apb2otp_block0_backup4_w4 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup4 word4 data.
         */
        uint32_t apb2otp_block0_backup4_w4:32;
    };
    uint32_t val;
} efuse_apb2otp_blk0_backup4_w4_reg_t;


/** Group: EFUSE_APB2OTP Block0 Backup4 Word5 Data */
/** Type of apb2otp_blk0_backup4_w5 register
 *  eFuse apb2otp block0 data register21.
 */
typedef union {
    struct {
        /** apb2otp_block0_backup4_w5 : RO; bitpos: [31:0]; default: 0;
         *  Otp block0 backup4 word5 data.
         */
        uint32_t apb2otp_block0_backup4_w5:32;
    };
    uint32_t val;
} efuse_apb2otp_blk0_backup4_w5_reg_t;


/** Group: EFUSE_APB2OTP Block1 Word1 Data */
/** Type of apb2otp_blk1_w1 register
 *  eFuse apb2otp block1 data register1.
 */
typedef union {
    struct {
        /** apb2otp_block1_w1 : RO; bitpos: [31:0]; default: 0;
         *  Otp block1  word1 data.
         */
        uint32_t apb2otp_block1_w1:32;
    };
    uint32_t val;
} efuse_apb2otp_blk1_w1_reg_t;


/** Group: EFUSE_APB2OTP Block1 Word2 Data */
/** Type of apb2otp_blk1_w2 register
 *  eFuse apb2otp block1 data register2.
 */
typedef union {
    struct {
        /** apb2otp_block1_w2 : RO; bitpos: [31:0]; default: 0;
         *  Otp block1  word2 data.
         */
        uint32_t apb2otp_block1_w2:32;
    };
    uint32_t val;
} efuse_apb2otp_blk1_w2_reg_t;


/** Group: EFUSE_APB2OTP Block1 Word3 Data */
/** Type of apb2otp_blk1_w3 register
 *  eFuse apb2otp block1 data register3.
 */
typedef union {
    struct {
        /** apb2otp_block1_w3 : RO; bitpos: [31:0]; default: 0;
         *  Otp block1  word3 data.
         */
        uint32_t apb2otp_block1_w3:32;
    };
    uint32_t val;
} efuse_apb2otp_blk1_w3_reg_t;


/** Group: EFUSE_APB2OTP Block1 Word4 Data */
/** Type of apb2otp_blk1_w4 register
 *  eFuse apb2otp block1 data register4.
 */
typedef union {
    struct {
        /** apb2otp_block1_w4 : RO; bitpos: [31:0]; default: 0;
         *  Otp block1  word4 data.
         */
        uint32_t apb2otp_block1_w4:32;
    };
    uint32_t val;
} efuse_apb2otp_blk1_w4_reg_t;


/** Group: EFUSE_APB2OTP Block1 Word5 Data */
/** Type of apb2otp_blk1_w5 register
 *  eFuse apb2otp block1 data register5.
 */
typedef union {
    struct {
        /** apb2otp_block1_w5 : RO; bitpos: [31:0]; default: 0;
         *  Otp block1  word5 data.
         */
        uint32_t apb2otp_block1_w5:32;
    };
    uint32_t val;
} efuse_apb2otp_blk1_w5_reg_t;


/** Group: EFUSE_APB2OTP Block1 Word6 Data */
/** Type of apb2otp_blk1_w6 register
 *  eFuse apb2otp block1 data register6.
 */
typedef union {
    struct {
        /** apb2otp_block1_w6 : RO; bitpos: [31:0]; default: 0;
         *  Otp block1  word6 data.
         */
        uint32_t apb2otp_block1_w6:32;
    };
    uint32_t val;
} efuse_apb2otp_blk1_w6_reg_t;


/** Group: EFUSE_APB2OTP Block1 Word7 Data */
/** Type of apb2otp_blk1_w7 register
 *  eFuse apb2otp block1 data register7.
 */
typedef union {
    struct {
        /** apb2otp_block1_w7 : RO; bitpos: [31:0]; default: 0;
         *  Otp block1  word7 data.
         */
        uint32_t apb2otp_block1_w7:32;
    };
    uint32_t val;
} efuse_apb2otp_blk1_w7_reg_t;


/** Group: EFUSE_APB2OTP Block1 Word8 Data */
/** Type of apb2otp_blk1_w8 register
 *  eFuse apb2otp block1 data register8.
 */
typedef union {
    struct {
        /** apb2otp_block1_w8 : RO; bitpos: [31:0]; default: 0;
         *  Otp block1  word8 data.
         */
        uint32_t apb2otp_block1_w8:32;
    };
    uint32_t val;
} efuse_apb2otp_blk1_w8_reg_t;


/** Group: EFUSE_APB2OTP Block1 Word9 Data */
/** Type of apb2otp_blk1_w9 register
 *  eFuse apb2otp block1 data register9.
 */
typedef union {
    struct {
        /** apb2otp_block1_w9 : RO; bitpos: [31:0]; default: 0;
         *  Otp block1  word9 data.
         */
        uint32_t apb2otp_block1_w9:32;
    };
    uint32_t val;
} efuse_apb2otp_blk1_w9_reg_t;


/** Group: EFUSE_APB2OTP Block2 Word1 Data */
/** Type of apb2otp_blk2_w1 register
 *  eFuse apb2otp block2 data register1.
 */
typedef union {
    struct {
        /** apb2otp_block2_w1 : RO; bitpos: [31:0]; default: 0;
         *  Otp block2 word1 data.
         */
        uint32_t apb2otp_block2_w1:32;
    };
    uint32_t val;
} efuse_apb2otp_blk2_w1_reg_t;


/** Group: EFUSE_APB2OTP Block2 Word2 Data */
/** Type of apb2otp_blk2_w2 register
 *  eFuse apb2otp block2 data register2.
 */
typedef union {
    struct {
        /** apb2otp_block2_w2 : RO; bitpos: [31:0]; default: 0;
         *  Otp block2 word2 data.
         */
        uint32_t apb2otp_block2_w2:32;
    };
    uint32_t val;
} efuse_apb2otp_blk2_w2_reg_t;


/** Group: EFUSE_APB2OTP Block2 Word3 Data */
/** Type of apb2otp_blk2_w3 register
 *  eFuse apb2otp block2 data register3.
 */
typedef union {
    struct {
        /** apb2otp_block2_w3 : RO; bitpos: [31:0]; default: 0;
         *  Otp block2 word3 data.
         */
        uint32_t apb2otp_block2_w3:32;
    };
    uint32_t val;
} efuse_apb2otp_blk2_w3_reg_t;


/** Group: EFUSE_APB2OTP Block2 Word4 Data */
/** Type of apb2otp_blk2_w4 register
 *  eFuse apb2otp block2 data register4.
 */
typedef union {
    struct {
        /** apb2otp_block2_w4 : RO; bitpos: [31:0]; default: 0;
         *  Otp block2 word4 data.
         */
        uint32_t apb2otp_block2_w4:32;
    };
    uint32_t val;
} efuse_apb2otp_blk2_w4_reg_t;


/** Group: EFUSE_APB2OTP Block2 Word5 Data */
/** Type of apb2otp_blk2_w5 register
 *  eFuse apb2otp block2 data register5.
 */
typedef union {
    struct {
        /** apb2otp_block2_w5 : RO; bitpos: [31:0]; default: 0;
         *  Otp block2 word5 data.
         */
        uint32_t apb2otp_block2_w5:32;
    };
    uint32_t val;
} efuse_apb2otp_blk2_w5_reg_t;


/** Group: EFUSE_APB2OTP Block2 Word6 Data */
/** Type of apb2otp_blk2_w6 register
 *  eFuse apb2otp block2 data register6.
 */
typedef union {
    struct {
        /** apb2otp_block2_w6 : RO; bitpos: [31:0]; default: 0;
         *  Otp block2 word6 data.
         */
        uint32_t apb2otp_block2_w6:32;
    };
    uint32_t val;
} efuse_apb2otp_blk2_w6_reg_t;


/** Group: EFUSE_APB2OTP Block2 Word7 Data */
/** Type of apb2otp_blk2_w7 register
 *  eFuse apb2otp block2 data register7.
 */
typedef union {
    struct {
        /** apb2otp_block2_w7 : RO; bitpos: [31:0]; default: 0;
         *  Otp block2 word7 data.
         */
        uint32_t apb2otp_block2_w7:32;
    };
    uint32_t val;
} efuse_apb2otp_blk2_w7_reg_t;


/** Group: EFUSE_APB2OTP Block2 Word8 Data */
/** Type of apb2otp_blk2_w8 register
 *  eFuse apb2otp block2 data register8.
 */
typedef union {
    struct {
        /** apb2otp_block2_w8 : RO; bitpos: [31:0]; default: 0;
         *  Otp block2 word8 data.
         */
        uint32_t apb2otp_block2_w8:32;
    };
    uint32_t val;
} efuse_apb2otp_blk2_w8_reg_t;


/** Group: EFUSE_APB2OTP Block2 Word9 Data */
/** Type of apb2otp_blk2_w9 register
 *  eFuse apb2otp block2 data register9.
 */
typedef union {
    struct {
        /** apb2otp_block2_w9 : RO; bitpos: [31:0]; default: 0;
         *  Otp block2 word9 data.
         */
        uint32_t apb2otp_block2_w9:32;
    };
    uint32_t val;
} efuse_apb2otp_blk2_w9_reg_t;


/** Group: EFUSE_APB2OTP Block2 Word10 Data */
/** Type of apb2otp_blk2_w10 register
 *  eFuse apb2otp block2 data register10.
 */
typedef union {
    struct {
        /** apb2otp_block2_w10 : RO; bitpos: [31:0]; default: 0;
         *  Otp block2 word10 data.
         */
        uint32_t apb2otp_block2_w10:32;
    };
    uint32_t val;
} efuse_apb2otp_blk2_w10_reg_t;


/** Group: EFUSE_APB2OTP Block2 Word11 Data */
/** Type of apb2otp_blk2_w11 register
 *  eFuse apb2otp block2 data register11.
 */
typedef union {
    struct {
        /** apb2otp_block2_w11 : RO; bitpos: [31:0]; default: 0;
         *  Otp block2 word11 data.
         */
        uint32_t apb2otp_block2_w11:32;
    };
    uint32_t val;
} efuse_apb2otp_blk2_w11_reg_t;

/** Type of apb2otp_blk10_w11 register
 *  eFuse apb2otp block10 data register11.
 */
typedef union {
    struct {
        /** apb2otp_block10_w11 : RO; bitpos: [31:0]; default: 0;
         *  Otp block10 word11 data.
         */
        uint32_t apb2otp_block10_w11:32;
    };
    uint32_t val;
} efuse_apb2otp_blk10_w11_reg_t;


/** Group: EFUSE_APB2OTP Block3 Word1 Data */
/** Type of apb2otp_blk3_w1 register
 *  eFuse apb2otp block3 data register1.
 */
typedef union {
    struct {
        /** apb2otp_block3_w1 : RO; bitpos: [31:0]; default: 0;
         *  Otp block3 word1 data.
         */
        uint32_t apb2otp_block3_w1:32;
    };
    uint32_t val;
} efuse_apb2otp_blk3_w1_reg_t;


/** Group: EFUSE_APB2OTP Block3 Word2 Data */
/** Type of apb2otp_blk3_w2 register
 *  eFuse apb2otp block3 data register2.
 */
typedef union {
    struct {
        /** apb2otp_block3_w2 : RO; bitpos: [31:0]; default: 0;
         *  Otp block3 word2 data.
         */
        uint32_t apb2otp_block3_w2:32;
    };
    uint32_t val;
} efuse_apb2otp_blk3_w2_reg_t;


/** Group: EFUSE_APB2OTP Block3 Word3 Data */
/** Type of apb2otp_blk3_w3 register
 *  eFuse apb2otp block3 data register3.
 */
typedef union {
    struct {
        /** apb2otp_block3_w3 : RO; bitpos: [31:0]; default: 0;
         *  Otp block3 word3 data.
         */
        uint32_t apb2otp_block3_w3:32;
    };
    uint32_t val;
} efuse_apb2otp_blk3_w3_reg_t;


/** Group: EFUSE_APB2OTP Block3 Word4 Data */
/** Type of apb2otp_blk3_w4 register
 *  eFuse apb2otp block3 data register4.
 */
typedef union {
    struct {
        /** apb2otp_block3_w4 : RO; bitpos: [31:0]; default: 0;
         *  Otp block3 word4 data.
         */
        uint32_t apb2otp_block3_w4:32;
    };
    uint32_t val;
} efuse_apb2otp_blk3_w4_reg_t;


/** Group: EFUSE_APB2OTP Block3 Word5 Data */
/** Type of apb2otp_blk3_w5 register
 *  eFuse apb2otp block3 data register5.
 */
typedef union {
    struct {
        /** apb2otp_block3_w5 : RO; bitpos: [31:0]; default: 0;
         *  Otp block3 word5 data.
         */
        uint32_t apb2otp_block3_w5:32;
    };
    uint32_t val;
} efuse_apb2otp_blk3_w5_reg_t;


/** Group: EFUSE_APB2OTP Block3 Word6 Data */
/** Type of apb2otp_blk3_w6 register
 *  eFuse apb2otp block3 data register6.
 */
typedef union {
    struct {
        /** apb2otp_block3_w6 : RO; bitpos: [31:0]; default: 0;
         *  Otp block3 word6 data.
         */
        uint32_t apb2otp_block3_w6:32;
    };
    uint32_t val;
} efuse_apb2otp_blk3_w6_reg_t;


/** Group: EFUSE_APB2OTP Block3 Word7 Data */
/** Type of apb2otp_blk3_w7 register
 *  eFuse apb2otp block3 data register7.
 */
typedef union {
    struct {
        /** apb2otp_block3_w7 : RO; bitpos: [31:0]; default: 0;
         *  Otp block3 word7 data.
         */
        uint32_t apb2otp_block3_w7:32;
    };
    uint32_t val;
} efuse_apb2otp_blk3_w7_reg_t;


/** Group: EFUSE_APB2OTP Block3 Word8 Data */
/** Type of apb2otp_blk3_w8 register
 *  eFuse apb2otp block3 data register8.
 */
typedef union {
    struct {
        /** apb2otp_block3_w8 : RO; bitpos: [31:0]; default: 0;
         *  Otp block3 word8 data.
         */
        uint32_t apb2otp_block3_w8:32;
    };
    uint32_t val;
} efuse_apb2otp_blk3_w8_reg_t;


/** Group: EFUSE_APB2OTP Block3 Word9 Data */
/** Type of apb2otp_blk3_w9 register
 *  eFuse apb2otp block3 data register9.
 */
typedef union {
    struct {
        /** apb2otp_block3_w9 : RO; bitpos: [31:0]; default: 0;
         *  Otp block3 word9 data.
         */
        uint32_t apb2otp_block3_w9:32;
    };
    uint32_t val;
} efuse_apb2otp_blk3_w9_reg_t;


/** Group: EFUSE_APB2OTP Block3 Word10 Data */
/** Type of apb2otp_blk3_w10 register
 *  eFuse apb2otp block3 data register10.
 */
typedef union {
    struct {
        /** apb2otp_block3_w10 : RO; bitpos: [31:0]; default: 0;
         *  Otp block3 word10 data.
         */
        uint32_t apb2otp_block3_w10:32;
    };
    uint32_t val;
} efuse_apb2otp_blk3_w10_reg_t;


/** Group: EFUSE_APB2OTP Block3 Word11 Data */
/** Type of apb2otp_blk3_w11 register
 *  eFuse apb2otp block3 data register11.
 */
typedef union {
    struct {
        /** apb2otp_block3_w11 : RO; bitpos: [31:0]; default: 0;
         *  Otp block3 word11 data.
         */
        uint32_t apb2otp_block3_w11:32;
    };
    uint32_t val;
} efuse_apb2otp_blk3_w11_reg_t;


/** Group: EFUSE_APB2OTP Block4 Word1 Data */
/** Type of apb2otp_blk4_w1 register
 *  eFuse apb2otp block4 data register1.
 */
typedef union {
    struct {
        /** apb2otp_block4_w1 : RO; bitpos: [31:0]; default: 0;
         *  Otp block4 word1 data.
         */
        uint32_t apb2otp_block4_w1:32;
    };
    uint32_t val;
} efuse_apb2otp_blk4_w1_reg_t;


/** Group: EFUSE_APB2OTP Block4 Word2 Data */
/** Type of apb2otp_blk4_w2 register
 *  eFuse apb2otp block4 data register2.
 */
typedef union {
    struct {
        /** apb2otp_block4_w2 : RO; bitpos: [31:0]; default: 0;
         *  Otp block4 word2 data.
         */
        uint32_t apb2otp_block4_w2:32;
    };
    uint32_t val;
} efuse_apb2otp_blk4_w2_reg_t;


/** Group: EFUSE_APB2OTP Block4 Word3 Data */
/** Type of apb2otp_blk4_w3 register
 *  eFuse apb2otp block4 data register3.
 */
typedef union {
    struct {
        /** apb2otp_block4_w3 : RO; bitpos: [31:0]; default: 0;
         *  Otp block4 word3 data.
         */
        uint32_t apb2otp_block4_w3:32;
    };
    uint32_t val;
} efuse_apb2otp_blk4_w3_reg_t;


/** Group: EFUSE_APB2OTP Block4 Word4 Data */
/** Type of apb2otp_blk4_w4 register
 *  eFuse apb2otp block4 data register4.
 */
typedef union {
    struct {
        /** apb2otp_block4_w4 : RO; bitpos: [31:0]; default: 0;
         *  Otp block4 word4 data.
         */
        uint32_t apb2otp_block4_w4:32;
    };
    uint32_t val;
} efuse_apb2otp_blk4_w4_reg_t;


/** Group: EFUSE_APB2OTP Block4 Word5 Data */
/** Type of apb2otp_blk4_w5 register
 *  eFuse apb2otp block4 data register5.
 */
typedef union {
    struct {
        /** apb2otp_block4_w5 : RO; bitpos: [31:0]; default: 0;
         *  Otp block4 word5 data.
         */
        uint32_t apb2otp_block4_w5:32;
    };
    uint32_t val;
} efuse_apb2otp_blk4_w5_reg_t;


/** Group: EFUSE_APB2OTP Block4 Word6 Data */
/** Type of apb2otp_blk4_w6 register
 *  eFuse apb2otp block4 data register6.
 */
typedef union {
    struct {
        /** apb2otp_block4_w6 : RO; bitpos: [31:0]; default: 0;
         *  Otp block4 word6 data.
         */
        uint32_t apb2otp_block4_w6:32;
    };
    uint32_t val;
} efuse_apb2otp_blk4_w6_reg_t;


/** Group: EFUSE_APB2OTP Block4 Word7 Data */
/** Type of apb2otp_blk4_w7 register
 *  eFuse apb2otp block4 data register7.
 */
typedef union {
    struct {
        /** apb2otp_block4_w7 : RO; bitpos: [31:0]; default: 0;
         *  Otp block4 word7 data.
         */
        uint32_t apb2otp_block4_w7:32;
    };
    uint32_t val;
} efuse_apb2otp_blk4_w7_reg_t;


/** Group: EFUSE_APB2OTP Block4 Word8 Data */
/** Type of apb2otp_blk4_w8 register
 *  eFuse apb2otp block4 data register8.
 */
typedef union {
    struct {
        /** apb2otp_block4_w8 : RO; bitpos: [31:0]; default: 0;
         *  Otp block4 word8 data.
         */
        uint32_t apb2otp_block4_w8:32;
    };
    uint32_t val;
} efuse_apb2otp_blk4_w8_reg_t;


/** Group: EFUSE_APB2OTP Block4 Word9 Data */
/** Type of apb2otp_blk4_w9 register
 *  eFuse apb2otp block4 data register9.
 */
typedef union {
    struct {
        /** apb2otp_block4_w9 : RO; bitpos: [31:0]; default: 0;
         *  Otp block4 word9 data.
         */
        uint32_t apb2otp_block4_w9:32;
    };
    uint32_t val;
} efuse_apb2otp_blk4_w9_reg_t;


/** Group: EFUSE_APB2OTP Block4 Word10 Data */
/** Type of apb2otp_blk4_w10 register
 *  eFuse apb2otp block4 data registe10.
 */
typedef union {
    struct {
        /** apb2otp_block4_w10 : RO; bitpos: [31:0]; default: 0;
         *  Otp block4 word10 data.
         */
        uint32_t apb2otp_block4_w10:32;
    };
    uint32_t val;
} efuse_apb2otp_blk4_w10_reg_t;


/** Group: EFUSE_APB2OTP Block4 Word11 Data */
/** Type of apb2otp_blk4_w11 register
 *  eFuse apb2otp block4 data register11.
 */
typedef union {
    struct {
        /** apb2otp_block4_w11 : RO; bitpos: [31:0]; default: 0;
         *  Otp block4 word11 data.
         */
        uint32_t apb2otp_block4_w11:32;
    };
    uint32_t val;
} efuse_apb2otp_blk4_w11_reg_t;


/** Group: EFUSE_APB2OTP Block5 Word1 Data */
/** Type of apb2otp_blk5_w1 register
 *  eFuse apb2otp block5 data register1.
 */
typedef union {
    struct {
        /** apb2otp_block5_w1 : RO; bitpos: [31:0]; default: 0;
         *  Otp block5 word1 data.
         */
        uint32_t apb2otp_block5_w1:32;
    };
    uint32_t val;
} efuse_apb2otp_blk5_w1_reg_t;


/** Group: EFUSE_APB2OTP Block5 Word2 Data */
/** Type of apb2otp_blk5_w2 register
 *  eFuse apb2otp block5 data register2.
 */
typedef union {
    struct {
        /** apb2otp_block5_w2 : RO; bitpos: [31:0]; default: 0;
         *  Otp block5 word2 data.
         */
        uint32_t apb2otp_block5_w2:32;
    };
    uint32_t val;
} efuse_apb2otp_blk5_w2_reg_t;


/** Group: EFUSE_APB2OTP Block5 Word3 Data */
/** Type of apb2otp_blk5_w3 register
 *  eFuse apb2otp block5 data register3.
 */
typedef union {
    struct {
        /** apb2otp_block5_w3 : RO; bitpos: [31:0]; default: 0;
         *  Otp block5 word3 data.
         */
        uint32_t apb2otp_block5_w3:32;
    };
    uint32_t val;
} efuse_apb2otp_blk5_w3_reg_t;


/** Group: EFUSE_APB2OTP Block5 Word4 Data */
/** Type of apb2otp_blk5_w4 register
 *  eFuse apb2otp block5 data register4.
 */
typedef union {
    struct {
        /** apb2otp_block5_w4 : RO; bitpos: [31:0]; default: 0;
         *  Otp block5 word4 data.
         */
        uint32_t apb2otp_block5_w4:32;
    };
    uint32_t val;
} efuse_apb2otp_blk5_w4_reg_t;


/** Group: EFUSE_APB2OTP Block5 Word5 Data */
/** Type of apb2otp_blk5_w5 register
 *  eFuse apb2otp block5 data register5.
 */
typedef union {
    struct {
        /** apb2otp_block5_w5 : RO; bitpos: [31:0]; default: 0;
         *  Otp block5 word5 data.
         */
        uint32_t apb2otp_block5_w5:32;
    };
    uint32_t val;
} efuse_apb2otp_blk5_w5_reg_t;


/** Group: EFUSE_APB2OTP Block5 Word6 Data */
/** Type of apb2otp_blk5_w6 register
 *  eFuse apb2otp block5 data register6.
 */
typedef union {
    struct {
        /** apb2otp_block5_w6 : RO; bitpos: [31:0]; default: 0;
         *  Otp block5 word6 data.
         */
        uint32_t apb2otp_block5_w6:32;
    };
    uint32_t val;
} efuse_apb2otp_blk5_w6_reg_t;


/** Group: EFUSE_APB2OTP Block5 Word7 Data */
/** Type of apb2otp_blk5_w7 register
 *  eFuse apb2otp block5 data register7.
 */
typedef union {
    struct {
        /** apb2otp_block5_w7 : RO; bitpos: [31:0]; default: 0;
         *  Otp block5 word7 data.
         */
        uint32_t apb2otp_block5_w7:32;
    };
    uint32_t val;
} efuse_apb2otp_blk5_w7_reg_t;


/** Group: EFUSE_APB2OTP Block5 Word8 Data */
/** Type of apb2otp_blk5_w8 register
 *  eFuse apb2otp block5 data register8.
 */
typedef union {
    struct {
        /** apb2otp_block5_w8 : RO; bitpos: [31:0]; default: 0;
         *  Otp block5 word8 data.
         */
        uint32_t apb2otp_block5_w8:32;
    };
    uint32_t val;
} efuse_apb2otp_blk5_w8_reg_t;


/** Group: EFUSE_APB2OTP Block5 Word9 Data */
/** Type of apb2otp_blk5_w9 register
 *  eFuse apb2otp block5 data register9.
 */
typedef union {
    struct {
        /** apb2otp_block5_w9 : RO; bitpos: [31:0]; default: 0;
         *  Otp block5 word9 data.
         */
        uint32_t apb2otp_block5_w9:32;
    };
    uint32_t val;
} efuse_apb2otp_blk5_w9_reg_t;


/** Group: EFUSE_APB2OTP Block5 Word10 Data */
/** Type of apb2otp_blk5_w10 register
 *  eFuse apb2otp block5 data register10.
 */
typedef union {
    struct {
        /** apb2otp_block5_w10 : RO; bitpos: [31:0]; default: 0;
         *  Otp block5 word10 data.
         */
        uint32_t apb2otp_block5_w10:32;
    };
    uint32_t val;
} efuse_apb2otp_blk5_w10_reg_t;


/** Group: EFUSE_APB2OTP Block5 Word11 Data */
/** Type of apb2otp_blk5_w11 register
 *  eFuse apb2otp block5 data register11.
 */
typedef union {
    struct {
        /** apb2otp_block5_w11 : RO; bitpos: [31:0]; default: 0;
         *  Otp block5 word11 data.
         */
        uint32_t apb2otp_block5_w11:32;
    };
    uint32_t val;
} efuse_apb2otp_blk5_w11_reg_t;


/** Group: EFUSE_APB2OTP Block6 Word1 Data */
/** Type of apb2otp_blk6_w1 register
 *  eFuse apb2otp block6 data register1.
 */
typedef union {
    struct {
        /** apb2otp_block6_w1 : RO; bitpos: [31:0]; default: 0;
         *  Otp block6 word1 data.
         */
        uint32_t apb2otp_block6_w1:32;
    };
    uint32_t val;
} efuse_apb2otp_blk6_w1_reg_t;


/** Group: EFUSE_APB2OTP Block6 Word2 Data */
/** Type of apb2otp_blk6_w2 register
 *  eFuse apb2otp block6 data register2.
 */
typedef union {
    struct {
        /** apb2otp_block6_w2 : RO; bitpos: [31:0]; default: 0;
         *  Otp block6 word2 data.
         */
        uint32_t apb2otp_block6_w2:32;
    };
    uint32_t val;
} efuse_apb2otp_blk6_w2_reg_t;


/** Group: EFUSE_APB2OTP Block6 Word3 Data */
/** Type of apb2otp_blk6_w3 register
 *  eFuse apb2otp block6 data register3.
 */
typedef union {
    struct {
        /** apb2otp_block6_w3 : RO; bitpos: [31:0]; default: 0;
         *  Otp block6 word3 data.
         */
        uint32_t apb2otp_block6_w3:32;
    };
    uint32_t val;
} efuse_apb2otp_blk6_w3_reg_t;


/** Group: EFUSE_APB2OTP Block6 Word4 Data */
/** Type of apb2otp_blk6_w4 register
 *  eFuse apb2otp block6 data register4.
 */
typedef union {
    struct {
        /** apb2otp_block6_w4 : RO; bitpos: [31:0]; default: 0;
         *  Otp block6 word4 data.
         */
        uint32_t apb2otp_block6_w4:32;
    };
    uint32_t val;
} efuse_apb2otp_blk6_w4_reg_t;


/** Group: EFUSE_APB2OTP Block6 Word5 Data */
/** Type of apb2otp_blk6_w5 register
 *  eFuse apb2otp block6 data register5.
 */
typedef union {
    struct {
        /** apb2otp_block6_w5 : RO; bitpos: [31:0]; default: 0;
         *  Otp block6 word5 data.
         */
        uint32_t apb2otp_block6_w5:32;
    };
    uint32_t val;
} efuse_apb2otp_blk6_w5_reg_t;


/** Group: EFUSE_APB2OTP Block6 Word6 Data */
/** Type of apb2otp_blk6_w6 register
 *  eFuse apb2otp block6 data register6.
 */
typedef union {
    struct {
        /** apb2otp_block6_w6 : RO; bitpos: [31:0]; default: 0;
         *  Otp block6 word6 data.
         */
        uint32_t apb2otp_block6_w6:32;
    };
    uint32_t val;
} efuse_apb2otp_blk6_w6_reg_t;


/** Group: EFUSE_APB2OTP Block6 Word7 Data */
/** Type of apb2otp_blk6_w7 register
 *  eFuse apb2otp block6 data register7.
 */
typedef union {
    struct {
        /** apb2otp_block6_w7 : RO; bitpos: [31:0]; default: 0;
         *  Otp block6 word7 data.
         */
        uint32_t apb2otp_block6_w7:32;
    };
    uint32_t val;
} efuse_apb2otp_blk6_w7_reg_t;


/** Group: EFUSE_APB2OTP Block6 Word8 Data */
/** Type of apb2otp_blk6_w8 register
 *  eFuse apb2otp block6 data register8.
 */
typedef union {
    struct {
        /** apb2otp_block6_w8 : RO; bitpos: [31:0]; default: 0;
         *  Otp block6 word8 data.
         */
        uint32_t apb2otp_block6_w8:32;
    };
    uint32_t val;
} efuse_apb2otp_blk6_w8_reg_t;


/** Group: EFUSE_APB2OTP Block6 Word9 Data */
/** Type of apb2otp_blk6_w9 register
 *  eFuse apb2otp block6 data register9.
 */
typedef union {
    struct {
        /** apb2otp_block6_w9 : RO; bitpos: [31:0]; default: 0;
         *  Otp block6 word9 data.
         */
        uint32_t apb2otp_block6_w9:32;
    };
    uint32_t val;
} efuse_apb2otp_blk6_w9_reg_t;


/** Group: EFUSE_APB2OTP Block6 Word10 Data */
/** Type of apb2otp_blk6_w10 register
 *  eFuse apb2otp block6 data register10.
 */
typedef union {
    struct {
        /** apb2otp_block6_w10 : RO; bitpos: [31:0]; default: 0;
         *  Otp block6 word10 data.
         */
        uint32_t apb2otp_block6_w10:32;
    };
    uint32_t val;
} efuse_apb2otp_blk6_w10_reg_t;


/** Group: EFUSE_APB2OTP Block6 Word11 Data */
/** Type of apb2otp_blk6_w11 register
 *  eFuse apb2otp block6 data register11.
 */
typedef union {
    struct {
        /** apb2otp_block6_w11 : RO; bitpos: [31:0]; default: 0;
         *  Otp block6 word11 data.
         */
        uint32_t apb2otp_block6_w11:32;
    };
    uint32_t val;
} efuse_apb2otp_blk6_w11_reg_t;


/** Group: EFUSE_APB2OTP Block7 Word1 Data */
/** Type of apb2otp_blk7_w1 register
 *  eFuse apb2otp block7 data register1.
 */
typedef union {
    struct {
        /** apb2otp_block7_w1 : RO; bitpos: [31:0]; default: 0;
         *  Otp block7 word1 data.
         */
        uint32_t apb2otp_block7_w1:32;
    };
    uint32_t val;
} efuse_apb2otp_blk7_w1_reg_t;


/** Group: EFUSE_APB2OTP Block7 Word2 Data */
/** Type of apb2otp_blk7_w2 register
 *  eFuse apb2otp block7 data register2.
 */
typedef union {
    struct {
        /** apb2otp_block7_w2 : RO; bitpos: [31:0]; default: 0;
         *  Otp block7 word2 data.
         */
        uint32_t apb2otp_block7_w2:32;
    };
    uint32_t val;
} efuse_apb2otp_blk7_w2_reg_t;


/** Group: EFUSE_APB2OTP Block7 Word3 Data */
/** Type of apb2otp_blk7_w3 register
 *  eFuse apb2otp block7 data register3.
 */
typedef union {
    struct {
        /** apb2otp_block7_w3 : RO; bitpos: [31:0]; default: 0;
         *  Otp block7 word3 data.
         */
        uint32_t apb2otp_block7_w3:32;
    };
    uint32_t val;
} efuse_apb2otp_blk7_w3_reg_t;


/** Group: EFUSE_APB2OTP Block7 Word4 Data */
/** Type of apb2otp_blk7_w4 register
 *  eFuse apb2otp block7 data register4.
 */
typedef union {
    struct {
        /** apb2otp_block7_w4 : RO; bitpos: [31:0]; default: 0;
         *  Otp block7 word4 data.
         */
        uint32_t apb2otp_block7_w4:32;
    };
    uint32_t val;
} efuse_apb2otp_blk7_w4_reg_t;


/** Group: EFUSE_APB2OTP Block7 Word5 Data */
/** Type of apb2otp_blk7_w5 register
 *  eFuse apb2otp block7 data register5.
 */
typedef union {
    struct {
        /** apb2otp_block7_w5 : RO; bitpos: [31:0]; default: 0;
         *  Otp block7 word5 data.
         */
        uint32_t apb2otp_block7_w5:32;
    };
    uint32_t val;
} efuse_apb2otp_blk7_w5_reg_t;


/** Group: EFUSE_APB2OTP Block7 Word6 Data */
/** Type of apb2otp_blk7_w6 register
 *  eFuse apb2otp block7 data register6.
 */
typedef union {
    struct {
        /** apb2otp_block7_w6 : RO; bitpos: [31:0]; default: 0;
         *  Otp block7 word6 data.
         */
        uint32_t apb2otp_block7_w6:32;
    };
    uint32_t val;
} efuse_apb2otp_blk7_w6_reg_t;


/** Group: EFUSE_APB2OTP Block7 Word7 Data */
/** Type of apb2otp_blk7_w7 register
 *  eFuse apb2otp block7 data register7.
 */
typedef union {
    struct {
        /** apb2otp_block7_w7 : RO; bitpos: [31:0]; default: 0;
         *  Otp block7 word7 data.
         */
        uint32_t apb2otp_block7_w7:32;
    };
    uint32_t val;
} efuse_apb2otp_blk7_w7_reg_t;


/** Group: EFUSE_APB2OTP Block7 Word8 Data */
/** Type of apb2otp_blk7_w8 register
 *  eFuse apb2otp block7 data register8.
 */
typedef union {
    struct {
        /** apb2otp_block7_w8 : RO; bitpos: [31:0]; default: 0;
         *  Otp block7 word8 data.
         */
        uint32_t apb2otp_block7_w8:32;
    };
    uint32_t val;
} efuse_apb2otp_blk7_w8_reg_t;


/** Group: EFUSE_APB2OTP Block7 Word9 Data */
/** Type of apb2otp_blk7_w9 register
 *  eFuse apb2otp block7 data register9.
 */
typedef union {
    struct {
        /** apb2otp_block7_w9 : RO; bitpos: [31:0]; default: 0;
         *  Otp block7 word9 data.
         */
        uint32_t apb2otp_block7_w9:32;
    };
    uint32_t val;
} efuse_apb2otp_blk7_w9_reg_t;


/** Group: EFUSE_APB2OTP Block7 Word10 Data */
/** Type of apb2otp_blk7_w10 register
 *  eFuse apb2otp block7 data register10.
 */
typedef union {
    struct {
        /** apb2otp_block7_w10 : RO; bitpos: [31:0]; default: 0;
         *  Otp block7 word10 data.
         */
        uint32_t apb2otp_block7_w10:32;
    };
    uint32_t val;
} efuse_apb2otp_blk7_w10_reg_t;


/** Group: EFUSE_APB2OTP Block7 Word11 Data */
/** Type of apb2otp_blk7_w11 register
 *  eFuse apb2otp block7 data register11.
 */
typedef union {
    struct {
        /** apb2otp_block7_w11 : RO; bitpos: [31:0]; default: 0;
         *  Otp block7 word11 data.
         */
        uint32_t apb2otp_block7_w11:32;
    };
    uint32_t val;
} efuse_apb2otp_blk7_w11_reg_t;


/** Group: EFUSE_APB2OTP Block8 Word1 Data */
/** Type of apb2otp_blk8_w1 register
 *  eFuse apb2otp block8 data register1.
 */
typedef union {
    struct {
        /** apb2otp_block8_w1 : RO; bitpos: [31:0]; default: 0;
         *  Otp block8 word1 data.
         */
        uint32_t apb2otp_block8_w1:32;
    };
    uint32_t val;
} efuse_apb2otp_blk8_w1_reg_t;


/** Group: EFUSE_APB2OTP Block8 Word2 Data */
/** Type of apb2otp_blk8_w2 register
 *  eFuse apb2otp block8 data register2.
 */
typedef union {
    struct {
        /** apb2otp_block8_w2 : RO; bitpos: [31:0]; default: 0;
         *  Otp block8 word2 data.
         */
        uint32_t apb2otp_block8_w2:32;
    };
    uint32_t val;
} efuse_apb2otp_blk8_w2_reg_t;


/** Group: EFUSE_APB2OTP Block8 Word3 Data */
/** Type of apb2otp_blk8_w3 register
 *  eFuse apb2otp block8 data register3.
 */
typedef union {
    struct {
        /** apb2otp_block8_w3 : RO; bitpos: [31:0]; default: 0;
         *  Otp block8 word3 data.
         */
        uint32_t apb2otp_block8_w3:32;
    };
    uint32_t val;
} efuse_apb2otp_blk8_w3_reg_t;


/** Group: EFUSE_APB2OTP Block8 Word4 Data */
/** Type of apb2otp_blk8_w4 register
 *  eFuse apb2otp block8 data register4.
 */
typedef union {
    struct {
        /** apb2otp_block8_w4 : RO; bitpos: [31:0]; default: 0;
         *  Otp block8 word4 data.
         */
        uint32_t apb2otp_block8_w4:32;
    };
    uint32_t val;
} efuse_apb2otp_blk8_w4_reg_t;


/** Group: EFUSE_APB2OTP Block8 Word5 Data */
/** Type of apb2otp_blk8_w5 register
 *  eFuse apb2otp block8 data register5.
 */
typedef union {
    struct {
        /** apb2otp_block8_w5 : RO; bitpos: [31:0]; default: 0;
         *  Otp block8 word5 data.
         */
        uint32_t apb2otp_block8_w5:32;
    };
    uint32_t val;
} efuse_apb2otp_blk8_w5_reg_t;


/** Group: EFUSE_APB2OTP Block8 Word6 Data */
/** Type of apb2otp_blk8_w6 register
 *  eFuse apb2otp block8 data register6.
 */
typedef union {
    struct {
        /** apb2otp_block8_w6 : RO; bitpos: [31:0]; default: 0;
         *  Otp block8 word6 data.
         */
        uint32_t apb2otp_block8_w6:32;
    };
    uint32_t val;
} efuse_apb2otp_blk8_w6_reg_t;


/** Group: EFUSE_APB2OTP Block8 Word7 Data */
/** Type of apb2otp_blk8_w7 register
 *  eFuse apb2otp block8 data register7.
 */
typedef union {
    struct {
        /** apb2otp_block8_w7 : RO; bitpos: [31:0]; default: 0;
         *  Otp block8 word7 data.
         */
        uint32_t apb2otp_block8_w7:32;
    };
    uint32_t val;
} efuse_apb2otp_blk8_w7_reg_t;


/** Group: EFUSE_APB2OTP Block8 Word8 Data */
/** Type of apb2otp_blk8_w8 register
 *  eFuse apb2otp block8 data register8.
 */
typedef union {
    struct {
        /** apb2otp_block8_w8 : RO; bitpos: [31:0]; default: 0;
         *  Otp block8 word8 data.
         */
        uint32_t apb2otp_block8_w8:32;
    };
    uint32_t val;
} efuse_apb2otp_blk8_w8_reg_t;


/** Group: EFUSE_APB2OTP Block8 Word9 Data */
/** Type of apb2otp_blk8_w9 register
 *  eFuse apb2otp block8 data register9.
 */
typedef union {
    struct {
        /** apb2otp_block8_w9 : RO; bitpos: [31:0]; default: 0;
         *  Otp block8 word9 data.
         */
        uint32_t apb2otp_block8_w9:32;
    };
    uint32_t val;
} efuse_apb2otp_blk8_w9_reg_t;


/** Group: EFUSE_APB2OTP Block8 Word10 Data */
/** Type of apb2otp_blk8_w10 register
 *  eFuse apb2otp block8 data register10.
 */
typedef union {
    struct {
        /** apb2otp_block8_w10 : RO; bitpos: [31:0]; default: 0;
         *  Otp block8 word10 data.
         */
        uint32_t apb2otp_block8_w10:32;
    };
    uint32_t val;
} efuse_apb2otp_blk8_w10_reg_t;


/** Group: EFUSE_APB2OTP Block8 Word11 Data */
/** Type of apb2otp_blk8_w11 register
 *  eFuse apb2otp block8 data register11.
 */
typedef union {
    struct {
        /** apb2otp_block8_w11 : RO; bitpos: [31:0]; default: 0;
         *  Otp block8 word11 data.
         */
        uint32_t apb2otp_block8_w11:32;
    };
    uint32_t val;
} efuse_apb2otp_blk8_w11_reg_t;


/** Group: EFUSE_APB2OTP Block9 Word1 Data */
/** Type of apb2otp_blk9_w1 register
 *  eFuse apb2otp block9 data register1.
 */
typedef union {
    struct {
        /** apb2otp_block9_w1 : RO; bitpos: [31:0]; default: 0;
         *  Otp block9 word1 data.
         */
        uint32_t apb2otp_block9_w1:32;
    };
    uint32_t val;
} efuse_apb2otp_blk9_w1_reg_t;


/** Group: EFUSE_APB2OTP Block9 Word2 Data */
/** Type of apb2otp_blk9_w2 register
 *  eFuse apb2otp block9 data register2.
 */
typedef union {
    struct {
        /** apb2otp_block9_w2 : RO; bitpos: [31:0]; default: 0;
         *  Otp block9 word2 data.
         */
        uint32_t apb2otp_block9_w2:32;
    };
    uint32_t val;
} efuse_apb2otp_blk9_w2_reg_t;


/** Group: EFUSE_APB2OTP Block9 Word3 Data */
/** Type of apb2otp_blk9_w3 register
 *  eFuse apb2otp block9 data register3.
 */
typedef union {
    struct {
        /** apb2otp_block9_w3 : RO; bitpos: [31:0]; default: 0;
         *  Otp block9 word3 data.
         */
        uint32_t apb2otp_block9_w3:32;
    };
    uint32_t val;
} efuse_apb2otp_blk9_w3_reg_t;


/** Group: EFUSE_APB2OTP Block9 Word4 Data */
/** Type of apb2otp_blk9_w4 register
 *  eFuse apb2otp block9 data register4.
 */
typedef union {
    struct {
        /** apb2otp_block9_w4 : RO; bitpos: [31:0]; default: 0;
         *  Otp block9 word4 data.
         */
        uint32_t apb2otp_block9_w4:32;
    };
    uint32_t val;
} efuse_apb2otp_blk9_w4_reg_t;


/** Group: EFUSE_APB2OTP Block9 Word5 Data */
/** Type of apb2otp_blk9_w5 register
 *  eFuse apb2otp block9 data register5.
 */
typedef union {
    struct {
        /** apb2otp_block9_w5 : RO; bitpos: [31:0]; default: 0;
         *  Otp block9 word5 data.
         */
        uint32_t apb2otp_block9_w5:32;
    };
    uint32_t val;
} efuse_apb2otp_blk9_w5_reg_t;


/** Group: EFUSE_APB2OTP Block9 Word6 Data */
/** Type of apb2otp_blk9_w6 register
 *  eFuse apb2otp block9 data register6.
 */
typedef union {
    struct {
        /** apb2otp_block9_w6 : RO; bitpos: [31:0]; default: 0;
         *  Otp block9 word6 data.
         */
        uint32_t apb2otp_block9_w6:32;
    };
    uint32_t val;
} efuse_apb2otp_blk9_w6_reg_t;


/** Group: EFUSE_APB2OTP Block9 Word7 Data */
/** Type of apb2otp_blk9_w7 register
 *  eFuse apb2otp block9 data register7.
 */
typedef union {
    struct {
        /** apb2otp_block9_w7 : RO; bitpos: [31:0]; default: 0;
         *  Otp block9 word7 data.
         */
        uint32_t apb2otp_block9_w7:32;
    };
    uint32_t val;
} efuse_apb2otp_blk9_w7_reg_t;


/** Group: EFUSE_APB2OTP Block9 Word8 Data */
/** Type of apb2otp_blk9_w8 register
 *  eFuse apb2otp block9 data register8.
 */
typedef union {
    struct {
        /** apb2otp_block9_w8 : RO; bitpos: [31:0]; default: 0;
         *  Otp block9 word8 data.
         */
        uint32_t apb2otp_block9_w8:32;
    };
    uint32_t val;
} efuse_apb2otp_blk9_w8_reg_t;


/** Group: EFUSE_APB2OTP Block9 Word9 Data */
/** Type of apb2otp_blk9_w9 register
 *  eFuse apb2otp block9 data register9.
 */
typedef union {
    struct {
        /** apb2otp_block9_w9 : RO; bitpos: [31:0]; default: 0;
         *  Otp block9 word9 data.
         */
        uint32_t apb2otp_block9_w9:32;
    };
    uint32_t val;
} efuse_apb2otp_blk9_w9_reg_t;


/** Group: EFUSE_APB2OTP Block9 Word10 Data */
/** Type of apb2otp_blk9_w10 register
 *  eFuse apb2otp block9 data register10.
 */
typedef union {
    struct {
        /** apb2otp_block9_w10 : RO; bitpos: [31:0]; default: 0;
         *  Otp block9 word10 data.
         */
        uint32_t apb2otp_block9_w10:32;
    };
    uint32_t val;
} efuse_apb2otp_blk9_w10_reg_t;


/** Group: EFUSE_APB2OTP Block9 Word11 Data */
/** Type of apb2otp_blk9_w11 register
 *  eFuse apb2otp block9 data register11.
 */
typedef union {
    struct {
        /** apb2otp_block9_w11 : RO; bitpos: [31:0]; default: 0;
         *  Otp block9 word11 data.
         */
        uint32_t apb2otp_block9_w11:32;
    };
    uint32_t val;
} efuse_apb2otp_blk9_w11_reg_t;


/** Group: EFUSE_APB2OTP Block10 Word1 Data */
/** Type of apb2otp_blk10_w1 register
 *  eFuse apb2otp block10 data register1.
 */
typedef union {
    struct {
        /** apb2otp_block10_w1 : RO; bitpos: [31:0]; default: 0;
         *  Otp block10 word1 data.
         */
        uint32_t apb2otp_block10_w1:32;
    };
    uint32_t val;
} efuse_apb2otp_blk10_w1_reg_t;


/** Group: EFUSE_APB2OTP Block10 Word2 Data */
/** Type of apb2otp_blk10_w2 register
 *  eFuse apb2otp block10 data register2.
 */
typedef union {
    struct {
        /** apb2otp_block10_w2 : RO; bitpos: [31:0]; default: 0;
         *  Otp block10 word2 data.
         */
        uint32_t apb2otp_block10_w2:32;
    };
    uint32_t val;
} efuse_apb2otp_blk10_w2_reg_t;


/** Group: EFUSE_APB2OTP Block10 Word3 Data */
/** Type of apb2otp_blk10_w3 register
 *  eFuse apb2otp block10 data register3.
 */
typedef union {
    struct {
        /** apb2otp_block10_w3 : RO; bitpos: [31:0]; default: 0;
         *  Otp block10 word3 data.
         */
        uint32_t apb2otp_block10_w3:32;
    };
    uint32_t val;
} efuse_apb2otp_blk10_w3_reg_t;


/** Group: EFUSE_APB2OTP Block10 Word4 Data */
/** Type of apb2otp_blk10_w4 register
 *  eFuse apb2otp block10 data register4.
 */
typedef union {
    struct {
        /** apb2otp_block10_w4 : RO; bitpos: [31:0]; default: 0;
         *  Otp block10 word4 data.
         */
        uint32_t apb2otp_block10_w4:32;
    };
    uint32_t val;
} efuse_apb2otp_blk10_w4_reg_t;


/** Group: EFUSE_APB2OTP Block10 Word5 Data */
/** Type of apb2otp_blk10_w5 register
 *  eFuse apb2otp block10 data register5.
 */
typedef union {
    struct {
        /** apb2otp_block10_w5 : RO; bitpos: [31:0]; default: 0;
         *  Otp block10 word5 data.
         */
        uint32_t apb2otp_block10_w5:32;
    };
    uint32_t val;
} efuse_apb2otp_blk10_w5_reg_t;


/** Group: EFUSE_APB2OTP Block10 Word6 Data */
/** Type of apb2otp_blk10_w6 register
 *  eFuse apb2otp block10 data register6.
 */
typedef union {
    struct {
        /** apb2otp_block10_w6 : RO; bitpos: [31:0]; default: 0;
         *  Otp block10 word6 data.
         */
        uint32_t apb2otp_block10_w6:32;
    };
    uint32_t val;
} efuse_apb2otp_blk10_w6_reg_t;


/** Group: EFUSE_APB2OTP Block10 Word7 Data */
/** Type of apb2otp_blk10_w7 register
 *  eFuse apb2otp block10 data register7.
 */
typedef union {
    struct {
        /** apb2otp_block10_w7 : RO; bitpos: [31:0]; default: 0;
         *  Otp block10 word7 data.
         */
        uint32_t apb2otp_block10_w7:32;
    };
    uint32_t val;
} efuse_apb2otp_blk10_w7_reg_t;


/** Group: EFUSE_APB2OTP Block10 Word8 Data */
/** Type of apb2otp_blk10_w8 register
 *  eFuse apb2otp block10 data register8.
 */
typedef union {
    struct {
        /** apb2otp_block10_w8 : RO; bitpos: [31:0]; default: 0;
         *  Otp block10 word8 data.
         */
        uint32_t apb2otp_block10_w8:32;
    };
    uint32_t val;
} efuse_apb2otp_blk10_w8_reg_t;


/** Group: EFUSE_APB2OTP Block10 Word9 Data */
/** Type of apb2otp_blk10_w9 register
 *  eFuse apb2otp block10 data register9.
 */
typedef union {
    struct {
        /** apb2otp_block10_w9 : RO; bitpos: [31:0]; default: 0;
         *  Otp block10 word9 data.
         */
        uint32_t apb2otp_block10_w9:32;
    };
    uint32_t val;
} efuse_apb2otp_blk10_w9_reg_t;


/** Group: EFUSE_APB2OTP Block10 Word10 Data */
/** Type of apb2otp_blk10_w10 register
 *  eFuse apb2otp block10 data register10.
 */
typedef union {
    struct {
        /** apb2otp_block19_w10 : RO; bitpos: [31:0]; default: 0;
         *  Otp block10 word10 data.
         */
        uint32_t apb2otp_block19_w10:32;
    };
    uint32_t val;
} efuse_apb2otp_blk10_w10_reg_t;


/** Group: EFUSE_APB2OTP Function Enable Singal */
/** Type of apb2otp_en register
 *  eFuse apb2otp enable configuration register.
 */
typedef union {
    struct {
        /** apb2otp_apb2otp_en : R/W; bitpos: [0]; default: 0;
         *  Apb2otp mode enable signal.
         */
        uint32_t apb2otp_apb2otp_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} efuse_apb2otp_en_reg_t;


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
    volatile efuse_rd_mac_sys_0_reg_t rd_mac_sys_0;
    volatile efuse_rd_mac_sys_1_reg_t rd_mac_sys_1;
    volatile efuse_rd_mac_sys_2_reg_t rd_mac_sys_2;
    volatile efuse_rd_mac_sys_3_reg_t rd_mac_sys_3;
    volatile efuse_rd_mac_sys_4_reg_t rd_mac_sys_4;
    volatile efuse_rd_mac_sys_5_reg_t rd_mac_sys_5;
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
    volatile efuse_rd_repeat_err4_reg_t rd_repeat_err4;
    uint32_t reserved_190[12];
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
    uint32_t reserved_200[384];
    volatile efuse_apb2otp_wr_dis_reg_t apb2otp_wr_dis;
    volatile efuse_apb2otp_blk0_backup1_w1_reg_t apb2otp_blk0_backup1_w1;
    volatile efuse_apb2otp_blk0_backup1_w2_reg_t apb2otp_blk0_backup1_w2;
    volatile efuse_apb2otp_blk0_backup1_w3_reg_t apb2otp_blk0_backup1_w3;
    volatile efuse_apb2otp_blk0_backup1_w4_reg_t apb2otp_blk0_backup1_w4;
    volatile efuse_apb2otp_blk0_backup1_w5_reg_t apb2otp_blk0_backup1_w5;
    volatile efuse_apb2otp_blk0_backup2_w1_reg_t apb2otp_blk0_backup2_w1;
    volatile efuse_apb2otp_blk0_backup2_w2_reg_t apb2otp_blk0_backup2_w2;
    volatile efuse_apb2otp_blk0_backup2_w3_reg_t apb2otp_blk0_backup2_w3;
    volatile efuse_apb2otp_blk0_backup2_w4_reg_t apb2otp_blk0_backup2_w4;
    volatile efuse_apb2otp_blk0_backup2_w5_reg_t apb2otp_blk0_backup2_w5;
    volatile efuse_apb2otp_blk0_backup3_w1_reg_t apb2otp_blk0_backup3_w1;
    volatile efuse_apb2otp_blk0_backup3_w2_reg_t apb2otp_blk0_backup3_w2;
    volatile efuse_apb2otp_blk0_backup3_w3_reg_t apb2otp_blk0_backup3_w3;
    volatile efuse_apb2otp_blk0_backup3_w4_reg_t apb2otp_blk0_backup3_w4;
    volatile efuse_apb2otp_blk0_backup3_w5_reg_t apb2otp_blk0_backup3_w5;
    volatile efuse_apb2otp_blk0_backup4_w1_reg_t apb2otp_blk0_backup4_w1;
    volatile efuse_apb2otp_blk0_backup4_w2_reg_t apb2otp_blk0_backup4_w2;
    volatile efuse_apb2otp_blk0_backup4_w3_reg_t apb2otp_blk0_backup4_w3;
    volatile efuse_apb2otp_blk0_backup4_w4_reg_t apb2otp_blk0_backup4_w4;
    volatile efuse_apb2otp_blk0_backup4_w5_reg_t apb2otp_blk0_backup4_w5;
    volatile efuse_apb2otp_blk1_w1_reg_t apb2otp_blk1_w1;
    volatile efuse_apb2otp_blk1_w2_reg_t apb2otp_blk1_w2;
    volatile efuse_apb2otp_blk1_w3_reg_t apb2otp_blk1_w3;
    volatile efuse_apb2otp_blk1_w4_reg_t apb2otp_blk1_w4;
    volatile efuse_apb2otp_blk1_w5_reg_t apb2otp_blk1_w5;
    volatile efuse_apb2otp_blk1_w6_reg_t apb2otp_blk1_w6;
    volatile efuse_apb2otp_blk1_w7_reg_t apb2otp_blk1_w7;
    volatile efuse_apb2otp_blk1_w8_reg_t apb2otp_blk1_w8;
    volatile efuse_apb2otp_blk1_w9_reg_t apb2otp_blk1_w9;
    volatile efuse_apb2otp_blk2_w1_reg_t apb2otp_blk2_w1;
    volatile efuse_apb2otp_blk2_w2_reg_t apb2otp_blk2_w2;
    volatile efuse_apb2otp_blk2_w3_reg_t apb2otp_blk2_w3;
    volatile efuse_apb2otp_blk2_w4_reg_t apb2otp_blk2_w4;
    volatile efuse_apb2otp_blk2_w5_reg_t apb2otp_blk2_w5;
    volatile efuse_apb2otp_blk2_w6_reg_t apb2otp_blk2_w6;
    volatile efuse_apb2otp_blk2_w7_reg_t apb2otp_blk2_w7;
    volatile efuse_apb2otp_blk2_w8_reg_t apb2otp_blk2_w8;
    volatile efuse_apb2otp_blk2_w9_reg_t apb2otp_blk2_w9;
    volatile efuse_apb2otp_blk2_w10_reg_t apb2otp_blk2_w10;
    volatile efuse_apb2otp_blk2_w11_reg_t apb2otp_blk2_w11;
    volatile efuse_apb2otp_blk3_w1_reg_t apb2otp_blk3_w1;
    volatile efuse_apb2otp_blk3_w2_reg_t apb2otp_blk3_w2;
    volatile efuse_apb2otp_blk3_w3_reg_t apb2otp_blk3_w3;
    volatile efuse_apb2otp_blk3_w4_reg_t apb2otp_blk3_w4;
    volatile efuse_apb2otp_blk3_w5_reg_t apb2otp_blk3_w5;
    volatile efuse_apb2otp_blk3_w6_reg_t apb2otp_blk3_w6;
    volatile efuse_apb2otp_blk3_w7_reg_t apb2otp_blk3_w7;
    volatile efuse_apb2otp_blk3_w8_reg_t apb2otp_blk3_w8;
    volatile efuse_apb2otp_blk3_w9_reg_t apb2otp_blk3_w9;
    volatile efuse_apb2otp_blk3_w10_reg_t apb2otp_blk3_w10;
    volatile efuse_apb2otp_blk3_w11_reg_t apb2otp_blk3_w11;
    volatile efuse_apb2otp_blk4_w1_reg_t apb2otp_blk4_w1;
    volatile efuse_apb2otp_blk4_w2_reg_t apb2otp_blk4_w2;
    volatile efuse_apb2otp_blk4_w3_reg_t apb2otp_blk4_w3;
    volatile efuse_apb2otp_blk4_w4_reg_t apb2otp_blk4_w4;
    volatile efuse_apb2otp_blk4_w5_reg_t apb2otp_blk4_w5;
    volatile efuse_apb2otp_blk4_w6_reg_t apb2otp_blk4_w6;
    volatile efuse_apb2otp_blk4_w7_reg_t apb2otp_blk4_w7;
    volatile efuse_apb2otp_blk4_w8_reg_t apb2otp_blk4_w8;
    volatile efuse_apb2otp_blk4_w9_reg_t apb2otp_blk4_w9;
    volatile efuse_apb2otp_blk4_w10_reg_t apb2otp_blk4_w10;
    volatile efuse_apb2otp_blk4_w11_reg_t apb2otp_blk4_w11;
    volatile efuse_apb2otp_blk5_w1_reg_t apb2otp_blk5_w1;
    volatile efuse_apb2otp_blk5_w2_reg_t apb2otp_blk5_w2;
    volatile efuse_apb2otp_blk5_w3_reg_t apb2otp_blk5_w3;
    volatile efuse_apb2otp_blk5_w4_reg_t apb2otp_blk5_w4;
    volatile efuse_apb2otp_blk5_w5_reg_t apb2otp_blk5_w5;
    volatile efuse_apb2otp_blk5_w6_reg_t apb2otp_blk5_w6;
    volatile efuse_apb2otp_blk5_w7_reg_t apb2otp_blk5_w7;
    volatile efuse_apb2otp_blk5_w8_reg_t apb2otp_blk5_w8;
    volatile efuse_apb2otp_blk5_w9_reg_t apb2otp_blk5_w9;
    volatile efuse_apb2otp_blk5_w10_reg_t apb2otp_blk5_w10;
    volatile efuse_apb2otp_blk5_w11_reg_t apb2otp_blk5_w11;
    volatile efuse_apb2otp_blk6_w1_reg_t apb2otp_blk6_w1;
    volatile efuse_apb2otp_blk6_w2_reg_t apb2otp_blk6_w2;
    volatile efuse_apb2otp_blk6_w3_reg_t apb2otp_blk6_w3;
    volatile efuse_apb2otp_blk6_w4_reg_t apb2otp_blk6_w4;
    volatile efuse_apb2otp_blk6_w5_reg_t apb2otp_blk6_w5;
    volatile efuse_apb2otp_blk6_w6_reg_t apb2otp_blk6_w6;
    volatile efuse_apb2otp_blk6_w7_reg_t apb2otp_blk6_w7;
    volatile efuse_apb2otp_blk6_w8_reg_t apb2otp_blk6_w8;
    volatile efuse_apb2otp_blk6_w9_reg_t apb2otp_blk6_w9;
    volatile efuse_apb2otp_blk6_w10_reg_t apb2otp_blk6_w10;
    volatile efuse_apb2otp_blk6_w11_reg_t apb2otp_blk6_w11;
    volatile efuse_apb2otp_blk7_w1_reg_t apb2otp_blk7_w1;
    volatile efuse_apb2otp_blk7_w2_reg_t apb2otp_blk7_w2;
    volatile efuse_apb2otp_blk7_w3_reg_t apb2otp_blk7_w3;
    volatile efuse_apb2otp_blk7_w4_reg_t apb2otp_blk7_w4;
    volatile efuse_apb2otp_blk7_w5_reg_t apb2otp_blk7_w5;
    volatile efuse_apb2otp_blk7_w6_reg_t apb2otp_blk7_w6;
    volatile efuse_apb2otp_blk7_w7_reg_t apb2otp_blk7_w7;
    volatile efuse_apb2otp_blk7_w8_reg_t apb2otp_blk7_w8;
    volatile efuse_apb2otp_blk7_w9_reg_t apb2otp_blk7_w9;
    volatile efuse_apb2otp_blk7_w10_reg_t apb2otp_blk7_w10;
    volatile efuse_apb2otp_blk7_w11_reg_t apb2otp_blk7_w11;
    volatile efuse_apb2otp_blk8_w1_reg_t apb2otp_blk8_w1;
    volatile efuse_apb2otp_blk8_w2_reg_t apb2otp_blk8_w2;
    volatile efuse_apb2otp_blk8_w3_reg_t apb2otp_blk8_w3;
    volatile efuse_apb2otp_blk8_w4_reg_t apb2otp_blk8_w4;
    volatile efuse_apb2otp_blk8_w5_reg_t apb2otp_blk8_w5;
    volatile efuse_apb2otp_blk8_w6_reg_t apb2otp_blk8_w6;
    volatile efuse_apb2otp_blk8_w7_reg_t apb2otp_blk8_w7;
    volatile efuse_apb2otp_blk8_w8_reg_t apb2otp_blk8_w8;
    volatile efuse_apb2otp_blk8_w9_reg_t apb2otp_blk8_w9;
    volatile efuse_apb2otp_blk8_w10_reg_t apb2otp_blk8_w10;
    volatile efuse_apb2otp_blk8_w11_reg_t apb2otp_blk8_w11;
    volatile efuse_apb2otp_blk9_w1_reg_t apb2otp_blk9_w1;
    volatile efuse_apb2otp_blk9_w2_reg_t apb2otp_blk9_w2;
    volatile efuse_apb2otp_blk9_w3_reg_t apb2otp_blk9_w3;
    volatile efuse_apb2otp_blk9_w4_reg_t apb2otp_blk9_w4;
    volatile efuse_apb2otp_blk9_w5_reg_t apb2otp_blk9_w5;
    volatile efuse_apb2otp_blk9_w6_reg_t apb2otp_blk9_w6;
    volatile efuse_apb2otp_blk9_w7_reg_t apb2otp_blk9_w7;
    volatile efuse_apb2otp_blk9_w8_reg_t apb2otp_blk9_w8;
    volatile efuse_apb2otp_blk9_w9_reg_t apb2otp_blk9_w9;
    volatile efuse_apb2otp_blk9_w10_reg_t apb2otp_blk9_w10;
    volatile efuse_apb2otp_blk9_w11_reg_t apb2otp_blk9_w11;
    volatile efuse_apb2otp_blk10_w1_reg_t apb2otp_blk10_w1;
    volatile efuse_apb2otp_blk10_w2_reg_t apb2otp_blk10_w2;
    volatile efuse_apb2otp_blk10_w3_reg_t apb2otp_blk10_w3;
    volatile efuse_apb2otp_blk10_w4_reg_t apb2otp_blk10_w4;
    volatile efuse_apb2otp_blk10_w5_reg_t apb2otp_blk10_w5;
    volatile efuse_apb2otp_blk10_w6_reg_t apb2otp_blk10_w6;
    volatile efuse_apb2otp_blk10_w7_reg_t apb2otp_blk10_w7;
    volatile efuse_apb2otp_blk10_w8_reg_t apb2otp_blk10_w8;
    volatile efuse_apb2otp_blk10_w9_reg_t apb2otp_blk10_w9;
    volatile efuse_apb2otp_blk10_w10_reg_t apb2otp_blk10_w10;
    volatile efuse_apb2otp_blk10_w11_reg_t apb2otp_blk10_w11;
    uint32_t reserved_a04;
    volatile efuse_apb2otp_en_reg_t apb2otp_en;
} efuse_dev_t;

extern efuse_dev_t EFUSE;

#ifndef __cplusplus
_Static_assert(sizeof(efuse_dev_t) == 0xa0c, "Invalid size of efuse_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
