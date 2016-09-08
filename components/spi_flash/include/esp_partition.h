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

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

enum esp_partition_type_t {
    PT_APP_MASK = 0x0000,
    PT_APP_FACTORY = PT_APP_MASK | 0x00,
    PT_APP_OTA_MIN = PT_APP_MASK | 0x10,
    PT_APP_OTA_0 = PT_APP_OTA_MIN + 0,
    PT_APP_OTA_1 = PT_APP_OTA_MIN + 1,
    PT_APP_OTA_2 = PT_APP_OTA_MIN + 2,
    PT_APP_OTA_3 = PT_APP_OTA_MIN + 3,
    PT_APP_OTA_4 = PT_APP_OTA_MIN + 4,
    PT_APP_OTA_5 = PT_APP_OTA_MIN + 5,
    PT_APP_OTA_6 = PT_APP_OTA_MIN + 6,
    PT_APP_OTA_7 = PT_APP_OTA_MIN + 7,
    PT_APP_OTA_8 = PT_APP_OTA_MIN + 8,
    PT_APP_OTA_9 = PT_APP_OTA_MIN + 9,
    PT_APP_OTA_10 = PT_APP_OTA_MIN + 10,
    PT_APP_OTA_11 = PT_APP_OTA_MIN + 11,
    PT_APP_OTA_12 = PT_APP_OTA_MIN + 12,
    PT_APP_OTA_13 = PT_APP_OTA_MIN + 13,
    PT_APP_OTA_14 = PT_APP_OTA_MIN + 14,
    PT_APP_OTA_15 = PT_APP_OTA_MIN + 15,
    PT_APP_OTA_MAX = PT_APP_MASK | 0x1f,
    PT_APP_TEST = PT_APP_MASK | 0x20,
    PT_APP_ANY = PT_APP_MASK | 0xff,

    PT_DATA_MASK = 0x0100,
    PT_DATA_OTA = PT_DATA_MASK | 0x00,
    PT_DATA_RF = PT_DATA_MASK | 0x01,
    PT_DATA_WIFI = PT_DATA_MASK | 0x02,
    PT_DATA_ANY = PT_DATA_MASK | 0xff,

    PT_FILESYSTEM_MASK = 0x0200,
    PT_FILESYSTEM_ESPHTTPD = 0x0200,
    PT_FILESYSTEM_FAT = 0x0201,
    PT_FILESYSTEM_SPIFFS = 0x0202,
    PT_FILESYSTEM_ANY = 0x20ff,

    PT_END = 0xffff
};

#define PT_APP_OTA(i) ((esp_partition_type_t)(PT_APP_OTA_MIN + ((i) & 0xf)))


typedef struct esp_partition_iterator_opaque_t* esp_partition_iterator_t;

/**
 * @brief Find partition based on one or more parameters
 *
 * @param type Partition type, one of esp_partition_type_t values
 *             To find all app partitions or all filesystem partitions,
 *             use PT_APP_ANY or PT_FILESYSTEM_ANY, respectively.
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
 * @brief Move partition iterator to the next partition found
 *
 * Any pointers obtained using esp_partition_label function for this iterator
 * will be invalid after this call.
 *
 * @param iterator Iterator obtained using esp_partition_find. Must be non-NULL.
 *
 * @return iterator pointing to the next partition found, or NULL if no more
 *          partitions were found.
 *
 */
esp_partition_iterator_t esp_partition_next(esp_partition_iterator_t iterator);

/**
 * @brief Get partition type
 *
 * @param iterator Iterator obtained using esp_partition_find. Must be non-NULL.
 *
 * @return esp_partition_type_t value for partition pointed to by the iterator.
 */
esp_partition_type_t esp_partition_type(esp_partition_iterator_t iterator);

/**
 * @brief Get partition size
 *
 * @param iterator Iterator obtained using esp_partition_find. Must be non-NULL.
 *
 * @return partition size, in bytes
 */
uint32_t esp_partition_size(esp_partition_iterator_t iterator);

/**
 * @brief Get partition address
 *
 * @param iterator Iterator obtained using esp_partition_find. Must be non-NULL.
 *
 * @return flash address of partition start
 */
uint32_t esp_partition_address(esp_partition_iterator_t iterator);

/**
 * @brief Get partition label
 *
 * @param iterator Iterator obtained using esp_partition_find. Must be non-NULL.
 *
 * @return pointer to a zero-terminated string with partition label.
 *         The pointer is valid until the call to esp_partition_next or
 *         esp_partition_iterator_release for the given iterator.
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
 *         ESP_INVALID_ARG, if iterator or src are NULL;
 *         ESP_INVALID_SIZE, if read would go out of bounds of the partition;
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
 *         ESP_INVALID_ARG, if iterator or dst are NULL;
 *         ESP_INVALID_SIZE, if write would go out of bounds of the partition;
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
 *         ESP_INVALID_ARG, if iterator or dst are NULL;
 *         ESP_INVALID_SIZE, if erase would go out of bounds of the partition;
 *         or one of error codes from lower-level flash driver.
 */
esp_err_t esp_partition_erase_range(esp_partition_iterator_t iterator,
                                    uint32_t start_addr, uint32_t size);


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
