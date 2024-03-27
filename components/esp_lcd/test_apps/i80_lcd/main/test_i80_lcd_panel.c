/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "esp_random.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_commands.h"
#include "soc/soc_caps.h"
#include "driver/gpio.h"
#include "test_i80_board.h"

#if SOC_I2S_LCD_I80_VARIANT
#include "driver/i2s_std.h"

TEST_CASE("i80_and_i2s_driver_co-existence", "[lcd][i2s]")
{
    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = TEST_LCD_DC_GPIO,
        .wr_gpio_num = TEST_LCD_PCLK_GPIO,
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .data_gpio_nums = {
            TEST_LCD_DATA0_GPIO,
            TEST_LCD_DATA1_GPIO,
            TEST_LCD_DATA2_GPIO,
            TEST_LCD_DATA3_GPIO,
            TEST_LCD_DATA4_GPIO,
            TEST_LCD_DATA5_GPIO,
            TEST_LCD_DATA6_GPIO,
            TEST_LCD_DATA7_GPIO,
        },
        .bus_width = 8,
        .max_transfer_bytes = 20,
    };
    TEST_ESP_OK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));

    i2s_chan_handle_t tx_handle = NULL;
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    // I2S driver won't be installed as the same I2S port has been used by LCD
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, i2s_new_channel(&chan_cfg, &tx_handle, NULL));
    TEST_ESP_OK(esp_lcd_del_i80_bus(i80_bus));
}
#endif // SOC_I2S_LCD_I80_VARIANT

#if SOC_LCDCAM_SUPPORTED
TEST_CASE("lcd_i80_device_swap_color_bytes", "[lcd]")
{
    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = TEST_LCD_DC_GPIO,
        .wr_gpio_num = TEST_LCD_PCLK_GPIO,
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .data_gpio_nums = {
            TEST_LCD_DATA0_GPIO,
            TEST_LCD_DATA1_GPIO,
            TEST_LCD_DATA2_GPIO,
            TEST_LCD_DATA3_GPIO,
            TEST_LCD_DATA4_GPIO,
            TEST_LCD_DATA5_GPIO,
            TEST_LCD_DATA6_GPIO,
            TEST_LCD_DATA7_GPIO,
        },
        .bus_width = 8,
        .max_transfer_bytes = 20,
    };
    TEST_ESP_OK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));

    esp_lcd_panel_io_handle_t io_handles[4] = {};
    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = TEST_LCD_CS_GPIO,
        .pclk_hz = 5000000,
        .trans_queue_depth = 10,
        .dc_levels = {
            .dc_idle_level = 0,
            .dc_cmd_level = 0,
            .dc_dummy_level = 0,
            .dc_data_level = 1,
        },
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };

    io_config.flags.reverse_color_bits = 0;
    io_config.flags.swap_color_bytes = 0;
    TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handles[0]));
    io_config.flags.reverse_color_bits = 0;
    io_config.flags.swap_color_bytes = 1;
    TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handles[1]));
    io_config.flags.reverse_color_bits = 1;
    io_config.flags.swap_color_bytes = 0;
    TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handles[2]));
    io_config.flags.reverse_color_bits = 1;
    io_config.flags.swap_color_bytes = 1;
    TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handles[3]));

    for (int i = 0; i < 4; i++) {
        esp_lcd_panel_io_tx_param(io_handles[i], 0xA5, (uint8_t[]) {
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06
        }, 6);
        esp_lcd_panel_io_tx_color(io_handles[i], 0x5A, (uint8_t[]) {
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06
        }, 6);
        TEST_ESP_OK(esp_lcd_panel_io_del(io_handles[i]));
    }

    TEST_ESP_OK(esp_lcd_del_i80_bus(i80_bus));
}

TEST_CASE("lcd_i80_device_clock_mode", "[lcd]")
{
    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = TEST_LCD_DC_GPIO,
        .wr_gpio_num = TEST_LCD_PCLK_GPIO,
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .data_gpio_nums = {
            TEST_LCD_DATA0_GPIO,
            TEST_LCD_DATA1_GPIO,
            TEST_LCD_DATA2_GPIO,
            TEST_LCD_DATA3_GPIO,
            TEST_LCD_DATA4_GPIO,
            TEST_LCD_DATA5_GPIO,
            TEST_LCD_DATA6_GPIO,
            TEST_LCD_DATA7_GPIO,
        },
        .bus_width = 8,
        .max_transfer_bytes = 20,
    };
    TEST_ESP_OK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));

    esp_lcd_panel_io_handle_t io_handles[4] = {};
    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = TEST_LCD_CS_GPIO,
        .pclk_hz = 5000000,
        .trans_queue_depth = 10,
        .dc_levels = {
            .dc_idle_level = 0,
            .dc_cmd_level = 0,
            .dc_dummy_level = 0,
            .dc_data_level = 1,
        },
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };

    io_config.flags.pclk_idle_low = 0;
    io_config.flags.pclk_active_neg = 0;
    TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handles[0]));
    io_config.flags.pclk_idle_low = 0;
    io_config.flags.pclk_active_neg = 1;
    TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handles[1]));
    io_config.flags.pclk_idle_low = 1;
    io_config.flags.pclk_active_neg = 0;
    TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handles[2]));
    io_config.flags.pclk_idle_low = 1;
    io_config.flags.pclk_active_neg = 1;
    TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handles[3]));

    for (int i = 0; i < 4; i++) {
        esp_lcd_panel_io_tx_param(io_handles[i], 0xA5, (uint8_t[]) {
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06
        }, 6);
        TEST_ESP_OK(esp_lcd_panel_io_del(io_handles[i]));
    }
    TEST_ESP_OK(esp_lcd_del_i80_bus(i80_bus));
}
#endif // SOC_LCDCAM_SUPPORTED

TEST_CASE("lcd_i80_bus_and_device_allocation", "[lcd]")
{
    esp_lcd_i80_bus_handle_t i80_buses[SOC_LCD_I80_BUSES] = {};
    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = TEST_LCD_DC_GPIO,
        .wr_gpio_num = TEST_LCD_PCLK_GPIO,
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .data_gpio_nums = {
            TEST_LCD_DATA0_GPIO,
            TEST_LCD_DATA1_GPIO,
            TEST_LCD_DATA2_GPIO,
            TEST_LCD_DATA3_GPIO,
            TEST_LCD_DATA4_GPIO,
            TEST_LCD_DATA5_GPIO,
            TEST_LCD_DATA6_GPIO,
            TEST_LCD_DATA7_GPIO,
        },
        .bus_width = 8,
        .max_transfer_bytes = TEST_LCD_H_RES * 40 * sizeof(uint16_t)
    };
    for (int i = 0; i < SOC_LCD_I80_BUSES; i++) {
        TEST_ESP_OK(esp_lcd_new_i80_bus(&bus_config, &i80_buses[i]));
    }
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, esp_lcd_new_i80_bus(&bus_config, &i80_buses[0]));
    esp_lcd_panel_io_handle_t io_handles[10] = {};
    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = TEST_LCD_CS_GPIO,
        .pclk_hz = 5000000,
        .trans_queue_depth = 4,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    for (int i = 0; i < 10; i++) {
        TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_buses[0], &io_config, &io_handles[i]));
    }
    // can't delete bus handle before we delete all devices
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_lcd_del_i80_bus(i80_buses[0]));
    for (int i = 0; i < 10; i++) {
        TEST_ESP_OK(esp_lcd_panel_io_del(io_handles[i]));
    }
    for (int i = 0; i < SOC_LCD_I80_BUSES; i++) {
        TEST_ESP_OK(esp_lcd_del_i80_bus(i80_buses[i]));
    }
}

TEST_CASE("lcd_i80_bus_exclusively_owned_by_one_device", "[lcd]")
{
    esp_lcd_i80_bus_handle_t i80_bus_handle = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = TEST_LCD_DC_GPIO,
        .wr_gpio_num = TEST_LCD_PCLK_GPIO,
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .data_gpio_nums = {
            TEST_LCD_DATA0_GPIO,
            TEST_LCD_DATA1_GPIO,
            TEST_LCD_DATA2_GPIO,
            TEST_LCD_DATA3_GPIO,
            TEST_LCD_DATA4_GPIO,
            TEST_LCD_DATA5_GPIO,
            TEST_LCD_DATA6_GPIO,
            TEST_LCD_DATA7_GPIO,
        },
        .bus_width = 8,
        .max_transfer_bytes = TEST_LCD_H_RES * 40 * sizeof(uint16_t)
    };
    TEST_ESP_OK(esp_lcd_new_i80_bus(&bus_config, &i80_bus_handle));
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = -1, // own the bus exclusively
        .pclk_hz = 5000000,
        .trans_queue_depth = 4,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_bus_handle, &io_config, &io_handle));
    io_config.cs_gpio_num = 0;
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, esp_lcd_new_panel_io_i80(i80_bus_handle, &io_config, &io_handle));
    TEST_ESP_OK(esp_lcd_panel_io_del(io_handle));
    TEST_ESP_OK(esp_lcd_del_i80_bus(i80_bus_handle));
}

TEST_CASE("lcd_panel_i80_io_test", "[lcd]")
{
    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = TEST_LCD_DC_GPIO,
        .wr_gpio_num = TEST_LCD_PCLK_GPIO,
        .clk_src = LCD_CLK_SRC_DEFAULT,
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
        .bus_width = 16,
        .max_transfer_bytes = 100,
    };
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = TEST_LCD_CS_GPIO,
        .pclk_hz = 8000000, // 8MHz
        .trans_queue_depth = 10,
        .dc_levels = {
            .dc_idle_level = 0,
            .dc_cmd_level = 0,
            .dc_dummy_level = 0,
            .dc_data_level = 1,
        },
    };
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = TEST_LCD_RST_GPIO,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };

    printf("testing bus-width=16bit, cmd/param bit-width=8bit\r\n");
    bus_config.bus_width = 16;
    TEST_ESP_OK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));
    io_config.lcd_cmd_bits = 8;
    io_config.lcd_param_bits = 8;
    TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));
    TEST_ESP_OK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

    esp_lcd_panel_io_tx_param(io_handle, 0x1A, NULL, 0);
    esp_lcd_panel_io_tx_param(io_handle, 0x1B, (uint8_t[]) {
        0x11, 0x22, 0x33
    }, 3);
    esp_lcd_panel_io_tx_param(io_handle, 0x1C, NULL, 0);
    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    TEST_ESP_OK(esp_lcd_panel_io_del(io_handle));
    TEST_ESP_OK(esp_lcd_del_i80_bus(i80_bus));

    printf("testing bus-width=16bit, cmd/param bit-width=16bit\r\n");
    bus_config.bus_width = 16;
    TEST_ESP_OK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));
    io_config.lcd_cmd_bits = 16;
    io_config.lcd_param_bits = 16;
    TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));
    TEST_ESP_OK(esp_lcd_new_panel_nt35510(io_handle, &panel_config, &panel_handle));
    esp_lcd_panel_io_tx_param(io_handle, 0x1A01, NULL, 0);
    esp_lcd_panel_io_tx_param(io_handle, 0x1B02, (uint16_t[]) {
        0x11, 0x22, 0x33
    }, 6);
    esp_lcd_panel_io_tx_param(io_handle, 0x1C03, NULL, 0);
    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    TEST_ESP_OK(esp_lcd_panel_io_del(io_handle));
    TEST_ESP_OK(esp_lcd_del_i80_bus(i80_bus));

    printf("testing bus-width=8bit, cmd/param bit-width=8bit\r\n");
    bus_config.bus_width = 8;
    TEST_ESP_OK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));
    io_config.lcd_cmd_bits = 8;
    io_config.lcd_param_bits = 8;
    TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));
    TEST_ESP_OK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));
    esp_lcd_panel_io_tx_param(io_handle, 0x1A, NULL, 0);
    esp_lcd_panel_io_tx_param(io_handle, 0x1B, (uint8_t[]) {
        0x11, 0x22, 0x33
    }, 3);
    esp_lcd_panel_io_tx_param(io_handle, 0x1C, NULL, 0);
    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    TEST_ESP_OK(esp_lcd_panel_io_del(io_handle));
    TEST_ESP_OK(esp_lcd_del_i80_bus(i80_bus));

    printf("testing bus-width=8bit, cmd/param bit-width=16bit\r\n");
    bus_config.bus_width = 8;
    TEST_ESP_OK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));
    io_config.lcd_cmd_bits = 16;
    io_config.lcd_param_bits = 16;
    TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));
    TEST_ESP_OK(esp_lcd_new_panel_nt35510(io_handle, &panel_config, &panel_handle));
    esp_lcd_panel_io_tx_param(io_handle, 0x1A01, NULL, 0);
    esp_lcd_panel_io_tx_param(io_handle, 0x1B02, (uint16_t[]) {
        0x11, 0x22, 0x33
    }, 6);
    esp_lcd_panel_io_tx_param(io_handle, 0x1C03, NULL, 0);
    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    TEST_ESP_OK(esp_lcd_panel_io_del(io_handle));
    TEST_ESP_OK(esp_lcd_del_i80_bus(i80_bus));
}

TEST_CASE("lcd_panel_with_i80_interface_(st7789, 8bits)", "[lcd]")
{
#define TEST_IMG_SIZE (100 * 100 * sizeof(uint16_t))
    uint8_t *img = heap_caps_malloc(TEST_IMG_SIZE, MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL(img);

    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << TEST_LCD_BK_LIGHT_GPIO
    };
    TEST_ESP_OK(gpio_config(&bk_gpio_config));

    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = TEST_LCD_DC_GPIO,
        .wr_gpio_num = TEST_LCD_PCLK_GPIO,
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .data_gpio_nums = {
            TEST_LCD_DATA0_GPIO,
            TEST_LCD_DATA1_GPIO,
            TEST_LCD_DATA2_GPIO,
            TEST_LCD_DATA3_GPIO,
            TEST_LCD_DATA4_GPIO,
            TEST_LCD_DATA5_GPIO,
            TEST_LCD_DATA6_GPIO,
            TEST_LCD_DATA7_GPIO,
        },
        .bus_width = 8,
        .max_transfer_bytes = TEST_IMG_SIZE + 10,
    };
    TEST_ESP_OK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = TEST_LCD_CS_GPIO,
        .pclk_hz = 8000000, // 8MHz
        .trans_queue_depth = 10,
        .dc_levels = {
            .dc_idle_level = 0,
            .dc_cmd_level = 0,
            .dc_dummy_level = 0,
            .dc_data_level = 1,
        },
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));

    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = TEST_LCD_RST_GPIO,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };
    TEST_ESP_OK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

    // turn off backlight
    gpio_set_level(TEST_LCD_BK_LIGHT_GPIO, 0);
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
        int x_start = esp_random() % (TEST_LCD_H_RES - 100);
        int y_start = esp_random() % (TEST_LCD_V_RES - 100);
        memset(img, color_byte, TEST_IMG_SIZE);
        esp_lcd_panel_draw_bitmap(panel_handle, x_start, y_start, x_start + 100, y_start + 100, img);
    }
    // turn off screen
    esp_lcd_panel_disp_on_off(panel_handle, false);

    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    TEST_ESP_OK(esp_lcd_panel_io_del(io_handle));
    TEST_ESP_OK(esp_lcd_del_i80_bus(i80_bus));
    TEST_ESP_OK(gpio_reset_pin(TEST_LCD_BK_LIGHT_GPIO));
    free(img);
#undef TEST_IMG_SIZE
}

// TODO: support the test on I2S LCD (IDF-7202)
#if !SOC_I2S_LCD_I80_VARIANT
TEST_CASE("i80_lcd_send_colors_to_fixed_region", "[lcd]")
{
    int x_start = 100;
    int y_start = 100;
    int x_end = 200;
    int y_end = 200;
    size_t color_size = (x_end - x_start) * (y_end - y_start) * 2;
    void *color_data = malloc(color_size);
    TEST_ASSERT_NOT_NULL(color_data);
    uint8_t color_byte = esp_random() & 0xFF;
    memset(color_data, color_byte, color_size);

    printf("creating i80 bus and IO driver\r\n");
    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = TEST_LCD_DC_GPIO,
        .wr_gpio_num = TEST_LCD_PCLK_GPIO,
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .data_gpio_nums = {
            TEST_LCD_DATA0_GPIO,
            TEST_LCD_DATA1_GPIO,
            TEST_LCD_DATA2_GPIO,
            TEST_LCD_DATA3_GPIO,
            TEST_LCD_DATA4_GPIO,
            TEST_LCD_DATA5_GPIO,
            TEST_LCD_DATA6_GPIO,
            TEST_LCD_DATA7_GPIO,
        },
        .bus_width = 8,
        .max_transfer_bytes = color_size * 2,
    };
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = TEST_LCD_CS_GPIO,
        .pclk_hz = 5000000, // 5MHz
        .trans_queue_depth = 10,
        .dc_levels = {
            .dc_idle_level = 0,
            .dc_cmd_level = 0,
            .dc_dummy_level = 0,
            .dc_data_level = 1,
        },
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    TEST_ESP_OK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));
    TEST_ESP_OK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));

    printf("creating LCD panel\r\n");
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = TEST_LCD_RST_GPIO,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };
    TEST_ESP_OK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

    // we don't use the panel handle in this test, creating the panel just for a quick initialization
    printf("initialize LCD panel\r\n");
    // turn off backlight
    gpio_set_level(TEST_LCD_BK_LIGHT_GPIO, 0);
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
    // change to another color
    color_byte = esp_random() & 0xFF;
    memset(color_data, color_byte, color_size);
    for (int i = 0; i < steps; i++) {
        TEST_ESP_OK(esp_lcd_panel_io_tx_color(io_handle, -1, color_data + i * color_size_per_step, color_size_per_step));
    }

    TEST_ESP_OK(esp_lcd_panel_del(panel_handle));
    TEST_ESP_OK(esp_lcd_panel_io_del(io_handle));
    TEST_ESP_OK(esp_lcd_del_i80_bus(i80_bus));
    free(color_data);
}
#endif
