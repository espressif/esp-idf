/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef __cplusplus
extern "C" {
#endif


// md5_digest_table 9444b887379d924049af42806ca71d45
// This file was generated from the file esp_efuse_table.csv. DO NOT CHANGE THIS FILE MANUALLY.
// If you want to change some fields, you need to change esp_efuse_table.csv file
// then run `efuse_common_table` or `efuse_custom_table` command it will generate this file.
// To show efuse_table run the command 'show_efuse_table'.


extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_RD_DIS[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_GROUP_1[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_GROUP_2[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SPI_BOOT_CRYPT_CNT[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_KEY_REVOKE0[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_KEY_REVOKE1[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_KEY_REVOKE2[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY0_PURPOSE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY1_PURPOSE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY2_PURPOSE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY3_PURPOSE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY4_PURPOSE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY5_PURPOSE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_EN[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SECURE_BOOT_AGGRESSIVE_REVOKE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_GROUP_3[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLK1[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SYS_DATA_PART1[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_USER_DATA[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY0[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY1[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY2[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY3[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY4[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_KEY5[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_SYS_DATA_PART2[];
extern const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_USB_EXCHG_PINS[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_KEY0[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_KEY1[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_KEY2[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_KEY3[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_KEY4[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_KEY5[];
extern const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_SYS_DATA_PART2[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_ICACHE[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_DCACHE[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_ICACHE[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_DCACHE[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_FORCE_DOWNLOAD[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_USB[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_CAN[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_APP_CPU[];
extern const esp_efuse_desc_t* ESP_EFUSE_SOFT_DIS_JTAG[];
extern const esp_efuse_desc_t* ESP_EFUSE_HARD_DIS_JTAG[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT[];
extern const esp_efuse_desc_t* ESP_EFUSE_USB_EXCHG_PINS[];
extern const esp_efuse_desc_t* ESP_EFUSE_USB_EXT_PHY_ENABLE[];
extern const esp_efuse_desc_t* ESP_EFUSE_BTLC_GPIO_ENABLE[];
extern const esp_efuse_desc_t* ESP_EFUSE_VDD_SPI_XPD[];
extern const esp_efuse_desc_t* ESP_EFUSE_VDD_SPI_TIEH[];
extern const esp_efuse_desc_t* ESP_EFUSE_VDD_SPI_FORCE[];
extern const esp_efuse_desc_t* ESP_EFUSE_WDT_DELAY_SEL[];
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_BOOT_CRYPT_CNT[];
extern const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_KEY_REVOKE0[];
extern const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_KEY_REVOKE1[];
extern const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_KEY_REVOKE2[];
extern const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_0[];
extern const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_1[];
extern const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_2[];
extern const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_3[];
extern const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_4[];
extern const esp_efuse_desc_t* ESP_EFUSE_KEY_PURPOSE_5[];
extern const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_EN[];
extern const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_AGGRESSIVE_REVOKE[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_JTAG[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_SERIAL_JTAG[];
extern const esp_efuse_desc_t* ESP_EFUSE_STRAP_JTAG_SEL[];
extern const esp_efuse_desc_t* ESP_EFUSE_USB_PHY_SEL[];
extern const esp_efuse_desc_t* ESP_EFUSE_FLASH_TPUW[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_DOWNLOAD_MODE[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_LEGACY_SPI_BOOT[];
extern const esp_efuse_desc_t* ESP_EFUSE_UART_PRINT_CHANNEL[];
extern const esp_efuse_desc_t* ESP_EFUSE_FLASH_ECC_MODE[];
extern const esp_efuse_desc_t* ESP_EFUSE_DIS_USB_DOWNLOAD_MODE[];
extern const esp_efuse_desc_t* ESP_EFUSE_ENABLE_SECURITY_DOWNLOAD[];
extern const esp_efuse_desc_t* ESP_EFUSE_UART_PRINT_CONTROL[];
extern const esp_efuse_desc_t* ESP_EFUSE_PIN_POWER_SELECTION[];
extern const esp_efuse_desc_t* ESP_EFUSE_FLASH_TYPE[];
extern const esp_efuse_desc_t* ESP_EFUSE_FLASH_PAGE_SIZE[];
extern const esp_efuse_desc_t* ESP_EFUSE_FLASH_ECC_EN[];
extern const esp_efuse_desc_t* ESP_EFUSE_FORCE_SEND_RESUME[];
extern const esp_efuse_desc_t* ESP_EFUSE_SECURE_VERSION[];
extern const esp_efuse_desc_t* ESP_EFUSE_MAC_FACTORY[];
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_CLK[];
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_Q_D1[];
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_D_D0[];
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_CS[];
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_HD_D3[];
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_WP_D2[];
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_DQS[];
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_D4[];
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_D5[];
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_D6[];
extern const esp_efuse_desc_t* ESP_EFUSE_SPI_PAD_CONFIG_D7[];
extern const esp_efuse_desc_t* ESP_EFUSE_WAFER_VERSION[];
extern const esp_efuse_desc_t* ESP_EFUSE_PKG_VERSION[];
extern const esp_efuse_desc_t* ESP_EFUSE_BLK_VER_MINOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC2_CAL_VOL_ATTEN3[];
extern const esp_efuse_desc_t* ESP_EFUSE_OPTIONAL_UNIQUE_ID[];
extern const esp_efuse_desc_t* ESP_EFUSE_BLK_VER_MAJOR[];
extern const esp_efuse_desc_t* ESP_EFUSE_TEMP_CALIB[];
extern const esp_efuse_desc_t* ESP_EFUSE_OCODE[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC1_INIT_CODE_ATTEN0[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC1_INIT_CODE_ATTEN1[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC1_INIT_CODE_ATTEN2[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC1_INIT_CODE_ATTEN3[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC2_INIT_CODE_ATTEN0[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC2_INIT_CODE_ATTEN1[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC2_INIT_CODE_ATTEN2[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC2_INIT_CODE_ATTEN3[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC1_CAL_VOL_ATTEN0[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC1_CAL_VOL_ATTEN1[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC1_CAL_VOL_ATTEN2[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC1_CAL_VOL_ATTEN3[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC2_CAL_VOL_ATTEN0[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC2_CAL_VOL_ATTEN1[];
extern const esp_efuse_desc_t* ESP_EFUSE_ADC2_CAL_VOL_ATTEN2[];
extern const esp_efuse_desc_t* ESP_EFUSE_USER_DATA[];
extern const esp_efuse_desc_t* ESP_EFUSE_USER_DATA_MAC_CUSTOM[];
extern const esp_efuse_desc_t* ESP_EFUSE_KEY0[];
extern const esp_efuse_desc_t* ESP_EFUSE_KEY1[];
extern const esp_efuse_desc_t* ESP_EFUSE_KEY2[];
extern const esp_efuse_desc_t* ESP_EFUSE_KEY3[];
extern const esp_efuse_desc_t* ESP_EFUSE_KEY4[];
extern const esp_efuse_desc_t* ESP_EFUSE_KEY5[];
extern const esp_efuse_desc_t* ESP_EFUSE_SYS_DATA_PART2[];

#ifdef __cplusplus
}
#endif
