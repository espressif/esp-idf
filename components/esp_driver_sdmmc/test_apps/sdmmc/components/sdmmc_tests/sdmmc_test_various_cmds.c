/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include <stdlib.h>
#include <string.h>
#include "unity.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/sdmmc_defs.h"
#include "driver/sdmmc_host.h"
#include "sdmmc_cmd.h"
#include "esp_private/sdmmc_common.h"
#include "sdmmc_test_begin_end_sd.h"

#if !SOC_SDMMC_HOST_SUPPORTED
#error "Targets with SDMMC host supported only"
#endif // !SOC_SDMMC_HOST_SUPPORTED

static const char* TAG = "sdmmc_test_various_cmds";

static void sdmmc_write_sectors_cmd25_error_test_acmd22(sdmmc_card_t* card, uint32_t write_size)
{
    esp_err_t err = ESP_OK;
    size_t block_size = (size_t)card->csd.sector_size;
    size_t block_count = write_size / block_size;
    void* buf = heap_caps_calloc(1, write_size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(buf);

    if (!host_is_spi(card)) {
        err = sdmmc_wait_for_idle(card, 0); // wait for the card to be idle (in transfer state)
        if (err != ESP_OK) {
            free(buf);
        }
        TEST_ESP_OK(err);
    }

    // Try to write to the card
    err = sdmmc_write_sectors(card, buf, 0, block_count);
    free(buf);
    TEST_ESP_OK(err);

    // Check if the number of written blocks is equal to the number ACMD22 returns
    // ACMD22 is usually only called if CMD25 fails but here we call it anyway to test it
    size_t sucessfully_written_blocks;
    err = sdmmc_send_cmd_num_of_written_blocks(card, &sucessfully_written_blocks);
    TEST_ESP_OK(err);
    TEST_ASSERT_EQUAL_size_t(sucessfully_written_blocks, block_count);
    ESP_LOGI(TAG, "%s: ACMD22 successfully written %zu blocks out of %zu", __func__, sucessfully_written_blocks, block_count);
}

static void do_one_mmc_acmd22_test(int slot, int width, int freq_khz, int ddr)
{
    sdmmc_card_t card;
    sdmmc_test_sd_skip_if_board_incompatible(slot, width, freq_khz, ddr, NO_EMMC);
    sdmmc_test_sd_begin(slot, width, freq_khz, ddr, &card);
    sdmmc_card_print_info(stdout, &card);
    sdmmc_write_sectors_cmd25_error_test_acmd22(&card, 4096 * 4);
    sdmmc_test_sd_end(&card);
}

TEST_CASE("send ACMD22 after writing multiple blocks to check real number of successfully written blocks, slot 0, 1-bit", "[sdmmc]")
{
    do_one_mmc_acmd22_test(SLOT_0, 1, SDMMC_FREQ_HIGHSPEED, NO_DDR);
}

TEST_CASE("send ACMD22 after writing multiple blocks to check real number of successfully written blocks, slot 1, 1-bit", "[sdmmc]")
{
    do_one_mmc_acmd22_test(SLOT_1, 1, SDMMC_FREQ_HIGHSPEED, NO_DDR);
}
