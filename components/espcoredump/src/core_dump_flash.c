/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "esp_partition.h"
#include "esp_log.h"
#include "esp_core_dump_types.h"
#include "core_dump_checksum.h"
#include "esp_flash_internal.h"
#include "esp_flash_encrypt.h"
#include "esp_rom_crc.h"
#include "esp_private/spi_flash_os.h"

#define BLANK_COREDUMP_SIZE 0xFFFFFFFF

const static char TAG[] __attribute__((unused)) = "esp_core_dump_flash";

#if CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH

typedef struct _core_dump_partition_t {
    /* Core dump partition start. */
    uint32_t start;
    /* Core dump partition size. */
    uint32_t size;
    /* Flag set to true if the partition is encrypted. */
    bool encrypted;
#if CONFIG_ESP_COREDUMP_FLASH_NO_OVERWRITE
    /* Flag set to true if the partition is empty. */
    bool empty;
#endif
} core_dump_partition_t;

typedef struct _core_dump_flash_config_t {
    /* Core dump partition config. */
    core_dump_partition_t partition;
    /* CRC of core dump partition config. */
    core_dump_crc_t partition_config_crc;
} core_dump_flash_config_t;

/* Core dump flash data. */
static core_dump_flash_config_t s_core_flash_config;

void esp_core_dump_print_write_start(void) __attribute__((alias("esp_core_dump_flash_print_write_start")));
void esp_core_dump_print_write_end(void) __attribute__((alias("esp_core_dump_flash_print_write_end")));
esp_err_t esp_core_dump_write_init(void) __attribute__((alias("esp_core_dump_flash_hw_init")));
esp_err_t esp_core_dump_write_prepare(core_dump_write_data_t *wr_data, uint32_t *data_len) __attribute__((alias("esp_core_dump_flash_write_prepare")));
esp_err_t esp_core_dump_write_start(core_dump_write_data_t *wr_data) __attribute__((alias("esp_core_dump_flash_write_start")));
esp_err_t esp_core_dump_write_end(core_dump_write_data_t *wr_data) __attribute__((alias("esp_core_dump_flash_write_end")));
esp_err_t esp_core_dump_write_data(core_dump_write_data_t *wr_data, void *data, uint32_t data_len) __attribute__((alias("esp_core_dump_flash_write_data")));

#define ESP_COREDUMP_FLASH_WRITE(_off_, _data_, _len_)           esp_flash_write(esp_flash_default_chip, _data_, _off_, _len_)
#define ESP_COREDUMP_FLASH_WRITE_ENCRYPTED(_off_, _data_, _len_) esp_flash_write_encrypted(esp_flash_default_chip, _off_, _data_, _len_)
#define ESP_COREDUMP_FLASH_ERASE(_off_, _len_)                   esp_flash_erase_region(esp_flash_default_chip, _off_, _len_)

esp_err_t esp_core_dump_image_check(void);
static esp_err_t esp_core_dump_partition_and_size_get(const esp_partition_t **partition, uint32_t* size);

static void esp_core_dump_flash_print_write_start(void)
{
    ESP_COREDUMP_LOGI("Save core dump to flash...");
}

static void esp_core_dump_flash_print_write_end(void)
{
    ESP_COREDUMP_LOGI("Core dump has been saved to flash.");
}

static esp_err_t esp_core_dump_flash_custom_write(uint32_t address, const void *buffer, uint32_t length)
{
    esp_err_t err = ESP_OK;

    if (esp_flash_encryption_enabled() && s_core_flash_config.partition.encrypted) {
        err = ESP_COREDUMP_FLASH_WRITE_ENCRYPTED(address, buffer, length);
    } else {
        err = ESP_COREDUMP_FLASH_WRITE(address, buffer, length);
    }

    return err;
}

static inline core_dump_crc_t esp_core_dump_calc_flash_config_crc(void)
{
    return esp_rom_crc32_le(0, (uint8_t const *)&s_core_flash_config.partition, sizeof(s_core_flash_config.partition));
}

static esp_err_t esp_core_dump_flash_hw_init(void)
{
    /* Check core dump partition configuration. */
    core_dump_crc_t crc = esp_core_dump_calc_flash_config_crc();
    if (s_core_flash_config.partition_config_crc != crc) {
        ESP_COREDUMP_LOGE("Core dump flash config is corrupted! CRC=0x%x instead of 0x%x", crc, s_core_flash_config.partition_config_crc);
        return ESP_FAIL;
    }

    /* Make sure that the partition can at least hold the data length. */
    if (s_core_flash_config.partition.start == 0 || s_core_flash_config.partition.size < sizeof(uint32_t)) {
        ESP_COREDUMP_LOGE("Invalid flash partition config!");
        return ESP_FAIL;
    }

#if CONFIG_ESP_COREDUMP_FLASH_NO_OVERWRITE
    if (!s_core_flash_config.partition.empty) {
        ESP_COREDUMP_LOGW("Core dump already exists in flash, will not overwrite it with a new core dump");
        return ESP_FAIL;
    }
#endif

    /* Initialize non-OS flash access critical section. */
    spi_flash_guard_set(&g_flash_guard_no_os_ops);
    esp_flash_app_disable_protect(true);
    return ESP_OK;
}

static void esp_core_dump_partition_init(void)
{
    const esp_partition_t *core_part = NULL;

    /* Look for the core dump partition on the flash. */
    ESP_COREDUMP_LOGI("Init core dump to flash");
    core_part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_COREDUMP, NULL);
    if (!core_part) {
        ESP_COREDUMP_LOGE("No core dump partition found!");
        return;
    }
    ESP_COREDUMP_LOGI("Found partition '%s' @ %x %d bytes", core_part->label, core_part->address, core_part->size);
    s_core_flash_config.partition.start      = core_part->address;
    s_core_flash_config.partition.size       = core_part->size;
    s_core_flash_config.partition.encrypted  = core_part->encrypted;

#if CONFIG_ESP_COREDUMP_FLASH_NO_OVERWRITE
    uint32_t core_size = 0;
    esp_err_t err = esp_partition_read(core_part, 0, &core_size, sizeof(core_size));
    if (err == ESP_OK) {
        s_core_flash_config.partition.empty = (core_size == BLANK_COREDUMP_SIZE);
    } else {
        ESP_COREDUMP_LOGE("Failed to read core dump data size (%d)!", err);
        s_core_flash_config.partition.empty = false;
    }
#endif

    s_core_flash_config.partition_config_crc = esp_core_dump_calc_flash_config_crc();

    if (esp_flash_encryption_enabled() && !core_part->encrypted) {
        ESP_COREDUMP_LOGW("core dump partition is plain text, consider enabling `encrypted` flag");
    }
}

static esp_err_t esp_core_dump_flash_write_data(core_dump_write_data_t* wr_data, uint8_t* data, uint32_t data_size)
{
    esp_err_t err = ESP_OK;
    uint32_t written = 0;
    uint32_t wr_sz = 0;

    /* Make sure that the partition is large enough to hold the data. */
    ESP_COREDUMP_ASSERT((wr_data->off + data_size) < s_core_flash_config.partition.size);

    if (wr_data->cached_bytes) {
        /* Some bytes are in the cache, let's continue filling the cache
         * with the data received as parameter. Let's calculate the maximum
         * amount of bytes we can still fill the cache with. */
        if ((COREDUMP_CACHE_SIZE - wr_data->cached_bytes) > data_size) {
            wr_sz = data_size;
        } else {
            wr_sz = COREDUMP_CACHE_SIZE - wr_data->cached_bytes;
        }

        /* Append wr_sz bytes from data parameter to the cache. */
        memcpy(&wr_data->cached_data[wr_data->cached_bytes], data, wr_sz);
        wr_data->cached_bytes += wr_sz;

        if (wr_data->cached_bytes == COREDUMP_CACHE_SIZE) {
            /* The cache is full, we can flush it to the flash. */
            err = esp_core_dump_flash_custom_write(s_core_flash_config.partition.start + wr_data->off,
                                                   wr_data->cached_data,
                                                   COREDUMP_CACHE_SIZE);
            if (err != ESP_OK) {
                ESP_COREDUMP_LOGE("Failed to write cached data to flash (%d)!", err);
                return err;
            }
            /* The offset of the next data that will be written onto the flash
             * can now be increased. */
            wr_data->off += COREDUMP_CACHE_SIZE;

            /* Update checksum with the newly written data on the flash. */
            esp_core_dump_checksum_update(&wr_data->checksum_ctx, &wr_data->cached_data, COREDUMP_CACHE_SIZE);

            /* Reset cache from the next use. */
            wr_data->cached_bytes = 0;
            memset(wr_data->cached_data, 0, COREDUMP_CACHE_SIZE);
        }

        written += wr_sz;
        data_size -= wr_sz;
    }

    /* Figure out how many bytes we can write onto the flash directly, without
     * using the cache. In our case the cache size is a multiple of the flash's
     * minimum writing block size, so we will use it for our calculation.
     * For example, if COREDUMP_CACHE_SIZE equals 32, here are interesting
     * values:
     * +---------+-----------------------+
     * |         |       data_size       |
     * +---------+---+----+----+----+----+
     * |         | 0 | 31 | 32 | 40 | 64 |
     * +---------+---+----+----+----+----+
     * | (blocks | 0 | 0  | 1  | 1  | 2) |
     * +---------+---+----+----+----+----+
     * | wr_sz   | 0 | 0  | 32 | 32 | 64 |
     * +---------+---+----+----+----+----+
     */
    wr_sz = (data_size / COREDUMP_CACHE_SIZE) * COREDUMP_CACHE_SIZE;
    if (wr_sz) {
        /* Write the contiguous amount of bytes to the flash,
         * without using the cache */
        err = esp_core_dump_flash_custom_write(s_core_flash_config.partition.start + wr_data->off, data + written, wr_sz);

        if (err != ESP_OK) {
            ESP_COREDUMP_LOGE("Failed to write data to flash (%d)!", err);
            return err;
        }

        /* Update the checksum with the newly written bytes */
        esp_core_dump_checksum_update(&wr_data->checksum_ctx, data + written, wr_sz);
        wr_data->off += wr_sz;
        written += wr_sz;
        data_size -= wr_sz;
    }

    if (data_size > 0) {
        /* There still some bytes from the data parameter that need to be sent,
         * append it to cache in order to write them later. (i.e. when there
         * will be enough bytes to fill the cache) */
        memcpy(&wr_data->cached_data, data + written, data_size);
        wr_data->cached_bytes = data_size;
    }

    return ESP_OK;
}

static esp_err_t esp_core_dump_flash_write_prepare(core_dump_write_data_t *wr_data, uint32_t *data_len)
{
    esp_err_t err = ESP_OK;
    uint32_t sec_num = 0;
    uint32_t cs_len = 0;

    /* Get the length, in bytes, of the checksum. */
    cs_len = esp_core_dump_checksum_size();

    /* At the end of the core dump file, a padding may be added, according to the
     * cache size. We must take that padding into account. */
    uint32_t padding = 0;
    const uint32_t modulo = *data_len % COREDUMP_CACHE_SIZE;
    if (modulo != 0) {
        /* The data length is not a multiple of the cache size,
         * so there will be a padding. */
        padding = COREDUMP_CACHE_SIZE - modulo;
    }

    /* Now we can check whether we have enough space in our core dump parition
     * or not. */
    if ((*data_len + padding + cs_len) > s_core_flash_config.partition.size) {
        ESP_COREDUMP_LOGE("Not enough space to save core dump!");
        return ESP_ERR_NO_MEM;
    }

    /* We have enough space in the partition, add the padding and the checksum
     * in the core dump file calculation. */
    *data_len += padding + cs_len;

    memset(wr_data, 0, sizeof(core_dump_write_data_t));

    /* In order to erase the right amount of data in the flash, we have to
     * calculate how many SPI flash sectors will be needed by the core dump
     * file. */
    sec_num = *data_len / SPI_FLASH_SEC_SIZE;
    if (*data_len % SPI_FLASH_SEC_SIZE) {
        sec_num++;
    }

    /* Erase the amount of sectors needed. */
    ESP_COREDUMP_LOGI("Erase flash %d bytes @ 0x%x", sec_num * SPI_FLASH_SEC_SIZE, s_core_flash_config.partition.start + 0);
    ESP_COREDUMP_ASSERT(sec_num * SPI_FLASH_SEC_SIZE <= s_core_flash_config.partition.size);
    err = ESP_COREDUMP_FLASH_ERASE(s_core_flash_config.partition.start + 0, sec_num * SPI_FLASH_SEC_SIZE);
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to erase flash (%d)!", err);
    }

    return err;
}

static esp_err_t esp_core_dump_flash_write_start(core_dump_write_data_t *wr_data)
{
    esp_core_dump_checksum_init(&wr_data->checksum_ctx);
    return ESP_OK;
}

static esp_err_t esp_core_dump_flash_write_end(core_dump_write_data_t *wr_data)
{
    esp_err_t err = ESP_OK;
    core_dump_checksum_bytes checksum = NULL;
    uint32_t cs_len = 0;

    /* Get the size, in bytes of the checksum. */
    cs_len  = esp_core_dump_checksum_size();

    /* Flush cached bytes, including the zero padding at the end (if any). */
    if (wr_data->cached_bytes) {
        err = esp_core_dump_flash_custom_write(s_core_flash_config.partition.start + wr_data->off,
                                               wr_data->cached_data,
                                               COREDUMP_CACHE_SIZE);

        if (err != ESP_OK) {
            ESP_COREDUMP_LOGE("Failed to flush cached data to flash (%d)!", err);
            return err;
        }

        /* Update the checksum with the data written, including the padding. */
        esp_core_dump_checksum_update(&wr_data->checksum_ctx, wr_data->cached_data, COREDUMP_CACHE_SIZE);
        wr_data->off += COREDUMP_CACHE_SIZE;
        wr_data->cached_bytes = 0;
    }

    /* All data have been written to the flash, the cache is now empty, we can
     * terminate the checksum calculation. */
    esp_core_dump_checksum_finish(&wr_data->checksum_ctx, &checksum);

    /* Use the cache to write the checksum if its size doesn't match the requirements.
     * (e.g. its size is not a multiple of 32) */
    if (cs_len < COREDUMP_CACHE_SIZE) {
        /* Copy the checksum into the cache. */
        memcpy(wr_data->cached_data, checksum, cs_len);

        /* Fill the rest of the cache with zeros. */
        memset(wr_data->cached_data + cs_len, 0, COREDUMP_CACHE_SIZE - cs_len);

        /* Finally, write the checksum on the flash, using the cache. */
        err = esp_core_dump_flash_custom_write(s_core_flash_config.partition.start + wr_data->off,
                                               wr_data->cached_data,
                                               COREDUMP_CACHE_SIZE);
    } else {
        /* In that case, the length of the checksum must be a multiple of 16. */
        ESP_COREDUMP_ASSERT(cs_len % 16 == 0);
        err = esp_core_dump_flash_custom_write(s_core_flash_config.partition.start + wr_data->off, checksum, cs_len);
    }

    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to flush cached data to flash (%d)!", err);
        return err;
    }
    wr_data->off += cs_len;
    ESP_COREDUMP_LOGI("Write end offset 0x%x, check sum length %d", wr_data->off, cs_len);

    return err;
}

void esp_core_dump_init(void)
{
    esp_core_dump_partition_init();

#if CONFIG_ESP_COREDUMP_CHECK_BOOT
    const esp_partition_t *partition = 0;
    uint32_t size = 0;

    if (esp_core_dump_image_check() == ESP_OK
            && esp_core_dump_partition_and_size_get(&partition, &size) == ESP_OK) {
        ESP_COREDUMP_LOGI("Found core dump %d bytes in flash @ 0x%x", size, partition->address);
    }
#endif
}

esp_err_t esp_core_dump_image_check(void)
{
    esp_err_t err = ESP_OK;
    const esp_partition_t *core_part = NULL;
    core_dump_write_data_t wr_data = { 0 };
    uint32_t size = 0;
    uint32_t total_size = 0;
    uint32_t offset = 0;
    const uint32_t checksum_size = esp_core_dump_checksum_size();
    core_dump_checksum_bytes checksum_calc = NULL;
    /* Initialize the checksum we have to read from the flash to the biggest
     * size we can have for a checksum. */
    uint8_t checksum_read[COREDUMP_CHECKSUM_MAX_LEN] = { 0 };

    /* Assert that we won't have any problems with our checksum size. */
    ESP_COREDUMP_DEBUG_ASSERT(checksum_size <= COREDUMP_CHECKSUM_MAX_LEN);

    /* Retrieve the partition and size. */
    err = esp_core_dump_partition_and_size_get(&core_part, &total_size);
    if (err != ESP_OK) {
        return err;
    }

    /* The final checksum, from the image, doesn't take part into the checksum
     * calculation, so subtract it from the bytes we are going to read. */
    size = total_size - checksum_size ;

    /* Initiate the checksum calculation for the coredump in the flash. */
    esp_core_dump_checksum_init(&wr_data.checksum_ctx);

    while (size > 0) {
        /* Use the cache in core_dump_write_data_t structure to read the
         * partition. */
        const uint32_t toread = (size < COREDUMP_CACHE_SIZE) ? size : COREDUMP_CACHE_SIZE;

        /* Read the content of the flash. */
        err = esp_partition_read(core_part, offset, wr_data.cached_data, toread);
        if (err != ESP_OK) {
            ESP_COREDUMP_LOGE("Failed to read data from core dump (%d)!", err);
            return err;
        }

        /* Update the checksum according to what was just read. */
        esp_core_dump_checksum_update(&wr_data.checksum_ctx, wr_data.cached_data, toread);

        /* Move the offset forward and decrease the remaining size. */
        offset += toread;
        size -= toread;
    }

    /* The coredump has been totally read, finish the checksum calculation. */
    esp_core_dump_checksum_finish(&wr_data.checksum_ctx, &checksum_calc);

    /* Read the checksum from the flash and compare to the one just
     * calculated. */
    err = esp_partition_read(core_part, total_size - checksum_size, checksum_read, checksum_size);
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to read checksum from core dump (%d)!", err);
        return err;
    }

    /* Compare the checksum read from the flash and the one just calculated. */
    if (memcmp(checksum_calc, checksum_read, checksum_size) != 0) {
        ESP_COREDUMP_LOGE("Core dump data check failed:");
        esp_core_dump_print_checksum("Calculated checksum", checksum_calc);
        esp_core_dump_print_checksum("Image checksum", checksum_read);
        return ESP_ERR_INVALID_CRC;
    } else {
        ESP_COREDUMP_LOGI("Core dump data checksum is correct");
    }

    return ESP_OK;
}

#endif

esp_err_t esp_core_dump_image_erase(void)
{
    /* If flash is encrypted, we can only write blocks of 16 bytes, let's always
     * write a 16-byte buffer. */
    uint32_t helper[4] = { BLANK_COREDUMP_SIZE };
    _Static_assert(sizeof(helper) % 16 == 0, "esp_partition_write() needs multiple of 16 bytes long buffers");

    /* Find the partition that could potentially contain a (previous) core dump. */
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

    esp_err_t err = ESP_OK;
    err = esp_partition_erase_range(core_part, 0, core_part->size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to erase core dump partition (%d)!", err);
        return err;
    }

    err = esp_partition_write(core_part, 0, helper, sizeof(helper));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write core dump partition size (%d)!", err);
    }

#if CONFIG_ESP_COREDUMP_FLASH_NO_OVERWRITE
    if (!s_core_flash_config.partition.empty) {
        s_core_flash_config.partition.empty = true;
        s_core_flash_config.partition_config_crc = esp_core_dump_calc_flash_config_crc();
    }
#endif

    return err;
}

static esp_err_t esp_core_dump_partition_and_size_get(const esp_partition_t **partition, uint32_t* size)
{
    uint32_t core_size = 0;
    const esp_partition_t *core_part = NULL;

    /* Check the arguments, at least one should be provided. */
    if (partition == NULL && size == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    /* Find the partition that could potentially contain a (previous) core dump. */
    core_part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
                                         ESP_PARTITION_SUBTYPE_DATA_COREDUMP,
                                         NULL);
    if (core_part == NULL) {
        ESP_COREDUMP_LOGE("No core dump partition found!");
        return ESP_ERR_NOT_FOUND;
    }
    if (core_part->size < sizeof(uint32_t)) {
        ESP_COREDUMP_LOGE("Too small core dump partition!");
        return ESP_ERR_INVALID_SIZE;
    }

    /* The partition has been found, get its first uint32_t value, which
     * describes the core dump file size. */
    esp_err_t err = esp_partition_read(core_part, 0, &core_size, sizeof(uint32_t));
    if (err != ESP_OK) {
        ESP_COREDUMP_LOGE("Failed to read core dump data size (%d)!", err);
        return err;
    }

    /* Verify that the size read from the flash is not corrupted. */
    if (core_size == 0xFFFFFFFF) {
        ESP_COREDUMP_LOGD("Blank core dump partition!");
        return ESP_ERR_INVALID_SIZE;
    }

    if ((core_size < sizeof(uint32_t)) || (core_size > core_part->size)) {
        ESP_COREDUMP_LOGE("Incorrect size of core dump image: %d", core_size);
        return ESP_ERR_INVALID_SIZE;
    }

    /* Return the values if needed. */
    if (partition != NULL) {
        *partition = core_part;
    }

    if (size != NULL) {
        *size = core_size;
    }

    return ESP_OK;
}

esp_err_t esp_core_dump_image_get(size_t* out_addr, size_t *out_size)
{
    esp_err_t err = ESP_OK;
    uint32_t size = 0;
    const esp_partition_t *core_part = NULL;

    /* Check the validity of the parameters. */
    if (out_addr == NULL || out_size == NULL) {
        return  ESP_ERR_INVALID_ARG;
    }

    /* Retrieve the partition and size. */
    err = esp_core_dump_partition_and_size_get(&core_part, &size);

    if (err != ESP_OK) {
        return err;
    }

    /* Save the address. */
    *out_addr = core_part->address;

    /* Save the size read. */
    *out_size = size;

    return ESP_OK;
}
