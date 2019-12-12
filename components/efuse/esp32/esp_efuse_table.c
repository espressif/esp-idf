// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at",
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License

#include "sdkconfig.h"
#include "esp_efuse.h"
#include <assert.h>
#include "esp_efuse_table.h"

// md5_digest_table 2e23344575b3d07f01ecb695294e9770
// This file was generated from the file esp_efuse_table.csv. DO NOT CHANGE THIS FILE MANUALLY.
// If you want to change some fields, you need to change esp_efuse_table.csv file
// then run `efuse_common_table` or `efuse_custom_table` command it will generate this file.
// To show efuse_table run the command 'show_efuse_table'.

#define MAX_BLK_LEN CONFIG_EFUSE_MAX_BLK_LEN

// The last free bit in the block is counted over the entire file.
#define LAST_FREE_BIT_BLK1 MAX_BLK_LEN
#define LAST_FREE_BIT_BLK2 MAX_BLK_LEN
#define LAST_FREE_BIT_BLK3 192

_Static_assert(LAST_FREE_BIT_BLK1 <= MAX_BLK_LEN, "The eFuse table does not match the coding scheme. Edit the table and restart the efuse_common_table or efuse_custom_table command to regenerate the new files.");
_Static_assert(LAST_FREE_BIT_BLK2 <= MAX_BLK_LEN, "The eFuse table does not match the coding scheme. Edit the table and restart the efuse_common_table or efuse_custom_table command to regenerate the new files.");
_Static_assert(LAST_FREE_BIT_BLK3 <= MAX_BLK_LEN, "The eFuse table does not match the coding scheme. Edit the table and restart the efuse_common_table or efuse_custom_table command to regenerate the new files.");

static const esp_efuse_desc_t MAC_FACTORY[] = {
    {EFUSE_BLK0, 72, 8}, 	 // Factory MAC addr [0],
    {EFUSE_BLK0, 64, 8}, 	 // Factory MAC addr [1],
    {EFUSE_BLK0, 56, 8}, 	 // Factory MAC addr [2],
    {EFUSE_BLK0, 48, 8}, 	 // Factory MAC addr [3],
    {EFUSE_BLK0, 40, 8}, 	 // Factory MAC addr [4],
    {EFUSE_BLK0, 32, 8}, 	 // Factory MAC addr [5],
};

static const esp_efuse_desc_t MAC_FACTORY_CRC[] = {
    {EFUSE_BLK0, 80, 8}, 	 // CRC8 for factory MAC address,
};

static const esp_efuse_desc_t MAC_CUSTOM_CRC[] = {
    {EFUSE_BLK3, 0, 8}, 	 // CRC8 for custom MAC address.,
};

static const esp_efuse_desc_t MAC_CUSTOM[] = {
    {EFUSE_BLK3, 8, 48}, 	 // Custom MAC,
};

static const esp_efuse_desc_t MAC_CUSTOM_VER[] = {
    {EFUSE_BLK3, 184, 8}, 	 // Custom MAC version,
};

static const esp_efuse_desc_t SECURE_BOOT_KEY[] = {
    {EFUSE_BLK2, 0, MAX_BLK_LEN}, 	 // Security boot. Key. (length = "None" - 256. "3/4" - 192. "REPEAT" - 128),
};

static const esp_efuse_desc_t ABS_DONE_0[] = {
    {EFUSE_BLK0, 196, 1}, 	 // Secure boot is enabled for bootloader image. EFUSE_RD_ABS_DONE_0,
};

static const esp_efuse_desc_t ENCRYPT_FLASH_KEY[] = {
    {EFUSE_BLK1, 0, MAX_BLK_LEN}, 	 // Flash encrypt. Key. (length = "None" - 256. "3/4" - 192. "REPEAT" - 128),
};

static const esp_efuse_desc_t ENCRYPT_CONFIG[] = {
    {EFUSE_BLK0, 188, 4}, 	 // Flash encrypt. EFUSE_FLASH_CRYPT_CONFIG_M,
};

static const esp_efuse_desc_t DISABLE_DL_ENCRYPT[] = {
    {EFUSE_BLK0, 199, 1}, 	 // Flash encrypt. Disable UART bootloader encryption. EFUSE_DISABLE_DL_ENCRYPT.,
};

static const esp_efuse_desc_t DISABLE_DL_DECRYPT[] = {
    {EFUSE_BLK0, 200, 1}, 	 // Flash encrypt. Disable UART bootloader decryption. EFUSE_DISABLE_DL_DECRYPT.,
};

static const esp_efuse_desc_t DISABLE_DL_CACHE[] = {
    {EFUSE_BLK0, 201, 1}, 	 // Flash encrypt. Disable UART bootloader MMU cache. EFUSE_DISABLE_DL_CACHE.,
};

static const esp_efuse_desc_t DISABLE_JTAG[] = {
    {EFUSE_BLK0, 198, 1}, 	 // Flash encrypt. Disable JTAG. EFUSE_RD_DISABLE_JTAG.,
};

static const esp_efuse_desc_t CONSOLE_DEBUG_DISABLE[] = {
    {EFUSE_BLK0, 194, 1}, 	 // Flash encrypt. Disable ROM BASIC interpreter fallback. EFUSE_RD_CONSOLE_DEBUG_DISABLE.,
};

static const esp_efuse_desc_t FLASH_CRYPT_CNT[] = {
    {EFUSE_BLK0, 20, 7}, 	 // Flash encrypt. Flash encryption is enabled if this field has an odd number of bits set. EFUSE_FLASH_CRYPT_CNT.,
};

static const esp_efuse_desc_t WR_DIS_FLASH_CRYPT_CNT[] = {
    {EFUSE_BLK0, 2, 1}, 	 // Flash encrypt. Write protection FLASH_CRYPT_CNT. EFUSE_WR_DIS_FLASH_CRYPT_CNT,
};

static const esp_efuse_desc_t WR_DIS_BLK1[] = {
    {EFUSE_BLK0, 7, 1}, 	 // Flash encrypt. Write protection encryption key. EFUSE_WR_DIS_BLK1,
};

static const esp_efuse_desc_t WR_DIS_BLK2[] = {
    {EFUSE_BLK0, 8, 1}, 	 // Security boot. Write protection security key. EFUSE_WR_DIS_BLK2,
};

static const esp_efuse_desc_t WR_DIS_BLK3[] = {
    {EFUSE_BLK0, 9, 1}, 	 // Write protection for EFUSE_BLK3. EFUSE_WR_DIS_BLK3,
};

static const esp_efuse_desc_t RD_DIS_BLK1[] = {
    {EFUSE_BLK0, 16, 1}, 	 // Flash encrypt. efuse_key_read_protected. EFUSE_RD_DIS_BLK1,
};

static const esp_efuse_desc_t RD_DIS_BLK2[] = {
    {EFUSE_BLK0, 17, 1}, 	 // Security boot. efuse_key_read_protected. EFUSE_RD_DIS_BLK2,
};

static const esp_efuse_desc_t RD_DIS_BLK3[] = {
    {EFUSE_BLK0, 18, 1}, 	 // Read protection for EFUSE_BLK3. EFUSE_RD_DIS_BLK3,
};

static const esp_efuse_desc_t CHIP_VER_DIS_APP_CPU[] = {
    {EFUSE_BLK0, 96, 1}, 	 // EFUSE_RD_CHIP_VER_DIS_APP_CPU,
};

static const esp_efuse_desc_t CHIP_VER_DIS_BT[] = {
    {EFUSE_BLK0, 97, 1}, 	 // EFUSE_RD_CHIP_VER_DIS_BT,
};

static const esp_efuse_desc_t CHIP_VER_PKG[] = {
    {EFUSE_BLK0, 105, 3}, 	 // EFUSE_RD_CHIP_VER_PKG,
};

static const esp_efuse_desc_t CHIP_CPU_FREQ_LOW[] = {
    {EFUSE_BLK0, 108, 1}, 	 // EFUSE_RD_CHIP_CPU_FREQ_LOW,
};

static const esp_efuse_desc_t CHIP_CPU_FREQ_RATED[] = {
    {EFUSE_BLK0, 109, 1}, 	 // EFUSE_RD_CHIP_CPU_FREQ_RATED,
};

static const esp_efuse_desc_t CHIP_VER_REV1[] = {
    {EFUSE_BLK0, 111, 1}, 	 // EFUSE_RD_CHIP_VER_REV1,
};

static const esp_efuse_desc_t CHIP_VER_REV2[] = {
    {EFUSE_BLK0, 180, 1}, 	 // EFUSE_RD_CHIP_VER_REV2,
};

static const esp_efuse_desc_t XPD_SDIO_REG[] = {
    {EFUSE_BLK0, 142, 1}, 	 // EFUSE_RD_XPD_SDIO_REG,
};

static const esp_efuse_desc_t SDIO_TIEH[] = {
    {EFUSE_BLK0, 143, 1}, 	 // EFUSE_RD_SDIO_TIEH,
};

static const esp_efuse_desc_t SDIO_FORCE[] = {
    {EFUSE_BLK0, 144, 1}, 	 // EFUSE_RD_SDIO_FORCE,
};

static const esp_efuse_desc_t ADC_VREF_AND_SDIO_DREF[] = {
    {EFUSE_BLK0, 136, 6}, 	 // EFUSE_RD_ADC_VREF[0..4] or ( SDIO_DREFH[0 1],
};

static const esp_efuse_desc_t ADC1_TP_LOW[] = {
    {EFUSE_BLK3, 96, 7}, 	 // TP_REG EFUSE_RD_ADC1_TP_LOW,
};

static const esp_efuse_desc_t ADC2_TP_LOW[] = {
    {EFUSE_BLK3, 112, 7}, 	 // TP_REG EFUSE_RD_ADC2_TP_LOW,
};

static const esp_efuse_desc_t ADC1_TP_HIGH[] = {
    {EFUSE_BLK3, 103, 9}, 	 // TP_REG EFUSE_RD_ADC1_TP_HIGH,
};

static const esp_efuse_desc_t ADC2_TP_HIGH[] = {
    {EFUSE_BLK3, 119, 9}, 	 // TP_REG EFUSE_RD_ADC2_TP_HIGH,
};

static const esp_efuse_desc_t SECURE_VERSION[] = {
    {EFUSE_BLK3, 128, 32}, 	 // Secure version for anti-rollback,
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

const esp_efuse_desc_t* ESP_EFUSE_MAC_FACTORY_CRC[] = {
    &MAC_FACTORY_CRC[0],    		// CRC8 for factory MAC address
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_MAC_CUSTOM_CRC[] = {
    &MAC_CUSTOM_CRC[0],    		// CRC8 for custom MAC address.
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_MAC_CUSTOM[] = {
    &MAC_CUSTOM[0],    		// Custom MAC
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_MAC_CUSTOM_VER[] = {
    &MAC_CUSTOM_VER[0],    		// Custom MAC version
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_BOOT_KEY[] = {
    &SECURE_BOOT_KEY[0],    		// Security boot. Key. (length = "None" - 256. "3/4" - 192. "REPEAT" - 128)
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ABS_DONE_0[] = {
    &ABS_DONE_0[0],    		// Secure boot is enabled for bootloader image. EFUSE_RD_ABS_DONE_0
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ENCRYPT_FLASH_KEY[] = {
    &ENCRYPT_FLASH_KEY[0],    		// Flash encrypt. Key. (length = "None" - 256. "3/4" - 192. "REPEAT" - 128)
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ENCRYPT_CONFIG[] = {
    &ENCRYPT_CONFIG[0],    		// Flash encrypt. EFUSE_FLASH_CRYPT_CONFIG_M
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DISABLE_DL_ENCRYPT[] = {
    &DISABLE_DL_ENCRYPT[0],    		// Flash encrypt. Disable UART bootloader encryption. EFUSE_DISABLE_DL_ENCRYPT.
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DISABLE_DL_DECRYPT[] = {
    &DISABLE_DL_DECRYPT[0],    		// Flash encrypt. Disable UART bootloader decryption. EFUSE_DISABLE_DL_DECRYPT.
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DISABLE_DL_CACHE[] = {
    &DISABLE_DL_CACHE[0],    		// Flash encrypt. Disable UART bootloader MMU cache. EFUSE_DISABLE_DL_CACHE.
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_DISABLE_JTAG[] = {
    &DISABLE_JTAG[0],    		// Flash encrypt. Disable JTAG. EFUSE_RD_DISABLE_JTAG.
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_CONSOLE_DEBUG_DISABLE[] = {
    &CONSOLE_DEBUG_DISABLE[0],    		// Flash encrypt. Disable ROM BASIC interpreter fallback. EFUSE_RD_CONSOLE_DEBUG_DISABLE.
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_FLASH_CRYPT_CNT[] = {
    &FLASH_CRYPT_CNT[0],    		// Flash encrypt. Flash encryption is enabled if this field has an odd number of bits set. EFUSE_FLASH_CRYPT_CNT.
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_FLASH_CRYPT_CNT[] = {
    &WR_DIS_FLASH_CRYPT_CNT[0],    		// Flash encrypt. Write protection FLASH_CRYPT_CNT. EFUSE_WR_DIS_FLASH_CRYPT_CNT
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLK1[] = {
    &WR_DIS_BLK1[0],    		// Flash encrypt. Write protection encryption key. EFUSE_WR_DIS_BLK1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLK2[] = {
    &WR_DIS_BLK2[0],    		// Security boot. Write protection security key. EFUSE_WR_DIS_BLK2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_WR_DIS_BLK3[] = {
    &WR_DIS_BLK3[0],    		// Write protection for EFUSE_BLK3. EFUSE_WR_DIS_BLK3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLK1[] = {
    &RD_DIS_BLK1[0],    		// Flash encrypt. efuse_key_read_protected. EFUSE_RD_DIS_BLK1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLK2[] = {
    &RD_DIS_BLK2[0],    		// Security boot. efuse_key_read_protected. EFUSE_RD_DIS_BLK2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_RD_DIS_BLK3[] = {
    &RD_DIS_BLK3[0],    		// Read protection for EFUSE_BLK3. EFUSE_RD_DIS_BLK3
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_CHIP_VER_DIS_APP_CPU[] = {
    &CHIP_VER_DIS_APP_CPU[0],    		// EFUSE_RD_CHIP_VER_DIS_APP_CPU
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_CHIP_VER_DIS_BT[] = {
    &CHIP_VER_DIS_BT[0],    		// EFUSE_RD_CHIP_VER_DIS_BT
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_CHIP_VER_PKG[] = {
    &CHIP_VER_PKG[0],    		// EFUSE_RD_CHIP_VER_PKG
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_CHIP_CPU_FREQ_LOW[] = {
    &CHIP_CPU_FREQ_LOW[0],    		// EFUSE_RD_CHIP_CPU_FREQ_LOW
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_CHIP_CPU_FREQ_RATED[] = {
    &CHIP_CPU_FREQ_RATED[0],    		// EFUSE_RD_CHIP_CPU_FREQ_RATED
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_CHIP_VER_REV1[] = {
    &CHIP_VER_REV1[0],    		// EFUSE_RD_CHIP_VER_REV1
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_CHIP_VER_REV2[] = {
    &CHIP_VER_REV2[0],    		// EFUSE_RD_CHIP_VER_REV2
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_XPD_SDIO_REG[] = {
    &XPD_SDIO_REG[0],    		// EFUSE_RD_XPD_SDIO_REG
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SDIO_TIEH[] = {
    &SDIO_TIEH[0],    		// EFUSE_RD_SDIO_TIEH
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SDIO_FORCE[] = {
    &SDIO_FORCE[0],    		// EFUSE_RD_SDIO_FORCE
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC_VREF_AND_SDIO_DREF[] = {
    &ADC_VREF_AND_SDIO_DREF[0],    		// EFUSE_RD_ADC_VREF[0..4] or ( SDIO_DREFH[0 1]
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_TP_LOW[] = {
    &ADC1_TP_LOW[0],    		// TP_REG EFUSE_RD_ADC1_TP_LOW
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC2_TP_LOW[] = {
    &ADC2_TP_LOW[0],    		// TP_REG EFUSE_RD_ADC2_TP_LOW
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC1_TP_HIGH[] = {
    &ADC1_TP_HIGH[0],    		// TP_REG EFUSE_RD_ADC1_TP_HIGH
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_ADC2_TP_HIGH[] = {
    &ADC2_TP_HIGH[0],    		// TP_REG EFUSE_RD_ADC2_TP_HIGH
    NULL
};

const esp_efuse_desc_t* ESP_EFUSE_SECURE_VERSION[] = {
    &SECURE_VERSION[0],    		// Secure version for anti-rollback
    NULL
};

