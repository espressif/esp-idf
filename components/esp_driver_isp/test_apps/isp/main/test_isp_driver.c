/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include "sdkconfig.h"
#include "unity.h"
#include "driver/isp.h"
#include "esp_private/isp_dvp_private.h"
#include "soc/soc_caps.h"

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

#if SOC_ISP_DVP_SUPPORTED
TEST_CASE("ISP DVP controller exhausted allocation", "[isp]")
{
    esp_isp_processor_cfg_t isp_config = {
        .clk_hz = 120 * 1000 * 1000,
        .input_data_source = ISP_INPUT_DATA_SOURCE_DVP,
        .input_data_color_type = ISP_COLOR_RAW8,
        .output_data_color_type = ISP_COLOR_RGB565,
        .has_line_start_packet = false,
        .has_line_end_packet = false,
        .h_res = 800,
        .v_res = 640,
    };
    isp_proc_handle_t isp_proc = NULL;
    TEST_ESP_OK(esp_isp_new_processor(&isp_config, &isp_proc));

    esp_cam_ctlr_isp_dvp_cfg_t dvp_ctlr_config = {
        .data_width = 8,
        .data_io = {53, 54, 52, 0, 1, 45, 46, 47, -1, -1, -1, -1, -1, -1, -1, -1},
        .pclk_io = 21,
        .hsync_io = 5,
        .vsync_io = 23,
        .de_io = 22,
        .io_flags.vsync_invert = 1,
    };
    esp_cam_ctlr_handle_t dvp_ctrlr[SOC_ISP_DVP_CTLR_NUMS + 1] = {};
    for (int i = 0; i < SOC_ISP_DVP_CTLR_NUMS; i++) {
        TEST_ESP_OK(esp_isp_new_dvp_controller(isp_proc, &dvp_ctlr_config, &dvp_ctrlr[i]));
    }

    TEST_ASSERT(esp_isp_new_dvp_controller(isp_proc, &dvp_ctlr_config, &dvp_ctrlr[SOC_ISP_DVP_CTLR_NUMS]) == ESP_ERR_NOT_FOUND);

    for (int i = 0; i < SOC_ISP_DVP_CTLR_NUMS; i++) {
        TEST_ESP_OK(esp_isp_del_dvp_controller(dvp_ctrlr[i]));
    }
    TEST_ESP_OK(esp_isp_del_processor(isp_proc));
}
#endif

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
