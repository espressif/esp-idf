/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include "esp_timer.h"
#include "sdmmc_common.h"

static const char* TAG = "sdmmc_cmd";


esp_err_t sdmmc_send_cmd(sdmmc_card_t* card, sdmmc_command_t* cmd)
{
    if (card->host.command_timeout_ms != 0) {
        cmd->timeout_ms = card->host.command_timeout_ms;
    } else if (cmd->timeout_ms == 0) {
        cmd->timeout_ms = SDMMC_DEFAULT_CMD_TIMEOUT_MS;
    }

    int slot = card->host.slot;
    ESP_LOGV(TAG, "sending cmd slot=%d op=%" PRIu32 " arg=%" PRIx32 " flags=%x data=%p blklen=%" PRIu32 " datalen=%" PRIu32 " timeout=%" PRIu32,
            slot, cmd->opcode, cmd->arg, cmd->flags, cmd->data, (uint32_t) cmd->blklen, (uint32_t) cmd->datalen, cmd->timeout_ms);
    esp_err_t err = (*card->host.do_transaction)(slot, cmd);
    if (err != 0) {
        ESP_LOGD(TAG, "cmd=%" PRIu32 ", sdmmc_req_run returned 0x%x", cmd->opcode, err);
        return err;
    }
    int state = MMC_R1_CURRENT_STATE(cmd->response);
    ESP_LOGV(TAG, "cmd response %08" PRIx32 " %08" PRIx32 " %08" PRIx32 " %08" PRIx32 " err=0x%x state=%d",
               cmd->response[0],
               cmd->response[1],
               cmd->response[2],
               cmd->response[3],
               cmd->error,
               state);
    return cmd->error;
}

esp_err_t sdmmc_send_app_cmd(sdmmc_card_t* card, sdmmc_command_t* cmd)
{
    sdmmc_command_t app_cmd = {
        .opcode = MMC_APP_CMD,
        .flags = SCF_CMD_AC | SCF_RSP_R1,
        .arg = MMC_ARG_RCA(card->rca),
    };
    esp_err_t err = sdmmc_send_cmd(card, &app_cmd);
    if (err != ESP_OK) {
        return err;
    }
    // Check APP_CMD status bit (only in SD mode)
    if (!host_is_spi(card) && !(MMC_R1(app_cmd.response) & MMC_R1_APP_CMD)) {
        ESP_LOGW(TAG, "card doesn't support APP_CMD");
        return ESP_ERR_NOT_SUPPORTED;
    }
    return sdmmc_send_cmd(card, cmd);
}


esp_err_t sdmmc_send_cmd_go_idle_state(sdmmc_card_t* card)
{
    sdmmc_command_t cmd = {
        .opcode = MMC_GO_IDLE_STATE,
        .flags = SCF_CMD_BC | SCF_RSP_R0,
    };
    esp_err_t err = sdmmc_send_cmd(card, &cmd);
    if (host_is_spi(card)) {
        /* To enter SPI mode, CMD0 needs to be sent twice (see figure 4-1 in
         * SD Simplified spec v4.10). Some cards enter SD mode on first CMD0,
         * so don't expect the above command to succeed.
         * SCF_RSP_R1 flag below tells the lower layer to expect correct R1
         * response (in SPI mode).
         */
        (void) err;
        vTaskDelay(SDMMC_GO_IDLE_DELAY_MS / portTICK_PERIOD_MS);

        cmd.flags |= SCF_RSP_R1;
        err = sdmmc_send_cmd(card, &cmd);
    }
    if (err == ESP_OK) {
        vTaskDelay(SDMMC_GO_IDLE_DELAY_MS / portTICK_PERIOD_MS);
    }
    return err;
}


esp_err_t sdmmc_send_cmd_send_if_cond(sdmmc_card_t* card, uint32_t ocr)
{
    const uint8_t pattern = 0xaa; /* any pattern will do here */
    sdmmc_command_t cmd = {
        .opcode = SD_SEND_IF_COND,
        .arg = (((ocr & SD_OCR_VOL_MASK) != 0) << 8) | pattern,
        .flags = SCF_CMD_BCR | SCF_RSP_R7,
    };
    esp_err_t err = sdmmc_send_cmd(card, &cmd);
    if (err != ESP_OK) {
        return err;
    }
    uint8_t response = cmd.response[0] & 0xff;
    if (response != pattern) {
        ESP_LOGD(TAG, "%s: received=0x%x expected=0x%x", __func__, response, pattern);
        return ESP_ERR_INVALID_RESPONSE;
    }
    return ESP_OK;
}

esp_err_t sdmmc_send_cmd_send_op_cond(sdmmc_card_t* card, uint32_t ocr, uint32_t *ocrp)
{
    esp_err_t err;

    /* If the host supports this, keep card clock enabled
     * from the start of ACMD41 until the card is idle.
     * (Ref. SD spec, section 4.4 "Clock control".)
     */
    if (card->host.set_cclk_always_on != NULL) {
        err = card->host.set_cclk_always_on(card->host.slot, true);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "%s: set_cclk_always_on (1) err=0x%x", __func__, err);
            return err;
        }
        ESP_LOGV(TAG, "%s: keeping clock on during ACMD41", __func__);
    }

    sdmmc_command_t cmd = {
            .arg = ocr,
            .flags = SCF_CMD_BCR | SCF_RSP_R3,
            .opcode = SD_APP_OP_COND
    };
    int nretries = SDMMC_SEND_OP_COND_MAX_RETRIES;
    int err_cnt = SDMMC_SEND_OP_COND_MAX_ERRORS;
    for (; nretries != 0; --nretries)  {
        bzero(&cmd, sizeof cmd);
        cmd.arg = ocr;
        cmd.flags = SCF_CMD_BCR | SCF_RSP_R3;
        if (!card->is_mmc) { /* SD mode */
            cmd.opcode = SD_APP_OP_COND;
            err = sdmmc_send_app_cmd(card, &cmd);
        } else { /* MMC mode */
            cmd.arg &= ~MMC_OCR_ACCESS_MODE_MASK;
            cmd.arg |= MMC_OCR_SECTOR_MODE;
            cmd.opcode = MMC_SEND_OP_COND;
            err = sdmmc_send_cmd(card, &cmd);
        }

        if (err != ESP_OK) {
            if (--err_cnt == 0) {
                ESP_LOGD(TAG, "%s: sdmmc_send_app_cmd err=0x%x", __func__, err);
                goto done;
            } else {
                ESP_LOGV(TAG, "%s: ignoring err=0x%x", __func__, err);
                continue;
            }
        }
        // In SD protocol, card sets MEM_READY bit in OCR when it is ready.
        // In SPI protocol, card clears IDLE_STATE bit in R1 response.
        if (!host_is_spi(card)) {
            if ((MMC_R3(cmd.response) & MMC_OCR_MEM_READY) ||
                ocr == 0) {
                break;
            }
        } else {
            if ((SD_SPI_R1(cmd.response) & SD_SPI_R1_IDLE_STATE) == 0) {
                break;
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    if (nretries == 0) {
        err = ESP_ERR_TIMEOUT;
        goto done;
    }

    if (ocrp) {
        *ocrp = MMC_R3(cmd.response);
    }

    err = ESP_OK;
done:

    if (card->host.set_cclk_always_on != NULL) {
        esp_err_t err_cclk_dis = card->host.set_cclk_always_on(card->host.slot, false);
        if (err_cclk_dis != ESP_OK) {
            ESP_LOGE(TAG, "%s: set_cclk_always_on (2) err=0x%x", __func__, err);
            /* If we failed to disable clock, don't overwrite 'err' to return the original error */
        }
        ESP_LOGV(TAG, "%s: clock always-on mode disabled", __func__);
    }

    return err;
}

esp_err_t sdmmc_send_cmd_read_ocr(sdmmc_card_t *card, uint32_t *ocrp)
{
    assert(ocrp);
    sdmmc_command_t cmd = {
        .opcode = SD_READ_OCR,
        .flags = SCF_CMD_BCR | SCF_RSP_R2
    };
    esp_err_t err = sdmmc_send_cmd(card, &cmd);
    if (err != ESP_OK) {
        return err;
    }
    *ocrp = SD_SPI_R3(cmd.response);
    return ESP_OK;
}


esp_err_t sdmmc_send_cmd_all_send_cid(sdmmc_card_t* card, sdmmc_response_t* out_raw_cid)
{
    assert(out_raw_cid);
    sdmmc_command_t cmd = {
            .opcode = MMC_ALL_SEND_CID,
            .flags = SCF_CMD_BCR | SCF_RSP_R2
    };
    esp_err_t err = sdmmc_send_cmd(card, &cmd);
    if (err != ESP_OK) {
        return err;
    }
    memcpy(out_raw_cid, &cmd.response, sizeof(sdmmc_response_t));
    return ESP_OK;
}

esp_err_t sdmmc_send_cmd_send_cid(sdmmc_card_t *card, sdmmc_cid_t *out_cid)
{
    assert(out_cid);
    assert(host_is_spi(card) && "SEND_CID should only be used in SPI mode");
    assert(!card->is_mmc && "MMC cards are not supported in SPI mode");
    sdmmc_response_t buf;
    sdmmc_command_t cmd = {
        .opcode = MMC_SEND_CID,
        .flags = SCF_CMD_READ | SCF_CMD_ADTC,
        .arg = 0,
        .data = &buf[0],
        .datalen = sizeof(buf)
    };
    esp_err_t err = sdmmc_send_cmd(card, &cmd);
    if (err != ESP_OK) {
        return err;
    }
    sdmmc_flip_byte_order(buf, sizeof(buf));
    return sdmmc_decode_cid(buf, out_cid);
}


esp_err_t sdmmc_send_cmd_set_relative_addr(sdmmc_card_t* card, uint16_t* out_rca)
{
    assert(out_rca);
    sdmmc_command_t cmd = {
            .opcode = SD_SEND_RELATIVE_ADDR,
            .flags = SCF_CMD_BCR | SCF_RSP_R6
    };

    /* MMC cards expect us to set the RCA.
     * Set RCA to 1 since we don't support multiple cards on the same bus, for now.
     */
    uint16_t mmc_rca = 1;
    if (card->is_mmc) {
        cmd.arg = MMC_ARG_RCA(mmc_rca);
    }

    esp_err_t err = sdmmc_send_cmd(card, &cmd);
    if (err != ESP_OK) {
        return err;
    }
    *out_rca = (card->is_mmc) ? mmc_rca : SD_R6_RCA(cmd.response);
    return ESP_OK;
}

esp_err_t sdmmc_send_cmd_set_blocklen(sdmmc_card_t* card, sdmmc_csd_t* csd)
{
    sdmmc_command_t cmd = {
            .opcode = MMC_SET_BLOCKLEN,
            .arg = csd->sector_size,
            .flags = SCF_CMD_AC | SCF_RSP_R1
    };
    return sdmmc_send_cmd(card, &cmd);
}

esp_err_t sdmmc_send_cmd_send_csd(sdmmc_card_t* card, sdmmc_csd_t* out_csd)
{
    /* The trick with SEND_CSD is that in SPI mode, it acts as a data read
     * command, while in SD mode it is an AC command with R2 response.
     */
    sdmmc_response_t spi_buf;
    const bool is_spi = host_is_spi(card);
    sdmmc_command_t cmd = {
            .opcode = MMC_SEND_CSD,
            .arg = is_spi ? 0 : MMC_ARG_RCA(card->rca),
            .flags = is_spi ? (SCF_CMD_READ | SCF_CMD_ADTC | SCF_RSP_R1) :
                    (SCF_CMD_AC | SCF_RSP_R2),
            .data = is_spi ? &spi_buf[0] : 0,
            .datalen = is_spi ? sizeof(spi_buf) : 0,
    };
    esp_err_t err = sdmmc_send_cmd(card, &cmd);
    if (err != ESP_OK) {
        return err;
    }
    uint32_t* ptr = cmd.response;
    if (is_spi) {
        sdmmc_flip_byte_order(spi_buf,  sizeof(spi_buf));
        ptr = spi_buf;
    }
    if (card->is_mmc) {
        err = sdmmc_mmc_decode_csd(cmd.response, out_csd);
    } else {
        err = sdmmc_decode_csd(ptr, out_csd);
    }
    return err;
}

esp_err_t sdmmc_send_cmd_select_card(sdmmc_card_t* card, uint32_t rca)
{
    /* Don't expect to see a response when de-selecting a card */
    uint32_t response = (rca == 0) ? 0 : SCF_RSP_R1;
    sdmmc_command_t cmd = {
            .opcode = MMC_SELECT_CARD,
            .arg = MMC_ARG_RCA(rca),
            .flags = SCF_CMD_AC | response
    };
    return sdmmc_send_cmd(card, &cmd);
}

esp_err_t sdmmc_send_cmd_send_scr(sdmmc_card_t* card, sdmmc_scr_t *out_scr)
{
    size_t datalen = 8;
    esp_err_t err = ESP_FAIL;
    void *buf = NULL;
    size_t actual_size = 0;
    esp_dma_mem_info_t dma_mem_info;
    card->host.get_dma_info(card->host.slot, &dma_mem_info);
    err = esp_dma_capable_malloc(datalen, &dma_mem_info, &buf, &actual_size);
    if (err != ESP_OK) {
        return err;
    }

    sdmmc_command_t cmd = {
            .data = buf,
            .datalen = datalen,
            .buflen = actual_size,
            .blklen = datalen,
            .flags = SCF_CMD_ADTC | SCF_CMD_READ | SCF_RSP_R1,
            .opcode = SD_APP_SEND_SCR
    };
    err = sdmmc_send_app_cmd(card, &cmd);
    if (err == ESP_OK) {
        err = sdmmc_decode_scr(buf, out_scr);
    }
    free(buf);
    return err;
}

esp_err_t sdmmc_send_cmd_set_bus_width(sdmmc_card_t* card, int width)
{
    sdmmc_command_t cmd = {
            .opcode = SD_APP_SET_BUS_WIDTH,
            .flags = SCF_RSP_R1 | SCF_CMD_AC,
            .arg = (width == 4) ? SD_ARG_BUS_WIDTH_4 : SD_ARG_BUS_WIDTH_1,
    };

    return sdmmc_send_app_cmd(card, &cmd);
}

esp_err_t sdmmc_send_cmd_crc_on_off(sdmmc_card_t* card, bool crc_enable)
{
    assert(host_is_spi(card) && "CRC_ON_OFF can only be used in SPI mode");
    sdmmc_command_t cmd = {
            .opcode = SD_CRC_ON_OFF,
            .arg = crc_enable ? 1 : 0,
            .flags = SCF_CMD_AC | SCF_RSP_R1
    };
    return sdmmc_send_cmd(card, &cmd);
}

esp_err_t sdmmc_send_cmd_send_status(sdmmc_card_t* card, uint32_t* out_status)
{
    sdmmc_command_t cmd = {
            .opcode = MMC_SEND_STATUS,
            .arg = MMC_ARG_RCA(card->rca),
            .flags = SCF_CMD_AC | SCF_RSP_R1
    };
    esp_err_t err = sdmmc_send_cmd(card, &cmd);
    if (err != ESP_OK) {
        return err;
    }
    if (out_status) {
        if (host_is_spi(card)) {
            *out_status = SD_SPI_R2(cmd.response);
        } else {
            *out_status = MMC_R1(cmd.response);
        }
    }
    return ESP_OK;
}

esp_err_t sdmmc_write_sectors(sdmmc_card_t* card, const void* src,
        size_t start_block, size_t block_count)
{
    if (block_count == 0) {
        return ESP_OK;
    }

    esp_err_t err = ESP_OK;
    size_t block_size = card->csd.sector_size;
    esp_dma_mem_info_t dma_mem_info;
    card->host.get_dma_info(card->host.slot, &dma_mem_info);
#ifdef SOC_SDMMC_PSRAM_DMA_CAPABLE
    dma_mem_info.extra_heap_caps |= MALLOC_CAP_SPIRAM;
#endif
    if (esp_dma_is_buffer_alignment_satisfied(src, block_size * block_count, dma_mem_info)) {
        err = sdmmc_write_sectors_dma(card, src, start_block, block_count, block_size * block_count);
    } else {
        // SDMMC peripheral needs DMA-capable buffers. Split the write into
        // separate single block writes, if needed, and allocate a temporary
        // DMA-capable buffer.
        void *tmp_buf = NULL;
        size_t actual_size = 0;
        // Clear the SPIRAM flag. We don't want to force the allocation into SPIRAM, the allocator
        // will decide based on the buffer size and memory availability.
        dma_mem_info.extra_heap_caps &= ~MALLOC_CAP_SPIRAM;
        err = esp_dma_capable_malloc(block_size, &dma_mem_info, &tmp_buf, &actual_size);
        if (err != ESP_OK) {
            return err;
        }

        const uint8_t* cur_src = (const uint8_t*) src;
        for (size_t i = 0; i < block_count; ++i) {
            memcpy(tmp_buf, cur_src, block_size);
            cur_src += block_size;
            err = sdmmc_write_sectors_dma(card, tmp_buf, start_block + i, 1, actual_size);
            if (err != ESP_OK) {
                ESP_LOGD(TAG, "%s: error 0x%x writing block %d+%d",
                        __func__, err, start_block, i);
                break;
            }
        }
        free(tmp_buf);
    }
    return err;
}

esp_err_t sdmmc_write_sectors_dma(sdmmc_card_t* card, const void* src,
        size_t start_block, size_t block_count, size_t buffer_len)
{
    if (start_block + block_count > card->csd.capacity) {
        return ESP_ERR_INVALID_SIZE;
    }
    size_t block_size = card->csd.sector_size;
    sdmmc_command_t cmd = {
            .flags = SCF_CMD_ADTC | SCF_RSP_R1,
            .blklen = block_size,
            .data = (void*) src,
            .datalen = block_count * block_size,
            .buflen = buffer_len,
            .timeout_ms = SDMMC_WRITE_CMD_TIMEOUT_MS
    };
    if (block_count == 1) {
        cmd.opcode = MMC_WRITE_BLOCK_SINGLE;
    } else {
        cmd.opcode = MMC_WRITE_BLOCK_MULTIPLE;
    }
    if (card->ocr & SD_OCR_SDHC_CAP) {
        cmd.arg = start_block;
    } else {
        cmd.arg = start_block * block_size;
    }
    esp_err_t err = sdmmc_send_cmd(card, &cmd);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: sdmmc_send_cmd returned 0x%x", __func__, err);
        return err;
    }
    uint32_t status = 0;
    size_t count = 0;
    int64_t yield_delay_us = 100 * 1000; // initially 100ms
    int64_t t0 = esp_timer_get_time();
    int64_t t1 = 0;
    /* SD mode: wait for the card to become idle based on R1 status */
    while (!host_is_spi(card) && !(status & MMC_R1_READY_FOR_DATA)) {
        t1 = esp_timer_get_time();
        if (t1 - t0 > SDMMC_READY_FOR_DATA_TIMEOUT_US) {
            ESP_LOGE(TAG, "write sectors dma - timeout");
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
    /* SPI mode: although card busy indication is based on the busy token,
     * SD spec recommends that the host checks the results of programming by sending
     * SEND_STATUS command. Some of the conditions reported in SEND_STATUS are not
     * reported via a data error token.
     */
    if (host_is_spi(card)) {
        err = sdmmc_send_cmd_send_status(card, &status);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "%s: sdmmc_send_cmd_send_status returned 0x%x", __func__, err);
            return err;
        }
        if (status & SD_SPI_R2_CARD_LOCKED) {
            ESP_LOGE(TAG, "%s: write failed, card is locked: r2=0x%04" PRIx32,
                     __func__, status);
            return ESP_ERR_INVALID_STATE;
        }
        if (status != 0) {
            ESP_LOGE(TAG, "%s: card status indicates an error after write operation: r2=0x%04" PRIx32,
                     __func__, status);
            return ESP_ERR_INVALID_RESPONSE;
        }
    }
    return ESP_OK;
}

esp_err_t sdmmc_read_sectors(sdmmc_card_t* card, void* dst,
        size_t start_block, size_t block_count)
{
    if (block_count == 0) {
        return ESP_OK;
    }

    esp_err_t err = ESP_OK;
    size_t block_size = card->csd.sector_size;
    esp_dma_mem_info_t dma_mem_info;
    card->host.get_dma_info(card->host.slot, &dma_mem_info);
    if (esp_dma_is_buffer_alignment_satisfied(dst, block_size * block_count, dma_mem_info)) {
        err = sdmmc_read_sectors_dma(card, dst, start_block, block_count, block_size * block_count);
    } else {
        // SDMMC peripheral needs DMA-capable buffers. Split the read into
        // separate single block reads, if needed, and allocate a temporary
        // DMA-capable buffer.
        void *tmp_buf = NULL;
        size_t actual_size = 0;
        err = esp_dma_capable_malloc(block_size, &dma_mem_info, &tmp_buf, &actual_size);
        if (err != ESP_OK) {
            return err;
        }
        uint8_t* cur_dst = (uint8_t*) dst;
        for (size_t i = 0; i < block_count; ++i) {
            err = sdmmc_read_sectors_dma(card, tmp_buf, start_block + i, 1, actual_size);
            if (err != ESP_OK) {
                ESP_LOGD(TAG, "%s: error 0x%x writing block %d+%d",
                        __func__, err, start_block, i);
                break;
            }
            memcpy(cur_dst, tmp_buf, block_size);
            cur_dst += block_size;
        }
        free(tmp_buf);
    }
    return err;
}

esp_err_t sdmmc_read_sectors_dma(sdmmc_card_t* card, void* dst,
        size_t start_block, size_t block_count, size_t buffer_len)
{
    if (start_block + block_count > card->csd.capacity) {
        return ESP_ERR_INVALID_SIZE;
    }
    size_t block_size = card->csd.sector_size;
    sdmmc_command_t cmd = {
            .flags = SCF_CMD_ADTC | SCF_CMD_READ | SCF_RSP_R1,
            .blklen = block_size,
            .data = (void*) dst,
            .datalen = block_count * block_size,
            .buflen = buffer_len,
    };
    if (block_count == 1) {
        cmd.opcode = MMC_READ_BLOCK_SINGLE;
    } else {
        cmd.opcode = MMC_READ_BLOCK_MULTIPLE;
    }
    if (card->ocr & SD_OCR_SDHC_CAP) {
        cmd.arg = start_block;
    } else {
        cmd.arg = start_block * block_size;
    }
    esp_err_t err = sdmmc_send_cmd(card, &cmd);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: sdmmc_send_cmd returned 0x%x", __func__, err);
        return err;
    }
    uint32_t status = 0;
    size_t count = 0;
    int64_t yield_delay_us = 100 * 1000; // initially 100ms
    int64_t t0 = esp_timer_get_time();
    int64_t t1 = 0;
    /* SD mode: wait for the card to become idle based on R1 status */
    while (!host_is_spi(card) && !(status & MMC_R1_READY_FOR_DATA)) {
        t1 = esp_timer_get_time();
        if (t1 - t0 > SDMMC_READY_FOR_DATA_TIMEOUT_US) {
            ESP_LOGE(TAG, "read sectors dma - timeout");
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
            ESP_LOGV(TAG, "waiting for card to become ready (%d)", count);
        }
    }
    return ESP_OK;
}

esp_err_t sdmmc_erase_sectors(sdmmc_card_t* card, size_t start_sector,
        size_t sector_count, sdmmc_erase_arg_t arg)
{
    if (sector_count == 0) {
        return ESP_OK;
    }

    if (start_sector + sector_count > card->csd.capacity) {
        return ESP_ERR_INVALID_SIZE;
    }

    uint32_t cmd38_arg;
    if (arg == SDMMC_ERASE_ARG) {
        cmd38_arg = card->is_mmc ? SDMMC_MMC_TRIM_ARG : SDMMC_SD_ERASE_ARG;
    } else {
        cmd38_arg = card->is_mmc ? SDMMC_MMC_DISCARD_ARG : SDMMC_SD_DISCARD_ARG;
    }

    /* validate the CMD38 argument against card supported features */
    if (card->is_mmc) {
        if ((cmd38_arg == SDMMC_MMC_TRIM_ARG) && (sdmmc_can_trim(card) != ESP_OK)) {
            return ESP_ERR_NOT_SUPPORTED;
        }
        if ((cmd38_arg == SDMMC_MMC_DISCARD_ARG) && (sdmmc_can_discard(card) != ESP_OK)) {
            return ESP_ERR_NOT_SUPPORTED;
        }
    } else { // SD card
        if ((cmd38_arg == SDMMC_SD_DISCARD_ARG) && (sdmmc_can_discard(card) != ESP_OK)) {
            return ESP_ERR_NOT_SUPPORTED;
        }
    }

    /* default as block unit address */
    size_t addr_unit_mult = 1;

    if (!(card->ocr & SD_OCR_SDHC_CAP)) {
        addr_unit_mult = card->csd.sector_size;
    }

    /* prepare command to set the start address */
    sdmmc_command_t cmd = {
            .flags = SCF_CMD_AC | SCF_RSP_R1 | SCF_WAIT_BUSY,
            .opcode = card->is_mmc ? MMC_ERASE_GROUP_START :
                    SD_ERASE_GROUP_START,
            .arg = (start_sector * addr_unit_mult),
    };

    esp_err_t err = sdmmc_send_cmd(card, &cmd);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: sdmmc_send_cmd (ERASE_GROUP_START) returned 0x%x", __func__, err);
        return err;
    }

    /* prepare command to set the end address */
    cmd.opcode = card->is_mmc ? MMC_ERASE_GROUP_END : SD_ERASE_GROUP_END;
    cmd.arg = ((start_sector + (sector_count - 1)) * addr_unit_mult);

    err = sdmmc_send_cmd(card, &cmd);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: sdmmc_send_cmd (ERASE_GROUP_END) returned 0x%x", __func__, err);
        return err;
    }

    /* issue erase command */
    memset((void *)&cmd, 0 , sizeof(sdmmc_command_t));
    cmd.flags = SCF_CMD_AC | SCF_RSP_R1B | SCF_WAIT_BUSY;
    cmd.opcode = MMC_ERASE;
    cmd.arg = cmd38_arg;
    cmd.timeout_ms = sdmmc_get_erase_timeout_ms(card, cmd38_arg, sector_count * card->csd.sector_size / 1024);

    err = sdmmc_send_cmd(card, &cmd);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: sdmmc_send_cmd (ERASE) returned 0x%x", __func__, err);
        return err;
    }

    if (host_is_spi(card)) {
        uint32_t status;
        err = sdmmc_send_cmd_send_status(card, &status);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "%s: sdmmc_send_cmd_send_status returned 0x%x", __func__, err);
            return err;
        }
        if (status != 0) {
            ESP_LOGE(TAG, "%s: card status indicates an error after erase operation: r2=0x%04" PRIx32,
                     __func__, status);
            return ESP_ERR_INVALID_RESPONSE;
        }
    }

    return ESP_OK;
}

esp_err_t sdmmc_can_discard(sdmmc_card_t* card)
{
    if ((card->is_mmc) && (card->ext_csd.rev >= EXT_CSD_REV_1_6)) {
         return ESP_OK;
    }
    // SD card
    if ((!card->is_mmc) && !host_is_spi(card) && (card->ssr.discard_support == 1)) {
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t sdmmc_can_trim(sdmmc_card_t* card)
{
    if ((card->is_mmc) && (card->ext_csd.sec_feature & EXT_CSD_SEC_GB_CL_EN)) {
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t sdmmc_mmc_can_sanitize(sdmmc_card_t* card)
{
    if ((card->is_mmc) && (card->ext_csd.sec_feature & EXT_CSD_SEC_SANITIZE)) {
        return ESP_OK;
    }
    return ESP_FAIL;
}

esp_err_t sdmmc_mmc_sanitize(sdmmc_card_t* card, uint32_t timeout_ms)
{
    esp_err_t err;
    uint8_t index = EXT_CSD_SANITIZE_START;
    uint8_t set = EXT_CSD_CMD_SET_NORMAL;
    uint8_t value = 0x01;

    if (sdmmc_mmc_can_sanitize(card) != ESP_OK) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    /*
     * A Sanitize operation is initiated by writing a value to the extended
     * CSD[165] SANITIZE_START. While the device is performing the sanitize
     * operation, the busy line is asserted.
     * SWITCH command is used to write the EXT_CSD register.
     */
    sdmmc_command_t cmd = {
            .opcode = MMC_SWITCH,
            .arg = (MMC_SWITCH_MODE_WRITE_BYTE << 24) | (index << 16) | (value << 8) | set,
            .flags = SCF_RSP_R1B | SCF_CMD_AC | SCF_WAIT_BUSY,
            .timeout_ms = timeout_ms,
    };
    err = sdmmc_send_cmd(card, &cmd);
    if (err == ESP_OK) {
        //check response bit to see that switch was accepted
        if (MMC_R1(cmd.response) & MMC_R1_SWITCH_ERROR) {
            err = ESP_ERR_INVALID_RESPONSE;
        }
    }
    return err;
}

esp_err_t sdmmc_full_erase(sdmmc_card_t* card)
{
    sdmmc_erase_arg_t arg = SDMMC_SD_ERASE_ARG; // erase by default for SD card
    esp_err_t err;
    if (card->is_mmc) {
        arg = sdmmc_mmc_can_sanitize(card) == ESP_OK ? SDMMC_MMC_DISCARD_ARG: SDMMC_MMC_TRIM_ARG;
    }
    err = sdmmc_erase_sectors(card, 0, card->csd.capacity, arg);
    if ((err == ESP_OK) && (arg == SDMMC_MMC_DISCARD_ARG)) {
        uint32_t timeout_ms = sdmmc_get_erase_timeout_ms(card, SDMMC_MMC_DISCARD_ARG, card->csd.capacity * ((uint64_t) card->csd.sector_size) / 1024);
        return sdmmc_mmc_sanitize(card, timeout_ms);
    }
    return err;
}

esp_err_t sdmmc_get_status(sdmmc_card_t* card)
{
    uint32_t stat;
    return sdmmc_send_cmd_send_status(card, &stat);
}
