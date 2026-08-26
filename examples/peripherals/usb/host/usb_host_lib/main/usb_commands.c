/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include "esp_console.h"
#include "esp_log.h"
#include "soc/soc_caps.h"
#include "usb/usb_host.h"
#include "usb_host_lib_example.h"
#include "sdkconfig.h"

static const char *TAG = "usb_cmd";

#if SOC_USB_OTG_PERIPH_NUM > 1
#define USB_INSTALL_CMD_HINT "<HS|FS|both>"
#define USB_INSTALL_CMD_HELP "Install USB Host stack and class driver on specified port (HS: High Speed port, FS: Full Speed port, both: all available ports; no argument defaults to HS)"
#else
#define USB_INSTALL_CMD_HINT NULL
#define USB_INSTALL_CMD_HELP "Install USB Host stack and class driver (no-op if already installed)"
#endif

#define HOST_LIB_TASK_PRIORITY    2
#define CLASS_TASK_PRIORITY       3

#ifdef CONFIG_USB_HOST_ENABLE_ENUM_FILTER_CALLBACK
#define ENABLE_ENUM_FILTER_CALLBACK
#endif // CONFIG_USB_HOST_ENABLE_ENUM_FILTER_CALLBACK

static uint8_t s_usb_stack_running = 0; // Bitmap of running USB host ports, 0 means not installed
static SemaphoreHandle_t s_usb_lifecycle_mx = NULL;
static SemaphoreHandle_t s_host_stopped_sem = NULL;

typedef struct {
    TaskHandle_t task_to_notify;
    unsigned peripheral_map;
} usb_host_lib_task_args_t;

static int usb_install_cmd(int argc, char **argv)
{
    unsigned peripheral_map;

#if SOC_USB_OTG_PERIPH_NUM > 1
    if (argc > 2) {
        ESP_LOGE(TAG, "Usage: usb_install <HS|FS|both>");
        return 1;
    }

    if (argc == 1 || strcmp(argv[1], "HS") == 0) {
        peripheral_map = BIT0;
    } else if (strcmp(argv[1], "FS") == 0) {
        peripheral_map = BIT1;
    } else if (strcmp(argv[1], "both") == 0) {
        peripheral_map = BIT0 | BIT1;
    } else {
        ESP_LOGE(TAG, "Invalid argument '%s', expected HS, FS, or both", argv[1]);
        return 1;
    }
#else
    if (argc != 1) {
        ESP_LOGE(TAG, "Usage: usb_install");
        return 1;
    }
    peripheral_map = BIT0;
#endif

    esp_err_t err = usb_example_install(peripheral_map);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "USB install failed: %s", esp_err_to_name(err));
        return 1;
    }
    return 0;
}

/**
 * @brief Set configuration callback
 *
 * Set the USB device configuration during the enumeration process, must be enabled in the menuconfig
 *
 * @note bConfigurationValue starts at index 1
 *
 * @param[in] dev_desc device descriptor of the USB device currently being enumerated
 * @param[out] bConfigurationValue configuration descriptor index, that will be user for enumeration
 *
 * @return bool
 * - true:  USB device will be enumerated
 * - false: USB device will not be enumerated
 */
#ifdef ENABLE_ENUM_FILTER_CALLBACK
static bool set_config_cb(const usb_device_desc_t *dev_desc, uint8_t *bConfigurationValue)
{
    if (dev_desc->bNumConfigurations > 1) {
        *bConfigurationValue = 2;
    } else {
        *bConfigurationValue = 1;
    }
    return true;
}
#endif // ENABLE_ENUM_FILTER_CALLBACK

/**
 * @brief USB Host install and event loop; uninstalls when there are no clients and no devices.
 */
static void usb_host_lib_task(void *arg)
{
    usb_host_lib_task_args_t *task_args = (usb_host_lib_task_args_t *)arg;

    ESP_LOGI(TAG, "Installing USB Host Library");
    usb_host_config_t host_config = {
        .skip_phy_setup = false,
        .intr_flags = ESP_INTR_FLAG_LOWMED,
#ifdef ENABLE_ENUM_FILTER_CALLBACK
        .enum_filter_cb = set_config_cb,
#endif // ENABLE_ENUM_FILTER_CALLBACK
        .peripheral_map = task_args->peripheral_map,
    };
    if (ESP_OK != usb_host_install(&host_config)) {
        ESP_LOGE(TAG, "Failed to install USB Host Library");
        vTaskDelete(NULL);
    }
    xTaskNotifyGive(task_args->task_to_notify);
    ESP_LOGI(TAG, "USB Host installed with peripheral map 0x%x", host_config.peripheral_map);

    bool has_clients = true;
    bool has_devices = false;
    while (has_clients) {
        uint32_t event_flags;
        ESP_ERROR_CHECK(usb_host_lib_handle_events(portMAX_DELAY, &event_flags)); // Returns error only if the library is not installed, which should not happen since we are inside the library task
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
            if (ESP_OK == usb_host_device_free_all()) {
                ESP_LOGI(TAG, "All devices marked as free, no need to wait FLAGS_ALL_FREE event");
                has_clients = false;
            } else {
                ESP_LOGI(TAG, "Wait for the FLAGS_ALL_FREE");
                has_devices = true;
            }
        }
        if (has_devices && event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE) {
            ESP_LOGI(TAG, "Got FLAGS_ALL_FREE");
            has_clients = false;
        }
    }
    ESP_LOGI(TAG, "No more clients and devices, uninstall USB Host library");

    ESP_ERROR_CHECK(usb_host_uninstall());
    if (s_host_stopped_sem) {
        xSemaphoreGive(s_host_stopped_sem);
    }
    vTaskDelete(NULL);
}

esp_err_t usb_example_install(unsigned peripheral_map)
{
    if (s_usb_lifecycle_mx == NULL) {
        s_usb_lifecycle_mx = xSemaphoreCreateMutex();
        assert(s_usb_lifecycle_mx);
    }
    if (s_host_stopped_sem == NULL) {
        s_host_stopped_sem = xSemaphoreCreateBinary();
        assert(s_host_stopped_sem);
    }

    xSemaphoreTake(s_usb_lifecycle_mx, portMAX_DELAY);

    esp_err_t ret = ESP_OK;
    if (s_usb_stack_running != 0) {
        ESP_LOGW(TAG, "USB host stack already installed");
        xSemaphoreGive(s_usb_lifecycle_mx);
        return ESP_OK;
    }

    usb_host_lib_task_args_t task_args = {
        .task_to_notify = xTaskGetCurrentTaskHandle(),
        .peripheral_map = peripheral_map,
    };
    BaseType_t task_created = xTaskCreatePinnedToCore(usb_host_lib_task,
                                                      "usb_host",
                                                      4096,
                                                      &task_args,
                                                      HOST_LIB_TASK_PRIORITY,
                                                      NULL,
                                                      0);
    assert(task_created == pdTRUE);
    if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(5000)) == 0) {
        ESP_LOGE(TAG, "Timeout waiting for USB host install");
        abort();
    }

    task_created = xTaskCreatePinnedToCore(class_driver_task,
                                           "class",
                                           5 * 1024,
                                           xTaskGetCurrentTaskHandle(),
                                           CLASS_TASK_PRIORITY,
                                           NULL,
                                           0);
    assert(task_created == pdTRUE);
    if (ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(5000)) == 0) {
        ESP_LOGE(TAG, "Timeout waiting client registration");
        abort();
    }

    s_usb_stack_running = peripheral_map;
    ESP_LOGI(TAG, "USB host stack installed (class driver running)");

    xSemaphoreGive(s_usb_lifecycle_mx);
    return ret;
}

/**
 * @brief Tear down the class driver client and uninstall the USB Host Library
 *
 * Deregisters the class driver, waits for the host library task to call `usb_host_uninstall()`,
 * and clears the installed state.
 *
 * @return
 *     - ESP_OK on success
 *     - ESP_ERR_INVALID_STATE if the USB host stack is not installed
 */
static esp_err_t usb_example_teardown_locked(void)
{
    xSemaphoreTake(s_usb_lifecycle_mx, portMAX_DELAY);
    if (s_usb_stack_running == 0) {
        ESP_LOGW(TAG, "USB host stack not installed, nothing to uninstall");
        xSemaphoreGive(s_usb_lifecycle_mx);
        return ESP_ERR_INVALID_STATE;
    }

    usb_host_lib_info_t lib_info;
    if (usb_host_lib_info(&lib_info) == ESP_OK && lib_info.num_devices != 0) {
        ESP_LOGW(TAG, "Uninstall with %u device(s) still attached", (unsigned)lib_info.num_devices);
    }

    class_driver_client_deregister();

    if (xSemaphoreTake(s_host_stopped_sem, pdMS_TO_TICKS(15000)) != pdTRUE) {
        ESP_LOGW(TAG, "Timeout waiting for USB host library uninstall");
        abort();
    }

    s_usb_stack_running = 0;
    xSemaphoreGive(s_usb_lifecycle_mx);
    return ESP_OK;
}

static int usb_uninstall_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    if (usb_example_teardown_locked() != ESP_OK) {
        ESP_LOGE(TAG, "Failed to teardown USB host library");
        return 1;
    }
    printf("\tUSB Host Library uninstalled\n");
    return 0;
}

static int usb_suspend_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    esp_err_t err = usb_host_lib_root_port_suspend();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to suspend USB root port: %s", esp_err_to_name(err));
        return 1;
    }
    if (s_usb_stack_running == (BIT0 | BIT1)) {
        ESP_LOGW(TAG, "In USB dual host mode, only HS port is suspended"); // This is a software limitation of the current implementation
    }
    printf("\tUSB root port suspended\n");
    return 0;
}

static int usb_resume_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    esp_err_t err = usb_host_lib_root_port_resume();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to resume USB root port: %s", esp_err_to_name(err));
        return 1;
    }
    if (s_usb_stack_running == (BIT0 | BIT1)) {
        ESP_LOGW(TAG, "In USB dual host mode, only HS port is resumed"); // This is a software limitation of the current implementation
    }
    printf("\tUSB root port resumed\n");
    return 0;
}

static int usb_info_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    usb_host_lib_info_t info;
    esp_err_t err = usb_host_lib_info(&info);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "usb_host_lib_info failed: %s", esp_err_to_name(err));
        return 1;
    }

    if (s_usb_stack_running == (BIT0 | BIT1)) {
        ESP_LOGW(TAG, "In USB dual host mode, only HS port can be suspended"); // This is a software limitation of the current implementation
    }

    printf("\tUSB host lib: Devices=%d, Clients=%d, Root port suspended=%s\n\tDevice addresses: ",
           info.num_devices, info.num_clients, info.root_port_suspended ? "yes" : "no");
    if (info.num_devices == 0) {
        printf("None\n");
        return 0;
    }

    uint8_t *dev_addrs = malloc(info.num_devices * sizeof(uint8_t));
    if (dev_addrs == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for device addresses");
        return 1;
    }
    int num_addrs = 0;
    err = usb_host_device_addr_list_fill(info.num_devices, dev_addrs, &num_addrs);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "usb_host_device_addr_list_fill failed: %s", esp_err_to_name(err));
        free(dev_addrs);
        return 1;
    }

    for (int i = 0; i < num_addrs; i++) {
        printf(" %u", (unsigned)dev_addrs[i]);
    }
    printf("\n");

    free(dev_addrs);
    return 0;
}

static int usb_power_cmd(int argc, char **argv)
{
    if (argc != 2) {
        ESP_LOGE(TAG, "Usage: usb_power <0|1>");
        return 1;
    }

    bool enable;
    if (strcmp(argv[1], "0") == 0) {
        enable = false;
    } else if (strcmp(argv[1], "1") == 0) {
        enable = true;
    } else {
        ESP_LOGE(TAG, "Invalid argument '%s', expected 0 or 1", argv[1]);
        return 1;
    }

    esp_err_t err = usb_host_lib_set_root_port_power(enable);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set USB root port power: %s", esp_err_to_name(err));
        return 1;
    }

    printf("\tUSB root port power %s\n", enable ? "enabled" : "disabled");
    return 0;
}

void register_usb(void)
{
    const esp_console_cmd_t cmd_install = {
        .command = "usb_install",
        .help = USB_INSTALL_CMD_HELP,
        .hint = USB_INSTALL_CMD_HINT,
        .func = &usb_install_cmd,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd_install));

    const esp_console_cmd_t cmd_uninstall = {
        .command = "usb_uninstall",
        .help = "Uninstall USB Host stack (use usb_install to restore)",
        .hint = NULL,
        .func = &usb_uninstall_cmd,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd_uninstall));

    const esp_console_cmd_t cmd_info = {
        .command = "usb_info",
        .help = "Print USB Host Library info and connected device bus addresses",
        .hint = NULL,
        .func = &usb_info_cmd,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd_info));

    const esp_console_cmd_t cmd_suspend = {
        .command = "usb_suspend",
        .help = "Suspend USB root port",
        .hint = NULL,
        .func = &usb_suspend_cmd,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd_suspend));

    const esp_console_cmd_t cmd_resume = {
        .command = "usb_resume",
        .help = "Resume USB root port",
        .hint = NULL,
        .func = &usb_resume_cmd,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd_resume));

    const esp_console_cmd_t cmd_power = {
        .command = "usb_power",
        .help = "Set USB root port power",
        .hint = "<0|1>",
        .func = &usb_power_cmd,
    };
    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd_power));
}
