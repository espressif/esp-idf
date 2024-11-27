/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <string.h>
#include "esp_log.h"
#include "nvs_bootloader.h"
#include "nvs_bootloader_private.h"
#include "esp_assert.h"

#include "esp_partition.h"
#include "nvs_constants.h"
#include <esp_rom_crc.h>

static const char* TAG = "nvs_bootloader";
const bool const_is_encrypted = false;

// Static asserts ensuring that the size of the c structures match NVS physical footprint on the flash
ESP_STATIC_ASSERT(sizeof(nvs_bootloader_page_header_t) == NVS_CONST_ENTRY_SIZE, "nvs_bootloader_page_header_t size is not 32 bytes");
ESP_STATIC_ASSERT(sizeof(nvs_bootloader_page_entry_states_t) == NVS_CONST_ENTRY_SIZE, "nvs_bootloader_page_entry_states_t size is not 32 bytes");
ESP_STATIC_ASSERT(sizeof(nvs_bootloader_single_entry_t) == NVS_CONST_ENTRY_SIZE, "nvs_bootloader_single_entry_t size is not 32 bytes");

esp_err_t nvs_bootloader_read(const char* partition_name,
                              const size_t read_list_count,
                              nvs_bootloader_read_list_t read_list[])
{

    /*
    The flow:

    1. validate parameters and if there are any, report errors.
    2. check if the partition exists
    3. read the partition and browse all NVS pages to figure out whether there is any page in the state of "FREEING"
        3.1. if there is a page in the state of "FREEING", then reading from the page marked as "ACTIVE" will be skipped
    4. read entries from pages marked as "FULL" or ("ACTIVE" xor "FREEING") to identify namespace indexes of the requested namespaces
    5. read the requested entries, same skipping of pages in the state of "ACTIVE" as in step 4

    */
    // load input parameters
    ESP_LOGD(TAG, "nvs_bootloader_read called with partition_name: %s, read_list_count: %u", partition_name, (unsigned)read_list_count);

    // Placeholder return value, replace with actual error handling
    esp_err_t ret = ESP_OK;

    // Check if the parameters are valid
    ret = nvs_bootloader_check_parameters(partition_name, read_list_count, read_list);
    if (ret != ESP_OK) {
        ESP_LOGD(TAG, "nvs_bootloader_check_parameters failed");
        return ret;
    }

    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, partition_name);
    if (partition == NULL) {
        ESP_LOGV(TAG, "esp_partition_find_first failed");
        return ESP_ERR_NVS_PART_NOT_FOUND;
    }

    // log the partition details
    ESP_LOGV(TAG, "Partition %s found, size is: %" PRIu32 "", partition->label, partition->size);

    // visit pages to get the number of pages in the state of "ACTIVE" and "FREEING"
    nvs_bootloader_page_visitor_param_get_page_states_t page_states_count = {0};

    ret = nvs_bootloader_visit_pages(partition, nvs_bootloader_page_visitor_get_page_states, (nvs_bootloader_page_visitor_param_t*)&page_states_count);

    if (ret != ESP_OK) {
        ESP_LOGV(TAG, "Failed reading page states");
        return ret;
    }
    if (page_states_count.no_freeing_pages > 1) {
        ESP_LOGV(TAG, "Multiple pages in the state of FREEING");
        return ESP_ERR_INVALID_STATE;
    }
    if (page_states_count.no_active_pages > 1) {
        ESP_LOGV(TAG, "Multiple pages in the state of ACTIVE");
        return ESP_ERR_INVALID_STATE;
    }

    // Here we have at most 1 active page and at most 1 freeing page. If there exists a freeing page, we will skip reading from the active page
    // Visit pages to get the namespace indexes of the requested namespaces
    nvs_bootloader_page_visitor_param_read_entries_t read_entries = { .skip_active_page = page_states_count.no_freeing_pages > 0, .read_list_count = read_list_count, .read_list = read_list };

    // log the visitor parameters
    ESP_LOGV(TAG, "nvs_bootloader_read - visiting pages to get namespace indexes");

    ret = nvs_bootloader_visit_pages(partition, nvs_bootloader_page_visitor_get_namespaces, (nvs_bootloader_page_visitor_param_t*) &read_entries);

    if (ret != ESP_OK) {
        ESP_LOGV(TAG, "nvs_bootloader_page_visitor_get_namespaces failed");
        return ret;
    }

    // log the visitor parameters
    ESP_LOGV(TAG, "nvs_bootloader_read - visiting pages to get key - value pairs");

    // Visit pages to read the requested key - value pairs
    ret = nvs_bootloader_visit_pages(partition, nvs_bootloader_page_visitor_get_key_value_pairs, (nvs_bootloader_page_visitor_param_t*) &read_entries);

    return ret;
}

esp_err_t nvs_bootloader_check_parameters(const char* partition_name,
                                          const size_t read_list_count,
                                          nvs_bootloader_read_list_t read_list[])
{
    // return `ESP_ERR_NVS_INVALID_NAME` if the partition name specified is too long or is null.
    if (partition_name == NULL || strlen(partition_name) > NVS_PART_NAME_MAX_SIZE) {
        ESP_LOGD(TAG, "Invalid argument: partition_name is NULL or too long");
        return ESP_ERR_NVS_INVALID_NAME;
    }
    // immediately return `ESP_ERR_INVALID_ARG` if the read_list is NULL or read_list_count is 0
    if (read_list == NULL || read_list_count == 0) {
        ESP_LOGD(TAG, "Invalid argument: read_list is NULL or read_list_count is 0");
        return ESP_ERR_INVALID_ARG;
    }
    esp_err_t ret = ESP_OK;
    // iterate over all the read_list entries and check if the namespace name is too long or is null,
    // or if the key name is too long or is null,
    // or if the value type is invalid
    // or if the value type is NVS_TYPE_STR and the buffer length is 0 or greater than the maximum allowed size
    for (size_t i = 0; i < read_list_count; i++) {
        // pre-fill the result code with `ESP_ERR_NVS_NOT_FOUND` and namespace index with 0
        read_list[i].result_code = ESP_ERR_NVS_NOT_FOUND;   // default result code during the check_parameters phase
        read_list[i].namespace_index = 0;                   // indicated namespace as not found

        // checks
        if (read_list[i].namespace_name == NULL || strlen(read_list[i].namespace_name) > NVS_NS_NAME_MAX_SIZE - 1) {
            ESP_LOGD(TAG, "Invalid argument: namespace_name is NULL or too long");
            read_list[i].result_code = ESP_ERR_NVS_INVALID_NAME;
        }
        if (read_list[i].key_name == NULL || strlen(read_list[i].key_name) > NVS_KEY_NAME_MAX_SIZE - 1) {
            ESP_LOGD(TAG, "Invalid argument: key_name is NULL or too long");
            read_list[i].result_code = ESP_ERR_NVS_KEY_TOO_LONG;
        }
        if (!NVS_BOOTLOADER_IS_SUPPORTED_TYPE(read_list[i].value_type)) {
            ESP_LOGD(TAG, "Invalid argument: value_type is invalid");
            read_list[i].result_code = ESP_ERR_INVALID_ARG;
        }
        if ((read_list[i].value_type) == NVS_TYPE_STR && (read_list[i].value.str_val.buff_len == 0 || read_list[i].value.str_val.buff_len > NVS_CONST_STR_LEN_MAX_SIZE)) {
            ESP_LOGD(TAG, "Invalid argument: buffer size provided for NVS_TYPE_STR is invalid");
            read_list[i].result_code = ESP_ERR_INVALID_SIZE;
        }
        if ((read_list[i].value_type) == NVS_TYPE_STR && (read_list[i].value.str_val.buff_ptr == NULL)) {
            ESP_LOGD(TAG, "Invalid argument: buffer pointer provided for NVS_TYPE_STR is null");
            read_list[i].result_code = ESP_ERR_INVALID_SIZE;
        }

        // if the individual result code has changed from it's default value ESP_ERR_NVS_NOT_FOUND, set the result code of function to `ESP_ERR_INVALID_ARG`
        if (read_list[i].result_code != ESP_ERR_NVS_NOT_FOUND) {
            ret = ESP_ERR_INVALID_ARG;
        }

        // clear the value placeholder for single entry data types. We do not touch the buffer type placeholder
        if (NVS_BOOTLOADER_TYPE_FITS_SINGLE_ENTRY(read_list[i].value_type)) {
            size_t val_len = sizeof(read_list[i].value);
            memset((void*) &read_list[i].value, 0, val_len);    // clear the value placeholder
        }
    }
    return ret;
}

// we are going to use the visitor pattern to process all NVS pages in three subsequent loops:
// 1. read the page header to figure out the existence of the page in state "FREEING"
// 2. depending on the state of the page, read the entries from the page to figure out the namespace indexes of the requested namespaces
// 3. read the requested key - value pairs from the pages

// visitor function for reading the page header
esp_err_t nvs_bootloader_page_visitor_get_page_states(nvs_bootloader_page_visitor_param_t *visitor_param,
                                                      const esp_partition_t *partition,
                                                      const size_t page_index,
                                                      const nvs_bootloader_page_header_t *page_header)
{
    nvs_bootloader_page_visitor_param_get_page_states_t *param = (nvs_bootloader_page_visitor_param_get_page_states_t*) visitor_param;
    switch (page_header->page_state) {
    case NVS_CONST_PAGE_STATE_ACTIVE:
        param->no_active_pages++;
        break;
    case NVS_CONST_PAGE_STATE_FREEING:
        param->no_freeing_pages++;
        break;
    default:
        break;
    }
    return ESP_OK;
}

// visitor function for reading the namespaces from pages
esp_err_t nvs_bootloader_page_visitor_get_namespaces(nvs_bootloader_page_visitor_param_t *visitor_param,
                                                     const esp_partition_t *partition,
                                                     const size_t page_index,
                                                     const nvs_bootloader_page_header_t *page_header)
{
    // log the visitor parameters
    ESP_LOGV(TAG, "nvs_bootloader_page_visitor_get_namespaces called with page_index: %u", (unsigned)page_index);

    nvs_bootloader_page_visitor_param_read_entries_t *param = (nvs_bootloader_page_visitor_param_read_entries_t*) visitor_param;

    // if the page is in the state of "ACTIVE" and we are skipping the active page, then return immediately
    if (param->skip_active_page && page_header->page_state == NVS_CONST_PAGE_STATE_ACTIVE) {
        return ESP_OK;
    }

    // read entry state map from the page
    WORD_ALIGNED_ATTR nvs_bootloader_page_entry_states_t page_entry_states = {0};

    esp_err_t ret = nvs_bootloader_read_page_entry_states(partition, page_index, &page_entry_states);

    if (ret != ESP_OK) {
        ESP_LOGV(TAG, "Error reading NVS page entry states");
        return ret;
    }

    // iterate over all entries with state written
    // if the entry is namespace entry, then iterate the read_list and populate the namespace index by matching the namespace name
    uint8_t start_index = 0;
    nvs_bootloader_single_entry_t item = {0};

    // repeat finding single entry items on the page until all entries are processed or error occurs
    while (ret == ESP_OK) {
        ret = nvs_bootloader_read_next_single_entry_item(partition, page_index, &page_entry_states, &start_index, &item);

        if (ret != ESP_OK) {
            break;
        }

        // if the entry is a namespace entry, then iterate the read_list and populate the namespace index by matching the namespace name
        if (item.namespace_index == 0 && item.data_type == NVS_TYPE_U8) {

            ESP_LOGV(TAG, "Namespace %s record found. NS index is: %d. Trying to match read list...", item.key, item.data.primitive_type.data[0]);
            for (size_t i = 0; i < param->read_list_count; i++) {
                // if the namespace index is already populated, skip the read list entry
                if (param->read_list[i].namespace_index != 0) {
                    continue;
                }

                if (strncmp(param->read_list[i].namespace_name, item.key, NVS_KEY_NAME_MAX_SIZE) == 0) {
                    param->read_list[i].namespace_index = item.data.primitive_type.data[0];
                    ESP_LOGV(TAG, "Namespace %s found for read item: %u key: %s NS index is: %d", param->read_list[i].namespace_name,(unsigned)i, param->read_list[i].key_name, param->read_list[i].namespace_index);
                }
            }
        }
    }

    if (ret == ESP_ERR_NVS_NOT_FOUND) {
        ret = ESP_OK;
    }
    return ret;
}

// visitor function for reading the key - value pairs from pages
esp_err_t nvs_bootloader_page_visitor_get_key_value_pairs(nvs_bootloader_page_visitor_param_t *visitor_param,
                                                          const esp_partition_t *partition,
                                                          const size_t page_index,
                                                          const nvs_bootloader_page_header_t *page_header)
{
    nvs_bootloader_page_visitor_param_read_entries_t *param = (nvs_bootloader_page_visitor_param_read_entries_t*) visitor_param;

    // if the page is in the state of "ACTIVE" and we are skipping the active page, then return immediately
    if (param->skip_active_page && page_header->page_state == NVS_CONST_PAGE_STATE_ACTIVE) {
        return ESP_OK;
    }

    // read entry state map from the page
    WORD_ALIGNED_ATTR nvs_bootloader_page_entry_states_t page_entry_states = {0};
    esp_err_t ret = nvs_bootloader_read_page_entry_states(partition, page_index, &page_entry_states);

    if (ret != ESP_OK) {
        ESP_LOGV(TAG, "Error reading NVS page entry states");
        return ret;
    }

    // iterate over all entries with state written
    // if the entry is not a namespace entry, then iterate the read_list and populate the value by matching the namespace index, key name and value type
    uint8_t next_index = 0;     // index of the next entry to read, updated to the next entry by the read_next_single_entry_item
    uint8_t current_index = 0;  // index of the actual entry being processed
    nvs_bootloader_single_entry_t item = {0};

    // repeat finding single entry items on the page until all entries are processed or error occurs
    while (ret == ESP_OK) {
        current_index = next_index;
        ret = nvs_bootloader_read_next_single_entry_item(partition, page_index, &page_entry_states, &next_index, &item);

        if (ret != ESP_OK) {
            break;
        }

        ESP_LOGV(TAG, "nvs_bootloader_page_visitor_get_key_value_pairs - read item NS index: %d, key: %s, data type: %d, span: %d. Trying to match read list...", item.namespace_index, item.key, item.data_type, item.span);

        // skip namespace entry
        if (item.namespace_index == 0) {
            continue;
        }

        // iterate the read_list and populate the value by matching the namespace index, key name and value type
        for (size_t i = 0; i < param->read_list_count; i++) {

            // skip non matching namespace indexes
            if (param->read_list[i].namespace_index != item.namespace_index) {
                continue;
            }

            // process only if the result code of the read list is still in the state of "NOT_FOUND"
            if (param->read_list[i].result_code != ESP_ERR_NVS_NOT_FOUND) {
                continue;
            }

            // try to match the key name
            if (strncmp(param->read_list[i].key_name, item.key, NVS_KEY_NAME_MAX_SIZE) != 0) {
                continue;
            }

            // check data type mismatch between the entry and the read_list, if data type requested and found differ, indicate it in the result code and skip the entry
            if (param->read_list[i].value_type != item.data_type) {
                param->read_list[i].result_code = ESP_ERR_NVS_TYPE_MISMATCH;
                continue;
            }

            ESP_LOGV(TAG, "nvs_bootloader_page_visitor_get_key_value_pairs - matched item with read_list index: %u", (unsigned)i);

            // for single entry data types, populate the value directly from the data of item
            if (NVS_BOOTLOADER_TYPE_FITS_SINGLE_ENTRY(item.data_type)) {
                memcpy((void*) &param->read_list[i].value, (void*) item.data.primitive_type.data, NVS_BOOTLOADER_GET_TYPE_LEN(item.data_type));
                param->read_list[i].result_code = ESP_OK;

                // we are done with this entry, continue with the next one
                break;
            }
            // for a string, the data is stored in the entries following the metadata entry in item
            else if (item.data_type == NVS_TYPE_STR) {
                // check if the buffer length is long enough to hold the string data and if not, indicate it with ESP_ERR_INVALID_SIZE
                if (param->read_list[i].value.str_val.buff_len < item.data.var_len_type.size) {
                    param->read_list[i].result_code = ESP_ERR_INVALID_SIZE;
                    // it does not make sense to continue with the next entry, as the the function doesn't support multiple entries for a single key
                    break;
                }

                // to get the actual data, entries at the index current_index + 1 onwards have to be read. If the result is different from ESP_OK, treat the situation as not found - it is technically a mismatch
                esp_err_t ret_str = nvs_bootloader_read_entries_block(partition, page_index, &page_entry_states, current_index + 1, item.data.var_len_type.size, (uint8_t*)param->read_list[i].value.str_val.buff_ptr);
                if (ret_str != ESP_OK) {
                    // it does not make sense to continue with the next entry, as the data is not found
                    break;
                }

                // calculate the crc32 of the data and compare it with the crc32 stored in the item. If they do not match, treat the situation as not found - it is technically a mismatch
                uint32_t calc_crc = esp_rom_crc32_le(0xffffffff, (uint8_t*)(param->read_list[i].value.str_val.buff_ptr), item.data.var_len_type.size);

                if (calc_crc != item.data.var_len_type.crc32) {
                    ESP_LOGV(TAG, "CRC32 of string data failed");
                    // it does not make sense to continue with the next entry, as the data is not found
                    break;
                }

                // set the result code to ESP_OK
                param->read_list[i].result_code = ESP_OK;
            }
        }
    }

    if (ret == ESP_ERR_NVS_NOT_FOUND) {
        ret = ESP_OK;
    }
    return ret;
}

// visitor pattern for processing of all NVS pages of type "FULL" or "ACTIVE" xor "FREEING"
esp_err_t nvs_bootloader_visit_pages(const esp_partition_t *partition,
                                     nvs_bootloader_page_visitor_t visitor,
                                     nvs_bootloader_page_visitor_param_t *visitor_param)
{
    esp_err_t ret = ESP_OK;
    WORD_ALIGNED_ATTR nvs_bootloader_page_header_t page_header = {0}; // has to be 32 bit aligned

    for (size_t page_index = 0; page_index < (partition->size / NVS_CONST_PAGE_SIZE); page_index++) {
        // reading the NVS page header
        ret = nvs_bootloader_read_page_header(partition, page_index, &page_header);

        if (ret != ESP_OK) {
            ESP_LOGV(TAG, "Error %04x reading NVS page header of page %u", ret, (unsigned)page_index);
        }

        // just skip page with invalid header as it can be the case of uninitialized page
        if (ret == ESP_ERR_NVS_INVALID_STATE) {
            ret = ESP_OK;
            continue;
        }

        // log the page header
        ESP_LOGV(TAG, "nvs_bootloader_visit_pages - page %u, state: %" PRIu32 "", (unsigned)page_index, page_header.page_state);

        //  skip pages not in states "FULL", "ACTIVE" or "FREEING"
        if (page_header.page_state != NVS_CONST_PAGE_STATE_FULL
                && page_header.page_state != NVS_CONST_PAGE_STATE_ACTIVE
                && page_header.page_state != NVS_CONST_PAGE_STATE_FREEING) {
            continue;
        }

        // visit the page
        ret = visitor(visitor_param, partition, page_index, &page_header);
        if (ret != ESP_OK) {
            ESP_LOGV(TAG, "Error %04x visiting NVS page %u", ret, (unsigned)page_index);
            return ret;
        }
    }

    return ret;
}

esp_err_t nvs_bootloader_read_page_header(const esp_partition_t *partition,
                                          const size_t page_index,
                                          const nvs_bootloader_page_header_t *page_header)
{

    esp_err_t ret = ESP_OK;

    ret = esp_partition_read(partition, page_index * NVS_CONST_PAGE_SIZE + NVS_CONST_PAGE_HEADER_OFFSET, (void*)page_header, sizeof(nvs_bootloader_page_header_t));

    if (ret != ESP_OK) {
        return ret;
    }

    // calculate the CRC32 of the page header
    uint32_t calc_crc = esp_rom_crc32_le(0xffffffff, (uint8_t*)page_header + offsetof(nvs_bootloader_page_header_t, sequence_number), offsetof(nvs_bootloader_page_header_t, crc32) - offsetof(nvs_bootloader_page_header_t, sequence_number));
    if (calc_crc != page_header->crc32) {
        ESP_LOGV(TAG, "CRC32 of page# %u header failed", (unsigned)page_index);
        return ESP_ERR_NVS_INVALID_STATE;
    }

    return ret;
}

esp_err_t nvs_bootloader_read_page_entry_states(const esp_partition_t *partition,
                                                const size_t page_index,
                                                nvs_bootloader_page_entry_states_t *page_entry_states)
{
    // log the read_page_entry_states parameters
    ESP_LOGV(TAG, "nvs_bootloader_page_visitor_get_namespaces - reading page entry states for page %u", (unsigned)page_index);

    return esp_partition_read(partition, page_index * NVS_CONST_PAGE_SIZE + NVS_CONST_PAGE_ENTRY_TABLE_OFFSET, (void*)page_entry_states, sizeof(nvs_bootloader_page_entry_states_t));
}

esp_err_t nvs_bootloader_read_next_single_entry_item(const esp_partition_t *partition,
                                                     const size_t page_index,
                                                     const nvs_bootloader_page_entry_states_t *page_entry_states,
                                                     uint8_t *entry_index,
                                                     nvs_bootloader_single_entry_t *item)
{

    // log parameters
    ESP_LOGV(TAG, "nvs_bootloader_read_next_single_entry_item called with page_index: %u, entry_index: %d", (unsigned)page_index, *entry_index);

    // check if the start index is valid. Not found is signal to the caller that there are no more entries to read
    if (*entry_index >= NVS_CONST_ENTRY_COUNT) {
        return ESP_ERR_NVS_NOT_FOUND;
    }

    esp_err_t ret = ESP_OK;

    // find the next single entry item
    while ((*entry_index) < NVS_CONST_ENTRY_COUNT) {
        // If the entry is written, try to read and process it
        if (NVS_BOOTLOADER_GET_ENTRY_STATE(page_entry_states, *entry_index) == NVS_CONST_ENTRY_STATE_WRITTEN) {

            ret = esp_partition_read(partition, page_index * NVS_CONST_PAGE_SIZE + NVS_CONST_PAGE_ENTRY_DATA_OFFSET + (*entry_index) * NVS_CONST_ENTRY_SIZE, (void*)item, sizeof(nvs_bootloader_single_entry_t));

            if (ret != ESP_OK) {
                return ret;
            }

            // only look at item with consistent header
            if (nvs_bootloader_check_item_header_consistency(item, *entry_index)) {
                // advance the start index
                (*entry_index) += item->span;

                // if the item is single entry value data type, return ESP_OK
                if (NVS_BOOTLOADER_IS_SUPPORTED_TYPE(item->data_type)) {
                    return ESP_OK;
                }
            }
            // inconsistent header, try next entry
            else {
                (*entry_index)++;
            }

        }
        // if the entry is not written just try next entry
        else {
            (*entry_index)++;
        }
    }

    return ESP_ERR_NVS_NOT_FOUND;
}

esp_err_t nvs_bootloader_read_entries_block(const esp_partition_t *partition,
                                            const size_t page_index,
                                            const nvs_bootloader_page_entry_states_t *entry_states,
                                            const size_t entry_index,
                                            const size_t block_len,
                                            uint8_t *block)
{
    // log parameters
    ESP_LOGV(TAG, "nvs_bootloader_read_entries_block called with page_index: %u, entry_index: %u, block_len: %u", (unsigned)page_index, (unsigned)entry_index, (unsigned)block_len);

    esp_err_t ret = ESP_ERR_INVALID_STATE;

    do {
        // check if the start index is valid
        if (entry_index >= NVS_CONST_ENTRY_COUNT) {
            ESP_LOGV(TAG, "entry index is beyond the page boundary");
            break;
        }

        // calculate the number of entries necessary to read the block
        size_t number_of_entries = block_len / NVS_CONST_ENTRY_SIZE;
        if (block_len % NVS_CONST_ENTRY_SIZE != 0) {
            number_of_entries++;
        }

        // check if the highest entry to be read still fits the page size
        if (entry_index + number_of_entries > NVS_CONST_ENTRY_COUNT) {
            ESP_LOGV(TAG, "block length exceeds the page boundary");
            break;
        }

        // check if all entries are in the state written
        bool all_entries_written = true;
        for (size_t i = 0; i < number_of_entries; i++) {
            if (NVS_BOOTLOADER_GET_ENTRY_STATE(entry_states, entry_index + i) != NVS_CONST_ENTRY_STATE_WRITTEN) {
                ESP_LOGV(TAG, "some entry is not in the state written");
                all_entries_written = false;
                break;
            }
        }
        if(!all_entries_written) {
            break;
        }

        ret = ESP_OK;
    } while (false);

    if (ret != ESP_OK) {
        return ret;
    }

    size_t data_offset = page_index * NVS_CONST_PAGE_SIZE + NVS_CONST_PAGE_ENTRY_DATA_OFFSET + entry_index * NVS_CONST_ENTRY_SIZE ;

    return esp_partition_read(partition, data_offset, block, block_len);
}

// validates item's header
// CRC is checked
// zero span is checked
// span is validated using the data type
// span is checked not to exceed the remaining number of entries in the page
bool nvs_bootloader_check_item_header_consistency(const nvs_bootloader_single_entry_t *item,
                                                  const uint8_t entry_index)
{

    // check crc of item
    uint32_t calc_crc = esp_rom_crc32_le(0xffffffff, (uint8_t*)item + offsetof(nvs_bootloader_single_entry_t, namespace_index), offsetof(nvs_bootloader_single_entry_t, crc32) - offsetof(nvs_bootloader_single_entry_t, namespace_index));
    calc_crc = esp_rom_crc32_le(calc_crc, (uint8_t*)item + offsetof(nvs_bootloader_single_entry_t, key), sizeof(item->key));
    calc_crc = esp_rom_crc32_le(calc_crc, (uint8_t*)item + offsetof(nvs_bootloader_single_entry_t, data), sizeof(item->data));

    bool ret = false;

    do {
        if (calc_crc != item->crc32) {
            ESP_LOGV(TAG, "Header CRC32 failed for item with key %s", item->key);
            break;
        }

        // check zero span
        if (item->span == 0) {
            ESP_LOGV(TAG, "Span of key %s is zero", item->key);
            break;
        }

        // check span according to the data type
        if (NVS_BOOTLOADER_TYPE_FITS_SINGLE_ENTRY(item->data_type) && item->span != 1) {
            ESP_LOGV(TAG, "Span of single entry key %s is not 1", item->key);
            break;
        }

        // check span according to the remaining space in the page
        if ((uint16_t)entry_index + (uint16_t)(item->span) > NVS_CONST_ENTRY_COUNT) {
            ESP_LOGV(TAG, "Span of key %s exceeds remaining page space", item->key);
            break;
        }

        // check span for string data type. The span has to be able to hold the data length but should not occupy more than necessary entries
        if (item->data_type == NVS_TYPE_STR) {
            if(    ((item->span - 1) * NVS_CONST_ENTRY_SIZE < item->data.var_len_type.size)
                || ((item->span - 1) * NVS_CONST_ENTRY_SIZE > item->data.var_len_type.size + NVS_CONST_ENTRY_SIZE - 1) ) {
                ESP_LOGV(TAG, "Span %u of key %s for string type doesn't match indicated string length %u", (unsigned)item->span, item->key, item->data.var_len_type.size);
                break;
            }
        }
        ret = true;
    } while (false);

    return ret;
}
