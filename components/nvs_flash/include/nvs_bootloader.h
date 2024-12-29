/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "esp_err.h"        // esp_err_t
#include "nvs.h"            // nvs entry data types

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file nvs_bootloader.h
 * @brief Definition of a structures and public functions provided for NVS bootloader operations.
 */

/**
 * @brief Placeholders for buffer pointer and length of string type
 */
typedef struct {
    char* buff_ptr;     /**< Pointer to the buffer where string and terminating zero character will be read */
    size_t buff_len;    /**< Buffer length in bytes */
} nvs_bootloader_str_value_placeholder_t;

/**
 * @brief Union of value placeholders for all nvs_type_t supported by bootloader code
 */
typedef union {
    uint8_t   u8_val;   /**< Placeholder for unsigned 8 bit integer variable */
    int8_t    i8_val;   /**< Placeholder for signed 8 bit integer variable */
    uint16_t u16_val;   /**< Placeholder for unsigned 16 bit integer variable */
    int16_t   i16_val;  /**< Placeholder for signed 16 bit integer variable */
    uint32_t u32_val;   /**< Placeholder for unsigned 32 bit integer variable */
    int32_t   i32_val;  /**< Placeholder for signed 32 bit integer variable */
    uint64_t u64_val;   /**< Placeholder for unsigned 64 bit integer variable */
    int64_t   i64_val;  /**< Placeholder for signed 64 bit integer variable */
    nvs_bootloader_str_value_placeholder_t str_val; /**< Placeholder for string buffer information */
} nvs_bootloader_value_placeholder_t;

/**
 * @brief Structure representing one NVS bootloader entry.
 *
 * This structure serves as read operation input parameters and result value and status placeholder.
 * Before calling the `nvs_bootloader_read` function, populate the namespace_name, key_name and value_type members.
 * If string value has to be read, provide also buffer and its length in the `value.str_val` member.
 *
 * The result_code member will be populated by the function with the result of the read operation.
 * There are 2 possible situations and interpretations of the result_code:
 * If the return value of the `nvs_bootloader_read` was ESP_OK, the result_code will be one of the following:
 * - `ESP_OK`: Entry found, value member contains the data. This is the only case when the value member is populated.
 * - `ESP_ERR_NVS_TYPE_MISMATCH`: Entry was found, but requested datatype doesn't match datatype found in NVS
 * - `ESP_ERR_NVS_NOT_FOUND`: Data was not found.
 * - `ESP_ERR_INVALID_SIZE`: the value found for string is longer than the space provided in placeholder (str_val.buff_len)
 * If the return value of the function was ESP_ERR_INVALID_ARG, the result_code will be one of the following:
 * - `ESP_ERR_NVS_NOT_FOUND`: Check of this parameters was successful.
 * - `ESP_ERR_NVS_INVALID_NAME`: namespace_name is NULL or too long
 * - `ESP_ERR_NVS_KEY_TOO_LONG`: key_name NULL or too long
 * - `ESP_ERR_INVALID_SIZE`: the size of the buffer provided for NVS_TYPE_STR in placeholder (str_val.buff_len) is zero or exceeds maximum value NVS_CONST_STR_LEN_MAX_SIZE
 * - `ESP_ERR_INVALID_ARG`: Invalid datatype requested
 *
 */
typedef struct {
    const char* namespace_name;                 /**< Namespace of the entry */
    const char* key_name;                       /**< Key of the entry */
    nvs_type_t value_type;                      /**< Expected datatype to be read, can be any of NVS_TYPE_U*, NVS_TYPE_I* or NVS_TYPE_STR */
    esp_err_t result_code;                      /**< Result code of this entry. Explanation is in general description of the struct nvs_bootloader_read_list_t*/
    nvs_bootloader_value_placeholder_t value;   /**< Placeholder for value read */
    uint8_t namespace_index;                    /**< Index of the namespace (internal variable, do not use) */
} nvs_bootloader_read_list_t;

/**
 * @brief Reads data specified from the specified NVS partition.
 *
 * This function reads data from the NVS partition specified by `partition_name`.
 * Multiple NVS entries can be read in a single call. The list of entries to read is specified in the `read_list` array.
 * Function indicates overall success or failure by its return value. In case it is ESP_OK or ESP_ERR_INVALID_ARG,
 * result of validation / reading of individual entry is returned in the `result_code` member of each element of the `read_list` array.
 *
 * @param partition_name The name of the NVS partition to read from.
 * @param read_list_count The number of elements in the `read_list` array.
 * @param read_list An array of `nvs_bootloader_read_list_t` structures specifying the keys and buffers for reading data.
 *
 * @return
 * The return value of the function in this file can be one of the following:
 * - `ESP_OK`: The function successfully checked all input parameters and executed successfully.
 *   The individual `result_code` in `read_list` indicates the result of the lookup for a particular requested key.
 * - `ESP_ERR_INVALID_ARG`: The validity of all `read_list` input parameters was
 *   checked and failed for at least one of the parameters. The individual `result_code`
 *   in `read_list` provides the detailed reason. This error code is also returned when read_list is null or read_list_count is 0.
 * - `ESP_ERR_NVS_INVALID_NAME`: The partition name specified is too long or is null.
 * - `ESP_ERR_NVS_PART_NOT_FOUND`: The partition was not found in the partition table.
 * - `ESP_ERR_NVS_CORRUPT_KEY_PART`: Encryption-related problems.
 * - `ESP_ERR_NVS_WRONG_ENCRYPTION`: Encryption-related problems.
 * - `ESP_ERR_INVALID_STATE`: NVS partition or pages related errors - wrong size of partition, header inconsistent / entries
 *   inconsistencies, multiple active pages, page in state INVALID.
 * - `ESP_ERR_NO_MEM`: Cannot allocate memory required to perform the function.
 * - Technical errors in underlying storage.
 */
esp_err_t nvs_bootloader_read(const char* partition_name,
                              const size_t read_list_count,
                              nvs_bootloader_read_list_t read_list[]);

#ifdef __cplusplus
}
#endif
