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
/* At least 2 ticks, so that the event queue wait lasts at least 1 ms. Much shorter than the transfer of
 * TEST_LONG_XFER_SIZE at the default frequency in 1-bit mode (about 26 ms), so the transfer is
 * always aborted while data is on the bus. */
#define TEST_ABORT_TIMEOUT_MS  2

#define CARD_STATE_PRG  7

/* Wrapper around the host transaction function, which shortens the timeout of the first command with
 * opcode s_abort_opcode. Commands sent afterwards (CMD13, CMD12, ACMD22) keep their normal timeouts. */
static esp_err_t (*s_orig_do_transaction)(int slot, sdmmc_command_t *cmd);
static uint32_t s_abort_opcode;
static bool s_abort_pending;

static esp_err_t do_transaction_abort_data_cmd(int slot, sdmmc_command_t *cmd)
{
    if (s_abort_pending && cmd->opcode == s_abort_opcode) {
        s_abort_pending = false;
        cmd->timeout_ms = TEST_ABORT_TIMEOUT_MS;
    }
    return s_orig_do_transaction(slot, cmd);
}

static void start_aborting_data_cmd(sdmmc_card_t *card, uint32_t opcode)
{
    s_orig_do_transaction = card->host.do_transaction;
    s_abort_opcode = opcode;
    s_abort_pending = true;
    card->host.do_transaction = do_transaction_abort_data_cmd;
}

static void stop_aborting_data_cmd(sdmmc_card_t *card)
{
    card->host.do_transaction = s_orig_do_transaction;
    TEST_ASSERT_FALSE_MESSAGE(s_abort_pending, "data command was not sent");
}

static uint32_t get_card_state(sdmmc_card_t *card)
{
    uint32_t status = 0;
    TEST_ESP_OK(sdmmc_send_cmd_send_status(card, &status));
    return MMC_R1_CURRENT_STATE_STATUS(status);
}

/* After an aborted write the card can be programming for a while before it returns to tran */
static uint32_t wait_while_programming(sdmmc_card_t *card)
{
    uint32_t state = get_card_state(card);
    for (int i = 0; i < 100 && state == CARD_STATE_PRG; i++) {
        vTaskDelay(pdMS_TO_TICKS(10));
        state = get_card_state(card);
    }
    return state;
}

/* Abort a long multi-block transfer while data is on the bus: the transaction gives up after
 * TEST_ABORT_TIMEOUT_MS (event queue timeout) while the card is still sending / receiving data.
 * If via_rw_api is false, the command is sent with sdmmc_send_cmd() and the test checks that the card
 * is left in the data / rcv state, then calls sdmmc_stop_transmission_if_needed() itself.
 * If via_rw_api is true, the transfer goes through sdmmc_read_sectors_dma() / sdmmc_write_sectors_dma(),
 * which must bring the card back to the tran state on their own.
 * In both cases, check that the following reads and writes work and carry the right data. */
static void do_midtransfer_abort_test(int slot, int width, int freq_khz, bool is_write, bool via_rw_api)
{
#if CONFIG_FREERTOS_HZ != 1000
    // with a longer tick, the event queue wait can last most of the transfer, so the abort may not happen mid-transfer
    TEST_IGNORE_MESSAGE("mid-transfer abort tests require CONFIG_FREERTOS_HZ=1000");
#endif
    sdmmc_card_t card;
    sdmmc_test_sd_skip_if_board_incompatible(slot, width, freq_khz, NO_DDR, NO_EMMC);
    sdmmc_test_sd_begin(slot, width, freq_khz, NO_DDR, &card);
    sdmmc_card_print_info(stdout, &card);
    TEST_ASSERT_EQUAL(0, card.host.command_timeout_ms);

    const size_t block_size = card.csd.sector_size;
    const size_t sector = card.csd.capacity / 2;
    const size_t long_sector = sector + 1024;
    const size_t long_block_count = TEST_LONG_XFER_SIZE / block_size;

    uint32_t *good = heap_caps_malloc(TEST_XFER_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    uint32_t *rd = heap_caps_malloc(TEST_XFER_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    uint32_t *big = heap_caps_malloc(TEST_LONG_XFER_SIZE, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    TEST_ASSERT_NOT_NULL(good);
    TEST_ASSERT_NOT_NULL(rd);
    TEST_ASSERT_NOT_NULL(big);

    fill_pattern(good, TEST_XFER_SIZE, 0x33330000u);
    fill_pattern(big, TEST_LONG_XFER_SIZE, 0xB16B0000u);
    TEST_ESP_OK(sdmmc_write_sectors(&card, good, sector, TEST_XFER_SIZE / block_size));

    const char *cmd_name = is_write ? "CMD25" : "CMD18";
    esp_err_t err;
    if (via_rw_api) {
        // only the data command gets the short timeout, the commands sent during recovery keep their normal timeouts
        start_aborting_data_cmd(&card, is_write ? MMC_WRITE_BLOCK_MULTIPLE : MMC_READ_BLOCK_MULTIPLE);
        if (is_write) {
            err = sdmmc_write_sectors_dma(&card, big, long_sector, long_block_count, TEST_LONG_XFER_SIZE);
        } else {
            err = sdmmc_read_sectors_dma(&card, big, long_sector, long_block_count, TEST_LONG_XFER_SIZE);
        }
        stop_aborting_data_cmd(&card);
        ESP_LOGI(TAG, "%s via sdmmc_%s_sectors_dma with %d ms timeout returned 0x%x",
                 cmd_name, is_write ? "write" : "read", TEST_ABORT_TIMEOUT_MS, err);
        TEST_ASSERT_NOT_EQUAL(ESP_OK, err);
    } else {
        sdmmc_command_t cmd = {
            .opcode = is_write ? MMC_WRITE_BLOCK_MULTIPLE : MMC_READ_BLOCK_MULTIPLE,
            .flags = SCF_CMD_ADTC | SCF_RSP_R1 | (is_write ? 0 : SCF_CMD_READ),
            .arg = card.ocr & SD_OCR_SDHC_CAP ? long_sector : long_sector * block_size,
            .data = big,
            .datalen = TEST_LONG_XFER_SIZE,
            .buflen = TEST_LONG_XFER_SIZE,
            .blklen = block_size,
            .timeout_ms = TEST_ABORT_TIMEOUT_MS,
        };
        err = sdmmc_send_cmd(&card, &cmd);
        ESP_LOGI(TAG, "%s with %d ms timeout returned 0x%x", cmd_name, TEST_ABORT_TIMEOUT_MS, err);
        TEST_ASSERT_NOT_EQUAL(ESP_OK, err);

        // the driver only resets the controller, the card must still be in the middle of the transfer
        uint32_t state = get_card_state(&card);
        ESP_LOGI(TAG, "card state after abort: %"PRIu32, state);
        TEST_ASSERT_EQUAL(is_write ? MMC_R1_CURRENT_STATE_RCV : MMC_R1_CURRENT_STATE_DATA, state);

        TEST_ESP_OK(sdmmc_stop_transmission_if_needed(&card));
    }

    uint32_t state = wait_while_programming(&card);
    ESP_LOGI(TAG, "card state after recovery: %"PRIu32, state);
    TEST_ASSERT_EQUAL(MMC_R1_CURRENT_STATE_TRAN, state);

    memset(rd, 0, TEST_XFER_SIZE);
    TEST_ESP_OK(sdmmc_read_sectors(&card, rd, sector, TEST_XFER_SIZE / block_size));
    TEST_ASSERT_EQUAL_HEX32_ARRAY(good, rd, TEST_XFER_SIZE / sizeof(uint32_t));

    fill_pattern(good, TEST_XFER_SIZE, 0x44440000u);
    TEST_ESP_OK(sdmmc_write_sectors(&card, good, sector, TEST_XFER_SIZE / block_size));
    memset(rd, 0, TEST_XFER_SIZE);
    TEST_ESP_OK(sdmmc_read_sectors(&card, rd, sector, TEST_XFER_SIZE / block_size));
    TEST_ASSERT_EQUAL_HEX32_ARRAY(good, rd, TEST_XFER_SIZE / sizeof(uint32_t));

    free(good);
    free(rd);
    free(big);
    sdmmc_test_sd_end(&card);
}

TEST_CASE("SDMMC recovers after mid-transfer abort of CMD18 (read), slot 0, 1-bit", "[sdmmc][dma_recovery]")
{
    do_midtransfer_abort_test(SLOT_0, 1, SDMMC_FREQ_DEFAULT, false, false);
}

TEST_CASE("SDMMC recovers after mid-transfer abort of CMD25 (write), slot 0, 1-bit", "[sdmmc][dma_recovery]")
{
    do_midtransfer_abort_test(SLOT_0, 1, SDMMC_FREQ_DEFAULT, true, false);
}

TEST_CASE("SDMMC read_sectors recovers after mid-transfer abort of CMD18, slot 0, 1-bit", "[sdmmc][dma_recovery]")
{
    do_midtransfer_abort_test(SLOT_0, 1, SDMMC_FREQ_DEFAULT, false, true);
}

TEST_CASE("SDMMC write_sectors recovers after mid-transfer abort of CMD25, slot 0, 1-bit", "[sdmmc][dma_recovery]")
{
    do_midtransfer_abort_test(SLOT_0, 1, SDMMC_FREQ_DEFAULT, true, true);
}

TEST_CASE("SDMMC recovers after RTO on CMD25 (write), slot 0, 4-bit", "[sdmmc][dma_recovery]")
{
    do_rto_recovery_test(SLOT_0, 4, true);
}

TEST_CASE("SDMMC recovers after RTO on CMD18 (read), slot 0, 4-bit", "[sdmmc][dma_recovery]")
{
    do_rto_recovery_test(SLOT_0, 4, false);
}
