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

#include "spi_flash_defs.h"
#include "memspi_host_driver.h"
#include "string.h"
#include "esp_log.h"
#include "cache_utils.h"
#include "esp_flash_partitions.h"

static const char TAG[] = "memspi";
static const spi_flash_host_driver_t esp_flash_default_host = ESP_FLASH_DEFAULT_HOST_DRIVER();

esp_err_t memspi_host_init_pointers(spi_flash_host_driver_t *host, memspi_host_data_t *data, const memspi_host_config_t *cfg)
{
    memcpy(host, &esp_flash_default_host, sizeof(spi_flash_host_driver_t));
    esp_err_t err = spi_flash_hal_init(data, cfg);
    if (err != ESP_OK) {
        return err;
    }

    host->driver_data = data;
    //some functions are not required if not SPI1
    if (data->spi != &SPI1) {
        host->flush_cache = NULL;
    }
    return ESP_OK;
}

esp_err_t memspi_host_read_id_hs(spi_flash_host_driver_t *host, uint32_t *id)
{
    //NOTE: we do have a read id function, however it doesn't work in high freq
    spi_flash_trans_t t = {
        .command = CMD_RDID,
        .mosi_data = 0,
        .mosi_len = 0,
        .miso_len = 24
    };
    host->common_command(host, &t);
    uint32_t raw_flash_id = t.miso_data[0];
    ESP_EARLY_LOGV(TAG, "raw_chip_id: %X\n", raw_flash_id);
    if (raw_flash_id == 0xFFFFFF || raw_flash_id == 0) {
        ESP_EARLY_LOGE(TAG, "no response\n");
        return ESP_ERR_FLASH_NO_RESPONSE;
    }

    // Byte swap the flash id as it's usually written the other way around
    uint8_t mfg_id = raw_flash_id & 0xFF;
    uint16_t flash_id = (raw_flash_id >> 16) | (raw_flash_id & 0xFF00);
    *id = ((uint32_t)mfg_id << 16) | flash_id;
    ESP_EARLY_LOGV(TAG, "chip_id: %X\n", *id);
    return ESP_OK;
}

esp_err_t memspi_host_read_status_hs(spi_flash_host_driver_t *driver, uint8_t *out_sr)
{
    //NOTE: we do have a read id function, however it doesn't work in high freq
    spi_flash_trans_t t = {
        .command = CMD_RDSR,
        .mosi_data = 0,
        .mosi_len = 0,
        .miso_len = 8
    };
    esp_err_t err = driver->common_command(driver, &t);
    if (err != ESP_OK) {
        return err;
    }
    *out_sr = t.miso_data[0];
    return ESP_OK;
}

esp_err_t memspi_host_flush_cache(spi_flash_host_driver_t* driver, uint32_t addr, uint32_t size)
{
    if (((memspi_host_data_t*)(driver->driver_data))->spi == &SPI1) {
        spi_flash_check_and_flush_cache(addr, size);
    }
    return ESP_OK;
}