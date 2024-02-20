/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_private/esp_ldo.h"
#include "esp_timer.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_lcd_ili9881c.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "lvgl.h"

static const char *TAG = "example";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your LCD Spec //////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// FPS = 80000000/(40+140+40+800)/(4+16+16+1280) = 60Hz
#define EXAMPLE_MIPI_DSI_DPI_CLK_MHZ  80
#define EXAMPLE_MIPI_DSI_LCD_H_RES    800
#define EXAMPLE_MIPI_DSI_LCD_V_RES    1280
#define EXAMPLE_MIPI_DSI_LCD_HSYNC    40
#define EXAMPLE_MIPI_DSI_LCD_HBP      140
#define EXAMPLE_MIPI_DSI_LCD_HFP      40
#define EXAMPLE_MIPI_DSI_LCD_VSYNC    4
#define EXAMPLE_MIPI_DSI_LCD_VBP      16
#define EXAMPLE_MIPI_DSI_LCD_VFP      16

#define EXAMPLE_MIPI_DSI_LANE_NUM          2    // 2 data lanes
#define EXAMPLE_MIPI_DSI_LANE_BITRATE_MBPS 1000 // 1Gbps

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your Board Design //////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// The "VDD_MIPI_DPHY" should be supplied with 2.5V, it can source from the integrated LDO unit or from external LDO chip
#define EXAMPLE_MIPI_DSI_PHY_PWR_LDO_UNIT       3  // LDO_VO3 is connected to VDD_MIPI_DPHY
#define EXAMPLE_MIPI_DSI_PHY_PWR_LDO_VOLTAGE_MV 2500
#define EXAMPLE_LCD_BK_LIGHT_ON_LEVEL           1
#define EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL          !EXAMPLE_LCD_BK_LIGHT_ON_LEVEL
#define EXAMPLE_PIN_NUM_BK_LIGHT                -1
#define EXAMPLE_PIN_NUM_LCD_RST                 -1

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your Application ///////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define EXAMPLE_LVGL_DRAW_BUF_LINES    200 // number of display lines in each draw buffer
#define EXAMPLE_LVGL_TICK_PERIOD_MS    2
#define EXAMPLE_LVGL_TASK_MAX_DELAY_MS 500
#define EXAMPLE_LVGL_TASK_MIN_DELAY_MS 1
#define EXAMPLE_LVGL_TASK_STACK_SIZE   (4 * 1024)
#define EXAMPLE_LVGL_TASK_PRIORITY     2

static SemaphoreHandle_t lvgl_api_mux = NULL;

extern void example_lvgl_demo_ui(lv_disp_t *disp);

static void example_lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t) drv->user_data;
    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;
    // pass the draw buffer to the driver
    esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
}

static void example_increase_lvgl_tick(void *arg)
{
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}

static bool example_lvgl_lock(int timeout_ms)
{
    // Convert timeout in milliseconds to FreeRTOS ticks
    // If `timeout_ms` is set to -1, the program will block until the condition is met
    const TickType_t timeout_ticks = (timeout_ms == -1) ? portMAX_DELAY : pdMS_TO_TICKS(timeout_ms);
    return xSemaphoreTakeRecursive(lvgl_api_mux, timeout_ticks) == pdTRUE;
}

static void example_lvgl_unlock(void)
{
    xSemaphoreGiveRecursive(lvgl_api_mux);
}

static void example_lvgl_port_task(void *arg)
{
    ESP_LOGI(TAG, "Starting LVGL task");
    uint32_t task_delay_ms = EXAMPLE_LVGL_TASK_MAX_DELAY_MS;
    while (1) {
        // Lock the mutex due to the LVGL APIs are not thread-safe
        if (example_lvgl_lock(-1)) {
            task_delay_ms = lv_timer_handler();
            // Release the mutex
            example_lvgl_unlock();
        }
        if (task_delay_ms > EXAMPLE_LVGL_TASK_MAX_DELAY_MS) {
            task_delay_ms = EXAMPLE_LVGL_TASK_MAX_DELAY_MS;
        } else if (task_delay_ms < EXAMPLE_LVGL_TASK_MIN_DELAY_MS) {
            task_delay_ms = EXAMPLE_LVGL_TASK_MIN_DELAY_MS;
        }
        vTaskDelay(pdMS_TO_TICKS(task_delay_ms));
    }
}

static bool example_notify_lvgl_flush_ready(esp_lcd_panel_handle_t panel, esp_lcd_dpi_panel_event_data_t *edata, void *user_ctx)
{
    lv_disp_drv_t *disp_driver = (lv_disp_drv_t *)user_ctx;
    lv_disp_flush_ready(disp_driver);
    return false;
}

static void example_bsp_enable_dsi_phy_power(void)
{
    // Turn on the power for MIPI DSI PHY, so it can go from "No Power" state to "Shutdown" state
    esp_ldo_unit_handle_t phy_pwr_unit = NULL;
#if EXAMPLE_MIPI_DSI_PHY_PWR_LDO_UNIT > 0
    esp_ldo_unit_init_cfg_t ldo_cfg = {
        .unit_id = EXAMPLE_MIPI_DSI_PHY_PWR_LDO_UNIT,
        .cfg = {
            .voltage_mv = EXAMPLE_MIPI_DSI_PHY_PWR_LDO_VOLTAGE_MV,
        },
    };
    ESP_ERROR_CHECK(esp_ldo_init_unit(&ldo_cfg, &phy_pwr_unit));
    ESP_ERROR_CHECK(esp_ldo_enable_unit(phy_pwr_unit));

    ESP_LOGI(TAG, "MIPI DSI PHY Powered on");
#endif
}

static void example_bsp_init_lcd_backlight(void)
{
#if EXAMPLE_PIN_NUM_BK_LIGHT >= 0
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << EXAMPLE_PIN_NUM_BK_LIGHT
    };
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));
#endif
}

static void example_bsp_set_lcd_backlight(uint32_t level)
{
#if EXAMPLE_PIN_NUM_BK_LIGHT >= 0
    gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, level);
#endif
}

void app_main(void)
{
    static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
    static lv_disp_drv_t disp_drv;      // contains callback functions

    example_bsp_enable_dsi_phy_power();
    example_bsp_init_lcd_backlight();
    example_bsp_set_lcd_backlight(EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL);

    // create MIPI DSI bus first, it will initialize the DSI PHY as well
    esp_lcd_dsi_bus_handle_t mipi_dsi_bus;
    esp_lcd_dsi_bus_config_t bus_config = {
        .bus_id = 0,
        .num_data_lanes = EXAMPLE_MIPI_DSI_LANE_NUM,
        .phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT,
        .lane_bit_rate_mbps = EXAMPLE_MIPI_DSI_LANE_BITRATE_MBPS,
    };
    ESP_ERROR_CHECK(esp_lcd_new_dsi_bus(&bus_config, &mipi_dsi_bus));

    ESP_LOGI(TAG, "Install MIPI DSI LCD control panel");
    esp_lcd_panel_io_handle_t mipi_dbi_io;
    // we use DBI interface to send LCD commands and parameters
    esp_lcd_dbi_io_config_t dbi_config = {
        .virtual_channel = 0,
        .lcd_cmd_bits = 8,   // according to the LCD ILI9881C spec
        .lcd_param_bits = 8, // according to the LCD ILI9881C spec
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_dbi(mipi_dsi_bus, &dbi_config, &mipi_dbi_io));
    // create ILI9881C control panel
    esp_lcd_panel_handle_t ili9881c_ctrl_panel;
    esp_lcd_panel_dev_config_t lcd_dev_config = {
        .bits_per_pixel = 16,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .reset_gpio_num = EXAMPLE_PIN_NUM_LCD_RST,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_ili9881c(mipi_dbi_io, &lcd_dev_config, &ili9881c_ctrl_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(ili9881c_ctrl_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_init(ili9881c_ctrl_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(ili9881c_ctrl_panel, true));

    ESP_LOGI(TAG, "Install MIPI DSI LCD data panel");
    esp_lcd_panel_handle_t mipi_dpi_panel;
    esp_lcd_dpi_panel_config_t dpi_config = {
        .virtual_channel = 0,
        .dpi_clk_src = MIPI_DSI_DPI_CLK_SRC_DEFAULT,
        .dpi_clock_freq_mhz = EXAMPLE_MIPI_DSI_DPI_CLK_MHZ,
        .pixel_format = LCD_COLOR_PIXEL_FORMAT_RGB565,
        .video_timing = {
            .h_size = EXAMPLE_MIPI_DSI_LCD_H_RES,
            .v_size = EXAMPLE_MIPI_DSI_LCD_V_RES,
            .hsync_back_porch = EXAMPLE_MIPI_DSI_LCD_HBP,
            .hsync_pulse_width = EXAMPLE_MIPI_DSI_LCD_HSYNC,
            .hsync_front_porch = EXAMPLE_MIPI_DSI_LCD_HFP,
            .vsync_back_porch = EXAMPLE_MIPI_DSI_LCD_VBP,
            .vsync_pulse_width = EXAMPLE_MIPI_DSI_LCD_VSYNC,
            .vsync_front_porch = EXAMPLE_MIPI_DSI_LCD_VFP,
        },
#if CONFIG_EXAMPLE_USE_DMA2D_COPY_FRAME
        .flags.use_dma2d = true,
#endif
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_dpi(mipi_dsi_bus, &dpi_config, &mipi_dpi_panel));
    // register event callbacks
    esp_lcd_dpi_panel_event_callbacks_t cbs = {
        .on_color_trans_done = example_notify_lvgl_flush_ready,
    };
    ESP_ERROR_CHECK(esp_lcd_dpi_panel_register_event_callbacks(mipi_dpi_panel, &cbs, &disp_drv));
    ESP_ERROR_CHECK(esp_lcd_panel_init(mipi_dpi_panel));

    // turn on backlight
    example_bsp_set_lcd_backlight(EXAMPLE_LCD_BK_LIGHT_ON_LEVEL);

    ESP_LOGI(TAG, "Initialize LVGL library");
    lv_init();
    void *buf1 = NULL;
    void *buf2 = NULL;
    ESP_LOGI(TAG, "Allocate separate LVGL draw buffers from PSRAM");
    buf1 = heap_caps_malloc(EXAMPLE_MIPI_DSI_LCD_H_RES * EXAMPLE_LVGL_DRAW_BUF_LINES * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    assert(buf1);
    buf2 = heap_caps_malloc(EXAMPLE_MIPI_DSI_LCD_H_RES * EXAMPLE_LVGL_DRAW_BUF_LINES * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    assert(buf2);
    // initialize LVGL draw buffers
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, EXAMPLE_MIPI_DSI_LCD_H_RES * EXAMPLE_LVGL_DRAW_BUF_LINES);

    // register display driver to LVGL library
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = EXAMPLE_MIPI_DSI_LCD_H_RES;
    disp_drv.ver_res = EXAMPLE_MIPI_DSI_LCD_V_RES;
    disp_drv.flush_cb = example_lvgl_flush_cb;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.user_data = mipi_dpi_panel;
    lv_disp_t *disp = lv_disp_drv_register(&disp_drv);

    ESP_LOGI(TAG, "Use esp_timer as LVGL tick timer");
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &example_increase_lvgl_tick,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000));

    // LVGL APIs are meant to be called across the threads without protection, so we use a mutex here
    lvgl_api_mux = xSemaphoreCreateRecursiveMutex();
    assert(lvgl_api_mux);

    ESP_LOGI(TAG, "Create LVGL task");
    xTaskCreate(example_lvgl_port_task, "LVGL", EXAMPLE_LVGL_TASK_STACK_SIZE, NULL, EXAMPLE_LVGL_TASK_PRIORITY, NULL);

    ESP_LOGI(TAG, "Display LVGL Meter Widget");
    // Lock the mutex due to the LVGL APIs are not thread-safe
    if (example_lvgl_lock(-1)) {
        example_lvgl_demo_ui(disp);
        // Release the mutex
        example_lvgl_unlock();
    }
}
