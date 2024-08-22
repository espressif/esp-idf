/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include "sdkconfig.h"
#include "unity.h"
#include "driver/isp.h"
#include "soc/soc_caps.h"

/*---------------------------------------------------------------
                      ISP
---------------------------------------------------------------*/

TEST_CASE("ISP processor exhausted allocation", "[isp]")
{
    esp_isp_processor_cfg_t isp_config = {
        .clk_hz = 80 * 1000 * 1000,
        .input_data_source = ISP_INPUT_DATA_SOURCE_CSI,
        .input_data_color_type = ISP_COLOR_RAW8,
        .output_data_color_type = ISP_COLOR_RGB565,
    };
    isp_proc_handle_t isp_proc[SOC_ISP_NUMS + 1] = {};

    for (int i = 0; i < SOC_ISP_NUMS; i++) {
        TEST_ESP_OK(esp_isp_new_processor(&isp_config, &isp_proc[i]));
    }

    TEST_ASSERT(esp_isp_new_processor(&isp_config, &isp_proc[SOC_ISP_NUMS]) == ESP_ERR_NOT_FOUND);

    for (int i = 0; i < SOC_ISP_NUMS; i++) {
        TEST_ESP_OK(esp_isp_del_processor(isp_proc[i]));
    }
}

/*---------------------------------------------------------------
                      AF
---------------------------------------------------------------*/

static bool test_isp_af_default_on_statistics_done_cb(isp_af_ctlr_t af_ctlr, const esp_isp_af_env_detector_evt_data_t *edata, void *user_data)
{
    (void) af_ctlr;
    (void) edata;
    (void) user_data;
    // Do nothing
    return false;
}

static bool test_isp_af_default_on_continuous_done_cb(isp_af_ctlr_t af_ctlr, const esp_isp_af_env_detector_evt_data_t *edata, void *user_data)
{
    (void) af_ctlr;
    (void) edata;
    (void) user_data;
    // Do nothing
    return false;
}

TEST_CASE("ISP AF driver oneshot vs continuous test", "[isp]")
{
    esp_isp_processor_cfg_t isp_config = {
        .clk_hz = 80 * 1000 * 1000,
        .input_data_source = ISP_INPUT_DATA_SOURCE_CSI,
        .input_data_color_type = ISP_COLOR_RAW8,
        .output_data_color_type = ISP_COLOR_RGB565,
    };
    isp_proc_handle_t isp_proc = NULL;
    TEST_ESP_OK(esp_isp_new_processor(&isp_config, &isp_proc));
    TEST_ESP_OK(esp_isp_enable(isp_proc));

    esp_isp_af_config_t af_config = {
        .edge_thresh = 128,
    };
    isp_af_ctlr_t af_ctlr = NULL;
    isp_af_result_t af_res = {};
    /* Create the af controller */
    TEST_ESP_OK(esp_isp_new_af_controller(isp_proc, &af_config, &af_ctlr));
    /* Register af callback */
    esp_isp_af_env_detector_evt_cbs_t af_cb = {
        .on_env_statistics_done = test_isp_af_default_on_statistics_done_cb,
        .on_env_change = test_isp_af_default_on_continuous_done_cb,
    };
    TEST_ESP_OK(esp_isp_af_env_detector_register_event_callbacks(af_ctlr, &af_cb, NULL));
    /* Enabled the af controller */
    TEST_ESP_OK(esp_isp_af_controller_enable(af_ctlr));
    /* Start continuous af statistics */
    TEST_ESP_OK(esp_isp_af_controller_start_continuous_statistics(af_ctlr));

    /* Test do oneshot mode when continuous mode start, should return error */
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_isp_af_controller_get_oneshot_statistics(af_ctlr, 0, &af_res));

    /* Stop continuous af statistics */
    TEST_ESP_OK(esp_isp_af_controller_stop_continuous_statistics(af_ctlr));
    TEST_ESP_ERR(ESP_ERR_TIMEOUT, esp_isp_af_controller_get_oneshot_statistics(af_ctlr, 1, &af_res));

    /* Disable the af controller */
    TEST_ESP_OK(esp_isp_af_controller_disable(af_ctlr));

    /* Delete the af controller and free the resources */
    TEST_ESP_OK(esp_isp_del_af_controller(af_ctlr));

    TEST_ESP_OK(esp_isp_disable(isp_proc));
    TEST_ESP_OK(esp_isp_del_processor(isp_proc));
}

TEST_CASE("ISP AF controller exhausted allocation", "[isp]")
{
    esp_isp_processor_cfg_t isp_config = {
        .clk_hz = 80 * 1000 * 1000,
        .input_data_source = ISP_INPUT_DATA_SOURCE_CSI,
        .input_data_color_type = ISP_COLOR_RAW8,
        .output_data_color_type = ISP_COLOR_RGB565,
    };
    isp_proc_handle_t isp_proc = NULL;
    TEST_ESP_OK(esp_isp_new_processor(&isp_config, &isp_proc));

    esp_isp_af_config_t af_config = {
        .edge_thresh = 128,
    };
    isp_af_ctlr_t af_ctrlr[SOC_ISP_AF_CTLR_NUMS + 1] = {};
    for (int i = 0; i < SOC_ISP_AF_CTLR_NUMS; i++) {
        TEST_ESP_OK(esp_isp_new_af_controller(isp_proc, &af_config, &af_ctrlr[i]));
    }

    TEST_ASSERT(esp_isp_new_af_controller(isp_proc, &af_config, &af_ctrlr[SOC_ISP_AF_CTLR_NUMS]) == ESP_ERR_NOT_FOUND);

    for (int i = 0; i < SOC_ISP_AF_CTLR_NUMS; i++) {
        TEST_ESP_OK(esp_isp_del_af_controller(af_ctrlr[i]));
    }
    TEST_ESP_OK(esp_isp_del_processor(isp_proc));
}

/*---------------------------------------------------------------
                      AWB
---------------------------------------------------------------*/

static bool test_isp_awb_default_on_statistics_done_cb(isp_awb_ctlr_t awb_ctlr, const esp_isp_awb_evt_data_t *edata, void *user_data)
{
    (void) awb_ctlr;
    (void) edata;
    (void) user_data;
    // Do nothing
    return false;
}

TEST_CASE("ISP AWB driver oneshot vs continuous test", "[isp]")
{
    esp_isp_processor_cfg_t isp_config = {
        .clk_hz = 80 * 1000 * 1000,
        .input_data_source = ISP_INPUT_DATA_SOURCE_CSI,
        .input_data_color_type = ISP_COLOR_RAW8,
        .output_data_color_type = ISP_COLOR_RGB565,
    };
    isp_proc_handle_t isp_proc = NULL;
    TEST_ESP_OK(esp_isp_new_processor(&isp_config, &isp_proc));
    TEST_ESP_OK(esp_isp_enable(isp_proc));

    isp_awb_ctlr_t awb_ctlr = NULL;
    uint32_t image_width = 800;
    uint32_t image_height = 600;
    /* Default parameters from helper macro */
    esp_isp_awb_config_t awb_config = {
        .sample_point = ISP_AWB_SAMPLE_POINT_AFTER_CCM,
        .window = {
            .top_left = {.x = image_width * 0.2, .y = image_height * 0.2},
            .btm_right = {.x = image_width * 0.8, .y = image_height * 0.8},
        },
        .white_patch = {
            .luminance = {.min = 0, .max = 220 * 3},
            .red_green_ratio = {.min = 0.0f, .max = 3.999f},
            .blue_green_ratio = {.min = 0.0f, .max = 3.999f},
        },
    };
    isp_awb_stat_result_t stat_res = {};
    /* Create the awb controller */
    TEST_ESP_OK(esp_isp_new_awb_controller(isp_proc, &awb_config, &awb_ctlr));
    /* Register AWB callback */
    esp_isp_awb_cbs_t awb_cb = {
        .on_statistics_done = test_isp_awb_default_on_statistics_done_cb,
    };
    TEST_ESP_OK(esp_isp_awb_register_event_callbacks(awb_ctlr, &awb_cb, NULL));
    /* Enabled the awb controller */
    TEST_ESP_OK(esp_isp_awb_controller_enable(awb_ctlr));
    /* Start continuous AWB statistics */
    TEST_ESP_OK(esp_isp_awb_controller_start_continuous_statistics(awb_ctlr));
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_isp_awb_controller_get_oneshot_statistics(awb_ctlr, 0, &stat_res));
    /* Stop continuous AWB statistics */
    TEST_ESP_OK(esp_isp_awb_controller_stop_continuous_statistics(awb_ctlr));
    TEST_ESP_ERR(ESP_ERR_TIMEOUT, esp_isp_awb_controller_get_oneshot_statistics(awb_ctlr, 1, &stat_res));
    /* Disable the awb controller */
    TEST_ESP_OK(esp_isp_awb_controller_disable(awb_ctlr));
    /* Delete the awb controller and free the resources */
    TEST_ESP_OK(esp_isp_del_awb_controller(awb_ctlr));

    TEST_ESP_OK(esp_isp_disable(isp_proc));
    TEST_ESP_OK(esp_isp_del_processor(isp_proc));
}

TEST_CASE("ISP CCM basic function", "[isp]")
{
    esp_isp_processor_cfg_t isp_config = {
        .clk_hz = 80 * 1000 * 1000,
        .input_data_source = ISP_INPUT_DATA_SOURCE_CSI,
        .input_data_color_type = ISP_COLOR_RAW8,
        .output_data_color_type = ISP_COLOR_RGB565,
    };
    isp_proc_handle_t isp_proc = NULL;
    TEST_ESP_OK(esp_isp_new_processor(&isp_config, &isp_proc));
    TEST_ESP_OK(esp_isp_enable(isp_proc));

    esp_isp_ccm_config_t ccm_cfg = {
        .matrix = {
            {5.0, 0.0, 0.0},
            {0.0, 1.0, 0.0},
            {0.0, 0.0, 1.0}
        },
        .saturation = false,
    };
    // Out of range case
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_isp_ccm_configure(isp_proc, &ccm_cfg));
    // saturation case
    ccm_cfg.saturation = true;
    TEST_ESP_OK(esp_isp_ccm_configure(isp_proc, &ccm_cfg));
    TEST_ESP_OK(esp_isp_ccm_enable(isp_proc));
    // Allow to be called after enabled
    ccm_cfg.matrix[0][0] = -1.1;
    TEST_ESP_OK(esp_isp_ccm_configure(isp_proc, &ccm_cfg));
    TEST_ESP_OK(esp_isp_ccm_disable(isp_proc));

    TEST_ESP_OK(esp_isp_disable(isp_proc));
    TEST_ESP_OK(esp_isp_del_processor(isp_proc));
}

/*---------------------------------------------------------------
                      AE
---------------------------------------------------------------*/

static bool test_isp_ae_default_on_statistics_done_cb(isp_ae_ctlr_t ae_ctlr, const esp_isp_ae_env_detector_evt_data_t *edata, void *user_data)
{
    (void) ae_ctlr;
    (void) edata;
    (void) user_data;
    // Do nothing
    return false;
}

static bool test_isp_ae_default_on_continuous_done_cb(isp_ae_ctlr_t ae_ctlr, const esp_isp_ae_env_detector_evt_data_t *edata, void *user_data)
{
    (void) ae_ctlr;
    (void) edata;
    (void) user_data;
    // Do nothing
    return false;
}

TEST_CASE("ISP AE driver oneshot vs continuous test", "[isp]")
{
    esp_isp_processor_cfg_t isp_config = {
        .clk_hz = 80 * 1000 * 1000,
        .input_data_source = ISP_INPUT_DATA_SOURCE_CSI,
        .input_data_color_type = ISP_COLOR_RAW8,
        .output_data_color_type = ISP_COLOR_RGB565,
    };
    isp_proc_handle_t isp_proc = NULL;
    TEST_ESP_OK(esp_isp_new_processor(&isp_config, &isp_proc));
    TEST_ESP_OK(esp_isp_enable(isp_proc));

    isp_ae_ctlr_t ae_ctlr = NULL;
    /* Default parameters from helper macro */
    esp_isp_ae_config_t ae_config = {
        .sample_point = ISP_AE_SAMPLE_POINT_AFTER_DEMOSAIC,
    };
    isp_ae_result_t ae_res = {};
    /* Create the ae controller */
    TEST_ESP_OK(esp_isp_new_ae_controller(isp_proc, &ae_config, &ae_ctlr));
    /* Register AE callback */
    esp_isp_ae_env_detector_evt_cbs_t ae_cb = {
        .on_env_statistics_done = test_isp_ae_default_on_statistics_done_cb,
        .on_env_change = test_isp_ae_default_on_continuous_done_cb,
    };
    TEST_ESP_OK(esp_isp_ae_env_detector_register_event_callbacks(ae_ctlr, &ae_cb, NULL));
    /* Enabled the ae controller */
    TEST_ESP_OK(esp_isp_ae_controller_enable(ae_ctlr));
    /* Start continuous AE statistics */
    TEST_ESP_OK(esp_isp_ae_controller_start_continuous_statistics(ae_ctlr));

    /* Test do oneshot mode when continuous mode start, should return error */
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_isp_ae_controller_get_oneshot_statistics(ae_ctlr, 0, &ae_res));

    /* Stop continuous AE statistics */
    TEST_ESP_OK(esp_isp_ae_controller_stop_continuous_statistics(ae_ctlr));
    TEST_ESP_ERR(ESP_ERR_TIMEOUT, esp_isp_ae_controller_get_oneshot_statistics(ae_ctlr, 1, &ae_res));

    /* Disable the ae controller */
    TEST_ESP_OK(esp_isp_ae_controller_disable(ae_ctlr));

    /* Delete the ae controller and free the resources */
    TEST_ESP_OK(esp_isp_del_ae_controller(ae_ctlr));

    TEST_ESP_OK(esp_isp_disable(isp_proc));
    TEST_ESP_OK(esp_isp_del_processor(isp_proc));
}

TEST_CASE("ISP AE controller exhausted allocation", "[isp]")
{
    esp_isp_processor_cfg_t isp_config = {
        .clk_hz = 80 * 1000 * 1000,
        .input_data_source = ISP_INPUT_DATA_SOURCE_CSI,
        .input_data_color_type = ISP_COLOR_RAW8,
        .output_data_color_type = ISP_COLOR_RGB565,
    };
    isp_proc_handle_t isp_proc = NULL;
    TEST_ESP_OK(esp_isp_new_processor(&isp_config, &isp_proc));

    esp_isp_ae_config_t ae_config = {
        .sample_point = ISP_AE_SAMPLE_POINT_AFTER_DEMOSAIC,
    };
    isp_ae_ctlr_t ae_ctrlr[SOC_ISP_AE_CTLR_NUMS + 1] = {};
    for (int i = 0; i < SOC_ISP_AE_CTLR_NUMS; i++) {
        TEST_ESP_OK(esp_isp_new_ae_controller(isp_proc, &ae_config, &ae_ctrlr[i]));
    }

    TEST_ASSERT(esp_isp_new_ae_controller(isp_proc, &ae_config, &ae_ctrlr[SOC_ISP_AE_CTLR_NUMS]) == ESP_ERR_NOT_FOUND);

    for (int i = 0; i < SOC_ISP_AE_CTLR_NUMS; i++) {
        TEST_ESP_OK(esp_isp_del_ae_controller(ae_ctrlr[i]));
    }
    TEST_ESP_OK(esp_isp_del_processor(isp_proc));
}

static bool test_isp_hist_default_on_statistics_done_cb(isp_hist_ctlr_t hist_ctlr, const esp_isp_hist_evt_data_t *edata, void *user_data)
{
    (void) hist_ctlr;
    (void) edata;
    (void) user_data;
    // Do nothing
    return false;
}

/*---------------------------------------------------------------
                      HIST
---------------------------------------------------------------*/
TEST_CASE("ISP HIST driver basic function", "[isp]")
{
    esp_isp_processor_cfg_t isp_config = {
        .clk_hz = 80 * 1000 * 1000,
        .input_data_source = ISP_INPUT_DATA_SOURCE_CSI,
        .input_data_color_type = ISP_COLOR_RAW8,
        .output_data_color_type = ISP_COLOR_RGB565,
    };
    isp_proc_handle_t isp_proc = NULL;
    TEST_ESP_OK(esp_isp_new_processor(&isp_config, &isp_proc));
    TEST_ESP_OK(esp_isp_enable(isp_proc));

    isp_hist_ctlr_t hist_ctlr = NULL;
    isp_hist_result_t hist_res = {};

    /* Test when sum of weight is not 256 */
    esp_isp_hist_config_t hist_config_error = {
        .segment_threshold = {16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240},
        .hist_mode = ISP_HIST_SAMPLING_RGB,
        .rgb_coefficient.coeff_r = {{86, 0}},
        .rgb_coefficient.coeff_g = {{85, 0}},
        .rgb_coefficient.coeff_b = {{85, 0}},
        .window_weight = {
            {{15, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}},
            {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}},
            {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}},
            {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}},
            {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}},
        },
    };
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_isp_new_hist_controller(isp_proc, &hist_config_error, &hist_ctlr));

    esp_isp_hist_config_t hist_config = {
        .segment_threshold = {16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240},
        .hist_mode = ISP_HIST_SAMPLING_RGB,
        .rgb_coefficient.coeff_r = {{86, 0}},
        .rgb_coefficient.coeff_g = {{85, 0}},
        .rgb_coefficient.coeff_b = {{85, 0}},
        .window_weight = {
            {{16, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}},
            {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}},
            {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}},
            {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}},
            {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}},
        },
    };
    TEST_ESP_OK(esp_isp_new_hist_controller(isp_proc, &hist_config, &hist_ctlr));

    /* Register HIST callback */
    esp_isp_hist_cbs_t hist_cb = {
        .on_statistics_done = test_isp_hist_default_on_statistics_done_cb,
    };
    TEST_ESP_OK(esp_isp_hist_register_event_callbacks(hist_ctlr, &hist_cb, NULL));
    /* Enabled the hist controller */
    TEST_ESP_OK(esp_isp_hist_controller_enable(hist_ctlr));
    /* Start continuous HIST statistics */
    TEST_ESP_OK(esp_isp_hist_controller_start_continuous_statistics(hist_ctlr));
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_isp_hist_controller_get_oneshot_statistics(hist_ctlr, 0, &hist_res));
    /* Stop continuous HIST statistics */
    TEST_ESP_OK(esp_isp_hist_controller_stop_continuous_statistics(hist_ctlr));
    TEST_ESP_ERR(ESP_ERR_TIMEOUT, esp_isp_hist_controller_get_oneshot_statistics(hist_ctlr, 1, &hist_res));
    /* Disable the hist controller */
    TEST_ESP_OK(esp_isp_hist_controller_disable(hist_ctlr));
    /* Delete the hist controller and free the resources */
    TEST_ESP_OK(esp_isp_del_hist_controller(hist_ctlr));

    TEST_ESP_OK(esp_isp_disable(isp_proc));
    TEST_ESP_OK(esp_isp_del_processor(isp_proc));
}

TEST_CASE("ISP HIST controller exhausted allocation", "[isp]")
{
    esp_isp_processor_cfg_t isp_config = {
        .clk_hz = 80 * 1000 * 1000,
        .input_data_source = ISP_INPUT_DATA_SOURCE_CSI,
        .input_data_color_type = ISP_COLOR_RAW8,
        .output_data_color_type = ISP_COLOR_RGB565,
    };
    isp_proc_handle_t isp_proc = NULL;
    TEST_ESP_OK(esp_isp_new_processor(&isp_config, &isp_proc));

    esp_isp_hist_config_t hist_config = {
        .segment_threshold = {16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240},
        .hist_mode = ISP_HIST_SAMPLING_RGB,
        .rgb_coefficient.coeff_r = {{86, 0}},
        .rgb_coefficient.coeff_g = {{85, 0}},
        .rgb_coefficient.coeff_b = {{85, 0}},
        .window_weight = {
            {{16, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}},
            {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}},
            {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}},
            {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}},
            {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}}, {{10, 0}},
        },
    };

    isp_hist_ctlr_t hist_ctlr[SOC_ISP_HIST_CTLR_NUMS + 1] = {};
    for (int i = 0; i < SOC_ISP_HIST_CTLR_NUMS; i++) {
        TEST_ESP_OK(esp_isp_new_hist_controller(isp_proc, &hist_config, &hist_ctlr[i]));
    }

    TEST_ASSERT(esp_isp_new_hist_controller(isp_proc, &hist_config, &hist_ctlr[SOC_ISP_HIST_CTLR_NUMS]) == ESP_ERR_NOT_FOUND);

    for (int i = 0; i < SOC_ISP_HIST_CTLR_NUMS; i++) {
        TEST_ESP_OK(esp_isp_del_hist_controller(hist_ctlr[i]));
    }
    TEST_ESP_OK(esp_isp_del_processor(isp_proc));
}
