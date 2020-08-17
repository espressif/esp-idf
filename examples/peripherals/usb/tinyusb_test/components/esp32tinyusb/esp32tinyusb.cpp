#include "esp_log.h"
#include "esp32tinyusb.h"

#define TAG "ESP_tinyUSB"

TinyUSB::TinyUSB()
{
}

void TinyUSB::init()
{
    ESP_LOGI(TAG, "USB initialization");

    tinyusb_config_t tusb_cfg = {
        .descriptor = NULL,
        .string_descriptor = NULL,
        .external_phy = false // In the most cases you need to use a `false` value
    };

    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));
    ESP_LOGI(TAG, "USB initialization DONE");
}

void TinyUSB::initCustom()
{

}

void TinyUSB::addCDCdescriptor()
{
// #define ARDUINO_USB_CDC_ENABLED 1
#ifndef CONFIG_USB_CDC_ENABLED
#   define CONFIG_USB_CDC_ENABLED
#define CFG_TUD_CDC 1
#endif

}

void TinyUSB::addMSCdescriptor()
{
// #define CONFIG_USB_MSC_ENABLED
}

void TinyUSB::addMIDIdescriptor()
{
// #define CONFIG_USB_MIDI_ENABLED
}

void TinyUSB::addHIDdescriptor()
{
// #define CONFIG_USB_HID_ENABLED
}
