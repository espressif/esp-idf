/*
 * Copyright (c) 2006 Uwe Stuehler <uwe@openbsd.org>
 * Adaptations to ESP-IDF Copyright (c) 2016-2018 Espressif Systems (Shanghai) PTE LTD
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#pragma once

#include <string.h>
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/sdmmc_defs.h"
#include "driver/sdmmc_types.h"
#include "sdmmc_cmd.h"
#include "sys/param.h"
#include "soc/soc_memory_layout.h"
#include "esp_dma_utils.h"

#define SDMMC_GO_IDLE_DELAY_MS              20
#define SDMMC_IO_SEND_OP_COND_DELAY_MS      10

#define SDMMC_INIT_WAIT_DATA_READY_TIMEOUT_US  (5000 * 1000)
#define SDMMC_READY_FOR_DATA_TIMEOUT_US        (5000 * 1000)

/* These delay values are mostly useful for cases when CD pin is not used, and
 * the card is removed. In this case, SDMMC peripheral may not always return
 * CMD_DONE / DATA_DONE interrupts after signaling the error. These timeouts work
 * as a safety net in such cases.
 */
#define SDMMC_DEFAULT_CMD_TIMEOUT_MS  1000   // Max timeout of ordinary commands
#define SDMMC_WRITE_CMD_TIMEOUT_MS    5000   // Max timeout of write commands


#define SDMMC_SD_DISCARD_TIMEOUT  250    // SD erase (discard) timeout

/* Maximum retry/error count for SEND_OP_COND (CMD1).
 * These are somewhat arbitrary, values originate from OpenBSD driver.
 */
#define SDMMC_SEND_OP_COND_MAX_RETRIES  100
#define SDMMC_SEND_OP_COND_MAX_ERRORS   3

/* supported arguments for erase command 38 */
#define SDMMC_SD_ERASE_ARG      0
#define SDMMC_SD_DISCARD_ARG    1
#define SDMMC_MMC_TRIM_ARG      1
#define SDMMC_MMC_DISCARD_ARG   3

/* Functions to send individual commands */
esp_err_t sdmmc_send_cmd(sdmmc_card_t* card, sdmmc_command_t* cmd);
esp_err_t sdmmc_send_app_cmd(sdmmc_card_t* card, sdmmc_command_t* cmd);
esp_err_t sdmmc_send_cmd_go_idle_state(sdmmc_card_t* card);
esp_err_t sdmmc_send_cmd_send_if_cond(sdmmc_card_t* card, uint32_t ocr);
esp_err_t sdmmc_send_cmd_send_op_cond(sdmmc_card_t* card, uint32_t ocr, uint32_t *ocrp);
esp_err_t sdmmc_send_cmd_read_ocr(sdmmc_card_t *card, uint32_t *ocrp);
esp_err_t sdmmc_send_cmd_send_cid(sdmmc_card_t *card, sdmmc_cid_t *out_cid);
esp_err_t sdmmc_send_cmd_all_send_cid(sdmmc_card_t* card, sdmmc_response_t* out_raw_cid);
esp_err_t sdmmc_send_cmd_set_relative_addr(sdmmc_card_t* card, uint16_t* out_rca);
esp_err_t sdmmc_send_cmd_set_blocklen(sdmmc_card_t* card, sdmmc_csd_t* csd);
esp_err_t sdmmc_send_cmd_switch_func(sdmmc_card_t* card,
        uint32_t mode, uint32_t group, uint32_t function,
        sdmmc_switch_func_rsp_t* resp);
esp_err_t sdmmc_send_cmd_send_csd(sdmmc_card_t* card, sdmmc_csd_t* out_csd);
esp_err_t sdmmc_send_cmd_select_card(sdmmc_card_t* card, uint32_t rca);
esp_err_t sdmmc_send_cmd_send_scr(sdmmc_card_t* card, sdmmc_scr_t *out_scr);
esp_err_t sdmmc_send_cmd_set_bus_width(sdmmc_card_t* card, int width);
esp_err_t sdmmc_send_cmd_send_status(sdmmc_card_t* card, uint32_t* out_status);
esp_err_t sdmmc_send_cmd_crc_on_off(sdmmc_card_t* card, bool crc_enable);

/* Higher level functions */
esp_err_t sdmmc_enable_hs_mode(sdmmc_card_t* card);
esp_err_t sdmmc_enable_hs_mode_and_check(sdmmc_card_t* card);
esp_err_t sdmmc_write_sectors_dma(sdmmc_card_t* card, const void* src,
        size_t start_block, size_t block_count, size_t buffer_len);
esp_err_t sdmmc_read_sectors_dma(sdmmc_card_t* card, void* dst,
        size_t start_block, size_t block_count, size_t buffer_len);
uint32_t sdmmc_get_erase_timeout_ms(const sdmmc_card_t* card, int arg, size_t erase_size_kb);

/* SD specific */
esp_err_t sdmmc_check_scr(sdmmc_card_t* card);
esp_err_t sdmmc_decode_cid(sdmmc_response_t resp, sdmmc_cid_t* out_cid);
esp_err_t sdmmc_decode_csd(sdmmc_response_t response, sdmmc_csd_t* out_csd);
esp_err_t sdmmc_decode_scr(uint32_t *raw_scr, sdmmc_scr_t* out_scr);
esp_err_t sdmmc_decode_ssr(uint32_t *raw_ssr, sdmmc_ssr_t* out_ssr);
uint32_t sdmmc_sd_get_erase_timeout_ms(const sdmmc_card_t* card, int arg, size_t erase_size_kb);

/* SDIO specific */
esp_err_t sdmmc_io_reset(sdmmc_card_t* card);
esp_err_t sdmmc_io_enable_hs_mode(sdmmc_card_t* card);
esp_err_t sdmmc_io_send_op_cond(sdmmc_card_t* card, uint32_t ocr, uint32_t *ocrp);
esp_err_t sdmmc_io_rw_direct(sdmmc_card_t* card, int function,
        uint32_t reg, uint32_t arg, uint8_t *byte);
esp_err_t sdmmc_io_rw_extended(sdmmc_card_t* card, int function,
        uint32_t reg, int arg, void *data, size_t size);


/* MMC specific */
esp_err_t sdmmc_mmc_send_ext_csd_data(sdmmc_card_t* card, void *out_data, size_t datalen, size_t buffer_len);
esp_err_t sdmmc_mmc_switch(sdmmc_card_t* card, uint8_t set, uint8_t index, uint8_t value);
esp_err_t sdmmc_mmc_decode_cid(int mmc_ver, sdmmc_response_t resp, sdmmc_cid_t* out_cid);
esp_err_t sdmmc_mmc_decode_csd(sdmmc_response_t response, sdmmc_csd_t* out_csd);
esp_err_t sdmmc_mmc_enable_hs_mode(sdmmc_card_t* card);
uint32_t sdmmc_mmc_get_erase_timeout_ms(const sdmmc_card_t* card, int arg, size_t erase_size_kb);

/* Parts of card initialization flow */
esp_err_t sdmmc_init_sd_if_cond(sdmmc_card_t* card);
esp_err_t sdmmc_init_select_card(sdmmc_card_t* card);
esp_err_t sdmmc_init_csd(sdmmc_card_t* card);
esp_err_t sdmmc_init_cid(sdmmc_card_t* card);
esp_err_t sdmmc_init_rca(sdmmc_card_t* card);
esp_err_t sdmmc_init_mmc_decode_cid(sdmmc_card_t* card);
esp_err_t sdmmc_init_ocr(sdmmc_card_t* card);
esp_err_t sdmmc_init_spi_crc(sdmmc_card_t* card);
esp_err_t sdmmc_init_io(sdmmc_card_t* card);
esp_err_t sdmmc_init_sd_blocklen(sdmmc_card_t* card);
esp_err_t sdmmc_init_sd_scr(sdmmc_card_t* card);
esp_err_t sdmmc_init_sd_ssr(sdmmc_card_t* card);
esp_err_t sdmmc_init_sd_wait_data_ready(sdmmc_card_t* card);
esp_err_t sdmmc_init_mmc_read_ext_csd(sdmmc_card_t* card);
esp_err_t sdmmc_init_mmc_read_cid(sdmmc_card_t* card);
esp_err_t sdmmc_init_host_bus_width(sdmmc_card_t* card);
esp_err_t sdmmc_init_sd_bus_width(sdmmc_card_t* card);
esp_err_t sdmmc_init_io_bus_width(sdmmc_card_t* card);
esp_err_t sdmmc_init_mmc_bus_width(sdmmc_card_t* card);
esp_err_t sdmmc_init_card_hs_mode(sdmmc_card_t* card);
esp_err_t sdmmc_init_host_frequency(sdmmc_card_t* card);
esp_err_t sdmmc_init_mmc_check_ext_csd(sdmmc_card_t* card);

/* Various helper functions */
static inline bool host_is_spi(const sdmmc_card_t* card)
{
    return (card->host.flags & SDMMC_HOST_FLAG_SPI) != 0;
}

static inline uint32_t get_host_ocr(float voltage)
{
    // TODO: report exact voltage to the card
    // For now tell that the host has 2.8-3.6V voltage range
    (void) voltage;
    return SD_OCR_VOL_MASK;
}

void sdmmc_flip_byte_order(uint32_t* response, size_t size);

esp_err_t sdmmc_fix_host_flags(sdmmc_card_t* card);
