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
#include "esp_check.h"
#include "esp_timer.h"
#include "esp_cache.h"
#include "esp_private/sdmmc_common.h"
#include "freertos/FreeRTOS.h"

#define SDMMC_DELAY_NUMS_MAX 10

static const char* TAG = "sdmmc_sd";

esp_err_t sdmmc_init_sd_if_cond(sdmmc_card_t* card)
{
    /* SEND_IF_COND (CMD8) command is used to identify SDHC/SDXC cards.
     * SD v1 and non-SD cards will not respond to this command.
     */
    uint32_t host_ocr = get_host_ocr(card->host.io_voltage);
    esp_err_t err = sdmmc_send_cmd_send_if_cond(card, host_ocr);
    if (err == ESP_OK) {
        ESP_LOGD(TAG, "SDHC/SDXC card");
        host_ocr |= SD_OCR_SDHC_CAP;
    } else if (err == ESP_ERR_TIMEOUT) {
        ESP_LOGD(TAG, "CMD8 timeout; not an SD v2.00 card");
    } else if (host_is_spi(card) && err == ESP_ERR_NOT_SUPPORTED) {
        ESP_LOGD(TAG, "CMD8 rejected; not an SD v2.00 card");
    } else {
        ESP_LOGE(TAG, "%s: send_if_cond (1) returned 0x%x", __func__, err);
        return err;
    }
    card->ocr = host_ocr;
    return ESP_OK;
}

esp_err_t sdmmc_init_sd_blocklen(sdmmc_card_t* card)
{
    /* SDSC cards support configurable data block lengths.
     * We don't use this feature and set the block length to 512 bytes,
     * same as the block length for SDHC cards.
     */
    if ((card->ocr & SD_OCR_SDHC_CAP) == 0) {
        esp_err_t err = sdmmc_send_cmd_set_blocklen(card, &card->csd);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "%s: set_blocklen returned 0x%x", __func__, err);
            return err;
        }
    }
    return ESP_OK;
}

esp_err_t sdmmc_init_sd_scr(sdmmc_card_t* card)
{
    esp_err_t err;
    /* Get the contents of SCR register: bus width and the version of SD spec
     * supported by the card.
     * In SD mode, this is the first command which uses D0 line. Errors at
     * this step usually indicate connection issue or lack of pull-up resistor.
     */
    err = sdmmc_send_cmd_send_scr(card, &card->scr);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: send_scr (1) returned 0x%x", __func__, err);
        return err;
    }

    if ((card->scr.bus_width & SCR_SD_BUS_WIDTHS_4BIT)
            && (card->host.flags & SDMMC_HOST_FLAG_4BIT)) {
        card->log_bus_width = 2;
    } else {
        card->log_bus_width = 0;
    }
    return ESP_OK;
}

esp_err_t sdmmc_init_sd_ssr(sdmmc_card_t* card)
{
    esp_err_t err = ESP_OK;
    /* Get the contents of SSR register: SD additional information
     * ACMD13 to read 512byte SD status information
     */
    uint32_t* sd_ssr = NULL;
    size_t actual_size = 0;

    sd_ssr = heap_caps_calloc(1, SD_SSR_SIZE, MALLOC_CAP_DMA);
    if (!sd_ssr) {
        ESP_LOGE(TAG, "%s: not enough mem, err=0x%x", __func__, ESP_ERR_NO_MEM);
        return ESP_ERR_NO_MEM;
    }
    actual_size = heap_caps_get_allocated_size(sd_ssr);

    sdmmc_command_t cmd = {
        .data = sd_ssr,
        .datalen = SD_SSR_SIZE,
        .buflen = actual_size,
        .blklen = SD_SSR_SIZE,
        .opcode = SD_APP_SD_STATUS,
        .arg = 0,
        .flags = SCF_CMD_ADTC | SCF_RSP_R1 | SCF_CMD_READ
    };

    // read SD status register
    err = sdmmc_send_app_cmd(card, &cmd);
    if (err != ESP_OK) {
        free(sd_ssr);
        ESP_LOGE(TAG, "%s: sdmmc_send_cmd returned 0x%x", __func__, err);
        return err;
    }

    err = sdmmc_decode_ssr(sd_ssr, &card->ssr);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: error sdmmc_decode_ssr returned 0x%x", __func__, err);
    }
    free(sd_ssr);
    return err;
}

esp_err_t sdmmc_init_sd_bus_width(sdmmc_card_t* card)
{
    int width = 1;
    if (card->log_bus_width == 2) {
        width = 4;
    } else if (card->log_bus_width == 3) {
        width = 8;
    }
    esp_err_t err = sdmmc_send_cmd_set_bus_width(card, width);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "set_bus_width failed (0x%x)", err);
        return err;
    }
    return ESP_OK;
}

esp_err_t sdmmc_init_sd_wait_data_ready(sdmmc_card_t* card)
{
    /* Wait for the card to be ready for data transfers */
    uint32_t status = 0;
    uint32_t count = 0;
    int64_t yield_delay_us = 100 * 1000; // initially 100ms
    int64_t t0 = esp_timer_get_time();
    int64_t t1 = 0;
    while (!host_is_spi(card) && !(status & MMC_R1_READY_FOR_DATA)) {
        t1 = esp_timer_get_time();
        if (t1 - t0 > SDMMC_INIT_WAIT_DATA_READY_TIMEOUT_US) {
            ESP_LOGE(TAG, "init wait data ready - timeout");
            return ESP_ERR_TIMEOUT;
        }
        if (t1 - t0 > yield_delay_us) {
            yield_delay_us *= 2;
            vTaskDelay(1);
        }
        esp_err_t err = sdmmc_send_cmd_send_status(card, &status);
        if (err != ESP_OK) {
            return err;
        }
        if (++count % 16 == 0) {
            ESP_LOGV(TAG, "waiting for card to become ready (%" PRIu32 ")", count);
        }
    }
    return ESP_OK;
}

esp_err_t sdmmc_send_cmd_switch_func(sdmmc_card_t* card,
        uint32_t mode, uint32_t group, uint32_t function,
        sdmmc_switch_func_rsp_t* resp)
{
    if (card->scr.sd_spec < SCR_SD_SPEC_VER_1_10 ||
        ((card->csd.card_command_class & SD_CSD_CCC_SWITCH) == 0)) {
            return ESP_ERR_NOT_SUPPORTED;
    }

    if (group == 0 ||
        group > SD_SFUNC_GROUP_MAX ||
        function > SD_SFUNC_FUNC_MAX) {
        return ESP_ERR_INVALID_ARG;
    }

    if (mode > 1) {
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t group_shift = (group - 1) << 2;
    /* all functions which should not be affected are set to 0xf (no change) */
    uint32_t other_func_mask = (0x00ffffff & ~(0xf << group_shift));
    uint32_t func_val = (function << group_shift) | other_func_mask;

    size_t datalen = sizeof(sdmmc_switch_func_rsp_t);
    sdmmc_command_t cmd = {
            .opcode = MMC_SWITCH,
            .flags = SCF_CMD_ADTC | SCF_CMD_READ | SCF_RSP_R1,
            .blklen = sizeof(sdmmc_switch_func_rsp_t),
            .data = resp->data,
            .datalen = datalen,
            .buflen = datalen,
            .arg = (!!mode << 31) | func_val
    };

    esp_err_t err = sdmmc_send_cmd(card, &cmd);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: sdmmc_send_cmd returned 0x%x", __func__, err);
        return err;
    }
    sdmmc_flip_byte_order(resp->data, sizeof(sdmmc_switch_func_rsp_t));
    uint32_t resp_ver = SD_SFUNC_VER(resp->data);
    if (resp_ver == 0) {
        /* busy response is never sent */
    } else if (resp_ver == 1) {
        if (SD_SFUNC_BUSY(resp->data, group) & (1 << function)) {
            ESP_LOGD(TAG, "%s: response indicates function %" PRIu32 ":%" PRIu32 " is busy",
                    __func__, group, function);
            return ESP_ERR_INVALID_STATE;
        }
    } else {
        ESP_LOGD(TAG, "%s: got an invalid version of SWITCH_FUNC response: 0x%02" PRIx32,
                __func__, resp_ver);
        return ESP_ERR_INVALID_RESPONSE;
    }
    return ESP_OK;
}

esp_err_t sdmmc_enter_higher_speed_mode(sdmmc_card_t* card)
{
    /* This will determine if the card supports SWITCH_FUNC command,
     * and high speed mode. If the cards supports both, this will enable
     * high speed mode at the card side.
     */
    if (card->scr.sd_spec < SCR_SD_SPEC_VER_1_10 ||
        ((card->csd.card_command_class & SD_CSD_CCC_SWITCH) == 0)) {
            return ESP_ERR_NOT_SUPPORTED;
    }

    sdmmc_switch_func_rsp_t *response = NULL;
    esp_err_t err = ESP_FAIL;
    response = heap_caps_malloc(sizeof(*response), MALLOC_CAP_DMA);
    if (!response) {
        ESP_LOGE(TAG, "%s: not enough mem, err=0x%x", __func__, ESP_ERR_NO_MEM);
        return ESP_ERR_NO_MEM;
    }

    err = sdmmc_send_cmd_switch_func(card, 0, SD_ACCESS_MODE, 0, response);
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "%s: sdmmc_send_cmd_switch_func (1) returned 0x%x", __func__, err);
        goto out;
    }
    uint32_t supported_mask = SD_SFUNC_SUPPORTED(response->data, 1);
    ESP_LOGV(TAG, "%s: access mode supported_mask: 0x%"PRIx32, __func__, supported_mask);

    if (((card->host.flags & SDMMC_HOST_FLAG_DDR) != 0) && (card->is_uhs1 == 1)) {
        //UHS-I DDR50
        ESP_LOGV(TAG, "%s: to switch to DDR50", __func__);
        if ((supported_mask & BIT(SD_ACCESS_MODE_DDR50)) == 0) {
            err = ESP_ERR_NOT_SUPPORTED;
            goto out;
        }
        err = sdmmc_send_cmd_switch_func(card, 1, SD_ACCESS_MODE, SD_ACCESS_MODE_DDR50, response);
        if (err != ESP_OK) {
            ESP_LOGD(TAG, "%s: sdmmc_send_cmd_switch_func (2) returned 0x%x", __func__, err);
            goto out;
        }

        card->is_ddr = 1;
        err = (*card->host.set_bus_ddr_mode)(card->host.slot, true);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "%s: failed to switch bus to DDR mode (0x%x)", __func__, err);
            return err;
        }
    } else if (card->host.max_freq_khz == SDMMC_FREQ_SDR104) {
        //UHS-I SDR104
        ESP_LOGV(TAG, "%s: to switch to SDR104", __func__);
        if ((supported_mask & BIT(SD_ACCESS_MODE_SDR104)) == 0) {
            err = ESP_ERR_NOT_SUPPORTED;
            goto out;
        }
        err = sdmmc_send_cmd_switch_func(card, 1, SD_ACCESS_MODE, SD_ACCESS_MODE_SDR104, response);
        if (err != ESP_OK) {
            ESP_LOGD(TAG, "%s: sdmmc_send_cmd_switch_func (2) returned 0x%x", __func__, err);
            goto out;
        }
    } else if (card->host.max_freq_khz == SDMMC_FREQ_SDR50) {
        //UHS-I SDR50
        ESP_LOGV(TAG, "%s: to switch to SDR50", __func__);
        if ((supported_mask & BIT(SD_ACCESS_MODE_SDR50)) == 0) {
            err = ESP_ERR_NOT_SUPPORTED;
            goto out;
        }
        err = sdmmc_send_cmd_switch_func(card, 1, SD_ACCESS_MODE, SD_ACCESS_MODE_SDR50, response);
        if (err != ESP_OK) {
            ESP_LOGD(TAG, "%s: sdmmc_send_cmd_switch_func (2) returned 0x%x", __func__, err);
            goto out;
        }
    } else {
        ESP_LOGV(TAG, "%s: to switch to SDR25", __func__);
        if ((supported_mask & BIT(SD_ACCESS_MODE_SDR25)) == 0) {
            err = ESP_ERR_NOT_SUPPORTED;
            goto out;
        }
        err = sdmmc_send_cmd_switch_func(card, 1, SD_ACCESS_MODE, SD_ACCESS_MODE_SDR25, response);
        if (err != ESP_OK) {
            ESP_LOGD(TAG, "%s: sdmmc_send_cmd_switch_func (2) returned 0x%x", __func__, err);
            goto out;
        }
    }

out:
    free(response);
    return err;
}

static const uint8_t s_tuning_block_pattern[] = {
	0xff, 0x0f, 0xff, 0x00, 0xff, 0xcc, 0xc3, 0xcc,
	0xc3, 0x3c, 0xcc, 0xff, 0xfe, 0xff, 0xfe, 0xef,
	0xff, 0xdf, 0xff, 0xdd, 0xff, 0xfb, 0xff, 0xfb,
	0xbf, 0xff, 0x7f, 0xff, 0x77, 0xf7, 0xbd, 0xef,
	0xff, 0xf0, 0xff, 0xf0, 0x0f, 0xfc, 0xcc, 0x3c,
	0xcc, 0x33, 0xcc, 0xcf, 0xff, 0xef, 0xff, 0xee,
	0xff, 0xfd, 0xff, 0xfd, 0xdf, 0xff, 0xbf, 0xff,
	0xbb, 0xff, 0xf7, 0xff, 0xf7, 0x7f, 0x7b, 0xde,
};

/**
 * Find consecutive successful sampling points.
 * e.g. array: {1, 1, 0, 0, 1, 1, 1, 0}
 * out_length: 3
 * out_end_index: 6
 */
static void find_max_consecutive_success_points(int *array, size_t size, size_t *out_length, uint32_t *out_end_index)
{
    uint32_t max = 0;
    uint32_t match_num = 0;
    uint32_t i = 0;
    uint32_t end = 0;

    while (i < size) {
        if (array[i] == 1) {
            match_num++;
        } else {
            if (match_num > max) {
                max = match_num;
                end = i - 1;
            }
            match_num = 0;
        }
        i++;
    }

    /**
     * this is to deal with the case when the last points are consecutive 1, e.g.
     * {1, 0, 0, 1, 1, 1, 1, 1, 1}
     */
    if (match_num > max) {
        max = match_num;
        end = i - 1;
    }

    *out_length = max;
    *out_end_index = end;
}

static esp_err_t read_tuning_block(sdmmc_card_t *card)
{
    esp_err_t ret = ESP_FAIL;
    size_t tuning_block_size = sizeof(s_tuning_block_pattern);
    ESP_LOGV(TAG, "tuning_block_size: %zu", tuning_block_size);
    uint8_t *databuf = NULL;
    databuf = heap_caps_calloc(1, tuning_block_size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    ESP_RETURN_ON_FALSE(databuf, ESP_ERR_NO_MEM, TAG, "no mem for tuning block databuf");

    sdmmc_command_t cmd = {
        .opcode = MMC_SEND_TUNING_BLOCK,
        .flags = SCF_CMD_ADTC | SCF_CMD_READ | SCF_RSP_R1,
        .blklen = tuning_block_size,
        .data = (void *) databuf,
        .datalen = 1 * tuning_block_size,
        .buflen = tuning_block_size,
    };

    ret = sdmmc_send_cmd(card, &cmd);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "%s: sdmmc_send_cmd returned 0x%x", __func__, ret);
        return ret;
    }

    uint32_t status = 0;
    size_t count = 0;
    int64_t yield_delay_us = 100 * 1000; // initially 100ms
    int64_t t0 = esp_timer_get_time();
    int64_t t1 = 0;
    while (!host_is_spi(card) && !(status & MMC_R1_READY_FOR_DATA)) {
        t1 = esp_timer_get_time();
        if (t1 - t0 > SDMMC_READY_FOR_DATA_TIMEOUT_US) {
            ESP_LOGW(TAG, "read sectors dma - timeout");
            return ESP_ERR_TIMEOUT;
        }
        if (t1 - t0 > yield_delay_us) {
            yield_delay_us *= 2;
            vTaskDelay(1);
        }
        ret = sdmmc_send_cmd_send_status(card, &status);
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "%s: sdmmc_send_cmd_send_status returned 0x%x", __func__, ret);
            return ret;
        }
        if (++count % 16 == 0) {
            ESP_LOGV(TAG, "waiting for card to become ready (%d)", count);
        }
    }

    bool success = false;
    if (memcmp(s_tuning_block_pattern, databuf, tuning_block_size) == 0) {
        success = true;
    }

    return success ? ESP_OK : ESP_FAIL;
}

esp_err_t sdmmc_do_timing_tuning(sdmmc_card_t *card, sdmmc_delay_mode_t delay_mode)
{
    esp_err_t ret = ESP_FAIL;

    ESP_RETURN_ON_FALSE(!host_is_spi(card), ESP_ERR_NOT_SUPPORTED, TAG, "sdspi not supported timing tuning");
    if (delay_mode == SDMMC_DELAY_MODE_PHASE) {
        ESP_RETURN_ON_FALSE(card->host.set_input_delay, ESP_ERR_NOT_SUPPORTED, TAG, "phase delay feature isn't supported");
    } else {
        ESP_RETURN_ON_FALSE(card->host.set_input_delayline, ESP_ERR_NOT_SUPPORTED, TAG, "line delay feature isn't supported");
    }

    int results[SDMMC_DELAY_NUMS_MAX] = {};
    int start_delay_item = (delay_mode == SDMMC_DELAY_MODE_PHASE) ? SDMMC_DELAY_PHASE_0 : SDMMC_DELAY_LINE_0;
    int slot = card->host.slot;
    int delay_total_nums = 4;
    if (delay_mode == SDMMC_DELAY_MODE_PHASE) {
        if (card->host.max_freq_khz == SDMMC_FREQ_SDR104) {
            delay_total_nums = SDMMC_DELAY_PHASE_AUTO;
        }
    } else {
        delay_total_nums = SDMMC_DELAY_LINE_AUTO;
    }
    for (int i = start_delay_item; i < delay_total_nums; i++) {
        if (delay_mode == SDMMC_DELAY_MODE_PHASE) {
            ESP_RETURN_ON_ERROR((*card->host.set_input_delay)(slot, i), TAG, "failed to set delay phase");
        } else {
            ESP_RETURN_ON_ERROR((*card->host.set_input_delayline)(slot, i), TAG, "failed to set delay line");
        }
        ret = read_tuning_block(card);
        if (ret == ESP_OK) {
            results[i] += 1;
        }
    }

    for (int i = 0; i < delay_total_nums; i++) {
        ESP_LOGV(TAG, "results[%d]: %d", i, results[i]);
    }

    size_t consecutive_len = 0;
    uint32_t end = 0;
    find_max_consecutive_success_points(results, delay_total_nums, &consecutive_len, &end);

    int proper_delay_id = SDMMC_DELAY_PHASE_AUTO;
    if (consecutive_len == 1) {
        proper_delay_id = end;
    } else if (consecutive_len <= SDMMC_DELAY_PHASE_AUTO) {
        proper_delay_id = end - (consecutive_len / 2);
    } else {
        assert(false && "exceeds max tuning point");
    }
    ESP_LOGI(TAG, "%s: proper delay phase/line id: %d", __func__, proper_delay_id);

    if (proper_delay_id != SDMMC_DELAY_PHASE_AUTO) {
        if (delay_mode == SDMMC_DELAY_MODE_PHASE) {
            ESP_RETURN_ON_ERROR((*card->host.set_input_delay)(slot, proper_delay_id), TAG, "failed to set delay phase");
        } else {
            ESP_RETURN_ON_ERROR((*card->host.set_input_delayline)(slot, proper_delay_id), TAG, "failed to set delay line");
        }
    }

    return ESP_OK;
}

esp_err_t sdmmc_select_driver_strength(sdmmc_card_t *card, sdmmc_driver_strength_t driver_strength)
{
    if (card->scr.sd_spec < SCR_SD_SPEC_VER_1_10 ||
        ((card->csd.card_command_class & SD_CSD_CCC_SWITCH) == 0)) {
            return ESP_ERR_NOT_SUPPORTED;
    }

    esp_err_t ret = ESP_FAIL;
    sdmmc_switch_func_rsp_t *response = NULL;
    response = heap_caps_calloc(1, sizeof(*response), MALLOC_CAP_DMA);
    ESP_RETURN_ON_FALSE(response, ESP_ERR_NO_MEM, TAG, "no mem for response buf");

    ret = sdmmc_send_cmd_switch_func(card, 1, SD_DRIVER_STRENGTH, driver_strength, response);
    ESP_GOTO_ON_ERROR(ret, out, TAG, "%s: sdmmc_send_cmd_switch_func (1) returned 0x%x", __func__, ret);

    uint32_t supported_mask = SD_SFUNC_SELECTED(response->data, SD_DRIVER_STRENGTH);
    ESP_GOTO_ON_FALSE(supported_mask != 0xf, ESP_ERR_NOT_SUPPORTED, out, TAG, "switch group1 result fail");
    ESP_LOGV(TAG, "driver strength: supported_mask: 0x%"PRIx32, supported_mask);
    ESP_GOTO_ON_FALSE(supported_mask == driver_strength, ESP_ERR_INVALID_ARG, out, TAG, "fail to switch to type 0x%x", driver_strength);

out:
    free(response);
    return ret;
}

esp_err_t sdmmc_enable_hs_mode_and_check(sdmmc_card_t* card)
{
    /* All cards should support at least default speed */
    card->max_freq_khz = SDMMC_FREQ_DEFAULT;
    if (card->host.max_freq_khz <= card->max_freq_khz) {
        /* Host is configured to use low frequency, don't attempt to switch */
        card->max_freq_khz = card->host.max_freq_khz;
        return ESP_OK;
    }

    /* Try to enabled HS mode */
    esp_err_t err = sdmmc_enter_higher_speed_mode(card);
    if (err != ESP_OK) {
        return err;
    }

    /* HS mode has been enabled on the card.
     * Read CSD again, it should now indicate that the card supports
     * 50MHz clock.
     * Since SEND_CSD is allowed only in standby mode, and the card is currently in data transfer
     * mode, deselect the card first, then get the CSD, then select the card again. This step is
     * not required in SPI mode, since CMD7 (select_card) is not supported.
     */
    const bool is_spi = host_is_spi(card);
    if (!is_spi) {
        err = sdmmc_send_cmd_select_card(card, 0);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "%s: select_card (1) returned 0x%x", __func__, err);
            return err;
        }
    }
    err = sdmmc_send_cmd_send_csd(card, &card->csd);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: send_csd returned 0x%x", __func__, err);
        return err;
    }
    if (!is_spi) {
        err = sdmmc_send_cmd_select_card(card, card->rca);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "%s: select_card (2) returned 0x%x", __func__, err);
            return err;
        }
    }

    ESP_LOGD(TAG, "%s: after enabling HS mode, tr_speed=%d", __func__, card->csd.tr_speed);
    card->max_freq_khz = MIN(card->host.max_freq_khz, SDMMC_FREQ_SDR104);

    return ESP_OK;
}

static esp_err_t sdmmc_init_sd_uhs1_volt_sw_cb(void* arg, int voltage_mv)
{
    sdmmc_card_t* card = (sdmmc_card_t*)arg;
    ESP_LOGV(TAG, "%s: Voltage switch callback (%umv)", __func__, voltage_mv);
    return sd_pwr_ctrl_set_io_voltage(card->host.pwr_ctrl_handle, voltage_mv);
}

esp_err_t sdmmc_init_sd_uhs1(sdmmc_card_t* card)
{
    sdmmc_command_t cmd = {
            .opcode = SD_SWITCH_VOLTAGE,
            .arg = 0,
            .flags = SCF_CMD_AC | SCF_RSP_R1,
            .volt_switch_cb = &sdmmc_init_sd_uhs1_volt_sw_cb,
            .volt_switch_cb_arg = card
    };
    esp_err_t err = sdmmc_send_cmd(card, &cmd);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: send_cmd returned 0x%x", __func__, err);
    }

    card->is_uhs1 = 1;

    return err;
}

esp_err_t sdmmc_select_current_limit(sdmmc_card_t *card, sdmmc_current_limit_t current_limit)
{
    if (card->scr.sd_spec < SCR_SD_SPEC_VER_1_10 ||
        ((card->csd.card_command_class & SD_CSD_CCC_SWITCH) == 0)) {
            return ESP_ERR_NOT_SUPPORTED;
    }

    esp_err_t ret = ESP_FAIL;
    sdmmc_switch_func_rsp_t *response = NULL;
    response = heap_caps_calloc(1, sizeof(*response), MALLOC_CAP_DMA);
    ESP_RETURN_ON_FALSE(response, ESP_ERR_NO_MEM, TAG, "no mem for response buf");

    ret = sdmmc_send_cmd_switch_func(card, 1, SD_CURRENT_LIMIT, current_limit, response);
    ESP_GOTO_ON_ERROR(ret, out, TAG, "%s: sdmmc_send_cmd_switch_func (1) returned 0x%x", __func__, ret);

    uint32_t supported_mask = SD_SFUNC_SELECTED(response->data, SD_CURRENT_LIMIT);
    ESP_GOTO_ON_FALSE(supported_mask != 0xf, ESP_ERR_NOT_SUPPORTED, out, TAG, "switch group4 result fail");
    ESP_LOGV(TAG, "current limit: supported_mask: 0x%"PRIx32, supported_mask);
    ESP_GOTO_ON_FALSE(supported_mask == current_limit, ESP_ERR_INVALID_ARG, out, TAG, "fail to switch to type 0x%x", current_limit);

out:
    free(response);
    return ret;
}

esp_err_t sdmmc_check_scr(sdmmc_card_t* card)
{
    /* If frequency switch has been performed, read SCR register one more time
     * and compare the result with the previous one. Use this simple check as
     * an indicator of potential signal integrity issues.
     */
    sdmmc_scr_t scr_tmp = { 0 };
    esp_err_t err = sdmmc_send_cmd_send_scr(card, &scr_tmp);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: send_scr returned 0x%x", __func__, err);
        return err;
    }
    if (memcmp(&card->scr, &scr_tmp, sizeof(scr_tmp)) != 0) {
        ESP_LOGE(TAG, "got corrupted data after increasing clock frequency");
        return ESP_ERR_INVALID_RESPONSE;
    }
    return ESP_OK;
}

esp_err_t sdmmc_init_spi_crc(sdmmc_card_t* card)
{
    /* In SD mode, CRC checks of data transfers are mandatory and performed
     * by the hardware. In SPI mode, CRC16 of data transfers is optional and
     * needs to be enabled.
     */
    assert(host_is_spi(card));
    esp_err_t err = sdmmc_send_cmd_crc_on_off(card, true);
    if (err == ESP_ERR_NOT_SUPPORTED) { // Some cards fail to enable CRC on the first try, trying again
       ESP_LOGD(TAG, "%s: enabling CRC failed with 0x%x, trying again", __func__, err);
       vTaskDelay(SDMMC_INIT_SPI_CRC_RETRY_DELAY_MS / portTICK_PERIOD_MS);
       err = sdmmc_send_cmd_crc_on_off(card, true);
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "%s: sdmmc_send_cmd_crc_on_off returned 0x%x", __func__, err);
        return err;
    }
    return ESP_OK;
}

esp_err_t sdmmc_decode_cid(sdmmc_response_t resp, sdmmc_cid_t* out_cid)
{
    out_cid->mfg_id = SD_CID_MID(resp);
    out_cid->oem_id = SD_CID_OID(resp);
    SD_CID_PNM_CPY(resp, out_cid->name);
    out_cid->revision = SD_CID_REV(resp);
    out_cid->serial = SD_CID_PSN(resp);
    out_cid->date = SD_CID_MDT(resp);
    return ESP_OK;
}

esp_err_t sdmmc_decode_csd(sdmmc_response_t response, sdmmc_csd_t* out_csd)
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
    ESP_LOGV(TAG, "%s: speed: 0x%x", __func__, speed);
    switch (speed) {
    case SD_CSD_SPEED_50_MHZ:
        out_csd->tr_speed = 50000000;
        break;
    case SD_CSD_SPEED_100_MHZ:
        out_csd->tr_speed = 100000000;
        break;
    case SD_CSD_SPEED_200_MHZ:
        out_csd->tr_speed = 200000000;
        break;
    default:
        out_csd->tr_speed = 25000000;
        break;
    }

    return ESP_OK;
}

esp_err_t sdmmc_decode_scr(uint32_t *raw_scr, sdmmc_scr_t* out_scr)
{
    sdmmc_response_t resp = { 0 };
    resp[1] = __builtin_bswap32(raw_scr[0]);
    resp[0] = __builtin_bswap32(raw_scr[1]);
    int ver = SCR_STRUCTURE(resp);
    if (ver != 0) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    out_scr->sd_spec = SCR_SD_SPEC(resp);
    out_scr->erase_mem_state = SCR_DATA_STAT_AFTER_ERASE(resp);
    out_scr->bus_width = SCR_SD_BUS_WIDTHS(resp);
    return ESP_OK;
}

static const uint32_t s_au_to_size_kb[] = {
    0, 16, 32, 64,
    128, 256, 512, 1024,
    2 * 1024, 4 * 1024,
    8 * 1024, 12 * 1024,
    16 * 1024, 24 * 1024,
    32 * 1024, 64 * 1024
};
_Static_assert(sizeof(s_au_to_size_kb)/sizeof(s_au_to_size_kb[0]) == 16, "invalid number of elements in s_au_to_size_kb");

esp_err_t sdmmc_decode_ssr(uint32_t *raw_ssr, sdmmc_ssr_t* out_ssr)
{
    uint32_t ssr[(SD_SSR_SIZE/sizeof(uint32_t))] = { 0 };
    size_t j = (SD_SSR_SIZE/sizeof(uint32_t) - 1);

    for(size_t i = 0; i < (SD_SSR_SIZE/sizeof(uint32_t)); i++) {
        ssr[j - i] = __builtin_bswap32(raw_ssr[i]);
    }

    out_ssr->cur_bus_width = SSR_DAT_BUS_WIDTH(ssr);
    out_ssr->discard_support = SSR_DISCARD_SUPPORT(ssr);
    out_ssr->fule_support = SSR_FULE_SUPPORT(ssr);
    uint32_t au = SSR_AU_SIZE(ssr);
    out_ssr->alloc_unit_kb = s_au_to_size_kb[au];
    out_ssr->erase_timeout = SSR_ERASE_TIMEOUT(ssr);
    out_ssr->erase_size_au = SSR_ERASE_SIZE(ssr);
    out_ssr->erase_offset = SSR_ERASE_OFFSET(ssr);

    return ESP_OK;
}

uint32_t sdmmc_sd_get_erase_timeout_ms(const sdmmc_card_t* card, int arg, size_t erase_size_kb)
{
    if (arg == SDMMC_SD_DISCARD_ARG) {
        return SDMMC_SD_DISCARD_TIMEOUT;
    } else if (arg == SDMMC_SD_ERASE_ARG) {
        if (card->ssr.alloc_unit_kb != 0 &&
                card->ssr.erase_size_au != 0 &&
                card->ssr.erase_timeout != 0 &&
                card->ssr.erase_offset != 0) {
            /* Card supports erase timeout estimation. See the erase timeout equation in SD spec. */
            uint32_t timeout_sec = card->ssr.erase_offset +
                card->ssr.erase_timeout * (erase_size_kb + card->ssr.alloc_unit_kb - 1) /
                    (card->ssr.erase_size_au * card->ssr.alloc_unit_kb);
            ESP_LOGD(TAG, "%s: erase timeout %" PRIu32 " s (erasing %" PRIu32 " kB, ES=%" PRIu32 ", ET=%" PRIu32 ", EO=%" PRIu32 ", AU=%" PRIu32 " kB)",
                     __func__, timeout_sec, (uint32_t) erase_size_kb, (uint32_t) card->ssr.erase_size_au,
                     (uint32_t) card->ssr.erase_timeout, (uint32_t) card->ssr.erase_offset, (uint32_t) card->ssr.alloc_unit_kb);
            return timeout_sec * 1000;
        } else {
            uint32_t timeout_ms = SDMMC_SD_DISCARD_TIMEOUT * erase_size_kb / card->csd.sector_size;
            timeout_ms = MAX(1000, timeout_ms);
            ESP_LOGD(TAG, "%s: erase timeout %" PRIu32 " s (erasing %" PRIu32 " kB, %" PRIu32 " ms per sector)",
                     __func__, (uint32_t) (timeout_ms / 1000), (uint32_t) erase_size_kb, (uint32_t) SDMMC_SD_DISCARD_TIMEOUT);
            return timeout_ms;
        }
    } else {
        assert(false && "unexpected SD erase argument");
        return 0;
    }
}
