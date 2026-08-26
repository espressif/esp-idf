/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include <inttypes.h>
#include <errno.h>

#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_console.h"

#include "nvs.h"
#include "nvs_flash.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#if CONFIG_EXAMPLE_CONNECT_WIFI
#include "esp_wifi.h"
#endif
#include "protocol_examples_common.h"

/* TODO: Workaround for pytest failure */
#if CONFIG_EXAMPLE_WIFI_SSID_PWD_FROM_STDIN
#include "driver/uart.h"
#endif

#include "cmd_ota.h"

#define PROMPT_STR CONFIG_IDF_TARGET

static const char *TAG = "example_tee_secure_ota";

void setup_console(void)
{
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    /* Prompt to be printed before each line.
     * This can be customized, made dynamic, etc.
     */
    repl_config.prompt = PROMPT_STR ">";
    repl_config.max_cmdline_length = 128;

    /* Register help command */
    ESP_ERROR_CHECK(esp_console_register_help_command());

    /* Register custom commands */
    const esp_console_cmd_t tee_ota_cmd = {
        .command = "tee_ota",
        .help = "Initiate TEE app OTA - signature: tee_ota <tee_ota_url>",
        .hint = NULL,
        .func = &cmd_tee_app_ota_task,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&tee_ota_cmd));

    const esp_console_cmd_t user_ota_cmd = {
        .command = "user_ota",
        .help = "Initiate User app OTA - signature: user_ota <user_ota_url>",
        .hint = NULL,
        .func = &cmd_user_app_ota_task,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&user_ota_cmd));

    /* TODO: For test cases, the SSID and password is taken through stdin,
     * which installs the UART driver but does not delete it. This causes an
     * issue when we try to install the UART driver again when initialising the console.
     */
#if CONFIG_EXAMPLE_WIFI_SSID_PWD_FROM_STDIN
    ESP_ERROR_CHECK(uart_driver_delete(CONFIG_ESP_CONSOLE_UART_NUM));
#endif

#if defined(CONFIG_ESP_CONSOLE_UART_DEFAULT) || defined(CONFIG_ESP_CONSOLE_UART_CUSTOM)
    esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &repl));
#elif defined(CONFIG_ESP_CONSOLE_USB_CDC)
    esp_console_dev_usb_cdc_config_t hw_config = ESP_CONSOLE_DEV_CDC_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_cdc(&hw_config, &repl_config, &repl));
#elif defined(CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG)
    esp_console_dev_usb_serial_jtag_config_t hw_config = ESP_CONSOLE_DEV_USB_SERIAL_JTAG_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_serial_jtag(&hw_config, &repl_config, &repl));
#else
#error Unsupported console type
#endif

    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}

void app_main(void)
{
    ESP_LOGI(TAG, "OTA with TEE enabled");

    // Initialize NVS.
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // OTA app partition table has a smaller NVS partition size than the non-OTA
        // partition table. This size mismatch may cause NVS initialization to fail.
        // If this happens, we erase NVS partition and initialize NVS again.
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

#if CONFIG_EXAMPLE_CONNECT_WIFI
    /* Ensure to disable any WiFi power save mode, this allows best throughput
     * and hence timings for overall OTA operation.
     */
    esp_wifi_set_ps(WIFI_PS_NONE);
#endif // CONFIG_EXAMPLE_CONNECT_WIFI

    setup_console();
}
