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

#include "esp_types.h"
#include "esp_attr.h"

#include "rom/cache.h"
#include "rom/ets_sys.h"
#include "rom/spi_flash.h"

#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/efuse_reg.h"
#include "soc/rtc_cntl_reg.h"

#include "sdkconfig.h"

#include "bootloader_log.h"
#include "bootloader_config.h"

/**
 *  @function :     bitcount
 *  @description:   caculate bit 1 in flash_crypt_cnt 
 *                  if it's even number ,need encrypt flash data,and burn efuse 
 *
 *  @inputs:        n     flash_crypt_cnt               
 *  @return:        number of 1 in flash_crypt_cnt
 *                  
 */
int bitcount(int n){
   int count = 0;  
   while (n > 0) {   
       count += n & 1;  
       n >>= 1;  
   }   
   return count;

}
/**
 *  @function :     flash_encrypt_write
 *  @description:   write encrypted data in flash
 *
 *  @inputs:        pos     address in flash
 *                  len    size of data need encrypt                
 *  @return:        return true, if the write flash success
 *                  
 */
bool flash_encrypt_write(uint32_t pos, uint32_t len)
{
   SpiFlashOpResult spiRet;    
   uint32_t buf[1024];
   int i = 0;
   Cache_Read_Disable(0);  
   for (i = 0;i<((len-1)/0x1000 + 1);i++) {
       spiRet = SPIRead(pos, buf, SPI_SEC_SIZE);     
       if (spiRet != SPI_FLASH_RESULT_OK) {   
           Cache_Read_Enable(0); 
           log_error(SPI_ERROR_LOG);
           return false;    
       }    
       spiRet = SPIEraseSector(pos/SPI_SEC_SIZE);    
       if (spiRet != SPI_FLASH_RESULT_OK) { 
           Cache_Read_Enable(0);
           log_error(SPI_ERROR_LOG);
           return false;      
       }
       spiRet = SPI_Encrypt_Write(pos, buf, SPI_SEC_SIZE);
       if (spiRet != SPI_FLASH_RESULT_OK) {    
           Cache_Read_Enable(0); 
           log_error(SPI_ERROR_LOG);
           return false;       
       }   
       pos += SPI_SEC_SIZE;
   }
   Cache_Read_Enable(0); 
   return true;
}
/**
 *  @function :     flash_encrypt
 *  @description:   encrypt 2nd boot ,partition table ,factory bin ��test bin (if use)��ota bin
 *                  ��OTA info sector.
 *
 *  @inputs:        bs     bootloader state structure used to save the data
 *                 
 *  @return:        return true, if the encrypt flash success
 *                  
 */
bool flash_encrypt(bootloader_state_t *bs)
{
   uint32_t bin_len = 0;
   uint32_t flash_crypt_cnt = REG_GET_FIELD(EFUSE_BLK0_RDATA0, EFUSE_FLASH_CRYPT_CNT);
   uint8_t count = bitcount(flash_crypt_cnt);
   int i = 0;
   log_debug("flash crypt cnt %x, count %d\n", flash_crypt_cnt, count); 

   if ((count % 2) == 0) {    
       boot_cache_redirect( 0, 64*1024);
        /* encrypt iv and abstruct */ 
       if (false == flash_encrypt_write(0,SPI_SEC_SIZE)) {
           log_error("encrypt iv and abstruct error"); 
           return false;
       }

        /* encrypt write boot bin*/
       bin_len = get_bin_len((uint32_t)MEM_CACHE(0x1000));
       if(bin_len != 0) {
           if (false == flash_encrypt_write(0x1000,bin_len)) {
               log_error("encrypt 2nd boot error"); 
               return false;
           }
       } else {
           log_error("2nd boot len error"); 
           return false;
       }
        /* encrypt partition table */
       if (false ==  flash_encrypt_write(PARTITION_ADD,SPI_SEC_SIZE)) {
           log_error("encrypt partition table error"); 
           return false;
       }

        /* encrypt write factory bin  */
       if(bs->factory.offset != 0x00) {
           log_debug("have factory bin\n");
           boot_cache_redirect(bs->factory.offset,bs->factory.size);
           bin_len = get_bin_len((uint32_t)MEM_CACHE(bs->factory.offset&0xffff));
           if(bin_len != 0) {           
               if (false ==  flash_encrypt_write(bs->factory.offset,bin_len)) {
                   log_error("encrypt factory bin error"); 
                   return false;
               }
           }
       }
        /* encrypt write test bin  */
       if(bs->test.offset != 0x00) {
           ets_printf("have test bin\n");
           boot_cache_redirect(bs->test.offset,bs->test.size);
           bin_len = get_bin_len((uint32_t)MEM_CACHE(bs->test.offset&0xffff));
           if(bin_len != 0) {
               if (false ==  flash_encrypt_write(bs->test.offset,bin_len)) {
                   log_error("encrypt test bin error"); 
                   return false;
               }
           }
       }
        /* encrypt write ota bin  */
       for (i = 0;i<16;i++) {
           if(bs->ota[i].offset != 0x00) {
               log_debug("have ota[%d] bin\n",i);
               boot_cache_redirect(bs->ota[i].offset,bs->ota[i].size);
               bin_len = get_bin_len((uint32_t)MEM_CACHE(bs->ota[i].offset&0xffff));
               if(bin_len != 0) {    
                   if (false == flash_encrypt_write(bs->ota[i].offset,bin_len)) {
                       log_error("encrypt ota bin error"); 
                       return false;
                   }
               }
           }
       }
        /* encrypt write ota info bin  */
       if (false == flash_encrypt_write(bs->ota_info.offset,2*SPI_SEC_SIZE)) {
           log_error("encrypt ota binfo error"); 
           return false;
       }  
       REG_SET_FIELD(EFUSE_BLK0_WDATA0, EFUSE_FLASH_CRYPT_CNT, 0x04);   
       REG_WRITE(EFUSE_CONF, 0x5A5A);  /* efuse_pgm_op_ena, force no rd/wr disable */     
       REG_WRITE(EFUSE_CMD,  0x02);    /* efuse_pgm_cmd */     
       while (REG_READ(EFUSE_CMD));    /* wait for efuse_pagm_cmd=0 */
       log_warn("burn  flash_crypt_cnt\n");   
       REG_WRITE(EFUSE_CONF, 0x5AA5);  /* efuse_read_op_ena, release force */   
       REG_WRITE(EFUSE_CMD,  0x01);    /* efuse_read_cmd */     
       while (REG_READ(EFUSE_CMD));    /* wait for efuse_read_cmd=0 */  
       return true;
   } else {   
       log_info("flash already encrypted.\n"); 
       return true;
   }
}
