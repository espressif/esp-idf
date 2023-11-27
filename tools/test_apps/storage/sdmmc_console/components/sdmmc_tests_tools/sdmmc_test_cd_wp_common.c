/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <unistd.h>
#include "unity.h"
#include "esp_rom_gpio.h"
#include "hal/gpio_hal.h"
#include "sdmmc_cmd.h"
#include "sdmmc_test_cd_wp_common.h"


void sdmmc_test_cd_input(int gpio_cd_num, const sdmmc_host_t* config)
{
    sdmmc_card_t* card = malloc(sizeof(sdmmc_card_t));
    TEST_ASSERT_NOT_NULL(card);

    // SDMMC host should have configured CD as input.
    // Enable output as well (not using the driver, to avoid touching input
    // enable bits).
    gpio_hal_context_t gpio_hal = {
        .dev = GPIO_HAL_GET_HW(GPIO_PORT_0)
    };
    esp_rom_gpio_connect_out_signal(gpio_cd_num, SIG_GPIO_OUT_IDX, false, false);
    gpio_hal_output_enable(&gpio_hal, gpio_cd_num);

    // Check that card initialization fails if CD is high
    gpio_hal_set_level(&gpio_hal, gpio_cd_num, 1);
    usleep(1000);
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, sdmmc_card_init(config, card));

    // Check that card initialization succeeds if CD is low
    gpio_hal_set_level(&gpio_hal, gpio_cd_num, 0);
    usleep(1000);
    TEST_ESP_OK(sdmmc_card_init(config, card));

    free(card);
}

void sdmmc_test_wp_input(int gpio_wp_num, const sdmmc_host_t* config)
{
    sdmmc_card_t* card = malloc(sizeof(sdmmc_card_t));
    TEST_ASSERT_NOT_NULL(card);

    // SDMMC host should have configured WP as input.
    // Enable output as well (not using the driver, to avoid touching input
    // enable bits).
    gpio_hal_context_t gpio_hal = {
        .dev = GPIO_HAL_GET_HW(GPIO_PORT_0)
    };
    esp_rom_gpio_connect_out_signal(gpio_wp_num, SIG_GPIO_OUT_IDX, false, false);
    gpio_hal_output_enable(&gpio_hal, gpio_wp_num);

    // Check that the card can be initialized with WP low
    gpio_hal_set_level(&gpio_hal, gpio_wp_num, 0);
    TEST_ESP_OK(sdmmc_card_init(config, card));

    uint32_t* data = heap_caps_calloc(1, 512, MALLOC_CAP_DMA);

    // Check that card write succeeds if WP is high
    gpio_hal_set_level(&gpio_hal, gpio_wp_num, 1);
    usleep(1000);
    TEST_ESP_OK(sdmmc_write_sectors(card, &data, 0, 1));

    // Check that write fails if WP is low
    gpio_hal_set_level(&gpio_hal, gpio_wp_num, 0);
    usleep(1000);
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, sdmmc_write_sectors(card, &data, 0, 1));
    // ...but reads still work
    TEST_ESP_OK(sdmmc_read_sectors(card, &data, 0, 1));

    free(data);
    free(card);
}
