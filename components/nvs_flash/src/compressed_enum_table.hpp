/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef compressed_enum_table_h
#define compressed_enum_table_h

#include <cstdint>
#include <cassert>
#include <type_traits>
#include "nvs_internal.h"
#include "esp_err.h"

template<typename Tenum, size_t Nbits, size_t Nitems>
class CompressedEnumTable
{
public:
    uint32_t* data()
    {
        return mData;
    }

    const uint32_t* data() const
    {
        return mData;
    }

    esp_err_t get(size_t index, Tenum *retval) const
    {
        NVS_ASSERT_OR_RETURN(index < Nitems, ESP_FAIL);
        size_t wordIndex = index / ITEMS_PER_WORD;
        size_t offset = (index % ITEMS_PER_WORD) * Nbits;

        *retval = static_cast<Tenum>((mData[wordIndex] >> offset) & VALUE_MASK);
        return ESP_OK;
    }

    esp_err_t set(size_t index, Tenum val)
    {
        NVS_ASSERT_OR_RETURN(index < Nitems, ESP_FAIL);
        size_t wordIndex = index / ITEMS_PER_WORD;
        size_t offset = (index % ITEMS_PER_WORD) * Nbits;

        uint32_t v = static_cast<uint32_t>(val) << offset;
        mData[wordIndex] = (mData[wordIndex] & ~(VALUE_MASK << offset)) | v;
        return ESP_OK;
    }

    static constexpr size_t getWordIndex(size_t index)
    {
        return index / ITEMS_PER_WORD;
    }

    static constexpr size_t byteSize()
    {
        return WORD_COUNT * 4;
    }

    static constexpr size_t count()
    {
        return Nitems;
    }


protected:
    static_assert(32 % Nbits == 0, "Nbits must divide 32");
    static const size_t ITEMS_PER_WORD = 32 / Nbits;
    static const size_t WORD_COUNT = ( Nbits * Nitems + 31 ) / 32;
    static const uint32_t VALUE_MASK = (1 << Nbits) - 1;
    uint32_t mData[WORD_COUNT];
};

#endif /* compressed_enum_table_h */
