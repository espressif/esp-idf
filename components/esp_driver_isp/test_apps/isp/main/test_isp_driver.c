/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include "sdkconfig.h"
#include "unity.h"
#include "driver/isp.h"

#include "esp_rom_sys.h"

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
    isp_af_ctrlr_t af_ctrlr[SOC_ISP_AF_CTLR_NUMS + 1] = {};
    for (int i = 0; i < SOC_ISP_AF_CTLR_NUMS; i++) {
        TEST_ESP_OK(esp_isp_new_af_controller(isp_proc, &af_config, &af_ctrlr[i]));
    }

    TEST_ASSERT(esp_isp_new_af_controller(isp_proc, &af_config, &af_ctrlr[SOC_ISP_AF_CTLR_NUMS]) == ESP_ERR_NOT_FOUND);

    for (int i = 0; i < SOC_ISP_AF_CTLR_NUMS; i++) {
        TEST_ESP_OK(esp_isp_del_af_controller(af_ctrlr[i]));
    }
    TEST_ESP_OK(esp_isp_del_processor(isp_proc));
}
