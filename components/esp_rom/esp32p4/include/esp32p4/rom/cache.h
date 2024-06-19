/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ROM_CACHE_H_
#define _ROM_CACHE_H_

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
#define TAG_SIZE 4
// L1 icache
/* basic param
 */
#define MAX_L1_ICACHE_SIZE      (16 * 1024)
#define MAX_L1_ICACHE_WAY       4
#define MAX_L1_ICACHE_BANK_NUM  1
#define MIN_L1_ICACHE_LINE_SIZE 64
/* cal param
 */
#define MAX_L1_ITAG_BANK_ITEMS     (MAX_L1_ICACHE_SIZE / MAX_L1_ICACHE_BANK_NUM / MIN_L1_ICACHE_LINE_SIZE)
#define MAX_L1_ITAG_BANK_WAY_ITEMS (MAX_L1_ITAG_BANK_ITEMS / MAX_L1_ICACHE_WAY)
#define MAX_L1_ITAG_BANK_SIZE      (MAX_L1_ITAG_BANK_ITEMS * TAG_SIZE)
#define MAX_L1_ITAG_BANK_WAY_SIZE  (MAX_L1_ITAG_BANK_WAY_ITEMS * TAG_SIZE)

// L1 dcache
/* basic param
 */
#define MAX_L1_DCACHE_SIZE      (64 * 1024)
#define MAX_L1_DCACHE_WAY       2
#define MAX_L1_DCACHE_BANK_NUM  4
#define MIN_L1_DCACHE_LINE_SIZE 64
/* cal param
 */
#define MAX_L1_DTAG_BANK_ITEMS     (MAX_L1_DCACHE_SIZE / MAX_L1_DCACHE_BANK_NUM / MIN_L1_DCACHE_LINE_SIZE)
#define MAX_L1_DTAG_BANK_WAY_ITEMS (MAX_L1_DTAG_BANK_ITEMS / MAX_L1_DCACHE_WAY)
#define MAX_L1_DTAG_BANK_SIZE      (MAX_L1_DTAG_BANK_ITEMS * TAG_SIZE)
#define MAX_L1_DTAG_BANK_WAY_SIZE  (MAX_L1_DTAG_BANK_WAY_ITEMS * TAG_SIZE)

// L2 cache
/* basic param
 */
#define MAX_L2_CACHE_SIZE     (256 * 1024)
#define MAX_L2_CACHE_WAY      8
#define MAX_L2_CACHE_BANK_NUM 2
// l2 cache line size only has 32/64B, 16 here used to calc tag in memory_access.c
#define MIN_L2_CACHE_LINE_SIZE 64
/* cal param
 */
#define MAX_L2_TAG_BANK_ITEMS     (MAX_L2_CACHE_SIZE / MAX_L2_CACHE_BANK_NUM / MIN_L2_CACHE_LINE_SIZE)
#define MAX_L2_TAG_BANK_WAY_ITEMS (MAX_L2_TAG_BANK_ITEMS / MAX_L2_CACHE_WAY)
#define MAX_L2_TAG_BANK_SIZE      (MAX_L2_TAG_BANK_ITEMS * TAG_SIZE)
#define MAX_L2_TAG_BANK_WAY_SIZE  (MAX_L2_TAG_BANK_WAY_ITEMS * TAG_SIZE)

/*
 * Simple MMU related defines
 */
#define SMMU_GID_MIN 0
#define SMMU_GID_MAX 15
/*
 * GID = 0 is a special group id, when accessing l2 memory, all accesses are treated as GID=0
 * GID = 1...15, are used in MSPI
 */
#define SMMU_GID_DEFAULT     0
#define SMMU_ENTRY_INDEX_LOW 0
// should NOT =
#define SMMU_ENTRY_INDEX_HIGH 1024
#define SMMU_ENTRY_NUM        (SMMU_ENTRY_INDEX_HIGH - SMMU_ENTRY_INDEX_LOW)

#define SMMU_SHARED_TBIT_INDEX  1024
#define SMMU_GID_TBIT_NUM       16
#define SMMU_GID_TBIT_INDEX_LOW 1025
// should NOT =
#define SMMU_GID_TBIT_INDEX_HIGH (SMMU_GID_TBIT_INDEX_LOW + SMMU_GID_TBIT_NUM)

typedef enum {
    CACHE_L1_ICACHE0 = 0,
    CACHE_L1_ICACHE1 = 1,
    CACHE_L1_DCACHE = 4,
    CACHE_L2_CACHE = 5,
} cache_t;

typedef enum {
    CACHE_SIZE_128K = 9,
    CACHE_SIZE_256K = 10,
    CACHE_SIZE_512K = 11,
} cache_size_t;

typedef enum {
    CACHE_2WAYS_ASSOC = 0, /*!< 2 way associated cache */
    CACHE_4WAYS_ASSOC = 1, /*!< 4 way associated cache */
    CACHE_8WAYS_ASSOC = 2, /*!< 8 way associated cache */
} cache_ways_t;

typedef enum {
    CACHE_LINE_SIZE_64B = 3,  /*!< 64 Byte cache line size */
    CACHE_LINE_SIZE_128B = 4, /*!< 128 Byte cache line size */
} cache_line_size_t;

typedef enum {
    CACHE_AUTOLOAD_POSITIVE = 0, /*!< cache autoload step is positive */
    CACHE_AUTOLOAD_NEGATIVE = 1, /*!< cache autoload step is negative */
} cache_autoload_order_t;

#define CACHE_AUTOLOAD_STEP(i) ((i)-1)

typedef enum {
    CACHE_AUTOLOAD_MISS_TRIGGER = 0, /*!< autoload only triggered by cache miss */
    CACHE_AUTOLOAD_HIT_TRIGGER = 1,  /*!< autoload only triggered by cache hit */
    CACHE_AUTOLOAD_BOTH_TRIGGER = 2, /*!< autoload triggered both by cache miss and hit */
} cache_autoload_trigger_t;

typedef enum {
    CACHE_FREEZE_ACK_BUSY = 0,  /*!< in this mode, cache ack busy to CPU if a cache miss happens*/
    CACHE_FREEZE_ACK_ERROR = 1, /*!< in this mode, cache ack wrong data to CPU and trigger an error if a cache miss happens */
} cache_freeze_mode_t;

struct cache_mode {
    uint32_t cache_size;      /*!< cache size in byte */
    uint16_t cache_line_size; /*!< cache line size in byte */
    uint8_t cache_ways;       /*!< cache ways, always 4 */
    cache_t cache_type;       /*!< the cache type indicated by cache_t*/
};

// l1 itag
struct l1_itag_item {
    uint32_t valid : 1;
    uint32_t lock : 1;
    uint32_t tag : 16;
    uint32_t gid : 4;
    uint32_t replace : 2;
    uint32_t reserved : 8;
};

// l1 dtag
struct l1_dtag_item {
    uint32_t dirty : 1;
    uint32_t valid : 1;
    uint32_t lock : 1;
    uint32_t tag : 13;
    uint32_t gid : 4;
    uint32_t replace : 1;
    uint32_t reserved : 11;
};

// l2 tag
struct l2_tag_item {
    uint32_t dirty : 1;
    uint32_t valid : 1;
    uint32_t lock : 1;
    uint32_t tag : 14;
    uint32_t gid : 4;
    uint32_t replace : 1;
    uint32_t reserved : 10;
};

struct l1_icache_autoload_config {
    uint8_t gid;
    uint8_t order;
    uint8_t trigger;
    uint8_t ena0;
    uint8_t ena1;
    uint32_t addr0;
    uint32_t size0;
    uint32_t addr1;
    uint32_t size1;
};

// l1 dcache and l2 cache both have 4 autoload regions
struct l1_dcache_l2_autoload_config {
    uint8_t gid;
    uint8_t order;
    uint8_t trigger;
    uint8_t ena0;
    uint8_t ena1;
    uint8_t ena2;
    uint8_t ena3;
    uint32_t addr0;
    uint32_t size0;
    uint32_t addr1;
    uint32_t size1;
    uint32_t addr2;
    uint32_t size2;
    uint32_t addr3;
    uint32_t size3;
};

struct tag_group_info {
    int use_legacy; /*!< 0 for new tag api, 1 for old tag api*/
    struct cache_mode mode;
    uint32_t filter_addr;
    uint32_t vaddr_offset;
    uint32_t tag_addr[MAX_L2_CACHE_WAY];
};

struct lock_config {
    uint32_t gid;
    uint32_t addr;  /*!< manual lock address*/
    uint16_t size;  /*!< manual lock size*/
    uint16_t group; /*!< manual lock group, 0 or 1*/
};

typedef enum {
    CACHE_LOCK_LOCK = BIT(0),
    CACHE_LOCK_UNLOCK = BIT(1),
} cache_lock_t;

typedef enum {
    CACHE_SYNC_INVALIDATE = BIT(0),
    CACHE_SYNC_CLEAN = BIT(1),
    CACHE_SYNC_WRITEBACK = BIT(2),
    CACHE_SYNC_WRITEBACK_INVALIDATE = BIT(3),
} cache_sync_t;

#define CACHE_MAP_L1_ICACHE_0 BIT(0)
#define CACHE_MAP_L1_ICACHE_1 BIT(1)
#define CACHE_MAP_L1_DCACHE   BIT(4)
#define CACHE_MAP_L2_CACHE    BIT(5)

#define CACHE_MAP_L1_ICACHE_MASK (CACHE_MAP_L1_ICACHE_0 | CACHE_MAP_L1_ICACHE_1)
#define CACHE_MAP_MASK           (CACHE_MAP_L1_ICACHE_MASK | CACHE_MAP_L1_DCACHE | CACHE_MAP_L2_CACHE)

struct cache_internal_stub_table {
    uint32_t (*l1_icache_line_size)(void);
    uint32_t (*l1_dcache_line_size)(void);
    uint32_t (*l2_cache_line_size)(void);
    uint32_t (*cache_addr)(uint32_t addr);
    // sync, type: invalidate/clean/writeback map: l1i0~3/l1d/l2
    void (*sync_cache_items)(uint32_t gid, uint32_t type, uint32_t map, uint32_t addr, uint32_t bytes);
    // lock: 0 -> unlock op, 1 -> lock op
    void (*lock_cache_items)(uint32_t gid, uint32_t lock, uint32_t map, uint32_t addr, uint32_t bytes);
    //autoload
    uint32_t (*suspend_l1_icache0_autoload)(void);
    void (*resume_l1_icache0_autoload)(uint32_t autoload);
    uint32_t (*suspend_l1_icache1_autoload)(void);
    void (*resume_l1_icache1_autoload)(uint32_t autoload);
    uint32_t (*suspend_l1_dcache_autoload)(void);
    void (*resume_l1_dcache_autoload)(uint32_t autoload);
    uint32_t (*suspend_l2_cache_autoload)(void);
    void (*resume_l2_cache_autoload)(uint32_t autoload);
    // freeze
    void (*freeze_l1_icache0_enable)(cache_freeze_mode_t mode);
    void (*freeze_l1_icache0_disable)(void);
    void (*freeze_l1_icache1_enable)(cache_freeze_mode_t mode);
    void (*freeze_l1_icache1_disable)(void);
    void (*freeze_l1_dcache_enable)(cache_freeze_mode_t mode);
    void (*freeze_l1_dcache_disable)(void);
    void (*freeze_l2_cache_enable)(cache_freeze_mode_t mode);
    void (*freeze_l2_cache_disable)(void);
    // op
    int (*op_addr)(uint32_t gid,
                   uint32_t op_type,
                   uint32_t map,
                   uint32_t start_addr,
                   uint32_t size,
                   uint32_t cache_line_size,
                   uint32_t max_sync_num,
                   void (*cache_op)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t));
};

typedef void (*cache_op_start)(void);
typedef void (*cache_op_end)(void);

typedef struct {
    cache_op_start start;
    cache_op_end end;
} cache_op_cb_t;

extern const struct cache_internal_stub_table *rom_cache_internal_table_ptr;
extern const cache_op_cb_t *rom_cache_op_cb;

#define ESP_ROM_ERR_INVALID_ARG    1
#define MMU_SET_ADDR_ALIGNED_ERROR 2
#define MMU_SET_PASE_SIZE_ERROR    3
#define MMU_SET_VADDR_OUT_RANGE    4

/**
  * @brief Initialise cache mmu, mark all entries as invalid.
  *        Please do not call this function in your SDK application.
  *
  * @param  None
  *
  * @return None
  */
void Cache_FLASH_MMU_Init(void);

/**
  * @brief Initialise cache mmu, mark all entries as invalid.
  *        Please do not call this function in your SDK application.
  *
  * @param  None
  *
  * @return None
  */
void Cache_PSRAM_MMU_Init(void);

/**
  * @brief Set ICache mmu mapping.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t sensitive : Need encryption/ecc check for the page
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
int Cache_FLASH_MMU_Set(uint32_t sensitive, uint32_t vaddr, uint32_t paddr, uint32_t psize, uint32_t num, uint32_t fixed);

/**
  * @brief Set Cache mmu mapping for dual_bank mspi.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t sensitive : Need encryption/ecc check for the page
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
int Cache_PSRAM_MMU_Set(uint32_t sensitive, uint32_t vaddr, uint32_t paddr, uint32_t psize, uint32_t num, uint32_t fixed);

/**
  * @brief Wrapper for Cache_MSPI_MMU_Set, which sets the sensitive bit according to flash encryption efuse
  *        Please do not call this function in your SDK application.
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
int Cache_FLASH_MMU_Set_Secure(uint32_t vaddr, uint32_t paddr, uint32_t psize, uint32_t num, uint32_t fixed);

/**
  * @brief Wrapper for Cache_DB_MSPI_MMU_Set, which sets the sensitive bit according to flash encryption efuse
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t ext_ram : MMU_MSPI_ACCESS_FLASH for flash, MMU_MSPI_ACCESS_SPIRAM for spiram, DPORT_MMU_INVALID for invalid.
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
int Cache_PSRAM_MMU_Set_Secure(uint32_t vaddr, uint32_t paddr, uint32_t psize, uint32_t num, uint32_t fixed);

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
uint32_t Cache_Count_Flash_Pages(uint32_t *page0_mapped);

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
uint32_t Cache_Flash_To_SPIRAM_Copy(uint32_t bus_start_addr, uint32_t start_page, uint32_t *page0_page);

/**
  * @brief Get cache mode of L1 cache/L2 cache.
  *        Please do not call this function in your SDK application.
  *
  * @param struct cache_mode * mode : the pointer of cache mode struct, caller should set the icache field
  *
  * return none
  */
void Cache_Get_Mode(struct cache_mode *mode);

/**
  * @brief set L2 Cache modes: cache size, associate ways and cache line size.
  *        Please do not call this function in your SDK application.
  *
  * @param cache_size_t cache_size : the cache size, can be CACHE_SIZE_128K/CACHE_SIZE_256K
  *
  * @param cache_ways_t ways : the associate ways of cache, can be CACHE_4WAYS_ASSOC and CACHE_8WAYS_ASSOC
  *
  * @param cache_line_size_t cache_line_size : the cache line size, can be CACHE_LINE_SIZE_32B/CACHE_LINE_SIZE_64B
  *
  * return none
  */
void Cache_Set_L2_Cache_Mode(cache_size_t cache_size, cache_ways_t ways, cache_line_size_t cache_line_size);

/**
  * @brief check if the address is accessed through Cache.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t addr : the address to check.
  *
  * @return 1 if the address is accessed through ICache, 0 if not.
  */
uint32_t Cache_Address_Through_Cache(uint32_t addr);

/**
  * @brief Init l1 Cache in early time, used to speed up rom access time.
  *
  * @param None
  *
  * @return None
  */
void ROM_L1_Cache_Init(void);

/**
  * @brief Init Cache for ROM boot, including resetting the Dcache, initializing Owner, MMU, setting DCache mode, Enabling DCache, unmasking bus.
  *
  * @param None
  *
  * @return None
  */
void ROM_Boot_Cache_Init(void);

/**
  * @brief Init Cache for ROM direct boot, including resetting the I/Dcache, initializing Owner, MMU, setting I/DCache mode, Enabling I/DCache, unmasking bus.
  *
  * @param None
  *
  * @return None
  */
void ROM_Direct_Boot_Cache_Init(void);

/**
  * @brief Init MMU for ROM direct boot, including mapping all ibus region and dbus region
  *
  * @param None
  *
  * @return 0 if mmu map is successfully, others if not.
  */
int ROM_Direct_Boot_MMU_Init(void);

/**
  * @brief After cache reset (hw or sw), wait cache sync done until it can start work.
  *
  * @param None
  *
  * @return None
  */
void Cache_Wait_Idle(void);

/**
  * @brief Cache address operation wrapper for sync and lock operation.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t op_type: operation type
  *
  * @param  uint32_t map: operation which cache
  *
  * @param  uint32_t start_addr: operation start address
  *
  * @param  uint32_t cache_line_size: corresponding cache line size to op_icache
  *
  * @param  uint32_t max_sync_num: max sync cache line count in one operation. Can be CACHE_MAX_SYNC_NUM or CACHE_MAX_LOCK_NUM
  *
  * @param  void(* cache_op)(uint32_t, uint32_t, uint32_t, uint32_t): cache operation hook
  *
  * @return 0 for success
  *         1 for invalid argument
  */
int Cache_Op_Addr(uint32_t gid,
                  uint32_t op_type,
                  uint32_t map,
                  uint32_t start_addr,
                  uint32_t size,
                  uint32_t cache_line_size,
                  uint32_t max_sync_num,
                  void (*cache_op)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t));

/**
  * @brief Invalidate the Cache items in the region from Cache.
  *        If the region is not in Cache addr room, nothing will be done.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t map  : operation which cache
  *
  * @param  uint32_t addr : invalidated region start address.
  *
  * @param  uint32_t size : invalidated region size.
  *
  * @return 0 for success
  *         1 for invalid argument
  */
int Cache_Invalidate_Addr(uint32_t map, uint32_t addr, uint32_t size);

/**
  * @brief Invalidate the Cache items in the region from Cache.
  *        If the region is not in Cache addr room, nothing will be done.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t map  : operation which cache
  *
  * @param  uint32_t addr : invalidated region start address.
  *
  * @param  uint32_t size : invalidated region size.
  *
  * @return 0 for success
  *         1 for invalid argument
  */
int Cache_Invalidate_Addr_Gid(uint32_t gid, uint32_t map, uint32_t addr, uint32_t size);

/**
  * @brief Clean the dirty bit of Cache items in the region from Cache.
  *        If the region is not in Cache addr room, nothing will be done.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t map  : operation which cache
  *
  * @param  uint32_t addr : cleaned region start address.
  *
  * @param  uint32_t size : cleaned region size.
  *
  * @return 0 for success
  *         1 for invalid argument
  */
int Cache_Clean_Addr(uint32_t map, uint32_t addr, uint32_t size);

/**
  * @brief Clean the dirty bit of Cache items in the region from Cache.
  *        If the region is not in Cache addr room, nothing will be done.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t map  : operation which cache
  *
  * @param  uint32_t addr : cleaned region start address.
  *
  * @param  uint32_t size : cleaned region size.
  *
  * @return 0 for success
  *         1 for invalid argument
  */
int Cache_Clean_Addr_Gid(uint32_t gid, uint32_t map, uint32_t addr, uint32_t size);

/**
  * @brief Writeback the Cache items(also clean the dirty bit) in the region from Cache.
  *        If the region is not in Cache addr room, nothing will be done.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t map  : operation which cache
  *
  * @param  uint32_t addr : writeback region start address.
  *
  * @param  uint32_t size : writeback region size.
  *
  * @return 0 for success
  *         1 for invalid argument
  */
int Cache_WriteBack_Addr(uint32_t map, uint32_t addr, uint32_t size);

/**
  * @brief Writeback the Cache items(also clean the dirty bit) in the region from Cache.
  *        If the region is not in Cache addr room, nothing will be done.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t map  : operation which cache
  *
  * @param  uint32_t addr : writeback region start address.
  *
  * @param  uint32_t size : writeback region size.
  *
  * @return 0 for success
  *         1 for invalid argument
  */
int Cache_WriteBack_Addr_Gid(uint32_t gid, uint32_t map, uint32_t addr, uint32_t size);

/**
  * @brief Writeback and invalidate the Cache items(also clean the dirty bit) in the region from Cache.
  *        If the region is not in Cache addr room, nothing will be done.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t map  : operation which cache
  *
  * @param  uint32_t addr : writeback region start address.
  *
  * @param  uint32_t size : writeback region size.
  *
  * @return 0 for success
  *         1 for invalid argument
  */
int Cache_WriteBack_Invalidate_Addr(uint32_t map, uint32_t addr, uint32_t size);

/**
  * @brief Writeback and invalidate the Cache items(also clean the dirty bit) in the region from Cache.
  *        If the region is not in Cache addr room, nothing will be done.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t map  : operation which cache
  *
  * @param  uint32_t addr : writeback region start address.
  *
  * @param  uint32_t size : writeback region size.
  *
  * @return 0 for success
  *         1 for invalid argument
  */
int Cache_WriteBack_Invalidate_Addr_Gid(uint32_t gid, uint32_t map, uint32_t addr, uint32_t size);

/**
  * @brief Invalidate all cache items in ICache.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t map : operation which cache
  *
  * @return 0 for success
  *         1 for invalid argument
  *
  * Note: ALAP to use this api in esp32p4, use *Cache_Invalidate_Addr* instead!
  */
int Cache_Invalidate_All(uint32_t map);

/**
  * @brief Invalidate all cache items in ICache.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t map : operation which cache
  *
  * @return 0 for success
  *         1 for invalid argument
  *
  * Note: ALAP to use this api in esp32p4, use *Cache_Invalidate_Addr* instead!
  */
int Cache_Invalidate_All_Gid(uint32_t gid, uint32_t map);

/**
  * @brief Clean the dirty bit of all cache items in DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t map : operation which cache
  *
  * @return 0 for success
  *         1 for invalid argument
  */
int Cache_Clean_All(uint32_t map);

/**
  * @brief Clean the dirty bit of all cache items in DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t map : operation which cache
  *
  * @return 0 for success
  *         1 for invalid argument
  */
int Cache_Clean_All_Gid(uint32_t gid, uint32_t map);

/**
  * @brief WriteBack all cache items in DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t map : operation which cache
  *
  * @return 0 for success
  *         1 for invalid argument
  */
int Cache_WriteBack_All(uint32_t map);

/**
  * @brief WriteBack all cache items in DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t map : operation which cache
  *
  * @return 0 for success
  *         1 for invalid argument
  */
int Cache_WriteBack_All_Gid(uint32_t gid, uint32_t map);

/**
  * @brief WriteBack all cache items in DCache during boot period.
  *        Please do not call this function in your SDK application.
  *
  * @param  None
  *
  * @return 0 for success
  *         1 for invalid argument
  */
int ROM_Boot_Cache_WriteBack(void);

/**
  * @brief WriteBack all cache items in DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t map : operation which cache
  *
  * @return 0 for success
  *         1 for invalid argument
  */
int Cache_WriteBack_Invalidate_All(uint32_t map);

/**
  * @brief WriteBack all cache items in DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t map : operation which cache
  *
  * @return 0 for success
  *         1 for invalid argument
  */
int Cache_WriteBack_Invalidate_All_Gid(uint32_t gid, uint32_t map);

/**
  * @brief Mask all buses through L1 Cache and L2 Cache.
  *        Please do not call this function in your SDK application.
  *
  * @param  None
  *
  * @return None
  */
void Cache_Mask_All(void);

/**
  * @brief Suspend L1 Core0 ICache auto preload operation, then you can resume it after some ICache operations.
  *        Please do not call this function in your SDK application.
  *
  * @param  None
  *
  * @return uint32_t : 0 for ICache not auto preload before suspend.
  */
uint32_t Cache_Suspend_L1_CORE0_ICache_Autoload(void);

/**
  * @brief Resume L1 Core0 ICache auto preload operation after some ICache operations.
  *        Please do not call this function in your SDK application.
  *
  * @param uint32_t autoload : 0 for ICache not auto preload before suspend.
  *
  * @return None.
  */
void Cache_Resume_L1_CORE0_ICache_Autoload(uint32_t autoload);

/**
  * @brief Suspend L1 Core1 ICache auto preload operation, then you can resume it after some ICache operations.
  *        Please do not call this function in your SDK application.
  *
  * @param  None
  *
  * @return uint32_t : 0 for ICache not auto preload before suspend.
  */
uint32_t Cache_Suspend_L1_CORE1_ICache_Autoload(void);

/**
  * @brief Resume L1 Core0 ICache auto preload operation after some ICache operations.
  *        Please do not call this function in your SDK application.
  *
  * @param uint32_t autoload : 0 for ICache not auto preload before suspend.
  *
  * @return None.
  */
void Cache_Resume_L1_CORE1_ICache_Autoload(uint32_t autoload);

/**
  * @brief Suspend L1 DCache auto preload operation, then you can resume it after some ICache operations.
  *        Please do not call this function in your SDK application.
  *
  * @param  None
  *
  * @return uint32_t : 0 for ICache not auto preload before suspend.
  */
uint32_t Cache_Suspend_L1_DCache_Autoload(void);

/**
  * @brief Resume L1 DCache auto preload operation after some ICache operations.
  *        Please do not call this function in your SDK application.
  *
  * @param uint32_t autoload : 0 for ICache not auto preload before suspend.
  *
  * @return None.
  */
void Cache_Resume_L1_DCache_Autoload(uint32_t autoload);

/**
  * @brief Suspend L2 Cache auto preload operation, then you can resume it after some ICache operations.
  *        Please do not call this function in your SDK application.
  *
  * @param  None
  *
  * @return uint32_t : 0 for ICache not auto preload before suspend.
  */
uint32_t Cache_Suspend_L2_Cache_Autoload(void);

/**
  * @brief Resume L2 Cache auto preload operation after some ICache operations.
  *        Please do not call this function in your SDK application.
  *
  * @param uint32_t autoload : 0 for ICache not auto preload before suspend.
  *
  * @return None.
  */
void Cache_Resume_L2_Cache_Autoload(uint32_t autoload);

/**
  * @brief Start an L1 Core0 ICache manual preload, will suspend auto preload of ICache.
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
uint32_t Cache_Start_L1_CORE0_ICache_Preload(uint32_t addr, uint32_t size, uint32_t order);

/**
  * @brief Return if the ICache manual preload done.
  *        Please do not call this function in your SDK application.
  *
  * @param  None
  *
  * @return uint32_t : 0 for ICache manual preload not done.
  */
uint32_t Cache_L1_CORE0_ICache_Preload_Done(void);

/**
  * @brief End the ICache manual preload to resume auto preload of ICache.
  *        Please do not call this function in your SDK application.
  *
  * @param uint32_t autoload : 0 for ICache not auto preload before manual preload.
  *
  * @return None
  */
void Cache_End_L1_CORE0_ICache_Preload(uint32_t autoload);

/**
  * @brief Start an L1 Core1 ICache manual preload, will suspend auto preload of DCache.
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
uint32_t Cache_Start_L1_CORE1_ICache_Preload(uint32_t addr, uint32_t size, uint32_t order);

/**
  * @brief Return if the DCache manual preload done.
  *        Please do not call this function in your SDK application.
  *
  * @param  None
  *
  * @return uint32_t : 0 for DCache manual preload not done.
  */
uint32_t Cache_L1_CORE1_ICache_Preload_Done(void);

/**
  * @brief End the DCache manual preload to resume auto preload of DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param uint32_t autoload : 0 for DCache not auto preload before manual preload.
  *
  * @return None
  */
void Cache_End_L1_CORE1_ICache_Preload(uint32_t autoload);

/**
  * @brief Start an L1 DCache manual preload, will suspend auto preload of DCache.
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
uint32_t Cache_Start_L1_DCache_Preload(uint32_t addr, uint32_t size, uint32_t order);

/**
  * @brief Return if the DCache manual preload done.
  *        Please do not call this function in your SDK application.
  *
  * @param  None
  *
  * @return uint32_t : 0 for DCache manual preload not done.
  */
uint32_t Cache_L1_DCache_Preload_Done(void);

/**
  * @brief End the DCache manual preload to resume auto preload of DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param uint32_t autoload : 0 for DCache not auto preload before manual preload.
  *
  * @return None
  */
void Cache_End_L1_DCache_Preload(uint32_t autoload);

/**
  * @brief Start an L2 Cache manual preload, will suspend auto preload of DCache.
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
uint32_t Cache_Start_L2_Cache_Preload(uint32_t addr, uint32_t size, uint32_t order);

/**
  * @brief Return if the DCache manual preload done.
  *        Please do not call this function in your SDK application.
  *
  * @param  None
  *
  * @return uint32_t : 0 for DCache manual preload not done.
  */
uint32_t Cache_L2_Cache_Preload_Done(void);

/**
  * @brief End the DCache manual preload to resume auto preload of DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param uint32_t autoload : 0 for DCache not auto preload before manual preload.
  *
  * @return None
  */
void Cache_End_L2_Cache_Preload(uint32_t autoload);

/**
  * @brief Config autoload parameters of L1 Core0 ICache.
  *        Please do not call this function in your SDK application.
  *
  * @param struct autoload_config * config : autoload parameters.
  *
  * @return ESP_ROM_ERR_INVALID_ARG : invalid param, 0 : success
  */
void Cache_Config_L1_CORE0_ICache_Autoload(const struct l1_icache_autoload_config *config);

/**
  * @brief Enable auto preload for L1 Core0 ICache.
  *        Please do not call this function in your SDK application.
  *
  * @param None
  *
  * @return None
  */
void Cache_Enable_L1_CORE0_ICache_Autoload(void);

/**
  * @brief Disable auto preload for L1 Core0 ICache.
  *        Please do not call this function in your SDK application.
  *
  * @param None
  *
  * @return None
  */
void Cache_Disable_L1_CORE0_ICache_Autoload(void);

/**
  * @brief Config autoload parameters of L1 Core1 ICache.
  *        Please do not call this function in your SDK application.
  *
  * @param struct autoload_config * config : autoload parameters.
  *
  * @return ESP_ROM_ERR_INVALID_ARG : invalid param, 0 : success
  */
void Cache_Config_L1_CORE1_ICache_Autoload(const struct l1_icache_autoload_config *config);

/**
  * @brief Enable auto preload for L1 Core1 ICache.
  *        Please do not call this function in your SDK application.
  *
  * @param None
  *
  * @return None
  */
void Cache_Enable_L1_CORE1_ICache_Autoload(void);

/**
  * @brief Disable auto preload for L1 Core1 ICache.
  *        Please do not call this function in your SDK application.
  *
  * @param None
  *
  * @return None
  */
void Cache_Disable_L1_CORE1_ICache_Autoload(void);

/**
  * @brief Config autoload parameters of L1 DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param struct autoload_config * config : autoload parameters.
  *
  * @return ESP_ROM_ERR_INVALID_ARG : invalid param, 0 : success
  */
void Cache_Config_L1_DCache_Autoload(const struct l1_dcache_l2_autoload_config *config);

/**
  * @brief Enable auto preload for L1 DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param None
  *
  * @return None
  */
void Cache_Enable_L1_DCache_Autoload(void);

/**
  * @brief Disable auto preload for L1 DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param None
  *
  * @return None
  */
void Cache_Disable_L1_DCache_Autoload(void);

/**
  * @brief Config autoload parameters of L2 Cache.
  *        Please do not call this function in your SDK application.
  *
  * @param struct autoload_config * config : autoload parameters.
  *
  * @return ESP_ROM_ERR_INVALID_ARG : invalid param, 0 : success
  */
void Cache_Config_L2_Cache_Autoload(const struct l1_dcache_l2_autoload_config *config);

/**
  * @brief Enable auto preload for L2 Cache.
  *        Please do not call this function in your SDK application.
  *
  * @param None
  *
  * @return None
  */
void Cache_Enable_L2_Cache_Autoload(void);

/**
  * @brief Disable auto preload for L2 Cache.
  *        Please do not call this function in your SDK application.
  *
  * @param None
  *
  * @return None
  */
void Cache_Disable_L2_Cache_Autoload(void);

/**
  * @brief Config a group of prelock parameters of L1 Core0 ICache.
  *        Please do not call this function in your SDK application.
  *
  * @param struct lock_config * config : a group of lock parameters.
  *
  * @return None
  */
void Cache_Enable_L1_CORE0_ICache_PreLock(const struct lock_config *config);

/**
  * @brief Disable a group of prelock parameters for L1 Core0 ICache.
  *        However, the locked data will not be released.
  *        Please do not call this function in your SDK application.
  *
  * @param uint16_t group : 0 for group0, 1 for group1.
  *
  * @return None
  */
void Cache_Disable_L1_CORE0_ICache_PreLock(uint16_t group);

/**
  * @brief Config a group of prelock parameters of L1 Core1 ICache.
  *        Please do not call this function in your SDK application.
  *
  * @param struct lock_config * config : a group of lock parameters.
  *
  * @return None
  */
void Cache_Enable_L1_CORE1_ICache_PreLock(const struct lock_config *config);

/**
  * @brief Disable a group of prelock parameters for L1 Core1 ICache.
  *        However, the locked data will not be released.
  *        Please do not call this function in your SDK application.
  *
  * @param uint16_t group : 0 for group0, 1 for group1.
  *
  * @return None
  */
void Cache_Disable_L1_CORE1_ICache_PreLock(uint16_t group);

/**
  * @brief Config a group of prelock parameters of L1 DCache.
  *        Please do not call this function in your SDK application.
  *
  * @param struct lock_config * config : a group of lock parameters.
  *
  * @return None
  */
void Cache_Enable_L1_DCache_PreLock(const struct lock_config *config);

/**
  * @brief Disable a group of prelock parameters for L1 DCache.
  *        However, the locked data will not be released.
  *        Please do not call this function in your SDK application.
  *
  * @param uint16_t group : 0 for group0, 1 for group1.
  *
  * @return None
  */
void Cache_Disable_L1_DCache_PreLock(uint16_t group);

/**
  * @brief Config a group of prelock parameters of L2 Cache.
  *        Please do not call this function in your SDK application.
  *
  * @param struct lock_config * config : a group of lock parameters.
  *
  * @return None
  */
void Cache_Enable_L2_Cache_PreLock(const struct lock_config *config);

/**
  * @brief Disable a group of prelock parameters for L2 Cache.
  *        However, the locked data will not be released.
  *        Please do not call this function in your SDK application.
  *
  * @param uint16_t group : 0 for group0, 1 for group1.
  *
  * @return None
  */
void Cache_Disable_L2_Cache_PreLock(uint16_t group);

/**
  * @brief Lock the cache items in tag memory for cache.
  *        Please do not call this function in your SDK application.
  *
  * @param uint32_t map  : operation which cache
  *
  * @param uint32_t addr : start address of lock region.
  *
  * @param uint32_t size : size of lock region.
  *
  * @return 0 for success
  *         1 for invalid argument
  */
int Cache_Lock_Addr(uint32_t map, uint32_t addr, uint32_t size);

/**
  * @brief Unlock the cache items in tag memory for cache.
  *        Please do not call this function in your SDK application.
  *
  * @param uint32_t map  : operation which cache
  *
  * @param uint32_t addr : start address of unlock region.
  *
  * @param uint32_t size : size of unlock region.
  *
  * @return 0 for success
  *         1 for invalid argument
  */
int Cache_Unlock_Addr(uint32_t map, uint32_t addr, uint32_t size);

/**
  * @brief Disable L1 Core0 ICache access for the cpu.
  *        This operation will make all ICache tag memory invalid, CPU can't access ICache, ICache will keep idle.
  *        Please do not call this function in your SDK application.
  *
  * @return uint32_t : auto preload enabled before
  */
uint32_t Cache_Disable_L1_CORE0_ICache(void);

/**
  * @brief Enable L1 Core0 ICache access for the cpu.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t autoload : ICache will preload then.
  *
  * @return None
  */
void Cache_Enable_L1_CORE0_ICache(uint32_t autoload);

/**
  * @brief Suspend L1 Core0 ICache access for the cpu.
  *        This operation will make all ICache tag memory invalid, CPU can't access ICache, ICache will keep idle.
  *        Please do not call this function in your SDK application.
  *
  * @return uint32_t : auto preload enabled before
  */
uint32_t Cache_Suspend_L1_CORE0_ICache(void);

/**
  * @brief Enable L1 Core0 ICache access for the cpu.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t autoload : ICache will preload then.
  *
  * @return None
  */
void Cache_Resume_L1_CORE0_ICache(uint32_t autoload);

/**
  * @brief Disable L1 Core1 ICache access for the cpu.
  *        This operation will make all ICache tag memory invalid, CPU can't access ICache, ICache will keep idle.
  *        Please do not call this function in your SDK application.
  *
  * @return uint32_t : auto preload enabled before
  */
uint32_t Cache_Disable_L1_CORE1_ICache(void);

/**
  * @brief Enable L1 Core1 ICache access for the cpu.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t autoload : ICache will preload then.
  *
  * @return None
  */
void Cache_Enable_L1_CORE1_ICache(uint32_t autoload);

/**
  * @brief Suspend L1 Core1 ICache access for the cpu.
  *        This operation will make all ICache tag memory invalid, CPU can't access ICache, ICache will keep idle.
  *        Please do not call this function in your SDK application.
  *
  * @return uint32_t : auto preload enabled before
  */
uint32_t Cache_Suspend_L1_CORE1_ICache(void);

/**
  * @brief Enable L1 Core1 ICache access for the cpu.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t autoload : ICache will preload then.
  *
  * @return None
  */
void Cache_Resume_L1_CORE1_ICache(uint32_t autoload);

/**
  * @brief Disable L1 DCache access for the cpu.
  *        This operation will make all ICache tag memory invalid, CPU can't access ICache, ICache will keep idle.
  *        Please do not call this function in your SDK application.
  *
  * @return uint32_t : auto preload enabled before
  */
uint32_t Cache_Disable_L1_DCache(void);

/**
  * @brief Enable L1 DCache access for the cpu.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t autoload : ICache will preload then.
  *
  * @return None
  */
void Cache_Enable_L1_DCache(uint32_t autoload);

/**
  * @brief Suspend L1 DCache access for the cpu.
  *        This operation will make all ICache tag memory invalid, CPU can't access ICache, ICache will keep idle.
  *        Please do not call this function in your SDK application.
  *
  * @return uint32_t : auto preload enabled before
  */
uint32_t Cache_Suspend_L1_DCache(void);

/**
  * @brief Enable L1 DCache access for the cpu.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t autoload : ICache will preload then.
  *
  * @return None
  */
void Cache_Resume_L1_DCache(uint32_t autoload);

/**
  * @brief Disable L2 Cache access for the cpu.
  *        This operation will make all ICache tag memory invalid, CPU can't access ICache, ICache will keep idle.
  *        Please do not call this function in your SDK application.
  *
  * @return uint32_t : auto preload enabled before
  */
uint32_t Cache_Disable_L2_Cache(void);

/**
  * @brief Enable L2 Cache access for the cpu.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t autoload : ICache will preload then.
  *
  * @return None
  */
void Cache_Enable_L2_Cache(uint32_t autoload);

/**
  * @brief Suspend L2 Cache access for the cpu.
  *        This operation will make all ICache tag memory invalid, CPU can't access ICache, ICache will keep idle.
  *        Please do not call this function in your SDK application.
  *
  * @return uint32_t : auto preload enabled before
  */
uint32_t Cache_Suspend_L2_Cache(void);

/**
  * @brief Enable L2 Cache access for the cpu.
  *        Please do not call this function in your SDK application.
  *
  * @param  uint32_t autoload : ICache will preload then.
  *
  * @return None
  */
void Cache_Resume_L2_Cache(uint32_t autoload);

/**
  * @brief Get L1 ICache cache line size
  *
  * @param  None
  *
  * @return uint32_t: 16, 32, 64 Byte
  */
uint32_t Cache_Get_L1_ICache_Line_Size(void);

/**
  * @brief Get L1 DCache cache line size
  *
  * @param  None
  *
  * @return uint32_t: 16, 32, 64 Byte
  */
uint32_t Cache_Get_L1_DCache_Line_Size(void);

/**
  * @brief Get L2 cache line size
  *
  * @param  None
  *
  * @return uint32_t: 16, 32, 64 Byte
  */
uint32_t Cache_Get_L2_Cache_Line_Size(void);

/**
  * @brief Get DCache cache line size
  *
  * @param  None
  *
  * @return uint32_t: 16, 32, 64 Byte
  */
uint32_t Cache_Get_DCache_Line_Size(void);

/**
  * @brief Freeze L1 core0 icache
  *
  * @param cache_freeze_mode_t : mode
  *
  * @return None
  */
void Cache_Freeze_L1_ICache0_Enable(cache_freeze_mode_t mode);

/**
  * @brief Disable L1 core0 icache freeze
  *
  * @param None
  *
  * @return None
  */
void Cache_Freeze_L1_ICache0_Disable(void);

/**
  * @brief Freeze L1 core1 icache
  *
  * @param cache_freeze_mode_t : mode
  *
  * @return None
  */
void Cache_Freeze_L1_ICache1_Enable(cache_freeze_mode_t mode);

/**
  * @brief Disable L1 core1 icache freeze
  *
  * @param None
  *
  * @return None
  */
void Cache_Freeze_L1_ICache1_Disable(void);

/**
  * @brief Freeze L1 dcache
  *
  * @param cache_freeze_mode_t : mode
  *
  * @return None
  */
void Cache_Freeze_L1_DCache_Enable(cache_freeze_mode_t mode);

/**
  * @brief Disable L1 dcache freeze
  *
  * @param None
  *
  * @return None
  */
void Cache_Freeze_L1_DCache_Disable(void);

/**
  * @brief Freeze L2 cache
  *
  * @param cache_freeze_mode_t : mode
  *
  * @return None
  */
void Cache_Freeze_L2_Cache_Enable(cache_freeze_mode_t mode);

/**
  * @brief Disable L2 cache freeze
  *
  * @param None
  *
  * @return None
  */
void Cache_Freeze_L2_Cache_Disable(void);

/**
  * @brief Travel tag memory to run a call back function using 1st tag api.
  *        ICache and DCache are suspend when doing this.
  *        The callback will get the parameter tag_group_info, which will include a group of tag memory addresses and cache memory addresses.
  *        Please do not call this function in your SDK application.
  *
  * @param  struct cache_mode * mode : the cache to check and the cache mode.
  *
  * @param  uint32_t filter_addr : only the cache lines which may include the filter_address will be returned to the call back function.
  *                                0 for do not filter, all cache lines will be returned.
  *
  * @param  void (* process)(struct tag_group_info *, int res[]) : call back function, which may be called many times, a group(the addresses in the group are in the same position in the cache ways) a time.
  *
  * @return None
  */
void Cache_Travel_Tag_Memory(struct cache_mode *mode, uint32_t filter_addr, void (*process)(struct tag_group_info *, int res[]), int res[]);

/**
  * @brief Travel tag memory to run a call back function using 2nd tag api.
  *        ICache and DCache are suspend when doing this.
  *        The callback will get the parameter tag_group_info, which will include a group of tag memory addresses and cache memory addresses.
  *        Please do not call this function in your SDK application.
  *
  * @param  struct cache_mode * mode : the cache to check and the cache mode.
  *
  * @param  uint32_t filter_addr : only the cache lines which may include the filter_address will be returned to the call back function.
  *                                0 for do not filter, all cache lines will be returned.
  *
  * @param  void (* process)(struct tag_group_info *, int res[]) : call back function, which may be called many times, a group(the addresses in the group are in the same position in the cache ways) a time.
  *
  * @return None
  */
void Cache_Travel_Tag_Memory2(struct cache_mode *mode, uint32_t filter_addr, void (*process)(struct tag_group_info *, int res[]), int res[]);

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

/* MACRO for rom boot */
#define ROM_boot_Cache_Suspend()        Cache_Suspend_L2_Cache()
#define ROM_boot_Cache_Invalidate_All() Cache_Invalidate_All(CACHE_MAP_L2_CACHE)
#define ROM_boot_Cache_Resume(preload)  Cache_Resume_L2_Cache(preload)

#define Cache_Dbus_MMU_Set(ext_mem, vaddr, paddr, page_size, page, fix) \
    Cache_MSPI_MMU_Set_Secure(ext_mem, vaddr, paddr, page_size, page, fix)
#define Cache_Ibus_MMU_Set(ext_mem, vaddr, paddr, page_size, page, fix) \
    Cache_MSPI_MMU_Set_Secure(ext_mem, vaddr, paddr, page_size, page, fix)
#define Cache_MMU_Init() Cache_MSPI_MMU_Init()

/* MACRO for spi flash mmap */
#define SPI_FLASH_MMAP_INVALIDATE_CACHE_ADDR(addr, size) Cache_Invalidate_Addr(CACHE_MAP_L1_DCACHE | CACHE_MAP_L2_CACHE, addr, size)

/**
  * @brief Used by SPI flash mmap
  *
  */
int flash2spiram_instruction_offset(void);
int flash2spiram_rodata_offset(void);
uint32_t flash_instr_rodata_start_page(uint32_t bus);
uint32_t flash_instr_rodata_end_page(uint32_t bus);

#ifdef __cplusplus
}
#endif

#endif /* _ROM_CACHE_H_ */
