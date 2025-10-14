/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "driver/parlio_tx.h"
#include "driver/dedic_gpio.h"
#include "driver/gptimer.h"
#include "lvgl.h"

static const char *TAG = "example";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your LED Matrix spec ///////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// color data, clock signal and control signals are powered by parlio_tx unit
#define EXAMPLE_HUB75_DATA_WIDTH 8 // 8-bit data width, used for transferring color data (RGB222) and control signals (OE and latch)
#define EXAMPLE_HUB75_R1_IDX     7 // R1 bit position
#define EXAMPLE_HUB75_R2_IDX     6 // R2 bit position
#define EXAMPLE_HUB75_LATCH_IDX  5 // LATCH bit position
#define EXAMPLE_HUB75_G1_IDX     4 // G1 bit position
#define EXAMPLE_HUB75_G2_IDX     3 // G2 bit position
#define EXAMPLE_HUB75_OE_IDX     2 // OE bit position
#define EXAMPLE_HUB75_B1_IDX     1 // B1 bit position
#define EXAMPLE_HUB75_B2_IDX     0 // B2 bit position
// GPIO assignment
#define EXAMPLE_PIN_NUM_R1       CONFIG_EXAMPLE_PIN_NUM_R1
#define EXAMPLE_PIN_NUM_G1       CONFIG_EXAMPLE_PIN_NUM_G1
#define EXAMPLE_PIN_NUM_B1       CONFIG_EXAMPLE_PIN_NUM_B1
#define EXAMPLE_PIN_NUM_R2       CONFIG_EXAMPLE_PIN_NUM_R2
#define EXAMPLE_PIN_NUM_G2       CONFIG_EXAMPLE_PIN_NUM_G2
#define EXAMPLE_PIN_NUM_B2       CONFIG_EXAMPLE_PIN_NUM_B2
#define EXAMPLE_PIN_NUM_LATCH    CONFIG_EXAMPLE_PIN_NUM_LATCH
#define EXAMPLE_PIN_NUM_OE       CONFIG_EXAMPLE_PIN_NUM_OE
#define EXAMPLE_PIN_NUM_PCLK     CONFIG_EXAMPLE_PIN_NUM_PCLK
// address signals are powered by fast GPIO module
#define EXAMPLE_PIN_NUM_A        CONFIG_EXAMPLE_PIN_NUM_A
#define EXAMPLE_PIN_NUM_B        CONFIG_EXAMPLE_PIN_NUM_B
#define EXAMPLE_PIN_NUM_C        CONFIG_EXAMPLE_PIN_NUM_C
#define EXAMPLE_PIN_NUM_D        CONFIG_EXAMPLE_PIN_NUM_D

// The pixel clock frequency
#define EXAMPLE_LED_MATRIX_PIXEL_CLOCK_HZ (10 * 1000 * 1000) // 10MHz
// The pixel number in horizontal and vertical
#define EXAMPLE_LED_MATRIX_H_RES 64
#define EXAMPLE_LED_MATRIX_V_RES 32

// We use the gptimer interrupt to generate the LVGL tick interface
#define EXAMPLE_GPTIMER_RESOLUTION_HZ  (1 * 1000 * 1000) // 1MHz
#define EXAMPLE_LVGL_TICK_PERIOD_MS    5 // 5ms

// LED Matrix frame buffer, we use a dedicated task to flush this frame buffer
static uint8_t s_frame_buffer[EXAMPLE_LED_MATRIX_H_RES * EXAMPLE_LED_MATRIX_V_RES / 2] = {};

// upper_half: pixel on the upper half screen
// lower_half: pixel on the lower half screen
static void merge_two_pixels(lv_color16_t *upper_half, lv_color16_t *lower_half, int index)
{
    s_frame_buffer[index]  = (upper_half->red >> 4) << EXAMPLE_HUB75_R1_IDX | (upper_half->green >> 5) << EXAMPLE_HUB75_G1_IDX | (upper_half->blue >> 4) << EXAMPLE_HUB75_B1_IDX;
    s_frame_buffer[index] |= (lower_half->red >> 4) << EXAMPLE_HUB75_R2_IDX | (lower_half->green >> 5) << EXAMPLE_HUB75_G2_IDX | (lower_half->blue >> 4) << EXAMPLE_HUB75_B2_IDX;
    // OE=1: disable the output
    s_frame_buffer[index] |= 1 << EXAMPLE_HUB75_OE_IDX;
}

extern void example_lvgl_demo_ui(lv_display_t *disp);

static void example_lvgl_flush_cb(lv_display_t *display, const lv_area_t *area, uint8_t *color_map)
{
    lv_color16_t *upper_half = (lv_color16_t *)color_map;
    lv_color16_t *lower_half = (lv_color16_t *)color_map + EXAMPLE_LED_MATRIX_V_RES * EXAMPLE_LED_MATRIX_H_RES / 2;
    int index = 0;
    for (int line = 0; line < EXAMPLE_LED_MATRIX_V_RES / 2; line++) {
        for (int col = 0; col < EXAMPLE_LED_MATRIX_H_RES - 1; col++) {
            merge_two_pixels(upper_half++, lower_half++, index++);
        }
        merge_two_pixels(upper_half++, lower_half++, index);
        // need special handling for the last pixel in each line
        // latch up at the end of each line
        s_frame_buffer[index++] |= (1 << EXAMPLE_HUB75_LATCH_IDX);
    }
    lv_display_flush_ready(display);
}

static IRAM_ATTR bool parlio_tx_line_done_cb(parlio_tx_unit_handle_t tx_unit, const parlio_tx_done_event_data_t *edata, void *user_ctx)
{
    static uint32_t line_number = 0;
    dedic_gpio_bundle_handle_t gpio_bundle = (dedic_gpio_bundle_handle_t)user_ctx;
    dedic_gpio_bundle_write(gpio_bundle, 0x0F, ++line_number);
    return false;
}

static IRAM_ATTR bool gptimer_alarm_cb_lvgl_tick(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
    return false;
}

void app_main(void)
{
    ESP_LOGI(TAG, "Install fast GPIO bundle for line address control");
    dedic_gpio_bundle_config_t dedic_gpio_conf = {
        .flags.out_en = true,
        .gpio_array = (int[])
        {
            EXAMPLE_PIN_NUM_A, EXAMPLE_PIN_NUM_B, EXAMPLE_PIN_NUM_C, EXAMPLE_PIN_NUM_D
        },
        .array_size = 4,
    };
    dedic_gpio_bundle_handle_t led_line_control_gpio_bundle;
    ESP_ERROR_CHECK(dedic_gpio_new_bundle(&dedic_gpio_conf, &led_line_control_gpio_bundle));
    // initial line address to 0
    dedic_gpio_bundle_write(led_line_control_gpio_bundle, 0x0F, 0x00);

    ESP_LOGI(TAG, "Install parallel IO TX unit");
    parlio_tx_unit_handle_t tx_unit = NULL;
    parlio_tx_unit_config_t config = {
        .clk_src = PARLIO_CLK_SRC_DEFAULT,
        .data_width = EXAMPLE_HUB75_DATA_WIDTH,
        .clk_in_gpio_num = -1, // use internal clock source
        .valid_gpio_num = -1,  // don't generate valid signal
        .clk_out_gpio_num = EXAMPLE_PIN_NUM_PCLK,
        .data_gpio_nums = {
            [EXAMPLE_HUB75_R1_IDX] = EXAMPLE_PIN_NUM_R1,
            [EXAMPLE_HUB75_R2_IDX] = EXAMPLE_PIN_NUM_R2,
            [EXAMPLE_HUB75_G1_IDX] = EXAMPLE_PIN_NUM_G1,
            [EXAMPLE_HUB75_G2_IDX] = EXAMPLE_PIN_NUM_G2,
            [EXAMPLE_HUB75_B1_IDX] = EXAMPLE_PIN_NUM_B1,
            [EXAMPLE_HUB75_B2_IDX] = EXAMPLE_PIN_NUM_B2,
            [EXAMPLE_HUB75_OE_IDX] = EXAMPLE_PIN_NUM_OE,
            [EXAMPLE_HUB75_LATCH_IDX] = EXAMPLE_PIN_NUM_LATCH,
        },
        .output_clk_freq_hz = EXAMPLE_LED_MATRIX_PIXEL_CLOCK_HZ,
        .trans_queue_depth = 32,
        .max_transfer_size = EXAMPLE_LED_MATRIX_H_RES * sizeof(uint8_t) * 2, // 2 lines as the maximum transfer size
        .sample_edge = PARLIO_SAMPLE_EDGE_POS,
    };
    ESP_ERROR_CHECK(parlio_new_tx_unit(&config, &tx_unit));
    ESP_ERROR_CHECK(parlio_tx_unit_enable(tx_unit));

    // we use the transaction done callback to update the line address
    parlio_tx_event_callbacks_t parlio_cbs = {
        .on_trans_done = parlio_tx_line_done_cb,
    };
    ESP_ERROR_CHECK(parlio_tx_unit_register_event_callbacks(tx_unit, &parlio_cbs, led_line_control_gpio_bundle));

    ESP_LOGI(TAG, "Initialize LVGL library");
    lv_init();
    lv_display_t *display = lv_display_create(EXAMPLE_LED_MATRIX_H_RES, EXAMPLE_LED_MATRIX_V_RES);

    // allocate two full-screen draw buffers
    size_t draw_buffer_sz = EXAMPLE_LED_MATRIX_H_RES * EXAMPLE_LED_MATRIX_V_RES * sizeof(lv_color16_t);
    void *buf1 = heap_caps_calloc(1, draw_buffer_sz, MALLOC_CAP_INTERNAL);
    assert(buf1);
    void *buf2 = heap_caps_calloc(1, draw_buffer_sz, MALLOC_CAP_INTERNAL);
    assert(buf2);
    // Use RGB565 because RGB332 is not supported in LVGL9.2
    lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565);
    // initialize LVGL draw buffers
    // Since the rgb matrix needs to be refreshed dynamically, use fullmode
    lv_display_set_buffers(display, buf1, buf2, draw_buffer_sz, LV_DISPLAY_RENDER_MODE_FULL);
    // set the callback which can copy the rendered image to an area of the display
    lv_display_set_flush_cb(display, example_lvgl_flush_cb);

    ESP_LOGI(TAG, "Install LVGL tick timer");
    // increase the LVGL tick in the GPTimer alarm callback
    gptimer_handle_t lvgl_tick_timer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = EXAMPLE_GPTIMER_RESOLUTION_HZ,
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &lvgl_tick_timer));
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = EXAMPLE_LVGL_TICK_PERIOD_MS * EXAMPLE_GPTIMER_RESOLUTION_HZ / 1000,
        .reload_count = 0,
        .flags.auto_reload_on_alarm = true,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(lvgl_tick_timer, &alarm_config));
    gptimer_event_callbacks_t gptimer_cbs = {
        .on_alarm = gptimer_alarm_cb_lvgl_tick,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(lvgl_tick_timer, &gptimer_cbs, NULL));
    ESP_ERROR_CHECK(gptimer_enable(lvgl_tick_timer));

    ESP_LOGI(TAG, "Display LVGL UI");
    example_lvgl_demo_ui(display);

    ESP_ERROR_CHECK(gptimer_start(lvgl_tick_timer));

    // the frame buffer is flushed to the screen in a while loop without yield, which may cause task watchdog timeout
    uint8_t *payload = (uint8_t *)s_frame_buffer;
    parlio_transmit_config_t transmit_config = {
        .idle_value = 0x00, // the idle value will force the OE line to low, thus enable the output
    };
    while (1) {
        for (int i = 0; i < EXAMPLE_LED_MATRIX_V_RES / 2; i++) {
            ESP_ERROR_CHECK(parlio_tx_unit_transmit(tx_unit, payload + EXAMPLE_LED_MATRIX_H_RES * i,
                                                    EXAMPLE_LED_MATRIX_H_RES * sizeof(uint8_t) * 8, &transmit_config));
        }
        ESP_ERROR_CHECK(parlio_tx_unit_wait_all_done(tx_unit, -1));
        lv_timer_handler();
    }
}
