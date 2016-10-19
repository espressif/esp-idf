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

#ifndef __ESP_PARTITION_H__
#define __ESP_PARTITION_H__

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ESP_PARTITION_APP_MASK = 0x0000,
    ESP_PARTITION_APP_FACTORY = ESP_PARTITION_APP_MASK | 0x00,
    ESP_PARTITION_APP_OTA_MIN = ESP_PARTITION_APP_MASK | 0x10,
    ESP_PARTITION_APP_OTA_0 = ESP_PARTITION_APP_OTA_MIN + 0,
    ESP_PARTITION_APP_OTA_1 = ESP_PARTITION_APP_OTA_MIN + 1,
    ESP_PARTITION_APP_OTA_2 = ESP_PARTITION_APP_OTA_MIN + 2,
    ESP_PARTITION_APP_OTA_3 = ESP_PARTITION_APP_OTA_MIN + 3,
    ESP_PARTITION_APP_OTA_4 = ESP_PARTITION_APP_OTA_MIN + 4,
    ESP_PARTITION_APP_OTA_5 = ESP_PARTITION_APP_OTA_MIN + 5,
    ESP_PARTITION_APP_OTA_6 = ESP_PARTITION_APP_OTA_MIN + 6,
    ESP_PARTITION_APP_OTA_7 = ESP_PARTITION_APP_OTA_MIN + 7,
    ESP_PARTITION_APP_OTA_8 = ESP_PARTITION_APP_OTA_MIN + 8,
    ESP_PARTITION_APP_OTA_9 = ESP_PARTITION_APP_OTA_MIN + 9,
    ESP_PARTITION_APP_OTA_10 = ESP_PARTITION_APP_OTA_MIN + 10,
    ESP_PARTITION_APP_OTA_11 = ESP_PARTITION_APP_OTA_MIN + 11,
    ESP_PARTITION_APP_OTA_12 = ESP_PARTITION_APP_OTA_MIN + 12,
    ESP_PARTITION_APP_OTA_13 = ESP_PARTITION_APP_OTA_MIN + 13,
    ESP_PARTITION_APP_OTA_14 = ESP_PARTITION_APP_OTA_MIN + 14,
    ESP_PARTITION_APP_OTA_15 = ESP_PARTITION_APP_OTA_MIN + 15,
    ESP_PARTITION_APP_OTA_MAX = ESP_PARTITION_APP_MASK | 0x1f,
    ESP_PARTITION_APP_TEST = ESP_PARTITION_APP_MASK | 0x20,
    ESP_PARTITION_APP_ANY = ESP_PARTITION_APP_MASK | 0xff,

    ESP_PARTITION_DATA_MASK = 0x0100,
    ESP_PARTITION_DATA_OTA = ESP_PARTITION_DATA_MASK | 0x00,
    ESP_PARTITION_DATA_RF = ESP_PARTITION_DATA_MASK | 0x01,
    ESP_PARTITION_DATA_WIFI = ESP_PARTITION_DATA_MASK | 0x02,
    ESP_PARTITION_DATA_ANY = ESP_PARTITION_DATA_MASK | 0xff,

    ESP_PARTITION_FILESYSTEM_MASK = 0x0200,
    ESP_PARTITION_FILESYSTEM_ESPHTTPD = 0x0200,
    ESP_PARTITION_FILESYSTEM_FAT = 0x0201,
    ESP_PARTITION_FILESYSTEM_SPIFFS = 0x0202,
    ESP_PARTITION_FILESYSTEM_ANY = 0x20ff,

    ESP_PARTITION_END = 0xffff
} esp_partition_type_t;

#define ESP_PARTITION_APP_OTA(i) ((esp_partition_type_t)(ESP_PARTITION_APP_OTA_MIN + ((i) & 0xf)))


typedef struct esp_partition_iterator_opaque_* esp_partition_iterator_t;

typedef struct {
    esp_partition_type_t type;
    uint32_t address;
    uint32_t size;
    char label[17];
    bool encrypted;
} esp_partition_t;

/**
 * @brief Find partition based on one or more parameters
 *
 * @param type Partition type, one of esp_partition_type_t values
 *             To find all app partitions or all filesystem partitions,
 *             use ESP_PARTITION_APP_ANY or ESP_PARTITION_FILESYSTEM_ANY,
 *             respectively.
 * @param label (optional) Partition label. Set this value if looking
 *             for partition with a specific name. Pass NULL otherwise.
 *
 * @return iterator which can be used to enumerate all the partitions found,
 *         or NULL if no partitions were found.
 *         Iterator obtained through this function has to be released
 *         using esp_partition_iterator_release when not used any more.
 */
esp_partition_iterator_t esp_partition_find(esp_partition_type_t type, const char* label);

/**
 * @brief Find first partition based on one or more parameters
 *
 * @param type Partition type, one of esp_partition_type_t values
 *             To find all app partitions or all filesystem partitions,
 *             use ESP_PARTITION_APP_ANY or ESP_PARTITION_FILESYSTEM_ANY,
 *             respectively.
 * @param label (optional) Partition label. Set this value if looking
 *             for partition with a specific name. Pass NULL otherwise.
 *
 * @return pointer to esp_partition_t structure, or NULL if no parition is found.
 *         This pointer is valid for the lifetime of the application.
 */
const esp_partition_t* esp_partition_find_first(esp_partition_type_t type, const char* label);

/**
 * @brief Get esp_partition_t structure for given partition
 *
 * @param iterator  Iterator obtained using esp_partition_find. Must be non-NULL.
 *
 * @return pointer to esp_partition_t structure. This pointer is valid for the lifetime
 *         of the application.
 */
const esp_partition_t* esp_partition_get(esp_partition_iterator_t iterator);

/**
 * @brief Move partition iterator to the next partition found
 *
 * Any copies of the iterator will be invalid after this call.
 *
 * @param iterator Iterator obtained using esp_partition_find. Must be non-NULL.
 *
 * @return NULL if no partition was found, valid esp_partition_iterator_t otherwise.
 */
esp_partition_iterator_t esp_partition_next(esp_partition_iterator_t iterator);

/**
 * @brief Get partition type
 *
 * @note This is a helper function built around esp_partition_get.
 *
 * @param iterator Iterator obtained using esp_partition_find. Must be non-NULL.
 *
 * @return esp_partition_type_t value for partition pointed to by the iterator.
 */
esp_partition_type_t esp_partition_type(esp_partition_iterator_t iterator);

/**
 * @brief Get partition size
 *
 * @note This is a helper function built around esp_partition_get.
 *
 * @param iterator Iterator obtained using esp_partition_find. Must be non-NULL.
 *
 * @return partition size, in bytes
 */
uint32_t esp_partition_size(esp_partition_iterator_t iterator);

/**
 * @brief Get partition address
 *
 * @note This is a helper function built around esp_partition_get.
 *
 * @param iterator Iterator obtained using esp_partition_find. Must be non-NULL.
 *
 * @return flash address of partition start
 */
uint32_t esp_partition_address(esp_partition_iterator_t iterator);

/**
 * @brief Get partition label
 *
 * @note This is a helper function built around esp_partition_get.
 *
 * @param iterator Iterator obtained using esp_partition_find. Must be non-NULL.
 *
 * @return pointer to a zero-terminated string with partition label.
 *         The pointer is valid for the lifetime of the application.
 */
const char* esp_partition_label(esp_partition_iterator_t iterator);

/**
 * @brief Read data from the partition
 *
 * @param iterator Iterator obtained using esp_partition_find. Must be non-NULL.
 * @param src_offset Address of the data to be read, relative to the
 *                   beginning of the partition.
 * @param dst Pointer to the buffer where data should be stored.
 *            Must be non-NULL and at least 'size' bytes long.
 * @param size Size of data to be read, in bytes.
 *
 * @return ESP_OK, if data was read successfully;
 *         ESP_ERR_INVALID_ARG, if iterator or src are NULL;
 *         ESP_ERR_INVALID_SIZE, if read would go out of bounds of the partition;
 *         or one of error codes from lower-level flash driver.
 */
esp_err_t esp_partition_read(esp_partition_iterator_t iterator,
                             uint32_t src_offset, uint8_t* dst, uint32_t size);

/**
 * @brief Write data to the partition
 *
 * @param iterator Iterator obtained using esp_partition_find. Must be non-NULL.
 * @param src Pointer to the source buffer.  Must be non-NULL and
 *              at least 'size' bytes long.
 * @param dst_offset Address where the data should be written, relative to the
 *                   beginning of the partition.
 * @param size Size of data to be written, in bytes.
 *
 * @note Prior to writing to flash memory, make sure it has been erased with
 *       esp_partition_erase_range call.
 *
 * @return ESP_OK, if data was written successfully;
 *         ESP_ERR_INVALID_ARG, if iterator or dst are NULL;
 *         ESP_ERR_INVALID_SIZE, if write would go out of bounds of the partition;
 *         or one of error codes from lower-level flash driver.
 */
esp_err_t esp_partition_write(esp_partition_iterator_t iterator,
                             const uint8_t* src, uint32_t dst_offset, uint32_t size);

/**
 * @brief Erase part of the partition
 *
 * @param iterator Iterator obtained using esp_partition_find. Must be non-NULL.
 * @param start_addr Address where erase operation should start. Must be aligned
 *                   to 4 kilobytes.
 * @param size Size of the range which should be erased, in bytes.
 *                   Must be divisible by 4 kilobytes.
 *
 * @return ESP_OK, if the range was erased successfully;
 *         ESP_ERR_INVALID_ARG, if iterator or dst are NULL;
 *         ESP_ERR_INVALID_SIZE, if erase would go out of bounds of the partition;
 *         or one of error codes from lower-level flash driver.
 */
esp_err_t esp_partition_erase_range(esp_partition_iterator_t iterator,
                                    uint32_t start_addr, uint32_t size);

/**
 * @brief Configure MMU to map partition into data memory
 *
 * @param iterator Iterator obtained using esp_partition_find. Must be non-NULL.
 *
 * @param offset Offset from the beginning of partition where mapping should start.
 *               Must be aligned to 64k.
 *
 * @param size Size of the area to be mapped.
 *
 * @return pointer to mapped memory, if successful
 *         NULL, if memory can not be mapped for any reason
 */
void* esp_partition_mmap(esp_partition_iterator_t iterator, uint32_t offset, uint32_t size);


/**
 * @brief Release partition iterator
 *
 * Any pointers obtained using esp_partition_label function will be invalid
 * after this call.
 *
 * @param iterator Iterator obtained using esp_partition_find. Must be non-NULL.
 *
 */
void esp_partition_iterator_release(esp_partition_iterator_t iterator);


#ifdef __cplusplus
}
#endif


#endif /* __ESP_PARTITION_H__ */
