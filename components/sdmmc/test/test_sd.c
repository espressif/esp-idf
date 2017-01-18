// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unity.h"
#include "driver/gpio.h"
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#include "esp_log.h"
#include "esp_heap_alloc_caps.h"
#include <time.h>
#include <sys/time.h>


TEST_CASE("can probe SD", "[sd][ignore]")
{
    sdmmc_host_t config = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    sdmmc_host_init();
    sdmmc_host_init_slot(SDMMC_HOST_SLOT_1, &slot_config);
    sdmmc_card_t* card = malloc(sizeof(sdmmc_card_t));
    TEST_ASSERT_NOT_NULL(card);
    TEST_ESP_OK(sdmmc_card_init(&config, card));
    sdmmc_card_print_info(stdout, card);
    sdmmc_host_deinit();
    free(card);
}


static void do_single_write_read_test(sdmmc_card_t* card,
        size_t start_block, size_t block_count)
{
    size_t block_size = card->csd.sector_size;
    size_t total_size = block_size * block_count;
    printf(" %8d |  %3d  |   %4.1f  ", start_block, block_count, total_size / 1024.0f);
    uint32_t* buffer = pvPortMallocCaps(total_size, MALLOC_CAP_DMA);
    srand(start_block);
    for (size_t i = 0; i < total_size / sizeof(buffer[0]); ++i) {
        buffer[i] = rand();
    }
    struct timeval t_start_wr;
    gettimeofday(&t_start_wr, NULL);
    TEST_ESP_OK(sdmmc_write_sectors(card, buffer, start_block, block_count));
    struct timeval t_stop_wr;
    gettimeofday(&t_stop_wr, NULL);
    float time_wr = 1e3f * (t_stop_wr.tv_sec - t_start_wr.tv_sec) + 1e-3f * (t_stop_wr.tv_usec - t_start_wr.tv_usec);
    memset(buffer, 0xbb, total_size);
    struct timeval t_start_rd;
    gettimeofday(&t_start_rd, NULL);
    TEST_ESP_OK(sdmmc_read_sectors(card, buffer, start_block, block_count));
    struct timeval t_stop_rd;
    gettimeofday(&t_stop_rd, NULL);
    float time_rd = 1e3f * (t_stop_rd.tv_sec - t_start_rd.tv_sec) + 1e-3f * (t_stop_rd.tv_usec - t_start_rd.tv_usec);

    printf(" |   %6.2f    |      %.2f      |    %.2fs    |     %.2f\n",
            time_wr, total_size / (time_wr / 1000) / (1024 * 1024),
            time_rd, total_size / (time_rd / 1000) / (1024 * 1024));
    srand(start_block);
    for (size_t i = 0; i < total_size / sizeof(buffer[0]); ++i) {
        TEST_ASSERT_EQUAL_HEX32(rand(), buffer[i]);
    }
    free(buffer);
}

TEST_CASE("can write and read back blocks", "[sd][ignore]")
{
    sdmmc_host_t config = SDMMC_HOST_DEFAULT();
    config.max_freq_khz = SDMMC_FREQ_HIGHSPEED;
    sdmmc_host_init();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    sdmmc_host_init_slot(SDMMC_HOST_SLOT_1, &slot_config);
    sdmmc_card_t* card = malloc(sizeof(sdmmc_card_t));
    TEST_ASSERT_NOT_NULL(card);
    TEST_ESP_OK(sdmmc_card_init(&config, card));
    sdmmc_card_print_info(stdout, card);
    printf("  sector  | count | size(kB) | wr_time(ms) | wr_speed(MB/s) | rd_time(ms) | rd_speed(MB/s)\n");
    do_single_write_read_test(card, 0, 1);
    do_single_write_read_test(card, 0, 4);
    do_single_write_read_test(card, 1, 16);
    do_single_write_read_test(card, 16, 32);
    do_single_write_read_test(card, 48, 64);
    do_single_write_read_test(card, 128, 128);
    do_single_write_read_test(card, card->csd.capacity - 64, 32);
    do_single_write_read_test(card, card->csd.capacity - 64, 64);
    do_single_write_read_test(card, card->csd.capacity - 8, 1);
    do_single_write_read_test(card, card->csd.capacity/2, 1);
    do_single_write_read_test(card, card->csd.capacity/2, 4);
    do_single_write_read_test(card, card->csd.capacity/2, 8);
    do_single_write_read_test(card, card->csd.capacity/2, 16);
    do_single_write_read_test(card, card->csd.capacity/2, 32);
    do_single_write_read_test(card, card->csd.capacity/2, 64);
    do_single_write_read_test(card, card->csd.capacity/2, 128);
    free(card);
    sdmmc_host_deinit();
}
