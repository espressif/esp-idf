/*
Abstraction layer for spi-ram. For now, it's no more than a stub for the spiram_psram functions, but if
we add more types of external RAM memory, this can be made into a more intelligent dispatcher.
*/

/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include <sys/param.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp32s2/spiram.h"
#include "spiram_psram.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/xtensa_api.h"
#include "soc/soc.h"
#include "esp_heap_caps_init.h"
#include "soc/soc_memory_layout.h"
#include "soc/dport_reg.h"
#include "esp32s2/rom/cache.h"
#include "soc/cache_memory.h"
#include "soc/extmem_reg.h"

#define PSRAM_MODE PSRAM_VADDR_MODE_NORMAL

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

#define DRAM0_ONLY_CACHE_SIZE                   BUS_IRAM0_CACHE_SIZE
#define DRAM0_DRAM1_CACHE_SIZE                  (BUS_IRAM0_CACHE_SIZE + BUS_IRAM1_CACHE_SIZE)
#define DRAM0_DRAM1_DPORT_CACHE_SIZE            (BUS_IRAM0_CACHE_SIZE + BUS_IRAM1_CACHE_SIZE + BUS_DPORT_CACHE_SIZE)
#define SPIRAM_SIZE_EXC_DRAM0_DRAM1_DPORT       (spiram_size - DRAM0_DRAM1_DPORT_CACHE_SIZE)

#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
extern uint8_t _ext_ram_bss_start, _ext_ram_bss_end;
#define ALIGN_UP_BY(num, align)                 (((num) + ((align) - 1)) & ~((align) - 1))
#define EXT_BSS_SIZE                            ((uint32_t)(&_ext_ram_bss_end - &_ext_ram_bss_start))
#define EXT_BSS_PAGE_ALIGN_SIZE                 (ALIGN_UP_BY(EXT_BSS_SIZE, 0x10000))
#endif

#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
#define SPIRAM_MAP_PADDR_START                  EXT_BSS_PAGE_ALIGN_SIZE
#define FREE_DRAM0_DRAM1_DPORT_CACHE_START      (DPORT_CACHE_ADDRESS_LOW + EXT_BSS_PAGE_ALIGN_SIZE)
#define FREE_DRAM0_DRAM1_DPORT_CACHE_SIZE       (DRAM0_DRAM1_DPORT_CACHE_SIZE - EXT_BSS_PAGE_ALIGN_SIZE)
#else
#define SPIRAM_MAP_PADDR_START                  0
#define FREE_DRAM0_DRAM1_DPORT_CACHE_START      (DPORT_CACHE_ADDRESS_LOW)
#define FREE_DRAM0_DRAM1_DPORT_CACHE_SIZE       (DRAM0_DRAM1_DPORT_CACHE_SIZE)
#endif // if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY

#define SPIRAM_MAP_VADDR_START                  (DRAM0_CACHE_ADDRESS_HIGH - spiram_map_size)
#define SPIRAM_MAP_SIZE                         spiram_map_size

static uint32_t next_map_page_num = 0;
static uint32_t instruction_in_spiram = 0;
static uint32_t rodata_in_spiram = 0;
static size_t   spiram_size = 0;
static size_t   spiram_map_size = 0;

#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
static int instr_flash2spiram_offs = 0;
static uint32_t instr_start_page = 0;
static uint32_t instr_end_page = 0;
#endif

#if CONFIG_SPIRAM_RODATA
static int rodata_flash2spiram_offs = 0;
static uint32_t rodata_start_page = 0;
static uint32_t rodata_end_page = 0;
#endif

#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS || CONFIG_SPIRAM_RODATA
static uint32_t page0_mapped = 0;
static uint32_t page0_page = INVALID_PHY_PAGE;
#endif

void IRAM_ATTR esp_spiram_init_cache(void)
{
    spiram_map_size = spiram_size;
    Cache_Suspend_DCache();

#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
    /*if instruction or rodata in flash will be load to spiram, some subsequent operations require the start
    address to be aligned by page, so allocate N pages address space for spiram's bss*/
    Cache_Dbus_MMU_Set(MMU_ACCESS_SPIRAM, DPORT_CACHE_ADDRESS_LOW, 0, 64, EXT_BSS_PAGE_ALIGN_SIZE >> 16, 0);
    REG_CLR_BIT(EXTMEM_PRO_DCACHE_CTRL1_REG, EXTMEM_PRO_DCACHE_MASK_DPORT);
    next_map_page_num += (EXT_BSS_PAGE_ALIGN_SIZE >> 16);
    spiram_map_size -= EXT_BSS_PAGE_ALIGN_SIZE;
#endif

    /* map the address from SPIRAM end to the start, map the address in order: DRAM0, DRAM1, DPORT */
    if (spiram_map_size <= DRAM0_ONLY_CACHE_SIZE) {
        /* psram need to be mapped vaddr size <= 3MB + 512 KB, only map DRAM0 bus */
        Cache_Dbus_MMU_Set(MMU_ACCESS_SPIRAM, SPIRAM_MAP_VADDR_START, SPIRAM_MAP_PADDR_START, 64, SPIRAM_MAP_SIZE >> 16, 0);
        REG_CLR_BIT(EXTMEM_PRO_DCACHE_CTRL1_REG, EXTMEM_PRO_DCACHE_MASK_DRAM0);
    } else if (spiram_map_size <= DRAM0_DRAM1_CACHE_SIZE) {
        /* psram need to be mapped vaddr size <= 7MB + 512KB, only map DRAM0 and DRAM1 bus */
        Cache_Dbus_MMU_Set(MMU_ACCESS_SPIRAM, SPIRAM_MAP_VADDR_START, SPIRAM_MAP_PADDR_START, 64, SPIRAM_MAP_SIZE >> 16, 0);
        REG_CLR_BIT(EXTMEM_PRO_DCACHE_CTRL1_REG, EXTMEM_PRO_DCACHE_MASK_DRAM1 | EXTMEM_PRO_DCACHE_MASK_DRAM0);
    } else if (spiram_size <= DRAM0_DRAM1_DPORT_CACHE_SIZE) { // Equivalent to {spiram_map_size < DRAM0_DRAM1_DPORT_CACHE_SIZE - (spiram_size - spiram_map_size)/*bss size*/}
        /* psram need to be mapped vaddr size <= 10MB + 512KB - bss_page_align_size, map DRAM0, DRAM1, DPORT bus */
        Cache_Dbus_MMU_Set(MMU_ACCESS_SPIRAM, SPIRAM_MAP_VADDR_START, SPIRAM_MAP_PADDR_START, 64, SPIRAM_MAP_SIZE >> 16, 0);
        REG_CLR_BIT(EXTMEM_PRO_DCACHE_CTRL1_REG, EXTMEM_PRO_DCACHE_MASK_DRAM1 | EXTMEM_PRO_DCACHE_MASK_DRAM0 | EXTMEM_PRO_DCACHE_MASK_DPORT);
    } else {
        /* psram need to be mapped vaddr size > 10MB + 512KB - bss_page_align_size, map DRAM0, DRAM1, DPORT bus ,discard the memory in the end of spiram */
        Cache_Dbus_MMU_Set(MMU_ACCESS_SPIRAM, FREE_DRAM0_DRAM1_DPORT_CACHE_START, SPIRAM_MAP_PADDR_START, 64, FREE_DRAM0_DRAM1_DPORT_CACHE_SIZE >> 16, 0);
        REG_CLR_BIT(EXTMEM_PRO_DCACHE_CTRL1_REG, EXTMEM_PRO_DCACHE_MASK_DRAM1 | EXTMEM_PRO_DCACHE_MASK_DRAM0 | EXTMEM_PRO_DCACHE_MASK_DPORT);
    }
    Cache_Resume_DCache(0);
}

uint32_t esp_spiram_instruction_access_enabled(void)
{
    return instruction_in_spiram;
}

uint32_t esp_spiram_rodata_access_enabled(void)
{
    return rodata_in_spiram;
}

#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
esp_err_t esp_spiram_enable_instruction_access(void)
{
    uint32_t pages_in_flash = 0;
    pages_in_flash += Cache_Count_Flash_Pages(PRO_CACHE_IBUS0, &page0_mapped);
    pages_in_flash += Cache_Count_Flash_Pages(PRO_CACHE_IBUS1, &page0_mapped);
    if ((pages_in_flash + next_map_page_num) > (spiram_size >> 16)) {
        ESP_EARLY_LOGE(TAG, "SPI RAM space not enough for the instructions, has %d pages, need %d pages.", (spiram_size >> 16), (pages_in_flash + next_map_page_num));
        return ESP_FAIL;
    }
    ESP_EARLY_LOGI(TAG, "Instructions copied and mapped to SPIRAM");
    uint32_t instr_mmu_offset = ((uint32_t)&_instruction_reserved_start & 0xFFFFFF)/MMU_PAGE_SIZE;
    uint32_t mmu_value = *(volatile uint32_t *)(DR_REG_MMU_TABLE + PRO_CACHE_IBUS0_MMU_START + instr_mmu_offset*sizeof(uint32_t));
    mmu_value &= MMU_ADDRESS_MASK;
    instr_flash2spiram_offs = mmu_value - next_map_page_num;
    ESP_EARLY_LOGV(TAG, "Instructions from flash page%d copy to SPIRAM page%d, Offset: %d", mmu_value, next_map_page_num, instr_flash2spiram_offs);
    next_map_page_num = Cache_Flash_To_SPIRAM_Copy(PRO_CACHE_IBUS0, IRAM0_ADDRESS_LOW, next_map_page_num, &page0_page);
    next_map_page_num = Cache_Flash_To_SPIRAM_Copy(PRO_CACHE_IBUS1, IRAM1_ADDRESS_LOW, next_map_page_num, &page0_page);
    instruction_in_spiram = 1;
    return ESP_OK;
}
#endif

#if CONFIG_SPIRAM_RODATA
esp_err_t esp_spiram_enable_rodata_access(void)
{
    uint32_t pages_in_flash = 0;
    pages_in_flash += Cache_Count_Flash_Pages(PRO_CACHE_IBUS2, &page0_mapped);
    pages_in_flash += Cache_Count_Flash_Pages(PRO_CACHE_DBUS0, &page0_mapped);
    pages_in_flash += Cache_Count_Flash_Pages(PRO_CACHE_DBUS1, &page0_mapped);
    pages_in_flash += Cache_Count_Flash_Pages(PRO_CACHE_DBUS2, &page0_mapped);

    if ((pages_in_flash + next_map_page_num) > (spiram_size >> 16)) {
        ESP_EARLY_LOGE(TAG, "SPI RAM space not enough for the read only data.");
        return ESP_FAIL;
    }

    ESP_EARLY_LOGI(TAG, "Read only data copied and mapped to SPIRAM");
    uint32_t rodata_mmu_offset = ((uint32_t)&_rodata_reserved_start & 0xFFFFFF)/MMU_PAGE_SIZE;
    uint32_t mmu_value = *(volatile uint32_t *)(DR_REG_MMU_TABLE + PRO_CACHE_IBUS2_MMU_START + rodata_mmu_offset*sizeof(uint32_t));
    mmu_value &= MMU_ADDRESS_MASK;
    rodata_flash2spiram_offs = mmu_value - next_map_page_num;
    ESP_EARLY_LOGV(TAG, "Rodata from flash page%d copy to SPIRAM page%d, Offset: %d", mmu_value, next_map_page_num, rodata_flash2spiram_offs);
    next_map_page_num = Cache_Flash_To_SPIRAM_Copy(PRO_CACHE_IBUS2, DROM0_ADDRESS_LOW, next_map_page_num, &page0_page);
    next_map_page_num = Cache_Flash_To_SPIRAM_Copy(PRO_CACHE_DBUS0, DRAM0_ADDRESS_LOW, next_map_page_num, &page0_page);
    next_map_page_num = Cache_Flash_To_SPIRAM_Copy(PRO_CACHE_DBUS1, DRAM1_ADDRESS_LOW, next_map_page_num, &page0_page);
    next_map_page_num = Cache_Flash_To_SPIRAM_Copy(PRO_CACHE_DBUS2, DPORT_ADDRESS_LOW, next_map_page_num, &page0_page);
    rodata_in_spiram = 1;
    return ESP_OK;
}
#endif

#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
void instruction_flash_page_info_init(void)
{
    uint32_t instr_page_cnt = ((uint32_t)&_instruction_reserved_end - SOC_IROM_LOW + MMU_PAGE_SIZE - 1)/MMU_PAGE_SIZE;
    uint32_t instr_mmu_offset = ((uint32_t)&_instruction_reserved_start & 0xFFFFFF)/MMU_PAGE_SIZE;

    instr_start_page = *(volatile uint32_t *)(DR_REG_MMU_TABLE + PRO_CACHE_IBUS0_MMU_START + instr_mmu_offset*sizeof(uint32_t));
    instr_start_page &= MMU_ADDRESS_MASK;
    instr_end_page = instr_start_page + instr_page_cnt - 1;
}

uint32_t IRAM_ATTR instruction_flash_start_page_get(void)
{
    return instr_start_page;
}

uint32_t IRAM_ATTR instruction_flash_end_page_get(void)
{
    return instr_end_page;
}

int IRAM_ATTR instruction_flash2spiram_offset(void)
{
    return instr_flash2spiram_offs;
}
#endif

#if CONFIG_SPIRAM_RODATA
void rodata_flash_page_info_init(void)
{
    uint32_t rodata_page_cnt = ((uint32_t)&_rodata_reserved_end - SOC_DROM_LOW + MMU_PAGE_SIZE - 1)/MMU_PAGE_SIZE;
    uint32_t rodata_mmu_offset = ((uint32_t)&_rodata_reserved_start & 0xFFFFFF)/MMU_PAGE_SIZE;

    rodata_start_page = *(volatile uint32_t *)(DR_REG_MMU_TABLE + PRO_CACHE_IBUS2_MMU_START + rodata_mmu_offset*sizeof(uint32_t));
    rodata_start_page &= MMU_ADDRESS_MASK;
    rodata_end_page = rodata_start_page + rodata_page_cnt - 1;
}

uint32_t IRAM_ATTR rodata_flash_start_page_get(void)
{
    return rodata_start_page;
}

uint32_t IRAM_ATTR rodata_flash_end_page_get(void)
{
    return rodata_end_page;
}

int IRAM_ATTR rodata_flash2spiram_offset(void)
{
    return rodata_flash2spiram_offs;
}
#endif

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

    spiram_inited = true;

    spiram_size = esp_spiram_get_size();

#if (CONFIG_SPIRAM_SIZE != -1)
    if (spiram_size != CONFIG_SPIRAM_SIZE) {
        ESP_EARLY_LOGE(TAG, "Expected %dKiB chip but found %dKiB chip. Bailing out..", CONFIG_SPIRAM_SIZE/1024, spiram_size/1024);
        return ESP_ERR_INVALID_SIZE;
    }
#endif

    ESP_EARLY_LOGI(TAG, "Found %dMBit SPI RAM device",
                                          (spiram_size*8)/(1024*1024));
    ESP_EARLY_LOGI(TAG, "SPI RAM mode: %s", PSRAM_SPEED == PSRAM_CACHE_S40M ? "sram 40m" : \
                                          PSRAM_SPEED == PSRAM_CACHE_S80M ? "sram 80m" : "sram 20m");
    ESP_EARLY_LOGI(TAG, "PSRAM initialized, cache is in %s mode.", \
                                          (PSRAM_MODE==PSRAM_VADDR_MODE_EVENODD)?"even/odd (2-core)": \
                                          (PSRAM_MODE==PSRAM_VADDR_MODE_LOWHIGH)?"low/high (2-core)": \
                                          (PSRAM_MODE==PSRAM_VADDR_MODE_NORMAL)?"normal (1-core)":"ERROR");
    return ESP_OK;
}


esp_err_t esp_spiram_add_to_heapalloc(void)
{
    size_t recycle_pages_size = 0;
    size_t map_size = 0;
    intptr_t map_vaddr, map_paddr;
    ESP_EARLY_LOGI(TAG, "Adding pool of %dK of external SPI memory to heap allocator", (spiram_size - (next_map_page_num << 16))/1024);

#if CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY
    if(EXT_BSS_SIZE){
        ESP_EARLY_LOGI(TAG, "Adding pool of %d Byte(spiram .bss page unused area) of external SPI memory to heap allocator", EXT_BSS_PAGE_ALIGN_SIZE - EXT_BSS_SIZE);
        esp_err_t err_status = heap_caps_add_region(DPORT_CACHE_ADDRESS_LOW + EXT_BSS_SIZE, FREE_DRAM0_DRAM1_DPORT_CACHE_START - 1);
        if (err_status != ESP_OK){
            return err_status;
        }
    }
#endif

#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS || CONFIG_SPIRAM_RODATA
        /* Part of the physical address space in spiram is mapped by IRAM0/DROM0,
         so the DPORT_DRAM0_DRAM1 address space of the same size can be released */
        uint32_t occupied_pages_size = (next_map_page_num << 16);
        recycle_pages_size = occupied_pages_size - SPIRAM_MAP_PADDR_START;
#endif

    // Small size: means DPORT_DRAM0_DRAM1 bus virtrual address space larger than the spiram size
    if (spiram_size <= DRAM0_DRAM1_DPORT_CACHE_SIZE) {
        map_vaddr = SPIRAM_MAP_VADDR_START;
        return heap_caps_add_region(map_vaddr + recycle_pages_size, map_vaddr + spiram_map_size - 1); // pass rodata & instruction section
    }

    // Middle size: means DPORT_DRAM0_DRAM1 bus virtrual address space less than the
    //              spiram size, but after releasing the virtual address space mapped
    //              from the rodata or instruction copied from the flash, the released
    //              virtual address space is enough to map the abandoned physical address
    //              space in spiram
    if (recycle_pages_size >= SPIRAM_SIZE_EXC_DRAM0_DRAM1_DPORT) {
        map_vaddr = SPIRAM_MAP_VADDR_START + recycle_pages_size;
        map_paddr = SPIRAM_MAP_PADDR_START + recycle_pages_size;
        map_size = SPIRAM_MAP_SIZE - recycle_pages_size;
        Cache_Dbus_MMU_Set(MMU_ACCESS_SPIRAM, map_vaddr, map_paddr, 64, map_size >> 16, 0);
        return heap_caps_add_region(map_vaddr , map_vaddr + map_size - 1);
    }

    // Large size:  means after releasing the virtual address space mapped from the rodata
    //              or instruction copied from the flash, the released virtual address space
    //              still not enough to map the abandoned physical address space in spiram,
    //              so use all the virtual address space as much as possible
    map_vaddr = FREE_DRAM0_DRAM1_DPORT_CACHE_START;
    map_paddr = SPIRAM_MAP_PADDR_START + recycle_pages_size;
    map_size = FREE_DRAM0_DRAM1_DPORT_CACHE_SIZE;
    Cache_Dbus_MMU_Set(MMU_ACCESS_SPIRAM, map_vaddr, map_paddr, 64, map_size >> 16, 0);
    return heap_caps_add_region(map_vaddr, map_vaddr + FREE_DRAM0_DRAM1_DPORT_CACHE_SIZE -1);
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
    if (!spiram_inited) {
        ESP_EARLY_LOGE(TAG, "SPI RAM not initialized");
        abort();
    }

    psram_size_t size=psram_get_size();
    if (size==PSRAM_SIZE_16MBITS) return 2*1024*1024;
    if (size==PSRAM_SIZE_32MBITS) return 4*1024*1024;
    if (size==PSRAM_SIZE_64MBITS) return 8*1024*1024;
    return CONFIG_SPIRAM_SIZE;
}

/*
 Before flushing the cache, if psram is enabled as a memory-mapped thing, we need to write back the data in the cache to the psram first,
 otherwise it will get lost. For now, we just read 64/128K of random PSRAM memory to do this.
*/
void IRAM_ATTR esp_spiram_writeback_cache(void)
{
    extern void Cache_WriteBack_All(void);
    Cache_WriteBack_All();
}

/**
 * @brief If SPI RAM(PSRAM) has been initialized
 *
 * @return true SPI RAM has been initialized successfully
 * @return false SPI RAM hasn't been initialized or initialized failed
 */
bool esp_spiram_is_initialized(void)
{
    return spiram_inited;
}

uint8_t esp_spiram_get_cs_io(void)
{
    return psram_get_cs_io();
}

/*
 Simple RAM test. Writes a word every 32 bytes. Takes about a second to complete for 4MiB. Returns
 true when RAM seems OK, false when test fails. WARNING: Do not run this before the 2nd cpu has been
 initialized (in a two-core system) or after the heap allocator has taken ownership of the memory.
*/
bool esp_spiram_test(void)
{
    volatile int *spiram = (volatile int*)(SOC_EXTRAM_DATA_HIGH - spiram_map_size);
    size_t p;
    size_t s = spiram_map_size;
    int errct=0;
    int initial_err=-1;

    if (SOC_EXTRAM_DATA_SIZE < spiram_map_size) {
        ESP_EARLY_LOGW(TAG, "Only test spiram from %08x to %08x\n", SOC_EXTRAM_DATA_LOW, SOC_EXTRAM_DATA_HIGH);
        spiram=(volatile int*)SOC_EXTRAM_DATA_LOW;
        s = SOC_EXTRAM_DATA_SIZE;
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

#endif
