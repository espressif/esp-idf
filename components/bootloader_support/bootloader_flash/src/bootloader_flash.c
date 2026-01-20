/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>

#include <bootloader_flash_priv.h>
#include <esp_log.h>
#include <esp_flash_encrypt.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "hal/efuse_ll.h"
#include "hal/efuse_hal.h"

#if !NON_OS_BUILD
#include "spi_flash_mmap.h"
#endif
#include "hal/spi_flash_ll.h"
#include "rom/spi_flash.h"
#include "esp_private/cache_utils.h"
#if !CONFIG_IDF_TARGET_ESP32
#include "hal/spimem_flash_ll.h"
#endif

// This dependency will be removed in the future.  IDF-5025
#include "esp_flash.h"

#include "esp_rom_spiflash.h"

#ifdef CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH
#define ENCRYPTION_IS_VIRTUAL (!efuse_hal_flash_encryption_enabled())
#else
#define ENCRYPTION_IS_VIRTUAL 0
#endif

#define BYTESHIFT(VAR, IDX)    (((VAR) >> ((IDX) * 8)) & 0xFF)
#define ISSI_ID                0x9D
#define MXIC_ID                0xC2
#define GD_Q_ID_HIGH           0xC8
#define GD_Q_ID_MID            0x40
#define GD_Q_ID_LOW            0x16

#define ESP_BOOTLOADER_SPIFLASH_BP_MASK_ISSI    (BIT7 | BIT5 | BIT4 | BIT3 | BIT2)
#define ESP_BOOTLOADER_SPIFLASH_QE_GD_SR2        BIT1   // QE position when you write 8 bits(for SR2) at one time.
#define ESP_BOOTLOADER_SPIFLASH_QE_SR1_2BYTE     BIT9   // QE position when you write 16 bits at one time.

#if !NON_OS_BUILD
/* Normal app version maps to spi_flash_mmap.h operations...
 */
ESP_LOG_ATTR_TAG(TAG, "bootloader_mmap");

static spi_flash_mmap_handle_t map;

uint32_t bootloader_mmap_get_free_pages(void)
{
    return spi_flash_mmap_get_free_pages(SPI_FLASH_MMAP_DATA);
}

const void *bootloader_mmap(uint32_t src_addr, uint32_t size)
{
    if (map) {
        ESP_EARLY_LOGE(TAG, "tried to bootloader_mmap twice");
        return NULL; /* existing mapping in use... */
    }
    const void *result = NULL;
    uint32_t src_page = src_addr & ~(SPI_FLASH_MMU_PAGE_SIZE - 1);
    size += (src_addr - src_page);
    esp_err_t err = spi_flash_mmap(src_page, size, SPI_FLASH_MMAP_DATA, &result, &map);
    if (err != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "spi_flash_mmap failed: 0x%x", err);
        return NULL;
    }
    return (void *)((intptr_t)result + (src_addr - src_page));
}

void bootloader_munmap(const void *mapping)
{
    if (mapping && map) {
        spi_flash_munmap(map);
    }
    map = 0;
}

esp_err_t bootloader_flash_read(size_t src, void *dest, size_t size, bool allow_decrypt)
{
    if (allow_decrypt && esp_flash_encryption_enabled()) {
        return esp_flash_read_encrypted(NULL, src, dest, size);
    } else {
        return esp_flash_read(NULL, dest, src, size);
    }
}

esp_err_t bootloader_flash_write(size_t dest_addr, void *src, size_t size, bool write_encrypted)
{
    if (write_encrypted && !ENCRYPTION_IS_VIRTUAL) {
        return esp_flash_write_encrypted(NULL, dest_addr, src, size);
    } else {
        return esp_flash_write(NULL, src, dest_addr, size);
    }
}

esp_err_t bootloader_flash_erase_sector(size_t sector)
{
    // Will de-dependency IDF-5025
    return esp_flash_erase_region(NULL, sector * SPI_FLASH_SEC_SIZE, SPI_FLASH_SEC_SIZE);
}

esp_err_t bootloader_flash_erase_range(uint32_t start_addr, uint32_t size)
{
    // Will de-dependency IDF-5025
    return esp_flash_erase_region(NULL, start_addr, size);
}

#else // NON_OS_BUILD
/* Bootloader version, uses ROM functions only */
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/cache.h"
#endif
#include "esp_rom_spiflash.h"
#include "esp_rom_sys.h"
#include "hal/mmu_hal.h"
#include "hal/mmu_ll.h"
#include "hal/cache_hal.h"
#include "hal/cache_ll.h"

#if CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/opi_flash.h"
#elif CONFIG_IDF_TARGET_ESP32P4
#include "esp32p4/rom/opi_flash.h"
#elif CONFIG_IDF_TARGET_ESP32C5
#include "esp32c5/rom/opi_flash.h"
#endif
#include "spi_flash/spi_flash_defs.h"

#if ESP_TEE_BUILD
#include "esp_fault.h"
#include "esp_flash_partitions.h"
#include "rom/spi_flash.h"

extern bool esp_tee_flash_check_prange_in_active_tee_part(const size_t paddr, const size_t len);
#endif

ESP_LOG_ATTR_TAG(TAG, "bootloader_flash");

/*
 * NOTE: Memory mapping strategy
 *
 * Bootloader:
 * - Uses the first N-1 MMU entries for general memory mapping.
 * - Reserves the Nth (last) MMU entry for flash read through the cache
 *   (auto-decryption).
 * - This strategy is viable because the bootloader runs exclusively
 *   on the device from the internal SRAM.
 *
 * ESP-TEE (Trusted Execution Environment)
 * - Cannot adopt the strategy used by the bootloader as the TEE app operates
 *   in parallel to the REE.
 * - The few initial MMU entries have already been taken by the TEE and REE
 *   application flash IDROM segments.
 * - The REE could have also mapped some custom flash partitions it requires.
 * - Therefore, the TEE uses MMU entries from the end of the range, with the number
 *   of entries corresponding to the size of its IDROM segment sizes.
 * - The final MMU entry in this range is reserved for flash reads through the
 *   cache (auto-decryption).
 * - The pages used by TEE are protected by PMP (Physical Memory Protection).
 *   While REE attempts to mmap this protected area would trigger a load access
 *   fault, this is unlikely since the MMU can address up to 16MB at once.
 */

#if CONFIG_IDF_TARGET_ESP32
/* Use first 50 blocks in MMU for bootloader_mmap,
   50th block for bootloader_flash_read
*/
#define MMU_BLOCK0_VADDR  SOC_DROM_LOW
#define MMU_TOTAL_SIZE     (0x320000)
#define MMU_BLOCK50_VADDR (MMU_BLOCK0_VADDR + MMU_TOTAL_SIZE)
#define FLASH_READ_VADDR  MMU_BLOCK50_VADDR

#else // !CONFIG_IDF_TARGET_ESP32

/* Use first 63 blocks in MMU for bootloader_mmap,
   63th block for bootloader_flash_read
*/
#define MMU_BLOCK0_VADDR  SOC_DROM_LOW
#if CONFIG_IDF_TARGET_ESP32S2
/**
 * On ESP32S2 we use `(SOC_DRAM0_CACHE_ADDRESS_HIGH - SOC_DRAM0_CACHE_ADDRESS_LOW)`.
 * As this code is in bootloader, we keep this on ESP32S2
 */
#define MMU_TOTAL_SIZE     (SOC_DRAM0_CACHE_ADDRESS_HIGH - SOC_DRAM0_CACHE_ADDRESS_LOW) // This mmu size means that the mmu size to be mapped
#else
#define MMU_TOTAL_SIZE     (SOC_DRAM_FLASH_ADDRESS_HIGH - SOC_DRAM_FLASH_ADDRESS_LOW) // This mmu size means that the mmu size to be mapped
#endif
#define MMU_END_VADDR      (MMU_BLOCK0_VADDR + MMU_TOTAL_SIZE)
#define MMU_BLOCKL_VADDR   (MMU_END_VADDR - 1 * CONFIG_MMU_PAGE_SIZE)
#define FLASH_READ_VADDR   MMU_BLOCKL_VADDR
#endif

#if !ESP_TEE_BUILD
#define MMAP_MMU_SIZE     (MMU_TOTAL_SIZE)
// Represents the MMU pages available for mmapping by the bootloader
#define MMU_FREE_PAGES    (MMAP_MMU_SIZE / CONFIG_MMU_PAGE_SIZE)
#define FLASH_MMAP_VADDR  (MMU_BLOCK0_VADDR)
#else /* ESP_TEE_BUILD */
#define MMAP_MMU_SIZE     (CONFIG_SECURE_TEE_IROM_SIZE + CONFIG_SECURE_TEE_DROM_SIZE)
// Represents the MMU pages available for mmapping by the TEE
#define MMU_FREE_PAGES    (MMAP_MMU_SIZE / CONFIG_MMU_PAGE_SIZE)
#define FLASH_MMAP_VADDR  (MMU_END_VADDR - (MMU_FREE_PAGES + 1) * CONFIG_MMU_PAGE_SIZE)
#endif /* !ESP_TEE_BUILD */

static bool mapped;

// Required for bootloader_flash_munmap() for ESP-TEE
static uint32_t current_mapped_size;

// Current bootloader mapping (ab)used for bootloader_read()
static uint32_t current_read_mapping = UINT32_MAX;

#if ESP_TEE_BUILD
/* [ESP-TEE] Workarounds for the ROM SPI flash APIs */
/*
 * TODO: The esp_rom_spiflash_read API requires two workarounds on ESP32-C6 ECO0 -
 *
 * 1. [IDF-7199] Call esp_rom_spiflash_write API once before reading.
 *    Without this, reads return corrupted data.
 *
 * 2. Configure ROM flash parameters before each read using the function below.
 *    Without this, the first byte read is corrupted.
 *
 * NOTE: These workarounds are not needed for ESP32-C6 ECO1 and later versions.
 */
static void rom_read_api_workaround(void)
{
#if CONFIG_ESP32C6_REV_MIN_0
    extern void spi_common_set_dummy_output(esp_rom_spiflash_read_mode_t mode);
    extern void spi_dummy_len_fix(uint8_t spi, uint8_t freqdiv);

    static bool is_first_call = true;
    if (is_first_call) {
        uint32_t dummy_val = UINT32_MAX;
        uint32_t dest_addr = ESP_PARTITION_TABLE_OFFSET + ESP_PARTITION_TABLE_MAX_LEN;
        esp_rom_spiflash_write(dest_addr, &dummy_val, sizeof(dummy_val));
        is_first_call = false;
    }

    uint32_t freqdiv = 0;

#if CONFIG_ESPTOOLPY_FLASHFREQ_80M
    freqdiv = 1;
#elif CONFIG_ESPTOOLPY_FLASHFREQ_40M
    freqdiv = 2;
#elif CONFIG_ESPTOOLPY_FLASHFREQ_20M
    freqdiv = 4;
#endif

    esp_rom_spiflash_read_mode_t read_mode;
#if CONFIG_ESPTOOLPY_FLASHMODE_QIO
    read_mode = ESP_ROM_SPIFLASH_QIO_MODE;
#elif CONFIG_ESPTOOLPY_FLASHMODE_QOUT
    read_mode = ESP_ROM_SPIFLASH_QOUT_MODE;
#elif CONFIG_ESPTOOLPY_FLASHMODE_DIO
    read_mode = ESP_ROM_SPIFLASH_DIO_MODE;
#elif CONFIG_ESPTOOLPY_FLASHMODE_DOUT
    read_mode = ESP_ROM_SPIFLASH_DOUT_MODE;
#endif

    esp_rom_spiflash_config_clk(freqdiv, 1);
    spi_dummy_len_fix(1, freqdiv);
    esp_rom_spiflash_config_readmode(read_mode);
    spi_common_set_dummy_output(read_mode);
#endif
}

/*
 * TODO: [IDF-13582]
 *
 * When `esp_flash_read()` is invoked from REE, it enables SPI1 WB (write-back) mode
 * via `spi_flash_ll_wb_mode_enable()`. The ROM flash APIs used by TEE do not support
 * WB mode, causing failures when TEE later accesses flash.
 *
 * Workaround applied in TEE flash layer:
 * 1. Save the current WB mode state.
 * 2. Temporarily disable WB mode before calling ROM flash APIs.
 * 3. Restore WB mode state after the ROM API call completes.
 *
 * NOTE: This workaround will become removed once IDF-13582 is implemented.
 */
static inline bool spi1_wb_mode_save_and_disable(void)
{
#if SOC_SPI_MEM_SUPPORT_WB_MODE_INDEPENDENT_CONTROL
    if (REG_GET_BIT(SPI_MEM_RD_STATUS_REG(1), SPI_MEM_WB_MODE_EN)) {
        REG_CLR_BIT(SPI_MEM_RD_STATUS_REG(1), SPI_MEM_WB_MODE_EN);
        return true;
    }
#endif
    return false;
}

static inline void spi1_wb_mode_restore(bool saved_state)
{
#if SOC_SPI_MEM_SUPPORT_WB_MODE_INDEPENDENT_CONTROL
    if (saved_state) {
        REG_SET_BIT(SPI_MEM_RD_STATUS_REG(1), SPI_MEM_WB_MODE_EN);
    }
#else
    (void)saved_state;
#endif
}
#endif

uint32_t bootloader_mmap_get_free_pages(void)
{
    /**
     * Allow mapping up to 50 of the 51 available MMU blocks (last one used for reads)
     * Since, bootloader_mmap function below assumes it to be 0x320000 (50 pages), we can safely do this.
     */
    return MMU_FREE_PAGES;
}

const void *bootloader_mmap(uint32_t src_paddr, uint32_t size)
{
    if (mapped) {
        ESP_EARLY_LOGE(TAG, "tried to bootloader_mmap twice");
        return NULL; /* can't map twice */
    }
    if (size > MMAP_MMU_SIZE) {
        ESP_EARLY_LOGE(TAG, "bootloader_mmap excess size %" PRIx32, size);
        return NULL;
    }

    uint32_t src_paddr_aligned = src_paddr & MMU_FLASH_MASK;
    //The addr is aligned, so we add the mask off length to the size, to make sure the corresponding buses are enabled.
    uint32_t size_after_paddr_aligned = (src_paddr - src_paddr_aligned) + size;

    uint32_t actual_mapped_len = 0;
    /**
     * @note 1
     * Will add here a check to make sure the vaddr is on read-only and executable buses, since we use others for psram
     * Now simply check if it's valid vaddr, didn't check if it's readable, writable or executable.
     * TODO: IDF-4710
     */
    if (mmu_ll_check_valid_ext_vaddr_region(0, FLASH_MMAP_VADDR, size_after_paddr_aligned, MMU_VADDR_DATA | MMU_VADDR_INSTRUCTION) == 0) {
        ESP_EARLY_LOGE(TAG, "vaddr not valid");
        return NULL;
    }

    //-------------stop cache to do the MMU mapping--------------
#if CONFIG_IDF_TARGET_ESP32
    Cache_Read_Disable(0);
    Cache_Flush(0);
#else
    /* NOTE: [ESP-TEE] Cache suspension vs disabling
     *
     * For ESP-TEE , we use suspend the cache instead of disabling it to avoid flushing the entire cache.
     * This prevents performance hits when returning to the REE app due to cache misses.
     * This is not applicable to the bootloader as it runs exclusively on the device from the internal SRAM.
     */
#if !ESP_TEE_BUILD
    cache_hal_disable(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
#else
    cache_hal_suspend(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
#endif
#endif

    //---------------Do mapping------------------------
    ESP_EARLY_LOGD(TAG, "rodata starts from paddr=0x%08" PRIx32 ", size=0x%" PRIx32 ", will be mapped to vaddr=0x%08" PRIx32, src_paddr, size, (uint32_t)FLASH_MMAP_VADDR);
#if CONFIG_IDF_TARGET_ESP32
    uint32_t count = GET_REQUIRED_MMU_PAGES(size, src_paddr);
    int e = cache_flash_mmu_set(0, 0, FLASH_MMAP_VADDR, src_paddr_aligned, 64, count);
    ESP_EARLY_LOGV(TAG, "after mapping, starting from paddr=0x%08" PRIx32 " and vaddr=0x%08" PRIx32 ", 0x%" PRIx32 " bytes are mapped", src_paddr_aligned, (uint32_t)FLASH_MMAP_VADDR, count * SPI_FLASH_MMU_PAGE_SIZE);
    if (e != 0) {
        ESP_EARLY_LOGE(TAG, "cache_flash_mmu_set failed: %d", e);
        Cache_Read_Enable(0);
        return NULL;
    }
#else
    /**
     * This hal won't return error, it assumes the inputs are valid. The related check should be done in `bootloader_mmap()`.
     * See above comments (note 1) about IDF-4710
     */
    mmu_hal_map_region(0, MMU_TARGET_FLASH0, FLASH_MMAP_VADDR, src_paddr_aligned, size_after_paddr_aligned, &actual_mapped_len);
    ESP_EARLY_LOGV(TAG, "after mapping, starting from paddr=0x%08" PRIx32 " and vaddr=0x%08" PRIx32 ", 0x%" PRIx32 " bytes are mapped", src_paddr_aligned, (uint32_t)FLASH_MMAP_VADDR, actual_mapped_len);
#endif

    /**
     * If after mapping, your code stucks, it possibly means that some of the buses are not enabled, check `cache_ll_l1_enable_bus()`
     * For now, keep this unchanged.
     */

    //-------------enable cache--------------
#if CONFIG_IDF_TARGET_ESP32
    Cache_Read_Enable(0);
#else
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
    cache_ll_invalidate_addr(CACHE_LL_LEVEL_ALL, CACHE_TYPE_ALL, CACHE_LL_ID_ALL, FLASH_MMAP_VADDR, actual_mapped_len);
#endif
#if !ESP_TEE_BUILD
    cache_hal_enable(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
#else
    cache_hal_resume(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
#endif
#endif

    mapped = true;
    current_mapped_size = actual_mapped_len;

    return (void *)(FLASH_MMAP_VADDR + (src_paddr - src_paddr_aligned));
}

void bootloader_munmap(const void *mapping)
{
    if (mapped)  {
#if CONFIG_IDF_TARGET_ESP32
        /* Full MMU reset */
        Cache_Read_Disable(0);
        Cache_Flush(0);
        mmu_init(0);
#else
#if !ESP_TEE_BUILD
        cache_hal_disable(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
        mmu_hal_unmap_all();
#else
        cache_hal_suspend(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
        mmu_hal_unmap_region(0, FLASH_MMAP_VADDR, current_mapped_size);
        cache_hal_invalidate_addr(FLASH_MMAP_VADDR, current_mapped_size);
        cache_hal_resume(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
#endif
#endif
        mapped = false;
        current_mapped_size = 0;
    }
}

static esp_err_t spi_to_esp_err(esp_rom_spiflash_result_t r)
{
    switch (r) {
    case ESP_ROM_SPIFLASH_RESULT_OK:
        return ESP_OK;
    case ESP_ROM_SPIFLASH_RESULT_ERR:
        return ESP_ERR_FLASH_OP_FAIL;
    case ESP_ROM_SPIFLASH_RESULT_TIMEOUT:
        return ESP_ERR_FLASH_OP_TIMEOUT;
    default:
        return ESP_FAIL;
    }
}

static esp_err_t bootloader_flash_read_no_decrypt(size_t src_addr, void *dest, size_t size)
{
#if CONFIG_IDF_TARGET_ESP32
    Cache_Read_Disable(0);
    Cache_Flush(0);
#else
#if !ESP_TEE_BUILD
    cache_hal_disable(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
#else
    rom_read_api_workaround();
    bool is_wb_saved = spi1_wb_mode_save_and_disable();
#endif
#endif

    esp_rom_spiflash_result_t r = esp_rom_spiflash_read(src_addr, dest, size);

#if CONFIG_IDF_TARGET_ESP32
    Cache_Read_Enable(0);
#else
#if !ESP_TEE_BUILD
    cache_hal_enable(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
#else
    spi1_wb_mode_restore(is_wb_saved);
#endif
#endif

    return spi_to_esp_err(r);
}

static esp_err_t bootloader_flash_read_allow_decrypt(size_t src_addr, void *dest, size_t size)
{
    uint32_t *dest_words = (uint32_t *)dest;

    for (size_t word = 0; word < size / 4; word++) {
        uint32_t word_src = src_addr + word * 4;  /* Read this offset from flash */
        uint32_t map_at = word_src & MMU_FLASH_MASK; /* Map this 64KB block from flash */
        uint32_t *map_ptr;

        /* Move the 64KB mmu mapping window to fit map_at */
        if (map_at != current_read_mapping) {

            //----------Stop cache for mapping----------------
#if CONFIG_IDF_TARGET_ESP32
            Cache_Read_Disable(0);
            Cache_Flush(0);
#else
#if !ESP_TEE_BUILD
            cache_hal_disable(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
#else
            cache_hal_suspend(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
            //---------------Invalidating entries at to-be-mapped v_addr------------------------
            cache_hal_invalidate_addr(FLASH_READ_VADDR, SPI_FLASH_MMU_PAGE_SIZE);
#endif
#endif

            //---------------Do mapping------------------------
            ESP_EARLY_LOGD(TAG, "mmu set block paddr=0x%08" PRIx32 " (was 0x%08" PRIx32 ")", map_at, current_read_mapping);
#if CONFIG_IDF_TARGET_ESP32
            //Should never fail if we only map a SPI_FLASH_MMU_PAGE_SIZE to the vaddr starting from FLASH_READ_VADDR
            // Return value unused if asserts are disabled
            int e __attribute__((unused)) = cache_flash_mmu_set(0, 0, FLASH_READ_VADDR, map_at, 64, 1);
            assert(e == 0);
#else
            uint32_t actual_mapped_len = 0;
            mmu_hal_map_region(0, MMU_TARGET_FLASH0, FLASH_READ_VADDR, map_at, SPI_FLASH_MMU_PAGE_SIZE - 1, &actual_mapped_len);
#endif
            current_read_mapping = map_at;

            //-------------enable cache-------------------------
#if CONFIG_IDF_TARGET_ESP32
            Cache_Read_Enable(0);
#else
#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE
            cache_ll_invalidate_addr(CACHE_LL_LEVEL_ALL, CACHE_TYPE_ALL, CACHE_LL_ID_ALL, FLASH_READ_VADDR, actual_mapped_len);
#endif
#if !ESP_TEE_BUILD
            cache_hal_enable(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
#else
            cache_hal_resume(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
#endif
#endif
        }
        map_ptr = (uint32_t *)(FLASH_READ_VADDR + (word_src - map_at));
        dest_words[word] = *map_ptr;
        current_read_mapping = UINT32_MAX;
    }
    return ESP_OK;
}

esp_err_t bootloader_flash_read(size_t src_addr, void *dest, size_t size, bool allow_decrypt)
{
    if ((src_addr & 3) || (size & 3) || ((intptr_t)dest & 3)) {
        ESP_EARLY_LOGE(TAG, "bootloader_flash_read src_addr 0x%x, size 0x%x or dest 0x%x not 4-byte aligned", src_addr, size, (intptr_t)dest);
        return ESP_FAIL;
    }

    if (allow_decrypt) {
        return bootloader_flash_read_allow_decrypt(src_addr, dest, size);
    } else {
        return bootloader_flash_read_no_decrypt(src_addr, dest, size);
    }
}

esp_err_t bootloader_flash_write(size_t dest_addr, void *src, size_t size, bool write_encrypted)
{
    /* NOTE: [ESP-TEE] Flash operation address validation with anti-FI check
     *
     * Ensure that flash operations cannot be executed within forbidden memory ranges
     * by validating the address before proceeding.
     */
#if ESP_TEE_BUILD
    bool addr_chk = esp_tee_flash_check_prange_in_active_tee_part(dest_addr, size);
    if (addr_chk) {
        ESP_EARLY_LOGE(TAG, "bootloader_flash_write invalid dest_addr");
        return ESP_FAIL;
    }
    ESP_FAULT_ASSERT(!addr_chk);
#endif
    size_t alignment = write_encrypted ? 32 : 4;
    if ((dest_addr % alignment) != 0) {
        ESP_EARLY_LOGE(TAG, "bootloader_flash_write dest_addr 0x%x not %d-byte aligned", dest_addr, alignment);
        return ESP_FAIL;
    }
    if ((size % alignment) != 0) {
        ESP_EARLY_LOGE(TAG, "bootloader_flash_write size 0x%x not %d-byte aligned", size, alignment);
        return ESP_FAIL;
    }
    if (((intptr_t)src % 4) != 0) {
        ESP_EARLY_LOGE(TAG, "bootloader_flash_write src 0x%x not 4 byte aligned", (intptr_t)src);
        return ESP_FAIL;
    }

    esp_err_t err = bootloader_flash_unlock();
    if (err != ESP_OK) {
        return err;
    }

#if ESP_TEE_BUILD
    bool is_wb_saved = spi1_wb_mode_save_and_disable();
#endif

    esp_rom_spiflash_result_t rc = ESP_ROM_SPIFLASH_RESULT_OK;

    if (write_encrypted && !ENCRYPTION_IS_VIRTUAL) {
        rc = esp_rom_spiflash_write_encrypted(dest_addr, src, size);
    } else {
        rc = esp_rom_spiflash_write(dest_addr, src, size);
    }
    /* NOTE: [ESP-TEE] Cache flushing after flash writes/erases
     *
     * After writing or erasing the flash, we need to flush the cache at locations
     * corresponding to the destination write/erase address. This prevents stale data
     * from being read from already memory-mapped addresses that were modified.
     */
#if ESP_TEE_BUILD
    spi1_wb_mode_restore(is_wb_saved);
    spi_flash_check_and_flush_cache(dest_addr, size);
#endif

    return spi_to_esp_err(rc);
}

esp_err_t bootloader_flash_erase_sector(size_t sector)
{
    return spi_to_esp_err(esp_rom_spiflash_erase_sector(sector));
}

esp_err_t bootloader_flash_erase_range(uint32_t start_addr, uint32_t size)
{
#if ESP_TEE_BUILD
    bool addr_chk = esp_tee_flash_check_prange_in_active_tee_part(start_addr, size);
    if (addr_chk) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_FAULT_ASSERT(!addr_chk);
#endif
    if (start_addr % FLASH_SECTOR_SIZE != 0) {
        return ESP_ERR_INVALID_ARG;
    }
    if (size % FLASH_SECTOR_SIZE != 0) {
        return ESP_ERR_INVALID_SIZE;
    }
    size_t start = start_addr / FLASH_SECTOR_SIZE;
    size_t end = start + size / FLASH_SECTOR_SIZE;
    const size_t sectors_per_block = FLASH_BLOCK_SIZE / FLASH_SECTOR_SIZE;

    esp_rom_spiflash_result_t rc = ESP_ROM_SPIFLASH_RESULT_OK;
    for (size_t sector = start; sector != end && rc == ESP_ROM_SPIFLASH_RESULT_OK; ) {
        if (sector % sectors_per_block == 0 && end - sector >= sectors_per_block) {
            rc = esp_rom_spiflash_erase_block(sector / sectors_per_block);
            sector += sectors_per_block;
        } else {
            rc = esp_rom_spiflash_erase_sector(sector);
            ++sector;
        }
    }
#if ESP_TEE_BUILD
    spi_flash_check_and_flush_cache(start_addr, size);
#endif

    return spi_to_esp_err(rc);
}

#if CONFIG_BOOTLOADER_CACHE_32BIT_ADDR_QUAD_FLASH || CONFIG_BOOTLOADER_CACHE_32BIT_ADDR_OCTAL_FLASH
void bootloader_flash_32bits_address_map_enable(esp_rom_spiflash_read_mode_t flash_mode)
{
    esp_rom_opiflash_spi0rd_t cache_rd = {};
    switch (flash_mode) {
    case ESP_ROM_SPIFLASH_DOUT_MODE:
        cache_rd.addr_bit_len = 32;
        cache_rd.dummy_bit_len = SPI_FLASH_DOUT_DUMMY_BITLEN;
        cache_rd.cmd = CMD_FASTRD_DUAL_4B;
        cache_rd.cmd_bit_len = 8;
        break;
    case ESP_ROM_SPIFLASH_DIO_MODE:
        cache_rd.addr_bit_len = 32;
        cache_rd.dummy_bit_len = SPI_FLASH_DIO_DUMMY_BITLEN;
        cache_rd.cmd = CMD_FASTRD_DIO_4B;
        cache_rd.cmd_bit_len = 8;
        break;
    case ESP_ROM_SPIFLASH_QOUT_MODE:
        cache_rd.addr_bit_len = 32;
        cache_rd.dummy_bit_len = SPI_FLASH_QOUT_DUMMY_BITLEN;
        cache_rd.cmd = CMD_FASTRD_QUAD_4B;
        cache_rd.cmd_bit_len = 8;
        break;
    case ESP_ROM_SPIFLASH_QIO_MODE:
        cache_rd.addr_bit_len = 32;
        cache_rd.dummy_bit_len = SPI_FLASH_QIO_DUMMY_BITLEN;
        cache_rd.cmd = CMD_FASTRD_QIO_4B;
        cache_rd.cmd_bit_len = 8;
        break;
    case ESP_ROM_SPIFLASH_FASTRD_MODE:
        cache_rd.addr_bit_len = 32;
        cache_rd.dummy_bit_len = SPI_FLASH_FASTRD_DUMMY_BITLEN;
        cache_rd.cmd = CMD_FASTRD_4B;
        cache_rd.cmd_bit_len = 8;
        break;
     case ESP_ROM_SPIFLASH_SLOWRD_MODE:
        cache_rd.addr_bit_len = 32;
        cache_rd.dummy_bit_len = SPI_FLASH_SLOWRD_DUMMY_BITLEN;
        cache_rd.cmd = CMD_SLOWRD_4B;
        cache_rd.cmd_bit_len = 8;
        break;
    default:
        assert(false);
        break;
    }
    cache_hal_disable(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
    esp_rom_opiflash_cache_mode_config(flash_mode, &cache_rd);
    cache_hal_enable(CACHE_LL_LEVEL_EXT_MEM, CACHE_TYPE_ALL);
}
#endif

#endif // NON_OS_BUILD


FORCE_INLINE_ATTR bool is_issi_chip(const esp_rom_spiflash_chip_t* chip)
{
    return BYTESHIFT(chip->device_id, 2) == ISSI_ID;
}

// For GD25Q32, GD25Q64, GD25Q127C, GD25Q128, which use single command to read/write different SR.
FORCE_INLINE_ATTR bool is_gd_q_chip(const esp_rom_spiflash_chip_t* chip)
{
    return BYTESHIFT(chip->device_id, 2) == GD_Q_ID_HIGH && BYTESHIFT(chip->device_id, 1) == GD_Q_ID_MID && BYTESHIFT(chip->device_id, 0) >= GD_Q_ID_LOW;
}

FORCE_INLINE_ATTR bool is_mxic_chip(const esp_rom_spiflash_chip_t* chip)
{
    return BYTESHIFT(chip->device_id, 2) == MXIC_ID;
}

esp_err_t IRAM_ATTR bootloader_flash_unlock_default(void)
{
    // At the beginning status == new_status == status_sr2 == new_status_sr2 == 0.
    // If the register doesn't need to be updated, keep them the same (0), so that no command will be actually sent.
    uint16_t status = 0;    // status for SR1 or SR1+SR2 if writing SR with 01H + 2Bytes.
    uint16_t new_status = 0;
    uint8_t status_sr2 = 0;    // status_sr2 for SR2.
    uint8_t new_status_sr2 = 0;
    uint8_t sr1_bit_num = 0;
    esp_err_t err = ESP_OK;

    esp_rom_spiflash_wait_idle(&g_rom_flashchip);
    if (is_issi_chip(&g_rom_flashchip) || is_mxic_chip(&g_rom_flashchip)) {
        // Currently ISSI & MXIC share the same command and register layout, which is different from the default model.
        // If any code here needs to be modified, check both chips.
        status = bootloader_execute_flash_command(CMD_RDSR, 0, 0, 8);

        /* Clear all bits in the mask.
        (This is different from ROM esp_rom_spiflash_unlock, which keeps all bits as-is.)
        */
        sr1_bit_num = 8;
        new_status = status & (~ESP_BOOTLOADER_SPIFLASH_BP_MASK_ISSI);
    } else if (is_gd_q_chip(&g_rom_flashchip)) {
        /* The GD chips behaviour is to clear all bits in SR1 and clear bits in SR2 except QE bit.
           Use 01H to write SR1 and 31H to write SR2.
        */
        status = bootloader_execute_flash_command(CMD_RDSR, 0, 0, 8);
        sr1_bit_num = 8;
        new_status = 0;

        status_sr2 = bootloader_execute_flash_command(CMD_RDSR2, 0, 0, 8);
        new_status_sr2 = status_sr2 & ESP_BOOTLOADER_SPIFLASH_QE_GD_SR2;
    } else {
        /* For common behaviour, like XMC chips, Use 01H+2Bytes to write both SR1 and SR2*/
        status = bootloader_execute_flash_command(CMD_RDSR, 0, 0, 8) | (bootloader_execute_flash_command(CMD_RDSR2, 0, 0, 8) << 8);

        /* Clear all bits except QE, if it is set.
        (This is different from ROM esp_rom_spiflash_unlock, which keeps all bits as-is.)
        */
        sr1_bit_num = 16;
        new_status = status & ESP_BOOTLOADER_SPIFLASH_QE_SR1_2BYTE;
    }

    // When SR is written, set to true to indicate that WRDI need to be sent to ensure the protection is ON before return.
    bool status_written = false;
    // Skip if nothing needs to be changed. Meaningless writing to SR increases the risk during write and wastes time.
    if (status != new_status) {
        esp_rom_spiflash_wait_idle(&g_rom_flashchip);
        bootloader_execute_flash_command(CMD_WREN, 0, 0, 0);
        bootloader_execute_flash_command(CMD_WRSR, new_status, sr1_bit_num, 0);
        status_written = true;
    }

    if (status_sr2 != new_status_sr2) {
        esp_rom_spiflash_wait_idle(&g_rom_flashchip);
        bootloader_execute_flash_command(CMD_WREN, 0, 0, 0);
        bootloader_execute_flash_command(CMD_WRSR2, new_status_sr2, 8, 0);
        status_written = true;
    }

    if (status_written) {
        //Call esp_rom_spiflash_wait_idle to make sure previous WRSR is completed.
        esp_rom_spiflash_wait_idle(&g_rom_flashchip);
        bootloader_execute_flash_command(CMD_WRDI, 0, 0, 0);
    }

    return err;
}

esp_err_t __attribute__((weak, alias("bootloader_flash_unlock_default"))) bootloader_flash_unlock(void);


#if CONFIG_SECURE_TEE_EXT_FLASH_MEMPROT_SPI1 && !NON_OS_BUILD
extern uint32_t bootloader_flash_execute_command_common(
    uint8_t command,
    uint32_t addr_len, uint32_t address,
    uint8_t dummy_len,
    uint8_t mosi_len, uint32_t mosi_data,
    uint8_t miso_len);
#else
IRAM_ATTR uint32_t bootloader_flash_execute_command_common(
    uint8_t command,
    uint32_t addr_len, uint32_t address,
    uint8_t dummy_len,
    uint8_t mosi_len, uint32_t mosi_data,
    uint8_t miso_len)
{
    assert(mosi_len <= 32);
    assert(miso_len <= 32);
    uint32_t old_ctrl_reg = 0;
    uint32_t old_user_reg = 0;
    uint32_t old_user1_reg = 0;
    uint32_t old_user2_reg = 0;
    spi_flash_ll_get_common_command_register_info(&SPIMEM_LL_APB, &old_ctrl_reg, &old_user_reg, &old_user1_reg, &old_user2_reg);
    SPIMEM_LL_APB.ctrl.val = 0;
#if CONFIG_IDF_TARGET_ESP32
    spi_flash_ll_set_wp_level(&SPIMEM_LL_APB, true);
#else
    spimem_flash_ll_set_wp_level(&SPIMEM_LL_APB, true);
#endif
    //command phase
    spi_flash_ll_set_command(&SPIMEM_LL_APB, command, 8);
    //addr phase
    spi_flash_ll_set_addr_bitlen(&SPIMEM_LL_APB, addr_len);
    spi_flash_ll_set_usr_address(&SPIMEM_LL_APB, address, addr_len);
    //dummy phase
    uint32_t total_dummy = dummy_len;
    if (miso_len > 0) {
        total_dummy += g_rom_spiflash_dummy_len_plus[1];
    }
    spi_flash_ll_set_dummy(&SPIMEM_LL_APB, total_dummy);
    //output data

    spi_flash_ll_set_mosi_bitlen(&SPIMEM_LL_APB, mosi_len);
    spi_flash_ll_set_buffer_data(&SPIMEM_LL_APB, &mosi_data, mosi_len / 8);
    //input data
    spi_flash_ll_set_miso_bitlen(&SPIMEM_LL_APB, miso_len);

    spi_flash_ll_user_start(&SPIMEM_LL_APB, false);
    while(!spi_flash_ll_cmd_is_done(&SPIMEM_LL_APB)) {
    }
    spi_flash_ll_set_common_command_register_info(&SPIMEM_LL_APB, old_ctrl_reg, old_user_reg, old_user1_reg, old_user2_reg);

    uint32_t output_data = 0;
    spi_flash_ll_get_buffer_data(&SPIMEM_LL_APB, &output_data, miso_len / 8);
    uint32_t ret = output_data;
    if (miso_len < 32) {
        //set unused bits to 0
        ret &= ~(UINT32_MAX << miso_len);
    }
    return ret;
}
#endif

uint32_t IRAM_ATTR bootloader_execute_flash_command(uint8_t command, uint32_t mosi_data, uint8_t mosi_len, uint8_t miso_len)
{
    const uint8_t addr_len = 0;
    const uint8_t address = 0;
    const uint8_t dummy_len = 0;

    return bootloader_flash_execute_command_common(command, addr_len, address,
            dummy_len, mosi_len, mosi_data, miso_len);
}

// cmd(0x5A) + 24bit address + 8 cycles dummy
uint32_t IRAM_ATTR bootloader_flash_read_sfdp(uint32_t sfdp_addr, unsigned int miso_byte_num)
{
    assert(miso_byte_num <= 4);
    const uint8_t command = CMD_RDSFDP;
    const uint8_t addr_len = 24;
    const uint8_t dummy_len = 8;
    const uint8_t mosi_len = 0;
    const uint32_t mosi_data = 0;
    const uint8_t miso_len = miso_byte_num * 8;

    return bootloader_flash_execute_command_common(command, addr_len, sfdp_addr,
            dummy_len, mosi_len, mosi_data, miso_len);
}

void bootloader_enable_wp(void)
{
    bootloader_execute_flash_command(CMD_WRDI, 0, 0, 0);   /* Exit OTP mode */
}

uint32_t IRAM_ATTR bootloader_read_flash_id(void)
{
    uint32_t id = bootloader_execute_flash_command(CMD_RDID, 0, 0, 24);
    id = ((id & 0xff) << 16) | ((id >> 16) & 0xff) | (id & 0xff00);
    return id;
}

void bootloader_spi_flash_reset(void)
{
    bootloader_execute_flash_command(CMD_RESETEN, 0, 0, 0);
    bootloader_execute_flash_command(CMD_RESET, 0, 0, 0);
}

/*******************************************************************************
 * XMC startup flow
 ******************************************************************************/

#define XMC_SUPPORT CONFIG_BOOTLOADER_FLASH_XMC_SUPPORT
#define XMC_VENDOR_ID_1 0x20

#if NON_OS_BUILD
#define BOOTLOADER_FLASH_LOG(level, ...)    ESP_EARLY_LOG##level(TAG, ##__VA_ARGS__)
#else
static DRAM_ATTR char bootloader_flash_tag[] = "bootloader_flash";
#define BOOTLOADER_FLASH_LOG(level, ...)    ESP_DRAM_LOG##level(bootloader_flash_tag, ##__VA_ARGS__)
#endif

#if XMC_SUPPORT
//strictly check the model
static IRAM_ATTR bool is_xmc_chip_strict(uint32_t rdid)
{
    uint32_t vendor_id = BYTESHIFT(rdid, 2);
    uint32_t mfid = BYTESHIFT(rdid, 1);
    uint32_t cpid = BYTESHIFT(rdid, 0);

    if (vendor_id != XMC_VENDOR_ID_1) {
        return false;
    }

    bool matched = false;
    if (mfid == 0x40) {
        if (cpid >= 0x13 && cpid <= 0x20) {
            matched = true;
        }
    } else if (mfid == 0x41) {
        if (cpid >= 0x17 && cpid <= 0x20) {
            matched = true;
        }
    } else if (mfid == 0x50) {
        if (cpid >= 0x15 && cpid <= 0x16) {
            matched =  true;
        }
    }
    return matched;
}

esp_err_t IRAM_ATTR bootloader_flash_xmc_startup(void)
{
    // If the RDID value is a valid XMC one, may skip the flow
    const bool fast_check = true;
    if (fast_check && is_xmc_chip_strict(g_rom_flashchip.device_id)) {
        BOOTLOADER_FLASH_LOG(D, "XMC chip detected by RDID (%08" PRIX32 "), skip.", g_rom_flashchip.device_id);
        return ESP_OK;
    }

    // Check the Manufacturer ID in SFDP registers (JEDEC standard). If not XMC chip, no need to run the flow
    const int sfdp_mfid_addr = 0x10;
    uint8_t mf_id = (bootloader_flash_read_sfdp(sfdp_mfid_addr, 1) & 0xff);
    if (mf_id != XMC_VENDOR_ID_1) {
        BOOTLOADER_FLASH_LOG(D, "non-XMC chip detected by SFDP Read (%02X), skip.", mf_id);
        return ESP_OK;
    }

    BOOTLOADER_FLASH_LOG(I, "XM25QHxxC startup flow");
    // Enter DPD
    bootloader_execute_flash_command(0xB9, 0, 0, 0);
    // Enter UDPD
    bootloader_execute_flash_command(0x79, 0, 0, 0);
    // Exit UDPD
    bootloader_execute_flash_command(0xFF, 0, 0, 0);
    // Delay tXUDPD
    esp_rom_delay_us(2000);
    // Release Power-down
    bootloader_execute_flash_command(0xAB, 0, 0, 0);
    esp_rom_delay_us(20);
    // Read flash ID and check again
    g_rom_flashchip.device_id = bootloader_read_flash_id();
    if (!is_xmc_chip_strict(g_rom_flashchip.device_id)) {
        BOOTLOADER_FLASH_LOG(E, "XMC flash startup fail");
        return ESP_FAIL;
    }

    return ESP_OK;
}

#else
//only compare the vendor id
static IRAM_ATTR bool is_xmc_chip(uint32_t rdid)
{
    uint32_t vendor_id = (rdid >> 16) & 0xFF;
    return (vendor_id == XMC_VENDOR_ID_1);
}

esp_err_t IRAM_ATTR bootloader_flash_xmc_startup(void)
{
    if (is_xmc_chip(g_rom_flashchip.device_id)) {
        BOOTLOADER_FLASH_LOG(E, "XMC chip detected (%08X) while support disabled.", g_rom_flashchip.device_id);
        return ESP_FAIL;
    }
    return ESP_OK;
}

#endif //XMC_SUPPORT

esp_err_t IRAM_ATTR bootloader_flash_reset_chip(void)
{
    spi_flash_ll_sync_reset();
    // Seems that sync_reset cannot make host totally idle.'
    // Sending an extra(useless) command to make the host idle in order to send reset command.
    bootloader_execute_flash_command(0x05, 0, 0, 0);
#if CONFIG_IDF_TARGET_ESP32
    if (SPI1.ext2.st != 0)
#else
    if (!spimem_flash_ll_host_idle(&SPIMEM1))
#endif
    {
        return ESP_FAIL;
    }
    bootloader_execute_flash_command(0x66, 0, 0, 0);
    bootloader_execute_flash_command(0x99, 0, 0, 0);

    return ESP_OK;
}

bool IRAM_ATTR bootloader_flash_is_octal_mode_enabled(void)
{
#if SOC_SPI_MEM_SUPPORT_FLASH_OPI_MODE
    return efuse_ll_get_flash_type();
#else
    return false;
#endif
}

esp_rom_spiflash_read_mode_t bootloader_flash_get_spi_mode(void)
{
    esp_rom_spiflash_read_mode_t spi_mode = ESP_ROM_SPIFLASH_FASTRD_MODE;
    uint32_t spi_ctrl = spi_flash_ll_get_ctrl_val(&SPIMEM_LL_CACHE);
#if CONFIG_IDF_TARGET_ESP32
    if (spi_ctrl & SPI_FREAD_QIO) {
        spi_mode = ESP_ROM_SPIFLASH_QIO_MODE;
    } else if (spi_ctrl & SPI_FREAD_QUAD) {
        spi_mode = ESP_ROM_SPIFLASH_QOUT_MODE;
    } else if (spi_ctrl & SPI_FREAD_DIO) {
        spi_mode = ESP_ROM_SPIFLASH_DIO_MODE;
    } else if (spi_ctrl & SPI_FREAD_DUAL) {
        spi_mode = ESP_ROM_SPIFLASH_DOUT_MODE;
    } else if (spi_ctrl & SPI_FASTRD_MODE) {
        spi_mode = ESP_ROM_SPIFLASH_FASTRD_MODE;
    } else {
        spi_mode = ESP_ROM_SPIFLASH_SLOWRD_MODE;
    }
#else
    if (spi_ctrl & SPI_MEM_FREAD_QIO) {
        spi_mode = ESP_ROM_SPIFLASH_QIO_MODE;
    } else if (spi_ctrl & SPI_MEM_FREAD_QUAD) {
        spi_mode = ESP_ROM_SPIFLASH_QOUT_MODE;
    } else if (spi_ctrl & SPI_MEM_FREAD_DIO) {
        spi_mode = ESP_ROM_SPIFLASH_DIO_MODE;
    } else if (spi_ctrl & SPI_MEM_FREAD_DUAL) {
        spi_mode = ESP_ROM_SPIFLASH_DOUT_MODE;
    } else if (spi_ctrl & SPI_MEM_FASTRD_MODE) {
        spi_mode = ESP_ROM_SPIFLASH_FASTRD_MODE;
    } else {
        spi_mode = ESP_ROM_SPIFLASH_SLOWRD_MODE;
    }
#endif
    return spi_mode;
}
