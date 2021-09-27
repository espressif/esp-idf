#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "test_utils.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_ops.h"
#include "soc/soc_caps.h"

#define TEST_LCD_H_RES         (480)
#define TEST_LCD_V_RES         (272)

#define TEST_LCD_VSYNC_GPIO    (48)
#define TEST_LCD_HSYNC_GPIO    (47)
#define TEST_LCD_DE_GPIO       (45)
#define TEST_LCD_PCLK_GPIO     (21)
#define TEST_LCD_DATA0_GPIO    (3)  // B0
#define TEST_LCD_DATA1_GPIO    (4)  // B1
#define TEST_LCD_DATA2_GPIO    (5)  // B2
#define TEST_LCD_DATA3_GPIO    (6)  // B3
#define TEST_LCD_DATA4_GPIO    (7)  // B4
#define TEST_LCD_DATA5_GPIO    (8)  // G0
#define TEST_LCD_DATA6_GPIO    (9)  // G1
#define TEST_LCD_DATA7_GPIO    (10) // G2
#define TEST_LCD_DATA8_GPIO    (11) // G3
#define TEST_LCD_DATA9_GPIO    (12) // G4
#define TEST_LCD_DATA10_GPIO   (13) // G5
#define TEST_LCD_DATA11_GPIO   (14) // R0
#define TEST_LCD_DATA12_GPIO   (15) // R1
#define TEST_LCD_DATA13_GPIO   (16) // R2
#define TEST_LCD_DATA14_GPIO   (17) // R3
#define TEST_LCD_DATA15_GPIO   (18) // R4
#define TEST_LCD_DISP_EN_GPIO  (39)

#if SOC_LCD_RGB_SUPPORTED
// RGB driver consumes a huge memory to save frame buffer, only test it with PSRAM enabled
#if CONFIG_SPIRAM_USE_MALLOC
TEST_CASE("lcd rgb lcd panel", "[lcd]")
{
#define TEST_IMG_SIZE (100 * 100 * sizeof(uint16_t))
    uint8_t *img = malloc(TEST_IMG_SIZE);
    TEST_ASSERT_NOT_NULL(img);

    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = 16,
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
            .pclk_hz = 12000000,
            .h_res = TEST_LCD_H_RES,
            .v_res = TEST_LCD_V_RES,
            .hsync_back_porch = 43,
            .hsync_front_porch = 2,
            .hsync_pulse_width = 1,
            .vsync_back_porch = 12,
            .vsync_front_porch = 1,
            .vsync_pulse_width = 1,
        },
        .flags.fb_in_psram = 1,
    };
    // Test stream mode and one-off mode
    for (int i = 0; i < 2; i++) {
        panel_config.flags.relax_on_idle = i;
        TEST_ESP_OK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));
        TEST_ESP_OK(esp_lcd_panel_reset(panel_handle));
        TEST_ESP_OK(esp_lcd_panel_init(panel_handle));

        for (int i = 0; i < 200; i++) {
            uint8_t color_byte = esp_random() & 0xFF;
            int x_start = esp_random() % (TEST_LCD_H_RES - 100);
            int y_start = esp_random() % (TEST_LCD_V_RES - 100);
            memset(img, color_byte, TEST_IMG_SIZE);
            esp_lcd_panel_draw_bitmap(panel_handle, x_start, y_start, x_start + 100, y_start + 100, img);
        }

        TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    }
    free(img);
#undef TEST_IMG_SIZE
}

// The following test shows a porting example of LVGL GUI library
// To run the LVGL tests, you need to clone the LVGL library into components directory firstly
#if CONFIG_LV_USE_USER_DATA
#include "test_lvgl_port.h"

static bool notify_lvgl_ready_to_flush(esp_lcd_panel_handle_t panel, esp_lcd_rgb_panel_event_data_t *edata, void *user_ctx)
{
    lv_disp_t *disp = *(lv_disp_t **)user_ctx;
    lv_disp_flush_ready(&disp->driver);
    return false;
}

TEST_CASE("lvgl gui with rgb interface", "[lcd][lvgl][ignore]")
{
    // initialize LVGL graphics library
    lv_disp_t *disp = NULL;
    lv_init();

    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = 16,
        .disp_gpio_num = -1,
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
            .pclk_hz = 6000000,
            .h_res = TEST_LCD_H_RES,
            .v_res = TEST_LCD_V_RES,
            .hsync_back_porch = 43,
            .hsync_front_porch = 2,
            .hsync_pulse_width = 1,
            .vsync_back_porch = 12,
            .vsync_front_porch = 1,
            .vsync_pulse_width = 1,
        },
        .flags.fb_in_psram = 1,
        .on_frame_trans_done = notify_lvgl_ready_to_flush,
        .user_ctx = &disp,
    };
    TEST_ESP_OK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));
    TEST_ESP_OK(esp_lcd_panel_reset(panel_handle));
    TEST_ESP_OK(esp_lcd_panel_init(panel_handle));

    test_lvgl_task_loop(panel_handle, TEST_LCD_H_RES, TEST_LCD_V_RES, &disp);
}
#endif // CONFIG_LV_USE_USER_DATA
#endif // CONFIG_SPIRAM_USE_MALLOC
#endif // SOC_LCD_RGB_SUPPORTED
