/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include <stdio.h>
#include "esp_log.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/isp_dpc.h"
#include "example_dpc.h"

static const char *TAG = "isp_dpc";

#define EXAMPLE_DPC_WHITE_THRESHOLD                    0xf0
#define EXAMPLE_DPC_BLACK_THRESHOLD                    0x0a
#define EXAMPLE_DPC_CALIBRATION_TIMEOUT_MS             5000

static esp_isp_dpc_calibration_ref_t s_white_calibration_ref;
static esp_isp_dpc_calibration_ref_t s_black_calibration_ref;

/* Wait for the user to prepare the requested calibration image. */
static void s_example_dpc_wait_for_calibration_image(const char *image_name)
{
    ESP_LOGI(TAG, "Present a uniform %s image to the camera, then press Enter", image_name);
    int c;
    do {
        c = getchar();
        if (c == EOF) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    } while (c != '\n');
}

/* Capture white and black calibration frames, then merge their bad-pixel lists. */
esp_err_t example_isp_dpc_calibrate_static(isp_proc_handle_t isp_proc, esp_isp_dpc_calibration_ref_t *calibration_ref)
{
    ESP_RETURN_ON_FALSE(isp_proc && calibration_ref, ESP_ERR_INVALID_ARG, TAG, "invalid arguments");

    esp_isp_dpc_calibration_config_t calibration_config = {
        .enable_output = true,
    };

    s_example_dpc_wait_for_calibration_image("white");
    calibration_config.threshold = EXAMPLE_DPC_WHITE_THRESHOLD;
    ESP_RETURN_ON_ERROR(esp_isp_dpc_static_calibration_start_once(isp_proc, ESP_ISP_DPC_CALIBRATION_IMAGE_WHITE, &calibration_config),
                        TAG, "failed to start white-frame DPC calibration");
    ESP_RETURN_ON_ERROR(esp_isp_dpc_calibration_read_result(isp_proc, EXAMPLE_DPC_CALIBRATION_TIMEOUT_MS, &s_white_calibration_ref),
                        TAG, "failed to read white-frame DPC calibration result");

    s_example_dpc_wait_for_calibration_image("black");
    calibration_config.threshold = EXAMPLE_DPC_BLACK_THRESHOLD;
    ESP_RETURN_ON_ERROR(esp_isp_dpc_static_calibration_start_once(isp_proc, ESP_ISP_DPC_CALIBRATION_IMAGE_BLACK, &calibration_config),
                        TAG, "failed to start black-frame DPC calibration");
    ESP_RETURN_ON_ERROR(esp_isp_dpc_calibration_read_result(isp_proc, EXAMPLE_DPC_CALIBRATION_TIMEOUT_MS, &s_black_calibration_ref),
                        TAG, "failed to read black-frame DPC calibration result");

    const esp_isp_dpc_calibration_ref_t *refs[] = {
        &s_white_calibration_ref,
        &s_black_calibration_ref,
    };
    ESP_RETURN_ON_ERROR(esp_isp_dpc_calibration_merge_result(refs, sizeof(refs) / sizeof(refs[0]), calibration_ref),
                        TAG, "failed to merge DPC calibration results");

    ESP_LOGI(TAG, "DPC static calibration done, %" PRIu32 " dead pixels found", calibration_ref->dead_pixel_count);
    return ESP_OK;
}

/* Configure static correction when available, followed by dynamic correction. */
esp_err_t example_isp_dpc_init(isp_proc_handle_t isp_proc, const esp_isp_dpc_calibration_ref_t *calibration_ref)
{
    ESP_RETURN_ON_FALSE(isp_proc, ESP_ERR_INVALID_ARG, TAG, "Invalid arguments: isp_proc is NULL");

    if (calibration_ref) {
        esp_isp_dpc_static_config_t static_config = {
            .dead_pixel_coords = calibration_ref->dead_pixel_coords,
            .dead_pixel_count = calibration_ref->dead_pixel_count,
        };
        esp_err_t ret = esp_isp_dpc_static_configure(isp_proc, &static_config);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to configure static DPC: %d", ret);
            return ret;
        }
        ESP_LOGI(TAG, "DPC static correction configured");
    }

    esp_isp_dpc_dynamic_config_t dpc_config = {
        .method = ESP_ISP_DPC_DYNAMIC_METHOD_1,
        .method_1 = {
            .high_threshold = 48,
            .low_threshold = 48,
        },
    };
    esp_err_t ret = esp_isp_dpc_dynamic_configure(isp_proc, &dpc_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure DPC: %d", ret);
        return ret;
    }

    esp_isp_dpc_config_t common_config = {
        .flags.update_once_configured = true,
    };
    ret = esp_isp_dpc_configure(isp_proc, &common_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure common DPC settings: %d", ret);
        return ret;
    }

    ESP_LOGI(TAG, "DPC module initialized");

    return ESP_OK;
}

/* Enable the configured DPC module. */
esp_err_t example_isp_dpc_enable(isp_proc_handle_t isp_proc)
{
    ESP_RETURN_ON_FALSE(isp_proc, ESP_ERR_INVALID_ARG, TAG, "Invalid arguments: isp_proc is NULL");

    esp_err_t ret = esp_isp_dpc_enable(isp_proc);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable DPC: %d", ret);
        return ret;
    }

    ESP_LOGI(TAG, "DPC module enabled");
    return ESP_OK;
}
