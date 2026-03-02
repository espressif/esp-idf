/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "unity.h"
#include "esp_cam_ctlr_csi.h"
#include "esp_cam_ctlr.h"
#include "esp_ldo_regulator.h"
#include "sdkconfig.h"
#include "driver/isp.h"
#include "example_sensor_init.h"
#include "esp_private/esp_cache_private.h"

#define TEST_USED_LDO_CHAN_ID               3
#define TEST_USED_LDO_VOLTAGE_MV            2500

#define TEST_RGB565_BITS_PER_PIXEL          16
#define TEST_MIPI_CSI_LANE_BITRATE_MBPS     200

#define TEST_MIPI_CSI_CAM_SCCB_SCL_IO       8
#define TEST_MIPI_CSI_CAM_SCCB_SDA_IO       7

#define TEST_MIPI_DSI_DISP_HRES             800
#define TEST_MIPI_DSI_DISP_VRES             1280
#define TEST_MIPI_CSI_DISP_HRES             800
#define TEST_MIPI_CSI_DISP_VRES             640

#define TEST_CAM_FORMAT                     "MIPI_2lane_24Minput_RAW8_800x640_50fps"

static int new_buffer_count = 0;
static int trans_finished_count = 0;

static bool IRAM_ATTR camera_get_new_buffer(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data)
{
    esp_cam_ctlr_trans_t *new_trans = (esp_cam_ctlr_trans_t *)user_data;
    trans->buffer = new_trans->buffer;
    trans->buflen = new_trans->buflen;
    new_buffer_count++;
    return false;
}

static bool IRAM_ATTR camera_trans_finished(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data)
{
    trans_finished_count++;
    return false;
}

TEST_CASE("TEST esp_cam on ov5647", "[csi][camera][ov5647]")
{
    new_buffer_count = 0;
    trans_finished_count = 0;

    size_t frame_buffer_size = TEST_MIPI_CSI_DISP_HRES *
                               TEST_MIPI_DSI_DISP_VRES *
                               TEST_RGB565_BITS_PER_PIXEL / 8;

    //mipi ldo
    esp_ldo_channel_handle_t ldo_mipi_phy = NULL;
    esp_ldo_channel_config_t ldo_config = {
        .chan_id   = TEST_USED_LDO_CHAN_ID,
        .voltage_mv = TEST_USED_LDO_VOLTAGE_MV,
    };
    TEST_ESP_OK(esp_ldo_acquire_channel(&ldo_config, &ldo_mipi_phy));

    size_t frame_buffer_alignment = 0;
    TEST_ESP_OK(esp_cache_get_alignment(0, &frame_buffer_alignment));
    void *frame_buffer = heap_caps_aligned_calloc(frame_buffer_alignment, 1, frame_buffer_size,
                                                  MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

    esp_cam_ctlr_trans_t trans_data = {
        .buffer = frame_buffer,
        .buflen = frame_buffer_size,
    };

    //--------Camera Sensor and SCCB Init-----------//
    example_sensor_handle_t sensor_handle = {
        .sccb_handle = NULL,
        .i2c_bus_handle = NULL,
    };
    example_sensor_config_t sensor_config = {
        .i2c_port_num  = I2C_NUM_0,
        .i2c_sda_io_num = TEST_MIPI_CSI_CAM_SCCB_SDA_IO,
        .i2c_scl_io_num = TEST_MIPI_CSI_CAM_SCCB_SCL_IO,
        .port          = ESP_CAM_SENSOR_MIPI_CSI,
        .format_name   = TEST_CAM_FORMAT,
    };
    example_sensor_init(&sensor_config, &sensor_handle);

    //---------------CSI Init------------------//
    esp_cam_ctlr_csi_config_t csi_config = {
        .ctlr_id                = 0,
        .h_res                  = TEST_MIPI_CSI_DISP_HRES,
        .v_res                  = TEST_MIPI_CSI_DISP_VRES,
        .lane_bit_rate_mbps     = TEST_MIPI_CSI_LANE_BITRATE_MBPS,
        .input_data_color_type  = CAM_CTLR_COLOR_RAW8,
        .output_data_color_type = CAM_CTLR_COLOR_RGB565,
        .data_lane_num          = 2,
        .byte_swap_en           = false,
        .queue_items            = 1,
    };
    esp_cam_ctlr_handle_t cam_handle = NULL;
    TEST_ESP_OK(esp_cam_new_csi_ctlr(&csi_config, &cam_handle));

    esp_cam_ctlr_evt_cbs_t cbs = {
        .on_get_new_trans   = camera_get_new_buffer,
        .on_trans_finished  = camera_trans_finished,
    };
    TEST_ESP_OK(esp_cam_ctlr_register_event_callbacks(cam_handle, &cbs, &trans_data));

    TEST_ESP_OK(esp_cam_ctlr_enable(cam_handle));

    //---------------ISP Init------------------//
    isp_proc_handle_t isp_proc = NULL;
    esp_isp_processor_cfg_t isp_config = {
        .clk_hz                = 80 * 1000 * 1000,
        .input_data_source     = ISP_INPUT_DATA_SOURCE_CSI,
        .input_data_color_type = ISP_COLOR_RAW8,
        .output_data_color_type = ISP_COLOR_RGB565,
        .has_line_start_packet = false,
        .has_line_end_packet   = false,
        .h_res                 = TEST_MIPI_CSI_DISP_HRES,
        .v_res                 = TEST_MIPI_CSI_DISP_VRES,
    };
    TEST_ESP_OK(esp_isp_new_processor(&isp_config, &isp_proc));

    TEST_ESP_OK(esp_isp_enable(isp_proc));
    TEST_ESP_OK(esp_cam_ctlr_start(cam_handle));
    TEST_ESP_OK(esp_cam_ctlr_receive(cam_handle, &trans_data, ESP_CAM_CTLR_MAX_DELAY));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    TEST_ESP_OK(esp_cam_ctlr_stop(cam_handle));

    // At least 35 new buffers callback and 34 completed transfer callback are expected within 1 second when camera is 50fps.
    TEST_ASSERT_GREATER_OR_EQUAL(35, new_buffer_count);
    TEST_ASSERT_GREATER_OR_EQUAL(34, trans_finished_count);

    TEST_ESP_OK(esp_cam_ctlr_disable(cam_handle));
    TEST_ESP_OK(esp_cam_ctlr_del(cam_handle));

    TEST_ESP_OK(esp_isp_disable(isp_proc));
    TEST_ESP_OK(esp_isp_del_processor(isp_proc));

    TEST_ESP_OK(esp_ldo_release_channel(ldo_mipi_phy));

    example_sensor_deinit(sensor_handle);

    heap_caps_free(frame_buffer);
}
