/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_log.h"
#include "esp_check.h"
#include "driver/isp_awb.h"
#include "driver/isp_wbg.h"
#include "driver/isp_types.h"
#include "example_config.h"
#include "example_awb.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/idf_additions.h"
#include "esp_heap_caps.h"

static const char *TAG = "isp_awb";

static isp_awb_ctlr_t s_awb_ctlr = NULL;
static isp_proc_handle_t s_isp_proc = NULL;
static QueueHandle_t s_awb_queue = NULL;
static TaskHandle_t s_awb_task_handle = NULL;

// Configuration for gain printing
#define AWB_GAIN_UPDATE_COUNT    5

// White balance gain normalization value (1.0 = neutral gain)
#define AWB_GAIN_NORM             256   // Normalization value for gain calculation

// PI Controller parameters for smooth gain convergence
#define AWB_P_GAIN                0.5f   // Proportional gain (0.0-1.0)

/**
 * @brief Get default AWB configuration based on image resolution
 */
static void s_get_default_awb_config(esp_isp_awb_config_t *config, uint32_t h_res, uint32_t v_res)
{
    // Set sample point: after CCM by default
    config->sample_point = ISP_AWB_SAMPLE_POINT_AFTER_CCM;

    // Configure window: middle 80% of image to avoid edge overexposure
    config->window.top_left.x = h_res * 0.2f;
    config->window.top_left.y = v_res * 0.2f;
    config->window.btm_right.x = h_res * 0.8f - 1;
    config->window.btm_right.y = v_res * 0.8f - 1;

    // Configure subwindow: same size as main window
    config->subwindow = config->window;

    // Configure white patch detection thresholds
    // Luminance range: [0, 220*3] to avoid overexposed pixels
    config->white_patch.luminance.min = 0;
    config->white_patch.luminance.max = 220 * 3;

    // Color ratio ranges: wide range to include all possible white patches
    config->white_patch.red_green_ratio.min = 0.5f;
    config->white_patch.red_green_ratio.max = 1.999f;
    config->white_patch.blue_green_ratio.min = 0.5f;
    config->white_patch.blue_green_ratio.max = 1.999f;

    // Interrupt priority: 0 means auto-allocate
    config->intr_priority = 0;
}

/**
 * @brief AWB statistics callback (runs in ISR context)
 *
 * This callback receives AWB statistics and sends them to the processing task via queue.
 * It should be lightweight as it runs in ISR context.
 */
static bool IRAM_ATTR s_awb_statistics_callback(isp_awb_ctlr_t awb_ctlr, const esp_isp_awb_evt_data_t *edata, void *user_data)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Send statistics to queue (from ISR)
    if (s_awb_queue != NULL) {
        if (xQueueSendFromISR(s_awb_queue, &edata->awb_result, &xHigherPriorityTaskWoken) != pdTRUE) {
            ESP_DRAM_LOGE(TAG, "Failed to send AWB statistics to queue (queue full)");
            return false;
        }
    }
    return xHigherPriorityTaskWoken == pdTRUE;
}

/**
 * @brief PI controller for smooth white balance gain update
 *
 * This function implements a PI (Proportional-Integral) controller to smoothly update white balance gains, preventing oscillation and ensuring convergence.
 *
 * Currently we just do the P control, the I control is not used and could be extended in the future.
 *
 * @param[in] target_gain Target gain calculated from statistics
 * @param[in] current_gain Currently applied gain
 * @param[out] new_gain Calculated new gain to apply
 */
static void s_pi_controller_update(isp_wbg_gain_t target_gain,
                                   isp_wbg_gain_t current_gain,
                                   isp_wbg_gain_t *new_gain)
{
    // Calculate error (target - current)
    float error_r = (float)target_gain.gain_r - (float)current_gain.gain_r;
    float error_b = (float)target_gain.gain_b - (float)current_gain.gain_b;

    // Calculate new gain: current + P*error + I*integral
    float new_gain_r = (float)current_gain.gain_r + AWB_P_GAIN * error_r;
    float new_gain_b = (float)current_gain.gain_b + AWB_P_GAIN * error_b;

    // Round and clamp to valid range
    new_gain->gain_r = (uint32_t)(new_gain_r + 0.5f);
    new_gain->gain_g = AWB_GAIN_NORM;  // G channel is reference
    new_gain->gain_b = (uint32_t)(new_gain_b + 0.5f);
}

/**
 * @brief Calculate white balance gain from statistics
 *
 * @param[in] stat_result AWB statistics result
 * @param[out] gain Calculated white balance gain
 * @return true if calculation successful, false otherwise
 */
static bool s_calculate_awb_gain(const isp_awb_stat_result_t *stat_result, isp_wbg_gain_t *gain)
{
    if (stat_result == NULL || gain == NULL) {
        return false;
    }

    // Check if we have enough white patches
    if (stat_result->white_patch_num == 0) {
        ESP_LOGD(TAG, "No white patches detected, keeping current gain");
        return false;
    }

    // Check if sum values are valid (avoid division by zero)
    if (stat_result->sum_r == 0 || stat_result->sum_b == 0) {
        ESP_LOGW(TAG, "Invalid sum values (R=%lu, B=%lu), keeping current gain",
                 stat_result->sum_r, stat_result->sum_b);
        return false;
    }

    // Calculate gains: use G channel as reference
    float gain_r_float = ((float)stat_result->sum_g / (float)stat_result->sum_r) * (float)AWB_GAIN_NORM;
    float gain_b_float = ((float)stat_result->sum_g / (float)stat_result->sum_b) * (float)AWB_GAIN_NORM;

    // Clamp gains to valid range
    gain->gain_r = (uint32_t)(gain_r_float + 0.5f);  // Round to nearest
    gain->gain_g = AWB_GAIN_NORM;
    gain->gain_b = (uint32_t)(gain_b_float + 0.5f);

    ESP_LOGD(TAG, "Calculated AWB gain: R=%lu, G=%lu, B=%lu (patches=%lu, sum_r=%lu, sum_g=%lu, sum_b=%lu)",
             gain->gain_r, gain->gain_g, gain->gain_b,
             stat_result->white_patch_num, stat_result->sum_r, stat_result->sum_g, stat_result->sum_b);

    return true;
}

/**
 * @brief White balance processing task
 *
 * This task receives AWB statistics from the queue, calculates white balance gains,
 * accumulates them over multiple periods, and applies the smoothed gain through PI controller
 * to prevent oscillation and ensure convergence.
 */
static void s_awb_task(void *pvParameters)
{
    isp_awb_stat_result_t stat_result;
    isp_wbg_gain_t gain;
    uint32_t gain_count = 0;
    uint64_t sum_r = 0, sum_g = 0, sum_b = 0;

    // Current applied gain (maintained by PI controller)
    isp_wbg_gain_t current_gain = {
        .gain_r = AWB_GAIN_NORM,
        .gain_g = AWB_GAIN_NORM,
        .gain_b = AWB_GAIN_NORM,
    };

    ESP_LOGI(TAG, "White balance task started");

    while (1) {
        // Wait for statistics from queue
        if (xQueueReceive(s_awb_queue, &stat_result, portMAX_DELAY) == pdTRUE) {
            // Calculate white balance gain
            if (s_calculate_awb_gain(&stat_result, &gain)) {
                // Accumulate gain values for averaging
                sum_r += gain.gain_r;
                sum_g += gain.gain_g;
                sum_b += gain.gain_b;
                gain_count++;

                // When we have enough samples, calculate average and apply with PI controller
                if (gain_count >= AWB_GAIN_UPDATE_COUNT) {
                    // Calculate average target gain
                    isp_wbg_gain_t target_gain = {};
                    target_gain.gain_r = (uint32_t)(sum_r / AWB_GAIN_UPDATE_COUNT);
                    target_gain.gain_g = (uint32_t)(sum_g / AWB_GAIN_UPDATE_COUNT);
                    target_gain.gain_b = (uint32_t)(sum_b / AWB_GAIN_UPDATE_COUNT);

                    // Apply PI controller to calculate new gain
                    isp_wbg_gain_t new_gain = {};
                    s_pi_controller_update(target_gain, current_gain, &new_gain);

                    // Apply new gain through WBG module
                    esp_err_t ret = esp_isp_wbg_set_wb_gain(s_isp_proc, new_gain);
                    if (ret != ESP_OK) {
                        ESP_LOGE(TAG, "Failed to set AWB gain: %d", ret);
                    } else {
                        // Update current gain
                        current_gain = new_gain;

                        // Print final updated gain values
                        float gain_r_norm = (float)new_gain.gain_r / (float)AWB_GAIN_NORM;
                        float gain_g_norm = (float)new_gain.gain_g / (float)AWB_GAIN_NORM;
                        float gain_b_norm = (float)new_gain.gain_b / (float)AWB_GAIN_NORM;

                        ESP_LOGD(TAG, "AWB Gain Updated - R: %lu (%.3f), G: %lu (%.3f), B: %lu (%.3f) [PI controlled]",
                                 new_gain.gain_r, gain_r_norm,
                                 new_gain.gain_g, gain_g_norm,
                                 new_gain.gain_b, gain_b_norm);
                    }

                    // Reset for next cycle
                    gain_count = 0;
                    sum_r = 0;
                    sum_g = 0;
                    sum_b = 0;
                }
            }
        }
    }

    ESP_LOGI(TAG, "White balance task exiting");
    vTaskDelete(NULL);
}

esp_err_t example_isp_awb_init(isp_proc_handle_t isp_proc)
{
    if (isp_proc == NULL) {
        ESP_LOGE(TAG, "Invalid arguments: isp_proc is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    if (s_awb_ctlr != NULL) {
        ESP_LOGW(TAG, "AWB controller already initialized");
        return ESP_OK;
    }

    s_isp_proc = isp_proc;

    esp_isp_awb_config_t awb_config = {};
    s_get_default_awb_config(&awb_config, CONFIG_EXAMPLE_MIPI_CSI_DISP_HRES, CONFIG_EXAMPLE_MIPI_CSI_DISP_VRES);

    // Create AWB controller for statistics
    esp_err_t ret = esp_isp_new_awb_controller(isp_proc, &awb_config, &s_awb_ctlr);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create AWB controller: %d", ret);
        return ret;
    }

    // Configure WBG module
    esp_isp_wbg_config_t wbg_config = {};
    ret = esp_isp_wbg_configure(isp_proc, &wbg_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure WBG: %d", ret);
        ESP_ERROR_CHECK(esp_isp_del_awb_controller(s_awb_ctlr));
        s_awb_ctlr = NULL;
        return ret;
    }

    ESP_LOGI(TAG, "AWB and WBG module initialized");

    return ESP_OK;
}

esp_err_t example_isp_awb_start(isp_proc_handle_t isp_proc)
{
    esp_err_t ret = ESP_OK;

    if (isp_proc == NULL) {
        ESP_LOGE(TAG, "Invalid arguments: isp_proc is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    if (s_awb_ctlr == NULL) {
        ESP_LOGE(TAG, "AWB not initialized, call example_isp_awb_init() first");
        return ESP_ERR_INVALID_STATE;
    }

    // Create queue for statistics (use internal RAM for ISR compatibility)
    // Ignore the results of frames that are not received (if any)
    s_awb_queue = xQueueCreateWithCaps(1, sizeof(isp_awb_stat_result_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    if (s_awb_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create AWB queue");
        return ESP_ERR_NO_MEM;
    }

    // Register AWB statistics callback
    esp_isp_awb_cbs_t awb_cbs = {
        .on_statistics_done = s_awb_statistics_callback,
    };
    ESP_GOTO_ON_ERROR(esp_isp_awb_register_event_callbacks(s_awb_ctlr, &awb_cbs, NULL), err, TAG, "Failed to register AWB callbacks");

    ESP_GOTO_ON_ERROR(esp_isp_wbg_enable(isp_proc), err, TAG, "Failed to enable WBG");

    // Set initial gain (neutral: all channels equal)
    isp_wbg_gain_t initial_gain = {
        .gain_r = AWB_GAIN_NORM,
        .gain_g = AWB_GAIN_NORM,
        .gain_b = AWB_GAIN_NORM,
    };
    ret = esp_isp_wbg_set_wb_gain(isp_proc, initial_gain);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Failed to set initial AWB gain: %d", ret);
    }

    // Enable AWB controller
    ESP_GOTO_ON_ERROR(esp_isp_awb_controller_enable(s_awb_ctlr), err, TAG, "Failed to enable AWB controller");

    // Start continuous statistics
    ESP_GOTO_ON_ERROR(esp_isp_awb_controller_start_continuous_statistics(s_awb_ctlr), err, TAG, "Failed to start continuous statistics");

    // Create white balance processing task
    ESP_GOTO_ON_FALSE(pdPASS == xTaskCreate(s_awb_task, "awb_task", 4096, NULL, 5, &s_awb_task_handle), ESP_FAIL, err, TAG, "Failed to create AWB task");

    ESP_LOGI(TAG, "AWB enabled: AWB statistics started, WBG module enabled, processing task created");
    return ESP_OK;

err:
    ESP_LOGE(TAG, "AWB start failed: %s (0x%x)", esp_err_to_name(ret), ret);
    esp_isp_awb_controller_stop_continuous_statistics(s_awb_ctlr);
    esp_isp_awb_controller_disable(s_awb_ctlr);
    esp_isp_wbg_disable(isp_proc);
    if (s_awb_queue != NULL) {
        vQueueDeleteWithCaps(s_awb_queue);
        s_awb_queue = NULL;
    }
    if (s_awb_task_handle != NULL) {
        vTaskDelete(s_awb_task_handle);
        s_awb_task_handle = NULL;
    }
    return ret;
}
