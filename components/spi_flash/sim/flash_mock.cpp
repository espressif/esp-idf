#include "SpiFlash.h"

#include "esp_spi_flash.h"
#include "esp_partition.h"

#include "esp_err.h"
#include "rom/spi_flash.h"

static SpiFlash spiflash = SpiFlash();

esp_rom_spiflash_chip_t g_rom_flashchip;

extern "C" void _spi_flash_init(size_t chip_size, size_t block_size, size_t sector_size, size_t page_size, const char* partitions_bin)
{
    spiflash.init(chip_size, block_size, sector_size, page_size, partitions_bin);

    g_rom_flashchip.chip_size = chip_size;
    g_rom_flashchip.block_size = block_size;
    g_rom_flashchip.sector_size = sector_size;
    g_rom_flashchip.page_size = page_size;
}

esp_err_t spi_flash_mmap(size_t src_addr, size_t size, spi_flash_mmap_memory_t memory,
                         const void** out_ptr, spi_flash_mmap_handle_t* out_handle)
{
    *out_handle = 0;
    *out_ptr = (void*)spiflash.get_memory_ptr(src_addr);

    return ESP_OK;
}

void spi_flash_munmap(spi_flash_mmap_handle_t handle)
{
    return;
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