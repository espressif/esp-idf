/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include "sdkconfig.h"
#include "unity.h"
#include "esp_cam_ctlr_dvp.h"
#include "esp_cam_ctlr.h"

TEST_CASE("TEST DVP driver allocation", "[DVP]")
{
    esp_cam_ctlr_dvp_config_t dvp_config = {
        .ctlr_id = 0,
        .clk_src = CAM_CLK_SRC_DEFAULT,
        .h_res = 800,
        .v_res = 640,
        .input_data_color_type = CAM_CTLR_COLOR_RGB565,
        .dma_burst_size = 128,
        .byte_swap_en = false,
        .pin_dont_init = true,
    };
    esp_cam_ctlr_handle_t handle = NULL;
    TEST_ESP_OK(esp_cam_new_dvp_ctlr(&dvp_config, &handle));
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, esp_cam_new_dvp_ctlr(&dvp_config, &handle));

    uint8_t *bk_buffer = NULL;
    size_t bk_buffer_len = 0;
    TEST_ESP_OK(esp_cam_ctlr_get_frame_buffer(handle, 1, (const void **)&bk_buffer));
    TEST_ESP_OK(esp_cam_ctlr_get_frame_buffer_len(handle, &bk_buffer_len));
    TEST_ASSERT_NOT_NULL(bk_buffer);
    TEST_ASSERT_EQUAL((dvp_config.h_res * dvp_config.v_res * 2), bk_buffer_len); // type RGB565 using 2 byte / pixel
    TEST_ESP_OK(esp_cam_ctlr_del(handle));
}

TEST_CASE("TEST DVP driver allocation with JPEG input", "[DVP]")
{
    esp_cam_ctlr_dvp_config_t dvp_config = {
        .ctlr_id = 0,
        .clk_src = CAM_CLK_SRC_DEFAULT,
        .h_res = 800,
        .v_res = 640,
        .dma_burst_size = 128,
        .byte_swap_en = false,
        .pin_dont_init = true,
        .pic_format_jpeg = true,
    };
    esp_cam_ctlr_handle_t handle = NULL;
    TEST_ESP_OK(esp_cam_new_dvp_ctlr(&dvp_config, &handle));
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, esp_cam_new_dvp_ctlr(&dvp_config, &handle));

    uint8_t *bk_buffer = NULL;
    size_t bk_buffer_len = 0;
    TEST_ESP_OK(esp_cam_ctlr_get_frame_buffer(handle, 1, (const void **)&bk_buffer));
    TEST_ESP_OK(esp_cam_ctlr_get_frame_buffer_len(handle, &bk_buffer_len));
    TEST_ASSERT_NOT_NULL(bk_buffer);
    TEST_ASSERT_EQUAL((dvp_config.h_res * dvp_config.v_res * 1), bk_buffer_len); // type JPEG using 1 byte / pixel
    TEST_ESP_OK(esp_cam_ctlr_del(handle));
}

TEST_CASE("TEST DVP driver no backup buffer usage", "[DVP]")
{
    esp_cam_ctlr_dvp_config_t dvp_config = {
        .ctlr_id = 0,
        .clk_src = CAM_CLK_SRC_DEFAULT,
        .h_res = 800,
        .v_res = 640,
        .input_data_color_type = CAM_CTLR_COLOR_RGB565,
        .dma_burst_size = 128,
        .byte_swap_en = false,
        .bk_buffer_dis = true,
        .pin_dont_init = true,
    };
    esp_cam_ctlr_handle_t handle = NULL;
    TEST_ESP_OK(esp_cam_new_dvp_ctlr(&dvp_config, &handle));
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, esp_cam_new_dvp_ctlr(&dvp_config, &handle));

    uint8_t *bk_buffer = NULL;
    size_t bk_buffer_len = 0;
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, esp_cam_ctlr_get_frame_buffer(handle, 1, (const void **)&bk_buffer));
    TEST_ESP_OK(esp_cam_ctlr_get_frame_buffer_len(handle, &bk_buffer_len));
    TEST_ASSERT_NULL(bk_buffer);
    TEST_ASSERT_EQUAL((dvp_config.h_res * dvp_config.v_res * 2), bk_buffer_len); // out type RGB565 using 2 byte / pixel
    TEST_ESP_OK(esp_cam_ctlr_del(handle));
}
