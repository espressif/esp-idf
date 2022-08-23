/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>

#include <bootloader_flash_priv.h>
#include <esp_log.h>
#include <esp_flash_encrypt.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"

#if CONFIG_IDF_TARGET_ESP32
#   include "soc/spi_struct.h"
#   include "soc/spi_reg.h"
    /* SPI flash controller */
#   define SPIFLASH SPI1
#else
#   include "soc/spi_mem_struct.h"
#   include "soc/spi_mem_reg.h"
    /* SPI flash controller */
#   define SPIFLASH SPIMEM1
#endif

// This dependency will be removed in the future.  IDF-5025
#include "esp_flash.h"

#include "esp_rom_spiflash.h"

#ifdef CONFIG_EFUSE_VIRTUAL_KEEP_IN_FLASH
#define ENCRYPTION_IS_VIRTUAL 1
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

#ifndef BOOTLOADER_BUILD
/* Normal app version maps to spi_flash_mmap.h operations...
 */
static const char *TAG = "bootloader_mmap";

static spi_flash_mmap_handle_t map;

uint32_t bootloader_mmap_get_free_pages(void)
{
    return spi_flash_mmap_get_free_pages(SPI_FLASH_MMAP_DATA);
}

const void *bootloader_mmap(uint32_t src_addr, uint32_t size)
{
    if (map) {
        ESP_LOGE(TAG, "tried to bootloader_mmap twice");
        return NULL; /* existing mapping in use... */
    }
    const void *result = NULL;
    uint32_t src_page = src_addr & ~(SPI_FLASH_MMU_PAGE_SIZE - 1);
    size += (src_addr - src_page);
    esp_err_t err = spi_flash_mmap(src_page, size, SPI_FLASH_MMAP_DATA, &result, &map);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "spi_flash_mmap failed: 0x%x", err);
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

#else //BOOTLOADER_BUILD
/* Bootloader version, uses ROM functions only */
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/cache.h"
#endif
#include "esp_rom_spiflash.h"
#include "hal/mmu_hal.h"
#include "hal/mmu_ll.h"
#include "hal/cache_hal.h"
static const char *TAG = "bootloader_flash";

#if CONFIG_IDF_TARGET_ESP32
/* Use first 50 blocks in MMU for bootloader_mmap,
   50th block for bootloader_flash_read
*/
#define MMU_BLOCK0_VADDR  SOC_DROM_LOW
#define MMU_SIZE          (0x320000)
#define MMU_BLOCK50_VADDR (MMU_BLOCK0_VADDR + MMU_SIZE)
#define FLASH_READ_VADDR MMU_BLOCK50_VADDR

#else // !CONFIG_IDF_TARGET_ESP32

/* Use first 63 blocks in MMU for bootloader_mmap,
   63th block for bootloader_flash_read
*/
#define MMU_BLOCK0_VADDR  SOC_DROM_LOW
#ifdef SOC_MMU_PAGE_SIZE_CONFIGURABLE
#define MMU_SIZE          (DRAM0_CACHE_ADDRESS_HIGH(SPI_FLASH_MMU_PAGE_SIZE) - DRAM0_CACHE_ADDRESS_LOW - SPI_FLASH_MMU_PAGE_SIZE) // This mmu size means that the mmu size to be mapped
#else
#define MMU_SIZE          (DRAM0_CACHE_ADDRESS_HIGH - DRAM0_CACHE_ADDRESS_LOW - SPI_FLASH_MMU_PAGE_SIZE) // This mmu size means that the mmu size to be mapped
#endif
#define MMU_BLOCK63_VADDR (MMU_BLOCK0_VADDR + MMU_SIZE)
#define FLASH_READ_VADDR MMU_BLOCK63_VADDR
#endif

#define MMU_FREE_PAGES    (MMU_SIZE / FLASH_BLOCK_SIZE)

static bool mapped;

// Current bootloader mapping (ab)used for bootloader_read()
static uint32_t current_read_mapping = UINT32_MAX;

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
    if (size > MMU_SIZE) {
        ESP_EARLY_LOGE(TAG, "bootloader_mmap excess size %x", size);
        return NULL;
    }

    uint32_t src_paddr_aligned = src_paddr & MMU_FLASH_MASK;
    //The addr is aligned, so we add the mask off length to the size, to make sure the corresponding buses are enabled.
    uint32_t size_after_paddr_aligned = (src_paddr - src_paddr_aligned) + size;
    /**
     * @note 1
     * Will add here a check to make sure the vaddr is on read-only and executable buses, since we use others for psram
     * Now simply check if it's valid vaddr, didn't check if it's readable, writable or executable.
     * TODO: IDF-4710
     */
    if (mmu_ll_check_valid_ext_vaddr_region(0, MMU_BLOCK0_VADDR, size_after_paddr_aligned) == 0) {
        ESP_EARLY_LOGE(TAG, "vaddr not valid");
        return NULL;
    }

    //-------------stop cache to do the MMU mapping--------------
#if CONFIG_IDF_TARGET_ESP32
    Cache_Read_Disable(0);
    Cache_Flush(0);
#else
    cache_hal_disable(CACHE_TYPE_ALL);
#endif

    //---------------Do mapping------------------------
    ESP_EARLY_LOGD(TAG, "rodata starts from paddr=0x%08x, size=0x%x, will be mapped to vaddr=0x%08x", src_paddr, size, MMU_BLOCK0_VADDR);
#if CONFIG_IDF_TARGET_ESP32
    uint32_t count = GET_REQUIRED_MMU_PAGES(size, src_paddr);
    int e = cache_flash_mmu_set(0, 0, MMU_BLOCK0_VADDR, src_paddr_aligned, 64, count);
    ESP_EARLY_LOGV(TAG, "after mapping, starting from paddr=0x%08x and vaddr=0x%08x, 0x%x bytes are mapped", src_paddr_aligned, MMU_BLOCK0_VADDR, count * SPI_FLASH_MMU_PAGE_SIZE);
    if (e != 0) {
        ESP_EARLY_LOGE(TAG, "cache_flash_mmu_set failed: %d\n", e);
        Cache_Read_Enable(0);
        return NULL;
    }
#else
    /**
     * This hal won't return error, it assumes the inputs are valid. The related check should be done in `bootloader_mmap()`.
     * See above comments (note 1) about IDF-4710
     */
    uint32_t actual_mapped_len = 0;
    mmu_hal_map_region(0, MMU_TARGET_FLASH0, MMU_BLOCK0_VADDR, src_paddr_aligned, size_after_paddr_aligned, &actual_mapped_len);
    ESP_EARLY_LOGV(TAG, "after mapping, starting from paddr=0x%08x and vaddr=0x%08x, 0x%x bytes are mapped", src_paddr_aligned, MMU_BLOCK0_VADDR, actual_mapped_len);
#endif

    /**
     * If after mapping, your code stucks, it possibly means that some of the buses are not enabled, check `cache_ll_l1_enable_bus()`
     * For now, keep this unchanged.
     */

    //-------------enable cache--------------
#if CONFIG_IDF_TARGET_ESP32
    Cache_Read_Enable(0);
#else
    cache_hal_enable(CACHE_TYPE_ALL);
#endif

    mapped = true;

    return (void *)(MMU_BLOCK0_VADDR + (src_paddr - src_paddr_aligned));
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
        cache_hal_disable(CACHE_TYPE_ALL);
        mmu_hal_init();
#endif
        mapped = false;
        current_read_mapping = UINT32_MAX;
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
    cache_hal_disable(CACHE_TYPE_ALL);
#endif

    esp_rom_spiflash_result_t r = esp_rom_spiflash_read(src_addr, dest, size);

#if CONFIG_IDF_TARGET_ESP32
    Cache_Read_Enable(0);
#else
    cache_hal_enable(CACHE_TYPE_ALL);
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
            cache_hal_disable(CACHE_TYPE_ALL);
#endif

            //---------------Do mapping------------------------
            ESP_EARLY_LOGD(TAG, "mmu set block paddr=0x%08x (was 0x%08x)", map_at, current_read_mapping);
#if CONFIG_IDF_TARGET_ESP32
            //Should never fail if we only map a SPI_FLASH_MMU_PAGE_SIZE to the vaddr starting from FLASH_READ_VADDR
            int e = cache_flash_mmu_set(0, 0, FLASH_READ_VADDR, map_at, 64, 1);
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
            cache_hal_enable(CACHE_TYPE_ALL);
#endif
        }
        map_ptr = (uint32_t *)(FLASH_READ_VADDR + (word_src - map_at));
        dest_words[word] = *map_ptr;
    }
    return ESP_OK;
}

esp_err_t bootloader_flash_read(size_t src_addr, void *dest, size_t size, bool allow_decrypt)
{
    if (src_addr & 3) {
        ESP_LOGE(TAG, "bootloader_flash_read src_addr 0x%x not 4-byte aligned", src_addr);
        return ESP_FAIL;
    }
    if (size & 3) {
        ESP_LOGE(TAG, "bootloader_flash_read size 0x%x not 4-byte aligned", size);
        return ESP_FAIL;
    }
    if ((intptr_t)dest & 3) {
        ESP_LOGE(TAG, "bootloader_flash_read dest 0x%x not 4-byte aligned", (intptr_t)dest);
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
    esp_err_t err;
    size_t alignment = write_encrypted ? 32 : 4;
    if ((dest_addr % alignment) != 0) {
        ESP_LOGE(TAG, "bootloader_flash_write dest_addr 0x%x not %d-byte aligned", dest_addr, alignment);
        return ESP_FAIL;
    }
    if ((size % alignment) != 0) {
        ESP_LOGE(TAG, "bootloader_flash_write size 0x%x not %d-byte aligned", size, alignment);
        return ESP_FAIL;
    }
    if (((intptr_t)src % 4) != 0) {
        ESP_LOGE(TAG, "bootloader_flash_write src 0x%x not 4 byte aligned", (intptr_t)src);
        return ESP_FAIL;
    }

    err = bootloader_flash_unlock();
    if (err != ESP_OK) {
        return err;
    }

    if (write_encrypted && !ENCRYPTION_IS_VIRTUAL) {
        return spi_to_esp_err(esp_rom_spiflash_write_encrypted(dest_addr, src, size));
    } else {
        return spi_to_esp_err(esp_rom_spiflash_write(dest_addr, src, size));
    }
}

esp_err_t bootloader_flash_erase_sector(size_t sector)
{
    return spi_to_esp_err(esp_rom_spiflash_erase_sector(sector));
}

esp_err_t bootloader_flash_erase_range(uint32_t start_addr, uint32_t size)
{
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
    return spi_to_esp_err(rc);
}

#endif // BOOTLOADER_BUILD


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

esp_err_t IRAM_ATTR __attribute__((weak)) bootloader_flash_unlock(void)
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

IRAM_ATTR uint32_t bootloader_flash_execute_command_common(
    uint8_t command,
    uint32_t addr_len, uint32_t address,
    uint8_t dummy_len,
    uint8_t mosi_len, uint32_t mosi_data,
    uint8_t miso_len)
{
    assert(mosi_len <= 32);
    assert(miso_len <= 32);
    uint32_t old_ctrl_reg = SPIFLASH.ctrl.val;
    uint32_t old_user_reg = SPIFLASH.user.val;
    uint32_t old_user1_reg = SPIFLASH.user1.val;
#if CONFIG_IDF_TARGET_ESP32
    SPIFLASH.ctrl.val = SPI_WP_REG_M; // keep WP high while idle, otherwise leave DIO mode
#else
    SPIFLASH.ctrl.val = SPI_MEM_WP_REG_M; // keep WP high while idle, otherwise leave DIO mode
#endif
    //command phase
    SPIFLASH.user.usr_command = 1;
    SPIFLASH.user2.usr_command_bitlen = 7;
    SPIFLASH.user2.usr_command_value = command;
    //addr phase
    SPIFLASH.user.usr_addr = addr_len > 0;
    SPIFLASH.user1.usr_addr_bitlen = addr_len - 1;
#if CONFIG_IDF_TARGET_ESP32
    SPIFLASH.addr = (addr_len > 0)? (address << (32-addr_len)) : 0;
#else
    SPIFLASH.addr = address;
#endif
    //dummy phase
    uint32_t total_dummy = dummy_len;
    if (miso_len > 0) {
        total_dummy += g_rom_spiflash_dummy_len_plus[1];
    }
    SPIFLASH.user.usr_dummy = total_dummy > 0;
    SPIFLASH.user1.usr_dummy_cyclelen = total_dummy - 1;
    //output data
    SPIFLASH.user.usr_mosi = mosi_len > 0;
#if CONFIG_IDF_TARGET_ESP32
    SPIFLASH.mosi_dlen.usr_mosi_dbitlen = mosi_len ? (mosi_len - 1) : 0;
#else
    SPIFLASH.mosi_dlen.usr_mosi_bit_len = mosi_len ? (mosi_len - 1) : 0;
#endif
    SPIFLASH.data_buf[0] = mosi_data;
    //input data
    SPIFLASH.user.usr_miso = miso_len > 0;
#if CONFIG_IDF_TARGET_ESP32
    SPIFLASH.miso_dlen.usr_miso_dbitlen = miso_len ? (miso_len - 1) : 0;
#else
    SPIFLASH.miso_dlen.usr_miso_bit_len = miso_len ? (miso_len - 1) : 0;
#endif

    SPIFLASH.cmd.usr = 1;
    while (SPIFLASH.cmd.usr != 0) {
    }
    SPIFLASH.ctrl.val = old_ctrl_reg;
    SPIFLASH.user.val = old_user_reg;
    SPIFLASH.user1.val = old_user1_reg;

    uint32_t ret = SPIFLASH.data_buf[0];
    if (miso_len < 32) {
        //set unused bits to 0
        ret &= ~(UINT32_MAX << miso_len);
    }
    return ret;
}

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

#if SOC_CACHE_SUPPORT_WRAP
esp_err_t bootloader_flash_wrap_set(spi_flash_wrap_mode_t mode)
{
    uint32_t reg_bkp_ctrl = SPIFLASH.ctrl.val;
    uint32_t reg_bkp_usr  = SPIFLASH.user.val;
    SPIFLASH.user.fwrite_dio = 0;
    SPIFLASH.user.fwrite_dual = 0;
    SPIFLASH.user.fwrite_qio = 1;
    SPIFLASH.user.fwrite_quad = 0;
    SPIFLASH.ctrl.fcmd_dual = 0;
    SPIFLASH.ctrl.fcmd_quad = 0;
    SPIFLASH.user.usr_dummy = 0;
    SPIFLASH.user.usr_addr = 1;
    SPIFLASH.user.usr_command = 1;
    SPIFLASH.user2.usr_command_bitlen = 7;
    SPIFLASH.user2.usr_command_value = CMD_WRAP;
    SPIFLASH.user1.usr_addr_bitlen = 23;
    SPIFLASH.addr = 0;
    SPIFLASH.user.usr_miso = 0;
    SPIFLASH.user.usr_mosi = 1;
    SPIFLASH.mosi_dlen.usr_mosi_bit_len = 7;
    SPIFLASH.data_buf[0] = (uint32_t) mode << 4;;
    SPIFLASH.cmd.usr = 1;
    while(SPIFLASH.cmd.usr != 0)
    { }

    SPIFLASH.ctrl.val = reg_bkp_ctrl;
    SPIFLASH.user.val = reg_bkp_usr;
    return ESP_OK;
}
#endif //SOC_CACHE_SUPPORT_WRAP

/*******************************************************************************
 * XMC startup flow
 ******************************************************************************/

#define XMC_SUPPORT CONFIG_BOOTLOADER_FLASH_XMC_SUPPORT
#define XMC_VENDOR_ID 0x20

#if BOOTLOADER_BUILD
#define BOOTLOADER_FLASH_LOG(level, ...)    ESP_LOG##level(TAG, ##__VA_ARGS__)
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

    if (vendor_id != XMC_VENDOR_ID) {
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
        BOOTLOADER_FLASH_LOG(D, "XMC chip detected by RDID (%08X), skip.", g_rom_flashchip.device_id);
        return ESP_OK;
    }

    // Check the Manufacturer ID in SFDP registers (JEDEC standard). If not XMC chip, no need to run the flow
    const int sfdp_mfid_addr = 0x10;
    uint8_t mf_id = (bootloader_flash_read_sfdp(sfdp_mfid_addr, 1) & 0xff);
    if (mf_id != XMC_VENDOR_ID) {
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
    return (vendor_id == XMC_VENDOR_ID);
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

FORCE_INLINE_ATTR void bootloader_mspi_reset(void)
{
#if CONFIG_IDF_TARGET_ESP32
    SPI1.slave.sync_reset = 0;
    SPI0.slave.sync_reset = 0;
    SPI1.slave.sync_reset = 1;
    SPI0.slave.sync_reset = 1;
    SPI1.slave.sync_reset = 0;
    SPI0.slave.sync_reset = 0;
#else
    SPIMEM1.ctrl2.sync_reset = 0;
    SPIMEM0.ctrl2.sync_reset = 0;
    SPIMEM1.ctrl2.sync_reset = 1;
    SPIMEM0.ctrl2.sync_reset = 1;
    SPIMEM1.ctrl2.sync_reset = 0;
    SPIMEM0.ctrl2.sync_reset = 0;
#endif
}

esp_err_t IRAM_ATTR bootloader_flash_reset_chip(void)
{
    bootloader_mspi_reset();
    // Seems that sync_reset cannot make host totally idle.'
    // Sending an extra(useless) command to make the host idle in order to send reset command.
    bootloader_execute_flash_command(0x05, 0, 0, 0);
#if CONFIG_IDF_TARGET_ESP32
    if (SPI1.ext2.st != 0)
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
    if (SPIMEM1.fsm.st != 0)
#else
    if (SPIMEM1.fsm.spi0_mst_st != 0)
#endif
    {
        return ESP_FAIL;
    }
    bootloader_execute_flash_command(0x66, 0, 0, 0);
    bootloader_execute_flash_command(0x99, 0, 0, 0);

    return ESP_OK;
}
