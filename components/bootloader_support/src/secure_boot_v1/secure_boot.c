/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "esp_attr.h"
#include "esp_types.h"
#include "esp_log.h"

#include "esp32/rom/cache.h"
#include "esp32/rom/secure_boot.h"

#include "soc/rtc_periph.h"
#include "bootloader_utility.h"

#include "sdkconfig.h"

#include "bootloader_flash_priv.h"
#include "bootloader_random.h"
#include "esp_image_format.h"
#include "esp_secure_boot.h"
#include "esp_flash_encrypt.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"

/* The following API implementations are used only when called
 * from the bootloader code.
 */

#ifdef CONFIG_SECURE_BOOT_V1_ENABLED
static const char *TAG = "secure_boot_v1";
/**
 *  @function :     secure_boot_generate
 *  @description:   generate boot digest (aka "abstract") & iv
 *
 *  @inputs:        image_len - length of image to calculate digest for
 */
static bool secure_boot_generate(uint32_t image_len){
    esp_err_t err;
    esp_secure_boot_iv_digest_t digest;
    const uint32_t *image;

    /* hardware secure boot engine only takes full blocks, so round up the
       image length. The additional data should all be 0xFF (or the appended SHA, if it falls in the same block).
    */
    if (image_len % sizeof(digest.iv) != 0) {
        image_len = (image_len / sizeof(digest.iv) + 1) * sizeof(digest.iv);
    }
    ets_secure_boot_start();
    ets_secure_boot_rd_iv((uint32_t *)digest.iv);
    ets_secure_boot_hash(NULL);
    /* iv stored in sec 0 */
    err = bootloader_flash_erase_sector(0);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "SPI erase failed: 0x%x", err);
        return false;
    }

    /* generate digest from image contents */
    image = bootloader_mmap(ESP_BOOTLOADER_OFFSET, image_len);
    if (!image) {
        ESP_LOGE(TAG, "bootloader_mmap(0x1000, 0x%x) failed", image_len);
        return false;
    }
    for (size_t i = 0; i < image_len; i+= sizeof(digest.iv)) {
        ets_secure_boot_hash(&image[i/sizeof(uint32_t)]);
    }
    bootloader_munmap(image);

    ets_secure_boot_obtain();
    ets_secure_boot_rd_abstract((uint32_t *)digest.digest);
    ets_secure_boot_finish();

    ESP_LOGD(TAG, "write iv+digest to flash");
    err = bootloader_flash_write(FLASH_OFFS_SECURE_BOOT_IV_DIGEST, &digest,
                           sizeof(digest), esp_flash_encryption_enabled());
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "SPI write failed: 0x%x", err);
        return false;
    }
    Cache_Read_Enable(0);
    return true;
}

esp_err_t esp_secure_boot_generate_digest(void)
{
    esp_err_t err;
    if (esp_secure_boot_enabled()) {
        ESP_LOGI(TAG, "bootloader secure boot is already enabled."
                      " No need to generate digest. continuing..");
        return ESP_OK;
    }

    esp_efuse_coding_scheme_t coding_scheme = esp_efuse_get_coding_scheme(EFUSE_BLK_SECURE_BOOT);
    if (coding_scheme != EFUSE_CODING_SCHEME_NONE && coding_scheme != EFUSE_CODING_SCHEME_3_4) {
        ESP_LOGE(TAG, "Unknown/unsupported CODING_SCHEME value 0x%x", coding_scheme);
        return ESP_ERR_NOT_SUPPORTED;
    }

    /* Verify the bootloader */
    esp_image_metadata_t bootloader_data = { 0 };
    err = esp_image_verify_bootloader_data(&bootloader_data);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "bootloader image appears invalid! error %d", err);
        return err;
    }

    bool dis_write = esp_efuse_read_field_bit(ESP_EFUSE_WR_DIS_BLK2);
    bool dis_read  = esp_efuse_read_field_bit(ESP_EFUSE_RD_DIS_BLK2);
    bool key_is_empty = esp_efuse_block_is_empty(EFUSE_BLK_SECURE_BOOT);

    /* Generate secure boot key and keep in EFUSE */
    if (!dis_read && !dis_write && key_is_empty) {
        ESP_LOGI(TAG, "Generating new secure boot key...");
        uint32_t key[8];
        size_t key_size = 256;
        if (coding_scheme == EFUSE_CODING_SCHEME_3_4) {
            key_size = 192;
        }
        bootloader_fill_random(key, key_size / 8);
        esp_efuse_write_block(EFUSE_BLK_SECURE_BOOT, key, 0, key_size);
    } else {
        ESP_LOGW(TAG, "Using pre-loaded secure boot key in EFUSE block 2");
    }

    /* Generate secure boot digest using programmed key in EFUSE */
    ESP_LOGI(TAG, "Generating secure boot digest...");
    uint32_t image_len = bootloader_data.image_len;
    if(bootloader_data.image.hash_appended) {
        /* Secure boot digest doesn't cover the hash */
        image_len -= ESP_IMAGE_HASH_LEN;
    }
    if (false == secure_boot_generate(image_len)){
        ESP_LOGE(TAG, "secure boot generation failed");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Digest generation complete.");

    return ESP_OK;
}

esp_err_t esp_secure_boot_permanently_enable(void)
{
    if (esp_secure_boot_enabled()) {
        ESP_LOGI(TAG, "bootloader secure boot is already enabled, continuing..");
        return ESP_OK;
    }

    bool dis_read  = esp_efuse_read_field_bit(ESP_EFUSE_RD_DIS_BLK2);
    bool dis_write = esp_efuse_read_field_bit(ESP_EFUSE_WR_DIS_BLK2);
    if (dis_read != dis_write) {
        ESP_LOGE(TAG, "Pre-loaded key is not %s %s protected. Refusing to blow secure boot efuse.",
                    (!dis_read) ? "read,":" ",
                    (!dis_read) ? "write":" ");
        return ESP_ERR_INVALID_STATE;
    }
    esp_efuse_batch_write_begin(); /* Batch all efuse writes at the end of this function */

    ESP_LOGI(TAG, "blowing secure boot efuse...");
    esp_err_t err = esp_secure_boot_enable_secure_features();
    if (err != ESP_OK) {
        esp_efuse_batch_write_cancel();
        return err;
    }

    err = esp_efuse_batch_write_commit();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error programming security eFuses (err=0x%x).", err);
        return err;
    }

    assert(esp_secure_boot_enabled());

    ESP_LOGI(TAG, "secure boot is now enabled for bootloader image");
    return ESP_OK;
}
#endif // CONFIG_SECURE_BOOT_V1_ENABLED
