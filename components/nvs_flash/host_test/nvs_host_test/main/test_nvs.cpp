/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "catch.hpp"
#include "nvs.hpp"
#include "sdkconfig.h"
#include "nvs_partition_manager.hpp"
#include "nvs_partition.hpp"
#include <sstream>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <string>
#include <random>
#include "test_fixtures.hpp"

#define TEST_ESP_ERR(rc, res) CHECK((rc) == (res))
#define TEST_ESP_OK(rc) CHECK((rc) == ESP_OK)

#define TEMPORARILY_DISABLED(x)

stringstream s_perf;

TEST_CASE("crc32 behaves as expected", "[nvs]")
{
    nvs::Item item1;
    item1.datatype = nvs::ItemType::I32;
    item1.nsIndex = 1;
    item1.crc32 = 0;
    item1.chunkIndex = 0xff;
    fill_n(item1.key, sizeof(item1.key), 0xbb);
    fill_n(item1.data, sizeof(item1.data), 0xaa);

    auto crc32_1 = item1.calculateCrc32();

    nvs::Item item2 = item1;
    item2.crc32 = crc32_1;

    CHECK(crc32_1 == item2.calculateCrc32());

    item2 = item1;
    item2.nsIndex = 2;
    CHECK(crc32_1 != item2.calculateCrc32());

    item2 = item1;
    item2.datatype = nvs::ItemType::U32;
    CHECK(crc32_1 != item2.calculateCrc32());

    item2 = item1;
    strncpy(item2.key, "foo", nvs::Item::MAX_KEY_LENGTH);
    CHECK(crc32_1 != item2.calculateCrc32());
}

TEST_CASE("Page starting with empty flash is in uninitialized state", "[nvs]")
{
    PartitionEmulationFixture f;
    nvs::Page page;
    CHECK(page.state() == nvs::Page::PageState::INVALID);
    CHECK(page.load(f.part(), 0) == ESP_OK);
    CHECK(page.state() == nvs::Page::PageState::UNINITIALIZED);
}

TEST_CASE("Page can distinguish namespaces", "[nvs]")
{
    PartitionEmulationFixture f;
    nvs::Page page;
    CHECK(page.load(f.part(), 0) == ESP_OK);
    int32_t val1 = 0x12345678;
    CHECK(page.writeItem(1, nvs::ItemType::I32, "intval1", &val1, sizeof(val1)) == ESP_OK);
    int32_t val2 = 0x23456789;
    CHECK(page.writeItem(2, nvs::ItemType::I32, "intval1", &val2, sizeof(val2)) == ESP_OK);

    int32_t readVal;
    CHECK(page.readItem(2, nvs::ItemType::I32, "intval1", &readVal, sizeof(readVal)) == ESP_OK);
    CHECK(readVal == val2);
}

TEST_CASE("Page reading with different type causes type mismatch error", "[nvs]")
{
    PartitionEmulationFixture f;
    nvs::Page page;
    CHECK(page.load(f.part(), 0) == ESP_OK);
    int32_t val = 0x12345678;
    CHECK(page.writeItem(1, nvs::ItemType::I32, "intval1", &val, sizeof(val)) == ESP_OK);
    CHECK(page.readItem(1, nvs::ItemType::U32, "intval1", &val, sizeof(val)) == ESP_ERR_NVS_TYPE_MISMATCH);
}

TEST_CASE("Page when erased, it's state becomes UNITIALIZED", "[nvs]")
{
    PartitionEmulationFixture f;
    nvs::Page page;
    CHECK(page.load(f.part(), 0) == ESP_OK);
    int32_t val = 0x12345678;
    CHECK(page.writeItem(1, nvs::ItemType::I32, "intval1", &val, sizeof(val)) == ESP_OK);
    CHECK(page.erase() == ESP_OK);
    CHECK(page.state() == nvs::Page::PageState::UNINITIALIZED);
}

TEST_CASE("Page when writing and erasing, used/erased counts are updated correctly", "[nvs]")
{
    PartitionEmulationFixture f;
    nvs::Page page;
    CHECK(page.load(f.part(), 0) == ESP_OK);
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
    for (size_t i = 0; i < nvs::Page::ENTRY_COUNT - 2; ++i) {
        char name[16];
        snprintf(name, sizeof(name), "i%ld", (long int)i);
        CHECK(page.writeItem(1, name, i) == ESP_OK);
    }
    CHECK(page.getUsedEntryCount() == nvs::Page::ENTRY_COUNT - 1);
    CHECK(page.getErasedEntryCount() == 1);
    for (size_t i = 0; i < nvs::Page::ENTRY_COUNT - 2; ++i) {
        char name[16];
        snprintf(name, sizeof(name), "i%ld", (long int)i);
        CHECK(page.eraseItem(1, nvs::itemTypeOf<size_t>(), name) == ESP_OK);
    }
    CHECK(page.getUsedEntryCount() == 1);
    CHECK(page.getErasedEntryCount() == nvs::Page::ENTRY_COUNT - 1);
}

TEST_CASE("Page when page is full, adding an element fails", "[nvs]")
{
    PartitionEmulationFixture f;
    nvs::Page page;
    CHECK(page.load(f.part(), 0) == ESP_OK);
    for (size_t i = 0; i < nvs::Page::ENTRY_COUNT; ++i) {
        char name[16];
        snprintf(name, sizeof(name), "i%ld", (long int)i);
        CHECK(page.writeItem(1, name, i) == ESP_OK);
    }
    CHECK(page.writeItem(1, "foo", 64UL) == ESP_ERR_NVS_PAGE_FULL);
}

TEST_CASE("Page maintains its seq number")
{
    PartitionEmulationFixture f;
    {
        nvs::Page page;
        CHECK(page.load(f.part(), 0) == ESP_OK);
        CHECK(page.setSeqNumber(123) == ESP_OK);
        int32_t val = 42;
        CHECK(page.writeItem(1, nvs::ItemType::I32, "dummy", &val, sizeof(val)) == ESP_OK);
    }
    {
        nvs::Page page;
        CHECK(page.load(f.part(), 0) == ESP_OK);
        uint32_t seqno;
        CHECK(page.getSeqNumber(seqno) == ESP_OK);
        CHECK(seqno == 123);
    }
}

TEST_CASE("Page can write and read variable length data", "[nvs]")
{
    PartitionEmulationFixture f;
    nvs::Page page;
    CHECK(page.load(f.part(), 0) == ESP_OK);
    const char str[] = "foobar1234foobar1234foobar1234foobar1234foobar1234foobar1234foobar1234foobar1234";
    size_t len = strlen(str);
    CHECK(page.writeItem(1, "stuff1", 42) == ESP_OK);
    CHECK(page.writeItem(1, "stuff2", 1) == ESP_OK);
    CHECK(page.writeItem(1, nvs::ItemType::SZ, "foobaar", str, len + 1) == ESP_OK);
    CHECK(page.writeItem(1, "stuff3", 2) == ESP_OK);
    CHECK(page.writeItem(1, nvs::ItemType::BLOB, "baz", str, len) == ESP_OK);
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
    CHECK(page.readItem(1, nvs::ItemType::SZ, "foobaar", buf, sizeof(buf)) == ESP_OK);
    CHECK(memcmp(buf, str, strlen(str) + 1) == 0);

    fill_n(buf, sizeof(buf), 0xff);
    CHECK(page.readItem(1, nvs::ItemType::BLOB, "baz", buf, sizeof(buf)) == ESP_OK);
    CHECK(memcmp(buf, str, strlen(str)) == 0);
}

TEST_CASE("Page different key names are distinguished even if the pointer is the same", "[nvs]")
{
    PartitionEmulationFixture f;
    nvs::Page page;
    TEST_ESP_OK(page.load(f.part(), 0));
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
    PartitionEmulationFixture f(0, 4);
    nvs::Page page;
    TEST_ESP_OK(page.load(f.part(), 0));
    // 16-character key fails
    TEST_ESP_ERR(page.writeItem(1, "0123456789123456", 1), ESP_ERR_NVS_KEY_TOO_LONG);
    // 15-character key is okay
    TEST_ESP_OK(page.writeItem(1, "012345678912345", 1));
}

TEST_CASE("Page validates blob size", "[nvs]")
{
    PartitionEmulationFixture f(0, 4);
    nvs::Page page;
    TEST_ESP_OK(page.load(f.part(), 0));

    char buf[4096] = { 0 };
    // There are two potential errors here:
    // - not enough space in the page (because one value has been written already)
    // - value is too long
    // Check that the second one is actually returned.
    TEST_ESP_ERR(page.writeItem(1, nvs::ItemType::BLOB, "2", buf, nvs::Page::ENTRY_COUNT * nvs::Page::ENTRY_SIZE), ESP_ERR_NVS_VALUE_TOO_LONG);
    // Should fail as well
    TEST_ESP_ERR(page.writeItem(1, nvs::ItemType::BLOB, "2", buf, nvs::Page::CHUNK_MAX_SIZE + 1), ESP_ERR_NVS_VALUE_TOO_LONG);
    TEST_ESP_OK(page.writeItem(1, nvs::ItemType::BLOB, "2", buf, nvs::Page::CHUNK_MAX_SIZE));
}

class HashListTestHelper : public nvs::HashList {
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
        nvs::Item item(1, nvs::ItemType::U32, 1, key);
        hashlist.insert(item, i);
    }
    INFO("Added " << count << " items, " << hashlist.getBlockCount() << " blocks");
    // Remove them in reverse order
    for (size_t i = count; i > 0; --i) {
        // Make sure that the element existed before it's erased
        CHECK(hashlist.erase(i - 1) == true);
    }
    CHECK(hashlist.getBlockCount() == 0);
    // Add again
    for (size_t i = 0; i < count; ++i) {
        char key[16];
        snprintf(key, sizeof(key), "i%ld", (long int)i);
        nvs::Item item(1, nvs::ItemType::U32, 1, key);
        hashlist.insert(item, i);
    }
    INFO("Added " << count << " items, " << hashlist.getBlockCount() << " blocks");
    // Remove them in the same order
    for (size_t i = 0; i < count; ++i) {
        CHECK(hashlist.erase(i) == true);
    }
    CHECK(hashlist.getBlockCount() == 0);
}

TEST_CASE("can init PageManager in empty flash", "[nvs]")
{
    PartitionEmulationFixture f(0, 4);
    nvs::PageManager pm;
    CHECK(pm.load(f.part(), 0, 4) == ESP_OK);
}

TEST_CASE("PageManager adds page in the correct order", "[nvs]")
{
    const size_t pageCount = 8;
    PartitionEmulationFixture f(0, pageCount);
    uint32_t pageNo[pageCount] = { -1U, 50, 11, -1U, 23, 22, 24, 49};

    for (uint32_t i = 0; i < pageCount; ++i) {
        nvs::Page p;
        p.load(f.part(), i);
        if (pageNo[i] != -1U) {
            p.setSeqNumber(pageNo[i]);
            p.writeItem(1, "foo", 10U);
        }
    }

    nvs::PageManager pageManager;
    CHECK(pageManager.load(f.part(), 0, pageCount) == ESP_OK);

    uint32_t lastSeqNo = 0;
    for (auto it = std::begin(pageManager); it != std::end(pageManager); ++it) {
        uint32_t seqNo;
        CHECK(it->getSeqNumber(seqNo) == ESP_OK);
        CHECK(seqNo > lastSeqNo);
    }
}

TEST_CASE("can init storage in empty flash", "[nvs]")
{
    PartitionEmulationFixture f(0, 8);
    nvs::Storage storage(f.part());
    TEMPORARILY_DISABLED(f.emu.setBounds(4, 8);)
    cout << "before check" << endl;
    CHECK(storage.init(4, 4) == ESP_OK);
    TEMPORARILY_DISABLED(s_perf << "Time to init empty storage (4 sectors): " << f.emu.getTotalTime() << " us" << std::endl;)
}

TEST_CASE("storage doesn't add duplicates within one page", "[nvs]")
{
    PartitionEmulationFixture f(0, 8);
    nvs::Storage storage(f.part());
    TEMPORARILY_DISABLED(f.emu.setBounds(4, 8);)
    CHECK(storage.init(4, 4) == ESP_OK);
    int bar = 0;
    CHECK(storage.writeItem(1, "bar", ++bar) == ESP_OK);
    CHECK(storage.writeItem(1, "bar", ++bar) == ESP_OK);

    nvs::Page page;
    page.load(f.part(), 4);
    CHECK(page.getUsedEntryCount() == 1);
    CHECK(page.getErasedEntryCount() == 1);
}

TEST_CASE("can write one item a thousand times", "[nvs]")
{
    PartitionEmulationFixture f(0, 8);
    nvs::Storage storage(f.part());
    TEMPORARILY_DISABLED(f.emu.setBounds(4, 8);)
    CHECK(storage.init(4, 4) == ESP_OK);
    for (size_t i = 0; i < nvs::Page::ENTRY_COUNT * 4 * 2; ++i) {
        REQUIRE(storage.writeItem(1, "i", static_cast<int>(i)) == ESP_OK);
    }
    TEMPORARILY_DISABLED(s_perf << "Time to write one item a thousand times: " << f.emu.getTotalTime() << " us (" << f.emu.getEraseOps() << " " << f.emu.getWriteOps() << " " << f.emu.getReadOps() << " " << f.emu.getWriteBytes() << " " << f.emu.getReadBytes() << ")" << std::endl;)
}

TEST_CASE("storage doesn't add duplicates within multiple pages", "[nvs]")
{
    PartitionEmulationFixture f(0, 8);
    nvs::Storage storage(f.part());
    TEMPORARILY_DISABLED(f.emu.setBounds(4, 8);)
    CHECK(storage.init(4, 4) == ESP_OK);
    int bar = 0;
    CHECK(storage.writeItem(1, "bar", ++bar) == ESP_OK);
    for (size_t i = 0; i < nvs::Page::ENTRY_COUNT; ++i) {
        CHECK(storage.writeItem(1, "foo", static_cast<int>(++bar)) == ESP_OK);
    }
    CHECK(storage.writeItem(1, "bar", ++bar) == ESP_OK);

    nvs::Page page;
    page.load(f.part(), 4);
    CHECK(page.findItem(1, nvs::itemTypeOf<int>(), "bar") == ESP_ERR_NVS_NOT_FOUND);
    page.load(f.part(), 5);
    CHECK(page.findItem(1, nvs::itemTypeOf<int>(), "bar") == ESP_OK);
}

TEST_CASE("storage can find items on second page if first is not fully written and has cached search data", "[nvs]")
{
    PartitionEmulationFixture f(0, 3);
    nvs::Storage storage(f.part());
    CHECK(storage.init(0, 3) == ESP_OK);
    int bar = 0;
    uint8_t bigdata[(nvs::Page::CHUNK_MAX_SIZE - nvs::Page::ENTRY_SIZE) / 2] = {0};
    // write one big chunk of data
    ESP_ERROR_CHECK(storage.writeItem(0, nvs::ItemType::BLOB, "1", bigdata, sizeof(bigdata)));
    // write another big chunk of data
    ESP_ERROR_CHECK(storage.writeItem(0, nvs::ItemType::BLOB, "2", bigdata, sizeof(bigdata)));

    // write third one; it will not fit into the first page
    ESP_ERROR_CHECK(storage.writeItem(0, nvs::ItemType::BLOB, "3", bigdata, sizeof(bigdata)));

    size_t size;
    ESP_ERROR_CHECK(storage.getItemDataSize(0, nvs::ItemType::BLOB, "1", size));
    CHECK(size == sizeof(bigdata));
    ESP_ERROR_CHECK(storage.getItemDataSize(0, nvs::ItemType::BLOB, "3", size));
    CHECK(size == sizeof(bigdata));
}

TEST_CASE("can write and read variable length data lots of times", "[nvs]")
{
    PartitionEmulationFixture f(0, 8);
    nvs::Storage storage(f.part());
    TEMPORARILY_DISABLED(f.emu.setBounds(4, 8);)
    CHECK(storage.init(4, 4) == ESP_OK);
    const char str[] = "foobar1234foobar1234foobar1234foobar1234foobar1234foobar1234foobar1234foobar1234";
    char buf[sizeof(str) + 16];
    size_t len = strlen(str);
    for (size_t i = 0; i < nvs::Page::ENTRY_COUNT * 4 * 2; ++i) {
        CAPTURE(i);
        CHECK(storage.writeItem(1, nvs::ItemType::SZ, "foobaar", str, len + 1) == ESP_OK);
        CHECK(storage.writeItem(1, "foo", static_cast<uint32_t>(i)) == ESP_OK);

        uint32_t value;
        CHECK(storage.readItem(1, "foo", value) == ESP_OK);
        CHECK(value == i);

        fill_n(buf, sizeof(buf), 0xff);
        CHECK(storage.readItem(1, nvs::ItemType::SZ, "foobaar", buf, sizeof(buf)) == ESP_OK);
        CHECK(memcmp(buf, str, strlen(str) + 1) == 0);
    }
    TEMPORARILY_DISABLED(s_perf << "Time to write one string and one integer a thousand times: " << f.emu.getTotalTime() << " us (" << f.emu.getEraseOps() << " " << f.emu.getWriteOps() << " " << f.emu.getReadOps() << " " << f.emu.getWriteBytes() << " " << f.emu.getReadBytes() << ")" << std::endl;)
}

TEST_CASE("can get length of variable length data", "[nvs]")
{
    PartitionEmulationFixture f(0, 8);
    TEMPORARILY_DISABLED(f.emu.randomize(200);)
    nvs::Storage storage(f.part());
    TEMPORARILY_DISABLED(f.emu.setBounds(4, 8);)
    CHECK(storage.init(4, 4) == ESP_OK);
    const char str[] = "foobar1234foobar1234foobar1234foobar1234foobar1234foobar1234foobar1234foobar1234";
    size_t len = strlen(str);
    CHECK(storage.writeItem(1, nvs::ItemType::SZ, "foobaar", str, len + 1) == ESP_OK);
    size_t dataSize;
    CHECK(storage.getItemDataSize(1, nvs::ItemType::SZ, "foobaar", dataSize) == ESP_OK);
    CHECK(dataSize == len + 1);

    CHECK(storage.writeItem(2, nvs::ItemType::BLOB, "foobaar", str, len) == ESP_OK);
    CHECK(storage.getItemDataSize(2, nvs::ItemType::BLOB, "foobaar", dataSize) == ESP_OK);
    CHECK(dataSize == len);
}

TEST_CASE("can create namespaces", "[nvs]")
{
    PartitionEmulationFixture f(0, 8);
    nvs::Storage storage(f.part());
    TEMPORARILY_DISABLED(f.emu.setBounds(4, 8);)
    CHECK(storage.init(4, 4) == ESP_OK);
    uint8_t nsi;
    CHECK(storage.createOrOpenNamespace("wifi", false, nsi) == ESP_ERR_NVS_NOT_FOUND);

    CHECK(storage.createOrOpenNamespace("wifi", true, nsi) == ESP_OK);
    nvs::Page page;
    page.load(f.part(), 4);
    CHECK(page.findItem(nvs::Page::NS_INDEX, nvs::ItemType::U8, "wifi") == ESP_OK);
}

TEST_CASE("storage may become full", "[nvs]")
{
    PartitionEmulationFixture f(0, 8);
    nvs::Storage storage(f.part());
    TEMPORARILY_DISABLED(f.emu.setBounds(4, 8);)
    CHECK(storage.init(4, 4) == ESP_OK);
    for (size_t i = 0; i < nvs::Page::ENTRY_COUNT * 3; ++i) {
        char name[nvs::Item::MAX_KEY_LENGTH + 1];
        snprintf(name, sizeof(name), "key%05d", static_cast<int>(i));
        REQUIRE(storage.writeItem(1, name, static_cast<int>(i)) == ESP_OK);
    }
    REQUIRE(storage.writeItem(1, "foo", 10) == ESP_ERR_NVS_NOT_ENOUGH_SPACE);
}

TEST_CASE("can modify an item on a page which will be erased", "[nvs]")
{
    PartitionEmulationFixture f(0, 8);
    nvs::Storage storage(f.part());
    CHECK(storage.init(0, 2) == ESP_OK);
    for (size_t i = 0; i < nvs::Page::ENTRY_COUNT * 3 + 1; ++i) {
        REQUIRE(storage.writeItem(1, "foo", 42U) == ESP_OK);
    }
}

TEST_CASE("erase operations are distributed among sectors", "[nvs]")
{
    const size_t sectors = 6;
    PartitionEmulationFixture f(0, sectors);
    nvs::Storage storage(f.part());
    CHECK(storage.init(0, sectors) == ESP_OK);

    /* Fill some part of storage with static values */
    const size_t static_sectors = 2;
    for (size_t i = 0; i < static_sectors * nvs::Page::ENTRY_COUNT; ++i) {
        char name[nvs::Item::MAX_KEY_LENGTH];
        snprintf(name, sizeof(name), "static%d", (int) i);
        REQUIRE(storage.writeItem(1, name, i) == ESP_OK);
    }

    /* Now perform many write operations */
    const size_t write_ops = 2000;
    for (size_t i = 0; i < write_ops; ++i) {
        REQUIRE(storage.writeItem(1, "value", i) == ESP_OK);
    }

    /* Check that erase counts are distributed between the remaining sectors */
    const size_t max_erase_cnt = write_ops / nvs::Page::ENTRY_COUNT / (sectors - static_sectors) + 1;
    for (size_t i = 0; i < sectors; ++i) {
        TEMPORARILY_DISABLED(
            auto erase_cnt = f.emu.getSectorEraseCount(i);
            INFO("Sector " << i << " erased " << erase_cnt);
            CHECK(erase_cnt <= max_erase_cnt);
        )
    }
}

TEST_CASE("can erase items", "[nvs]")
{
    PartitionEmulationFixture f(0, 8);
    nvs::Storage storage(f.part());
    CHECK(storage.init(0, 3) == ESP_OK);
    for (size_t i = 0; i < nvs::Page::ENTRY_COUNT * 2 - 3; ++i) {
        char name[nvs::Item::MAX_KEY_LENGTH + 1];
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

TEST_CASE("readonly handle fails on writing", "[nvs]")
{
    PartitionEmulationFixture f(0, 10);
    const char *str = "value 0123456789abcdef0123456789abcdef";
    const uint8_t blob[8] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7};

    nvs_handle_t handle_1;
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    TEMPORARILY_DISABLED(f.emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);)

    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(),
                NVS_FLASH_SECTOR,
                NVS_FLASH_SECTOR_COUNT_MIN));

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
    PartitionEmulationFixture f(0, 10);
    TEMPORARILY_DISABLED(f.emu.randomize(100);)

    nvs_handle_t handle_1;
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    TEMPORARILY_DISABLED(f.emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);)


    TEST_ESP_ERR(nvs_open("namespace1", NVS_READWRITE, &handle_1), ESP_ERR_NVS_NOT_INITIALIZED);
    for (uint16_t i = NVS_FLASH_SECTOR; i < NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN; ++i) {
        TEMPORARILY_DISABLED(f.emu.erase(i);)
    }
    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(),
                NVS_FLASH_SECTOR,
                NVS_FLASH_SECTOR_COUNT_MIN));

    TEST_ESP_ERR(nvs_open("namespace1", NVS_READONLY, &handle_1), ESP_ERR_NVS_NOT_FOUND);

    TEST_ESP_OK(nvs_open("namespace1", NVS_READWRITE, &handle_1));
    TEST_ESP_OK(nvs_set_i32(handle_1, "foo", 0x12345678));
    TEST_ESP_OK(nvs_set_i32(handle_1, "foo", 0x23456789));

    nvs_handle_t handle_2;
    TEST_ESP_OK(nvs_open("namespace2", NVS_READWRITE, &handle_2));
    TEST_ESP_OK(nvs_set_i32(handle_2, "foo", 0x3456789a));
    const char *str = "value 0123456789abcdef0123456789abcdef";
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

TEST_CASE("deinit partition doesn't affect other partition's open handles", "[nvs]")
{
    const char *OTHER_PARTITION_NAME = "other_part";
    PartitionEmulationFixture f(0, 10);
    PartitionEmulationFixture f_other(0, 10, OTHER_PARTITION_NAME);
    const char *str = "value 0123456789abcdef0123456789abcdef";
    const uint8_t blob[8] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7};

    nvs_handle_t handle_1;
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    TEMPORARILY_DISABLED(f.emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);)
    TEMPORARILY_DISABLED(f_other.emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);)

    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(),
                NVS_FLASH_SECTOR,
                NVS_FLASH_SECTOR_COUNT_MIN));
    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(f_other.part(),
                NVS_FLASH_SECTOR,
                NVS_FLASH_SECTOR_COUNT_MIN));

    TEST_ESP_OK(nvs_open_from_partition(OTHER_PARTITION_NAME, "ns", NVS_READWRITE, &handle_1));

    // Deinitializing must not interfere with the open handle from the other partition.
    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));

    TEST_ESP_OK(nvs_set_i32(handle_1, "foo", 0x3456789a));
    nvs_close(handle_1);

    TEST_ESP_OK(nvs_flash_deinit_partition(OTHER_PARTITION_NAME));
}

TEST_CASE("nvs iterator nvs_entry_find invalid parameter test", "[nvs]")
{
    nvs_iterator_t it = reinterpret_cast<nvs_iterator_t>(0xbeef);
    CHECK(nvs_entry_find(nullptr, NULL, NVS_TYPE_ANY, &it) == ESP_ERR_INVALID_ARG);
    CHECK(nvs_entry_find("nvs", NULL, NVS_TYPE_ANY, nullptr) == ESP_ERR_INVALID_ARG);
}

TEST_CASE("nvs iterator nvs_entry_find doesn't change iterator on parameter error", "[nvs]")
{
    nvs_iterator_t it = reinterpret_cast<nvs_iterator_t>(0xbeef);
    REQUIRE(nvs_entry_find(nullptr, NULL, NVS_TYPE_ANY, &it) == ESP_ERR_INVALID_ARG);
    CHECK(it == reinterpret_cast<nvs_iterator_t>(0xbeef));

    it = nullptr;
    REQUIRE(nvs_entry_find(nullptr, NULL, NVS_TYPE_ANY, &it) == ESP_ERR_INVALID_ARG);
    CHECK(it == nullptr);
}

TEST_CASE("nvs_entry_next return ESP_ERR_INVALID_ARG on parameter is NULL", "[nvs]")
{
    CHECK(nvs_entry_next(nullptr) == ESP_ERR_INVALID_ARG);
}

TEST_CASE("nvs_entry_info fails with ESP_ERR_INVALID_ARG if a parameter is NULL", "[nvs]")
{
    nvs_iterator_t it = reinterpret_cast<nvs_iterator_t>(0xbeef);
    nvs_entry_info_t info;
    CHECK(nvs_entry_info(it, nullptr) == ESP_ERR_INVALID_ARG);
    CHECK(nvs_entry_info(nullptr, &info) == ESP_ERR_INVALID_ARG);
}

TEST_CASE("nvs_entry_info doesn't change iterator on parameter error", "[nvs]")
{
    nvs_iterator_t it = reinterpret_cast<nvs_iterator_t>(0xbeef);
    nvs_entry_info_t info;
    REQUIRE(nvs_entry_info(it, nullptr) == ESP_ERR_INVALID_ARG);
    CHECK(it == reinterpret_cast<nvs_iterator_t>(0xbeef));

    it = nullptr;
    REQUIRE(nvs_entry_info(it, nullptr) == ESP_ERR_INVALID_ARG);
    CHECK(it == nullptr);
}

TEST_CASE("nvs iterators tests", "[nvs]")
{
    PartitionEmulationFixture f(0, 5);

    const uint32_t NVS_FLASH_SECTOR = 0;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 5;
    TEMPORARILY_DISABLED(f.emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);)

    for (uint16_t i = NVS_FLASH_SECTOR; i < NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN; ++i) {
        TEMPORARILY_DISABLED(f.emu.erase(i);)
    }
    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(),
                NVS_FLASH_SECTOR,
                NVS_FLASH_SECTOR_COUNT_MIN));

    nvs_iterator_t it;
    nvs_entry_info_t info;
    nvs_handle_t handle_1;
    nvs_handle_t handle_2;
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
        int count = 0;
        nvs_iterator_t it = nullptr;
        esp_err_t res = nvs_entry_find(part, name, type, &it);
        for (count = 0; res == ESP_OK; count++)
        {
            res = nvs_entry_next(&it);
        }
        CHECK(res == ESP_ERR_NVS_NOT_FOUND); // after finishing the loop or if no entry was found to begin with,
        // res has to be ESP_ERR_NVS_NOT_FOUND or some internal error
        // or programming error occurred
        nvs_release_iterator(it); // unneccessary call but emphasizes the programming pattern
        return count;
    };

    SECTION("No partition found return ESP_ERR_NVS_NOT_FOUND") {
        CHECK(nvs_entry_find("", NULL, NVS_TYPE_ANY, &it) == ESP_ERR_NVS_NOT_FOUND);
    }

    SECTION("No matching namespace found return ESP_ERR_NVS_NOT_FOUND") {
        CHECK(nvs_entry_find(NVS_DEFAULT_PART_NAME, "nonexistent", NVS_TYPE_ANY, &it) == ESP_ERR_NVS_NOT_FOUND);
    }

    SECTION("nvs_entry_find sets iterator to null if no matching element found") {
        it = reinterpret_cast<nvs_iterator_t>(0xbeef);
        REQUIRE(nvs_entry_find(NVS_DEFAULT_PART_NAME, "nonexistent", NVS_TYPE_I16, &it) == ESP_ERR_NVS_NOT_FOUND);
        CHECK(it == nullptr);
    }

    SECTION("Finding iterator means iterator is valid") {
        it = nullptr;
        CHECK(nvs_entry_find(NVS_DEFAULT_PART_NAME, nullptr, NVS_TYPE_ANY, &it) == ESP_OK);
        CHECK(it != nullptr);
        nvs_release_iterator(it);
    }

    SECTION("Return ESP_ERR_NVS_NOT_FOUND after iterating over last matching element") {
        it = nullptr;
        REQUIRE(nvs_entry_find(NVS_DEFAULT_PART_NAME, name_1, NVS_TYPE_I16, &it) == ESP_OK);
        REQUIRE(it != nullptr);
        CHECK(nvs_entry_next(&it) == ESP_ERR_NVS_NOT_FOUND);
    }

    SECTION("Set iterator to NULL after iterating over last matching element") {
        it = nullptr;
        REQUIRE(nvs_entry_find(NVS_DEFAULT_PART_NAME, name_1, NVS_TYPE_I16, &it) == ESP_OK);
        REQUIRE(it != nullptr);
        REQUIRE(nvs_entry_next(&it) == ESP_ERR_NVS_NOT_FOUND);
        CHECK(it == nullptr);
    }

    SECTION("Number of entries found for specified namespace and type is correct") {
        CHECK(entry_count(NVS_DEFAULT_PART_NAME, NULL, NVS_TYPE_ANY) == 15);
        CHECK(entry_count(NVS_DEFAULT_PART_NAME, name_1, NVS_TYPE_ANY) == 11);
        CHECK(entry_count(NVS_DEFAULT_PART_NAME, name_1, NVS_TYPE_I32) == 3);
        CHECK(entry_count(NVS_DEFAULT_PART_NAME, NULL, NVS_TYPE_I32) == 5);
        CHECK(entry_count(NVS_DEFAULT_PART_NAME, NULL, NVS_TYPE_U64) == 1);
    }

    SECTION("New entry is not created when existing key-value pair is set") {
        CHECK(entry_count(NVS_DEFAULT_PART_NAME, name_2, NVS_TYPE_ANY) == 4);
        TEST_ESP_OK(nvs_set_i32(handle_2, "value1", -222));
        CHECK(entry_count(NVS_DEFAULT_PART_NAME, name_2, NVS_TYPE_ANY) == 4);
    }

    SECTION("Number of entries found decrease when entry is erased") {
        CHECK(entry_count(NVS_DEFAULT_PART_NAME, NULL, NVS_TYPE_U64) == 1);
        TEST_ESP_OK(nvs_erase_key(handle_2, "value4"));
        CHECK(entry_count(NVS_DEFAULT_PART_NAME, "", NVS_TYPE_U64) == 0);
    }

    SECTION("All fields of nvs_entry_info_t structure are correct") {
        it = nullptr;
        esp_err_t res = nvs_entry_find(NVS_DEFAULT_PART_NAME, name_1, NVS_TYPE_I32, &it);
        REQUIRE(res == ESP_OK);
        string key = "value5";
        while (res == ESP_OK) {
            REQUIRE(nvs_entry_info(it, &info) == ESP_OK);

            CHECK(string(name_1) == info.namespace_name);
            CHECK(key == info.key);
            CHECK(info.type == NVS_TYPE_I32);

            res = nvs_entry_next(&it);
            key[5]++;
        }
        CHECK(res == ESP_ERR_NVS_NOT_FOUND); // after finishing the loop, res has to be ESP_ERR_NVS_NOT_FOUND
        // or some internal error or programming error occurred
        CHECK(key == "value8");
        nvs_release_iterator(it); // unneccessary call but emphasizes the programming pattern
    }

    SECTION("Entry info is not affected by subsequent erase") {
        nvs_entry_info_t info_after_erase;

        CHECK(nvs_entry_find(NVS_DEFAULT_PART_NAME, name_1, NVS_TYPE_ANY, &it) == ESP_OK);
        REQUIRE(nvs_entry_info(it, &info) == ESP_OK);
        TEST_ESP_OK(nvs_erase_key(handle_1, "value1"));
        REQUIRE(nvs_entry_info(it, &info_after_erase) == ESP_OK);
        CHECK(memcmp(&info, &info_after_erase, sizeof(info)) == 0);
        nvs_release_iterator(it);
    }

    SECTION("Entry info is not affected by subsequent set") {
        nvs_entry_info_t info_after_set;

        CHECK(nvs_entry_find(NVS_DEFAULT_PART_NAME, name_1, NVS_TYPE_ANY, &it) == ESP_OK);
        REQUIRE(nvs_entry_info(it, &info) == ESP_OK);
        TEST_ESP_OK(nvs_set_u8(handle_1, info.key, 44));
        REQUIRE(nvs_entry_info(it, &info_after_set) == ESP_OK);
        CHECK(memcmp(&info, &info_after_set, sizeof(info)) == 0);
        nvs_release_iterator(it);
    }


    SECTION("Iterating over multiple pages works correctly") {
        nvs_handle_t handle_3;
        const char *name_3 = "namespace3";
        const int entries_created = 250;

        TEST_ESP_OK(nvs_open(name_3, NVS_READWRITE, &handle_3));
        for  (size_t i = 0; i < entries_created; i++) {
            TEST_ESP_OK(nvs_set_u8(handle_3, to_string(i).c_str(), 123));
        }

        int entries_found = 0;
        it = nullptr;
        esp_err_t res = nvs_entry_find(NVS_DEFAULT_PART_NAME, name_3, NVS_TYPE_ANY, &it);
        while (res == ESP_OK) {
            entries_found++;
            res = nvs_entry_next(&it);
        }
        CHECK(res == ESP_ERR_NVS_NOT_FOUND); // after finishing the loop, res has to be ESP_ERR_NVS_NOT_FOUND
        // or some internal error or programming error occurred
        CHECK(entries_created == entries_found);

        nvs_release_iterator(it); // unneccessary call but emphasizes the programming pattern
        nvs_close(handle_3);
    }

    SECTION("Iterating over multi-page blob works correctly") {
        nvs_handle_t handle_3;
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
    PartitionEmulationFixture f(0, 5);
    nvs_iterator_t it;
    nvs_handle_t my_handle;
    const char *NAMESPACE = "test_ns_4";

    const uint32_t NVS_FLASH_SECTOR = 0;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 5;
    TEMPORARILY_DISABLED(f.emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);)

    for (uint16_t i = NVS_FLASH_SECTOR; i < NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN; ++i) {
        TEMPORARILY_DISABLED(f.emu.erase(i);)
    }
    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(),
                NVS_FLASH_SECTOR,
                NVS_FLASH_SECTOR_COUNT_MIN));

    // writing string to namespace (a type which spans multiple entries)
    TEST_ESP_OK(nvs_open(NAMESPACE, NVS_READWRITE, &my_handle));
    TEST_ESP_OK(nvs_set_str(my_handle, "test-string", "InitString0"));
    TEST_ESP_OK(nvs_commit(my_handle));
    nvs_close(my_handle);

    CHECK(nvs_entry_find(NVS_DEFAULT_PART_NAME, NAMESPACE, NVS_TYPE_I32, &it) == ESP_ERR_NVS_NOT_FOUND);

    // re-init to trigger cleaning up of broken items -> a corrupted string will be erased
    nvs_flash_deinit();
    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(),
                NVS_FLASH_SECTOR,
                NVS_FLASH_SECTOR_COUNT_MIN));

    CHECK(nvs_entry_find(NVS_DEFAULT_PART_NAME, NAMESPACE, NVS_TYPE_STR, &it) == ESP_OK);
    nvs_release_iterator(it);

    // without deinit it affects "nvs api tests"
    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
}

TEST_CASE("wifi test", "[nvs]")
{
    PartitionEmulationFixture f(0, 10);
    TEMPORARILY_DISABLED(f.emu.randomize(10);)

    const uint32_t NVS_FLASH_SECTOR = 5;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    TEMPORARILY_DISABLED(f.emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);)
    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(),
                NVS_FLASH_SECTOR,
                NVS_FLASH_SECTOR_COUNT_MIN));

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

    //s_perf << "Time to simulate nvs init with wifi libs: " << f.emu.getTotalTime() << " us (" << f.emu.getEraseOps() << "E " << f.emu.getWriteOps() << "W " << f.emu.getReadOps() << "R " << f.emu.getWriteBytes() << "Wb " << f.emu.getReadBytes() << "Rb)" << std::endl;

    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
}

extern "C" void nvs_dump(const char *partName);

class RandomTest {

    static const size_t nKeys = 11;
    int32_t v1 = 0, v2 = 0;
    uint64_t v3 = 0, v4 = 0;
    static const size_t strBufLen = 1024;
    static const size_t smallBlobLen = nvs::Page::CHUNK_MAX_SIZE / 3;
    static const size_t largeBlobLen = nvs::Page::CHUNK_MAX_SIZE * 3;
    char v5[strBufLen], v6[strBufLen], v7[strBufLen], v8[strBufLen], v9[strBufLen];
    uint8_t v10[smallBlobLen], v11[largeBlobLen];
    bool written[nKeys];

public:
    RandomTest()
    {
        std::fill_n(written, nKeys, false);
    }

    template<typename TGen>
    esp_err_t doRandomThings(nvs_handle_t handle, TGen gen, size_t &count)
    {

        const char *keys[] = {"foo", "bar", "longkey_0123456", "another key", "param1", "param2", "param3", "param4", "param5", "singlepage", "multipage"};
        const nvs::ItemType types[] = {nvs::ItemType::I32, nvs::ItemType::I32, nvs::ItemType::U64, nvs::ItemType::U64, nvs::ItemType::SZ, nvs::ItemType::SZ, nvs::ItemType::SZ, nvs::ItemType::SZ, nvs::ItemType::SZ, nvs::ItemType::BLOB, nvs::ItemType::BLOB};

        void *values[] = {&v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8, &v9, &v10, &v11};

        const size_t nKeys = sizeof(keys) / sizeof(keys[0]);
        static_assert(nKeys == sizeof(types) / sizeof(types[0]), "");
        static_assert(nKeys == sizeof(values) / sizeof(values[0]), "");

        auto randomRead = [&](size_t index) -> esp_err_t {
            switch (types[index])
            {
            case nvs::ItemType::I32: {
                int32_t val;
                auto err = nvs_get_i32(handle, keys[index], &val);
                if (err == ESP_ERR_FLASH_OP_FAIL) {
                    return err;
                }
                if (!written[index]) {
                    REQUIRE(err == ESP_ERR_NVS_NOT_FOUND);
                } else {
                    REQUIRE(err == ESP_OK);
                    REQUIRE(val == *reinterpret_cast<int32_t *>(values[index]));
                }
                break;
            }

            case nvs::ItemType::U64: {
                uint64_t val;
                auto err = nvs_get_u64(handle, keys[index], &val);
                if (err == ESP_ERR_FLASH_OP_FAIL) {
                    return err;
                }
                if (!written[index]) {
                    REQUIRE(err == ESP_ERR_NVS_NOT_FOUND);
                } else {
                    REQUIRE(err == ESP_OK);
                    REQUIRE(val == *reinterpret_cast<uint64_t *>(values[index]));
                }
                break;
            }

            case nvs::ItemType::SZ: {
                char buf[strBufLen];
                size_t len = strBufLen;
                auto err = nvs_get_str(handle, keys[index], buf, &len);
                if (err == ESP_ERR_FLASH_OP_FAIL) {
                    return err;
                }
                if (!written[index]) {
                    REQUIRE(err == ESP_ERR_NVS_NOT_FOUND);
                } else {
                    REQUIRE(err == ESP_OK);
                    REQUIRE(strncmp(buf, reinterpret_cast<const char *>(values[index]), strBufLen) == 0);
                }
                break;
            }

            case nvs::ItemType::BLOB: {
                uint32_t blobBufLen = 0;
                if (strncmp(keys[index], "singlepage", sizeof("singlepage")) == 0) {
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
                } else {
                    REQUIRE(err == ESP_OK);
                    REQUIRE(memcmp(buf, reinterpret_cast<const uint8_t *>(values[index]), blobBufLen) == 0);
                }
                break;
            }


            default:
                assert(0);
            }
            return ESP_OK;
        };

        auto randomWrite = [&](size_t index) -> esp_err_t {
            switch (types[index])
            {
            case nvs::ItemType::I32: {
                int32_t val = static_cast<int32_t>(gen());

                auto err = nvs_set_i32(handle, keys[index], val);
                if (err == ESP_ERR_FLASH_OP_FAIL) {
                    return err;
                }
                if (err == ESP_ERR_NVS_REMOVE_FAILED) {
                    written[index] = true;
                    *reinterpret_cast<int32_t *>(values[index]) = val;
                    return ESP_ERR_FLASH_OP_FAIL;
                }
                REQUIRE(err == ESP_OK);
                written[index] = true;
                *reinterpret_cast<int32_t *>(values[index]) = val;
                break;
            }

            case nvs::ItemType::U64: {
                uint64_t val = static_cast<uint64_t>(gen());

                auto err = nvs_set_u64(handle, keys[index], val);
                if (err == ESP_ERR_FLASH_OP_FAIL) {
                    return err;
                }
                if (err == ESP_ERR_NVS_REMOVE_FAILED) {
                    written[index] = true;
                    *reinterpret_cast<uint64_t *>(values[index]) = val;
                    return ESP_ERR_FLASH_OP_FAIL;
                }
                REQUIRE(err == ESP_OK);
                written[index] = true;
                *reinterpret_cast<uint64_t *>(values[index]) = val;
                break;
            }

            case nvs::ItemType::SZ: {
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
                    strncpy(reinterpret_cast<char *>(values[index]), buf, strBufLen);
                    return ESP_ERR_FLASH_OP_FAIL;
                }
                REQUIRE(err == ESP_OK);
                written[index] = true;
                strncpy(reinterpret_cast<char *>(values[index]), buf, strBufLen);
                break;
            }

            case nvs::ItemType::BLOB: {
                uint32_t blobBufLen = 0;
                if (strncmp(keys[index], "singlepage", sizeof("singlepage")) == 0) {
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
                    memcpy(reinterpret_cast<uint8_t *>(values[index]), buf, blobBufLen);
                    return ESP_ERR_FLASH_OP_FAIL;
                }
                REQUIRE(err == ESP_OK);
                written[index] = true;
                memcpy(reinterpret_cast<char *>(values[index]), buf, blobBufLen);
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

    esp_err_t handleExternalWriteAtIndex(uint8_t index, const void *value, const size_t len )
    {
        if (index == 9) { /* This is only done for small-page blobs for now*/
            if (len > smallBlobLen) {
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

    PartitionEmulationFixture f(0, 10);
    TEMPORARILY_DISABLED(f.emu.randomize(seed);)
    TEMPORARILY_DISABLED(f.emu.clearStats();)

    const uint32_t NVS_FLASH_SECTOR = 2;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 8;
    TEMPORARILY_DISABLED(f.emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);)

    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(),
                NVS_FLASH_SECTOR,
                NVS_FLASH_SECTOR_COUNT_MIN));

    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open("namespace1", NVS_READWRITE, &handle));
    RandomTest test;
    size_t count = 1000;
    CHECK(test.doRandomThings(handle, gen, count) == ESP_OK);

    //s_perf << "Monkey test: nErase=" << f.emu.getEraseOps() << " nWrite=" << f.emu.getWriteOps() << std::endl;

    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
}

TEST_CASE("test for memory leaks in open/set", "[leaks]")
{
    PartitionEmulationFixture f(0, 10);
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    TEMPORARILY_DISABLED(f.emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);)
    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(),
                NVS_FLASH_SECTOR,
                NVS_FLASH_SECTOR_COUNT_MIN));

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

TEST_CASE("read/write failure (TW8406)", "[nvs]")
{
    PartitionEmulationFixture f(0, 3);
    nvs::NVSPartitionManager::get_instance()->init_custom(f.part(), 0, 3);
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
    const size_t blob_size = nvs::Page::CHUNK_MAX_SIZE;
    uint8_t blob[blob_size] = {0};
    PartitionEmulationFixture f(0, 8);
    TEST_ESP_OK( nvs::NVSPartitionManager::get_instance()->init_custom(f.part(), 0, 5) );
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
    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
    // first two pages are now full, third one is writable, last two are empty
    // init should fail
    TEST_ESP_ERR( nvs::NVSPartitionManager::get_instance()->init_custom(f.part(), 0, 3),
                  ESP_ERR_NVS_NO_FREE_PAGES );

    // in case this test fails, to not affect other tests
    nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME);
}

TEST_CASE("nvs page selection takes into account free entries also not just erased entries", "[nvs]")
{
    const size_t blob_size = nvs::Page::CHUNK_MAX_SIZE / 2;
    uint8_t blob[blob_size] = {0};
    PartitionEmulationFixture f(0, 3);
    TEST_ESP_OK( nvs::NVSPartitionManager::get_instance()->init_custom(f.part(), 0, 3) );
    nvs_handle_t handle;
    TEST_ESP_OK( nvs_open("test", NVS_READWRITE, &handle) );
    // Fill first page
    TEST_ESP_OK( nvs_set_blob(handle, "1a", blob, blob_size / 3) );
    TEST_ESP_OK( nvs_set_blob(handle, "1b", blob, blob_size) );
    // Fill second page
    TEST_ESP_OK( nvs_set_blob(handle, "2a", blob, blob_size) );
    TEST_ESP_OK( nvs_set_blob(handle, "2b", blob, blob_size) );

    // The item below should be able to fit the first page.
    TEST_ESP_OK( nvs_set_blob(handle, "3a", blob, 4) );
    TEST_ESP_OK( nvs_commit(handle) );
    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(f.part()->get_partition_name()));
}

TEST_CASE("calculate used and free space", "[nvs]")
{
    PartitionEmulationFixture f(0, 6);
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
    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(), 0, 6));

    TEST_ESP_ERR(nvs_get_used_entry_count(handle, &h_count_entries), ESP_ERR_NVS_INVALID_HANDLE);
    CHECK(h_count_entries == 0);

    nvs::Page p;
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

    TEST_ESP_OK(nvs_flash_deinit_partition(f.part()->get_partition_name()));
}

TEST_CASE("Multi-page blobs are supported", "[nvs]")
{
    const size_t blob_size = nvs::Page::CHUNK_MAX_SIZE * 2;
    uint8_t blob[blob_size] = {0};
    PartitionEmulationFixture f(0, 5);
    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(), 0, 5));
    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open("test", NVS_READWRITE, &handle));
    TEST_ESP_OK(nvs_set_blob(handle, "abc", blob, blob_size));
    TEST_ESP_OK(nvs_commit(handle));
    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(f.part()->get_partition_name()));
}

TEST_CASE("Failures are handled while storing multi-page blobs", "[nvs]")
{
    const size_t blob_size = nvs::Page::CHUNK_MAX_SIZE * 7;
    uint8_t blob[blob_size] = {0};
    PartitionEmulationFixture f(0, 5);
    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(), 0, 5));
    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open("test", NVS_READWRITE, &handle));
    TEST_ESP_ERR(nvs_set_blob(handle, "abc", blob, blob_size), ESP_ERR_NVS_VALUE_TOO_LONG);
    TEST_ESP_OK(nvs_set_blob(handle, "abc", blob, nvs::Page::CHUNK_MAX_SIZE * 2));
    TEST_ESP_OK(nvs_commit(handle));
    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(f.part()->get_partition_name()));
}

TEST_CASE("Reading multi-page blobs", "[nvs]")
{
    const size_t blob_size = nvs::Page::CHUNK_MAX_SIZE * 3;
    uint8_t blob[blob_size];
    uint8_t blob_read[blob_size];
    size_t read_size = blob_size;
    PartitionEmulationFixture f(0, 5);
    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(), 0, 5));
    nvs_handle_t handle;
    memset(blob, 0x11, blob_size);
    memset(blob_read, 0xee, blob_size);
    TEST_ESP_OK(nvs_open("readTest", NVS_READWRITE, &handle));
    TEST_ESP_OK(nvs_set_blob(handle, "abc", blob, blob_size));
    TEST_ESP_OK(nvs_get_blob(handle, "abc", blob_read, &read_size));
    CHECK(memcmp(blob, blob_read, blob_size) == 0);
    TEST_ESP_OK(nvs_commit(handle));
    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(f.part()->get_partition_name()));
}

TEST_CASE("Modification of values for Multi-page blobs are supported", "[nvs]")
{
    const size_t blob_size = nvs::Page::CHUNK_MAX_SIZE * 2;
    uint8_t blob[blob_size] = {0};
    uint8_t blob_read[blob_size] = {0xfe};;
    uint8_t blob2[blob_size] = {0x11};
    uint8_t blob3[blob_size] = {0x22};
    uint8_t blob4[blob_size] = { 0x33};
    size_t read_size = blob_size;
    PartitionEmulationFixture f(0, 6);
    TEST_ESP_OK( nvs::NVSPartitionManager::get_instance()->init_custom(f.part(), 0, 6) );
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

    TEST_ESP_OK(nvs_flash_deinit_partition(f.part()->get_partition_name()));
}

TEST_CASE("Modification from single page blob to multi-page", "[nvs]")
{
    const size_t blob_size = nvs::Page::CHUNK_MAX_SIZE * 3;
    uint8_t blob[blob_size] = {0};
    uint8_t blob_read[blob_size] = {0xff};
    size_t read_size = blob_size;
    PartitionEmulationFixture f(0, 5);
    TEST_ESP_OK( nvs::NVSPartitionManager::get_instance()->init_custom(f.part(), 0, 5) );
    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open("Test", NVS_READWRITE, &handle) );
    TEST_ESP_OK(nvs_set_blob(handle, "abc", blob, nvs::Page::CHUNK_MAX_SIZE / 2));
    TEST_ESP_OK(nvs_set_blob(handle, "abc", blob, blob_size));
    TEST_ESP_OK(nvs_get_blob(handle, "abc", blob_read, &read_size));
    CHECK(memcmp(blob, blob_read, blob_size) == 0);
    TEST_ESP_OK(nvs_commit(handle) );
    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(f.part()->get_partition_name()));
}

TEST_CASE("Modification from  multi-page to single page", "[nvs]")
{
    const size_t blob_size = nvs::Page::CHUNK_MAX_SIZE * 3;
    uint8_t blob[blob_size] = {0};
    uint8_t blob_read[blob_size] = {0xff};
    size_t read_size = blob_size;
    PartitionEmulationFixture f(0, 5);
    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(), 0, 5) );
    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open("Test", NVS_READWRITE, &handle) );
    TEST_ESP_OK(nvs_set_blob(handle, "abc", blob, blob_size));
    TEST_ESP_OK(nvs_set_blob(handle, "abc", blob, nvs::Page::CHUNK_MAX_SIZE / 2));
    TEST_ESP_OK(nvs_set_blob(handle, "abc2", blob, blob_size));
    TEST_ESP_OK(nvs_get_blob(handle, "abc", blob_read, &read_size));
    CHECK(memcmp(blob, blob_read, nvs::Page::CHUNK_MAX_SIZE) == 0);
    TEST_ESP_OK(nvs_commit(handle) );
    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(f.part()->get_partition_name()));
}

TEST_CASE("Multi-page blob erased using nvs_erase_key should not be found when probed for just length", "[nvs]")
{
    const size_t blob_size = nvs::Page::CHUNK_MAX_SIZE * 3;
    uint8_t blob[blob_size] = {0};
    size_t read_size = blob_size;
    PartitionEmulationFixture f(0, 5);
    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(), 0, 5));
    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open("Test", NVS_READWRITE, &handle));
    TEST_ESP_OK(nvs_set_blob(handle, "abc", blob, blob_size));
    TEST_ESP_OK(nvs_erase_key(handle, "abc"));
    TEST_ESP_ERR(nvs_get_blob(handle, "abc", NULL, &read_size), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_OK(nvs_commit(handle));
    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(f.part()->get_partition_name()));
}


TEST_CASE("Check that orphaned blobs are erased during init", "[nvs]")
{
    const size_t blob_size = nvs::Page::CHUNK_MAX_SIZE * 3 ;
    uint8_t blob[blob_size] = {0x11};
    uint8_t blob2[blob_size] = {0x22};
    uint8_t blob3[blob_size] = {0x33};
    PartitionEmulationFixture f(0, 5);
    nvs::Storage storage(f.part());

    TEST_ESP_OK(storage.init(0, 5));

    TEST_ESP_OK(storage.writeItem(1, nvs::ItemType::BLOB, "key", blob, sizeof(blob)));


    TEST_ESP_OK(storage.init(0, 5));
    /* Check that multi-page item is still available.**/
    TEST_ESP_OK(storage.readItem(1, nvs::ItemType::BLOB, "key", blob, sizeof(blob)));

    TEST_ESP_ERR(storage.writeItem(1, nvs::ItemType::BLOB, "key2", blob, sizeof(blob)), ESP_ERR_NVS_NOT_ENOUGH_SPACE);

    nvs::Page p;
    p.load(f.part(), 3); // This is where index will be placed.
    p.erase();

    TEST_ESP_OK(storage.init(0, 5));

    TEST_ESP_ERR(storage.readItem(1, nvs::ItemType::BLOB, "key", blob, sizeof(blob)), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_OK(storage.writeItem(1, nvs::ItemType::BLOB, "key3", blob, sizeof(blob)));
}

TEST_CASE("nvs blob fragmentation test", "[nvs]")
{
    PartitionEmulationFixture f(0, 4);
    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(), 0, 4) );
    const size_t BLOB_SIZE = 3500;
    uint8_t *blob = (uint8_t *) malloc(BLOB_SIZE);
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

    TEST_ESP_OK(nvs_flash_deinit_partition(f.part()->get_partition_name()));
}

TEST_CASE("nvs code handles errors properly when partition is near to full", "[nvs]")
{
    const size_t blob_size = nvs::Page::CHUNK_MAX_SIZE * 0.3 ;
    uint8_t blob[blob_size] = {0x11};
    PartitionEmulationFixture f(0, 5);
    nvs::Storage storage(f.part());
    char nvs_key[16] = "";

    TEST_ESP_OK(storage.init(0, 5));

    /* Four pages should fit roughly 12 blobs*/
    for (uint8_t count = 1; count <= 12; count++) {
        sprintf(nvs_key, "key:%u", count);
        TEST_ESP_OK(storage.writeItem(1, nvs::ItemType::BLOB, nvs_key, blob, sizeof(blob)));
    }

    for (uint8_t count = 13; count <= 20; count++) {
        sprintf(nvs_key, "key:%u", count);
        TEST_ESP_ERR(storage.writeItem(1, nvs::ItemType::BLOB, nvs_key, blob, sizeof(blob)), ESP_ERR_NVS_NOT_ENOUGH_SPACE);
    }
}

TEST_CASE("Check for nvs version incompatibility", "[nvs]")
{
    PartitionEmulationFixture f(0, 3);

    int32_t val1 = 0x12345678;
    nvs::Page p;
    p.load(f.part(), 0);
    TEST_ESP_OK(p.setVersion(nvs::Page::NVS_VERSION - 1));
    TEST_ESP_OK(p.writeItem(1, nvs::ItemType::I32, "foo", &val1, sizeof(val1)));

    TEST_ESP_ERR(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(), 0, 3),
                 ESP_ERR_NVS_NEW_VERSION_FOUND);

    // if something went wrong, clean up
    nvs_flash_deinit_partition(f.part()->get_partition_name());
}

// TODO: leaks memory
TEST_CASE("monkey test with old-format blob present", "[nvs][monkey]")
{
    std::random_device rd;
    std::mt19937 gen(rd());
    uint32_t seed = 3;
    gen.seed(seed);

    PartitionEmulationFixture f(0, 10);
    TEMPORARILY_DISABLED(f.emu.randomize(seed);)
    TEMPORARILY_DISABLED(f.emu.clearStats();)

    const uint32_t NVS_FLASH_SECTOR = 2;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 8;
    static const size_t smallBlobLen = nvs::Page::CHUNK_MAX_SIZE / 3;

    TEMPORARILY_DISABLED(f.emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);)

    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(),
                NVS_FLASH_SECTOR,
                NVS_FLASH_SECTOR_COUNT_MIN));

    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open("namespace1", NVS_READWRITE, &handle));
    RandomTest test;

    for ( uint8_t it = 0; it < 10; it++) {
        size_t count = 200;

        /* Erase index and chunks for the blob with "singlepage" key */
        for (uint8_t num = NVS_FLASH_SECTOR; num < NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN; num++) {
            nvs::Page p;
            p.load(f.part(), num);
            p.eraseItem(1, nvs::ItemType::BLOB, "singlepage", nvs::Item::CHUNK_ANY, nvs::VerOffset::VER_ANY);
            p.eraseItem(1, nvs::ItemType::BLOB_IDX, "singlepage", nvs::Item::CHUNK_ANY, nvs::VerOffset::VER_ANY);
            p.eraseItem(1, nvs::ItemType::BLOB_DATA, "singlepage", nvs::Item::CHUNK_ANY, nvs::VerOffset::VER_ANY);
        }

        /* Now write "singlepage" blob in old format*/
        for (uint8_t num = NVS_FLASH_SECTOR; num < NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN; num++) {
            nvs::Page p;
            p.load(f.part(), num);
            if (p.state() == nvs::Page::PageState::ACTIVE) {
                uint8_t buf[smallBlobLen];
                size_t blobLen = gen() % smallBlobLen;

                if (blobLen > p.getVarDataTailroom()) {
                    blobLen = p.getVarDataTailroom();
                }

                std::generate_n(buf, blobLen, [&]() -> uint8_t {
                    return static_cast<uint8_t>(gen() % 256);
                });

                TEST_ESP_OK(p.writeItem(1, nvs::ItemType::BLOB, "singlepage", buf, blobLen, nvs::Item::CHUNK_ANY));
                TEST_ESP_OK(p.findItem(1, nvs::ItemType::BLOB, "singlepage"));
                test.handleExternalWriteAtIndex(9, buf, blobLen); // This assumes "singlepage" is always at index 9

                break;
            }
        }

        TEST_ESP_OK(nvs_flash_deinit_partition(f.part()->get_partition_name()));
        /* Initialize again */
        TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(),
                    NVS_FLASH_SECTOR,
                    NVS_FLASH_SECTOR_COUNT_MIN));
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
            nvs::Page p;
            p.load(f.part(), num);
            if (!oldVerPresent && p.findItem(1, nvs::ItemType::BLOB, "singlepage", nvs::Item::CHUNK_ANY, nvs::VerOffset::VER_ANY) == ESP_OK) {
                oldVerPresent = true;
            }

            if (!newVerPresent && p.findItem(1, nvs::ItemType::BLOB_IDX, "singlepage", nvs::Item::CHUNK_ANY, nvs::VerOffset::VER_ANY) == ESP_OK) {
                newVerPresent = true;
            }
        }
        CHECK(oldVerPresent != newVerPresent);
    }

    TEST_ESP_OK(nvs_flash_deinit_partition(f.part()->get_partition_name()));
    //s_perf << "Monkey test: nErase=" << f.emu.getEraseOps() << " nWrite=" << f.emu.getWriteOps() << std::endl;
}

TEST_CASE("Recovery from power-off during modification of blob present in old-format (same page)", "[nvs]")
{
    std::random_device rd;
    std::mt19937 gen(rd());
    uint32_t seed = 3;
    gen.seed(seed);

    PartitionEmulationFixture f(0, 3);
    TEMPORARILY_DISABLED(f.emu.clearStats();)

    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(), 0, 3));

    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open("namespace1", NVS_READWRITE, &handle));

    uint8_t hexdata[] = {0x01, 0x02, 0x03, 0xab, 0xcd, 0xef};
    uint8_t hexdata_old[] = {0x11, 0x12, 0x13, 0xbb, 0xcc, 0xee};
    size_t buflen = sizeof(hexdata);
    uint8_t buf[nvs::Page::CHUNK_MAX_SIZE];

    /* Power-off when blob was being written on the same page where its old version in old format
     * was present*/
    nvs::Page p;
    p.load(f.part(), 0);
    /* Write blob in old-format*/
    TEST_ESP_OK(p.writeItem(1, nvs::ItemType::BLOB, "singlepage", hexdata_old, sizeof(hexdata_old)));

    /* Write blob in new format*/
    TEST_ESP_OK(p.writeItem(1, nvs::ItemType::BLOB_DATA, "singlepage", hexdata, sizeof(hexdata), 0));
    /* All pages are stored. Now store the index.*/
    nvs::Item item;
    item.blobIndex.dataSize = sizeof(hexdata);
    item.blobIndex.chunkCount = 1;
    item.blobIndex.chunkStart = nvs::VerOffset::VER_0_OFFSET;

    TEST_ESP_OK(p.writeItem(1, nvs::ItemType::BLOB_IDX, "singlepage", item.data, sizeof(item.data)));

    TEST_ESP_OK(p.findItem(1, nvs::ItemType::BLOB, "singlepage"));

    TEST_ESP_OK(nvs_flash_deinit_partition(f.part()->get_partition_name()));
    /* Initialize again */
    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(), 0, 3));
    TEST_ESP_OK(nvs_open("namespace1", NVS_READWRITE, &handle));

    TEST_ESP_OK( nvs_get_blob(handle, "singlepage", buf, &buflen));
    CHECK(memcmp(buf, hexdata, buflen) == 0);

    nvs::Page p2;
    p2.load(f.part(), 0);
    TEST_ESP_ERR(p2.findItem(1, nvs::ItemType::BLOB, "singlepage"), ESP_ERR_NVS_TYPE_MISMATCH);

    TEST_ESP_OK(nvs_flash_deinit_partition(f.part()->get_partition_name()));
}

TEST_CASE("Recovery from power-off during modification of blob present in old-format (different page)", "[nvs]")
{
    std::random_device rd;
    std::mt19937 gen(rd());
    uint32_t seed = 3;
    gen.seed(seed);

    PartitionEmulationFixture f(0, 3);
    TEMPORARILY_DISABLED(f.emu.clearStats();)

    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(), 0, 3));

    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open("namespace1", NVS_READWRITE, &handle));

    uint8_t hexdata[] = {0x01, 0x02, 0x03, 0xab, 0xcd, 0xef};
    uint8_t hexdata_old[] = {0x11, 0x12, 0x13, 0xbb, 0xcc, 0xee};
    size_t buflen = sizeof(hexdata);
    uint8_t buf[nvs::Page::CHUNK_MAX_SIZE];


    /* Power-off when blob was being written on the different page where its old version in old format
     * was present*/
    nvs::Page p;
    p.load(f.part(), 0);
    /* Write blob in old-format*/
    TEST_ESP_OK(p.writeItem(1, nvs::ItemType::BLOB, "singlepage", hexdata_old, sizeof(hexdata_old)));

    /* Write blob in new format*/
    TEST_ESP_OK(p.writeItem(1, nvs::ItemType::BLOB_DATA, "singlepage", hexdata, sizeof(hexdata), 0));
    /* All pages are stored. Now store the index.*/
    nvs::Item item;
    item.blobIndex.dataSize = sizeof(hexdata);
    item.blobIndex.chunkCount = 1;
    item.blobIndex.chunkStart = nvs::VerOffset::VER_0_OFFSET;
    p.markFull();
    nvs::Page p2;
    p2.load(f.part(), 1);
    p2.setSeqNumber(1);

    TEST_ESP_OK(p2.writeItem(1, nvs::ItemType::BLOB_IDX, "singlepage", item.data, sizeof(item.data)));

    TEST_ESP_OK(p.findItem(1, nvs::ItemType::BLOB, "singlepage"));

    TEST_ESP_OK(nvs_flash_deinit_partition(f.part()->get_partition_name()));
    /* Initialize again */
    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(f.part(), 0, 3));
    TEST_ESP_OK(nvs_open("namespace1", NVS_READWRITE, &handle));

    TEST_ESP_OK( nvs_get_blob(handle, "singlepage", buf, &buflen));
    CHECK(memcmp(buf, hexdata, buflen) == 0);

    nvs::Page p3;
    p3.load(f.part(), 0);
    TEST_ESP_ERR(p3.findItem(1, nvs::ItemType::BLOB, "singlepage"), ESP_ERR_NVS_NOT_FOUND);

    TEST_ESP_OK(nvs_flash_deinit_partition(f.part()->get_partition_name()));
}
