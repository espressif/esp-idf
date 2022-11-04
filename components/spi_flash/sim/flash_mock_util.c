#include "esp_err.h"
#include "esp_rom_spiflash.h"

bool spi_flash_check_and_flush_cache(size_t start_addr, size_t length)
{
    return true;
}

esp_rom_spiflash_result_t esp_rom_spiflash_unlock(void)
{
    return ESP_ROM_SPIFLASH_RESULT_OK;
}

void spi_flash_init_lock(void)
{
    return;
}

void spi_flash_op_lock(void)
{
    return;
}

void spi_flash_op_unlock(void)
{
    return;
}

void spi_flash_disable_interrupts_caches_and_other_cpu(void)
{
    return;
}

void spi_flash_enable_interrupts_caches_and_other_cpu(void)
{
    return;
}

void spi_flash_disable_interrupts_caches_and_other_cpu_no_os(void)
{
    return;
}

void spi_flash_enable_interrupts_caches_no_os(void)
{
    return;
}
