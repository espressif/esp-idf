#include <string.h>

#include "unity.h"
#include "esp_spi_flash.h"
#include "esp_ota_ops.h"

#if CONFIG_SPI_FLASH_WRITING_DANGEROUS_REGIONS_ABORTS || CONFIG_SPI_FLASH_WRITING_DANGEROUS_REGIONS_FAILS

static const char *data = "blah blah blah";

#if CONFIG_SPI_FLASH_WRITING_DANGEROUS_REGIONS_FAILS
#define TEST_TAGS "[spi_flash]"
#else // ABORTS
#define TEST_TAGS "[spi_flash][ignore]"
#endif

TEST_CASE("can't overwrite bootloader", TEST_TAGS)
{
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, spi_flash_write(0x1000, data, strlen(data)));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, spi_flash_write(0x0FF8, data, strlen(data)));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, spi_flash_write(0x1400, data, strlen(data)));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, spi_flash_erase_range(0x8000, 0x2000));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, spi_flash_erase_range(0x7000, 0x2000));
}

TEST_CASE("can't overwrite current running app", TEST_TAGS)
{
    const esp_partition_t *p = esp_ota_get_running_partition();
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, spi_flash_write(p->address + 1024, data, strlen(data)));
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, spi_flash_erase_range(p->address + 4096, 8192));
}

#endif // FAILS || ABORTS
