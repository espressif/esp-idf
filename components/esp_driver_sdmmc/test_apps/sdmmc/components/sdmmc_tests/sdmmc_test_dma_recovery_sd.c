/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include <stdlib.h>
#include <string.h>
#include "unity.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
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

static const char* TAG = "sdmmc_dma_recovery";

#define TEST_XFER_SIZE   (16 * 1024)
#define TEST_ITERATIONS  3

static void fill_pattern(uint32_t *buf, size_t size, uint32_t seed)
{
    for (size_t i = 0; i < size / sizeof(uint32_t); i++) {
        buf[i] = seed ^ (i * 0x9E3779B1u);
    }
}

/* Send a data command while the card is deselected (stand-by state).
 * The card ignores data commands in this state, so the host gets a response timeout (RTO)
 * after the IDMAC has already been armed (and for writes, has already pre-filled the TX FIFO). */
static esp_err_t send_data_cmd_expect_rto(sdmmc_card_t *card, bool is_write, void *buf, size_t sector)
{
    size_t block_size = card->csd.sector_size;
    sdmmc_command_t cmd = {
        .opcode = is_write ? MMC_WRITE_BLOCK_MULTIPLE : MMC_READ_BLOCK_MULTIPLE,
        .flags = SCF_CMD_ADTC | SCF_RSP_R1 | (is_write ? 0 : SCF_CMD_READ),
        .arg = card->ocr & SD_OCR_SDHC_CAP ? sector : sector * block_size,
        .data = buf,
        .datalen = TEST_XFER_SIZE,
        .buflen = TEST_XFER_SIZE,
        .blklen = block_size,
        .timeout_ms = 1000,
    };
    return sdmmc_send_cmd(card, &cmd);
}

static void do_rto_recovery_test(int slot, int width, bool is_write)
{
    sdmmc_card_t card;
    sdmmc_test_sd_skip_if_board_incompatible(slot, width, SDMMC_FREQ_HIGHSPEED, NO_DDR, NO_EMMC);
    sdmmc_test_sd_begin(slot, width, SDMMC_FREQ_HIGHSPEED, NO_DDR, &card);
    sdmmc_card_print_info(stdout, &card);

    const size_t block_size = card.csd.sector_size;
    const size_t block_count = TEST_XFER_SIZE / block_size;
    const size_t sector = card.csd.capacity / 2;

    uint32_t *good = heap_caps_malloc(TEST_XFER_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    uint32_t *bad = heap_caps_malloc(TEST_XFER_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    uint32_t *rd = heap_caps_malloc(TEST_XFER_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(good);
    TEST_ASSERT_NOT_NULL(bad);
    TEST_ASSERT_NOT_NULL(rd);

    for (int it = 0; it < TEST_ITERATIONS; it++) {
        ESP_LOGI(TAG, "===== iteration %d (%s) =====", it, is_write ? "CMD25" : "CMD18");
        fill_pattern(good, TEST_XFER_SIZE, 0x11110000u + it);
        fill_pattern(bad, TEST_XFER_SIZE, 0xBAD00000u + it);

        // Baseline: write a known pattern
        TEST_ESP_OK(sdmmc_write_sectors(&card, good, sector, block_count));

        // Deselect the card and trigger an RTO on a data command
        TEST_ESP_OK(sdmmc_send_cmd_select_card(&card, 0));
        esp_err_t err = send_data_cmd_expect_rto(&card, is_write, bad, sector);
        ESP_LOGI(TAG, "data cmd on deselected card returned 0x%x", err);
        TEST_ASSERT_EQUAL_HEX32(ESP_ERR_TIMEOUT, err);

        // Reselect and check that the next transfers work and carry the right data
        err = sdmmc_send_cmd_select_card(&card, card.rca);
        ESP_LOGI(TAG, "reselect returned 0x%x", err);
        TEST_ESP_OK(err);

        memset(rd, 0, TEST_XFER_SIZE);
        err = sdmmc_read_sectors(&card, rd, sector, block_count);
        ESP_LOGI(TAG, "read after RTO returned 0x%x", err);
        TEST_ESP_OK(err);
        TEST_ASSERT_EQUAL_HEX32_ARRAY(good, rd, TEST_XFER_SIZE / sizeof(uint32_t));

        fill_pattern(good, TEST_XFER_SIZE, 0x22220000u + it);
        err = sdmmc_write_sectors(&card, good, sector, block_count);
        ESP_LOGI(TAG, "write after RTO returned 0x%x", err);
        TEST_ESP_OK(err);

        memset(rd, 0, TEST_XFER_SIZE);
        TEST_ESP_OK(sdmmc_read_sectors(&card, rd, sector, block_count));
        TEST_ASSERT_EQUAL_HEX32_ARRAY(good, rd, TEST_XFER_SIZE / sizeof(uint32_t));
    }

    free(good);
    free(bad);
    free(rd);
    sdmmc_test_sd_end(&card);
}

#define TEST_LONG_XFER_SIZE  (64 * 1024)

/* Abort a long multi-block transfer while data is on the bus: the transaction gives up after 1 ms
 * (event queue timeout) while the card is still sending / receiving data. Then check that the host
 * can still talk to the card. */
static void do_midtransfer_abort_test(int slot, int width, int freq_khz, bool is_write)
{
    sdmmc_card_t card;
    sdmmc_test_sd_skip_if_board_incompatible(slot, width, freq_khz, NO_DDR, NO_EMMC);
    sdmmc_test_sd_begin(slot, width, freq_khz, NO_DDR, &card);
    sdmmc_card_print_info(stdout, &card);
    TEST_ASSERT_EQUAL(0, card.host.command_timeout_ms);

    const size_t block_size = card.csd.sector_size;
    const size_t sector = card.csd.capacity / 2;

    uint32_t *good = heap_caps_malloc(TEST_XFER_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    uint32_t *rd = heap_caps_malloc(TEST_XFER_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    uint32_t *big = heap_caps_malloc(TEST_LONG_XFER_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(good);
    TEST_ASSERT_NOT_NULL(rd);
    TEST_ASSERT_NOT_NULL(big);

    fill_pattern(good, TEST_XFER_SIZE, 0x33330000u);
    fill_pattern(big, TEST_LONG_XFER_SIZE, 0xB16B0000u);
    TEST_ESP_OK(sdmmc_write_sectors(&card, good, sector, TEST_XFER_SIZE / block_size));

    sdmmc_command_t cmd = {
        .opcode = is_write ? MMC_WRITE_BLOCK_MULTIPLE : MMC_READ_BLOCK_MULTIPLE,
        .flags = SCF_CMD_ADTC | SCF_RSP_R1 | (is_write ? 0 : SCF_CMD_READ),
        .arg = card.ocr & SD_OCR_SDHC_CAP ? sector + 1024 : (sector + 1024) * block_size,
                   .data = big,
                   .datalen = TEST_LONG_XFER_SIZE,
                   .buflen = TEST_LONG_XFER_SIZE,
                   .blklen = block_size,
                   .timeout_ms = 1,
    };
    esp_err_t err = sdmmc_send_cmd(&card, &cmd);
    ESP_LOGI(TAG, "%s with 1 ms timeout returned 0x%x", is_write ? "CMD25" : "CMD18", err);
    vTaskDelay(pdMS_TO_TICKS(200));

    // the driver only resets the controller, bringing the card out of the data state is done by the protocol layer
    esp_err_t stop_err = sdmmc_stop_transmission_if_needed(&card);
    ESP_LOGI(TAG, "stop transmission after abort returned 0x%x", stop_err);

    uint32_t status = 0;
    err = sdmmc_send_cmd_send_status(&card, &status);
    ESP_LOGI(TAG, "CMD13 after abort returned 0x%x, status=0x%08"PRIx32" (state=%d)", err, status, (int)MMC_R1_CURRENT_STATE(&status));

    memset(rd, 0, TEST_XFER_SIZE);
    err = sdmmc_read_sectors(&card, rd, sector, TEST_XFER_SIZE / block_size);
    ESP_LOGI(TAG, "read after abort returned 0x%x", err);
    bool read_ok = (err == ESP_OK) && memcmp(good, rd, TEST_XFER_SIZE) == 0;
    ESP_LOGI(TAG, "read data %s", read_ok ? "matches" : "DOES NOT match");

    fill_pattern(good, TEST_XFER_SIZE, 0x44440000u);
    esp_err_t werr = sdmmc_write_sectors(&card, good, sector, TEST_XFER_SIZE / block_size);
    ESP_LOGI(TAG, "write after abort returned 0x%x", werr);
    memset(rd, 0, TEST_XFER_SIZE);
    esp_err_t rerr = sdmmc_read_sectors(&card, rd, sector, TEST_XFER_SIZE / block_size);
    bool rw_ok = werr == ESP_OK && rerr == ESP_OK && memcmp(good, rd, TEST_XFER_SIZE) == 0;
    ESP_LOGI(TAG, "write+read-back after abort: %s (werr=0x%x rerr=0x%x)", rw_ok ? "OK" : "FAILED", werr, rerr);

    free(good);
    free(rd);
    free(big);
    sdmmc_test_sd_end(&card);
    TEST_ESP_OK(stop_err);
    TEST_ASSERT_TRUE(read_ok);
    TEST_ASSERT_TRUE(rw_ok);
}

TEST_CASE("SDMMC recovers after mid-transfer abort of CMD18 (read), slot 0, 1-bit", "[sdmmc][dma_recovery]")
{
    do_midtransfer_abort_test(SLOT_0, 1, SDMMC_FREQ_DEFAULT, false);
}

TEST_CASE("SDMMC recovers after mid-transfer abort of CMD25 (write), slot 0, 1-bit", "[sdmmc][dma_recovery]")
{
    do_midtransfer_abort_test(SLOT_0, 1, SDMMC_FREQ_DEFAULT, true);
}

TEST_CASE("SDMMC recovers after RTO on CMD25 (write), slot 0, 4-bit", "[sdmmc][dma_recovery]")
{
    do_rto_recovery_test(SLOT_0, 4, true);
}

TEST_CASE("SDMMC recovers after RTO on CMD18 (read), slot 0, 4-bit", "[sdmmc][dma_recovery]")
{
    do_rto_recovery_test(SLOT_0, 4, false);
}
