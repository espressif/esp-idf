
/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "unity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_private/usb_phy.h"
#include "usb/usb_host.h"
#include "msc_host.h"
#include "msc_host_vfs.h"
#include "ffconf.h"
#include "ff.h"
#include "esp_vfs.h"
#include "test_common.h"
#include "soc/usb_wrap_struct.h"
#include "soc/soc_caps.h"

#if SOC_USB_OTG_SUPPORTED

static const char *TAG = "APP";

#define ESP_OK_ASSERT(exp) TEST_ASSERT_EQUAL(ESP_OK, exp)

static esp_vfs_fat_mount_config_t mount_config = {
    .format_if_mount_failed = false,
    .max_files = 3,
    .allocation_unit_size = 1024,
};

static QueueHandle_t app_queue;
static SemaphoreHandle_t ready_to_deinit_usb;
static msc_host_device_handle_t device;
static msc_host_vfs_handle_t vfs_handle;
static volatile bool waiting_for_sudden_disconnect;
static usb_phy_handle_t phy_hdl = NULL;

static void force_conn_state(bool connected, TickType_t delay_ticks)
{
    TEST_ASSERT(phy_hdl);
    if (delay_ticks > 0) {
        //Delay of 0 ticks causes a yield. So skip if delay_ticks is 0.
        vTaskDelay(delay_ticks);
    }
    ESP_ERROR_CHECK(usb_phy_action(phy_hdl, (connected) ? USB_PHY_ACTION_HOST_ALLOW_CONN : USB_PHY_ACTION_HOST_FORCE_DISCONN));
}

static void msc_event_cb(const msc_host_event_t *event, void *arg)
{
    if (waiting_for_sudden_disconnect) {
        waiting_for_sudden_disconnect = false;
        TEST_ASSERT_EQUAL(MSC_DEVICE_DISCONNECTED, event->event);
    }

    if (event->event == MSC_DEVICE_CONNECTED) {
        printf("MSC_DEVICE_CONNECTED\n");
    } else {
        printf("MSC_DEVICE_DISCONNECTED\n");
    }

    xQueueSend(app_queue, event, 10);
}

static const char *TEST_STRING = "Hello World!";
static const char *FILE_NAME = "/usb/ESP32.txt";

static void write_read_file(const char *file_path)
{
    char line[64];

    ESP_LOGI(TAG, "Writing file");
    FILE *f = fopen(file_path, "w");
    TEST_ASSERT(f);
    fprintf(f, TEST_STRING);
    fclose(f);

    ESP_LOGI(TAG, "Reading file");
    TEST_ASSERT(fopen(file_path, "r"));
    fgets(line, sizeof(line), f);
    fclose(f);
    // strip newline
    char *pos = strchr(line, '\n');
    if (pos) {
        *pos = '\0';
    }
    TEST_ASSERT_EQUAL_STRING(line, TEST_STRING);
    ESP_LOGI(TAG, "Done");
}

static bool file_exists(const char *file_path)
{
    return ( access(file_path, F_OK) == 0 );
}

// Handles common USB host library events
static void handle_usb_events(void *args)
{
    uint32_t end_flags = 0;

    while (1) {
        uint32_t event_flags;
        usb_host_lib_handle_events(portMAX_DELAY, &event_flags);
        // Release devices once all clients has deregistered
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
            printf("USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS\n");
            usb_host_device_free_all();
            end_flags |= 1;
        }
        // Give ready_to_deinit_usb semaphore to indicate that USB Host library
        // can be deinitialized, and terminate this task.
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE) {
            printf("USB_HOST_LIB_EVENT_FLAGS_ALL_FREE\n");
            end_flags |= 2;
        }

        if (end_flags == 3) {
            xSemaphoreGive(ready_to_deinit_usb);
            break;
        }
    }
    vTaskDelete(NULL);
}

static void check_file_content(const char *file_path, const char *expected)
{
    ESP_LOGI(TAG, "Reading %s:", file_path);
    FILE *file = fopen(file_path, "r");
    TEST_ASSERT(file)

    char content[200];
    fread(content, 1, sizeof(content), file);
    TEST_ASSERT_EQUAL_STRING(content, expected);
    fclose(file);
}

static void check_sudden_disconnect(void)
{
    uint8_t data[512];
    const size_t DATA_SIZE = sizeof(data);

    ESP_LOGI(TAG, "Creating test.tx");
    FILE *file = fopen("/usb/test.txt", "w");
    TEST_ASSERT(file);

    ESP_LOGI(TAG, "Write data");
    TEST_ASSERT_EQUAL(DATA_SIZE, fwrite(data, 1, DATA_SIZE, file));
    TEST_ASSERT_EQUAL(DATA_SIZE, fwrite(data, 1, DATA_SIZE, file));
    TEST_ASSERT_EQUAL(0, fflush(file));

    ESP_LOGI(TAG, "Trigger a disconnect");
    //Trigger a disconnect
    waiting_for_sudden_disconnect = true;
    force_conn_state(false, 0);

    // Make sure flag was leared in callback
    vTaskDelay( pdMS_TO_TICKS(100) );
    TEST_ASSERT_FALSE(waiting_for_sudden_disconnect);

    ESP_LOGI(TAG, "Write data after disconnect");
    TEST_ASSERT_NOT_EQUAL( DATA_SIZE, fwrite(data, 1, DATA_SIZE, file));

    fclose(file);
}

static void msc_setup(void)
{
    BaseType_t task_created;

    ready_to_deinit_usb = xSemaphoreCreateBinary();

    TEST_ASSERT( app_queue = xQueueCreate(5, sizeof(msc_host_event_t)) );

    //Initialize the internal USB PHY to connect to the USB OTG peripheral. We manually install the USB PHY for testing
    usb_phy_config_t phy_config = {
        .controller = USB_PHY_CTRL_OTG,
        .target = USB_PHY_TARGET_INT,
        .otg_mode = USB_OTG_MODE_HOST,
        .otg_speed = USB_PHY_SPEED_UNDEFINED,   //In Host mode, the speed is determined by the connected device
        .gpio_conf = NULL,
    };
    ESP_OK_ASSERT(usb_new_phy(&phy_config, &phy_hdl));
    const usb_host_config_t host_config = {
        .skip_phy_setup = true,
        .intr_flags = ESP_INTR_FLAG_LEVEL1,
    };
    ESP_OK_ASSERT( usb_host_install(&host_config) );

    task_created = xTaskCreatePinnedToCore(handle_usb_events, "usb_events", 2048, NULL, 2, NULL, 0);
    TEST_ASSERT(task_created);

    const msc_host_driver_config_t msc_config = {
        .create_backround_task = true,
        .callback = msc_event_cb,
        .stack_size = 4096,
        .task_priority = 5,
    };
    ESP_OK_ASSERT( msc_host_install(&msc_config) );

    ESP_LOGI(TAG, "Waiting for USB stick to be connected");
    msc_host_event_t app_event;
    xQueueReceive(app_queue, &app_event, portMAX_DELAY);
    TEST_ASSERT_EQUAL(MSC_DEVICE_CONNECTED, app_event.event);
    uint8_t device_addr = app_event.device.address;

    ESP_OK_ASSERT( msc_host_install_device(device_addr, &device) );
    ESP_OK_ASSERT( msc_host_vfs_register(device, "/usb", &mount_config, &vfs_handle) );
}

static void msc_teardown(void)
{
    // Wait to finish any ongoing USB operations
    vTaskDelay(100);

    ESP_OK_ASSERT( msc_host_vfs_unregister(vfs_handle) );
    ESP_OK_ASSERT( msc_host_uninstall_device(device) );
    ESP_OK_ASSERT( msc_host_uninstall() );

    xSemaphoreTake(ready_to_deinit_usb, portMAX_DELAY);
    vSemaphoreDelete(ready_to_deinit_usb);
    ESP_OK_ASSERT( usb_host_uninstall() );
    //Tear down USB PHY
    ESP_OK_ASSERT(usb_del_phy(phy_hdl));
    phy_hdl = NULL;

    vQueueDelete(app_queue);
}

static void write_read_sectors(void)
{
    uint8_t write_data[DISK_BLOCK_SIZE];
    uint8_t read_data[DISK_BLOCK_SIZE];

    memset(write_data, 0x55, DISK_BLOCK_SIZE);
    memset(read_data, 0, DISK_BLOCK_SIZE);

    msc_host_write_sector(device, 10, write_data, DISK_BLOCK_SIZE);
    msc_host_read_sector(device, 10, read_data, DISK_BLOCK_SIZE);

    TEST_ASSERT_EQUAL_MEMORY(write_data, read_data, DISK_BLOCK_SIZE);
}

static void erase_storage(void)
{
    uint8_t data[DISK_BLOCK_SIZE];
    memset(data, 0xFF, DISK_BLOCK_SIZE);

    for (int block = 0; block < DISK_BLOCK_NUM; block++) {
        msc_host_write_sector(device, block, data, DISK_BLOCK_SIZE);
    }
}

TEST_CASE("write_and_read_file", "[usb_msc]")
{
    msc_setup();
    write_read_file(FILE_NAME);
    msc_teardown();
}

TEST_CASE("sudden_disconnect", "[usb_msc]")
{
    msc_setup();
    check_sudden_disconnect();
    msc_teardown();
}

TEST_CASE("sectors_can_be_written_and_read", "[usb_msc]")
{
    msc_setup();
    write_read_sectors();
    msc_teardown();
}

TEST_CASE("check_README_content", "[usb_msc]")
{
    msc_setup();
    check_file_content("/usb/README.TXT", README_CONTENTS);
    msc_teardown();
}

/**
 * @brief USB MSC format testcase
 * @attention This testcase deletes all content on the USB MSC device.
 *            The device must be reset in order to contain the FILE_NAME again.
 */
TEST_CASE("can_be_formated", "[usb_msc]")
{
    printf("Create file\n");
    msc_setup();
    write_read_file(FILE_NAME);
    msc_teardown();

    printf("File exists after mounting again\n");
    msc_setup();
    TEST_ASSERT(file_exists(FILE_NAME));
    printf("Erase storage device\n");
    erase_storage();
    msc_teardown();

    printf("Check file does not exist after formatting\n");
    mount_config.format_if_mount_failed = true;
    msc_setup();
    TEST_ASSERT_FALSE(file_exists(FILE_NAME));
    msc_teardown();
    mount_config.format_if_mount_failed = false;
}

TEST_CASE("mock_device_app", "[usb_msc_device][ignore]")
{
    device_app();
}

#endif
