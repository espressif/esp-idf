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
#include "driver/sdspi_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include <time.h>
#include <sys/time.h>


TEST_CASE("can probe SD", "[sd][ignore]")
{
    sdmmc_host_t config = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    TEST_ESP_OK(sdmmc_host_init());
    TEST_ESP_OK(sdmmc_host_init_slot(SDMMC_HOST_SLOT_1, &slot_config));
    sdmmc_card_t* card = malloc(sizeof(sdmmc_card_t));
    TEST_ASSERT_NOT_NULL(card);
    TEST_ESP_OK(sdmmc_card_init(&config, card));
    sdmmc_card_print_info(stdout, card);
    TEST_ESP_OK(sdmmc_host_deinit());
    free(card);
}

TEST_CASE("can probe SD (using SPI)", "[sdspi][ignore]")
{
    sdmmc_host_t config = SDSPI_HOST_DEFAULT();
    sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();
    TEST_ESP_OK(sdspi_host_init());
    TEST_ESP_OK(sdspi_host_init_slot(config.slot, &slot_config));
    sdmmc_card_t* card = malloc(sizeof(sdmmc_card_t));
    TEST_ASSERT_NOT_NULL(card);
    TEST_ESP_OK(sdmmc_card_init(&config, card));
    sdmmc_card_print_info(stdout, card);
    TEST_ESP_OK(sdspi_host_deinit());
    free(card);
}

// Fill buffer pointed to by 'dst' with 'count' 32-bit ints generated
// from 'rand' with the starting value of 'seed'
static void fill_buffer(uint32_t seed, uint8_t* dst, size_t count) {
    srand(seed);
    for (size_t i = 0; i < count; ++i) {
        uint32_t val = rand();
        memcpy(dst + i * sizeof(uint32_t), &val, sizeof(val));
    }
}

// Check if the buffer pointed to by 'dst' contains 'count' 32-bit
// ints generated from 'rand' with the starting value of 'seed'
static void check_buffer(uint32_t seed, const uint8_t* src, size_t count) {
    srand(seed);
    for (size_t i = 0; i < count; ++i) {
        uint32_t val;
        memcpy(&val, src + i * sizeof(uint32_t), sizeof(val));
        TEST_ASSERT_EQUAL_HEX32(rand(), val);
    }
}

static void do_single_write_read_test(sdmmc_card_t* card,
        size_t start_block, size_t block_count, size_t alignment)
{
    size_t block_size = card->csd.sector_size;
    size_t total_size = block_size * block_count;
    printf(" %8d |  %3d  |   %d   |    %4.1f  ", start_block, block_count, alignment, total_size / 1024.0f);

    uint32_t* buffer = heap_caps_malloc(total_size + 4, MALLOC_CAP_DMA);
    size_t offset = alignment % 4;
    uint8_t* c_buffer = (uint8_t*) buffer + offset;
    fill_buffer(start_block, c_buffer, total_size / sizeof(buffer[0]));

    struct timeval t_start_wr;
    gettimeofday(&t_start_wr, NULL);
    TEST_ESP_OK(sdmmc_write_sectors(card, c_buffer, start_block, block_count));
    struct timeval t_stop_wr;
    gettimeofday(&t_stop_wr, NULL);
    float time_wr = 1e3f * (t_stop_wr.tv_sec - t_start_wr.tv_sec) + 1e-3f * (t_stop_wr.tv_usec - t_start_wr.tv_usec);

    memset(buffer, 0xbb, total_size + 4);

    struct timeval t_start_rd;
    gettimeofday(&t_start_rd, NULL);
    TEST_ESP_OK(sdmmc_read_sectors(card, c_buffer, start_block, block_count));
    struct timeval t_stop_rd;
    gettimeofday(&t_stop_rd, NULL);
    float time_rd = 1e3f * (t_stop_rd.tv_sec - t_start_rd.tv_sec) + 1e-3f * (t_stop_rd.tv_usec - t_start_rd.tv_usec);

    printf(" |   %6.2f    |      %5.2f      |    %6.2f     |     %5.2f\n",
            time_wr, total_size / (time_wr / 1000) / (1024 * 1024),
            time_rd, total_size / (time_rd / 1000) / (1024 * 1024));
    check_buffer(start_block, c_buffer, total_size / sizeof(buffer[0]));
    free(buffer);
}

static void read_write_test(sdmmc_card_t* card)
{
    sdmmc_card_print_info(stdout, card);
    printf("  sector  | count | align | size(kB)  | wr_time(ms) | wr_speed(MB/s)  |  rd_time(ms)  | rd_speed(MB/s)\n");
    do_single_write_read_test(card, 0, 1, 4);
    do_single_write_read_test(card, 0, 4, 4);
    do_single_write_read_test(card, 1, 16, 4);
    do_single_write_read_test(card, 16, 32, 4);
    do_single_write_read_test(card, 48, 64, 4);
    do_single_write_read_test(card, 128, 128, 4);
    do_single_write_read_test(card, card->csd.capacity - 64, 32, 4);
    do_single_write_read_test(card, card->csd.capacity - 64, 64, 4);
    do_single_write_read_test(card, card->csd.capacity - 8, 1, 4);
    do_single_write_read_test(card, card->csd.capacity/2, 1, 4);
    do_single_write_read_test(card, card->csd.capacity/2, 4, 4);
    do_single_write_read_test(card, card->csd.capacity/2, 8, 4);
    do_single_write_read_test(card, card->csd.capacity/2, 16, 4);
    do_single_write_read_test(card, card->csd.capacity/2, 32, 4);
    do_single_write_read_test(card, card->csd.capacity/2, 64, 4);
    do_single_write_read_test(card, card->csd.capacity/2, 128, 4);
    do_single_write_read_test(card, card->csd.capacity/2, 1, 1);
    do_single_write_read_test(card, card->csd.capacity/2, 8, 1);
    do_single_write_read_test(card, card->csd.capacity/2, 128, 1);
}

TEST_CASE("can write and read back blocks", "[sd][ignore]")
{
    sdmmc_host_t config = SDMMC_HOST_DEFAULT();
    config.max_freq_khz = SDMMC_FREQ_HIGHSPEED;
    TEST_ESP_OK(sdmmc_host_init());
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    TEST_ESP_OK(sdmmc_host_init_slot(SDMMC_HOST_SLOT_1, &slot_config));
    sdmmc_card_t* card = malloc(sizeof(sdmmc_card_t));
    TEST_ASSERT_NOT_NULL(card);
    TEST_ESP_OK(sdmmc_card_init(&config, card));
    read_write_test(card);
    free(card);
    TEST_ESP_OK(sdmmc_host_deinit());
}

TEST_CASE("can write and read back blocks (using SPI)", "[sdspi][ignore]")
{
    sdmmc_host_t config = SDSPI_HOST_DEFAULT();
    config.max_freq_khz = SDMMC_FREQ_HIGHSPEED;
    sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();
    TEST_ESP_OK(sdspi_host_init());
    TEST_ESP_OK(sdspi_host_init_slot(config.slot, &slot_config));
    sdmmc_card_t* card = malloc(sizeof(sdmmc_card_t));
    TEST_ASSERT_NOT_NULL(card);
    TEST_ESP_OK(sdmmc_card_init(&config, card));
    read_write_test(card);
    free(card);
    TEST_ESP_OK(sdspi_host_deinit());
}

TEST_CASE("reads and writes with an unaligned buffer", "[sd][ignore]")
{
    sdmmc_host_t config = SDMMC_HOST_DEFAULT();
    TEST_ESP_OK(sdmmc_host_init());
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    TEST_ESP_OK(sdmmc_host_init_slot(SDMMC_HOST_SLOT_1, &slot_config));
    sdmmc_card_t* card = malloc(sizeof(sdmmc_card_t));
    TEST_ASSERT_NOT_NULL(card);
    TEST_ESP_OK(sdmmc_card_init(&config, card));

    const size_t buffer_size = 4096;
    const size_t block_count = buffer_size / 512;
    const size_t extra = 4;
    uint8_t* buffer = heap_caps_malloc(buffer_size + extra, MALLOC_CAP_DMA);

    // Check read behavior: do aligned write, then unaligned read
    const uint32_t seed = 0x89abcdef;
    fill_buffer(seed, buffer, buffer_size / sizeof(uint32_t));
    TEST_ESP_OK(sdmmc_write_sectors(card, buffer, 0, block_count));
    memset(buffer, 0xcc, buffer_size + extra);
    TEST_ESP_OK(sdmmc_read_sectors(card, buffer + 1, 0, block_count));
    check_buffer(seed, buffer + 1, buffer_size / sizeof(uint32_t));

    // Check write behavior: do unaligned write, then aligned read
    fill_buffer(seed, buffer + 1, buffer_size / sizeof(uint32_t));
    TEST_ESP_OK(sdmmc_write_sectors(card, buffer + 1, 8, block_count));
    memset(buffer, 0xcc, buffer_size + extra);
    TEST_ESP_OK(sdmmc_read_sectors(card, buffer, 8, block_count));
    check_buffer(seed, buffer, buffer_size / sizeof(uint32_t));

    free(buffer);
    free(card);
    TEST_ESP_OK(sdmmc_host_deinit());
}
