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
#include "esp32/rom/crc.h"

#include "soc/efuse_periph.h"
#include "soc/rtc_periph.h"
#include "bootloader_sha.h"
#include "bootloader_utility.h"

#include "sdkconfig.h"

#include "bootloader_flash_priv.h"
#include "bootloader_random.h"
#include "esp_image_format.h"
#include "esp_secure_boot.h"
#include "esp_flash_encrypt.h"
#include "esp_efuse.h"

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
        esp_efuse_burn_new_values();
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
    uint32_t new_wdata0 = 0;
    uint32_t new_wdata6 = 0;

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
        new_wdata0 = EFUSE_WR_DIS_BLK2 | EFUSE_RD_DIS_BLK2;
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

    new_wdata6 |= EFUSE_RD_ABS_DONE_0;

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

    REG_WRITE(EFUSE_BLK0_WDATA0_REG, new_wdata0);
    REG_WRITE(EFUSE_BLK0_WDATA6_REG, new_wdata6);
    esp_efuse_burn_new_values();
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
#elif CONFIG_SECURE_BOOT_V2_ENABLED

#define ALIGN_UP(num, align) (((num) + ((align) - 1)) & ~((align) - 1))
static const char *TAG = "secure_boot_v2";

#define SIG_BLOCK_MAGIC_BYTE 0xe7
#define CRC_SIGN_BLOCK_LEN 1196
#define SIG_BLOCK_PADDING 4096

#define DIGEST_LEN 32

static esp_err_t validate_signature_block(const ets_secure_boot_signature_t *sig_block, uint8_t *digest)
{
    uint32_t crc = crc32_le(0, (uint8_t *)sig_block, CRC_SIGN_BLOCK_LEN);
    if (sig_block->block[0].magic_byte == SIG_BLOCK_MAGIC_BYTE && sig_block->block[0].block_crc == crc && !memcmp(digest, sig_block->block[0].image_digest, DIGEST_LEN)) {
        ESP_LOGI(TAG, "valid signature block found");
        return ESP_OK;
    }
    return ESP_FAIL;
}

static esp_err_t secure_boot_v2_digest_generate(uint32_t flash_offset, uint32_t flash_size, uint8_t *public_key_digest)
{
    esp_err_t ret = ESP_FAIL;

    uint8_t image_digest[DIGEST_LEN] = {0};
    size_t sig_block_addr = flash_offset + ALIGN_UP(flash_size, FLASH_SECTOR_SIZE);
    ret = bootloader_sha256_flash_contents(flash_offset, sig_block_addr - flash_offset, image_digest);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "error generating image digest, %d", ret);
        return ret;
    }

    ESP_LOGD(TAG, "reading signature block");
    const ets_secure_boot_signature_t *sig_block = bootloader_mmap(sig_block_addr, sizeof(ets_secure_boot_signature_t));
    if (sig_block == NULL) {
        ESP_LOGE(TAG, "bootloader_mmap(0x%x, 0x%x) failed", sig_block_addr, sizeof(ets_secure_boot_signature_t));
        return ret;
    }

    /* Validating Signature block */
    ret = validate_signature_block(sig_block, image_digest);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "signature block (address 0x%x) validation failed %d", sig_block_addr, ret);
        goto done;
    }

    /* Verifying Signature block */
    uint8_t verified_digest[DIGEST_LEN] = {0};

    /* Generating the SHA of the public key components in the signature block */
    bootloader_sha256_handle_t sig_block_sha;
    sig_block_sha = bootloader_sha256_start();
    bootloader_sha256_data(sig_block_sha, &sig_block->block[0].key, sizeof(sig_block->block[0].key));
    bootloader_sha256_finish(sig_block_sha, public_key_digest);

    secure_boot_v2_status_t error;
    error = ets_secure_boot_verify_signature(sig_block, image_digest, public_key_digest, verified_digest);
    if (error != SBV2_SUCCESS) {
        ESP_LOGE(TAG, "secure boot v2 verification failed %d", error);
        ret = ESP_FAIL;
        goto done;
    } else {
        ret = ESP_OK;
    }

done:
    bootloader_munmap(sig_block);
    return ret;
}

esp_err_t esp_secure_boot_v2_permanently_enable(const esp_image_metadata_t *image_data)
{
    uint32_t new_wdata0 = 0;
    uint32_t new_wdata6 = 0;

    ESP_LOGI(TAG, "enabling secure boot v2...");
    esp_err_t ret;
    if (esp_secure_boot_enabled()) {
        ESP_LOGI(TAG, "secure boot v2 is already enabled. Continuing..");
        return ESP_OK;
    }

    uint32_t coding_scheme = REG_GET_FIELD(EFUSE_BLK0_RDATA6_REG, EFUSE_CODING_SCHEME);
    if (coding_scheme != EFUSE_CODING_SCHEME_VAL_NONE) {
        ESP_LOGE(TAG, "No coding schemes are supported in secure boot v2.(Detected scheme: 0x%x)", coding_scheme);
        return ESP_ERR_NOT_SUPPORTED;
    }

    /* Verify the bootloader */
    esp_image_metadata_t bootloader_data = { 0 };
    ret = esp_image_verify_bootloader_data(&bootloader_data);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "bootloader image appears invalid! error %d", ret);
        return ret;
    }

    uint8_t boot_pub_key_digest[DIGEST_LEN];
    uint32_t dis_reg = REG_READ(EFUSE_BLK0_RDATA0_REG);
    bool efuse_key_read_protected = dis_reg & EFUSE_RD_DIS_BLK2;
    bool efuse_key_write_protected = dis_reg & EFUSE_WR_DIS_BLK2;
    uint32_t efuse_blk2_r0, efuse_blk2_r1, efuse_blk2_r2, efuse_blk2_r3, efuse_blk2_r4, efuse_blk2_r5, efuse_blk2_r6, efuse_blk2_r7;
    efuse_blk2_r0 = REG_READ(EFUSE_BLK2_RDATA0_REG);
    efuse_blk2_r1 = REG_READ(EFUSE_BLK2_RDATA1_REG);
    efuse_blk2_r2 = REG_READ(EFUSE_BLK2_RDATA2_REG);
    efuse_blk2_r3 = REG_READ(EFUSE_BLK2_RDATA3_REG);
    efuse_blk2_r4 = REG_READ(EFUSE_BLK2_RDATA4_REG);
    efuse_blk2_r5 = REG_READ(EFUSE_BLK2_RDATA5_REG);
    efuse_blk2_r6 = REG_READ(EFUSE_BLK2_RDATA6_REG);
    efuse_blk2_r7 = REG_READ(EFUSE_BLK2_RDATA7_REG);

    if (efuse_key_read_protected == true) {
        ESP_LOGE(TAG, "Secure Boot v2 digest(BLK2) read protected, aborting....");
        return ESP_FAIL;
    }

    if (efuse_key_write_protected == false
        && efuse_blk2_r0 == 0 && efuse_blk2_r1 == 0
        && efuse_blk2_r2 == 0 && efuse_blk2_r3 == 0
        && efuse_blk2_r4 == 0 && efuse_blk2_r5 == 0
        && efuse_blk2_r6 == 0 && efuse_blk2_r7 == 0) {
        /* Verifies the signature block appended to the image matches with the signature block of the app to be loaded */
        ret = secure_boot_v2_digest_generate(bootloader_data.start_addr, bootloader_data.image_len - SIG_BLOCK_PADDING, boot_pub_key_digest);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Public key digest generation failed");
            return ret;
        }

        ESP_LOGI(TAG, "Burning public key hash to efuse.");
        uint32_t *boot_public_key_digest_ptr = (uint32_t *) boot_pub_key_digest;
        for (int i = 0; i < 8 ; i++) {
            REG_WRITE(EFUSE_BLK2_WDATA0_REG + 4 * i, boot_public_key_digest_ptr[i]);
            ESP_LOGD(TAG, "EFUSE_BLKx_WDATA%d_REG = 0x%08x", i, boot_public_key_digest_ptr[i]);
        }

    } else {
        uint32_t efuse_blk2_digest[8];
        efuse_blk2_digest[0] = efuse_blk2_r0;
        efuse_blk2_digest[1] = efuse_blk2_r1;
        efuse_blk2_digest[2] = efuse_blk2_r2;
        efuse_blk2_digest[3] = efuse_blk2_r3;
        efuse_blk2_digest[4] = efuse_blk2_r4;
        efuse_blk2_digest[5] = efuse_blk2_r5;
        efuse_blk2_digest[6] = efuse_blk2_r6;
        efuse_blk2_digest[7] = efuse_blk2_r7;
        memcpy(boot_pub_key_digest, efuse_blk2_digest, DIGEST_LEN);
        ESP_LOGW(TAG, "Using pre-loaded secure boot v2 public key digest in EFUSE block 2");
    }

    if (efuse_key_write_protected == false) {
        ESP_LOGI(TAG, "Write protecting public key digest...");
        new_wdata0 |= EFUSE_WR_DIS_BLK2; // delay burning until second half of this function
        efuse_key_write_protected = true;
    }

    uint8_t app_pub_key_digest[DIGEST_LEN];
    ret = secure_boot_v2_digest_generate(image_data->start_addr, image_data->image_len - SIG_BLOCK_PADDING, app_pub_key_digest);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Application signature block is invalid.");
        return ret;
    }

    /* Confirming if the public key in the bootloader's signature block matches with the one in the application's signature block */
    if (memcmp(boot_pub_key_digest, app_pub_key_digest, DIGEST_LEN) != 0) {
        ESP_LOGE(TAG, "Application not signed with a valid private key.");
        return ESP_FAIL;
    }

    if (efuse_key_read_protected) {
        ESP_LOGE(TAG, "Efuse BLK2 (public key digest) is read protected. Refusing to blow secure boot efuse.");
        return ESP_ERR_INVALID_STATE;
    }
    if (!efuse_key_write_protected) {
        ESP_LOGE(TAG, "Efuse BLK2 (public key digest) is not write protected. Refusing to blow secure boot efuse.");
        return ESP_ERR_INVALID_STATE;
    }

    ESP_LOGI(TAG, "blowing secure boot efuse...");
    ESP_LOGD(TAG, "before updating, EFUSE_BLK0_RDATA6 %x", REG_READ(EFUSE_BLK0_RDATA6_REG));

    new_wdata6 |= EFUSE_RD_ABS_DONE_1;

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

#ifndef CONFIG_SECURE_BOOT_V2_ALLOW_EFUSE_RD_DIS
    bool rd_dis_now = true;
#ifdef CONFIG_SECURE_FLASH_ENC_ENABLED
    /* If flash encryption is not enabled yet then don't read-disable efuses yet, do it later in the boot
       when Flash Encryption is being enabled */
    rd_dis_now = esp_flash_encryption_enabled();
#endif
    if (rd_dis_now) {
        ESP_LOGI(TAG, "Prevent read disabling of additional efuses...");
        new_wdata0 |= EFUSE_WR_DIS_RD_DIS;
    }
#else
    ESP_LOGW(TAG, "Allowing read disabling of additional efuses - SECURITY COMPROMISED");
#endif

    REG_WRITE(EFUSE_BLK0_WDATA0_REG, new_wdata0);
    REG_WRITE(EFUSE_BLK0_WDATA6_REG, new_wdata6);
    esp_efuse_burn_new_values();
    uint32_t after = REG_READ(EFUSE_BLK0_RDATA6_REG);
    ESP_LOGD(TAG, "after updating, EFUSE_BLK0_RDATA0 0x%08x EFUSE_BLK0_RDATA6 0x%08x",
             REG_READ(EFUSE_BLK0_RDATA0_REG), after);
    if (after & EFUSE_RD_ABS_DONE_1) {
        ESP_LOGI(TAG, "secure boot v2 is now enabled.");
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, " secure boot v2 not enabled, EFUSE_RD_ABS_DONE_1 is probably write protected!");
        return ESP_ERR_INVALID_STATE;
    }
}

#endif // CONFIG_SECURE_BOOT_V2_ENABLED
