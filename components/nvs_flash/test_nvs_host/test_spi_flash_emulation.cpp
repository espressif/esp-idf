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
#include "esp_spi_flash.h"
#include "spi_flash_emulation.h"

using namespace std;

template <typename Tit>
bool range_empty_n(Tit it_begin, size_t n)
{
    return all_of(it_begin, it_begin + n, bind(equal_to<uint32_t>(), placeholders::_1, 0xffffffff));
}

TEST_CASE("flash starts with all bytes == 0xff", "[spi_flash_emu]")
{
    SpiFlashEmulator emu(4);

    uint8_t sector[SPI_FLASH_SEC_SIZE];

    for (int i = 0; i < 4; ++i) {
        CHECK(spi_flash_read(0, sector, sizeof(sector)) == ESP_OK);
        for (auto v: sector) {
            CHECK(v == 0xff);
        }
    }
}

TEST_CASE("invalid writes are checked", "[spi_flash_emu]")
{
    SpiFlashEmulator emu(1);

    uint32_t val = 0;
    CHECK(spi_flash_write(0, &val, 4) == ESP_OK);
    val = 1;
    CHECK(spi_flash_write(0, &val, 4) == ESP_ERR_FLASH_OP_FAIL);
}


TEST_CASE("out of bounds writes fail", "[spi_flash_emu]")
{
    SpiFlashEmulator emu(4);
    uint32_t vals[8];
    std::fill_n(vals, 8, 0);
    CHECK(spi_flash_write(0, vals, sizeof(vals)) == ESP_OK);

    CHECK(spi_flash_write(4*4096 - sizeof(vals), vals, sizeof(vals)) == ESP_OK);

    CHECK(spi_flash_write(4*4096 - sizeof(vals) + 4, vals, sizeof(vals)) == ESP_ERR_FLASH_OP_FAIL);
}


TEST_CASE("after erase the sector is set to 0xff", "[spi_flash_emu]")
{
    SpiFlashEmulator emu(4);
    uint32_t val1 = 0xab00cd12;
    CHECK(spi_flash_write(0, &val1, sizeof(val1)) == ESP_OK);
    uint32_t val2 = 0x5678efab;
    CHECK(spi_flash_write(4096 - 4, &val2, sizeof(val2)) == ESP_OK);

    CHECK(emu.words()[0] == val1);
    CHECK(range_empty_n(emu.words() + 1, 4096 / 4 - 2));
    CHECK(emu.words()[4096 / 4 - 1] == val2);

    CHECK(spi_flash_erase_sector(0) == ESP_OK);

    CHECK(emu.words()[0] == 0xffffffff);
    CHECK(range_empty_n(emu.words() + 1, 4096 / 4 - 2));
    CHECK(emu.words()[4096 / 4 - 1] == 0xffffffff);
}

TEST_CASE("read/write/erase operation times are calculated correctly", "[spi_flash_emu]")
{
    SpiFlashEmulator emu(1);
    uint8_t data[512];
    spi_flash_read(0, data, 4);
    CHECK(emu.getTotalTime() == 7);
    CHECK(emu.getReadOps() == 1);
    CHECK(emu.getReadBytes() == 4);
    emu.clearStats();
    spi_flash_read(0, data, 8);
    CHECK(emu.getTotalTime() == 5);
    CHECK(emu.getReadOps() == 1);
    CHECK(emu.getReadBytes() == 8);
    emu.clearStats();
    spi_flash_read(0, data, 16);
    CHECK(emu.getTotalTime() == 6);
    CHECK(emu.getReadOps() == 1);
    CHECK(emu.getReadBytes() == 16);
    emu.clearStats();
    spi_flash_read(0, data, 128);
    CHECK(emu.getTotalTime() == 18);
    CHECK(emu.getReadOps() == 1);
    CHECK(emu.getReadBytes() == 128);
    emu.clearStats();
    spi_flash_read(0, data, 256);
    CHECK(emu.getTotalTime() == 32);
    emu.clearStats();
    spi_flash_read(0, data, (128+256)/2);
    CHECK(emu.getTotalTime() == (18+32)/2);
    emu.clearStats();

    spi_flash_write(0, data, 4);
    CHECK(emu.getTotalTime() == 19);
    CHECK(emu.getWriteOps() == 1);
    CHECK(emu.getWriteBytes() == 4);
    emu.clearStats();
    CHECK(emu.getWriteOps() == 0);
    CHECK(emu.getWriteBytes() == 0);
    spi_flash_write(0, data, 8);
    CHECK(emu.getTotalTime() == 23);
    emu.clearStats();
    spi_flash_write(0, data, 16);
    CHECK(emu.getTotalTime() == 35);
    CHECK(emu.getWriteOps() == 1);
    CHECK(emu.getWriteBytes() == 16);
    emu.clearStats();
    spi_flash_write(0, data, 128);
    CHECK(emu.getTotalTime() == 205);
    emu.clearStats();
    spi_flash_write(0, data, 256);
    CHECK(emu.getTotalTime() == 417);
    emu.clearStats();
    spi_flash_write(0, data, (128+256)/2);
    CHECK(emu.getTotalTime() == (205+417)/2);
    emu.clearStats();

    spi_flash_erase_sector(0);
    CHECK(emu.getEraseOps() == 1);
    CHECK(emu.getTotalTime() == 37142);
}

TEST_CASE("data is randomized predictably", "[spi_flash_emu]")
{
    SpiFlashEmulator emu1(3);
    emu1.randomize(0x12345678);
    
    SpiFlashEmulator emu2(3);
    emu2.randomize(0x12345678);
    
    CHECK(std::equal(emu1.bytes(), emu1.bytes() + emu1.size(), emu2.bytes()));
}

