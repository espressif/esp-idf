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

#include <string.h>

#include "esp_attr.h"
#include "esp_types.h"
#include "esp_log.h"

#include "esp32/rom/cache.h"
#include "esp32/rom/ets_sys.h"
#include "esp32/rom/secure_boot.h"

#include "soc/efuse_periph.h"
#include "soc/rtc_periph.h"

#include "sdkconfig.h"

#include "bootloader_flash.h"
#include "bootloader_random.h"
#include "esp_image_format.h"
#include "esp_secure_boot.h"
#include "esp_flash_encrypt.h"
#include "esp_efuse.h"

/* The following API implementations are used only when called
 * from the bootloader code.
 */

static const char* TAG = "secure_boot";

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
    for (int i = 0; i < image_len; i+= sizeof(digest.iv)) {
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

/* Burn values written to the efuse write registers */
static inline void burn_efuses()
{
    esp_efuse_burn_new_values();
}

esp_err_t esp_secure_boot_generate_digest(void)
{
    esp_err_t err;
    if (esp_secure_boot_enabled()) {
        ESP_LOGI(TAG, "bootloader secure boot is already enabled."
                      " No need to generate digest. continuing..");
        return ESP_OK;
    }

    uint32_t coding_scheme = REG_GET_FIELD(EFUSE_BLK0_RDATA6_REG, EFUSE_CODING_SCHEME);
    if (coding_scheme != EFUSE_CODING_SCHEME_VAL_NONE && coding_scheme != EFUSE_CODING_SCHEME_VAL_34) {
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

    /* Generate secure boot key and keep in EFUSE */
    uint32_t dis_reg = REG_READ(EFUSE_BLK0_RDATA0_REG);
    bool efuse_key_read_protected = dis_reg & EFUSE_RD_DIS_BLK2;
    bool efuse_key_write_protected = dis_reg & EFUSE_WR_DIS_BLK2;
    if (efuse_key_read_protected == false
        && efuse_key_write_protected == false
        && REG_READ(EFUSE_BLK2_RDATA0_REG) == 0
        && REG_READ(EFUSE_BLK2_RDATA1_REG) == 0
        && REG_READ(EFUSE_BLK2_RDATA2_REG) == 0
        && REG_READ(EFUSE_BLK2_RDATA3_REG) == 0
        && REG_READ(EFUSE_BLK2_RDATA4_REG) == 0
        && REG_READ(EFUSE_BLK2_RDATA5_REG) == 0
        && REG_READ(EFUSE_BLK2_RDATA6_REG) == 0
        && REG_READ(EFUSE_BLK2_RDATA7_REG) == 0) {
        ESP_LOGI(TAG, "Generating new secure boot key...");
        esp_efuse_write_random_key(EFUSE_BLK2_WDATA0_REG);
        burn_efuses();
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

    uint32_t dis_reg = REG_READ(EFUSE_BLK0_RDATA0_REG);
    bool efuse_key_read_protected = dis_reg & EFUSE_RD_DIS_BLK2;
    bool efuse_key_write_protected = dis_reg & EFUSE_WR_DIS_BLK2;
    if (efuse_key_read_protected == false
        && efuse_key_write_protected == false) {
        ESP_LOGI(TAG, "Read & write protecting new key...");
        REG_WRITE(EFUSE_BLK0_WDATA0_REG, EFUSE_WR_DIS_BLK2 | EFUSE_RD_DIS_BLK2);
        burn_efuses();
        efuse_key_read_protected = true;
        efuse_key_write_protected = true;
    }

    if (!efuse_key_read_protected) {
        ESP_LOGE(TAG, "Pre-loaded key is not read protected. Refusing to blow secure boot efuse.");
        return ESP_ERR_INVALID_STATE;
    }
    if (!efuse_key_write_protected) {
        ESP_LOGE(TAG, "Pre-loaded key is not write protected. Refusing to blow secure boot efuse.");
        return ESP_ERR_INVALID_STATE;
    }

    ESP_LOGI(TAG, "blowing secure boot efuse...");
    ESP_LOGD(TAG, "before updating, EFUSE_BLK0_RDATA6 %x", REG_READ(EFUSE_BLK0_RDATA6_REG));

    uint32_t new_wdata6 = EFUSE_RD_ABS_DONE_0;

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

    REG_WRITE(EFUSE_BLK0_WDATA6_REG, new_wdata6);
    burn_efuses();
    uint32_t after = REG_READ(EFUSE_BLK0_RDATA6_REG);
    ESP_LOGD(TAG, "after updating, EFUSE_BLK0_RDATA6 %x", after);
    if (after & EFUSE_RD_ABS_DONE_0) {
        ESP_LOGI(TAG, "secure boot is now enabled for bootloader image");
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "secure boot not enabled for bootloader image, EFUSE_RD_ABS_DONE_0 is probably write protected!");
        return ESP_ERR_INVALID_STATE;
    }
}
