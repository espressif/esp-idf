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
#ifndef nvs_types_h
#define nvs_types_h

#include <cstdint>
#include <type_traits>
#include <cstring>
#include <cassert>
#include <algorithm>
#include "nvs.h"
#include "compressed_enum_table.hpp"
#include "string.h"

using namespace std;

namespace nvs
{

enum class ItemType : uint8_t {
    U8   = NVS_TYPE_U8,
    I8   = NVS_TYPE_I8,
    U16  = NVS_TYPE_U16,
    I16  = NVS_TYPE_I16,
    U32  = NVS_TYPE_U32,
    I32  = NVS_TYPE_I32,
    U64  = NVS_TYPE_U64,
    I64  = NVS_TYPE_I64,
    SZ   = NVS_TYPE_STR,
    BLOB = 0x41,
    BLOB_DATA = NVS_TYPE_BLOB,
    BLOB_IDX  = 0x48,
    ANY  = NVS_TYPE_ANY
};

enum class VerOffset: uint8_t {
    VER_0_OFFSET = 0x0,
    VER_1_OFFSET = 0x80,
    VER_ANY = 0xff,
};

template<typename T, typename std::enable_if<std::is_integral<T>::value, void*>::type = nullptr>
constexpr ItemType itemTypeOf()
{
    return static_cast<ItemType>(((std::is_signed<T>::value)?0x10:0x00) | sizeof(T));
}

template<typename T>
constexpr ItemType itemTypeOf(const T&)
{
    return itemTypeOf<T>();
}

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
            char     key[16];
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
    void getValue(T& dst)
    {
        assert(itemTypeOf(dst) == datatype);
        dst = *reinterpret_cast<T*>(data);
    }
};

} // namespace nvs

#endif /* nvs_types_h */
