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
#ifndef ESP_NVS_H
#define ESP_NVS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Opaque pointer type representing non-volatile storage handle
 */
typedef uint32_t nvs_handle;

#define ESP_ERR_NVS_BASE                    0x1100                     /*!< Starting number of error codes */
#define ESP_ERR_NVS_NOT_INITIALIZED         (ESP_ERR_NVS_BASE + 0x01)  /*!< The storage driver is not initialized */
#define ESP_ERR_NVS_NOT_FOUND               (ESP_ERR_NVS_BASE + 0x02)  /*!< Id namespace doesn’t exist yet and mode is NVS_READONLY */
#define ESP_ERR_NVS_TYPE_MISMATCH           (ESP_ERR_NVS_BASE + 0x03)  /*!< The type of set or get operation doesn't match the type of value stored in NVS */
#define ESP_ERR_NVS_READ_ONLY               (ESP_ERR_NVS_BASE + 0x04)  /*!< Storage handle was opened as read only */
#define ESP_ERR_NVS_NOT_ENOUGH_SPACE        (ESP_ERR_NVS_BASE + 0x05)  /*!< There is not enough space in the underlying storage to save the value */
#define ESP_ERR_NVS_INVALID_NAME            (ESP_ERR_NVS_BASE + 0x06)  /*!< Namespace name doesn’t satisfy constraints */
#define ESP_ERR_NVS_INVALID_HANDLE          (ESP_ERR_NVS_BASE + 0x07)  /*!< Handle has been closed or is NULL */
#define ESP_ERR_NVS_REMOVE_FAILED           (ESP_ERR_NVS_BASE + 0x08)  /*!< The value wasn’t updated because flash write operation has failed. The value was written however, and update will be finished after re-initialization of nvs, provided that flash operation doesn’t fail again. */
#define ESP_ERR_NVS_KEY_TOO_LONG            (ESP_ERR_NVS_BASE + 0x09)  /*!< Key name is too long */
#define ESP_ERR_NVS_PAGE_FULL               (ESP_ERR_NVS_BASE + 0x0a)  /*!< Internal error; never returned by nvs API functions */
#define ESP_ERR_NVS_INVALID_STATE           (ESP_ERR_NVS_BASE + 0x0b)  /*!< NVS is in an inconsistent state due to a previous error. Call nvs_flash_init and nvs_open again, then retry. */
#define ESP_ERR_NVS_INVALID_LENGTH          (ESP_ERR_NVS_BASE + 0x0c)  /*!< String or blob length is not sufficient to store data */
#define ESP_ERR_NVS_NO_FREE_PAGES           (ESP_ERR_NVS_BASE + 0x0d)  /*!< NVS partition doesn't contain any empty pages. This may happen if NVS partition was truncated. Erase the whole partition and call nvs_flash_init again. */
#define ESP_ERR_NVS_VALUE_TOO_LONG          (ESP_ERR_NVS_BASE + 0x0e)  /*!< String or blob length is longer than supported by the implementation */
#define ESP_ERR_NVS_PART_NOT_FOUND          (ESP_ERR_NVS_BASE + 0x0f)  /*!< Partition with specified name is not found in the partition table */

#define ESP_ERR_NVS_NEW_VERSION_FOUND       (ESP_ERR_NVS_BASE + 0x10)  /*!< NVS partition contains data in new format and cannot be recognized by this version of code */
#define ESP_ERR_NVS_XTS_ENCR_FAILED         (ESP_ERR_NVS_BASE + 0x11)  /*!< XTS encryption failed while writing NVS entry */
#define ESP_ERR_NVS_XTS_DECR_FAILED         (ESP_ERR_NVS_BASE + 0x12)  /*!< XTS decryption failed while reading NVS entry */
#define ESP_ERR_NVS_XTS_CFG_FAILED          (ESP_ERR_NVS_BASE + 0x13)  /*!< XTS configuration setting failed */
#define ESP_ERR_NVS_XTS_CFG_NOT_FOUND       (ESP_ERR_NVS_BASE + 0x14)  /*!< XTS configuration not found */
#define ESP_ERR_NVS_ENCR_NOT_SUPPORTED      (ESP_ERR_NVS_BASE + 0x15)  /*!< NVS encryption is not supported in this version */
#define ESP_ERR_NVS_KEYS_NOT_INITIALIZED    (ESP_ERR_NVS_BASE + 0x16)  /*!< NVS key partition is uninitialized */
#define ESP_ERR_NVS_CORRUPT_KEY_PART        (ESP_ERR_NVS_BASE + 0x17)  /*!< NVS key partition is corrupt */


#define NVS_DEFAULT_PART_NAME           "nvs"   /*!< Default partition name of the NVS partition in the partition table */
/**
 * @brief Mode of opening the non-volatile storage
 *
 */
typedef enum {
	NVS_READONLY,  /*!< Read only */
	NVS_READWRITE  /*!< Read and write */
} nvs_open_mode;

typedef enum {
    NVS_TYPE_U8    = 0x01,
    NVS_TYPE_I8    = 0x11,
    NVS_TYPE_U16   = 0x02,
    NVS_TYPE_I16   = 0x12,
    NVS_TYPE_U32   = 0x04,
    NVS_TYPE_I32   = 0x14,
    NVS_TYPE_U64   = 0x08,
    NVS_TYPE_I64   = 0x18,
    NVS_TYPE_STR   = 0x21,
    NVS_TYPE_BLOB  = 0x42,
    NVS_TYPE_ANY   = 0xff // Must be last
} nvs_type_t;

/**
 * @brief      Open non-volatile storage with a given namespace from the default NVS partition
 *
 * Multiple internal ESP-IDF and third party application modules can store
 * their key-value pairs in the NVS module. In order to reduce possible
 * conflicts on key names, each module can use its own namespace.
 * The default NVS partition is the one that is labelled "nvs" in the partition
 * table.
 *
 * @param[in]  name        Namespace name. Maximal length is determined by the
 *                         underlying implementation, but is guaranteed to be
 *                         at least 15 characters. Shouldn't be empty.
 * @param[in]  open_mode   NVS_READWRITE or NVS_READONLY. If NVS_READONLY, will
 *                         open a handle for reading only. All write requests will
 *			   be rejected for this handle.
 * @param[out] out_handle  If successful (return code is zero), handle will be
 *                         returned in this argument.
 *
 * @return
 *             - ESP_OK if storage handle was opened successfully
 *             - ESP_ERR_NVS_NOT_INITIALIZED if the storage driver is not initialized
 *             - ESP_ERR_NVS_PART_NOT_FOUND if the partition with label "nvs" is not found
 *             - ESP_ERR_NVS_NOT_FOUND id namespace doesn't exist yet and
 *               mode is NVS_READONLY
 *             - ESP_ERR_NVS_INVALID_NAME if namespace name doesn't satisfy constraints
 *             - other error codes from the underlying storage driver
 */
esp_err_t nvs_open(const char* name, nvs_open_mode open_mode, nvs_handle *out_handle);

/**
 * @brief      Open non-volatile storage with a given namespace from specified partition
 *
 * The behaviour is same as nvs_open() API. However this API can operate on a specified NVS
 * partition instead of default NVS partition. Note that the specified partition must be registered
 * with NVS using nvs_flash_init_partition() API.
 *
 * @param[in]  part_name   Label (name) of the partition of interest for object read/write/erase
 * @param[in]  name        Namespace name. Maximal length is determined by the
 *                         underlying implementation, but is guaranteed to be
 *                         at least 15 characters. Shouldn't be empty.
 * @param[in]  open_mode   NVS_READWRITE or NVS_READONLY. If NVS_READONLY, will 
 *                         open a handle for reading only. All write requests will 
 *			   be rejected for this handle.
 * @param[out] out_handle  If successful (return code is zero), handle will be
 *                         returned in this argument.
 *
 * @return
 *             - ESP_OK if storage handle was opened successfully
 *             - ESP_ERR_NVS_NOT_INITIALIZED if the storage driver is not initialized
 *             - ESP_ERR_NVS_PART_NOT_FOUND if the partition with specified name is not found
 *             - ESP_ERR_NVS_NOT_FOUND id namespace doesn't exist yet and
 *               mode is NVS_READONLY
 *             - ESP_ERR_NVS_INVALID_NAME if namespace name doesn't satisfy constraints
 *             - other error codes from the underlying storage driver
 */
esp_err_t nvs_open_from_partition(const char *part_name, const char* name, nvs_open_mode open_mode, nvs_handle *out_handle);

/**@{*/
/**
 * @brief      set value for given key
 *
 * This family of functions set value for the key, given its name. Note that
 * actual storage will not be updated until nvs_commit function is called.
 *
 * @param[in]  handle  Handle obtained from nvs_open function.
 *                     Handles that were opened read only cannot be used.
 * @param[in]  key     Key name. Maximal length is determined by the underlying
 *                     implementation, but is guaranteed to be at least
 *                     15 characters. Shouldn't be empty.
 * @param[in]  value   The value to set.
 *                     For strings, the maximum length (including null character) is
 *                     4000 bytes.
 *
 * @return
 *             - ESP_OK if value was set successfully
 *             - ESP_ERR_NVS_INVALID_HANDLE if handle has been closed or is NULL
 *             - ESP_ERR_NVS_READ_ONLY if storage handle was opened as read only
 *             - ESP_ERR_NVS_INVALID_NAME if key name doesn't satisfy constraints
 *             - ESP_ERR_NVS_NOT_ENOUGH_SPACE if there is not enough space in the
 *               underlying storage to save the value
 *             - ESP_ERR_NVS_REMOVE_FAILED if the value wasn't updated because flash
 *               write operation has failed. The value was written however, and
 *               update will be finished after re-initialization of nvs, provided that
 *               flash operation doesn't fail again.
 *             - ESP_ERR_NVS_VALUE_TOO_LONG if the string value is too long
 */
esp_err_t nvs_set_i8  (nvs_handle handle, const char* key, int8_t value);
esp_err_t nvs_set_u8  (nvs_handle handle, const char* key, uint8_t value);
esp_err_t nvs_set_i16 (nvs_handle handle, const char* key, int16_t value);
esp_err_t nvs_set_u16 (nvs_handle handle, const char* key, uint16_t value);
esp_err_t nvs_set_i32 (nvs_handle handle, const char* key, int32_t value);
esp_err_t nvs_set_u32 (nvs_handle handle, const char* key, uint32_t value);
esp_err_t nvs_set_i64 (nvs_handle handle, const char* key, int64_t value);
esp_err_t nvs_set_u64 (nvs_handle handle, const char* key, uint64_t value);
esp_err_t nvs_set_str (nvs_handle handle, const char* key, const char* value);
/**@}*/ 

/**
 * @brief       set variable length binary value for given key
 *
 * This family of functions set value for the key, given its name. Note that
 * actual storage will not be updated until nvs_commit function is called.
 *
 * @param[in]  handle  Handle obtained from nvs_open function.
 *                     Handles that were opened read only cannot be used.
 * @param[in]  key     Key name. Maximal length is 15 characters. Shouldn't be empty.
 * @param[in]  value   The value to set.
 * @param[in]  length  length of binary value to set, in bytes; Maximum length is
 *                     508000 bytes or (97.6% of the partition size - 4000) bytes
 *                     whichever is lower.
 *
 * @return
 *             - ESP_OK if value was set successfully
 *             - ESP_ERR_NVS_INVALID_HANDLE if handle has been closed or is NULL
 *             - ESP_ERR_NVS_READ_ONLY if storage handle was opened as read only
 *             - ESP_ERR_NVS_INVALID_NAME if key name doesn't satisfy constraints
 *             - ESP_ERR_NVS_NOT_ENOUGH_SPACE if there is not enough space in the
 *               underlying storage to save the value
 *             - ESP_ERR_NVS_REMOVE_FAILED if the value wasn't updated because flash
 *               write operation has failed. The value was written however, and
 *               update will be finished after re-initialization of nvs, provided that
 *               flash operation doesn't fail again.
 *             - ESP_ERR_NVS_VALUE_TOO_LONG if the value is too long
 */
esp_err_t nvs_set_blob(nvs_handle handle, const char* key, const void* value, size_t length);

/**@{*/
/**
 * @brief      get value for given key
 *
 * These functions retrieve value for the key, given its name. If key does not
 * exist, or the requested variable type doesn't match the type which was used
 * when setting a value, an error is returned.
 *
 * In case of any error, out_value is not modified.
 *
 * All functions expect out_value to be a pointer to an already allocated variable
 * of the given type.
 *
 * \code{c}
 * // Example of using nvs_get_i32:
 * int32_t max_buffer_size = 4096; // default value
 * esp_err_t err = nvs_get_i32(my_handle, "max_buffer_size", &max_buffer_size);
 * assert(err == ESP_OK || err == ESP_ERR_NVS_NOT_FOUND);
 * // if ESP_ERR_NVS_NOT_FOUND was returned, max_buffer_size will still
 * // have its default value.
 *
 * \endcode
 *
 * @param[in]     handle     Handle obtained from nvs_open function.
 * @param[in]     key        Key name. Maximal length is determined by the underlying
 *                           implementation, but is guaranteed to be at least
 *                           15 characters. Shouldn't be empty.
 * @param         out_value  Pointer to the output value.
 *                           May be NULL for nvs_get_str and nvs_get_blob, in this
 *                           case required length will be returned in length argument.
 *
 * @return
 *             - ESP_OK if the value was retrieved successfully
 *             - ESP_ERR_NVS_NOT_FOUND if the requested key doesn't exist
 *             - ESP_ERR_NVS_INVALID_HANDLE if handle has been closed or is NULL
 *             - ESP_ERR_NVS_INVALID_NAME if key name doesn't satisfy constraints
 *             - ESP_ERR_NVS_INVALID_LENGTH if length is not sufficient to store data
 */
esp_err_t nvs_get_i8  (nvs_handle handle, const char* key, int8_t* out_value);
esp_err_t nvs_get_u8  (nvs_handle handle, const char* key, uint8_t* out_value);
esp_err_t nvs_get_i16 (nvs_handle handle, const char* key, int16_t* out_value);
esp_err_t nvs_get_u16 (nvs_handle handle, const char* key, uint16_t* out_value);
esp_err_t nvs_get_i32 (nvs_handle handle, const char* key, int32_t* out_value);
esp_err_t nvs_get_u32 (nvs_handle handle, const char* key, uint32_t* out_value);
esp_err_t nvs_get_i64 (nvs_handle handle, const char* key, int64_t* out_value);
esp_err_t nvs_get_u64 (nvs_handle handle, const char* key, uint64_t* out_value);
/**@}*/ 

/**
 * @brief      get value for given key
 *
 * These functions retrieve value for the key, given its name. If key does not
 * exist, or the requested variable type doesn't match the type which was used
 * when setting a value, an error is returned.
 *
 * In case of any error, out_value is not modified.
 *
 * All functions expect out_value to be a pointer to an already allocated variable
 * of the given type.
 * 
 * nvs_get_str and nvs_get_blob functions support WinAPI-style length queries.
 * To get the size necessary to store the value, call nvs_get_str or nvs_get_blob
 * with zero out_value and non-zero pointer to length. Variable pointed to
 * by length argument will be set to the required length. For nvs_get_str,
 * this length includes the zero terminator. When calling nvs_get_str and
 * nvs_get_blob with non-zero out_value, length has to be non-zero and has to
 * point to the length available in out_value.
 * It is suggested that nvs_get/set_str is used for zero-terminated C strings, and
 * nvs_get/set_blob used for arbitrary data structures.
 *
 * \code{c}
 * // Example (without error checking) of using nvs_get_str to get a string into dynamic array:
 * size_t required_size;
 * nvs_get_str(my_handle, "server_name", NULL, &required_size);
 * char* server_name = malloc(required_size);
 * nvs_get_str(my_handle, "server_name", server_name, &required_size);
 *
 * // Example (without error checking) of using nvs_get_blob to get a binary data
 * into a static array:
 * uint8_t mac_addr[6];
 * size_t size = sizeof(mac_addr);
 * nvs_get_blob(my_handle, "dst_mac_addr", mac_addr, &size);
 * \endcode
 *
 * @param[in]     handle     Handle obtained from nvs_open function.
 * @param[in]     key        Key name. Maximal length is determined by the underlying
 *                           implementation, but is guaranteed to be at least
 *                           15 characters. Shouldn't be empty.
 * @param         out_value  Pointer to the output value.
 *                           May be NULL for nvs_get_str and nvs_get_blob, in this
 *                           case required length will be returned in length argument.
 * @param[inout]  length     A non-zero pointer to the variable holding the length of out_value.
 *                           In case out_value a zero, will be set to the length
 *                           required to hold the value. In case out_value is not
 *                           zero, will be set to the actual length of the value
 *                           written. For nvs_get_str this includes zero terminator.
 *
 * @return
 *             - ESP_OK if the value was retrieved successfully
 *             - ESP_ERR_NVS_NOT_FOUND if the requested key doesn't exist
 *             - ESP_ERR_NVS_INVALID_HANDLE if handle has been closed or is NULL
 *             - ESP_ERR_NVS_INVALID_NAME if key name doesn't satisfy constraints
 *             - ESP_ERR_NVS_INVALID_LENGTH if length is not sufficient to store data
 */
/**@{*/
esp_err_t nvs_get_str (nvs_handle handle, const char* key, char* out_value, size_t* length);
esp_err_t nvs_get_blob(nvs_handle handle, const char* key, void* out_value, size_t* length);
/**@}*/

/**
 * @brief      Erase key-value pair with given key name.
 *
 * Note that actual storage may not be updated until nvs_commit function is called.
 *
 * @param[in]  handle  Storage handle obtained with nvs_open.
 *                     Handles that were opened read only cannot be used.
 *
 * @param[in]  key     Key name. Maximal length is determined by the underlying
 *                     implementation, but is guaranteed to be at least
 *                     15 characters. Shouldn't be empty.
 *
 * @return
 *              - ESP_OK if erase operation was successful
 *              - ESP_ERR_NVS_INVALID_HANDLE if handle has been closed or is NULL
 *              - ESP_ERR_NVS_READ_ONLY if handle was opened as read only
 *              - ESP_ERR_NVS_NOT_FOUND if the requested key doesn't exist
 *              - other error codes from the underlying storage driver
 */
esp_err_t nvs_erase_key(nvs_handle handle, const char* key);

/**
 * @brief      Erase all key-value pairs in a namespace
 *
 * Note that actual storage may not be updated until nvs_commit function is called.
 *
 * @param[in]  handle  Storage handle obtained with nvs_open.
 *                     Handles that were opened read only cannot be used.
 *
 * @return
 *              - ESP_OK if erase operation was successful
 *              - ESP_ERR_NVS_INVALID_HANDLE if handle has been closed or is NULL
 *              - ESP_ERR_NVS_READ_ONLY if handle was opened as read only
 *              - other error codes from the underlying storage driver
 */
esp_err_t nvs_erase_all(nvs_handle handle);

/**
 * @brief      Write any pending changes to non-volatile storage
 *
 * After setting any values, nvs_commit() must be called to ensure changes are written
 * to non-volatile storage. Individual implementations may write to storage at other times,
 * but this is not guaranteed.
 *
 * @param[in]  handle  Storage handle obtained with nvs_open.
 *                     Handles that were opened read only cannot be used.
 *
 * @return
 *             - ESP_OK if the changes have been written successfully
 *             - ESP_ERR_NVS_INVALID_HANDLE if handle has been closed or is NULL
 *             - other error codes from the underlying storage driver
 */
esp_err_t nvs_commit(nvs_handle handle);

/**
 * @brief      Close the storage handle and free any allocated resources
 *
 * This function should be called for each handle opened with nvs_open once
 * the handle is not in use any more. Closing the handle may not automatically
 * write the changes to nonvolatile storage. This has to be done explicitly using
 * nvs_commit function.
 * Once this function is called on a handle, the handle should no longer be used.
 *
 * @param[in]  handle  Storage handle to close
 */
void nvs_close(nvs_handle handle);

/**
 * @note Info about storage space NVS.
 */
typedef struct {
    size_t used_entries;      /**< Amount of used entries. */
    size_t free_entries;      /**< Amount of free entries. */
    size_t total_entries;     /**< Amount all available entries. */
    size_t namespace_count;   /**< Amount name space. */
} nvs_stats_t;

/**
 * @brief      Fill structure nvs_stats_t. It provides info about used memory the partition.
 *
 * This function calculates to runtime the number of used entries, free entries, total entries,
 * and amount namespace in partition.
 *
 * \code{c}
 * // Example of nvs_get_stats() to get the number of used entries and free entries:
 * nvs_stats_t nvs_stats;
 * nvs_get_stats(NULL, &nvs_stats);
 * printf("Count: UsedEntries = (%d), FreeEntries = (%d), AllEntries = (%d)\n",
          nvs_stats.used_entries, nvs_stats.free_entries, nvs_stats.total_entries);
 * \endcode
 *
 * @param[in]   part_name   Partition name NVS in the partition table.
 *                          If pass a NULL than will use NVS_DEFAULT_PART_NAME ("nvs").
 *
 * @param[out]  nvs_stats   Returns filled structure nvs_states_t.
 *                          It provides info about used memory the partition.
 *
 *
 * @return
 *             - ESP_OK if the changes have been written successfully.
 *               Return param nvs_stats will be filled.
 *             - ESP_ERR_NVS_PART_NOT_FOUND if the partition with label "name" is not found.
 *               Return param nvs_stats will be filled 0.
 *             - ESP_ERR_NVS_NOT_INITIALIZED if the storage driver is not initialized.
 *               Return param nvs_stats will be filled 0.
 *             - ESP_ERR_INVALID_ARG if nvs_stats equal to NULL.
 *             - ESP_ERR_INVALID_STATE if there is page with the status of INVALID.
 *               Return param nvs_stats will be filled not with correct values because
 *               not all pages will be counted. Counting will be interrupted at the first INVALID page.
 */
esp_err_t nvs_get_stats(const char* part_name, nvs_stats_t* nvs_stats);

/**
 * @brief      Calculate all entries in a namespace.
 *
 * Note that to find out the total number of records occupied by the namespace,
 * add one to the returned value used_entries (if err is equal to ESP_OK).
 * Because the name space entry takes one entry.
 *
 * \code{c}
 * // Example of nvs_get_used_entry_count() to get amount of all key-value pairs in one namespace:
 * nvs_handle handle;
 * nvs_open("namespace1", NVS_READWRITE, &handle);
 * ...
 * size_t used_entries;
 * size_t total_entries_namespace;
 * if(nvs_get_used_entry_count(handle, &used_entries) == ESP_OK){
 *     // the total number of records occupied by the namespace
 *     total_entries_namespace = used_entries + 1;
 * }
 * \endcode
 *
 * @param[in]   handle              Handle obtained from nvs_open function.
 *
 * @param[out]  used_entries        Returns amount of used entries from a namespace.
 *
 *
 * @return
 *             - ESP_OK if the changes have been written successfully.
 *               Return param used_entries will be filled valid value.
 *             - ESP_ERR_NVS_NOT_INITIALIZED if the storage driver is not initialized.
 *               Return param used_entries will be filled 0.
 *             - ESP_ERR_NVS_INVALID_HANDLE if handle has been closed or is NULL.
 *               Return param used_entries will be filled 0.
 *             - ESP_ERR_INVALID_ARG if nvs_stats equal to NULL.
 *             - Other error codes from the underlying storage driver.
 *               Return param used_entries will be filled 0.
 */
esp_err_t nvs_get_used_entry_count(nvs_handle handle, size_t* used_entries);

#ifdef __cplusplus
} // extern "C"
#endif

#endif //ESP_NVS_H

