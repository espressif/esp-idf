/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "unity.h"
#include "driver/i2c_master.h"
#include "esp_attr.h"
#include "esp_heap_caps.h"
#include "esp_cam_ctlr.h"
#include "esp_cam_ctlr_dvp.h"
#include "example_sensor_init.h"
#include "test_dvp_ov2640_board.h"

static int s_new_trans_count;
static int s_trans_finished_count;

static bool IRAM_ATTR camera_get_new_buffer(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data)
{
    (void)handle;
    esp_cam_ctlr_trans_t *new_trans = (esp_cam_ctlr_trans_t *)user_data;
    trans->buffer = new_trans->buffer;
    trans->buflen = new_trans->buflen;
    s_new_trans_count++;
    return false;
}

static bool IRAM_ATTR camera_trans_finished(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data)
{
    (void)handle;
    (void)trans;
    (void)user_data;
    s_trans_finished_count++;
    return false;
}

TEST_CASE("TEST esp_cam DVP on ov2640", "[dvp][camera][ov2640]")
{
    s_new_trans_count = 0;
    s_trans_finished_count = 0;

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
        .h_res = TEST_DVP_CAM_FRAME_WIDTH,
        .v_res = TEST_DVP_CAM_FRAME_HEIGHT,
        .input_data_color_type = TEST_DVP_CAM_INPUT_COLOR_TYPE,
        .output_data_color_type = TEST_DVP_CAM_OUTPUT_COLOR_TYPE,
        .dma_burst_size = 64,
        .pin = &pin_cfg,
        .bk_buffer_dis = 1,
        .xclk_freq = TEST_DVP_CAM_XCLK_FREQ_HZ,
        .cam_data_width = TEST_DVP_CAM_DATA_WIDTH,
        .byte_swap_en = false,
        .bit_swap_en = false,
    };
    esp_cam_ctlr_handle_t cam_handle = NULL;
    TEST_ESP_OK(esp_cam_new_dvp_ctlr(&dvp_config, &cam_handle));

    size_t frame_buffer_size = TEST_DVP_CAM_FRAME_SIZE_BYTES;
    void *frame_buffer = esp_cam_ctlr_alloc_buffer(cam_handle, frame_buffer_size, TEST_DVP_CAM_BUF_ALLOC_CAPS);
    TEST_ASSERT_NOT_NULL(frame_buffer);

    esp_cam_ctlr_trans_t trans_data = {
        .buffer = frame_buffer,
        .buflen = frame_buffer_size,
    };

    example_sensor_handle_t sensor_handle = {
        .sccb_handle = NULL,
        .i2c_bus_handle = NULL,
    };
    example_sensor_config_t sensor_config = {
        .i2c_port_num = I2C_NUM_0,
        .i2c_sda_io_num = TEST_DVP_CAM_SCCB_SDA_IO,
        .i2c_scl_io_num = TEST_DVP_CAM_SCCB_SCL_IO,
        .reset_pin = TEST_DVP_CAM_RESET_IO,
        .pwdn_pin = TEST_DVP_CAM_PWDN_IO,
        .xclk_pin = TEST_DVP_CAM_XCLK_IO,
        .port = ESP_CAM_SENSOR_DVP,
        .format_name = TEST_DVP_CAM_FORMAT_NAME,
    };
    example_sensor_init(&sensor_config, &sensor_handle);
    if (sensor_handle.sccb_handle == NULL || sensor_handle.i2c_bus_handle == NULL) {
        TEST_ESP_OK(esp_cam_ctlr_del(cam_handle));
        heap_caps_free(frame_buffer);
        TEST_FAIL_MESSAGE("failed to detect or initialize ov2640 sensor");
    }

    esp_cam_ctlr_evt_cbs_t cbs = {
        .on_get_new_trans = camera_get_new_buffer,
        .on_trans_finished = camera_trans_finished,
    };
    TEST_ESP_OK(esp_cam_ctlr_register_event_callbacks(cam_handle, &cbs, &trans_data));

    TEST_ESP_OK(esp_cam_ctlr_enable(cam_handle));
    TEST_ESP_OK(esp_cam_ctlr_start(cam_handle));
    vTaskDelay(pdMS_TO_TICKS(1000));
    TEST_ESP_OK(esp_cam_ctlr_stop(cam_handle));

    // Roughly one transfer per frame is expected for the selected OV2640 format.
    TEST_ASSERT_GREATER_OR_EQUAL(TEST_DVP_CAM_EXPECTED_NEW_TRANS_MIN, s_new_trans_count);
    TEST_ASSERT_GREATER_OR_EQUAL(TEST_DVP_CAM_EXPECTED_FINISHED_MIN, s_trans_finished_count);

    TEST_ESP_OK(esp_cam_ctlr_disable(cam_handle));
    TEST_ESP_OK(esp_cam_ctlr_del(cam_handle));

    example_sensor_deinit(sensor_handle);
    heap_caps_free(frame_buffer);
}
