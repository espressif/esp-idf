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

#include "soc/soc_caps.h"
#include "spi_flash_defs.h"
#include "memspi_host_driver.h"
#include "string.h"
#include "esp_log.h"
#include "cache_utils.h"
#include "esp_flash_partitions.h"


#define SPI_FLASH_HAL_MAX_WRITE_BYTES 64
#define SPI_FLASH_HAL_MAX_READ_BYTES 64

DRAM_ATTR static const spi_flash_host_driver_t esp_flash_default_host = ESP_FLASH_DEFAULT_HOST_DRIVER();

#if SOC_MEMSPI_IS_INDEPENDENT
extern void spi_flash_hal_gpspi_poll_cmd_done(spi_flash_host_inst_t *host);
extern esp_err_t spi_flash_hal_gpspi_device_config(spi_flash_host_inst_t *host);
esp_err_t spi_flash_hal_gpspi_configure_host_io_mode(
    spi_flash_host_inst_t *host,
    uint32_t command,
    uint32_t addr_bitlen,
    int dummy_cyclelen_base,
    esp_flash_io_mode_t io_mode);
extern esp_err_t spi_flash_hal_gpspi_common_command(spi_flash_host_inst_t *host, spi_flash_trans_t *trans);
extern esp_err_t spi_flash_hal_gpspi_read(spi_flash_host_inst_t *host, void *buffer, uint32_t address, uint32_t read_len);
extern uint32_t spi_flash_hal_gpspi_check_status(spi_flash_host_inst_t *host);
extern bool spi_flash_hal_gpspi_supports_direct_write(spi_flash_host_inst_t *host, const void *p);
extern bool spi_flash_hal_gpspi_supports_direct_read(spi_flash_host_inst_t *host, const void *p);

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
        .write_data_slicer = memspi_host_write_data_slicer,
        .read = spi_flash_hal_gpspi_read,
        .read_data_slicer = memspi_host_read_data_slicer,
        .host_status = spi_flash_hal_gpspi_check_status,
        .configure_host_io_mode = spi_flash_hal_gpspi_configure_host_io_mode,
        .poll_cmd_done = spi_flash_hal_gpspi_poll_cmd_done,
        .flush_cache = NULL,
        .check_suspend = NULL,
        .resume = spi_flash_hal_resume,
        .suspend = spi_flash_hal_suspend,
};
#endif

esp_err_t memspi_host_init_pointers(memspi_host_inst_t *host, const memspi_host_config_t *cfg)
{
#if SOC_MEMSPI_IS_INDEPENDENT
    if (cfg->host_id == SPI1_HOST)
        host->inst.driver = &esp_flash_default_host;
    else {
        host->inst.driver = &esp_flash_gpspi_host;
    }
#else
    host->inst.driver = &esp_flash_default_host;
#endif

    esp_err_t err = spi_flash_hal_init(host, cfg);
    return err;
}

#ifndef CONFIG_SPI_FLASH_ROM_IMPL

static const char TAG[] = "memspi";

esp_err_t memspi_host_read_id_hs(spi_flash_host_inst_t *host, uint32_t *id)
{
    uint32_t id_buf = 0;
    spi_flash_trans_t t = {
        .command = CMD_RDID,
        .miso_len = 3,
        .miso_data = ((uint8_t*) &id_buf),
    };
    host->driver->common_command(host, &t);

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

esp_err_t memspi_host_read_status_hs(spi_flash_host_inst_t *host, uint8_t *out_sr)
{
    //NOTE: we do have a read id function, however it doesn't work in high freq
    uint32_t stat_buf = 0;
    spi_flash_trans_t t = {
        .command = CMD_RDSR,
        .miso_data = ((uint8_t*) &stat_buf),
        .miso_len = 1
    };
    esp_err_t err = host->driver->common_command(host, &t);
    if (err != ESP_OK) {
        return err;
    }
    *out_sr = stat_buf;
    return ESP_OK;
}

esp_err_t memspi_host_flush_cache(spi_flash_host_inst_t *host, uint32_t addr, uint32_t size)
{
    if ((void*)((memspi_host_inst_t*)host)->spi == (void*) spi_flash_ll_get_hw(SPI1_HOST)) {
        spi_flash_check_and_flush_cache(addr, size);
    }
    return ESP_OK;
}

void memspi_host_erase_chip(spi_flash_host_inst_t *host)
{
    spi_flash_trans_t t = { 0 };
    t.command = CMD_CHIP_ERASE;
    host->driver->common_command(host, &t);
}

// Only support 24bit address
void memspi_host_erase_sector(spi_flash_host_inst_t *host, uint32_t start_address)
{
    assert(start_address < 0x1000000);
    spi_flash_trans_t t = {
        .command = CMD_SECTOR_ERASE,
        .address_bitlen = 24,
        .address = start_address
    };
    host->driver->common_command(host, &t);
}

// Only support 24bit address
void memspi_host_erase_block(spi_flash_host_inst_t *host, uint32_t start_address)
{
    assert(start_address < 0x1000000);
    spi_flash_trans_t t = {
        .command = CMD_LARGE_BLOCK_ERASE,
        .address_bitlen = 24,
        .address = start_address,
    };
    host->driver->common_command(host, &t);
}

// Only support 24bit address
void memspi_host_program_page(spi_flash_host_inst_t *host, const void *buffer, uint32_t address, uint32_t length)
{
    assert(address + length <= 0x1000000);
    spi_flash_trans_t t = {
        .command = CMD_PROGRAM_PAGE,
        .address_bitlen = 24,
        .address = address,
        .mosi_len = length,
        .mosi_data = buffer
    };
    host->driver->common_command(host, &t);
}

esp_err_t memspi_host_read(spi_flash_host_inst_t *host, void *buffer, uint32_t address, uint32_t read_len)
{
    spi_flash_trans_t t = {
        .command = CMD_READ,
        .address_bitlen = 24,
        .address = address,
        .miso_len = read_len,
        .miso_data = buffer
    };
    host->driver->common_command(host, &t);
    return ESP_OK;
}

esp_err_t memspi_host_set_write_protect(spi_flash_host_inst_t *host, bool wp)
{
    spi_flash_trans_t t = {
        .command = wp ? CMD_WRDI : CMD_WREN
    };
    host->driver->common_command(host, &t);
    return ESP_OK;
}

// When encryption is enabled, etc. the data slicer may be complicated
// This is the simple case where the hardware has no other requirements than the size and page boundary
int memspi_host_write_data_slicer(spi_flash_host_inst_t *host, uint32_t address, uint32_t len, uint32_t *align_address, uint32_t page_size)
{
    uint32_t slicer_flag = ((spi_flash_hal_context_t*)host)->slicer_flags;
    uint32_t align_addr = address;

    if (slicer_flag & SPI_FLASH_HOST_CONTEXT_SLICER_FLAG_DTR) {
        if (((align_addr % 2) != 0) && ((len % 2) != 0)) {
            align_addr -= 1;
            len += 1;
        } else if (((align_addr % 2) != 0) && ((len % 2) == 0)) {
            align_addr -= 1;
            len += 2;
        } else if (((align_addr % 2) == 0) && ((len % 2) != 0)) {
            len += 1;
        }
    }

    uint32_t end_bound = (align_addr/page_size + 1) * page_size;
    // Shouldn't program cross the page, or longer than SPI_FLASH_HAL_MAX_WRITE_BYTES
    uint32_t max_len = MIN(end_bound - align_addr, SPI_FLASH_HAL_MAX_WRITE_BYTES);
    *align_address = align_addr;
    return MIN(max_len, len);
}

int memspi_host_read_data_slicer(spi_flash_host_inst_t *host, uint32_t address, uint32_t len, uint32_t *align_address, uint32_t page_size)
{
    // Shouldn't read longer than SPI_FLASH_HAL_MAX_READ_BYTES
    uint32_t slicer_flag = ((spi_flash_hal_context_t*)host)->slicer_flags;
    uint32_t align_addr = address;

    if (slicer_flag & SPI_FLASH_HOST_CONTEXT_SLICER_FLAG_DTR) {
        if (((align_addr % 2) != 0) && ((len % 2) != 0)) {
            align_addr -= 1;
            len += 1;
        } else if (((align_addr % 2) != 0) && ((len % 2) == 0)) {
            align_addr -= 1;
            len += 2;
        } else if (((align_addr % 2) == 0) && ((len % 2) != 0)) {
            len += 1;
        }
    }
    uint32_t max_len = SPI_FLASH_HAL_MAX_READ_BYTES;
    *align_address = align_addr;
    return MIN(max_len, len);
}

#endif // CONFIG_SPI_FLASH_ROM_IMPL
