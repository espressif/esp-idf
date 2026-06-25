/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/lock.h>
#include <sys/param.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_lcd_panel_ops.h"
#include "lvgl.h"
#include "example_rgb_lcd_panel.h"

static const char *TAG = "example";

// LVGL draw buffers must use the same pixel format as the RGB panel output.
#if CONFIG_EXAMPLE_LCD_DATA_LINES_16
#define EXAMPLE_LV_COLOR_FORMAT      LV_COLOR_FORMAT_RGB565
#elif CONFIG_EXAMPLE_LCD_DATA_LINES_24
#define EXAMPLE_LV_COLOR_FORMAT      LV_COLOR_FORMAT_RGB888
#else
#error "Unsupported LVGL color format"
#endif

#define EXAMPLE_LVGL_DRAW_BUF_LINES    50 // number of display lines in each draw buffer
#define EXAMPLE_LVGL_TICK_PERIOD_MS    2
#define EXAMPLE_LVGL_TASK_STACK_SIZE   (5 * 1024)
#define EXAMPLE_LVGL_TASK_PRIORITY     2
#define EXAMPLE_LVGL_TASK_MAX_DELAY_MS 500
#define EXAMPLE_LVGL_TASK_MIN_DELAY_MS 1000 / CONFIG_FREERTOS_HZ

// LVGL is not thread-safe. In this example both app_main() and the LVGL task touch
// the LVGL object tree, so guard every LVGL call with the same lock.
static _lock_t lvgl_api_lock;
static TaskHandle_t lvgl_task_handle;

extern void example_lvgl_demo_ui(lv_display_t *disp);

#if CONFIG_EXAMPLE_USE_DOUBLE_FB
static bool example_on_frame_buf_complete(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t *event_data, void *user_ctx)
{
    (void)panel;
    (void)event_data;
    (void)user_ctx;
    BaseType_t need_yield = pdFALSE;

    if (lvgl_task_handle) {
        vTaskNotifyGiveFromISR(lvgl_task_handle, &need_yield);
    }
    return need_yield == pdTRUE;
}

static void example_lvgl_flush_wait_cb(lv_display_t *disp)
{
    // In direct-mode double buffering, lv_display_flush_cb() only tells the driver
    // which frame buffer should be displayed next. LVGL must then wait until the
    // driver finishes switching buffers before it renders the next frame.
    if (lv_display_flush_is_last(disp)) {
        // Wait until the previous frame buffer is no longer referenced by DMA.
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }
    lv_display_flush_ready(disp);
}
#else
static bool example_notify_lvgl_flush_ready(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t *event_data, void *user_ctx)
{
    lv_display_t *disp = (lv_display_t *)user_ctx;
    lv_display_flush_ready(disp);
    return false;
}
#endif

static void example_lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    esp_lcd_panel_handle_t panel_handle = lv_display_get_user_data(disp);
    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;
#if CONFIG_EXAMPLE_USE_DOUBLE_FB
    if (!lv_display_flush_is_last(disp)) {
        // LVGL may split one frame into several dirty rectangles. In direct mode,
        // switch the hardware frame buffer only after the last rectangle is done.
        lv_display_flush_ready(disp);
        return;
    }
    offsetx1 = 0;
    offsety1 = 0;
    offsetx2 = EXAMPLE_LCD_H_RES - 1;
    offsety2 = EXAMPLE_LCD_V_RES - 1;
    // Clear any stale completion from the previous frame before waiting for this frame.
    ulTaskNotifyTake(pdTRUE, 0);
#endif
    // pass the draw buffer to the driver
    esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, px_map);
}

static void example_increase_lvgl_tick(void *arg)
{
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}

static void example_lvgl_port_task(void *arg)
{
    ESP_LOGI(TAG, "Starting LVGL task");
    uint32_t time_till_next_ms = 0;
    while (1) {
        // lv_timer_handler() runs animations, input handling, and screen refresh scheduling.
        _lock_acquire(&lvgl_api_lock);
        time_till_next_ms = lv_timer_handler();
        _lock_release(&lvgl_api_lock);
        // in case of task watch dog timeout
        time_till_next_ms = MAX(time_till_next_ms, EXAMPLE_LVGL_TASK_MIN_DELAY_MS);
        // in case of lvgl display not ready yet
        time_till_next_ms = MIN(time_till_next_ms, EXAMPLE_LVGL_TASK_MAX_DELAY_MS);
        usleep(1000 * time_till_next_ms);
    }
}

void app_main(void)
{
    // Keep the backlight off while the RGB panel and LVGL are being configured.
    ESP_ERROR_CHECK(example_rgb_lcd_backlight_init());
    ESP_LOGI(TAG, "Initialize LCD backlight");
    ESP_LOGI(TAG, "Turn off LCD backlight");
    example_rgb_lcd_backlight_set(false);

    // Create the RGB panel object from the GPIO/timing configuration in
    // example_rgb_lcd_panel.{h,c}, then reset and start the hardware.
    ESP_LOGI(TAG, "Create RGB LCD panel");
    esp_lcd_panel_handle_t panel_handle = NULL;
    ESP_ERROR_CHECK(example_rgb_lcd_panel_new(&panel_handle));

    ESP_LOGI(TAG, "Initialize RGB LCD panel");
    ESP_ERROR_CHECK(example_rgb_lcd_panel_init(panel_handle));

    ESP_LOGI(TAG, "Turn on LCD backlight");
    example_rgb_lcd_backlight_set(true);

    // Create one LVGL display that uses the RGB panel as its flush target.
    ESP_LOGI(TAG, "Initialize LVGL library");
    lv_init();
    lv_display_t *display = lv_display_create(EXAMPLE_LCD_H_RES, EXAMPLE_LCD_V_RES);
    lv_display_set_user_data(display, panel_handle);
    lv_display_set_color_format(display, EXAMPLE_LV_COLOR_FORMAT);
    void *buf1 = NULL;
    void *buf2 = NULL;
#if CONFIG_EXAMPLE_USE_DOUBLE_FB
    ESP_LOGI(TAG, "Use frame buffers as LVGL draw buffers");
    ESP_ERROR_CHECK(esp_lcd_rgb_panel_get_frame_buffer(panel_handle, 2, &buf1, &buf2));
    // Direct mode lets LVGL render straight into the hardware frame buffers.
    lv_display_set_buffers(display, buf1, buf2, EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES * EXAMPLE_PIXEL_SIZE, LV_DISPLAY_RENDER_MODE_DIRECT);
#else
    ESP_LOGI(TAG, "Allocate LVGL draw buffers");
    // Partial mode uses a smaller draw buffer and copies only the dirty area to the panel.
    // Allocate this buffer from internal RAM for better DMA and CPU access performance.
    size_t draw_buffer_sz = EXAMPLE_LCD_H_RES * EXAMPLE_LVGL_DRAW_BUF_LINES * EXAMPLE_PIXEL_SIZE;
    buf1 = esp_lcd_rgb_alloc_draw_buffer(panel_handle, draw_buffer_sz, 0);
    assert(buf1);
    lv_display_set_buffers(display, buf1, buf2, draw_buffer_sz, LV_DISPLAY_RENDER_MODE_PARTIAL);
#endif // CONFIG_EXAMPLE_USE_DOUBLE_FB

    // Connect LVGL's flush path to the RGB panel driver.
    lv_display_set_flush_cb(display, example_lvgl_flush_cb);
#if CONFIG_EXAMPLE_USE_DOUBLE_FB
    lv_display_set_flush_wait_cb(display, example_lvgl_flush_wait_cb);
#endif

    ESP_LOGI(TAG, "Register event callbacks");
    esp_lcd_rgb_panel_event_callbacks_t cbs = {
#if CONFIG_EXAMPLE_USE_DOUBLE_FB
        // Signal LVGL when the panel has switched to the new frame buffer.
        .on_frame_buf_complete = example_on_frame_buf_complete,
#else
        // Signal LVGL when the driver finishes copying the dirty area.
        .on_color_trans_done = example_notify_lvgl_flush_ready,
#endif
    };
    ESP_ERROR_CHECK(esp_lcd_rgb_panel_register_event_callbacks(panel_handle, &cbs, display));

    ESP_LOGI(TAG, "Install LVGL tick timer");
    // Feed LVGL with a periodic tick so it can keep time for animations and timers.
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &example_increase_lvgl_tick,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000));

    ESP_LOGI(TAG, "Create LVGL task");
    xTaskCreate(example_lvgl_port_task, "LVGL", EXAMPLE_LVGL_TASK_STACK_SIZE, NULL, EXAMPLE_LVGL_TASK_PRIORITY, &lvgl_task_handle);

    // Build the demo UI once the display pipeline is ready.
    ESP_LOGI(TAG, "Display LVGL UI");
    _lock_acquire(&lvgl_api_lock);
    example_lvgl_demo_ui(display);
    _lock_release(&lvgl_api_lock);
}
