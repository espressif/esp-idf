/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "unity.h"
#include "unity_fixture.h"
#include "unity_test_utils.h"
#include "sd_protocol_defs.h"
#include "sdmmc_cmd.h"
#include "sdmmc_test_begin_end_sd.h"
#include "sdmmc_test_rw_common.h"
#include "esp_private/sdmmc_common.h"
#include "esp_blockdev.h"
#include "esp_private/sdmmc_blockdev.h"

#define TEST_MEMORY_LEAK_THRESHOLD (200)

static size_t test_get_bus_width(int slot)
{
    (void)slot;
    return 1;
}

static bool test_check_buffer_alignment(int slot, const void *buf, size_t size)
{
    (void)slot;
    (void)buf;
    (void)size;
    return true;
}

static esp_err_t test_fail_transaction(int slot, sdmmc_command_t *cmdinfo)
{
    (void)slot;
    (void)cmdinfo;
    return ESP_ERR_INVALID_STATE;
}

TEST_CASE("sdmmc card deinit releases allocated aligned buffer", "[sdmmc]")
{
    sdmmc_card_t card = {};
    card.host.flags = SDMMC_HOST_FLAG_ALLOC_ALIGNED_BUF;
    card.host.dma_aligned_buffer = malloc(SDMMC_IO_BLOCK_SIZE);
    TEST_ASSERT_NOT_NULL(card.host.dma_aligned_buffer);

    TEST_ESP_OK(sdmmc_card_deinit(&card));
    TEST_ASSERT_NULL(card.host.dma_aligned_buffer);
    TEST_ESP_OK(sdmmc_card_deinit(&card));

    void *caller_owned_buffer = malloc(SDMMC_IO_BLOCK_SIZE);
    TEST_ASSERT_NOT_NULL(caller_owned_buffer);
    card.host.flags = 0;
    card.host.dma_aligned_buffer = caller_owned_buffer;
    TEST_ESP_OK(sdmmc_card_deinit(&card));
    TEST_ASSERT_EQUAL_PTR(caller_owned_buffer, card.host.dma_aligned_buffer);
    free(caller_owned_buffer);

    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, sdmmc_card_deinit(NULL));
}

TEST_CASE("sdmmc card init releases allocated aligned buffer on failure", "[sdmmc]")
{
    sdmmc_host_t host = {
        .flags = SDMMC_HOST_FLAG_1BIT | SDMMC_HOST_FLAG_ALLOC_ALIGNED_BUF,
        .get_bus_width = test_get_bus_width,
        .do_transaction = test_fail_transaction,
        .check_buffer_alignment = test_check_buffer_alignment,
    };
    sdmmc_card_t card;

    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, sdmmc_card_init(&host, &card));
    TEST_ASSERT_NULL(card.host.dma_aligned_buffer);
}

TEST_CASE("sdmmc card init rejects preset buffer with ALLOC_ALIGNED_BUF", "[sdmmc]")
{
    void *caller_owned_buffer = malloc(SDMMC_IO_BLOCK_SIZE);
    TEST_ASSERT_NOT_NULL(caller_owned_buffer);

    sdmmc_host_t host = {
        .flags = SDMMC_HOST_FLAG_1BIT | SDMMC_HOST_FLAG_ALLOC_ALIGNED_BUF,
        .get_bus_width = test_get_bus_width,
        .do_transaction = test_fail_transaction,
        .check_buffer_alignment = test_check_buffer_alignment,
        .dma_aligned_buffer = caller_owned_buffer,
    };
    sdmmc_card_t card;
    memset(&card, 0xAA, sizeof(card));

    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, sdmmc_card_init(&host, &card));
    /* card must be left untouched, and the caller keeps ownership of the buffer */
    TEST_ASSERT_EQUAL_PTR(caller_owned_buffer, host.dma_aligned_buffer);
    for (size_t i = 0; i < sizeof(card); i++) {
        TEST_ASSERT_EQUAL_UINT8(0xAA, ((uint8_t *)&card)[i]);
    }

    free(caller_owned_buffer);
}

TEST_CASE("sdmmc blockdev converts byte ranges to sectors", "[sdmmc]")
{
    size_t start_sector;
    size_t sector_count;

    TEST_ESP_OK(sdmmc_blockdev_calculate_sectors(512, UINT64_C(0x100000000), 1024,
                                                 &start_sector, &sector_count));
    TEST_ASSERT_EQUAL_UINT32(8388608, start_sector);
    TEST_ASSERT_EQUAL_UINT32(2, sector_count);

    TEST_ESP_OK(sdmmc_blockdev_calculate_sectors(512, UINT64_C(0x100000000), 0,
                                                 &start_sector, &sector_count));
    TEST_ASSERT_EQUAL_UINT32(8388608, start_sector);
    TEST_ASSERT_EQUAL_UINT32(0, sector_count);

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG,
                      sdmmc_blockdev_calculate_sectors(512, 1, 512, NULL, NULL));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG,
                      sdmmc_blockdev_calculate_sectors(512, 0, 1, NULL, NULL));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG,
                      sdmmc_blockdev_calculate_sectors(0, 0, 512, NULL, NULL));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_SIZE,
                      sdmmc_blockdev_calculate_sectors(512, UINT64_MAX - 511, 512, NULL, NULL));
}

TEST_CASE("sdmmc extra, bdl interface test", "[sdcard]")
{
    sdmmc_card_t card;
    int slot = 1;
    int width = 1;
    int freq_khz = SDMMC_FREQ_DEFAULT;
    sdmmc_test_sd_skip_if_board_incompatible(slot, width, freq_khz, 0, 0);
    sdmmc_test_sd_begin(slot, width, freq_khz, 0, &card);

    uint8_t erased_val = card.scr.erase_mem_state == 0 ? 0 : 255;  // SD card marks erased space either 0 or as 0xFF (255)

    //get the block-device interface instance
    esp_blockdev_handle_t sdmmc_blockdev = NULL;
    TEST_ESP_OK(sdmmc_get_blockdev(&card, &sdmmc_blockdev));
    TEST_ASSERT_NOT_NULL(sdmmc_blockdev);

    const uint64_t target_addr = 512 * 10;// a multiple of 512 (sector size)
    const size_t data_size = 1024; // a multiple of 512 (sector size)

    //write to the blockdev
    uint8_t* test_data = malloc(data_size);
    memset((void*)test_data, 'A', data_size);
    TEST_ESP_OK(sdmmc_blockdev->ops->write(sdmmc_blockdev, test_data, target_addr, data_size));

    //read from the blockdev the data written before
    uint8_t* data_buffer = malloc(data_size);
    memset((void*)data_buffer, 'X', data_size);
    TEST_ESP_OK(sdmmc_blockdev->ops->read(sdmmc_blockdev, data_buffer, data_size, target_addr, data_size));
    TEST_ASSERT_EQUAL(0, memcmp(test_data, data_buffer, data_size));

    //erase the data from the blockdev and check it's really wiped
    TEST_ESP_OK(sdmmc_blockdev->ops->erase(sdmmc_blockdev, target_addr, data_size));
    TEST_ESP_OK(sdmmc_blockdev->ops->read(sdmmc_blockdev, data_buffer, data_size, target_addr, data_size));
    {
        uint8_t ten_erased[10] = {[0 ... 9] = erased_val};
        TEST_ASSERT_EQUAL(0, memcmp(ten_erased, data_buffer, 10));
    }
    free(data_buffer);
    free(test_data);

    //release the BDL object
    TEST_ESP_OK(sdmmc_blockdev->ops->release(sdmmc_blockdev));
    sdmmc_test_sd_end(&card);
}

TEST_CASE("sdmmc extra, multiblock unaligned rw test", "[sdcard]")
{
    sdmmc_card_t card;
    int slot = 1;
    int width = 1;
    int freq_khz = SDMMC_FREQ_DEFAULT;
    sdmmc_test_sd_skip_if_board_incompatible(slot, width, freq_khz, 0, 0);
    sdmmc_test_sd_begin(slot, width, freq_khz, 0, &card);
    sdmmc_test_rw_unaligned_buffer_multiblock(&card, 4);
    sdmmc_test_sd_end(&card);
}

void setUp(void)
{
    unity_utils_record_free_mem();
}

void tearDown(void)
{
    unity_utils_evaluate_leaks_direct(TEST_MEMORY_LEAK_THRESHOLD);
}

void app_main(void)
{
    unity_run_menu();
}
