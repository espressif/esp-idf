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
#include <sys/param.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp32s2beta/spiram.h"
#include "spiram_psram.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/xtensa_api.h"
#include "soc/soc.h"
#include "esp_heap_caps_init.h"
#include "soc/soc_memory_layout.h"
#include "soc/dport_reg.h"
#include "esp32s2beta/rom/cache.h"

#if CONFIG_FREERTOS_UNICORE
#define PSRAM_MODE PSRAM_VADDR_MODE_NORMAL
#else
#warning "spiram.c: TODO: no even/odd mode for ESP32S2 PSRAM?"
#if 0
#define PSRAM_MODE PSRAM_VADDR_MODE_EVENODD
#else
#define PSRAM_MODE PSRAM_VADDR_MODE_LOWHIGH
#endif
#endif

#if CONFIG_SPIRAM

static const char* TAG = "spiram";

#if CONFIG_SPIRAM_SPEED_40M
#define PSRAM_SPEED PSRAM_CACHE_S40M
#elif CONFIG_SPIRAM_SPEED_80M
#define PSRAM_SPEED PSRAM_CACHE_S80M
#else
#define PSRAM_SPEED PSRAM_CACHE_S20M
#endif


static bool spiram_inited=false;


/*
 Simple RAM test. Writes a word every 32 bytes. Takes about a second to complete for 4MiB. Returns
 true when RAM seems OK, false when test fails. WARNING: Do not run this before the 2nd cpu has been
 initialized (in a two-core system) or after the heap allocator has taken ownership of the memory.
*/
bool esp_spiram_test(void)
{
    volatile int *spiram=(volatile int*)(SOC_EXTRAM_DATA_HIGH - CONFIG_SPIRAM_SIZE);
    size_t p;
    size_t s=CONFIG_SPIRAM_SIZE;
    int errct=0;
    int initial_err=-1;

    if ((SOC_EXTRAM_DATA_HIGH - SOC_EXTRAM_DATA_LOW) < CONFIG_SPIRAM_SIZE) {
        ESP_EARLY_LOGW(TAG, "Only test spiram from %08x to %08x\n", SOC_EXTRAM_DATA_LOW, SOC_EXTRAM_DATA_HIGH);
        spiram=(volatile int*)SOC_EXTRAM_DATA_LOW;
        s = SOC_EXTRAM_DATA_HIGH - SOC_EXTRAM_DATA_LOW;
    }
    for (p=0; p<(s/sizeof(int)); p+=8) {
        spiram[p]=p^0xAAAAAAAA;
    }
    for (p=0; p<(s/sizeof(int)); p+=8) {
        if (spiram[p]!=(p^0xAAAAAAAA)) {
            errct++;
            if (errct==1) initial_err=p*4;
            if (errct < 4) {
                ESP_EARLY_LOGE(TAG, "SPI SRAM error@%08x:%08x/%08x \n", &spiram[p], spiram[p], p^0xAAAAAAAA);
            }
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

#define DRAM0_ONLY_CACHE_SIZE                   BUS_IRAM0_CACHE_SIZE
#define DRAM0_DRAM1_CACHE_SIZE	                (BUS_IRAM0_CACHE_SIZE + BUS_IRAM1_CACHE_SIZE)
#define DRAM0_DRAM1_DPORT_CACHE_SIZE            (BUS_IRAM0_CACHE_SIZE + BUS_IRAM1_CACHE_SIZE + BUS_DPORT_CACHE_SIZE)
#define DBUS3_ONLY_CACHE_SIZE                   BUS_AHB_DBUS3_CACHE_SIZE
#define DRAM0_DRAM1_DPORT_DBUS3_CACHE_SIZE      (DRAM0_DRAM1_DPORT_CACHE_SIZE + DBUS3_ONLY_CACHE_SIZE)

#define SPIRAM_SIZE_EXC_DRAM0_DRAM1_DPORT      (CONFIG_SPIRAM_SIZE - DRAM0_DRAM1_DPORT_CACHE_SIZE)
#define SPIRAM_SIZE_EXC_DATA_CACHE             (CONFIG_SPIRAM_SIZE - DRAM0_DRAM1_DPORT_DBUS3_CACHE_SIZE)

#define SPIRAM_SMALL_SIZE_MAP_VADDR             (DRAM0_CACHE_ADDRESS_HIGH - CONFIG_SPIRAM_SIZE)
#define SPIRAM_SMALL_SIZE_MAP_PADDR             0
#define SPIRAM_SMALL_SIZE_MAP_SIZE              CONFIG_SPIRAM_SIZE

#define SPIRAM_MID_SIZE_MAP_VADDR               (AHB_DBUS3_ADDRESS_HIGH - SPIRAM_SIZE_EXC_DRAM0_DRAM1_DPORT)
#define SPIRAM_MID_SIZE_MAP_PADDR               0
#define SPIRAM_MID_SIZE_MAP_SIZE                (SPIRAM_SIZE_EXC_DRAM0_DRAM1_DPORT)

#define SPIRAM_BIG_SIZE_MAP_VADDR               AHB_DBUS3_ADDRESS_LOW
#define SPIRAM_BIG_SIZE_MAP_PADDR               (AHB_DBUS3_ADDRESS_HIGH - DRAM0_DRAM1_DPORT_DBUS3_CACHE_SIZE)
#define SPIRAM_BIG_SIZE_MAP_SIZE                DBUS3_ONLY_CACHE_SIZE

#define SPIRAM_MID_BIG_SIZE_MAP_VADDR           DPORT_CACHE_ADDRESS_LOW
#define SPIRAM_MID_BIG_SIZE_MAP_PADDR           SPIRAM_SIZE_EXC_DRAM0_DRAM1_DPORT
#define SPIRAM_MID_BIG_SIZE_MAP_SIZE            DRAM0_DRAM1_DPORT_DBUS3_CACHE_SIZE


void IRAM_ATTR esp_spiram_init_cache(void)
{
    Cache_Suspend_DCache();
    /* map the address from SPIRAM end to the start, map the address in order: DRAM1, DRAM1, DPORT, DBUS3 */
#if CONFIG_SPIRAM_SIZE <= DRAM0_ONLY_CACHE_SIZE
    /* cache size <= 3MB + 576 KB, only map DRAM0 bus */
    Cache_Dbus_MMU_Set(DPORT_MMU_ACCESS_SPIRAM, SPIRAM_SMALL_SIZE_MAP_VADDR, SPIRAM_SMALL_SIZE_MAP_PADDR, 64, SPIRAM_SMALL_SIZE_MAP_SIZE >> 16, 0);
    REG_SET_BIT(DPORT_CACHE_SOURCE_1_REG, DPORT_PRO_CACHE_D_SOURCE_PRO_DRAM0);
    REG_CLR_BIT(DPORT_PRO_DCACHE_CTRL1_REG, DPORT_PRO_DCACHE_MASK_DRAM0);
#elif CONFIG_SPIRAM_SIZE <= DRAM0_DRAM1_CACHE_SIZE
    /* cache size <= 7MB + 576KB, only map DRAM0 and DRAM1 bus */
    Cache_Dbus_MMU_Set(DPORT_MMU_ACCESS_SPIRAM, SPIRAM_SMALL_SIZE_MAP_VADDR, SPIRAM_SMALL_SIZE_MAP_PADDR, 64, SPIRAM_SMALL_SIZE_MAP_SIZE >> 16, 0);
    REG_SET_BIT(DPORT_CACHE_SOURCE_1_REG, DPORT_PRO_CACHE_D_SOURCE_PRO_DRAM0);
    REG_CLR_BIT(DPORT_PRO_DCACHE_CTRL1_REG, DPORT_PRO_DCACHE_MASK_DRAM1 | DPORT_PRO_DCACHE_MASK_DRAM0);
#elif CONFIG_SPIRAM_SIZE <= DRAM0_DRAM1_DPORT_CACHE_SIZE
    /* cache size <= 10MB + 576KB, map DRAM0, DRAM1, DPORT bus */
    Cache_Dbus_MMU_Set(DPORT_MMU_ACCESS_SPIRAM, SPIRAM_SMALL_SIZE_MAP_VADDR, SPIRAM_SMALL_SIZE_MAP_PADDR, 64, SPIRAM_SMALL_SIZE_MAP_SIZE >> 16, 0);
    REG_SET_BIT(DPORT_CACHE_SOURCE_1_REG, DPORT_PRO_CACHE_D_SOURCE_PRO_DPORT | DPORT_PRO_CACHE_D_SOURCE_PRO_DRAM0);
    REG_CLR_BIT(DPORT_PRO_DCACHE_CTRL1_REG, DPORT_PRO_DCACHE_MASK_DRAM1 | DPORT_PRO_DCACHE_MASK_DRAM0 | DPORT_PRO_DCACHE_MASK_DPORT);
#else
#if CONFIG_SPIRAM_USE_AHB_DBUS3
#if CONFIG_SPIRAM_SIZE <= DRAM0_DRAM1_DPORT_DBUS3_CACHE_SIZE
    /* cache size <= 14MB + 576KB, map DRAM0, DRAM1, DPORT bus, as well as data bus3 */
    Cache_Dbus_MMU_Set(DPORT_MMU_ACCESS_SPIRAM, SPIRAM_MID_SIZE_MAP_VADDR, SPIRAM_MID_SIZE_MAP_PADDR, 64, SPIRAM_MID_SIZE_MAP_SIZE >> 16, 0);
#else
    /* cache size > 14MB + 576KB, map DRAM0, DRAM1, DPORT bus, as well as data bus3 */
    Cache_Dbus_MMU_Set(DPORT_MMU_ACCESS_SPIRAM, SPIRAM_BIG_SIZE_MAP_VADDR, SPIRAM_BIG_SIZE_MAP_PADDR, 64, SPIRAM_BIG_SIZE_MAP_SIZE >> 16, 0);
#endif
    Cache_Dbus_MMU_Set(DPORT_MMU_ACCESS_SPIRAM, SPIRAM_MID_BIG_SIZE_MAP_VADDR, SPIRAM_MID_BIG_SIZE_MAP_PADDR, 64, SPIRAM_MID_BIG_SIZE_MAP_SIZE >> 16, 0);
    REG_SET_BIT(DPORT_CACHE_SOURCE_1_REG, DPORT_PRO_CACHE_D_SOURCE_PRO_DPORT | DPORT_PRO_CACHE_D_SOURCE_PRO_DRAM0);
    REG_CLR_BIT(DPORT_CACHE_SOURCE_1_REG, DPORT_PRO_CACHE_D_SOURCE_PRO_DROM0);
    REG_CLR_BIT(DPORT_PRO_DCACHE_CTRL1_REG, DPORT_PRO_DCACHE_MASK_DRAM1 | DPORT_PRO_DCACHE_MASK_DRAM0 | DPORT_PRO_DCACHE_MASK_DPORT | DPORT_PRO_DCACHE_MASK_BUS3);
#else
    /* cache size > 10MB + 576KB, map DRAM0, DRAM1, DPORT bus , only remap 0x3f500000 ~ 0x3ff90000*/
    Cache_Dbus_MMU_Set(DPORT_MMU_ACCESS_SPIRAM, SPIRAM_MID_BIG_SIZE_MAP_VADDR, SPIRAM_MID_BIG_SIZE_MAP_PADDR, 64, SPIRAM_MID_BIG_SIZE_MAP_SIZE >> 16, 0);
    REG_SET_BIT(DPORT_CACHE_SOURCE_1_REG, DPORT_PRO_CACHE_D_SOURCE_PRO_DPORT | DPORT_PRO_CACHE_D_SOURCE_PRO_DRAM0);
    REG_CLR_BIT(DPORT_PRO_DCACHE_CTRL1_REG, DPORT_PRO_DCACHE_MASK_DRAM1 | DPORT_PRO_DCACHE_MASK_DRAM0 | DPORT_PRO_DCACHE_MASK_DPORT);
#endif
#endif
}

static uint32_t pages_for_flash = 0;
static uint32_t page0_mapped = 0;
static uint32_t page0_page = 0xffff;
static uint32_t instrcution_in_spiram = 0;
static uint32_t rodata_in_spiram = 0;

uint32_t esp_spiram_instruction_access_enabled(void)
{
    return instrcution_in_spiram;
}

uint32_t esp_spiram_rodata_access_enabled(void)
{
    return rodata_in_spiram;
}

esp_err_t esp_spiram_enable_instruction_access(void)
{
    uint32_t pages_in_flash = 0;
    pages_in_flash += Cache_Count_Flash_Pages(PRO_CACHE_IBUS0, &page0_mapped);
    pages_in_flash += Cache_Count_Flash_Pages(PRO_CACHE_IBUS1, &page0_mapped);
    pages_in_flash += Cache_Count_Flash_Pages(PRO_CACHE_IBUS2, &page0_mapped);
    if ((pages_in_flash + pages_for_flash) > (CONFIG_SPIRAM_SIZE >> 16)) {
        ESP_EARLY_LOGE(TAG, "SPI RAM space not enough for the instructions, has %d pages, need %d pages.", (CONFIG_SPIRAM_SIZE >> 16), (pages_in_flash + pages_for_flash));
        return ESP_FAIL;
    }
    ESP_EARLY_LOGI(TAG, "Instructions copied and mapped to SPIRAM");
    pages_for_flash = Cache_Flash_To_SPIRAM_Copy(PRO_CACHE_IBUS0, IRAM0_ADDRESS_LOW, pages_for_flash, &page0_page);
    pages_for_flash = Cache_Flash_To_SPIRAM_Copy(PRO_CACHE_IBUS1, IRAM1_ADDRESS_LOW, pages_for_flash, &page0_page);
    pages_for_flash = Cache_Flash_To_SPIRAM_Copy(PRO_CACHE_IBUS2, IROM0_ADDRESS_LOW, pages_for_flash, &page0_page);
    instrcution_in_spiram = 1;
    return ESP_OK;
}

esp_err_t esp_spiram_enable_rodata_access(void)
{
    uint32_t pages_in_flash = 0;
    if (Cache_Drom0_Using_ICache()) {
        pages_in_flash += Cache_Count_Flash_Pages(PRO_CACHE_IBUS3, &page0_mapped);
    } else {
        pages_in_flash += Cache_Count_Flash_Pages(PRO_CACHE_DBUS3, &page0_mapped);
    }
    pages_in_flash += Cache_Count_Flash_Pages(PRO_CACHE_DBUS0, &page0_mapped);
    pages_in_flash += Cache_Count_Flash_Pages(PRO_CACHE_DBUS1, &page0_mapped);
    pages_in_flash += Cache_Count_Flash_Pages(PRO_CACHE_DBUS2, &page0_mapped);

    if ((pages_in_flash + pages_for_flash) > (CONFIG_SPIRAM_SIZE >> 16)) {
        ESP_EARLY_LOGE(TAG, "SPI RAM space not enough for the read only data.");
        return ESP_FAIL;
    }

    ESP_EARLY_LOGI(TAG, "Read only data copied and mapped to SPIRAM");
    if (Cache_Drom0_Using_ICache()) {
        pages_for_flash = Cache_Flash_To_SPIRAM_Copy(PRO_CACHE_IBUS3, DROM0_ADDRESS_LOW, pages_for_flash, &page0_page);
    } else {
        pages_for_flash = Cache_Flash_To_SPIRAM_Copy(PRO_CACHE_DBUS3, DROM0_ADDRESS_LOW, pages_for_flash, &page0_page);
    }
    pages_for_flash = Cache_Flash_To_SPIRAM_Copy(PRO_CACHE_DBUS0, DRAM0_ADDRESS_LOW, pages_for_flash, &page0_page);
    pages_for_flash = Cache_Flash_To_SPIRAM_Copy(PRO_CACHE_DBUS1, DRAM1_ADDRESS_LOW, pages_for_flash, &page0_page);
    pages_for_flash = Cache_Flash_To_SPIRAM_Copy(PRO_CACHE_DBUS2, DPORT_ADDRESS_LOW, pages_for_flash, &page0_page);
    rodata_in_spiram = 1;
    return ESP_OK;
}

esp_err_t esp_spiram_init(void)
{
    esp_err_t r;
    r = psram_enable(PSRAM_SPEED, PSRAM_MODE);
    if (r != ESP_OK) {
#if CONFIG_SPIRAM_IGNORE_NOTFOUND
        ESP_EARLY_LOGE(TAG, "SPI RAM enabled but initialization failed. Bailing out.");
#endif
        return r;
    }

    ESP_EARLY_LOGI(TAG, "SPI RAM mode: %s", PSRAM_SPEED == PSRAM_CACHE_S40M ? "sram 40m" : \
                                          PSRAM_SPEED == PSRAM_CACHE_S80M ? "sram 80m" : "sram 20m");
    ESP_EARLY_LOGI(TAG, "PSRAM initialized, cache is in %s mode.", \
                                          (PSRAM_MODE==PSRAM_VADDR_MODE_EVENODD)?"even/odd (2-core)": \
                                          (PSRAM_MODE==PSRAM_VADDR_MODE_LOWHIGH)?"low/high (2-core)": \
                                          (PSRAM_MODE==PSRAM_VADDR_MODE_NORMAL)?"normal (1-core)":"ERROR");
    spiram_inited=true;
    return ESP_OK;
}


esp_err_t esp_spiram_add_to_heapalloc(void)
{
    uint32_t size_for_flash = (pages_for_flash << 16);
    ESP_EARLY_LOGI(TAG, "Adding pool of %dK of external SPI memory to heap allocator", (CONFIG_SPIRAM_SIZE - (pages_for_flash << 16))/1024);
    //Add entire external RAM region to heap allocator. Heap allocator knows the capabilities of this type of memory, so there's
    //no need to explicitly specify them.

#if CONFIG_SPIRAM_SIZE <= DRAM0_DRAM1_DPORT_CACHE_SIZE
    /* cache size <= 10MB + 576KB, map DRAM0, DRAM1, DPORT bus */
    return heap_caps_add_region((intptr_t)SPIRAM_SMALL_SIZE_MAP_VADDR + size_for_flash, (intptr_t)SPIRAM_SMALL_SIZE_MAP_VADDR + SPIRAM_SMALL_SIZE_MAP_SIZE -1);
#else
#if CONFIG_SPIRAM_USE_AHB_DBUS3
#if CONFIG_SPIRAM_SIZE <= DRAM0_DRAM1_DPORT_DBUS3_CACHE_SIZE
    /* cache size <= 14MB + 576KB, map DRAM0, DRAM1, DPORT bus, as well as data bus3 */
    if (size_for_flash <= SPIRAM_MID_SIZE_MAP_SIZE) {
        esp_err_t err = heap_caps_add_region((intptr_t)SPIRAM_MID_SIZE_MAP_VADDR + size_for_flash, (intptr_t)SPIRAM_MID_SIZE_MAP_VADDR + SPIRAM_MID_SIZE_MAP_SIZE -1);
        if (err) {
            return err;
        }
        return heap_caps_add_region((intptr_t)SPIRAM_MID_BIG_SIZE_MAP_VADDR, (intptr_t)SPIRAM_MID_BIG_SIZE_MAP_VADDR + SPIRAM_MID_BIG_SIZE_MAP_SIZE -1);
    } else {
        return heap_caps_add_region((intptr_t)SPIRAM_MID_BIG_SIZE_MAP_VADDR + size_for_flash - SPIRAM_MID_SIZE_MAP_SIZE, (intptr_t)SPIRAM_MID_BIG_SIZE_MAP_VADDR + SPIRAM_MID_BIG_SIZE_MAP_SIZE -1);
    }
#else
    if (size_for_flash <= SPIRAM_SIZE_EXC_DATA_CACHE) {
        esp_err_t err = heap_caps_add_region((intptr_t)SPIRAM_BIG_SIZE_MAP_VADDR, (intptr_t)SPIRAM_BIG_SIZE_MAP_VADDR + SPIRAM_BIG_SIZE_MAP_SIZE -1);
        if (err) {
            return err;
        }
        return heap_caps_add_region((intptr_t)SPIRAM_MID_BIG_SIZE_MAP_VADDR, (intptr_t)SPIRAM_MID_BIG_SIZE_MAP_VADDR + SPIRAM_MID_BIG_SIZE_MAP_SIZE -1);
    } else if (size_for_flash <= SPIRAM_SIZE_EXC_DRAM0_DRAM1_DPORT) {
        esp_err_t err = heap_caps_add_region((intptr_t)SPIRAM_BIG_SIZE_MAP_VADDR + size_for_flash - SPIRAM_SIZE_EXC_DATA_CACHE, (intptr_t)SPIRAM_MID_SIZE_MAP_VADDR + SPIRAM_MID_SIZE_MAP_SIZE -1);
        if (err) {
            return err;
        }
        return heap_caps_add_region((intptr_t)SPIRAM_MID_BIG_SIZE_MAP_VADDR, (intptr_t)SPIRAM_MID_BIG_SIZE_MAP_VADDR + SPIRAM_MID_BIG_SIZE_MAP_SIZE -1);
    } else {
        return heap_caps_add_region((intptr_t)SPIRAM_MID_BIG_SIZE_MAP_VADDR + size_for_flash - SPIRAM_SIZE_EXC_DRAM0_DRAM1_DPORT, (intptr_t)SPIRAM_MID_BIG_SIZE_MAP_VADDR + SPIRAM_MID_BIG_SIZE_MAP_SIZE -1);
    }
#endif
#else
    Cache_Dbus_MMU_Set(DPORT_MMU_ACCESS_SPIRAM, SPIRAM_MID_BIG_SIZE_MAP_VADDR, SPIRAM_MID_BIG_SIZE_MAP_PADDR, 64, SPIRAM_MID_BIG_SIZE_MAP_SIZE >> 16, 0);
    if (size_for_flash <= SPIRAM_SIZE_EXC_DRAM0_DRAM1_DPORT) {
        return heap_caps_add_region((intptr_t)SPIRAM_MID_BIG_SIZE_MAP_VADDR, (intptr_t)SPIRAM_MID_BIG_SIZE_MAP_VADDR + SPIRAM_MID_BIG_SIZE_MAP_SIZE -1);
    } else {
        return heap_caps_add_region((intptr_t)SPIRAM_MID_BIG_SIZE_MAP_VADDR + size_for_flash, (intptr_t)SPIRAM_MID_BIG_SIZE_MAP_VADDR + SPIRAM_MID_BIG_SIZE_MAP_SIZE -1);
    }
#endif
#endif
}


static uint8_t *dma_heap;

esp_err_t esp_spiram_reserve_dma_pool(size_t size) {
    if (size==0) return ESP_OK; //no-op
    ESP_EARLY_LOGI(TAG, "Reserving pool of %dK of internal memory for DMA/internal allocations", size/1024);
    dma_heap=heap_caps_malloc(size, MALLOC_CAP_DMA|MALLOC_CAP_INTERNAL);
    if (!dma_heap) return ESP_ERR_NO_MEM;
    uint32_t caps[]={MALLOC_CAP_DMA|MALLOC_CAP_INTERNAL, 0, MALLOC_CAP_8BIT|MALLOC_CAP_32BIT};
    return heap_caps_add_region_with_caps(caps, (intptr_t) dma_heap, (intptr_t) dma_heap+size-1);
}

size_t esp_spiram_get_size(void)
{
    return CONFIG_SPIRAM_SIZE;
}

/*
 Before flushing the cache, if psram is enabled as a memory-mapped thing, we need to write back the data in the cache to the psram first,
 otherwise it will get lost. For now, we just read 64/128K of random PSRAM memory to do this.
*/
void IRAM_ATTR esp_spiram_writeback_cache(void)
{
    extern void Cache_WriteBack_All(void);
    int cache_was_disabled=0;

    if (!spiram_inited) return;

    //We need cache enabled for this to work. Re-enable it if needed; make sure we
    //disable it again on exit as well.
    if (DPORT_REG_GET_BIT(DPORT_PRO_DCACHE_CTRL_REG, DPORT_PRO_DCACHE_ENABLE)==0) {
        cache_was_disabled|=(1<<0);
        DPORT_SET_PERI_REG_BITS(DPORT_PRO_DCACHE_CTRL_REG, 1, 1, DPORT_PRO_DCACHE_ENABLE_S);
    }

#ifndef CONFIG_FREERTOS_UNICORE
    if (DPORT_REG_GET_BIT(DPORT_APP_CACHE_CTRL_REG, DPORT_APP_CACHE_ENABLE)==0) {
        cache_was_disabled|=(1<<1);
        DPORT_SET_PERI_REG_BITS(DPORT_APP_CACHE_CTRL_REG, 1, 1, DPORT_APP_CACHE_ENABLE_S);
    }
#endif

    Cache_WriteBack_All();

    if (cache_was_disabled&(1<<0)) {
#ifdef DPORT_CODE_COMPLETE
        while (DPORT_GET_PERI_REG_BITS2(DPORT_PRO_DCACHE_DBUG2_REG, DPORT_PRO_CACHE_STATE, DPORT_PRO_CACHE_STATE_S) != 1) ;
#endif
        DPORT_SET_PERI_REG_BITS(DPORT_PRO_DCACHE_CTRL_REG, 1, 0, DPORT_PRO_DCACHE_ENABLE_S);
    }
#ifndef CONFIG_FREERTOS_UNICORE
    if (cache_was_disabled&(1<<1)) {
        while (DPORT_GET_PERI_REG_BITS2(DPORT_APP_DCACHE_DBUG2_REG, DPORT_APP_CACHE_STATE, DPORT_APP_CACHE_STATE_S) != 1) ;
        DPORT_SET_PERI_REG_BITS(DPORT_APP_CACHE_CTRL_REG, 1, 0, DPORT_APP_CACHE_ENABLE_S);
    }
#endif
}

#endif
