/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <assert.h>
#include <unistd.h>
#include <sys/lock.h>
#include <sys/param.h>
#include "sdkconfig.h"
#include "esp_heap_caps.h"
#include "esp_lcd_mipi_dsi.h"
#include "esp_lcd_panel_ops.h"
#include "esp_log.h"
#include "esp_macros.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lvgl.h"
#include "lvgl_port.h"

static const char *TAG = "dsi_hdmi_bridge";

#define EXAMPLE_H_RES                  1920
#define EXAMPLE_V_RES                  1080
#define EXAMPLE_BYTES_PER_PIXEL        2
#define EXAMPLE_LVGL_COLOR_FORMAT      LV_COLOR_FORMAT_RGB565
#define EXAMPLE_LVGL_DRAW_BUF_LINES    20
#define EXAMPLE_LVGL_TICK_PERIOD_MS    2
#define EXAMPLE_LVGL_TASK_STACK_SIZE   (8 * 1024)
#define EXAMPLE_LVGL_TASK_PRIORITY     2
#define EXAMPLE_LVGL_TASK_MAX_DELAY_MS 500
#define EXAMPLE_LVGL_TASK_MIN_DELAY_MS (1000 / CONFIG_FREERTOS_HZ)

// LVGL APIs are called from multiple tasks, so serialize access with a lock.
static _lock_t s_lvgl_api_lock;

extern void example_lvgl_demo_ui(lv_display_t *disp);

static void example_rounder_flush_area_cb(lv_event_t *event)
{
    // DMA2D / MSPI may require 16-byte aligned windows (flash encryption, PSRAM ECC, etc.).
    lv_area_t *area = lv_event_get_invalidated_area(event);
    area->x1 = ESP_ALIGN_DOWN(area->x1, 16);
    area->x2 = ESP_ALIGN_UP(area->x2 + 1, 16) - 1;
}

static void example_lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    esp_lcd_panel_handle_t panel = lv_display_get_user_data(disp);
    // LVGL coordinates are inclusive, while draw_bitmap uses an exclusive end.
    ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(panel, area->x1, area->y1, area->x2 + 1, area->y2 + 1, px_map));
}

static bool example_notify_lvgl_flush_ready(esp_lcd_panel_handle_t panel,
                                            esp_lcd_dpi_panel_event_data_t *edata,
                                            void *user_ctx)
{
    lv_display_flush_ready((lv_display_t *)user_ctx);
    return false;
}

static void example_increase_lvgl_tick(void *arg)
{
    lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}

static void example_lvgl_port_task(void *arg)
{
    ESP_LOGI(TAG, "Starting LVGL task");
    while (true) {
        _lock_acquire(&s_lvgl_api_lock);
        uint32_t time_till_next_ms = lv_timer_handler();
        _lock_release(&s_lvgl_api_lock);
        time_till_next_ms = MAX(time_till_next_ms, EXAMPLE_LVGL_TASK_MIN_DELAY_MS);
        time_till_next_ms = MIN(time_till_next_ms, EXAMPLE_LVGL_TASK_MAX_DELAY_MS);
        usleep(1000 * time_till_next_ms);
    }
}

void example_lvgl_init(esp_lcd_panel_handle_t panel)
{
    ESP_LOGI(TAG, "Initialize LVGL");
    lv_init();
    lv_display_t *display = lv_display_create(EXAMPLE_H_RES, EXAMPLE_V_RES);
    assert(display);
    lv_display_set_user_data(display, panel);
    lv_display_set_color_format(display, EXAMPLE_LVGL_COLOR_FORMAT);

    const size_t draw_buffer_size =
        EXAMPLE_H_RES * EXAMPLE_LVGL_DRAW_BUF_LINES * EXAMPLE_BYTES_PER_PIXEL;
    // Keep the small 20-line partial buffers in internal RAM to reduce PSRAM
    // contention with the continuous framebuffer scanout.
    void *buf1 = heap_caps_aligned_calloc(16, 1, draw_buffer_size,
                                          MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    void *buf2 = heap_caps_aligned_calloc(16, 1, draw_buffer_size,
                                          MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    assert(buf1 && buf2);
    lv_display_set_buffers(display, buf1, buf2, draw_buffer_size, LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(display, example_lvgl_flush_cb);
    lv_display_add_event_cb(display, example_rounder_flush_area_cb, LV_EVENT_INVALIDATE_AREA, NULL);

    const esp_lcd_dpi_panel_event_callbacks_t callbacks = {
        .on_color_trans_done = example_notify_lvgl_flush_ready,
    };
    ESP_ERROR_CHECK(esp_lcd_dpi_panel_register_event_callbacks(panel, &callbacks, display));

    const esp_timer_create_args_t tick_timer_args = {
        .callback = example_increase_lvgl_tick,
        .name = "lvgl_tick",
    };
    esp_timer_handle_t tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&tick_timer_args, &tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000));

    _lock_acquire(&s_lvgl_api_lock);
    example_lvgl_demo_ui(display);
    _lock_release(&s_lvgl_api_lock);
    BaseType_t task_created = xTaskCreate(example_lvgl_port_task, "LVGL",
                                          EXAMPLE_LVGL_TASK_STACK_SIZE, NULL,
                                          EXAMPLE_LVGL_TASK_PRIORITY, NULL);
    assert(task_created == pdPASS);
    ESP_LOGI(TAG, "Displaying LVGL UI at %dx%d", EXAMPLE_H_RES, EXAMPLE_V_RES);
}
