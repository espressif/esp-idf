/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include "sdkconfig.h"
#include "unity.h"
#include "esp_cam_ctlr_csi.h"
#include "esp_cam_ctlr.h"

TEST_CASE("TEST CSI driver allocation", "[csi]")
{
    esp_cam_ctlr_csi_config_t csi_config = {
        .ctlr_id = 0,
        .h_res = 800,
        .v_res = 640,
        .clk_freq_hz = 200000000,
        .input_data_color_type = MIPI_CSI_COLOR_RAW8,
        .output_data_color_type = MIPI_CSI_COLOR_RGB565,
        .data_lane_num = 2,
        .byte_swap_en = false,
        .queue_items = 1,
    };
    esp_cam_ctlr_handle_t handle = NULL;
    TEST_ESP_OK(esp_cam_new_csi_ctlr(&csi_config, &handle));
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, esp_cam_new_csi_ctlr(&csi_config, &handle));
    TEST_ESP_OK(esp_cam_del_ctlr(handle));
}
