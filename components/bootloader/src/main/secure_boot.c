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

#include "rom/cache.h"
#include "rom/ets_sys.h"
#include "rom/spi_flash.h"
#include "rom/secure_boot.h"

#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/efuse_reg.h"
#include "soc/rtc_cntl_reg.h"

#include "sdkconfig.h"

#include "bootloader_log.h"
#include "bootloader_config.h"

/**
 *  @function :     secure_boot_generate
 *  @description:   generate boot abstruct & iv 
 *
 *  @inputs:        bool
 */
bool secure_boot_generate(uint32_t bin_len){
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
		log_error(SPI_ERROR_LOG);
		return false;
	}
	/* write iv to flash, 0x0000, 128 bytes (1024 bits) */
	spiRet = SPIWrite(0, buf, 128);
	if (spiRet != SPI_FLASH_RESULT_OK) 
	{
		log_error(SPI_ERROR_LOG);
		return false;
	}
	log_debug("write iv to flash.\n");
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
		log_error(SPI_ERROR_LOG);
		return false;
	}
	log_debug("write abstract to flash.\n");
	Cache_Read_Enable(0);
	return true;
}


/**
 *  @function :     secure_boot
 *  @description:   protect boot code inflash
 *
 *  @inputs:        bool
 */
bool secure_boot(void){ 
	uint32_t bin_len = 0;
	if (REG_READ(EFUSE_BLK0_RDATA6) & EFUSE_RD_ABS_DONE_0)
	{     
		log_info("already secure boot !\n"); 
		return true;
	} else {  
		boot_cache_redirect( 0, 64*1024);
		bin_len = get_bin_len((uint32_t)MEM_CACHE(0x1000));
		if (bin_len == 0) {
			log_error("boot len is error");
			return false;
		}
		if (false == secure_boot_generate(bin_len)){
			log_error("secure boot generate failed");
			return false;
		}  
	}  

	REG_SET_BIT(EFUSE_BLK0_WDATA6, EFUSE_RD_ABS_DONE_0); 
	REG_WRITE(EFUSE_CONF, 0x5A5A);  /* efuse_pgm_op_ena, force no rd/wr disable */     
	REG_WRITE(EFUSE_CMD,  0x02);    /* efuse_pgm_cmd */    
	while (REG_READ(EFUSE_CMD));    /* wait for efuse_pagm_cmd=0 */   
	log_warn("burn abstract_done_0\n");   
	REG_WRITE(EFUSE_CONF, 0x5AA5);  /* efuse_read_op_ena, release force */   
	REG_WRITE(EFUSE_CMD,  0x01);    /* efuse_read_cmd */     
	while (REG_READ(EFUSE_CMD));    /* wait for efuse_read_cmd=0 */       
	log_debug("read EFUSE_BLK0_RDATA6 %x\n", REG_READ(EFUSE_BLK0_RDATA6)); 
	return true;

}
