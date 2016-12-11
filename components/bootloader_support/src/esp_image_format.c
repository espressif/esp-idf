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

#include <esp_image_format.h>
#include <esp_log.h>
#include <bootloader_flash.h>

static const char *TAG = "esp_image";

#define SIXTEEN_MB 0x1000000
#define ESP_ROM_CHECKSUM_INITIAL 0xEF

esp_err_t esp_image_load_header(uint32_t src_addr, bool log_errors, esp_image_header_t *image_header)
{
    esp_err_t err;
    ESP_LOGD(TAG, "reading image header @ 0x%x", src_addr);

    err = bootloader_flash_read(src_addr, image_header, sizeof(esp_image_header_t), true);

    if (err == ESP_OK) {
        if (image_header->magic != ESP_IMAGE_HEADER_MAGIC) {
            if (log_errors) {
                ESP_LOGE(TAG, "image at 0x%x has invalid magic byte", src_addr);
            }
            err = ESP_ERR_IMAGE_INVALID;
        }
        if (log_errors) {
            if (image_header->spi_mode > ESP_IMAGE_SPI_MODE_SLOW_READ) {
                ESP_LOGW(TAG, "image at 0x%x has invalid SPI mode %d", src_addr, image_header->spi_mode);
            }
            if (image_header->spi_speed > ESP_IMAGE_SPI_SPEED_80M) {
                ESP_LOGW(TAG, "image at 0x%x has invalid SPI speed %d", src_addr, image_header->spi_speed);
            }
            if (image_header->spi_size > ESP_IMAGE_FLASH_SIZE_MAX) {
                ESP_LOGW(TAG, "image at 0x%x has invalid SPI size %d", src_addr, image_header->spi_size);
            }
        }
    }

    if (err != ESP_OK) {
        bzero(image_header, sizeof(esp_image_header_t));
    }
    return err;
}

esp_err_t esp_image_load_segment_header(uint8_t index, uint32_t src_addr, const esp_image_header_t *image_header, bool log_errors, esp_image_segment_header_t *segment_header, uint32_t *segment_data_offset)
{
    esp_err_t err = ESP_OK;
    uint32_t next_addr = src_addr + sizeof(esp_image_header_t);

    if(index >= image_header->segment_count) {
        if (log_errors) {
            ESP_LOGE(TAG, "index %d higher than segment count %d", index, image_header->segment_count);
        }
        return ESP_ERR_INVALID_ARG;
    }

    for(int i = 0; i <= index && err == ESP_OK; i++) {
        ESP_LOGV(TAG, "loading segment header %d at offset 0x%x", i, next_addr);
        err = bootloader_flash_read(next_addr, segment_header, sizeof(esp_image_segment_header_t), true);
        if (err == ESP_OK) {
            if ((segment_header->data_len & 3) != 0
                || segment_header->data_len >= SIXTEEN_MB) {
                if (log_errors) {
                    ESP_LOGE(TAG, "invalid segment length 0x%x", segment_header->data_len);
                }
                err = ESP_ERR_IMAGE_INVALID;
            }
            next_addr += sizeof(esp_image_segment_header_t);
            ESP_LOGV(TAG, "segment data length 0x%x data starts 0x%x", segment_header->data_len, next_addr);
            *segment_data_offset = next_addr;
            next_addr += segment_header->data_len;
        }
    }

    if (err != ESP_OK) {
        *segment_data_offset = 0;
        bzero(segment_header, sizeof(esp_image_segment_header_t));
    }

    return err;
}

esp_err_t esp_image_basic_verify(uint32_t src_addr, bool log_errors, uint32_t *p_length)
{
    esp_err_t err;
    uint8_t buf[128];
    uint8_t checksum = ESP_ROM_CHECKSUM_INITIAL;
    esp_image_header_t image_header;
    esp_image_segment_header_t segment_header = { 0 };
    uint32_t segment_data_offs = 0;
    uint32_t end_addr;
    uint32_t length;

    if (p_length != NULL) {
        *p_length = 0;
    }

    err = esp_image_load_header(src_addr, log_errors, &image_header);
    if (err != ESP_OK) {
        return err;
    }

    ESP_LOGD(TAG, "reading %d image segments", image_header.segment_count);

    /* Checksum each segment's data */
    for (int i = 0; i < image_header.segment_count; i++) {
        err = esp_image_load_segment_header(i, src_addr, &image_header, log_errors,
                                      &segment_header, &segment_data_offs);
        if (err != ESP_OK) {
            return err;
        }

        for (int i = 0; i < segment_header.data_len; i += sizeof(buf)) {
            err = bootloader_flash_read(segment_data_offs + i, buf, sizeof(buf), true);
            if (err != ESP_OK) {
                return err;
            }
            for (int j = 0; j < sizeof(buf) && i + j < segment_header.data_len; j++) {
                checksum ^= buf[j];
            }
        }
    }

    /* End of image, verify checksum */
    end_addr = segment_data_offs + segment_header.data_len;

    if (end_addr < src_addr) {
        if (log_errors) {
            ESP_LOGE(TAG, "image offset has wrapped");
        }
        return ESP_ERR_IMAGE_INVALID;
    }

    length = end_addr - src_addr;
    if (length >= SIXTEEN_MB) {
        if (log_errors) {
            ESP_LOGE(TAG, "invalid total length 0x%x", length);
        }
        return ESP_ERR_IMAGE_INVALID;
    }

    /* image padded to next full 16 byte block, with checksum byte at very end */
    ESP_LOGV(TAG, "unpadded image length 0x%x", length);
    length += 16; /* always pad by at least 1 byte */
    length = length - (length % 16);
    ESP_LOGV(TAG, "padded image length 0x%x", length);
    ESP_LOGD(TAG, "reading checksum block at 0x%x", src_addr + length - 16);
    bootloader_flash_read(src_addr + length - 16, buf, 16, true);
    if (checksum != buf[15]) {
        if (log_errors) {
            ESP_LOGE(TAG, "checksum failed. Calculated 0x%x read 0x%x",
                     checksum, buf[15]);
        }
        return ESP_ERR_IMAGE_INVALID;
    }

    if (p_length != NULL) {
        *p_length = length;
    }
    return ESP_OK;
}
