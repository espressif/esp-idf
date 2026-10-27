/*
 * SPDX-FileCopyrightText: 2016-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "esp_partition.h"
#include "esp_private/partition_linux.h"

#include "wear_levelling.h"
#include "WL_Flash.h"
#include "crc32.h"


#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "sdkconfig.h"

#include "esp_log.h"
static const char *TAG = "test_wl";

// Number of test cycles. Prime number close to 100
#define TEST_COUNT_MAX 101

// Number of erase operations until emulated power off error is raised
// Prime number close to 100
#define ERASE_CYCLES_TILL_POWER_OFF 97

TEST_CASE("write and read back data", "[wear_levelling]")
{
    esp_err_t result;
    wl_handle_t wl_handle;

    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");

    // Mount wear-levelled partition
    result = wl_mount(partition, &wl_handle);
    REQUIRE(result == ESP_OK);

    // Get the sector size
    uint32_t sector_size = wl_sector_size(wl_handle);
    REQUIRE(sector_size == CONFIG_WL_SECTOR_SIZE);

    size_t size = wl_size(wl_handle);
    uint8_t* data = (uint8_t*) malloc(size);
    uint8_t* read = (uint8_t*) malloc(size);

    uint32_t sectors = size / sector_size;

    // Generate data
    for(uint32_t sector = 0; sector < sectors; sector++)
    {
        uint32_t sector_address = sector * sector_size;

        for(uint32_t i = 0; i < sector_size / sizeof(i); i++)
        {
            ((uint32_t*) (data + sector_address))[i] = sector_address + i;
        }
    }

    // Write data (erase first, flash may hold data from previous tests)
    result = wl_erase_range(wl_handle, 0, size);
    REQUIRE(result == ESP_OK);
    result = wl_write(wl_handle, 0, data, size);
    REQUIRE(result == ESP_OK);

    // Read data
    result = wl_read(wl_handle, 0, read, size);
    REQUIRE(result == ESP_OK);

    // Verify that written and read data match
    REQUIRE(memcmp(data, read, size) == 0);

    // Erase some ranges
    result = wl_erase_range(wl_handle, 0, sector_size);
    REQUIRE(result == ESP_OK);
    result = wl_erase_range(wl_handle, 12288, sector_size * 2);
    REQUIRE(result == ESP_OK);
    result = wl_erase_range(wl_handle, 28672, sector_size * 3);
    REQUIRE(result == ESP_OK);

    // Expected data after erasure
    memset(data + 0, 0xFF, sector_size);
    memset(data + 12288, 0xFF, sector_size * 2);
    memset(data + 28672, 0xFF, sector_size * 3);

    // Read again, with erased ranges
    result = wl_read(wl_handle, 0, read, size);
    REQUIRE(result == ESP_OK);

    // Verify that written and read data match
    REQUIRE(memcmp(data, read, size) == 0);

    // Unmount
    result = wl_unmount(wl_handle);
    REQUIRE(result == ESP_OK);

    free(data);
    free(read);
}

TEST_CASE("write and read with zero size are safe no-ops", "[wear_levelling]")
{
    esp_err_t result;
    wl_handle_t wl_handle;

    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");

    // Mount wear-levelled partition
    result = wl_mount(partition, &wl_handle);
    REQUIRE(result == ESP_OK);

    // Zero-length wl_write/read must be no-ops; size==0 used to underflow (size-1) and OOB the buffer.
    uint8_t dummy = 0xAA;
    result = wl_write(wl_handle, 0, &dummy, 0);
    REQUIRE(result == ESP_OK);

    uint8_t read_back = 0x55;
    result = wl_read(wl_handle, 0, &read_back, 0);
    REQUIRE(result == ESP_OK);

    // Untouched by a genuine zero-length read.
    REQUIRE(read_back == 0x55);

    // Unmount
    result = wl_unmount(wl_handle);
    REQUIRE(result == ESP_OK);
}

// Position record size, read from the wl_config_t that WL stores in the
// last sector of the partition. Valid once the partition has been mounted.
static size_t wl_pos_record_size(const esp_partition_t *partition)
{
    wl_config_t cfg;
    REQUIRE(esp_partition_read(partition, partition->size - partition->erase_size, &cfg, sizeof(cfg)) == ESP_OK);
    // WL_Flash::fillOkBuff() writes 4 x uint32_t into each record.
    REQUIRE(cfg.wl_pos_update_record_size >= 4 * sizeof(uint32_t));
    return cfg.wl_pos_update_record_size;
}

// Offset of the first WL state copy, mirroring WL_Flash::config() for
// wl_partition_start_addr == 0 and a one-sector wl_config_t.
static size_t wl_state1_addr(const esp_partition_t *partition)
{
    const size_t sector = partition->erase_size;
    size_t state_size = sizeof(wl_state_t) + (partition->size / sector) * wl_pos_record_size(partition);
    state_size = (state_size + sector - 1) / sector * sector;
    return partition->size - 2 * state_size - sector;
}

// Read the dummy sector position and move count from flash. The position is
// not kept in wl_state_t on flash; like WL_Flash::recoverPos(), count the
// position records written after the state structure (erased record = end).
static void wl_read_dummy_pos(const esp_partition_t *partition, uint32_t *pos, uint32_t *move_count)
{
    const size_t state1 = wl_state1_addr(partition);
    const size_t record_size = wl_pos_record_size(partition);
    wl_state_t state;
    REQUIRE(esp_partition_read(partition, state1, &state, sizeof(state)) == ESP_OK);
    *move_count = state.wl_dummy_sec_move_count;

    // Only the first 4 x uint32_t of a record are written.
    uint8_t record[4 * sizeof(uint32_t)];
    uint8_t erased[sizeof(record)];
    memset(erased, 0xFF, sizeof(erased));
    *pos = 0;
    while (true) {
        REQUIRE(esp_partition_read(partition, state1 + sizeof(state) + *pos * record_size, record, sizeof(record)) == ESP_OK);
        if (memcmp(record, erased, sizeof(record)) == 0) {
            break;
        }
        (*pos)++;
    }
}

// Move the dummy sector n positions. wl_unmount() calls WL_Flash::flush(),
// which moves it exactly once.
static void wl_move_dummy(const esp_partition_t *partition, wl_handle_t *handle, size_t n)
{
    for (size_t i = 0; i < n; i++) {
        REQUIRE(wl_unmount(*handle) == ESP_OK);
        REQUIRE(wl_mount(partition, handle) == ESP_OK);
    }
}

// Regression test: unaligned wl_read()/wl_write() crossing a WL page boundary
// used to hit the wrong physical page when the dummy sector sat in between.
// Catch2 reruns the body, setup included, for each GENERATE() combination.
TEST_CASE("unaligned read/write across WL page boundary", "[wear_levelling]")
{
    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
    const size_t page = 4096;   // WL page size (flash sector)
    const size_t region = 4 * page;
    static uint8_t write_buf[region];
    static uint8_t read_buf[region];
    static uint8_t expected[region];
    wl_handle_t handle;

    auto [offset, size] = GENERATE_COPY(table<size_t, size_t>({
        { 0,            page },             // aligned, exactly one page
        { 1,            1 },                // unaligned, inside one page
        { page - 1,     1 },                // last byte of a page
        { page / 2,     page / 2 },         // unaligned, ends on page boundary
        { page - 1,     2 },                // crosses boundary by one byte
        { page - 1,     page + 2 },         // crosses two boundaries
        { 1,            3 * page - 1 },     // unaligned start, ends on page boundary
        { page + 3,     2 * page + 5 },     // starts in logical page 1
    }));
    // Dummy sector at physical page 1 sits under the unaligned head; at
    // physical page 2 it sits under the page-aligned remainder.
    const uint32_t dummy_pos = GENERATE(1, 2);
    CAPTURE(offset, size, dummy_pos);

    // Unique byte per position, so data landing at a wrong offset is detected.
    for (size_t i = 0; i < region; i++) {
        write_buf[i] = (uint8_t)(i * 7 + 1);
    }

    // Fresh WL state (dummy sector at physical page 0), then move the dummy
    // sector to physical page dummy_pos.
    REQUIRE(esp_partition_erase_range(partition, 0, partition->size) == ESP_OK);
    REQUIRE(wl_mount(partition, &handle) == ESP_OK);
    wl_move_dummy(partition, &handle, dummy_pos);
    uint32_t pos;
    uint32_t move_count;
    wl_read_dummy_pos(partition, &pos, &move_count);
    REQUIRE(pos == dummy_pos);
    REQUIRE(move_count == 0);

    // Aligned read of the whole region: only [offset, offset + size) may have
    // changed, the rest stays erased. Unaligned read of the same range: the
    // buffer holds exactly what was written and nothing past size.
    auto check_readback = [&]() {
        REQUIRE(wl_read(handle, 0, read_buf, region) == ESP_OK);
        memset(expected, 0xFF, region);
        memcpy(expected + offset, write_buf, size);
        CHECK(memcmp(read_buf, expected, region) == 0);

        memset(read_buf, 0, region);
        REQUIRE(wl_read(handle, offset, read_buf, size) == ESP_OK);
        memset(expected, 0, region);
        memcpy(expected, write_buf, size);
        CHECK(memcmp(read_buf, expected, region) == 0);
    };

    REQUIRE(wl_write(handle, offset, write_buf, size) == ESP_OK);
    check_readback();

    // Remounting moves the dummy sector once more, relocating a data page.
    // The data must read back the same through the new mapping.
    wl_move_dummy(partition, &handle, 1);
    check_readback();

    REQUIRE(wl_unmount(handle) == ESP_OK);
    // Leave erased flash; other tests write without erasing first.
    REQUIRE(esp_partition_erase_range(partition, 0, partition->size) == ESP_OK);
}

// Regression test: once the dummy sector has completed a full pass
// (wl_dummy_sec_move_count >= 1), logical page 0 maps to the last data page,
// so an unaligned write crossing into logical page 1 used to run past the
// data area and overwrite the WL state, leaving the volume unusable.
TEST_CASE("unaligned write after dummy sector wrap keeps WL state intact", "[wear_levelling]")
{
    const esp_partition_t *storage = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
    // Small partition, so a full dummy sector pass needs few moves.
    esp_partition_t partition = *storage;
    partition.size = 8 * partition.erase_size;
    const size_t page = 4096;   // WL page size (flash sector)
    REQUIRE(partition.erase_size == page);
    const size_t region = 2 * page;
    const size_t offset = page / 2;
    const size_t size = page;
    static uint8_t write_buf[page];
    static uint8_t read_buf[region];
    static uint8_t expected[region];
    wl_handle_t handle;

    for (size_t i = 0; i < size; i++) {
        write_buf[i] = (uint8_t)(i * 7 + 1);
    }

    REQUIRE(esp_partition_erase_range(&partition, 0, partition.size) == ESP_OK);
    REQUIRE(wl_mount(&partition, &handle) == ESP_OK);

    // WL metadata (both state copies and config) starts right after the
    // dummy sector positions, so one move per page below it completes a pass.
    const size_t meta_addr = wl_state1_addr(&partition);
    const size_t meta_size = partition.size - meta_addr;
    wl_move_dummy(&partition, &handle, meta_addr / page);
    uint32_t pos;
    uint32_t move_count;
    wl_read_dummy_pos(&partition, &pos, &move_count);
    REQUIRE(move_count == 1);
    REQUIRE(pos == 0);

    std::vector<uint8_t> meta_before(meta_size);
    std::vector<uint8_t> meta_after(meta_size);
    REQUIRE(esp_partition_read(&partition, meta_addr, meta_before.data(), meta_size) == ESP_OK);
    REQUIRE(wl_write(handle, offset, write_buf, size) == ESP_OK);
    REQUIRE(esp_partition_read(&partition, meta_addr, meta_after.data(), meta_size) == ESP_OK);
    CHECK(meta_before == meta_after);

    memset(expected, 0xFF, region);
    memcpy(expected + offset, write_buf, size);
    REQUIRE(wl_read(handle, 0, read_buf, region) == ESP_OK);
    CHECK(memcmp(read_buf, expected, region) == 0);

    // Unaligned read across the same boundary must not read from the WL state.
    memset(read_buf, 0, size);
    REQUIRE(wl_read(handle, offset, read_buf, size) == ESP_OK);
    CHECK(memcmp(read_buf, write_buf, size) == 0);

    // The volume must still mount, keep the data and accept erases.
    REQUIRE(wl_unmount(handle) == ESP_OK);
    REQUIRE(wl_mount(&partition, &handle) == ESP_OK);
    REQUIRE(wl_read(handle, 0, read_buf, region) == ESP_OK);
    CHECK(memcmp(read_buf, expected, region) == 0);
    REQUIRE(wl_erase_range(handle, 0, wl_size(handle)) == ESP_OK);

    REQUIRE(wl_unmount(handle) == ESP_OK);
    REQUIRE(esp_partition_erase_range(storage, 0, storage->size) == ESP_OK);
}

TEST_CASE("power down test", "[wear_levelling]")
{
    esp_err_t result;
    wl_handle_t wl_handle;

    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");

    // Disable power down failure counting
    esp_partition_fail_after(SIZE_MAX, 0);

    // Mount wear-levelled partition
    result = wl_mount(partition, &wl_handle);
    REQUIRE(result == ESP_OK);

    // Get wl partition information
    size_t sector_size = wl_sector_size(wl_handle);
    int32_t sectors_count = wl_size(wl_handle) / sector_size;

    uint32_t add_const = 0;
    uint32_t *sector_data = new uint32_t[sector_size / sizeof(uint32_t)];

    // Fill partition with check data
    for (int32_t i = 0; i < sectors_count; i++) {
        ESP_LOGV(TAG, "%s(%d): wl_erase_range (*, %lu, %zu)", __FUNCTION__, __LINE__, i * sector_size, sector_size);
        REQUIRE(wl_erase_range(wl_handle, i * sector_size, sector_size) == ESP_OK);
        for (uint32_t m = 0; m < sector_size / sizeof(uint32_t); m++) {
            uint32_t temp_data = i * sector_size + add_const + m;
            sector_data[m] = temp_data;
        }
        ESP_LOGV(TAG, "%s(%d): wl_write (*, %lu, *, %zu)", __FUNCTION__, __LINE__, i * sector_size, sector_size);
        REQUIRE(wl_write(wl_handle, i * sector_size, sector_data, sector_size) == ESP_OK);
    }

    for (int32_t i = 0; i < sectors_count; i++) {
        ESP_LOGV(TAG, "%s(%d): wl_read (*, %lu, *, %zu)", __FUNCTION__, __LINE__, i * sector_size, sector_size);
        result |= wl_read(wl_handle, i * sector_size, sector_data, sector_size);
        for (uint32_t m = 0; m < sector_size / sizeof(uint32_t); m++) {
            uint32_t temp_data = i * sector_size + add_const + m;
            REQUIRE(temp_data == sector_data[m]);
            if (temp_data != sector_data[m]) {
                printf("Error - read: %08x, expected %08x\n", sector_data[m], temp_data);
            }
        }
    }

    // Perform test
    int32_t max_count = ERASE_CYCLES_TILL_POWER_OFF;
    int32_t max_check_count = TEST_COUNT_MAX;

    ESP_LOGI(TAG, "%s(%d): max_check_count = %d)", __FUNCTION__, __LINE__, max_check_count);

    for (int32_t k = 0; k < max_check_count; k++) {

        // Enable power down failure after max_count cycles
        esp_partition_fail_after(max_count, ESP_PARTITION_FAIL_AFTER_MODE_BOTH);

        int32_t err_sector = -1;
        for (int32_t i = 0; i < sectors_count; i++) {
            result = ESP_OK;
            ESP_LOGV(TAG, "%s(%d): wl_erase_range (*, %lu, %zu)", __FUNCTION__, __LINE__, i * sector_size, sector_size);
            result = wl_erase_range(wl_handle, i * sector_size, sector_size);
            if (result != ESP_OK) {
                err_sector = i;
                break;
            }

            for (uint32_t m = 0; m < sector_size / sizeof(uint32_t); m++) {
                uint32_t temp_data = i * sector_size + add_const + m;
                sector_data[m] = temp_data;
            }
            ESP_LOGV(TAG, "%s(%d): wl_write (*, %lu, *, %zu)", __FUNCTION__, __LINE__, i * sector_size, sector_size);
            result = wl_write(wl_handle, i * sector_size, sector_data, sector_size);
            if (result != ESP_OK) {
                err_sector = i;
                break;
            }
        }

        if (err_sector >= 0) {
            max_count++;
        } else {
            max_count = 0;
        }

        // Call unmount, but don't care about the result as the power down failure may be persisting or even arise during the unmount.
        // In real power down scenario, this function won't be called, here in the test, we need it to free wl handles in driver.
        ESP_LOGV(TAG, "%s(%d): wl_unmount", __FUNCTION__, __LINE__);
        wl_unmount(wl_handle);

        // Disable power down failure counting
        esp_partition_fail_after(SIZE_MAX, 0);

        ESP_LOGV(TAG, "%s(%d): wl_mount", __FUNCTION__, __LINE__);
        result = wl_mount(partition, &wl_handle);
        REQUIRE(result == ESP_OK);

        for (int32_t i = 0; i < sectors_count; i++) {
            if (i != err_sector) {
                ESP_LOGV(TAG, "%s(%d): wl_read (*, %lu, *, %zu)", __FUNCTION__, __LINE__, i * sector_size, sector_size);
                result |= wl_read(wl_handle, i * sector_size, sector_data, sector_size);
                for (uint32_t m = 0; m < sector_size / sizeof(uint32_t); m++) {
                    uint32_t temp_data = i * sector_size + add_const + m;
                    REQUIRE(temp_data == sector_data[m]);
                    if (temp_data != sector_data[m]) {
                        printf("Error - read: %08x, expected %08x, m=%i, sector=%i\n", sector_data[m], temp_data, m, i);
                    }
                }
            }
        }

        if (err_sector != -1) {
            ESP_LOGV(TAG, "%s(%d): wl_erase_range (*, %lu, %zu)", __FUNCTION__, __LINE__, err_sector * sector_size, sector_size);
            result |= wl_erase_range(wl_handle, err_sector * sector_size, sector_size);
            for (uint32_t m = 0; m < sector_size / sizeof(uint32_t); m++) {
                uint32_t temp_data = err_sector * sector_size + add_const + m;
                sector_data[m] = temp_data;
            }
            ESP_LOGV(TAG, "%s(%d): wl_write (*, %lu, *, %zu)", __FUNCTION__, __LINE__, err_sector * sector_size, sector_size);
            result |= wl_write(wl_handle, err_sector * sector_size, sector_data, sector_size);
        }
    }

    delete[] sector_data;

    // Unmount
    ESP_LOGV(TAG, "%s(%d): wl_unmount", __FUNCTION__, __LINE__);
    result = wl_unmount(wl_handle);
    REQUIRE(result == ESP_OK);
}

// Calculates wl status blocks offsets and status block size
void calculate_wl_state_address_info(const esp_partition_t *partition, size_t *offset_state_1, size_t *offset_state_2, size_t *state_size)
{
    // This code follows ::init of WL_Flash.cpp
    // and define directives from wear_levelling.cpp

    // get sector size
    esp_err_t result;
    wl_handle_t wl_handle;

    // Try to mount wear-levelled partition
    ESP_LOGD(TAG, "wl_mount");
    result = wl_mount(partition, &wl_handle);
    REQUIRE(result == ESP_OK);

    size_t sector_size = wl_sector_size(wl_handle); //SPI_FLASH_SEC_SIZE 4096;
    REQUIRE(sector_size == CONFIG_WL_SECTOR_SIZE);

    // Unmount
    ESP_LOGD(TAG, "wl_unmount");
    result = wl_unmount(wl_handle);
    REQUIRE(result == ESP_OK);

    // rest of parameters
    size_t full_mem_size = partition->size;
    size_t start_addr = 0; // WL_DEFAULT_START_ADDR   0
    size_t wr_size = 16; // WL_DEFAULT_WRITE_SIZE   16
    size_t cfg_size = 0;


    *state_size = sector_size;
    if (*state_size < (sizeof(wl_state_t) + (full_mem_size / sector_size) * wr_size)) {
        *state_size = ((sizeof(wl_state_t) + (full_mem_size / sector_size) * wr_size) + sector_size - 1) / sector_size;
        *state_size = *state_size * sector_size;
    }
    cfg_size = (sizeof(wl_config_t) + sector_size - 1) / sector_size;
    cfg_size = cfg_size * sector_size;

    *offset_state_1 = start_addr + full_mem_size - *state_size * 2 - cfg_size;
    *offset_state_2 = start_addr + full_mem_size - *state_size * 1 - cfg_size;
}

#ifndef WL_CFG_CRC_CONST
#define WL_CFG_CRC_CONST UINT32_MAX
#endif // WL_CFG_CRC_CONST

// calculates crc of wear levelling state block
void calculate_wl_state_crc(WL_State_s *state_ptr)
{
    int check_size = WL_STATE_CRC_LEN_V2;
    // Check CRC and recover state
    state_ptr->crc32 = crc32::crc32_le(WL_CFG_CRC_CONST, (uint8_t *)state_ptr, check_size);
 }

TEST_CASE("power down during WL status 1 update", "[wear_levelling]")
{
    // Manipulates wl status block 1 as if it wasn't written correctly due to power down event
    // Tries to let such a damaged flash wl_mount (and recover)

    ESP_LOGI(TAG, "power down during WL status 1 update");

    esp_err_t result;
    wl_handle_t wl_handle;

    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");

    size_t offset_state_1, offset_state_2, size_state = 0;

    // get offsets of respective status blocks in flash
    calculate_wl_state_address_info(partition, &offset_state_1, &offset_state_2, &size_state);

    // allocate temporary buffer for status manipulation
    uint8_t* tmp_state = (uint8_t*) malloc(size_state);

    // damage 1st status block
    memset(tmp_state, 0xff, size_state);

    ESP_LOGD(TAG, "esp_partition_erase_range offset: %zu size: %zu", offset_state_1, size_state);
    result = esp_partition_erase_range(partition, offset_state_1, size_state);
    REQUIRE(result == ESP_OK);
    ESP_LOGD(TAG, "esp_partition_write offset: %zu size: %zu", offset_state_1, size_state);
    result = esp_partition_write(partition, offset_state_1, tmp_state, size_state);
    REQUIRE(result == ESP_OK);

    // Try to mount wear-levelled partition
    ESP_LOGD(TAG, "wl_mount");
    result = wl_mount(partition, &wl_handle);
    REQUIRE(result == ESP_OK);

    // Unmount
    ESP_LOGD(TAG, "wl_unmount");
    result = wl_unmount(wl_handle);
    REQUIRE(result == ESP_OK);

    free(tmp_state);
}

TEST_CASE("power down during WL status 2 update", "[wear_levelling]")
{
    // Manipulates wl status block 2 as if it wasn't written correctly due to power down event
    // Tries to let such a damaged flash wl_mount (and recover)

    ESP_LOGI(TAG, "power down during WL status 2 update");

    esp_err_t result;
    wl_handle_t wl_handle;

    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");

    size_t offset_state_1, offset_state_2, size_state = 0;

    // get offsets of respective status blocks in flash
    calculate_wl_state_address_info(partition, &offset_state_1, &offset_state_2, &size_state);

    // allocate temporary buffer for status manipulation
    uint8_t* tmp_state = (uint8_t*) malloc(size_state);

    // damage 2nd status block
    memset(tmp_state, 0xff, size_state);

    ESP_LOGD(TAG, "esp_partition_erase_range offset: %zu size: %zu", offset_state_2, size_state);
    result = esp_partition_erase_range(partition, offset_state_2, size_state);
    REQUIRE(result == ESP_OK);
    ESP_LOGD(TAG, "esp_partition_write offset: %zu size: %zu", offset_state_2, size_state);
    result = esp_partition_write(partition, offset_state_2, tmp_state, size_state);
    REQUIRE(result == ESP_OK);

    // Try to mount wear-levelled partition
    ESP_LOGD(TAG, "wl_mount");
    result = wl_mount(partition, &wl_handle);
    REQUIRE(result == ESP_OK);

    // Unmount
    ESP_LOGD(TAG, "wl_unmount");
    result = wl_unmount(wl_handle);
    REQUIRE(result == ESP_OK);

    free(tmp_state);
}

TEST_CASE("power down between WL status 1 and WL status 2 update", "[wear_levelling]")
{
    // Manipulates wl status block 2 and reclaculates its crc just to have two different ones as if it wasn't updates due to power down event
    // Tries to let such a damaged flash wl_mount (and recover)

    ESP_LOGI(TAG, "power down between WL status 1 and WL status 2 update");

    esp_err_t result;
    wl_handle_t wl_handle;

    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");

    size_t offset_state_1, offset_state_2, size_state = 0;

    // get offsets of respective status blocks in flash
    calculate_wl_state_address_info(partition, &offset_state_1, &offset_state_2, &size_state);

    // allocate temporary buffer for status manipulation
    uint8_t* tmp_state = (uint8_t*) malloc(size_state);

    // unsync 1st and 2nd block state - change move count in 2nd block and recalculate its crc

    // read actual status2
    ESP_LOGD(TAG, "esp_partition_read offset: %zu size: %zu", offset_state_2, size_state);
    result = esp_partition_read(partition, offset_state_2, tmp_state, size_state);
    REQUIRE(result == ESP_OK);

    // change move count and recalc crc
    WL_State_s *state_ptr = (WL_State_s *) tmp_state;
    state_ptr->wl_dummy_sec_move_count++;
    calculate_wl_state_crc(state_ptr);

    // write back modified status2
    ESP_LOGD(TAG, "esp_partition_erase_range offset: %zu size: %zu", offset_state_2, size_state);
    result = esp_partition_erase_range(partition, offset_state_2, size_state);
    REQUIRE(result == ESP_OK);
    ESP_LOGD(TAG, "esp_partition_write offset: %zu size: %zu", offset_state_2, size_state);
    result = esp_partition_write(partition, offset_state_2, tmp_state, size_state);
    REQUIRE(result == ESP_OK);

    // Try to mount wear-levelled partition
    ESP_LOGD(TAG, "wl_mount");
    result = wl_mount(partition, &wl_handle);
    REQUIRE(result == ESP_OK);

    // Unmount
    ESP_LOGD(TAG, "wl_unmount");
    result = wl_unmount(wl_handle);
    REQUIRE(result == ESP_OK);

    free(tmp_state);
}
