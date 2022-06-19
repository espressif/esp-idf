#include "SpiFlash.h"

#include <string.h>
#include <stdlib.h>

#include "esp_spi_flash.h"
#include "esp_partition.h"

#include "esp_err.h"
#include "esp32/rom/spi_flash.h"

SpiFlash spiflash = SpiFlash();

esp_rom_spiflash_chip_t g_rom_flashchip;

size_t convert_chip_size_string(const char* chip_size_str)
{
    int size = 0;
    if (strcmp(chip_size_str, "1MB") == 0) {
        size = 0x100000;
    }
    else if (strcmp(chip_size_str, "2MB") == 0) {
        size = 0x200000;
    } 
    else if (strcmp(chip_size_str, "4MB") == 0) {
        size = 0x400000;
    }
    else if (strcmp(chip_size_str, "8MB") == 0) {
        size = 0x800000;
    }
    else if (strcmp(chip_size_str, "16MB") == 0) {
        size = 0x1000000;
    } else {
        size = 0;
    }

    return size;
}

extern "C" void _spi_flash_init(const char* chip_size, size_t block_size, size_t sector_size, size_t page_size, const char* partitions_bin)
{
    size_t size = convert_chip_size_string(chip_size);

    assert(size != 0);

    spiflash.init(size, block_size, sector_size, page_size, partitions_bin);

    g_rom_flashchip.chip_size = size;
    g_rom_flashchip.block_size = block_size;
    g_rom_flashchip.sector_size = sector_size;
    g_rom_flashchip.page_size = page_size;
}

extern "C" esp_err_t spi_flash_mmap(size_t src_addr, size_t size, spi_flash_mmap_memory_t memory,
                         const void** out_ptr, spi_flash_mmap_handle_t* out_handle)
{
    *out_handle = 0;
    *out_ptr = (void*)spiflash.get_memory_ptr(src_addr);

    return ESP_OK;
}

extern "C" void spi_flash_munmap(spi_flash_mmap_handle_t handle)
{
    return;
}

extern "C" int spi_flash_get_total_erase_cycles(void)
{
    return spiflash.get_total_erase_cycles();
}

extern "C" int spi_flash_get_erase_cycles(size_t sector)
{
    return spiflash.get_erase_cycles(sector);
}

extern "C" esp_err_t bootloader_flash_unlock(void)
{
    return ESP_OK;
}

esp_rom_spiflash_result_t esp_rom_spiflash_read(uint32_t target, uint32_t *dest, int32_t len)
{
    return spiflash.read(target, dest, len);
}

esp_rom_spiflash_result_t esp_rom_spiflash_erase_block(uint32_t block)
{
    return spiflash.erase_block(block);
}

esp_rom_spiflash_result_t esp_rom_spiflash_erase_sector(uint32_t sector)
{
    return spiflash.erase_sector(sector);
}

esp_rom_spiflash_result_t esp_rom_spiflash_erase_page(uint32_t page)
{
    return spiflash.erase_page(page);
}

esp_rom_spiflash_result_t esp_rom_spiflash_write(uint32_t target, const uint32_t *src, int32_t len)
{
    return spiflash.write(target, src, len);
}

esp_rom_spiflash_result_t esp_rom_spiflash_write_encrypted(uint32_t flash_addr, uint32_t *data, uint32_t len)
{
    return spiflash.write(flash_addr, data, len);
}

void *heap_caps_malloc( size_t size, uint32_t caps )
{
    return NULL;
}

esp_flash_t* esp_flash_default_chip = NULL;
