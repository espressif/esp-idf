#include "esp_spi_flash.h"
#include "esp_partition.h"

#include "esp_err.h"
#include "esp32/rom/spi_flash.h"

extern void _spi_flash_init(const char* chip_size, size_t block_size, size_t sector_size, size_t page_size, const char* partition_bin);

void spi_flash_init(const char* chip_size, size_t block_size, size_t sector_size, size_t page_size, const char* partition_bin)
{
    _spi_flash_init(chip_size, block_size, sector_size, page_size, partition_bin);
}

bool spi_flash_check_and_flush_cache(size_t start_addr, size_t length)
{
    return true;
}

esp_rom_spiflash_result_t esp_rom_spiflash_unlock()
{
    return ESP_ROM_SPIFLASH_RESULT_OK;
}

void spi_flash_init_lock()
{
    return;
}

void spi_flash_op_lock()
{
    return;
}

void spi_flash_op_unlock()
{
    return;
}

void spi_flash_disable_interrupts_caches_and_other_cpu()
{
    return;
}

void spi_flash_enable_interrupts_caches_and_other_cpu()
{
    return;
}

void spi_flash_disable_interrupts_caches_and_other_cpu_no_os()
{
    return;
}

void spi_flash_enable_interrupts_caches_no_os()
{
    return;
}
