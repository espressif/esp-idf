/* OpenThread Border Router Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>

#include "esp_check.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_netif_ip_addr.h"
#include "esp_netif_net_stack.h"
#include "esp_openthread.h"
#include "esp_openthread_border_router.h"
#include "esp_openthread_cli.h"
#include "esp_openthread_lock.h"
#include "esp_openthread_netif_glue.h"
#include "esp_openthread_types.h"
#include "esp_ot_config.h"
#include "esp_vfs_eventfd.h"
#include "esp_wifi.h"
#include "mdns.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "sdkconfig.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"
#include "hal/uart_types.h"
#include "openthread/border_router.h"
#include "openthread/cli.h"
#include "openthread/dataset.h"
#include "openthread/dataset_ftd.h"
#include "openthread/dataset_updater.h"
#include "openthread/error.h"
#include "openthread/instance.h"
#include "openthread/ip6.h"
#include "openthread/logging.h"
#include "openthread/tasklet.h"
#include "openthread/thread.h"
#include "openthread/thread_ftd.h"

#define TAG "esp_ot_br"

static int hex_digit_to_int(char hex)
{
    if ('A' <= hex && hex <= 'F') {
        return 10 + hex - 'A';
    }
    if ('a' <= hex && hex <= 'f') {
        return 10 + hex - 'a';
    }
    if ('0' <= hex && hex <= '9') {
        return hex - '0';
    }
    return -1;
}

static size_t hex_string_to_binary(const char *hex_string, uint8_t *buf, size_t buf_size)
{
    int num_char = strlen(hex_string);

    if (num_char != buf_size * 2) {
        return 0;
    }
    for (size_t i = 0; i < num_char; i += 2) {
        int digit0 = hex_digit_to_int(hex_string[i]);
        int digit1 = hex_digit_to_int(hex_string[i + 1]);

        if (digit0 < 0 || digit1 < 0) {
            return 0;
        }
        buf[i / 2] = (digit0 << 4) + digit1;
    }

    return buf_size;
}

static void create_config_network(otInstance *instance)
{
    otOperationalDataset dataset;

    if (otDatasetGetActive(instance, &dataset) == OT_ERROR_NONE) {
        ESP_LOGI(TAG, "Already has network, skip configuring OpenThread network.");
        return;
    }

    uint16_t network_name_len = strnlen(CONFIG_OPENTHREAD_NETWORK_NAME, OT_NETWORK_NAME_MAX_SIZE + 1);

    assert(network_name_len <= OT_NETWORK_NAME_MAX_SIZE);

    if (otDatasetCreateNewNetwork(instance, &dataset) != OT_ERROR_NONE) {
        ESP_LOGE(TAG, "Failed to create OpenThread network dataset.");
        abort();
    }
    dataset.mChannel = CONFIG_OPENTHREAD_NETWORK_CHANNEL;
    dataset.mComponents.mIsChannelPresent = true;
    dataset.mPanId = CONFIG_OPENTHREAD_NETWORK_PANID;
    dataset.mComponents.mIsPanIdPresent = true;
    memcpy(dataset.mNetworkName.m8, CONFIG_OPENTHREAD_NETWORK_NAME, network_name_len);
    dataset.mComponents.mIsNetworkNamePresent = true;
    if (hex_string_to_binary(CONFIG_OPENTHREAD_NETWORK_EXTPANID, dataset.mExtendedPanId.m8,
                             sizeof(dataset.mExtendedPanId.m8)) != sizeof(dataset.mExtendedPanId.m8)) {
        ESP_LOGE(TAG, "Cannot convert OpenThread extended pan id. Please double-check your config.");
        abort();
    }
    dataset.mComponents.mIsExtendedPanIdPresent = true;
    if (hex_string_to_binary(CONFIG_OPENTHREAD_NETWORK_MASTERKEY, dataset.mNetworkKey.m8,
                             sizeof(dataset.mNetworkKey.m8)) != sizeof(dataset.mNetworkKey.m8)) {
        ESP_LOGE(TAG, "Cannot convert OpenThread master key. Please double-check your config.");
        abort();
    }
    dataset.mComponents.mIsNetworkKeyPresent = true;
    if (hex_string_to_binary(CONFIG_OPENTHREAD_NETWORK_PSKC, dataset.mPskc.m8, sizeof(dataset.mPskc.m8)) !=
            sizeof(dataset.mPskc.m8)) {
        ESP_LOGE(TAG, "Cannot convert OpenThread pre-shared commissioner key. Please double-check your config.");
        abort();
    }
    dataset.mComponents.mIsPskcPresent = true;
    if (otDatasetSetActive(instance, &dataset) != OT_ERROR_NONE) {
        ESP_LOGE(TAG, "Failed to set OpenThread active dataset.");
        abort();
    }
    if (otBorderRouterRegister(instance) != OT_ERROR_NONE) {
        ESP_LOGE(TAG, "Failed to register border router.");
        abort();
    }
    return;
}

static void launch_openthread_network(otInstance *instance)
{
    if (otIp6SetEnabled(instance, true) != OT_ERROR_NONE) {
        ESP_LOGE(TAG, "Failed to enable OpenThread IP6 link");
        abort();
    }
    if (otThreadSetEnabled(instance, true) != OT_ERROR_NONE) {
        ESP_LOGE(TAG, "Failed to enable OpenThread");
        abort();
    }
}

static void ot_task_worker(void *aContext)
{
    esp_openthread_platform_config_t config = {
        .radio_config = ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG(),
        .host_config = ESP_OPENTHREAD_DEFAULT_HOST_CONFIG(),
        .port_config = ESP_OPENTHREAD_DEFAULT_PORT_CONFIG(),
    };

    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_OPENTHREAD();
    esp_netif_t *openthread_netif = esp_netif_new(&cfg);
    assert(openthread_netif != NULL);

    // Initialize the OpenThread stack
    ESP_ERROR_CHECK(esp_openthread_init(&config));

    // Initialize border routing features
    ESP_ERROR_CHECK(esp_netif_attach(openthread_netif, esp_openthread_netif_glue_init(&config)));
    ESP_ERROR_CHECK(esp_openthread_border_router_init(get_example_netif()));

    esp_openthread_lock_acquire(portMAX_DELAY);
    (void)otLoggingSetLevel(CONFIG_LOG_DEFAULT_LEVEL);
    esp_openthread_cli_init();
    create_config_network(esp_openthread_get_instance());
    launch_openthread_network(esp_openthread_get_instance());
    esp_openthread_lock_release();

    // Run the main loop
    esp_openthread_cli_create_task();
    esp_openthread_launch_mainloop();

    // Clean up
    esp_netif_destroy(openthread_netif);
    esp_openthread_netif_glue_deinit();
    esp_vfs_eventfd_unregister();
    vTaskDelete(NULL);
}

void app_main(void)
{
    // Used eventfds:
    // * netif
    // * task queue
    // * border router
    esp_vfs_eventfd_config_t eventfd_config = {
        .max_fds = 3,
    };
    ESP_ERROR_CHECK(esp_vfs_eventfd_register(&eventfd_config));

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
    ESP_ERROR_CHECK(mdns_init());
    ESP_ERROR_CHECK(mdns_hostname_set("esp-ot-br"));
    xTaskCreate(ot_task_worker, "ot_br_main", 20480, xTaskGetCurrentTaskHandle(), 5, NULL);
}
