/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_check.h"
#include "esp_probe.h"
#include "esp_probe_streams.h"

// Alias of the Kconfig options
#if CONFIG_EXAMPLE_TCP_STREAM
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "esp_event.h"

#define EXAMPLE_HOST_IP_ADDR    CONFIG_EXAMPLE_HOST_IP_ADDR     // Host IP (string)
#define EXAMPLE_HOST_PORT       CONFIG_EXAMPLE_HOST_PORT        // Host port (int)
#elif CONFIG_EXAMPLE_FLASH_STREAM
#define EXAMPLE_MOUNT_POINT     "/esp_probe"
#define EXAMPLE_DATA_FILE_PATH  EXAMPLE_MOUNT_POINT"/probe_raw.dat"
#define EXAMPLE_PARTITION_LABEL CONFIG_EXAMPLE_PARTITION_LABEL  // Flash partition label (string, see 'partitions.csv')
#endif

#define EXAMPLE_SAMPLE_RATE_HZ          (8 * 1000 * 1000)
#define EXAMPLE_STORAGE_DEPTH_KB        128

// GPIOs to probe
const static int s_probe_gpio[] = {2, 3, 4, 5};
const static char *TAG = "example";
static esp_probe_handle_t s_probe = NULL;

#if CONFIG_EXAMPLE_INTERNAL_SIGNAL
void example_init_gpio_to_generate_internal_signal(void)
{
    gpio_config_t gpio_cfg = {
        .pin_bit_mask = BIT(s_probe_gpio[0]) |
        BIT(s_probe_gpio[1]) |
        BIT(s_probe_gpio[2]) |
        BIT(s_probe_gpio[3]),
        .mode = GPIO_MODE_INPUT_OUTPUT,
    };
    gpio_config(&gpio_cfg);
}
#endif

void example_probe_function(void)
{
#if CONFIG_EXAMPLE_INTERNAL_SIGNAL
    // Simulate the internal signals
    // Pulses might not spread averagely because the thread might be interrupted
    for (int cnt = 0; cnt < 100; cnt++) {
        for (int i = 0; i < 4; i++) {
            gpio_set_level(s_probe_gpio[i], cnt & BIT(i));
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
#else  // CONFIG_EXAMPLE_EXTERNAL_SIGNAL
    // Probing the external signal here
    vTaskDelay(pdMS_TO_TICKS(100));
#endif  // CONFIG_EXAMPLE_INTERNAL_SIGNAL
}

FILE *example_probe_init(void)
{
    FILE *f = NULL;
    // Create dump stream
#if CONFIG_EXAMPLE_TCP_STREAM
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());
    f = esp_probe_open_tcp_stream(EXAMPLE_HOST_IP_ADDR, EXAMPLE_HOST_PORT);
#elif CONFIG_EXAMPLE_FLASH_STREAM
    ESP_ERROR_CHECK(esp_probe_init_spiflash_fatfs(EXAMPLE_MOUNT_POINT, EXAMPLE_PARTITION_LABEL, NULL));
    f = esp_probe_open_file_stream(EXAMPLE_DATA_FILE_PATH);
#endif
    assert(f);

    // Configure and allocate the ESP probe
    esp_probe_config_t config = ESP_PROBE_DEFAULT_BUFFER_CONFIG(EXAMPLE_SAMPLE_RATE_HZ, EXAMPLE_STORAGE_DEPTH_KB);
    // Set the GPIOs to be probed
    memcpy(&config.probe_gpio, &s_probe_gpio, sizeof(s_probe_gpio));
    ESP_ERROR_CHECK(esp_new_probe(&config, &s_probe));
    return f;
}

void example_probe_signals(FILE *f, void (*probe_func)(void))
{
    uint32_t dump_data_size = 0;
    // Probe the signals during the function, the data will be dumped via the out stream
    ESP_ERROR_CHECK(esp_probe_start(s_probe, f));
    probe_func();
    ESP_ERROR_CHECK(esp_probe_stop(s_probe, &dump_data_size));
    ESP_LOGI(TAG, "Probe finished! %"PRIu32" (0x%"PRIx32") bytes dumped\n", dump_data_size, dump_data_size);
}

void example_probe_deinit(FILE *f)
{
    // Delete the probe instance and free the resources
    ESP_ERROR_CHECK(esp_del_probe(s_probe));
    s_probe = NULL;

    // Close the output stream
#if CONFIG_EXAMPLE_TCP_STREAM
    esp_probe_close_tcp_stream(f);
    example_disconnect();
#elif CONFIG_EXAMPLE_FLASH_STREAM
    esp_probe_close_file_stream(f);
    esp_probe_deinit_spiflash_fatfs(EXAMPLE_PARTITION_LABEL);
#endif
}

void app_main(void)
{
#if CONFIG_EXAMPLE_INTERNAL_SIGNAL
    // If choose to probe the internal signals via IO MUX, init GPIOs for simulating the internal signals
    example_init_gpio_to_generate_internal_signal();
#endif
    // Initialize the probe, the probe needs an output stream to dump the raw data
    FILE *f = example_probe_init();
    // Use the probe as decorator, probe the signals that generated during this function
    example_probe_signals(f, example_probe_function);
    // Deinitialize the probe, close the output stream
    example_probe_deinit(f);
}
