/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* Includes */
#include "led.h"
#include "common.h"

/* Private variables */
static uint8_t led_state;

#ifdef CONFIG_BLINK_LED_STRIP
static led_strip_handle_t led_strip;
#endif

/* Public functions */
uint8_t get_led_state(void) { return led_state; }

#ifdef CONFIG_BLINK_LED_STRIP

void led_on(void) {
    /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
    led_strip_set_pixel(led_strip, 0, 16, 16, 16);

    /* Refresh the strip to send data */
    led_strip_refresh(led_strip);

    /* Update LED state */
    led_state = true;
}

void led_off(void) {
    /* Set all LED off to clear all pixels */
    led_strip_clear(led_strip);

    /* Update LED state */
    led_state = false;
}

void led_init(void) {
    ESP_LOGI(TAG, "example configured to blink addressable led!");
    /* LED strip initialization with the GPIO and pixels number*/
    led_strip_config_t strip_config = {
        .strip_gpio_num = CONFIG_BLINK_GPIO,
        .max_leds = 1, // at least one LED on board
    };
#if CONFIG_BLINK_LED_STRIP_BACKEND_RMT
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .flags.with_dma = false,
    };
    ESP_ERROR_CHECK(
        led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
#elif CONFIG_BLINK_LED_STRIP_BACKEND_SPI
    led_strip_spi_config_t spi_config = {
        .spi_bus = SPI2_HOST,
        .flags.with_dma = true,
    };
    ESP_ERROR_CHECK(
        led_strip_new_spi_device(&strip_config, &spi_config, &led_strip));
#else
#error "unsupported LED strip backend"
#endif
    /* Set all LED off to clear all pixels */
    led_off();
}

#elif CONFIG_BLINK_LED_GPIO

void led_on(void) { gpio_set_level(CONFIG_BLINK_GPIO, true); }

void led_off(void) { gpio_set_level(CONFIG_BLINK_GPIO, false); }

void led_init(void) {
    ESP_LOGI(TAG, "example configured to blink gpio led!");
    gpio_reset_pin(CONFIG_BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(CONFIG_BLINK_GPIO, GPIO_MODE_OUTPUT);
}

#else
#error "unsupported LED type"
#endif
