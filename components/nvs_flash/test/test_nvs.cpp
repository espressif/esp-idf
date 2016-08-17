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
#include "nvs.hpp"
#include "nvs_flash.h"
#include "spi_flash_emulation.h"
#include <sstream>
#include <iostream>

using namespace std;
using namespace nvs;

stringstream s_perf;

void dumpBytes(const uint8_t* data, size_t count)
{
    for (uint32_t i = 0; i < count; ++i) {
        if (i % 32 == 0) {
            printf("%08x    ", i);
        }
        printf("%02x ", data[i]);
        if ((i + 1) % 32 == 0) {
            printf("\n");
        }
    }
}

TEST_CASE("crc32 behaves as expected", "[nvs]")
{
    Item item1;
    item1.datatype = ItemType::I32;
    item1.nsIndex = 1;
    item1.crc32 = 0;
    item1.reserved = 0xff;
    fill_n(item1.key, sizeof(item1.key), 0xbb);
    fill_n(item1.data, sizeof(item1.data), 0xaa);

    auto crc32_1 = item1.calculateCrc32();

    Item item2 = item1;
    item2.crc32 = crc32_1;

    CHECK(crc32_1 == item2.calculateCrc32());

    item2 = item1;
    item2.nsIndex = 2;
    CHECK(crc32_1 != item2.calculateCrc32());

    item2 = item1;
    item2.datatype = ItemType::U32;
    CHECK(crc32_1 != item2.calculateCrc32());

    item2 = item1;
    strlcpy(item2.key, "foo", Item::MAX_KEY_LENGTH);
    CHECK(crc32_1 != item2.calculateCrc32());
}

TEST_CASE("starting with empty flash, page is in uninitialized state", "[nvs]")
{
    SpiFlashEmulator emu(1);
    Page page;
    CHECK(page.state() == Page::PageState::INVALID);
    CHECK(page.load(0) == ESP_OK);
    CHECK(page.state() == Page::PageState::UNINITIALIZED);
}

TEST_CASE("can distinguish namespaces", "[nvs]")
{
    SpiFlashEmulator emu(1);
    Page page;
    CHECK(page.load(0) == ESP_OK);
    int32_t val1 = 0x12345678;
    CHECK(page.writeItem(1, ItemType::I32, "intval1", &val1, sizeof(val1)) == ESP_OK);
    int32_t val2 = 0x23456789;
    CHECK(page.writeItem(2, ItemType::I32, "intval1", &val2, sizeof(val2)) == ESP_OK);

    int32_t readVal;
    CHECK(page.readItem(2, ItemType::I32, "intval1", &readVal, sizeof(readVal)) == ESP_OK);
    CHECK(readVal == val2);

}

TEST_CASE("reading with different type causes type mismatch error", "[nvs]")
{
    SpiFlashEmulator emu(1);
    Page page;
    CHECK(page.load(0) == ESP_OK);
    int32_t val = 0x12345678;
    CHECK(page.writeItem(1, ItemType::I32, "intval1", &val, sizeof(val)) == ESP_OK);
    CHECK(page.readItem(1, ItemType::U32, "intval1", &val, sizeof(val)) == ESP_ERR_NVS_TYPE_MISMATCH);
}

TEST_CASE("when page is erased, it's state becomes UNITIALIZED", "[nvs]")
{
    SpiFlashEmulator emu(1);
    Page page;
    CHECK(page.load(0) == ESP_OK);
    int32_t val = 0x12345678;
    CHECK(page.writeItem(1, ItemType::I32, "intval1", &val, sizeof(val)) == ESP_OK);
    CHECK(page.erase() == ESP_OK);
    CHECK(page.state() == Page::PageState::UNINITIALIZED);
}

TEST_CASE("when writing and erasing, used/erased counts are updated correctly", "[nvs]")
{
    SpiFlashEmulator emu(1);
    Page page;
    CHECK(page.load(0) == ESP_OK);
    CHECK(page.getUsedEntryCount() == 0);
    CHECK(page.getErasedEntryCount() == 0);
    uint32_t foo1 = 0;
    CHECK(page.writeItem(1, "foo1", foo1) == ESP_OK);
    CHECK(page.getUsedEntryCount() == 1);
    CHECK(page.writeItem(2, "foo1", foo1) == ESP_OK);
    CHECK(page.getUsedEntryCount() == 2);
    CHECK(page.eraseItem<uint32_t>(2, "foo1") == ESP_OK);
    CHECK(page.getUsedEntryCount() == 1);
    CHECK(page.getErasedEntryCount() == 1);
    for (size_t i = 0; i < Page::ENTRY_COUNT - 2; ++i) {
        char name[16];
        snprintf(name, sizeof(name), "i%ld", i);
        CHECK(page.writeItem(1, name, i) == ESP_OK);
    }
    CHECK(page.getUsedEntryCount() == Page::ENTRY_COUNT - 1);
    CHECK(page.getErasedEntryCount() == 1);
    for (size_t i = 0; i < Page::ENTRY_COUNT - 2; ++i) {
        char name[16];
        snprintf(name, sizeof(name), "i%ld", i);
        CHECK(page.eraseItem(1, itemTypeOf<size_t>(), name) == ESP_OK);
    }
    CHECK(page.getUsedEntryCount() == 1);
    CHECK(page.getErasedEntryCount() == Page::ENTRY_COUNT - 1);
}

TEST_CASE("when page is full, adding an element fails", "[nvs]")
{
    SpiFlashEmulator emu(1);
    Page page;
    CHECK(page.load(0) == ESP_OK);
    for (size_t i = 0; i < Page::ENTRY_COUNT; ++i) {
        char name[16];
        snprintf(name, sizeof(name), "i%ld", i);
        CHECK(page.writeItem(1, name, i) == ESP_OK);
    }
    CHECK(page.state() == Page::PageState::FULL);
    CHECK(page.writeItem(1, "foo", 64UL) == ESP_ERR_NVS_PAGE_FULL);
    CHECK(page.state() == Page::PageState::FULL);
}

TEST_CASE("page maintains its seq number")
{
    SpiFlashEmulator emu(1);
    {
        Page page;
        CHECK(page.load(0) == ESP_OK);
        CHECK(page.setSeqNumber(123) == ESP_OK);
        int32_t val = 42;
        CHECK(page.writeItem(1, ItemType::I32, "dummy", &val, sizeof(val)) == ESP_OK);
    }
    {
        Page page;
        CHECK(page.load(0) == ESP_OK);
        uint32_t seqno;
        CHECK(page.getSeqNumber(seqno) == ESP_OK);
        CHECK(seqno == 123);
    }
}

TEST_CASE("can write and read variable length data", "[nvs]")
{
    SpiFlashEmulator emu(1);
    Page page;
    CHECK(page.load(0) == ESP_OK);
    const char str[] = "foobar1234foobar1234foobar1234foobar1234foobar1234foobar1234foobar1234foobar1234";
    size_t len = strlen(str);
    CHECK(page.writeItem(1, "stuff1", 42) == ESP_OK);
    CHECK(page.writeItem(1, "stuff2", 1) == ESP_OK);
    CHECK(page.writeItem(1, ItemType::SZ, "foobaar", str, len + 1) == ESP_OK);
    CHECK(page.writeItem(1, "stuff3", 2) == ESP_OK);
    CHECK(page.writeItem(1, ItemType::BLOB, "baz", str, len) == ESP_OK);
    CHECK(page.writeItem(1, "stuff4", 0x7abbccdd) == ESP_OK);

    char buf[sizeof(str) + 16];
    int32_t value;
    CHECK(page.readItem(1, "stuff1", value) == ESP_OK);
    CHECK(value == 42);
    CHECK(page.readItem(1, "stuff2", value) == ESP_OK);
    CHECK(value == 1);
    CHECK(page.readItem(1, "stuff3", value) == ESP_OK);
    CHECK(value == 2);
    CHECK(page.readItem(1, "stuff4", value) == ESP_OK);
    CHECK(value == 0x7abbccdd);

    fill_n(buf, sizeof(buf), 0xff);
    CHECK(page.readItem(1, ItemType::SZ, "foobaar", buf, sizeof(buf)) == ESP_OK);
    CHECK(memcmp(buf, str, strlen(str) + 1) == 0);

    fill_n(buf, sizeof(buf), 0xff);
    CHECK(page.readItem(1, ItemType::BLOB, "baz", buf, sizeof(buf)) == ESP_OK);
    CHECK(memcmp(buf, str, strlen(str)) == 0);
}


TEST_CASE("can init PageManager in empty flash", "[nvs]")
{
    SpiFlashEmulator emu(4);
    PageManager pm;
    CHECK(pm.load(0, 4) == ESP_OK);
}

TEST_CASE("PageManager adds page in the correct order", "[nvs]")
{
    const size_t pageCount = 8;
    SpiFlashEmulator emu(pageCount);
    uint32_t pageNo[pageCount] = { -1U, 50, 11, -1U, 23, 22, 24, 49};

    for (uint32_t i = 0; i < pageCount; ++i) {
        Page p;
        p.load(i);
        if (pageNo[i] != -1U) {
            p.setSeqNumber(pageNo[i]);
            p.writeItem(1, "foo", 10U);
        }
    }

    PageManager pageManager;
    CHECK(pageManager.load(0, pageCount) == ESP_OK);

    uint32_t lastSeqNo = 0;
    for (auto it = std::begin(pageManager); it != std::end(pageManager); ++it) {
        uint32_t seqNo;
        CHECK(it->getSeqNumber(seqNo) == ESP_OK);
        CHECK(seqNo > lastSeqNo);
    }
}

TEST_CASE("can init storage in empty flash", "[nvs]")
{
    SpiFlashEmulator emu(8);
    Storage storage;
    emu.setBounds(4, 8);
    CHECK(storage.init(4, 4) == ESP_OK);
    s_perf << "Time to init empty storage (4 sectors): " << emu.getTotalTime() << " us" << std::endl;
}

TEST_CASE("storage doesn't add duplicates within one page", "[nvs]")
{
    SpiFlashEmulator emu(8);
    Storage storage;
    emu.setBounds(4, 8);
    CHECK(storage.init(4, 4) == ESP_OK);
    int bar = 0;
    CHECK(storage.writeItem(1, "bar", bar) == ESP_OK);
    CHECK(storage.writeItem(1, "bar", bar) == ESP_OK);

    Page page;
    page.load(4);
    CHECK(page.getUsedEntryCount() == 1);
    CHECK(page.getErasedEntryCount() == 1);
}

TEST_CASE("can write one item a thousand times", "[nvs]")
{
    SpiFlashEmulator emu(8);
    Storage storage;
    emu.setBounds(4, 8);
    CHECK(storage.init(4, 4) == ESP_OK);
    for (size_t i = 0; i < Page::ENTRY_COUNT * 4 * 2; ++i) {
        REQUIRE(storage.writeItem(1, "i", static_cast<int>(i)) == ESP_OK);
    }
    s_perf << "Time to write one item a thousand times: " << emu.getTotalTime() << " us (" << emu.getEraseOps() << " " << emu.getWriteOps() << " " << emu.getReadOps() << " " << emu.getWriteBytes() << " " << emu.getReadBytes() << ")" << std::endl;
}

TEST_CASE("storage doesn't add duplicates within multiple pages", "[nvs]")
{
    SpiFlashEmulator emu(8);
    Storage storage;
    emu.setBounds(4, 8);
    CHECK(storage.init(4, 4) == ESP_OK);
    int bar = 0;
    CHECK(storage.writeItem(1, "bar", bar) == ESP_OK);
    for (size_t i = 0; i < Page::ENTRY_COUNT; ++i) {
        CHECK(storage.writeItem(1, "foo", static_cast<int>(bar)) == ESP_OK);
    }
    CHECK(storage.writeItem(1, "bar", bar) == ESP_OK);

    Page page;
    page.load(4);
    CHECK(page.findItem(1, itemTypeOf<int>(), "bar") == ESP_ERR_NVS_NOT_FOUND);
    page.load(5);
    CHECK(page.findItem(1, itemTypeOf<int>(), "bar") == ESP_OK);
}

TEST_CASE("can write and read variable length data lots of times", "[nvs]")
{
    SpiFlashEmulator emu(8);
    Storage storage;
    emu.setBounds(4, 8);
    CHECK(storage.init(4, 4) == ESP_OK);
    const char str[] = "foobar1234foobar1234foobar1234foobar1234foobar1234foobar1234foobar1234foobar1234";
    char buf[sizeof(str) + 16];
    size_t len = strlen(str);
    for (size_t i = 0; i < Page::ENTRY_COUNT * 4 * 2; ++i) {
        CAPTURE(i);
        CHECK(storage.writeItem(1, ItemType::SZ, "foobaar", str, len + 1) == ESP_OK);
        CHECK(storage.writeItem(1, "foo", static_cast<uint32_t>(i)) == ESP_OK);

        uint32_t value;
        CHECK(storage.readItem(1, "foo", value) == ESP_OK);
        CHECK(value == i);

        fill_n(buf, sizeof(buf), 0xff);
        CHECK(storage.readItem(1, ItemType::SZ, "foobaar", buf, sizeof(buf)) == ESP_OK);
        CHECK(memcmp(buf, str, strlen(str) + 1) == 0);
    }
    s_perf << "Time to write one string and one integer a thousand times: " << emu.getTotalTime() << " us (" << emu.getEraseOps() << " " << emu.getWriteOps() << " " << emu.getReadOps() << " " << emu.getWriteBytes() << " " << emu.getReadBytes() << ")" << std::endl;
}


TEST_CASE("can get length of variable length data", "[nvs]")
{
    SpiFlashEmulator emu(8);
    emu.randomize(200);
    Storage storage;
    emu.setBounds(4, 8);
    CHECK(storage.init(4, 4) == ESP_OK);
    const char str[] = "foobar1234foobar1234foobar1234foobar1234foobar1234foobar1234foobar1234foobar1234";
    size_t len = strlen(str);
    CHECK(storage.writeItem(1, ItemType::SZ, "foobaar", str, len + 1) == ESP_OK);
    size_t dataSize;
    CHECK(storage.getItemDataSize(1, ItemType::SZ, "foobaar", dataSize) == ESP_OK);
    CHECK(dataSize == len + 1);

    CHECK(storage.writeItem(2, ItemType::BLOB, "foobaar", str, len) == ESP_OK);
    CHECK(storage.getItemDataSize(2, ItemType::BLOB, "foobaar", dataSize) == ESP_OK);
    CHECK(dataSize == len);
}


TEST_CASE("can create namespaces", "[nvs]")
{
    SpiFlashEmulator emu(8);
    Storage storage;
    emu.setBounds(4, 8);
    CHECK(storage.init(4, 4) == ESP_OK);
    uint8_t nsi;
    CHECK(storage.createOrOpenNamespace("wifi", false, nsi) == ESP_ERR_NVS_NOT_FOUND);

    CHECK(storage.createOrOpenNamespace("wifi", true, nsi) == ESP_OK);
    Page page;
    page.load(4);
    CHECK(page.findItem(Page::NS_INDEX, ItemType::U8, "wifi") == ESP_OK);
}

TEST_CASE("storage may become full", "[nvs]")
{
    SpiFlashEmulator emu(8);
    Storage storage;
    emu.setBounds(4, 8);
    CHECK(storage.init(4, 4) == ESP_OK);
    for (size_t i = 0; i < Page::ENTRY_COUNT * 3; ++i) {
        char name[Item::MAX_KEY_LENGTH + 1];
        snprintf(name, sizeof(name), "key%05d", static_cast<int>(i));
        REQUIRE(storage.writeItem(1, name, static_cast<int>(i)) == ESP_OK);
    }
    REQUIRE(storage.writeItem(1, "foo", 10) == ESP_ERR_NVS_NOT_ENOUGH_SPACE);
}

TEST_CASE("can modify an item on a page which will be erased", "[nvs]")
{
    SpiFlashEmulator emu(2);
    Storage storage;
    CHECK(storage.init(0, 2) == ESP_OK);
    for (size_t i = 0; i < Page::ENTRY_COUNT * 3 + 1; ++i) {
        REQUIRE(storage.writeItem(1, "foo", 42U) == ESP_OK);
    }
}


#define TEST_ESP_ERR(rc, res) CHECK((rc) == (res))
#define TEST_ESP_OK(rc) CHECK((rc) == ESP_OK)

TEST_CASE("nvs api tests", "[nvs]")
{
    SpiFlashEmulator emu(10);
    emu.randomize(100);
    
    nvs_handle handle_1;
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);


    TEST_ESP_ERR(nvs_open("namespace1", NVS_READWRITE, &handle_1), ESP_ERR_NVS_NOT_INITIALIZED);
    for (uint16_t i = NVS_FLASH_SECTOR; i <NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN; ++i) {
        spi_flash_erase_sector(i);
    }
    TEST_ESP_OK(nvs_flash_init(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN));

    TEST_ESP_ERR(nvs_open("namespace1", NVS_READONLY, &handle_1), ESP_ERR_NVS_NOT_FOUND);

    // TEST_ESP_ERR(nvs_set_i32(handle_1, "foo", 0x12345678), ESP_ERR_NVS_READ_ONLY);
    // nvs_close(handle_1);

    TEST_ESP_OK(nvs_open("namespace1", NVS_READWRITE, &handle_1));
    TEST_ESP_OK(nvs_set_i32(handle_1, "foo", 0x12345678));
    TEST_ESP_OK(nvs_set_i32(handle_1, "foo", 0x23456789));

    nvs_handle handle_2;
    TEST_ESP_OK(nvs_open("namespace2", NVS_READWRITE, &handle_2));
    TEST_ESP_OK(nvs_set_i32(handle_2, "foo", 0x3456789a));
    const char* str = "value 0123456789abcdef0123456789abcdef";
    TEST_ESP_OK(nvs_set_str(handle_2, "key", str));

    int32_t v1;
    TEST_ESP_OK(nvs_get_i32(handle_1, "foo", &v1));
    CHECK(0x23456789 == v1);

    int32_t v2;
    TEST_ESP_OK(nvs_get_i32(handle_2, "foo", &v2));
    CHECK(0x3456789a == v2);

    char buf[strlen(str) + 1];
    size_t buf_len = sizeof(buf);

    TEST_ESP_OK(nvs_get_str(handle_2, "key", buf, &buf_len));

    CHECK(0 == strcmp(buf, str));
}


TEST_CASE("wifi test", "[nvs]")
{
    SpiFlashEmulator emu(10);
    emu.randomize(10);
    
    nvs_handle handle;
    const uint32_t NVS_FLASH_SECTOR = 5;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);
    TEST_ESP_OK(nvs_flash_init(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN));
    
    TEST_ESP_OK(nvs_open("nvs.net80211", NVS_READWRITE, &handle));
    
    uint8_t opmode = 2;
    if (nvs_get_u8(handle, "wifi.opmode", &opmode) != ESP_OK) {
        TEST_ESP_OK(nvs_set_u8(handle, "wifi.opmode", opmode));
    }
}


TEST_CASE("can init storage from flash with random contents", "[nvs]")
{
    SpiFlashEmulator emu(10);
    emu.randomize(42);
    
    nvs_handle handle;
    const uint32_t NVS_FLASH_SECTOR = 5;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);
    TEST_ESP_OK(nvs_flash_init(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN));
    
    TEST_ESP_OK(nvs_open("nvs.net80211", NVS_READWRITE, &handle));
    
    uint8_t opmode = 2;
    if (nvs_get_u8(handle, "wifi.opmode", &opmode) != ESP_OK) {
        TEST_ESP_OK(nvs_set_u8(handle, "wifi.opmode", opmode));
    }
}


TEST_CASE("nvs api tests, starting with random data in flash", "[nvs][.][long]")
{
    for (size_t count = 0; count < 10000; ++count) {
        SpiFlashEmulator emu(10);
        emu.randomize(static_cast<uint32_t>(count));
        
        const uint32_t NVS_FLASH_SECTOR = 6;
        const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
        emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);
        
        TEST_ESP_OK(nvs_flash_init(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN));
        
        nvs_handle handle_1;
        TEST_ESP_ERR(nvs_open("namespace1", NVS_READONLY, &handle_1), ESP_ERR_NVS_NOT_FOUND);
        
        TEST_ESP_OK(nvs_open("namespace1", NVS_READWRITE, &handle_1));
        TEST_ESP_OK(nvs_set_i32(handle_1, "foo", 0x12345678));
        for (size_t i = 0; i < 500; ++i) {
            nvs_handle handle_2;
            TEST_ESP_OK(nvs_open("namespace2", NVS_READWRITE, &handle_2));
            TEST_ESP_OK(nvs_set_i32(handle_1, "foo", 0x23456789 % (i + 1)));
            TEST_ESP_OK(nvs_set_i32(handle_2, "foo", static_cast<int32_t>(i)));
            const char* str = "value 0123456789abcdef0123456789abcdef %09d";
            char str_buf[128];
            snprintf(str_buf, sizeof(str_buf), str, i + count * 1024);
            TEST_ESP_OK(nvs_set_str(handle_2, "key", str_buf));
            
            int32_t v1;
            TEST_ESP_OK(nvs_get_i32(handle_1, "foo", &v1));
            CHECK(0x23456789 % (i + 1) == v1);
            
            int32_t v2;
            TEST_ESP_OK(nvs_get_i32(handle_2, "foo", &v2));
            CHECK(static_cast<int32_t>(i) == v2);
            
            char buf[128];
            size_t buf_len = sizeof(buf);
            
            TEST_ESP_OK(nvs_get_str(handle_2, "key", buf, &buf_len));
            
            CHECK(0 == strcmp(buf, str_buf));
            nvs_close(handle_2);
        }
        nvs_close(handle_1);
    }
}

template<typename TGen>
esp_err_t doRandomThings(nvs_handle handle, TGen gen, size_t count) {
    
    const char* keys[] = {"foo", "bar", "longkey_0123456", "another key", "param1", "param2", "param3", "param4", "param5"};
    const ItemType types[] = {ItemType::I32, ItemType::I32, ItemType::U64, ItemType::U64, ItemType::SZ, ItemType::SZ, ItemType::SZ, ItemType::SZ, ItemType::SZ};
    
    int32_t v1 = 0, v2 = 0;
    uint64_t v3 = 0, v4 = 0;
    const size_t strBufLen = 1024;
    char v5[strBufLen], v6[strBufLen], v7[strBufLen], v8[strBufLen], v9[strBufLen];
    
    void* values[] = {&v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8, &v9};
    
    const size_t nKeys = sizeof(keys) / sizeof(keys[0]);
    static_assert(nKeys == sizeof(types) / sizeof(types[0]), "");
    static_assert(nKeys == sizeof(values) / sizeof(values[0]), "");
    
    bool written[nKeys];
    std::fill_n(written, nKeys, false);
    
    auto generateRandomString = [](char* dst, size_t size) {
        size_t len = 0;
    };
    
    auto randomRead = [&](size_t index) -> esp_err_t {
        switch (types[index]) {
            case ItemType::I32:
            {
                int32_t val;
                auto err = nvs_get_i32(handle, keys[index], &val);
                if (err == ESP_ERR_FLASH_OP_FAIL) {
                    return err;
                }
                if (!written[index]) {
                    REQUIRE(err == ESP_ERR_NVS_NOT_FOUND);
                }
                else {
                    REQUIRE(val == *reinterpret_cast<int32_t*>(values[index]));
                }
                break;
            }
                
            case ItemType::U64:
            {
                uint64_t val;
                auto err = nvs_get_u64(handle, keys[index], &val);
                if (err == ESP_ERR_FLASH_OP_FAIL) {
                    return err;
                }
                if (!written[index]) {
                    REQUIRE(err == ESP_ERR_NVS_NOT_FOUND);
                }
                else {
                    REQUIRE(val == *reinterpret_cast<uint64_t*>(values[index]));
                }
                break;
            }
                
            case ItemType::SZ:
            {
                char buf[strBufLen];
                size_t len = strBufLen;
                auto err = nvs_get_str(handle, keys[index], buf, &len);
                if (err == ESP_ERR_FLASH_OP_FAIL) {
                    return err;
                }
                if (!written[index]) {
                    REQUIRE(err == ESP_ERR_NVS_NOT_FOUND);
                }
                else {
                    REQUIRE(strncmp(buf, reinterpret_cast<const char*>(values[index]), strBufLen));
                }
                break;
            }
                
            default:
                assert(0);
        }
        return ESP_OK;
    };
    
    auto randomWrite = [&](size_t index) -> esp_err_t {
        switch (types[index]) {
            case ItemType::I32:
            {
                int32_t val = static_cast<int32_t>(gen());
                *reinterpret_cast<int32_t*>(values[index]) = val;
                
                auto err = nvs_set_i32(handle, keys[index], val);
                if (err == ESP_ERR_FLASH_OP_FAIL) {
                    return err;
                }
                REQUIRE(err == ESP_OK);
                written[index] = true;
                break;
            }
                
            case ItemType::U64:
            {
                uint64_t val = static_cast<uint64_t>(gen());
                *reinterpret_cast<uint64_t*>(values[index]) = val;
                
                auto err = nvs_set_u64(handle, keys[index], val);
                if (err == ESP_ERR_FLASH_OP_FAIL) {
                    return err;
                }
                REQUIRE(err == ESP_OK);
                written[index] = true;
                break;
            }
                
            case ItemType::SZ:
            {
                char buf[strBufLen];
                size_t len = strBufLen;
                
                size_t strLen = gen() % (strBufLen - 1);
                std::generate_n(buf, strLen, [&]() -> char {
                    const char c = static_cast<char>(gen() % 127);
                    return (c < 32) ? 32 : c;
                });
                
                auto err = nvs_set_str(handle, keys[index], buf);
                if (err == ESP_ERR_FLASH_OP_FAIL) {
                    return err;
                }
                REQUIRE(err == ESP_OK);
                written[index] = true;
                break;
            }
                
            default:
                assert(0);
        }
        return ESP_OK;
    };
    
    
    for (size_t i = 0; i < count; ++i) {
        size_t index = gen() % nKeys;
        switch (gen() % 3) {
            case 0:  // read, 1/3
                if (randomRead(index) == ESP_ERR_FLASH_OP_FAIL) {
                    return ESP_ERR_FLASH_OP_FAIL;
                }
                break;
                
            default: // write, 2/3
                if (randomWrite(index) == ESP_ERR_FLASH_OP_FAIL) {
                    return ESP_ERR_FLASH_OP_FAIL;
                }
                break;
        }
    }
    return ESP_OK;
}

TEST_CASE("monkey test", "[nvs][monkey]")
{
    std::random_device rd;
    std::mt19937 gen(rd());
    uint32_t seed = 3;
    gen.seed(seed);
    
    SpiFlashEmulator emu(10);
    emu.randomize(seed);
    
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);
    
    TEST_ESP_OK(nvs_flash_init(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN));
    
    nvs_handle handle;
    TEST_ESP_OK(nvs_open("namespace1", NVS_READWRITE, &handle));

    CHECK(doRandomThings(handle, gen, 10000) == ESP_OK);

}

TEST_CASE("dump all performance data", "[nvs]")
{
    std::cout << "====================" << std::endl << "Dumping benchmarks" << std::endl;
    std::cout << s_perf.str() << std::endl;
    std::cout << "====================" << std::endl;
}
