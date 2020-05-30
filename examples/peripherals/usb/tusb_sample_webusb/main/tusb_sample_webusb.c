/* USB Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "tinyusb.h"

static const char *TAG = "example";

enum
{
    VENDOR_REQUEST_WEBUSB = 1,
    VENDOR_REQUEST_MICROSOFT = 2
};

uint8_t const desc_ms_os_20[100] = {0};

#define URL "www.tinyusb.org/examples/webusb-serial"

const tusb_desc_webusb_url_t desc_url =
    {
        .bLength = 3 + sizeof(URL) - 1,
        .bDescriptorType = 3, // WEBUSB URL type
        .bScheme = 1,         // 0: http, 1: https
        .url = URL};

static bool web_serial_connected = false;

//------------- prototypes -------------//
void cdc_task(void *);
static void webserial_task(void *p);

// USB Device Driver task
// This top level thread processes all usb events and invokes callbacks
static void usb_device_task(void *param)
{
    (void)param;
    ESP_LOGI(TAG, "USB task started");
    while (1)
    {
        tud_task(); // RTOS forever loop
    }
}

void app_main(void)
{

    ESP_LOGI(TAG, "USB initialization");

    tinyusb_config_t tusb_cfg = {
        .descriptor = NULL,
        .string_descriptor = NULL,
        .external_phy = false // In the most cases you need to use a `false` value
    };

    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));
    ESP_LOGI(TAG, "USB initialization DONE");

    // Create a task for tinyusb device stack:
    xTaskCreate(usb_device_task, "usbd", 4096, NULL, 5, NULL);
    xTaskCreate(cdc_task, "cdc", 4096, NULL, 5, NULL);
    xTaskCreate(webserial_task, "web", 4096, NULL, 5, NULL);
    return;
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
    ESP_LOGI(TAG, "%s", __func__);
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
    ESP_LOGI(TAG, "%s", __func__);
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
    (void)remote_wakeup_en;
    ESP_LOGI(TAG, "%s", __func__);
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
    ESP_LOGI(TAG, "%s", __func__);
}

// send characters to both CDC and WebUSB
static void echo_all(uint8_t buf[], uint32_t count)
{
    // echo to web serial
    if (web_serial_connected)
    {
        tud_vendor_write(buf, count);
    }

    // echo to cdc
    if (tud_cdc_connected())
    {
        for (uint32_t i = 0; i < count; i++)
        {
            tud_cdc_write_char(buf[i]);

            if (buf[i] == '\r')
                tud_cdc_write_char('\n');
        }
        tud_cdc_write_flush();
    }
}

//--------------------------------------------------------------------+
// USB CDC
//--------------------------------------------------------------------+
void cdc_task(void *p)
{
    while (1)
    {
        if (tud_cdc_connected())
        {
            // connected and there are data available
            if (tud_cdc_available())
            {
                uint8_t buf[64];

                uint32_t count = tud_cdc_read(buf, sizeof(buf));

                // echo back to both web serial and cdc
                echo_all(buf, count);
            }
        }
    vTaskDelay(10);
    }
}

// Invoked when cdc when line state changed e.g connected/disconnected
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts)
{
    (void)itf;

    // connected
    if (dtr && rts)
    {
        // print initial message when connected
        tud_cdc_write_str("\r\nTinyUSB WebUSB device example\r\n");
    }
}

// Invoked when CDC interface received data from host
void tud_cdc_rx_cb(uint8_t itf)
{
    (void)itf;
}

//--------------------------------------------------------------------+
// WebUSB use vendor class
//--------------------------------------------------------------------+

// Invoked when received VENDOR control request
bool tud_vendor_control_request_cb(uint8_t rhport, tusb_control_request_t const *request)
{
    switch (request->bRequest)
    {
    case VENDOR_REQUEST_WEBUSB:
        // match vendor request in BOS descriptor
        // Get landing page url
        return tud_control_xfer(rhport, request, (void *)&desc_url, desc_url.bLength);

    case VENDOR_REQUEST_MICROSOFT:
        if (request->wIndex == 7)
        {
            // Get Microsoft OS 2.0 compatible descriptor
            uint16_t total_len;
            memcpy(&total_len, desc_ms_os_20 + 8, 2);

            return tud_control_xfer(rhport, request, (void *)desc_ms_os_20, total_len);
        }
        else
        {
            return false;
        }

    case 0x22:
        // Webserial simulate the CDC_REQUEST_SET_CONTROL_LINE_STATE (0x22) to
        // connect and disconnect.
        web_serial_connected = (request->wValue != 0);

        // Always lit LED if connected
        if (web_serial_connected)
        {
            tud_vendor_write_str("\r\nTinyUSB WebUSB device example\r\n");
        }

        // response with status OK
        return tud_control_status(rhport, request);

    default:
        // stall unknown request
        return false;
    }

    return true;
}

// Invoked when DATA Stage of VENDOR's request is complete
bool tud_vendor_control_complete_cb(uint8_t rhport, tusb_control_request_t const *request)
{
    (void)rhport;
    (void)request;

    // nothing to do
    return true;
}

static void webserial_task(void *p)
{
    while (1)
    {

        if (web_serial_connected)
        {
            if (tud_vendor_available())
            {
                uint8_t buf[64];
                uint32_t count = tud_vendor_read(buf, sizeof(buf));

                // echo back to both web serial and cdc
                echo_all(buf, count);
            }
        }
    vTaskDelay(10);
    }
}
