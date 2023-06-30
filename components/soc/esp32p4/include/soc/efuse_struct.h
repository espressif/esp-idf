/**
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct {
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
    uint32_t rd_wr_dis;
    union {
        struct {
            uint32_t rd_dis                    :    7;  /*Represents whether reading of individual eFuse block(block4~block10) is disabled or enabled. 1: disabled. 0: enabled.*/
            uint32_t usb_device_exchg_pins     :    1;  /*Enable usb device exchange pins of D+ and D-.*/
            uint32_t usb_otg11_exchg_pins      :    1;  /*Enable usb otg11 exchange pins of D+ and D-.*/
            uint32_t dis_usb_jtag              :    1;  /*Represents whether the function of usb switch to jtag is disabled or enabled. 1: disabled. 0: enabled.*/
            uint32_t powerglitch_en            :    1;  /*Represents whether power glitch function is enabled. 1: enabled. 0: disabled.*/
            uint32_t dis_usb_serial_jtag       :    1;  /*Represents whether USB-Serial-JTAG is disabled or enabled. 1: disabled. 0: enabled.*/
            uint32_t dis_force_download        :    1;  /*Represents whether the function that forces chip into download mode is disabled or enabled. 1: disabled. 0: enabled.*/
            uint32_t spi_download_mspi_dis     :    1;  /*Set this bit to disable accessing MSPI flash/MSPI ram by SYS AXI matrix during boot_mode_download.*/
            uint32_t dis_twai                  :    1;  /*Represents whether TWAI function is disabled or enabled. 1: disabled. 0: enabled.*/
            uint32_t jtag_sel_enable           :    1;  /*Represents whether the selection between usb_to_jtag and pad_to_jtag through strapping gpio15 when both EFUSE_DIS_PAD_JTAG and EFUSE_DIS_USB_JTAG are equal to 0 is enabled or disabled. 1: enabled. 0: disabled.*/
            uint32_t soft_dis_jtag             :    3;  /*Represents whether JTAG is disabled in soft way. Odd number: disabled. Even number: enabled.*/
            uint32_t dis_pad_jtag              :    1;  /*Represents whether JTAG is disabled in the hard way(permanently). 1: disabled. 0: enabled.*/
            uint32_t dis_download_manual_encrypt:    1;  /*Represents whether flash encrypt function is disabled or enabled(except in SPI boot mode). 1: disabled. 0: enabled.*/
            uint32_t usb_device_drefh          :    2;  /*USB intphy of usb device signle-end input high threshold, 1.76V to 2V. Step by 80mV*/
            uint32_t usb_otg11_drefh           :    2;  /*USB intphy of usb otg11 signle-end input high threshold, 1.76V to 2V. Step by 80mV*/
            uint32_t usb_phy_sel               :    1;  /*TBD*/
            uint32_t huk_gen_state_low         :    6;  /*Set this bit to control validation of HUK generate mode. Odd of 1 is invalid, even of 1 is valid.*/
        };
        uint32_t val;
    } rd_repeat_data0;
    union {
        struct {
            uint32_t huk_gen_state_high        :    3;  /*Set this bit to control validation of HUK generate mode. Odd of 1 is invalid, even of 1 is valid.*/
            uint32_t km_rnd_switch_cycle       :    2;  /*Set bits to control key manager random number switch cycle. 0: control by register. 1: 8 km clk cycles. 2: 16 km cycles. 3: 32 km cycles.*/
            uint32_t km_deploy_only_once       :    4;  /*Set each bit to control whether corresponding key can only be deployed once. 1 is true, 0 is false. Bit0: ecdsa. Bit1: xts. Bit2: hmac. Bit3: ds.*/
            uint32_t force_use_key_manager_key :    4;  /*Set each bit to control whether corresponding key must come from key manager.. 1 is true, 0 is false. Bit0: ecdsa. Bit1: xts. Bit2: hmac. Bit3: ds.*/
            uint32_t force_disable_sw_init_key :    1;  /*Set this bit to disable software written init key, and force use efuse_init_key.*/
            uint32_t xts_key_length_256        :    1;  /*Set this bit to configure flash encryption use xts-128 key, else use xts-256 key.*/
            uint32_t reserved15                    :    1;  /*Reserved.*/
            uint32_t wdt_delay_sel             :    2;  /*Represents whether RTC watchdog timeout threshold is selected at startup. 1: selected. 0: not selected.*/
            uint32_t spi_boot_crypt_cnt        :    3;  /*Represents whether SPI boot encrypt/decrypt is disabled or enabled. Odd number of 1: enabled. Even number of 1: disabled.*/
            uint32_t secure_boot_key_revoke0   :    1;  /*Represents whether revoking first secure boot key is enabled or disabled. 1: enabled. 0: disabled.*/
            uint32_t secure_boot_key_revoke1   :    1;  /*Represents whether revoking second secure boot key is enabled or disabled. 1: enabled. 0: disabled.*/
            uint32_t secure_boot_key_revoke2   :    1;  /*Represents whether revoking third secure boot key is enabled or disabled. 1: enabled. 0: disabled.*/
            uint32_t key_purpose_0             :    4;  /*Represents the purpose of Key0.*/
            uint32_t key_purpose_1             :    4;  /*Represents the purpose of Key1.*/
        };
        uint32_t val;
    } rd_repeat_data1;
    union {
        struct {
            uint32_t key_purpose_2             :    4;  /*Represents the purpose of Key2.*/
            uint32_t key_purpose_3             :    4;  /*Represents the purpose of Key3.*/
            uint32_t key_purpose_4             :    4;  /*Represents the purpose of Key4.*/
            uint32_t key_purpose_5             :    4;  /*Represents the purpose of Key5.*/
            uint32_t sec_dpa_level             :    2;  /*Represents the spa secure level by configuring the clock random divide mode.*/
            uint32_t ecdsa_enable_soft_k       :    1;  /*Represents whether hardware random number k is forced used in ESDCA. 1: force used. 0: not force used.*/
            uint32_t crypt_dpa_enable          :    1;  /*Represents whether anti-dpa attack is enabled. 1:enabled. 0: disabled.*/
            uint32_t secure_boot_en            :    1;  /*Represents whether secure boot is enabled or disabled. 1: enabled. 0: disabled.*/
            uint32_t secure_boot_aggressive_revoke:    1;  /*Represents whether revoking aggressive secure boot is enabled or disabled. 1: enabled. 0: disabled.*/
            uint32_t reserved22                    :    1;  /*Reserved.*/
            uint32_t flash_type                :    1;  /*The type of interfaced flash. 0: four data lines, 1: eight data lines.*/
            uint32_t flash_page_size           :    2;  /*Set flash page size.*/
            uint32_t flash_ecc_en              :    1;  /*Set this bit to enable ecc for flash boot.*/
            uint32_t dis_usb_otg_download_mode :    1;  /*Set this bit to disable download via USB-OTG.*/
            uint32_t flash_tpuw                :    4;  /*Represents the flash waiting time after power-up, in unit of ms. When the value less than 15, the waiting time is the programmed value. Otherwise, the waiting time is 2 times the programmed value.*/
        };
        uint32_t val;
    } rd_repeat_data2;
    union {
        struct {
            uint32_t dis_download_mode         :    1;  /*Represents whether Download mode is disabled or enabled. 1: disabled. 0: enabled.*/
            uint32_t dis_direct_boot           :    1;  /*Represents whether direct boot mode is disabled or enabled. 1: disabled. 0: enabled.*/
            uint32_t dis_usb_serial_jtag_rom_print:    1;  /*Represents whether print from USB-Serial-JTAG is disabled or enabled. 1: disabled. 0: enabled.*/
            uint32_t lock_km_key               :    1;  /*TBD*/
            uint32_t dis_usb_serial_jtag_download_mode:    1;  /*Represents whether the USB-Serial-JTAG download function is disabled or enabled. 1: disabled. 0: enabled.*/
            uint32_t enable_security_download  :    1;  /*Represents whether security download is enabled or disabled. 1: enabled. 0: disabled.*/
            uint32_t uart_print_control        :    2;  /*Represents the type of UART printing. 00: force enable printing. 01: enable printing when GPIO8 is reset at low level. 10: enable printing when GPIO8 is reset at high level. 11: force disable printing. */
            uint32_t force_send_resume         :    1;  /*Represents whether ROM code is forced to send a resume command during SPI boot. 1: forced. 0:not forced.*/
            uint32_t secure_version            :    16;  /*Represents the version used by ESP-IDF anti-rollback feature.*/
            uint32_t secure_boot_disable_fast_wake:    1;  /*Represents whether FAST VERIFY ON WAKE is disabled or enabled when Secure Boot is enabled. 1: disabled. 0: enabled.*/
            uint32_t hys_en_pad                :    1;  /*Represents whether the hysteresis function of corresponding PAD is enabled. 1: enabled. 0:disabled.*/
            uint32_t dcdc_vset                 :    5;  /*Set the dcdc voltage default.*/
        };
        uint32_t val;
    } rd_repeat_data3;
    union {
        struct {
            uint32_t _0pxa_tieh_sel_0           :    2;  /*TBD*/
            uint32_t _0pxa_tieh_sel_1           :    2;  /*TBD.*/
            uint32_t _0pxa_tieh_sel_2           :    2;  /*TBD.*/
            uint32_t _0pxa_tieh_sel_3           :    2;  /*TBD.*/
            uint32_t km_disable_deploy_mode    :    4;  /*TBD*/
            uint32_t usb_device_drefl          :    2;  /*Represents the usb device single-end input low threhold, 0.8 V to 1.04 V with step of 80 mV.*/
            uint32_t usb_otg11_drefl           :    2;  /*Represents the usb otg11 single-end input low threhold, 0.8 V to 1.04 V with step of 80 mV.*/
            uint32_t reserved16                    :    2;  /*Reserved.*/
            uint32_t hp_pwr_src_sel            :    1;  /*HP system power source select. 0:LDO. 1: DCDC.*/
            uint32_t dcdc_vset_en              :    1;  /*Select dcdc vset use efuse_dcdc_vset.*/
            uint32_t dis_wdt                   :    1;  /*Set this bit to disable watch dog.*/
            uint32_t dis_swd                   :    1;  /*Set this bit to disable super-watchdog.*/
            uint32_t reserved22                    :    2;  /*Reserved.*/
            uint32_t reserved24                    :    8;  /*Reserved.*/
        };
        uint32_t val;
    } rd_repeat_data4;
    uint32_t rd_mac_sys_0;
    union {
        struct {
            uint32_t mac_1                     :    16;  /*Stores the high 16 bits of MAC address.*/
            uint32_t mac_ext                   :    16;  /*Stores the extended bits of MAC address.*/
        };
        uint32_t val;
    } rd_mac_sys_1;
    union {
        struct {
            uint32_t mac_reserved_1            :    14;  /*Reserved.*/
            uint32_t mac_reserved_0            :    18;  /*Reserved.*/
        };
        uint32_t val;
    } rd_mac_sys_2;
    union {
        struct {
            uint32_t mac_reserved_2            :    18;  /*Reserved.*/
            uint32_t sys_data_part0_0          :    14;  /*Stores the first 14 bits of the zeroth part of system data.*/
        };
        uint32_t val;
    } rd_mac_sys_3;
    uint32_t rd_mac_sys_4;
    uint32_t rd_mac_sys_5;
    uint32_t rd_sys_part1_data0;
    uint32_t rd_sys_part1_data1;
    uint32_t rd_sys_part1_data2;
    uint32_t rd_sys_part1_data3;
    uint32_t rd_sys_part1_data4;
    uint32_t rd_sys_part1_data5;
    uint32_t rd_sys_part1_data6;
    uint32_t rd_sys_part1_data7;
    uint32_t rd_usr_data0;
    uint32_t rd_usr_data1;
    uint32_t rd_usr_data2;
    uint32_t rd_usr_data3;
    uint32_t rd_usr_data4;
    uint32_t rd_usr_data5;
    uint32_t rd_usr_data6;
    uint32_t rd_usr_data7;
    uint32_t rd_key0_data0;
    uint32_t rd_key0_data1;
    uint32_t rd_key0_data2;
    uint32_t rd_key0_data3;
    uint32_t rd_key0_data4;
    uint32_t rd_key0_data5;
    uint32_t rd_key0_data6;
    uint32_t rd_key0_data7;
    uint32_t rd_key1_data0;
    uint32_t rd_key1_data1;
    uint32_t rd_key1_data2;
    uint32_t rd_key1_data3;
    uint32_t rd_key1_data4;
    uint32_t rd_key1_data5;
    uint32_t rd_key1_data6;
    uint32_t rd_key1_data7;
    uint32_t rd_key2_data0;
    uint32_t rd_key2_data1;
    uint32_t rd_key2_data2;
    uint32_t rd_key2_data3;
    uint32_t rd_key2_data4;
    uint32_t rd_key2_data5;
    uint32_t rd_key2_data6;
    uint32_t rd_key2_data7;
    uint32_t rd_key3_data0;
    uint32_t rd_key3_data1;
    uint32_t rd_key3_data2;
    uint32_t rd_key3_data3;
    uint32_t rd_key3_data4;
    uint32_t rd_key3_data5;
    uint32_t rd_key3_data6;
    uint32_t rd_key3_data7;
    uint32_t rd_key4_data0;
    uint32_t rd_key4_data1;
    uint32_t rd_key4_data2;
    uint32_t rd_key4_data3;
    uint32_t rd_key4_data4;
    uint32_t rd_key4_data5;
    uint32_t rd_key4_data6;
    uint32_t rd_key4_data7;
    uint32_t rd_key5_data0;
    uint32_t rd_key5_data1;
    uint32_t rd_key5_data2;
    uint32_t rd_key5_data3;
    uint32_t rd_key5_data4;
    uint32_t rd_key5_data5;
    uint32_t rd_key5_data6;
    uint32_t rd_key5_data7;
    uint32_t rd_sys_part2_data0;
    uint32_t rd_sys_part2_data1;
    uint32_t rd_sys_part2_data2;
    uint32_t rd_sys_part2_data3;
    uint32_t rd_sys_part2_data4;
    uint32_t rd_sys_part2_data5;
    uint32_t rd_sys_part2_data6;
    uint32_t rd_sys_part2_data7;
    union {
        struct {
            uint32_t rd_dis_err                :    7;  /*Indicates a programming error of RD_DIS.*/
            uint32_t usb_device_exchg_pins_err :    1;  /*Indicates a programming error of DIS_USB_DEVICE_EXCHG_PINS.*/
            uint32_t usb_otg11_exchg_pins_err  :    1;  /*Indicates a programming error of DIS_USB_OTG11_EXCHG_PINS.*/
            uint32_t dis_usb_jtag_err          :    1;  /*Indicates a programming error of DIS_USB_JTAG.*/
            uint32_t powerglitch_en_err        :    1;  /*Indicates a programming error of POWERGLITCH_EN.*/
            uint32_t dis_usb_serial_jtag_err   :    1;  /*Indicates a programming error of DIS_USB_SERIAL_JTAG.*/
            uint32_t dis_force_download_err    :    1;  /*Indicates a programming error of DIS_FORCE_DOWNLOAD.*/
            uint32_t spi_download_mspi_dis_err :    1;  /*Indicates a programming error of SPI_DOWNLOAD_MSPI_DIS.*/
            uint32_t dis_twai_err              :    1;  /*Indicates a programming error of DIS_TWAI.*/
            uint32_t jtag_sel_enable_err       :    1;  /*Indicates a programming error of JTAG_SEL_ENABLE.*/
            uint32_t soft_dis_jtag_err         :    3;  /*Indicates a programming error of SOFT_DIS_JTAG.*/
            uint32_t dis_pad_jtag_err          :    1;  /*Indicates a programming error of DIS_PAD_JTAG.*/
            uint32_t dis_download_manual_encrypt_err:    1;  /*Indicates a programming error of DIS_DOWNLOAD_MANUAL_ENCRYPT.*/
            uint32_t usb_device_drefh_err      :    2;  /*Indicates a programming error of USB_DEVICE_DREFH.*/
            uint32_t usb_otg11_drefh_err       :    2;  /*Indicates a programming error of USB_OTG11_DREFH.*/
            uint32_t usb_phy_sel_err           :    1;  /*Indicates a programming error of USB_PHY_SEL.*/
            uint32_t huk_gen_state_low_err     :    6;  /*Indicates a programming error of HUK_GEN_STATE_LOW.*/
        };
        uint32_t val;
    } rd_repeat_err0;
    union {
        struct {
            uint32_t huk_gen_state_high_err    :    3;  /*Indicates a programming error of HUK_GEN_STATE_HIGH.*/
            uint32_t km_rnd_switch_cycle_err   :    2;  /*Indicates a programming error of KM_RND_SWITCH_CYCLE.*/
            uint32_t km_deploy_only_once_err   :    4;  /*Indicates a programming error of KM_DEPLOY_ONLY_ONCE.*/
            uint32_t force_use_key_manager_key_err:    4;  /*Indicates a programming error of FORCE_USE_KEY_MANAGER_KEY.*/
            uint32_t force_disable_sw_init_key_err:    1;  /*Indicates a programming error of FORCE_DISABLE_SW_INIT_KEY.*/
            uint32_t xts_key_length_256_err    :    1;  /*Indicates a programming error of XTS_KEY_LENGTH_256.*/
            uint32_t reserved15                    :    1;  /*Reserved.*/
            uint32_t wdt_delay_sel_err         :    2;  /*Indicates a programming error of WDT_DELAY_SEL.*/
            uint32_t spi_boot_crypt_cnt_err    :    3;  /*Indicates a programming error of SPI_BOOT_CRYPT_CNT.*/
            uint32_t secure_boot_key_revoke0_err:    1;  /*Indicates a programming error of SECURE_BOOT_KEY_REVOKE0.*/
            uint32_t secure_boot_key_revoke1_err:    1;  /*Indicates a programming error of SECURE_BOOT_KEY_REVOKE1.*/
            uint32_t secure_boot_key_revoke2_err:    1;  /*Indicates a programming error of SECURE_BOOT_KEY_REVOKE2.*/
            uint32_t key_purpose_0_err         :    4;  /*Indicates a programming error of KEY_PURPOSE_0.*/
            uint32_t key_purpose_1_err         :    4;  /*Indicates a programming error of KEY_PURPOSE_1.*/
        };
        uint32_t val;
    } rd_repeat_err1;
    union {
        struct {
            uint32_t key_purpose_2_err         :    4;  /*Indicates a programming error of KEY_PURPOSE_2.*/
            uint32_t key_purpose_3_err         :    4;  /*Indicates a programming error of KEY_PURPOSE_3.*/
            uint32_t key_purpose_4_err         :    4;  /*Indicates a programming error of KEY_PURPOSE_4.*/
            uint32_t key_purpose_5_err         :    4;  /*Indicates a programming error of KEY_PURPOSE_5.*/
            uint32_t sec_dpa_level_err         :    2;  /*Indicates a programming error of SEC_DPA_LEVEL.*/
            uint32_t ecdsa_enable_soft_k_err   :    1;  /*Indicates a programming error of ECDSA_FORCE_USE_HARDWARE_K.*/
            uint32_t crypt_dpa_enable_err      :    1;  /*Indicates a programming error of CRYPT_DPA_ENABLE.*/
            uint32_t secure_boot_en_err        :    1;  /*Indicates a programming error of SECURE_BOOT_EN.*/
            uint32_t secure_boot_aggressive_revoke_err:    1;  /*Indicates a programming error of SECURE_BOOT_AGGRESSIVE_REVOKE.*/
            uint32_t reserved22                    :    1;  /*Reserved.*/
            uint32_t flash_type_err            :    1;  /*Indicates a programming error of FLASH_TYPE.*/
            uint32_t flash_page_size_err       :    2;  /*Indicates a programming error of FLASH_PAGE_SIZE.*/
            uint32_t flash_ecc_en_err          :    1;  /*Indicates a programming error of FLASH_ECC_EN.*/
            uint32_t dis_usb_otg_download_mode_err:    1;  /*Indicates a programming error of DIS_USB_OTG_DOWNLOAD_MODE.*/
            uint32_t flash_tpuw_err            :    4;  /*Indicates a programming error of FLASH_TPUW.*/
        };
        uint32_t val;
    } rd_repeat_err2;
    union {
        struct {
            uint32_t dis_download_mode_err     :    1;  /*Indicates a programming error of DIS_DOWNLOAD_MODE.*/
            uint32_t dis_direct_boot_err       :    1;  /*Indicates a programming error of DIS_DIRECT_BOOT.*/
            uint32_t dis_usb_serial_jtag_rom_print_err:    1;  /*Indicates a programming error of DIS_USB_SERIAL_JTAG_ROM_PRINT_ERR.*/
            uint32_t lock_km_key_err           :    1;  /*TBD*/
            uint32_t dis_usb_serial_jtag_download_mode_err:    1;  /*Indicates a programming error of DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE.*/
            uint32_t enable_security_download_err:    1;  /*Indicates a programming error of ENABLE_SECURITY_DOWNLOAD.*/
            uint32_t uart_print_control_err    :    2;  /*Indicates a programming error of UART_PRINT_CONTROL.*/
            uint32_t force_send_resume_err     :    1;  /*Indicates a programming error of FORCE_SEND_RESUME.*/
            uint32_t secure_version_err        :    16;  /*Indicates a programming error of SECURE VERSION.*/
            uint32_t secure_boot_disable_fast_wake_err:    1;  /*Indicates a programming error of SECURE_BOOT_DISABLE_FAST_WAKE.*/
            uint32_t hys_en_pad_err            :    1;  /*Indicates a programming error of HYS_EN_PAD.*/
            uint32_t dcdc_vset_err             :    5;  /*Indicates a programming error of DCDC_VSET.*/
        };
        uint32_t val;
    } rd_repeat_err3;
    union {
        struct {
            uint32_t _0pxa_tieh_sel_0_err       :    2;  /*Indicates a programming error of 0PXA_TIEH_SEL_0.*/
            uint32_t _0pxa_tieh_sel_1_err       :    2;  /*Indicates a programming error of 0PXA_TIEH_SEL_1.*/
            uint32_t _0pxa_tieh_sel_2_err       :    2;  /*Indicates a programming error of 0PXA_TIEH_SEL_2.*/
            uint32_t _0pxa_tieh_sel_3_err       :    2;  /*Indicates a programming error of 0PXA_TIEH_SEL_3.*/
            uint32_t km_disable_deploy_mode_err:    4;  /*TBD.*/
            uint32_t usb_device_drefl_err      :    2;  /*Indicates a programming error of USB_DEVICE_DREFL.*/
            uint32_t usb_otg11_drefl_err       :    2;  /*Indicates a programming error of USB_OTG11_DREFL.*/
            uint32_t reserved16                    :    2;  /*Reserved.*/
            uint32_t hp_pwr_src_sel_err        :    1;  /*Indicates a programming error of HP_PWR_SRC_SEL.*/
            uint32_t dcdc_vset_en_err          :    1;  /*Indicates a programming error of DCDC_VSET_EN.*/
            uint32_t dis_wdt_err               :    1;  /*Indicates a programming error of DIS_WDT.*/
            uint32_t dis_swd_err               :    1;  /*Indicates a programming error of DIS_SWD.*/
            uint32_t reserved22                    :    2;  /*Reserved.*/
            uint32_t reserved24                    :    8;  /*Reserved.*/
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
            uint32_t mac_sys_err_num           :    3;  /*The value of this signal means the number of error bytes.*/
            uint32_t mac_sys_fail              :    1;  /*0: Means no failure and that the data of MAC_SPI_8M is reliable 1: Means that programming user data failed and the number of error bytes is over 6.*/
            uint32_t sys_part1_err_num         :    3;  /*The value of this signal means the number of error bytes.*/
            uint32_t sys_part1_fail            :    1;  /*0: Means no failure and that the data of system part1 is reliable 1: Means that programming user data failed and the number of error bytes is over 6.*/
            uint32_t usr_data_err_num          :    3;  /*The value of this signal means the number of error bytes.*/
            uint32_t usr_data_fail             :    1;  /*0: Means no failure and that the user data is reliable 1: Means that programming user data failed and the number of error bytes is over 6.*/
            uint32_t key0_err_num              :    3;  /*The value of this signal means the number of error bytes.*/
            uint32_t key0_fail                 :    1;  /*0: Means no failure and that the data of key0 is reliable 1: Means that programming key0 failed and the number of error bytes is over 6.*/
            uint32_t key1_err_num              :    3;  /*The value of this signal means the number of error bytes.*/
            uint32_t key1_fail                 :    1;  /*0: Means no failure and that the data of key1 is reliable 1: Means that programming key1 failed and the number of error bytes is over 6.*/
            uint32_t key2_err_num              :    3;  /*The value of this signal means the number of error bytes.*/
            uint32_t key2_fail                 :    1;  /*0: Means no failure and that the data of key2 is reliable 1: Means that programming key2 failed and the number of error bytes is over 6.*/
            uint32_t key3_err_num              :    3;  /*The value of this signal means the number of error bytes.*/
            uint32_t key3_fail                 :    1;  /*0: Means no failure and that the data of key3 is reliable 1: Means that programming key3 failed and the number of error bytes is over 6.*/
            uint32_t key4_err_num              :    3;  /*The value of this signal means the number of error bytes.*/
            uint32_t key4_fail                 :    1;  /*0: Means no failure and that the data of key4 is reliable 1: Means that programming key4 failed and the number of error bytes is over 6.*/
        };
        uint32_t val;
    } rd_rs_err0;
    union {
        struct {
            uint32_t key5_err_num              :    3;  /*The value of this signal means the number of error bytes.*/
            uint32_t key5_fail                 :    1;  /*0: Means no failure and that the data of key5 is reliable 1: Means that programming key5 failed and the number of error bytes is over 6.*/
            uint32_t sys_part2_err_num         :    3;  /*The value of this signal means the number of error bytes.*/
            uint32_t sys_part2_fail            :    1;  /*0: Means no failure and that the data of system part2 is reliable 1: Means that programming user data failed and the number of error bytes is over 6.*/
            uint32_t reserved8                     :    24;  /*Reserved.*/
        };
        uint32_t val;
    } rd_rs_err1;
    union {
        struct {
            uint32_t efuse_mem_force_pd        :    1;  /*Set this bit to force eFuse SRAM into power-saving mode.*/
            uint32_t efuse_mem_clk_force_on    :    1;  /*Set this bit and force to activate clock signal of eFuse SRAM.*/
            uint32_t efuse_mem_force_pu        :    1;  /*Set this bit to force eFuse SRAM into working mode.*/
            uint32_t reserved3                     :    13;  /*Reserved.*/
            uint32_t clk_en                    :    1;  /*Set this bit to force enable eFuse register configuration clock signal.*/
            uint32_t reserved17                    :    15;  /*Reserved.*/
        };
        uint32_t val;
    } clk;
    union {
        struct {
            uint32_t op_code                   :    16;  /*0x5A5A:  programming operation command 0x5AA5: read operation command.*/
            uint32_t cfg_ecdsa_blk             :    4;  /*Configures which block to use for ECDSA key output.*/
            uint32_t reserved20                    :    12;  /*Reserved.*/
        };
        uint32_t val;
    } conf;
    union {
        struct {
            uint32_t state                     :    4;  /*Indicates the state of the eFuse state machine.*/
            uint32_t otp_load_sw               :    1;  /*The value of OTP_LOAD_SW.*/
            uint32_t otp_vddq_c_sync2          :    1;  /*The value of OTP_VDDQ_C_SYNC2.*/
            uint32_t otp_strobe_sw             :    1;  /*The value of OTP_STROBE_SW.*/
            uint32_t otp_csb_sw                :    1;  /*The value of OTP_CSB_SW.*/
            uint32_t otp_pgenb_sw              :    1;  /*The value of OTP_PGENB_SW.*/
            uint32_t otp_vddq_is_sw            :    1;  /*The value of OTP_VDDQ_IS_SW.*/
            uint32_t blk0_valid_bit_cnt        :    10;  /*Indicates the number of block valid bit.*/
            uint32_t cur_ecdsa_blk             :    4;  /*Indicates which block is used for ECDSA key output.*/
            uint32_t reserved24                    :    8;  /*Reserved.*/
        };
        uint32_t val;
    } status;
    union {
        struct {
            uint32_t read_cmd                  :    1;  /*Set this bit to send read command.*/
            uint32_t pgm_cmd                   :    1;  /*Set this bit to send programming command.*/
            uint32_t blk_num                   :    4;  /*The serial number of the block to be programmed. Value 0-10 corresponds to block number 0-10, respectively.*/
            uint32_t reserved6                     :    26;  /*Reserved.*/
        };
        uint32_t val;
    } cmd;
    union {
        struct {
            uint32_t read_done_int_raw         :    1;  /*The raw bit signal for read_done interrupt.*/
            uint32_t pgm_done_int_raw          :    1;  /*The raw bit signal for pgm_done interrupt.*/
            uint32_t reserved2                     :    30;  /*Reserved.*/
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t read_done_int_st          :    1;  /*The status signal for read_done interrupt.*/
            uint32_t pgm_done_int_st           :    1;  /*The status signal for pgm_done interrupt.*/
            uint32_t reserved2                     :    30;  /*Reserved.*/
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t read_done_int_ena         :    1;  /*The enable signal for read_done interrupt.*/
            uint32_t pgm_done_int_ena          :    1;  /*The enable signal for pgm_done interrupt.*/
            uint32_t reserved2                     :    30;  /*Reserved.*/
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t read_done_int_clr         :    1;  /*The clear signal for read_done interrupt.*/
            uint32_t pgm_done_int_clr          :    1;  /*The clear signal for pgm_done interrupt.*/
            uint32_t reserved2                     :    30;  /*Reserved.*/
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t dac_clk_div               :    8;  /*Controls the division factor of the rising clock of the programming voltage.*/
            uint32_t dac_clk_pad_sel           :    1;  /*Don't care.*/
            uint32_t dac_num                   :    8;  /*Controls the rising period of the programming voltage.*/
            uint32_t oe_clr                    :    1;  /*Reduces the power supply of the programming voltage.*/
            uint32_t reserved18                    :    14;  /*Reserved.*/
        };
        uint32_t val;
    } dac_conf;
    union {
        struct {
            uint32_t thr_a                     :    8;  /*Configures the read hold time.*/
            uint32_t trd                       :    8;  /*Configures the read time.*/
            uint32_t tsur_a                    :    8;  /*Configures the read setup time.*/
            uint32_t read_init_num             :    8;  /*Configures the waiting time of reading eFuse memory.*/
        };
        uint32_t val;
    } rd_tim_conf;
    union {
        struct {
            uint32_t tsup_a                    :    8;  /*Configures the programming setup time.*/
            uint32_t pwr_on_num                :    16;  /*Configures the power up time for VDDQ.*/
            uint32_t thp_a                     :    8;  /*Configures the programming hold time.*/
        };
        uint32_t val;
    } wr_tim_conf1;
    union {
        struct {
            uint32_t pwr_off_num               :    16;  /*Configures the power outage time for VDDQ.*/
            uint32_t tpgm                      :    16;  /*Configures the active programming time.*/
        };
        uint32_t val;
    } wr_tim_conf2;
    union {
        struct {
            uint32_t bypass_rs_correction      :    1;  /*Set this bit to bypass reed solomon correction step.*/
            uint32_t bypass_rs_blk_num         :    11;  /*Configures block number of programming twice operation.*/
            uint32_t update                    :    1;  /*Set this bit to update multi-bit register signals.*/
            uint32_t tpgm_inactive             :    8;  /*Configures the inactive programming time.*/
            uint32_t reserved21                    :    11;  /*Reserved.*/
        };
        uint32_t val;
    } wr_tim_conf0_rs_bypass;
    union {
        struct {
            uint32_t date                      :    28;  /*Stores eFuse version.*/
            uint32_t reserved28                    :    4;  /*Reserved.*/
        };
        uint32_t val;
    } date;
    uint32_t reserved_200;
    uint32_t reserved_204;
    uint32_t reserved_208;
    uint32_t reserved_20c;
    uint32_t reserved_210;
    uint32_t reserved_214;
    uint32_t reserved_218;
    uint32_t reserved_21c;
    uint32_t reserved_220;
    uint32_t reserved_224;
    uint32_t reserved_228;
    uint32_t reserved_22c;
    uint32_t reserved_230;
    uint32_t reserved_234;
    uint32_t reserved_238;
    uint32_t reserved_23c;
    uint32_t reserved_240;
    uint32_t reserved_244;
    uint32_t reserved_248;
    uint32_t reserved_24c;
    uint32_t reserved_250;
    uint32_t reserved_254;
    uint32_t reserved_258;
    uint32_t reserved_25c;
    uint32_t reserved_260;
    uint32_t reserved_264;
    uint32_t reserved_268;
    uint32_t reserved_26c;
    uint32_t reserved_270;
    uint32_t reserved_274;
    uint32_t reserved_278;
    uint32_t reserved_27c;
    uint32_t reserved_280;
    uint32_t reserved_284;
    uint32_t reserved_288;
    uint32_t reserved_28c;
    uint32_t reserved_290;
    uint32_t reserved_294;
    uint32_t reserved_298;
    uint32_t reserved_29c;
    uint32_t reserved_2a0;
    uint32_t reserved_2a4;
    uint32_t reserved_2a8;
    uint32_t reserved_2ac;
    uint32_t reserved_2b0;
    uint32_t reserved_2b4;
    uint32_t reserved_2b8;
    uint32_t reserved_2bc;
    uint32_t reserved_2c0;
    uint32_t reserved_2c4;
    uint32_t reserved_2c8;
    uint32_t reserved_2cc;
    uint32_t reserved_2d0;
    uint32_t reserved_2d4;
    uint32_t reserved_2d8;
    uint32_t reserved_2dc;
    uint32_t reserved_2e0;
    uint32_t reserved_2e4;
    uint32_t reserved_2e8;
    uint32_t reserved_2ec;
    uint32_t reserved_2f0;
    uint32_t reserved_2f4;
    uint32_t reserved_2f8;
    uint32_t reserved_2fc;
    uint32_t reserved_300;
    uint32_t reserved_304;
    uint32_t reserved_308;
    uint32_t reserved_30c;
    uint32_t reserved_310;
    uint32_t reserved_314;
    uint32_t reserved_318;
    uint32_t reserved_31c;
    uint32_t reserved_320;
    uint32_t reserved_324;
    uint32_t reserved_328;
    uint32_t reserved_32c;
    uint32_t reserved_330;
    uint32_t reserved_334;
    uint32_t reserved_338;
    uint32_t reserved_33c;
    uint32_t reserved_340;
    uint32_t reserved_344;
    uint32_t reserved_348;
    uint32_t reserved_34c;
    uint32_t reserved_350;
    uint32_t reserved_354;
    uint32_t reserved_358;
    uint32_t reserved_35c;
    uint32_t reserved_360;
    uint32_t reserved_364;
    uint32_t reserved_368;
    uint32_t reserved_36c;
    uint32_t reserved_370;
    uint32_t reserved_374;
    uint32_t reserved_378;
    uint32_t reserved_37c;
    uint32_t reserved_380;
    uint32_t reserved_384;
    uint32_t reserved_388;
    uint32_t reserved_38c;
    uint32_t reserved_390;
    uint32_t reserved_394;
    uint32_t reserved_398;
    uint32_t reserved_39c;
    uint32_t reserved_3a0;
    uint32_t reserved_3a4;
    uint32_t reserved_3a8;
    uint32_t reserved_3ac;
    uint32_t reserved_3b0;
    uint32_t reserved_3b4;
    uint32_t reserved_3b8;
    uint32_t reserved_3bc;
    uint32_t reserved_3c0;
    uint32_t reserved_3c4;
    uint32_t reserved_3c8;
    uint32_t reserved_3cc;
    uint32_t reserved_3d0;
    uint32_t reserved_3d4;
    uint32_t reserved_3d8;
    uint32_t reserved_3dc;
    uint32_t reserved_3e0;
    uint32_t reserved_3e4;
    uint32_t reserved_3e8;
    uint32_t reserved_3ec;
    uint32_t reserved_3f0;
    uint32_t reserved_3f4;
    uint32_t reserved_3f8;
    uint32_t reserved_3fc;
    uint32_t reserved_400;
    uint32_t reserved_404;
    uint32_t reserved_408;
    uint32_t reserved_40c;
    uint32_t reserved_410;
    uint32_t reserved_414;
    uint32_t reserved_418;
    uint32_t reserved_41c;
    uint32_t reserved_420;
    uint32_t reserved_424;
    uint32_t reserved_428;
    uint32_t reserved_42c;
    uint32_t reserved_430;
    uint32_t reserved_434;
    uint32_t reserved_438;
    uint32_t reserved_43c;
    uint32_t reserved_440;
    uint32_t reserved_444;
    uint32_t reserved_448;
    uint32_t reserved_44c;
    uint32_t reserved_450;
    uint32_t reserved_454;
    uint32_t reserved_458;
    uint32_t reserved_45c;
    uint32_t reserved_460;
    uint32_t reserved_464;
    uint32_t reserved_468;
    uint32_t reserved_46c;
    uint32_t reserved_470;
    uint32_t reserved_474;
    uint32_t reserved_478;
    uint32_t reserved_47c;
    uint32_t reserved_480;
    uint32_t reserved_484;
    uint32_t reserved_488;
    uint32_t reserved_48c;
    uint32_t reserved_490;
    uint32_t reserved_494;
    uint32_t reserved_498;
    uint32_t reserved_49c;
    uint32_t reserved_4a0;
    uint32_t reserved_4a4;
    uint32_t reserved_4a8;
    uint32_t reserved_4ac;
    uint32_t reserved_4b0;
    uint32_t reserved_4b4;
    uint32_t reserved_4b8;
    uint32_t reserved_4bc;
    uint32_t reserved_4c0;
    uint32_t reserved_4c4;
    uint32_t reserved_4c8;
    uint32_t reserved_4cc;
    uint32_t reserved_4d0;
    uint32_t reserved_4d4;
    uint32_t reserved_4d8;
    uint32_t reserved_4dc;
    uint32_t reserved_4e0;
    uint32_t reserved_4e4;
    uint32_t reserved_4e8;
    uint32_t reserved_4ec;
    uint32_t reserved_4f0;
    uint32_t reserved_4f4;
    uint32_t reserved_4f8;
    uint32_t reserved_4fc;
    uint32_t reserved_500;
    uint32_t reserved_504;
    uint32_t reserved_508;
    uint32_t reserved_50c;
    uint32_t reserved_510;
    uint32_t reserved_514;
    uint32_t reserved_518;
    uint32_t reserved_51c;
    uint32_t reserved_520;
    uint32_t reserved_524;
    uint32_t reserved_528;
    uint32_t reserved_52c;
    uint32_t reserved_530;
    uint32_t reserved_534;
    uint32_t reserved_538;
    uint32_t reserved_53c;
    uint32_t reserved_540;
    uint32_t reserved_544;
    uint32_t reserved_548;
    uint32_t reserved_54c;
    uint32_t reserved_550;
    uint32_t reserved_554;
    uint32_t reserved_558;
    uint32_t reserved_55c;
    uint32_t reserved_560;
    uint32_t reserved_564;
    uint32_t reserved_568;
    uint32_t reserved_56c;
    uint32_t reserved_570;
    uint32_t reserved_574;
    uint32_t reserved_578;
    uint32_t reserved_57c;
    uint32_t reserved_580;
    uint32_t reserved_584;
    uint32_t reserved_588;
    uint32_t reserved_58c;
    uint32_t reserved_590;
    uint32_t reserved_594;
    uint32_t reserved_598;
    uint32_t reserved_59c;
    uint32_t reserved_5a0;
    uint32_t reserved_5a4;
    uint32_t reserved_5a8;
    uint32_t reserved_5ac;
    uint32_t reserved_5b0;
    uint32_t reserved_5b4;
    uint32_t reserved_5b8;
    uint32_t reserved_5bc;
    uint32_t reserved_5c0;
    uint32_t reserved_5c4;
    uint32_t reserved_5c8;
    uint32_t reserved_5cc;
    uint32_t reserved_5d0;
    uint32_t reserved_5d4;
    uint32_t reserved_5d8;
    uint32_t reserved_5dc;
    uint32_t reserved_5e0;
    uint32_t reserved_5e4;
    uint32_t reserved_5e8;
    uint32_t reserved_5ec;
    uint32_t reserved_5f0;
    uint32_t reserved_5f4;
    uint32_t reserved_5f8;
    uint32_t reserved_5fc;
    uint32_t reserved_600;
    uint32_t reserved_604;
    uint32_t reserved_608;
    uint32_t reserved_60c;
    uint32_t reserved_610;
    uint32_t reserved_614;
    uint32_t reserved_618;
    uint32_t reserved_61c;
    uint32_t reserved_620;
    uint32_t reserved_624;
    uint32_t reserved_628;
    uint32_t reserved_62c;
    uint32_t reserved_630;
    uint32_t reserved_634;
    uint32_t reserved_638;
    uint32_t reserved_63c;
    uint32_t reserved_640;
    uint32_t reserved_644;
    uint32_t reserved_648;
    uint32_t reserved_64c;
    uint32_t reserved_650;
    uint32_t reserved_654;
    uint32_t reserved_658;
    uint32_t reserved_65c;
    uint32_t reserved_660;
    uint32_t reserved_664;
    uint32_t reserved_668;
    uint32_t reserved_66c;
    uint32_t reserved_670;
    uint32_t reserved_674;
    uint32_t reserved_678;
    uint32_t reserved_67c;
    uint32_t reserved_680;
    uint32_t reserved_684;
    uint32_t reserved_688;
    uint32_t reserved_68c;
    uint32_t reserved_690;
    uint32_t reserved_694;
    uint32_t reserved_698;
    uint32_t reserved_69c;
    uint32_t reserved_6a0;
    uint32_t reserved_6a4;
    uint32_t reserved_6a8;
    uint32_t reserved_6ac;
    uint32_t reserved_6b0;
    uint32_t reserved_6b4;
    uint32_t reserved_6b8;
    uint32_t reserved_6bc;
    uint32_t reserved_6c0;
    uint32_t reserved_6c4;
    uint32_t reserved_6c8;
    uint32_t reserved_6cc;
    uint32_t reserved_6d0;
    uint32_t reserved_6d4;
    uint32_t reserved_6d8;
    uint32_t reserved_6dc;
    uint32_t reserved_6e0;
    uint32_t reserved_6e4;
    uint32_t reserved_6e8;
    uint32_t reserved_6ec;
    uint32_t reserved_6f0;
    uint32_t reserved_6f4;
    uint32_t reserved_6f8;
    uint32_t reserved_6fc;
    uint32_t reserved_700;
    uint32_t reserved_704;
    uint32_t reserved_708;
    uint32_t reserved_70c;
    uint32_t reserved_710;
    uint32_t reserved_714;
    uint32_t reserved_718;
    uint32_t reserved_71c;
    uint32_t reserved_720;
    uint32_t reserved_724;
    uint32_t reserved_728;
    uint32_t reserved_72c;
    uint32_t reserved_730;
    uint32_t reserved_734;
    uint32_t reserved_738;
    uint32_t reserved_73c;
    uint32_t reserved_740;
    uint32_t reserved_744;
    uint32_t reserved_748;
    uint32_t reserved_74c;
    uint32_t reserved_750;
    uint32_t reserved_754;
    uint32_t reserved_758;
    uint32_t reserved_75c;
    uint32_t reserved_760;
    uint32_t reserved_764;
    uint32_t reserved_768;
    uint32_t reserved_76c;
    uint32_t reserved_770;
    uint32_t reserved_774;
    uint32_t reserved_778;
    uint32_t reserved_77c;
    uint32_t reserved_780;
    uint32_t reserved_784;
    uint32_t reserved_788;
    uint32_t reserved_78c;
    uint32_t reserved_790;
    uint32_t reserved_794;
    uint32_t reserved_798;
    uint32_t reserved_79c;
    uint32_t reserved_7a0;
    uint32_t reserved_7a4;
    uint32_t reserved_7a8;
    uint32_t reserved_7ac;
    uint32_t reserved_7b0;
    uint32_t reserved_7b4;
    uint32_t reserved_7b8;
    uint32_t reserved_7bc;
    uint32_t reserved_7c0;
    uint32_t reserved_7c4;
    uint32_t reserved_7c8;
    uint32_t reserved_7cc;
    uint32_t reserved_7d0;
    uint32_t reserved_7d4;
    uint32_t reserved_7d8;
    uint32_t reserved_7dc;
    uint32_t reserved_7e0;
    uint32_t reserved_7e4;
    uint32_t reserved_7e8;
    uint32_t reserved_7ec;
    uint32_t reserved_7f0;
    uint32_t reserved_7f4;
    uint32_t reserved_7f8;
    uint32_t reserved_7fc;
    uint32_t apb2otp_wr_dis;
    uint32_t apb2otp_blk0_backup1_w1;
    uint32_t apb2otp_blk0_backup1_w2;
    uint32_t apb2otp_blk0_backup1_w3;
    uint32_t apb2otp_blk0_backup1_w4;
    uint32_t apb2otp_blk0_backup1_w5;
    uint32_t apb2otp_blk0_backup2_w1;
    uint32_t apb2otp_blk0_backup2_w2;
    uint32_t apb2otp_blk0_backup2_w3;
    uint32_t apb2otp_blk0_backup2_w4;
    uint32_t apb2otp_blk0_backup2_w5;
    uint32_t apb2otp_blk0_backup3_w1;
    uint32_t apb2otp_blk0_backup3_w2;
    uint32_t apb2otp_blk0_backup3_w3;
    uint32_t apb2otp_blk0_backup3_w4;
    uint32_t apb2otp_blk0_backup3_w5;
    uint32_t apb2otp_blk0_backup4_w1;
    uint32_t apb2otp_blk0_backup4_w2;
    uint32_t apb2otp_blk0_backup4_w3;
    uint32_t apb2otp_blk0_backup4_w4;
    uint32_t apb2otp_blk0_backup4_w5;
    uint32_t apb2otp_blk1_w1;
    uint32_t apb2otp_blk1_w2;
    uint32_t apb2otp_blk1_w3;
    uint32_t apb2otp_blk1_w4;
    uint32_t apb2otp_blk1_w5;
    uint32_t apb2otp_blk1_w6;
    uint32_t apb2otp_blk1_w7;
    uint32_t apb2otp_blk1_w8;
    uint32_t apb2otp_blk1_w9;
    uint32_t apb2otp_blk2_w1;
    uint32_t apb2otp_blk2_w2;
    uint32_t apb2otp_blk2_w3;
    uint32_t apb2otp_blk2_w4;
    uint32_t apb2otp_blk2_w5;
    uint32_t apb2otp_blk2_w6;
    uint32_t apb2otp_blk2_w7;
    uint32_t apb2otp_blk2_w8;
    uint32_t apb2otp_blk2_w9;
    uint32_t apb2otp_blk2_w10;
    uint32_t apb2otp_blk2_w11;
    uint32_t apb2otp_blk3_w1;
    uint32_t apb2otp_blk3_w2;
    uint32_t apb2otp_blk3_w3;
    uint32_t apb2otp_blk3_w4;
    uint32_t apb2otp_blk3_w5;
    uint32_t apb2otp_blk3_w6;
    uint32_t apb2otp_blk3_w7;
    uint32_t apb2otp_blk3_w8;
    uint32_t apb2otp_blk3_w9;
    uint32_t apb2otp_blk3_w10;
    uint32_t apb2otp_blk3_w11;
    uint32_t apb2otp_blk4_w1;
    uint32_t apb2otp_blk4_w2;
    uint32_t apb2otp_blk4_w3;
    uint32_t apb2otp_blk4_w4;
    uint32_t apb2otp_blk4_w5;
    uint32_t apb2otp_blk4_w6;
    uint32_t apb2otp_blk4_w7;
    uint32_t apb2otp_blk4_w8;
    uint32_t apb2otp_blk4_w9;
    uint32_t apb2otp_blk4_w10;
    uint32_t apb2otp_blk4_w11;
    uint32_t apb2otp_blk5_w1;
    uint32_t apb2otp_blk5_w2;
    uint32_t apb2otp_blk5_w3;
    uint32_t apb2otp_blk5_w4;
    uint32_t apb2otp_blk5_w5;
    uint32_t apb2otp_blk5_w6;
    uint32_t apb2otp_blk5_w7;
    uint32_t apb2otp_blk5_w8;
    uint32_t apb2otp_blk5_w9;
    uint32_t apb2otp_blk5_w10;
    uint32_t apb2otp_blk5_w11;
    uint32_t apb2otp_blk6_w1;
    uint32_t apb2otp_blk6_w2;
    uint32_t apb2otp_blk6_w3;
    uint32_t apb2otp_blk6_w4;
    uint32_t apb2otp_blk6_w5;
    uint32_t apb2otp_blk6_w6;
    uint32_t apb2otp_blk6_w7;
    uint32_t apb2otp_blk6_w8;
    uint32_t apb2otp_blk6_w9;
    uint32_t apb2otp_blk6_w10;
    uint32_t apb2otp_blk6_w11;
    uint32_t apb2otp_blk7_w1;
    uint32_t apb2otp_blk7_w2;
    uint32_t apb2otp_blk7_w3;
    uint32_t apb2otp_blk7_w4;
    uint32_t apb2otp_blk7_w5;
    uint32_t apb2otp_blk7_w6;
    uint32_t apb2otp_blk7_w7;
    uint32_t apb2otp_blk7_w8;
    uint32_t apb2otp_blk7_w9;
    uint32_t apb2otp_blk7_w10;
    uint32_t apb2otp_blk7_w11;
    uint32_t apb2otp_blk8_w1;
    uint32_t apb2otp_blk8_w2;
    uint32_t apb2otp_blk8_w3;
    uint32_t apb2otp_blk8_w4;
    uint32_t apb2otp_blk8_w5;
    uint32_t apb2otp_blk8_w6;
    uint32_t apb2otp_blk8_w7;
    uint32_t apb2otp_blk8_w8;
    uint32_t apb2otp_blk8_w9;
    uint32_t apb2otp_blk8_w10;
    uint32_t apb2otp_blk8_w11;
    uint32_t apb2otp_blk9_w1;
    uint32_t apb2otp_blk9_w2;
    uint32_t apb2otp_blk9_w3;
    uint32_t apb2otp_blk9_w4;
    uint32_t apb2otp_blk9_w5;
    uint32_t apb2otp_blk9_w6;
    uint32_t apb2otp_blk9_w7;
    uint32_t apb2otp_blk9_w8;
    uint32_t apb2otp_blk9_w9;
    uint32_t apb2otp_blk9_w10;
    uint32_t apb2otp_blk9_w11;
    uint32_t apb2otp_blk10_w1;
    uint32_t apb2otp_blk10_w2;
    uint32_t apb2otp_blk10_w3;
    uint32_t apb2otp_blk10_w4;
    uint32_t apb2otp_blk10_w5;
    uint32_t apb2otp_blk10_w6;
    uint32_t apb2otp_blk10_w7;
    uint32_t apb2otp_blk10_w8;
    uint32_t apb2otp_blk10_w9;
    uint32_t apb2otp_blk10_w10;
    uint32_t apb2otp_blk10_w11;
    uint32_t reserved_a04;
    union {
        struct {
            uint32_t apb2otp_enable            :    1;  /*Apb2otp mode enable signal.*/
            uint32_t reserved1                     :    31;  /*Reserved.*/
        };
        uint32_t val;
    } apb2otp_en;
} efuse_dev_t;

extern efuse_dev_t EFUSE;
#ifdef __cplusplus
}
#endif
