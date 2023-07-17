/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/param.h>
#include <string.h>

#include "esp_memory_utils.h"
#include "spi_flash_chip_driver.h"
#include "memspi_host_driver.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp_flash_internal.h"
#include "spi_flash_defs.h"
#include "spi_flash_mmap.h"
#include "esp_rom_caps.h"
#include "esp_rom_spiflash.h"
#include "esp_private/esp_clk.h"
#include "esp_spi_flash_counters.h"

#if CONFIG_IDF_TARGET_ESP32S2
#include "esp_crypto_lock.h" // for locking flash encryption peripheral
#endif //CONFIG_IDF_TARGET_ESP32S2

DRAM_ATTR static const char TAG[] = "spi_flash";

#ifdef CONFIG_SPI_FLASH_WRITE_CHUNK_SIZE
#define MAX_WRITE_CHUNK CONFIG_SPI_FLASH_WRITE_CHUNK_SIZE /* write in chunks */
#else
#define MAX_WRITE_CHUNK 8192 /* write in chunks */
#endif // CONFIG_SPI_FLASH_WRITE_CHUNK_SIZE

#define MAX_READ_CHUNK 16384
#define VERIFY_BUF_LEN 64


#ifdef CONFIG_SPI_FLASH_DANGEROUS_WRITE_ABORTS
#define UNSAFE_WRITE_ADDRESS abort()
#else
#define UNSAFE_WRITE_ADDRESS return ESP_ERR_INVALID_ARG
#endif

/* CHECK_WRITE_ADDRESS macro to fail writes which land in the
   bootloader, partition table, or running application region.
*/
#define CHECK_WRITE_ADDRESS(CHIP, ADDR, SIZE) do { \
        if (CHIP && CHIP->os_func->region_protected) { \
            esp_err_t ret = CHIP->os_func->region_protected(CHIP->os_func_data, ADDR, SIZE); \
            if (ret == ESP_ERR_NOT_ALLOWED) { \
                return ret; /* ESP_ERR_NOT_ALLOWED from read-only partition check */ \
            } else if (ret != ESP_OK) { \
                UNSAFE_WRITE_ADDRESS; /* FAILS or ABORTS */ \
            } \
        } \
    } while(0)

/* Convenience macro for beginning of all API functions.
 * Check the return value of `rom_spiflash_api_funcs->chip_check` is correct,
 * and the chip supports the operation in question.
 */
#define VERIFY_CHIP_OP(op) do { \
        if (err != ESP_OK) return err; \
        if (chip->chip_drv->op == NULL) { \
            return ESP_ERR_FLASH_UNSUPPORTED_CHIP; \
        } \
    } while (0)


#if CONFIG_SPI_FLASH_ENABLE_COUNTERS
static esp_flash_counters_t esp_flash_stats;

#define COUNTER_START()     uint32_t ts_begin = esp_cpu_get_cycle_count()
#define COUNTER_STOP(counter)  \
    do{ \
        esp_flash_stats.counter.count++; \
        esp_flash_stats.counter.time += (esp_cpu_get_cycle_count() - ts_begin) / (esp_clk_cpu_freq() / 1000000); \
    } while(0)

#define COUNTER_ADD_BYTES(counter, size) \
    do { \
        esp_flash_stats.counter.bytes += size; \
    } while (0)



const esp_flash_counters_t *esp_flash_get_counters(void)
{
    return &esp_flash_stats;
}

void esp_flash_reset_counters(void)
{
    memset(&esp_flash_stats, 0, sizeof(esp_flash_stats));
}

void esp_flash_dump_counters(FILE* stream)
{
    if (stream != NULL) {
        fprintf(stream, " read: count=%8ld  time=%8ldus  bytes=%8ld\n", esp_flash_stats.read.count, esp_flash_stats.read.time, esp_flash_stats.read.bytes);
        fprintf(stream, "write: count=%8ld  time=%8ldus  bytes=%8ld\n", esp_flash_stats.write.count, esp_flash_stats.write.time, esp_flash_stats.write.bytes);
        fprintf(stream, "erase: count=%8ld  time=%8ldus  bytes=%8ld\n", esp_flash_stats.erase.count, esp_flash_stats.erase.time, esp_flash_stats.erase.bytes);
    }
}


const spi_flash_counters_t *spi_flash_get_counters(void)
{
    return (spi_flash_counters_t *)esp_flash_get_counters();
}

void spi_flash_reset_counters(void)
{
    esp_flash_reset_counters();
}

void spi_flash_dump_counters(void)
{
    esp_flash_dump_counters(stdout);
}

#else
#define COUNTER_START()
#define COUNTER_STOP(counter)
#define COUNTER_ADD_BYTES(counter, size)

#endif //CONFIG_SPI_FLASH_ENABLE_COUNTERS

#define IO_STR_LEN  10

static const char io_mode_str[][IO_STR_LEN] = {
    "slowrd",
    "fastrd",
    "dout",
    "dio",
    "qout",
    "qio",
    [6 ... 15] = "not used", // reserved io mode for future, not used currently.
    "opi_str",
    "opi_dtr",
};

_Static_assert(sizeof(io_mode_str)/IO_STR_LEN == SPI_FLASH_READ_MODE_MAX, "the io_mode_str should be consistent with the esp_flash_io_mode_t defined in spi_flash_types.h");

esp_err_t esp_flash_read_chip_id(esp_flash_t* chip, uint32_t* flash_id);

#ifndef CONFIG_SPI_FLASH_ROM_IMPL
static esp_err_t spiflash_start_default(esp_flash_t *chip);
static esp_err_t spiflash_end_default(esp_flash_t *chip, esp_err_t err);
static esp_err_t check_chip_pointer_default(esp_flash_t **inout_chip);
static esp_err_t flash_end_flush_cache(esp_flash_t* chip, esp_err_t err, bool bus_acquired, uint32_t address, uint32_t length);
#endif //CONFIG_SPI_FLASH_ROM_IMPL

typedef struct {
    esp_err_t (*start)(esp_flash_t *chip);
    esp_err_t (*end)(esp_flash_t *chip, esp_err_t err);
    esp_err_t (*chip_check)(esp_flash_t **inout_chip);
    esp_err_t (*flash_end_flush_cache)(esp_flash_t* chip, esp_err_t err, bool bus_acquired, uint32_t address, uint32_t length);
} rom_spiflash_api_func_t;

#ifndef CONFIG_SPI_FLASH_ROM_IMPL
// These functions can be placed in the ROM. For now we use the code in IDF.
DRAM_ATTR static rom_spiflash_api_func_t default_spiflash_rom_api = {
    .start = spiflash_start_default,
    .end = spiflash_end_default,
    .chip_check = check_chip_pointer_default,
    .flash_end_flush_cache = flash_end_flush_cache,
};

DRAM_ATTR rom_spiflash_api_func_t *rom_spiflash_api_funcs = &default_spiflash_rom_api;
#else
extern rom_spiflash_api_func_t *esp_flash_api_funcs;
#define rom_spiflash_api_funcs esp_flash_api_funcs
#endif // CONFIG_SPI_FLASH_ROM_IMPL

/* Static function to notify OS of a new SPI flash operation.

   If returns an error result, caller must abort. If returns ESP_OK, caller must
   call rom_spiflash_api_funcs->end() before returning.
*/
#ifndef CONFIG_SPI_FLASH_ROM_IMPL
static esp_err_t IRAM_ATTR spiflash_start_default(esp_flash_t *chip)
{
    if (chip->os_func != NULL && chip->os_func->start != NULL) {
        esp_err_t err = chip->os_func->start(chip->os_func_data);
        if (err != ESP_OK) {
            return err;
        }
    }
    chip->host->driver->dev_config(chip->host);
    return ESP_OK;
}

/* Static function to notify OS that SPI flash operation is complete.
 */
static esp_err_t IRAM_ATTR spiflash_end_default(esp_flash_t *chip, esp_err_t err)
{
    if (chip->os_func != NULL
        && chip->os_func->end != NULL) {
        esp_err_t end_err = chip->os_func->end(chip->os_func_data);
        if (err == ESP_OK) {
            err = end_err; // Only return the 'end' error if we haven't already failed
        }
    }
    return err;
}

// check that the 'chip' parameter is properly initialised
static IRAM_ATTR esp_err_t check_chip_pointer_default(esp_flash_t **inout_chip)
{
    esp_flash_t *chip = *inout_chip;
    if (chip == NULL) {
        chip = esp_flash_default_chip;
    }
    *inout_chip = chip;
    if (chip == NULL || !esp_flash_chip_driver_initialized(chip)) {
        return ESP_ERR_FLASH_NOT_INITIALISED;
    }
    return ESP_OK;
}

static IRAM_ATTR esp_err_t flash_end_flush_cache(esp_flash_t* chip, esp_err_t err, bool bus_acquired, uint32_t address, uint32_t length)
{
    if (!bus_acquired) {
        // Try to acquire the bus again to flush the cache before exit.
        esp_err_t acquire_err = rom_spiflash_api_funcs->start(chip);
        if (acquire_err != ESP_OK) {
            return (err == ESP_OK)? acquire_err: err;
        }
    }

    if (chip->host->driver->flush_cache) {
        esp_err_t flush_err = chip->host->driver->flush_cache(chip->host, address, length);
        if (err == ESP_OK) {
            err = flush_err;
        }
    }
    return rom_spiflash_api_funcs->end(chip, err);
}
#endif //CONFIG_SPI_FLASH_ROM_IMPL

/* Top-level API functions, calling into chip_drv functions via chip->drv */

static esp_err_t detect_spi_flash_chip(esp_flash_t *chip);

bool IRAM_ATTR esp_flash_chip_driver_initialized(const esp_flash_t *chip)
{
    if (!chip->chip_drv) return false;
    return true;
}

esp_err_t IRAM_ATTR esp_flash_init(esp_flash_t *chip)
{
    // Chip init flow
    // 1. Read chip id
    // 2. (optional) Detect chip vendor
    // 3. Get basic parameters of the chip (size, dummy count, etc.)
    // 4. Init chip into desired mode (without breaking the cache!)
    esp_err_t err = ESP_OK;
    if (chip == NULL || chip->host == NULL || chip->host->driver == NULL ||
        ((memspi_host_inst_t*)chip->host)->spi == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    //read chip id
    uint32_t flash_id;
    int retries = 10;
    do {
        err = esp_flash_read_chip_id(chip, &flash_id);
    } while (err == ESP_ERR_FLASH_NOT_INITIALISED && retries-- > 0);

    if (err != ESP_OK) {
        return err;
    }
    chip->chip_id = flash_id;

    if (!esp_flash_chip_driver_initialized(chip)) {
        // Detect chip_drv
        err = detect_spi_flash_chip(chip);
        if (err != ESP_OK) {
            return err;
        }
    }

    // Detect flash size
    uint32_t size;
    err = esp_flash_get_physical_size(chip, &size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to get chip size");
        return err;
    }

    if (chip->chip_drv->get_chip_caps == NULL) {
        // chip caps get failed, pass the flash capability check.
        ESP_EARLY_LOGW(TAG, "get_chip_caps function pointer hasn't been initialized");
    } else {
        if (((chip->chip_drv->get_chip_caps(chip) & SPI_FLASH_CHIP_CAP_32MB_SUPPORT) == 0) && (size > (16 *1024 * 1024))) {
            ESP_EARLY_LOGW(TAG, "Detected flash size > 16 MB, but access beyond 16 MB is not supported for this flash model yet.");
            size = (16 * 1024 * 1024);
        }
    }

    ESP_LOGI(TAG, "flash io: %s", io_mode_str[chip->read_mode]);
    err = rom_spiflash_api_funcs->start(chip);
    if (err != ESP_OK) {
        return err;
    }

    if (err == ESP_OK) {
        // Try to set the flash mode to whatever default mode was chosen
        err = chip->chip_drv->set_io_mode(chip);
        if (err == ESP_ERR_FLASH_NO_RESPONSE && !esp_flash_is_quad_mode(chip)) {
            //some chips (e.g. Winbond) don't support to clear QE, treat as success
            err = ESP_OK;
        }
    }
    // Done: all fields on 'chip' are initialised
    return rom_spiflash_api_funcs->end(chip, err);
}

// Note: This function is only used for internal. Only call this function to initialize the main flash.
// (flash chip on SPI1 CS0)
esp_err_t IRAM_ATTR esp_flash_init_main(esp_flash_t *chip)
{
    // Chip init flow
    // 1. Read chip id
    // 2. (optional) Detect chip vendor
    // 3. Get basic parameters of the chip (size, dummy count, etc.)
    // 4. Init chip into desired mode (without breaking the cache!)
    esp_err_t err = ESP_OK;
    bool octal_mode;

    if (chip == NULL || chip->host == NULL || chip->host->driver == NULL ||
        ((memspi_host_inst_t*)chip->host)->spi == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    octal_mode = (chip->read_mode >= SPI_FLASH_OPI_FLAG);
    //read chip id
    // This can indicate the MSPI support OPI, if the flash works on MSPI in OPI mode, we directly bypass read id.
    uint32_t flash_id = 0;
    if (octal_mode) {
        // bypass the reading but get the flash_id from the ROM variable, to avoid resetting the chip to QSPI mode and read the ID again
        flash_id = g_rom_flashchip.device_id;
    } else {
        int retries = 10;
        do {
            err = esp_flash_read_chip_id(chip, &flash_id);
        } while (err == ESP_ERR_FLASH_NOT_INITIALISED && retries-- > 0);
    }

    if (err != ESP_OK) {
        return err;
    }
    chip->chip_id = flash_id;

    if (!esp_flash_chip_driver_initialized(chip)) {
        // Detect chip_drv
        err = detect_spi_flash_chip(chip);
        if (err != ESP_OK) {
            return err;
        }
    }

    // Detect flash size
    uint32_t size;
    err = esp_flash_get_physical_size(chip, &size);
    if (err != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "failed to get chip size");
        return err;
    }

    if (chip->chip_drv->get_chip_caps == NULL) {
        // chip caps get failed, pass the flash capability check.
        ESP_EARLY_LOGW(TAG, "get_chip_caps function pointer hasn't been initialized");
    } else {
        if (((chip->chip_drv->get_chip_caps(chip) & SPI_FLASH_CHIP_CAP_32MB_SUPPORT) == 0) && (size > (16 *1024 * 1024))) {
            ESP_EARLY_LOGW(TAG, "Detected flash size > 16 MB, but access beyond 16 MB is not supported for this flash model yet.");
            size = (16 * 1024 * 1024);
        }
    }

    ESP_EARLY_LOGI(TAG, "flash io: %s", io_mode_str[chip->read_mode]);
    err = rom_spiflash_api_funcs->start(chip);
    if (err != ESP_OK) {
        return err;
    }

    if (err == ESP_OK && !octal_mode) {
        // Try to set the flash mode to whatever default mode was chosen
        err = chip->chip_drv->set_io_mode(chip);
        if (err == ESP_ERR_FLASH_NO_RESPONSE && !esp_flash_is_quad_mode(chip)) {
            //some chips (e.g. Winbond) don't support to clear QE, treat as success
            err = ESP_OK;
        }
    }
    // Done: all fields on 'chip' are initialised
    return rom_spiflash_api_funcs->end(chip, err);
}

static esp_err_t IRAM_ATTR read_id_core(esp_flash_t* chip, uint32_t* out_id, bool sanity_check)
{
    bool installed = esp_flash_chip_driver_initialized(chip);
    esp_err_t err = rom_spiflash_api_funcs->start(chip);
    if (err != ESP_OK) {
        return err;
    }

    esp_err_t (*read_id_func)(void*, uint32_t*);
    void* read_id_arg;
    if (installed && chip->chip_drv->read_id) {
        read_id_func = (void*)chip->chip_drv->read_id;
        read_id_arg = (void*)chip;
    } else {
        //default option if the chip is not detected/chosen yet.
        read_id_func = (void*)chip->host->driver->read_id;
        read_id_arg = (void*)chip->host;
    }

    // Inner function fails if it sees all-ones or all-zeroes.
    err = read_id_func(read_id_arg, out_id);

    if (sanity_check && err == ESP_OK) {
        // Send RDID command twice, check for a matching result and retry in case we just powered on
        uint32_t new_id;
        err = read_id_func(read_id_arg, &new_id);
        if (err == ESP_OK && (new_id != *out_id)) {
            err = ESP_ERR_FLASH_NOT_INITIALISED;
        }
    }

    return rom_spiflash_api_funcs->end(chip, err);
}

// Faster version with sanity check.
// Called in esp_flash_init and unit test (though not public)
esp_err_t esp_flash_read_chip_id(esp_flash_t* chip, uint32_t* out_id)
{
    return read_id_core(chip, out_id, true);
}

#ifndef CONFIG_SPI_FLASH_ROM_IMPL
esp_err_t esp_flash_read_id(esp_flash_t* chip, uint32_t* out_id)
{
    esp_err_t err = rom_spiflash_api_funcs->chip_check(&chip);
    //Accept uninitialized chip when reading chip id
    if (err != ESP_OK && !(err == ESP_ERR_FLASH_NOT_INITIALISED && chip != NULL)) return err;
    if (out_id == NULL) return ESP_ERR_INVALID_ARG;

    return read_id_core(chip, out_id, false);
}
#endif //CONFIG_SPI_FLASH_ROM_IMPL

static esp_err_t IRAM_ATTR NOINLINE_ATTR read_unique_id(esp_flash_t* chip, uint64_t* out_uid)
{
    esp_err_t err = rom_spiflash_api_funcs->start(chip);
    if (err != ESP_OK) {
        return err;
    }

    err = chip->chip_drv->read_unique_id(chip, out_uid);

    return rom_spiflash_api_funcs->end(chip, err);
}

esp_err_t esp_flash_read_unique_chip_id(esp_flash_t *chip, uint64_t* out_uid)
{
    esp_err_t err = rom_spiflash_api_funcs->chip_check(&chip);
    if (err != ESP_OK) {
        return err;
    }
    if (chip->chip_drv->get_chip_caps == NULL) {
        // chip caps get failed, pass the flash capability check.
        ESP_EARLY_LOGW(TAG, "get_chip_caps function pointer hasn't been initialized");
    } else {
        if ((chip->chip_drv->get_chip_caps(chip) & SPI_FLASH_CHIP_CAP_UNIQUE_ID) == 0) {
            ESP_EARLY_LOGE(TAG, "chip %s doesn't support reading unique id", chip->chip_drv->name);
            return ESP_ERR_NOT_SUPPORTED;
        }
    }

    if (out_uid == NULL) {
        return ESP_ERR_INVALID_ARG;
    };

    return read_unique_id(chip, out_uid);
}

static esp_err_t IRAM_ATTR detect_spi_flash_chip(esp_flash_t *chip)
{
    esp_err_t err;
    uint32_t flash_id = chip->chip_id;

    // Detect the chip and set the chip_drv structure for it
    const spi_flash_chip_t **drivers = esp_flash_registered_chips;
    while (*drivers != NULL && !esp_flash_chip_driver_initialized(chip)) {
        chip->chip_drv = *drivers;
        // start/end SPI operation each time, for multitasking
        // and also so esp_flash_registered_flash_drivers can live in flash
        ESP_EARLY_LOGD(TAG, "trying chip: %s", chip->chip_drv->name);

        err = rom_spiflash_api_funcs->start(chip);
        if (err != ESP_OK) {
            return err;
        }

        if (chip->chip_drv->probe(chip, flash_id) != ESP_OK) {
            chip->chip_drv = NULL;
        }
        // if probe succeeded, chip->drv stays set
        drivers++;

        err = rom_spiflash_api_funcs->end(chip, err);
        if (err != ESP_OK) {
            return err;
        }
    }
    if (!esp_flash_chip_driver_initialized(chip)) {
        return ESP_ERR_NOT_FOUND;
    }
    ESP_EARLY_LOGI(TAG, "detected chip: %s", chip->chip_drv->name);
    return ESP_OK;
}

esp_err_t IRAM_ATTR esp_flash_get_physical_size(esp_flash_t *chip, uint32_t *flash_size)
{
    esp_err_t err = rom_spiflash_api_funcs->chip_check(&chip);
    if (err != ESP_OK) {
        return err;
    }
    VERIFY_CHIP_OP(detect_size);
    if (flash_size == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = rom_spiflash_api_funcs->start(chip);
    if (err != ESP_OK) {
        return err;
    }
    uint32_t detect_size;
    err = chip->chip_drv->detect_size(chip, &detect_size);
    if (err == ESP_OK) {
        if (chip->size == 0) {
            // chip->size will not be changed if detected, it will always be equal to configured flash size.
            chip->size = detect_size;
        }
        *flash_size = detect_size;
    }
    return rom_spiflash_api_funcs->end(chip, err);
}

#ifndef CONFIG_SPI_FLASH_ROM_IMPL

/* Return true if regions 'a' and 'b' overlap at all, based on their start offsets and lengths. */
inline static bool regions_overlap(uint32_t a_start, uint32_t a_len,uint32_t b_start, uint32_t b_len);

esp_err_t IRAM_ATTR esp_flash_get_size(esp_flash_t *chip, uint32_t *out_size)
{
    esp_err_t err = rom_spiflash_api_funcs->chip_check(&chip);
    if (err != ESP_OK) {
        return err;
    }
    if (out_size == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (chip->size != 0) {
        *out_size = chip->size;
        return ESP_OK;
    }
    //Return flash chip physical size, when this API is called before flash initialisation,
    //After initialization will return available size.
    return esp_flash_get_physical_size(chip, out_size);
}

esp_err_t IRAM_ATTR esp_flash_erase_chip(esp_flash_t *chip)
{
    esp_err_t err = ESP_OK;
    uint32_t size = 0;
    err = esp_flash_get_size(chip, &size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_flash_get_size failed, flash error code: %d", err);
        return err;
    }
    err = esp_flash_erase_region(chip, 0, size);
    return err;
}

esp_err_t IRAM_ATTR esp_flash_erase_region(esp_flash_t *chip, uint32_t start, uint32_t len)
{
    esp_err_t err = rom_spiflash_api_funcs->chip_check(&chip);
    VERIFY_CHIP_OP(erase_sector);
    VERIFY_CHIP_OP(erase_block);
    CHECK_WRITE_ADDRESS(chip, start, len);

    uint32_t block_erase_size = chip->chip_drv->erase_block == NULL ? 0 : chip->chip_drv->block_erase_size;
    uint32_t sector_size = chip->chip_drv->sector_size;

    COUNTER_START();

    if (sector_size == 0 || (block_erase_size % sector_size) != 0) {
        return ESP_ERR_FLASH_NOT_INITIALISED;
    }
    if (start > chip->size || start + len > chip->size) {
        return ESP_ERR_INVALID_ARG;
    }
    if ((start % chip->chip_drv->sector_size) != 0 || (len % chip->chip_drv->sector_size) != 0) {
        // Can only erase multiples of the sector size, starting at sector boundary
        return ESP_ERR_INVALID_ARG;
    }
    if (len == 0) {
        return ESP_OK;
    }

    err = ESP_OK;
    // Check for write protected regions overlapping the erase region
    if (chip->chip_drv->get_protected_regions != NULL &&
        chip->chip_drv->num_protectable_regions > 0) {

        err = rom_spiflash_api_funcs->start(chip);
        if (err != ESP_OK) {
            return err;
        }
        uint64_t protected = 0;
        err = chip->chip_drv->get_protected_regions(chip, &protected);
        if (err == ESP_OK && protected != 0) {
            for (int i = 0; i < chip->chip_drv->num_protectable_regions && err == ESP_OK; i++) {
                const esp_flash_region_t *region = &chip->chip_drv->protectable_regions[i];
                if ((protected & BIT64(i))
                    && regions_overlap(start, len, region->offset, region->size)) {
                    err = ESP_ERR_FLASH_PROTECTED;
                }
            }
        }
        // Don't lock the SPI flash for the entire erase, as this may be very long
        err = rom_spiflash_api_funcs->end(chip, err);
    }
    if (err != ESP_OK) {
        return err;
    }

    uint32_t erase_addr = start;
    uint32_t len_remain = len;
    // Indicate whether the bus is acquired by the driver, needs to be released before return
    bool bus_acquired = false;
    while (1) {
        //check before the operation, in case this is called too close to the last operation
        if (chip->chip_drv->yield) {
            err = chip->chip_drv->yield(chip, 0);
            if (err != ESP_OK) {
                return err;
            }
        }

        err = rom_spiflash_api_funcs->start(chip);
        if (err != ESP_OK) {
            break;
        }
        bus_acquired = true;

#ifndef CONFIG_SPI_FLASH_BYPASS_BLOCK_ERASE
        // If possible erase an entire multi-sector block
        if (block_erase_size > 0 && len_remain >= block_erase_size && (erase_addr % block_erase_size) == 0) {
            err = chip->chip_drv->erase_block(chip, erase_addr);
            erase_addr += block_erase_size;
            len_remain -= block_erase_size;
            COUNTER_ADD_BYTES(erase, block_erase_size);
        } else
#endif
        {
            // Otherwise erase individual sector only
            err = chip->chip_drv->erase_sector(chip, erase_addr);
            erase_addr += sector_size;
            len_remain -= sector_size;
            COUNTER_ADD_BYTES(erase, sector_size);
        }

        assert(len_remain < len);

        if (err != ESP_OK || len_remain == 0) {
            // On ESP32, the cache re-enable is in the end() function, while flush_cache should
            // happen when the cache is still disabled on ESP32. Break before the end() function and
            // do end() later
            assert(bus_acquired);
            break;
        }

        err = rom_spiflash_api_funcs->end(chip, ESP_OK);
        if (err != ESP_OK) {
            break;
        }
        bus_acquired = false;
    }

    COUNTER_STOP(erase);

    return rom_spiflash_api_funcs->flash_end_flush_cache(chip, err, bus_acquired, start, len);
}

#endif // !CONFIG_SPI_FLASH_ROM_IMPL

#if defined(CONFIG_SPI_FLASH_ROM_IMPL) && ESP_ROM_HAS_ERASE_0_REGION_BUG

/* ROM esp_flash_erase_region implementation doesn't handle 0 erase size correctly.
 * Check the size and call ROM function instead of overriding it completely.
 * The behavior is slightly different from esp_flash_erase_region above, thought:
 * here the check for 0 size is done first, but in esp_flash_erase_region the check is
 * done after the other arguments are checked.
 */
extern esp_err_t rom_esp_flash_erase_region(esp_flash_t *chip, uint32_t start, uint32_t len);
esp_err_t IRAM_ATTR esp_flash_erase_region(esp_flash_t *chip, uint32_t start, uint32_t len)
{
    if (len == 0) {
        return ESP_OK;
    }
    return rom_esp_flash_erase_region(chip, start, len);
}
#endif // defined(CONFIG_SPI_FLASH_ROM_IMPL) && ESP_ROM_HAS_ERASE_0_REGION_BUG

#ifndef CONFIG_SPI_FLASH_ROM_IMPL

esp_err_t IRAM_ATTR esp_flash_get_chip_write_protect(esp_flash_t *chip, bool *out_write_protected)
{
    esp_err_t err = rom_spiflash_api_funcs->chip_check(&chip);
    VERIFY_CHIP_OP(get_chip_write_protect);
    if (out_write_protected == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    err = rom_spiflash_api_funcs->start(chip);
    if (err != ESP_OK) {
        return err;
    }

    err = chip->chip_drv->get_chip_write_protect(chip, out_write_protected);

    return rom_spiflash_api_funcs->end(chip, err);
}

esp_err_t IRAM_ATTR esp_flash_set_chip_write_protect(esp_flash_t *chip, bool write_protect)
{
    esp_err_t err = rom_spiflash_api_funcs->chip_check(&chip);
    VERIFY_CHIP_OP(set_chip_write_protect);
    //TODO: skip writing if already locked or unlocked

    err = rom_spiflash_api_funcs->start(chip);
    if (err != ESP_OK) {
        return err;
    }

    err = chip->chip_drv->set_chip_write_protect(chip, write_protect);

    return rom_spiflash_api_funcs->end(chip, err);
}

esp_err_t esp_flash_get_protectable_regions(const esp_flash_t *chip, const esp_flash_region_t **out_regions, uint32_t *out_num_regions)
{
    if(out_num_regions != NULL) {
        *out_num_regions = 0; // In case caller doesn't check result
    }
    esp_err_t err = rom_spiflash_api_funcs->chip_check((esp_flash_t **)&chip);
    VERIFY_CHIP_OP(get_protected_regions);

    if(out_regions == NULL || out_num_regions == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    *out_num_regions = chip->chip_drv->num_protectable_regions;
    *out_regions = chip->chip_drv->protectable_regions;
    return ESP_OK;
}

static esp_err_t find_region(const esp_flash_t *chip, const esp_flash_region_t *region, uint8_t *index)
{
   if (region == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

   for(*index = 0; *index < chip->chip_drv->num_protectable_regions; (*index)++) {
       if (memcmp(&chip->chip_drv->protectable_regions[*index],
                  region, sizeof(esp_flash_region_t)) == 0) {
           return ESP_OK;
       }
   }

   return ESP_ERR_NOT_FOUND;
}

esp_err_t IRAM_ATTR esp_flash_get_protected_region(esp_flash_t *chip, const esp_flash_region_t *region, bool *out_protected)
{
    esp_err_t err = rom_spiflash_api_funcs->chip_check(&chip);
    VERIFY_CHIP_OP(get_protected_regions);

    if (out_protected == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t index;
    err = find_region(chip, region, &index);
    if (err != ESP_OK) {
        return err;
    }

    uint64_t protection_mask = 0;
    err = rom_spiflash_api_funcs->start(chip);
    if (err != ESP_OK) {
        return err;
    }

    err = chip->chip_drv->get_protected_regions(chip, &protection_mask);
    if (err == ESP_OK) {
        *out_protected = protection_mask & (1LL << index);
    }

    return rom_spiflash_api_funcs->end(chip, err);
}

esp_err_t IRAM_ATTR esp_flash_set_protected_region(esp_flash_t *chip, const esp_flash_region_t *region, bool protect)
{
    esp_err_t err = rom_spiflash_api_funcs->chip_check(&chip);
    VERIFY_CHIP_OP(set_protected_regions);

    uint8_t index;
    err = find_region(chip, region, &index);
    if (err != ESP_OK) {
        return err;
    }

    uint64_t protection_mask = 0;
    err = rom_spiflash_api_funcs->start(chip);
    if (err != ESP_OK) {
        return err;
    }

    err = chip->chip_drv->get_protected_regions(chip, &protection_mask);
    if (err == ESP_OK) {
        if (protect) {
            protection_mask |= (1LL << index);
        } else {
            protection_mask &= ~(1LL << index);
        }
        err = chip->chip_drv->set_protected_regions(chip, protection_mask);
    }

    return rom_spiflash_api_funcs->end(chip, err);
}

esp_err_t IRAM_ATTR esp_flash_read(esp_flash_t *chip, void *buffer, uint32_t address, uint32_t length)
{
    esp_err_t err = rom_spiflash_api_funcs->chip_check(&chip);
    VERIFY_CHIP_OP(read);
    if (buffer == NULL || address > chip->size || address+length > chip->size) {
        return ESP_ERR_INVALID_ARG;
    }
    if (length == 0) {
        return ESP_OK;
    }

    //when the cache is disabled, only the DRAM can be read, check whether we need to receive in another buffer in DRAM.
    bool direct_read = false;
    //If the buffer is internal already, it's ok to use it directly
    direct_read |= esp_ptr_in_dram(buffer);
    //If not, we need to check if the HW support direct write
    direct_read |= chip->host->driver->supports_direct_read(chip->host, buffer);
    uint8_t* temp_buffer = NULL;

    //each time, we at most read this length
    //after that, we release the lock to allow some other operations
    size_t read_chunk_size = MIN(MAX_READ_CHUNK, length);

    if (!direct_read) {
        size_t actual_len = 0;
        if (chip->os_func->get_temp_buffer != NULL) {
            temp_buffer = chip->os_func->get_temp_buffer(chip->os_func_data, read_chunk_size, &actual_len);
            read_chunk_size = actual_len;
        }
        if (temp_buffer == NULL) {
            return ESP_ERR_NO_MEM;
        }
    }

    COUNTER_START();

    err = ESP_OK;
    do {
        err = rom_spiflash_api_funcs->start(chip);
        if (err != ESP_OK) {
            break;
        }
        //if required (dma buffer allocated), read to the buffer instead of the original buffer
        uint8_t* buffer_to_read = (temp_buffer)? temp_buffer : buffer;

        // Length we will read this iteration is either the chunk size or the remaining length, whichever is smaller
        size_t length_to_read = MIN(read_chunk_size, length);

        if (err == ESP_OK) {
            err = chip->chip_drv->read(chip, buffer_to_read, address, length_to_read);
        }
        if (err != ESP_OK) {
            rom_spiflash_api_funcs->end(chip, err);
            break;
        }
        //even if this is failed, the data is still valid, copy before quit
        err = rom_spiflash_api_funcs->end(chip, err);

        //copy back to the original buffer
        if (temp_buffer) {
            memcpy(buffer, temp_buffer, length_to_read);
        }
        address += length_to_read;
        length -= length_to_read;
        buffer = (void*)((intptr_t)buffer + length_to_read);
        COUNTER_ADD_BYTES(read, length_to_read);
    } while (err == ESP_OK && length > 0);

    if (chip->os_func->release_temp_buffer != NULL) {
        chip->os_func->release_temp_buffer(chip->os_func_data, temp_buffer);
    }

    COUNTER_STOP(read);
    return err;
}

#if CONFIG_SPI_FLASH_WARN_SETTING_ZERO_TO_ONE
static esp_err_t IRAM_ATTR s_check_setting_zero_to_one(esp_flash_t *chip, uint32_t verify_address, uint32_t remain_verify_len, const uint32_t *to_write_buf, bool is_encrypted)
{
    esp_err_t err = ESP_FAIL;
    uint8_t verify_buffer[VERIFY_BUF_LEN];
    uint32_t *val_in_flash = (uint32_t *)verify_buffer;

    while (remain_verify_len) {
        uint32_t this_len = MIN(remain_verify_len, VERIFY_BUF_LEN);

        err = chip->chip_drv->read(chip, verify_buffer, verify_address, this_len);
        if (err != ESP_OK) {
            ESP_DRAM_LOGE(TAG, "failed to read flash to verify if setting zero to one, err: 0x%x", err);
            return err;
        }

        for (int r = 0; r < this_len / sizeof(uint32_t); r++) {
            if (is_encrypted) {
                (void)to_write_buf;
                if (val_in_flash[r] != 0xFFFFFFFF) {
                    ESP_DRAM_LOGW(TAG, "Write at offset 0x%x but not erased (0x%08x)",
                                  verify_address + r, val_in_flash[r]);
                }
            } else {
                if ((val_in_flash[r] & to_write_buf[r]) != to_write_buf[r]) {
                    ESP_DRAM_LOGW(TAG, "Write at offset 0x%x requests 0x%08x but will write 0x%08x -> 0x%08x",
                                  verify_address + r, to_write_buf[r], val_in_flash[r], (val_in_flash[r] & to_write_buf[r]));
                }
            }
        }

        remain_verify_len -= this_len;
        verify_address += this_len;
    }

    return ESP_OK;
}
#endif  //#if CONFIG_SPI_FLASH_WARN_SETTING_ZERO_TO_ONE

#if CONFIG_SPI_FLASH_VERIFY_WRITE
static esp_err_t IRAM_ATTR s_verify_write(esp_flash_t *chip, uint32_t verify_address, uint32_t remain_verify_len, const uint32_t *expected_buf, bool is_encrypted)
{
    esp_err_t err = ESP_FAIL;
    uint8_t verify_buffer[VERIFY_BUF_LEN];
    uint32_t *val_in_flash = (uint32_t *)verify_buffer;

    while (remain_verify_len) {
        uint32_t this_len = MIN(remain_verify_len, VERIFY_BUF_LEN);

        if (is_encrypted) {
            err = esp_flash_read_encrypted(chip, verify_address, verify_buffer, this_len);
        } else {
            err = chip->chip_drv->read(chip, verify_buffer, verify_address, this_len);
        }
        if (err != ESP_OK) {
            ESP_DRAM_LOGE(TAG, "failed to read flash to verify previous write, err: 0x%x", err);
            return err;
        }

        for (int r = 0; r < this_len / sizeof(uint32_t); r++) {
            if (val_in_flash[r] != expected_buf[r]) {
#if CONFIG_SPI_FLASH_LOG_FAILED_WRITE
                ESP_DRAM_LOGE(TAG, "Bad write at %d offset: 0x%x, expected: 0x%08x, readback: 0x%08x", r, verify_address + r, expected_buf[r], val_in_flash[r]);
#endif  //#if CONFIG_SPI_FLASH_LOG_FAILED_WRITE
                return ESP_FAIL;
            }
        }

        expected_buf = (uint32_t *)((void *)expected_buf + this_len);
        remain_verify_len -= this_len;
        verify_address += this_len;
    }

    return ESP_OK;
}
#endif //#if CONFIG_SPI_FLASH_VERIFY_WRITE

esp_err_t IRAM_ATTR esp_flash_write(esp_flash_t *chip, const void *buffer, uint32_t address, uint32_t length)
{
    esp_err_t ret = ESP_FAIL;
#if CONFIG_SPI_FLASH_VERIFY_WRITE
    //used for verify write
    bool is_encrypted = false;
#endif //CONFIG_SPI_FLASH_VERIFY_WRITE

    esp_err_t err = rom_spiflash_api_funcs->chip_check(&chip);
    VERIFY_CHIP_OP(write);
    CHECK_WRITE_ADDRESS(chip, address, length);
    if (buffer == NULL || address > chip->size || address+length > chip->size) {
        return ESP_ERR_INVALID_ARG;
    }
    if (length == 0) {
        return ESP_OK;
    }

    //when the cache is disabled, only the DRAM can be read, check whether we need to copy the data first
    bool direct_write = false;
    //If the buffer is internal already, it's ok to write it directly
    direct_write |= esp_ptr_in_dram(buffer);
    //If not, we need to check if the HW support direct write
    direct_write |= chip->host->driver->supports_direct_write(chip->host, buffer);

    // Indicate whether the bus is acquired by the driver, needs to be released before return
    bool bus_acquired = false;
    err = ESP_OK;

    COUNTER_START();

    /* Write output in chunks, either by buffering on stack or
       by artificially cutting into MAX_WRITE_CHUNK parts (in an OS
       environment, this prevents writing from causing interrupt or higher priority task
       starvation.) */
    uint32_t write_addr = address;
    uint32_t len_remain = length;
    while (1) {
        uint32_t write_len;
        const void *write_buf;
        uint32_t temp_buf[8];
        if (direct_write) {
            write_len = MIN(len_remain, MAX_WRITE_CHUNK);
            write_buf = buffer;
        } else {
            write_len = MIN(len_remain, sizeof(temp_buf));
            memcpy(temp_buf, buffer, write_len);
            write_buf = temp_buf;
        }

        //check before the operation, in case this is called too close to the last operation
        if (chip->chip_drv->yield) {
            err = chip->chip_drv->yield(chip, 0);
            if (err != ESP_OK) {
                return err;
            }
        }

        err = rom_spiflash_api_funcs->start(chip);
        if (err != ESP_OK) {
            goto restore_cache;
        }
        bus_acquired = true;

#if CONFIG_SPI_FLASH_WARN_SETTING_ZERO_TO_ONE
        err = s_check_setting_zero_to_one(chip, write_addr, write_len, write_buf, is_encrypted);
        if (err != ESP_OK) {
            //Error happens, we end flash operation. Re-enable cache and flush it
            goto restore_cache;
        }
#endif  //#if CONFIG_SPI_FLASH_WARN_SETTING_ZERO_TO_ONE

        err = chip->chip_drv->write(chip, write_buf, write_addr, write_len);
        len_remain -= write_len;
        assert(len_remain < length);
        COUNTER_ADD_BYTES(write, write_len);

        if (err != ESP_OK) {
            //Error happens, we end flash operation. Re-enable cache and flush it
            assert(bus_acquired);
            goto restore_cache;
        }

#if CONFIG_SPI_FLASH_VERIFY_WRITE
        err = s_verify_write(chip, write_addr, write_len, write_buf, is_encrypted);
        if (err != ESP_OK) {
            //Error happens, we end flash operation. Re-enable cache and flush it
            goto restore_cache;
        }
#endif  //#if CONFIG_SPI_FLASH_VERIFY_WRITE


        if (len_remain == 0) {
            //Flash operation done
            break;
        }

        err = rom_spiflash_api_funcs->end(chip, err);
        if (err != ESP_OK) {
            goto restore_cache;
        }
        bus_acquired = false;

        write_addr += write_len;
        buffer = (void *)((intptr_t)buffer + write_len);
    }

    COUNTER_STOP(write);
    err = rom_spiflash_api_funcs->flash_end_flush_cache(chip, err, bus_acquired, address, length);

    return err;

restore_cache:
    COUNTER_STOP(write);
    ret = rom_spiflash_api_funcs->flash_end_flush_cache(chip, err, bus_acquired, address, length);
    if (ret != ESP_OK) {
       ESP_DRAM_LOGE(TAG, "restore cache fail\n");
    }

    return err;
}

esp_err_t IRAM_ATTR esp_flash_write_encrypted(esp_flash_t *chip, uint32_t address, const void *buffer, uint32_t length)
{
    esp_err_t ret = ESP_FAIL;
#if CONFIG_SPI_FLASH_VERIFY_WRITE
    //used for verify write
    bool is_encrypted = true;
#endif //CONFIG_SPI_FLASH_VERIFY_WRITE

    esp_err_t err = rom_spiflash_api_funcs->chip_check(&chip);
    // Flash encryption only support on main flash.
    if (chip != esp_flash_default_chip) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    if (err != ESP_OK) return err;
    if (buffer == NULL || address + length > chip->size) {
        return ESP_ERR_INVALID_ARG;
    }

    if ((address % 16) != 0) {
        ESP_EARLY_LOGE(TAG, "flash encrypted write address must be 16 bytes aligned");
        return ESP_ERR_INVALID_ARG;
    }

    if (length == 0) {
        return ESP_OK;
    }

    if ((length % 16) != 0) {
        ESP_EARLY_LOGE(TAG, "flash encrypted write length must be multiple of 16");
        return ESP_ERR_INVALID_SIZE;
    }

    bool bus_acquired = false;

    const uint8_t *ssrc = (const uint8_t *)buffer;

    COUNTER_START();

    /* On ESP32, write_encrypted encrypts data in RAM as it writes,
       so copy to a temporary buffer - 32 bytes at a time.

       Each call to write_encrypted takes a 32 byte "row" of
       data to encrypt, and each row is two 16 byte AES blocks
       that share a key (as derived from flash address).

       On ESP32-S2 and later, the temporary buffer need to be
       seperated into 16-bytes, 32-bytes, 64-bytes(if supported).

       So, on ESP32-S2 and later, here has a totally different
       data prepare implementation.
    */
    uint8_t encrypt_buf[64] __attribute__((aligned(4)));
    uint32_t row_size_length;
    for (size_t i = 0; i < length; i += row_size_length) {
        uint32_t row_addr = address + i;
        uint8_t row_size;
        uint8_t encrypt_byte;
#if CONFIG_IDF_TARGET_ESP32
        if (i == 0 && (row_addr % 32) != 0) {
            /* writing to second block of a 32 byte row */
            row_size = 16;
            row_addr -= 16;
            /* copy to second block in buffer */
            memcpy(encrypt_buf + 16, ssrc + i, row_size);
            /* decrypt the first block from flash, will reencrypt to same bytes */
            esp_flash_read_encrypted(chip, row_addr, encrypt_buf, 16);
        } else if (length - i == 16) {
            /* 16 bytes left, is first block of a 32 byte row */
            row_size = 16;
            /* copy to first block in buffer */
            memcpy(encrypt_buf, ssrc + i, row_size);
            /* decrypt the second block from flash, will reencrypt to same bytes */
            esp_flash_read_encrypted(chip, row_addr + 16, encrypt_buf + 16, 16);
        } else {
            /* Writing a full 32 byte row (2 blocks) */
            row_size = 32;
            memcpy(encrypt_buf, ssrc + i, row_size);
        }
        encrypt_byte = 32;
        row_size_length = row_size;
#else // FOR ESP32-S2, ESP32-S3, ESP32-C3
        if ((row_addr % 64) == 0 && (length - i) >= 64 && SOC_FLASH_ENCRYPTED_XTS_AES_BLOCK_MAX == 64) {
            row_size = 64;
            memcpy(encrypt_buf, ssrc + i, row_size);
        } else if ((row_addr % 32) == 0 && (length - i) >= 32) {
            row_size = 32;
            memcpy(encrypt_buf, ssrc + i, row_size);
        } else {
            row_size = 16;
            memcpy(encrypt_buf, ssrc + i, row_size);
        }
        encrypt_byte = row_size;
        row_size_length = row_size;
#endif //CONFIG_IDF_TARGET_ESP32

#if CONFIG_SPI_FLASH_WARN_SETTING_ZERO_TO_ONE
        err = s_check_setting_zero_to_one(chip, row_addr, encrypt_byte, NULL, is_encrypted);
        if (err != ESP_OK) {
            //Error happens, we end flash operation. Re-enable cache and flush it
            goto restore_cache;
        }
#endif  //#if CONFIG_SPI_FLASH_WARN_SETTING_ZERO_TO_ONE

#if CONFIG_IDF_TARGET_ESP32S2
        esp_crypto_dma_lock_acquire();
#endif //CONFIG_IDF_TARGET_ESP32S2
        err = rom_spiflash_api_funcs->start(chip);

        if (err != ESP_OK) {
#if CONFIG_IDF_TARGET_ESP32S2
            esp_crypto_dma_lock_release();
#endif //CONFIG_IDF_TARGET_ESP32S2
            //Error happens, we end flash operation. Re-enable cache and flush it
            goto restore_cache;
        }
        bus_acquired = true;

        err = chip->chip_drv->write_encrypted(chip, (uint32_t *)encrypt_buf, row_addr, encrypt_byte);
        if (err!= ESP_OK) {
#if CONFIG_IDF_TARGET_ESP32S2
            esp_crypto_dma_lock_release();
#endif //CONFIG_IDF_TARGET_ESP32S2
            bus_acquired = false;
            assert(bus_acquired);
            //Error happens, we end flash operation. Re-enable cache and flush it
            goto restore_cache;
        }
        err = rom_spiflash_api_funcs->end(chip, ESP_OK);
        COUNTER_ADD_BYTES(write, encrypt_byte);
#if CONFIG_IDF_TARGET_ESP32S2
        esp_crypto_dma_lock_release();
#endif //CONFIG_IDF_TARGET_ESP32S2
        if (err != ESP_OK) {
            bus_acquired = false;
            //Error happens, we end flash operation. Re-enable cache and flush it
            goto restore_cache;
        }
        bus_acquired = false;

#if CONFIG_SPI_FLASH_VERIFY_WRITE
        err = s_verify_write(chip, row_addr, encrypt_byte, (uint32_t *)encrypt_buf, is_encrypted);
        if (err != ESP_OK) {
            //Error happens, we end flash operation. Re-enable cache and flush it
            goto restore_cache;
        }
#endif //CONFIG_SPI_FLASH_VERIFY_WRITE
    }

    COUNTER_STOP(write);
    err = rom_spiflash_api_funcs->flash_end_flush_cache(chip, err, bus_acquired, address, length);

    return err;

restore_cache:
    COUNTER_STOP(write);
    ret = rom_spiflash_api_funcs->flash_end_flush_cache(chip, err, bus_acquired, address, length);
    if (ret != ESP_OK) {
       ESP_DRAM_LOGE(TAG, "restore cache fail\n");
    }

    return err;
}

inline static IRAM_ATTR bool regions_overlap(uint32_t a_start, uint32_t a_len,uint32_t b_start, uint32_t b_len)
{
    uint32_t a_end = a_start + a_len;
    uint32_t b_end = b_start + b_len;
    return (a_end > b_start && b_end > a_start);
}

esp_err_t IRAM_ATTR esp_flash_read_encrypted(esp_flash_t *chip, uint32_t address, void *out_buffer, uint32_t length)
{
    esp_err_t err = rom_spiflash_api_funcs->chip_check(&chip);
    if (err != ESP_OK) return err;
    if (address + length > g_rom_flashchip.chip_size) {
        return ESP_ERR_INVALID_SIZE;
    }
    if (length == 0) {
        return ESP_OK;
    }
    if (out_buffer == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    COUNTER_START();
    const uint8_t *map;
    spi_flash_mmap_handle_t map_handle;
    size_t map_src = address & ~(SPI_FLASH_MMU_PAGE_SIZE - 1);
    size_t map_size = length + (address - map_src);

    err = spi_flash_mmap(map_src, map_size, SPI_FLASH_MMAP_DATA, (const void **)&map, &map_handle);
    if (err != ESP_OK) {
        return err;
    }
    memcpy(out_buffer, map + (address - map_src), length);
    spi_flash_munmap(map_handle);

    COUNTER_ADD_BYTES(read, length);
    COUNTER_STOP(read);
    return err;
}

// test only, non-public
IRAM_ATTR esp_err_t esp_flash_get_io_mode(esp_flash_t* chip, bool* qe)
{
    esp_err_t err = rom_spiflash_api_funcs->chip_check(&chip);
    VERIFY_CHIP_OP(get_io_mode);
    esp_flash_io_mode_t io_mode;

    err = rom_spiflash_api_funcs->start(chip);
    if (err != ESP_OK) {
        return err;
    }
    err = chip->chip_drv->get_io_mode(chip, &io_mode);
    err = rom_spiflash_api_funcs->end(chip, err);
    if (err == ESP_OK) {
        *qe = (io_mode == SPI_FLASH_QOUT);
    }
    return err;
}

IRAM_ATTR esp_err_t esp_flash_set_io_mode(esp_flash_t* chip, bool qe)
{
    esp_err_t err = rom_spiflash_api_funcs->chip_check(&chip);
    VERIFY_CHIP_OP(set_io_mode);

    chip->read_mode = (qe? SPI_FLASH_QOUT: SPI_FLASH_SLOWRD);
    err = rom_spiflash_api_funcs->start(chip);
    if (err != ESP_OK) {
        return err;
    }
    err = chip->chip_drv->set_io_mode(chip);
    return rom_spiflash_api_funcs->end(chip, err);
}
#endif //CONFIG_SPI_FLASH_ROM_IMPL

//init suspend mode cmd, uses internal.
esp_err_t esp_flash_suspend_cmd_init(esp_flash_t* chip)
{
    ESP_EARLY_LOGW(TAG, "Flash suspend feature is enabled");
    if (chip->chip_drv->get_chip_caps == NULL) {
        // chip caps get failed, pass the flash capability check.
        ESP_EARLY_LOGW(TAG, "get_chip_caps function pointer hasn't been initialized");
    } else {
        if ((chip->chip_drv->get_chip_caps(chip) & SPI_FLASH_CHIP_CAP_SUSPEND) == 0) {
            ESP_EARLY_LOGW(TAG, "Suspend and resume may not supported for this flash model yet.");
        }
    }
    return chip->chip_drv->sus_setup(chip);
}

esp_err_t esp_flash_app_disable_protect(bool disable)
{
    if (disable) {
        return esp_flash_app_disable_os_functions(esp_flash_default_chip);
    } else {
        return esp_flash_app_enable_os_functions(esp_flash_default_chip);
    }
}
