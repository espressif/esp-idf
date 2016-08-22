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

#ifndef compressed_enum_table_h
#define compressed_enum_table_h

#include <cstdint>
#include <cassert>
#include <type_traits>

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

    Tenum get(size_t index) const
    {
        assert(index >= 0 && index < Nitems);
        size_t wordIndex = index / ITEMS_PER_WORD;
        size_t offset = (index % ITEMS_PER_WORD) * Nbits;

        return static_cast<Tenum>((mData[wordIndex] >> offset) & VALUE_MASK);
    }

    void set(size_t index, Tenum val)
    {
        assert(index >= 0 && index < Nitems);
        size_t wordIndex = index / ITEMS_PER_WORD;
        size_t offset = (index % ITEMS_PER_WORD) * Nbits;

        uint32_t v = static_cast<uint32_t>(val) << offset;
        mData[wordIndex] = (mData[wordIndex] & ~(VALUE_MASK << offset)) | v;
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
