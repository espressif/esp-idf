/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include "esp_dma_utils.h"
#include "esp_heap_caps.h"
#include "test_utils.h"
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "unity.h"
#include "sd_protocol_defs.h"
#include "sdmmc_cmd.h"
#include "sdmmc_test_rw_common.h"

static void do_single_rw_perf_test(sdmmc_card_t* card, size_t start_block,
                                   size_t block_count, size_t alignment, FILE* performance_log,
                                   uint32_t extra_alloc_caps);

static void fill_buffer(uint32_t seed, uint8_t* dst, size_t count)
{
    srand(seed);
    for (size_t i = 0; i < count; ++i) {
        uint32_t val = rand();
        memcpy(dst + i * sizeof(uint32_t), &val, sizeof(val));
    }
}

// Check if the buffer pointed to by 'dst' contains 'count' 32-bit
// ints generated from 'rand' with the starting value of 'seed'
static void check_buffer(uint32_t seed, const uint8_t* src, size_t count)
{
    srand(seed);
    for (size_t i = 0; i < count; ++i) {
        uint32_t val;
        memcpy(&val, src + i * sizeof(uint32_t), sizeof(val));
        TEST_ASSERT_EQUAL_HEX32(rand(), val);
    }
}

static void do_single_rw_perf_test(sdmmc_card_t* card, size_t start_block,
                                   size_t block_count, size_t alignment, FILE* performance_log,
                                   uint32_t extra_alloc_caps)
{
    size_t block_size = card->csd.sector_size;
    size_t total_size = block_size * block_count;
    const char* alloc_str = (extra_alloc_caps & MALLOC_CAP_SPIRAM) ? "spiram" : " sram ";
    printf(" %8d |  %3d  |   %d   | %s |    %4.1f  ", start_block, block_count, alignment, alloc_str, total_size / 1024.0f);

    size_t actual_size = 0;
    uint32_t *buffer = NULL;
    esp_dma_mem_info_t dma_mem_info = {
        .extra_heap_caps = extra_alloc_caps,
        .dma_alignment_bytes = 64,
    };
    TEST_ESP_OK(esp_dma_capable_malloc(total_size + 4, &dma_mem_info, (void**) &buffer, &actual_size));

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

    if (performance_log) {
        FILE* old_stdout = stdout;
        stdout = performance_log;
        static const char wr_speed_str[] = "SDMMC_WR_SPEED";
        static const char rd_speed_str[] = "SDMMC_RD_SPEED";
        int aligned = ((alignment % 4) == 0) ? 1 : 0;
        IDF_LOG_PERFORMANCE(wr_speed_str, "%d, blk_n: %d, aligned: %d, alloc: %s",
                            (int)(total_size * 1000 / time_wr), block_count, aligned, alloc_str);
        IDF_LOG_PERFORMANCE(rd_speed_str, "%d, blk_n: %d, aligned: %d, alloc: %s",
                            (int)(total_size * 1000 / time_rd), block_count, aligned, alloc_str);
        stdout = old_stdout;
    }
}

void sdmmc_test_rw_unaligned_buffer(sdmmc_card_t* card)
{
    const size_t buffer_size = 4096;
    const size_t block_count = buffer_size / 512;
    const size_t extra = 4;
    const size_t total_size = buffer_size + extra;
    size_t actual_size = 0;
    uint8_t *buffer = NULL;
    esp_dma_mem_info_t dma_mem_info = {
        .dma_alignment_bytes = 64,
    };
    TEST_ESP_OK(esp_dma_capable_malloc(total_size + 4, &dma_mem_info, (void**) &buffer, &actual_size));

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
}

void sdmmc_test_rw_performance(sdmmc_card_t *card, FILE *perf_log)
{
    sdmmc_card_print_info(stdout, card);
    printf("  sector  | count | align | alloc  | size(kB)  | wr_time(ms) | wr_speed(MB/s)  |  rd_time(ms)  | rd_speed(MB/s)\n");
    const int offset = 0;
    /* aligned */
    do_single_rw_perf_test(card, offset, 1, 4, perf_log, 0);
    do_single_rw_perf_test(card, offset, 4, 4, perf_log, 0);
    do_single_rw_perf_test(card, offset, 8, 4, perf_log, 0);
    do_single_rw_perf_test(card, offset, 16, 4, perf_log, 0);
    do_single_rw_perf_test(card, offset, 32, 4, perf_log, 0);
    do_single_rw_perf_test(card, offset, 64, 4, perf_log, 0);
    do_single_rw_perf_test(card, offset, 128, 4, perf_log, 0);
    /* unaligned */
    do_single_rw_perf_test(card, offset, 1, 1, perf_log, 0);
    do_single_rw_perf_test(card, offset, 8, 1, perf_log, 0);
    do_single_rw_perf_test(card, offset, 128, 1, perf_log, 0);
#if CONFIG_SPIRAM
    /* spiram */
    do_single_rw_perf_test(card, offset, 1, 4, perf_log, MALLOC_CAP_SPIRAM);
    do_single_rw_perf_test(card, offset, 4, 4, perf_log, MALLOC_CAP_SPIRAM);
    do_single_rw_perf_test(card, offset, 8, 4, perf_log, MALLOC_CAP_SPIRAM);
    do_single_rw_perf_test(card, offset, 16, 4, perf_log, MALLOC_CAP_SPIRAM);
    do_single_rw_perf_test(card, offset, 32, 4, perf_log, MALLOC_CAP_SPIRAM);
    do_single_rw_perf_test(card, offset, 64, 4, perf_log, MALLOC_CAP_SPIRAM);
    do_single_rw_perf_test(card, offset, 128, 4, perf_log, MALLOC_CAP_SPIRAM);
#endif
}

void sdmmc_test_rw_with_offset(sdmmc_card_t* card)
{
    sdmmc_card_print_info(stdout, card);
    printf("  sector  | count | align | size(kB)  | wr_time(ms) | wr_speed(MB/s)  |  rd_time(ms)  | rd_speed(MB/s)\n");
    /* aligned */
    do_single_rw_perf_test(card, 1, 16, 4, NULL, 0);
    do_single_rw_perf_test(card, 16, 32, 4, NULL, 0);
    do_single_rw_perf_test(card, 48, 64, 4, NULL, 0);
    do_single_rw_perf_test(card, 128, 128, 4, NULL, 0);
    do_single_rw_perf_test(card, card->csd.capacity - 64, 32, 4, NULL, 0);
    do_single_rw_perf_test(card, card->csd.capacity - 64, 64, 4, NULL, 0);
    do_single_rw_perf_test(card, card->csd.capacity - 8, 1, 4, NULL, 0);
    do_single_rw_perf_test(card, card->csd.capacity / 2, 1, 4, NULL, 0);
    do_single_rw_perf_test(card, card->csd.capacity / 2, 4, 4, NULL, 0);
    do_single_rw_perf_test(card, card->csd.capacity / 2, 8, 4, NULL, 0);
    do_single_rw_perf_test(card, card->csd.capacity / 2, 16, 4, NULL, 0);
    do_single_rw_perf_test(card, card->csd.capacity / 2, 32, 4, NULL, 0);
    do_single_rw_perf_test(card, card->csd.capacity / 2, 64, 4, NULL, 0);
    do_single_rw_perf_test(card, card->csd.capacity / 2, 128, 4, NULL, 0);
    /* unaligned */
    do_single_rw_perf_test(card, card->csd.capacity / 2, 1, 1, NULL, 0);
    do_single_rw_perf_test(card, card->csd.capacity / 2, 8, 1, NULL, 0);
    do_single_rw_perf_test(card, card->csd.capacity / 2, 128, 1, NULL, 0);
}
