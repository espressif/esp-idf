/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <math.h>
#include "example_pipelines.h"
#include "esp_log.h"
#include "driver/isp.h"
#include "example_config.h"

static const char *TAG = "isp_pipeline";

/**
 * @brief Default gamma correction curve
 */
static uint32_t s_gamma_correction_curve(uint32_t x)
{
    return pow((double)x / 256, 0.7) * 256;
}

esp_err_t example_create_isp_processor(const esp_isp_processor_cfg_t *config, isp_proc_handle_t *isp_proc)
{
    if (config == NULL || isp_proc == NULL) {
        ESP_LOGE(TAG, "Invalid arguments");
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = esp_isp_new_processor(config, isp_proc);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create ISP processor: %d", ret);
        return ret;
    }

    ret = esp_isp_enable(*isp_proc);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable ISP processor: %d", ret);
        return ret;
    }

    ESP_LOGI(TAG, "ISP processor initialized");
    return ESP_OK;
}

/**
 * @brief Configure BLC (Black Level Correction) module
 */
static esp_err_t example_isp_pipelines_config_blc(isp_proc_handle_t isp_proc)
{
    if (isp_proc == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

#if CONFIG_ESP32P4_REV_MIN_FULL >= 300
    /**
     * This piece of BLC code is to show how to use the BLC related APIs.
     * Suggested way to calibrate the BLC is by covering the lens and record the raw data.
     * Then, use the recorded data to calibrate the BLC.
     */
    esp_isp_blc_config_t blc_config = {
        .window = {
            .top_left = {
                .x = 0,
                .y = 0,
            },
            .btm_right = {
                .x = CONFIG_EXAMPLE_MIPI_CSI_DISP_HRES,
                .y = CONFIG_EXAMPLE_MIPI_CSI_DISP_VRES,
            },
        },
        .filter_enable = true,
        .filter_threshold = {
            .top_left_chan_thresh = 128,
            .top_right_chan_thresh = 128,
            .bottom_left_chan_thresh = 128,
            .bottom_right_chan_thresh = 128,
        },
        .stretch = {
            .top_left_chan_stretch_en = true,
            .top_right_chan_stretch_en = true,
            .bottom_left_chan_stretch_en = true,
            .bottom_right_chan_stretch_en = true,
        },
    };

    esp_err_t ret = esp_isp_blc_configure(isp_proc, &blc_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure BLC: %d", ret);
        return ret;
    }

    ret = esp_isp_blc_enable(isp_proc);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable BLC: %d", ret);
        return ret;
    }

    esp_isp_blc_offset_t blc_offset = {
        .top_left_chan_offset = 20,
        .top_right_chan_offset = 20,
        .bottom_left_chan_offset = 20,
        .bottom_right_chan_offset = 20,
    };

    ret = esp_isp_blc_set_correction_offset(isp_proc, &blc_offset);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set BLC offset: %d", ret);
        return ret;
    }

    ESP_LOGI(TAG, "BLC module configured");
#else
    ESP_LOGW(TAG, "BLC not supported on this chip revision");
    return ESP_ERR_NOT_SUPPORTED;
#endif

    return ESP_OK;
}

/**
 * @brief Configure BF (Bayer Filter) module
 */
static esp_err_t example_isp_pipelines_config_bf(isp_proc_handle_t isp_proc)
{
    if (isp_proc == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_isp_bf_config_t bf_config = {
        .denoising_level = 5,
        .padding_mode = ISP_BF_EDGE_PADDING_MODE_SRND_DATA,
        .bf_template = {
            {1, 2, 1},
            {2, 4, 2},
            {1, 2, 1},
        },
        .padding_line_tail_valid_start_pixel = 0,
        .padding_line_tail_valid_end_pixel = 0,
    };

    esp_err_t ret = esp_isp_bf_configure(isp_proc, &bf_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure BF: %d", ret);
        return ret;
    }

    ret = esp_isp_bf_enable(isp_proc);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable BF: %d", ret);
        return ret;
    }

    ESP_LOGI(TAG, "BF module configured");
    return ESP_OK;
}

/**
 * @brief Configure LSC (Lens Shading Correction) module
 */
static esp_err_t example_isp_pipelines_config_lsc(isp_proc_handle_t isp_proc)
{
    if (isp_proc == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

#if CONFIG_ESP32P4_REV_MIN_FULL >= 100
    esp_isp_lsc_gain_array_t gain_array = {};
    esp_isp_lsc_config_t lsc_config = {
        .gain_array = &gain_array,
    };

    size_t gain_size = 0;
    esp_err_t ret = esp_isp_lsc_allocate_gain_array(isp_proc, &gain_array, &gain_size);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to allocate LSC gain array: %d", ret);
        return ret;
    }

    isp_lsc_gain_t gain_val = {
        .decimal = 204,
        .integer = 0,
    };

    for (int i = 0; i < gain_size; i++) {
        gain_array.gain_r[i].val = gain_val.val;
        gain_array.gain_gr[i].val = gain_val.val;
        gain_array.gain_gb[i].val = gain_val.val;
        gain_array.gain_b[i].val = gain_val.val;
    }

    ret = esp_isp_lsc_configure(isp_proc, &lsc_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure LSC: %d", ret);
        return ret;
    }

    ret = esp_isp_lsc_enable(isp_proc);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable LSC: %d", ret);
        return ret;
    }

    ESP_LOGI(TAG, "LSC module configured");
#else
    ESP_LOGW(TAG, "LSC not supported on this chip revision");
    return ESP_ERR_NOT_SUPPORTED;
#endif

    return ESP_OK;
}

/**
 * @brief Configure DEMOSAIC module
 */
static esp_err_t example_isp_pipelines_config_demosaic(isp_proc_handle_t isp_proc)
{
    if (isp_proc == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_isp_demosaic_config_t demosaic_config = {
        .grad_ratio = {
            .integer = 2,
            .decimal = 5,
        },
    };

    esp_err_t ret = esp_isp_demosaic_configure(isp_proc, &demosaic_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure DEMOSAIC: %d", ret);
        return ret;
    }

    ret = esp_isp_demosaic_enable(isp_proc);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable DEMOSAIC: %d", ret);
        return ret;
    }

    ESP_LOGI(TAG, "DEMOSAIC module configured");
    return ESP_OK;
}

/**
 * @brief Configure CCM (Color Correction Matrix) module
 */
static esp_err_t example_isp_pipelines_config_ccm(isp_proc_handle_t isp_proc, const float (*matrix)[3])
{
    if (isp_proc == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    /**
     * CCM is used for color correction and white balance adjustment.
     * It should be configured after demosaic and before gamma correction.
     *
     * The matrix format is:
     * [R_out]   [RR RG RB] [R_in]
     * [G_out] = [GR GG GB] [G_in]
     * [B_out]   [BR BG BB] [B_in]
     *
     * For ESP32P4 ECO5:
     * - Matrix coefficients range: ±15.996 (4-bit integer + 8-bit fraction)
     * - For earlier versions: ±3.999 (2-bit integer + 10-bit fraction)
     */
    esp_isp_ccm_config_t ccm_config = {
        .matrix = {
            // Default identity matrix (no color correction)
            {1.0, 0.0, 0.0},   // R channel: R = 1.0*R + 0.0*G + 0.0*B
            {0.0, 1.0, 0.0},   // G channel: G = 0.0*R + 1.0*G + 0.0*B
            {0.0, 0.0, 1.0}    // B channel: B = 0.0*R + 0.0*G + 1.0*B
        },
        .saturation = false        // Don't use saturation for out-of-range values
    };

    // Use provided matrix if available
    if (matrix != NULL) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                ccm_config.matrix[i][j] = matrix[i][j];
            }
        }
    }

    esp_err_t ret = esp_isp_ccm_configure(isp_proc, &ccm_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure CCM: %d", ret);
        return ret;
    }

    ret = esp_isp_ccm_enable(isp_proc);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable CCM: %d", ret);
        return ret;
    }

    ESP_LOGI(TAG, "CCM module configured");
    return ESP_OK;
}

/**
 * @brief Configure GAMMA correction module
 */
static esp_err_t example_isp_pipelines_config_gamma(isp_proc_handle_t isp_proc, uint32_t (*gamma_curve)(uint32_t))
{
    if (isp_proc == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    // Use provided gamma curve or default
    uint32_t (*curve_func)(uint32_t) = gamma_curve;
    if (curve_func == NULL) {
        curve_func = s_gamma_correction_curve;
    }

    isp_gamma_curve_points_t pts = {};
    esp_err_t ret = esp_isp_gamma_fill_curve_points(curve_func, &pts);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to fill gamma curve points: %d", ret);
        return ret;
    }

    ret = esp_isp_gamma_configure(isp_proc, COLOR_COMPONENT_R, &pts);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure gamma R: %d", ret);
        return ret;
    }

    ret = esp_isp_gamma_configure(isp_proc, COLOR_COMPONENT_G, &pts);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure gamma G: %d", ret);
        return ret;
    }

    ret = esp_isp_gamma_configure(isp_proc, COLOR_COMPONENT_B, &pts);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure gamma B: %d", ret);
        return ret;
    }

    ret = esp_isp_gamma_enable(isp_proc);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable gamma: %d", ret);
        return ret;
    }

    ESP_LOGI(TAG, "GAMMA module configured");
    return ESP_OK;
}

/**
 * @brief Configure SHARPEN module
 */
static esp_err_t example_isp_pipelines_config_sharpen(isp_proc_handle_t isp_proc)
{
    if (isp_proc == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_isp_sharpen_config_t sharpen_config = {
        .h_freq_coeff = {
            .integer = 2,
            .decimal = 0,
        },
        .m_freq_coeff = {
            .integer = 2,
            .decimal = 0,
        },
        .h_thresh = 255,
        .l_thresh = 0,
        .padding_mode = ISP_SHARPEN_EDGE_PADDING_MODE_SRND_DATA,
        .sharpen_template = {
            {1, 2, 1},
            {2, 4, 2},
            {1, 2, 1},
        },
        .padding_line_tail_valid_start_pixel = 0,
        .padding_line_tail_valid_end_pixel = 0,
    };

    esp_err_t ret = esp_isp_sharpen_configure(isp_proc, &sharpen_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure SHARPEN: %d", ret);
        return ret;
    }

    ret = esp_isp_sharpen_enable(isp_proc);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable SHARPEN: %d", ret);
        return ret;
    }

    ESP_LOGI(TAG, "SHARPEN module configured");
    return ESP_OK;
}

/**
 * @brief Configure COLOR adjustment module
 */
static esp_err_t example_isp_pipelines_config_color(isp_proc_handle_t isp_proc)
{
    if (isp_proc == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_isp_color_config_t color_config = {
        .color_contrast = {
            .integer = 1,
            .decimal = 0,
        },
        .color_saturation = {
            .integer = 1,
            .decimal = 0,
        },
        .color_hue = 0,
        .color_brightness = 0,
    };

    esp_err_t ret = esp_isp_color_configure(isp_proc, &color_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure COLOR: %d", ret);
        return ret;
    }

    ret = esp_isp_color_enable(isp_proc);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable COLOR: %d", ret);
        return ret;
    }

    ESP_LOGI(TAG, "COLOR module configured");
    return ESP_OK;
}

#ifdef CONFIG_EXAMPLE_ISP_CROP_ENABLE

/**
 * @brief Configure CROP module
 */
static esp_err_t example_isp_pipelines_config_crop(isp_proc_handle_t isp_proc,
                                                   int crop_left,
                                                   int crop_top,
                                                   int crop_right,
                                                   int crop_bottom)
{
    if (isp_proc == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_isp_crop_config_t crop_config = {
        .window = {
            .top_left = {
                .x = crop_left,
                .y = crop_top
            },
            .btm_right = {
                .x = crop_right,
                .y = crop_bottom
            }
        }
    };

    esp_err_t ret = esp_isp_crop_configure(isp_proc, &crop_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure CROP: %d", ret);
        return ret;
    }

    ret = esp_isp_crop_enable(isp_proc);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable CROP: %d", ret);
        return ret;
    }

    ESP_LOGI(TAG, "CROP module configured: (%d,%d) to (%d,%d)",
             crop_left, crop_top, crop_right, crop_bottom);

    return ESP_OK;
}
#endif

esp_err_t example_isp_init_all_pipelines(isp_proc_handle_t isp_proc)
{
    if (isp_proc == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_ERROR_CHECK(example_isp_pipelines_config_blc(isp_proc));
    ESP_ERROR_CHECK(example_isp_pipelines_config_bf(isp_proc));
    ESP_ERROR_CHECK(example_isp_pipelines_config_lsc(isp_proc));
    ESP_ERROR_CHECK(example_isp_pipelines_config_demosaic(isp_proc));
    ESP_ERROR_CHECK(example_isp_pipelines_config_ccm(isp_proc, NULL));
    ESP_ERROR_CHECK(example_isp_pipelines_config_gamma(isp_proc, NULL));
    ESP_ERROR_CHECK(example_isp_pipelines_config_sharpen(isp_proc));
    ESP_ERROR_CHECK(example_isp_pipelines_config_color(isp_proc));

#ifdef CONFIG_EXAMPLE_ISP_CROP_ENABLE
    ESP_ERROR_CHECK(example_isp_pipelines_config_crop(isp_proc,
                                                      CONFIG_EXAMPLE_ISP_CROP_TOP_LEFT_H,
                                                      CONFIG_EXAMPLE_ISP_CROP_TOP_LEFT_V,
                                                      CONFIG_EXAMPLE_ISP_CROP_BOTTOM_RIGHT_H,
                                                      CONFIG_EXAMPLE_ISP_CROP_BOTTOM_RIGHT_V));
#endif

    ESP_LOGI(TAG, "All ISP pipeline modules initialized");
    return ESP_OK;
}
