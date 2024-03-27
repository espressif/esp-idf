/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "unity.h"
#include "sdmmc_cmd.h"

#define PATTERN_SEED    0x12345678
#define FLAG_ERASE_TEST_ADJACENT    (1 << 0)
#define FLAG_VERIFY_ERASE_STATE     (1 << 1)

#ifdef CONFIG_SOC_SDMMC_HOST_SUPPORTED
extern bool get_sanitize_flag(void);
#endif

static void check_buffer(uint32_t seed, const uint8_t* src, size_t count)
{
    srand(seed);
    for (size_t i = 0; i < count; ++i) {
        uint32_t val;
        memcpy(&val, src + i * sizeof(uint32_t), sizeof(val));
        TEST_ASSERT_EQUAL_HEX32(rand(), val);
    }
}

static void fill_buffer(uint32_t seed, uint8_t* dst, size_t count)
{
    srand(seed);
    for (size_t i = 0; i < count; ++i) {
        uint32_t val = rand();
        memcpy(dst + i * sizeof(uint32_t), &val, sizeof(val));
    }
}

static void ensure_sector_written(sdmmc_card_t* card, size_t sector,
                                  uint8_t *pattern_buf, uint8_t *temp_buf)
{
    size_t block_size = card->csd.sector_size;
    TEST_ESP_OK(sdmmc_write_sectors(card, pattern_buf, sector, 1));
    memset((void *)temp_buf, 0x00, block_size);
    TEST_ESP_OK(sdmmc_read_sectors(card, temp_buf, sector, 1));
    check_buffer(PATTERN_SEED, temp_buf, block_size / sizeof(uint32_t));
}

static void ensure_sector_intact(sdmmc_card_t* card, size_t sector,
                                 uint8_t *pattern_buf, uint8_t *temp_buf)
{
    size_t block_size = card->csd.sector_size;
    memset((void *)temp_buf, 0x00, block_size);
    TEST_ESP_OK(sdmmc_read_sectors(card, temp_buf, sector, 1));
    check_buffer(PATTERN_SEED, temp_buf, block_size / sizeof(uint32_t));
}

static int32_t ensure_sector_erase(sdmmc_card_t* card, size_t sector,
                                   uint8_t *pattern_buf, uint8_t *temp_buf)
{
    size_t block_size = card->csd.sector_size;
    memset((void *)temp_buf, 0, block_size);
    TEST_ESP_OK(sdmmc_read_sectors(card, temp_buf, sector, 1));
    return memcmp(pattern_buf, temp_buf, block_size);
}

static void do_single_erase_test(sdmmc_card_t* card, size_t start_block,
                                 size_t block_count, uint8_t flags, sdmmc_erase_arg_t arg)
{
    size_t block_size = card->csd.sector_size;
    uint8_t *temp_buf = NULL;
    uint8_t *pattern_buf = NULL;
    size_t end_block = (start_block + block_count - 1);

    /*
     * To ensure erase is successful/valid
     * selected blocks after erase should have erase state data pattern
     * data of blocks adjacent to selected region should remain intact
     */
    TEST_ESP_OK((start_block + block_count) > card->csd.capacity);

    pattern_buf = (uint8_t *)heap_caps_malloc(block_size, MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(pattern_buf);
    temp_buf = (uint8_t *)heap_caps_malloc(block_size, MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(temp_buf);

    // create pattern buffer
    fill_buffer(PATTERN_SEED, pattern_buf, block_size / sizeof(uint32_t));

    // check if it's not the first block of device & write/read/verify pattern
    if ((flags & FLAG_ERASE_TEST_ADJACENT) && start_block) {
        ensure_sector_written(card, (start_block - 1), pattern_buf, temp_buf);
    }

    ensure_sector_written(card, start_block, pattern_buf, temp_buf);

    // check if it's not the last block of device & write/read/verify pattern
    if ((flags & FLAG_ERASE_TEST_ADJACENT) && (end_block < (card->csd.capacity - 1))) {
        ensure_sector_written(card, (end_block + 1), pattern_buf, temp_buf);
    }

    // when block count is 1, start and end block is same, hence skip
    if (block_count != 1) {
        ensure_sector_written(card, end_block, pattern_buf, temp_buf);
    }

    // fill pattern to (start_block + end_block)/2 in the erase range
    if (block_count > 2) {
        ensure_sector_written(card, (start_block + end_block) / 2, pattern_buf, temp_buf);
    }

    float total_size = (block_count / 1024.0f) * block_size;
    printf(" %10d |  %10d   |  %8.1f    ", start_block, block_count, total_size);
    fflush(stdout);

    // erase the blocks
    struct timeval t_start_er;
    gettimeofday(&t_start_er, NULL);
    TEST_ESP_OK(sdmmc_erase_sectors(card, start_block, block_count, arg));
#ifdef CONFIG_SOC_SDMMC_HOST_SUPPORTED
    if (get_sanitize_flag()) {
        TEST_ESP_OK(sdmmc_mmc_sanitize(card, block_count * 500));
    }
#endif
    struct timeval t_stop_wr;
    gettimeofday(&t_stop_wr, NULL);
    float time_er = 1e3f * (t_stop_wr.tv_sec - t_start_er.tv_sec) + 1e-3f * (t_stop_wr.tv_usec - t_start_er.tv_usec);
    printf(" |   %8.2f\n", time_er);

    // ensure adjacent blocks are not affected
    // block before start_block
    if ((flags & FLAG_ERASE_TEST_ADJACENT) && start_block) {
        ensure_sector_intact(card, (start_block - 1), pattern_buf, temp_buf);
    }

    // block after end_block
    if ((flags & FLAG_ERASE_TEST_ADJACENT) && (end_block < (card->csd.capacity - 1))) {
        ensure_sector_intact(card, (end_block + 1), pattern_buf, temp_buf);
    }

    uint8_t erase_mem_byte = 0xFF;
    // ensure all the blocks are erased and are up to after erase state.
    if (!card->is_mmc) {
        erase_mem_byte = card->scr.erase_mem_state ? 0xFF : 0x00;
    } else {
        erase_mem_byte = card->ext_csd.erase_mem_state ? 0xFF : 0x00;
    }

    memset((void *)pattern_buf, erase_mem_byte, block_size);

    // as it is block by block comparison, a time taking process. Really long
    // when you do erase and verify on complete device.
    if (flags & FLAG_VERIFY_ERASE_STATE) {
        for (size_t i  = 0; i < block_count; i++) {
            if (ensure_sector_erase(card, (start_block + i), pattern_buf, temp_buf)) {
                printf("Error: Sector %d erase\n", (start_block + i));
                break;
            }
        }
    }

    free(temp_buf);
    free(pattern_buf);
}

void sdmmc_test_sd_erase_blocks(sdmmc_card_t* card)
{
    printf("block size %d capacity %d\n", card->csd.sector_size, card->csd.capacity);
    printf("  sector    |    count      |   size(kB)    |   er_time(ms) \n");
    /*
     * bit-0: verify adjacent blocks of given range
     * bit-1: verify erase state of blocks in range
     */
    uint8_t flags = 0;
    sdmmc_erase_arg_t arg = SDMMC_ERASE_ARG;

    //check for adjacent blocks and erase state of blocks
    flags |= (uint8_t)FLAG_ERASE_TEST_ADJACENT | (uint8_t)FLAG_VERIFY_ERASE_STATE;
    do_single_erase_test(card, 1, 16, flags, arg);
    do_single_erase_test(card, 1, 13, flags, arg);
    do_single_erase_test(card, 16, 32, flags, arg);
    do_single_erase_test(card, 48, 64, flags, arg);
    do_single_erase_test(card, 128, 128, flags, arg);
    do_single_erase_test(card, card->csd.capacity - 64, 32, flags, arg);
    do_single_erase_test(card, card->csd.capacity - 64, 64, flags, arg);
    // single sector erase is failing on different make cards
    do_single_erase_test(card, card->csd.capacity - 8, 1, flags, arg);
    do_single_erase_test(card, card->csd.capacity / 2, 1, flags, arg);
    do_single_erase_test(card, card->csd.capacity / 2, 4, flags, arg);
    do_single_erase_test(card, card->csd.capacity / 2, 8, flags, arg);
    do_single_erase_test(card, card->csd.capacity / 2, 16, flags, arg);
    do_single_erase_test(card, card->csd.capacity / 2, 32, flags, arg);
    do_single_erase_test(card, card->csd.capacity / 2, 64, flags, arg);
    do_single_erase_test(card, card->csd.capacity / 2, 128, flags, arg);
#ifdef SDMMC_FULL_ERASE_TEST
    /*
     * check for adjacent blocks, do not check erase state of blocks as it is
     * time taking process to verify all the blocks.
     */
    flags &= ~(uint8_t)FLAG_VERIFY_ERASE_STATE; //comment this line to verify after-erase state
    // erase complete card
    do_single_erase_test(card, 0, card->csd.capacity, flags, arg);
#endif //SDMMC_FULL_ERASE_TEST
}
