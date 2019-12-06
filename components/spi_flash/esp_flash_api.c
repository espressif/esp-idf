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

#include <stdlib.h>
#include <stdio.h>
#include <sys/param.h>
#include <string.h>

#include "spi_flash_chip_driver.h"
#include "memspi_host_driver.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "esp_heap_caps.h"
#include "esp_flash_internal.h"

static const char TAG[] = "spi_flash";

#define MAX_WRITE_CHUNK 8192 /* write in chunks */
#define MAX_READ_CHUNK 16384


#ifdef CONFIG_SPI_FLASH_DANGEROUS_WRITE_ABORTS
#define UNSAFE_WRITE_ADDRESS abort()
#else
#define UNSAFE_WRITE_ADDRESS return ESP_ERR_INVALID_ARG
#endif

/* CHECK_WRITE_ADDRESS macro to fail writes which land in the
   bootloader, partition table, or running application region.
*/
#if CONFIG_SPI_FLASH_DANGEROUS_WRITE_ALLOWED
#define CHECK_WRITE_ADDRESS(CHIP, ADDR, SIZE)
#else /* FAILS or ABORTS */
#define CHECK_WRITE_ADDRESS(CHIP, ADDR, SIZE) do {                            \
        if (CHIP && CHIP->os_func->region_protected && CHIP->os_func->region_protected(CHIP->os_func_data, ADDR, SIZE)) {                       \
            UNSAFE_WRITE_ADDRESS;                                 \
        }                                                               \
    } while(0)
#endif // CONFIG_SPI_FLASH_DANGEROUS_WRITE_ALLOWED

#define IO_STR_LEN  7

static const char io_mode_str[][IO_STR_LEN] = {
    "slowrd",
    "fastrd",
    "dout",
    "dio",
    "qout",
    "qio",
};

_Static_assert(sizeof(io_mode_str)/IO_STR_LEN == SPI_FLASH_READ_MODE_MAX, "the io_mode_str should be consistent with the esp_flash_io_mode_t defined in spi_flash_ll.h");

esp_err_t esp_flash_read_chip_id(esp_flash_t* chip, uint32_t* flash_id);

/* Static function to notify OS of a new SPI flash operation.

   If returns an error result, caller must abort. If returns ESP_OK, caller must
   call spiflash_end() before returning.
*/
static esp_err_t IRAM_ATTR spiflash_start(esp_flash_t *chip)
{
    if (chip->os_func != NULL && chip->os_func->start != NULL) {
        esp_err_t err = chip->os_func->start(chip->os_func_data);
        if (err != ESP_OK) {
            return err;
        }
    }
    chip->host->dev_config(chip->host);
    return ESP_OK;
}

/* Static function to notify OS that SPI flash operation is complete.
 */
static esp_err_t IRAM_ATTR  spiflash_end(const esp_flash_t *chip, esp_err_t err)
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

/* Return true if regions 'a' and 'b' overlap at all, based on their start offsets and lengths. */
inline static bool regions_overlap(uint32_t a_start, uint32_t a_len,uint32_t b_start, uint32_t b_len);

/* Top-level API functions, calling into chip_drv functions via chip->drv */

static esp_err_t detect_spi_flash_chip(esp_flash_t *chip);

bool esp_flash_chip_driver_initialized(const esp_flash_t *chip)
{
    if (!chip->chip_drv) return false;
    return true;
}

esp_err_t IRAM_ATTR esp_flash_init(esp_flash_t *chip)
{
    esp_err_t err = ESP_OK;
    if (chip == NULL || chip->host == NULL || chip->host->driver_data == NULL ||
        ((memspi_host_data_t*)chip->host->driver_data)->spi == NULL) {
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
    err = esp_flash_get_size(chip, &size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to get chip size");
        return err;
    }

    ESP_LOGI(TAG, "flash io: %s", io_mode_str[chip->read_mode]);
    err = spiflash_start(chip);
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
    return spiflash_end(chip, err);
}

//this is not public, but useful in unit tests
esp_err_t IRAM_ATTR esp_flash_read_chip_id(esp_flash_t* chip, uint32_t* flash_id)
{
    esp_err_t err = spiflash_start(chip);
    if (err != ESP_OK) {
        return err;
    }

    // Send generic RDID command twice, check for a matching result and retry in case we just powered on (inner
    // function fails if it sees all-ones or all-zeroes.)
    err = chip->host->read_id(chip->host, flash_id);

    if (err == ESP_OK) { // check we see the same ID twice, in case of transient power-on errors
        uint32_t new_id;
        err = chip->host->read_id(chip->host, &new_id);
        if (err == ESP_OK && (new_id != *flash_id)) {
            err = ESP_ERR_FLASH_NOT_INITIALISED;
        }
    }

    return spiflash_end(chip, err);
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
        ESP_LOGD(TAG, "trying chip: %s", chip->chip_drv->name);

        err = spiflash_start(chip);
        if (err != ESP_OK) {
            return err;
        }

        if (chip->chip_drv->probe(chip, flash_id) != ESP_OK) {
            chip->chip_drv = NULL;
        }
        // if probe succeeded, chip->drv stays set
        drivers++;

        err = spiflash_end(chip, err);
        if (err != ESP_OK) {
            return err;
        }
    }
    if (!esp_flash_chip_driver_initialized(chip)) {
        return ESP_ERR_NOT_FOUND;
    }
    ESP_LOGI(TAG, "detected chip: %s", chip->chip_drv->name);
    return ESP_OK;
}

// Convenience macro for beginning of all API functions,
// check that the 'chip' parameter is properly initialised
// and supports the operation in question
#define VERIFY_OP(OP) do {                                  \
        if (chip == NULL) {                                 \
            chip = esp_flash_default_chip;                  \
        }                                                   \
        if (chip == NULL || !esp_flash_chip_driver_initialized(chip)) { \
            return ESP_ERR_FLASH_NOT_INITIALISED;               \
        }                                                   \
        if (chip->chip_drv->OP == NULL) {                        \
            return ESP_ERR_FLASH_UNSUPPORTED_CHIP;              \
        }                                                   \
    } while (0)

esp_err_t IRAM_ATTR esp_flash_read_id(esp_flash_t *chip, uint32_t *out_id)
{
    if (chip == NULL) {
        chip = esp_flash_default_chip;
    }
    if (chip == NULL || !esp_flash_chip_driver_initialized(chip)) {
        return ESP_ERR_FLASH_NOT_INITIALISED;
    }
    if (out_id == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t err = spiflash_start(chip);
    if (err != ESP_OK) {
        return err;
    }

    err = chip->host->read_id(chip->host, out_id);

    return spiflash_end(chip, err);
}

esp_err_t IRAM_ATTR esp_flash_get_size(esp_flash_t *chip, uint32_t *out_size)
{
    VERIFY_OP(detect_size);
    if (out_size == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (chip->size != 0) {
        *out_size = chip->size;
        return ESP_OK;
    }

    esp_err_t err = spiflash_start(chip);
    if (err != ESP_OK) {
        return err;
    }
    uint32_t detect_size;
    err = chip->chip_drv->detect_size(chip, &detect_size);
    if (err == ESP_OK) {
        chip->size = detect_size;
    }
    return spiflash_end(chip, err);
}

esp_err_t IRAM_ATTR esp_flash_erase_chip(esp_flash_t *chip)
{
    VERIFY_OP(erase_chip);
    CHECK_WRITE_ADDRESS(chip, 0, chip->size);

    esp_err_t err = spiflash_start(chip);
    if (err != ESP_OK) {
        return err;
    }

    err = chip->chip_drv->erase_chip(chip);
    return spiflash_end(chip, err);
}

esp_err_t IRAM_ATTR esp_flash_erase_region(esp_flash_t *chip, uint32_t start, uint32_t len)
{
    VERIFY_OP(erase_sector);
    VERIFY_OP(erase_block);
    CHECK_WRITE_ADDRESS(chip, start, len);
    uint32_t block_erase_size = chip->chip_drv->erase_block == NULL ? 0 : chip->chip_drv->block_erase_size;
    uint32_t sector_size = chip->chip_drv->sector_size;

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

    esp_err_t err = ESP_OK;
    // Check for write protected regions overlapping the erase region
    if (chip->chip_drv->get_protected_regions != NULL &&
        chip->chip_drv->num_protectable_regions > 0) {

        err = spiflash_start(chip);
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
        err = spiflash_end(chip, err);
    }


    while (err == ESP_OK && len >= sector_size) {
        err = spiflash_start(chip);
        if (err != ESP_OK) {
            return err;
        }

        // If possible erase an entire multi-sector block
        if (block_erase_size > 0 && len >= block_erase_size && (start % block_erase_size) == 0) {
            err = chip->chip_drv->erase_block(chip, start);
            start += block_erase_size;
            len -= block_erase_size;
        }
        else {
            // Otherwise erase individual sector only
            err = chip->chip_drv->erase_sector(chip, start);
            start += sector_size;
            len -= sector_size;
        }

        err = spiflash_end(chip, err);
    }
    return err;
}

esp_err_t IRAM_ATTR esp_flash_get_chip_write_protect(esp_flash_t *chip, bool *out_write_protected)
{
    VERIFY_OP(get_chip_write_protect);
    if (out_write_protected == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = spiflash_start(chip);
    if (err != ESP_OK) {
        return err;
    }

    err = chip->chip_drv->get_chip_write_protect(chip, out_write_protected);

    return spiflash_end(chip, err);
}

esp_err_t IRAM_ATTR esp_flash_set_chip_write_protect(esp_flash_t *chip, bool write_protect)
{
    VERIFY_OP(set_chip_write_protect);
    //TODO: skip writing if already locked or unlocked

    esp_err_t err = spiflash_start(chip);
    if (err != ESP_OK) {
        return err;
    }

    err = chip->chip_drv->set_chip_write_protect(chip, write_protect);

    return spiflash_end(chip, err);
}

esp_err_t esp_flash_get_protectable_regions(const esp_flash_t *chip, const esp_flash_region_t **out_regions, uint32_t *out_num_regions)
{
    if(out_num_regions != NULL) {
        *out_num_regions = 0; // In case caller doesn't check result
    }
    VERIFY_OP(get_protected_regions);

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
    VERIFY_OP(get_protected_regions);

    if (out_protected == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t index;
    esp_err_t err = find_region(chip, region, &index);
    if (err != ESP_OK) {
        return err;
    }

    uint64_t protection_mask = 0;
    err = spiflash_start(chip);
    if (err != ESP_OK) {
        return err;
    }

    err = chip->chip_drv->get_protected_regions(chip, &protection_mask);
    if (err == ESP_OK) {
        *out_protected = protection_mask & (1LL << index);
    }

    return spiflash_end(chip, err);
}

esp_err_t IRAM_ATTR esp_flash_set_protected_region(esp_flash_t *chip, const esp_flash_region_t *region, bool protect)
{
    VERIFY_OP(set_protected_regions);

    uint8_t index;
    esp_err_t err = find_region(chip, region, &index);
    if (err != ESP_OK) {
        return err;
    }

    uint64_t protection_mask = 0;
    err = spiflash_start(chip);
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

    return spiflash_end(chip, err);
}

esp_err_t IRAM_ATTR esp_flash_read(esp_flash_t *chip, void *buffer, uint32_t address, uint32_t length)
{
    if (length == 0) {
        return ESP_OK;
    }
    VERIFY_OP(read);
    if (buffer == NULL || address > chip->size || address+length > chip->size) {
        return ESP_ERR_INVALID_ARG;
    }

    //when the cache is disabled, only the DRAM can be read, check whether we need to receive in another buffer in DRAM.
    bool direct_read = chip->host->supports_direct_read(chip->host, buffer);
    uint8_t* temp_buffer = NULL;

    if (!direct_read) {
        uint32_t length_to_allocate = MAX(MAX_READ_CHUNK, length);
        length_to_allocate = (length_to_allocate+3)&(~3);
        temp_buffer = heap_caps_malloc(length_to_allocate, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        ESP_LOGV(TAG, "allocate temp buffer: %p", temp_buffer);
        if (temp_buffer == NULL) return ESP_ERR_NO_MEM;
    }

    esp_err_t err = ESP_OK;

    do {
        err = spiflash_start(chip);
        if (err != ESP_OK) {
            break;
        }
        //if required (dma buffer allocated), read to the buffer instead of the original buffer
        uint8_t* buffer_to_read = (temp_buffer)? temp_buffer : buffer;
        //each time, we at most read this length
        //after that, we release the lock to allow some other operations
        uint32_t length_to_read = MIN(MAX_READ_CHUNK, length);

        if (err == ESP_OK) {
            err = chip->chip_drv->read(chip, buffer_to_read, address, length_to_read);
        }
        if (err != ESP_OK) {
            spiflash_end(chip, err);
            break;
        }
        //even if this is failed, the data is still valid, copy before quit
        err = spiflash_end(chip, err);

        //copy back to the original buffer
        if (temp_buffer) {
            memcpy(buffer, temp_buffer, length_to_read);
        }
        address += length_to_read;
        length -= length_to_read;
        buffer += length_to_read;
    } while (err == ESP_OK && length > 0);

    free(temp_buffer);
    return err;
}

esp_err_t IRAM_ATTR esp_flash_write(esp_flash_t *chip, const void *buffer, uint32_t address, uint32_t length)
{
    if (length == 0) {
        return ESP_OK;
    }
    VERIFY_OP(write);
    CHECK_WRITE_ADDRESS(chip, address, length);
    if (buffer == NULL || address > chip->size || address+length > chip->size) {
        return ESP_ERR_INVALID_ARG;
    }

    //when the cache is disabled, only the DRAM can be read, check whether we need to copy the data first
    bool direct_write = chip->host->supports_direct_write(chip->host, buffer);

    esp_err_t err = ESP_OK;
    /* Write output in chunks, either by buffering on stack or
       by artificially cutting into MAX_WRITE_CHUNK parts (in an OS
       environment, this prevents writing from causing interrupt or higher priority task
       starvation.) */
    do {
        uint32_t write_len;
        const void *write_buf;
        if (direct_write) {
            write_len = MIN(length, MAX_WRITE_CHUNK);
            write_buf = buffer;
        } else {
            uint32_t buf[8];
            write_len = MIN(length, sizeof(buf));
            memcpy(buf, buffer, write_len);
            write_buf = buf;
        }

        err = spiflash_start(chip);
        if (err != ESP_OK) {
            return err;
        }

        err = chip->chip_drv->write(chip, write_buf, address, write_len);

        address += write_len;
        buffer = (void *)((intptr_t)buffer + write_len);
        length -= write_len;

        err = spiflash_end(chip, err);
    } while (err == ESP_OK && length > 0);
    return err;
}

//currently the legacy implementation is used, from flash_ops.c
esp_err_t spi_flash_write_encrypted(size_t dest_addr, const void *src, size_t size);

esp_err_t IRAM_ATTR esp_flash_write_encrypted(esp_flash_t *chip, uint32_t address, const void *buffer, uint32_t length)
{
    /*
     * Since currently this feature is supported only by the hardware, there
     * is no way to support non-standard chips. We use the legacy
     * implementation and skip the chip and driver layers.
     */
    if (chip == NULL) {
        chip = esp_flash_default_chip;
    } else if (chip != esp_flash_default_chip) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    if (buffer == NULL || address > chip->size || address+length > chip->size) {
        return ESP_ERR_INVALID_ARG;
    }
    return spi_flash_write_encrypted(address, buffer, length);
}

inline static IRAM_ATTR bool regions_overlap(uint32_t a_start, uint32_t a_len,uint32_t b_start, uint32_t b_len)
{
    uint32_t a_end = a_start + a_len;
    uint32_t b_end = b_start + b_len;
    return (a_end > b_start && b_end > a_start);
}

//currently the legacy implementation is used, from flash_ops.c
esp_err_t spi_flash_read_encrypted(size_t src, void *dstv, size_t size);

esp_err_t IRAM_ATTR esp_flash_read_encrypted(esp_flash_t *chip, uint32_t address, void *out_buffer, uint32_t length)
{
    /*
     * Since currently this feature is supported only by the hardware, there
     * is no way to support non-standard chips. We use the legacy
     * implementation and skip the chip and driver layers.
     */
    if (chip == NULL) {
        chip = esp_flash_default_chip;
    } else if (chip != esp_flash_default_chip) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    return spi_flash_read_encrypted(address, out_buffer, length);
}

// test only, non-public
IRAM_ATTR esp_err_t esp_flash_get_io_mode(esp_flash_t* chip, bool* qe)
{
    VERIFY_OP(get_io_mode);
    esp_flash_io_mode_t io_mode;

    esp_err_t err = spiflash_start(chip);
    if (err != ESP_OK) {
        return err;
    }
    err = chip->chip_drv->get_io_mode(chip, &io_mode);
    err = spiflash_end(chip, err);
    if (err == ESP_OK) {
        *qe = (io_mode == SPI_FLASH_QOUT);
    }
    return err;
}

IRAM_ATTR esp_err_t esp_flash_set_io_mode(esp_flash_t* chip, bool qe)
{
    VERIFY_OP(set_io_mode);
    chip->read_mode = (qe? SPI_FLASH_QOUT: SPI_FLASH_SLOWRD);
    esp_err_t err = spiflash_start(chip);
    if (err != ESP_OK) {
        return err;
    }
    err = chip->chip_drv->set_io_mode(chip);
    return spiflash_end(chip, err);
}

#ifndef CONFIG_SPI_FLASH_USE_LEGACY_IMPL
esp_err_t esp_flash_app_disable_protect(bool disable)
{
    if (disable) {
        return esp_flash_app_disable_os_functions(esp_flash_default_chip);
    } else {
        return esp_flash_app_init_os_functions(esp_flash_default_chip);
    }
}
#endif

/*------------------------------------------------------------------------------
    Adapter layer to original api before IDF v4.0
------------------------------------------------------------------------------*/

#ifndef CONFIG_SPI_FLASH_USE_LEGACY_IMPL

static IRAM_ATTR esp_err_t spi_flash_translate_rc(esp_err_t err)
{
    switch (err) {
        case ESP_OK:
            return ESP_OK;
        case ESP_ERR_INVALID_ARG:
            return ESP_ERR_INVALID_ARG;
        case ESP_ERR_FLASH_NOT_INITIALISED:
        case ESP_ERR_FLASH_PROTECTED:
            return ESP_ERR_INVALID_STATE;
        case ESP_ERR_NOT_FOUND:
        case ESP_ERR_FLASH_UNSUPPORTED_HOST:
        case ESP_ERR_FLASH_UNSUPPORTED_CHIP:
            return ESP_ERR_NOT_SUPPORTED;
        case ESP_ERR_FLASH_NO_RESPONSE:
            return ESP_ERR_INVALID_RESPONSE;
        default:
            ESP_EARLY_LOGE(TAG, "unexpected spi flash error code: %x", err);
            abort();
    }
    return ESP_OK;
}

esp_err_t IRAM_ATTR spi_flash_erase_range(uint32_t start_addr, uint32_t size)
{
    esp_err_t err = esp_flash_erase_region(NULL, start_addr, size);
    return spi_flash_translate_rc(err);
}

esp_err_t IRAM_ATTR spi_flash_write(size_t dst, const void *srcv, size_t size)
{
    esp_err_t err = esp_flash_write(NULL, srcv, dst, size);
    return spi_flash_translate_rc(err);
}

esp_err_t IRAM_ATTR spi_flash_read(size_t src, void *dstv, size_t size)
{
    esp_err_t err = esp_flash_read(NULL, dstv, src, size);
    return spi_flash_translate_rc(err);
}

#endif // CONFIG_SPI_FLASH_USE_LEGACY_IMPL
