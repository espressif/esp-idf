#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include "esp_lcd_io_spi.h"
#include "esp_log.h"
#include "esp_lcd_panel_io.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/spi_master.h"
#include "sdkconfig.h"

static const char *TAG = "waveshare_lcd_demo";

#define LCD_HOST SPI2_HOST
#define LCD_H_RES 320
#define LCD_V_RES 172
#define LCD_PIXEL_CLOCK_HZ (8 * 1000 * 1000)
#define LCD_CMD_BITS 8
#define LCD_PARAM_BITS 8
#define LCD_X_GAP 0
#define LCD_Y_GAP 34
#define DRAW_CHUNK_LINES 40

#define PIN_NUM_MOSI 6
#define PIN_NUM_MISO -1
#define PIN_NUM_SCLK 7
#define PIN_NUM_LCD_CS 14
#define PIN_NUM_LCD_DC 15
#define PIN_NUM_LCD_RST 21
#define PIN_NUM_BK_LIGHT 22
#define LCD_BK_LIGHT_ON_LEVEL 1
#define LCD_BK_LIGHT_OFF_LEVEL 0
#define LCD_BK_LIGHT_LEDC_TIMER LEDC_TIMER_0
#define LCD_BK_LIGHT_LEDC_MODE LEDC_LOW_SPEED_MODE
#define LCD_BK_LIGHT_LEDC_CHANNEL LEDC_CHANNEL_0
#define LCD_BK_LIGHT_LEDC_DUTY_RES LEDC_TIMER_13_BIT
#define LCD_BK_LIGHT_LEDC_MAX_DUTY ((1 << LCD_BK_LIGHT_LEDC_DUTY_RES) - 1)

static void lcd_reset(void)
{
    gpio_config_t rst_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << PIN_NUM_LCD_RST,
    };
    ESP_ERROR_CHECK(gpio_config(&rst_gpio_config));
    ESP_ERROR_CHECK(gpio_set_level(PIN_NUM_LCD_RST, 0));
    vTaskDelay(pdMS_TO_TICKS(10));
    ESP_ERROR_CHECK(gpio_set_level(PIN_NUM_LCD_RST, 1));
    vTaskDelay(pdMS_TO_TICKS(10));
}

static void lcd_backlight_init(void)
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LCD_BK_LIGHT_LEDC_MODE,
        .timer_num = LCD_BK_LIGHT_LEDC_TIMER,
        .duty_resolution = LCD_BK_LIGHT_LEDC_DUTY_RES,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ledc_channel_config_t ledc_channel = {
        .speed_mode = LCD_BK_LIGHT_LEDC_MODE,
        .channel = LCD_BK_LIGHT_LEDC_CHANNEL,
        .timer_sel = LCD_BK_LIGHT_LEDC_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = PIN_NUM_BK_LIGHT,
        .duty = 0,
        .hpoint = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

static void lcd_backlight_set(uint8_t percent)
{
    if (percent > 100) {
        percent = 100;
    }
    uint32_t duty = (LCD_BK_LIGHT_LEDC_MAX_DUTY * percent) / 100;
    ESP_ERROR_CHECK(ledc_set_duty(LCD_BK_LIGHT_LEDC_MODE, LCD_BK_LIGHT_LEDC_CHANNEL, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LCD_BK_LIGHT_LEDC_MODE, LCD_BK_LIGHT_LEDC_CHANNEL));
}

static void waveshare_st7789t_configure(esp_lcd_panel_io_handle_t io_handle)
{
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0x11, NULL, 0));
    vTaskDelay(pdMS_TO_TICKS(100));
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0x36, (uint8_t[]){0xE0}, 1));
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0x3A, (uint8_t[]){0x55}, 1));
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0xB0, (uint8_t[]){0x00, 0xE8}, 2));
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0xB2, (uint8_t[]){0x0C, 0x0C, 0x00, 0x33, 0x33}, 5));
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0xB7, (uint8_t[]){0x75}, 1));
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0xBB, (uint8_t[]){0x1A}, 1));
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0xC0, (uint8_t[]){0x80}, 1));
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0xC2, (uint8_t[]){0x01, 0xFF}, 2));
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0xC3, (uint8_t[]){0x13}, 1));
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0xC4, (uint8_t[]){0x20}, 1));
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0xC6, (uint8_t[]){0x0F}, 1));
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0xD0, (uint8_t[]){0xA4, 0xA1}, 2));
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0xE0, (uint8_t[]){0xD0, 0x0D, 0x14, 0x0D, 0x0D, 0x09, 0x38, 0x44, 0x4E, 0x3A, 0x17, 0x18, 0x2F, 0x30}, 14));
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0xE1, (uint8_t[]){0xD0, 0x09, 0x0F, 0x08, 0x07, 0x14, 0x37, 0x44, 0x4D, 0x38, 0x15, 0x16, 0x2C, 0x2E}, 14));
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0x20, NULL, 0));
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0x29, NULL, 0));
}

static void lcd_draw_bitmap(esp_lcd_panel_io_handle_t io_handle, int x_start, int y_start, int x_end, int y_end, const void *color_data)
{
    x_start += LCD_X_GAP;
    x_end += LCD_X_GAP;
    y_start += LCD_Y_GAP;
    y_end += LCD_Y_GAP;

    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0x2A, (uint8_t[]){
        (x_start >> 8) & 0xFF,
        x_start & 0xFF,
        ((x_end - 1) >> 8) & 0xFF,
        (x_end - 1) & 0xFF,
    }, 4));
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0x2B, (uint8_t[]){
        (y_start >> 8) & 0xFF,
        y_start & 0xFF,
        ((y_end - 1) >> 8) & 0xFF,
        (y_end - 1) & 0xFF,
    }, 4));
    ESP_ERROR_CHECK(esp_lcd_panel_io_tx_color(io_handle, 0x2C, color_data, (x_end - x_start) * (y_end - y_start) * sizeof(uint16_t)));
}

void app_main(void)
{
    if (PIN_NUM_BK_LIGHT >= 0) {
        lcd_backlight_init();
        lcd_backlight_set(0);
    }

    ESP_LOGI(TAG, "Initializing SPI bus");
    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = LCD_H_RES * 40 * sizeof(uint16_t),
    };
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

    ESP_LOGI(TAG, "Installing LCD IO");
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = PIN_NUM_LCD_DC,
        .cs_gpio_num = PIN_NUM_LCD_CS,
        .pclk_hz = LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = LCD_CMD_BITS,
        .lcd_param_bits = LCD_PARAM_BITS,
        .spi_mode = 0,
        .trans_queue_depth = 1,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &io_handle));

    ESP_LOGI(TAG, "Configuring ST7789T panel");
    lcd_reset();
    waveshare_st7789t_configure(io_handle);

    if (PIN_NUM_BK_LIGHT >= 0) {
        lcd_backlight_set(75);
    }

    ESP_LOGI(TAG, "Drawing test pattern");
    uint16_t *frame_buffer = heap_caps_malloc(LCD_H_RES * LCD_V_RES * sizeof(uint16_t), MALLOC_CAP_DMA);
    if (frame_buffer) {
        for (int y = 0; y < LCD_V_RES; y++) {
            for (int x = 0; x < LCD_H_RES; x++) {
                uint16_t color;
                static const uint16_t colors[] = {
                    0xF800, 0x001F, 0x07E0, 0xFFFF,
                    0x0000, 0x07FF, 0xF81F, 0xFFE0,
                };
                int col = (x * 4) / LCD_H_RES;
                int row = (y * 2) / LCD_V_RES;
                color = colors[row * 4 + col];
                frame_buffer[y * LCD_H_RES + x] = color;
            }
        }
        for (int y_start = 0; y_start < LCD_V_RES; y_start += DRAW_CHUNK_LINES) {
            int y_end = y_start + DRAW_CHUNK_LINES;
            if (y_end > LCD_V_RES) {
                y_end = LCD_V_RES;
            }
            lcd_draw_bitmap(io_handle, 0, y_start, LCD_H_RES, y_end, &frame_buffer[y_start * LCD_H_RES]);
        }
        ESP_ERROR_CHECK(esp_lcd_panel_io_tx_param(io_handle, 0x00, NULL, 0));
    } else {
        ESP_LOGE(TAG, "Failed to allocate frame buffer");
    }

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI(TAG, "Display test pattern active");
    }
}
