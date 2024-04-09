/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "perf_benchmark_example_sd_utils.h"

#include "sd_protocol_types.h"
#include "soc/soc_caps.h"
#include "driver/sdspi_host.h"
#include "driver/sdmmc_defs.h"
#include "sdmmc_cmd.h"
#if SOC_SDMMC_HOST_SUPPORTED
#include "driver/sdmmc_host.h"
#endif
#include "esp_log.h"

#ifdef CONFIG_EXAMPLE_TEST_SD_CARD

static const char *TAG = "sd_utils";

sdmmc_host_t host_g;
#ifdef CONFIG_EXAMPLE_USE_SDMMC
sdmmc_slot_config_t slot_config_g;
#else // CONFIG_EXAMPLE_USE_SDMMC
sdspi_device_config_t slot_config_g;
#endif // CONFIG_EXAMPLE_USE_SDSPI

#ifdef CONFIG_EXAMPLE_USE_SDMMC
void init_sd_config(sdmmc_host_t *out_host, sdmmc_slot_config_t *out_slot_config, int freq_khz) {
#else // CONFIG_EXAMPLE_USE_SDMMC
void init_sd_config(sdmmc_host_t *out_host, sdspi_device_config_t *out_slot_config, int freq_khz) {
#endif // CONFIG_EXAMPLE_USE_SDSPI

    // By default, SD card frequency is initialized to SDMMC_FREQ_DEFAULT (20MHz)
    // For setting a specific frequency, use host.max_freq_khz
    // (range 400kHz - 40MHz for SDMMC, 400kHz - 20MHz for SDSPI)
    // Example: for fixed frequency of 10MHz, use host.max_freq_khz = 10000;
#ifdef CONFIG_EXAMPLE_USE_SDMMC
    ESP_LOGI(TAG, "Using SDMMC peripheral");
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = freq_khz;

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    // Set bus width to use:
    #ifdef CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
        slot_config.width = 4;
    #else
        slot_config.width = 1;
    #endif

    // On chips where the GPIOs used for SD card can be configured, set them in
    // the slot_config structure:
    #ifdef CONFIG_SOC_SDMMC_USE_GPIO_MATRIX
        slot_config.clk = CONFIG_EXAMPLE_PIN_CLK;
        slot_config.cmd = CONFIG_EXAMPLE_PIN_CMD;
        slot_config.d0 = CONFIG_EXAMPLE_PIN_D0;
        #ifdef CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
            slot_config.d1 = CONFIG_EXAMPLE_PIN_D1;
            slot_config.d2 = CONFIG_EXAMPLE_PIN_D2;
            slot_config.d3 = CONFIG_EXAMPLE_PIN_D3;
        #endif  // CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
    #endif  // CONFIG_SOC_SDMMC_USE_GPIO_MATRIX

    // Enable internal pullups on enabled pins. The internal pullups
    // are insufficient however, please make sure 10k external pullups are
    // connected on the bus. This is for debug / example purpose only.
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;
#else // CONFIG_EXAMPLE_USE_SDMMC
    ESP_LOGI(TAG, "Using SPI peripheral");
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.max_freq_khz = freq_khz;

    spi_bus_config_t bus_cfg = {
        .mosi_io_num = CONFIG_EXAMPLE_PIN_MOSI,
        .miso_io_num = CONFIG_EXAMPLE_PIN_MISO,
        .sclk_io_num = CONFIG_EXAMPLE_PIN_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    esp_err_t ret = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    ESP_ERROR_CHECK(ret);

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = CONFIG_EXAMPLE_PIN_CS;
    slot_config.host_id = host.slot;
#endif // CONFIG_EXAMPLE_USE_SDSPI
    *out_host = host;
    *out_slot_config = slot_config;
}

esp_err_t init_sd_card(sdmmc_card_t **out_card) {
    esp_err_t ret = ESP_OK;
    sdmmc_card_t* card = (sdmmc_card_t *)malloc(sizeof(sdmmc_card_t));
    if (card == NULL) {
        ESP_LOGE(TAG, "Failed to allocate sdmmc_card_t structure");
        ESP_ERROR_CHECK(ESP_ERR_NO_MEM);
    }

// Initialize the interface
#ifdef CONFIG_EXAMPLE_USE_SDMMC
    ret = sdmmc_host_init();
    ESP_ERROR_CHECK(ret);
    ret = sdmmc_host_init_slot(SDMMC_HOST_SLOT_1, &slot_config_g);
    ESP_ERROR_CHECK(ret);
#elif CONFIG_EXAMPLE_USE_SDSPI // CONFIG_EXAMPLE_USE_SDMMC
    int card_handle = -1;
    ret = sdspi_host_init_device((const sdspi_device_config_t *)&slot_config_g, &card_handle);
    ESP_ERROR_CHECK(ret);
#endif // CONFIG_EXAMPLE_USE_SDSPI

    ret = sdmmc_card_init(&host_g, card);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SD card (%s)", esp_err_to_name(ret));
        ESP_LOGE(TAG, "If you were using SDMMC and switched to SPI reinsert the SD card or power cycle the board");
        free(card);
        ESP_ERROR_CHECK(ret);
    }
    ESP_LOGI(TAG, "SD card mounted - raw access");
    sdmmc_card_print_info(stdout, card);

    *out_card = card;

    return ret;
}

void deinit_sd_card(sdmmc_card_t **card) {
// Unmount SD card
#ifdef CONFIG_EXAMPLE_USE_SDMMC
    sdmmc_host_deinit();
#else // CONFIG_EXAMPLE_USE_SDMMC
    sdspi_host_deinit();
#endif // CONFIG_EXAMPLE_USE_SDSPI
    free(*card);
    *card = NULL;
}

#endif // CONFIG_EXAMPLE_TEST_SD_CARD
