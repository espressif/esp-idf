/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
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
#define MIN_ICACHE_SIZE         16384
#define MAX_ICACHE_SIZE         16384
#define MIN_ICACHE_WAYS         2
#define MAX_ICACHE_WAYS         2
#define MAX_CACHE_WAYS          2
#define MIN_CACHE_LINE_SIZE     32
#define TAG_SIZE                4
#define MIN_ICACHE_BANK_NUM     1
#define MAX_ICACHE_BANK_NUM     1
#define CACHE_MEMORY_BANK_NUM   1
#define CACHE_MEMORY_IBANK_SIZE 0x4000

#define MAX_ITAG_BANK_ITEMS             (MAX_ICACHE_SIZE / MAX_ICACHE_BANK_NUM / MIN_CACHE_LINE_SIZE)
#define MAX_ITAG_BLOCK_ITEMS            (MAX_ICACHE_SIZE / MAX_ICACHE_BANK_NUM / MAX_ICACHE_WAYS / MIN_CACHE_LINE_SIZE)
#define MAX_ITAG_BANK_SIZE              (MAX_ITAG_BANK_ITEMS * TAG_SIZE)
#define MAX_ITAG_BLOCK_SIZE             (MAX_ITAG_BLOCK_ITEMS * TAG_SIZE)

typedef enum {
    CACHE_SIZE_HALF = 0,                /*!< 8KB for icache and dcache */
    CACHE_SIZE_FULL = 1,                /*!< 16KB for icache and dcache */
} cache_size_t;

typedef enum {
    CACHE_4WAYS_ASSOC = 0,              /*!< 4 way associated cache */
    CACHE_8WAYS_ASSOC = 1,              /*!< 8 way associated cache */
} cache_ways_t;

typedef enum {
    CACHE_LINE_SIZE_16B = 0,            /*!< 16 Byte cache line size */
    CACHE_LINE_SIZE_32B = 1,            /*!< 32 Byte cache line size */
    CACHE_LINE_SIZE_64B = 2,            /*!< 64 Byte cache line size */
} cache_line_size_t;

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

typedef enum {
    CACHE_FREEZE_ACK_BUSY = 0,          /*!< in this mode, cache ack busy to CPU if a cache miss happens*/
    CACHE_FREEZE_ACK_ERROR  = 1,        /*!< in this mode, cache ack wrong data to CPU and trigger an error if a cache miss happens */
} cache_freeze_mode_t;

typedef enum {
    MMU_PAGE_MODE_64KB = 0,
    MMU_PAGE_MODE_32KB = 1,
    MMU_PAGE_MODE_16KB = 2,
    MMU_PAGE_MODE_8KB = 3,
    MMU_PAGE_MODE_INVALID,
} mmu_page_mode_t;

struct cache_mode {
    uint32_t cache_size;                /*!< cache size in byte */
    uint16_t cache_line_size;           /*!< cache line size in byte */
    uint8_t cache_ways;                 /*!< cache ways, always 4 */
    uint8_t ibus;                     /*!< the cache index, 0 for dcache, 1 for icache */
};

struct icache_tag_item {
    uint32_t valid:1;                   /*!< the tag item is valid or not */
    uint32_t lock:1;                    /*!< the cache line is locked or not */
    uint32_t fifo_cnt:3;                /*!< fifo cnt, 0 ~ 3 for 4 ways cache */
    uint32_t tag:13;                    /*!< the tag is the high part of the cache address, however is only 16MB (8MB Ibus + 8MB Dbus) range, and without low part */
    uint32_t reserved:14;
};

struct autoload_config {
    uint8_t order;                      /*!< autoload step is positive or negative */
    uint8_t trigger;                    /*!< autoload trigger */
    uint8_t ena0;                       /*!< autoload region0 enable */
    uint8_t ena1;                       /*!< autoload region1 enable */
    uint32_t addr0;                     /*!< autoload region0 start address */
    uint32_t size0;                     /*!< autoload region0 size */
    uint32_t addr1;                     /*!< autoload region1 start address */
    uint32_t size1;                     /*!< autoload region1 size */
};

struct tag_group_info {
    struct cache_mode mode;                         /*!< cache and cache mode */
    uint32_t filter_addr;                           /*!< the address that used to generate the struct */
    uint32_t vaddr_offset;                          /*!< virtual address offset of the cache ways */
    uint32_t tag_addr[MAX_CACHE_WAYS];              /*!< tag memory address, only [0~mode.ways-1] is valid to use */
    uint32_t cache_memory_offset[MAX_CACHE_WAYS];   /*!< cache memory address, only [0~mode.ways-1] is valid to use */
};

struct lock_config {
    uint32_t addr;                                  /*!< manual lock address*/
    uint16_t size;                                  /*!< manual lock size*/
    uint16_t group;                                 /*!< manual lock group, 0 or 1*/
};

struct cache_internal_stub_table {
    uint32_t (* icache_line_size)(void);
    uint32_t (* icache_addr)(uint32_t addr);
    uint32_t (* dcache_addr)(uint32_t addr);
    void (* invalidate_icache_items)(uint32_t addr, uint32_t items);
    void (* lock_icache_items)(uint32_t addr, uint32_t items);
    void (* unlock_icache_items)(uint32_t addr, uint32_t items);
    uint32_t (* suspend_icache_autoload)(void);
    void (* resume_icache_autoload)(uint32_t autoload);
    void (* freeze_icache_enable)(cache_freeze_mode_t mode);
    void (* freeze_icache_disable)(void);
    int (* op_addr)(uint32_t start_addr, uint32_t size, uint32_t cache_line_size, uint32_t max_sync_num, void(* cache_Iop)(uint32_t, uint32_t));
};

/* Defined in the interface file, default value is rom_default_cache_internal_table */
extern const struct cache_internal_stub_table* rom_cache_internal_table_ptr;

typedef void (* cache_op_start)(void);
typedef void (* cache_op_end)(void);

typedef struct {
    cache_op_start start;
    cache_op_end end;
} cache_op_cb_t;

/* Defined in the interface file, default value is NULL */
extern const cache_op_cb_t* rom_cache_op_cb;

#define ESP_ROM_ERR_INVALID_ARG         1
#define MMU_SET_ADDR_ALIGNED_ERROR      2
#define MMU_SET_PASE_SIZE_ERROR         3
#define MMU_SET_VADDR_OUT_RANGE         4

#define CACHE_OP_ICACHE_Y               1
#define CACHE_OP_ICACHE_N               0

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
  * @brief Init Cache for ROM boot, including resetting the Icache, initializing MMU, Enabling ICache, unmasking bus.
  *
  * @param None
  *
  * @return None
  */
void ROM_Boot_Cache_Init(void);

/**
  * @brief Set ICache mmu mapping.
  *        Please do not call this function in your SDK application.
  *
  * @param uint32_t senitive : Config this page should apply flash encryption or not
  *
  * @param uint32_t ext_ram : DPORT_MMU_ACCESS_FLASH for flash, DPORT_MMU_INVALID for invalid. In
  *                 esp32h21, external memory is always flash
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
  *                   2 : vaddr or paddr is not aligned
  *                   3 : psize error
  *                   4 : vaddr is out of range
  */
int Cache_MSPI_MMU_Set(uint32_t sensitive, uint32_t ext_ram, uint32_t vaddr, uint32_t paddr,  uint32_t psize, uint32_t num, uint32_t fixed);

/**
  * @brief Set DCache mmu mapping.
  *        Please do not call this function in your SDK application.
  *
  * @param uint32_t ext_ram : DPORT_MMU_ACCESS_FLASH for flash, DPORT_MMU_INVALID for invalid. In
  *                 esp32c3, external memory is always flash
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
  *                   2 : vaddr or paddr is not aligned
  *                   3 : psize error
  *                   4 : vaddr is out of range
  */
int Cache_Dbus_MMU_Set(uint32_t ext_ram, uint32_t vaddr, uint32_t paddr, uint32_t psize, uint32_t num, uint32_t fixed);

/**
  * @brief Get cache mode of ICache or DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param struct cache_mode * mode : the pointer of cache mode struct, caller should set the icache field
  *
  * return none
  */
void Cache_Get_Mode(struct cache_mode * mode);

/**
  * @brief Set cache page mode.
  *
  * @param mmu_page_mode_t
  *
  * @return None
  */
void MMU_Set_Page_Mode(mmu_page_mode_t pg_mode);

/**
  * @brief Get cache page mode.
  *
  * @param None
  *
  * @return page mode
  */
mmu_page_mode_t MMU_Get_Page_Mode(void);

/**
  * @brief Invalidate the cache items for ICache.
  *        Operation will be done CACHE_LINE_SIZE aligned.
  *        If the region is not in ICache addr room, nothing will be done.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t addr: start address to invalidate
  *
  * @param  uint32_t items: cache lines to invalidate, items * cache_line_size should not exceed the bus address size(16MB/32MB/64MB)
  *
  * @return None
  */
void Cache_Invalidate_ICache_Items(uint32_t addr, uint32_t items);

/**
  * @brief Invalidate the Cache items in the region from ICache or DCache.
  *        If the region is not in Cache addr room, nothing will be done.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t addr : invalidated region start address.
  *
  * @param  uint32_t size : invalidated region size.
  *
  * @return 0 for success
  *         1 for invalid argument
  */
int Cache_Invalidate_Addr(uint32_t addr, uint32_t size);

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
  * @brief Mask all buses through ICache and DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param  None
  *
  * @return None
  */
void Cache_Mask_All(void);

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
  * @brief Config a group of prelock parameters of ICache.
  *        Please do not call this function in your SDK application.
  *
  * @param struct lock_config * config : a group of lock parameters.
  *
  * @return None
  */

void Cache_Enable_ICache_PreLock(const struct lock_config *config);

/**
  * @brief Disable a group of prelock parameters for ICache.
  *        However, the locked data will not be released.
  *        Please do not call this function in your SDK application.
  *
  * @param uint16_t group : 0 for group0, 1 for group1.
  *
  * @return None
  */
void Cache_Disable_ICache_PreLock(uint16_t group);

/**
  * @brief Lock the cache items for ICache.
  *        Operation will be done CACHE_LINE_SIZE aligned.
  *        If the region is not in ICache addr room, nothing will be done.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t addr: start address to lock
  *
  * @param  uint32_t items: cache lines to lock, items * cache_line_size should not exceed the bus address size(16MB/32MB/64MB)
  *
  * @return None
  */
void Cache_Lock_ICache_Items(uint32_t addr, uint32_t items);

/**
  * @brief Unlock the cache items for ICache.
  *        Operation will be done CACHE_LINE_SIZE aligned.
  *        If the region is not in ICache addr room, nothing will be done.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t addr: start address to unlock
  *
  * @param  uint32_t items: cache lines to unlock, items * cache_line_size should not exceed the bus address size(16MB/32MB/64MB)
  *
  * @return None
  */
void Cache_Unlock_ICache_Items(uint32_t addr, uint32_t items);

/**
  * @brief Lock the cache items in tag memory for ICache or DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param uint32_t addr : start address of lock region.
  *
  * @param uint32_t size : size of lock region.
  *
  * @return 0 for success
  *         1 for invalid argument
  */
int Cache_Lock_Addr(uint32_t addr, uint32_t size);

/**
  * @brief Unlock the cache items in tag memory for ICache or DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param uint32_t addr : start address of unlock region.
  *
  * @param uint32_t size : size of unlock region.
  *
  * @return 0 for success
  *         1 for invalid argument
  */
int Cache_Unlock_Addr(uint32_t addr, uint32_t size);

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
  * @brief Get ICache cache line size
  *
  * @param  None
  *
  * @return uint32_t: 16, 32, 64 Byte
  */
uint32_t Cache_Get_ICache_Line_Size(void);

/**
  * @brief Enable freeze for ICache.
  *        Any miss request will be rejected, including cpu miss and preload/autoload miss.
  *        Please do not call this function in your SDK application.
  *
  * @param cache_freeze_mode_t mode : 0 for assert busy 1 for assert hit
  *
  * @return None
  */
void Cache_Freeze_ICache_Enable(cache_freeze_mode_t mode);

/**
  * @brief Disable freeze for ICache.
  *        Please do not call this function in your SDK application.
  *
  * @return None
  */
void Cache_Freeze_ICache_Disable(void);

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
  * @param  uint32_t tag : the tag part of a tag item, 12-14 bits.
  *
  * @param  uint32_t addr_offset : the virtual address offset of the cache ways.
  *
  * @return uint32_t : the virtual address.
  */
uint32_t Cache_Get_Virtual_Addr(struct cache_mode *mode, uint32_t tag, uint32_t vaddr_offset);

/**
  * @}
  */

/**
  * @brief Get the cache MMU IROM end address.
  *        Please do not call this function in your SDK application.
  *
  * @param  void
  *
  * @return uint32_t : the word value of the address.
  */
uint32_t Cache_Get_IROM_MMU_End(void);

/**
  * @brief Get the cache MMU DROM end address.
  *        Please do not call this function in your SDK application.
  *
  * @param  void
  *
  * @return uint32_t : the word value of the address.
  */
uint32_t Cache_Get_DROM_MMU_End(void);

/**
 * @brief Configure cache MMU page size according to instruction and rodata size
 *
 * @param irom_size The instruction cache MMU page size
 * @param drom_size The rodata data cache MMU page size
 */
void Cache_Set_IDROM_MMU_Size(uint32_t irom_size, uint32_t drom_size);

#define Cache_Dbus_MMU_Set(ext_ram, vaddr, paddr, psize, num, fixed) \
    Cache_MSPI_MMU_Set(ets_efuse_cache_encryption_enabled() ? SOC_MMU_SENSITIVE : 0, ext_ram, vaddr, paddr, psize, num, fixed)

#ifdef __cplusplus
}
#endif
