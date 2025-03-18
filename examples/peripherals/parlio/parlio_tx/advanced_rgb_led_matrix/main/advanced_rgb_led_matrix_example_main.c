/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "driver/parlio_tx.h"
#include "esp_timer.h"
#include "lvgl.h"

static const char *TAG = "example";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// Please update the following configuration according to your LED Matrix spec ///////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// color data, clock signal and control signals are powered by parlio_tx unit
#define EXAMPLE_HUB75_WIDTH      16 // 8-bit data width, used for transferring color data (RGB222) and control signals (OE and latch), 4-bit address width, 4-bit reserved
#define EXAMPLE_HUB75_R1_IDX     7 // R1 bit position
#define EXAMPLE_HUB75_R2_IDX     6 // R2 bit position
#define EXAMPLE_HUB75_LATCH_IDX  5 // LATCH bit position
#define EXAMPLE_HUB75_G1_IDX     4 // G1 bit position
#define EXAMPLE_HUB75_G2_IDX     3 // G2 bit position
#define EXAMPLE_HUB75_OE_IDX     2 // OE bit position
#define EXAMPLE_HUB75_B1_IDX     1 // B1 bit position
#define EXAMPLE_HUB75_B2_IDX     0 // B2 bit position
#define EXAMPLE_HUB75_NUM_A_IDX  8 // Address A bit position
#define EXAMPLE_HUB75_NUM_B_IDX  9 // Address B bit position
#define EXAMPLE_HUB75_NUM_C_IDX  10 // Address C bit position
#define EXAMPLE_HUB75_NUM_D_IDX  11 // Address D bit position
#define EXAMPLE_HUB75_RESERVED1_IDX  12 // Reserved bit position
#define EXAMPLE_HUB75_RESERVED2_IDX  13 // Reserved bit position
#define EXAMPLE_HUB75_RESERVED3_IDX  14 // Reserved bit position
#define EXAMPLE_HUB75_RESERVED4_IDX  15 // Reserved bit position

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
// We need to insert gap cycles to enable the line output (OE=0) for some time.
#define EXAMPLE_GAP_CYCLE_PER_LINE    40

// LED Matrix frame buffer, we use ping-pong buffer to prevent the frame from being tearing
static uint16_t s_frame_buffer[2][(EXAMPLE_LED_MATRIX_H_RES + EXAMPLE_GAP_CYCLE_PER_LINE) * EXAMPLE_LED_MATRIX_V_RES / 2] = {};

typedef struct {
    parlio_tx_unit_handle_t tx_unit;
    parlio_transmit_config_t transmit_config;
} parlio_tx_context_t;

// upper_half: pixel on the upper half screen
// lower_half: pixel on the lower half screen
static void merge_two_pixels(lv_color16_t *upper_half, lv_color16_t *lower_half, uint8_t buffer_idx, uint32_t pixel_idx)
{
    s_frame_buffer[buffer_idx][pixel_idx]  = (upper_half->red >> 4) << EXAMPLE_HUB75_R1_IDX | (upper_half->green >> 5) << EXAMPLE_HUB75_G1_IDX | (upper_half->blue >> 4) << EXAMPLE_HUB75_B1_IDX;
    s_frame_buffer[buffer_idx][pixel_idx] |= (lower_half->red >> 4) << EXAMPLE_HUB75_R2_IDX | (lower_half->green >> 5) << EXAMPLE_HUB75_G2_IDX | (lower_half->blue >> 4) << EXAMPLE_HUB75_B2_IDX;
    // OE=1: disable the output
    s_frame_buffer[buffer_idx][pixel_idx] |= 1 << EXAMPLE_HUB75_OE_IDX;
    // update the address line
    s_frame_buffer[buffer_idx][pixel_idx] |= (pixel_idx / (EXAMPLE_LED_MATRIX_H_RES + EXAMPLE_GAP_CYCLE_PER_LINE)) << EXAMPLE_HUB75_NUM_A_IDX;
}

extern void example_lvgl_demo_ui(lv_display_t *disp);

static void example_lvgl_flush_cb(lv_display_t *display, const lv_area_t *area, uint8_t *color_map)
{
    static uint8_t buffer_idx = 0;
    parlio_tx_context_t *user_ctx = lv_display_get_user_data(display);
    parlio_tx_unit_handle_t tx_unit = user_ctx->tx_unit;
    parlio_transmit_config_t transmit_config = user_ctx->transmit_config;
    lv_color16_t *upper_half = (lv_color16_t *)color_map;
    lv_color16_t *lower_half = (lv_color16_t *)color_map + EXAMPLE_LED_MATRIX_V_RES * EXAMPLE_LED_MATRIX_H_RES / 2;
    uint32_t pixel_idx = 0;
    for (int line = 0; line < EXAMPLE_LED_MATRIX_V_RES / 2; line++) {
        for (int col = 0; col < EXAMPLE_LED_MATRIX_H_RES - 1; col++) {
            merge_two_pixels(upper_half++, lower_half++, buffer_idx, pixel_idx++);
        }
        merge_two_pixels(upper_half++, lower_half++, buffer_idx, pixel_idx);
        // need special handling for the last pixel in each line
        // latch up at the end of each line
        s_frame_buffer[buffer_idx][pixel_idx++] |= (1 << EXAMPLE_HUB75_LATCH_IDX);
        // insert gap cycles to enable the line output (OE=0) for some time.
        for (int i = 0; i < EXAMPLE_GAP_CYCLE_PER_LINE; i++) {
            // we need to make sure the output is disabled during the second half of gap cycle, otherwise the screen will be ghosted.
            int output_disable = (i > EXAMPLE_GAP_CYCLE_PER_LINE / 2) ? 1 : 0;
            s_frame_buffer[buffer_idx][pixel_idx] = output_disable << EXAMPLE_HUB75_OE_IDX;
            s_frame_buffer[buffer_idx][pixel_idx] |= (pixel_idx / (EXAMPLE_LED_MATRIX_H_RES + EXAMPLE_GAP_CYCLE_PER_LINE)) << EXAMPLE_HUB75_NUM_A_IDX;
            pixel_idx++;
        }
    }
    parlio_tx_unit_transmit(tx_unit, s_frame_buffer[buffer_idx], (EXAMPLE_LED_MATRIX_V_RES / 2 * (EXAMPLE_LED_MATRIX_H_RES + EXAMPLE_GAP_CYCLE_PER_LINE)) * sizeof(uint16_t) * 8, &transmit_config);
    // switch to the next frame buffer
    buffer_idx ^= 0x01;
    lv_display_flush_ready(display);
}

static IRAM_ATTR void timer_alarm_cb_lvgl_tick(void* arg)
{
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}

void app_main(void)
{
    ESP_LOGI(TAG, "Install parallel IO TX unit");
    parlio_tx_unit_handle_t tx_unit = NULL;
    parlio_tx_unit_config_t config = {
        .clk_src = PARLIO_CLK_SRC_DEFAULT,
        .data_width = EXAMPLE_HUB75_WIDTH,
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
            [EXAMPLE_HUB75_NUM_A_IDX] = EXAMPLE_PIN_NUM_A,
            [EXAMPLE_HUB75_NUM_B_IDX] = EXAMPLE_PIN_NUM_B,
            [EXAMPLE_HUB75_NUM_C_IDX] = EXAMPLE_PIN_NUM_C,
            [EXAMPLE_HUB75_NUM_D_IDX] = EXAMPLE_PIN_NUM_D,
            [EXAMPLE_HUB75_RESERVED1_IDX] = -1,
            [EXAMPLE_HUB75_RESERVED2_IDX] = -1,
            [EXAMPLE_HUB75_RESERVED3_IDX] = -1,
            [EXAMPLE_HUB75_RESERVED4_IDX] = -1,
        },
        .output_clk_freq_hz = EXAMPLE_LED_MATRIX_PIXEL_CLOCK_HZ,
        .trans_queue_depth = 4,
        .max_transfer_size = (EXAMPLE_LED_MATRIX_H_RES + EXAMPLE_GAP_CYCLE_PER_LINE) * EXAMPLE_LED_MATRIX_V_RES / 2 * sizeof(uint16_t),  // full frame as the maximum transfer size
        .sample_edge = PARLIO_SAMPLE_EDGE_POS,
    };
    parlio_transmit_config_t transmit_config = {
        .idle_value = 0x00, // the idle value will take no effect since we are using the loop mode
        .flags.loop_transmission = true, // use loop mode. we rely on the LVGL flush callback to update the loop transmission payload
    };
    ESP_ERROR_CHECK(parlio_new_tx_unit(&config, &tx_unit));
    ESP_ERROR_CHECK(parlio_tx_unit_enable(tx_unit));

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
    // we do parlio transmission in the flush callback
    parlio_tx_context_t user_ctx = {
        .tx_unit = tx_unit,
        .transmit_config = transmit_config,
    };
    lv_display_set_user_data(display, &user_ctx);

    ESP_LOGI(TAG, "Install LVGL tick timer");
    // increase the LVGL tick in the esp_timer alarm callback
    const esp_timer_create_args_t timer_args = {
        .callback = &timer_alarm_cb_lvgl_tick,
        .name = "lvgl_tick_timer"
    };
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &lvgl_tick_timer));

    ESP_LOGI(TAG, "Display LVGL UI");
    example_lvgl_demo_ui(display);

    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000));

    uint32_t time_till_next_ms = 0;
    while (1) {
        if (time_till_next_ms > 500) {
            time_till_next_ms = 500;
        } else if (time_till_next_ms < 10) {
            time_till_next_ms = 10;
        }
        vTaskDelay(pdMS_TO_TICKS(time_till_next_ms));
        time_till_next_ms = lv_timer_handler();
    }
}
