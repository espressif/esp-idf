#include "esp_spi_flash.h"
#include "esp_partition.h"

void init_spi_flash(const char* chip_size, size_t block_size, size_t sector_size, size_t page_size, const char* partition_bin)
{
    spi_flash_init(chip_size, block_size, sector_size, page_size, partition_bin);
}
