/*
 * Copyright (c) 2006 Uwe Stuehler <uwe@openbsd.org>
 * Adaptations to ESP-IDF Copyright (c) 2016-2024 Espressif Systems (Shanghai) PTE LTD
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

#include <inttypes.h>
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_private/sdmmc_common.h"

static const char* TAG = "sdmmc_common";

esp_err_t sdmmc_init_ocr(sdmmc_card_t* card)
{
    esp_err_t err;
    /* In SPI mode, READ_OCR (CMD58) command is used to figure out which voltage
     * ranges the card can support. This step is skipped since 1.8V isn't
     * supported on the ESP32.
     */

    uint32_t host_ocr = get_host_ocr(card->host.io_voltage);

    /* In SPI mode, the only non-zero bit of ACMD41 is HCS (bit 30)
     * In SD mode, bits 23:8 contain the supported voltage mask
     */
    uint32_t acmd41_arg = 0;
    if (!host_is_spi(card)) {
        acmd41_arg = host_ocr;
    }

    if ((card->ocr & SD_OCR_SDHC_CAP) != 0) {
        acmd41_arg |= SD_OCR_SDHC_CAP;
    }

    bool to_set_to_uhs1 = false;
    if (card->host.is_slot_set_to_uhs1) {
        ESP_RETURN_ON_ERROR(card->host.is_slot_set_to_uhs1(card->host.slot, &to_set_to_uhs1), TAG, "failed to get slot info");
    }
    if (to_set_to_uhs1) {
        acmd41_arg |= SD_OCR_S18_RA;
        acmd41_arg |= SD_OCR_XPC;
    }
    ESP_LOGV(TAG, "%s: acmd41_arg=0x%08" PRIx32, __func__, card->ocr);

    /* Send SEND_OP_COND (ACMD41) command to the card until it becomes ready. */
    err = sdmmc_send_cmd_send_op_cond(card, acmd41_arg, &card->ocr);

    /* If time-out, re-try send_op_cond as MMC */
    if (err == ESP_ERR_TIMEOUT && !host_is_spi(card)) {
        ESP_LOGD(TAG, "send_op_cond timeout, trying MMC");
        card->is_mmc = 1;
        err = sdmmc_send_cmd_send_op_cond(card, acmd41_arg, &card->ocr);
    }

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: send_op_cond (1) returned 0x%x", __func__, err);
        return err;
    }
    if (host_is_spi(card)) {
        err = sdmmc_send_cmd_read_ocr(card, &card->ocr);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "%s: read_ocr returned 0x%x", __func__, err);
            return err;
        }
    }
    ESP_LOGD(TAG, "host_ocr=0x%" PRIx32 " card_ocr=0x%" PRIx32, host_ocr, card->ocr);

    /* Clear all voltage bits in host's OCR which the card doesn't support.
     * Don't touch CCS bit because in SPI mode cards don't report CCS in ACMD41
     * response.
     */
    host_ocr &= (card->ocr | (~SD_OCR_VOL_MASK));
    ESP_LOGD(TAG, "sdmmc_card_init: host_ocr=%08" PRIx32 ", card_ocr=%08" PRIx32, host_ocr, card->ocr);
    return ESP_OK;
}

esp_err_t sdmmc_init_cid(sdmmc_card_t* card)
{
    esp_err_t err;
    sdmmc_response_t raw_cid;
    if (!host_is_spi(card)) {
        err = sdmmc_send_cmd_all_send_cid(card, &raw_cid);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "%s: all_send_cid returned 0x%x", __func__, err);
            return err;
        }
        if (!card->is_mmc) {
            err = sdmmc_decode_cid(raw_cid, &card->cid);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "%s: decoding CID failed (0x%x)", __func__, err);
                return err;
            }
        } else {
            /* For MMC, need to know CSD to decode CID. But CSD can only be read
             * in data transfer mode, and it is not possible to read CID in data
             * transfer mode. We temporiliy store the raw cid and do the
             * decoding after the RCA is set and the card is in data transfer
             * mode.
             */
            memcpy(card->raw_cid, raw_cid, sizeof(sdmmc_response_t));
        }
    } else {
        err = sdmmc_send_cmd_send_cid(card, &card->cid);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "%s: send_cid returned 0x%x", __func__, err);
            return err;
        }
    }
    return ESP_OK;
}

esp_err_t sdmmc_init_rca(sdmmc_card_t* card)
{
    esp_err_t err;
    err = sdmmc_send_cmd_set_relative_addr(card, &card->rca);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: set_relative_addr returned 0x%x", __func__, err);
        return err;
    }
    return ESP_OK;
}

esp_err_t sdmmc_init_mmc_decode_cid(sdmmc_card_t* card)
{
    esp_err_t err;
    sdmmc_response_t raw_cid;
    memcpy(raw_cid, card->raw_cid, sizeof(raw_cid));
    err = sdmmc_mmc_decode_cid(card->csd.mmc_ver, raw_cid, &card->cid);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: decoding CID failed (0x%x)", __func__, err);
        return err;
    }
    return ESP_OK;
}

esp_err_t sdmmc_init_csd(sdmmc_card_t* card)
{
    assert(card->is_mem == 1);
    /* Get and decode the contents of CSD register. Determine card capacity. */
    esp_err_t err = sdmmc_send_cmd_send_csd(card, &card->csd);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: send_csd returned 0x%x", __func__, err);
        return err;
    }
    const size_t max_sdsc_capacity = UINT32_MAX / card->csd.sector_size + 1;
    if (!(card->ocr & SD_OCR_SDHC_CAP) &&
         card->csd.capacity > max_sdsc_capacity) {
        ESP_LOGW(TAG, "%s: SDSC card reports capacity=%u. Limiting to %u.",
                __func__, card->csd.capacity, max_sdsc_capacity);
        card->csd.capacity = max_sdsc_capacity;
    }
    return ESP_OK;
}

esp_err_t sdmmc_init_select_card(sdmmc_card_t* card)
{
    assert(!host_is_spi(card));
    esp_err_t err = sdmmc_send_cmd_select_card(card, card->rca);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: select_card returned 0x%x", __func__, err);
        return err;
    }
    return ESP_OK;
}

esp_err_t sdmmc_init_card_hs_mode(sdmmc_card_t* card)
{
    esp_err_t err = ESP_ERR_NOT_SUPPORTED;
    if (card->is_mem && !card->is_mmc) {
        err = sdmmc_enable_hs_mode_and_check(card);
    } else if (card->is_sdio) {
        err = sdmmc_io_enable_hs_mode(card);
    } else if (card->is_mmc){
        err = sdmmc_mmc_enable_hs_mode(card);
    }
    if (err == ESP_ERR_NOT_SUPPORTED) {
        ESP_LOGD(TAG, "%s: host supports HS mode, but card doesn't", __func__);
        card->max_freq_khz = SDMMC_FREQ_DEFAULT;
    } else if (err != ESP_OK) {
        return err;
    }
    return ESP_OK;
}

esp_err_t sdmmc_init_sd_driver_strength(sdmmc_card_t *card)
{
    return sdmmc_select_driver_strength(card, card->host.driver_strength);
}

esp_err_t sdmmc_init_sd_current_limit(sdmmc_card_t *card)
{
    return sdmmc_select_current_limit(card, card->host.current_limit);
}

esp_err_t sdmmc_init_sd_timing_tuning(sdmmc_card_t *card)
{
    return sdmmc_do_timing_tuning(card);
}

esp_err_t sdmmc_init_host_bus_width(sdmmc_card_t* card)
{
    int bus_width = 1;

    if ((card->host.flags & SDMMC_HOST_FLAG_4BIT) &&
        (card->log_bus_width == 2)) {
        bus_width = 4;
    } else if ((card->host.flags & SDMMC_HOST_FLAG_8BIT) &&
        (card->log_bus_width == 3)) {
        bus_width = 8;
    }
    ESP_LOGD(TAG, "%s: using %d-bit bus", __func__, bus_width);
    if (bus_width > 1) {
        esp_err_t err = (*card->host.set_bus_width)(card->host.slot, bus_width);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "host.set_bus_width failed (0x%x)", err);
            return err;
        }
    }
    return ESP_OK;
}

esp_err_t sdmmc_init_host_frequency(sdmmc_card_t* card)
{
    esp_err_t err;
    assert(card->max_freq_khz <= card->host.max_freq_khz);

#if !SOC_SDMMC_UHS_I_SUPPORTED
    ESP_RETURN_ON_FALSE(card->host.input_delay_phase != SDMMC_DELAY_PHASE_AUTO, ESP_ERR_INVALID_ARG, TAG, "auto tuning not supported");
#endif

    if (card->host.input_delay_phase == SDMMC_DELAY_PHASE_AUTO) {
        ESP_RETURN_ON_FALSE((card->host.max_freq_khz == SDMMC_FREQ_SDR50 || card->host.max_freq_khz == SDMMC_FREQ_SDR104), ESP_ERR_INVALID_ARG, TAG, "auto tuning only supported for SDR50 / SDR104");
    }

    if (card->max_freq_khz > SDMMC_FREQ_PROBING) {
        err = (*card->host.set_card_clk)(card->host.slot, card->max_freq_khz);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "failed to switch bus frequency (0x%x)", err);
            return err;
        }
    }

    if (card->host.input_delay_phase != SDMMC_DELAY_PHASE_0) {
        if (card->host.set_input_delay) {
            err = (*card->host.set_input_delay)(card->host.slot, card->host.input_delay_phase);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "host.set_input_delay failed (0x%x)", err);
                return err;
            }
        } else {
            ESP_LOGE(TAG, "input phase delay feature isn't supported");
            return ESP_ERR_NOT_SUPPORTED;
        }
    }

    err = (*card->host.get_real_freq)(card->host.slot, &(card->real_freq_khz));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to get real working frequency (0x%x)", err);
        return err;
    }

    if (card->is_ddr) {
        if (card->host.set_bus_ddr_mode == NULL) {
            ESP_LOGE(TAG, "host doesn't support DDR mode or voltage switching");
            return ESP_ERR_NOT_SUPPORTED;
        }
        err = (*card->host.set_bus_ddr_mode)(card->host.slot, true);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "failed to switch bus to DDR mode (0x%x)", err);
            return err;
        }
    }
    return ESP_OK;
}

void sdmmc_flip_byte_order(uint32_t* response, size_t size)
{
    assert(size % (2 * sizeof(uint32_t)) == 0);
    const size_t n_words = size / sizeof(uint32_t);
    for (int i = 0; i < n_words / 2; ++i) {
        uint32_t left = __builtin_bswap32(response[i]);
        uint32_t right = __builtin_bswap32(response[n_words - i - 1]);
        response[i] = right;
        response[n_words - i - 1] = left;
    }
}

void sdmmc_card_print_info(FILE* stream, const sdmmc_card_t* card)
{
    bool print_scr = false;
    bool print_csd = false;
    const char* type;

    fprintf(stream, "Name: %s\n", card->cid.name);

    if (card->is_sdio) {
        type = "SDIO";
        print_scr = true;
        print_csd = true;
    } else if (card->is_mmc) {
        type = "MMC";
        print_csd = true;
    } else {
        if ((card->ocr & SD_OCR_SDHC_CAP) == 0) {
            type = "SDSC";
        } else {
            if (card->ocr & SD_OCR_S18_RA) {
                type = "SDHC/SDXC (UHS-I)";
            } else {
                type = "SDHC";
            }
        }
        print_csd = true;
    }
    fprintf(stream, "Type: %s\n", type);

    if (card->real_freq_khz == 0) {
        fprintf(stream, "Speed: N/A\n");
    } else {
        const char *freq_unit = card->real_freq_khz < 1000 ? "kHz" : "MHz";
        const float freq = card->real_freq_khz < 1000 ? card->real_freq_khz : card->real_freq_khz / 1000.0;
        const char *max_freq_unit = card->max_freq_khz < 1000 ? "kHz" : "MHz";
        const float max_freq = card->max_freq_khz < 1000 ? card->max_freq_khz : card->max_freq_khz / 1000.0;
        fprintf(stream, "Speed: %.2f %s (limit: %.2f %s)%s\n", freq, freq_unit, max_freq, max_freq_unit, card->is_ddr ? ", DDR" : "");
    }

    fprintf(stream, "Size: %lluMB\n", ((uint64_t) card->csd.capacity) * card->csd.sector_size / (1024 * 1024));

    if (print_csd) {
        fprintf(stream, "CSD: ver=%d, sector_size=%d, capacity=%d read_bl_len=%d\n",
                (int) (card->is_mmc ? card->csd.csd_ver : card->csd.csd_ver + 1),
                card->csd.sector_size, card->csd.capacity, card->csd.read_block_len);
        if (card->is_mmc) {
            fprintf(stream, "EXT CSD: bus_width=%" PRIu32 "\n", (uint32_t) (1 << card->log_bus_width));
        } else if (!card->is_sdio){ // make sure card is SD
            fprintf(stream, "SSR: bus_width=%" PRIu32 "\n", (uint32_t) (card->ssr.cur_bus_width ? 4 : 1));
        }
    }
    if (print_scr) {
        fprintf(stream, "SCR: sd_spec=%d, bus_width=%d\n", card->scr.sd_spec, card->scr.bus_width);
    }
}

esp_err_t sdmmc_fix_host_flags(sdmmc_card_t* card)
{
    const uint32_t width_1bit = SDMMC_HOST_FLAG_1BIT;
    const uint32_t width_4bit = SDMMC_HOST_FLAG_4BIT;
    const uint32_t width_8bit = SDMMC_HOST_FLAG_8BIT;
    const uint32_t width_mask = width_1bit | width_4bit | width_8bit;

    int slot_bit_width = card->host.get_bus_width(card->host.slot);
    if (slot_bit_width == 1 &&
            (card->host.flags & (width_4bit | width_8bit))) {
        card->host.flags &= ~width_mask;
        card->host.flags |= width_1bit;
    } else if (slot_bit_width == 4 && (card->host.flags & width_8bit)) {
        if ((card->host.flags & width_4bit) == 0) {
            ESP_LOGW(TAG, "slot width set to 4, but host flags don't have 4 line mode enabled; using 1 line mode");
            card->host.flags &= ~width_mask;
            card->host.flags |= width_1bit;
        } else {
            card->host.flags &= ~width_mask;
            card->host.flags |= width_4bit;
        }
    }

#if !SOC_SDMMC_UHS_I_SUPPORTED
    if ((card->host.max_freq_khz == SDMMC_FREQ_SDR50) ||
        (card->host.max_freq_khz == SDMMC_FREQ_DDR50) ||
        (card->host.max_freq_khz == SDMMC_FREQ_SDR104)) {
        ESP_RETURN_ON_FALSE(false, ESP_ERR_NOT_SUPPORTED, TAG, "UHS-I is not supported");
    }
#else
    if (card->host.max_freq_khz == SDMMC_FREQ_DDR50) {
        ESP_RETURN_ON_FALSE(((card->host.flags & SDMMC_HOST_FLAG_DDR) != 0), ESP_ERR_INVALID_ARG, TAG, "DDR is not selected");
    }
#endif

    return ESP_OK;
}

esp_err_t sdmmc_allocate_aligned_buf(sdmmc_card_t* card)
{
    if (card->host.flags & SDMMC_HOST_FLAG_ALLOC_ALIGNED_BUF) {
        void* buf = NULL;
        size_t actual_size = 0;
        esp_dma_mem_info_t dma_mem_info;
        card->host.get_dma_info(card->host.slot, &dma_mem_info);
        esp_err_t ret = esp_dma_capable_malloc(SDMMC_IO_BLOCK_SIZE, &dma_mem_info, &buf, &actual_size);

        if (ret != ESP_OK) {
            return ret;
        }
        assert(actual_size == SDMMC_IO_BLOCK_SIZE);
        card->host.dma_aligned_buffer = buf;
    }
    return ESP_OK;
}

uint32_t sdmmc_get_erase_timeout_ms(const sdmmc_card_t* card, int arg, size_t erase_size_kb)
{
    if (card->is_mmc) {
        return sdmmc_mmc_get_erase_timeout_ms(card, arg, erase_size_kb);
    } else {
        return sdmmc_sd_get_erase_timeout_ms(card, arg, erase_size_kb);
    }
}

esp_err_t sdmmc_wait_for_idle(sdmmc_card_t* card, uint32_t status)
{
    assert(!host_is_spi(card));
    esp_err_t err = ESP_OK;
    size_t count = 0;
    int64_t yield_delay_us = 100 * 1000; // initially 100ms
    int64_t t0 = esp_timer_get_time();
    int64_t t1 = 0;
    /* SD mode: wait for the card to become idle based on R1 status */
    while (!sdmmc_ready_for_data(status)) {
        t1 = esp_timer_get_time();
        if (t1 - t0 > SDMMC_READY_FOR_DATA_TIMEOUT_US) {
            return ESP_ERR_TIMEOUT;
        }
        if (t1 - t0 > yield_delay_us) {
            yield_delay_us *= 2;
            vTaskDelay(1);
        }
        err = sdmmc_send_cmd_send_status(card, &status);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "%s: sdmmc_send_cmd_send_status returned 0x%x", __func__, err);
            return err;
        }
        if (++count % 16 == 0) {
            ESP_LOGV(TAG, "waiting for card to become ready (%" PRIu32 ")", (uint32_t) count);
        }
    }
    return err;
}
