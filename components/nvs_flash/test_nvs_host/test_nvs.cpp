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
#include "nvs_test_api.h"
#include "spi_flash_emulation.h"
#include <sstream>
#include <iostream>

#define TEST_ESP_ERR(rc, res) CHECK((rc) == (res))
#define TEST_ESP_OK(rc) CHECK((rc) == ESP_OK)

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
    strncpy(item2.key, "foo", Item::MAX_KEY_LENGTH);
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
    CHECK(page.writeItem(1, "foo", 64UL) == ESP_ERR_NVS_PAGE_FULL);
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

TEST_CASE("different key names are distinguished even if the pointer is the same", "[nvs]")
{
    SpiFlashEmulator emu(1);
    Page page;
    TEST_ESP_OK(page.load(0));
    TEST_ESP_OK(page.writeItem(1, "i1", 1));
    TEST_ESP_OK(page.writeItem(1, "i2", 2));
    int32_t value;
    char keyname[10] = {0};
    for (int i = 0; i < 2; ++i) {
        strncpy(keyname, "i1", sizeof(keyname) - 1);
        TEST_ESP_OK(page.readItem(1, keyname, value));
        CHECK(value == 1);
        strncpy(keyname, "i2", sizeof(keyname) - 1);
        TEST_ESP_OK(page.readItem(1, keyname, value));
        CHECK(value == 2);
    }
}

TEST_CASE("Page validates key size", "[nvs]")
{
    SpiFlashEmulator emu(4);
    Page page;
    TEST_ESP_OK(page.load(0));
    // 16-character key fails
    TEST_ESP_ERR(page.writeItem(1, "0123456789123456", 1), ESP_ERR_NVS_KEY_TOO_LONG);
    // 15-character key is okay
    TEST_ESP_OK(page.writeItem(1, "012345678912345", 1));
}

TEST_CASE("Page validates blob size", "[nvs]")
{
    SpiFlashEmulator emu(4);
    Page page;
    TEST_ESP_OK(page.load(0));

    char buf[2048] = { 0 };
    // There are two potential errors here:
    // - not enough space in the page (because one value has been written already)
    // - value is too long
    // Check that the second one is actually returned.
    TEST_ESP_ERR(page.writeItem(1, ItemType::BLOB, "2", buf, Page::ENTRY_COUNT * Page::ENTRY_SIZE), ESP_ERR_NVS_VALUE_TOO_LONG);
    // Should fail as well
    TEST_ESP_ERR(page.writeItem(1, ItemType::BLOB, "2", buf, Page::BLOB_MAX_SIZE + 1), ESP_ERR_NVS_VALUE_TOO_LONG);
    TEST_ESP_OK(page.writeItem(1, ItemType::BLOB, "2", buf, Page::BLOB_MAX_SIZE));
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

TEST_CASE("storage can find items on second page if first is not fully written and has cached search data", "[nvs]")
{
    SpiFlashEmulator emu(3);
    Storage storage;
    CHECK(storage.init(0, 3) == ESP_OK);
    int bar = 0;
    uint8_t bigdata[Page::BLOB_MAX_SIZE] = {0};
    // write one big chunk of data
    ESP_ERROR_CHECK(storage.writeItem(0, ItemType::BLOB, "1", bigdata, sizeof(bigdata)));
    // write another big chunk of data
    ESP_ERROR_CHECK(storage.writeItem(0, ItemType::BLOB, "2", bigdata, sizeof(bigdata)));
    
    // write third one; it will not fit into the first page
    ESP_ERROR_CHECK(storage.writeItem(0, ItemType::BLOB, "3", bigdata, sizeof(bigdata)));
    
    size_t size;
    ESP_ERROR_CHECK(storage.getItemDataSize(0, ItemType::BLOB, "1", size));
    CHECK(size == sizeof(bigdata));
    ESP_ERROR_CHECK(storage.getItemDataSize(0, ItemType::BLOB, "3", size));
    CHECK(size == sizeof(bigdata));
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


TEST_CASE("can erase items", "[nvs]")
{
    SpiFlashEmulator emu(3);
    Storage storage;
    CHECK(storage.init(0, 3) == ESP_OK);
    for (size_t i = 0; i < Page::ENTRY_COUNT * 2 - 3; ++i) {
        char name[Item::MAX_KEY_LENGTH + 1];
        snprintf(name, sizeof(name), "key%05d", static_cast<int>(i));
        REQUIRE(storage.writeItem(3, name, static_cast<int>(i)) == ESP_OK);
    }
    CHECK(storage.writeItem(1, "foo", 32) == ESP_OK);
    CHECK(storage.writeItem(2, "foo", 64) == ESP_OK);
    CHECK(storage.eraseItem(2, "foo") == ESP_OK);
    int val;
    CHECK(storage.readItem(1, "foo", val) == ESP_OK);
    CHECK(val == 32);
    CHECK(storage.eraseNamespace(3) == ESP_OK);
    CHECK(storage.readItem(2, "foo", val) == ESP_ERR_NVS_NOT_FOUND);
    CHECK(storage.readItem(3, "key00222", val) == ESP_ERR_NVS_NOT_FOUND);
}

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
    TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN));

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

    size_t buf_len_needed;
    TEST_ESP_OK(nvs_get_str(handle_2, "key", NULL, &buf_len_needed));
    CHECK(buf_len_needed == buf_len);
    
    size_t buf_len_short = buf_len - 1;
    TEST_ESP_ERR(ESP_ERR_NVS_INVALID_LENGTH, nvs_get_str(handle_2, "key", buf, &buf_len_short));
    CHECK(buf_len_short == buf_len);
    
    TEST_ESP_OK(nvs_get_str(handle_2, "key", buf, &buf_len));

    CHECK(0 == strcmp(buf, str));
}

TEST_CASE("wifi test", "[nvs]")
{
    SpiFlashEmulator emu(10);
    emu.randomize(10);
    
    
    const uint32_t NVS_FLASH_SECTOR = 5;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);
    TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN));
    
    nvs_handle misc_handle;
    TEST_ESP_OK(nvs_open("nvs.net80211", NVS_READWRITE, &misc_handle));
    char log[33];
    size_t log_size = sizeof(log);
    TEST_ESP_ERR(nvs_get_str(misc_handle, "log", log, &log_size), ESP_ERR_NVS_NOT_FOUND);
    strcpy(log, "foobarbazfizzz");
    TEST_ESP_OK(nvs_set_str(misc_handle, "log", log));
    
    nvs_handle net80211_handle;
    TEST_ESP_OK(nvs_open("nvs.net80211", NVS_READWRITE, &net80211_handle));
    
    uint8_t opmode = 2;
    TEST_ESP_ERR(nvs_get_u8(net80211_handle, "wifi.opmode", &opmode), ESP_ERR_NVS_NOT_FOUND);
    
    TEST_ESP_OK(nvs_set_u8(net80211_handle, "wifi.opmode", opmode));
    
    uint8_t country = 0;
    TEST_ESP_ERR(nvs_get_u8(net80211_handle, "wifi.country", &opmode), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_OK(nvs_set_u8(net80211_handle, "wifi.country", opmode));
    
    char ssid[36];
    size_t size = sizeof(ssid);
    TEST_ESP_ERR(nvs_get_blob(net80211_handle, "sta.ssid", ssid, &size), ESP_ERR_NVS_NOT_FOUND);
    strcpy(ssid, "my android AP");
    TEST_ESP_OK(nvs_set_blob(net80211_handle, "sta.ssid", ssid, size));
    
    char mac[6];
    size = sizeof(mac);
    TEST_ESP_ERR(nvs_get_blob(net80211_handle, "sta.mac", mac, &size), ESP_ERR_NVS_NOT_FOUND);
    memset(mac, 0xab, 6);
    TEST_ESP_OK(nvs_set_blob(net80211_handle, "sta.mac", mac, size));
    
    uint8_t authmode = 1;
    TEST_ESP_ERR(nvs_get_u8(net80211_handle, "sta.authmode", &authmode), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_OK(nvs_set_u8(net80211_handle, "sta.authmode", authmode));

    char pswd[65];
    size = sizeof(pswd);
    TEST_ESP_ERR(nvs_get_blob(net80211_handle, "sta.pswd", pswd, &size), ESP_ERR_NVS_NOT_FOUND);
    strcpy(pswd, "`123456788990-=");
    TEST_ESP_OK(nvs_set_blob(net80211_handle, "sta.pswd", pswd, size));

    char pmk[32];
    size = sizeof(pmk);
    TEST_ESP_ERR(nvs_get_blob(net80211_handle, "sta.pmk", pmk, &size), ESP_ERR_NVS_NOT_FOUND);
    memset(pmk, 1, size);
    TEST_ESP_OK(nvs_set_blob(net80211_handle, "sta.pmk", pmk, size));
    
    uint8_t chan = 1;
    TEST_ESP_ERR(nvs_get_u8(net80211_handle, "sta.chan", &chan), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_OK(nvs_set_u8(net80211_handle, "sta.chan", chan));
    
    uint8_t autoconn = 1;
    TEST_ESP_ERR(nvs_get_u8(net80211_handle, "auto.conn", &autoconn), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_OK(nvs_set_u8(net80211_handle, "auto.conn", autoconn));

    uint8_t bssid_set = 1;
    TEST_ESP_ERR(nvs_get_u8(net80211_handle, "bssid.set", &bssid_set), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_OK(nvs_set_u8(net80211_handle, "bssid.set", bssid_set));

    char bssid[6];
    size = sizeof(bssid);
    TEST_ESP_ERR(nvs_get_blob(net80211_handle, "sta.bssid", bssid, &size), ESP_ERR_NVS_NOT_FOUND);
    memset(mac, 0xcd, 6);
    TEST_ESP_OK(nvs_set_blob(net80211_handle, "sta.bssid", bssid, size));

    uint8_t phym = 3;
    TEST_ESP_ERR(nvs_get_u8(net80211_handle, "sta.phym", &phym), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_OK(nvs_set_u8(net80211_handle, "sta.phym", phym));
    
    uint8_t phybw = 2;
    TEST_ESP_ERR(nvs_get_u8(net80211_handle, "sta.phybw", &phybw), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_OK(nvs_set_u8(net80211_handle, "sta.phybw", phybw));
    
    char apsw[2];
    size = sizeof(apsw);
    TEST_ESP_ERR(nvs_get_blob(net80211_handle, "sta.apsw", apsw, &size), ESP_ERR_NVS_NOT_FOUND);
    memset(apsw, 0x2, size);
    TEST_ESP_OK(nvs_set_blob(net80211_handle, "sta.apsw", apsw, size));
    
    char apinfo[700];
    size = sizeof(apinfo);
    TEST_ESP_ERR(nvs_get_blob(net80211_handle, "sta.apinfo", apinfo, &size), ESP_ERR_NVS_NOT_FOUND);
    memset(apinfo, 0, size);
    TEST_ESP_OK(nvs_set_blob(net80211_handle, "sta.apinfo", apinfo, size));
    
    size = sizeof(ssid);
    TEST_ESP_ERR(nvs_get_blob(net80211_handle, "ap.ssid", ssid, &size), ESP_ERR_NVS_NOT_FOUND);
    strcpy(ssid, "ESP_A2F340");
    TEST_ESP_OK(nvs_set_blob(net80211_handle, "ap.ssid", ssid, size));
    
    size = sizeof(mac);
    TEST_ESP_ERR(nvs_get_blob(net80211_handle, "ap.mac", mac, &size), ESP_ERR_NVS_NOT_FOUND);
    memset(mac, 0xac, 6);
    TEST_ESP_OK(nvs_set_blob(net80211_handle, "ap.mac", mac, size));
    
    size = sizeof(pswd);
    TEST_ESP_ERR(nvs_get_blob(net80211_handle, "ap.passwd", pswd, &size), ESP_ERR_NVS_NOT_FOUND);
    strcpy(pswd, "");
    TEST_ESP_OK(nvs_set_blob(net80211_handle, "ap.passwd", pswd, size));
    
    size = sizeof(pmk);
    TEST_ESP_ERR(nvs_get_blob(net80211_handle, "ap.pmk", pmk, &size), ESP_ERR_NVS_NOT_FOUND);
    memset(pmk, 1, size);
    TEST_ESP_OK(nvs_set_blob(net80211_handle, "ap.pmk", pmk, size));
    
    chan = 6;
    TEST_ESP_ERR(nvs_get_u8(net80211_handle, "ap.chan", &chan), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_OK(nvs_set_u8(net80211_handle, "ap.chan", chan));

    authmode = 0;
    TEST_ESP_ERR(nvs_get_u8(net80211_handle, "ap.authmode", &authmode), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_OK(nvs_set_u8(net80211_handle, "ap.authmode", authmode));
    
    uint8_t hidden = 0;
    TEST_ESP_ERR(nvs_get_u8(net80211_handle, "ap.hidden", &hidden), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_OK(nvs_set_u8(net80211_handle, "ap.hidden", hidden));
    
    uint8_t max_conn = 4;
    TEST_ESP_ERR(nvs_get_u8(net80211_handle, "ap.max.conn", &max_conn), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_OK(nvs_set_u8(net80211_handle, "ap.max.conn", max_conn));
    
    uint8_t bcn_interval = 2;
    TEST_ESP_ERR(nvs_get_u8(net80211_handle, "bcn_interval", &bcn_interval), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_OK(nvs_set_u8(net80211_handle, "bcn_interval", bcn_interval));
    
    s_perf << "Time to simulate nvs init with wifi libs: " << emu.getTotalTime() << " us (" << emu.getEraseOps() << "E " << emu.getWriteOps() << "W " << emu.getReadOps() << "R " << emu.getWriteBytes() << "Wb " << emu.getReadBytes() << "Rb)" << std::endl;

}


TEST_CASE("can init storage from flash with random contents", "[nvs]")
{
    SpiFlashEmulator emu(10);
    emu.randomize(42);
    
    nvs_handle handle;
    const uint32_t NVS_FLASH_SECTOR = 5;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);
    TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN));
    
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
        
        TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN));
        
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

extern "C" void nvs_dump(const char *partName);

class RandomTest {
    
    static const size_t nKeys = 9;
    int32_t v1 = 0, v2 = 0;
    uint64_t v3 = 0, v4 = 0;
    static const size_t strBufLen = 1024;
    char v5[strBufLen], v6[strBufLen], v7[strBufLen], v8[strBufLen], v9[strBufLen];
    bool written[nKeys];
    
public:
    RandomTest()
    {
        std::fill_n(written, nKeys, false);
    }

    template<typename TGen>
    esp_err_t doRandomThings(nvs_handle handle, TGen gen, size_t& count) {
    
        const char* keys[] = {"foo", "bar", "longkey_0123456", "another key", "param1", "param2", "param3", "param4", "param5"};
        const ItemType types[] = {ItemType::I32, ItemType::I32, ItemType::U64, ItemType::U64, ItemType::SZ, ItemType::SZ, ItemType::SZ, ItemType::SZ, ItemType::SZ};
        
        void* values[] = {&v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8, &v9};
        
        const size_t nKeys = sizeof(keys) / sizeof(keys[0]);
        static_assert(nKeys == sizeof(types) / sizeof(types[0]), "");
        static_assert(nKeys == sizeof(values) / sizeof(values[0]), "");
        
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
                        REQUIRE(err == ESP_OK);
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
                        REQUIRE(err == ESP_OK);
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
                        REQUIRE(err == ESP_OK);
                        REQUIRE(strncmp(buf, reinterpret_cast<const char*>(values[index]), strBufLen) == 0);
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
                    
                    auto err = nvs_set_i32(handle, keys[index], val);
                    if (err == ESP_ERR_FLASH_OP_FAIL) {
                        return err;
                    }
                    if (err == ESP_ERR_NVS_REMOVE_FAILED) {
                        written[index] = true;
                        *reinterpret_cast<int32_t*>(values[index]) = val;
                        return ESP_ERR_FLASH_OP_FAIL;
                    }
                    REQUIRE(err == ESP_OK);
                    written[index] = true;
                    *reinterpret_cast<int32_t*>(values[index]) = val;
                    break;
                }
                    
                case ItemType::U64:
                {
                    uint64_t val = static_cast<uint64_t>(gen());
                    
                    auto err = nvs_set_u64(handle, keys[index], val);
                    if (err == ESP_ERR_FLASH_OP_FAIL) {
                        return err;
                    }
                    if (err == ESP_ERR_NVS_REMOVE_FAILED) {
                        written[index] = true;
                        *reinterpret_cast<uint64_t*>(values[index]) = val;
                        return ESP_ERR_FLASH_OP_FAIL;
                    }
                    REQUIRE(err == ESP_OK);
                    written[index] = true;
                    *reinterpret_cast<uint64_t*>(values[index]) = val;
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
                    buf[strLen] = 0;
                    
                    auto err = nvs_set_str(handle, keys[index], buf);
                    if (err == ESP_ERR_FLASH_OP_FAIL) {
                        return err;
                    }
                    if (err == ESP_ERR_NVS_REMOVE_FAILED) {
                        written[index] = true;
                        strncpy(reinterpret_cast<char*>(values[index]), buf, strBufLen);
                        return ESP_ERR_FLASH_OP_FAIL;
                    }
                    REQUIRE(err == ESP_OK);
                    written[index] = true;
                    strncpy(reinterpret_cast<char*>(values[index]), buf, strBufLen);
                    break;
                }
                    
                default:
                    assert(0);
            }
            return ESP_OK;
        };
        
        
        for (; count != 0; --count) {
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
};


TEST_CASE("monkey test", "[nvs][monkey]")
{
    std::random_device rd;
    std::mt19937 gen(rd());
    uint32_t seed = 3;
    gen.seed(seed);
    
    SpiFlashEmulator emu(10);
    emu.randomize(seed);
    emu.clearStats();
    
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);
    
    TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN));
    
    nvs_handle handle;
    TEST_ESP_OK(nvs_open("namespace1", NVS_READWRITE, &handle));
    RandomTest test;
    size_t count = 1000;
    CHECK(test.doRandomThings(handle, gen, count) == ESP_OK);
    
    s_perf << "Monkey test: nErase=" << emu.getEraseOps() << " nWrite=" << emu.getWriteOps() << std::endl;
}

TEST_CASE("test recovery from sudden poweroff", "[.][long][nvs][recovery][monkey]")
{
    std::random_device rd;
    std::mt19937 gen(rd());
    uint32_t seed = 3;
    gen.seed(seed);
    const size_t iter_count = 2000;
    
    SpiFlashEmulator emu(10);
    
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);
    
    size_t totalOps = 0;
    int lastPercent = -1;
    for (uint32_t errDelay = 0; ; ++errDelay) {
        INFO(errDelay);
        emu.randomize(seed);
        emu.clearStats();
        emu.failAfter(errDelay);
        RandomTest test;
        
        if (totalOps != 0) {
            int percent = errDelay * 100 / totalOps;
            if (percent > lastPercent) {
                printf("%d/%d (%d%%)\r\n", errDelay, static_cast<int>(totalOps), percent);
                lastPercent = percent;
            }
        }
        

        nvs_handle handle;
        size_t count = iter_count;

        if (nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN) == ESP_OK) {
            if (nvs_open("namespace1", NVS_READWRITE, &handle) == ESP_OK) {
                if(test.doRandomThings(handle, gen, count) != ESP_ERR_FLASH_OP_FAIL) {
                    nvs_close(handle);
                    break;
                }
                nvs_close(handle);
            }
        }
        
        TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN));
        TEST_ESP_OK(nvs_open("namespace1", NVS_READWRITE, &handle));
        auto res = test.doRandomThings(handle, gen, count);
        if (res != ESP_OK) {
            nvs_dump(NVS_DEFAULT_PART_NAME);
            CHECK(0);
        }
        nvs_close(handle);
        totalOps = emu.getEraseOps() + emu.getWriteBytes() / 4;
    }
}

TEST_CASE("test for memory leaks in open/set", "[leaks]")
{
    SpiFlashEmulator emu(10);
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);
    TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN));
    
    for (int i = 0; i < 100000; ++i) {
        nvs_handle light_handle = 0;
        char lightbulb[1024] = {12, 13, 14, 15, 16};
        TEST_ESP_OK(nvs_open("light", NVS_READWRITE, &light_handle));
        TEST_ESP_OK(nvs_set_blob(light_handle, "key", lightbulb, sizeof(lightbulb)));
        TEST_ESP_OK(nvs_commit(light_handle));
        nvs_close(light_handle);
    }
}

TEST_CASE("duplicate items are removed", "[nvs][dupes]")
{
    SpiFlashEmulator emu(3);
    {
        // create one item
        nvs::Page p;
        p.load(0);
        p.writeItem<uint8_t>(1, "opmode", 3);
    }
    {
        // add another two without deleting the first one
        nvs::Item item(1, ItemType::U8, 1, "opmode");
        item.data[0] = 2;
        item.crc32 = item.calculateCrc32();
        emu.write(3 * 32, reinterpret_cast<const uint32_t*>(&item), sizeof(item));
        emu.write(4 * 32, reinterpret_cast<const uint32_t*>(&item), sizeof(item));
        uint32_t mask = 0xFFFFFFEA;
        emu.write(32, &mask, 4);
    }
    {
        // load page and check that second item persists
        nvs::Storage s;
        s.init(0, 3);
        uint8_t val;
        ESP_ERROR_CHECK(s.readItem(1, "opmode", val));
        CHECK(val == 2);
    }
    {
        Page p;
        p.load(0);
        CHECK(p.getErasedEntryCount() == 2);
        CHECK(p.getUsedEntryCount() == 1);
    }
}

TEST_CASE("recovery after failure to write data", "[nvs]")
{
    SpiFlashEmulator emu(3);
    const char str[] = "value 0123456789abcdef012345678value 0123456789abcdef012345678";

    // make flash write fail exactly in Page::writeEntryData
    emu.failAfter(17);
    {
        Storage storage;
        TEST_ESP_OK(storage.init(0, 3));
        
        TEST_ESP_ERR(storage.writeItem(1, ItemType::SZ, "key", str, strlen(str)), ESP_ERR_FLASH_OP_FAIL);
        
        // check that repeated operations cause an error
        TEST_ESP_ERR(storage.writeItem(1, ItemType::SZ, "key", str, strlen(str)), ESP_ERR_NVS_INVALID_STATE);
        
        uint8_t val;
        TEST_ESP_ERR(storage.readItem(1, ItemType::U8, "key", &val, sizeof(val)), ESP_ERR_NVS_NOT_FOUND);
    }
    {
        // load page and check that data was erased
        Page p;
        p.load(0);
        CHECK(p.getErasedEntryCount() == 3);
        CHECK(p.getUsedEntryCount() == 0);
        
        // try to write again
        TEST_ESP_OK(p.writeItem(1, ItemType::SZ, "key", str, strlen(str)));
    }
}

TEST_CASE("crc error in variable length item is handled", "[nvs]")
{
    SpiFlashEmulator emu(3);
    const uint64_t before_val = 0xbef04e;
    const uint64_t after_val = 0xaf7e4;
    // write some data
    {
        Page p;
        p.load(0);
        TEST_ESP_OK(p.writeItem<uint64_t>(0, "before", before_val));
        const char* str = "foobar";
        TEST_ESP_OK(p.writeItem(0, ItemType::SZ, "key", str, strlen(str)));
        TEST_ESP_OK(p.writeItem<uint64_t>(0, "after", after_val));
    }
    // corrupt some data
    uint32_t w;
    CHECK(emu.read(&w, 32 * 3 + 8, sizeof(w)));
    w &= 0xf000000f;
    CHECK(emu.write(32 * 3 + 8, &w, sizeof(w)));
    // load and check
    {
        Page p;
        p.load(0);
        CHECK(p.getUsedEntryCount() == 2);
        CHECK(p.getErasedEntryCount() == 2);

        uint64_t val;
        TEST_ESP_OK(p.readItem<uint64_t>(0, "before", val));
        CHECK(val == before_val);
        TEST_ESP_ERR(p.findItem(0, ItemType::SZ, "key"), ESP_ERR_NVS_NOT_FOUND);
        TEST_ESP_OK(p.readItem<uint64_t>(0, "after", val));
        CHECK(val == after_val);
    }
}


TEST_CASE("read/write failure (TW8406)", "[nvs]")
{
    SpiFlashEmulator emu(3);
    nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, 0, 3);
    for (int attempts = 0; attempts < 3; ++attempts) {
        int i = 0;
        nvs_handle light_handle = 0;
        char key[15] = {0};
        char data[76] = {12, 13, 14, 15, 16};
        uint8_t number = 20;
        size_t data_len = sizeof(data);
        
        ESP_ERROR_CHECK(nvs_open("LIGHT", NVS_READWRITE, &light_handle));
        ESP_ERROR_CHECK(nvs_set_u8(light_handle, "RecordNum", number));
        for (i = 0; i < number; ++i) {
            sprintf(key, "light%d", i);
            ESP_ERROR_CHECK(nvs_set_blob(light_handle, key, data, sizeof(data)));
        }
        nvs_commit(light_handle);
        
        uint8_t get_number = 0;
        ESP_ERROR_CHECK(nvs_get_u8(light_handle, "RecordNum", &get_number));
        REQUIRE(number == get_number);
        for (i = 0; i < number; ++i) {
            char data[76] = {0};
            sprintf(key, "light%d", i);
            ESP_ERROR_CHECK(nvs_get_blob(light_handle, key, data, &data_len));
        }
        nvs_close(light_handle);
    }
}

TEST_CASE("nvs_flash_init checks for an empty page", "[nvs]")
{
    const size_t blob_size = Page::BLOB_MAX_SIZE;
    uint8_t blob[blob_size] = {0};
    SpiFlashEmulator emu(5);
    TEST_ESP_OK( nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, 0, 5) );
    nvs_handle handle;
    TEST_ESP_OK( nvs_open("test", NVS_READWRITE, &handle) );
    // Fill first page
    TEST_ESP_OK( nvs_set_blob(handle, "1a", blob, blob_size) );
    TEST_ESP_OK( nvs_set_blob(handle, "1b", blob, blob_size) );
    // Fill second page
    TEST_ESP_OK( nvs_set_blob(handle, "2a", blob, blob_size) );
    TEST_ESP_OK( nvs_set_blob(handle, "2b", blob, blob_size) );
    // Fill third page
    TEST_ESP_OK( nvs_set_blob(handle, "3a", blob, blob_size) );
    TEST_ESP_OK( nvs_set_blob(handle, "3b", blob, blob_size) );
    TEST_ESP_OK( nvs_commit(handle) );
    nvs_close(handle);
    // first two pages are now full, third one is writable, last two are empty
    // init should fail
    TEST_ESP_ERR( nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, 0, 3), ESP_ERR_NVS_NO_FREE_PAGES );
}

TEST_CASE("multiple partitions access check", "[nvs]")
{
    SpiFlashEmulator emu(10);
    TEST_ESP_OK( nvs_flash_init_custom("nvs1", 0, 5) );
    TEST_ESP_OK( nvs_flash_init_custom("nvs2", 5, 5) );
    nvs_handle handle1, handle2;
    TEST_ESP_OK( nvs_open_from_partition("nvs1", "test", NVS_READWRITE, &handle1) );
    TEST_ESP_OK( nvs_open_from_partition("nvs2", "test", NVS_READWRITE, &handle2) );
    TEST_ESP_OK( nvs_set_i32(handle1, "foo", 0xdeadbeef));
    TEST_ESP_OK( nvs_set_i32(handle2, "foo", 0xcafebabe));
    int32_t v1, v2;
    TEST_ESP_OK( nvs_get_i32(handle1, "foo", &v1));
    TEST_ESP_OK( nvs_get_i32(handle2, "foo", &v2));
    CHECK(v1 == 0xdeadbeef);
    CHECK(v2 == 0xcafebabe);
}

TEST_CASE("dump all performance data", "[nvs]")
{
    std::cout << "====================" << std::endl << "Dumping benchmarks" << std::endl;
    std::cout << s_perf.str() << std::endl;
    std::cout << "====================" << std::endl;
}
