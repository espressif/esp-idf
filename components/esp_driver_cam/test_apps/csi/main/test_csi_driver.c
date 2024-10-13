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
        .lane_bit_rate_mbps = 200,
        .input_data_color_type = CAM_CTLR_COLOR_RAW8,
        .output_data_color_type = CAM_CTLR_COLOR_RGB565,
        .data_lane_num = 2,
        .byte_swap_en = false,
        .queue_items = 1,
    };
    esp_cam_ctlr_handle_t handle = NULL;
    TEST_ESP_OK(esp_cam_new_csi_ctlr(&csi_config, &handle));
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, esp_cam_new_csi_ctlr(&csi_config, &handle));

    uint8_t *bk_buffer = NULL;
    size_t bk_buffer_len = 0;
    TEST_ESP_OK(esp_cam_ctlr_get_frame_buffer(handle, 1, (const void **)&bk_buffer));
    TEST_ESP_OK(esp_cam_ctlr_get_frame_buffer_len(handle, &bk_buffer_len));
    TEST_ASSERT_NOT_NULL(bk_buffer);
    TEST_ASSERT_EQUAL((csi_config.h_res * csi_config.v_res * 2), bk_buffer_len); // out type RGB565 using 2 byte / pixel
    TEST_ESP_OK(esp_cam_ctlr_del(handle));
}

TEST_CASE("TEST CSI driver no backup buffer usage", "[csi]")
{
    esp_cam_ctlr_csi_config_t csi_config = {
        .ctlr_id = 0,
        .h_res = 800,
        .v_res = 640,
        .lane_bit_rate_mbps = 200,
        .input_data_color_type = CAM_CTLR_COLOR_RAW8,
        .output_data_color_type = CAM_CTLR_COLOR_RGB565,
        .data_lane_num = 2,
        .byte_swap_en = false,
        .queue_items = 1,
        .bk_buffer_dis = 1,
    };
    esp_cam_ctlr_handle_t handle = NULL;
    TEST_ESP_OK(esp_cam_new_csi_ctlr(&csi_config, &handle));
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, esp_cam_new_csi_ctlr(&csi_config, &handle));

    uint8_t *bk_buffer = NULL;
    size_t bk_buffer_len = 0;
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_cam_ctlr_get_frame_buffer(handle, 1, (const void **)&bk_buffer));
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_cam_ctlr_get_frame_buffer_len(handle, &bk_buffer_len));
    TEST_ASSERT_NULL(bk_buffer);
    TEST_ASSERT_EQUAL(0, bk_buffer_len);
    TEST_ESP_OK(esp_cam_ctlr_del(handle));
}
