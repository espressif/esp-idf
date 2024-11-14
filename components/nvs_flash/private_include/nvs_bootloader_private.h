/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "nvs_constants.h"  // NVS_CONST_ENTRY_SIZE and all size related constants shared with cpp implementation of NVS
#include "nvs_bootloader.h" // nvs_bootloader_read_list_t and function prototypes
#include "esp_partition.h"  // esp_partition_t

#ifdef __cplusplus
extern "C" {
#endif

// evaluates to true for NVS types fitting single NVS entry. At the moment all NVS_TYPE_U* or NVS_TYPE_I* */
#define NVS_BOOTLOADER_TYPE_FITS_SINGLE_ENTRY(data_type) \
(   (data_type == NVS_TYPE_U8) \
  ||(data_type == NVS_TYPE_I8) \
  ||(data_type == NVS_TYPE_U16) \
  ||(data_type == NVS_TYPE_I16) \
  ||(data_type == NVS_TYPE_U32) \
  ||(data_type == NVS_TYPE_I32) \
  ||(data_type == NVS_TYPE_U64) \
  ||(data_type == NVS_TYPE_I64) \
)

// evaluates to true for NVS types supported by the nvs bootloader code*/
#define NVS_BOOTLOADER_IS_SUPPORTED_TYPE(data_type) \
(   (data_type == NVS_TYPE_STR) || NVS_BOOTLOADER_TYPE_FITS_SINGLE_ENTRY(data_type) \
)

// returns length of supported data field in bytes
#define NVS_BOOTLOADER_GET_TYPE_LEN(data_type) \
(   (data_type == NVS_TYPE_U8) ? 1 \
  :(data_type == NVS_TYPE_I8) ? 1 \
  :(data_type == NVS_TYPE_U16) ? 2 \
  :(data_type == NVS_TYPE_I16) ? 2 \
  :(data_type == NVS_TYPE_U32) ? 4 \
  :(data_type == NVS_TYPE_I32) ? 4 \
  :(data_type == NVS_TYPE_U64) ? 8 \
  :(data_type == NVS_TYPE_I64) ? 8 \
  :0 \
)

esp_err_t nvs_bootloader_check_parameters(const char* partition_name,
                                         const size_t read_list_count,
                                         nvs_bootloader_read_list_t read_list[]);

// NVS page header structure
typedef struct {
    uint32_t page_state;            // page state
    uint32_t sequence_number;       // sequence number of this page
    uint8_t  nvs_format_version;    // nvs format version
    uint8_t  reserved[19];          // unused, must be 0xff
    uint32_t crc32;                 // crc32 of everything except page_state
} nvs_bootloader_page_header_t;

// NVS page entry state array structure
// Each entry state is represented by two bits
// There are 126 entries in a page
// The last 4 bits are reserved
typedef struct {
    uint8_t entry_states[NVS_CONST_ENTRY_SIZE];
} nvs_bootloader_page_entry_states_t;

// Macro to extract the state of an entry from the entry state structure
#define NVS_BOOTLOADER_GET_ENTRY_STATE(page_entry_states, entry_index) \
    ((page_entry_states->entry_states[(entry_index) / 4] >> (((entry_index) % 4) * 2)) & 0x03)

typedef struct {
    uint8_t data[8];
} nvs_bootloader_entry_data_primitive_type_t;

typedef struct {
    uint16_t size;
    uint16_t reserved;
    uint32_t crc32;
} nvs_bootloader_entry_data_var_len_type_t;

typedef union {
    nvs_bootloader_entry_data_primitive_type_t primitive_type;
    nvs_bootloader_entry_data_var_len_type_t var_len_type;
} nvs_bootloader_entry_data_t;

// NVS single entry item definition
typedef struct {
    uint8_t namespace_index;            // namespace index
    uint8_t data_type;                  // data type
    uint8_t span;                       // span
    uint8_t chunk_index;                // chunk index
    uint32_t crc32;                     // crc32 of the entry
    char key[NVS_KEY_NAME_MAX_SIZE];    // key
    nvs_bootloader_entry_data_t data;   // data, interpreted based on data_type
} nvs_bootloader_single_entry_t;

#define MAX_KEY_LENGTH = (NVS_KEY_NAME_MAX_SIZE - 1);

// read a single entry item from a page
// only reads entries marked as written
// skips items spanning multiple entries
// entry_index is the index of the first entry to read
// item is the output parameter
// entry_index is updated to the index of the next entry
// returns ESP_ERR_NOT_FOUND if no more entries are found
// returns ESP_OK if an entry is found
// returns other error codes if an error occurs
esp_err_t nvs_bootloader_read_next_single_entry_item(const esp_partition_t *partition, const size_t page_index, const nvs_bootloader_page_entry_states_t *entry_states, uint8_t *entry_index, nvs_bootloader_single_entry_t *item);

// read a block of data from the entries from a page
// the block is read starting from the offset specified as entry_index of entry
// the length of the block is specified in bytes in parameter block_len
// the block is read into the buffer pointed to by block
// before accessing the entries, the entry states are checked and the entries are read only if they are marked as written
// if any of the entries that needs to be read is not in the state written, the function returns ESP_ERR_INVALID_STATE
// if the block is not fully read after reaching end of the page, the function returns ESP_ERR_INVALID_STATE
// if the block is fully read, the function returns ESP_OK
esp_err_t nvs_bootloader_read_entries_block(const esp_partition_t *partition, const size_t page_index, const nvs_bootloader_page_entry_states_t *entry_states, const size_t entry_index, const size_t block_len, uint8_t *block);

// visitor function parameters for reading the page header
typedef struct {
    size_t no_active_pages;
    size_t no_freeing_pages;
} nvs_bootloader_page_visitor_param_get_page_states_t;

// visitor function parameters for reading the namespaces from pages and the key - value pairs from pages
typedef struct {
    const bool skip_active_page;
    const size_t read_list_count;
    nvs_bootloader_read_list_t *read_list;
} nvs_bootloader_page_visitor_param_read_entries_t;

// visitor function parameter union type declaration
typedef union {
    nvs_bootloader_page_visitor_param_get_page_states_t *get_page_states;
    nvs_bootloader_page_visitor_param_read_entries_t *read_entries;
} nvs_bootloader_page_visitor_param_t;

// visitor function type declaration
typedef esp_err_t (*nvs_bootloader_page_visitor_t)(nvs_bootloader_page_visitor_param_t *visitor_param,
                                                   const esp_partition_t *partition,
                                                   const size_t page_index,
                                                   const nvs_bootloader_page_header_t *page_header);

// visit all pages in the partition
esp_err_t nvs_bootloader_visit_pages(const esp_partition_t *partition,
                                     nvs_bootloader_page_visitor_t visitor,
                                     nvs_bootloader_page_visitor_param_t *visitor_param);

// read page header
esp_err_t nvs_bootloader_read_page_header(const esp_partition_t *partition,
                                          const size_t page_index,
                                          const nvs_bootloader_page_header_t *page_header);

// read entry states from a page
esp_err_t nvs_bootloader_read_page_entry_states(const esp_partition_t *partition,
                                                const size_t page_index,
                                                nvs_bootloader_page_entry_states_t *entry_states);

// populates the page header for a given page
esp_err_t nvs_bootloader_page_visitor_get_page_states(nvs_bootloader_page_visitor_param_t *visitor_param,
                                                      const esp_partition_t *partition,
                                                      const size_t page_index,
                                                      const nvs_bootloader_page_header_t *page_header);

// populates the namespace indexes found on a page
esp_err_t nvs_bootloader_page_visitor_get_namespaces(nvs_bootloader_page_visitor_param_t *visitor_param,
                                                     const esp_partition_t *partition,
                                                     const size_t page_index,
                                                     const nvs_bootloader_page_header_t *page_header);

// populates the key value pairs found on a page
esp_err_t nvs_bootloader_page_visitor_get_key_value_pairs(nvs_bootloader_page_visitor_param_t *visitor_param,
                                                          const esp_partition_t *partition,
                                                          const size_t page_index,
                                                          const nvs_bootloader_page_header_t *page_header);

// check if the item header is consistent - crc32 matches, namespace index is valid, data type is valid and other fields are consistent
bool nvs_bootloader_check_item_header_consistency(const nvs_bootloader_single_entry_t *item,
                                                  const uint8_t entry_index);
#ifdef __cplusplus
}
#endif
