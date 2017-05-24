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
#include <stdlib.h>
#include <stdio.h>
#include <sys/param.h>
#include <string.h>

#include "spi_flash_lowlevel_driver.h"
#include "spi_flash_lowlevel_generic.h"
#include "soc/spi_reg.h"

#define MAX_WRITE_CHUNK 8192 /* write in chunks */

/* Static function to notify OS of a new SPI flash operation.

   If returns an error result, caller must abort. If returns FLASH_OK, caller must
   call spiflash_end() before returning.
*/
static esp_flash_err_t spiflash_start(const esp_flash_chip_t *chip)
{
    if (esp_flash_os_functions != NULL
        && esp_flash_os_functions->start != NULL) {
        esp_flash_err_t err = esp_flash_os_functions->start(chip);
        if (err != FLASH_OK) {
            return err;
        }
    }
    return FLASH_OK;
}

/* Static function to notify OS that SPI flash operation is complete.
 */
static esp_flash_err_t spiflash_end(const esp_flash_chip_t *chip, esp_flash_err_t err)
{
    if (esp_flash_os_functions != NULL
        && esp_flash_os_functions->end != NULL) {
        esp_flash_err_t end_err = esp_flash_os_functions->end(chip);
        if (err == FLASH_OK) {
            err = end_err; // Only return the 'end' error if we haven't already failed
        }
    }
    return err;
}

/* Return true if regions 'a' and 'b' overlap at all, based on their start offsets and lengths. */
inline static bool regions_overlap(uint32_t a_start, uint32_t a_len,uint32_t b_start, uint32_t b_len);

/* Top-level API functions, calling into driver functions via chip->drv */

static esp_flash_err_t detect_spi_flash_chip(esp_flash_chip_t *chip);

esp_flash_err_t esp_flash_init(esp_flash_chip_t *chip)
{
    if (chip->spi == NULL) {
        return FLASH_ERR_INVALID_ARG;
    }

    // TODO: configure SPI host clock speed, pin configuration

    if (chip->drv == NULL) {
        // Detect driver
        esp_flash_err_t err = detect_spi_flash_chip(chip);
        if (err != FLASH_OK) {
            return err;
        }
    }

    esp_flash_err_t err = spiflash_start(chip);
    if (err != FLASH_OK) {
        return err;
    }

    if (chip->size == 0) {
         // Detect flash size
        err = chip->drv->detect_size(chip, &chip->size);
    }

    if (err == FLASH_OK) {
        // Try to set the flash mode to whatever default mode was chosen
        // (this isn't necessary at this point for functionality, but init will fail
        // if this mode can't be set on this chip.)
        err = chip->drv->set_read_mode(chip);
    }

    // Done: all fields on 'chip' are initialised
    return spiflash_end(chip, err);
}

static esp_flash_err_t detect_spi_flash_chip(esp_flash_chip_t *chip)
{
    esp_flash_err_t err;
    uint32_t flash_id;
    int retries = 10;
    do {
        err = spiflash_start(chip);
        if (err != FLASH_OK) {
            return err;
        }

        // Send generic RDID command twice, check for a matching result and retry in case we just powered on (inner
        // function fails if it sees all-ones or all-zeroes.)
        err = spi_flash_generic_read_id(chip, &flash_id);

        if (err == FLASH_OK) { // check we see the same ID twice, in case of transient power-on errors
            uint32_t new_id;
            err = spi_flash_generic_read_id(chip, &new_id);
            if (err == FLASH_OK && (new_id != flash_id)) {
                err = FLASH_ERR_NOT_INITIALISED;
            }
        }

        err = spiflash_end(chip, err);
    } while (err != FLASH_OK && retries-- > 0);

    // Detect the chip and set the driver structure for it
    const esp_flash_driver_t **drivers = esp_flash_registered_flash_drivers;
    while (*drivers != NULL && chip->drv == NULL) {
        chip->drv = *drivers;

        // start/end SPI operation each time, for multitasking
        // and also so esp_flash_registered_flash_drivers can live in flash
        err = spiflash_start(chip);
        if (err != FLASH_OK) {
            return err;
        }

        if (chip->drv->probe(chip, flash_id) != FLASH_OK) {
            chip->drv = NULL;
        }
        // if probe succeeded, chip->drv stays set
        drivers++;

        err = spiflash_end(chip, err);
        if (err != FLASH_OK) {
            return err;
        }
    }

    return (chip->drv == NULL) ? FLASH_ERR_NOT_FOUND : FLASH_OK;
}

// Convenience macro for beginning of all API functions,
// check that the 'chip' parameter is properly initialised
// and supports the operation in question
#define VERIFY_OP(OP) do {                                  \
        if (chip == NULL) {                                 \
            chip = esp_flash_default_chip;                  \
        }                                                   \
        if (chip == NULL || chip->drv == NULL) {            \
            return FLASH_ERR_NOT_INITIALISED;               \
        }                                                   \
        if (chip->drv->OP == NULL) {                        \
            return FLASH_ERR_UNSUPPORTED_CHIP;              \
        }                                                   \
    } while (0)

esp_flash_err_t esp_flash_read_id(const esp_flash_chip_t *chip, uint32_t *id)
{
    printf("chip %p esp_flash_default_chip %p\n",
           chip, esp_flash_default_chip);
    VERIFY_OP(read_id);
    if (id == NULL) {
        return FLASH_ERR_INVALID_ARG;
    }
    esp_flash_err_t err = spiflash_start(chip);
    if (err != FLASH_OK) {
        return err;
    }

    err = chip->drv->read_id(chip, id);

    return spiflash_end(chip, err);
}

esp_flash_err_t esp_flash_detect_size(const esp_flash_chip_t *chip, uint32_t *size)
{
    VERIFY_OP(detect_size);
    if (size == NULL) {
        return FLASH_ERR_INVALID_ARG;
    }
    *size = 0;

    esp_flash_err_t err = spiflash_start(chip);
    if (err != FLASH_OK) {
        return err;
    }

    err = chip->drv->detect_size(chip, size);

    return spiflash_end(chip, err);
}

esp_flash_err_t esp_flash_erase_chip(const esp_flash_chip_t *chip)
{
    VERIFY_OP(erase_chip);
    bool write_protect = false;

    esp_flash_err_t err = spiflash_start(chip);
    if (err != FLASH_OK) {
        return err;
    }

    err  = esp_flash_get_chip_write_protect(chip, &write_protect);

    if (err == FLASH_OK && write_protect) {
        err = FLASH_ERR_PROTECTED;
    }

    if (err == FLASH_OK) {
        err = chip->drv->erase_chip(chip);
    }

    return spiflash_end(chip, err);
}

esp_flash_err_t esp_flash_erase_region(const esp_flash_chip_t *chip, uint32_t start, uint32_t len)
{
    VERIFY_OP(erase_sector);
    uint32_t block_erase_size = chip->drv->erase_block == NULL ? 0 : chip->drv->block_erase_size;
    uint32_t sector_size = chip->drv->sector_size;
    bool write_protect = false;

    if (sector_size == 0 || (block_erase_size % sector_size) != 0) {
        return FLASH_ERR_NOT_INITIALISED;
    }
    if (start > chip->size || start + len > chip->size) {
        return FLASH_ERR_INVALID_ARG;
    }
    if ((start % chip->drv->sector_size) != 0 || (len % chip->drv->sector_size) != 0) {
        // Can only erase multiples of the sector size, starting at sector boundary
        return FLASH_ERR_INVALID_ARG;
    }

    esp_flash_err_t err = spiflash_start(chip);
    if (err != FLASH_OK) {
        return err;
    }

    // Check for write protection on whole chip
    if (chip->drv->get_chip_write_protect != NULL) {
        err = chip->drv->get_chip_write_protect(chip, &write_protect);
        if (err == FLASH_OK && write_protect) {
            err = FLASH_ERR_PROTECTED;
        }
    }

    // Check for write protected regions overlapping the erase region
    if (err == FLASH_OK && chip->drv->get_protected_regions != NULL && chip->drv->num_protectable_regions > 0) {
        uint64_t protected = 0;
        err = chip->drv->get_protected_regions(chip, &protected);
        if (protected != 0) {
            for (int i = 0; i < chip->drv->num_protectable_regions && err == FLASH_OK; i++) {
                const esp_flash_region_t *region = &chip->drv->protectable_regions[i];
                if ((protected & (1LL << i))
                    && regions_overlap(start, len, region->offset, region->size)) {
                    err = FLASH_ERR_PROTECTED;
                }
            }
        }
    }

    // Don't lock the SPI flash for the entire erase, as this may be very long
    err = spiflash_end(chip, err);

    while (err == FLASH_OK && len >= sector_size) {
        esp_flash_err_t err = spiflash_start(chip);
        if (err != FLASH_OK) {
            return err;
        }

        // If possible erase an entire multi-sector block
        if (block_erase_size > 0 && len >= block_erase_size && (start % block_erase_size) == 0) {
            err = chip->drv->erase_block(chip, start);
            start += block_erase_size;
            len -= block_erase_size;
        }
        else {
            // Otherwise erase individual sector only
            err = chip->drv->erase_sector(chip, start);
            start += sector_size;
            len -= sector_size;
        }

        err = spiflash_end(chip, err);
    }

    return err;
}

esp_flash_err_t esp_flash_get_chip_write_protect(const esp_flash_chip_t *chip, bool *write_protected)
{
    VERIFY_OP(get_chip_write_protect);
    if (write_protected == NULL) {
        return FLASH_ERR_INVALID_ARG;
    }

    esp_flash_err_t err = spiflash_start(chip);
    if (err != FLASH_OK) {
        return err;
    }

    err = chip->drv->get_chip_write_protect(chip, write_protected);

    return spiflash_end(chip, err);
}

esp_flash_err_t esp_flash_set_chip_write_protect(const esp_flash_chip_t *chip, bool write_protect_chip)
{
    VERIFY_OP(set_chip_write_protect);

    esp_flash_err_t err = spiflash_start(chip);
    if (err != FLASH_OK) {
        return err;
    }

    err = chip->drv->set_chip_write_protect(chip, write_protect_chip);

    return spiflash_end(chip, err);
}

esp_flash_err_t esp_flash_get_protectable_regions(const esp_flash_chip_t *chip, const esp_flash_region_t **regions, uint32_t *num_regions)
{
    if(num_regions != NULL) {
        *num_regions = 0; // In case caller doesn't check result
    }
    VERIFY_OP(get_protected_regions);

    if(regions == NULL || num_regions == NULL) {
        return FLASH_ERR_INVALID_ARG;
    }

    *num_regions = chip->drv->num_protectable_regions;
    *regions = chip->drv->protectable_regions;
    return FLASH_OK;
}

static esp_flash_err_t find_region(const esp_flash_chip_t *chip, const esp_flash_region_t *region, uint8_t *index)
{
   if (region == NULL) {
        return FLASH_ERR_INVALID_ARG;
    }

   for(*index = 0; *index < chip->drv->num_protectable_regions; (*index)++) {
       if (memcmp(&chip->drv->protectable_regions[*index],
                  region, sizeof(esp_flash_region_t)) == 0) {
           return FLASH_OK;
       }
   }

   return FLASH_ERR_NOT_FOUND;
}

esp_flash_err_t esp_flash_get_protected_region(const esp_flash_chip_t *chip, const esp_flash_region_t *region, bool *protected)
{
    VERIFY_OP(get_protected_regions);

    if (protected == NULL) {
        return FLASH_ERR_INVALID_ARG;
    }

    uint8_t index;
    esp_flash_err_t err = find_region(chip, region, &index);
    if (err != FLASH_OK) {
        return err;
    }

    uint64_t protection_mask = 0;
    err = spiflash_start(chip);
    if (err != FLASH_OK) {
        return err;
    }

    err = chip->drv->get_protected_regions(chip, &protection_mask);
    if (err == FLASH_OK) {
        *protected = protection_mask & (1LL << index);
    }

    return spiflash_end(chip, err);
}

esp_flash_err_t esp_flash_set_protected_region(const esp_flash_chip_t *chip, const esp_flash_region_t *region, bool protected)
{
    VERIFY_OP(set_protected_regions);

    uint8_t index;
    esp_flash_err_t err = find_region(chip, region, &index);
    if (err != FLASH_OK) {
        return err;
    }

    uint64_t protection_mask = 0;
    err = spiflash_start(chip);
    if (err != FLASH_OK) {
        return err;
    }

    err = chip->drv->get_protected_regions(chip, &protection_mask);
    if (err == FLASH_OK) {
        if (protected) {
            protection_mask |= (1LL << index);
        } else {
            protection_mask &= ~(1LL << index);
        }
        err = chip->drv->set_protected_regions(chip, protection_mask);
    }

    return spiflash_end(chip, err);
}

esp_flash_err_t esp_flash_read(const esp_flash_chip_t *chip, void *buffer, uint32_t address, uint32_t length)
{
    VERIFY_OP(read);
    if (buffer == NULL || address > chip->size || address+length > chip->size) {
        return FLASH_ERR_INVALID_ARG;
    }

    esp_flash_err_t err = spiflash_start(chip);
    if (err != FLASH_OK) {
        return err;
    }

    if (err == FLASH_OK) {
        err = chip->drv->set_read_mode(chip);
    }

    if (err == FLASH_OK) {
        err = chip->drv->read(chip, buffer, address, length);
    }

    return spiflash_end(chip, err);
}

esp_flash_err_t esp_flash_write(const esp_flash_chip_t *chip, uint32_t address, const void *buffer, uint32_t length)
{
    VERIFY_OP(write);
    if (buffer == NULL || address > chip->size || address+length > chip->size) {
        return FLASH_ERR_INVALID_ARG;
    }

    /* If 'chip' is connected to the main SPI bus, we can only write directly from regions that are accessible
       with cache disabled. */
#ifdef ESP_PLATFORM
    bool direct_write = ( chip->spi != &SPI1
                          || ( (uintptr_t) address >= 0x3FFAE000
                               && (uintptr_t) address < 0x40000000 ) );
#else
    bool direct_write = true;
#endif

    esp_flash_err_t err = FLASH_OK;

    /* Write output in chunks, either by buffering on stack or
       by artificially cutting into MAX_WRITE_CHUNK parts (in an OS
       environment, this prevents writing from causing interrupt or higher priority task
       starvation.) */
    while(err == FLASH_OK && length > 0) {
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
        if (err != FLASH_OK) {
            return err;
        }

        err = chip->drv->write(chip, address, write_buf, write_len);

        address += write_len;
        buffer = (void *)((intptr_t)buffer + write_len);
        length -= write_len;

        err = spiflash_end(chip, err);
    }
    return err;
}

esp_flash_err_t esp_flash_write_encrypted(const esp_flash_chip_t *chip, uint32_t address, const void *buffer, uint32_t length)
{
    VERIFY_OP(write_encrypted);
    if (chip->spi != 0) {
        // Encrypted operations have to use SPI0
        return FLASH_ERR_UNSUPPORTED_HOST;
    }
    if (buffer == NULL || address > chip->size || address+length > chip->size) {
        return FLASH_ERR_INVALID_ARG;
    }

    esp_flash_err_t err = spiflash_start(chip);
    if (err != FLASH_OK) {
        return err;
    }

    err = chip->drv->write_encrypted(chip, address, buffer, length);

    return spiflash_end(chip, err);
}


inline static bool regions_overlap(uint32_t a_start, uint32_t a_len,uint32_t b_start, uint32_t b_len)
{
    uint32_t a_end = a_start + a_len;
    uint32_t b_end = b_start + b_len;

    return ((a_start >= b_start && a_start <= b_end)
            || (a_end >= b_start && a_end <= b_end)
            || (b_start >= a_start && b_start <= a_end)
            || (b_end >= a_start && b_end <= a_end));
}

const esp_flash_chip_t *esp_flash_default_chip;

static esp_flash_chip_t default_chip;

esp_flash_err_t esp_flash_init_default_chip()
{
    default_chip.spi = &SPI1;
    default_chip.read_mode = ESP_FLASH_FASTRD; // TODO: initialise properly
    default_chip.speed = ESP_FLASH_20MHZ; // TODO: initialise properly

    // ROM TODO: account for non-standard default pins in efuse

    // ROM TODO: to account for chips which are slow to power on, maybe keep probing in a loop here

    esp_flash_err_t err = esp_flash_init(&default_chip);
    if (err != FLASH_OK) {
        return err;
    }

    esp_flash_default_chip = &default_chip;
    return FLASH_OK;
}

const esp_flash_os_functions_t *esp_flash_os_functions = &esp_flash_noos_functions;
