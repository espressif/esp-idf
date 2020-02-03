/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */
#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_partition.h"
#include "rom/crc.h"
#include "secure_cert_config.h"

#define TAG "Sec Cert Ops"

static esp_err_t secure_cert_read_raw_flash(const esp_partition_t* partition, uint32_t src_offset, void * dst, uint32_t size)
{
    /* Encrypted partitions need to be read via a cache mapping */
    const void *buf;
    spi_flash_mmap_handle_t handle;
    esp_err_t err;

    err = esp_partition_mmap(partition, src_offset, size, SPI_FLASH_MMAP_DATA, &buf, &handle);
    if (err != ESP_OK) {
        return err;
    }
    memcpy(dst, buf, size);
    spi_flash_munmap(handle);
    return ESP_OK;
}

static esp_err_t secure_cert_read(size_t offset, unsigned char *buffer, uint32_t * len)
{
    esp_err_t err;
    esp_partition_iterator_t it = esp_partition_find(SECURE_CERT_PARTITION_TYPE, ESP_PARTITION_SUBTYPE_ANY, SECURE_CERT_PARTITION_NAME);
    if (it == NULL) {
        ESP_LOGE(TAG, "Partition not found.");
        return ESP_FAIL;
    }

    const esp_partition_t * part = esp_partition_get(it);
    if (part == NULL) {
        ESP_LOGE(TAG, "Could not get partition.");
        return ESP_FAIL;
    }

    secure_cert_metadata metadata;
    err = secure_cert_read_raw_flash(part, offset, &metadata, sizeof(metadata));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Could not read magic byte.");
        return ESP_FAIL;
    }

    if (!(metadata.magic_byte == SECURE_CERT_PKEY_MAGIC_BYTE || metadata.magic_byte == SECURE_DEVICE_CERT_MAGIC_BYTE || metadata.magic_byte == SECURE_CA_CERT_MAGIC_BYTE)) {
        ESP_LOGE(TAG, "Unrecognised Magic Byte. %x", metadata.magic_byte);
        return ESP_FAIL;
    }

    if (buffer == NULL) {
        *len = metadata.secure_cert_len;
        return ESP_OK;
    }

    if (*len < metadata.secure_cert_len) {
        ESP_LOGE(TAG, "Insufficient length of buffer");        
        return ESP_FAIL;
    }

    err = secure_cert_read_raw_flash(part, offset + METADATA_SIZE, buffer, metadata.secure_cert_len);
    if (err != ESP_OK) {    
        ESP_LOGE(TAG, "Could not read data.");
        return ESP_FAIL;
    }

    uint32_t read_crc = crc32_le(UINT32_MAX ,(const uint8_t * )buffer, metadata.secure_cert_len);
    if (read_crc != metadata.secure_cert_crc) {
        ESP_LOGE(TAG,"Data has been tampered");
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t secure_cert_get_priv_key(unsigned char * buffer, uint32_t *len)
{
    return secure_cert_read(0, buffer, len);
}

esp_err_t secure_cert_get_device_cert(unsigned char * buffer, uint32_t *len)
{
    return secure_cert_read(SECURE_CERT_OFFSET, buffer, len);
}

esp_err_t secure_cert_get_ca_cert(unsigned char * buffer, uint32_t *len)
{
    return secure_cert_read(2 * SECURE_CERT_OFFSET, buffer, len);
}
