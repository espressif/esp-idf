/* LCD tjpgd example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "pretty_effect.h"

#define LCD_HOST    SPI2_HOST   /*!< spi peripheral for LCD */

/**
 * If not using the default settings, the SPI peripheral on LCD and the GPIO numbers can be
 * changed below.
*/
#define PIN_NUM_MOSI 23     /*!< gpio number for LCD MOSI, also the gpio number for LCD DATA0 at 8-line mode */
#define PIN_NUM_CLK  19     /*!< gpio number for LCD clock */
#define PIN_NUM_CS   22     /*!< gpio number for LCD CS */
#define PIN_NUM_DC   21     /*!< gpio number for LCD DC */
#define PIN_NUM_RST  18     /*!< gpio number for LCD RST */
#define PIN_NUM_BCKL 5      /*!< gpio number for LCD Back Light */
#ifdef CONFIG_LCD_SPI_8_LINE_MODE  // If using 8-line LCD
#define PIN_NUM_DATA1 6     /*!< gpio number for LCD DATA1 */
#define PIN_NUM_DATA2 7     /*!< gpio number for LCD DATA2 */
#define PIN_NUM_DATA3 8     /*!< gpio number for LCD DATA3 */
#define PIN_NUM_DATA4 9     /*!< gpio number for LCD DATA4 */
#define PIN_NUM_DATA5 10    /*!< gpio number for LCD DATA5 */
#define PIN_NUM_DATA6 11    /*!< gpio number for LCD DATA6 */
#define PIN_NUM_DATA7 12    /*!< gpio number for LCD DATA7 */
#endif

// The pixel number in horizontal and vertical
#define EXAMPLE_LCD_H_RES (320)
#define EXAMPLE_LCD_V_RES (240)

// The SPI mode supported by the LCD
#define LCD_SPI_MODE 0

// To speed up transfers, every SPI transfer sends a bunch of lines. This define specifies how many.
// More means more memory use, but less overhead for setting up / finishing transfers. Make sure 240
// is dividable by this.
#define PARALLEL_LINES 16

// The number of frames to show before rotate the graph
#define ROTATE_FRAME 30


// Simple routine to generate some patterns and send them to the LCD. Because the
// SPI driver handles transactions in the background, we can calculate the next line
// while the previous one is being sent.
static uint16_t *s_lines[2];
static void display_pretty_colors(esp_lcd_panel_handle_t panel_handle)
{
    int frame = 0;
    // Indexes of the line currently being sent to the LCD and the line we're calculating
    int sending_line = 0;
    int calc_line = 0;

    // After ROTATE_FRAME frames, the image will be rotated
    while (frame <= ROTATE_FRAME) {
        frame++;
        for (int y = 0; y < EXAMPLE_LCD_V_RES; y += PARALLEL_LINES) {
            // Calculate a line
            pretty_effect_calc_lines(s_lines[calc_line], y, frame, PARALLEL_LINES);
            sending_line = calc_line;
            calc_line = !calc_line;
            // Send the calculated data
            esp_lcd_panel_draw_bitmap(panel_handle, 0, y, 0 + EXAMPLE_LCD_H_RES, y + PARALLEL_LINES, s_lines[sending_line]);
        }
    }
}

void app_main(void)
{
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << PIN_NUM_BCKL
    };
    // Initialize the GPIO of backlight
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));

    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
#ifdef CONFIG_LCD_SPI_8_LINE_MODE
        .data1_io_num = PIN_NUM_DATA1,
        .data2_io_num = PIN_NUM_DATA2,
        .data3_io_num = PIN_NUM_DATA3,
        .data4_io_num = PIN_NUM_DATA4,
        .data5_io_num = PIN_NUM_DATA5,
        .data6_io_num = PIN_NUM_DATA6,
        .data7_io_num = PIN_NUM_DATA7,
        .flags = SPICOMMON_BUSFLAG_OCTAL,
#else
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
#endif
        .max_transfer_sz = PARALLEL_LINES * EXAMPLE_LCD_H_RES * 2 + 8
    };
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = PIN_NUM_DC,
        .cs_gpio_num = PIN_NUM_CS,
#ifdef CONFIG_LCD_OVERCLOCK
        .pclk_hz = 26 * 1000 * 1000,     // Clock out at 26 MHz
#else
        .pclk_hz = 10 * 1000 * 1000,     // Clock out at 10 MHz
#endif
        .spi_mode = LCD_SPI_MODE,
        .trans_queue_depth = 7,
#ifdef CONFIG_LCD_SPI_8_LINE_MODE
        .flags.octal_mode = 1,
#endif
    };
    // Initialize the SPI bus
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));
    // Attach the LCD to the SPI bus
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &io_handle));


    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_NUM_RST,
        .color_space = ESP_LCD_COLOR_SPACE_BGR,
        .bits_per_pixel = 16,
    };
    // Initialize the LCD configuration
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

    // Turn off backlight to avoid unpredictable display on the LCD screen while initializing
    // the LCD panel driver. (Different LCD screens may need different levels)
    ESP_ERROR_CHECK(gpio_set_level(PIN_NUM_BCKL, 1));

    // Reset the display
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));

    // Initialize LCD panel
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

    // Swap x and y axis (Different LCD screens may need different options)
    ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(panel_handle, true));

    // Turn on backlight (Different LCD screens may need different levels)
    ESP_ERROR_CHECK(gpio_set_level(PIN_NUM_BCKL, 0));

    // Initialize the effect displayed
    ESP_ERROR_CHECK(pretty_effect_init());

    // "Rotate or not" flag
    bool is_rotated = false;

    // Allocate memory for the pixel buffers
    for (int i = 0; i < 2; i++) {
        s_lines[i] = heap_caps_malloc(EXAMPLE_LCD_H_RES * PARALLEL_LINES * sizeof(uint16_t), MALLOC_CAP_DMA);
        assert(s_lines[i] != NULL);
    }

    // Start and rotate
    while (1) {
        // Set driver configuration to rotate 180 degrees each time
        ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, is_rotated, is_rotated));
        // Display
        display_pretty_colors(panel_handle);
        is_rotated = !is_rotated;
    }

}
