// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <strings.h>

#include "bootloader_flash.h"
#include "esp_image_format.h"
#include "esp_flash_encrypt.h"
#include "esp_flash_partitions.h"
#include "esp_secure_boot.h"
#include "esp_efuse.h"
#include "esp_log.h"
#include "esp32/rom/secure_boot.h"
#include "hal/wdt_hal.h"

#include "esp32/rom/cache.h"
#include "esp32/rom/spi_flash.h"   /* TODO: Remove this */

/* This file implements FLASH ENCRYPTION related APIs to perform
 * various operations such as programming necessary flash encryption
 * eFuses, detect whether flash encryption is enabled (by reading eFuse)
 * and if required encrypt the partitions in flash memory
 */

static const char *TAG = "flash_encrypt";

/* Static functions for stages of flash encryption */
static esp_err_t initialise_flash_encryption(void);
static esp_err_t encrypt_flash_contents(uint32_t flash_crypt_cnt, bool flash_crypt_wr_dis) __attribute__((unused));
static esp_err_t encrypt_bootloader(void);
static esp_err_t encrypt_and_load_partition_table(esp_partition_info_t *partition_table, int *num_partitions);
static esp_err_t encrypt_partition(int index, const esp_partition_info_t *partition);

esp_err_t esp_flash_encrypt_check_and_update(void)
{
    uint32_t efuse_blk0 = REG_READ(EFUSE_BLK0_RDATA0_REG);
    ESP_LOGV(TAG, "efuse_blk0 raw value %08x", efuse_blk0);
    uint32_t flash_crypt_cnt = (efuse_blk0 & EFUSE_RD_FLASH_CRYPT_CNT_M) >> EFUSE_RD_FLASH_CRYPT_CNT_S;
    bool flash_crypt_wr_dis = efuse_blk0 & EFUSE_WR_DIS_FLASH_CRYPT_CNT;
    ESP_LOGV(TAG, "efuse FLASH_CRYPT_CNT 0x%x WR_DIS_FLASH_CRYPT_CNT 0x%x", flash_crypt_cnt, flash_crypt_wr_dis);

    if (__builtin_parity(flash_crypt_cnt) == 1) {
        /* Flash is already encrypted */
        int left = (7 - __builtin_popcount(flash_crypt_cnt)) / 2;
        if (flash_crypt_wr_dis) {
            left = 0; /* can't update FLASH_CRYPT_CNT, no more flashes */
        }
        ESP_LOGI(TAG, "flash encryption is enabled (%d plaintext flashes left)", left);
        return ESP_OK;
    }
    else {
#ifndef CONFIG_SECURE_FLASH_REQUIRE_ALREADY_ENABLED
        /* Flash is not encrypted, so encrypt it! */
        return encrypt_flash_contents(flash_crypt_cnt, flash_crypt_wr_dis);
#else
        ESP_LOGE(TAG, "flash encryption is not enabled, and SECURE_FLASH_REQUIRE_ALREADY_ENABLED "
                      "is set, refusing to boot.");
        return ESP_ERR_INVALID_STATE;
#endif // CONFIG_SECURE_FLASH_REQUIRE_ALREADY_ENABLED
    }
}

static esp_err_t initialise_flash_encryption(void)
{
    uint32_t new_wdata0 = 0;
    uint32_t new_wdata5 = 0;
    uint32_t new_wdata6 = 0;

    uint32_t coding_scheme = REG_GET_FIELD(EFUSE_BLK0_RDATA6_REG, EFUSE_CODING_SCHEME);
    if (coding_scheme != EFUSE_CODING_SCHEME_VAL_NONE && coding_scheme != EFUSE_CODING_SCHEME_VAL_34) {
        ESP_LOGE(TAG, "Unknown/unsupported CODING_SCHEME value 0x%x", coding_scheme);
        return ESP_ERR_NOT_SUPPORTED;
    }

    /* Before first flash encryption pass, need to initialise key & crypto config */

    /* Generate key */
    uint32_t dis_reg = REG_READ(EFUSE_BLK0_RDATA0_REG);
    bool efuse_key_read_protected = dis_reg & EFUSE_RD_DIS_BLK1;
    bool efuse_key_write_protected = dis_reg & EFUSE_WR_DIS_BLK1;
    if (efuse_key_read_protected == false
        && efuse_key_write_protected == false
        && REG_READ(EFUSE_BLK1_RDATA0_REG) == 0
        && REG_READ(EFUSE_BLK1_RDATA1_REG) == 0
        && REG_READ(EFUSE_BLK1_RDATA2_REG) == 0
        && REG_READ(EFUSE_BLK1_RDATA3_REG) == 0
        && REG_READ(EFUSE_BLK1_RDATA4_REG) == 0
        && REG_READ(EFUSE_BLK1_RDATA5_REG) == 0
        && REG_READ(EFUSE_BLK1_RDATA6_REG) == 0
        && REG_READ(EFUSE_BLK1_RDATA7_REG) == 0) {
        ESP_LOGI(TAG, "Generating new flash encryption key...");
        esp_efuse_write_random_key(EFUSE_BLK1_WDATA0_REG);
        // defer efuse programming step to the end

        ESP_LOGI(TAG, "Read & write protecting new key...");
        new_wdata0 |= EFUSE_WR_DIS_BLK1 | EFUSE_RD_DIS_BLK1;
    } else {

        if(!(efuse_key_read_protected && efuse_key_write_protected)) {
            ESP_LOGE(TAG, "Flash encryption key has to be either unset or both read and write protected");
            return ESP_ERR_INVALID_STATE;
        }
        ESP_LOGW(TAG, "Using pre-loaded flash encryption key in EFUSE block 1");
    }
    /* CRYPT_CONFIG determines which bits of the AES block key are XORed
       with bits from the flash address, to provide the key tweak.

       CRYPT_CONFIG == 0 is effectively AES ECB mode (NOT SUPPORTED)

       For now this is hardcoded to XOR all 256 bits of the key.

       If you need to override it, you can pre-burn this efuse to the
       desired value and then write-protect it, in which case this
       operation does nothing. Please note this is not recommended!
    */
    ESP_LOGI(TAG, "Setting CRYPT_CONFIG efuse to 0xF");
    new_wdata5 |= EFUSE_FLASH_CRYPT_CONFIG_M;

#ifndef CONFIG_SECURE_FLASH_UART_BOOTLOADER_ALLOW_ENC
    ESP_LOGI(TAG, "Disable UART bootloader encryption...");
    new_wdata6 |= EFUSE_DISABLE_DL_ENCRYPT;
#else
    ESP_LOGW(TAG, "Not disabling UART bootloader encryption");
#endif

#ifndef CONFIG_SECURE_FLASH_UART_BOOTLOADER_ALLOW_DEC
    ESP_LOGI(TAG, "Disable UART bootloader decryption...");
    new_wdata6 |= EFUSE_DISABLE_DL_DECRYPT;
#else
    ESP_LOGW(TAG, "Not disabling UART bootloader decryption - SECURITY COMPROMISED");
#endif

#ifndef CONFIG_SECURE_FLASH_UART_BOOTLOADER_ALLOW_CACHE
    ESP_LOGI(TAG, "Disable UART bootloader MMU cache...");
    new_wdata6 |= EFUSE_DISABLE_DL_CACHE;
#else
    ESP_LOGW(TAG, "Not disabling UART bootloader MMU cache - SECURITY COMPROMISED");
#endif

#ifndef CONFIG_SECURE_BOOT_ALLOW_JTAG
    ESP_LOGI(TAG, "Disable JTAG...");
    new_wdata6 |= EFUSE_RD_DISABLE_JTAG;
#else
    ESP_LOGW(TAG, "Not disabling JTAG - SECURITY COMPROMISED");
#endif

#ifndef CONFIG_SECURE_BOOT_ALLOW_ROM_BASIC
    ESP_LOGI(TAG, "Disable ROM BASIC interpreter fallback...");
    new_wdata6 |= EFUSE_RD_CONSOLE_DEBUG_DISABLE;
#else
    ESP_LOGW(TAG, "Not disabling ROM BASIC fallback - SECURITY COMPROMISED");
#endif

#if defined(CONFIG_SECURE_BOOT_V2_ENABLED) && !defined(CONFIG_SECURE_BOOT_V2_ALLOW_EFUSE_RD_DIS)
    // This bit is set when enabling Secure Boot V2, but we can't enable it until this later point in the first boot
    // otherwise the Flash Encryption key cannot be read protected
    new_wdata0 |= EFUSE_WR_DIS_RD_DIS;
#endif

    REG_WRITE(EFUSE_BLK0_WDATA0_REG, new_wdata0);
    REG_WRITE(EFUSE_BLK0_WDATA5_REG, new_wdata5);
    REG_WRITE(EFUSE_BLK0_WDATA6_REG, new_wdata6);
    esp_efuse_burn_new_values();

    return ESP_OK;
}

/* Encrypt all flash data that should be encrypted */
static esp_err_t encrypt_flash_contents(uint32_t flash_crypt_cnt, bool flash_crypt_wr_dis)
{
    esp_err_t err;
    esp_partition_info_t partition_table[ESP_PARTITION_TABLE_MAX_ENTRIES];
    int num_partitions;

    /* If the last flash_crypt_cnt bit is burned or write-disabled, the
       device can't re-encrypt itself. */
    if (flash_crypt_wr_dis || flash_crypt_cnt == 0xFF) {
        ESP_LOGE(TAG, "Cannot re-encrypt data (FLASH_CRYPT_CNT 0x%02x write disabled %d", flash_crypt_cnt, flash_crypt_wr_dis);
        return ESP_FAIL;
    }

    if (flash_crypt_cnt == 0) {
        /* Very first flash of encrypted data: generate keys, etc. */
        err = initialise_flash_encryption();
        if (err != ESP_OK) {
            return err;
        }
    }

    err = encrypt_bootloader();
    if (err != ESP_OK) {
        return err;
    }

    err = encrypt_and_load_partition_table(partition_table, &num_partitions);
    if (err != ESP_OK) {
        return err;
    }

    /* Now iterate the just-loaded partition table, looking for entries to encrypt
     */

    /* Go through each partition and encrypt if necessary */
    for (int i = 0; i < num_partitions; i++) {
        err = encrypt_partition(i, &partition_table[i]);
        if (err != ESP_OK) {
            return err;
        }
    }

    ESP_LOGD(TAG, "All flash regions checked for encryption pass");

    uint32_t new_flash_crypt_cnt;
#ifdef CONFIG_SECURE_FLASH_ENCRYPTION_MODE_RELEASE
    // Go straight to max, permanently enabled
    ESP_LOGI(TAG, "Setting FLASH_CRYPT_CNT for permanent encryption");
    new_flash_crypt_cnt = EFUSE_FLASH_CRYPT_CNT;
#else
    /* Set least significant 0-bit in flash_crypt_cnt */
    int ffs_inv = __builtin_ffs((~flash_crypt_cnt) & EFUSE_RD_FLASH_CRYPT_CNT);
    /* ffs_inv shouldn't be zero, as zero implies flash_crypt_cnt == EFUSE_RD_FLASH_CRYPT_CNT (0x7F) */
    new_flash_crypt_cnt = flash_crypt_cnt + (1 << (ffs_inv - 1));
#endif
    ESP_LOGD(TAG, "FLASH_CRYPT_CNT 0x%x -> 0x%x", flash_crypt_cnt, new_flash_crypt_cnt);
    uint32_t wdata0_reg = ((new_flash_crypt_cnt & EFUSE_FLASH_CRYPT_CNT) << EFUSE_FLASH_CRYPT_CNT_S);

    REG_WRITE(EFUSE_BLK0_WDATA0_REG, wdata0_reg);
    esp_efuse_burn_new_values();

    ESP_LOGI(TAG, "Flash encryption completed");

    return ESP_OK;
}

static esp_err_t encrypt_bootloader(void)
{
    esp_err_t err;
    uint32_t image_length;
    /* Check for plaintext bootloader (verification will fail if it's already encrypted) */
    if (esp_image_verify_bootloader(&image_length) == ESP_OK) {
        ESP_LOGD(TAG, "bootloader is plaintext. Encrypting...");

#if CONFIG_SECURE_BOOT_V2_ENABLED
        // Account for the signature sector after the bootloader
        image_length = (image_length + FLASH_SECTOR_SIZE - 1) & ~(FLASH_SECTOR_SIZE - 1);
        image_length += FLASH_SECTOR_SIZE;
        if (ESP_BOOTLOADER_OFFSET + image_length > ESP_PARTITION_TABLE_OFFSET) {
            ESP_LOGE(TAG, "Bootloader is too large to fit Secure Boot V2 signature sector and partition table (configured offset 0x%x)", ESP_PARTITION_TABLE_OFFSET);
            return ESP_ERR_INVALID_STATE;
        }
#endif // CONFIG_SECURE_BOOT_V2_ENABLED

        err = esp_flash_encrypt_region(ESP_BOOTLOADER_OFFSET, image_length);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to encrypt bootloader in place: 0x%x", err);
            return err;
        }

#ifdef CONFIG_SECURE_BOOT_V1_ENABLED
        /* If secure boot is enabled and bootloader was plaintext, also
         * need to encrypt secure boot IV+digest.
         */
        ESP_LOGD(TAG, "Encrypting secure bootloader IV & digest...");
        err = esp_flash_encrypt_region(FLASH_OFFS_SECURE_BOOT_IV_DIGEST,
                                       FLASH_SECTOR_SIZE);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to encrypt bootloader IV & digest in place: 0x%x", err);
            return err;
        }
#endif
    }
    else {
        ESP_LOGW(TAG, "no valid bootloader was found");
    }

    return ESP_OK;
}

static esp_err_t encrypt_and_load_partition_table(esp_partition_info_t *partition_table, int *num_partitions)
{
    esp_err_t err;
    /* Check for plaintext partition table */
    err = bootloader_flash_read(ESP_PARTITION_TABLE_OFFSET, partition_table, ESP_PARTITION_TABLE_MAX_LEN, false);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read partition table data");
        return err;
    }
    if (esp_partition_table_verify(partition_table, false, num_partitions) == ESP_OK) {
        ESP_LOGD(TAG, "partition table is plaintext. Encrypting...");
        esp_err_t err = esp_flash_encrypt_region(ESP_PARTITION_TABLE_OFFSET,
                                                 FLASH_SECTOR_SIZE);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to encrypt partition table in place. %x", err);
            return err;
        }
    }
    else {
        ESP_LOGE(TAG, "Failed to read partition table data - not plaintext?");
        return ESP_ERR_INVALID_STATE;
    }

    /* Valid partition table loded */
    return ESP_OK;
}


static esp_err_t encrypt_partition(int index, const esp_partition_info_t *partition)
{
    esp_err_t err;
    bool should_encrypt = (partition->flags & PART_FLAG_ENCRYPTED);

    if (partition->type == PART_TYPE_APP) {
      /* check if the partition holds a valid unencrypted app */
      esp_image_metadata_t data_ignored;
      err = esp_image_verify(ESP_IMAGE_VERIFY,
                           &partition->pos,
                           &data_ignored);
      should_encrypt = (err == ESP_OK);
    } else if ((partition->type == PART_TYPE_DATA && partition->subtype == PART_SUBTYPE_DATA_OTA)
                || (partition->type == PART_TYPE_DATA && partition->subtype == PART_SUBTYPE_DATA_NVS_KEYS)) {
        /* check if we have ota data partition and the partition should be encrypted unconditionally */
        should_encrypt = true;
    }

    if (!should_encrypt) {
        return ESP_OK;
    }
    else {
        /* should_encrypt */
        ESP_LOGI(TAG, "Encrypting partition %d at offset 0x%x...", index, partition->pos.offset);

        err = esp_flash_encrypt_region(partition->pos.offset, partition->pos.size);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to encrypt partition %d", index);
        }
        return err;
    }
}


esp_err_t esp_flash_encrypt_region(uint32_t src_addr, size_t data_length)
{
    esp_err_t err;
    uint32_t buf[FLASH_SECTOR_SIZE / sizeof(uint32_t)];

    if (src_addr % FLASH_SECTOR_SIZE != 0) {
        ESP_LOGE(TAG, "esp_flash_encrypt_region bad src_addr 0x%x",src_addr);
        return ESP_FAIL;
    }

    wdt_hal_context_t rtc_wdt_ctx = {.inst = WDT_RWDT, .rwdt_dev = &RTCCNTL};
    for (size_t i = 0; i < data_length; i += FLASH_SECTOR_SIZE) {
        wdt_hal_write_protect_disable(&rtc_wdt_ctx);
        wdt_hal_feed(&rtc_wdt_ctx);
        wdt_hal_write_protect_enable(&rtc_wdt_ctx);
        uint32_t sec_start = i + src_addr;
        err = bootloader_flash_read(sec_start, buf, FLASH_SECTOR_SIZE, false);
        if (err != ESP_OK) {
            goto flash_failed;
        }
        err = bootloader_flash_erase_sector(sec_start / FLASH_SECTOR_SIZE);
        if (err != ESP_OK) {
            goto flash_failed;
        }
        err = bootloader_flash_write(sec_start, buf, FLASH_SECTOR_SIZE, true);
        if (err != ESP_OK) {
            goto flash_failed;
        }
    }
    return ESP_OK;

 flash_failed:
    ESP_LOGE(TAG, "flash operation failed: 0x%x", err);
    return err;
}
