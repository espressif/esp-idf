/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "nvs_types.hpp"
#include "nvs_page.hpp"
#include "esp_log.h"
#include "esp_rom_crc.h"

#define TAG "nvs"

namespace nvs {
uint32_t Item::calculateCrc32() const
{
    uint32_t result = 0xffffffff;
    const uint8_t* p = reinterpret_cast<const uint8_t*>(this);
    result = esp_rom_crc32_le(result, p + offsetof(Item, nsIndex),
                              offsetof(Item, crc32) - offsetof(Item, nsIndex));
    result = esp_rom_crc32_le(result, p + offsetof(Item, key), sizeof(key));
    result = esp_rom_crc32_le(result, p + offsetof(Item, data), sizeof(data));
    return result;
}

uint32_t Item::calculateCrc32WithoutValue() const
{
    uint32_t result = 0xffffffff;
    const uint8_t* p = reinterpret_cast<const uint8_t*>(this);
    result = esp_rom_crc32_le(result, p + offsetof(Item, nsIndex),
                              offsetof(Item, datatype) - offsetof(Item, nsIndex));
    result = esp_rom_crc32_le(result, p + offsetof(Item, key), sizeof(key));
    result = esp_rom_crc32_le(result, p + offsetof(Item, chunkIndex), sizeof(chunkIndex));
    return result;
}

uint32_t Item::calculateCrc32(const uint8_t* data, size_t size)
{
    uint32_t result = 0xffffffff;
    result = esp_rom_crc32_le(result, data, size);
    return result;
}

bool Item::checkHeaderConsistency(const uint8_t entryIndex) const
{
    // calculate and check the crc32
    if (crc32 != calculateCrc32()) {
        ESP_LOGD(TAG, "CRC32 mismatch for entry %d", entryIndex);
        return false;
    }

    // validate the datatype and check the rest of the header fields
    switch (datatype) {
    // Entries occupying just one entry
    case ItemType::U8:
    case ItemType::I8:
    case ItemType::U16:
    case ItemType::I16:
    case ItemType::U32:
    case ItemType::I32:
    case ItemType::U64:
    case ItemType::I64: {
        if (span != 1) {
            ESP_LOGD(TAG, "Invalid span %u for datatype %#04x", (unsigned int)span, (unsigned int)datatype);
            return false;
        }
        break;
    }

    // Special case for BLOB_IDX
    case ItemType::BLOB_IDX: {
        // span must be 1
        if (span != 1) {
            ESP_LOGD(TAG, "Invalid span %u for BLOB_IDX", (unsigned int)span);
            return false;
        }

        // chunkIndex must be CHUNK_ANY
        if (chunkIndex != CHUNK_ANY) {
            ESP_LOGD(TAG, "Invalid chunk index %u for BLOB_IDX", (unsigned int)chunkIndex);
            return false;
        }

        // check maximum data length
        // the maximal data length is determined by:
        // maximum number of chunks. Chunks are stored in uin8_t, but are logically divided into two "VerOffset" ranges of values (0 based and 128 based)
        // maximum theoretical number of entries in the chunk (Page::ENTRY_COUNT - 1) and the number of bytes entry can store (Page::ENTRY_SIZE)
        const uint32_t maxDataSize = (uint32_t)((UINT8_MAX / 2) * (Page::ENTRY_COUNT - 1) * Page::ENTRY_SIZE);
        if (blobIndex.dataSize > maxDataSize) {
            ESP_LOGD(TAG, "Data size %u bytes exceeds maximum possible size %u bytes for BLOB_IDX", (unsigned int)blobIndex.dataSize, (unsigned int)maxDataSize);
            return false;
        }
        break;
    }

    // Entries with variable length data
    case ItemType::SZ:
    case ItemType::BLOB:
    case ItemType::BLOB_DATA: {
        uint16_t maxAvailableVDataSize;
        uint8_t maxAvailablePageSpan;
        uint8_t spanCalcFromLen;

        // for BLOB_DATA, chunkIndex must NOT be CHUNK_ANY as this value is used to search ALL chunks in findItem
        if (datatype == ItemType::BLOB_DATA) {
            // chunkIndex must not be CHUNK_ANY
            if (chunkIndex == CHUNK_ANY) {
                ESP_LOGD(TAG, "Invalid chunk index %u for BLOB_DATA", (unsigned int)chunkIndex);
                return false;
            }
        }

        // variable length and span checks

        //  based on the entryIndex determine the maximum variable length capacity in bytes to the end of the page
        maxAvailableVDataSize = ((Page::ENTRY_COUNT - entryIndex) - 1) * Page::ENTRY_SIZE;

        // check if the variable data length is not exceeding the maximum capacity available till the end of the page
        if (varLength.dataSize > maxAvailableVDataSize) {
            ESP_LOGD(TAG, "Variable data length %u bytes exceeds page boundary. Maximum calculated from the current entry position within page is %u bytes for datatype %#04x ", (unsigned int)varLength.dataSize, (unsigned int)maxAvailableVDataSize, (unsigned int)datatype);
            return false;
        }

        // based on the entryIndex determine the maximum possible span up to the end of the page
        maxAvailablePageSpan = Page::ENTRY_COUNT - entryIndex;

        // this check ensures no data is read beyond the end of the page
        if (span > maxAvailablePageSpan) {
            ESP_LOGD(TAG, "Span %u exceeds page boundary. Maximum calculated from the current entry position within page is %u for datatype %#04x ", (unsigned int)span, (unsigned int)maxAvailablePageSpan, (unsigned int)datatype);
            return false;
        }

        // here we have both span and varLength.dataSize within the page boundary. Check if these values are consistent
        spanCalcFromLen = (uint8_t)(((size_t) varLength.dataSize + Page::ENTRY_SIZE - 1) / Page::ENTRY_SIZE);
        spanCalcFromLen ++; // add overhead entry

        // this check ensures that the span is equal to the number of entries required to store the data plus the overhead entry
        if (span != spanCalcFromLen) {
            ESP_LOGD(TAG, "Span %i does not match span %u calculated from variable data length %u bytes for datatype %#04x", (unsigned int)span, (unsigned int)spanCalcFromLen, (unsigned int)varLength.dataSize, (unsigned int)datatype);
            return false;
        }
        break;
    }

    // Invalid datatype
    default: {
        ESP_LOGD(TAG, "Invalid datatype %#04x", (unsigned int)datatype);
        return false;
    }
    }
    return true;
}

} // namespace nvs
