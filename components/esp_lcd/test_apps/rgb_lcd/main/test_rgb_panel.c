/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_ops.h"
#include "esp_random.h"
#include "esp_timer.h"
#include "esp_attr.h"
#include "test_rgb_board.h"
#include "esp_private/spi_flash_os.h"

#if CONFIG_LCD_RGB_ISR_IRAM_SAFE
#define TEST_LCD_CALLBACK_ATTR IRAM_ATTR
#else
#define TEST_LCD_CALLBACK_ATTR
#endif // CONFIG_LCD_RGB_ISR_IRAM_SAFE

#define TEST_IMG_SIZE (100 * 100 * sizeof(uint16_t))

static esp_lcd_panel_handle_t test_rgb_panel_initialization(size_t data_width, size_t bpp, size_t bb_pixels, bool refresh_on_demand,
                                                            esp_lcd_rgb_panel_vsync_cb_t vsync_cb, void *user_data)
{
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = data_width,
        .dma_burst_size = 64,
        .bounce_buffer_size_px = bb_pixels,
        .bits_per_pixel = bpp,
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .disp_gpio_num = TEST_LCD_DISP_EN_GPIO,
        .pclk_gpio_num = TEST_LCD_PCLK_GPIO,
        .vsync_gpio_num = TEST_LCD_VSYNC_GPIO,
        .hsync_gpio_num = TEST_LCD_HSYNC_GPIO,
        .de_gpio_num = TEST_LCD_DE_GPIO,
        .data_gpio_nums = {
            TEST_LCD_DATA0_GPIO,
            TEST_LCD_DATA1_GPIO,
            TEST_LCD_DATA2_GPIO,
            TEST_LCD_DATA3_GPIO,
            TEST_LCD_DATA4_GPIO,
            TEST_LCD_DATA5_GPIO,
            TEST_LCD_DATA6_GPIO,
            TEST_LCD_DATA7_GPIO,
            TEST_LCD_DATA8_GPIO,
            TEST_LCD_DATA9_GPIO,
            TEST_LCD_DATA10_GPIO,
            TEST_LCD_DATA11_GPIO,
            TEST_LCD_DATA12_GPIO,
            TEST_LCD_DATA13_GPIO,
            TEST_LCD_DATA14_GPIO,
            TEST_LCD_DATA15_GPIO,
        },
        .timings = {
            .pclk_hz = TEST_LCD_PIXEL_CLOCK_HZ,
            .h_res = TEST_LCD_H_RES,
            .v_res = TEST_LCD_V_RES,
            .hsync_back_porch = TEST_LCD_HBP,
            .hsync_front_porch = TEST_LCD_HFP,
            .hsync_pulse_width = TEST_LCD_HSYNC,
            .vsync_back_porch = TEST_LCD_VBP,
            .vsync_front_porch = TEST_LCD_VFP,
            .vsync_pulse_width = TEST_LCD_VSYNC,
        },
        .flags.fb_in_psram = 1, // allocate frame buffer in PSRAM
        .flags.refresh_on_demand = refresh_on_demand,
    };

    TEST_ESP_OK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));

    esp_lcd_rgb_panel_event_callbacks_t cbs = {
        .on_vsync = vsync_cb,
    };
    TEST_ESP_OK(esp_lcd_rgb_panel_register_event_callbacks(panel_handle, &cbs, user_data));
    TEST_ESP_OK(esp_lcd_panel_reset(panel_handle));
    TEST_ESP_OK(esp_lcd_panel_init(panel_handle));

    return panel_handle;
}

TEST_CASE("lcd_rgb_panel_stream_mode", "[lcd]")
{
    uint8_t *img = malloc(TEST_IMG_SIZE);
    TEST_ASSERT_NOT_NULL(img);

    printf("initialize RGB panel with stream mode\r\n");
    esp_lcd_panel_handle_t panel_handle = test_rgb_panel_initialization(16, 16, 0, false, NULL, NULL);
    printf("flush random color block\r\n");
    for (int i = 0; i < 200; i++) {
        uint8_t color_byte = esp_random() & 0xFF;
        int x_start = esp_random() % (TEST_LCD_H_RES - 100);
        int y_start = esp_random() % (TEST_LCD_V_RES - 100);
        memset(img, color_byte, TEST_IMG_SIZE);
        esp_lcd_panel_draw_bitmap(panel_handle, x_start, y_start, x_start + 100, y_start + 100, img);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    printf("delete RGB panel\r\n");
    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    free(img);
}

TEST_CASE("lcd_rgb_panel_8bit_interface", "[lcd]")
{
    uint8_t *img = malloc(100 * 100 * 3);
    TEST_ASSERT_NOT_NULL(img);

    printf("initialize RGB panel with stream mode\r\n");
    // bpp for RGB888 is 24
    esp_lcd_panel_handle_t panel_handle = test_rgb_panel_initialization(8, 24, 0, false, NULL, NULL);
    uint8_t color_byte = esp_random() & 0xFF;
    printf("flush random color block 0x%x\r\n", color_byte);
    int x_start = esp_random() % (TEST_LCD_H_RES - 100);
    int y_start = esp_random() % (TEST_LCD_V_RES - 100);
    memset(img, color_byte, 100 * 100 * 3);
    esp_lcd_panel_draw_bitmap(panel_handle, x_start, y_start, x_start + 100, y_start + 100, img);
    vTaskDelay(pdMS_TO_TICKS(2000));
    printf("delete RGB panel\r\n");
    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    free(img);
}

TEST_LCD_CALLBACK_ATTR static bool test_rgb_panel_trans_done(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t *edata, void *user_ctx)
{
    TaskHandle_t task_to_notify = (TaskHandle_t)user_ctx;
    BaseType_t high_task_wakeup;
    vTaskNotifyGiveFromISR(task_to_notify, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

TEST_CASE("lcd_rgb_panel_refresh_on_demand", "[lcd]")
{
    uint8_t *img = malloc(TEST_IMG_SIZE);
    TEST_ASSERT_NOT_NULL(img);
    TaskHandle_t cur_task = xTaskGetCurrentTaskHandle();

    printf("initialize RGB panel with non-stream mode\r\n");
    esp_lcd_panel_handle_t panel_handle = test_rgb_panel_initialization(16, 16, 0, true, test_rgb_panel_trans_done, cur_task);
    printf("flush random color block\r\n");
    for (int i = 0; i < 200; i++) {
        uint8_t color_byte = esp_random() & 0xFF;
        int x_start = esp_random() % (TEST_LCD_H_RES - 100);
        int y_start = esp_random() % (TEST_LCD_V_RES - 100);
        memset(img, color_byte, TEST_IMG_SIZE);
        esp_lcd_panel_draw_bitmap(panel_handle, x_start, y_start, x_start + 100, y_start + 100, img);
        esp_lcd_rgb_panel_refresh(panel_handle);
        // wait for flush done
        TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
    }

    printf("delete RGB panel\r\n");
    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    free(img);
}

TEST_CASE("lcd_rgb_panel_bounce_buffer", "[lcd]")
{
    uint8_t *img = malloc(TEST_IMG_SIZE);
    TEST_ASSERT_NOT_NULL(img);
    TaskHandle_t cur_task = xTaskGetCurrentTaskHandle();

    printf("initialize RGB panel with non-stream mode\r\n");
    esp_lcd_panel_handle_t panel_handle = test_rgb_panel_initialization(16, 16, 10 * TEST_LCD_H_RES, false, test_rgb_panel_trans_done, cur_task);
    printf("flush random color block\r\n");
    for (int i = 0; i < 200; i++) {
        uint8_t color_byte = esp_random() & 0xFF;
        int x_start = esp_random() % (TEST_LCD_H_RES - 100);
        int y_start = esp_random() % (TEST_LCD_V_RES - 100);
        memset(img, color_byte, TEST_IMG_SIZE);
        esp_lcd_panel_draw_bitmap(panel_handle, x_start, y_start, x_start + 100, y_start + 100, img);
        // wait for flush done
        TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000)));
    }

    printf("delete RGB panel\r\n");
    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    free(img);
}

TEST_CASE("lcd_rgb_panel_update_pclk", "[lcd]")
{
    uint8_t *img = malloc(TEST_IMG_SIZE);
    TEST_ASSERT_NOT_NULL(img);

    printf("initialize RGB panel with stream mode\r\n");
    esp_lcd_panel_handle_t panel_handle = test_rgb_panel_initialization(16, 16, 0, false, NULL, NULL);
    printf("flush one clock block to the LCD\r\n");
    uint8_t color_byte = esp_random() & 0xFF;
    int x_start = esp_random() % (TEST_LCD_H_RES - 100);
    int y_start = esp_random() % (TEST_LCD_V_RES - 100);
    memset(img, color_byte, TEST_IMG_SIZE);
    esp_lcd_panel_draw_bitmap(panel_handle, x_start, y_start, x_start + 100, y_start + 100, img);
    printf("The LCD driver should keep flushing the color block in the background (as it's in stream mode)\r\n");
    vTaskDelay(pdMS_TO_TICKS(1000));

    printf("Update the PCLK in the background\r\n");
    const uint32_t test_pclk_freq[] = {10000000, 12000000, 8000000};
    for (size_t i = 0; i < sizeof(test_pclk_freq) / sizeof(test_pclk_freq[0]); i++) {
        esp_lcd_rgb_panel_set_pclk(panel_handle, test_pclk_freq[i]);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    printf("delete RGB panel\r\n");
    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    free(img);
}

TEST_CASE("lcd_rgb_panel_restart", "[lcd]")
{
    uint8_t *img = malloc(TEST_IMG_SIZE);
    TEST_ASSERT_NOT_NULL(img);

    printf("initialize RGB panel with stream mode\r\n");
    esp_lcd_panel_handle_t panel_handle = test_rgb_panel_initialization(16, 16, 0, false, NULL, NULL);
    printf("flush one clock block to the LCD\r\n");
    uint8_t color_byte = esp_random() & 0xFF;
    int x_start = esp_random() % (TEST_LCD_H_RES - 100);
    int y_start = esp_random() % (TEST_LCD_V_RES - 100);
    memset(img, color_byte, TEST_IMG_SIZE);
    esp_lcd_panel_draw_bitmap(panel_handle, x_start, y_start, x_start + 100, y_start + 100, img);
    printf("The LCD driver should keep flushing the color block in the background (as it's in stream mode)\r\n");
    vTaskDelay(pdMS_TO_TICKS(1000));

    printf("Restart the DMA transmission in the background\r\n");
    TEST_ESP_OK(esp_lcd_rgb_panel_restart(panel_handle));
    vTaskDelay(pdMS_TO_TICKS(1000));

    printf("delete RGB panel\r\n");
    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    free(img);
}

TEST_CASE("lcd_rgb_panel_rotate", "[lcd]")
{
    const int w = 200;
    const int h = 100;
    uint64_t t = 0;
    uint8_t *img = malloc(w * h * sizeof(uint16_t));
    TEST_ASSERT_NOT_NULL(img);
    uint8_t color_byte = esp_random() & 0xFF;
    memset(img, color_byte, w * h * sizeof(uint16_t));

    printf("initialize RGB panel with stream mode\r\n");
    esp_lcd_panel_handle_t panel_handle = test_rgb_panel_initialization(16, 16, 0, false, NULL, NULL);

    printf("Update the rotation of panel\r\n");
    for (size_t i = 0; i < 8; i++) {
        esp_lcd_panel_swap_xy(panel_handle, i & 4);
        esp_lcd_panel_mirror(panel_handle, i & 2, i & 1);
        printf("Panel Rotation=%d\r\n", i);
        t = esp_timer_get_time();
        esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, w, h, img);
        t = esp_timer_get_time() - t;
        printf("@resolution %dx%d time per frame=%.2fMS\r\n", w, h, (float)t / 1000.0f);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    printf("delete RGB panel\r\n");
    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    free(img);
}

#if CONFIG_LCD_RGB_ISR_IRAM_SAFE
TEST_LCD_CALLBACK_ATTR static bool test_rgb_panel_count_in_callback(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t *edata, void *user_ctx)
{
    uint32_t *count = (uint32_t *)user_ctx;
    *count = *count + 1;
    return false;
}

static void IRAM_ATTR test_disable_flash_cache(void)
{
    // disable flash cache
    spi_flash_guard_get()->start();
    esp_rom_delay_us(200000);
    // enable flash cache
    spi_flash_guard_get()->end();
}

TEST_CASE("lcd_rgb_panel_iram_safe", "[lcd]")
{
    uint8_t *img = malloc(TEST_IMG_SIZE);
    TEST_ASSERT_NOT_NULL(img);

    uint32_t callback_calls = 0;

    printf("initialize RGB panel with stream mode\r\n");
    esp_lcd_panel_handle_t panel_handle = test_rgb_panel_initialization(16, 16, 0, false, test_rgb_panel_count_in_callback, &callback_calls);
    printf("flush one clock block to the LCD\r\n");
    uint8_t color_byte = esp_random() & 0xFF;
    int x_start = esp_random() % (TEST_LCD_H_RES - 100);
    int y_start = esp_random() % (TEST_LCD_V_RES - 100);
    memset(img, color_byte, TEST_IMG_SIZE);
    esp_lcd_panel_draw_bitmap(panel_handle, x_start, y_start, x_start + 100, y_start + 100, img);
    printf("The LCD driver should keep flushing the color block in the background (as it's in stream mode)\r\n");

    // read/write the SPI Flash by NVS APIs, the LCD driver should stay work
    printf("disable the cache for a while\r\n");
    test_disable_flash_cache();
    printf("the RGB ISR handle should keep working while the flash cache is disabled\r\n");
    printf("callback calls: %"PRIu32"\r\n", callback_calls);
    TEST_ASSERT(callback_calls > 2);

    printf("delete RGB panel\r\n");
    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    free(img);
}
#endif // CONFIG_LCD_RGB_ISR_IRAM_SAFE
