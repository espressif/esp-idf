/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <unistd.h>
#include "esp_log.h"
#include "tcp_server.h"
#include "libuvc/libuvc.h"
#include "libuvc_helper.h"
#include "libuvc_adapter.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "usb/usb_host.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"

static const char *TAG = "example";

#define USB_DISCONNECT_PIN  GPIO_NUM_0

#define FPS 30
#define WIDTH 320
#define HEIGHT 240
#define FORMAT UVC_COLOR_FORMAT_MJPEG // UVC_COLOR_FORMAT_YUYV

// Attached camera can be filtered out based on (non-zero value of) PID, VID, SERIAL_NUMBER
#define PID 0
#define VID 0
#define SERIAL_NUMBER NULL

#define UVC_CHECK(exp) do {                 \
    uvc_error_t _err_ = (exp);              \
    if(_err_ < 0) {                         \
        ESP_LOGE(TAG, "UVC error: %s",      \
                 uvc_error_string(_err_));  \
        assert(0);                          \
    }                                       \
} while(0)

static SemaphoreHandle_t ready_to_uninstall_usb;
static EventGroupHandle_t app_flags;

// Handles common USB host library events
static void usb_lib_handler_task(void *args)
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
        }
    }

    vTaskDelete(NULL);
}

static esp_err_t initialize_usb_host_lib(void)
{
    TaskHandle_t task_handle = NULL;

    const usb_host_config_t host_config = {
        .intr_flags = ESP_INTR_FLAG_LEVEL1
    };

    esp_err_t err = usb_host_install(&host_config);
    if (err != ESP_OK) {
        return err;
    }

    ready_to_uninstall_usb = xSemaphoreCreateBinary();
    if (ready_to_uninstall_usb == NULL) {
        usb_host_uninstall();
        return ESP_ERR_NO_MEM;
    }

    if (xTaskCreate(usb_lib_handler_task, "usb_events", 4096, NULL, 2, &task_handle) != pdPASS) {
        vSemaphoreDelete(ready_to_uninstall_usb);
        usb_host_uninstall();
        return ESP_ERR_NO_MEM;
    }

    return ESP_OK;
}

static void uninitialize_usb_host_lib(void)
{
    xSemaphoreTake(ready_to_uninstall_usb, portMAX_DELAY);
    vSemaphoreDelete(ready_to_uninstall_usb);

    if ( usb_host_uninstall() != ESP_OK) {
        ESP_LOGE(TAG, "Failed to uninstall usb_host");
    }
}

/* This callback function runs once per frame. Use it to perform any
 * quick processing you need, or have it put the frame into your application's
 * input queue. If this function takes too long, you'll start losing frames. */
void frame_callback(uvc_frame_t *frame, void *ptr)
{
    static size_t fps;
    static size_t bytes_per_second;
    static int64_t start_time;

    int64_t current_time = esp_timer_get_time();
    bytes_per_second += frame->data_bytes;
    fps++;

    if (!start_time) {
        start_time = current_time;
    }

    if (current_time > start_time + 1000000) {
        ESP_LOGI(TAG, "fps: %u, bytes per second: %u", fps, bytes_per_second);
        start_time = current_time;
        bytes_per_second = 0;
        fps = 0;
    }

    // Stream received frame to client, if enabled
    tcp_server_send(frame->data, frame->data_bytes);
}

void button_callback(int button, int state, void *user_ptr)
{
    printf("button %d state %d\n", button, state);
}

static void libuvc_adapter_cb(libuvc_adapter_event_t event)
{
    xEventGroupSetBits(app_flags, event);
}

static EventBits_t wait_for_event(EventBits_t event)
{
    return xEventGroupWaitBits(app_flags, event, pdTRUE, pdFALSE, portMAX_DELAY) & event;
}

int app_main(int argc, char **argv)
{
    uvc_context_t *ctx;
    uvc_device_t *dev;
    uvc_device_handle_t *devh;
    uvc_stream_ctrl_t ctrl;
    uvc_error_t res;

    app_flags = xEventGroupCreate();
    assert(app_flags);

    const gpio_config_t input_pin = {
        .pin_bit_mask = BIT64(USB_DISCONNECT_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    ESP_ERROR_CHECK( gpio_config(&input_pin) );

    ESP_ERROR_CHECK( initialize_usb_host_lib() );

    libuvc_adapter_config_t config = {
        .create_background_task = true,
        .task_priority = 5,
        .stack_size = 4096,
        .callback = libuvc_adapter_cb
    };

    libuvc_adapter_set_config(&config);

    UVC_CHECK( uvc_init(&ctx, NULL) );

    // Streaming takes place only when enabled in menuconfig
    ESP_ERROR_CHECK( tcp_server_wait_for_connection() );

    do {

        printf("Waiting for device\n");
        wait_for_event(UVC_DEVICE_CONNECTED);

        UVC_CHECK( uvc_find_device(ctx, &dev, PID, VID, SERIAL_NUMBER) );
        puts("Device found");

        UVC_CHECK( uvc_open(dev, &devh) );

        // Uncomment to print configuration descriptor
        // libuvc_adapter_print_descriptors(devh);

        uvc_set_button_callback(devh, button_callback, NULL);

        // Print known device information
        uvc_print_diag(devh, stderr);

        // Negotiate stream profile
        res = uvc_get_stream_ctrl_format_size(devh, &ctrl, FORMAT, WIDTH, HEIGHT, FPS );
        while (res != UVC_SUCCESS) {
            printf("Negotiating streaming format failed, trying again...\n");
            res = uvc_get_stream_ctrl_format_size(devh, &ctrl, FORMAT, WIDTH, HEIGHT, FPS );
            sleep(1);
        }

        // dwMaxPayloadTransferSize has to be overwritten to MPS (maximum packet size)
        // supported by ESP32-S2(S3), as libuvc selects the highest possible MPS by default.
        ctrl.dwMaxPayloadTransferSize = 512;

        uvc_print_stream_ctrl(&ctrl, stderr);

        UVC_CHECK( uvc_start_streaming(devh, &ctrl, frame_callback, NULL, 0) );
        puts("Streaming...");

        wait_for_event(UVC_DEVICE_DISCONNECTED);

        uvc_stop_streaming(devh);
        puts("Done streaming.");

        uvc_close(devh);

    } while (gpio_get_level(USB_DISCONNECT_PIN) != 0);

    tcp_server_close_when_done();

    uvc_exit(ctx);
    puts("UVC exited");

    uninitialize_usb_host_lib();

    return 0;
}
