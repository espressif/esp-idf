/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "unity.h"
#include "unity_fixture.h"
#include "sd_protocol_defs.h"
#include "sdmmc_cmd.h"
#include "sdmmc_test_begin_end_sd.h"
#include "esp_blockdev.h"

TEST_GROUP(sdmmc);

TEST_SETUP(sdmmc)
{
}

TEST_TEAR_DOWN(sdmmc)
{
}

TEST(sdmmc, test_bdl_interface)
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

TEST_GROUP_RUNNER(sdmmc)
{
    RUN_TEST_CASE(sdmmc, test_bdl_interface)
}

void app_main(void)
{
    UNITY_MAIN(sdmmc);
}
