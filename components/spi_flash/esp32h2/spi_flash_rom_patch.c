// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "sdkconfig.h"
#include "esp32h2/rom/spi_flash.h"
#include "soc/spi_periph.h"
#include "spi_flash_defs.h"

#define SPI_IDX   1

esp_rom_spiflash_result_t esp_rom_spiflash_write_disable(void)
{
    REG_WRITE(SPI_MEM_CMD_REG(SPI_IDX), SPI_MEM_FLASH_WRDI);
    while (READ_PERI_REG(PERIPHS_SPI_FLASH_CMD) != 0);
    return ESP_ROM_SPIFLASH_RESULT_OK;
}
