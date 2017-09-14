/*
Abstraction layer for spi-ram. For now, it's no more than a stub for the spiram_psram functions, but if 
we add more types of external RAM memory, this can be made into a more intelligent dispatcher.
*/

// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdint.h>
#include <string.h>

#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "spiram_psram.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/xtensa_api.h"
#include "soc/soc.h"
#include "soc/dport_reg.h"
#include "rom/cache.h"

#if CONFIG_FREERTOS_UNICORE
#define PSRAM_MODE PSRAM_VADDR_MODE_NORMAL
#else
#if CONFIG_MEMMAP_SPIRAM_CACHE_EVENODD
#define PSRAM_MODE PSRAM_VADDR_MODE_EVENODD
#else
#define PSRAM_MODE PSRAM_VADDR_MODE_LOWHIGH
#endif
#endif

#if CONFIG_SPIRAM_SUPPORT

static const char* TAG = "spiram";

#if CONFIG_SPIRAM_SPEED_40M && CONFIG_ESPTOOLPY_FLASHFREQ_40M
#define PSRAM_SPEED PSRAM_CACHE_F40M_S40M
#elif CONFIG_SPIRAM_SPEED_40M && CONFIG_ESPTOOLPY_FLASHFREQ_80M
#define PSRAM_SPEED PSRAM_CACHE_F80M_S40M
#elif CONFIG_SPIRAM_SPEED_80M && CONFIG_ESPTOOLPY_FLASHFREQ_80M
#define PSRAM_SPEED PSRAM_CACHE_F80M_S80M
#else
#error "FLASH speed can only be equal to or higher than SRAM speed while SRAM is enabled!"
#endif


static bool spiram_inited=false;


/*
 Simple RAM test. Writes a word every 32 bytes. Takes about a second to complete for 4MiB. Returns
 true when RAM seems OK, false when test fails. WARNING: Do not run this before the 2nd cpu has been
 initialized (in a two-core system) or after the heap allocator has taken ownership of the memory.
*/
bool esp_spiram_test()
{
    volatile int *spiram=(volatile int*)SOC_EXTRAM_DATA_LOW;
    size_t p;
    size_t s=CONFIG_SPIRAM_SIZE;
    int errct=0;
    int initial_err=-1;
    for (p=0; p<(s/sizeof(int)); p+=8) {
        spiram[p]=p^0xAAAAAAAA;
    }
    for (p=0; p<(s/sizeof(int)); p+=8) {
        if (spiram[p]!=(p^0xAAAAAAAA)) {
            errct++;
            if (errct==1) initial_err=p*4;
        }
    }
    if (errct) {
        ESP_EARLY_LOGE(TAG, "SPI SRAM memory test fail. %d/%d writes failed, first @ %X\n", errct, s/32, initial_err+SOC_EXTRAM_DATA_LOW);
        return false;
    } else {
        ESP_EARLY_LOGI(TAG, "SPI SRAM memory test OK");
        return true;
    }
}



esp_err_t esp_spiram_init()
{
    //Enable external RAM in MMU
    cache_sram_mmu_set( 0, 0, SOC_EXTRAM_DATA_LOW, 0, 32, 128 );
    //Flush and enable icache for APP CPU
#if !CONFIG_FREERTOS_UNICORE
    DPORT_CLEAR_PERI_REG_MASK(DPORT_APP_CACHE_CTRL1_REG, DPORT_APP_CACHE_MASK_DRAM1);
    cache_sram_mmu_set( 1, 0, SOC_EXTRAM_DATA_LOW, 0, 32, 128 );
#endif

    esp_err_t r;
    r = psram_enable(PSRAM_SPEED, PSRAM_MODE);
    if (r != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "SPI RAM enabled but initialization failed. Bailing out.");
        return r;
    }

    ESP_EARLY_LOGI(TAG, "SPI RAM mode: %s", PSRAM_SPEED == PSRAM_CACHE_F40M_S40M ? "flash 40m sram 40m" : \
                                          PSRAM_SPEED == PSRAM_CACHE_F80M_S40M ? "flash 80m sram 40m" : \
                                          PSRAM_SPEED == PSRAM_CACHE_F80M_S80M ? "flash 80m sram 80m" : "ERROR");
    ESP_EARLY_LOGI(TAG, "PSRAM initialized, cache is in %s mode.", \
                                          (PSRAM_MODE==PSRAM_VADDR_MODE_EVENODD)?"even/odd (2-core)": \
                                          (PSRAM_MODE==PSRAM_VADDR_MODE_LOWHIGH)?"low/high (2-core)": \
                                          (PSRAM_MODE==PSRAM_VADDR_MODE_NORMAL)?"normal (1-core)":"ERROR");
    spiram_inited=true;
    return ESP_OK;
}



size_t esp_spiram_get_size()
{
    return CONFIG_SPIRAM_SIZE;
}

/*
 Before flushing the cache, if psram is enabled as a memory-mapped thing, we need to write back the data in the cache to the psram first,
 otherwise it will get lost. For now, we just read 64/128K of random PSRAM memory to do this.
*/
void IRAM_ATTR esp_spiram_writeback_cache() 
{
    int x;
    volatile int i=0;
    volatile uint8_t *psram=(volatile uint8_t*)SOC_EXTRAM_DATA_LOW;
    int cache_was_disabled=0;

    if (!spiram_inited) return;

    //We need cache enabled for this to work. Re-enable it if needed; make sure we 
    //disable it again on exit as well.
    if (DPORT_REG_GET_BIT(DPORT_PRO_CACHE_CTRL_REG, DPORT_PRO_CACHE_ENABLE)==0) {
        cache_was_disabled|=(1<<0);
        DPORT_SET_PERI_REG_BITS(DPORT_PRO_CACHE_CTRL_REG, 1, 1, DPORT_PRO_CACHE_ENABLE_S);
    }
#ifndef CONFIG_FREERTOS_UNICORE
    if (DPORT_REG_GET_BIT(DPORT_APP_CACHE_CTRL_REG, DPORT_APP_CACHE_ENABLE)==0) {
        cache_was_disabled|=(1<<1);
        DPORT_SET_PERI_REG_BITS(DPORT_APP_CACHE_CTRL_REG, 1, 1, DPORT_APP_CACHE_ENABLE_S);
    }
#endif

#if CONFIG_FREERTOS_UNICORE
    for (x=0; x<1024*64; x+=32) {
        i+=psram[x];
    }
#else
    /*
    Note: this assumes the amount of external RAM is >2M. If it is 2M or less, what this code does is undefined. If 
    we ever support external RAM chips of 2M or smaller, this may need adjusting.
    */
    for (x=0; x<1024*64; x+=32) {
        i+=psram[x];
        i+=psram[x+(1024*1024*2)+(1024*64)]; //address picked to also clear cache of app cpu in low/high mode
    }
#endif

    if (cache_was_disabled&(1<<0)) {
        while (DPORT_GET_PERI_REG_BITS2(DPORT_PRO_DCACHE_DBUG0_REG, DPORT_PRO_CACHE_STATE, DPORT_PRO_CACHE_STATE_S) != 1) ;
        DPORT_SET_PERI_REG_BITS(DPORT_PRO_CACHE_CTRL_REG, 1, 0, DPORT_PRO_CACHE_ENABLE_S);
    }
#ifndef CONFIG_FREERTOS_UNICORE
    if (cache_was_disabled&(1<<1)) {
        while (DPORT_GET_PERI_REG_BITS2(DPORT_APP_DCACHE_DBUG0_REG, DPORT_APP_CACHE_STATE, DPORT_APP_CACHE_STATE_S) != 1);
        DPORT_SET_PERI_REG_BITS(DPORT_APP_CACHE_CTRL_REG, 1, 0, DPORT_APP_CACHE_ENABLE_S);
    }
#endif
}



#endif