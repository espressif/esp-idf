/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "catch.hpp"
#include "spi_flash_mmap.h"
#include "esp_partition.h"
#include "spi_flash_emulation.h"
#include <functional>

template <typename Tit>
bool range_empty_n(Tit it_begin, size_t n)
{
    return std::all_of(it_begin, it_begin + n, bind(std::equal_to<uint32_t>(), std::placeholders::_1, 0xffffffff));
}

struct FlashEmuFixture {
    FlashEmuFixture(size_t sectors) : esp_part(), emu(sectors) { }

    esp_partition_t esp_part;
    SpiFlashEmulator emu;
};

TEST_CASE("flash starts with all bytes == 0xff", "[spi_flash_emu]")
{
    FlashEmuFixture f(4);

    uint8_t sector[SPI_FLASH_SEC_SIZE];

    for (int i = 0; i < 4; ++i) {
        CHECK(esp_partition_read(&f.esp_part, 0, sector, sizeof(sector)) == ESP_OK);
        for (auto v: sector) {
            CHECK(v == 0xff);
        }
    }
}

TEST_CASE("invalid writes are checked", "[spi_flash_emu]")
{
    FlashEmuFixture f(1);

    uint32_t val = 0;
    CHECK(esp_partition_write(&f.esp_part, 0, &val, 4) == ESP_OK);
    val = 1;
    CHECK(esp_partition_write(&f.esp_part, 0, &val, 4) == ESP_ERR_FLASH_OP_FAIL);
}


TEST_CASE("out of bounds writes fail", "[spi_flash_emu]")
{
    FlashEmuFixture f(4);
    uint32_t vals[8];
    std::fill_n(vals, 8, 0);
    CHECK(esp_partition_write(&f.esp_part, 0, &vals, sizeof(vals)) == ESP_OK);

    CHECK(esp_partition_write(&f.esp_part, 4*4096 - sizeof(vals), &vals, sizeof(vals)) == ESP_OK);

    CHECK(esp_partition_write(&f.esp_part, 4*4096 - sizeof(vals) + 4, &vals, sizeof(vals)) == ESP_ERR_FLASH_OP_FAIL);
}

TEST_CASE("after erase the sector is set to 0xff", "[spi_flash_emu]")
{
    FlashEmuFixture f(4);
    uint32_t val1 = 0xab00cd12;
    CHECK(esp_partition_write(&f.esp_part, 0, &val1, sizeof(val1)) == ESP_OK);
    uint32_t val2 = 0x5678efab;
    CHECK(esp_partition_write(&f.esp_part, 4096 - 4, &val2, sizeof(val2)) == ESP_OK);

    CHECK(f.emu.words()[0] == val1);
    CHECK(range_empty_n(f.emu.words() + 1, 4096 / 4 - 2));
    CHECK(f.emu.words()[4096 / 4 - 1] == val2);

    CHECK(esp_partition_erase_range(&f.esp_part, 0, SPI_FLASH_SEC_SIZE) == ESP_OK);

    CHECK(f.emu.words()[0] == 0xffffffff);
    CHECK(range_empty_n(f.emu.words() + 1, 4096 / 4 - 2));
    CHECK(f.emu.words()[4096 / 4 - 1] == 0xffffffff);
}

TEST_CASE("EMU raw read function works", "[spi_flash_emu]")
{
    FlashEmuFixture f(4);
    uint32_t value = 0xdeadbeef;
    uint32_t read_value = 0;
    CHECK(esp_partition_write(&f.esp_part, 0, &value, sizeof(value)) == ESP_OK);

    CHECK(esp_partition_read_raw(&f.esp_part, 0, &read_value, sizeof(read_value)) == ESP_OK);

    CHECK(read_value == 0xdeadbeef);
}

TEST_CASE("EMU raw write function works", "[spi_flash_emu]")
{
    FlashEmuFixture f(4);
    uint32_t value = 0xdeadbeef;
    uint32_t read_value = 0;
    CHECK(esp_partition_write_raw(&f.esp_part, 0, &value, sizeof(value)) == ESP_OK);

    CHECK(esp_partition_read(&f.esp_part, 0, &read_value, sizeof(read_value)) == ESP_OK);

    CHECK(read_value == 0xdeadbeef);
}

TEST_CASE("read/write/erase operation times are calculated correctly", "[spi_flash_emu]")
{
    FlashEmuFixture f(1);
    uint8_t data[512];
    esp_partition_read(&f.esp_part, 0, data, 4);
    CHECK(f.emu.getTotalTime() == 7);
    CHECK(f.emu.getReadOps() == 1);
    CHECK(f.emu.getReadBytes() == 4);
    f.emu.clearStats();
    esp_partition_read(&f.esp_part, 0, data, 8);
    CHECK(f.emu.getTotalTime() == 5);
    CHECK(f.emu.getReadOps() == 1);
    CHECK(f.emu.getReadBytes() == 8);
    f.emu.clearStats();
    esp_partition_read(&f.esp_part, 0, data, 16);
    CHECK(f.emu.getTotalTime() == 6);
    CHECK(f.emu.getReadOps() == 1);
    CHECK(f.emu.getReadBytes() == 16);
    f.emu.clearStats();
    esp_partition_read(&f.esp_part, 0, data, 128);
    CHECK(f.emu.getTotalTime() == 18);
    CHECK(f.emu.getReadOps() == 1);
    CHECK(f.emu.getReadBytes() == 128);
    f.emu.clearStats();
    esp_partition_read(&f.esp_part, 0, data, 256);
    CHECK(f.emu.getTotalTime() == 32);
    f.emu.clearStats();
    esp_partition_read(&f.esp_part, 0, data, (128+256)/2);
    CHECK(f.emu.getTotalTime() == (18+32)/2);
    f.emu.clearStats();

    esp_partition_write(&f.esp_part, 0, data, 4);
    CHECK(f.emu.getTotalTime() == 19);
    CHECK(f.emu.getWriteOps() == 1);
    CHECK(f.emu.getWriteBytes() == 4);
    f.emu.clearStats();
    CHECK(f.emu.getWriteOps() == 0);
    CHECK(f.emu.getWriteBytes() == 0);
    esp_partition_write(&f.esp_part, 0, data, 8);
    CHECK(f.emu.getTotalTime() == 23);
    f.emu.clearStats();
    esp_partition_write(&f.esp_part, 0, data, 16);
    CHECK(f.emu.getTotalTime() == 35);
    CHECK(f.emu.getWriteOps() == 1);
    CHECK(f.emu.getWriteBytes() == 16);
    f.emu.clearStats();
    esp_partition_write(&f.esp_part, 0, data, 128);
    CHECK(f.emu.getTotalTime() == 205);
    f.emu.clearStats();
    esp_partition_write(&f.esp_part, 0, data, 256);
    CHECK(f.emu.getTotalTime() == 417);
    f.emu.clearStats();
    esp_partition_write(&f.esp_part, 0, data, (128+256)/2);
    CHECK(f.emu.getTotalTime() == (205+417)/2);
    f.emu.clearStats();

    esp_partition_erase_range(&f.esp_part, 0, SPI_FLASH_SEC_SIZE);
    CHECK(f.emu.getEraseOps() == 1);
    CHECK(f.emu.getTotalTime() == 37142);
}

TEST_CASE("data is randomized predictably", "[spi_flash_emu]")
{
    SpiFlashEmulator emu1(3);
    emu1.randomize(0x12345678);

    SpiFlashEmulator emu2(3);
    emu2.randomize(0x12345678);

    CHECK(std::equal(emu1.bytes(), emu1.bytes() + emu1.size(), emu2.bytes()));
}
