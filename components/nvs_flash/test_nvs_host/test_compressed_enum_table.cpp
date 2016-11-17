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
#include "catch.hpp"
#include "compressed_enum_table.hpp"
#include <cstring>


TEST_CASE("test if CompressedEnumTable works as expected", "[enumtable]")
{

    enum class TEnum1 : uint32_t {
        ZERO = 0,
        ONE  = 1,
        TWO  = 2,
        THREE = 3,
    };
    CompressedEnumTable<TEnum1, 2, 252> table;
    memset(table.data(), 0xff, table.byteSize());
    for (size_t i = 0; i < table.count(); ++i) {
        CHECK(table.get(i) == TEnum1::THREE);
    }

    table.set(0, TEnum1::ONE);
    table.set(1, TEnum1::TWO);
    table.set(2, TEnum1::ZERO);
    table.set(3, TEnum1::ONE);
    table.set(4, TEnum1::TWO);
    table.set(5, TEnum1::ZERO);
    table.set(6, TEnum1::ONE);
    table.set(7, TEnum1::TWO);
    table.set(8, TEnum1::ZERO);
    table.set(9, TEnum1::ZERO);
    table.set(10, TEnum1::ONE);
    table.set(11, TEnum1::TWO);
//  table.set(12, ...
    table.set(13, TEnum1::ZERO);
    table.set(14, TEnum1::ONE);
    table.set(15, TEnum1::TWO);

    // b10010011100100001001001001001001
    // h   9   3   9   0   9   2   4   9

    CHECK(table.data()[0] == 0x93909249);

}