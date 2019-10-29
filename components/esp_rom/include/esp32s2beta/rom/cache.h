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

#ifndef _ROM_CACHE_H_
#define _ROM_CACHE_H_

#include "esp_bit_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup cache_apis, cache operation related apis
  * @brief cache apis
  */

/** @addtogroup cache_apis
  * @{
  */

#define MIN_CACHE_SIZE                  8192
#define MAX_CACHE_SIZE                  16384
#define MIN_CACHE_WAYS                  4
#define MAX_CACHE_WAYS                  8
#define MIN_CACHE_LINE_SIZE             16
#define MAX_CACHE_LINE_SIZE             64
//normally should be (MAX_CACHE_SIZE / MIN_CACHE_WAYS / MIN_CACHE_LINE_SIZE), however, the items not all in one tag memory block.
#define MAX_TAG_BLOCK_ITEMS             (MAX_CACHE_SIZE / MAX_CACHE_WAYS / MIN_CACHE_LINE_SIZE)
#define TAG_SIZE                        4
#define MAX_TAG_BLOCK_SIZE              (MAX_TAG_BLOCK_ITEMS * TAG_SIZE)

#define ESP_CACHE_TEMP_ADDR DROM0_ADDRESS_LOW
#define CACHE_MAX_OPERATION_SIZE BUS_ADDR_SIZE


typedef enum {
    CACHE_MEMORY_INVALID = 0,
    CACHE_MEMORY_ICACHE_LOW = BIT(0),
    CACHE_MEMORY_ICACHE_HIGH = BIT(1),
    CACHE_MEMORY_DCACHE_LOW = BIT(2),
    CACHE_MEMORY_DCACHE_HIGH = BIT(3),
} cache_layout_t;

typedef enum {
    CACHE_SIZE_8KB = 0,
    CACHE_SIZE_16KB = 1,
} cache_size_t;

typedef enum {
    CACHE_4WAYS_ASSOC = 0,
    CACHE_8WAYS_ASSOC = 1,
} cache_ways_t;

typedef enum {
    CACHE_LINE_SIZE_16B = 0,
    CACHE_LINE_SIZE_32B = 1,
    CACHE_LINE_SIZE_64B = 2,
} cache_line_size_t;

typedef enum {
    CACHE_AUTOLOAD_NORMAL_MODE = 0,     /*!< normal mode will autoload anytime if enabled */
    CACHE_AUTOLOAD_REGION_MODE = 1,     /*!< region mode only autoload if access the memory in regions */
} cache_autoload_mode_t;

typedef enum {
    CACHE_AUTOLOAD_POSITIVE = 0,        /*!< cache autoload step is positive */
    CACHE_AUTOLOAD_NEGATIVE = 1,        /*!< cache autoload step is negative */
} cache_autoload_order_t;

#define CACHE_AUTOLOAD_STEP(i) ((i) - 1)

typedef enum {
    CACHE_AUTOLOAD_MISS_TRIGGER = 0,    /*!< autoload only triggered by cache miss */
    CACHE_AUTOLOAD_HIT_TRIGGER  = 1,    /*!< autoload only triggered by cache hit */
    CACHE_AUTOLOAD_BOTH_TRIGGER = 2,    /*!< autoload triggered both by cache miss and hit */
} cache_autoload_trigger_t;

struct cache_mode {
    uint32_t cache_size;
    uint16_t cache_line_size;
    uint8_t cache_ways;
    uint8_t icache;
};

struct tag_item {
    uint32_t dirty:1;                   /*!< the cache line value is dirty or not */
    uint32_t tag:14;                    /*!< the tag is the high part of the cache address, however is only 16MB range, and with out low part */
    uint32_t valid:1;                   /*!< the tag item is valid or not */
    uint32_t fifo_cnt:3;                /*!< fifo cnt, 0 ~ 3 for 4 ways cache, 0 ~ 7 for 8 ways cache */
    uint32_t lock:1;                    /*!< the cache line is locked or not */
    uint32_t attr:3;                    /*!< the attribute of the external memory physical address */
    uint32_t access:1;                  /*!< software accessable, used by hardware */
    uint32_t reserved:8;
};

struct autoload_config {
    uint8_t mode;                       /*!< autoload mode */
    uint8_t order;                      /*!< autoload step is positive or negative */
    uint8_t step;                       /*!< autoload step */
    uint8_t trigger;                    /*!< autoload trigger */
    uint32_t autoload_size;             /*!< autoload size */
    uint32_t addr0;                     /*!< autoload region0 start address */
    uint32_t size0;                     /*!< autoload region0 size */
    uint32_t addr1;                     /*!< autoload region1 start address */
    uint32_t size1;                     /*!< autoload region1 size */
};

struct tag_group_info {
    struct cache_mode mode;                         /*!< cache and cache mode */
    uint32_t vaddr_offset;		            /*!< virtual address offset of the cache ways */
    uint32_t tag_addr[MAX_CACHE_WAYS];              /*!< tag memory address, only [0~mode.ways-1] is valid to use */
    uint32_t cache_memory_offset[MAX_CACHE_WAYS];   /*!< cache memory address, only [0~mode.ways-1] is valid to use */
};

struct lock_config {
    uint32_t addr;                                  /*!< manual lock address*/
    uint16_t size;                                  /*!< manual lock size*/
    uint16_t group;                                 /*!< manual lock group, 0 or 1*/
};

#define MMU_SET_ADDR_ALIGNED_ERROR	1
#define MMU_SET_PASE_SIZE_ERROR		3
#define MMU_SET_VADDR_OUT_RANGE		5

/**
  * @brief Initialise cache mmu, mark all entries as invalid.
  *        Please do not call this function in your SDK application.
  *
  * @param  None
  *
  * @return None
  */
void Cache_MMU_Init(void);

/**
  * @brief Set ICache mmu mapping.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t ext_ram : DPORT_MMU_ACCESS_FLASH for flash, DPORT_MMU_ACCESS_SPIRAM for spiram, DPORT_MMU_INVALID for invalid.
  *
  * @param  uint32_t vaddr : virtual address in CPU address space.
  *                              Can be Iram0,Iram1,Irom0,Drom0 and AHB buses address.
  *                              Should be aligned by psize.
  *
  * @param  uint32_t paddr : physical address in external memory.
  *                              Should be aligned by psize.
  *
  * @param  uint32_t psize : page size of ICache, in kilobytes. Should be 64 here.
  *
  * @param  uint32_t num : pages to be set.
  *
  * @param  uint32_t fixed : 0 for physical pages grow with virtual pages, other for virtual pages map to same physical page.
  *
  * @return uint32_t: error status
  *                   0 : mmu set success
  *                   1 : vaddr or paddr is not aligned
  *                   3 : psize error
  *                   5 : vaddr is out of range
  */
int Cache_Ibus_MMU_Set(uint32_t ext_ram, uint32_t vaddr, uint32_t paddr,  uint32_t psize, uint32_t num, uint32_t fixed);

/**
  * @brief Set DCache mmu mapping.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t ext_ram : DPORT_MMU_ACCESS_FLASH for flash, DPORT_MMU_ACCESS_SPIRAM for spiram, DPORT_MMU_INVALID for invalid.
  *
  * @param  uint32_t vaddr : virtual address in CPU address space.
  *                              Can be DRam0, DRam1, DRom0, DPort and AHB buses address.
  *                              Should be aligned by psize.
  *
  * @param  uint32_t paddr : physical address in external memory.
  *                              Should be aligned by psize.
  *
  * @param  uint32_t psize : page size of DCache, in kilobytes. Should be 64 here.
  *
  * @param  uint32_t num : pages to be set.

  * @param  uint32_t fixed : 0 for physical pages grow with virtual pages, other for virtual pages map to same physical page.
  *
  * @return uint32_t: error status
  *                   0 : mmu set success
  *                   1 : vaddr or paddr is not aligned
  *                   3 : psize error
  *                   5 : vaddr is out of range
  */
int Cache_Dbus_MMU_Set(uint32_t ext_ram, uint32_t vaddr, uint32_t paddr, uint32_t psize, uint32_t num, uint32_t fixed);

/**
  * @brief Copy DRom0 ICache MMU to DCache MMU.
  *        Please do not call this function in your SDK application.
  *
  * @param  None
  *
  * @return None
  */
void MMU_Drom0_I2D_Copy(void);

/**
  * @brief Unmap DRom0 ICache MMU.
  *        Please do not call this function in your SDK application.
  *
  * @param  None
  *
  * @return None
  */
void MMU_Drom_ICache_Unmap(void);

/**
  * @brief Count the pages in the bus room address which map to Flash.
  *        Please do not call this function in your SDK application.
  *
  * @param uint32_t bus : the bus to count with.
  *
  * @param uint32_t * page0_mapped : value should be initial by user, 0 for not mapped, other for mapped count.
  *
  * return uint32_t : the number of pages which map to Flash.
  */
uint32_t Cache_Count_Flash_Pages(uint32_t bus, uint32_t * page0_mapped);

/**
  * @brief Copy Instruction or rodata from Flash to SPIRAM, and remap to SPIRAM.
  *        Please do not call this function in your SDK application.
  *
  * @param uint32_t bus : the bus which need to copy to SPIRAM.
  *
  * @param uint32_t bus_start_addr : the start virtual address for the bus.
  *
  * @param uint32_t start_page : the start (64KB) page number in SPIRAM.
  *
  * @param uint32_t * page0_page : the flash page0 in SPIRAM page number, 0xffff for invalid.
  *
  * return uint32_t : the next start page number for SPIRAM not mapped.
  */
uint32_t Cache_Flash_To_SPIRAM_Copy(uint32_t bus, uint32_t bus_start_addr, uint32_t start_page, uint32_t * page0_page);


/**
  * @brief allocate memory to used by ICache and DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param cache_layout_t sram0_layout : the usage of first 8KB internal memory block, can be CACHE_MEMORY_INVALID, CACHE_MEMORY_ICACHE_LOW, CACHE_MEMORY_ICACHE_HIGH, CACHE_MEMORY_DCACHE_LOW and CACHE_MEMORY_DCACHE_HIGH
  *
  * @param cache_layout_t sram1_layout : the usage of second 8KB internal memory block 
  *
  * @param cache_layout_t sram2_layout : the usage of third 8KB internal memory block 
  *
  * @param cache_layout_t sram3_layout : the usage of forth 8KB internal memory block 
  *
  * return none
  */
void Cache_Allocate_SRAM(cache_layout_t sram0_layout, cache_layout_t sram1_layout, cache_layout_t sram2_layout, cache_layout_t sram3_layout);

/**
  * @brief Get cache mode of ICache or DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param struct cache_mode * mode : the pointer of cache mode struct
  *
  * return none
  */
void Cache_Get_Mode(struct cache_mode * mode);

/**
  * @brief set ICache modes: cache size, associate ways and cache line size.
  *        Please do not call this function in your SDK application.
  *
  * @param cache_size_t cache_size : the cache size, can be CACHE_SIZE_8KB and CACHE_SIZE_16KB
  *
  * @param cache_ways_t ways : the associate ways of cache, cane be CACHE_4WAYS_ASSOC and CACHE_8WAYS_ASSOC 
  *
  * @param cache_line_size_t cache_line_size : the cache line size, can be CACHE_LINE_SIZE_16B, CACHE_LINE_SIZE_32B and CACHE_LINE_SIZE_64B 
  *
  * @param cache_layout_t sram3_layout : the usage of forth 8KB internal memory block 
  *
  * return none
  */
void Cache_Set_ICache_Mode(cache_size_t cache_size, cache_ways_t ways, cache_line_size_t cache_line_size);

/**
  * @brief set DCache modes: cache size, associate ways and cache line size.
  *        Please do not call this function in your SDK application.
  *
  * @param cache_size_t cache_size : the cache size, can be CACHE_SIZE_8KB and CACHE_SIZE_16KB
  *
  * @param cache_ways_t ways : the associate ways of cache, cane be CACHE_4WAYS_ASSOC and CACHE_8WAYS_ASSOC 
  *
  * @param cache_line_size_t cache_line_size : the cache line size, can be CACHE_LINE_SIZE_16B, CACHE_LINE_SIZE_32B and CACHE_LINE_SIZE_64B 
  *
  * @param cache_layout_t sram3_layout : the usage of forth 8KB internal memory block 
  *
  * return none
  */
void Cache_Set_DCache_Mode(cache_size_t cache_size, cache_ways_t ways, cache_line_size_t cache_line_size);

/**
  * @brief check if the address is accessed through ICache.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t addr : the address to check.
  *
  * @return 1 if the address is accessed through ICache, 0 if not.
  */
uint32_t Cache_Address_Through_ICache(uint32_t addr);

/**
  * @brief check if the address is accessed through DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t addr : the address to check.
  *
  * @return 1 if the address is accessed through DCache, 0 if not.
  */
uint32_t Cache_Address_Through_DCache(uint32_t addr);

/**
  * @brief Invalidate the cache items for ICache.
  *        Operation will be done CACHE_LINE_SIZE aligned.
  *        If the addr is not in our addr room, we will Flush all Cache.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t addr: start address to invalidate
  *
  * @param  uint32_t size: size to invalidate, should <= 4MB
  *
  * @return None
  */
void Cache_Invalidate_ICache_Items(uint32_t addr, uint32_t size);

/**
  * @brief Invalidate the cache items for DCache.
  *        Operation will be done CACHE_LINE_SIZE aligned.
  *        If the addr is not in our addr room, we will Flush all Cache.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t addr: start address to invalidate
  *
  * @param  uint32_t size: size to invalidate, should <= 4MB
  *
  * @return None
  */
void Cache_Invalidate_DCache_Items(uint32_t addr, uint32_t size);

/**
  * @brief Clean the dirty bit of cache Items of DCache.
  *        Operation will be done CACHE_LINE_SIZE aligned.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t addr: start address to Clean
  *
  * @param  uint32_t size: size to Clean, should <= 4MB
  *
  * @return None
  */
void Cache_Clean_Items(uint32_t addr, uint32_t size);

/**
  * @brief Write back the cache items of DCache.
  *        Operation will be done CACHE_LINE_SIZE aligned.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t addr: start address to write back
  *
  * @param  uint32_t size: size to write back, should <= 4MB
  *
  * @return None
  */
void Cache_WriteBack_Items(uint32_t addr, uint32_t size);

/**
  * @brief Invalidate the Cache items in the region from ICache or DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t addr : invalidated region start address.
  *
  * @param  uint32_t size : invalidated region size.
  *
  * @return None
  */
void Cache_Invalidate_Addr(uint32_t addr, uint32_t size);

/**
  * @brief Clean the dirty bit of Cache items in the region from DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t addr : cleaned region start address.
  *
  * @param  uint32_t size : cleaned region size.
  *
  * @return None
  */
void Cache_Clean_Addr(uint32_t addr, uint32_t size);

/**
  * @brief Writeback the Cache items(also clean the dirty bit) in the region from DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t addr : writeback region start address.
  *
  * @param  uint32_t size : writeback region size.
  *
  * @return None
  */
void Cache_WriteBack_Addr(uint32_t addr, uint32_t size);


/**
  * @brief Invalidate all cache items in ICache.
  *        Please do not call this function in your SDK application.
  *
  * @param  None
  *
  * @return None
  */
void Cache_Invalidate_ICache_All(void);

/**
  * @brief Invalidate all cache items in DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param  None
  *
  * @return None
  */
void Cache_Invalidate_DCache_All(void);

/**
  * @brief Clean the dirty bit of all cache items in DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param  None
  *
  * @return None
  */
void Cache_Clean_All(void);

/**
  * @brief WriteBack all cache items in DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param  None
  *
  * @return None
  */
void Cache_WriteBack_All(void);

/**
  * @brief Mask all buses through ICache and DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param  None
  *
  * @return None
  */
void Cache_Mask_All(void);

/**
  * @brief UnMask DRom0 bus through ICache or DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param  None
  *
  * @return None
  */
void Cache_UnMask_Drom0(void);

/**
  * @brief Suspend ICache auto preload operation, then you can resume it after some ICache operations.
  *        Please do not call this function in your SDK application.
  *
  * @param  None
  *
  * @return uint32_t : 0 for ICache not auto preload before suspend.
  */
uint32_t Cache_Suspend_ICache_Autoload(void);

/**
  * @brief Resume ICache auto preload operation after some ICache operations.
  *        Please do not call this function in your SDK application.
  *
  * @param uint32_t autoload : 0 for ICache not auto preload before suspend.
  *
  * @return None.
  */
void Cache_Resume_ICache_Autoload(uint32_t autoload);

/**
  * @brief Suspend DCache auto preload operation, then you can resume it after some DCache operations.
  *        Please do not call this function in your SDK application.
  *
  * @param  None
  *
  * @return uint32_t : 0 for DCache not auto preload before suspend.
  */
uint32_t Cache_Suspend_DCache_Autoload(void);

/**
  * @brief Resume DCache auto preload operation after some DCache operations.
  *        Please do not call this function in your SDK application.
  *
  * @param uint32_t autoload : 0 for DCache not auto preload before suspend.
  *
  * @return None.
  */
void Cache_Resume_DCache_Autoload(uint32_t autoload);

/**
  * @brief Start an ICache manual preload, will suspend auto preload of ICache.
  *        Please do not call this function in your SDK application.
  *
  * @param uint32_t addr : start address of the preload region.
  *
  * @param uint32_t size : size of the preload region, should not exceed the size of ICache.
  *
  * @param uint32_t order : the preload order, 0 for positive, other for negative
  *
  * @return uint32_t : 0 for ICache not auto preload before manual preload.
  */
uint32_t Cache_Start_ICache_Preload(uint32_t addr, uint32_t size, uint32_t order);

/**
  * @brief Return if the ICache manual preload done.
  *        Please do not call this function in your SDK application.
  *
  * @param  None
  *
  * @return uint32_t : 0 for ICache manual preload not done.
  */
uint32_t Cache_ICache_Preload_Done(void);

/**
  * @brief End the ICache manual preload to resume auto preload of ICache.
  *        Please do not call this function in your SDK application.
  *
  * @param uint32_t autoload : 0 for ICache not auto preload before manual preload.
  *
  * @return None
  */
void Cache_End_ICache_Preload(uint32_t autoload);

/**
  * @brief Start an DCache manual preload, will suspend auto preload of DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param uint32_t addr : start address of the preload region.
  *
  * @param uint32_t size : size of the preload region, should not exceed the size of DCache.
  *
  * @param uint32_t order : the preload order, 0 for positive, other for negative
  *
  * @return uint32_t : 0 for DCache not auto preload before manual preload.
  */
uint32_t Cache_Start_DCache_Preload(uint32_t addr, uint32_t size, uint32_t order);

/**
  * @brief Return if the DCache manual preload done.
  *        Please do not call this function in your SDK application.
  *
  * @param  None
  *
  * @return uint32_t : 0 for DCache manual preload not done.
  */
uint32_t Cache_DCache_Preload_Done(void);

/**
  * @brief End the DCache manual preload to resume auto preload of DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param uint32_t autoload : 0 for DCache not auto preload before manual preload.
  *
  * @return None
  */
void Cache_End_DCache_Preload(uint32_t autoload);

/**
  * @brief Config autoload parameters of ICache.
  *        Please do not call this function in your SDK application.
  *
  * @param struct autoload_config * config : autoload parameters.
  *
  * @return None
  */
void Cache_Config_ICache_Autoload(const struct autoload_config * config);

/**
  * @brief Enable auto preload for ICache.
  *        Please do not call this function in your SDK application.
  *
  * @param None
  *
  * @return None
  */
void Cache_Enable_ICache_Autoload(void);

/**
  * @brief Disable auto preload for ICache.
  *        Please do not call this function in your SDK application.
  *
  * @param None
  *
  * @return None
  */
void Cache_Disable_ICache_Autoload(void);

/**
  * @brief Config autoload parameters of DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param struct autoload_config * config : autoload parameters.
  *
  * @return None
  */
void Cache_Config_DCache_Autoload(const struct autoload_config * config);

/**
  * @brief Enable auto preload for DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param None
  *
  * @return None
  */
void Cache_Enable_DCache_Autoload(void);

/**
  * @brief Disable auto preload for DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param None
  *
  * @return None
  */
void Cache_Disable_DCache_Autoload(void);

/**
  * @brief Config a group of lock parameters of ICache.
  *        Please do not call this function in your SDK application.
  *
  * @param struct lock_config * config : a group of lock parameters.
  *
  * @return None
  */
void Cache_Enable_ICache_Lock(const struct lock_config *config);

/**
  * @brief Disable a group of lock parameters for ICache.
  *        However, the locked data will not be released.
  *        Please do not call this function in your SDK application.
  *
  * @param uint16_t group : 0 for group0, 1 for group1.
  *
  * @return None
  */
void Cache_Disable_ICache_Lock(uint16_t group);

/**
  * @brief Unlock the cache items in tag memory for ICache.
  *        Please do not call this function in your SDK application.
  *
  * @param uint32_t addr : start address of unlock region.
  *
  * @param uint32_t size : size of unlock region.
  *
  * @return None
  */
void Cache_Unlock_ICache(uint32_t addr, uint32_t size);

/**
  * @brief Config a group of lock parameters of DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param struct lock_config * config : a group of lock parameters.
  *
  * @return None
  */
void Cache_Enable_DCache_Lock(const struct lock_config *config);

/**
  * @brief Disable a group of lock parameters for DCache.
  *        However, the locked data will not be released.
  *        Please do not call this function in your SDK application.
  *
  * @param uint16_t group : 0 for group0, 1 for group1.
  *
  * @return None
  */
void Cache_Disable_DCache_Lock(uint16_t group);

/**
  * @brief Unlock the cache items in tag memory for DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param uint32_t addr : start address of unlock region.
  *
  * @param uint32_t size : size of unlock region.
  *
  * @return None
  */
void Cache_Unlock_DCache(uint32_t addr, uint32_t size);

/**
  * @brief Disable ICache access for the cpu.
  *        This operation will make all ICache tag memory invalid, CPU can't access ICache, ICache will keep idle.
  *        Please do not call this function in your SDK application.
  *
  * @return uint32_t : auto preload enabled before
  */
uint32_t Cache_Disable_ICache(void);

/**
  * @brief Enable ICache access for the cpu.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t autoload : ICache will preload then.
  *
  * @return None
  */
void Cache_Enable_ICache(uint32_t autoload);

/**
  * @brief Disable DCache access for the cpu. 
  *        This operation will make all DCache tag memory invalid, CPU can't access DCache, DCache will keep idle
  *        Please do not call this function in your SDK application.
  *
  * @return uint32_t : auto preload enabled before
  */
uint32_t Cache_Disable_DCache(void);

/**
  * @brief Enable DCache access for the cpu.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t autoload : DCache will preload then.
  *
  * @return None
  */
void Cache_Enable_DCache(uint32_t autoload);

/**
  * @brief Suspend ICache access for the cpu. 
  *        The ICache tag memory is still there, CPU can't access ICache, ICache will keep idle.
  *        Please do not change MMU, cache mode or tag memory(tag memory can be changed in some special case).
  *        Please do not call this function in your SDK application.
  *
  * @param  None
  *
  * @return uint32_t : auto preload enabled before
  */
uint32_t Cache_Suspend_ICache(void);

/**
  * @brief Resume ICache access for the cpu.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t autoload : ICache will preload then.
  *
  * @return None
  */
void Cache_Resume_ICache(uint32_t autoload);

/**
  * @brief Suspend DCache access for the cpu.
  *        The ICache tag memory is still there, CPU can't access DCache, DCache will keep idle.
  ×        Please do not change MMU, cache mode or tag memory(tag memory can be changed in some special case).
  *        Please do not call this function in your SDK application.
  *
  * @param  None
  *
  * @return uint32_t : auto preload enabled before
  */
uint32_t Cache_Suspend_DCache(void);

/**
  * @brief Resume DCache access for the cpu.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t autoload : DCache will preload then.
  *
  * @return None
  */
void Cache_Resume_DCache(uint32_t autoload);

/**
  * @brief Make Drom0 bus access from ICache.
  *
  * @param  None
  *
  * @return None
  */
void Cache_Drom0_Source_ICache(void);

/**
  * @brief Make Drom0 bus access from DCache.
  *
  * @param  None
  *
  * @return None
  */
void Cache_Drom0_Source_DCache(void);

/**
  * @brief Return if Drom0 bus access from ICache.
  *
  * @param  None
  *
  * @return uint32_t: 0 for no, other for yes
  */
uint32_t Cache_Drom0_Using_ICache(void);

/**
  * @brief Return if Drom0 bus access from DCache.
  *
  * @param  None
  *
  * @return uint32_t: 0 for no, other for yes
  */
uint32_t Cache_Drom0_Using_DCache(void);

/**
  * @brief Get ICache cache line size
  *
  * @param  None
  *
  * @return uint32_t: 16, 32, 64 Byte
  */
uint32_t Cache_Get_ICache_Line_Size(void);

/**
  * @brief Get DCache cache line size
  *
  * @param  None
  *
  * @return uint32_t: 16, 32, 64 Byte
  */
uint32_t Cache_Get_DCache_Line_Size(void);

/**
  * @brief Set default mode from boot.
  *
  * @param  None
  *
  * @return None
  */
void Cache_Set_Default_Mode(void);

/**
  * @brief Travel tag memory to run a call back function.
  *        ICache and DCache are suspend when doing this.
  *        The callback will get the parameter tag_group_info, which will include a group of tag memory addresses and cache memory addresses.
  *        Please do not call this function in your SDK application.
  *
  * @param  struct cache_mode * mode : the cache to check and the cache mode.
  *
  * @param  uint32_t filter_addr : only the cache lines which may include the filter_address will be returned to the call back function.
  *                                0 for do not filter, all cache lines will be returned.
  *
  * @param  void (* process)(struct tag_group_info *) : call back function, which may be called many times, a group(the addresses in the group are in the same position in the cache ways) a time.
  *
  * @return None
  */
void Cache_Travel_Tag_Memory(struct cache_mode * mode, uint32_t filter_addr, void (* process)(struct tag_group_info *));

/**
  * @brief Get the virtual address from cache mode, cache tag and the virtual address offset of cache ways.
  *        Please do not call this function in your SDK application.
  *
  * @param  struct cache_mode * mode : the cache to calculate the virtual address and the cache mode.
  *
  * @param  uint32_t tag : the tag part fo a tag item, 12-14 bits.
  *
  * @param  uint32_t addr_offset : the virtual address offset of the cache ways.
  *
  * @return uint32_t : the virtual address.
  */
uint32_t Cache_Get_Virtual_Addr(struct cache_mode *mode, uint32_t tag, uint32_t vaddr_offset);

/**
  * @brief Get cache memory block base address.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t icache : 0 for dcache, other for icache.
  *
  * @param  uint32_t high : 0 for low part block, 1 for high part block.
  *
  * @return uint32_t : the cache memory block base address, 0 if the block not used.
  */
uint32_t Cache_Get_Memory_BaseAddr(uint32_t icache, uint32_t high);

/**
  * @brief Get the cache memory address from cache mode, cache memory offset and the virtual address offset of cache ways.
  *        Please do not call this function in your SDK application.
  *
  * @param  struct cache_mode * mode : the cache to calculate the virtual address and the cache mode.
  *
  * @param  uint32_t cache_memory_offset : the cache memory offset of the whole cache (ICache or DCache) for the cache line.
  *
  * @param  uint32_t addr_offset : the virtual address offset of the cache ways.
  *
  * @return uint32_t : the virtual address.
  */
uint32_t Cache_Get_Memory_Addr(struct cache_mode *mode, uint32_t cache_memory_offset, uint32_t vaddr_offset);

/**
  * @brief Get the cache memory value by DRAM address.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t cache_memory_addr : DRAM address for the cache memory.
  *
  * @return uint32_t : the word value of the address.
  */
uint32_t Cache_Get_Memory_value(uint32_t cache_memory_addr);
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* _ROM_CACHE_H_ */
