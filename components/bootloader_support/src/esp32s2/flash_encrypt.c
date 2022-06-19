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

#include "bootloader_flash_priv.h"
#include "bootloader_random.h"
#include "bootloader_utility.h"
#include "esp_image_format.h"
#include "esp_flash_encrypt.h"
#include "esp_flash_partitions.h"
#include "esp_secure_boot.h"
#include "esp_log.h"
#include "esp32s2/rom/secure_boot.h"
#include "esp32s2/rom/cache.h"
#include "esp32s2/rom/efuse.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "hal/wdt_hal.h"

static const char *TAG = "flash_encrypt";

/* Static functions for stages of flash encryption */
static esp_err_t initialise_flash_encryption(void);
static esp_err_t encrypt_flash_contents(uint32_t flash_crypt_cnt, bool flash_crypt_wr_dis) __attribute__((unused));
static esp_err_t encrypt_bootloader(void);
static esp_err_t encrypt_and_load_partition_table(esp_partition_info_t *partition_table, int *num_partitions);
static esp_err_t encrypt_partition(int index, const esp_partition_info_t *partition);

esp_err_t esp_flash_encrypt_check_and_update(void)
{
    uint8_t flash_crypt_wr_dis = 0;
    uint32_t flash_crypt_cnt = 0;

    esp_efuse_read_field_blob(ESP_EFUSE_SPI_BOOT_CRYPT_CNT, &flash_crypt_cnt, 3);
    esp_efuse_read_field_blob(ESP_EFUSE_WR_DIS_SPI_BOOT_CRYPT_CNT, &flash_crypt_wr_dis, 1);

    ESP_LOGV(TAG, "SPI_BOOT_CRYPT_CNT 0x%x", flash_crypt_cnt);
    ESP_LOGV(TAG, "EFUSE_WR_DIS_SPI_BOOT_CRYPT_CNT 0x%x", flash_crypt_wr_dis);

    _Static_assert(EFUSE_SPI_BOOT_CRYPT_CNT == 0x7, "assuming CRYPT_CNT is only 3 bits wide");

    if (__builtin_parity(flash_crypt_cnt) == 1) {
        /* Flash is already encrypted */
        int left = (flash_crypt_cnt == 1) ? 1 : 0;
        if (flash_crypt_wr_dis) {
            left = 0; /* can't update FLASH_CRYPT_CNT, no more flashes */
        }
        ESP_LOGI(TAG, "flash encryption is enabled (%d plaintext flashes left)", left);
        return ESP_OK;
    } else {

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

static bool s_key_dis_read(ets_efuse_block_t block)
{
    unsigned key_num = block - ETS_EFUSE_BLOCK_KEY0;
    return REG_GET_FIELD(EFUSE_RD_REPEAT_DATA0_REG, EFUSE_RD_DIS) & (EFUSE_RD_DIS_KEY0 << key_num);
}

static bool s_key_dis_write(ets_efuse_block_t block)
{
    unsigned key_num = block - ETS_EFUSE_BLOCK_KEY0;
    return REG_GET_FIELD(EFUSE_RD_WR_DIS_REG, EFUSE_WR_DIS) & (EFUSE_WR_DIS_KEY0 << key_num);
}

static esp_err_t check_and_generate_encryption_keys(void)
{
    esp_err_t err = ESP_ERR_INVALID_STATE;
    ets_efuse_block_t aes_128_key_block;
    ets_efuse_block_t aes_256_key_block_1;
    ets_efuse_block_t aes_256_key_block_2;

    bool has_aes128 = ets_efuse_find_purpose(ETS_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY, &aes_128_key_block);
    bool has_aes256_1 = ets_efuse_find_purpose(ETS_EFUSE_KEY_PURPOSE_XTS_AES_256_KEY_1, &aes_256_key_block_1);
    bool has_aes256_2 = ets_efuse_find_purpose(ETS_EFUSE_KEY_PURPOSE_XTS_AES_256_KEY_2, &aes_256_key_block_2);
    bool has_key = has_aes128 || (has_aes256_1 && has_aes256_2);
    bool dis_write = false;
    bool dis_read = false;

    // If there are keys set, they must be write and read protected!
    if(has_key && has_aes128) {
        dis_write = s_key_dis_write(aes_128_key_block);
        dis_read = s_key_dis_read(aes_128_key_block);
    } else if (has_key && has_aes256_1 && has_aes256_2) {
        dis_write = s_key_dis_write(aes_256_key_block_1) && s_key_dis_write(aes_256_key_block_2);
        dis_read = s_key_dis_read(aes_256_key_block_1) && s_key_dis_read(aes_256_key_block_2);
    }

    if (!has_key && (has_aes256_1 || has_aes256_2)) {
        ESP_LOGE(TAG, "Invalid efuse key blocks: Both AES-256 key blocks must be set.");
        return ESP_ERR_INVALID_STATE;
    }

    if(has_key && (!dis_read || !dis_write)) {
        ESP_LOGE(TAG, "Invalid key state, a key was set but not read and write protected.");
        return ESP_ERR_INVALID_STATE;
    }

    if(!has_key && !dis_write && !dis_read) {
        ESP_LOGI(TAG, "Generating new flash encryption key...");
#ifdef CONFIG_SECURE_FLASH_ENCRYPTION_AES256
        const unsigned BLOCKS_NEEDED = 2;
        const ets_efuse_purpose_t PURPOSE_START = ETS_EFUSE_KEY_PURPOSE_XTS_AES_256_KEY_1;
        const ets_efuse_purpose_t PURPOSE_END = ETS_EFUSE_KEY_PURPOSE_XTS_AES_256_KEY_2;
#else
        const unsigned BLOCKS_NEEDED = 1;
        const ets_efuse_purpose_t PURPOSE_START = ETS_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY;
        const ets_efuse_purpose_t PURPOSE_END = ETS_EFUSE_KEY_PURPOSE_XTS_AES_128_KEY;
#endif

        if (ets_efuse_count_unused_key_blocks() < BLOCKS_NEEDED) {
            ESP_LOGE(TAG, "Not enough free efuse key blocks (need %d) to continue", BLOCKS_NEEDED);
            return ESP_ERR_INVALID_STATE;
        }

        for(ets_efuse_purpose_t purpose = PURPOSE_START; purpose <= PURPOSE_END; purpose++) {
            uint32_t buf[8] = {0};
            bootloader_fill_random(buf, sizeof(buf));
            ets_efuse_block_t block = ets_efuse_find_unused_key_block();
            ESP_LOGD(TAG, "Writing ETS_EFUSE_BLOCK_KEY%d with purpose %d",
                     block - ETS_EFUSE_BLOCK_KEY0, purpose);

            /* Note: everything else in this function is deferred as a batch write, but we write the
               key (and write protect it) immediately as it's too fiddly to manage unused key blocks, etc.
               in bootloader size footprint otherwise. */
            int r = ets_efuse_write_key(block, purpose, buf, sizeof(buf));
            if (r != 0) {
                ESP_LOGE(TAG, "Failed to write efuse block %d with purpose %d. Can't continue.",
                        block, purpose);
                return ESP_FAIL;
            }

            /* assuming numbering of esp_efuse_block_t matches ets_efuse_block_t */
            _Static_assert((int)EFUSE_BLK_KEY0 == (int)ETS_EFUSE_BLOCK_KEY0, "esp_efuse_block_t doesn't match ets_efuse_block_t");
            _Static_assert((int)EFUSE_BLK_KEY1 == (int)ETS_EFUSE_BLOCK_KEY1, "esp_efuse_block_t doesn't match ets_efuse_block_t");
            _Static_assert((int)EFUSE_BLK_KEY2 == (int)ETS_EFUSE_BLOCK_KEY2, "esp_efuse_block_t doesn't match ets_efuse_block_t");
            _Static_assert((int)EFUSE_BLK_KEY3 == (int)ETS_EFUSE_BLOCK_KEY3, "esp_efuse_block_t doesn't match ets_efuse_block_t");
            _Static_assert((int)EFUSE_BLK_KEY4 == (int)ETS_EFUSE_BLOCK_KEY4, "esp_efuse_block_t doesn't match ets_efuse_block_t");
            _Static_assert((int)EFUSE_BLK_KEY5 == (int)ETS_EFUSE_BLOCK_KEY5, "esp_efuse_block_t doesn't match ets_efuse_block_t");

            // protect this block against reading after key is set (writing is done by ets_efuse_write_key)
            err = esp_efuse_set_read_protect(block);
            if(err != ESP_OK) {
                ESP_LOGE(TAG, "Failed to set read protect to efuse block %d. Can't continue.", block);
                return err;
            }
        }
        ESP_LOGD(TAG, "Key generation complete");
        return ESP_OK;

    } else {
        ESP_LOGI(TAG, "Using pre-existing key in efuse");
        return ESP_OK;
    }
}

static esp_err_t initialise_flash_encryption(void)
{
    esp_efuse_batch_write_begin(); /* Batch all efuse writes at the end of this function */

    esp_err_t key_state = check_and_generate_encryption_keys();
    if(key_state != ESP_OK) {
        esp_efuse_batch_write_cancel();
        return key_state;
    }

#ifndef CONFIG_SECURE_FLASH_UART_BOOTLOADER_ALLOW_ENC
    ESP_LOGI(TAG, "Disable UART bootloader encryption...");
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_DOWNLOAD_MANUAL_ENCRYPT);
#else
    ESP_LOGW(TAG, "Not disabling UART bootloader encryption");
#endif

#ifndef CONFIG_SECURE_FLASH_UART_BOOTLOADER_ALLOW_CACHE
    ESP_LOGI(TAG, "Disable UART bootloader cache...");
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_DOWNLOAD_DCACHE);
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_DOWNLOAD_ICACHE);
#else
    ESP_LOGW(TAG, "Not disabling UART bootloader cache - SECURITY COMPROMISED");
#endif

#ifndef CONFIG_SECURE_BOOT_ALLOW_JTAG
    ESP_LOGI(TAG, "Disable JTAG...");
    esp_efuse_write_field_bit(ESP_EFUSE_HARD_DIS_JTAG);
#else
    ESP_LOGW(TAG, "Not disabling JTAG - SECURITY COMPROMISED");
#endif

    esp_efuse_write_field_bit(ESP_EFUSE_DIS_BOOT_REMAP);
    esp_efuse_write_field_bit(ESP_EFUSE_DIS_LEGACY_SPI_BOOT);

#if defined(CONFIG_SECURE_BOOT_V2_ENABLED) && !defined(CONFIG_SECURE_BOOT_V2_ALLOW_EFUSE_RD_DIS)
    // This bit is set when enabling Secure Boot V2, but we can't enable it until this later point in the first boot
    // otherwise the Flash Encryption key cannot be read protected
    esp_efuse_write_field_bit(ESP_EFUSE_WR_DIS_RD_DIS);
#endif

    esp_err_t err = esp_efuse_batch_write_commit();

    return err;
}

/* Encrypt all flash data that should be encrypted */
static esp_err_t encrypt_flash_contents(uint32_t spi_boot_crypt_cnt, bool flash_crypt_wr_dis)
{
    esp_err_t err;
    esp_partition_info_t partition_table[ESP_PARTITION_TABLE_MAX_ENTRIES];
    int num_partitions;

    /* If the last spi_boot_crypt_cnt bit is burned or write-disabled, the
       device can't re-encrypt itself. */
    if (flash_crypt_wr_dis || spi_boot_crypt_cnt == EFUSE_SPI_BOOT_CRYPT_CNT) {
        ESP_LOGE(TAG, "Cannot re-encrypt data SPI_BOOT_CRYPT_CNT 0x%02x write disabled %d", spi_boot_crypt_cnt, flash_crypt_wr_dis);
        return ESP_FAIL;
    }

    if (spi_boot_crypt_cnt == 0) {
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

    /* Now iterate the just-loaded partition table, looking for entries to encrypt */
    for (int i = 0; i < num_partitions; i++) {
        err = encrypt_partition(i, &partition_table[i]);
        if (err != ESP_OK) {
            return err;
        }
    }

    ESP_LOGD(TAG, "All flash regions checked for encryption pass");

    /* Set least significant 0-bit in spi_boot_crypt_cnt */
    int ffs_inv = __builtin_ffs((~spi_boot_crypt_cnt) & 0x7);
    /* ffs_inv shouldn't be zero, as zero implies spi_boot_crypt_cnt == 0xFF */
    uint32_t new_spi_boot_crypt_cnt = (1 << (ffs_inv - 1));
    ESP_LOGD(TAG, "SPI_BOOT_CRYPT_CNT 0x%x -> 0x%x", spi_boot_crypt_cnt, new_spi_boot_crypt_cnt + spi_boot_crypt_cnt);

    esp_efuse_write_field_blob(ESP_EFUSE_SPI_BOOT_CRYPT_CNT, &new_spi_boot_crypt_cnt, 3);

#ifdef CONFIG_SECURE_FLASH_ENCRYPTION_MODE_RELEASE
    //Secure SPI boot cnt after its update if needed.
    const uint32_t spi_boot_cnt_wr_dis = 1;
    ESP_LOGI(TAG, "Write protecting SPI_CRYPT_CNT eFuse");
    esp_efuse_write_field_blob(ESP_EFUSE_WR_DIS_SPI_BOOT_CRYPT_CNT, &spi_boot_cnt_wr_dis, 1);
#endif
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
        /* The image length obtained from esp_image_verify_bootloader includes the sector boundary padding and the signature block lengths */
        if (ESP_BOOTLOADER_OFFSET + image_length > ESP_PARTITION_TABLE_OFFSET) {
            ESP_LOGE(TAG, "Bootloader is too large to fit Secure Boot V2 signature sector and partition table (configured offset 0x%x)", ESP_PARTITION_TABLE_OFFSET);
            return ESP_ERR_INVALID_SIZE;
        }
#endif // CONFIG_SECURE_BOOT_V2_ENABLED

        err = esp_flash_encrypt_region(ESP_BOOTLOADER_OFFSET, image_length);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to encrypt bootloader in place: 0x%x", err);
            return err;
        }

        ESP_LOGI(TAG, "bootloader encrypted successfully");
        return err;
    }
    else {
        ESP_LOGW(TAG, "no valid bootloader was found");
        return ESP_ERR_NOT_FOUND;
    }
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
    ESP_LOGI(TAG, "partition table encrypted and loaded successfully");
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
    } else if (partition->type == PART_TYPE_DATA && partition->subtype == PART_SUBTYPE_DATA_OTA) {
        /* check if we have ota data partition and the partition should be encrypted unconditionally */
        should_encrypt = true;
    }

    if (!should_encrypt) {
        return ESP_OK;
    }
    else {
        /* should_encrypt */
        ESP_LOGI(TAG, "Encrypting partition %d at offset 0x%x (length 0x%x)...", index, partition->pos.offset, partition->pos.size);

        err = esp_flash_encrypt_region(partition->pos.offset, partition->pos.size);
        ESP_LOGI(TAG, "Done encrypting");
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
