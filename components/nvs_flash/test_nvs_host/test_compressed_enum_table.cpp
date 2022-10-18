/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
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
    TEnum1 tmp;
    for (size_t i = 0; i < table.count(); ++i) {
        CHECK(table.get(i, &tmp) == ESP_OK);
        CHECK(tmp == TEnum1::THREE);
    }

    CHECK(table.set(0, TEnum1::ONE) == ESP_OK);
    CHECK(table.set(1, TEnum1::TWO) == ESP_OK);
    CHECK(table.set(2, TEnum1::ZERO) == ESP_OK);
    CHECK(table.set(3, TEnum1::ONE) == ESP_OK);
    CHECK(table.set(4, TEnum1::TWO) == ESP_OK);
    CHECK(table.set(5, TEnum1::ZERO) == ESP_OK);
    CHECK(table.set(6, TEnum1::ONE) == ESP_OK);
    CHECK(table.set(7, TEnum1::TWO) == ESP_OK);
    CHECK(table.set(8, TEnum1::ZERO) == ESP_OK);
    CHECK(table.set(9, TEnum1::ZERO) == ESP_OK);
    CHECK(table.set(10, TEnum1::ONE) == ESP_OK);
    CHECK(table.set(11, TEnum1::TWO) == ESP_OK);
//  table.set(12, ...
    CHECK(table.set(13, TEnum1::ZERO) == ESP_OK);
    CHECK(table.set(14, TEnum1::ONE) == ESP_OK);
    CHECK(table.set(15, TEnum1::TWO) == ESP_OK);

    // b10010011100100001001001001001001
    // h   9   3   9   0   9   2   4   9

    CHECK(table.data()[0] == 0x93909249);
}
