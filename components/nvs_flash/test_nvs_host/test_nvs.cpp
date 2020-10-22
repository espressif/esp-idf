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
#include "sdkconfig.h"
#ifdef CONFIG_NVS_ENCRYPTION
#include "nvs_encr.hpp"
#endif
#include "spi_flash_emulation.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <string>

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
    item1.chunkIndex = 0xff;
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
        snprintf(name, sizeof(name), "i%ld", (long int)i);
        CHECK(page.writeItem(1, name, i) == ESP_OK);
    }
    CHECK(page.getUsedEntryCount() == Page::ENTRY_COUNT - 1);
    CHECK(page.getErasedEntryCount() == 1);
    for (size_t i = 0; i < Page::ENTRY_COUNT - 2; ++i) {
        char name[16];
        snprintf(name, sizeof(name), "i%ld", (long int)i);
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
        snprintf(name, sizeof(name), "i%ld", (long int)i);
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

    char buf[4096] = { 0 };
    // There are two potential errors here:
    // - not enough space in the page (because one value has been written already)
    // - value is too long
    // Check that the second one is actually returned.
    TEST_ESP_ERR(page.writeItem(1, ItemType::BLOB, "2", buf, Page::ENTRY_COUNT * Page::ENTRY_SIZE), ESP_ERR_NVS_VALUE_TOO_LONG);
    // Should fail as well
    TEST_ESP_ERR(page.writeItem(1, ItemType::BLOB, "2", buf, Page::CHUNK_MAX_SIZE + 1), ESP_ERR_NVS_VALUE_TOO_LONG);
    TEST_ESP_OK(page.writeItem(1, ItemType::BLOB, "2", buf, Page::CHUNK_MAX_SIZE));
}

TEST_CASE("Page handles invalid CRC of variable length items", "[nvs][cur]")
{
    SpiFlashEmulator emu(4);
    {
        Page page;
        TEST_ESP_OK(page.load(0));
        char buf[128] = {0};
        TEST_ESP_OK(page.writeItem(1, ItemType::BLOB, "1", buf, sizeof(buf)));
    }
    // corrupt header of the item (64 is the offset of the first item in page)
    uint32_t overwrite_buf = 0;
    emu.write(64, &overwrite_buf, 4);
    // load page again
    {
        Page page;
        TEST_ESP_OK(page.load(0));
    }
}

class HashListTestHelper : public HashList
{
    public:
        size_t getBlockCount()
        {
            return mBlockList.size();
        }
};

TEST_CASE("HashList is cleaned up as soon as items are erased", "[nvs]")
{
    HashListTestHelper hashlist;
    // Add items
    const size_t count = 128;
    for (size_t i = 0; i < count; ++i) {
        char key[16];
        snprintf(key, sizeof(key), "i%ld", (long int)i);
        Item item(1, ItemType::U32, 1, key);
        hashlist.insert(item, i);
    }
    INFO("Added " << count << " items, " << hashlist.getBlockCount() << " blocks");
    // Remove them in reverse order
    for (size_t i = count; i > 0; --i) {
        hashlist.erase(i - 1, true);
    }
    CHECK(hashlist.getBlockCount() == 0);
    // Add again
    for (size_t i = 0; i < count; ++i) {
        char key[16];
        snprintf(key, sizeof(key), "i%ld", (long int)i);
        Item item(1, ItemType::U32, 1, key);
        hashlist.insert(item, i);
    }
    INFO("Added " << count << " items, " << hashlist.getBlockCount() << " blocks");
    // Remove them in the same order
    for (size_t i = 0; i < count; ++i) {
        hashlist.erase(i, true);
    }
    CHECK(hashlist.getBlockCount() == 0);
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
    CHECK(storage.writeItem(1, "bar", ++bar) == ESP_OK);
    CHECK(storage.writeItem(1, "bar", ++bar) == ESP_OK);

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
    CHECK(storage.writeItem(1, "bar", ++bar) == ESP_OK);
    for (size_t i = 0; i < Page::ENTRY_COUNT; ++i) {
        CHECK(storage.writeItem(1, "foo", static_cast<int>(++bar)) == ESP_OK);
    }
    CHECK(storage.writeItem(1, "bar", ++bar) == ESP_OK);

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
    uint8_t bigdata[(Page::CHUNK_MAX_SIZE - Page::ENTRY_SIZE)/2] = {0};
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

TEST_CASE("erase operations are distributed among sectors", "[nvs]")
{
    const size_t sectors = 6;
    SpiFlashEmulator emu(sectors);
    Storage storage;
    CHECK(storage.init(0, sectors) == ESP_OK);

    /* Fill some part of storage with static values */
    const size_t static_sectors = 2;
    for (size_t i = 0; i < static_sectors * Page::ENTRY_COUNT; ++i) {
        char name[Item::MAX_KEY_LENGTH];
        snprintf(name, sizeof(name), "static%d", (int) i);
        REQUIRE(storage.writeItem(1, name, i) == ESP_OK);
    }

    /* Now perform many write operations */
    const size_t write_ops = 2000;
    for (size_t i = 0; i < write_ops; ++i) {
        REQUIRE(storage.writeItem(1, "value", i) == ESP_OK);
    }

    /* Check that erase counts are distributed between the remaining sectors */
    const size_t max_erase_cnt = write_ops / Page::ENTRY_COUNT / (sectors - static_sectors) + 1;
    for (size_t i = 0; i < sectors; ++i) {
        auto erase_cnt = emu.getSectorEraseCount(i);
        INFO("Sector " << i << " erased " << erase_cnt);
        CHECK(erase_cnt <= max_erase_cnt);
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

TEST_CASE("partition name is deep copy", "[nvs]")
{
    SpiFlashEmulator emu(10);
    char partition_name[16];
    strcpy(partition_name, "const_name");

    nvs_handle_t handle_1;
    nvs_handle_t handle_2;
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);

    TEST_ESP_OK(nvs_flash_init_custom(partition_name, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN));

    strcpy(partition_name, "just_kidding");

    TEST_ESP_OK(nvs_open_from_partition("const_name", "test", NVS_READWRITE, &handle_1));
    CHECK(nvs_open_from_partition("just_kidding", "test", NVS_READWRITE, &handle_2) == ESP_ERR_NVS_PART_NOT_FOUND);

    nvs_close(handle_1);
    nvs_close(handle_2);

    nvs_flash_deinit_partition("const_name");
    nvs_flash_deinit_partition("just_kidding"); // just in case, try not to affect other tests
}

TEST_CASE("namespace name is deep copy", "[nvs]")
{
    SpiFlashEmulator emu(10);
    char ns_name[16];
    strcpy(ns_name, "const_name");

    nvs_handle_t handle_1;
    nvs_handle_t handle_2;
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);

    TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN));

    TEST_ESP_OK(nvs_open("const_name", NVS_READWRITE, &handle_1));
    strcpy(ns_name, "just_kidding");

    CHECK(nvs_open("just_kidding", NVS_READONLY, &handle_2) == ESP_ERR_NVS_NOT_FOUND);

    nvs_close(handle_1);
    nvs_close(handle_2);

    nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME);
}

TEST_CASE("Partition name no longer than 16 characters", "[nvs]")
{
    SpiFlashEmulator emu(10);
    const char *TOO_LONG_NAME = "0123456789abcdefg";

    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);

    CHECK(nvs_flash_init_custom(TOO_LONG_NAME, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN)
            == ESP_ERR_INVALID_ARG);

    nvs_flash_deinit_partition(TOO_LONG_NAME); // just in case
}

TEST_CASE("readonly handle fails on writing", "[nvs]")
{
    SpiFlashEmulator emu(10);
    const char* str = "value 0123456789abcdef0123456789abcdef";
    const uint8_t blob[8] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7};

    nvs_handle_t handle_1;
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);

    TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN));

    // first, creating namespace...
    TEST_ESP_OK(nvs_open("ro_ns", NVS_READWRITE, &handle_1));
    nvs_close(handle_1);

    TEST_ESP_OK(nvs_open("ro_ns", NVS_READONLY, &handle_1));
    TEST_ESP_ERR(nvs_set_i32(handle_1, "key", 47), ESP_ERR_NVS_READ_ONLY);
    TEST_ESP_ERR(nvs_set_str(handle_1, "key", str), ESP_ERR_NVS_READ_ONLY);
    TEST_ESP_ERR(nvs_set_blob(handle_1, "key", blob, 8), ESP_ERR_NVS_READ_ONLY);

    nvs_close(handle_1);

    // without deinit it affects "nvs api tests"
    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
}

TEST_CASE("nvs api tests", "[nvs]")
{
    SpiFlashEmulator emu(10);
    emu.randomize(100);

    nvs_handle_t handle_1;
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

    nvs_handle_t handle_2;
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

    size_t buf_len_long = buf_len + 1;
    TEST_ESP_OK(nvs_get_str(handle_2, "key", buf, &buf_len_long));
    CHECK(buf_len_long == buf_len);

    TEST_ESP_OK(nvs_get_str(handle_2, "key", buf, &buf_len));

    CHECK(0 == strcmp(buf, str));
    nvs_close(handle_1);
    nvs_close(handle_2);

    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
}

TEST_CASE("nvs iterators tests", "[nvs]")
{
    SpiFlashEmulator emu(5);

    const uint32_t NVS_FLASH_SECTOR = 0;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 5;
    emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);

    for (uint16_t i = NVS_FLASH_SECTOR; i < NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN; ++i) {
        spi_flash_erase_sector(i);
    }
    TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN));

    nvs_iterator_t it;
    nvs_entry_info_t info;
    nvs_handle handle_1;
    nvs_handle handle_2;
    const  uint32_t blob = 0x11223344;
    const char *name_1 = "namespace1";
    const char *name_2 = "namespace2";
    TEST_ESP_OK(nvs_open(name_1, NVS_READWRITE, &handle_1));
    TEST_ESP_OK(nvs_open(name_2, NVS_READWRITE, &handle_2));

    TEST_ESP_OK(nvs_set_i8(handle_1, "value1", -11));
    TEST_ESP_OK(nvs_set_u8(handle_1, "value2", 11));
    TEST_ESP_OK(nvs_set_i16(handle_1, "value3", 1234));
    TEST_ESP_OK(nvs_set_u16(handle_1, "value4", -1234));
    TEST_ESP_OK(nvs_set_i32(handle_1, "value5", -222));
    TEST_ESP_OK(nvs_set_i32(handle_1, "value6", -222));
    TEST_ESP_OK(nvs_set_i32(handle_1, "value7", -222));
    TEST_ESP_OK(nvs_set_u32(handle_1, "value8", 222));
    TEST_ESP_OK(nvs_set_u32(handle_1, "value9", 222));
    TEST_ESP_OK(nvs_set_str(handle_1, "value10", "foo"));
    TEST_ESP_OK(nvs_set_blob(handle_1, "value11", &blob, sizeof(blob)));
    TEST_ESP_OK(nvs_set_i32(handle_2, "value1", -111));
    TEST_ESP_OK(nvs_set_i32(handle_2, "value2", -111));
    TEST_ESP_OK(nvs_set_i64(handle_2, "value3", -555));
    TEST_ESP_OK(nvs_set_u64(handle_2, "value4", 555));

    auto entry_count = [](const char *part, const char *name, nvs_type_t type)-> int {
        int count;
        nvs_iterator_t it = nvs_entry_find(part, name, type);
        for (count = 0; it != nullptr; count++) {
            it = nvs_entry_next(it);
        }
        return count;
    };

   SECTION("Number of entries found for specified namespace and type is correct")
   {
        CHECK(nvs_entry_find("", NULL, NVS_TYPE_ANY) == NULL);
        CHECK(entry_count(NVS_DEFAULT_PART_NAME, NULL, NVS_TYPE_ANY) == 15);
        CHECK(entry_count(NVS_DEFAULT_PART_NAME, name_1, NVS_TYPE_ANY) == 11);
        CHECK(entry_count(NVS_DEFAULT_PART_NAME, name_1, NVS_TYPE_I32) == 3);
        CHECK(entry_count(NVS_DEFAULT_PART_NAME, NULL, NVS_TYPE_I32) == 5);
        CHECK(entry_count(NVS_DEFAULT_PART_NAME, NULL, NVS_TYPE_U64) == 1);
   }

   SECTION("New entry is not created when existing key-value pair is set")
   {
        CHECK(entry_count(NVS_DEFAULT_PART_NAME, name_2, NVS_TYPE_ANY) == 4);
        TEST_ESP_OK(nvs_set_i32(handle_2, "value1", -222));
        CHECK(entry_count(NVS_DEFAULT_PART_NAME, name_2, NVS_TYPE_ANY) == 4);
   }

    SECTION("Number of entries found decrease when entry is erased")
    {
        CHECK(entry_count(NVS_DEFAULT_PART_NAME, NULL, NVS_TYPE_U64) == 1);
        TEST_ESP_OK(nvs_erase_key(handle_2, "value4"));
        CHECK(entry_count(NVS_DEFAULT_PART_NAME, "", NVS_TYPE_U64) == 0);
    }

    SECTION("All fields of nvs_entry_info_t structure are correct")
    {
        it = nvs_entry_find(NVS_DEFAULT_PART_NAME, name_1, NVS_TYPE_I32);
        CHECK(it != nullptr);
        string key = "value5";
        do {
            nvs_entry_info(it, &info);

            CHECK(string(name_1) == info.namespace_name);
            CHECK(key == info.key);
            CHECK(info.type == NVS_TYPE_I32);

            it = nvs_entry_next(it);
            key[5]++;
        } while (it != NULL);
        nvs_release_iterator(it);
    }

    SECTION("Entry info is not affected by subsequent erase")
    {
        nvs_entry_info_t info_after_erase;

        it = nvs_entry_find(NVS_DEFAULT_PART_NAME, name_1, NVS_TYPE_ANY);
        nvs_entry_info(it, &info);
        TEST_ESP_OK(nvs_erase_key(handle_1, "value1"));
        nvs_entry_info(it, &info_after_erase);
        CHECK(memcmp(&info, &info_after_erase, sizeof(info)) == 0);
        nvs_release_iterator(it);
    }

    SECTION("Entry info is not affected by subsequent set")
    {
        nvs_entry_info_t info_after_set;

        it = nvs_entry_find(NVS_DEFAULT_PART_NAME, name_1, NVS_TYPE_ANY);
        nvs_entry_info(it, &info);
        TEST_ESP_OK(nvs_set_u8(handle_1, info.key, 44));
        nvs_entry_info(it, &info_after_set);
        CHECK(memcmp(&info, &info_after_set, sizeof(info)) == 0);
        nvs_release_iterator(it);
    }


    SECTION("Iterating over multiple pages works correctly")
    {
        nvs_handle handle_3;
        const char *name_3 = "namespace3";
        const int entries_created = 250;

        TEST_ESP_OK(nvs_open(name_3, NVS_READWRITE, &handle_3));
        for  (size_t i = 0; i < entries_created; i++) {
            TEST_ESP_OK(nvs_set_u8(handle_3, to_string(i).c_str(), 123));
        }

        int entries_found = 0;
        it = nvs_entry_find(NVS_DEFAULT_PART_NAME, name_3, NVS_TYPE_ANY);
        while(it != nullptr) {
            entries_found++;
            it = nvs_entry_next(it);
        }
        CHECK(entries_created == entries_found);

        nvs_release_iterator(it);
        nvs_close(handle_3);
    }

    SECTION("Iterating over multi-page blob works correctly")
    {
        nvs_handle handle_3;
        const char *name_3 = "namespace3";
        const uint8_t multipage_blob[4096 * 2] = { 0 };
        const int NUMBER_OF_ENTRIES_PER_PAGE = 125;
        size_t occupied_entries;

        TEST_ESP_OK(nvs_open(name_3, NVS_READWRITE, &handle_3));
        nvs_set_blob(handle_3, "blob", multipage_blob, sizeof(multipage_blob));
        TEST_ESP_OK(nvs_get_used_entry_count(handle_3, &occupied_entries));
        CHECK(occupied_entries > NUMBER_OF_ENTRIES_PER_PAGE *  2);

        CHECK(entry_count(NVS_DEFAULT_PART_NAME, name_3, NVS_TYPE_BLOB) == 1);

        nvs_close(handle_3);
    }

    nvs_close(handle_1);
    nvs_close(handle_2);

    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
}

TEST_CASE("Iterator with not matching type iterates correctly", "[nvs]")
{
    SpiFlashEmulator emu(5);
    nvs_iterator_t it;
    nvs_handle_t my_handle;
    const char* NAMESPACE = "test_ns_4";

    const uint32_t NVS_FLASH_SECTOR = 0;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 5;
    emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);

    for (uint16_t i = NVS_FLASH_SECTOR; i < NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN; ++i) {
        spi_flash_erase_sector(i);
    }
    TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN));

    // writing string to namespace (a type which spans multiple entries)
    TEST_ESP_OK(nvs_open(NAMESPACE, NVS_READWRITE, &my_handle));
    TEST_ESP_OK(nvs_set_str(my_handle, "test-string", "InitString0"));
    TEST_ESP_OK(nvs_commit(my_handle));
    nvs_close(my_handle);

    it = nvs_entry_find(NVS_DEFAULT_PART_NAME, NAMESPACE, NVS_TYPE_I32);
    CHECK(it == NULL);

    // re-init to trigger cleaning up of broken items -> a corrupted string will be erased
    nvs_flash_deinit();
    TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN));

    it = nvs_entry_find(NVS_DEFAULT_PART_NAME, NAMESPACE, NVS_TYPE_STR);
    CHECK(it != NULL);
    nvs_release_iterator(it);

    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
}

TEST_CASE("wifi test", "[nvs]")
{
    SpiFlashEmulator emu(10);
    emu.randomize(10);

    const uint32_t NVS_FLASH_SECTOR = 5;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);
    TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN));

    nvs_handle_t misc_handle;
    TEST_ESP_OK(nvs_open("nvs.net80211", NVS_READWRITE, &misc_handle));
    char log[33];
    size_t log_size = sizeof(log);
    TEST_ESP_ERR(nvs_get_str(misc_handle, "log", log, &log_size), ESP_ERR_NVS_NOT_FOUND);
    strcpy(log, "foobarbazfizzz");
    TEST_ESP_OK(nvs_set_str(misc_handle, "log", log));

    nvs_handle_t net80211_handle;
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

    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
}

TEST_CASE("writing the identical content does not write or erase", "[nvs]")
{
    SpiFlashEmulator emu(20);

    const uint32_t NVS_FLASH_SECTOR = 5;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 10;
    emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);
    TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN));

    nvs_handle misc_handle;
    TEST_ESP_OK(nvs_open("test", NVS_READWRITE, &misc_handle));

    // Test writing a u8 twice, then changing it
    nvs_set_u8(misc_handle, "test_u8", 8);
    emu.clearStats();
    nvs_set_u8(misc_handle, "test_u8", 8);
    CHECK(emu.getWriteOps() == 0);
    CHECK(emu.getEraseOps() == 0);
    CHECK(emu.getReadOps()  != 0);
    emu.clearStats();
    nvs_set_u8(misc_handle, "test_u8", 9);
    CHECK(emu.getWriteOps() != 0);
    CHECK(emu.getReadOps()  != 0);

    // Test writing a string twice, then changing it
    static const char *test[2] = {"Hello world.", "Hello world!"};
    nvs_set_str(misc_handle, "test_str", test[0]);
    emu.clearStats();
    nvs_set_str(misc_handle, "test_str", test[0]);
    CHECK(emu.getWriteOps() == 0);
    CHECK(emu.getEraseOps() == 0);
    CHECK(emu.getReadOps()  != 0);
    emu.clearStats();
    nvs_set_str(misc_handle, "test_str", test[1]);
    CHECK(emu.getWriteOps() != 0);
    CHECK(emu.getReadOps()  != 0);

    // Test writing a multi-page blob, then changing it
    uint8_t blob[Page::CHUNK_MAX_SIZE * 3] = {0};
    memset(blob, 1, sizeof(blob));
    nvs_set_blob(misc_handle, "test_blob", blob, sizeof(blob));
    emu.clearStats();
    nvs_set_blob(misc_handle, "test_blob", blob, sizeof(blob));
    CHECK(emu.getWriteOps() == 0);
    CHECK(emu.getEraseOps() == 0);
    CHECK(emu.getReadOps()  != 0);
    blob[sizeof(blob) - 1]++;
    emu.clearStats();
    nvs_set_blob(misc_handle, "test_blob", blob, sizeof(blob));
    CHECK(emu.getWriteOps() != 0);
    CHECK(emu.getReadOps()  != 0);

    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
}

TEST_CASE("can init storage from flash with random contents", "[nvs]")
{
    SpiFlashEmulator emu(10);
    emu.randomize(42);

    nvs_handle_t handle;
    const uint32_t NVS_FLASH_SECTOR = 5;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);
    TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN));

    TEST_ESP_OK(nvs_open("nvs.net80211", NVS_READWRITE, &handle));

    uint8_t opmode = 2;
    if (nvs_get_u8(handle, "wifi.opmode", &opmode) != ESP_OK) {
        TEST_ESP_OK(nvs_set_u8(handle, "wifi.opmode", opmode));
    }

    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
}


TEST_CASE("nvs api tests, starting with random data in flash", "[nvs][long]")
{
    const size_t testIters = 3000;
    int lastPercent = -1;
    for (size_t count = 0; count < testIters; ++count) {
        int percentDone = (int) (count * 100 / testIters);
        if (percentDone != lastPercent) {
            lastPercent = percentDone;
            printf("%d%%\n", percentDone);
        }
        SpiFlashEmulator emu(10);
        emu.randomize(static_cast<uint32_t>(count));

        const uint32_t NVS_FLASH_SECTOR = 6;
        const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
        emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);

        TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN));

        nvs_handle_t handle_1;
        TEST_ESP_ERR(nvs_open("namespace1", NVS_READONLY, &handle_1), ESP_ERR_NVS_NOT_FOUND);

        TEST_ESP_OK(nvs_open("namespace1", NVS_READWRITE, &handle_1));
        TEST_ESP_OK(nvs_set_i32(handle_1, "foo", 0x12345678));
        for (size_t i = 0; i < 500; ++i) {
            nvs_handle_t handle_2;
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

        TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
    }
}
extern "C" void nvs_dump(const char *partName);

class RandomTest {

    static const size_t nKeys = 11;
    int32_t v1 = 0, v2 = 0;
    uint64_t v3 = 0, v4 = 0;
    static const size_t strBufLen = 1024;
    static const size_t smallBlobLen = Page::CHUNK_MAX_SIZE / 3;
    static const size_t largeBlobLen = Page::CHUNK_MAX_SIZE * 3;
    char v5[strBufLen], v6[strBufLen], v7[strBufLen], v8[strBufLen], v9[strBufLen];
    uint8_t v10[smallBlobLen], v11[largeBlobLen];
    bool written[nKeys];

public:
    RandomTest()
    {
        std::fill_n(written, nKeys, false);
    }

    template<typename TGen>
    esp_err_t doRandomThings(nvs_handle_t handle, TGen gen, size_t& count) {

        const char* keys[] = {"foo", "bar", "longkey_0123456", "another key", "param1", "param2", "param3", "param4", "param5", "singlepage", "multipage"};
        const ItemType types[] = {ItemType::I32, ItemType::I32, ItemType::U64, ItemType::U64, ItemType::SZ, ItemType::SZ, ItemType::SZ, ItemType::SZ, ItemType::SZ, ItemType::BLOB, ItemType::BLOB};

        void* values[] = {&v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8, &v9, &v10, &v11};

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

                case ItemType::BLOB:
                {
                    uint32_t blobBufLen = 0;
                    if(strncmp(keys[index],"singlepage", sizeof("singlepage")) == 0) {
                       blobBufLen = smallBlobLen ;
                    } else {
                       blobBufLen = largeBlobLen ;

                    }
                    uint8_t buf[blobBufLen];
                    memset(buf, 0, blobBufLen);

                    size_t len = blobBufLen;
                    auto err = nvs_get_blob(handle, keys[index], buf, &len);
                    if (err == ESP_ERR_FLASH_OP_FAIL) {
                        return err;
                    }
                    if (!written[index]) {
                        REQUIRE(err == ESP_ERR_NVS_NOT_FOUND);
                    }
                    else {
                        REQUIRE(err == ESP_OK);
                        REQUIRE(memcmp(buf, reinterpret_cast<const uint8_t*>(values[index]), blobBufLen) == 0);
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

                case ItemType::BLOB:
                {
                    uint32_t blobBufLen = 0;
                    if(strncmp(keys[index],"singlepage", sizeof("singlepage")) == 0) {
                       blobBufLen = smallBlobLen ;
                    } else {
                       blobBufLen = largeBlobLen ;
                    }
                    uint8_t buf[blobBufLen];
                    memset(buf, 0, blobBufLen);
                    size_t blobLen = gen() % blobBufLen;
                    std::generate_n(buf, blobLen, [&]() -> uint8_t {
                        return static_cast<uint8_t>(gen() % 256);
                    });

                    auto err = nvs_set_blob(handle, keys[index], buf, blobLen);
                    if (err == ESP_ERR_FLASH_OP_FAIL) {
                        return err;
                    }
                    if (err == ESP_ERR_NVS_REMOVE_FAILED) {
                        written[index] = true;
                        memcpy(reinterpret_cast<uint8_t*>(values[index]), buf, blobBufLen);
                        return ESP_ERR_FLASH_OP_FAIL;
                    }
                    REQUIRE(err == ESP_OK);
                    written[index] = true;
                    memcpy(reinterpret_cast<char*>(values[index]), buf, blobBufLen);
                    break;
                }

                default:
                    assert(0);
            }
            return ESP_OK;
        };


        for (; count != 0; --count) {
            size_t index = gen() % (nKeys);
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

    esp_err_t handleExternalWriteAtIndex(uint8_t index, const void* value, const size_t len ) {
        if(index == 9) {  /* This is only done for small-page blobs for now*/
            if(len > smallBlobLen) {
                return ESP_FAIL;
            }
            memcpy(v10, value, len);
            written[index] = true;
            return ESP_OK;
        } else {
            return ESP_FAIL;
        }
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

    const uint32_t NVS_FLASH_SECTOR = 2;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 8;
    emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);

    TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN));

    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open("namespace1", NVS_READWRITE, &handle));
    RandomTest test;
    size_t count = 1000;
    CHECK(test.doRandomThings(handle, gen, count) == ESP_OK);

    s_perf << "Monkey test: nErase=" << emu.getEraseOps() << " nWrite=" << emu.getWriteOps() << std::endl;

    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
}

TEST_CASE("test recovery from sudden poweroff", "[long][nvs][recovery][monkey]")
{
    std::random_device rd;
    std::mt19937 gen(rd());
    uint32_t seed = 3;
    gen.seed(seed);
    const size_t iter_count = 2000;

    SpiFlashEmulator emu(10);

    const uint32_t NVS_FLASH_SECTOR = 2;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 8;

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


        nvs_handle_t handle;
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

        TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
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
        nvs_handle_t light_handle = 0;
        char lightbulb[1024] = {12, 13, 14, 15, 16};
        TEST_ESP_OK(nvs_open("light", NVS_READWRITE, &light_handle));
        TEST_ESP_OK(nvs_set_blob(light_handle, "key", lightbulb, sizeof(lightbulb)));
        TEST_ESP_OK(nvs_commit(light_handle));
        nvs_close(light_handle);
    }

    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
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

TEST_CASE("crc errors in item header are handled", "[nvs]")
{
    SpiFlashEmulator emu(3);
    Storage storage;
    // prepare some data
    TEST_ESP_OK(storage.init(0, 3));
    TEST_ESP_OK(storage.writeItem(0, "ns1", static_cast<uint8_t>(1)));
    TEST_ESP_OK(storage.writeItem(1, "value1", static_cast<uint32_t>(1)));
    TEST_ESP_OK(storage.writeItem(1, "value2", static_cast<uint32_t>(2)));

    // corrupt item header
    uint32_t val = 0;
    emu.write(32 * 3, &val, 4);

    // check that storage can recover
    TEST_ESP_OK(storage.init(0, 3));
    TEST_ESP_OK(storage.readItem(1, "value2", val));
    CHECK(val == 2);
    // check that the corrupted item is no longer present
    TEST_ESP_ERR(ESP_ERR_NVS_NOT_FOUND, storage.readItem(1, "value1", val));

    // add more items to make the page full
    for (size_t i = 0; i < Page::ENTRY_COUNT; ++i) {
        char item_name[Item::MAX_KEY_LENGTH + 1];
        snprintf(item_name, sizeof(item_name), "item_%ld", (long int)i);
        TEST_ESP_OK(storage.writeItem(1, item_name, static_cast<uint32_t>(i)));
    }

    // corrupt another item on the full page
    val = 0;
    emu.write(32 * 4, &val, 4);

    // check that storage can recover
    TEST_ESP_OK(storage.init(0, 3));
    // check that the corrupted item is no longer present
    TEST_ESP_ERR(ESP_ERR_NVS_NOT_FOUND, storage.readItem(1, "value2", val));
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
        nvs_handle_t light_handle = 0;
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

    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
}

TEST_CASE("nvs_flash_init checks for an empty page", "[nvs]")
{
    const size_t blob_size = Page::CHUNK_MAX_SIZE;
    uint8_t blob[blob_size] = {0};
    SpiFlashEmulator emu(5);
    TEST_ESP_OK( nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, 0, 5) );
    nvs_handle_t handle;
    TEST_ESP_OK( nvs_open("test", NVS_READWRITE, &handle) );
    // Fill first page
    TEST_ESP_OK( nvs_set_blob(handle, "1a", blob, blob_size) );
    // Fill second page
    TEST_ESP_OK( nvs_set_blob(handle, "2a", blob, blob_size) );
    // Fill third page
    TEST_ESP_OK( nvs_set_blob(handle, "3a", blob, blob_size) );
    TEST_ESP_OK( nvs_commit(handle) );
    nvs_close(handle);
    // first two pages are now full, third one is writable, last two are empty
    // init should fail
    TEST_ESP_ERR( nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, 0, 3), ESP_ERR_NVS_NO_FREE_PAGES );

    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
}

TEST_CASE("multiple partitions access check", "[nvs]")
{
    SpiFlashEmulator emu(10);
    TEST_ESP_OK( nvs_flash_init_custom("nvs1", 0, 5) );
    TEST_ESP_OK( nvs_flash_init_custom("nvs2", 5, 5) );
    nvs_handle_t handle1, handle2;
    TEST_ESP_OK( nvs_open_from_partition("nvs1", "test", NVS_READWRITE, &handle1) );
    TEST_ESP_OK( nvs_open_from_partition("nvs2", "test", NVS_READWRITE, &handle2) );
    TEST_ESP_OK( nvs_set_i32(handle1, "foo", 0xdeadbeef));
    TEST_ESP_OK( nvs_set_i32(handle2, "foo", 0xcafebabe));
    int32_t v1, v2;
    TEST_ESP_OK( nvs_get_i32(handle1, "foo", &v1));
    TEST_ESP_OK( nvs_get_i32(handle2, "foo", &v2));
    CHECK(v1 == 0xdeadbeef);
    CHECK(v2 == 0xcafebabe);

    TEST_ESP_OK(nvs_flash_deinit_partition("nvs1"));
    TEST_ESP_OK(nvs_flash_deinit_partition("nvs2"));
}

TEST_CASE("nvs page selection takes into account free entries also not just erased entries", "[nvs]")
{
    const size_t blob_size = Page::CHUNK_MAX_SIZE/2;
    uint8_t blob[blob_size] = {0};
    SpiFlashEmulator emu(3);
    TEST_ESP_OK( nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, 0, 3) );
    nvs_handle_t handle;
    TEST_ESP_OK( nvs_open("test", NVS_READWRITE, &handle) );
    // Fill first page
    TEST_ESP_OK( nvs_set_blob(handle, "1a", blob, blob_size/3) );
    TEST_ESP_OK( nvs_set_blob(handle, "1b", blob, blob_size) );
    // Fill second page
    TEST_ESP_OK( nvs_set_blob(handle, "2a", blob, blob_size) );
    TEST_ESP_OK( nvs_set_blob(handle, "2b", blob, blob_size) );

    // The item below should be able to fit the first page.
    TEST_ESP_OK( nvs_set_blob(handle, "3a", blob, 4) );
    TEST_ESP_OK( nvs_commit(handle) );
    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
}

TEST_CASE("calculate used and free space", "[nvs]")
{
    SpiFlashEmulator emu(6);
    nvs_flash_deinit();
    TEST_ESP_ERR(nvs_get_stats(NULL, NULL), ESP_ERR_INVALID_ARG);
    nvs_stats_t stat1;
    nvs_stats_t stat2;
    TEST_ESP_ERR(nvs_get_stats(NULL, &stat1), ESP_ERR_NVS_NOT_INITIALIZED);
    CHECK(stat1.free_entries == 0);
    CHECK(stat1.namespace_count == 0);
    CHECK(stat1.total_entries == 0);
    CHECK(stat1.used_entries == 0);

    nvs_handle_t handle = 0;
    size_t h_count_entries;
    TEST_ESP_ERR(nvs_get_used_entry_count(handle, &h_count_entries), ESP_ERR_NVS_INVALID_HANDLE);
    CHECK(h_count_entries == 0);

    // init nvs
    TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, 0, 6));

    TEST_ESP_ERR(nvs_get_used_entry_count(handle, &h_count_entries), ESP_ERR_NVS_INVALID_HANDLE);
    CHECK(h_count_entries == 0);

    Page p;
    // after erase. empty partition
    TEST_ESP_OK(nvs_get_stats(NULL, &stat1));
    CHECK(stat1.free_entries != 0);
    CHECK(stat1.namespace_count == 0);
    CHECK(stat1.total_entries == 6 * p.ENTRY_COUNT);
    CHECK(stat1.used_entries == 0);

    // create namespace test_k1
    nvs_handle_t handle_1;
    TEST_ESP_OK(nvs_open("test_k1", NVS_READWRITE, &handle_1));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat2));
    CHECK(stat2.free_entries + 1 == stat1.free_entries);
    CHECK(stat2.namespace_count == 1);
    CHECK(stat2.total_entries == stat1.total_entries);
    CHECK(stat2.used_entries == 1);

    // create pair key-value com
    TEST_ESP_OK(nvs_set_i32(handle_1, "com", 0x12345678));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat1));
    CHECK(stat1.free_entries + 1 == stat2.free_entries);
    CHECK(stat1.namespace_count == 1);
    CHECK(stat1.total_entries == stat2.total_entries);
    CHECK(stat1.used_entries == 2);

    // change value in com
    TEST_ESP_OK(nvs_set_i32(handle_1, "com", 0x01234567));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat2));
    CHECK(stat2.free_entries == stat1.free_entries);
    CHECK(stat2.namespace_count == 1);
    CHECK(stat2.total_entries != 0);
    CHECK(stat2.used_entries == 2);

    // create pair key-value ru
    TEST_ESP_OK(nvs_set_i32(handle_1, "ru", 0x00FF00FF));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat1));
    CHECK(stat1.free_entries + 1 == stat2.free_entries);
    CHECK(stat1.namespace_count == 1);
    CHECK(stat1.total_entries != 0);
    CHECK(stat1.used_entries == 3);

    // amount valid pair in namespace 1
    size_t h1_count_entries;
    TEST_ESP_OK(nvs_get_used_entry_count(handle_1, &h1_count_entries));
    CHECK(h1_count_entries == 2);

    nvs_handle_t handle_2;
    // create namespace test_k2
    TEST_ESP_OK(nvs_open("test_k2", NVS_READWRITE, &handle_2));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat2));
    CHECK(stat2.free_entries + 1 == stat1.free_entries);
    CHECK(stat2.namespace_count == 2);
    CHECK(stat2.total_entries == stat1.total_entries);
    CHECK(stat2.used_entries == 4);

    // create pair key-value
    TEST_ESP_OK(nvs_set_i32(handle_2, "su1", 0x00000001));
    TEST_ESP_OK(nvs_set_i32(handle_2, "su2", 0x00000002));
    TEST_ESP_OK(nvs_set_i32(handle_2, "sus", 0x00000003));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat1));
    CHECK(stat1.free_entries + 3 == stat2.free_entries);
    CHECK(stat1.namespace_count == 2);
    CHECK(stat1.total_entries == stat2.total_entries);
    CHECK(stat1.used_entries == 7);

    CHECK(stat1.total_entries == (stat1.used_entries + stat1.free_entries));

    // amount valid pair in namespace 2
    size_t h2_count_entries;
    TEST_ESP_OK(nvs_get_used_entry_count(handle_2, &h2_count_entries));
    CHECK(h2_count_entries == 3);

    CHECK(stat1.used_entries == (h1_count_entries + h2_count_entries + stat1.namespace_count));

    nvs_close(handle_1);
    nvs_close(handle_2);

    size_t temp = h2_count_entries;
    TEST_ESP_ERR(nvs_get_used_entry_count(handle_1, &h2_count_entries), ESP_ERR_NVS_INVALID_HANDLE);
    CHECK(h2_count_entries == 0);
    h2_count_entries = temp;
    TEST_ESP_ERR(nvs_get_used_entry_count(handle_1, NULL), ESP_ERR_INVALID_ARG);

    nvs_handle_t handle_3;
    // create namespace test_k3
    TEST_ESP_OK(nvs_open("test_k3", NVS_READWRITE, &handle_3));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat2));
    CHECK(stat2.free_entries + 1 == stat1.free_entries);
    CHECK(stat2.namespace_count == 3);
    CHECK(stat2.total_entries == stat1.total_entries);
    CHECK(stat2.used_entries == 8);

    // create pair blobs
    uint32_t blob[12];
    TEST_ESP_OK(nvs_set_blob(handle_3, "bl1", &blob, sizeof(blob)));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat1));
    CHECK(stat1.free_entries + 4 == stat2.free_entries);
    CHECK(stat1.namespace_count == 3);
    CHECK(stat1.total_entries == stat2.total_entries);
    CHECK(stat1.used_entries == 12);

    // amount valid pair in namespace 2
    size_t h3_count_entries;
    TEST_ESP_OK(nvs_get_used_entry_count(handle_3, &h3_count_entries));
    CHECK(h3_count_entries == 4);

    CHECK(stat1.used_entries == (h1_count_entries + h2_count_entries + h3_count_entries + stat1.namespace_count));

    nvs_close(handle_3);

    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
}

TEST_CASE("Recovery from power-off when the entry being erased is not on active page", "[nvs]")
{
    const size_t blob_size = Page::CHUNK_MAX_SIZE/2 ;
    size_t read_size = blob_size;
    uint8_t blob[blob_size] = {0x11};
    SpiFlashEmulator emu(3);
    TEST_ESP_OK( nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, 0, 3) );
    nvs_handle_t handle;
    TEST_ESP_OK( nvs_open("test", NVS_READWRITE, &handle) );

    emu.clearStats();
    emu.failAfter(Page::CHUNK_MAX_SIZE/4 + 75);
    TEST_ESP_OK( nvs_set_blob(handle, "1a", blob, blob_size) );
    TEST_ESP_OK( nvs_set_blob(handle, "1b", blob, blob_size) );

    TEST_ESP_ERR( nvs_erase_key(handle, "1a"), ESP_ERR_FLASH_OP_FAIL );

    TEST_ESP_OK( nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, 0, 3) );

    /* Check 1a is erased fully*/
    TEST_ESP_ERR( nvs_get_blob(handle, "1a", blob, &read_size), ESP_ERR_NVS_NOT_FOUND);

    /* Check 2b is still accessible*/
    TEST_ESP_OK( nvs_get_blob(handle, "1b", blob, &read_size));

    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
}

TEST_CASE("Recovery from power-off when page is being freed.", "[nvs]")
{
    const size_t blob_size = (Page::ENTRY_COUNT-3) * Page::ENTRY_SIZE;
    size_t read_size = blob_size/2;
    uint8_t blob[blob_size] = {0};
    SpiFlashEmulator emu(3);
    TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, 0, 3));
    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open("test", NVS_READWRITE, &handle));
    // Fill first page
    TEST_ESP_OK(nvs_set_blob(handle, "1a", blob, blob_size/3));
    TEST_ESP_OK(nvs_set_blob(handle, "1b", blob, blob_size/3));
    TEST_ESP_OK(nvs_set_blob(handle, "1c", blob, blob_size/4));
    // Fill second page
    TEST_ESP_OK(nvs_set_blob(handle, "2a", blob, blob_size/2));
    TEST_ESP_OK(nvs_set_blob(handle, "2b", blob, blob_size/2));

    TEST_ESP_OK(nvs_erase_key(handle, "1c"));

    emu.clearStats();
    emu.failAfter(6 * Page::ENTRY_COUNT);
    TEST_ESP_ERR(nvs_set_blob(handle, "1d", blob, blob_size/4), ESP_ERR_FLASH_OP_FAIL);

    TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, 0, 3));

    read_size = blob_size/3;
    TEST_ESP_OK( nvs_get_blob(handle, "1a", blob, &read_size));
    TEST_ESP_OK( nvs_get_blob(handle, "1b", blob, &read_size));

    read_size = blob_size /4;
    TEST_ESP_ERR( nvs_get_blob(handle, "1c", blob, &read_size), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_ERR( nvs_get_blob(handle, "1d", blob, &read_size), ESP_ERR_NVS_NOT_FOUND);

    read_size = blob_size /2;
    TEST_ESP_OK( nvs_get_blob(handle, "2a", blob, &read_size));
    TEST_ESP_OK( nvs_get_blob(handle, "2b", blob, &read_size));

    TEST_ESP_OK(nvs_commit(handle));
    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
}

TEST_CASE("Multi-page blobs are supported", "[nvs]")
{
    const size_t blob_size = Page::CHUNK_MAX_SIZE *2;
    uint8_t blob[blob_size] = {0};
    SpiFlashEmulator emu(5);
    TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, 0, 5));
    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open("test", NVS_READWRITE, &handle));
    TEST_ESP_OK(nvs_set_blob(handle, "abc", blob, blob_size));
    TEST_ESP_OK(nvs_commit(handle));
    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
}

TEST_CASE("Failures are handled while storing multi-page blobs", "[nvs]")
{
    const size_t blob_size = Page::CHUNK_MAX_SIZE *7;
    uint8_t blob[blob_size] = {0};
    SpiFlashEmulator emu(5);
    TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, 0, 5));
    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open("test", NVS_READWRITE, &handle));
    TEST_ESP_ERR(nvs_set_blob(handle, "abc", blob, blob_size), ESP_ERR_NVS_VALUE_TOO_LONG);
    TEST_ESP_OK(nvs_set_blob(handle, "abc", blob, Page::CHUNK_MAX_SIZE*2));
    TEST_ESP_OK(nvs_commit(handle));
    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
}

TEST_CASE("Reading multi-page blobs", "[nvs]")
{
    const size_t blob_size = Page::CHUNK_MAX_SIZE *3;
    uint8_t blob[blob_size];
    uint8_t blob_read[blob_size];
    size_t read_size = blob_size;
    SpiFlashEmulator emu(5);
    TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, 0, 5));
    nvs_handle_t handle;
    memset(blob, 0x11, blob_size);
    memset(blob_read, 0xee, blob_size);
    TEST_ESP_OK(nvs_open("readTest", NVS_READWRITE, &handle));
    TEST_ESP_OK(nvs_set_blob(handle, "abc", blob, blob_size));
    TEST_ESP_OK(nvs_get_blob(handle, "abc", blob_read, &read_size));
    CHECK(memcmp(blob, blob_read, blob_size) == 0);
    TEST_ESP_OK(nvs_commit(handle));
    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
}

TEST_CASE("Modification of values for Multi-page blobs are supported", "[nvs]")
{
    const size_t blob_size = Page::CHUNK_MAX_SIZE *2;
    uint8_t blob[blob_size] = {0};
    uint8_t blob_read[blob_size] = {0xfe};;
    uint8_t blob2[blob_size] = {0x11};
    uint8_t blob3[blob_size] = {0x22};
    uint8_t blob4[blob_size] ={ 0x33};
    size_t read_size = blob_size;
    SpiFlashEmulator emu(6);
    TEST_ESP_OK( nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, 0, 6) );
    nvs_handle_t handle;
    memset(blob, 0x11, blob_size);
    memset(blob2, 0x22, blob_size);
    memset(blob3, 0x33, blob_size);
    memset(blob4, 0x44, blob_size);
    memset(blob_read, 0xff, blob_size);
    TEST_ESP_OK( nvs_open("test", NVS_READWRITE, &handle) );
    TEST_ESP_OK( nvs_set_blob(handle, "abc", blob, blob_size) );
    TEST_ESP_OK( nvs_set_blob(handle, "abc", blob2, blob_size) );
    TEST_ESP_OK( nvs_set_blob(handle, "abc", blob3, blob_size) );
    TEST_ESP_OK( nvs_set_blob(handle, "abc", blob4, blob_size) );
    TEST_ESP_OK( nvs_get_blob(handle, "abc", blob_read, &read_size));
    CHECK(memcmp(blob4, blob_read, blob_size) == 0);
    TEST_ESP_OK( nvs_commit(handle) );
    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
}

TEST_CASE("Modification from single page blob to multi-page", "[nvs]")
{
    const size_t blob_size = Page::CHUNK_MAX_SIZE *3;
    uint8_t blob[blob_size] = {0};
    uint8_t blob_read[blob_size] = {0xff};
    size_t read_size = blob_size;
    SpiFlashEmulator emu(5);
    TEST_ESP_OK( nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, 0, 5) );
    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open("Test", NVS_READWRITE, &handle) );
    TEST_ESP_OK(nvs_set_blob(handle, "abc", blob, Page::CHUNK_MAX_SIZE/2));
    TEST_ESP_OK(nvs_set_blob(handle, "abc", blob, blob_size));
    TEST_ESP_OK(nvs_get_blob(handle, "abc", blob_read, &read_size));
    CHECK(memcmp(blob, blob_read, blob_size) == 0);
    TEST_ESP_OK(nvs_commit(handle) );
    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
}

TEST_CASE("Modification from  multi-page to single page", "[nvs]")
{
    const size_t blob_size = Page::CHUNK_MAX_SIZE *3;
    uint8_t blob[blob_size] = {0};
    uint8_t blob_read[blob_size] = {0xff};
    size_t read_size = blob_size;
    SpiFlashEmulator emu(5);
    TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, 0, 5) );
    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open("Test", NVS_READWRITE, &handle) );
    TEST_ESP_OK(nvs_set_blob(handle, "abc", blob, blob_size));
    TEST_ESP_OK(nvs_set_blob(handle, "abc", blob, Page::CHUNK_MAX_SIZE/2));
    TEST_ESP_OK(nvs_set_blob(handle, "abc2", blob, blob_size));
    TEST_ESP_OK(nvs_get_blob(handle, "abc", blob_read, &read_size));
    CHECK(memcmp(blob, blob_read, Page::CHUNK_MAX_SIZE) == 0);
    TEST_ESP_OK(nvs_commit(handle) );
    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
}

TEST_CASE("Multi-page blob erased using nvs_erase_key should not be found when probed for just length", "[nvs]")
{
    const size_t blob_size = Page::CHUNK_MAX_SIZE *3;
    uint8_t blob[blob_size] = {0};
    size_t read_size = blob_size;
    SpiFlashEmulator emu(5);
    TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, 0, 5));
    nvs_handle handle;
    TEST_ESP_OK(nvs_open("Test", NVS_READWRITE, &handle));
    TEST_ESP_OK(nvs_set_blob(handle, "abc", blob, blob_size));
    TEST_ESP_OK(nvs_erase_key(handle, "abc"));
    TEST_ESP_ERR(nvs_get_blob(handle, "abc", NULL, &read_size), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_OK(nvs_commit(handle));
    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
}


TEST_CASE("Check that orphaned blobs are erased during init", "[nvs]")
{
    const size_t blob_size = Page::CHUNK_MAX_SIZE *3 ;
    uint8_t blob[blob_size] = {0x11};
    uint8_t blob2[blob_size] = {0x22};
    uint8_t blob3[blob_size] = {0x33};
    SpiFlashEmulator emu(5);
    Storage storage;

    TEST_ESP_OK(storage.init(0, 5));

    TEST_ESP_OK(storage.writeItem(1, ItemType::BLOB, "key", blob, sizeof(blob)));


    TEST_ESP_OK(storage.init(0, 5));
    /* Check that multi-page item is still available.**/
    TEST_ESP_OK(storage.readItem(1, ItemType::BLOB, "key", blob, sizeof(blob)));

    TEST_ESP_ERR(storage.writeItem(1, ItemType::BLOB, "key2", blob, sizeof(blob)), ESP_ERR_NVS_NOT_ENOUGH_SPACE);

    Page p;
    p.load(3); // This is where index will be placed.
    p.erase();

    TEST_ESP_OK(storage.init(0, 5));

    TEST_ESP_ERR(storage.readItem(1, ItemType::BLOB, "key", blob, sizeof(blob)), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_OK(storage.writeItem(1, ItemType::BLOB, "key3", blob, sizeof(blob)));
}

TEST_CASE("nvs blob fragmentation test", "[nvs]")
{
    SpiFlashEmulator emu(4);
    TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, 0, 4) );
    const size_t BLOB_SIZE = 3500;
    uint8_t *blob = (uint8_t*) malloc(BLOB_SIZE);
    CHECK(blob != NULL);
    memset(blob, 0xEE, BLOB_SIZE);
    const uint32_t magic = 0xff33eaeb;
    nvs_handle_t h;
    TEST_ESP_OK( nvs_open("blob_tests", NVS_READWRITE, &h) );
    for (int i = 0; i < 128; i++) {
        INFO("Iteration " << i << "...\n");
        TEST_ESP_OK( nvs_set_u32(h, "magic", magic) );
        TEST_ESP_OK( nvs_set_blob(h, "blob", blob, BLOB_SIZE) );
        char seq_buf[16];
        sprintf(seq_buf, "seq%d", i);
        TEST_ESP_OK( nvs_set_u32(h, seq_buf, i) );
    }
    free(blob);

    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
}

TEST_CASE("nvs code handles errors properly when partition is near to full", "[nvs]")
{
    const size_t blob_size = Page::CHUNK_MAX_SIZE * 0.3 ;
    uint8_t blob[blob_size] = {0x11};
    SpiFlashEmulator emu(5);
    Storage storage;
    char nvs_key[16] = "";

    TEST_ESP_OK(storage.init(0, 5));

    /* Four pages should fit roughly 12 blobs*/
    for(uint8_t count = 1; count <= 12; count++) {
        sprintf(nvs_key, "key:%u", count);
        TEST_ESP_OK(storage.writeItem(1, ItemType::BLOB, nvs_key, blob, sizeof(blob)));
    }

    for(uint8_t count = 13; count <= 20; count++) {
        sprintf(nvs_key, "key:%u", count);
        TEST_ESP_ERR(storage.writeItem(1, ItemType::BLOB, nvs_key, blob, sizeof(blob)), ESP_ERR_NVS_NOT_ENOUGH_SPACE);
    }
}

TEST_CASE("Check for nvs version incompatibility", "[nvs]")
{
    SpiFlashEmulator emu(3);

    int32_t val1 = 0x12345678;
    Page p;
    p.load(0);
    TEST_ESP_OK(p.setVersion(Page::NVS_VERSION - 1));
    TEST_ESP_OK(p.writeItem(1, ItemType::I32, "foo", &val1, sizeof(val1)));

    TEST_ESP_ERR(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, 0, 3), ESP_ERR_NVS_NEW_VERSION_FOUND);

    nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME);
}

TEST_CASE("Check that NVS supports old blob format without blob index", "[nvs]")
{
    SpiFlashEmulator emu("../nvs_partition_generator/part_old_blob_format.bin");
    nvs_handle_t handle;

    TEST_ESP_OK( nvs_flash_init_custom("test", 0, 2) );
    TEST_ESP_OK( nvs_open_from_partition("test", "dummyNamespace", NVS_READWRITE, &handle));

    char buf[64] = {0};
    size_t buflen = 64;
    uint8_t hexdata[] = {0x01, 0x02, 0x03, 0xab, 0xcd, 0xef};
    TEST_ESP_OK( nvs_get_blob(handle, "dummyHex2BinKey", buf, &buflen));
    CHECK(memcmp(buf, hexdata, buflen) == 0);

    buflen = 64;
    uint8_t base64data[] = {'1', '2', '3', 'a', 'b', 'c'};
    TEST_ESP_OK( nvs_get_blob(handle, "dummyBase64Key", buf, &buflen));
    CHECK(memcmp(buf, base64data, buflen) == 0);

    Page p;
    p.load(0);

    /* Check that item is stored in old format without blob index*/
    TEST_ESP_OK(p.findItem(1, ItemType::BLOB, "dummyHex2BinKey"));

    /* Modify the blob so that it is stored in the new format*/
    hexdata[0] = hexdata[1] = hexdata[2] = 0x99;
    TEST_ESP_OK(nvs_set_blob(handle, "dummyHex2BinKey", hexdata, sizeof(hexdata)));

    Page p2;
    p2.load(0);

    /* Check the type of the blob. Expect type mismatch since the blob is stored in new format*/
    TEST_ESP_ERR(p2.findItem(1, ItemType::BLOB, "dummyHex2BinKey"), ESP_ERR_NVS_TYPE_MISMATCH);

    /* Check that index is present for the modified blob according to new format*/
    TEST_ESP_OK(p2.findItem(1, ItemType::BLOB_IDX, "dummyHex2BinKey"));

    /* Read the blob in new format and check the contents*/
    buflen = 64;
    TEST_ESP_OK( nvs_get_blob(handle, "dummyBase64Key", buf, &buflen));
    CHECK(memcmp(buf, base64data, buflen) == 0);

    TEST_ESP_OK(nvs_flash_deinit_partition("test"));
}

TEST_CASE("monkey test with old-format blob present", "[nvs][monkey]")
{
    std::random_device rd;
    std::mt19937 gen(rd());
    uint32_t seed = 3;
    gen.seed(seed);

    SpiFlashEmulator emu(10);
    emu.randomize(seed);
    emu.clearStats();

    const uint32_t NVS_FLASH_SECTOR = 2;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 8;
    static const size_t smallBlobLen = Page::CHUNK_MAX_SIZE / 3;

    emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);

    TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN));

    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open("namespace1", NVS_READWRITE, &handle));
    RandomTest test;

    for ( uint8_t it = 0; it < 10; it++) {
        size_t count = 200;

        /* Erase index and chunks for the blob with "singlepage" key */
        for (uint8_t num = NVS_FLASH_SECTOR; num < NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN; num++) {
            Page p;
            p.load(num);
            p.eraseItem(1, ItemType::BLOB, "singlepage", Item::CHUNK_ANY, VerOffset::VER_ANY);
            p.eraseItem(1, ItemType::BLOB_IDX, "singlepage", Item::CHUNK_ANY, VerOffset::VER_ANY);
            p.eraseItem(1, ItemType::BLOB_DATA, "singlepage", Item::CHUNK_ANY, VerOffset::VER_ANY);
        }

        /* Now write "singlepage" blob in old format*/
        for (uint8_t num = NVS_FLASH_SECTOR; num < NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN; num++) {
            Page p;
            p.load(num);
            if (p.state() == Page::PageState::ACTIVE) {
                uint8_t buf[smallBlobLen];
                size_t blobLen = gen() % smallBlobLen;

                if(blobLen > p.getVarDataTailroom()) {
                    blobLen = p.getVarDataTailroom();
                }

                std::generate_n(buf, blobLen, [&]() -> uint8_t {
                        return static_cast<uint8_t>(gen() % 256);
                        });

                TEST_ESP_OK(p.writeItem(1, ItemType::BLOB, "singlepage", buf, blobLen, Item::CHUNK_ANY));
                TEST_ESP_OK(p.findItem(1, ItemType::BLOB, "singlepage"));
                test.handleExternalWriteAtIndex(9, buf, blobLen); // This assumes "singlepage" is always at index 9

                break;
            }
        }
        /* Initialize again */
        TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN));
        TEST_ESP_OK(nvs_open("namespace1", NVS_READWRITE, &handle));

        /* Perform random things */
        auto res = test.doRandomThings(handle, gen, count);
        if (res != ESP_OK) {
            nvs_dump(NVS_DEFAULT_PART_NAME);
            CHECK(0);
        }

        /* Check that only one version is present for "singlepage". Its possible that last iteration did not write
         * anything for "singlepage". So either old version or new version should be present.*/
        bool oldVerPresent = false, newVerPresent = false;

        for (uint8_t num = NVS_FLASH_SECTOR; num < NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN; num++) {
            Page p;
            p.load(num);
            if(!oldVerPresent && p.findItem(1, ItemType::BLOB, "singlepage", Item::CHUNK_ANY, VerOffset::VER_ANY) == ESP_OK) {
                oldVerPresent = true;
            }

            if(!newVerPresent && p.findItem(1, ItemType::BLOB_IDX, "singlepage", Item::CHUNK_ANY, VerOffset::VER_ANY) == ESP_OK) {
                newVerPresent = true;
            }
        }
        CHECK(oldVerPresent != newVerPresent);
    }

    s_perf << "Monkey test: nErase=" << emu.getEraseOps() << " nWrite=" << emu.getWriteOps() << std::endl;

    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
}

TEST_CASE("Recovery from power-off during modification of blob present in old-format (same page)", "[nvs]")
{
    std::random_device rd;
    std::mt19937 gen(rd());
    uint32_t seed = 3;
    gen.seed(seed);

    SpiFlashEmulator emu(3);
    emu.clearStats();

    TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, 0, 3));

    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open("namespace1", NVS_READWRITE, &handle));

    uint8_t hexdata[] = {0x01, 0x02, 0x03, 0xab, 0xcd, 0xef};
    uint8_t hexdata_old[] = {0x11, 0x12, 0x13, 0xbb, 0xcc, 0xee};
    size_t buflen = sizeof(hexdata);
    uint8_t buf[Page::CHUNK_MAX_SIZE];

    /* Power-off when blob was being written on the same page where its old version in old format
     * was present*/
    Page p;
    p.load(0);
    /* Write blob in old-format*/
    TEST_ESP_OK(p.writeItem(1, ItemType::BLOB, "singlepage", hexdata_old, sizeof(hexdata_old)));

    /* Write blob in new format*/
    TEST_ESP_OK(p.writeItem(1, ItemType::BLOB_DATA, "singlepage", hexdata, sizeof(hexdata), 0));
    /* All pages are stored. Now store the index.*/
    Item item;
    item.blobIndex.dataSize = sizeof(hexdata);
    item.blobIndex.chunkCount = 1;
    item.blobIndex.chunkStart = VerOffset::VER_0_OFFSET;

    TEST_ESP_OK(p.writeItem(1, ItemType::BLOB_IDX, "singlepage", item.data, sizeof(item.data)));

    TEST_ESP_OK(p.findItem(1, ItemType::BLOB, "singlepage"));

    /* Initialize again */
    TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, 0, 3));
    TEST_ESP_OK(nvs_open("namespace1", NVS_READWRITE, &handle));

    TEST_ESP_OK( nvs_get_blob(handle, "singlepage", buf, &buflen));
    CHECK(memcmp(buf, hexdata, buflen) == 0);

    Page p2;
    p2.load(0);
    TEST_ESP_ERR(p2.findItem(1, ItemType::BLOB, "singlepage"), ESP_ERR_NVS_TYPE_MISMATCH);

    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
}

TEST_CASE("Recovery from power-off during modification of blob present in old-format (different page)", "[nvs]")
{
    std::random_device rd;
    std::mt19937 gen(rd());
    uint32_t seed = 3;
    gen.seed(seed);

    SpiFlashEmulator emu(3);
    emu.clearStats();

    TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, 0, 3));

    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open("namespace1", NVS_READWRITE, &handle));

    uint8_t hexdata[] = {0x01, 0x02, 0x03, 0xab, 0xcd, 0xef};
    uint8_t hexdata_old[] = {0x11, 0x12, 0x13, 0xbb, 0xcc, 0xee};
    size_t buflen = sizeof(hexdata);
    uint8_t buf[Page::CHUNK_MAX_SIZE];


    /* Power-off when blob was being written on the different page where its old version in old format
     * was present*/
    Page p;
    p.load(0);
    /* Write blob in old-format*/
    TEST_ESP_OK(p.writeItem(1, ItemType::BLOB, "singlepage", hexdata_old, sizeof(hexdata_old)));

    /* Write blob in new format*/
    TEST_ESP_OK(p.writeItem(1, ItemType::BLOB_DATA, "singlepage", hexdata, sizeof(hexdata), 0));
    /* All pages are stored. Now store the index.*/
    Item item;
    item.blobIndex.dataSize = sizeof(hexdata);
    item.blobIndex.chunkCount = 1;
    item.blobIndex.chunkStart = VerOffset::VER_0_OFFSET;
    p.markFull();
    Page p2;
    p2.load(1);
    p2.setSeqNumber(1);

    TEST_ESP_OK(p2.writeItem(1, ItemType::BLOB_IDX, "singlepage", item.data, sizeof(item.data)));

    TEST_ESP_OK(p.findItem(1, ItemType::BLOB, "singlepage"));

    /* Initialize again */
    TEST_ESP_OK(nvs_flash_init_custom(NVS_DEFAULT_PART_NAME, 0, 3));
    TEST_ESP_OK(nvs_open("namespace1", NVS_READWRITE, &handle));

    TEST_ESP_OK( nvs_get_blob(handle, "singlepage", buf, &buflen));
    CHECK(memcmp(buf, hexdata, buflen) == 0);

    Page p3;
    p3.load(0);
    TEST_ESP_ERR(p3.findItem(1, ItemType::BLOB, "singlepage"), ESP_ERR_NVS_NOT_FOUND);

    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
}

static void check_nvs_part_gen_args(char const *part_name, int size, char const *filename, bool is_encr, nvs_sec_cfg_t* xts_cfg)
{
    nvs_handle_t handle;

    if (is_encr)
        TEST_ESP_OK(nvs_flash_secure_init_custom(part_name, 0, size, xts_cfg));
    else
        TEST_ESP_OK( nvs_flash_init_custom(part_name, 0, size) );

    TEST_ESP_OK( nvs_open_from_partition(part_name, "dummyNamespace", NVS_READONLY, &handle));
    uint8_t u8v;
    TEST_ESP_OK( nvs_get_u8(handle, "dummyU8Key", &u8v));
    CHECK(u8v == 127);
    int8_t i8v;
    TEST_ESP_OK( nvs_get_i8(handle, "dummyI8Key", &i8v));
    CHECK(i8v == -128);
    uint16_t u16v;
    TEST_ESP_OK( nvs_get_u16(handle, "dummyU16Key", &u16v));
    CHECK(u16v == 32768);
    uint32_t u32v;
    TEST_ESP_OK( nvs_get_u32(handle, "dummyU32Key", &u32v));
    CHECK(u32v == 4294967295);
    int32_t i32v;
    TEST_ESP_OK( nvs_get_i32(handle, "dummyI32Key", &i32v));
    CHECK(i32v == -2147483648);

    char buf[64] = {0};
    size_t buflen = 64;
    TEST_ESP_OK( nvs_get_str(handle, "dummyStringKey", buf, &buflen));
    CHECK(strncmp(buf, "0A:0B:0C:0D:0E:0F", buflen) == 0);

    uint8_t hexdata[] = {0x01, 0x02, 0x03, 0xab, 0xcd, 0xef};
    buflen = 64;
    int j;
    TEST_ESP_OK( nvs_get_blob(handle, "dummyHex2BinKey", buf, &buflen));
    CHECK(memcmp(buf, hexdata, buflen) == 0);

    uint8_t base64data[] = {'1', '2', '3', 'a', 'b', 'c'};
    TEST_ESP_OK( nvs_get_blob(handle, "dummyBase64Key", buf, &buflen));
    CHECK(memcmp(buf, base64data, buflen) == 0);

    buflen = 64;
    uint8_t hexfiledata[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};
    TEST_ESP_OK( nvs_get_blob(handle, "hexFileKey", buf, &buflen));
    CHECK(memcmp(buf, hexfiledata, buflen) == 0);

    buflen = 64;
    uint8_t strfiledata[64] = "abcdefghijklmnopqrstuvwxyz\0";
    TEST_ESP_OK( nvs_get_str(handle, "stringFileKey", buf, &buflen));
    CHECK(memcmp(buf, strfiledata, buflen) == 0);

    char bin_data[5200];
    size_t bin_len = sizeof(bin_data);
    char binfiledata[5200];
    ifstream file;
    file.open(filename);
    file.read(binfiledata,5200);
    TEST_ESP_OK( nvs_get_blob(handle, "binFileKey", bin_data, &bin_len));
    CHECK(memcmp(bin_data, binfiledata, bin_len) == 0);

    file.close();

    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(part_name));
}


TEST_CASE("check and read data from partition generated via partition generation utility with multipage blob support disabled", "[nvs_part_gen]")
{
    int status;
    int childpid = fork();
    if (childpid == 0) {
        exit(execlp("cp", " cp",
                    "-rf",
                    "../nvs_partition_generator/testdata",
                    ".",NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) != -1);

        childpid = fork();

        if (childpid == 0) {
            exit(execlp("python", "python",
                    "../nvs_partition_generator/nvs_partition_gen.py",
                    "generate",
                    "../nvs_partition_generator/sample_singlepage_blob.csv",
                    "partition_single_page.bin",
                    "0x3000",
                    "--version",
                    "1",
                    "--outdir",
                    "../nvs_partition_generator",NULL));
        } else {
            CHECK(childpid > 0);
            int status;
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) != -1);
        }
    }

    SpiFlashEmulator emu("../nvs_partition_generator/partition_single_page.bin");

    check_nvs_part_gen_args("test", 3, "../nvs_partition_generator/testdata/sample_singlepage_blob.bin", false, NULL);

    childpid = fork();
    if (childpid == 0) {
        exit(execlp("rm", " rm",
                    "-rf",
                    "testdata",NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) != -1);

    }
}

TEST_CASE("check and read data from partition generated via partition generation utility with multipage blob support enabled", "[nvs_part_gen]")
{
    int status;
    int childpid = fork();
    if (childpid == 0) {
        exit(execlp("cp", " cp",
                    "-rf",
                    "../nvs_partition_generator/testdata",
                    ".",NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) != -1);

        childpid = fork();

        if (childpid == 0) {
            exit(execlp("python", "python",
                    "../nvs_partition_generator/nvs_partition_gen.py",
                    "generate",
                    "../nvs_partition_generator/sample_multipage_blob.csv",
                    "partition_multipage_blob.bin",
                    "0x4000",
                    "--version",
                    "2",
                    "--outdir",
                    "../nvs_partition_generator",NULL));
        } else {
            CHECK(childpid > 0);
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) != -1);
        }
    }

    SpiFlashEmulator emu("../nvs_partition_generator/partition_multipage_blob.bin");

    check_nvs_part_gen_args("test", 4, "../nvs_partition_generator/testdata/sample_multipage_blob.bin",false,NULL);

    childpid = fork();
    if (childpid == 0) {
        exit(execlp("rm", " rm",
                    "-rf",
                    "testdata",NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) != -1);

    }
}

TEST_CASE("check and read data from partition generated via manufacturing utility with multipage blob support disabled", "[mfg_gen]")
{
    int childpid = fork();
    int status;

    if (childpid == 0) {
        exit(execlp("bash", "bash",
                    "-c",
                    "rm -rf ../../../tools/mass_mfg/host_test && \
                    cp -rf ../../../tools/mass_mfg/testdata mfg_testdata && \
                    cp -rf ../nvs_partition_generator/testdata . && \
                    mkdir -p ../../../tools/mass_mfg/host_test", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) != -1);

        childpid = fork();
        if (childpid == 0) {
            exit(execlp("python", "python",
                        "../../../tools/mass_mfg/mfg_gen.py",
                        "generate",
                        "../../../tools/mass_mfg/samples/sample_config.csv",
                        "../../../tools/mass_mfg/samples/sample_values_singlepage_blob.csv",
                        "Test",
                        "0x3000",
                        "--outdir",
                        "../../../tools/mass_mfg/host_test",
                        "--version",
                        "1",NULL));

        } else {
            CHECK(childpid > 0);
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) != -1);

            childpid = fork();
            if (childpid == 0) {
                exit(execlp("python", "python",
                            "../nvs_partition_generator/nvs_partition_gen.py",
                            "generate",
                            "../../../tools/mass_mfg/host_test/csv/Test-1.csv",
                            "../nvs_partition_generator/Test-1-partition.bin",
                            "0x3000",
                            "--version",
                            "1",NULL));

            } else {
                CHECK(childpid > 0);
                waitpid(childpid, &status, 0);
                CHECK(WEXITSTATUS(status) != -1);

            }

        }

    }

    SpiFlashEmulator emu1("../../../tools/mass_mfg/host_test/bin/Test-1.bin");
    check_nvs_part_gen_args("test", 3, "mfg_testdata/sample_singlepage_blob.bin", false, NULL);

    SpiFlashEmulator emu2("../nvs_partition_generator/Test-1-partition.bin");
    check_nvs_part_gen_args("test", 3, "testdata/sample_singlepage_blob.bin", false, NULL);


    childpid = fork();
    if (childpid == 0) {
        exit(execlp("bash", " bash",
                    "-c",
                    "rm -rf ../../../tools/mass_mfg/host_test | \
                    rm -rf mfg_testdata | \
                    rm -rf testdata",NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) != -1);

    }

}

TEST_CASE("check and read data from partition generated via manufacturing utility with multipage blob support enabled", "[mfg_gen]")
{
    int childpid = fork();
    int status;

    if (childpid == 0) {
        exit(execlp("bash", " bash",
                    "-c",
                    "rm -rf ../../../tools/mass_mfg/host_test | \
                    cp -rf ../../../tools/mass_mfg/testdata mfg_testdata | \
                    cp -rf ../nvs_partition_generator/testdata . | \
                    mkdir -p ../../../tools/mass_mfg/host_test",NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) != -1);

        childpid = fork();
        if (childpid == 0) {
            exit(execlp("python", "python",
                        "../../../tools/mass_mfg/mfg_gen.py",
                        "generate",
                        "../../../tools/mass_mfg/samples/sample_config.csv",
                        "../../../tools/mass_mfg/samples/sample_values_multipage_blob.csv",
                        "Test",
                        "0x4000",
                        "--outdir",
                        "../../../tools/mass_mfg/host_test",
                        "--version",
                        "2",NULL));

        } else {
            CHECK(childpid > 0);
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) != -1);

            childpid = fork();
            if (childpid == 0) {
                exit(execlp("python", "python",
                            "../nvs_partition_generator/nvs_partition_gen.py",
                            "generate",
                            "../../../tools/mass_mfg/host_test/csv/Test-1.csv",
                            "../nvs_partition_generator/Test-1-partition.bin",
                            "0x4000",
                            "--version",
                            "2",NULL));

            } else {
                CHECK(childpid > 0);
                waitpid(childpid, &status, 0);
                CHECK(WEXITSTATUS(status) != -1);

            }

        }

    }

    SpiFlashEmulator emu1("../../../tools/mass_mfg/host_test/bin/Test-1.bin");
    check_nvs_part_gen_args("test", 4, "mfg_testdata/sample_multipage_blob.bin", false, NULL);

    SpiFlashEmulator emu2("../nvs_partition_generator/Test-1-partition.bin");
    check_nvs_part_gen_args("test", 4, "testdata/sample_multipage_blob.bin", false, NULL);

    childpid = fork();
    if (childpid == 0) {
        exit(execlp("bash", " bash",
                    "-c",
                    "rm -rf ../../../tools/mass_mfg/host_test | \
                    rm -rf mfg_testdata | \
                    rm -rf testdata",NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) != -1);

    }

}

#if CONFIG_NVS_ENCRYPTION
TEST_CASE("check underlying xts code for 32-byte size sector encryption", "[nvs]")
{
    auto toHex = [](char ch) {
        if(ch >= '0' && ch <= '9')
            return ch - '0';
        else if(ch >= 'a' && ch <= 'f')
            return ch - 'a' + 10;
        else if(ch >= 'A' && ch <= 'F')
            return ch - 'A' + 10;
        else
            return 0;
    };

    auto toHexByte = [toHex](char* c) {
        return 16 * toHex(c[0]) + toHex(c[1]);
    };

    auto toHexStream = [toHexByte](char* src, uint8_t* dest) {
        uint32_t cnt =0;
        char* p = src;
        while(*p != '\0' && *(p + 1) != '\0')
        {
            dest[cnt++] = toHexByte(p); p += 2;
        }
    };

    uint8_t eky_hex[2 * NVS_KEY_SIZE];
    uint8_t ptxt_hex[Page::ENTRY_SIZE], ctxt_hex[Page::ENTRY_SIZE], ba_hex[16];
    mbedtls_aes_xts_context ectx[1];
    mbedtls_aes_xts_context dctx[1];

    char eky[][2 * NVS_KEY_SIZE + 1] = {
        "0000000000000000000000000000000000000000000000000000000000000000",
        "1111111111111111111111111111111111111111111111111111111111111111"
    };
    char tky[][2 * NVS_KEY_SIZE + 1] = {
        "0000000000000000000000000000000000000000000000000000000000000000",
        "2222222222222222222222222222222222222222222222222222222222222222"
    };
    char blk_addr[][2*16 + 1]  = {
        "00000000000000000000000000000000",
        "33333333330000000000000000000000"
    };

    char ptxt[][2 * Page::ENTRY_SIZE + 1] = {
        "0000000000000000000000000000000000000000000000000000000000000000",
        "4444444444444444444444444444444444444444444444444444444444444444"
    };
    char ctxt[][2 * Page::ENTRY_SIZE + 1] = {
        "d456b4fc2e620bba6ffbed27b956c9543454dd49ebd8d8ee6f94b65cbe158f73",
        "e622334f184bbce129a25b2ac76b3d92abf98e22df5bdd15af471f3db8946a85"
    };

    mbedtls_aes_xts_init(ectx);
    mbedtls_aes_xts_init(dctx);

    for(uint8_t cnt = 0; cnt < sizeof(eky)/sizeof(eky[0]); cnt++) {
        toHexStream(eky[cnt], eky_hex);
        toHexStream(tky[cnt], &eky_hex[NVS_KEY_SIZE]);
        toHexStream(ptxt[cnt], ptxt_hex);
        toHexStream(ctxt[cnt], ctxt_hex);
        toHexStream(blk_addr[cnt], ba_hex);

        CHECK(!mbedtls_aes_xts_setkey_enc(ectx, eky_hex, 2 * NVS_KEY_SIZE * 8));
        CHECK(!mbedtls_aes_xts_setkey_enc(dctx, eky_hex, 2 * NVS_KEY_SIZE * 8));

        CHECK(!mbedtls_aes_crypt_xts(ectx, MBEDTLS_AES_ENCRYPT, Page::ENTRY_SIZE, ba_hex, ptxt_hex, ptxt_hex));

        CHECK(!memcmp(ptxt_hex, ctxt_hex, Page::ENTRY_SIZE));
    }
}

TEST_CASE("test nvs apis with encryption enabled", "[nvs]")
{
    SpiFlashEmulator emu(10);
    emu.randomize(100);

    nvs_handle_t handle_1;
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);

    nvs_sec_cfg_t xts_cfg;
    for(int count = 0; count < NVS_KEY_SIZE; count++) {
        xts_cfg.eky[count] = 0x11;
        xts_cfg.tky[count] = 0x22;
    }

    for (uint16_t i = NVS_FLASH_SECTOR; i <NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN; ++i) {
        spi_flash_erase_sector(i);
    }
    TEST_ESP_OK(nvs_flash_secure_init_custom(NVS_DEFAULT_PART_NAME, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN, &xts_cfg));

    TEST_ESP_ERR(nvs_open("namespace1", NVS_READONLY, &handle_1), ESP_ERR_NVS_NOT_FOUND);


    TEST_ESP_OK(nvs_open("namespace1", NVS_READWRITE, &handle_1));
    TEST_ESP_OK(nvs_set_i32(handle_1, "foo", 0x12345678));
    TEST_ESP_OK(nvs_set_i32(handle_1, "foo", 0x23456789));

    nvs_handle_t handle_2;
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

    size_t buf_len_long = buf_len + 1;
    TEST_ESP_OK(nvs_get_str(handle_2, "key", buf, &buf_len_long));
    CHECK(buf_len_long == buf_len);

    TEST_ESP_OK(nvs_get_str(handle_2, "key", buf, &buf_len));

    CHECK(0 == strcmp(buf, str));
    nvs_close(handle_1);
    nvs_close(handle_2);

    TEST_ESP_OK(nvs_flash_deinit());
}

TEST_CASE("test nvs apis for nvs partition generator utility with encryption enabled", "[nvs_part_gen]")
{
    int status;
    int childpid = fork();
    if (childpid == 0) {
        exit(execlp("cp", " cp",
                    "-rf",
                    "../nvs_partition_generator/testdata",
                    ".",NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) != -1);

        childpid = fork();

        if (childpid == 0) {
            exit(execlp("python", "python",
                    "../nvs_partition_generator/nvs_partition_gen.py",
                    "encrypt",
                    "../nvs_partition_generator/sample_multipage_blob.csv",
                    "partition_encrypted.bin",
                    "0x4000",
                    "--inputkey",
                    "../nvs_partition_generator/testdata/sample_encryption_keys.bin",
                    "--outdir",
                    "../nvs_partition_generator",NULL));
        } else {
            CHECK(childpid > 0);
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) != -1);
        }
    }

    SpiFlashEmulator emu("../nvs_partition_generator/partition_encrypted.bin");

    nvs_sec_cfg_t cfg;
    for(int count = 0; count < NVS_KEY_SIZE; count++) {
        cfg.eky[count] = 0x11;
        cfg.tky[count] = 0x22;
    }

    check_nvs_part_gen_args(NVS_DEFAULT_PART_NAME, 4, "../nvs_partition_generator/testdata/sample_multipage_blob.bin", true, &cfg);

    childpid = fork();
    if (childpid == 0) {
        exit(execlp("rm", " rm",
                    "-rf",
                    "testdata",NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) != -1);

    }

}

TEST_CASE("test decrypt functionality for encrypted data", "[nvs_part_gen]")
{

    //retrieving the temporary test data
    int status = system("cp -rf ../nvs_partition_generator/testdata .");
    CHECK(status == 0);

    //encoding data from sample_multipage_blob.csv
    status = system("python ../nvs_partition_generator/nvs_partition_gen.py generate ../nvs_partition_generator/sample_multipage_blob.csv partition_encoded.bin 0x5000 --outdir ../nvs_partition_generator");
    CHECK(status == 0);

    //encrypting data from sample_multipage_blob.csv
    status = system("python ../nvs_partition_generator/nvs_partition_gen.py encrypt ../nvs_partition_generator/sample_multipage_blob.csv partition_encrypted.bin 0x5000 --inputkey ../nvs_partition_generator/testdata/sample_encryption_keys.bin --outdir ../nvs_partition_generator");
    CHECK(status == 0);

    //decrypting data from partition_encrypted.bin
    status = system("python ../nvs_partition_generator/nvs_partition_gen.py decrypt ../nvs_partition_generator/partition_encrypted.bin ../nvs_partition_generator/testdata/sample_encryption_keys.bin ../nvs_partition_generator/partition_decrypted.bin");
    CHECK(status == 0);

    status = system("diff ../nvs_partition_generator/partition_decrypted.bin ../nvs_partition_generator/partition_encoded.bin");
    CHECK(status == 0);
    CHECK(WEXITSTATUS(status) == 0);


    //cleaning up the temporary test data
    status = system("rm -rf testdata");
    CHECK(status == 0);

}

TEST_CASE("test nvs apis for nvs partition generator utility with encryption enabled using keygen", "[nvs_part_gen]")
{
    int childpid = fork();
    int status;

    if (childpid == 0) {
        exit(execlp("cp", " cp",
                    "-rf",
                    "../nvs_partition_generator/testdata",
                    ".",NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) != -1);

        childpid = fork();

        if (childpid == 0) {
            exit(execlp("rm", " rm",
                        "-rf",
                        "../nvs_partition_generator/keys",NULL));
        } else {
            CHECK(childpid > 0);
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) != -1);

            childpid = fork();
            if (childpid == 0) {
                exit(execlp("python", "python",
                            "../nvs_partition_generator/nvs_partition_gen.py",
                            "encrypt",
                            "../nvs_partition_generator/sample_multipage_blob.csv",
                            "partition_encrypted_using_keygen.bin",
                            "0x4000",
                            "--keygen",
                            "--outdir",
                            "../nvs_partition_generator",NULL));

            } else {
                CHECK(childpid > 0);
                waitpid(childpid, &status, 0);
                CHECK(WEXITSTATUS(status) != -1);

            }
        }
    }


    DIR *dir;
    struct dirent *file;
    char *filename;
    char *files;
    char *file_ext;

    dir = opendir("../nvs_partition_generator/keys");
    while ((file = readdir(dir)) != NULL)
    {
        filename = file->d_name;
        files = strrchr(filename, '.');
        if (files != NULL)
        {
            file_ext = files+1;
            if (strncmp(file_ext,"bin",3) == 0)
            {
                break;
            }
        }
    }

    std::string encr_file = std::string("../nvs_partition_generator/keys/") + std::string(filename);
    SpiFlashEmulator emu("../nvs_partition_generator/partition_encrypted_using_keygen.bin");

    char buffer[64];
    FILE *fp;

    fp = fopen(encr_file.c_str(),"rb");
    fread(buffer,sizeof(buffer),1,fp);

    fclose(fp);

    nvs_sec_cfg_t cfg;

    for(int count = 0; count < NVS_KEY_SIZE; count++) {
        cfg.eky[count] = buffer[count] & 255;
        cfg.tky[count] = buffer[count+32] & 255;
    }

    check_nvs_part_gen_args(NVS_DEFAULT_PART_NAME, 4, "../nvs_partition_generator/testdata/sample_multipage_blob.bin", true, &cfg);

}

TEST_CASE("test nvs apis for nvs partition generator utility with encryption enabled using inputkey", "[nvs_part_gen]")
{
    int childpid = fork();
    int status;

    DIR *dir;
    struct dirent *file;
    char *filename;
    char *files;
    char *file_ext;

    dir = opendir("../nvs_partition_generator/keys");
    while ((file = readdir(dir)) != NULL)
    {
        filename = file->d_name;
        files = strrchr(filename, '.');
        if (files != NULL)
        {
            file_ext = files+1;
            if (strncmp(file_ext,"bin",3) == 0)
            {
                break;
            }
        }
    }

    std::string encr_file = std::string("../nvs_partition_generator/keys/") + std::string(filename);

     if (childpid == 0) {
        exit(execlp("python", "python",
                "../nvs_partition_generator/nvs_partition_gen.py",
                "encrypt",
                "../nvs_partition_generator/sample_multipage_blob.csv",
                "partition_encrypted_using_keyfile.bin",
                "0x4000",
                "--inputkey",
                encr_file.c_str(),
                "--outdir",
                "../nvs_partition_generator",NULL));

    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) != -1);
    }

    SpiFlashEmulator emu("../nvs_partition_generator/partition_encrypted_using_keyfile.bin");

    char buffer[64];
    FILE *fp;

    fp = fopen(encr_file.c_str(),"rb");
    fread(buffer,sizeof(buffer),1,fp);

    fclose(fp);

    nvs_sec_cfg_t cfg;

    for(int count = 0; count < NVS_KEY_SIZE; count++) {
        cfg.eky[count] = buffer[count] & 255;
        cfg.tky[count] = buffer[count+32] & 255;
    }

    check_nvs_part_gen_args(NVS_DEFAULT_PART_NAME, 4, "../nvs_partition_generator/testdata/sample_multipage_blob.bin", true, &cfg);

    childpid = fork();
    if (childpid == 0) {
        exit(execlp("rm", " rm",
                    "-rf",
                    "../nvs_partition_generator/keys",NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) != -1);

        childpid = fork();

        if (childpid == 0) {
            exit(execlp("rm", " rm",
                        "-rf",
                        "testdata",NULL));
        } else {
            CHECK(childpid > 0);
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) != -1);
        }
    }

}

TEST_CASE("check and read data from partition generated via manufacturing utility with encryption enabled using sample inputkey", "[mfg_gen]")
{
    int childpid = fork();
    int status;

    if (childpid == 0) {
        exit(execlp("bash", " bash",
                    "-c",
                    "rm -rf ../../../tools/mass_mfg/host_test | \
                    cp -rf ../../../tools/mass_mfg/testdata mfg_testdata | \
                    cp -rf ../nvs_partition_generator/testdata . | \
                    mkdir -p ../../../tools/mass_mfg/host_test",NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) != -1);

        childpid = fork();
        if (childpid == 0) {
            exit(execlp("python", "python",
                        "../../../tools/mass_mfg/mfg_gen.py",
                        "generate",
                        "../../../tools/mass_mfg/samples/sample_config.csv",
                        "../../../tools/mass_mfg/samples/sample_values_multipage_blob.csv",
                        "Test",
                        "0x4000",
                        "--outdir",
                        "../../../tools/mass_mfg/host_test",
                        "--version",
                        "2",
                        "--inputkey",
                        "mfg_testdata/sample_encryption_keys.bin",NULL));

        } else {
            CHECK(childpid > 0);
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) != -1);

            childpid = fork();
            if (childpid == 0) {
                exit(execlp("python", "python",
                            "../nvs_partition_generator/nvs_partition_gen.py",
                            "encrypt",
                            "../../../tools/mass_mfg/host_test/csv/Test-1.csv",
                            "../nvs_partition_generator/Test-1-partition-encrypted.bin",
                            "0x4000",
                            "--version",
                            "2",
                            "--inputkey",
                            "testdata/sample_encryption_keys.bin",NULL));

            } else {
                CHECK(childpid > 0);
                waitpid(childpid, &status, 0);
                CHECK(WEXITSTATUS(status) != -1);

            }

        }

    }

    SpiFlashEmulator emu1("../../../tools/mass_mfg/host_test/bin/Test-1.bin");

    nvs_sec_cfg_t cfg;
    for(int count = 0; count < NVS_KEY_SIZE; count++) {
        cfg.eky[count] = 0x11;
        cfg.tky[count] = 0x22;
    }

    check_nvs_part_gen_args(NVS_DEFAULT_PART_NAME, 4, "mfg_testdata/sample_multipage_blob.bin", true, &cfg);

    SpiFlashEmulator emu2("../nvs_partition_generator/Test-1-partition-encrypted.bin");

    check_nvs_part_gen_args(NVS_DEFAULT_PART_NAME, 4, "testdata/sample_multipage_blob.bin", true, &cfg);


    childpid = fork();
    if (childpid == 0) {
        exit(execlp("bash", " bash",
                    "-c",
                    "rm -rf ../../../tools/mass_mfg/host_test | \
                    rm -rf mfg_testdata | \
                    rm -rf testdata",NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) != -1);

    }

}

TEST_CASE("check and read data from partition generated via manufacturing utility with encryption enabled using new generated key", "[mfg_gen]")
{
    int childpid = fork();
    int status;

    if (childpid == 0) {
        exit(execlp("bash", " bash",
                    "-c",
                    "rm -rf ../../../tools/mass_mfg/host_test | \
                    cp -rf ../../../tools/mass_mfg/testdata mfg_testdata | \
                    cp -rf ../nvs_partition_generator/testdata . | \
                    mkdir -p ../../../tools/mass_mfg/host_test",NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) != -1);

        childpid = fork();
        if (childpid == 0) {
            exit(execlp("python", "python",
                        "../../../tools/mass_mfg/mfg_gen.py",
                        "generate-key",
                        "--outdir",
                        "../../../tools/mass_mfg/host_test",
                        "--keyfile",
                        "encr_keys_host_test.bin",NULL));

        } else {
            CHECK(childpid > 0);
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) != -1);

            childpid = fork();
            if (childpid == 0) {
                exit(execlp("python", "python",
                            "../../../tools/mass_mfg/mfg_gen.py",
                            "generate",
                            "../../../tools/mass_mfg/samples/sample_config.csv",
                            "../../../tools/mass_mfg/samples/sample_values_multipage_blob.csv",
                            "Test",
                            "0x4000",
                            "--outdir",
                            "../../../tools/mass_mfg/host_test",
                            "--version",
                            "2",
                            "--inputkey",
                            "../../../tools/mass_mfg/host_test/keys/encr_keys_host_test.bin",NULL));

            } else {
                CHECK(childpid > 0);
                waitpid(childpid, &status, 0);
                CHECK(WEXITSTATUS(status) != -1);

                childpid = fork();
                if (childpid == 0) {
                    exit(execlp("python", "python",
                                "../nvs_partition_generator/nvs_partition_gen.py",
                                "encrypt",
                                "../../../tools/mass_mfg/host_test/csv/Test-1.csv",
                                "../nvs_partition_generator/Test-1-partition-encrypted.bin",
                                "0x4000",
                                "--version",
                                "2",
                                "--inputkey",
                                "../../../tools/mass_mfg/host_test/keys/encr_keys_host_test.bin",NULL));

                } else {
                    CHECK(childpid > 0);
                    waitpid(childpid, &status, 0);
                    CHECK(WEXITSTATUS(status) != -1);

                }

            }

        }

    }


    SpiFlashEmulator emu1("../../../tools/mass_mfg/host_test/bin/Test-1.bin");

    char buffer[64];
    FILE *fp;

    fp = fopen("../../../tools/mass_mfg/host_test/keys/encr_keys_host_test.bin","rb");
    fread(buffer,sizeof(buffer),1,fp);

    fclose(fp);

    nvs_sec_cfg_t cfg;

    for(int count = 0; count < NVS_KEY_SIZE; count++) {
        cfg.eky[count] = buffer[count] & 255;
        cfg.tky[count] = buffer[count+32] & 255;
    }

    check_nvs_part_gen_args(NVS_DEFAULT_PART_NAME, 4, "mfg_testdata/sample_multipage_blob.bin", true, &cfg);

    SpiFlashEmulator emu2("../nvs_partition_generator/Test-1-partition-encrypted.bin");

    check_nvs_part_gen_args(NVS_DEFAULT_PART_NAME, 4, "testdata/sample_multipage_blob.bin", true, &cfg);

    childpid = fork();
    if (childpid == 0) {
        exit(execlp("bash", " bash",
                    "-c",
                    "rm -rf keys | \
                    rm -rf mfg_testdata | \
                    rm -rf testdata | \
                    rm -rf ../../../tools/mass_mfg/host_test",NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) != -1);

    }

}
#endif

/* Add new tests above */
/* This test has to be the final one */

TEST_CASE("dump all performance data", "[nvs]")
{
    std::cout << "====================" << std::endl << "Dumping benchmarks" << std::endl;
    std::cout << s_perf.str() << std::endl;
    std::cout << "====================" << std::endl;
}
