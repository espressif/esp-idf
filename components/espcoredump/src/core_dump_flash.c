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
#include "esp_partition.h"
#include "esp_log.h"
#include "esp_core_dump_priv.h"
#include "esp_flash_internal.h"
#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/crc.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/crc.h"
#endif

const static DRAM_ATTR char TAG[] __attribute__((unused)) = "esp_core_dump_flash";

#if CONFIG_ESP32_ENABLE_COREDUMP_TO_FLASH

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

#ifdef CONFIG_SPI_FLASH_USE_LEGACY_IMPL
#define ESP_COREDUMP_FLASH_WRITE(_off_, _data_, _len_)  spi_flash_write(_off_, _data_, _len_)
#define ESP_COREDUMP_FLASH_READ(_off_, _data_, _len_)   spi_flash_read(_off_, _data_, _len_)
#define ESP_COREDUMP_FLASH_ERASE(_off_, _len_)          spi_flash_erase_range(_off_, _len_)
#else
#define ESP_COREDUMP_FLASH_WRITE(_off_, _data_, _len_)  esp_flash_write(esp_flash_default_chip, _data_, _off_, _len_)
#define ESP_COREDUMP_FLASH_READ(_off_, _data_, _len_)   esp_flash_read(esp_flash_default_chip, _data_, _off_, _len_)
#define ESP_COREDUMP_FLASH_ERASE(_off_, _len_)          esp_flash_erase_region(esp_flash_default_chip, _off_, _len_)
#endif

esp_err_t esp_core_dump_image_get(size_t* out_addr, size_t *out_size);

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

static esp_err_t esp_core_dump_flash_write_data(void *priv, uint8_t *data, uint32_t data_size)
{
    esp_err_t err;
    core_dump_write_data_t *wr_data = (core_dump_write_data_t *)priv;
    uint32_t written = 0, wr_sz;

    assert((wr_data->off + data_size) < s_core_flash_config.partition.size);

    if (wr_data->cached_bytes) {
        if ((sizeof(wr_data->cached_data)-wr_data->cached_bytes) > data_size)
            wr_sz = data_size;
        else
            wr_sz = sizeof(wr_data->cached_data)-wr_data->cached_bytes;
        // append to data cache
        memcpy(&wr_data->cached_data.data8[wr_data->cached_bytes], data, wr_sz);
        wr_data->cached_bytes += wr_sz;
        if (wr_data->cached_bytes == sizeof(wr_data->cached_data)) {
            err = ESP_COREDUMP_FLASH_WRITE(s_core_flash_config.partition.start + wr_data->off, &wr_data->cached_data, sizeof(wr_data->cached_data));
            if (err != ESP_OK) {
                ESP_COREDUMP_LOGE("Failed to write cached data to flash (%d)!", err);
                return err;
            }
            // update checksum according to padding
            esp_core_dump_checksum_update(wr_data, &wr_data->cached_data, sizeof(wr_data->cached_data));
            // reset data cache
            wr_data->cached_bytes = 0;
            memset(&wr_data->cached_data, 0, sizeof(wr_data->cached_data));
        }
        wr_data->off += sizeof(wr_data->cached_data);
        written += wr_sz;
        data_size -= wr_sz;
    }

    wr_sz = (data_size / sizeof(wr_data->cached_data)) * sizeof(wr_data->cached_data);
    if (wr_sz) {
        err = ESP_COREDUMP_FLASH_WRITE(s_core_flash_config.partition.start + wr_data->off, data + written, wr_sz);
        if (err != ESP_OK) {
            ESP_COREDUMP_LOGE("Failed to write data to flash (%d)!", err);
            return err;
        }
        // update checksum of data written
        esp_core_dump_checksum_update(wr_data, data + written, wr_sz);
        wr_data->off += wr_sz;
        written += wr_sz;
        data_size -= wr_sz;
    }

    if (data_size > 0) {
        // append to data cache
        memcpy(&wr_data->cached_data, data + written, data_size);
        wr_data->cached_bytes = data_size;
    }

    return ESP_OK;
}

static esp_err_t esp_core_dump_flash_write_prepare(void *priv, uint32_t *data_len)
{
    esp_err_t err;
    uint32_t sec_num;
    core_dump_write_data_t *wr_data = (core_dump_write_data_t *)priv;
    uint32_t cs_len;
    cs_len = esp_core_dump_checksum_finish(wr_data, NULL);

    // check for available space in partition
    if ((*data_len + cs_len) > s_core_flash_config.partition.size) {
        ESP_COREDUMP_LOGE("Not enough space to save core dump!");
        return ESP_ERR_NO_MEM;
    }
    // add space for checksum
    *data_len += cs_len;

    memset(wr_data, 0, sizeof(core_dump_write_data_t));

    sec_num = *data_len / SPI_FLASH_SEC_SIZE;
    if (*data_len % SPI_FLASH_SEC_SIZE) {
        sec_num++;
    }
    ESP_COREDUMP_LOGI("Erase flash %d bytes @ 0x%x", sec_num * SPI_FLASH_SEC_SIZE, s_core_flash_config.partition.start + 0);
    assert(sec_num * SPI_FLASH_SEC_SIZE <= s_core_flash_config.partition.size);
    err = ESP_COREDUMP_FLASH_ERASE(s_core_flash_config.partition.start + 0, sec_num * SPI_FLASH_SEC_SIZE);
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to erase flash (%d)!", err);
        return err;
    }
    return err;
}

static esp_err_t esp_core_dump_flash_write_start(void *priv)
{
    core_dump_write_data_t *wr_data = (core_dump_write_data_t *)priv;
    esp_core_dump_checksum_init(wr_data);
    return ESP_OK;
}

static esp_err_t esp_core_dump_flash_write_end(void *priv)
{
    esp_err_t err;
    core_dump_write_data_t *wr_data = (core_dump_write_data_t *)priv;
    void* checksum;
    uint32_t cs_len = esp_core_dump_checksum_finish(wr_data, &checksum);

    // flush cached bytes with zero padding
    if (wr_data->cached_bytes) {
        err = ESP_COREDUMP_FLASH_WRITE(s_core_flash_config.partition.start + wr_data->off, &wr_data->cached_data, sizeof(wr_data->cached_data));
        if (err != ESP_OK) {
            ESP_COREDUMP_LOGE("Failed to flush cached data to flash (%d)!", err);
            return err;
        }
        // update checksum according to padding
        esp_core_dump_checksum_update(wr_data, &wr_data->cached_data, sizeof(wr_data->cached_data));
        wr_data->off += sizeof(wr_data->cached_data);
    }
    err = ESP_COREDUMP_FLASH_WRITE(s_core_flash_config.partition.start + wr_data->off, checksum, cs_len);
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to flush cached data to flash (%d)!", err);
        return err;
    }
    wr_data->off += cs_len;
    ESP_COREDUMP_LOGI("Write end offset 0x%x, check sum length %d", wr_data->off, cs_len);
#if LOG_LOCAL_LEVEL >= ESP_LOG_DEBUG
    union
    {
        uint8_t    data8[sizeof(core_dump_header_t)];
        uint32_t   data32[sizeof(core_dump_header_t)/sizeof(uint32_t)];
    } rom_data;
    err = ESP_COREDUMP_FLASH_READ(s_core_flash_config.partition.start + 0, &rom_data, sizeof(rom_data));
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to read back coredump header (%d)!", err);
        return err;
    } else {
        ESP_COREDUMP_LOG_PROCESS("Core dump header words from flash:");
        for (uint32_t i = 0; i < sizeof(rom_data)/sizeof(uint32_t); i++) {
            ESP_COREDUMP_LOG_PROCESS("0x%x", rom_data.data32[i]);
        }
    }
    uint32_t crc;
    err = ESP_COREDUMP_FLASH_READ(s_core_flash_config.partition.start + wr_data->off - cs_len, &crc, sizeof(crc));
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to read back checksum word (%d)!", err);
        return err;
    } else {
        ESP_COREDUMP_LOG_PROCESS("Checksum word from flash: 0x%x @ 0x%x", crc, wr_data->off - cs_len);
    }
#endif
    return err;
}

void esp_core_dump_to_flash(panic_info_t *info)
{
    static core_dump_write_config_t wr_cfg;
    static core_dump_write_data_t wr_data;

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
    wr_cfg.write = (esp_core_dump_flash_write_data_t)esp_core_dump_flash_write_data;
    wr_cfg.priv = &wr_data;

    ESP_COREDUMP_LOGI("Save core dump to flash...");
    esp_core_dump_write(info, &wr_cfg);
    ESP_COREDUMP_LOGI("Core dump has been saved to flash.");
}

void esp_core_dump_init(void)
{
    size_t core_data_sz = 0;
    size_t core_data_addr = 0;
    esp_core_dump_flash_init();
    if (esp_core_dump_image_get(&core_data_addr, &core_data_sz) == ESP_OK && core_data_sz > 0) {
        ESP_COREDUMP_LOGI("Found core dump %d bytes in flash @ 0x%x", core_data_sz, core_data_addr);
    }
}
#endif

esp_err_t esp_core_dump_image_get(size_t* out_addr, size_t *out_size)
{
    esp_err_t err;
    const void *core_data;
    spi_flash_mmap_handle_t core_data_handle;

    if (out_addr == NULL || out_size == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    const esp_partition_t *core_part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
                                                                ESP_PARTITION_SUBTYPE_DATA_COREDUMP,
                                                                NULL);
    if (!core_part) {
        ESP_LOGE(TAG, "No core dump partition found!");
        return ESP_ERR_NOT_FOUND;
    }
    if (core_part->size < sizeof(uint32_t)) {
        ESP_LOGE(TAG, "Too small core dump partition!");
        return ESP_ERR_INVALID_SIZE;
    }

    err = esp_partition_mmap(core_part, 0,  sizeof(uint32_t),
                             SPI_FLASH_MMAP_DATA, &core_data, &core_data_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mmap core dump data (%d)!", err);
        return err;
    }

    uint32_t *dw = (uint32_t *)core_data;
    *out_size = *dw;
    spi_flash_munmap(core_data_handle);
    if (*out_size == 0xFFFFFFFF) {
        ESP_LOGD(TAG, "Blank core dump partition!");
        return ESP_ERR_INVALID_SIZE;
    } else if ((*out_size < sizeof(uint32_t)) || (*out_size > core_part->size)) {
        ESP_LOGE(TAG, "Incorrect size of core dump image: %d", *out_size);
        return ESP_ERR_INVALID_SIZE;
    }

    // remap full core dump with CRC
    err = esp_partition_mmap(core_part, 0, *out_size,
                             SPI_FLASH_MMAP_DATA, &core_data, &core_data_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mmap core dump data (%d)!", err);
        return err;
    }
    // TODO: check CRC or SHA basing on the version of coredump image stored in flash
#if CONFIG_ESP32_COREDUMP_CHECKSUM_CRC32
    uint32_t *crc = (uint32_t *)(((uint8_t *)core_data) + *out_size);
    crc--; // Point to CRC field

    // Calculate CRC over core dump data except for CRC field
    core_dump_crc_t cur_crc = crc32_le(0, (uint8_t const *)core_data, *out_size - sizeof(core_dump_crc_t));
    if (*crc != cur_crc) {
        ESP_LOGD(TAG, "Core dump CRC offset 0x%x, data size: %u",
                (uint32_t)((uint32_t)crc - (uint32_t)core_data), *out_size);
        ESP_LOGE(TAG, "Core dump data CRC check failed: 0x%x -> 0x%x!", *crc, cur_crc);
        spi_flash_munmap(core_data_handle);
        return ESP_ERR_INVALID_CRC;
    }
#elif CONFIG_ESP32_COREDUMP_CHECKSUM_SHA256
    uint8_t* sha256_ptr = (uint8_t*)(((uint8_t *)core_data) + *out_size);
    sha256_ptr -= COREDUMP_SHA256_LEN;
    ESP_LOGD(TAG, "Core dump data offset, size: %d, %u!",
                    (uint32_t)((uint32_t)sha256_ptr - (uint32_t)core_data), *out_size);
    unsigned char sha_output[COREDUMP_SHA256_LEN];
    mbedtls_sha256_context ctx;
    ESP_LOGI(TAG, "Calculate SHA256 for coredump:");
    (void)esp_core_dump_sha(&ctx, core_data, *out_size - COREDUMP_SHA256_LEN, sha_output);
    if (memcmp((uint8_t*)sha256_ptr, (uint8_t*)sha_output, COREDUMP_SHA256_LEN) != 0) {
        ESP_LOGE(TAG, "Core dump data SHA256 check failed:");
        esp_core_dump_print_sha256("Calculated SHA256", (uint8_t*)sha_output);
        esp_core_dump_print_sha256("Image SHA256",(uint8_t*)sha256_ptr);
        spi_flash_munmap(core_data_handle);
        return ESP_ERR_INVALID_CRC;
    } else {
        ESP_LOGI(TAG, "Core dump data SHA256 is correct");
    }
#endif
    spi_flash_munmap(core_data_handle);

    *out_addr = core_part->address;
    return ESP_OK;
}
