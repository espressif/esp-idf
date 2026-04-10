/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "esp_cache.h"
#include "driver/i2c_master.h"
#include "esp_cam_ctlr.h"
#include "esp_cam_ctlr_dvp.h"
#include "driver/ledc.h"
#include "example_sensor_init.h"
#include "unity.h"

//----------CAM Config------------//
#define TEST_RGB565_BITS_PER_PIXEL      16

#define TEST_DVP_CAM_SCCB_SCL_IO        (5)
#define TEST_DVP_CAM_SCCB_SDA_IO        (4)
#define TEST_DVP_CAM_XCLK_FREQ_HZ       (20000000)
#define TEST_DVP_CAM_DATA_WIDTH         (8)

#define TEST_DVP_CAM_D0_IO              (11)
#define TEST_DVP_CAM_D1_IO              (9)
#define TEST_DVP_CAM_D2_IO              (8)
#define TEST_DVP_CAM_D3_IO              (10)
#define TEST_DVP_CAM_D4_IO              (12)
#define TEST_DVP_CAM_D5_IO              (18)
#define TEST_DVP_CAM_D6_IO              (17)
#define TEST_DVP_CAM_D7_IO              (16)

#define TEST_DVP_CAM_XCLK_IO            (15)
#define TEST_DVP_CAM_PCLK_IO            (13)
#define TEST_DVP_CAM_DE_IO              (7)
#define TEST_DVP_CAM_VSYNC_IO           (6)
#define TEST_DVP_CAM_HSYNC_IO           (-1)

#define TEST_DVP_CAM_BUF_ALLOC_CAPS     (MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA)

#define TEST_DVP_CAM_H_RES              240
#define TEST_DVP_CAM_V_RES              240
#define TEST_CAM_FORMAT                 "DVP_8bit_20Minput_RGB565_240x240_25fps"

#define BUFFER_SIZE         (TEST_DVP_CAM_H_RES * TEST_DVP_CAM_V_RES * TEST_RGB565_BITS_PER_PIXEL / 8)

typedef struct {
    // esp_lcd_panel_handle_t panel_hdl;
    esp_cam_ctlr_trans_t cam_trans;
} example_cam_context_t;

static bool s_camera_get_new_vb(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data);
static bool s_camera_get_finished_trans(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data);

int count_new_vb = 0;
int count_finished_trans = 0;

TEST_CASE("TEST DVP camera on esp32s3_eye", "[dvp][camera][esp32s3_eye]")
{
    //----------CAM Controller Init------------//
    esp_cam_ctlr_handle_t cam_handle = NULL;
    esp_cam_ctlr_dvp_pin_config_t pin_cfg = {
        .data_width = TEST_DVP_CAM_DATA_WIDTH,
        .data_io = {
            TEST_DVP_CAM_D0_IO,
            TEST_DVP_CAM_D1_IO,
            TEST_DVP_CAM_D2_IO,
            TEST_DVP_CAM_D3_IO,
            TEST_DVP_CAM_D4_IO,
            TEST_DVP_CAM_D5_IO,
            TEST_DVP_CAM_D6_IO,
            TEST_DVP_CAM_D7_IO,
        },
        .vsync_io = TEST_DVP_CAM_VSYNC_IO,
        .de_io = TEST_DVP_CAM_DE_IO,
        .pclk_io = TEST_DVP_CAM_PCLK_IO,
        .xclk_io = TEST_DVP_CAM_XCLK_IO,
    };

    esp_cam_ctlr_dvp_config_t dvp_config = {
        .ctlr_id = 0,
        .clk_src = CAM_CLK_SRC_DEFAULT,
        .h_res = TEST_DVP_CAM_H_RES,
        .v_res = TEST_DVP_CAM_V_RES,
        .input_data_color_type = CAM_CTLR_COLOR_RGB565,
        .output_data_color_type = CAM_CTLR_COLOR_RGB565,
        .dma_burst_size = 64,
        .pin = &pin_cfg,
        .bk_buffer_dis = 1,
        .xclk_freq = TEST_DVP_CAM_XCLK_FREQ_HZ,
    };

    TEST_ESP_OK(esp_cam_new_dvp_ctlr(&dvp_config, &cam_handle));

    //--------Allocate Camera Buffer----------//
    size_t cam_buffer_size = TEST_DVP_CAM_H_RES * TEST_DVP_CAM_V_RES * TEST_RGB565_BITS_PER_PIXEL / 8;
    void *cam_buffer = NULL;

    cam_buffer = esp_cam_ctlr_alloc_buffer(cam_handle, cam_buffer_size, TEST_DVP_CAM_BUF_ALLOC_CAPS);

    //--------Camera Sensor and SCCB Init-----------//
    example_sensor_config_t cam_sensor_config = {
        .i2c_port_num = I2C_NUM_0,
        .i2c_sda_io_num = TEST_DVP_CAM_SCCB_SDA_IO,
        .i2c_scl_io_num = TEST_DVP_CAM_SCCB_SCL_IO,
        .port = ESP_CAM_SENSOR_DVP,
        .format_name = TEST_CAM_FORMAT,
    };
    example_sensor_handle_t sensor_handle = {
        .sccb_handle = NULL,
        .i2c_bus_handle = NULL,
    };
    example_sensor_init(&cam_sensor_config, &sensor_handle);

    //--------Register Camera Callbacks----------//
    example_cam_context_t cam_ctx = {
        // .panel_hdl = lcd_panel_hdl,
        .cam_trans = {
            .buffer = cam_buffer,
            .buflen = cam_buffer_size,
        }
    };

    esp_cam_ctlr_evt_cbs_t cbs = {
        .on_get_new_trans = s_camera_get_new_vb,
        .on_trans_finished = s_camera_get_finished_trans,
    };
    TEST_ESP_OK(esp_cam_ctlr_register_event_callbacks(cam_handle, &cbs, &cam_ctx));

    TEST_ESP_OK(esp_cam_ctlr_enable(cam_handle));
    TEST_ESP_OK(esp_cam_ctlr_start(cam_handle));

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    //For this format"DVP_8bit_20Minput_RGB565_240x240_25fps", the FPS is 25
    TEST_ASSERT_GREATER_OR_EQUAL(25, count_new_vb);
    TEST_ASSERT_GREATER_OR_EQUAL(24, count_finished_trans);

    TEST_ESP_OK(esp_cam_ctlr_stop(cam_handle));
    TEST_ESP_OK(esp_cam_ctlr_disable(cam_handle));
    TEST_ESP_OK(esp_cam_ctlr_del(cam_handle));

    example_sensor_deinit(sensor_handle);
    heap_caps_free(cam_buffer);
}

static bool s_camera_get_new_vb(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data)
{
    example_cam_context_t *ctx = (example_cam_context_t *)user_data;
    *trans = ctx->cam_trans;

    count_new_vb++;

    return false;
}

static bool s_camera_get_finished_trans(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data)
{
    count_finished_trans++;
    return false;
}
