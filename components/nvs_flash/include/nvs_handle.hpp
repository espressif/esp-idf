/*
 * SPDX-FileCopyrightText: 2019-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <string>
#include <memory>
#include <type_traits>

#include "nvs.h"

namespace nvs {

#if defined(SEGGER_H) && defined(GLOBAL_H)
NVS_GUARD_SYSVIEW_MACRO_EXPANSION_PUSH();
#undef U8
#undef I8
#undef U16
#undef I16
#undef U32
#undef I32
#undef U64
#undef I64
#endif

/**
 * The possible blob types. This is a helper definition for template functions.
 */
enum class ItemType : uint8_t {
    U8   = NVS_TYPE_U8,
    I8   = NVS_TYPE_I8,
    U16  = NVS_TYPE_U16,
    I16  = NVS_TYPE_I16,
    U32  = NVS_TYPE_U32,
    I32  = NVS_TYPE_I32,
    U64  = NVS_TYPE_U64,
    I64  = NVS_TYPE_I64,
    FLOAT  = NVS_TYPE_FLOAT,
    DOUBLE = NVS_TYPE_DOUBLE,
    SZ   = NVS_TYPE_STR,
    BLOB = 0x41,
    BLOB_DATA = NVS_TYPE_BLOB,
    BLOB_IDX  = 0x48,
    ANY  = NVS_TYPE_ANY
};

#if defined(SEGGER_H) && defined(GLOBAL_H)
NVS_GUARD_SYSVIEW_MACRO_EXPANSION_POP();
#endif

/**
 * @brief A handle allowing nvs-entry related operations on the NVS.
 *
 * @note The scope of this handle may vary depending on the implementation, but normally would be the namespace of
 * a particular partition. Outside that scope, nvs entries can't be accessed/altered.
 */
class NVSHandle {
public:
    virtual ~NVSHandle() { }

    /**
     * @brief      set value for given key
     *
     * Sets value for key. Note that physical storage will not be updated until nvs_commit function is called.
     *
     * @param[in]  key     Key name. Maximal length is (NVS_KEY_NAME_MAX_SIZE-1) characters. Shouldn't be empty.
     * @param[in]  value   The value to set. Allowed types are the ones declared in ItemType as well as enums.
     *                     For strings, the maximum length (including null character) is
     *                     4000 bytes, if there is one complete page free for writing.
     *                     This decreases, however, if the free space is fragmented.
     *                     Note that enums lose their type information when stored in NVS. Ensure that the correct
     *                     enum type is used during retrieval with get_item.
     *
     * @return
     *             - ESP_OK if value was set successfully
     *             - ESP_ERR_NVS_INVALID_HANDLE if the handle has been closed or is NULL
     *             - ESP_ERR_NVS_READ_ONLY if storage handle was opened as read only
     *             - ESP_ERR_NVS_KEY_TOO_LONG if the key name is longer than (NVS_KEY_NAME_MAX_SIZE-1) characters
     *             - ESP_ERR_NVS_NOT_ENOUGH_SPACE if there is not enough space in the
     *               underlying storage to save the value
     *             - ESP_ERR_NVS_REMOVE_FAILED if the value wasn't updated because flash
     *               write operation has failed. The value was written however, and
     *               update will be finished after re-initialization of nvs, provided that
     *               flash operation doesn't fail again.
     *             - ESP_ERR_NVS_VALUE_TOO_LONG if the value is too long
     *             - ESP_FAIL if there is an internal error; most likely due to corrupted
     *               NVS partition (only if NVS assertion checks are disabled)
     *             - other error codes from the underlying storage driver
     */
    template<typename T>
    esp_err_t set_item(const char *key, T value);

    /**
     * @brief      set string for given key
     *
     * @param[in]  key     Key name. Maximal length is (NVS_KEY_NAME_MAX_SIZE-1) characters. Shouldn't be empty.
     * @param[in]  value   The string to set. Maximum length (including null character) is 4000 bytes,
     *                     if there is one complete page free for writing.
     *
     * @return
     *             - ESP_OK if value was set successfully
     *             - ESP_ERR_NVS_INVALID_HANDLE if handle has been closed or is invalid
     *             - ESP_ERR_NVS_READ_ONLY if storage handle was opened as read only
     *             - ESP_ERR_NVS_KEY_TOO_LONG if key name exceeds the maximum length
     *             - ESP_ERR_NVS_NOT_ENOUGH_SPACE if there is not enough space in the
     *               underlying storage to save the value
     *             - ESP_ERR_NVS_REMOVE_FAILED if the value wasn't updated because flash
     *               write operation has failed
     *             - ESP_ERR_NVS_VALUE_TOO_LONG if the string value is too long
     *             - other error codes from the underlying storage driver
     */
    virtual esp_err_t set_string(const char *key, const char* value) = 0;

    /**
     * @brief      get value for given key
     *
     * These functions retrieve value for the key, given its name. If key does not
     * exist, or the requested variable type doesn't match the type which was used
     * when setting a value, an error is returned.
     *
     * In case of any error, out_value is not modified.
     *
     * @param[in]     key        Key name. Maximal length is (NVS_KEY_NAME_MAX_SIZE-1) characters. Shouldn't be empty.
     * @param         value      The output value. All integral types which are declared in ItemType as well as enums
     *                           are allowed. Note however that enums lost their type information when stored in NVS.
     *                           Ensure that the correct enum type is used during retrieval with get_item.
     *
     * @return
     *             - ESP_OK if the value was retrieved successfully
     *             - ESP_ERR_NVS_INVALID_HANDLE if the handle has been closed or is NULL
     *             - ESP_ERR_NVS_NOT_FOUND if the requested key doesn't exist
     *             - ESP_ERR_NVS_TYPE_MISMATCH if the type of the stored value doesn't match the requested type
     *             - ESP_FAIL if there is an internal error; most likely due to corrupted
     *               NVS partition (only if NVS assertion checks are disabled)
     *             - other error codes from the underlying storage driver
     */
    template<typename T>
    esp_err_t get_item(const char *key, T &value);

    /**
     * @brief       set variable length binary value for given key
     *
     * This family of functions set value for the key, given its name. Note that
     * actual storage will not be updated until nvs_commit function is called.
     *
     * @param[in]  key     Key name. Maximal length is (NVS_KEY_NAME_MAX_SIZE-1) characters. Shouldn't be empty.
     * @param[in]  blob    The blob value to set.
     * @param[in]  len     length of binary value to set, in bytes; Maximum length is
     *                     508000 bytes or (97.6% of the partition size - 4000) bytes
     *                     whichever is lower.
     *
     * @return
     *             - ESP_OK if value was set successfully
     *             - ESP_ERR_NVS_INVALID_HANDLE if the handle has been closed or is NULL
     *             - ESP_ERR_NVS_READ_ONLY if storage handle was opened as read only
     *             - ESP_ERR_NVS_KEY_TOO_LONG if the key name is longer than (NVS_KEY_NAME_MAX_SIZE-1) characters
     *             - ESP_ERR_NVS_NOT_ENOUGH_SPACE if there is not enough space in the
     *               underlying storage to save the value
     *             - ESP_ERR_NVS_REMOVE_FAILED if the value wasn't updated because flash
     *               write operation has failed. The value was written however, and
     *               update will be finished after re-initialization of nvs, provided that
     *               flash operation doesn't fail again.
     *             - ESP_ERR_NVS_VALUE_TOO_LONG if the value is too long
     *             - ESP_FAIL if there is an internal error; most likely due to corrupted
     *               NVS partition (only if NVS assertion checks are disabled)
     *             - other error codes from the underlying storage driver
     *
     * @note compare to nvs_set_blob() in nvs.h
     */
    virtual esp_err_t set_blob(const char *key, const void* blob, size_t len) = 0;

    /**
     * @brief      get string value for given key
     *
     * Retrieves the string data of an entry, given its key. If key does not
     * exist, or the requested variable type doesn't match the type which was used
     * when setting a value, an error is returned.
     *
     * In case of any error, out_str is not modified.
     *
     * @param[in]     key        Key name. Maximum length is (NVS_KEY_NAME_MAX_SIZE-1) characters. Shouldn't be empty.
     * @param[out]    out_str    Pointer to the output string buffer.
     * @param[in]     len        The length of the output buffer pointed to by out_str.
     *                           Use get_item_size to query the size of the item beforehand.
     *
     * @return
     *             - ESP_OK if the value was retrieved successfully
     *             - ESP_ERR_NVS_INVALID_HANDLE if the handle has been closed or is NULL
     *             - ESP_ERR_NVS_NOT_FOUND if the requested key doesn't exist
     *             - ESP_ERR_NVS_TYPE_MISMATCH if the type of the stored value doesn't match the requested type
     *             - ESP_ERR_NVS_INVALID_LENGTH if length is not sufficient to store data
     *             - ESP_FAIL if there is an internal error; most likely due to corrupted
     *               NVS partition (only if NVS assertion checks are disabled)
     *             - other error codes from the underlying storage driver
     */
    virtual esp_err_t get_string(const char *key, char* out_str, size_t len) = 0;

    /**
     * @brief      get blob value for given key
     *
     * Retrieves the binary data of an entry, given its key. If key does not
     * exist, or the requested variable type doesn't match the type which was used
     * when setting a value, an error is returned.
     *
     * In case of any error, out_blob is not modified.
     *
     * @param[in]     key        Key name. Maximum length is (NVS_KEY_NAME_MAX_SIZE-1) characters. Shouldn't be empty.
     * @param[out]    out_blob   Pointer to the output blob buffer.
     * @param[in]     len        The length of the output buffer pointed to by out_blob.
     *                           Use get_item_size to query the size of the item beforehand.
     *
     * @return
     *             - ESP_OK if the value was retrieved successfully
     *             - ESP_ERR_NVS_INVALID_HANDLE if handle has been closed or is invalid
     *             - ESP_ERR_NVS_NOT_FOUND if the requested key doesn't exist
     *             - ESP_ERR_NVS_TYPE_MISMATCH if the stored value type does not match the requested type
     *             - ESP_ERR_NVS_INVALID_LENGTH if length is not sufficient to store data
     *             - other error codes from the underlying storage driver
     */
    virtual esp_err_t get_blob(const char *key, void* out_blob, size_t len) = 0;

    /**
     * @brief Look up the size of an entry's data.
     *
     * @param[in]     datatype   Data type to search for.
     * @param[in]     key        Key name. Maximum length is (NVS_KEY_NAME_MAX_SIZE-1) characters. Shouldn't be empty.
     * @param[out]    size       Size of the item, if it exists.
     *                           For strings, this size includes the zero terminator.
     *
     * @return     - ESP_OK if the item with specified type and key exists. Its size will be returned via \c size.
     *             - ESP_ERR_NVS_INVALID_HANDLE if the handle has been closed or is NULL
     *             - ESP_ERR_NVS_NOT_FOUND if an item with the requested key and type doesn't exist
     *             - ESP_ERR_NVS_TYPE_MISMATCH if an item with the requested key exists but has a different type
     *             - other error codes from the underlying storage driver
     */
    virtual esp_err_t get_item_size(ItemType datatype, const char *key, size_t &size) = 0;

    /**
     * @brief Checks whether key exists and optionally returns also data type of associated entry.
     *
     * @param[in]     key        Key name. Maximum length is (NVS_KEY_NAME_MAX_SIZE-1) characters. Shouldn't be empty.
     * @param[out]    nvstype    Nvs data type of entry, if it exists.
     *
     * @return
     *             - ESP_OK if NVS entry for key provided was found. Data type will be returned via nvstype.
     *             - ESP_ERR_NVS_INVALID_HANDLE if handle has been closed or is invalid
     *             - ESP_ERR_NVS_NOT_FOUND if the requested key doesn't exist
     *             - ESP_FAIL if there is an internal error; most likely due to corrupted
     *               NVS partition (only if NVS assertion checks are disabled)
     *             - other error codes from the underlying storage driver
     */
    virtual esp_err_t find_key(const char* key, nvs_type_t &nvstype) = 0;

    /**
     * @brief Erases an entry.
     *
     * @param[in] key Key name. Maximum length is (NVS_KEY_NAME_MAX_SIZE-1) characters. Shouldn't be empty.
     *
     * @return
     *             - ESP_OK if erase operation was successful
     *             - ESP_ERR_NVS_INVALID_HANDLE if the handle has been closed or is NULL
     *             - ESP_ERR_NVS_READ_ONLY if storage handle was opened as read only
     *             - ESP_ERR_NVS_NOT_FOUND if the requested key doesn't exist
     *             - ESP_FAIL if there is an internal error; most likely due to corrupted
     *               NVS partition (only if NVS assertion checks are disabled)
     *             - other error codes from the underlying storage driver
     */
    virtual esp_err_t erase_item(const char* key) = 0;

    /**
     * @brief Erases all entries in the scope of this handle.
     *
     * The scope may vary, depending on the implementation (typically the opened namespace).
     *
     * @note If you want to erase the whole NVS flash partition, use nvs_flash_erase() /
     *       nvs_flash_erase_partition() instead.
     *
     * @return
     *             - ESP_OK if erase operation was successful
     *             - ESP_ERR_NVS_INVALID_HANDLE if the handle has been closed or is NULL
     *             - ESP_ERR_NVS_READ_ONLY if storage handle was opened as read only
     *             - other error codes from the underlying storage driver
     */
    virtual esp_err_t erase_all() = 0;

    /**
     * @brief Purges all erased entries in the scope of this handle.
     *
     * The scope may vary, depending on the implementation.
     *
     * @return
     *             - ESP_OK if purge operation was successful
     *             - ESP_ERR_NVS_INVALID_HANDLE if the handle has been closed or is NULL
     *             - ESP_ERR_NVS_READ_ONLY if storage handle was opened as read only
     *             - other error codes from the underlying storage driver
     */
    virtual esp_err_t purge_all() = 0;

    /**
     * @brief Commits all changes done through this handle so far.
     *
     * Currently, NVS writes to storage right after the set and get functions,
     * but this is not guaranteed.
     *
     * @return
     *             - ESP_OK if commit was successful
     *             - ESP_ERR_NVS_INVALID_HANDLE if the handle has been closed or is NULL
     */
    virtual esp_err_t commit() = 0;

    /**
     * @brief      Calculate all entries in the scope of the handle.
     *
     * @param[out]  usedEntries Returns amount of used entries from a namespace on success.
     *
     * @return
     *             - ESP_OK if the used entry count has been calculated successfully.
     *               Return param usedEntries will be filled with a valid value.
     *             - ESP_ERR_NVS_INVALID_HANDLE if the handle has been closed or is NULL.
     *               Return param usedEntries will be filled with 0.
     *             - ESP_ERR_NVS_NOT_INITIALIZED if the storage driver is not initialized.
     *               Return param usedEntries will be filled with 0.
     *             - Other error codes from the underlying storage driver.
     *               Return param usedEntries will be filled with 0.
     */
    virtual esp_err_t get_used_entry_count(size_t& usedEntries) = 0;

protected:
    /**
     * @brief Set an item of a given datatype.
     *
     * @param[in] datatype Item data type.
     * @param[in] key      Key name.
     * @param[in] data     Pointer to the value to store.
     * @param[in] dataSize Size of the value in bytes.
     *
     * @return esp_err_t result codes matching set_item / set_blob.
     */
    virtual esp_err_t set_typed_item(ItemType datatype, const char *key, const void* data, size_t dataSize) = 0;

    /**
     * @brief Get an item of a given datatype.
     *
     * @param[in]  datatype Item data type.
     * @param[in]  key      Key name.
     * @param[out] data     Pointer to the output buffer.
     * @param[in]  dataSize Size of the output buffer in bytes.
     *
     * @return esp_err_t result codes matching get_item / get_blob.
     */
    virtual esp_err_t get_typed_item(ItemType datatype, const char *key, void* data, size_t dataSize) = 0;
};

/**
 * @brief Opens non-volatile storage and returns a handle object.
 *
 * The handle is automatically closed on destruction. The scope of the handle is the namespace ns_name
 * in a particular partition partition_name.
 * The parameters partition_name, ns_name and open_mode have the same meaning and restrictions as the parameters
 * part_name, namespace_name and open_mode in nvs_open_from_partition(), respectively.
 *
 * @param[in]  partition_name Label of the NVS partition to open.
 * @param[in]  ns_name        Namespace name. Maximum length is (NVS_KEY_NAME_MAX_SIZE-1) characters.
 * @param[in]  open_mode      NVS_READONLY, NVS_READWRITE, or NVS_READWRITE_PURGE.
 * @param[out] err            Optional pointer to an esp_err_t result of the open operation, having the same meaning
 *                            as the return value of nvs_open_from_partition():
 *             - ESP_OK if storage handle was opened successfully
 *             - ESP_ERR_INVALID_ARG if partition_name or ns_name is NULL
 *             - ESP_ERR_NVS_NOT_INITIALIZED if the storage driver is not initialized
 *             - ESP_ERR_NVS_PART_NOT_FOUND if the partition with the specified name is not found
 *             - ESP_ERR_NVS_NOT_FOUND if namespace doesn't exist yet and
 *               mode is NVS_READONLY
 *             - ESP_ERR_NVS_KEY_TOO_LONG if the namespace name is longer than (NVS_KEY_NAME_MAX_SIZE-1) characters
 *             - ESP_ERR_NOT_ALLOWED if the NVS partition is read-only and mode is NVS_READWRITE
 *             - ESP_ERR_NVS_NOT_ENOUGH_SPACE if there is no space for a new entry or there are too many
 *               different namespaces (maximum allowed different namespaces: 254)
 *             - ESP_ERR_NO_MEM if memory could not be allocated for the internal structures
 *             - other error codes from the underlying storage driver
 *
 * @return unique pointer of an nvs handle on success, an empty unique pointer otherwise
 */
std::unique_ptr<NVSHandle> open_nvs_handle_from_partition(const char *partition_name,
        const char *ns_name,
        nvs_open_mode_t open_mode,
        esp_err_t *err = nullptr);

/**
 * @brief Open NVS handle from the default NVS partition.
 *
 * This function does the same as open_nvs_handle_from_partition() but uses the default nvs partition
 * instead of a partition_name parameter.
 *
 * @param[in]  ns_name   Namespace name. Maximum length is (NVS_KEY_NAME_MAX_SIZE-1) characters.
 * @param[in]  open_mode NVS_READONLY, NVS_READWRITE, or NVS_READWRITE_PURGE.
 * @param[out] err       Optional pointer to an esp_err_t result of the open operation. See
 *                       open_nvs_handle_from_partition() for the list of possible error codes.
 *
 * @return unique pointer of an nvs handle on success, an empty unique pointer otherwise
 */
std::unique_ptr<NVSHandle> open_nvs_handle(const char *ns_name,
        nvs_open_mode_t open_mode,
        esp_err_t *err = nullptr);

// Helper functions for template usage
/**
 * Help to translate all integral types into ItemType.
 */
template<typename T, typename std::enable_if<std::is_integral<T>::value, void*>::type = nullptr>
constexpr ItemType itemTypeOf()
{
    return static_cast<ItemType>(((std::is_signed<T>::value)?0x10:0x00) | sizeof(T));
}

/**
 * Help to translate all enum types into integral ItemType.
 */
template<typename T, typename std::enable_if<std::is_enum<T>::value, int>::type = 0>
constexpr ItemType itemTypeOf()
{
    return static_cast<ItemType>(((std::is_signed<T>::value)?0x10:0x00) | sizeof(T));
}

/**
 * Help to translate IEEE 754 floating-point types into ItemType.
 */
template<typename T, typename std::enable_if<std::is_floating_point<T>::value, char>::type = 0>
constexpr ItemType itemTypeOf()
{
    return static_cast<ItemType>(0x20 | sizeof(T));
}

template<typename T>
constexpr ItemType itemTypeOf(const T&)
{
    return itemTypeOf<T>();
}

// Template Implementations
template<typename T>
esp_err_t NVSHandle::set_item(const char *key, T value) {
    return set_typed_item(itemTypeOf(value), key, &value, sizeof(value));
}

template<typename T>
esp_err_t NVSHandle::get_item(const char *key, T &value) {
    return get_typed_item(itemTypeOf(value), key, &value, sizeof(value));
}

} // nvs
