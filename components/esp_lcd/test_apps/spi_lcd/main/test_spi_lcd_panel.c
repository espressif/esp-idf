/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "unity.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_random.h"
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
        .max_transfer_sz = TEST_LCD_H_RES * TEST_LCD_V_RES * sizeof(uint16_t)
    };
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
    if (oct_mode) {
        io_config.flags.octal_mode = 1;
        io_config.spi_mode = 3;
    }
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

    for (int i = 0; i < 200; i++) {
        uint8_t color_byte = esp_random() & 0xFF;
        int x_start = esp_random() % (TEST_LCD_H_RES - 200);
        int y_start = esp_random() % (TEST_LCD_V_RES - 200);
        memset(img, color_byte, TEST_IMG_SIZE);
        esp_lcd_panel_draw_bitmap(panel_handle, x_start, y_start, x_start + 200, y_start + 200, img);
    }
    // turn off screen
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
        .rgb_endian = LCD_RGB_ENDIAN_RGB,
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
        .rgb_endian = LCD_RGB_ENDIAN_RGB,
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
        .rgb_endian = LCD_RGB_ENDIAN_RGB,
        .bits_per_pixel = 16,
    };
    TEST_ESP_OK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));
    lcd_panel_test(io_handle, panel_handle);
}
