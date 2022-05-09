/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <sys/param.h>
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_private/mmu_psram.h"
#include "esp32s3/rom/cache.h"
#include "esp32s3/rom/ets_sys.h"
#include "soc/ext_mem_defs.h"
#include "soc/extmem_reg.h"

#define MMU_PAGE_SIZE    0x10000
#define MMU_PAGE_TO_BYTES(page_id)      ((page_id) * MMU_PAGE_SIZE)
#define BYTES_TO_MMU_PAGE(bytes)        ((bytes) / MMU_PAGE_SIZE)

const static char *TAG = "mmu_psram";

//------------------------------------Copy Flash .text to PSRAM-------------------------------------//
#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
extern int _instruction_reserved_start;
extern int _instruction_reserved_end;

static uint32_t instruction_in_spiram;
static uint32_t instr_start_page;
static uint32_t instr_end_page;
static int instr_flash2spiram_offs;

uint32_t esp_spiram_instruction_access_enabled(void);
int instruction_flash2spiram_offset(void);
uint32_t instruction_flash_start_page_get(void);
uint32_t instruction_flash_end_page_get(void);
#endif  //CONFIG_SPIRAM_FETCH_INSTRUCTIONS


//------------------------------------Copy Flash .rodata to PSRAM-------------------------------------//
#if CONFIG_SPIRAM_RODATA
extern int _rodata_reserved_start;
extern int _rodata_reserved_end;

static uint32_t rodata_in_spiram;
static int rodata_flash2spiram_offs;
static uint32_t rodata_start_page;
static uint32_t rodata_end_page;

uint32_t esp_spiram_rodata_access_enabled(void);
int rodata_flash2spiram_offset(void);
uint32_t rodata_flash_start_page_get(void);
uint32_t rodata_flash_end_page_get(void);
#endif  //#if CONFIG_SPIRAM_RODATA

#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS || CONFIG_SPIRAM_RODATA
//TODO IDF-4387
static uint32_t page0_mapped = 0;
static uint32_t page0_page = INVALID_PHY_PAGE;
#endif  //#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS || CONFIG_SPIRAM_RODATA


#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
esp_err_t mmu_config_psram_text_segment(uint32_t start_page, uint32_t psram_size, uint32_t *out_page)
{
    uint32_t page_id = start_page;

    /**
     * TODO IDF-4387
     * `Cache_Count_Flash_Pages` seems give wrong results. Need to confirm this.
     * FOR NOW, leave these logics just as it used to be.
     */
    uint32_t flash_pages = 0;
    flash_pages += Cache_Count_Flash_Pages(CACHE_IBUS, &page0_mapped);
    if ((flash_pages + page_id) > BYTES_TO_MMU_PAGE(psram_size)) {
        ESP_EARLY_LOGE(TAG, "PSRAM space not enough for the Flash instructions, need %d B, from %d B to %d B",
                       MMU_PAGE_TO_BYTES(flash_pages), MMU_PAGE_TO_BYTES(start_page), MMU_PAGE_TO_BYTES(flash_pages + page_id));
        return ESP_FAIL;
    }

    //Enable DBUS, which is used for copying FLASH .text to PSRAM
    REG_CLR_BIT(EXTMEM_DCACHE_CTRL1_REG, EXTMEM_DCACHE_SHUT_CORE0_BUS);
#if !CONFIG_FREERTOS_UNICORE
    REG_CLR_BIT(EXTMEM_DCACHE_CTRL1_REG, EXTMEM_DCACHE_SHUT_CORE1_BUS);
#endif

    // uint32_t instr_page_cnt = ((uint32_t)&_instruction_reserved_end - (uint32_t)&_instruction_reserved_start + MMU_PAGE_SIZE - 1) / MMU_PAGE_SIZE;
    // uint32_t instr_mmu_offset = ((uint32_t)&_instruction_reserved_start & MMU_VADDR_MASK) / MMU_PAGE_SIZE;

    // instr_start_page = ((volatile uint32_t *)(DR_REG_MMU_TABLE + PRO_CACHE_IBUS0_MMU_START))[instr_mmu_offset];
    // instr_start_page &= MMU_VALID_VAL_MASK;
    // instr_end_page = instr_start_page + instr_page_cnt - 1;
    // instr_flash2spiram_offs = instr_start_page - page_id;
    // ESP_EARLY_LOGV(TAG, "Instructions from flash page%d copy to SPIRAM page%d, Offset: %d", instr_start_page, page_id, instr_flash2spiram_offs);

    ets_printf(DRAM_STR("_instruction_reserved_end addr is %x\n"), (uint32_t)&_instruction_reserved_start);
    uint32_t instr_page_cnt = ((uint32_t)&_instruction_reserved_end - SOC_IROM_LOW + MMU_PAGE_SIZE - 1) / MMU_PAGE_SIZE;

    instr_start_page = *(volatile uint32_t *)(DR_REG_MMU_TABLE + CACHE_IROM_MMU_START);
    instr_start_page &= MMU_VALID_VAL_MASK;
    instr_end_page = instr_start_page + instr_page_cnt - 1;
    instr_flash2spiram_offs = instr_start_page - page_id;
    ESP_EARLY_LOGE(TAG, "Instructions from flash page%d copy to SPIRAM page%d, Offset: %d", instr_start_page, page_id, instr_flash2spiram_offs);








    // page_id = Cache_Flash_To_SPIRAM_Copy(PRO_CACHE_IBUS0, IRAM0_ADDRESS_LOW, page_id, &page0_page);
    // page_id = Cache_Flash_To_SPIRAM_Copy(PRO_CACHE_IBUS1, IRAM1_ADDRESS_LOW, page_id, &page0_page);
    // instruction_in_spiram = 1;
    // ESP_EARLY_LOGV(TAG, "after copy instruction, page_id is %d", page_id);
    // ESP_EARLY_LOGI(TAG, "Instructions copied and mapped to SPIRAM");

    page_id = Cache_Flash_To_SPIRAM_Copy(CACHE_IBUS, IRAM0_CACHE_ADDRESS_LOW, page_id, &page0_page);
    instruction_in_spiram = 1;
    ESP_EARLY_LOGV(TAG, "after copy instruction, page_id is %d", page_id);
    ESP_EARLY_LOGI(TAG, "Instructions copied and mapped to SPIRAM");





    /**
     * Disable DRAM0_BUS.
     * .text (instructions) are mapped in `Cache_Flash_To_SPIRAM_Copy` to both `PRO_CACHE_IBUS0` and `PRO_CACHE_IBUS1`.
     *
     * For now, this bus (DRAM0) is only used for copying, so can be disabled. If it is used later, other code
     * should be responsible for enabling it.
     */
    REG_SET_BIT(EXTMEM_DCACHE_CTRL1_REG, EXTMEM_DCACHE_SHUT_CORE0_BUS);
#if !CONFIG_FREERTOS_UNICORE
    REG_SET_BIT(EXTMEM_DCACHE_CTRL1_REG, EXTMEM_DCACHE_SHUT_CORE1_BUS);
#endif
    *out_page = page_id - start_page;

    return ESP_OK;
}
#endif  //#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS



#if CONFIG_SPIRAM_RODATA
esp_err_t mmu_config_psram_rodata_segment(uint32_t start_page, uint32_t psram_size, uint32_t *out_page)
{
    uint32_t page_id = start_page;

    /**
     * TODO IDF-4387
     * `Cache_Count_Flash_Pages` seems give wrong results. Need to confirm this.
     * FOR NOW, leave these logics just as it used to be.
     */
    uint32_t flash_pages = 0;
    flash_pages += Cache_Count_Flash_Pages(CACHE_DBUS, &page0_mapped);
    if ((flash_pages + page_id) > BYTES_TO_MMU_PAGE(psram_size)) {
        ESP_EARLY_LOGE(TAG, "SPI RAM space not enough for the instructions, need to copy to %d B.", MMU_PAGE_TO_BYTES(flash_pages + page_id));
        return ESP_FAIL;
    }

    //Enable DBUS, which is used for copying FLASH .text to PSRAM
    REG_CLR_BIT(EXTMEM_DCACHE_CTRL1_REG, EXTMEM_DCACHE_SHUT_CORE0_BUS);
#if !CONFIG_FREERTOS_UNICORE
    REG_CLR_BIT(EXTMEM_DCACHE_CTRL1_REG, EXTMEM_DCACHE_SHUT_CORE1_BUS);
#endif






    // uint32_t rodata_page_cnt = ((uint32_t)&_rodata_reserved_end - (uint32_t)&_rodata_reserved_start + MMU_PAGE_SIZE - 1) / MMU_PAGE_SIZE;
    // uint32_t rodata_mmu_offset = ((uint32_t)&_rodata_reserved_start & MMU_VADDR_MASK) / MMU_PAGE_SIZE;

    // rodata_start_page = ((volatile uint32_t *)(DR_REG_MMU_TABLE + PRO_CACHE_IBUS2_MMU_START))[rodata_mmu_offset];
    // rodata_start_page &= MMU_VALID_VAL_MASK;
    // rodata_end_page = rodata_start_page + rodata_page_cnt - 1;
    // rodata_flash2spiram_offs = rodata_start_page - page_id;
    // ESP_EARLY_LOGV(TAG, "Rodata from flash page%d copy to SPIRAM page%d, Offset: %d", rodata_start_page, page_id, rodata_flash2spiram_offs);

    uint32_t rodata_page_cnt = ((uint32_t)&_rodata_reserved_end - ((uint32_t)&_rodata_reserved_start & ~ (MMU_PAGE_SIZE - 1)) + MMU_PAGE_SIZE - 1) / MMU_PAGE_SIZE;
    rodata_start_page = *(volatile uint32_t *)(DR_REG_MMU_TABLE + CACHE_DROM_MMU_START);
    rodata_start_page &= MMU_VALID_VAL_MASK;
    rodata_end_page = rodata_start_page + rodata_page_cnt - 1;
    rodata_flash2spiram_offs = rodata_start_page - page_id;
    ESP_EARLY_LOGE(TAG, "Rodata from flash page%d copy to SPIRAM page%d, Offset: %d", rodata_start_page, page_id, rodata_flash2spiram_offs);












    // page_id = Cache_Flash_To_SPIRAM_Copy(PRO_CACHE_IBUS2, DROM0_ADDRESS_LOW, page_id, &page0_page);
    // page_id = Cache_Flash_To_SPIRAM_Copy(PRO_CACHE_DBUS0, DRAM0_ADDRESS_LOW, page_id, &page0_page);
    // page_id = Cache_Flash_To_SPIRAM_Copy(PRO_CACHE_DBUS1, DRAM1_ADDRESS_LOW, page_id, &page0_page);
    // page_id = Cache_Flash_To_SPIRAM_Copy(PRO_CACHE_DBUS2, DPORT_ADDRESS_LOW, page_id, &page0_page);
    // rodata_in_spiram = 1;
    // ESP_EARLY_LOGV(TAG, "after copy rodata, page_id is %d", page_id);
    // ESP_EARLY_LOGI(TAG, "Read only data copied and mapped to SPIRAM");


    page_id = Cache_Flash_To_SPIRAM_Copy(CACHE_DBUS, DRAM0_CACHE_ADDRESS_LOW, page_id, &page0_page);
    rodata_in_spiram = 1;
    ESP_EARLY_LOGV(TAG, "after copy rodata, page_id is %d", page_id);
    ESP_EARLY_LOGI(TAG, "Read only data copied and mapped to SPIRAM");

    /**
     * Disable DRAM0_BUS.
     * .text (instructions) are mapped in `Cache_Flash_To_SPIRAM_Copy` to both `PRO_CACHE_IBUS0` and `PRO_CACHE_IBUS1`.
     *
     * For now, this bus (DRAM0) is only used for copying, so can be disabled. If it is used later, other code
     * should be responsible for enabling it.
     */
    REG_SET_BIT(EXTMEM_DCACHE_CTRL1_REG, EXTMEM_DCACHE_SHUT_CORE0_BUS);
#if !CONFIG_FREERTOS_UNICORE
    REG_SET_BIT(EXTMEM_DCACHE_CTRL1_REG, EXTMEM_DCACHE_SHUT_CORE1_BUS);
#endif

    *out_page = page_id - start_page;

    return ESP_OK;
}
#endif  //#if CONFIG_SPIRAM_RODATA




#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
uint32_t esp_spiram_instruction_access_enabled(void)
{
    return instruction_in_spiram;
}

int instruction_flash2spiram_offset(void)
{
    return instr_flash2spiram_offs;
}

uint32_t instruction_flash_start_page_get(void)
{
    return instr_start_page;
}

uint32_t instruction_flash_end_page_get(void)
{
    return instr_end_page;
}
#endif  //CONFIG_SPIRAM_FETCH_INSTRUCTIONS

#if CONFIG_SPIRAM_RODATA
uint32_t esp_spiram_rodata_access_enabled(void)
{
    return rodata_in_spiram;
}

int rodata_flash2spiram_offset(void)
{
    return rodata_flash2spiram_offs;
}

uint32_t rodata_flash_start_page_get(void)
{
    return rodata_start_page;
}

uint32_t rodata_flash_end_page_get(void)
{
    return rodata_end_page;
}
#endif  //#if CONFIG_SPIRAM_RODATA
