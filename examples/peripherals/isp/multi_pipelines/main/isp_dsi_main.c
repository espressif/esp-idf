/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "esp_cache.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_ldo_regulator.h"
#include "driver/i2c_master.h"
#include "esp_cam_ctlr_csi.h"
#include "esp_cam_ctlr.h"
#include "esp_sccb_intf.h"
#include "esp_sccb_i2c.h"
#include "esp_cam_sensor.h"
#include "example_dsi_init.h"
#include "example_dsi_init_config.h"
#include "example_sensor_init.h"
#include "example_config.h"

// Include modular ISP components
#include "example_buffer.h"
#include "example_af.h"
#include "example_awb.h"
#include "example_pipelines.h"
#ifdef CONFIG_EXAMPLE_ISP_CROP_ENABLE
#include "example_crop.h"
#endif

static const char *TAG = "isp_dsi";

/**
 * @brief Camera callback: Get new video buffer
 *
 * This callback is called when CSI needs a new buffer to write frame data.
 */
static bool s_camera_get_new_vb(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data)
{
    example_pingpong_buffer_ctx_t *ctx = (example_pingpong_buffer_ctx_t *)user_data;

    // Provide the current CSI buffer for the next frame
    trans->buffer = example_isp_buffer_get_csi_buffer(ctx);
    trans->buflen = CONFIG_EXAMPLE_MIPI_CSI_DISP_HRES * CONFIG_EXAMPLE_MIPI_DSI_DISP_VRES * EXAMPLE_RGB565_BYTES_PER_PIXEL;

    return false;
}

/**
 * @brief Camera callback: Frame transfer finished
 *
 * This callback is called when CSI finishes writing a frame.
 * It handles buffer swapping and display update.
 */
static bool s_camera_get_finished_trans(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data)
{
    example_pingpong_buffer_ctx_t *ctx = (example_pingpong_buffer_ctx_t *)user_data;

#ifdef CONFIG_EXAMPLE_ISP_CROP_ENABLE
    // Signal frame ready for processing (handled by frame processing task)
    return example_isp_crop_frame_ready_routine(ctx, trans->buffer);
#else
    // Simple ping-pong swap and display
    example_isp_buffer_swap(ctx);
    esp_err_t ret = esp_lcd_panel_draw_bitmap(ctx->panel, 0, 0, ctx->h_res, ctx->v_res, trans->buffer);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to draw bitmap: %d", ret);
    }
    return false;
#endif
}

void app_main(void)
{
    esp_err_t ret = ESP_FAIL;
    esp_lcd_dsi_bus_handle_t mipi_dsi_bus = NULL;
    esp_lcd_panel_io_handle_t mipi_dbi_io = NULL;
    esp_lcd_panel_handle_t mipi_dpi_panel = NULL;
    void *fb0 = NULL;
    void *fb1 = NULL;
    size_t frame_buffer_size = 0;

    //---------------MIPI LDO Init------------------//
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
    //---------------DSI Init with Dual Frame Buffers------------------//
    example_dsi_alloc_config_t dsi_alloc_config = {
        .num_fbs = 2,  // Enable dual frame buffers
    };
    example_dsi_resource_alloc(&dsi_alloc_config, &mipi_dsi_bus, &mipi_dbi_io, &mipi_dpi_panel, &fb0, &fb1);

    //---------------Resolution Configuration------------------//
    int display_h_res = CONFIG_EXAMPLE_MIPI_CSI_DISP_HRES;
    int display_v_res = CONFIG_EXAMPLE_MIPI_CSI_DISP_VRES;
    int crop_h_res = display_h_res;
    int crop_v_res = display_v_res;

    frame_buffer_size = CONFIG_EXAMPLE_MIPI_DSI_DISP_HRES * CONFIG_EXAMPLE_MIPI_DSI_DISP_VRES * EXAMPLE_RGB565_BYTES_PER_PIXEL;

    ESP_LOGI(TAG, "Original CSI resolution: %dx%d", CONFIG_EXAMPLE_MIPI_CSI_DISP_HRES, CONFIG_EXAMPLE_MIPI_CSI_DISP_VRES);
    ESP_LOGI(TAG, "Display resolution: %dx%d, bits per pixel: %d", display_h_res, display_v_res, EXAMPLE_RGB565_BITS_PER_PIXEL);
    ESP_LOGI(TAG, "Frame buffers: fb0=%p, fb1=%p", fb0, fb1);

    //---------------Ping-Pong Buffer Init------------------//
    example_pingpong_buffer_ctx_t pp_ctx = {0};
    ret = example_isp_buffer_init(&pp_ctx, fb0, fb1, mipi_dpi_panel,
                                  CONFIG_EXAMPLE_MIPI_DSI_DISP_HRES,
                                  CONFIG_EXAMPLE_MIPI_DSI_DISP_VRES);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize buffer: %d", ret);
        return;
    }

#ifdef CONFIG_EXAMPLE_ISP_CROP_ENABLE
    // Use cropped resolution for frame buffer if crop is enabled
    crop_h_res = CONFIG_EXAMPLE_ISP_CROP_BOTTOM_RIGHT_H - CONFIG_EXAMPLE_ISP_CROP_TOP_LEFT_H + 1;
    crop_v_res = CONFIG_EXAMPLE_ISP_CROP_BOTTOM_RIGHT_V - CONFIG_EXAMPLE_ISP_CROP_TOP_LEFT_V + 1;

    ret = example_isp_crop_init(&pp_ctx, crop_h_res, crop_v_res);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize cropping: %d", ret);
        return;
    }

    // Start frame processing task
    ret = example_isp_crop_start_frame_processing_task(&pp_ctx, 6, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start frame processing task: %d", ret);
        return;
    }
#endif

    //---------------Camera Sensor and SCCB Init------------------//
    example_sensor_handle_t sensor_handle = {
        .sccb_handle = NULL,
        .i2c_bus_handle = NULL,
    };
    example_sensor_config_t cam_sensor_config = {
        .i2c_port_num = I2C_NUM_0,
        .i2c_sda_io_num = EXAMPLE_MIPI_CSI_CAM_SCCB_SDA_IO,
        .i2c_scl_io_num = EXAMPLE_MIPI_CSI_CAM_SCCB_SCL_IO,
        .port = ESP_CAM_SENSOR_MIPI_CSI,
        .format_name = EXAMPLE_CAM_FORMAT,
    };
    example_sensor_init(&cam_sensor_config, &sensor_handle);

    //---------------VCM SCCB Init------------------//
    esp_sccb_io_handle_t dw9714_io_handle = NULL;
    sccb_i2c_config_t i2c_config = {
        .scl_speed_hz = EXAMPLE_MIPI_SCCB_FREQ,
        .device_address = EXAMPLE_DW9714_DEV_ADDR,
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    };
    ESP_ERROR_CHECK(sccb_new_i2c_io(sensor_handle.i2c_bus_handle, &i2c_config, &dw9714_io_handle));

    //---------------CSI Init------------------//
    esp_cam_ctlr_csi_config_t csi_config = {
        .ctlr_id = 0,
        .h_res = crop_h_res,
        .v_res = crop_v_res,
        .lane_bit_rate_mbps = EXAMPLE_MIPI_CSI_LANE_BITRATE_MBPS,
        .input_data_color_type = CAM_CTLR_COLOR_RAW8,
        .output_data_color_type = CAM_CTLR_COLOR_RGB565,
        .data_lane_num = 2,
        .byte_swap_en = false,
        .queue_items = 1,
    };
    esp_cam_ctlr_handle_t handle = NULL;
    ret = esp_cam_new_csi_ctlr(&csi_config, &handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "CSI init fail[%d]", ret);
        return;
    }

    esp_cam_ctlr_evt_cbs_t cbs = {
        .on_get_new_trans = s_camera_get_new_vb,
        .on_trans_finished = s_camera_get_finished_trans,
    };
    if (esp_cam_ctlr_register_event_callbacks(handle, &cbs, &pp_ctx) != ESP_OK) {
        ESP_LOGE(TAG, "Camera callbacks register fail");
        return;
    }

    ESP_ERROR_CHECK(esp_cam_ctlr_enable(handle));

    //---------------ISP Processor Init------------------//
    isp_proc_handle_t isp_proc = NULL;
    esp_isp_processor_cfg_t isp_config = {
        .clk_hz = 80 * 1000 * 1000,
        .input_data_source = ISP_INPUT_DATA_SOURCE_CSI,
        .input_data_color_type = ISP_COLOR_RAW8,
        .output_data_color_type = ISP_COLOR_RGB565,
        .has_line_start_packet = false,
        .has_line_end_packet = false,
        .h_res = CONFIG_EXAMPLE_MIPI_CSI_DISP_HRES,
        .v_res = CONFIG_EXAMPLE_MIPI_CSI_DISP_VRES,
    };
    ret = example_create_isp_processor(&isp_config, &isp_proc);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "ISP pipeline init fail[%d]", ret);
        return;
    }

    //---------------ISP Pipeline Configuration------------------//
    // Initialize all ISP processing modules
    ret = example_isp_init_all_pipelines(isp_proc);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "ISP pipeline modules init fail[%d]", ret);
        return;
    }

#if CONFIG_ESP32P4_REV_MIN_FULL >= 300
    //---------------White Balance Init------------------//
    ret = example_isp_awb_init(isp_proc);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "AWB init fail[%d]", ret);
        return;
    }

    ret = example_isp_awb_start(isp_proc);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "AWB enable fail[%d]", ret);
        return;
    }
#endif /* CONFIG_ESP32P4_REV_MIN_FULL >= 300 */

    //---------------AF Init and Start------------------//
    ret = example_isp_af_init(isp_proc, dw9714_io_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "AF init fail[%d]", ret);
        return;
    }

    ret = example_isp_af_start(5, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "AF start fail[%d]", ret);
        return;
    }

    //---------------DPI Reset and Init------------------//
    example_dpi_panel_reset(mipi_dpi_panel);

    // Initialize both frame buffers to white
    memset(fb0, 0xFF, frame_buffer_size);
    memset(fb1, 0xFF, frame_buffer_size);
    esp_cache_msync((void *)fb0, frame_buffer_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);
    esp_cache_msync((void *)fb1, frame_buffer_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);

    //---------------Start Camera------------------//
    if (esp_cam_ctlr_start(handle) != ESP_OK) {
        ESP_LOGE(TAG, "Camera start fail");
        return;
    }

    example_dpi_panel_init(mipi_dpi_panel);

    ESP_LOGI(TAG, "ISP DSI example started");

    // Main loop: receive frames
    while (1) {
        esp_cam_ctlr_trans_t new_trans = {
            .buffer = example_isp_buffer_get_csi_buffer(&pp_ctx),
            .buflen = frame_buffer_size,
        };
        ESP_ERROR_CHECK(esp_cam_ctlr_receive(handle, &new_trans, ESP_CAM_CTLR_MAX_DELAY));
    }
}
