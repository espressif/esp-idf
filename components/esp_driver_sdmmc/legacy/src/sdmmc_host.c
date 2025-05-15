/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <sys/param.h>
#include "esp_log.h"
#include "esp_check.h"
#include "driver/sdmmc_host.h"
#include "driver/sd_host_sdmmc.h"
#include "driver/sd_host.h"
#include "sdmmc_internal.h"

#define SLOT_CHECK(slot_num) \
if (slot_num < 0 || slot_num >= SOC_SDMMC_NUM_SLOTS) { \
    return ESP_ERR_INVALID_ARG; \
}

#define SDMMC_EVENT_QUEUE_LENGTH    32

static const char *TAG = "sdmmc_periph";
static sd_host_ctlr_handle_t s_ctlr = NULL;
static sd_host_slot_handle_t s_slot0 = NULL;
static sd_host_slot_handle_t s_slot1 = NULL;

esp_err_t sdmmc_host_set_card_clk(int slot, uint32_t freq_khz)
{
    SLOT_CHECK(slot);

    sd_host_slot_handle_t hdl = sdmmc_get_slot_handle(slot);
    sd_host_slot_cfg_t cfg = {
        .freq_hz = freq_khz * 1000,
    };
    ESP_RETURN_ON_ERROR(sd_host_slot_configure(hdl, &cfg), TAG, "failed to configure slot freq");

    return ESP_OK;
}

esp_err_t sdmmc_host_get_real_freq(int slot, int *real_freq_khz)
{
    SLOT_CHECK(slot);

    sd_host_slot_handle_t hdl = sdmmc_get_slot_handle(slot);
    sd_host_sdmmc_slot_t *slot_ctx = __containerof(hdl, sd_host_sdmmc_slot_t, drv);
    ESP_RETURN_ON_ERROR(sd_host_slot_get_calc_real_freq(slot_ctx, real_freq_khz), TAG, "failed to get slot freq");

    return ESP_OK;
}

esp_err_t sdmmc_host_set_input_delay(int slot, sdmmc_delay_phase_t delay_phase)
{
    SLOT_CHECK(slot);
    sd_host_slot_handle_t hdl = sdmmc_get_slot_handle(slot);
    sd_host_slot_cfg_t cfg = {
        .delayphase = delay_phase,
    };
    ESP_RETURN_ON_ERROR(sd_host_slot_configure(hdl, &cfg), TAG, "failed to configure slot delay phase");

    return ESP_OK;
}

esp_err_t sdmmc_host_set_input_delayline(int slot, sdmmc_delay_line_t delay_line)
{
    SLOT_CHECK(slot);
    sd_host_slot_handle_t hdl = sdmmc_get_slot_handle(slot);
    sd_host_slot_cfg_t cfg = {
        .delayline = delay_line,
    };
    ESP_RETURN_ON_ERROR(sd_host_slot_configure(hdl, &cfg), TAG, "failed to configure slot delay line");

    return ESP_OK;
}

esp_err_t sdmmc_host_init(void)
{
    sd_host_sdmmc_cfg_t cfg = {
        .event_queue_items = SDMMC_EVENT_QUEUE_LENGTH,
    };
    esp_err_t ret = sd_host_create_sdmmc_controller(&cfg, &s_ctlr);
    ESP_RETURN_ON_ERROR(ret, TAG, "failed to create new SD controller");

    return ESP_OK;
}

sd_host_slot_handle_t sdmmc_get_slot_handle(int slot_id)
{
    return slot_id == 0 ? s_slot0 : s_slot1;
}

esp_err_t sdmmc_host_is_slot_set_to_uhs1(int slot, bool *is_uhs1)
{
    SLOT_CHECK(slot);
    sd_host_slot_handle_t hdl = sdmmc_get_slot_handle(slot);

    sd_host_slot_info_t info = {};
    ESP_RETURN_ON_ERROR(sd_host_slot_get_info(hdl, &info), TAG, "failed to get slot info");
    if (info.sd_mode == SD_MODE_UHS1) {
        *is_uhs1 = true;
    }

    return ESP_OK;
}

esp_err_t sdmmc_host_init_slot(int slot, const sdmmc_slot_config_t *slot_config)
{
    esp_err_t ret = ESP_FAIL;

    bool internal_pullup = (slot_config->flags & SDMMC_SLOT_FLAG_INTERNAL_PULLUP);
    bool wp_active_high = (slot_config->flags & SDMMC_SLOT_FLAG_WP_ACTIVE_HIGH);
    sd_host_slot_sdmmc_init_cfg_t cfg = {
        .slot_id = slot,
        .sd_mode = (slot_config->flags & SDMMC_SLOT_FLAG_UHS1) ? SD_MODE_UHS1 : SD_MODE_NORMAL,
        .io_config = {
            .width = slot_config->width,
            .clk_io = slot_config->clk,
            .cmd_io = slot_config->cmd,
            .cd_io = slot_config->cd,
            .wp_io = slot_config->wp,
            .d0_io = slot_config->d0,
            .d1_io = slot_config->d1,
            .d2_io = slot_config->d2,
            .d3_io = slot_config->d3,
            .d4_io = slot_config->d4,
            .d5_io = slot_config->d5,
            .d6_io = slot_config->d6,
            .d7_io = slot_config->d7,
        },
        .slot_flags.internal_pullup = internal_pullup,
        .slot_flags.wp_active_high = wp_active_high,
    };
    if (slot == 0) {
        ret = sd_host_sdmmc_controller_add_slot(s_ctlr, &cfg, &s_slot0);
    } else {
        ret = sd_host_sdmmc_controller_add_slot(s_ctlr, &cfg, &s_slot1);
    }
    ESP_RETURN_ON_ERROR(ret, TAG, "failed to add new SD slot");

    return ESP_OK;
}

esp_err_t sdmmc_host_deinit_slot(int slot)
{
    esp_err_t ret = ESP_FAIL;
    sd_host_slot_handle_t hdl = sdmmc_get_slot_handle(slot);
    ESP_RETURN_ON_ERROR(sd_host_remove_slot(hdl), TAG, "failed to remove slot");

    ret = sd_host_del_controller(s_ctlr);
    //for backward compatibility, return ESP_OK when only slot is removed and host is still there
    if (ret == ESP_ERR_INVALID_STATE) {
        ret = ESP_OK;
    }

    return ret;
}

esp_err_t sdmmc_host_deinit(void)
{
    esp_err_t ret = ESP_FAIL;
    sd_host_slot_handle_t hdl[2] = {s_slot0, s_slot1};
    for (int i = 0; i < 2; i++) {
        if (hdl[i]) {
            ret = sd_host_remove_slot(hdl[i]);
            ESP_RETURN_ON_ERROR(ret, TAG, "failed to remove slot%d", i);
        }
    }
    ESP_RETURN_ON_ERROR(sd_host_del_controller(s_ctlr), TAG, "failed to delete controller");

    return ESP_OK;
}

esp_err_t sdmmc_host_set_bus_width(int slot, size_t width)
{
    SLOT_CHECK(slot);

    sd_host_slot_handle_t hdl = sdmmc_get_slot_handle(slot);
    sd_host_slot_cfg_t cfg = {
        .width = width,
    };
    ESP_RETURN_ON_ERROR(sd_host_slot_configure(hdl, &cfg), TAG, "failed to configure slot bus width");

    return ESP_OK;
}

size_t sdmmc_host_get_slot_width(int slot)
{
    SLOT_CHECK(slot);

    sd_host_slot_handle_t hdl = sdmmc_get_slot_handle(slot);
    sd_host_slot_info_t info = {};
    esp_err_t ret = sd_host_slot_get_info(hdl, &info);
    assert(ret == ESP_OK);

    return info.width;
}

esp_err_t sdmmc_host_set_bus_ddr_mode(int slot, bool ddr_enabled)
{
    sd_host_slot_handle_t hdl = sdmmc_get_slot_handle(slot);
    sd_host_slot_cfg_t cfg = {
        .sampling_mode = ddr_enabled ? SD_SAMPLING_MODE_DDR : SD_SAMPLING_MODE_SDR,
    };
    ESP_RETURN_ON_ERROR(sd_host_slot_configure(hdl, &cfg), TAG, "failed to configure slot ddr mode");

    return ESP_OK;
}

esp_err_t sdmmc_host_set_cclk_always_on(int slot, bool cclk_always_on)
{
    SLOT_CHECK(slot);

    sd_host_slot_handle_t hdl = sdmmc_get_slot_handle(slot);
    ESP_RETURN_ON_ERROR(sd_host_slot_set_cclk_always_on(hdl, cclk_always_on), TAG, "failed to configure slot cclk always on");

    return ESP_OK;
}

esp_err_t sdmmc_host_io_int_enable(int slot)
{
    sd_host_slot_handle_t hdl = sdmmc_get_slot_handle(slot);
    return sd_host_slot_enable_io_int(hdl);
}

esp_err_t sdmmc_host_io_int_wait(int slot, TickType_t timeout_ticks)
{
    assert(slot == 0 || slot == 1);
    sd_host_slot_handle_t hdl = sdmmc_get_slot_handle(slot);
    ESP_RETURN_ON_ERROR(sd_host_slot_wait_io_int(hdl, timeout_ticks), TAG, "failed to wait io interrupt");

    return ESP_OK;
}

esp_err_t sdmmc_host_get_dma_info(int slot, esp_dma_mem_info_t *dma_mem_info)
{
    SLOT_CHECK(slot);

    if (dma_mem_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    dma_mem_info->extra_heap_caps = MALLOC_CAP_DMA;
    dma_mem_info->dma_alignment_bytes = 4;
    return ESP_OK;
}

bool sdmmc_host_check_buffer_alignment(int slot, const void *buf, size_t size)
{
    assert(slot == 0 || slot == 1);
    sd_host_slot_handle_t hdl = sdmmc_get_slot_handle(slot);
    sd_host_sdmmc_slot_t *slot_ctx = __containerof(hdl, sd_host_sdmmc_slot_t, drv);

    return sd_host_check_buffer_alignment(slot_ctx, buf, size);
}

esp_err_t sdmmc_host_get_state(sdmmc_host_state_t* state)
{
    ESP_RETURN_ON_FALSE(state, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    if (s_ctlr) {
        state->host_initialized = true;
        sd_host_sdmmc_ctlr_t *ctlr_ctx = __containerof(s_ctlr, sd_host_sdmmc_ctlr_t, drv);
        state->num_of_init_slots = ctlr_ctx->registered_slot_nums;
    }

    return ESP_OK;
}
