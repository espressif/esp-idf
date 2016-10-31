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

#include "bootloader_config.h"

static const char* TAG = "secure_boot";

/**
 *  @function :     secure_boot_generate
 *  @description:   generate boot abstract & iv
 *
 *  @inputs:        bool
 */
static bool secure_boot_generate(uint32_t bin_len){
	SpiFlashOpResult spiRet;
	uint16_t i;
	uint32_t buf[32];
	if (bin_len % 128 != 0) {  
		bin_len = (bin_len / 128 + 1) * 128; 
	} 
	ets_secure_boot_start();
	ets_secure_boot_rd_iv(buf);
	ets_secure_boot_hash(NULL);
	Cache_Read_Disable(0);
	/* iv stored in sec 0 */ 
	spiRet = SPIEraseSector(0);
	if (spiRet != SPI_FLASH_RESULT_OK)
	{   
		ESP_LOGE(TAG, SPI_ERROR_LOG);
		return false;
	}
	/* write iv to flash, 0x0000, 128 bytes (1024 bits) */
	spiRet = SPIWrite(0, buf, 128);
	if (spiRet != SPI_FLASH_RESULT_OK) 
	{
		ESP_LOGE(TAG, SPI_ERROR_LOG);
		return false;
	}
	ESP_LOGD(TAG, "write iv to flash.");
	Cache_Read_Enable(0);
	/* read 4K code image from flash, for test */
	for (i = 0; i < bin_len; i+=128) {
		ets_secure_boot_hash((uint32_t *)(0x3f400000 + 0x1000 + i));
	}

	ets_secure_boot_obtain();
	ets_secure_boot_rd_abstract(buf);
	ets_secure_boot_finish();
	Cache_Read_Disable(0);
	/* write abstract to flash, 0x0080, 64 bytes (512 bits) */
	spiRet = SPIWrite(0x80, buf, 64);
	if (spiRet != SPI_FLASH_RESULT_OK) {
		ESP_LOGE(TAG, SPI_ERROR_LOG);
		return false;
	}
	ESP_LOGD(TAG, "write abstract to flash.");
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

/**
 *  @function :     secure_boot_generate_bootloader_digest
 *
 *  @description: Called if the secure boot flag is set on the
 *  bootloader image in flash. If secure boot is not yet enabled for
 *  bootloader, this will generate the secure boot digest and enable
 *  secure boot by blowing the EFUSE_RD_ABS_DONE_0 efuse.
 *
 *  This function does not verify secure boot of the bootloader (the
 *  ROM bootloader does this.)
 *
 *  @return true if secure boot is enabled (either was already enabled,
 *  or is freshly enabled as a result of calling this function.)
 */
bool secure_boot_generate_bootloader_digest(void) {
    uint32_t bin_len = 0;
    if (REG_READ(EFUSE_BLK0_RDATA6_REG) & EFUSE_RD_ABS_DONE_0)
    {
        ESP_LOGI(TAG, "bootloader secure boot is already enabled, continuing..");
        return true;
    }

    boot_cache_redirect( 0, 64*1024);
    bin_len = get_bin_len((uint32_t)MEM_CACHE(0x1000));
    if (bin_len == 0) {
        ESP_LOGE(TAG, "Invalid bootloader image length zero.");
        return false;
    }
    if (bin_len > 0x100000) {
        ESP_LOGE(TAG, "Invalid bootloader image length %x", bin_len);
        return false;
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
    if (false == secure_boot_generate(bin_len)){
        ESP_LOGE(TAG, "secure boot generation failed");
        return false;
    }
    ESP_LOGI(TAG, "Digest generation complete.");

    if (!efuse_key_read_protected) {
        ESP_LOGE(TAG, "Pre-loaded key is not read protected. Refusing to blow secure boot efuse.");
        return false;
    }
    if (!efuse_key_write_protected) {
        ESP_LOGE(TAG, "Pre-loaded key is not write protected. Refusing to blow secure boot efuse.");
        return false;
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
        return true;
    } else {
        ESP_LOGE(TAG, "secure boot not enabled for bootloader image, EFUSE_RD_ABS_DONE_0 is probably write protected!");
        return false;
    }
}
