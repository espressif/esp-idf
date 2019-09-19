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
#include <string.h>
#include "esp32/rom/crc.h"
#include "esp_partition.h"
#include "esp_core_dump_priv.h"
#include "esp_flash_internal.h"

#include "sdkconfig.h"

const static DRAM_ATTR char TAG[] __attribute__((unused)) = "esp_core_dump_flash";

#if CONFIG_ESP32_ENABLE_COREDUMP_TO_FLASH

typedef struct _core_dump_write_flash_data_t
{
    uint32_t        off; // current offset in partition
    core_dump_crc_t crc; // CRC of dumped data
} core_dump_write_flash_data_t;

typedef struct _core_dump_partition_t
{
    // core dump partition start
    uint32_t start;
    // core dump partition size
    uint32_t size;
} core_dump_partition_t;

typedef struct _core_dump_flash_config_t
{
    // core dump partition config
    core_dump_partition_t partition;
    // CRC of core dump partition config
    core_dump_crc_t       partition_config_crc;
} core_dump_flash_config_t;

// core dump flash data
static core_dump_flash_config_t s_core_flash_config;

static inline core_dump_crc_t esp_core_dump_calc_flash_config_crc(void)
{
    return crc32_le(0, (uint8_t const *)&s_core_flash_config.partition, sizeof(s_core_flash_config.partition));
}

void esp_core_dump_flash_init(void)
{
    const esp_partition_t *core_part;

    ESP_COREDUMP_LOGI("Init core dump to flash");
    core_part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_COREDUMP, NULL);
    if (!core_part) {
        ESP_COREDUMP_LOGE("No core dump partition found!");
        return;
    }
    ESP_COREDUMP_LOGI("Found partition '%s' @ %x %d bytes", core_part->label, core_part->address, core_part->size);
    s_core_flash_config.partition.start      = core_part->address;
    s_core_flash_config.partition.size       = core_part->size;
    s_core_flash_config.partition_config_crc = esp_core_dump_calc_flash_config_crc();
}

static uint32_t esp_core_dump_write_flash_padded(size_t off, uint8_t *data, uint32_t data_size)
{
    esp_err_t err;
    uint32_t data_len = 0, k, len;
    union
    {
        uint8_t    data8[4];
        uint32_t   data32;
    } rom_data;

    data_len = (data_size / sizeof(uint32_t)) * sizeof(uint32_t);

    assert(off >= s_core_flash_config.partition.start);
    assert((off + data_len + (data_size % sizeof(uint32_t) ? sizeof(uint32_t) : 0)) <=
        s_core_flash_config.partition.start + s_core_flash_config.partition.size);

#ifdef CONFIG_SPI_FLASH_USE_LEGACY_IMPL
    err = spi_flash_write(off, data, data_len);
#else
    err = esp_flash_write(esp_flash_default_chip, data, off, data_len);
#endif
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to write data to flash (%d)!", err);
        return 0;
    }

    len = data_size % sizeof(uint32_t);
    if (len) {
        // write last bytes with padding, actual TCB len can be retrieved by esptool from core dump header
        rom_data.data32 = 0;
        for (k = 0; k < len; k++) {
            rom_data.data8[k] = *(data + data_len + k);
        }
#ifdef CONFIG_SPI_FLASH_USE_LEGACY_IMPL
        err = spi_flash_write(off + data_len, &rom_data, sizeof(uint32_t));
#else
        err = esp_flash_write(esp_flash_default_chip, &rom_data, off + data_len, sizeof(uint32_t));
#endif
        if (err != ESP_OK) {
            ESP_COREDUMP_LOGE("Failed to finish write data to flash (%d)!", err);
            return 0;
        }
        data_len += sizeof(uint32_t);
    }

    return data_len;
}

static esp_err_t esp_core_dump_flash_write_prepare(void *priv, uint32_t *data_len)
{
    esp_err_t err;
    uint32_t sec_num;
    core_dump_write_flash_data_t *wr_data = (core_dump_write_flash_data_t *)priv;

    // check for available space in partition
    if ((*data_len + sizeof(uint32_t)) > s_core_flash_config.partition.size) {
        ESP_COREDUMP_LOGE("Not enough space to save core dump!");
        return ESP_ERR_NO_MEM;
    }
    // add space for CRC
    *data_len += sizeof(core_dump_crc_t);

    memset(wr_data, 0, sizeof(*wr_data));

    sec_num = *data_len / SPI_FLASH_SEC_SIZE;
    if (*data_len % SPI_FLASH_SEC_SIZE) {
        sec_num++;
    }
    assert(sec_num * SPI_FLASH_SEC_SIZE <= s_core_flash_config.partition.size);
#ifdef CONFIG_SPI_FLASH_USE_LEGACY_IMPL
    err = spi_flash_erase_range(s_core_flash_config.partition.start + 0, sec_num * SPI_FLASH_SEC_SIZE);
#else
    err = esp_flash_erase_region(esp_flash_default_chip, s_core_flash_config.partition.start + 0, sec_num * SPI_FLASH_SEC_SIZE);
#endif
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to erase flash (%d)!", err);
        return err;
    }
    return err;
}

static esp_err_t esp_core_dump_flash_write_word(core_dump_write_flash_data_t *wr_data, uint32_t word)
{
    esp_err_t err = ESP_OK;
    uint32_t  data32 = word;

    assert(wr_data->off + sizeof(uint32_t) <= s_core_flash_config.partition.size);
#ifdef CONFIG_SPI_FLASH_USE_LEGACY_IMPL
    err = spi_flash_write(s_core_flash_config.partition.start + wr_data->off, &data32, sizeof(uint32_t));
#else
    err = esp_flash_write(esp_flash_default_chip, &data32, s_core_flash_config.partition.start + wr_data->off, sizeof(uint32_t));
#endif
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to write to flash (%d)!", err);
        return err;
    }
    wr_data->off += sizeof(uint32_t);

    return err;
}

static esp_err_t esp_core_dump_flash_write_start(void *priv)
{
    return ESP_OK;
}

static esp_err_t esp_core_dump_flash_write_end(void *priv)
{
    core_dump_write_flash_data_t *wr_data = (core_dump_write_flash_data_t *)priv;
#if LOG_LOCAL_LEVEL >= ESP_LOG_DEBUG
    union
    {
        uint8_t    data8[16];
        uint32_t   data32[4];
    } rom_data;

#ifdef CONFIG_SPI_FLASH_USE_LEGACY_IMPL
    esp_err_t err = spi_flash_read(s_core_flash_config.partition.start + 0, &rom_data, sizeof(rom_data));
#else
    esp_err_t err = esp_flash_read(esp_flash_default_chip, &rom_data, s_core_flash_config.partition.start + 0, sizeof(rom_data));
#endif
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to read flash (%d)!", err);
        return err;
    } else {
        ESP_COREDUMP_LOG_PROCESS("Data from flash:");
        for (uint32_t i = 0; i < sizeof(rom_data)/sizeof(rom_data.data32[0]); i++) {
            ESP_COREDUMP_LOG_PROCESS("%x", rom_data.data32[i]);
        }
    }
#endif
    // write core dump CRC
    ESP_COREDUMP_LOG_PROCESS("Dump data CRC = 0x%x", wr_data->crc);
    return esp_core_dump_flash_write_word(wr_data, wr_data->crc);
}

static esp_err_t esp_core_dump_flash_write_data(void *priv, void * data, uint32_t data_len)
{
    esp_err_t err = ESP_OK;
    core_dump_write_flash_data_t *wr_data = (core_dump_write_flash_data_t *)priv;

    uint32_t len = esp_core_dump_write_flash_padded(s_core_flash_config.partition.start + wr_data->off, data, data_len);
    if (len != data_len) {
        return ESP_FAIL;
    }

    wr_data->off += len;
    wr_data->crc = crc32_le(wr_data->crc, data, data_len);

    return err;
}

void esp_core_dump_to_flash(XtExcFrame *frame)
{
    static core_dump_write_config_t wr_cfg;
    static core_dump_write_flash_data_t wr_data;

    core_dump_crc_t crc = esp_core_dump_calc_flash_config_crc();
    if (s_core_flash_config.partition_config_crc != crc) {
        ESP_COREDUMP_LOGE("Core dump flash config is corrupted! CRC=0x%x instead of 0x%x", crc, s_core_flash_config.partition_config_crc);
        return;
    }
    // check that partition can hold at least core dump data length
    if (s_core_flash_config.partition.start == 0 || s_core_flash_config.partition.size < sizeof(uint32_t)) {
        ESP_COREDUMP_LOGE("Invalid flash partition config!");
        return;
    }

    // init non-OS flash access critical section
    spi_flash_guard_set(&g_flash_guard_no_os_ops);
    esp_flash_app_disable_protect(true);

    memset(&wr_cfg, 0, sizeof(wr_cfg));
    wr_cfg.prepare = esp_core_dump_flash_write_prepare;
    wr_cfg.start = esp_core_dump_flash_write_start;
    wr_cfg.end = esp_core_dump_flash_write_end;
    wr_cfg.write = esp_core_dump_flash_write_data;
    wr_cfg.priv = &wr_data;

    ESP_COREDUMP_LOGI("Save core dump to flash...");
    esp_core_dump_write((void*)frame, &wr_cfg);
    ESP_COREDUMP_LOGI("Core dump has been saved to flash.");
}
#endif

