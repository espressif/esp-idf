/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <catch2/catch_test_macros.hpp>
#include "nvs_flash.h"
#include "nvs_storage.hpp"
#include "nvs_partition_manager.hpp"
#include "nvs_page.hpp"
#include "test_fixtures.hpp"
#include <vector>
#include <cstdio>

using namespace std;

#define TEST_DEFAULT_PARTITION_NAME "nvs"
#define TEST_DEFAULT_PURGE_AFTER_ERASE true        // erase with purge after erase

// Counts how many pages within [0, sectorCount) currently hold a BLOB_DATA entry
// for the given <nsIndex, key> and the exact chunkIndex (the chunk version is encoded in the
// most significant bit of the chunk index: VER_0_OFFSET -> 0x00.., VER_1_OFFSET -> 0x80..).
static int count_blob_data_chunk(NVSPartitionTestHelper &h, uint32_t sectorCount, uint8_t nsIndex, const char *key, uint8_t chunkIndex)
{
    int count = 0;
    for (uint32_t sector = 0; sector < sectorCount; ++sector) {
        nvs::Page page;
        if (page.load(&h, sector) != ESP_OK) {
            continue;
        }
        if (page.findItem(nsIndex, nvs::ItemType::BLOB_DATA, key, chunkIndex) == ESP_OK) {
            count++;
        }
    }
    return count;
}

TEST_CASE("Regression: failed multi-page blob update must not leak new-version chunks", "[nvs_storage]")
{
    // Intent validated by this regression test:
    //
    // When an existing multi-page blob is overwritten, NVS writes the new value as a new version while
    // keeping the previous value intact until the new one is fully stored. If the write is interrupted
    // before completion, NVS must roll the operation back completely: every partially written chunk of
    // the new value has to be removed and the previous value has to remain intact and readable.
    //
    // This test overwrites an existing two-chunk blob and interrupts the write after the first chunk of
    // the new value has been stored. It then verifies that no chunk of the new value survives and that
    // the original value can still be read back.
    //
    // The write is made to fail naturally: the partition is almost completely filled beforehand so that
    // the update runs out of space (ESP_ERR_NVS_NOT_ENOUGH_SPACE) after the first chunk of the new value
    // has been written but before the second one can be stored. The remaining free space is arranged so
    // that no page can be reclaimed during the update, keeping this scenario independent from the
    // reclamation scenario covered by the next regression test.

    const uint32_t sectorCount = 8;                            // enough sectors for the fill-up below
    const size_t blob_size = nvs::Page::CHUNK_MAX_SIZE * 2;    // spans two data chunks
    // Amount of free space (in entries) that must remain right before the update. It is small enough
    // that the update cannot store the whole new value, yet large enough that its first chunk still
    // fits, so the write fails only after that first chunk has been written.
    const size_t leave_available_entries = 64;

    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);
    REQUIRE(h.get_sectors() >= sectorCount);

    nvs::Storage storage(&h);
    REQUIRE(storage.init(0, sectorCount) == ESP_OK);
    uint8_t ns;
    REQUIRE(storage.createOrOpenNamespace("ns", true, ns) == ESP_OK);

    std::vector<uint8_t> original(blob_size, 0xAA);
    std::vector<uint8_t> updated(blob_size, 0xBB);

    // Store the initial value (first version -> chunk indexes 0x00 and 0x01).
    REQUIRE(storage.writeItem(ns, nvs::ItemType::BLOB, "blob", original.data(), blob_size, TEST_DEFAULT_PURGE_AFTER_ERASE) == ESP_OK);
    REQUIRE(count_blob_data_chunk(h, sectorCount, ns, "blob", 0x00) == 1);
    REQUIRE(count_blob_data_chunk(h, sectorCount, ns, "blob", 0x01) == 1);

    // Fill the rest of the partition with small single-entry items until only a little free space is
    // left. Nothing is erased, so no reclaimable (stale) space exists when the update runs.
    nvs_stats_t stats;
    for (int i = 0; ; ++i) {
        REQUIRE(storage.fillStats(stats) == ESP_OK);
        if (stats.available_entries <= leave_available_entries) {
            break;
        }
        char filler_key[18];
        snprintf(filler_key, sizeof(filler_key), "filler%05d", i);
        const uint8_t filler_value = 0x11;
        REQUIRE(storage.writeItem(ns, nvs::ItemType::U8, filler_key, &filler_value, sizeof(filler_value), TEST_DEFAULT_PURGE_AFTER_ERASE) == ESP_OK);
    }

    // Overwrite the value (second version -> chunk indexes 0x80 and 0x81). The write runs out of space
    // after the first new chunk has been stored, which triggers the rollback of the aborted update.
    esp_err_t err = storage.writeItem(ns, nvs::ItemType::BLOB, "blob", updated.data(), blob_size, TEST_DEFAULT_PURGE_AFTER_ERASE);
    CHECK(err == ESP_ERR_NVS_NOT_ENOUGH_SPACE);

    // Intended failure point: the rollback must remove every chunk of the aborted new value.
    CHECK(count_blob_data_chunk(h, sectorCount, ns, "blob", 0x80) == 0);
    CHECK(count_blob_data_chunk(h, sectorCount, ns, "blob", 0x81) == 0);

    // The original value must be left intact and readable after the failed update.
    std::vector<uint8_t> read_back(blob_size, 0x00);
    CHECK(storage.readItem(ns, nvs::ItemType::BLOB, "blob", read_back.data(), blob_size) == ESP_OK);
    CHECK(read_back == original);
}

TEST_CASE("Regression: failed multi-page blob write must not leak a chunk relocated by reclamation", "[nvs_storage]")
{
    // Intent validated by this regression test:
    //
    // While storing a multi-page blob, NVS may relocate already-written data to another page as part of
    // reclaiming free space. If the write is subsequently interrupted, the rollback of the aborted write
    // must remove every chunk that was written, including any chunk that was relocated in the meantime.
    // No data of the aborted blob may be left behind on any page.
    //
    // This test prepares the partition so that the page receiving the first chunk is later reclaimed
    // (and its content relocated) while the following chunks are written, then interrupts the write and
    // verifies that no chunk of the aborted blob survives anywhere.
    //
    // A brand new blob (its first version) is used so that this scenario is independent from the
    // new-version rollback covered by the previous regression test.
    //
    // The write is made to fail naturally: with only three sectors and a page full of stale entries,
    // the blob is sized so that after its first chunk is relocated by reclamation, the write still runs
    // out of space (ESP_ERR_NVS_NOT_ENOUGH_SPACE) before it can be completed.

    const uint32_t sectorCount = 3;             // minimal free space so reclamation must happen mid-write
    // Sized so that the first chunk lands on the prepared page, is relocated by reclamation while the
    // following chunks are written, and the write then runs out of space before it can complete.
    const size_t blob_size = 7912;

    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);
    REQUIRE(h.get_sectors() >= sectorCount);

    nvs::Storage storage(&h);
    REQUIRE(storage.init(0, sectorCount) == ESP_OK);
    uint8_t ns;
    REQUIRE(storage.createOrOpenNamespace("ns", true, ns) == ESP_OK);

    // Prepare the active page so that it holds only a couple of live entries but many stale (erased)
    // ones near the end of the page. Such a page becomes the natural candidate for space reclamation,
    // which relocates its live content (including the blob's first chunk) to another page.
    const uint8_t small_value = 0x11;
    for (int i = 0; i < 100; ++i) {
        char key[16];
        snprintf(key, sizeof(key), "small%03d", i);
        REQUIRE(storage.writeItem(ns, nvs::ItemType::U8, key, &small_value, sizeof(small_value), TEST_DEFAULT_PURGE_AFTER_ERASE) == ESP_OK);
    }
    for (int i = 2; i < 100; ++i) {
        char key[16];
        snprintf(key, sizeof(key), "small%03d", i);
        REQUIRE(storage.eraseItem(ns, nvs::ItemType::U8, key, TEST_DEFAULT_PURGE_AFTER_ERASE) == ESP_OK);
    }

    // Write a fresh multi-page blob. Its first chunk lands on the prepared page, that page is reclaimed
    // and its content relocated while the remaining chunks are written, and the write then runs out of
    // space, which triggers the rollback of the aborted write.
    std::vector<uint8_t> blob(blob_size, 0xCC);
    esp_err_t err = storage.writeItem(ns, nvs::ItemType::BLOB, "big", blob.data(), blob_size, TEST_DEFAULT_PURGE_AFTER_ERASE);
    CHECK(err == ESP_ERR_NVS_NOT_ENOUGH_SPACE);

    // Intended failure point: the rollback must remove every chunk of the aborted blob, including the
    // relocated first chunk (0x00).
    CHECK(count_blob_data_chunk(h, sectorCount, ns, "big", 0x00) == 0);
    CHECK(count_blob_data_chunk(h, sectorCount, ns, "big", 0x01) == 0);
    CHECK(count_blob_data_chunk(h, sectorCount, ns, "big", 0x02) == 0);
}

TEST_CASE("Regression: failed multi-page blob update must not destroy the previous value", "[nvs_storage]")
{
    // Intent validated by this regression test:
    //
    // Overwriting an existing multi-page blob must be atomic from the reader's point of view: until the new
    // value has been stored completely, the previously stored value must stay intact and readable. If the
    // update is interrupted before it finishes, NVS must roll back only the partially written new value and
    // leave the previous value untouched. Under no circumstances may the rollback of the aborted update
    // remove data that belongs to the previous, still-valid value.
    //
    // This test overwrites an existing two-chunk blob under space pressure. It is arranged so that, while the
    // new value is being written, NVS relocates the previous value's first chunk to another page as part of
    // reclaiming free space, and that relocated chunk ends up sharing a page with the first chunk of the new
    // value. The update then runs out of space and is rolled back. The test verifies that the previous
    // value's first chunk survives and that the previous value can still be read back unchanged.
    //
    // The write is made to fail naturally: the partition is filled beforehand so that the update runs out of
    // space (ESP_ERR_NVS_NOT_ENOUGH_SPACE) after the first chunk of the new value has been written.
    //
    // NOTE: This test is expected to FAIL on the current implementation. The intended failure point is the
    // check that the previous value's first chunk still exists after the aborted update.

    const uint32_t sectorCount = 3;                            // tight space so reclamation must happen mid-write
    const size_t blob_size = nvs::Page::CHUNK_MAX_SIZE + 1000; // spans two data chunks (0x00 and 0x01)
    const int filler_count = 90;                               // small entries co-located with the blob's first chunk

    NVSPartitionTestHelper h(TEST_DEFAULT_PARTITION_NAME);
    REQUIRE(h.get_sectors() >= sectorCount);

    nvs::Storage storage(&h);
    REQUIRE(storage.init(0, sectorCount) == ESP_OK);
    uint8_t ns;
    REQUIRE(storage.createOrOpenNamespace("ns", true, ns) == ESP_OK);

    // Fill most of the first page with small entries so that the blob's first chunk is forced to share that
    // page with them (instead of getting a page of its own).
    const uint8_t small_value = 0x11;
    for (int i = 0; i < filler_count; ++i) {
        char key[16];
        snprintf(key, sizeof(key), "small%03d", i);
        REQUIRE(storage.writeItem(ns, nvs::ItemType::U8, key, &small_value, sizeof(small_value), TEST_DEFAULT_PURGE_AFTER_ERASE) == ESP_OK);
    }

    // Store the initial value (first version -> chunk indexes 0x00 and 0x01). Its first chunk lands on the
    // page filled above, next to the small entries.
    std::vector<uint8_t> original(blob_size, 0xAA);
    REQUIRE(storage.writeItem(ns, nvs::ItemType::BLOB, "blob", original.data(), blob_size, TEST_DEFAULT_PURGE_AFTER_ERASE) == ESP_OK);
    REQUIRE(count_blob_data_chunk(h, sectorCount, ns, "blob", 0x00) == 1);
    REQUIRE(count_blob_data_chunk(h, sectorCount, ns, "blob", 0x01) == 1);
    std::vector<uint8_t> verify(blob_size, 0x00);
    REQUIRE(storage.readItem(ns, nvs::ItemType::BLOB, "blob", verify.data(), blob_size) == ESP_OK);
    REQUIRE(verify == original);

    // Erase the small entries. Their page now holds only the blob's first chunk plus a lot of stale space,
    // which makes it the natural candidate for reclamation. When the update below reclaims it, the blob's
    // first chunk is relocated onto the page that also receives the first chunk of the new value.
    for (int i = 0; i < filler_count; ++i) {
        char key[16];
        snprintf(key, sizeof(key), "small%03d", i);
        REQUIRE(storage.eraseItem(ns, nvs::ItemType::U8, key, TEST_DEFAULT_PURGE_AFTER_ERASE) == ESP_OK);
    }

    // Overwrite the value (second version -> chunk indexes 0x80 and 0x81). The write runs out of space after
    // the first new chunk has been stored, which triggers the rollback of the aborted update.
    std::vector<uint8_t> updated(blob_size, 0xBB);
    esp_err_t err = storage.writeItem(ns, nvs::ItemType::BLOB, "blob", updated.data(), blob_size, TEST_DEFAULT_PURGE_AFTER_ERASE);
    CHECK(err == ESP_ERR_NVS_NOT_ENOUGH_SPACE);

    // Intended failure point: the rollback must not touch the previous value, so its first chunk (0x00) must
    // still be present. (These counts are checked before the read-back below, because a failed read may
    // rewrite the on-flash layout.)
    CHECK(count_blob_data_chunk(h, sectorCount, ns, "blob", 0x00) == 1);

    // The previous value must remain intact and readable after the failed update.
    std::vector<uint8_t> read_back(blob_size, 0x00);
    esp_err_t read_err = storage.readItem(ns, nvs::ItemType::BLOB, "blob", read_back.data(), blob_size);
    CHECK(read_err == ESP_OK);
    if (read_err == ESP_OK) {
        CHECK(read_back == original);
    }
}

TEST_CASE("Storage iterator recognizes blob with VerOffset::VER_1_OFFSET", "[nvs_storage]")
{
    // Positive TC for Storage iterator recognizing a blob with VerOffset::VER_1_OFFSET

    // Erase the partition before testing
    REQUIRE(nvs_flash_erase_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);

    // Init the default partition
    REQUIRE(nvs_flash_init_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);

    uint8_t blob [] = {0x0, 0x1, 0x2, 0x3};
    uint8_t blob_new [] = {0x3, 0x2, 0x1, 0x0};
    nvs::Storage *storage = nvs::NVSPartitionManager::get_instance()->lookup_storage_from_name(TEST_DEFAULT_PARTITION_NAME);
    uint8_t ns_index;

    REQUIRE(storage != nullptr);
    storage->createOrOpenNamespace("test_ns", true, ns_index);

    CHECK(storage->writeItem(ns_index, nvs::ItemType::BLOB, "test_blob", blob, sizeof(blob), TEST_DEFAULT_PURGE_AFTER_ERASE) == ESP_OK);

    // changing provokes a blob with version offset 1 (VerOffset::VER_1_OFFSET)
    CHECK(storage->writeItem(ns_index, nvs::ItemType::BLOB, "test_blob", blob_new, sizeof(blob_new), TEST_DEFAULT_PURGE_AFTER_ERASE) == ESP_OK);
    nvs_opaque_iterator_t it;
    it.storage = storage;
    it.type = NVS_TYPE_ANY;

    // Central check: does the iterator recognize the blob with version 1?
    REQUIRE(storage->findEntry(&it, "test_ns"));

    CHECK(string(it.entry_info.namespace_name) == string("test_ns"));
    CHECK(string(it.entry_info.key) == string("test_blob"));
    CHECK(it.entry_info.type == NVS_TYPE_BLOB);

    REQUIRE(nvs_flash_deinit_partition(TEST_DEFAULT_PARTITION_NAME) == ESP_OK);
}
