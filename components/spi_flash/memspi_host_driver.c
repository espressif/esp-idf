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

#ifdef CONFIG_IDF_TARGET_ESP32S2BETA
extern void spi_flash_hal_gpspi_poll_cmd_done(spi_flash_host_driver_t *driver);
extern esp_err_t spi_flash_hal_gpspi_device_config(spi_flash_host_driver_t *driver);
esp_err_t spi_flash_hal_gpspi_configure_host_io_mode(
    spi_flash_host_driver_t *host,
    uint32_t command,
    uint32_t addr_bitlen,
    int dummy_cyclelen_base,
    esp_flash_io_mode_t io_mode);
extern esp_err_t spi_flash_hal_gpspi_common_command(spi_flash_host_driver_t *chip_drv, spi_flash_trans_t *trans);
extern esp_err_t spi_flash_hal_gpspi_read(spi_flash_host_driver_t *chip_drv, void *buffer, uint32_t address, uint32_t read_len);
extern bool spi_flash_hal_gpspi_host_idle(spi_flash_host_driver_t *chip_drv);
extern bool spi_flash_hal_gpspi_supports_direct_write(spi_flash_host_driver_t *driver, const void *p);
extern bool spi_flash_hal_gpspi_supports_direct_read(spi_flash_host_driver_t *driver, const void *p);

/** Default configuration for GPSPI */
static const spi_flash_host_driver_t esp_flash_gpspi_host = { 
        .dev_config = spi_flash_hal_gpspi_device_config, 
        .common_command = spi_flash_hal_gpspi_common_command, 
        .read_id = memspi_host_read_id_hs, 
        .erase_chip = memspi_host_erase_chip, 
        .erase_sector = memspi_host_erase_sector, 
        .erase_block = memspi_host_erase_block, 
        .read_status = memspi_host_read_status_hs, 
        .set_write_protect = memspi_host_set_write_protect, 
        .supports_direct_write = spi_flash_hal_gpspi_supports_direct_write, 
        .supports_direct_read = spi_flash_hal_gpspi_supports_direct_read, 
        .program_page = memspi_host_program_page, 
        .max_write_bytes = SPI_FLASH_HAL_MAX_WRITE_BYTES, 
        .read = spi_flash_hal_gpspi_read, 
        .max_read_bytes = SPI_FLASH_HAL_MAX_READ_BYTES, 
        .host_idle = spi_flash_hal_gpspi_host_idle, 
        .configure_host_io_mode = spi_flash_hal_gpspi_configure_host_io_mode, 
        .poll_cmd_done = spi_flash_hal_gpspi_poll_cmd_done, 
        .flush_cache = NULL, 
};
#endif

esp_err_t memspi_host_init_pointers(spi_flash_host_driver_t *host, memspi_host_data_t *data, const memspi_host_config_t *cfg)
{
#ifdef CONFIG_IDF_TARGET_ESP32
    memcpy(host, &esp_flash_default_host, sizeof(spi_flash_host_driver_t));
#elif CONFIG_IDF_TARGET_ESP32S2BETA
    if (cfg->host_id == SPI_HOST)
        memcpy(host, &esp_flash_default_host, sizeof(spi_flash_host_driver_t));
    else {
        memcpy(host, &esp_flash_gpspi_host, sizeof(spi_flash_host_driver_t));
    }
#endif

    esp_err_t err = spi_flash_hal_init(data, cfg);
    if (err != ESP_OK) {
        return err;
    }

    host->driver_data = data;
    //some functions are not required if not SPI1
    if ((void*)data->spi != (void*)spi_flash_ll_get_hw(SPI_HOST)) {
        host->flush_cache = NULL;
    }
    return ESP_OK;
}

esp_err_t memspi_host_read_id_hs(spi_flash_host_driver_t *host, uint32_t *id)
{
    uint32_t id_buf = 0;
    spi_flash_trans_t t = {
        .command = CMD_RDID,
        .miso_len = 3,
        .miso_data = ((uint8_t*) &id_buf),
    };
    host->common_command(host, &t);

    uint32_t raw_flash_id = id_buf;
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
    uint32_t stat_buf = 0;
    spi_flash_trans_t t = {
        .command = CMD_RDSR,
        .miso_data = ((uint8_t*) &stat_buf),
        .miso_len = 1
    };
    esp_err_t err = driver->common_command(driver, &t);
    if (err != ESP_OK) {
        return err;
    }
    *out_sr = stat_buf;
    return ESP_OK;
}

esp_err_t memspi_host_flush_cache(spi_flash_host_driver_t* driver, uint32_t addr, uint32_t size)
{
    if ((void*)((memspi_host_data_t*)(driver->driver_data))->spi == (void*) spi_flash_ll_get_hw(SPI_HOST)) {
        spi_flash_check_and_flush_cache(addr, size);
    }
    return ESP_OK;
}

void memspi_host_erase_chip(spi_flash_host_driver_t *chip_drv)
{
    spi_flash_trans_t t = { 0 };
    t.command = CMD_CHIP_ERASE;
    chip_drv->common_command(chip_drv, &t);
}

void memspi_host_erase_sector(spi_flash_host_driver_t *chip_drv, uint32_t start_address)
{
    spi_flash_trans_t t = { 
        .command = CMD_SECTOR_ERASE,
        .address_bitlen = 24,
        .address = start_address
    };
    chip_drv->common_command(chip_drv, &t);
}

void memspi_host_erase_block(spi_flash_host_driver_t *chip_drv, uint32_t start_address)
{
    spi_flash_trans_t t = { 
        .command = CMD_LARGE_BLOCK_ERASE,
        .address_bitlen = 24,
        .address = start_address,
    };
    chip_drv->common_command(chip_drv, &t);
}

void memspi_host_program_page(spi_flash_host_driver_t *chip_drv, const void *buffer, uint32_t address, uint32_t length)
{
    spi_flash_trans_t t = { 
        .command = CMD_PROGRAM_PAGE,
        .address_bitlen = 24,
        .address = address,
        .mosi_len = length,
        .mosi_data = buffer
    };
    chip_drv->common_command(chip_drv, &t);
}

esp_err_t memspi_host_read(spi_flash_host_driver_t *chip_drv, void *buffer, uint32_t address, uint32_t read_len)
{
    spi_flash_trans_t t = {
        .address_bitlen = 24,
        .address = address,
        .miso_len = read_len,
        .miso_data = buffer
    };
    chip_drv->common_command(chip_drv, &t);
    return ESP_OK;
}

esp_err_t memspi_host_set_write_protect(spi_flash_host_driver_t *chip_drv, bool wp)
{
    spi_flash_trans_t t = {
        .command = wp ? CMD_WRDI : CMD_WREN
    };
    chip_drv->common_command(chip_drv, &t);
    return ESP_OK;
}
