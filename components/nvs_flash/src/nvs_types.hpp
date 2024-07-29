/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef nvs_types_h
#define nvs_types_h

#include <cstdint>
#include <cstring>
#include <cassert>
#include <algorithm>
#include "nvs.h"
#include "nvs_handle.hpp"
#include "compressed_enum_table.hpp"
#include "string.h"

using namespace std;

namespace nvs
{

/**
 * Used to recognize transient states of a blob. Once a blob is modified, new chunks with the new data are written
 * with a new version. The version is saved in the highest bit of Item::chunkIndex as well as in
 * Item::blobIndex::chunkStart.
 * If a chunk is modified and hence re-written, the version swaps: 0x0 -> 0x80 or 0x80 -> 0x0.
 */
enum class VerOffset: uint8_t {
    VER_0_OFFSET = 0x0,
    VER_1_OFFSET = 0x80,
    VER_ANY = 0xff,
};

inline bool isVariableLengthType(ItemType type)
{
    return (type == ItemType::BLOB ||
            type == ItemType::SZ ||
            type == ItemType::BLOB_DATA);
}

class Item
{
public:
    union {
        struct {
            uint8_t  nsIndex;
            ItemType datatype;
            uint8_t  span;
            uint8_t  chunkIndex;
            uint32_t crc32;
            char     key[NVS_KEY_NAME_MAX_SIZE];
            union {
                struct {
                    uint16_t dataSize;
                    uint16_t reserved;
                    uint32_t dataCrc32;
                } varLength;
                struct {
                    uint32_t   dataSize;
                    uint8_t    chunkCount; // Number of children data blobs.
                    VerOffset  chunkStart; // Offset from which the chunkIndex for children blobs starts
                    uint16_t   reserved;
                } blobIndex;
                uint8_t data[8];
            };
        };
        uint8_t rawData[32];
    };

    static const size_t MAX_KEY_LENGTH = sizeof(key) - 1;

    // 0xff cannot be used as a valid chunkIndex for blob datatype.
    static const uint8_t CHUNK_ANY = 0xff;


    Item(uint8_t nsIndex, ItemType datatype, uint8_t span, const char* key_, uint8_t chunkIdx = CHUNK_ANY)
        : nsIndex(nsIndex), datatype(datatype), span(span), chunkIndex(chunkIdx)
    {
        std::fill_n(reinterpret_cast<uint32_t*>(key),  sizeof(key)  / 4, 0xffffffff);
        std::fill_n(reinterpret_cast<uint32_t*>(data), sizeof(data) / 4, 0xffffffff);
        if (key_) {
            strncpy(key, key_, sizeof(key) - 1);
            key[sizeof(key) - 1] = 0;
        } else {
            key[0] = 0;
        }
    }

    Item()
    {
    }

    uint32_t calculateCrc32() const;
    uint32_t calculateCrc32WithoutValue() const;
    static uint32_t calculateCrc32(const uint8_t* data, size_t size);

    void getKey(char* dst, size_t dstSize)
    {
        strncpy(dst, key, min(dstSize, sizeof(key)));
        dst[dstSize-1] = 0;
    }

    template<typename T>
    esp_err_t getValue(T& dst)
    {
        NVS_ASSERT_OR_RETURN(itemTypeOf(dst) == datatype, ESP_FAIL);
        dst = *reinterpret_cast<T*>(data);
        return ESP_OK;
    }

    // Returns true if item's header:
    // crc32 matches the calculated crc32
    // and datatype is one of the supported types
    // and span is within the allowed range for the datatype and below the maximum calculated from the entryIndex
    //
    // Parameter entryIndex is used to calculate the maximum span for the given entry
    bool checkHeaderConsistency(const uint8_t entryIndex) const;
};

} // namespace nvs

#endif /* nvs_types_h */
