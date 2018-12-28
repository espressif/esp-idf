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

#include "sdmmc_common.h"

static const char* TAG = "sdmmc_io";

esp_err_t sdmmc_io_reset(sdmmc_card_t* card)
{
    uint8_t sdio_reset = CCCR_CTL_RES;
    esp_err_t err = sdmmc_io_rw_direct(card, 0, SD_IO_CCCR_CTL, SD_ARG_CMD52_WRITE, &sdio_reset);
    if (err == ESP_ERR_TIMEOUT || (host_is_spi(card) && err == ESP_ERR_NOT_SUPPORTED)) {
        /* Non-IO cards are allowed to time out (in SD mode) or
         * return "invalid command" error (in SPI mode).
         */
    } else if (err == ESP_ERR_NOT_FOUND) {
        ESP_LOGD(TAG, "%s: card not present", __func__);
        return err;
    } else if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: unexpected return: 0x%x", __func__, err );
        return err;
    }
    return ESP_OK;
}

esp_err_t sdmmc_init_io(sdmmc_card_t* card)
{
    /* IO_SEND_OP_COND(CMD5), Determine if the card is an IO card.
     * Non-IO cards will not respond to this command.
     */
    esp_err_t err = sdmmc_io_send_op_cond(card, 0, &card->ocr);
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "%s: io_send_op_cond (1) returned 0x%x; not IO card", __func__, err);
        card->is_sdio = 0;
        card->is_mem = 1;
    } else {
        card->is_sdio = 1;

        if (card->ocr & SD_IO_OCR_MEM_PRESENT) {
            ESP_LOGD(TAG, "%s: IO-only card", __func__);
            card->is_mem = 0;
        }
        card->num_io_functions = SD_IO_OCR_NUM_FUNCTIONS(card->ocr);
        ESP_LOGD(TAG, "%s: number of IO functions: %d", __func__, card->num_io_functions);
        if (card->num_io_functions == 0) {
            card->is_sdio = 0;
        }
        uint32_t host_ocr = get_host_ocr(card->host.io_voltage);
        host_ocr &= card->ocr;
        err = sdmmc_io_send_op_cond(card, host_ocr, &card->ocr);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "%s: sdmmc_io_send_op_cond (1) returned 0x%x", __func__, err);
            return err;
        }
        err = sdmmc_io_enable_int(card);
        if (err != ESP_OK) {
            ESP_LOGD(TAG, "%s: sdmmc_enable_int failed (0x%x)", __func__, err);
        }
    }
    return ESP_OK;
}

esp_err_t sdmmc_init_io_bus_width(sdmmc_card_t* card)
{
    esp_err_t err;
    card->log_bus_width = 0;
    if (card->host.flags & SDMMC_HOST_FLAG_4BIT) {
        uint8_t card_cap = 0;
        err = sdmmc_io_rw_direct(card, 0, SD_IO_CCCR_CARD_CAP,
                SD_ARG_CMD52_READ, &card_cap);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "%s: sdmmc_io_rw_direct (read SD_IO_CCCR_CARD_CAP) returned 0x%0x", __func__, err);
            return err;
        }
        ESP_LOGD(TAG, "IO card capabilities byte: %02x", card_cap);
        if (!(card_cap & CCCR_CARD_CAP_LSC) ||
                (card_cap & CCCR_CARD_CAP_4BLS)) {
            // This card supports 4-bit bus mode
            uint8_t bus_width = CCCR_BUS_WIDTH_4;
            err = sdmmc_io_rw_direct(card, 0, SD_IO_CCCR_BUS_WIDTH,
                                SD_ARG_CMD52_WRITE, &bus_width);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "%s: sdmmc_io_rw_direct (write SD_IO_CCCR_BUS_WIDTH) returned 0x%0x", __func__, err);
                return err;
            }
            card->log_bus_width = 2;
        }
    }
    return ESP_OK;
}


esp_err_t sdmmc_io_enable_hs_mode(sdmmc_card_t* card)
{
    /* If the host is configured to use low frequency, don't attempt to switch */
    if (card->host.max_freq_khz < SDMMC_FREQ_DEFAULT) {
        card->max_freq_khz = card->host.max_freq_khz;
        return ESP_OK;
    } else if (card->host.max_freq_khz < SDMMC_FREQ_HIGHSPEED) {
        card->max_freq_khz = SDMMC_FREQ_DEFAULT;
        return ESP_OK;
    }

    /* For IO cards, do write + read operation on "High Speed" register,
     * setting EHS bit. If both EHS and SHS read back as set, then HS mode
     * has been enabled.
     */
    uint8_t val = CCCR_HIGHSPEED_ENABLE;
    esp_err_t err = sdmmc_io_rw_direct(card, 0, SD_IO_CCCR_HIGHSPEED,
            SD_ARG_CMD52_WRITE | SD_ARG_CMD52_EXCHANGE, &val);
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "%s: sdmmc_io_rw_direct returned 0x%x", __func__, err);
        return err;
    }

    ESP_LOGD(TAG, "%s: CCCR_HIGHSPEED=0x%02x", __func__, val);
    const uint8_t hs_mask = CCCR_HIGHSPEED_ENABLE | CCCR_HIGHSPEED_SUPPORT;
    if ((val & hs_mask) != hs_mask) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    card->max_freq_khz = SDMMC_FREQ_HIGHSPEED;
    return ESP_OK;
}


esp_err_t sdmmc_io_send_op_cond(sdmmc_card_t* card, uint32_t ocr, uint32_t *ocrp)
{
    esp_err_t err = ESP_OK;
    sdmmc_command_t cmd = {
        .flags = SCF_CMD_BCR | SCF_RSP_R4,
        .arg = ocr,
        .opcode = SD_IO_SEND_OP_COND
    };
    for (size_t i = 0; i < 100; i++) {
        err = sdmmc_send_cmd(card, &cmd);
        if (err != ESP_OK) {
            break;
        }
        if ((MMC_R4(cmd.response) & SD_IO_OCR_MEM_READY) ||
            ocr == 0) {
            break;
        }
        err = ESP_ERR_TIMEOUT;
        vTaskDelay(SDMMC_IO_SEND_OP_COND_DELAY_MS / portTICK_PERIOD_MS);
    }
    if (err == ESP_OK && ocrp != NULL)
        *ocrp = MMC_R4(cmd.response);

    return err;
}

esp_err_t sdmmc_io_rw_direct(sdmmc_card_t* card, int func,
        uint32_t reg, uint32_t arg, uint8_t *byte)
{
    esp_err_t err;
    sdmmc_command_t cmd = {
        .flags = SCF_CMD_AC | SCF_RSP_R5,
        .arg = 0,
        .opcode = SD_IO_RW_DIRECT
    };

    arg |= (func & SD_ARG_CMD52_FUNC_MASK) << SD_ARG_CMD52_FUNC_SHIFT;
    arg |= (reg & SD_ARG_CMD52_REG_MASK) << SD_ARG_CMD52_REG_SHIFT;
    arg |= (*byte & SD_ARG_CMD52_DATA_MASK) << SD_ARG_CMD52_DATA_SHIFT;
    cmd.arg = arg;

    err = sdmmc_send_cmd(card, &cmd);
    if (err != ESP_OK) {
        ESP_LOGV(TAG, "%s: sdmmc_send_cmd returned 0x%x", __func__, err);
        return err;
    }

    *byte = SD_R5_DATA(cmd.response);

    return ESP_OK;
}


esp_err_t sdmmc_io_read_byte(sdmmc_card_t* card, uint32_t function,
        uint32_t addr, uint8_t *out_byte)
{
    esp_err_t ret = sdmmc_io_rw_direct(card, function, addr, SD_ARG_CMD52_READ, out_byte);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "%s: sdmmc_io_rw_direct (read 0x%x) returned 0x%x", __func__, addr, ret);
    }
    return ret;
}

esp_err_t sdmmc_io_write_byte(sdmmc_card_t* card, uint32_t function,
        uint32_t addr, uint8_t in_byte, uint8_t* out_byte)
{
    uint8_t tmp_byte = in_byte;
    esp_err_t ret = sdmmc_io_rw_direct(card, function, addr,
            SD_ARG_CMD52_WRITE | SD_ARG_CMD52_EXCHANGE, &tmp_byte);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "%s: sdmmc_io_rw_direct (write 0x%x) returned 0x%x", __func__, addr, ret);
        return ret;
    }
    if (out_byte != NULL) {
        *out_byte = tmp_byte;
    }
    return ESP_OK;
}

esp_err_t sdmmc_io_rw_extended(sdmmc_card_t* card, int func,
    uint32_t reg, int arg, void *datap, size_t datalen)
{
    esp_err_t err;
    const size_t max_byte_transfer_size = 512;
    sdmmc_command_t cmd = {
        .flags = SCF_CMD_AC | SCF_RSP_R5,
        .arg = 0,
        .opcode = SD_IO_RW_EXTENDED,
        .data = datap,
        .datalen = datalen,
        .blklen = max_byte_transfer_size /* TODO: read max block size from CIS */
    };

    uint32_t count; /* number of bytes or blocks, depending on transfer mode */
    if (arg & SD_ARG_CMD53_BLOCK_MODE) {
        if (cmd.datalen % cmd.blklen != 0) {
            return ESP_ERR_INVALID_SIZE;
        }
        count = cmd.datalen / cmd.blklen;
    } else {
        if (datalen > max_byte_transfer_size) {
            /* TODO: split into multiple operations? */
            return ESP_ERR_INVALID_SIZE;
        }
        if (datalen == max_byte_transfer_size) {
            count = 0;  // See 5.3.1 SDIO simplifed spec
        } else {
            count = datalen;
        }
        cmd.blklen = datalen;
    }

    arg |= (func & SD_ARG_CMD53_FUNC_MASK) << SD_ARG_CMD53_FUNC_SHIFT;
    arg |= (reg & SD_ARG_CMD53_REG_MASK) << SD_ARG_CMD53_REG_SHIFT;
    arg |= (count & SD_ARG_CMD53_LENGTH_MASK) << SD_ARG_CMD53_LENGTH_SHIFT;
    cmd.arg = arg;

    if ((arg & SD_ARG_CMD53_WRITE) == 0) {
        cmd.flags |= SCF_CMD_READ;
    }

    err = sdmmc_send_cmd(card, &cmd);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: sdmmc_send_cmd returned 0x%x", __func__, err);
        return err;
    }

    return ESP_OK;
}

esp_err_t sdmmc_io_read_bytes(sdmmc_card_t* card, uint32_t function,
        uint32_t addr, void* dst, size_t size)
{
    /* host quirk: SDIO transfer with length not divisible by 4 bytes
     * has to be split into two transfers: one with aligned length,
     * the other one for the remaining 1-3 bytes.
     */
    uint8_t *pc_dst = dst;
    while (size > 0) {
        size_t size_aligned = size & (~3);
        size_t will_transfer = size_aligned > 0 ? size_aligned : size;

        esp_err_t err = sdmmc_io_rw_extended(card, function, addr,
                SD_ARG_CMD53_READ | SD_ARG_CMD53_INCREMENT,
                pc_dst, will_transfer);
        if (err != ESP_OK) {
            return err;
        }
        pc_dst += will_transfer;
        size -= will_transfer;
        addr += will_transfer;
    }
    return ESP_OK;
}

esp_err_t sdmmc_io_write_bytes(sdmmc_card_t* card, uint32_t function,
        uint32_t addr, const void* src, size_t size)
{
    /* same host quirk as in sdmmc_io_read_bytes */
    const uint8_t *pc_src = (const uint8_t*) src;

    while (size > 0) {
        size_t size_aligned = size & (~3);
        size_t will_transfer = size_aligned > 0 ? size_aligned : size;

        esp_err_t err = sdmmc_io_rw_extended(card, function, addr,
                SD_ARG_CMD53_WRITE | SD_ARG_CMD53_INCREMENT,
                (void*) pc_src, will_transfer);
        if (err != ESP_OK) {
            return err;
        }
        pc_src += will_transfer;
        size -= will_transfer;
        addr += will_transfer;
    }
    return ESP_OK;
}

esp_err_t sdmmc_io_read_blocks(sdmmc_card_t* card, uint32_t function,
        uint32_t addr, void* dst, size_t size)
{
    if (size % 4 != 0) {
        return ESP_ERR_INVALID_SIZE;
    }
    return sdmmc_io_rw_extended(card, function, addr,
            SD_ARG_CMD53_READ | SD_ARG_CMD53_INCREMENT | SD_ARG_CMD53_BLOCK_MODE,
            dst, size);
}

esp_err_t sdmmc_io_write_blocks(sdmmc_card_t* card, uint32_t function,
        uint32_t addr, const void* src, size_t size)
{
    if (size % 4 != 0) {
        return ESP_ERR_INVALID_SIZE;
    }
    return sdmmc_io_rw_extended(card, function, addr,
            SD_ARG_CMD53_WRITE | SD_ARG_CMD53_INCREMENT | SD_ARG_CMD53_BLOCK_MODE,
            (void*) src, size);
}

esp_err_t sdmmc_io_enable_int(sdmmc_card_t* card)
{
    if (card->host.io_int_enable == NULL) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    return (*card->host.io_int_enable)(card->host.slot);
}

esp_err_t sdmmc_io_wait_int(sdmmc_card_t* card, TickType_t timeout_ticks)
{
    if (card->host.io_int_wait == NULL) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    return (*card->host.io_int_wait)(card->host.slot, timeout_ticks);
}
