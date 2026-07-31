/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdatomic.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/isp_core.h"
#include "driver/isp_dpc.h"
#include "esp_private/isp_private.h"
#include "hal/efuse_hal.h"
#include "soc/chip_revision.h"

/*---------------------------------------------------------------
                    DPC (Dead Pixel Correction)
---------------------------------------------------------------*/

static const char *TAG = "ISP_DPC";

#define DPC_MAX_DEAD_PIXELS ESP_ISP_DPC_MAX_DEAD_PIXELS
#define DPC_INVALID_DEAD_PIXEL_COORD UINT32_MAX
#define DPC_HW_COORD_X_BITS 11
#define DPC_HW_COORD_X_MASK ((1U << DPC_HW_COORD_X_BITS) - 1)

static int compare_pixel_coords(const void *a, const void *b)
{
    const esp_isp_dpc_pixel_coord_t *coord_a = a;
    const esp_isp_dpc_pixel_coord_t *coord_b = b;
    if (coord_a->y < coord_b->y || (coord_a->y == coord_b->y && coord_a->x < coord_b->x)) {
        return -1;
    }
    if (coord_a->y > coord_b->y || (coord_a->y == coord_b->y && coord_a->x > coord_b->x)) {
        return 1;
    }
    return 0;
}

static esp_err_t s_dpc_update_shadow(isp_proc_handle_t isp_proc, bool update_once)
{
    bool valid = isp_ll_shadow_update_dpc(isp_proc->hal.hw, update_once);
    ESP_RETURN_ON_FALSE(valid, ESP_ERR_INVALID_STATE, TAG, "shadow update failed");
    return ESP_OK;
}

static uint32_t s_dpc_coord_to_hw(esp_isp_dpc_pixel_coord_t coord)
{
    return ((uint32_t)coord.y << DPC_HW_COORD_X_BITS) | coord.x;
}

static esp_isp_dpc_pixel_coord_t s_dpc_coord_from_hw(uint32_t coord)
{
    return (esp_isp_dpc_pixel_coord_t) {
        .x = coord & DPC_HW_COORD_X_MASK,
        .y = coord >> DPC_HW_COORD_X_BITS,
    };
}

static void s_dpc_lut_write(isp_proc_handle_t isp_proc, uint32_t index, esp_isp_dpc_pixel_coord_t coord)
{
    isp_ll_lut_dpc_set_wdata(isp_proc->hal.hw, s_dpc_coord_to_hw(coord));
    isp_ll_lut_dpc_set_cmd(isp_proc->hal.hw, true, index);
}

static void s_dpc_lut_clear_range(isp_proc_handle_t isp_proc, uint32_t start_index)
{
    for (uint32_t i = start_index; i < DPC_MAX_DEAD_PIXELS; i++) {
        isp_ll_lut_dpc_set_wdata(isp_proc->hal.hw, DPC_INVALID_DEAD_PIXEL_COORD);
        isp_ll_lut_dpc_set_cmd(isp_proc->hal.hw, true, i);
    }
}

static bool s_dpc_take_check_done_event(isp_proc_handle_t isp_proc)
{
    uint32_t events = isp_ll_get_intr_raw(isp_proc->hal.hw);
    if (events & ISP_LL_EVENT_DPC_CHECK_DONE) {
        isp_ll_clear_intr(isp_proc->hal.hw, ISP_LL_EVENT_DPC_CHECK_DONE);
        return true;
    }
    return false;
}

static esp_err_t s_dpc_configure_dynamic(isp_proc_handle_t isp_proc, const esp_isp_dpc_dynamic_config_t *config)
{
    if (config->method == ESP_ISP_DPC_DYNAMIC_METHOD_1) {
        isp_ll_dpc_set_dynamic_correction_method(isp_proc->hal.hw, ISP_LL_DPC_DYNAMIC_CORRECTION_METHOD_SIMPLE);
        isp_ll_dpc_set_high_thresh(isp_proc->hal.hw, config->method_1.high_threshold);
        isp_ll_dpc_set_low_thresh(isp_proc->hal.hw, config->method_1.low_threshold);
    } else if (config->method == ESP_ISP_DPC_DYNAMIC_METHOD_2) {
        uint32_t upper_ratio_raw = config->method_2.first_stage_upper_ratio.val;
        uint32_t lower_ratio_raw = config->method_2.first_stage_lower_ratio.val;
        uint32_t bright_factor_raw = config->method_2.bright_deviation_factor.val;
        uint32_t dark_factor_raw = config->method_2.dark_deviation_factor.val;
        ESP_RETURN_ON_FALSE(upper_ratio_raw <= ISP_DPC_RATIO_MAX,
                            ESP_ERR_INVALID_ARG, TAG, "first-stage upper ratio out of range");
        ESP_RETURN_ON_FALSE(lower_ratio_raw <= ISP_DPC_RATIO_MAX,
                            ESP_ERR_INVALID_ARG, TAG, "first-stage lower ratio out of range");
        ESP_RETURN_ON_FALSE(bright_factor_raw <= ISP_DPC_DEVIATION_FACTOR_MAX,
                            ESP_ERR_INVALID_ARG, TAG, "bright deviation factor out of range");
        ESP_RETURN_ON_FALSE(dark_factor_raw <= ISP_DPC_DEVIATION_FACTOR_MAX,
                            ESP_ERR_INVALID_ARG, TAG, "dark deviation factor out of range");
        ESP_RETURN_ON_FALSE(upper_ratio_raw > lower_ratio_raw,
                            ESP_ERR_INVALID_ARG, TAG, "first-stage upper ratio must exceed lower ratio");

        isp_ll_dpc_set_dynamic_correction_method(isp_proc->hal.hw, ISP_LL_DPC_DYNAMIC_CORRECTION_METHOD_HARD);
        isp_ll_dpc_set_high_thresh(isp_proc->hal.hw, upper_ratio_raw);
        isp_ll_dpc_set_low_thresh(isp_proc->hal.hw, lower_ratio_raw);
        isp_ll_dpc_set_dynamic_correction_method_1_bright_factor(isp_proc->hal.hw, bright_factor_raw);
        isp_ll_dpc_set_dynamic_correction_method_1_dark_factor(isp_proc->hal.hw, dark_factor_raw);
    } else {
        ESP_RETURN_ON_FALSE(false, ESP_ERR_INVALID_ARG, TAG, "invalid dynamic correction method");
    }
    return ESP_OK;
}

esp_err_t esp_isp_dpc_static_calibration_start_once(isp_proc_handle_t isp_proc,
                                                    esp_isp_dpc_calibration_image_t image_type,
                                                    const esp_isp_dpc_calibration_config_t *config)
{
#if CONFIG_IDF_TARGET_ESP32P4
    unsigned chip_version = efuse_hal_chip_revision();
    if (!ESP_CHIP_REV_ABOVE(chip_version, 300)) {
        ESP_RETURN_ON_FALSE(false, ESP_ERR_NOT_SUPPORTED, TAG, "DPC is not supported on ESP32P4 chips prior than v3.0");
    }
#endif

    ESP_RETURN_ON_FALSE(isp_proc && config, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(image_type == ESP_ISP_DPC_CALIBRATION_IMAGE_WHITE || image_type == ESP_ISP_DPC_CALIBRATION_IMAGE_BLACK,
                        ESP_ERR_INVALID_ARG, TAG, "invalid calibration image type");
    isp_fsm_t expected_fsm = ISP_FSM_INIT;
    ESP_RETURN_ON_FALSE(atomic_compare_exchange_strong(&isp_proc->dpc_fsm, &expected_fsm, ISP_FSM_ENABLE),
                        ESP_ERR_INVALID_STATE, TAG, "dpc is enabled already");

    s_dpc_take_check_done_event(isp_proc);

    if (image_type == ESP_ISP_DPC_CALIBRATION_IMAGE_WHITE) {
        isp_ll_dpc_set_high_thresh(isp_proc->hal.hw, config->threshold);
    } else {
        isp_ll_dpc_set_low_thresh(isp_proc->hal.hw, config->threshold);
    }

    // Static calibration requires static correction enabled and dynamic correction disabled.
    isp_ll_dpc_enable_dynamic_correction(isp_proc->hal.hw, false);
    isp_ll_dpc_enable_static_correction(isp_proc->hal.hw, true);
    isp_ll_dpc_enable_check_mode_data(isp_proc->hal.hw, config->enable_output);
    isp_ll_dpc_set_input_color(isp_proc->hal.hw, image_type == ESP_ISP_DPC_CALIBRATION_IMAGE_BLACK ?
                               ISP_LL_DPC_INPUT_COLOR_BLACK : ISP_LL_DPC_INPUT_COLOR_WHITE);
    isp_ll_dpc_enable_check_mode(isp_proc->hal.hw, true);
    isp_ll_dpc_enable(isp_proc->hal.hw, true);
    esp_err_t ret = s_dpc_update_shadow(isp_proc, true);
    if (ret != ESP_OK) {
        atomic_store(&isp_proc->dpc_fsm, ISP_FSM_INIT);
        return ret;
    }

    ESP_LOGD(TAG, "DPC calibration started for %s frame", image_type == ESP_ISP_DPC_CALIBRATION_IMAGE_WHITE ? "white" : "black");
    return ESP_OK;
}

static esp_err_t s_dpc_wait_calibration_done(isp_proc_handle_t isp_proc, uint32_t timeout_ms)
{
    ESP_RETURN_ON_FALSE(isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(atomic_load(&isp_proc->dpc_fsm) == ISP_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "dpc calibration isn't running");

    TickType_t timeout_ticks = pdMS_TO_TICKS(timeout_ms);
    if (timeout_ms > 0 && timeout_ticks == 0) {
        timeout_ticks = 1;
    }
    TickType_t start_ticks = xTaskGetTickCount();
    do {
        if (s_dpc_take_check_done_event(isp_proc)) {
            return ESP_OK;
        }
        if (timeout_ms == 0 || (xTaskGetTickCount() - start_ticks) >= timeout_ticks) {
            break;
        }
        vTaskDelay(1);
    } while (true);

    return ESP_ERR_TIMEOUT;
}

esp_err_t esp_isp_dpc_calibration_read_result(isp_proc_handle_t isp_proc, uint32_t timeout_ms,
                                              esp_isp_dpc_calibration_ref_t *result)
{
    ESP_RETURN_ON_FALSE(isp_proc && result, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_ERROR(s_dpc_wait_calibration_done(isp_proc, timeout_ms), TAG, "DPC calibration did not complete");
    isp_fsm_t expected_fsm = ISP_FSM_ENABLE;
    ESP_RETURN_ON_FALSE(atomic_compare_exchange_strong(&isp_proc->dpc_fsm, &expected_fsm, ISP_FSM_INIT),
                        ESP_ERR_INVALID_STATE, TAG, "dpc calibration isn't running");

    // Disable check mode before reading LUT.
    isp_ll_dpc_enable_check_mode(isp_proc->hal.hw, false);
    isp_ll_dpc_enable(isp_proc->hal.hw, false);
    ESP_RETURN_ON_ERROR(s_dpc_update_shadow(isp_proc, true), TAG, "failed to update DPC shadow registers");

    result->dead_pixel_count = 0;

    // Read dead pixel count
    uint32_t deadpix_cnt = isp_ll_dpc_get_deadpix_cnt(isp_proc->hal.hw);
    if (deadpix_cnt > DPC_MAX_DEAD_PIXELS) {
        ESP_LOGW(TAG, "dead pixel count exceeds LUT size (%lu), limiting to %d", (unsigned long)deadpix_cnt, DPC_MAX_DEAD_PIXELS);
        deadpix_cnt = DPC_MAX_DEAD_PIXELS;
    }

    for (uint32_t i = 0; i < deadpix_cnt; i++) {
        isp_ll_lut_dpc_set_cmd(isp_proc->hal.hw, false, i);
        result->dead_pixel_coords[i] = s_dpc_coord_from_hw(isp_ll_lut_dpc_get_rdata(isp_proc->hal.hw));
    }
    result->dead_pixel_count = deadpix_cnt;

    ESP_LOGD(TAG, "Read %lu dead pixels", (unsigned long)deadpix_cnt);
    return ESP_OK;
}

esp_err_t esp_isp_dpc_calibration_merge_result(const esp_isp_dpc_calibration_ref_t *const refs[],
                                               size_t ref_count,
                                               esp_isp_dpc_calibration_ref_t *merged_ref)
{
    ESP_RETURN_ON_FALSE(refs && ref_count > 0 && merged_ref,
                        ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    for (size_t i = 0; i < ref_count; i++) {
        const esp_isp_dpc_calibration_ref_t *ref = refs[i];
        ESP_RETURN_ON_FALSE(ref, ESP_ERR_INVALID_ARG, TAG, "input reference is NULL");
        ESP_RETURN_ON_FALSE(ref != merged_ref, ESP_ERR_INVALID_ARG, TAG, "output reference must not be an input");
        ESP_RETURN_ON_FALSE(ref->dead_pixel_count <= DPC_MAX_DEAD_PIXELS,
                            ESP_ERR_INVALID_ARG, TAG, "dead pixel count exceeds maximum");
    }

    merged_ref->dead_pixel_count = 0;
    bool truncated = false;
    for (size_t i = 0; i < ref_count; i++) {
        const esp_isp_dpc_calibration_ref_t *ref = refs[i];
        for (uint32_t j = 0; j < ref->dead_pixel_count; j++) {
            const esp_isp_dpc_pixel_coord_t coord = ref->dead_pixel_coords[j];
            uint32_t insert_index = 0;
            while (insert_index < merged_ref->dead_pixel_count &&
                    compare_pixel_coords(&merged_ref->dead_pixel_coords[insert_index], &coord) < 0) {
                insert_index++;
            }
            if (insert_index < merged_ref->dead_pixel_count &&
                    compare_pixel_coords(&merged_ref->dead_pixel_coords[insert_index], &coord) == 0) {
                continue;
            }
            if (insert_index == DPC_MAX_DEAD_PIXELS) {
                truncated = true;
                continue;
            }

            uint32_t new_count = merged_ref->dead_pixel_count;
            if (new_count < DPC_MAX_DEAD_PIXELS) {
                new_count++;
            } else {
                truncated = true;
            }
            for (uint32_t k = new_count - 1; k > insert_index; k--) {
                merged_ref->dead_pixel_coords[k] = merged_ref->dead_pixel_coords[k - 1];
            }
            merged_ref->dead_pixel_coords[insert_index] = coord;
            merged_ref->dead_pixel_count = new_count;
        }
    }

    if (truncated) {
        ESP_LOGW(TAG, "Dead pixel count exceeds maximum, limiting to %d", DPC_MAX_DEAD_PIXELS);
    }
    ESP_LOGD(TAG, "Merged %zu references into %lu unique dead pixels",
             ref_count, (unsigned long)merged_ref->dead_pixel_count);
    return ESP_OK;
}

static esp_err_t s_dpc_load_static_lut(isp_proc_handle_t isp_proc,
                                       const esp_isp_dpc_pixel_coord_t *dead_pixel_coords,
                                       uint32_t dead_pixel_count)
{
#if CONFIG_IDF_TARGET_ESP32P4
    unsigned chip_version = efuse_hal_chip_revision();
    if (!ESP_CHIP_REV_ABOVE(chip_version, 300)) {
        ESP_RETURN_ON_FALSE(false, ESP_ERR_NOT_SUPPORTED, TAG, "DPC is not supported on ESP32P4 chips prior than v3.0");
    }
#endif

    ESP_RETURN_ON_FALSE(isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null processor");
    ESP_RETURN_ON_FALSE(atomic_load(&isp_proc->dpc_fsm) == ISP_FSM_INIT,
                        ESP_ERR_INVALID_STATE, TAG, "dpc is enabled already");
    ESP_RETURN_ON_FALSE(dead_pixel_count <= DPC_MAX_DEAD_PIXELS,
                        ESP_ERR_INVALID_ARG, TAG, "dead pixel count exceeds maximum");
    ESP_RETURN_ON_FALSE(dead_pixel_count == 0 || dead_pixel_coords,
                        ESP_ERR_INVALID_ARG, TAG, "dead pixel coordinates are NULL");

    for (uint32_t i = 0; i < dead_pixel_count; i++) {
        const esp_isp_dpc_pixel_coord_t *coord = &dead_pixel_coords[i];
        ESP_RETURN_ON_FALSE(coord->x < isp_proc->h_res && coord->y < isp_proc->v_res,
                            ESP_ERR_INVALID_ARG, TAG, "dead pixel coordinate is outside the input frame");
        ESP_RETURN_ON_FALSE(i == 0 || compare_pixel_coords(coord, &dead_pixel_coords[i - 1]) > 0,
                            ESP_ERR_INVALID_ARG, TAG, "dead pixel coordinates must be sorted by y/x and unique");
        s_dpc_lut_write(isp_proc, i, *coord);
    }
    s_dpc_lut_clear_range(isp_proc, dead_pixel_count);
    ESP_LOGD(TAG, "Loaded %lu dead pixels into static DPC LUT", (unsigned long)dead_pixel_count);
    return ESP_OK;
}

esp_err_t esp_isp_dpc_static_configure(isp_proc_handle_t isp_proc, const esp_isp_dpc_static_config_t *config)
{
    ESP_RETURN_ON_FALSE(isp_proc && config, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(atomic_load(&isp_proc->dpc_fsm) == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "dpc is enabled already");

    ESP_RETURN_ON_ERROR(s_dpc_load_static_lut(isp_proc, config->dead_pixel_coords, config->dead_pixel_count),
                        TAG, "failed to load static DPC LUT");
    isp_ll_dpc_enable_static_correction(isp_proc->hal.hw, true);

    ESP_LOGD(TAG, "Static DPC configured");
    return ESP_OK;
}

esp_err_t esp_isp_dpc_dynamic_configure(isp_proc_handle_t isp_proc, const esp_isp_dpc_dynamic_config_t *config)
{
#if CONFIG_IDF_TARGET_ESP32P4
    unsigned chip_version = efuse_hal_chip_revision();
    if (!ESP_CHIP_REV_ABOVE(chip_version, 300)) {
        ESP_RETURN_ON_FALSE(false, ESP_ERR_NOT_SUPPORTED, TAG, "DPC is not supported on ESP32P4 chips prior than v3.0");
    }
#endif

    ESP_RETURN_ON_FALSE(isp_proc && config, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(atomic_load(&isp_proc->dpc_fsm) == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "dpc is enabled already");

    ESP_RETURN_ON_ERROR(s_dpc_configure_dynamic(isp_proc, config), TAG, "failed to configure dynamic DPC");
    isp_ll_dpc_enable_dynamic_correction(isp_proc->hal.hw, true);

    ESP_LOGD(TAG, "Dynamic DPC configured");
    return ESP_OK;
}

esp_err_t esp_isp_dpc_configure(isp_proc_handle_t isp_proc, const esp_isp_dpc_config_t *config)
{
    ESP_RETURN_ON_FALSE(isp_proc && config, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    ESP_RETURN_ON_FALSE(atomic_load(&isp_proc->dpc_fsm) == ISP_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "dpc is enabled already");

    isp_ll_dpc_set_clk_ctrl_mode(isp_proc->hal.hw, ISP_LL_PIPELINE_CLK_CTRL_AUTO);
    isp_ll_dpc_enable_check_mode(isp_proc->hal.hw, false);
    isp_ll_dpc_enable_check_mode_data(isp_proc->hal.hw, false);
    ESP_RETURN_ON_ERROR(s_dpc_update_shadow(isp_proc, config->flags.update_once_configured), TAG,
                        "failed to update DPC shadow registers");
    isp_proc->sub_module_flags.dpc_update_once_configured = config->flags.update_once_configured;

    ESP_LOGD(TAG, "DPC common configuration applied");
    return ESP_OK;
}

esp_err_t esp_isp_dpc_enable(isp_proc_handle_t isp_proc)
{
    ESP_RETURN_ON_FALSE(isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    isp_fsm_t expected_fsm = ISP_FSM_INIT;
    ESP_RETURN_ON_FALSE(atomic_compare_exchange_strong(&isp_proc->dpc_fsm, &expected_fsm, ISP_FSM_ENABLE), ESP_ERR_INVALID_STATE, TAG, "dpc is enabled already");

    isp_ll_dpc_enable(isp_proc->hal.hw, true);
    esp_err_t ret = s_dpc_update_shadow(isp_proc, isp_proc->sub_module_flags.dpc_update_once_configured);
    if (ret != ESP_OK) {
        atomic_store(&isp_proc->dpc_fsm, ISP_FSM_INIT);
        return ret;
    }

    ESP_LOGD(TAG, "DPC enabled");
    return ESP_OK;
}

esp_err_t esp_isp_dpc_disable(isp_proc_handle_t isp_proc)
{
    ESP_RETURN_ON_FALSE(isp_proc, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");
    isp_fsm_t expected_fsm = ISP_FSM_ENABLE;
    ESP_RETURN_ON_FALSE(atomic_compare_exchange_strong(&isp_proc->dpc_fsm, &expected_fsm, ISP_FSM_INIT), ESP_ERR_INVALID_STATE, TAG, "dpc isn't enabled yet");

    isp_ll_dpc_enable(isp_proc->hal.hw, false);
    esp_err_t ret = s_dpc_update_shadow(isp_proc, isp_proc->sub_module_flags.dpc_update_once_configured);
    if (ret != ESP_OK) {
        atomic_store(&isp_proc->dpc_fsm, ISP_FSM_ENABLE);
        return ret;
    }

    ESP_LOGD(TAG, "DPC disabled");
    return ESP_OK;
}
