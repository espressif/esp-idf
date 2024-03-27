/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <dirent.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_timer.h"
#include "esp_err.h"
#include "esp_log.h"
#include "usb/usb_host.h"
#include "usb/msc_host.h"
#include "usb/msc_host_vfs.h"
#include "ffconf.h"
#include "errno.h"
#include "driver/gpio.h"

static const char *TAG = "example";
#define MNT_PATH         "/usb"     // Path in the Virtual File System, where the USB flash drive is going to be mounted
#define APP_QUIT_PIN     GPIO_NUM_0 // BOOT button on most boards
#define BUFFER_SIZE      4096       // The read/write performance can be improved with larger buffer for the cost of RAM, 4kB is enough for most usecases

/**
 * @brief Application Queue and its messages ID
 */
static QueueHandle_t app_queue;
typedef struct {
    enum {
        APP_QUIT,                // Signals request to exit the application
        APP_DEVICE_CONNECTED,    // USB device connect event
        APP_DEVICE_DISCONNECTED, // USB device disconnect event
    } id;
    union {
        uint8_t new_dev_address; // Address of new USB device for APP_DEVICE_CONNECTED event if
    } data;
} app_message_t;

/**
 * @brief BOOT button pressed callback
 *
 * Signal application to exit the main task
 *
 * @param[in] arg Unused
 */
static void gpio_cb(void *arg)
{
    BaseType_t xTaskWoken = pdFALSE;
    app_message_t message = {
        .id = APP_QUIT,
    };

    if (app_queue) {
        xQueueSendFromISR(app_queue, &message, &xTaskWoken);
    }

    if (xTaskWoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

/**
 * @brief MSC driver callback
 *
 * Signal device connection/disconnection to the main task
 *
 * @param[in] event MSC event
 * @param[in] arg   MSC event data
 */
static void msc_event_cb(const msc_host_event_t *event, void *arg)
{
    if (event->event == MSC_DEVICE_CONNECTED) {
        ESP_LOGI(TAG, "MSC device connected");
        app_message_t message = {
            .id = APP_DEVICE_CONNECTED,
            .data.new_dev_address = event->device.address,
        };
        xQueueSend(app_queue, &message, portMAX_DELAY);
    } else if (event->event == MSC_DEVICE_DISCONNECTED) {
        ESP_LOGI(TAG, "MSC device disconnected");
        app_message_t message = {
            .id = APP_DEVICE_DISCONNECTED,
        };
        xQueueSend(app_queue, &message, portMAX_DELAY);
    }
}

static void print_device_info(msc_host_device_info_t *info)
{
    const size_t megabyte = 1024 * 1024;
    uint64_t capacity = ((uint64_t)info->sector_size * info->sector_count) / megabyte;

    printf("Device info:\n");
    printf("\t Capacity: %llu MB\n", capacity);
    printf("\t Sector size: %"PRIu32"\n", info->sector_size);
    printf("\t Sector count: %"PRIu32"\n", info->sector_count);
    printf("\t PID: 0x%04X \n", info->idProduct);
    printf("\t VID: 0x%04X \n", info->idVendor);
#ifndef CONFIG_NEWLIB_NANO_FORMAT
    wprintf(L"\t iProduct: %S \n", info->iProduct);
    wprintf(L"\t iManufacturer: %S \n", info->iManufacturer);
    wprintf(L"\t iSerialNumber: %S \n", info->iSerialNumber);
#endif
}

static void file_operations(void)
{
    const char *directory = "/usb/esp";
    const char *file_path = "/usb/esp/test.txt";

    // Create /usb/esp directory
    struct stat s = {0};
    bool directory_exists = stat(directory, &s) == 0;
    if (!directory_exists) {
        if (mkdir(directory, 0775) != 0) {
            ESP_LOGE(TAG, "mkdir failed with errno: %s", strerror(errno));
        }
    }

    // Create /usb/esp/test.txt file, if it doesn't exist
    if (stat(file_path, &s) != 0) {
        ESP_LOGI(TAG, "Creating file");
        FILE *f = fopen(file_path, "w");
        if (f == NULL) {
            ESP_LOGE(TAG, "Failed to open file for writing");
            return;
        }
        fprintf(f, "Hello World!\n");
        fclose(f);
    }

    // Read back the file
    FILE *f;
    ESP_LOGI(TAG, "Reading file");
    f = fopen(file_path, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }
    char line[64];
    fgets(line, sizeof(line), f);
    fclose(f);
    // strip newline
    char *pos = strchr(line, '\n');
    if (pos) {
        *pos = '\0';
    }
    ESP_LOGI(TAG, "Read from file '%s': '%s'", file_path, line);
}

void speed_test(void)
{
#define TEST_FILE "/usb/esp/dummy"
#define ITERATIONS  256  // 256 * 4kb = 1MB
    int64_t test_start, test_end;

    FILE *f = fopen(TEST_FILE, "wb+");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }
    // Set larger buffer for this file. It results in larger and more effective USB transfers
    setvbuf(f, NULL, _IOFBF, BUFFER_SIZE);

    // Allocate application buffer used for read/write
    uint8_t *data = malloc(BUFFER_SIZE);
    assert(data);

    ESP_LOGI(TAG, "Writing to file %s", TEST_FILE);
    test_start = esp_timer_get_time();
    for (int i = 0; i < ITERATIONS; i++) {
        if (fwrite(data, BUFFER_SIZE, 1, f) == 0) {
            return;
        }
    }
    test_end = esp_timer_get_time();
    ESP_LOGI(TAG, "Write speed %1.2f MiB/s", (BUFFER_SIZE * ITERATIONS) / (float)(test_end - test_start));
    rewind(f);

    ESP_LOGI(TAG, "Reading from file %s", TEST_FILE);
    test_start = esp_timer_get_time();
    for (int i = 0; i < ITERATIONS; i++) {
        if (0 == fread(data, BUFFER_SIZE, 1, f)) {
            return;
        }
    }
    test_end = esp_timer_get_time();
    ESP_LOGI(TAG, "Read speed %1.2f MiB/s", (BUFFER_SIZE * ITERATIONS) / (float)(test_end - test_start));

    fclose(f);
    free(data);
}

/**
 * @brief USB task
 *
 * Install USB Host Library and MSC driver.
 * Handle USB Host Library events
 *
 * @param[in] args Unused
 */
static void usb_task(void *args)
{
    const usb_host_config_t host_config = { .intr_flags = ESP_INTR_FLAG_LEVEL1 };
    ESP_ERROR_CHECK(usb_host_install(&host_config));

    const msc_host_driver_config_t msc_config = {
        .create_backround_task = true,
        .task_priority = 5,
        .stack_size = 4096,
        .callback = msc_event_cb,
    };
    ESP_ERROR_CHECK(msc_host_install(&msc_config));

    bool has_clients = true;
    while (true) {
        uint32_t event_flags;
        usb_host_lib_handle_events(portMAX_DELAY, &event_flags);

        // Release devices once all clients has deregistered
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
            has_clients = false;
            if (usb_host_device_free_all() == ESP_OK) {
                break;
            };
        }
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE && !has_clients) {
            break;
        }
    }

    vTaskDelay(10); // Give clients some time to uninstall
    ESP_LOGI(TAG, "Deinitializing USB");
    ESP_ERROR_CHECK(usb_host_uninstall());
    vTaskDelete(NULL);
}

void app_main(void)
{
    // Create FreeRTOS primitives
    app_queue = xQueueCreate(5, sizeof(app_message_t));
    assert(app_queue);

    BaseType_t task_created = xTaskCreate(usb_task, "usb_task", 4096, NULL, 2, NULL);
    assert(task_created);

    // Init BOOT button: Pressing the button simulates app request to exit
    // It will disconnect the USB device and uninstall the MSC driver and USB Host Lib
    const gpio_config_t input_pin = {
        .pin_bit_mask = BIT64(APP_QUIT_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };
    ESP_ERROR_CHECK(gpio_config(&input_pin));
    ESP_ERROR_CHECK(gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1));
    ESP_ERROR_CHECK(gpio_isr_handler_add(APP_QUIT_PIN, gpio_cb, NULL));

    ESP_LOGI(TAG, "Waiting for USB flash drive to be connected");
    msc_host_device_handle_t msc_device = NULL;
    msc_host_vfs_handle_t vfs_handle = NULL;

    // Perform all example operations in a loop to allow USB reconnections
    while (1) {
        app_message_t msg;
        xQueueReceive(app_queue, &msg, portMAX_DELAY);

        if (msg.id == APP_DEVICE_CONNECTED) {
            // 1. MSC flash drive connected. Open it and map it to Virtual File System
            ESP_ERROR_CHECK(msc_host_install_device(msg.data.new_dev_address, &msc_device));
            const esp_vfs_fat_mount_config_t mount_config = {
                .format_if_mount_failed = false,
                .max_files = 3,
                .allocation_unit_size = 8192,
            };
            ESP_ERROR_CHECK(msc_host_vfs_register(msc_device, MNT_PATH, &mount_config, &vfs_handle));

            // 2. Print information about the connected disk
            msc_host_device_info_t info;
            ESP_ERROR_CHECK(msc_host_get_device_info(msc_device, &info));
            msc_host_print_descriptors(msc_device);
            print_device_info(&info);

            // 3. List all the files in root directory
            ESP_LOGI(TAG, "ls command output:");
            struct dirent *d;
            DIR *dh = opendir(MNT_PATH);
            assert(dh);
            while ((d = readdir(dh)) != NULL) {
                printf("%s\n", d->d_name);
            }
            closedir(dh);

            // 4. The disk is mounted to Virtual File System, perform some basic demo file operation
            file_operations();

            // 5. Perform speed test
            speed_test();

            ESP_LOGI(TAG, "Example finished, you can disconnect the USB flash drive");
        }
        if ((msg.id == APP_DEVICE_DISCONNECTED) || (msg.id == APP_QUIT)) {
            if (vfs_handle) {
                ESP_ERROR_CHECK(msc_host_vfs_unregister(vfs_handle));
                vfs_handle = NULL;
            }
            if (msc_device) {
                ESP_ERROR_CHECK(msc_host_uninstall_device(msc_device));
                msc_device = NULL;
            }
            if (msg.id == APP_QUIT) {
                // This will cause the usb_task to exit
                ESP_ERROR_CHECK(msc_host_uninstall());
                break;
            }
        }
    }

    ESP_LOGI(TAG, "Done");
    gpio_isr_handler_remove(APP_QUIT_PIN);
    vQueueDelete(app_queue);
}
