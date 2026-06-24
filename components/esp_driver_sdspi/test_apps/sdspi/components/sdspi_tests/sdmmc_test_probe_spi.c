/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>
#include <string.h>
#include "unity.h"
#include "sdmmc_cmd.h"
#include "sdmmc_test_begin_end_spi.h"
#include "esp_heap_caps.h"

#define SDMMC_FREQ_CUSTOM_10M    10000

static void do_one_sdspi_probe(int slot, int freq_khz)
{
    sdmmc_card_t card;
    sdmmc_test_spi_skip_if_board_incompatible(slot, freq_khz);
    sdmmc_test_spi_begin(slot, freq_khz, &card, NULL, NULL, NULL);
    sdmmc_card_print_info(stdout, &card);
    uint8_t* buffer = heap_caps_calloc(512, 1, MALLOC_CAP_DMA);
    TEST_ESP_OK(sdmmc_read_sectors(&card, buffer, 0, 1));
    free(buffer);
    sdmmc_test_spi_end(slot, &card);
}

TEST_CASE("sdspi probe, slot 0", "[sdspi]")
{
    do_one_sdspi_probe(SLOT_0, SDMMC_FREQ_PROBING);
    do_one_sdspi_probe(SLOT_0, SDMMC_FREQ_DEFAULT);
}

TEST_CASE("sdspi probe, slot 0, HS", "[sdspi]")
{
    do_one_sdspi_probe(SLOT_0, SDMMC_FREQ_HIGHSPEED);
}

TEST_CASE("sdspi probe, slot 1", "[sdspi]")
{
    do_one_sdspi_probe(SLOT_1, SDMMC_FREQ_PROBING);
    do_one_sdspi_probe(SLOT_1, SDMMC_FREQ_DEFAULT);
    do_one_sdspi_probe(SLOT_1, SDMMC_FREQ_CUSTOM_10M);
}

TEST_CASE("sdspi probe, slot 1, HS", "[sdspi]")
{
    //TODO: IDF-8749
    //here freq should be changed to SDMMC_FREQ_HIGHSPEED after fixing IDF-8749
    do_one_sdspi_probe(SLOT_1, SDMMC_FREQ_DEFAULT);
}

static void do_one_sdspi_probe_ignore_crc(int slot, int freq_khz)
{
    sdmmc_card_t card;
    sdmmc_test_spi_skip_if_board_incompatible(slot, freq_khz);
    sdmmc_host_t config = SDSPI_HOST_DEFAULT();
    config.flags |= SDMMC_HOST_FLAG_SPI_IGNORE_DATA_CRC;
    sdmmc_test_spi_begin(slot, freq_khz, &card, &config, NULL, NULL);
    sdmmc_card_print_info(stdout, &card);
    size_t sector_size = card.csd.sector_size;
    uint8_t* buffer = heap_caps_calloc(sector_size, 1, MALLOC_CAP_DMA);
    TEST_ESP_OK(sdmmc_read_sectors(&card, buffer, 0, 1));
    uint8_t* w_buffer = heap_caps_calloc(sector_size, 1, MALLOC_CAP_DMA);
    memset(w_buffer, 0xAB, sector_size);
    int k = 10; // write every k sectors
    for (int i = 0; i < 30; i += sector_size * k) {
        memset(buffer, 0, sector_size);
        TEST_ESP_OK(sdmmc_write_sectors(&card, w_buffer, i, 1));
        TEST_ESP_OK(sdmmc_read_sectors(&card, buffer, i, 1));
        TEST_ASSERT_TRUE(memcmp(buffer, w_buffer, sector_size) == 0);
    }
    free(w_buffer);
    free(buffer);
    sdmmc_test_spi_end(slot, &card);
}

TEST_CASE("sdspi probe, slot 0, disabled crc check", "[sdspi]")
{
    do_one_sdspi_probe_ignore_crc(SLOT_0, SDMMC_FREQ_DEFAULT);
}

TEST_CASE("sdspi probe, slot 1, disabled crc check", "[sdspi]")
{
    do_one_sdspi_probe_ignore_crc(SLOT_1, SDMMC_FREQ_DEFAULT);
}
