/*
 * Copyright (c) 2006 Uwe Stuehler <uwe@openbsd.org>
 * Adaptations to ESP-IDF Copyright (c) 2016 Espressif Systems (Shanghai) PTE LTD
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

#include <string.h>
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/sdmmc_defs.h"
#include "driver/sdmmc_types.h"
#include "sdmmc_cmd.h"

#define MIN(a,b) (((a)<(b))?(a):(b))

static const char* TAG = "sdmmc_cmd";

static esp_err_t sdmmc_send_cmd(sdmmc_card_t* card, sdmmc_command_t* cmd);
static esp_err_t sdmmc_send_app_cmd(sdmmc_card_t* card, sdmmc_command_t* cmd);
static esp_err_t sdmmc_send_cmd_go_idle_state(sdmmc_card_t* card);
static esp_err_t sdmmc_send_cmd_send_if_cond(sdmmc_card_t* card, uint32_t ocr);
static esp_err_t sdmmc_send_cmd_send_op_cond(sdmmc_card_t* card, uint32_t ocr, uint32_t *ocrp);
static esp_err_t sdmmc_decode_cid(sdmmc_response_t resp, sdmmc_cid_t* out_cid);
static esp_err_t sddmc_send_cmd_all_send_cid(sdmmc_card_t* card, sdmmc_cid_t* out_cid);
static esp_err_t sdmmc_send_cmd_set_relative_addr(sdmmc_card_t* card, uint16_t* out_rca);
static esp_err_t sdmmc_send_cmd_set_blocklen(sdmmc_card_t* card, sdmmc_csd_t* csd);
static esp_err_t sdmmc_decode_csd(sdmmc_response_t response, sdmmc_csd_t* out_csd);
static esp_err_t sdmmc_send_cmd_send_csd(sdmmc_card_t* card, sdmmc_csd_t* out_csd);
static esp_err_t sdmmc_send_cmd_select_card(sdmmc_card_t* card);
static esp_err_t sdmmc_decode_scr(uint32_t *raw_scr, sdmmc_scr_t* out_scr);
static esp_err_t sdmmc_send_cmd_send_scr(sdmmc_card_t* card, sdmmc_scr_t *out_scr);
static esp_err_t sdmmc_send_cmd_set_bus_width(sdmmc_card_t* card, int width);
static esp_err_t sdmmc_send_cmd_stop_transmission(sdmmc_card_t* card, uint32_t* status);
static esp_err_t sdmmc_send_cmd_send_status(sdmmc_card_t* card, uint32_t* out_status);
static uint32_t  get_host_ocr(float voltage);


esp_err_t sdmmc_card_init(const sdmmc_host_t* config,
        sdmmc_card_t* card)
{
    ESP_LOGD(TAG, "%s", __func__);
    memset(card, 0, sizeof(*card));
    memcpy(&card->host, config, sizeof(*config));
    esp_err_t err = sdmmc_send_cmd_go_idle_state(card);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: go_idle_state (1) returned 0x%x", __func__, err);
        return err;
    }
    ets_delay_us(10000);
    uint32_t host_ocr = get_host_ocr(config->io_voltage);
    err = sdmmc_send_cmd_send_if_cond(card, host_ocr);
    if (err == ESP_OK) {
        ESP_LOGD(TAG, "SDHC/SDXC card");
        host_ocr |= SD_OCR_SDHC_CAP;
    } else if (err == ESP_ERR_TIMEOUT) {
        ESP_LOGD(TAG, "CMD8 timeout; not an SDHC/SDXC card");
    } else {
        ESP_LOGE(TAG, "%s: send_if_cond (1) returned 0x%x", __func__, err);
        return err;
    }
    err = sdmmc_send_cmd_send_op_cond(card, host_ocr, &card->ocr);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: send_op_cond (1) returned 0x%x", __func__, err);
        return err;
    }
    host_ocr &= card->ocr;
    ESP_LOGD(TAG, "sdmmc_card_init: host_ocr=%08x, card_ocr=%08x", host_ocr, card->ocr);
    err = sddmc_send_cmd_all_send_cid(card, &card->cid);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: all_send_cid returned 0x%x", __func__, err);
        return err;
    }
    err = sdmmc_send_cmd_set_relative_addr(card, &card->rca);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: set_relative_addr returned 0x%x", __func__, err);
        return err;
    }
    err = sdmmc_send_cmd_send_csd(card, &card->csd);
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
    err = sdmmc_send_cmd_select_card(card);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: select_card returned 0x%x", __func__, err);
        return err;
    }
    if ((card->ocr & SD_OCR_SDHC_CAP) == 0) {
        err = sdmmc_send_cmd_set_blocklen(card, &card->csd);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "%s: set_blocklen returned 0x%x", __func__, err);
            return err;
        }
    }
    err = sdmmc_send_cmd_send_scr(card, &card->scr);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: send_scr returned 0x%x", __func__, err);
        return err;
    }
    if ((config->flags & SDMMC_HOST_FLAG_4BIT) &&
        (card->scr.bus_width & SCR_SD_BUS_WIDTHS_4BIT)) {
        ESP_LOGD(TAG, "switching to 4-bit bus mode");
        err = sdmmc_send_cmd_set_bus_width(card, 4);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "set_bus_width failed");
            return err;
        }
        err = (*config->set_bus_width)(config->slot, 4);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "slot->set_bus_width failed");
            return err;
        }
        uint32_t status;
        err = sdmmc_send_cmd_stop_transmission(card, &status);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "stop_transmission failed (0x%x)", err);
            return err;
        }
    }
    uint32_t status = 0;
    while (!(status & MMC_R1_READY_FOR_DATA)) {
        // TODO: add some timeout here
        uint32_t count = 0;
        err = sdmmc_send_cmd_send_status(card, &status);
        if (err != ESP_OK) {
            return err;
        }
        if (++count % 10 == 0) {
            ESP_LOGV(TAG, "waiting for card to become ready (%d)", count);
        }
    }
    if (config->max_freq_khz >= SDMMC_FREQ_HIGHSPEED &&
        card->csd.tr_speed / 1000 >= SDMMC_FREQ_HIGHSPEED) {
        ESP_LOGD(TAG, "switching to HS bus mode");
        err = (*config->set_card_clk)(config->slot, SDMMC_FREQ_HIGHSPEED);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "failed to switch peripheral to HS bus mode");
            return err;
        }
    } else if (config->max_freq_khz >= SDMMC_FREQ_DEFAULT &&
        card->csd.tr_speed / 1000 >= SDMMC_FREQ_DEFAULT) {
        ESP_LOGD(TAG, "switching to DS bus mode");
        err = (*config->set_card_clk)(config->slot, SDMMC_FREQ_DEFAULT);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "failed to switch peripheral to HS bus mode");
            return err;
        }
    }
    sdmmc_scr_t scr_tmp;
    err = sdmmc_send_cmd_send_scr(card, &scr_tmp);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: send_scr returned 0x%x", __func__, err);
        return err;
    }
    if (memcmp(&card->scr, &scr_tmp, sizeof(scr_tmp)) != 0) {
        ESP_LOGE(TAG, "data check fail!");
        return ESP_ERR_INVALID_RESPONSE;
    }
    return ESP_OK;
}

void sdmmc_card_print_info(FILE* stream, const sdmmc_card_t* card)
{
    fprintf(stream, "Name: %s\n", card->cid.name);
    fprintf(stream, "Type: %s\n", (card->ocr & SD_OCR_SDHC_CAP)?"SDHC/SDXC":"SDSC");
    fprintf(stream, "Speed: %s\n", (card->csd.tr_speed > 25000000)?"high speed":"default speed");
    fprintf(stream, "Size: %lluMB\n", ((uint64_t) card->csd.capacity) * card->csd.sector_size / (1024 * 1024));
    fprintf(stream, "CSD: ver=%d, sector_size=%d, capacity=%d read_bl_len=%d\n",
            card->csd.csd_ver,
            card->csd.sector_size, card->csd.capacity, card->csd.read_block_len);
    fprintf(stream, "SCR: sd_spec=%d, bus_width=%d\n", card->scr.sd_spec, card->scr.bus_width);
}

static esp_err_t sdmmc_send_cmd(sdmmc_card_t* card, sdmmc_command_t* cmd)
{
    int slot = card->host.slot;
    ESP_LOGV(TAG, "sending cmd slot=%d op=%d arg=%x flags=%x data=%p blklen=%d datalen=%d",
            slot, cmd->opcode, cmd->arg, cmd->flags, cmd->data, cmd->blklen, cmd->datalen);
    esp_err_t err = (*card->host.do_transaction)(slot, cmd);
    if (err != 0) {
        ESP_LOGD(TAG, "sdmmc_req_run returned 0x%x", err);
        return err;
    }
    int state = MMC_R1_CURRENT_STATE(cmd->response);
    ESP_LOGV(TAG, "cmd response %08x %08x %08x %08x err=0x%x state=%d",
               cmd->response[0],
               cmd->response[1],
               cmd->response[2],
               cmd->response[3],
               cmd->error,
               state);
    return cmd->error;
}

static esp_err_t sdmmc_send_app_cmd(sdmmc_card_t* card, sdmmc_command_t* cmd)
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
    if (!(MMC_R1(app_cmd.response) & MMC_R1_APP_CMD)) {
        ESP_LOGW(TAG, "card doesn't support APP_CMD");
        return ESP_ERR_NOT_SUPPORTED;
    }
    return sdmmc_send_cmd(card, cmd);
}


static esp_err_t sdmmc_send_cmd_go_idle_state(sdmmc_card_t* card)
{
    sdmmc_command_t cmd = {
        .opcode = MMC_GO_IDLE_STATE,
        .flags = SCF_CMD_BC | SCF_RSP_R0,
    };
    return sdmmc_send_cmd(card, &cmd);
}


static esp_err_t sdmmc_send_cmd_send_if_cond(sdmmc_card_t* card, uint32_t ocr)
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
        return ESP_ERR_INVALID_RESPONSE;
    }
    return ESP_OK;
}

static esp_err_t sdmmc_send_cmd_send_op_cond(sdmmc_card_t* card, uint32_t ocr, uint32_t *ocrp)
{
    sdmmc_command_t cmd = {
            .arg = ocr,
            .flags = SCF_CMD_BCR | SCF_RSP_R3,
            .opcode = SD_APP_OP_COND
    };
    int nretries = 100;   // arbitrary, BSD driver uses this value
    for (; nretries != 0; --nretries)  {
        esp_err_t err = sdmmc_send_app_cmd(card, &cmd);
        if (err != ESP_OK) {
            return err;
        }
        if ((MMC_R3(cmd.response) & MMC_OCR_MEM_READY) ||
             ocr == 0) {
            break;
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    if (nretries == 0) {
        return ESP_ERR_TIMEOUT;
    }
    if (ocrp) {
        *ocrp = MMC_R3(cmd.response);
    }
    return ESP_OK;
}

static esp_err_t sdmmc_decode_cid(sdmmc_response_t resp, sdmmc_cid_t* out_cid)
{
    out_cid->mfg_id = SD_CID_MID(resp);
    out_cid->oem_id = SD_CID_OID(resp);
    SD_CID_PNM_CPY(resp, out_cid->name);
    out_cid->revision = SD_CID_REV(resp);
    out_cid->serial = SD_CID_PSN(resp);
    out_cid->date = SD_CID_MDT(resp);
    return ESP_OK;
}

static esp_err_t sddmc_send_cmd_all_send_cid(sdmmc_card_t* card, sdmmc_cid_t* out_cid)
{
    assert(out_cid);
    sdmmc_command_t cmd = {
            .opcode = MMC_ALL_SEND_CID,
            .flags = SCF_CMD_BCR | SCF_RSP_R2
    };
    esp_err_t err = sdmmc_send_cmd(card, &cmd);
    if (err != ESP_OK) {
        return err;
    }
    return sdmmc_decode_cid(cmd.response, out_cid);
}


static esp_err_t sdmmc_send_cmd_set_relative_addr(sdmmc_card_t* card, uint16_t* out_rca)
{
    assert(out_rca);
    sdmmc_command_t cmd = {
            .opcode = SD_SEND_RELATIVE_ADDR,
            .flags = SCF_CMD_BCR | SCF_RSP_R6
    };

    esp_err_t err = sdmmc_send_cmd(card, &cmd);
    if (err != ESP_OK) {
        return err;
    }
    *out_rca = SD_R6_RCA(cmd.response);
    return ESP_OK;
}


static esp_err_t sdmmc_send_cmd_set_blocklen(sdmmc_card_t* card, sdmmc_csd_t* csd)
{
    sdmmc_command_t cmd = {
            .opcode = MMC_SET_BLOCKLEN,
            .arg = csd->sector_size,
            .flags = SCF_CMD_AC | SCF_RSP_R1
    };
    return sdmmc_send_cmd(card, &cmd);
}

static esp_err_t sdmmc_decode_csd(sdmmc_response_t response, sdmmc_csd_t* out_csd)
{
    out_csd->csd_ver = SD_CSD_CSDVER(response);
    switch (out_csd->csd_ver) {
    case SD_CSD_CSDVER_2_0:
        out_csd->capacity = SD_CSD_V2_CAPACITY(response);
        out_csd->read_block_len = SD_CSD_V2_BL_LEN;
        break;
    case SD_CSD_CSDVER_1_0:
        out_csd->capacity = SD_CSD_CAPACITY(response);
        out_csd->read_block_len = SD_CSD_READ_BL_LEN(response);
        break;
    default:
        ESP_LOGE(TAG, "unknown SD CSD structure version 0x%x", out_csd->csd_ver);
        return ESP_ERR_NOT_SUPPORTED;
    }
    out_csd->card_command_class = SD_CSD_CCC(response);
    int read_bl_size = 1 << out_csd->read_block_len;
    out_csd->sector_size = MIN(read_bl_size, 512);
    if (out_csd->sector_size < read_bl_size) {
        out_csd->capacity *= read_bl_size / out_csd->sector_size;
    }
    int speed = SD_CSD_SPEED(response);
    if (speed == SD_CSD_SPEED_50_MHZ) {
        out_csd->tr_speed = 50000000;
    } else {
        out_csd->tr_speed = 25000000;
    }
    return ESP_OK;
}

static esp_err_t sdmmc_send_cmd_send_csd(sdmmc_card_t* card, sdmmc_csd_t* out_csd)
{
    sdmmc_command_t cmd = {
            .opcode = MMC_SEND_CSD,
            .arg = MMC_ARG_RCA(card->rca),
            .flags = SCF_CMD_AC | SCF_RSP_R2
    };
    esp_err_t err = sdmmc_send_cmd(card, &cmd);
    if (err != ESP_OK) {
        return err;
    }
    return sdmmc_decode_csd(cmd.response, out_csd);
}

static esp_err_t sdmmc_send_cmd_select_card(sdmmc_card_t* card)
{
    sdmmc_command_t cmd = {
            .opcode = MMC_SELECT_CARD,
            .arg = MMC_ARG_RCA(card->rca),
            .flags = SCF_CMD_AC | SCF_RSP_R1
    };
    return sdmmc_send_cmd(card, &cmd);
}

static esp_err_t sdmmc_decode_scr(uint32_t *raw_scr, sdmmc_scr_t* out_scr)
{
    sdmmc_response_t resp = {0xabababab, 0xabababab, 0x12345678, 0x09abcdef};
    resp[2] = __builtin_bswap32(raw_scr[0]);
    resp[3] = __builtin_bswap32(raw_scr[1]);
    int ver = SCR_STRUCTURE(resp);
    if (ver != 0) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    out_scr->sd_spec = SCR_SD_SPEC(resp);
    out_scr->bus_width = SCR_SD_BUS_WIDTHS(resp);
    return ESP_OK;
}

static esp_err_t sdmmc_send_cmd_send_scr(sdmmc_card_t* card, sdmmc_scr_t *out_scr)
{
    size_t datalen = 8;
    uint32_t* buf = (uint32_t*) heap_caps_malloc(datalen, MALLOC_CAP_DMA);
    if (buf == NULL) {
        return ESP_ERR_NO_MEM;
    }
    sdmmc_command_t cmd = {
            .data = buf,
            .datalen = datalen,
            .blklen = datalen,
            .flags = SCF_CMD_ADTC | SCF_CMD_READ | SCF_RSP_R1,
            .opcode = SD_APP_SEND_SCR
    };
    esp_err_t err = sdmmc_send_app_cmd(card, &cmd);
    if (err == ESP_OK) {
        buf[0] = (buf[0]);
        buf[1] = (buf[1]);
        err = sdmmc_decode_scr(buf, out_scr);
    }
    free(buf);
    return err;
}

static esp_err_t sdmmc_send_cmd_set_bus_width(sdmmc_card_t* card, int width)
{
    sdmmc_command_t cmd = {
            .opcode = SD_APP_SET_BUS_WIDTH,
            .flags = SCF_RSP_R1 | SCF_CMD_AC,
            .arg = (width == 4) ? SD_ARG_BUS_WIDTH_4 : SD_ARG_BUS_WIDTH_1
    };

    return sdmmc_send_app_cmd(card, &cmd);
}

static esp_err_t sdmmc_send_cmd_stop_transmission(sdmmc_card_t* card, uint32_t* status)
{
    sdmmc_command_t cmd = {
            .opcode = MMC_STOP_TRANSMISSION,
            .arg = 0,
            .flags = SCF_RSP_R1B | SCF_CMD_AC
    };
    esp_err_t err = sdmmc_send_cmd(card, &cmd);
    if (err == 0) {
        *status = MMC_R1(cmd.response);
    }
    return err;
}

static uint32_t get_host_ocr(float voltage)
{
    // TODO: report exact voltage to the card
    // For now tell that the host has 2.8-3.6V voltage range
    (void) voltage;
    return SD_OCR_VOL_MASK;
}

static esp_err_t sdmmc_send_cmd_send_status(sdmmc_card_t* card, uint32_t* out_status)
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
        *out_status = MMC_R1(cmd.response);
    }
    return ESP_OK;
}

esp_err_t sdmmc_write_sectors(sdmmc_card_t* card, const void* src,
        size_t start_block, size_t block_count)
{
    if (start_block + block_count > card->csd.capacity) {
        return ESP_ERR_INVALID_SIZE;
    }
    size_t block_size = card->csd.sector_size;
    sdmmc_command_t cmd = {
            .flags = SCF_CMD_ADTC | SCF_RSP_R1,
            .blklen = block_size,
            .data = (void*) src,
            .datalen = block_count * block_size
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
    while (!(status & MMC_R1_READY_FOR_DATA)) {
        // TODO: add some timeout here
        err = sdmmc_send_cmd_send_status(card, &status);
        if (err != ESP_OK) {
            return err;
        }
        if (++count % 10 == 0) {
            ESP_LOGV(TAG, "waiting for card to become ready (%d)", count);
        }
    }
    return ESP_OK;
}

esp_err_t sdmmc_read_sectors(sdmmc_card_t* card, void* dst,
        size_t start_block, size_t block_count)
{
    if (start_block + block_count > card->csd.capacity) {
        return ESP_ERR_INVALID_SIZE;
    }
    size_t block_size = card->csd.sector_size;
    sdmmc_command_t cmd = {
            .flags = SCF_CMD_ADTC | SCF_CMD_READ | SCF_RSP_R1,
            .blklen = block_size,
            .data = (void*) dst,
            .datalen = block_count * block_size
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
    while (!(status & MMC_R1_READY_FOR_DATA)) {
        // TODO: add some timeout here
        err = sdmmc_send_cmd_send_status(card, &status);
        if (err != ESP_OK) {
            return err;
        }
        if (++count % 10 == 0) {
            ESP_LOGV(TAG, "waiting for card to become ready (%d)", count);
        }
    }
    return ESP_OK;
}
