#include "esp_spi_flash.h"

int spi_flash_get_chip_size(void)
{
    return (1024 * 1024 * 1024);
}