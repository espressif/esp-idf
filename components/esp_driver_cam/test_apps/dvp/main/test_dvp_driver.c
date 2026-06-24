/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include "sdkconfig.h"
#include "unity.h"
#include "esp_cam_ctlr_dvp.h"
#include "esp_private/esp_cam_dvp.h"
#include "esp_cam_ctlr.h"

TEST_CASE("TEST DVP driver allocation", "[DVP]")
{
    esp_cam_ctlr_dvp_config_t dvp_config = {
        .ctlr_id = 0,
        .clk_src = CAM_CLK_SRC_DEFAULT,
        .h_res = 800,
        .v_res = 640,
        .input_data_color_type = CAM_CTLR_COLOR_RGB565,
        .output_data_color_type = CAM_CTLR_COLOR_RGB565,
        .dma_burst_size = 64,
        .byte_swap_en = false,
        .pin_dont_init = true,
        .external_xtal = true,
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
        .dma_burst_size = 64,
        .byte_swap_en = false,
        .pin_dont_init = true,
        .pic_format_jpeg = true,
        .external_xtal = true,
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
        .output_data_color_type = CAM_CTLR_COLOR_RGB565,
        .dma_burst_size = 64,
        .byte_swap_en = false,
        .bk_buffer_dis = true,
        .pin_dont_init = true,
        .external_xtal = true,
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

TEST_CASE("TEST DVP driver intern/extern init", "[DVP]")
{
    esp_cam_ctlr_dvp_config_t dvp_config = {
        .ctlr_id = 0,
        .clk_src = CAM_CLK_SRC_DEFAULT,
        .h_res = 800,
        .v_res = 640,
        .input_data_color_type = CAM_CTLR_COLOR_RGB565,
        .output_data_color_type = CAM_CTLR_COLOR_RGB565,
        .dma_burst_size = 64,
        .byte_swap_en = false,
        .external_xtal = true,
    };
    esp_cam_ctlr_handle_t handle = NULL;

    //Init externally, do not check pin
    dvp_config.pin_dont_init = true;
    TEST_ESP_OK(esp_cam_new_dvp_ctlr(&dvp_config, &handle));
    TEST_ESP_OK(esp_cam_ctlr_del(handle));

    //Init internally but not set pin
    dvp_config.pin_dont_init = false;
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_cam_new_dvp_ctlr(&dvp_config, &handle));

    //Init internally and set pin
    esp_cam_ctlr_dvp_pin_config_t pin_cfg = {
        .data_width = 8,
    };
    dvp_config.pin = &pin_cfg;
    TEST_ESP_OK(esp_cam_new_dvp_ctlr(&dvp_config, &handle));
    TEST_ESP_OK(esp_cam_ctlr_del(handle));
}

TEST_CASE("TEST DVP driver intern/extern generate xclk", "[DVP]")
{
    esp_cam_ctlr_dvp_config_t dvp_config = {
        .ctlr_id = 0,
        .clk_src = CAM_CLK_SRC_DEFAULT,
        .h_res = 800,
        .v_res = 640,
        .input_data_color_type = CAM_CTLR_COLOR_RGB565,
        .output_data_color_type = CAM_CTLR_COLOR_RGB565,
        .dma_burst_size = 64,
        .byte_swap_en = false,
        .external_xtal = true,
    };
    esp_cam_ctlr_handle_t handle = NULL;

    //Init externally, generate xclk externally, check nothing
    dvp_config.pin_dont_init = true;
    dvp_config.external_xtal = true;
    TEST_ESP_OK(esp_cam_new_dvp_ctlr(&dvp_config, &handle));
    TEST_ESP_OK(esp_cam_ctlr_del(handle));

    //Init externally, generate xclk internally, do not check pin, check xclk_freq
    dvp_config.pin_dont_init = true;
    dvp_config.external_xtal = false;
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_cam_new_dvp_ctlr(&dvp_config, &handle));
    dvp_config.xclk_freq = 20000000;
    TEST_ESP_OK(esp_cam_new_dvp_ctlr(&dvp_config, &handle));
    TEST_ESP_OK(esp_cam_ctlr_del(handle));

    esp_cam_ctlr_dvp_pin_config_t pin_cfg = {
        .data_width = 8,
        .xclk_io = GPIO_NUM_NC,
    };

    //Init internally, generate xclk externally, check nothing
    dvp_config.pin = &pin_cfg;
    dvp_config.pin_dont_init = false;
    dvp_config.external_xtal = true;
    TEST_ESP_OK(esp_cam_new_dvp_ctlr(&dvp_config, &handle));
    TEST_ESP_OK(esp_cam_ctlr_del(handle));

    //Init internally, generate xclk internally, check xclk_io and xclk_freq
    dvp_config.pin = &pin_cfg;
    dvp_config.pin_dont_init = false;
    dvp_config.external_xtal = false;
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, esp_cam_new_dvp_ctlr(&dvp_config, &handle));

    pin_cfg.xclk_io = 15;
    dvp_config.pin = &pin_cfg;
    TEST_ESP_OK(esp_cam_new_dvp_ctlr(&dvp_config, &handle));
    TEST_ESP_OK(esp_cam_ctlr_del(handle));
}

TEST_CASE("TEST DVP driver only output xclk signal", "[DVP]")
{
    TEST_ESP_OK(esp_cam_ctlr_dvp_start_clock(0, 15, CAM_CLK_SRC_DEFAULT, 20000000));
    TEST_ESP_OK(esp_cam_ctlr_dvp_deinit(0));

#if CONFIG_IDF_TARGET_ESP32S3
    TEST_ESP_OK(esp_cam_ctlr_dvp_start_clock(0, 15, CAM_CLK_SRC_PLL240M, 24000000));
    TEST_ESP_OK(esp_cam_ctlr_dvp_deinit(0));
#endif
}
