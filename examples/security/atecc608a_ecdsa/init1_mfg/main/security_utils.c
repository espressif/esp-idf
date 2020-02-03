/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <string.h>

#include "esp_log.h"
#include "esp_err.h"
#include "esp_partition.h"
#include "esp_flash_partitions.h"
#include "esp_spi_flash.h"
#include "driver/uart.h"
#include "rom/crc.h"

#include "soc/soc.h"
#include "soc/efuse_reg.h"
#include "esp_efuse.h"
#include "esp_image_format.h"
#include "esp_secure_boot.h"
#include "rom/secure_boot.h"

#ifdef CONFIG_MFG_DEBUGGING
#define ESP_MFG_DEBUG ESP_LOGI
#else
#define ESP_MFG_DEBUG(...)
#endif /* MFG_DEBUG */

#define MFG_DBG "Security Utils"

static esp_err_t esp_image_verify_cust_bootloader_data(esp_image_metadata_t *data, uint32_t bootloader_src_addr)
{
    if (data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    const esp_partition_pos_t bootloader_part = {
        .offset = bootloader_src_addr,
        .size = ESP_PARTITION_TABLE_OFFSET - ESP_BOOTLOADER_OFFSET,
    };
    return esp_image_verify(ESP_IMAGE_VERIFY,
                          &bootloader_part,
                          data);
}

static bool secure_boot_generate(uint32_t image_len, uint32_t boot_start_addr, uint32_t boot_digest_start_addr){
    esp_err_t err;
    esp_secure_boot_iv_digest_t digest;
    const uint32_t *image;
    spi_flash_mmap_handle_t map = 0;

    if (image_len % sizeof(digest.iv) != 0) {
        image_len = (image_len / sizeof(digest.iv) + 1) * sizeof(digest.iv);
    }
    ets_secure_boot_start();
    ets_secure_boot_rd_iv((uint32_t *)digest.iv);
    ets_secure_boot_hash(NULL);
    
    err = spi_flash_erase_range(boot_digest_start_addr, boot_start_addr - boot_digest_start_addr);
    if (err != ESP_OK) {
        ESP_MFG_DEBUG(MFG_DBG, "SPI erase failed: 0x%x", err);
        return false;
    }

    const void *result = NULL;
    uint32_t src_page = boot_start_addr & ~(SPI_FLASH_MMU_PAGE_SIZE-1);
    uint32_t image_size = image_len;

    image_size += (boot_start_addr - src_page);
    err = spi_flash_mmap(src_page, image_size, SPI_FLASH_MMAP_DATA, &result, &map);
    if (err != ESP_OK) {
        ESP_MFG_DEBUG(MFG_DBG, "spi_flash_mmap failed: 0x%x", err);
        return NULL;
    }
    image = (void *)((intptr_t)result + (boot_start_addr - src_page));
    if (!image) {
        ESP_MFG_DEBUG(MFG_DBG, "boot_mmap(0x1000, 0x%x) failed", image_len);
        return false;
    }

    for (int i = 0; i < image_len; i+= sizeof(digest.iv)) {
        ets_secure_boot_hash(&image[i/sizeof(uint32_t)]);
    }
    
    spi_flash_munmap(map);
    map = 0;

    ets_secure_boot_obtain();
    ets_secure_boot_rd_abstract((uint32_t *)digest.digest);
    ets_secure_boot_finish();

    ESP_MFG_DEBUG(MFG_DBG, "write iv+digest to flash start_addr %x", boot_digest_start_addr);
    err = spi_flash_write(boot_digest_start_addr, &digest, sizeof(digest));
    if (err != ESP_OK) {
        ESP_MFG_DEBUG(MFG_DBG, "SPI write failed: 0x%x", err);
        return false;
    }
    return true;
}

esp_err_t esp_cust_secure_boot_enable(void) {
    esp_err_t err;

    uint32_t coding_scheme = REG_GET_FIELD(EFUSE_BLK0_RDATA6_REG, EFUSE_CODING_SCHEME);
    if (coding_scheme != EFUSE_CODING_SCHEME_VAL_NONE && coding_scheme != EFUSE_CODING_SCHEME_VAL_34) {
        ESP_MFG_DEBUG(MFG_DBG, "Unknown/unsupported CODING_SCHEME value 0x%x", coding_scheme);
        return ESP_ERR_NOT_SUPPORTED;
    }

    esp_image_metadata_t cust_bootloader_data = { 0 };
    err = esp_image_verify_cust_bootloader_data(&cust_bootloader_data, 0x11000);
    if (err != ESP_OK) {
        ESP_MFG_DEBUG(MFG_DBG, "cust bootloader image appears invalid! error %d", err);
        err =  ESP_OK;
    }

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
        ESP_MFG_DEBUG(MFG_DBG, "Generating new secure boot key...");
        esp_efuse_write_random_key(EFUSE_BLK2_WDATA0_REG);
        esp_efuse_burn_new_values();
        ESP_MFG_DEBUG(MFG_DBG, "Read & write protecting new key...");
        REG_WRITE(EFUSE_BLK0_WDATA0_REG, EFUSE_WR_DIS_BLK2 | EFUSE_RD_DIS_BLK2);
        esp_efuse_burn_new_values();
        efuse_key_read_protected = true;
        efuse_key_write_protected = true;

    } else {
        ESP_MFG_DEBUG(MFG_DBG, "Using pre-loaded secure boot key in EFUSE block 2");
    }

    ESP_MFG_DEBUG(MFG_DBG, "Generating custom secure boot digest...");
    uint32_t cust_image_len = cust_bootloader_data.image_len;
    if(cust_bootloader_data.image.hash_appended) {
        /* Secure boot digest doesn't cover the hash */
        cust_image_len -= ESP_IMAGE_HASH_LEN;
    }
    if (false == secure_boot_generate(cust_image_len, 0x11000, 0x10000)) {
        ESP_MFG_DEBUG(MFG_DBG, "secure custom boot generation failed");
        return ESP_FAIL;
    }
    ESP_MFG_DEBUG(MFG_DBG, "Digest generation complete.");

    ESP_MFG_DEBUG(MFG_DBG, "blowing secure boot efuse...");
    ESP_MFG_DEBUG(MFG_DBG, "before updating, EFUSE_BLK0_RDATA6 %x", REG_READ(EFUSE_BLK0_RDATA6_REG));

    uint32_t new_wdata6 = EFUSE_RD_ABS_DONE_0;
    ESP_MFG_DEBUG(MFG_DBG, "Disable JTAG...");
    new_wdata6 |= EFUSE_RD_DISABLE_JTAG;

    ESP_MFG_DEBUG(MFG_DBG, "Disable ROM BASIC interpreter fallback...");
    new_wdata6 |= EFUSE_RD_CONSOLE_DEBUG_DISABLE;

    REG_WRITE(EFUSE_BLK0_WDATA6_REG, new_wdata6);
    esp_efuse_burn_new_values();
    uint32_t after = REG_READ(EFUSE_BLK0_RDATA6_REG);
    ESP_MFG_DEBUG(MFG_DBG, "after updating, EFUSE_BLK0_RDATA6 %x", after);
    if (after & EFUSE_RD_ABS_DONE_0) {
        ESP_MFG_DEBUG(MFG_DBG, "secure boot is now enabled for bootloader image");
        return ESP_OK;
    }
    return ESP_ERR_INVALID_STATE;
}

esp_err_t flash_encryption_keygen(void)
{
    uint32_t coding_scheme = REG_GET_FIELD(EFUSE_BLK0_RDATA6_REG, EFUSE_CODING_SCHEME);
    if (coding_scheme != EFUSE_CODING_SCHEME_VAL_NONE && coding_scheme != EFUSE_CODING_SCHEME_VAL_34) {
        ESP_MFG_DEBUG(MFG_DBG, "Unknown/unsupported CODING_SCHEME value 0x%x", coding_scheme);
        return ESP_ERR_NOT_SUPPORTED;
    }

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
        ESP_MFG_DEBUG(MFG_DBG, "Generating new flash encryption key...");
        esp_efuse_write_random_key(EFUSE_BLK1_WDATA0_REG);
        esp_efuse_burn_new_values();

        ESP_MFG_DEBUG(MFG_DBG, "Read & write protecting new key...");
        REG_WRITE(EFUSE_BLK0_WDATA0_REG, EFUSE_WR_DIS_BLK1 | EFUSE_RD_DIS_BLK1);
        esp_efuse_burn_new_values();
    } else {
        if(!(efuse_key_read_protected && efuse_key_write_protected)) {
            ESP_MFG_DEBUG(MFG_DBG, "Flash encryption key has to be either unset or both read and write protected");
            return ESP_ERR_INVALID_STATE;
        }
        ESP_MFG_DEBUG(MFG_DBG, "Using pre-loaded flash encryption key in EFUSE block 1");
    }

    ESP_MFG_DEBUG(MFG_DBG, "Setting CRYPT_CONFIG efuse to 0xF");
    REG_WRITE(EFUSE_BLK0_WDATA5_REG, EFUSE_FLASH_CRYPT_CONFIG_M);
    esp_efuse_burn_new_values();

    uint32_t new_wdata6 = 0;
    ESP_MFG_DEBUG(MFG_DBG, "Disable UART bootloader encryption...");
    new_wdata6 |= EFUSE_DISABLE_DL_ENCRYPT;

    ESP_MFG_DEBUG(MFG_DBG, "Disable UART bootloader decryption...");
    new_wdata6 |= EFUSE_DISABLE_DL_DECRYPT;

    ESP_MFG_DEBUG(MFG_DBG, "Disable UART bootloader MMU cache...");
    new_wdata6 |= EFUSE_DISABLE_DL_CACHE;

    ESP_MFG_DEBUG(MFG_DBG, "Disable JTAG...");
    new_wdata6 |= EFUSE_RD_DISABLE_JTAG;

    ESP_MFG_DEBUG(MFG_DBG, "Disable ROM BASIC interpreter fallback...");
    new_wdata6 |= EFUSE_RD_CONSOLE_DEBUG_DISABLE;

    if (new_wdata6 != 0) {
        REG_WRITE(EFUSE_BLK0_WDATA6_REG, new_wdata6);
        esp_efuse_burn_new_values();
    }

    return ESP_OK;
}
