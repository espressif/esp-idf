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
         *  Represents whether reading of individual eFuse block(block4~block9) is disabled or
         *  enabled.
         *  1: disabled
         *  0: enabled
         */
        uint32_t rd_dis:7;
        uint32_t reserved_7:3;
        /** dis_usb_jtag : RO; bitpos: [10]; default: 0;
         *  Represents whether the function of usb switch to jtag is disabled or enabled.
         *  1: disabled
         *  0: enabled
         */
        uint32_t dis_usb_jtag:1;
        uint32_t reserved_11:1;
        /** dis_force_download : RO; bitpos: [12]; default: 0;
         *  Represents whether the function that forces chip into download mode is disabled or
         *  enabled.
         *  1: disabled
         *  0: enabled
         */
        uint32_t dis_force_download:1;
        /** spi_download_mspi_dis : RO; bitpos: [13]; default: 0;
         *  Represents whether SPI0 controller during boot_mode_download is disabled or enabled.
         *  1: disabled
         *  0: enabled
         */
        uint32_t spi_download_mspi_dis:1;
        /** dis_twai : RO; bitpos: [14]; default: 0;
         *  Represents whether TWAI function is disabled or enabled.
         *  1: disabled
         *  0: enabled
         */
        uint32_t dis_twai:1;
        /** jtag_sel_enable : RO; bitpos: [15]; default: 0;
         *  Represents whether the selection between usb_to_jtag and pad_to_jtag through
         *  strapping gpio15 when both EFUSE_DIS_PAD_JTAG and EFUSE_DIS_USB_JTAG are equal to 0
         *  is enabled or disabled.
         *  1: enabled
         *  0: disabled
         */
        uint32_t jtag_sel_enable:1;
        /** soft_dis_jtag : RO; bitpos: [18:16]; default: 0;
         *  Represents whether JTAG is disabled in soft way.
         *  Odd number: disabled
         *  Even number: enabled
         */
        uint32_t soft_dis_jtag:3;
        /** dis_pad_jtag : RO; bitpos: [19]; default: 0;
         *  Represents whether JTAG is disabled in the hard way(permanently).
         *  1: disabled
         *  0: enabled
         */
        uint32_t dis_pad_jtag:1;
        /** dis_download_manual_encrypt : RO; bitpos: [20]; default: 0;
         *  Represents whether flash encrypt function is disabled or enabled(except in SPI boot
         *  mode).
         *  1: disabled
         *  0: enabled
         */
        uint32_t dis_download_manual_encrypt:1;
        uint32_t reserved_21:2;
        /** huk_gen_state : RO; bitpos: [27:23]; default: 0;
         *  Represents the control of validation of HUK generate mode. Odd of 1 is invalid,
         *  even of 1 is valid.
         */
        uint32_t huk_gen_state:5;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} efuse_rd_repeat_data0_reg_t;

/** Type of rd_repeat_data1 register
 *  Represents rd_repeat_data
 */
typedef union {
    struct {
        /** km_rnd_switch_cycle : RO; bitpos: [0]; default: 0;
         *  Represents the control of key manager random number switch cycle. 0: control by
         *  register. 1: 8 km clk cycles. 2: 16 km cycles. 3: 32 km cycles
         */
        uint32_t km_rnd_switch_cycle:1;
        uint32_t reserved_1:1;
        /** km_disable_deploy_mode : RO; bitpos: [6:2]; default: 0;
         *  Represents whether the deploy mode of key manager is disable or not.
         *  1: disabled
         *  0: enabled.
         *  bit 0: ecsda, bit 1: flash & spi boot srambler, bit2: hmac & aes, bit3: ds & rma
         *  nonce, bit4: psram
         */
        uint32_t km_disable_deploy_mode:5;
        /** km_deploy_only_once : RO; bitpos: [11:7]; default: 0;
         *  Represents whether corresponding key can only be deployed once. 1 is true, 0 is
         *  false.
         *  0: ecsda
         *  1: flash & spi boot srambler
         *  2: hmac & aes
         *  3: ds & rma nonce
         *  4: psram
         */
        uint32_t km_deploy_only_once:5;
        /** force_use_key_manager_key : RO; bitpos: [16:12]; default: 0;
         *  Represents whether corresponding key must come from key manager. 1 is true, 0 is
         *  false.
         *  0: ecsda
         *  1: flash
         *  2: reserved
         *  3: reserved
         *  4: psram
         */
        uint32_t force_use_key_manager_key:5;
        /** force_disable_sw_init_key : RO; bitpos: [17]; default: 0;
         *  Represents whether to disable software written init key, and force use
         *  efuse_init_key.
         */
        uint32_t force_disable_sw_init_key:1;
        /** km_xts_key_length_256 : RO; bitpos: [18]; default: 0;
         *  Represents whether to configure flash encryption use xts-128 key. else use xts-256
         *  key.
         *  0: 128-bit key
         *  1: 256-bit key
         */
        uint32_t km_xts_key_length_256:1;
        /** wdt_delay_sel : RO; bitpos: [19]; default: 0;
         *  Represents the threshold level of the RTC watchdog STG0 timeout.
         *  0: Original threshold configuration value of STG0 *2
         *  1: Original threshold configuration value of STG0 *4
         *  2: Original threshold configuration value of STG0 *8
         *  3: Original threshold configuration value of STG0 *16
         */
        uint32_t wdt_delay_sel:1;
        /** dis_sm_crypt : RO; bitpos: [20]; default: 0;
         *  Represents whether to disable all the SM crypto functions, including SM2, SM3.
         *  1: disabled
         *  0: enabled
         */
        uint32_t dis_sm_crypt:1;
        /** spi_boot_crypt_cnt : RO; bitpos: [23:21]; default: 0;
         *  Represents whether SPI boot encrypt/decrypt is disabled or enabled.
         *  Odd number of 1: enabled
         *  Even number of 1: disabled
         */
        uint32_t spi_boot_crypt_cnt:3;
        /** secure_boot_key_revoke0 : RO; bitpos: [24]; default: 0;
         *  Represents whether revoking first secure boot key is enabled or disabled.
         *  1: enabled
         *  0: disabled
         */
        uint32_t secure_boot_key_revoke0:1;
        /** secure_boot_key_revoke1 : RO; bitpos: [25]; default: 0;
         *  Represents whether revoking second secure boot key is enabled or disabled.
         *  1: enabled
         *  0: disabled
         */
        uint32_t secure_boot_key_revoke1:1;
        /** secure_boot_key_revoke2 : RO; bitpos: [26]; default: 0;
         *  Represents whether revoking third secure boot key is enabled or disabled.
         *  1: enabled
         *  0: disabled
         */
        uint32_t secure_boot_key_revoke2:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} efuse_rd_repeat_data1_reg_t;

/** Type of rd_repeat_data2 register
 *  Represents rd_repeat_data
 */
typedef union {
    struct {
        /** key_purpose_0 : RO; bitpos: [4:0]; default: 0;
         *  Represents the purpose of Key0.
         */
        uint32_t key_purpose_0:5;
        /** key_purpose_1 : RO; bitpos: [9:5]; default: 0;
         *  Represents the purpose of Key1.
         */
        uint32_t key_purpose_1:5;
        /** key_purpose_2 : RO; bitpos: [14:10]; default: 0;
         *  Represents the purpose of Key2.
         */
        uint32_t key_purpose_2:5;
        /** key_purpose_3 : RO; bitpos: [19:15]; default: 0;
         *  Represents the purpose of Key3.
         */
        uint32_t key_purpose_3:5;
        /** key_purpose_4 : RO; bitpos: [24:20]; default: 0;
         *  Represents the purpose of Key4.
         */
        uint32_t key_purpose_4:5;
        /** ecc_force_const_time : RO; bitpos: [25]; default: 0;
         *  Represents whether permanently turn on ECC const-time mode.
         *  1: turn on
         *  0: turn off
         */
        uint32_t ecc_force_const_time:1;
        /** ecdsa_disable_soft_k : RO; bitpos: [26]; default: 0;
         *  Represents whether permanently turn off ECDSA software set KEY.
         *  1: turn off
         *  0: turn on
         */
        uint32_t ecdsa_disable_soft_k:1;
        /** sec_dpa_level : RO; bitpos: [28:27]; default: 0;
         *  Represents the spa secure level by configuring the clock random divide mode.
         */
        uint32_t sec_dpa_level:2;
        /** xts_dpa_clk_enable : RO; bitpos: [29]; default: 0;
         *  Represents whether to enable xts clock anti-dpa attack function.
         *  0: Disabled.
         *  1: Enabled
         */
        uint32_t xts_dpa_clk_enable:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} efuse_rd_repeat_data2_reg_t;

/** Type of rd_repeat_data3 register
 *  Represents rd_repeat_data
 */
typedef union {
    struct {
        /** xts_dpa_pseudo_level : RO; bitpos: [1:0]; default: 0;
         *  Represents the control of the xts pseudo-round anti-dpa attack function.
         *  0: controlled by register.
         *  1-3: the higher the value is, the more pseudo-rounds are inserted to the xts-aes
         *  calculation
         */
        uint32_t xts_dpa_pseudo_level:2;
        /** secure_boot_en : RO; bitpos: [2]; default: 0;
         *  Represents whether secure boot is enabled or disabled.
         *  1: enabled
         *  0: disabled
         */
        uint32_t secure_boot_en:1;
        /** secure_boot_aggressive_revoke : RO; bitpos: [3]; default: 0;
         *  Represents whether revoking aggressive secure boot is enabled or disabled.
         *  1: enabled.
         *  0: disabled
         */
        uint32_t secure_boot_aggressive_revoke:1;
        uint32_t reserved_4:1;
        /** flash_type : RO; bitpos: [5]; default: 0;
         *  flash type: 0: nor flash, 1: nand flash
         */
        uint32_t flash_type:1;
        uint32_t reserved_6:3;
        /** dis_usb_otg_download_mode : RO; bitpos: [9]; default: 0;
         *  Set this bit to disable download via USB-OTG.
         */
        uint32_t dis_usb_otg_download_mode:1;
        uint32_t reserved_10:2;
        /** flash_tpuw : RO; bitpos: [15:12]; default: 0;
         *  Represents the flash waiting time after power-up, in unit of ms. When the value
         *  less than 15, the waiting time is the programmed value. Otherwise, the waiting time
         *  is 2 times the programmed value.
         */
        uint32_t flash_tpuw:4;
        /** dis_download_mode : RO; bitpos: [16]; default: 0;
         *  Represents whether Download mode is disabled or enabled.
         *  1: disabled
         *  0: enabled
         */
        uint32_t dis_download_mode:1;
        /** dis_direct_boot : RO; bitpos: [17]; default: 0;
         *  Represents whether direct boot mode is disabled or enabled.
         *  1: disabled
         *  0: enabled
         */
        uint32_t dis_direct_boot:1;
        /** dis_usb_serial_jtag_rom_print : RO; bitpos: [18]; default: 0;
         *  Represents whether print from USB-Serial-JTAG is disabled or enabled.
         *  1: disabled
         *  0: enabled
         */
        uint32_t dis_usb_serial_jtag_rom_print:1;
        /** lock_km_key : RO; bitpos: [19]; default: 0;
         *  Represetns whether to lock the efuse xts key.
         *  1. Lock
         *  0: Unlock
         */
        uint32_t lock_km_key:1;
        /** dis_usb_serial_jtag_download_mode : RO; bitpos: [20]; default: 0;
         *  Represents whether the USB-Serial-JTAG download function is disabled or enabled.
         *  1: Disable
         *  0: Enable
         */
        uint32_t dis_usb_serial_jtag_download_mode:1;
        /** enable_security_download : RO; bitpos: [21]; default: 0;
         *  Represents whether security download is enabled or disabled.
         *  1: enabled
         *  0: disabled
         */
        uint32_t enable_security_download:1;
        /** uart_print_control : RO; bitpos: [23:22]; default: 0;
         *  Represents the type of UART printing.
         *  00: force enable printing
         *  01: enable printing when GPIO8 is reset at low level
         *  10: enable printing when GPIO8 is reset at high level
         *  11: force disable printing
         */
        uint32_t uart_print_control:2;
        /** force_send_resume : RO; bitpos: [24]; default: 0;
         *  Represents whether ROM code is forced to send a resume command during SPI boot.
         *  1: forced
         *  0:not forced
         */
        uint32_t force_send_resume:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} efuse_rd_repeat_data3_reg_t;

/** Type of rd_repeat_data4 register
 *  Represents rd_repeat_data
 */
typedef union {
    struct {
        /** secure_version : RO; bitpos: [15:0]; default: 0;
         *  Represents the version used by ESP-IDF anti-rollback feature.
         */
        uint32_t secure_version:16;
        /** secure_boot_disable_fast_wake : RO; bitpos: [16]; default: 0;
         *  Represents whether FAST VERIFY ON WAKE is disabled or enabled when Secure Boot is
         *  enabled.
         *  1: disabled
         *  0: enabled
         */
        uint32_t secure_boot_disable_fast_wake:1;
        /** hys_en_pad : RO; bitpos: [17]; default: 0;
         *  Represents whether the hysteresis function of corresponding PAD is enabled.
         *  1: enabled
         *  0:disabled
         */
        uint32_t hys_en_pad:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} efuse_rd_repeat_data4_reg_t;

/** Type of rd_repeat_data5 register
 *  Represents rd_repeat_data
 */
typedef union {
    struct {
        uint32_t reserved_0:2;
        /** dcdc_vset_en : RO; bitpos: [2]; default: 0;
         *  Select dcdc vset use efuse_dcdc_vset.
         */
        uint32_t dcdc_vset_en:1;
        /** dis_wdt : RO; bitpos: [3]; default: 0;
         *  Set this bit to disable watch dog.
         */
        uint32_t dis_wdt:1;
        /** dis_swd : RO; bitpos: [4]; default: 0;
         *  Set this bit to disable super-watchdog.
         */
        uint32_t dis_swd:1;
        uint32_t reserved_5:6;
        /** secure_boot_sha384_en : RO; bitpos: [11]; default: 0;
         *  Represents whether secure boot using SHA-384 is enabled. 0: Disable 1: Enable
         */
        uint32_t secure_boot_sha384_en:1;
        /** bootloader_anti_rollback_secure_version : RO; bitpos: [15:12]; default: 0;
         *  Represents the anti-rollback secure version of the 2nd stage bootloader used by the
         *  ROM bootloader.
         */
        uint32_t bootloader_anti_rollback_secure_version:4;
        /** bootloader_anti_rollback_en : RO; bitpos: [16]; default: 0;
         *  Represents whether the ani-rollback check for the 2nd stage bootloader is enabled.
         *  1: Enabled
         *  0: Disabled
         */
        uint32_t bootloader_anti_rollback_en:1;
        /** bootloader_anti_rollback_update_in_rom : RO; bitpos: [17]; default: 0;
         *  Represents whether the ani-rollback SECURE_VERSION will be updated from the ROM
         *  bootloader.
         *  1: Enable
         *  0: Disable
         */
        uint32_t bootloader_anti_rollback_update_in_rom:1;
        /** recovery_bootloader_flash_sector : RO; bitpos: [29:18]; default: 0;
         *  Represents the starting flash sector (flash sector size is 0x1000) of the recovery
         *  bootloader used by the ROM bootloader If the primary bootloader fails. 0 and 0xFFF
         *  - this feature is disabled.
         */
        uint32_t recovery_bootloader_flash_sector:12;
        /** rma_ena : RO; bitpos: [31:30]; default: 0;
         *  Represents whether rma function is supported in download mode.
         *  2'b01/2'b10: enabled
         *  2'b00/2'b11: disabled
         */
        uint32_t rma_ena:2;
    };
    uint32_t val;
} efuse_rd_repeat_data5_reg_t;

/** Type of rd_repeat_data6 register
 *  Represents rd_repeat_data
 */
typedef union {
    struct {
        /** rma_session_counter : RO; bitpos: [2:0]; default: 0;
         *  Represents the number of times the RMA session has been entered.
         */
        uint32_t rma_session_counter:3;
        /** rma_nonce_ena : RO; bitpos: [4:3]; default: 0;
         *  Represents whether random number NONCE is used in RMA and whether the KM module is
         *  used to generate the NONCE
         *  . 2'bx0: No NONCE
         *  2'b1x: Use KM generate NONCE.
         */
        uint32_t rma_nonce_ena:2;
        /** rma_chip_info_source : RO; bitpos: [5]; default: 0;
         *  Represents whether HUK_info is selected as the source for calculating CHIP_info in
         *  RMA.
         *  1: use HUK_info
         *  0: use UNIQ_id
         */
        uint32_t rma_chip_info_source:1;
        /** rma_disable_fast_vef : RO; bitpos: [6]; default: 0;
         *  Represents whether disable FAST_VEF in RMA session.
         *  1: disable
         *  0: enable
         */
        uint32_t rma_disable_fast_vef:1;
        /** pvt_0_glitch_en : RO; bitpos: [7]; default: 0;
         *  Represents whether to enable PVT power glitch monitor function.
         *  1:Enable.
         *  0:Disable
         */
        uint32_t pvt_0_glitch_en:1;
        /** pvt_0_glitch_mode : RO; bitpos: [9:8]; default: 0;
         *  Use to configure glitch mode
         */
        uint32_t pvt_0_glitch_mode:2;
        /** pvt_1_glitch_en : RO; bitpos: [10]; default: 0;
         *  Represents whether to enable PVT power glitch monitor function.
         *  1:Enable.
         *  0:Disable
         */
        uint32_t pvt_1_glitch_en:1;
        /** pvt_1_glitch_mode : RO; bitpos: [12:11]; default: 0;
         *  Use to configure glitch mode
         */
        uint32_t pvt_1_glitch_mode:2;
        /** pmu_flash_power_sel : RO; bitpos: [13]; default: 0;
         *  FLASH power select.
         *  1'b1: use 3.3V
         *  1'b0: use 1.8V
         */
        uint32_t pmu_flash_power_sel:1;
        /** pmu_flash_power_sel_en : RO; bitpos: [14]; default: 0;
         *  FLASH power select enable signal. 1'b1 : validates EFUSE_PMU_FLASH_POWER_SEL 1'b0:
         *  invalidates EFUSE_PMU_FLASH_POWER_SEL
         */
        uint32_t pmu_flash_power_sel_en:1;
        /** power_glitch_en : RO; bitpos: [18:15]; default: 0;
         *  set these bit enable power glitch enable
         */
        uint32_t power_glitch_en:4;
        /** ena_xts_shadow : RO; bitpos: [19]; default: 0;
         *  Represents whether to enable XTS-AES shadow core countermeasure against fault
         *  injection attacks.
         *  0: Disabled
         *  1: Enabled
         */
        uint32_t ena_xts_shadow:1;
        /** ena_spi_boot_crypt_scrambler : RO; bitpos: [20]; default: 0;
         *  Represents whether to enable ciphertext scrambler for external memory .
         *  0: Disabled
         *  1: Enabled
         */
        uint32_t ena_spi_boot_crypt_scrambler:1;
        /** re_enable_jtag_source : RO; bitpos: [21]; default: 0;
         *  Represents which Crypto peripheral is selected for re-enabling JTAG.
         *  0: RMA
         *  1: HMAC
         */
        uint32_t re_enable_jtag_source:1;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} efuse_rd_repeat_data6_reg_t;

/** Type of rd_repeat_data7 register
 *  Represents rd_repeat_data
 */
typedef union {
    struct {
        /** repeat7_rsvd : RO; bitpos: [15:0]; default: 0;
         *  Reserved.
         */
        uint32_t repeat7_rsvd:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} efuse_rd_repeat_data7_reg_t;


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
        /** mac_ext : RO; bitpos: [31:16]; default: 0;
         *  Represents the extended bits of MAC address.
         */
        uint32_t mac_ext:16;
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
/** Type of rd_sys_part2_data0 register
 *  Represents rd_sys_part2_data
 */
typedef union {
    struct {
        /** sys_data_part2_0 : RO; bitpos: [31:0]; default: 0;
         *  Represents the zeroth 32-bit of second part of system data.
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

/** Type of rd_sys_part2_data6 register
 *  Represents rd_sys_part2_data
 */
typedef union {
    struct {
        /** usb_device_exchg_pins : RO; bitpos: [0]; default: 0;
         *  Represents whether enable usb device exchange pins of D+ and D- or not.
         *  1: enabled
         *  0: disabled
         */
        uint32_t usb_device_exchg_pins:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} efuse_rd_sys_part2_data6_reg_t;


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
        uint32_t reserved_7:3;
        /** dis_usb_jtag_err : RO; bitpos: [10]; default: 0;
         *  Represents the programming error of EFUSE_DIS_USB_JTAG
         */
        uint32_t dis_usb_jtag_err:1;
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
        uint32_t reserved_21:2;
        /** huk_gen_state_err : RO; bitpos: [27:23]; default: 0;
         *  Represents the programming error of EFUSE_HUK_GEN_STATE
         */
        uint32_t huk_gen_state_err:5;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} efuse_rd_repeat_data_err0_reg_t;

/** Type of rd_repeat_data_err1 register
 *  Represents rd_repeat_data_err
 */
typedef union {
    struct {
        /** km_rnd_switch_cycle_err : RO; bitpos: [0]; default: 0;
         *  Represents the programming error of EFUSE_KM_RND_SWITCH_CYCLE
         */
        uint32_t km_rnd_switch_cycle_err:1;
        uint32_t reserved_1:1;
        /** km_disable_deploy_mode_err : RO; bitpos: [6:2]; default: 0;
         *  Represents the programming error of EFUSE_KM_DISABLE_DEPLOY_MODE
         */
        uint32_t km_disable_deploy_mode_err:5;
        /** km_deploy_only_once_err : RO; bitpos: [11:7]; default: 0;
         *  Represents the programming error of EFUSE_KM_DEPLOY_ONLY_ONCE
         */
        uint32_t km_deploy_only_once_err:5;
        /** force_use_key_manager_key_err : RO; bitpos: [16:12]; default: 0;
         *  Represents the programming error of EFUSE_FORCE_USE_KEY_MANAGER_KEY
         */
        uint32_t force_use_key_manager_key_err:5;
        /** force_disable_sw_init_key_err : RO; bitpos: [17]; default: 0;
         *  Represents the programming error of EFUSE_FORCE_DISABLE_SW_INIT_KEY
         */
        uint32_t force_disable_sw_init_key_err:1;
        /** km_xts_key_length_256_err : RO; bitpos: [18]; default: 0;
         *  Represents the programming error of EFUSE_KM_XTS_KEY_LENGTH_256
         */
        uint32_t km_xts_key_length_256_err:1;
        /** wdt_delay_sel_err : RO; bitpos: [19]; default: 0;
         *  Represents the programming error of EFUSE_WDT_DELAY_SEL
         */
        uint32_t wdt_delay_sel_err:1;
        /** dis_sm_crypt_err : RO; bitpos: [20]; default: 0;
         *  Represents the programming error of EFUSE_DIS_SM_CRYPT
         */
        uint32_t dis_sm_crypt_err:1;
        /** spi_boot_crypt_cnt_err : RO; bitpos: [23:21]; default: 0;
         *  Represents the programming error of EFUSE_SPI_BOOT_CRYPT_CNT
         */
        uint32_t spi_boot_crypt_cnt_err:3;
        /** secure_boot_key_revoke0_err : RO; bitpos: [24]; default: 0;
         *  Represents the programming error of EFUSE_SECURE_BOOT_KEY_REVOKE0
         */
        uint32_t secure_boot_key_revoke0_err:1;
        /** secure_boot_key_revoke1_err : RO; bitpos: [25]; default: 0;
         *  Represents the programming error of EFUSE_SECURE_BOOT_KEY_REVOKE1
         */
        uint32_t secure_boot_key_revoke1_err:1;
        /** secure_boot_key_revoke2_err : RO; bitpos: [26]; default: 0;
         *  Represents the programming error of EFUSE_SECURE_BOOT_KEY_REVOKE2
         */
        uint32_t secure_boot_key_revoke2_err:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} efuse_rd_repeat_data_err1_reg_t;

/** Type of rd_repeat_data_err2 register
 *  Represents rd_repeat_data_err
 */
typedef union {
    struct {
        /** key_purpose_0_err : RO; bitpos: [4:0]; default: 0;
         *  Represents the programming error of EFUSE_KEY_PURPOSE_0
         */
        uint32_t key_purpose_0_err:5;
        /** key_purpose_1_err : RO; bitpos: [9:5]; default: 0;
         *  Represents the programming error of EFUSE_KEY_PURPOSE_1
         */
        uint32_t key_purpose_1_err:5;
        /** key_purpose_2_err : RO; bitpos: [14:10]; default: 0;
         *  Represents the programming error of EFUSE_KEY_PURPOSE_2
         */
        uint32_t key_purpose_2_err:5;
        /** key_purpose_3_err : RO; bitpos: [19:15]; default: 0;
         *  Represents the programming error of EFUSE_KEY_PURPOSE_3
         */
        uint32_t key_purpose_3_err:5;
        /** key_purpose_4_err : RO; bitpos: [24:20]; default: 0;
         *  Represents the programming error of EFUSE_KEY_PURPOSE_4
         */
        uint32_t key_purpose_4_err:5;
        /** ecc_force_const_time_err : RO; bitpos: [25]; default: 0;
         *  Represents the programming error of EFUSE_ECC_FORCE_CONST_TIME
         */
        uint32_t ecc_force_const_time_err:1;
        /** ecdsa_disable_soft_k_err : RO; bitpos: [26]; default: 0;
         *  Represents the programming error of EFUSE_ECDSA_DISABLE_SOFT_K
         */
        uint32_t ecdsa_disable_soft_k_err:1;
        /** sec_dpa_level_err : RO; bitpos: [28:27]; default: 0;
         *  Represents the programming error of EFUSE_SEC_DPA_LEVEL
         */
        uint32_t sec_dpa_level_err:2;
        /** xts_dpa_clk_enable_err : RO; bitpos: [29]; default: 0;
         *  Represents the programming error of EFUSE_XTS_DPA_CLK_ENABLE
         */
        uint32_t xts_dpa_clk_enable_err:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} efuse_rd_repeat_data_err2_reg_t;

/** Type of rd_repeat_data_err3 register
 *  Represents rd_repeat_data_err
 */
typedef union {
    struct {
        /** xts_dpa_pseudo_level_err : RO; bitpos: [1:0]; default: 0;
         *  Represents the programming error of EFUSE_XTS_DPA_PSEUDO_LEVEL
         */
        uint32_t xts_dpa_pseudo_level_err:2;
        /** secure_boot_en_err : RO; bitpos: [2]; default: 0;
         *  Represents the programming error of EFUSE_SECURE_BOOT_EN
         */
        uint32_t secure_boot_en_err:1;
        /** secure_boot_aggressive_revoke_err : RO; bitpos: [3]; default: 0;
         *  Represents the programming error of EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE
         */
        uint32_t secure_boot_aggressive_revoke_err:1;
        uint32_t reserved_4:1;
        /** flash_type_err : RO; bitpos: [5]; default: 0;
         *  Represents the programming error of EFUSE_FLASH_TYPE
         */
        uint32_t flash_type_err:1;
        uint32_t reserved_6:3;
        /** dis_usb_otg_download_mode_err : RO; bitpos: [9]; default: 0;
         *  Represents the programming error of EFUSE_DIS_USB_OTG_DOWNLOAD_MODE
         */
        uint32_t dis_usb_otg_download_mode_err:1;
        uint32_t reserved_10:2;
        /** flash_tpuw_err : RO; bitpos: [15:12]; default: 0;
         *  Represents the programming error of EFUSE_FLASH_TPUW
         */
        uint32_t flash_tpuw_err:4;
        /** dis_download_mode_err : RO; bitpos: [16]; default: 0;
         *  Represents the programming error of EFUSE_DIS_DOWNLOAD_MODE
         */
        uint32_t dis_download_mode_err:1;
        /** dis_direct_boot_err : RO; bitpos: [17]; default: 0;
         *  Represents the programming error of EFUSE_DIS_DIRECT_BOOT
         */
        uint32_t dis_direct_boot_err:1;
        /** dis_usb_serial_jtag_rom_print_err : RO; bitpos: [18]; default: 0;
         *  Represents the programming error of EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT
         */
        uint32_t dis_usb_serial_jtag_rom_print_err:1;
        /** lock_km_key_err : RO; bitpos: [19]; default: 0;
         *  Represents the programming error of EFUSE_LOCK_KM_KEY
         */
        uint32_t lock_km_key_err:1;
        /** dis_usb_serial_jtag_download_mode_err : RO; bitpos: [20]; default: 0;
         *  Represents the programming error of EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE
         */
        uint32_t dis_usb_serial_jtag_download_mode_err:1;
        /** enable_security_download_err : RO; bitpos: [21]; default: 0;
         *  Represents the programming error of EFUSE_ENABLE_SECURITY_DOWNLOAD
         */
        uint32_t enable_security_download_err:1;
        /** uart_print_control_err : RO; bitpos: [23:22]; default: 0;
         *  Represents the programming error of EFUSE_UART_PRINT_CONTROL
         */
        uint32_t uart_print_control_err:2;
        /** force_send_resume_err : RO; bitpos: [24]; default: 0;
         *  Represents the programming error of EFUSE_FORCE_SEND_RESUME
         */
        uint32_t force_send_resume_err:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} efuse_rd_repeat_data_err3_reg_t;

/** Type of rd_repeat_data_err4 register
 *  Represents rd_repeat_data_err
 */
typedef union {
    struct {
        /** secure_version_err : RO; bitpos: [15:0]; default: 0;
         *  Represents the programming error of EFUSE_SECURE_VERSION
         */
        uint32_t secure_version_err:16;
        /** secure_boot_disable_fast_wake_err : RO; bitpos: [16]; default: 0;
         *  Represents the programming error of EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE
         */
        uint32_t secure_boot_disable_fast_wake_err:1;
        /** hys_en_pad_err : RO; bitpos: [17]; default: 0;
         *  Represents the programming error of EFUSE_HYS_EN_PAD
         */
        uint32_t hys_en_pad_err:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} efuse_rd_repeat_data_err4_reg_t;

/** Type of rd_repeat_data_err5 register
 *  Represents rd_repeat_data_err
 */
typedef union {
    struct {
        uint32_t reserved_0:2;
        /** dcdc_vset_en_err : RO; bitpos: [2]; default: 0;
         *  Represents the programming error of EFUSE_DCDC_VSET_EN
         */
        uint32_t dcdc_vset_en_err:1;
        /** dis_wdt_err : RO; bitpos: [3]; default: 0;
         *  Represents the programming error of EFUSE_DIS_WDT
         */
        uint32_t dis_wdt_err:1;
        /** dis_swd_err : RO; bitpos: [4]; default: 0;
         *  Represents the programming error of EFUSE_DIS_SWD
         */
        uint32_t dis_swd_err:1;
        uint32_t reserved_5:6;
        /** secure_boot_sha384_en_err : RO; bitpos: [11]; default: 0;
         *  Represents the programming error of EFUSE_SECURE_BOOT_SHA384_EN
         */
        uint32_t secure_boot_sha384_en_err:1;
        /** bootloader_anti_rollback_secure_version_err : RO; bitpos: [15:12]; default: 0;
         *  Represents the programming error of EFUSE_BOOTLOADER_ANTI_ROLLBACK_SECURE_VERSION
         */
        uint32_t bootloader_anti_rollback_secure_version_err:4;
        /** bootloader_anti_rollback_en_err : RO; bitpos: [16]; default: 0;
         *  Represents the programming error of EFUSE_BOOTLOADER_ANTI_ROLLBACK_EN
         */
        uint32_t bootloader_anti_rollback_en_err:1;
        /** bootloader_anti_rollback_update_in_rom_err : RO; bitpos: [17]; default: 0;
         *  Represents the programming error of EFUSE_BOOTLOADER_ANTI_ROLLBACK_UPDATE_IN_ROM
         */
        uint32_t bootloader_anti_rollback_update_in_rom_err:1;
        /** recovery_bootloader_flash_sector_err : RO; bitpos: [29:18]; default: 0;
         *  Represents the programming error of EFUSE_RECOVERY_BOOTLOADER_FLASH_SECTOR
         */
        uint32_t recovery_bootloader_flash_sector_err:12;
        /** rma_ena_err : RO; bitpos: [31:30]; default: 0;
         *  Represents the programming error of EFUSE_RMA_ENA
         */
        uint32_t rma_ena_err:2;
    };
    uint32_t val;
} efuse_rd_repeat_data_err5_reg_t;

/** Type of rd_repeat_data_err6 register
 *  Represents rd_repeat_data_err
 */
typedef union {
    struct {
        /** rma_session_counter_err : RO; bitpos: [2:0]; default: 0;
         *  Represents the programming error of EFUSE_RMA_SESSION_COUNTER
         */
        uint32_t rma_session_counter_err:3;
        /** rma_nonce_ena_err : RO; bitpos: [4:3]; default: 0;
         *  Represents the programming error of EFUSE_RMA_NONCE_ENA
         */
        uint32_t rma_nonce_ena_err:2;
        /** rma_chip_info_source_err : RO; bitpos: [5]; default: 0;
         *  Represents the programming error of EFUSE_RMA_CHIP_INFO_SOURCE
         */
        uint32_t rma_chip_info_source_err:1;
        /** rma_disable_fast_vef_err : RO; bitpos: [6]; default: 0;
         *  Represents the programming error of EFUSE_RMA_DISABLE_FAST_VEF
         */
        uint32_t rma_disable_fast_vef_err:1;
        /** pvt_0_glitch_en_err : RO; bitpos: [7]; default: 0;
         *  Represents the programming error of EFUSE_PVT_0_GLITCH_EN
         */
        uint32_t pvt_0_glitch_en_err:1;
        /** pvt_0_glitch_mode_err : RO; bitpos: [9:8]; default: 0;
         *  Represents the programming error of EFUSE_PVT_0_GLITCH_MODE
         */
        uint32_t pvt_0_glitch_mode_err:2;
        /** pvt_1_glitch_en_err : RO; bitpos: [10]; default: 0;
         *  Represents the programming error of EFUSE_PVT_1_GLITCH_EN
         */
        uint32_t pvt_1_glitch_en_err:1;
        /** pvt_1_glitch_mode_err : RO; bitpos: [12:11]; default: 0;
         *  Represents the programming error of EFUSE_PVT_1_GLITCH_MODE
         */
        uint32_t pvt_1_glitch_mode_err:2;
        /** pmu_flash_power_sel_err : RO; bitpos: [13]; default: 0;
         *  Represents the programming error of EFUSE_PMU_FLASH_POWER_SEL
         */
        uint32_t pmu_flash_power_sel_err:1;
        /** pmu_flash_power_sel_en_err : RO; bitpos: [14]; default: 0;
         *  Represents the programming error of EFUSE_PMU_FLASH_POWER_SEL_EN
         */
        uint32_t pmu_flash_power_sel_en_err:1;
        /** power_glitch_en_err : RO; bitpos: [18:15]; default: 0;
         *  Represents the programming error of EFUSE_POWER_GLITCH_EN
         */
        uint32_t power_glitch_en_err:4;
        /** ena_xts_shadow_err : RO; bitpos: [19]; default: 0;
         *  Represents the programming error of EFUSE_ENA_XTS_SHADOW
         */
        uint32_t ena_xts_shadow_err:1;
        /** ena_spi_boot_crypt_scrambler_err : RO; bitpos: [20]; default: 0;
         *  Represents the programming error of EFUSE_ENA_SPI_BOOT_CRYPT_SCRAMBLER
         */
        uint32_t ena_spi_boot_crypt_scrambler_err:1;
        /** re_enable_jtag_source_err : RO; bitpos: [21]; default: 0;
         *  Represents the programming error of EFUSE_RE_ENABLE_JTAG_SOURCE
         */
        uint32_t re_enable_jtag_source_err:1;
        uint32_t reserved_22:10;
    };
    uint32_t val;
} efuse_rd_repeat_data_err6_reg_t;

/** Type of rd_repeat_data_err7 register
 *  Represents rd_repeat_data_err
 */
typedef union {
    struct {
        /** repeat7_rsvd_err : RO; bitpos: [15:0]; default: 0;
         *  Represents the programming error of EFUSE_REPEAT7_RSVD
         */
        uint32_t repeat7_rsvd_err:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} efuse_rd_repeat_data_err7_reg_t;


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
        /** rd_sys_part2_data_err_num : RO; bitpos: [2:0]; default: 0;
         *  Represents the error number of registers.
         *  The value of this signal means the number of error bytes in rd_sys_part2_data
         */
        uint32_t rd_sys_part2_data_err_num:3;
        /** rd_sys_part2_data_fail : RO; bitpos: [3]; default: 0;
         *  Represents error status of register.
         *  0: Means no failure and that the data of rd_sys_part2_data is reliable
         *  1: Means that programming rd_sys_part2_data failed and the number of error bytes is
         *  over 6.
         */
        uint32_t rd_sys_part2_data_fail:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} efuse_rd_rs_data_err1_reg_t;


/** Group: EFUSE Version Register */
/** Type of date register
 *  eFuse version register.
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 38834580;
         *  Represents eFuse version. Date:2025-09-19 19:52:16,
         *  ScriptRev:abfb97fc087287a4eacd02828cb3ca8dd0d0e75f
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} efuse_date_reg_t;


/** Group: EFUSE Clock Registers */
/** Type of clk register
 *  eFuse clcok configuration register.
 */
typedef union {
    struct {
        /** mem_force_pd : R/W; bitpos: [0]; default: 0;
         *  Configures whether to force power down eFuse SRAM.
         *  1: Force
         *  0: No effect
         */
        uint32_t mem_force_pd:1;
        /** mem_clk_force_on : R/W; bitpos: [1]; default: 1;
         *  Configures whether to force activate clock signal of eFuse SRAM.
         *  1: Force activate
         *  0: No effect
         */
        uint32_t mem_clk_force_on:1;
        /** mem_force_pu : R/W; bitpos: [2]; default: 0;
         *  Configures whether to force power up eFuse SRAM.
         *  1: Force
         *  0: No effect
         */
        uint32_t mem_force_pu:1;
        uint32_t reserved_3:13;
        /** clk_en : R/W; bitpos: [16]; default: 0;
         *  Configures whether to force enable eFuse register configuration clock signal.
         *  1: Force
         *  0: The clock is enabled only during the reading and writing of registers
         */
        uint32_t clk_en:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} efuse_clk_reg_t;


/** Group: EFUSE Configure Registers */
/** Type of conf register
 *  eFuse operation mode configuration register
 */
typedef union {
    struct {
        /** op_code : R/W; bitpos: [15:0]; default: 0;
         *  Configures operation command type.
         *  0x5A5A: Program operation command
         *  0x5AA5: Read operation command
         *  Other values: No effect
         */
        uint32_t op_code:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} efuse_conf_reg_t;

/** Type of dac_conf register
 *  Controls the eFuse programming voltage.
 */
typedef union {
    struct {
        /** dac_clk_div : R/W; bitpos: [7:0]; default: 24;
         *  Configures the division factor of the rising clock of the programming voltage
         *  (ramp-up slew rate be slower than 2.5V/250us, and ramp-up 10mV for one-cycle, so
         *  reg_dac_clk_div > fclk).
         */
        uint32_t dac_clk_div:8;
        /** dac_clk_pad_sel : R/W; bitpos: [8]; default: 0;
         *  Don't care.
         */
        uint32_t dac_clk_pad_sel:1;
        /** dac_num : R/W; bitpos: [16:9]; default: 255;
         *  Configures clock cycles for programming voltage to rise (VDDQ 2.5V, ramp-up 10mV
         *  for one-cycle). Measurement unit: a clock cycle divided by EFUSE_DAC_CLK_DIV.
         */
        uint32_t dac_num:8;
        /** oe_clr : R/W; bitpos: [17]; default: 0;
         *  Configures whether to reduce the power supply of programming voltage.
         *  0: Not reduce
         *  1: Reduce
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
         *  Configures the read hold time (>=14ns). Measurement unit: One cycle of the eFuse
         *  core clock.
         */
        uint32_t thr_a:8;
        /** trd : R/W; bitpos: [15:8]; default: 2;
         *  Configures the read time (>=52ns). Measurement unit: One cycle of the eFuse core
         *  clock.
         */
        uint32_t trd:8;
        /** tsur_a : R/W; bitpos: [23:16]; default: 1;
         *  Configures the read setup time (>=14ns). Measurement unit: One cycle of the eFuse
         *  core clock.
         */
        uint32_t tsur_a:8;
        /** read_init_num : R/W; bitpos: [31:24]; default: 27;
         *  Configures the waiting time of reading eFuse memory. Measurement unit: One cycle of
         *  the eFuse core clock.
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
         *  Configures the programming setup time (>14ns). Measurement unit: One cycle of the
         *  eFuse core clock.
         */
        uint32_t tsup_a:8;
        /** pwr_on_num : R/W; bitpos: [23:8]; default: 14746;
         *  Configures the power up time for VDDQ (>reg_dac_clk_div*reg_dac_clk_div).
         *  Measurement unit: One cycle of the eFuse core clock.
         */
        uint32_t pwr_on_num:16;
        /** thp_a : R/W; bitpos: [31:24]; default: 1;
         *  Configures the programming hold time (>=11ns). Measurement unit: One cycle of the
         *  eFuse core clock.
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
        /** pwr_off_num : R/W; bitpos: [15:0]; default: 480;
         *  Configures the power outage time for VDDQ. Measurement unit: One cycle of the eFuse
         *  core clock.
         */
        uint32_t pwr_off_num:16;
        /** tpgm : R/W; bitpos: [31:16]; default: 240;
         *  Configures the active programming time (>10us). Measurement unit: One cycle of the
         *  eFuse core clock.
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
         *  Configures whether to bypass the Reed-Solomon (RS) correction step.
         *  0: Not bypass
         *  1: Bypass
         */
        uint32_t bypass_rs_correction:1;
        /** bypass_rs_blk_num : R/W; bitpos: [11:1]; default: 0;
         *  Configures which block number to bypass the Reed-Solomon (RS) correction step.
         */
        uint32_t bypass_rs_blk_num:11;
        /** update : WT; bitpos: [12]; default: 0;
         *  Configures whether to update multi-bit register signals.
         *  1: Update
         *  0: No effect
         */
        uint32_t update:1;
        /** tpgm_inactive : R/W; bitpos: [20:13]; default: 1;
         *  Configures the inactive programming time. Measurement unit: One cycle of the eFuse
         *  core clock.
         */
        uint32_t tpgm_inactive:8;
        uint32_t reserved_21:11;
    };
    uint32_t val;
} efuse_wr_tim_conf0_rs_bypass_reg_t;


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


/** Group: EFUSE Status Registers */
/** Type of status register
 *  eFuse status register.
 */
typedef union {
    struct {
        /** state : RO; bitpos: [3:0]; default: 0;
         *  Represents the state of the eFuse state machine.
         *  0: Reset state, the initial state after power-up
         *  1: Idle state
         *  Other values: Non-idle state
         */
        uint32_t state:4;
        uint32_t reserved_4:6;
        /** blk0_valid_bit_cnt : RO; bitpos: [19:10]; default: 0;
         *  Represents the number of block valid bit.
         */
        uint32_t blk0_valid_bit_cnt:10;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} efuse_status_reg_t;


/** Group: EFUSE Command Registers */
/** Type of cmd register
 *  eFuse command register.
 */
typedef union {
    struct {
        /** read_cmd : R/W/SC; bitpos: [0]; default: 0;
         *  Configures whether to send read commands.
         *  1: Send
         *  0: No effect
         */
        uint32_t read_cmd:1;
        /** pgm_cmd : R/W/SC; bitpos: [1]; default: 0;
         *  Configures whether to send programming commands.
         *  1: Send
         *  0: No effect
         */
        uint32_t pgm_cmd:1;
        /** blk_num : R/W; bitpos: [5:2]; default: 0;
         *  Configures the serial number of the block to be programmed. Value 0-10 corresponds
         *  to block number 0-10, respectively.
         */
        uint32_t blk_num:4;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} efuse_cmd_reg_t;


/** Group: Interrupt Registers */
/** Type of int_raw register
 *  eFuse raw interrupt register.
 */
typedef union {
    struct {
        /** read_done_int_raw : R/SS/WTC; bitpos: [0]; default: 0;
         *  The raw interrupt status of EFUSE_READ_DONE_INT.
         */
        uint32_t read_done_int_raw:1;
        /** pgm_done_int_raw : R/SS/WTC; bitpos: [1]; default: 0;
         *  The raw interrupt status of EFUSE_PGM_DONE_INT.
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
         *  The masked interrupt status of EFUSE_READ_DONE_INT.
         */
        uint32_t read_done_int_st:1;
        /** pgm_done_int_st : RO; bitpos: [1]; default: 0;
         *  The masked interrupt status of EFUSE_PGM_DONE_INT.
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
         *  Write 1 to enable EFUSE_READ_DONE_INT.
         */
        uint32_t read_done_int_ena:1;
        /** pgm_done_int_ena : R/W; bitpos: [1]; default: 0;
         *  Write 1 to enable EFUSE_PGM_DONE_INT.
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
         *  Write 1 to clear EFUSE_READ_DONE_INT.
         */
        uint32_t read_done_int_clr:1;
        /** pgm_done_int_clr : WT; bitpos: [1]; default: 0;
         *  Write 1 to clear EFUSE_PGM_DONE_INT.
         */
        uint32_t pgm_done_int_clr:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} efuse_int_clr_reg_t;


typedef struct {
    volatile efuse_pgm_datan_reg_t pgm_datan[8];
    volatile efuse_pgm_check_valuen_reg_t pgm_check_valuen[3];
    volatile efuse_rd_wr_dis_reg_t rd_wr_dis;
    volatile efuse_rd_repeat_data0_reg_t rd_repeat_data0;
    volatile efuse_rd_repeat_data1_reg_t rd_repeat_data1;
    volatile efuse_rd_repeat_data2_reg_t rd_repeat_data2;
    volatile efuse_rd_repeat_data3_reg_t rd_repeat_data3;
    volatile efuse_rd_repeat_data4_reg_t rd_repeat_data4;
    volatile efuse_rd_repeat_data5_reg_t rd_repeat_data5;
    volatile efuse_rd_repeat_data6_reg_t rd_repeat_data6;
    volatile efuse_rd_repeat_data7_reg_t rd_repeat_data7;
    volatile efuse_rd_mac_sys0_reg_t rd_mac_sys0;
    volatile efuse_rd_mac_sys1_reg_t rd_mac_sys1;
    uint32_t reserved_058;
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
    volatile efuse_rd_sys_part2_data0_reg_t rd_sys_part2_data0;
    volatile efuse_rd_sys_part2_data1_reg_t rd_sys_part2_data1;
    volatile efuse_rd_sys_part2_data2_reg_t rd_sys_part2_data2;
    volatile efuse_rd_sys_part2_data3_reg_t rd_sys_part2_data3;
    volatile efuse_rd_sys_part2_data4_reg_t rd_sys_part2_data4;
    volatile efuse_rd_sys_part2_data5_reg_t rd_sys_part2_data5;
    volatile efuse_rd_sys_part2_data6_reg_t rd_sys_part2_data6;
    uint32_t reserved_164;
    volatile efuse_rd_repeat_data_err0_reg_t rd_repeat_data_err0;
    volatile efuse_rd_repeat_data_err1_reg_t rd_repeat_data_err1;
    volatile efuse_rd_repeat_data_err2_reg_t rd_repeat_data_err2;
    volatile efuse_rd_repeat_data_err3_reg_t rd_repeat_data_err3;
    volatile efuse_rd_repeat_data_err4_reg_t rd_repeat_data_err4;
    volatile efuse_rd_repeat_data_err5_reg_t rd_repeat_data_err5;
    volatile efuse_rd_repeat_data_err6_reg_t rd_repeat_data_err6;
    volatile efuse_rd_repeat_data_err7_reg_t rd_repeat_data_err7;
    volatile efuse_rd_rs_data_err0_reg_t rd_rs_data_err0;
    volatile efuse_rd_rs_data_err1_reg_t rd_rs_data_err1;
    volatile efuse_date_reg_t date;
    uint32_t reserved_194[13];
    volatile efuse_clk_reg_t clk;
    volatile efuse_conf_reg_t conf;
    volatile efuse_ecdsa_reg_t ecdsa;
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
} efuse_dev_t;

extern efuse_dev_t EFUSE;

#ifndef __cplusplus
_Static_assert(sizeof(efuse_dev_t) == 0x200, "Invalid size of efuse_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
