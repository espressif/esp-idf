/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_ops.h"
#include "esp_random.h"
#include "esp_attr.h"
#include "nvs_flash.h"
#include "test_rgb_board.h"

#if CONFIG_LCD_RGB_ISR_IRAM_SAFE
#define TEST_LCD_CALLBACK_ATTR IRAM_ATTR
#else
#define TEST_LCD_CALLBACK_ATTR
#endif // CONFIG_LCD_RGB_ISR_IRAM_SAFE

#define TEST_IMG_SIZE (100 * 100 * sizeof(uint16_t))

void test_app_include_rgb_lcd(void)
{
}

static esp_lcd_panel_handle_t test_rgb_panel_initialization(bool stream_mode, esp_lcd_rgb_panel_frame_trans_done_cb_t cb, void *user_data)
{
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = 16,
        .psram_trans_align = 64,
        .clk_src = LCD_CLK_SRC_PLL160M,
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
            .hsync_back_porch = 68,
            .hsync_front_porch = 20,
            .hsync_pulse_width = 5,
            .vsync_back_porch = 18,
            .vsync_front_porch = 4,
            .vsync_pulse_width = 1,
        },
        .on_frame_trans_done = cb,
        .user_ctx = user_data,
        .flags.fb_in_psram = 1, // allocate frame buffer in PSRAM
        .flags.relax_on_idle = !stream_mode,
    };

    TEST_ESP_OK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));
    TEST_ESP_OK(esp_lcd_panel_reset(panel_handle));
    TEST_ESP_OK(esp_lcd_panel_init(panel_handle));

    return panel_handle;
}

TEST_CASE("lcd_rgb_panel_stream_mode", "[lcd]")
{
    uint8_t *img = malloc(TEST_IMG_SIZE);
    TEST_ASSERT_NOT_NULL(img);

    printf("initialize RGB panel with stream mode\r\n");
    esp_lcd_panel_handle_t panel_handle = test_rgb_panel_initialization(true, NULL, NULL);
    printf("flush random color block\r\n");
    for (int i = 0; i < 200; i++) {
        uint8_t color_byte = esp_random() & 0xFF;
        int x_start = esp_random() % (TEST_LCD_H_RES - 100);
        int y_start = esp_random() % (TEST_LCD_V_RES - 100);
        memset(img, color_byte, TEST_IMG_SIZE);
        esp_lcd_panel_draw_bitmap(panel_handle, x_start, y_start, x_start + 100, y_start + 100, img);
    }
    printf("delete RGB panel\r\n");
    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    free(img);
}

TEST_LCD_CALLBACK_ATTR static bool test_rgb_panel_trans_done(esp_lcd_panel_handle_t panel, esp_lcd_rgb_panel_event_data_t *edata, void *user_ctx)
{
    TaskHandle_t task_to_notify = (TaskHandle_t)user_ctx;
    BaseType_t high_task_wakeup;
    vTaskNotifyGiveFromISR(task_to_notify, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

TEST_CASE("lcd_rgb_panel_one_shot_mode", "[lcd]")
{
    uint8_t *img = malloc(TEST_IMG_SIZE);
    TEST_ASSERT_NOT_NULL(img);
    TaskHandle_t cur_task = xTaskGetCurrentTaskHandle();

    printf("initialize RGB panel with ont-shot mode\r\n");
    esp_lcd_panel_handle_t panel_handle = test_rgb_panel_initialization(false, test_rgb_panel_trans_done, cur_task);
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

#if CONFIG_LCD_RGB_ISR_IRAM_SAFE
TEST_LCD_CALLBACK_ATTR static bool test_rgb_panel_count_in_callback(esp_lcd_panel_handle_t panel, esp_lcd_rgb_panel_event_data_t *edata, void *user_ctx)
{
    uint32_t *count = (uint32_t *)user_ctx;
    *count = *count + 1;
    return false;
}

TEST_CASE("lcd_rgb_panel_with_nvs_read_write", "[lcd]")
{
    uint8_t *img = malloc(TEST_IMG_SIZE);
    TEST_ASSERT_NOT_NULL(img);

    uint32_t callback_calls = 0;

    printf("initialize RGB panel with stream mode\r\n");
    esp_lcd_panel_handle_t panel_handle = test_rgb_panel_initialization(true, test_rgb_panel_count_in_callback, &callback_calls);
    printf("flush one clock block to the LCD\r\n");
    uint8_t color_byte = esp_random() & 0xFF;
    int x_start = esp_random() % (TEST_LCD_H_RES - 100);
    int y_start = esp_random() % (TEST_LCD_V_RES - 100);
    memset(img, color_byte, TEST_IMG_SIZE);
    esp_lcd_panel_draw_bitmap(panel_handle, x_start, y_start, x_start + 100, y_start + 100, img);
    printf("The LCD driver should keep flushing the color block in the background (as it's in stream mode)\r\n");

    // read/write the SPI Flash by NVS APIs, the LCD driver should stay work
    printf("initialize NVS flash\r\n");
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        TEST_ESP_OK(nvs_flash_erase());
        // Retry nvs_flash_init
        err = nvs_flash_init();
    }
    TEST_ESP_OK(err);
    printf("open NVS storage\r\n");
    nvs_handle_t my_handle;
    TEST_ESP_OK(nvs_open("storage", NVS_READWRITE, &my_handle));
    TEST_ESP_OK(nvs_erase_all(my_handle));
    int test_count;
    for (int i = 0; i < 50; i++) {
        printf("write %d to NVS partition\r\n", i);
        TEST_ESP_OK(nvs_set_i32(my_handle, "test_count", i));
        TEST_ESP_OK(nvs_commit(my_handle));
        TEST_ESP_OK(nvs_get_i32(my_handle, "test_count", &test_count));
        TEST_ASSERT_EQUAL(i, test_count);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    printf("close NVS storage\r\n");
    nvs_close(my_handle);
    TEST_ESP_OK(nvs_flash_deinit());

    TEST_ASSERT(callback_calls > 50);

    printf("delete RGB panel\r\n");
    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    free(img);
}
#endif // CONFIG_LCD_RGB_ISR_IRAM_SAFE
