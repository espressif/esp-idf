/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: program_data registers */
/** Type of pgm_datan register
 *  Represents pgm_datan
 */
typedef union {
    struct {
        /** pgm_data_n : R/W; bitpos: [31:0]; default: 0;
         *  Configures the nth 32-bit data to be programmed.
         */
        uint32_t pgm_data_n:32;
    };
    uint32_t val;
} efuse_pgm_datan_reg_t;

/** Type of pgm_check_valuen register
 *  Represents pgm_check_valuen
 */
typedef union {
    struct {
        /** pgm_rs_data_n : R/W; bitpos: [31:0]; default: 0;
         *  Configures the nth RS code to be programmed.
         */
        uint32_t pgm_rs_data_n:32;
    };
    uint32_t val;
} efuse_pgm_check_valuen_reg_t;


/** Group: block0 registers */
/** Type of rd_wr_dis register
 *  Represents rd_wr_dis
 */
typedef union {
    struct {
        /** wr_dis : RO; bitpos: [31:0]; default: 0;
         *  Represents whether programming of individual eFuse memory bit is disabled.  For
         *  mapping between the bits of this field and the eFuse memory bits, please refer to
         *  Table \ref{tab:efuse-block0-para} and Table \ref{tab:efuse-block-1-10-para}.
         *  1: Disabled
         *  0: Enabled
         */
        uint32_t wr_dis:32;
    };
    uint32_t val;
} efuse_rd_wr_dis_reg_t;

/** Type of rd_repeat_data0 register
 *  Represents rd_repeat_data
 */
typedef union {
    struct {
        /** rd_dis : RO; bitpos: [6:0]; default: 0;
         *  Represents whether reading of individual eFuse block(block4~block10) is disabled or
         *  enabled.
         *  1: disabled
         *  0: enabled
         */
        uint32_t rd_dis:7;
        /** recovery_bootloader_flash_sector_0_1 : RO; bitpos: [8:7]; default: 0;
         *  Represents the starting flash sector (flash sector size is 0x1000) of the recovery
         *  bootloader used by the ROM bootloader If the primary bootloader fails. 0 and 0xFFF
         *  - this feature is disabled.
         */
        uint32_t recovery_bootloader_flash_sector_0_1:2;
        /** dis_usb_jtag : RO; bitpos: [9]; default: 0;
         *  Set this bit to disable function of usb switch to jtag in module of usb device.
         */
        uint32_t dis_usb_jtag:1;
        /** recovery_bootloader_flash_sector_2_2 : RO; bitpos: [10]; default: 0;
         *  Represents the starting flash sector (flash sector size is 0x1000) of the recovery
         *  bootloader used by the ROM bootloader If the primary bootloader fails. 0 and 0xFFF
         *  - this feature is disabled.
         */
        uint32_t recovery_bootloader_flash_sector_2_2:1;
        uint32_t reserved_11:1;
        /** dis_force_download : RO; bitpos: [12]; default: 0;
         *  Set this bit to disable the function that forces chip into download mode.
         */
        uint32_t dis_force_download:1;
        /** spi_download_mspi_dis : RO; bitpos: [13]; default: 0;
         *  Set this bit to disable accessing MSPI flash/MSPI ram by SYS AXI matrix during
         *  boot_mode_download.
         */
        uint32_t spi_download_mspi_dis:1;
        /** dis_twai : RO; bitpos: [14]; default: 0;
         *  Set this bit to disable TWAI function.
         */
        uint32_t dis_twai:1;
        /** jtag_sel_enable : RO; bitpos: [15]; default: 0;
         *  Set this bit to enable selection between usb_to_jtag and pad_to_jtag through
         *  strapping gpio25 when both EFUSE_DIS_PAD_JTAG and EFUSE_DIS_USB_JTAG are equal to 0.
         */
        uint32_t jtag_sel_enable:1;
        /** soft_dis_jtag : RO; bitpos: [18:16]; default: 0;
         *  Set odd bits to disable JTAG in the soft way. JTAG can be enabled in HMAC module.
         */
        uint32_t soft_dis_jtag:3;
        /** dis_pad_jtag : RO; bitpos: [19]; default: 0;
         *  Set this bit to disable JTAG in the hard way. JTAG is disabled permanently.
         */
        uint32_t dis_pad_jtag:1;
        /** dis_download_manual_encrypt : RO; bitpos: [20]; default: 0;
         *  Set this bit to disable flash manual encrypt function (except in SPI boot mode).
         */
        uint32_t dis_download_manual_encrypt:1;
        /** recovery_bootloader_flash_sector_3_6 : RO; bitpos: [24:21]; default: 0;
         *  Represents the starting flash sector (flash sector size is 0x1000) of the recovery
         *  bootloader used by the ROM bootloader If the primary bootloader fails. 0 and 0xFFF
         *  - this feature is disabled.
         */
        uint32_t recovery_bootloader_flash_sector_3_6:4;
        /** usb_phy_sel : RO; bitpos: [25]; default: 0;
         *  0: intphy(gpio24/25) <---> usb_device
         *  1: intphy(26/27) <---> usb_otg11.1: intphy(gpio26/27) <---> usb_device
         *  1: intphy(24/25) <---> usb_otg11.
         */
        uint32_t usb_phy_sel:1;
        /** huk_gen_state : RO; bitpos: [30:26]; default: 0;
         *  Set the bits to control validation of HUK generate mode. Odd of 1 is invalid, even
         *  of 1 is valid.
         */
        uint32_t huk_gen_state:5;
        /** recovery_bootloader_flash_sector_7_7 : RO; bitpos: [31]; default: 0;
         *  Represents the starting flash sector (flash sector size is 0x1000) of the recovery
         *  bootloader used by the ROM bootloader If the primary bootloader fails. 0 and 0xFFF
         *  - this feature is disabled.
         */
        uint32_t recovery_bootloader_flash_sector_7_7:1;
    };
    uint32_t val;
} efuse_rd_repeat_data0_reg_t;

/** Type of rd_repeat_data1 register
 *  Represents rd_repeat_data
 */
typedef union {
    struct {
        /** recovery_bootloader_flash_sector_8_10 : RO; bitpos: [2:0]; default: 0;
         *  Represents the starting flash sector (flash sector size is 0x1000) of the recovery
         *  bootloader used by the ROM bootloader If the primary bootloader fails. 0 and 0xFFF
         *  - this feature is disabled.
         */
        uint32_t recovery_bootloader_flash_sector_8_10:3;
        /** recovery_bootloader_flash_sector_11_11 : RO; bitpos: [3]; default: 0;
         *  Represents the starting flash sector (flash sector size is 0x1000) of the recovery
         *  bootloader used by the ROM bootloader If the primary bootloader fails. 0 and 0xFFF
         *  - this feature is disabled.
         */
        uint32_t recovery_bootloader_flash_sector_11_11:1;
        /** km_rnd_switch_cycle : RO; bitpos: [4]; default: 0;
         *  Set the bits to control key manager random number switch cycle. 0: control by
         *  register. 1: 8 km clk cycles. 2: 16 km cycles. 3: 32 km cycles
         */
        uint32_t km_rnd_switch_cycle:1;
        /** km_deploy_only_once : RO; bitpos: [8:5]; default: 0;
         *  EFUSE_KM_DEPLOY_ONLY_ONCE and EFUSE_KM_DEPLOY_ONLY_ONCE_H together form one field:
         *  {EFUSE_KM_DEPLOY_ONLY_ONCE_H, EFUSE_KM_DEPLOY_ONLY_ONCE[3:0]}. Set each bit to
         *  control whether corresponding key can only be deployed once. 1 is true, 0 is false.
         *  bit 0: ecsda, bit 1: xts, bit2: hmac, bit3: ds, bit4:psram
         */
        uint32_t km_deploy_only_once:4;
        /** force_use_key_manager_key : RO; bitpos: [12:9]; default: 0;
         *  EFUSE_FORCE_USE_KEY_MANAGER_KEY and EFUSE_FORCE_USE_KEY_MANAGER_KEY_H together form
         *  one field: {EFUSE_FORCE_USE_KEY_MANAGER_KEY_H,
         *  EFUSE_FORCE_USE_KEY_MANAGER_KEY[3:0]}. Set each bit to control whether
         *  corresponding key must come from key manager. 1 is true, 0 is false. bit 0: ecsda,
         *  bit 1: xts, bit2: hmac, bit3: ds, bit4:psram
         */
        uint32_t force_use_key_manager_key:4;
        /** force_disable_sw_init_key : RO; bitpos: [13]; default: 0;
         *  Set this bit to disable software written init key, and force use efuse_init_key.
         */
        uint32_t force_disable_sw_init_key:1;
        /** km_xts_key_length_256 : RO; bitpos: [14]; default: 0;
         *  Set this bit to config flash encryption xts-512 key, else use xts-256 key when
         *  using the key manager
         */
        uint32_t km_xts_key_length_256:1;
        /** ecc_force_const_time : RO; bitpos: [15]; default: 0;
         *  Set this bit to permanently turn on ECC const-time mode.
         */
        uint32_t ecc_force_const_time:1;
        uint32_t reserved_16:1;
        /** wdt_delay_sel : RO; bitpos: [17]; default: 0;
         *  Select lp wdt timeout threshold at startup = initial timeout value * (2 ^
         *  (EFUSE_WDT_DELAY_SEL + 1))
         */
        uint32_t wdt_delay_sel:1;
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
 *  Represents rd_repeat_data
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
        /** sec_dpa_level : RO; bitpos: [17:16]; default: 0;
         *  Configures the clock random divide mode to determine the dpa secure level
         */
        uint32_t sec_dpa_level:2;
        uint32_t reserved_18:1;
        /** xts_dpa_clk_enable : RO; bitpos: [19]; default: 0;
         *  Sets this bit to enable xts clock anti-dpa attack function.
         */
        uint32_t xts_dpa_clk_enable:1;
        /** secure_boot_en : RO; bitpos: [20]; default: 0;
         *  Set this bit to enable secure boot.
         */
        uint32_t secure_boot_en:1;
        /** secure_boot_aggressive_revoke : RO; bitpos: [21]; default: 0;
         *  Set this bit to enable revoking aggressive secure boot.
         */
        uint32_t secure_boot_aggressive_revoke:1;
        /** km_deploy_only_once_h : RO; bitpos: [22]; default: 0;
         *  EFUSE_KM_DEPLOY_ONLY_ONCE and EFUSE_KM_DEPLOY_ONLY_ONCE_H together form one field:
         *  {EFUSE_KM_DEPLOY_ONLY_ONCE_H, EFUSE_KM_DEPLOY_ONLY_ONCE[3:0]}. Set each bit to
         *  control whether corresponding key can only be deployed once. 1 is true, 0 is false.
         *  bit 0: ecsda, bit 1: xts, bit2: hmac, bit3: ds, bit4:psram
         */
        uint32_t km_deploy_only_once_h:1;
        /** force_use_key_manager_key_h : RO; bitpos: [23]; default: 0;
         *  EFUSE_FORCE_USE_KEY_MANAGER_KEY and EFUSE_FORCE_USE_KEY_MANAGER_KEY_H together form
         *  one field: {EFUSE_FORCE_USE_KEY_MANAGER_KEY_H,
         *  EFUSE_FORCE_USE_KEY_MANAGER_KEY[3:0]}. Set each bit to control whether
         *  corresponding key must come from key manager. 1 is true, 0 is false. bit 0: ecsda,
         *  bit 1: xts, bit2: hmac, bit3: ds, bit4:psram
         */
        uint32_t force_use_key_manager_key_h:1;
        uint32_t reserved_24:2;
        /** flash_ecc_en : RO; bitpos: [26]; default: 0;
         *  Set this bit to enable ECC for flash boot.
         */
        uint32_t flash_ecc_en:1;
        /** dis_usb_otg_download_mode : RO; bitpos: [27]; default: 0;
         *  Set this bit to disable download via USB-OTG.
         */
        uint32_t dis_usb_otg_download_mode:1;
        /** flash_tpuw : RO; bitpos: [31:28]; default: 0;
         *  Configures flash waiting time after power-up, in unit of ms. When the value less
         *  than 15, the waiting time is the configurable value. Otherwise, the waiting time is
         *  30.
         */
        uint32_t flash_tpuw:4;
    };
    uint32_t val;
} efuse_rd_repeat_data2_reg_t;

/** Type of rd_repeat_data3 register
 *  Represents rd_repeat_data
 */
typedef union {
    struct {
        /** dis_download_mode : RO; bitpos: [0]; default: 0;
         *  Set this bit to disable download mode (boot_mode[3:0] = 0, 1, 2, 4, 5, 6, 7).
         */
        uint32_t dis_download_mode:1;
        /** dis_direct_boot : RO; bitpos: [1]; default: 0;
         *  Set this bit to disable direct boot mode
         */
        uint32_t dis_direct_boot:1;
        /** dis_usb_serial_jtag_rom_print : RO; bitpos: [2]; default: 0;
         *  Set this bit to disable USB-Serial-JTAG print during rom boot.
         */
        uint32_t dis_usb_serial_jtag_rom_print:1;
        /** lock_km_key : RO; bitpos: [3]; default: 0;
         *  set this bit to lock the key manager key after deploy
         */
        uint32_t lock_km_key:1;
        /** dis_usb_serial_jtag_download_mode : RO; bitpos: [4]; default: 0;
         *  Set this bit to disable the USB-Serial-JTAG download function.
         */
        uint32_t dis_usb_serial_jtag_download_mode:1;
        /** enable_security_download : RO; bitpos: [5]; default: 0;
         *  Set this bit to enable security download mode.
         */
        uint32_t enable_security_download:1;
        /** uart_print_control : RO; bitpos: [7:6]; default: 0;
         *  Set the type of UART printing, 00: force enable printing, 01: enable printing when
         *  GPIO8 is reset at low level, 10: enable printing when GPIO8 is reset at high level,
         *  11: force disable printing
         */
        uint32_t uart_print_control:2;
        /** force_send_resume : RO; bitpos: [8]; default: 0;
         *  Set this bit to force ROM code to send a resume command during SPI boot.
         */
        uint32_t force_send_resume:1;
        /** secure_version : RO; bitpos: [24:9]; default: 0;
         *  Secure version used by ESP-IDF anti-rollback feature.
         */
        uint32_t secure_version:16;
        /** secure_boot_disable_fast_wake : RO; bitpos: [25]; default: 0;
         *  Represents whether secure boot do fast verification on wake is disabled. 0: enabled
         *  1: disabled
         */
        uint32_t secure_boot_disable_fast_wake:1;
        /** hys_en_pad : RO; bitpos: [26]; default: 0;
         *  Set bits to enable hysteresis function of PAD0~27
         */
        uint32_t hys_en_pad:1;
        /** key_purpose_0_h : RO; bitpos: [27]; default: 0;
         *  Purpose of Key0. The 5-th bit.
         */
        uint32_t key_purpose_0_h:1;
        /** key_purpose_1_h : RO; bitpos: [28]; default: 0;
         *  Purpose of Key1. The 5-th bit.
         */
        uint32_t key_purpose_1_h:1;
        /** key_purpose_2_h : RO; bitpos: [29]; default: 0;
         *  Purpose of Key2. The 5-th bit.
         */
        uint32_t key_purpose_2_h:1;
        /** key_purpose_3_h : RO; bitpos: [30]; default: 0;
         *  Purpose of Key3. The 5-th bit.
         */
        uint32_t key_purpose_3_h:1;
        /** key_purpose_4_h : RO; bitpos: [31]; default: 0;
         *  Purpose of Key4. The 5-th bit.
         */
        uint32_t key_purpose_4_h:1;
    };
    uint32_t val;
} efuse_rd_repeat_data3_reg_t;

/** Type of rd_repeat_data4 register
 *  Represents rd_repeat_data
 */
typedef union {
    struct {
        /** pxa0_tieh_sel_0 : RO; bitpos: [1:0]; default: 0;
         *  Output LDO VO0 tieh source select. 0: 1'b1 1: sdmmc1 2: reg 3:sdmmc0
         */
        uint32_t pxa0_tieh_sel_0:2;
        /** pvt_glitch_en : RO; bitpos: [2]; default: 0;
         *  Represents whether to enable PVT power glitch monitor function.
         *  1:Enable.
         *  0:Disable
         */
        uint32_t pvt_glitch_en:1;
        uint32_t reserved_3:1;
        /** key_purpose_5_h : RO; bitpos: [4]; default: 0;
         *  Purpose of Key5. The 5-th bit.
         */
        uint32_t key_purpose_5_h:1;
        uint32_t reserved_5:2;
        /** km_disable_deploy_mode_h : RO; bitpos: [7]; default: 0;
         *  EFUSE_KM_DISABLE_DEPLOY_MODE and EFUSE_KM_DISABLE_DEPLOY_MODE_H together form one
         *  field: {EFUSE_KM_DISABLE_DEPLOY_MODE_H, EFUSE_KM_DISABLE_DEPLOY_MODE[3:0]}. Set
         *  each bit to control whether corresponding key's deploy mode of new value deployment
         *  is disabled. 1 is true, 0 is false. bit 0: ecsda, bit 1: xts, bit2: hmac, bit3: ds,
         *  bit4:psram
         */
        uint32_t km_disable_deploy_mode_h:1;
        /** km_disable_deploy_mode : RO; bitpos: [11:8]; default: 0;
         *  EFUSE_KM_DISABLE_DEPLOY_MODE and EFUSE_KM_DISABLE_DEPLOY_MODE_H together form one
         *  field: {EFUSE_KM_DISABLE_DEPLOY_MODE_H, EFUSE_KM_DISABLE_DEPLOY_MODE[3:0]}. Set
         *  each bit to control whether corresponding key's deploy mode of new value deployment
         *  is disabled. 1 is true, 0 is false. bit 0: ecsda, bit 1: xts, bit2: hmac, bit3: ds,
         *  bit4:psram
         */
        uint32_t km_disable_deploy_mode:4;
        uint32_t reserved_12:4;
        /** xts_dpa_pseudo_level : RO; bitpos: [17:16]; default: 0;
         *  Sets this bit to control the xts pseudo-round anti-dpa attack function. 0:
         *  controlled by register. 1-3: the higher the value is, the more pseudo-rounds are
         *  inserted to the xts-aes calculation
         */
        uint32_t xts_dpa_pseudo_level:2;
        /** hp_pwr_src_sel : RO; bitpos: [18]; default: 0;
         *  HP system power source select. 0:LDO  1: DCDC
         */
        uint32_t hp_pwr_src_sel:1;
        /** secure_boot_sha384_en : RO; bitpos: [19]; default: 0;
         *  Represents whether secure boot using SHA-384 is enabled. 0: disable 1: enable
         */
        uint32_t secure_boot_sha384_en:1;
        /** dis_wdt : RO; bitpos: [20]; default: 0;
         *  Set this bit to disable watch dog.
         */
        uint32_t dis_wdt:1;
        /** dis_swd : RO; bitpos: [21]; default: 0;
         *  Set bit to disable super-watchdog
         */
        uint32_t dis_swd:1;
        /** pvt_glitch_mode : RO; bitpos: [23:22]; default: 0;
         *  Use to configure glitch mode
         */
        uint32_t pvt_glitch_mode:2;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} efuse_rd_repeat_data4_reg_t;


/** Group: block1 registers */
/** Type of rd_mac_sys0 register
 *  Represents rd_mac_sys
 */
typedef union {
    struct {
        /** mac_0 : RO; bitpos: [31:0]; default: 0;
         *  Represents MAC address. Low 32-bit.
         */
        uint32_t mac_0:32;
    };
    uint32_t val;
} efuse_rd_mac_sys0_reg_t;

/** Type of rd_mac_sys1 register
 *  Represents rd_mac_sys
 */
typedef union {
    struct {
        /** mac_1 : RO; bitpos: [15:0]; default: 0;
         *  Represents MAC address. High 16-bit.
         */
        uint32_t mac_1:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} efuse_rd_mac_sys1_reg_t;

/** Type of rd_mac_sys3 register
 *  Represents rd_mac_sys
 */
typedef union {
    struct {
        uint32_t reserved_0:18;
        /** sys_data_part0_0 : RO; bitpos: [31:18]; default: 0;
         *  Represents the first 14-bit of zeroth part of system data.
         */
        uint32_t sys_data_part0_0:14;
    };
    uint32_t val;
} efuse_rd_mac_sys3_reg_t;

/** Type of rd_mac_sys4 register
 *  Represents rd_mac_sys
 */
typedef union {
    struct {
        /** sys_data_part0_1 : RO; bitpos: [31:0]; default: 0;
         *  Represents the second 32-bit of zeroth part of system data.
         */
        uint32_t sys_data_part0_1:32;
    };
    uint32_t val;
} efuse_rd_mac_sys4_reg_t;

/** Type of rd_mac_sys5 register
 *  Represents rd_mac_sys
 */
typedef union {
    struct {
        /** sys_data_part0_2 : RO; bitpos: [31:0]; default: 0;
         *  Represents the third 32-bit of zeroth part of system data.
         */
        uint32_t sys_data_part0_2:32;
    };
    uint32_t val;
} efuse_rd_mac_sys5_reg_t;


/** Group: block2 registers */
/** Type of rd_sys_part1_datan register
 *  Represents rd_sys_part1_datan
 */
typedef union {
    struct {
        /** sys_data_part1_n : RO; bitpos: [31:0]; default: 0;
         *  Represents the zeroth 32-bit of first part of system data.
         */
        uint32_t sys_data_part1_n:32;
    };
    uint32_t val;
} efuse_rd_sys_part1_datan_reg_t;


/** Group: block3 registers */
/** Type of rd_usr_datan register
 *  Represents rd_usr_datan
 */
typedef union {
    struct {
        /** usr_datan : RO; bitpos: [31:0]; default: 0;
         *  Represents the zeroth 32-bit of block3 (user).
         */
        uint32_t usr_datan:32;
    };
    uint32_t val;
} efuse_rd_usr_datan_reg_t;


/** Group: block4 registers */
/** Type of rd_key0_datan register
 *  Represents rd_key0_datan
 */
typedef union {
    struct {
        /** key0_datan : RO; bitpos: [31:0]; default: 0;
         *  Represents the zeroth 32-bit of key0.
         */
        uint32_t key0_datan:32;
    };
    uint32_t val;
} efuse_rd_key0_datan_reg_t;


/** Group: block5 registers */
/** Type of rd_key1_datan register
 *  Represents rd_key1_datan
 */
typedef union {
    struct {
        /** key1_datan : RO; bitpos: [31:0]; default: 0;
         *  Represents the zeroth 32-bit of key1.
         */
        uint32_t key1_datan:32;
    };
    uint32_t val;
} efuse_rd_key1_datan_reg_t;


/** Group: block6 registers */
/** Type of rd_key2_datan register
 *  Represents rd_key2_datan
 */
typedef union {
    struct {
        /** key2_datan : RO; bitpos: [31:0]; default: 0;
         *  Represents the zeroth 32-bit of key2.
         */
        uint32_t key2_datan:32;
    };
    uint32_t val;
} efuse_rd_key2_datan_reg_t;


/** Group: block7 registers */
/** Type of rd_key3_datan register
 *  Represents rd_key3_datan
 */
typedef union {
    struct {
        /** key3_datan : RO; bitpos: [31:0]; default: 0;
         *  Represents the zeroth 32-bit of key3.
         */
        uint32_t key3_datan:32;
    };
    uint32_t val;
} efuse_rd_key3_datan_reg_t;


/** Group: block8 registers */
/** Type of rd_key4_datan register
 *  Represents rd_key4_datan
 */
typedef union {
    struct {
        /** key4_datan : RO; bitpos: [31:0]; default: 0;
         *  Represents the zeroth 32-bit of key4.
         */
        uint32_t key4_datan:32;
    };
    uint32_t val;
} efuse_rd_key4_datan_reg_t;


/** Group: block9 registers */
/** Type of rd_key5_datan register
 *  Represents rd_key5_datan
 */
typedef union {
    struct {
        /** key5_datan : RO; bitpos: [31:0]; default: 0;
         *  Represents the zeroth 32-bit of key5.
         */
        uint32_t key5_datan:32;
    };
    uint32_t val;
} efuse_rd_key5_datan_reg_t;


/** Group: block10 registers */
/** Type of rd_sys_part2_data0 register
 *  Represents rd_sys_part2_data
 */
typedef union {
    struct {
        /** sys_data_part2_0 : RO; bitpos: [31:0]; default: 0;
         *  Represents the first 32-bit of second part of system data.
         */
        uint32_t sys_data_part2_0:32;
    };
    uint32_t val;
} efuse_rd_sys_part2_data0_reg_t;

/** Type of rd_sys_part2_data1 register
 *  Represents rd_sys_part2_data
 */
typedef union {
    struct {
        /** sys_data_part2_1 : RO; bitpos: [31:0]; default: 0;
         *  Represents the first 32-bit of second part of system data.
         */
        uint32_t sys_data_part2_1:32;
    };
    uint32_t val;
} efuse_rd_sys_part2_data1_reg_t;

/** Type of rd_sys_part2_data2 register
 *  Represents rd_sys_part2_data
 */
typedef union {
    struct {
        /** sys_data_part2_2 : RO; bitpos: [31:0]; default: 0;
         *  Represents the second 32-bit of second part of system data.
         */
        uint32_t sys_data_part2_2:32;
    };
    uint32_t val;
} efuse_rd_sys_part2_data2_reg_t;

/** Type of rd_sys_part2_data3 register
 *  Represents rd_sys_part2_data
 */
typedef union {
    struct {
        /** sys_data_part2_3 : RO; bitpos: [31:0]; default: 0;
         *  Represents the third 32-bit of second part of system data.
         */
        uint32_t sys_data_part2_3:32;
    };
    uint32_t val;
} efuse_rd_sys_part2_data3_reg_t;

/** Type of rd_sys_part2_data4 register
 *  Represents rd_sys_part2_data
 */
typedef union {
    struct {
        /** sys_data_part2_4 : RO; bitpos: [31:0]; default: 0;
         *  Represents the fourth 32-bit of second part of system data.
         */
        uint32_t sys_data_part2_4:32;
    };
    uint32_t val;
} efuse_rd_sys_part2_data4_reg_t;

/** Type of rd_sys_part2_data5 register
 *  Represents rd_sys_part2_data
 */
typedef union {
    struct {
        /** sys_data_part2_5 : RO; bitpos: [31:0]; default: 0;
         *  Represents the fifth 32-bit of second part of system data.
         */
        uint32_t sys_data_part2_5:32;
    };
    uint32_t val;
} efuse_rd_sys_part2_data5_reg_t;

/** Type of rd_sys_part2_data7 register
 *  Represents rd_sys_part2_data
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** usb_device_exchg_pins : RO; bitpos: [4]; default: 0;
         *  Enable usb device exchange pins of D+ and D-.
         */
        uint32_t usb_device_exchg_pins:1;
        /** usb_otg11_exchg_pins : RO; bitpos: [5]; default: 0;
         *  Enable usb otg11 exchange pins of D+ and D-.
         */
        uint32_t usb_otg11_exchg_pins:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} efuse_rd_sys_part2_data7_reg_t;


/** Group: block0 error report registers */
/** Type of rd_repeat_data_err0 register
 *  Represents rd_repeat_data_err
 */
typedef union {
    struct {
        /** rd_dis_err : RO; bitpos: [6:0]; default: 0;
         *  Represents the programming error of EFUSE_RD_DIS
         */
        uint32_t rd_dis_err:7;
        /** recovery_bootloader_flash_sector_0_1_err : RO; bitpos: [8:7]; default: 0;
         *  Represents the programming error of EFUSE_RECOVERY_BOOTLOADER_FLASH_SECTOR_0_1
         */
        uint32_t recovery_bootloader_flash_sector_0_1_err:2;
        /** dis_usb_jtag_err : RO; bitpos: [9]; default: 0;
         *  Represents the programming error of EFUSE_DIS_USB_JTAG
         */
        uint32_t dis_usb_jtag_err:1;
        /** recovery_bootloader_flash_sector_2_2_err : RO; bitpos: [10]; default: 0;
         *  Represents the programming error of EFUSE_RECOVERY_BOOTLOADER_FLASH_SECTOR_2_2
         */
        uint32_t recovery_bootloader_flash_sector_2_2_err:1;
        uint32_t reserved_11:1;
        /** dis_force_download_err : RO; bitpos: [12]; default: 0;
         *  Represents the programming error of EFUSE_DIS_FORCE_DOWNLOAD
         */
        uint32_t dis_force_download_err:1;
        /** spi_download_mspi_dis_err : RO; bitpos: [13]; default: 0;
         *  Represents the programming error of EFUSE_SPI_DOWNLOAD_MSPI_DIS
         */
        uint32_t spi_download_mspi_dis_err:1;
        /** dis_twai_err : RO; bitpos: [14]; default: 0;
         *  Represents the programming error of EFUSE_DIS_TWAI
         */
        uint32_t dis_twai_err:1;
        /** jtag_sel_enable_err : RO; bitpos: [15]; default: 0;
         *  Represents the programming error of EFUSE_JTAG_SEL_ENABLE
         */
        uint32_t jtag_sel_enable_err:1;
        /** soft_dis_jtag_err : RO; bitpos: [18:16]; default: 0;
         *  Represents the programming error of EFUSE_SOFT_DIS_JTAG
         */
        uint32_t soft_dis_jtag_err:3;
        /** dis_pad_jtag_err : RO; bitpos: [19]; default: 0;
         *  Represents the programming error of EFUSE_DIS_PAD_JTAG
         */
        uint32_t dis_pad_jtag_err:1;
        /** dis_download_manual_encrypt_err : RO; bitpos: [20]; default: 0;
         *  Represents the programming error of EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT
         */
        uint32_t dis_download_manual_encrypt_err:1;
        /** recovery_bootloader_flash_sector_3_6_err : RO; bitpos: [24:21]; default: 0;
         *  Represents the programming error of EFUSE_RECOVERY_BOOTLOADER_FLASH_SECTOR_3_6
         */
        uint32_t recovery_bootloader_flash_sector_3_6_err:4;
        /** usb_phy_sel_err : RO; bitpos: [25]; default: 0;
         *  Represents the programming error of EFUSE_USB_PHY_SEL
         */
        uint32_t usb_phy_sel_err:1;
        /** huk_gen_state_err : RO; bitpos: [30:26]; default: 0;
         *  Represents the programming error of EFUSE_HUK_GEN_STATE
         */
        uint32_t huk_gen_state_err:5;
        /** recovery_bootloader_flash_sector_7_7_err : RO; bitpos: [31]; default: 0;
         *  Represents the programming error of EFUSE_RECOVERY_BOOTLOADER_FLASH_SECTOR_7_7
         */
        uint32_t recovery_bootloader_flash_sector_7_7_err:1;
    };
    uint32_t val;
} efuse_rd_repeat_data_err0_reg_t;

/** Type of rd_repeat_data_err1 register
 *  Represents rd_repeat_data_err
 */
typedef union {
    struct {
        /** recovery_bootloader_flash_sector_8_10_err : RO; bitpos: [2:0]; default: 0;
         *  Represents the programming error of EFUSE_RECOVERY_BOOTLOADER_FLASH_SECTOR_8_10
         */
        uint32_t recovery_bootloader_flash_sector_8_10_err:3;
        /** recovery_bootloader_flash_sector_11_11_err : RO; bitpos: [3]; default: 0;
         *  Represents the programming error of EFUSE_RECOVERY_BOOTLOADER_FLASH_SECTOR_11_11
         */
        uint32_t recovery_bootloader_flash_sector_11_11_err:1;
        /** km_rnd_switch_cycle_err : RO; bitpos: [4]; default: 0;
         *  Represents the programming error of EFUSE_KM_RND_SWITCH_CYCLE
         */
        uint32_t km_rnd_switch_cycle_err:1;
        /** km_deploy_only_once_err : RO; bitpos: [8:5]; default: 0;
         *  Represents the programming error of EFUSE_KM_DEPLOY_ONLY_ONCE
         */
        uint32_t km_deploy_only_once_err:4;
        /** force_use_key_manager_key_err : RO; bitpos: [12:9]; default: 0;
         *  Represents the programming error of EFUSE_FORCE_USE_KEY_MANAGER_KEY
         */
        uint32_t force_use_key_manager_key_err:4;
        /** force_disable_sw_init_key_err : RO; bitpos: [13]; default: 0;
         *  Represents the programming error of EFUSE_FORCE_DISABLE_SW_INIT_KEY
         */
        uint32_t force_disable_sw_init_key_err:1;
        /** km_xts_key_length_256_err : RO; bitpos: [14]; default: 0;
         *  Represents the programming error of EFUSE_KM_XTS_KEY_LENGTH_256
         */
        uint32_t km_xts_key_length_256_err:1;
        /** ecc_force_const_time_err : RO; bitpos: [15]; default: 0;
         *  Represents the programming error of EFUSE_ECC_FORCE_CONST_TIME
         */
        uint32_t ecc_force_const_time_err:1;
        uint32_t reserved_16:1;
        /** wdt_delay_sel_err : RO; bitpos: [17]; default: 0;
         *  Represents the programming error of EFUSE_WDT_DELAY_SEL
         */
        uint32_t wdt_delay_sel_err:1;
        /** spi_boot_crypt_cnt_err : RO; bitpos: [20:18]; default: 0;
         *  Represents the programming error of EFUSE_SPI_BOOT_CRYPT_CNT
         */
        uint32_t spi_boot_crypt_cnt_err:3;
        /** secure_boot_key_revoke0_err : RO; bitpos: [21]; default: 0;
         *  Represents the programming error of EFUSE_SECURE_BOOT_KEY_REVOKE0
         */
        uint32_t secure_boot_key_revoke0_err:1;
        /** secure_boot_key_revoke1_err : RO; bitpos: [22]; default: 0;
         *  Represents the programming error of EFUSE_SECURE_BOOT_KEY_REVOKE1
         */
        uint32_t secure_boot_key_revoke1_err:1;
        /** secure_boot_key_revoke2_err : RO; bitpos: [23]; default: 0;
         *  Represents the programming error of EFUSE_SECURE_BOOT_KEY_REVOKE2
         */
        uint32_t secure_boot_key_revoke2_err:1;
        /** key_purpose_0_err : RO; bitpos: [27:24]; default: 0;
         *  Represents the programming error of EFUSE_KEY_PURPOSE_0
         */
        uint32_t key_purpose_0_err:4;
        /** key_purpose_1_err : RO; bitpos: [31:28]; default: 0;
         *  Represents the programming error of EFUSE_KEY_PURPOSE_1
         */
        uint32_t key_purpose_1_err:4;
    };
    uint32_t val;
} efuse_rd_repeat_data_err1_reg_t;

/** Type of rd_repeat_data_err2 register
 *  Represents rd_repeat_data_err
 */
typedef union {
    struct {
        /** key_purpose_2_err : RO; bitpos: [3:0]; default: 0;
         *  Represents the programming error of EFUSE_KEY_PURPOSE_2
         */
        uint32_t key_purpose_2_err:4;
        /** key_purpose_3_err : RO; bitpos: [7:4]; default: 0;
         *  Represents the programming error of EFUSE_KEY_PURPOSE_3
         */
        uint32_t key_purpose_3_err:4;
        /** key_purpose_4_err : RO; bitpos: [11:8]; default: 0;
         *  Represents the programming error of EFUSE_KEY_PURPOSE_4
         */
        uint32_t key_purpose_4_err:4;
        /** key_purpose_5_err : RO; bitpos: [15:12]; default: 0;
         *  Represents the programming error of EFUSE_KEY_PURPOSE_5
         */
        uint32_t key_purpose_5_err:4;
        /** sec_dpa_level_err : RO; bitpos: [17:16]; default: 0;
         *  Represents the programming error of EFUSE_SEC_DPA_LEVEL
         */
        uint32_t sec_dpa_level_err:2;
        uint32_t reserved_18:1;
        /** xts_dpa_clk_enable_err : RO; bitpos: [19]; default: 0;
         *  Represents the programming error of EFUSE_XTS_DPA_CLK_ENABLE
         */
        uint32_t xts_dpa_clk_enable_err:1;
        /** secure_boot_en_err : RO; bitpos: [20]; default: 0;
         *  Represents the programming error of EFUSE_SECURE_BOOT_EN
         */
        uint32_t secure_boot_en_err:1;
        /** secure_boot_aggressive_revoke_err : RO; bitpos: [21]; default: 0;
         *  Represents the programming error of EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE
         */
        uint32_t secure_boot_aggressive_revoke_err:1;
        /** km_deploy_only_once_h_err : RO; bitpos: [22]; default: 0;
         *  Represents the programming error of EFUSE_KM_DEPLOY_ONLY_ONCE_H
         */
        uint32_t km_deploy_only_once_h_err:1;
        /** force_use_key_manager_key_h_err : RO; bitpos: [23]; default: 0;
         *  Represents the programming error of EFUSE_FORCE_USE_KEY_MANAGER_KEY_H
         */
        uint32_t force_use_key_manager_key_h_err:1;
        uint32_t reserved_24:2;
        /** flash_ecc_en_err : RO; bitpos: [26]; default: 0;
         *  Represents the programming error of EFUSE_FLASH_ECC_EN
         */
        uint32_t flash_ecc_en_err:1;
        /** dis_usb_otg_download_mode_err : RO; bitpos: [27]; default: 0;
         *  Represents the programming error of EFUSE_DIS_USB_OTG_DOWNLOAD_MODE
         */
        uint32_t dis_usb_otg_download_mode_err:1;
        /** flash_tpuw_err : RO; bitpos: [31:28]; default: 0;
         *  Represents the programming error of EFUSE_FLASH_TPUW
         */
        uint32_t flash_tpuw_err:4;
    };
    uint32_t val;
} efuse_rd_repeat_data_err2_reg_t;

/** Type of rd_repeat_data_err3 register
 *  Represents rd_repeat_data_err
 */
typedef union {
    struct {
        /** dis_download_mode_err : RO; bitpos: [0]; default: 0;
         *  Represents the programming error of EFUSE_DIS_DOWNLOAD_MODE
         */
        uint32_t dis_download_mode_err:1;
        /** dis_direct_boot_err : RO; bitpos: [1]; default: 0;
         *  Represents the programming error of EFUSE_DIS_DIRECT_BOOT
         */
        uint32_t dis_direct_boot_err:1;
        /** dis_usb_serial_jtag_rom_print_err : RO; bitpos: [2]; default: 0;
         *  Represents the programming error of EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT
         */
        uint32_t dis_usb_serial_jtag_rom_print_err:1;
        /** lock_km_key_err : RO; bitpos: [3]; default: 0;
         *  Represents the programming error of EFUSE_LOCK_KM_KEY
         */
        uint32_t lock_km_key_err:1;
        /** dis_usb_serial_jtag_download_mode_err : RO; bitpos: [4]; default: 0;
         *  Represents the programming error of EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE
         */
        uint32_t dis_usb_serial_jtag_download_mode_err:1;
        /** enable_security_download_err : RO; bitpos: [5]; default: 0;
         *  Represents the programming error of EFUSE_ENABLE_SECURITY_DOWNLOAD
         */
        uint32_t enable_security_download_err:1;
        /** uart_print_control_err : RO; bitpos: [7:6]; default: 0;
         *  Represents the programming error of EFUSE_UART_PRINT_CONTROL
         */
        uint32_t uart_print_control_err:2;
        /** force_send_resume_err : RO; bitpos: [8]; default: 0;
         *  Represents the programming error of EFUSE_FORCE_SEND_RESUME
         */
        uint32_t force_send_resume_err:1;
        /** secure_version_err : RO; bitpos: [24:9]; default: 0;
         *  Represents the programming error of EFUSE_SECURE_VERSION
         */
        uint32_t secure_version_err:16;
        /** secure_boot_disable_fast_wake_err : RO; bitpos: [25]; default: 0;
         *  Represents the programming error of EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE
         */
        uint32_t secure_boot_disable_fast_wake_err:1;
        /** hys_en_pad_err : RO; bitpos: [26]; default: 0;
         *  Represents the programming error of EFUSE_HYS_EN_PAD
         */
        uint32_t hys_en_pad_err:1;
        /** key_purpose_0_h_err : RO; bitpos: [27]; default: 0;
         *  Represents the programming error of EFUSE_KEY_PURPOSE_0_H
         */
        uint32_t key_purpose_0_h_err:1;
        /** key_purpose_1_h_err : RO; bitpos: [28]; default: 0;
         *  Represents the programming error of EFUSE_KEY_PURPOSE_1_H
         */
        uint32_t key_purpose_1_h_err:1;
        /** key_purpose_2_h_err : RO; bitpos: [29]; default: 0;
         *  Represents the programming error of EFUSE_KEY_PURPOSE_2_H
         */
        uint32_t key_purpose_2_h_err:1;
        /** key_purpose_3_h_err : RO; bitpos: [30]; default: 0;
         *  Represents the programming error of EFUSE_KEY_PURPOSE_3_H
         */
        uint32_t key_purpose_3_h_err:1;
        /** key_purpose_4_h_err : RO; bitpos: [31]; default: 0;
         *  Represents the programming error of EFUSE_KEY_PURPOSE_4_H
         */
        uint32_t key_purpose_4_h_err:1;
    };
    uint32_t val;
} efuse_rd_repeat_data_err3_reg_t;

/** Type of rd_repeat_data_err4 register
 *  Represents rd_repeat_data_err
 */
typedef union {
    struct {
        /** pxa0_tieh_sel_0_err : RO; bitpos: [1:0]; default: 0;
         *  Represents the programming error of 0PXA_TIEH_SEL_0
         */
        uint32_t pxa0_tieh_sel_0_err:2;
        /** pvt_glitch_en_err : RO; bitpos: [2]; default: 0;
         *  Represents the programming error of EFUSE_PVT_GLITCH_EN
         */
        uint32_t pvt_glitch_en_err:1;
        uint32_t reserved_3:1;
        /** key_purpose_5_h_err : RO; bitpos: [4]; default: 0;
         *  Represents the programming error of EFUSE_KEY_PURPOSE_5_H
         */
        uint32_t key_purpose_5_h_err:1;
        uint32_t reserved_5:2;
        /** km_disable_deploy_mode_h_err : RO; bitpos: [7]; default: 0;
         *  Represents the programming error of EFUSE_KM_DISABLE_DEPLOY_MODE_H
         */
        uint32_t km_disable_deploy_mode_h_err:1;
        /** km_disable_deploy_mode_err : RO; bitpos: [11:8]; default: 0;
         *  Represents the programming error of EFUSE_KM_DISABLE_DEPLOY_MODE
         */
        uint32_t km_disable_deploy_mode_err:4;
        uint32_t reserved_12:4;
        /** xts_dpa_pseudo_level_err : RO; bitpos: [17:16]; default: 0;
         *  Represents the programming error of EFUSE_XTS_DPA_PSEUDO_LEVEL
         */
        uint32_t xts_dpa_pseudo_level_err:2;
        /** hp_pwr_src_sel_err : RO; bitpos: [18]; default: 0;
         *  Represents the programming error of EFUSE_HP_PWR_SRC_SEL
         */
        uint32_t hp_pwr_src_sel_err:1;
        /** secure_boot_sha384_en_err : RO; bitpos: [19]; default: 0;
         *  Represents the programming error of EFUSE_SECURE_BOOT_SHA384_EN
         */
        uint32_t secure_boot_sha384_en_err:1;
        /** dis_wdt_err : RO; bitpos: [20]; default: 0;
         *  Represents the programming error of EFUSE_DIS_WDT
         */
        uint32_t dis_wdt_err:1;
        /** dis_swd_err : RO; bitpos: [21]; default: 0;
         *  Represents the programming error of EFUSE_DIS_SWD
         */
        uint32_t dis_swd_err:1;
        /** pvt_glitch_mode_err : RO; bitpos: [23:22]; default: 0;
         *  Represents the programming error of EFUSE_PVT_GLITCH_MODE
         */
        uint32_t pvt_glitch_mode_err:2;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} efuse_rd_repeat_data_err4_reg_t;


/** Group: EFUSE ECDSA Configure Registers */
/** Type of ecdsa register
 *  eFuse status register.
 */
typedef union {
    struct {
        /** cfg_ecdsa_p192_blk : R/W; bitpos: [3:0]; default: 0;
         *  Configures which block to use for ECDSA P192 key output.
         */
        uint32_t cfg_ecdsa_p192_blk:4;
        /** cfg_ecdsa_p256_blk : R/W; bitpos: [7:4]; default: 0;
         *  Configures which block to use for ECDSA P256 key output.
         */
        uint32_t cfg_ecdsa_p256_blk:4;
        /** cfg_ecdsa_p384_l_blk : R/W; bitpos: [11:8]; default: 0;
         *  Configures which block to use for ECDSA P384 key low part output.
         */
        uint32_t cfg_ecdsa_p384_l_blk:4;
        /** cfg_ecdsa_p384_h_blk : R/W; bitpos: [15:12]; default: 0;
         *  Configures which block to use for ECDSA P256 key high part output.
         */
        uint32_t cfg_ecdsa_p384_h_blk:4;
        /** cur_ecdsa_p192_blk : RO; bitpos: [19:16]; default: 0;
         *  Represents which block is used for ECDSA P192 key output.
         */
        uint32_t cur_ecdsa_p192_blk:4;
        /** cur_ecdsa_p256_blk : RO; bitpos: [23:20]; default: 0;
         *  Represents which block is used for ECDSA P256 key output.
         */
        uint32_t cur_ecdsa_p256_blk:4;
        /** cur_ecdsa_p384_l_blk : RO; bitpos: [27:24]; default: 0;
         *  Represents which block is used for ECDSA P384 key low part output.
         */
        uint32_t cur_ecdsa_p384_l_blk:4;
        /** cur_ecdsa_p384_h_blk : RO; bitpos: [31:28]; default: 0;
         *  Represents which block is used for ECDSA P384 key high part output.
         */
        uint32_t cur_ecdsa_p384_h_blk:4;
    };
    uint32_t val;
} efuse_ecdsa_reg_t;


/** Group: RS block error report registers */
/** Type of rd_rs_data_err0 register
 *  Represents rd_rs_data_err
 */
typedef union {
    struct {
        /** rd_mac_sys_err_num : RO; bitpos: [2:0]; default: 0;
         *  Represents the error number of registers.
         *  The value of this signal means the number of error bytes in rd_mac_sys
         */
        uint32_t rd_mac_sys_err_num:3;
        /** rd_mac_sys_fail : RO; bitpos: [3]; default: 0;
         *  Represents error status of register.
         *  0: Means no failure and that the data of rd_mac_sys is reliable
         *  1: Means that programming rd_mac_sys failed and the number of error bytes is over 6.
         */
        uint32_t rd_mac_sys_fail:1;
        /** rd_sys_part1_data_err_num : RO; bitpos: [6:4]; default: 0;
         *  Represents the error number of registers.
         *  The value of this signal means the number of error bytes in rd_sys_part1_data
         */
        uint32_t rd_sys_part1_data_err_num:3;
        /** rd_sys_part1_data_fail : RO; bitpos: [7]; default: 0;
         *  Represents error status of register.
         *  0: Means no failure and that the data of rd_sys_part1_data is reliable
         *  1: Means that programming rd_sys_part1_data failed and the number of error bytes is
         *  over 6.
         */
        uint32_t rd_sys_part1_data_fail:1;
        /** rd_usr_data_err_num : RO; bitpos: [10:8]; default: 0;
         *  Represents the error number of registers.
         *  The value of this signal means the number of error bytes in rd_usr_data
         */
        uint32_t rd_usr_data_err_num:3;
        /** rd_usr_data_fail : RO; bitpos: [11]; default: 0;
         *  Represents error status of register.
         *  0: Means no failure and that the data of rd_usr_data is reliable
         *  1: Means that programming rd_usr_data failed and the number of error bytes is over
         *  6.
         */
        uint32_t rd_usr_data_fail:1;
        /** rd_key0_data_err_num : RO; bitpos: [14:12]; default: 0;
         *  Represents the error number of registers.
         *  The value of this signal means the number of error bytes in rd_key0_data
         */
        uint32_t rd_key0_data_err_num:3;
        /** rd_key0_data_fail : RO; bitpos: [15]; default: 0;
         *  Represents error status of register.
         *  0: Means no failure and that the data of rd_key0_data is reliable
         *  1: Means that programming rd_key0_data failed and the number of error bytes is over
         *  6.
         */
        uint32_t rd_key0_data_fail:1;
        /** rd_key1_data_err_num : RO; bitpos: [18:16]; default: 0;
         *  Represents the error number of registers.
         *  The value of this signal means the number of error bytes in rd_key1_data
         */
        uint32_t rd_key1_data_err_num:3;
        /** rd_key1_data_fail : RO; bitpos: [19]; default: 0;
         *  Represents error status of register.
         *  0: Means no failure and that the data of rd_key1_data is reliable
         *  1: Means that programming rd_key1_data failed and the number of error bytes is over
         *  6.
         */
        uint32_t rd_key1_data_fail:1;
        /** rd_key2_data_err_num : RO; bitpos: [22:20]; default: 0;
         *  Represents the error number of registers.
         *  The value of this signal means the number of error bytes in rd_key2_data
         */
        uint32_t rd_key2_data_err_num:3;
        /** rd_key2_data_fail : RO; bitpos: [23]; default: 0;
         *  Represents error status of register.
         *  0: Means no failure and that the data of rd_key2_data is reliable
         *  1: Means that programming rd_key2_data failed and the number of error bytes is over
         *  6.
         */
        uint32_t rd_key2_data_fail:1;
        /** rd_key3_data_err_num : RO; bitpos: [26:24]; default: 0;
         *  Represents the error number of registers.
         *  The value of this signal means the number of error bytes in rd_key3_data
         */
        uint32_t rd_key3_data_err_num:3;
        /** rd_key3_data_fail : RO; bitpos: [27]; default: 0;
         *  Represents error status of register.
         *  0: Means no failure and that the data of rd_key3_data is reliable
         *  1: Means that programming rd_key3_data failed and the number of error bytes is over
         *  6.
         */
        uint32_t rd_key3_data_fail:1;
        /** rd_key4_data_err_num : RO; bitpos: [30:28]; default: 0;
         *  Represents the error number of registers.
         *  The value of this signal means the number of error bytes in rd_key4_data
         */
        uint32_t rd_key4_data_err_num:3;
        /** rd_key4_data_fail : RO; bitpos: [31]; default: 0;
         *  Represents error status of register.
         *  0: Means no failure and that the data of rd_key4_data is reliable
         *  1: Means that programming rd_key4_data failed and the number of error bytes is over
         *  6.
         */
        uint32_t rd_key4_data_fail:1;
    };
    uint32_t val;
} efuse_rd_rs_data_err0_reg_t;

/** Type of rd_rs_data_err1 register
 *  Represents rd_rs_data_err
 */
typedef union {
    struct {
        /** rd_key5_data_err_num : RO; bitpos: [2:0]; default: 0;
         *  Represents the error number of registers.
         *  The value of this signal means the number of error bytes in rd_key5_data
         */
        uint32_t rd_key5_data_err_num:3;
        /** rd_key5_data_fail : RO; bitpos: [3]; default: 0;
         *  Represents error status of register.
         *  0: Means no failure and that the data of rd_key5_data is reliable
         *  1: Means that programming rd_key5_data failed and the number of error bytes is over
         *  6.
         */
        uint32_t rd_key5_data_fail:1;
        /** rd_sys_part2_data_err_num : RO; bitpos: [6:4]; default: 0;
         *  Represents the error number of registers.
         *  The value of this signal means the number of error bytes in rd_sys_part2_data
         */
        uint32_t rd_sys_part2_data_err_num:3;
        /** rd_sys_part2_data_fail : RO; bitpos: [7]; default: 0;
         *  Represents error status of register.
         *  0: Means no failure and that the data of rd_sys_part2_data is reliable
         *  1: Means that programming rd_sys_part2_data failed and the number of error bytes is
         *  over 6.
         */
        uint32_t rd_sys_part2_data_fail:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} efuse_rd_rs_data_err1_reg_t;


/** Group: ******** Registers */
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
 *  eFuse operation mode configuration register
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

/** Type of status register
 *  eFuse status register.
 */
typedef union {
    struct {
        /** state : RO; bitpos: [3:0]; default: 0;
         *  Indicates the state of the eFuse state machine.
         */
        uint32_t state:4;
        uint32_t reserved_4:6;
        /** blk0_valid_bit_cnt : RO; bitpos: [19:10]; default: 0;
         *  Indicates the number of block valid bit.
         */
        uint32_t blk0_valid_bit_cnt:10;
        uint32_t reserved_20:12;
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
        /** date : R/W; bitpos: [27:0]; default: 38805904;
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


/** Group: EFUSE_APB2OTP Function Enable Signal */
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
    volatile efuse_pgm_datan_reg_t pgm_datan[8];
    volatile efuse_pgm_check_valuen_reg_t pgm_check_valuen[3];
    volatile efuse_rd_wr_dis_reg_t rd_wr_dis;
    volatile efuse_rd_repeat_data0_reg_t rd_repeat_data0;
    volatile efuse_rd_repeat_data1_reg_t rd_repeat_data1;
    volatile efuse_rd_repeat_data2_reg_t rd_repeat_data2;
    volatile efuse_rd_repeat_data3_reg_t rd_repeat_data3;
    volatile efuse_rd_repeat_data4_reg_t rd_repeat_data4;
    volatile efuse_rd_mac_sys0_reg_t rd_mac_sys0;
    volatile efuse_rd_mac_sys1_reg_t rd_mac_sys1;
    uint32_t reserved_04c;
    volatile efuse_rd_mac_sys3_reg_t rd_mac_sys3;
    volatile efuse_rd_mac_sys4_reg_t rd_mac_sys4;
    volatile efuse_rd_mac_sys5_reg_t rd_mac_sys5;
    volatile efuse_rd_sys_part1_datan_reg_t rd_sys_part1_datan[8];
    volatile efuse_rd_usr_datan_reg_t rd_usr_datan[8];
    volatile efuse_rd_key0_datan_reg_t rd_key0_datan[8];
    volatile efuse_rd_key1_datan_reg_t rd_key1_datan[8];
    volatile efuse_rd_key2_datan_reg_t rd_key2_datan[8];
    volatile efuse_rd_key3_datan_reg_t rd_key3_datan[8];
    volatile efuse_rd_key4_datan_reg_t rd_key4_datan[8];
    volatile efuse_rd_key5_datan_reg_t rd_key5_datan[8];
    volatile efuse_rd_sys_part2_data0_reg_t rd_sys_part2_data0;
    volatile efuse_rd_sys_part2_data1_reg_t rd_sys_part2_data1;
    volatile efuse_rd_sys_part2_data2_reg_t rd_sys_part2_data2;
    volatile efuse_rd_sys_part2_data3_reg_t rd_sys_part2_data3;
    volatile efuse_rd_sys_part2_data4_reg_t rd_sys_part2_data4;
    volatile efuse_rd_sys_part2_data5_reg_t rd_sys_part2_data5;
    uint32_t reserved_174;
    volatile efuse_rd_sys_part2_data7_reg_t rd_sys_part2_data7;
    volatile efuse_rd_repeat_data_err0_reg_t rd_repeat_data_err0;
    volatile efuse_rd_repeat_data_err1_reg_t rd_repeat_data_err1;
    volatile efuse_rd_repeat_data_err2_reg_t rd_repeat_data_err2;
    volatile efuse_rd_repeat_data_err3_reg_t rd_repeat_data_err3;
    volatile efuse_rd_repeat_data_err4_reg_t rd_repeat_data_err4;
    uint32_t reserved_190[8];
    volatile efuse_ecdsa_reg_t ecdsa;
    uint32_t reserved_1b4[3];
    volatile efuse_rd_rs_data_err0_reg_t rd_rs_data_err0;
    volatile efuse_rd_rs_data_err1_reg_t rd_rs_data_err1;
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
