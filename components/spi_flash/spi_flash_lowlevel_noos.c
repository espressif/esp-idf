// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
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
#include <stdarg.h>
#include "spi_flash_lowlevel_driver.h"

#include "rom/ets_sys.h"
#include "rom/cache.h"
#include "esp_attr.h"
#include "esp_spi_flash.h"

static esp_flash_err_t start(const esp_flash_chip_t *chip)
{
    if (chip->spi == &SPI1) {
        Cache_Read_Disable(0);
        Cache_Read_Disable(1);
    }
    return FLASH_OK;
}
static esp_flash_err_t end(const esp_flash_chip_t *chip)
{
    if (chip->spi == &SPI1) {
        Cache_Flush(0);
        Cache_Flush(1);
        Cache_Read_Enable(0);
        Cache_Read_Enable(1);
    }
    return FLASH_OK;
}

static esp_flash_err_t delay_ms(unsigned ms)
{
    ets_delay_us(1000 * ms);
    return FLASH_OK;
}

const esp_flash_os_functions_t esp_flash_noos_functions = {
    .start = start,
    .end = end,
    .delay_ms = delay_ms,
};
