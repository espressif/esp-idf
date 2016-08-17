// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <esp_spi_flash.h>
#include <rom/spi_flash.h>
#include <rom/cache.h>
#include <esp_attr.h>
#include <soc/dport_reg.h>

static esp_err_t spi_flash_translate_rc(SpiFlashOpResult rc)
{
    switch (rc) {
    case SPI_FLASH_RESULT_OK:
        return ESP_OK;
    case SPI_FLASH_RESULT_TIMEOUT:
        return ESP_ERR_FLASH_OP_TIMEOUT;
    case SPI_FLASH_RESULT_ERR:
    default:
        return ESP_ERR_FLASH_OP_FAIL;
    }
}

extern void Cache_Flush(int);

static void IRAM_ATTR spi_flash_disable_interrupts_caches_and_app_cpu()
{
    vTaskSuspendAll();
    // SET_PERI_REG_MASK(APPCPU_CTRL_REG_C, DPORT_APPCPU_RUNSTALL);
    Cache_Read_Disable(0);
    Cache_Read_Disable(1);
}

static void IRAM_ATTR spi_flash_enable_interrupts_caches_and_app_cpu()
{
    Cache_Read_Enable(0);
    Cache_Read_Enable(1);
    // CLEAR_PERI_REG_MASK(APPCPU_CTRL_REG_C, DPORT_APPCPU_RUNSTALL);
    xTaskResumeAll();
}

esp_err_t IRAM_ATTR spi_flash_erase_sector(uint16_t sec)
{
    spi_flash_disable_interrupts_caches_and_app_cpu();
    SpiFlashOpResult rc;
    if (xPortGetCoreID() == 1) {
        rc = SPI_FLASH_RESULT_ERR;
    }
    else {
        rc = SPIUnlock();
        if (rc == SPI_FLASH_RESULT_OK) {
            rc = SPIEraseSector(sec);
        }
    }
    Cache_Flush(0);
    Cache_Flush(1);
    spi_flash_enable_interrupts_caches_and_app_cpu();
    return spi_flash_translate_rc(rc);
}

esp_err_t IRAM_ATTR spi_flash_write(uint32_t dest_addr, const uint32_t *src, uint32_t size)
{
    spi_flash_disable_interrupts_caches_and_app_cpu();
    SpiFlashOpResult rc;
    if (xPortGetCoreID() == 1) {
        rc = SPI_FLASH_RESULT_ERR;
    }
    else {
        rc = SPIUnlock();
        if (rc == SPI_FLASH_RESULT_OK) {
            rc = SPIWrite(dest_addr, src, (int32_t) size);
        }
    }
    Cache_Flush(0);
    Cache_Flush(1);
    spi_flash_enable_interrupts_caches_and_app_cpu();
    return spi_flash_translate_rc(rc);
}

esp_err_t IRAM_ATTR spi_flash_read(uint32_t src_addr, uint32_t *dest, uint32_t size)
{
    spi_flash_disable_interrupts_caches_and_app_cpu();
    SpiFlashOpResult rc;
    if (xPortGetCoreID() == 1) {
        rc = SPI_FLASH_RESULT_ERR;
    }
    else {
        rc = SPIRead(src_addr, dest, (int32_t) size);
    }
    spi_flash_enable_interrupts_caches_and_app_cpu();
    return spi_flash_translate_rc(rc);
}
