/*
 * SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "esp_efuse.h"
#include <assert.h>
#include "esp_efuse_table.h"

// md5_digest_table 57d7fbd8410e3a4d1e0d19e39b9cb3c0
// This file was generated from the file esp_efuse_table.csv. DO NOT CHANGE THIS FILE MANUALLY.
// If you want to change some fields, you need to change esp_efuse_table.csv file
// then run `efuse_common_table` or `efuse_custom_table` command it will generate this file.
// To show efuse_table run the command 'show_efuse_table'.

static const esp_efuse_desc_t WR_DIS[] = {
    {EFUSE_BLK0, 0, 32}, 	 // Write protection,
};

static const esp_efuse_desc_t WR_DIS_RD_DIS[] = {
    {EFUSE_BLK0, 0, 1}, 	 // Write protection for RD_DIS_KEY0 RD_DIS_KEY1 RD_DIS_KEY2 RD_DIS_KEY3 RD_DIS_KEY4 RD_DIS_KEY5 RD_DIS_SYS_DATA_PART2,
};

static const esp_efuse_desc_t WR_DIS_GROUP_1[] = {
    {EFUSE_BLK0, 1, 1}, 	 // Write protection for KM_RND_SWITCH_CYCLE KM_DEPLOY_ONLY_ONCE FORCE_USE_KEY_MANAGER_KEY FORCE_DISABLE_SW_INIT_KEY,
};

static const esp_efuse_desc_t WR_DIS_GROUP_2[] = {
    {EFUSE_BLK0, 2, 1}, 	 // Write protection for DIS_USB_JTAG POWERGLITCH_EN DIS_USB_DERIAL_JTAG DIS_FORCE_DOWNLOAD DIS_TWAI JTAG_SEL_ENABLE DIS_PAD_JTAG DIS_DOWNLOAD_MANUAL_ENCRYPT WDT_DELAY_SEL HYS_EN_PAD _0PXA_TIEH_SEL_0 _0PXA_TIEH_SEL_1 _0PXA_TIEH_SEL_2 _0PXA_TIEH_SEL_3 DIS_WDT DIS_SWD,
};

static const esp_efuse_desc_t WR_DIS_HP_PWR_SRC_SEL[] = {
    {EFUSE_BLK0, 3, 1}, 	 // Write protection for HP_PWR_SRC_SEL,
};

static const esp_efuse_desc_t WR_DIS_SPI_BOOT_CRYPT_CNT[] = {
    {EFUSE_BLK0, 4, 1}, 	 // Write protection for SPI_BOOT_CRYPT_CNT,
};

static const esp_efuse_desc_t WR_DIS_SECURE_BOOT_KEY_REVOKE0[] = {
    {EFUSE_BLK0, 5, 1}, 	 // Write protection for SECURE_BOOT_KEY_REVOKE0,
};

static const esp_efuse_desc_t WR_DIS_SECURE_BOOT_KEY_REVOKE1[] = {
    {EFUSE_BLK0, 6, 1}, 	 // Write protection for SECURE_BOOT_KEY_REVOKE1,
};

static const esp_efuse_desc_t WR_DIS_SECURE_BOOT_KEY_REVOKE2[] = {
    {EFUSE_BLK0, 7, 1}, 	 // Write protection for SECURE_BOOT_KEY_REVOKE2,
};

static const esp_efuse_desc_t WR_DIS_KEY0_PURPOSE[] = {
    {EFUSE_BLK0, 8, 1}, 	 // Write protection for key_purpose. KEY0,
};

static const esp_efuse_desc_t WR_DIS_KEY1_PURPOSE[] = {
    {EFUSE_BLK0, 9, 1}, 	 // Write protection for key_purpose. KEY1,
};

static const esp_efuse_desc_t WR_DIS_KEY2_PURPOSE[] = {
    {EFUSE_BLK0, 10, 1}, 	 // Write protection for key_purpose. KEY2,
};

static const esp_efuse_desc_t WR_DIS_KEY3_PURPOSE[] = {
    {EFUSE_BLK0, 11, 1}, 	 // Write protection for key_purpose. KEY3,
};

static const esp_efuse_desc_t WR_DIS_KEY4_PURPOSE[] = {
    {EFUSE_BLK0, 12, 1}, 	 // Write protection for key_purpose. KEY4,
};

static const esp_efuse_desc_t WR_DIS_KEY5_PURPOSE[] = {
    {EFUSE_BLK0, 13, 1}, 	 // Write protection for key_purpose. KEY5,
};

static const esp_efuse_desc_t WR_DIS_GROUP_3[] = {
    {EFUSE_BLK0, 14, 1}, 	 // Write protection for SEC_DPA_LEVEL CRYPT_DPA_ENABLE,
};

static const esp_efuse_desc_t WR_DIS_SECURE_BOOT_EN[] = {
    {EFUSE_BLK0, 15, 1}, 	 // Write protection for SECURE_BOOT_EN,
};

static const esp_efuse_desc_t WR_DIS_SECURE_BOOT_AGGRESSIVE_REVOKE[] = {
    {EFUSE_BLK0, 16, 1}, 	 // Write protection for SECURE_BOOT_AGGRESSIVE_REVOKE,
};

static const esp_efuse_desc_t WR_DIS_ECDSA_FORCE_USE_HARDWARE_K[] = {
    {EFUSE_BLK0, 17, 1}, 	 // Write protection for ECDSA_FORCE_USE_HARDWARE_K,
};

static const esp_efuse_desc_t WR_DIS_GROUP_4[] = {
    {EFUSE_BLK0, 18, 1}, 	 // Write protection for FLASH_TYPE FLASH_PAGE_SIZE FLASH_ECC_EN DIS_USB_OTG_DOWNLOAD_MODE FLASH_TPUW DIS_DOWNLOAD_MODE DIS_DIRECT_BOOT DIS_USB_SERIAL_JTAG_ROM_PRINT DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE ENABLE_SECURITY_DOWNLOAD UART_PRINT_CONTROL FORCE_SEND_RESUME SECURE_VERSION SECURE_BOOT_DISABLE_FAST_WAKE,
};

static const esp_efuse_desc_t WR_DIS_HUK_GEN_STATE[] = {
    {EFUSE_BLK0, 19, 1}, 	 // Write protection for HUK_GEN_STATE,
};

static const esp_efuse_desc_t WR_DIS_BLK1[] = {
    {EFUSE_BLK0, 20, 1}, 	 // Write protection for EFUSE_BLK1.  MAC_SPI_8M_SYS,
};

static const esp_efuse_desc_t WR_DIS_SYS_DATA_PART1[] = {
    {EFUSE_BLK0, 21, 1}, 	 // Write protection for EFUSE_BLK2.  SYS_DATA_PART1,
};

static const esp_efuse_desc_t WR_DIS_USER_DATA[] = {
    {EFUSE_BLK0, 22, 1}, 	 // Write protection for EFUSE_BLK3.  USER_DATA,
};

static const esp_efuse_desc_t WR_DIS_KEY0[] = {
    {EFUSE_BLK0, 23, 1}, 	 // Write protection for EFUSE_BLK4.  KEY0,
};

static const esp_efuse_desc_t WR_DIS_KEY1[] = {
    {EFUSE_BLK0, 24, 1}, 	 // Write protection for EFUSE_BLK5.  KEY1,
};

static const esp_efuse_desc_t WR_DIS_KEY2[] = {
    {EFUSE_BLK0, 25, 1}, 	 // Write protection for EFUSE_BLK6.  KEY2,
};

static const esp_efuse_desc_t WR_DIS_KEY3[] = {
    {EFUSE_BLK0, 26, 1}, 	 // Write protection for EFUSE_BLK7.  KEY3,
};

static const esp_efuse_desc_t WR_DIS_KEY4[] = {
    {EFUSE_BLK0, 27, 1}, 	 // Write protection for EFUSE_BLK8.  KEY4,
};

static const esp_efuse_desc_t WR_DIS_KEY5[] = {
    {EFUSE_BLK0, 28, 1}, 	 // Write protection for EFUSE_BLK9.  KEY5,
};

static const esp_efuse_desc_t WR_DIS_SYS_DATA_PART2[] = {
    {EFUSE_BLK0, 29, 1}, 	 // Write protection for EFUSE_BLK10. SYS_DATA_PART2,
};

static const esp_efuse_desc_t WR_DIS_GROUP_5[] = {
    {EFUSE_BLK0, 30, 1}, 	 // Write protection for USB_DEVICE_EXCHG_PINS USB_OTG11_EXCHG_PINS USB_DEVICE_DREFH USB_OTG11_DREFH USB_PHY_SEL USB_DEVICE_DREFL USB_OTG11_DREFL,
};

static const esp_efuse_desc_t WR_DIS_SOFT_DIS_JTAG[] = {
    {EFUSE_BLK0, 31, 1}, 	 // Write protection for SOFT_DIS_JTAG,
};

static const esp_efuse_desc_t RD_DIS[] = {
    {EFUSE_BLK0, 32, 7}, 	 // Read protection,
};

static const esp_efuse_desc_t RD_DIS_KEY0[] = {
    {EFUSE_BLK0, 32, 1}, 	 // Read protection for EFUSE_BLK4.  KEY0,
};

static const esp_efuse_desc_t RD_DIS_KEY1[] = {
    {EFUSE_BLK0, 33, 1}, 	 // Read protection for EFUSE_BLK5.  KEY1,
};

static const esp_efuse_desc_t RD_DIS_KEY2[] = {
    {EFUSE_BLK0, 34, 1}, 	 // Read protection for EFUSE_BLK6.  KEY2,
};

static const esp_efuse_desc_t RD_DIS_KEY3[] = {
    {EFUSE_BLK0, 35, 1}, 	 // Read protection for EFUSE_BLK7.  KEY3,
};

static const esp_efuse_desc_t RD_DIS_KEY4[] = {
    {EFUSE_BLK0, 36, 1}, 	 // Read protection for EFUSE_BLK8.  KEY4,
};

static const esp_efuse_desc_t RD_DIS_KEY5[] = {
    {EFUSE_BLK0, 37, 1}, 	 // Read protection for EFUSE_BLK9.  KEY5,
};

static const esp_efuse_desc_t RD_DIS_SYS_DATA_PART2[] = {
    {EFUSE_BLK0, 38, 1}, 	 // Read protection for EFUSE_BLK10. SYS_DATA_PART2,
};

static const esp_efuse_desc_t USB_DEVICE_EXCHG_PINS[] = {
    {EFUSE_BLK0, 39, 1}, 	 // USB-device exchange pins,
};

static const esp_efuse_desc_t USB_OTG11_EXCHG_PINS[] = {
    {EFUSE_BLK0, 40, 1}, 	 // USB-otg11 change pins,
};

static const esp_efuse_desc_t DIS_USB_JTAG[] = {
    {EFUSE_BLK0, 41, 1}, 	 // Disable USB JTAG,
};

static const esp_efuse_desc_t POWERGLITCH_EN[] = {
    {EFUSE_BLK0, 42, 1}, 	 // Power glitch enable,
};

static const esp_efuse_desc_t DIS_USB_SERIAL_JTAG[] = {
    {EFUSE_BLK0, 43, 1}, 	 // Disable USB_SERIAL_JTAG,
};

static const esp_efuse_desc_t DIS_FORCE_DOWNLOAD[] = {
    {EFUSE_BLK0, 44, 1}, 	 // Disable force chip go to download mode function,
};

static const esp_efuse_desc_t DIS_SPI_DOWNLOAD_MSPI[] = {
    {EFUSE_BLK0, 45, 1}, 	 // Represents whether the SPI0 controller is disabled in boot_mode_download,
};

static const esp_efuse_desc_t DIS_TWAI[] = {
    {EFUSE_BLK0, 46, 1}, 	 // Disable TWAI function,
};

static const esp_efuse_desc_t JTAG_SEL_ENABLE[] = {
    {EFUSE_BLK0, 47, 1}, 	 // Set this bit to enable selection between usb_to_jtag and pad_to_jtag through strapping gpio10 when both reg_dis_usb_jtag and reg_dis_pad_jtag are equal to 0.,
};

static const esp_efuse_desc_t SOFT_DIS_JTAG[] = {
    {EFUSE_BLK0, 48, 3}, 	 // Set these bits to soft disable JTAG (odd number 1 means disable). JTAG can be enabled in HMAC module.,
};

static const esp_efuse_desc_t DIS_PAD_JTAG[] = {
    {EFUSE_BLK0, 51, 1}, 	 // Hard disable JTAG. JTAG is disabled permanently.,
};

static const esp_efuse_desc_t DIS_DOWNLOAD_MANUAL_ENCRYPT[] = {
    {EFUSE_BLK0, 52, 1}, 	 // Disable flash encryption when in download boot modes.,
};

static const esp_efuse_desc_t USB_DEVICE_DREFH[] = {
    {EFUSE_BLK0, 53, 2}, 	 // USB-device drefh,
};

static const esp_efuse_desc_t USB_OTG11_DREFH[] = {
    {EFUSE_BLK0, 55, 2}, 	 // USB-otg11 drefh,
};

static const esp_efuse_desc_t USB_PHY_SEL[] = {
    {EFUSE_BLK0, 57, 1}, 	 // Select usb phy,
};

static const esp_efuse_desc_t HUK_GEN_STATE[] = {
    {EFUSE_BLK0, 58, 9}, 	 // HUK generator state,
};

static const esp_efuse_desc_t KM_RND_SWITCH_CYCLE[] = {
    {EFUSE_BLK0, 67, 2}, 	 // km random switch cycle,
};

static const esp_efuse_desc_t KM_DEPLOY_ONLY_ONCE[] = {
    {EFUSE_BLK0, 69, 4}, 	 // km deploy only once,
};

static const esp_efuse_desc_t FORCE_USE_KEY_MANAGER_KEY[] = {
    {EFUSE_BLK0, 73, 4}, 	 // force use key manager key,
};

static const esp_efuse_desc_t FORCE_DISABLE_SW_INIT_KEY[] = {
    {EFUSE_BLK0, 77, 1}, 	 // force disable software init key,
};

static const esp_efuse_desc_t WDT_DELAY_SEL[] = {
    {EFUSE_BLK0, 80, 2}, 	 // watchdog delay selection,
};

static const esp_efuse_desc_t SPI_BOOT_CRYPT_CNT[] = {
    {EFUSE_BLK0, 82, 3}, 	 // spi boot encryption counter,
};

static const esp_efuse_desc_t SECURE_BOOT_KEY_REVOKE0[] = {
    {EFUSE_BLK0, 85, 1}, 	 // Enable revoke first secure boot key,
};

static const esp_efuse_desc_t SECURE_BOOT_KEY_REVOKE1[] = {
    {EFUSE_BLK0, 86, 1}, 	 // Enable revoke second secure boot key,
};

static const esp_efuse_desc_t SECURE_BOOT_KEY_REVOKE2[] = {
    {EFUSE_BLK0, 87, 1}, 	 // Enable revoke third secure boot key,
};

static const esp_efuse_desc_t KEY_PURPOSE_0[] = {
    {EFUSE_BLK0, 88, 4}, 	 // Key0 purpose,
};

static const esp_efuse_desc_t KEY_PURPOSE_1[] = {
    {EFUSE_BLK0, 92, 4}, 	 // Key1 purpose,
};

static const esp_efuse_desc_t KEY_PURPOSE_2[] = {
    {EFUSE_BLK0, 96, 4}, 	 // Key2 purpose,
};

static const esp_efuse_desc_t KEY_PURPOSE_3[] = {
    {EFUSE_BLK0, 100, 4}, 	 // Key3 purpose,
};

static const esp_efuse_desc_t KEY_PURPOSE_4[] = {
    {EFUSE_BLK0, 104, 4}, 	 // Key4 purpose,
};

static const esp_efuse_desc_t KEY_PURPOSE_5[] = {
    {EFUSE_BLK0, 108, 4}, 	 // Key5 purpose,
};

static const esp_efuse_desc_t SEC_DPA_LEVEL[] = {
    {EFUSE_BLK0, 112, 2}, 	 // Configures the clock random divide mode to determine the DPA security level,
};

static const esp_efuse_desc_t ECDSA_FORCE_USE_HARDWARE_K[] = {
    {EFUSE_BLK0, 114, 1}, 	 // ecdsa force use hardware key,
};

static const esp_efuse_desc_t CRYPT_DPA_ENABLE[] = {
    {EFUSE_BLK0, 115, 1}, 	 // Represents whether defense against DPA attack is enabled,
};

static const esp_efuse_desc_t SECURE_BOOT_EN[] = {
    {EFUSE_BLK0, 116, 1}, 	 // Secure boot enable,
};

static const esp_efuse_desc_t SECURE_BOOT_AGGRESSIVE_REVOKE[] = {
    {EFUSE_BLK0, 117, 1}, 	 // Enable aggressive secure boot revoke,
};

static const esp_efuse_desc_t FLASH_TYPE[] = {
    {EFUSE_BLK0, 119, 1}, 	 // flash type,
};

static const esp_efuse_desc_t FLASH_PAGE_SIZE[] = {
    {EFUSE_BLK0, 120, 2}, 	 // flash page size,
};

static const esp_efuse_desc_t FLASH_ECC_EN[] = {
    {EFUSE_BLK0, 122, 1}, 	 // flash ecc enable,
};

static const esp_efuse_desc_t DIS_USB_OTG_DOWNLOAD_MODE[] = {
    {EFUSE_BLK0, 123, 1}, 	 // disable use otg download mode,
};

static const esp_efuse_desc_t FLASH_TPUW[] = {
    {EFUSE_BLK0, 124, 4}, 	 // flash tpuw,
};

static const esp_efuse_desc_t DIS_DOWNLOAD_MODE[] = {
    {EFUSE_BLK0, 128, 1}, 	 // Disble download mode,
};

static const esp_efuse_desc_t DIS_DIRECT_BOOT[] = {
    {EFUSE_BLK0, 129, 1}, 	 // Disable direct boot mode,
};

static const esp_efuse_desc_t DIS_USB_SERIAL_JTAG_ROM_PRINT[] = {
    {EFUSE_BLK0, 130, 1}, 	 // Represents whether print from USB-Serial-JTAG during ROM boot is disabled,
};

static const esp_efuse_desc_t DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[] = {
    {EFUSE_BLK0, 132, 1}, 	 // Represents whether the USB-Serial-JTAG download func- tion is disabled,
};

static const esp_efuse_desc_t ENABLE_SECURITY_DOWNLOAD[] = {
    {EFUSE_BLK0, 133, 1}, 	 // Enable secure download mode,
};

static const esp_efuse_desc_t UART_PRINT_CONTROL[] = {
    {EFUSE_BLK0, 134, 2}, 	 // b00:force print. b01:control by GPIO8 - low level print. b10:control by GPIO8 - high level print. b11:force disable print.,
};

static const esp_efuse_desc_t FORCE_SEND_RESUME[] = {
    {EFUSE_BLK0, 136, 1}, 	 // Force ROM code to send a resume command during SPI boot,
};

static const esp_efuse_desc_t SECURE_VERSION[] = {
    {EFUSE_BLK0, 137, 16}, 	 // Secure version for anti-rollback,
};

static const esp_efuse_desc_t SECURE_BOOT_DISABLE_FAST_WAKE[] = {
    {EFUSE_BLK0, 153, 1}, 	 // Represents whether FAST VERIFY ON WAKE is disabled or enabled when Secure Boot is enabled,
};

static const esp_efuse_desc_t HYS_EN_PAD[] = {
    {EFUSE_BLK0, 154, 1}, 	 // Hys en pad,
};

static const esp_efuse_desc_t DCDC_VSET[] = {
    {EFUSE_BLK0, 155, 5}, 	 // dcdc vset,
};

static const esp_efuse_desc_t _0PXA_TIEH_SEL_0[] = {
    {EFUSE_BLK0, 160, 2}, 	 // 0pxa tieh sel0,
};

static const esp_efuse_desc_t _0PXA_TIEH_SEL_1[] = {
    {EFUSE_BLK0, 162, 2}, 	 // 0pxa tieh sel1,
};

static const esp_efuse_desc_t _0PXA_TIEH_SEL_2[] = {
    {EFUSE_BLK0, 164, 2}, 	 // 0pxa tieh sel2,
};

static const esp_efuse_desc_t _0PXA_TIEH_SEL_3[] = {
    {EFUSE_BLK0, 166, 2}, 	 // 0pxa tieh sel3,
};

static const esp_efuse_desc_t USB_DEVICE_DREFL[] = {
    {EFUSE_BLK0, 172, 2}, 	 // usb device drefl,
};

static const esp_efuse_desc_t USB_OTG11_DREFL[] = {
    {EFUSE_BLK0, 174, 2}, 	 // usb otg1.1 drefl,
};

static const esp_efuse_desc_t HP_PWR_SRC_SEL[] = {
    {EFUSE_BLK0, 178, 1}, 	 // hp power source selection,
};

static const esp_efuse_desc_t DCDC_VSET_EN[] = {
    {EFUSE_BLK0, 179, 1}, 	 // dcdc vset enable,
};

static const esp_efuse_desc_t DIS_WDT[] = {
    {EFUSE_BLK0, 180, 1}, 	 // disable watchdog,
};

static const esp_efuse_desc_t DIS_SWD[] = {
    {EFUSE_BLK0, 181, 1}, 	 // disable super watchdog,
};

static const esp_efuse_desc_t MAC_FACTORY[] = {
    {EFUSE_BLK1, 40, 8}, 	 // Factory MAC addr [0],
    {EFUSE_BLK1, 32, 8}, 	 // Factory MAC addr [1],
    {EFUSE_BLK1, 24, 8}, 	 // Factory MAC addr [2],
    {EFUSE_BLK1, 16, 8}, 	 // Factory MAC addr [3],
    {EFUSE_BLK1, 8, 8}, 	 // Factory MAC addr [4],
    {EFUSE_BLK1, 0, 8}, 	 // Factory MAC addr [5],
};

static const esp_efuse_desc_t MAC_EXT[] = {
    {EFUSE_BLK1, 48, 8}, 	 // Extend  MAC addr [0],
    {EFUSE_BLK1, 56, 8}, 	 // Extend  MAC addr [1],
};

static const esp_efuse_desc_t WAFER_VERSION_MINOR[] = {
    {EFUSE_BLK1, 114, 3}, 	 // WAFER_VERSION_MINOR least significant bits,
    {EFUSE_BLK1, 183, 1}, 	 // WAFER_VERSION_MINOR most significant bit,
};

static const esp_efuse_desc_t PKG_VERSION[] = {
    {EFUSE_BLK1, 117, 3}, 	 // Package version 0:ESP32C3,
};

static const esp_efuse_desc_t BLK_VERSION_MINOR[] = {
    {EFUSE_BLK1, 120, 3}, 	 // BLK_VERSION_MINOR,
};

static const esp_efuse_desc_t OPTIONAL_UNIQUE_ID[] = {
    {EFUSE_BLK2, 0, 128}, 	 // Optional unique 128-bit ID,
};

static const esp_efuse_desc_t BLK_VERSION_MAJOR[] = {
    {EFUSE_BLK2, 128, 2}, 	 // BLK_VERSION_MAJOR of BLOCK2,
};

static const esp_efuse_desc_t TEMP_CALIB[] = {
    {EFUSE_BLK2, 131, 9}, 	 // Temperature calibration data,
};

static const esp_efuse_desc_t OCODE[] = {
    {EFUSE_BLK2, 140, 8}, 	 // ADC OCode,
};

static const esp_efuse_desc_t ADC1_INIT_CODE_ATTEN0[] = {
    {EFUSE_BLK2, 148, 10}, 	 // ADC1 init code at atten0,
};

static const esp_efuse_desc_t ADC1_INIT_CODE_ATTEN1[] = {
    {EFUSE_BLK2, 158, 10}, 	 // ADC1 init code at atten1,
};

static const esp_efuse_desc_t ADC1_INIT_CODE_ATTEN2[] = {
    {EFUSE_BLK2, 168, 10}, 	 // ADC1 init code at atten2,
};

static const esp_efuse_desc_t ADC1_INIT_CODE_ATTEN3[] = {
    {EFUSE_BLK2, 178, 10}, 	 // ADC1 init code at atten3,
};

static const esp_efuse_desc_t ADC1_CAL_VOL_ATTEN0[] = {
    {EFUSE_BLK2, 188, 10}, 	 // ADC1 calibration voltage at atten0,
};

static const esp_efuse_desc_t ADC1_CAL_VOL_ATTEN1[] = {
    {EFUSE_BLK2, 198, 10}, 	 // ADC1 calibration voltage at atten1,
};

static const esp_efuse_desc_t ADC1_CAL_VOL_ATTEN2[] = {
    {EFUSE_BLK2, 208, 10}, 	 // ADC1 calibration voltage at atten2,
};

static const esp_efuse_desc_t ADC1_CAL_VOL_ATTEN3[] = {
    {EFUSE_BLK2, 218, 10}, 	 // ADC1 calibration voltage at atten3,
};

static const esp_efuse_desc_t USER_DATA[] = {
    {EFUSE_BLK3, 0, 256}, 	 // User data,
};

static const esp_efuse_desc_t USER_DATA_MAC_CUSTOM[] = {
    {EFUSE_BLK3, 200, 48}, 	 // Custom MAC,
};

static const esp_efuse_desc_t KEY0[] = {
    {EFUSE_BLK4, 0, 256}, 	 // Key0 or user data,
};

static const esp_efuse_desc_t KEY1[] = {
    {EFUSE_BLK5, 0, 256}, 	 // Key1 or user data,
};

static const esp_efuse_desc_t KEY2[] = {
    {EFUSE_BLK6, 0, 256}, 	 // Key2 or user data,
};

static const esp_efuse_desc_t KEY3[] = {
    {EFUSE_BLK7, 0, 256}, 	 // Key3 or user data,
};

static const esp_efuse_desc_t KEY4[] = {
    {EFUSE_BLK8, 0, 256}, 	 // Key4 or user data,
};

static const esp_efuse_desc_t KEY5[] = {
    {EFUSE_BLK9, 0, 256}, 	 // Key5 or user data,
};

static const esp_efuse_desc_t SYS_DATA_PART2[] = {
    {EFUSE_BLK10, 0, 256}, 	 // System configuration,
};

static const esp_efuse_desc_t K_RTC_LDO[] = {
    {EFUSE_BLK1, 135, 7}, 	 // BLOCK1 K_RTC_LDO,
};

static const esp_efuse_desc_t K_DIG_LDO[] = {
    {EFUSE_BLK1, 142, 7}, 	 // BLOCK1 K_DIG_LDO,
};

static const esp_efuse_desc_t V_RTC_DBIAS20[] = {
    {EFUSE_BLK1, 149, 8}, 	 // BLOCK1 voltage of rtc dbias20,
};

static const esp_efuse_desc_t V_DIG_DBIAS20[] = {
    {EFUSE_BLK1, 157, 8}, 	 // BLOCK1 voltage of digital dbias20,
};

static const esp_efuse_desc_t DIG_DBIAS_HVT[] = {
    {EFUSE_BLK1, 165, 5}, 	 // BLOCK1 digital dbias when hvt,
};

static const esp_efuse_desc_t THRES_HVT[] = {
    {EFUSE_BLK1, 170, 10}, 	 // BLOCK1 pvt threshold when hvt,
};





const esp_efuse_desc_t* ESP_EFUSE_WR_DIS[] = {
    &WR_DIS[0],    		// Write protection
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RD_DIS[] = {
    &WR_DIS_RD_DIS[0],    		// Write protection for RD_DIS_KEY0 RD_DIS_KEY1 RD_DIS_KEY2 RD_DIS_KEY3 RD_DIS_KEY4 RD_DIS_KEY5 RD_DIS_SYS_DATA_PART2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_GROUP_1[] = {
    &WR_DIS_GROUP_1[0],    		// Write protection for KM_RND_SWITCH_CYCLE KM_DEPLOY_ONLY_ONCE FORCE_USE_KEY_MANAGER_KEY FORCE_DISABLE_SW_INIT_KEY
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_GROUP_2[] = {
    &WR_DIS_GROUP_2[0],    		// Write protection for DIS_USB_JTAG POWERGLITCH_EN DIS_USB_DERIAL_JTAG DIS_FORCE_DOWNLOAD DIS_TWAI JTAG_SEL_ENABLE DIS_PAD_JTAG DIS_DOWNLOAD_MANUAL_ENCRYPT WDT_DELAY_SEL HYS_EN_PAD _0PXA_TIEH_SEL_0 _0PXA_TIEH_SEL_1 _0PXA_TIEH_SEL_2 _0PXA_TIEH_SEL_3 DIS_WDT DIS_SWD
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_HP_PWR_SRC_SEL[] = {
    &WR_DIS_HP_PWR_SRC_SEL[0],    		// Write protection for HP_PWR_SRC_SEL
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_BOOT_CRYPT_CNT[] = {
    &WR_DIS_SPI_BOOT_CRYPT_CNT[0],    		// Write protection for SPI_BOOT_CRYPT_CNT
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_KEY_REVOKE0[] = {
    &WR_DIS_SECURE_BOOT_KEY_REVOKE0[0],    		// Write protection for SECURE_BOOT_KEY_REVOKE0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_KEY_REVOKE1[] = {
    &WR_DIS_SECURE_BOOT_KEY_REVOKE1[0],    		// Write protection for SECURE_BOOT_KEY_REVOKE1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_KEY_REVOKE2[] = {
    &WR_DIS_SECURE_BOOT_KEY_REVOKE2[0],    		// Write protection for SECURE_BOOT_KEY_REVOKE2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY0_PURPOSE[] = {
    &WR_DIS_KEY0_PURPOSE[0],    		// Write protection for key_purpose. KEY0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY1_PURPOSE[] = {
    &WR_DIS_KEY1_PURPOSE[0],    		// Write protection for key_purpose. KEY1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY2_PURPOSE[] = {
    &WR_DIS_KEY2_PURPOSE[0],    		// Write protection for key_purpose. KEY2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY3_PURPOSE[] = {
    &WR_DIS_KEY3_PURPOSE[0],    		// Write protection for key_purpose. KEY3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY4_PURPOSE[] = {
    &WR_DIS_KEY4_PURPOSE[0],    		// Write protection for key_purpose. KEY4
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY5_PURPOSE[] = {
    &WR_DIS_KEY5_PURPOSE[0],    		// Write protection for key_purpose. KEY5
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_GROUP_3[] = {
    &WR_DIS_GROUP_3[0],    		// Write protection for SEC_DPA_LEVEL CRYPT_DPA_ENABLE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_EN[] = {
    &WR_DIS_SECURE_BOOT_EN[0],    		// Write protection for SECURE_BOOT_EN
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_AGGRESSIVE_REVOKE[] = {
    &WR_DIS_SECURE_BOOT_AGGRESSIVE_REVOKE[0],    		// Write protection for SECURE_BOOT_AGGRESSIVE_REVOKE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_ECDSA_FORCE_USE_HARDWARE_K[] = {
    &WR_DIS_ECDSA_FORCE_USE_HARDWARE_K[0],    		// Write protection for ECDSA_FORCE_USE_HARDWARE_K
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_GROUP_4[] = {
    &WR_DIS_GROUP_4[0],    		// Write protection for FLASH_TYPE FLASH_PAGE_SIZE FLASH_ECC_EN DIS_USB_OTG_DOWNLOAD_MODE FLASH_TPUW DIS_DOWNLOAD_MODE DIS_DIRECT_BOOT DIS_USB_SERIAL_JTAG_ROM_PRINT DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE ENABLE_SECURITY_DOWNLOAD UART_PRINT_CONTROL FORCE_SEND_RESUME SECURE_VERSION SECURE_BOOT_DISABLE_FAST_WAKE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_HUK_GEN_STATE[] = {
    &WR_DIS_HUK_GEN_STATE[0],    		// Write protection for HUK_GEN_STATE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLK1[] = {
    &WR_DIS_BLK1[0],    		// Write protection for EFUSE_BLK1.  MAC_SPI_8M_SYS
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SYS_DATA_PART1[] = {
    &WR_DIS_SYS_DATA_PART1[0],    		// Write protection for EFUSE_BLK2.  SYS_DATA_PART1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_USER_DATA[] = {
    &WR_DIS_USER_DATA[0],    		// Write protection for EFUSE_BLK3.  USER_DATA
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY0[] = {
    &WR_DIS_KEY0[0],    		// Write protection for EFUSE_BLK4.  KEY0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY1[] = {
    &WR_DIS_KEY1[0],    		// Write protection for EFUSE_BLK5.  KEY1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY2[] = {
    &WR_DIS_KEY2[0],    		// Write protection for EFUSE_BLK6.  KEY2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY3[] = {
    &WR_DIS_KEY3[0],    		// Write protection for EFUSE_BLK7.  KEY3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY4[] = {
    &WR_DIS_KEY4[0],    		// Write protection for EFUSE_BLK8.  KEY4
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY5[] = {
    &WR_DIS_KEY5[0],    		// Write protection for EFUSE_BLK9.  KEY5
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SYS_DATA_PART2[] = {
    &WR_DIS_SYS_DATA_PART2[0],    		// Write protection for EFUSE_BLK10. SYS_DATA_PART2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_GROUP_5[] = {
    &WR_DIS_GROUP_5[0],    		// Write protection for USB_DEVICE_EXCHG_PINS USB_OTG11_EXCHG_PINS USB_DEVICE_DREFH USB_OTG11_DREFH USB_PHY_SEL USB_DEVICE_DREFL USB_OTG11_DREFL
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SOFT_DIS_JTAG[] = {
    &WR_DIS_SOFT_DIS_JTAG[0],    		// Write protection for SOFT_DIS_JTAG
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS[] = {
    &RD_DIS[0],    		// Read protection
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_KEY0[] = {
    &RD_DIS_KEY0[0],    		// Read protection for EFUSE_BLK4.  KEY0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_KEY1[] = {
    &RD_DIS_KEY1[0],    		// Read protection for EFUSE_BLK5.  KEY1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_KEY2[] = {
    &RD_DIS_KEY2[0],    		// Read protection for EFUSE_BLK6.  KEY2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_KEY3[] = {
    &RD_DIS_KEY3[0],    		// Read protection for EFUSE_BLK7.  KEY3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_KEY4[] = {
    &RD_DIS_KEY4[0],    		// Read protection for EFUSE_BLK8.  KEY4
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_KEY5[] = {
    &RD_DIS_KEY5[0],    		// Read protection for EFUSE_BLK9.  KEY5
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_SYS_DATA_PART2[] = {
    &RD_DIS_SYS_DATA_PART2[0],    		// Read protection for EFUSE_BLK10. SYS_DATA_PART2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USB_DEVICE_EXCHG_PINS[] = {
    &USB_DEVICE_EXCHG_PINS[0],    		// USB-device exchange pins
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USB_OTG11_EXCHG_PINS[] = {
    &USB_OTG11_EXCHG_PINS[0],    		// USB-otg11 change pins
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_JTAG[] = {
    &DIS_USB_JTAG[0],    		// Disable USB JTAG
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_POWERGLITCH_EN[] = {
    &POWERGLITCH_EN[0],    		// Power glitch enable
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_SERIAL_JTAG[] = {
    &DIS_USB_SERIAL_JTAG[0],    		// Disable USB_SERIAL_JTAG
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_FORCE_DOWNLOAD[] = {
    &DIS_FORCE_DOWNLOAD[0],    		// Disable force chip go to download mode function
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_SPI_DOWNLOAD_MSPI[] = {
    &DIS_SPI_DOWNLOAD_MSPI[0],    		// Represents whether the SPI0 controller is disabled in boot_mode_download
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_TWAI[] = {
    &DIS_TWAI[0],    		// Disable TWAI function
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_JTAG_SEL_ENABLE[] = {
    &JTAG_SEL_ENABLE[0],    		// Set this bit to enable selection between usb_to_jtag and pad_to_jtag through strapping gpio10 when both reg_dis_usb_jtag and reg_dis_pad_jtag are equal to 0.
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SOFT_DIS_JTAG[] = {
    &SOFT_DIS_JTAG[0],    		// Set these bits to soft disable JTAG (odd number 1 means disable). JTAG can be enabled in HMAC module.
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_PAD_JTAG[] = {
    &DIS_PAD_JTAG[0],    		// Hard disable JTAG. JTAG is disabled permanently.
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT[] = {
    &DIS_DOWNLOAD_MANUAL_ENCRYPT[0],    		// Disable flash encryption when in download boot modes.
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USB_DEVICE_DREFH[] = {
    &USB_DEVICE_DREFH[0],    		// USB-device drefh
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USB_OTG11_DREFH[] = {
    &USB_OTG11_DREFH[0],    		// USB-otg11 drefh
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USB_PHY_SEL[] = {
    &USB_PHY_SEL[0],    		// Select usb phy
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_HUK_GEN_STATE[] = {
    &HUK_GEN_STATE[0],    		// HUK generator state
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KM_RND_SWITCH_CYCLE[] = {
    &KM_RND_SWITCH_CYCLE[0],    		// km random switch cycle
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KM_DEPLOY_ONLY_ONCE[] = {
    &KM_DEPLOY_ONLY_ONCE[0],    		// km deploy only once
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FORCE_USE_KEY_MANAGER_KEY[] = {
    &FORCE_USE_KEY_MANAGER_KEY[0],    		// force use key manager key
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FORCE_DISABLE_SW_INIT_KEY[] = {
    &FORCE_DISABLE_SW_INIT_KEY[0],    		// force disable software init key
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WDT_DELAY_SEL[] = {
    &WDT_DELAY_SEL[0],    		// watchdog delay selection
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SPI_BOOT_CRYPT_CNT[] = {
    &SPI_BOOT_CRYPT_CNT[0],    		// spi boot encryption counter
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_KEY_REVOKE0[] = {
    &SECURE_BOOT_KEY_REVOKE0[0],    		// Enable revoke first secure boot key
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_KEY_REVOKE1[] = {
    &SECURE_BOOT_KEY_REVOKE1[0],    		// Enable revoke second secure boot key
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_KEY_REVOKE2[] = {
    &SECURE_BOOT_KEY_REVOKE2[0],    		// Enable revoke third secure boot key
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_0[] = {
    &KEY_PURPOSE_0[0],    		// Key0 purpose
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_1[] = {
    &KEY_PURPOSE_1[0],    		// Key1 purpose
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_2[] = {
    &KEY_PURPOSE_2[0],    		// Key2 purpose
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_3[] = {
    &KEY_PURPOSE_3[0],    		// Key3 purpose
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_4[] = {
    &KEY_PURPOSE_4[0],    		// Key4 purpose
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_5[] = {
    &KEY_PURPOSE_5[0],    		// Key5 purpose
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SEC_DPA_LEVEL[] = {
    &SEC_DPA_LEVEL[0],    		// Configures the clock random divide mode to determine the DPA security level
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ECDSA_FORCE_USE_HARDWARE_K[] = {
    &ECDSA_FORCE_USE_HARDWARE_K[0],    		// ecdsa force use hardware key
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_CRYPT_DPA_ENABLE[] = {
    &CRYPT_DPA_ENABLE[0],    		// Represents whether defense against DPA attack is enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_EN[] = {
    &SECURE_BOOT_EN[0],    		// Secure boot enable
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE[] = {
    &SECURE_BOOT_AGGRESSIVE_REVOKE[0],    		// Enable aggressive secure boot revoke
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_TYPE[] = {
    &FLASH_TYPE[0],    		// flash type
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_PAGE_SIZE[] = {
    &FLASH_PAGE_SIZE[0],    		// flash page size
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_ECC_EN[] = {
    &FLASH_ECC_EN[0],    		// flash ecc enable
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_OTG_DOWNLOAD_MODE[] = {
    &DIS_USB_OTG_DOWNLOAD_MODE[0],    		// disable use otg download mode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_TPUW[] = {
    &FLASH_TPUW[0],    		// flash tpuw
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_MODE[] = {
    &DIS_DOWNLOAD_MODE[0],    		// Disble download mode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_DIRECT_BOOT[] = {
    &DIS_DIRECT_BOOT[0],    		// Disable direct boot mode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_SERIAL_JTAG_ROM_PRINT[] = {
    &DIS_USB_SERIAL_JTAG_ROM_PRINT[0],    		// Represents whether print from USB-Serial-JTAG during ROM boot is disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[] = {
    &DIS_USB_SERIAL_JTAG_DOWNLOAD_MODE[0],    		// Represents whether the USB-Serial-JTAG download func- tion is disabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ENABLE_SECURITY_DOWNLOAD[] = {
    &ENABLE_SECURITY_DOWNLOAD[0],    		// Enable secure download mode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_UART_PRINT_CONTROL[] = {
    &UART_PRINT_CONTROL[0],    		// b00:force print. b01:control by GPIO8 - low level print. b10:control by GPIO8 - high level print. b11:force disable print.
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FORCE_SEND_RESUME[] = {
    &FORCE_SEND_RESUME[0],    		// Force ROM code to send a resume command during SPI boot
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_VERSION[] = {
    &SECURE_VERSION[0],    		// Secure version for anti-rollback
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_DISABLE_FAST_WAKE[] = {
    &SECURE_BOOT_DISABLE_FAST_WAKE[0],    		// Represents whether FAST VERIFY ON WAKE is disabled or enabled when Secure Boot is enabled
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_HYS_EN_PAD[] = {
    &HYS_EN_PAD[0],    		// Hys en pad
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DCDC_VSET[] = {
    &DCDC_VSET[0],    		// dcdc vset
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE__0PXA_TIEH_SEL_0[] = {
    &_0PXA_TIEH_SEL_0[0],    		// 0pxa tieh sel0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE__0PXA_TIEH_SEL_1[] = {
    &_0PXA_TIEH_SEL_1[0],    		// 0pxa tieh sel1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE__0PXA_TIEH_SEL_2[] = {
    &_0PXA_TIEH_SEL_2[0],    		// 0pxa tieh sel2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE__0PXA_TIEH_SEL_3[] = {
    &_0PXA_TIEH_SEL_3[0],    		// 0pxa tieh sel3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USB_DEVICE_DREFL[] = {
    &USB_DEVICE_DREFL[0],    		// usb device drefl
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USB_OTG11_DREFL[] = {
    &USB_OTG11_DREFL[0],    		// usb otg1.1 drefl
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_HP_PWR_SRC_SEL[] = {
    &HP_PWR_SRC_SEL[0],    		// hp power source selection
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DCDC_VSET_EN[] = {
    &DCDC_VSET_EN[0],    		// dcdc vset enable
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_WDT[] = {
    &DIS_WDT[0],    		// disable watchdog
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIS_SWD[] = {
    &DIS_SWD[0],    		// disable super watchdog
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_MAC_FACTORY[] = {
    &MAC_FACTORY[0],    		// Factory MAC addr [0]
    &MAC_FACTORY[1],    		// Factory MAC addr [1]
    &MAC_FACTORY[2],    		// Factory MAC addr [2]
    &MAC_FACTORY[3],    		// Factory MAC addr [3]
    &MAC_FACTORY[4],    		// Factory MAC addr [4]
    &MAC_FACTORY[5],    		// Factory MAC addr [5]
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_MAC_EXT[] = {
    &MAC_EXT[0],    		// Extend  MAC addr [0]
    &MAC_EXT[1],    		// Extend  MAC addr [1]
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WAFER_VERSION_MINOR[] = {
    &WAFER_VERSION_MINOR[0],    		// WAFER_VERSION_MINOR least significant bits
    &WAFER_VERSION_MINOR[1],    		// WAFER_VERSION_MINOR most significant bit
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_PKG_VERSION[] = {
    &PKG_VERSION[0],    		// Package version 0:ESP32C3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_BLK_VERSION_MINOR[] = {
    &BLK_VERSION_MINOR[0],    		// BLK_VERSION_MINOR
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_OPTIONAL_UNIQUE_ID[] = {
    &OPTIONAL_UNIQUE_ID[0],    		// Optional unique 128-bit ID
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_BLK_VERSION_MAJOR[] = {
    &BLK_VERSION_MAJOR[0],    		// BLK_VERSION_MAJOR of BLOCK2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_TEMP_CALIB[] = {
    &TEMP_CALIB[0],    		// Temperature calibration data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_OCODE[] = {
    &OCODE[0],    		// ADC OCode
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_INIT_CODE_ATTEN0[] = {
    &ADC1_INIT_CODE_ATTEN0[0],    		// ADC1 init code at atten0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_INIT_CODE_ATTEN1[] = {
    &ADC1_INIT_CODE_ATTEN1[0],    		// ADC1 init code at atten1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_INIT_CODE_ATTEN2[] = {
    &ADC1_INIT_CODE_ATTEN2[0],    		// ADC1 init code at atten2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_INIT_CODE_ATTEN3[] = {
    &ADC1_INIT_CODE_ATTEN3[0],    		// ADC1 init code at atten3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_CAL_VOL_ATTEN0[] = {
    &ADC1_CAL_VOL_ATTEN0[0],    		// ADC1 calibration voltage at atten0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_CAL_VOL_ATTEN1[] = {
    &ADC1_CAL_VOL_ATTEN1[0],    		// ADC1 calibration voltage at atten1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_CAL_VOL_ATTEN2[] = {
    &ADC1_CAL_VOL_ATTEN2[0],    		// ADC1 calibration voltage at atten2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_CAL_VOL_ATTEN3[] = {
    &ADC1_CAL_VOL_ATTEN3[0],    		// ADC1 calibration voltage at atten3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USER_DATA[] = {
    &USER_DATA[0],    		// User data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_USER_DATA_MAC_CUSTOM[] = {
    &USER_DATA_MAC_CUSTOM[0],    		// Custom MAC
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY0[] = {
    &KEY0[0],    		// Key0 or user data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY1[] = {
    &KEY1[0],    		// Key1 or user data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY2[] = {
    &KEY2[0],    		// Key2 or user data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY3[] = {
    &KEY3[0],    		// Key3 or user data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY4[] = {
    &KEY4[0],    		// Key4 or user data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_KEY5[] = {
    &KEY5[0],    		// Key5 or user data
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SYS_DATA_PART2[] = {
    &SYS_DATA_PART2[0],    		// System configuration
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_K_RTC_LDO[] = {
    &K_RTC_LDO[0],    		// BLOCK1 K_RTC_LDO
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_K_DIG_LDO[] = {
    &K_DIG_LDO[0],    		// BLOCK1 K_DIG_LDO
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_V_RTC_DBIAS20[] = {
    &V_RTC_DBIAS20[0],    		// BLOCK1 voltage of rtc dbias20
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_V_DIG_DBIAS20[] = {
    &V_DIG_DBIAS20[0],    		// BLOCK1 voltage of digital dbias20
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DIG_DBIAS_HVT[] = {
    &DIG_DBIAS_HVT[0],    		// BLOCK1 digital dbias when hvt
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_THRES_HVT[] = {
    &THRES_HVT[0],    		// BLOCK1 pvt threshold when hvt
    NULL
};
