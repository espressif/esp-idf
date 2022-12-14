/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "catch.hpp"
#include "nvs.hpp"
#include "nvs_test_api.h"
#include "sdkconfig.h"
#include "spi_flash_emulation.h"
#include "nvs_partition_manager.hpp"
#include "nvs_partition.hpp"
#include "mbedtls/aes.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <string>
#include "test_fixtures.hpp"

#define TEST_ESP_ERR(rc, res) CHECK((rc) == (res))
#define TEST_ESP_OK(rc) CHECK((rc) == ESP_OK)

stringstream s_perf;

void dumpBytes(const uint8_t *data, size_t count)
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


TEST_CASE("Page handles invalid CRC of variable length items", "[nvs][cur]")
{
    PartitionEmulationFixture f(0, 4);
    {
        nvs::Page page;
        TEST_ESP_OK(page.load(&f.part, 0));
        char buf[128] = {0};
        TEST_ESP_OK(page.writeItem(1, nvs::ItemType::BLOB, "1", buf, sizeof(buf)));
    }
    // corrupt header of the item (64 is the offset of the first item in page)
    uint32_t overwrite_buf = 0;
    f.emu.write(64, &overwrite_buf, 4);
    // load page again
    {
        nvs::Page page;
        TEST_ESP_OK(page.load(&f.part, 0));
    }
}

TEST_CASE("namespace name is deep copy", "[nvs]")
{
    char ns_name[16];
    strcpy(ns_name, "const_name");

    nvs_handle_t handle_1;
    nvs_handle_t handle_2;
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;

    PartitionEmulationFixture f(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN);

    f.emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);

    TEST_ESP_OK( nvs::NVSPartitionManager::get_instance()->init_custom(&f.part,
                NVS_FLASH_SECTOR,
                NVS_FLASH_SECTOR_COUNT_MIN));

    TEST_ESP_OK(nvs_open("const_name", NVS_READWRITE, &handle_1));
    strcpy(ns_name, "just_kidding");

    CHECK(nvs_open("just_kidding", NVS_READONLY, &handle_2) == ESP_ERR_NVS_NOT_FOUND);

    nvs_close(handle_1);
    nvs_close(handle_2);

    nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME);
}

TEST_CASE("writing the identical content does not write or erase", "[nvs]")
{
    PartitionEmulationFixture f(0, 20);

    const uint32_t NVS_FLASH_SECTOR = 5;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 10;
    f.emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);
    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(&f.part,
                NVS_FLASH_SECTOR,
                NVS_FLASH_SECTOR_COUNT_MIN));

    nvs_handle misc_handle;
    TEST_ESP_OK(nvs_open("test", NVS_READWRITE, &misc_handle));

    // Test writing a u8 twice, then changing it
    nvs_set_u8(misc_handle, "test_u8", 8);
    f.emu.clearStats();
    nvs_set_u8(misc_handle, "test_u8", 8);
    CHECK(f.emu.getWriteOps() == 0);
    CHECK(f.emu.getEraseOps() == 0);
    CHECK(f.emu.getReadOps()  != 0);
    f.emu.clearStats();
    nvs_set_u8(misc_handle, "test_u8", 9);
    CHECK(f.emu.getWriteOps() != 0);
    CHECK(f.emu.getReadOps()  != 0);

    // Test writing a string twice, then changing it
    static const char *test[2] = {"Hello world.", "Hello world!"};
    nvs_set_str(misc_handle, "test_str", test[0]);
    f.emu.clearStats();
    nvs_set_str(misc_handle, "test_str", test[0]);
    CHECK(f.emu.getWriteOps() == 0);
    CHECK(f.emu.getEraseOps() == 0);
    CHECK(f.emu.getReadOps()  != 0);
    f.emu.clearStats();
    nvs_set_str(misc_handle, "test_str", test[1]);
    CHECK(f.emu.getWriteOps() != 0);
    CHECK(f.emu.getReadOps()  != 0);

    // Test writing a multi-page blob, then changing it
    uint8_t blob[nvs::Page::CHUNK_MAX_SIZE * 3] = {0};
    memset(blob, 1, sizeof(blob));
    nvs_set_blob(misc_handle, "test_blob", blob, sizeof(blob));
    f.emu.clearStats();
    nvs_set_blob(misc_handle, "test_blob", blob, sizeof(blob));
    CHECK(f.emu.getWriteOps() == 0);
    CHECK(f.emu.getEraseOps() == 0);
    CHECK(f.emu.getReadOps()  != 0);
    blob[sizeof(blob) - 1]++;
    f.emu.clearStats();
    nvs_set_blob(misc_handle, "test_blob", blob, sizeof(blob));
    CHECK(f.emu.getWriteOps() != 0);
    CHECK(f.emu.getReadOps()  != 0);

    TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
}

TEST_CASE("can init storage from flash with random contents", "[nvs]")
{
    PartitionEmulationFixture f(0, 10);
    f.emu.randomize(42);

    nvs_handle_t handle;
    const uint32_t NVS_FLASH_SECTOR = 5;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
    f.emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);
    TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(&f.part,
                NVS_FLASH_SECTOR,
                NVS_FLASH_SECTOR_COUNT_MIN));

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
        PartitionEmulationFixture f(0, 10);
        f.emu.randomize(static_cast<uint32_t>(count));

        const uint32_t NVS_FLASH_SECTOR = 6;
        const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;
        f.emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);

        TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(&f.part,
                    NVS_FLASH_SECTOR,
                    NVS_FLASH_SECTOR_COUNT_MIN));

        nvs_handle_t handle_1;
        TEST_ESP_ERR(nvs_open("namespace1", NVS_READONLY, &handle_1), ESP_ERR_NVS_NOT_FOUND);

        TEST_ESP_OK(nvs_open("namespace1", NVS_READWRITE, &handle_1));
        TEST_ESP_OK(nvs_set_i32(handle_1, "foo", 0x12345678));
        for (size_t i = 0; i < 500; ++i) {
            nvs_handle_t handle_2;
            TEST_ESP_OK(nvs_open("namespace2", NVS_READWRITE, &handle_2));
            TEST_ESP_OK(nvs_set_i32(handle_1, "foo", 0x23456789 % (i + 1)));
            TEST_ESP_OK(nvs_set_i32(handle_2, "foo", static_cast<int32_t>(i)));
            const char *str = "value 0123456789abcdef0123456789abcdef %09d";
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

        TEST_ESP_OK(nvs_flash_deinit_partition(f.part.get_partition_name()));
    }
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

TEST_CASE("test recovery from sudden poweroff", "[long][nvs][recovery][monkey]")
{
    std::random_device rd;
    std::mt19937 gen(rd());
    uint32_t seed = 3;
    gen.seed(seed);
    const size_t iter_count = 2000;

    size_t totalOps = 0;
    int lastPercent = -1;
    for (uint32_t errDelay = 0; ; ++errDelay) {
        INFO(errDelay);

        PartitionEmulationFixture f(0, 10);
        const uint32_t NVS_FLASH_SECTOR = 2;
        const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 8;
        f.emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);

        f.emu.randomize(seed);
        f.emu.clearStats();
        f.emu.failAfter(errDelay);
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

        if (nvs::NVSPartitionManager::get_instance()->init_custom(&f.part,
                NVS_FLASH_SECTOR,
                NVS_FLASH_SECTOR_COUNT_MIN) == ESP_OK) {
            auto res = ESP_ERR_FLASH_OP_FAIL;
            if (nvs_open("namespace1", NVS_READWRITE, &handle) == ESP_OK) {
                res = test.doRandomThings(handle, gen, count);
                nvs_close(handle);
            }

            TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
            if (res != ESP_ERR_FLASH_OP_FAIL) {
                // This means we got to the end without an error due to f.emu.failAfter(), therefore errDelay
                // is high enough that we're not triggering it any more, therefore we're done
                break;
            }
        }

        TEST_ESP_OK(nvs::NVSPartitionManager::get_instance()->init_custom(&f.part,
                    NVS_FLASH_SECTOR,
                    NVS_FLASH_SECTOR_COUNT_MIN));
        TEST_ESP_OK(nvs_open("namespace1", NVS_READWRITE, &handle));
        auto res = test.doRandomThings(handle, gen, count);
        if (res != ESP_OK) {
            nvs_dump(NVS_DEFAULT_PART_NAME);
            CHECK(0);
        }
        nvs_close(handle);
        totalOps = f.emu.getEraseOps() + f.emu.getWriteBytes() / 4;

        TEST_ESP_OK(nvs_flash_deinit_partition(NVS_DEFAULT_PART_NAME));
    }
}

TEST_CASE("duplicate items are removed", "[nvs][dupes]")
{
    PartitionEmulationFixture f(0, 3);
    {
        // create one item
        nvs::Page p;
        p.load(&f.part, 0);
        p.writeItem<uint8_t>(1, "opmode", 3);
    }
    {
        // add another two without deleting the first one
        nvs::Item item(1, nvs::ItemType::U8, 1, "opmode");
        item.data[0] = 2;
        item.crc32 = item.calculateCrc32();
        f.emu.write(3 * 32, reinterpret_cast<const uint32_t *>(&item), sizeof(item));
        f.emu.write(4 * 32, reinterpret_cast<const uint32_t *>(&item), sizeof(item));
        uint32_t mask = 0xFFFFFFEA;
        f.emu.write(32, &mask, 4);
    }
    {
        // load page and check that second item persists
        nvs::Storage s(&f.part);
        s.init(0, 3);
        uint8_t val;
        ESP_ERROR_CHECK(s.readItem(1, "opmode", val));
        CHECK(val == 2);
    }
    {
        nvs::Page p;
        p.load(&f.part, 0);
        CHECK(p.getErasedEntryCount() == 2);
        CHECK(p.getUsedEntryCount() == 1);
    }
}

TEST_CASE("recovery after failure to write data", "[nvs]")
{
    PartitionEmulationFixture f(0, 3);
    const char str[] = "value 0123456789abcdef012345678value 0123456789abcdef012345678";

    // make flash write fail exactly in nvs::Page::writeEntryData
    f.emu.failAfter(17);
    {
        nvs::Storage storage(&f.part);
        TEST_ESP_OK(storage.init(0, 3));

        TEST_ESP_ERR(storage.writeItem(1, nvs::ItemType::SZ, "key", str, strlen(str)), ESP_ERR_FLASH_OP_FAIL);

        // check that repeated operations cause an error
        TEST_ESP_ERR(storage.writeItem(1, nvs::ItemType::SZ, "key", str, strlen(str)), ESP_ERR_NVS_INVALID_STATE);

        uint8_t val;
        TEST_ESP_ERR(storage.readItem(1, nvs::ItemType::U8, "key", &val, sizeof(val)), ESP_ERR_NVS_NOT_FOUND);
    }
    {
        // load page and check that data was erased
        nvs::Page p;
        p.load(&f.part, 0);
        CHECK(p.getErasedEntryCount() == 3);
        CHECK(p.getUsedEntryCount() == 0);

        // try to write again
        TEST_ESP_OK(p.writeItem(1, nvs::ItemType::SZ, "key", str, strlen(str)));
    }
}

TEST_CASE("crc errors in item header are handled", "[nvs]")
{
    PartitionEmulationFixture f(0, 3);
    nvs::Storage storage(&f.part);
    // prepare some data
    TEST_ESP_OK(storage.init(0, 3));
    TEST_ESP_OK(storage.writeItem(0, "ns1", static_cast<uint8_t>(1)));
    TEST_ESP_OK(storage.writeItem(1, "value1", static_cast<uint32_t>(1)));
    TEST_ESP_OK(storage.writeItem(1, "value2", static_cast<uint32_t>(2)));

    // corrupt item header
    uint32_t val = 0;
    f.emu.write(32 * 3, &val, 4);

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
        TEST_ESP_OK(storage.writeItem(1, item_name, static_cast<uint32_t>(i)));
    }

    // corrupt another item on the full page
    val = 0;
    f.emu.write(32 * 4, &val, 4);

    // check that storage can recover
    TEST_ESP_OK(storage.init(0, 3));
    // check that the corrupted item is no longer present
    TEST_ESP_ERR(ESP_ERR_NVS_NOT_FOUND, storage.readItem(1, "value2", val));
}

TEST_CASE("crc error in variable length item is handled", "[nvs]")
{
    PartitionEmulationFixture f(0, 3);
    const uint64_t before_val = 0xbef04e;
    const uint64_t after_val = 0xaf7e4;
    // write some data
    {
        nvs::Page p;
        p.load(&f.part, 0);
        TEST_ESP_OK(p.writeItem<uint64_t>(0, "before", before_val));
        const char *str = "foobar";
        TEST_ESP_OK(p.writeItem(0, nvs::ItemType::SZ, "key", str, strlen(str)));
        TEST_ESP_OK(p.writeItem<uint64_t>(0, "after", after_val));
    }
    // corrupt some data
    uint32_t w;
    CHECK(f.emu.read(&w, 32 * 3 + 8, sizeof(w)));
    w &= 0xf000000f;
    CHECK(f.emu.write(32 * 3 + 8, &w, sizeof(w)));
    // load and check
    {
        nvs::Page p;
        p.load(&f.part, 0);
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

TEST_CASE("multiple partitions access check", "[nvs]")
{
    SpiFlashEmulator emu(10);
    PartitionEmulation p0(&emu, 0 * SPI_FLASH_SEC_SIZE, 5 * SPI_FLASH_SEC_SIZE, "nvs1");
    PartitionEmulation p1(&emu, 5 * SPI_FLASH_SEC_SIZE, 5 * SPI_FLASH_SEC_SIZE, "nvs2");

    TEST_ESP_OK( nvs::NVSPartitionManager::get_instance()->init_custom(&p0, 0, 5) );
    TEST_ESP_OK( nvs::NVSPartitionManager::get_instance()->init_custom(&p1, 5, 5) );
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

    TEST_ESP_OK(nvs_flash_deinit_partition(p0.get_partition_name()));
    TEST_ESP_OK(nvs_flash_deinit_partition(p1.get_partition_name()));
}

// leaks memory
TEST_CASE("Recovery from power-off when the entry being erased is not on active page", "[nvs]")
{
    const size_t blob_size = nvs::Page::CHUNK_MAX_SIZE / 2 ;
    size_t read_size = blob_size;
    uint8_t blob[blob_size] = {0x11};
    PartitionEmulationFixture f(0, 3);
    TEST_ESP_OK( nvs::NVSPartitionManager::get_instance()->init_custom(&f.part, 0, 3) );
    nvs_handle_t handle;
    TEST_ESP_OK( nvs_open("test", NVS_READWRITE, &handle) );

    f.emu.clearStats();
    f.emu.failAfter(nvs::Page::CHUNK_MAX_SIZE / 4 + 75);
    TEST_ESP_OK( nvs_set_blob(handle, "1a", blob, blob_size) );
    TEST_ESP_OK( nvs_set_blob(handle, "1b", blob, blob_size) );

    TEST_ESP_ERR( nvs_erase_key(handle, "1a"), ESP_ERR_FLASH_OP_FAIL );

    TEST_ESP_OK( nvs::NVSPartitionManager::get_instance()->init_custom(&f.part, 0, 3) );

    /* Check 1a is erased fully*/
    TEST_ESP_ERR( nvs_get_blob(handle, "1a", blob, &read_size), ESP_ERR_NVS_NOT_FOUND);

    /* Check 2b is still accessible*/
    TEST_ESP_OK( nvs_get_blob(handle, "1b", blob, &read_size));

    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(f.part.get_partition_name()));
}

// leaks memory
TEST_CASE("Recovery from power-off when page is being freed.", "[nvs]")
{
    const size_t blob_size = (nvs::Page::ENTRY_COUNT - 3) * nvs::Page::ENTRY_SIZE;
    size_t read_size = blob_size / 2;
    uint8_t blob[blob_size] = {0};
    PartitionEmulationFixture f(0, 3);
    TEST_ESP_OK( nvs::NVSPartitionManager::get_instance()->init_custom(&f.part, 0, 3));
    nvs_handle_t handle;
    TEST_ESP_OK(nvs_open("test", NVS_READWRITE, &handle));
    // Fill first page
    TEST_ESP_OK(nvs_set_blob(handle, "1a", blob, blob_size / 3));
    TEST_ESP_OK(nvs_set_blob(handle, "1b", blob, blob_size / 3));
    TEST_ESP_OK(nvs_set_blob(handle, "1c", blob, blob_size / 4));
    // Fill second page
    TEST_ESP_OK(nvs_set_blob(handle, "2a", blob, blob_size / 2));
    TEST_ESP_OK(nvs_set_blob(handle, "2b", blob, blob_size / 2));

    TEST_ESP_OK(nvs_erase_key(handle, "1c"));

    f.emu.clearStats();
    f.emu.failAfter(6 * nvs::Page::ENTRY_COUNT);
    TEST_ESP_ERR(nvs_set_blob(handle, "1d", blob, blob_size / 4), ESP_ERR_FLASH_OP_FAIL);

    TEST_ESP_OK( nvs::NVSPartitionManager::get_instance()->init_custom(&f.part, 0, 3));

    read_size = blob_size / 3;
    TEST_ESP_OK( nvs_get_blob(handle, "1a", blob, &read_size));
    TEST_ESP_OK( nvs_get_blob(handle, "1b", blob, &read_size));

    read_size = blob_size / 4;
    TEST_ESP_ERR( nvs_get_blob(handle, "1c", blob, &read_size), ESP_ERR_NVS_NOT_FOUND);
    TEST_ESP_ERR( nvs_get_blob(handle, "1d", blob, &read_size), ESP_ERR_NVS_NOT_FOUND);

    read_size = blob_size / 2;
    TEST_ESP_OK( nvs_get_blob(handle, "2a", blob, &read_size));
    TEST_ESP_OK( nvs_get_blob(handle, "2b", blob, &read_size));

    TEST_ESP_OK(nvs_commit(handle));
    nvs_close(handle);

    TEST_ESP_OK(nvs_flash_deinit_partition(f.part.get_partition_name()));
}

TEST_CASE("Check that NVS supports old blob format without blob index", "[nvs]")
{
    SpiFlashEmulator emu("../nvs_partition_generator/part_old_blob_format.bin");
    PartitionEmulation part(&emu, 0, 2 * SPI_FLASH_SEC_SIZE, "test");
    nvs_handle_t handle;

    TEST_ESP_OK(  nvs::NVSPartitionManager::get_instance()->init_custom(&part, 0, 2) );
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

    nvs::Page p;
    p.load(&part, 0);

    /* Check that item is stored in old format without blob index*/
    TEST_ESP_OK(p.findItem(1, nvs::ItemType::BLOB, "dummyHex2BinKey"));

    /* Modify the blob so that it is stored in the new format*/
    hexdata[0] = hexdata[1] = hexdata[2] = 0x99;
    TEST_ESP_OK(nvs_set_blob(handle, "dummyHex2BinKey", hexdata, sizeof(hexdata)));

    nvs::Page p2;
    p2.load(&part, 0);

    /* Check the type of the blob. Expect type mismatch since the blob is stored in new format*/
    TEST_ESP_ERR(p2.findItem(1, nvs::ItemType::BLOB, "dummyHex2BinKey"), ESP_ERR_NVS_TYPE_MISMATCH);

    /* Check that index is present for the modified blob according to new format*/
    TEST_ESP_OK(p2.findItem(1, nvs::ItemType::BLOB_IDX, "dummyHex2BinKey"));

    /* Read the blob in new format and check the contents*/
    buflen = 64;
    TEST_ESP_OK( nvs_get_blob(handle, "dummyBase64Key", buf, &buflen));
    CHECK(memcmp(buf, base64data, buflen) == 0);

    TEST_ESP_OK(nvs_flash_deinit_partition(part.get_partition_name()));
}

static void check_nvs_part_gen_args(SpiFlashEmulator *spi_flash_emulator,
                                    char const *part_name,
                                    int size,
                                    char const *filename,
                                    bool is_encr,
                                    nvs_sec_cfg_t *xts_cfg)
{
    nvs_handle_t handle;

    esp_partition_t esp_part;
    esp_part.encrypted = false; // we're not testing generic flash encryption here, only the legacy NVS encryption
    esp_part.address = 0;
    esp_part.size = size * SPI_FLASH_SEC_SIZE;
    strncpy(esp_part.label, part_name, PART_NAME_MAX_SIZE);
    unique_ptr<nvs::Partition> part;

    if (is_encr) {
        nvs::NVSEncryptedPartition *enc_part = new (std::nothrow) nvs::NVSEncryptedPartition(&esp_part);
        REQUIRE(enc_part != nullptr);
        TEST_ESP_OK(enc_part->init(xts_cfg));
        part.reset(enc_part);
    } else {
        part.reset(new PartitionEmulation(spi_flash_emulator, 0, size, part_name));
    }

    TEST_ESP_OK(  nvs::NVSPartitionManager::get_instance()->init_custom(part.get(), 0, size) );

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

    char string_buf[256];
    const char test_str[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.\n"
                             "Fusce quis risus justo.\n"
                             "Suspendisse egestas in nisi sit amet auctor.\n"
                             "Pellentesque rhoncus dictum sodales.\n"
                             "In justo erat, viverra at interdum eget, interdum vel dui.";
    size_t str_len = sizeof(test_str);
    TEST_ESP_OK( nvs_get_str(handle, "dummyStringKey", string_buf, &str_len));
    CHECK(strncmp(string_buf, test_str, str_len) == 0);

    char buf[64] = {0};
    uint8_t hexdata[] = {0x01, 0x02, 0x03, 0xab, 0xcd, 0xef};
    size_t buflen = 64;
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

static void check_nvs_part_gen_args_mfg(SpiFlashEmulator *spi_flash_emulator,
                                        char const *part_name,
                                        int size,
                                        char const *filename,
                                        bool is_encr,
                                        nvs_sec_cfg_t *xts_cfg)
{
    nvs_handle_t handle;

    esp_partition_t esp_part;
    esp_part.encrypted = false; // we're not testing generic flash encryption here, only the legacy NVS encryption
    esp_part.address = 0;
    esp_part.size = size * SPI_FLASH_SEC_SIZE;
    strncpy(esp_part.label, part_name, PART_NAME_MAX_SIZE);
    unique_ptr<nvs::Partition> part;

    if (is_encr) {
        nvs::NVSEncryptedPartition *enc_part = new (std::nothrow) nvs::NVSEncryptedPartition(&esp_part);
        REQUIRE(enc_part != nullptr);
        TEST_ESP_OK(enc_part->init(xts_cfg));
        part.reset(enc_part);
    } else {
        part.reset(new PartitionEmulation(spi_flash_emulator, 0, size, part_name));
    }

    TEST_ESP_OK(  nvs::NVSPartitionManager::get_instance()->init_custom(part.get(), 0, size) );

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
                    ".", NULL));
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
                        "../nvs_partition_generator", NULL));
        } else {
            CHECK(childpid > 0);
            int status;
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) == 0);
        }
    }

    SpiFlashEmulator emu("../nvs_partition_generator/partition_single_page.bin");

    check_nvs_part_gen_args(&emu, "test", 3, "../nvs_partition_generator/testdata/sample_singlepage_blob.bin", false, NULL);

    childpid = fork();
    if (childpid == 0) {
        exit(execlp("rm", " rm",
                    "-rf",
                    "testdata", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

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
                    ".", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

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
                        "../nvs_partition_generator", NULL));
        } else {
            CHECK(childpid > 0);
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) == 0);
        }
    }

    SpiFlashEmulator emu("../nvs_partition_generator/partition_multipage_blob.bin");

    check_nvs_part_gen_args(&emu, "test", 4, "../nvs_partition_generator/testdata/sample_multipage_blob.bin", false, NULL);

    childpid = fork();
    if (childpid == 0) {
        exit(execlp("rm", " rm",
                    "-rf",
                    "testdata", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

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
        CHECK(WEXITSTATUS(status) == 0);

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
                        "1", NULL));

        } else {
            CHECK(childpid > 0);
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) == 0);

            childpid = fork();
            if (childpid == 0) {
                exit(execlp("python", "python",
                            "../nvs_partition_generator/nvs_partition_gen.py",
                            "generate",
                            "../../../tools/mass_mfg/host_test/csv/Test-1.csv",
                            "../nvs_partition_generator/Test-1-partition.bin",
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

    SpiFlashEmulator emu1("../../../tools/mass_mfg/host_test/bin/Test-1.bin");
    check_nvs_part_gen_args_mfg(&emu1, "test", 3, "mfg_testdata/sample_singlepage_blob.bin", false, NULL);

    SpiFlashEmulator emu2("../nvs_partition_generator/Test-1-partition.bin");
    check_nvs_part_gen_args_mfg(&emu2, "test", 3, "testdata/sample_singlepage_blob.bin", false, NULL);


    childpid = fork();
    if (childpid == 0) {
        exit(execlp("bash", " bash",
                    "-c",
                    "rm -rf ../../../tools/mass_mfg/host_test | \
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
        CHECK(WEXITSTATUS(status) == 0);

        childpid = fork();
        if (childpid == 0) {
            exit(execlp("python", "python",
                        "../../../tools/mass_mfg/mfg_gen.py",
                        "generate",
                        "../../../tools/mass_mfg/samples/sample_config_blank_lines.csv",
                        "../../../tools/mass_mfg/samples/sample_values_singlepage_blob_blank_lines.csv",
                        "Test",
                        "0x3000",
                        "--outdir",
                        "../../../tools/mass_mfg/host_test",
                        "--version",
                        "1", NULL));

        } else {
            CHECK(childpid > 0);
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) == 0);

            childpid = fork();
            if (childpid == 0) {
                exit(execlp("python", "python",
                            "../nvs_partition_generator/nvs_partition_gen.py",
                            "generate",
                            "../../../tools/mass_mfg/host_test/csv/Test-1.csv",
                            "../nvs_partition_generator/Test-1-partition.bin",
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

    SpiFlashEmulator emu1("../../../tools/mass_mfg/host_test/bin/Test-1.bin");
    check_nvs_part_gen_args_mfg(&emu1, "test", 3, "mfg_testdata/sample_singlepage_blob.bin", false, NULL);

    SpiFlashEmulator emu2("../nvs_partition_generator/Test-1-partition.bin");
    check_nvs_part_gen_args_mfg(&emu2, "test", 3, "testdata/sample_singlepage_blob.bin", false, NULL);


    childpid = fork();
    if (childpid == 0) {
        exit(execlp("bash", " bash",
                    "-c",
                    "rm -rf ../../../tools/mass_mfg/host_test | \
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
    int childpid = fork();
    int status;

    if (childpid == 0) {
        exit(execlp("bash", " bash",
                    "-c",
                    "rm -rf ../../../tools/mass_mfg/host_test | \
                    cp -rf ../../../tools/mass_mfg/testdata mfg_testdata | \
                    cp -rf ../nvs_partition_generator/testdata . | \
                    mkdir -p ../../../tools/mass_mfg/host_test", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

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
                        "2", NULL));

        } else {
            CHECK(childpid > 0);
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) == 0);

            childpid = fork();
            if (childpid == 0) {
                exit(execlp("python", "python",
                            "../nvs_partition_generator/nvs_partition_gen.py",
                            "generate",
                            "../../../tools/mass_mfg/host_test/csv/Test-1.csv",
                            "../nvs_partition_generator/Test-1-partition.bin",
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

    SpiFlashEmulator emu1("../../../tools/mass_mfg/host_test/bin/Test-1.bin");
    check_nvs_part_gen_args_mfg(&emu1, "test", 4, "mfg_testdata/sample_multipage_blob.bin", false, NULL);

    SpiFlashEmulator emu2("../nvs_partition_generator/Test-1-partition.bin");
    check_nvs_part_gen_args_mfg(&emu2, "test", 4, "testdata/sample_multipage_blob.bin", false, NULL);

    childpid = fork();
    if (childpid == 0) {
        exit(execlp("bash", " bash",
                    "-c",
                    "rm -rf ../../../tools/mass_mfg/host_test | \
                    rm -rf mfg_testdata | \
                    rm -rf testdata", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

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
    uint8_t ptxt_hex[nvs::Page::ENTRY_SIZE], ctxt_hex[nvs::Page::ENTRY_SIZE], ba_hex[16];
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

    char ptxt[][2 * nvs::Page::ENTRY_SIZE + 1] = {
        "0000000000000000000000000000000000000000000000000000000000000000",
        "4444444444444444444444444444444444444444444444444444444444444444"
    };
    char ctxt[][2 * nvs::Page::ENTRY_SIZE + 1] = {
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

        CHECK(!mbedtls_aes_crypt_xts(ectx, MBEDTLS_AES_ENCRYPT, nvs::Page::ENTRY_SIZE, ba_hex, ptxt_hex, ptxt_hex));

        CHECK(!memcmp(ptxt_hex, ctxt_hex, nvs::Page::ENTRY_SIZE));
    }
}

TEST_CASE("test nvs apis with encryption enabled", "[nvs]")
{
    nvs_handle_t handle_1;
    const uint32_t NVS_FLASH_SECTOR = 6;
    const uint32_t NVS_FLASH_SECTOR_COUNT_MIN = 3;

    nvs_sec_cfg_t xts_cfg;
    for(int count = 0; count < NVS_KEY_SIZE; count++) {
        xts_cfg.eky[count] = 0x11;
        xts_cfg.tky[count] = 0x22;
    }
    EncryptedPartitionFixture fixture(&xts_cfg, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN);
    fixture.emu.randomize(100);
    fixture.emu.setBounds(NVS_FLASH_SECTOR, NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN);

    for (uint16_t i = NVS_FLASH_SECTOR; i <NVS_FLASH_SECTOR + NVS_FLASH_SECTOR_COUNT_MIN; ++i) {
        fixture.emu.erase(i);
    }
    TEST_ESP_OK( nvs::NVSPartitionManager::get_instance()->
            init_custom(&fixture.part, NVS_FLASH_SECTOR, NVS_FLASH_SECTOR_COUNT_MIN));

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
                    ".", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

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
                        "../nvs_partition_generator", NULL));
        } else {
            CHECK(childpid > 0);
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) == 0);
        }
    }

    SpiFlashEmulator emu("../nvs_partition_generator/partition_encrypted.bin");

    nvs_sec_cfg_t cfg;
    for (int count = 0; count < NVS_KEY_SIZE; count++) {
        cfg.eky[count] = 0x11;
        cfg.tky[count] = 0x22;
    }

    check_nvs_part_gen_args(&emu, NVS_DEFAULT_PART_NAME, 4, "../nvs_partition_generator/testdata/sample_multipage_blob.bin", true, &cfg);

    childpid = fork();
    if (childpid == 0) {
        exit(execlp("rm", " rm",
                    "-rf",
                    "testdata", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

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
                    ".", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

        childpid = fork();

        if (childpid == 0) {
            exit(execlp("rm", " rm",
                        "-rf",
                        "../nvs_partition_generator/keys", NULL));
        } else {
            CHECK(childpid > 0);
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) == 0);

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
                            "../nvs_partition_generator", NULL));

            } else {
                CHECK(childpid > 0);
                waitpid(childpid, &status, 0);
                CHECK(WEXITSTATUS(status) == 0);

            }
        }
    }


    DIR *dir;
    struct dirent *file;
    char *filename;
    char *files;
    char *file_ext;

    dir = opendir("../nvs_partition_generator/keys");
    while ((file = readdir(dir)) != NULL) {
        filename = file->d_name;
        files = strrchr(filename, '.');
        if (files != NULL) {
            file_ext = files + 1;
            if (strncmp(file_ext, "bin", 3) == 0) {
                break;
            }
        }
    }

    std::string encr_file = std::string("../nvs_partition_generator/keys/") + std::string(filename);
    SpiFlashEmulator emu("../nvs_partition_generator/partition_encrypted_using_keygen.bin");

    char buffer[64];
    FILE *fp;

    fp = fopen(encr_file.c_str(), "rb");
    fread(buffer, sizeof(buffer), 1, fp);

    fclose(fp);

    nvs_sec_cfg_t cfg;

    for (int count = 0; count < NVS_KEY_SIZE; count++) {
        cfg.eky[count] = buffer[count] & 255;
        cfg.tky[count] = buffer[count + 32] & 255;
    }

    check_nvs_part_gen_args(&emu, NVS_DEFAULT_PART_NAME, 4, "../nvs_partition_generator/testdata/sample_multipage_blob.bin", true, &cfg);

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
    while ((file = readdir(dir)) != NULL) {
        filename = file->d_name;
        files = strrchr(filename, '.');
        if (files != NULL) {
            file_ext = files + 1;
            if (strncmp(file_ext, "bin", 3) == 0) {
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
                    "../nvs_partition_generator", NULL));

    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);
    }

    SpiFlashEmulator emu("../nvs_partition_generator/partition_encrypted_using_keyfile.bin");

    char buffer[64];
    FILE *fp;

    fp = fopen(encr_file.c_str(), "rb");
    fread(buffer, sizeof(buffer), 1, fp);

    fclose(fp);

    nvs_sec_cfg_t cfg;

    for (int count = 0; count < NVS_KEY_SIZE; count++) {
        cfg.eky[count] = buffer[count] & 255;
        cfg.tky[count] = buffer[count + 32] & 255;
    }

    check_nvs_part_gen_args(&emu, NVS_DEFAULT_PART_NAME, 4, "../nvs_partition_generator/testdata/sample_multipage_blob.bin", true, &cfg);

    childpid = fork();
    if (childpid == 0) {
        exit(execlp("rm", " rm",
                    "-rf",
                    "../nvs_partition_generator/keys", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

        childpid = fork();

        if (childpid == 0) {
            exit(execlp("rm", " rm",
                        "-rf",
                        "testdata", NULL));
        } else {
            CHECK(childpid > 0);
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) == 0);
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
                    mkdir -p ../../../tools/mass_mfg/host_test", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

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
                        "mfg_testdata/sample_encryption_keys.bin", NULL));

        } else {
            CHECK(childpid > 0);
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) == 0);

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
                            "testdata/sample_encryption_keys.bin", NULL));

            } else {
                CHECK(childpid > 0);
                waitpid(childpid, &status, 0);
                CHECK(WEXITSTATUS(status) == 0);

            }

        }

    }

    SpiFlashEmulator emu1("../../../tools/mass_mfg/host_test/bin/Test-1.bin");

    nvs_sec_cfg_t cfg;
    for (int count = 0; count < NVS_KEY_SIZE; count++) {
        cfg.eky[count] = 0x11;
        cfg.tky[count] = 0x22;
    }

    check_nvs_part_gen_args_mfg(&emu1, NVS_DEFAULT_PART_NAME, 4, "mfg_testdata/sample_multipage_blob.bin", true, &cfg);

    SpiFlashEmulator emu2("../nvs_partition_generator/Test-1-partition-encrypted.bin");

    check_nvs_part_gen_args_mfg(&emu2, NVS_DEFAULT_PART_NAME, 4, "testdata/sample_multipage_blob.bin", true, &cfg);


    childpid = fork();
    if (childpid == 0) {
        exit(execlp("bash", " bash",
                    "-c",
                    "rm -rf ../../../tools/mass_mfg/host_test | \
                    rm -rf mfg_testdata | \
                    rm -rf testdata", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

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
                    mkdir -p ../../../tools/mass_mfg/host_test", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

        childpid = fork();
        if (childpid == 0) {
            exit(execlp("python", "python",
                        "../../../tools/mass_mfg/mfg_gen.py",
                        "generate-key",
                        "--outdir",
                        "../../../tools/mass_mfg/host_test",
                        "--keyfile",
                        "encr_keys_host_test.bin", NULL));

        } else {
            CHECK(childpid > 0);
            waitpid(childpid, &status, 0);
            CHECK(WEXITSTATUS(status) == 0);

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
                            "../../../tools/mass_mfg/host_test/keys/encr_keys_host_test.bin", NULL));

            } else {
                CHECK(childpid > 0);
                waitpid(childpid, &status, 0);
                CHECK(WEXITSTATUS(status) == 0);

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
                                "../../../tools/mass_mfg/host_test/keys/encr_keys_host_test.bin", NULL));

                } else {
                    CHECK(childpid > 0);
                    waitpid(childpid, &status, 0);
                    CHECK(WEXITSTATUS(status) == 0);

                }

            }

        }

    }


    SpiFlashEmulator emu1("../../../tools/mass_mfg/host_test/bin/Test-1.bin");

    char buffer[64];
    FILE *fp;

    fp = fopen("../../../tools/mass_mfg/host_test/keys/encr_keys_host_test.bin", "rb");
    fread(buffer, sizeof(buffer), 1, fp);

    fclose(fp);

    nvs_sec_cfg_t cfg;

    for (int count = 0; count < NVS_KEY_SIZE; count++) {
        cfg.eky[count] = buffer[count] & 255;
        cfg.tky[count] = buffer[count + 32] & 255;
    }

    check_nvs_part_gen_args_mfg(&emu1, NVS_DEFAULT_PART_NAME, 4, "mfg_testdata/sample_multipage_blob.bin", true, &cfg);

    SpiFlashEmulator emu2("../nvs_partition_generator/Test-1-partition-encrypted.bin");

    check_nvs_part_gen_args_mfg(&emu2, NVS_DEFAULT_PART_NAME, 4, "testdata/sample_multipage_blob.bin", true, &cfg);

    childpid = fork();
    if (childpid == 0) {
        exit(execlp("bash", " bash",
                    "-c",
                    "rm -rf keys | \
                    rm -rf mfg_testdata | \
                    rm -rf testdata | \
                    rm -rf ../../../tools/mass_mfg/host_test", NULL));
    } else {
        CHECK(childpid > 0);
        waitpid(childpid, &status, 0);
        CHECK(WEXITSTATUS(status) == 0);

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
