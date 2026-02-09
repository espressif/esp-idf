/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hal/cam_ctlr_types.h"
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "esp_lcd_panel_ops.h"
#include "esp_cache.h"
#include "driver/i2c_master.h"
#include "esp_cam_ctlr.h"
#include "esp_cam_ctlr_dvp.h"
#include "example_config.h"
#include "driver/ledc.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "example_sensor_init.h"

static const char *TAG = "dvp_spi_lcd";

#define BUFFER_SIZE         (CONFIG_EXAMPLE_CAM_HRES * CONFIG_EXAMPLE_CAM_VRES * EXAMPLE_RGB565_BYTES_PER_PIXEL)

typedef struct {
    esp_lcd_panel_handle_t panel_hdl;
    esp_cam_ctlr_trans_t cam_trans;
} example_cam_context_t;

static bool s_camera_get_new_vb(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data);
static bool s_camera_get_finished_trans(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data);

static void lcd_display_init(esp_lcd_panel_handle_t *lcd_panel_hdl, esp_lcd_panel_io_handle_t lcd_io_hdl)
{
    esp_lcd_panel_handle_t panel_handle = NULL;
    //----------LEDC initialization------------//
    const ledc_timer_config_t lcd_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .duty_resolution  = LEDC_TIMER_10_BIT,
        .timer_num        = EXAMPLE_LEDC_LCD_BACKLIGHT,
        .freq_hz          = 5000,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&lcd_timer));
    const ledc_channel_config_t lcd_channel = {
        .gpio_num       = EXAMPLE_LCD_BACKLIGHT,
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = LEDC_CHANNEL_0,
        .timer_sel      = EXAMPLE_LEDC_LCD_BACKLIGHT,
        .intr_type      = LEDC_INTR_DISABLE,
        .duty           = 0,
        .hpoint         = 0,
        .flags.output_invert = true,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&lcd_channel));

    //----------SPI initialization------------//
    ESP_LOGI(TAG, "Init SPI bus");
    const spi_bus_config_t bus_cfg = {
        .sclk_io_num = EXAMPLE_LCD_SPI_CLK,
        .mosi_io_num = EXAMPLE_LCD_SPI_MOSI,
        .miso_io_num = GPIO_NUM_NC,
        .quadwp_io_num = GPIO_NUM_NC,
        .quadhd_io_num = GPIO_NUM_NC,
        .max_transfer_sz = BUFFER_SIZE,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(EXAMPLE_LCD_SPI_NUM, &bus_cfg, SPI_DMA_CH_AUTO));

    //----------Panel IO initialization------------//
    ESP_LOGI(TAG, "New panel IO SPI");
    const esp_lcd_panel_io_spi_config_t io_cfg = {
        .dc_gpio_num = EXAMPLE_LCD_DC,
        .cs_gpio_num = EXAMPLE_LCD_SPI_CS,
        .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = EXAMPLE_LCD_CMD_BITS,
        .lcd_param_bits = EXAMPLE_LCD_PARAM_BITS,
        .spi_mode         = 2,
        .trans_queue_depth = 10,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(
                        (esp_lcd_spi_bus_handle_t)EXAMPLE_LCD_SPI_NUM,
                        &io_cfg,
                        &lcd_io_hdl
                    ));

    //----------ST7789 Panel initialization------------//
    ESP_LOGI(TAG, "New ST7789 panel");
    const esp_lcd_panel_dev_config_t panel_dev_cfg = {
        .reset_gpio_num   = EXAMPLE_LCD_RST,
        .rgb_ele_order    = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel   = EXAMPLE_RGB565_BITS_PER_PIXEL,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(lcd_io_hdl, &panel_dev_cfg, &panel_handle));

    ESP_LOGI(TAG, "Reset and init panel");
    esp_lcd_panel_reset(panel_handle);
    esp_lcd_panel_init(panel_handle);
    esp_lcd_panel_invert_color(panel_handle, true);

    ESP_LOGI(TAG, "Turn on display");
    esp_lcd_panel_disp_on_off(panel_handle, true);

    const int brightness = 100;
    uint32_t duty = (1023 * brightness) / 100;
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));

    *lcd_panel_hdl = panel_handle;
}

void app_main(void)
{
    esp_err_t ret = ESP_FAIL;

    esp_lcd_panel_handle_t lcd_panel_hdl = NULL;
    esp_lcd_panel_io_handle_t lcd_io_hdl = NULL;

    lcd_display_init(&lcd_panel_hdl, lcd_io_hdl);

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
        .xclk_io = EXAMPLE_DVP_CAM_XCLK_IO,
    };

    esp_cam_ctlr_dvp_config_t dvp_config = {
        .ctlr_id = 0,
        .clk_src = CAM_CLK_SRC_DEFAULT,
        .h_res = CONFIG_EXAMPLE_CAM_HRES,
        .v_res = CONFIG_EXAMPLE_CAM_VRES,
#if CONFIG_EXAMPLE_CAM_INPUT_FORMAT_YUV422
        .input_data_color_type = CAM_CTLR_COLOR_YUV422_UYVY,
        .output_data_color_type = CAM_CTLR_COLOR_RGB565,
#else
        .input_data_color_type = CAM_CTLR_COLOR_RGB565,
        .output_data_color_type = CAM_CTLR_COLOR_RGB565,
#endif
        .conv_std = COLOR_CONV_STD_RGB_YUV_BT601,
        .input_range = COLOR_RANGE_LIMIT,
        .output_range = COLOR_RANGE_LIMIT,
        .dma_burst_size = 64,
        .pin = &pin_cfg,
        .bk_buffer_dis = 1,
        .xclk_freq = EXAMPLE_DVP_CAM_XCLK_FREQ_HZ,
    };

    ret = esp_cam_new_dvp_ctlr(&dvp_config, &cam_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "dvp init fail[%d]", ret);
        return;
    }

    //--------Allocate Camera Buffer----------//
    size_t cam_buffer_size = CONFIG_EXAMPLE_CAM_HRES * CONFIG_EXAMPLE_CAM_VRES * EXAMPLE_RGB565_BYTES_PER_PIXEL;
    void *cam_buffer = NULL;

    cam_buffer = esp_cam_ctlr_alloc_buffer(cam_handle, cam_buffer_size, EXAMPLE_DVP_CAM_BUF_ALLOC_CAPS);

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

    //--------Register Camera Callbacks----------//
    example_cam_context_t cam_ctx = {
        .panel_hdl = lcd_panel_hdl,
        .cam_trans = {
            .buffer = cam_buffer,
            .buflen = cam_buffer_size,
        }
    };

    esp_cam_ctlr_evt_cbs_t cbs = {
        .on_get_new_trans = s_camera_get_new_vb,
        .on_trans_finished = s_camera_get_finished_trans,
    };
    if (esp_cam_ctlr_register_event_callbacks(cam_handle, &cbs, &cam_ctx) != ESP_OK) {
        ESP_LOGE(TAG, "ops register fail");
        return;
    }

    //--------Enable and start Camera Controller----------//
    ESP_ERROR_CHECK(esp_cam_ctlr_enable(cam_handle));

#if CONFIG_EXAMPLE_CAM_INPUT_FORMAT_YUV422
    ESP_LOGI(TAG, "Configure format conversion: YUV422 -> RGB565");
    // Configure format conversion
    const cam_ctlr_format_conv_config_t conv_cfg = {
        .src_format = CAM_CTLR_COLOR_YUV422_UYVY,       // Source format: YUV422
        .dst_format = CAM_CTLR_COLOR_RGB565,            // Destination format: RGB565
        .conv_std = COLOR_CONV_STD_RGB_YUV_BT601,
        .data_width = 8,
        .input_range = COLOR_RANGE_LIMIT,
        .output_range = COLOR_RANGE_LIMIT,
    };
    ESP_ERROR_CHECK(esp_cam_ctlr_format_conversion(cam_handle, &conv_cfg));
#endif

    if (esp_cam_ctlr_start(cam_handle) != ESP_OK) {
        ESP_LOGE(TAG, "Driver start fail");
        return;
    }

    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

static bool s_camera_get_new_vb(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data)
{
    example_cam_context_t *ctx = (example_cam_context_t *)user_data;
    *trans = ctx->cam_trans;
    return false;
}

static bool s_camera_get_finished_trans(esp_cam_ctlr_handle_t handle, esp_cam_ctlr_trans_t *trans, void *user_data)
{
    example_cam_context_t *ctx = (example_cam_context_t *)user_data;
    esp_lcd_panel_draw_bitmap(ctx->panel_hdl, 0, 0, CONFIG_EXAMPLE_CAM_HRES, CONFIG_EXAMPLE_CAM_VRES, trans->buffer);
    return false;
}
