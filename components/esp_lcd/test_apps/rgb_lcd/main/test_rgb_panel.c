/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "soc/soc_caps.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_ops.h"
#include "esp_timer.h"
#include "esp_attr.h"
#include "hal/lcd_ll.h"
#include "test_rgb_board.h"
#include "esp_private/spi_flash_os.h"
#include "esp_clk_tree.h"
#include "driver/ppa.h"
#include "esp_efuse.h"
#include "esp_macros.h"

#if CONFIG_LCD_RGB_ISR_IRAM_SAFE
#define TEST_LCD_CALLBACK_ATTR IRAM_ATTR
#else
#define TEST_LCD_CALLBACK_ATTR
#endif // CONFIG_LCD_RGB_ISR_IRAM_SAFE

#define TEST_IMG_SIZE (100 * 100 * sizeof(uint16_t))

static esp_lcd_panel_handle_t test_rgb_panel_initialization(size_t data_width, lcd_color_format_t in_color_format, lcd_color_format_t out_color_format,
                                                            size_t bb_pixels, lcd_clock_source_t clk_src, bool refresh_on_demand, bool user_fb,
                                                            esp_lcd_rgb_panel_vsync_cb_t vsync_cb, void *user_data)
{
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = data_width,
        .in_color_format = in_color_format,
        .out_color_format = out_color_format,
        .dma_burst_size = 64,
        .bounce_buffer_size_px = bb_pixels,
        .clk_src = clk_src,
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

    if (user_fb) {
        void *frame_buffer = heap_caps_aligned_calloc(64, 1, TEST_LCD_H_RES * TEST_LCD_V_RES * sizeof(uint16_t), MALLOC_CAP_SPIRAM);
        TEST_ASSERT_NOT_NULL(frame_buffer);
        panel_config.user_fbs[0] = frame_buffer;
    }

#if LCD_LL_SUPPORT(RGB2RGB_CONV)
    if (data_width == 24) {
        const int data_gpios[24] = {
            TEST_LCD_24BIT_DATA0_GPIO,
            TEST_LCD_24BIT_DATA1_GPIO,
            TEST_LCD_24BIT_DATA2_GPIO,
            TEST_LCD_24BIT_DATA3_GPIO,
            TEST_LCD_24BIT_DATA4_GPIO,
            TEST_LCD_24BIT_DATA5_GPIO,
            TEST_LCD_24BIT_DATA6_GPIO,
            TEST_LCD_24BIT_DATA7_GPIO,
            TEST_LCD_24BIT_DATA8_GPIO,
            TEST_LCD_24BIT_DATA9_GPIO,
            TEST_LCD_24BIT_DATA10_GPIO,
            TEST_LCD_24BIT_DATA11_GPIO,
            TEST_LCD_24BIT_DATA12_GPIO,
            TEST_LCD_24BIT_DATA13_GPIO,
            TEST_LCD_24BIT_DATA14_GPIO,
            TEST_LCD_24BIT_DATA15_GPIO,
            TEST_LCD_24BIT_DATA16_GPIO,
            TEST_LCD_24BIT_DATA17_GPIO,
            TEST_LCD_24BIT_DATA18_GPIO,
            TEST_LCD_24BIT_DATA19_GPIO,
            TEST_LCD_24BIT_DATA20_GPIO,
            TEST_LCD_24BIT_DATA21_GPIO,
            TEST_LCD_24BIT_DATA22_GPIO,
            TEST_LCD_24BIT_DATA23_GPIO,
        };
        memcpy(panel_config.data_gpio_nums, data_gpios, sizeof(data_gpios));
    }
#endif

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
    esp_lcd_panel_handle_t panel_handle = test_rgb_panel_initialization(16, LCD_COLOR_FMT_RGB565, LCD_COLOR_FMT_RGB565, 0, LCD_CLK_SRC_DEFAULT, false, false, NULL, NULL);
    printf("flush random color block\r\n");
    for (int i = 0; i < 200; i++) {
        uint8_t color_byte = rand() & 0xFF;
        int x_start = rand() % (TEST_LCD_H_RES - 100);
        int y_start = rand() % (TEST_LCD_V_RES - 100);
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
    esp_lcd_panel_handle_t panel_handle = test_rgb_panel_initialization(8, LCD_COLOR_FMT_RGB888, LCD_COLOR_FMT_RGB888, 0, LCD_CLK_SRC_DEFAULT, false, false, NULL, NULL);
    uint8_t color_byte = rand() & 0xFF;
    printf("flush random color block 0x%x\r\n", color_byte);
    int x_start = rand() % (TEST_LCD_H_RES - 100);
    int y_start = rand() % (TEST_LCD_V_RES - 100);
    memset(img, color_byte, 100 * 100 * 3);
    esp_lcd_panel_draw_bitmap(panel_handle, x_start, y_start, x_start + 100, y_start + 100, img);
    vTaskDelay(pdMS_TO_TICKS(2000));
    printf("delete RGB panel\r\n");
    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    free(img);
}

#if LCD_LL_SUPPORT(RGB2RGB_CONV)
TEST_CASE("lcd_rgb_panel_rgb2rgb_conversion", "[lcd]")
{
    uint8_t *img = malloc(100 * 100 * 3);
    TEST_ASSERT_NOT_NULL(img);

    printf("initialize RGB panel with RGB888 frame buffer and RGB565 output\r\n");
    esp_lcd_panel_handle_t panel_handle = test_rgb_panel_initialization(16, LCD_COLOR_FMT_RGB888, LCD_COLOR_FMT_RGB565,
                                                                        0, LCD_CLK_SRC_DEFAULT, false, false, NULL, NULL);
    printf("flush random color block\r\n");
    for (int i = 0; i < 200; i++) {
        uint8_t red = rand() & 0xFF;
        uint8_t green = rand() & 0xFF;
        uint8_t blue = rand() & 0xFF;
        int x_start = rand() % (TEST_LCD_H_RES - 100);
        int y_start = rand() % (TEST_LCD_V_RES - 100);
        for (int j = 0; j < 100 * 100; j++) {
            img[j * 3 + 0] = blue;
            img[j * 3 + 1] = green;
            img[j * 3 + 2] = red;
        }
        esp_lcd_panel_draw_bitmap(panel_handle, x_start, y_start, x_start + 100, y_start + 100, img);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    printf("delete RGB panel\r\n");
    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));

    vTaskDelay(pdMS_TO_TICKS(500));

    printf("initialize RGB panel with RGB565 frame buffer and RGB888 output\r\n");
    panel_handle = test_rgb_panel_initialization(24, LCD_COLOR_FMT_RGB565, LCD_COLOR_FMT_RGB888,
                                                 0, LCD_CLK_SRC_DEFAULT, false, false, NULL, NULL);
    printf("flush random color block\r\n");
    for (int i = 0; i < 200; i++) {
        uint8_t color_byte = rand() & 0xFF;
        int x_start = rand() % (TEST_LCD_H_RES - 100);
        int y_start = rand() % (TEST_LCD_V_RES - 100);
        memset(img, color_byte, TEST_IMG_SIZE);
        esp_lcd_panel_draw_bitmap(panel_handle, x_start, y_start, x_start + 100, y_start + 100, img);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    printf("delete RGB panel\r\n");
    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    free(img);
}
#endif

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
    esp_lcd_panel_handle_t panel_handle = test_rgb_panel_initialization(16, LCD_COLOR_FMT_RGB565, LCD_COLOR_FMT_RGB565, 0, LCD_CLK_SRC_DEFAULT, true, false, test_rgb_panel_trans_done, cur_task);
    printf("flush random color block\r\n");
    for (int i = 0; i < 200; i++) {
        uint8_t color_byte = rand() & 0xFF;
        int x_start = rand() % (TEST_LCD_H_RES - 100);
        int y_start = rand() % (TEST_LCD_V_RES - 100);
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
    esp_lcd_panel_handle_t panel_handle = test_rgb_panel_initialization(16, LCD_COLOR_FMT_RGB565, LCD_COLOR_FMT_RGB565, 20 * TEST_LCD_H_RES, LCD_CLK_SRC_DEFAULT, false, false, test_rgb_panel_trans_done, cur_task);
    printf("flush random color block\r\n");
    for (int i = 0; i < 200; i++) {
        uint8_t color_byte = rand() & 0xFF;
        int x_start = rand() % (TEST_LCD_H_RES - 100);
        int y_start = rand() % (TEST_LCD_V_RES - 100);
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
    esp_lcd_panel_handle_t panel_handle = test_rgb_panel_initialization(16, LCD_COLOR_FMT_RGB565, LCD_COLOR_FMT_RGB565, 0, LCD_CLK_SRC_DEFAULT, false, false, NULL, NULL);
    printf("flush one clock block to the LCD\r\n");
    uint8_t color_byte = rand() & 0xFF;
    int x_start = rand() % (TEST_LCD_H_RES - 100);
    int y_start = rand() % (TEST_LCD_V_RES - 100);
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
#if CONFIG_IDF_TARGET_ESP32S3
    uint8_t *img = malloc(TEST_IMG_SIZE);
    TEST_ASSERT_NOT_NULL(img);

    printf("initialize RGB panel with stream mode\r\n");
    esp_lcd_panel_handle_t panel_handle = test_rgb_panel_initialization(16, LCD_COLOR_FMT_RGB565, LCD_COLOR_FMT_RGB565, 0, LCD_CLK_SRC_DEFAULT, false, false, NULL, NULL);
    printf("flush one clock block to the LCD\r\n");
    uint8_t color_byte = rand() & 0xFF;
    int x_start = rand() % (TEST_LCD_H_RES - 100);
    int y_start = rand() % (TEST_LCD_V_RES - 100);
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
#else
    printf("initialize RGB panel with stream mode\r\n");
    esp_lcd_panel_handle_t panel_handle = test_rgb_panel_initialization(16, LCD_COLOR_FMT_RGB565, LCD_COLOR_FMT_RGB565, 0, LCD_CLK_SRC_DEFAULT, false, false, NULL, NULL);
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_SUPPORTED, esp_lcd_rgb_panel_restart(panel_handle));
    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
#endif
}

TEST_CASE("lcd_rgb_panel_rotate", "[lcd]")
{
    const int w = 200;
    const int h = 100;
    uint64_t t = 0;
    uint8_t *img = malloc(w * h * sizeof(uint16_t));
    TEST_ASSERT_NOT_NULL(img);
    uint8_t color_byte = rand() & 0xFF;
    memset(img, color_byte, w * h * sizeof(uint16_t));

    printf("initialize RGB panel with stream mode\r\n");
    esp_lcd_panel_handle_t panel_handle = test_rgb_panel_initialization(16, LCD_COLOR_FMT_RGB565, LCD_COLOR_FMT_RGB565, 0, LCD_CLK_SRC_DEFAULT, false, false, NULL, NULL);

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

TEST_CASE("lcd_rgb_panel_user_frame_buffer", "[lcd]")
{
    uint8_t *img = malloc(TEST_IMG_SIZE);
    TEST_ASSERT_NOT_NULL(img);

    printf("initialize RGB panel with stream mode\r\n");
    esp_lcd_panel_handle_t panel_handle = test_rgb_panel_initialization(16, LCD_COLOR_FMT_RGB565, LCD_COLOR_FMT_RGB565, 0, LCD_CLK_SRC_DEFAULT, false, true, NULL, NULL);

    printf("flush one clock block to the LCD\r\n");
    uint8_t color_byte = rand() & 0xFF;
    int x_start = rand() % (TEST_LCD_H_RES - 100);
    int y_start = rand() % (TEST_LCD_V_RES - 100);
    memset(img, color_byte, TEST_IMG_SIZE);
    esp_lcd_panel_draw_bitmap(panel_handle, x_start, y_start, x_start + 100, y_start + 100, img);
    printf("The LCD driver should keep flushing the color block in the background (as it's in stream mode)\r\n");
    vTaskDelay(pdMS_TO_TICKS(1000));

    // get the user frame buffer to free it
    void *user_frame_buffer;
    esp_lcd_rgb_panel_get_frame_buffer(panel_handle, 1, &user_frame_buffer);
    TEST_ASSERT_NOT_NULL(user_frame_buffer);

    printf("delete RGB panel\r\n");
    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    free(img);
    free(user_frame_buffer);
}

#if SOC_CLK_APLL_SUPPORTED
TEST_CASE("lcd_rgb_panel_use_apll", "[lcd]")
{
    uint8_t *img = malloc(TEST_IMG_SIZE);
    TEST_ASSERT_NOT_NULL(img);

    printf("set APLL frequency\r\n");
    uint32_t real_freq = 0;
    TEST_ESP_OK(esp_clk_tree_src_set_freq_hz(SOC_MOD_CLK_APLL, 120 * 1000 * 1000, &real_freq));
    printf("APLL frequency: %"PRIu32" Hz\r\n", real_freq);

    printf("initialize RGB panel with stream mode\r\n");
    esp_lcd_panel_handle_t panel_handle = test_rgb_panel_initialization(16, LCD_COLOR_FMT_RGB565, LCD_COLOR_FMT_RGB565, 0, LCD_CLK_SRC_APLL, false, false, NULL, NULL);
    printf("flush random color block\r\n");
    for (int i = 0; i < 200; i++) {
        uint8_t color_byte = rand() & 0xFF;
        int x_start = rand() % (TEST_LCD_H_RES - 100);
        int y_start = rand() % (TEST_LCD_V_RES - 100);
        memset(img, color_byte, TEST_IMG_SIZE);
        esp_lcd_panel_draw_bitmap(panel_handle, x_start, y_start, x_start + 100, y_start + 100, img);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    printf("delete RGB panel\r\n");
    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    free(img);
}
#endif // SOC_CLK_APLL_SUPPORTED

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
    esp_lcd_panel_handle_t panel_handle = test_rgb_panel_initialization(16, LCD_COLOR_FMT_RGB565, LCD_COLOR_FMT_RGB565, 0, LCD_CLK_SRC_DEFAULT, false, false, test_rgb_panel_count_in_callback, &callback_calls);
    printf("flush one clock block to the LCD\r\n");
    uint8_t color_byte = rand() & 0xFF;
    int x_start = rand() % (TEST_LCD_H_RES - 100);
    int y_start = rand() % (TEST_LCD_V_RES - 100);
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

TEST_CASE("lcd_rgb_panel_draw_bitmap_2d", "[lcd]")
{
    // Allocate a larger source image (200x200) for testing partial copy
    size_t src_img_size = 200 * 200 * sizeof(uint16_t);
    uint8_t *src_img = malloc(src_img_size);
    TEST_ASSERT_NOT_NULL(src_img);

    printf("initialize RGB panel with stream mode\r\n");
    esp_lcd_panel_handle_t panel_handle = test_rgb_panel_initialization(16, LCD_COLOR_FMT_RGB565, LCD_COLOR_FMT_RGB565, 0, LCD_CLK_SRC_DEFAULT, false, false, NULL, NULL);

    printf("Draw bitmap 2D by CPU - copy partial region from source to destination\r\n");
    for (int i = 0; i < 100; i++) {
        int x_start = rand() % (TEST_LCD_H_RES - 100);
        int y_start = rand() % (TEST_LCD_V_RES - 100);
        // Fill source image with random pattern
        uint8_t color_byte = rand() & 0xFF;
        memset(src_img, color_byte, src_img_size / 2);
        color_byte = rand() & 0xFF;
        memset(src_img + src_img_size / 2, color_byte, src_img_size / 2);

        // Copy a 100x100 region from source (starting at 50,50) to destination at (x_start, y_start)
        // Source image is 200x200, we copy region from (50,50) to (150,150)
        esp_lcd_panel_draw_bitmap_2d(panel_handle, x_start, y_start, x_start + 100, y_start + 100,
                                     src_img, 200, 200, 50, 50, 150, 150);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    vTaskDelay(pdMS_TO_TICKS(1000));

    printf("delete RGB panel\r\n");
    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    free(src_img);
}

#if SOC_HAS(DMA2D)
TEST_CASE("lcd_rgb_panel_dma2d_hook", "[lcd]")
{
    // Allocate a larger source image (200x200) for testing partial copy
    size_t src_img_size = 200 * 200 * sizeof(uint16_t);
    size_t buffer_alignment = 1;
    if (esp_efuse_is_flash_encryption_enabled()) {
        buffer_alignment = SOC_MEMSPI_ENCRYPTION_ALIGNMENT;
    }
    uint8_t *src_img = heap_caps_aligned_calloc(buffer_alignment, 1, src_img_size, MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM);
    TEST_ASSERT_NOT_NULL(src_img);

    printf("initialize RGB panel with stream mode\r\n");
    esp_lcd_panel_handle_t panel_handle = test_rgb_panel_initialization(16, LCD_COLOR_FMT_RGB565, LCD_COLOR_FMT_RGB565, 0, LCD_CLK_SRC_DEFAULT, false, false, NULL, NULL);

    printf("Draw bitmap 2D by CPU first\r\n");
    for (int i = 0; i < 50; i++) {
        int x_start = rand() % (TEST_LCD_H_RES - 100);
        int y_start = rand() % (TEST_LCD_V_RES - 100);
        uint8_t color_byte = rand() & 0xFF;
        memset(src_img, color_byte, src_img_size / 2);
        color_byte = rand() & 0xFF;
        memset(src_img + src_img_size / 2, color_byte, src_img_size / 2);
        esp_lcd_panel_draw_bitmap_2d(panel_handle, x_start, y_start, x_start + 100, y_start + 100,
                                     src_img, 200, 200, 50, 50, 150, 150);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    vTaskDelay(pdMS_TO_TICKS(1000));

    size_t test_block_size = 100;
    size_t start_alignment = 1;
    size_t src_x_start = 50;
    size_t src_y_start = 50;
    if (esp_efuse_is_flash_encryption_enabled()) {
        test_block_size = ESP_ALIGN_DOWN(test_block_size, SOC_MEMSPI_ENCRYPTION_ALIGNMENT);
        start_alignment = SOC_MEMSPI_ENCRYPTION_ALIGNMENT;
        src_x_start = ESP_ALIGN_DOWN(src_x_start, SOC_MEMSPI_ENCRYPTION_ALIGNMENT);
        src_y_start = ESP_ALIGN_DOWN(src_y_start, SOC_MEMSPI_ENCRYPTION_ALIGNMENT);
    }

    printf("Enable DMA2D draw bitmap hook\r\n");
    TEST_ESP_OK(esp_lcd_rgb_panel_enable_dma2d(panel_handle));
    printf("Draw bitmap 2D by DMA2D\r\n");
    for (int i = 0; i < 100; i++) {
        int x_start = ESP_ALIGN_DOWN(rand() % (TEST_LCD_H_RES - test_block_size), start_alignment);
        int y_start = ESP_ALIGN_DOWN(rand() % (TEST_LCD_V_RES - test_block_size), start_alignment);
        uint8_t color_byte = rand() & 0xFF;
        memset(src_img, color_byte, src_img_size / 2);
        color_byte = rand() & 0xFF;
        memset(src_img + src_img_size / 2, color_byte, src_img_size / 2);
        esp_lcd_panel_draw_bitmap_2d(panel_handle, x_start, y_start, x_start + test_block_size, y_start + test_block_size,
                                     src_img, 200, 200, src_x_start, src_y_start, src_x_start + test_block_size, src_y_start + test_block_size);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    vTaskDelay(pdMS_TO_TICKS(1000));

    printf("Disable DMA2D draw bitmap hook\r\n");
    TEST_ESP_OK(esp_lcd_rgb_panel_disable_dma2d(panel_handle));
    vTaskDelay(pdMS_TO_TICKS(1000));

    printf("delete RGB panel\r\n");
    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    free(src_img);
}
#endif // SOC_HAS(DMA2D)

#if SOC_HAS(PPA)
typedef struct {
    ppa_client_handle_t ppa_srm_handle;
    esp_lcd_draw_bitmap_hook_data_t hook_data;
    SemaphoreHandle_t draw_sem;
    esp_lcd_panel_handle_t panel;
} test_rgb_panel_draw_bitmap_hook_ctx_t;

typedef struct {
    uint32_t count;
} test_rgb_panel_color_trans_done_callback_ctx_t;

TEST_LCD_CALLBACK_ATTR static bool test_ppa_srm_trans_done_callback(ppa_client_handle_t ppa_client, ppa_event_data_t *edata, void *user_ctx)
{
    bool need_yield = false;
    test_rgb_panel_draw_bitmap_hook_ctx_t *hook_ctx = (test_rgb_panel_draw_bitmap_hook_ctx_t *)user_ctx;
    esp_lcd_draw_bitmap_hook_data_t *hook_data = &hook_ctx->hook_data;

    if (hook_data->on_hook_end) {
        if (hook_data->on_hook_end(hook_ctx->panel)) {
            need_yield = true;
        }
    }

    BaseType_t task_woken = pdFALSE;
    xSemaphoreGiveFromISR(hook_ctx->draw_sem, &task_woken);
    if (task_woken == pdTRUE) {
        need_yield = true;
    }

    return need_yield;
}

static esp_err_t test_draw_bitmap_hook_ppa(esp_lcd_panel_handle_t panel, const esp_lcd_draw_bitmap_hook_data_t *hook_data, void *user_ctx)
{
    test_rgb_panel_draw_bitmap_hook_ctx_t *hook_ctx = (test_rgb_panel_draw_bitmap_hook_ctx_t *)user_ctx;
    ppa_client_handle_t ppa_srm_handle = hook_ctx->ppa_srm_handle;
    xSemaphoreTake(hook_ctx->draw_sem, portMAX_DELAY);
    memcpy(&hook_ctx->hook_data, hook_data, sizeof(esp_lcd_draw_bitmap_hook_data_t));
    ppa_srm_oper_config_t srm_config = {
        .in.buffer = hook_data->src_data,
        .in.pic_w = hook_data->src_x_size,
        .in.pic_h = hook_data->src_y_size,
        .in.block_w = hook_data->src_x_end - hook_data->src_x_start,
        .in.block_h = hook_data->src_y_end - hook_data->src_y_start,
        .in.block_offset_x = hook_data->src_x_start,
        .in.block_offset_y = hook_data->src_y_start,
        .in.srm_cm = PPA_SRM_COLOR_MODE_RGB565,
        .out.buffer = hook_data->dst_data,
        .out.buffer_size = hook_data->dst_x_size * hook_data->dst_y_size * hook_data->bits_per_pixel / 8,
        .out.pic_w = hook_data->dst_x_size,
        .out.pic_h = hook_data->dst_y_size,
        .out.block_offset_x = hook_data->dst_x_start,
        .out.block_offset_y = hook_data->dst_y_start,
        .out.srm_cm = PPA_SRM_COLOR_MODE_RGB565,
        .rotation_angle = PPA_SRM_ROTATION_ANGLE_90,
        .scale_x = 0.5,
        .scale_y = 0.5,
        .rgb_swap = 0,
        .byte_swap = 0,
        .mode = PPA_TRANS_MODE_NON_BLOCKING,
        .user_data = hook_ctx,
    };

    ppa_event_callbacks_t ppa_srm_event_callbacks = {
        .on_trans_done = test_ppa_srm_trans_done_callback,
    };
    TEST_ESP_OK(ppa_client_register_event_callbacks(ppa_srm_handle, &ppa_srm_event_callbacks));

    TEST_ESP_OK(ppa_do_scale_rotate_mirror(ppa_srm_handle, &srm_config));

    return ESP_OK;
}

TEST_LCD_CALLBACK_ATTR static bool test_rgb_panel_color_trans_done_count_callback(esp_lcd_panel_handle_t panel, const esp_lcd_rgb_panel_event_data_t *edata, void *user_ctx)
{
    test_rgb_panel_color_trans_done_callback_ctx_t *color_trans_done_ctx = (test_rgb_panel_color_trans_done_callback_ctx_t *)user_ctx;
    color_trans_done_ctx->count++;
    return false;
}

TEST_CASE("lcd_rgb_panel_ppa_hook", "[lcd]")
{
    if (esp_efuse_is_flash_encryption_enabled()) {
        TEST_PASS_MESSAGE("PPA SRM is not compatible with encrypted memory, skip this test");
    }

    // Allocate a larger source image (200x200) for testing
    size_t src_img_size = 200 * 200 * sizeof(uint16_t);
    uint8_t *src_img = malloc(src_img_size);
    TEST_ASSERT_NOT_NULL(src_img);

    printf("initialize RGB panel with stream mode\r\n");
    esp_lcd_panel_handle_t panel_handle = test_rgb_panel_initialization(16, LCD_COLOR_FMT_RGB565, LCD_COLOR_FMT_RGB565, 0, LCD_CLK_SRC_DEFAULT, false, false, NULL, NULL);

    SemaphoreHandle_t draw_sem = xSemaphoreCreateBinaryWithCaps(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(draw_sem);
    xSemaphoreGive(draw_sem);

    // use PPA to scale and rotate the image in draw bitmap hook
    ppa_client_handle_t ppa_srm_handle = NULL;
    ppa_client_config_t ppa_srm_config = {
        .oper_type = PPA_OPERATION_SRM,
        .max_pending_trans_num = 1,
    };
    TEST_ESP_OK(ppa_register_client(&ppa_srm_config, &ppa_srm_handle));

    esp_lcd_rgb_panel_event_callbacks_t cbs = {
        .on_color_trans_done = test_rgb_panel_color_trans_done_count_callback,
    };

    test_rgb_panel_color_trans_done_callback_ctx_t color_trans_done_ctx = {
        .count = 0,
    };
    TEST_ESP_OK(esp_lcd_rgb_panel_register_event_callbacks(panel_handle, &cbs, &color_trans_done_ctx));

    printf("Add PPA draw bitmap hook\r\n");
    esp_lcd_panel_hooks_t hooks = {
        .draw_bitmap_hook = test_draw_bitmap_hook_ppa,
    };
    test_rgb_panel_draw_bitmap_hook_ctx_t hook_ctx = {
        .draw_sem = draw_sem,
        .ppa_srm_handle = ppa_srm_handle,
        .panel = panel_handle,
    };
    TEST_ESP_OK(esp_lcd_rgb_panel_register_hooks(panel_handle, &hooks, &hook_ctx));

    for (int i = 0; i < 100; i++) {
        int x_start = rand() % (TEST_LCD_H_RES - 100);
        int y_start = rand() % (TEST_LCD_V_RES - 100);
        uint8_t color_byte = rand() & 0xFF;
        memset(src_img, color_byte, src_img_size / 2);
        color_byte = rand() & 0xFF;
        memset(src_img + src_img_size / 2, color_byte, src_img_size / 2);
        esp_lcd_panel_draw_bitmap_2d(panel_handle, x_start, y_start, x_start + 50, y_start + 50,
                                     src_img, 200, 200, 0, 0, 200, 200);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    xSemaphoreTake(draw_sem, portMAX_DELAY);
    TEST_ASSERT_EQUAL_INT(100, color_trans_done_ctx.count);

    hooks.draw_bitmap_hook = NULL;
    TEST_ESP_OK(esp_lcd_rgb_panel_register_hooks(panel_handle, &hooks, NULL));
    TEST_ESP_OK(ppa_unregister_client(ppa_srm_handle));

    printf("delete RGB panel\r\n");
    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    vSemaphoreDeleteWithCaps(draw_sem);
    free(src_img);
}
#endif // SOC_HAS(PPA)
