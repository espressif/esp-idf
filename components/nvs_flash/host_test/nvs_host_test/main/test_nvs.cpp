/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <catch2/catch_test_macros.hpp>
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
#include "spi_flash_mmap.h"

using namespace std;

#define TEST_DEFAULT_PARTITION_NAME "nvs"               // Default partition name used in the tests - 10 sectors
#define TEST_SECONDARY_PARTITION_NAME "nvs_sec"         // Secondary partition name used in the tests - 10 sectors
#define TEST_3SEC_PARTITION_NAME "nvs_3sec"             // Partition used in the space constrained tests - 3 sectors

#define TEST_ESP_ERR(rc, res) CHECK((rc) == (res))
#define TEST_ESP_OK(rc) CHECK((rc) == ESP_OK)

#define WD_PREFIX "./components/nvs_flash/host_test/nvs_host_test/" // path from ci cwd to the location of host test

#define TEST_DEFAULT_PURGE_AFTER_ERASE true             // erase with purge after erase

#if defined(SEGGER_H) && defined(GLOBAL_H)
NVS_GUARD_SYSVIEW_MACRO_EXPANSION_PUSH();
#undef U8
#undef I8
#undef U16
#undef I16
#undef U32
#undef I32
#undef U64
#undef I64
#endif

stringstream s_perf;

TEST_CASE("crc32 behaves as expected", "[nvs]")
{
    // TC verifies that crc32 is calculated correctly for Item.
    // It verifies that:
    // - crc32 is calculated correctly for Item with all fields set to 0xff
    // - changing any field changes the crc32
    // - changing nsIndex, datatype, key or chunkIndex changes the crc32
    // - changing data changes the crc32

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

    item2 = item1;
    fill_n(item2.data, sizeof(item2.data), 0xab);
    CHECK(crc32_1 != item2.calculateCrc32());
}

TEST_CASE("Page starting with empty flash is in uninitialized state", "[nvs]")
{
    // TC verifies that Page starts in INVALID state and after loading it from an empty partition, it becomes UNINITIALIZED.

    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);

    nvs::Page page;
    CHECK(page.state() == nvs::Page::PageState::INVALID);
    TEST_ESP_OK(page.load(&h, 0));
    CHECK(page.state() == nvs::Page::PageState::UNINITIALIZED);
}

TEST_CASE("Page can distinguish namespaces", "[nvs]")
{
    // TC verifies that Page.writeItem and readItem respects namespace parameter.

    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);

    nvs::Page page;
    TEST_ESP_OK(page.load(&h, 0));
    int32_t val1 = 0x12345678;
    TEST_ESP_OK(page.writeItem(1, nvs::ItemType::I32, "intval1", &val1, sizeof(val1)));
    int32_t val2 = 0x23456789;
    TEST_ESP_OK(page.writeItem(2, nvs::ItemType::I32, "intval1", &val2, sizeof(val2)));

    int32_t readVal;
    TEST_ESP_OK(page.readItem(2, nvs::ItemType::I32, "intval1", &readVal, sizeof(readVal)));
    CHECK(readVal == val2);
}

TEST_CASE("Page reading with different type causes type mismatch error", "[nvs]")
{
    // TC verifies that Page.readItem returns ESP_ERR_NVS_TYPE_MISMATCH when reading with different type than written.

    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);

    nvs::Page page;
    TEST_ESP_OK(page.load(&h, 0));
    int32_t val = 0x12345678;
    TEST_ESP_OK(page.writeItem(1, nvs::ItemType::I32, "intval1", &val, sizeof(val)));
    CHECK(page.readItem(1, nvs::ItemType::U32, "intval1", &val, sizeof(val)) == ESP_ERR_NVS_TYPE_MISMATCH);
}

TEST_CASE("Page when erased, it's state becomes UNINITIALIZED", "[nvs]")
{
    // TC verifies that Page.erase() sets the state to UNINITIALIZED.

    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);

    nvs::Page page;
    TEST_ESP_OK(page.load(&h, 0));
    int32_t val = 0x12345678;
    TEST_ESP_OK(page.writeItem(1, nvs::ItemType::I32, "intval1", &val, sizeof(val)));
    TEST_ESP_OK(page.erase());
    CHECK(page.state() == nvs::Page::PageState::UNINITIALIZED);
}

TEST_CASE("Page when writing and erasing, used/erased counts are updated correctly", "[nvs]")
{
    // TC verifies that Page.writeItem and eraseItem update used/erased entry counts correctly.
    // The test verifies following:
    // - writing an item increases used entry count
    // - overwriting and item increases used entry count
    // - erasing an item increases erased entry count and decreases used entry count
    // - completely erasinf the page sets used entry count to 1 and erased entry count to Page::ENTRY_COUNT - 1
    // The remaining, not deleted entry is the namespace entry.

    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);
    const bool purgeAfterErase = TEST_DEFAULT_PURGE_AFTER_ERASE;

    nvs::Page page;
    TEST_ESP_OK(page.load(&h, 0));
    CHECK(page.getUsedEntryCount() == 0);
    CHECK(page.getErasedEntryCount() == 0);
    uint32_t foo1 = 0;
    TEST_ESP_OK(page.writeItem(1, "foo1", foo1));
    CHECK(page.getUsedEntryCount() == 1);
    TEST_ESP_OK(page.writeItem(2, "foo1", foo1));
    CHECK(page.getUsedEntryCount() == 2);
    TEST_ESP_OK(page.eraseItem<uint32_t>(2, "foo1", purgeAfterErase));
    CHECK(page.getUsedEntryCount() == 1);
    CHECK(page.getErasedEntryCount() == 1);
    for (size_t i = 0; i < nvs::Page::ENTRY_COUNT - 2; ++i) {
        char name[16];
        snprintf(name, sizeof(name), "i%ld", (long int)i);
        TEST_ESP_OK(page.writeItem(1, name, i));
    }
    CHECK(page.getUsedEntryCount() == nvs::Page::ENTRY_COUNT - 1);
    CHECK(page.getErasedEntryCount() == 1);
    for (size_t i = 0; i < nvs::Page::ENTRY_COUNT - 2; ++i) {
        char name[16];
        snprintf(name, sizeof(name), "i%ld", (long int)i);
        TEST_ESP_OK(page.eraseItem(1, nvs::itemTypeOf<size_t>(), name, purgeAfterErase));
    }
    CHECK(page.getUsedEntryCount() == 1);
    CHECK(page.getErasedEntryCount() == nvs::Page::ENTRY_COUNT - 1);
}

TEST_CASE("Page when page is full, adding an element fails", "[nvs]")
{
    // TC verifies that Page.writeItem returns ESP_ERR_NVS_PAGE_FULL when the page is full
    // and writeItem is called.

    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);

    nvs::Page page;
    TEST_ESP_OK(page.load(&h, 0));
    for (size_t i = 0; i < nvs::Page::ENTRY_COUNT; ++i) {
        char name[16];
        snprintf(name, sizeof(name), "i%ld", (long int)i);
        TEST_ESP_OK(page.writeItem(1, name, i));
    }
    CHECK(page.writeItem(1, "foo", 64UL) == ESP_ERR_NVS_PAGE_FULL);
}

TEST_CASE("Page maintains its seq number", "[nvs]")
{
    // TC verifies that Page.setSeqNumber and getSeqNumber works as expected.
    // The test verifies following:
    // - after loading the page, seq number is 0
    // - after setting the seq number, it is persisted
    // - after loading the page again, the seq number is the same as set before

    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);
    {
        nvs::Page page;
        TEST_ESP_OK(page.load(&h, 0));
        TEST_ESP_OK(page.setSeqNumber(123));
        int32_t val = 42;
        TEST_ESP_OK(page.writeItem(1, nvs::ItemType::I32, "dummy", &val, sizeof(val)));
    }
    {
        nvs::Page page;
        TEST_ESP_OK(page.load(&h, 0));
        uint32_t seqno;
        TEST_ESP_OK(page.getSeqNumber(seqno));
        CHECK(seqno == 123);
    }
}

TEST_CASE("Page can write and read variable length data", "[nvs]")
{
    // TC verifies that Page.writeItem and readItem works with variable length data.
    // The test verifies following:
    // - writing a variable length data works if performed in the series of fixed data writes
    // - reading a variable length data after some fixed length data writes works

    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);

    nvs::Page page;
    TEST_ESP_OK(page.load(&h, 0));
    const char str[] = "foobar1234foobar1234foobar1234foobar1234foobar1234foobar1234foobar1234foobar1234";
    size_t len = strlen(str);
    TEST_ESP_OK(page.writeItem(1, "stuff1", 42));
    TEST_ESP_OK(page.writeItem(1, "stuff2", 1));
    TEST_ESP_OK(page.writeItem(1, nvs::ItemType::SZ, "foobaar", str, len + 1));
    TEST_ESP_OK(page.writeItem(1, "stuff3", 2));
    TEST_ESP_OK(page.writeItem(1, nvs::ItemType::BLOB, "baz", str, len));
    TEST_ESP_OK(page.writeItem(1, "stuff4", 0x7abbccdd));

    char buf[sizeof(str) + 16];
    int32_t value;
    TEST_ESP_OK(page.readItem(1, "stuff1", value));
    CHECK(value == 42);
    TEST_ESP_OK(page.readItem(1, "stuff2", value));
    CHECK(value == 1);
    TEST_ESP_OK(page.readItem(1, "stuff3", value));
    CHECK(value == 2);
    TEST_ESP_OK(page.readItem(1, "stuff4", value));
    CHECK(value == 0x7abbccdd);

    fill_n(buf, sizeof(buf), 0xff);
    TEST_ESP_OK(page.readItem(1, nvs::ItemType::SZ, "foobaar", buf, sizeof(buf)));
    CHECK(memcmp(buf, str, strlen(str) + 1) == 0);

    fill_n(buf, sizeof(buf), 0xff);
    TEST_ESP_OK(page.readItem(1, nvs::ItemType::BLOB, "baz", buf, sizeof(buf)));
    CHECK(memcmp(buf, str, strlen(str)) == 0);
}

TEST_CASE("Page different key names are distinguished even if the pointer is the same", "[nvs]")
{
    // TC verifies that Page.writeItem and readItem uses content of the buffer as key name
    // and not the pointer to the buffer.
    // The test verifies following:
    // - writing two items with the same pointer to buffer containing always different key creates 2 different entries
    // - reading the items with the same pointer to buffer containing always different key returns the correct values

    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);

    nvs::Page page;
    TEST_ESP_OK(page.load(&h, 0));
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
    // TC verifies that Page.writeItem returns ESP_ERR_NVS_KEY_TOO_LONG when the key length exceed the limit.
    // The limit is 15 characters plus null terminator, so 16-character key should fail.

    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);

    nvs::Page page;
    TEST_ESP_OK(page.load(&h, 0));
    // 16-character key fails
    TEST_ESP_ERR(page.writeItem(1, "0123456789123456", 1), ESP_ERR_NVS_KEY_TOO_LONG);
    // 15-character key is okay
    TEST_ESP_OK(page.writeItem(1, "012345678912345", 1));
}

TEST_CASE("Page validates blob size", "[nvs]")
{
    // TC verifies that Page.writeItem returns ESP_ERR_NVS_VALUE_TOO_LONG when the blob size exceeds the limit.
    // The technical limit at the Page level is the actual number of available entries in the page - 1 (for BLOB metadata)
    // The enforced limit assuming the complete page is available for BLOBs is 4096 bytes.
    // In this test, both limits are actually the same (4096 bytes).
    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);

    nvs::Page page;
    TEST_ESP_OK(page.load(&h, 0));

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
    // TC verifies that HashList is cleaned up as soon as items are erased.
    // The test verifies following:
    // - adding items increases the block count
    // - removing items decreases the block count
    // - adding items again after removing them increases the block count again
    // - removing items in the same order works

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
    // TC verifies that PageManager can be initialized in empty flash.
    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);

    nvs::PageManager pm;
    TEST_ESP_OK(pm.load(&h, 0, h.get_sectors()));
}

TEST_CASE("PageManager adds page in the correct order", "[nvs]")
{
    // TC verifies that PageManager adds pages in the correct order.
    // The test verifies following:
    // - PageManager iterates through pages in the order of their seq number regardless of the order they were added

    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);
    const size_t pageCount = 10;

    // We have to make sure that the PageManager is initialized with the same number of pages as the partition has.
    CHECK(pageCount == h.get_sectors());

    // The page numbers are set in such a way that the seq numbers are not in order.
    uint32_t pageNo[pageCount] = { -1U, 50, 11, -1U, 23, 22, 24, 49, 31, 32};

    // Populate the storage with pages in random order.
    for (uint32_t i = 0; i < pageCount; ++i) {
        nvs::Page p;
        TEST_ESP_OK(p.load(&h, i));
        if (pageNo[i] != -1U) {
            p.setSeqNumber(pageNo[i]);
            p.writeItem(1, "foo", 10U);
        }
    }

    // Load the PageManager from the same partition and check that the pages
    // are iterated in the order of their seq numbers.
    nvs::PageManager pageManager;
    TEST_ESP_OK(pageManager.load(&h, 0, h.get_sectors()));

    uint32_t lastSeqNo = 0;
    for (auto it = std::begin(pageManager); it != std::end(pageManager); ++it) {
        uint32_t seqNo;
        TEST_ESP_OK(it->getSeqNumber(seqNo));
        CHECK(seqNo > lastSeqNo);
    }
}

TEST_CASE("can init storage in empty flash", "[nvs]")
{
    // TC verifies that Storage can be initialized in empty flash.
    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);

    nvs::Storage storage(&h);

    NVSPartitionTestHelper::clear_stats(); // Clear statistics before init

    TEST_ESP_OK(storage.init(0, h.get_sectors()));

    s_perf << "Time to init empty storage (" << h.get_sectors() << " sectors): " << NVSPartitionTestHelper::get_total_time() << " us" << std::endl;
}

TEST_CASE("storage doesn't add duplicates within one page", "[nvs]")
{
    // TC verifies that Storage.writeItem doesn't add duplicates within one page.
    // The test verifies following:
    // - writing the same item twice doesn't increase used entry count
    // - overwriting the same item sets the erased entry count to 1

    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);
    const bool purgeAfterErase = TEST_DEFAULT_PURGE_AFTER_ERASE;

    nvs::Storage storage(&h);

    TEST_ESP_OK(storage.init(0, h.get_sectors()));
    int bar = 0;
    TEST_ESP_OK(storage.writeItem(1, "bar", ++bar, purgeAfterErase));
    TEST_ESP_OK(storage.writeItem(1, "bar", ++bar, purgeAfterErase));

    nvs::Page page;

    // We are assuming that sector 0 is used for the first page.
    TEST_ESP_OK(page.load(&h, 0));
    CHECK(page.getUsedEntryCount() == 1);
    CHECK(page.getErasedEntryCount() == 1);
}

TEST_CASE("can write one item a thousand times", "[nvs]")
{
    // TC verifies that Storage.writeItem can write one item a thousand times.

    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);
    const bool purgeAfterErase = TEST_DEFAULT_PURGE_AFTER_ERASE;

    const size_t NO_WRITES = 1000;
    nvs::Storage storage(&h);

    TEST_ESP_OK(storage.init(0, h.get_sectors()));

    NVSPartitionTestHelper::clear_stats(); // Clear statistics before writing

    for (size_t i = 0; i < NO_WRITES; ++i) {
        TEST_ESP_OK(storage.writeItem(1, "i", static_cast<int>(i), purgeAfterErase));
    }
    s_perf << "Time to write one item a " << NO_WRITES << " times: " << NVSPartitionTestHelper::get_total_time() << " us (" << NVSPartitionTestHelper::get_erase_ops() << " " << NVSPartitionTestHelper::get_write_ops() << " " << NVSPartitionTestHelper::get_read_ops() << " " << NVSPartitionTestHelper::get_write_bytes() << " " << NVSPartitionTestHelper::get_read_bytes() << ")" << std::endl;
}

TEST_CASE("storage doesn't add duplicates within multiple pages", "[nvs]")
{
    // TC verifies that Storage.writeItem doesn't add duplicates within multiple pages.
    // The test writes the item "bar" to the first page of the partition,
    // Then it writes the item "foo" so many times that it doesn't fit into the first page.
    // At the end it writes the item "bar" again, this time to the second page.
    // The test verifies following:
    // - attempt to read the item "bar" from the first page returns ESP_ERR_NVS_NOT_FOUND

    // - reading the item "bar" from the second page works
    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);
    const bool purgeAfterErase = TEST_DEFAULT_PURGE_AFTER_ERASE;

    nvs::Storage storage(&h);

    TEST_ESP_OK(storage.init(0, h.get_sectors()));
    int bar = 0;
    TEST_ESP_OK(storage.writeItem(1, "bar", ++bar, purgeAfterErase));
    for (size_t i = 0; i < nvs::Page::ENTRY_COUNT; ++i) {
        TEST_ESP_OK(storage.writeItem(1, "foo", static_cast<int>(++bar), purgeAfterErase));
    }
    TEST_ESP_OK(storage.writeItem(1, "bar", ++bar, purgeAfterErase));
    nvs::Page page;
    TEST_ESP_OK(page.load(&h, 0));  // first page attempt
    CHECK(page.findItem(1, nvs::itemTypeOf<int>(), "bar") == ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_OK(page.load(&h, 1));  // second page attempt
    TEST_ESP_OK(page.findItem(1, nvs::itemTypeOf<int>(), "bar"));
}

TEST_CASE("storage can find items on second page if first is not fully written and has cached search data", "[nvs]")
{
    // TC verifies that Storage.findItem can find items on the second page if the first page is not fully written
    // The first page is occupied by the item "1" and "2" which occupy enough of the first page in order to not fit the third item "3".

    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);
    const bool purgeAfterErase = TEST_DEFAULT_PURGE_AFTER_ERASE;

    nvs::Storage storage(&h);
    TEST_ESP_OK(storage.init(0, h.get_sectors()));

    uint8_t bigdata[(nvs::Page::CHUNK_MAX_SIZE - nvs::Page::ENTRY_SIZE) / 2] = {0};
    // write one big chunk of data
    ESP_ERROR_CHECK(storage.writeItem(1, nvs::ItemType::BLOB, "1", bigdata, sizeof(bigdata), purgeAfterErase));
    // write another big chunk of data
    ESP_ERROR_CHECK(storage.writeItem(1, nvs::ItemType::BLOB, "2", bigdata, sizeof(bigdata), purgeAfterErase));

    // write third one; it will not fit into the first page
    ESP_ERROR_CHECK(storage.writeItem(1, nvs::ItemType::BLOB, "3", bigdata, sizeof(bigdata), purgeAfterErase));

    size_t size;
    ESP_ERROR_CHECK(storage.getItemDataSize(1, nvs::ItemType::BLOB, "1", size));
    CHECK(size == sizeof(bigdata));
    ESP_ERROR_CHECK(storage.getItemDataSize(1, nvs::ItemType::BLOB, "3", size));
    CHECK(size == sizeof(bigdata));
}

TEST_CASE("can write and read variable length data lots of times", "[nvs]")
{
    // TC verifies that Storage.writeItem and readItem works with large amounts
    // of variable length data interleaved with fixed length data.

    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);
    const bool purgeAfterErase = TEST_DEFAULT_PURGE_AFTER_ERASE;

    const size_t NO_WRITES = 1000;
    nvs::Storage storage(&h);

    TEST_ESP_OK(storage.init(0, h.get_sectors()));
    const char str[] = "foobar1234foobar1234foobar1234foobar1234foobar1234foobar1234foobar1234foobar1234";
    char buf[sizeof(str) + 16];
    size_t len = strlen(str);

    NVSPartitionTestHelper::clear_stats(); // Clear statistics before writing

    for (size_t i = 0; i < NO_WRITES; ++i) {
        CAPTURE(i);
        TEST_ESP_OK(storage.writeItem(1, nvs::ItemType::SZ, "foobaar", str, len + 1, purgeAfterErase));
        TEST_ESP_OK(storage.writeItem(1, "foo", static_cast<uint32_t>(i), purgeAfterErase));

        uint32_t value;
        TEST_ESP_OK(storage.readItem(1, "foo", value));
        CHECK(value == i);

        fill_n(buf, sizeof(buf), 0xff);
        TEST_ESP_OK(storage.readItem(1, nvs::ItemType::SZ, "foobaar", buf, sizeof(buf)));
        CHECK(memcmp(buf, str, strlen(str) + 1) == 0);
    }
    s_perf << "Time to write one string and one integer a " << NO_WRITES << " times: " << NVSPartitionTestHelper::get_total_time() << " us (" << NVSPartitionTestHelper::get_erase_ops() << " " << NVSPartitionTestHelper::get_write_ops() << " " << NVSPartitionTestHelper::get_read_ops() << " " << NVSPartitionTestHelper::get_write_bytes() << " " << NVSPartitionTestHelper::get_read_bytes() << ")" << std::endl;
}

TEST_CASE("can get length of variable length data", "[nvs]")
{
    // TC verifies that Storage.getItemDataSize works with variable length data.
    // The test verifies following:
    // - getting the size of a string and blob works correctly and independently between namespaces

    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);
    const bool purgeAfterErase = TEST_DEFAULT_PURGE_AFTER_ERASE;

    TEST_ESP_OK(NVSPartitionTestHelper::randomize_partition(TEST_DEFAULT_PARTITION_NAME, 200));

    nvs::Storage storage(&h);

    TEST_ESP_OK(storage.init(0, h.get_sectors()));

    const char str[] = "foobar1234foobar1234foobar1234foobar1234foobar1234foobar1234foobar1234foobar1234";
    size_t len = strlen(str);
    TEST_ESP_OK(storage.writeItem(1, nvs::ItemType::SZ, "foobaar", str, len + 1, purgeAfterErase));      // namespace 1
    size_t dataSize;
    TEST_ESP_OK(storage.getItemDataSize(1, nvs::ItemType::SZ, "foobaar", dataSize));    // namespace 1
    CHECK(dataSize == len + 1);

    TEST_ESP_OK(storage.writeItem(2, nvs::ItemType::BLOB, "foobaar", str, len, purgeAfterErase));        // namespace 2
    TEST_ESP_OK(storage.getItemDataSize(2, nvs::ItemType::BLOB, "foobaar", dataSize));  // namespace 2
    CHECK(dataSize == len);
}

TEST_CASE("can create namespaces", "[nvs]")
{
    // TC verifies that Storage.createOrOpenNamespace works as expected.
    // The test verifies following:
    // - creating a namespace fails if it doesn't exist and parameter allowing it's creation is false
    // - creating a namespace works if it doesn't exist and parameter allowing it's creation is true

    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);

    nvs::Storage storage(&h);

    TEST_ESP_OK(storage.init(0, h.get_sectors()));
    uint8_t nsi;
    CHECK(storage.createOrOpenNamespace("wifi", false, nsi) == ESP_ERR_NVS_NOT_FOUND);

    TEST_ESP_OK(storage.createOrOpenNamespace("wifi", true, nsi));
    nvs::Page page;
    TEST_ESP_OK(page.load(&h, 0));
    TEST_ESP_OK(page.findItem(nvs::Page::NS_INDEX, nvs::ItemType::U8, "wifi"));
}

TEST_CASE("storage may become full", "[nvs]")
{
    // TC verifies that Storage.writeItem returns ESP_ERR_NVS_NOT_ENOUGH_SPACE when the storage is full.
    // The test fills the storage with items until it becomes full.
    // The test verifies following:
    // - writing an item when the storage is not full works
    // - writing an item when the storage is full returns ESP_ERR_NVS_NOT_ENOUGH_SPACE

    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);
    const bool purgeAfterErase = TEST_DEFAULT_PURGE_AFTER_ERASE;

    nvs::Storage storage(&h);

    TEST_ESP_OK(storage.init(0, h.get_sectors()));

    // Calculate how many items can be written to the storage.
    // One page (sector) has to be reserved for space reclaim operations,
    size_t max_items = nvs::Page::ENTRY_COUNT * (h.get_sectors() - 1);

    for (size_t i = 0; i < max_items; ++i) {
        char name[nvs::Item::MAX_KEY_LENGTH + 1];
        snprintf(name, sizeof(name), "key%05d", static_cast<int>(i));
        TEST_ESP_OK(storage.writeItem(1, name, static_cast<int>(i), purgeAfterErase));
    }
    REQUIRE(storage.writeItem(1, "foo", 10, purgeAfterErase) == ESP_ERR_NVS_NOT_ENOUGH_SPACE);
}

TEST_CASE("can reuse the space previously occupied by the overwritten item", "[nvs]")
{
    // TC verifies that Storage.writeItem will reuse the space previously occupied by the overwritten item.
    // The test fills the complete storage with items having same key until it becomes full.
    // Then it writes one more item which may not fail as all previously overwritten items were
    // marked as erased and thus space occupied by some of them will be reclaimed and page will be erased.

    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);
    const bool purgeAfterErase = TEST_DEFAULT_PURGE_AFTER_ERASE;

    nvs::Storage storage(&h);
    TEST_ESP_OK(storage.init(0, h.get_sectors()));

    // Calculate how many items can be written to the storage.
    // One page (sector) has to be reserved for space reclaim operations,
    size_t max_usable_items = nvs::Page::ENTRY_COUNT * (h.get_sectors() - 1);
    // Add one more item to the count
    max_usable_items += 1;

    for (size_t i = 0; i < max_usable_items; ++i) {
        TEST_ESP_OK(storage.writeItem(1, "foo", 42U, purgeAfterErase));
    }
}

TEST_CASE("erase operations are distributed among sectors", "[nvs]")
{
    // This TC verifies inherent wear levelling of the NVS storage.
    // The test fills the storage with static items occupying 2 sectors, then performs many re-write operations
    // on the same item, and finally checks that erase counts are distributed among the remaining sectors.

    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);
    const bool purgeAfterErase = TEST_DEFAULT_PURGE_AFTER_ERASE;

    // Test parameters
    const size_t static_pages = 2;  // Number of NVS pages to fill with stable items
    const size_t erase_count =  4;   // Preset number of required erase cycles on NVS pages containing dynamic items

    // One page is reserved for space reclaim operations
    size_t all_pages = h.get_sectors();
    size_t usable_pages = all_pages -1;
    REQUIRE(usable_pages > static_pages);

    nvs::Storage storage(&h);
    TEST_ESP_OK(storage.init(0, all_pages));

    // Reset statistics
    NVSPartitionTestHelper::clear_stats();

    // Fill some part of storage with static values

    for (size_t i = 0; i < static_pages * nvs::Page::ENTRY_COUNT; ++i) {
        char name[nvs::Item::MAX_KEY_LENGTH];
        snprintf(name, sizeof(name), "static%d", (int) i);
        TEST_ESP_OK(storage.writeItem(1, name, i, purgeAfterErase));
    }

    // Calculate how many write operations we need to perform to ensure that every page will be erased
    size_t write_ops = (all_pages - static_pages) * nvs::Page::ENTRY_COUNT * erase_count;

    for (size_t i = 0; i < write_ops; ++i) {
        TEST_ESP_OK(storage.writeItem(1, "value", i, purgeAfterErase));
    }

    const size_t max_erase_cnt = write_ops / nvs::Page::ENTRY_COUNT / (all_pages - static_pages);

    for (size_t i = 0; i < all_pages; ++i) {

        auto erase_cnt = h.get_sector_erase_count(i);
        INFO("Page " << i << " erased " << erase_cnt);

        // If the page is within the static pages, it should not be erased
        if (i < static_pages) {
            CHECK(erase_cnt == 0);
            continue;
        }

        // Pages with dynamic items should be erased at most max_erase_cnt times, may be one less
        auto delta = max_erase_cnt - erase_cnt;
        CHECK(delta <= 1);
    }
}

TEST_CASE("can erase items", "[nvs]")
{
    // TC verifies that Storage.eraseItem works as expected.
    // The test verifies following:
    // - erasing an item works
    // - reading an erased item returns ESP_ERR_NVS_NOT_FOUND
    // - erasing a namespace works
    // - reading an item from erased namespace returns ESP_ERR_NVS_NOT_FOUND
    // - reading an item from another namespace works
    // - namespace id is respected in eraseItem and readItem

    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);
    const bool purgeAfterErase = TEST_DEFAULT_PURGE_AFTER_ERASE;

    nvs::Storage storage(&h);
    TEST_ESP_OK(storage.init(0, h.get_sectors()));

    // Fill the storage with some items
    for (size_t i = 0; i < nvs::Page::ENTRY_COUNT * 2 - 3; ++i) {
        char name[nvs::Item::MAX_KEY_LENGTH + 1];
        snprintf(name, sizeof(name), "key%05d", static_cast<int>(i));
        TEST_ESP_OK(storage.writeItem(3, name, static_cast<int>(i), purgeAfterErase));
    }
    TEST_ESP_OK(storage.writeItem(1, "foo", 32, purgeAfterErase));
    TEST_ESP_OK(storage.writeItem(2, "foo", 64, purgeAfterErase));
    TEST_ESP_OK(storage.eraseItem(2, "foo", purgeAfterErase));
    int val;
    TEST_ESP_OK(storage.readItem(1, "foo", val));
    CHECK(val == 32);
    TEST_ESP_OK(storage.eraseNamespace(3, purgeAfterErase));
    CHECK(storage.readItem(2, "foo", val) == ESP_ERR_NVS_NOT_FOUND);
    CHECK(storage.readItem(3, "key00222", val) == ESP_ERR_NVS_NOT_FOUND);
}

TEST_CASE("readonly handle fails on writing", "[nvs]")
{
    // TC verifies that readonly handle fails on writing.
    // The test verifies following:
    // - creating a namespace in read-write mode creates it
    // - opening a namespace in read-only mode fails on writing

    // Clear the partition before testing
    TEST_ESP_OK(nvs_flash_erase_partition(TEST_3SEC_PARTITION_NAME));

    const char *str = "value 0123456789abcdef0123456789abcdef";
    const uint8_t blob[8] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7};
    nvs_handle_t handle_1;

    TEST_ESP_OK(nvs_flash_init_partition(TEST_3SEC_PARTITION_NAME));

    // first, creating namespace...
    TEST_ESP_OK(nvs_open_from_partition(TEST_3SEC_PARTITION_NAME, "ro_ns", NVS_READWRITE, &handle_1));
    nvs_close(handle_1);

    TEST_ESP_OK(nvs_open_from_partition(TEST_3SEC_PARTITION_NAME, "ro_ns", NVS_READONLY, &handle_1));
    TEST_ESP_ERR(nvs_set_i32(handle_1, "key", 47), ESP_ERR_NVS_READ_ONLY);
    TEST_ESP_ERR(nvs_set_str(handle_1, "key", str), ESP_ERR_NVS_READ_ONLY);
    TEST_ESP_ERR(nvs_set_blob(handle_1, "key", blob, 8), ESP_ERR_NVS_READ_ONLY);

    nvs_close(handle_1);

    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_3SEC_PARTITION_NAME));
}

TEST_CASE("nvs api tests", "[nvs]")
{
    // TC verifies that NVS API works as expected.

    // Fill the partition with random data before testing
    TEST_ESP_OK(NVSPartitionTestHelper::randomize_partition(TEST_3SEC_PARTITION_NAME, 100));

    nvs_handle_t handle_1;

    // Opening a namespace before initializing NVS should fail
    TEST_ESP_ERR(nvs_open_from_partition(TEST_3SEC_PARTITION_NAME, "namespace1", NVS_READWRITE, &handle_1), ESP_ERR_NVS_NOT_INITIALIZED);

    // Clear the partition before initializing NVS
    TEST_ESP_OK(nvs_flash_erase_partition(TEST_3SEC_PARTITION_NAME));

    // Initialize NVS partition
    TEST_ESP_OK(nvs_flash_init_partition(TEST_3SEC_PARTITION_NAME));

    // Now opening a non existent namespace in read-only mode should fail
    TEST_ESP_ERR(nvs_open_from_partition(TEST_3SEC_PARTITION_NAME, "namespace1", NVS_READONLY, &handle_1), ESP_ERR_NVS_NOT_FOUND);

    // Opening a non existent namespace in read-write mode should create it
    TEST_ESP_OK(nvs_open_from_partition(TEST_3SEC_PARTITION_NAME, "namespace1", NVS_READWRITE, &handle_1));
    TEST_ESP_OK(nvs_set_i32(handle_1, "foo", 0x12345678));
    TEST_ESP_OK(nvs_set_i32(handle_1, "foo", 0x23456789));

    nvs_handle_t handle_2;
    TEST_ESP_OK(nvs_open_from_partition(TEST_3SEC_PARTITION_NAME, "namespace2", NVS_READWRITE, &handle_2));
    TEST_ESP_OK(nvs_set_i32(handle_2, "foo", 0x3456789a));
    char str[] = "value 0123456789abcdef0123456789abcdef";
    TEST_ESP_OK(nvs_set_str(handle_2, "key", str));

    // Now we can read the values back
    int32_t v1;
    TEST_ESP_OK(nvs_get_i32(handle_1, "foo", &v1));
    CHECK(0x23456789 == v1);

    int32_t v2;
    TEST_ESP_OK(nvs_get_i32(handle_2, "foo", &v2));
    CHECK(0x3456789a == v2);

    char buf[sizeof(str)];
    size_t buf_len = sizeof(buf);

    // get str returns the length of the string when the buffer is NULL
    size_t buf_len_needed;
    TEST_ESP_OK(nvs_get_str(handle_2, "key", NULL, &buf_len_needed));
    CHECK(buf_len_needed == buf_len);

    // attempt to get string into the buffer which is too short should fail
    size_t buf_len_short = buf_len - 1;
    TEST_ESP_ERR(ESP_ERR_NVS_INVALID_LENGTH, nvs_get_str(handle_2, "key", buf, &buf_len_short));
    CHECK(buf_len_short == buf_len);

    // attempt to get string into the buffer which is larger than the value should succeed
    size_t buf_len_long = buf_len + 1;
    TEST_ESP_OK(nvs_get_str(handle_2, "key", buf, &buf_len_long));
    CHECK(buf_len_long == buf_len);

    // attempt to get string into the buffer which matches the size of the value should succeed
    TEST_ESP_OK(nvs_get_str(handle_2, "key", buf, &buf_len));

    CHECK(0 == strcmp(buf, str));
    nvs_close(handle_1);
    nvs_close(handle_2);

    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_3SEC_PARTITION_NAME));
}

TEST_CASE("deinit partition doesn't affect other partition's open handles", "[nvs]")
{
    // TC verifies that deinitializing one NVS partition doesn't affect open handles in another partition.
    // The test verifies following:
    // - deinitializing one partition doesn't affect open handles in another partition
    // - writing to the open handle in the other partition works after deinitialization of the first partition

    // Clear both partitions before initializing NVS
    TEST_ESP_OK(nvs_flash_erase_partition(TEST_3SEC_PARTITION_NAME));
    TEST_ESP_OK(nvs_flash_erase_partition(TEST_SECONDARY_PARTITION_NAME));


    // Initialize both NVS partitions
    TEST_ESP_OK(nvs_flash_init_partition(TEST_3SEC_PARTITION_NAME));
    TEST_ESP_OK(nvs_flash_init_partition(TEST_SECONDARY_PARTITION_NAME));

    nvs_handle_t handle_1;

    // Open a namespace in the secondary partition
    TEST_ESP_OK(nvs_open_from_partition(TEST_SECONDARY_PARTITION_NAME, "ns", NVS_READWRITE, &handle_1));

    // Deinitializing must not interfere with the open handle from the other partition.
    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_3SEC_PARTITION_NAME));

    TEST_ESP_OK(nvs_set_i32(handle_1, "foo", 0x3456789a));
    nvs_close(handle_1);

    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_SECONDARY_PARTITION_NAME));
}

TEST_CASE("nvs iterator nvs_entry_find invalid parameter test", "[nvs]")
{
    // TC verifies that nvs_entry_find returns ESP_ERR_INVALID_ARG on invalid parameters.

    nvs_iterator_t it = reinterpret_cast<nvs_iterator_t>(0xbeef);
    CHECK(nvs_entry_find(nullptr, NULL, NVS_TYPE_ANY, &it) == ESP_ERR_INVALID_ARG);
    CHECK(nvs_entry_find("nvs", NULL, NVS_TYPE_ANY, nullptr) == ESP_ERR_INVALID_ARG);
}

TEST_CASE("nvs iterator nvs_entry_find doesn't change iterator on parameter error", "[nvs]")
{
    // TC verifies that nvs_entry_find doesn't change iterator on parameter error.
    // The test verifies following:
    // - if the iterator is not NULL, it should not be changed on parameter error
    // - if the iterator is NULL, it should remain NULL on parameter error
    nvs_iterator_t it = reinterpret_cast<nvs_iterator_t>(0xbeef);
    REQUIRE(nvs_entry_find(nullptr, NULL, NVS_TYPE_ANY, &it) == ESP_ERR_INVALID_ARG);
    CHECK(it == reinterpret_cast<nvs_iterator_t>(0xbeef));

    it = nullptr;
    REQUIRE(nvs_entry_find(nullptr, NULL, NVS_TYPE_ANY, &it) == ESP_ERR_INVALID_ARG);
    CHECK(it == nullptr);
}

TEST_CASE("nvs_entry_next return ESP_ERR_INVALID_ARG on parameter is NULL", "[nvs]")
{
    // TC verifies that nvs_entry_next returns ESP_ERR_INVALID_ARG on NULL iterator.
    // The test verifies following:
    // - if the iterator is NULL, nvs_entry_next should return ESP_ERR_INVALID_ARG

    CHECK(nvs_entry_next(nullptr) == ESP_ERR_INVALID_ARG);
}

TEST_CASE("nvs_entry_info fails with ESP_ERR_INVALID_ARG if a parameter is NULL", "[nvs]")
{
    // TC verifies that nvs_entry_info returns ESP_ERR_INVALID_ARG on NULL parameters.
    // The test verifies following:
    // - if the iterator is NULL, nvs_entry_info should return ESP_ERR_INVALID_ARG
    // - if the info pointer is NULL, nvs_entry_info should return ESP_ERR_INVALID

    nvs_iterator_t it = reinterpret_cast<nvs_iterator_t>(0xbeef);
    nvs_entry_info_t info;
    CHECK(nvs_entry_info(it, nullptr) == ESP_ERR_INVALID_ARG);
    CHECK(nvs_entry_info(nullptr, &info) == ESP_ERR_INVALID_ARG);
}

TEST_CASE("nvs_entry_info doesn't change iterator on parameter error", "[nvs]")
{
    // TC verifies that nvs_entry_info doesn't change iterator on parameter error.
    // The test verifies following:
    // - if the iterator is not NULL and the info pointer is NULL, iterator should not be changed
    // - if the iterator is NULL and the info pointer is not NULL, iterator should remain NULL

    nvs_iterator_t it = reinterpret_cast<nvs_iterator_t>(0xbeef);
    REQUIRE(nvs_entry_info(it, nullptr) == ESP_ERR_INVALID_ARG);
    CHECK(it == reinterpret_cast<nvs_iterator_t>(0xbeef));

    it = nullptr;
    REQUIRE(nvs_entry_info(it, nullptr) == ESP_ERR_INVALID_ARG);
    CHECK(it == nullptr);
}

TEST_CASE("nvs iterators tests", "[nvs]")
{
    // Clear the partition before testing
    TEST_ESP_OK(nvs_flash_erase_partition(TEST_DEFAULT_PARTITION_NAME));

    // Initialize NVS partition
    TEST_ESP_OK(nvs_flash_init_partition(TEST_DEFAULT_PARTITION_NAME));

    nvs_iterator_t it;
    nvs_entry_info_t info;
    nvs_handle_t handle_1;
    nvs_handle_t handle_2;
    const  uint32_t blob = 0x11223344;
    const char *name_1 = "namespace1";
    const char *name_2 = "namespace2";

    TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, name_1, NVS_READWRITE, &handle_1));
    TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, name_2, NVS_READWRITE, &handle_2));

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
        for (count = 0; res == ESP_OK; count++) {
            res = nvs_entry_next(&it);
        }
        CHECK(res == ESP_ERR_NVS_NOT_FOUND); // after finishing the loop or if no entry was found to begin with,
        // res has to be ESP_ERR_NVS_NOT_FOUND or some internal error
        // or programming error occurred
        nvs_release_iterator(it); // unnecessary call but emphasizes the programming pattern
        return count;
    };

    auto entry_count_handle = [](nvs_handle_t handle, nvs_type_t type)-> int {
        int count = 0;
        nvs_iterator_t it = nullptr;
        esp_err_t res = nvs_entry_find_in_handle(handle, type, &it);
        for (count = 0; res == ESP_OK; count++) {
            res = nvs_entry_next(&it);
        }
        CHECK(res == ESP_ERR_NVS_NOT_FOUND); // after finishing the loop or if no entry was found to begin with,
        // res has to be ESP_ERR_NVS_NOT_FOUND or some internal error
        // or programming error occurred
        nvs_release_iterator(it); // unnecessary call but emphasizes the programming pattern
        return count;
    };

    SECTION("No partition found return ESP_ERR_NVS_NOT_FOUND") {
        CHECK(nvs_entry_find("", NULL, NVS_TYPE_ANY, &it) == ESP_ERR_NVS_NOT_FOUND);
    }

    SECTION("No matching namespace found return ESP_ERR_NVS_NOT_FOUND") {
        CHECK(nvs_entry_find(TEST_DEFAULT_PARTITION_NAME, "nonexistent", NVS_TYPE_ANY, &it) == ESP_ERR_NVS_NOT_FOUND);
    }

    SECTION("nvs_entry_find sets iterator to null if no matching element found") {
        it = reinterpret_cast<nvs_iterator_t>(0xbeef);
        REQUIRE(nvs_entry_find(TEST_DEFAULT_PARTITION_NAME, "nonexistent", NVS_TYPE_I16, &it) == ESP_ERR_NVS_NOT_FOUND);
        CHECK(it == nullptr);
    }

    SECTION("Finding iterator means iterator is valid") {
        it = nullptr;
        TEST_ESP_OK(nvs_entry_find(TEST_DEFAULT_PARTITION_NAME, nullptr, NVS_TYPE_ANY, &it));
        CHECK(it != nullptr);
        nvs_release_iterator(it);
    }

    SECTION("Return ESP_ERR_NVS_NOT_FOUND after iterating over last matching element") {
        it = nullptr;
        REQUIRE(nvs_entry_find(TEST_DEFAULT_PARTITION_NAME, name_1, NVS_TYPE_I16, &it) == ESP_OK);
        REQUIRE(it != nullptr);
        CHECK(nvs_entry_next(&it) == ESP_ERR_NVS_NOT_FOUND);
    }

    SECTION("Set iterator to NULL after iterating over last matching element") {
        it = nullptr;
        REQUIRE(nvs_entry_find(TEST_DEFAULT_PARTITION_NAME, name_1, NVS_TYPE_I16, &it) == ESP_OK);
        REQUIRE(it != nullptr);
        REQUIRE(nvs_entry_next(&it) == ESP_ERR_NVS_NOT_FOUND);
        CHECK(it == nullptr);
    }

    SECTION("Number of entries found for specified namespace and type is correct") {
        CHECK(entry_count(TEST_DEFAULT_PARTITION_NAME, NULL, NVS_TYPE_ANY) == 15);
        CHECK(entry_count(TEST_DEFAULT_PARTITION_NAME, name_1, NVS_TYPE_ANY) == 11);
        CHECK(entry_count(TEST_DEFAULT_PARTITION_NAME, name_1, NVS_TYPE_I32) == 3);
        CHECK(entry_count(TEST_DEFAULT_PARTITION_NAME, NULL, NVS_TYPE_I32) == 5);
        CHECK(entry_count(TEST_DEFAULT_PARTITION_NAME, NULL, NVS_TYPE_U64) == 1);
    }

    SECTION("Number of entries found for specified handle and type is correct") {
        CHECK(entry_count_handle(handle_1, NVS_TYPE_ANY) == 11);
        CHECK(entry_count_handle(handle_1, NVS_TYPE_I32) == 3);
        CHECK(entry_count_handle(handle_2, NVS_TYPE_ANY) == 4);
        CHECK(entry_count_handle(handle_2, NVS_TYPE_I32) == 2);
        CHECK(entry_count_handle(handle_2, NVS_TYPE_U64) == 1);
    }

    SECTION("New entry is not created when existing key-value pair is set") {
        CHECK(entry_count(TEST_DEFAULT_PARTITION_NAME, name_2, NVS_TYPE_ANY) == 4);
        TEST_ESP_OK(nvs_set_i32(handle_2, "value1", -222));
        CHECK(entry_count(TEST_DEFAULT_PARTITION_NAME, name_2, NVS_TYPE_ANY) == 4);
    }

    SECTION("Number of entries found decrease when entry is erased") {
        CHECK(entry_count(TEST_DEFAULT_PARTITION_NAME, NULL, NVS_TYPE_U64) == 1);
        TEST_ESP_OK(nvs_erase_key(handle_2, "value4"));
        CHECK(entry_count(TEST_DEFAULT_PARTITION_NAME, "", NVS_TYPE_U64) == 0);
    }

    SECTION("All fields of nvs_entry_info_t structure are correct") {
        it = nullptr;
        esp_err_t res = nvs_entry_find(TEST_DEFAULT_PARTITION_NAME, name_1, NVS_TYPE_I32, &it);
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
        nvs_release_iterator(it); // unnecessary call but emphasizes the programming pattern
    }

    SECTION("Entry info is not affected by subsequent erase") {
        nvs_entry_info_t info_after_erase;

        TEST_ESP_OK(nvs_entry_find(TEST_DEFAULT_PARTITION_NAME, name_1, NVS_TYPE_ANY, &it));
        REQUIRE(nvs_entry_info(it, &info) == ESP_OK);
        TEST_ESP_OK(nvs_erase_key(handle_1, "value1"));
        REQUIRE(nvs_entry_info(it, &info_after_erase) == ESP_OK);
        CHECK(memcmp(&info, &info_after_erase, sizeof(info)) == 0);
        nvs_release_iterator(it);
    }

    SECTION("Entry info is not affected by subsequent set") {
        nvs_entry_info_t info_after_set;

        TEST_ESP_OK(nvs_entry_find(TEST_DEFAULT_PARTITION_NAME, name_1, NVS_TYPE_ANY, &it));
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

        TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, name_3, NVS_READWRITE, &handle_3));
        for (size_t i = 0; i < entries_created; i++) {
            TEST_ESP_OK(nvs_set_u8(handle_3, to_string(i).c_str(), 123));
        }

        int entries_found = 0;
        it = nullptr;
        esp_err_t res = nvs_entry_find(TEST_DEFAULT_PARTITION_NAME, name_3, NVS_TYPE_ANY, &it);
        while (res == ESP_OK) {
            entries_found++;
            res = nvs_entry_next(&it);
        }
        CHECK(res == ESP_ERR_NVS_NOT_FOUND); // after finishing the loop, res has to be ESP_ERR_NVS_NOT_FOUND
        // or some internal error or programming error occurred
        CHECK(entries_created == entries_found);

        nvs_release_iterator(it); // unnecessary call but emphasizes the programming pattern
        nvs_close(handle_3);
    }

    SECTION("Iterating over multi-page blob works correctly") {
        nvs_handle_t handle_3;
        const char *name_3 = "namespace3";
        const uint8_t multipage_blob[4096 * 2] = { 0 };
        const int NUMBER_OF_ENTRIES_PER_PAGE = 125;
        size_t occupied_entries;

        TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, name_3, NVS_READWRITE, &handle_3));
        TEST_ESP_OK(nvs_set_blob(handle_3, "blob", multipage_blob, sizeof(multipage_blob)));
        TEST_ESP_OK(nvs_get_used_entry_count(handle_3, &occupied_entries));
        CHECK(occupied_entries > NUMBER_OF_ENTRIES_PER_PAGE *  2);

        CHECK(entry_count(TEST_DEFAULT_PARTITION_NAME, name_3, NVS_TYPE_BLOB) == 1);

        nvs_close(handle_3);
    }

    nvs_close(handle_1);
    nvs_close(handle_2);

    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME));
}

TEST_CASE("Iterator with not matching type iterates correctly", "[nvs]")
{
    // TC verifies that nvs_entry_find iterates correctly when the type doesn't match.
    // The test verifies following:
    // - nvs_entry_find with type NVS_TYPE_I32 returns ESP_ERR_NVS_NOT_FOUND if no entry with that type exists
    // - nvs_entry_find with type NVS_TYPE_STR returns ESP_OK if an entry with that type exists
    // - after writing nvs_entry with type NVS_TYPE_I32 nvs_entry_find with type NVS_TYPE_I32 returns ESP_OK

    // Clear the partition before testing
    TEST_ESP_OK(nvs_flash_erase_partition(TEST_DEFAULT_PARTITION_NAME));

    // Initialize NVS partition
    TEST_ESP_OK(nvs_flash_init_partition(TEST_DEFAULT_PARTITION_NAME));

    nvs_iterator_t it;
    nvs_handle_t my_handle;
    const char *NAMESPACE = "test_ns_4";

    // writing string to namespace (a type which spans multiple entries)
    TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, NAMESPACE, NVS_READWRITE, &my_handle));
    TEST_ESP_OK(nvs_set_str(my_handle, "test-string", "InitString0"));
    TEST_ESP_OK(nvs_commit(my_handle));
    nvs_close(my_handle);

    // attempt to find an entry with type NVS_TYPE_I32 should return ESP_ERR_NVS_NOT_FOUND
    CHECK(nvs_entry_find(TEST_DEFAULT_PARTITION_NAME, NAMESPACE, NVS_TYPE_I32, &it) == ESP_ERR_NVS_NOT_FOUND);

    // attempt to find an entry with type NVS_TYPE_STR should return ESP_OK and iterator should be valid
    TEST_ESP_OK(nvs_entry_find(TEST_DEFAULT_PARTITION_NAME, NAMESPACE, NVS_TYPE_STR, &it));
    nvs_release_iterator(it);

    // Write an entry with type NVS_TYPE_I32
    TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, NAMESPACE, NVS_READWRITE, &my_handle));
    TEST_ESP_OK(nvs_set_i32(my_handle, "test-int32", 42));
    TEST_ESP_OK(nvs_commit(my_handle));
    nvs_close(my_handle);

    // attempt to find an entry with type NVS_TYPE_I32 should return ESP_OK and iterator should be valid
    TEST_ESP_OK(nvs_entry_find(TEST_DEFAULT_PARTITION_NAME, NAMESPACE, NVS_TYPE_I32, &it));
    nvs_release_iterator(it);

    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME));
}

TEST_CASE("wifi test", "[nvs]")
{
    // TC verifies that NVS API works as expected for WiFi related items.

    // Fill the partition with random data before testing
    TEST_ESP_OK(NVSPartitionTestHelper::randomize_partition(TEST_3SEC_PARTITION_NAME, 10));

    NVSPartitionTestHelper::clear_stats(); // Clear statistics before init

    // Initialize NVS partition
    TEST_ESP_OK(nvs_flash_init_partition(TEST_3SEC_PARTITION_NAME));

    nvs_handle_t misc_handle;
    TEST_ESP_OK(nvs_open_from_partition(TEST_3SEC_PARTITION_NAME, "nvs.net80211", NVS_READWRITE, &misc_handle));
    char log[33];
    size_t log_size = sizeof(log);
    TEST_ESP_ERR(nvs_get_str(misc_handle, "log", log, &log_size), ESP_ERR_NVS_NOT_FOUND);
    strcpy(log, "foobarbazfizzz");
    TEST_ESP_OK(nvs_set_str(misc_handle, "log", log));
    nvs_close(misc_handle);

    nvs_handle_t net80211_handle;
    TEST_ESP_OK(nvs_open_from_partition(TEST_3SEC_PARTITION_NAME, "nvs.net80211", NVS_READWRITE, &net80211_handle));

    uint8_t opmode = 2;
    TEST_ESP_ERR(nvs_get_u8(net80211_handle, "wifi.opmode", &opmode), ESP_ERR_NVS_NOT_FOUND);

    TEST_ESP_OK(nvs_set_u8(net80211_handle, "wifi.opmode", opmode));

    uint8_t country = 0;
    TEST_ESP_ERR(nvs_get_u8(net80211_handle, "wifi.country", &country), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_OK(nvs_set_u8(net80211_handle, "wifi.country", country));

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

    nvs_close(net80211_handle);

    s_perf << "Time to simulate nvs init with wifi libs: " << NVSPartitionTestHelper::get_total_time() << " us (" << NVSPartitionTestHelper::get_erase_ops() << "E " << NVSPartitionTestHelper::get_write_ops() << "W " << NVSPartitionTestHelper::get_read_ops() << "R " << NVSPartitionTestHelper::get_write_bytes() << "Wb " << NVSPartitionTestHelper::get_read_bytes() << "Rb)" << std::endl;

    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_3SEC_PARTITION_NAME));
}

extern "C" void nvs_dump(const char *partName);

class RandomTest {

    static const size_t nKeys = 11;
    static const size_t strBufLen = 1024;
    static const size_t smallBlobLen = nvs::Page::CHUNK_MAX_SIZE / 3;
    static const size_t largeBlobLen = nvs::Page::CHUNK_MAX_SIZE * 3;

    // values
    int32_t v1 = 0, v2 = 0;
    uint64_t v3 = 0, v4 = 0;
    char v5[strBufLen], v6[strBufLen], v7[strBufLen], v8[strBufLen], v9[strBufLen];
    uint8_t v10[smallBlobLen], v11[largeBlobLen];

    // future_values
    int32_t f_v1 = 0, f_v2 = 0;
    uint64_t f_v3 = 0, f_v4 = 0;
    char f_v5[strBufLen], f_v6[strBufLen], f_v7[strBufLen], f_v8[strBufLen], f_v9[strBufLen];
    uint8_t f_v10[smallBlobLen], f_v11[largeBlobLen];

    bool written[nKeys];
    bool potentially_written[nKeys];

public:
    RandomTest()
    {
        std::fill_n(written, nKeys, false);
        std::fill_n(potentially_written, nKeys, false);
    }

    bool compare(const nvs::ItemType read_type,
                 const void *buf1,
                 const void *buf2,
                 const size_t len = 0)
    {
        switch (read_type) {
        case nvs::ItemType::I32:
            return (*reinterpret_cast<const int32_t *>(buf1) == *reinterpret_cast<const int32_t *>(buf2));
        case nvs::ItemType::U64:
            return (*reinterpret_cast<const int64_t *>(buf1) == *reinterpret_cast<const int64_t *>(buf2));
        case nvs::ItemType::SZ:
            return (strncmp(reinterpret_cast<const char *>(buf1), reinterpret_cast<const char *>(buf2), len) == 0);
        case nvs::ItemType::BLOB:
            return (memcmp(reinterpret_cast<const uint8_t *>(buf1), reinterpret_cast<const uint8_t *>(buf2), len) == 0);
        default:
            return false;
        }
    }

    const char *getTypeDesc(const nvs::ItemType desc_type)
    {
        switch (desc_type) {
        case nvs::ItemType::I32: return "nvs::ItemType::I32";
        case nvs::ItemType::U64: return "nvs::ItemType::U64";
        case nvs::ItemType::SZ: return "nvs::ItemType::SZ";
        case nvs::ItemType::BLOB: return "nvs::ItemType::BLOB";
        default: return "Other";
        }
    }

    bool evaluate(const uint32_t delayCount,    // marks place in test where the event occurs
                  const esp_err_t read_result,
                  const nvs::ItemType read_type,
                  const bool written,
                  const bool potentially_written,
                  const void *buff,
                  const void *value,
                  const void *future_value,
                  const size_t len = 0)
    {

        // sequential evaluation
        // read     | written | potentially_written | values | future_values | result
        // NotFound | FALSE   | *                   | *      | *             | PASS
        // OK       | TRUE    | *                   | X      | *             | PASS
        // OK       | *       | TRUE                | *      | X             | PASS
        // REST IS FAILED

        if (read_result == ESP_ERR_NVS_NOT_FOUND && !written) {
            return true;
        }
        if (read_result == ESP_OK) {
            if (written && compare(read_type, buff, value, len)) {
                return true;
            }
            if (potentially_written && compare(read_type, buff, future_value, len)) {
                return true;
            }
        }

        // if we are here, the read operation failed or returned unexpected value
        auto print_bytes = [](const void* ptr, size_t len) -> std::string {
            if (!ptr) return "null";
            std::ostringstream oss;
            oss << "0x";
            const uint8_t* bytes = reinterpret_cast<const uint8_t*>(ptr);
            for (size_t i = 0; i < len; ++i) {
            oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(bytes[i]);
            }
            return oss.str();
        };

        std::cout   << "Error at delayCount: " << delayCount << std::endl
                    << "read_result: " << esp_err_to_name(read_result) << std::endl
                    << "read_type: " << getTypeDesc(read_type) << std::endl
                    << "written: " << (written ? "true" : "false") << std::endl
                    << "potentially_written: " << (potentially_written ? "true" : "false") << std::endl
                    << "len: " << len << std::endl
                    << "buff: " << print_bytes(buff, len) << std::endl
                    << "value: " << print_bytes(value, len) << std::endl
                    << "future_value: " << print_bytes(future_value, len) << std::endl;
        return false;
    }

    template<typename TGen>
    esp_err_t doRandomThings(nvs_handle_t handle, TGen gen, size_t &count, uint32_t delayCount = 0)
    // the power - off feature msometimes causes nvs write call to return ESP_ERR_FLASH_OP_FAIL while subsequent
    // nvs read operation actually returns the data written by the failed call.
    // To allow verification of correctly written as well as Schroedinger's way written data we:
    // Initialize values[] written[] and future_values[] and potentially_written[] to empty(0) / false
    // - before any attempt to write data, we will remember, what are we writing into future_values[]
    // - in case of success, we will move the new data to the actual buffer values[]
    // and set respective written[] flag to true
    // - when result code from write operation is ESP_ERR_FLASH_OP_FAIL
    // we keep values[] and written[] as it was before the call AND
    // we mark entry in potentially_written[] to tell the evaluation part of function to accept either values[]
    // or future_values[] as correct content.
    {
        const char *keys[] = {"foo", "bar", "longkey_0123456", "another key", "param1", "param2", "param3", "param4", "param5", "singlepage", "multipage"};
        const nvs::ItemType types[] = {nvs::ItemType::I32, nvs::ItemType::I32, nvs::ItemType::U64, nvs::ItemType::U64, nvs::ItemType::SZ, nvs::ItemType::SZ, nvs::ItemType::SZ, nvs::ItemType::SZ, nvs::ItemType::SZ, nvs::ItemType::BLOB, nvs::ItemType::BLOB};

        void *values[] = {&v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8, &v9, &v10, &v11};
        void *future_values[] = {&f_v1, &f_v2, &f_v3, &f_v4, &f_v5, &f_v6, &f_v7, &f_v8, &f_v9, &f_v10, &f_v11};

        const size_t nKeys = sizeof(keys) / sizeof(keys[0]);
        static_assert(nKeys == sizeof(types) / sizeof(types[0]), "");
        static_assert(nKeys == sizeof(values) / sizeof(values[0]), "");
        static_assert(nKeys == sizeof(future_values) / sizeof(future_values[0]), "");

        auto randomRead = [&](size_t index) -> esp_err_t {
            switch (types[index]) {
            case nvs::ItemType::I32: {
                int32_t val;
                auto err = nvs_get_i32(handle, keys[index], &val);
                REQUIRE(evaluate(delayCount, err, types[index], written[index], potentially_written[index], &val, values[index], future_values[index]) == true);
                break;
            }

            case nvs::ItemType::U64: {
                uint64_t val;
                auto err = nvs_get_u64(handle, keys[index], &val);
                REQUIRE(evaluate(delayCount, err, types[index], written[index], potentially_written[index], &val, values[index], future_values[index]) == true);
                break;
            }

            case nvs::ItemType::SZ: {
                char buf[strBufLen];
                size_t len = strBufLen;
                auto err = nvs_get_str(handle, keys[index], buf, &len);
                REQUIRE(evaluate(delayCount, err, types[index], written[index], potentially_written[index], buf, values[index], future_values[index], strBufLen) == true);
                break;
            }

            case nvs::ItemType::BLOB: {
                uint32_t blobBufLen = 0;
                if (strncmp(keys[index], "singlepage", sizeof("singlepage")) == 0) {
                    blobBufLen = smallBlobLen ;
                } else {
                    blobBufLen = largeBlobLen ;
                }

                uint8_t* buf = static_cast<uint8_t*>(malloc(blobBufLen));
                memset(buf, 0, blobBufLen);

                size_t len = blobBufLen;
                auto err = nvs_get_blob(handle, keys[index], buf, &len);
                auto eval_result = evaluate(delayCount, err, types[index], written[index], potentially_written[index], buf, values[index], future_values[index], len);
                free(buf);

                REQUIRE(eval_result == true);
                break;
            }

            default:
                assert(0);
            }
            return ESP_OK;
        };

        auto randomWrite = [&](size_t index) -> esp_err_t {
            switch (types[index]) {
            case nvs::ItemType::I32: {
                int32_t val = static_cast<int32_t>(gen());

                auto err = nvs_set_i32(handle, keys[index], val);

                // remember future value
                *reinterpret_cast<int32_t *>(future_values[index]) = val;

                if (err == ESP_ERR_FLASH_OP_FAIL) {
                    // mark potentially written
                    potentially_written[index] = true;
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
                    // mark potentially written
                    potentially_written[index] = true;
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

                size_t strLen = gen() % (strBufLen - 1);
                std::generate_n(buf, strLen, [&]() -> char {
                    const char c = static_cast<char>(gen() % 127);
                    return (c < 32) ? 32 : c;
                });
                buf[strLen] = 0;

                auto err = nvs_set_str(handle, keys[index], buf);

                // remember value
                strncpy(reinterpret_cast<char *>(future_values[index]), buf, strBufLen);

                if (err == ESP_ERR_FLASH_OP_FAIL) {
                    // mark potentially written
                    potentially_written[index] = true;
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
                uint8_t* buf = static_cast<uint8_t*>(malloc(blobBufLen));
                if(!buf) {
                    return ESP_ERR_NO_MEM;
                }
                memset(buf, 0, blobBufLen);
                size_t blobLen = gen() % blobBufLen;
                std::generate_n(buf, blobLen, [&]() -> uint8_t {
                    return static_cast<uint8_t>(gen() % 256);
                });

                auto err = nvs_set_blob(handle, keys[index], buf, blobLen);

                // remember value
                memcpy(reinterpret_cast<char *>(future_values[index]), buf, blobBufLen);

                if (err == ESP_ERR_FLASH_OP_FAIL) {
                    // mark potentially written
                    potentially_written[index] = true;
                    free(buf);
                    return err;
                }
                if (err == ESP_ERR_NVS_REMOVE_FAILED) {
                    written[index] = true;
                    memcpy(reinterpret_cast<uint8_t *>(values[index]), buf, blobBufLen);
                    free(buf);
                    return ESP_ERR_FLASH_OP_FAIL;
                }
                REQUIRE(err == ESP_OK);
                written[index] = true;
                memcpy(reinterpret_cast<char *>(values[index]), buf, blobBufLen);
                free(buf);
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

    esp_err_t handleExternalWriteAtIndex(uint8_t index, const void *value, const size_t len)
    {
        if (index == 9) { // This is only done for small-page blobs for now
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
    // TC verifies that NVS API works as expected for random read/write operations.
    // This is very long test, which is intended to be run manually

    // Test parameters
    size_t count = 1000;
    uint32_t seed = 3;

    // Randomize the partition before testing
    TEST_ESP_OK(NVSPartitionTestHelper::randomize_partition(TEST_DEFAULT_PARTITION_NAME, seed));

    // Initialize NVS partition
    TEST_ESP_OK(nvs_flash_init_partition(TEST_DEFAULT_PARTITION_NAME));

    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, "namespace1", NVS_READWRITE, &handle));
    RandomTest test;

    std::random_device rd;
    std::mt19937 gen(rd());
    gen.seed(seed);

    NVSPartitionTestHelper::clear_stats(); // Clear statistics

    TEST_ESP_OK(test.doRandomThings(handle, gen, count));

    s_perf << "Monkey test: nErase=" << NVSPartitionTestHelper::get_erase_ops() << " nWrite=" << NVSPartitionTestHelper::get_write_ops() << std::endl;

    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME));
}

TEST_CASE("test for memory leaks in open/set", "[leaks]")
{
    // TC verifies that NVS API does not leak memory when opening and setting values.
    // The test verifies following:
    // - nvs_open and nvs_set_blob are called in a loop
    // - nvs_flash_deinit_partition is called at the end of the test

    // Erase the partition before testing
    TEST_ESP_OK(nvs_flash_erase_partition(TEST_3SEC_PARTITION_NAME));

    // Initialize NVS partition
    TEST_ESP_OK(nvs_flash_init_partition(TEST_3SEC_PARTITION_NAME));

    for (int i = 0; i < 100000; ++i) {
        nvs_handle_t light_handle = 0;
        char lightbulb[1024] = {12, 13, 14, 15, 16};
        TEST_ESP_OK(nvs_open_from_partition(TEST_3SEC_PARTITION_NAME, "light", NVS_READWRITE, &light_handle));
        TEST_ESP_OK(nvs_set_blob(light_handle, "key", lightbulb, sizeof(lightbulb)));
        TEST_ESP_OK(nvs_commit(light_handle));
        nvs_close(light_handle);
    }

    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_3SEC_PARTITION_NAME));
}

TEST_CASE("read/write failure (TW8406)", "[nvs]")
{
    // TC verifies that NVS API works as expected when writing and reading values.

    // Erase the partition before testing
    TEST_ESP_OK(nvs_flash_erase_partition(TEST_3SEC_PARTITION_NAME));

    // Initialize NVS partition
    TEST_ESP_OK(nvs_flash_init_partition(TEST_3SEC_PARTITION_NAME));

    for (int attempts = 0; attempts < 3; ++attempts) {
        int i = 0;
        nvs_handle_t light_handle = 0;
        char key[15] = {0};
        char data[76] = {12, 13, 14, 15, 16};
        uint8_t number = 20;
        size_t data_len = sizeof(data);

        ESP_ERROR_CHECK(nvs_open_from_partition(TEST_3SEC_PARTITION_NAME, "LIGHT", NVS_READWRITE, &light_handle));
        ESP_ERROR_CHECK(nvs_set_u8(light_handle, "RecordNum", number));
        for (i = 0; i < number; ++i) {
            snprintf(key, sizeof(key), "light%d", i);
            ESP_ERROR_CHECK(nvs_set_blob(light_handle, key, data, sizeof(data)));
        }
        nvs_commit(light_handle);

        uint8_t get_number = 0;
        ESP_ERROR_CHECK(nvs_get_u8(light_handle, "RecordNum", &get_number));
        REQUIRE(number == get_number);
        for (i = 0; i < number; ++i) {
            char data[76] = {0};
            snprintf(key, sizeof(key), "light%d", i);
            ESP_ERROR_CHECK(nvs_get_blob(light_handle, key, data, &data_len));
        }
        nvs_close(light_handle);
    }

    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_3SEC_PARTITION_NAME));
}

TEST_CASE("nvs_flash_init checks for an empty page", "[nvs]")
{
    // TC verifies that nvs_flash_init_partition checks for an empty page in the partition.
    // This test is to ensure that the NVS partition manager does not initialize
    // the partition if it does not have at least one empty page available.
    // The test verifies following:
    // - nvs_flash_init_partition returns ESP_ERR_NVS_NO_FREE_PAGES if there are no free pages available
    // - nvs_flash_init_partition returns ESP_OK if there is at least one free page available


    // Initialize NVS partition
    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);

    // We need at least 5 sectors to be able to run this test
    CHECK(h.get_sectors() >= 5);

    const size_t blob_size = nvs::Page::CHUNK_MAX_SIZE;
    uint8_t blob[blob_size] = {0};

    // Initialize NVS partition with 5 pages
    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(&h, 0, 5));
    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, "test", NVS_READWRITE, &handle));
    // Fill first page
    TEST_ESP_OK(nvs_set_blob(handle, "1a", blob, blob_size));
    // Fill second page
    TEST_ESP_OK(nvs_set_blob(handle, "2a", blob, blob_size));
    // Fill third page
    TEST_ESP_OK(nvs_set_blob(handle, "3a", blob, blob_size));
    TEST_ESP_OK(nvs_commit(handle));
    nvs_close(handle);
    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME));

    // first three pages are now full, fourth one is active, last one is empty
    // init custom with explicitly reducing the number of pages to 4 should fail
    TEST_ESP_ERR(nvs::NVSPartitionManager::get_instance()->init_custom(&h, 0, 4),
                 ESP_ERR_NVS_NO_FREE_PAGES);

    // in case this test fails, to not affect other tests
    nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME);
}

TEST_CASE("nvs page selection takes into account free entries also not just erased entries", "[nvs]")
{
    // TC verifies that potential next available NVS Page selection takes into account remaining free entries
    // in the partition, not just erased entries.
    // This test is to ensure that the NVS partition manager doesn't raise false not enough space errors
    // when there are enough free entries available in the partition, which is not at the moment the active page.

    // The test verifies the goal following way:
    // - The partition contains just three pages
    // - nvs_flash_init_partition initializes the partition with enough free entries
    // - nvs_set_blob writes enough data to fill the first page up to the point where the next item
    //   would not fit into the first page, but would fit into the second page.
    // - nvs_set_blob writes the next items which should completely fill the second page.
    // - nvs_set_blob writes the next item which should fit into the remaining space in the first page.

    // Erase the partition before testing
    TEST_ESP_OK(nvs_flash_erase_partition(TEST_3SEC_PARTITION_NAME));

    // Initialize NVS partition
    TEST_ESP_OK(nvs_flash_init_partition(TEST_3SEC_PARTITION_NAME));

    const size_t blob_size = nvs::Page::CHUNK_MAX_SIZE / 2;
    uint8_t blob[blob_size] = {0};
    nvs_handle_t handle;

    TEST_ESP_OK(nvs_open_from_partition(TEST_3SEC_PARTITION_NAME, "test", NVS_READWRITE, &handle));
    // Fill first page, leave some space for small entry, but not enough for the next full entry
    TEST_ESP_OK(nvs_set_blob(handle, "1a", blob, blob_size / 3));
    TEST_ESP_OK(nvs_set_blob(handle, "1b", blob, blob_size));
    // Activate and fill second page
    TEST_ESP_OK(nvs_set_blob(handle, "2a", blob, blob_size));
    TEST_ESP_OK(nvs_set_blob(handle, "2b", blob, blob_size));

    // The item below should be able to fit the remaining space in the first page.
    TEST_ESP_OK(nvs_set_blob(handle, "3a", blob, 4));
    TEST_ESP_OK(nvs_commit(handle));
    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_3SEC_PARTITION_NAME));
}

TEST_CASE("calculate used and free space", "[nvs]")
{
    // TC verifies that NVS API works as expected when calculating used and free space in the partition.
    // The test verifies following:
    // - nvs_get_stats returns the correct number of total, free, available and used entries
    //   in the partition.
    // - nvs_get_stats returns the correct number of namespaces in the partition
    // - nvs_get_used_entry_count returns the correct number of used entries in the namespace
    // - nvs_get_used_entry_count returns ESP_ERR_NVS_INVALID_HANDLE if the handle is invalid
    // - nvs_get_stats returns ESP_ERR_NVS_NOT_INITIALIZED if the partition is not initialized
    // - nvs_get_stats returns ESP_ERR_INVALID_ARG if the partition name is NULL
    // Expected values are documented in the comments below.

    // Erase the partition before testing
    TEST_ESP_OK(nvs_flash_erase_partition(TEST_DEFAULT_PARTITION_NAME));

    size_t consumed_entries = 0;

    // Make sure the partition is not initialized
    nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME);

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

    // Initialize NVS partition
    TEST_ESP_OK(nvs_flash_init_partition(TEST_DEFAULT_PARTITION_NAME));

    // Retrieve the partition size in nvs pages
    size_t total_sectors = NVSPartitionTestHelper::get_sectors(TEST_DEFAULT_PARTITION_NAME);
    CHECK(total_sectors > 0);

    TEST_ESP_ERR(nvs_get_used_entry_count(handle, &h_count_entries), ESP_ERR_NVS_INVALID_HANDLE);
    CHECK(h_count_entries == 0);

    // after erase. empty partition
    TEST_ESP_OK(nvs_get_stats(NULL, &stat1));
    CHECK(stat1.free_entries != 0);
    CHECK(stat1.namespace_count == 0);
    CHECK(stat1.total_entries == total_sectors * nvs::Page::ENTRY_COUNT);
    CHECK(stat1.used_entries == 0);

    // namespace test_k1
    nvs_handle_t handle_1;
    size_t ns1_expected_entries = 0;

    // create namespace
    consumed_entries = 1;   // should consume one entry
    TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, "test_k1", NVS_READWRITE, &handle_1));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat2));
    CHECK(stat2.free_entries + consumed_entries == stat1.free_entries);
    CHECK(stat2.namespace_count == 1);
    CHECK(stat2.total_entries == stat1.total_entries);
    CHECK(stat2.used_entries == stat1.used_entries + consumed_entries);
    CHECK(stat2.available_entries + consumed_entries == stat1.available_entries);

    // create pair key-value com
    consumed_entries = 1;   // should consume one entry
    TEST_ESP_OK(nvs_set_i32(handle_1, "com", 0x12345678));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat1));
    CHECK(stat1.free_entries + consumed_entries == stat2.free_entries);
    CHECK(stat1.namespace_count == 1);
    CHECK(stat1.total_entries == stat2.total_entries);
    CHECK(stat1.used_entries == stat2.used_entries + consumed_entries);
    CHECK(stat1.available_entries + consumed_entries == stat2.available_entries);
    ns1_expected_entries += consumed_entries;

    // change value in com
    consumed_entries = 0;   // should not consume any entry
    TEST_ESP_OK(nvs_set_i32(handle_1, "com", 0x01234567));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat2));
    CHECK(stat2.free_entries == stat1.free_entries);
    CHECK(stat2.namespace_count == 1);
    CHECK(stat2.total_entries != 0);
    CHECK(stat2.used_entries == stat1.used_entries + consumed_entries);
    CHECK(stat2.available_entries + consumed_entries == stat1.available_entries);
    ns1_expected_entries += consumed_entries;

    // create pair key-value ru
    consumed_entries = 1;   // should consume one entry
    TEST_ESP_OK(nvs_set_i32(handle_1, "ru", 0x00FF00FF));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat1));
    CHECK(stat1.free_entries + consumed_entries == stat2.free_entries);
    CHECK(stat1.namespace_count == 1);
    CHECK(stat1.total_entries != 0);
    CHECK(stat1.used_entries == stat2.used_entries + consumed_entries);
    CHECK(stat1.available_entries + consumed_entries == stat2.available_entries);
    ns1_expected_entries += consumed_entries;

    // amount of valid pairs in namespace 1
    size_t ns1_reported_entries;
    TEST_ESP_OK(nvs_get_used_entry_count(handle_1, &ns1_reported_entries));
    CHECK(ns1_reported_entries == ns1_expected_entries);

    // namespace test_k2
    nvs_handle_t handle_2;
    size_t ns2_expected_entries = 0;

    // create namespace
    consumed_entries = 1;   // should consume one entry
    TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, "test_k2", NVS_READWRITE, &handle_2));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat2));
    CHECK(stat2.free_entries + consumed_entries == stat1.free_entries);
    CHECK(stat2.namespace_count == 2);
    CHECK(stat2.total_entries == stat1.total_entries);
    CHECK(stat2.used_entries == stat1.used_entries + consumed_entries);
    CHECK(stat2.available_entries + consumed_entries == stat1.available_entries);

    // create 3 pairs key-value
    consumed_entries = 3;   // should consume three entries
    TEST_ESP_OK(nvs_set_i32(handle_2, "su1", 0x00000001));
    TEST_ESP_OK(nvs_set_i32(handle_2, "su2", 0x00000002));
    TEST_ESP_OK(nvs_set_i32(handle_2, "sus", 0x00000003));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat1));
    CHECK(stat1.free_entries + consumed_entries == stat2.free_entries);
    CHECK(stat1.namespace_count == 2);
    CHECK(stat1.total_entries == stat2.total_entries);
    CHECK(stat1.used_entries == stat2.used_entries + consumed_entries);
    CHECK(stat1.available_entries + consumed_entries == stat2.available_entries);
    ns2_expected_entries += consumed_entries;

    // amount of valid pairs in namespace 2
    size_t ns2_reported_entries;
    TEST_ESP_OK(nvs_get_used_entry_count(handle_2, &ns2_reported_entries));
    CHECK(ns2_reported_entries == ns2_expected_entries);

    CHECK(stat1.used_entries == (ns1_reported_entries + ns2_reported_entries + stat1.namespace_count));

    nvs_close(handle_1);
    nvs_close(handle_2);

    size_t temp = ns2_reported_entries;
    TEST_ESP_ERR(nvs_get_used_entry_count(handle_1, &ns2_reported_entries), ESP_ERR_NVS_INVALID_HANDLE);
    CHECK(ns2_reported_entries == 0);
    ns2_reported_entries = temp;
    TEST_ESP_ERR(nvs_get_used_entry_count(handle_1, NULL), ESP_ERR_INVALID_ARG);

    // namespace test_k3
    nvs_handle_t handle_3;
    size_t ns3_expected_entries = 0;

    // create namespace
    consumed_entries = 1;   // should consume one entry
    TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, "test_k3", NVS_READWRITE, &handle_3));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat2));
    CHECK(stat2.free_entries + consumed_entries == stat1.free_entries);
    CHECK(stat2.namespace_count == 3);
    CHECK(stat2.total_entries == stat1.total_entries);
    CHECK(stat2.used_entries == stat1.used_entries + consumed_entries);
    CHECK(stat2.available_entries + consumed_entries == stat1.available_entries);

    // create pair key - blob
    uint32_t blob[12];
    consumed_entries = 2 + (sizeof(blob) + 31) / 32;   // should consume 2 + entry for each started block of 32 bytes
    TEST_ESP_OK(nvs_set_blob(handle_3, "bl1", &blob, sizeof(blob)));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat1));
    CHECK(stat1.free_entries + consumed_entries == stat2.free_entries);
    CHECK(stat1.namespace_count == 3);
    CHECK(stat1.total_entries == stat2.total_entries);
    CHECK(stat1.used_entries == stat2.used_entries + consumed_entries);
    CHECK(stat1.available_entries + consumed_entries == stat2.available_entries);
    CHECK(stat1.total_entries == (stat1.used_entries + stat1.available_entries + nvs::Page::ENTRY_COUNT));
    ns3_expected_entries += consumed_entries;

    // create pair key - string
    char input_string[] = "abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789";
    consumed_entries = 1 + (strlen(input_string) + 1 + 31) / 32;   // should consume 1 + entry for each started block of 32 bytes
    TEST_ESP_OK(nvs_set_str(handle_3, "str1", input_string));
    TEST_ESP_OK(nvs_get_stats(NULL, &stat2));
    CHECK(stat2.free_entries + consumed_entries == stat1.free_entries);
    CHECK(stat2.namespace_count == 3);
    CHECK(stat2.total_entries == stat1.total_entries);
    CHECK(stat2.used_entries == stat1.used_entries + consumed_entries);
    CHECK(stat2.available_entries + consumed_entries == stat1.available_entries);
    ns3_expected_entries += consumed_entries;

    // amount of valid pairs in namespace 3
    size_t ns3_reported_entries;
    TEST_ESP_OK(nvs_get_used_entry_count(handle_3, &ns3_reported_entries));
    CHECK(ns3_reported_entries == ns3_expected_entries);

    // overall check of used entries across all namespaces
    CHECK(stat2.used_entries == (ns1_reported_entries + ns2_reported_entries + ns3_reported_entries + stat2.namespace_count));

    nvs_close(handle_3);

    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME));
}

TEST_CASE("Multi-page blobs are supported", "[nvs]")
{
    // TC verifies that NVS API works as expected when storing multi-page blobs.
    // The test verifies following:
    // - nvs_set_blob can store blobs larger than a single page

    // Erase the partition before testing
    TEST_ESP_OK(nvs_flash_erase_partition(TEST_DEFAULT_PARTITION_NAME));

    // Initialize NVS partition
    TEST_ESP_OK(nvs_flash_init_partition(TEST_DEFAULT_PARTITION_NAME));

    const size_t blob_size = nvs::Page::CHUNK_MAX_SIZE * 2;
    uint8_t blob[blob_size] = {0};

    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, "test", NVS_READWRITE, &handle));
    TEST_ESP_OK(nvs_set_blob(handle, "abc", blob, blob_size));
    TEST_ESP_OK(nvs_commit(handle));
    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME));
}

TEST_CASE("Failures are handled while storing multi-page blobs", "[nvs]")
{
    // TC verifies that NVS API works as expected when storing multi-page blobs.
    // The test verifies following:
    // - nvs_set_blob returns ESP_ERR_NVS_VALUE_TOO_LONG if the blob is larger than the maximum available space

    // Use NVSPartitionTestHelper to wrap the partition operations and allow custom initialization
    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);

    // Initialize NVS partition by using only 5 pages
    CHECK(h.get_sectors() >= 5);
    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(&h, 0, 5));

    // Requires at least 7 pages plus one entry for namespace and 1 + 7 metadata entries
    // for the blob. Cannot fit into 4 + 1 // pages.
    const size_t blob_size = nvs::Page::CHUNK_MAX_SIZE * 7;
    uint8_t blob[blob_size] = {0};

    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, "test", NVS_READWRITE, &handle));
    TEST_ESP_ERR(nvs_set_blob(handle, "abc", blob, blob_size), ESP_ERR_NVS_VALUE_TOO_LONG);
    TEST_ESP_OK(nvs_set_blob(handle, "abc", blob, nvs::Page::CHUNK_MAX_SIZE * 2));
    TEST_ESP_OK(nvs_commit(handle));
    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(h.get_partition_name()));
}

TEST_CASE("Reading multi-page blobs", "[nvs]")
{
    // TC verifies that NVS API works as expected when reading multi-page blobs.
    // The test verifies following:
    // - nvs_set blob can store blobs larger than a single page
    // - nvs_get_blob can read blobs larger than a single page

    // Erase the partition before testing
    TEST_ESP_OK(nvs_flash_erase_partition(TEST_DEFAULT_PARTITION_NAME));

    // Initialize NVS partition
    TEST_ESP_OK(nvs_flash_init_partition(TEST_DEFAULT_PARTITION_NAME));

    // Ensure that the partition has enough pages
    // 1 entry for namespace, 1 entry for blob metadata, 3 pages for blob data -> 4 + 1 (spare) NVS pages
    CHECK(NVSPartitionTestHelper::get_sectors(TEST_DEFAULT_PARTITION_NAME) >= 5);

    const size_t blob_size = nvs::Page::CHUNK_MAX_SIZE * 3;
    uint8_t blob[blob_size];
    uint8_t blob_read[blob_size];
    size_t read_size = blob_size;

    nvs_handle_t handle;
    memset(blob, 0x11, blob_size);
    memset(blob_read, 0xee, blob_size);
    TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, "readTest", NVS_READWRITE, &handle));
    TEST_ESP_OK(nvs_set_blob(handle, "abc", blob, blob_size));
    TEST_ESP_OK(nvs_get_blob(handle, "abc", blob_read, &read_size));
    CHECK(memcmp(blob, blob_read, blob_size) == 0);
    TEST_ESP_OK(nvs_commit(handle));
    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME));
}

TEST_CASE("Modification of values for Multi-page blobs are supported", "[nvs]")
{
    // TC verifies that NVS API works as expected when modifying multi-page blobs.
    // The test verifies following:
    // - nvs_set_blob can overwrite multi-page blobs with the same key
    // - nvs_get_blob can read the modified multi-page blob

    // Erase the partition before testing
    TEST_ESP_OK(nvs_flash_erase_partition(TEST_DEFAULT_PARTITION_NAME));

    // Initialize NVS partition
    TEST_ESP_OK(nvs_flash_init_partition(TEST_DEFAULT_PARTITION_NAME));

    // Ensure that the partition has enough NVS pages
    // Requires 1 namespace entry, 1 + 2 metadata entries for the blob, and 2 pages for blob data.
    // To be able to overwrite the blob with the same key, we need additional 1 + 2 metadata entries
    // and 2 pages for blob data.
    // It means 5 + 1 (spare) NVS pages are required to run this test.
    CHECK(NVSPartitionTestHelper::get_sectors(TEST_DEFAULT_PARTITION_NAME) >= 6);

    const size_t blob_size = nvs::Page::CHUNK_MAX_SIZE * 2;
    uint8_t blob[blob_size] = {0};
    uint8_t blob_read[blob_size] = {0xfe};;
    uint8_t blob2[blob_size] = {0x11};
    uint8_t blob3[blob_size] = {0x22};
    uint8_t blob4[blob_size] = { 0x33};
    size_t read_size = blob_size;

    nvs_handle_t handle;
    memset(blob, 0x11, blob_size);
    memset(blob2, 0x22, blob_size);
    memset(blob3, 0x33, blob_size);
    memset(blob4, 0x44, blob_size);
    memset(blob_read, 0xff, blob_size);
    TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, "test", NVS_READWRITE, &handle));
    TEST_ESP_OK(nvs_set_blob(handle, "abc", blob, blob_size));
    TEST_ESP_OK(nvs_set_blob(handle, "abc", blob2, blob_size));
    TEST_ESP_OK(nvs_set_blob(handle, "abc", blob3, blob_size));
    TEST_ESP_OK(nvs_set_blob(handle, "abc", blob4, blob_size));
    TEST_ESP_OK(nvs_get_blob(handle, "abc", blob_read, &read_size));
    CHECK(memcmp(blob4, blob_read, blob_size) == 0);
    TEST_ESP_OK(nvs_commit(handle));
    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME));
}

TEST_CASE("Modification from single page blob to multi-page", "[nvs]")
{
    // TC verifies that NVS API works as expected when modifying blob originally stored as single page blob to multi-page blob.
    // The test verifies following:
    // - nvs_set_blob can overwrite single page blob with a multi-page blob
    // - nvs_get_blob can read the modified multi-page blob

    // Erase the partition before testing
    TEST_ESP_OK(nvs_flash_erase_partition(TEST_DEFAULT_PARTITION_NAME));

    // Initialize NVS partition
    TEST_ESP_OK(nvs_flash_init_partition(TEST_DEFAULT_PARTITION_NAME));

    // Ensure that the partition has enough NVS pages
    // Requires 1 namespace entry, 1 + 1 metadata entries for the blob, and 1/2 of the page for the blob data.
    // To be able to overwrite the blob with the same key and data occupying 3 full NVS pages, we need
    // additional 1 + 3 metadata entries and 3 whole pages for blob data.
    // It means 4 + 1 (spare) NVS pages are required to run this test.
    CHECK(NVSPartitionTestHelper::get_sectors(TEST_DEFAULT_PARTITION_NAME) >= 5);


    const size_t blob_size = nvs::Page::CHUNK_MAX_SIZE * 3;
    uint8_t blob[blob_size] = {0};
    uint8_t blob_read[blob_size] = {0xff};
    size_t read_size = blob_size;

    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, "Test", NVS_READWRITE, &handle));
    TEST_ESP_OK(nvs_set_blob(handle, "abc", blob, nvs::Page::CHUNK_MAX_SIZE / 2));  // Set single page blob
    TEST_ESP_OK(nvs_set_blob(handle, "abc", blob, blob_size));
    TEST_ESP_OK(nvs_get_blob(handle, "abc", blob_read, &read_size));
    CHECK(memcmp(blob, blob_read, blob_size) == 0);
    TEST_ESP_OK(nvs_commit(handle));
    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME));
}

TEST_CASE("Modification from  multi-page to single page", "[nvs]")
{
    // TC verifies that NVS API works as expected when modifying blob originally stored as multi-page blob to single page blob.
    // The test verifies following:
    // - nvs_set_blob can overwrite multi-page blob with a single page blob
    // - nvs_get_blob can read the modified single page blob

    // Erase the partition before testing
    TEST_ESP_OK(nvs_flash_erase_partition(TEST_DEFAULT_PARTITION_NAME));

    // Initialize NVS partition
    TEST_ESP_OK(nvs_flash_init_partition(TEST_DEFAULT_PARTITION_NAME));
    // Ensure that the partition has enough NVS pages
    // Requires 1 namespace entry, 1 + 2 metadata entries for the blob,
    // and 3 pages for blob data. To be able to overwrite the blob with the
    // same key and data occupying 1/2 of the page, we need additional
    // 1 + 1 metadata entries and 1/2 of the page for blob data.
    // It means 4 + 1 (spare) NVS pages are required to run this test.
    CHECK(NVSPartitionTestHelper::get_sectors(TEST_DEFAULT_PARTITION_NAME) >= 5);

    const size_t blob_size = nvs::Page::CHUNK_MAX_SIZE * 3;
    uint8_t blob[blob_size] = {0};
    uint8_t blob_read[blob_size] = {0xff};
    size_t read_size = blob_size;

    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, "Test", NVS_READWRITE, &handle));
    TEST_ESP_OK(nvs_set_blob(handle, "abc", blob, blob_size));
    TEST_ESP_OK(nvs_set_blob(handle, "abc", blob, nvs::Page::CHUNK_MAX_SIZE / 2));
    TEST_ESP_OK(nvs_set_blob(handle, "abc2", blob, blob_size));
    TEST_ESP_OK(nvs_get_blob(handle, "abc", blob_read, &read_size));
    CHECK(memcmp(blob, blob_read, nvs::Page::CHUNK_MAX_SIZE) == 0);
    TEST_ESP_OK(nvs_commit(handle));
    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME));
}

TEST_CASE("Multi-page blob erased using nvs_erase_key should not be found when probed for just length", "[nvs]")
{
    // TC verifies that NVS API works as expected when erasing multi-page blobs.
    // The test verifies following:
    // - nvs_erase_key can erase multi-page blobs
    // - nvs_get_blob returns ESP_ERR_NVS_NOT_FOUND when probing for length of a multi-page blob that has been erased

    // Erase the partition before testing
    TEST_ESP_OK(nvs_flash_erase_partition(TEST_DEFAULT_PARTITION_NAME));

    // Initialize NVS partition
    TEST_ESP_OK(nvs_flash_init_partition(TEST_DEFAULT_PARTITION_NAME));
    // Ensure that the partition has enough NVS pages
    // Requires 1 namespace entry, 1 + 3 metadata entries for the blob,
    // and 3 pages for blob data.
    // It means 4 + 1 (spare) NVS pages are required to run this test.
    CHECK(NVSPartitionTestHelper::get_sectors(TEST_DEFAULT_PARTITION_NAME) >= 5);

    const size_t blob_size = nvs::Page::CHUNK_MAX_SIZE * 3;
    uint8_t blob[blob_size] = {0};
    size_t read_size = blob_size;

    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, "Test", NVS_READWRITE, &handle));
    TEST_ESP_OK(nvs_set_blob(handle, "abc", blob, blob_size));
    TEST_ESP_OK(nvs_erase_key(handle, "abc"));
    TEST_ESP_ERR(nvs_get_blob(handle, "abc", NULL, &read_size), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_OK(nvs_commit(handle));
    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME));
}

TEST_CASE("Check that orphaned blobs are erased during init", "[nvs]")
{
    // TC verifies that NVS API works as expected when initializing with orphaned blobs left in the partition.
    // The test verifies following:
    // - nvs_init can initialize the partition with orphaned blobs left in the partition
    // - nvs_init erases orphaned blobs during initialization

    // Use NVSPartitionTestHelper to provide nvs::Partition instance for the test
    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);
    const bool purgeAfterErase = true;

    // Make sure the partition has enough NVS pages
    // Requires 1 namespace entry, 1 + 3  metadata entries for the blob,
    // and 3 pages for blob data.
    // 4 + 1 (spare) NVS pages are required to run this test.
    const size_t required_sectors = 5;
    CHECK(NVSPartitionTestHelper::get_sectors(TEST_DEFAULT_PARTITION_NAME) >= required_sectors);

    const size_t blob_size = nvs::Page::CHUNK_MAX_SIZE * 3;
    uint8_t blob[blob_size] = {0x11};

    nvs::Storage storage(&h);

    TEST_ESP_OK(storage.init(0, required_sectors));

    TEST_ESP_OK(storage.writeItem(1, nvs::ItemType::BLOB, "key", blob, sizeof(blob), purgeAfterErase));

    TEST_ESP_OK(storage.init(0, required_sectors));
    // Check that multi-page item is still available.
    TEST_ESP_OK(storage.readItem(1, nvs::ItemType::BLOB, "key", blob, sizeof(blob)));

    TEST_ESP_ERR(storage.writeItem(1, nvs::ItemType::BLOB, "key2", blob, sizeof(blob), purgeAfterErase), ESP_ERR_NVS_NOT_ENOUGH_SPACE);

    nvs::Page p;
    TEST_ESP_OK(p.load(&h, 3)); // This is where index will be placed.
    TEST_ESP_OK(p.erase());

    TEST_ESP_OK(storage.init(0, required_sectors));

    TEST_ESP_ERR(storage.readItem(1, nvs::ItemType::BLOB, "key", blob, sizeof(blob)), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_OK(storage.writeItem(1, nvs::ItemType::BLOB, "key3", blob, sizeof(blob), purgeAfterErase));
}

TEST_CASE("nvs blob fragmentation test", "[nvs]")
{
    // TC verifies that NVS API works as expected when writing and updating large blobs.
    // The test verifies following:
    // - nvs_set_blob can write large blobs that span multiple pages
    // - nvs_set_blob can update large blobs that span multiple pages
    // - nvs_set_blob can handle fragmentation when updating large blobs

    // In each loop cycle, the test overwrites one u32 key and one blob key. On top of overwrites, it adds
    // one more additional u32 key to enforce fragmentation on subsequent updates.

    // Initialize NVS partition
    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);

    // We need at least 4 sectors to be able to run this test
    CHECK(h.get_sectors() >= 4);

    // Initialize NVS partition limited to 4 pages
    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(&h, 0, 4));

    // Blob definition
    const size_t BLOB_SIZE = 3500;
    uint8_t *blob = (uint8_t *) malloc(BLOB_SIZE);
    CHECK(blob != NULL);
    memset(blob, 0xEE, BLOB_SIZE);

    uint32_t magic = 0xff33eaeb;

    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, "blob_tests", NVS_READWRITE, &handle));

    for (int i = 0; i < 128; i++) {
        INFO("Iteration " << i << "...\n");

        // Modify the magic and the content of the buffer to force the value updates
        magic += i;
        blob[0] = (uint8_t)(i & 0xFF);

        // Update magic key
        TEST_ESP_OK(nvs_set_u32(handle, "magic", magic));

        // Update blob key
        TEST_ESP_OK(nvs_set_blob(handle, "blob", blob, BLOB_SIZE));

        // Create extra uint32 key to force record shifting on update and thus fragmentation
        char seq_buf[16];
        snprintf(seq_buf, sizeof(seq_buf), "seq%d", i);
        TEST_ESP_OK(nvs_set_u32(handle, seq_buf, i));
    }

    nvs_close(handle);
    free(blob);

    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME));
}

TEST_CASE("nvs code handles errors properly when partition is near to full", "[nvs]")
{
    // TC verifies that NVS API works as expected when the partition is near to full.
    // The test verifies following:
    // - nvs_writeItem returns ESP_ERR_NVS_NOT_ENOUGH_SPACE when trying to write an item that does not fit into the partition
    // - nvs_writeItem can write items that fit into the partition
    // - nvs_writeItem can write items that fit into the remaining space in the partition

    // Initialize NVS partition
    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);
    const bool purgeAfterErase = TEST_DEFAULT_PURGE_AFTER_ERASE;

    // This test requires at least 4+1 NVS pages to run.
    CHECK(h.get_sectors() >= 5);

    const size_t blob_size = nvs::Page::CHUNK_MAX_SIZE * 0.3 ;
    uint8_t blob[blob_size] = {0x11};

    nvs::Storage storage(&h);
    char nvs_key[16] = "";

    TEST_ESP_OK(storage.init(0, 5));

    // Four pages should fit roughly 12 blobs
    for (uint8_t count = 1; count <= 12; count++) {
        snprintf(nvs_key, sizeof(nvs_key), "key:%u", count);
        TEST_ESP_OK(storage.writeItem(1, nvs::ItemType::BLOB, nvs_key, blob, sizeof(blob), purgeAfterErase));
    }

    for (uint8_t count = 13; count <= 20; count++) {
        snprintf(nvs_key, sizeof(nvs_key), "key:%u", count);
        TEST_ESP_ERR(storage.writeItem(1, nvs::ItemType::BLOB, nvs_key, blob, sizeof(blob), purgeAfterErase), ESP_ERR_NVS_NOT_ENOUGH_SPACE);
    }
}

TEST_CASE("Check for nvs version incompatibility", "[nvs]")
{
    // TC verifies that NVS API works as expected when the partition has an old version of NVS.
    // The test verifies following:
    // - nvs_init returns ESP_ERR_NVS_NEW_VERSION_FOUND when the partition has an old version of NVS
    // - nvs_init does not initialize the partition with an old version of NVS

    // Initialize NVS partition
    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);

    // This test requires at least 3 NVS pages to run.
    CHECK(h.get_sectors() >= 3);

    int32_t val1 = 0x12345678;
    nvs::Page p;
    TEST_ESP_OK(p.load(&h, 0));
    TEST_ESP_OK(p.setVersion(nvs::Page::NVS_VERSION - 1));
    TEST_ESP_OK(p.writeItem(1, nvs::ItemType::I32, "foo", &val1, sizeof(val1)));

    TEST_ESP_ERR(nvs::NVSPartitionManager::get_instance()->init_custom(&h, 0, 3),
                 ESP_ERR_NVS_NEW_VERSION_FOUND);

    // if something went wrong, clean up
    nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME);
}

TEST_CASE("monkey test with old-format blob present", "[nvs][monkey][xxx]")
{
    // TC verifies that NVS API works as expected when the partition has an old-format blob present.
    // The test verifies following:
    // - nvs_init can initialize the partition with an old-format blob present
    // - NVS operations can handle write operations on other items with an old-format blob present
    // - If NVS updates the old-format blob, it should not leave both versions present in the partition

     // PartitionTestHelper is used to provide nvs::Partition instance for the test
    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);

    const uint32_t seed = 3;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 8;              // Minimal required NVS pages for this test

    uint32_t sector_count = h.get_sectors();

    // Make sure the partition has enough NVS pages
    CHECK(sector_count >= NVS_FLASH_SECTOR_COUNT_MIN);

    // Fill the partition with random data before testing
    TEST_ESP_OK(NVSPartitionTestHelper::randomize_partition(TEST_DEFAULT_PARTITION_NAME, seed));

    // Prepare random data generator
    std::random_device rd;
    std::mt19937 gen(rd());
    gen.seed(seed);

    // Clear partition stats before testing
    NVSPartitionTestHelper::clear_stats();
    const bool purgeAfterErase = true;

    static const size_t smallBlobLen = nvs::Page::CHUNK_MAX_SIZE / 3;

    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(&h,
                                                                      0,
                                                                      sector_count));

    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, "namespace1", NVS_READWRITE, &handle));
    RandomTest test;

    for (uint8_t it = 0; it < 10; it++) {
        size_t count = 200;

        // Erase index and chunks for the blob with "singlepage" key, do not care about errorcodes
        for (uint8_t num = 0; num < sector_count; num++) {
            nvs::Page p;
            TEST_ESP_OK(p.load(&h, num));
            p.eraseItem(1, nvs::ItemType::BLOB, "singlepage", purgeAfterErase, nvs::Item::CHUNK_ANY, nvs::VerOffset::VER_ANY);
            p.eraseItem(1, nvs::ItemType::BLOB_IDX, "singlepage", purgeAfterErase, nvs::Item::CHUNK_ANY, nvs::VerOffset::VER_ANY);
            p.eraseItem(1, nvs::ItemType::BLOB_DATA, "singlepage", purgeAfterErase, nvs::Item::CHUNK_ANY, nvs::VerOffset::VER_ANY);
        }

        // Now write "singlepage" blob in old format
        for (uint8_t num = 0; num < sector_count; num++) {
            nvs::Page p;
            TEST_ESP_OK(p.load(&h, num));
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
                TEST_ESP_OK(test.handleExternalWriteAtIndex(9, buf, blobLen)); // This assumes "singlepage" is always at index 9

                break;
            }
        }

        TEST_ESP_OK(nvs_flash_deinit_partition(h.get_partition_name()));
        // Initialize again
        TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(&h,
                                                                          0,
                                                                          sector_count));
        TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, "namespace1", NVS_READWRITE, &handle));

        // Perform random things
        auto res = test.doRandomThings(handle, gen, count);
        if (res != ESP_OK) {
            nvs_dump(TEST_DEFAULT_PARTITION_NAME);
            CHECK(0);
        }

        // Check that only one version is present for "singlepage". Its possible that last iteration did not write
        // anything for "singlepage". So either old version or new version should be present.
        bool oldVerPresent = false, newVerPresent = false;

        for (uint8_t num = 0; num < sector_count; num++) {
            nvs::Page p;
            TEST_ESP_OK(p.load(&h, num));
            if (!oldVerPresent && p.findItem(1, nvs::ItemType::BLOB, "singlepage", nvs::Item::CHUNK_ANY, nvs::VerOffset::VER_ANY) == ESP_OK) {
                oldVerPresent = true;
            }

            if (!newVerPresent && p.findItem(1, nvs::ItemType::BLOB_IDX, "singlepage", nvs::Item::CHUNK_ANY, nvs::VerOffset::VER_ANY) == ESP_OK) {
                newVerPresent = true;
            }
        }
        CHECK(oldVerPresent != newVerPresent);              // Only one version should be present, either old or new
        CHECK((oldVerPresent || newVerPresent) == true);    // At least one version should be present
    }

    TEST_ESP_OK(nvs_flash_deinit_partition(h.get_partition_name()));

    s_perf << "Monkey test: nErase=" << NVSPartitionTestHelper::get_erase_ops() << " nWrite=" << NVSPartitionTestHelper::get_write_ops() << std::endl;
}

TEST_CASE("Recovery from power-off during modification of blob present in old-format (same page)", "[nvs]")
{
    // TC verifies that NVS API works as expected when the power is lost during modification of a blob
    // that is present in the old-format.
    // The test verifies following:
    // - nvs_init can initialize the partition with an old-format blob present together with the updated blob value
    //   stored in new-format
    // - power-off in this test case means that the new-format blob is written, but the old-format blob is not removed
    // - nvs_init removes the old-format blob when the new-format blob is present
    // - nvs_get_blob can read the new format blob

    const uint32_t NVS_FLASH_SECTOR_COUNT = 3;

    // PartitionTestHelper is used to provide nvs::Partition instance for the test
    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);

    // This test requires at least 3 NVS pages to run.
    CHECK(h.get_sectors() >= NVS_FLASH_SECTOR_COUNT);

    // Clear partition stats before testing
    NVSPartitionTestHelper::clear_stats();

    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(&h, 0, NVS_FLASH_SECTOR_COUNT));

    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, "namespace1", NVS_READWRITE, &handle));

    uint8_t hexdata[] = {0x01, 0x02, 0x03, 0xab, 0xcd, 0xef};
    uint8_t hexdata_old[] = {0x11, 0x12, 0x13, 0xbb, 0xcc, 0xee};
    size_t buflen = sizeof(hexdata);
    uint8_t buf[nvs::Page::CHUNK_MAX_SIZE];

    nvs::Page p;
    TEST_ESP_OK(p.load(&h, 0));
    // Write blob in old-format
    TEST_ESP_OK(p.writeItem(1, nvs::ItemType::BLOB, "singlepage", hexdata_old, sizeof(hexdata_old)));

    // Write blob in new format
    // Write BLOB_DATA item
    TEST_ESP_OK(p.writeItem(1, nvs::ItemType::BLOB_DATA, "singlepage", hexdata, sizeof(hexdata), 0));
    // BLOB_DATA item is stored. Now store the index.
    nvs::Item item;
    item.blobIndex.dataSize = sizeof(hexdata);
    item.blobIndex.chunkCount = 1;
    item.blobIndex.chunkStart = nvs::VerOffset::VER_0_OFFSET;
    TEST_ESP_OK(p.writeItem(1, nvs::ItemType::BLOB_IDX, "singlepage", item.data, sizeof(item.data)));

    // We have written the new-format blob and its index after the old-format blob
    // So the old-format blob should still be present on the page
    TEST_ESP_OK(p.findItem(1, nvs::ItemType::BLOB, "singlepage"));

    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME));
    // Initialize again
    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(&h, 0, NVS_FLASH_SECTOR_COUNT));
    TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, "namespace1", NVS_READWRITE, &handle));

    // The blob should be present in the new format
    TEST_ESP_OK(nvs_get_blob(handle, "singlepage", buf, &buflen));
    CHECK(memcmp(buf, hexdata, buflen) == 0);

    // Low level check to ensure the old-format blob is not present at all
    nvs::Page p2;
    TEST_ESP_OK(p2.load(&h, 0));

    // After the power-off, the old-format blob should not be present
    // Search on the page for BLOB will find BLOB_INDEX and indicate the expected type mismatch
    TEST_ESP_ERR(p2.findItem(1, nvs::ItemType::BLOB, "singlepage"), ESP_ERR_NVS_TYPE_MISMATCH);

    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME));
}

TEST_CASE("Recovery from power-off during modification of blob present in old-format (different page)", "[nvs]")
{
    // TC verifies that NVS API works as expected when the power is lost during modification of a blob
    // that is present in the old-format.
    // The test verifies following:
    // - nvs_init can initialize the partition with an old-format blob present together with the updated blob value
    //   stored in new-format
    // - power-off in this test case means that the new-format blob is written, but the old-format blob is not removed
    // - nvs_init removes the old-format blob when the new-format blob is present
    // - nvs_get_blob can read the new format blob

    const uint32_t NVS_FLASH_SECTOR_COUNT = 3;

    // PartitionTestHelper is used to provide nvs::Partition instance for the test
    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);

    // This test requires at least 3 NVS pages to run.
    CHECK(h.get_sectors() >= NVS_FLASH_SECTOR_COUNT);

    // Clear partition stats before testing
    NVSPartitionTestHelper::clear_stats();

    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(&h, 0, NVS_FLASH_SECTOR_COUNT));

    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, "namespace1", NVS_READWRITE, &handle));

    uint8_t hexdata[] = {0x01, 0x02, 0x03, 0xab, 0xcd, 0xef};
    uint8_t hexdata_old[] = {0x11, 0x12, 0x13, 0xbb, 0xcc, 0xee};
    size_t buflen = sizeof(hexdata);
    uint8_t buf[nvs::Page::CHUNK_MAX_SIZE];

    // Power-off when blob was being written on the different page where its old version in old format
    // was present
    nvs::Page p;
    TEST_ESP_OK(p.load(&h, 0));
    // Write blob in old-format
    TEST_ESP_OK(p.writeItem(1, nvs::ItemType::BLOB, "singlepage", hexdata_old, sizeof(hexdata_old)));

    // Write blob in new format
    TEST_ESP_OK(p.writeItem(1, nvs::ItemType::BLOB_DATA, "singlepage", hexdata, sizeof(hexdata), 0));
    // All pages are stored. Now store the index, to the next page.
    nvs::Item item;
    item.blobIndex.dataSize = sizeof(hexdata);
    item.blobIndex.chunkCount = 1;
    item.blobIndex.chunkStart = nvs::VerOffset::VER_0_OFFSET;
    TEST_ESP_OK(p.markFull());
    nvs::Page p2;

    // p2 is loaded from the next page
    TEST_ESP_OK(p2.load(&h, 1));
    TEST_ESP_OK(p2.setSeqNumber(1));

    TEST_ESP_OK(p2.writeItem(1, nvs::ItemType::BLOB_IDX, "singlepage", item.data, sizeof(item.data)));

    // Old format blob should still be present on the first page
    TEST_ESP_OK(p.findItem(1, nvs::ItemType::BLOB, "singlepage"));

    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME));
    // Initialize again
    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(&h, 0, NVS_FLASH_SECTOR_COUNT));
    TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, "namespace1", NVS_READWRITE, &handle));

    // API should return the new format blob
    TEST_ESP_OK(nvs_get_blob(handle, "singlepage", buf, &buflen));
    CHECK(memcmp(buf, hexdata, buflen) == 0);

    // Low level check to ensure the old-format blob is not present at all
    // Search on the page for BLOB will find BLOB_INDEX and indicate the expected type mismatch
    // p3 is loaded from the first page, where old-format blob was present before initialization
    // but it should not be present after initialization as the new-format blob is present
    nvs::Page p3;
    TEST_ESP_OK(p3.load(&h, 0));
    TEST_ESP_ERR(p3.findItem(1, nvs::ItemType::BLOB, "singlepage"), ESP_ERR_NVS_NOT_FOUND);

    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME));
}

TEST_CASE("Page handles invalid CRC of variable length items", "[nvs][cur]")
{
    // TC verifies that NVS Page works as expected when the page has an invalid CRC for variable length items.
    // The test verifies following:
    // - NVS Page load does not crash when the NVS item has an invalid CRC

    const uint32_t NVS_FLASH_SECTOR_COUNT = 3;

    // PartitionTestHelper is used to provide nvs::Partition instance for the test
    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);

    // This test requires at least 3 NVS pages to run.
    CHECK(h.get_sectors() >= NVS_FLASH_SECTOR_COUNT);

    // Write the BLOB item
    nvs::Page p;
    TEST_ESP_OK(p.load(&h, 0));
    char buf[128] = {0};
    TEST_ESP_OK(p.writeItem(1, nvs::ItemType::BLOB, "1", buf, sizeof(buf)));

    // corrupt header of the item (64 is the offset of the first item in page)
    uint32_t overwrite_buf = 0;
    TEST_ESP_OK(h.write_raw(64, &overwrite_buf, sizeof(overwrite_buf)));

    // load page again
    nvs::Page p1;
    TEST_ESP_OK(p1.load(&h, 0));
}

TEST_CASE("namespace name is deep copy", "[nvs]")
{
    // TC verifies that NVS API works as expected when the namespace name is passed as a pointer.
    // The test verifies following:
    // - nvs_open can open a namespace with a name passed via a pointer
    // - nvs_open does not modify the namespace name passed as a pointer

    char ns_name[16];
    strcpy(ns_name, "const_name");

    nvs_handle_t handle_1;
    nvs_handle_t handle_2;

    TEST_ESP_OK(nvs_flash_erase_partition(TEST_3SEC_PARTITION_NAME));

    // Initialize NVS partition
    TEST_ESP_OK(nvs_flash_init_partition(TEST_3SEC_PARTITION_NAME));

    // Open (and create) a namespace with a name passed via a pointer
    TEST_ESP_OK(nvs_open_from_partition(TEST_3SEC_PARTITION_NAME, ns_name, NVS_READWRITE, &handle_1));

    // Modify the namespace name in the variable used as a parameter before
    strcpy(ns_name, "just_kidding");
    // Try to open the namespace using pointer to the same variable
    // This time intentionally in ReadOnly mode
    // NVS API should not find (and may not create) the namespace
    // because the name is not the same as the one used in nvs_open above
    CHECK(nvs_open_from_partition(TEST_3SEC_PARTITION_NAME, ns_name, NVS_READONLY, &handle_2) == ESP_ERR_NVS_NOT_FOUND);

    nvs_close(handle_1);
    nvs_close(handle_2);

    nvs_flash_deinit_partition(TEST_3SEC_PARTITION_NAME);
}

TEST_CASE("multiple partitions access check", "[nvs]")
{
    // TC verifies that NVS API works as expected when accessing multiple partitions.
    // The test verifies following:
    // - nvs_open_from_partition can open same namespace from different partitions
    // - nvs_set_i32 can write different values to the same key in different partitions
    // - nvs_get_i32 can read different values from the same key in different partitions
    // - nvs_flash_init_partition can initialize multiple partitions independently


    // Erase the partitions before testing
    TEST_ESP_OK(nvs_flash_erase_partition(TEST_DEFAULT_PARTITION_NAME));
    TEST_ESP_OK(nvs_flash_erase_partition(TEST_SECONDARY_PARTITION_NAME));

    // Initialize NVS partitions
    TEST_ESP_OK(nvs_flash_init_partition(TEST_DEFAULT_PARTITION_NAME));
    TEST_ESP_OK(nvs_flash_init_partition(TEST_SECONDARY_PARTITION_NAME));

    nvs_handle_t handle1, handle2;

    // Open namespaces in both partitions
    // and write different values to the same key in both partitions
    // Check that they are stored correctly
    // and can be read back correctly
    // This verifies that NVS can handle multiple partitions independently
    TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, "test", NVS_READWRITE, &handle1));
    TEST_ESP_OK(nvs_open_from_partition(TEST_SECONDARY_PARTITION_NAME, "test", NVS_READWRITE, &handle2));
    TEST_ESP_OK(nvs_set_i32(handle1, "foo", 0xdeadbeef));
    TEST_ESP_OK(nvs_set_i32(handle2, "foo", 0xcafebabe));
    int32_t v1, v2;
    TEST_ESP_OK(nvs_get_i32(handle1, "foo", &v1));
    TEST_ESP_OK(nvs_get_i32(handle2, "foo", &v2));
    CHECK(v1 == 0xdeadbeef);
    CHECK(v2 == 0xcafebabe);

    // Deinitialize partitions
    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME));
    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_SECONDARY_PARTITION_NAME));
}

TEST_CASE("writing the identical content does not write or erase", "[nvs]")
{
    // TC verifies that NVS API works as expected when writing the identical content.
    // The test verifies following:
    // - nvs_set_u8 does not write or erase when writing the same value
    // - nvs_set_str does not write or erase when writing the same string
    // - nvs_set_blob does not write or erase when writing the same blob

    // Erase the partition before testing
    TEST_ESP_OK(nvs_flash_erase_partition(TEST_DEFAULT_PARTITION_NAME));

    // Initialize NVS partition
    TEST_ESP_OK(nvs_flash_init_partition(TEST_DEFAULT_PARTITION_NAME));

    nvs_handle_t misc_handle;
    TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, "test", NVS_READWRITE, &misc_handle));

    // Test writing a u8 twice, then changing it
    nvs_set_u8(misc_handle, "test_u8", 8);
    NVSPartitionTestHelper::clear_stats();
    nvs_set_u8(misc_handle, "test_u8", 8);
    CHECK(NVSPartitionTestHelper::get_write_ops() == 0);
    CHECK(NVSPartitionTestHelper::get_erase_ops() == 0);
    CHECK(NVSPartitionTestHelper::get_read_ops()  != 0);
    NVSPartitionTestHelper::clear_stats();
    nvs_set_u8(misc_handle, "test_u8", 9);
    CHECK(NVSPartitionTestHelper::get_write_ops() != 0);
    CHECK(NVSPartitionTestHelper::get_read_ops()  != 0);

    // Test writing a string twice, then changing it
    static const char *test[2] = {"Hello world.", "Hello world!"};
    nvs_set_str(misc_handle, "test_str", test[0]);
    NVSPartitionTestHelper::clear_stats();
    nvs_set_str(misc_handle, "test_str", test[0]);
    CHECK(NVSPartitionTestHelper::get_write_ops() == 0);
    CHECK(NVSPartitionTestHelper::get_erase_ops() == 0);
    CHECK(NVSPartitionTestHelper::get_read_ops()  != 0);
    NVSPartitionTestHelper::clear_stats();
    nvs_set_str(misc_handle, "test_str", test[1]);
    CHECK(NVSPartitionTestHelper::get_write_ops() != 0);
    CHECK(NVSPartitionTestHelper::get_read_ops()  != 0);

    // Test writing a multi-page blob, then changing it
    uint8_t blob[nvs::Page::CHUNK_MAX_SIZE * 3] = {0};
    memset(blob, 1, sizeof(blob));
    nvs_set_blob(misc_handle, "test_blob", blob, sizeof(blob));
    NVSPartitionTestHelper::clear_stats();
    nvs_set_blob(misc_handle, "test_blob", blob, sizeof(blob));
    CHECK(NVSPartitionTestHelper::get_write_ops() == 0);
    CHECK(NVSPartitionTestHelper::get_erase_ops() == 0);
    CHECK(NVSPartitionTestHelper::get_read_ops()  != 0);
    blob[sizeof(blob) - 1]++;
    NVSPartitionTestHelper::clear_stats();
    nvs_set_blob(misc_handle, "test_blob", blob, sizeof(blob));
    CHECK(NVSPartitionTestHelper::get_write_ops() != 0);
    CHECK(NVSPartitionTestHelper::get_read_ops()  != 0);

    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME));
}

TEST_CASE("can init storage from flash with random contents", "[nvs]")
{
    // TC verifies that NVS API can be initialized from a partition with random contents.
    // The test verifies following:
    // - nvs_flash_init_partition can initialize the partition with random contents
    // - nvs_open_from_partition can open a namespace in the partition with random contents
    // - nvs_get_u8 reports the not found key as ESP_ERR_NVS_NOT_FOUND
    // - nvs_set_u8 can set a key in the partition with random contents

    // Randomize the partition contents
    TEST_ESP_OK(NVSPartitionTestHelper::randomize_partition(TEST_3SEC_PARTITION_NAME, 42));

    // Initialize NVS partition
    TEST_ESP_OK(nvs_flash_init_partition(TEST_3SEC_PARTITION_NAME));

    nvs_handle_t handle;

    // Open the namespace
    TEST_ESP_OK(nvs_open_from_partition(TEST_3SEC_PARTITION_NAME, "nvs.net80211", NVS_READWRITE, &handle));

    uint8_t opmode = 2;

    // Verify the key "wifi.opmode" is not found
    TEST_ESP_ERR(nvs_get_u8(handle, "wifi.opmode", &opmode), ESP_ERR_NVS_NOT_FOUND);

    // Set the key "wifi.opmode" to a value
    TEST_ESP_OK(nvs_set_u8(handle, "wifi.opmode", opmode));

    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_3SEC_PARTITION_NAME));
}

TEST_CASE("nvs api tests, starting with random data in flash", "[nvs][long][.]")
{
    // TC verifies that NVS API works as expected when the partition is initialized with random data.
    // The test verifies following:
    // - nvs_flash_init_partition can initialize the partition with random data
    // - nvs_open_from_partition can open a namespace in the partition with random data
    // - nvs_set_i32 can set a key in the initialized partition
    // - nvs_get_i32 can get a key from the initialized partition
    // - nvs_set_str can set a string in the initialized partition
    // - nvs_get_str can get a string from the initialized partition
    // - two independen t namespaces can be created and used in the same partition

    // Number of iterations for the test
    const size_t testIters = 3000;

    int lastPercent = -1;
    for (size_t count = 0; count < testIters; ++count) {
        // Progress reporting
        int percentDone = (int)(count * 100 / testIters);
        if (percentDone != lastPercent) {
            lastPercent = percentDone;
            printf("%d%%\n", percentDone);
        }

        // Randomize the partition contents, the random seed is based on the iteration cycle
        TEST_ESP_OK(NVSPartitionTestHelper::randomize_partition(TEST_DEFAULT_PARTITION_NAME, static_cast<uint32_t>(count))) ;

        // Initialize NVS partition
        TEST_ESP_OK(nvs_flash_init_partition(TEST_DEFAULT_PARTITION_NAME));

        nvs_handle_t handle_1;
        // Readonly open should fail, as the namespace does not exist yet
        TEST_ESP_ERR(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, "namespace1", NVS_READONLY, &handle_1), ESP_ERR_NVS_NOT_FOUND);

        // Open the namespace in read-write mode, this should create the namespace
        // and allow writing to it
        TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, "namespace1", NVS_READWRITE, &handle_1));

        // Set a key in the first namespace
        // This should succeed and create the key in the namespace
        TEST_ESP_OK(nvs_set_i32(handle_1, "foo", 0x12345678));
        for (size_t i = 0; i < 500; ++i) {
            nvs_handle_t handle_2;

            // Open the second namespace, this should create it
            // and allow writing to it
            TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, "namespace2", NVS_READWRITE, &handle_2));
            // Set a key in the second namespace
            TEST_ESP_OK(nvs_set_i32(handle_1, "foo", 0x23456789 % (i + 1)));
            // Overwrite the key in the second namespace
            TEST_ESP_OK(nvs_set_i32(handle_2, "foo", static_cast<int32_t>(i)));
            const char *str = "value 0123456789abcdef0123456789abcdef %09d";
            char str_buf[128];
            snprintf(str_buf, sizeof(str_buf), str, i + count * 1024);
            // Set a string in the second namespace
            TEST_ESP_OK(nvs_set_str(handle_2, "key", str_buf));

            int32_t v1;
            // Get the key from the first namespace
            TEST_ESP_OK(nvs_get_i32(handle_1, "foo", &v1));
            CHECK(0x23456789 % (i + 1) == v1);

            int32_t v2;
            // Get the key from the second namespace
            TEST_ESP_OK(nvs_get_i32(handle_2, "foo", &v2));
            CHECK(static_cast<int32_t>(i) == v2);

            char buf[128];
            size_t buf_len = sizeof(buf);
            // Get the string from the second namespace
            TEST_ESP_OK(nvs_get_str(handle_2, "key", buf, &buf_len));

            CHECK(0 == strcmp(buf, str_buf));
            nvs_close(handle_2);
        }
        nvs_close(handle_1);

        TEST_ESP_OK(nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME));
    }
}

TEST_CASE("test recovery from sudden poweroff", "[long][nvs][recovery][monkey][.]")
{
    // TC verifies that NVS API works as expected when the power is lost during random operations.
    // The test verifies following:
    // - nvs_flash_init_partition can initialize the partition with random contents
    // - nvs_open_from_partition can open a namespace in the partition with random contents
    // - random operation using nvs API can be performed
    // - once the power-off emulation is triggered, the NVS can be re-initialized and
    //   all random operations can be can be performed without errors

    // Note: This test is designed to run for a long time, and it takes more than one hour to complete.

    // Random device setup
    std::random_device rd;
    std::mt19937 gen(rd());
    uint32_t seed = 3;
    gen.seed(seed);

    // Set the number of iterations for the test
    const size_t iter_count = 2000;

    // Total operations counter holding the number of erase and 1/4 write operations performed
    size_t totalOps = 0;

    // Counter for progress reporting
    int lastPercent = -1;

    // Main loop counter for the test
    // It controls the number of operations before the next power-off simulation
    uint32_t errDelay;

    for (errDelay = 0; ; ++errDelay) {
        INFO(errDelay);

        // Randomize the partition contents
        TEST_ESP_OK(NVSPartitionTestHelper::randomize_partition(TEST_DEFAULT_PARTITION_NAME, seed));

        NVSPartitionTestHelper::clear_stats();

        // Step 1 of the cycle is to initialize the power-off emulation
        NVSPartitionTestHelper::fail_after(errDelay, ESP_PARTITION_FAIL_AFTER_MODE_BOTH);
        RandomTest test;

        // Report progress
        if (totalOps != 0) {
            int percent = errDelay * 100 / totalOps;

            // Report progress every increment by 1%
            if (percent > lastPercent) {
                printf("%d/%d (%d%%)\r\n", errDelay, static_cast<int>(totalOps), percent);
                lastPercent = percent;
            }
        }

        nvs_handle_t handle;
        size_t count = iter_count;      // We need local variable for count, as it is referenced in the test.doRandomThings() method

        // Step 2 tries to perform the random operations
        // As the moment of the power-off is driven by the errDelay which gradually increases,
        // it is expected that the test will fail continuously until the errDelay is high enough
        // to not trigger the power-off emulation.
        // We are observing the result of the test.doRandomThings() method to determine if the power-off emulation was triggered.
        // Once we reach the point where the errDelay is high enough to not trigger the power-off emulation,
        // the loop will break and the test will finish successfully.
        if(nvs_flash_init_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK)
        {
            // If the partition was initialized successfully, we can proceed with the test
            esp_err_t res = ESP_ERR_FLASH_OP_FAIL;
            if (nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, "namespace1", NVS_READWRITE, &handle) == ESP_OK) {
                res = test.doRandomThings(handle, gen, count, errDelay);
                nvs_close(handle);
            }

            TEST_ESP_OK(nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME));

            // As the res was previously set to ESP_ERR_FLASH_OP_FAIL, we check if it was changed
            // If the res remains the same, it means that the test encountered and emulated power-off and we can
            if (res != ESP_ERR_FLASH_OP_FAIL) {
                // This means we got to the end without an error due to NVSPartitionTestHelper::fail_after(), therefore errDelay
                // is high enough that we're not triggering it any more, therefore we're done
                break;
            }
        }

        // Step 3 tries to recover the partition after the power-off emulation
        // Note: the power-off won't trigger at this point, as it is deactiavated after the first occurrence
        // of the ESP_ERR_FLASH_OP_FAIL error.
        TEST_ESP_OK(nvs_flash_init_partition(TEST_DEFAULT_PARTITION_NAME));

        TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, "namespace1", NVS_READWRITE, &handle));

        esp_err_t res;
        res = test.doRandomThings(handle, gen, count, errDelay);

        // We expect the test.doRandomThings() to return ESP_OK at this point,
        // If not, we dump the partition contents and fail the test
        if (res != ESP_OK) {
            nvs_dump(TEST_DEFAULT_PARTITION_NAME);
            CHECK(0);
        }
        nvs_close(handle);
        totalOps = NVSPartitionTestHelper::get_erase_ops() + NVSPartitionTestHelper::get_write_bytes() / 4;

        TEST_ESP_OK(nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME));
    }
}

TEST_CASE("duplicate items are removed", "[nvs][dupes]")
{
    // TC verifies that NVS API works as expected when duplicate items are present.
    // The test verifies following:
    // - nvs::Page can handle duplicate items
    // - nvs::Storage can handle duplicate items
    // - nvs::Storage can read the correct value after duplicates are removed
    // - nvs::Page can report the correct number of used and erased entries after duplicates are removed

    // PartitionTestHelper is used to provide nvs::Partition instance for the test
    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);
    {
        // create one item
        nvs::Page p;
        TEST_ESP_OK(p.load(&h, 0));
        TEST_ESP_OK(p.writeItem<uint8_t>(1, "opmode", 3));
    }
    {
        // add another two without deleting the first one
        nvs::Item item(1, nvs::ItemType::U8, 1, "opmode");
        item.data[0] = 2;
        item.crc32 = item.calculateCrc32();

        TEST_ESP_OK(h.write_raw(3 * 32, reinterpret_cast<const uint32_t *>(&item), sizeof(item)));
        TEST_ESP_OK(h.write_raw(4 * 32, reinterpret_cast<const uint32_t *>(&item), sizeof(item)));

        uint32_t mask = 0xFFFFFFEA;

        TEST_ESP_OK(h.write_raw(32, &mask, 4));
    }
    {
        // load page and check that second item persists
        nvs::Storage s(&h);
        TEST_ESP_OK(s.init(0, 3));
        uint8_t val;
        ESP_ERROR_CHECK(s.readItem(1, "opmode", val));
        CHECK(val == 2);
    }
    {
        nvs::Page p;
        TEST_ESP_OK(p.load(&h, 0));
        CHECK(p.getErasedEntryCount() == 2);
        CHECK(p.getUsedEntryCount() == 1);
    }
}

TEST_CASE("recovery after failure to write data", "[nvs]")
{   // TC verifies that NVS API works as expected when the power is lost during writing data.
    // The test verifies following:
    // - nvs::Storage can recover from a failure to write data
    // - nvs::Page can handle the situation when the data is not written correctly
    // - nvs::Storage can read the correct value after recovery
    // - nvs::Page can report the correct number of used and erased entries after recovery

    // PartitionTestHelper is used to provide nvs::Partition instance for the test
    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);
    const bool purgeAfterErase = TEST_DEFAULT_PURGE_AFTER_ERASE;

    const char str[] = "value 0123456789abcdef012345678value 0123456789abcdef012345678";

    // make flash write fail exactly in nvs::Page::writeEntryData
    NVSPartitionTestHelper::fail_after(17, ESP_PARTITION_FAIL_AFTER_MODE_BOTH);
    {
        nvs::Storage storage(&h);
        TEST_ESP_OK(storage.init(0, 3));

        TEST_ESP_ERR(storage.writeItem(1, nvs::ItemType::SZ, "key", str, strlen(str), purgeAfterErase), ESP_ERR_FLASH_OP_FAIL);

        // check that repeated operations cause an error
        TEST_ESP_ERR(storage.writeItem(1, nvs::ItemType::SZ, "key", str, strlen(str), purgeAfterErase), ESP_ERR_NVS_INVALID_STATE);

        uint8_t val;
        TEST_ESP_ERR(storage.readItem(1, nvs::ItemType::U8, "key", &val, sizeof(val)), ESP_ERR_NVS_NOT_FOUND);
    }
    {
        // load page and check that data was erased
        nvs::Page p;
        TEST_ESP_OK(p.load(&h, 0));
        CHECK(p.getErasedEntryCount() == 3);
        CHECK(p.getUsedEntryCount() == 0);

        // try to write again
        TEST_ESP_OK(p.writeItem(1, nvs::ItemType::SZ, "key", str, strlen(str)));
    }
}

TEST_CASE("crc errors in item header are handled", "[nvs]")
{
    // TC verifies that NVS API works as expected when the item header has a CRC error.
    // The test verifies following:
    // - nvs::Storage can recover from a CRC error in the item header
    // - nvs::Page can handle the situation when the item header has a CRC error
    // - nvs::Storage can recover from a CRC error in the item header on a full page
    // - nvs::Storage can read the correct value after recovery

    // PartitionTestHelper is used to provide nvs::Partition instance for the test
    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);
    const bool purgeAfterErase = TEST_DEFAULT_PURGE_AFTER_ERASE;

    nvs::Storage storage(&h);
    // prepare some data
    TEST_ESP_OK(storage.init(0, 3));
    TEST_ESP_OK(storage.writeItem(0, "ns1", static_cast<uint8_t>(1), purgeAfterErase));
    TEST_ESP_OK(storage.writeItem(1, "value1", static_cast<uint32_t>(1), purgeAfterErase));
    TEST_ESP_OK(storage.writeItem(1, "value2", static_cast<uint32_t>(2), purgeAfterErase));

    // corrupt item header
    uint32_t val = 0;
    TEST_ESP_OK(h.write_raw(32 * 3 + 4, &val, 4)); // overwrite crc32

    // check that storage can recover
    TEST_ESP_OK(storage.init(0, 3));
    TEST_ESP_OK(storage.readItem(1, "value2", val));
    CHECK(val == 2);
    // check that the corrupted item is no longer present
    TEST_ESP_ERR(ESP_ERR_NVS_NOT_FOUND, storage.readItem(1, "value1", val));

    // add more items to make the page full
    for (size_t i = 0; i < nvs::Page::ENTRY_COUNT; ++i) {
        char item_name[nvs::Item::MAX_KEY_LENGTH + 1];
        snprintf(item_name, sizeof(item_name), "item_%ld", (long int)i);
        TEST_ESP_OK(storage.writeItem(1, item_name, static_cast<uint32_t>(i), purgeAfterErase));
    }

    // corrupt another item on the full page
    val = 0;
    TEST_ESP_OK(h.write_raw(32 * 4, &val, 4));

    // check that storage can recover
    TEST_ESP_OK(storage.init(0, 3));
    // check that the corrupted item is no longer present
    TEST_ESP_ERR(ESP_ERR_NVS_NOT_FOUND, storage.readItem(1, "value2", val));
}

TEST_CASE("crc error in variable length item is handled", "[nvs]")
{
    // TC verifies that NVS API works as expected when the variable length item has a CRC error.
    // The test verifies following:
    // - nvs::Page can handle the situation when the variable length item has a CRC error
    // - nvs::Page can report the correct number of used and erased entries after recovery
    // - nvs::Page can read the correct value after recovery

    // PartitionTestHelper is used to provide nvs::Partition instance for the test
    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);

    const uint64_t before_val = 0xbef04e;
    const uint64_t after_val = 0xaf7e4;
    // write some data
    {
        nvs::Page p;
        TEST_ESP_OK(p.load(&h, 0));
        TEST_ESP_OK(p.writeItem<uint64_t>(0, "before", before_val));
        const char *str = "foobar";
        TEST_ESP_OK(p.writeItem(0, nvs::ItemType::SZ, "key", str, strlen(str)));
        TEST_ESP_OK(p.writeItem<uint64_t>(0, "after", after_val));
    }
    // corrupt some data
    uint32_t w;
    TEST_ESP_OK(h.read_raw(32 * 3 + 8, &w, sizeof(w)));
    w &= 0xf000000f;
    TEST_ESP_OK(h.write_raw(32 * 3 + 8, &w, sizeof(w)));
    // load and check
    {
        nvs::Page p;
        TEST_ESP_OK(p.load(&h, 0));
        CHECK(p.getUsedEntryCount() == 2);
        CHECK(p.getErasedEntryCount() == 2);

        uint64_t val;
        TEST_ESP_OK(p.readItem<uint64_t>(0, "before", val));
        CHECK(val == before_val);
        TEST_ESP_ERR(p.findItem(0, nvs::ItemType::SZ, "key"), ESP_ERR_NVS_NOT_FOUND);
        TEST_ESP_OK(p.readItem<uint64_t>(0, "after", val));
        CHECK(val == after_val);
    }
}

// handle damaged item header's span=0 even if crc is correct
TEST_CASE("zero span in item header with correct crc is handled", "[nvs]")
{
    // TC verifies that NVS API works as expected when the item header has span=0.
    // The test verifies following:
    // - nvs::Storage can recover from a span=0 error in the item header on a non-full page
    // - nvs::Storage can recover from a span=0 error in the item header on full page
    // - nvs::Storage reports non-existing removed item as ESP_ERR_NVS_NOT_FOUND

    // PartitionTestHelper is used to provide nvs::Partition instance for the test
    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);
    const bool purgeAfterErase = TEST_DEFAULT_PURGE_AFTER_ERASE;

    nvs::Storage storage(&h);
    // prepare some data
    TEST_ESP_OK(storage.init(0, 3));
    TEST_ESP_OK(storage.writeItem(0, "ns1", static_cast<uint8_t>(1), purgeAfterErase));
    TEST_ESP_OK(storage.writeItem(1, "value1", static_cast<uint32_t>(1), purgeAfterErase));
    TEST_ESP_OK(storage.writeItem(1, "value2", static_cast<uint32_t>(2), purgeAfterErase));

    // damage item header of value1 to introduce span==0 error, recalculate crc
    {
        uint8_t new_span = 0;
        size_t entry_offset = 32 * 3; // 2x page header + 1x ns1
        uint8_t buff[sizeof(nvs::Item)] = {0};
        TEST_ESP_OK(h.read_raw(entry_offset, &buff, sizeof(buff)));

        nvs::Item* item = (nvs::Item*)buff;

        // set span to 0
        item->span = new_span;
        // recalculate crc same way as nvs::Item::calculateCrc32 does
        item->crc32 = ((nvs::Item*)buff)->calculateCrc32();

        // allow erase smaller portion of flash than whole 4k page
        TEST_ESP_OK(h.set_erase_size(1));
        TEST_ESP_OK(h.erase_range(entry_offset, sizeof(buff)));
        TEST_ESP_OK(h.write_raw(entry_offset, &buff, sizeof(buff)));
    }

    // check that storage can recover
    uint32_t val = 0;

    TEST_ESP_OK(storage.init(0, 3));
    TEST_ESP_OK(storage.readItem(1, "value2", val));
    CHECK(val == 2);
    // check that the damaged item is no longer present
    TEST_ESP_ERR(ESP_ERR_NVS_NOT_FOUND, storage.readItem(1, "value1", val));

    // add more items named "item_0" till "item_125" to make the page full
    for (size_t i = 0; i < nvs::Page::ENTRY_COUNT; ++i) {
        char item_name[nvs::Item::MAX_KEY_LENGTH + 1];
        snprintf(item_name, sizeof(item_name), "item_%ld", (long int)i);
        TEST_ESP_OK(storage.writeItem(1, item_name, static_cast<uint32_t>(i), purgeAfterErase));
    }

    // damage item header of item_125 to introduce span==0 error, recalculate crc
    {
        uint8_t new_span = 0;
        size_t entry_offset = 32 * (2 + 1 + 1 + 128) ; // 2x page header + 1x ns1 + 1x value1 + whole page
        uint8_t buff[sizeof(nvs::Item)] = {0};
        TEST_ESP_OK(h.read_raw(entry_offset, &buff, sizeof(buff)));

        nvs::Item* item = (nvs::Item*)buff;
        // set span to 0
        item->span = new_span;
        // recalculate crc same way as nvs::Item::calculateCrc32 does
        item->crc32 = ((nvs::Item*)buff)->calculateCrc32();

        // allow erase smaller portion of flash than whole 4k page
        TEST_ESP_OK(h.set_erase_size(1));
        TEST_ESP_OK(h.erase_range(entry_offset, sizeof(buff)));
        TEST_ESP_OK(h.write_raw(entry_offset, &buff, sizeof(buff)));
    }

    // check that storage can recover
    TEST_ESP_OK(storage.init(0, 3));
    // check that the damaged item is no longer present
    TEST_ESP_ERR(ESP_ERR_NVS_NOT_FOUND, storage.readItem(1, "item_125", val));
}


TEST_CASE("inconsistent fields in item header with correct crc are handled for string", "[nvs]")
{
    // TC testing damaged item header with correct crc using string.
    // first section, span goes over the remaining entries in the page
    // second section, span goes over the number of entries required to store the string
    // third section, span goes below the number of entries required to store the string
    // fourth section, indicated variable data length goes over the remaining space in the page
    // After the sections, the common evaluation is done to check that the storage can recover from the damaged item header.

    // PartitionTestHelper is used to provide nvs::Partition instance for the test
    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);
    const bool purgeAfterErase = TEST_DEFAULT_PURGE_AFTER_ERASE;

    nvs::Storage storage(&h);
    // prepare some data
    TEST_ESP_OK(storage.init(0, 3));
    TEST_ESP_OK(storage.writeItem(0, "ns1", static_cast<uint8_t>(1), purgeAfterErase));
    const char str[] = "String67890123456789012345678901String67890123456789012345678901String6789012345678901234567890"; // 95 + 1 bytes data occupy 3 entries, overhead 1

    TEST_ESP_OK(storage.writeItem(1, nvs::ItemType::SZ, "valuestr1", str, strlen(str), purgeAfterErase));
    TEST_ESP_OK(storage.writeItem(1, nvs::ItemType::SZ, "valuestr2", str, strlen(str), purgeAfterErase));
    TEST_ESP_OK(storage.writeItem(1, nvs::ItemType::SZ, "valuestr3", str, strlen(str), purgeAfterErase));
    TEST_ESP_OK(storage.writeItem(1, nvs::ItemType::SZ, "valuestr4", str, strlen(str), purgeAfterErase));
    TEST_ESP_OK(storage.writeItem(1, "valueu32", static_cast<uint32_t>(2), purgeAfterErase));

    // read the values back
    char read_str[sizeof(str)] = {0};
    size_t read_str_size = sizeof(read_str);
    uint32_t val = 0;

    TEST_ESP_OK(storage.readItem(1, nvs::ItemType::SZ, "valuestr1", (void*)read_str, read_str_size));
    CHECK(strcmp(read_str, str) == 0);
    TEST_ESP_OK(storage.readItem(1, nvs::ItemType::SZ, "valuestr2", (void*)read_str, read_str_size));
    CHECK(strcmp(read_str, str) == 0);
    TEST_ESP_OK(storage.readItem(1, nvs::ItemType::SZ, "valuestr3", (void*)read_str, read_str_size));
    CHECK(strcmp(read_str, str) == 0);
    TEST_ESP_OK(storage.readItem(1, nvs::ItemType::SZ, "valuestr4", (void*)read_str, read_str_size));
    CHECK(strcmp(read_str, str) == 0);

    // default values for re-check after sections of the test
    esp_err_t exp_err_str1 = ESP_OK;
    esp_err_t exp_err_str2 = ESP_OK;
    esp_err_t exp_err_str3 = ESP_OK;
    esp_err_t exp_err_str4 = ESP_OK;

    TEST_ESP_OK(storage.readItem(1, "valueu32", val));
    CHECK(val == 2);

    // allow overwriting smaller portion of flash than whole 4k page
    h.set_erase_size(1);

    SECTION("damage item header of valuestr1 to introduce span exceeding remaining page size error") {
        // span of the valstr1 is 4, there are 126 - 1  = 125 entries left in the page, set damaged span to 126
        uint8_t new_span = 126;

        const uint8_t entry_index = 3; // 2x page header + 1x ns1

        size_t entry_offset = 32 * entry_index;
        uint8_t buff[sizeof(nvs::Item)] = {0};
        TEST_ESP_OK(h.read_raw(entry_offset, &buff, sizeof(buff)));

        nvs::Item* item = (nvs::Item*)buff;

        item->span = new_span;
        // recalculate crc same way as nvs::Item::calculateCrc32 does
        item->crc32 = ((nvs::Item*)buff)->calculateCrc32();

        TEST_ESP_OK(h.erase_range(entry_offset, sizeof(buff)));
        TEST_ESP_OK(h.write_raw(entry_offset, &buff, sizeof(buff)));

        // expect error when trying to read the item
        exp_err_str1 = ESP_ERR_NVS_NOT_FOUND;
    }

    SECTION("damage item header of valuestr2 to introduce span exceeding the number of entries required to store the string") {
        // span of the valstr2 is 4, there are 126 - (1 + 4)  = 121 entries left in the page, set damaged span to value below 121 and above 4
        uint8_t new_span = 10;

        const uint8_t entry_index = 7; // 2x page header + 1x ns1 + 4x valuestr1

        size_t entry_offset = 32 * entry_index;
        uint8_t buff[sizeof(nvs::Item)] = {0};
        TEST_ESP_OK(h.read_raw(entry_offset, &buff, sizeof(buff)));

        nvs::Item* item = (nvs::Item*)buff;

        item->span = new_span;
        // recalculate crc same way as nvs::Item::calculateCrc32 does
        item->crc32 = ((nvs::Item*)buff)->calculateCrc32();

        TEST_ESP_OK(h.erase_range(entry_offset, sizeof(buff)));
        TEST_ESP_OK(h.write_raw(entry_offset, &buff, sizeof(buff)));

        // expect error when trying to read the item
        exp_err_str2 = ESP_ERR_NVS_NOT_FOUND;
    }

    SECTION("damage item header of valuestr3 to introduce span lower than the number of entries required to store the string") {
        // span of the valstr3 is 4, set to 3
        uint8_t new_span = 3;

        const uint8_t entry_index = 11; // 2x page header + 1x ns1 + 4x valuestr1 + 4x valuestr2

        size_t entry_offset = 32 * entry_index;
        uint8_t buff[sizeof(nvs::Item)] = {0};
        TEST_ESP_OK(h.read_raw(entry_offset, &buff, sizeof(buff)));

        nvs::Item* item = (nvs::Item*)buff;

        item->span = new_span;
        // recalculate crc same way as nvs::Item::calculateCrc32 does
        item->crc32 = ((nvs::Item*)buff)->calculateCrc32();

        TEST_ESP_OK(h.erase_range(entry_offset, sizeof(buff)));
        TEST_ESP_OK(h.write_raw(entry_offset, &buff, sizeof(buff)));

        // expect error when trying to read the item
        exp_err_str3 = ESP_ERR_NVS_NOT_FOUND;
    }

    SECTION("damage item header of valuestr4 to introduce indicated variable data length going over the remaining space in the page") {
        const uint8_t entry_index = 15; // 2x page header + 1x ns1 + 4x valuestr1 + 4x valuestr2 + 4x valuestr3

        size_t entry_offset = 32 * entry_index;
        // we are at entry 15, there are 126 - 15 = 111 entries left in the page, set data size to 112 * 32 bytes
        uint16_t new_size = 112 * 32;

        uint8_t buff[sizeof(nvs::Item)] = {0};
        TEST_ESP_OK(h.read_raw(entry_offset, &buff, sizeof(buff)));

        nvs::Item* item = (nvs::Item*)buff;

        item->varLength.dataSize = new_size;
        // recalculate crc same way as nvs::Item::calculateCrc32 does
        item->crc32 = ((nvs::Item*)buff)->calculateCrc32();

        TEST_ESP_OK(h.erase_range(entry_offset, sizeof(buff)));
        TEST_ESP_OK(h.write_raw(entry_offset, &buff, sizeof(buff)));

        // expect error when trying to read the item
        exp_err_str4 = ESP_ERR_NVS_NOT_FOUND;
    }

    // check that storage can recover

    TEST_ESP_OK(storage.init(0, 3));
    TEST_ESP_OK(storage.readItem(1, "valueu32", val));
    CHECK(val == 2);

    // check that the damaged items are no longer present
    char read_buff[sizeof(str)];
    TEST_ESP_ERR(exp_err_str1, storage.readItem(1, nvs::ItemType::SZ, "valuestr1", read_buff, sizeof(read_buff)));
    TEST_ESP_ERR(exp_err_str2, storage.readItem(1, nvs::ItemType::SZ, "valuestr2", read_buff, sizeof(read_buff)));
    TEST_ESP_ERR(exp_err_str3, storage.readItem(1, nvs::ItemType::SZ, "valuestr3", read_buff, sizeof(read_buff)));
    TEST_ESP_ERR(exp_err_str4, storage.readItem(1, nvs::ItemType::SZ, "valuestr4", read_buff, sizeof(read_buff)));
}


TEST_CASE("inconsistent fields in item header with correct crc are handled for multi page blob", "[nvs]")
{
    // TC testing inconsistent fields in item header with correct crc are handled for blobs

    // Before each sub case damaging the blob with key = "valueblob1", following scheme will be used to store the blob and do the test
    // Page 1
    // 1x namespace entry
    // 1x U32 entry, key = "valueu32_1" before data
    // 124x BLOB_DATA entries, key = "valueblob1", chunk index = 0 containing 123*32 bytes of data

    // Page 2
    // 3x BLOB_DATA entries, key = "valueblob1", chunk index = 1 containing 2*32 bytes of data
    // 1x BLOB_INDEX entry key = "valueblob1", chunkVersion = 0, chunkCount = 2
    // 3x BLOB_DATA entries, key = "valueblob2", chunk index = 0 containing 2*32 bytes of data
    // 1x BLOB_INDEX entry key = "valueblob2", chunkVersion = 0, chunkCount = 1
    // 1x U32 entry key = "valueu32_2" after data

    // Please note that the PartitionTestHelper is kept in the scope block to ensure it's destructor is called prior the
    // nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME) in the evaluation section.

    // keys
    char ukey1[] = "valueu32_1";
    char ukey2[] = "valueu32_2";
    char blob_key1[] = "valueblob1";
    char blob_key2[] = "valueblob2";

    // planned chunk lengths
    size_t blob_key1_chunk_0_len = 123 * 32;
    size_t blob_key1_chunk_1_len = 2 * 32;
    size_t blob_key2_chunk_len = 2 * 32;

    // initial values
    uint32_t uval1 = 1;
    uint32_t uval2 = 2;
    size_t blob_data1_len = blob_key1_chunk_0_len + blob_key1_chunk_1_len;
    size_t blob_data2_len = blob_key2_chunk_len;
    uint8_t* blob_data1 = new uint8_t[blob_data1_len];
    uint8_t* blob_data2 = new uint8_t[blob_data2_len];

    // value buffers
    uint32_t read_u32_1;
    uint32_t read_u32_2;

    size_t read_blob1_size = blob_data1_len;
    size_t read_blob2_size = blob_data2_len;
    uint8_t* read_blob1 = new uint8_t[read_blob1_size];
    uint8_t* read_blob2 = new uint8_t[read_blob2_size];


    // Skip one page, 2 entries of page header and 3 entries of valueblob1's BLOB_DATA entries
    const size_t blob_index_offset = 4096 + 32 * 2 + 32 * 3;

    // Skip 2 entries of page header 1 entry of namespace and 1 entry of valueu32_1
    const size_t blob_data_chunk0_offset = 32 * 2 + 32 * 1 + 32 * 1;

    // returns length of BLOB_DATA of blob_key1 on page 1 within the partition
    // Skip 2 entries of page header 1 entry of namespace and 1 entry of valueu32_1
    const uint16_t blob_data_chunk0_len = blob_key1_chunk_0_len;

    // returns length of BLOB_DATA of blob_key1 on page 1 within the partition
    // Skip 2 entries of page header 1 entry of namespace and 1 entry of valueu32_1
    const uint16_t blob_data_chunk1_len = blob_key1_chunk_1_len;

    // returns offset of BLOB_DATA of blob_key1 on page 1 within the partition
    // Skip one page, 2 entries of page header
    const size_t blob_data_chunk1_offset = 4096 + 32 * 2;

    // initialize buffers
    read_u32_1 = 0;
    read_u32_2 = 0;
    memset(blob_data1, 0x55, blob_data1_len);
    memset(blob_data2, 0xaa, blob_data2_len);
    memset(read_blob1, 0, read_blob1_size);
    memset(read_blob2, 0, read_blob2_size);

    // Initialisation part. Write initial data to the nvs partition
    {
        TEST_ESP_OK(nvs_flash_erase_partition(TEST_DEFAULT_PARTITION_NAME));
        TEST_ESP_OK(nvs_flash_init_partition(TEST_DEFAULT_PARTITION_NAME));
        nvs_handle_t handle;
        TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, "test", NVS_READWRITE, &handle));
        TEST_ESP_OK(nvs_set_u32(handle, ukey1, uval1));
        TEST_ESP_OK(nvs_set_blob(handle, blob_key1, blob_data1, blob_data1_len));
        TEST_ESP_OK(nvs_set_blob(handle, blob_key2, blob_data2, blob_data2_len));
        TEST_ESP_OK(nvs_set_u32(handle, ukey2, uval2));
        nvs_close(handle);

        TEST_ESP_OK(nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME));
    }

    {   // Do not delete the block as it defines scope of the NVSPartitionTestHelper

        // PartitionTestHelper is used to provide nvs::Partition instance for the test
        // second parameter is set to false, so the partition is not erased as a part of the helper constructor
        NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME, false);

        // during the test, we will make changes of the data in more granular way than the whole 4k page
        // allow overwriting smaller portion of flash than whole 4k page
        h.set_erase_size(1);

        // Sub test cases
        SECTION("damage BLOB_IDX - span set to 0") {
            // Damage BLOB_IDX of valueblob1, modify span == 1 to 0
            size_t entry_offset = blob_index_offset;
            uint8_t expected_span = 1;
            uint8_t new_span = 0;

            uint8_t buff[sizeof(nvs::Item)] = {0};
            TEST_ESP_OK(h.read_raw(entry_offset, &buff, sizeof(buff)));
            nvs::Item* item = (nvs::Item*)buff;

            CHECK(item->span == expected_span);
            item->span = new_span;
            // recalculate crc same way as nvs::Item::calculateCrc32 does
            item->crc32 = ((nvs::Item*)buff)->calculateCrc32();
            TEST_ESP_OK(h.erase_range(entry_offset, sizeof(buff)));
            TEST_ESP_OK(h.write_raw(entry_offset, &buff, sizeof(buff)));
        }

        SECTION("damage BLOB_IDX - chunkIndex set to 111 instead of CHUNK_ANY") {
            // Damage BLOB_IDX of valueblob1, modify chunkIndex == 111
            size_t entry_offset = blob_index_offset;
            uint8_t expected_chunk_index = nvs::Item::CHUNK_ANY;
            uint8_t new_chunk_index = 111;

            uint8_t buff[sizeof(nvs::Item)] = {0};
            TEST_ESP_OK(h.read_raw(entry_offset, &buff, sizeof(buff)));
            nvs::Item* item = (nvs::Item*)buff;

            CHECK(item->chunkIndex == expected_chunk_index);
            item->chunkIndex = new_chunk_index;
            // recalculate crc same way as nvs::Item::calculateCrc32 does
            item->crc32 = ((nvs::Item*)buff)->calculateCrc32();
            TEST_ESP_OK(h.erase_range(entry_offset, sizeof(buff)));
            TEST_ESP_OK(h.write_raw(entry_offset, &buff, sizeof(buff)));
        }

        SECTION("damage BLOB_IDX - blobIndex.dataSize over theoretical limit of 32*125*127 (entry size * max payload entries in page * max # of chunks)") {
            // Damage BLOB_IDX of valueblob1, modify blobIndex.dataSize
            size_t entry_offset = blob_index_offset;
            uint32_t expected_data_size = blob_data_chunk0_len + blob_data_chunk1_len;
            uint32_t new_data_size = (32 * 125 * 127) + 1;

            uint8_t buff[sizeof(nvs::Item)] = {0};
            TEST_ESP_OK(h.read_raw(entry_offset, &buff, sizeof(buff)));
            nvs::Item* item = (nvs::Item*)buff;

            CHECK(item->blobIndex.dataSize == expected_data_size);
            item->chunkIndex = new_data_size;
            // recalculate crc same way as nvs::Item::calculateCrc32 does
            item->crc32 = ((nvs::Item*)buff)->calculateCrc32();
            TEST_ESP_OK(h.erase_range(entry_offset, sizeof(buff)));
            TEST_ESP_OK(h.write_raw(entry_offset, &buff, sizeof(buff)));
        }

        SECTION("damage BLOB_DATA - chunkIndex set to invalid value of CHUNK_ANY") {
            // Damage BLOB_DATA of valueblob1, modify chunkIndex to CHUNK_ANY
            size_t entry_offset = blob_data_chunk0_offset;
            uint8_t unexpected_chunk_index = nvs::Item::CHUNK_ANY;
            uint8_t new_chunk_index = nvs::Item::CHUNK_ANY;

            uint8_t buff[sizeof(nvs::Item)] = {0};
            TEST_ESP_OK(h.read_raw(entry_offset, &buff, sizeof(buff)));
            nvs::Item* item = (nvs::Item*)buff;

            CHECK(item->chunkIndex != unexpected_chunk_index);
            item->chunkIndex = new_chunk_index;

            // recalculate crc same way as nvs::Item::calculateCrc32 does
            item->crc32 = ((nvs::Item*)buff)->calculateCrc32();
            TEST_ESP_OK(h.erase_range(entry_offset, sizeof(buff)));
            TEST_ESP_OK(h.write_raw(entry_offset, &buff, sizeof(buff)));
        }

        SECTION("damage BLOB_DATA - varLength.dataSize is exceeding the maximum capacity available till the end of the page") {
            // Damage BLOB_DATA of valueblob1, set new data size to be beyond remaining capacity in the page.
            // Even if all entries are used for payload, there will be one overhead entry required
            size_t entry_offset = blob_data_chunk1_offset; // let's take chunk1 for this test
            uint16_t expected_data_size = blob_data_chunk1_len;
            uint16_t new_data_size = 32 * 126;

            uint8_t buff[sizeof(nvs::Item)] = {0};
            TEST_ESP_OK(h.read_raw(entry_offset, &buff, sizeof(buff)));
            nvs::Item* item = (nvs::Item*)buff;

            CHECK(item->varLength.dataSize == expected_data_size);
            item->varLength.dataSize = new_data_size;

            // recalculate crc same way as nvs::Item::calculateCrc32 does
            item->crc32 = ((nvs::Item*)buff)->calculateCrc32();
            TEST_ESP_OK(h.erase_range(entry_offset, sizeof(buff)));
            TEST_ESP_OK(h.write_raw(entry_offset, &buff, sizeof(buff)));
        }

        SECTION("damage BLOB_DATA - span > maxAvailablePageSpan") {
            // Damage BLOB_DATA of valueblob1, set span to the value larger than the rest of entries in the page but smaller than maximum of entries .
            // page can accommodate
            // chunk0 starts at entry index 2 so max span is 126 - 2 = 124. Use 125
            size_t entry_offset = blob_data_chunk0_offset;
            uint8_t expected_span = (blob_data_chunk0_len + 31) / 32 + 1;
            uint8_t new_span = 125;

            uint8_t buff[sizeof(nvs::Item)] = {0};
            TEST_ESP_OK(h.read_raw(entry_offset, &buff, sizeof(buff)));
            nvs::Item* item = (nvs::Item*)buff;

            CHECK(item->span == expected_span);
            item->span = new_span;

            // recalculate crc same way as nvs::Item::calculateCrc32 does
            item->crc32 = ((nvs::Item*)buff)->calculateCrc32();
            TEST_ESP_OK(h.erase_range(entry_offset, sizeof(buff)));
            TEST_ESP_OK(h.write_raw(entry_offset, &buff, sizeof(buff)));
        }
    } // end of block keeping the helper scope

    // Evaluation part. Check that storage can recover and validate the remaining data
    {
        TEST_ESP_OK(nvs_flash_init_partition(TEST_DEFAULT_PARTITION_NAME));

        nvs_handle_t handle;
        TEST_ESP_OK(nvs_open("test", NVS_READWRITE, &handle));
        TEST_ESP_OK(nvs_get_u32(handle, ukey1, &read_u32_1));
        TEST_ESP_ERR(nvs_get_blob(handle, blob_key1, read_blob1,  &read_blob1_size), ESP_ERR_NVS_NOT_FOUND);
        TEST_ESP_OK(nvs_get_blob(handle, blob_key2, read_blob2, &read_blob2_size));
        TEST_ESP_OK(nvs_get_u32(handle, ukey2, &read_u32_2));
        nvs_close(handle);
        TEST_ESP_OK(nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME));

        // validate the data
        CHECK(read_u32_1 == uval1);
        CHECK(read_u32_2 == uval2);
        CHECK(read_blob2_size == blob_data2_len);
        CHECK(memcmp(read_blob2, blob_data2, read_blob2_size) == 0);
    }
}


TEST_CASE("invalid data type in item header with correct crc is handled", "[nvs]")
{
    // TC testing invalid data type in item header with correct crc is handled
    // header entry of u32 entry will be damaged in a way that data type will be invalid
    // while crc will be correct

    // PartitionTestHelper is used to provide nvs::Partition instance for the test
    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);
    const bool purgeAfterErase = TEST_DEFAULT_PURGE_AFTER_ERASE;

    nvs::Storage storage(&h);
    // prepare some data
    TEST_ESP_OK(storage.init(0, 3));
    TEST_ESP_OK(storage.writeItem(0, "ns1", static_cast<uint8_t>(1), purgeAfterErase));
    TEST_ESP_OK(storage.writeItem(1, "value1", static_cast<uint32_t>(1), purgeAfterErase));
    TEST_ESP_OK(storage.writeItem(1, "value2", static_cast<uint32_t>(2), purgeAfterErase));
    TEST_ESP_OK(storage.writeItem(1, "value3", static_cast<uint32_t>(3), purgeAfterErase));

    // damage item header of value2 to introduce data type error, recalculate crc
    {
        nvs::ItemType new_data_type = (nvs::ItemType) 0xfe;
        size_t entry_offset = 32 * 4; // 2x page header + 1x ns1 + 1x value1
        uint8_t buff[sizeof(nvs::Item)] = {0};
        TEST_ESP_OK(h.read_raw(entry_offset, &buff, sizeof(buff)));

        nvs::Item* item = (nvs::Item*)buff;

        // set span to U8
        item->datatype = new_data_type;
        // recalculate crc same way as nvs::Item::calculateCrc32 does
        item->crc32 = ((nvs::Item*)buff)->calculateCrc32();

        // allow overwriting smaller portion of flash than whole 4k page
        h.set_erase_size(1);
        TEST_ESP_OK(h.erase_range(entry_offset, sizeof(buff)));
        TEST_ESP_OK(h.write_raw(entry_offset, &buff, sizeof(buff)));
    }

    // check that storage can recover
    uint32_t val = 0;

    TEST_ESP_OK(storage.init(0, 3));
    TEST_ESP_OK(storage.readItem(1, "value1", val));
    CHECK(val == 1);
    // check that the damaged item is no longer present
    TEST_ESP_ERR(ESP_ERR_NVS_NOT_FOUND, storage.readItem(1, "value2", val));

    TEST_ESP_OK(storage.readItem(1, "value3", val));
    CHECK(val == 3);
}

// leaks memory
TEST_CASE("Recovery from power-off when the entry being erased is not on active page", "[nvs]")
{
    // TC testing recovery from power-off when the entry being erased is not on active page
    // This test will write some data to the partition, then erase a key and power-off
    // while the entry is not on the active page. The test will check that the erased
    // key is not present after re-initialization of the partition.

    // Erase the partition before testing
    TEST_ESP_OK(nvs_flash_erase_partition(TEST_3SEC_PARTITION_NAME));

    const size_t blob_size = nvs::Page::CHUNK_MAX_SIZE / 2 ;
    size_t read_size = blob_size;
    uint8_t blob[blob_size] = {0x11};

    TEST_ESP_OK(nvs_flash_init_partition(TEST_3SEC_PARTITION_NAME));

    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open_from_partition(TEST_3SEC_PARTITION_NAME, "test", NVS_READWRITE, &handle));

    NVSPartitionTestHelper::clear_stats();
    NVSPartitionTestHelper::fail_after(nvs::Page::CHUNK_MAX_SIZE / 4 + 75, ESP_PARTITION_FAIL_AFTER_MODE_BOTH);
    TEST_ESP_OK(nvs_set_blob(handle, "1a", blob, blob_size));
    TEST_ESP_OK(nvs_set_blob(handle, "1b", blob, blob_size));

    TEST_ESP_ERR(nvs_erase_key(handle, "1a"), ESP_ERR_FLASH_OP_FAIL);

    // re-initialize the partition
    TEST_ESP_OK(nvs_flash_init_partition(TEST_3SEC_PARTITION_NAME));
    TEST_ESP_OK(nvs_open_from_partition(TEST_3SEC_PARTITION_NAME, "test", NVS_READWRITE, &handle));

    // Check 1a is erased fully
    TEST_ESP_ERR(nvs_get_blob(handle, "1a", blob, &read_size), ESP_ERR_NVS_NOT_FOUND);

    // Check 2b is still accessible
    TEST_ESP_OK(nvs_get_blob(handle, "1b", blob, &read_size));

    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_3SEC_PARTITION_NAME));
}

// leaks memory
TEST_CASE("Recovery from power-off when page is being freed", "[nvs]")
{
    // TC testing recovery from power-off when page is being freed
    // This test will write some data to the partition, then erase a key and power-off while freeing the page.
    // The test will check that the erased key is not present after re-initialization of the partition.

    // Use only 3 sectors partition for the test to induce the page freeing
    // Erase the partition before testing
    TEST_ESP_OK(nvs_flash_erase_partition(TEST_3SEC_PARTITION_NAME));

    const size_t blob_size = (nvs::Page::ENTRY_COUNT - 3) * nvs::Page::ENTRY_SIZE;
    size_t read_size = blob_size / 2;
    uint8_t blob[blob_size] = {0};

    TEST_ESP_OK(nvs_flash_init_partition(TEST_3SEC_PARTITION_NAME));

    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open_from_partition(TEST_3SEC_PARTITION_NAME, "test", NVS_READWRITE, &handle));
    // Fill first page
    TEST_ESP_OK(nvs_set_blob(handle, "1a", blob, blob_size / 3));
    TEST_ESP_OK(nvs_set_blob(handle, "1b", blob, blob_size / 3));
    TEST_ESP_OK(nvs_set_blob(handle, "1c", blob, blob_size / 4));
    // Fill second page
    TEST_ESP_OK(nvs_set_blob(handle, "2a", blob, blob_size / 2));
    TEST_ESP_OK(nvs_set_blob(handle, "2b", blob, blob_size / 2));

    TEST_ESP_OK(nvs_erase_key(handle, "1c"));

    NVSPartitionTestHelper::clear_stats();
    NVSPartitionTestHelper::fail_after(6 * nvs::Page::ENTRY_COUNT, ESP_PARTITION_FAIL_AFTER_MODE_BOTH);
    TEST_ESP_ERR(nvs_set_blob(handle, "1d", blob, blob_size / 4), ESP_ERR_FLASH_OP_FAIL);

    // re -initialize the partition
    TEST_ESP_OK(nvs_flash_init_partition(TEST_3SEC_PARTITION_NAME));
    TEST_ESP_OK(nvs_open_from_partition(TEST_3SEC_PARTITION_NAME, "test", NVS_READWRITE, &handle));

    read_size = blob_size / 3;
    TEST_ESP_OK(nvs_get_blob(handle, "1a", blob, &read_size));
    TEST_ESP_OK(nvs_get_blob(handle, "1b", blob, &read_size));

    read_size = blob_size / 4;
    TEST_ESP_ERR(nvs_get_blob(handle, "1c", blob, &read_size), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_ERR(nvs_get_blob(handle, "1d", blob, &read_size), ESP_ERR_NVS_NOT_FOUND);

    read_size = blob_size / 2;
    TEST_ESP_OK(nvs_get_blob(handle, "2a", blob, &read_size));
    TEST_ESP_OK(nvs_get_blob(handle, "2b", blob, &read_size));

    TEST_ESP_OK(nvs_commit(handle));
    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_3SEC_PARTITION_NAME));
}

TEST_CASE("Check that NVS supports old blob format without blob index", "[nvs]")
{
    // TC testing that NVS supports old blob format without blob index
    // This test will load a partition with pre-generated binary image containing blobs in old format
    // and then modify one of the blobs to be stored in new format with blob index.
    // The test will check that the blob is read correctly in both formats and that the blob index is present
    // for the modified blob.

    // PartitionTestHelper is used to provide nvs::Partition instance for the test
    NVSPartitionTestHelper h(TEST_3SEC_PARTITION_NAME);

    // Populate the partition with pre-generated file with blob in old format
    // Binary image contains 2 blobs:
    // - dummyHex2BinKey: hex data in old format without blob index
    // - dummyBase64Key: base64 data in old format without blob index
    TEST_ESP_OK(h.load_from_file(WD_PREFIX "../../nvs_partition_generator/part_old_blob_format.bin"));

    TEST_ESP_OK(nvs_flash_init_partition(TEST_3SEC_PARTITION_NAME));

    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open_from_partition(TEST_3SEC_PARTITION_NAME, "dummyNamespace", NVS_READWRITE, &handle));

    esp_err_t ret = ESP_OK;

    // Validate the contents of the blobs to make sure the right binary was loaded
    char buf[64] = {0};
    size_t buflen = 64;
    uint8_t hexdata[] = {0x01, 0x02, 0x03, 0xab, 0xcd, 0xef};
    TEST_ESP_OK(ret = nvs_get_blob(handle, "dummyHex2BinKey", buf, &buflen));
    if (ret == ESP_OK) {
        size_t len = buflen <= sizeof(hexdata) ? buflen : sizeof(hexdata);
        CHECK(memcmp(buf, hexdata, len) == 0);
    }

    buflen = 64;
    uint8_t base64data[] = {'1', '2', '3', 'a', 'b', 'c'};
    TEST_ESP_OK(ret = nvs_get_blob(handle, "dummyBase64Key", buf, &buflen));
    if (ret == ESP_OK) {
        size_t len = buflen <= sizeof(base64data) ? buflen : sizeof(base64data);
        CHECK(memcmp(buf, base64data, len) == 0);
    }

    // Use direct access to the page data using nvs::Page to perform exact item type checks
    nvs::Page p;
    TEST_ESP_OK(p.load(&h, 0));

    // Check that item is stored in old format without blob index
    TEST_ESP_OK(p.findItem(1, nvs::ItemType::BLOB, "dummyHex2BinKey"));

    // Modify the blob so that it is now stored in the new format
    hexdata[0] = hexdata[1] = hexdata[2] = 0x99;
    TEST_ESP_OK(nvs_set_blob(handle, "dummyHex2BinKey", hexdata, sizeof(hexdata)));

    nvs::Page p2;
    TEST_ESP_OK(p2.load(&h, 0));

    // Check the type of the blob. Expect type mismatch since the blob is stored in new format
    TEST_ESP_ERR(p2.findItem(1, nvs::ItemType::BLOB, "dummyHex2BinKey"), ESP_ERR_NVS_TYPE_MISMATCH);

    // Check that index is present for the modified blob according to new format
    TEST_ESP_OK(p2.findItem(1, nvs::ItemType::BLOB_IDX, "dummyHex2BinKey"));

    // Read the blob in new format and check the contents
    buflen = 64;
    TEST_ESP_OK(ret = nvs_get_blob(handle, "dummyBase64Key", buf, &buflen));
    if (ret == ESP_OK) {
        size_t len = buflen <= sizeof(base64data) ? buflen : sizeof(base64data);
        CHECK(memcmp(buf, base64data, len) == 0);
    }
    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_3SEC_PARTITION_NAME));
}

// Expected content of the string kye dummyStringKey in the tests using nvs_partition_generator utility
const char test_str_long[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.\n"
                            "Fusce quis risus justo.\n"
                            "Suspendisse egestas in nisi sit amet auctor.\n"
                            "Pellentesque rhoncus dictum sodales.\n"
                            "In justo erat, viverra at interdum eget, interdum vel dui.";

// Expected content of the string kye dummyStringKey in the tests using manufacturing utility
const char test_str_short[] = "0A:0B:0C:0D:0E:0F";


static void check_nvs_part_gen_args(char const *flash_binary_filename,      // name of binary containing emulated flash content
                                    char const *part_name,                  // name of partition
                                    int size,                               // required size of partition in sectors
                                    char const *blob_filename,              // file with expected contents of blob binFileKey
                                    const char *expected_string)            // expected string for dummyStringKey
{
    // Supplementary function to check NVS partition with given arguments
    // This function loads the partition from the binary file, initializes it,
    // and checks the values stored in the NVS partition against expected values.
    // The function expects the partition to contain the following keys and values:

    // Hardcoded expected values:
    // "dummyU8Key",      uint8_t,    127
    // "dummyI8Key",      int8_t,     -128
    // "dummyU16Key",     uint16_t,   32768
    // "dummyU32Key",     uint32_t,   4294967295
    // "dummyI32Key",     int32_t,    -2147483648
    // "dummyHex2BinKey", blob,       {0x01, 0x02, 0x03, 0xab, 0xcd, 0xef}
    // "dummyBase64Key",  blob,       {'1', '2', '3', 'a', 'b', 'c'}
    // "hexFileKey",      blob,       {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef}
    // "stringFileKey",   string,     "abcdefghijklmnopqrstuvwxyz\0"

    // Parametrised values:
    // "dummyStringKey",  string,     expected_string (function parameter, up to 256 bytes)
    // "binFileKey",      blob,       contents of blob_filename (function parameter, up to 5200 bytes)

    nvs_handle_t handle;

    // Validate that the partition is large enough
    CHECK(NVSPartitionTestHelper::get_sectors(part_name) >= size);

    // Load the partition from the binary file
    TEST_ESP_OK(NVSPartitionTestHelper::load_from_file(part_name, flash_binary_filename));

    TEST_ESP_OK(nvs_flash_init_partition(part_name));
    TEST_ESP_OK(nvs_open_from_partition(part_name, "dummyNamespace", NVS_READONLY, &handle));

    // Check that the partition contains the expected keys and values
    uint8_t u8v;
    TEST_ESP_OK(nvs_get_u8(handle, "dummyU8Key", &u8v));
    CHECK(u8v == 127);
    int8_t i8v;
    TEST_ESP_OK(nvs_get_i8(handle, "dummyI8Key", &i8v));
    CHECK(i8v == -128);
    uint16_t u16v;
    TEST_ESP_OK(nvs_get_u16(handle, "dummyU16Key", &u16v));
    CHECK(u16v == 32768);
    uint32_t u32v;
    TEST_ESP_OK(nvs_get_u32(handle, "dummyU32Key", &u32v));
    CHECK(u32v == 4294967295);
    int32_t i32v;
    TEST_ESP_OK(nvs_get_i32(handle, "dummyI32Key", &i32v));
    CHECK(i32v == -2147483648);


    char string_buf[256];
    size_t str_len = strlen(expected_string) + 1; // +1 for null terminator
    CHECK(sizeof(string_buf) >= str_len);
    TEST_ESP_OK(nvs_get_str(handle, "dummyStringKey", string_buf, &str_len));
    CHECK(strncmp(string_buf, expected_string, str_len) == 0);

    char buf[64] = {0};
    uint8_t hexdata[] = {0x01, 0x02, 0x03, 0xab, 0xcd, 0xef};
    size_t buflen = 64;
    TEST_ESP_OK(nvs_get_blob(handle, "dummyHex2BinKey", buf, &buflen));
    CHECK(memcmp(buf, hexdata, buflen) == 0);

    uint8_t base64data[] = {'1', '2', '3', 'a', 'b', 'c'};
    TEST_ESP_OK(nvs_get_blob(handle, "dummyBase64Key", buf, &buflen));
    CHECK(memcmp(buf, base64data, buflen) == 0);

    buflen = 64;
    uint8_t hexfiledata[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};
    TEST_ESP_OK(nvs_get_blob(handle, "hexFileKey", buf, &buflen));
    CHECK(memcmp(buf, hexfiledata, buflen) == 0);

    buflen = 64;
    uint8_t strfiledata[64] = "abcdefghijklmnopqrstuvwxyz\0";
    TEST_ESP_OK(nvs_get_str(handle, "stringFileKey", buf, &buflen));
    CHECK(memcmp(buf, strfiledata, buflen) == 0);

    char bin_data[5200];
    size_t bin_len = sizeof(bin_data);
    char binfiledata[5200];
    ifstream file;
    file.open(blob_filename);
    file.read(binfiledata, 5200);
    TEST_ESP_OK(nvs_get_blob(handle, "binFileKey", bin_data, &bin_len));
    CHECK(memcmp(bin_data, binfiledata, bin_len) == 0);

    file.close();
    nvs_close(handle);
    TEST_ESP_OK(nvs_flash_deinit_partition(part_name));
}

TEST_CASE("check and read data from partition generated via partition generation utility with multipage blob support disabled", "[nvs_part_gen]")
{
    // TC testing partition generation utility with multipage blob support disabled
    // This test will generate a partition with blob stored in single page format (without index)
    // and then read the data from the partition to check that it is correct.

    int status;
    int childpid = fork();
    if (childpid == 0) {
        // Copy testdata directory to the current working directory
        exit(execlp("cp", " cp",
                    "-rf",
                    WD_PREFIX "../../nvs_partition_generator/testdata",
                    ".",
                    NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) != -1);

        childpid = fork();

        if (childpid == 0) {
            // Generate partition with single page blob
            // The partition will contain a single page blob and data as specified in the sample_singlepage_blob.csv file
            exit(execlp("python", "python",
                        WD_PREFIX "../../nvs_partition_generator/nvs_partition_gen.py",
                        "generate",
                        WD_PREFIX "../../nvs_partition_generator/sample_singlepage_blob.csv",
                        "partition_single_page.bin",
                        "0x3000",
                        "--version",
                        "1",
                        "--outdir",
                        WD_PREFIX "../../nvs_partition_generator", NULL));
        } else {
            CHECK(childpid > 0);
            int status;
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) == 0);
        }
    }

    // Check the genetated partition binary using nvs flash API
    check_nvs_part_gen_args(WD_PREFIX "../../nvs_partition_generator/partition_single_page.bin",
                            TEST_3SEC_PARTITION_NAME,
                            3,
                            WD_PREFIX "../../nvs_partition_generator/testdata/sample_singlepage_blob.bin",
                            test_str_long);

    childpid = fork();
    if (childpid == 0) {
        // Clean up testdata directory and generated partition binary
        exit(execlp("bash", "bash",
                    "-c",
                    "rm -rf testdata && \
                    rm " WD_PREFIX "../../nvs_partition_generator/partition_single_page.bin", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

    }
}

TEST_CASE("check and read data from partition generated via partition generation utility with multipage blob support enabled", "[nvs_part_gen]")
{
    // TC testing partition generation utility with multipage blob support enabled
    // This test will generate a partition with blob stored in multipage format (with index)
    // and then read the data from the partition to check that it is correct.
    int status;
    int childpid = fork();
    if (childpid == 0) {
        // Copy testdata directory to the current working directory
        exit(execlp("cp", " cp",
                    "-rf",
                    WD_PREFIX "../../nvs_partition_generator/testdata",
                    ".", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

        childpid = fork();

        if (childpid == 0) {
            // Generate partition with multipage blob
            // The partition will contain a multipage blob and data as specified in the sample_multipage_blob.csv file
            // The partition will be generated with multipage blob support enabled
            exit(execlp("python", "python",
                        WD_PREFIX "../../nvs_partition_generator/nvs_partition_gen.py",
                        "generate",
                        WD_PREFIX "../../nvs_partition_generator/sample_multipage_blob.csv",
                        "partition_multipage_blob.bin",
                        "0x4000",
                        "--version",
                        "2",
                        "--outdir",
                        WD_PREFIX "../../nvs_partition_generator", NULL));
        } else {
            CHECK(childpid > 0);
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) == 0);
        }
    }

    // Check the generated partition binary using nvs flash API
    check_nvs_part_gen_args(WD_PREFIX "../../nvs_partition_generator/partition_multipage_blob.bin",
                            TEST_DEFAULT_PARTITION_NAME,
                            4,
                            WD_PREFIX "../../nvs_partition_generator/testdata/sample_multipage_blob.bin",
                            test_str_long);

    childpid = fork();
    if (childpid == 0) {
        // Clean up testdata directory and generated partition binary
        exit(execlp("bash", "bash",
                    "-c",
                    "rm -rf testdata && \
                    rm " WD_PREFIX "../../nvs_partition_generator/partition_multipage_blob.bin", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);
    }
}

TEST_CASE("check and read data from partition generated via manufacturing utility with multipage blob support disabled", "[mfg_gen]")
{
    // TC testing manufacturing utility and nvs_partition_gen with multipage blob support disabled
    // This test will generate a partition using each of generators with blob stored in single page format (without index)
    // and then read the data from the partition binaries to check that generators produce the same result.
    int childpid = fork();
    int status;

    if (childpid == 0) {
        // Copy testdata directory to the current working directory
        // Create host_test directory for the manufacturing utility
        // and copy testdata to it
        // Copy nvs_partition_generator testdata to the current working directory
        exit(execlp("bash", "bash",
                    "-c",
                    "rm -rf " WD_PREFIX "../../../../tools/mass_mfg/host_test && \
                    cp -rf " WD_PREFIX "../../../../tools/mass_mfg/testdata mfg_testdata && \
                    cp -rf " WD_PREFIX "../../nvs_partition_generator/testdata . && \
                    mkdir -p " WD_PREFIX "../../../../tools/mass_mfg/host_test", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

        childpid = fork();
        if (childpid == 0) {
            // Generate partition binary using mfg_gen (manufacturing utility) with single page blob format
            // The partition will contain a single page blob and data as specified in the sample_singlepage_blob.csv file
            exit(execlp("python", "python",
                        WD_PREFIX "../../../../tools/mass_mfg/mfg_gen.py",
                        "generate",
                        WD_PREFIX "../../../../tools/mass_mfg/samples/sample_config.csv",
                        WD_PREFIX "../../../../tools/mass_mfg/samples/sample_values_singlepage_blob.csv",
                        "Test",
                        "0x3000",
                        "--outdir",
                        WD_PREFIX "../../../../tools/mass_mfg/host_test",
                        "--version",
                        "1", NULL));

        } else {
            CHECK(childpid > 0);
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) == 0);

            childpid = fork();
            if (childpid == 0) {
                // Generate partition binary using nvs_partition_gen (nvs partition generator utility)
                // The partition will contain a single page blob and data as specified in the Test-1.csv file
                exit(execlp("python", "python",
                            WD_PREFIX "../../nvs_partition_generator/nvs_partition_gen.py",
                            "generate",
                            WD_PREFIX "../../../../tools/mass_mfg/host_test/csv/Test-1.csv",
                            WD_PREFIX "../../nvs_partition_generator/Test-1-partition.bin",
                            "0x3000",
                            "--version",
                            "1", NULL));

            } else {
                CHECK(childpid > 0);
                waitpid(childpid, &status, 0);
                CHECK(WEXITSTATUS(status) == 0);
            }
        }
    }

    // Check the partition binary generated by nvs partition generator utility using nvs flash API
    check_nvs_part_gen_args(WD_PREFIX "../../../../tools/mass_mfg/host_test/bin/Test-1.bin",
                            TEST_3SEC_PARTITION_NAME,
                            3,
                            "mfg_testdata/sample_singlepage_blob.bin",
                            test_str_short);

    // Check the partition binary generated by manufacturing utility using nvs flash API
    check_nvs_part_gen_args(WD_PREFIX "../../nvs_partition_generator/Test-1-partition.bin",
                            TEST_3SEC_PARTITION_NAME,
                            3,
                            "testdata/sample_singlepage_blob.bin",
                            test_str_short);

    childpid = fork();
    if (childpid == 0) {
        // Clean up testdata directory, generated partition binary and host_test directory
        exit(execlp("bash", " bash",
                    "-c",
                    "rm -rf " WD_PREFIX "../../../../tools/mass_mfg/host_test | \
                    rm " WD_PREFIX "../../nvs_partition_generator/Test-1-partition.bin | \
                    rm " WD_PREFIX "../../../../tools/mass_mfg/samples/" "*tmp* | \
                    rm -rf mfg_testdata | \
                    rm -rf testdata", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);
    }
}

TEST_CASE("check and read data from partition generated via manufacturing utility with blank lines in csv files and multipage blob support disabled", "[mfg_gen]")
{
    // TC testing manufacturing utility with blank lines in csv files and multipage blob support disabled
    // This test will generate a partition using manufacturing utility with blank lines in csv files
    // and blob stored in single page format (without index) and then read the data from the partition binaries
    // to check that the partition is generated correctly.
    int childpid = fork();
    int status;

    if (childpid == 0) {
        // Copy testdata directory to the current working directory
        // Create host_test directory for the manufacturing utility
        // and copy testdata to it
        // Copy nvs_partition_generator testdata to the current working directory
        exit(execlp("bash", "bash",
                    "-c",
                    "rm -rf " WD_PREFIX "../../../../tools/mass_mfg/host_test && \
                    cp -rf " WD_PREFIX "../../../../tools/mass_mfg/testdata mfg_testdata && \
                    cp -rf " WD_PREFIX "../../nvs_partition_generator/testdata . && \
                    mkdir -p " WD_PREFIX "../../../../tools/mass_mfg/host_test", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

        childpid = fork();
        if (childpid == 0) {
            // Generate partition binary using mfg_gen (manufacturing utility) with single page blob format
            // The partition will contain a single page blob and data as specified in the sample_values_singlepage_blob_blank_lines.csv file
            // The sample_config_blank_lines.csv file will be used to generate the partition
            // The csv files will contain blank lines and the partition should be generated correctly
            exit(execlp("python", "python",
                        WD_PREFIX "../../../../tools/mass_mfg/mfg_gen.py",
                        "generate",
                        WD_PREFIX "../../../../tools/mass_mfg/samples/sample_config_blank_lines.csv",
                        WD_PREFIX "../../../../tools/mass_mfg/samples/sample_values_singlepage_blob_blank_lines.csv",
                        "Test",
                        "0x3000",
                        "--outdir",
                        WD_PREFIX "../../../../tools/mass_mfg/host_test",
                        "--version",
                        "1", NULL));

        } else {
            CHECK(childpid > 0);
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) == 0);

            childpid = fork();
            if (childpid == 0) {
                // Generate partition binary using nvs_partition_gen (nvs partition generator utility)
                // The partition will contain a single page blob and data as specified in the Test-1.csv file
                exit(execlp("python", "python",
                            WD_PREFIX "../../nvs_partition_generator/nvs_partition_gen.py",
                            "generate",
                            WD_PREFIX "../../../../tools/mass_mfg/host_test/csv/Test-1.csv",
                            WD_PREFIX "../../nvs_partition_generator/Test-1-partition.bin",
                            "0x3000",
                            "--version",
                            "1", NULL));

            } else {
                CHECK(childpid > 0);
                waitpid(childpid, &status, 0);
                CHECK(WEXITSTATUS(status) == 0);
            }
        }
    }

    // Check the partition binary generated by manufacturing utility using nvs flash API
    // The partition should be generated correctly despite the blank lines in the csv files
    check_nvs_part_gen_args(WD_PREFIX "../../../../tools/mass_mfg/host_test/bin/Test-1.bin",
                            TEST_3SEC_PARTITION_NAME,
                            3,
                            "mfg_testdata/sample_singlepage_blob.bin",
                            test_str_short);

    // Check the partition binary generated by nvs partition generator utility using nvs flash API
    check_nvs_part_gen_args(WD_PREFIX "../../nvs_partition_generator/Test-1-partition.bin",
                            TEST_3SEC_PARTITION_NAME,
                            3,
                            "testdata/sample_singlepage_blob.bin",
                            test_str_short);

    childpid = fork();
    if (childpid == 0) {
        // Clean up testdata directory, generated partition binary and host_test directory
        exit(execlp("bash", " bash",
                    "-c",
                    "rm -rf " WD_PREFIX "../../../../tools/mass_mfg/host_test | \
                    rm " WD_PREFIX "../../nvs_partition_generator/Test-1-partition.bin | \
                    rm " WD_PREFIX "../../../../tools/mass_mfg/samples/" "*tmp* | \
                    rm -rf mfg_testdata | \
                    rm -rf testdata", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);
    }
}

TEST_CASE("check and read data from partition generated via manufacturing utility with multipage blob support enabled", "[mfg_gen]")
{
    // TC testing manufacturing utility and nvs_partition_gen with multipage blob support enabled
    // This test will generate a partition using each of generators with blob stored in multiple page format (with index)
    // and then read the data from the partition binaries to check that generators produce the same result.
    int childpid = fork();
    int status;

    if (childpid == 0) {
        // Copy testdata directory to the current working directory
        // Create host_test directory for the manufacturing utility
        // and copy testdata to it
        // Copy nvs_partition_generator testdata to the current working directory
        exit(execlp("bash", " bash",
                    "-c",
                    "rm -rf " WD_PREFIX "../../../../tools/mass_mfg/host_test | \
                    cp -rf " WD_PREFIX "../../../../tools/mass_mfg/testdata mfg_testdata | \
                    cp -rf " WD_PREFIX "../../nvs_partition_generator/testdata . | \
                    mkdir -p " WD_PREFIX "../../../../tools/mass_mfg/host_test", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

        childpid = fork();
        if (childpid == 0) {
            // Generate partition binary using mfg_gen (manufacturing utility) with multipage blob format
            // The partition will contain a multipage blob and data as specified in the sample_multipage_blob.csv file
            // The partition will be generated with multipage blob support enabled
            exit(execlp("python", "python",
                        WD_PREFIX "../../../../tools/mass_mfg/mfg_gen.py",
                        "generate",
                        WD_PREFIX "../../../../tools/mass_mfg/samples/sample_config.csv",
                        WD_PREFIX "../../../../tools/mass_mfg/samples/sample_values_multipage_blob.csv",
                        "Test",
                        "0x4000",
                        "--outdir",
                        WD_PREFIX "../../../../tools/mass_mfg/host_test",
                        "--version",
                        "2", NULL));

        } else {
            CHECK(childpid > 0);
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) == 0);

            childpid = fork();
            if (childpid == 0) {
                // Generate partition binary using nvs_partition_gen (nvs partition generator utility)
                // The partition will contain a multipage blob and data as specified in the Test-1.csv file
                // The partition will be generated with multipage blob support enabled
                exit(execlp("python", "python",
                            WD_PREFIX "../../nvs_partition_generator/nvs_partition_gen.py",
                            "generate",
                            WD_PREFIX "../../../../tools/mass_mfg/host_test/csv/Test-1.csv",
                            WD_PREFIX "../../nvs_partition_generator/Test-1-partition.bin",
                            "0x4000",
                            "--version",
                            "2", NULL));

            } else {
                CHECK(childpid > 0);
                waitpid(childpid, &status, 0);
                CHECK(WEXITSTATUS(status) == 0);
            }
        }
    }

    // Check the partition binary generated by manufacturing utility using nvs flash API

    check_nvs_part_gen_args(WD_PREFIX "../../../../tools/mass_mfg/host_test/bin/Test-1.bin",
                            TEST_DEFAULT_PARTITION_NAME,
                            4,
                            "mfg_testdata/sample_multipage_blob.bin",
                            test_str_short);

    // Check the partition binary generated by nvs partition generator utility using nvs flash API
    check_nvs_part_gen_args(WD_PREFIX "../../nvs_partition_generator/Test-1-partition.bin",
                            TEST_DEFAULT_PARTITION_NAME,
                            4,
                            "testdata/sample_multipage_blob.bin",
                            test_str_short);

    childpid = fork();
    if (childpid == 0) {
        // Clean up testdata directory, generated partition binary and host_test directory
        // Remove temporary files created by the manufacturing utility
        // and nvs_partition_generator utility
        // Remove testdata directory
        exit(execlp("bash", " bash",
                    "-c",
                    "rm -rf " WD_PREFIX "../../../../tools/mass_mfg/host_test | \
                    rm " WD_PREFIX "../../nvs_partition_generator/Test-1-partition.bin | \
                    rm " WD_PREFIX "../../../../tools/mass_mfg/samples/" "*tmp* | \
                    rm -rf mfg_testdata | \
                    rm -rf testdata", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);
    }
}

TEST_CASE("nvs multiple write with same key but different types", "[nvs]")
{
    // TC testing that NVS supports multiple writes with same key but different types
    // This test will write multiple values with the same key but different types
    // and then read the values to check that the latest value is returned.
    // Based on the configuration option CONFIG_NVS_LEGACY_DUP_KEYS_COMPATIBILITY, the behavior will differ.

    // Erase the partition before testing
    TEST_ESP_OK(nvs_flash_erase_partition(TEST_3SEC_PARTITION_NAME));

    TEST_ESP_OK(nvs_flash_init_partition(TEST_3SEC_PARTITION_NAME));

    nvs_handle_t handle_1;
    TEST_ESP_OK(nvs_open_from_partition(TEST_3SEC_PARTITION_NAME, "namespace1", NVS_READWRITE, &handle_1));

    int32_t v32;
    int8_t v8;

    TEST_ESP_OK(nvs_set_i32(handle_1, "foo", (int32_t)12345678));
    TEST_ESP_OK(nvs_set_i8(handle_1, "foo", (int8_t)12));
    TEST_ESP_OK(nvs_set_i8(handle_1, "foo", (int8_t)34));

#ifdef CONFIG_NVS_LEGACY_DUP_KEYS_COMPATIBILITY
    // Legacy behavior
    // First use of key hooks data type until removed by nvs_erase_key. Alternative reuse of same key with different
    // data type is written to the storage as hidden active value. It is returned by nvs_get function after nvs_erase_key is called.
    // Mixing more than 2 data types brings undefined behavior. It is not tested here.

    TEST_ESP_ERR(nvs_get_i8(handle_1, "foo", &v8), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_OK(nvs_get_i32(handle_1, "foo", &v32));
    TEST_ESP_OK(nvs_erase_key(handle_1, "foo"));

    TEST_ESP_OK(nvs_get_i8(handle_1, "foo", &v8));
    TEST_ESP_ERR(nvs_get_i32(handle_1, "foo", &v32), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_OK(nvs_erase_key(handle_1, "foo"));

    TEST_ESP_OK(nvs_get_i8(handle_1, "foo", &v8));
    TEST_ESP_ERR(nvs_get_i32(handle_1, "foo", &v32), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_OK(nvs_erase_key(handle_1, "foo"));

    TEST_ESP_ERR(nvs_erase_key(handle_1, "foo"), ESP_ERR_NVS_NOT_FOUND);
#else
    // New behavior
    // Latest nvs_set call replaces any existing value. Only one active value under the key exists in storage

    TEST_ESP_OK(nvs_get_i8(handle_1, "foo", &v8));
    TEST_ESP_ERR(nvs_get_i32(handle_1, "foo", &v32), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_OK(nvs_erase_key(handle_1, "foo"));

    TEST_ESP_ERR(nvs_get_i8(handle_1, "foo", &v8), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_ERR(nvs_get_i32(handle_1, "foo", &v32), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_ERR(nvs_erase_key(handle_1, "foo"), ESP_ERR_NVS_NOT_FOUND);
#endif

    nvs_close(handle_1);

    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_3SEC_PARTITION_NAME));
}

#ifndef CONFIG_NVS_LEGACY_DUP_KEYS_COMPATIBILITY
// Following test case is not valid for new behavior not leading to multiple active values under the same key
TEST_CASE("nvs multiple write with same key blob and string involved", "[nvs]")
{
    // Erase the partition before testing
    TEST_ESP_OK(nvs_flash_erase_partition(TEST_3SEC_PARTITION_NAME));

    TEST_ESP_OK(nvs_flash_init_partition(TEST_3SEC_PARTITION_NAME));

    nvs_handle_t handle_1;
    TEST_ESP_OK(nvs_open_from_partition(TEST_3SEC_PARTITION_NAME, "namespace1", NVS_READWRITE, &handle_1));

    const char key_name[] = "foo";

    // integer variables
    int32_t v32;
    int8_t v8;

    // string
    #define str_data_len 64
    const char str_data[] = "string data";
    char str_buf[str_data_len] = {0};
    size_t str_len = str_data_len;

    // blob
    #define blob_data_len 64
    uint8_t blob_data[blob_data_len] = {0};
    uint8_t blob_buf[blob_data_len] = {0};
    size_t blob_read_size;

    // first write is i32
    TEST_ESP_OK(nvs_set_i32(handle_1, key_name, (int32_t)12345678));

    TEST_ESP_ERR(nvs_get_i8(handle_1, key_name, &v8), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_OK(nvs_get_i32(handle_1, key_name, &v32));
    TEST_ESP_ERR(nvs_get_str(handle_1, key_name, str_buf, &str_len), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_ERR(nvs_get_blob(handle_1, key_name, blob_buf, &blob_read_size), ESP_ERR_NVS_NOT_FOUND);


    // second write is string
    TEST_ESP_OK(nvs_set_str(handle_1, key_name, str_data));

    TEST_ESP_ERR(nvs_get_i8(handle_1, key_name, &v8), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_ERR(nvs_get_i32(handle_1, key_name, &v32), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_OK(nvs_get_str(handle_1, key_name, str_buf, &str_len));
    TEST_ESP_ERR(nvs_get_blob(handle_1, key_name, blob_buf, &blob_read_size), ESP_ERR_NVS_NOT_FOUND);

    // third write is blob
    TEST_ESP_OK(nvs_set_blob(handle_1, key_name, blob_data, blob_data_len));

    TEST_ESP_ERR(nvs_get_i8(handle_1, key_name, &v8), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_ERR(nvs_get_i32(handle_1, key_name, &v32), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_ERR(nvs_get_str(handle_1, key_name, str_buf, &str_len), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_OK(nvs_get_blob(handle_1, key_name, blob_buf, &blob_read_size));

    // fourth write is i8
    TEST_ESP_OK(nvs_set_i8(handle_1, key_name, (int8_t)12));

    TEST_ESP_OK(nvs_get_i8(handle_1, key_name, &v8));
    TEST_ESP_ERR(nvs_get_i32(handle_1, key_name, &v32), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_ERR(nvs_get_str(handle_1, key_name, str_buf, &str_len), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_ERR(nvs_get_blob(handle_1, key_name, blob_buf, &blob_read_size), ESP_ERR_NVS_NOT_FOUND);

    nvs_close(handle_1);

    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_3SEC_PARTITION_NAME));
}
#endif // !CONFIG_NVS_LEGACY_DUP_KEYS_COMPATIBILITY

TEST_CASE("nvs find key tests", "[nvs]")
{
    // TC testing nvs_find_key function
    // This test will open a writable namespace, write some values, erase them and check that
    // nvs_find_key function returns correct results before and after each of the steps.
    // It will also test that nvs_find_key respects the namespace and does not find keys in other namespaces.

    // Erase the partition before testing
    TEST_ESP_OK(NVSPartitionTestHelper::randomize_partition(TEST_DEFAULT_PARTITION_NAME, 100));

    TEST_ESP_OK(nvs_flash_init_partition(TEST_DEFAULT_PARTITION_NAME));

    const size_t buff_len = 4096;   // length of blob to be used in tests
    nvs_handle_t handle_1;          // handle for first namespace
    nvs_handle_t handle_2;          // handle for second namespace
    nvs_type_t datatype_found;      // datatype of entry found

    // open writeable namespace
    TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, "namespace1", NVS_READWRITE, &handle_1));

    // set value, erease value, test find before and after each of steps
    TEST_ESP_ERR(nvs_find_key(handle_1, "foo", &datatype_found), ESP_ERR_NVS_NOT_FOUND);
    // write "foo" as I32, should find it, first attempt without pointer to type variable
    TEST_ESP_OK(nvs_set_i32(handle_1, "foo", 0x12345678));
    TEST_ESP_OK(nvs_find_key(handle_1, "foo", nullptr));
    // second search attempt with pointer to type variable specified
    TEST_ESP_OK(nvs_find_key(handle_1, "foo", &datatype_found));
    CHECK(datatype_found == NVS_TYPE_I32);
    TEST_ESP_OK(nvs_erase_key(handle_1, "foo"));
    TEST_ESP_ERR(nvs_find_key(handle_1, "foo", &datatype_found), ESP_ERR_NVS_NOT_FOUND);

    // set value, rewrite value, erease value, test find before and after each of steps
    TEST_ESP_ERR(nvs_find_key(handle_1, "foo1", &datatype_found), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_OK(nvs_set_i16(handle_1, "foo1", 0x1234));
    TEST_ESP_OK(nvs_find_key(handle_1, "foo1", &datatype_found));
    CHECK(datatype_found == NVS_TYPE_I16);
    TEST_ESP_OK(nvs_set_i16(handle_1, "foo1", 0x4321));
    TEST_ESP_OK(nvs_find_key(handle_1, "foo1", &datatype_found));
    CHECK(datatype_found == NVS_TYPE_I16);
    TEST_ESP_OK(nvs_erase_key(handle_1, "foo1"));
    TEST_ESP_ERR(nvs_find_key(handle_1, "foo1", &datatype_found), ESP_ERR_NVS_NOT_FOUND);

    // set blob value, rewrite blob, delete blob, test find before and after each of steps
    uint8_t *p_buff = (uint8_t *) malloc(buff_len);
    CHECK(p_buff != nullptr);
    TEST_ESP_ERR(nvs_find_key(handle_1, "foo2", &datatype_found), ESP_ERR_NVS_NOT_FOUND);
    for (size_t i = 0; i < buff_len; i++) {
        p_buff[i] = (uint8_t)(i % 0xff);
    }
    TEST_ESP_OK(nvs_set_blob(handle_1, "foo2", p_buff, buff_len));
    TEST_ESP_OK(nvs_find_key(handle_1, "foo2", &datatype_found));
    CHECK(datatype_found == NVS_TYPE_BLOB);
    for (size_t i = 0; i < buff_len; i++) {
        p_buff[i] = (uint8_t)((buff_len - i - 1) % 0xff);
    }
    TEST_ESP_OK(nvs_set_blob(handle_1, "foo2", p_buff, buff_len));
    TEST_ESP_OK(nvs_find_key(handle_1, "foo2", &datatype_found));
    CHECK(datatype_found == NVS_TYPE_BLOB);
    TEST_ESP_OK(nvs_erase_key(handle_1, "foo2"));
    TEST_ESP_ERR(nvs_find_key(handle_1, "foo2", &datatype_found), ESP_ERR_NVS_NOT_FOUND);

    // test namespace is respected in nvs_find_key
    // open second writeable namespace
    TEST_ESP_OK(nvs_open_from_partition(TEST_DEFAULT_PARTITION_NAME, "namespace2", NVS_READWRITE, &handle_2));
    TEST_ESP_ERR(nvs_find_key(handle_1, "foon1", &datatype_found), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_ERR(nvs_find_key(handle_2, "foon1", &datatype_found), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_OK(nvs_set_i16(handle_1, "foon1", 0x1234));
    TEST_ESP_OK(nvs_find_key(handle_1, "foon1", &datatype_found));
    TEST_ESP_ERR(nvs_find_key(handle_2, "foon1", &datatype_found), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_OK(nvs_set_i16(handle_2, "foon1", 0x1234));
    TEST_ESP_OK(nvs_find_key(handle_1, "foon1", &datatype_found));
    TEST_ESP_OK(nvs_find_key(handle_2, "foon1", &datatype_found));
    TEST_ESP_OK(nvs_erase_key(handle_1, "foon1"));
    TEST_ESP_ERR(nvs_find_key(handle_1, "foon1", &datatype_found), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_OK(nvs_find_key(handle_2, "foon1", &datatype_found));
    TEST_ESP_OK(nvs_erase_key(handle_2, "foon1"));
    TEST_ESP_ERR(nvs_find_key(handle_1, "foon1", &datatype_found), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_ERR(nvs_find_key(handle_2, "foon1", &datatype_found), ESP_ERR_NVS_NOT_FOUND);

    nvs_close(handle_1);
    nvs_close(handle_2);
    TEST_ESP_OK(nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME));
}

TEST_CASE("nvs handle purge test", "[nvs]")
{
    // TC validating that erased and overwritten entries are purged from NVS storage based on the mode of handle opening.
    // The test covers both modes: with implicit purge enabled and disabled.
    // To validate the behavior, the test:
    // Writes entries of different types to NVS storage. Entries include: integer, string and multi page blob.
    // Inf first round, the data contains initial markers to identify them.
    // Then it erases / overwrites the entries one by one and after each erase it checks whether the markers are still present in the storage

    // SECTIONS:
    // 1. Open writable namespace without implicit purge. Erase data. Should find markers.
    // 2/ Open writable namespace without implicit purge. Overwrite data. Should find old markers.
    // 3. Open writable namespace with implicit purge. Erase data. Should NOT find markers.
    // 4. Open writable namespace with implicit purge. Overwrite data. Should NOT find old markers.

    const char* part_name = TEST_DEFAULT_PARTITION_NAME;
    const char* ns_name = "ns1";


    // Markers and lengths used in the test
    // String mrkers are fixed strings at the beginning of the string data as well as at the end of the string data
    const char str_key[] = "str_key";
    const char str_marker_start[] = "STR_START_";;
    const char str_marker_end[] = "_STR_END";
    const size_t string_len = 3000;                 // 4000 is max, 3000 fills almost entire page

    // Blob markers are fixed patterns at the beginning of the blob data as well as at the end of the blob data
    const char blob_key[] = "blob_key";
    const uint8_t blob_marker_start[] = {0xBA, 0xAD, 0xF0, 0x0D};
    const uint8_t blob_marker_end[]   = {0xD0, 0x0F, 0xDA, 0xAB};
    const size_t blob_len = 8192;                   // 8192 bytes to ensure multipage blob

    // uint64_t marker is used for integer data
    const char uint_key[] = "uint_key";
    const uint64_t uint_marker = 0xDEADBEEFFEEBDAED;

    // Overwrite patterns
    const char str_overwrite_pattern = 'X';
    const uint8_t blob_overwrite_pattern = 0x5A;
    const uint64_t uint_overwrite_pattern = 0xA5A5A5A5A5A5A5A5;


    // nvs_flash_erase_partition to start with clean partition
    TEST_ESP_OK(nvs_flash_erase_partition(part_name));
    TEST_ESP_OK(nvs_flash_init_partition(part_name));

    // init non-purge handle
    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open_from_partition(part_name, ns_name, NVS_READWRITE, &handle));

    // populate with markers
    // string
    char* str_data = (char*) malloc(string_len);
    CHECK(str_data != nullptr);
    memset(str_data, 'A', string_len);
    memcpy(str_data, str_marker_start, strlen(str_marker_start));
    memcpy(str_data + string_len - (strlen(str_marker_end) + 1), str_marker_end, strlen(str_marker_end));
    str_data[string_len - 1] = '\0'; // ensure null termination
    TEST_ESP_OK(nvs_set_str(handle, str_key, str_data));
    free(str_data);

    // blob
    uint8_t* blob_data = (uint8_t*) malloc(blob_len);
    CHECK(blob_data != nullptr);
    memset(blob_data, 0xFF, blob_len);
    memcpy(blob_data, blob_marker_start, sizeof(blob_marker_start));
    memcpy(blob_data + blob_len - sizeof(blob_marker_end), blob_marker_end, sizeof(blob_marker_end));
    TEST_ESP_OK(nvs_set_blob(handle, blob_key, blob_data, blob_len));
    free(blob_data);

    // uint64_t
    TEST_ESP_OK(nvs_set_u64(handle, uint_key, uint_marker));
    TEST_ESP_OK(nvs_commit(handle));

    // make sure markers are written
    // check string marker
    TEST_ESP_OK(NVSPartitionTestHelper::check_marker(part_name, (const uint8_t*)str_marker_start, strlen(str_marker_start)));
    TEST_ESP_OK(NVSPartitionTestHelper::check_marker(part_name, (const uint8_t*)str_marker_end, strlen(str_marker_end)));
    // check blob marker
    TEST_ESP_OK(NVSPartitionTestHelper::check_marker(part_name, blob_marker_start, sizeof(blob_marker_start)));
    TEST_ESP_OK(NVSPartitionTestHelper::check_marker(part_name, blob_marker_end, sizeof(blob_marker_end)));
    // check uint64_t marker
    TEST_ESP_OK(NVSPartitionTestHelper::check_marker(part_name, (const uint8_t*)&uint_marker, sizeof(uint_marker)));

    // deinit
    nvs_close(handle);

    SECTION("non-purge handle - erase entries")
    {
        // init non-purge handle
        TEST_ESP_OK(nvs_open_from_partition(part_name, ns_name, NVS_READWRITE, &handle));

        // erase / check markers
        TEST_ESP_OK(nvs_erase_key(handle, str_key));
        TEST_ESP_OK(nvs_erase_key(handle, blob_key));
        TEST_ESP_OK(nvs_erase_key(handle, uint_key));

        TEST_ESP_OK(nvs_commit(handle));

        // this is non implicit purge handle, so markers should be found
        // check string marker
        TEST_ESP_OK(NVSPartitionTestHelper::check_marker(part_name, (const uint8_t*)str_marker_start, strlen(str_marker_start)));
        TEST_ESP_OK(NVSPartitionTestHelper::check_marker(part_name, (const uint8_t*)str_marker_end, strlen(str_marker_end)));
        // check blob marker
        TEST_ESP_OK(NVSPartitionTestHelper::check_marker(part_name, blob_marker_start, sizeof(blob_marker_start)));
        TEST_ESP_OK(NVSPartitionTestHelper::check_marker(part_name, blob_marker_end, sizeof(blob_marker_end)));
        // check uint64_t marker
        TEST_ESP_OK(NVSPartitionTestHelper::check_marker(part_name, (const uint8_t*)&uint_marker, sizeof(uint_marker)));

        // explicitly call purge
        TEST_ESP_OK(nvs_purge_all(handle));

        // after purge, markers should NOT be found
        // check string marker
        TEST_ESP_ERR(NVSPartitionTestHelper::check_marker(part_name, (const uint8_t*)str_marker_start, strlen(str_marker_start)), ESP_ERR_NVS_NOT_FOUND);
        TEST_ESP_ERR(NVSPartitionTestHelper::check_marker(part_name, (const uint8_t*)str_marker_end, strlen(str_marker_end)), ESP_ERR_NVS_NOT_FOUND);
        // check blob marker
        TEST_ESP_ERR(NVSPartitionTestHelper::check_marker(part_name, blob_marker_start, sizeof(blob_marker_start)), ESP_ERR_NVS_NOT_FOUND);
        TEST_ESP_ERR(NVSPartitionTestHelper::check_marker(part_name, blob_marker_end, sizeof(blob_marker_end)), ESP_ERR_NVS_NOT_FOUND);
        // check uint64_t marker
        TEST_ESP_ERR(NVSPartitionTestHelper::check_marker(part_name, (const uint8_t*)&uint_marker, sizeof(uint_marker)), ESP_ERR_NVS_NOT_FOUND);
    }

    SECTION("non-purge handle - overwrite entries")
    {
        // init non-purge handle
        TEST_ESP_OK(nvs_open_from_partition(part_name, ns_name, NVS_READWRITE, &handle));

        // overwrite / check markers
        // string
        char* str_data_ov = (char*) malloc(string_len);
        CHECK(str_data_ov != nullptr);
        memset(str_data_ov, str_overwrite_pattern, string_len);
        str_data_ov[string_len - 1] = '\0'; // ensure null termination
        TEST_ESP_OK(nvs_set_str(handle, str_key, str_data_ov));

        // blob
        uint8_t* blob_data_ov = (uint8_t*) malloc(blob_len);
        CHECK(blob_data_ov != nullptr);
        memset(blob_data_ov, blob_overwrite_pattern, blob_len);
        TEST_ESP_OK(nvs_set_blob(handle, blob_key, blob_data_ov, blob_len));

        // uint64_t
        TEST_ESP_OK(nvs_set_u64(handle, uint_key, uint_overwrite_pattern));
        TEST_ESP_OK(nvs_commit(handle));

        free(str_data_ov);
        free(blob_data_ov);

        // this is non implicit purge handle, so old markers should be found
        // check string marker
        TEST_ESP_OK(NVSPartitionTestHelper::check_marker(part_name, (const uint8_t*)str_marker_start, strlen(str_marker_start)));
        TEST_ESP_OK(NVSPartitionTestHelper::check_marker(part_name, (const uint8_t*)str_marker_end, strlen(str_marker_end)));
        // check blob marker
        TEST_ESP_OK(NVSPartitionTestHelper::check_marker(part_name, blob_marker_start, sizeof(blob_marker_start)));
        TEST_ESP_OK(NVSPartitionTestHelper::check_marker(part_name, blob_marker_end, sizeof(blob_marker_end)));
        // check uint64_t marker
        TEST_ESP_OK(NVSPartitionTestHelper::check_marker(part_name, (const uint8_t*)&uint_marker, sizeof(uint_marker)));

        // explicitly call purge
        TEST_ESP_OK(nvs_purge_all(handle));

        // after purge, markers should NOT be found
        // check string marker
        TEST_ESP_ERR(NVSPartitionTestHelper::check_marker(part_name, (const uint8_t*)str_marker_start, strlen(str_marker_start)), ESP_ERR_NVS_NOT_FOUND);
        TEST_ESP_ERR(NVSPartitionTestHelper::check_marker(part_name, (const uint8_t*)str_marker_end, strlen(str_marker_end)), ESP_ERR_NVS_NOT_FOUND);
        // check blob marker
        TEST_ESP_ERR(NVSPartitionTestHelper::check_marker(part_name, blob_marker_start, sizeof(blob_marker_start)), ESP_ERR_NVS_NOT_FOUND);
        TEST_ESP_ERR(NVSPartitionTestHelper::check_marker(part_name, blob_marker_end, sizeof(blob_marker_end)), ESP_ERR_NVS_NOT_FOUND);
        // check uint64_t marker
        TEST_ESP_ERR(NVSPartitionTestHelper::check_marker(part_name, (const uint8_t*)&uint_marker, sizeof(uint_marker)), ESP_ERR_NVS_NOT_FOUND);
    }

    SECTION("purge handle - erase entries")
    {
        // init purge handle
        TEST_ESP_OK(nvs_open_from_partition(part_name, ns_name, NVS_READWRITE_PURGE, &handle));

        // erase / check markers
        TEST_ESP_OK(nvs_erase_key(handle, str_key));
        TEST_ESP_OK(nvs_erase_key(handle, blob_key));
        TEST_ESP_OK(nvs_erase_key(handle, uint_key));

        TEST_ESP_OK(nvs_commit(handle));

        // this is purge handle, so markers should NOT be found
        // check string marker
        TEST_ESP_ERR(NVSPartitionTestHelper::check_marker(part_name, (const uint8_t*)str_marker_start, strlen(str_marker_start)), ESP_ERR_NVS_NOT_FOUND);
        TEST_ESP_ERR(NVSPartitionTestHelper::check_marker(part_name, (const uint8_t*)str_marker_end, strlen(str_marker_end)), ESP_ERR_NVS_NOT_FOUND);
        // check blob marker
        TEST_ESP_ERR(NVSPartitionTestHelper::check_marker(part_name, blob_marker_start, sizeof(blob_marker_start)), ESP_ERR_NVS_NOT_FOUND);
        TEST_ESP_ERR(NVSPartitionTestHelper::check_marker(part_name, blob_marker_end, sizeof(blob_marker_end)), ESP_ERR_NVS_NOT_FOUND);
        // check uint64_t marker
        TEST_ESP_ERR(NVSPartitionTestHelper::check_marker(part_name, (const uint8_t*)&uint_marker, sizeof(uint_marker)), ESP_ERR_NVS_NOT_FOUND);
    }

    SECTION("purge handle - overwrite entries")
    {
        // init purge handle
        TEST_ESP_OK(nvs_open_from_partition(part_name, ns_name, NVS_READWRITE_PURGE, &handle));

        // overwrite / check markers
        // string
        char* str_data_ov = (char*) malloc(string_len);
        CHECK(str_data_ov != nullptr);
        memset(str_data_ov, str_overwrite_pattern, string_len);
        str_data_ov[string_len - 1] = '\0'; // ensure null termination

        TEST_ESP_OK(nvs_set_str(handle, str_key, str_data_ov));

        // blob
        uint8_t* blob_data_ov = (uint8_t*) malloc(blob_len);
        CHECK(blob_data_ov != nullptr);
        memset(blob_data_ov, blob_overwrite_pattern, blob_len);
        TEST_ESP_OK(nvs_set_blob(handle, blob_key, blob_data_ov, blob_len));

        // uint64_t
        TEST_ESP_OK(nvs_set_u64(handle, uint_key, uint_overwrite_pattern));
        TEST_ESP_OK(nvs_commit(handle));

        free(str_data_ov);
        free(blob_data_ov);

        // this is purge handle, so old markers should NOT be found
        // check string marker
        TEST_ESP_ERR(NVSPartitionTestHelper::check_marker(part_name, (const uint8_t*)str_marker_start, strlen(str_marker_start)), ESP_ERR_NVS_NOT_FOUND);
        TEST_ESP_ERR(NVSPartitionTestHelper::check_marker(part_name, (const uint8_t*)str_marker_end, strlen(str_marker_end)), ESP_ERR_NVS_NOT_FOUND);
        // check blob marker
        TEST_ESP_ERR(NVSPartitionTestHelper::check_marker(part_name, blob_marker_start, sizeof(blob_marker_start)), ESP_ERR_NVS_NOT_FOUND);
        TEST_ESP_ERR(NVSPartitionTestHelper::check_marker(part_name, blob_marker_end, sizeof(blob_marker_end)), ESP_ERR_NVS_NOT_FOUND);
        // check uint64_t marker
        TEST_ESP_ERR(NVSPartitionTestHelper::check_marker(part_name, (const uint8_t*)&uint_marker, sizeof(uint_marker)), ESP_ERR_NVS_NOT_FOUND);
    }

    // deinit
    nvs_close(handle);

    // clean up partition
    TEST_ESP_OK(nvs_flash_erase_partition(part_name));
}


// Add new tests above
// This test has to be the final one

TEST_CASE("dump all performance data", "[nvs]")
{
    std::cout << "====================" << std::endl << "Dumping benchmarks" << std::endl;
    std::cout << s_perf.str() << std::endl;
    std::cout << "====================" << std::endl;
}

#if defined(SEGGER_H) && defined(GLOBAL_H)
NVS_GUARD_SYSVIEW_MACRO_EXPANSION_POP();
#endif
