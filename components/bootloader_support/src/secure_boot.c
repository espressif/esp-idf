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

#include "rom/cache.h"
#include "rom/ets_sys.h"
#include "rom/spi_flash.h"
#include "rom/secure_boot.h"

#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/efuse_reg.h"
#include "soc/rtc_cntl_reg.h"

#include "sdkconfig.h"

#include "bootloader_flash.h"
#include "esp_image_format.h"
#include "esp_secure_boot.h"

static const char* TAG = "secure_boot";

#define HASH_BLOCK_SIZE 128
#define IV_LEN HASH_BLOCK_SIZE
#define DIGEST_LEN 64

/**
 *  @function :     secure_boot_generate
 *  @description:   generate boot digest (aka "abstract") & iv
 *
 *  @inputs:        image_len - length of image to calculate digest for
 */
static bool secure_boot_generate(uint32_t image_len){
    SpiFlashOpResult spiRet;
    /* buffer is uint32_t not uint8_t to meet ROM SPI API signature */
    uint32_t buf[IV_LEN / sizeof(uint32_t)];
    const void *image;

    /* hardware secure boot engine only takes full blocks, so round up the
       image length. The additional data should all be 0xFF.
    */
    if (image_len % HASH_BLOCK_SIZE != 0) {
        image_len = (image_len / HASH_BLOCK_SIZE + 1) * HASH_BLOCK_SIZE;
    }
    ets_secure_boot_start();
    ets_secure_boot_rd_iv(buf);
    ets_secure_boot_hash(NULL);
    Cache_Read_Disable(0);
    /* iv stored in sec 0 */
    spiRet = SPIEraseSector(0);
    if (spiRet != SPI_FLASH_RESULT_OK)
    {
        ESP_LOGE(TAG, "SPI erase failed %d", spiRet);
        return false;
    }
    Cache_Read_Enable(0);

    /* write iv to flash, 0x0000, 128 bytes (1024 bits) */
    ESP_LOGD(TAG, "write iv to flash.");
    spiRet = SPIWrite(0, buf, IV_LEN);
    if (spiRet != SPI_FLASH_RESULT_OK)
    {
        ESP_LOGE(TAG, "SPI write failed %d", spiRet);
        return false;
    }
    bzero(buf, sizeof(buf));

    /* generate digest from image contents */
    image = bootloader_mmap(0x1000, image_len);
    if (!image) {
        ESP_LOGE(TAG, "bootloader_mmap(0x1000, 0x%x) failed", image_len);
        return false;
    }
    for (int i = 0; i < image_len; i+= HASH_BLOCK_SIZE) {
        ets_secure_boot_hash(image + i/sizeof(void *));
    }
    bootloader_unmap(image);

    ets_secure_boot_obtain();
    ets_secure_boot_rd_abstract(buf);
    ets_secure_boot_finish();

    ESP_LOGD(TAG, "write digest to flash.");
    spiRet = SPIWrite(0x80, buf, DIGEST_LEN);
    if (spiRet != SPI_FLASH_RESULT_OK) {
        ESP_LOGE(TAG, "SPI write failed %d", spiRet);
        return false;
    }
    ESP_LOGD(TAG, "write digest to flash.");
    Cache_Read_Enable(0);
    return true;
}

/* Burn values written to the efuse write registers */
static inline void burn_efuses()
{
    REG_WRITE(EFUSE_CONF_REG, 0x5A5A);  /* efuse_pgm_op_ena, force no rd/wr disable */
    REG_WRITE(EFUSE_CMD_REG,  0x02);    /* efuse_pgm_cmd */
    while (REG_READ(EFUSE_CMD_REG));    /* wait for efuse_pagm_cmd=0 */
    REG_WRITE(EFUSE_CONF_REG, 0x5AA5);  /* efuse_read_op_ena, release force */
    REG_WRITE(EFUSE_CMD_REG,  0x01);    /* efuse_read_cmd */
    while (REG_READ(EFUSE_CMD_REG));    /* wait for efuse_read_cmd=0 */
}

esp_err_t esp_secure_boot_permanently_enable(void) {
    esp_err_t err;
    uint32_t image_len = 0;
    if (esp_secure_boot_enabled())
    {
        ESP_LOGI(TAG, "bootloader secure boot is already enabled, continuing..");
        return ESP_OK;
    }

    err = esp_image_basic_verify(0x1000, &image_len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "bootloader image appears invalid! error %d", err);
        return err;
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
        ESP_LOGI(TAG, "Generating new secure boot key...");
        /* reuse the secure boot IV generation function to generate
           the key, as this generator uses the hardware RNG. */
        uint32_t buf[32];
        ets_secure_boot_start();
        ets_secure_boot_rd_iv(buf);
        ets_secure_boot_finish();
        for (int i = 0; i < 8; i++) {
            ESP_LOGV(TAG, "EFUSE_BLK2_WDATA%d_REG = 0x%08x", i, buf[i]);
            REG_WRITE(EFUSE_BLK2_WDATA0_REG + 4*i, buf[i]);
        }
        bzero(buf, sizeof(buf));
        burn_efuses();
        ESP_LOGI(TAG, "Read & write protecting new key...");
        REG_WRITE(EFUSE_BLK0_WDATA0_REG, EFUSE_WR_DIS_BLK2 | EFUSE_RD_DIS_BLK2);
        burn_efuses();
        efuse_key_read_protected = true;
        efuse_key_write_protected = true;

    } else {
        ESP_LOGW(TAG, "Using pre-loaded secure boot key in EFUSE block 2");
    }

    ESP_LOGI(TAG, "Generating secure boot digest...");
    if (false == secure_boot_generate(image_len)){
        ESP_LOGE(TAG, "secure boot generation failed");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Digest generation complete.");

    if (!efuse_key_read_protected) {
        ESP_LOGE(TAG, "Pre-loaded key is not read protected. Refusing to blow secure boot efuse.");
        return ESP_ERR_INVALID_STATE;
    }
    if (!efuse_key_write_protected) {
        ESP_LOGE(TAG, "Pre-loaded key is not write protected. Refusing to blow secure boot efuse.");
        return ESP_ERR_INVALID_STATE;
    }

    ESP_LOGI(TAG, "blowing secure boot efuse & disabling JTAG...");
    ESP_LOGD(TAG, "before updating, EFUSE_BLK0_RDATA6 %x", REG_READ(EFUSE_BLK0_RDATA6_REG));
    REG_WRITE(EFUSE_BLK0_WDATA6_REG,
                EFUSE_RD_ABS_DONE_0 | EFUSE_RD_DISABLE_JTAG);
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
