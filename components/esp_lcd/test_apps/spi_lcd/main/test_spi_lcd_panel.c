/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_commands.h"
#include "soc/soc_caps.h"
#include "test_spi_board.h"

#define TEST_SPI_HOST_ID  SPI2_HOST

void test_spi_lcd_common_initialize(esp_lcd_panel_io_handle_t *io_handle, esp_lcd_panel_io_color_trans_done_cb_t on_color_trans_done,
                                    void *user_data, int cmd_bits, int param_bits, bool oct_mode)
{
    // turn off backlight
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << TEST_LCD_BK_LIGHT_GPIO
    };
    TEST_ESP_OK(gpio_config(&bk_gpio_config));
    gpio_set_level(TEST_LCD_BK_LIGHT_GPIO, 0);

    spi_bus_config_t buscfg = {
        .sclk_io_num = TEST_LCD_PCLK_GPIO,
        .mosi_io_num = TEST_LCD_DATA0_GPIO,
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 100 * 100 * sizeof(uint16_t),
    };
#if SOC_SPI_SUPPORT_OCT
    if (oct_mode) {
        buscfg.data1_io_num = TEST_LCD_DATA1_GPIO;
        buscfg.data2_io_num = TEST_LCD_DATA2_GPIO;
        buscfg.data3_io_num = TEST_LCD_DATA3_GPIO;
        buscfg.data4_io_num = TEST_LCD_DATA4_GPIO;
        buscfg.data5_io_num = TEST_LCD_DATA5_GPIO;
        buscfg.data6_io_num = TEST_LCD_DATA6_GPIO;
        buscfg.data7_io_num = TEST_LCD_DATA7_GPIO;
        buscfg.flags = SPICOMMON_BUSFLAG_OCTAL;
    }
#endif
    TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST_ID, &buscfg, SPI_DMA_CH_AUTO));

    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = TEST_LCD_DC_GPIO,
        .cs_gpio_num = TEST_LCD_CS_GPIO,
        .pclk_hz = TEST_LCD_PIXEL_CLOCK_HZ,
        .spi_mode = 0,
        .trans_queue_depth = 10,
        .lcd_cmd_bits = cmd_bits,
        .lcd_param_bits = param_bits,
        .on_color_trans_done = on_color_trans_done,
        .user_ctx = user_data,
    };
#if SOC_SPI_SUPPORT_OCT
    if (oct_mode) {
        io_config.flags.octal_mode = 1;
        io_config.spi_mode = 3;
    }
#endif
    TEST_ESP_OK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)TEST_SPI_HOST_ID, &io_config, io_handle));
}

#define TEST_IMG_SIZE (200 * 200 * sizeof(uint16_t))

static void lcd_panel_test(esp_lcd_panel_io_handle_t io_handle, esp_lcd_panel_handle_t panel_handle)
{
    uint8_t *img = heap_caps_malloc(TEST_IMG_SIZE, MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(img);

    esp_lcd_panel_reset(panel_handle);
    esp_lcd_panel_init(panel_handle);
    esp_lcd_panel_invert_color(panel_handle, true);
    // the gap is LCD panel specific, even panels with the same driver IC, can have different gap value
    esp_lcd_panel_set_gap(panel_handle, 0, 20);
    // turn on display
    esp_lcd_panel_disp_on_off(panel_handle, true);
    // turn on backlight
    gpio_set_level(TEST_LCD_BK_LIGHT_GPIO, 1);

    for (int i = 0; i < 100; i++) {
        uint8_t color_byte = rand() & 0xFF;
        int x_start = rand() % (TEST_LCD_H_RES - 200);
        int y_start = rand() % (TEST_LCD_V_RES - 200);
        memset(img, color_byte, TEST_IMG_SIZE);
        esp_lcd_panel_draw_bitmap(panel_handle, x_start, y_start, x_start + 200, y_start + 200, img);
    }

    printf("go into sleep mode\r\n");
    esp_lcd_panel_disp_sleep(panel_handle, true);
    vTaskDelay(pdMS_TO_TICKS(500));
    printf("exit sleep mode\r\n");
    esp_lcd_panel_disp_sleep(panel_handle, false);

    for (int i = 0; i < 100; i++) {
        uint8_t color_byte = rand() & 0xFF;
        int x_start = rand() % (TEST_LCD_H_RES - 200);
        int y_start = rand() % (TEST_LCD_V_RES - 200);
        memset(img, color_byte, TEST_IMG_SIZE);
        esp_lcd_panel_draw_bitmap(panel_handle, x_start, y_start, x_start + 200, y_start + 200, img);
    }

    printf("turn off the panel\r\n");
    esp_lcd_panel_disp_on_off(panel_handle, false);
    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    TEST_ESP_OK(esp_lcd_panel_io_del(io_handle));
    TEST_ESP_OK(spi_bus_free(TEST_SPI_HOST_ID));
    TEST_ESP_OK(gpio_reset_pin(TEST_LCD_BK_LIGHT_GPIO));
    free(img);
}

TEST_CASE("lcd_panel_spi_io_test", "[lcd]")
{
    esp_lcd_panel_io_handle_t io_handle = NULL;
    test_spi_lcd_common_initialize(&io_handle, NULL, NULL, 8, 8, false);
    esp_lcd_panel_io_tx_param(io_handle, 0x1A, NULL, 0);
    esp_lcd_panel_io_tx_param(io_handle, 0x1B, (uint8_t[]) {
        0x11, 0x22, 0x33
    }, 3);
    esp_lcd_panel_io_tx_param(io_handle, 0x1C, NULL, 0);
    TEST_ESP_OK(esp_lcd_panel_io_del(io_handle));
    TEST_ESP_OK(spi_bus_free(TEST_SPI_HOST_ID));

    test_spi_lcd_common_initialize(&io_handle, NULL, NULL, 16, 16, false);
    esp_lcd_panel_io_tx_param(io_handle, 0x1A01, NULL, 0);
    esp_lcd_panel_io_tx_param(io_handle, 0x1B02, (uint16_t[]) {
        0x11, 0x22, 0x33
    }, 6);
    esp_lcd_panel_io_tx_param(io_handle, 0x1C03, NULL, 0);
    TEST_ESP_OK(esp_lcd_panel_io_del(io_handle));
    TEST_ESP_OK(spi_bus_free(TEST_SPI_HOST_ID));

#if SOC_SPI_SUPPORT_OCT
    test_spi_lcd_common_initialize(&io_handle, NULL, NULL, 8, 8, true);
    esp_lcd_panel_io_tx_param(io_handle, 0x1A, NULL, 0);
    esp_lcd_panel_io_tx_param(io_handle, 0x1B, (uint8_t[]) {
        0x11, 0x22, 0x33
    }, 3);
    esp_lcd_panel_io_tx_param(io_handle, 0x1C, NULL, 0);
    TEST_ESP_OK(esp_lcd_panel_io_del(io_handle));
    TEST_ESP_OK(spi_bus_free(TEST_SPI_HOST_ID));

    test_spi_lcd_common_initialize(&io_handle, NULL, NULL, 16, 16, true);
    esp_lcd_panel_io_tx_param(io_handle, 0x1A01, NULL, 0);
    esp_lcd_panel_io_tx_param(io_handle, 0x1B02, (uint16_t[]) {
        0x11, 0x22, 0x33
    }, 6);
    esp_lcd_panel_io_tx_param(io_handle, 0x1C03, NULL, 0);
    TEST_ESP_OK(esp_lcd_panel_io_del(io_handle));
    TEST_ESP_OK(spi_bus_free(TEST_SPI_HOST_ID));
#endif // SOC_SPI_SUPPORT_OCT
}

#if SOC_SPI_SUPPORT_OCT
TEST_CASE("lcd_panel_with_8-line_spi_interface_(st7789)", "[lcd]")
{
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_handle_t panel_handle = NULL;
    test_spi_lcd_common_initialize(&io_handle, NULL, NULL, 8, 8, true);
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = TEST_LCD_RST_GPIO,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };
    TEST_ESP_OK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));
    lcd_panel_test(io_handle, panel_handle);
}

TEST_CASE("lcd_panel_with_8-line_spi_interface_(nt35510)", "[lcd]")
{
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_handle_t panel_handle = NULL;
    test_spi_lcd_common_initialize(&io_handle, NULL, NULL, 16, 16, true);
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = TEST_LCD_RST_GPIO,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };
    TEST_ESP_OK(esp_lcd_new_panel_nt35510(io_handle, &panel_config, &panel_handle));
    lcd_panel_test(io_handle, panel_handle);
}
#endif // SOC_SPI_SUPPORT_OCT

TEST_CASE("lcd_panel_with_1-line_spi_interface_(st7789)", "[lcd]")
{
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_handle_t panel_handle = NULL;
    test_spi_lcd_common_initialize(&io_handle, NULL, NULL, 8, 8, false);
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = TEST_LCD_RST_GPIO,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };
    TEST_ESP_OK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));
    lcd_panel_test(io_handle, panel_handle);
}

TEST_CASE("spi_lcd_send_colors_to_fixed_region", "[lcd]")
{
    int x_start = 100;
    int y_start = 100;
    int x_end = 200;
    int y_end = 200;
    size_t color_size = (x_end - x_start) * (y_end - y_start) * 2;
    void *color_data = malloc(color_size);
    TEST_ASSERT_NOT_NULL(color_data);
    uint8_t color_byte = rand() & 0xFF;
    memset(color_data, color_byte, color_size);

    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_handle_t panel_handle = NULL;
    test_spi_lcd_common_initialize(&io_handle, NULL, NULL, 8, 8, false);

    // we don't use the panel handle in this test, creating the panel just for a quick initialization
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = TEST_LCD_RST_GPIO,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };
    TEST_ESP_OK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));
    esp_lcd_panel_reset(panel_handle);
    esp_lcd_panel_init(panel_handle);
    esp_lcd_panel_invert_color(panel_handle, true);
    // the gap is LCD panel specific, even panels with the same driver IC, can have different gap value
    esp_lcd_panel_set_gap(panel_handle, 0, 20);
    // turn on display
    esp_lcd_panel_disp_on_off(panel_handle, true);
    // turn on backlight
    gpio_set_level(TEST_LCD_BK_LIGHT_GPIO, 1);

    printf("set the flush window for only once\r\n");
    esp_lcd_panel_io_tx_param(io_handle, LCD_CMD_CASET, (uint8_t[]) {
        (x_start >> 8) & 0xFF,
        x_start & 0xFF,
        ((x_end - 1) >> 8) & 0xFF,
        (x_end - 1) & 0xFF,
    }, 4);
    esp_lcd_panel_io_tx_param(io_handle, LCD_CMD_RASET, (uint8_t[]) {
        (y_start >> 8) & 0xFF,
        y_start & 0xFF,
        ((y_end - 1) >> 8) & 0xFF,
        (y_end - 1) & 0xFF,
    }, 4);
    esp_lcd_panel_io_tx_param(io_handle, LCD_CMD_RAMWR, NULL, 0);

    printf("send colors to the fixed region in multiple steps\r\n");
    const int steps = 10;
    int color_size_per_step = color_size / steps;
    for (int i = 0; i < steps; i++) {
        TEST_ESP_OK(esp_lcd_panel_io_tx_color(io_handle, -1, color_data + i * color_size_per_step, color_size_per_step));
    }
    vTaskDelay(pdMS_TO_TICKS(1000));

    printf("change to another color\r\n");
    color_byte = rand() & 0xFF;
    memset(color_data, color_byte, color_size);
    for (int i = 0; i < steps; i++) {
        TEST_ESP_OK(esp_lcd_panel_io_tx_color(io_handle, -1, color_data + i * color_size_per_step, color_size_per_step));
    }
    vTaskDelay(pdMS_TO_TICKS(1000));

    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    TEST_ESP_OK(esp_lcd_panel_io_del(io_handle));
    TEST_ESP_OK(spi_bus_free(TEST_SPI_HOST_ID));
    free(color_data);
}
