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
#include "esp_lcd_mipi_dsi.h"
#include "esp_lcd_panel_ops.h"
#include "esp_ldo_regulator.h"
#include "esp_cache.h"
#include "driver/i2c_master.h"
#include "esp_cam_ctlr.h"
#include "esp_cam_ctlr_dvp.h"
#include "example_dsi_init.h"
#include "example_dsi_init_config.h"
#include "example_sensor_init.h"
#include "example_config.h"

static const char *TAG = "dvp_dsi";

typedef struct {
    esp_lcd_panel_handle_t panel_hdl;
    void *cam_buf;
} display_update_param_t;

static bool s_camera_get_new_vb(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data);
static bool s_camera_get_finished_trans(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data);

static bool example_display_update_ready(esp_lcd_panel_handle_t panel, esp_lcd_dpi_panel_event_data_t *edata, void *user_ctx)
{
    display_update_param_t *param = (display_update_param_t *)user_ctx;
    esp_lcd_panel_draw_bitmap(param->panel_hdl, 0, 0, CONFIG_EXAMPLE_CAM_HRES, CONFIG_EXAMPLE_CAM_VRES, param->cam_buf);
    return false;
}

void app_main(void)
{
    esp_err_t ret = ESP_FAIL;
    esp_lcd_dsi_bus_handle_t mipi_dsi_bus = NULL;
    esp_lcd_panel_io_handle_t mipi_dbi_io = NULL;
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

    //---------------DSI Init------------------//
    example_dsi_resource_alloc(NULL, &mipi_dsi_bus, &mipi_dbi_io, &mipi_dpi_panel, &frame_buffer, NULL);

    //---------------Necessary variable config------------------//
    frame_buffer_size = CONFIG_EXAMPLE_MIPI_DSI_DISP_HRES * CONFIG_EXAMPLE_MIPI_DSI_DISP_VRES * EXAMPLE_RGB565_BYTES_PER_PIXEL;

    ESP_LOGD(TAG, "CONFIG_EXAMPLE_MIPI_DSI_DISP_HRES: %d, CONFIG_EXAMPLE_MIPI_DSI_DISP_VRES: %d, bits per pixel: %d", CONFIG_EXAMPLE_MIPI_DSI_DISP_HRES, CONFIG_EXAMPLE_MIPI_DSI_DISP_VRES, EXAMPLE_RGB565_BITS_PER_PIXEL);
    ESP_LOGD(TAG, "frame_buffer_size: %zu", frame_buffer_size);
    ESP_LOGD(TAG, "frame_buffer: %p", frame_buffer);

    //----------CAM Controller Init------------//
    esp_cam_ctlr_handle_t cam_handle = NULL;
    esp_cam_ctlr_dvp_pin_config_t pin_cfg = {
        .data_width = EXAMPLE_DVP_CAM_DATA_WIDTH,
        .data_io = {
            EXAMPLE_DVP_CAM_D0_IO,
            EXAMPLE_DVP_CAM_D1_IO,
            EXAMPLE_DVP_CAM_D2_IO,
            EXAMPLE_DVP_CAM_D3_IO,
            EXAMPLE_DVP_CAM_D4_IO,
            EXAMPLE_DVP_CAM_D5_IO,
            EXAMPLE_DVP_CAM_D6_IO,
            EXAMPLE_DVP_CAM_D7_IO,
        },
        .vsync_io = EXAMPLE_DVP_CAM_VSYNC_IO,
        .de_io = EXAMPLE_DVP_CAM_DE_IO,
        .pclk_io = EXAMPLE_DVP_CAM_PCLK_IO,
        .xclk_io = EXAMPLE_DVP_CAM_XCLK_IO, // Set XCLK pin to generate XCLK signal
    };

    esp_cam_ctlr_dvp_config_t dvp_config = {
        .ctlr_id = 0,
        .clk_src = CAM_CLK_SRC_DEFAULT,
        .h_res = CONFIG_EXAMPLE_CAM_HRES,
        .v_res = CONFIG_EXAMPLE_CAM_VRES,
        .input_data_color_type = CAM_CTLR_COLOR_RGB565,
        .output_data_color_type = CAM_CTLR_COLOR_RGB565,
        .dma_burst_size = 128,
        .pin = &pin_cfg,
        .bk_buffer_dis = 1,
        .xclk_freq = EXAMPLE_DVP_CAM_XCLK_FREQ_HZ,
        .cam_data_width = EXAMPLE_DVP_CAM_DATA_WIDTH,
        .bit_swap_en = false,
        .byte_swap_en = false,
    };

    ret = esp_cam_new_dvp_ctlr(&dvp_config, &cam_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "dvp init fail[%d]", ret);
        return;
    }

    //--------Allocate Camera Buffer----------//
    size_t cam_buffer_size = CONFIG_EXAMPLE_CAM_HRES * CONFIG_EXAMPLE_CAM_VRES * EXAMPLE_RGB565_BYTES_PER_PIXEL;
    void *cam_buffer = esp_cam_ctlr_alloc_buffer(cam_handle, cam_buffer_size, MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM);
    if (!cam_buffer) {
        ESP_LOGE(TAG, "no mem for cam_buffer");
        return;
    }
    esp_cam_ctlr_trans_t cam_trans = {
        .buffer = cam_buffer,
        .buflen = cam_buffer_size,
    };

    //--------Camera Sensor and SCCB Init-----------//
    example_sensor_config_t cam_sensor_config = {
        .i2c_port_num = I2C_NUM_0,
        .i2c_sda_io_num = EXAMPLE_DVP_CAM_SCCB_SDA_IO,
        .i2c_scl_io_num = EXAMPLE_DVP_CAM_SCCB_SCL_IO,
        .port = ESP_CAM_SENSOR_DVP,
        .format_name = EXAMPLE_CAM_FORMAT,
    };
    example_sensor_handle_t sensor_handle = {
        .sccb_handle = NULL,
        .i2c_bus_handle = NULL,
    };
    example_sensor_init(&cam_sensor_config, &sensor_handle);

    esp_cam_ctlr_evt_cbs_t cbs = {
        .on_get_new_trans = s_camera_get_new_vb,
        .on_trans_finished = s_camera_get_finished_trans,
    };
    if (esp_cam_ctlr_register_event_callbacks(cam_handle, &cbs, &cam_trans) != ESP_OK) {
        ESP_LOGE(TAG, "ops register fail");
        return;
    }

    ESP_ERROR_CHECK(esp_cam_ctlr_enable(cam_handle));

    //---------------DPI Reset------------------//
    example_dpi_panel_reset(mipi_dpi_panel);

    //init to all white
    memset(frame_buffer, 0xFF, frame_buffer_size);
    esp_cache_msync((void *)frame_buffer, frame_buffer_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M);

    example_dpi_panel_init(mipi_dpi_panel);

    if (esp_cam_ctlr_start(cam_handle) != ESP_OK) {
        ESP_LOGE(TAG, "Driver start fail");
        return;
    }

    // Register DPI panel event callback for display update ready notification
    display_update_param_t display_update_param = {
        .panel_hdl = mipi_dpi_panel,
        .cam_buf = cam_buffer,
    };
    esp_lcd_dpi_panel_event_callbacks_t dpi_cbs = {
        .on_color_trans_done = example_display_update_ready,
    };
    ESP_ERROR_CHECK(esp_lcd_dpi_panel_register_event_callbacks(mipi_dpi_panel, &dpi_cbs, &display_update_param));
    esp_lcd_panel_draw_bitmap(mipi_dpi_panel, 0, 0, CONFIG_EXAMPLE_CAM_HRES, CONFIG_EXAMPLE_CAM_VRES, cam_buffer);

    while (1) {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

static bool s_camera_get_new_vb(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data)
{
    esp_cam_ctlr_trans_t cam_trans = *(esp_cam_ctlr_trans_t *)user_data;
    trans->buffer = cam_trans.buffer;
    trans->buflen = cam_trans.buflen;
    return false;
}

static bool s_camera_get_finished_trans(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data)
{
    return false;
}
