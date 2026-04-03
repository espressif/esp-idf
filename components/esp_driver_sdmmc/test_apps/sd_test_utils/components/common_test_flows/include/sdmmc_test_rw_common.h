/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sd_protocol_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Test read/write performance of the card
 *
 * This function writes a buffer to the card, then reads it back.
 * The time taken for each operation is measured, and the throughput is calculated.
 * The process is repeated for different buffer sizes and card offsets.
 * In this test, data is always written and read at the beginning of the card.
 *
 * This test function works both with SDMMC and SDSPI hosts.
 *
 * @param card Pointer to the card object, must be initialized before calling this function.
 * @param perf_log If not null, additional "performance log" lines are written to this file.
 *                 These lines are in a format recognized by pytest-embedded.
 */
void sdmmc_test_rw_performance(sdmmc_card_t *card, FILE *perf_log);

/**
 * @brief Test read/write with unaligned buffer
 *
 * This function verifies that the driver deals with unaligned source/destination buffers correctly.
 *
 * This test function works both with SDMMC and SDSPI hosts.
 *
 * @param card Pointer to the card object, must be initialized before calling this function.
 */
void sdmmc_test_rw_unaligned_buffer(sdmmc_card_t* card);

/**
 * @brief Test read/write with offset
 *
 * Similar to sdmmc_test_rw_performance, but this time data is written at different
 * offsets on the card.
 *
 * This test function works both with SDMMC and SDSPI hosts.
 *
 * @param card Pointer to the card object, must be initialized before calling this function.
 */
void sdmmc_test_rw_with_offset(sdmmc_card_t* card);

/**
 * @brief Test read/write with higher priority tasks running concurrently
 * @param card Pointer to the card object, must be initialized before calling this function.
 */
void sdmmc_test_rw_highprio_task(sdmmc_card_t* card);

/**
 * @brief Test multi-block read/write with unaligned buffers
 *
 * This function verifies that multi-block chunked transfers work correctly
 * when the source/destination buffers are not DMA-aligned. It exercises:
 * - Multi-block unaligned writes and reads with more blocks than the configured chunk size
 * - The dma_aligned_buffer reuse path (when card->host.dma_aligned_buffer is pre-allocated)
 *
 * This test function works both with SDMMC and SDSPI hosts.
 *
 * @param card Pointer to the card object, must be initialized before calling this function.
 * @param chunk_size Maximum number of blocks to transfer at once when using an
 *                   unaligned bounce buffer. This value is written to
 *                   card->host.unaligned_multi_block_rw_max_chunk_size before the test runs.
 */
void sdmmc_test_rw_unaligned_buffer_multiblock(sdmmc_card_t* card, size_t chunk_size);

#ifdef __cplusplus
};
#endif
