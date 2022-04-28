/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "usb/usb_host.h"
#include "msc_host.h"
#include "msc_host_vfs.h"
#include "ffconf.h"
#include "ff.h"
#include "esp_vfs.h"
#include "errno.h"
#include "hal/usb_hal.h"

static const char* TAG = "example";

static QueueHandle_t app_queue;
static SemaphoreHandle_t ready_to_uninstall_usb;

static void msc_event_cb(const msc_host_event_t *event, void *arg)
{
    if (event->event == MSC_DEVICE_CONNECTED) {
        ESP_LOGI(TAG, "MSC device connected");
    } else if (event->event == MSC_DEVICE_DISCONNECTED) {
        ESP_LOGI(TAG, "MSC device disconnected");
    }
    xQueueSend(app_queue, event, 10);
}

static void print_device_info(msc_host_device_info_t *info)
{
    const size_t megabyte = 1024 * 1024;
    uint64_t capacity = ((uint64_t)info->sector_size * info->sector_count) / megabyte;

    printf("Device info:\n");
    printf("\t Capacity: %llu MB\n", capacity);
    printf("\t Sector size: %u\n", info->sector_size);
    printf("\t Sector count: %u\n", info->sector_count);
    printf("\t PID: 0x%4X \n", info->idProduct);
    printf("\t VID: 0x%4X \n", info->idVendor);
    wprintf(L"\t iProduct: %S \n", info->iProduct);
    wprintf(L"\t iManufacturer: %S \n", info->iManufacturer);
    wprintf(L"\t iSerialNumber: %S \n", info->iSerialNumber);
}

static void file_operations(void)
{
    const char *directory = "/usb/esp";
    const char *file_path = "/usb/esp/test.txt";

    struct stat s = {0};
    bool directory_exists = stat(directory, &s) == 0;
    if (!directory_exists) {
        if (mkdir(directory, 0775) != 0) {
            ESP_LOGE(TAG, "mkdir failed with errno: %s\n", strerror(errno));
        }
    }

    ESP_LOGI(TAG, "Writing file");
    FILE *f = fopen(file_path, "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }
    fprintf(f, "Hello World!\n");
    fclose(f);

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
    ESP_LOGI(TAG, "Read from file: '%s'", line);
}

// Handles common USB host library events
static void handle_usb_events(void *args)
{
    while (1) {
        uint32_t event_flags;
        usb_host_lib_handle_events(portMAX_DELAY, &event_flags);
        // Release devices once all clients has deregistered
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
            usb_host_device_free_all();
        }
        // Give ready_to_uninstall_usb semaphore to indicate that USB Host library
        // can be deinitialized, and terminate this task.
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE) {
            xSemaphoreGive(ready_to_uninstall_usb);
            break;
        }
    }

    vTaskDelete(NULL);
}

static uint8_t wait_for_msc_device(void)
{
    msc_host_event_t app_event;
    ESP_LOGI(TAG, "Waiting for USB stick to be connected");
    xQueueReceive(app_queue, &app_event, portMAX_DELAY);
    assert( app_event.event == MSC_DEVICE_CONNECTED );
    return app_event.device.address;
}

void app_main(void)
{
    msc_host_device_handle_t msc_device;
    BaseType_t task_created;

    ready_to_uninstall_usb = xSemaphoreCreateBinary();

    app_queue = xQueueCreate(3, sizeof(msc_host_event_t));
    assert(app_queue);

    const usb_host_config_t host_config = {
        .skip_phy_setup = false,
        .intr_flags = ESP_INTR_FLAG_LEVEL1,
    };
    ESP_ERROR_CHECK( usb_host_install(&host_config) );

    task_created = xTaskCreate(handle_usb_events, "usb_events", 2048, NULL, 2, NULL);
    assert(task_created);

    const msc_host_driver_config_t msc_config = {
        .create_backround_task = true,
        .task_priority = 5,
        .stack_size = 2048,
        .callback = msc_event_cb,
    };
    ESP_ERROR_CHECK( msc_host_install(&msc_config) );

    uint8_t device_address = wait_for_msc_device();

    ESP_ERROR_CHECK( msc_host_install_device(device_address, &msc_device) );

    msc_host_print_descriptors(msc_device);

    msc_host_device_info_t info;
    ESP_ERROR_CHECK( msc_host_get_device_info(msc_device, &info) );
    print_device_info(&info);

    msc_host_vfs_handle_t vfs_handle;
    const esp_vfs_fat_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 3,
        .allocation_unit_size = 1024,
    };

    ESP_ERROR_CHECK( msc_host_vfs_register(msc_device, "/usb", &mount_config, &vfs_handle) );

    file_operations();

    ESP_ERROR_CHECK( msc_host_vfs_unregister(vfs_handle) );
    ESP_ERROR_CHECK( msc_host_uninstall_device(msc_device) );
    ESP_ERROR_CHECK( msc_host_uninstall() );

    xSemaphoreTake(ready_to_uninstall_usb, portMAX_DELAY);
    ESP_ERROR_CHECK( usb_host_uninstall() );

    ESP_LOGI(TAG, "Done");
}
