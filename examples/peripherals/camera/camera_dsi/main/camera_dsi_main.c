/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_attr.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_ili9881c.h"
#include "esp_ldo_regulator.h"
#include "esp_cache.h"
#include "driver/i2c_master.h"
#include "driver/isp.h"
#include "esp_cam_ctlr_csi.h"
#include "esp_cam_ctlr.h"
#include "esp_sccb_intf.h"
#include "esp_sccb_i2c.h"
#include "esp_cam_sensor.h"
#include "ov5647.h"
#include "example_dsi_init.h"
#include "example_config.h"

static const char *TAG = "cam_dsi";

static bool s_camera_get_new_vb(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data);
static bool s_camera_get_finished_trans(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data);

void app_main(void)
{
    esp_err_t ret = ESP_FAIL;
    esp_lcd_panel_handle_t ili9881c_ctrl_panel = NULL;
    esp_lcd_panel_handle_t mipi_dpi_panel = NULL;
    void *frame_buffer = NULL;
    size_t frame_buffer_size = 0;

    //mipi ldo
    esp_ldo_channel_handle_t ldo_mipi_phy = NULL;
    esp_ldo_channel_config_t ldo_mipi_phy_config = {
        .chan_id = CONFIG_EXAMPLE_USED_LDO_CHAN_ID,
        .voltage_mv = CONFIG_EXAMPLE_USED_LDO_VOLTAGE_MV,
    };
    ESP_ERROR_CHECK(esp_ldo_acquire_channel(&ldo_mipi_phy_config, &ldo_mipi_phy));

    /**
     * @background
     * Sensor use RAW8
     * ISP convert to RGB565
     */
    //---------------DSI Init------------------//
    example_dsi_resource_alloc(&ili9881c_ctrl_panel, &mipi_dpi_panel, &frame_buffer);

    //---------------Necessary variable config------------------//
    frame_buffer_size = EXAMPLE_MIPI_CSI_DISP_HSIZE * EXAMPLE_MIPI_DSI_IMAGE_VSIZE * EXAMPLE_RGB565_BITS_PER_PIXEL / 8;

    ESP_LOGD(TAG, "EXAMPLE_MIPI_CSI_DISP_HSIZE: %d, EXAMPLE_MIPI_DSI_IMAGE_VSIZE: %d, bits per pixel: %d", EXAMPLE_MIPI_CSI_DISP_HSIZE, EXAMPLE_MIPI_DSI_IMAGE_VSIZE, 8);
    ESP_LOGD(TAG, "frame_buffer_size: %zu", frame_buffer_size);
    ESP_LOGD(TAG, "frame_buffer: %p", frame_buffer);

    esp_cam_ctlr_trans_t new_trans = {
        .buffer = frame_buffer,
        .buflen = frame_buffer_size,
    };

    //---------------I2C Init------------------//
    i2c_master_bus_config_t i2c_bus_conf = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .sda_io_num = EXAMPLE_MIPI_SCCB_SDA_IO,
        .scl_io_num = EXAMPLE_MIPI_SCCB_SCL_IO,
        .i2c_port = I2C_NUM_0,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus_handle = NULL;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_conf, &bus_handle));

    //---------------SCCB Init------------------//
    esp_sccb_io_handle_t ov5647_io_handle = NULL;
    sccb_i2c_config_t i2c_config = {
        .scl_speed_hz = EXAMPLE_MIPI_SCCB_FREQ,
        .device_address = EXAMPLE_OV5647_DEV_ADDR,
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    };
    ESP_ERROR_CHECK(sccb_new_i2c_io(bus_handle, &i2c_config, &ov5647_io_handle));

    //---------------CSI Init------------------//
    esp_cam_ctlr_csi_config_t csi_config = {
        .ctlr_id = 0,
        .h_res = EXAMPLE_MIPI_CSI_DISP_HSIZE,
#if CONFIG_EXAMPLE_MIPI_CSI_VRES_640
        .v_res = EXAMPLE_MIPI_CSI_DISP_VSIZE_640P,
#else
        .v_res = EXAMPLE_MIPI_CSI_DISP_VSIZE_1280P,
#endif
        .lane_bit_rate_mbps = EXAMPLE_MIPI_CSI_LANE_BITRATE_MBPS,
        .input_data_color_type = MIPI_CSI_COLOR_RAW8,
        .output_data_color_type = MIPI_CSI_COLOR_RGB565,
        .data_lane_num = 2,
        .byte_swap_en = false,
        .queue_items = 1,
    };
    esp_cam_ctlr_handle_t cam_handle = NULL;
    ret = esp_cam_new_csi_ctlr(&csi_config, &cam_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "csi init fail[%d]", ret);
        return;
    }

    esp_cam_ctlr_evt_cbs_t cbs = {
        .on_get_new_trans = s_camera_get_new_vb,
        .on_trans_finished = s_camera_get_finished_trans,
    };
    if (esp_cam_ctlr_register_event_callbacks(cam_handle, &cbs, &new_trans) != ESP_OK) {
        ESP_LOGE(TAG, "ops register fail");
        return;
    }

    ESP_ERROR_CHECK(esp_cam_ctlr_enable(cam_handle));

    //---------------ISP Init------------------//
    isp_proc_handle_t isp_proc = NULL;
    esp_isp_processor_cfg_t isp_config = {
        .clk_hz = 80 * 1000 * 1000,
        .input_data_source = ISP_INPUT_DATA_SOURCE_CSI,
        .input_data_color_type = ISP_COLOR_RAW8,
        .output_data_color_type = ISP_COLOR_RGB565,
        .has_line_start_packet = false,
        .has_line_end_packet = false,
        .h_res = EXAMPLE_MIPI_CSI_DISP_HSIZE,
#if CONFIG_EXAMPLE_MIPI_CSI_VRES_640
        .v_res = EXAMPLE_MIPI_CSI_DISP_VSIZE_640P,
#else
        .v_res = EXAMPLE_MIPI_CSI_DISP_VSIZE_1280P,
#endif
    };
    ESP_ERROR_CHECK(esp_isp_new_processor(&isp_config, &isp_proc));
    ESP_ERROR_CHECK(esp_isp_enable(isp_proc));

    //---------------DSI Panel Init------------------//
    example_dsi_ili9881c_panel_init(ili9881c_ctrl_panel);

    //init to all white
    memset(frame_buffer, 0xFF, frame_buffer_size);
    esp_cache_msync((void *)frame_buffer, frame_buffer_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);

    if (esp_cam_ctlr_start(cam_handle) != ESP_OK) {
        ESP_LOGE(TAG, "Driver start fail");
        return;
    }

    esp_cam_sensor_config_t cam_config = {
        .sccb_handle = ov5647_io_handle,
        .reset_pin = -1,
        .pwdn_pin = -1,
        .xclk_pin = -1,
        .sensor_port = ESP_CAM_SENSOR_MIPI_CSI,
    };

    esp_cam_sensor_device_t *cam = ov5647_detect(&cam_config);
    if (!cam) {
        ESP_LOGE(TAG, "failed to detect 5647");
        return;
    }

    esp_cam_sensor_format_array_t cam_fmt_array = {0};
    esp_cam_sensor_query_format(cam, &cam_fmt_array);
    const esp_cam_sensor_format_t *parray = cam_fmt_array.format_array;
    for (int i = 0; i < cam_fmt_array.count; i++) {
        ESP_LOGI(TAG, "fmt[%d].name:%s", i, parray[i].name);
    }

    esp_cam_sensor_format_t *cam_cur_fmt = NULL;
    for (int i = 0; i < cam_fmt_array.count; i++) {

#if CONFIG_EXAMPLE_MIPI_CSI_VRES_640
        if (!strcmp(parray[i].name, "MIPI_2lane_24Minput_RAW8_800x640_50fps")) {
            cam_cur_fmt = (esp_cam_sensor_format_t *) & (parray[i].name);
        }
#else
        if (!strcmp(parray[i].name, "MIPI_2lane_24Minput_RAW8_800x1280_50fps")) {
            cam_cur_fmt = (esp_cam_sensor_format_t *) & (parray[i].name);
        }
#endif
    }

    ret = esp_cam_sensor_set_format(cam, (const esp_cam_sensor_format_t *) cam_cur_fmt);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Format set fail");
    } else {
        ESP_LOGI(TAG, "Format in use:%s", cam_cur_fmt->name);
    }
    int enable_flag = 1;
    // Set sensor output stream
    ret = esp_cam_sensor_ioctl(cam, ESP_CAM_SENSOR_IOC_S_STREAM, &enable_flag);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Start stream fail");
    }

    example_dpi_panel_init(mipi_dpi_panel);

    while (1) {
        ESP_ERROR_CHECK(esp_cam_ctlr_receive(cam_handle, &new_trans, ESP_CAM_CTLR_MAX_DELAY));
    }
}

static bool s_camera_get_new_vb(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data)
{
    esp_cam_ctlr_trans_t new_trans = *(esp_cam_ctlr_trans_t *)user_data;
    trans->buffer = new_trans.buffer;
    trans->buflen = new_trans.buflen;

    return false;
}

static bool s_camera_get_finished_trans(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data)
{
    return false;
}
