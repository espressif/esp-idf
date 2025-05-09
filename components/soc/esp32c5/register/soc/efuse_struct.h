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
         *  Represents whether reading of individual eFuse block (BLOCK4 \verb+~+ BLOCK10) is
         *  disabled. For mapping between the bits of this field and the eFuse blocks, please
         *  refer to Table \ref{tab:efuse-block-1-10-para}.
         *  1: Disabled
         *  0: Enabled
         */
        uint32_t rd_dis:7;
        /** bootloader_anti_rollback_secure_version_hi : RO; bitpos: [7]; default: 0;
         *  Represents the anti-rollback secure version of the 2nd stage bootloader used by the
         *  ROM bootloader (the high part of the field).
         */
        uint32_t bootloader_anti_rollback_secure_version_hi:1;
        /** dis_icache : RO; bitpos: [8]; default: 0;
         *  Represents whether cache is disabled.
         *  1: Disabled
         *  0: Enabled
         */
        uint32_t dis_icache:1;
        /** dis_usb_jtag : RO; bitpos: [9]; default: 0;
         *  Represents whether the USB-to-JTAG function in USB Serial/JTAG is disabled. Note
         *  that \hyperref[fielddesc:EFUSEDISUSBJTAG]{EFUSE\_DIS\_USB\_JTAG} is available only
         *  when \hyperref[fielddesc:EFUSEDISUSBSERIALJTAG]{EFUSE\_DIS\_USB\_SERIAL\_JTAG} is
         *  configured to 0. For more information, please refer to Chapter \ref{mod:bootctrl}
         *  \textit{\nameref{mod:bootctrl}}.
         *  1: Disabled
         *  0: Enabled
         */
        uint32_t dis_usb_jtag:1;
        /** bootloader_anti_rollback_en : RO; bitpos: [10]; default: 0;
         *  Represents whether the ani-rollback check for the 2nd stage bootloader is enabled.
         *  1: Enabled
         *  0: Disabled
         */
        uint32_t bootloader_anti_rollback_en:1;
        /** dis_usb_serial_jtag : RO; bitpos: [11]; default: 0;
         *  Represents whether USB Serial/JTAG is disabled.
         *  1: Disabled
         *  0: Enabled
         *
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t dis_usb_serial_jtag:1;
        /** dis_force_download : RO; bitpos: [12]; default: 0;
         *  Represents whether the function that forces chip into Download mode is disabled.
         *  1: Disabled
         *  0: Enabled
         */
        uint32_t dis_force_download:1;
        /** spi_download_mspi_dis : RO; bitpos: [13]; default: 0;
         *  Represents whether SPI0 controller during boot\_mode\_download is disabled.
         *  0: Enabled
         *  1: Disabled
         */
        uint32_t spi_download_mspi_dis:1;
        /** dis_twai : RO; bitpos: [14]; default: 0;
         *  Represents whether TWAI$^®$ function is disabled.
         *  1: Disabled
         *  0: Enabled
         */
        uint32_t dis_twai:1;
        /** jtag_sel_enable : RO; bitpos: [15]; default: 0;
         *  Represents whether the selection of a JTAG signal source through the strapping pin
         *  value is enabled when all of
         *  \hyperref[fielddesc:EFUSEDISPADJTAG]{EFUSE\_DIS\_PAD\_JTAG},
         *  \hyperref[fielddesc:EFUSEDISUSBJTAG]{EFUSE\_DIS\_USB\_JTAG} and
         *  \hyperref[fielddesc:EFUSEDISUSBSERIALJTAG]{EFUSE\_DIS\_USB\_SERIAL\_JTAG} are
         *  configured to 0. For more information, please refer to Chapter \ref{mod:bootctrl}
         *  \textit{\nameref{mod:bootctrl}}.
         *  1: Enabled
         *  0: Disabled
         */
        uint32_t jtag_sel_enable:1;
        /** soft_dis_jtag : RO; bitpos: [18:16]; default: 0;
         *  Represents whether PAD JTAG is disabled in the soft way. It can be restarted via
         *  HMAC.
         *  Odd count of bits with a value of 1: Disabled
         *  Even count of bits with a value of 1: Enabled
         */
        uint32_t soft_dis_jtag:3;
        /** dis_pad_jtag : RO; bitpos: [19]; default: 0;
         *  Represents whether PAD JTAG is disabled in the hard way (permanently).
         *  1: Disabled
         *  0: Enabled
         */
        uint32_t dis_pad_jtag:1;
        /** dis_download_manual_encrypt : RO; bitpos: [20]; default: 0;
         *  Represents whether flash encryption is disabled (except in SPI boot mode).
         *  1: Disabled
         *  0: Enabled
         */
        uint32_t dis_download_manual_encrypt:1;
        /** usb_drefh : RO; bitpos: [22:21]; default: 0;
         *  Represents the single-end input threshold vrefh, 1.76 V to 2 V with step of 80 mV.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t usb_drefh:2;
        /** usb_drefl : RO; bitpos: [24:23]; default: 0;
         *  Represents the single-end input threshold vrefl, 1.76 V to 2 V with step of 80 mV.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t usb_drefl:2;
        /** usb_exchg_pins : RO; bitpos: [25]; default: 0;
         *  Represents whether the D+ and D- pins is exchanged.
         *  1: Exchanged
         *  0: Not exchanged
         */
        uint32_t usb_exchg_pins:1;
        /** vdd_spi_as_gpio : RO; bitpos: [26]; default: 0;
         *  Represents whether VDD SPI pin is functioned as GPIO.
         *  1: Functioned
         *  0: Not functioned
         */
        uint32_t vdd_spi_as_gpio:1;
        /** wdt_delay_sel : RO; bitpos: [28:27]; default: 0;
         *  Represents RTC watchdog timeout threshold.
         *  0: The originally configured STG0 threshold × 2
         *  1: The originally configured STG0 threshold × 4
         *  2: The originally configured STG0 threshold × 8
         *  3: The originally configured STG0 threshold × 16
         */
        uint32_t wdt_delay_sel:2;
        /** bootloader_anti_rollback_secure_version_lo : RO; bitpos: [31:29]; default: 0;
         *  Represents the anti-rollback secure version of the 2nd stage bootloader used by the
         *  ROM bootloader (the low part of the field).
         */
        uint32_t bootloader_anti_rollback_secure_version_lo:3;
    };
    uint32_t val;
} efuse_rd_repeat_data0_reg_t;

/** Type of rd_repeat_data1 register
 *  Represents rd_repeat_data
 */
typedef union {
    struct {
        /** km_disable_deploy_mode : RO; bitpos: [3:0]; default: 0;
         *  Represents whether the new key deployment of key manager is disabled.
         *  Bit0: Represents whether the new ECDSA key deployment is disabled
         *  0: Enabled
         *  1: Disabled
         *  Bit1: Represents whether the new XTS-AES (flash and PSRAM) key deployment is
         *  disabled
         *  0: Enabled
         *  1: Disabled
         *  Bit2: Represents whether the new HMAC key deployment is disabled
         *  0: Enabled
         *  1: Disabled
         *  Bit3: Represents whether the new DS key deployment is disabled
         *  0: Enabled
         *  1: Disabled
         */
        uint32_t km_disable_deploy_mode:4;
        /** km_rnd_switch_cycle : RO; bitpos: [5:4]; default: 0;
         *  Represents the cycle at which the Key Manager switches random numbers.
         *  0: Controlled by the
         *  \hyperref[fielddesc:KEYMNGRNDSWITCHCYCLE]{KEYMNG\_RND\_SWITCH\_CYCLE} register. For
         *  more information, please refer to Chapter \ref{mod:keymng}
         *  \textit{\nameref{mod:keymng}}
         *  1: 8 Key Manager clock cycles
         *  2: 16 Key Manager clock cycles
         *  3: 32 Key Manager clock cycles
         */
        uint32_t km_rnd_switch_cycle:2;
        /** km_deploy_only_once : RO; bitpos: [9:6]; default: 0;
         *  Represents whether the corresponding key can be deployed only once.
         *  Bit0: Represents whether the ECDSA key can be deployed only once
         *  0: The key can be deployed multiple times
         *  1: The key can be deployed only once
         *  Bit1: Represents whether the XTS-AES (flash and PSRAM) key can be deployed only once
         *  0: The key can be deployed multiple times
         *  1: The key can be deployed only once
         *  Bit2: Represents whether the HMAC key can be deployed only once
         *  0: The key can be deployed multiple times
         *  1: The key can be deployed only once
         *  Bit3: Represents whether the DS key can be deployed only once
         *  0: The key can be deployed multiple times
         *  1: The key can be deployed only once
         */
        uint32_t km_deploy_only_once:4;
        /** force_use_key_manager_key : RO; bitpos: [13:10]; default: 0;
         *  Represents whether the corresponding key must come from Key Manager.
         *  Bit0: Represents whether the ECDSA key must come from Key Manager.
         *  0: The key does not need to come from Key Manager
         *  1: The key must come from Key Manager
         *  Bit1: Represents whether the XTS-AES (flash and PSRAM) key must come from Key
         *  Manager.
         *  0: The key does not need to come from Key Manager
         *  1: The key must come from Key Manager
         *  Bit2: Represents whether the HMAC key must come from Key Manager.
         *  0: The key does not need to come from Key Manager
         *  1: The key must come from Key Manager
         *  Bit3: Represents whether the DS key must come from Key Manager.
         *  0: The key does not need to come from Key Manager
         *  1: The key must come from Key Manager
         */
        uint32_t force_use_key_manager_key:4;
        /** force_disable_sw_init_key : RO; bitpos: [14]; default: 0;
         *  Represents whether to disable the use of the initialization key written by software
         *  and instead force use efuse\_init\_key.
         *  0: Enable
         *  1: Disable
         */
        uint32_t force_disable_sw_init_key:1;
        /** bootloader_anti_rollback_update_in_rom : RO; bitpos: [15]; default: 0;
         *  Represents whether the ani-rollback SECURE_VERSION will be updated from the ROM
         *  bootloader.
         *  1: Enable
         *  0: Disable
         */
        uint32_t bootloader_anti_rollback_update_in_rom:1;
        /** spi_boot_crypt_cnt : RO; bitpos: [18:16]; default: 0;
         *  Represents whether SPI boot encryption/decryption is enabled.
         *  Odd count of bits with a value of 1: Enabled
         *  Even count of bits with a value of 1: Disabled
         */
        uint32_t spi_boot_crypt_cnt:3;
        /** secure_boot_key_revoke0 : RO; bitpos: [19]; default: 0;
         *  Represents whether revoking Secure Boot key digest 0 is enabled.
         *  1: Enabled
         *  0: Disabled
         */
        uint32_t secure_boot_key_revoke0:1;
        /** secure_boot_key_revoke1 : RO; bitpos: [20]; default: 0;
         *  Represents whether revoking Secure Boot key digest 1 is enabled.
         *  1: Enabled
         *  0: Disabled
         */
        uint32_t secure_boot_key_revoke1:1;
        /** secure_boot_key_revoke2 : RO; bitpos: [21]; default: 0;
         *  Represents whether revoking Secure Boot key digest 2 is enabled.
         *  1: Enabled
         *  0: Disabled
         */
        uint32_t secure_boot_key_revoke2:1;
        /** key_purpose_0 : RO; bitpos: [26:22]; default: 0;
         *  Represents the purpose of Key0. See Table \ref{tab:efuse-key-purpose}.
         */
        uint32_t key_purpose_0:5;
        /** key_purpose_1 : RO; bitpos: [31:27]; default: 0;
         *  Represents the purpose of Key1. See Table \ref{tab:efuse-key-purpose}.
         */
        uint32_t key_purpose_1:5;
    };
    uint32_t val;
} efuse_rd_repeat_data1_reg_t;

/** Type of rd_repeat_data2 register
 *  Represents rd_repeat_data
 */
typedef union {
    struct {
        /** key_purpose_2 : RO; bitpos: [4:0]; default: 0;
         *  Represents the purpose of Key2. See Table \ref{tab:efuse-key-purpose}.
         */
        uint32_t key_purpose_2:5;
        /** key_purpose_3 : RO; bitpos: [9:5]; default: 0;
         *  Represents the purpose of Key3. See Table \ref{tab:efuse-key-purpose}.
         */
        uint32_t key_purpose_3:5;
        /** key_purpose_4 : RO; bitpos: [14:10]; default: 0;
         *  Represents the purpose of Key4. See Table \ref{tab:efuse-key-purpose}.
         */
        uint32_t key_purpose_4:5;
        /** key_purpose_5 : RO; bitpos: [19:15]; default: 0;
         *  Represents the purpose of Key5. See Table \ref{tab:efuse-key-purpose}.
         */
        uint32_t key_purpose_5:5;
        /** sec_dpa_level : RO; bitpos: [21:20]; default: 0;
         *  Represents the security level of anti-DPA attack. The level is adjusted by
         *  configuring the clock random frequency division mode.
         *  0: Security level is SEC\_DPA\_OFF
         *  1: Security level is SEC\_DPA\_LOW
         *  2: Security level is SEC\_DPA\_MIDDLE
         *  3: Security level is SEC\_DPA\_HIGH
         *  For more information, please refer to Chapter \ref{mod:sysreg}
         *  \textit{\nameref{mod:sysreg}} > Section
         *  \ref{sec:sysreg-anti-dpa-attack-security-control}
         *  \textit{\nameref{sec:sysreg-anti-dpa-attack-security-control}}.
         */
        uint32_t sec_dpa_level:2;
        /** recovery_bootloader_flash_sector_hi : RO; bitpos: [24:22]; default: 0;
         *  Represents the starting flash sector (flash sector size is 0x1000) of the recovery
         *  bootloader used by the ROM bootloader If the primary bootloader fails. 0 and 0xFFF
         *  - this feature is disabled. (The high part of the field).
         */
        uint32_t recovery_bootloader_flash_sector_hi:3;
        /** secure_boot_en : RO; bitpos: [25]; default: 0;
         *  Represents whether Secure Boot is enabled.
         *  1: Enabled
         *  0: Disabled
         */
        uint32_t secure_boot_en:1;
        /** secure_boot_aggressive_revoke : RO; bitpos: [26]; default: 0;
         *  Represents whether aggressive revocation of Secure Boot is enabled.
         *  1: Enabled
         *  0: Disabled
         */
        uint32_t secure_boot_aggressive_revoke:1;
        /** km_xts_key_length_256 : RO; bitpos: [27]; default: 0;
         *  Represents which key flash encryption uses.
         *  0: XTS-AES-256 key
         *  1: XTS-AES-128 key
         */
        uint32_t km_xts_key_length_256:1;
        /** flash_tpuw : RO; bitpos: [31:28]; default: 0;
         *  Represents the flash waiting time after power-up. Measurement unit: ms. When the
         *  value is less than 15, the waiting time is the programmed value. Otherwise, the
         *  waiting time is a fixed value, i.e. 30 ms.
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
         *  Represents whether Download mode is disable or enable. 1. Disable
         *  0: Enable.
         */
        uint32_t dis_download_mode:1;
        /** dis_direct_boot : RO; bitpos: [1]; default: 0;
         *  Represents whether direct boot mode is disabled or enabled. 1. Disable
         *  0: Enable.
         */
        uint32_t dis_direct_boot:1;
        /** dis_usb_serial_jtag_rom_print : RO; bitpos: [2]; default: 0;
         *  Represents whether print from USB-Serial-JTAG is disabled or enabled. 1. Disable
         *  0: Enable.
         */
        uint32_t dis_usb_serial_jtag_rom_print:1;
        /** lock_km_key : RO; bitpos: [3]; default: 0;
         *  Represents whether the keys in the Key Manager are locked after deployment.
         *  0: Not locked
         *  1: Locked
         */
        uint32_t lock_km_key:1;
        /** dis_usb_serial_jtag_download_mode : RO; bitpos: [4]; default: 0;
         *  Represents whether the USB-Serial-JTAG download function is disabled or enabled. 1:
         *  Disable
         *  0: Enable.
         */
        uint32_t dis_usb_serial_jtag_download_mode:1;
        /** enable_security_download : RO; bitpos: [5]; default: 0;
         *  Represents whether security download is enabled. Only downloading into flash is
         *  supported. Reading/writing RAM or registers is not supported (i.e. stub download is
         *  not supported).
         *  1: Enabled
         *  0: Disabled
         */
        uint32_t enable_security_download:1;
        /** uart_print_control : RO; bitpos: [7:6]; default: 0;
         *  Represents the type of UART printing.
         *  0: Force enable printing.
         *  1: Enable printing when GPIO27 is reset at low level.
         *  2: Enable printing when GPIO27 is reset at high level.
         *  3: Force disable printing.
         */
        uint32_t uart_print_control:2;
        /** force_send_resume : RO; bitpos: [8]; default: 0;
         *  Represents whether ROM code is forced to send a resume command during SPI boot.
         *  1: Forced.
         *  0: Not forced.
         */
        uint32_t force_send_resume:1;
        /** secure_version : RO; bitpos: [17:9]; default: 0;
         *  Represents the app secure version used by ESP-IDF anti-rollback feature.
         */
        uint32_t secure_version:9;
        /** rd_reserve_0_146 : RW; bitpos: [24:18]; default: 0;
         *  Reserved, it was created by set_missed_fields_in_regs func
         */
        uint32_t rd_reserve_0_146:7;
        /** secure_boot_disable_fast_wake : RO; bitpos: [25]; default: 0;
         *  Represents whether FAST VERIFY ON WAKE is disabled when Secure Boot is enabled.
         *  1: Disabled
         *  0: Enabled
         */
        uint32_t secure_boot_disable_fast_wake:1;
        /** hys_en_pad : RO; bitpos: [26]; default: 0;
         *  Represents whether the hysteresis function of PAD0 – PAD27 is enabled.
         *  1: Enabled
         *  0: Disabled
         */
        uint32_t hys_en_pad:1;
        /** xts_dpa_pseudo_level : RO; bitpos: [28:27]; default: 0;
         *  Represents the pseudo round level of XTS-AES anti-DPA attack.
         *  0: Disabled
         *  1: Low
         *  2: Moderate
         *  3: High
         */
        uint32_t xts_dpa_pseudo_level:2;
        /** xts_dpa_clk_enable : RO; bitpos: [29]; default: 0;
         *  Represents whether XTS-AES anti-DPA attack clock is enabled.
         *  0: Disable
         *  1: Enabled
         */
        uint32_t xts_dpa_clk_enable:1;
        /** rd_reserve_0_158 : RW; bitpos: [30]; default: 0;
         *  Reserved, it was created by set_missed_fields_in_regs func
         */
        uint32_t rd_reserve_0_158:1;
        /** secure_boot_sha384_en : RO; bitpos: [31]; default: 0;
         *  Represents if the chip supports Secure Boot using SHA-384
         */
        uint32_t secure_boot_sha384_en:1;
    };
    uint32_t val;
} efuse_rd_repeat_data3_reg_t;

/** Type of rd_repeat_data4 register
 *  Represents rd_repeat_data
 */
typedef union {
    struct {
        /** huk_gen_state : RO; bitpos: [8:0]; default: 0;
         *  Represents whether the HUK generate mode is valid.
         *  Odd count of bits with a value of 1: Invalid
         *  Even count of bits with a value of 1: Valid
         */
        uint32_t huk_gen_state:9;
        /** xtal_48m_sel : RO; bitpos: [11:9]; default: 0;
         *  Determines the frequency of the XTAL clock alone in \textbf{SPI Boot} mode, or
         *  together with \hyperref[fielddesc:EFUSEXTAL48MSELMODE]{EFUSE\_XTAL\_48M\_SEL\_MODE}
         *  in \textbf{Joint Download Boot} mode. For more information, please refer to Chapter
         *  \ref{mod:bootctrl} \textit{\nameref{mod:bootctrl}}.
         *  Odd count of bits with a value of 1: 48 MHz
         *  Even count of bits with a value of 1: 40 MHz
         */
        uint32_t xtal_48m_sel:3;
        /** xtal_48m_sel_mode : RO; bitpos: [12]; default: 0;
         *  Represents what determines the XTAL frequency in \textbf{Joint Download Boot} mode.
         *  For more information, please refer to Chapter \ref{mod:bootctrl}
         *  \textit{\nameref{mod:bootctrl}}.
         *  0: Strapping PAD state
         *  1: \hyperref[fielddesc:EFUSEXTAL48MSEL]{EFUSE\_XTAL\_48M\_SEL} in eFuse
         */
        uint32_t xtal_48m_sel_mode:1;
        /** ecc_force_const_time : RO; bitpos: [13]; default: 0;
         *  Represents whether to force ECC to use constant-time mode for point multiplication
         *  calculation.
         *  0: Not force
         *  1: Force
         */
        uint32_t ecc_force_const_time:1;
        /** recovery_bootloader_flash_sector_lo : RO; bitpos: [22:14]; default: 0;
         *  Represents the starting flash sector (flash sector size is 0x1000) of the recovery
         *  bootloader used by the ROM bootloader If the primary bootloader fails. 0 and 0xFFF
         *  - this feature is disabled. (The low part of the field).
         */
        uint32_t recovery_bootloader_flash_sector_lo:9;
        /** rd_reserve_0_183 : RW; bitpos: [31:23]; default: 0;
         *  Reserved, it was created by set_missed_fields_in_regs func
         */
        uint32_t rd_reserve_0_183:9;
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
        /** mac_ext : RO; bitpos: [31:16]; default: 0;
         *  Represents the extended bits of MAC address.
         */
        uint32_t mac_ext:16;
    };
    uint32_t val;
} efuse_rd_mac_sys1_reg_t;

/** Type of rd_mac_sys2 register
 *  Represents rd_mac_sys
 */
typedef union {
    struct {
        /** wafer_version_minor : R; bitpos: [3:0]; default: 0;
         *  Minor chip version
         */
        uint32_t wafer_version_minor:4;
        /** wafer_version_major : R; bitpos: [5:4]; default: 0;
         *  Minor chip version
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
        /** flash_vendor : R; bitpos: [18:16]; default: 0;
         *  Flash vendor
         */
        uint32_t flash_vendor:3;
        /** psram_cap : R; bitpos: [21:19]; default: 0;
         *  Psram capacity
         */
        uint32_t psram_cap:3;
        /** psram_vendor : R; bitpos: [23:22]; default: 0;
         *  Psram vendor
         */
        uint32_t psram_vendor:2;
        /** temp : R; bitpos: [25:24]; default: 0;
         *  Temp (die embedded inside)
         */
        uint32_t temp:2;
        /** pkg_version : R; bitpos: [28:26]; default: 0;
         *  Package version
         */
        uint32_t pkg_version:3;
        /** pa_trim_version : R; bitpos: [31:29]; default: 0;
         *  PADC CAL PA trim version
         */
        uint32_t pa_trim_version:3;
    };
    uint32_t val;
} efuse_rd_mac_sys2_reg_t;

/** Type of rd_mac_sys3 register
 *  Represents rd_mac_sys
 */
typedef union {
    struct {
        /** trim_n_bias : R; bitpos: [4:0]; default: 0;
         *  PADC CAL N bias
         */
        uint32_t trim_n_bias:5;
        /** trim_p_bias : R; bitpos: [9:5]; default: 0;
         *  PADC CAL P bias
         */
        uint32_t trim_p_bias:5;
        /** active_hp_dbias : R; bitpos: [13:10]; default: 0;
         *  Active HP DBIAS of fixed voltage
         */
        uint32_t active_hp_dbias:4;
        /** active_lp_dbias : R; bitpos: [17:14]; default: 0;
         *  Active LP DBIAS of fixed voltage
         */
        uint32_t active_lp_dbias:4;
        /** lslp_hp_dbg : R; bitpos: [19:18]; default: 0;
         *  LSLP HP DBG of fixed voltage
         */
        uint32_t lslp_hp_dbg:2;
        /** lslp_hp_dbias : R; bitpos: [23:20]; default: 0;
         *  LSLP HP DBIAS of fixed voltage
         */
        uint32_t lslp_hp_dbias:4;
        /** dslp_lp_dbg : R; bitpos: [27:24]; default: 0;
         *  DSLP LP DBG of fixed voltage
         */
        uint32_t dslp_lp_dbg:4;
        /** dslp_lp_dbias : R; bitpos: [31:28]; default: 0;
         *  DSLP LP DBIAS of fixed voltage
         */
        uint32_t dslp_lp_dbias:4;
    };
    uint32_t val;
} efuse_rd_mac_sys3_reg_t;

/** Type of rd_mac_sys4 register
 *  Represents rd_mac_sys
 */
typedef union {
    struct {
        /** dslp_lp_dbias_1 : R; bitpos: [0]; default: 0;
         *  DSLP LP DBIAS of fixed voltage
         */
        uint32_t dslp_lp_dbias_1:1;
        /** lp_hp_dbias_vol_gap : R; bitpos: [5:1]; default: 0;
         *  DBIAS gap between LP and HP
         */
        uint32_t lp_hp_dbias_vol_gap:5;
        /** ref_curr_code : R; bitpos: [9:6]; default: 0;
         *  REF PADC Calibration Curr
         */
        uint32_t ref_curr_code:4;
        /** res_tune_code : R; bitpos: [14:10]; default: 0;
         *  RES PADC Calibration Tune
         */
        uint32_t res_tune_code:5;
        /** reserved_1_143 : R; bitpos: [31:15]; default: 0;
         *  reserved
         */
        uint32_t reserved_1_143:17;
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
/** Type of rd_sys_part1_data0 register
 *  Represents rd_sys_part1_data0
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
 *  Represents rd_sys_part1_data1
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
 *  Represents rd_sys_part1_data2
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
 *  Represents rd_sys_part1_data3
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
 *  Represents rd_sys_part1_data4
 */
typedef union {
    struct {
        /** temperature_sensor : R; bitpos: [8:0]; default: 0;
         *  Temperature calibration data
         */
        uint32_t temperature_sensor:9;
        /** ocode : R; bitpos: [16:9]; default: 0;
         *  ADC OCode
         */
        uint32_t ocode:8;
        /** adc1_ave_initcode_atten0 : R; bitpos: [26:17]; default: 0;
         *  Average initcode of ADC1 atten0
         */
        uint32_t adc1_ave_initcode_atten0:10;
        /** adc1_ave_initcode_atten1 : R; bitpos: [31:27]; default: 0;
         *  Average initcode of ADC1 atten0
         */
        uint32_t adc1_ave_initcode_atten1:5;
    };
    uint32_t val;
} efuse_rd_sys_part1_data4_reg_t;

/** Type of rd_sys_part1_data5 register
 *  Represents rd_sys_part1_data5
 */
typedef union {
    struct {
        /** adc1_ave_initcode_atten1_1 : R; bitpos: [4:0]; default: 0;
         *  Average initcode of ADC1 atten0
         */
        uint32_t adc1_ave_initcode_atten1_1:5;
        /** adc1_ave_initcode_atten2 : R; bitpos: [14:5]; default: 0;
         *  Average initcode of ADC1 atten0
         */
        uint32_t adc1_ave_initcode_atten2:10;
        /** adc1_ave_initcode_atten3 : R; bitpos: [24:15]; default: 0;
         *  Average initcode of ADC1 atten0
         */
        uint32_t adc1_ave_initcode_atten3:10;
        /** adc1_hi_dout_atten0 : R; bitpos: [31:25]; default: 0;
         *  HI DOUT of ADC1 atten0
         */
        uint32_t adc1_hi_dout_atten0:7;
    };
    uint32_t val;
} efuse_rd_sys_part1_data5_reg_t;

/** Type of rd_sys_part1_data6 register
 *  Represents rd_sys_part1_data6
 */
typedef union {
    struct {
        /** adc1_hi_dout_atten0_1 : R; bitpos: [2:0]; default: 0;
         *  HI DOUT of ADC1 atten0
         */
        uint32_t adc1_hi_dout_atten0_1:3;
        /** adc1_hi_dout_atten1 : R; bitpos: [12:3]; default: 0;
         *  HI DOUT of ADC1 atten1
         */
        uint32_t adc1_hi_dout_atten1:10;
        /** adc1_hi_dout_atten2 : R; bitpos: [22:13]; default: 0;
         *  HI DOUT of ADC1 atten2
         */
        uint32_t adc1_hi_dout_atten2:10;
        /** adc1_hi_dout_atten3 : R; bitpos: [31:23]; default: 0;
         *  HI DOUT of ADC1 atten3
         */
        uint32_t adc1_hi_dout_atten3:9;
    };
    uint32_t val;
} efuse_rd_sys_part1_data6_reg_t;

/** Type of rd_sys_part1_data7 register
 *  Represents rd_sys_part1_data7
 */
typedef union {
    struct {
        /** adc1_hi_dout_atten3_1 : R; bitpos: [0]; default: 0;
         *  HI DOUT of ADC1 atten3
         */
        uint32_t adc1_hi_dout_atten3_1:1;
        /** adc1_ch0_atten0_initcode_diff : R; bitpos: [4:1]; default: 0;
         *  Gap between ADC1 CH0 and average initcode
         */
        uint32_t adc1_ch0_atten0_initcode_diff:4;
        /** adc1_ch1_atten0_initcode_diff : R; bitpos: [8:5]; default: 0;
         *  Gap between ADC1 CH1 and average initcode
         */
        uint32_t adc1_ch1_atten0_initcode_diff:4;
        /** adc1_ch2_atten0_initcode_diff : R; bitpos: [12:9]; default: 0;
         *  Gap between ADC1 CH2 and average initcode
         */
        uint32_t adc1_ch2_atten0_initcode_diff:4;
        /** adc1_ch3_atten0_initcode_diff : R; bitpos: [16:13]; default: 0;
         *  Gap between ADC1 CH3 and average initcode
         */
        uint32_t adc1_ch3_atten0_initcode_diff:4;
        /** adc1_ch4_atten0_initcode_diff : R; bitpos: [20:17]; default: 0;
         *  Gap between ADC1 CH4 and average initcode
         */
        uint32_t adc1_ch4_atten0_initcode_diff:4;
        /** adc1_ch5_atten0_initcode_diff : R; bitpos: [24:21]; default: 0;
         *  Gap between ADC1 CH5 and average initcode
         */
        uint32_t adc1_ch5_atten0_initcode_diff:4;
        /** reserved_2_249 : R; bitpos: [31:25]; default: 0;
         *  reserved
         */
        uint32_t reserved_2_249:7;
    };
    uint32_t val;
} efuse_rd_sys_part1_data7_reg_t;


/** Group: block3 registers */
/** Type of rd_usr_data0 register
 *  Represents rd_usr_data0
 */
typedef union {
    struct {
        /** usr_data0 : RO; bitpos: [31:0]; default: 0;
         *  Represents the zeroth 32-bit of block3 (user).
         */
        uint32_t usr_data0:32;
    };
    uint32_t val;
} efuse_rd_usr_data0_reg_t;

/** Type of rd_usr_data1 register
 *  Represents rd_usr_data1
 */
typedef union {
    struct {
        /** usr_data1 : RO; bitpos: [31:0]; default: 0;
         *  Represents the zeroth 32-bit of block3 (user).
         */
        uint32_t usr_data1:32;
    };
    uint32_t val;
} efuse_rd_usr_data1_reg_t;

/** Type of rd_usr_data2 register
 *  Represents rd_usr_data2
 */
typedef union {
    struct {
        /** usr_data2 : RO; bitpos: [31:0]; default: 0;
         *  Represents the zeroth 32-bit of block3 (user).
         */
        uint32_t usr_data2:32;
    };
    uint32_t val;
} efuse_rd_usr_data2_reg_t;

/** Type of rd_usr_data3 register
 *  Represents rd_usr_data3
 */
typedef union {
    struct {
        /** usr_data3 : RO; bitpos: [31:0]; default: 0;
         *  Represents the zeroth 32-bit of block3 (user).
         */
        uint32_t usr_data3:32;
    };
    uint32_t val;
} efuse_rd_usr_data3_reg_t;

/** Type of rd_usr_data4 register
 *  Represents rd_usr_data4
 */
typedef union {
    struct {
        /** usr_data4 : RO; bitpos: [31:0]; default: 0;
         *  Represents the zeroth 32-bit of block3 (user).
         */
        uint32_t usr_data4:32;
    };
    uint32_t val;
} efuse_rd_usr_data4_reg_t;

/** Type of rd_usr_data5 register
 *  Represents rd_usr_data5
 */
typedef union {
    struct {
        /** usr_data5 : RO; bitpos: [31:0]; default: 0;
         *  Represents the zeroth 32-bit of block3 (user).
         */
        uint32_t usr_data5:32;
    };
    uint32_t val;
} efuse_rd_usr_data5_reg_t;

/** Type of rd_usr_data6 register
 *  Represents rd_usr_data6
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
 *  Represents rd_usr_data7
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
/** Type of rd_sys_part2_datan register
 *  Represents rd_sys_part2_datan
 */
typedef union {
    struct {
        /** sys_data_part2_n : RO; bitpos: [31:0]; default: 0;
         *  Represents the zeroth 32-bit of second part of system data.
         */
        uint32_t sys_data_part2_n:32;
    };
    uint32_t val;
} efuse_rd_sys_part2_datan_reg_t;


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
        /** bootloader_anti_rollback_secure_version_hi_err : RO; bitpos: [7]; default: 0;
         *  Represents the programming error of EFUSE_BOOTLOADER_ANTI_ROLLBACK_SECURE_VERSION_HI
         */
        uint32_t bootloader_anti_rollback_secure_version_hi_err:1;
        /** dis_icache_err : RO; bitpos: [8]; default: 0;
         *  Represents the programming error of EFUSE_DIS_ICACHE
         */
        uint32_t dis_icache_err:1;
        /** dis_usb_jtag_err : RO; bitpos: [9]; default: 0;
         *  Represents the programming error of EFUSE_DIS_USB_JTAG
         */
        uint32_t dis_usb_jtag_err:1;
        /** bootloader_anti_rollback_en_err : RO; bitpos: [10]; default: 0;
         *  Represents the programming error of EFUSE_BOOTLOADER_ANTI_ROLLBACK_EN
         */
        uint32_t bootloader_anti_rollback_en_err:1;
        /** dis_usb_serial_jtag_err : RO; bitpos: [11]; default: 0;
         *  Represents the programming error of EFUSE_DIS_USB_SERIAL_JTAG
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t dis_usb_serial_jtag_err:1;
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
        /** usb_drefh_err : RO; bitpos: [22:21]; default: 0;
         *  Represents the programming error of EFUSE_USB_DREFH
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t usb_drefh_err:2;
        /** usb_drefl_err : RO; bitpos: [24:23]; default: 0;
         *  Represents the programming error of EFUSE_USB_DREFL
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t usb_drefl_err:2;
        /** usb_exchg_pins_err : RO; bitpos: [25]; default: 0;
         *  Represents the programming error of EFUSE_USB_EXCHG_PINS
         */
        uint32_t usb_exchg_pins_err:1;
        /** vdd_spi_as_gpio_err : RO; bitpos: [26]; default: 0;
         *  Represents the programming error of EFUSE_VDD_SPI_AS_GPIO
         */
        uint32_t vdd_spi_as_gpio_err:1;
        /** wdt_delay_sel_err : RO; bitpos: [28:27]; default: 0;
         *  Represents the programming error of EFUSE_WDT_DELAY_SEL
         */
        uint32_t wdt_delay_sel_err:2;
        /** bootloader_anti_rollback_secure_version_lo_err : RO; bitpos: [31:29]; default: 0;
         *  Represents the programming error of EFUSE_BOOTLOADER_ANTI_ROLLBACK_SECURE_VERSION_LO
         */
        uint32_t bootloader_anti_rollback_secure_version_lo_err:3;
    };
    uint32_t val;
} efuse_rd_repeat_data_err0_reg_t;

/** Type of rd_repeat_data_err1 register
 *  Represents rd_repeat_data_err
 */
typedef union {
    struct {
        /** km_disable_deploy_mode_err : RO; bitpos: [3:0]; default: 0;
         *  Represents the programming error of EFUSE_KM_DISABLE_DEPLOY_MODE
         */
        uint32_t km_disable_deploy_mode_err:4;
        /** km_rnd_switch_cycle_err : RO; bitpos: [5:4]; default: 0;
         *  Represents the programming error of EFUSE_KM_RND_SWITCH_CYCLE
         */
        uint32_t km_rnd_switch_cycle_err:2;
        /** km_deploy_only_once_err : RO; bitpos: [9:6]; default: 0;
         *  Represents the programming error of EFUSE_KM_DEPLOY_ONLY_ONCE
         */
        uint32_t km_deploy_only_once_err:4;
        /** force_use_key_manager_key_err : RO; bitpos: [13:10]; default: 0;
         *  Represents the programming error of EFUSE_FORCE_USE_KEY_MANAGER_KEY
         */
        uint32_t force_use_key_manager_key_err:4;
        /** force_disable_sw_init_key_err : RO; bitpos: [14]; default: 0;
         *  Represents the programming error of EFUSE_FORCE_DISABLE_SW_INIT_KEY
         */
        uint32_t force_disable_sw_init_key_err:1;
        /** bootloader_anti_rollback_update_in_rom_err : RO; bitpos: [15]; default: 0;
         *  Represents the programming error of EFUSE_BOOTLOADER_ANTI_ROLLBACK_UPDATE_IN_ROM
         */
        uint32_t bootloader_anti_rollback_update_in_rom_err:1;
        /** spi_boot_crypt_cnt_err : RO; bitpos: [18:16]; default: 0;
         *  Represents the programming error of EFUSE_SPI_BOOT_CRYPT_CNT
         */
        uint32_t spi_boot_crypt_cnt_err:3;
        /** secure_boot_key_revoke0_err : RO; bitpos: [19]; default: 0;
         *  Represents the programming error of EFUSE_SECURE_BOOT_KEY_REVOKE0
         */
        uint32_t secure_boot_key_revoke0_err:1;
        /** secure_boot_key_revoke1_err : RO; bitpos: [20]; default: 0;
         *  Represents the programming error of EFUSE_SECURE_BOOT_KEY_REVOKE1
         */
        uint32_t secure_boot_key_revoke1_err:1;
        /** secure_boot_key_revoke2_err : RO; bitpos: [21]; default: 0;
         *  Represents the programming error of EFUSE_SECURE_BOOT_KEY_REVOKE2
         */
        uint32_t secure_boot_key_revoke2_err:1;
        /** key_purpose_0_err : RO; bitpos: [26:22]; default: 0;
         *  Represents the programming error of EFUSE_KEY_PURPOSE_0
         */
        uint32_t key_purpose_0_err:5;
        /** key_purpose_1_err : RO; bitpos: [31:27]; default: 0;
         *  Represents the programming error of EFUSE_KEY_PURPOSE_1
         */
        uint32_t key_purpose_1_err:5;
    };
    uint32_t val;
} efuse_rd_repeat_data_err1_reg_t;

/** Type of rd_repeat_data_err2 register
 *  Represents rd_repeat_data_err
 */
typedef union {
    struct {
        /** key_purpose_2_err : RO; bitpos: [4:0]; default: 0;
         *  Represents the programming error of EFUSE_KEY_PURPOSE_2
         */
        uint32_t key_purpose_2_err:5;
        /** key_purpose_3_err : RO; bitpos: [9:5]; default: 0;
         *  Represents the programming error of EFUSE_KEY_PURPOSE_3
         */
        uint32_t key_purpose_3_err:5;
        /** key_purpose_4_err : RO; bitpos: [14:10]; default: 0;
         *  Represents the programming error of EFUSE_KEY_PURPOSE_4
         */
        uint32_t key_purpose_4_err:5;
        /** key_purpose_5_err : RO; bitpos: [19:15]; default: 0;
         *  Represents the programming error of EFUSE_KEY_PURPOSE_5
         */
        uint32_t key_purpose_5_err:5;
        /** sec_dpa_level_err : RO; bitpos: [21:20]; default: 0;
         *  Represents the programming error of EFUSE_SEC_DPA_LEVEL
         */
        uint32_t sec_dpa_level_err:2;
        /** recovery_bootloader_flash_sector_hi_err : RO; bitpos: [24:22]; default: 0;
         *  Represents the programming error of EFUSE_RECOVERY_BOOTLOADER_FLASH_SECTOR_HI
         */
        uint32_t recovery_bootloader_flash_sector_hi_err:3;
        /** secure_boot_en_err : RO; bitpos: [25]; default: 0;
         *  Represents the programming error of EFUSE_SECURE_BOOT_EN
         */
        uint32_t secure_boot_en_err:1;
        /** secure_boot_aggressive_revoke_err : RO; bitpos: [26]; default: 0;
         *  Represents the programming error of EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE
         */
        uint32_t secure_boot_aggressive_revoke_err:1;
        /** km_xts_key_length_256_err : RO; bitpos: [27]; default: 0;
         *  Represents the programming error of EFUSE_KM_XTS_KEY_LENGTH_256
         */
        uint32_t km_xts_key_length_256_err:1;
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
        /** secure_version_err : RO; bitpos: [17:9]; default: 0;
         *  Represents the programming error of EFUSE_SECURE_VERSION
         */
        uint32_t secure_version_err:9;
        uint32_t reserved_18:7;
        /** secure_boot_disable_fast_wake_err : RO; bitpos: [25]; default: 0;
         *  Represents the programming error of EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE
         */
        uint32_t secure_boot_disable_fast_wake_err:1;
        /** hys_en_pad_err : RO; bitpos: [26]; default: 0;
         *  Represents the programming error of EFUSE_HYS_EN_PAD
         */
        uint32_t hys_en_pad_err:1;
        /** xts_dpa_pseudo_level_err : RO; bitpos: [28:27]; default: 0;
         *  Represents the programming error of EFUSE_XTS_DPA_PSEUDO_LEVEL
         */
        uint32_t xts_dpa_pseudo_level_err:2;
        /** xts_dpa_clk_enable_err : RO; bitpos: [29]; default: 0;
         *  Represents the programming error of EFUSE_XTS_DPA_CLK_ENABLE
         */
        uint32_t xts_dpa_clk_enable_err:1;
        uint32_t reserved_30:1;
        /** ecdsa_p384_enable_err : RO; bitpos: [31]; default: 0;
         *  Represents the programming error of EFUSE_ECDSA_P384_ENABLE
         */
        uint32_t ecdsa_p384_enable_err:1;
    };
    uint32_t val;
} efuse_rd_repeat_data_err3_reg_t;

/** Type of rd_repeat_data_err4 register
 *  Represents rd_repeat_data_err
 */
typedef union {
    struct {
        /** huk_gen_state_err : RO; bitpos: [8:0]; default: 0;
         *  Represents the programming error of EFUSE_HUK_GEN_STATE
         */
        uint32_t huk_gen_state_err:9;
        /** xtal_48m_sel_err : RO; bitpos: [11:9]; default: 0;
         *  Represents the programming error of EFUSE_XTAL_48M_SEL
         */
        uint32_t xtal_48m_sel_err:3;
        /** xtal_48m_sel_mode_err : RO; bitpos: [12]; default: 0;
         *  Represents the programming error of EFUSE_XTAL_48M_SEL_MODE
         */
        uint32_t xtal_48m_sel_mode_err:1;
        /** ecc_force_const_time_err : RO; bitpos: [13]; default: 0;
         *  Represents the programming error of EFUSE_ECC_FORCE_CONST_TIME
         */
        uint32_t ecc_force_const_time_err:1;
        /** recovery_bootloader_flash_sector_lo_err : RO; bitpos: [22:14]; default: 0;
         *  Represents the programming error of EFUSE_RECOVERY_BOOTLOADER_FLASH_SECTOR_LO
         */
        uint32_t recovery_bootloader_flash_sector_lo_err:9;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} efuse_rd_repeat_data_err4_reg_t;


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


/** Group: EFUSE Version Register */
/** Type of date register
 *  eFuse version register.
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 37818960;
         *  Represents eFuse version. Date:2024-12-19 16:08:23,
         *  ScriptRev:5b7d298cf2df9f93b161800aff1807aeadbb74f4
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
        /** dac_clk_div : R/W; bitpos: [7:0]; default: 19;
         *  Configures the division factor of the rising clock of the programming voltage.
         */
        uint32_t dac_clk_div:8;
        /** dac_clk_pad_sel : R/W; bitpos: [8]; default: 0;
         *  Don't care.
         */
        uint32_t dac_clk_pad_sel:1;
        /** dac_num : R/W; bitpos: [16:9]; default: 255;
         *  Configures clock cycles for programming voltage to rise. Measurement unit: a clock
         *  cycle divided by EFUSE_DAC_CLK_DIV.
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
         *  Configures the read hold time. Measurement unit: One cycle of the eFuse core clock.
         */
        uint32_t thr_a:8;
        /** trd : R/W; bitpos: [15:8]; default: 2;
         *  Configures the read time. Measurement unit: One cycle of the eFuse core clock.
         */
        uint32_t trd:8;
        /** tsur_a : R/W; bitpos: [23:16]; default: 1;
         *  Configures the read setup time. Measurement unit: One cycle of the eFuse core clock.
         */
        uint32_t tsur_a:8;
        /** read_init_num : R/W; bitpos: [31:24]; default: 18;
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
         *  Configures the programming setup time. Measurement unit: One cycle of the eFuse
         *  core clock.
         */
        uint32_t tsup_a:8;
        /** pwr_on_num : R/W; bitpos: [23:8]; default: 9831;
         *  Configures the power up time for VDDQ. Measurement unit: One cycle of the eFuse
         *  core clock.
         */
        uint32_t pwr_on_num:16;
        /** thp_a : R/W; bitpos: [31:24]; default: 1;
         *  Configures the programming hold time. Measurement unit: One cycle of the eFuse core
         *  clock.
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
         *  Configures the power outage time for VDDQ. Measurement unit: One cycle of the eFuse
         *  core clock.
         */
        uint32_t pwr_off_num:16;
        /** tpgm : R/W; bitpos: [31:16]; default: 160;
         *  Configures the active programming time. Measurement unit: One cycle of the eFuse
         *  core clock.
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
        /** otp_load_sw : RO; bitpos: [4]; default: 0;
         *  Represents the value of OTP_LOAD_SW.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t otp_load_sw:1;
        /** otp_vddq_c_sync2 : RO; bitpos: [5]; default: 0;
         *  Represents the value of OTP_VDDQ_C_SYNC2.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t otp_vddq_c_sync2:1;
        /** otp_strobe_sw : RO; bitpos: [6]; default: 0;
         *  Represents the value of OTP_STROBE_SW.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t otp_strobe_sw:1;
        /** otp_csb_sw : RO; bitpos: [7]; default: 0;
         *  Represents the value of OTP_CSB_SW.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t otp_csb_sw:1;
        /** otp_pgenb_sw : RO; bitpos: [8]; default: 0;
         *  Represents the value of OTP_PGENB_SW.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t otp_pgenb_sw:1;
        /** otp_vddq_is_sw : RO; bitpos: [9]; default: 0;
         *  Represents the value of OTP_VDDQ_IS_SW.
         *  This field is only for internal debugging purposes. Do not use it in applications.
         */
        uint32_t otp_vddq_is_sw:1;
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


/** Group: EFUSE_APB2OTP BLOCK7 Word1 Data */
/** Type of apb2otp_blk4_w1 register
 *  eFuse apb2otp BLOCK7 data register1.
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


/** Group: EFUSE_APB2OTP BLOCK7 Word2 Data */
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


/** Group: EFUSE_APB2OTP BLOCK7 Word3 Data */
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


/** Group: EFUSE_APB2OTP BLOCK7 Word4 Data */
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


/** Group: EFUSE_APB2OTP BLOCK7 Word5 Data */
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


/** Group: EFUSE_APB2OTP BLOCK7 Word6 Data */
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


/** Group: EFUSE_APB2OTP BLOCK7 Word7 Data */
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


/** Group: EFUSE_APB2OTP BLOCK7 Word8 Data */
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


/** Group: EFUSE_APB2OTP BLOCK7 Word9 Data */
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


/** Group: EFUSE_APB2OTP BLOCK7 Word10 Data */
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


/** Group: EFUSE_APB2OTP BLOCK7 Word11 Data */
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
        /** apb2otp_block10_w10 : RO; bitpos: [31:0]; default: 0;
         *  Otp block10 word10 data.
         */
        uint32_t apb2otp_block10_w10:32;
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
    volatile efuse_rd_mac_sys2_reg_t rd_mac_sys2;
    volatile efuse_rd_mac_sys3_reg_t rd_mac_sys3;
    volatile efuse_rd_mac_sys4_reg_t rd_mac_sys4;
    volatile efuse_rd_mac_sys5_reg_t rd_mac_sys5;
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
    volatile efuse_rd_key0_datan_reg_t rd_key0_datan[8];
    volatile efuse_rd_key1_datan_reg_t rd_key1_datan[8];
    volatile efuse_rd_key2_datan_reg_t rd_key2_datan[8];
    volatile efuse_rd_key3_datan_reg_t rd_key3_datan[8];
    volatile efuse_rd_key4_datan_reg_t rd_key4_datan[8];
    volatile efuse_rd_key5_datan_reg_t rd_key5_datan[8];
    volatile efuse_rd_sys_part2_datan_reg_t rd_sys_part2_datan[8];
    volatile efuse_rd_repeat_data_err0_reg_t rd_repeat_data_err0;
    volatile efuse_rd_repeat_data_err1_reg_t rd_repeat_data_err1;
    volatile efuse_rd_repeat_data_err2_reg_t rd_repeat_data_err2;
    volatile efuse_rd_repeat_data_err3_reg_t rd_repeat_data_err3;
    volatile efuse_rd_repeat_data_err4_reg_t rd_repeat_data_err4;
    volatile efuse_rd_rs_data_err0_reg_t rd_rs_data_err0;
    volatile efuse_rd_rs_data_err1_reg_t rd_rs_data_err1;
    volatile efuse_date_reg_t date;
    uint32_t reserved_19c[11];
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
    uint32_t reserved_200[192];
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
    uint32_t reserved_704;
    volatile efuse_apb2otp_en_reg_t apb2otp_en;
} efuse_dev_t;

extern efuse_dev_t EFUSE;

#ifndef __cplusplus
_Static_assert(sizeof(efuse_dev_t) == 0x70c, "Invalid size of efuse_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
