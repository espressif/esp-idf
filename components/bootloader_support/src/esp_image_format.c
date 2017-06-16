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

#include <rom/rtc.h>
#include <soc/cpu.h>
#include <esp_image_format.h>
#include <esp_log.h>
#include <bootloader_flash.h>

static const char *TAG = "esp_image";

#define SIXTEEN_MB 0x1000000
#define ESP_ROM_CHECKSUM_INITIAL 0xEF

/* Headroom to ensure between stack SP (at time of checking) and data loaded from flash */
#define STACK_LOAD_HEADROOM 4096

/* Return true if load_addr is an address the bootloader should load into */
static bool should_load(uint32_t load_addr);
/* Return true if load_addr is an address the bootloader should map via flash cache */
static bool should_map(uint32_t load_addr);

/* Load or verify a segment */
static esp_err_t process_segment(int index, uint32_t flash_addr, esp_image_segment_header_t *header, bool silent, bool do_load, uint32_t *checksum);

/* Verify the main image header */
static esp_err_t verify_image_header(uint32_t src_addr, const esp_image_header_t *image, bool silent);

/* Verify a segment header */
static esp_err_t verify_segment_header(int index, const esp_image_segment_header_t *segment, uint32_t segment_data_offs, bool silent);

/* Log-and-fail macro for use in esp_image_load */
#define FAIL_LOAD(...) do {                         \
        if (!silent) {                              \
            ESP_LOGE(TAG, __VA_ARGS__);             \
        }                                           \
        goto err;                                   \
    }                                               \
    while(0)

esp_err_t esp_image_load(esp_image_load_mode_t mode, const esp_partition_pos_t *part, esp_image_metadata_t *data)
{
#ifdef BOOTLOADER_BUILD
    bool do_load = (mode == ESP_IMAGE_LOAD);
#else
    bool do_load = false; // Can't load the image in app mode
#endif
    bool silent = (mode == ESP_IMAGE_VERIFY_SILENT);
    esp_err_t err = ESP_OK;
    // checksum the image a word at a time. This shaves 30-40ms per MB of image size
    uint32_t checksum_word = ESP_ROM_CHECKSUM_INITIAL;

    if (data == NULL || part == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (part->size > SIXTEEN_MB) {
        err = ESP_ERR_INVALID_ARG;
        FAIL_LOAD("partition size %d invalid, larger than 16MB", part->size);
    }

    bzero(data, sizeof(esp_image_metadata_t));
    data->start_addr = part->offset;

    ESP_LOGD(TAG, "reading image header @ 0x%x", data->start_addr);
    err = bootloader_flash_read(data->start_addr, &data->image, sizeof(esp_image_header_t), true);
    if (err != ESP_OK) {
        goto err;
    }

    ESP_LOGD(TAG, "image header: 0x%02x 0x%02x 0x%02x 0x%02x %08x",
             data->image.magic,
             data->image.segment_count,
             data->image.spi_mode,
             data->image.spi_size,
             data->image.entry_addr);

    err = verify_image_header(data->start_addr, &data->image, silent);
    if (err != ESP_OK) {
goto err;
    }

    if (data->image.segment_count > ESP_IMAGE_MAX_SEGMENTS) {
        FAIL_LOAD("image at 0x%x segment count %d exceeds max %d",
                  data->start_addr, data->image.segment_count, ESP_IMAGE_MAX_SEGMENTS);
    }

    uint32_t next_addr = data->start_addr + sizeof(esp_image_header_t);
    for(int i = 0; i < data->image.segment_count && err == ESP_OK; i++) {
        esp_image_segment_header_t *header = &data->segments[i];
        ESP_LOGV(TAG, "loading segment header %d at offset 0x%x", i, next_addr);
        err = process_segment(i, next_addr, header, silent, do_load, &checksum_word);
        if (err != ESP_OK) {
            goto err;
        }
        next_addr += sizeof(esp_image_segment_header_t);
        data->segment_data[i] = next_addr;
        next_addr += header->data_len;
    }

    // Segments all loaded, verify length
    uint32_t end_addr = next_addr;
    if (end_addr < data->start_addr) {
        FAIL_LOAD("image offset has wrapped");
    }

    uint32_t length = end_addr - data->start_addr;
    length = length + 1; // Add a byte for the checksum
    length = (length + 15) & ~15; // Pad to next full 16 byte block
    if (length > part->size) {
        FAIL_LOAD("Image length %d doesn't fit in partition length %d", length, part->size);
    }

    // Verify checksum
    uint32_t buf[16/sizeof(uint32_t)];
    err = bootloader_flash_read(data->start_addr + length - 16, buf, 16, true);
    uint8_t calc = ((uint8_t *)buf)[15];
    uint8_t checksum = (checksum_word >> 24)
        ^ (checksum_word >> 16)
        ^ (checksum_word >> 8)
        ^ (checksum_word >> 0);
    if (err != ESP_OK || checksum != calc) {
        FAIL_LOAD("Checksum failed. Calculated 0x%x read 0x%x",
                  checksum, calc);
    }

    data->image_length = length;

    // Success!
    return ESP_OK;

 err:
    if (err == ESP_OK) {
      err = ESP_ERR_IMAGE_INVALID;
    }
    // Prevent invalid/incomplete data leaking out
    bzero(data, sizeof(esp_image_metadata_t));
    return err;
}

static esp_err_t verify_image_header(uint32_t src_addr, const esp_image_header_t *image, bool silent)
{
    esp_err_t err = ESP_OK;

    if (image->magic != ESP_IMAGE_HEADER_MAGIC) {
        if (!silent) {
            ESP_LOGE(TAG, "image at 0x%x has invalid magic byte", src_addr);
        }
        err = ESP_ERR_IMAGE_INVALID;
    }
    if (!silent) {
        if (image->spi_mode > ESP_IMAGE_SPI_MODE_SLOW_READ) {
            ESP_LOGW(TAG, "image at 0x%x has invalid SPI mode %d", src_addr, image->spi_mode);
        }
        if (image->spi_speed > ESP_IMAGE_SPI_SPEED_80M) {
            ESP_LOGW(TAG, "image at 0x%x has invalid SPI speed %d", src_addr, image->spi_speed);
        }
        if (image->spi_size > ESP_IMAGE_FLASH_SIZE_MAX) {
            ESP_LOGW(TAG, "image at 0x%x has invalid SPI size %d", src_addr, image->spi_size);
        }
    }
    return err;
}

static esp_err_t process_segment(int index, uint32_t flash_addr, esp_image_segment_header_t *header, bool silent, bool do_load, uint32_t *checksum)
{
    esp_err_t err;

    /* read segment header */
    err = bootloader_flash_read(flash_addr, header, sizeof(esp_image_segment_header_t), true);
    if (err != ESP_OK) {
        return err;
    }

    intptr_t load_addr = header->load_addr;
    uint32_t data_len = header->data_len;
    uint32_t data_addr = flash_addr + sizeof(esp_image_segment_header_t);

    ESP_LOGV(TAG, "segment data length 0x%x data starts 0x%x", data_len, data_addr);

    err = verify_segment_header(index, header, data_addr, silent);
    if (err != ESP_OK) {
        return err;
    }

    if (data_len % 4 != 0) {
        FAIL_LOAD("unaligned segment length 0x%x", data_len);
    }

    bool is_mapping = should_map(load_addr);
    do_load = do_load && should_load(load_addr);

    if (!silent) {
        ESP_LOGI(TAG, "segment %d: paddr=0x%08x vaddr=0x%08x size=0x%05x (%6d) %s",
                 index, data_addr, load_addr,
                 data_len, data_len,
                 (do_load)?"load":(is_mapping)?"map":"");
    }

    if (do_load) {
        /* Before loading segment, check it doesn't clobber bootloader RAM... */
        uint32_t end_addr = load_addr + data_len;
        if (end_addr < 0x40000000) {
            intptr_t sp = (intptr_t)get_sp();
            if (end_addr > sp - STACK_LOAD_HEADROOM) {
                ESP_LOGE(TAG, "Segment %d end address 0x%08x too high (bootloader stack 0x%08x liimit 0x%08x)",
                         index, end_addr, sp, sp - STACK_LOAD_HEADROOM);
                return ESP_ERR_IMAGE_INVALID;
            }
        }
    }

    const void *data = bootloader_mmap(data_addr, data_len);
    if(!data) {
        ESP_LOGE(TAG, "bootloader_mmap(0x%x, 0x%x) failed",
                 data_addr, data_len);
        return ESP_FAIL;
    }
    const uint32_t *checksum_from;
    if (do_load) {
        memcpy((void *)load_addr, data, data_len);
        checksum_from = (const uint32_t *)load_addr;
    } else {
        checksum_from = (const uint32_t *)data;
    }
    // Update checksum, either from RAM we just loaded or from flash
    for (const uint32_t *c = checksum_from;
         c < checksum_from + (data_len/sizeof(uint32_t));
         c++) {
        *checksum ^= *c;
    }

    bootloader_munmap(data);

    return ESP_OK;

 err:
    if (err == ESP_OK) {
        err = ESP_ERR_IMAGE_INVALID;
    }
    return err;
}

static esp_err_t verify_segment_header(int index, const esp_image_segment_header_t *segment, uint32_t segment_data_offs, bool silent)
{
    if ((segment->data_len & 3) != 0
        || segment->data_len >= SIXTEEN_MB) {
        if (!silent) {
            ESP_LOGE(TAG, "invalid segment length 0x%x", segment->data_len);
        }
        return ESP_ERR_IMAGE_INVALID;
    }

    uint32_t load_addr = segment->load_addr;

    /* Check that flash cache mapped segment aligns correctly from flash to its mapped address,
       relative to the 64KB page mapping size.
    */
    ESP_LOGV(TAG, "segment %d map_segment %d segment_data_offs 0x%x load_addr 0x%x",
             index, map_segment, segment_data_offs, load_addr);
    if (should_map(load_addr)
        && ((segment_data_offs % SPI_FLASH_MMU_PAGE_SIZE) != (load_addr % SPI_FLASH_MMU_PAGE_SIZE))) {
        if (!silent) {
            ESP_LOGE(TAG, "Segment %d has load address 0x%08x, doesn't match segment data at 0x%08x",
                     index, load_addr, segment_data_offs);
        }
        return ESP_ERR_IMAGE_INVALID;
    }

    return ESP_OK;
}

static bool should_map(uint32_t load_addr)
{
    return (load_addr >= SOC_IROM_LOW && load_addr < SOC_IROM_HIGH)
        || (load_addr >= SOC_DROM_LOW && load_addr < SOC_DROM_HIGH);
}

static bool should_load(uint32_t load_addr)
{
    /* Reload the RTC memory segments whenever a non-deepsleep reset
       is occurring */
    bool load_rtc_memory = rtc_get_reset_reason(0) != DEEPSLEEP_RESET;

    if (should_map(load_addr)) {
        return false;
    }

    if (load_addr < 0x10000000) {
        // Reserved for non-loaded addresses.
        // Current reserved values are
        // 0x0 (padding block)
        // 0x4 (unused, but reserved for an MD5 block)
        return false;
    }

    if (!load_rtc_memory) {
        if (load_addr >= SOC_RTC_IRAM_LOW && load_addr < SOC_RTC_IRAM_HIGH) {
            ESP_LOGD(TAG, "Skipping RTC code segment at 0x%08x\n", load_addr);
            return false;
        }
        if (load_addr >= SOC_RTC_DATA_LOW && load_addr < SOC_RTC_DATA_HIGH) {
            ESP_LOGD(TAG, "Skipping RTC data segment at 0x%08x\n", load_addr);
            return false;
        }
    }

    return true;
}

esp_err_t esp_image_verify_bootloader(uint32_t *length)
{
    esp_image_metadata_t data;
    const esp_partition_pos_t bootloader_part = {
        .offset = ESP_BOOTLOADER_OFFSET,
        .size = ESP_PARTITION_TABLE_OFFSET - ESP_BOOTLOADER_OFFSET,
    };
    esp_err_t err = esp_image_load(ESP_IMAGE_VERIFY,
                                   &bootloader_part,
                                   &data);
    if (length != NULL) {
        *length = (err == ESP_OK) ? data.image_length : 0;
    }
    return err;
}
