// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "spi_flash_hal_common.inc"

void spi_flash_hal_erase_chip(spi_flash_host_driver_t *host)
{
    spi_dev_t *dev = get_spi_dev(host);
    spi_flash_ll_erase_chip(dev);
    host->poll_cmd_done(host);
}

void spi_flash_hal_erase_sector(spi_flash_host_driver_t *host, uint32_t start_address)
{
    spi_dev_t *dev = get_spi_dev(host);
    spi_flash_ll_set_addr_bitlen(dev, 24);
    spi_flash_ll_set_address(dev, start_address & ADDRESS_MASK_24BIT);
    spi_flash_ll_erase_sector(dev);
    host->poll_cmd_done(host);
}

void spi_flash_hal_erase_block(spi_flash_host_driver_t *host, uint32_t start_address)
{
    spi_dev_t *dev = get_spi_dev(host);
    spi_flash_ll_set_addr_bitlen(dev, 24);
    spi_flash_ll_set_address(dev, start_address & ADDRESS_MASK_24BIT);
    spi_flash_ll_erase_block(dev);
    host->poll_cmd_done(host);
}

void spi_flash_hal_program_page(spi_flash_host_driver_t *host, const void *buffer, uint32_t address, uint32_t length)
{
    spi_dev_t *dev = get_spi_dev(host);
    spi_flash_ll_set_addr_bitlen(dev, 24);
    spi_flash_ll_set_address(dev, (address & ADDRESS_MASK_24BIT) | (length << 24));
    spi_flash_ll_program_page(dev, buffer, length);
    host->poll_cmd_done(host);
}

esp_err_t spi_flash_hal_set_write_protect(spi_flash_host_driver_t *host, bool wp)
{
    spi_dev_t *dev = get_spi_dev(host);
    spi_flash_ll_set_write_protect(dev, wp);
    host->poll_cmd_done(host);
    return ESP_OK;
}

bool spi_flash_hal_host_idle(spi_flash_host_driver_t *chip_drv)
{
    spi_dev_t *dev = get_spi_dev(chip_drv);
    bool idle = spi_flash_ll_host_idle(dev);

    // Not clear if this is necessary, or only necessary if
    // chip->spi == SPI1. But probably doesn't hurt...
    if ((void*) dev == spi_flash_ll_get_hw(SPI_HOST)) {
#if CONFIG_IDF_TARGET_ESP32
        idle &= spi_flash_ll_host_idle(&SPI0);
#elif CONFIG_IDF_TARGET_ESP32S2BETA
        idle &= spi_flash_ll_host_idle(&SPIMEM0);
#endif
    }

    return idle;
}